#!/bin/bash
#
# Script for automating the checkout of the Gaudi repository when
# building the whole software stack in a nightly/release.
#
# The script must receive the tag/branch that should be checked out of the
# Gaudi repository. And it can write the commit hash that the
# checkout ended up with, into another file.
#

# Stop on errors:
set -e

# Function printing the usage information for the script
usage() {
    echo "Usage: checkout_Gaudi.sh [options]"
    echo "  Options:"
    echo "    -t branch/tag: Mandatory branch/tag/hash to check out"
    echo "    -s directory: Mandatory source directory to use"
    echo "    -e url: Optional source URL to use for the checkout"
    echo
    echo "  This script will allow the environment variables"
    echo "  Gaudi_URL and Gaudi_REF to override"
    echo "  other values (even those on the command line)"
    echo "  If Gaudi_URL is set to 'current' then"
	echo "  no clone or checkout is done and the working copy"
	echo "  in the source directory is left untouched."
}

_max_retry_=5

_retry_ () {
    local cmd="$*"
    local n=0
    while ! $cmd ; do
        if test $n -eq $_max_retry_ ; then
            echo "ERROR: $cmd FAILED $_max_retry_ times. EXIT(1)" >&2
            exit 1
        fi
        echo "WARNING: $cmd FAILED, retry in 30 sec ... "
        sleep 30s
        n=`expr $n + 1`
    done
}

_max_retry_=5

_retry_ () {
    local cmd="$*"
    local n=0
    while ! $cmd ; do
        if test $n -eq $_max_retry_ ; then
            echo "ERROR: $cmd FAILED $_max_retry_ times. EXIT(1)" >&2
            exit 1
        fi
        echo "WARNING: $cmd FAILED, retry in 30 sec ... "
        sleep 30s
        n=`expr $n + 1`
    done
}

# Parse the command line arguments:
TAGBRANCH=""
SOURCEDIR=""
GAUDIURL="https://gitlab.cern.ch/atlas/Gaudi.git"
while getopts ":t:o:s:e:h" opt; do
    case $opt in
        t)
            TAGBRANCH=$OPTARG
            ;;
        s)
            SOURCEDIR=$OPTARG
            ;;
        e)
            GAUDIURL=$OPTARG
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

if [ "$Gaudi_URL" != "" ]; then
	GAUDIURL=$Gaudi_URL
	echo "Gaudi URL overridden to $GAUDIURL from environment"
	if [ "$Gaudi_URL" = "current" ]; then
		echo "Leaving current checkout in place for build"
		exit 0
	fi 
fi

if [ "$Gaudi_REF" != "" ]; then
	TAGBRANCH=$Gaudi_REF
	echo "Gaudi ref overridden to $TAGBRANCH from environment"
fi

# The tag/branch and a source directory must have been specified:
if [ "$TAGBRANCH" = "" ] || [ "$SOURCEDIR" = "" ]; then
    echo "Not all required arguments were provided!"
    usage
    exit 1
fi

# Tell the user what will happen:
echo "Checking out Gaudi tag/branch: $TAGBRANCH"
echo "   from: $GAUDIURL"

if [ ! -d "${SOURCEDIR}" ]; then
    # Clone the repository:
    _retry_ git clone ${GAUDIURL} ${SOURCEDIR}
else
    echo "${SOURCEDIR} already exists -> assume previous checkout"
fi

# Get the appropriate version of it:
cd ${SOURCEDIR}
_retry_ git fetch --prune origin
_retry_ git checkout -f ${TAGBRANCH}
