#!/bin/bash
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# NOTE do NOT run with /bin/bash -x as the output is too big for gitlab-ci
# arguments: [options] SUBMIT_DIRECTORY SCRIPT_DIRECTORY PACKAGE SEQUENCE_TAG SPLIT NIGHTLY_RELEASE_SHORT PROJECT PLATFORM NIGHTLY_TAG OUT_FILE
# env: ART_GRID_OPTIONS
#
# author : Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>
#
# options have to be in-order
#
# example: [--skip-setup --test-name TestName --inDS user.tcuhadar.SingleMuon... --nFiles 3 --nEventsPerFile 5] tmp /cvmfs/atlas-nightlies.cern.ch/sw/... Tier0ChainTests grid 316236 3 21.0 Athena x86_64-slc6-gcc62-opt 2017-02-26T2119  user.${USER}.atlas.${NIGHTLY_RELEASE_SHORT}.${PROJECT}.${PLATFORM}.${NIGHTLY_TAG}.${SEQUENCE_TAG}.${PACKAGE}[.${TEST_NUMBER}]
#set -e

echo "Script executed by $(whoami) on $(date)"

SKIP_SETUP=0
if [ $1 == "--skip-setup" ]; then
  SKIP_SETUP=1
  shift
fi
TYPE_OPTION="batch %RNDM:0"
PATHENA_OPTIONS="--destSE=CERN-PROD_SCRATCHDISK"
if [ $1 == "--test-name" ]; then
  TYPE_OPTION="single $2"
  PATHENA_OPTIONS="--destSE=CERN-PROD_SCRATCHDISK --forceStaged"
  shift
  shift
fi
INDS=""
if [ $1 == "--inDS" ]; then
  INDS="--inDS $2"
  shift
  shift
fi
NFILES=""
if [ $1 == "--nFiles" ]; then
  NFILES="--nFiles $2"
  shift
  shift
fi
NEVENTS=""
if [ $1 == "--nEvents" ]; then
  NEVENTS="--nEventsPerFile $2"
  shift
  shift
fi
FILELIST=""
if [ $1 == "--fileList" ]; then
  FILELIST="--fileList $2"
  shift
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
SPLIT=$1
shift
NIGHTLY_RELEASE_SHORT=$1
shift
PROJECT=$1
shift
PLATFORM=$1
shift
NIGHTLY_TAG=$1
shift
OUTFILE=$1
shift

# we seem to have to copy the env variables locally
GRID_OPTIONS=$ART_GRID_OPTIONS

if [ ${SKIP_SETUP} -eq 0 ]; then
    echo "Setting up release: ${PLATFORM} ${NIGHTLY_RELEASE_SHORT} ${NIGHTLY_TAG} ${PROJECT}"
    USER=artprod

    export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
    source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

    export RUCIO_ACCOUNT=artprod

    lsetup panda "asetup --platform=${PLATFORM} ${NIGHTLY_RELEASE_SHORT},${NIGHTLY_TAG},${PROJECT}"

    voms-proxy-init --rfc -noregen -cert ./grid.proxy -voms atlas

fi

if [ ${SPLIT} -eq 0 ]; then
  SPLIT=""
else
  SPLIT="--split ${SPLIT}"
fi

# NOTE: for art-internal.py the current dir can be used as it is copied there
cd ${SUBMIT_DIRECTORY}/${PACKAGE}/run
SUBCOMMAND="./art-internal.py job grid ${SCRIPT_DIRECTORY} ${PACKAGE} ${TYPE} ${SEQUENCE_TAG} ${TYPE_OPTION} %OUT.tar ${NIGHTLY_RELEASE_SHORT} ${PROJECT} ${PLATFORM} ${NIGHTLY_TAG}"
CMD="pathena ${GRID_OPTIONS} ${PATHENA_OPTIONS} --noBuild --expertOnly_skipScout --trf \"${SUBCOMMAND}\" ${SPLIT} --outDS ${OUTFILE} --extOutFile art-job.json ${INDS} ${NFILES} ${NEVENTS} ${FILELIST}"

#--disableAutoRetry
#--excludedSite=ANALY_TECHNION-HEP-CREAM
#--site=ANALY_NIKHEF-ELPROD_SHORT,ANALY_NIKHEF-ELPROD"
#--site=ANALY_FZK,ANALY_BNL,ANALY_RAL"

echo ${CMD}

RESULT=`eval "${CMD}"`
echo ${RESULT}
