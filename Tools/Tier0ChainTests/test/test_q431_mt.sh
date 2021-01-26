#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8

Reco_tf.py --AMI=q431 --athenaopts='--threads=8' --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --imf False --maxEvents=100
echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 30 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees
echo "art-result: $? Diff"
