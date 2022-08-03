# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of common interface with various track fitters
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from InDetConfig.InDetConfigFlags import TrackFitterType

#########################
##### InDet configs #####
#########################

def InDetTrackFitterCfg(flags, name='InDetTrackFitter', **kwargs) :
    from TrkConfig.TrkDistributedKalmanFilterConfig import DistributedKalmanFilterCfg
    from TrkConfig.TrkGlobalChi2FitterConfig import InDetGlobalChi2FitterCfg
    from TrkConfig.TrkGaussianSumFilterConfig import GaussianSumFitterCfg
    return {
            TrackFitterType.DistributedKalmanFilter : DistributedKalmanFilterCfg,
            TrackFitterType.GlobalChi2Fitter        : InDetGlobalChi2FitterCfg,
            TrackFitterType.GaussianSumFilter       : GaussianSumFitterCfg
    }[flags.InDet.Tracking.trackFitterType](flags, name, **kwargs)

def InDetTrackFitterHoleSearchCfg(flags, name='InDetTrackFitterHoleSearch', **kwargs) :
    acc = ComponentAccumulator()

    if "BoundaryCheckTool" not in kwargs:
        from InDetConfig.InDetBoundaryCheckToolConfig import InDetBoundaryCheckToolCfg
        kwargs.setdefault("BoundaryCheckTool", acc.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags)))

    kwargs.setdefault("DoHoleSearch", True)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetTrackFitterCfg(flags, name, **kwargs)))
    return acc

def InDetTrackFitterAmbiCfg(flags, name='InDetTrackFitterAmbi', **kwargs) :
    acc = ComponentAccumulator()

    InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterHoleSearchCfg(flags, name, **kwargs))
    ClusterSplitProbabilityName = "InDetAmbiguityProcessorSplitProb" + flags.InDet.Tracking.ActivePass.extension

    if flags.InDet.Tracking.trackFitterType==TrackFitterType.DistributedKalmanFilter:
        InDetTrackFitter.RecalibratorHandle.BroadPixelClusterOnTrackTool.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    elif flags.InDet.Tracking.trackFitterType==TrackFitterType.GlobalChi2Fitter:
        InDetTrackFitter.ClusterSplitProbabilityName = ClusterSplitProbabilityName
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

def InDetTrackFitterTRTCfg(flags, name='InDetTrackFitterTRT', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.trackFitterType != TrackFitterType.GlobalChi2Fitter :
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetTrackFitter)
    else :
        from TrkConfig.TrkGlobalChi2FitterConfig import InDetGlobalChi2FitterTRTCfg
        InDetGlobalChi2FitterTRT = acc.popToolsAndMerge(InDetGlobalChi2FitterTRTCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetGlobalChi2FitterTRT)

    return acc

def InDetTrackFitterLowPtCfg(flags, name='InDetTrackFitter', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.trackFitterType != TrackFitterType.GlobalChi2Fitter :
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetTrackFitter)
    else :
        from TrkConfig.TrkGlobalChi2FitterConfig import InDetGlobalChi2FitterLowPtCfg
        InDetGlobalChi2FitterLowPt = acc.popToolsAndMerge(InDetGlobalChi2FitterLowPtCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetGlobalChi2FitterLowPt)
    return acc

def InDetTrackFitterLowPtHoleSearchCfg(flags, name='InDetTrackFitterHoleSearch', **kwargs) :
    acc = ComponentAccumulator()

    if "BoundaryCheckTool" not in kwargs:
        from InDetConfig.InDetBoundaryCheckToolConfig import InDetBoundaryCheckToolCfg
        kwargs.setdefault("BoundaryCheckTool", acc.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags)))

    kwargs.setdefault("DoHoleSearch", True)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetTrackFitterLowPtCfg(flags, name, **kwargs)))
    return acc

def InDetTrackFitterLowPtAmbiCfg(flags, name='InDetTrackFitterAmbi', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.trackFitterType != TrackFitterType.GlobalChi2Fitter :
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterAmbiCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetTrackFitter)
    else :
        from TrkConfig.TrkGlobalChi2FitterConfig import InDetGlobalChi2FitterLowPtCfg
        InDetGlobalChi2FitterLowPt = acc.popToolsAndMerge(InDetGlobalChi2FitterLowPtCfg(flags, name, **kwargs))
        ClusterSplitProbabilityName = "InDetAmbiguityProcessorSplitProb" + flags.InDet.Tracking.ActivePass.extension
        InDetGlobalChi2FitterLowPt.RotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName
        InDetGlobalChi2FitterLowPt.BroadRotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName
        acc.setPrivateTools(InDetGlobalChi2FitterLowPt)
    return acc

def InDetTrackFitterBTCfg(flags, name='InDetTrackFitterBT', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.trackFitterType != TrackFitterType.GlobalChi2Fitter :
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetTrackFitter)
    else :
        from TrkConfig.TrkGlobalChi2FitterConfig import InDetGlobalChi2FitterBTCfg
        InDetGlobalChi2Fitter = acc.popToolsAndMerge(InDetGlobalChi2FitterBTCfg(flags, name, **kwargs))
        acc.setPrivateTools(InDetGlobalChi2Fitter)
    return acc


#########################
#####  ITk configs  #####
#########################

def ITkTrackFitterCfg(flags, name='ITkTrackFitter', **kwargs) :
    from TrkConfig.TrkDistributedKalmanFilterConfig import ITkDistributedKalmanFilterCfg
    from TrkConfig.TrkGlobalChi2FitterConfig import ITkGlobalChi2FitterCfg
    from TrkConfig.TrkGaussianSumFilterConfig import ITkGaussianSumFitterCfg
    return {
            TrackFitterType.DistributedKalmanFilter : ITkDistributedKalmanFilterCfg,
            TrackFitterType.GlobalChi2Fitter        : ITkGlobalChi2FitterCfg,
            TrackFitterType.GaussianSumFilter       : ITkGaussianSumFitterCfg
    }[flags.ITk.Tracking.trackFitterType](flags, name, **kwargs)

def ITkTrackFitterAmbiCfg(flags, name='ITkTrackFitterAmbi', **kwargs) :
    acc = ComponentAccumulator()

    if "BoundaryCheckTool" not in kwargs:
        from InDetConfig.InDetBoundaryCheckToolConfig import ITkBoundaryCheckToolCfg
        kwargs.setdefault("BoundaryCheckTool", acc.popToolsAndMerge(ITkBoundaryCheckToolCfg(flags)))

    kwargs.setdefault("DoHoleSearch", True)

    ITkTrackFitter = acc.popToolsAndMerge(ITkTrackFitterCfg(flags, name, **kwargs))
    ClusterSplitProbabilityName = "ITkAmbiguityProcessorSplitProb" + flags.ITk.Tracking.ActivePass.extension

    if flags.ITk.Tracking.trackFitterType==TrackFitterType.DistributedKalmanFilter:
        ITkTrackFitter.RecalibratorHandle.BroadPixelClusterOnTrackTool.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    elif flags.ITk.Tracking.trackFitterType==TrackFitterType.GlobalChi2Fitter:
        ITkTrackFitter.ClusterSplitProbabilityName = ClusterSplitProbabilityName
        ITkTrackFitter.RotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName
        ITkTrackFitter.BroadRotCreatorTool.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    elif flags.ITk.Tracking.trackFitterType==TrackFitterType.GaussianSumFilter:
        ITkTrackFitter.ToolForROTCreation.ToolPixelCluster.ClusterSplitProbabilityName = ClusterSplitProbabilityName

    acc.setPrivateTools(ITkTrackFitter)
    return acc
