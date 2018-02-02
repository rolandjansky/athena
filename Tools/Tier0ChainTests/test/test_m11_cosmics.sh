#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py  --outputHISTFile=myMergedMonitoring_Main_0.root --AMI=x383 --maxEvents=50 --outputESDFile=myESD_Main_0.pool.root --geometryVersion=ATLAS-R2-2015-04-00-00 --outputAODFile=myAOD_Main_0.AOD.pool.root --outputTAGFile=myTAG_Main_0.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_cos.00289954.physics_Main.daq.RAW._lb0844._SFO-6._0001.data --imf False
echo "art-result: $?"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $?"

art.py compare grid --days=3 --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $?"
