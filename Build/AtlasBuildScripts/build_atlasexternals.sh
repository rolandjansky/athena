#!/bin/bash
#
# Example script used for building one of the projects from the atlasexternals
# repository.
#

# Don't stop on errors:
set +e

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
mkdir -p ${BUILDDIR}
cd ${BUILDDIR}

# Extra settings for providing a project version for the build if necessary:
EXTRACONF=
if [ "$PROJECTVERSION" != "" ]; then
    PNAME=$(echo ${PROJECT} | awk '{print toupper($0)}')
    EXTRACONF=-D${PNAME}_PROJECT_VERSION:STRING=${PROJECTVERSION}
fi

# Configure the build:
cmake -DCMAKE_BUILD_TYPE:STRING=${BUILDTYPE} -DCTEST_USE_LAUNCHERS:BOOL=TRUE \
    ${EXTRACONF} \
    ${SOURCEDIR}/Projects/${PROJECT}/ 2>&1 | tee cmake_config.log

# Build it:
make -k

# Install it:
make -k install/fast DESTDIR=${INSTALLDIR}

# If no RPM directory was specified, stop here:
if [ "$RPMDIR" = "" ]; then
    exit 0
fi

# Build the RPM or other package for the project:
cpack
mkdir -p ${RPMDIR}
FILES=$(ls ${PROJECT}*.rpm ${PROJECT}*.tar.gz ${PROJECT}*.dmg)
cp ${FILES} ${RPMDIR}
