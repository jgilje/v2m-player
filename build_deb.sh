#!/bin/bash

if [ ! -d build_deb ]; then
    mkdir build_deb
fi

cd build_deb
cmake -DCMAKE_BUILD_TYPE=Release -G Ninja ..
ninja
LD_LIBRARY_PATH=`pwd` cpack -G DEB
