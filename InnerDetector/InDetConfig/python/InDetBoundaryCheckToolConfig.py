# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetBoundaryCheckTool package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

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
    from InDetConfig.InDetRecToolConfig import InDetTestPixelLayerToolCfg
    kwargs.setdefault("PixelLayerTool", result.popToolsAndMerge(InDetTestPixelLayerToolCfg(flags)))

  kwargs.setdefault("UsePixel", flags.Detector.EnablePixel)
  kwargs.setdefault("UseSCT", flags.Detector.EnableSCT)
  kwargs.setdefault("CheckBadSCT", flags.InDet.checkDeadElementsOnTrack)

  result.setPrivateTools(CompFactory.InDet.InDetBoundaryCheckTool(name, **kwargs))
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
    from InDetConfig.ITkRecToolConfig import ITkTestPixelLayerToolCfg
    kwargs.setdefault("PixelLayerTool", result.popToolsAndMerge(ITkTestPixelLayerToolCfg(flags)))

  kwargs.setdefault("UsePixel", flags.Detector.EnableITkPixel)
  kwargs.setdefault("UseSCT", flags.Detector.EnableITkStrip)
  kwargs.setdefault("CheckBadSCT", False)

  indet_boundary_check_tool = CompFactory.InDet.InDetBoundaryCheckTool(name, **kwargs)
  result.setPrivateTools(indet_boundary_check_tool)
  return result
