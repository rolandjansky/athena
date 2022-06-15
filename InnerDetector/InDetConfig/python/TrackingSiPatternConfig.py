# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def SiSpacePointsSeedMakerCfg(flags, name="InDetSpSeedsMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- Space points seeds maker, use different ones for cosmics and collisions
    #
    if flags.Beam.Type is BeamType.Cosmics:
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_Cosmic
    elif flags.Reco.EnableHI:
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_HeavyIon
    elif flags.InDet.Tracking.ActivePass.isLowPt:
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_LowMomentum
    elif flags.InDet.Tracking.ActivePass.extension == "BeamGas":
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_BeamGas
    else:
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_ATLxk
        if flags.InDet.Tracking.writeSeedValNtuple:
            kwargs.setdefault("WriteNtuple", True)
            HistService = CompFactory.THistSvc(Output = ["valNtuples DATAFILE='SeedMakerValidation.root' OPT='RECREATE'"])
            acc.addService(HistService)

    kwargs.setdefault("pTmin", flags.InDet.Tracking.ActivePass.minPT )
    kwargs.setdefault("maxdImpact", flags.InDet.Tracking.ActivePass.maxPrimaryImpact )
    kwargs.setdefault("maxZ", flags.InDet.Tracking.ActivePass.maxZImpact )
    kwargs.setdefault("minZ", -flags.InDet.Tracking.ActivePass.maxZImpact )
    kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel and flags.InDet.Tracking.ActivePass.extension != "R3LargeD0")
    kwargs.setdefault("SpacePointsPixelName", 'PixelSpacePoints') # InDetKeys.PixelSpacePoints()
    kwargs.setdefault("useSCT", flags.InDet.Tracking.ActivePass.useSCT and flags.InDet.Tracking.ActivePass.useSCTSeeding )
    kwargs.setdefault("SpacePointsSCTName", 'SCT_SpacePoints') # InDetKeys.SCT_SpacePoints()
    kwargs.setdefault("useOverlapSpCollection", flags.InDet.Tracking.ActivePass.useSCT and flags.InDet.Tracking.ActivePass.useSCTSeeding )
    kwargs.setdefault("SpacePointsOverlapName", 'OverlapSpacePoints') # InDetKeys.OverlapSpacePoints()
    kwargs.setdefault("radMax", flags.InDet.Tracking.ActivePass.radMax)
    kwargs.setdefault("RapidityCut",  flags.InDet.Tracking.ActivePass.maxEta )

    if not flags.Reco.EnableHI \
       and (flags.InDet.Tracking.ActivePass.extension=="" or flags.InDet.Tracking.ActivePass.extension=="Forward" or flags.InDet.Tracking.ActivePass.extension=="BLS"):
        kwargs.setdefault("maxdImpactPPS", flags.InDet.Tracking.ActivePass.maxdImpactPPSSeeds)
        kwargs.setdefault("maxdImpactSSS", flags.InDet.Tracking.ActivePass.maxdImpactSSSSeeds)
        kwargs.setdefault("maxSeedsForSpacePointStrips", flags.InDet.Tracking.ActivePass.maxSeedsPerSP_Strips)
        kwargs.setdefault("maxSeedsForSpacePointPixels", flags.InDet.Tracking.ActivePass.maxSeedsPerSP_Pixels)
        kwargs.setdefault("alwaysKeepConfirmedStripSeeds", flags.InDet.Tracking.ActivePass.keepAllConfirmedStripSeeds)
        kwargs.setdefault("alwaysKeepConfirmedPixelSeeds", flags.InDet.Tracking.ActivePass.keepAllConfirmedPixelSeeds)
        kwargs.setdefault("mindRadius", 10)
        kwargs.setdefault("maxSizeSP", 200)
        kwargs.setdefault("dImpactCutSlopeUnconfirmedSSS", 1.25)
        kwargs.setdefault("dImpactCutSlopeUnconfirmedPPP", 2.0)

    if flags.Reco.EnableHI:
        kwargs.setdefault("maxdImpactPPS", flags.InDet.Tracking.ActivePass.maxdImpactPPSSeeds)
        kwargs.setdefault("maxdImpactSSS", flags.InDet.Tracking.ActivePass.maxdImpactSSSSeeds)

    if (len(InputCollections) > 0) and flags.InDet.Tracking.ActivePass.usePrdAssociationTool:
        # not all classes have that property !!!
        kwargs.setdefault("PRDtoTrackMap", 'InDetPRDtoTrackMap'+ flags.InDet.Tracking.ActivePass.extension)

    if flags.Beam.Type is not BeamType.Cosmics:
        kwargs.setdefault("maxRadius1", 0.75*flags.InDet.Tracking.ActivePass.radMax)
        kwargs.setdefault("maxRadius2", flags.InDet.Tracking.ActivePass.radMax)
        kwargs.setdefault("maxRadius3", flags.InDet.Tracking.ActivePass.radMax)

    if flags.InDet.Tracking.ActivePass.isLowPt:
        kwargs.setdefault("pTmax", flags.InDet.Tracking.ActivePass.maxPT)
        kwargs.setdefault("mindRadius", 4.0)

    if flags.InDet.Tracking.ActivePass.extension == "R3LargeD0":
        kwargs.setdefault("optimisePhiBinning", False)
        kwargs.setdefault("etaMax", flags.InDet.Tracking.ActivePass.maxEta)
        kwargs.setdefault("maxSeedsForSpacePointStrips", flags.InDet.Tracking.ActivePass.maxSeedsPerSP_Strips)
        kwargs.setdefault("alwaysKeepConfirmedStripSeeds", flags.InDet.Tracking.ActivePass.keepAllConfirmedStripSeeds)
        kwargs.setdefault("maxdRadius", 150)
        kwargs.setdefault("seedScoreBonusConfirmationSeed", -2000)
    elif flags.InDet.Tracking.ActivePass.extension == "Forward":
        kwargs.setdefault("checkEta", True)
        kwargs.setdefault("etaMin", flags.InDet.Tracking.ActivePass.minEta)
        kwargs.setdefault("etaMax", flags.InDet.Tracking.ActivePass.maxEta)
    elif flags.InDet.Tracking.ActivePass.extension == "DBM":
        kwargs.setdefault("etaMin", flags.InDet.Tracking.ActivePass.minEta)
        kwargs.setdefault("etaMax", flags.InDet.Tracking.ActivePass.maxEta)
        kwargs.setdefault("useDBM", True)

    InDetSiSpacePointsSeedMaker = SiSpacePointsSeedMaker (name = name+flags.InDet.Tracking.ActivePass.extension, **kwargs)

    acc.setPrivateTools(InDetSiSpacePointsSeedMaker)
    return acc

