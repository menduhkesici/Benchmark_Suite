cmake_minimum_required(VERSION 3.12)

project(BenchmarkSuite)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Treat compiler warnings as errors
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wshadow -Wuninitialized -Wconversion -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4 /WX)
endif()

find_package(benchmark REQUIRED CONFIG)
find_package(OpenMP REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_UNIT_TESTS ON CACHE INTERNAL "Set to ON for Debug builds")
    find_package(GTest REQUIRED CONFIG)
    enable_testing()
endif()

add_subdirectory(src)
