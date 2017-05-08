# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getAthenaStackingActionTool(name='G4UA::AthenaStackingActionTool', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if "ATLAS" in simFlags.SimLayout():
        kwargs.setdefault('KillAllNeutrinos',  True)
    return CfgMgr.G4UA__AthenaStackingActionTool(name, **kwargs)

def getAthenaTrackingActionTool(name='G4UA::AthenaTrackingActionTool', **kwargs):
    kwargs.setdefault('SecondarySavingLevel', 2)
    return CfgMgr.G4UA__AthenaTrackingActionTool(name,**kwargs)

def getG4AtlasAlg(name='G4AtlasAlg', **kwargs):
    ## Killing neutrinos
    from G4AtlasApps.SimFlags import simFlags
    if hasattr(simFlags, 'ReleaseGeoModel') and simFlags.ReleaseGeoModel.statusOn:
        ## Don't drop the GeoModel
        kwargs.setdefault('ReleaseGeoModel' ,simFlags.ReleaseGeoModel.get_Value())

    if hasattr(simFlags, 'RecordFlux') and simFlags.RecordFlux.statusOn:
        ## Record the particle flux during the simulation
        kwargs.setdefault('RecordFlux' ,simFlags.RecordFlux.get_Value())

    if hasattr(simFlags, 'IncludeParentsInG4Event') and simFlags.IncludeParentsInG4Event.statusOn:
        ## Propagate quasi-stable particles
        kwargs.setdefault('IncludeParentsInG4Event' ,simFlags.IncludeParentsInG4Event.get_Value())

    if hasattr(simFlags, 'FlagAbortedEvents') and simFlags.FlagAbortedEvents.statusOn:
        ## default false
        kwargs.setdefault('FlagAbortedEvents' ,simFlags.FlagAbortedEvents.get_Value())
        if simFlags.FlagAbortedEvents.get_Value() and simFlags.KillAbortedEvents.get_Value():
            print 'WARNING When G4AtlasAlg.FlagAbortedEvents is True G4AtlasAlg.KillAbortedEvents should be False!!! Setting G4AtlasAlg.KillAbortedEvents = False now!'
            kwargs.setdefault('KillAbortedEvents' ,False)
    if hasattr(simFlags, 'KillAbortedEvents') and simFlags.KillAbortedEvents.statusOn:
        ## default true
        kwargs.setdefault('KillAbortedEvents' ,simFlags.KillAbortedEvents.get_Value())

    if hasattr(simFlags, 'RandomSvc') and simFlags.RandomSvc.statusOn:
        ## default true
        kwargs.setdefault('AtRndmGenSvc' ,simFlags.RandomSvc.get_Value())
    if not simFlags.RandomSeedList.checkForExistingSeed('AtlasG4'):
        simFlags.RandomSeedList.addSeed( "AtlasG4", 423451, 3213210 )

    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() > 0:
        is_hive = True
    else:
        is_hive = False
    kwargs.setdefault('MultiThreading', is_hive)

    kwargs.setdefault('TruthRecordService', 'ISF_MC12TruthService')
    kwargs.setdefault('GeoIDSvc', 'ISF_GeoIDSvc')

    ## G4AtlasAlg verbosities (available domains = Navigator, Propagator, Tracking, Stepping, Stacking, Event)
    ## Set stepper verbose = 1 if the Athena logging level is <= DEBUG
    # TODO: Why does it complain that G4AtlasAlgConf.G4AtlasAlg has no "Verbosities" object? Fix.
    verbosities=dict()
    #from AthenaCommon.AppMgr import ServiceMgr
    #if ServiceMgr.MessageSvc.OutputLevel <= 2:
    #    verbosities["Tracking"]='1'
    #    print verbosities
    kwargs.setdefault('Verbosities', verbosities)

    # Set commands for the G4AtlasAlg
    kwargs.setdefault("G4Commands", simFlags.G4Commands.get_Value())

    return CfgMgr.G4AtlasAlg(name, **kwargs)
