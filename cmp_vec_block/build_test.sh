#!/bin/bash

gcc -std=c99 -g -O2 -o block_test block_test.c ../lib/libhs.a -lstdc++ -lm -I../hs || exit 1

gcc -std=c99 -g -O2 -o vector_test vector_test.c ../lib/libhs.a -lstdc++ -lm -I../hs || exit 1

