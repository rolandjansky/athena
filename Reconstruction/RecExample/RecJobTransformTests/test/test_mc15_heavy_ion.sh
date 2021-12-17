#!/bin/sh
#
# art-description: heavy ion reconstruction test from Andrzej Olszewski and Iwona Grabowska-Bold
# art-athena-mt: 4
# art-type: grid
# art-include: master/Athena

export TRF_ECHO=True; Reco_tf.py --athenaopts="--threads=8" --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc15_5TeV.420000.Hijing_PbPb_5p02TeV_MinBias_Flow_JJFV6.merge.HITS.e4962_s3004_s2921/HITS.09732013._000496.pool.root.1 --outputESDFile=ESD.pool.root --outputAODFile=AOD.pool.root --maxEvents=25 --conditionsTag 'all:OFLCOND-MC16-SDR-RUN2-08' --postInclude 'all:RecJobTransforms/UseFrontier.py,SimulationJobOptions/postInclude.HijingPars.py' --preExec 'all:rec.doHeavyIon.set_Value_and_Lock(True);rec.doZdc.set_Value_and_Lock(False);from CaloRec.CaloRecFlags import jobproperties;jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True);from AthenaMonitoring.DQMonFlags import jobproperties; jobproperties.DQMonFlagsCont.doHIMon.set_Value_and_Lock(False);from ParticleBuilderOptions.AODFlags import AODFlags;AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False); AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False); AODFlags.AddEgammaMuonTracksInAOD.set_Value_and_Lock(False);AODFlags.ThinNegativeEnergyCaloClusters.set_Value_and_Lock(False);' --autoConfiguration 'everything' --triggerConfig 'MCRECO:MC_HI_v3_tight_mc_prescale' --DataRunNumber '226000' --geometryVersion 'all:ATLAS-R2-2015-03-01-00'

RES=$?
echo "art-result: $RES Reco"

