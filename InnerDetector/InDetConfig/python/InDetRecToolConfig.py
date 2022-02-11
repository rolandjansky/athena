# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType


def InDetPrdAssociationToolCfg(flags, name='InDetPrdAssociationTool', **kwargs) :
  if flags.Detector.GeometryITk:
    name = name.replace("InDet", "ITk")
    from InDetConfig.ITkRecToolConfig import ITkPrdAssociationToolCfg
    return ITkPrdAssociationToolCfg(flags, name,**kwargs)

  acc = ComponentAccumulator()
  '''
  Provide an instance for all clients in which the tool is only set in c++
  '''
  kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'PixelClusterAmbiguitiesMap') # InDetKeys.GangedPixelMap
  kwargs.setdefault("addTRToutliers", True)

  InDetPRD_AssociationToolGangedPixels = CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(name, **kwargs)
  acc.setPrivateTools(InDetPRD_AssociationToolGangedPixels)
  return acc

def InDetPrdAssociationTool_setupCfg(flags, name='InDetPrdAssociationTool_setup', **kwargs) :
  '''
  Provide an instance for all clients which set the tool explicitely
  '''
  kwargs.setdefault("SetupCorrect", True)
  return InDetPrdAssociationToolCfg(flags, name, **kwargs)

def InDetTrackSummaryHelperToolCfg(flags, name='InDetSummaryHelper', **kwargs):
  if flags.Detector.GeometryITk:
    name = name.replace("InDet", "ITk")
    from InDetConfig.ITkRecToolConfig import ITkTrackSummaryHelperToolCfg
    return ITkTrackSummaryHelperToolCfg(flags, name, **kwargs)

  result = ComponentAccumulator()

  if 'AssoTool' not in kwargs :
    InDetPrdAssociationTool_setup = result.popToolsAndMerge(InDetPrdAssociationTool_setupCfg(flags))
    result.addPublicTool(InDetPrdAssociationTool_setup)
    kwargs.setdefault("AssoTool", InDetPrdAssociationTool_setup)

  if "HoleSearch" not in kwargs:
    InDetTrackHoleSearchTool = result.popToolsAndMerge(InDetTrackHoleSearchToolCfg(flags))
    result.addPublicTool(InDetTrackHoleSearchTool)
    kwargs.setdefault("HoleSearch", InDetTrackHoleSearchTool)

  if not flags.Detector.EnableTRT:
    kwargs.setdefault("TRTStrawSummarySvc", "")

  kwargs.setdefault("PixelToTPIDTool", None)
  kwargs.setdefault("TestBLayerTool", None)
  kwargs.setdefault("RunningTIDE_Ambi", flags.InDet.Tracking.doTIDE_Ambi)
  kwargs.setdefault("DoSharedHits", False)
  kwargs.setdefault("usePixel", flags.Detector.EnablePixel)
  kwargs.setdefault("useSCT", flags.Detector.EnableSCT)
  kwargs.setdefault("useTRT", flags.Detector.EnableTRT)

  result.setPrivateTools(CompFactory.InDet.InDetTrackSummaryHelperTool(name, **kwargs))
  return result

def InDetBoundaryCheckToolCfg(flags, name='InDetBoundarySearchTool', **kwargs):
  result = ComponentAccumulator()

  if 'SctSummaryTool' not in kwargs:
    if flags.Detector.EnableSCT:
      from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
      tmpAcc = SCT_ConditionsSummaryToolCfg(flags)
      kwargs.setdefault("SctSummaryTool", tmpAcc.popPrivateTools())
      result.merge(tmpAcc)
    else:
      kwargs.setdefault("SctSummaryTool", None)

  if 'PixelLayerTool' not in kwargs :
    kwargs.setdefault("PixelLayerTool", result.popToolsAndMerge(InDetTestPixelLayerToolCfg(flags)))

  kwargs.setdefault("UsePixel", flags.Detector.EnablePixel)
  kwargs.setdefault("UseSCT", flags.Detector.EnableSCT)
  kwargs.setdefault("CheckBadSCT", flags.InDet.checkDeadElementsOnTrack)

  result.setPrivateTools(CompFactory.InDet.InDetBoundaryCheckTool(name, **kwargs))
  return result


def InDetTrackHoleSearchToolCfg(flags, name = 'InDetHoleSearchTool', **kwargs):
  if flags.Detector.GeometryITk:
    name = name.replace("InDet", "ITk")
    from InDetConfig.ITkRecToolConfig import ITkTrackHoleSearchToolCfg
    return ITkTrackHoleSearchToolCfg(flags, name, **kwargs)

  result = ComponentAccumulator()
  if 'Extrapolator' not in kwargs:
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    kwargs.setdefault("Extrapolator", result.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

  if 'BoundaryCheckTool' not in kwargs:
    BoundaryCheckTool = result.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags))
    kwargs.setdefault('BoundaryCheckTool', BoundaryCheckTool)

  kwargs.setdefault("Cosmics", flags.Beam.Type is BeamType.Cosmics)
  kwargs.setdefault("CountDeadModulesAfterLastHit", True)

  indet_hole_search_tool = CompFactory.InDet.InDetTrackHoleSearchTool(name, **kwargs)
  result.setPrivateTools(indet_hole_search_tool)
  return result




def InDetTestPixelLayerToolCfg(flags, name = "InDetTestPixelLayerTool", **kwargs):
  result = ComponentAccumulator()
  if 'PixelSummaryTool' not in kwargs :
    from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    kwargs.setdefault("PixelSummaryTool", result.popToolsAndMerge(PixelConditionsSummaryCfg(flags)))

  if 'Extrapolator' not in kwargs :
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    InDetExtrapolator = result.popToolsAndMerge(InDetExtrapolatorCfg(flags))
    result.addPublicTool(InDetExtrapolator)  # TODO: migrate to private?
    kwargs.setdefault("Extrapolator", InDetExtrapolator)

  kwargs.setdefault("CheckActiveAreas", flags.InDet.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDeadRegions", flags.InDet.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDisabledFEs", flags.InDet.checkDeadElementsOnTrack)

  tool = CompFactory.InDet.InDetTestPixelLayerTool(name, **kwargs)
  result.setPrivateTools( tool )
  return result
