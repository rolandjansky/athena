#!/bin/bash
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# NOTE do NOT run with /bin/bash -x as the output is too big for gitlab-ci
# arguments:  PACKAGE INDEX EXTENSION NIGHTLY_RELEASE PROJECT PLATFORM NIGHTLY_TAG
# env: ART_GRID_OPTIONS
#
# author : Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>
#
# example: Tier0ChainTests 4 _EXT0 21.0 Athena x86_64-slc6-gcc62-opt 2017-07-24T2151

if [ $# -ne 7 ]; then
    echo 'Usage: art-get-tar.sh PACKAGE INDEX EXTENSION NIGHTLY_RELEASE PROJECT PLATFORM NIGHTLY_TAG'
    exit 1
fi

echo "Script executed by $(whoami) on $(date)"
echo "User: ${USER}"
ART_USER='artprod'

PACKAGE=$1
shift
INDEX=$1
shift
EXTENSION=$1
shift
NIGHTLY_RELEASE=$1
shift
PROJECT=$1
shift
PLATFORM=$1
shift
NIGHTLY_TAG=$1
shift

#  Note: should be $USER
#printf -v CONTAINER_LIST 'user.%s.atlas.%s.%s.%s.%s.*.%s%s' ${ART_USER} ${NIGHTLY_RELEASE} ${PROJECT} ${PLATFORM} ${NIGHTLY_TAG} ${PACKAGE} ${EXTENSION}
printf -v CONTAINER_LIST 'user.%s.atlas.%s.%s.%s.%s.*.%s%s' 'artprod' ${NIGHTLY_RELEASE} ${PROJECT} ${PLATFORM} ${NIGHTLY_TAG} ${PACKAGE} ${EXTENSION}

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

unset ALRB_noGridMW

lsetup -f rucio

echo "ContainerList: ${CONTAINER_LIST}"

# Do not use: rucio delivers warnings as exit code 127
#set -e

FILELIST =`rucio list-files --csv ${CONTAINER_LIST}`
echo "Tar files in the Container: ${FILELIST}"

CONTAINER=`rucio list-dids ${CONTAINER_LIST} --filter type=container | grep ${NIGHTLY_TAG} | sort -r | cut -d ' ' -f 2 | head -n 1`
echo "Container: ${CONTAINER}"

printf -v INDEX_FORMAT '_%06d.tar' ${INDEX}
TAR_NAME=`rucio list-files --csv ${CONTAINER} | grep ${INDEX_FORMAT} | cut -d ',' -f 1`
echo "Tar Name: ${TAR_NAME}"

rucio download ${TAR_NAME}

echo "TAR_NAME=${TAR_NAME}"
