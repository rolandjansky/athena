#!/bin/sh
set -e

Reco_tf.py  --outputHISTFile=myMergedMonitoring_express_3.root --AMI=f628 --maxEvents=30 --outputESDFile=myESD_express_3.pool.root --outputAODFile=myAOD_express_3.AOD.pool.root --outputTAGFile=myTAG_express_3.root --ignoreErrors=False --steering=doRAWtoALL --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0226._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0227._SFO-ALL._0001.1

