/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2

#include "TrigTauEmulation/ToolsRegistry.h"

StatusCode ToolsRegistry::HLT_tau25_perf_ptonly() {
  auto m_hlttau_tool_25_perf_ptonly = new HltTauSelectionTool("tau25_perf_ptonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_perf_ptonly));

  ATH_CHECK(m_hlttau_tool_25_perf_ptonly->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_perf_ptonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_25_perf_ptonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_25_perf_ptonly->setProperty("UseTauID", false));
  ATH_CHECK(m_hlttau_tool_25_perf_ptonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
    
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_perf_ptonly);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_perf_calo() {
  auto m_hlttau_tool_25_perf_calo = new HltTauSelectionTool("tau25_perf_calo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_perf_calo));

  ATH_CHECK(m_hlttau_tool_25_perf_calo->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_perf_calo->setProperty("CentFracStrategy", "pt_dependent"));
  ATH_CHECK(m_hlttau_tool_25_perf_calo->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_25_perf_calo->setProperty("UseTauID", false));
  ATH_CHECK(m_hlttau_tool_25_perf_calo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_perf_calo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_perf_tracktwo() {
  auto m_hlttau_tool_25_perf_tracktwo = new HltTauSelectionTool("tau25_perf_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_perf_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_25_perf_tracktwo->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_perf_tracktwo->setProperty("UsePresel", false));
  ATH_CHECK(m_hlttau_tool_25_perf_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_25_perf_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_25_perf_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_25_perf_tracktwo->setProperty("UseTauID", false));
  ATH_CHECK(m_hlttau_tool_25_perf_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_perf_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_idperf_tracktwo() {
  auto m_hlttau_tool_25_idperf_tracktwo = new HltTauSelectionTool("tau25_idperf_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_idperf_tracktwo));

  ATH_CHECK(m_hlttau_tool_25_idperf_tracktwo->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_idperf_tracktwo->setProperty("UsePresel", false));
  ATH_CHECK(m_hlttau_tool_25_idperf_tracktwo->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_25_idperf_tracktwo->setProperty("UseTauID", false));
  ATH_CHECK(m_hlttau_tool_25_idperf_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_idperf_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_loose1_ptonly() {
  auto m_hlttau_tool_25_loose1_ptonly = new HltTauSelectionTool("tau25_loose1_ptonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_loose1_ptonly));

  ATH_CHECK(m_hlttau_tool_25_loose1_ptonly->setProperty("PreselPt", 25000.));
  // ATH_CHECK(m_hlttau_tool_25_loose1_ptonly->setProperty("UsePresel", false));
  ATH_CHECK(m_hlttau_tool_25_loose1_ptonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_25_loose1_ptonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_25_loose1_ptonly->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_25_loose1_ptonly->setProperty("IdLevel", "loose"));
  ATH_CHECK(m_hlttau_tool_25_loose1_ptonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_loose1_ptonly);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_loose1_calo() {
  auto m_hlttau_tool_25_loose1_calo = new HltTauSelectionTool("tau25_loose1_calo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_loose1_calo));

  ATH_CHECK(m_hlttau_tool_25_loose1_calo->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_loose1_calo->setProperty("CentFracStrategy", "pt_dependent"));
  ATH_CHECK(m_hlttau_tool_25_loose1_calo->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_25_loose1_calo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_25_loose1_calo->setProperty("IdLevel", "loose"));
  ATH_CHECK(m_hlttau_tool_25_loose1_calo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_loose1_calo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_loose1_tracktwo() {
  auto m_hlttau_tool_25_loose1_tracktwo = new HltTauSelectionTool("tau25_loose1_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_loose1_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_25_loose1_tracktwo->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_loose1_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_25_loose1_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_25_loose1_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_25_loose1_tracktwo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_25_loose1_tracktwo->setProperty("IdLevel", "loose"));
  ATH_CHECK(m_hlttau_tool_25_loose1_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_loose1_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_medium1_ptonly() {
  auto m_hlttau_tool_25_medium1_ptonly = new HltTauSelectionTool("tau25_medium1_ptonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_medium1_ptonly));

  ATH_CHECK(m_hlttau_tool_25_medium1_ptonly->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_medium1_ptonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_25_medium1_ptonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_25_medium1_ptonly->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_25_medium1_ptonly->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_25_medium1_ptonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_medium1_ptonly);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_medium1_calo() {
  auto m_hlttau_tool_25_medium1_calo = new HltTauSelectionTool("tau25_medium1_calo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_medium1_calo));

  ATH_CHECK(m_hlttau_tool_25_medium1_calo->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_medium1_calo->setProperty("CentFracStrategy", "pt_dependent"));
  ATH_CHECK(m_hlttau_tool_25_medium1_calo->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_25_medium1_calo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_25_medium1_calo->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_25_medium1_calo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_medium1_calo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_medium1_tracktwo() {
  auto m_hlttau_tool_25_medium1_tracktwo = new HltTauSelectionTool("tau25_medium1_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_medium1_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_25_medium1_tracktwo->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_medium1_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_25_medium1_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_25_medium1_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_25_medium1_tracktwo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_25_medium1_tracktwo->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_25_medium1_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  ATH_CHECK(m_hlttau_tool_25_medium1_tracktwo->setProperty("IdLevel", "medium"));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_medium1_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_medium1_mvonly() {
  auto m_hlttau_tool_25_medium1_mvonly = new HltTauSelectionTool("tau25_medium1_mvonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_medium1_mvonly));

  ATH_CHECK(m_hlttau_tool_25_medium1_mvonly->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_medium1_mvonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_25_medium1_mvonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_25_medium1_mvonly->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_25_medium1_mvonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_medium1_mvonly);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_tight1_ptonly() {
  auto m_hlttau_tool_25_tight1_ptonly = new HltTauSelectionTool("tau25_tight1_ptonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_tight1_ptonly));

  ATH_CHECK(m_hlttau_tool_25_tight1_ptonly->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_tight1_ptonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_25_tight1_ptonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_25_tight1_ptonly->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_25_tight1_ptonly->setProperty("IdLevel", "tight"));
  ATH_CHECK(m_hlttau_tool_25_tight1_ptonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_tight1_ptonly);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_tight1_calo() {
  auto m_hlttau_tool_25_tight1_calo = new HltTauSelectionTool("tau25_tight1_calo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_tight1_calo));

  ATH_CHECK(m_hlttau_tool_25_tight1_calo->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_tight1_calo->setProperty("CentFracStrategy", "pt_dependent"));
  ATH_CHECK(m_hlttau_tool_25_tight1_calo->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_25_tight1_calo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_25_tight1_calo->setProperty("IdLevel", "tight"));
  ATH_CHECK(m_hlttau_tool_25_tight1_calo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_tight1_calo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau25_tight1_tracktwo() {
  auto m_hlttau_tool_25_tight1_tracktwo = new HltTauSelectionTool("tau25_tight1_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_25_tight1_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_25_tight1_tracktwo->setProperty("PreselPt", 25000.));
  ATH_CHECK(m_hlttau_tool_25_tight1_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_25_tight1_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_25_tight1_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_25_tight1_tracktwo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_25_tight1_tracktwo->setProperty("IdLevel", "tight"));
  ATH_CHECK(m_hlttau_tool_25_tight1_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_25_tight1_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau35_loose1_tracktwo() {
  auto m_hlttau_tool_35_loose1_tracktwo = new HltTauSelectionTool("tau35_loose1_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_35_loose1_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_35_loose1_tracktwo->setProperty("PreselPt", 35000.));
  ATH_CHECK(m_hlttau_tool_35_loose1_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_35_loose1_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_35_loose1_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_35_loose1_tracktwo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_35_loose1_tracktwo->setProperty("IdLevel", "loose"));
  ATH_CHECK(m_hlttau_tool_35_loose1_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_35_loose1_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau35_loose1_ptonly() {
  auto m_hlttau_tool_35_loose1_ptonly = new HltTauSelectionTool("tau35_loose1_ptonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_35_loose1_ptonly));

  ATH_CHECK(m_hlttau_tool_35_loose1_ptonly->setProperty("PreselPt", 35000.));
  ATH_CHECK(m_hlttau_tool_35_loose1_ptonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_35_loose1_ptonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_35_loose1_ptonly->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_35_loose1_ptonly->setProperty("IdLevel", "loose"));
  ATH_CHECK(m_hlttau_tool_35_loose1_ptonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_35_loose1_ptonly);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau35_medium1_tracktwo() {
  auto m_hlttau_tool_35_medium1_tracktwo = new HltTauSelectionTool("tau35_medium1_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_35_medium1_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_35_medium1_tracktwo->setProperty("PreselPt", 35000.));
  ATH_CHECK(m_hlttau_tool_35_medium1_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_35_medium1_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_35_medium1_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_35_medium1_tracktwo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_35_medium1_tracktwo->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_35_medium1_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_35_medium1_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau35_medium1_ptonly() {
  auto m_hlttau_tool_35_medium1_ptonly = new HltTauSelectionTool("tau35_medium1_ptonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_35_medium1_ptonly));

  ATH_CHECK(m_hlttau_tool_35_medium1_ptonly->setProperty("PreselPt", 35000.));
  ATH_CHECK(m_hlttau_tool_35_medium1_ptonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_35_medium1_ptonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_35_medium1_ptonly->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_35_medium1_ptonly->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_35_medium1_ptonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_35_medium1_ptonly);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau35_medium1_calo() {
  auto m_hlttau_tool_35_medium1_calo = new HltTauSelectionTool("tau35_medium1_calo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_35_medium1_calo));

  ATH_CHECK(m_hlttau_tool_35_medium1_calo->setProperty("PreselPt", 35000.));
  ATH_CHECK(m_hlttau_tool_35_medium1_calo->setProperty("UseCaloPresel", true));
  ATH_CHECK(m_hlttau_tool_35_medium1_calo->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_35_medium1_calo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_35_medium1_calo->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_35_medium1_calo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_35_medium1_calo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau35_tight1_tracktwo() {
  auto m_hlttau_tool_35_tight1_tracktwo = new HltTauSelectionTool("tau35_tight1_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_35_tight1_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_35_tight1_tracktwo->setProperty("PreselPt", 35000.));
  ATH_CHECK(m_hlttau_tool_35_tight1_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_35_tight1_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_35_tight1_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_35_tight1_tracktwo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_35_tight1_tracktwo->setProperty("IdLevel", "tight"));
  ATH_CHECK(m_hlttau_tool_35_tight1_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_35_tight1_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau35_tight1_ptonly() {
  auto m_hlttau_tool_35_tight1_ptonly = new HltTauSelectionTool("tau35_tight1_ptonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_35_tight1_ptonly));

  ATH_CHECK(m_hlttau_tool_35_tight1_ptonly->setProperty("PreselPt", 35000.));
  ATH_CHECK(m_hlttau_tool_35_tight1_ptonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_35_tight1_ptonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_35_tight1_ptonly->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_35_tight1_ptonly->setProperty("IdLevel", "tight"));
  ATH_CHECK(m_hlttau_tool_35_tight1_ptonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_35_tight1_ptonly);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau35_perf_tracktwo() {
  auto m_hlttau_tool_35_perf_tracktwo = new HltTauSelectionTool("tau35_perf_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_35_perf_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_35_perf_tracktwo->setProperty("PreselPt", 35000.));
  ATH_CHECK(m_hlttau_tool_35_perf_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_35_perf_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_35_perf_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_35_perf_tracktwo->setProperty("UseTauID", false));
  ATH_CHECK(m_hlttau_tool_35_perf_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_35_perf_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau35_perf_ptonly() {
  auto m_hlttau_tool_35_perf_ptonly = new HltTauSelectionTool("tau35_perf_ptonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_35_perf_ptonly));

  ATH_CHECK(m_hlttau_tool_35_perf_ptonly->setProperty("PreselPt", 35000.));
  ATH_CHECK(m_hlttau_tool_35_perf_ptonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_35_perf_ptonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_35_perf_ptonly->setProperty("UseTauID", false));
  ATH_CHECK(m_hlttau_tool_35_perf_ptonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_35_perf_ptonly);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau80_medium1_calo() {
  auto m_hlttau_tool_80_medium1_calo = new HltTauSelectionTool("tau80_medium1_calo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_80_medium1_calo));

  ATH_CHECK(m_hlttau_tool_80_medium1_calo->setProperty("PreselPt", 80000.));
  ATH_CHECK(m_hlttau_tool_80_medium1_calo->setProperty("UseCaloPresel", true));
  ATH_CHECK(m_hlttau_tool_80_medium1_calo->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_80_medium1_calo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_80_medium1_calo->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_80_medium1_calo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_80_medium1_calo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau80_medium1_tracktwo() {
  auto m_hlttau_tool_80_medium1_tracktwo = new HltTauSelectionTool("tau80_medium1_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_80_medium1_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_80_medium1_tracktwo->setProperty("PreselPt", 80000.));
  ATH_CHECK(m_hlttau_tool_80_medium1_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_80_medium1_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_80_medium1_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_80_medium1_tracktwo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_80_medium1_tracktwo->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_80_medium1_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_80_medium1_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau50_medium1_tracktwo() {
  auto m_hlttau_tool_50_medium1_tracktwo = new HltTauSelectionTool("tau50_medium1_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_50_medium1_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_50_medium1_tracktwo->setProperty("PreselPt", 50000.));
  ATH_CHECK(m_hlttau_tool_50_medium1_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_50_medium1_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_50_medium1_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_50_medium1_tracktwo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_50_medium1_tracktwo->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_50_medium1_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_50_medium1_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau125_medium1_tracktwo() {
  auto m_hlttau_tool_125_medium1_tracktwo = new HltTauSelectionTool("tau125_medium1_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_125_medium1_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_125_medium1_tracktwo->setProperty("PreselPt", 125000.));
  ATH_CHECK(m_hlttau_tool_125_medium1_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_125_medium1_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_125_medium1_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_125_medium1_tracktwo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_125_medium1_tracktwo->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_125_medium1_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_125_medium1_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau125_medium1_calo() {
  auto m_hlttau_tool_125_medium1_calo = new HltTauSelectionTool("tau125_medium1_calo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_125_medium1_calo));

  ATH_CHECK(m_hlttau_tool_125_medium1_calo->setProperty("PreselPt", 125000.));
  ATH_CHECK(m_hlttau_tool_125_medium1_calo->setProperty("UseCaloPresel", true));
  ATH_CHECK(m_hlttau_tool_125_medium1_calo->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_125_medium1_calo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_125_medium1_calo->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_125_medium1_calo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_125_medium1_calo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau125_perf_tracktwo() {
  auto m_hlttau_tool_125_perf_tracktwo = new HltTauSelectionTool("tau125_perf_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_125_perf_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_125_perf_tracktwo->setProperty("PreselPt", 125000.));
  ATH_CHECK(m_hlttau_tool_125_perf_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_125_perf_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_125_perf_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  ATH_CHECK(m_hlttau_tool_125_perf_tracktwo->setProperty("UseTauID", false));
  ATH_CHECK(m_hlttau_tool_125_perf_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_125_perf_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau125_perf_ptonly() {
  auto m_hlttau_tool_125_perf_ptonly = new HltTauSelectionTool("tau125_perf_ptonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_125_perf_ptonly));

  ATH_CHECK(m_hlttau_tool_125_perf_ptonly->setProperty("PreselPt", 125000.));
  ATH_CHECK(m_hlttau_tool_125_perf_ptonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_125_perf_ptonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_125_perf_ptonly->setProperty("UseTauID", false));
  ATH_CHECK(m_hlttau_tool_125_perf_ptonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_125_perf_ptonly);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau160_medium1_tracktwo() {
  auto m_hlttau_tool_160_medium1_tracktwo = new HltTauSelectionTool("tau160_medium1_tracktwo");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_160_medium1_tracktwo));

  ToolHandle<IFastTrackSelectionTool> ftf_handle(m_ftf_tool);
  ATH_CHECK(m_hlttau_tool_160_medium1_tracktwo->setProperty("PreselPt", 160000.));
  ATH_CHECK(m_hlttau_tool_160_medium1_tracktwo->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_160_medium1_tracktwo->setProperty("UseFastTracking", true));
  ATH_CHECK(m_hlttau_tool_160_medium1_tracktwo->setProperty("UseTauID", true));
  ATH_CHECK(m_hlttau_tool_160_medium1_tracktwo->setProperty("IdLevel", "medium"));
  ATH_CHECK(m_hlttau_tool_160_medium1_tracktwo->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  ATH_CHECK(m_hlttau_tool_160_medium1_tracktwo->setProperty("FastTrackSelectionTool", ftf_handle));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_160_medium1_tracktwo);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau5_perf_ptonly() {
  auto m_hlttau_tool_5_perf_ptonly = new HltTauSelectionTool("tau5_perf_ptonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_5_perf_ptonly));

  ATH_CHECK(m_hlttau_tool_5_perf_ptonly->setProperty("PreselPt", 5000.));
  ATH_CHECK(m_hlttau_tool_5_perf_ptonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_5_perf_ptonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_5_perf_ptonly->setProperty("UseTauID", false));
  ATH_CHECK(m_hlttau_tool_5_perf_ptonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_5_perf_ptonly);

  return StatusCode::SUCCESS;
}


StatusCode ToolsRegistry::HLT_tau0_perf_ptonly() {
  auto m_hlttau_tool_0_perf_ptonly = new HltTauSelectionTool("tau0_perf_ptonly");
  //m_hlttau_tools.push_back(ToolHandle<IHltTauSelectionTool>(m_hlttau_tool_0_perf_ptonly));

  ATH_CHECK(m_hlttau_tool_0_perf_ptonly->setProperty("PreselPt", 0.));
  ATH_CHECK(m_hlttau_tool_0_perf_ptonly->setProperty("UseCaloPresel", false));
  ATH_CHECK(m_hlttau_tool_0_perf_ptonly->setProperty("UseFastTracking", false));
  ATH_CHECK(m_hlttau_tool_0_perf_ptonly->setProperty("UseTauID", false));
  ATH_CHECK(m_hlttau_tool_0_perf_ptonly->setProperty("RecalculateBDTscore", m_recalculateBDTscore));
  
  addTool<HltTauSelectionTool*>(m_hlttau_tool_0_perf_ptonly);

  return StatusCode::SUCCESS;
}
