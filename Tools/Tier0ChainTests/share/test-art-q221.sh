#!/bin/sh
set -e

Reco_tf.py --AMI=q221 --outputRDOFile=myRDO.pool.root --outputTAGFile=myTAG.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc15_13TeV.410500.PowhegPythia8EvtGen_A14_ttbar_hdamp172p5_nonallhad.simul.HITS.e4797_s2726.50events.pool.root

Reco_tf.py --validationFlags 'doExample,doMET,doPFlow,doTau,doTopoCluster,doEgamma,doBtag,doZee,doJet,doMuon,doTrigMinBias,doTrigIDtrk,doTrigBphys,doTrigMET,doTrigJet,doTrigTau, doTrigEgamma,doTrigMuon,doTrigBjet,doTrigHLTResult' --inputAODFile=myAOD.pool.root  --outputNTUP_PHYSVALFile=myNTUP_PHYSVAL.root
