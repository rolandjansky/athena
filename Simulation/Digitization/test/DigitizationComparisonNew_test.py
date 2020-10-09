#!/usr/bin/env python
"""Run LAr digitization with ComponentAccumulator configuration and dump output

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from Digitization.DigitizationParametersConfig import writeDigitizationMetadata
from BCM_Digitization.BCM_DigitizationConfigNew import BCM_DigitizationCfg
from SCT_Digitization.SCT_DigitizationConfigNew import SCT_DigitizationCfg
from TRT_Digitization.TRT_DigitizationConfigNew import TRT_DigitizationCfg
from PixelDigitization.PixelDigitizationConfigNew import PixelDigitizationCfg
from MuonConfig.MDT_DigitizationConfig import MDT_DigitizationDigitToRDOCfg
from MuonConfig.TGC_DigitizationConfig import TGC_DigitizationDigitToRDOCfg
from MuonConfig.RPC_DigitizationConfig import RPC_DigitizationDigitToRDOCfg
from MuonConfig.CSC_DigitizationConfig import CSC_DigitizationDigitToRDOCfg
from LArDigitization.LArDigitizationConfigNew import LArTriggerDigitizationCfg
from TileSimAlgs.TileDigitizationConfig import TileDigitizationCfg, TileTriggerDigitizationCfg
from MCTruthSimAlgs.RecoTimingConfig import MergeRecoTimingObjCfg
from AthenaConfiguration.JobOptsDumper import JobOptsDumperCfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
ConfigFlags.Input.Files = [
    "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/"
    "valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/"
    "HITS.10504490._000425.pool.root.1"
   ]
ConfigFlags.Output.RDOFileName = "mc16a_ttbar.CA.RDO.pool.root"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01"
ConfigFlags.Beam.BunchSpacing = 25
ConfigFlags.LAr.ROD.NumberOfCollisions = 20
ConfigFlags.LAr.ROD.nSamples = 4
ConfigFlags.LAr.ROD.DoOFCPileupOptimization = True
ConfigFlags.LAr.ROD.FirstSample = 0
ConfigFlags.LAr.ROD.UseHighestGainAutoCorr = True
ConfigFlags.Digitization.Pileup = False
ConfigFlags.Digitization.DoDigiTruth = False
ConfigFlags.Digitization.TruthOutput = True
ConfigFlags.Digitization.RandomSeedOffset = 170
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.Concurrency.NumThreads = 1
ConfigFlags.Concurrency.NumConcurrentEvents = 1
ConfigFlags.Tile.BestPhaseFromCOOL = False
ConfigFlags.Tile.correctTime = False
ConfigFlags.lock()

# Construct our accumulator to run
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(PoolWriteCfg(ConfigFlags))
acc.merge(writeDigitizationMetadata(ConfigFlags))

# Old EventInfo conversion
if "EventInfo" not in ConfigFlags.Input.Collections:
    acc.merge(EventInfoCnvAlgCfg(ConfigFlags,
                                 inputKey="McEventInfo",
                                 outputKey="EventInfo"))

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
acc.merge(JobOptsDumperCfg(ConfigFlags, FileName="DigiComparisonConfig.txt"))
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
with open("DigitizationConfigCA.pkl", "wb") as f:
    acc.store(f)

# Execute and finish
sc = acc.run(maxEvents=20)
# Success should be 0
sys.exit(not sc.isSuccess())
