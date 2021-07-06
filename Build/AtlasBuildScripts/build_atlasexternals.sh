#!/bin/bash
#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Example script used for building one of the projects from the atlasexternals
# repository.
#

# Don't stop on errors but count them
set +e
ERROR_COUNT=0

# consider a pipe failed if ANY of the commands fails
set -o pipefail

# Function printing the usage information for the script
usage() {
    echo "Usage: build_atlasexternals.sh <-s source dir> <-b build dir> " \
        "<-i install dir> [-p project] [-r RPM dir] [-t build type] " \
        "[-x extra CMake arguments] [-m extra 'build tool' arguments]"
}

# Parse the command line arguments:
SOURCEDIR=""
BUILDDIR=""
INSTALLDIR=""
PROJECT="AthenaExternals"
RPMDIR=""
BUILDTYPE="Release"
PROJECTVERSION=""
EXTRACMAKE=()
EXTRAMAKE=()
while getopts ":s:b:i:p:r:t:v:hx:m:" opt; do
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
        p)
            PROJECT=$OPTARG
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
        m)
            EXTRAMAKE+=($OPTARG)
            ;;
        v)
            PROJECTVERSION=$OPTARG
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
if [ "$SOURCEDIR" = "" ] || [ "$BUILDDIR" = "" ] || [ "$INSTALLDIR" = "" ]; then
    echo "Not all required parameters received!"
    usage
    exit 1
fi

# Create the build directory if it doesn't exist.
mkdir -p "${BUILDDIR}" || ((ERROR_COUNT++))

# Extra settings for providing a project version for the build if necessary:
EXTRACONF=
if [ "$PROJECTVERSION" != "" ]; then
    EXTRACONF=-DCMAKE_PROJECT_VERSION=${PROJECTVERSION}
fi

# Configure the build:
error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
{
 rm -f ${BUILDDIR}/CMakeCache.txt
 cmake -DCMAKE_BUILD_TYPE:STRING=${BUILDTYPE} -DCTEST_USE_LAUNCHERS:BOOL=TRUE \
    ${EXTRACONF} ${EXTRACMAKE[@]} \
    -B "${BUILDDIR}/" \
    -S "${SOURCEDIR}/Projects/${PROJECT}/" || touch $error_stamp
} 2>&1 | tee "${BUILDDIR}/cmake_config.log"
test -f $error_stamp && ((ERROR_COUNT++))
rm -f $error_stamp

# Build it:
error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
{
 cmake --build "${BUILDDIR}" ${EXTRAMAKE[@]} || touch $error_stamp
} 2>&1 | tee "${BUILDDIR}/cmake_build.log"
test -f $error_stamp && ((ERROR_COUNT++))
rm -f $error_stamp

# Install it:
error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
{
 DESTDIR="${INSTALLDIR}" cmake --install "${BUILDDIR}" || touch $error_stamp
} 2>&1 | tee "${BUILDDIR}/cmake_install.log"
test -f $error_stamp && ((ERROR_COUNT++))
rm -f $error_stamp

# Only perform the packaging if an "RPM directory" was specified.
if [ "${RPMDIR}" != "" ]; then

    # Build the RPM or other package for the project.
    error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
    {
        cpack -B "${BUILDDIR}" --config "${BUILDDIR}/CPackConfig.cmake" || touch $error_stamp
    } 2>&1 | tee "${BUILDDIR}/cmake_cpack.log"
    test -f $error_stamp && ((ERROR_COUNT++))
    rm -f $error_stamp

    # Copy the produced package file(s) into the specified directory.
    error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
    {
        mkdir -p "${RPMDIR}" && \
        FILES=`ls ${BUILDDIR}/${PROJECT}*.rpm ${BUILDDIR}/${PROJECT}*.tar.gz ${BUILDDIR}/${PROJECT}*.dmg 2>/dev/null ; true ;` && \
        test "X$FILES" != "X" && \
        cp ${FILES} "${RPMDIR}/" || touch $error_stamp
    } 2>&1 | tee "${BUILDDIR}/cp_rpm.log"
    test -f $error_stamp && ((ERROR_COUNT++))
    rm -f $error_stamp
fi

# Tell the user how the build went.
if [ $ERROR_COUNT -ne 0 ]; then
	echo "AtlasExternals build script counted $ERROR_COUNT error(s)"
fi
exit ${ERROR_COUNT}
