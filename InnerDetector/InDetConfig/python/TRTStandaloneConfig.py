# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
import InDetConfig.TrackingCommonConfig         as   TC

def InDetTrtTrackScoringToolCfg(flags, name ='InDetTRT_StandaloneScoringTool', extension = "", **kwargs):
    acc = ComponentAccumulator()

    #
    # --- set up special Scoring Tool for standalone TRT tracks
    #
    InDetTrackSummaryTool = acc.getPrimaryAndMerge(TC.InDetTrackSummaryToolCfg(flags))
    InDetTRTDriftCircleCut = acc.getPrimaryAndMerge(TC.InDetTRTDriftCircleCutForPatternRecoCfg(flags))

    #
    # Cut values and output key for the TRT segments standalone TRT track finder
    #
    if extension == "_TRT":
        # TRT track segments
        pTmin = flags.InDet.Tracking.minPT
    else:
        # TRT standalone
        pTmin = flags.InDet.Tracking.minTRTonlyPt # new cut parameter to make it flexible...

    kwargs.setdefault("SummaryTool", InDetTrackSummaryTool)
    kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCut)
    kwargs.setdefault("useAmbigFcn", True)
    kwargs.setdefault("useSigmaChi2", False)
    kwargs.setdefault("PtMin", pTmin)
    kwargs.setdefault("minTRTonTrk", flags.InDet.Tracking.minTRTonly)
    kwargs.setdefault("maxEta", 2.1)
    kwargs.setdefault("UseParameterization", flags.InDet.Tracking.useTRTonlyParamCuts)
    kwargs.setdefault("OldTransitionLogic", flags.InDet.Tracking.useTRTonlyOldLogic)
    kwargs.setdefault("minTRTPrecisionFraction", flags.InDet.Tracking.minSecondaryTRTPrecFrac)
    kwargs.setdefault("TRTTrksEtaBins", flags.InDet.Tracking.TrkSel.TRTTrksEtaBins)
    kwargs.setdefault("TRTTrksMinTRTHitsThresholds", flags.InDet.Tracking.TrkSel.TRTTrksMinTRTHitsThresholds)
    kwargs.setdefault("TRTTrksMinTRTHitsMuDependencies", flags.InDet.Tracking.TrkSel.TRTTrksMinTRTHitsMuDependencies)

    acc.setPrivateTools(CompFactory.InDet.InDetTrtTrackScoringTool(name, **kwargs))
    return acc

def TRT_SegmentToTrackToolCfg(flags, name ='InDetTRT_SegmentToTrackTool', extension = "", usePrdAssociationTool = True, **kwargs):
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    acc = MagneticFieldSvcCfg(flags)

    #
    # set up TRT_SegmentToTrackTool
    #

    if usePrdAssociationTool:
        asso_tool = acc.popToolsAndMerge( TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags) )
    else:
        asso_tool = None

    InDetTrackFitterTRT = acc.popToolsAndMerge(TC.InDetTrackFitterTRTCfg(flags))
    acc.addPublicTool(InDetTrackFitterTRT)

    InDetTrackSummaryTool = acc.getPrimaryAndMerge(TC.InDetTrackSummaryToolCfg(flags))

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))

    
    InDetTRT_StandaloneScoringTool = acc.popToolsAndMerge(InDetTrtTrackScoringToolCfg(flags,name='InDetTRT_StandaloneScoringTool'+ extension,
                                                                                            extension=extension))
    acc.addPublicTool(InDetTRT_StandaloneScoringTool)
    
    kwargs.setdefault("RefitterTool", InDetTrackFitterTRT)
    kwargs.setdefault("AssociationTool", asso_tool)
    kwargs.setdefault("TrackSummaryTool", InDetTrackSummaryTool)
    kwargs.setdefault("ScoringTool", InDetTRT_StandaloneScoringTool)
    kwargs.setdefault("Extrapolator", InDetExtrapolator)
    kwargs.setdefault("FinalRefit", True)
    kwargs.setdefault("MaxSharedHitsFraction", flags.InDet.Tracking.maxTRTonlyShared)
    kwargs.setdefault("SuppressHoleSearch", True)

    InDetTRT_SegmentToTrackTool = CompFactory.InDet.TRT_SegmentToTrackTool(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SegmentToTrackTool)
    return acc

