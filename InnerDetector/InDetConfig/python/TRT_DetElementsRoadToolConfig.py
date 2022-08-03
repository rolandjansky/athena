# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TRT_DetElementsRoadTool_xk package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TRT_DetElementsRoadCondAlgCfg(flags, name="TRT_DetElementsRoadCondAlg_xk", **kwargs):
    acc = ComponentAccumulator()
    acc.addCondAlgo(CompFactory.InDet.TRT_DetElementsRoadCondAlg_xk(name, **kwargs))
    return acc

def TRT_DetElementsRoadMaker_xkCfg(flags, name='InDetTRT_RoadMaker', **kwargs):
    from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
    acc = AtlasFieldCacheCondAlgCfg(flags)
    acc.merge(TRT_DetElementsRoadCondAlgCfg(flags)) # To produce the input TRT_DetElementsRoadData_xk CondHandle
    
    kwargs.setdefault("RoadWidth", 20.)
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
    kwargs.setdefault("PropagatorTool", acc.popToolsAndMerge(RungeKuttaPropagatorCfg(flags)))
    acc.setPrivateTools(CompFactory.InDet.TRT_DetElementsRoadMaker_xk(name, **kwargs))
    return acc
