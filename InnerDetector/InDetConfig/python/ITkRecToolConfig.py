# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType


def ITkPrdAssociationToolCfg(flags, name='ITkPrdAssociationTool',**kwargs) :
  acc = ComponentAccumulator()
  '''
  Provide an instance for all clients in which the tool is only set in c++
  '''
  kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'ITkPixelClusterAmbiguitiesMap')
  kwargs.setdefault("addTRToutliers", False)

  ITkPRD_AssociationToolGangedPixels = CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(name, **kwargs)
  acc.setPrivateTools(ITkPRD_AssociationToolGangedPixels)
  return acc

def ITkPrdAssociationTool_setupCfg(flags, name='ITkPrdAssociationTool_setup',**kwargs) :
  '''
  Provide an instance for all clients which set the tool explicitely
  '''
  kwargs.setdefault("SetupCorrect", True)
  return ITkPrdAssociationToolCfg(flags, name, **kwargs)

def ITkTrackSummaryHelperToolCfg(flags, name='ITkSummaryHelper', **kwargs):
  result = ComponentAccumulator()

  if 'AssoTool' not in kwargs :
    assoTool = result.popToolsAndMerge(ITkPrdAssociationTool_setupCfg(flags))
    result.addPublicTool(assoTool)
    kwargs.setdefault("AssoTool", assoTool)

  if "HoleSearch" not in kwargs:
    ITkTrackHoleSearchTool = result.popToolsAndMerge(ITkTrackHoleSearchToolCfg(flags))
    result.addPublicTool(ITkTrackHoleSearchTool)
    kwargs.setdefault("HoleSearch", ITkTrackHoleSearchTool)

  kwargs.setdefault("TRTStrawSummarySvc", "")
  kwargs.setdefault("PixelToTPIDTool", None)
  kwargs.setdefault("TestBLayerTool", None)
  kwargs.setdefault("RunningTIDE_Ambi", True)
  kwargs.setdefault("DoSharedHits", False)
  kwargs.setdefault("usePixel", flags.Detector.EnableITkPixel)
  kwargs.setdefault("useSCT", flags.Detector.EnableITkStrip)
  kwargs.setdefault("useTRT", False)

  result.setPrivateTools(CompFactory.InDet.InDetTrackSummaryHelperTool(name, **kwargs))
  return result

def ITkBoundaryCheckToolCfg(flags, name='ITkBoundaryCheckTool', **kwargs):
  result = ComponentAccumulator()

  if 'SctSummaryTool' not in kwargs:
    if flags.Detector.EnableITkStrip:
      from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ITkStripConditionsSummaryToolCfg
      tmpAcc = ITkStripConditionsSummaryToolCfg(flags)
      kwargs.setdefault("SctSummaryTool", tmpAcc.popPrivateTools())
      result.merge(tmpAcc)
    else:
      kwargs.setdefault("SctSummaryTool", None)

  if 'PixelLayerTool' not in kwargs :
    kwargs.setdefault("PixelLayerTool", result.popToolsAndMerge(ITkTestPixelLayerToolCfg(flags)))

  kwargs.setdefault("UsePixel", flags.Detector.EnableITkPixel)
  kwargs.setdefault("UseSCT", flags.Detector.EnableITkStrip)
  kwargs.setdefault("CheckBadSCT", False)

  indet_boundary_check_tool = CompFactory.InDet.InDetBoundaryCheckTool(name, **kwargs)
  result.setPrivateTools(indet_boundary_check_tool)
  return result


def ITkTrackHoleSearchToolCfg(flags, name='ITkHoleSearchTool', **kwargs):
  result = ComponentAccumulator()
  if 'Extrapolator' not in kwargs:
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    result.addPublicTool(extrapolator)  # TODO: migrate to private?
    kwargs.setdefault("Extrapolator", extrapolator)

  if 'BoundaryCheckTool' not in kwargs:
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
