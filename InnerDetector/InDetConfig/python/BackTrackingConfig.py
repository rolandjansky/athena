# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
import InDetConfig.TrackingCommonConfig         as   TC

def SiDetElementsRoadMaker_xkCfg(flags, name = 'InDetTRT_SeededSiRoad', **kwargs):
    acc = ComponentAccumulator()
    #
    # Silicon det elements road maker tool
    #
    InDetPatternPropagator = TC.InDetPatternPropagatorCfg()
    acc.addPublicTool(InDetPatternPropagator)

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("usePixel", flags.InDet.Tracking.usePixel)
    kwargs.setdefault("PixManagerLocation", 'Pixel') # InDetKeys.PixelManager()
    kwargs.setdefault("useSCT", flags.InDet.Tracking.useSCT)
    kwargs.setdefault("SCTManagerLocation", 'SCT') # InDetKeys.SCT_Manager()
    kwargs.setdefault("RoadWidth", 35.)
    kwargs.setdefault("MaxStep", 20.)

    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("RoadWidth", 50)
    # Condition algorithm for InDet__SiDetElementsRoadMaker_xk
    if flags.InDet.Tracking.useSCT:
        acc.addCondAlgo(CompFactory.InDet.SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk"))

    InDetTRT_SeededSiRoadMaker = CompFactory.InDet.SiDetElementsRoadMaker_xk(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededSiRoadMaker)
    return acc

def SimpleTRT_SeededSpacePointFinder_ATLCfg(flags, name='InDetTRT_SeededSpFinder', InputCollections=[], **kwargs):
    acc = ComponentAccumulator()

    #
    # --- decide if use the association tool
    #
    if len(InputCollections) > 0:
        prefix = 'InDetSegment'
        suffix = ''
        asso_tool = acc.popToolsAndMerge(TC.ConstPRD_AssociationToolCfg(namePrefix = prefix, nameSuffix = suffix))
        acc.addPublicTool(asso_tool)

    kwargs.setdefault("SpacePointsSCTName", 'SCT_SpacePoints') # InDetKeys.SCT_SpacePoints()
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
    kwargs.setdefault("pTmin", flags.InDet.Tracking.minSecondaryPt)

    InDetTRT_SeededSpacePointFinder = CompFactory.InDet.TRT_SeededSpacePointFinder_ATL(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededSpacePointFinder)
    return acc

def TRT_SeededTrackFinder_ATLCfg(flags, name='InDetTRT_SeededTrackMaker', InputCollections=[], **kwargs):
    acc = ComponentAccumulator()
    #
    # --- TRT seeded back tracking tool
    #
    InDetPatternPropagator = TC.InDetPatternPropagatorCfg()
    acc.addPublicTool(InDetPatternPropagator)

    InDetPatternUpdator = TC.InDetPatternUpdatorCfg()
    acc.addPublicTool(InDetPatternUpdator)

    InDetSiComTrackFinder = acc.popToolsAndMerge(TC.SiCombinatorialTrackFinder_xkCfg(flags))
    acc.addPublicTool(InDetSiComTrackFinder)

    if (flags.InDet.Tracking.usePixel and flags.InDet.Tracking.useSCT) is not False:
        InDetTRT_SeededSiRoadMaker = acc.popToolsAndMerge(SiDetElementsRoadMaker_xkCfg(flags))
        acc.addPublicTool(InDetTRT_SeededSiRoadMaker)
        kwargs.setdefault("RoadTool", InDetTRT_SeededSiRoadMaker)

    #
    # --- decide which TRT seed space point finder to use
    #
    if flags.InDet.loadTRTSeededSPFinder:
        InDetTRT_SeededSpacePointFinder = acc.popToolsAndMerge(TRT_SeededSpacePointFinder_ATLCfg(flags, 
                                                                                                 InputCollections=InputCollections))
    elif flags.InDet.loadSimpleTRTSeededSPFinder:
        InDetTRT_SeededSpacePointFinder = acc.popToolsAndMerge(SimpleTRT_SeededSpacePointFinder_ATLCfg(flags, InputCollections=InputCollections))

    acc.addPublicTool(InDetTRT_SeededSpacePointFinder)

    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    kwargs.setdefault("UpdatorTool", InDetPatternUpdator)
    kwargs.setdefault("SeedTool", InDetTRT_SeededSpacePointFinder)
    kwargs.setdefault("CombinatorialTrackFinder", InDetSiComTrackFinder)
    kwargs.setdefault("pTmin", flags.InDet.Tracking.minSecondaryPt)
    kwargs.setdefault("nHolesMax", flags.InDet.Tracking.SecondarynHolesMax)
    kwargs.setdefault("nHolesGapMax", flags.InDet.Tracking.SecondarynHolesGapMax)
    kwargs.setdefault("Xi2max", flags.InDet.Tracking.SecondaryXi2max)
    kwargs.setdefault("Xi2maxNoAdd", flags.InDet.Tracking.SecondaryXi2maxNoAdd)
    kwargs.setdefault("SearchInCaloROI", False)
    kwargs.setdefault("InputClusterContainerName", 'InDetCaloClusterROIs') # InDetKeys.CaloClusterROIContainer()
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

    InDetTrackFitterBT = acc.popToolsAndMerge(TC.InDetTrackFitterBTCfg(flags))
    acc.addPublicTool(InDetTrackFitterBT)

    InDetTrackSummaryToolNoHoleSearch = acc.popToolsAndMerge(TC.InDetTrackSummaryToolNoHoleSearchCfg(flags))
    acc.addPublicTool(InDetTrackSummaryToolNoHoleSearch)

    InDetTRTExtensionTool = acc.popToolsAndMerge(TC.InDetTRT_ExtensionToolCfg(flags))
    acc.addPublicTool(InDetTRTExtensionTool)

    from  InDetConfig.InDetRecToolConfig import InDetExtrapolatorCfg
    tmpAcc =  InDetExtrapolatorCfg(flags)
    InDetExtrapolator = tmpAcc.getPrimary()
    acc.merge(tmpAcc)

    InDetTRT_SeededTrackTool = acc.popToolsAndMerge(TRT_SeededTrackFinder_ATLCfg(flags, 
                                                                                 InputCollections=InputCollections))
    acc.addPublicTool(InDetTRT_SeededTrackTool)

    kwargs.setdefault("RefitterTool", InDetTrackFitterBT)
    kwargs.setdefault("TrackTool", InDetTRT_SeededTrackTool)
    kwargs.setdefault("PRDtoTrackMap", prefix+'PRDtoTrackMap'+suffix if usePrdAssociationTool else "")
    kwargs.setdefault("TrackSummaryTool", InDetTrackSummaryToolNoHoleSearch)
    kwargs.setdefault("TrackExtensionTool", InDetTRTExtensionTool)
    kwargs.setdefault("MinTRTonSegment", flags.InDet.Tracking.minSecondaryTRTonTrk)
    kwargs.setdefault("MinTRTonly", flags.InDet.Tracking.minTRTonly)
    kwargs.setdefault("TrtExtension", True)
    kwargs.setdefault("SiExtensionCuts", flags.InDet.Tracking.SiExtensionCuts)
    kwargs.setdefault("minPt", flags.InDet.Tracking.minSecondaryPt)
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.maxSecondaryImpact)
    kwargs.setdefault("maxZImp", flags.InDet.Tracking.maxZImpact)
    kwargs.setdefault("maxEta", flags.InDet.Tracking.maxEta)
    kwargs.setdefault("Extrapolator", InDetExtrapolator)
    kwargs.setdefault("RejectShortExtension", flags.InDet.Tracking.rejectShortExtensions)
    kwargs.setdefault("FinalRefit", False)
    kwargs.setdefault("FinalStatistics", False)
    kwargs.setdefault("OutputSegments", False)
    kwargs.setdefault("InputSegmentsLocation", 'TRTSegments') # InDetKeys.TRT_Segments()
    kwargs.setdefault("OutputTracksLocation", TRTSeededTracks)
    kwargs.setdefault("CaloClusterEt", flags.InDet.Tracking.minRoIClusterEt)

    if flags.InDet.Tracking.RoISeededBackTracking:
        from RegionSelector.RegSelToolConfig import regSelTool_SCT_Cfg
        RegSelTool_SCT   = acc.popToolsAndMerge(regSelTool_SCT_Cfg(flags))
        acc.addPublicTool(RegSelTool_SCT)

        kwargs.setdefault("RegSelTool", RegSelTool_SCT)
        kwargs.setdefault("CaloSeededRoI", True)

    InDetTRT_SeededTrackFinder = CompFactory.InDet.TRT_SeededTrackFinder(name = name, **kwargs)
    acc.addEventAlgo(InDetTRT_SeededTrackFinder)
    return acc

