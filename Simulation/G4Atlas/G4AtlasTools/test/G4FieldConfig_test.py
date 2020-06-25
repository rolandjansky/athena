#!/usr/bin/env python
"""Run tests on G4AtlasFieldConfig

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from __future__ import print_function

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

  # Finalize
#  ConfigFlags.lock()

  ## Initialize a new component accumulator
  cfg = MainServicesCfg(ConfigFlags)


  from G4AtlasTools.G4FieldConfigNew import ATLASFieldManagerToolCfg, TightMuonsATLASFieldManagerToolCfg, Q1FwdFieldManagerToolCfg
  #add the algorithm
  acc1 = ATLASFieldManagerToolCfg(ConfigFlags)
  acc2 = TightMuonsATLASFieldManagerToolCfg(ConfigFlags)

  cfg.popToolsAndMerge(acc1)
  cfg.popToolsAndMerge(acc2)

  #don't run for simulation only tests (todo - make new general test)
  import os
  if not "AthSimulation_DIR" in os.environ:
    acc3 = Q1FwdFieldManagerToolCfg(ConfigFlags)
    cfg.popToolsAndMerge(acc3)


  # Dump config
  #cfg.getService("StoreGateSvc").Dump = True
  #cfg.getService("ConditionStore").Dump = True
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  f=open("test.pkl","wb")
  cfg.store(f)
  f.close()

  print("-----------------finished----------------------")
