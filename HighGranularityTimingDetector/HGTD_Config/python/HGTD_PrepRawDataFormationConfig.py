# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def HGTD_ClusterMakerToolCfg(flags, name = "HGTD_ClusterMakerTool", **kwargs):
    """Configures a tool that forms HGTD clusters """
    acc = ComponentAccumulator()

    acc.setPrivateTools(CompFactory.HGTD_ClusterMakerTool(name, **kwargs))
    return acc

def SinglePadClusterToolCfg(flags, name = "SinglePadClusterTool", **kwargs):
    """Configures a tool that creates 1-to-1 HGTD clusters out of single pads """
    acc = ComponentAccumulator()

    kwargs.setdefault("ClusterMakerTool", acc.popToolsAndMerge(HGTD_ClusterMakerToolCfg(flags)))
    acc.setPrivateTools(CompFactory.HGTD.SinglePadClusterTool(name, **kwargs))
    return acc

def PadClusterizationCfg(flags, name = "PadClusterizationAlg", **kwargs):
    """Schedules a clusterization alg to produce HGTD_Clusters out of HGTD_RDOs """
    acc = ComponentAccumulator()

    kwargs.setdefault("ClusterizationTool", acc.popToolsAndMerge(SinglePadClusterToolCfg(flags)))
    kwargs.setdefault("RDOContainerName", "HGTD_RDOs")
    kwargs.setdefault("PRDContainerName", "HGTD_Clusters")
    acc.addEventAlgo(CompFactory.HGTD.PadClusterizationAlg(name, **kwargs))
    return acc
