
#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''@file TauDQAConfig.py
@author N.Pettersson
@date 2022-06-16
@brief Main CA-based python configuration for TauDQA
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PhysValTauCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    from AthenaCommon.Constants import WARNING
    kwargs.setdefault("EnableLumi", False)
    kwargs.setdefault("OutputLevel", WARNING)
    kwargs.setdefault("DetailLevel", 10)
    kwargs.setdefault("isMC", flags.Input.isMC)

    from TauDQA.TauDQATools import TauDQAPrimitiveTauSelectionToolCfg, TauDQANominalTauSelectionToolCfg, TauDQATauTruthMatchingToolCfg
    kwargs.setdefault("NominalTauSelectionTool", TauDQANominalTauSelectionToolCfg(flags))
    kwargs.setdefault("PrimitiveTauSelectionTool", TauDQAPrimitiveTauSelectionToolCfg(flags))
    kwargs.setdefault("TauTruthMatchingTool", TauDQATauTruthMatchingToolCfg(flags))
    tool = CompFactory.PhysValTau(**kwargs)
    acc.setPrivateTools(tool)
    return acc
    

def TauDQACfg(flags):
    acc = ComponentAccumulator()

    monMan = CompFactory.AthenaMonManager( "PhysValMonManager" )
    monMan.FileKey = "M_output"
    monMan.Environment = "altprod"
    monMan.ManualDataTypeSetup = True
    monMan.DataType = "monteCarlo"
    monMan.ManualRunLBSetup = True
    monMan.Run = 1
    monMan.LumiBlock = 1
    monMan.AthenaMonTools += [ acc.popToolsAndMerge(PhysValTauCfg(flags)) ]
    
    acc.addEventAlgo(monMan, primary = True)
    return acc