#! /bin/bash
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

REPOURL="https://:@gitlab.cern.ch:8443/atlas/athena.git"

# Function printing the usage information for the script
usage() {
    cat <<EOF
Usage: tag_build.sh [-d date_stamp] [-u repository_url]
       Tag a 'nightly' build based on the current branch and timestamp. If the tag
       already exists it will be checked out instead. This script should only be used
       by NICOS to tag nightly builds, not for private builds.

       date_stamp        Format YYYY-MM-DDTHHMM (default: \$datestamp)
       repository_url    git repository URL (default: $REPOURL)
EOF
}

# Parse the command line arguments:
DATESTAMP="$datestamp"
while getopts ":d:u:h" opt; do
    case $opt in
        d)
            DATESTAMP=$OPTARG
            ;;
        u)
            REPOURL=$OPTARG
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

# Check we got a datestamp and that it's a valid format
if [ "$DATESTAMP" = "" ]; then
	echo "No date_stamp defined from NICOS or given as a parameter"
	exit 1
fi
echo $DATESTAMP | egrep "^[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{4}$" >& /dev/null
if [ $? != "0" ]; then
	echo "Datestamp '$DATESTAMP' does not correspond to the ATLAS format YYYY-MM-DDTHHMM"
	exit 1
fi

# Abort if any of the next commands fail
set -e

# Use the source of this script to ensure that we cd to within the root of the
# local repository
ScriptSrcDir=$(dirname ${BASH_SOURCE[0]})
cd $ScriptSrcDir/../..

# Get branch name, then tag/push or checkout if it already exists
BRANCH=$(git symbolic-ref --short HEAD)
TAG="nightly/$BRANCH/$DATESTAMP"
if git rev-parse $TAG > /dev/null 2>&1; then
    echo "Tag $TAG already exists. Doing checkout..."
    git checkout $TAG
else
    echo "Creating tag $TAG"
    git tag $TAG
    git push $REPOURL $TAG
fi
