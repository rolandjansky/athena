#!/usr/bin/env python
"""Run tests on SCT_ConditionsTools/python/*Config.py scripts

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_DCSConditionsCfg, SCT_ReadCalibChipDataCfg, SCT_SiliconConditionsCfg

# test setup
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
ConfigFlags.Input.Files = []
ConfigFlags.lock()
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
