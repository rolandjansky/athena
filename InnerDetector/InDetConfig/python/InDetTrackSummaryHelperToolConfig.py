# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetTrackSummaryHelperTool package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetTrackSummaryHelperToolCfg(flags, name='InDetSummaryHelper', **kwargs):
  if flags.Detector.GeometryITk:
    name = name.replace("InDet", "ITk")
    return ITkTrackSummaryHelperToolCfg(flags, name, **kwargs)

  result = ComponentAccumulator()

  if 'AssoTool' not in kwargs :
    from InDetConfig.InDetAssociationToolsConfig import InDetPrdAssociationTool_setupCfg
    InDetPrdAssociationTool_setup = result.popToolsAndMerge(InDetPrdAssociationTool_setupCfg(flags))
    result.addPublicTool(InDetPrdAssociationTool_setup)
    kwargs.setdefault("AssoTool", InDetPrdAssociationTool_setup)

  if "HoleSearch" not in kwargs:
    from InDetConfig.InDetTrackHoleSearchConfig import InDetTrackHoleSearchToolCfg
    InDetTrackHoleSearchTool = result.popToolsAndMerge(InDetTrackHoleSearchToolCfg(flags))
    result.addPublicTool(InDetTrackHoleSearchTool)
    kwargs.setdefault("HoleSearch", InDetTrackHoleSearchTool)

  if not flags.Detector.EnableTRT:
    kwargs.setdefault("TRTStrawSummarySvc", "")

  kwargs.setdefault("TestBLayerTool", None)
  kwargs.setdefault("RunningTIDE_Ambi", flags.InDet.Tracking.doTIDE_Ambi)
  kwargs.setdefault("DoSharedHits", False)
  kwargs.setdefault("usePixel", flags.Detector.EnablePixel)
  kwargs.setdefault("useSCT", flags.Detector.EnableSCT)
  kwargs.setdefault("useTRT", flags.Detector.EnableTRT)

  result.setPrivateTools(CompFactory.InDet.InDetTrackSummaryHelperTool(name, **kwargs))
  return result

def InDetSummaryHelperNoHoleSearchCfg(flags, name='InDetSummaryHelperNoHoleSearch', **kwargs):
  kwargs.setdefault("HoleSearch", None)
  return InDetTrackSummaryHelperToolCfg(flags, name, **kwargs)

def InDetSummaryHelperSharedHitsCfg(flags, name='InDetSummaryHelperSharedHits', **kwargs):
  acc = ComponentAccumulator()

  if 'TestBLayerTool' not in kwargs and flags.Detector.EnablePixel:
    from InDetConfig.TrackingCommonConfig import InDetRecTestBLayerToolCfg
    InDetRecTestBLayerTool = acc.popToolsAndMerge(InDetRecTestBLayerToolCfg(flags))
    kwargs.setdefault("TestBLayerTool", InDetRecTestBLayerTool)

  kwargs.setdefault("DoSharedHits", flags.InDet.Tracking.doSharedHits)
  kwargs.setdefault("DoSharedHitsTRT", flags.InDet.Tracking.doSharedHits and flags.Detector.EnableTRT)

  InDetSummaryHelper = acc.popToolsAndMerge(InDetTrackSummaryHelperToolCfg(flags, name, **kwargs))
  acc.setPrivateTools(InDetSummaryHelper)
  return acc

