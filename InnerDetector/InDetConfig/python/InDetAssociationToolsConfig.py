# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetAssociationTools package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetPrdAssociationTool_noTRTCfg(flags, name='InDetPrdAssociationTool_noTRT', **kwargs) :
  acc = ComponentAccumulator()
  kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'ITkPixelClusterAmbiguitiesMap' if flags.Detector.GeometryITk else 'PixelClusterAmbiguitiesMap')
  kwargs.setdefault("addTRToutliers", False)
  InDetPRD_AssociationToolGangedPixels = CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(name, **kwargs)
  acc.setPrivateTools(InDetPRD_AssociationToolGangedPixels)
  return acc

def InDetPrdAssociationToolCfg(flags, name='InDetPrdAssociationTool', **kwargs) :
  if flags.Detector.GeometryITk:
    name = name.replace("InDet", "ITk")
    return ITkPrdAssociationToolCfg(flags, name,**kwargs)

  kwargs.setdefault("addTRToutliers", True)
  return InDetPrdAssociationTool_noTRTCfg(flags, name, **kwargs)

def InDetPrdAssociationTool_setupCfg(flags, name='InDetPrdAssociationTool_setup', **kwargs) :
  kwargs.setdefault("SetupCorrect", True)
  return InDetPrdAssociationToolCfg(flags, name, **kwargs)

def TrigPrdAssociationToolCfg(flags, name='InDetTrigPrdAssociationTool', **kwargs):
  kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'TrigPixelClusterAmbiguitiesMap')
  return InDetPrdAssociationTool_noTRTCfg(flags, name, **kwargs)

def ITkPrdAssociationToolCfg(flags, name='ITkPrdAssociationTool', **kwargs):
  return InDetPrdAssociationTool_noTRTCfg(flags, name, **kwargs)

def ITkPrdAssociationTool_setupCfg(flags, name='ITkPrdAssociationTool_setup', **kwargs):
  kwargs.setdefault("SetupCorrect", True)
  return ITkPrdAssociationToolCfg(flags, name, **kwargs)

def InDetPRDtoTrackMapToolGangedPixelsCfg(flags, name='PRDtoTrackMapToolGangedPixels', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'PixelClusterAmbiguitiesMap')
    kwargs.setdefault("addTRToutliers", True)
    acc.setPrivateTools(CompFactory.InDet.InDetPRDtoTrackMapToolGangedPixels(name, **kwargs))
    return acc

def TrigPRDtoTrackMapToolGangedPixelsCfg(flags, name='InDetTrigPRDtoTrackMapToolGangedPixels', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'TrigPixelClusterAmbiguitiesMap')
    kwargs.setdefault("addTRToutliers", False)
    acc.setPrivateTools(CompFactory.InDet.InDetPRDtoTrackMapToolGangedPixels(name, **kwargs))
    return acc

def ITkPRDtoTrackMapToolGangedPixelsCfg(flags, name='ITkPRDtoTrackMapToolGangedPixels', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("PixelClusterAmbiguitiesMapName", 'ITkPixelClusterAmbiguitiesMap')
    kwargs.setdefault("addTRToutliers", False)
    acc.setPrivateTools(CompFactory.InDet.InDetPRDtoTrackMapToolGangedPixels(name, **kwargs))
    return acc
