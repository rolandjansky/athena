# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
import AthenaCommon.SystemOfUnits as Units

def ITkPRDtoTrackMapToolGangedPixelsCfg(flags, name='ITkPRDtoTrackMapToolGangedPixels', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'ITkPixelClusterAmbiguitiesMap')
    kwargs.setdefault("addTRToutliers", False)
    acc.setPrivateTools(CompFactory.InDet.InDetPRDtoTrackMapToolGangedPixels( name, **kwargs))
    return acc

def ITkTrackPRD_AssociationCfg(flags, name='ITkTrackPRD_Association', **kwargs):
    acc = ComponentAccumulator()

    if kwargs.get('TracksName', None) is None :
        raise Exception('Not TracksName argument provided')

    if 'AssociationTool' not in kwargs:
        kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(ITkPRDtoTrackMapToolGangedPixelsCfg(flags)))

    kwargs.setdefault("AssociationMapName", 'ITkPRDtoTrackMap' + flags.ITk.Tracking.ActivePass.extension)
    acc.addEventAlgo(CompFactory.InDet.InDetTrackPRD_Association(name, **kwargs))
    return acc

def ITkTrackSummaryToolCfg(flags, name='ITkTrackSummaryTool', **kwargs):
    acc = ComponentAccumulator()
    do_holes=kwargs.get("doHolesInDet",True)

    if 'InDetSummaryHelperTool' not in kwargs:
        if do_holes:
            from  InDetConfig.InDetTrackSummaryHelperToolConfig import ITkTrackSummaryHelperToolCfg
            ITkSummaryHelperTool = acc.popToolsAndMerge(ITkTrackSummaryHelperToolCfg(flags))
        else:
            from  InDetConfig.InDetTrackSummaryHelperToolConfig import ITkSummaryHelperNoHoleSearchCfg
            ITkSummaryHelperTool = acc.popToolsAndMerge(ITkSummaryHelperNoHoleSearchCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperTool)

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", do_holes)
    kwargs.setdefault("TRT_ElectronPidTool", None) # we don't want to use those tools during pattern
    kwargs.setdefault("PixelToTPIDTool", None) # we don't want to use those tools during pattern
    acc.addPublicTool(CompFactory.Trk.TrackSummaryTool(name, **kwargs), primary=True)
    return acc

def ITkTrackSummaryToolAmbiCfg(flags, name='ITkTrackSummaryToolAmbi', **kwargs):
    acc = ComponentAccumulator()

    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import ITkTrackSummaryHelperToolCfg
        ITkSummaryHelperTool = acc.popToolsAndMerge(ITkTrackSummaryHelperToolCfg(flags,
                                                                                 ClusterSplitProbabilityName = "ITkAmbiguityProcessorSplitProb" + flags.ITk.Tracking.ActivePass.extension))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperTool)

    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags, name, **kwargs))
    acc.addPublicTool(ITkTrackSummaryTool, primary=True)
    return acc

def ITkTrackSummaryToolSharedHitsCfg(flags, name='ITkTrackSummaryToolSharedHits',**kwargs):
    acc = ComponentAccumulator()
    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import ITkSummaryHelperSharedHitsCfg
        ITkSummaryHelperSharedHits = acc.popToolsAndMerge(ITkSummaryHelperSharedHitsCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperSharedHits)

    kwargs.setdefault( "PixelToTPIDTool", None)
    kwargs.setdefault( "doSharedHits", flags.ITk.Tracking.doSharedHits)

    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags, name, **kwargs))
    acc.addPublicTool(ITkTrackSummaryTool, primary=True)
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

    if flags.ITk.Tracking.trackFitterType in ['KalmanFitter', 'KalmanDNAFitter', 'ReferenceKalmanFitter', 'DistributedKalmanFilter']:
        ITkTrackFitter.RecalibratorHandle.BroadPixelClusterOnTrackTool.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    elif flags.ITk.Tracking.trackFitterType=='GlobalChi2Fitter':
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
    from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasNavigatorCfg, AtlasEnergyLossUpdatorCfg, ITkPropagatorCfg, ITkMaterialEffectsUpdatorCfg
    from TrkConfig.TrkMeasurementUpdatorConfig import ITkUpdatorCfg

    Extrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    Navigator = acc.popToolsAndMerge(AtlasNavigatorCfg(flags))
    ELossUpdator = acc.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags))
    ITkPropagator = acc.popToolsAndMerge(ITkPropagatorCfg(flags))
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

def ITkFKFCfg(flags, name='ITkFKF', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("StateChi2PerNDFPreCut", 30.0)
    PublicFKF = CompFactory.Trk.ForwardKalmanFitter(name = name, **kwargs)
    acc.setPrivateTools(PublicFKF)
    return acc

def ITkBKSCfg(flags, name='ITkBKS', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("InitialCovarianceSeedFactor", 200.)
    PublicBKS = CompFactory.Trk.KalmanSmoother(name = name, **kwargs)
    acc.setPrivateTools(PublicBKS)
    return acc

def ITkKOLCfg(flags, name = 'ITkKOL', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackChi2PerNDFCut", 17.0)
    kwargs.setdefault("StateChi2PerNDFCut", 12.5)
    PublicKOL = CompFactory.Trk.KalmanOutlierLogic(name = name, **kwargs)
    acc.setPrivateTools(PublicKOL)
    return acc

#############################################################################################
# BackTracking
#############################################################################################

def ITkTrackSummaryToolNoHoleSearchCfg(flags, name='ITkTrackSummaryToolNoHoleSearch',**kwargs) :
    kwargs.setdefault('doHolesInDet', False)
    return ITkTrackSummaryToolCfg(flags, name=name, **kwargs)

def ITkROIInfoVecCondAlgCfg(flags, name='ITkROIInfoVecCondAlg', **kwargs) :
    acc = ComponentAccumulator()
    from InDetConfig.InDetCaloClusterROISelectorConfig import CaloClusterROI_SelectorCfg
    acc.merge(CaloClusterROI_SelectorCfg(flags))
    kwargs.setdefault("InputEmClusterContainerName", "ITkCaloClusterROIs")
    kwargs.setdefault("WriteKey", kwargs.get("namePrefix","") +"ROIInfoVec"+ kwargs.get("nameSuffix","") )
    kwargs.setdefault("minPtEM", 5000.0) #in MeV
    acc.addEventAlgo(CompFactory.ROIInfoVecAlg(name = name,**kwargs), primary=True)
    return acc

def ITkAmbiScoringToolBaseCfg(flags, name='ITkAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags))

    have_calo_rois = flags.ITk.Tracking.doBremRecovery and flags.ITk.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois:
        alg = acc.getPrimaryAndMerge(ITkROIInfoVecCondAlgCfg(flags))
        kwargs.setdefault("CaloROIInfoName", alg.WriteKey )
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

    ITkTrackSummaryTool = acc.getPrimaryAndMerge(ITkTrackSummaryToolCfg(flags))

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
