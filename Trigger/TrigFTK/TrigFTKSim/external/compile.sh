#!/bin/bash

skip_compile_dcap=$1

# Extracts libdcap, compiles it, and sets DCAP_LDPRELOAD to libpdcap.so
# if skip_compile_dcap is set, just sets DCAP_LDPRELOAD
# Usage: source compile.sh while inside TrigFTKSim/external

dcap_version=dcap-2.47.2
dcap_archive=${dcap_version}.tar.bz2

if [ ! -n "${skip_compile_dcap}" ]; then
    tar xvfj $dcap_archive
    if [ ! -d ${dcap_version} ]; then
	echo "ERROR: cannot find dcap folder ${dcap_version}"
	ls -l
	exit 3
    fi;

    cd ${dcap_version}
    ./configure
    make
    cd ../
fi;

cd ${dcap_version}
export DCAP_LDPRELOAD=`pwd`/src/.libs/libpdcap.so
if [ -f "${DCAP_LDPRELOAD}" ]; then
    echo "Libdcap compilation successful!"
    file -L ${DCAP_LDPRELOAD}
    file -L $(which tar)
    echo "DCAP_LDPRELOAD=${DCAP_LDPRELOAD}"
else
    echo "ERROR: failed to compile DCAP_LDPRELOAD"
    exit 3
fi;
