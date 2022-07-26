#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''@file PFlowPhysVal.py
@author N. Pettersson
@date 2022-07-11
@brief Main CA-based python configuration for PFODQA
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PhysValPFOToolCfg(name, container='', **kwargs):
    acc = ComponentAccumulator()

    from AthenaCommon.Constants import WARNING
    kwargs.setdefault("OutputLevel", WARNING)
    kwargs.setdefault("DetailLevel", 10)
    kwargs.setdefault("EnableLumi", False)
    kwargs.setdefault("FlowElementContainerName", container)
    kwargs.setdefault("useNeutralFE", "Neutral" in container)
    tool = CompFactory.PhysValFE(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def PhysValPFOCfg(flags):
    acc = ComponentAccumulator()

    tools = [ acc.popToolsAndMerge(PhysValPFOToolCfg("PhysValFE_charged", "JetETMissChargedParticleFlowObjects")) ]
    tools += [ acc.popToolsAndMerge(PhysValPFOToolCfg("PhysValFE_neutral", "JetETMissNeutralParticleFlowObjects")) ]
    acc.setPrivateTools(tools)
    return acc

def PFODQACf(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("FileKey", "PhysVal")
    kwargs.setdefault("Environment", "altprod")
    kwargs.setdefault("ManualDataTypeSetup", True)
    kwargs.setdefault("DataType", "monteCarlo")
    kwargs.setdefault("ManualRunLBSetup", True)
    kwargs.setdefault("Run", 1)
    kwargs.setdefault("LumiBlock", 1)
    kwargs.setdefault("AthenaMonTools", acc.popToolsAndMerge(PhysValPFOCfg(flags)))
    acc.addEventAlgo(CompFactory.AthenaMonManager( "PhysValMonManager", **kwargs), primary = True)
 
    return acc
