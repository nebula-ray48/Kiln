//------------------------------------------------//
// Copyright (c) 2026 Nebula-Ray42.               //
// SPDX-License-Identifier: BSD-2-Clause-Patent   //
//------------------------------------------------//

#include "mesh/mesh_processor.h"

#include <algorithm>
#include <expected>
#include <string>

namespace kiln::mesh {

std::expected<ProcessedMesh, std::string> process_mesh(const MeshData& raw_mesh) noexcept {

    if (auto valid = validate_mesh(raw_mesh); not valid.has_value()) {
        return std::unexpected(valid.error());
    }

    // 生データを取り出す
    const auto& raw_pos = raw_mesh.get_positions();
    const auto& raw_uvs = raw_mesh.get_uvs();
    const auto& raw_normals = raw_mesh.get_normals();

    const size_t vertex_count = raw_pos.size() / 3;

    ProcessedMesh result;
    // メモリの再確保を防ぐため、最初に必要な分だけ確保しておく
    result.positions.reserve(vertex_count);
    result.attributes.reserve(vertex_count);

    // 2. データをエンジン用の「リスト1」と「リスト2」に詰め替える
    for (size_t i = 0; i < vertex_count; ++i) {
        // リスト1: 位置だけを純度100%で抽出
        result.positions.push_back(conduit::Position{
            raw_pos[i * 3 + 0],
            raw_pos[i * 3 + 1],
            raw_pos[i * 3 + 2],
        });

        // リスト2: 色塗りに必要な情報をまとめる（インターリーブ）
        conduit::VertexAttribute attr{};
        if (not raw_uvs.empty()) {
            attr.u = raw_uvs[i * 2 + 0];
            attr.v = raw_uvs[i * 2 + 1];
        }
        if (not raw_normals.empty()) {
            attr.nx = raw_normals[i * 3 + 0];
            attr.ny = raw_normals[i * 3 + 1];
            attr.nz = raw_normals[i * 3 + 2];
        }

        result.attributes.push_back(attr);
    }

    // 3. インデックスデータのハイブリッド処理 (16bit / 32bit)
    const uint32_t index_type = raw_mesh.indices_component_type;

    if (index_type == 5123) { // 16bit (UNSIGNED_SHORT)
        const auto raw_idx = raw_mesh.get_indices_u16();
        result.indices = std::vector<uint16_t>(raw_idx.begin(), raw_idx.end());
    }
    else if (index_type == 5125) { // 32bit (UNSIGNED_INT)
        const auto raw_idx = raw_mesh.get_indices_u32();
        result.indices = std::vector<uint32_t>(raw_idx.begin(), raw_idx.end());
    }
    else if (raw_mesh.indices_count > 0) {
        return std::unexpected("エラー: サポートされていないインデックス形式です");
    }

    return result;
}

std::expected<void, std::string> validate_mesh(const MeshData& mesh) noexcept {

    const auto positions = mesh.get_positions();
    const auto uvs = mesh.get_uvs();

    if (positions.empty()) {
        return std::unexpected("エラー: 頂点データ(positions)が空っぽです");
    }

    const size_t vertex_count = positions.size() / 3;

    if (not uvs.empty()) {
        const size_t uv_count = uvs.size() / 2;
        if (vertex_count != uv_count) {
            return std::unexpected("エラー: 頂点の数とUVの数が一致しません");
        }
    }

    // インデックスの最大値チェックも16bit/32bitで分岐させて安全に読み込む
    const uint32_t index_type = mesh.indices_component_type;
    size_t max_index = 0;

    if (index_type == 5123) {
        const auto indices = mesh.get_indices_u16();
        if (not indices.empty()) max_index = std::ranges::max(indices);
    }
    else if (index_type == 5125) {
        const auto indices = mesh.get_indices_u32();
        if (not indices.empty()) max_index = std::ranges::max(indices);
    }

    if (mesh.indices_count > 0 && max_index >= vertex_count) {
        return std::unexpected("エラー: 存在しない頂点を指しているインデックスがあります");
    }

    return {};
}

std::expected<std::vector<uint8_t>, std::string> pack_to_binary(const ProcessedMesh& mesh) noexcept {

    // 各データが占有するバイト数を計算する
    const size_t total_vertex_positions_bytes = mesh.positions.size() * sizeof(conduit::Position);
    const size_t total_attribute_bytes = mesh.attributes.size() * sizeof(conduit::VertexAttribute);

    size_t index_bytes = 0;
    size_t index_count = 0;

    // 16bitか32bitかを確認し、実際に中身を取り出してバイト数を計算する
    if (std::holds_alternative<std::vector<uint16_t>>(mesh.indices)) {
        const auto& indices_16 = std::get<std::vector<uint16_t>>(mesh.indices);
        index_count = indices_16.size();
        index_bytes = index_count * sizeof(uint16_t);
    } else {
        const auto& indices_32 = std::get<std::vector<uint32_t>>(mesh.indices);
        index_count = indices_32.size();
        index_bytes = index_count * sizeof(uint32_t);
    }

    // 最終的な「ファイル全体のサイズ」を割り出す
    const size_t total_bytes = sizeof(conduit::MeshHeader) + total_vertex_positions_bytes + total_attribute_bytes + index_bytes;

    // 計算した総サイズの分だけ、空のバッファを作る
    std::vector<uint8_t> buffer(total_bytes);

    // 目次（MeshHeader）を組み立てる
    conduit::MeshHeader header{};
    header.magic = conduit::CONDUIT_MAGIC;
    header.version = 1;
    header.vertex_count = static_cast<uint32_t>(mesh.positions.size());
    header.index_count = static_cast<uint32_t>(index_count);

    // オフセットの計算
    header.positions_offset = sizeof(conduit::MeshHeader);
    header.attributes_offset = header.positions_offset + total_vertex_positions_bytes;
    header.indices_offset = header.attributes_offset + total_attribute_bytes;

    // バッファの指定位置に、データを直接流し込む (std::memcpy)
    // 1. バッファの先頭に、ヘッダー（40バイト）をコピー
    std::memcpy(buffer.data(), &header, sizeof(conduit::MeshHeader));

    // 2. positions_offset の位置から、座標データをコピー
    std::memcpy(buffer.data() + header.positions_offset, mesh.positions.data(), total_vertex_positions_bytes);

    // 3. attributes_offset の位置から、属性データをコピー
    std::memcpy(buffer.data() + header.attributes_offset, mesh.attributes.data(), total_attribute_bytes);

    // 4. indices_offset の位置から、インデックスデータをコピー
    if (std::holds_alternative<std::vector<uint16_t>>(mesh.indices)) {
        const auto& indices_16 = std::get<std::vector<uint16_t>>(mesh.indices);
        std::memcpy(buffer.data() + header.indices_offset, indices_16.data(), index_bytes);
    } else {
        const auto& indices_32 = std::get<std::vector<uint32_t>>(mesh.indices);
        std::memcpy(buffer.data() + header.indices_offset, indices_32.data(), index_bytes);
    }

    return buffer;
}

} // namespace kiln::mesh