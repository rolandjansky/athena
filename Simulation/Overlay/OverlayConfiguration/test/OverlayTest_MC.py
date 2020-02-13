#!/usr/bin/env python
"""Run tests for MC+MC overlay

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
parser = CommonTestArgumentParser("OverlayTest_MC.py")
args = parser.parse_args()

# Configure
ConfigFlags.Input.Files = defaultTestFiles.RDO_BKG
ConfigFlags.Input.SecondaryFiles = defaultTestFiles.HITS
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.Overlay.DataOverlay = False
ConfigFlags.Output.RDOFileName = "mcOverlayRDO.pool.root"
ConfigFlags.Output.RDO_SGNLFileName = "mcOverlayRDO_SGNL.pool.root"

postprocessAndLockFlags(ConfigFlags, args)

# Construct our accumulator to run
acc = OverlayMainCfg(ConfigFlags)

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
