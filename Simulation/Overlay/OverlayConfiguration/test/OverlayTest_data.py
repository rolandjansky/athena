#!/usr/bin/env python
"""Run tests for MC+data overlay

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles

from OverlayConfiguration.OverlaySteering import OverlayMainCfg
from OverlayConfiguration.OverlayTestHelpers import \
    CommonTestArgumentParser, postprocessAndLockFlags, printAndRun

# Set up logging and new style config
Configurable.configurableRun3Behavior = True

# Argument parsing
parser = CommonTestArgumentParser("OverlayTest_data.py")
args = parser.parse_args()

# Configure
ConfigFlags.Input.Files = defaultTestFiles.HITS_DATA_OVERLAY
ConfigFlags.Input.SecondaryFiles = defaultTestFiles.RAW_BKG
ConfigFlags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2016-12"
ConfigFlags.Overlay.DataOverlay = True
ConfigFlags.Output.RDOFileName = "dataOverlayRDO.pool.root"
# Temporarily disabled due to issues with MT
# ConfigFlags.Output.RDO_SGNLFileName = "dataOverlayRDO_SGNL.pool.root"

postprocessAndLockFlags(ConfigFlags, args)

# Construct our accumulator to run
acc = OverlayMainCfg(ConfigFlags)

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
