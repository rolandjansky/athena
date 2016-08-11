# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def getFastCaloSimParamActionTool(name='G4UA::FastCaloSimParamActionTool', **kwargs):
    from ISF_Geant4UserActions.ISF_Geant4UserActionsConf import G4UA__FastCaloSimParamActionTool
    return G4UA__FastCaloSimParamActionTool(name,**kwargs)

def addFastCaloSimParamActionTool(name='G4UA::FastCaloSimParamActionTool',system=False):
    from G4AtlasServices import G4AtlasServicesConfig
    G4AtlasServicesConfig.addAction(name,['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'],system)

def getTestBoundariesUserActionTool(name='G4UA::iGeant4::TestBoundariesUserActionTool', **kwargs):
    from AthenaCommon import Logging
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() >1:
        log=Logging.logging.getLogger(name)
        log.fatal('Attempt to run '+name+' with more than one thread, which is not supported')
        #from AthenaCommon.AppMgr import theApp
        #theApp.exit(1)
        return False
    from ISF_Geant4UserActions.ISF_Geant4UserActionsConf import G4UA__iGeant4__TestBoundariesUserActionTool
    return G4UA__iGeant4__TestBoundariesUserActionTool(name,**kwargs)
