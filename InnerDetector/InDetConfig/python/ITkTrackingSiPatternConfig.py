# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from InDetConfig.ITkRecToolConfig import ITkBoundaryCheckToolCfg, ITkPatternPropagatorCfg, ITkPatternUpdatorCfg
import InDetConfig.ITkTrackingCommonConfig as TC


def ITkSiSpacePointsSeedMakerCfg(flags, name="ITkSpSeedsMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- decide if use the association tool
    #
    if (len(InputCollections) > 0) and \
        (flags.ITk.Tracking.extension == "LargeD0" \
         or flags.ITk.Tracking.extension == "ConversionFinding"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if usePrdAssociationTool:
        prefix = 'ITk'
        suffix = flags.ITk.Tracking.extension
    #
    # --- Space points seeds maker, use different ones for cosmics and collisions
    #
    SiSpacePointsSeedMaker = CompFactory.ITk.SiSpacePointsSeedMaker

    kwargs.setdefault("pTmin", flags.ITk.Tracking.minPTSeed )
    kwargs.setdefault("maxdImpact", flags.ITk.Tracking.maxPrimaryImpactSeed )
    kwargs.setdefault("maxZ", flags.ITk.Tracking.maxZImpactSeed )
    kwargs.setdefault("minZ", -flags.ITk.Tracking.maxZImpactSeed )
    kwargs.setdefault("usePixel", flags.ITk.Tracking.useITkPixel)
    kwargs.setdefault("SpacePointsPixelName", 'ITkPixelSpacePoints')
    kwargs.setdefault("useStrip", flags.ITk.Tracking.useITkStrip and flags.ITk.Tracking.useITkStripSeeding )
    kwargs.setdefault("SpacePointsStripName", 'ITkStripSpacePoints')
    kwargs.setdefault("useOverlapSpCollection", flags.ITk.Tracking.useITkStrip and flags.ITk.Tracking.useITkStripSeeding )
    kwargs.setdefault("SpacePointsOverlapName", 'ITkOverlapSpacePoints')
    kwargs.setdefault("radMax", flags.ITk.Tracking.radMax)
    kwargs.setdefault("etaMax", flags.ITk.Tracking.maxEta )

    if usePrdAssociationTool:
        # not all classes have that property !!!
        kwargs.setdefault("PRDtoTrackMap", prefix+'PRDtoTrackMap'+ suffix)
    if not flags.Beam.Type == 'cosmics':
        kwargs.setdefault("maxRadius1", 0.75*flags.ITk.Tracking.radMax)
        kwargs.setdefault("maxRadius2", flags.ITk.Tracking.radMax)
        kwargs.setdefault("maxRadius3", flags.ITk.Tracking.radMax)

    if flags.ITk.Tracking.extension == "LargeD0":
        kwargs.setdefault("maxSeedsForSpacePoint", 5)
        kwargs.setdefault("isLRT", True)
        kwargs.setdefault("maxZPPP", flags.ITk.Tracking.maxZSpacePointsPPPSeeds)
        kwargs.setdefault("maxZSSS", flags.ITk.Tracking.maxZSpacePointsSSSSeeds)

    if flags.ITk.doFastTracking :
        kwargs.setdefault("useFastTracking", True)
        kwargs.setdefault("maxSeedsForSpacePoint", 3)
        kwargs.setdefault("useStrip", False)
        if flags.ITk.Tracking.extension == "LargeD0":
            kwargs.setdefault("usePixel", False)

    ITkSiSpacePointsSeedMaker = SiSpacePointsSeedMaker (name = name+flags.ITk.Tracking.extension, **kwargs)

    acc.setPrivateTools(ITkSiSpacePointsSeedMaker)
    return acc

def ITkSiZvertexMaker_xkCfg(flags, name="ITkZvertexMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("Zmax", flags.ITk.Tracking.maxZImpact)
    kwargs.setdefault("Zmin", -flags.ITk.Tracking.maxZImpact)
    kwargs.setdefault("minRatio", 0.17)

    ITkSiSpacePointsSeedMaker = acc.popToolsAndMerge(ITkSiSpacePointsSeedMakerCfg(flags,
                                                                                  InputCollections = InputCollections ))

    kwargs.setdefault("SeedMakerTool", ITkSiSpacePointsSeedMaker)
    if flags.ITk.doHeavyIon:
        kwargs.setdefault("HistSize", 2000)
        kwargs.setdefault("minContent", 30)

    ITkZvertexMaker = CompFactory.InDet.SiZvertexMaker_xk(name = name+flags.ITk.Tracking.extension, **kwargs)
    acc.setPrivateTools(ITkZvertexMaker)
    return acc

def ITkSiDetElementsRoadMaker_xkCfg(flags, name="ITkSiRoadMaker", **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- SCT and Pixel detector elements road builder
    #
    ITkPatternPropagator = acc.getPrimaryAndMerge(ITkPatternPropagatorCfg(flags))

    kwargs.setdefault("PropagatorTool", ITkPatternPropagator)
    kwargs.setdefault("usePixel", flags.ITk.Tracking.useITkPixel )
    kwargs.setdefault("PixManagerLocation", 'ITkPixel')
    kwargs.setdefault("useSCT", flags.ITk.Tracking.useITkStrip)
    kwargs.setdefault("SCTManagerLocation", 'ITkStrip')
    kwargs.setdefault("RoadWidth", flags.ITk.Tracking.roadWidth)

    ITkSiDetElementsRoadMaker = CompFactory.InDet.SiDetElementsRoadMaker_xk(name = name+flags.ITk.Tracking.extension, **kwargs)
    acc.setPrivateTools(ITkSiDetElementsRoadMaker)
    return acc

def ITkSiCombinatorialTrackFinder_xkCfg(flags, name="ITkSiComTrackFinder", **kwargs) :
    acc = ComponentAccumulator()

    #
    # --- Local track finding using sdCaloSeededSSSpace point seed
    #
    # @TODO ensure that PRD association map is used if usePrdAssociationTool is set
    ITkRotCreatorDigital = acc.getPrimaryAndMerge(TC.ITkRotCreatorDigitalCfg(flags))
    ITkPatternPropagator = acc.getPrimaryAndMerge(ITkPatternPropagatorCfg(flags))
    ITkPatternUpdator = acc.popToolsAndMerge(ITkPatternUpdatorCfg(flags))

    ITkBoundaryCheckTool = acc.popToolsAndMerge(ITkBoundaryCheckToolCfg(flags))

    kwargs.setdefault("PropagatorTool", ITkPatternPropagator)
    kwargs.setdefault("UpdatorTool", ITkPatternUpdator)
    kwargs.setdefault("BoundaryCheckTool", ITkBoundaryCheckTool)
    kwargs.setdefault("RIOonTrackTool", ITkRotCreatorDigital)
    kwargs.setdefault("usePixel", flags.Detector.EnableITkPixel)
    kwargs.setdefault("useSCT", flags.Detector.EnableITkStrip)
    kwargs.setdefault("PixelClusterContainer", 'ITkPixelClusters')
    kwargs.setdefault("SCT_ClusterContainer", 'ITkStripClusters')
    kwargs.setdefault("PixelDetElementBoundaryLinks_xk", "ITkPixelDetElementBoundaryLinks_xk")
    kwargs.setdefault("SCT_DetElementBoundaryLinks_xk", "ITkStripDetElementBoundaryLinks_xk")
    kwargs.setdefault("SCTDetEleCollKey","ITkStripDetectorElementCollection")
    kwargs.setdefault("ITkGeometry", True)
    kwargs.setdefault("doFastTracking", flags.ITk.doFastTracking)

    if flags.Detector.EnableITkStrip:
        from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ITkStripConditionsSummaryToolCfg
        ITkStripConditionsSummaryTool = acc.popToolsAndMerge(ITkStripConditionsSummaryToolCfg(flags))
        kwargs.setdefault("SctSummaryTool", ITkStripConditionsSummaryTool)
    else:
        kwargs.setdefault("SctSummaryTool", None)

    if flags.Detector.EnableITkPixel:
        from PixelConditionsTools.ITkPixelConditionsSummaryConfig import ITkPixelConditionsSummaryCfg
        kwargs.setdefault("PixelSummaryTool", acc.popToolsAndMerge(ITkPixelConditionsSummaryCfg(flags)) )
    else:
        kwargs.setdefault("PixelSummaryTool", None)

    ITkSiComTrackFinder = CompFactory.InDet.SiCombinatorialTrackFinder_xk(name = name+flags.ITk.Tracking.extension, **kwargs)
    acc.setPrivateTools(ITkSiComTrackFinder)
    return acc

def ITkSiTrackMaker_xkCfg(flags, name="ITkSiTrackMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()
    useBremMode = flags.ITk.Tracking.extension == "" #Disabled for second passes in reco
    ITkSiDetElementsRoadMaker = acc.popToolsAndMerge(ITkSiDetElementsRoadMaker_xkCfg(flags))

    if flags.ITk.Tracking.useITkPixel:
        acc.addCondAlgo( CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk( name = "ITkSiDetElementBoundaryLinksPixelCondAlg",
                                                                                ReadKey  = "ITkPixelDetectorElementCollection",
                                                                                WriteKey = "ITkPixelDetElementBoundaryLinks_xk",
                                                                                ITkGeometry = True ) )
    if flags.ITk.Tracking.useITkStrip:
        acc.addCondAlgo(CompFactory.InDet.SiDetElementsRoadCondAlg_xk(name = "ITkSiDetElementsRoadCondAlg_xk",
                                                                      PixelDetEleCollKey = "ITkPixelDetectorElementCollection",
                                                                      SCTDetEleCollKey = "ITkStripDetectorElementCollection"))

        acc.addCondAlgo( CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk( name = "ITkSiDetElementBoundaryLinksSCTCondAlg",
                                                                                ReadKey  = "ITkStripDetectorElementCollection",
                                                                                WriteKey = "ITkStripDetElementBoundaryLinks_xk",
                                                                                ITkGeometry = True ) )

    track_finder = acc.popToolsAndMerge(ITkSiCombinatorialTrackFinder_xkCfg(flags))

    #
    # --- decide if use the association tool
    #
    if (len(InputCollections) > 0) and (flags.ITk.Tracking.extension == "LargeD0" or flags.ITk.Tracking.extension == "ConversionFinding"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False

    kwargs.setdefault("useSCT", flags.ITk.Tracking.useITkStrip)
    kwargs.setdefault("usePixel", flags.ITk.Tracking.useITkPixel)
    kwargs.setdefault("RoadTool", ITkSiDetElementsRoadMaker)
    kwargs.setdefault("CombinatorialTrackFinder", track_finder)
    kwargs.setdefault("etaBins", flags.ITk.Tracking.etaBins)
    kwargs.setdefault("pTBins", flags.ITk.Tracking.minPT)
    kwargs.setdefault("pTmin", flags.ITk.Tracking.minPT[0])
    kwargs.setdefault("pTminBrem", flags.ITk.Tracking.minPTBrem[0])
    kwargs.setdefault("pTminSSS", flags.ITk.Tracking.pT_SSScut)
    kwargs.setdefault("nClustersMin", min(flags.ITk.Tracking.minClusters))
    kwargs.setdefault("nHolesMax", flags.ITk.Tracking.nHolesMax[0])
    kwargs.setdefault("nHolesGapMax", flags.ITk.Tracking.nHolesGapMax[0])
    kwargs.setdefault("SeedsFilterLevel", flags.ITk.Tracking.seedFilterLevel)
    kwargs.setdefault("Xi2max", flags.ITk.Tracking.Xi2max[0])
    kwargs.setdefault("Xi2maxNoAdd", flags.ITk.Tracking.Xi2maxNoAdd[0])
    kwargs.setdefault("nWeightedClustersMin", flags.ITk.Tracking.nWeightedClustersMin[0])
    kwargs.setdefault("CosmicTrack", flags.Beam.Type == 'cosmics')
    kwargs.setdefault("Xi2maxMultiTracks", flags.ITk.Tracking.Xi2max[0])
    kwargs.setdefault("useSSSseedsFilter", flags.ITk.doSSSfilter)
    kwargs.setdefault("doMultiTracksProd", True)
    kwargs.setdefault("useBremModel", flags.ITk.doBremRecovery and useBremMode and flags.Detector.EnableCalo) # only for NewTracking the brem is debugged !!!
    kwargs.setdefault("doCaloSeededBrem", flags.ITk.doCaloSeededBrem and flags.Detector.EnableCalo)
    kwargs.setdefault("doHadCaloSeedSSS", flags.ITk.doHadCaloSeededSSS and flags.Detector.EnableCalo)
    if kwargs["useBremModel"] and kwargs["doCaloSeededBrem"]:
        from InDetConfig.ITkRecCaloSeededROISelectionConfig import ITkCaloClusterROI_SelectorCfg
        acc.merge(ITkCaloClusterROI_SelectorCfg(flags))
    kwargs.setdefault("phiWidth", flags.ITk.Tracking.phiWidthBrem[0])
    kwargs.setdefault("etaWidth", flags.ITk.Tracking.etaWidthBrem[0])
    kwargs.setdefault("InputClusterContainerName", 'ITkCaloClusterROIs')
    kwargs.setdefault("InputHadClusterContainerName", 'ITkHadCaloClusterROIs')
    kwargs.setdefault("UseAssociationTool", usePrdAssociationTool)
    kwargs.setdefault("ITKGeometry", True)

    if flags.Beam.Type == 'cosmics':
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_Cosmic')

    elif flags.ITk.doHeavyIon:
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_HeavyIon')

    elif flags.ITk.Tracking.extension == "ConversionFinding":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_ITkConversionTracks')

    elif flags.ITk.Tracking.extension == "LargeD0":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_LargeD0')

    else:
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSPSeededFinder')

    if flags.ITk.doStoreTrackSeeds:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
        acc.addPublicTool(extrapolator)  # TODO: migrate to private?

        ITkSeedToTrackConversion = CompFactory.InDet.SeedToTrackConversionTool(name="ITkSeedToTrackConversion",
                                                                               OutputName=f"SiSPSeedSegments{flags.ITk.Tracking.extension}",
                                                                               Extrapolator=extrapolator)
        acc.setPrivateTools(ITkSeedToTrackConversion)
        kwargs.setdefault("SeedToTrackConversion", ITkSeedToTrackConversion)
        kwargs.setdefault("SeedSegmentsWrite", True)

    ITkSiTrackMaker = CompFactory.InDet.SiTrackMaker_xk(name = name+flags.ITk.Tracking.extension, **kwargs)
    acc.setPrivateTools(ITkSiTrackMaker)
    return acc

def ITkSiSPSeededTrackFinderCfg(flags, name="ITkSiSpTrackFinder", InputCollections = None, SiSPSeededTrackCollectionKey = None, **kwargs) :
    acc = ComponentAccumulator()

    # set output track collection name
    #
    SiTrackCollection = SiSPSeededTrackCollectionKey
    #
    # --- decide if use the association tool
    #
    if (len(InputCollections) > 0) and (flags.ITk.Tracking.extension == "LargeD0" or flags.ITk.Tracking.extension == "ConversionFinding"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if usePrdAssociationTool:
        prefix = 'ITk'
        suffix = flags.ITk.Tracking.extension

    ITkSiTrackMaker = acc.popToolsAndMerge(ITkSiTrackMaker_xkCfg(flags,
                                                                 InputCollections = InputCollections ))
    from TrkConfig.AtlasExtrapolatorToolsConfig import ITkPropagatorCfg
    ITkPropagator = acc.getPrimaryAndMerge(ITkPropagatorCfg(flags))
    ITkTrackSummaryToolNoHoleSearch = acc.getPrimaryAndMerge(TC.ITkTrackSummaryToolNoHoleSearchCfg(flags))
    ITkSiSpacePointsSeedMaker = acc.popToolsAndMerge(ITkSiSpacePointsSeedMakerCfg(flags,
                                                                                  InputCollections = InputCollections ))

    #
    # --- Z-coordinates primary vertices finder (only for collisions)
    #
    if flags.ITk.useZvertexTool:
        ITkZvertexMaker = acc.popToolsAndMerge(ITkSiZvertexMaker_xkCfg(flags,
                                                                    InputCollections = InputCollections))
    else:
        ITkZvertexMaker = None

    #
    # --- Setup Track finder using space points seeds
    #
    kwargs.setdefault("TrackTool", ITkSiTrackMaker)
    kwargs.setdefault("PropagatorTool", ITkPropagator)
    kwargs.setdefault("PRDtoTrackMap", prefix+'PRDtoTrackMap'+suffix \
                                            if usePrdAssociationTool else '')
    kwargs.setdefault("TrackSummaryTool", ITkTrackSummaryToolNoHoleSearch)
    kwargs.setdefault("TracksLocation", SiTrackCollection)
    kwargs.setdefault("SeedsTool", ITkSiSpacePointsSeedMaker)
    kwargs.setdefault("ZvertexTool", ITkZvertexMaker)
    kwargs.setdefault("useZvertexTool", flags.ITk.useZvertexTool)
    kwargs.setdefault("useNewStrategy", flags.ITk.useNewSiSPSeededTF)
    kwargs.setdefault("useZBoundFinding", flags.ITk.Tracking.doZBoundary)
    kwargs.setdefault("ITKGeometry", True)
    kwargs.setdefault("SpacePointsSCTName", "ITkStripSpacePoints")
    kwargs.setdefault("SpacePointsPixelName", "ITkPixelSpacePoints")

    if flags.ITk.doHeavyIon :
        kwargs.setdefault("FreeClustersCut",2) #Heavy Ion optimization from Igor

    if flags.ITk.doFastTracking :
        kwargs.setdefault("doFastTracking", True)

        if 'InDetEtaDependentCutsSvc' not in kwargs :
            acc.merge(TC.ITkEtaDependentCutsSvcCfg(flags))
            kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("ITkEtaDependentCutsSvc"+flags.ITk.Tracking.extension))

    ITkSiSPSeededTrackFinder = CompFactory.InDet.SiSPSeededTrackFinder(name = name+flags.ITk.Tracking.extension, **kwargs)
    acc.addEventAlgo(ITkSiSPSeededTrackFinder)
    return acc

def ITkSiSPSeededTrackFinderROIConvCfg(flags, name="ITkSiSpTrackFinderROIConv", InputCollections = None, SiSPSeededTrackCollectionKey = None, **kwargs) :
    from InDetConfig.ITkRecCaloSeededROISelectionConfig import ITkCaloClusterROI_SelectorCfg
    acc = ITkCaloClusterROI_SelectorCfg(flags)

    from RegionSelector.RegSelToolConfig import regSelTool_ITkStrip_Cfg
    RegSelTool_ITkStrip   = acc.popToolsAndMerge(regSelTool_ITkStrip_Cfg(flags))

    kwargs.setdefault("RegSelTool_Strip", RegSelTool_ITkStrip)
    kwargs.setdefault("useITkConvSeeded", True)
    kwargs.setdefault("InputClusterContainerName", "ITkCaloClusterROIs")

    acc.merge(ITkSiSPSeededTrackFinderCfg(flags, name = name,
                                          InputCollections = InputCollections,
                                          SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey,
                                          **kwargs))
    return acc

def ITkCopyAlgForAmbiCfg(flags, name="ITkCopyAlgForAmbi", InputTrackCollection = None, OutputTrackCollection = None, **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("CollectionName", InputTrackCollection)
    kwargs.setdefault("AliasName", OutputTrackCollection)
    ITkCopyAlgForAmbi = CompFactory.Trk.TrkCollectionAliasAlg (name = name+flags.ITk.Tracking.extension, **kwargs)
    acc.addEventAlgo(ITkCopyAlgForAmbi)
    return acc


def ITkAmbiTrackSelectionToolCfg(flags, name="ITkAmbiTrackSelectionTool", **kwargs) :
    acc = ComponentAccumulator()

    # ------------------------------------------------------------
    #
    # ---------- Ambiguity solving
    #
    # ------------------------------------------------------------

    #
    # --- load InnerDetector TrackSelectionTool
    #
    prob1 = flags.ITk.pixelClusterSplitProb1
    prob2 = flags.ITk.pixelClusterSplitProb2
    nhitsToAllowSplitting = 9

    ITkPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.ITkPRDtoTrackMapToolGangedPixelsCfg(flags))

    kwargs.setdefault("DriftCircleCutTool", None)
    kwargs.setdefault("AssociationTool" , ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("minTRTHits"      , 0) # used for Si only tracking !!!
    kwargs.setdefault("UseParameterization" , False)
    kwargs.setdefault("Cosmics"             , flags.Beam.Type == 'cosmics')
    kwargs.setdefault("doPixelSplitting"    , flags.ITk.doPixelClusterSplitting )
    kwargs.setdefault("doITk" , True)

    if flags.ITk.doTIDE_Ambi:
        kwargs.setdefault("sharedProbCut"             , prob1)
        kwargs.setdefault("sharedProbCut2"            , prob2)
        kwargs.setdefault("minSiHitsToAllowSplitting" , nhitsToAllowSplitting)
        kwargs.setdefault("minUniqueSCTHits"          , 4)
        kwargs.setdefault("minTrackChi2ForSharedHits" , 3)
        kwargs.setdefault("minPtSplit"                , flags.ITk.pixelClusterSplitMinPt)       #Only allow split clusters on track withe pt greater than this MeV
        kwargs.setdefault("maxSharedModulesInROI"     , 3)     #Maximum number of shared modules for tracks in ROI
        kwargs.setdefault("minNotSharedInROI"         , 2)     #Minimum number of unique modules for tracks in ROI
        kwargs.setdefault("minSiHitsToAllowSplittingInROI" , 8)  #Minimum number of Si hits to allow splittings for tracks in ROI
        kwargs.setdefault("phiWidth"                  , 0.05)     #Split cluster ROI size
        kwargs.setdefault("etaWidth"                  , 0.05)     #Split cluster ROI size
        kwargs.setdefault("doEmCaloSeed"              , flags.ITk.doCaloSeededAmbi)   #Only split in cluster in region of interest
        kwargs.setdefault("InputEmClusterContainerName", 'ITkCaloClusterROIs')
        if flags.Detector.EnableCalo:
            from InDetConfig.ITkRecCaloSeededROISelectionConfig import ITkCaloClusterROI_SelectorCfg
            acc.merge(ITkCaloClusterROI_SelectorCfg(flags))
        kwargs.setdefault("doHadCaloSeed"             , flags.ITk.doCaloSeededAmbi)   #Do special cuts in region of interest
        kwargs.setdefault("InputHadClusterContainerName", "ITkHadCaloClusterROIs" + "Bjet")
        if flags.Detector.EnableCalo:
            from InDetConfig.ITkRecCaloSeededROISelectionConfig import ITkHadCaloClusterROI_SelectorCfg
            acc.merge(ITkHadCaloClusterROI_SelectorCfg(flags))
        kwargs.setdefault("minPtConv"                 , 10000)   #Only allow split clusters on track withe pt greater than this MeV
        kwargs.setdefault("minPtBjetROI"              , 10000)
        kwargs.setdefault("phiWidthEM"                , 0.05)     #Split cluster ROI size
        kwargs.setdefault("etaWidthEM"                , 0.05)     #Split cluster ROI size
    else:
        kwargs.setdefault("sharedProbCut", 0.10)

    if 'InDetEtaDependentCutsSvc' not in kwargs :
        acc.merge(TC.ITkEtaDependentCutsSvcCfg(flags))
        kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("ITkEtaDependentCutsSvc"+flags.ITk.Tracking.extension))

    if flags.ITk.doTIDE_Ambi:
        AmbiTrackSelectionTool = CompFactory.InDet.InDetDenseEnvAmbiTrackSelectionTool
    else:
        AmbiTrackSelectionTool = CompFactory.InDet.InDetAmbiTrackSelectionTool

    ITkAmbiTrackSelectionTool = AmbiTrackSelectionTool(name = name+flags.ITk.Tracking.extension, **kwargs)
    acc.setPrivateTools(ITkAmbiTrackSelectionTool)
    return acc

def ITkDenseEnvironmentsAmbiguityScoreProcessorToolCfg(flags, name = "ITkAmbiguityScoreProcessor", ClusterSplitProbContainer='', **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- set up different Scoring Tool for collisions and cosmics
    #
    if flags.Beam.Type == 'cosmics':
        ITkAmbiScoringTool = acc.popToolsAndMerge(TC.ITkCosmicsScoringToolCfg(flags))
    else:
        ITkAmbiScoringTool = acc.popToolsAndMerge(TC.ITkAmbiScoringToolCfg(flags))

    from InDetConfig.ITkSiliconPreProcessing import ITkNnPixelClusterSplitProbToolCfg
    ITkNnPixelClusterSplitProbTool = acc.popToolsAndMerge(ITkNnPixelClusterSplitProbToolCfg(flags))
    ITkPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.ITkPRDtoTrackMapToolGangedPixelsCfg(flags))
    ITkPRDtoTrackMapTool = acc.popToolsAndMerge(TC.ITkPRDtoTrackMapToolCfg(flags))

    prob1 = flags.ITk.pixelClusterSplitProb1
    prob2 = flags.ITk.pixelClusterSplitProb2

    if flags.ITk.doTIDE_Ambi:
        kwargs.setdefault("sharedProbCut", prob1)
        kwargs.setdefault("sharedProbCut2", prob2)
        if flags.ITk.Tracking.extension == "":
            kwargs.setdefault("SplitClusterMap_old", "")
        elif flags.ITk.Tracking.extension == "Disappearing":
            kwargs.setdefault("SplitClusterMap_old", 'SplitClusterAmbiguityMap')
        kwargs.setdefault("SplitClusterMap_new", 'SplitClusterAmbiguityMap'+flags.ITk.Tracking.extension)

    kwargs.setdefault("ScoringTool", ITkAmbiScoringTool)
    kwargs.setdefault("SplitProbTool", ITkNnPixelClusterSplitProbTool if flags.ITk.doPixelClusterSplitting else None,)
    kwargs.setdefault("AssociationTool", ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationToolNotGanged", ITkPRDtoTrackMapTool)
    kwargs.setdefault("AssociationMapName", 'ITkPRDToTrackMap'+flags.ITk.Tracking.extension)
    kwargs.setdefault("InputClusterSplitProbabilityName", ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'SplitProb'+flags.ITk.Tracking.extension)

    # DenseEnvironmentsAmbiguityScoreProcessorTool
    ScoreProcessorTool = CompFactory.Trk.DenseEnvironmentsAmbiguityScoreProcessorTool
    ITkAmbiguityScoreProcessor = ScoreProcessorTool(name=name+flags.ITk.Tracking.extension, **kwargs)

    acc.setPrivateTools(ITkAmbiguityScoreProcessor)
    return acc

def ITkDenseEnvironmentsAmbiguityProcessorToolCfg(flags, name = "ITkAmbiguityProcessor", ClusterSplitProbContainer='', **kwargs) :
    acc = ComponentAccumulator()

    useBremMode = flags.ITk.Tracking.extension == "" #Disabled for second passes in reco

    #
    # --- set up different Scoring Tool for collisions and cosmics
    #
    if flags.Beam.Type == 'cosmics':
        ITkAmbiScoringTool = acc.popToolsAndMerge(TC.ITkCosmicsScoringToolCfg(flags))
    else:
        ITkAmbiScoringTool = acc.popToolsAndMerge(TC.ITkAmbiScoringToolCfg(flags))

    fitter_args = {}
    fitter_args.setdefault("nameSuffix", 'Ambi'+flags.ITk.Tracking.extension)
    fitter_args.setdefault("SplitClusterMapExtension", flags.ITk.Tracking.extension)
    fitter_args.setdefault("ClusterSplitProbabilityName", 'ITkAmbiguityProcessorSplitProb'+flags.ITk.Tracking.extension)

    fitter_args.setdefault("DoHoleSearch", True)

    ITkBoundaryCheckTool = acc.popToolsAndMerge(ITkBoundaryCheckToolCfg(flags))
    fitter_args.setdefault("BoundaryCheckTool", ITkBoundaryCheckTool)

    fitter_list=[]
    ITkTrackFitterAmbi = acc.getPrimaryAndMerge(TC.ITkTrackFitterCfg(flags, name='ITkTrackFitter'+'Ambi'+flags.ITk.Tracking.extension, **fitter_args))
    fitter_list.append(ITkTrackFitterAmbi)

    if flags.ITk.doRefitInvalidCov:
        if len(flags.ITk.Tracking.extension) > 0 :
            fitter_args = {}
            fitter_args.setdefault("SplitClusterMapExtension", flags.ITk.Tracking.extension)
            KalmanFitter = acc.popToolsAndMerge(TC.ITkKalmanFitterCfg(flags, name='ITkKalmanFitter'+flags.ITk.Tracking.extension, **fitter_args))
            fitter_list.append(KalmanFitter)

            ReferenceKalmanFitter = acc.popToolsAndMerge(TC.ITkReferenceKalmanFitterCfg(flags, name='ITkReferenceKalmanFitter'+flags.ITk.Tracking.extension, **fitter_args))
            fitter_list.append(ReferenceKalmanFitter)
        else:
            KalmanFitter = acc.popToolsAndMerge(TC.ITkKalmanFitterCfg(flags))
            fitter_list.append(KalmanFitter)

            ReferenceKalmanFitter = acc.popToolsAndMerge(TC.ITkReferenceKalmanFitterCfg(flags))
            fitter_list.append(ReferenceKalmanFitter)

    ITkPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.ITkPRDtoTrackMapToolGangedPixelsCfg(flags))

    ambi_track_summary_tool = acc.getPrimaryAndMerge(TC.ITkTrackSummaryToolCfg( flags,
                                                                                namePrefix                  = 'ITkAmbiguityProcessorSplitProb',
                                                                                nameSuffix                  = flags.ITk.Tracking.extension,
                                                                                ClusterSplitProbabilityName = 'ITkAmbiguityProcessorSplitProb'+flags.ITk.Tracking.extension))

    ITkAmbiTrackSelectionTool = acc.popToolsAndMerge(ITkAmbiTrackSelectionToolCfg(flags))


    kwargs.setdefault("Fitter", fitter_list)
    kwargs.setdefault("AssociationTool", ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationMapName", 'ITkPRDToTrackMap'+flags.ITk.Tracking.extension)
    kwargs.setdefault("TrackSummaryTool", ambi_track_summary_tool)
    kwargs.setdefault("ScoringTool", ITkAmbiScoringTool)
    kwargs.setdefault("SelectionTool", ITkAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName", 'SplitProb'+flags.ITk.Tracking.extension)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'ITkAmbiguityProcessorSplitProb'+flags.ITk.Tracking.extension)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("tryBremFit", flags.ITk.doBremRecovery and useBremMode)
    kwargs.setdefault("caloSeededBrem", flags.ITk.doCaloSeededBrem)
    kwargs.setdefault("pTminBrem", flags.ITk.Tracking.minPTBrem[0])
    kwargs.setdefault("RefitPrds", True)
    kwargs.setdefault("KeepHolesFromBeforeRefit", flags.ITk.useHolesFromPattern)

    # DenseEnvironmentsAmbiguityProcessorTool
    ProcessorTool = CompFactory.Trk.DenseEnvironmentsAmbiguityProcessorTool
    ITkAmbiguityProcessor = ProcessorTool(name=name+flags.ITk.Tracking.extension, **kwargs)
    acc.setPrivateTools(ITkAmbiguityProcessor)
    return acc

def ITkSimpleAmbiguityProcessorToolCfg(flags, name = "ITkAmbiguityProcessor", ClusterSplitProbContainer='', **kwargs) :
    acc = ComponentAccumulator()
    useBremMode = flags.ITk.Tracking.extension == "" #Disabled for second passes in reco

    #
    # --- set up different Scoring Tool for collisions and cosmics
    #
    if flags.Beam.Type == 'cosmics':
        ITkAmbiScoringTool = acc.popToolsAndMerge(TC.ITkCosmicsScoringToolCfg(flags))
    else:
        ITkAmbiScoringTool = acc.popToolsAndMerge(TC.ITkAmbiScoringToolCfg(flags))

    ITkTrackFitter = acc.popToolsAndMerge(TC.ITkTrackFitterCfg(flags))
    ITkPRDtoTrackMapToolGangedPixels = TC.ITkPRDtoTrackMapToolGangedPixelsCfg(flags)

    ambi_track_summary_tool = acc.getPrimaryAndMerge(TC.ITkTrackSummaryToolCfg( flags,
                                                                                namePrefix                  = 'ITkAmbiguityProcessorSplitProb',
                                                                                nameSuffix                  = flags.ITk.Tracking.extension,
                                                                                ClusterSplitProbabilityName = 'ITkAmbiguityProcessorSplitProb'+flags.ITk.Tracking.extension))

    ITkAmbiTrackSelectionTool = acc.popToolsAndMerge(ITkAmbiTrackSelectionToolCfg(flags))

    kwargs.setdefault("Fitter", ITkTrackFitter)
    kwargs.setdefault("AssociationTool", ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("TrackSummaryTool", ambi_track_summary_tool)
    kwargs.setdefault("ScoringTool", ITkAmbiScoringTool)
    kwargs.setdefault("SelectionTool", ITkAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName", ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'ITkAmbiguityProcessorSplitProb'+flags.ITk.Tracking.extension)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("tryBremFit", flags.ITk.doBremRecovery and useBremMode)
    kwargs.setdefault("caloSeededBrem", flags.ITk.doCaloSeededBrem)
    kwargs.setdefault("pTminBrem", flags.ITk.Tracking.minPTBrem[0])
    kwargs.setdefault("RefitPrds", True)

    ProcessorTool = CompFactory.Trk.SimpleAmbiguityProcessorTool

    if flags.ITk.materialInteractions:
        kwargs.setdefault("MatEffects", flags.ITk.materialInteractionsType)
    else:
        kwargs.setdefault("MatEffects", 0)

    ITkAmbiguityProcessor = ProcessorTool(name=name+flags.ITk.Tracking.extension, **kwargs)
    acc.setPrivateTools(ITkAmbiguityProcessor)
    return acc

def ITkTrkAmbiguityScoreCfg(flags, name="ITkAmbiguityScore", SiSPSeededTrackCollectionKey = None, **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- set input and output collection
    #
    InputTrackCollection = SiSPSeededTrackCollectionKey

    if flags.ITk.doTIDE_Ambi :
        ITkAmbiguityScoreProcessor = acc.popToolsAndMerge(ITkDenseEnvironmentsAmbiguityScoreProcessorToolCfg(flags))
    else:
        ITkAmbiguityScoreProcessor = None

    #
    # --- configure Ambiguity (score) solver
    #
    kwargs.setdefault("TrackInput" , [ InputTrackCollection ])
    kwargs.setdefault("TrackOutput", 'ScoredMap'+'ITkAmbiguityScore'+ flags.ITk.Tracking.extension)
    kwargs.setdefault("AmbiguityScoreProcessor" ,  ITkAmbiguityScoreProcessor ) ## TODO: check the case when it is None object

    ITkAmbiguityScore = CompFactory.Trk.TrkAmbiguityScore(name = name+flags.ITk.Tracking.extension, **kwargs)
    acc.addEventAlgo(ITkAmbiguityScore)
    return acc


def ITkTrkAmbiguitySolverCfg(flags, name="ITkAmbiguitySolver", ResolvedTrackCollectionKey = None, ClusterSplitProbContainer='', **kwargs):
    acc = ComponentAccumulator()
    SiTrackCollection = ResolvedTrackCollectionKey

    if flags.ITk.doTIDE_Ambi:
    # DenseEnvironmentsAmbiguityProcessorTool
        ITkAmbiguityProcessor = acc.popToolsAndMerge(ITkDenseEnvironmentsAmbiguityProcessorToolCfg( flags,
                                                                                                    ClusterSplitProbContainer=ClusterSplitProbContainer))
    else:
        ITkAmbiguityProcessor = acc.popToolsAndMerge(ITkSimpleAmbiguityProcessorToolCfg( flags,
                                                                                        ClusterSplitProbContainer=ClusterSplitProbContainer))

    #
    # --- configure Ambiguity solver
    #
    kwargs.setdefault("TrackInput", 'ScoredMap'+'ITkAmbiguityScore'+ flags.ITk.Tracking.extension)
    kwargs.setdefault("TrackOutput", SiTrackCollection)
    kwargs.setdefault( "AmbiguityProcessor", ITkAmbiguityProcessor)

    ITkAmbiguitySolver = CompFactory.Trk.TrkAmbiguitySolver(name = name+flags.ITk.Tracking.extension, **kwargs)
    acc.addEventAlgo(ITkAmbiguitySolver )
    return acc

# ------------------------------------------------------------
#
# ----------- Setup Si Pattern for New tracking
#
# ------------------------------------------------------------
def ITkTrackingSiPatternCfg(flags, InputCollections = None, ResolvedTrackCollectionKey = None, SiSPSeededTrackCollectionKey = None , ClusterSplitProbContainer=''):
    acc = ComponentAccumulator()
    #
    # --- decide if use the association tool
    #
    if (len(InputCollections) > 0) and (flags.ITk.Tracking.extension == "LargeD0" or flags.ITk.Tracking.extension == "ConversionFinding"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if usePrdAssociationTool:
        prefix = 'ITk'
        suffix = flags.ITk.Tracking.extension
        acc.merge(TC.ITkTrackPRD_AssociationCfg(flags,namePrefix = prefix,
                                                nameSuffix = suffix,
                                                TracksName = list(InputCollections)))

    # ------------------------------------------------------------
    #
    # ----------- SiSPSeededTrackFinder
    #
    # ------------------------------------------------------------

    SiSPSeededTrackFinderCfg = ITkSiSPSeededTrackFinderCfg
    if flags.ITk.Tracking.extension == "ConversionFinding":
        SiSPSeededTrackFinderCfg = ITkSiSPSeededTrackFinderROIConvCfg
    acc.merge(SiSPSeededTrackFinderCfg( flags,
                                        InputCollections = InputCollections,
                                        SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey))
    # ------------------------------------------------------------
    #
    # ---------- Ambiguity solving
    #
    # ------------------------------------------------------------

    if flags.ITk.doFastTracking:
        acc.merge(ITkCopyAlgForAmbiCfg( flags,
                                        InputTrackCollection = SiSPSeededTrackCollectionKey,
                                        OutputTrackCollection = ResolvedTrackCollectionKey ))

    else:
        acc.merge(ITkTrkAmbiguityScoreCfg( flags,
                                           SiSPSeededTrackCollectionKey=SiSPSeededTrackCollectionKey))

        acc.merge(ITkTrkAmbiguitySolverCfg(flags,
                                           ResolvedTrackCollectionKey=ResolvedTrackCollectionKey))

    return acc

