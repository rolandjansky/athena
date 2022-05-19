# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
import AthenaCommon.SystemOfUnits as Units

def ITkRecTestBLayerToolCfg(flags, name='ITkRecTestBLayerTool', **kwargs):
    acc = ComponentAccumulator()

    if not flags.Detector.EnableITkPixel:
        return None

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.getPrimaryAndMerge(AtlasExtrapolatorCfg(flags)))

    if 'PixelSummaryTool' not in kwargs:
        from PixelConditionsTools.ITkPixelConditionsSummaryConfig import ITkPixelConditionsSummaryCfg
        ITkPixelConditionsSummaryTool = acc.popToolsAndMerge(ITkPixelConditionsSummaryCfg(flags))
        kwargs.setdefault( "PixelSummaryTool", ITkPixelConditionsSummaryTool)

    ITkTestBLayerTool = CompFactory.InDet.InDetTestBLayerTool(name, **kwargs)
    acc.setPrivateTools(ITkTestBLayerTool)
    return acc

def ITkMultipleScatteringUpdatorCfg(flags, name = "ITkMultipleScatteringUpdator", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault( "UseTrkUtils", False)
    MultipleScatteringUpdator = CompFactory.Trk.MultipleScatteringUpdator(name, **kwargs)
    acc.setPrivateTools(MultipleScatteringUpdator)
    return acc

def ITkDistributedKalmanFilterCfg(flags, name="ITkDistributedKalmanFilter", **kwargs) :
    acc = ComponentAccumulator()

    if 'ExtrapolatorTool' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs.setdefault('ExtrapolatorTool', acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    if 'ROTcreator' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import ITkRotCreatorCfg
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags))
        kwargs.setdefault('ROTcreator', ITkRotCreator)

    DistributedKalmanFilter = CompFactory.Trk.DistributedKalmanFilter(name = name, **kwargs)
    acc.setPrivateTools(DistributedKalmanFilter)
    return acc

def ITkGlobalChi2FitterCfg(flags, name='ITkGlobalChi2Fitter', **kwargs) :
    acc = ComponentAccumulator()

    if 'RotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import ITkRotCreatorCfg
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags))
        kwargs.setdefault('RotCreatorTool', ITkRotCreator)

    if 'BroadRotCreatorTool' not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import ITkBroadRotCreatorCfg
        ITkBroadRotCreator = acc.popToolsAndMerge(ITkBroadRotCreatorCfg(flags))
        kwargs.setdefault('BroadRotCreatorTool', ITkBroadRotCreator)

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault('MaxOutliers', 99)
        kwargs.setdefault('Acceleration', False)

    if flags.ITk.Tracking.materialInteractions and not flags.BField.solenoidOn:
        kwargs.setdefault('Momentum', 1000.*Units.MeV)

    ITkGlobalChi2Fitter = acc.popToolsAndMerge(ITkGlobalChi2FitterBaseCfg(flags, name=name, **kwargs))
    acc.setPrivateTools(ITkGlobalChi2Fitter)
    return acc

def ITkGaussianSumFitterCfg(flags, name="ITkGaussianSumFitter", **kwargs):
    acc = ComponentAccumulator()

    if "ToolForROTCreation" not in kwargs:
        from TrkConfig.TrkRIO_OnTrackCreatorConfig import ITkRotCreatorCfg
        ITkRotCreator = acc.popToolsAndMerge(ITkRotCreatorCfg(flags))
        kwargs.setdefault("ToolForROTCreation", ITkRotCreator )

    kwargs.setdefault("MakePerigee", True)
    kwargs.setdefault("RefitOnMeasurementBase", True)
    kwargs.setdefault("DoHitSorting", True)

    from egammaTrackTools.GSFTrackFitterConfig import EMGSFTrackFitterCfg

    GaussianSumFitter = acc.popToolsAndMerge(
        EMGSFTrackFitterCfg(flags, name=name, **kwargs)
    )

    acc.setPrivateTools(GaussianSumFitter)
    return acc

