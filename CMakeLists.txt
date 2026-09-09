cmake_minimum_required(VERSION 3.28)
project(Kiln VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")


if(WIN32)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)

    if(MSVC)
        add_compile_options(
                /utf-8
                /Zc:__cplusplus
                /permissive-
        )
    endif()
elseif(APPLE)

elseif(UNIX AND NOT APPLE)

endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)

add_subdirectory(source)
add_subdirectory(standalone)

enable_testing()
add_subdirectory(tests)

target_include_directories(kiln_core PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Conduit/include")
target_include_directories(kiln_main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Conduit/include")
target_include_directories(kiln_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../Conduit/include")

set_project_warnings(kiln_core)
set_project_warnings(kiln_main)
set_project_warnings(kiln_tests)