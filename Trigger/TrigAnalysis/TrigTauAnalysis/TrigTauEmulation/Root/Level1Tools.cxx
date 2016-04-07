/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2

#include "TrigTauEmulation/ToolsRegistry.h"

StatusCode ToolsRegistry::L1_J12() {
    auto m_l1jet_tool_12 = new JetRoISelectionTool("J12");
    m_l1jet_tools.push_back(ToolHandle<IJetRoISelectionTool>(m_l1jet_tool_12));

    ATH_CHECK(m_l1jet_tool_12->setProperty("ClusterPt", 12000.));

    addTool<JetRoISelectionTool*>(m_l1jet_tool_12);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_J20() {
    auto m_l1jet_tool_20 = new JetRoISelectionTool("J20");
    m_l1jet_tools.push_back(ToolHandle<IJetRoISelectionTool>(m_l1jet_tool_20));

    ATH_CHECK(m_l1jet_tool_20->setProperty("ClusterPt", 20000.));
    
    addTool<JetRoISelectionTool*>(m_l1jet_tool_20);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_J25() {
    auto m_l1jet_tool_25 = new JetRoISelectionTool("J25");
    m_l1jet_tools.push_back(ToolHandle<IJetRoISelectionTool>(m_l1jet_tool_25));

    ATH_CHECK(m_l1jet_tool_25->setProperty("ClusterPt", 25000.));

    addTool<JetRoISelectionTool*>(m_l1jet_tool_25);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU8() {
    auto m_l1tau_tool_8 = new EmTauSelectionTool("TAU8");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_8));

    ATH_CHECK(m_l1tau_tool_8->setProperty("ClusterPt", 8000.));
    ATH_CHECK(m_l1tau_tool_8->setProperty("IsolationThresh", -9999.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_8);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU12() {
    auto m_l1tau_tool_12 = new EmTauSelectionTool("TAU12");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_12));

    ATH_CHECK(m_l1tau_tool_12->setProperty("ClusterPt", 12000.));
    ATH_CHECK(m_l1tau_tool_12->setProperty("IsolationThresh", -9999.));

    addTool<EmTauSelectionTool*>(m_l1tau_tool_12);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU15() {
    auto m_l1tau_tool_15 = new EmTauSelectionTool("TAU15");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_15));

    ATH_CHECK(m_l1tau_tool_15->setProperty("ClusterPt", 15000.));
    ATH_CHECK(m_l1tau_tool_15->setProperty("IsolationThresh", -9999.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_15);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU20() {
    auto m_l1tau_tool_20 = new EmTauSelectionTool("TAU20");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_20));

    ATH_CHECK(m_l1tau_tool_20->setProperty("ClusterPt", 20000.));
    ATH_CHECK(m_l1tau_tool_20->setProperty("IsolationThresh", -9999.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_20);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU25() {
    auto m_l1tau_tool_25 = new EmTauSelectionTool("TAU25");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_25));

    ATH_CHECK(m_l1tau_tool_25->setProperty("ClusterPt", 25000.));
    ATH_CHECK(m_l1tau_tool_25->setProperty("IsolationThresh", -9999.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_25);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU30() {
    auto m_l1tau_tool_30 = new EmTauSelectionTool("TAU30");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_30));

    ATH_CHECK(m_l1tau_tool_30->setProperty("ClusterPt", 30000.));
    ATH_CHECK(m_l1tau_tool_30->setProperty("IsolationThresh", -9999.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_30);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU40() {
    auto m_l1tau_tool_40 = new EmTauSelectionTool("TAU40");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_40));

    ATH_CHECK(m_l1tau_tool_40->setProperty("ClusterPt", 40000.));
    ATH_CHECK(m_l1tau_tool_40->setProperty("IsolationThresh", -9999.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_40);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU60() {
    auto m_l1tau_tool_60 = new EmTauSelectionTool("TAU60");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_60));

    ATH_CHECK(m_l1tau_tool_60->setProperty("ClusterPt", 60000.));
    ATH_CHECK(m_l1tau_tool_60->setProperty("IsolationThresh", -9999.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_60);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU12IL() {
    auto m_l1tau_tool_12IL = new EmTauSelectionTool("TAU12IL");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_12IL));

    ATH_CHECK(m_l1tau_tool_12IL->setProperty("ClusterPt", 12000.));
    ATH_CHECK(m_l1tau_tool_12IL->setProperty("IsolationOffset", 3000.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_12IL);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU12IM() {
    auto m_l1tau_tool_12IM = new EmTauSelectionTool("TAU12IM");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_12IM));

    ATH_CHECK(m_l1tau_tool_12IM->setProperty("ClusterPt", 12000.));
    ATH_CHECK(m_l1tau_tool_12IM->setProperty("IsolationOffset", 2000.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_12IM);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU12IT() {
    auto m_l1tau_tool_12IT = new EmTauSelectionTool("TAU12IT");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_12IT));

    ATH_CHECK(m_l1tau_tool_12IT->setProperty("ClusterPt", 12000.));
    ATH_CHECK(m_l1tau_tool_12IT->setProperty("IsolationOffset", 1500.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_12IT);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU20IL() {
    auto m_l1tau_tool_20IL = new EmTauSelectionTool("TAU20IL");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_20IL));

    ATH_CHECK(m_l1tau_tool_20IL->setProperty("ClusterPt", 20000.));
    ATH_CHECK(m_l1tau_tool_20IL->setProperty("IsolationOffset", 3000.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_20IL);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU20IM() {
    auto m_l1tau_tool_20IM = new EmTauSelectionTool("TAU20IM");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_20IM));

    ATH_CHECK(m_l1tau_tool_20IM->setProperty("ClusterPt", 20000.));
    ATH_CHECK(m_l1tau_tool_20IM->setProperty("IsolationOffset", 2000.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_20IM);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU20IT() {
    auto m_l1tau_tool_20IT = new EmTauSelectionTool("TAU20IT");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_20IT));

    ATH_CHECK(m_l1tau_tool_20IT->setProperty("ClusterPt", 20000.));
    ATH_CHECK(m_l1tau_tool_20IT->setProperty("IsolationOffset", 1500.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_20IT);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_TAU25IT() {
    auto m_l1tau_tool_25IT = new EmTauSelectionTool("TAU25IT");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1tau_tool_25IT));

    ATH_CHECK(m_l1tau_tool_25IT->setProperty("ClusterPt", 25000.));
    ATH_CHECK(m_l1tau_tool_25IT->setProperty("IsolationOffset", 1500.));
    
    addTool<EmTauSelectionTool*>(m_l1tau_tool_25IT);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_EM15() {
    auto m_l1ele_tool_15 = new EmTauSelectionTool("EM15");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1ele_tool_15));

    ATH_CHECK(m_l1ele_tool_15->setProperty("ClusterPt", 15000.));
    ATH_CHECK(m_l1ele_tool_15->setProperty("UseEmClus", true));
    ATH_CHECK(m_l1ele_tool_15->setProperty("IsolationThresh", -9999.));
    
    addTool<EmTauSelectionTool*>(m_l1ele_tool_15);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_EM15HI() {
    auto m_l1ele_tool_15HI = new EmTauSelectionTool("EM15HI");
    m_l1tau_tools.push_back(ToolHandle<IEmTauSelectionTool>(m_l1ele_tool_15HI));

    ATH_CHECK(m_l1ele_tool_15HI->setProperty("ClusterPt", 15000.));
    ATH_CHECK(m_l1ele_tool_15HI->setProperty("UseEmClus", true));
    ATH_CHECK(m_l1ele_tool_15HI->setProperty("UseHadCore", true));
    ATH_CHECK(m_l1ele_tool_15HI->setProperty("IsolationThresh", 50000.));
    ATH_CHECK(m_l1ele_tool_15HI->setProperty("IsolationMinimum", 2000.));
    ATH_CHECK(m_l1ele_tool_15HI->setProperty("IsolationOffset", -1800.));
    ATH_CHECK(m_l1ele_tool_15HI->setProperty("IsolationSlope", 1./8.));
    ATH_CHECK(m_l1ele_tool_15HI->setProperty("HadronicLeakageThresh", 50000.));
    ATH_CHECK(m_l1ele_tool_15HI->setProperty("HadronicLeakageMinimum", 1000.));
    ATH_CHECK(m_l1ele_tool_15HI->setProperty("HadronicLeakageOffset", -200.));
    ATH_CHECK(m_l1ele_tool_15HI->setProperty("HadronicLeakageSlope", 1./23.0));
    
    addTool<EmTauSelectionTool*>(m_l1ele_tool_15HI);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_XE35() {
    auto m_l1xe_tool_35 = new EnergySumSelectionTool("XE35");
    m_l1xe_tools.push_back(ToolHandle<IEnergySumSelectionTool>(m_l1xe_tool_35));

    ATH_CHECK(m_l1xe_tool_35->setProperty("XE_cut", 35000.));
    
    addTool<EnergySumSelectionTool*>(m_l1xe_tool_35);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_XE40() {
    auto m_l1xe_tool_40 = new EnergySumSelectionTool("XE40");
    m_l1xe_tools.push_back(ToolHandle<IEnergySumSelectionTool>(m_l1xe_tool_40));

    ATH_CHECK(m_l1xe_tool_40->setProperty("XE_cut", 40000.));
    
    addTool<EnergySumSelectionTool*>(m_l1xe_tool_40);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_XE45() {
    auto m_l1xe_tool_45 = new EnergySumSelectionTool("XE45");
    m_l1xe_tools.push_back(ToolHandle<IEnergySumSelectionTool>(m_l1xe_tool_45));

    ATH_CHECK(m_l1xe_tool_45->setProperty("XE_cut", 45000.));
    
    addTool<EnergySumSelectionTool*>(m_l1xe_tool_45);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_XE50() {
    auto m_l1xe_tool_50 = new EnergySumSelectionTool("XE50");
    m_l1xe_tools.push_back(ToolHandle<IEnergySumSelectionTool>(m_l1xe_tool_50));

    ATH_CHECK(m_l1xe_tool_50->setProperty("XE_cut", 50000.));
    
    addTool<EnergySumSelectionTool*>(m_l1xe_tool_50);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_MU10() {
    auto m_l1muon_tool_10 = new MuonRoISelectionTool("MU10");
    m_l1muon_tools.push_back(ToolHandle<IMuonRoISelectionTool>(m_l1muon_tool_10));

    ATH_CHECK(m_l1muon_tool_10->setProperty("ClusterPt", 10000.));
    
    addTool<MuonRoISelectionTool*>(m_l1muon_tool_10);

    return StatusCode::SUCCESS;
}

StatusCode ToolsRegistry::L1_MU20() {
    auto m_l1muon_tool_20 = new MuonRoISelectionTool("MU20");
    m_l1muon_tools.push_back(ToolHandle<IMuonRoISelectionTool>(m_l1muon_tool_20));

    ATH_CHECK(m_l1muon_tool_20->setProperty("ClusterPt", 20000.));
    
    addTool<MuonRoISelectionTool*>(m_l1muon_tool_20);

    return StatusCode::SUCCESS;
}
