# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
import AthenaCommon.SystemOfUnits as Units
#######################################################################

def NeuralNetworkToHistoToolCfg(**kwargs):
    acc = ComponentAccumulator()
    name = kwargs.pop('name',"NeuralNetworkToHistoTool")

    NeuralNetworkToHistoTool=CompFactory.Trk.NeuralNetworkToHistoTool(name, **kwargs)
    acc.setPrivateTools(NeuralNetworkToHistoTool)
    return acc

def PixelClusterNnCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    track_nn = kwargs.pop('TrackNetwork',False)
    nn_names = [
          "NumberParticles_NoTrack/",
          "ImpactPoints1P_NoTrack/",
          "ImpactPoints2P_NoTrack/",
          "ImpactPoints3P_NoTrack/",
          "ImpactPointErrorsX1_NoTrack/",
          "ImpactPointErrorsX2_NoTrack/",
          "ImpactPointErrorsX3_NoTrack/",
          "ImpactPointErrorsY1_NoTrack/",
          "ImpactPointErrorsY2_NoTrack/",
          "ImpactPointErrorsY3_NoTrack/" ]

    if track_nn :
        nn_names = [ elm.replace('_NoTrack', '')  for elm in nn_names ]

    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixelClustering/PixelClusNNCalib", "/PIXEL/PixelClustering/PixelClusNNCalib", className='CondAttrListCollection'))
    kwargs.setdefault("NetworkNames", nn_names)
    kwargs.setdefault("WriteKey", 'PixelClusterNN' if not track_nn else 'PixelClusterNNWithTrack')

    if 'NetworkToHistoTool' not in kwargs:
        NeuralNetworkToHistoTool = acc.popToolsAndMerge(NeuralNetworkToHistoToolCfg(name = "NeuralNetworkToHistoTool"))
        kwargs.setdefault("NetworkToHistoTool", NeuralNetworkToHistoTool)

    acc.addCondAlgo(CompFactory.InDet.TTrainedNetworkCondAlg(kwargs.pop("name", 'PixelClusterNnCondAlg'), **kwargs))
    return acc

def PixelClusterNnWithTrackCondAlgCfg(flags, **kwargs):
    kwargs.setdefault("TrackNetwork", True)
    kwargs.setdefault("name", 'PixelClusterNnWithTrackCondAlg')

    acc = PixelClusterNnCondAlgCfg(flags, **kwargs)
    return acc

def LWTNNCondAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    # Check for the folder
    acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixelClustering/PixelNNCalibJSON", "/PIXEL/PixelClustering/PixelNNCalibJSON", className="CondAttrListCollection"))
    # What we'll store it as
    kwargs.setdefault("WriteKey", 'PixelClusterNNJSON')

    # Set up the algorithm
    acc.addCondAlgo(CompFactory.InDet.LWTNNCondAlg(kwargs.pop("name", "LWTNNCondAlg"), **kwargs))
    return acc

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

def InDetBroadTRT_DriftCircleOnTrackToolCfg(name='InDetBroadTRT_DriftCircleOnTrackTool', **kwargs):
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackNoDriftTimeTool(name, **kwargs))
    return acc

def InDetTRT_DriftCircleOnTrackUniversalToolCfg(name='InDetTRT_RefitRotCreator', **kwargs):
    acc = ComponentAccumulator()
    default_ScaleHitUncertainty = 2.5

    if 'RIOonTrackToolDrift' not in kwargs:
        RIOonTrackToolDrift = acc.popToolsAndMerge(InDetBroadTRT_DriftCircleOnTrackToolCfg())
        kwargs.setdefault("RIOonTrackToolDrift", RIOonTrackToolDrift)
    if 'RIOonTrackToolTube' not in kwargs:
        RIOonTrackToolTube = acc.popToolsAndMerge(InDetBroadTRT_DriftCircleOnTrackToolCfg())
        kwargs.setdefault("RIOonTrackToolTube", RIOonTrackToolTube)
    kwargs.setdefault("ScaleHitUncertainty", default_ScaleHitUncertainty) 
    acc.setPrivateTools(CompFactory.InDet.TRT_DriftCircleOnTrackUniversalTool(name, **kwargs))
    return acc

