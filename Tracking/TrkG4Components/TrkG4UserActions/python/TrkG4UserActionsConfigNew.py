# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

"""Define methods to configure TrkG4UserActions"""

def MaterialStepRecorderCfg(configFlags, name="G4UA::UserActionSvc.MaterialStepRecorderTool", **kwargs):
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  from AthenaConfiguration.ComponentFactory import CompFactory
  result = ComponentAccumulator()  
  result.setPrivateTools(CompFactory.G4UA.MaterialStepRecorderTool(name, **kwargs))
  return result
  

def MaterialStepRecorderUserActionSvcCfg(configFlags, name="G4UA::MaterialStepRecorderUserActionSvc", **kwargs):
  from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
  from AthenaConfiguration.ComponentFactory import CompFactory
  
  result = ComponentAccumulator()
  
  #Setting up the CA for the MaterialStepRecorder
  actionAcc = ComponentAccumulator()
  actions = []
  actions += [actionAcc.popToolsAndMerge(MaterialStepRecorderCfg(configFlags))]
  actionAcc.setPrivateTools(actions)
  MaterialStepRecorderAction = result.popToolsAndMerge(actionAcc)
  
  #Retrieving the default action list
  from G4AtlasServices.G4AtlasUserActionConfigNew import getDefaultActions
  defaultActions = result.popToolsAndMerge(getDefaultActions(configFlags))
    
  #Adding LengthIntegrator to defaults
  actionList = (defaultActions + MaterialStepRecorderAction)

  #Setting up UserActionsService
  kwargs.setdefault("UserActionTools",actionList)
  result.addService(CompFactory.G4UA.UserActionSvc(name, **kwargs))    
    
  return result
