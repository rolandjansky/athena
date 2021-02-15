/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTMinBiasMonAlgMT.h"

HLTMinBiasMonAlgMT::HLTMinBiasMonAlgMT(const std::string &name, ISvcLocator *pSvcLocator) : AthMonitorAlgorithm(name, pSvcLocator)
{
}

HLTMinBiasMonAlgMT::~HLTMinBiasMonAlgMT()
{
}

StatusCode HLTMinBiasMonAlgMT::initialize()
{
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_spCountsKey.initialize());
  ATH_CHECK(m_trkCountsKey.initialize());
  ATH_CHECK(m_offlineTrkKey.initialize());
  ATH_CHECK(m_onlineTrkKey.initialize());

  return AthMonitorAlgorithm::initialize();
}

StatusCode HLTMinBiasMonAlgMT::finalize()
{

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonAlgMT::fillHistograms(const EventContext &context) const
{

  ATH_CHECK(monitorPurities(context));
  ATH_CHECK(monitorSPCounts(context));
  ATH_CHECK(monitorTrkCounts(context));

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonAlgMT::monitorPurities(const EventContext & /*context*/) const
{

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonAlgMT::monitorSPCounts(const EventContext &context) const
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
    if (trigDecTool->isPassed(trig))
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

      fill(trig+"_SP", pixelCL, PixBarr_SP, PixECA_SP, PixECC_SP, SctTot, SctBarr_SP, SctECA_SP, SctECC_SP);
    }
    else
      ATH_MSG_DEBUG("Chain " << trig << " is passed: NO");
  }

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonAlgMT::monitorTrkCounts(const EventContext &context) const
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
  auto nTrkOffline = Scalar("nTrkOffline", offlineTrkHandle->size());

  const auto &trigDecTool = getTrigDecisionTool();
  auto sptrkTriggers = trigDecTool->getChainGroup("HLT_mb_sp.*_L1.*")->getListOfTriggers();
  for (auto &trig : m_triggerList )
  {
    if ( std::find(sptrkTriggers.begin(), sptrkTriggers.end(), trig) == sptrkTriggers.end() ) continue; //not the mbsptrk trigger

    ATH_MSG_DEBUG("Chain " << trig << "  " << (trigDecTool->isPassed(trig) ? "passed" : "did not pass"));
  
    const unsigned int passBits = trigDecTool->isPassedBits(trig);
    ATH_MSG_DEBUG("Pass Bits: " << passBits);
    if ((!(passBits & TrigDefs::EF_prescaled)) && (passBits & TrigDefs::L1_isPassedAfterVeto))
    {
      auto decision = Scalar<int>("decision", trigDecTool->isPassed(trig) ? 1 : 0);
      auto nTrk = Scalar("nTrkOnline", trkCountsHandle->at(0)->getDetail<int>("ntrks"));
      auto whichtrigger = Scalar<std::string>("whichTrigger", trig);

      double nTrkRatio = offlineTrkHandle->size() > 0 ? static_cast<double>(offlineTrkHandle->size()) / static_cast<double>(trkCountsHandle->at(0)->getDetail<int>("ntrks")) : -1.0;
      auto trkRatio = Scalar("nTrkRatio", nTrkRatio);
      fill(trig + "_Eff", nTrkOffline, decision, whichtrigger, trkRatio);
      fill("EffAll", decision, whichtrigger);
    }

    if (trigDecTool->isPassed(trig))
    {
      auto whichtrigger = Scalar("whichTrigger", trig);
      auto PurityPassed = Scalar<int>("PurityPassed", nTrkOffline > 0 ? 1 : 0);
      fill("EffAll", PurityPassed, whichtrigger);
    }

  }
  return StatusCode::SUCCESS;
}
