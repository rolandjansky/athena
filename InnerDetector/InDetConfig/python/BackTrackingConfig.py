# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
import InDetConfig.TrackingCommonConfig         as   TC

def SiDetElementsRoadMaker_xkCfg(flags, name = 'InDetTRT_SeededSiRoad', **kwargs):
    acc = ComponentAccumulator()
    #
    # Silicon det elements road maker tool
    #
    InDetPatternPropagator = acc.getPrimaryAndMerge(TC.InDetPatternPropagatorCfg())

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel)
    kwargs.setdefault("PixManagerLocation", 'Pixel') # InDetKeys.PixelManager()
    kwargs.setdefault("useSCT", flags.InDet.Tracking.ActivePass.useSCT)
    kwargs.setdefault("SCTManagerLocation", 'SCT') # InDetKeys.SCT_Manager()
    kwargs.setdefault("RoadWidth", 35.)
    kwargs.setdefault("MaxStep", 20.)

    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("RoadWidth", 50)
    # Condition algorithm for InDet__SiDetElementsRoadMaker_xk
    if flags.InDet.Tracking.ActivePass.useSCT:
        acc.addCondAlgo(CompFactory.InDet.SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk"))

    InDetTRT_SeededSiRoadMaker = CompFactory.InDet.SiDetElementsRoadMaker_xk(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededSiRoadMaker)
    return acc

def SimpleTRT_SeededSpacePointFinder_ATLCfg(flags, name='InDetTRT_SeededSpFinder', InputCollections=[], **kwargs):
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    acc = MagneticFieldSvcCfg(flags)

    #
    # --- decide if use the association tool
    #
    if len(InputCollections) > 0:
        usePrdAssociationTool = True
        prefix = 'InDetSegment'
        suffix = ''
    else:
        prefix = ''
        suffix = ''
        usePrdAssociationTool = False

    kwargs.setdefault("SpacePointsSCTName", 'SCT_SpacePoints')
    kwargs.setdefault("PRDtoTrackMap", prefix+'PRDtoTrackMap'+suffix if usePrdAssociationTool else "")
    kwargs.setdefault("PerigeeCut", 1000.)
    kwargs.setdefault("DirectionPhiCut", .3)
    kwargs.setdefault("DirectionEtaCut", 1.)
    kwargs.setdefault("MaxHoles", 2)
    kwargs.setdefault("RestrictROI", True)

    InDetTRT_SeededSpacePointFinder = CompFactory.InDet.SimpleTRT_SeededSpacePointFinder_ATL(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededSpacePointFinder)
    return acc

def TRT_SeededSpacePointFinder_ATLCfg(flags, name='InDetTRT_SeededSpFinder', InputCollections=[], **kwargs):
    acc = ComponentAccumulator()
    #
    # --- decide if use the association tool
    #
    if len(InputCollections) > 0:
        usePrdAssociationTool = True
        prefix = 'InDetSegment'
        suffix = ''
    else:
        prefix = ''
        suffix = ''
        usePrdAssociationTool = False
    #
    # --- defaul space point finder
    #
    kwargs.setdefault("SpacePointsSCTName", 'SCT_SpacePoints') # InDetKeys.SCT_SpacePoints()
    kwargs.setdefault("SpacePointsOverlapName", 'OverlapSpacePoints') # InDetKeys.OverlapSpacePoints())
    kwargs.setdefault("PRDtoTrackMap", prefix+'PRDtoTrackMap'+suffix if usePrdAssociationTool else "")
    kwargs.setdefault("NeighborSearch", True)
    kwargs.setdefault("LoadFull", False)
    kwargs.setdefault("DoCosmics", flags.Beam.Type == "cosmics")
    kwargs.setdefault("pTmin", flags.InDet.Tracking.ActivePass.minSecondaryPt)

    InDetTRT_SeededSpacePointFinder = CompFactory.InDet.TRT_SeededSpacePointFinder_ATL(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededSpacePointFinder)
    return acc

def TRT_SeededTrackFinder_ATLCfg(flags, name='InDetTRT_SeededTrackMaker', InputCollections=[], **kwargs):
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    acc = MagneticFieldSvcCfg(flags)

    #
    # --- TRT seeded back tracking tool
    #
    InDetPatternPropagator = acc.getPrimaryAndMerge(TC.InDetPatternPropagatorCfg())
    InDetPatternUpdator = acc.getPrimaryAndMerge(TC.InDetPatternUpdatorCfg())

    InDetSiComTrackFinder = acc.popToolsAndMerge(TC.SiCombinatorialTrackFinder_xkCfg(flags))

    if flags.InDet.Tracking.ActivePass.usePixel and flags.InDet.Tracking.ActivePass.useSCT:
        InDetTRT_SeededSiRoadMaker = acc.popToolsAndMerge(SiDetElementsRoadMaker_xkCfg(flags))
        acc.addPublicTool(InDetTRT_SeededSiRoadMaker)
        kwargs.setdefault("RoadTool", InDetTRT_SeededSiRoadMaker)

    #
    # --- decide which TRT seed space point finder to use
    #
    InDetTRT_SeededSpacePointFinder = acc.popToolsAndMerge(TRT_SeededSpacePointFinder_ATLCfg(flags, InputCollections=InputCollections))
    acc.addPublicTool(InDetTRT_SeededSpacePointFinder)

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("UpdatorTool", InDetPatternUpdator)
    kwargs.setdefault("SeedTool", InDetTRT_SeededSpacePointFinder)
    kwargs.setdefault("CombinatorialTrackFinder", InDetSiComTrackFinder)
    kwargs.setdefault("pTmin", flags.InDet.Tracking.ActivePass.minSecondaryPt)
    kwargs.setdefault("nHolesMax", flags.InDet.Tracking.ActivePass.SecondarynHolesMax)
    kwargs.setdefault("nHolesGapMax", flags.InDet.Tracking.ActivePass.SecondarynHolesGapMax)
    kwargs.setdefault("Xi2max", flags.InDet.Tracking.ActivePass.SecondaryXi2max)
    kwargs.setdefault("Xi2maxNoAdd", flags.InDet.Tracking.ActivePass.SecondaryXi2maxNoAdd)
    kwargs.setdefault("SearchInCaloROI", False)
    if kwargs["SearchInCaloROI"]:
        from InDetConfig.InDetRecCaloSeededROISelectionConfig import CaloClusterROI_SelectorCfg
        acc.merge(CaloClusterROI_SelectorCfg(flags))
        kwargs.setdefault("InputClusterContainerName", "InDetCaloClusterROIs") # InDetKeys.CaloClusterROIContainer()
    else:
        kwargs.setdefault("InputClusterContainerName", "")
    kwargs.setdefault("ConsistentSeeds", True)
    kwargs.setdefault("BremCorrection", False)

    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("nWClustersMin", 0)

    InDetTRT_SeededTrackTool = CompFactory.InDet.TRT_SeededTrackFinder_ATL(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededTrackTool)
    return acc

def TRT_SeededTrackFinderCfg(flags, name='InDetTRT_SeededTrackFinder', InputCollections=[], **kwargs):
    acc = ComponentAccumulator()

    #
    # --- Output key for the finder
    #
    TRTSeededTracks = 'TRTSeededTracks' # InDetKeys.TRTSeededTracks()
    #
    # TRT seeded back tracking algorithm

    #
    # --- decide if use the association tool
    #
    if len(InputCollections) > 0:
        usePrdAssociationTool = True
        prefix = 'InDetSegment'
        suffix = ''
    else:
        prefix = ''
        suffix = ''
        usePrdAssociationTool = False

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    acc.merge(BeamSpotCondAlgCfg(flags))

    if flags.InDet.Tracking.ActivePass.usePixel:
        acc.addCondAlgo(CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk(name="InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                              ReadKey="PixelDetectorElementCollection",
                                                                              WriteKey="PixelDetElementBoundaryLinks_xk"))
    if flags.InDet.Tracking.ActivePass.useSCT:
        acc.addCondAlgo(CompFactory.InDet.SiDetElementsRoadCondAlg_xk(name="InDet__SiDetElementsRoadCondAlg_xk"))

        acc.addCondAlgo(CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk(name="InDetSiDetElementBoundaryLinksSCTCondAlg",
                                                                              ReadKey="SCT_DetectorElementCollection",
                                                                              WriteKey="SCT_DetElementBoundaryLinks_xk"))

    InDetTrackFitterBT = acc.popToolsAndMerge(TC.InDetTrackFitterBTCfg(flags))

    InDetTrackSummaryToolNoHoleSearch = acc.getPrimaryAndMerge(TC.InDetTrackSummaryToolNoHoleSearchCfg(flags))

    InDetTRTExtensionTool = acc.popToolsAndMerge(TC.InDetTRT_ExtensionToolCfg(flags))
    acc.addPublicTool(InDetTRTExtensionTool)

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))

    InDetTRT_SeededTrackTool = acc.popToolsAndMerge(TRT_SeededTrackFinder_ATLCfg(flags, 
                                                                                 InputCollections=InputCollections))
    acc.addPublicTool(InDetTRT_SeededTrackTool)

    kwargs.setdefault("RefitterTool", InDetTrackFitterBT)
    kwargs.setdefault("TrackTool", InDetTRT_SeededTrackTool)
    kwargs.setdefault("PRDtoTrackMap", prefix+'PRDtoTrackMap'+suffix if usePrdAssociationTool else "")
    kwargs.setdefault("TrackSummaryTool", InDetTrackSummaryToolNoHoleSearch)
    kwargs.setdefault("TrackExtensionTool", InDetTRTExtensionTool)
    kwargs.setdefault("MinTRTonSegment", flags.InDet.Tracking.ActivePass.minSecondaryTRTonTrk)
    kwargs.setdefault("MinTRTonly", flags.InDet.Tracking.ActivePass.minTRTonly)
    kwargs.setdefault("TrtExtension", True)
    kwargs.setdefault("SiExtensionCuts", flags.InDet.Tracking.ActivePass.SiExtensionCuts)
    kwargs.setdefault("minPt", flags.InDet.Tracking.ActivePass.minSecondaryPt)
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.ActivePass.maxSecondaryImpact)
    kwargs.setdefault("maxZImp", flags.InDet.Tracking.ActivePass.maxZImpact)
    kwargs.setdefault("maxEta", flags.InDet.Tracking.ActivePass.maxEta)
    kwargs.setdefault("Extrapolator", InDetExtrapolator)
    kwargs.setdefault("RejectShortExtension", flags.InDet.Tracking.ActivePass.rejectShortExtensions)
    kwargs.setdefault("FinalRefit", False)
    kwargs.setdefault("FinalStatistics", False)
    kwargs.setdefault("OutputSegments", False)
    kwargs.setdefault("InputSegmentsLocation", 'TRTSegments') # InDetKeys.TRT_Segments()
    kwargs.setdefault("OutputTracksLocation", TRTSeededTracks)
    kwargs.setdefault("CaloClusterEt", flags.InDet.Tracking.ActivePass.minRoIClusterEt)

    if flags.InDet.Tracking.ActivePass.RoISeededBackTracking:
        from RegionSelector.RegSelToolConfig import regSelTool_SCT_Cfg
        RegSelTool_SCT   = acc.popToolsAndMerge(regSelTool_SCT_Cfg(flags))
        acc.addPublicTool(RegSelTool_SCT)

        kwargs.setdefault("RegSelTool", RegSelTool_SCT)
        kwargs.setdefault("CaloSeededRoI", True)

    InDetTRT_SeededTrackFinder = CompFactory.InDet.TRT_SeededTrackFinder(name = name, **kwargs)
    acc.addEventAlgo(InDetTRT_SeededTrackFinder)
    return acc

