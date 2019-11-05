# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

from AthenaCommon import CfgMgr

def getG4AtlasAlg(name='G4AtlasAlg', **kwargs):
    kwargs.setdefault("InputTruthCollection", "BeamTruthEvent")
    kwargs.setdefault("OutputTruthCollection", "TruthEvent")
    ## Killing neutrinos
    from G4AtlasApps.SimFlags import simFlags
    if hasattr(simFlags, 'ReleaseGeoModel') and simFlags.ReleaseGeoModel.statusOn:
        ## Don't drop the GeoModel
        kwargs.setdefault('ReleaseGeoModel' ,simFlags.ReleaseGeoModel.get_Value())

    if hasattr(simFlags, 'RecordFlux') and simFlags.RecordFlux.statusOn:
        ## Record the particle flux during the simulation
        kwargs.setdefault('RecordFlux' ,simFlags.RecordFlux.get_Value())

    if hasattr(simFlags, 'FlagAbortedEvents') and simFlags.FlagAbortedEvents.statusOn:
        ## default false
        kwargs.setdefault('FlagAbortedEvents' ,simFlags.FlagAbortedEvents.get_Value())
        if simFlags.FlagAbortedEvents.get_Value() and simFlags.KillAbortedEvents.get_Value():
            print('WARNING When G4AtlasAlg.FlagAbortedEvents is True G4AtlasAlg.KillAbortedEvents should be False!!! Setting G4AtlasAlg.KillAbortedEvents = False now!')
            kwargs.setdefault('KillAbortedEvents' ,False)
    if hasattr(simFlags, 'KillAbortedEvents') and simFlags.KillAbortedEvents.statusOn:
        ## default true
        kwargs.setdefault('KillAbortedEvents' ,simFlags.KillAbortedEvents.get_Value())

    if hasattr(simFlags, 'RandomSvcMT') and simFlags.RandomSvcMT.statusOn:
        ## default true
        kwargs.setdefault('AtRndmGenSvc', simFlags.RandomSvcMT.get_Value())
    kwargs.setdefault("RandomGenerator", "athena")

    # Multi-threading settinggs
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    kwargs.setdefault('MultiThreading', is_hive)
    if is_hive:
        kwargs.setdefault('Cardinality', concurrencyProps.ConcurrencyFlags.NumThreads())

    kwargs.setdefault('TruthRecordService', simFlags.TruthStrategy.TruthServiceName())
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
