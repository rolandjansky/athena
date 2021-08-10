#!/bin/sh
#
# art-description: Run mc20e reco with pileup, and then run phys val on the output AOD.
# art-output: log.*
# art-athena-mt: 8
# art-type: grid
# art-include: master/Athena

#Unset these to get around ATLASJT-420                                                                                                                                                                                                                                         
unset ATHENA_CORE_NUMBER
unset ATHENA_PROC_NUMBER

Reco_tf.py --AMI=r12806 --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV/valid1/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3227/HITS.12560240._000287.pool.root.1 --inputRDO_BKGFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run2/large/mc20e_presampling.2k.latest.RDO.pool.root --maxEvents=500 --imf False --athenaopts "RAWtoESD:--threads=1" "ESDtoAOD:--threads=1" --conditionsTag="OFLCOND-MC16-SDR-RUN2-08"

RES=$?
echo "art-result: $RES Reco"

Reco_tf.py --inputAODFile=myAOD.pool.root --outputNTUP_PHYSVALFile physval.root  --validationFlags 'doExample,doMET,doPFlow_FlowElements,doTau,doEgamma,doBtag,doZee,doJet,doTopoCluster,doMuon,doTrigMinBias,doTrigIDtrk,doTrigBphys,doTrigMET,doTrigJet,doTrigTau, doTrigEgamma,doTrigMuon,doTrigBjet,doTrigHLTResult'
RES=$?
echo "art-result: $RES Reco"


