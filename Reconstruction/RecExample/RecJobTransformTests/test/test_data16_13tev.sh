#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data with all streams 2016
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data16_13TeV.00310809.physics_Main.daq.RAW._lb1219._SFO-2._0001.data --maxEvents 300 --autoConfiguration everything --conditionsTag="CONDBR2-BLKPA-2016-19" --preExec 'rec.doTrigger=False;' --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHist.root

RES=$?
echo "art-result: $RES Reco"
return $RES
