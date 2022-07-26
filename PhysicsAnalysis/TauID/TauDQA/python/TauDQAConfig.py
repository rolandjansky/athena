
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
    

def TauDQACfg(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("FileKey", "M_output")
    kwargs.setdefault("Environment", "altprod")
    kwargs.setdefault("ManualDataTypeSetup", True)
    kwargs.setdefault("DataType", "monteCarlo")
    kwargs.setdefault("ManualRunLBSetup", True)
    kwargs.setdefault("Run", 1)
    kwargs.setdefault("LumiBlock", 1)
    kwargs.setdefault("AthenaMonTools", [acc.popToolsAndMerge(PhysValTauCfg(flags))])
    acc.addEventAlgo(CompFactory.AthenaMonManager( "PhysValMonManager", **kwargs), primary = True)
 
    return acc