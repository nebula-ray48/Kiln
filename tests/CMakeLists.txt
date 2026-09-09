add_executable(kiln_tests
        baker_test.cpp
)

find_package(flatbuffers CONFIG REQUIRED)

add_test(NAME KilnCoreTest COMMAND kiln_tests)