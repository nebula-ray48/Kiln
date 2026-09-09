//------------------------------------------------//
// Copyright (c) 2026 Nebula-Ray42.               //
// SPDX-License-Identifier: BSD-2-Clause-Patent   //
//------------------------------------------------//

#include <iostream>
#include <fstream>
#include <vector>

#include "mesh_generated.h"

int main() {
    std::cout << "=== [Kiln] ゼロコピー読み込みテスト開始 ===" << std::endl;

    std::ifstream infile("data/test_output.bin", std::ios::binary);

    if (!infile) {
        infile.open("../../data/test_output.bin", std::ios::binary);
    }

    if (!infile) {
        std::cerr << "エラー: test_output.bin が見つかりません。" << std::endl;
        return 1;
    }

    infile.seekg(0, std::ios::end);
    size_t length = static_cast<size_t>(infile.tellg());
    infile.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(length);

    infile.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(length));
    infile.close();

    auto mesh = kiln::schema::GetMeshData(buffer.data());
    auto positions = mesh->positions();

    if (positions) {
        std::cout << "[*] 読み込み成功 頂点数: " << positions->size() << std::endl;

        for (uint32_t i = 0; i < positions->size(); ++i) {
            auto v = positions->Get(i);

            std::cout << "    頂点 " << i << ": ("
                      << v->x() << ", "
                      << v->y() << ", "
                      << v->z() << ")" << std::endl;
        }
    } else {
        std::cout << "[!] 頂点データが空です。" << std::endl;
    }

    std::cout << "=== [Kiln] テスト完了 ===" << std::endl;
    return 0;
}