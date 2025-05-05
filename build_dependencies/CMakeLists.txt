cmake_minimum_required(VERSION 3.12)

project(BuildDependencies)

include(FetchContent)

# Google Benchmark
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY  https://github.com/google/benchmark.git
    GIT_TAG         v1.9.2
)
FetchContent_MakeAvailable(googlebenchmark)

# GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         release-1.12.1
)
FetchContent_MakeAvailable(googletest)
