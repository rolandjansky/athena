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
    git clone ${GAUDIURL} ${SOURCEDIR}
else
    echo "${SOURCEDIR} already exists -> assume previous checkout"
fi

# Get the appropriate version of it:
cd ${SOURCEDIR}
git fetch --prune origin
git checkout -f ${TAGBRANCH}
