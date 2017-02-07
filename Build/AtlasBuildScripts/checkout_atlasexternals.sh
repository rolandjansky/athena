#!/bin/bash
#
# Script for automating the checkout of the atlasexternals repository when
# building the whole software stack in a nightly/release.
#
# The script must receive the tag/branch that should be checked out of the
# atlasexternals repository. And it can write the commit hash that the
# checkout ended up with, into another file.
#

# Stop on errors:
set -e

# Function printing the usage information for the script
usage() {
    echo "Usage: checkout_atlasexternals.sh <-t branch/tag> " \
        "<-s source directory> <-e atlas externals repo url> " \
	"[-o hash_file.txt]"
}

# Parse the command line arguments:
TAGBRANCH=""
SOURCEDIR=""
HASHFILE=""
EXTERNALSURL="https://:@gitlab.cern.ch:8443/atlas/atlasexternals.git"
while getopts ":t:o:s:h:e" opt; do
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
	e)
	    EXTERNALSURL=$OPTARG
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
echo "Checking out atlasexternals tag/branch: $TAGBRANCH"
if [ "$HASHFILE" != "" ]; then
    echo "Writing the commit hash into file: $HASHFILE"
fi

# Clone the repository:
git clone ${EXTERNALSURL} ${SOURCEDIR}

# Get the appropriate tag of it:
cd ${SOURCEDIR}
git checkout ${TAGBRANCH}

# If an output file was not specified, stop here:
if [ "$HASHFILE" = "" ]; then
    exit 0
fi

# Write the hash of whatever we have checked out currently, into the
# specified output file:
git rev-parse ${TAGBRANCH} > ${HASHFILE}