#!/bin/bash
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Script for building the release on top of externals built using one of the
# scripts in this directory.
#

# Helper function for timing the various build steps.
_time_() { local c="time -p " ; while test "X$1" != "X" ; do c+=" \"$1\"" ;
           shift; done; ( eval "$c" ) 2>&1 |
           sed "s,^real[[:space:]],time::${c}:: real ," ; }

# Function printing the usage information for the script
usage() {
    echo "Usage: build.sh [-t build type] [-b build dir] [-c] [-m] [-i] [-p] [-a] [-x opt]"
    echo " -c: Execute CMake step"
    echo " -m: Execute make step"
    echo " -i: Execute install step"
    echo " -p: Execute CPack step"
    echo " -a: Abort on error"
    echo " -x: Extra configuration argument(s) for CMake"
    echo ""
    echo "If none of the c, m, i or p options are set then the script will do"
    echo "*all* steps. Otherwise only the enabled steps are run - it's your"
    echo "reponsibility to ensure that precusors are in good shape"
}

# Parse the command line arguments:
BUILDDIR=""
BUILDTYPE="RelWithDebInfo"
EXE_CMAKE=""
EXE_MAKE=""
EXE_INSTALL=""
EXE_CPACK=""
NIGHTLY=true
EXTRACMAKE=()
while getopts ":t:b:hcmipax:" opt; do
    case $opt in
        t)
            BUILDTYPE=$OPTARG
            ;;
        b)
            BUILDDIR=$OPTARG
            ;;
        c)
            EXE_CMAKE="1"
            ;;
        m)
            EXE_MAKE="1"
            ;;
        i)
            EXE_INSTALL="1"
            ;;
        p)
            EXE_CPACK="1"
            ;;
        a)
            NIGHTLY=false
            ;;
        x)
            EXTRACMAKE+=($OPTARG)
            ;;
        h)
            usage
            exit 0
            ;;
        :)
            echo "Argument -$OPTARG requires a parameter!"
            usage
            exit 1
            ;;
        ?)
            echo "Unknown argument: -$OPTARG"
            usage
            exit 1
            ;;
    esac
done

# If no step was explicitly specified, turn them all on:
if [ -z "$EXE_CMAKE" -a -z "$EXE_MAKE" -a -z "$EXE_INSTALL" \
     -a -z "$EXE_CPACK" ]; then
    EXE_CMAKE="1"
    EXE_MAKE="1"
    EXE_INSTALL="1"
    EXE_CPACK="1"
fi

# Stop on errors from here on out:
set -e
# consider a pipe failed if ANY of the commands fails
set -o pipefail

# Source in our environment
AthAnalysisSrcDir=$(dirname ${BASH_SOURCE[0]})
if [ -z "$BUILDDIR" ]; then
    BUILDDIR=${AthAnalysisSrcDir}/../../../build
fi
mkdir -p ${BUILDDIR}
BUILDDIR=$(\cd ${BUILDDIR} && \pwd)
source $AthAnalysisSrcDir/build_env.sh -b $BUILDDIR >& ${BUILDDIR}/build_env.log
cat ${BUILDDIR}/build_env.log

# create the actual build directory
mkdir -p ${BUILDDIR}/build/AthAnalysis

# CMake:
if [ -n "$EXE_CMAKE" ]; then
    # Remove the CMakeCache.txt file, to force CMake to find externals
    # from scratch in an incremental build.
    rm -f ${BUILDDIR}/build/AthAnalysis/CMakeCache.txt
    # Now run the actual CMake configuration:
    { _time_ cmake --graphviz=${BUILDDIR}/build/AthAnalysis/packages.dot \
        -DCMAKE_BUILD_TYPE:STRING=${BUILDTYPE} \
        -DCTEST_USE_LAUNCHERS:BOOL=TRUE \
        ${EXTRACMAKE[@]} \
        -B ${BUILDDIR}/build/AthAnalysis \
        -S ${AthAnalysisSrcDir}; } \
            2>&1 | tee ${BUILDDIR}/build/AthAnalysis/cmake_config.log
fi

# for nightly builds we want to get as far as we can
if [ "$NIGHTLY" = true ]; then
    # At this point stop worrying about errors:
    set +e
fi

# make:
if [ -n "$EXE_MAKE" ]; then
    # Forcibly remove the merged CLID file from the previous build, to
    # avoid issues with some library possibly changing the name/CLID
    # of something during the build.
    rm -f ${BUILDDIR}/build/AthAnalysis/*/share/clid.db
    # Extra argument(s) for the build tool.
    BUILDARGS=()
    if [ "$NIGHTLY" = true ]; then
        if [[ "${EXTRACMAKE[@]}" == *"Ninja"* ]]; then
            BUILDARGS+=(-k0)
        else
            BUILDARGS+=(-k)
        fi
    fi
    # Build the project.
    { _time_ cmake --build ${BUILDDIR}/build/AthAnalysis -- ${BUILDARGS[@]}; } \
        2>&1 | tee ${BUILDDIR}/build/AthAnalysis/cmake_build.log
fi

# Install the results:
if [ -n "$EXE_INSTALL" ]; then
    { DESTDIR=${BUILDDIR}/install _time_ \
      cmake --install ${BUILDDIR}/build/AthAnalysis; } \
	 2>&1 | tee ${BUILDDIR}/build/AthAnalysis/cmake_install.log
fi

# Build an RPM for the release:
if [ -n "$EXE_CPACK" ]; then
    cd ${BUILDDIR}/build/AthAnalysis
    { _time_ cpack; } 2>&1 | tee ${BUILDDIR}/build/AthAnalysis/cmake_cpack.log
    if [ "$BUILDTYPE" = "RelWithDebInfo" ]; then
	    { _time_ cpack --config CPackDbgRPMConfig.cmake; } \
            2>&1 | tee -a ${BUILDDIR}/build/AthAnalysis/cmake_cpack.log
    fi
    cp AthAnalysis*.rpm ${BUILDDIR}/
fi
