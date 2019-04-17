# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
include ("L1TopoSimulation/L1Simulation_topOptions.py")

def enableL1TopoSimulation():

    algseq =  CfgMgr.AthSequencer("AthAlgSeq")
    algseq += L1TopoSimulation()
    algseq.L1TopoSimulation.JetInputProvider="LVL1::JetInputProviderFEX/JetInputProviderFEX"         
    algseq.L1TopoSimulation.EMTAUInputProvider="LVL1::EMTauInputProviderFEX/EMTauInputProviderFEX"   
    algseq.L1TopoSimulation.EnergyInputProvider="LVL1::EnergyInputProviderFEX/EnergyInputProviderFEX"

    from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += L1MuctpiTool()
    algseq.L1TopoSimulation.MuonInputProvider.MuctpiSimTool = L1MuctpiTool()
    
    # enable the reduced (coarse) granularity topo simulation
    # currently only for MC
    from AthenaCommon.GlobalFlags  import globalflags
    if globalflags.DataSource()!='data':
        #add logger
        algseq.L1TopoSimulation.MuonInputProvider.MuonEncoding = 1
    else:
        #add logger
        algseq.L1TopoSimulation.MuonInputProvider.MuonEncoding = 1
