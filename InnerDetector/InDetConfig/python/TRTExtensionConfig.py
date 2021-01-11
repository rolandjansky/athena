# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
import InDetConfig.TrackingCommonConfig         as   TC
import AthenaCommon.SystemOfUnits               as   Units

#///////////// Temporary location TrackingSiPatternConfig configurations ///////////////////////
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

    InDetPatternPropagator = TC.InDetPatternPropagatorCfg()
    acc.addPublicTool(InDetPatternPropagator)

    InDetPatternUpdator = TC.InDetPatternUpdatorCfg()
    acc.addPublicTool(InDetPatternUpdator)

    from  InDetConfig.InDetRecToolConfig import InDetBoundaryCheckToolCfg
    boundary_check_tool = acc.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags))
    acc.addPublicTool(boundary_check_tool)

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("UpdatorTool", InDetPatternUpdator)
    kwargs.setdefault("BoundaryCheckTool", boundary_check_tool)
    kwargs.setdefault("RIOonTrackTool", rot_creator_digital)
    kwargs.setdefault("usePixel", flags.Detector.RecoPixel)
    kwargs.setdefault("useSCT", flags.Detector.RecoSCT if not is_dbm else False)
    kwargs.setdefault("PixelClusterContainer", 'PixelClusters') # InDetKeys.PixelClusters()
    kwargs.setdefault("SCT_ClusterContainer", 'SCT_Clusters') # InDetKeys.SCT_Clusters()

    if flags.InDet.Tracking.extension == "Offline": 
        kwargs.setdefault("writeHolesFromPattern", flags.InDet.useHolesFromPattern)

    if is_dbm :
        kwargs.setdefault("MagneticFieldMode", "NoField")
        kwargs.setdefault("TrackQualityCut", 9.3)

    if flags.Detector.RecoSCT:
        InDetSCT_ConditionsSummaryTool = CompFactory.SCT_ConditionsSummaryTool(name = 'InDetSCT_ConditionsSummaryTool')
        acc.addPublicTool(InDetSCT_ConditionsSummaryTool)
        kwargs.setdefault("SctSummaryTool", InDetSCT_ConditionsSummaryTool)
    else:
        kwargs.setdefault("SctSummaryTool", None)

    track_finder = CompFactory.InDet.SiCombinatorialTrackFinder_xk(name = name+flags.InDet.Tracking.extension, **kwargs)
    acc.setPrivateTools(track_finder)
    return acc

