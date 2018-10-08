#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data with all streams 2015
# art-type: grid

Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data15_13TeV.00283429.physics_Main.daq.RAW._lb0154._SFO-1._0001.data --maxEvents 300 --autoConfiguration everything --conditionsTag="CONDBR2-BLKPA-2016-14" --preExec 'rec.doTrigger=False;' --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputTAGFile myTAG.pool.root --outputHISTFile myHist.root

echo "art-result: $?"

