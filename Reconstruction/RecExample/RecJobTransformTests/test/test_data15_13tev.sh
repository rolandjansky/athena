#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data with all streams 2015
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena

Reco_tf.py --athenaopts="--threads=8" --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data15_13TeV.00283429.physics_Main.daq.RAW._lb0154._SFO-1._0001.data --maxEvents 300 --autoConfiguration everything --conditionsTag="CONDBR2-BLKPA-RUN2-06"  --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHist.root

RES=$?
echo "art-result: $RES Reco"
