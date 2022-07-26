#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''@file JetTagDQAConfig.py
@author T. Strebler
@date 2022-06-16
@brief Main CA-based python configuration for JetTagDQA
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PhysValBTagCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("DetailLevel", 10)
    kwargs.setdefault("isData", not flags.Input.isMC)

    import ROOT
    path = ROOT.PathResolver.find_file( 'JetTagDQA/PhysValBtag_VariablesMenu.json', 'DATAPATH' )
    from PhysValMonitoring.PhysValUtils import getHistogramDefinitions
    kwargs.setdefault("HistogramDefinitions", getHistogramDefinitions(path, 'PHYSVAL', 'ALL'))

    tool = CompFactory.JetTagDQA.PhysValBTag(**kwargs)
    acc.setPrivateTools(tool)
    return acc
    

def JetTagDQACfg(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("FileKey", "M_output")
    kwargs.setdefault("Environment", "altprod")
    kwargs.setdefault("ManualDataTypeSetup", True)
    kwargs.setdefault("DataType", "monteCarlo")
    kwargs.setdefault("ManualRunLBSetup", True)
    kwargs.setdefault("Run", 1)
    kwargs.setdefault("LumiBlock", 1)
    kwargs.setdefault("AthenaMonTools", [acc.popToolsAndMerge(PhysValBTagCfg(flags))])
    acc.addEventAlgo(CompFactory.AthenaMonManager( "PhysValMonManager", **kwargs), primary = True)
 
    return acc
