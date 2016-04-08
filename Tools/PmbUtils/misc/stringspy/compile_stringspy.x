#!/bin/bash
#This compiles with TString support and without very VERBOSE output
#(see the start of stringspy.cxx for more details)
g++ -Wall -fPIC `root-config --cflags` -c stringspy.cxx && \
g++  -shared -Wl,-soname,libstringspy.so stringspy.o -ldl `root-config --ldflags --libs` -o libstringspy.so
