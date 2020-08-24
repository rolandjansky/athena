#!/usr/bin/env python
"""Test various ComponentAccumulator Digitization configuration modules

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
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
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

# Configure
ConfigFlags.Input.Files = defaultTestFiles.HITS_SPECIAL
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.Concurrency.NumThreads = 1
ConfigFlags.Concurrency.NumConcurrentEvents=1
ConfigFlags.Beam.NumberOfCollisions = 0.
ConfigFlags.lock()

# Core components
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

# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
sc = acc.run(maxEvents=3)
# Success should be 0
sys.exit(not sc.isSuccess())
