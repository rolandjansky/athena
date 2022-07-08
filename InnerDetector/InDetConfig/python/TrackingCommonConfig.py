# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

#######################################################################

def LumiCondDataKeyForTRTMuScalingCfg(flags, **kwargs) :
    acc = ComponentAccumulator()
    LuminosityOutputKey = ''
    if flags.Beam.Type is not BeamType.Cosmics:
        from LumiBlockComps.LuminosityCondAlgConfig import LuminosityCondAlgCfg
        LuminosityCondAlg = LuminosityCondAlgCfg (flags)
        acc.merge(LuminosityCondAlg)
        LuminosityOutputKey = LuminosityCondAlg.getCondAlgo ('LuminosityCondAlg').LuminosityOutputKey

    return acc, LuminosityOutputKey

def InDetTRT_DriftCircleOnTrackToolCfg(flags, name='TRT_DriftCircleOnTrackTool', **kwargs):
    acc = ComponentAccumulator()
    from TrkConfig.TrkRIO_OnTrackCreatorConfig import RIO_OnTrackErrorScalingCondAlgCfg
    acc.merge(RIO_OnTrackErrorScalingCondAlgCfg(flags))
    kwargs.setdefault("TRTErrorScalingKey", '/Indet/TrkErrorScalingTRT')
    tmpAcc, LuminosityOutputKey = LumiCondDataKeyForTRTMuScalingCfg(flags)
    acc.merge(tmpAcc)
    kwargs.setdefault("LumiDataKey", LuminosityOutputKey)
    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackTool(name, **kwargs))
    return acc

def TRT_DriftCircleOnTrackNoDriftTimeToolCfg(name='TRT_DriftCircleOnTrackNoDriftTimeTool', **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackNoDriftTimeTool(name, **kwargs))
    return acc

def InDetTRT_DriftCircleOnTrackUniversalToolCfg(name='InDetTRT_RefitRotCreator', **kwargs):
    acc = ComponentAccumulator()
    default_ScaleHitUncertainty = 2.5

    if 'RIOonTrackToolDrift' not in kwargs:
        RIOonTrackToolDrift = acc.popToolsAndMerge(TRT_DriftCircleOnTrackNoDriftTimeToolCfg())
        kwargs.setdefault("RIOonTrackToolDrift", RIOonTrackToolDrift)
    if 'RIOonTrackToolTube' not in kwargs:
        RIOonTrackToolTube = acc.popToolsAndMerge(TRT_DriftCircleOnTrackNoDriftTimeToolCfg())
        kwargs.setdefault("RIOonTrackToolTube", RIOonTrackToolTube)
    kwargs.setdefault("ScaleHitUncertainty", default_ScaleHitUncertainty) 
    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackUniversalTool(name, **kwargs))
    return acc

def InDetTRTDriftCircleCutForPatternRecoCfg(flags, name='InDetTRTDriftCircleCutForPatternReco', **kwargs):
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTActiveCondAlgCfg
    result = TRTActiveCondAlgCfg(flags) # To produce the input TRTCond::ActiveFraction CondHandle
    kwargs.setdefault("MinOffsetDCs", 5)
    kwargs.setdefault("UseNewParameterization", flags.InDet.Tracking.ActivePass.useNewParameterizationTRT)
    kwargs.setdefault("UseActiveFractionSvc", flags.Detector.EnableTRT)
    result.setPrivateTools(CompFactory.InDet.InDetTrtDriftCircleCutTool(name, **kwargs))
    return result

def PixeldEdxAlg(flags, name = "PixeldEdxAlg", **kwargs):
    acc = ComponentAccumulator()
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/PixdEdx", "/PIXEL/PixdEdx", className='AthenaAttributeList'))
    PixeldEdxAlg = CompFactory.PixeldEdxAlg(name=name, ReadFromCOOL = True, **kwargs)
    acc.addCondAlgo(PixeldEdxAlg)
    return acc

def InDetPixelToTPIDToolCfg(flags, name = "InDetPixelToTPIDTool", **kwargs):
    acc = PixeldEdxAlg(flags)
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelConfigCondAlgCfg
    acc.merge(PixelConfigCondAlgCfg(flags)) # To produce PixelModuleData CondHandle
    InDetPixelToTPIDTool = CompFactory.InDet.PixelToTPIDTool(name, **kwargs)
    acc.setPrivateTools(InDetPixelToTPIDTool)
    return acc

