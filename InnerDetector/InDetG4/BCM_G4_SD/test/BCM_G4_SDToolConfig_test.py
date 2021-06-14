#!/usr/bin/env python
"""Run tests on BCM_G4_SD configuration

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""


if __name__ == '__main__':
  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  ConfigFlags.Sim.ISFRun = True

  #Provide input
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  # Finalize
  ConfigFlags.lock()

  # Setup the tool
  from BCM_G4_SD.BCM_G4_SDToolConfig import BCMSensorSDCfg
  cfg = BCMSensorSDCfg(ConfigFlags)
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","wb")
  cfg.store(f)
  f.close()

  print(cfg._privateTools)
  print("-----------------finished----------------------")
