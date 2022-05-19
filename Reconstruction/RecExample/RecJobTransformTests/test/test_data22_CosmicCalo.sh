#!/bin/sh
#
# art-description: Reco_tf runs on 2020 CosmicCalo stream, without trigger.
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena
# art-include: 22.0/Athena

Reco_tf.py --AMI=x636 --athenaopts="--threads=8" --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data22_calib/data22_calib.00412340.physics_CosmicCalo.merge.RAW/data22_calib.00412340.physics_CosmicCalo.merge.RAW._lb0016._SFO-16._0001.1 --maxEvents=300 --outputESDFile=myESD.pool.root --outputAODFile=myAOD.pool.root 
RES=$?
echo "art-result: $RES Reco"



