#!/usr/bin/env python
"""Run tests on G4AtlasServicesConfigNew

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
  
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  # Setup detector flags
  detectors = ['Bpipe', 'BCM', 'DBM', 'Pixel', 'SCT', 'TRT', 'LAr', 'Tile', 'CSC', 'MDT', 'RPC', 'TGC']
  from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
  setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)

  from G4AtlasApps.SimEnums import CavernBackground
  ConfigFlags.Sim.CavernBackground = CavernBackground.Signal  #for it to go via atlas?
  ConfigFlags.Sim.WorldRRange = 15000
  ConfigFlags.Sim.WorldZRange = 27000
  # Finalize 
  ConfigFlags.lock()

  ## Initialize a new component accumulator
  cfg = MainServicesCfg(ConfigFlags)


  from G4AtlasServices.G4AtlasServicesConfigNew import DetectorGeometrySvcCfg
  #add the algorithm
  acc = DetectorGeometrySvcCfg(ConfigFlags)
  cfg.merge(acc)


  # Dump config
  #cfg.getService("StoreGateSvc").Dump = True
  #cfg.getService("ConditionStore").Dump = True
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  f=open("test.pkl","wb")
  cfg.store(f) 
  f.close()

  print ("-----------------finished----------------------")