#############################################################################################
#TRTSegmentFinder
#############################################################################################

def InDetTRT_DriftCircleOnTrackUniversalToolCosmicsCfg(name='TRT_DriftCircleOnTrackUniversalTool', **kwargs):
    kwargs.setdefault("ScaleHitUncertainty", 2.)
    return InDetTRT_DriftCircleOnTrackUniversalToolCfg(name=name, **kwargs)

def InDetTRT_ExtensionToolCosmicsCfg(flags, name='InDetTRT_ExtensionToolCosmics', **kwargs):
    acc = ComponentAccumulator()

    if 'Propagator' not in kwargs:
        from TrkConfig.TrkExRungeKuttaPropagatorConfig import InDetPropagatorCfg
        InDetPropagator = acc.popToolsAndMerge(InDetPropagatorCfg(flags))
        acc.addPublicTool(InDetPropagator)
        kwargs.setdefault("Propagator", InDetPropagator)

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    if 'RIOonTrackToolYesDr' not in kwargs:
        InDetTRT_DriftCircleOnTrackUniversalToolCosmics = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCosmicsCfg())
        kwargs.setdefault("RIOonTrackToolYesDr", InDetTRT_DriftCircleOnTrackUniversalToolCosmics)

    if 'RIOonTrackToolNoDr' not in kwargs:
        TRT_DriftCircleOnTrackNoDriftTimeTool = acc.popToolsAndMerge(TRT_DriftCircleOnTrackNoDriftTimeToolCfg())
        kwargs.setdefault("RIOonTrackToolNoDr", TRT_DriftCircleOnTrackNoDriftTimeTool)

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCircles') # InDetKeys.TRT_DriftCircles()
    kwargs.setdefault("SearchNeighbour", False)   # needs debugging!!!
    kwargs.setdefault("RoadWidth", 10.0)
    acc.setPrivateTools(CompFactory.InDet.TRT_TrackExtensionToolCosmics(name, **kwargs))
    return acc

def InDetTRT_TrackExtensionTool_xkCfg(flags, name='InDetTRT_ExtensionTool', **kwargs):
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    acc = MagneticFieldSvcCfg(flags)

    if 'PropagatorTool' not in kwargs:
        from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
        InDetPatternPropagator = acc.popToolsAndMerge(RungeKuttaPropagatorCfg(flags, name="InDetPatternPropagator"))
        acc.addPublicTool(InDetPatternPropagator)
        kwargs.setdefault("PropagatorTool", InDetPatternPropagator)

    if 'UpdatorTool' not in kwargs:
        from TrkConfig.TrkMeasurementUpdatorConfig import KalmanUpdator_xkCfg
        InDetPatternUpdator = acc.popToolsAndMerge(KalmanUpdator_xkCfg(flags, name="InDetPatternUpdator"))
        acc.addPublicTool(InDetPatternUpdator)
        kwargs.setdefault("UpdatorTool", InDetPatternUpdator)

    if 'DriftCircleCutTool' not in kwargs:
        InDetTRTDriftCircleCutForPatternReco = acc.popToolsAndMerge(InDetTRTDriftCircleCutForPatternRecoCfg(flags))
        kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCutForPatternReco)

    if 'RIOonTrackToolYesDr' not in kwargs:
        kwargs.setdefault("RIOonTrackToolYesDr", acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackToolCfg(flags)))


    if 'RIOonTrackToolNoDr' not in kwargs:
        TRT_DriftCircleOnTrackNoDriftTimeTool = acc.popToolsAndMerge(TRT_DriftCircleOnTrackNoDriftTimeToolCfg())
        kwargs.setdefault("RIOonTrackToolNoDr", TRT_DriftCircleOnTrackNoDriftTimeTool)

    if 'RoadTool' not in kwargs:
        InDetTRT_RoadMaker = acc.popToolsAndMerge(InDetTRT_RoadMakerCfg(flags))
        kwargs.setdefault("RoadTool", InDetTRT_RoadMaker)

    kwargs.setdefault("TRT_ClustersContainer", "TRT_DriftCircles")
    kwargs.setdefault("TrtManagerLocation", "TRT")
    kwargs.setdefault("UseDriftRadius", not flags.InDet.noTRTTiming)
    kwargs.setdefault("MinNumberDriftCircles", flags.InDet.Tracking.ActivePass.minTRTonTrk)
    kwargs.setdefault("ScaleHitUncertainty", 2)
    kwargs.setdefault("RoadWidth", 20.)
    kwargs.setdefault("UseParameterization", flags.InDet.Tracking.ActivePass.useParameterizedTRTCuts)
    kwargs.setdefault("maxImpactParameter", 500 if flags.InDet.Tracking.doBeamGas else 50 )  # single beam running, open cuts

    if flags.InDet.Tracking.ActivePass.RoISeededBackTracking:
        kwargs.setdefault("minTRTSegmentpT", flags.InDet.Tracking.ActivePass.minSecondaryPt)

    acc.setPrivateTools(CompFactory.InDet.TRT_TrackExtensionTool_xk(name, **kwargs))
    return acc

