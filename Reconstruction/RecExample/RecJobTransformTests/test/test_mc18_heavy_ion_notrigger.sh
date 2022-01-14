#!/bin/sh
#
# art-description: heavy ion reconstruction test from Sebastian Tapia
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena


export TRF_ECHO=True; Reco_tf.py \
--athenaopts="--threads=8" \
--inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_5TeV.420000.Hijing_PbPb_5p02TeV_MinBias_Flow_JJFV6.merge.HITS.e4962_a890_s3136/HITS.17784755._001903.pool.root.1 \
--outputESDFile=ESD.pool.root \
--outputAODFile=AOD.pool.root \
--maxEvents=20 \
--conditionsTag 'all:OFLCOND-MC16-SDR-RUN2-08' \
--postInclude 'all:RecJobTransforms/UseFrontier.py,SimulationJobOptions/postInclude.HijingPars.py' \
--postExec  'r2a:y=(StreamAOD.ItemList if "StreamAOD" in dir() else []);y+=["xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.SECOND_R.SECOND_LAMBDA.CENTER_MAG.CENTER_LAMBDA.FIRST_ENG_DENS.ENG_FRAC_MAX.ISOLATION.ENG_BAD_CELLS.N_BAD_CELLS.BADLARQ_FRAC.ENG_POS.AVG_LAR_Q.AVG_TILE_Q.EM_PROBABILITY.BadChannelList.CELL_SIGNIFICANCE.CELL_SIG_SAMPLING"];' \
--preExec  'r2a:from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.cutLevel.set_Value_and_Lock(4);jobproperties.Beam.bunchSpacing.set_Value_and_Lock(100);rec.doDPD.set_Value_and_Lock(True);' 'all:from AthenaMonitoring.DQMonFlags import jobproperties; jobproperties.DQMonFlagsCont.doHIMon.set_Value_and_Lock(False);rec.doZdc.set_Value_and_Lock(False);rec.doHeavyIon.set_Value_and_Lock(True);rec.doTrigger.set_Value_and_Lock(False);' \
--autoConfiguration 'everything' \
--DataRunNumber '313000' 

RES=$?
echo "art-result: $RES Reco"