def SiDetElementsRoadMaker_xkCfg(flags, name="InDetSiRoadMaker", **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- SCT and Pixel detector elements road builder
    #
    InDetPatternPropagator = TC.InDetPatternPropagatorCfg()
    acc.addPublicTool(InDetPatternPropagator)

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("usePixel", flags.InDet.Tracking.usePixel )
    kwargs.setdefault("PixManagerLocation", 'Pixel')
    kwargs.setdefault("useSCT", flags.InDet.Tracking.useSCT)
    kwargs.setdefault("SCTManagerLocation", 'SCT')
    kwargs.setdefault("RoadWidth", flags.InDet.Tracking.roadWidth)

    InDetSiDetElementsRoadMaker = CompFactory.InDet.SiDetElementsRoadMaker_xk(name = name+flags.InDet.Tracking.extension, **kwargs)
    acc.setPrivateTools(InDetSiDetElementsRoadMaker)
    return acc

def SiTrackMaker_xkCfg(flags, name="InDetSiTrackMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()
    useBremMode = flags.InDet.Tracking.extension == "Offline" or flags.InDet.Tracking.extension == "SLHC" or flags.InDet.Tracking.extension == "DBM"
    InDetSiDetElementsRoadMaker = acc.popToolsAndMerge(SiDetElementsRoadMaker_xkCfg(flags))
    acc.addPublicTool(InDetSiDetElementsRoadMaker)
    if flags.Detector.RecoPixel:
        acc.addCondAlgo( CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk( name = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                                ReadKey  = "PixelDetectorElementCollection",
                                                                                WriteKey = "PixelDetElementBoundaryLinks_xk") )
    if flags.InDet.Tracking.useSCT:
        acc.addCondAlgo(CompFactory.InDet.SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk"))

        acc.addCondAlgo( CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk( name = "InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                                ReadKey  = "SCT_DetectorElementCollection",
                                                                                WriteKey = "SCT_DetElementBoundaryLinks_xk") )


    track_finder = acc.popToolsAndMerge(SiCombinatorialTrackFinder_xkCfg(flags))
    acc.addPublicTool(track_finder)

    #
    # --- decide if use the association tool
    #
    if (len(InputCollections) > 0) and (flags.InDet.Tracking.extension == "LowPt" or flags.InDet.Tracking.extension == "VeryLowPt" or flags.InDet.Tracking.extension == "LargeD0" or flags.InDet.Tracking.extension == "LowPtLargeD0" or flags.InDet.Tracking.extension == "BeamGas" or flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "ForwardSLHCTracks"  or flags.InDet.Tracking.extension == "Disappearing" or flags.InDet.Tracking.extension == "VeryForwardSLHCTracks" or flags.InDet.Tracking.extension == "SLHCConversionFinding"):
        usePrdAssociationTool = True
    else:
        usePrdAssociationTool = False

    kwargs.setdefault("useSCT", flags.InDet.Tracking.useSCT)  #flags.InDet.Tracking.useSCT()
    kwargs.setdefault("usePixel", flags.InDet.Tracking.usePixel) #flags.InDet.Tracking.usePixel()
    kwargs.setdefault("RoadTool", InDetSiDetElementsRoadMaker)
    kwargs.setdefault("CombinatorialTrackFinder", track_finder)
    kwargs.setdefault("pTmin", flags.InDet.Tracking.minPT)
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.minPTBrem)
    kwargs.setdefault("pTminSSS", flags.InDet.pT_SSScut)
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
    kwargs.setdefault("useBremModel", flags.InDet.doBremRecovery and useBremMode) # only for NewTracking the brem is debugged !!!
    kwargs.setdefault("doCaloSeededBrem", flags.InDet.doCaloSeededBrem)
    kwargs.setdefault("doHadCaloSeedSSS", flags.InDet.doHadCaloSeededSSS)
    kwargs.setdefault("phiWidth", flags.InDet.Tracking.phiWidthBrem)
    kwargs.setdefault("etaWidth", flags.InDet.Tracking.etaWidthBrem)
    kwargs.setdefault("InputClusterContainerName", 'InDetCaloClusterROIs') # InDetKeys.CaloClusterROIContainer()
    kwargs.setdefault("InputHadClusterContainerName", 'InDetHadCaloClusterROIs') # InDetKeys.HadCaloClusterROIContainer()
    kwargs.setdefault("UseAssociationTool", usePrdAssociationTool)

    if flags.InDet.Tracking.extension == "SLHC" or flags.InDet.Tracking.extension == "ForwardSLHCTracks" or flags.InDet.Tracking.extension == "VeryForwardSLHCTracks" :
        kwargs.setdefault("ITKGeometry", True)

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

    elif flags.InDet.Tracking.extension == "ForwardSLHCTracks":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_ForwardSLHCTracks')

    elif flags.InDet.Tracking.extension == "VeryForwardSLHCTracks":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_VeryForwardSLHCTracks')

    elif flags.InDet.Tracking.extension == "SLHCConversionFinding":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_SLHCConversionTracks')

    elif flags.InDet.Tracking.extension == "LargeD0" or flags.InDet.Tracking.extension == "LowPtLargeD0":
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSpacePointsSeedMaker_LargeD0')

    else:
        kwargs.setdefault("TrackPatternRecoInfo", 'SiSPSeededFinder')
            
    if flags.InDet.doStoreTrackSeeds:
        InDet_SeedToTrackConversion = CompFactory.InDet.SeedToTrackConversionTool(  name = "InDet_SeedToTrackConversion",
                                                                                    OutputName = 'SiSPSeedSegments' + flags.InDet.Tracking.extension)
        acc.addPublicTool(InDet_SeedToTrackConversion)
        kwargs.setdefault("SeedToTrackConversion", InDet_SeedToTrackConversion)
        kwargs.setdefault("SeedSegmentsWrite", True)

    InDetSiTrackMaker = CompFactory.InDet.SiTrackMaker_xk(name = name+flags.InDet.Tracking.extension, **kwargs)
    acc.setPrivateTools(InDetSiTrackMaker)
    return acc

def SiSpacePointsSeedMakerCfg(flags, name="InDetSpSeedsMaker", InputCollections = None, **kwargs) :
    acc = ComponentAccumulator()
    #
    # --- decide if use the association tool
    #
    if (len(InputCollections) > 0) and (flags.InDet.Tracking.extension == "LowPt" or flags.InDet.Tracking.extension == "VeryLowPt" or flags.InDet.Tracking.extension == "LargeD0" or flags.InDet.Tracking.extension == "LowPtLargeD0" or flags.InDet.Tracking.extension == "BeamGas" or flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "ForwardSLHCTracks"  or flags.InDet.Tracking.extension == "Disappearing" or flags.InDet.Tracking.extension == "VeryForwardSLHCTracks" or flags.InDet.Tracking.extension == "SLHCConversionFinding"):
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
    elif flags.InDet.Tracking.extension == "SLHC" or flags.InDet.Tracking.extension == "ForwardSLHCTracks" or flags.InDet.Tracking.extension == "VeryForwardSLHCTracks" :
        SiSpacePointsSeedMaker = CompFactory.InDet.SiSpacePointsSeedMaker_ITK
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


    if flags.InDet.Tracking.extension == "Offline" or flags.InDet.doHeavyIon or  flags.InDet.Tracking.extension == "ForwardTracks":
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
    if flags.InDet.Tracking.extension == "SLHC" or flags.InDet.Tracking.extension == "SLHCConversionFinding":
        kwargs.setdefault("minRadius1", 0)
        kwargs.setdefault("minRadius2", 0)
        kwargs.setdefault("minRadius3", 0)
        kwargs.setdefault("maxRadius1", 1000.*Units.mm)
        kwargs.setdefault("maxRadius2", 1000.*Units.mm)
        kwargs.setdefault("maxRadius3", 1000.*Units.mm)
    if flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "ForwardSLHCTracks" or flags.InDet.Tracking.extension == "VeryForwardSLHCTracks":
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
                                                                                 InputCollections = InputCollections))
    acc.addPublicTool(InDetSiSpacePointsSeedMaker)

    kwargs.setdefault("SeedMakerTool", InDetSiSpacePointsSeedMaker)
    if flags.InDet.doHeavyIon:
        kwargs.setdefault("HistSize", 2000)
        kwargs.setdefault("minContent", 30)

    InDetZvertexMaker = CompFactory.InDet.SiZvertexMaker_xk(name = name+flags.InDet.Tracking.extension, **kwargs)
    acc.setPrivateTools(InDetZvertexMaker)
    return acc

