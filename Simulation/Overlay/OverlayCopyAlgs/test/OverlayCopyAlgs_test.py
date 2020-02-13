#!/usr/bin/env python
"""Run tests on OverlayCopyAlgsConfig.py

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
import sys

from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from OverlayConfiguration.OverlayTestHelpers import \
    CommonTestArgumentParser, postprocessAndLockFlags, printAndRun
from OverlayCopyAlgs.OverlayCopyAlgsConfig import \
    CopyCaloCalibrationHitContainersCfg, CopyJetTruthInfoCfg, CopyMcEventCollectionCfg, \
    CopyTimingsCfg, CopyTrackRecordCollectionsCfg

# Argument parsing
parser = CommonTestArgumentParser("OverlayCopyAlgs_test.py")
args = parser.parse_args()

# Configure
Configurable.configurableRun3Behavior = True

ConfigFlags.Input.Files = defaultTestFiles.RDO_BKG
ConfigFlags.Input.SecondaryFiles = defaultTestFiles.HITS
ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-16"
ConfigFlags.GeoModel.Align.Dynamic = False
ConfigFlags.Overlay.DataOverlay = False
ConfigFlags.Output.RDOFileName = "myRDO.pool.root"
# TODO: temporarily disabled due to I/O problems
# ConfigFlags.Output.RDO_SGNLFileName = "myRDO_SGNL.pool.root"

postprocessAndLockFlags(ConfigFlags, args)

# Construct our accumulator to run
acc = MainServicesThreadedCfg(ConfigFlags)
acc.merge(PoolReadCfg(ConfigFlags))

# Add truth overlay (needed downstream)
acc.merge(CopyMcEventCollectionCfg(ConfigFlags))
acc.merge(CopyJetTruthInfoCfg(ConfigFlags))
acc.merge(CopyTimingsCfg(ConfigFlags))
acc.merge(CopyCaloCalibrationHitContainersCfg(ConfigFlags))
acc.merge(CopyTrackRecordCollectionsCfg(ConfigFlags))

# Print and run
sys.exit(printAndRun(acc, ConfigFlags, args))
