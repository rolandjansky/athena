#!/usr/bin/env python
"""Run tests on BCM_DigitizationConfig.py

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from BCM_Digitization.BCM_DigitizationConfig import BCM_DigitizationCfg

# Set up logging and new style config
log.setLevel(DEBUG)
# Configure
ConfigFlags.Input.Files = defaultTestFiles.HITS_RUN2
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.lock()
# Construct our accumulator to run
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))
acc.merge(BCM_DigitizationCfg(ConfigFlags))
# Dump config
acc.getService("StoreGateSvc").Dump=True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
sc = acc.run(maxEvents=3)
# Success should be 0
sys.exit(not sc.isSuccess())

