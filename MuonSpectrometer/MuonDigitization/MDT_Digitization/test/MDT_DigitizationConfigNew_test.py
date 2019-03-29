#!/usr/bin/env python
"""Run tests on MDT_DigitizationConfigNew.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from Digitization.DigitizationConfigFlags import createDigitizationCfgFlags
from OverlayCommonAlgs.OverlayConfigFlags import createOverlayCfgFlags
# muon imports
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
# MDT imports
from MDT_Digitization.MDT_DigitizationConfigNew import (
    MDT_RangeToolCfg, MDT_DigitizationToolCfg, MDT_DigitizerCfg,
    MDT_OverlayDigitizationToolCfg, MDT_OverlayDigitizerCfg,
    RT_Relation_DB_DigiToolCfg,
)
# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Configure
ConfigFlags.Input.Files = defaultTestFiles.HITS_SPECIAL
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.join(createDigitizationCfgFlags())
ConfigFlags.join(createOverlayCfgFlags())
ConfigFlags.lock()
# Function tests
# using __init__ to prevent errors
tool = MDT_RangeToolCfg(ConfigFlags)
tool = RT_Relation_DB_DigiToolCfg(ConfigFlags)
tacc = MDT_DigitizationToolCfg(ConfigFlags)
tacc.__init__()
tacc = MDT_OverlayDigitizationToolCfg(ConfigFlags)
tacc.__init__()
tacc = MDT_DigitizerCfg(ConfigFlags)
tacc.merge(MDT_OverlayDigitizerCfg(ConfigFlags))
tacc.__init__()
# Construct our accumulator to run
acc = MainServicesSerialCfg()
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(MuonGeoModelCfg(ConfigFlags))
acc.merge(MDT_DigitizerCfg(ConfigFlags))
# Add configuration to write HITS pool file
ItemList = [
    "MuonSimDataCollection#*",
    "MdtCsmContainer#*",
]
acc.merge(OutputStreamCfg(ConfigFlags, "RDO", ItemList=ItemList))
# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
acc.run(maxEvents=3)

