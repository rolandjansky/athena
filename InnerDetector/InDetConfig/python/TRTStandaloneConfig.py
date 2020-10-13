# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
import InDetConfig.TrackingCommonConfig         as   TC

def InDetTrtTrackScoringToolCfg(flags, name ='InDetTRT_StandaloneScoringTool', TrackingCuts = None, extension = "", **kwargs):
    acc = ComponentAccumulator()

    #
    # --- set up special Scoring Tool for standalone TRT tracks
    #
    InDetTrackSummaryTool = acc.popToolsAndMerge(TC.InDetTrackSummaryToolCfg(flags))
    acc.addPublicTool(InDetTrackSummaryTool)

    InDetTRTDriftCircleCut = TC.InDetTRTDriftCircleCutForPatternRecoCfg(flags, TrackingCuts= TrackingCuts)
    acc.addPublicTool(InDetTRTDriftCircleCut)

    #
    # Cut values and output key for the TRT segments standalone TRT track finder
    #
    if extension == "_TRT":
        # TRT track segments
        pTmin = TrackingCuts.minPT
    else:
        # TRT standalone
        pTmin = TrackingCuts.minTRTonlyPt # new cut parameter to make it flexible...

    kwargs.setdefault("SummaryTool", InDetTrackSummaryTool)
    kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCut)
    kwargs.setdefault("useAmbigFcn", True)
    kwargs.setdefault("useSigmaChi2", False)
    kwargs.setdefault("PtMin", pTmin)
    kwargs.setdefault("minTRTonTrk", TrackingCuts.minTRTonly)
    kwargs.setdefault("maxEta", 2.1)
    kwargs.setdefault("UseParameterization", TrackingCuts.useTRTonlyParamCuts)
    kwargs.setdefault("OldTransitionLogic", TrackingCuts.useTRTonlyOldLogic)
    kwargs.setdefault("minTRTPrecisionFraction", TrackingCuts.minSecondaryTRTPrecFrac)

    InDetTRT_StandaloneScoringTool = CompFactory.InDet.InDetTrtTrackScoringTool(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_StandaloneScoringTool)
    return acc

def TRT_SegmentToTrackToolCfg(flags, name ='InDetTRT_SegmentToTrackTool', TrackingCuts = None, extension = "", usePrdAssociationTool = True, **kwargs):
    acc = ComponentAccumulator()
    #
    # set up TRT_SegmentToTrackTool
    #

    if usePrdAssociationTool:
        asso_tool = TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags)
        acc.addPublicTool(asso_tool)
    else:
        asso_tool = None

    InDetTrackFitterTRT = acc.popToolsAndMerge(TC.GlobalChi2FitterCfg(flags))
    acc.addPublicTool(InDetTrackFitterTRT)

    InDetTrackSummaryTool = acc.popToolsAndMerge(TC.InDetTrackSummaryToolCfg(flags))
    acc.addPublicTool(InDetTrackSummaryTool)

    from InDetConfig.InDetRecToolConfig  import InDetExtrapolatorCfg
    tmpAcc =  InDetExtrapolatorCfg(flags)
    InDetExtrapolator = tmpAcc.getPrimary()
    acc.merge(tmpAcc)
    
    InDetTRT_StandaloneScoringTool = acc.popToolsAndMerge(InDetTrtTrackScoringToolCfg(flags,name='InDetTRT_StandaloneScoringTool'+ extension, 
                                                                                            TrackingCuts = TrackingCuts,
                                                                                            extension=extension))
    acc.addPublicTool(InDetTRT_StandaloneScoringTool)
    
    kwargs.setdefault("RefitterTool", InDetTrackFitterTRT)
    kwargs.setdefault("AssociationTool", asso_tool)
    kwargs.setdefault("TrackSummaryTool", InDetTrackSummaryTool)
    kwargs.setdefault("ScoringTool", InDetTRT_StandaloneScoringTool)
    kwargs.setdefault("Extrapolator", InDetExtrapolator)
    kwargs.setdefault("FinalRefit", True)
    kwargs.setdefault("MaxSharedHitsFraction", TrackingCuts.maxTRTonlyShared)
    kwargs.setdefault("SuppressHoleSearch", True)

    InDetTRT_SegmentToTrackTool = CompFactory.InDet.TRT_SegmentToTrackTool(name = name, **kwargs)
    acc.setPrivateTools(InDetTRT_SegmentToTrackTool)
    return acc

