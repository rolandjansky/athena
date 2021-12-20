# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
import InDetConfig.TrackingCommonConfig         as   TC

def SiSpacePointsSeedMakerCfg(flags, name="InDetSpSeedsMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- decide if use the association tool
    #
    if (len(InputCollections) > 0) and \
        (flags.InDet.Tracking.extension == "LowPt" \
            or flags.InDet.Tracking.extension == "VeryLowPt" \
            or flags.InDet.Tracking.extension == "LargeD0" \
            or flags.InDet.Tracking.extension == "R3LargeD0" \
            or flags.InDet.Tracking.extension == "LowPtLargeD0" \
            or flags.InDet.Tracking.extension == "BeamGas" \
            or flags.InDet.Tracking.extension == "ForwardTracks" \
            or flags.InDet.Tracking.extension == "Disappearing"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if usePrdAssociationTool:
        prefix = 'InDet'
        suffix = flags.InDet.Tracking.extension
    #
    # --- Space points seeds maker, use different ones for cosmics and collisions
    #
    if flags.InDet.Tracking.extension == "DBM":
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_ATLxk
    elif flags.Beam.Type == 'cosmics':
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_Cosmic
    elif flags.InDet.doHeavyIon:
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_HeavyIon
    elif flags.InDet.Tracking.extension == "LowPt" or flags.InDet.Tracking.extension == "VeryLowPt" or (flags.InDet.Tracking.extension == "Pixel" and flags.InDet.doMinBias) :
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_LowMomentum
    elif flags.InDet.Tracking.extension == "BeamGas":
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_BeamGas
    else:
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_ATLxk

    kwargs.setdefault("pTmin", flags.InDet.Tracking.minPT )
    kwargs.setdefault("maxdImpact", flags.InDet.Tracking.maxPrimaryImpact )
    kwargs.setdefault("maxZ", flags.InDet.Tracking.maxZImpact )
    kwargs.setdefault("minZ", -flags.InDet.Tracking.maxZImpact )
    kwargs.setdefault("usePixel", flags.InDet.Tracking.usePixel)
    kwargs.setdefault("SpacePointsPixelName", 'PixelSpacePoints') # InDetKeys.PixelSpacePoints()
    kwargs.setdefault("useSCT", flags.InDet.Tracking.useSCT and flags.InDet.Tracking.useSCTSeeding )
    kwargs.setdefault("SpacePointsSCTName", 'SCT_SpacePoints') # InDetKeys.SCT_SpacePoints()
    kwargs.setdefault("useOverlapSpCollection", flags.InDet.Tracking.useSCT and flags.InDet.Tracking.useSCTSeeding )
    kwargs.setdefault("SpacePointsOverlapName", 'OverlapSpacePoints') # InDetKeys.OverlapSpacePoints()
    kwargs.setdefault("radMax", flags.InDet.Tracking.radMax)
    kwargs.setdefault("RapidityCut",  flags.InDet.Tracking.maxEta )


    if flags.InDet.Tracking.extension == "" or flags.InDet.doHeavyIon or  flags.InDet.Tracking.extension == "ForwardTracks":
        kwargs.setdefault("maxdImpactPPS", flags.InDet.Tracking.maxdImpactPPSSeeds)
        kwargs.setdefault("maxdImpactSSS", flags.InDet.Tracking.maxdImpactSSSSeeds)
        if not flags.InDet.doHeavyIon:
            kwargs.setdefault("maxSeedsForSpacePointStrips", flags.InDet.Tracking.maxSeedsPerSP_Strips)
            kwargs.setdefault("maxSeedsForSpacePointPixels", flags.InDet.Tracking.maxSeedsPerSP_Pixels)
            kwargs.setdefault("alwaysKeepConfirmedStripSeeds", flags.InDet.Tracking.keepAllConfirmedStripSeeds)
            kwargs.setdefault("alwaysKeepConfirmedPixelSeeds", flags.InDet.Tracking.keepAllConfirmedPixelSeeds)
            kwargs.setdefault("mindRadius", 10)
            kwargs.setdefault("maxSizeSP", 200)
            kwargs.setdefault("dImpactCutSlopeUnconfirmedSSS", 1.25)
            kwargs.setdefault("dImpactCutSlopeUnconfirmedPPP", 2.0)
        
    if flags.InDet.Tracking.extension == "R3LargeD0":
        kwargs.setdefault("optimisePhiBinning", False)
        kwargs.setdefault("usePixel", False)
        kwargs.setdefault("etaMax", flags.InDet.Tracking.maxEta)
        kwargs.setdefault("maxSeedsForSpacePointStrips", flags.InDet.Tracking.maxSeedsPerSP_Strips)
        kwargs.setdefault("alwaysKeepConfirmedStripSeeds", flags.InDet.Tracking.keepAllConfirmedStripSeeds)
        kwargs.setdefault("maxdRadius", 150)
        kwargs.setdefault("seedScoreBonusConfirmationSeed", -2000)

    if usePrdAssociationTool:
        # not all classes have that property !!!
        kwargs.setdefault("PRDtoTrackMap", prefix+'PRDtoTrackMap'+ suffix \
                                            if usePrdAssociationTool else '')
    if not flags.Beam.Type == 'cosmics':
        kwargs.setdefault("maxRadius1", 0.75*flags.InDet.Tracking.radMax)
        kwargs.setdefault("maxRadius2", flags.InDet.Tracking.radMax)
        kwargs.setdefault("maxRadius3", flags.InDet.Tracking.radMax)
    if flags.InDet.Tracking.extension == "LowPt" or flags.InDet.Tracking.extension == "VeryLowPt" or (flags.InDet.Tracking.extension == "Pixel" and flags.InDet.doMinBias):
        kwargs.setdefault("pTmax", flags.InDet.Tracking.maxPT)
        kwargs.setdefault("mindRadius", 4.0)
    if flags.InDet.Tracking.extension == "ForwardTracks":
        kwargs.setdefault("checkEta", True)
        kwargs.setdefault("etaMin", flags.InDet.Tracking.minEta)
        kwargs.setdefault("etaMax", flags.InDet.Tracking.maxEta)
        kwargs.setdefault("RapidityCut", flags.InDet.Tracking.maxEta)
    if flags.InDet.Tracking.extension == "DBM":
        kwargs.setdefault("etaMin", flags.InDet.Tracking.minEta)
        kwargs.setdefault("etaMax", flags.InDet.Tracking.maxEta)
        kwargs.setdefault("useDBM", True)

    InDetSiSpacePointsSeedMaker = SiSpacePointsSeedMaker (name = name+flags.InDet.Tracking.extension, **kwargs)

    acc.setPrivateTools(InDetSiSpacePointsSeedMaker)
    return acc

def SiZvertexMaker_xkCfg(flags, name="InDetZvertexMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("Zmax", flags.InDet.Tracking.maxZImpact)
    kwargs.setdefault("Zmin", -flags.InDet.Tracking.maxZImpact)
    kwargs.setdefault("minRatio", 0.17)
    
    InDetSiSpacePointsSeedMaker = acc.popToolsAndMerge(SiSpacePointsSeedMakerCfg(flags, 
                                                                                 InputCollections = InputCollections ))

    kwargs.setdefault("SeedMakerTool", InDetSiSpacePointsSeedMaker)
    if flags.InDet.doHeavyIon:
        kwargs.setdefault("HistSize", 2000)
        kwargs.setdefault("minContent", 30)

    InDetZvertexMaker = CompFactory.InDet.SiZvertexMaker_xk(name = name+flags.InDet.Tracking.extension, **kwargs)
    acc.setPrivateTools(InDetZvertexMaker)
    return acc

def SiDetElementsRoadMaker_xkCfg(flags, name="InDetSiRoadMaker", **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- SCT and Pixel detector elements road builder
    #
    InDetPatternPropagator = acc.getPrimaryAndMerge(TC.InDetPatternPropagatorCfg())

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("usePixel", flags.InDet.Tracking.usePixel )
    kwargs.setdefault("PixManagerLocation", 'Pixel')
    kwargs.setdefault("useSCT", flags.InDet.Tracking.useSCT)
    kwargs.setdefault("SCTManagerLocation", 'SCT')
    kwargs.setdefault("RoadWidth", flags.InDet.Tracking.roadWidth)

    InDetSiDetElementsRoadMaker = CompFactory.InDet.SiDetElementsRoadMaker_xk(name = name+flags.InDet.Tracking.extension, **kwargs)
    acc.setPrivateTools(InDetSiDetElementsRoadMaker)
    return acc

def SiCombinatorialTrackFinder_xkCfg(flags, name="InDetSiComTrackFinder", **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- Local track finding using sdCaloSeededSSSpace point seed
    #
    # @TODO ensure that PRD association map is used if usePrdAssociationTool is set
    is_dbm = flags.InDet.doDBMstandalone or flags.InDet.Tracking.extension == 'DBM'
    if not is_dbm:
        rot_creator_digital = acc.popToolsAndMerge(TC.InDetRotCreatorDigitalCfg(flags))
    else:
        rot_creator_digital = acc.popToolsAndMerge(TC.InDetRotCreatorDBMCfg(flags))

    acc.addPublicTool(rot_creator_digital)

    InDetPatternPropagator = acc.getPrimaryAndMerge(TC.InDetPatternPropagatorCfg())
    InDetPatternUpdator = acc.getPrimaryAndMerge(TC.InDetPatternUpdatorCfg())

    from  InDetConfig.InDetRecToolConfig import InDetBoundaryCheckToolCfg
    boundary_check_tool = acc.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags))

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("UpdatorTool", InDetPatternUpdator)
    kwargs.setdefault("BoundaryCheckTool", boundary_check_tool)
    kwargs.setdefault("RIOonTrackTool", rot_creator_digital)
    kwargs.setdefault("usePixel", flags.Detector.EnablePixel)
    kwargs.setdefault("useSCT", flags.Detector.EnableSCT if not is_dbm else False)
    kwargs.setdefault("PixelClusterContainer", 'PixelClusters') # InDetKeys.PixelClusters()
    kwargs.setdefault("SCT_ClusterContainer", 'SCT_Clusters') # InDetKeys.SCT_Clusters()

    if flags.InDet.Tracking.extension == "": 
        kwargs.setdefault("writeHolesFromPattern", flags.InDet.useHolesFromPattern)

    if is_dbm :
        kwargs.setdefault("MagneticFieldMode", "NoField")
        kwargs.setdefault("TrackQualityCut", 9.3)

    if flags.Detector.EnableSCT:
        InDetSCT_ConditionsSummaryTool = CompFactory.SCT_ConditionsSummaryTool(name = 'InDetSCT_ConditionsSummaryTool')
        kwargs.setdefault("SctSummaryTool", InDetSCT_ConditionsSummaryTool)
    else:
        kwargs.setdefault("SctSummaryTool", None)

    track_finder = CompFactory.InDet.SiCombinatorialTrackFinder_xk(name = name+flags.InDet.Tracking.extension, **kwargs)
    acc.setPrivateTools(track_finder)
    return acc

def SiTrackMaker_xkCfg(flags, name="InDetSiTrackMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()
    useBremMode = flags.InDet.Tracking.extension == "" or flags.InDet.Tracking.extension == "DBM"
    InDetSiDetElementsRoadMaker = acc.popToolsAndMerge(SiDetElementsRoadMaker_xkCfg(flags))

    if flags.InDet.Tracking.usePixel:
        acc.addCondAlgo(CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk(name="InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                              ReadKey="PixelDetectorElementCollection",
                                                                              WriteKey="PixelDetElementBoundaryLinks_xk"))
    if flags.InDet.Tracking.useSCT:
        acc.addCondAlgo(CompFactory.InDet.SiDetElementsRoadCondAlg_xk(name="InDet__SiDetElementsRoadCondAlg_xk"))

        acc.addCondAlgo(CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk(name="InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                              ReadKey="SCT_DetectorElementCollection",
                                                                              WriteKey="SCT_DetElementBoundaryLinks_xk"))

    track_finder = acc.popToolsAndMerge(SiCombinatorialTrackFinder_xkCfg(flags))

    #
    # --- decide if use the association tool
    #
    if (len(InputCollections) > 0) and (flags.InDet.Tracking.extension == "LowPt" or flags.InDet.Tracking.extension == "VeryLowPt" or flags.InDet.Tracking.extension == "LargeD0" or flags.InDet.Tracking.extension == "R3LargeD0" or flags.InDet.Tracking.extension == "LowPtLargeD0" or flags.InDet.Tracking.extension == "BeamGas" or flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "Disappearing"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False

    kwargs.setdefault("useSCT", flags.InDet.Tracking.useSCT)  #flags.InDet.Tracking.useSCT()
    kwargs.setdefault("usePixel", flags.InDet.Tracking.usePixel) #flags.InDet.Tracking.usePixel()
    kwargs.setdefault("RoadTool", InDetSiDetElementsRoadMaker)
    kwargs.setdefault("CombinatorialTrackFinder", track_finder)
    kwargs.setdefault("pTmin", flags.InDet.Tracking.minPT)
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.minPTBrem)
    kwargs.setdefault("pTminSSS", flags.InDet.Tracking.pT_SSScut)
    kwargs.setdefault("nClustersMin", flags.InDet.Tracking.minClusters)
    kwargs.setdefault("nHolesMax", flags.InDet.Tracking.nHolesMax)
    kwargs.setdefault("nHolesGapMax", flags.InDet.Tracking.nHolesGapMax)
    kwargs.setdefault("SeedsFilterLevel", flags.InDet.Tracking.seedFilterLevel)
    kwargs.setdefault("Xi2max", flags.InDet.Tracking.Xi2max)
    kwargs.setdefault("Xi2maxNoAdd", flags.InDet.Tracking.Xi2maxNoAdd)
    kwargs.setdefault("nWeightedClustersMin", flags.InDet.Tracking.nWeightedClustersMin)
    kwargs.setdefault("CosmicTrack", flags.Beam.Type == 'cosmics')
    kwargs.setdefault("Xi2maxMultiTracks", flags.InDet.Tracking.Xi2max)
    kwargs.setdefault("useSSSseedsFilter", flags.InDet.doSSSfilter)
    kwargs.setdefault("doMultiTracksProd", True)
    kwargs.setdefault("useBremModel", flags.InDet.doBremRecovery and useBremMode and flags.Detector.EnableCalo) # only for NewTracking the brem is debugged !!!
    kwargs.setdefault("doCaloSeededBrem", flags.InDet.doCaloSeededBrem and flags.Detector.EnableCalo)
    if kwargs["useBremModel"] and kwargs["doCaloSeededBrem"]:
        from InDetConfig.InDetRecCaloSeededROISelectionConfig import CaloClusterROI_SelectorCfg
        acc.merge(CaloClusterROI_SelectorCfg(flags))
    kwargs.setdefault("doHadCaloSeedSSS", flags.InDet.doHadCaloSeededSSS and flags.Detector.EnableCalo)
    kwargs.setdefault("phiWidth", flags.InDet.Tracking.phiWidthBrem)
    kwargs.setdefault("etaWidth", flags.InDet.Tracking.etaWidthBrem)
    kwargs.setdefault("InputClusterContainerName", 'InDetCaloClusterROIs') # InDetKeys.CaloClusterROIContainer()
    kwargs.setdefault("InputHadClusterContainerName", 'InDetHadCaloClusterROIs') # InDetKeys.HadCaloClusterROIContainer()
    kwargs.setdefault("UseAssociationTool", usePrdAssociationTool)

    if flags.InDet.Tracking.extension == "DBM":
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

    elif flags.InDet.doHeavyIon:
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_HeavyIon')
    
    elif flags.InDet.Tracking.extension == "LowPt":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_LowMomentum')

    elif flags.InDet.Tracking.extension == "VeryLowPt" or (flags.InDet.Tracking.extension == "Pixel" and flags.InDet.doMinBias):
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_VeryLowMomentum')

    elif flags.InDet.Tracking.extension == "BeamGas":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_BeamGas')

    elif flags.InDet.Tracking.extension == "ForwardTracks":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_ForwardTracks')

    elif flags.InDet.Tracking.extension == "LargeD0" or flags.InDet.Tracking.extension == "R3LargeD0" or flags.InDet.Tracking.extension == "LowPtLargeD0":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_LargeD0')

    else:
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSPSeededFinder')
            
    if flags.InDet.doStoreTrackSeeds:
        InDet_SeedToTrackConversion = CompFactory.InDet.SeedToTrackConversionTool(  name = "InDet_SeedToTrackConversion",
                                                                                    OutputName = 'SiSPSeedSegments' + flags.InDet.Tracking.extension)
        acc.setPrivateTools(InDet_SeedToTrackConversion)
        kwargs.setdefault("SeedToTrackConversion", InDet_SeedToTrackConversion)
        kwargs.setdefault("SeedSegmentsWrite", True)

    InDetSiTrackMaker = CompFactory.InDet.SiTrackMaker_xk(name = name+flags.InDet.Tracking.extension, **kwargs)
    acc.setPrivateTools(InDetSiTrackMaker)
    return acc

def SiSPSeededTrackFinderCfg(flags, name="InDetSiSpTrackFinder", InputCollections = None, SiSPSeededTrackCollectionKey = None, **kwargs) :
    acc = ComponentAccumulator()

    # set output track collection name
    #
    SiTrackCollection = SiSPSeededTrackCollectionKey
    #
    # --- decide if use the association tool
    #
    if (len(InputCollections) > 0) and (flags.InDet.Tracking.extension == "LowPt" or flags.InDet.Tracking.extension == "VeryLowPt" or flags.InDet.Tracking.extension == "LargeD0" or flags.InDet.Tracking.extension == "R3LargeD0" or flags.InDet.Tracking.extension == "LowPtLargeD0" or flags.InDet.Tracking.extension == "BeamGas" or flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "Disappearing"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if usePrdAssociationTool:
        prefix = 'InDet'
        suffix = flags.InDet.Tracking.extension

    InDetSiTrackMaker = acc.popToolsAndMerge(SiTrackMaker_xkCfg(flags,
                                                                InputCollections = InputCollections ))
 
    InDetTrackSummaryToolNoHoleSearch = acc.getPrimaryAndMerge(TC.InDetTrackSummaryToolNoHoleSearchCfg(flags))

    InDetSiSpacePointsSeedMaker = acc.popToolsAndMerge(SiSpacePointsSeedMakerCfg(flags, 
                                                                                 InputCollections = InputCollections ))

    #
    # --- Z-coordinates primary vertices finder (only for collisions)
    #
    if flags.InDet.useZvertexTool and flags.InDet.Tracking.extension != "DBM":
        InDetZvertexMaker = acc.popToolsAndMerge(SiZvertexMaker_xkCfg(flags,
                                                 InputCollections = InputCollections))
    else:
        InDetZvertexMaker = None

    #
    # --- Setup Track finder using space points seeds
    #
    kwargs.setdefault("TrackTool", InDetSiTrackMaker)
    kwargs.setdefault("PRDtoTrackMap", prefix+'PRDtoTrackMap'+suffix \
                                            if usePrdAssociationTool else '')
    kwargs.setdefault("TrackSummaryTool", InDetTrackSummaryToolNoHoleSearch)
    kwargs.setdefault("TracksLocation", SiTrackCollection)
    kwargs.setdefault("SeedsTool", InDetSiSpacePointsSeedMaker)
    kwargs.setdefault("ZvertexTool", InDetZvertexMaker)
    kwargs.setdefault("useMBTSTimeDiff", flags.InDet.useMBTSTimeDiff)

    if flags.InDet.Tracking.extension == "ForwardTracks":
        kwargs.setdefault("useZvertexTool", flags.InDet.useZvertexTool)
        kwargs.setdefault("useNewStrategy", False)
        kwargs.setdefault("useZBoundFinding", False)
    else:
        kwargs.setdefault("useZvertexTool", flags.InDet.useZvertexTool and flags.InDet.Tracking.extension != "DBM")
        kwargs.setdefault("useNewStrategy", flags.InDet.useNewSiSPSeededTF and flags.InDet.Tracking.extension != "DBM")
        kwargs.setdefault("useZBoundFinding", flags.InDet.Tracking.doZBoundary and flags.InDet.Tracking.extension != "DBM")
    
    if flags.InDet.doHeavyIon :
        kwargs.setdefault("FreeClustersCut",2) #Heavy Ion optimization from Igor

    if flags.InDet.Tracking.extension == "":
        kwargs.setdefault("writeHolesFromPattern", flags.InDet.useHolesFromPattern)

    InDetSiSPSeededTrackFinder = CompFactory.InDet.SiSPSeededTrackFinder(name = name+flags.InDet.Tracking.extension, **kwargs)
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
    prob1 = flags.InDet.pixelClusterSplitProb1
    prob2 = flags.InDet.pixelClusterSplitProb2
    nhitsToAllowSplitting = 8 if flags.GeoModel.Run == 'RUN1' else 9

    if flags.InDet.doTIDE_Ambi and not (flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "DBM"):
        AmbiTrackSelectionTool = CompFactory.InDet.InDetDenseEnvAmbiTrackSelectionTool
    else:
        AmbiTrackSelectionTool = CompFactory.InDet.InDetAmbiTrackSelectionTool

    if 'UseParameterization' in kwargs and kwargs.get('UseParameterization',False) :
        InDetTRTDriftCircleCut = acc.getPrimaryAndMerge(TC.InDetTRTDriftCircleCutForPatternRecoCfg(flags))
        kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCut)

    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags))

    kwargs.setdefault("AssociationTool" , InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("minHits"         , flags.InDet.Tracking.minClusters)
    kwargs.setdefault("minNotShared"    , flags.InDet.Tracking.minSiNotShared)
    kwargs.setdefault("maxShared"       , flags.InDet.Tracking.maxShared)
    kwargs.setdefault("minTRTHits"      , 0) # used for Si only tracking !!!
    kwargs.setdefault("UseParameterization" , False)
    kwargs.setdefault("Cosmics"             , flags.Beam.Type == 'cosmics' and flags.InDet.Tracking.extension != "DBM")
    kwargs.setdefault("doPixelSplitting"    , flags.InDet.doPixelClusterSplitting and flags.InDet.Tracking.extension != "DBM")
    
    if flags.InDet.doTIDE_Ambi and not (flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "DBM"):
        kwargs.setdefault("sharedProbCut"             , prob1)
        kwargs.setdefault("sharedProbCut2"            , prob2)
        kwargs.setdefault("minSiHitsToAllowSplitting" , nhitsToAllowSplitting)
        kwargs.setdefault("minUniqueSCTHits"          , 4)
        kwargs.setdefault("minTrackChi2ForSharedHits" , 3)
        kwargs.setdefault("minPtSplit"                , flags.InDet.pixelClusterSplitMinPt)       #Only allow split clusters on track withe pt greater than this MeV
        kwargs.setdefault("maxSharedModulesInROI"     , 3)     #Maximum number of shared modules for tracks in ROI
        kwargs.setdefault("minNotSharedInROI"         , 2)     #Minimum number of unique modules for tracks in ROI
        kwargs.setdefault("minSiHitsToAllowSplittingInROI" , 8)  #Minimum number of Si hits to allow splittings for tracks in ROI
        kwargs.setdefault("phiWidth"                  , 0.05)     #Split cluster ROI size
        kwargs.setdefault("etaWidth"                  , 0.05)     #Split cluster ROI size
        kwargs.setdefault("doEmCaloSeed"              , flags.InDet.doCaloSeededAmbi)   #Only split in cluster in region of interest
        kwargs.setdefault("InputEmClusterContainerName", 'InDetCaloClusterROIs')
        if flags.InDet.doCaloSeededAmbi:
            from InDetConfig.InDetRecCaloSeededROISelectionConfig import CaloClusterROI_SelectorCfg
            acc.merge(CaloClusterROI_SelectorCfg(flags))
        kwargs.setdefault("doHadCaloSeed"             , flags.InDet.doCaloSeededAmbi)   #Do special cuts in region of interest
        kwargs.setdefault("InputHadClusterContainerName", "InDetHadCaloClusterROIs" + "Bjet")
        if flags.InDet.doCaloSeededAmbi:
            from InDetConfig.InDetRecCaloSeededROISelectionConfig import HadCaloClusterROI_SelectorCfg
            acc.merge(HadCaloClusterROI_SelectorCfg(flags))
        kwargs.setdefault("minPtConv"                 , 10000)   #Only allow split clusters on track withe pt greater than this MeV
        kwargs.setdefault("minPtBjetROI"              , 10000)
        kwargs.setdefault("phiWidthEM"                , 0.05)     #Split cluster ROI size
        kwargs.setdefault("etaWidthEM"                , 0.05)     #Split cluster ROI size
    
    elif flags.InDet.Tracking.extension == "DBM":
        kwargs.setdefault("maxShared"             , 1000)
        kwargs.setdefault("maxTracksPerSharedPRD" , 2)
        kwargs.setdefault("minHits"               , 0)
        kwargs.setdefault("minNotShared"          , 0)
        kwargs.setdefault("minScoreShareTracks"   , 0.0)
        kwargs.setdefault("minTRTHits"            , 0)
        kwargs.setdefault("sharedProbCut"         , 0.1)
    else:
        kwargs.setdefault("sharedProbCut", 0.10)

    InDetAmbiTrackSelectionTool = AmbiTrackSelectionTool(name = name+flags.InDet.Tracking.extension, **kwargs)
    acc.setPrivateTools(InDetAmbiTrackSelectionTool)
    return acc

def DenseEnvironmentsAmbiguityScoreProcessorToolCfg(flags, name = "InDetAmbiguityScoreProcessor", ClusterSplitProbContainer='', **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- set up different Scoring Tool for collisions and cosmics
    #
    if flags.Beam.Type == 'cosmics' and flags.InDet.Tracking.extension != "DBM":
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetCosmicsScoringToolCfg(flags))
    elif(flags.InDet.Tracking.extension == "R3LargeD0" and flags.InDet.nnCutLargeD0Threshold > 0):
        # Set up NN config
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetNNScoringToolSiCfg(flags))
    else:
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetAmbiScoringToolSiCfg(flags))

    from InDetConfig.SiliconPreProcessing import NnPixelClusterSplitProbToolCfg
    NnPixelClusterSplitProbTool = acc.popToolsAndMerge(NnPixelClusterSplitProbToolCfg(flags))

    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags))

    PRDtoTrackMapTool = acc.popToolsAndMerge(TC.PRDtoTrackMapToolCfg())

    prob1 = flags.InDet.pixelClusterSplitProb1
    prob2 = flags.InDet.pixelClusterSplitProb2
    
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
    if CommonGeometryFlags.Run() == 1:
        prob1 = flags.InDet.pixelClusterSplitProb1_run1
        prob2 = flags.InDet.pixelClusterSplitProb2_run1

    if flags.InDet.doTIDE_Ambi and not (flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "DBM") :
        kwargs.setdefault("sharedProbCut", prob1)
        kwargs.setdefault("sharedProbCut2", prob2)
        if flags.InDet.Tracking.extension == "":
            kwargs.setdefault("SplitClusterMap_old", "")
        elif flags.InDet.Tracking.extension == "Disappearing":
            kwargs.setdefault("SplitClusterMap_old", 'SplitClusterAmbiguityMap')
        kwargs.setdefault("SplitClusterMap_new", 'SplitClusterAmbiguityMap'+flags.InDet.Tracking.extension)

    kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)
    kwargs.setdefault("SplitProbTool", NnPixelClusterSplitProbTool if flags.InDet.doPixelClusterSplitting else None,)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationToolNotGanged", PRDtoTrackMapTool)
    kwargs.setdefault("AssociationMapName", 'PRDToTrackMap'+flags.InDet.Tracking.extension)
    kwargs.setdefault("InputClusterSplitProbabilityName", ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'SplitProb'+flags.InDet.Tracking.extension)

    # DenseEnvironmentsAmbiguityScoreProcessorTool
    ScoreProcessorTool = CompFactory.Trk.DenseEnvironmentsAmbiguityScoreProcessorTool
    InDetAmbiguityScoreProcessor = ScoreProcessorTool(name=name+flags.InDet.Tracking.extension, **kwargs)

    acc.setPrivateTools(InDetAmbiguityScoreProcessor)
    return acc

