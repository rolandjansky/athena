# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from G4AtlasServices.G4AtlasServicesConfig import DetectorGeometrySvcCfg, PhysicsListSvcCfg
from ISF_Services.ISF_ServicesConfigNew import TruthServiceCfg, InputConverterCfg
from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg
from G4AtlasTools.G4AtlasToolsConfig import SensitiveDetectorMasterToolCfg, FastSimulationMasterToolCfg
from G4AtlasServices.G4AtlasUserActionConfig import UserActionSvcCfg
from SimulationConfig.SimulationMetadata import writeSimulationParametersMetadata
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def G4AtlasAlgCfg(ConfigFlags, name="G4AtlasAlg", **kwargs):
    """Return ComponentAccumulator configured for Atlas G4 simulation, without output"""
    # wihout output
    result = ComponentAccumulator()
    kwargs.setdefault("DetGeoSvc", result.getPrimaryAndMerge(DetectorGeometrySvcCfg(ConfigFlags)).name)

    kwargs.setdefault("InputTruthCollection", "BeamTruthEvent") #tocheck -are these string inputs?
    kwargs.setdefault("OutputTruthCollection", "TruthEvent")
    ## Killing neutrinos

    ## Don"t drop the GeoModel
    kwargs.setdefault("ReleaseGeoModel", ConfigFlags.Sim.ReleaseGeoModel)

    ## Record the particle flux during the simulation
    kwargs.setdefault("RecordFlux", ConfigFlags.Sim.RecordFlux)

    if ConfigFlags.Sim.FlagAbortedEvents:
        ## default false
        kwargs.setdefault("FlagAbortedEvents", ConfigFlags.Sim.FlagAbortedEvents)
        if ConfigFlags.Sim.FlagAbortedEvents and ConfigFlags.Sim.KillAbortedEvents:
            print("WARNING When G4AtlasAlg.FlagAbortedEvents is True G4AtlasAlg.KillAbortedEvents should be False. Setting G4AtlasAlg.KillAbortedEvents = False now.")
            kwargs.setdefault("KillAbortedEvents", False)

    ## default true
    kwargs.setdefault("KillAbortedEvents", ConfigFlags.Sim.KillAbortedEvents)

    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("AtRndmGenSvc",
                      result.getPrimaryAndMerge(AthRNGSvcCfg(ConfigFlags)).name)

    kwargs.setdefault("RandomGenerator", "athena")

    # Multi-threading settinggs
    #is_hive = (concurrencyProps.ConcurrencyFlags.NumThreads() > 0)
    is_hive = ConfigFlags.Concurrency.NumThreads > 0
    kwargs.setdefault("MultiThreading", is_hive)

    kwargs.setdefault("TruthRecordService", result.getPrimaryAndMerge(TruthServiceCfg(ConfigFlags)).name)
    kwargs.setdefault("GeoIDSvc", result.getPrimaryAndMerge(GeoIDSvcCfg(ConfigFlags)).name)

    #input converter
    kwargs.setdefault("InputConverter", result.getPrimaryAndMerge(InputConverterCfg(ConfigFlags)).name)

    #sensitive detector master tool
    SensitiveDetector = result.popToolsAndMerge(SensitiveDetectorMasterToolCfg(ConfigFlags))
    result.addPublicTool(SensitiveDetector)
    kwargs.setdefault("SenDetMasterTool", result.getPublicTool(SensitiveDetector.name))

    #fast simulation master tool
    FastSimulation = result.popToolsAndMerge(FastSimulationMasterToolCfg(ConfigFlags))
    result.addPublicTool(FastSimulation)
    kwargs.setdefault("FastSimMasterTool", result.getPublicTool(FastSimulation.name))

    #Write MetaData container
    result.merge(writeSimulationParametersMetadata(ConfigFlags))

    #User action services (Slow...)
    kwargs.setdefault("UserActionSvc", result.getPrimaryAndMerge(UserActionSvcCfg(ConfigFlags)).name)

    #PhysicsListSvc
    kwargs.setdefault("PhysicsListSvc", result.getPrimaryAndMerge(PhysicsListSvcCfg(ConfigFlags)).name)

    ## G4AtlasAlg verbosities (available domains = Navigator, Propagator, Tracking, Stepping, Stacking, Event)
    ## Set stepper verbose = 1 if the Athena logging level is <= DEBUG
    # TODO: Why does it complain that G4AtlasAlgConf.G4AtlasAlg has no "Verbosities" object? Fix.
    # FIXME GaudiConfig2 seems to fail to distinguish an empty dict {} from None
    verbosities=dict(foo="bar")
    #from AthenaCommon.AppMgr import ServiceMgr
    #if ServiceMgr.MessageSvc.OutputLevel <= 2:
    #    verbosities["Tracking"]="1"
    #    print verbosities
    kwargs.setdefault("Verbosities", verbosities)

    # Set commands for the G4AtlasAlg
    kwargs.setdefault("G4Commands", ConfigFlags.Sim.G4Commands)

    result.addEventAlgo(CompFactory.G4AtlasAlg(name, **kwargs))

    return result