def TRT_StandaloneTrackFinderCfg(flags, name ='InDetTRT_StandaloneTrackFinder', TrackingCuts = None, extension = "", BarrelSegments = None, prd_to_track_map = '', **kwargs):
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
                                                                                        TrackingCuts = TrackingCuts,
                                                                                        extension=extension,
                                                                                        usePrdAssociationTool = usePrdAssociationTool))
    acc.addPublicTool(InDetTRT_SegmentToTrackTool)

    kwargs.setdefault("MinNumDriftCircles", TrackingCuts.minTRTonly)
    kwargs.setdefault("MinPt", TrackingCuts.minTRTonlyPt)
    kwargs.setdefault("InputSegmentsLocation", BarrelSegments)
    kwargs.setdefault("MaterialEffects", 0)
    kwargs.setdefault("PRDtoTrackMap", prd_to_track_map)
    kwargs.setdefault("OldTransitionLogic", TrackingCuts.useTRTonlyOldLogic)
    kwargs.setdefault("OutputTracksLocation", TRTStandaloneTracks)
    kwargs.setdefault("TRT_SegToTrackTool", InDetTRT_SegmentToTrackTool)

    InDetTRT_StandaloneTrackFinder = CompFactory.InDet.TRT_StandaloneTrackFinder(name = name, **kwargs)
    acc.addEventAlgo(InDetTRT_StandaloneTrackFinder)
    return acc

