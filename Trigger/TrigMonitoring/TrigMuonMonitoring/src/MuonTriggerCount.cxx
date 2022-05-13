/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTriggerCount.h"


MuonTriggerCount :: MuonTriggerCount(const std::string& name, ISvcLocator* pSvcLocator )
  : TrigMuonMonitorAlgorithm(name, pSvcLocator)
{}


StatusCode MuonTriggerCount :: fillVariables(const EventContext&) const {

  ATH_MSG_DEBUG ("Filling histograms for " << name() << "...");

  std::vector<std::string> passed_chains;
  for ( const auto& c: m_monitored_chains ) { if (getTrigDecisionTool()->isPassed(c)) passed_chains.push_back(c); }
  auto passed = Monitored::Collection("TriggerCount", passed_chains );

  fill(m_group, passed);

  return StatusCode::SUCCESS;
}
