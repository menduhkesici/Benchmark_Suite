cmake_minimum_required(VERSION 3.12)

project(BenchmarkSuite)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(benchmark REQUIRED CONFIG)
find_package(GTest REQUIRED CONFIG)
find_package(OpenMP REQUIRED)

file(GLOB subdirectories ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(subdirectory ${subdirectories})
    if(IS_DIRECTORY ${subdirectory})
        add_subdirectory(${subdirectory})
    endif()
endforeach()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_UNIT_TESTS ON CACHE INTERNAL "Set to ON for Debug builds")
    enable_testing()
endif()
