#!/usr/bin/env python
"""Run tests on PixelOverlayConfig.py

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from InDetOverlay.PixelOverlayConfig import PixelOverlayCfg
from OverlayConfiguration.OverlayTestHelpers import \
    CommonTestArgumentParser, defaultTestFlags, postprocessAndLockFlags, printAndRun
from OverlayCopyAlgs.OverlayCopyAlgsConfig import CopyMcEventCollectionCfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoOverlayCfg

# Argument parsing
parser = CommonTestArgumentParser("PixelOverlayConfig_test.py")
args = parser.parse_args()

# Configure
defaultTestFlags(ConfigFlags, args)
ConfigFlags.Output.RDOFileName = 'mcOverlayRDO_Pixel.pool.root'
postprocessAndLockFlags(ConfigFlags, args)

# Construct our accumulator to run
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))

# Add event and truth overlay (needed downstream)
acc.merge(EventInfoOverlayCfg(ConfigFlags))
acc.merge(CopyMcEventCollectionCfg(ConfigFlags))

# Add Pixel overlay
acc.merge(PixelOverlayCfg(ConfigFlags))

# Dump the pickle
with open("PixelOverlayCfg.pkl", "wb") as f:
    acc.store(f)

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
