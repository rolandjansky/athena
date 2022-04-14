# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetTrackPRD_Association package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetTrackPRD_AssociationCfg(flags, name='InDetTrackPRD_Association', **kwargs):
    acc = ComponentAccumulator()

    if kwargs.get('TracksName', None) is None :
        raise Exception('Not TracksName argument provided')

    if 'AssociationTool' not in kwargs:
        from InDetConfig.InDetAssociationToolsConfig import InDetPRDtoTrackMapToolGangedPixelsCfg
        kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(InDetPRDtoTrackMapToolGangedPixelsCfg(flags)))

    if "AssociationMapName" not in kwargs:
        kwargs.setdefault("AssociationMapName", "InDetPRDtoTrackMap" + flags.InDet.Tracking.ActivePass.extension)

    acc.addEventAlgo(CompFactory.InDet.InDetTrackPRD_Association(name, **kwargs))
    return acc

def ITkTrackPRD_AssociationCfg(flags, name='ITkTrackPRD_Association', **kwargs):
    acc = ComponentAccumulator()

    if kwargs.get('TracksName', None) is None :
        raise Exception('Not TracksName argument provided')

    if 'AssociationTool' not in kwargs:
        from InDetConfig.InDetAssociationToolsConfig import ITkPRDtoTrackMapToolGangedPixelsCfg
        kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(ITkPRDtoTrackMapToolGangedPixelsCfg(flags)))

    kwargs.setdefault("AssociationMapName", 'ITkPRDtoTrackMap' + flags.ITk.Tracking.ActivePass.extension)
    acc.addEventAlgo(CompFactory.InDet.InDetTrackPRD_Association(name, **kwargs))
    return acc
