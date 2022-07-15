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

def PFODQACf(flags):
    acc = ComponentAccumulator()

    monMan = CompFactory.AthenaMonManager( "PhysValMonManager" )
    monMan.FileKey = "PhysVal"
    monMan.Environment = "altprod"
    monMan.ManualDataTypeSetup = True
    monMan.DataType = "monteCarlo"
    monMan.ManualRunLBSetup = True
    monMan.Run = 1
    monMan.LumiBlock = 1
    monMan.AthenaMonTools += acc.popToolsAndMerge(PhysValPFOCfg(flags))
   
    acc.addEventAlgo(monMan, primary = True)
    return acc
