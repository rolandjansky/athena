#!/usr/bin/env python
"""Run tests on SCT_SiPropertiesConfig.py

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
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
ConfigFlags.Input.Files = defaultTestFiles.HITS_RUN2
ConfigFlags.lock()
# test
sct_acc = SCT_SiPropertiesCfg(ConfigFlags, name="SCT_SiPropertiesConfigTest")
sct_acc.popPrivateTools()
pix_acc = PixelSiPropertiesCfg(ConfigFlags, name="PixelSiPropertiesConfigTest")
pix_acc.popPrivateTools()

sct_acc.wasMerged()
pix_acc.wasMerged()


