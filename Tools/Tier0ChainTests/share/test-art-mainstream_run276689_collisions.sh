#!/bin/sh
set -e

Reco_tf.py  --outputHISTFile=myMergedMonitoring_Main_0.root --AMI=f628 --maxEvents=375 --outputESDFile=myESD_Main_0.pool.root --outputAODFile=myAOD_Main_0.AOD.pool.root --outputTAGFile=myTAG_Main_0.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.physics_Main.daq.RAW._lb0220._SFO-1._0001.data,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.physics_Main.daq.RAW._lb0221._SFO-1._0001.data

