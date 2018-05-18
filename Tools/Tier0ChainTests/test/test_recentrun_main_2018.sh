#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py --AMI=f921 --maxEvents=250 --outputESDFile=myESD_Main.pool.root --outputAODFile=myAOD_Main.AOD.pool.root --outputTAGFile=myTAG_Main.root --outputHISTFile=myMergedMonitoring_Main.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data18_13TeV.00348618.physics_Main.daq.RAW._lb0220._SFO-1._0001.data
echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"
