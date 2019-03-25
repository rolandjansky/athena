#!/usr/bin/env python
"""Run tests on SCT_SiPropertiesConfig.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from SiPropertiesTool.SCT_SiPropertiesConfig import SCT_SiPropertiesCfg
from SiPropertiesTool.PixelSiPropertiesConfig import PixelSiPropertiesCfg

# test setup
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
ConfigFlags.Input.Files = defaultTestFiles.HITS
# test
tacc = SCT_SiPropertiesCfg(ConfigFlags, name="SCT_SiPropertiesConfigTest")
tacc.merge(PixelSiPropertiesCfg(ConfigFlags, name="PixelSiPropertiesConfigTest"))
# reset to prevent errors on deletion
tacc.__init__()

