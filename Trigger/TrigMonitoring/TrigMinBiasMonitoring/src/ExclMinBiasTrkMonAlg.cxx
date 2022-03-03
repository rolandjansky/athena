/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ExclMinBiasTrkMonAlg.h"

ExclMinBiasTrkMonAlg::ExclMinBiasTrkMonAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthMonitorAlgorithm(name, pSvcLocator) {}

StatusCode ExclMinBiasTrkMonAlg::initialize() {
  // Populate m_cuts map
  const std::regex cutsRegex("([\\d]+)trk([\\d]+)_pt([\\d]+)");

  for (const auto& trigger : m_triggerList) {
    std::smatch cutsMatch;
    if (not std::regex_search(trigger, cutsMatch, cutsRegex)) {
      m_cuts[trigger] = {0, -1, 0.};
      continue;
    }

    const int minTrack = std::stoi(cutsMatch[1].str());
    const int maxTrack = std::stoi(cutsMatch[2].str());
    const float minPt  = std::stof(cutsMatch[3].str());

    m_cuts[trigger] = {minTrack, maxTrack, minPt};
  }

  ATH_CHECK(m_trkCountsKey.initialize());
  ATH_CHECK(m_offlineTrkKey.initialize());
  ATH_CHECK(m_onlineTrkKey.initialize());

  ATH_CHECK(m_trackSelectionTool.retrieve());

  return AthMonitorAlgorithm::initialize();
}

StatusCode ExclMinBiasTrkMonAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode ExclMinBiasTrkMonAlg::fillHistograms(const EventContext& context) const {
  using namespace Monitored;
  const auto& trigDecTool = getTrigDecisionTool();

  auto trkCountsHandle = SG::makeHandle(m_trkCountsKey, context);
  if (!trkCountsHandle.isValid()) {
    ATH_MSG_DEBUG("Could not retrieve track counts");
    return StatusCode::SUCCESS;
  }

  if (trkCountsHandle->size() == 0) { // trigger did not run, no monitoring
    return StatusCode::SUCCESS;
  }
  ATH_CHECK(trkCountsHandle->size() == 1); // if object is present then it should have size == 1
  auto nTrkOnline = Scalar("nTrkOnline", trkCountsHandle->at(0)->getDetail<int>("ntrks"));

  auto track_selector = [this](const xAOD::TrackParticle& trk) {
    return m_trackSelectionTool->accept(trk) and std::abs(trk.pt()) > m_minPt;
  };


  //* offline tracks
  auto offlineTrkHandle = SG::makeHandle(m_offlineTrkKey, context);

  int countPassingAny = 0;
  int countPassingPt1 = 0;
  int countPassingPt2 = 0;
  int countPassingPt4 = 0;
  int countPassingPt6 = 0;

  float leadingPt = 0.;
  for (const auto trk : *offlineTrkHandle) {
    if (not track_selector(*trk)) { break; }
    countPassingAny++;

    const float pt = trk->pt() * 1e-3;
    if (pt > 1) { countPassingPt1++; }
    if (pt > 2) { countPassingPt2++; }
    if (pt > 4) { countPassingPt4++; }
    if (pt > 6) { countPassingPt6++; }
    if (pt > leadingPt) { leadingPt = pt; }
  }

  auto nTrkOfflineAll = Scalar("nTrkOfflineAll", offlineTrkHandle->size());

  auto nTrkOfflineAny = Scalar("nTrkOfflineGood", countPassingAny);
  auto nTrkOfflinePt1 = Scalar("nTrkOfflineGoodPt1", countPassingPt1);
  auto nTrkOfflinePt2 = Scalar("nTrkOfflineGoodPt2", countPassingPt2);
  auto nTrkOfflinePt4 = Scalar("nTrkOfflineGoodPt4", countPassingPt4);
  auto nTrkOfflinePt6 = Scalar("nTrkOfflineGoodPt6", countPassingPt6);
  auto leadPtOffline  = Scalar("leadingPtOffline", leadingPt);

  auto trackPt  = Collection("trkOfflinePt", *offlineTrkHandle, [](const auto& trk) { return trk->pt() * 1e-3; });
  auto trackEta = Collection("trkOfflineEta", *offlineTrkHandle, &xAOD::TrackParticle::eta);
  auto trackPhi = Collection("trkOfflinePhi", *offlineTrkHandle, &xAOD::TrackParticle::phi);
  auto trackD0  = Collection("trkOfflineD0", *offlineTrkHandle, &xAOD::TrackParticle::d0);
  auto trackZ0  = Collection("trkOfflineZ0", *offlineTrkHandle, &xAOD::TrackParticle::z0);

  auto trackCuts = Collection("trackCuts", *offlineTrkHandle, [&](const auto& trk) { return track_selector(*trk); });

  ATH_MSG_DEBUG("::monitorTrkCounts countPassing offline = " << countPassingAny);
  //* offline tracks


  //* online tracks
  auto onlineTrkHandle = SG::makeHandle(m_onlineTrkKey, context);

  int countPassingOnlineAny = 0;
  int countPassingOnlinePt1 = 0;
  int countPassingOnlinePt2 = 0;
  int countPassingOnlinePt4 = 0;
  int countPassingOnlinePt6 = 0;

  float leadingPtOnline = 0.;
  for (const auto trk : *onlineTrkHandle) {
    if (not track_selector(*trk)) { break; }
    countPassingOnlineAny++;

    const float pt = trk->pt() * 1e-3;
    if (pt > 1) { countPassingOnlinePt1++; }
    if (pt > 2) { countPassingOnlinePt2++; }
    if (pt > 4) { countPassingOnlinePt4++; }
    if (pt > 6) { countPassingOnlinePt6++; }
    if (pt > leadingPtOnline) { leadingPtOnline = pt; }
  }

  auto nTrkOnlineAll = Scalar("nTrkOnlineAll", onlineTrkHandle->size());

  auto nTrkOnlineAny = Scalar("nTrkOnlineGood", countPassingOnlineAny);
  auto nTrkOnlinePt1 = Scalar("nTrkOnlineGoodPt1", countPassingOnlinePt1);
  auto nTrkOnlinePt2 = Scalar("nTrkOnlineGoodPt2", countPassingOnlinePt2);
  auto nTrkOnlinePt4 = Scalar("nTrkOnlineGoodPt4", countPassingOnlinePt4);
  auto nTrkOnlinePt6 = Scalar("nTrkOnlineGoodPt6", countPassingOnlinePt6);
  auto leadPtOnline  = Scalar("leadingPtOnline", leadingPtOnline);

  auto trackOnlinePt  = Collection("trkOnlinePt", *onlineTrkHandle, [](const auto& trk) { return trk->pt() * 1e-3; });
  auto trackOnlineEta = Collection("trkOnlineEta", *onlineTrkHandle, &xAOD::TrackParticle::eta);
  auto trackOnlinePhi = Collection("trkOnlinePhi", *onlineTrkHandle, &xAOD::TrackParticle::phi);
  auto trackOnlineD0  = Collection("trkOnlineD0", *onlineTrkHandle, &xAOD::TrackParticle::d0);
  auto trackOnlineZ0  = Collection("trkOnlineZ0", *onlineTrkHandle, &xAOD::TrackParticle::z0);

  auto trackOnlineCuts = Collection("trackOnlineCuts", *onlineTrkHandle, [&](const auto& trk) { return track_selector(*trk); });

  ATH_MSG_DEBUG("::monitorTrkCounts countPassing online  = " << countPassingOnlineAny);
  //* online tracks


  //* event info
  auto evtPileUp = Scalar("evtPileUp", GetEventInfo(context)->actualInteractionsPerCrossing());
  //* event info


  for (const auto& ref : m_refTriggerList) {
    ATH_MSG_DEBUG("Checking reference: " << ref << "...");
    if (not trigDecTool->isPassed(ref, TrigDefs::requireDecision)) { continue; }
    ATH_MSG_DEBUG("Reference: " << ref << " passed.");

    for (const auto& trig : m_triggerList) {
      static constexpr int exclMaxTrk    = 15;
      const auto [minTrk, maxTrk, minPt] = m_cuts.at(trig);

      ATH_MSG_DEBUG("\tChecking " << trig << " vs " << ref << " ...");
      const auto trigPassed = trigDecTool->isPassed(trig, TrigDefs::requireDecision);

      ATH_MSG_DEBUG("\tChecking " << ref << " vs " << trig << " cuts");
      auto trkCounter      = [pt = minPt * 1e3](const auto& trk) { return trk->pt() > pt; };
      const auto nTrk      = std::count_if(onlineTrkHandle->cbegin(), onlineTrkHandle->cend(), trkCounter);
      const auto trkPassed = (nTrk >= minTrk and nTrk <= maxTrk and nTrkOnlineAny <= exclMaxTrk);

      ATH_MSG_DEBUG("\t" << ref << " vs " << trig << ", cuts: " << trkPassed << ", trig: " << trigPassed);

      if (not trkPassed) { continue; }
      ATH_MSG_DEBUG("\t" << ref << " passed same track selection as " << trig);

      auto passedRefTrig = Scalar("passedRefTrig", static_cast<int>(trigPassed));
      fill(ref + "_" + trig, passedRefTrig, evtPileUp, leadPtOffline, leadPtOnline);

      if (not trigPassed) { continue; }
      ATH_MSG_DEBUG("\t" << ref << " vs " << trig << " accepted.");

      fill(ref + "_" + trig + "_passed", nTrkOfflineAll, nTrkOfflineAny, nTrkOfflinePt1, nTrkOfflinePt2, nTrkOfflinePt4,
           nTrkOfflinePt6, trackPt, trackEta, trackPhi, trackD0, trackZ0, trackCuts, nTrkOnlineAll, nTrkOnlineAny, nTrkOnlinePt1,
           nTrkOnlinePt2, nTrkOnlinePt4, nTrkOnlinePt6, trackOnlinePt, trackOnlineEta, trackOnlinePhi, trackOnlineD0,
           trackOnlineZ0, trackOnlineCuts, evtPileUp, leadPtOffline, leadPtOnline);
    }
  }

  return StatusCode::SUCCESS;
}
