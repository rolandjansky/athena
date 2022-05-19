#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py \
--AMI=q431 \
--athenaopts='--threads=1' \
--conditionsTag 'all:CONDBR2-BLKPA-RUN2-09' \
--maxEvents=100 \
--outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --imf False

echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 30 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees --ignore-exit-code diff-pool
echo "art-result: $? Diff"
