cmake_minimum_required(VERSION 3.12)

project(BenchmarkSuite)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.5
)
FetchContent_MakeAvailable(googlebenchmark)

file(GLOB subdirectories ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(subdirectory ${subdirectories})
    if(IS_DIRECTORY ${subdirectory})
        add_subdirectory(${subdirectory})
    endif()
endforeach()
