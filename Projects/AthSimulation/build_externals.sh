#!/bin/bash
#
# Script building all the externals necessary for the nightly build.
#

# Function printing the usage information for the script
usage() {
    echo "Usage: build_externals.sh [-t build_type] [-b build_dir] [-f] [-c]"
    echo " -f: Force rebuild of externals from scratch, otherwise if script"
    echo "     finds an external build present it will only do an incremental"
    echo "     build"
    echo " -c: Build the externals for the continuous integration (CI) system,"
    echo "     skipping the build of the externals RPMs."
    echo " -x: Extra cmake argument(s) to provide for the build(configuration)"
    echo "     of all externals needed by AthSimulation."
    echo "If a build_dir is not given the default is '../build'"
    echo "relative to the athena checkout"
}

# Parse the command line arguments:
BUILDDIR=""
BUILDTYPE="RelWithDebInfo"
FORCE=""
CI=""
EXTRACMAKE=()
while getopts ":t:b:x:fch" opt; do
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

# Stop on errors from here on out:
set -e
set -o pipefail

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

if [ "$FORCE" = "1" ]; then
    echo "Force deleting existing build area..."
    rm -fr ${BUILDDIR}/install/AthSimulationExternals ${BUILDDIR}/install/GAUDI
    rm -fr ${BUILDDIR}/src/AthSimulationExternals ${BUILDDIR}/src/GAUDI
    rm -fr ${BUILDDIR}/build/AthSimulationExternals ${BUILDDIR}/build/GAUDI
fi

# Create some directories:
mkdir -p ${BUILDDIR}/{src,install}

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

# Read in the tag/branch to use for AthSimulationExternals:
AthSimulationExternalsVersion=$(awk '/^AthSimulationExternalsVersion/{print $3}' ${thisdir}/externals.txt)

# Check out AthSimulationExternals from the right branch/tag:
${scriptsdir}/checkout_atlasexternals.sh \
    -t ${AthSimulationExternalsVersion} \
    -s ${BUILDDIR}/src/AthSimulationExternals 2>&1 | tee ${BUILDDIR}/src/checkout.AthSimulationExternals.log

# Build AthSimulationExternals:
export NICOS_PROJECT_HOME=$(cd ${BUILDDIR}/install;pwd)/AthSimulationExternals
${scriptsdir}/build_atlasexternals.sh \
    -s ${BUILDDIR}/src/AthSimulationExternals \
    -b ${BUILDDIR}/build/AthSimulationExternals \
    -i ${BUILDDIR}/install/AthSimulationExternals/${NICOS_PROJECT_VERSION} \
    -p AthSimulationExternals ${RPMOPTIONS} -t ${BUILDTYPE} \
    ${EXTRACMAKE[@]/#/-x } -v ${NICOS_PROJECT_VERSION}

# Get the "platform name" from the directory created by the AthSimulationExternals
# build:
platform=$(cd ${BUILDDIR}/install/AthSimulationExternals/${NICOS_PROJECT_VERSION}/InstallArea;ls)

# Read in the tag/branch to use for Gaudi:
GaudiVersion=$(awk '/^GaudiVersion/{print $3}' ${thisdir}/externals.txt)

# Check out Gaudi from the right branch/tag:
${scriptsdir}/checkout_Gaudi.sh \
    -t ${GaudiVersion} \
    -s ${BUILDDIR}/src/GAUDI 2>&1 | tee ${BUILDDIR}/src/checkout.GAUDI.log

# Build Gaudi:
export NICOS_PROJECT_HOME=$(cd ${BUILDDIR}/install;pwd)/GAUDI
${scriptsdir}/build_Gaudi.sh \
    -s ${BUILDDIR}/src/GAUDI \
    -b ${BUILDDIR}/build/GAUDI \
    -i ${BUILDDIR}/install/GAUDI/${NICOS_PROJECT_VERSION} \
    -e ${BUILDDIR}/install/AthSimulationExternals/${NICOS_PROJECT_VERSION}/InstallArea/${platform} \
    -p AthSimulationExternals -f ${platform} ${EXTRACMAKE[@]/#/-x } \
    ${RPMOPTIONS} -t ${BUILDTYPE}
