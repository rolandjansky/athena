#!/usr/bin/env python
"""Run tests on TRTOverlayConfig.py

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from InDetOverlay.TRTOverlayConfig import TRTOverlayCfg
from OverlayConfiguration.OverlayTestHelpers import \
    CommonTestArgumentParser, defaultTestFlags, postprocessAndLockFlags, printAndRun
from OverlayCopyAlgs.OverlayCopyAlgsConfig import CopyMcEventCollectionCfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoOverlayCfg

# Argument parsing
parser = CommonTestArgumentParser("TRTOverlayConfig_test.py")
args = parser.parse_args()

# Configure
defaultTestFlags(ConfigFlags, args)
ConfigFlags.Output.RDOFileName = 'mcOverlayRDO_TRT.pool.root'
postprocessAndLockFlags(ConfigFlags, args)

# Construct our accumulator to run
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))

# Add event and truth overlay (needed downstream)
acc.merge(EventInfoOverlayCfg(ConfigFlags))
acc.merge(CopyMcEventCollectionCfg(ConfigFlags))

# Add TRT overlay
acc.merge(TRTOverlayCfg(ConfigFlags))

# Dump the pickle
with open("TRTOverlayCfg.pkl", "wb") as f:
    acc.store(f)

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
