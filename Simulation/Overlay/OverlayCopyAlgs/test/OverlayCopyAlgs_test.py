#!/usr/bin/env python
"""Run tests on OverlayCopyAlgsConfig.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OverlayCopyAlgs.OverlayCopyAlgsConfig import \
    CopyCaloCalibrationHitContainersCfg, CopyJetTruthInfoCfg, CopyMcEventCollectionCfg, \
    CopyTimingsCfg, CopyTrackRecordCollectionsCfg

# Argument parsing
from argparse import ArgumentParser
parser = ArgumentParser(prog="OverlayCopyAlgs_test.py")
parser.add_argument("-n", "--nEvents",  default=3, type=int, help="The number of events to run. 0 skips execution")
parser.add_argument("-t", "--nThreads", default=1, type=int, help="The number of concurrent threads to run. 0 uses serial Athena.")
parser.add_argument("-V", "--verboseAccumulators", default=False, action="store_true", help="Print full details of the AlgSequence for each accumulator")
args = parser.parse_args()

# Configure
Configurable.configurableRun3Behavior = True

ConfigFlags.Input.Files = defaultTestFiles.RDO_BKG
ConfigFlags.Input.SecondaryFiles = defaultTestFiles.HITS
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.Overlay.DataOverlay = False
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
# TODO: temporarily disabled due to I/O problems
# ConfigFlags.Output.RDO_SGNLFileName = "myRDO_SGNL.pool.root"

# Flags relating to multithreaded execution
ConfigFlags.Concurrency.NumThreads = args.nThreads
if args.nThreads > 0:
    ConfigFlags.Scheduler.ShowDataDeps = True
    ConfigFlags.Scheduler.ShowDataFlow = True
    ConfigFlags.Scheduler.ShowControlFlow = True
    ConfigFlags.Concurrency.NumConcurrentEvents = args.nThreads

ConfigFlags.lock()

# Construct our accumulator to run
acc = MainServicesThreadedCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))

# Add truth overlay (needed downstream)
acc.merge(CopyMcEventCollectionCfg(ConfigFlags))
acc.merge(CopyJetTruthInfoCfg(ConfigFlags))
acc.merge(CopyTimingsCfg(ConfigFlags))
acc.merge(CopyCaloCalibrationHitContainersCfg(ConfigFlags))
acc.merge(CopyTrackRecordCollectionsCfg(ConfigFlags))

# Dump config
if args.verboseAccumulators:
    acc.printConfig(withDetails=True)
ConfigFlags.dump()

# Execute and finish
sc = acc.run(maxEvents=args.nEvents)

# Dump config summary
acc.printConfig(withDetails=False)

# Success should be 0
sys.exit(not sc.isSuccess())
