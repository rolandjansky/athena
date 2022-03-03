#!/usr/bin/env python
"""Run tests for EventInfo overlay with legacy inputs

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OverlayConfiguration.OverlayTestHelpers import \
    CommonTestArgumentParser, postprocessAndLockFlags, printAndRun
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoOverlayCfg

# Set up logging and new style config
Configurable.configurableRun3Behavior = True

# Argument parsing
parser = CommonTestArgumentParser("EventInfoOverlayLegacy_test.py")
args = parser.parse_args()

# Configure
ConfigFlags.Input.Files = defaultTestFiles.RDO_BKG_RUN2
# use old HITS on purpose
ConfigFlags.Input.SecondaryFiles = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"]
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.Overlay.DataOverlay = False
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
ConfigFlags.Output.RDO_SGNLFileName = "myRDO_SGNL.pool.root"

postprocessAndLockFlags(ConfigFlags, args)

# Function tests
accAlg = EventInfoOverlayCfg(ConfigFlags)
# reset to prevent errors on deletion
accAlg.__init__()

# Construct our accumulator to run
acc = MainServicesCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))

# Add event info overlay
acc.merge(EventInfoOverlayCfg(ConfigFlags))

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
