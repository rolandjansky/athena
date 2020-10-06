#!/usr/bin/env python
"""ComponentAccumulator PileUp (PU) Digitization configuration test

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from Digitization.DigitizationParametersConfig import writeDigitizationMetadata
from MCTruthSimAlgs.RecoTimingConfig import MergeRecoTimingObjCfg
from AthenaConfiguration.JobOptsDumper import JobOptsDumperCfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
from Digitization.PileUpConfigNew import PileUpEventLoopMgrCfg
from DigitizationPUConfigNew_test_setup import (
    test_BeamIntensityPattern,
    test_fragment,
    pileUpCalc,
    makeBkgInputCol,
)
from BCM_Digitization.BCM_DigitizationConfigNew import BCM_DigitizationCfg
from SCT_Digitization.SCT_DigitizationConfigNew import SCT_DigitizationCfg
from TRT_Digitization.TRT_DigitizationConfigNew import TRT_DigitizationCfg
from PixelDigitization.PixelDigitizationConfigNew import PixelDigitizationCfg
from LArDigitization.LArDigitizationConfigNew import LArTriggerDigitizationCfg
from TileSimAlgs.TileDigitizationConfig import TileDigitizationCfg, TileTriggerDigitizationCfg
from MuonConfig.MDT_DigitizationConfig import MDT_DigitizationDigitToRDOCfg
from MuonConfig.TGC_DigitizationConfig import TGC_DigitizationDigitToRDOCfg
from MuonConfig.RPC_DigitizationConfig import RPC_DigitizationDigitToRDOCfg
from MuonConfig.CSC_DigitizationConfig import CSC_DigitizationDigitToRDOCfg

def EvtIdModifierSvc_add_modifier(svc, 
        run_nbr=None, evt_nbr=None, time_stamp=None, lbk_nbr=None, 
        nevts=1):
    if run_nbr is None:
        modify_run_nbr = 0
        run_nbr = 0
    else:
        modify_run_nbr = 1
        
    if evt_nbr is None:
        modify_evt_nbr = 0
        evt_nbr = 0
    else:
        modify_evt_nbr = 1
        
    if time_stamp is None:
        modify_time_stamp = 0
        time_stamp = 0
    else:
        modify_time_stamp = 1

    if lbk_nbr is None:
        modify_lbk_nbr = 0
        lbk_nbr = 0
    else:
        modify_lbk_nbr = 1

    mod_bit = int(0b0000
                | (modify_run_nbr << 0)
                | (modify_evt_nbr << 1)
                | (modify_time_stamp << 2)
                | (modify_lbk_nbr << 3))

    svc.Modifiers += [run_nbr, evt_nbr, time_stamp, lbk_nbr,
                    nevts, mod_bit]


# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

ConfigFlags.Exec.MaxEvents = 4

ConfigFlags.Input.Files = defaultTestFiles.HITS
ConfigFlags.Output.RDOFileName = "mc16d_ttbar.CA.RDO.pool.root"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"

ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"

ConfigFlags.Beam.BunchSpacing = 25
ConfigFlags.Beam.NumberOfCollisions = 20.

ConfigFlags.LAr.ROD.NumberOfCollisions = 20
ConfigFlags.LAr.ROD.nSamples = 4
ConfigFlags.LAr.ROD.DoOFCPileupOptimization = True
ConfigFlags.LAr.ROD.FirstSample = 0
ConfigFlags.LAr.ROD.UseHighestGainAutoCorr = True

ConfigFlags.Digitization.Pileup = False
ConfigFlags.Digitization.DoDigiTruth = True
ConfigFlags.Digitization.TruthOutput = True
ConfigFlags.Digitization.RandomSeedOffset = 170

ConfigFlags.Digitization.DoXingByXingPileUp = True # TODO get success with this flag enabled
ConfigFlags.Digitization.HighGainEMECIW = False

ConfigFlags.Digitization.PU.BunchSpacing = 25
ConfigFlags.Digitization.PU.CavernIgnoresBeamInt = True
ConfigFlags.Digitization.PU.NumberOfCavern = 0.0
ConfigFlags.Digitization.PU.NumberOfHighPtMinBias = 0.2099789464
ConfigFlags.Digitization.PU.NumberOfLowPtMinBias = 80.290021063135
ConfigFlags.Digitization.PU.BeamIntensityPattern = test_BeamIntensityPattern
ConfigFlags.Digitization.PU.RunAndLumiOverrideList = test_fragment

cols = makeBkgInputCol(ConfigFlags, defaultTestFiles.HITS_MINBIAS_HIGH,
                       ConfigFlags.Digitization.PU.NumberOfHighPtMinBias, True)
ConfigFlags.Digitization.PU.HighPtMinBiasInputCols = cols

cols = makeBkgInputCol(ConfigFlags, defaultTestFiles.HITS_MINBIAS_LOW,
                       ConfigFlags.Digitization.PU.NumberOfLowPtMinBias, True)
ConfigFlags.Digitization.PU.LowPtMinBiasInputCols = cols

ConfigFlags.Concurrency.NumThreads = 1
ConfigFlags.Concurrency.NumConcurrentEvents = 1
ConfigFlags.Tile.BestPhaseFromCOOL = False
ConfigFlags.Tile.correctTime = False

ConfigFlags.lock()

# test this flag
ConfigFlags.Sim.RunToTimestampDict

# Core components
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(PoolWriteCfg(ConfigFlags))

acc.merge(writeDigitizationMetadata(ConfigFlags))

# Signal-only truth information
from Digitization.PileUpToolsConfig import PileUpToolsCfg
from MCTruthSimAlgs.MCTruthSimAlgsConfigNew import (
    SignalOnlyMcEventCollCfg,
    MergeTruthJetsCfg,
    MergeMuonEntryLayerCfg,
    MergeCalibHitsCfg,
)

acc.merge(PileUpEventLoopMgrCfg(ConfigFlags))

acc.merge(SignalOnlyMcEventCollCfg(ConfigFlags))
acc.merge(MergeTruthJetsCfg(ConfigFlags))
acc.merge(MergeMuonEntryLayerCfg(ConfigFlags))
acc.merge(MergeCalibHitsCfg(ConfigFlags))

# Inner Detector
acc.merge(BCM_DigitizationCfg(ConfigFlags))
acc.merge(PixelDigitizationCfg(ConfigFlags))
acc.merge(SCT_DigitizationCfg(ConfigFlags))
acc.merge(TRT_DigitizationCfg(ConfigFlags))

# Calorimeter
acc.merge(LArTriggerDigitizationCfg(ConfigFlags))
acc.merge(TileDigitizationCfg(ConfigFlags))
acc.merge(TileTriggerDigitizationCfg(ConfigFlags))

# Muon Spectrometer
acc.merge(MDT_DigitizationDigitToRDOCfg(ConfigFlags))
acc.merge(TGC_DigitizationDigitToRDOCfg(ConfigFlags))
acc.merge(RPC_DigitizationDigitToRDOCfg(ConfigFlags))
acc.merge(CSC_DigitizationDigitToRDOCfg(ConfigFlags))

# Timing
acc.merge(MergeRecoTimingObjCfg(ConfigFlags))

# FIXME hack to match to buggy behaviour in old style configuration
OutputStreamRDO = acc.getEventAlgo("OutputStreamRDO")
OutputStreamRDO.ItemList += ["EventInfo#*"]
OutputStreamRDO.ItemList.remove("xAOD::EventInfo#EventInfo")
OutputStreamRDO.ItemList.remove("xAOD::EventAuxInfo#EventInfoAux.")
# FIXME this is marked "# Temporary for debugging MBTSHits" in DigiOutput.py
OutputStreamRDO.ItemList += ["TileHitVector#MBTSHits"]
# for Tile
# new style configures these, but they are left default in old config
TilePulseForTileMuonReceiver = acc.getEventAlgo("TilePulseForTileMuonReceiver")
TilePulseForTileMuonReceiver.TileRawChannelBuilderMF.TimeMaxForAmpCorrection = 25.
TilePulseForTileMuonReceiver.TileRawChannelBuilderMF.TimeMinForAmpCorrection = -25.
TileRChMaker = acc.getEventAlgo("TileRChMaker")
TileRChMaker.TileRawChannelBuilderFitOverflow.TimeMaxForAmpCorrection = 25.
TileRChMaker.TileRawChannelBuilderFitOverflow.TimeMinForAmpCorrection = -25.

# Dump config
acc.merge(JobOptsDumperCfg(ConfigFlags, FileName="DigiPUConfigCA.txt"))
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True, summariseProps=True)
ConfigFlags.dump()
# print services
from AthenaConfiguration.ComponentAccumulator import filterComponents
for s, _ in filterComponents(acc._services):
    acc._msg.info(s)
# print conditions
for s in acc._conditionsAlgs:
    acc._msg.info(s)

# dump pickle
with open("DigiPUConfigCA.pkl", "wb") as f:
    acc.store(f)

# TODO uncomment running in the test once successful
# Execute and finish
#sc = acc.run(maxEvents=ConfigFlags.Exec.MaxEvents)
# Success should be 0
#sys.exit(not sc.isSuccess())