def DenseEnvironmentsAmbiguityProcessorToolCfg(flags, name = "InDetAmbiguityProcessor", ClusterSplitProbContainer='', **kwargs) :
    acc = ComponentAccumulator()

    useBremMode = flags.InDet.Tracking.extension == "" or flags.InDet.Tracking.extension == "DBM"
    
    #
    # --- set up different Scoring Tool for collisions and cosmics
    #
    if flags.Beam.Type == 'cosmics' and flags.InDet.Tracking.extension != "DBM":
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetCosmicsScoringToolCfg(flags))
    elif(flags.InDet.Tracking.extension == "R3LargeD0" and flags.InDet.nnCutLargeD0Threshold > 0):
        # Set up NN config
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetNNScoringToolSiCfg(flags))
    else:
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetAmbiScoringToolSiCfg(flags))

    use_low_pt_fitter =  True if flags.InDet.Tracking.extension == "LowPt" or flags.InDet.Tracking.extension == "VeryLowPt" or (flags.InDet.Tracking.extension == "Pixel" and flags.InDet.doMinBias) else False
    
    fitter_args = {}
    fitter_args.setdefault("nameSuffix", 'Ambi'+flags.InDet.Tracking.extension)
    fitter_args.setdefault("SplitClusterMapExtension", flags.InDet.Tracking.extension)
    fitter_args.setdefault("ClusterSplitProbabilityName", 'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.extension)

    if True: #flags.InDet.holeSearchInGX2Fit:
        fitter_args.setdefault("DoHoleSearch", True)
        from  InDetConfig.InDetRecToolConfig import InDetBoundaryCheckToolCfg
        InDetBoundaryCheckTool = acc.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags))
        fitter_args.setdefault("BoundaryCheckTool", InDetBoundaryCheckTool)

    fitter_list=[]
    if not use_low_pt_fitter:
        InDetTrackFitterAmbi = acc.popToolsAndMerge(TC.InDetTrackFitterCfg(flags, name='InDetTrackFitter'+'Ambi'+flags.InDet.Tracking.extension, **fitter_args))
        fitter_list.append(InDetTrackFitterAmbi)
    else:
        InDetTrackFitterLowPt = acc.popToolsAndMerge(TC.InDetTrackFitterLowPt(flags, name='InDetTrackFitterLowPt'+flags.InDet.Tracking.extension, **fitter_args))
        fitter_list.append(InDetTrackFitterLowPt)

    if flags.InDet.doRefitInvalidCov: 
        if len(flags.InDet.Tracking.extension) > 0 :
            fitter_args = {}
            fitter_args.setdefault("SplitClusterMapExtension", flags.InDet.Tracking.extension)
            KalmanFitter = acc.popToolsAndMerge(TC.KalmanFitterCfg(flags, name='KalmanFitter'+flags.InDet.Tracking.extension, **fitter_args))
            fitter_list.append(KalmanFitter)

            ReferenceKalmanFitter = acc.popToolsAndMerge(TC.ReferenceKalmanFitterCfg(flags, name='ReferenceKalmanFitter'+flags.InDet.Tracking.extension, **fitter_args))
            fitter_list.append(ReferenceKalmanFitter)
        else:
            KalmanFitter = acc.popToolsAndMerge(TC.KalmanFitterCfg(flags))
            fitter_list.append(KalmanFitter)

            ReferenceKalmanFitter = acc.popToolsAndMerge(TC.ReferenceKalmanFitterCfg(flags))
            fitter_list.append(ReferenceKalmanFitter)

    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags))

    ambi_track_summary_tool = acc.getPrimaryAndMerge(TC.InDetTrackSummaryToolCfg( flags,
                                                                                  namePrefix                  = 'InDetAmbiguityProcessorSplitProb',
                                                                                  nameSuffix                  = flags.InDet.Tracking.extension,
                                                                                  ClusterSplitProbabilityName = 'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.extension))

    InDetAmbiTrackSelectionTool = acc.popToolsAndMerge(InDetAmbiTrackSelectionToolCfg(flags))

    kwargs.setdefault("Fitter", fitter_list)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationMapName", 'PRDToTrackMap'+flags.InDet.Tracking.extension)
    kwargs.setdefault("TrackSummaryTool", ambi_track_summary_tool)
    kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)
    kwargs.setdefault("SelectionTool", InDetAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName", 'SplitProb'+flags.InDet.Tracking.extension)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.extension)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("tryBremFit", flags.InDet.doBremRecovery and useBremMode and flags.InDet.Tracking.extension != "DBM")
    kwargs.setdefault("caloSeededBrem", flags.InDet.doCaloSeededBrem and flags.InDet.Tracking.extension != "DBM")
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.minPTBrem)
    kwargs.setdefault("RefitPrds", True)
    kwargs.setdefault("KeepHolesFromBeforeRefit", flags.InDet.useHolesFromPattern)

    # DenseEnvironmentsAmbiguityProcessorTool
    ProcessorTool = CompFactory.Trk.DenseEnvironmentsAmbiguityProcessorTool
    InDetAmbiguityProcessor = ProcessorTool(name=name+flags.InDet.Tracking.extension, **kwargs)
    acc.setPrivateTools(InDetAmbiguityProcessor)
    return acc

def SimpleAmbiguityProcessorToolCfg(flags, name = "InDetAmbiguityProcessor", ClusterSplitProbContainer='', **kwargs) :
    acc = ComponentAccumulator()
    useBremMode = flags.InDet.Tracking.extension == "" or flags.InDet.Tracking.extension == "DBM"
    
    #
    # --- set up different Scoring Tool for collisions and cosmics
    #
    if flags.Beam.Type == 'cosmics' and flags.InDet.Tracking.extension != "DBM":
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetCosmicsScoringToolCfg(flags))
    elif(flags.InDet.Tracking.extension == "R3LargeD0" and flags.InDet.nnCutLargeD0Threshold > 0):
        # Set up NN config
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetNNScoringToolSiCfg(flags))
    else:
        InDetAmbiScoringTool = acc.popToolsAndMerge(TC.InDetAmbiScoringToolSiCfg(flags))

    InDetTrackFitter = acc.popToolsAndMerge(TC.InDetTrackFitterCfg(flags))

    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags))

    ambi_track_summary_tool = acc.getPrimaryAndMerge(TC.InDetTrackSummaryToolCfg( flags,
                                                                                namePrefix                  = 'InDetAmbiguityProcessorSplitProb',
                                                                                nameSuffix                  = flags.InDet.Tracking.extension,
                                                                                ClusterSplitProbabilityName = 'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.extension))

    InDetAmbiTrackSelectionTool = acc.popToolsAndMerge(InDetAmbiTrackSelectionToolCfg(flags))

    kwargs.setdefault("Fitter", InDetTrackFitter)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("TrackSummaryTool", ambi_track_summary_tool)
    kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)
    kwargs.setdefault("SelectionTool", InDetAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName", ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.extension)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("tryBremFit", flags.InDet.doBremRecovery and useBremMode and flags.InDet.Tracking.extension != "DBM")
    kwargs.setdefault("caloSeededBrem", flags.InDet.doCaloSeededBrem and flags.InDet.Tracking.extension != "DBM")
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.minPTBrem)
    kwargs.setdefault("RefitPrds", True)

    if flags.InDet.Tracking.extension == "Pixel" or flags.InDet.Tracking.extension == "DBM":
        kwargs.setdefault("SuppressHoleSearch", True)

    ProcessorTool = CompFactory.Trk.SimpleAmbiguityProcessorTool

    if flags.InDet.Tracking.extension == "LowPt" or flags.InDet.Tracking.extension == "VeryLowPt" or (flags.InDet.Tracking.extension == "Pixel" and flags.InDet.doMinBias):
        if ProcessorTool.getName().find('Dense') :
            pass
        else:
            InDetTrackFitterLowPt = acc.popToolsAndMerge(TC.InDetTrackFitterLowPt(flags))
            kwargs.setdefault("Fitter", InDetTrackFitterLowPt)

    if flags.InDet.materialInteractions:
        kwargs.setdefault("MatEffects", flags.InDet.materialInteractionsType)
    else:
        kwargs.setdefault("MatEffects", 0)

    InDetAmbiguityProcessor = ProcessorTool(name=name+flags.InDet.Tracking.extension, **kwargs)
    acc.setPrivateTools(InDetAmbiguityProcessor)
    return acc

def TrkAmbiguityScoreCfg(flags, name="InDetAmbiguityScore", SiSPSeededTrackCollectionKey = None, **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- set input and output collection
    #
    InputTrackCollection = SiSPSeededTrackCollectionKey

    if flags.InDet.doTIDE_Ambi and not (flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "DBM"):
        InDetAmbiguityScoreProcessor = acc.popToolsAndMerge(DenseEnvironmentsAmbiguityScoreProcessorToolCfg(flags))
    else:
        InDetAmbiguityScoreProcessor = None

    #
    # --- configure Ambiguity (score) solver
    #
    kwargs.setdefault("TrackInput" , [ InputTrackCollection ])
    kwargs.setdefault("TrackOutput", 'ScoredMap'+'InDetAmbiguityScore'+ flags.InDet.Tracking.extension)
    kwargs.setdefault("AmbiguityScoreProcessor" ,  InDetAmbiguityScoreProcessor ) ## TODO: check the case when it is None object

    InDetAmbiguityScore = CompFactory.Trk.TrkAmbiguityScore(name = name+flags.InDet.Tracking.extension, **kwargs)
    acc.addEventAlgo(InDetAmbiguityScore)
    return acc

def TrkAmbiguitySolverCfg(flags, name="InDetAmbiguitySolver", ResolvedTrackCollectionKey = None, ClusterSplitProbContainer='', **kwargs):
    acc = ComponentAccumulator()
    SiTrackCollection = ResolvedTrackCollectionKey

    if flags.InDet.doTIDE_Ambi and not (flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "DBM"):
    # DenseEnvironmentsAmbiguityProcessorTool
        InDetAmbiguityProcessor = acc.popToolsAndMerge(DenseEnvironmentsAmbiguityProcessorToolCfg(  flags,
                                                                                                    ClusterSplitProbContainer=ClusterSplitProbContainer))
    else:
        InDetAmbiguityProcessor = acc.popToolsAndMerge(SimpleAmbiguityProcessorToolCfg( flags,
                                                                                        ClusterSplitProbContainer=ClusterSplitProbContainer))
    
    #
    # --- configure Ambiguity solver
    #
    kwargs.setdefault("TrackInput", 'ScoredMap'+'InDetAmbiguityScore'+ flags.InDet.Tracking.extension)
    kwargs.setdefault("TrackOutput", SiTrackCollection)
    kwargs.setdefault( "AmbiguityProcessor", InDetAmbiguityProcessor)

    InDetAmbiguitySolver = CompFactory.Trk.TrkAmbiguitySolver(name = name+flags.InDet.Tracking.extension, **kwargs)
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
    # --- decide if use the association tool
    #
    if (len(InputCollections) > 0) and (flags.InDet.Tracking.extension == "LowPt" or flags.InDet.Tracking.extension == "VeryLowPt" or flags.InDet.Tracking.extension == "LargeD0" or flags.InDet.Tracking.extension == "R3LargeD0" or flags.InDet.Tracking.extension == "LowPtLargeD0" or flags.InDet.Tracking.extension == "BeamGas" or flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "Disappearing"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False
    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    if usePrdAssociationTool:
        prefix = 'InDet'
        suffix = flags.InDet.Tracking.extension
        acc.merge(TC.InDetTrackPRD_AssociationCfg(flags,namePrefix = prefix,
                                                        nameSuffix = suffix,
                                                        TracksName = list(InputCollections)))

    # ------------------------------------------------------------
    #
    # ----------- SiSPSeededTrackFinder
    #
    # ------------------------------------------------------------
    
    if flags.InDet.doSiSPSeededTrackFinder:
        acc.merge(SiSPSeededTrackFinderCfg( flags,
                                            InputCollections = InputCollections, 
                                            SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey))
    # ------------------------------------------------------------
    #
    # ---------- Ambiguity solving
    #
    # ------------------------------------------------------------
    if True: #flags.InDet.doAmbiSolving: is this flag needed
        #
        # --- load InnerDetector TrackSelectionTool
        #
        acc.merge(TrkAmbiguityScoreCfg( flags,
                                        SiSPSeededTrackCollectionKey=SiSPSeededTrackCollectionKey))

        acc.merge(TrkAmbiguitySolverCfg(flags,
                                        ResolvedTrackCollectionKey=ResolvedTrackCollectionKey))

    return acc


