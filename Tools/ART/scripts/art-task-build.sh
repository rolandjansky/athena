#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
# arguments: RELEASE_BASE, PROJECT, PLATFORM, DATESTAMP
# author : Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>, Emil Obreshkov <Emil.Obreshkov@cern.ch>

date

echo "INFO: Script executed by $(whoami) on $(date)"

RELEASE_BASE=$1
PROJECT=$2
PLATFORM=$3
DATESTAMP=$4

BRANCH=`echo ${RELEASE_BASE} |tr "/" " " |awk '{print $5}'`
echo BRANCH $BRANCH

if [ -d /cvmfs/atlas.cern.ch/repo/sw/tdaq ]; then
   echo "WARNING: Setting TDAQ_RELEASE_BASE to /cvmfs/atlas.cern.ch/repo/sw/tdaq"
   export TDAQ_RELEASE_BASE=/cvmfs/atlas.cern.ch/repo/sw/tdaq
else
   echo "Error: Cannot find TDAQ software installation"
   return 1
fi

alias abeta="source /afs/cern.ch/atlas/software/dist/beta/AtlasSetup/scripts/asetup.sh"
abeta ${PROJECT} --platform ${PLATFORM} --releasebase ${RELEASE_BASE}/build/install --noLcgReleaseBase

# setup AtlasBuildBranch since that is not set bu the above asetup for the local build setup
export AtlasBuildBranch=$BRANCH
# for nightly testing point AtlasVersion to AtlasBuildStamp
export AtlasVersion=$AtlasBuildStamp

echo "TDAQ_RELEASE_BASE = " $TDAQ_RELEASE_BASE
echo "AtlasBuildBranch = " $AtlasBuildBranch
echo "AtlasProject = " $AtlasProject
echo "AtlasBuildStamp = " $AtlasBuildStamp
echo "AtlasVersion = " $AtlasVersion

ART_DIRECTORY=`which art.py`
ART_VERSION=`art.py --version`
echo "INFO: Using ART version ${ART_VERSION} in ${ART_DIRECTORY} directory"

# run build tests
SUBDIR=${BRANCH}/${PROJECT}/${PLATFORM}/${DATESTAMP}
OUTDIR="${RELEASE_BASE}/art-build/${SUBDIR}"
CMD="art.py run ${RELEASE_BASE}/build/install/${PROJECT}/*/InstallArea/${PLATFORM}/src ${OUTDIR}"
echo ${CMD}
RESULT=`eval "${CMD}"`
echo ${RESULT}

date

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
