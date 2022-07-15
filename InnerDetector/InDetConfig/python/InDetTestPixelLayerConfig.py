# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetTestPixelLayer package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetTestPixelLayerToolCfg(flags, name = "InDetTestPixelLayerTool", **kwargs):
  if flags.Detector.GeometryITk:
    name = name.replace("InDet", "ITk")
    return ITkTestPixelLayerToolCfg(flags, name, **kwargs)

  result = ComponentAccumulator()
  if 'PixelSummaryTool' not in kwargs :
    from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    kwargs.setdefault("PixelSummaryTool", result.popToolsAndMerge(PixelConditionsSummaryCfg(flags)))

  if "PixelDetElStatus" not in kwargs and not flags.Common.isOnline:
    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDetectorElementStatusAlgCfg
    result.merge( PixelDetectorElementStatusAlgCfg(flags) )
    kwargs.setdefault("PixelDetElStatus", "PixelDetectorElementStatus")

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

def InDetTestPixelLayerToolInnerCfg(flags, name = "InDetTestPixelLayerToolInner", **kwargs):
  if flags.Detector.GeometryITk:
    name = name.replace("InDet", "ITk")
    return ITkTestPixelLayerToolInnerCfg(flags, name, **kwargs)

  kwargs.setdefault("CheckActiveAreas", False)
  kwargs.setdefault("CheckDeadRegions", False)
  kwargs.setdefault("CheckDisabledFEs", False)
  kwargs.setdefault("OuterRadius", 100.) # To allow for extrapolation up to B-layer = next-to-innermost
  return InDetTestPixelLayerToolCfg(flags, name, **kwargs)

def InDetTrigTestPixelLayerToolInnerCfg(flags, name = "InDetTrigTestPixelLayerToolInner", **kwargs):
  kwargs.setdefault("CheckActiveAreas", True)
  kwargs.setdefault("PixelDetElStatus", "")
  return InDetTestPixelLayerToolInnerCfg(flags, name, **kwargs)

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
  kwargs.setdefault("OuterRadius", 350.) # Outer pixel detector radius, to allow for extrapolation up to outermost layer if needed

  tool = CompFactory.InDet.InDetTestPixelLayerTool( name, **kwargs)
  result.setPrivateTools( tool )
  return result

def ITkTestPixelLayerToolInnerCfg(flags, name = "ITkTestPixelLayerInnerTool", **kwargs):
  kwargs.setdefault("CheckActiveAreas", False)
  kwargs.setdefault("CheckDeadRegions", False)
  kwargs.setdefault("CheckDisabledFEs", False)
  kwargs.setdefault("OuterRadius", 130.) # To allow for extrapolation up to next-to-innermost layer
  return ITkTestPixelLayerToolCfg(flags, name, **kwargs)
