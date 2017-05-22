#!/bin/sh
set -e

Reco_tf.py  --outputHISTFile=myMergedMonitoring_express_0.root --AMI=f628 --maxEvents=25 --outputESDFile=myESD_express_0.pool.root --outputAODFile=myAOD_express_0.AOD.pool.root --outputTAGFile=myTAG_express_00.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0220._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0221._SFO-ALL._0001.1

Reco_tf.py --inputAODFile=myAOD_express_0.AOD.pool.root --outputTAGFile=myTAG_express_01.root

