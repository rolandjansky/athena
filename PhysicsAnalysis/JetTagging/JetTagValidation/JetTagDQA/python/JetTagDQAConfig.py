#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''@file InDetPhysValMonitoringConfig.py
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
    

def JetTagDQACfg(flags):
    acc = ComponentAccumulator()

    monMan = CompFactory.AthenaMonManager( "PhysValMonManager" )
    monMan.FileKey = "M_output"
    monMan.Environment = "altprod"
    monMan.ManualDataTypeSetup = True
    monMan.DataType = "monteCarlo"
    monMan.ManualRunLBSetup = True
    monMan.Run = 1
    monMan.LumiBlock = 1
    monMan.AthenaMonTools += [ acc.popToolsAndMerge(PhysValBTagCfg(flags)) ]
    
    acc.addEventAlgo(monMan, primary = True)
    return acc
