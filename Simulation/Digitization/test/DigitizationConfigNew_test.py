#!/usr/bin/env python
"""Test various ComponentAccumulator Digitization configuration modules

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from Digitization.DigitizationConfigFlags import createDigitizationCfgFlags
from OverlayCommonAlgs.OverlayConfigFlags import createOverlayCfgFlags
from BCM_Digitization.BCM_DigitizationConfigNew import BCM_DigitizationCfg
from SCT_Digitization.SCT_DigitizationConfigNew import SCT_DigitizationHSCfg
from TRT_Digitization.TRT_DigitizationConfigNew import TRT_DigitizationHSCfg
from PixelDigitization.PixelDigitizationConfigNew import PixelDigitizationHSCfg
from MDT_Digitization.MDT_DigitizationConfigNew import MDT_DigitizerCfg
from TGC_Digitization.TGC_DigitizationConfigNew import TGC_DigitizerCfg
from RPC_Digitization.RPC_DigitizationConfigNew import RPC_DigitizerCfg
from CSC_Digitization.CSC_DigitizationConfigNew import CSC_DigitBuilderCfg

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Configure
ConfigFlags.join(createDigitizationCfgFlags())
ConfigFlags.join(createOverlayCfgFlags())
ConfigFlags.Input.Files = defaultTestFiles.HITS_SPECIAL
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.Concurrency.NumThreads = 1
ConfigFlags.GeoModel.Type = "BrlIncl4.0_ref"
ConfigFlags.Beam.NumberOfCollisions = 0.
ConfigFlags.lock()
# Construct our accumulator to run
acc = MainServicesSerialCfg()
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(BCM_DigitizationCfg(ConfigFlags))
acc.merge(SCT_DigitizationHSCfg(ConfigFlags))
acc.merge(TRT_DigitizationHSCfg(ConfigFlags))
acc.merge(PixelDigitizationHSCfg(ConfigFlags))
acc.merge(MDT_DigitizerCfg(ConfigFlags))
acc.merge(TGC_DigitizerCfg(ConfigFlags))
acc.merge(RPC_DigitizerCfg(ConfigFlags))
acc.merge(CSC_DigitBuilderCfg(ConfigFlags))
# FIXME once OutputStreamCfg merges correctly
from BCM_Digitization.BCM_DigitizationConfigNew import BCM_ItemList
from SCT_Digitization.SCT_DigitizationConfigNew import SCT_ItemList
from TRT_Digitization.TRT_DigitizationConfigNew import TRT_ItemList
from PixelDigitization.PixelDigitizationConfigNew import PixelItemList
from MDT_Digitization.MDT_DigitizationConfigNew import MDT_ItemList
from TGC_Digitization.TGC_DigitizationConfigNew import TGC_ItemList
from RPC_Digitization.RPC_DigitizationConfigNew import RPC_ItemList
from CSC_Digitization.CSC_DigitizationConfigNew import CSC_ItemList
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
ItemList = BCM_ItemList() + SCT_ItemList() + TRT_ItemList() + PixelItemList() + MDT_ItemList() + TGC_ItemList() + RPC_ItemList() + CSC_ItemList()
ItemList = list(set(ItemList)) # remove duplicates
acc.merge(OutputStreamCfg(ConfigFlags, "RDO", ItemList))
# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
sc = acc.run(maxEvents=3)
# Success should be 0
sys.exit(not sc.isSuccess())

