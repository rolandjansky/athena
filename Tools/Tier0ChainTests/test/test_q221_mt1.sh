#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena                                                                                                                           

Reco_tf.py \
--AMI=q221 \
--athenaopts='--threads=1' \
--preExec="all:from IOVDbSvc.CondDB import conddb; conddb.addOverride('/PIXEL/PixMapOverlay','PixMapOverlay-SIM-MC16-000-03');" \
--maxEvents=100 \
--outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --imf False

rc1=$?
echo "art-result: $rc1 Reco"


rc2=-9999
if [ ${rc1} -eq 0 ]
then
  Reco_tf.py --validationFlags 'doExample,doMET,doPFlow_FlowElements,doTau,doEgamma,doBtag,doZee,doJet,doTopoCluster,doMuon,doTrigMinBias,doTrigIDtrk,doTrigBphys,doTrigMET,doTrigJet,doTrigTau, doTrigEgamma,doTrigMuon,doTrigBjet,doTrigHLTResult' --inputAODFile=myAOD.pool.root  --outputNTUP_PHYSVALFile=myNTUP_PHYSVAL.root
  echo "art-result: $? PhysVal"

  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees --ignore-exit-code diff-pool --file "*AOD*" --file "*PHYSVAL*"
  rc2=$?
fi
echo  "art-result: ${rc2} Diff"


