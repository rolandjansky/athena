#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# NOTE do NOT run with /bin/bash -x as the output is too big for gitlab-ci
# arguments:  INPUTNAME
#
# author : Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>
#
# example: art-download NAME DIRECTORY

if [ $# -ne 2 ]; then
    echo 'Usage: art-get-input.sh NAME DIRECTORY'
    exit 1
fi

NAME=$1
shift
DIRECTORY=$1
shift

export ATLAS_LOCAL_ROOT_BASE="${ATLAS_LOCAL_ROOT_BASE:-/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase}"
# shellcheck source=/dev/null
source "${ATLAS_LOCAL_ROOT_BASE}"/user/atlasLocalSetup.sh --quiet

unset ALRB_noGridMW

lsetup -f rucio
lsetup -f "xrootd 4.7.1"

echo "Name: ${NAME}"
echo "Directory: ${DIRECTORY}"

# Do not use: rucio delivers warnings as exit code 127
#set -e

rucio download --dir "${DIRECTORY}" "${NAME}"
