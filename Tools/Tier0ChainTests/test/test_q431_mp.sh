#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py --AMI=q431 --athenaopts='--nprocs=2' --maxEvents=100 --outputTAGFile=myTAG.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root