def TrigTrackSummaryHelperToolCfg(flags, name="InDetTrigSummaryHelper", **kwargs):
  """
  based on: InnerDetector/InDetExample/InDetTrigRecExample/python/InDetTrigConfigRecLoadTools.py
  """

  result = ComponentAccumulator()

  if 'AssoTool' not in kwargs :
    from InDetConfig.InDetAssociationToolsConfig import TrigPrdAssociationToolCfg
    associationTool = result.popToolsAndMerge( TrigPrdAssociationToolCfg(flags) )
    result.addPublicTool(associationTool)
    kwargs.setdefault("AssoTool", associationTool)

  if "HoleSearch" not in kwargs:
    from InDetConfig.InDetTrackHoleSearchConfig import TrigHoleSearchToolCfg
    holeSearchTool = result.popToolsAndMerge( TrigHoleSearchToolCfg(flags) )
    result.addPublicTool(holeSearchTool)
    kwargs.setdefault("HoleSearch", holeSearchTool)

  # Kept for consistency with previous config but unclear if different from default TRT_StrawStatusSummaryTool loaded in C++
  if "TRTStrawSummarySvc" not in kwargs:
    from TRT_ConditionsServices.TRT_ConditionsServicesConfig import TRT_StrawStatusSummaryToolCfg
    TRT_StrawStatusSummaryTool = result.popToolsAndMerge( TRT_StrawStatusSummaryToolCfg(flags) )
    kwargs.setdefault("TRTStrawSummarySvc", TRT_StrawStatusSummaryTool)
      
  if "TestBLayerTool" not in kwargs:
    from TrigInDetConfig.TrigInDetConfig import TestBlayerToolCfg
    TestBlayerTool = result.popToolsAndMerge(TestBlayerToolCfg(flags))
    kwargs.setdefault("TestBLayerTool", TestBlayerTool)

  kwargs.setdefault("DoSharedHits", True)
  kwargs.setdefault("usePixel", flags.Detector.EnablePixel)
  kwargs.setdefault("useSCT", flags.Detector.EnableSCT)
  kwargs.setdefault("useTRT", flags.Detector.EnableTRT)

  result.setPrivateTools(CompFactory.InDet.InDetTrackSummaryHelperTool(name, **kwargs))
  return result

def ITkTrackSummaryHelperToolCfg(flags, name='ITkSummaryHelper', **kwargs):
  result = ComponentAccumulator()

  if 'AssoTool' not in kwargs :
    from InDetConfig.InDetAssociationToolsConfig import ITkPrdAssociationTool_setupCfg
    assoTool = result.popToolsAndMerge(ITkPrdAssociationTool_setupCfg(flags))
    result.addPublicTool(assoTool)
    kwargs.setdefault("AssoTool", assoTool)

  if "HoleSearch" not in kwargs:
    from InDetConfig.InDetTrackHoleSearchConfig import ITkTrackHoleSearchToolCfg
    ITkTrackHoleSearchTool = result.popToolsAndMerge(ITkTrackHoleSearchToolCfg(flags))
    result.addPublicTool(ITkTrackHoleSearchTool)
    kwargs.setdefault("HoleSearch", ITkTrackHoleSearchTool)

  kwargs.setdefault("TRTStrawSummarySvc", "")
  kwargs.setdefault("TestBLayerTool", None)
  kwargs.setdefault("RunningTIDE_Ambi", True)
  kwargs.setdefault("DoSharedHits", False)
  kwargs.setdefault("usePixel", flags.Detector.EnableITkPixel)
  kwargs.setdefault("useSCT", flags.Detector.EnableITkStrip)
  kwargs.setdefault("useTRT", False)

  result.setPrivateTools(CompFactory.InDet.InDetTrackSummaryHelperTool(name, **kwargs))
  return result

def ITkSummaryHelperNoHoleSearchCfg(flags, name='ITkSummaryHelperNoHoleSearch', **kwargs):
  kwargs.setdefault("HoleSearch", None)
  return ITkTrackSummaryHelperToolCfg(flags, name, **kwargs)

def ITkSummaryHelperSharedHitsCfg(flags, name='ITkSummaryHelperSharedHits', **kwargs):
  acc = ComponentAccumulator()

  if 'TestBLayerTool' not in kwargs and flags.Detector.EnableITkPixel:
    from InDetConfig.ITkTrackingCommonConfig import ITkRecTestBLayerToolCfg
    ITkRecTestBLayerTool = acc.popToolsAndMerge(ITkRecTestBLayerToolCfg(flags))
    kwargs.setdefault("TestBLayerTool", ITkRecTestBLayerTool)

  kwargs.setdefault("DoSharedHits", flags.ITk.Tracking.doSharedHits)
  return ITkTrackSummaryHelperToolCfg(flags, name = name, **kwargs)
