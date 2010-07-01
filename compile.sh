#!/bin/sh
clang -g loop.cpp `llvm-config --cppflags --ldflags --libs core` -o loop

