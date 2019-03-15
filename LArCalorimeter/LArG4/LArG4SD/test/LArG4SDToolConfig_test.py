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

  acc1, tool1 = LArEMBSensitiveDetectorCfg(ConfigFlags)
  acc1.addPublicTool(tool1)
  cfg.merge(acc1)
  

  acc2, tool2 = LArEMECSensitiveDetectorCfg(ConfigFlags)
  acc2.addPublicTool(tool2)
  cfg.merge(acc2)

  acc3, tool3 = LArFCALSensitiveDetectorCfg(ConfigFlags)
  acc3.addPublicTool(tool3)
  cfg.merge(acc3)
  
  acc4, tool4 = LArHECSensitiveDetectorCfg(ConfigFlags)
  acc4.addPublicTool(tool4)
  cfg.merge(acc4)

  toolMiniFCAL = LArMiniFCALSensitiveDetectorToolCfg(ConfigFlags)
  cfg.addPublicTool(toolMiniFCAL)

  toolDeadSensitiveDetector = LArDeadSensitiveDetectorToolCfg(ConfigFlags)
  cfg.addPublicTool(toolDeadSensitiveDetector)
  


  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","w")
  cfg.store(f) 
  f.close()



  print cfg._publicTools
  print "-----------------finished----------------------"