//------------------------------------------------//
// Copyright (c) 2026 Nebula-Ray42.               //
// SPDX-License-Identifier: BSD-2-Clause-Patent   //
//------------------------------------------------//

#pragma once
#include <vector>
#include <variant>
#include <span>
#include <cstdint>

#include "conduit/mesh_types.h"

namespace kiln::mesh {

struct ProcessedMesh {
    std::vector<conduit::Position> positions;
    std::vector<conduit::VertexAttribute> attributes;
    std::variant<std::vector<uint16_t>, std::vector<uint32_t>> indices;
};

// glTFから読み込んだ入力側の生データ
struct MeshData {

    std::vector<std::byte> raw_buffer;

    size_t positions_byte_offset = 0;
    size_t positions_float_count = 0;

    size_t normals_byte_offset = 0;
    size_t normals_float_count = 0;

    size_t uvs_byte_offset = 0;
    size_t uvs_float_count = 0;

    size_t indices_byte_offset = 0;
    size_t indices_count = 0;
    uint32_t indices_component_type = 0;

    [[nodiscard]] std::span<const float> get_positions() const noexcept {
        if (positions_float_count == 0) return {};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        const auto* ptr = reinterpret_cast<const float*>(raw_buffer.data() + positions_byte_offset);
        return {ptr, positions_float_count};
    }

    [[nodiscard]] std::span<const float> get_normals() const noexcept {
        if (normals_float_count == 0) return {};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        const auto* ptr = reinterpret_cast<const float*>(raw_buffer.data() + normals_byte_offset);
        return {ptr, normals_float_count};
    }

    [[nodiscard]] std::span<const float> get_uvs() const noexcept {
        if (uvs_float_count == 0) return {};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        const auto* ptr = reinterpret_cast<const float*>(raw_buffer.data() + uvs_byte_offset);
        return {ptr, uvs_float_count};
    }

    [[nodiscard]] std::span<const uint32_t> get_indices() const noexcept {
        if (indices_count == 0) return {};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        const auto* ptr = reinterpret_cast<const uint32_t*>(raw_buffer.data() + indices_byte_offset);
        return {ptr, indices_count};
    }

    [[nodiscard]] std::span<const uint16_t> get_indices_u16() const noexcept {
        if (indices_count == 0 || indices_component_type != 5123) return {};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        const auto* ptr = reinterpret_cast<const uint16_t*>(raw_buffer.data() + indices_byte_offset);
        return {ptr, indices_count};
    }

    [[nodiscard]] std::span<const uint32_t> get_indices_u32() const noexcept {
        if (indices_count == 0 || indices_component_type != 5125) return {};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        const auto* ptr = reinterpret_cast<const uint32_t*>(raw_buffer.data() + indices_byte_offset);
        return {ptr, indices_count};
    }
};

}  // namespace kiln::mesh