#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py  --outputHISTFile=myMergedMonitoring_express_0.root --AMI=f628 --maxEvents=25 --outputESDFile=myESD_express_0.pool.root --outputAODFile=myAOD_express_0.AOD.pool.root --outputTAGFile=myTAG_express_0.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0220._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0221._SFO-ALL._0001.1 --imf False
echo "art-result: $?"

Reco_tf.py --inputAODFile myAOD_express_0.AOD.pool.root --outputTAGFile myTAG_express_0.root
echo "art-result: $?"


SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD_express_0.AOD.pool.root myESD_express_0.pool.root
echo "art-result: $?"

art.py compare grid --days=3 $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD_express_0.AOD.pool.root myESD_express_0.pool.root
echo "art-result: $?"
