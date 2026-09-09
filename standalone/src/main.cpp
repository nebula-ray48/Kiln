//------------------------------------------------//
// Copyright (c) 2026 Nebula-Ray42.               //
// SPDX-License-Identifier: BSD-2-Clause-Patent   //
//------------------------------------------------//

#include <fstream>
#include <iostream>

#include "format/gltf_parser.h"
#include "mesh/mesh_processor.h"

int main() {
    auto result = kiln::format::parse_gltf("/Users/sakakibarayuto/Kiln/tests/assets/Box.gltf");

    if (not result.has_value()) {
        std::cerr << "失敗: " << result.error() << "\n";
        return 1;
    }

    const auto& mesh_data = result.value();
    std::cout << "\n=== MeshData 抽出レポート ===\n";

    auto positions = mesh_data.get_positions();
    std::cout << "頂点数: " << (positions.size() / 3) << " (合計 " << positions.size() << " floats)\n";
    if (positions.size() >= 3) {
        std::cout << "  最初の頂点座標 -> X: " << positions[0]
                  << ", Y: " << positions[1]
                  << ", Z: " << positions[2] << "\n";
    }

    auto normals = mesh_data.get_normals();
    std::cout << "法線数: " << (normals.size() / 3) << " (合計 " << normals.size() << " floats)\n";
    if (normals.size() >= 3) {
        std::cout << "  最初の法線ベクトル -> X: " << normals[0]
                  << ", Y: " << normals[1]
                  << ", Z: " << normals[2] << "\n";
    }

    auto uvs = mesh_data.get_uvs();
    std::cout << "UV数: " << (uvs.size() / 2) << " (合計 " << uvs.size() << " floats)\n";

    auto indices_16 = mesh_data.get_indices_u16();
    auto indices_32 = mesh_data.get_indices_u32();

    if (not indices_16.empty()) {
        std::cout << "インデックス数 (uint16_t): " << indices_16.size() << "\n";
        if (indices_16.size() >= 3) {
            std::cout << "  最初の三角形 -> " << indices_16[0] << ", "
                      << indices_16[1] << ", " << indices_16[2] << "\n";
        }
    } else if (not indices_32.empty()) {
        std::cout << "インデックス数 (uint32_t): " << indices_32.size() << "\n";
    } else {
        std::cout << "インデックスデータ: なし\n";
    }

    std::cout << "=================================\n";

    std::cout << "\n=== バイナリ形式へのベイク処理開始 ===\n";
    auto process_result = kiln::mesh::process_mesh(mesh_data);
    if (not process_result.has_value()) {
        std::cerr << "プロセス失敗: " << process_result.error() << "\n";
        return 1;
    }

    auto pack_result = kiln::mesh::pack_to_binary(process_result.value());
    if (not pack_result.has_value()) {
        std::cerr << "パック失敗: " << pack_result.error() << "\n";
        return 1;
    }

    const std::string out_path = "/Users/sakakibarayuto/Kiln/tests/assets/Box_baked.bin";
    std::ofstream outfile(out_path, std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(pack_result.value().data()), static_cast<std::streamsize>(pack_result.value().size()));
    outfile.close();

    std::cout << "[SUCCESS] ベイク完了! \n保存先: " << out_path << "\nサイズ: " << pack_result.value().size() << " bytes\n";

    return 0;
}
