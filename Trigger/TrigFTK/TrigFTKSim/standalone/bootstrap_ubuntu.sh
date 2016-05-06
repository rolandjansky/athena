#!/bin/sh
# Tested on Ubuntu 15.10
# sudo apt-get install libeigen3-dev libboost-all-dev

mkdir -p tmp/tsp

if [ ! -h TrigFTKSim ]; then
    ln -s ../TrigFTKSim
fi

if [ ! -h src ]; then
    ln -s ../src
fi

export EIGEN_CXXFLAGS=`pkg-config --cflags eigen3`
export BOOST_LDFLAGS="-lboost_system -lboost_iostreams -lboost_program_options -lboost_filesystem"
export BOOST_CXXFLAGS="-I/usr/include/boost"
export LD_LIBRARY_PATH=${PWD}:${LD_LIBRARY_PATH}
export CXXFLAGS="-DHAVE_BOOST_IOSTREAMS -O2"
export PATH=${PWD}:${PATH}
