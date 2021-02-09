#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-athena-mt: 8

# MT enabled for "RDOtoRDOTrigger, RAWtoESD, and ESDtoAOD as discussed in https://its.cern.ch/jira/browse/ATR-22498
Reco_tf.py \
--AMI=q440 \
--athenaopts "RDOtoRDOTrigger:--threads=8" "RAWtoESD:--threads=8" "ESDtoAOD:--threads=8" \
--maxEvents=100 \
--outputRDOFile=myRDO.pool.root --outputTAGFile=myTAG.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --imf False


echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"

