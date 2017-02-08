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
    echo "Usage: checkout_Gaudi.sh <-t branch/tag> " \
        "<-s source directory> [-o hash_file.txt]"
}

# Parse the command line arguments:
TAGBRANCH=""
SOURCEDIR=""
HASHFILE=""
while getopts ":t:o:s:h" opt; do
    case $opt in
        t)
            TAGBRANCH=$OPTARG
            ;;
        s)
            SOURCEDIR=$OPTARG
            ;;
        o)
            HASHFILE=$OPTARG
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
if [ "$HASHFILE" != "" ]; then
    echo "Writing the commit hash into file: $HASHFILE"
fi

if [ ! -d "${SOURCEDIR}" ]; then
    # Clone the repository:
    git clone https://gitlab.cern.ch/atlas/Gaudi.git \
    ${SOURCEDIR}
else
    echo "${SOURCEDIR} already exists -> assume previous checkout"
fi

# Get the appropriate tag of it:
cd ${SOURCEDIR}
git fetch origin
git checkout ${TAGBRANCH}

# If an output file was not specified, stop here:
if [ "$HASHFILE" = "" ]; then
    exit 0
fi

# Write the hash of whatever we have checked out currently, into the
# specified output file:
git rev-parse ${TAGBRANCH} > ${HASHFILE}
