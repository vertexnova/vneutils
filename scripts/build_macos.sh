#!/bin/bash
# ----------------------------------------------------------------------
# Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
# ----------------------------------------------------------------------

set -e

BUILD_TYPE="Debug"
BUILD_TESTS="ON"
CLEAN_BUILD=0

while [[ $# -gt 0 ]]; do
    case $1 in
        --release) BUILD_TYPE="Release"; shift ;;
        --debug) BUILD_TYPE="Debug"; shift ;;
        --no-tests) BUILD_TESTS="OFF"; shift ;;
        --clean) CLEAN_BUILD=1; shift ;;
        --help)
            echo "Usage: $0 [--release|--debug] [--no-tests] [--clean]"
            exit 0
            ;;
        *) echo "Unknown option: $1"; exit 1 ;;
    esac
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build/$BUILD_TYPE"

echo "=== VneUtils Build Script ==="
echo "Build Type: $BUILD_TYPE"
echo "Build Tests: $BUILD_TESTS"

if [ $CLEAN_BUILD -eq 1 ]; then
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"

cmake -B "$BUILD_DIR" -S "$PROJECT_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DBUILD_TESTS="$BUILD_TESTS"

cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" --parallel

echo "=== Build Complete ==="
