# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetTestPixelLayer package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

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