def InDetWeightCalculatorCfg(name='InDetWeightCalculator', **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.DAF_SimpleWeightCalculator(name, **kwargs))
    return acc

def InDetCompetingTRT_DC_ToolCfg(flags, name='InDetCompetingTRT_DC_Tool', **kwargs):
    acc = ComponentAccumulator()

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    if 'ToolForWeightCalculation' not in kwargs:
        InDetWeightCalculator = acc.popToolsAndMerge(InDetWeightCalculatorCfg())
        kwargs.setdefault("ToolForWeightCalculation", InDetWeightCalculator)

    if 'ToolForTRT_DriftCircleOnTrackCreation' not in kwargs:
        kwargs.setdefault("ToolForTRT_DriftCircleOnTrackCreation", acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackToolCfg(flags)))

    acc.setPrivateTools(CompFactory.InDet.CompetingTRT_DriftCirclesOnTrackTool(name, **kwargs))
    return acc

def InDetTRT_RoadMakerCfg(flags, name='InDetTRT_RoadMaker', **kwargs):
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    acc = MagneticFieldSvcCfg(flags)
    acc.merge(TRT_DetElementsRoadCondAlgCfg(flags)) # To produce the input TRT_DetElementsRoadData_xk CondHandle
    
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
    InDetPatternPropagator = acc.popToolsAndMerge(RungeKuttaPropagatorCfg(flags, name="InDetPatternPropagator"))
    kwargs.setdefault("RoadWidth", 20.)
    kwargs.setdefault("PropagatorTool", InDetPatternPropagator)
    acc.setPrivateTools(CompFactory.InDet.TRT_DetElementsRoadMaker_xk(name, **kwargs))
    return acc

def InDetTRT_TrackExtensionTool_DAFCfg(flags, name='TRT_TrackExtensionTool_DAF',**kwargs):
    acc = ComponentAccumulator()

    if 'CompetingDriftCircleTool' not in kwargs:
        InDetCompetingTRT_DC_Tool = acc.popToolsAndMerge(InDetCompetingTRT_DC_ToolCfg(flags))
        kwargs.setdefault("CompetingDriftCircleTool", InDetCompetingTRT_DC_Tool)

    if 'PropagatorTool' not in kwargs:
        from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
        InDetPatternPropagator = acc.popToolsAndMerge(RungeKuttaPropagatorCfg(flags, name="InDetPatternPropagator"))
        acc.addPublicTool(InDetPatternPropagator)
        kwargs.setdefault("PropagatorTool", InDetPatternPropagator)

    if 'RoadTool' not in kwargs:
        InDetTRT_RoadMaker = acc.popToolsAndMerge(InDetTRT_RoadMakerCfg(flags)) 
        kwargs.setdefault("RoadTool", InDetTRT_RoadMaker)

    kwargs.setdefault("TRT_DriftCircleContainer", 'TRT_DriftCircles') # InDetKeys.TRT_DriftCircles()

    acc.setPrivateTools(CompFactory.InDet.TRT_TrackExtensionTool_DAF(name,**kwargs))
    return acc

def TRT_DetElementsRoadCondAlgCfg(flags, name="InDet__TRT_DetElementsRoadCondAlg_xk", **kwargs):
    acc = ComponentAccumulator()
    acc.addCondAlgo(CompFactory.InDet.TRT_DetElementsRoadCondAlg_xk(name, **kwargs))
    return acc

