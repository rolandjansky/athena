# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
import InDetConfig.TrackingCommonConfig         as   TC

def SiSpacePointsSeedMakerCfg(flags, name="InDetSpSeedsMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- Space points seeds maker, use different ones for cosmics and collisions
    #
    if flags.Beam.Type == 'cosmics':
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_Cosmic
    elif flags.Reco.EnableHI:
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_HeavyIon
    elif flags.InDet.Tracking.ActivePass.isLowPt:
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_LowMomentum
    elif flags.InDet.Tracking.ActivePass.extension == "BeamGas":
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_BeamGas
    else:
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_ATLxk

    kwargs.setdefault("pTmin", flags.InDet.Tracking.ActivePass.minPT )
    kwargs.setdefault("maxdImpact", flags.InDet.Tracking.ActivePass.maxPrimaryImpact )
    kwargs.setdefault("maxZ", flags.InDet.Tracking.ActivePass.maxZImpact )
    kwargs.setdefault("minZ", -flags.InDet.Tracking.ActivePass.maxZImpact )
    kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel)
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

    if not flags.Beam.Type == 'cosmics':
        kwargs.setdefault("maxRadius1", 0.75*flags.InDet.Tracking.ActivePass.radMax)
        kwargs.setdefault("maxRadius2", flags.InDet.Tracking.ActivePass.radMax)
        kwargs.setdefault("maxRadius3", flags.InDet.Tracking.ActivePass.radMax)

    if flags.InDet.Tracking.ActivePass.isLowPt:
        kwargs.setdefault("pTmax", flags.InDet.Tracking.ActivePass.maxPT)
        kwargs.setdefault("mindRadius", 4.0)

    if flags.InDet.Tracking.ActivePass.extension == "R3LargeD0":
        kwargs.setdefault("optimisePhiBinning", False)
        kwargs.setdefault("usePixel", False)
        kwargs.setdefault("etaMax", flags.InDet.Tracking.ActivePass.maxEta)
        kwargs.setdefault("maxSeedsForSpacePointStrips", flags.InDet.Tracking.ActivePass.maxSeedsPerSP_Strips)
        kwargs.setdefault("alwaysKeepConfirmedStripSeeds", flags.InDet.Tracking.ActivePass.keepAllConfirmedStripSeeds)
        kwargs.setdefault("maxdRadius", 150)
        kwargs.setdefault("seedScoreBonusConfirmationSeed", -2000)
    elif flags.InDet.Tracking.ActivePass.extension == "Forward":
        kwargs.setdefault("checkEta", True)
        kwargs.setdefault("etaMin", flags.InDet.Tracking.ActivePass.minEta)
        kwargs.setdefault("etaMax", flags.InDet.Tracking.ActivePass.maxEta)
        kwargs.setdefault("RapidityCut", flags.InDet.Tracking.ActivePass.maxEta)
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

