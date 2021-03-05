#!/usr/bin/env python
"""Run tests on G4Geometry Tool configuration

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""

from __future__ import print_function
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
  from AthenaConfiguration.Enums import ProductionStep
  ConfigFlags.Common.ProductionStep = ProductionStep.Simulation

  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  import os
  if "AthSimulation_DIR" in os.environ:
    detectors =['Bpipe', 'BCM', 'DBM',  'Pixel', 'SCT', 'TRT', 'LAr', 'Tile', 'CSC', 'MDT', 'RPC', 'TGC'] # Forward Detector geometry not currently included in AthSimulation
  else:
    detectors =['Bpipe', 'BCM', 'DBM',  'Pixel', 'SCT', 'TRT', 'LAr', 'Tile', 'CSC', 'MDT', 'RPC', 'TGC', 'FwdRegion', 'Lucid', 'ZDC', 'ALFA', 'AFP']

  # Setup detector flags
  from SimuJobTransforms.SimulationTestHelpers import setupDetectorSimulateFlagsFromList
  setupDetectorSimulateFlagsFromList(ConfigFlags, detectors)

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

  tool = cfg.popToolsAndMerge(BeamPipeGeoDetectorToolCfg(ConfigFlags))
  cfg.addPublicTool(tool)

  tool = cfg.popToolsAndMerge(PixelGeoDetectorToolCfg(ConfigFlags))
  cfg.addPublicTool(tool)
  
  tool = cfg.popToolsAndMerge(SCTGeoDetectorToolCfg(ConfigFlags))
  cfg.addPublicTool(tool)
  
  tool = cfg.popToolsAndMerge(TRTGeoDetectorToolCfg(ConfigFlags))
  cfg.addPublicTool(tool)
  
  tool = cfg.popToolsAndMerge(IDETEnvelopeCfg(ConfigFlags))
  cfg.addPublicTool(tool)
  
  tool = cfg.popToolsAndMerge(ForwardRegionEnvelopeCfg(ConfigFlags))
  cfg.addPublicTool(tool)
  
  tool = cfg.popToolsAndMerge(CALOEnvelopeCfg(ConfigFlags))
  cfg.addPublicTool(tool)
  
  tool = cfg.popToolsAndMerge(LucidGeoDetectorToolCfg(ConfigFlags))
  cfg.addPublicTool(tool)

  tool = cfg.popToolsAndMerge(ALFAGeoDetectorToolCfg(ConfigFlags))
  cfg.addPublicTool(tool)

  tool = cfg.popToolsAndMerge(ZDCGeoDetectorToolCfg(ConfigFlags))
  cfg.addPublicTool(tool)
  
  tool = cfg.popToolsAndMerge(AFPGeoDetectorToolCfg(ConfigFlags))
  cfg.addPublicTool(tool)

  tool = cfg.popToolsAndMerge(ATLASEnvelopeCfg(ConfigFlags))
  cfg.addPublicTool(tool)

  cfg.addPublicTool(MaterialDescriptionToolCfg(ConfigFlags))

  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","wb")
  cfg.store(f)
  f.close()

  print("-----------------finished----------------------")
