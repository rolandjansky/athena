#!/usr/bin/env python
"""Run tests on G4AtlasToolsConfigNew

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""

if __name__ == '__main__':
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg

  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  from AthenaConfiguration.Enums import ProductionStep
  ConfigFlags.Common.ProductionStep = ProductionStep.Simulation
  ConfigFlags.Sim.ISFRun = True

  #Provide input
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT
  
  ConfigFlags.GeoModel.Run = "RUN2" 

  #ConfigFlags.Beam.Type = "cosmics"
  ConfigFlags.Detector.SimulateBCM = True
  ConfigFlags.Detector.SimulatePixel = True
  ConfigFlags.Detector.SimulateSCT = True
  ConfigFlags.Detector.SimulateTRT = True
  ConfigFlags.Detector.SimulateLAr = True
  ConfigFlags.Detector.SimulateHGTD = False
  ConfigFlags.Detector.SimulateTile = False

  #ConfigFlags.GeoModel.AtlasVersion = "tb_Tile2000_2003"
  #ConfigFlags.GeoModel.AtlasVersion = "ctbh8"
  ConfigFlags.GeoModel.AtlasVersion = 'ATLAS-R2-2015-03-01-00'
  # Finalize 
  ConfigFlags.lock()


  ## Initialize a new component accumulator
  cfg = MainServicesCfg(ConfigFlags) 


  from G4AtlasTools.G4AtlasToolsConfigNew import SensitiveDetectorMasterToolCfg
  acc  = SensitiveDetectorMasterToolCfg(ConfigFlags)
  tool = cfg.popToolsAndMerge(acc)
  cfg.setPrivateTools(tool)

  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  #cfg not being used so complains ...fine now!
  f=open("test.pkl","wb")
  cfg.store(f) #sets wasmerged = true
  f.close()


  print ("-----------------finished----------------------")
