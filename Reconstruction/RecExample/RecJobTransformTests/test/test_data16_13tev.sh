#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data with all streams 2016
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena

Reco_tf.py --athenaopts="--threads=8" --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data16_13TeV.00310809.physics_Main.daq.RAW._lb1219._SFO-2._0001.data --maxEvents 300 --autoConfiguration everything --conditionsTag="CONDBR2-BLKPA-RUN2-06"  --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHist.root

RES=$?
echo "art-result: $RES Reco"
