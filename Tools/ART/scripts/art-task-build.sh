#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
# arguments: RELEASE_BASE, PROJECT, PLATFORM
# author : Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>, Emil Obreshkov <Emil.Obreshkov@cern.ch>

echo "INFO: Script executed by $(whoami) on $(date)"

RELEASE_BASE=$1
PROJECT=$2
PLATFORM=$3

BRANCH="$(echo "${RELEASE_BASE}" | tr '/' ' ' | awk '{print $5}')"
echo BRANCH "${BRANCH}"

if [ -d /cvmfs/atlas.cern.ch/repo/sw/tdaq ]; then
   echo "WARNING: Setting TDAQ_RELEASE_BASE to /cvmfs/atlas.cern.ch/repo/sw/tdaq"
   export TDAQ_RELEASE_BASE=/cvmfs/atlas.cern.ch/repo/sw/tdaq
else
   echo "ERROR: Cannot find TDAQ software installation"
   return 1
fi

export ATLAS_LOCAL_ROOT_BASE="${ATLAS_LOCAL_ROOT_BASE:-/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase}"
# shellcheck source=/dev/null 
source "${ATLAS_LOCAL_ROOT_BASE}"/user/atlasLocalSetup.sh --quiet
if [ "${BRANCH}" == "master" ]; then
   lsetup -a testing asetup
   echo "INFO: setting up for master"
else
   lsetup -a current asetup
   echo "INFO: setting up for ${BRANCH}"
fi
asetup "${PROJECT}" --platform "${PLATFORM}" --releasebase "${RELEASE_BASE}"/build/install --noLcgReleaseBase


# setup AtlasBuildBranch since that is not set bu the above asetup for the local build setup
export AtlasBuildBranch=${BRANCH}
# for nightly testing point AtlasVersion to AtlasBuildStamp
export AtlasVersion="${AtlasBuildStamp}"

echo "TDAQ_RELEASE_BASE = ${TDAQ_RELEASE_BASE}"
echo "AtlasBuildBranch = ${AtlasBuildBranch}"
echo "AtlasProject = ${AtlasProject}"
echo "AtlasBuildStamp  = ${AtlasBuildStamp}"
echo "AtlasVersion = ${AtlasVersion}"

ART_DIRECTORY=$(command -v art.py)
ART_VERSION=$(art.py --version)
echo "INFO: Using ART version ${ART_VERSION} in ${ART_DIRECTORY} directory"

# automatic clean-up build-output EOS area
art-clean.py --eos --release --base-dir=/eos/atlas/atlascerngroupdisk/data-art/build-output --delete "${AtlasBuildBranch}" "${AtlasProject}" "${PLATFORM}" || true &

# configure EOS_MGM_URL
if [ -z "${EOS_MGM_URL}" ]; then
  echo "WARNING: EOS_MGM_URL variable is empty, setting it to root://eosatlas.cern.ch"
  export EOS_MGM_URL="root://eosatlas.cern.ch"
else
  echo "EOS_MGM_URL variable contains", ${EOS_MGM_URL}
fi

# run build tests
SUBDIR=${AtlasBuildBranch}/${AtlasProject}/${PLATFORM}/${AtlasBuildStamp}
OUTDIR="${RELEASE_BASE}/art-build/${SUBDIR}"
CMD="art.py run ${RELEASE_BASE}/build/install/${AtlasProject}/*/InstallArea/${PLATFORM}/src ${OUTDIR}"
echo "${CMD}"
RESULT=$(eval "${CMD}")
echo "${RESULT}"

# copy the test results to EOS area
TARGETDIR=/eos/atlas/atlascerngroupdisk/data-art/build-output/${SUBDIR}
if [[ ! -e ${TARGETDIR} ]]; then
  echo Target directory "${TARGETDIR}"
  eos mkdir -p "${TARGETDIR}"
  xrdcp -vr "${OUTDIR}" "${TARGETDIR}"
fi