def InDetPRDtoTrackMapToolGangedPixelsCfg(flags, name='PRDtoTrackMapToolGangedPixels', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'PixelClusterAmbiguitiesMap')
    kwargs.setdefault("addTRToutliers", True)
    acc.setPrivateTools(CompFactory.InDet.InDetPRDtoTrackMapToolGangedPixels(name, **kwargs))
    return acc

def InDetTrackPRD_AssociationCfg(flags, name='InDetTrackPRD_Association', **kwargs):
    acc = ComponentAccumulator()

    if kwargs.get('TracksName', None) is None :
        raise Exception('Not TracksName argument provided')

    if 'AssociationTool' not in kwargs:
        kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(InDetPRDtoTrackMapToolGangedPixelsCfg(flags)))

    if "AssociationMapName" not in kwargs:
        kwargs.setdefault("AssociationMapName", "InDetPRDtoTrackMap" + flags.InDet.Tracking.ActivePass.extension)

    acc.addEventAlgo(CompFactory.InDet.InDetTrackPRD_Association(name, **kwargs))
    return acc

def InDetTRTDriftCircleCutForPatternRecoCfg(flags, name='InDetTRTDriftCircleCutForPatternReco', **kwargs):
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTActiveCondAlgCfg
    result = TRTActiveCondAlgCfg(flags) # To produce the input TRTCond::ActiveFraction CondHandle
    kwargs.setdefault("MinOffsetDCs", 5)
    kwargs.setdefault("UseNewParameterization", flags.InDet.Tracking.ActivePass.useNewParameterizationTRT)
    kwargs.setdefault("UseActiveFractionSvc", flags.Detector.EnableTRT)
    result.setPrivateTools(CompFactory.InDet.InDetTrtDriftCircleCutTool(name, **kwargs))
    return result

def InDetTrackSummaryToolCfg(flags, name='InDetTrackSummaryTool', **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        from InDetConfig.ITkTrackingCommonConfig import ITkTrackSummaryToolCfg
        return ITkTrackSummaryToolCfg(flags, name, **kwargs)

    acc = ComponentAccumulator()
    do_holes = kwargs.get("doHolesInDet",True)

    if 'InDetSummaryHelperTool' not in kwargs:
        if do_holes:
            from InDetConfig.InDetTrackSummaryHelperToolConfig import InDetTrackSummaryHelperToolCfg
            InDetSummaryHelperTool = acc.popToolsAndMerge(InDetTrackSummaryHelperToolCfg(flags))
        else:
            from InDetConfig.InDetTrackSummaryHelperToolConfig import InDetSummaryHelperNoHoleSearchCfg
            InDetSummaryHelperTool = acc.popToolsAndMerge(InDetSummaryHelperNoHoleSearchCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelperTool)

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", do_holes)
    kwargs.setdefault("TRT_ElectronPidTool", None) # we don't want to use those tools during pattern
    kwargs.setdefault("PixelToTPIDTool", None) # we don't want to use those tools during pattern

    acc.addPublicTool(CompFactory.Trk.TrackSummaryTool(name, **kwargs), primary=True)
    return acc

def InDetTrackSummaryToolAmbiCfg(flags, name='InDetTrackSummaryToolAmbi', **kwargs):
    acc = ComponentAccumulator()

    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import InDetTrackSummaryHelperToolCfg
        InDetSummaryHelperTool = acc.popToolsAndMerge(InDetTrackSummaryHelperToolCfg(flags,
                                                                                     name = "InDetAmbiguityProcessorSplitProbSummaryHelper" + flags.InDet.Tracking.ActivePass.extension,
                                                                                     ClusterSplitProbabilityName = "InDetAmbiguityProcessorSplitProb" + flags.InDet.Tracking.ActivePass.extension))
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelperTool)

    InDetTrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags, name, **kwargs))
    acc.addPublicTool(InDetTrackSummaryTool, primary=True)
    return acc

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

def InDetRecTestBLayerToolCfg(flags, name='InDetRecTestBLayerTool', **kwargs):
    acc = ComponentAccumulator()

    if not flags.Detector.EnablePixel:
        return None

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    if 'PixelSummaryTool' not in kwargs:
        from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
        PixelConditionsSummaryTool = acc.popToolsAndMerge(PixelConditionsSummaryCfg(flags))
        kwargs.setdefault( "PixelSummaryTool", PixelConditionsSummaryTool)

    if "PixelDetElStatus" not in kwargs :
        from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDetectorElementStatusAlgCfg
        acc.merge( PixelDetectorElementStatusAlgCfg(flags) )
        kwargs.setdefault("PixelDetElStatus", "PixelDetectorElementStatus")

    InDetTestBLayerTool = CompFactory.InDet.InDetTestBLayerTool(name, **kwargs)
    acc.setPrivateTools(InDetTestBLayerTool)
    return acc

