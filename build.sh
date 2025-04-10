#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_FOLDER="${SCRIPT_DIR}/src"
BUILD_FOLDER_ROOT="${SCRIPT_DIR}/build"

DEPENDENCIES_SRC_FOLDER="${SCRIPT_DIR}/build_dependencies"
DEPENDENCIES_INSTALL_FOLDER="${BUILD_FOLDER_ROOT}/_Dependencies"
DEPENDENCIES_BUILD_FOLDER="${DEPENDENCIES_INSTALL_FOLDER}/build"

# Build and install dependencies in Release mode
mkdir --parents "${DEPENDENCIES_BUILD_FOLDER}"
cmake -S "${DEPENDENCIES_SRC_FOLDER}" -B "${DEPENDENCIES_BUILD_FOLDER}" -DCMAKE_BUILD_TYPE="Release"
cmake --build "${DEPENDENCIES_BUILD_FOLDER}"
cmake --install "${DEPENDENCIES_BUILD_FOLDER}" --prefix "${DEPENDENCIES_INSTALL_FOLDER}"

# Build the project in multiple modes
for BUILD_TYPE in "Debug" "RelWithDebInfo" "Release"; do
    BUILD_FOLDER="${BUILD_FOLDER_ROOT}/${BUILD_TYPE}"
    mkdir --parents "${BUILD_FOLDER}"
    cmake -S "${SOURCE_FOLDER}" -B "${BUILD_FOLDER}" -DCMAKE_INSTALL_PREFIX="${DEPENDENCIES_INSTALL_FOLDER}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    cmake --build "${BUILD_FOLDER}"
done