def TRT_StandaloneTrackFinderCfg(flags, name ='InDetTRT_StandaloneTrackFinder', extension = "", BarrelSegments = None, prd_to_track_map = '', **kwargs):
    acc = ComponentAccumulator()
    if not flags.Input.isMC:
        # TODO: certainly not the right dependency but at least it makes this algorithm run
        from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
        acc.merge(LumiBlockMuWriterCfg(flags))

    usePrdAssociationTool = True
    #usePrdAssociationTool = True if len(InputCollections) > 0 else False

    if extension == "_TRT":
        TRTStandaloneTracks = 'StandaloneTRTTracks' # InDetKeys.TRTTracks
    else:
        TRTStandaloneTracks = 'TRTStandaloneTracks' # flags.InDetKeys.TRTTracks_NewT
    #
    # set up TRT_SegmentToTrackTool
    #
    InDetTRT_SegmentToTrackTool = acc.popToolsAndMerge(TRT_SegmentToTrackToolCfg(flags, name='InDetTRT_SegmentToTrackTool'+ extension,
                                                                                        extension=extension,
                                                                                        usePrdAssociationTool = usePrdAssociationTool))
    acc.addPublicTool(InDetTRT_SegmentToTrackTool)

    kwargs.setdefault("MinNumDriftCircles", flags.InDet.Tracking.minTRTonly)
    kwargs.setdefault("MinPt", flags.InDet.Tracking.minTRTonlyPt)
    kwargs.setdefault("InputSegmentsLocation", BarrelSegments)
    kwargs.setdefault("MaterialEffects", 0)
    kwargs.setdefault("PRDtoTrackMap", prd_to_track_map)
    kwargs.setdefault("OldTransitionLogic", flags.InDet.Tracking.useTRTonlyOldLogic)
    kwargs.setdefault("OutputTracksLocation", TRTStandaloneTracks)
    kwargs.setdefault("TRT_SegToTrackTool", InDetTRT_SegmentToTrackTool)

    InDetTRT_StandaloneTrackFinder = CompFactory.InDet.TRT_StandaloneTrackFinder(name = name, **kwargs)
    acc.addEventAlgo(InDetTRT_StandaloneTrackFinder)
    return acc

def TRT_SegmentsToTrackCfg( flags, name ='InDetTRT_SegmentsToTrack_Barrel', extension = "", BarrelSegments = None, prd_to_track_map = '', **kwargs):
    acc = ComponentAccumulator()

    usePrdAssociationTool = True
    #usePrdAssociationTool = True if len(InputCollections) > 0 else False

    if extension == "_TRT":
        TRTStandaloneTracks = 'StandaloneTRTTracks' # InDetKeys.TRTTracks
    else:
        TRTStandaloneTracks = 'TRTStandaloneTracks' # flags.InDetKeys.TRTTracks_NewT

    #
    # set up TRT_SegmentToTrackTool
    #
    InDetTRT_SegmentToTrackTool = acc.popToolsAndMerge(TRT_SegmentToTrackToolCfg(flags, name='InDetTRT_SegmentToTrackTool'+ extension,
                                                                                        extension=extension,
                                                                                        usePrdAssociationTool = usePrdAssociationTool))
    acc.addPublicTool(InDetTRT_SegmentToTrackTool)

    #
    # --- cosmics segment to track conversion for Barrel
    #
    InDetTrackFitter = acc.popToolsAndMerge(TC.InDetKalmanFitterCfg(flags))
    acc.addPublicTool(InDetTrackFitter)

    InDetTrackSummaryToolTRTTracks = acc.popToolsAndMerge(TC.InDetTrackSummaryToolTRTTracksCfg(flags))
    acc.addPublicTool(InDetTrackSummaryToolTRTTracks)

    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge( TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags) )
    acc.addPublicTool(InDetPRDtoTrackMapToolGangedPixels)

    kwargs.setdefault("InputSegmentsCollection", BarrelSegments)
    kwargs.setdefault("OutputTrackCollection", TRTStandaloneTracks)
    kwargs.setdefault("TrackFitter", InDetTrackFitter)
    kwargs.setdefault("SummaryTool", InDetTrackSummaryToolTRTTracks)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels if prd_to_track_map !='' else None,)
    kwargs.setdefault("InputAssociationMapName", prd_to_track_map)
    kwargs.setdefault("MinNHit", flags.InDet.Tracking.minTRTonly)
    kwargs.setdefault("OutlierRemoval", True)
    kwargs.setdefault("MaterialEffects", False)

    InDetTrkSegmenttoTrk = CompFactory.InDet.TRT_SegmentsToTrack(name = name, **kwargs)
    acc.addEventAlgo(InDetTrkSegmenttoTrk)
    return acc
