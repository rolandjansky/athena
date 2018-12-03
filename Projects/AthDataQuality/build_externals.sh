#!/bin/bash
#
# Script building all the externals necessary for the nightly build.
#

# Function printing the usage information for the script
usage() {
    echo "Usage: build_externals.sh [-t build_type] [-b build_dir] [-f] [-c]"
    echo " -f: Force rebuild of externals from scratch, otherwise if script"
    echo "     finds an external build present it will only do an incremental"
    echo "     build. (unused)"
    echo " -c: Build the externals for the continuous integration (CI) system,"
    echo "     skipping the build of the externals RPMs. (unused)"
    echo "If a build_dir is not given the default is '../build'"
    echo "relative to the athena checkout"
}

# Parse the command line arguments:
BUILDDIR=""
BUILDTYPE="RelWithDebInfo"
FORCE=""
CI=""
while getopts ":t:b:fch" opt; do
    case $opt in
        t)
            BUILDTYPE=$OPTARG
            ;;
        b)
            BUILDDIR=$OPTARG
            ;;
        f)
            FORCE="1"
            ;;
        c)
            CI="1"
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
mkdir -p ${BUILDDIR}
BUILDDIR=$(cd $BUILDDIR; pwd)

# Create some directories:
mkdir -p ${BUILDDIR}/src

# Set some environment variables that the builds use internally:
export NICOS_PROJECT_VERSION=`cat ${thisdir}/version.txt`
export NICOS_ATLAS_RELEASE=${NICOS_PROJECT_VERSION}
export NICOS_PROJECT_RELNAME=${NICOS_PROJECT_VERSION}

# The directory holding the helper scripts:
scriptsdir=${thisdir}/../../Build/AtlasBuildScripts
scriptsdir=$(cd ${scriptsdir}; pwd)

# Set the environment variable for finding LCG releases:
source ${scriptsdir}/LCG_RELEASE_BASE.sh

# Flag for triggering the build of RPMs for the externals:
RPMOPTIONS="-r ${BUILDDIR}"
if [ "$CI" = "1" ]; then
    RPMOPTIONS=
fi

# Read in the tag/branch to use for atlasexternals:
AtlasExternalsVersion=$(awk '/^AtlasExternalsVersion/{print $3}' ${thisdir}/externals.txt)

# Check out AthenaExternals from the right branch/tag:
${scriptsdir}/checkout_atlasexternals.sh \
    -t ${AtlasExternalsVersion} \
    -s ${BUILDDIR}/src/atlasexternals 2>&1 | tee ${BUILDDIR}/src/checkout.atlasexternals.log 
