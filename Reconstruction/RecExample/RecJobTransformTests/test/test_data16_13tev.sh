#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data with all streams 2016
# art-type: grid

Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data16_13TeV.00310809.physics_Main.daq.RAW._lb1219._SFO-2._0001.data --maxEvents 300 --autoConfiguration everything --conditionsTag="CONDBR2-BLKPA-2016-19" --preExec 'rec.doTrigger=False;' --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputTAGFile myTAG.pool.root --outputHISTFile myHist.root

echo "art-result: $?"

