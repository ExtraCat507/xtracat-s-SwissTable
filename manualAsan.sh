#!/bin/bash

cmake -B build-asan -G "MinGW Makefiles"  -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug -DSANITIZE_ADDRESS=ON
cmake --build build-asan
ctest --test-dir build-asan --output-on-failure
read
