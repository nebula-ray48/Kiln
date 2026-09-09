add_executable(kiln_tests
        baker_test.cpp
)

target_sources(kiln_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../Conduit/source/conduit.cpp")

target_link_libraries(kiln_tests PRIVATE kiln_core)

add_test(NAME KilnCoreTest COMMAND kiln_tests)