#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Logging import log
from AthenaCommon.Constants import DEBUG
from AthenaCommon.Configurable import Configurable
from PixelReadoutGeometry.PixelReadoutGeometryConfig import PixelReadoutManagerCfg, ITkPixelReadoutManagerCfg

# test setup
log.setLevel(DEBUG)
Configurable.configurableRun3Behavior = True
ConfigFlags.Input.Files = []
# test
PixelReadoutManagerAcc = PixelReadoutManagerCfg(ConfigFlags, name="PixelReadoutManagerTest")
# prevent raise on __del__
PixelReadoutManagerAcc.wasMerged()

# test ITk
ITkPixelReadoutManagerAcc = ITkPixelReadoutManagerCfg(ConfigFlags, name="ITkPixelReadoutManagerTest")
# prevent raise on __del__
ITkPixelReadoutManagerAcc.wasMerged()
