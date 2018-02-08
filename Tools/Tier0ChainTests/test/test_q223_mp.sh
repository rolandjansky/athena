#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py --AMI=q223 --athenaopts='--nprocs=2' --maxEvents=100 --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_comm.00264034.physics_MinBias.daq.RAW._lb0644._SFO-6._0001.data

