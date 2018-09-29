#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py --AMI=r10786 --maxEvents=25 --ignoreErrors=False --inputBSFile='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00334678.physics_Main.recon.DRAW_RPVLL.r10203/DRAW_RPVLL.12882891._007592.pool.root.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00334678.physics_Main.recon.DRAW_RPVLL.r10203/DRAW_RPVLL.12882891._004274.pool.root.1' --outputAODFile=myDAOD_RPVLL.pool.root --imf=False

echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"
