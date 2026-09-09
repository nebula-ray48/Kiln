find_package(nlohmann_json CONFIG REQUIRED)


add_library(kiln_core STATIC
        baker_core.cpp
        ../include/mesh/mesh_types.h
        ../include/format/gltf_parser.h
        mesh/mesh_processor.cpp
        ../include/mesh/mesh_processor.h
        format/gltf_parser.cpp
        ../include/core/file_io.h
        core/file_io.cpp
)


target_include_directories(kiln_core PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(kiln_core PRIVATE nlohmann_json::nlohmann_json)