def InDetTRT_ExtensionToolCfg(flags, **kwargs):
    # @TODO set all names to InDetTRT_ExtensionTool ?
    if flags.InDet.Tracking.trtExtensionType == 'xk':
        if flags.Beam.Type is BeamType.Cosmics:
            return InDetTRT_ExtensionToolCosmicsCfg(flags, **kwargs)
        else:
            return InDetTRT_TrackExtensionTool_xkCfg(flags, **kwargs)
    if flags.InDet.Tracking.trtExtensionType == 'DAF':
        return InDetTRT_TrackExtensionTool_DAFCfg(flags, name='InDetTRT_ExtensionTool', **kwargs)

#############################################################################################
# BackTracking
#############################################################################################

def InDetAmbiScoringToolBaseCfg(flags, name='InDetAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    kwargs.setdefault("Extrapolator", acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolCfg
    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))

    if 'DriftCircleCutTool' not in kwargs:
        InDetTRTDriftCircleCutForPatternReco = acc.popToolsAndMerge(InDetTRTDriftCircleCutForPatternRecoCfg(flags))
        kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCutForPatternReco )

    have_calo_rois = flags.InDet.Tracking.doBremRecovery and flags.InDet.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois:
        from InDetConfig.InDetCaloClusterROISelectorConfig import CaloClusterROIPhiRZContainerMakerCfg
        acc.merge(CaloClusterROIPhiRZContainerMakerCfg(flags))
        kwargs.setdefault("EMROIPhiRZContainer", "InDetCaloClusterROIPhiRZ5GeV")
    kwargs.setdefault("SummaryTool", InDetTrackSummaryTool )
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxZImp", flags.InDet.Tracking.ActivePass.maxZImpact )
    kwargs.setdefault("maxEta", flags.InDet.Tracking.ActivePass.maxEta )
    kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel )
    kwargs.setdefault("useSCT", flags.InDet.Tracking.ActivePass.useSCT )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )
    acc.setPrivateTools(CompFactory.InDet.InDetAmbiScoringTool(name, **kwargs))
    return acc

def InDetCosmicsScoringToolBaseCfg(flags, name='InDetCosmicsScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolCfg
    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))
    acc.addPublicTool(InDetTrackSummaryTool)

    kwargs.setdefault("nWeightedClustersMin", flags.InDet.Tracking.ActivePass.nWeightedClustersMin )
    kwargs.setdefault("minTRTHits", 0 )
    kwargs.setdefault("SummaryTool", InDetTrackSummaryTool )

    acc.setPrivateTools(CompFactory.InDet.InDetCosmicScoringTool(name, **kwargs ))
    return acc

def InDetTRT_ExtensionToolPhaseCfg(flags, name='InDetTRT_ExtensionToolPhase', **kwargs) :
    acc = ComponentAccumulator()
    InDetTRT_DriftCircleOnTrackUniversalTool = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCfg())
    if 'RIOonTrackToolYesDr' not in kwargs:
        kwargs.setdefault("RIOonTrackToolYesDr", InDetTRT_DriftCircleOnTrackUniversalTool)

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCirclesUncalibrated')
    kwargs.setdefault("RoadWidth", 20.0)
    acc.setPrivateTools(acc.popToolsAndMerge(InDetTRT_ExtensionToolCosmicsCfg(flags, name = name, **kwargs)))
    return acc

def InDetCosmicExtenScoringToolCfg(flags, name='InDetCosmicExtenScoringTool',**kwargs):
    kwargs.setdefault("nWeightedClustersMin", 0)
    kwargs.setdefault("minTRTHits", flags.InDet.Tracking.ActivePass.minTRTonTrk )
    return InDetCosmicsScoringToolBaseCfg(flags, name, **kwargs)

