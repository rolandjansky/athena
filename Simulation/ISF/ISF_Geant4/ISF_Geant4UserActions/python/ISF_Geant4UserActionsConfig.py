# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getFastCaloSimParamActionTool(name='G4UA::FastCaloSimParamActionTool', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__FastCaloSimParamActionTool(name,**kwargs)


def addFastCaloSimParamActionTool(name='G4UA::FastCaloSimParamActionTool',system=False):
    from G4AtlasServices import G4AtlasServicesConfig
    G4AtlasServicesConfig.addAction(name,['Run','Event','Step'],system)


def getTestBoundariesUserActionTool(name='G4UA::iGeant4::TestBoundariesUserActionTool', **kwargs):
    from AthenaCommon import Logging
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() >1:
        log=Logging.logging.getLogger(name)
        log.fatal('Attempt to run '+name+' with more than one thread, which is not supported')
        #from AthenaCommon.AppMgr import theApp
        #theApp.exit(1)
        return False
    return CfgMgr.G4UA__iGeant4__TestBoundariesUserActionTool(name,**kwargs)
