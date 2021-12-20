#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: 21.0/Athena
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-athena-mt: 8

Reco_tf.py \
--AMI=q221 \
--athenaopts='--threads=8' \
--conditionsTag 'all:OFLCOND-MC16-SDR-RUN2-09' \
--preExec 'all:from IOVDbSvc.CondDB import conddb; conddb.addOverride("/PIXEL/PixMapOverlay","PixMapOverlay-SIM-MC16-000-03"); rec.Commissioning.set_Value_and_Lock(True); from AthenaCommon.BeamFlags import jobproperties; jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.); from LArROD.LArRODFlags import larRODFlags; larRODFlags.nSamples.set_Value_and_Lock(4); from AthenaConfiguration.AllConfigFlags import ConfigFlags; ConfigFlags.Trigger.AODEDMSet="AODFULL"' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.overrideMetadata+=["PhysicsList"]' 'RAWtoESD:from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False' 'ESDtoAOD:from JetRec.JetRecFlags import jetFlags; jetFlags.writeJetsToAOD.set_Value_and_Lock(True); from METReconstruction.METRecoFlags import metFlags; metFlags.WriteMETAssocToOutput.set_Value_and_Lock(True)' \
--maxEvents=500 \
--outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --imf False

rc1=$?
echo "art-result: $rc1 Reco"



rc2=-9999
if [ ${rc1} -eq 0 ]
then
  Reco_tf.py --validationFlags 'doExample,doPFlow_FlowElements,doTau,doEgamma,doBtag,doZee,doJet,doTopoCluster,doMuon,doTrigMinBias,doTrigIDtrk,doTrigBphys,doTrigMET,doTrigJet,doTrigTau, doTrigEgamma,doTrigMuon,doTrigBjet,doTrigHLTResult' --inputAODFile=myAOD.pool.root  --outputNTUP_PHYSVALFile=myNTUP_PHYSVAL.root
  echo "art-result: $? PhysVal, no MET"

  Reco_tf.py --validationFlags 'doExample,doMET,doPFlow_FlowElements,doTau,doEgamma,doBtag,doZee,doJet,doTopoCluster,doMuon,doTrigMinBias,doTrigIDtrk,doTrigBphys,doTrigMET,doTrigJet,doTrigTau, doTrigEgamma,doTrigMuon,doTrigBjet,doTrigHLTResult' --inputAODFile=myAOD.pool.root  --outputNTUP_PHYSVALFile=myNTUP_PHYSVAL.root
  echo "art-result: $? PhysVal, with MET"

  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees --ignore-exit-code diff-pool --ignore-leave 'Token' --ignore-leave 'index_ref' --ignore-leave '(.*)_timings\.(.*)' --ignore-leave '(.*)_mems\.(.*)' --ignore-leave '(.*)TrigCostContainer(.*)' --ignore-leave '(.*)HLTNav_Summary_OnlineSlimmed(.*)'

  rc2=$?
fi
echo  "art-result: ${rc2} Diff"

