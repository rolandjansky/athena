#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data 2017, early data, A3
# art-athena-mt: 4
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data17_13TeV.00324910.physics_Main.daq.RAW._lb0713._SFO-6._0001.data --maxEvents 300 --AMI=f908 --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHist.root

#Remember retval of transform as art result
RES=$?

xAODDigest.py myAOD.pool.root digest.txt

echo "art-result: $RES Reco"
return $RES

