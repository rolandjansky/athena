# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from G4AtlasServices.G4AtlasServicesConfigNew import DetectorGeometrySvcCfg, PhysicsListSvcCfg
from ISF_Services.ISF_ServicesConfigNew import MC15aPlusTruthServiceCfg, GeoIDSvcCfg, InputConverterCfg
from G4AtlasTools.G4AtlasToolsConfigNew import SensitiveDetectorMasterToolCfg, FastSimulationMasterToolCfg
from G4AtlasServices.G4AtlasUserActionConfigNew import UserActionSvcCfg
from G4AtlasApps.G4Atlas_MetadataNew import writeSimulationParametersMetadata


#todo - think about the flow, do we need if statements?!

from  G4AtlasAlg.G4AtlasAlgConf import G4AtlasAlg

def G4AtlasAlgCfg(ConfigFlags, name='G4AtlasAlg', **kwargs):
    #add Services to G4AtlasAlg
    result = DetectorGeometrySvcCfg(ConfigFlags)
    kwargs.setdefault('DetGeoSvc', result.getService("DetectorGeometrySvc"))

    
    kwargs.setdefault("InputTruthCollection", "BeamTruthEvent") #tocheck -are these string inputs?
    kwargs.setdefault("OutputTruthCollection", "TruthEvent")
    ## Killing neutrinos
    if ConfigFlags.Sim.ReleaseGeoModel:
        ## Don't drop the GeoModel
        kwargs.setdefault('ReleaseGeoModel' ,ConfigFlags.Sim.ReleaseGeoModel)

    if ConfigFlags.Sim.RecordFlux:
        ## Record the particle flux during the simulation
        kwargs.setdefault('RecordFlux' , ConfigFlags.Sim.RecordFlux)

    if ConfigFlags.Sim.FlagAbortedEvents:
        ## default false
        kwargs.setdefault('FlagAbortedEvents' ,ConfigFlags.Sim.FlagAbortedEvents)
        if ConfigFlags.Sim.FlagAbortedEvents and ConfigFlags.Sim.KillAbortedEvents:
            print('WARNING When G4AtlasAlg.FlagAbortedEvents is True G4AtlasAlg.KillAbortedEvents should be False!!! Setting G4AtlasAlg.KillAbortedEvents = False now!')
            kwargs.setdefault('KillAbortedEvents' ,False)
    if  ConfigFlags.Sim.KillAbortedEvents:
        ## default true
        kwargs.setdefault('KillAbortedEvents' ,ConfigFlags.Sim.KillAbortedEvents)

    from RngComps.RandomServices import AthEngines, RNG
    if ConfigFlags.Random.Engine in AthEngines.keys():
        result.merge(RNG(ConfigFlags.Random.Engine, name="AthRNGSvc"))
        kwargs.setdefault("AtRndmGenSvc",result.getService("AthRNGSvc"))

    kwargs.setdefault("RandomGenerator", "athena")

    # Multi-threading settinggs
    #is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    is_hive = ConfigFlags.Concurrency.NumThreads > 0
    kwargs.setdefault('MultiThreading', is_hive)


    accMCTruth = MC15aPlusTruthServiceCfg(ConfigFlags)
    result.merge(accMCTruth)
    kwargs.setdefault('TruthRecordService', result.getService("ISF_MC15aPlusTruthService"))
    #kwargs.setdefault('TruthRecordService', ConfigFlags.Sim.TruthStrategy) # TODO need to have manual override (simFlags.TruthStrategy.TruthServiceName())

    accGeoID = GeoIDSvcCfg(ConfigFlags)
    result.merge(accGeoID)
    kwargs.setdefault('GeoIDSvc', result.getService('ISF_GeoIDSvc'))

    #input converter
    accInputConverter = InputConverterCfg(ConfigFlags)
    result.merge(accInputConverter)
    kwargs.setdefault('InputConverter', result.getService("ISF_InputConverter"))

    #sensitive detector master tool
    accSensitiveDetector = SensitiveDetectorMasterToolCfg(ConfigFlags)
    result.merge(accSensitiveDetector)
    kwargs.setdefault('SenDetMasterTool', result.getPublicTool("SensitiveDetectorMasterTool")) #NOTE - is still a public tool

    #fast simulation master tool
    accFastSimulation = FastSimulationMasterToolCfg(ConfigFlags)
    result.merge(accFastSimulation)
    kwargs.setdefault('FastSimMasterTool', result.getPublicTool("FastSimulationMasterTool")) # NOTE - is still a public tool

    #Write MetaData container
    result.merge(writeSimulationParametersMetadata(ConfigFlags))

    #User action services (Slow...)
    result.merge( UserActionSvcCfg(ConfigFlags) )
    kwargs.setdefault('UserActionSvc', result.getService( "G4UA::UserActionSvc") )

    #PhysicsListSvc
    result.merge( PhysicsListSvcCfg(ConfigFlags) )
    kwargs.setdefault('PhysicsListSvc', result.getService( "PhysicsListSvc") )

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
    kwargs.setdefault("G4Commands", ConfigFlags.Sim.G4Commands)

    result.addEventAlgo(G4AtlasAlg(name, **kwargs))

    return result
    
