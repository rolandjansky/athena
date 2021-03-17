#!/bin/sh
#
# art-description: RecoTrf with beamType=collisions
# art-type: grid

Reco_tf.py  --outputHISTFile=myMergedMonitoring_Muons_0.root --maxEvents=500 --outputESDFile=myESD_Muons_0.pool.root --outputAODFile=myAOD_Muons_0.AOD.pool.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00330470.physics_Main.merge.DRAW_ZMUMU.f843_m1831._0245.1 --preExec='rec.doTrigger=False;' --imf False --conditionsTag=CONDBR2-BLKPA-2018-03 --geometryVersion=ATLAS-R2-2016-01-00-01
echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"
