#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''@file InDetPhysValMonitoringConfig.py
@author N.Pettersson
@date 2022-07-08
@brief Main CA-based python configuration for ClusterDQACfg
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PhysValClusterToolCfg(name, container, flags, **kwargs):
    acc = ComponentAccumulator()

    from AthenaCommon.Constants import INFO
    kwargs.setdefault("OutputLevel", INFO)
    kwargs.setdefault("DetailLevel", 10)
    kwargs.setdefault("EnableLumi", False)
    kwargs.setdefault("ClusterContainerName", container)

    tool = CompFactory.PhysValCluster(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc


def PhysValClusterCfg(flags):
    acc = ComponentAccumulator()

    tools = [ acc.popToolsAndMerge(PhysValClusterToolCfg("PhysValCluster_LC", "CaloCalTopoClusters", flags)) ]
    tools += [ acc.popToolsAndMerge(PhysValClusterToolCfg("PhysValCluster_origin", "LCOriginTopoClusters", flags)) ]
    acc.setPrivateTools(tools)
    return acc


def ClusterDQACfg(flags):
    acc = ComponentAccumulator()

    monMan = CompFactory.AthenaMonManager( "PhysValMonManager" )
    monMan.FileKey = "M_output"
    monMan.Environment = "altprod"
    monMan.ManualDataTypeSetup = True
    monMan.DataType = "monteCarlo"
    monMan.ManualRunLBSetup = True
    monMan.Run = 1
    monMan.LumiBlock = 1
    monMan.AthenaMonTools +=  acc.popToolsAndMerge(PhysValClusterCfg(flags)) 

    acc.addEventAlgo(monMan, primary = True)
    return acc
