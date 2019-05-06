#!/usr/bin/env python
"""Test various ComponentAccumulator Digitization configuration modules

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from Digitization.DigitizationConfigFlags import createDigitizationCfgFlags
from OverlayCommonAlgs.OverlayConfigFlags import createOverlayCfgFlags
from BCM_Digitization.BCM_DigitizationConfigNew import BCM_DigitizationCfg
from SCT_Digitization.SCT_DigitizationConfigNew import SCT_DigitizationHSCfg
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
ConfigFlags.lock()
# Construct our accumulator to run
acc = MainServicesSerialCfg()
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(BCM_DigitizationCfg(ConfigFlags))
acc.merge(SCT_DigitizationHSCfg(ConfigFlags))
acc.merge(MDT_DigitizerCfg(ConfigFlags))
acc.merge(TGC_DigitizerCfg(ConfigFlags))
acc.merge(RPC_DigitizerCfg(ConfigFlags))
acc.merge(CSC_DigitBuilderCfg(ConfigFlags))
# add output configuration
ItemList = [
    "InDetSimDataCollection#*",
    "BCM_RDO_Container#*",
    "InDet::SiClusterContainer#*",
    "SCT_RDO_Container#*",
    "MuonSimDataCollection#*",
    "MdtCsmContainer#*",
    "TgcRdoContainer#*",
    "RpcPadContainer#*",
    "CscSimDataCollection#CSC_SDO",
    "CscRawDataContainer#*",
]
acc.merge(OutputStreamCfg(ConfigFlags, "RDO", ItemList=ItemList))
# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
sc = acc.run(maxEvents=3)
# Success should be 0
sys.exit(not sc.isSuccess())

