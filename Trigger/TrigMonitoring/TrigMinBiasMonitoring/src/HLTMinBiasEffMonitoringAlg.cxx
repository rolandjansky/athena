/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTMinBiasEffMonitoringAlg.h"

HLTMinBiasEffMonitoringAlg::HLTMinBiasEffMonitoringAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthMonitorAlgorithm(name, pSvcLocator) {}

HLTMinBiasEffMonitoringAlg::~HLTMinBiasEffMonitoringAlg() {}

StatusCode HLTMinBiasEffMonitoringAlg::initialize()
{
  using namespace Monitored;
  ATH_CHECK(m_TrigT2MbtsBitsContainerKey.initialize());
  ATH_CHECK(m_offlineTrkKey.initialize());
  ATH_CHECK(m_trkCountsKey.initialize());
  ATH_CHECK(m_refTriggerList.size() == m_triggerList.size());
  std::set<std::string> temp(m_triggerList.begin(), m_triggerList.end());
  m_uniqueTriggerList.insert(m_uniqueTriggerList.end(), temp.begin(), temp.end());

  ATH_CHECK(m_trackSelectionTool.retrieve());

  return AthMonitorAlgorithm::initialize();
}

StatusCode HLTMinBiasEffMonitoringAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasEffMonitoringAlg::fillHistograms(const EventContext& context) const
{
  using namespace Monitored;

  const auto& trigDecTool = getTrigDecisionTool();

  auto offlineTrkHandle = SG::makeHandle(m_offlineTrkKey, context);
  int countPassing = 0;
  int countPassing_pt05 = 0; // count of tracks passing higher pt (here 0.5 GeV)
  int countPassing_pt1 = 0; // count of tracks passing higher pt (here 1 GeV)
  int countPassing_pt2 = 0; // ...
  int countPassing_pt4 = 0;
  int countPassing_pt6 = 0;
  int countPassing_pt8 = 0;
  auto leadingTrackPt = Scalar<double>("leadingTrackPt");
  for (const auto trk : *offlineTrkHandle)
  {
    if (m_trackSelectionTool->accept(*trk) and std::fabs(trk->pt()) > m_minPt) {
      const double pt = std::fabs(trk->pt()) * 1e-3; // fabs used in case the charge is encoded in pt ( i.e. it is really q * pt)

      ++countPassing;
      if (pt > 0.5)
        ++countPassing_pt05;
      if (pt > 1.)
        ++countPassing_pt1;
      if (pt > 2.)
        ++countPassing_pt2;
      if (pt > 4.)
        ++countPassing_pt4;
      if (pt > 6.)
        ++countPassing_pt6;
      if (pt > 8.)
        ++countPassing_pt8;
      if (pt > leadingTrackPt) leadingTrackPt = pt;
    }
  }
  ATH_MSG_DEBUG("::monitorTrkCounts countPassing = " << countPassing);
  auto nTrkOffline = Scalar("nTrkOffline", countPassing);
  auto nTrkOffline_pt05 = Scalar("nTrkOffline_pt05", countPassing_pt05);
  auto nTrkOffline_pt1 = Scalar("nTrkOffline_pt1", countPassing_pt1);
  auto nTrkOffline_pt2 = Scalar("nTrkOffline_pt2", countPassing_pt2);
  auto nTrkOffline_pt4 = Scalar("nTrkOffline_pt4", countPassing_pt4);
  auto nTrkOffline_pt6 = Scalar("nTrkOffline_pt6", countPassing_pt6);
  auto nTrkOffline_pt8 = Scalar("nTrkOffline_pt8", countPassing_pt8);

  for (auto& ref : m_refTriggerList)
  {
    auto trig = m_triggerList[&ref - &m_refTriggerList[0]];
    ATH_MSG_DEBUG("checking " << trig << " vs " << ref);

    if (trigDecTool->isPassed(ref, TrigDefs::requireDecision))
    {
      ATH_MSG_DEBUG("ref passed for " << trig << " vs " << ref);
      const unsigned int passBits = trigDecTool->isPassedBits(trig);
      if (!(passBits & TrigDefs::EF_prescaled)) {
        auto decision = ((passBits & TrigDefs::EF_passedRaw) != 0) ? 1 : 0;
        auto effPassed = Scalar<int>("EffPassed", decision);
        fill(trig + ref, effPassed, nTrkOffline, nTrkOffline_pt05, nTrkOffline_pt1, nTrkOffline_pt2, nTrkOffline_pt4, nTrkOffline_pt6, nTrkOffline_pt8, leadingTrackPt);
      }
    }
  }

  for (auto& trig : m_uniqueTriggerList)
  {
    if (trigDecTool->isPassed(trig, TrigDefs::requireDecision))
    {
      auto whichtrigger = Scalar<std::string>("TrigCounts", trig);
      auto nTrkOffline = Scalar("nTrkOffline_counts_" + trig, countPassing);
      fill("TrigAll", whichtrigger, nTrkOffline);
    }
  }

  return StatusCode::SUCCESS;
}