def InDetTrackSummaryToolSharedHitsCfg(flags, name='InDetTrackSummaryToolSharedHits',**kwargs):
    acc = ComponentAccumulator()
    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import InDetSummaryHelperSharedHitsCfg
        InDetSummaryHelperSharedHits = acc.popToolsAndMerge(InDetSummaryHelperSharedHitsCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelperSharedHits)

    if 'TRT_ElectronPidTool' not in kwargs:
        if not flags.Detector.EnableTRT or flags.InDet.Tracking.doHighPileup:
            kwargs.setdefault("TRT_ElectronPidTool", None)
        else:
            from InDetConfig.TRT_ElectronPidToolsConfig import TRT_ElectronPidToolCfg
            kwargs.setdefault("TRT_ElectronPidTool", acc.popToolsAndMerge(TRT_ElectronPidToolCfg(flags, name="InDetTRT_ElectronPidTool")))

    if 'PixelToTPIDTool' not in kwargs:
        InDetPixelToTPIDTool = acc.popToolsAndMerge(InDetPixelToTPIDToolCfg(flags))
        kwargs.setdefault( "PixelToTPIDTool", InDetPixelToTPIDTool)

    kwargs.setdefault( "doSharedHits", flags.InDet.Tracking.doSharedHits)

    InDetTrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags, name, **kwargs))
    acc.addPublicTool(InDetTrackSummaryTool, primary=True)
    return acc

def InDetMultipleScatteringUpdatorCfg(name = "InDetMultipleScatteringUpdator", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault( "UseTrkUtils", False)
    MultipleScatteringUpdator = CompFactory.Trk.MultipleScatteringUpdator(name, **kwargs)
    
    acc.setPrivateTools(MultipleScatteringUpdator)
    return acc

def DistributedKalmanFilterCfg(flags, name="DistributedKalmanFilter", **kwargs) :
    acc = ComponentAccumulator()

    if 'ExtrapolatorTool' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault('ExtrapolatorTool', acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    if 'ROTcreator' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetRotCreatorCfg
        InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags))
        kwargs.setdefault('ROTcreator', InDetRotCreator)

    DistributedKalmanFilter = CompFactory.Trk.DistributedKalmanFilter(name = name, **kwargs)
    acc.setPrivateTools(DistributedKalmanFilter)
    return acc

def InDetGlobalChi2FitterCfg(flags, name='InDetGlobalChi2Fitter', **kwargs) :
    acc = ComponentAccumulator()

    if 'RotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetRotCreatorCfg
        InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags))
        kwargs.setdefault('RotCreatorTool', InDetRotCreator)
    if 'BroadRotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetBroadRotCreatorCfg
        InDetBroadRotCreator = acc.popToolsAndMerge(InDetBroadRotCreatorCfg(flags))
        kwargs.setdefault('BroadRotCreatorTool', InDetBroadRotCreator)

    # PHF cut during fit iterations to save CPU time
    kwargs.setdefault('MinPHFCut', flags.InDet.Tracking.ActivePass.minTRTPrecFrac)

    if flags.InDet.Tracking.doDBMstandalone:
        kwargs.setdefault('StraightLine', True)
        kwargs.setdefault('OutlierCut', 5)
        kwargs.setdefault('RecalibrateTRT', False)
        kwargs.setdefault('TRTExtensionCuts', False)
        kwargs.setdefault('TrackChi2PerNDFCut', 20)

    if (flags.InDet.Tracking.useBroadPixClusterErrors or flags.InDet.Tracking.useBroadSCTClusterErrors) and not flags.InDet.Tracking.doDBMstandalone:
        kwargs.setdefault('RecalibrateSilicon', False)

    if flags.InDet.Tracking.doRobustReco:
        kwargs.setdefault('OutlierCut', 10.0)
        kwargs.setdefault('TrackChi2PerNDFCut', 20)

    if flags.InDet.Tracking.doRobustReco or flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault('MaxOutliers', 99)

    if flags.Beam.Type is BeamType.Cosmics or flags.InDet.Tracking.doBeamGas:
        kwargs.setdefault('Acceleration', False)

    if flags.InDet.Tracking.materialInteractions and not flags.BField.solenoidOn:
        kwargs.setdefault('Momentum', 1000.*Units.MeV)

    InDetGlobalChi2FitterBase = acc.popToolsAndMerge(InDetGlobalChi2FitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetGlobalChi2FitterBase)
    return acc