def SiZvertexMaker_xkCfg(flags, name="InDetZvertexMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("Zmax", flags.InDet.Tracking.ActivePass.maxZImpact)
    kwargs.setdefault("Zmin", -flags.InDet.Tracking.ActivePass.maxZImpact)
    kwargs.setdefault("minRatio", 0.17)

    InDetSiSpacePointsSeedMaker = acc.popToolsAndMerge(SiSpacePointsSeedMakerCfg(flags,
                                                                                 InputCollections=InputCollections))

    kwargs.setdefault("SeedMakerTool", InDetSiSpacePointsSeedMaker)
    if flags.Reco.EnableHI:
        kwargs.setdefault("HistSize", 2000)
        kwargs.setdefault("minContent", 30)

    InDetZvertexMaker = CompFactory.InDet.SiZvertexMaker_xk(name = name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(InDetZvertexMaker)
    return acc

def SiTrackMaker_xkCfg(flags, name="InDetSiTrackMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("useSCT", flags.InDet.Tracking.ActivePass.useSCT)
    kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel)

    from InDetConfig.SiDetElementsRoadToolConfig import SiDetElementsRoadMaker_xkCfg
    kwargs.setdefault("RoadTool", acc.popToolsAndMerge(SiDetElementsRoadMaker_xkCfg(flags)))

    from InDetConfig.SiCombinatorialTrackFinderToolConfig import SiCombinatorialTrackFinder_xkCfg
    kwargs.setdefault("CombinatorialTrackFinder", acc.popToolsAndMerge(SiCombinatorialTrackFinder_xkCfg(flags)))

    kwargs.setdefault("pTmin", flags.InDet.Tracking.ActivePass.minPT)
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.ActivePass.minPTBrem)
    kwargs.setdefault("pTminSSS", flags.InDet.Tracking.ActivePass.pT_SSScut)
    kwargs.setdefault("nClustersMin", flags.InDet.Tracking.ActivePass.minClusters)
    kwargs.setdefault("nHolesMax", flags.InDet.Tracking.ActivePass.nHolesMax)
    kwargs.setdefault("nHolesGapMax", flags.InDet.Tracking.ActivePass.nHolesGapMax)
    kwargs.setdefault("SeedsFilterLevel", flags.InDet.Tracking.ActivePass.seedFilterLevel)
    kwargs.setdefault("Xi2max", flags.InDet.Tracking.ActivePass.Xi2max)
    kwargs.setdefault("Xi2maxNoAdd", flags.InDet.Tracking.ActivePass.Xi2maxNoAdd)
    kwargs.setdefault("nWeightedClustersMin", flags.InDet.Tracking.ActivePass.nWeightedClustersMin)
    kwargs.setdefault("CosmicTrack", flags.Beam.Type is BeamType.Cosmics)
    kwargs.setdefault("Xi2maxMultiTracks", flags.InDet.Tracking.ActivePass.Xi2max)
    kwargs.setdefault("useSSSseedsFilter", True)
    kwargs.setdefault("doMultiTracksProd", True)
    kwargs.setdefault("useBremModel", flags.InDet.Tracking.doBremRecovery and flags.Detector.EnableCalo
                      and (flags.InDet.Tracking.ActivePass.extension=="" or flags.InDet.Tracking.ActivePass.extension=="BLS") )
    kwargs.setdefault("doCaloSeededBrem", flags.InDet.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo)
    if kwargs["useBremModel"] and kwargs["doCaloSeededBrem"]:
        from InDetConfig.InDetCaloClusterROISelectorConfig import CaloClusterROIPhiRZContainerMakerCfg
        acc.merge(CaloClusterROIPhiRZContainerMakerCfg(flags))
    kwargs.setdefault("doHadCaloSeedSSS", flags.InDet.Tracking.doHadCaloSeededSSS and flags.Detector.EnableCalo)
    if kwargs["doHadCaloSeedSSS"]:
        from InDetConfig.InDetCaloClusterROISelectorConfig import HadCaloClusterROIPhiRZContainerMakerCfg
        acc.merge(HadCaloClusterROIPhiRZContainerMakerCfg(flags))
    kwargs.setdefault("phiWidth", flags.InDet.Tracking.ActivePass.phiWidthBrem)
    kwargs.setdefault("etaWidth", flags.InDet.Tracking.ActivePass.etaWidthBrem)
    kwargs.setdefault("EMROIPhiRZContainer", "InDetCaloClusterROIPhiRZ0GeV")
    kwargs.setdefault("HadROIPhiRZContainer", "InDetHadCaloClusterROIPhiRZ")
    kwargs.setdefault("UseAssociationTool", (len(InputCollections) > 0) and (flags.InDet.Tracking.ActivePass.usePrdAssociationTool))

    if flags.InDet.Tracking.ActivePass.extension == "DBM":
        kwargs.setdefault("MagneticFieldMode", "NoField")
        kwargs.setdefault("useBremModel", False)
        kwargs.setdefault("doMultiTracksProd", False)
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSPSeededFinder')
        kwargs.setdefault("pTminSSS", -1)
        kwargs.setdefault("CosmicTrack", False)
        kwargs.setdefault("useSSSseedsFilter", False)
        kwargs.setdefault("doCaloSeededBrem", False)
        kwargs.setdefault("doHadCaloSeedSSS", False)

    elif flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_Cosmic')

    elif flags.Reco.EnableHI:
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_HeavyIon')
    
    elif flags.InDet.Tracking.ActivePass.extension == "LowPt":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_LowMomentum')

    elif flags.InDet.Tracking.ActivePass.extension == "VeryLowPt" or (flags.InDet.Tracking.ActivePass.extension == "Pixel" and flags.InDet.Tracking.doMinBias):
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_VeryLowMomentum')

    elif flags.InDet.Tracking.ActivePass.extension == "BeamGas":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_BeamGas')

    elif flags.InDet.Tracking.ActivePass.extension == "Forward":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_ForwardTracks')

    elif flags.InDet.Tracking.ActivePass.extension == "LargeD0" or flags.InDet.Tracking.ActivePass.extension == "R3LargeD0" or flags.InDet.Tracking.ActivePass.extension == "LowPtLargeD0":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_LargeD0')

    else:
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSPSeededFinder')

    if flags.InDet.Tracking.doStoreTrackSeeds:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("SeedToTrackConversion", CompFactory.InDet.SeedToTrackConversionTool(
            name="InDet_SeedToTrackConversion",
            Extrapolator=acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)),
            OutputName=f"SiSPSeedSegments{flags.InDet.Tracking.ActivePass.extension}",
        ))
        kwargs.setdefault("SeedSegmentsWrite", True)

    InDetSiTrackMaker = CompFactory.InDet.SiTrackMaker_xk(name = name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(InDetSiTrackMaker)
    return acc

def SiSPSeededTrackFinderCfg(flags, name="InDetSiSpTrackFinder", InputCollections = None, SiSPSeededTrackCollectionKey = None, **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.TrkExRungeKuttaPropagatorConfig import InDetPropagatorCfg
    InDetPropagator = acc.popToolsAndMerge(InDetPropagatorCfg(flags))
    acc.addPublicTool(InDetPropagator)

    #
    # --- Setup Track finder using space points seeds
    #
    kwargs.setdefault("TrackTool", acc.popToolsAndMerge(SiTrackMaker_xkCfg(flags, InputCollections = InputCollections)))
    kwargs.setdefault("PropagatorTool", InDetPropagator)
    from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolNoHoleSearchCfg
    kwargs.setdefault("TrackSummaryTool", acc.popToolsAndMerge(InDetTrackSummaryToolNoHoleSearchCfg(flags)))
    kwargs.setdefault("TracksLocation", SiSPSeededTrackCollectionKey)
    kwargs.setdefault("SeedsTool", acc.popToolsAndMerge(SiSpacePointsSeedMakerCfg(flags, InputCollections = InputCollections)))
    kwargs.setdefault("useMBTSTimeDiff", flags.Reco.EnableHI) # Heavy-ion config

    if (len(InputCollections) > 0) and flags.InDet.Tracking.ActivePass.usePrdAssociationTool:
        # not all classes have that property !!!
        kwargs.setdefault("PRDtoTrackMap", 'InDetPRDtoTrackMap'+ flags.InDet.Tracking.ActivePass.extension)

    if flags.InDet.Tracking.ActivePass.extension == "Forward":
        kwargs.setdefault("useZvertexTool", flags.Reco.EnableHI) # For heavy-ion
        kwargs.setdefault("useZBoundFinding", False)
    elif flags.InDet.Tracking.ActivePass.extension == "DBM":
        kwargs.setdefault("useZvertexTool", False)
        kwargs.setdefault("useZBoundFinding", False)
    else:
        kwargs.setdefault("useZvertexTool", flags.Reco.EnableHI) # For heavy-ion
        kwargs.setdefault("useZBoundFinding", flags.InDet.Tracking.ActivePass.doZBoundary)
    
    #
    # --- Z-coordinates primary vertices finder (only for collisions)
    #
    if kwargs["useZvertexTool"]:
        kwargs.setdefault("ZvertexTool", acc.popToolsAndMerge(SiZvertexMaker_xkCfg(flags, InputCollections = InputCollections)))

    if flags.Reco.EnableHI:
        kwargs.setdefault("FreeClustersCut",2) #Heavy Ion optimization from Igor

    InDetSiSPSeededTrackFinder = CompFactory.InDet.SiSPSeededTrackFinder(name = name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.addEventAlgo(InDetSiSPSeededTrackFinder)
    return acc

# ------------------------------------------------------------
#
# ----------- Setup Si Pattern for New tracking
#
# ------------------------------------------------------------
def TrackingSiPatternCfg(flags, InputCollections = None, ResolvedTrackCollectionKey = None, SiSPSeededTrackCollectionKey = None , ClusterSplitProbContainer=''):
    acc = ComponentAccumulator()
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if (len(InputCollections) > 0) and flags.InDet.Tracking.ActivePass.usePrdAssociationTool:
        from InDetConfig.InDetTrackPRD_AssociationConfig import InDetTrackPRD_AssociationCfg
        acc.merge(InDetTrackPRD_AssociationCfg(flags,
                                               name = 'InDetTrackPRD_Association' + flags.InDet.Tracking.ActivePass.extension,
                                               TracksName = list(InputCollections)))

    # ------------------------------------------------------------
    #
    # ----------- SiSPSeededTrackFinder
    #
    # ------------------------------------------------------------

    acc.merge(SiSPSeededTrackFinderCfg( flags,
                                        InputCollections = InputCollections,
                                        SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey))

    # ------------------------------------------------------------
    #
    # ---------- Ambiguity solving
    #
    # ------------------------------------------------------------

    from TrkConfig.TrkAmbiguitySolverConfig import TrkAmbiguityScoreCfg, TrkAmbiguitySolverCfg
    acc.merge(TrkAmbiguityScoreCfg( flags,
                                    SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey,
                                    ClusterSplitProbContainer = ClusterSplitProbContainer))

    acc.merge(TrkAmbiguitySolverCfg(flags,
                                    ResolvedTrackCollectionKey = ResolvedTrackCollectionKey,
                                    ClusterSplitProbContainer = ClusterSplitProbContainer))

    return acc


