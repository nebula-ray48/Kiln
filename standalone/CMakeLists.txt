add_executable(kiln_main
        src/main.cpp
)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set_target_properties(kiln_main PROPERTIES UNITY_BUILD ON)

target_sources(kiln_main PRIVATE)

target_include_directories(kiln_main PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(kiln_main PRIVATE
        kiln_core
)