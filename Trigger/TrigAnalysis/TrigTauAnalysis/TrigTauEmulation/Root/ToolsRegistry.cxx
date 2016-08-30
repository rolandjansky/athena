/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2

#include <functional>
#include "TrigTauEmulation/ToolsRegistry.h"
#include "TrigTauEmulation/MsgStream.h"

StatusCode ToolsRegistry::initializeTool(const std::string &name) {
  if(m_initializedToolNames.find(name) != m_initializedToolNames.end()) {
    MY_MSG_DEBUG("Tool " << name << " already initialized");
    return StatusCode::SUCCESS;
  }

  auto it = m_initializeFunctions.find(name);
  if(it == m_initializeFunctions.end()) {
    std::stringstream e;
    e << "Chain " << name << " not known in ToolsRegistry";
    throw std::invalid_argument(e.str());
  }

  auto func = it->second;
  //ATH_CHECK( (this->*func)() );
  ATH_CHECK(func());
  m_initializedToolNames.insert(name);

  MY_MSG_DEBUG("Tool " << name << " initialized");
  return StatusCode::SUCCESS;
}

ToolsRegistry::ToolsRegistry(const std::string & name)
  : asg::AsgTool(name),
    m_l1tau_tools(),
    m_l1xe_tools(),
    m_l1jet_tools(),
    m_l1muon_tools()
{

  declareProperty("RecalculateBDTscore", m_recalculateBDTscore=false, "Recalculate tau BDT scores");

  // FTF TOOL
  m_ftf_tool = new FastTrackSelectionTool("FastTrackSelector");

  m_initializeFunctions["J12"]     = std::bind(&ToolsRegistry::L1_J12, this);
  m_initializeFunctions["J20"]     = std::bind(&ToolsRegistry::L1_J20, this);
  m_initializeFunctions["J25"]     = std::bind(&ToolsRegistry::L1_J25, this);

  m_initializeFunctions["TAU8"]    = std::bind(&ToolsRegistry::L1_TAU8, this);
  m_initializeFunctions["TAU12"]   = std::bind(&ToolsRegistry::L1_TAU12, this);
  m_initializeFunctions["TAU15"]   = std::bind(&ToolsRegistry::L1_TAU15, this);
  m_initializeFunctions["TAU20"]   = std::bind(&ToolsRegistry::L1_TAU20, this);
  m_initializeFunctions["TAU25"]   = std::bind(&ToolsRegistry::L1_TAU25, this);
  m_initializeFunctions["TAU30"]   = std::bind(&ToolsRegistry::L1_TAU30, this);
  m_initializeFunctions["TAU40"]   = std::bind(&ToolsRegistry::L1_TAU40, this);
  m_initializeFunctions["TAU60"]   = std::bind(&ToolsRegistry::L1_TAU60, this);
  
  m_initializeFunctions["TAU12IL"] = std::bind(&ToolsRegistry::L1_TAU12IL, this);
  m_initializeFunctions["TAU12IM"] = std::bind(&ToolsRegistry::L1_TAU12IM, this);
  m_initializeFunctions["TAU12IT"] = std::bind(&ToolsRegistry::L1_TAU12IT, this);
  m_initializeFunctions["TAU20IL"] = std::bind(&ToolsRegistry::L1_TAU20IL, this);
  m_initializeFunctions["TAU20IM"] = std::bind(&ToolsRegistry::L1_TAU20IM, this);
  m_initializeFunctions["TAU20IT"] = std::bind(&ToolsRegistry::L1_TAU20IT, this);
  m_initializeFunctions["TAU25IT"] = std::bind(&ToolsRegistry::L1_TAU25IT, this);
  
  m_initializeFunctions["EM15"]    = std::bind(&ToolsRegistry::L1_EM15, this);
  m_initializeFunctions["EM15HI"]  = std::bind(&ToolsRegistry::L1_EM15HI, this);
  
  m_initializeFunctions["XE35"]    = std::bind(&ToolsRegistry::L1_XE35, this);
  m_initializeFunctions["XE40"]    = std::bind(&ToolsRegistry::L1_XE40, this);
  m_initializeFunctions["XE45"]    = std::bind(&ToolsRegistry::L1_XE45, this);
  m_initializeFunctions["XE50"]    = std::bind(&ToolsRegistry::L1_XE50, this);
  m_initializeFunctions["MU10"]    = std::bind(&ToolsRegistry::L1_MU10, this);
  m_initializeFunctions["MU20"]    = std::bind(&ToolsRegistry::L1_MU20, this);

  m_initializeFunctions["tau0_perf_ptonly"]        = std::bind(&ToolsRegistry::HLT_tau0_perf_ptonly, this);
  m_initializeFunctions["tau5_perf_ptonly"]        = std::bind(&ToolsRegistry::HLT_tau5_perf_ptonly, this);
  
  m_initializeFunctions["tau25_perf_ptonly"]       = std::bind(&ToolsRegistry::HLT_tau25_perf_ptonly, this);
  m_initializeFunctions["tau25_perf_calo"]         = std::bind(&ToolsRegistry::HLT_tau25_perf_calo, this);
  m_initializeFunctions["tau25_perf_tracktwo"]     = std::bind(&ToolsRegistry::HLT_tau25_perf_tracktwo, this);
  m_initializeFunctions["tau25_idperf_tracktwo"]   = std::bind(&ToolsRegistry::HLT_tau25_idperf_tracktwo, this);
  m_initializeFunctions["tau25_loose1_ptonly"]     = std::bind(&ToolsRegistry::HLT_tau25_loose1_ptonly, this);
  m_initializeFunctions["tau25_loose1_calo"]       = std::bind(&ToolsRegistry::HLT_tau25_loose1_calo, this);
  m_initializeFunctions["tau25_loose1_tracktwo"]   = std::bind(&ToolsRegistry::HLT_tau25_loose1_tracktwo, this);
  m_initializeFunctions["tau25_medium1_ptonly"]    = std::bind(&ToolsRegistry::HLT_tau25_medium1_ptonly, this);
  m_initializeFunctions["tau25_medium1_calo"]      = std::bind(&ToolsRegistry::HLT_tau25_medium1_calo, this);
  m_initializeFunctions["tau25_medium1_tracktwo"]  = std::bind(&ToolsRegistry::HLT_tau25_medium1_tracktwo, this);
  m_initializeFunctions["tau25_medium1_mvonly"]    = std::bind(&ToolsRegistry::HLT_tau25_medium1_mvonly, this);
  m_initializeFunctions["tau25_tight1_ptonly"]     = std::bind(&ToolsRegistry::HLT_tau25_tight1_ptonly, this);
  m_initializeFunctions["tau25_tight1_calo"]       = std::bind(&ToolsRegistry::HLT_tau25_tight1_calo, this);
  m_initializeFunctions["tau25_tight1_tracktwo"]   = std::bind(&ToolsRegistry::HLT_tau25_tight1_tracktwo, this);
  
  m_initializeFunctions["tau35_loose1_tracktwo"]   = std::bind(&ToolsRegistry::HLT_tau35_loose1_tracktwo, this);
  m_initializeFunctions["tau35_loose1_ptonly"]     = std::bind(&ToolsRegistry::HLT_tau35_loose1_ptonly, this);
  m_initializeFunctions["tau35_medium1_tracktwo"]  = std::bind(&ToolsRegistry::HLT_tau35_medium1_tracktwo, this);
  m_initializeFunctions["tau35_medium1_ptonly"]    = std::bind(&ToolsRegistry::HLT_tau35_medium1_ptonly, this);
  m_initializeFunctions["tau35_medium1_calo"]      = std::bind(&ToolsRegistry::HLT_tau35_medium1_calo, this);
  m_initializeFunctions["tau35_tight1_tracktwo"]   = std::bind(&ToolsRegistry::HLT_tau35_tight1_tracktwo, this);
  m_initializeFunctions["tau35_tight1_ptonly"]     = std::bind(&ToolsRegistry::HLT_tau35_tight1_ptonly, this);
  m_initializeFunctions["tau35_perf_tracktwo"]     = std::bind(&ToolsRegistry::HLT_tau35_perf_tracktwo, this);
  m_initializeFunctions["tau35_perf_ptonly"]       = std::bind(&ToolsRegistry::HLT_tau35_perf_ptonly, this);
  
  m_initializeFunctions["tau50_medium1_tracktwo"]  = std::bind(&ToolsRegistry::HLT_tau50_medium1_tracktwo, this);
  
  m_initializeFunctions["tau80_medium1_calo"]      = std::bind(&ToolsRegistry::HLT_tau80_medium1_calo, this);
  m_initializeFunctions["tau80_medium1_tracktwo"]  = std::bind(&ToolsRegistry::HLT_tau80_medium1_tracktwo, this);
  
  m_initializeFunctions["tau125_medium1_tracktwo"] = std::bind(&ToolsRegistry::HLT_tau125_medium1_tracktwo, this);
  m_initializeFunctions["tau125_medium1_calo"]     = std::bind(&ToolsRegistry::HLT_tau125_medium1_calo, this);
  m_initializeFunctions["tau125_perf_tracktwo"]    = std::bind(&ToolsRegistry::HLT_tau125_perf_tracktwo, this);
  m_initializeFunctions["tau125_perf_ptonly"]      = std::bind(&ToolsRegistry::HLT_tau125_perf_ptonly, this);
  
  m_initializeFunctions["tau160_medium1_tracktwo"] = std::bind(&ToolsRegistry::HLT_tau160_medium1_tracktwo, this);

}

ToolsRegistry::~ToolsRegistry(){
  ExtensionStack exts = clearExtensions(); 
  while (auto ext = exts.pop_back()) { 
    delete ext;
  }
}

StatusCode ToolsRegistry::initialize()
{
  m_ftf_tool->msg().setLevel(this->msg().level());
  ATH_CHECK(m_ftf_tool->initialize());

  return StatusCode::SUCCESS;
}
