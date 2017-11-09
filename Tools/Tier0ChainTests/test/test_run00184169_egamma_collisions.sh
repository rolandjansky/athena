#!/bin/sh
#
# art-description: RecoTrf with beamType=collisions
# art-type: grid

Reco_tf.py  --outputHISTFile=myMergedMonitoring_Egamma_0.root --maxEvents=500 --outputESDFile=myESD_Egamma_0.pool.root --outputAODFile=myAOD_Egamma_0.AOD.pool.root --outputTAGFile=myTAG_Egamma_0.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_7TeV.00184169.physics_Egamma.merge.RAW._lb0900._SFO-10._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_7TeV.00184169.physics_Egamma.merge.RAW._lb0900._SFO-11._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_7TeV.00184169.physics_Egamma.merge.RAW._lb0900._SFO-12._0001.1 --preExec='rec.doTrigger=False;' --imf False
echo "art-result: $?"

SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD_Egamma_0.AOD.pool.root myESD_Egamma_0.pool.root
echo "art-result: $?"
