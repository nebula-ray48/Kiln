//------------------------------------------------//
// Copyright (c) 2026 Nebula-Ray42.               //
// SPDX-License-Identifier: BSD-2-Clause-Patent   //
//------------------------------------------------//

#pragma once
#include <expected>
#include <string>

#include "conduit/mesh_types.h"
#include "mesh_types.h"

namespace kiln::mesh {

[[nodiscard]] std::expected<void, std::string> validate_mesh(const MeshData& mesh) noexcept;

[[nodiscard]] std::expected<MeshData, std::string> optimize_mesh(MeshData mesh) noexcept;

[[nodiscard]] std::expected<ProcessedMesh, std::string> process_mesh(const MeshData& raw_mesh) noexcept;

[[nodiscard]] std::expected<std::vector<uint8_t>, std::string> pack_to_binary(const ProcessedMesh& mesh) noexcept;

}  // namespace kiln::mesh