# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getQuirkPhysicsTool(name="QuirkPhysicsTool", **kwargs):
    return CfgMgr.QuirkPhysicsToolTool(name, **kwargs)

def getDebugSteppingActionTool(name="G4UA::DebugSteppingActionTool", **kwargs):
    from Quirks.QuirksConf import G4UA__DebugSteppingActionTool
    from G4AtlasApps.SimFlags import simFlags
    # use configuration passed through the flags
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
            
    return G4UA__DebugSteppingActionTool(name, **kwargs)


