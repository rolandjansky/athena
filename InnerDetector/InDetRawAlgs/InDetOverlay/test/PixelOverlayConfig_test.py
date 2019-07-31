#!/usr/bin/env python
"""Run tests on PixelOverlayConfig.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OverlayCommonAlgs.OverlayCopyAlgsConfig import CopyMcEventCollectionCfg
from InDetOverlay.PixelOverlayConfig import PixelOverlayCfg

# Global test config
nThreads = 1

# Configure
Configurable.configurableRun3Behavior = True

ConfigFlags.Input.Files = defaultTestFiles.RDO_BKG
ConfigFlags.Input.SecondaryFiles = defaultTestFiles.HITS
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.Overlay.DataOverlay = False

# Flags relating to multithreaded execution
ConfigFlags.Concurrency.NumThreads = nThreads
if nThreads > 0:
    ConfigFlags.Scheduler.ShowDataDeps = True
    ConfigFlags.Scheduler.ShowDataFlow = True
    ConfigFlags.Scheduler.ShowControlFlow = True
    ConfigFlags.Concurrency.NumConcurrentEvents = nThreads

ConfigFlags.lock()

# Construct our accumulator to run
acc = MainServicesThreadedCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))

# Add truth overlay (needed downstream)
acc.merge(CopyMcEventCollectionCfg(ConfigFlags))

# Add Pixel overlay
acc.merge(PixelOverlayCfg(ConfigFlags))

# Dump config
acc.printConfig(withDetails=True)
ConfigFlags.dump()

# Execute and finish
sc = acc.run(maxEvents=3)

# Dump config summary
acc.printConfig(withDetails=False)

# Success should be 0
sys.exit(not sc.isSuccess())
