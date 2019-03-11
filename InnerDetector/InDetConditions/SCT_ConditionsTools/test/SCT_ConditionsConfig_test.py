#!/usr/bin/env python
"""Run tests on SCT_ConditionsTools/python/*Config.py scripts

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from SCT_ConditionsTools.SCT_DCSConditionsConfig import SCT_DCSConditionsCfg
from SCT_ConditionsTools.SCT_SiliconConditionsConfig import SCT_SiliconConditionsCfg
from SCT_ConditionsTools.SCT_ReadCalibChipDataConfig import SCT_ReadCalibChipDataCfg

# test setup
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
ConfigFlags.Input.Files = defaultTestFiles.HITS
# DCS
DCSAcc, DCSTool = SCT_DCSConditionsCfg(ConfigFlags, name="DCSTest")
# Silicon
SiliconAcc, SiliconTool = SCT_SiliconConditionsCfg(ConfigFlags, name="SiliconTest")
# ReadCalibChipData
ReadAcc, ReadTool = SCT_ReadCalibChipDataCfg(ConfigFlags, name="ReadTest")
# prevent raise on __del__
DCSAcc.wasMerged()
SiliconAcc.wasMerged()
ReadAcc.wasMerged()

