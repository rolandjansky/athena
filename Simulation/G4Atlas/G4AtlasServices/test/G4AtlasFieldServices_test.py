#!/usr/bin/env python
"""Run tests on G4AtlasFieldServices

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
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

  # Finalize 
  ConfigFlags.lock()

  ## Initialize a new component accumulator
  cfg = MainServicesCfg(ConfigFlags)

  from G4AtlasServices.G4AtlasFieldServices import StandardFieldSvcCfg
  from G4AtlasServices.G4AtlasFieldServices import ForwardFieldSvcCfg
  from G4AtlasServices.G4AtlasFieldServices import Q1FwdG4FieldSvcCfg

  #add the algorithm
  acc1 = StandardFieldSvcCfg(ConfigFlags)
  acc2 = ForwardFieldSvcCfg(ConfigFlags)

  #don't run for simulation only tests (todo - make new general test)
  import os
  if not "AthSimulation_DIR" in os.environ:
    acc3 = Q1FwdG4FieldSvcCfg(ConfigFlags)
    cfg.merge(acc3)

  cfg.merge(acc1)
  cfg.merge(acc2)

  # Dump config
  #cfg.getService("StoreGateSvc").Dump = True
  #cfg.getService("ConditionStore").Dump = True
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  f=open("test.pkl","wb")
  cfg.store(f)
  f.close()



  print (cfg._publicTools)
  print ("-----------------finished----------------------")
