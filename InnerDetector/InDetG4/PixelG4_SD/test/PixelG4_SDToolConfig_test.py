#!/usr/bin/env python
"""Run tests on PixelG4_SD configuration

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


if __name__ == '__main__':
  

  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  ConfigFlags.Sim.ISF.Run = True
  

  #Provide input
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT
  
  # Finalize 
  ConfigFlags.lock()


  ## Initialize a new component accumulator
  cfg = ComponentAccumulator()

  from PixelG4_SD.PixelG4_SDToolConfig import PixelSensorSDCfg
  from PixelG4_SD.PixelG4_SDToolConfig import PixelSensor_CTBCfg
  from PixelG4_SD.PixelG4_SDToolConfig import DBMSensorSDCfg



  acc, tool = PixelSensorSDCfg(ConfigFlags)
  acc.addPublicTool(tool)
  cfg.merge(acc)

  tool2 = PixelSensor_CTBCfg()
  cfg.addPublicTool(tool2)

  tool3 = DBMSensorSDCfg() 
  cfg.addPublicTool(tool3) 
  


  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","w")
  cfg.store(f) 
  f.close()



  print cfg._publicTools
  print "-----------------finished----------------------"