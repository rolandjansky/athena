#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''@file EGammaPhysValMonitoringConfig.py
@author T. Strebler
@date 2022-07-05
@brief Main CA-based python configuration for EGammaPhysValMonitoring
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def EgammaPhysValMonitoringToolCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("EnableLumi", False)
    kwargs.setdefault("DetailLevel", 10)
    kwargs.setdefault("isMC", flags.Input.isMC)

    from MCTruthClassifier.MCTruthClassifierConfig import MCTruthClassifierCaloTruthMatchCfg
    kwargs.setdefault("MCTruthClassifier", acc.popToolsAndMerge(MCTruthClassifierCaloTruthMatchCfg(flags)))

    tool = CompFactory.EgammaPhysValMonitoring.EgammaPhysValMonitoringTool(**kwargs)
    acc.setPrivateTools(tool)
    return acc

def EgammaPhysValMonitoringCfg(flags):
    acc = ComponentAccumulator()

    monMan = CompFactory.AthenaMonManager( "PhysValMonManager" )
    monMan.FileKey = "M_output"
    monMan.Environment = "altprod"
    monMan.ManualDataTypeSetup = True
    monMan.DataType = "monteCarlo"
    monMan.ManualRunLBSetup = True
    monMan.Run = 1
    monMan.LumiBlock = 1
    monMan.AthenaMonTools += [ acc.popToolsAndMerge(EgammaPhysValMonitoringToolCfg(flags)) ]
    
    acc.addEventAlgo(monMan, primary = True)
    return acc

