# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

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
    from InDetConfig.InDetBoundaryCheckToolConfig import InDetBoundaryCheckToolCfg
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
