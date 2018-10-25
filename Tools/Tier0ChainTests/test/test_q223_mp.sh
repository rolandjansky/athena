#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

Reco_tf.py --AMI=q223 --athenaopts='--nprocs=2' --maxEvents=100 --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root
echo "art-result: $? Reco"