def SiDetElementsRoadMaker_xkCfg(flags, name="InDetSiRoadMaker", **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- SCT and Pixel detector elements road builder
    #
    InDetPatternPropagator = acc.getPrimaryAndMerge(TC.InDetPatternPropagatorCfg())

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel )
    kwargs.setdefault("PixManagerLocation", 'Pixel')
    kwargs.setdefault("useSCT", flags.InDet.Tracking.ActivePass.useSCT)
    kwargs.setdefault("SCTManagerLocation", 'SCT')
    kwargs.setdefault("RoadWidth", flags.InDet.Tracking.ActivePass.roadWidth)

    InDetSiDetElementsRoadMaker = CompFactory.InDet.SiDetElementsRoadMaker_xk(name = name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(InDetSiDetElementsRoadMaker)
    return acc

def SiCombinatorialTrackFinder_xkCfg(flags, name="InDetSiComTrackFinder", **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- Local track finding using sdCaloSeededSSSpace point seed
    #
    if flags.InDet.Tracking.doDBMstandalone:
        RotCreator = acc.popToolsAndMerge(TC.InDetRotCreatorDBMCfg(flags))
        kwargs.setdefault("useSCT", False)
        kwargs.setdefault("MagneticFieldMode", "NoField")
        kwargs.setdefault("TrackQualityCut", 9.3)
    else:
        RotCreator = acc.popToolsAndMerge(TC.InDetRotCreatorDigitalCfg(flags))
        kwargs.setdefault("useSCT", flags.Detector.EnableSCT)

    acc.addPublicTool(RotCreator)
    kwargs.setdefault("RIOonTrackTool", RotCreator)

    kwargs.setdefault("PropagatorTool", acc.getPrimaryAndMerge(TC.InDetPatternPropagatorCfg()))
    kwargs.setdefault("UpdatorTool", acc.getPrimaryAndMerge(TC.InDetPatternUpdatorCfg()))

    from  InDetConfig.InDetRecToolConfig import InDetBoundaryCheckToolCfg
    kwargs.setdefault("BoundaryCheckTool", acc.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags)))
    
    kwargs.setdefault("usePixel", flags.Detector.EnablePixel)
    kwargs.setdefault("PixelClusterContainer", "PixelClusters")
    kwargs.setdefault("SCT_ClusterContainer", "SCT_Clusters")

    if flags.Detector.EnablePixel:
        from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
        kwargs.setdefault("PixelSummaryTool", acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags)))
    else:
        kwargs.setdefault("PixelSummaryTool", "")

    if flags.Detector.EnableSCT:
        from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
        kwargs.setdefault("SctSummaryTool", acc.popToolsAndMerge(SCT_ConditionsSummaryToolCfg(flags)))
    else:
        kwargs.setdefault("SctSummaryTool", "")

    track_finder = CompFactory.InDet.SiCombinatorialTrackFinder_xk(name = name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(track_finder)
    return acc

def SiTrackMaker_xkCfg(flags, name="InDetSiTrackMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.ActivePass.usePixel:
        acc.addCondAlgo(CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk(name="InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                              ReadKey="PixelDetectorElementCollection",
                                                                              WriteKey="PixelDetElementBoundaryLinks_xk"))
    if flags.InDet.Tracking.ActivePass.useSCT:
        acc.addCondAlgo(CompFactory.InDet.SiDetElementsRoadCondAlg_xk(name="InDet__SiDetElementsRoadCondAlg_xk"))

        acc.addCondAlgo(CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk(name="InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                              ReadKey="SCT_DetectorElementCollection",
                                                                              WriteKey="SCT_DetElementBoundaryLinks_xk"))

    kwargs.setdefault("useSCT", flags.InDet.Tracking.ActivePass.useSCT)
    kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel)
    kwargs.setdefault("RoadTool", acc.popToolsAndMerge(SiDetElementsRoadMaker_xkCfg(flags)))
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
    kwargs.setdefault("CosmicTrack", flags.Beam.Type == 'cosmics')
    kwargs.setdefault("Xi2maxMultiTracks", flags.InDet.Tracking.ActivePass.Xi2max)
    kwargs.setdefault("useSSSseedsFilter", True)
    kwargs.setdefault("doMultiTracksProd", True)
    kwargs.setdefault("useBremModel", flags.InDet.Tracking.doBremRecovery and flags.Detector.EnableCalo
                      and (flags.InDet.Tracking.ActivePass.extension=="" or flags.InDet.Tracking.ActivePass.extension=="BLS") )
    kwargs.setdefault("doCaloSeededBrem", flags.InDet.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo)
    if kwargs["useBremModel"] and kwargs["doCaloSeededBrem"]:
        from InDetConfig.InDetRecCaloSeededROISelectionConfig import CaloClusterROI_SelectorCfg
        acc.merge(CaloClusterROI_SelectorCfg(flags))
    kwargs.setdefault("doHadCaloSeedSSS", flags.InDet.Tracking.doHadCaloSeededSSS and flags.Detector.EnableCalo)
    if kwargs["doHadCaloSeedSSS"]:
        from InDetConfig.InDetRecCaloSeededROISelectionConfig import HadCaloClusterROI_SelectorCfg
        acc.merge(HadCaloClusterROI_SelectorCfg(flags))
    kwargs.setdefault("phiWidth", flags.InDet.Tracking.ActivePass.phiWidthBrem)
    kwargs.setdefault("etaWidth", flags.InDet.Tracking.ActivePass.etaWidthBrem)
    kwargs.setdefault("InputClusterContainerName", 'InDetCaloClusterROIs') # InDetKeys.CaloClusterROIContainer()
    kwargs.setdefault("InputHadClusterContainerName", 'InDetHadCaloClusterROIs') # InDetKeys.HadCaloClusterROIContainer()
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

    elif flags.Beam.Type == 'cosmics':
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

    #
    # --- Setup Track finder using space points seeds
    #
    kwargs.setdefault("TrackTool", acc.popToolsAndMerge(SiTrackMaker_xkCfg(flags, InputCollections = InputCollections)))
    kwargs.setdefault("TrackSummaryTool", acc.getPrimaryAndMerge(TC.InDetTrackSummaryToolNoHoleSearchCfg(flags)))
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

def InDetAmbiTrackSelectionToolCfg(flags, name="InDetAmbiTrackSelectionTool", **kwargs) :
    acc = ComponentAccumulator()

    # ------------------------------------------------------------
    #
    # ---------- Ambiguity solving
    #
    # ------------------------------------------------------------

    #
    # --- load InnerDetector TrackSelectionTool
    #
    if flags.InDet.Tracking.ActivePass.useTIDE_Ambi:
        AmbiTrackSelectionTool = CompFactory.InDet.InDetDenseEnvAmbiTrackSelectionTool
    else:
        AmbiTrackSelectionTool = CompFactory.InDet.InDetAmbiTrackSelectionTool

    if 'UseParameterization' in kwargs and kwargs.get('UseParameterization',False) :
        InDetTRTDriftCircleCut = acc.popToolsAndMerge(TC.InDetTRTDriftCircleCutForPatternRecoCfg(flags))
        kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCut)

    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags))

    kwargs.setdefault("AssociationTool" , InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("minHits"         , flags.InDet.Tracking.ActivePass.minClusters)
    kwargs.setdefault("minNotShared"    , flags.InDet.Tracking.ActivePass.minSiNotShared)
    kwargs.setdefault("maxShared"       , flags.InDet.Tracking.ActivePass.maxShared)
    kwargs.setdefault("minTRTHits"      , 0) # used for Si only tracking !!!
    kwargs.setdefault("UseParameterization" , False)
    kwargs.setdefault("Cosmics"             , flags.Beam.Type == 'cosmics' and flags.InDet.Tracking.ActivePass.extension != "DBM")
    kwargs.setdefault("doPixelSplitting"    , flags.InDet.Tracking.doPixelClusterSplitting and flags.InDet.Tracking.ActivePass.extension != "DBM")

    if flags.InDet.Tracking.ActivePass.useTIDE_Ambi:
        kwargs.setdefault("sharedProbCut"             , flags.InDet.Tracking.pixelClusterSplitProb1)
        kwargs.setdefault("sharedProbCut2"            , flags.InDet.Tracking.pixelClusterSplitProb2)
        kwargs.setdefault("minSiHitsToAllowSplitting" , 8 if flags.GeoModel.Run == "Run1" else 9)
        kwargs.setdefault("minUniqueSCTHits"          , 4)
        kwargs.setdefault("minTrackChi2ForSharedHits" , 3)
        kwargs.setdefault("minPtSplit"                , 1000)       #Only allow split clusters on track withe pt greater than this MeV
        kwargs.setdefault("maxSharedModulesInROI"     , 3)     #Maximum number of shared modules for tracks in ROI
        kwargs.setdefault("minNotSharedInROI"         , 2)     #Minimum number of unique modules for tracks in ROI
        kwargs.setdefault("minSiHitsToAllowSplittingInROI" , 8)  #Minimum number of Si hits to allow splittings for tracks in ROI
        kwargs.setdefault("phiWidth"                  , 0.05)     #Split cluster ROI size
        kwargs.setdefault("etaWidth"                  , 0.05)     #Split cluster ROI size
        kwargs.setdefault("doEmCaloSeed"              , flags.InDet.Tracking.doCaloSeededAmbi and flags.Detector.EnableCalo)   #Only split in cluster in region of interest
        kwargs.setdefault("InputEmClusterContainerName", 'InDetCaloClusterROIs')
        if flags.InDet.Tracking.doCaloSeededAmbi:
            from InDetConfig.InDetRecCaloSeededROISelectionConfig import CaloClusterROI_SelectorCfg
            acc.merge(CaloClusterROI_SelectorCfg(flags))
        kwargs.setdefault("doHadCaloSeed"             , flags.InDet.Tracking.doCaloSeededAmbi)   #Do special cuts in region of interest
        kwargs.setdefault("InputHadClusterContainerName", "InDetHadCaloClusterROIs" + "Bjet")
        if flags.InDet.Tracking.doCaloSeededAmbi:
            from InDetConfig.InDetRecCaloSeededROISelectionConfig import HadCaloClusterROI_SelectorCfg
            acc.merge(HadCaloClusterROI_SelectorCfg(flags))
        kwargs.setdefault("minPtConv"                 , 10000)   #Only allow split clusters on track withe pt greater than this MeV
        kwargs.setdefault("minPtBjetROI"              , 10000)
        kwargs.setdefault("phiWidthEM"                , 0.05)     #Split cluster ROI size
        kwargs.setdefault("etaWidthEM"                , 0.05)     #Split cluster ROI size

    elif flags.InDet.Tracking.ActivePass.extension == "DBM":
        kwargs.setdefault("maxShared"             , 1000)
        kwargs.setdefault("maxTracksPerSharedPRD" , 2)
        kwargs.setdefault("minHits"               , 0)
        kwargs.setdefault("minNotShared"          , 0)
        kwargs.setdefault("minScoreShareTracks"   , 0.0)
        kwargs.setdefault("minTRTHits"            , 0)
        kwargs.setdefault("sharedProbCut"         , 0.1)
    else:
        kwargs.setdefault("sharedProbCut", 0.10)

    InDetAmbiTrackSelectionTool = AmbiTrackSelectionTool(name = name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(InDetAmbiTrackSelectionTool)
    return acc


def DenseEnvironmentsAmbiguityScoreProcessorToolCfg(flags, name="InDetAmbiguityScoreProcessor", ClusterSplitProbContainer="", **kwargs):
    acc = ComponentAccumulator()
    # --- set up different Scoring Tool for collisions and cosmics
    if flags.Beam.Type == "cosmics" and flags.InDet.Tracking.ActivePass.extension != "DBM":
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetCosmicsScoringToolCfg(flags))
    elif flags.InDet.Tracking.ActivePass.extension == "R3LargeD0" and flags.InDet.Tracking.nnCutLargeD0Threshold > 0:
        # Set up NN config
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetNNScoringToolSiCfg(flags))
    else:
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetAmbiScoringToolSiCfg(flags))
    kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)

    from InDetConfig.SiliconPreProcessing import NnPixelClusterSplitProbToolCfg
    kwargs.setdefault("SplitProbTool", acc.popToolsAndMerge(NnPixelClusterSplitProbToolCfg(flags)) if flags.InDet.Tracking.doPixelClusterSplitting else "")

    kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags)))
    kwargs.setdefault("AssociationToolNotGanged", acc.popToolsAndMerge(TC.PRDtoTrackMapToolCfg()))
    kwargs.setdefault("AssociationMapName", f"PRDToTrackMap{flags.InDet.Tracking.ActivePass.extension}")

    if flags.InDet.Tracking.ActivePass.useTIDE_Ambi:
        kwargs.setdefault("sharedProbCut", flags.InDet.Tracking.pixelClusterSplitProb1)
        kwargs.setdefault("sharedProbCut2", flags.InDet.Tracking.pixelClusterSplitProb2)
        kwargs.setdefault("SplitClusterMap_new", f"SplitClusterAmbiguityMap{flags.InDet.Tracking.ActivePass.extension}")

    kwargs.setdefault("InputClusterSplitProbabilityName", ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName", f"SplitProb{flags.InDet.Tracking.ActivePass.extension}")

    ScoreProcessorTool = CompFactory.Trk.DenseEnvironmentsAmbiguityScoreProcessorTool
    acc.setPrivateTools(ScoreProcessorTool(f"{name}{flags.InDet.Tracking.ActivePass.extension}", **kwargs))
    return acc


def DenseEnvironmentsAmbiguityProcessorToolCfg(flags, name="InDetAmbiguityProcessor", ClusterSplitProbContainer="", **kwargs):
    acc = ComponentAccumulator()
    
    # --- set up different Scoring Tool for collisions and cosmics
    if flags.Beam.Type == "cosmics" and flags.InDet.Tracking.ActivePass.extension != "DBM":
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetCosmicsScoringToolCfg(flags))
    elif flags.InDet.Tracking.ActivePass.extension == "R3LargeD0" and flags.InDet.Tracking.nnCutLargeD0Threshold > 0:
        # Set up NN config
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetNNScoringToolSiCfg(flags))
    else:
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetAmbiScoringToolSiCfg(flags))
    kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)

    fitter_args = {}
    fitter_args.setdefault("nameSuffix", 'Ambi'+flags.InDet.Tracking.ActivePass.extension)
    fitter_args.setdefault("ClusterSplitProbabilityName", 'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.ActivePass.extension)

    if flags.InDet.Tracking.holeSearchInGX2Fit:
        fitter_args.setdefault("DoHoleSearch", True)
        from  InDetConfig.InDetRecToolConfig import InDetBoundaryCheckToolCfg
        InDetBoundaryCheckTool = acc.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags))
        fitter_args.setdefault("BoundaryCheckTool", InDetBoundaryCheckTool)

    fitter_list=[]

    if flags.InDet.Tracking.ActivePass.isLowPt:
        InDetTrackFitterLowPt = acc.popToolsAndMerge(TC.InDetTrackFitterLowPt(flags, name='InDetTrackFitterLowPt'+flags.InDet.Tracking.ActivePass.extension, **fitter_args))
        fitter_list.append(InDetTrackFitterLowPt)
    else:
        InDetTrackFitterAmbi = acc.popToolsAndMerge(TC.InDetTrackFitterCfg(flags, name='InDetTrackFitterAmbi'+flags.InDet.Tracking.ActivePass.extension, **fitter_args))
        fitter_list.append(InDetTrackFitterAmbi)

    if flags.InDet.Tracking.doRefitInvalidCov:
        if not flags.InDet.Tracking.ActivePass.extension=="":
            KalmanFitter = acc.popToolsAndMerge(TC.KalmanFitterCfg(flags, name='KalmanFitter'+flags.InDet.Tracking.ActivePass.extension))
            fitter_list.append(KalmanFitter)

            ReferenceKalmanFitter = acc.popToolsAndMerge(TC.ReferenceKalmanFitterCfg(flags, name='ReferenceKalmanFitter'+flags.InDet.Tracking.ActivePass.extension))
            fitter_list.append(ReferenceKalmanFitter)
        else:
            KalmanFitter = acc.popToolsAndMerge(TC.KalmanFitterCfg(flags))
            fitter_list.append(KalmanFitter)

            ReferenceKalmanFitter = acc.popToolsAndMerge(TC.ReferenceKalmanFitterCfg(flags))
            fitter_list.append(ReferenceKalmanFitter)

    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags))

    ambi_track_summary_tool = acc.getPrimaryAndMerge(TC.InDetTrackSummaryToolCfg( flags,
                                                                                  namePrefix                  = 'InDetAmbiguityProcessorSplitProb',
                                                                                  nameSuffix                  = flags.InDet.Tracking.ActivePass.extension,
                                                                                  ClusterSplitProbabilityName = 'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.ActivePass.extension))

    InDetAmbiTrackSelectionTool = acc.popToolsAndMerge(InDetAmbiTrackSelectionToolCfg(flags))

    kwargs.setdefault("Fitter", fitter_list)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationMapName", 'PRDToTrackMap'+flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("TrackSummaryTool", ambi_track_summary_tool)
    kwargs.setdefault("SelectionTool", InDetAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName", 'SplitProb'+flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("tryBremFit", flags.InDet.Tracking.doBremRecovery and (flags.InDet.Tracking.ActivePass.extension=="" or flags.InDet.Tracking.ActivePass.extension=="BLS") )
    kwargs.setdefault("caloSeededBrem", flags.InDet.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo)
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.ActivePass.minPTBrem)
    kwargs.setdefault("RefitPrds", True)
    kwargs.setdefault("KeepHolesFromBeforeRefit", False)

    InDetAmbiguityProcessor = CompFactory.Trk.DenseEnvironmentsAmbiguityProcessorTool(name=name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(InDetAmbiguityProcessor)
    return acc

def SimpleAmbiguityProcessorToolCfg(flags, name = "InDetAmbiguityProcessor", ClusterSplitProbContainer='', **kwargs) :
    acc = ComponentAccumulator()

    #
    # --- set up different Scoring Tool for collisions and cosmics
    #
    if flags.Beam.Type == 'cosmics' and flags.InDet.Tracking.ActivePass.extension != "DBM":
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetCosmicsScoringToolCfg(flags))
    elif flags.InDet.Tracking.ActivePass.extension == "R3LargeD0" and flags.InDet.Tracking.nnCutLargeD0Threshold > 0:
        # Set up NN config
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetNNScoringToolSiCfg(flags))
    else:
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetAmbiScoringToolSiCfg(flags))

    if flags.InDet.Tracking.ActivePass.isLowPt:
        InDetTrackFitter = acc.popToolsAndMerge(TC.InDetTrackFitterLowPt(flags))
    else:
        InDetTrackFitter = acc.popToolsAndMerge(TC.InDetTrackFitterCfg(flags))

    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags))

    ambi_track_summary_tool = acc.getPrimaryAndMerge(TC.InDetTrackSummaryToolCfg( flags,
                                                                                namePrefix                  = 'InDetAmbiguityProcessorSplitProb',
                                                                                nameSuffix                  = flags.InDet.Tracking.ActivePass.extension,
                                                                                ClusterSplitProbabilityName = 'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.ActivePass.extension))

    InDetAmbiTrackSelectionTool = acc.popToolsAndMerge(InDetAmbiTrackSelectionToolCfg(flags))

    kwargs.setdefault("Fitter", InDetTrackFitter)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("TrackSummaryTool", ambi_track_summary_tool)
    kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)
    kwargs.setdefault("SelectionTool", InDetAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName", ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("tryBremFit", flags.InDet.Tracking.doBremRecovery and (flags.InDet.Tracking.ActivePass.extension=="" or flags.InDet.Tracking.ActivePass.extension=="BLS"))
    kwargs.setdefault("caloSeededBrem", flags.InDet.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo)
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.ActivePass.minPTBrem)
    kwargs.setdefault("RefitPrds", True)
    kwargs.setdefault("MatEffects", flags.InDet.Tracking.materialInteractionsType if flags.InDet.Tracking.materialInteractions else 0)

    if flags.InDet.Tracking.ActivePass.extension == "Pixel" or flags.InDet.Tracking.ActivePass.extension == "DBM":
        kwargs.setdefault("SuppressHoleSearch", True)

    InDetAmbiguityProcessor = CompFactory.Trk.SimpleAmbiguityProcessorTool(name=name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(InDetAmbiguityProcessor)
    return acc

def TrkAmbiguityScoreCfg(flags, name="InDetAmbiguityScore", SiSPSeededTrackCollectionKey = None, **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- set input and output collection
    #
    InputTrackCollection = SiSPSeededTrackCollectionKey

    if flags.InDet.Tracking.ActivePass.useTIDE_Ambi:
        InDetAmbiguityScoreProcessor = acc.popToolsAndMerge(DenseEnvironmentsAmbiguityScoreProcessorToolCfg(flags))
    else:
        InDetAmbiguityScoreProcessor = None

    #
    # --- configure Ambiguity (score) solver
    #
    kwargs.setdefault("TrackInput" , [ InputTrackCollection ])
    kwargs.setdefault("TrackOutput", 'ScoredMap'+'InDetAmbiguityScore'+ flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("AmbiguityScoreProcessor" ,  InDetAmbiguityScoreProcessor ) ## TODO: check the case when it is None object

    InDetAmbiguityScore = CompFactory.Trk.TrkAmbiguityScore(name = name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.addEventAlgo(InDetAmbiguityScore)
    return acc

def TrkAmbiguitySolverCfg(flags, name="InDetAmbiguitySolver", ResolvedTrackCollectionKey = None, ClusterSplitProbContainer='', **kwargs):
    acc = ComponentAccumulator()
    SiTrackCollection = ResolvedTrackCollectionKey

    if flags.InDet.Tracking.ActivePass.useTIDE_Ambi:
        InDetAmbiguityProcessor = acc.popToolsAndMerge(DenseEnvironmentsAmbiguityProcessorToolCfg(flags, ClusterSplitProbContainer=ClusterSplitProbContainer))
    else:
        InDetAmbiguityProcessor = acc.popToolsAndMerge(SimpleAmbiguityProcessorToolCfg(flags, ClusterSplitProbContainer=ClusterSplitProbContainer))

    #
    # --- configure Ambiguity solver
    #
    kwargs.setdefault("TrackInput", 'ScoredMap'+'InDetAmbiguityScore'+ flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("TrackOutput", SiTrackCollection)
    kwargs.setdefault( "AmbiguityProcessor", InDetAmbiguityProcessor)

    InDetAmbiguitySolver = CompFactory.Trk.TrkAmbiguitySolver(name = name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.addEventAlgo(InDetAmbiguitySolver )
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
        acc.merge(TC.InDetTrackPRD_AssociationCfg(flags,namePrefix = 'InDet',
                                                  nameSuffix = flags.InDet.Tracking.ActivePass.extension,
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

    acc.merge(TrkAmbiguityScoreCfg( flags,
                                    SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey))

    acc.merge(TrkAmbiguitySolverCfg(flags,
                                    ResolvedTrackCollectionKey = ResolvedTrackCollectionKey,
                                    ClusterSplitProbContainer = ClusterSplitProbContainer))

    return acc


