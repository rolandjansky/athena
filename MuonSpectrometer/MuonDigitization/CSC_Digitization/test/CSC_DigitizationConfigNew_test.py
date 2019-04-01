#!/usr/bin/env python
"""Run tests on CSC_DigitizationConfigNew.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from Digitization.DigitizationConfigFlags import createDigitizationCfgFlags
from OverlayCommonAlgs.OverlayConfigFlags import createOverlayCfgFlags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
# muon imports
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
# CSC imports
from MuonConfig.MuonCalibConfig import CscCoolStrSvcCfg
from MuonCondSvc.MuonCondSvcConf import CSCCondSummarySvc
from CSC_Digitization.CSC_DigitizationConfigNew import (
    CSC_RangeToolCfg, CSC_DigitizationToolCfg, CSC_DigitBuilderCfg,
    CSC_OverlayDigitizationToolCfg, CSC_OverlayDigitBuilderCfg,
)

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Configure
ConfigFlags.Input.Files = defaultTestFiles.HITS
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.join(createDigitizationCfgFlags())
ConfigFlags.join(createOverlayCfgFlags())
ConfigFlags.lock()
# Function tests
# using __init__ to prevent errors
tool = CSC_RangeToolCfg(ConfigFlags)
tacc = CSC_DigitizationToolCfg(ConfigFlags)
tacc.__init__()
tacc = CSC_OverlayDigitizationToolCfg(ConfigFlags)
tacc.__init__()
tacc = CSC_DigitBuilderCfg(ConfigFlags)
tacc.merge(CSC_OverlayDigitBuilderCfg(ConfigFlags))
tacc.__init__()
# Construct our accumulator to run
acc = MainServicesSerialCfg()
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(MuonGeoModelCfg(ConfigFlags))
acc.merge(CSC_DigitBuilderCfg(ConfigFlags))
# Add configuration to write HITS pool file
ItemList = [
    "CscSimDataCollection#CSC_SDO",
    "MuonSimDataCollection#*",
    "CscRawDataContainer#*",
]
acc.merge(OutputStreamCfg(ConfigFlags, "RDO", ItemList=ItemList))
# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
acc.run(maxEvents=3)