# ------------------------------------------------------------------------------------
#
# ----------- TRT Standelone Track Finding
#
# ------------------------------------------------------------------------------------
def TRTStandaloneCfg( flags, extension = '', InputCollections = None, BarrelSegments = None, PRDtoTrackMap = ''):
    acc = ComponentAccumulator()
    # --- Always use PRD association tool (even if only 1 collection) to remove TRT
    #     segments with significant overlaping hits 
    usePrdAssociationTool = True
    #usePrdAssociationTool = True if len(InputCollections) > 0 else False

    #
    # --- get list of already associated hits (always do this, even if no other tracking ran before)
    #
    prd_to_track_map = PRDtoTrackMap
    if usePrdAssociationTool and extension != "_TRT" :
        prefix='InDetTRTonly_'
        prd_to_track_map = prefix+'PRDtoTrackMap'+extension
        acc.merge(TC.InDetTrackPRD_AssociationCfg(flags, namePrefix = prefix,
                                                         nameSuffix = extension,
                                                         TracksName = list(InputCollections)))
    
    if not flags.Beam.Type == "cosmics":
        #
        # --- TRT standalone tracks algorithm
        #
        acc.merge(TRT_StandaloneTrackFinderCfg(flags,   name = 'InDetTRT_StandaloneTrackFinder'+extension,
                                                        extension=extension,
                                                        BarrelSegments = BarrelSegments,
                                                        prd_to_track_map = prd_to_track_map))
    else: 
        #
        # --- cosmics segment to track conversion for Barrel
        #
        acc.merge(TRT_SegmentsToTrackCfg(flags, name = 'InDetTRT_SegmentsToTrack_Barrel'+extension,
                                                extension=extension,
                                                BarrelSegments = BarrelSegments,
                                                prd_to_track_map = prd_to_track_map))

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files=defaultTestFiles.RDO

    # TODO: TRT only?

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads

    ConfigFlags.loadAllDynamicFlags()

    ConfigFlags.lock()
    ConfigFlags.dump()
    
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    ################################ Aditional configurations ###############################
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
    top_acc.merge(TRT_ReadoutGeometryCfg( ConfigFlags ))

    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    top_acc.merge( PixelReadoutGeometryCfg(ConfigFlags) )

    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    top_acc.merge(SCT_ReadoutGeometryCfg(ConfigFlags))

    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg, MuonIdHelperSvcCfg
    top_acc.merge(MuonGeoModelCfg(ConfigFlags))
    top_acc.merge(MuonIdHelperSvcCfg(ConfigFlags))

    from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
    ###
    ###
    top_acc.merge(TC.PixelClusterNnCondAlgCfg(ConfigFlags))
    top_acc.merge(TC.PixelClusterNnWithTrackCondAlgCfg(ConfigFlags))
    ###
    ###
    top_acc.merge(addFoldersSplitOnline(ConfigFlags, "PIXEL", "/PIXEL/PixdEdx", "/PIXEL/PixdEdx", className='AthenaAttributeList'))

    PixeldEdxAlg = CompFactory.PixeldEdxAlg(name="PixeldEdxAlg", ReadFromCOOL = True)
    top_acc.addCondAlgo(PixeldEdxAlg)
    ###

    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTStrawCondAlgCfg, TRTToTCondAlg, TRTHTCondAlgCfg
    top_acc.merge(TRTStrawCondAlgCfg(ConfigFlags))
    top_acc.merge(TRTToTCondAlg(ConfigFlags))
    top_acc.merge(TRTHTCondAlgCfg(ConfigFlags))

    ###
    ###
    from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool, PixelLorentzAngleCfg
    top_acc.addPublicTool(PixelLorentzAngleTool(ConfigFlags))
    top_acc.addPublicTool(top_acc.popToolsAndMerge(PixelLorentzAngleCfg(ConfigFlags)))

    from SiLorentzAngleTool.SCT_LorentzAngleConfig import SCT_LorentzAngleCfg
    top_acc.addPublicTool(top_acc.popToolsAndMerge(SCT_LorentzAngleCfg(ConfigFlags)))
    ###
    ###
    from PixelConditionsAlgorithms.PixelConditionsConfig import (PixelOfflineCalibCondAlgCfg, PixelDistortionAlgCfg)
    top_acc.merge(PixelOfflineCalibCondAlgCfg(ConfigFlags))
    top_acc.merge(PixelDistortionAlgCfg(ConfigFlags))
    ###
    ###
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTActiveCondAlgCfg
    top_acc.merge(TRTActiveCondAlgCfg(ConfigFlags))
    top_acc.merge(TC.TRT_DetElementsRoadCondAlgCfg())
    ############################# TRTPreProcessing configuration ############################
    from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
    if not ConfigFlags.InDet.doDBMstandalone:
        top_acc.merge(TRTPreProcessingCfg(ConfigFlags,(not ConfigFlags.InDet.doTRTPhaseCalculation or ConfigFlags.Beam.Type =="collisions"),False))
    ########################### TRTSegmentFindingCfg configuration ##########################
    # NewTracking collection keys
    InputCombinedInDetTracks = []

    from InDetConfig.TRTSegmentFindingConfig import TRTSegmentFindingCfg
    top_acc.merge(TRTSegmentFindingCfg( ConfigFlags,
                                        "",
                                        InputCombinedInDetTracks,
                                        'TRTSegments')) # InDetKeys.TRT_Segments
    #########################################################################################
    ############################### TRTStandalone configuration #############################
    top_acc.merge(TRTStandaloneCfg( ConfigFlags, 
                                    extension = "",
                                    InputCollections = [],
                                    BarrelSegments = 'TRTSegments')) # InDetKeys.TRT_Segments()

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5

    top_acc.run(25)
    top_acc.store(open("test_TRTStandaloneConfig.pkl", "wb"))
