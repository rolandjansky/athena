# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

#Based on G4UserActionsConfigNew.py

def VolumeDebuggerToolCfg(ConfigFlags, name='G4UA::VolumeDebuggerTool', **kwargs):
    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.G4UA.VolumeDebuggerTool(name,**kwargs))
    return result

def VolumeDebugger(configFlags, name="G4UA::ISFFullUserActionSvc", **kwargs):
    
    kwargs.setdefault("RunGeoTest",True)
    kwargs.setdefault("Verbose",False)

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    result = ComponentAccumulator()
    #Setting up the CA for the LengthIntegrator
    from G4DebuggingTools.G4DebuggingToolsConfigNew import VolumeDebuggerToolCfg
    actionAcc = ComponentAccumulator()
    actions = []
    actions += [actionAcc.popToolsAndMerge(VolumeDebuggerToolCfg(configFlags,**kwargs))]
    actionAcc.setPrivateTools(actions)
    volumeDebuggerAction = result.popToolsAndMerge(actionAcc)
    
    actionList = volumeDebuggerAction
    #We clear it here because UserActionsTools also wants kwargs, different
    #from the tool above - probably this can be improved...
    kwargs_UATools = {}
    kwargs_UATools.setdefault("UserActionTools",actionList)
    result.addService(CompFactory.G4UA.UserActionSvc(name,**kwargs_UATools))

    return result