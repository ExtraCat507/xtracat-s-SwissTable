#!/bin/bash

cd benchmark
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_TESTING=OFF -DBUILD_SHARED_LIBS=OFF -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
cmake --build build

cd ..
clang++ googlebenchmark.cpp -I benchmark/include benchmark/build/src/libbenchmark.a -O3 -march=native -lpthread -lshlwapi -DBENCHMARK_STATIC_DEFINE -o googlebench


echo "Compiled to googlebench.exe"
echo "All good, press enter..."

read
