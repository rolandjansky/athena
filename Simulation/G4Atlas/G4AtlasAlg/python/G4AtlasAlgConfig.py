# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def getAthenaStackingAction(name='AthenaStackingAction', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if "ATLAS" in simFlags.SimLayout():
        kwargs.setdefault('KillAllNeutrinos',  True)
    from G4AtlasAlg.G4AtlasAlgConf import AthenaStackingAction
    return AthenaStackingAction(name,**kwargs)

def getAthenaTrackingAction(name='AthenaTrackingAction', **kwargs):
    from G4AtlasAlg.G4AtlasAlgConf import AthenaTrackingAction
    return AthenaTrackingAction(name,**kwargs)
