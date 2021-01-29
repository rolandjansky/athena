#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8

# Added "preExec" here, because it was needed to disable dynamic alignment wrt q223 as discussed in ATLASRECTS-5783 (changed InDetGeometryFlags.useDynamicAlignFolders to false wrt q223).                                                      
# Updated to data18 input file (q223 uses data15 input file)                               

Reco_tf.py \
--athenaopts='--threads=8' \
--AMI=q223 \
--preExec 'all:larCondFlags.OFCShapeFolder.set_Value_and_Lock("4samples3bins17phases");from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.useBeamConstraint.set_Value_and_Lock(False);InDetFlags.doMinBias=True;InDetFlags.useDCS.set_Value_and_Lock(False);DQMonFlags.doStreamAwareMon=False;DQMonFlags.enableLumiAccess=False;DQMonFlags.doCTPMon=False;from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags;InDetGeometryFlags.useDynamicAlignFolders.set_Value_and_Lock(False);' \
--inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q223_input_data18/data18_comm.00353055.physics_MinBias.daq.RAW._lb0062._SFO-2._0001.data \
--conditionsTag='CONDBR2-BLKPA-RUN2-02' \
--geometryVersion='ATLAS-R2-2016-01-00-01' \
--maxEvents=100 \
--outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --imf False


echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees
echo "art-result: $? Diff"
