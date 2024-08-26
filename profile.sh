EXE_NAME="$1"
valgrind --tool=callgrind --callgrind-out-file=./build/callgrind.out.$EXE_NAME ./build/RelWithDebInfo/bin/$EXE_NAME
