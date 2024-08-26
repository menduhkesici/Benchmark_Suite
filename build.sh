SOURCE_FOLDER="./src"
BUILD_FOLDER_ROOT="./build"

for BUILD_TYPE in "Debug" "Release" "RelWithDebInfo"; do
    BUILD_FOLDER="${BUILD_FOLDER_ROOT}/${BUILD_TYPE}"
    mkdir -p $BUILD_FOLDER
    cmake -S $SOURCE_FOLDER -B $BUILD_FOLDER -DCMAKE_BUILD_TYPE=$BUILD_TYPE
    make -C $BUILD_FOLDER --no-print-directory
done
