#!/usr/bin/env python
"""Run tests on TRT_G4_SD configuration

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

  ## Initialize the main component accumulator
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  from TRT_G4_SD.TRT_G4_SDToolConfig import TRTSensitiveDetectorCfg
  from TRT_G4_SD.TRT_G4_SDToolConfig import TRTSensitiveDetector_CTBCfg

  tools = []
  cfg = ComponentAccumulator()
  tools += [ cfg.popToolsAndMerge(TRTSensitiveDetectorCfg(ConfigFlags)) ]
  tools += [ cfg.popToolsAndMerge(TRTSensitiveDetector_CTBCfg(ConfigFlags)) ]

  cfg.setPrivateTools(tools)
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()

  f=open("test.pkl","wb")
  cfg.store(f)
  f.close()

  print(cfg._privateTools)
  print("-----------------finished----------------------")
