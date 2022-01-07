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
        (flags.ITk.Tracking.Pass.extension == "LargeD0" \
         or flags.ITk.Tracking.Pass.extension == "ConversionFinding"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if usePrdAssociationTool:
        prefix = 'ITk'
        suffix = flags.ITk.Tracking.Pass.extension
    #
    # --- Space points seeds maker, use different ones for cosmics and collisions
    #
    SiSpacePointsSeedMaker = CompFactory.ITk.SiSpacePointsSeedMaker

    kwargs.setdefault("pTmin", flags.ITk.Tracking.Pass.minPTSeed )
    kwargs.setdefault("maxdImpact", flags.ITk.Tracking.Pass.maxPrimaryImpactSeed )
    kwargs.setdefault("maxZ", flags.ITk.Tracking.Pass.maxZImpactSeed )
    kwargs.setdefault("minZ", -flags.ITk.Tracking.Pass.maxZImpactSeed )
    kwargs.setdefault("usePixel", flags.ITk.Tracking.Pass.useITkPixel)
    kwargs.setdefault("SpacePointsPixelName", 'ITkPixelSpacePoints')
    kwargs.setdefault("useStrip", flags.ITk.Tracking.Pass.useITkStrip and flags.ITk.Tracking.Pass.useITkStripSeeding )
    kwargs.setdefault("SpacePointsStripName", 'ITkStripSpacePoints')
    kwargs.setdefault("useOverlapSpCollection", flags.ITk.Tracking.Pass.useITkStrip and flags.ITk.Tracking.Pass.useITkStripSeeding )
    kwargs.setdefault("SpacePointsOverlapName", 'ITkOverlapSpacePoints')
    kwargs.setdefault("radMax", flags.ITk.Tracking.Pass.radMax)
    kwargs.setdefault("etaMax", flags.ITk.Tracking.Pass.maxEta )

    if usePrdAssociationTool:
        # not all classes have that property !!!
        kwargs.setdefault("PRDtoTrackMap", prefix+'PRDtoTrackMap'+ suffix)
    if not flags.Beam.Type == 'cosmics':
        kwargs.setdefault("maxRadius1", 0.75*flags.ITk.Tracking.Pass.radMax)
        kwargs.setdefault("maxRadius2", flags.ITk.Tracking.Pass.radMax)
        kwargs.setdefault("maxRadius3", flags.ITk.Tracking.Pass.radMax)

    if flags.ITk.Tracking.Pass.extension == "LargeD0":
        kwargs.setdefault("maxSeedsForSpacePoint", 5)
        kwargs.setdefault("isLRT", True)
        kwargs.setdefault("maxZPPP", flags.ITk.Tracking.Pass.maxZSpacePointsPPPSeeds)
        kwargs.setdefault("maxZSSS", flags.ITk.Tracking.Pass.maxZSpacePointsSSSSeeds)

    if flags.ITk.Tracking.doFastTracking :
        kwargs.setdefault("useFastTracking", True)
        kwargs.setdefault("maxSeedsForSpacePoint", 3)
        kwargs.setdefault("useStrip", False)
        if flags.ITk.Tracking.Pass.extension == "LargeD0":
            kwargs.setdefault("usePixel", False)

    ITkSiSpacePointsSeedMaker = SiSpacePointsSeedMaker (name = name+flags.ITk.Tracking.Pass.extension, **kwargs)

    acc.setPrivateTools(ITkSiSpacePointsSeedMaker)
    return acc

def ITkSiDetElementsRoadMaker_xkCfg(flags, name="ITkSiRoadMaker", **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- SCT and Pixel detector elements road builder
    #
    ITkPatternPropagator = acc.getPrimaryAndMerge(ITkPatternPropagatorCfg(flags))

    kwargs.setdefault("PropagatorTool", ITkPatternPropagator)
    kwargs.setdefault("usePixel", flags.ITk.Tracking.Pass.useITkPixel )
    kwargs.setdefault("PixManagerLocation", 'ITkPixel')
    kwargs.setdefault("useSCT", flags.ITk.Tracking.Pass.useITkStrip)
    kwargs.setdefault("SCTManagerLocation", 'ITkStrip')
    kwargs.setdefault("RoadWidth", flags.ITk.Tracking.Pass.roadWidth)

    ITkSiDetElementsRoadMaker = CompFactory.InDet.SiDetElementsRoadMaker_xk(name = name+flags.ITk.Tracking.Pass.extension, **kwargs)
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
    kwargs.setdefault("doFastTracking", flags.ITk.Tracking.doFastTracking)

    if flags.Detector.EnableITkPixel:
        from PixelConditionsTools.ITkPixelConditionsSummaryConfig import ITkPixelConditionsSummaryCfg
        kwargs.setdefault("PixelSummaryTool", acc.popToolsAndMerge(ITkPixelConditionsSummaryCfg(flags)))
    else:
        kwargs.setdefault("PixelSummaryTool", None)

    if flags.Detector.EnableITkStrip:
        from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ITkStripConditionsSummaryToolCfg
        kwargs.setdefault("SctSummaryTool", acc.popToolsAndMerge(ITkStripConditionsSummaryToolCfg(flags)))
    else:
        kwargs.setdefault("SctSummaryTool", None)

    ITkSiComTrackFinder = CompFactory.InDet.SiCombinatorialTrackFinder_xk(name = name+flags.ITk.Tracking.Pass.extension, **kwargs)
    acc.setPrivateTools(ITkSiComTrackFinder)
    return acc

def ITkSiTrackMaker_xkCfg(flags, name="ITkSiTrackMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()

    ITkSiDetElementsRoadMaker = acc.popToolsAndMerge(ITkSiDetElementsRoadMaker_xkCfg(flags))

    if flags.ITk.Tracking.Pass.useITkPixel:
        acc.addCondAlgo( CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk( name = "ITkSiDetElementBoundaryLinksPixelCondAlg",
                                                                                ReadKey  = "ITkPixelDetectorElementCollection",
                                                                                WriteKey = "ITkPixelDetElementBoundaryLinks_xk",
                                                                                ITkGeometry = True ) )
    if flags.ITk.Tracking.Pass.useITkStrip:
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
    if (len(InputCollections) > 0) and (flags.ITk.Tracking.Pass.extension == "LargeD0" or flags.ITk.Tracking.Pass.extension == "ConversionFinding"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False

    kwargs.setdefault("useSCT", flags.ITk.Tracking.Pass.useITkStrip)
    kwargs.setdefault("usePixel", flags.ITk.Tracking.Pass.useITkPixel)
    kwargs.setdefault("RoadTool", ITkSiDetElementsRoadMaker)
    kwargs.setdefault("CombinatorialTrackFinder", track_finder)
    kwargs.setdefault("etaBins", flags.ITk.Tracking.Pass.etaBins)
    kwargs.setdefault("pTBins", flags.ITk.Tracking.Pass.minPT)
    kwargs.setdefault("pTmin", flags.ITk.Tracking.Pass.minPT[0])
    kwargs.setdefault("pTminBrem", flags.ITk.Tracking.Pass.minPTBrem[0])
    kwargs.setdefault("pTminSSS", flags.ITk.Tracking.Pass.pT_SSScut)
    kwargs.setdefault("nClustersMin", min(flags.ITk.Tracking.Pass.minClusters))
    kwargs.setdefault("nHolesMax", flags.ITk.Tracking.Pass.nHolesMax[0])
    kwargs.setdefault("nHolesGapMax", flags.ITk.Tracking.Pass.nHolesGapMax[0])
    kwargs.setdefault("SeedsFilterLevel", flags.ITk.Tracking.Pass.seedFilterLevel)
    kwargs.setdefault("Xi2max", flags.ITk.Tracking.Pass.Xi2max[0])
    kwargs.setdefault("Xi2maxNoAdd", flags.ITk.Tracking.Pass.Xi2maxNoAdd[0])
    kwargs.setdefault("nWeightedClustersMin", flags.ITk.Tracking.Pass.nWeightedClustersMin[0])
    kwargs.setdefault("CosmicTrack", flags.Beam.Type == 'cosmics')
    kwargs.setdefault("Xi2maxMultiTracks", flags.ITk.Tracking.Pass.Xi2max[0])
    kwargs.setdefault("doMultiTracksProd", True)
    kwargs.setdefault("useBremModel", flags.Detector.EnableCalo and flags.ITk.Tracking.doBremRecovery and flags.ITk.Tracking.Pass.extension == "") # Disabled for second passes in reco
    kwargs.setdefault("doCaloSeededBrem", flags.ITk.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo)
    kwargs.setdefault("doHadCaloSeedSSS", flags.ITk.Tracking.doHadCaloSeededSSS and flags.Detector.EnableCalo)
    if kwargs["useBremModel"] and kwargs["doCaloSeededBrem"]:
        from InDetConfig.ITkRecCaloSeededROISelectionConfig import ITkCaloClusterROI_SelectorCfg
        acc.merge(ITkCaloClusterROI_SelectorCfg(flags))
    kwargs.setdefault("phiWidth", flags.ITk.Tracking.Pass.phiWidthBrem[0])
    kwargs.setdefault("etaWidth", flags.ITk.Tracking.Pass.etaWidthBrem[0])
    kwargs.setdefault("InputClusterContainerName", 'ITkCaloClusterROIs')
    kwargs.setdefault("InputHadClusterContainerName", 'ITkHadCaloClusterROIs')
    kwargs.setdefault("UseAssociationTool", usePrdAssociationTool)
    kwargs.setdefault("ITKGeometry", True)

    if flags.Beam.Type == 'cosmics':
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_Cosmic')

    elif flags.ITk.Tracking.Pass.extension == "ConversionFinding":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_ITkConversionTracks')

    elif flags.ITk.Tracking.Pass.extension == "LargeD0":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_LargeD0')

    else:
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSPSeededFinder')

    if flags.ITk.Tracking.doStoreTrackSeeds:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
        acc.addPublicTool(extrapolator)  # TODO: migrate to private?

        kwargs.setdefault("SeedToTrackConversion", CompFactory.InDet.SeedToTrackConversionTool(
            name="ITkSeedToTrackConversion",
            Extrapolator=extrapolator,
            OutputName=f"SiSPSeedSegments{flags.ITk.Tracking.Pass.extension}"))
        kwargs.setdefault("SeedSegmentsWrite", True)

    ITkSiTrackMaker = CompFactory.InDet.SiTrackMaker_xk(name = name+flags.ITk.Tracking.Pass.extension, **kwargs)
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
    if (len(InputCollections) > 0) and (flags.ITk.Tracking.Pass.extension == "LargeD0" or flags.ITk.Tracking.Pass.extension == "ConversionFinding"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if usePrdAssociationTool:
        prefix = 'ITk'
        suffix = flags.ITk.Tracking.Pass.extension

    ITkSiTrackMaker = acc.popToolsAndMerge(ITkSiTrackMaker_xkCfg(flags,
                                                                 InputCollections = InputCollections ))
    from TrkConfig.AtlasExtrapolatorToolsConfig import ITkPropagatorCfg
    ITkPropagator = acc.popToolsAndMerge(ITkPropagatorCfg(flags))
    ITkTrackSummaryToolNoHoleSearch = acc.getPrimaryAndMerge(TC.ITkTrackSummaryToolNoHoleSearchCfg(flags))
    ITkSiSpacePointsSeedMaker = acc.popToolsAndMerge(ITkSiSpacePointsSeedMakerCfg(flags,
                                                                                  InputCollections = InputCollections ))

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
    kwargs.setdefault("useZvertexTool", False)
    kwargs.setdefault("useZBoundFinding", flags.ITk.Tracking.Pass.doZBoundary)
    kwargs.setdefault("ITKGeometry", True)
    kwargs.setdefault("SpacePointsSCTName", "ITkStripSpacePoints")
    kwargs.setdefault("SpacePointsPixelName", "ITkPixelSpacePoints")

    if flags.ITk.Tracking.doFastTracking :
        kwargs.setdefault("doFastTracking", True)

        if 'InDetEtaDependentCutsSvc' not in kwargs :
            acc.merge(TC.ITkEtaDependentCutsSvcCfg(flags))
            kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("ITkEtaDependentCutsSvc"+flags.ITk.Tracking.Pass.extension))

    ITkSiSPSeededTrackFinder = CompFactory.InDet.SiSPSeededTrackFinder(name = name+flags.ITk.Tracking.Pass.extension, **kwargs)
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
    ITkCopyAlgForAmbi = CompFactory.Trk.TrkCollectionAliasAlg (name = name+flags.ITk.Tracking.Pass.extension, **kwargs)
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

    ITkPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.ITkPRDtoTrackMapToolGangedPixelsCfg(flags))

    kwargs.setdefault("DriftCircleCutTool", None)
    kwargs.setdefault("AssociationTool" , ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("minTRTHits"      , 0) # used for Si only tracking !!!
    kwargs.setdefault("UseParameterization" , False)
    kwargs.setdefault("Cosmics"             , flags.Beam.Type == 'cosmics')
    kwargs.setdefault("doPixelSplitting"    , flags.ITk.Tracking.doPixelClusterSplitting )
    kwargs.setdefault("doITk" , True)

    kwargs.setdefault("sharedProbCut"             , flags.ITk.Tracking.pixelClusterSplitProb1)
    kwargs.setdefault("sharedProbCut2"            , flags.ITk.Tracking.pixelClusterSplitProb2)
    kwargs.setdefault("minSiHitsToAllowSplitting" , 9)
    kwargs.setdefault("minUniqueSCTHits"          , 4)
    kwargs.setdefault("minTrackChi2ForSharedHits" , 3)
    kwargs.setdefault("minPtSplit"                , flags.ITk.Tracking.pixelClusterSplitMinPt)       #Only allow split clusters on track withe pt greater than this MeV
    kwargs.setdefault("maxSharedModulesInROI"     , 3)     #Maximum number of shared modules for tracks in ROI
    kwargs.setdefault("minNotSharedInROI"         , 2)     #Minimum number of unique modules for tracks in ROI
    kwargs.setdefault("minSiHitsToAllowSplittingInROI" , 8)  #Minimum number of Si hits to allow splittings for tracks in ROI
    kwargs.setdefault("phiWidth"                  , 0.05)     #Split cluster ROI size
    kwargs.setdefault("etaWidth"                  , 0.05)     #Split cluster ROI size
    kwargs.setdefault("doEmCaloSeed"              , flags.ITk.Tracking.doCaloSeededAmbi)   #Only split in cluster in region of interest
    kwargs.setdefault("InputEmClusterContainerName", 'ITkCaloClusterROIs')
    if flags.Detector.EnableCalo:
        from InDetConfig.ITkRecCaloSeededROISelectionConfig import ITkCaloClusterROI_SelectorCfg
        acc.merge(ITkCaloClusterROI_SelectorCfg(flags))
    kwargs.setdefault("doHadCaloSeed"             , flags.ITk.Tracking.doCaloSeededAmbi)   #Do special cuts in region of interest
    kwargs.setdefault("InputHadClusterContainerName", "ITkHadCaloClusterROIs" + "Bjet")
    if flags.Detector.EnableCalo:
        from InDetConfig.ITkRecCaloSeededROISelectionConfig import ITkHadCaloClusterROI_SelectorCfg
        acc.merge(ITkHadCaloClusterROI_SelectorCfg(flags))
    kwargs.setdefault("minPtConv"                 , 10000)   #Only allow split clusters on track withe pt greater than this MeV
    kwargs.setdefault("minPtBjetROI"              , 10000)
    kwargs.setdefault("phiWidthEM"                , 0.05)     #Split cluster ROI size
    kwargs.setdefault("etaWidthEM"                , 0.05)     #Split cluster ROI size

    if 'InDetEtaDependentCutsSvc' not in kwargs :
        acc.merge(TC.ITkEtaDependentCutsSvcCfg(flags))
        kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("ITkEtaDependentCutsSvc"+flags.ITk.Tracking.Pass.extension))

    ITkAmbiTrackSelectionTool = CompFactory.InDet.InDetDenseEnvAmbiTrackSelectionTool(name = name+flags.ITk.Tracking.Pass.extension, **kwargs)
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

    kwargs.setdefault("sharedProbCut",  flags.ITk.Tracking.pixelClusterSplitProb1)
    kwargs.setdefault("sharedProbCut2", flags.ITk.Tracking.pixelClusterSplitProb2)
    if flags.ITk.Tracking.Pass.extension == "":
        kwargs.setdefault("SplitClusterMap_old", "")
    kwargs.setdefault("SplitClusterMap_new", 'SplitClusterAmbiguityMap'+flags.ITk.Tracking.Pass.extension)

    kwargs.setdefault("ScoringTool", ITkAmbiScoringTool)
    kwargs.setdefault("SplitProbTool", ITkNnPixelClusterSplitProbTool if flags.ITk.Tracking.doPixelClusterSplitting else None,)
    kwargs.setdefault("AssociationTool", ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationToolNotGanged", ITkPRDtoTrackMapTool)
    kwargs.setdefault("AssociationMapName", 'ITkPRDToTrackMap'+flags.ITk.Tracking.Pass.extension)
    kwargs.setdefault("InputClusterSplitProbabilityName", ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'SplitProb'+flags.ITk.Tracking.Pass.extension)

    # DenseEnvironmentsAmbiguityScoreProcessorTool
    ITkAmbiguityScoreProcessor = CompFactory.Trk.DenseEnvironmentsAmbiguityScoreProcessorTool(name=name+flags.ITk.Tracking.Pass.extension, **kwargs)

    acc.setPrivateTools(ITkAmbiguityScoreProcessor)
    return acc

def ITkDenseEnvironmentsAmbiguityProcessorToolCfg(flags, name = "ITkAmbiguityProcessor", ClusterSplitProbContainer='', **kwargs) :
    acc = ComponentAccumulator()

    #
    # --- set up different Scoring Tool for collisions and cosmics
    #
    if flags.Beam.Type == 'cosmics':
        ITkAmbiScoringTool = acc.popToolsAndMerge(TC.ITkCosmicsScoringToolCfg(flags))
    else:
        ITkAmbiScoringTool = acc.popToolsAndMerge(TC.ITkAmbiScoringToolCfg(flags))

    fitter_args = {}
    fitter_args.setdefault("nameSuffix", 'Ambi'+flags.ITk.Tracking.Pass.extension)
    fitter_args.setdefault("SplitClusterMapExtension", flags.ITk.Tracking.Pass.extension)
    fitter_args.setdefault("ClusterSplitProbabilityName", 'ITkAmbiguityProcessorSplitProb'+flags.ITk.Tracking.Pass.extension)

    fitter_args.setdefault("DoHoleSearch", True)

    ITkBoundaryCheckTool = acc.popToolsAndMerge(ITkBoundaryCheckToolCfg(flags))
    fitter_args.setdefault("BoundaryCheckTool", ITkBoundaryCheckTool)

    fitter_list=[]
    ITkTrackFitterAmbi = acc.getPrimaryAndMerge(TC.ITkTrackFitterCfg(flags, name='ITkTrackFitter'+'Ambi'+flags.ITk.Tracking.Pass.extension, **fitter_args))
    fitter_list.append(ITkTrackFitterAmbi)

    ITkPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.ITkPRDtoTrackMapToolGangedPixelsCfg(flags))

    ambi_track_summary_tool = acc.getPrimaryAndMerge(TC.ITkTrackSummaryToolCfg( flags,
                                                                                namePrefix                  = 'ITkAmbiguityProcessorSplitProb',
                                                                                nameSuffix                  = flags.ITk.Tracking.Pass.extension,
                                                                                ClusterSplitProbabilityName = 'ITkAmbiguityProcessorSplitProb'+flags.ITk.Tracking.Pass.extension))

    ITkAmbiTrackSelectionTool = acc.popToolsAndMerge(ITkAmbiTrackSelectionToolCfg(flags))


    kwargs.setdefault("Fitter", fitter_list)
    kwargs.setdefault("AssociationTool", ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationMapName", 'ITkPRDToTrackMap'+flags.ITk.Tracking.Pass.extension)
    kwargs.setdefault("TrackSummaryTool", ambi_track_summary_tool)
    kwargs.setdefault("ScoringTool", ITkAmbiScoringTool)
    kwargs.setdefault("SelectionTool", ITkAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName", 'SplitProb'+flags.ITk.Tracking.Pass.extension)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'ITkAmbiguityProcessorSplitProb'+flags.ITk.Tracking.Pass.extension)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("tryBremFit", flags.ITk.Tracking.doBremRecovery and flags.Detector.EnableCalo and flags.ITk.Tracking.Pass.extension == "") # Disabled for second passes in reco
    kwargs.setdefault("caloSeededBrem", flags.ITk.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo)
    kwargs.setdefault("pTminBrem", flags.ITk.Tracking.Pass.minPTBrem[0])
    kwargs.setdefault("RefitPrds", True)
    kwargs.setdefault("KeepHolesFromBeforeRefit", flags.ITk.Tracking.useHolesFromPattern)

    # DenseEnvironmentsAmbiguityProcessorTool
    ProcessorTool = CompFactory.Trk.DenseEnvironmentsAmbiguityProcessorTool
    ITkAmbiguityProcessor = ProcessorTool(name=name+flags.ITk.Tracking.Pass.extension, **kwargs)
    acc.setPrivateTools(ITkAmbiguityProcessor)
    return acc

def ITkTrkAmbiguityScoreCfg(flags, name="ITkAmbiguityScore", SiSPSeededTrackCollectionKey = None, **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- set input and output collection
    #
    InputTrackCollection = SiSPSeededTrackCollectionKey

    ITkAmbiguityScoreProcessor = acc.popToolsAndMerge(ITkDenseEnvironmentsAmbiguityScoreProcessorToolCfg(flags))

    #
    # --- configure Ambiguity (score) solver
    #
    kwargs.setdefault("TrackInput" , [ InputTrackCollection ])
    kwargs.setdefault("TrackOutput", 'ScoredMap'+'ITkAmbiguityScore'+ flags.ITk.Tracking.Pass.extension)
    kwargs.setdefault("AmbiguityScoreProcessor" ,  ITkAmbiguityScoreProcessor ) ## TODO: check the case when it is None object

    ITkAmbiguityScore = CompFactory.Trk.TrkAmbiguityScore(name = name+flags.ITk.Tracking.Pass.extension, **kwargs)
    acc.addEventAlgo(ITkAmbiguityScore)
    return acc


def ITkTrkAmbiguitySolverCfg(flags, name="ITkAmbiguitySolver", ResolvedTrackCollectionKey = None, ClusterSplitProbContainer='', **kwargs):
    acc = ComponentAccumulator()
    SiTrackCollection = ResolvedTrackCollectionKey

    # DenseEnvironmentsAmbiguityProcessorTool
    ITkAmbiguityProcessor = acc.popToolsAndMerge(ITkDenseEnvironmentsAmbiguityProcessorToolCfg( flags,
                                                                                                ClusterSplitProbContainer=ClusterSplitProbContainer))

    #
    # --- configure Ambiguity solver
    #
    kwargs.setdefault("TrackInput", 'ScoredMap'+'ITkAmbiguityScore'+ flags.ITk.Tracking.Pass.extension)
    kwargs.setdefault("TrackOutput", SiTrackCollection)
    kwargs.setdefault( "AmbiguityProcessor", ITkAmbiguityProcessor)

    ITkAmbiguitySolver = CompFactory.Trk.TrkAmbiguitySolver(name = name+flags.ITk.Tracking.Pass.extension, **kwargs)
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
    if (len(InputCollections) > 0) and (flags.ITk.Tracking.Pass.extension == "LargeD0" or flags.ITk.Tracking.Pass.extension == "ConversionFinding"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if usePrdAssociationTool:
        prefix = 'ITk'
        suffix = flags.ITk.Tracking.Pass.extension
        acc.merge(TC.ITkTrackPRD_AssociationCfg(flags,namePrefix = prefix,
                                                nameSuffix = suffix,
                                                TracksName = list(InputCollections)))

    # ------------------------------------------------------------
    #
    # ----------- SiSPSeededTrackFinder
    #
    # ------------------------------------------------------------

    SiSPSeededTrackFinderCfg = ITkSiSPSeededTrackFinderCfg
    if flags.ITk.Tracking.Pass.extension == "ConversionFinding":
        SiSPSeededTrackFinderCfg = ITkSiSPSeededTrackFinderROIConvCfg
    acc.merge(SiSPSeededTrackFinderCfg( flags,
                                        InputCollections = InputCollections,
                                        SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey))
    # ------------------------------------------------------------
    #
    # ---------- Ambiguity solving
    #
    # ------------------------------------------------------------

    if flags.ITk.Tracking.doFastTracking:
        acc.merge(ITkCopyAlgForAmbiCfg( flags,
                                        InputTrackCollection = SiSPSeededTrackCollectionKey,
                                        OutputTrackCollection = ResolvedTrackCollectionKey ))

    else:
        acc.merge(ITkTrkAmbiguityScoreCfg( flags,
                                           SiSPSeededTrackCollectionKey=SiSPSeededTrackCollectionKey))

        acc.merge(ITkTrkAmbiguitySolverCfg(flags,
                                           ResolvedTrackCollectionKey=ResolvedTrackCollectionKey))

    return acc