def TrkAmbiguityScoreCfg(name='InDetTRT_SeededAmbiguityScore', **kwargs):
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

def InDetAmbiTrackSelectionToolCfg(flags, name='InDetTRT_SeededAmbiTrackSelectionTool', **kwargs):
    acc = ComponentAccumulator()

    InDetTRTDriftCircleCut = TC.InDetTRTDriftCircleCutForPatternRecoCfg(flags)
    acc.addPublicTool(InDetTRTDriftCircleCut)

    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge( TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags) )
    acc.addPublicTool(InDetPRDtoTrackMapToolGangedPixels)

    kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCut)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("minScoreShareTracks", -1.) # off !
    kwargs.setdefault("minHits", flags.InDet.Tracking.minSecondaryClusters)
    kwargs.setdefault("minNotShared", flags.InDet.Tracking.minSecondarySiNotShared)
    kwargs.setdefault("maxShared", flags.InDet.Tracking.maxSecondaryShared)
    kwargs.setdefault("minTRTHits", flags.InDet.Tracking.minSecondaryTRTonTrk)
    kwargs.setdefault("UseParameterization", flags.InDet.Tracking.useParameterizedTRTCuts)
    kwargs.setdefault("Cosmics", flags.Beam.Type == "cosmics")
    kwargs.setdefault("doPixelSplitting", flags.InDet.doPixelClusterSplitting)

    InDetTRT_SeededAmbiTrackSelectionTool = CompFactory.InDet.InDetAmbiTrackSelectionTool(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededAmbiTrackSelectionTool)
    return acc

