#!/bin/bash

BUILD_DOC=OFF # ON or OFF

sudo pacman -Sy --needed sfml 
# For doxygen generation
if [ "$BUILD_DOC" = "ON" ]; then
    sudo pacman -Sy --needed doxygen graphviz
fi

cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_DOC=$BUILD_DOC .
make -j8
