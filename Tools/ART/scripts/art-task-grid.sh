#!/bin/bash
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# NOTE do NOT run with /bin/bash -x as the output is too big for gitlab-ci
# arguments: PACKAGE, SEQUENCE_TAG, NUMBER_OF_TESTS, NIGHTLY_RELEASE, PROJECT, PLATFORM, NIGHTLY_TAG
#
# author : Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>
#
# example: Tier0ChainTests grid 316236 32 21.0 Athena x86_64-slc6-gcc62-opt 2017-02-26T2119
#set -e

USER=artprod

whoami
date

PACKAGE=$1
TYPE=$2
SEQUENCE_TAG=$3
NUMBER_OF_TESTS=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

export RUCIO_ACCOUNT=artprod

lsetup panda

voms-proxy-init --rfc -noregen -cert ./grid.proxy -voms atlas

# change -VAL-Prod and others into -VAL
NIGHTLY_RELEASE_SHORT=${NIGHTLY_RELEASE/-VAL-*/-VAL}

asetup --platform=${PLATFORM} ${NIGHTLY_RELEASE_SHORT},${NIGHTLY_TAG},${PROJECT}

# NOTE: for art-internal.py the current dir can be used as it is copied there
cd ./tmp/${PACKAGE}/run
OUTFILE="user.${USER}.atlas.${NIGHTLY_RELEASE_SHORT}.${PROJECT}.${PLATFORM}.${NIGHTLY_TAG}.${SEQUENCE_TAG}.${PACKAGE}"
CMD="pathena --noBuild --skipScout --trf \"./art-internal.py job grid ${PACKAGE} ${TYPE} ${SEQUENCE_TAG} %RNDM:0 %OUT.tar ${NIGHTLY_RELEASE_SHORT} ${PROJECT} ${PLATFORM} ${NIGHTLY_TAG}\" --split ${NUMBER_OF_TESTS} --outDS ${OUTFILE}"
#--site=ANALY_NIKHEF-ELPROD_SHORT,ANALY_NIKHEF-ELPROD"
#--site=ANALY_FZK,ANALY_BNL,ANALY_RAL"
echo ${CMD}

RESULT=`eval "${CMD}"`
echo ${RESULT}
