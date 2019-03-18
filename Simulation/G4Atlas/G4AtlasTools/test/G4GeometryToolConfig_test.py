#!/usr/bin/env python
"""Run tests on G4Geometry Tool configuration

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


if __name__ == '__main__':
  from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
  import os

  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  ConfigFlags.Detector.GeometryPixel = True
  ConfigFlags.Detector.GeometrySCT = True
  ConfigFlags.Detector.GeometryTRT = True

  # Finalize 
  ConfigFlags.lock()


  from G4AtlasTools.G4GeometryToolConfig import BeamPipeGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import PixelGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import SCTGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import TRTGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import IDETEnvelopeCfg

  ## Initialize a new component accumulator
  cfg = ComponentAccumulator()

  acc, tool  = BeamPipeGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc)
  cfg.addPublicTool(tool)

  acc1, tool1  = PixelGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc1)
  cfg.addPublicTool(tool1)

  acc2, tool2  = SCTGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc2)
  cfg.addPublicTool(tool2)

  acc3, tool3  = TRTGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc3)
  cfg.addPublicTool(tool3)

  acc4, tool4  = IDETEnvelopeCfg(ConfigFlags)
  cfg.merge(acc4)
  cfg.addPublicTool(tool4)
  
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","w")
  cfg.store(f) 
  f.close()



  print cfg._publicTools
  print "-----------------finished----------------------"
