#!/bin/sh
#
# art-description: MC21-style simulation using FullG4 for producing input samples needed for the Fast Calorimeter Simulation parametrisation
# art-type: build
# art-include: master/Athena

# Full chain with special flags
# ATLAS-R3S-2021-02-00-00 and OFLCOND-MC21-SDR-RUN3-03
Sim_tf.py --simulator 'FullG4MT'  \
--conditionsTag 'default:OFLCOND-MC21-SDR-RUN3-03' \
--physicsList 'FTFP_BERT_ATL_VALIDATION' \
--truthStrategy 'MC15aPlus' \
--postInclude "all:PyJobTransforms/UseFrontier.py" "EVNTtoHITS:ISF_FastCaloSimParametrization/ISF_FastCaloSimParametrization_SimPostInclude_1mm.py" \
--preExec 'EVNTtoHITS:from G4AtlasApps.SimFlags import simFlags;simFlags.VertexFromCondDB.set_Value_and_Lock(False)' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,ISF_FastCaloSimParametrization/ISF_FastCaloSimParametrization_SimPreInclude.py' \
--postExec "from AthenaCommon.AppMgr import ToolSvc;ToolSvc.ISF_EntryLayerToolMT.ParticleFilters=[]" \
--DataRunNumber '410000' \
--geometryVersion 'default:ATLAS-R3S-2021-02-00-00_VALIDATION' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc15_13TeV.431004.ParticleGun_pid22_E65536_disj_eta_m25_m20_20_25_zv_0.evgen.EVNT.e6556.EVNT.13283012._000001.pool.root.1" \
--outputHITSFile "Hits.pool.root" \
--maxEvents=2 \

echo  "art-result: $? simulation"

Reco_tf.py --inputHITSFile "Hits.pool.root" \
--outputESDFile ESD.pool.root \
--conditionsTag "default:OFLCOND-MC21-SDR-RUN3-03" \
--geometryVersion 'default:ATLAS-R3S-2021-02-00-00' \
--DataRunNumber='410000' \
--postExec 'HITtoRDO:ToolSvc.LArPileUpTool.CrossTalk=False' 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' 'all:conddb.addOverride("/LAR/BadChannels/BadChannels", "LARBadChannelsBadChannels-MC-empty")' 'all:conddb.addOverride("/TILE/OFL02/STATUS/ADC", "TileOfl02StatusAdc-EmptyBCh")' 'RAWtoESD:StreamESD.ItemList+=["ISF_FCS_Parametrization::FCS_StepInfoCollection#MergedEventSteps","LArHitContainer#*","TileHitVector#*", "TrackRecordCollection#CaloEntryLayer", "TrackRecordCollection#MuonEntryLayer"]' \
--postInclude "all:PyJobTransforms/UseFrontier.py" "HITtoRDO:ISF_FastCaloSimParametrization/ISF_FastCaloSimParametrization_DigiTilePostInclude.py" "HITtoRDO:ISF_FastCaloSimParametrization/ISF_FastCaloSimParametrization_DigiPostInclude.py" \
--preExec "all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)" "RAWtoESD:from CaloRec.CaloCellFlags import jobproperties;jobproperties.CaloCellFlags.doLArCellEmMisCalib=False;from RecExConfig.RecFlags import rec;rec.doTrigger=False" "HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.doCaloNoise=False" \
--maxEvents -1 \
--autoConfiguration everything

echo  "art-result: $? reconstruction"

FCS_Ntup_tf.py --inputESDFile 'ESD.pool.root' \
--outputNTUP_FCSFile 'calohit.root' \
--doG4Hits true 

echo  "art-result: $? fcs_ntup step"