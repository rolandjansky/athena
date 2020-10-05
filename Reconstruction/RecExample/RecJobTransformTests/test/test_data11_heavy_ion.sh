#!/bin/sh
#
# art-description: heavy ion reconstruction test from Andrzej Olszewski and Iwona Grabowska-Bold
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


export TRF_ECHO=True; Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data11_hi.00193321.physics_HardProbes.merge.RAW._lb0050._SFO-9._0002.1 --outputESDFile ESD.pool.root --outputAODFile AOD.pool.root --conditionsTag COMCOND-BLKPA-RUN1-06 --geometryVersion ATLAS-R1-2011-02-00-00 --autoConfiguration=everything --maxEvents='25'

RES=$?
echo "art-result: $RES Reco"
return $RES
