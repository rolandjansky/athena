#!/bin/bash
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# arguments: RELEASE_BASE, PROJECT, PLATFORM, DATESTAMP
# author : Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>, Emil Obreshkov <Emil.Obreshkov@cern.ch>

echo "Script executed by $(whoami) on $(date)"

RELEASE_BASE=$1
PROJECT=$2
PLATFORM=$3
DATESTAMP=$4

echo Release base $RELEASE_BASE
echo Project $PROJECT
echo Platform $PLATFORM
echo Date $DATESTAMP

BRANCH=`echo $RELEASE_BASE |tr "/" " " |awk '{print $5}'`

# setup for the build
[[ "${ATLAS_LOCAL_ROOT_BASE}" = "" ]] && export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh --quiet
lsetup asetup
asetup none,cmakesetup --platform ${PLATFORM}
source ${RELEASE_BASE}/build/install/${PROJECT}/*/InstallArea/${PLATFORM}/setup.sh

# run build tests
SUBDIR=${BRANCH}/${PROJECT}/${PLATFORM}/${DATESTAMP}
OUTDIR="art-build/${SUBDIR}"
CMD="art.py run ${RELEASE_BASE} ${OUTDIR}"
echo ${CMD}
RESULT=`eval "${CMD}"`
echo ${RESULT}

# copy the test results to EOS area
TARGETDIR=/eos/atlas/atlascerngroupdisk/art-build/${SUBDIR}
if [[ ! -e ${TARGETDIR} ]]; then
  eos mkdir -p ${TARGETDIR}
  xrdcp -vr $OUTDIR $TARGETDIR
fi
