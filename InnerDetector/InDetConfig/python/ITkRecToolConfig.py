# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def ITkPrdAssociationToolCfg(flags, name='ITkPrdAssociationTool',**kwargs) :
  acc = ComponentAccumulator()
  '''
  Provide an instance for all clients in which the tool is only set in c++
  '''
  the_name = makeName( name, kwargs)

  kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'ITkPixelClusterAmbiguitiesMap')
  kwargs.setdefault("addTRToutliers", False)

  ITkPRD_AssociationToolGangedPixels = CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(the_name, **kwargs)
  acc.addPublicTool(ITkPRD_AssociationToolGangedPixels, primary=True)
  return acc

def ITkPrdAssociationTool_setupCfg(flags, name='ITkPrdAssociationTool_setup',**kwargs) :
  '''
  Provide an instance for all clients which set the tool explicitely
  '''
  kwargs.setdefault("SetupCorrect", True)
  return ITkPrdAssociationToolCfg(flags, name, **kwargs)

def ITkTrigPrdAssociationToolCfg(flags, name='ITkTrigPrdAssociationTool_setup',**kwargs) :
  kwargs.setdefault("PixelClusterAmbiguitiesMapName", "ITkTrigPixelClusterAmbiguitiesMap")
  kwargs.setdefault("addTRToutliers", False)
  return ITkPrdAssociationToolCfg(flags, name, **kwargs)

def ITkTrackSummaryHelperToolCfg(flags, name='ITkSummaryHelper', **kwargs):
  result = ComponentAccumulator()

  the_name = makeName( name, kwargs)
  isHLT=kwargs.pop("isHLT",False)

  if 'AssoTool' not in kwargs :
    assoTool = None
    if not isHLT:
      assoTool = result.getPrimaryAndMerge(ITkPrdAssociationTool_setupCfg(flags))
    else:
      assoTool = result.getPrimaryAndMerge(ITkTrigPrdAssociationToolCfg(flags))
    kwargs.setdefault("AssoTool", assoTool)

  if "HoleSearch" not in kwargs:
    ITkTrackHoleSearchTool = result.getPrimaryAndMerge(ITkTrackHoleSearchToolCfg(flags))
    # FIXME: assuming we don't use DetailedPixelHoleSearch (since it seems to be off in standard workflows)
    kwargs.setdefault("HoleSearch", ITkTrackHoleSearchTool)

  kwargs.setdefault("TRTStrawSummarySvc", "")
  kwargs.setdefault("PixelToTPIDTool", None)
  kwargs.setdefault("TestBLayerTool", None)
  kwargs.setdefault("RunningTIDE_Ambi", flags.ITk.doTIDE_Ambi)
  kwargs.setdefault("DoSharedHits", False)
  kwargs.setdefault("usePixel", flags.Detector.EnableITkPixel)
  kwargs.setdefault("useSCT", flags.Detector.EnableITkStrip)
  kwargs.setdefault("useTRT", False)

  result.setPrivateTools(CompFactory.InDet.InDetTrackSummaryHelperTool(the_name, **kwargs))
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

  if flags.Beam.Type == "cosmics":
    kwargs.setdefault("Cosmics", True)

  kwargs.setdefault("CountDeadModulesAfterLastHit", True)

  result.addPublicTool(CompFactory.InDet.InDetTrackHoleSearchTool(name, **kwargs), primary=True)
  return result

def ITkTestPixelLayerToolCfg(flags, name = "ITkTestPixelLayerTool", **kwargs):
  the_name = makeName( name, kwargs)
  result = ComponentAccumulator()
  if 'PixelSummaryTool' not in kwargs :
    from PixelConditionsTools.ITkPixelConditionsSummaryConfig import ITkPixelConditionsSummaryCfg
    kwargs.setdefault("PixelSummaryTool", result.popToolsAndMerge(ITkPixelConditionsSummaryCfg(flags)))

  if 'Extrapolator' not in kwargs :
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    extrapolator = result.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    result.addPublicTool(extrapolator)  # TODO: migrate to private?
    kwargs.setdefault("Extrapolator", extrapolator)

  kwargs.setdefault("CheckActiveAreas", flags.ITk.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDeadRegions", flags.ITk.checkDeadElementsOnTrack)
  kwargs.setdefault("CheckDisabledFEs", flags.ITk.checkDeadElementsOnTrack)

  tool = CompFactory.InDet.InDetTestPixelLayerTool( name = the_name, **kwargs)
  result.setPrivateTools( tool )
  return result

def ITkPatternPropagatorCfg(flags, name='ITkPatternPropagator', **kwargs):
    the_name = makeName( name, kwargs)
    result = ComponentAccumulator()
    tool = CompFactory.Trk.RungeKuttaPropagator(name = the_name, **kwargs)
    result.addPublicTool( tool, primary=True )
    return result

def ITkPatternUpdatorCfg(flags, name='ITkPatternUpdator', **kwargs):
    the_name = makeName(name, kwargs)
    result = ComponentAccumulator()
    tool = CompFactory.Trk.KalmanUpdator_xk(name = the_name, **kwargs)
    result.setPrivateTools( tool )
    return result

def ITkUpdatorCfg(flags, name = 'ITkUpdator', **kwargs):
    the_name = makeName( name, kwargs )
    result = ComponentAccumulator()

    tool = None
    if flags.ITk.kalmanUpdator == "fast" :
        tool = CompFactory.Trk.KalmanUpdator_xk(name = the_name, **kwargs)
    elif flags.ITk.kalmanUpdator == "weight" :
        tool = CompFactory.Trk.KalmanWeightUpdator(name = the_name, **kwargs)
    elif flags.ITk.kalmanUpdator == "smatrix" :
        tool = CompFactory.Trk.KalmanUpdatorSMatrix(name = the_name, **kwargs)
    elif flags.ITk.kalmanUpdator == "amg" :
        tool = CompFactory.Trk.KalmanUpdatorAmg(name = the_name, **kwargs)
    else :
        tool = CompFactory.Trk.KalmanUpdator(name = the_name, **kwargs)

    result.setPrivateTools( tool )
    return result

def splitDefaultPrefix(name) :
    default_prefix=''
    for prefix in ['ITk'] :
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

