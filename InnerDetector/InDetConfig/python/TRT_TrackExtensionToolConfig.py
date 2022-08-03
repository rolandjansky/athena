# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TRT_TrackExtensionTool_xk packages
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def TRT_TrackExtensionToolCosmicsCfg(flags, name='TRT_TrackExtensionToolCosmics', **kwargs):
    acc = ComponentAccumulator()

    if 'Propagator' not in kwargs:
        from TrkConfig.TrkExRungeKuttaPropagatorConfig import InDetPropagatorCfg
        InDetPropagator = acc.popToolsAndMerge(InDetPropagatorCfg(flags))
        acc.addPublicTool(InDetPropagator)
        kwargs.setdefault("Propagator", InDetPropagator)

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        InDetExtrapolator = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
        acc.addPublicTool(InDetExtrapolator)
        kwargs.setdefault("Extrapolator", InDetExtrapolator)

    if 'RIOonTrackToolYesDr' not in kwargs:
        from InDetConfig.TRT_DriftCircleOnTrackToolConfig import TRT_DriftCircleOnTrackUniversalToolCosmicsCfg
        kwargs.setdefault("RIOonTrackToolYesDr", acc.popToolsAndMerge(TRT_DriftCircleOnTrackUniversalToolCosmicsCfg(flags)))

    if 'RIOonTrackToolNoDr' not in kwargs:
        from InDetConfig.TRT_DriftCircleOnTrackToolConfig import TRT_DriftCircleOnTrackNoDriftTimeToolCfg
        kwargs.setdefault("RIOonTrackToolNoDr", acc.popToolsAndMerge(TRT_DriftCircleOnTrackNoDriftTimeToolCfg(flags)))

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCircles')
    kwargs.setdefault("SearchNeighbour", False)   # needs debugging!!!
    kwargs.setdefault("RoadWidth", 10.0)
    acc.setPrivateTools(CompFactory.InDet.TRT_TrackExtensionToolCosmics(name, **kwargs))
    return acc

def TRT_TrackExtensionToolPhaseCfg(flags, name='TRT_TrackExtensionToolPhase', **kwargs) :
    acc = ComponentAccumulator()

    if 'RIOonTrackToolYesDr' not in kwargs:
        from InDetConfig.TRT_DriftCircleOnTrackToolConfig import TRT_DriftCircleOnTrackUniversalToolCfg
        kwargs.setdefault("RIOonTrackToolYesDr", acc.popToolsAndMerge(TRT_DriftCircleOnTrackUniversalToolCfg(flags)))

    kwargs.setdefault("TRT_ClustersContainer", 'TRT_DriftCirclesUncalibrated')
    kwargs.setdefault("RoadWidth", 20.0)
    acc.setPrivateTools(acc.popToolsAndMerge(TRT_TrackExtensionToolCosmicsCfg(flags, name, **kwargs)))
    return acc

def TRT_TrackExtensionTool_xkCfg(flags, name='TRT_TrackExtensionTool_xk', **kwargs):
    from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
    acc = AtlasFieldCacheCondAlgCfg(flags)

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
        from InDetConfig.InDetTrackSelectorToolConfig import InDetTRTDriftCircleCutToolCfg
        kwargs.setdefault("DriftCircleCutTool", acc.popToolsAndMerge(InDetTRTDriftCircleCutToolCfg(flags)))

    if 'RIOonTrackToolYesDr' not in kwargs:
        from InDetConfig.TRT_DriftCircleOnTrackToolConfig import TRT_DriftCircleOnTrackToolCfg
        kwargs.setdefault("RIOonTrackToolYesDr", acc.popToolsAndMerge(TRT_DriftCircleOnTrackToolCfg(flags)))

    if 'RIOonTrackToolNoDr' not in kwargs:
        from InDetConfig.TRT_DriftCircleOnTrackToolConfig import TRT_DriftCircleOnTrackNoDriftTimeToolCfg
        kwargs.setdefault("RIOonTrackToolNoDr", acc.popToolsAndMerge(TRT_DriftCircleOnTrackNoDriftTimeToolCfg(flags)))

    if 'RoadTool' not in kwargs:
        from InDetConfig.TRT_DetElementsRoadToolConfig import TRT_DetElementsRoadMaker_xkCfg
        kwargs.setdefault("RoadTool", acc.popToolsAndMerge(TRT_DetElementsRoadMaker_xkCfg(flags)))

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

def TRT_TrackExtensionToolCfg(flags, name='TRT_TrackExtensionTool', **kwargs):
    if flags.Beam.Type is BeamType.Cosmics:
        return TRT_TrackExtensionToolCosmicsCfg(flags, name, **kwargs)
    else:
        return TRT_TrackExtensionTool_xkCfg(flags, name, **kwargs)
