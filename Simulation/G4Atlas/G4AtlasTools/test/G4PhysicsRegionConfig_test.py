#!/usr/bin/env python

"""Run tests on G4PhysicsRegionConfig configuration

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

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

  #import the following tool configs
  from G4AtlasTools.G4PhysicsRegionConfigNew import *
  #add the tools
  cfg.addPublicTool(BeampipeFwdCutPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(FWDBeamLinePhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(FwdRegionPhysicsRegionToolCfg(ConfigFlags))  
  cfg.addPublicTool(PixelPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(SCTPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(TRTPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(TRT_ArPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(TRT_KrPhysicsRegionToolCfg(ConfigFlags)) 
  cfg.addPublicTool(EMBPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(EMECPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(HECPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(FCALPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(EMECParaPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(FCALParaPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(FCAL2ParaPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(PreSampLArPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(DeadMaterialPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(DriftWallPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(DriftWall1PhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(DriftWall2PhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(MuonSystemFastPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(MuonPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(SX1PhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(BedrockPhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(CavernShaftsConcretePhysicsRegionToolCfg(ConfigFlags))
  cfg.addPublicTool(SCTSiliconPhysicsRegionToolCfg(ConfigFlags))



  # Dump config
  #cfg.getService("StoreGateSvc").Dump = True
  #cfg.getService("ConditionStore").Dump = True
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  f=open("test.pkl","wb")
  cfg.store(f) 
  f.close()

  print("-----------------finished----------------------")
