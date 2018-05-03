#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py --conditionsTag all:CONDBR2-BLKPA-2017-14 --ignoreErrors 'False' --autoConfiguration='everything' --maxEvents '500' --AMITag 'f908' --preExec  'r2a:from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.useDynamicAlignFolders.set_Value_and_Lock(True);TriggerFlags.AODEDMSet="AODFULL";rec.doTrigger=False;' --geometryVersion all:ATLAS-R2-2016-01-00-01 --steering='doRAWtoALL' --imf False --outputHISTFile=myMergedMonitoring_CosmicCalo_0.root --outputESDFile=myESD_CosmicCalo_0.pool.root --outputAODFile=myAOD_CosmicCalo_0.AOD.pool.root --outputTAGFile=myTAG_CosmicCalo_0.root --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_cos.00342075.physics_CosmicCalo.merge.RAW._lb1545._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_cos.00342075.physics_CosmicCalo.merge.RAW._lb1546._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_cos.00342075.physics_CosmicCalo.merge.RAW._lb1547._SFO-ALL._0001.1
echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"