#!/bin/sh
set -e

Reco_tf.py  --outputHISTFile=myMergedMonitoring_Muons_0.root --maxEvents=500 --outputESDFile=myESD_Muons_0.pool.root --outputAODFile=myAOD_Muons_0.AOD.pool.root --outputTAGFile=myTAG_Muons_0.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_7TeV.00183021.physics_Muons.merge.RAW._lb0550._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_7TeV.00183021.physics_Muons.merge.RAW._lb0551._SFO-10._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_7TeV.00183021.physics_Muons.merge.RAW._lb0551._SFO-11._0001.1 --preExec='rec.doTrigger=False;'

