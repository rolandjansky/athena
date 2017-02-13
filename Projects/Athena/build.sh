#!/bin/bash
#
# Script for building the release on top of externals built using one of the
# scripts in this directory.
#

# Function printing the usage information for the script
usage() {
    echo "Usage: build.sh [-t build type] [-b build dir] [-c] [-m] [-i] [-p]"
    echo " -c: Execute CMake step"
    echo " -m: Execute make step"
    echo " -i: Execute install step"
    echo " -p: Execute CPack step"
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
while getopts ":t:b:hcmip" opt; do
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

if [ -z "$EXE_CMAKE" -a -z "$EXE_MAKE" -a -z "$EXE_INSTALL" -a -z "$EXE_CPACK" ]; then
    EXE_CMAKE="1"
    EXE_MAKE="1"
    EXE_INSTALL="1"
    EXE_CPACK="1"
fi

# Stop on errors from here on out:
set -e

# Source in our environment
AthenaSrcDir=$(dirname ${BASH_SOURCE[0]})
if [ -z "$BUILDDIR" ]; then
    BUILDDIR=${AthenaSrcDir}/../../../build
fi
mkdir -p ${BUILDDIR}
BUILDDIR=$(cd ${BUILDDIR} && pwd)
source $AthenaSrcDir/build_env.sh -b $BUILDDIR

# CMake:
if [ -n "$EXE_CMAKE" ]; then
    mkdir -p ${BUILDDIR}/build/Athena
    cd ${BUILDDIR}/build/Athena
    time cmake -DCMAKE_BUILD_TYPE:STRING=${BUILDTYPE} \
	-DCTEST_USE_LAUNCHERS:BOOL=TRUE \
	${AthenaSrcDir} 2>&1 | tee cmake_config.log
fi

# At this point stop worrying about errors:
set +e

# make:
if [ -n "$EXE_MAKE" ]; then
    time make -k
fi

# Install the results:
if [ -n "$EXE_INSTALL" ]; then
    time make install/fast \
	DESTDIR=${BUILDDIR}/install/Athena/${NICOS_PROJECT_VERSION}
fi

# Build an RPM for the release:
if [ -n "$EXE_CPACK" ]; then
    time cpack
    cp Athena*.rpm ${BUILDDIR}/
fi
