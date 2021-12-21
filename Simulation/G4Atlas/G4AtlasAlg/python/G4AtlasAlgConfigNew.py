# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from G4AtlasServices.G4AtlasServicesConfigNew import DetectorGeometrySvcCfg, PhysicsListSvcCfg
from ISF_Services.ISF_ServicesConfigNew import TruthServiceCfg, InputConverterCfg
from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg
from G4AtlasTools.G4AtlasToolsConfigNew import SensitiveDetectorMasterToolCfg, FastSimulationMasterToolCfg
from G4AtlasServices.G4AtlasUserActionConfigNew import UserActionSvcCfg
from G4AtlasApps.G4Atlas_MetadataNew import writeSimulationParametersMetadata
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg


def G4AtlasAlgBasicCfg(ConfigFlags, name="G4AtlasAlg", **kwargs):
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

    from RngComps.RandomServices import RNG
    result.merge(RNG(ConfigFlags.Random.Engine, name="AthRNGSvc"))
    kwargs.setdefault("AtRndmGenSvc", result.getService("AthRNGSvc"))

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


def G4AtlasAlgOutputCfg(ConfigFlags):
    """
    Return ComponentAccumulator with output for G4 simulation. Not standalone.

    follows G4Atlas.flat.configuration.py
    """
    ItemList = ["EventInfo#*",
                "McEventCollection#TruthEvent",
                "JetCollection#*"]

    if ConfigFlags.Sim.IncludeParentsInG4Event:
        ItemList += ["McEventCollection#GEN_EVENT"]

    ItemList += ["xAOD::JetContainer#*",
                 "xAOD::JetAuxContainer#*"]

    if ConfigFlags.Detector.EnableID:
        ItemList += ["SiHitCollection#*",
                     "TRTUncompressedHitCollection#*",
                     "TrackRecordCollection#CaloEntryLayer"]

    if ConfigFlags.Detector.EnableITk:
        ItemList += ["SiHitCollection#*",
                     "TrackRecordCollection#CaloEntryLayer"]

    if ConfigFlags.Detector.EnableCalo:
        ItemList += ["CaloCalibrationHitContainer#*",
                     "LArHitContainer#*",
                     "TileHitVector#*",
                     "TrackRecordCollection#MuonEntryLayer"]

    if ConfigFlags.Detector.EnableMuon:
        ItemList += ["RPCSimHitCollection#*",
                     "TGCSimHitCollection#*",
                     "MDTSimHitCollection#*",
                     "TrackRecordCollection#MuonExitLayer"]
        if ConfigFlags.Detector.EnableCSC:
            ItemList += ["CSCSimHitCollection#*"]
        if ConfigFlags.Detector.EnablesTGC:
            ItemList += ["sTGCSimHitCollection#*"]
        if ConfigFlags.Detector.EnableMM:
            ItemList += ["MMSimHitCollection#*"]

    if ConfigFlags.Detector.EnableLucid:
        ItemList += ["LUCID_SimHitCollection#*"]

    if ConfigFlags.Detector.EnableFwdRegion:
        ItemList += ["SimulationHitCollection#*"]

    if ConfigFlags.Detector.EnableZDC:
        ItemList += ["ZDC_SimPixelHit_Collection#*",
                     "ZDC_SimStripHit_Collection#*"]

    if ConfigFlags.Detector.EnableALFA:
        ItemList += ["ALFA_HitCollection#*",
                     "ALFA_ODHitCollection#*"]

    if ConfigFlags.Detector.EnableAFP:
        ItemList += ["AFP_TDSimHitCollection#*",
                     "AFP_SIDSimHitCollection#*"]

    # TimingAlg
    ItemList += ["RecoTimingObj#EVNTtoHITS_timings"]

    # FIXME reproduce to support non standard setups
    #0140         ## Add cosmics and test beam configuration hit persistency if required cf. geom tag
    #0141         layout = simFlags.SimLayout.get_Value()
    #0142         if "tb" not in layout:
    #0143             from AthenaCommon.BeamFlags import jobproperties
    #0144             if jobproperties.Beam.beamType() == 'cosmics' or \
    #0145                     (hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn) or \
    #0146                     (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
    #0147                 stream1.ItemList += ["TrackRecordCollection#CosmicRecord", "TrackRecordCollection#CosmicPerigee"]
    #0148         else:
    #0149             ## CTB-specific
    #0150             if layout.startswith("ctb"):
    #0151                 if simFlags.LArFarUpstreamMaterial.statusOn and simFlags.LArFarUpstreamMaterial.get_Value():
    #0152                     stream1.ItemList.append("TrackRecordCollection#LArFarUpstreamMaterialExitLayer")
    #0153             ## Persistency of test-beam layout
    #0154             if layout.startswith('ctb') or layout.startswith('tb_Tile2000_'):
    #0155                 stream1.ItemList += ["TBElementContainer#*"]
    acc = OutputStreamCfg(ConfigFlags,"HITS", ItemList=ItemList, disableEventTag=True)

    # Make stream aware of aborted events
    OutputStreamHITS = acc.getEventAlgo("OutputStreamHITS")
    OutputStreamHITS.AcceptAlgs += ["G4AtlasAlg"]

    # G4Atlas.flat.configuration.py#0333 onwards
    # FIXME unifyset now fails
    #PoolAttributes = ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
    #PoolAttributes += ["DatabaseName = '" + ConfigFlags.Output.HITSFileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'"]
    #acc.addService(CompFactory.AthenaPoolCnvSvc(PoolAttributes=PoolAttributes))

    return acc


def G4AtlasAlgCfg(ConfigFlags, name="G4AtlasAlg", **kwargs):
    """Return ComponentAccumulator configured for Atlas G4 simulation, with output"""
    acc = G4AtlasAlgBasicCfg(ConfigFlags, **kwargs)
    acc.merge(G4AtlasAlgOutputCfg(ConfigFlags))
    return acc
