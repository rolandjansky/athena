#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8

Reco_tf.py \
--AMI=q442 \
--athenaopts='--nprocs=2' \
--maxEvents=500 \
--outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root \
--imf False

rc1=$?
echo "art-result: $rc1 Reco"

rc2=-9999
if [ $rc1 -eq 0 ]
then
  art.py compare grid --entries 20 "$1" "$2" --mode=semi-detailed --order-trees --ignore-exit-code diff-pool
  rc2=$?
fi
echo "art-result: $rc2 Diff"
