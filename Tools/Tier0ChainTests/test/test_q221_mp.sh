#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-athena-mt: 8

Reco_tf.py \
--AMI=q221 \
--athenaopts='--nprocs=2' \
--conditionsTag 'all:OFLCOND-MC16-SDR-RUN2-08' \
--preExec 'all:from IOVDbSvc.CondDB import conddb; conddb.addOverride("/PIXEL/PixMapOverlay","PixMapOverlay-SIM-MC16-000-03"); rec.Commissioning.set_Value_and_Lock(True); from AthenaCommon.BeamFlags import jobproperties; jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.); from LArROD.LArRODFlags import larRODFlags; larRODFlags.nSamples.set_Value_and_Lock(4); from TriggerJobOpts.TriggerFlags import TriggerFlags; TriggerFlags.AODEDMSet="AODFULL"' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.overrideMetadata+=["PhysicsList"]' 'RAWtoESD:from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False' \
--maxEvents=100 \
--outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --imf False


echo "art-result: $? Reco"

Reco_tf.py --validationFlags 'doExample,doMET,doPFlow_FlowElements,doTau,doEgamma,doBtag,doZee,doJet,doTopoCluster,doMuon,doTrigMinBias,doTrigIDtrk,doTrigBphys,doTrigMET,doTrigJet,doTrigTau, doTrigEgamma,doTrigMuon,doTrigBjet,doTrigHLTResult' --inputAODFile=myAOD.pool.root  --outputNTUP_PHYSVALFile=myNTUP_PHYSVAL.root
echo "art-result: $? PhysVal"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 30 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"

