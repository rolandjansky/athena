#!/bin/sh
set -e

Reco_tf.py  --outputHISTFile=myMergedMonitoring_Main_0.root --AMI=x383 --maxEvents=50 --outputESDFile=myESD_Main_0.pool.root --geometryVersion=ATLAS-R2-2015-04-00-00 --outputAODFile=myAOD_Main_0.AOD.pool.root --outputTAGFile=myTAG_Main_0.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_cos.00289954.physics_Main.daq.RAW._lb0844._SFO-6._0001.data

