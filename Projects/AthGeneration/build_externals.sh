#!/bin/bash
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Script building all the externals necessary for the nightly build.
#

# Function printing the usage information for the script
usage() {
    echo "Usage: build_externals.sh [-t build_type] [-b build_dir] [-f] [-c] [-x]"
    echo " -f: Force rebuild of externals from scratch, otherwise if script"
    echo "     finds an external build present it will only do an incremental"
    echo "     build"
    echo " -c: Build the externals for the continuous integration (CI) system,"
    echo "     skipping the build of the externals RPMs."
    echo " -x: Extra cmake argument(s) to provide for the build(configuration)"
    echo "     of all externals needed by AthGeneration."
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

# Only stop on errors if we are in the CI. Otherwise just count them.
if [ "$CI" = "1" ]; then
    set -e
    set -o pipefail
fi
ERROR_COUNT=0

# We are in BASH, get the path of this script in a simple way:
thisdir=$(dirname ${BASH_SOURCE[0]})
thisdir=$(cd ${thisdir};pwd)

# Go to the main directory of the repository:
cd ${thisdir}/../..

{
 test "X${NIGHTLY_STATUS}" != "X" && {
    scriptsdir_nightly_status=${NIGHTLY_STATUS_SCRIPTS}
    test "X$scriptsdir_nightly_status" = "X" && scriptsdir_nightly_status=${scriptsdir}/nightly_status
    test -x $scriptsdir_nightly_status/externals_status_on_exit.sh  && trap $scriptsdir_nightly_status/externals_status_on_exit.sh EXIT
 }
}

# Check if the user specified any source/build directories:
if [ "$BUILDDIR" = "" ]; then
    BUILDDIR=${thisdir}/../../../build
fi
mkdir -p ${BUILDDIR}
BUILDDIR=$(cd $BUILDDIR; pwd)

if [ "$FORCE" = "1" ]; then
    echo "Force deleting existing build area..."
    rm -fr ${BUILDDIR}/install/AthGenerationExternals ${BUILDDIR}/install/GAUDI
    rm -fr ${BUILDDIR}/src/AthGenerationExternals ${BUILDDIR}/src/GAUDI
    rm -fr ${BUILDDIR}/build/AthGenerationExternals ${BUILDDIR}/build/GAUDI
fi

# Get the version of AthGeneration for the build.
version=`cat ${thisdir}/version.txt`

# Check if previous externals build can be reused:
externals_stamp=${BUILDDIR}/build/AthGenerationExternals/externals-${version}.stamp
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

# Set the environment variable for finding LCG releases:
source ${scriptsdir}/LCG_RELEASE_BASE.sh

# Flag for triggering the build of RPMs for the externals:
RPMOPTIONS="-r ${BUILDDIR}"
if [ "$CI" = "1" ]; then
    RPMOPTIONS=
fi

# Read in the tag/branch to use for AthGenerationExternals:
AthGenerationExternalsVersion=$(awk '/^AthGenerationExternalsVersion/{print $3}' ${thisdir}/externals.txt)

# Check out AthGenerationExternals from the right branch/tag:
${scriptsdir}/checkout_atlasexternals.sh \
    -t ${AthGenerationExternalsVersion} \
    -s ${BUILDDIR}/src/AthGenerationExternals 2>&1 | \
    tee ${BUILDDIR}/src/checkout.AthGenerationExternals.log

# log analyzer never affects return status in the parent shell:
{
 test "X${NIGHTLY_STATUS}" != "X" && {
    branch=$(basename $(cd .. ; pwd)) #FIXME: should be taken from env.
    timestamp_tmp=` basename ${BUILDDIR}/../.@@__* 2>/dev/null | sed 's,^\.,,' ` #to be used until the final stamp from ReleaseData is available
    test "X$timestamp_tmp" != "X" || {
        timestamp_tmp=@@__`date "+%Y-%m-%dT%H%M"`__@@
        touch ${BUILDDIR}/../.${timestamp_tmp}
    }
   (set +e
    ${scriptsdir_nightly_status}/checkout_status.sh "$branch" "$BINARY_TAG" "$timestamp_tmp" AthGenerationExternals ${BUILDDIR}/src/checkout.AthGenerationExternals.log
   )
 } || true
}

## Build AthGenerationExternals:
${scriptsdir}/build_atlasexternals.sh \
    -s ${BUILDDIR}/src/AthGenerationExternals \
    -b ${BUILDDIR}/build/AthGenerationExternals \
    -i ${BUILDDIR}/install \
    -p AthGenerationExternals ${RPMOPTIONS} -t ${BUILDTYPE} \
    -v ${version} \
    ${EXTRACMAKE[@]/#/-x } || ((ERROR_COUNT++))

{
 test "X${NIGHTLY_STATUS}" != "X" && {
   (set +e
    ${scriptsdir_nightly_status}/cmake_config_status.sh "$branch" "$BINARY_TAG" "$timestamp_tmp" AthGenerationExternals ${BUILDDIR}/build/AthGenerationExternals/cmake_config.log
    ${scriptsdir_nightly_status}/cmake_build_status.sh  "$branch" "$BINARY_TAG" "$timestamp_tmp" AthGenerationExternals ${BUILDDIR}/build/AthGenerationExternals/cmake_build.log
   )
 } || true
}

# Get the "platform name" from the directory created by the AthGenerationExternals
# build:
platform=$(cd ${BUILDDIR}/install/AthGenerationExternals/${version}/InstallArea;ls)

# Read in the tag/branch to use for Gaudi:
GaudiVersion=$(awk '/^GaudiVersion/{print $3}' ${thisdir}/externals.txt)

# Check out Gaudi from the right branch/tag:
${scriptsdir}/checkout_Gaudi.sh \
    -t ${GaudiVersion} \
    -s ${BUILDDIR}/src/GAUDI 2>&1 | tee ${BUILDDIR}/src/checkout.GAUDI.log

{
 test "X${NIGHTLY_STATUS}" != "X" && {
   (set +e
    ${scriptsdir_nightly_status}/checkout_status.sh "$branch" "$BINARY_TAG" "$timestamp_tmp" GAUDI ${BUILDDIR}/src/checkout.GAUDI.log
   )
 } || true
}

# Build Gaudi:
${scriptsdir}/build_Gaudi.sh \
    -s ${BUILDDIR}/src/GAUDI \
    -b ${BUILDDIR}/build/GAUDI \
    -i ${BUILDDIR}/install \
    -e ${BUILDDIR}/install/AthGenerationExternals/${version}/InstallArea/${platform} \
    -v ${version} \
    -p AthGenerationExternals -f ${platform} ${RPMOPTIONS} -t ${BUILDTYPE} \
    ${EXTRACMAKE[@]/#/-x } || ((ERROR_COUNT++))

{
 test "X${NIGHTLY_STATUS}" != "X" && {
   (set +e
    ${scriptsdir_nightly_status}/cmake_config_status.sh "$branch" "$BINARY_TAG" "$timestamp_tmp" GAUDI ${BUILDDIR}/build/GAUDI/cmake_config.log
    ${scriptsdir_nightly_status}/cmake_build_status.sh  "$branch" "$BINARY_TAG" "$timestamp_tmp" GAUDI ${BUILDDIR}/build/GAUDI/cmake_build.log
   )
 } || true
}

# Exit with the error count taken into account.
if [ ${ERROR_COUNT} -ne 0 ]; then
    echo "AthGeneration externals build encountered ${ERROR_COUNT} error(s)"
else
    cp ${thisdir}/externals.txt ${externals_stamp}
fi
exit ${ERROR_COUNT}
