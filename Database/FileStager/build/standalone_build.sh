#!/bin/sh

if [ $# -eq 0 ]; then
    echo "No additional options given, using defaults"
else
    echo "Running ./configure $*"
fi

sleep 1

if [ x$ROOTSYS = x ]; then
    echo "ROOTSYS not set, exiting"
    exit 1
fi

if [ ! -x $ROOTSYS/bin/root-config ]; then
    echo "root-config not found, exiting"
    exit 1
fi

# set CFLAGS in order to get proper machine type for libtool
export CFLAGS=`root-config --cflags`

./bootstrap.sh || exit 1

mkdir build

echo "Running ./configure $@"

if [ x`uname -s` = xLinux ]; then
    NCPUS=`grep -c processor /proc/cpuinfo`
elif [ x`uname -s` = xDarwin ]; then
    NCPUS=`sysctl -n hw.ncpu`
fi

if [ ! -z $NCPUS ]; then
    echo "Detected ${NCPUS} cpus, using make -j${NCPUS}"
    MAKEFLAGS="-j${NCPUS}"
    sleep 1
fi

(cd build && ../configure $@ && make $MAKEFLAGS && make install)
