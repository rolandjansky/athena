/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigAFPSidHypoMonitoringAlg.h"

TrigAFPSidHypoMonitoringAlg::TrigAFPSidHypoMonitoringAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthMonitorAlgorithm (name, pSvcLocator)
{
}

TrigAFPSidHypoMonitoringAlg::~TrigAFPSidHypoMonitoringAlg()
{
}

StatusCode TrigAFPSidHypoMonitoringAlg::initialize()
{
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  return StatusCode::SUCCESS;
}

StatusCode TrigAFPSidHypoMonitoringAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TrigAFPSidHypoMonitoringAlg::fillHistograms(const EventContext& /*context*/) const
{
  // in future should be split into many smaller methods called from here
  std::vector<std::string> passedAFPChains = {"all"}; // also includes ALL events counter
  for (auto chainName: m_chains) {
    if ( getTrigDecisionTool()->isPassed(chainName) )
      passedAFPChains.emplace_back(chainName);
  }
  if ( passedAFPChains.size() > 1) passedAFPChains.emplace_back("AFP");
  auto whichTrigger = Monitored::Collection("TrigCounts", passedAFPChains);
  fill("AFPCoarse", whichTrigger);

  return StatusCode::SUCCESS;
}

