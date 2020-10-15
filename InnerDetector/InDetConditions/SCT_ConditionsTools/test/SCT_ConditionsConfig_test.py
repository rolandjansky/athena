#!/usr/bin/env python
"""Run tests on SCT_ConditionsTools/python/*Config.py scripts

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
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
# call tests
dcs_acc = SCT_DCSConditionsCfg(ConfigFlags, name="DCSTest")
dcs_acc.popPrivateTools()
acc1=SCT_SiliconConditionsCfg(ConfigFlags, name="SiliconTest")
log.info(acc1.popPrivateTools())

acc2=SCT_SiliconConditionsCfg(ConfigFlags, name="SiliconTest")
log.info(acc2.popPrivateTools())
acc3=SCT_ReadCalibChipDataCfg(ConfigFlags, name="ReadTest")
log.info(acc3.popPrivateTools())

dcs_acc.wasMerged()
acc1.wasMerged()
acc2.wasMerged()
acc3.wasMerged()