def TRT_SegmentsToTrackCfg( flags, name ='InDetTRT_SegmentsToTrack_Barrel', TrackingCuts = None, extension = "", BarrelSegments = None, prd_to_track_map = '', **kwargs):
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
                                                                                        TrackingCuts = TrackingCuts,
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

    InDetPRDtoTrackMapToolGangedPixels = TC.InDetPRDtoTrackMapToolGangedPixelsCfg(flags)
    acc.addPublicTool(InDetPRDtoTrackMapToolGangedPixels)

    kwargs.setdefault("InputSegmentsCollection", BarrelSegments)
    kwargs.setdefault("OutputTrackCollection", TRTStandaloneTracks)
    kwargs.setdefault("TrackFitter", InDetTrackFitter)
    kwargs.setdefault("SummaryTool", InDetTrackSummaryToolTRTTracks)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels if prd_to_track_map !='' else None,)
    kwargs.setdefault("InputAssociationMapName", prd_to_track_map)
    kwargs.setdefault("MinNHit", TrackingCuts.minTRTonly)
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
def TRTStandaloneCfg( flags, extension = '', InputCollections = None, NewTrackingCuts = None, BarrelSegments = None, PRDtoTrackMap = ''):
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
                                                        TrackingCuts = NewTrackingCuts,
                                                        extension=extension,
                                                        BarrelSegments = BarrelSegments,
                                                        prd_to_track_map = prd_to_track_map))
    else: 
        #
        # --- cosmics segment to track conversion for Barrel
        #
        acc.merge(TRT_SegmentsToTrackCfg(flags, name = 'InDetTRT_SegmentsToTrack_Barrel'+extension,
                                                TrackingCuts = NewTrackingCuts,
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

    ConfigFlags.Detector.RecoTRT = True
    ConfigFlags.Detector.RecoIBL = True

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

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    top_acc.merge(MagneticFieldSvcCfg(ConfigFlags))

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    top_acc.merge(TRT_GeometryCfg( ConfigFlags ))

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    top_acc.merge( PixelGeometryCfg(ConfigFlags) )

    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    top_acc.merge(SCT_GeometryCfg(ConfigFlags))

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
    top_acc.merge(addFoldersSplitOnline(ConfigFlags, "TRT", "/TRT/Onl/Calib/PID_vector", "/TRT/Calib/PID_vector", className='CondAttrListVec'))
    # HT probability algorithm
    TRTHTCondAlg = CompFactory.TRTHTCondAlg(name = "TRTHTCondAlg", HTWriteKey = "HTcalculator")
    top_acc.addCondAlgo(TRTHTCondAlg)
    ###
    ###
    top_acc.merge(addFoldersSplitOnline(ConfigFlags, "PIXEL", "/PIXEL/PixdEdx", "/PIXEL/PixdEdx", className='AthenaAttributeList'))

    PixeldEdxAlg = CompFactory.PixeldEdxAlg(name="PixeldEdxAlg", ReadFromCOOL = True)
    top_acc.addCondAlgo(PixeldEdxAlg)
    ###
    ###
    top_acc.merge(addFoldersSplitOnline(ConfigFlags, "TRT", "/TRT/Onl/Cond/Status", "/TRT/Cond/Status", className='TRTCond::StrawStatusMultChanContainer'))

    InDetTRTStrawStatusSummaryTool = top_acc.popToolsAndMerge(TC.InDetTRTStrawStatusSummaryToolCfg(ConfigFlags))
    top_acc.addPublicTool(InDetTRTStrawStatusSummaryTool)

    TRTStrawCondAlg = CompFactory.TRTStrawCondAlg(  name="TRTStrawCondAlg", 
                                                    TRTStrawStatusSummaryTool = InDetTRTStrawStatusSummaryTool,
                                                    StrawWriteKey  = "AliveStraws",
                                                    isGEANT4 = ConfigFlags.Input.isMC)
    top_acc.addCondAlgo(TRTStrawCondAlg)
    ###
    ###
    top_acc.merge(addFoldersSplitOnline(ConfigFlags, "TRT", "/TRT/Onl/Calib/ToT/ToTVectors", "/TRT/Calib/ToT/ToTVectors", className='CondAttrListVec'))
    top_acc.merge(addFoldersSplitOnline(ConfigFlags, "TRT", "/TRT/Onl/Calib/ToT/ToTValue", "/TRT/Calib/ToT/ToTValue", className='CondAttrListCollection'))

    TRTToTCondAlg = CompFactory.TRTToTCondAlg(  name        = "TRTToTCondAlg",
                                                ToTWriteKey = "Dedxcorrection")
    top_acc.addCondAlgo(TRTToTCondAlg)
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
    from InDetConfig.TRTSegmentFindingConfig import TRTActiveCondAlgCfg, TRTSegmentFindingCfg

    top_acc.merge(TRTActiveCondAlgCfg(ConfigFlags))
    top_acc.merge(TC.TRT_DetElementsRoadCondAlgCfg())

    from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
    if not ConfigFlags.InDet.doDBMstandalone:
        top_acc.merge(TRTPreProcessingCfg(ConfigFlags,(not ConfigFlags.InDet.doTRTPhaseCalculation or ConfigFlags.Beam.Type =="collisions"),False))

    InDetNewTrackingCuts = ConfigFlags.InDet.Tracking
    # NewTracking collection keys
    InputCombinedInDetTracks = []

    top_acc.merge(TRTSegmentFindingCfg( ConfigFlags,
                                        "",
                                        InputCombinedInDetTracks,
                                        InDetNewTrackingCuts,
                                        'TRTSegments')) # InDetKeys.TRT_Segments

    ############################### TRTStandalone configuration #############################
    top_acc.merge(TRTStandaloneCfg( ConfigFlags, 
                                    extension = "",
                                    InputCollections = [],
                                    NewTrackingCuts = InDetNewTrackingCuts,
                                    BarrelSegments = 'TRTSegments')) # InDetKeys.TRT_Segments()

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5

    top_acc.getService('StoreGateSvc').Dump = True
    top_acc.printConfig(withDetails = True, summariseProps = True)
    top_acc.run(25)
    top_acc.store(open("test_TRTStandaloneConfig.pkl", "wb"))