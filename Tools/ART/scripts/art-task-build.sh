#!/bin/bash
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# arguments: RELEASE_BASE, PROJECT, PLATFORM, DATESTAMP
# author : Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>, Emil Obreshkov <Emil.Obreshkov@cern.ch>

echo "INFO: Script executed by $(whoami) on $(date)"

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

if [ -d /cvmfs/atlas.cern.ch/repo/sw/tdaq ]; then
   echo "WARNING: Setting TDAQ_RELEASE_BASE to /cvmfs/atlas.cern.ch/repo/sw/tdaq"
   export TDAQ_RELEASE_BASE=/cvmfs/atlas.cern.ch/repo/sw/tdaq
else
   echo "Error: Cannot find TDAQ software installation"
   return 1
fi
source ${RELEASE_BASE}/build/install/${PROJECT}/*/InstallArea/${PLATFORM}/setup.sh

# setup as if asetup was run
export AtlasBuildBranch=$BRANCH
export AtlasProject=$PROJECT
export ${AtlasProject}_PLATFORM=$PLATFORM
export AtlasBuildStamp=$DATESTAMP

ART_DIRECTORY=`which art.py`
ART_VERSION=`art.py --version`
echo "INFO: Using ART version ${ART_VERSION} in ${ART_DIRECTORY} directory"

# run build tests
SUBDIR=${BRANCH}/${PROJECT}/${PLATFORM}/${DATESTAMP}
OUTDIR="${RELEASE_BASE}/art-build/${SUBDIR}"
CMD="art.py run ${RELEASE_BASE}/build/build/${PROJECT} ${OUTDIR}"
echo ${CMD}
RESULT=`eval "${CMD}"`
echo ${RESULT}

# copy the test results to EOS area
if [ -z "${EOS_MGM_URL}" ]; then
  echo "WARNING: EOS_MGM_URL variable is empty, setting it to root://eosatlas.cern.ch"
  export EOS_MGM_URL="root://eosatlas.cern.ch"
else
  echo "EOS_MGM_URL variable contains", ${EOS_MGM_URL}
fi

TARGETDIR=/eos/atlas/atlascerngroupdisk/data-art/build-output/${SUBDIR}
if [[ ! -e ${TARGETDIR} ]]; then
  echo Target directory ${TARGETDIR}
  eos mkdir -p ${TARGETDIR}
  xrdcp -vr ${OUTDIR} ${TARGETDIR}
fi
