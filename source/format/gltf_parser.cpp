//------------------------------------------------//
// Copyright (c) 2026 Nebula-Ray42.               //
// SPDX-License-Identifier: BSD-2-Clause-Patent   //
//------------------------------------------------//

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#include "format/gltf_parser.h"
#include "core/file_io.h"

#include <iostream>
#include <filesystem>
#include <memory>

namespace kiln::format {
std::expected<mesh::MeshData, std::string> parse_gltf(std::string_view filepath) noexcept {
    cgltf_options options = {};
    cgltf_data*   data    = nullptr;
    cgltf_result  result  = cgltf_parse_file(&options, filepath.data(), &data);

    if (result != cgltf_result_success) {
        return std::unexpected("エラー: glTFファイルの読み込みに失敗しました");
    }

    std::unique_ptr<cgltf_data, decltype(&cgltf_free)> safe_data(data, cgltf_free);

    if (data->meshes_count == 0 || data->meshes[0].primitives_count == 0) {
        return std::unexpected("エラー: 形状データが見つかりません");
    }

    const cgltf_primitive& primitive = data->meshes[0].primitives[0];

    mesh::MeshData mesh_data{};
    std::string    bin_filename = "";

    if (primitive.indices != nullptr) {
        const cgltf_accessor* acc = primitive.indices;
        // accessorのズレ ＋ buffer_viewのズレ ＝ 実際のデータの開始位置
        mesh_data.indices_byte_offset = acc->offset + acc->buffer_view->offset;
        mesh_data.indices_count       = acc->count;

        if (acc->component_type == cgltf_component_type_r_16u) {
            mesh_data.indices_component_type = 5123;
        } else if (acc->component_type == cgltf_component_type_r_32u) {
            mesh_data.indices_component_type = 5125;
        } else {
            mesh_data.indices_component_type = 0;
        }
    }

        // 頂点や法線などの情報を抜き出す
        for (size_t i = 0; i < primitive.attributes_count; ++i) {
            const cgltf_attribute& attr            = primitive.attributes[i];
            const cgltf_accessor*  acc             = attr.data;
            const size_t           absolute_offset = acc->offset + acc->buffer_view->offset;

            if (attr.type == cgltf_attribute_type_position) {
                mesh_data.positions_byte_offset = absolute_offset;
                mesh_data.positions_float_count = acc->count * 3;  // x,y,z なので3倍
                bin_filename                    = acc->buffer_view->buffer->uri;
            } else if (attr.type == cgltf_attribute_type_normal) {
                mesh_data.normals_byte_offset = absolute_offset;
                mesh_data.normals_float_count = acc->count * 3;
            } else if (attr.type == cgltf_attribute_type_texcoord) {
                mesh_data.uvs_byte_offset = absolute_offset;
                mesh_data.uvs_float_count = acc->count * 2;
            }
        }

        if (mesh_data.positions_float_count == 0) {
            return std::unexpected("エラー: 必須である POSITION が存在しません");
        }

        std::filesystem::path gltf_dir = std::filesystem::path(filepath).parent_path();
        std::filesystem::path bin_path = gltf_dir / bin_filename;

        auto binary_result = kiln::core::read_binary_file(bin_path.string());
        if (not binary_result.has_value()) {
            return std::unexpected(binary_result.error());
        }
        mesh_data.raw_buffer = std::move(binary_result.value());

        std::cout << "[SUCCESS] glTFの読み込みとメモリ構造の構築に成功しました\n";
        return mesh_data;
    }

} // namespace kiln::format