def SiSPSeededTrackFinderCfg(flags, name="InDetSiSpTrackFinder", InputCollections = None, SiSPSeededTrackCollectionKey = None, **kwargs) :
    acc = ComponentAccumulator()

    # set output track collection name
    #
    SiTrackCollection = SiSPSeededTrackCollectionKey
    #
    # --- decide if use the association tool
    #
    if (len(InputCollections) > 0) and (flags.InDet.Tracking.extension == "LowPt" or flags.InDet.Tracking.extension == "VeryLowPt" or flags.InDet.Tracking.extension == "LargeD0" or flags.InDet.Tracking.extension == "LowPtLargeD0" or flags.InDet.Tracking.extension == "BeamGas" or flags.InDet.Tracking.extension == "ForwardTracks" or flags.InDet.Tracking.extension == "ForwardSLHCTracks"  or flags.InDet.Tracking.extension == "Disappearing" or flags.InDet.Tracking.extension == "VeryForwardSLHCTracks" or flags.InDet.Tracking.extension == "SLHCConversionFinding"):
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
                                                                InputCollections = InputCollections))
    acc.addPublicTool(InDetSiTrackMaker)
 
    InDetTrackSummaryToolNoHoleSearch = acc.popToolsAndMerge(TC.InDetTrackSummaryToolNoHoleSearchCfg(flags))
    acc.addPublicTool(InDetTrackSummaryToolNoHoleSearch)

    InDetSiSpacePointsSeedMaker = acc.popToolsAndMerge(SiSpacePointsSeedMakerCfg(flags, 
                                                                                 InputCollections = InputCollections))
    acc.addPublicTool(InDetSiSpacePointsSeedMaker)

    #
    # --- Z-coordinates primary vertices finder (only for collisions)
    #
    if flags.InDet.useZvertexTool and flags.InDet.Tracking.extension != "DBM":
        InDetZvertexMaker = acc.popToolsAndMerge(SiZvertexMaker_xkCfg(flags,
                                                 InputCollections = InputCollections))
        acc.addPublicTool(InDetZvertexMaker)
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

    if flags.InDet.Tracking.extension == "ForwardSLHCTracks" or flags.InDet.Tracking.extension == "ForwardTracks":
        kwargs.setdefault("useZvertexTool", flags.InDet.useZvertexTool)
        kwargs.setdefault("useNewStrategy", False)
        kwargs.setdefault("useZBoundFinding", False)
    else:
        kwargs.setdefault("useZvertexTool", flags.InDet.useZvertexTool and flags.InDet.Tracking.extension != "DBM")
        kwargs.setdefault("useNewStrategy", flags.InDet.useNewSiSPSeededTF and flags.InDet.Tracking.extension != "DBM")
        kwargs.setdefault("useZBoundFinding", flags.InDet.Tracking.doZBoundary and flags.InDet.Tracking.extension != "DBM")
    
    if flags.InDet.doHeavyIon :
        kwargs.setdefault("FreeClustersCut",2) #Heavy Ion optimization from Igor

    if flags.InDet.Tracking.extension == "Offline":
        kwargs.setdefault("writeHolesFromPattern", flags.InDet.useHolesFromPattern)

    InDetSiSPSeededTrackFinder = CompFactory.InDet.SiSPSeededTrackFinder(name = name+flags.InDet.Tracking.extension, **kwargs)
    acc.addEventAlgo(InDetSiSPSeededTrackFinder)
    return acc