def GaussianSumFitterCfg(flags, name="GaussianSumFitter", **kwargs):
    acc = ComponentAccumulator()

    if "ToolForROTCreation" not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetRotCreatorCfg
        InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags))
        kwargs.setdefault("ToolForROTCreation", InDetRotCreator)

    kwargs.setdefault("MakePerigee", True)
    kwargs.setdefault("RefitOnMeasurementBase", True)
    kwargs.setdefault("DoHitSorting", True)

    from egammaTrackTools.GSFTrackFitterConfig import EMGSFTrackFitterCfg

    GaussianSumFitter = acc.popToolsAndMerge(
        EMGSFTrackFitterCfg(flags, name=name, **kwargs)
    )

    acc.setPrivateTools(GaussianSumFitter)
    return acc

def InDetTrackFitterCfg(flags, name='InDetTrackFitter', **kwargs) :
    return {
            'DistributedKalmanFilter' : DistributedKalmanFilterCfg,
            'GlobalChi2Fitter'        : InDetGlobalChi2FitterCfg,
            'GaussianSumFilter'       : GaussianSumFitterCfg
    }[flags.InDet.Tracking.trackFitterType](flags, name, **kwargs)

def InDetTrackFitterAmbiCfg(flags, name='InDetTrackFitterAmbi', **kwargs) :
    acc = ComponentAccumulator()

    InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags, name, **kwargs))
    ClusterSplitProbabilityName = "InDetAmbiguityProcessorSplitProb" + flags.InDet.Tracking.ActivePass.extension

    if flags.InDet.Tracking.trackFitterType in ['KalmanFitter', 'KalmanDNAFitter', 'ReferenceKalmanFitter', 'DistributedKalmanFilter']:
        InDetTrackFitter.RecalibratorHandle.BroadPixelClusterOnTrackTool.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    elif flags.InDet.Tracking.trackFitterType=='GlobalChi2Fitter':
        InDetTrackFitter.RotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName
        InDetTrackFitter.BroadRotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName
        # Name change for tools can be cleaned ultimately when config is validated
        InDetTrackFitter.RotCreatorTool.name = "InDetRotCreatorAmbi" + flags.InDet.Tracking.ActivePass.extension
        InDetTrackFitter.BroadRotCreatorTool.name = "InDetBroadInDetRotCreatorAmbi" + flags.InDet.Tracking.ActivePass.extension
        InDetTrackFitter.RotCreatorTool.ToolPixelCluster.name = "InDetPixelClusterOnTrackToolAmbi" + flags.InDet.Tracking.ActivePass.extension
        InDetTrackFitter.BroadRotCreatorTool.ToolPixelCluster.name = "InDetBroadPixelClusterOnTrackToolAmbi" + flags.InDet.Tracking.ActivePass.extension

    elif flags.InDet.Tracking.trackFitterType=='GaussianSumFilter':
        InDetTrackFitter.ToolForROTCreation.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    acc.setPrivateTools(InDetTrackFitter)
    return acc

