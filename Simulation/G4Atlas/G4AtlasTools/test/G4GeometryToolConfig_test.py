#!/usr/bin/env python
"""Run tests on G4Geometry Tool configuration

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from __future__ import print_function
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
  ConfigFlags.Detector.SimulateMuon = True
  ConfigFlags.Detector.SimulateID = True
  ConfigFlags.Detector.SimulateCalo = True
  ConfigFlags.Detector.SimulateBpipe = True
  ConfigFlags.Detector.SimulateFwdRegion = True
  ConfigFlags.Detector.GeometryLAr = True
  ConfigFlags.Detector.GeometryTile = True
  ConfigFlags.Detector.GeometryLucid = True
  ConfigFlags.Detector.GeometryZDC = True
  ConfigFlags.Detector.GeometryALFA = True
  ConfigFlags.Detector.GeometryAFP = True
  ConfigFlags.Sim.WorldRRange = 15000
  ConfigFlags.Sim.WorldZRange = 27000

  ConfigFlags.Sim.TwissFileNomReal = "nominal" #so it doesn't crash

  # Finalize
  ConfigFlags.lock()


  from G4AtlasTools.G4GeometryToolConfig import BeamPipeGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import PixelGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import SCTGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import TRTGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import IDETEnvelopeCfg
  from G4AtlasTools.G4GeometryToolConfig import ATLASEnvelopeCfg
  from G4AtlasTools.G4GeometryToolConfig import CALOEnvelopeCfg
  from G4AtlasTools.G4GeometryToolConfig import LucidGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import ALFAGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import ZDCGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import AFPGeoDetectorToolCfg
  from G4AtlasTools.G4GeometryToolConfig import ForwardRegionEnvelopeCfg
  from G4AtlasTools.G4GeometryToolConfig import MaterialDescriptionToolCfg


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


  acc5,tool5 = ForwardRegionEnvelopeCfg(ConfigFlags)
  cfg.merge(acc5)
  cfg.addPublicTool(tool5)

  acc6,tool6 = CALOEnvelopeCfg(ConfigFlags)
  cfg.merge(acc6)
  cfg.addPublicTool(tool6)

  acc7, tool7 = LucidGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc7)
  cfg.addPublicTool(tool7)

  acc8, tool8 = ALFAGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc8)
  cfg.addPublicTool(tool8)

  acc9, tool9 = ZDCGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc9)
  cfg.addPublicTool(tool9)

  acc10, tool10 = AFPGeoDetectorToolCfg(ConfigFlags)
  cfg.merge(acc10)
  cfg.addPublicTool(tool10)


  accATLAS,toolATLAS = ATLASEnvelopeCfg(ConfigFlags)
  cfg.merge(accATLAS)
  cfg.addPublicTool(toolATLAS)

  cfg.addPublicTool(MaterialDescriptionToolCfg(ConfigFlags))

  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","wb")
  cfg.store(f)
  f.close()



  print(cfg._publicTools)
  print("-----------------finished----------------------")
