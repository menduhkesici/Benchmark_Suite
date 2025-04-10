#!/bin/bash
set -e

EXE_NAME="$1"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
valgrind --tool=callgrind --callgrind-out-file="${SCRIPT_DIR}/build/callgrind-${EXE_NAME}.out" "${SCRIPT_DIR}/build/RelWithDebInfo/bin/${EXE_NAME}"
