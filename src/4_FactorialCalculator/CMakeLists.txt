get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Main library
project(${FOLDER_NAME}-lib)
set(LIBRARY_NAME ${PROJECT_NAME})
add_library(${PROJECT_NAME} STATIC
    include/factorial.hpp
    src/factorial.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)

# Main executable
project(${FOLDER_NAME})
add_executable(${PROJECT_NAME}
    main.cpp
)
target_link_libraries(${PROJECT_NAME}
    ${LIBRARY_NAME}
)

# Benchmark executable
project(${FOLDER_NAME}-benchmark)
add_executable(${PROJECT_NAME}
    benchmark.cpp
)
target_link_libraries(${PROJECT_NAME}
    ${LIBRARY_NAME}
    benchmark::benchmark
)

# Unit-tests executable
if(BUILD_UNIT_TESTS)
    project(${FOLDER_NAME}-unit-tests)
    add_executable(${PROJECT_NAME}
        unit-tests.cpp
    )
    target_link_libraries(${PROJECT_NAME}
        ${LIBRARY_NAME}
        GTest::gtest_main
    )
    add_test(
        NAME    ${PROJECT_NAME}
        COMMAND ${PROJECT_NAME}
    )
endif()
