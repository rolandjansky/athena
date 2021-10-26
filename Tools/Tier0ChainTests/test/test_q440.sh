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

# MT enabled for "RDOtoRDOTrigger, RAWtoESD, and ESDtoAOD as discussed in https://its.cern.ch/jira/browse/ATR-22498
Reco_tf.py \
--AMI=q440 \
--athenaopts "RDOtoRDOTrigger:--threads=8" "RAWtoESD:--threads=8" "ESDtoAOD:--threads=8" \
--conditionsTag 'all:OFLCOND-MC16-SDR-RUN2-09' \
--maxEvents=500 \
--outputRDOFile=myRDO.pool.root --outputTAGFile=myTAG.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --imf False


rc1=$?
echo "art-result: $rc1 Reco"

rc2=-9999
if [ ${rc1} -eq 0 ]
then
  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees --ignore-exit-code diff-pool --ignore-leave 'Token' --ignore-leave 'index_ref' --ignore-leave '(.*)_timings\.(.*)' --ignore-leave '(.*)_mems\.(.*)' --ignore-leave '(.*)TrigCostContainer(.*)' --ignore-leave '(.*)HLTNav_Summary_OnlineSlimmed(.*)'
  rc2=$?
fi
echo  "art-result: ${rc2} Diff"
