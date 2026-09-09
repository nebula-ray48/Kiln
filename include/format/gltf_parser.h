//------------------------------------------------//
// Copyright (c) 2026 Nebula-Ray42.               //
// SPDX-License-Identifier: BSD-2-Clause-Patent   //
//------------------------------------------------//

#pragma once
#include <expected>
#include <filesystem>

#include "mesh/mesh_types.h"

namespace kiln::format {

std::expected<kiln::mesh::MeshData, std::string> parse_gltf(std::string_view filepath) noexcept;

}  // namespace kiln::format