def InDetGlobalChi2FitterBaseCfg(flags, name='GlobalChi2FitterBase', **kwargs):
    acc = ComponentAccumulator()

    if 'TrackingGeometryReadKey' not in kwargs:
        from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
            TrackingGeometryCondAlgCfg)
        cond_alg = TrackingGeometryCondAlgCfg(flags)
        geom_cond_key = cond_alg.getPrimary().TrackingGeometryWriteKey
        acc.merge(cond_alg)
        kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    from TrkConfig.AtlasExtrapolatorToolsConfig import (
        AtlasNavigatorCfg, AtlasEnergyLossUpdatorCfg, InDetPropagatorCfg, InDetMaterialEffectsUpdatorCfg)

    InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))
    InDetNavigator = acc.popToolsAndMerge(AtlasNavigatorCfg(flags, name="InDetNavigator"))
    ELossUpdator = acc.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags))
    InDetPropagator = acc.popToolsAndMerge(InDetPropagatorCfg(flags))
    from TrkConfig.TrkMeasurementUpdatorConfig import InDetUpdatorCfg
    InDetUpdator = acc.popToolsAndMerge(InDetUpdatorCfg(flags))

    InDetMultipleScatteringUpdator = acc.popToolsAndMerge(
        InDetMultipleScatteringUpdatorCfg())

    InDetMaterialEffectsUpdator = acc.popToolsAndMerge(
        InDetMaterialEffectsUpdatorCfg(flags))

    kwargs.setdefault("ExtrapolationTool", InDetExtrapolator)
    kwargs.setdefault("NavigatorTool", InDetNavigator)
    kwargs.setdefault("PropagatorTool", InDetPropagator)
    kwargs.setdefault("MultipleScatteringTool", InDetMultipleScatteringUpdator)
    kwargs.setdefault("EnergyLossTool", ELossUpdator)
    kwargs.setdefault("MeasurementUpdateTool", InDetUpdator)
    kwargs.setdefault("MaterialUpdateTool", InDetMaterialEffectsUpdator)
    kwargs.setdefault("StraightLine", not flags.BField.solenoidOn)
    kwargs.setdefault("OutlierCut", 4)
    kwargs.setdefault("SignedDriftRadius", True)
    kwargs.setdefault("ReintegrateOutliers", True)
    kwargs.setdefault("RecalibrateSilicon", True)
    kwargs.setdefault("RecalibrateTRT", True)
    kwargs.setdefault("TRTTubeHitCut", 1.75)
    kwargs.setdefault("MaxIterations", 40)
    kwargs.setdefault("Acceleration", True)
    kwargs.setdefault("RecalculateDerivatives", flags.InDet.Tracking.doMinBias or flags.Beam.Type is BeamType.Cosmics or flags.InDet.Tracking.doBeamGas)
    kwargs.setdefault("TRTExtensionCuts", True)
    kwargs.setdefault("TrackChi2PerNDFCut", 7)

    GlobalChi2Fitter = CompFactory.Trk.GlobalChi2Fitter(name=name, **kwargs)
    acc.setPrivateTools(GlobalChi2Fitter)
    return acc

def InDetGlobalChi2FitterTRTCfg(flags, name='InDetGlobalChi2FitterTRT', **kwargs) :
    acc = ComponentAccumulator()
    '''
    Global Chi2 Fitter for TRT segments with different settings
    '''

    if 'RotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetRefitRotCreatorCfg
        InDetRefitRotCreator = acc.popToolsAndMerge(InDetRefitRotCreatorCfg(flags))
        kwargs.setdefault("RotCreatorTool", InDetRefitRotCreator)

    kwargs.setdefault("MaterialUpdateTool", '')
    kwargs.setdefault("SignedDriftRadius", True)
    kwargs.setdefault("RecalibrateSilicon", False)
    kwargs.setdefault("RecalibrateTRT", False)
    kwargs.setdefault("TRTTubeHitCut", 2.5)
    kwargs.setdefault("MaxIterations", 10)
    kwargs.setdefault("Acceleration", False)
    kwargs.setdefault("RecalculateDerivatives", False)
    kwargs.setdefault("TRTExtensionCuts", True)
    kwargs.setdefault("TrackChi2PerNDFCut", 999999)
    kwargs.setdefault("Momentum", 1000.*Units.MeV   if flags.InDet.Tracking.materialInteractions and not flags.BField.solenoidOn else  0)
    kwargs.setdefault("OutlierCut", 5)
    kwargs.setdefault("MaxOutliers", 99 if flags.InDet.Tracking.doRobustReco or flags.Beam.Type is BeamType.Cosmics else 10)
    kwargs.setdefault("ReintegrateOutliers", False)

    InDetGlobalChi2FitterBase = acc.popToolsAndMerge(InDetGlobalChi2FitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetGlobalChi2FitterBase)
    return acc

