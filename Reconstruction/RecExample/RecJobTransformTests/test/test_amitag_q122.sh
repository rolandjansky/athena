#!/bin/sh
#
# art-description: Reco_tf runs with AMI configTag q122
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


Reco_tf.py --AMI=q122 --DataRunNumber 00191920 --outputESDFile=myESD.pool.root --outputAODFile=myAOD.pool.root --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts

RES=$?
echo "art-result: $RES Reco"
return $RES
