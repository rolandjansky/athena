#!/usr/bin/env python
"""Run tests on EventInfoOverlayConfig.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from OverlayCommonAlgs.EventInfoOverlayConfig import EventInfoCnvAlgCfg, EventInfoOverlayCfg

# Set up logging and new style config
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True

# Configure
ConfigFlags.Input.Files = defaultTestFiles.RDO_BKG
ConfigFlags.Input.SecondaryFiles = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/TestCase_xAODEventInfo.root"]
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.Concurrency.NumThreads = 0
ConfigFlags.Overlay.DataOverlay = False
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

# Add configuration to write HITS pool file
outConfig = OutputStreamCfg(ConfigFlags, "RDO")
acc.merge(outConfig)

# Dump config
acc.getService("StoreGateSvc").Dump = True
acc.printConfig(withDetails=True)
ConfigFlags.dump()

# Execute and finish
sc = acc.run(maxEvents=3)

# Success should be 0
sys.exit(not sc.isSuccess())
