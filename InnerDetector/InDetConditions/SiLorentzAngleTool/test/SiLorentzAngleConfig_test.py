#!/usr/bin/env python
"""Run tests on *LorentzAngleConfig.py

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
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
ConfigFlags.Input.Files = defaultTestFiles.HITS_RUN2
# using __init__ to reset, preventing errors on deletion
# case online
ConfigFlags.Common.isOnline = True
ConfigFlags.lock()
tacc = SCT_LorentzAngleCfg(ConfigFlags, name="SCT_LorentzAngleTestOnline")
tacc.__init__()
tacc = PixelLorentzAngleCfg(ConfigFlags, name="PixelLorentzAngleTestOnline")
tacc.__init__()
# case offline
flagsOffline = ConfigFlags.clone()
flagsOffline.Common.isOnline=False
flagsOffline.lock()
tacc = SCT_LorentzAngleCfg(flagsOffline, name="SCT_LorentzAngleTestOffline")
tacc.__init__()
tacc = PixelLorentzAngleCfg(flagsOffline, name="PixelLorentzAngleTestOffline")
tacc.__init__()

