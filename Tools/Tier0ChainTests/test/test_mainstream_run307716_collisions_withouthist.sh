#!/bin/sh
#
# art-description: RecoTrf with beamType=collisions
# art-type: grid

Reco_tf.py  --AMI=f741 --maxEvents=375 --outputESDFile=myESD_Main_2.pool.root --outputAODFile=myAOD_Main_2.AOD.pool.root --outputTAGFile=myTAG_Main_2.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_13TeV.00307716.physics_Main.daq.RAW._lb0220._SFO-1._0001.data,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_13TeV.00307716.physics_Main.daq.RAW._lb0221._SFO-1._0001.data --imf False
echo "art-result: $? Reco"

Reco_tf.py --autoConfiguration=everything  --inputESDFile=myESD_Main_2.pool.root --outputTAGFile=myTAG_Main_3.root
echo "art-result: $? ESDtoTAG"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"
