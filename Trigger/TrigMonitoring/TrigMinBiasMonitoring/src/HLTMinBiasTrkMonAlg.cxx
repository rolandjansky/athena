/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTMinBiasTrkMonAlg.h"

HLTMinBiasTrkMonAlg::HLTMinBiasTrkMonAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthMonitorAlgorithm(name, pSvcLocator)
{
}

HLTMinBiasTrkMonAlg::~HLTMinBiasTrkMonAlg()
{
}

StatusCode HLTMinBiasTrkMonAlg::initialize()
{
  ATH_CHECK(m_spCountsKey.initialize());
  ATH_CHECK(m_trkCountsKey.initialize());
  ATH_CHECK(m_offlineTrkKey.initialize());
  ATH_CHECK(m_onlineTrkKey.initialize());
  ATH_CHECK(m_lvl1EnergySumROIKey.initialize());  

  return AthMonitorAlgorithm::initialize();
}

StatusCode HLTMinBiasTrkMonAlg::finalize()
{

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasTrkMonAlg::fillHistograms(const EventContext& context) const
{

  ATH_CHECK(monitorPurities(context));
  ATH_CHECK(monitorSPCounts(context));
  ATH_CHECK(monitorTrkCounts(context));
  ATH_CHECK(m_trackSelectionTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasTrkMonAlg::monitorPurities(const EventContext& /*context*/) const
{

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasTrkMonAlg::monitorSPCounts(const EventContext& context) const
{
  auto spCountsHandle = SG::makeHandle(m_spCountsKey, context);
  ATH_MSG_DEBUG("SP monitoring, handle validity " << spCountsHandle.isValid());
  if (!spCountsHandle.isValid())
  {
    ATH_MSG_DEBUG("Could not retrieve spCountsHandle");
    return StatusCode::SUCCESS;
  }
  if (spCountsHandle->size() == 0)
  { // trigger did not run, no monitoring
    return StatusCode::SUCCESS;
  }
  ATH_CHECK(spCountsHandle->size() == 1); // if object is present then it should have size == 1

  using namespace Monitored;
  const auto& trigDecTool = getTrigDecisionTool();

  for (auto& trig : m_triggerList)
  {
    if (trigDecTool->isPassed(trig, TrigDefs::requireDecision))
    {
      ATH_MSG_DEBUG("Chain " << trig << " is passed: YES");
      auto pixelCL = Scalar("PixelCL", spCountsHandle->at(0)->getDetail<int>("pixCL"));
      auto PixBarr_SP = Scalar("PixBarr_SP", spCountsHandle->at(0)->getDetail<int>("pixCLBarrel"));
      auto PixECA_SP = Scalar("PixECA_SP", spCountsHandle->at(0)->getDetail<int>("pixCLEndcapA"));
      auto PixECC_SP = Scalar("PixECC_SP", spCountsHandle->at(0)->getDetail<int>("pixCLEndcapC"));

      auto SctTot = Scalar("SctTot", spCountsHandle->at(0)->getDetail<int>("sctSP"));
      auto SctBarr_SP = Scalar("SctBarr_SP", spCountsHandle->at(0)->getDetail<int>("sctSPBarrel"));
      auto SctECA_SP = Scalar("SctECA_SP", spCountsHandle->at(0)->getDetail<int>("sctSPEndcapA"));
      auto SctECC_SP = Scalar("SctECC_SP", spCountsHandle->at(0)->getDetail<int>("sctSPEndcapC"));

      fill(trig + "_SpacePoints", pixelCL, PixBarr_SP, PixECA_SP, PixECC_SP, SctTot, SctBarr_SP, SctECA_SP, SctECC_SP);
    }
    else
      ATH_MSG_DEBUG("Chain " << trig << " is passed: NO");
  }

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasTrkMonAlg::monitorTrkCounts(const EventContext& context) const
{
  using namespace Monitored;
  const auto& trigDecTool = getTrigDecisionTool();

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
  auto nTrkOnline = Scalar("nTrkOnline", trkCountsHandle->at(0)->getDetail<int>("ntrks"));

  auto offlineTrkHandle = SG::makeHandle(m_offlineTrkKey, context);
  int countPassing = 0;

  auto track_selector = [this](const xAOD::TrackParticle& trk ){
    return m_trackSelectionTool->accept(trk) and std::abs(trk.pt()) > m_minPt  and std::abs(trk.z0()) < m_z0; 
  };

  for (const auto trk : *offlineTrkHandle) {
    if ( track_selector(*trk) ) 
      ++countPassing;
  }
  ATH_MSG_DEBUG("::monitorTrkCounts countPassing  = " << countPassing);

  auto onlineTrkHandle = SG::makeHandle(m_onlineTrkKey, context);



  auto nTrkOffline = Scalar("nTrkOffline", countPassing);
  auto nAllTrkOffline = Scalar("nAllTrkOffline", offlineTrkHandle->size());
  auto trkMask = Collection("trkMask", *offlineTrkHandle, [&](const auto& trk) { return track_selector(*trk); });
  auto trkPt = Collection("trkPt", *offlineTrkHandle, [](const auto& trk) { return trk->pt() * 1.e-3; });
  auto trkEta = Collection("trkEta", *offlineTrkHandle, [](const auto& trk) { return trk->eta(); });
  auto trkPhi = Collection("trkPhi", *offlineTrkHandle, [](const auto& trk) { return trk->phi(); });
  auto trkD0 = Collection("trkD0", *offlineTrkHandle, [](const auto& trk) { return trk->d0(); });
  auto trkZ0 = Collection("trkZ0", *offlineTrkHandle, [](const auto& trk) { return trk->z0(); });

  auto getNhits = [](const xAOD::TrackParticle* trk) {
    int nhits = 0; 
    uint32_t pattern = trk->hitPattern();
    for ( int bit = 0; bit < 32; bit++) 
      nhits += (pattern & (1<<bit) ? 1 : 0);
    return nhits;
  };
  auto trkHits = Collection("trkHits", *offlineTrkHandle, getNhits);

  auto onlTrkPt = Collection("onlTrkPt", *onlineTrkHandle, [](const auto& trk) { return trk->pt() * 1.e-3; });
  auto onlTrkEta = Collection("onlTrkEta", *onlineTrkHandle, [](const auto& trk) { return trk->eta(); });
  auto onlTrkPhi = Collection("onlTrkPhi", *onlineTrkHandle, [](const auto& trk) { return trk->phi(); });
  auto onlTrkD0 = Collection("onlTrkD0", *onlineTrkHandle, [](const auto& trk) { return trk->d0(); });
  auto onlTrkZ0 = Collection("onlTrkZ0", *onlineTrkHandle, [](const auto& trk) { return trk->z0(); });

  auto onlTrkHits = Collection("onlTrkHits", *onlineTrkHandle, getNhits);


  auto nMBTrkTrkOfflineRatio = Scalar("trkSelOfflineRatio", (offlineTrkHandle->size() == 0 ? -1 : static_cast<double>(nTrkOffline) / offlineTrkHandle->size()));

  auto L1TEHandle = SG::makeHandle(m_lvl1EnergySumROIKey, context);
  ATH_MSG_DEBUG("L1TE monitoring, handle validity " << L1TEHandle.isValid());
  float sum_roi_sumEt = 0.;
  if (!L1TEHandle.isValid()) sum_roi_sumEt = -1.;
  else sum_roi_sumEt = static_cast<float>(L1TEHandle->energyT()) / 1000.;
  auto L1sumEt = Scalar("L1sumEt", sum_roi_sumEt);

  auto spCountsHandle = SG::makeHandle(m_spCountsKey, context);
  ATH_MSG_DEBUG("SP monitoring, handle validity " << spCountsHandle.isValid());
  if (!spCountsHandle.isValid())
  {
    ATH_MSG_DEBUG("Could not retrieve spCountsHandle ");
    return StatusCode::SUCCESS;
  }
  if (spCountsHandle->size() == 0)
  { // trigger did not run, no monitoring
    return StatusCode::SUCCESS;
  }
  ATH_CHECK(spCountsHandle->size() == 1); // if object is present then it should have size == 1
  for (auto& trig : m_triggerList)
  {
    if (trigDecTool->isPassed(trig, TrigDefs::requireDecision))
    {
      ATH_MSG_DEBUG("Chain " << trig << " is passed: YES");
      auto pixelCL = Scalar("PixelCL", spCountsHandle->at(0)->getDetail<int>("pixCL"));
      auto PixBarr_SP = Scalar("PixBarr_SP", spCountsHandle->at(0)->getDetail<int>("pixCLBarrel"));
      auto PixECA_SP = Scalar("PixECA_SP", spCountsHandle->at(0)->getDetail<int>("pixCLEndcapA"));
      auto PixECC_SP = Scalar("PixECC_SP", spCountsHandle->at(0)->getDetail<int>("pixCLEndcapC"));

      auto SctTot = Scalar("SctTot", spCountsHandle->at(0)->getDetail<int>("sctSP"));
      auto SctBarr_SP = Scalar("SctBarr_SP", spCountsHandle->at(0)->getDetail<int>("sctSPBarrel"));
      auto SctECA_SP = Scalar("SctECA_SP", spCountsHandle->at(0)->getDetail<int>("sctSPEndcapA"));
      auto SctECC_SP = Scalar("SctECC_SP", spCountsHandle->at(0)->getDetail<int>("sctSPEndcapC"));

      ATH_MSG_DEBUG("::monitorTrkCounts Chain " << trig << "  " << (trigDecTool->isPassed(trig, TrigDefs::requireDecision) ? "passed" : "did not pass"));


      double nTrkRatio = offlineTrkHandle->size() > 0 ? static_cast<double>(offlineTrkHandle->size()) / static_cast<double>(trkCountsHandle->at(0)->getDetail<int>("ntrks")) : -1.0;
      auto trkRatio = Scalar("nTrkRatio", nTrkRatio);
      fill(trig + "_Tracking", nTrkOffline, nAllTrkOffline, nTrkOnline, trkRatio, nMBTrkTrkOfflineRatio, pixelCL,
        PixBarr_SP, PixECA_SP, PixECC_SP, 
        SctTot, SctBarr_SP, SctECA_SP, SctECC_SP, 
        L1sumEt, 
        trkMask, trkPt, trkEta, trkPhi, trkD0, trkZ0, trkHits,
        onlTrkPt, onlTrkEta, onlTrkPhi, onlTrkHits, onlTrkD0, onlTrkZ0);
    }

    // measure eff wrt the L1
    const unsigned int passBits = trigDecTool->isPassedBits(trig);
    if ((!(passBits & TrigDefs::EF_prescaled)) && (passBits & TrigDefs::L1_isPassedAfterVeto)) { // HLT did run
      auto decision = Scalar<int>("decision", trigDecTool->isPassed(trig, TrigDefs::requireDecision) ? 1 : 0); // HLT did pass
      fill(trig + "_Tracking", nTrkOffline, nAllTrkOffline, nTrkOnline, decision);
    }


  }
  return StatusCode::SUCCESS;
}