def TrkAmbiguityScoreCfg(flags, name='InDetTRT_SeededAmbiguityScore', **kwargs):
    acc = ComponentAccumulator()
    #
    # --- Output key for the finder
    #
    TRTSeededTracks = 'TRTSeededTracks' # InDetKeys.TRTSeededTracks()

    kwargs.setdefault("TrackInput", [ TRTSeededTracks ])
    kwargs.setdefault("TrackOutput", 'ScoredMap'+'InDetTRT_SeededAmbiguityScore')

    InDetAmbiguityScore = CompFactory.Trk.TrkAmbiguityScore(name = name, **kwargs)
    acc.addEventAlgo(InDetAmbiguityScore)
    return acc

def InDetTRTAmbiTrackSelectionToolCfg(flags, name='InDetTRT_SeededAmbiTrackSelectionTool', **kwargs):
    acc = ComponentAccumulator()

    InDetTRTDriftCircleCut = acc.popToolsAndMerge(TC.InDetTRTDriftCircleCutForPatternRecoCfg(flags))

    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge( TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags) )

    kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCut)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("minScoreShareTracks", -1.) # off !
    kwargs.setdefault("minHits", flags.InDet.Tracking.ActivePass.minSecondaryClusters)
    kwargs.setdefault("minNotShared", flags.InDet.Tracking.ActivePass.minSecondarySiNotShared)
    kwargs.setdefault("maxShared", flags.InDet.Tracking.ActivePass.maxSecondaryShared)
    kwargs.setdefault("minTRTHits", flags.InDet.Tracking.ActivePass.minSecondaryTRTonTrk)
    kwargs.setdefault("UseParameterization", flags.InDet.Tracking.ActivePass.useParameterizedTRTCuts)
    kwargs.setdefault("Cosmics", flags.Beam.Type == "cosmics")
    kwargs.setdefault("doPixelSplitting", flags.InDet.Tracking.doPixelClusterSplitting)

    acc.setPrivateTools(CompFactory.InDet.InDetAmbiTrackSelectionTool(name, **kwargs))
    return acc

