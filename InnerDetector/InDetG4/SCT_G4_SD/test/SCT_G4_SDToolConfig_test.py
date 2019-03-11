#!/usr/bin/env python
"""Run tests on SCT_G4_SD configuration

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

  from SCT_G4_SD.SCT_G4_SDToolConfig import SLHC_SctSensorSD_GmxCfg
  from SCT_G4_SD.SCT_G4_SDToolConfig import SLHC_SctSensorSDCfg
  from SCT_G4_SD.SCT_G4_SDToolConfig import SctSensorSDCfg
  from SCT_G4_SD.SCT_G4_SDToolConfig import SctSensor_CTBCfg

  acc1, tool1 = SLHC_SctSensorSD_GmxCfg(ConfigFlags)
  acc1.addPublicTool(tool1)
  cfg.merge(acc1)

  acc2, tool2 = SLHC_SctSensorSDCfg(ConfigFlags)
  acc2.addPublicTool(tool2)
  cfg.merge(acc2)

  acc, tool = SctSensorSDCfg(ConfigFlags) 
  acc.addPublicTool(tool) 
  cfg.merge(acc)

  tool3  = SctSensor_CTBCfg()
  cfg.addPublicTool(tool3)
  


  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","w")
  cfg.store(f) 
  f.close()



  print cfg._publicTools
  print "-----------------finished----------------------"