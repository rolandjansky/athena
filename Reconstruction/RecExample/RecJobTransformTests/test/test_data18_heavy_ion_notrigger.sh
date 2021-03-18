#!/bin/sh
#
# art-description: heavy ion reconstruction test from Sebastian Tapia 
# art-type: grid
# art-include: master/Athena


export TRF_ECHO=True; Reco_tf.py \
--athenaopts="--threads=1" \
--inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data18_hi.00367384.physics_HardProbes.daq.RAW._lb0145._SFO-8._0001.data \
--outputESDFile=ESD.pool.root \
--outputAODFile=AOD.pool.root \
--maxEvents=20 \
--conditionsTag 'default:CONDBR2-BLKPA-2018-12' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--autoConfiguration 'everything' \
--steering='doRAWtoALL' \
--postExec  'r2a:y=(StreamAOD.ItemList if "StreamAOD" in dir() else []);y+=["xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.SECOND_R.SECOND_LAMBDA.CENTER_MAG.CENTER_LAMBDA.FIRST_ENG_DENS.ENG_FRAC_MAX.ISOLATION.ENG_BAD_CELLS.N_BAD_CELLS.BADLARQ_FRAC.ENG_POS.AVG_LAR_Q.AVG_TILE_Q.EM_PROBABILITY.BadChannelList.CELL_SIGNIFICANCE.CELL_SIG_SAMPLING"];' \
--preExec  'r2a:from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.cutLevel.set_Value_and_Lock(4);from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags;InDetGeometryFlags.useDynamicAlignFolders.set_Value_and_Lock(True);jobproperties.Beam.bunchSpacing.set_Value_and_Lock(100);rec.doTrigger.set_Value_and_Lock(False);TriggerFlags.AODEDMSet="AODFULL";rec.doHeavyIon.set_Value_and_Lock(True);rec.doDPD.set_Value_and_Lock(True); from CaloRec.CaloRecFlags import jobproperties;jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True);from AthenaMonitoring.DQMonFlags import jobproperties; jobproperties.DQMonFlagsCont.doHIMon.set_Value_and_Lock(False);AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False);AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False);AODFlags.AddEgammaMuonTracksInAOD.set_Value_and_Lock(False);AODFlags.ThinNegativeEnergyCaloClusters.set_Value_and_Lock(False);rec.doZdc.set_Value_and_Lock(False);' 
RES=$?
echo "art-result: $RES Reco"
return $RES
