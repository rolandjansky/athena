#!/usr/bin/env python
"""Run tests on PixelG4_SD configuration

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""


if __name__ == '__main__':
  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1

  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  ConfigFlags.Sim.ISFRun = True

  #Provide input
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  # Finalize
  ConfigFlags.lock()

  ## Initialize the main component accumulator
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  from PixelG4_SD.PixelG4_SDToolConfig import PixelSensorSDCfg
  from PixelG4_SD.PixelG4_SDToolConfig import PixelSensor_CTBCfg
  from PixelG4_SD.PixelG4_SDToolConfig import DBMSensorSDCfg

  tools = []
  cfg = ComponentAccumulator()
  tools += [ cfg.popToolsAndMerge(PixelSensorSDCfg(ConfigFlags)) ]
  tools += [ cfg.popToolsAndMerge(PixelSensor_CTBCfg(ConfigFlags)) ]
  tools += [ cfg.popToolsAndMerge(DBMSensorSDCfg(ConfigFlags)) ]

  cfg.setPrivateTools(tools)
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","wb")
  cfg.store(f)
  f.close()

  print(cfg._privateTools)
  print("-----------------finished----------------------")
