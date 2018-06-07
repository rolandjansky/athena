#!/bin/sh
#
# art-description: Reco_tf runs on 8TeV collision data with Hephaestos for checking for memory leaks
# art-type: grid

export LD_PRELOAD=/cvmfs/atlas-nightlies.cern.ch/repo/sw/21.0/sw/lcg/releases/libunwind/5c2cade-76996/x86_64-slc6-gcc62-opt/lib/libunwind.so; Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data12_8TeV.00213092.physics_JetTauEtmiss.merge.RAW._lb0285._SFO-1._0001.1 --maxEvents 5 --autoConfiguration everything --conditionsTag="COMCOND-BLKPA-RUN1-07" --preExec 'rec.doTrigger=False' --outputESDFile myESD.pool.root --athenaopts '--stdcmalloc --leak-check-execute'

