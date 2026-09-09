//------------------------------------------------//
// Copyright (c) 2026 Nebula-Ray42.               //
// SPDX-License-Identifier: BSD-2-Clause-Patent   //
//------------------------------------------------//

#include <iostream>
#include <fstream>
#include <vector>
#include <span>

#include "conduit/mesh_types.h"
#include "conduit/parser.h"
#include "mesh/mesh_processor.h"

int main() {
    std::cout << "=== [Kiln] 変換とゼロコピー読み込みの統合テスト開始 ===" << std::endl;
    const std::string test_filepath = "test_output.bin";

    // テスト用のダミーデータ
    kiln::mesh::ProcessedMesh dummy_mesh;
    dummy_mesh.positions.push_back({1.0f, 2.0f, 3.0f});
    dummy_mesh.positions.push_back({4.0f, 5.0f, 6.0f});
    dummy_mesh.positions.push_back({7.0f, 8.0f, 9.0f});
    dummy_mesh.indices = std::vector<uint16_t>{0, 1, 2}; // 16bitのインデックス

    auto pack_result = kiln::mesh::pack_to_binary(dummy_mesh);
    if (!pack_result.has_value()) {
        std::cerr << "[!] パック処理に失敗しました: " << pack_result.error() << std::endl;
        return 1;
    }

    std::ofstream outfile(test_filepath, std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(pack_result.value().data()), static_cast<std::streamsize>(pack_result.value().size()));
    outfile.close();
    std::cout << "[*] 新規格のバイナリを保存しました (" << pack_result.value().size() << " bytes)" << std::endl;

    std::ifstream infile(test_filepath, std::ios::binary);
    infile.seekg(0, std::ios::end);
    size_t length = static_cast<size_t>(infile.tellg());
    infile.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(length);
    infile.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(length));
    infile.close();

    auto header_result = conduit::verify_and_get_header(buffer);
    if (!header_result.has_value()) {
        std::cerr << "[!] 読み込みエラー: " << header_result.error() << std::endl;
        return 1;
    }

    const conduit::MeshHeader* header = header_result.value();
    std::cout << "[*] 読み込み成功 頂点数: " << header->vertex_count << std::endl;

    const auto* positions = reinterpret_cast<const conduit::Position*>(buffer.data() + header->positions_offset);
    for (uint32_t i = 0; i < header->vertex_count; ++i) {
        const auto& v = positions[i];
        std::cout << "    頂点 " << i << ": (" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
    }

    std::remove(test_filepath.c_str());

    std::cout << "=== [Kiln] テスト完了 ===" << std::endl;
    return 0;
}
