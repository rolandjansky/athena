#!/bin/sh
set -e

Reco_tf.py  --outputHISTFile=myMergedMonitoring_express_1.root --AMI=f628 --maxEvents=25 --outputESDFile=myESD_express_1.pool.root --outputAODFile=myAOD_express_1.AOD.pool.root --outputTAGFile=myTAG_express_1.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0222._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0223._SFO-ALL._0001.1

Reco_tf.py --inputAODFile=myAOD_express_1.AOD.pool.root --outputTAGFile=myTAG_express_1.root
