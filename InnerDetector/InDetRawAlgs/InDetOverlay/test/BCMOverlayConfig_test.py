#!/usr/bin/env python
"""Run tests on BCMOverlayConfig.py

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from InDetOverlay.BCMOverlayConfig import BCMOverlayCfg
from OverlayConfiguration.OverlayTestHelpers import \
    CommonTestArgumentParser, defaultTestFlags, postprocessAndLockFlags, printAndRun
from OverlayCopyAlgs.OverlayCopyAlgsConfig import CopyMcEventCollectionCfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoOverlayCfg

# Configure
Configurable.configurableRun3Behavior = True

# Argument parsing
parser = CommonTestArgumentParser("BCMOverlayConfig_test.py")
args = parser.parse_args()

# Configure
defaultTestFlags(ConfigFlags, args)
postprocessAndLockFlags(ConfigFlags, args)

# Construct our accumulator to run
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))

# Add event and truth overlay (needed downstream)
acc.merge(EventInfoOverlayCfg(ConfigFlags))
acc.merge(CopyMcEventCollectionCfg(ConfigFlags))

# Add BCM overlay
acc.merge(BCMOverlayCfg(ConfigFlags))

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
