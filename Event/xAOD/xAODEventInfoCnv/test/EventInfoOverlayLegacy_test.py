#!/usr/bin/env python
"""Run tests for EventInfo overlay with legacy inputs

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoOverlayCfg

# Set up logging and new style config
Configurable.configurableRun3Behavior = True

# Argument parsing
from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("-n", "--maxEvents",  default=3, type=int, help="The number of events to run. 0 skips execution")
parser.add_argument("-t", "--threads", default=1, type=int, help="The number of concurrent threads to run. 0 uses serial Athena.")
parser.add_argument("-V", "--verboseAccumulators", default=False, action="store_true", help="Print full details of the AlgSequence for each accumulator")
args = parser.parse_args()

# Configure
ConfigFlags.Input.Files = defaultTestFiles.RDO_BKG
ConfigFlags.Input.SecondaryFiles = defaultTestFiles.HITS
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.Overlay.DataOverlay = False
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.Output.RDO_SGNLFileName = "myRDO_SGNL.pool.root"

# Flags relating to multithreaded execution
ConfigFlags.Concurrency.NumThreads = args.threads
if args.threads > 0:
    ConfigFlags.Scheduler.ShowDataDeps = True
    ConfigFlags.Scheduler.ShowDataFlow = True
    ConfigFlags.Scheduler.ShowControlFlow = True
    ConfigFlags.Concurrency.NumConcurrentEvents = args.threads

ConfigFlags.lock()

# Function tests
accAlg = EventInfoOverlayCfg(ConfigFlags)
# reset to prevent errors on deletion
accAlg.__init__()

# Construct our accumulator to run
acc = MainServicesSerialCfg()
acc.merge(PoolReadCfg(ConfigFlags))

# Add event info overlay
acc.merge(EventInfoOverlayCfg(ConfigFlags))

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
