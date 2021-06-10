/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTEfficiencyMonitoringAlg.h"

HLTEfficiencyMonitoringAlg::HLTEfficiencyMonitoringAlg(const std::string &name, ISvcLocator *pSvcLocator) : AthMonitorAlgorithm(name, pSvcLocator) {}

HLTEfficiencyMonitoringAlg::~HLTEfficiencyMonitoringAlg() {}

StatusCode HLTEfficiencyMonitoringAlg::initialize()
{
  using namespace Monitored;
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_TrigT2MbtsBitsContainerKey.initialize());
  ATH_CHECK(m_offlineTrkKey.initialize());
  ATH_CHECK(m_trkCountsKey.initialize());
  ATH_CHECK(m_refTriggerList.size() == m_triggerList.size());
  std::set<std::string> temp( m_triggerList.begin(), m_triggerList.end());
  m_uniqueTriggerList.insert(m_uniqueTriggerList.end(), temp.begin(), temp.end());
  return AthMonitorAlgorithm::initialize();
}

StatusCode HLTEfficiencyMonitoringAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode HLTEfficiencyMonitoringAlg::fillHistograms(const EventContext &context) const
{
  using namespace Monitored;

  const auto &trigDecTool = getTrigDecisionTool();

  auto trkCountsHandle = SG::makeHandle(m_trkCountsKey, context);
  if (!trkCountsHandle.isValid())
  {
    ATH_MSG_DEBUG("Could not retrieve track counts");
    return StatusCode::SUCCESS;
  }

  if (trkCountsHandle->size() == 0)
  { // trigger did not run, no monitoring
    return StatusCode::SUCCESS;
  }
  ATH_CHECK(trkCountsHandle->size() == 1); // if object is present then it should have size == 1
  ATH_CHECK(m_trackSelectionTool.retrieve());

  auto offlineTrkHandle = SG::makeHandle(m_offlineTrkKey, context);

  int countPassing = 0;
  for (const auto trk : *offlineTrkHandle)
  {
    if (m_trackSelectionTool->accept(*trk))
      ++countPassing;
  }
  ATH_MSG_DEBUG("::monitorTrkCounts countPassing = " << countPassing);
  auto nTrkOffline = Scalar("nTrkOffline", countPassing);

  for (auto &ref: m_refTriggerList)
  {
    if (trigDecTool->isPassed(ref, TrigDefs::requireDecision))
    {
      auto trig = m_triggerList[&ref - &m_refTriggerList[0]];   
      ATH_MSG_DEBUG("::monitorTrigEff "<<trig<<" vs "<< ref);
      auto decision = trigDecTool->isPassed(trig, TrigDefs::requireDecision) and (nTrkOffline > 0);
      auto effPassed = Scalar<int>("EffPassed", decision ? 1 : 0);
      const unsigned int passBits = trigDecTool->isPassedBits(trig);
      if (!(passBits & TrigDefs::EF_prescaled))
        fill(trig + ref, effPassed, nTrkOffline);
    }
  }

  for (auto &trig : m_uniqueTriggerList)
  {
    if (trigDecTool->isPassed(trig, TrigDefs::requireDecision))
    {
      auto whichtrigger = Scalar<std::string>("TrigCounts", trig);
      auto nTrkOffline = Scalar("nTrkOffline_counts_"+trig, countPassing);
      fill("TrigAll", whichtrigger, nTrkOffline);
    }
  }

  return StatusCode::SUCCESS;
}
