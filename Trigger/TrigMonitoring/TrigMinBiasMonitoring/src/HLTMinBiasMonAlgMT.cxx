/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTMinBiasMonAlgMT.h"

HLTMinBiasMonAlgMT::HLTMinBiasMonAlgMT(const std::string& name, ISvcLocator* pSvcLocator) :
AthMonitorAlgorithm(name, pSvcLocator),
m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool")
{
}

HLTMinBiasMonAlgMT::~HLTMinBiasMonAlgMT()
{
}

StatusCode HLTMinBiasMonAlgMT::initialize()
{
  ATH_CHECK( AthMonitorAlgorithm::initialize() );
  ATH_CHECK( m_spCountsKey.initialize() );
  ATH_CHECK( m_trkCountsKey.initialize() );
  ATH_CHECK( m_HLTxaodTrkKey.initialize() );
  ATH_CHECK( m_inDetTrackParticlesKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonAlgMT::finalize()
{

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonAlgMT::fillHistograms(const EventContext& context) const
{
  using namespace Monitored;

  ATH_CHECK( monitorPurities( context ) );
  ATH_CHECK( monitorSPCounts( context ) );
  ATH_CHECK( monitorTrkCounts( context ) );
  ATH_CHECK( m_trigDecTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonAlgMT::monitorPurities(const EventContext& /*context*/) const {

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonAlgMT::monitorSPCounts(const EventContext& context) const {
  auto spCountsHandle = SG::makeHandle( m_spCountsKey, context);


  if (!spCountsHandle.isValid()) {
  ATH_MSG_DEBUG("Could not retrieve spCountsHandle");
   return StatusCode::SUCCESS; }

   ATH_CHECK( spCountsHandle.isValid() );
  if ( spCountsHandle->size() == 0 ) { // trigger did not run, no monitoring
    return StatusCode::SUCCESS;
  }
  ATH_CHECK( spCountsHandle->size() == 1 ); // if object is present then it should have size == 1

  using namespace Monitored;
  for (long unsigned int i =0; i<m_triggerList.size(); i++){

    if (m_trigDecTool->getPrescale(m_triggerList[i])) {
      auto pixelSPLow  = Scalar( "PixelSPLow", spCountsHandle->at(0)->getDetail<int>("totNumPixSP") );
      auto pixelSPHigh = Scalar( "PixelSPHigh", spCountsHandle->at(0)->getDetail<int>("totNumPixSP") );
      auto PixBarr_SP  = Scalar( "PixBarr_SP", spCountsHandle->at(0)->getDetail<int>("pixClBarrel") );
      auto PixECA_SP   = Scalar( "PixECA_SP", spCountsHandle->at(0)->getDetail<int>("pixClEndcapA") );
      auto PixECC_SP   = Scalar( "PixECC_SP", spCountsHandle->at(0)->getDetail<int>("pixClEndcapC") );

      auto SctTot       = Scalar( "SctTot", spCountsHandle->at(0)->getDetail<int>("totNumSctSP") );
      auto SctBarr_SP  = Scalar( "SctBarr_SP", spCountsHandle->at(0)->getDetail<int>("SctSpBarrel") );
      auto SctECA_SP   = Scalar( "SctECA_SP", spCountsHandle->at(0)->getDetail<int>("SctSpEndcapA") );
      auto SctECC_SP   = Scalar( "SctECC_SP", spCountsHandle->at(0)->getDetail<int>("SctSpEndcapC") );

      fill(m_triggerList[i], pixelSPLow, pixelSPHigh, PixBarr_SP,PixECA_SP, PixECC_SP, SctTot, SctBarr_SP, SctECA_SP, SctECC_SP);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonAlgMT::monitorTrkCounts(const EventContext& context) const {
  auto HLTxaodTrkHandle = SG::makeHandle( m_HLTxaodTrkKey, context);
  auto inDetTrackParticlesHandle = SG::makeHandle( m_inDetTrackParticlesKey, context);
  auto trkCountsHandle = SG::makeHandle( m_trkCountsKey, context);

  using namespace Monitored;

  if (!trkCountsHandle.isValid()) {
  ATH_MSG_DEBUG("Could not retrieve trkCountsHandle");
   return StatusCode::SUCCESS; }

  ATH_CHECK( trkCountsHandle.isValid() );
  if ( trkCountsHandle->size() == 0 ) { // trigger did not run, no monitoring
     return StatusCode::SUCCESS;
  }

  ATH_CHECK( HLTxaodTrkHandle.isValid() );
  if ( HLTxaodTrkHandle->size() == 0 ) { // trigger did not run, no monitoring
    return StatusCode::SUCCESS;
  }
  ATH_CHECK( inDetTrackParticlesHandle.isValid() );
  if ( inDetTrackParticlesHandle->size() == 0 ) { // trigger did not run, no monitoring
    return StatusCode::SUCCESS;
  }
  ATH_CHECK( trkCountsHandle->size() == 1 ); // if object is present then it should have size == 1

  using namespace Monitored;
  for (long unsigned int i =0; i<m_triggerList.size(); i++){
    const unsigned int passBits = m_trigDecTool->isPassedBits(m_triggerList[i]);
    ATH_MSG_DEBUG(passBits);
    ATH_MSG_DEBUG(TrigDefs::EF_prescaled);
    ATH_MSG_DEBUG(m_trigDecTool->isPassed("L1_RD0_FILLED"));
    ATH_MSG_DEBUG(m_trigDecTool->isPassed(m_triggerList[i]));
    if ((!(passBits & TrigDefs::EF_prescaled)) && (passBits & TrigDefs::L1_isPassedAfterVeto)) {
      auto xaodntrk = Scalar( "xaodnTrk", HLTxaodTrkHandle->size() );
      auto decision = Scalar<int>("decision", m_trigDecTool->isPassed(m_triggerList[i]) ? 1 : 0);
      auto NumGoodOfflineTracks = Scalar("NumGoodOfflineTracks", inDetTrackParticlesHandle->size());
      auto ntrk = Scalar( "nTrk", trkCountsHandle->at(0)->getDetail<int>("ntrks") );
      auto NumGoodOnlineTracks = Scalar("NumGoodOnlineTracks", trkCountsHandle->at(0)->getDetail<int>("ntrks"));
      auto whichTrigger =  Scalar("whichTrigger",m_triggerList[i]);

      fill(m_triggerList[i]+"_Eff", ntrk,NumGoodOnlineTracks,xaodntrk,decision,NumGoodOfflineTracks,whichTrigger);
      fill("EffAll",decision,whichTrigger);

    }

    if ( m_trigDecTool->isPassed(m_triggerList[i]) ) {
      auto whichTrigger =  Scalar("whichTrigger",m_triggerList[i]);
      auto PurityPassed = Scalar<int>("PurityPassed", m_trigDecTool->isPassed(m_triggerList[i]) ? 1 : 0);
      fill("EffAll",PurityPassed,whichTrigger);
    }
  }
  return StatusCode::SUCCESS;
}
