#!/bin/bash
#
# Script for building the release on top of externals built using one of the
# scripts in this directory.
#

# Function printing the usage information for the script
usage() {
    echo "Usage: build.sh [-t build type] [-b build dir]"
}

# Parse the command line arguments:
BUILDDIR=""
BUILDTYPE="RelWithDebInfo"
while getopts ":t:s:b:h" opt; do
    case $opt in
        t)
            BUILDTYPE=$OPTARG
            ;;
        b)
            BUILDDIR=$OPTARG
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

# Stop on errors from here on out:
set -e

# We are in BASH, get the path of this script in a simple way:
AthenaSrcDir=$(dirname ${BASH_SOURCE[0]})
AthenaSrcDir=$(cd ${AthenaSrcDir};pwd)

# The directory holding the helper scripts:
scriptsdir=${AthenaSrcDir}/../../Build/AtlasBuildScripts

# Go to the main directory of the repository:
cd ${AthenaSrcDir}/../..

# Check if the user specified any source/build directories:
if [ "$BUILDDIR" = "" ]; then
    BUILDDIR=${AthenaSrcDir}/../../../build
fi

# Set up the environment for the build:
export NICOS_PROJECT_VERSION=`cat ${AthenaSrcDir}/version.txt`
export NICOS_ATLAS_RELEASE=${NICOS_PROJECT_VERSION}
export NICOS_PROJECT_RELNAME=${NICOS_PROJECT_VERSION}
export NICOS_PROJECT_HOME=$(cd ${BUILDDIR}/install;pwd)/Athena

# Set up the environment variables for finding LCG and the TDAQ externals:
source ${scriptsdir}/LCG_RELEASE_BASE.sh
source ${scriptsdir}/TDAQ_RELEASE_BASE.sh

# Set up the AthenaExternals project:
extDir=${BUILDDIR}/install/AthenaExternals/${NICOS_PROJECT_VERSION}/InstallArea
if [ ! -d ${extDir} ]; then
    echo "Didn't find the AthenaExternals project under ${extDir}"
    exit 1
fi
echo "Setting up AthenaExternals from: ${extDir}"
source ${extDir}/*/setup.sh

# Get the "platform name" from the directory created by the AthenaExternals
# build:
platform=$(cd ${extDir};ls)

# Point to Gaudi:
export GAUDI_ROOT=${BUILDDIR}/install/GAUDI/${NICOS_PROJECT_VERSION}/InstallArea/${platform}
echo "Taking Gaudi from: ${GAUDI_ROOT}"

# Configure the build:
mkdir -p ${BUILDDIR}/build/Athena
cd ${BUILDDIR}/build/Athena
time cmake -DCMAKE_BUILD_TYPE:STRING=${BUILDTYPE} \
    -DCTEST_USE_LAUNCHERS:BOOL=TRUE \
    ${AthenaSrcDir} 2>&1 | tee cmake_config.log

# At this point stop worrying about errors:
set +e

# Execute the build:
time make -k

# Install the results:
time make install/fast \
    DESTDIR=${BUILDDIR}/install/Athena/${NICOS_PROJECT_VERSION}

# Build an RPM for the release:
time cpack
cp Athena*.rpm ${BUILDDIR}/
