#!/usr/bin/env bash

if [ -z $ROOTSYS ]; then
    echo "Please set up ROOT to run"
    exit 1
fi 

echo "--------------------------------------------------------"
echo "compiling posprocessing script"
echo "--------------------------------------------------------"
NAME=postprocessHistos_updt
g++ -O2 -Wall -fPIC -std=c++11 $(root-config --cflags) -o ${NAME} ${NAME}.cxx $(root-config --libs) -lboost_program_options

echo "--------------------------------------------------------"
echo "running postprocessing"
echo "--------------------------------------------------------"
./postprocessHistos_updt $@

