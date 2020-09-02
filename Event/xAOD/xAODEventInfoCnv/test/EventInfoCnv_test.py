#!/usr/bin/env python
"""Run tests for old EventInfo to xAOD::EventInfo conversion

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

# Argument parsing
from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("-n", "--maxEvents",  default=3, type=int, help="The number of events to run. 0 skips execution")
parser.add_argument("-t", "--threads", default=1, type=int, help="The number of concurrent threads to run. 0 uses serial Athena.")
parser.add_argument("-V", "--verboseAccumulators", default=False, action="store_true", help="Print full details of the AlgSequence for each accumulator")
args = parser.parse_args()

# Configure
ConfigFlags.Input.Files = defaultTestFiles.HITS
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.Output.HITSFileName = "myHITS.pool.root"

# Flags relating to multithreaded execution
ConfigFlags.Concurrency.NumThreads = args.threads
if args.threads > 0:
    ConfigFlags.Scheduler.ShowDataDeps = True
    ConfigFlags.Scheduler.ShowDataFlow = True
    ConfigFlags.Scheduler.ShowControlFlow = True
    ConfigFlags.Concurrency.NumConcurrentEvents = args.threads

ConfigFlags.lock()

# Function tests
accAlg = EventInfoCnvAlgCfg(ConfigFlags)
# reset to prevent errors on deletion
accAlg.__init__()

# Construct our accumulator to run
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))

# Add event info overlay
acc.merge(EventInfoCnvAlgCfg(ConfigFlags))

# Add output
acc.merge(OutputStreamCfg(ConfigFlags, "HITS"))

# Dump config
if args.verboseAccumulators:
    acc.printConfig(withDetails=True)
acc.getService("StoreGateSvc").Dump = True
ConfigFlags.dump()

# Execute and finish
sc = acc.run(maxEvents=args.maxEvents)

# Dump config summary
acc.printConfig(withDetails=False)

# Success should be 0
sys.exit(not sc.isSuccess())