def InDetTrackFitterTRTCfg(flags, name='InDetTrackFitterTRT', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.trackFitterType != 'GlobalChi2Fitter' :
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetTrackFitter)
    else :
        InDetGlobalChi2FitterTRT = acc.popToolsAndMerge(InDetGlobalChi2FitterTRTCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetGlobalChi2FitterTRT)

    return acc

def InDetGlobalChi2FitterLowPtCfg(flags, name='InDetGlobalChi2FitterLowPt', **kwargs) :
    acc = ComponentAccumulator()

    if 'RotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetRotCreatorCfg
        InDetRotCreator = acc.popToolsAndMerge(InDetRotCreatorCfg(flags))
        kwargs.setdefault('RotCreatorTool', InDetRotCreator)
    if 'BroadRotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import InDetBroadRotCreatorCfg
        InDetBroadRotCreator = acc.popToolsAndMerge(InDetBroadRotCreatorCfg(flags))
        kwargs.setdefault('BroadRotCreatorTool', InDetBroadRotCreator)

    kwargs.setdefault('OutlierCut', 5.0)
    kwargs.setdefault('Acceleration', False)
    kwargs.setdefault('RecalculateDerivatives', True)
    kwargs.setdefault('TrackChi2PerNDFCut', 10)

    InDetGlobalChi2FitterBase = acc.popToolsAndMerge(InDetGlobalChi2FitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetGlobalChi2FitterBase)
    return acc

def InDetTrackFitterLowPtCfg(flags, name='InDetTrackFitter', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.trackFitterType != 'GlobalChi2Fitter' :
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetTrackFitter)
    else :
        InDetGlobalChi2FitterLowPt = acc.popToolsAndMerge(InDetGlobalChi2FitterLowPtCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetGlobalChi2FitterLowPt)
    return acc

def InDetTrackFitterLowPtAmbiCfg(flags, name='InDetTrackFitterAmbi', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.trackFitterType != 'GlobalChi2Fitter' :
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterAmbiCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetTrackFitter)
    else :
        InDetGlobalChi2FitterLowPt = acc.popToolsAndMerge(InDetGlobalChi2FitterLowPtCfg(flags, name, **kwargs))
        ClusterSplitProbabilityName = "InDetAmbiguityProcessorSplitProb" + flags.InDet.Tracking.ActivePass.extension
        InDetGlobalChi2FitterLowPt.RotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName
        InDetGlobalChi2FitterLowPt.BroadRotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName
        acc.setPrivateTools(InDetGlobalChi2FitterLowPt)
    return acc

def InDetGlobalChi2FitterBTCfg(flags, name='InDetGlobalChi2FitterBT', **kwargs):
    acc = ComponentAccumulator()
    '''
    Global Chi2 Fitter for backtracking
    '''
    kwargs.setdefault("MinPHFCut", 0.)

    InDetGlobalChi2Fitter = acc.popToolsAndMerge(InDetGlobalChi2FitterCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetGlobalChi2Fitter)
    return acc

def InDetTrackFitterBTCfg(flags, name='InDetTrackFitterBT', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.trackFitterType != 'GlobalChi2Fitter' :
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetTrackFitter)
    else :
        InDetGlobalChi2Fitter = acc.popToolsAndMerge(InDetGlobalChi2FitterBTCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetGlobalChi2Fitter)
    return acc

def InDetTrackSummaryToolTRTTracksCfg(flags, name='InDetTrackSummaryToolTRTTracks',**kwargs):
    kwargs.setdefault("doSharedHits", True)
    return InDetTrackSummaryToolSharedHitsCfg(flags, name = name, **kwargs)

#############################################################################################
#TRTSegmentFinder
#############################################################################################

def InDetTRT_DriftCircleOnTrackUniversalToolCosmicsCfg(name='TRT_DriftCircleOnTrackUniversalTool', **kwargs):
    kwargs.setdefault("ScaleHitUncertainty", 2.)
    return InDetTRT_DriftCircleOnTrackUniversalToolCfg(name=name, **kwargs)

