#!/bin/sh

echo -n "tmp/ "
mkdir -p tmp/tsp
if [ ! -d tmp ]; then
    echo "created"
else
    echo "exists"
fi

echo -n "TrigFTKSim/ (link) "
if [ ! -h TrigFTKSim ]; then
    ln -s ../TrigFTKSim
    echo "created"
else
    echo "exists"
fi

echo -n "src/ (link) "
if [ ! -h src ]; then
    ln -s ../src
    echo "created"
else
    echo "exists"
fi

source ../grid/dependencies.sh
export LD_LIBRARY_PATH=${PWD}:${LD_LIBRARY_PATH}

if [ "$BOOSTDIR" != "" ]; then
    BOOST_GCC_VER=`ls $BOOSTDIR/lib/libboost_iostreams-gcc*-mt*a | sed -e "s,.*gcc,gcc," | sed -e "s,-.*,,"`;
fi

echo
echo dcap and boost configuration is
echo
echo export BOOSTDIR=$BOOSTDIR
echo export BOOST_VER=$BOOST_VER
echo export BOOST_GCC_VER=$BOOST_GCC_VER
echo export DCAPDIR=$DCAPDIR
echo
echo make -f Makefile.boost.mk