def InDetCosmicScoringTool_TRTCfg(flags, name='InDetCosmicScoringTool_TRT',**kwargs):
    acc = ComponentAccumulator()
    from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolNoHoleSearchCfg
    InDetTrackSummaryToolNoHoleSearch = acc.popToolsAndMerge(InDetTrackSummaryToolNoHoleSearchCfg(flags))
    acc.addPublicTool(InDetTrackSummaryToolNoHoleSearch)

    kwargs.setdefault("minTRTHits", flags.InDet.Tracking.ActivePass.minSecondaryTRTonTrk)
    kwargs.setdefault("SummaryTool", InDetTrackSummaryToolNoHoleSearch)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetCosmicExtenScoringToolCfg(flags, name, **kwargs)))
    return acc

def InDetTRT_SeededScoringToolCfg(flags, name='InDetTRT_SeededScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("useAmbigFcn", False)
    kwargs.setdefault("useTRT_AmbigFcn", True)
    kwargs.setdefault("minTRTonTrk", flags.InDet.Tracking.ActivePass.minSecondaryTRTonTrk)
    kwargs.setdefault("minTRTPrecisionFraction", flags.InDet.Tracking.ActivePass.minSecondaryTRTPrecFrac)
    kwargs.setdefault("minPt", flags.InDet.Tracking.ActivePass.minSecondaryPt)
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.ActivePass.maxSecondaryImpact)
    kwargs.setdefault("minSiClusters", flags.InDet.Tracking.ActivePass.minSecondaryClusters)
    kwargs.setdefault("maxSiHoles", flags.InDet.Tracking.ActivePass.maxSecondaryHoles)
    kwargs.setdefault("maxPixelHoles", flags.InDet.Tracking.ActivePass.maxSecondaryPixelHoles)
    kwargs.setdefault("maxSCTHoles", flags.InDet.Tracking.ActivePass.maxSecondarySCTHoles)
    kwargs.setdefault("maxDoubleHoles", flags.InDet.Tracking.ActivePass.maxSecondaryDoubleHoles)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetAmbiScoringToolBaseCfg(flags, name=name, **kwargs)))
    return acc

#########################################################################################################
#TRTExtension
#########################################################################################################

def InDetAmbiScoringToolCfg(flags, name='InDetAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("minTRTonTrk", 0 )
    kwargs.setdefault("minTRTPrecisionFraction", 0 )
    kwargs.setdefault("minPt", flags.InDet.Tracking.ActivePass.minPT )
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.ActivePass.maxPrimaryImpact )
    kwargs.setdefault("minSiClusters", flags.InDet.Tracking.ActivePass.minClusters )
    kwargs.setdefault("minPixel", flags.InDet.Tracking.ActivePass.minPixel )
    kwargs.setdefault("maxSiHoles", flags.InDet.Tracking.ActivePass.maxHoles )
    kwargs.setdefault("maxPixelHoles", flags.InDet.Tracking.ActivePass.maxPixelHoles )
    kwargs.setdefault("maxSCTHoles", flags.InDet.Tracking.ActivePass.maxSctHoles )
    kwargs.setdefault("maxDoubleHoles", flags.InDet.Tracking.ActivePass.maxDoubleHoles )
    acc.setPrivateTools(acc.popToolsAndMerge(InDetAmbiScoringToolBaseCfg(flags, name + flags.InDet.Tracking.ActivePass.extension, **kwargs)))
    return acc

def InDetAmbiScoringToolSiCfg(flags, name='InDetAmbiScoringToolSi', **kwargs) :
    kwargs.setdefault('DriftCircleCutTool','')
    return InDetAmbiScoringToolCfg(flags, name, **kwargs)

