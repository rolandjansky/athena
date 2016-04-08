#!/usr/bin/env bash
#
# Check the given shared libraries for debugging symbols
# Return code: 0  no debugging symbols found
#              1  at least one file contains debugging symbols
#
# Author: Frank Winklmeier
#

if [ $# -eq 0 ]; then
    echo "Syntax: `basename $0` FILE..."
fi

# Ignore these libraries (regexp)
filter='HepMC|libfastjet|libCDFConesPlugin|libSISConePlugin|libsiscone|libulxmlrpcpp|libG4processes|libModelInteractions|libCascade_i|libMCTester_i|
|libColMan|libATLASConePlugin|libCMSIterativeConePlugin|libD0RunIIConePlugin|libEECambridgePlugin|libJadePlugin|libNestedDefsPlugin|libPxConePlugin|libTrackJetPlugin|libtbb|libtcmalloc|libfftw3f|libprofiler|libgmock|libgtest'

ignored=''
dbglibs=''
for lib in $@; do
    if [[ $lib =~ $filter ]]; then
        ignored="$ignored $lib"
        continue
    fi
    if [[ ! -f ${lib} ]]; then
        continue
    fi
    readelf -e ${lib} | grep debug_info &> /dev/null
    if [ $? -eq 0 ]; then
        dbglibs="$dbglibs $lib"
    fi
done

if [[ -n $dbglibs ]]; then
    echo "Libraries with debug symbols:"
    echo "------------------------------"
    for l in $dbglibs; do
        echo $l
    done
fi
if [[ -n $ignored ]]; then
    echo
    echo "Ignored the following libraries with debug symbols:"
    echo "----------------------------------------------------"
    for l in $ignored; do
        echo $l
    done
fi

if [[ -n $dbglibs ]]; then
    exit 1
else
    exit 0
fi
