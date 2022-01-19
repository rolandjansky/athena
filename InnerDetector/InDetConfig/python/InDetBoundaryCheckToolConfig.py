# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetBoundaryCheckTool package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetBoundaryCheckToolCfg(flags, name='InDetBoundarySearchTool', **kwargs):
  result = ComponentAccumulator()

  if 'SctSummaryTool' not in kwargs:
    if flags.Detector.EnableSCT:
      from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
      kwargs.setdefault("SctSummaryTool", result.popToolsAndMerge(SCT_ConditionsSummaryToolCfg(flags)))
    else:
      kwargs.setdefault("SctSummaryTool", None)

  if flags.Detector.EnableSCT:
    if "SCTDetElStatus" not in kwargs and not flags.Common.isOnline :
      from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConfig  import SCT_DetectorElementStatusAlgCfg
      result.merge( SCT_DetectorElementStatusAlgCfg(flags) )
      kwargs.setdefault("SCTDetElStatus", "SCTDetectorElementStatus" )

  if flags.Detector.EnablePixel :
    if 'PixelLayerTool' not in kwargs :
      from InDetConfig.InDetTestPixelLayerConfig import InDetTestPixelLayerToolCfg
      kwargs.setdefault("PixelLayerTool", result.popToolsAndMerge(InDetTestPixelLayerToolCfg(flags)))
  else :
    kwargs.setdefault("PixelLayerTool", "")


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
      kwargs.setdefault("SctSummaryTool", result.popToolsAndMerge(ITkStripConditionsSummaryToolCfg(flags)))
    else:
      kwargs.setdefault("SctSummaryTool", None)

  if 'PixelLayerTool' not in kwargs :
    from InDetConfig.InDetTestPixelLayerConfig import ITkTestPixelLayerToolCfg
    kwargs.setdefault("PixelLayerTool", result.popToolsAndMerge(ITkTestPixelLayerToolCfg(flags)))

  kwargs.setdefault("UsePixel", flags.Detector.EnableITkPixel)
  kwargs.setdefault("UseSCT", flags.Detector.EnableITkStrip)
  kwargs.setdefault("CheckBadSCT", False)

  indet_boundary_check_tool = CompFactory.InDet.InDetBoundaryCheckTool(name, **kwargs)
  result.setPrivateTools(indet_boundary_check_tool)
  return result
