#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py --conditionsTag all:CONDBR2-BLKPA-2018-03 --ignoreErrors 'False' --autoConfiguration='everything' --maxEvents '250' --geometryVersion all:ATLAS-R2-2016-01-00-01 --steering='doRAWtoALL' --preExec 'r2a:from InDetRecExample.InDetJobProperties import InDetFlags; InDetFlags.useDynamicAlignFolders.set_Value_and_Lock(True);TriggerFlags.AODEDMSet="AODFULL";' --inputBSFile='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data18_13TeV.00348353.physics_Background.merge.RAW._lb0078._SFO-ALL._0001.1' --outputDAOD_IDNCBFile 'myDAOD_IDNCB.pool.root' --imf False
echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"