def InDetTRT_ExtensionToolCosmicsCfg(flags, name='InDetTRT_ExtensionToolCosmics', **kwargs):
    acc = ComponentAccumulator()

    if 'Propagator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorToolsConfig import InDetPropagatorCfg
        InDetPropagator = acc.getPrimaryAndMerge(InDetPropagatorCfg(flags))
        kwargs.setdefault("Propagator", InDetPropagator)

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    if 'RIOonTrackToolYesDr' not in kwargs:
        InDetTRT_DriftCircleOnTrackUniversalToolCosmics = acc.popToolsAndMerge(InDetTRT_DriftCircleOnTrackUniversalToolCosmicsCfg())
        kwargs.setdefault("RIOonTrackToolYesDr", InDetTRT_DriftCircleOnTrackUniversalToolCosmics)

    if 'RIOonTrackToolNoDr' not in kwargs:
        InDetBroadTRT_DriftCircleOnTrackTool = acc.popToolsAndMerge(InDetBroadTRT_DriftCircleOnTrackToolCfg())
        kwargs.setdefault("RIOonTrackToolNoDr", InDetBroadTRT_DriftCircleOnTrackTool)

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

def InDetTrackSummaryToolNoHoleSearchCfg(flags, name='InDetTrackSummaryToolNoHoleSearch',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault('doHolesInDet', False)
    InDetTrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(InDetTrackSummaryTool)
    return acc

def ROIInfoVecAlgCfg(flags, name='InDetROIInfoVecCondAlg', **kwargs) :
    acc = ComponentAccumulator()
    from InDetConfig.InDetCaloClusterROISelectorConfig import CaloClusterROI_SelectorCfg
    acc.merge(CaloClusterROI_SelectorCfg(flags))
    kwargs.setdefault("InputEmClusterContainerName", "InDetCaloClusterROIs")
    kwargs.setdefault("WriteKey", kwargs.get("namePrefix","") +"ROIInfoVec"+ kwargs.get("nameSuffix","") )
    kwargs.setdefault("minPtEM", 5000.0) #in MeV
    acc.addEventAlgo(CompFactory.ROIInfoVecAlg(name = name,**kwargs), primary=True)
    return acc

def InDetAmbiScoringToolBaseCfg(flags, name='InDetAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    kwargs.setdefault("Extrapolator", acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

    InDetTrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags))

    if 'DriftCircleCutTool' not in kwargs:
        InDetTRTDriftCircleCutForPatternReco = acc.popToolsAndMerge(InDetTRTDriftCircleCutForPatternRecoCfg(flags))
        kwargs.setdefault("DriftCircleCutTool", InDetTRTDriftCircleCutForPatternReco )

    have_calo_rois = flags.InDet.Tracking.doBremRecovery and flags.InDet.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois:
        alg = acc.getPrimaryAndMerge(ROIInfoVecAlgCfg(flags))
        kwargs.setdefault("CaloROIInfoName", alg.WriteKey )
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

    InDetTrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags))

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

def InDetCosmicExtenScoringToolCfg(flags, name='InDetCosmicExtenScoringTool',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("nWeightedClustersMin", 0)
    kwargs.setdefault("minTRTHits", flags.InDet.Tracking.ActivePass.minTRTonTrk )
    acc.setPrivateTools(acc.popToolsAndMerge(InDetCosmicsScoringToolBaseCfg(flags, name = 'InDetCosmicExtenScoringTool', **kwargs)))
    return acc

def InDetCosmicScoringTool_TRTCfg(flags, name='InDetCosmicExtenScoringTool',**kwargs) :
    acc = ComponentAccumulator()
    InDetTrackSummaryToolNoHoleSearch = acc.popToolsAndMerge(InDetTrackSummaryToolNoHoleSearchCfg(flags))

    kwargs.setdefault("minTRTHits", flags.InDet.Tracking.ActivePass.minSecondaryTRTonTrk)
    kwargs.setdefault("SummaryTool", InDetTrackSummaryToolNoHoleSearch)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetCosmicExtenScoringToolCfg(flags,
                                                                            name = 'InDetCosmicScoringTool_TRT', **kwargs)))
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
    if flags.InDet.Tracking.trackFitterType in ['KalmanFitter', 'KalmanDNAFitter', 'ReferenceKalmanFitter']:
        kwargs.setdefault("minTRTPrecisionFraction", 0.2)
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
        alg = acc.popToolsAndMerge(ROIInfoVecAlgCfg(flags))
        kwargs.setdefault("CaloROIInfoName", alg.WriteKey )

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))
    InDetTrackSummaryTool = acc.getPrimaryAndMerge(InDetTrackSummaryToolCfg(flags))

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
