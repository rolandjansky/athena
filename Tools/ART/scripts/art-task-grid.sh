#!/bin/bash
# NOTE do NOT run with /bin/bash -x as the output is too big for gitlab-ci
# arguments: NIGHTLY_RELEASE, PROJECT, PLATFORM, NIGHTLY_TAG, SEQUENCE_TAG, PACKAGE, NUMBER_OF_TESTS
#
# example: 21.0 Athena x86_64-slc6-gcc62-opt 2017-02-26T2119 316236 Tier0ChainTests 32
#set -e

whoami
date

NIGHTLY_RELEASE=$1
PROJECT=$2
PLATFORM=$3
NIGHTLY_TAG=$4
SEQUENCE_TAG=$5
PACKAGE=$6
NUMBER_OF_TESTS=$7

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

lsetup panda

voms-proxy-init --rfc -noregen -cert ./grid.proxy -voms atlas

# change -VAL-Prod and others into -VAL 
NIGHTLY_RELEASE_SHORT=${NIGHTLY_RELEASE/-VAL-*/-VAL} 

asetup --platform=${PLATFORM} ${NIGHTLY_RELEASE_SHORT},${NIGHTLY_TAG},${PROJECT}

cd ./tmp/${PACKAGE}/run
OUTFILE="user.tcuhadar.atlas.art.${NIGHTLY_RELEASE_SHORT}.${PROJECT}.${PLATFORM}.${NIGHTLY_TAG}.${SEQUENCE_TAG}.${PACKAGE}"
CMD="pathena --noBuild --skipScout --trf \"./art.py job ${NIGHTLY_RELEASE_SHORT} ${PROJECT} ${PLATFORM} ${NIGHTLY_TAG} ${PACKAGE} %RNDM:0 %OUT.tar\" --split ${NUMBER_OF_TESTS} --outDS ${OUTFILE}"
#--site=ANALY_NIKHEF-ELPROD_SHORT,ANALY_NIKHEF-ELPROD"
#--site=ANALY_FZK,ANALY_BNL,ANALY_RAL"
echo ${CMD}

RESULT=`eval "${CMD}"`
echo ${RESULT}
