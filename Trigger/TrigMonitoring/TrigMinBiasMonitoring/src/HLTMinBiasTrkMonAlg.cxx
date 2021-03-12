/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTMinBiasTrkMonAlg.h"

HLTMinBiasTrkMonAlg::HLTMinBiasTrkMonAlg(const std::string &name, ISvcLocator *pSvcLocator) : AthMonitorAlgorithm(name, pSvcLocator)
{
}

HLTMinBiasTrkMonAlg::~HLTMinBiasTrkMonAlg()
{
}

StatusCode HLTMinBiasTrkMonAlg::initialize()
{
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_spCountsKey.initialize());
  ATH_CHECK(m_trkCountsKey.initialize());
  ATH_CHECK(m_offlineTrkKey.initialize());
  ATH_CHECK(m_onlineTrkKey.initialize());

  return AthMonitorAlgorithm::initialize();
}

StatusCode HLTMinBiasTrkMonAlg::finalize()
{

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasTrkMonAlg::fillHistograms(const EventContext &context) const
{

  ATH_CHECK(monitorPurities(context));
  ATH_CHECK(monitorSPCounts(context));
  ATH_CHECK(monitorTrkCounts(context));
  ATH_CHECK(m_trackSelectionTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasTrkMonAlg::monitorPurities(const EventContext & /*context*/) const
{

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasTrkMonAlg::monitorSPCounts(const EventContext &context) const
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
  const auto &trigDecTool = getTrigDecisionTool();

  for (auto &trig : m_triggerList)
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

      fill(trig+"_SpacePoints", pixelCL, PixBarr_SP, PixECA_SP, PixECC_SP, SctTot, SctBarr_SP, SctECA_SP, SctECC_SP);
    }
    else
      ATH_MSG_DEBUG("Chain " << trig << " is passed: NO");
  }

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasTrkMonAlg::monitorTrkCounts(const EventContext &context) const
{
  using namespace Monitored;

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

  auto offlineTrkHandle = SG::makeHandle(m_offlineTrkKey, context);
  // TODO, instead counting all tracks need to count tracks passing offline min-bias selection

  int countPassing = 0;
  for(const auto trk : *offlineTrkHandle)
  {
    if(m_trackSelectionTool->accept(*trk)) ++countPassing;
  }
  ATH_MSG_DEBUG("::monitorTrkCounts countPassing  =" << countPassing );

  auto nTrkOffline = Scalar("nTrkOffline", countPassing);
  auto nAllTrkOffline = Scalar("nAllTrkOffline", offlineTrkHandle->size());

  auto nMBTrkTrkOfflineRatio = Scalar("trkSelOfflineRatio", (offlineTrkHandle->size() == 0 ? -1 : static_cast<double>(nTrkOffline)/offlineTrkHandle->size() ));

  const auto &trigDecTool = getTrigDecisionTool();
  for (auto &trig : m_triggerList )
  {
    ATH_MSG_DEBUG("::monitorTrkCounts Chain " << trig << "  " << (trigDecTool->isPassed(trig, TrigDefs::requireDecision) ? "passed" : "did not pass"));
  
    const unsigned int passBits = trigDecTool->isPassedBits(trig);
    ATH_MSG_DEBUG("::monitorTrkCountsPass Bits: " << passBits);

//    if ((!(passBits & TrigDefs::EF_prescaled)) && (passBits & TrigDefs::L1_isPassedAfterVeto))
    if ( ! (passBits & TrigDefs::EF_prescaled) )
    {
      auto decision = Scalar<int>("decision", trigDecTool->isPassed(trig, TrigDefs::requireDecision) ? 1 : 0);
      auto nTrk = Scalar("nTrkOnline", trkCountsHandle->at(0)->getDetail<int>("ntrks"));
      auto whichtrigger = Scalar<std::string>("whichTrigger", trig);

      double nTrkRatio = offlineTrkHandle->size() > 0 ? static_cast<double>(offlineTrkHandle->size()) / static_cast<double>(trkCountsHandle->at(0)->getDetail<int>("ntrks")) : -1.0;
      auto trkRatio = Scalar("nTrkRatio", nTrkRatio);
      fill(trig + "_Tracking", nTrkOffline, nAllTrkOffline, decision, whichtrigger, trkRatio, nMBTrkTrkOfflineRatio);
      fill("EffAll", decision, whichtrigger);
    }

    if (trigDecTool->isPassed(trig, TrigDefs::requireDecision))
    {
      auto whichtrigger = Scalar("whichTrigger", trig);
      auto PurityPassed = Scalar<int>("PurityPassed", nTrkOffline > 0 ? 1 : 0);
      fill("EffAll", PurityPassed, whichtrigger);
    }

  }
  return StatusCode::SUCCESS;
}
