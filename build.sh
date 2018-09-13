#!/bin/bash


gcc -o simplegrep simplegrep.c ./lib/libhs.a -lstdc++ -lm -I./include
