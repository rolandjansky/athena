# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def InDetPrdAssociationToolCfg(flags, name='InDetPrdAssociationTool', **kwargs) :
  if flags.Detector.GeometryITk:
    name = name.replace("InDet", "ITk")
    from InDetConfig.ITkRecToolConfig import ITkPrdAssociationToolCfg
    return ITkPrdAssociationToolCfg(flags, name,**kwargs)

  acc = ComponentAccumulator()
  '''
  Provide an instance for all clients in which the tool is only set in c++
  '''
  the_name = makeName( name, kwargs)

  kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'PixelClusterAmbiguitiesMap') # InDetKeys.GangedPixelMap
  kwargs.setdefault("addTRToutliers", True)

  InDetPRD_AssociationToolGangedPixels = CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(the_name, **kwargs)
  acc.setPrivateTools(InDetPRD_AssociationToolGangedPixels)
  return acc

def InDetPrdAssociationTool_setupCfg(flags, name='InDetPrdAssociationTool_setup', **kwargs) :
  '''
  Provide an instance for all clients which set the tool explicitely
  '''
  kwargs.setdefault("SetupCorrect", True)
  return InDetPrdAssociationToolCfg(flags, name, **kwargs)

def InDetTrigPrdAssociationToolCfg(flags, name='InDetTrigPrdAssociationTool_setup', **kwargs) :
  kwargs.setdefault("PixelClusterAmbiguitiesMapName", "TrigPixelClusterAmbiguitiesMap")
  kwargs.setdefault("addTRToutliers", False)

  return InDetPrdAssociationToolCfg(flags, name, **kwargs)

def InDetTrackSummaryHelperToolCfg(flags, name='InDetSummaryHelper', **kwargs):
  if flags.Detector.GeometryITk:
    name = name.replace("InDet", "ITk")
    from InDetConfig.ITkRecToolConfig import ITkTrackSummaryHelperToolCfg
    return ITkTrackSummaryHelperToolCfg(flags, name, **kwargs)

  result = ComponentAccumulator()

  the_name = makeName( name, kwargs)
  isHLT=kwargs.pop("isHLT",False)

  if 'AssoTool' not in kwargs :
    if not isHLT:
      InDetPrdAssociationTool_setup = result.popToolsAndMerge(InDetPrdAssociationTool_setupCfg(flags))
      result.addPublicTool(InDetPrdAssociationTool_setup)
      kwargs.setdefault("AssoTool", InDetPrdAssociationTool_setup)
    else:
      InDetTrigPrdAssociationTool = result.popToolsAndMerge(InDetTrigPrdAssociationToolCfg(flags))
      result.addPublicTool(InDetTrigPrdAssociationTool)
      kwargs.setdefault("AssoTool", InDetTrigPrdAssociationTool)

  if "HoleSearch" not in kwargs:
    acc = InDetTrackHoleSearchToolCfg(flags)
    # FIXME: assuming we don't use DetailedPixelHoleSearch (since it seems to be off in standard workflows)
    kwargs.setdefault("HoleSearch", acc.getPrimary())
    result.merge(acc)

  if not flags.Detector.EnableTRT:
    kwargs.setdefault("TRTStrawSummarySvc", "")

  kwargs.setdefault("PixelToTPIDTool", None)
  kwargs.setdefault("TestBLayerTool", None)
  kwargs.setdefault("RunningTIDE_Ambi", flags.InDet.doTIDE_Ambi)
  kwargs.setdefault("DoSharedHits", False)
  kwargs.setdefault("usePixel", flags.Detector.EnablePixel)
  kwargs.setdefault("useSCT", flags.Detector.EnableSCT)
  kwargs.setdefault("useTRT", flags.Detector.EnableTRT)

  result.setPrivateTools(CompFactory.InDet.InDetTrackSummaryHelperTool(the_name, **kwargs))
  return result

def InDetBoundaryCheckToolCfg(flags, name='InDetBoundaryCheckTool', **kwargs):
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

  indet_boundary_check_tool = CompFactory.InDet.InDetBoundaryCheckTool(name, **kwargs)
  result.setPrivateTools(indet_boundary_check_tool)
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

  if flags.Beam.Type == "cosmics" :
    kwargs.setdefault("Cosmics", True)

  kwargs.setdefault( "CountDeadModulesAfterLastHit" , True)

  indet_hole_search_tool = CompFactory.InDet.InDetTrackHoleSearchTool(name, **kwargs)
  result.addPublicTool(indet_hole_search_tool, primary=True)
  return result




def InDetTestPixelLayerToolCfg(flags, name = "InDetTestPixelLayerTool", **kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  if 'PixelSummaryTool' not in kwargs :
    from PixelConditionsTools.PixelConditionsSummaryConfig import PixelConditionsSummaryCfg
    kwargs.setdefault("PixelSummaryTool", result.popToolsAndMerge(PixelConditionsSummaryCfg(flags)))

  if 'Extrapolator' not in kwargs :
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    kwargs.setdefault("Extrapolator", result.getPrimaryAndMerge(InDetExtrapolatorCfg(flags)))

  kwargs.setdefault("CheckActiveAreas", flags.InDet.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDeadRegions", flags.InDet.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDisabledFEs", flags.InDet.checkDeadElementsOnTrack)

  tool = CompFactory.InDet.InDetTestPixelLayerTool( name = the_name, **kwargs)
  result.setPrivateTools( tool )
  return result

def splitDefaultPrefix(name) :
    default_prefix=''
    for prefix in ['InDet','InDetTrig'] :
        if name[0:len(prefix)] == prefix :
            name=name[len(prefix):]
            default_prefix=prefix
            break
    return default_prefix,name

def makeName( name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix

def makeNameGetPreAndSuffix( name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix,namePrefix,nameSuffix
