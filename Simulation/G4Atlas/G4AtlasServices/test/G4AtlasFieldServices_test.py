#!/usr/bin/env python
"""Run tests on G4AtlasFieldServices

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

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

  # Finalize 
  ConfigFlags.lock()

  ## Initialize a new component accumulator
  cfg = MainServicesSerialCfg()

  from G4AtlasServices.G4AtlasFieldServices import StandardFieldSvcCfg
  from G4AtlasServices.G4AtlasFieldServices import ForwardFieldSvcCfg

  #add the algorithm
  acc1 = StandardFieldSvcCfg(ConfigFlags)
  acc2 = ForwardFieldSvcCfg(ConfigFlags)

  cfg.merge(acc1)
  cfg.merge(acc2)

  # Dump config
  #cfg.getService("StoreGateSvc").Dump = True
  #cfg.getService("ConditionStore").Dump = True
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  f=open("test.pkl","w")
  cfg.store(f) 
  f.close()



  print cfg._publicTools
  print "-----------------finished----------------------"