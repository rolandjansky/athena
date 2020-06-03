#!/bin/sh
#
# art-description: Reco_tf runs on splash events with all streams
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

export TRF_ECHO=True; Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data15_comm.00265545.physics_MinBias.merge.RAW._lb0048._SFO-ALL._0001.1 --autoConfiguration everything --conditionsTag="CONDBR2-BLKPA-2015-04" --geometryVersion="ATLAS-R2-2015-03-01-00" --outputESDFile myESD.pool.root --outputHISTFile myMergedMonitoring.root --maxEvents 300 --preExec 'rec.doTrigger=False;'

RES=$?
echo "art-result: $RES Reco"
return $RES

