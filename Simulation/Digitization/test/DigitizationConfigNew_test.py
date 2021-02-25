#!/usr/bin/env python
"""Test various ComponentAccumulator Digitization configuration modules

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
import sys
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from Digitization.DigitizationSteering import DigitizationMainCfg, DigitizationMessageSvcCfg

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

# TODO: temporary
ConfigFlags.Digitization.TruthOutput = True

ConfigFlags.lock()

# Construct our accumulator to run
acc = DigitizationMainCfg(ConfigFlags)
acc.merge(DigitizationMessageSvcCfg(ConfigFlags))

# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.getService("ConditionStore").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()
# Execute and finish
sc = acc.run(maxEvents=3)
# Success should be 0
sys.exit(not sc.isSuccess())
