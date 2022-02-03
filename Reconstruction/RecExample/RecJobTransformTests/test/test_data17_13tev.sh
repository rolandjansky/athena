#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data 2017, early data, A3
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena

Reco_tf.py --athenaopts="--threads=8" --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data17_13TeV.00324910.physics_Main.daq.RAW._lb0713._SFO-6._0001.data --maxEvents 300 --AMI=r13286 --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHist.root --conditionsTag="CONDBR2-BLKPA-RUN2-06"  

#Remember retval of transform as art result
RES=$?

xAODDigest.py myAOD.pool.root digest.txt

echo "art-result: $RES Reco"