def SimpleAmbiguityProcessorToolCfg(flags, name='InDetTRT_SeededAmbiguityProcessor', ClusterSplitProbContainer="", **kwargs):
    acc = ComponentAccumulator()
    #
    # --- load Ambiguity Processor
    #
    InDetTrackFitterBT = acc.popToolsAndMerge(TC.InDetTrackFitterBTCfg(flags))
    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge( TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags) )

    #
    # --- set up special Scoring Tool for TRT seeded tracks
    #
    if flags.Beam.Type == "cosmics":
        InDetTRT_SeededScoringTool = acc.popToolsAndMerge(TC.InDetCosmicScoringTool_TRTCfg(flags))
        InDetTRT_SeededSummaryTool = acc.popToolsAndMerge(TC.InDetTrackSummaryToolSharedHitsCfg(flags))
    else:
        InDetTRT_SeededScoringTool = acc.popToolsAndMerge(TC.InDetTRT_SeededScoringToolCfg(flags))
        InDetTRT_SeededSummaryTool = acc.getPrimaryAndMerge(TC.InDetTrackSummaryToolCfg(flags))

    InDetTRT_SeededAmbiTrackSelectionTool = acc.popToolsAndMerge(InDetTRTAmbiTrackSelectionToolCfg(flags))

    kwargs.setdefault("Fitter", InDetTrackFitterBT)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("TrackSummaryTool", InDetTRT_SeededSummaryTool)
    kwargs.setdefault("SelectionTool", InDetTRT_SeededAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName", ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'InDetTRT_SeededAmbiguityProcessorSplitProb'+flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("RefitPrds", False)
    kwargs.setdefault("SuppressTrackFit", False)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("ScoringTool", InDetTRT_SeededScoringTool)
    kwargs.setdefault("MatEffects", flags.InDet.Tracking.materialInteractionsType if flags.InDet.Tracking.materialInteractions else 0)

    InDetTRT_SeededAmbiguityProcessor = CompFactory.Trk.SimpleAmbiguityProcessorTool(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededAmbiguityProcessor)
    return acc

def TrkAmbiguitySolverCfg(flags, name='InDetTRT_SeededAmbiguitySolver', ClusterSplitProbContainer ='', **kwargs):
    acc = ComponentAccumulator()

    ResolvedTRTSeededTracks = 'ResolvedTRTSeededTracks' # InDetKeys.ResolvedTRTSeededTracks()

    InDetTRT_SeededAmbiguityProcessor = acc.popToolsAndMerge(SimpleAmbiguityProcessorToolCfg(flags, 
                                                                                             ClusterSplitProbContainer=ClusterSplitProbContainer))
    acc.addPublicTool(InDetTRT_SeededAmbiguityProcessor)

    kwargs.setdefault("TrackInput", 'ScoredMap'+'InDetTRT_SeededAmbiguityScore')
    kwargs.setdefault("TrackOutput", ResolvedTRTSeededTracks)
    kwargs.setdefault("AmbiguityProcessor", InDetTRT_SeededAmbiguityProcessor)

    InDetTRT_SeededAmbiguitySolver = CompFactory.Trk.TrkAmbiguitySolver(name = name, **kwargs)
    acc.addEventAlgo(InDetTRT_SeededAmbiguitySolver)
    return acc

# ------------------------------------------------------------
#
# ----------- 2nd iteration, outside in tracking
#
# ------------------------------------------------------------

def BackTrackingCfg(flags, InputCollections = None, TrackCollectionKeys=[] , TrackCollectionTruthKeys=[], ClusterSplitProbContainer=''):
    acc = ComponentAccumulator()
    # ------------------------------------------------------------
    #
    # ---------- TRT Seeded Tracking
    #
    # ------------------------------------------------------------

    acc.merge(TRT_SeededTrackFinderCfg(flags,
                                       InputCollections = InputCollections))
    # ------------------------------------------------------------
    #
    # --- Resolve back tracking tracks
    #
    # ------------------------------------------------------------
    acc.merge(TrkAmbiguityScoreCfg(flags))
    acc.merge(TrkAmbiguitySolverCfg(flags,
                                    ClusterSplitProbContainer = ClusterSplitProbContainer))

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    flags.Input.Files=defaultTestFiles.RDO_RUN2

    # disable calo for this test
    flags.Detector.EnableCalo = False

    numThreads=1
    flags.Concurrency.NumThreads=numThreads
    flags.Concurrency.NumConcurrentEvents=numThreads

    flags.loadAllDynamicFlags()

    flags.lock()
    flags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(flags)

    ################################ Aditional configurations ################################
    ##
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(flags))

    ##
    from InDetConfig.SiliconPreProcessing import InDetRecPreProcessingSiliconCfg
    top_acc.merge(InDetRecPreProcessingSiliconCfg(flags))

    ########################## TRTPreProcessing Configuration ################################

    if not flags.InDet.Tracking.doDBMstandalone:
        from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
        top_acc.merge(TRTPreProcessingCfg(flags))

    ######################################## TRTSegmentFinding Configuration ###########################################
    InputCollections = []

    from InDetConfig.TRTSegmentFindingConfig import TRTSegmentFindingCfg
    top_acc.merge(TRTSegmentFindingCfg( flags,
                                        extension = "",
                                        InputCollections = InputCollections,
                                        BarrelSegments = 'TRTSegments'))

    ########################################## BackTracking Configuration ##############################################
    TrackCollectionKeys = []

    top_acc.merge(BackTrackingCfg(  flags,
                                    InputCollections = InputCollections,
                                    TrackCollectionKeys=TrackCollectionKeys,
                                    TrackCollectionTruthKeys=[],
                                    ClusterSplitProbContainer=''))

    ####################################################################################################################

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5

    top_acc.printConfig()
    top_acc.store(open("test_BackTrackingConfig.pkl", "wb"))
    import sys
    if "--norun" not in sys.argv:
        sc = top_acc.run(5)
        sys.exit(not sc.isSuccess())
