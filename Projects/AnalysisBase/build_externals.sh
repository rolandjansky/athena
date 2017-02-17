#!/bin/bash
#
# Script building all the externals necessary for the nightly build.
#

# Stop on errors:
set -e

# Function printing the usage information for the script
usage() {
    echo "Usage: build_externals.sh [-t build_type] [-b build_dir] [-f]"
    echo " -f: Force rebuild of externals, otherwise if script"
    echo "     finds an external build present it will simply exit"
    echo "If a build_dir is not given the default is '../build'"
    echo "relative to the athena checkout"
}

# Parse the command line arguments:
BUILDDIR=""
BUILDTYPE="RelWithDebInfo"
FORCE=""
while getopts ":t:b:fh" opt; do
    case $opt in
        t)
            BUILDTYPE=$OPTARG
            ;;
        b)
            BUILDDIR=$OPTARG
            ;;
	f)
            FORCE=1
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

if [ -n "$FORCE" ]; then
    echo "Force deleting existing build area..."
    rm -fr ${BUILDDIR}/install ${BUILDDIR}/src ${BUILDDIR}/build
fi

if [ -d ${BUILDDIR}/install/AnalysisBaseExternals ]; then
        echo "Found install directorr for AnalysisBaseExternalsn ${BUILDDIR}/install"
        echo "Use -f option to force a rebuild"
        exit 0
fi

# Create some directories:
mkdir -p ${BUILDDIR}/install

# Set some environment variables that the builds use internally:
export NICOS_PROJECT_VERSION=`cat ${thisdir}/version.txt`
export NICOS_ATLAS_RELEASE=${NICOS_PROJECT_VERSION}
export NICOS_PROJECT_RELNAME=${NICOS_PROJECT_VERSION}

# The directory holding the helper scripts:
scriptsdir=${thisdir}/../../Build/AtlasBuildScripts
scriptsdir=$(cd ${scriptsdir}; pwd)

# Read in the tag/branch to use for AnalysisBaseExternals:
AnalysisBaseExternalsVersion=$(awk '/^AnalysisBaseExternalsVersion/{print $3}' ${thisdir}/externals.txt)

# Check out AnalysisBaseExternals from the right branch/tag:
${scriptsdir}/checkout_atlasexternals.sh \
    -t ${AnalysisBaseExternalsVersion} \
    -s ${BUILDDIR}/src/AnalysisBaseExternals

# Build AnalysisBaseExternals:
export NICOS_PROJECT_HOME=$(cd ${BUILDDIR}/install;pwd)/AnalysisBaseExternals
${scriptsdir}/build_atlasexternals.sh \
    -s ${BUILDDIR}/src/AnalysisBaseExternals \
    -b ${BUILDDIR}/build/AnalysisBaseExternals \
    -i ${BUILDDIR}/install/AnalysisBaseExternals/${NICOS_PROJECT_VERSION} \
    -p AnalysisBaseExternals -r ${BUILDDIR} -t ${BUILDTYPE} \
    -v ${NICOS_PROJECT_VERSION}
