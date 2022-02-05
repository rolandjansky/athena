#!/bin/sh
#
# art-description: Reco_tf runs on splash events with all streams
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena

#Monitoring is disabled because it tries to use the trigger information, which is disabled.
export TRF_ECHO=True; Reco_tf.py --athenaopts="--threads=8" --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data15_1beam/data15_1beam.00260466.physics_L1Calo.merge.RAW._lb1380._SFO-ALL._0001.1 --autoConfiguration everything --conditionsTag="CONDBR2-BLKPA-RUN2-06" --geometryVersion="ATLAS-R2-2015-03-01-00" --outputESDFile myESD.pool.root --maxEvents 300 --preExec 'rec.doTrigger=False;from AthenaMonitoring.DQMonFlags import jobproperties;jobproperties.DQMonFlagsCont.doMonitoring.set_Value_and_Lock(False)'

RES=$?
echo "art-result: $RES Reco"

