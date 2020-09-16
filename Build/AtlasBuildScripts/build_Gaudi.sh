#!/bin/bash
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Script used for building Gaudi.
#

# Don't stop on errors but count them
set +e
ERROR_COUNT=0

# consider a pipe failed if ANY of the commands fails
set -o pipefail

# Function printing the usage information for the script
usage() {
    echo "Usage: build_Gaudi.sh <-s source dir> <-b build dir> " \
        "<-i install dir> <-e externals dir> <-p externals project name> " \
        "<-f platform name> [-r RPM dir] [-t build type] " \
        "[-x extra CMake arguments]"
}

# Parse the command line arguments:
SOURCEDIR=""
BUILDDIR=""
INSTALLDIR=""
EXTDIR=""
EXTPROJECT=""
PLATFORM=""
RPMDIR=""
BUILDTYPE="Release"
EXTRACMAKE=()
while getopts ":s:b:i:e:p:f:r:t:x:v:h" opt; do
    case $opt in
        s)
            SOURCEDIR=$OPTARG
            ;;
        b)
            BUILDDIR=$OPTARG
            ;;
        i)
            INSTALLDIR=$OPTARG
            ;;
        e)
            EXTDIR=$OPTARG
            ;;
        p)
            EXTPROJECT=$OPTARG
            ;;
        f)
            PLATFORM=$OPTARG
            ;;
        r)
            RPMDIR=$OPTARG
            ;;
        t)
            BUILDTYPE=$OPTARG
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

# Make sure that the required options were all specified:
if [ "$SOURCEDIR" = "" ] || [ "$BUILDDIR" = "" ] || [ "$INSTALLDIR" = "" ] \
    || [ "$EXTDIR" = "" ] || [ "$EXTPROJECT" = "" ] \
    || [ "$PLATFORM" = "" ]; then
    echo "Not all required parameters received!"
    usage
    exit 1
fi

# Create the build directory if it doesn't exist, and move to it:
mkdir -p ${BUILDDIR} || ((ERROR_COUNT++))
cd ${BUILDDIR} || ((ERROR_COUNT++))

# Set up the externals project:
source ${EXTDIR}/setup.sh || ((ERROR_COUNT++))

# Configure the build:
error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
{
rm -f CMakeCache.txt
rm -rf * # Remove the full build temporarily, to fix GAUDI-1315
cmake -DCMAKE_BUILD_TYPE:STRING=${BUILDTYPE} -DCTEST_USE_LAUNCHERS:BOOL=TRUE \
    -DGAUDI_ATLAS:BOOL=TRUE -DGAUDI_ATLAS_BASE_PROJECT:STRING=${EXTPROJECT} \
    -DCMAKE_INSTALL_PREFIX:PATH=/InstallArea/${PLATFORM} \
    ${EXTRACMAKE[@]} ${SOURCEDIR} || touch $error_stamp
} 2>&1 | tee cmake_config.log
test -f $error_stamp && ((ERROR_COUNT++))
rm -f $error_stamp

# Trigger xenv to pickle the Gaudi build configuration ahead of the build.
# Otherwise parallel processes may step on each others' toes. See ATLINFR-3336.
if [ -f ${BUILDDIR}/config/Gaudi-build.xenv ] && [ -x ${BUILDDIR}/bin/xenv ]; then
    echo "Triggering the creation of ${BUILDDIR}/config/Gaudi-build.xenvc"
    ${BUILDDIR}/bin/xenv --xml ${BUILDDIR}/config/Gaudi-build.xenv true
fi

# Build it:
error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
{
make -k || touch $error_stamp
} 2>&1 | tee cmake_build.log
test -f $error_stamp && ((ERROR_COUNT++))
rm -f $error_stamp


# Install it:
error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
{
make -k install/fast DESTDIR=${INSTALLDIR} || touch $error_stamp
} 2>&1 | tee cmake_install.log
test -f $error_stamp && ((ERROR_COUNT++))
rm -f $error_stamp

# If no RPM directory was specified, stop here:
if [ "$RPMDIR" = "" ]; then
    exit ${ERROR_COUNT}
fi

# Build the RPM for the project:
error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
{
cpack || touch $error_stamp
if [ "$BUILDTYPE" = "RelWithDebInfo" ]; then
    cpack --config CPackDbgRPMConfig.cmake || touch $error_stamp
fi
} 2>&1 | tee cmake_cpack.log
test -f $error_stamp && ((ERROR_COUNT++))
rm -f $error_stamp

error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
{
mkdir -p ${RPMDIR} && cp GAUDI*.rpm ${RPMDIR} || touch $error_stamp
} 2>&1 | tee cp_rpm.log
test -f $error_stamp && ((ERROR_COUNT++))
rm -f $error_stamp

if [ $ERROR_COUNT -ne 0 ]; then
	echo "Gaudi build script counted $ERROR_COUNT errors"
fi

exit ${ERROR_COUNT}
