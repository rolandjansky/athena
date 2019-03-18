#!/usr/bin/env python
"""Run tests on *LorentzAngleConfig.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleCfg

# test setup
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
ConfigFlags.Input.Files = defaultTestFiles.HITS
# case online
ConfigFlags.Common.isOnline = True
acc, tool = SCT_LorentzAngleCfg(ConfigFlags, name="SCT_LorentzAngleTestOnline")
acc2, tool = PixelLorentzAngleCfg(ConfigFlags, name="PixelLorentzAngleTestOnline")
acc.merge(acc2)
acc.wasMerged()
# case offline
ConfigFlags.Common.isOnline = False
acc, tool = SCT_LorentzAngleCfg(ConfigFlags, name="SCT_LorentzAngleTestOffline")
acc2, tool = PixelLorentzAngleCfg(ConfigFlags, name="PixelLorentzAngleTestOffline")
acc.merge(acc2)
acc.wasMerged()

