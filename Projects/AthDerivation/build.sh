#!/bin/bash
#
# Script for building the release on top of externals built using one of the
# scripts in this directory.
#
_time_() { local c="time -p " ; while test "X$1" != "X" ; do c+=" \"$1\"" ; shift; done; ( eval "$c" ) 2>&1 | sed "s,^real[[:space:]],time::${c}:: real ," ; }

# Function printing the usage information for the script
usage() {
    echo "Usage: build.sh [-t build type] [-b build dir] [-c] [-m] [-i] [-p] [-a]"
    echo " -c: Execute CMake step"
    echo " -m: Execute make step"
    echo " -i: Execute install step"
    echo " -p: Execute CPack step"
    echo " -a: Abort on error"
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
while getopts ":t:b:hcmipa" opt; do
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
if [ -z "$EXE_CMAKE" -a -z "$EXE_MAKE" -a -z "$EXE_INSTALL" -a -z "$EXE_CPACK" ]; then
    EXE_CMAKE="1"
    EXE_MAKE="1"
    EXE_INSTALL="1"
    EXE_CPACK="1"
fi

# Stop on errors from here on out:
set -e

# Source in our environment
AthDerivationSrcDir=$(dirname ${BASH_SOURCE[0]})
if [ -z "$BUILDDIR" ]; then
    BUILDDIR=${AthDerivationSrcDir}/../../../build
fi
mkdir -p ${BUILDDIR}
BUILDDIR=$(cd ${BUILDDIR} && pwd)
source $AthDerivationSrcDir/build_env.sh -b $BUILDDIR >& ${BUILDDIR}/build_env.log
cat  ${BUILDDIR}/build_env.log

# Set Gaudi's version to the same value as this project's version:
export GAUDI_VERSION=`cat ${AthDerivationSrcDir}/version.txt`

# create the actual build directory
mkdir -p ${BUILDDIR}/build/AthDerivation
cd ${BUILDDIR}/build/AthDerivation

# consider a pipe failed if ANY of the commands fails
set -o pipefail

# CMake:
if [ -n "$EXE_CMAKE" ]; then
    # Remove the CMakeCache.txt file, to force CMake to find externals
    # from scratch in an incremental build.
    rm -f CMakeCache.txt
    # Now run the actual CMake configuration:
    _time_ cmake -DCMAKE_BUILD_TYPE:STRING=${BUILDTYPE} \
        -DCTEST_USE_LAUNCHERS:BOOL=TRUE \
        ${AthDerivationSrcDir} 2>&1 | tee cmake_config.log
fi

# for nightly builds we want to get as far as we can
if [ "$NIGHTLY" = true ]; then
    # At this point stop worrying about errors:
    set +e
fi

# Build the project:
if [ -n "$EXE_MAKE" ]; then
    EXTRAOPT=
    if [ -n "$NIGHTLY" ]; then
        EXTRAOPT=-k
    fi
    _time_ make ${EXTRAOPT} 2>&1 | tee cmake_build.log
fi

# Install the results:
if [ -n "$EXE_INSTALL" ]; then
    _time_ make install/fast \
	DESTDIR=${BUILDDIR}/install/AthDerivation/${NICOS_PROJECT_VERSION} 2>&1 | tee cmake_install.log
fi

# Build an RPM for the release:
if [ -n "$EXE_CPACK" ]; then
    _time_ cpack 2>&1 | tee cmake_cpack.log
    cp AthDerivation*.rpm ${BUILDDIR}/
fi
