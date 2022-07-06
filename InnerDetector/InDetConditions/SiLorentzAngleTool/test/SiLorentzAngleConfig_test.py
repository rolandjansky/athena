#!/usr/bin/env python
"""Run tests on *LorentzAngleConfig.py

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleToolCfg
from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleToolCfg

# test setup
log.setLevel(DEBUG)
ConfigFlags.Input.Files = defaultTestFiles.HITS_RUN2
# using __init__ to reset, preventing errors on deletion
# case online
ConfigFlags.Common.isOnline = True
ConfigFlags.lock()
tacc = SCT_LorentzAngleToolCfg(ConfigFlags, name="SCT_LorentzAngleTestOnline")
tacc.__init__()
tacc = PixelLorentzAngleToolCfg(ConfigFlags, name="PixelLorentzAngleTestOnline")
tacc.__init__()
# case offline
flagsOffline = ConfigFlags.clone()
flagsOffline.Common.isOnline=False
flagsOffline.lock()
tacc = SCT_LorentzAngleToolCfg(flagsOffline, name="SCT_LorentzAngleTestOffline")
tacc.__init__()
tacc = PixelLorentzAngleToolCfg(flagsOffline, name="PixelLorentzAngleTestOffline")
tacc.__init__()

