#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''@file PhysValMonitoringConfig.py
@author T. Strebler
@date 2022-06-16
@brief Main CA-based python configuration for PhysValMonitoring
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PhysValMonitoringCfg(flags):
    acc = ComponentAccumulator()

    monMan = CompFactory.AthenaMonManager( "PhysValMonManager" )
    monMan.FileKey = "M_output"
    monMan.Environment = "altprod"
    monMan.ManualDataTypeSetup = True
    monMan.DataType = "monteCarlo"
    monMan.ManualRunLBSetup = True
    monMan.Run = 1
    monMan.LumiBlock = 1

    if flags.PhysVal.doInDet:
        from InDetPhysValMonitoring.InDetPhysValMonitoringConfig import InDetPhysValMonitoringToolCfg
        monMan.AthenaMonTools += [ acc.popToolsAndMerge(InDetPhysValMonitoringToolCfg(flags)) ]
    if flags.PhysVal.doBtag:
        from JetTagDQA.JetTagDQAConfig import PhysValBTagCfg
        monMan.AthenaMonTools += [ acc.popToolsAndMerge(PhysValBTagCfg(flags)) ]
    if flags.PhysVal.doMET:
        from MissingEtDQA.MissingEtDQAConfig import PhysValMETCfg
        monMan.AthenaMonTools += [ acc.popToolsAndMerge(PhysValMETCfg(flags)) ]
    if flags.PhysVal.doEgamma:
        from EgammaPhysValMonitoring.EgammaPhysValMonitoringConfig import EgammaPhysValMonitoringToolCfg
        monMan.AthenaMonTools += [ acc.popToolsAndMerge(EgammaPhysValMonitoringToolCfg(flags)) ]
    if flags.PhysVal.doTau:
        from TauDQA.TauDQAConfig import PhysValTauCfg
        monMan.AthenaMonTools += [ acc.popToolsAndMerge(PhysValTauCfg(flags))]
    if flags.PhysVal.doJet:
        from JetValidation.JetValidationConfig import PhysValJetCfg
        monMan.AthenaMonTools += [ acc.popToolsAndMerge(PhysValJetCfg(flags)) ]
    if flags.PhysVal.doTopoCluster:
        from PFODQA.ClusterDQAConfig import  ClusterDQACfg
        monMan.AthenaMonTools += [ acc.popToolsAndMerge(ClusterDQACfg(flags))]
    if flags.PhysVal.doZee:
        from ZeeValidation.ZeeValidationMonToolConfig import PhysValZeeCfg
        monMan.AthenaMonTools += [ acc.popToolsAndMerge(PhysValZeeCfg(flags)) ]
    if flags.PhysVal.doPFlow:
        from PFODQA.PFPhysValConfig import PhysValPFOCfg
        monMan.AthenaMonTools += acc.popToolsAndMerge(PhysValPFOCfg(flags))
    if flags.PhysVal.doMuon:
        from MuonPhysValMonitoring.MuonPhysValConfig import PhysValMuonCfg
        monMan.AthenaMonTools += acc.popToolsAndMerge(PhysValMuonCfg(flags))

    acc.addEventAlgo(monMan, primary = True)
    return acc
