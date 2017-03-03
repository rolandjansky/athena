#!/bin/sh
set -e

Reco_tf.py  --AMI=f628 --maxEvents=375 --outputESDFile=myESD_Main_2.pool.root --outputAODFile=myAOD_Main_2.AOD.pool.root --outputTAGFile=myTAG_Main_2.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.physics_Main.daq.RAW._lb0220._SFO-1._0001.data,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.physics_Main.daq.RAW._lb0221._SFO-1._0001.data

Reco_tf.py --autoConfiguration=everything  --inputESDFile=myESD_Main_2.pool.root --outputTAGFile=myTAG_Main_3.root
