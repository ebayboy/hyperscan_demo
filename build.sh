#!/bin/bash


gcc -g -o simplegrep simplegrep.c ./lib/libhs.a -lstdc++ -lm -I./include
