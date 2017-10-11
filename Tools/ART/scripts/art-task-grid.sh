#!/bin/bash
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# NOTE do NOT run with /bin/bash -x as the output is too big for gitlab-ci
# arguments: [options] SUBMIT_DIRECTORY SCRIPT_DIRECTORY PACKAGE SEQUENCE_TAG NUMBER_OF_TESTS NIGHTLY_RELEASE PROJECT PLATFORM NIGHTLY_TAG
# env: ART_GRID_OPTIONS
#
# author : Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>
#
# example: [--skip-setup] tmp /cvmfs/atlas-nightlies.cern.ch/sw/... Tier0ChainTests grid 316236 32 21.0 Athena x86_64-slc6-gcc62-opt 2017-02-26T2119
#set -e

echo "Script executed by $(whoami) on $(date)"

SKIP_SETUP=0
if [ $1 == "--skip-setup" ]; then
  SKIP_SETUP=1
  shift
fi
SUBMIT_DIRECTORY=$1
shift
SCRIPT_DIRECTORY=$1
shift
PACKAGE=$1
shift
TYPE=$1
shift
SEQUENCE_TAG=$1
shift
NUMBER_OF_TESTS=$1
shift
NIGHTLY_RELEASE=$1
shift
PROJECT=$1
shift
PLATFORM=$1
shift
NIGHTLY_TAG=$1
shift

# we seem to have to copy the env variables locally
GRID_OPTIONS=$ART_GRID_OPTIONS

# change -VAL-Prod and others into -VAL
NIGHTLY_RELEASE_SHORT=${NIGHTLY_RELEASE/-VAL-*/-VAL}

if [ ${SKIP_SETUP} -eq 0 ]; then
    echo "Setting up release: ${PLATFORM} ${NIGHTLY_RELEASE_SHORT} ${NIGHTLY_TAG} ${PROJECT}"
    USER=artprod

    export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
    source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

    export RUCIO_ACCOUNT=artprod

    lsetup panda "asetup --platform=${PLATFORM} ${NIGHTLY_RELEASE_SHORT},${NIGHTLY_TAG},${PROJECT}"

    voms-proxy-init --rfc -noregen -cert ./grid.proxy -voms atlas

fi

# NOTE: for art-internal.py the current dir can be used as it is copied there
cd ${SUBMIT_DIRECTORY}/${PACKAGE}/run
OUTFILE="user.${USER}.atlas.${NIGHTLY_RELEASE_SHORT}.${PROJECT}.${PLATFORM}.${NIGHTLY_TAG}.${SEQUENCE_TAG}.${PACKAGE}"
CMD="pathena ${GRID_OPTIONS} --noBuild --expertOnly_skipScout --trf \"./art-internal.py job grid ${SCRIPT_DIRECTORY} ${PACKAGE} ${TYPE} ${SEQUENCE_TAG} %RNDM:0 %OUT.tar ${NIGHTLY_RELEASE_SHORT} ${PROJECT} ${PLATFORM} ${NIGHTLY_TAG}\" --split ${NUMBER_OF_TESTS} --outDS ${OUTFILE}"
#--disableAutoRetry
#--excludedSite=ANALY_TECHNION-HEP-CREAM
#--site=ANALY_NIKHEF-ELPROD_SHORT,ANALY_NIKHEF-ELPROD"
#--site=ANALY_FZK,ANALY_BNL,ANALY_RAL"
echo ${CMD}

RESULT=`eval "${CMD}"`
echo ${RESULT}
