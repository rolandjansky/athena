#!/bin/sh
set -e

Reco_tf.py --AMI=q431 --outputTAGFile=myTAG.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_13TeV.00297447.physics_Main.daq.RAW._lb0555._SFO-1._0001.data

