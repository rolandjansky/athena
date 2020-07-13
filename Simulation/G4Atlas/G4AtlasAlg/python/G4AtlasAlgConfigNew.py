# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from G4AtlasServices.G4AtlasServicesConfigNew import DetectorGeometrySvcCfg, PhysicsListSvcCfg
from ISF_Services.ISF_ServicesConfigNew import MC15aPlusTruthServiceCfg, InputConverterCfg
from ISF_Services.ISF_ServicesCoreConfigNew import GeoIDSvcCfg
from G4AtlasTools.G4AtlasToolsConfigNew import SensitiveDetectorMasterToolCfg, FastSimulationMasterToolCfg
from G4AtlasServices.G4AtlasUserActionConfigNew import UserActionSvcCfg
from G4AtlasApps.G4Atlas_MetadataNew import writeSimulationParametersMetadata
from AthenaConfiguration.ComponentFactory import CompFactory
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg


def G4AtlasAlgBasicCfg(ConfigFlags, name="G4AtlasAlg", **kwargs):
    """Return ComponentAccumulator configured for Atlas G4 simulation, without output"""
    # wihout output
    result = DetectorGeometrySvcCfg(ConfigFlags)
    kwargs.setdefault("DetGeoSvc", result.getService("DetectorGeometrySvc"))

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


    accMCTruth = MC15aPlusTruthServiceCfg(ConfigFlags)
    result.merge(accMCTruth)
    kwargs.setdefault("TruthRecordService", result.getService("ISF_MC15aPlusTruthService"))
    #kwargs.setdefault("TruthRecordService", ConfigFlags.Sim.TruthStrategy) # TODO need to have manual override (simFlags.TruthStrategy.TruthServiceName())

    accGeoID = GeoIDSvcCfg(ConfigFlags)
    result.merge(accGeoID)
    kwargs.setdefault("GeoIDSvc", result.getService("ISF_GeoIDSvc"))

    #input converter
    accInputConverter = InputConverterCfg(ConfigFlags)
    result.merge(accInputConverter)
    kwargs.setdefault("InputConverter", result.getService("ISF_InputConverter"))

    #sensitive detector master tool
    accSensitiveDetector = SensitiveDetectorMasterToolCfg(ConfigFlags)
    result.merge(accSensitiveDetector)
    kwargs.setdefault("SenDetMasterTool", result.getPublicTool("SensitiveDetectorMasterTool")) #NOTE - is still a public tool

    #fast simulation master tool
    accFastSimulation = FastSimulationMasterToolCfg(ConfigFlags)
    result.merge(accFastSimulation)
    kwargs.setdefault("FastSimMasterTool", result.getPublicTool("FastSimulationMasterTool")) # NOTE - is still a public tool

    #Write MetaData container
    result.merge(writeSimulationParametersMetadata(ConfigFlags))

    #User action services (Slow...)
    result.merge( UserActionSvcCfg(ConfigFlags) )
    kwargs.setdefault("UserActionSvc", result.getService( "G4UA::UserActionSvc") )

    #PhysicsListSvc
    result.merge( PhysicsListSvcCfg(ConfigFlags) )
    kwargs.setdefault("PhysicsListSvc", result.getService( "PhysicsListSvc") )

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

    if ConfigFlags.Detector.SimulateID:
        ItemList += ["SiHitCollection#*",
                     "TRTUncompressedHitCollection#*",
                     "TrackRecordCollection#CaloEntryLayer"]

    if ConfigFlags.Detector.SimulateCalo:
        ItemList += ["CaloCalibrationHitContainer#*",
                     "LArHitContainer#*",
                     "TileHitVector#*",
                     "TrackRecordCollection#MuonEntryLayer"]

    if ConfigFlags.Detector.SimulateMuon:
        ItemList += ["RPCSimHitCollection#*",
                     "TGCSimHitCollection#*",
                     "MDTSimHitCollection#*",
                     "TrackRecordCollection#MuonExitLayer"]
        if ConfigFlags.Detector.GeometryCSC:
            ItemList += ["CSCSimHitCollection#*"]
        if ConfigFlags.Detector.GeometrysTGC:
            ItemList += ["sTGCSimHitCollection#*"]
        if ConfigFlags.Detector.GeometryMM:
            ItemList += ["MMSimHitCollection#*"]

    if ConfigFlags.Detector.SimulateLucid:
        ItemList += ["LUCID_SimHitCollection#*"]

    if ConfigFlags.Detector.SimulateFwdRegion:
        ItemList += ["SimulationHitCollection#*"]

    if ConfigFlags.Detector.SimulateZDC:
        ItemList += ["ZDC_SimPixelHit_Collection#*",
                     "ZDC_SimStripHit_Collection#*"]

    if ConfigFlags.Detector.SimulateALFA:
        ItemList += ["ALFA_HitCollection#*",
                     "ALFA_ODHitCollection#*"]

    if ConfigFlags.Detector.SimulateAFP:
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
    acc = OutputStreamCfg(ConfigFlags,"HITS", ItemList=ItemList)

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
