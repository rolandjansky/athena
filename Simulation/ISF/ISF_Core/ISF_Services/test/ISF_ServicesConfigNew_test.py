#!/usr/bin/env python
"""Run tests on ISF_ServicesConfigNew.py

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
if __name__ == '__main__':
  from AthenaConfiguration.MainServicesConfig import MainServicesCfg
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

  ConfigFlags.Sim.WorldRRange = 15000
  ConfigFlags.Sim.WorldZRange = 27000 #change defaults?
  ConfigFlags.Detector.SimulateForward = False
  # Finalize 
  ConfigFlags.lock()

  from ISF_Services.ISF_ServicesConfigNew import MC15aPlusTruthServiceCfg, InputConverterCfg
  from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg

  ## Initialize a new component accumulator
  cfg = MainServicesCfg(ConfigFlags)

  #add the algorithm
  cfg.merge(MC15aPlusTruthServiceCfg(ConfigFlags))
  cfg.merge(InputConverterCfg(ConfigFlags))
  cfg.merge(GeoIDSvcCfg(ConfigFlags))

  # Dump config
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  f=open("test.pkl","wb")
  cfg.store(f)
  f.close()
