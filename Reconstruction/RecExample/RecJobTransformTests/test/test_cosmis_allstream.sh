#!/bin/sh
#
# art-description: Reco_tf runs on cosmics with all streams
# art-memory: 6000
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data11_cos.00193024.physics_CosmicCalo.merge.RAW._lb0096._SFO-ALL._0001.1 --maxEvents 300 --autoConfiguration everything --conditionsTag="COMCOND-BLKPA-RUN1-07" --preExec="rec.doTrigger=False;" --outputESDFile myESD.pool.root --outputHISTFile myMergedMonitoring.root 

RES=$?
echo "art-result: $RES Reco"
return $RES
