#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py --outputHISTFile=myMergedMonitoring_CosmicCalo_0.root --maxEvents=500 --outputESDFile=myESD_CosmicCalo_0.pool.root --outputAODFile=myAOD_CosmicCalo_0.AOD.pool.root --outputTAGFile=myTAG_CosmicCalo_0.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_cos.00342075.physics_CosmicCalo.merge.RAW._lb1545._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_cos.00342075.physics_CosmicCalo.merge.RAW._lb1546._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_cos.00342075.physics_CosmicCalo.merge.RAW._lb1547._SFO-ALL._0001.1 --preExec='rec.doTrigger=False;' --imf False
echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"