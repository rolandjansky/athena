#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py  --outputHISTFile=myMergedMonitoring_Main_0.root --AMI=x383 --maxEvents=50 --outputESDFile=myESD_Main_0.pool.root --geometryVersion=ATLAS-R2-2015-04-00-00 --outputAODFile=myAOD_Main_0.AOD.pool.root --outputTAGFile=myTAG_Main_0.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_cos.00289954.physics_Main.daq.RAW._lb0844._SFO-6._0001.data --imf False
echo "art-result: $?"

SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD_Main_0.AOD.pool.root myESD_Main_0.pool.root
echo "art-result: $?"

art.py compare grid --days=3 $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD_Main_0.AOD.pool.root myESD_Main_0.pool.root
echo "art-result: $?"
