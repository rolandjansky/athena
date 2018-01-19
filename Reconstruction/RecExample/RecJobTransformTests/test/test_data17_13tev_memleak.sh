#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data with Hephaestos for checking for memory leaks 2017
# art-type: grid

export LD_PRELOAD=$LCG_RELEASE_BASE/libunwind/5c2cade-76996/$LCG_PLATFORM/lib/libunwind.so; Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data17_13TeV.00324910.physics_Main.daq.RAW/data17_13TeV.00324910.physics_Main.daq.RAW._lb0713._SFO-6._0001.data --maxEvents 10 --autoConfiguration everything --conditionsTag="CONDBR2-BLKPA-2017-08" --preExec 'rec.doTrigger=False' --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputTAGFile myTAG.pool.root --athenaopts '--stdcmalloc --leak-check-execute'

echo "art-result: $?"