def ITkTrackFitterCfg(flags, name='ITkTrackFitter', **kwargs) :
    return {
            'DistributedKalmanFilter' : ITkDistributedKalmanFilterCfg,
            'GlobalChi2Fitter'        : ITkGlobalChi2FitterCfg,
            'GaussianSumFilter'       : ITkGaussianSumFitterCfg
    }[flags.ITk.Tracking.trackFitterType](flags, name, **kwargs)

def ITkTrackFitterAmbiCfg(flags, name='ITkTrackFitterAmbi', **kwargs) :
    acc = ComponentAccumulator()

    ITkTrackFitter = acc.popToolsAndMerge(ITkTrackFitterCfg(flags, name, **kwargs))
    ClusterSplitProbabilityName = "ITkAmbiguityProcessorSplitProb" + flags.ITk.Tracking.ActivePass.extension

    if flags.ITk.Tracking.trackFitterType in ['DistributedKalmanFilter']:
        ITkTrackFitter.RecalibratorHandle.BroadPixelClusterOnTrackTool.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    elif flags.ITk.Tracking.trackFitterType=='GlobalChi2Fitter':
        ITkTrackFitter.ClusterSplitProbabilityName = ClusterSplitProbabilityName
        ITkTrackFitter.RotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName
        ITkTrackFitter.BroadRotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    elif flags.InDet.Tracking.trackFitterType=='GaussianSumFilter':
        ITkTrackFitter.ToolForROTCreation.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    acc.setPrivateTools(ITkTrackFitter)
    return acc

def ITkGlobalChi2FitterBaseCfg(flags, name='ITkGlobalChi2FitterBase', **kwargs) :
    acc = ComponentAccumulator()

    if 'TrackingGeometryReadKey' not in kwargs:
        from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
                TrackingGeometryCondAlgCfg)
        geom_cond = TrackingGeometryCondAlgCfg(flags)
        geom_cond_key = geom_cond.getPrimary().TrackingGeometryWriteKey
        acc.merge(acc)
        kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasNavigatorCfg, AtlasEnergyLossUpdatorCfg, ITkMaterialEffectsUpdatorCfg

    Extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    Navigator = acc.popToolsAndMerge(AtlasNavigatorCfg(flags))
    ELossUpdator = acc.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags))
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import ITkPropagatorCfg
    ITkPropagator = acc.popToolsAndMerge(ITkPropagatorCfg(flags))
    from TrkConfig.TrkMeasurementUpdatorConfig import ITkUpdatorCfg
    ITkUpdator = acc.popToolsAndMerge(ITkUpdatorCfg(flags))
    ITkMultipleScatteringUpdator = acc.popToolsAndMerge(ITkMultipleScatteringUpdatorCfg(flags))
    ITkMaterialEffectsUpdator = acc.popToolsAndMerge(ITkMaterialEffectsUpdatorCfg(flags))

    kwargs.setdefault("ExtrapolationTool", Extrapolator)
    kwargs.setdefault("NavigatorTool", Navigator)
    kwargs.setdefault("PropagatorTool", ITkPropagator)
    kwargs.setdefault("MultipleScatteringTool", ITkMultipleScatteringUpdator)
    kwargs.setdefault("EnergyLossTool", ELossUpdator)
    kwargs.setdefault("MeasurementUpdateTool", ITkUpdator)
    kwargs.setdefault("MaterialUpdateTool", ITkMaterialEffectsUpdator)
    kwargs.setdefault("StraightLine", not flags.BField.solenoidOn)
    kwargs.setdefault("OutlierCut", 4)
    kwargs.setdefault("SignedDriftRadius", True)
    kwargs.setdefault("ReintegrateOutliers", True)
    kwargs.setdefault("RecalibrateSilicon", True)
    kwargs.setdefault("MaxIterations", 40)
    kwargs.setdefault("Acceleration", True)
    kwargs.setdefault("RecalculateDerivatives", flags.Beam.Type is BeamType.Cosmics)
    kwargs.setdefault("TrackChi2PerNDFCut", 7)

    GlobalChi2Fitter = CompFactory.Trk.GlobalChi2Fitter(name=name, **kwargs)
    acc.setPrivateTools(GlobalChi2Fitter)
    return acc

