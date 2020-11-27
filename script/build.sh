#!/bin/bash

mkdir -p cmake_build && \
rm -rf cmake_build/* && \
cd cmake_build && \
cmake -DCMAKE_BUILD_TYPE=Debug .. && \
make -j 2 && \
cd ..

