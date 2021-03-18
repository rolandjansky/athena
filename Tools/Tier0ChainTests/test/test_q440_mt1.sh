#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

# MT enabled for RDOtoRDOTrigger, RAWtoESD, and ESDtoAOD, as discussed in https://its.cern.ch/jira/browse/ATR-22498
Reco_tf.py \
--AMI=q440 \
--athenaopts "RDOtoRDOTrigger:--threads=1" "RAWtoESD:--threads=1" "ESDtoAOD:--threads=1" \
--maxEvents=100 \
--outputRDOFile=myRDO.pool.root --outputTAGFile=myTAG.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --imf False



rc1=$?
echo "art-result: $rc1 Reco"

rc2=-9999
if [ ${rc1} -eq 0 ]
then
  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --file *AOD*
  rc2=$?
fi
echo  "art-result: ${rc2} Diff"

