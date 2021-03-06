#!/bin/bash

#yum install libpcap-devel

gcc -g -o simplegrep simplegrep.c ./lib/libhs.a -lstdc++ -lm -I./hs

gcc -g -o simplegrep_multi simplegrep_multi.c ./lib/libhs.a -lstdc++ -lm -I./hs

gcc -g -o simplegrep_multi_vector simplegrep_multi_vector.c ./lib/libhs.a -lstdc++ -lm -I./hs

g++ -g -o pcapscan  examples/pcapscan.cc ./lib/libhs.a -lm -I./hs -std=gnu++11 -lpcap
