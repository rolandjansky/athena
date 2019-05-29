#!/usr/bin/env python
"""Run tests on MDT_DigitizationConfigNew.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaConfiguration.AllConfigFlags import ConfigFlags
# MDT imports
from MDT_Digitization.MDT_DigitizationConfigNew import MDT_DigitizerCfg
# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Configure
ConfigFlags.Input.Files = defaultTestFiles.HITS_SPECIAL
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.lock()
# Construct our accumulator to run
acc = MainServicesSerialCfg()
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(MDT_DigitizerCfg(ConfigFlags))
# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
sc = acc.run(maxEvents=3)
# Success should be 0
sys.exit(not sc.isSuccess())

