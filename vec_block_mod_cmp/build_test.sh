#!/bin/bash

gcc -g -o vector_test vector_test.c ./lib/libhs.a -lstdc++ -lm -I./hs
