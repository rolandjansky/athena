# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def ITkTrackHoleSearchToolCfg(flags, name='ITkHoleSearchTool', **kwargs):
  result = ComponentAccumulator()
  if 'Extrapolator' not in kwargs:
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    result.addPublicTool(extrapolator)  # TODO: migrate to private?
    kwargs.setdefault("Extrapolator", extrapolator)

  if 'BoundaryCheckTool' not in kwargs:
    from InDetConfig.InDetBoundaryCheckToolConfig import ITkBoundaryCheckToolCfg
    kwargs.setdefault('BoundaryCheckTool', result.popToolsAndMerge(ITkBoundaryCheckToolCfg(flags)))

  kwargs.setdefault("Cosmics", flags.Beam.Type is BeamType.Cosmics)
  kwargs.setdefault("CountDeadModulesAfterLastHit", True)

  result.setPrivateTools(CompFactory.InDet.InDetTrackHoleSearchTool(name, **kwargs))
  return result

def ITkTestPixelLayerToolCfg(flags, name = "ITkTestPixelLayerTool", **kwargs):
  result = ComponentAccumulator()
  if 'PixelSummaryTool' not in kwargs :
    from PixelConditionsTools.ITkPixelConditionsSummaryConfig import ITkPixelConditionsSummaryCfg
    kwargs.setdefault("PixelSummaryTool", result.popToolsAndMerge(ITkPixelConditionsSummaryCfg(flags)))

  if 'Extrapolator' not in kwargs :
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    result.addPublicTool(extrapolator)  # TODO: migrate to private?
    kwargs.setdefault("Extrapolator", extrapolator)

  kwargs.setdefault("CheckActiveAreas", flags.ITk.checkDeadPixelsOnTrack)
  kwargs.setdefault("CheckDeadRegions", flags.ITk.checkDeadPixelsOnTrack)
  kwargs.setdefault("CheckDisabledFEs", flags.ITk.checkDeadPixelsOnTrack)

  tool = CompFactory.InDet.InDetTestPixelLayerTool( name, **kwargs)
  result.setPrivateTools( tool )
  return result

def ITkPatternPropagatorCfg(flags, name='ITkPatternPropagator', **kwargs):
    result = ComponentAccumulator()
    tool = CompFactory.Trk.RungeKuttaPropagator(name, **kwargs)
    result.addPublicTool( tool, primary=True )
    return result

def ITkPatternUpdatorCfg(flags, name='ITkPatternUpdator', **kwargs):
    result = ComponentAccumulator()
    tool = CompFactory.Trk.KalmanUpdator_xk(name, **kwargs)
    result.setPrivateTools( tool )
    return result

def ITkUpdatorCfg(flags, name = 'ITkUpdator', **kwargs):
    result = ComponentAccumulator()

    tool = None
    if flags.ITk.Tracking.kalmanUpdator == "fast" :
        tool = CompFactory.Trk.KalmanUpdator_xk(name, **kwargs)
    elif flags.ITk.Tracking.kalmanUpdator == "weight" :
        tool = CompFactory.Trk.KalmanWeightUpdator(name, **kwargs)
    elif flags.ITk.Tracking.kalmanUpdator == "smatrix" :
        tool = CompFactory.Trk.KalmanUpdatorSMatrix(name, **kwargs)
    elif flags.ITk.Tracking.kalmanUpdator == "amg" :
        tool = CompFactory.Trk.KalmanUpdatorAmg(name, **kwargs)
    else :
        tool = CompFactory.Trk.KalmanUpdator(name, **kwargs)

    result.setPrivateTools( tool )
    return result
