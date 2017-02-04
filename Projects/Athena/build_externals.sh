#!/bin/bash
#
# Script building all the externals necessary for the nightly build.
#

# Function printing the usage information for the script
usage() {
    echo "Usage: build_externals.sh [-t build type] [-b build dir]"
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

# Version comparison function. Taken from a StackOverflow article.
verlte() {
    if [ "$1" = "`echo -e "$1\n$2" | sort -V | head -n1`" ]; then
        return 1
    fi
    return 0
}

# First off, check that we are using a new enough version of Git. We need
# at least version 1.8.1.
git_min_version=1.8.1
git_version=`git --version | awk '{print $3}'`
verlte "${git_min_version}" "${git_version}"
if [ $? = 0 ]; then
    echo "Detected git version (${git_version}) not new enough."
    echo "Need at least: ${git_min_version}"
    exit 1
fi

# Stop on errors from here on out:
set -e

# We are in BASH, get the path of this script in a simple way:
thisdir=$(dirname ${BASH_SOURCE[0]})
thisdir=$(cd ${thisdir};pwd)

# Go to the main directory of the repository:
cd ${thisdir}/../..

# Check if the user specified any source/build directories:
if [ "$BUILDDIR" = "" ]; then
    BUILDDIR=${thisdir}/../../../build
fi

# Create some directories:
mkdir -p ${BUILDDIR}/install

# Set some environment variables that the builds use internally:
export NICOS_PROJECT_VERSION=`cat ${thisdir}/version.txt`
export NICOS_ATLAS_RELEASE=${NICOS_PROJECT_VERSION}
export NICOS_PROJECT_RELNAME=${NICOS_PROJECT_VERSION}

# The directory holding the helper scripts:
scriptsdir=${thisdir}/../../Build/AtlasBuildScripts

# Set the environment variable for finding LCG releases:
source ${scriptsdir}/LCG_RELEASE_BASE.sh

# Read in the tag/branch to use for AthenaExternals:
AthenaExternalsVersion=$(awk '/^AthenaExternalsVersion/{print $3}' ${thisdir}/externals.txt)

# Check out AthenaExternals from the right branch/tag:
${scriptsdir}/checkout_atlasexternals.sh \
    -t ${AthenaExternalsVersion} \
    -s ${BUILDDIR}/src/AthenaExternals

# Build AthenaExternals:
export NICOS_PROJECT_HOME=$(cd ${BUILDDIR}/install;pwd)/AthenaExternals
${scriptsdir}/build_atlasexternals.sh \
    -s ${BUILDDIR}/src/AthenaExternals \
    -b ${BUILDDIR}/build/AthenaExternals \
    -i ${BUILDDIR}/install/AthenaExternals/${NICOS_PROJECT_VERSION} \
    -p AthenaExternals -r ${BUILDDIR} -t ${BUILDTYPE} \
    -v ${NICOS_PROJECT_VERSION}

# Get the "platform name" from the directory created by the AthenaExternals
# build:
platform=$(cd ${BUILDDIR}/install/AthenaExternals/${NICOS_PROJECT_VERSION}/InstallArea;ls)

# Read in the tag/branch to use for Gaudi:
GaudiVersion=$(awk '/^GaudiVersion/{print $3}' ${thisdir}/externals.txt)

# Check out Gaudi from the right branch/tag:
${scriptsdir}/checkout_Gaudi.sh \
    -t ${GaudiVersion} \
    -s ${BUILDDIR}/src/GAUDI

# Build Gaudi:
export NICOS_PROJECT_HOME=$(cd ${BUILDDIR}/install;pwd)/Gaudi
${scriptsdir}/build_Gaudi.sh \
    -s ${BUILDDIR}/src/GAUDI \
    -b ${BUILDDIR}/build/GAUDI \
    -i ${BUILDDIR}/install/GAUDI/${NICOS_PROJECT_VERSION} \
    -e ${BUILDDIR}/install/AthenaExternals/${NICOS_PROJECT_VERSION}/InstallArea/${platform} \
    -p AthenaExternals -f ${platform} \
    -r ${BUILDDIR} -t ${BUILDTYPE}
