#!/bin/bash

# Script to build and run tests for simple-x86-simulator
# Run from tests/ directory

set -e  # Exit on error

echo "Building tests..."
mkdir -p build
cd build
cmake ..
make -j$(nproc)

echo "Running tests..."
./test_cpu

echo "All tests passed!"
cd ..
rm -rf build  # Optional cleanup