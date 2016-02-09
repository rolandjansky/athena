# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def getFastCaloSimParamActionTool(name='G4UA::FastCaloSimParamActionTool', **kwargs):
    from ISF_Geant4UserActions.ISF_Geant4USerActionsConf import G4UA__FastCaloSimParamActionTool
    return G4UA__FastCaloSimParamActionTool(name,**kwargs)

def addFastCaloSimParamActionTool(name='G4UA::FastCaloSimParamActionTool',system=False):
    from G4AtlasServices import G4AtlasServicesConfig
    G4AtlasServicesConfig.addAction(name,['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'],system)
