#!/usr/bin/env python
"""Run tests on LArG4SD configuration

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



  from LArG4SD.LArG4SDToolConfig import LArEMBSensitiveDetectorCfg
  from LArG4SD.LArG4SDToolConfig import LArEMECSensitiveDetectorCfg
  from LArG4SD.LArG4SDToolConfig import LArFCALSensitiveDetectorCfg
  from LArG4SD.LArG4SDToolConfig import LArHECSensitiveDetectorCfg
  from LArG4SD.LArG4SDToolConfig import LArMiniFCALSensitiveDetectorToolCfg
  from LArG4SD.LArG4SDToolConfig import LArDeadSensitiveDetectorToolCfg
  from LArG4SD.LArG4SDToolConfig import LArActiveSensitiveDetectorToolCfg
  from LArG4SD.LArG4SDToolConfig import LArInactiveSensitiveDetectorToolCfg
  
  acc1 = LArEMBSensitiveDetectorCfg(ConfigFlags)
  tool1 = cfg.popToolsAndMerge(acc1)
  cfg.addPublicTool(tool1)
  
  acc2 = LArEMECSensitiveDetectorCfg(ConfigFlags)
  tool2 = cfg.popToolsAndMerge(acc2)
  cfg.addPublicTool(tool2)


  acc3 = LArFCALSensitiveDetectorCfg(ConfigFlags)
  tool3 = cfg.popToolsAndMerge(acc3)
  cfg.addPublicTool(tool3)
  
  acc4 = LArHECSensitiveDetectorCfg(ConfigFlags)
  tool4 = cfg.popToolsAndMerge(acc4)
  cfg.addPublicTool(tool4)

  acc5 = LArMiniFCALSensitiveDetectorToolCfg(ConfigFlags)
  tool5 = cfg.popToolsAndMerge(acc5)
  cfg.addPublicTool(tool5)

  toolDeadSensitiveDetector = LArDeadSensitiveDetectorToolCfg(ConfigFlags)
  cfg.addPublicTool(toolDeadSensitiveDetector)

  toolActiveSensitiveDetector = LArActiveSensitiveDetectorToolCfg(ConfigFlags)
  cfg.addPublicTool(toolActiveSensitiveDetector)

  toolInactiveSensitiveDetector = LArInactiveSensitiveDetectorToolCfg(ConfigFlags)
  cfg.addPublicTool(toolInactiveSensitiveDetector)


  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","w")
  cfg.store(f) 
  f.close()



  print cfg._publicTools
  print "-----------------finished----------------------"