#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8                                                                                                                                     

Reco_tf.py \
--AMI=q431 \
--athenaopts='--threads=8' \
--maxEvents=100 \
--outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --imf False

echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 30 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"