#///////////////////////////////////////////////////////////////////////////////////////////////
def TRT_TrackExtensionAlgCfg(flags, name = 'InDetTRT_ExtensionPhase', SiTrackCollection=None, ExtendedTracksMap =None, **kwargs):
    acc = ComponentAccumulator()
    # set output extension map name
    OutputExtendedTracks = ExtendedTracksMap

    InDetTRTExtensionToolPhase = acc.popToolsAndMerge(TC.InDetTRT_ExtensionToolPhaseCfg(flags))
    acc.addPublicTool(InDetTRTExtensionToolPhase)

    kwargs.setdefault("InputTracksLocation", SiTrackCollection)
    kwargs.setdefault("ExtendedTracksLocation", OutputExtendedTracks )
    kwargs.setdefault("TrackExtensionTool", InDetTRTExtensionToolPhase)

    acc.addEventAlgo(CompFactory.InDet.TRT_TrackExtensionAlg(name = name, **kwargs))
    return acc

def CompetingRIOsOnTrackToolCfg(flags, name = 'InDetCompetingRotCreator', **kwargs):
    acc = ComponentAccumulator()
    InDetCompetingTRT_DC_Tool = acc.popToolsAndMerge(TC.InDetCompetingTRT_DC_ToolCfg(flags))
    acc.addPublicTool(InDetCompetingTRT_DC_Tool)

    kwargs.setdefault("ToolForCompPixelClusters", None)
    kwargs.setdefault("ToolForCompSCT_Clusters", None)
    kwargs.setdefault("ToolForCompTRT_DriftCircles", InDetCompetingTRT_DC_Tool)
    acc.setPrivateTools(CompFactory.Trk.CompetingRIOsOnTrackTool(name = name, **kwargs))
    return acc