def SimpleAmbiguityProcessorToolCfg(flags, name='InDetTRT_SeededAmbiguityProcessor', ClusterSplitProbContainer="", **kwargs):
    acc = ComponentAccumulator()
    #
    # --- load Ambiguity Processor
    #
    InDetTrackFitterBT = acc.popToolsAndMerge(TC.InDetTrackFitterBTCfg(flags))
    acc.addPublicTool(InDetTrackFitterBT)

    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge( TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags) )
    acc.addPublicTool(InDetPRDtoTrackMapToolGangedPixels)

    #
    # --- set up special Scoring Tool for TRT seeded tracks
    #
    if flags.Beam.Type == "cosmics":
        InDetTRT_SeededScoringTool = acc.popToolsAndMerge(TC.InDetCosmicScoringTool_TRTCfg(flags))
        acc.addPublicTool(InDetTRT_SeededScoringTool)
        InDetTRT_SeededSummaryTool = acc.popToolsAndMerge(TC.InDetTrackSummaryToolSharedHitsCfg(flags))
        acc.addPublicTool(InDetTRT_SeededSummaryTool)
    else:
        InDetTRT_SeededScoringTool = acc.popToolsAndMerge(TC.InDetTRT_SeededScoringToolCfg(flags))
        acc.addPublicTool(InDetTRT_SeededScoringTool)
        InDetTRT_SeededSummaryTool = acc.popToolsAndMerge(TC.InDetTrackSummaryToolCfg(flags))
        acc.addPublicTool(InDetTRT_SeededSummaryTool)

    InDetTRT_SeededAmbiTrackSelectionTool = acc.popToolsAndMerge(InDetAmbiTrackSelectionToolCfg(flags))
    acc.addPublicTool(InDetTRT_SeededAmbiTrackSelectionTool)

    kwargs.setdefault("Fitter", InDetTrackFitterBT)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("TrackSummaryTool", InDetTRT_SeededSummaryTool)
    kwargs.setdefault("SelectionTool", InDetTRT_SeededAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName", ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName", 'InDetTRT_SeededAmbiguityProcessorSplitProb'+flags.InDet.Tracking.extension)
    kwargs.setdefault("RefitPrds", not flags.InDet.refitROT)
    kwargs.setdefault("SuppressTrackFit", False)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("ScoringTool", InDetTRT_SeededScoringTool)

    if flags.InDet.materialInteractions:
        kwargs.setdefault("MatEffects", flags.InDet.materialInteractionsType)
    else:
        kwargs.setdefault("MatEffects", 0)

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

    if flags.InDet.doTRTSeededTrackFinder:
        #
        # --- decide which TRT seed space point finder to use
        #
        acc.merge(TRT_SeededTrackFinderCfg( flags,
                                            InputCollections=InputCollections))
    # ------------------------------------------------------------
    #
    # --- Resolve back tracking tracks ?
    #
    # ------------------------------------------------------------
    if flags.InDet.doResolveBackTracks:
        acc.merge(TrkAmbiguityScoreCfg())
        acc.merge(TrkAmbiguitySolverCfg(flags,
                                        ClusterSplitProbContainer = ClusterSplitProbContainer))

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files=defaultTestFiles.RDO

    ConfigFlags.Detector.GeometryPixel   = True
    ConfigFlags.Detector.GeometrySCT   = True
    ConfigFlags.Detector.GeometryTRT   = True

    ConfigFlags.Detector.RecoIBL = True
    ConfigFlags.Detector.RecoPixel = True
    ConfigFlags.Detector.RecoTRT = True

    ConfigFlags.InDet.doTRTSeededTrackFinder = True
    ConfigFlags.InDet.doResolveBackTracks = True

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads

    ConfigFlags.loadAllDynamicFlags()

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    ################################ Aditional configurations ################################
    ##
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    top_acc.merge( PixelGeometryCfg(ConfigFlags) )

    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    top_acc.merge(SCT_GeometryCfg(ConfigFlags))

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    top_acc.merge(TRT_GeometryCfg( ConfigFlags ))

    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg, MuonIdHelperSvcCfg
    top_acc.merge(MuonGeoModelCfg(ConfigFlags))
    top_acc.merge(MuonIdHelperSvcCfg(ConfigFlags))
    ##
    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    top_acc.merge(BeamSpotCondAlgCfg(ConfigFlags))

    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    top_acc.merge(BeamPipeGeometryCfg(ConfigFlags))
    ##
    top_acc.addCondAlgo( CompFactory.InDet.SiDetElementBoundaryLinksCondAlg_xk( name = "InDetSiDetElementBoundaryLinksPixelCondAlg",
                                                                                ReadKey  = "PixelDetectorElementCollection",
                                                                                WriteKey = "PixelDetElementBoundaryLinks_xk") )

    top_acc.addCondAlgo(CompFactory.InDet.SiDetElementsRoadCondAlg_xk(name = "InDet__SiDetElementsRoadCondAlg_xk"))
    ##
    from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool, PixelLorentzAngleCfg
    top_acc.addPublicTool(PixelLorentzAngleTool(ConfigFlags))
    top_acc.addPublicTool(top_acc.popToolsAndMerge(PixelLorentzAngleCfg(ConfigFlags)))

    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    top_acc.addPublicTool(top_acc.popToolsAndMerge(SCT_LorentzAngleCfg(ConfigFlags)))
    ##
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelOfflineCalibCondAlgCfg, PixelDistortionAlgCfg
    top_acc.merge(PixelOfflineCalibCondAlgCfg(ConfigFlags))
    top_acc.merge(PixelDistortionAlgCfg(ConfigFlags))
    ##
    top_acc.merge(TC.PixelClusterNnCondAlgCfg(ConfigFlags))
    top_acc.merge(TC.PixelClusterNnWithTrackCondAlgCfg(ConfigFlags))
    ##
    from InDetConfig.TRTSegmentFindingConfig import TRTActiveCondAlgCfg
    top_acc.merge(TRTActiveCondAlgCfg(ConfigFlags))
    top_acc.merge(TC.TRT_DetElementsRoadCondAlgCfg())

    ########################## TRTPreProcessing Configuration ################################

    from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
    if not ConfigFlags.InDet.doDBMstandalone:
        top_acc.merge(TRTPreProcessingCfg(ConfigFlags,(not ConfigFlags.InDet.doTRTPhaseCalculation or ConfigFlags.Beam.Type =="collisions"),False))

    ######################################## TRTSegmentFinding Configuration ###########################################
    InputCollections = []

    from InDetConfig.TRTSegmentFindingConfig import TRTSegmentFindingCfg
    top_acc.merge(TRTSegmentFindingCfg( ConfigFlags,
                                        extension = "",
                                        InputCollections = InputCollections,
                                        BarrelSegments = 'TRTSegments', # InDetKeys.TRT_Segments
                                        doPhase = False))

    ########################################## BackTracking Configuration ##############################################
    TrackCollectionKeys = []

    top_acc.merge(BackTrackingCfg(  ConfigFlags,
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
        sc = top_acc.run(25)
        sys.exit(not sc.isSuccess())
