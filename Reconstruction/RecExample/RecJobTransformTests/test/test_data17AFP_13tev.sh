#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data 2017, run 338480 has AFP detector included.
# art-athena-mt: 8
# art-type: grid
# art-include: master/Athena
# art-include: 22.0/Athena

Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data17_13TeV/data17_13TeV.00338480.physics_Main.daq.RAW._lb0309._SFO-4._0006.data --maxEvents 300 --AMI=r13286 --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHist.root

#Remember retval of transform as art result
RES=$?

xAODDigest.py myAOD.pool.root digest.txt

echo "art-result: $RES Reco"