def DeterministicAnnealingFilterCfg(flags, name = 'InDetDAF', **kwargs):
    acc = ComponentAccumulator()
    InDetCompetingRotCreator = acc.popToolsAndMerge(CompetingRIOsOnTrackToolCfg(flags))
    acc.addPublicTool(InDetCompetingRotCreator)

    from InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
    tmpAcc =  InDetExtrapolatorCfg(flags)
    InDetExtrapolator = tmpAcc.getPrimary()
    acc.merge(tmpAcc)

    from InDetConfig.TrackingCommonConfig import InDetUpdatorCfg
    InDetUpdator = InDetUpdatorCfg(flags)
    acc.addPublicTool(InDetUpdator)

    kwargs.setdefault("ToolForExtrapolation", InDetExtrapolator)
    kwargs.setdefault("ToolForCompetingROTsCreation", InDetCompetingRotCreator)
    kwargs.setdefault("ToolForUpdating", InDetUpdator)
    kwargs.setdefault("AnnealingScheme", [200., 81., 9., 4., 1., 1., 1.])
    kwargs.setdefault("DropOutlierCutValue", 1.E-7)
    kwargs.setdefault("OutlierCutValue", 0.01)
    acc.setPrivateTools(CompFactory.Trk.DeterministicAnnealingFilter(name = name, **kwargs))
    return acc

def InDetExtensionProcessorCfg(flags, SiTrackCollection=None, ExtendedTrackCollection = None, ExtendedTracksMap = None, doPhase=True, **kwargs):
    acc = ComponentAccumulator()

    ForwardTrackCollection = ExtendedTrackCollection
    # set output extension map name
    OutputExtendedTracks = ExtendedTracksMap

    if flags.InDet.trtExtensionType == 'DAF' :
        #
        # --- DAF Fitter setup
        #
        InDetExtensionFitter = acc.popToolsAndMerge(DeterministicAnnealingFilterCfg(flags, name = 'InDetDAF'+ flags.InDet.Tracking.extension))
        acc.addPublicTool(InDetExtensionFitter)
    else:
        fitter_args = {}
        if flags.InDet.holeSearchInGX2Fit:
            fitter_args.setdefault("DoHoleSearch", True)
            from  InDetConfig.InDetRecToolConfig import InDetBoundaryCheckToolCfg
            InDetBoundaryCheckTool = acc.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags))
            acc.addPublicTool(InDetBoundaryCheckTool)
            fitter_args.setdefault("BoundaryCheckTool", InDetBoundaryCheckTool)
        if flags.InDet.Tracking.extension != "LowPt":
            InDetExtensionFitter = acc.popToolsAndMerge(TC.InDetTrackFitterCfg(flags, 'InDetTrackFitter_TRTExtension'+flags.InDet.Tracking.extension, **fitter_args))
            acc.addPublicTool(InDetExtensionFitter)
        else:
            InDetExtensionFitter = acc.popToolsAndMerge(TC.InDetTrackFitterLowPt(flags, 'InDetTrackFitter_TRTExtension'+flags.InDet.Tracking.extension, **fitter_args))
            acc.addPublicTool(InDetExtensionFitter)
    #
    # --- load scoring for extension
    #
    if flags.Beam.Type == "cosmics":
        InDetExtenScoringTool = acc.popToolsAndMerge(TC.InDetCosmicExtenScoringToolCfg(flags))
        acc.addPublicTool(InDetExtenScoringTool)
    else:
        InDetExtenScoringTool = acc.popToolsAndMerge(TC.InDetExtenScoringToolCfg(flags))
        acc.addPublicTool(InDetExtenScoringTool)
    #
    # --- get configured track extension processor
    #
    InDetTrackSummaryTool = acc.popToolsAndMerge(TC.InDetTrackSummaryToolCfg(flags))
    acc.addPublicTool(InDetTrackSummaryTool)

    if flags.InDet.materialInteractions:
        kwargs.setdefault("matEffects", flags.InDet.materialInteractionsType)
    else:
        kwargs.setdefault("matEffects", 0)

    kwargs.setdefault("TrackName", SiTrackCollection)
    kwargs.setdefault("ExtensionMap", OutputExtendedTracks)
    kwargs.setdefault("NewTrackName", ForwardTrackCollection)
    kwargs.setdefault("TrackFitter", InDetExtensionFitter)
    kwargs.setdefault("TrackSummaryTool", InDetTrackSummaryTool)
    kwargs.setdefault("ScoringTool", InDetExtenScoringTool)
    kwargs.setdefault("suppressHoleSearch", False)
    kwargs.setdefault("tryBremFit", flags.InDet.doBremRecovery)
    kwargs.setdefault("caloSeededBrem", flags.InDet.doCaloSeededBrem)
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.minPTBrem)
    kwargs.setdefault("RefitPrds", not (flags.InDet.refitROT or (flags.InDet.trtExtensionType == 'DAF')))
    
    if doPhase:
        kwargs.setdefault("Cosmics", True)

        acc.addEventAlgo(CompFactory.InDet.InDetExtensionProcessor(name = "InDetExtensionProcessorPhase" + flags.InDet.Tracking.extension, **kwargs))
    else:
        kwargs.setdefault("Cosmics", flags.Beam.Type == "cosmics")

        acc.addEventAlgo(CompFactory.InDet.InDetExtensionProcessor("InDetExtensionProcessor" + flags.InDet.Tracking.extension, **kwargs))

    return acc

