# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def LengthIntegrator(configFlags, name="G4UA::ISFFullUserActionSvc", **kwargs):
    
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    
    result = ComponentAccumulator()
    
    #Setting up the CA for the LengthIntegrator
    from G4UserActions.G4UserActionsConfigNew import LengthIntegratorToolCfg
    actionAcc = ComponentAccumulator()
    actions = []
    actions += [actionAcc.popToolsAndMerge(LengthIntegratorToolCfg(configFlags))]
    actionAcc.setPrivateTools(actions)
    lengthIntegratorAction = result.popToolsAndMerge(actionAcc)
    
    actionList = lengthIntegratorAction
    #Setting up UserActionsService
    kwargs.setdefault("UserActionTools",actionList)
    result.addService(CompFactory.G4UA.UserActionSvc(name, **kwargs))

    return result
