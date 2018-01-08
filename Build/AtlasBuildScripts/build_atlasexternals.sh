#!/bin/bash
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
        "<-i install dir> [-p project] [-r RPM dir] [-t build type]"
}

# Parse the command line arguments:
SOURCEDIR=""
BUILDDIR=""
INSTALLDIR=""
PROJECT="AthenaExternals"
RPMDIR=""
BUILDTYPE="Release"
PROJECTVERSION=""
while getopts ":s:b:i:p:r:t:v:h" opt; do
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
        v)
            PROJECTVERSION=$OPTARG
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

# Create the build directory if it doesn't exist, and move to it:
mkdir -p ${BUILDDIR} || ((ERROR_COUNT++))
cd ${BUILDDIR} || ((ERROR_COUNT++))

# Extra settings for providing a project version for the build if necessary:
EXTRACONF=
if [ "$PROJECTVERSION" != "" ]; then
    PNAME=$(echo ${PROJECT} | awk '{print toupper($0)}')
    EXTRACONF=-D${PNAME}_PROJECT_VERSION:STRING=${PROJECTVERSION}
fi

#FIXME: simplify error counting:


# Configure the build:
error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
{
 cmake -DCMAKE_BUILD_TYPE:STRING=${BUILDTYPE} -DCTEST_USE_LAUNCHERS:BOOL=TRUE \
    ${EXTRACONF} \
    ${SOURCEDIR}/Projects/${PROJECT}/ || touch $error_stamp
} 2>&1 | tee cmake_config.log 
test -f $error_stamp && ((ERROR_COUNT++)) 
rm -f $error_stamp #FIXME: w/o $error_stamp one can't pass the status outside  { ... } | tee ... shell

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

# Build the RPM or other package for the project:
error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
{
cpack || touch $error_stamp
} 2>&1 | tee cmake_cpack.log
test -f $error_stamp && ((ERROR_COUNT++)) 
rm -f $error_stamp 
    
error_stamp=`mktemp .tmp.error.XXXXX` ; rm -f $error_stamp
{
 mkdir -p ${RPMDIR} && \
 FILES=`ls ${PROJECT}*.rpm  ${PROJECT}*.tar.gz  ${PROJECT}*.dmg 2>/dev/null ; true ;` && \
 test "X$FILES" != "X" && \
 cp ${FILES} ${RPMDIR} || touch $error_stamp
} 2>&1 | tee cp_rpm.log
test -f $error_stamp && ((ERROR_COUNT++)) 
rm -f $error_stamp 

if [ $ERROR_COUNT -ne 0 ]; then
	echo "AtlasExternals build script counted $ERROR_COUNT errors"
fi

exit ${ERROR_COUNT}