##########################################################################################################################
# ------------------------------------------------------------
#
# ----------- Setup TRT Extension for New tracking
#
# ------------------------------------------------------------
def NewTrackingTRTExtensionCfg(flags, SiTrackCollection = None, ExtendedTrackCollection = None, ExtendedTracksMap = None, doPhase = True):
    acc = ComponentAccumulator()
    #
    # ---------- TRT_TrackExtension
    #
    if flags.InDet.doTRTExtension and flags.InDet.doTRTExtensionNew:
        #
        # Track extension to TRT algorithm
        #
        if doPhase:
            acc.merge(TRT_TrackExtensionAlgCfg( flags,
                                                name = 'InDetTRT_ExtensionPhase' + flags.InDet.Tracking.extension,
                                                SiTrackCollection=SiTrackCollection,
                                                ExtendedTracksMap = ExtendedTracksMap))
        else:
            acc.merge(TRT_TrackExtensionAlgCfg( flags,  
                                                name = 'InDetTRT_Extension' + flags.InDet.Tracking.extension,
                                                SiTrackCollection=SiTrackCollection,
                                                ExtendedTracksMap = ExtendedTracksMap,
                                                TrackExtensionTool = acc.popToolsAndMerge(TC.InDetTRT_ExtensionToolCfg(flags))))
    #
    # ------------ Track Extension Processor
    #
    if flags.InDet.doExtensionProcessor and flags.InDet.doTRTExtensionNew:
        acc.merge(InDetExtensionProcessorCfg(flags,
                                             SiTrackCollection = SiTrackCollection,
                                             ExtendedTrackCollection = ExtendedTrackCollection,
                                             ExtendedTracksMap = ExtendedTracksMap,
                                             doPhase = doPhase))
    return acc
