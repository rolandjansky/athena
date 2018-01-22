#!/bin/bash
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# NOTE do NOT run with /bin/bash -x as the output is too big for gitlab-ci
# arguments:  INPUTNAME
#
# author : Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>
#
# example: CONTAINER_ID:ENTRY_NAME

if [ $# -ne 1 ]; then
    echo 'Usage: art-get-input.sh INPUTNAME'
    exit 1
fi

echo "Script executed by $(whoami) on $(date)"
echo "User: ${USER}"

INPUTNAME=$1
shift

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

unset ALRB_noGridMW

lsetup -f rucio

echo "InputName: ${INPUTNAME}"

# Do not use: rucio delivers warnings as exit code 127
#set -e

rucio download ${INPUTNAME}
