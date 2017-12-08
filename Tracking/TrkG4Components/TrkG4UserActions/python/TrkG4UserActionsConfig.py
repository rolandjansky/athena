# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def getMaterialStepRecorderTool(name="G4UA::MaterialStepRecorderTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    from TrkG4UserActions.TrkG4UserActionsConf import G4UA__MaterialStepRecorderTool
    return G4UA__MaterialStepRecorderTool(name, **kwargs)
