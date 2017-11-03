#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py --AMI=q221 --outputRDOFile=myRDO.pool.root --outputTAGFile=myTAG.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc15_13TeV.410500.PowhegPythia8EvtGen_A14_ttbar_hdamp172p5_nonallhad.simul.HITS.e4797_s2726.50events.pool.root --imf False
echo "art-result: $?"

Reco_tf.py --validationFlags 'doExample,doMET,doPFlow,doTau,doEgamma,doBtag,doZee,doJet,doMuon,doTrigMinBias,doTrigIDtrk,doTrigBphys,doTrigMET,doTrigJet,doTrigTau, doTrigEgamma,doTrigMuon,doTrigBjet,doTrigHLTResult' --preExec "all:from InDetPhysValMonitoring.addTruthJets import addTruthJetsIfNotExising; addTruthJetsIfNotExising(\"AntiKt4TruthJets\");" --inputAODFile=myAOD.pool.root  --outputNTUP_PHYSVALFile=myNTUP_PHYSVAL.root
echo "art-result: $?"


SCRIPT_DIRECTORY=$1
PACKAGE=$2
TYPE=$3
TEST_NAME=$4
NIGHTLY_RELEASE=$5
PROJECT=$6
PLATFORM=$7
NIGHTLY_TAG=$8

art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD.pool.root myESD.pool.root
echo "art-result: $?"

art.py compare grid --days=2 $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME myAOD.pool.root myESD.pool.root
echo "art-result: $?"