##########################################################################################################################

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads

    ConfigFlags.Detector.GeometryPixel = True 
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.Detector.GeometryTRT   = True

    ConfigFlags.Detector.RecoIBL = True
    ConfigFlags.Detector.RecoPixel = True
    ConfigFlags.Detector.RecoTRT = True
    ConfigFlags.Detector.RecoSCT = True

    ConfigFlags.addFlag('InDet.doTRTExtension', True)
    ConfigFlags.addFlag('InDet.doExtensionProcessor', True)
    ConfigFlags.addFlag('InDet.useHolesFromPattern', False)
    ConfigFlags.addFlag('InDet.holeSearchInGX2Fit', True)

    # SiliconPreProcessing
    ConfigFlags.InDet.doPixelClusterSplitting = True
    ConfigFlags.InDet.doSiSPSeededTrackFinder = True

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    top_acc.merge(PixelGeometryCfg(ConfigFlags))
    top_acc.merge(SCT_GeometryCfg(ConfigFlags))

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    top_acc.merge(TRT_GeometryCfg( ConfigFlags ))

    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg, MuonIdHelperSvcCfg
    top_acc.merge(MuonGeoModelCfg(ConfigFlags))
    top_acc.merge(MuonIdHelperSvcCfg(ConfigFlags))

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    top_acc.merge(BeamSpotCondAlgCfg(ConfigFlags))

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDistortionAlgCfg
    top_acc.merge(PixelDistortionAlgCfg(ConfigFlags))

    from InDetConfig.TRTSegmentFindingConfig import TRTActiveCondAlgCfg

    top_acc.merge(TRTActiveCondAlgCfg(ConfigFlags))
    top_acc.merge(TC.TRT_DetElementsRoadCondAlgCfg())

    from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool, PixelLorentzAngleCfg
    top_acc.addPublicTool(PixelLorentzAngleTool(ConfigFlags))
    top_acc.addPublicTool(top_acc.popToolsAndMerge(PixelLorentzAngleCfg(ConfigFlags)))

    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    top_acc.addPublicTool(top_acc.popToolsAndMerge(SCT_LorentzAngleCfg(ConfigFlags)))

    top_acc.merge(TC.PixelClusterNnCondAlgCfg(ConfigFlags))
    top_acc.merge(TC.PixelClusterNnWithTrackCondAlgCfg(ConfigFlags))

    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    top_acc.merge(BeamPipeGeometryCfg(ConfigFlags))

    InputCollections = []
    
    InDetSpSeededTracksKey    = 'SiSPSeededTracks'  # InDetKeys.SiSpSeededTracks()
    SiSPSeededTrackCollectionKey = InDetSpSeededTracksKey

    ExtendedTrackCollection = 'ExtendedTracksPhase' # InDetKeys.ExtendedTracksPhase
    ExtendedTracksMap = 'ExtendedTracksMapPhase'    # InDetKeys.ExtendedTracksMapPhase

    #################### Additional Configuration  ########################
    #######################################################################
    ################# TRTPreProcessing Configuration ######################
    from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
    if not ConfigFlags.InDet.doDBMstandalone:
        top_acc.merge(TRTPreProcessingCfg(ConfigFlags,(not ConfigFlags.InDet.doTRTPhaseCalculation or ConfigFlags.Beam.Type =="collisions"),False))

    ################ TRTSegmentFinding Configuration ######################
    from InDetConfig.TRTSegmentFindingConfig import TRTSegmentFindingCfg
    top_acc.merge(TRTSegmentFindingCfg( ConfigFlags,
                                        extension = "",
                                        InputCollections = InputCollections,
                                        BarrelSegments = 'TRTSegments', # InDetKeys.TRT_Segments
                                        doPhase = False))

    ####################### TrackingSiPattern #############################
    if ConfigFlags.InDet.doSiSPSeededTrackFinder:
        top_acc.merge(SiSPSeededTrackFinderCfg( ConfigFlags,
                                                InputCollections = InputCollections, 
                                                SiSPSeededTrackCollectionKey = InDetSpSeededTracksKey))

    ########################## Clusterization #############################
    from InDetConfig.ClusterizationConfig import InDetClusterizationAlgorithmsCfg
    top_acc.merge(InDetClusterizationAlgorithmsCfg(ConfigFlags))

    ######################## PixelByteStreamErrs ##########################
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelHitDiscCnfgAlgCfg
    top_acc.merge(PixelHitDiscCnfgAlgCfg(ConfigFlags))

    from InDetOverlay.PixelOverlayConfig import PixelRawDataProviderAlgCfg
    top_acc.merge(PixelRawDataProviderAlgCfg(ConfigFlags))
    ########################### TRTExtension  #############################
    top_acc.merge(NewTrackingTRTExtensionCfg(ConfigFlags,
                                             SiTrackCollection=InDetSpSeededTracksKey,
                                             ExtendedTrackCollection = ExtendedTrackCollection, 
                                             ExtendedTracksMap = ExtendedTracksMap,
                                             doPhase = False))
    #######################################################################

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5
    #
    top_acc.printConfig()
    top_acc.run(25)
    top_acc.store(open("test_TRTExtensionConfig.pkl", "wb"))