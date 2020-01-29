#!/usr/bin/env python
"""Run tests on PixelOverlayConfig.py

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from InDetOverlay.PixelOverlayConfig import PixelOverlayCfg
from OverlayConfiguration.OverlayTestHelpers import \
    CommonTestArgumentParser, postprocessAndLockFlags, printAndRun
from OverlayCopyAlgs.OverlayCopyAlgsConfig import CopyMcEventCollectionCfg

# Configure
Configurable.configurableRun3Behavior = True

# Argument parsing
parser = CommonTestArgumentParser("PixelOverlayConfig_test.py")
args = parser.parse_args()

# Configure
ConfigFlags.Input.Files = defaultTestFiles.RDO_BKG
ConfigFlags.Input.SecondaryFiles = defaultTestFiles.HITS
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.Overlay.DataOverlay = False
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.Output.RDO_SGNLFileName = "myRDO_SGNL.pool.root"

postprocessAndLockFlags(ConfigFlags, args)

# Construct our accumulator to run
acc = MainServicesThreadedCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))

# Add truth overlay (needed downstream)
acc.merge(CopyMcEventCollectionCfg(ConfigFlags))

# Add Pixel overlay
acc.merge(PixelOverlayCfg(ConfigFlags))

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
