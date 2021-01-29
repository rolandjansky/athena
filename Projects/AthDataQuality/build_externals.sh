#!/bin/bash
#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Script setting up the externals repository for AthDataQuality. Note that this
# script does not make use of build_project_externals.sh, as it alone does not
# actually need to build anything. It only needs to check out the CMake code
# of atlas/atlasexternals.
#

# Function printing the usage information for the script
usage() {
    echo "Script getting the cmake code for AthDataQuality"
    echo ""
    echo "Usage: build_externals.sh [options]"
    echo "Options:"
    echo " -b: 'Main build directory' to use for the code checkout and build"
    echo " -f: Force the re-download of the externals repository"
    echo ""
    echo "If a build_dir is not given the default is '../build'"
    echo "relative to the athena checkout"
}

# Parse the command line arguments:
BUILDDIR=""
FORCE=""
while getopts ":b:fh" opt; do
    case $opt in
        b)
            BUILDDIR=$OPTARG
            ;;
        f)
            FORCE="1"
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

# Stop on errors from here on out:
set -e
set -o pipefail

# We are in BASH, get the path of this script in a simple way:
thisdir=$(dirname ${BASH_SOURCE[0]})
thisdir=$(cd ${thisdir};pwd)

# Check if the user specified any source/build directories:
if [ "$BUILDDIR" = "" ]; then
    BUILDDIR=${thisdir}/../../../build
fi
mkdir -p ${BUILDDIR}
BUILDDIR=$(cd $BUILDDIR; pwd)

# Clean the build directory, if necessary.
if [ "$FORCE" = "1" ]; then
   echo "Force deleting existing build area..."
   rm -rf "${BUILDDIR}/src/atlasexternals"
fi

# Create some directories:
mkdir -p ${BUILDDIR}/src

# The directory holding the helper scripts:
scriptsdir=${thisdir}/../../Build/AtlasBuildScripts
scriptsdir=$(cd ${scriptsdir}; pwd)

# Read in the tag/branch to use for atlasexternals:
AtlasExternalsVersion=$(awk '/^AtlasExternalsVersion/{print $3}' ${thisdir}/externals.txt)

# Check out atlasexternals from the right branch/tag:
${scriptsdir}/checkout_atlasexternals.sh                                       \
    -t "${AtlasExternalsVersion}"                                              \
    -s "${BUILDDIR}/src/atlasexternals"                                        \
    2>&1 | tee "${BUILDDIR}/src/checkout.atlasexternals.log"