def InDetExtenScoringToolCfg(flags, name='InDetExtenScoringTool', **kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("minTRTonTrk", flags.InDet.Tracking.ActivePass.minTRTonTrk)
    kwargs.setdefault("minTRTPrecisionFraction", flags.InDet.Tracking.ActivePass.minTRTPrecFrac)
    acc.setPrivateTools(acc.popToolsAndMerge(InDetAmbiScoringToolCfg(flags, name = name,  **kwargs)))
    return acc

#############################################################################################
#TrackingSiPatternConfig
#############################################################################################

def PRDtoTrackMapToolCfg(name='PRDtoTrackMapTool',**kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.PRDtoTrackMapTool( name, **kwargs))
    return acc

def InDetNNScoringToolBaseCfg(flags, name='InDetNNScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    have_calo_rois = flags.InDet.Tracking.doBremRecovery and flags.InDet.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois :
        from InDetConfig.InDetCaloClusterROISelectorConfig import CaloClusterROIPhiRZContainerMakerCfg
        acc.merge(CaloClusterROIPhiRZContainerMakerCfg(flags))
        kwargs.setdefault("EMROIPhiRZContainer", "InDetCaloClusterROIPhiRZ5GeV")

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))
    from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolCfg
    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))

    if 'DriftCircleCutTool' not in kwargs:
        InDetTRTDriftCircleCutForPatternReco = acc.popToolsAndMerge(InDetTRTDriftCircleCutForPatternRecoCfg(flags))
        kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCutForPatternReco )

    kwargs.setdefault("nnCutConfig", "dev/TrackingCP/LRTAmbiNetwork/20200727_225401/nn-config.json" )
    kwargs.setdefault("nnCutThreshold", flags.InDet.Tracking.nnCutLargeD0Threshold )
    kwargs.setdefault("Extrapolator", InDetExtrapolator )
    kwargs.setdefault("SummaryTool", InDetTrackSummaryTool )
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxZImp", flags.InDet.Tracking.ActivePass.maxZImpact )
    kwargs.setdefault("maxEta", flags.InDet.Tracking.ActivePass.maxEta )
    kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel )
    kwargs.setdefault("useSCT", flags.InDet.Tracking.ActivePass.useSCT )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )

    acc.setPrivateTools(CompFactory.InDet.InDetNNScoringTool(name, **kwargs ))
    return acc

def InDetNNScoringToolCfg(flags, name='InDetNNScoringTool', **kwargs) :
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("minTRTonTrk", 0 )
    kwargs.setdefault("minTRTPrecisionFraction", 0 )
    kwargs.setdefault("minPt", flags.InDet.Tracking.ActivePass.minPT )
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.ActivePass.maxPrimaryImpact )
    kwargs.setdefault("minSiClusters", flags.InDet.Tracking.ActivePass.minClusters )
    kwargs.setdefault("minPixel", flags.InDet.Tracking.ActivePass.minPixel )
    kwargs.setdefault("maxSiHoles", flags.InDet.Tracking.ActivePass.maxHoles )
    kwargs.setdefault("maxPixelHoles", flags.InDet.Tracking.ActivePass.maxPixelHoles )
    kwargs.setdefault("maxSCTHoles", flags.InDet.Tracking.ActivePass.maxSctHoles )
    kwargs.setdefault("maxDoubleHoles", flags.InDet.Tracking.ActivePass.maxDoubleHoles)

    return InDetNNScoringToolBaseCfg(flags, name=name+flags.InDet.Tracking.ActivePass.extension, **kwargs )

def InDetNNScoringToolSiCfg(flags, name='InDetNNScoringToolSi', **kwargs) :
    kwargs.setdefault('DriftCircleCutTool','')
    return InDetNNScoringToolCfg(flags,name, **kwargs)

def InDetCosmicsScoringToolCfg(flags, name='InDetCosmicsScoringTool', **kwargs) :
    return InDetCosmicsScoringToolBaseCfg(flags,
                                          name=name+flags.InDet.Tracking.ActivePass.extension)

def FullLinearizedTrackFactoryCfg(flags, name='TrackToVertexIPEstimator', **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg

    acc = InDetExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", acc.getPrimary()) #private
    # FIXME Configure AtlasFieldCacheCondObj
    acc.setPrivateTools(CompFactory.Trk.FullLinearizedTrackFactory( **kwargs))
    return acc

def TrackToVertexIPEstimatorCfg(flags, name='TrackToVertexIPEstimator', **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg

    # All sub-tools are currently public
    acc = FullLinearizedTrackFactoryCfg(flags, 'FullLinearizedTrkFactory')
    linearizedTrackFactory = acc.popPrivateTools()
    acc.addPublicTool(linearizedTrackFactory)
    kwargs.setdefault("LinearizedTrackFactory", linearizedTrackFactory )
    if 'Extrapolator' not in kwargs:
        extrapolator = acc.getPrimaryAndMerge( InDetExtrapolatorCfg(flags) )
        kwargs.setdefault("Extrapolator", extrapolator)
    # Think it's okay not to configure VertexUpdator
    acc.setPrivateTools(CompFactory.Trk.TrackToVertexIPEstimator( **kwargs))
    return acc