#############################################################################################
# BackTracking
#############################################################################################

def ITkAmbiScoringToolBaseCfg(flags, name='ITkAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    from TrkConfig.TrkTrackSummaryToolConfig import ITkTrackSummaryToolCfg
    ITkTrackSummaryTool = acc.popToolsAndMerge(ITkTrackSummaryToolCfg(flags))

    have_calo_rois = flags.ITk.Tracking.doBremRecovery and flags.ITk.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois:
        from InDetConfig.InDetCaloClusterROISelectorConfig import ITkCaloClusterROIPhiRZContainerMakerCfg
        acc.merge(ITkCaloClusterROIPhiRZContainerMakerCfg(flags))
        kwargs.setdefault("EMROIPhiRZContainer","ITkCaloClusterROIPhiRZ0GeV")
    kwargs.setdefault("SummaryTool", ITkTrackSummaryTool )
    kwargs.setdefault("DriftCircleCutTool", None )
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxEta", flags.ITk.Tracking.ActivePass.maxEta )
    kwargs.setdefault("usePixel", flags.ITk.Tracking.ActivePass.useITkPixel )
    kwargs.setdefault("useSCT", flags.ITk.Tracking.ActivePass.useITkStrip )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )
    kwargs.setdefault("useITkAmbigFcn", True )
    kwargs.setdefault("minTRTonTrk", 0 )
    kwargs.setdefault("minTRTPrecisionFraction", 0 )

    if 'InDetEtaDependentCutsSvc' not in kwargs:
        from InDetConfig.InDetEtaDependentCutsConfig import ITkEtaDependentCutsSvcCfg
        acc.merge(ITkEtaDependentCutsSvcCfg(flags))
        kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("ITkEtaDependentCutsSvc"+flags.ITk.Tracking.ActivePass.extension))

    the_name = name + flags.ITk.Tracking.ActivePass.extension
    acc.setPrivateTools(CompFactory.InDet.InDetAmbiScoringTool(name = the_name, **kwargs))
    return acc

def ITkCosmicsScoringToolBaseCfg(flags, name='ITkCosmicsScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.TrkTrackSummaryToolConfig import ITkTrackSummaryToolCfg
    ITkTrackSummaryTool = acc.popToolsAndMerge(ITkTrackSummaryToolCfg(flags))
    acc.addPublicTool(ITkTrackSummaryTool)

    kwargs.setdefault("nWeightedClustersMin", flags.ITk.Tracking.ActivePass.nWeightedClustersMin )
    kwargs.setdefault("minTRTHits", 0 )
    kwargs.setdefault("SummaryTool", ITkTrackSummaryTool )

    acc.setPrivateTools( CompFactory.InDet.InDetCosmicScoringTool(name, **kwargs ) )
    return acc

def ITkCosmicExtenScoringToolCfg(flags, name='ITkCosmicExtenScoringTool',**kwargs) :
    kwargs.setdefault("nWeightedClustersMin", 0)
    kwargs.setdefault("minTRTHits", 0 )
    return ITkCosmicsScoringToolBaseCfg(flags, name = 'ITkCosmicExtenScoringTool', **kwargs)

def ITkAmbiScoringToolCfg(flags, name='ITkAmbiScoringTool', **kwargs) :
    return ITkAmbiScoringToolBaseCfg(flags, name = name + flags.ITk.Tracking.ActivePass.extension, **kwargs)


#############################################################################################
#TrackingSiPatternConfig
#############################################################################################

def ITkPRDtoTrackMapToolCfg(flags, name='ITkPRDtoTrackMapTool',**kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.PRDtoTrackMapTool(name, **kwargs))
    return acc

def ITkCosmicsScoringToolCfg(flags, name='ITkCosmicsScoringTool', **kwargs) :
    return ITkCosmicsScoringToolBaseCfg(flags,
                                        name=name+flags.ITk.Tracking.ActivePass.extension)
