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

def EgammaPhysValMonitoringCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("FileKey", "M_output")
    kwargs.setdefault("Environment", "altprod")
    kwargs.setdefault("ManualDataTypeSetup", True)
    kwargs.setdefault("DataType", "monteCarlo")
    kwargs.setdefault("ManualRunLBSetup", True)
    kwargs.setdefault("Run", 1)
    kwargs.setdefault("LumiBlock", 1)
    kwargs.setdefault("AthenaMonTools", [acc.popToolsAndMerge(EgammaPhysValMonitoringToolCfg(flags))])
    acc.addEventAlgo(CompFactory.AthenaMonManager( "PhysValMonManager", **kwargs), primary = True)

    return acc

