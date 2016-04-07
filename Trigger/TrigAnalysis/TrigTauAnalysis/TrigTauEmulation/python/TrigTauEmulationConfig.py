# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration




from TrigTauEmulation.TrigTauEmulationConf import (JetRoISelectionTool, EmTauSelectionTool, 
                                                   EnergySumSelectionTool, MuonRoISelectionTool, 
                                                   HltTauSelectionTool)


# JET ROI TOOLS INSTANTIATION
JetTools = [
    JetRoISelectionTool(
        name="J12", ClusterPt=12000.),
    JetRoISelectionTool(
        name="J20", ClusterPt=20000.),
    JetRoISelectionTool(
        name="J25", ClusterPt=25000.),
]


# EMTAU TOOLS INTANTIATION
EmTauTools = [
    EmTauSelectionTool(
        name='TAU8', ClusterPt=8000., IsolationThresh=-9999.),
    EmTauSelectionTool(
        name='TAU12', ClusterPt=12000., IsolationThresh=-9999.),
    EmTauSelectionTool(
        name='TAU15', ClusterPt=15000., IsolationThresh=-9999.),
    EmTauSelectionTool(
        name='TAU20', ClusterPt=20000., IsolationThresh=-9999.),
    EmTauSelectionTool(
        name='TAU25', ClusterPt=25000., IsolationThresh=-9999.),
    EmTauSelectionTool(
        name='TAU30', ClusterPt=30000., IsolationThresh=-9999.),
    EmTauSelectionTool(
        name='TAU40', ClusterPt=40000., IsolationThresh=-9999.),
    EmTauSelectionTool(
        name='TAU60', ClusterPt=60000., IsolationThresh=-9999.),
    EmTauSelectionTool(
        name='TAU12IL', ClusterPt=12000., IsolationOffset=3000.),
    EmTauSelectionTool(
        name='TAU12IM', ClusterPt=12000., IsolationOffset=2000.),
    EmTauSelectionTool(
        name='TAU12IT', ClusterPt=12000., IsolationOffset=1500.),
    EmTauSelectionTool(
        name='TAU20IL', ClusterPt=20000., IsolationOffset=3000.),
    EmTauSelectionTool(
        name='TAU20IM', ClusterPt=20000., IsolationOffset=2000.),
    EmTauSelectionTool(
        name='TAU20IT', ClusterPt=20000., IsolationOffset=1500.),
    EmTauSelectionTool(
        name='TAU25IT', ClusterPt=25000., IsolationOffset=1500.),
    EmTauSelectionTool(
        name='EM15', ClusterPt=15000., UseEmClus=True, IsolationThresh=-9999.),
    EmTauSelectionTool(
        name='EM15HI', 
        ClusterPt=15000.,
        UseEmClus=True,
        UseHadCore=True,
        IsolationThresh=50000, 
        IsolationMinimum=2000.,
        IsolationOffset=-1800.,
        IsolationSlope=1. / 8.,
        HadronicLeakageThresh=50000.,
        HadronicLeakageMinimum=1000.,
        HadronicLeakageOffset=-200.,
        HadronicLeakageSlope=1. / 23.0),
]

# ENERGY SUM TOOLS INSTANTIATION
XeTools = [
    EnergySumSelectionTool(
        name='XE35', XE_cut=35000.),
    EnergySumSelectionTool(
        name='XE40', XE_cut=40000.),
    EnergySumSelectionTool(
        name='XE45', XE_cut=45000.),
    EnergySumSelectionTool(
        name='XE50', XE_cut=50000.),
]    

# MUON ROI TOOLS INSTANTIATION
MuonTools = [
    MuonRoISelectionTool(
        name='MU10', ClusterPt=10000.),
    MuonRoISelectionTool(
        name='MU20', ClusterPt=20000.),
]

HltTauTools = [
    HltTauSelectionTool(
        name="tau25_perf_ptonly",
        UseCaloPresel=False,
        UseFastTracking=False,
        UseTauID=False),
    HltTauSelectionTool(
        name="tau25_perf_tracktwo",
        UseCaloPresel=False,
        UseFastTracking=False,
        UseTauID=False),
    HltTauSelectionTool(
        name="tau25_medium1_tracktwo",
        UseCaloPresel=False,
        UseFastTracking=False,
        UseTauID=True, IdLevel='medium'),
    HltTauSelectionTool(
        name="tau25_medium1_ptonly",
        UseCaloPresel=False,
        UseFastTracking=False,
        UseTauID=True, IdLevel='medium'),
]


from AthenaCommon.AppMgr import ToolSvc
print ToolSvc

for tool in EmTauTools + JetTools + XeTools + MuonTools + HltTauTools:
    ToolSvc += tool

from TrigTauEmulation.TrigTauEmulationConf import TrigTauEmul__Level1EmulationTool
def get_level1_emulator(name, l1_chains):
    emulator = TrigTauEmul__Level1EmulationTool(
        name=name,
        l1_chains=l1_chains)
        #EmTauTools=EmTauTools,
        #JetTools=JetTools,
        #XeTools=XeTools,
        #MuonTools=MuonTools
    return emulator


if not hasattr(ToolSvc, 'TrigDecisionTool'):
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    ToolSvc += Trig__TrigDecisionTool('TrigDecisionTool')


from TrigTauEmulation.TrigTauEmulationConf import TrigTauEmul__HltEmulationTool
def get_hlt_emulator(name, chains, level1_emulator):
    hlt_emulator = TrigTauEmul__HltEmulationTool(
        name=name,
        hlt_chains=chains,
        Level1EmulationTool=level1_emulator,
        # EmTauTools=EmTauTools,
        # JetTools=JetTools,
        # XeTools=XeTools,
        # MuonTools=MuonTools,
        # HltTauTools=HltTauTools,
        TrigDecTool=ToolSvc.TrigDecisionTool)
    return hlt_emulator

# ToolSvc += Level1Emulator
print ToolSvc
