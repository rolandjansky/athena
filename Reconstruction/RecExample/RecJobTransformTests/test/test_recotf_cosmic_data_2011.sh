#!/bin/sh
#
# art-description: Jamie Boyd - should be updated for 2015
# art-athena-mt: 4
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

export TRF_ECHO=True; Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data11_cos.00193024.physics_CosmicCalo.merge.RAW._lb0096._SFO-ALL._0001.1 --maxEvents 10 --autoConfiguration everything --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHIST.root --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True' 

RES=$?
echo "art-result: $RES Reco"


