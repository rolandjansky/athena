#!/usr/bin/env python
"""Run tests on BCM_DigitizationConfigNew.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import os
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from BCM_DigitizationConfigFlags import createBCMCfgFlags
from BCM_DigitizationConfigNew import BCM_DigitizationCfg
# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
# Provide input
dataDir = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art"
inputDir = os.environ.get("ATLAS_REFERENCE_DATA", dataDir)
fileDir = "/SimCoreTests/e_E50_eta34_49.EVNT.pool.root"
ConfigFlags.Input.Files = [inputDir + fileDir]
# Specify output
ConfigFlags.Output.HITFileName = "myHITS.pool.root"
ConfigFlags.lock()
# Construct ComponentAccumulator
cfg = MainServicesSerialCfg() #This syntax for storegate
cfg.merge(PoolReadCfg(ConfigFlags))
# Use BCM tools
BCMflags = createBCMCfgFlags()
acc = BCM_DigitizationCfg(BCMflags)
cfg.merge(acc)
# Add configuration to write HITS pool file
outConfig = OutputStreamCfg(ConfigFlags, "HITS")
cfg.merge(outConfig)
cfg.getService("StoreGateSvc").Dump=True
cfg.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
cfg.run(maxEvents=3)

