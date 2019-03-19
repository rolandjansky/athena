#!/usr/bin/env python
"""Run tests on PixelCablingConfigNew.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from PixelCabling.PixelCablingConfigNew import PixelCablingSvcCfg

# test setup
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
ConfigFlags.Input.Files = defaultTestFiles.HITS
# test
PixelCablingAcc = PixelCablingSvcCfg(ConfigFlags, name="PixelCablingSvcTest")
# prevent raise on __del__
PixelCablingAcc.wasMerged()

