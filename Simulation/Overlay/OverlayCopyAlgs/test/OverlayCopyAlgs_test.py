#!/usr/bin/env python
"""Run tests on OverlayCopyAlgsConfig.py

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OverlayConfiguration.OverlayTestHelpers import \
    CommonTestArgumentParser, defaultTestFlags, postprocessAndLockFlags, printAndRun
from OverlayCopyAlgs.OverlayCopyAlgsConfig import \
    CopyCaloCalibrationHitContainersCfg, CopyJetTruthInfoCfg, CopyMcEventCollectionCfg, \
    CopyTimingsCfg, CopyTrackRecordCollectionsCfg

# Configure
Configurable.configurableRun3Behavior = True

# Argument parsing
parser = CommonTestArgumentParser("OverlayCopyAlgs_test.py")
args = parser.parse_args()

# Configure
defaultTestFlags(ConfigFlags, args)
postprocessAndLockFlags(ConfigFlags, args)

# Construct our accumulator to run
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))

# Add truth overlay (needed downstream)
acc.merge(CopyMcEventCollectionCfg(ConfigFlags))
acc.merge(CopyJetTruthInfoCfg(ConfigFlags))
acc.merge(CopyTimingsCfg(ConfigFlags))
acc.merge(CopyCaloCalibrationHitContainersCfg(ConfigFlags))
acc.merge(CopyTrackRecordCollectionsCfg(ConfigFlags))

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
