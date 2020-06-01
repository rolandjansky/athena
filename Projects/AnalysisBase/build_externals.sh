#!/bin/bash
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Script building all the externals necessary for the nightly build.
#

# Stop on errors:
set -e

# Function printing the usage information for the script
usage() {
    echo "Usage: build_externals.sh [-t build_type] [-b build_dir] [-f] [-c] [-x opt]"
    echo " -f: Force rebuild of externals, otherwise if script"
    echo "     finds an external build present it will simply exit"
    echo " -c: Build the externals for the continuous integration (CI) system,"
    echo "     skipping the build of the externals RPMs."
    echo " -x: Extra cmake argument(s) to provide for the build(configuration)"
    echo "     of all externals needed by Athena."
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
    rm -fr ${BUILDDIR}/install/AnalysisBaseExternals
    rm -fr ${BUILDDIR}/src/AnalysisBaseExternals
    rm -fr ${BUILDDIR}/build/AnalysisBaseExternals
fi

# Get the version of AnalysisBase for the build.
version=`cat ${thisdir}/version.txt`

# Check if previous externals build can be reused:
externals_stamp=${BUILDDIR}/build/AnalysisBaseExternals/externals-${version}.stamp
if [ -f ${externals_stamp} ]; then
    if diff -q ${externals_stamp} ${thisdir}/externals.txt; then
        echo "Correct version of externals already available in ${BUILDDIR}"
        exit 0
    else
        rm ${externals_stamp}
    fi
fi

# Create some directories:
mkdir -p ${BUILDDIR}/{src,install}

# The directory holding the helper scripts:
scriptsdir=${thisdir}/../../Build/AtlasBuildScripts
scriptsdir=$(cd ${scriptsdir}; pwd)

# Flag for triggering the build of RPMs for the externals:
RPMOPTIONS="-r ${BUILDDIR}"
if [ "$CI" = "1" ]; then
    RPMOPTIONS=
fi

# Read in the tag/branch to use for AnalysisBaseExternals:
AnalysisBaseExternalsVersion=$(awk '/^AnalysisBaseExternalsVersion/{print $3}' ${thisdir}/externals.txt)

# Stop on all errors in the following (piped) commands:
set -o pipefail

# Check out AnalysisBaseExternals from the right branch/tag:
${scriptsdir}/checkout_atlasexternals.sh \
    -t ${AnalysisBaseExternalsVersion} \
    -s ${BUILDDIR}/src/AnalysisBaseExternals 2>&1 | tee ${BUILDDIR}/src/checkout.AnalysisBaseExternals.log

# Build AnalysisBaseExternals:
export NICOS_PROJECT_HOME=$(cd ${BUILDDIR}/install;pwd)/AnalysisBaseExternals
${scriptsdir}/build_atlasexternals.sh \
    -s ${BUILDDIR}/src/AnalysisBaseExternals \
    -b ${BUILDDIR}/build/AnalysisBaseExternals \
    -i ${BUILDDIR}/install \
    -p AnalysisBaseExternals ${RPMOPTIONS} -t ${BUILDTYPE} \
    -v ${version} ${EXTRACMAKE[@]/#/-x }

# Due to 'set -e' only gets here on success:
cp ${thisdir}/externals.txt ${externals_stamp}
