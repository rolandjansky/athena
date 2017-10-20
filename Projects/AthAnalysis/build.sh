#!/bin/bash
#
# Script for building the release on top of externals built using the
# script in this directory.
#

# Function printing the usage information for the script
usage() {
    echo "Usage: build.sh [-t type] [-b dir] [-g generator] [-c] [-m] [-i] [-p] [-a]"
    echo ""
    echo "  General flags:"
    echo "    -t: The (optional) CMake build type to use."
    echo "    -b: The (optional) build directory to use."
    echo "    -g: The (optional) CMake generator to use."
    echo "    -a: Abort on error."
    echo "  Build step selection:"
    echo "    -c: Execute the CMake step."
    echo "    -m: Execute the make/build step."
    echo "    -i: Execute the install step."
    echo "    -p: Execute the CPack step."
    echo ""
    echo "  If none of the c, m, i or p options are set then the script will do"
    echo "  *all* steps. Otherwise only the enabled steps are run - it's your"
    echo "  reponsibility to ensure that precusors are in good shape"
}

# Parse the command line arguments:
BUILDDIR=""
BUILDTYPE="RelWithDebInfo"
GENERATOR="Unix Makefiles"
EXE_CMAKE=""
EXE_MAKE=""
EXE_INSTALL=""
EXE_CPACK=""
NIGHTLY=true
while getopts ":t:b:g:hcmipa" opt; do
    case $opt in
        t)
            BUILDTYPE=$OPTARG
            ;;
        b)
            BUILDDIR=$OPTARG
            ;;
        g)
            GENERATOR=$OPTARG
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

if [ -z "$EXE_CMAKE" -a -z "$EXE_MAKE" -a -z "$EXE_INSTALL" -a \
        -z "$EXE_CPACK" ]; then
    EXE_CMAKE="1"
    EXE_MAKE="1"
    EXE_INSTALL="1"
    EXE_CPACK="1"
fi

# Stop on errors from here on out:
set -e
set -o pipefail

# Source in our environment
AthAnalysisSrcDir=$(dirname ${BASH_SOURCE[0]})
if [ -z "$BUILDDIR" ]; then
    BUILDDIR=${AthAnalysisSrcDir}/../../../build
fi
mkdir -p ${BUILDDIR}
BUILDDIR=$(cd ${BUILDDIR} && pwd)
source $AthAnalysisSrcDir/build_env.sh -b $BUILDDIR >& ${BUILDDIR}/build_env.log
cat ${BUILDDIR}/build_env.log

# Set Gaudi's version to the same value as this project's version:
export GAUDI_VERSION=`cat ${AthAnalysisSrcDir}/version.txt`

# create the actual build directory
mkdir -p ${BUILDDIR}/build/AthAnalysis
cd ${BUILDDIR}/build/AthAnalysis

# CMake:
if [ -n "$EXE_CMAKE" ]; then
    # Remove the CMakeCache.txt file, to force CMake to find externals
    # from scratch in an incremental build.
    rm -f CMakeCache.txt

    # The package specific log files can only be generated with the Makefile
    # and Ninja generators. Very notably it doesn't work with IDEs in general.
    USE_LAUNCHERS=""
    if [ "${GENERATOR}" = "Ninja" ] || [ "${GENERATOR}" = "Unix Makefiles" ]; then
        USE_LAUNCHERS=-DCTEST_USE_LAUNCHERS:BOOL=TRUE
    fi

    # Now run the actual CMake configuration:
    time cmake -G "${GENERATOR}" \
         -DCMAKE_BUILD_TYPE:STRING=${BUILDTYPE} \
         ${USE_LAUNCHERS} \
         ${AthAnalysisSrcDir} 2>&1 | tee cmake_config.log
fi

# For nightly builds we want to get as far as we can:
if [ "$NIGHTLY" = true ]; then
    # At this point stop worrying about errors:
    set +e
fi

# Run the build:
if [ -n "$EXE_MAKE" ]; then
    if [ "$NIGHTLY" = true ]; then
        # In order to build the project in a nightly setup, allowing for some
        # build steps to fail while still continuing, we need to use "make"
        # directly. Only allowing the usage of the Makefile generator.
        time make -k 2>&1 | tee cmake_build.log
    else
        # However in a non-nightly setup we can just rely on CMake to start
        # the build for us. In this case we can use any generator we'd like
        # for the build. Notice however that the installation step can still
        # be only done correctly by using GNU Make directly.
        time cmake --build . 2>&1 | tee cmake_build.log
    fi
fi

# Install the results:
if [ -n "$EXE_INSTALL" ]; then
    time make install/fast \
         DESTDIR=${BUILDDIR}/install/AthAnalysis/${NICOS_PROJECT_VERSION} \
         2>&1 | tee cmake_install.log
fi

# Build an RPM for the release:
if [ -n "$EXE_CPACK" ]; then
    time cpack 2>&1 | tee cmake_cpack.log
    FILES=$(ls AthAnalysis*.rpm AthAnalysis*.dmg AthAnalysis*.tar.gz)
    cp ${FILES} ${BUILDDIR}/
fi
