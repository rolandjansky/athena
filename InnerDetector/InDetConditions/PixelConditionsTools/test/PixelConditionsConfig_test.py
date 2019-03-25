#!/usr/bin/env python
"""Run tests on PixelConditionsTools/python/*Config.py scripts

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from PixelConditionsTools.PixelDCSConditionsConfig import PixelDCSConditionsCfg
from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg

# test setup
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
ConfigFlags.Input.Files = defaultTestFiles.HITS
# test
DCSAcc = PixelDCSConditionsCfg(ConfigFlags, name="DCSTest")
SummaryAcc, SummaryTool = PixelConditionsSummaryCfg(ConfigFlags, name="SummaryTest")
# prevent raise on __del__
DCSAcc.wasMerged()
SummaryAcc.wasMerged()

