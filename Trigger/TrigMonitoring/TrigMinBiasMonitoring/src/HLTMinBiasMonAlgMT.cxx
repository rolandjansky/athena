/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HLTMinBiasMonAlgMT.h"

HLTMinBiasMonAlgMT::HLTMinBiasMonAlgMT(const std::string& name, ISvcLocator* pSvcLocator) :
AthMonitorAlgorithm(name, pSvcLocator)
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

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonAlgMT::monitorPurities(const EventContext& /*context*/) const {

  return StatusCode::SUCCESS;
}

StatusCode HLTMinBiasMonAlgMT::monitorSPCounts(const EventContext& context) const {
  auto spCountsHandle = SG::makeHandle( m_spCountsKey, context);
  ATH_MSG_DEBUG("Inside SP Loop");
  ATH_MSG_DEBUG(spCountsHandle->size());
  ATH_MSG_DEBUG(spCountsHandle.isValid());
  if (!spCountsHandle.isValid()) {
    ATH_MSG_DEBUG("Could not retrieve spCountsHandle");
    return StatusCode::SUCCESS; }

    ATH_CHECK( spCountsHandle.isValid() );
    if ( spCountsHandle->size() == 0 ) { // trigger did not run, no monitoring
      return StatusCode::SUCCESS;
    }
    ATH_CHECK( spCountsHandle->size() == 1 ); // if object is present then it should have size == 1

    using namespace Monitored;
    const auto& trigDecTool = getTrigDecisionTool();
    auto chaingroup = trigDecTool->getChainGroup("HLT_mb.*");//checking if this works ;

    for(auto &trig : chaingroup->getListOfTriggers()) {
      auto cg = trigDecTool->getChainGroup(trig);
      std::string thisTrig = trig;

      ATH_MSG_DEBUG (thisTrig << " chain prescale = " << cg->getPrescale());

      if(trigDecTool->isPassed(trig)) {ATH_MSG_DEBUG("Chain "<<trig <<" is passed: YES");}
      else ATH_MSG_DEBUG("Chain "<<trig <<" is passed: NO");

      if (cg->getPrescale()) {
        auto pixelSPLow  = Scalar( "PixelSPLow", spCountsHandle->at(0)->getDetail<int>("totNumPixSP") );
        auto pixelSPHigh = Scalar( "PixelSPHigh", spCountsHandle->at(0)->getDetail<int>("totNumPixSP") );
        auto PixBarr_SP  = Scalar( "PixBarr_SP", spCountsHandle->at(0)->getDetail<int>("pixClBarrel") );
        auto PixECA_SP   = Scalar( "PixECA_SP", spCountsHandle->at(0)->getDetail<int>("pixClEndcapA") );
        auto PixECC_SP   = Scalar( "PixECC_SP", spCountsHandle->at(0)->getDetail<int>("pixClEndcapC") );

        auto SctTot       = Scalar( "SctTot", spCountsHandle->at(0)->getDetail<int>("totNumSctSP") );
        auto SctBarr_SP  = Scalar( "SctBarr_SP", spCountsHandle->at(0)->getDetail<int>("SctSpBarrel") );
        auto SctECA_SP   = Scalar( "SctECA_SP", spCountsHandle->at(0)->getDetail<int>("SctSpEndcapA") );
        auto SctECC_SP   = Scalar( "SctECC_SP", spCountsHandle->at(0)->getDetail<int>("SctSpEndcapC") );

        fill(thisTrig, pixelSPLow, pixelSPHigh, PixBarr_SP,PixECA_SP, PixECC_SP, SctTot, SctBarr_SP, SctECA_SP, SctECC_SP);
      }
    }

    return StatusCode::SUCCESS;
  }

  StatusCode HLTMinBiasMonAlgMT::monitorTrkCounts(const EventContext& context) const {
    auto HLTxaodTrkHandle = SG::makeHandle( m_HLTxaodTrkKey, context);
    auto inDetTrackParticlesHandle = SG::makeHandle( m_inDetTrackParticlesKey, context);
    auto trkCountsHandle = SG::makeHandle( m_trkCountsKey, context);
    ATH_MSG_DEBUG("Inside Trk Loop");
    ATH_MSG_DEBUG(trkCountsHandle->size());
    ATH_MSG_DEBUG(trkCountsHandle.isValid());

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
      const auto& trigDecTool = getTrigDecisionTool();
      auto chaingroup = trigDecTool->getChainGroup("HLT_mb_sptrk_L1.*");//checking if this works ;
      long unsigned int counter = 0;
      for(auto &trig : chaingroup->getListOfTriggers()) {
        std::string thisTrig = trig;
        if(thisTrig!=m_triggerList[counter]) {
          ATH_MSG_DEBUG("Trigger Names or Ordering Don't match");
          ATH_MSG_DEBUG ("trig in Alg = "<<thisTrig << " trig in List = " << m_triggerList[counter]);
          return StatusCode::SUCCESS;
        }

        if(trigDecTool->isPassed(trig)) {
          ATH_MSG_DEBUG("Chain "<<trig <<" is passed: YES");
        }
        else ATH_MSG_DEBUG("Chain "<<trig <<" is passed: NO");

        const unsigned int passBits = trigDecTool->isPassedBits(trig);
        ATH_MSG_DEBUG(passBits);
        ATH_MSG_DEBUG(TrigDefs::EF_prescaled);
        ATH_MSG_DEBUG(trigDecTool->isPassed("L1_RD0_FILLED"));
        ATH_MSG_DEBUG(trigDecTool->isPassed(trig));
        if ((!(passBits & TrigDefs::EF_prescaled)) && (passBits & TrigDefs::L1_isPassedAfterVeto)) {
          auto xaodntrk = Scalar( "xaodnTrk", HLTxaodTrkHandle->size() );
          auto decision = Scalar<int>("decision", trigDecTool->isPassed(trig) ? 1 : 0);
          auto NumGoodOfflineTracks = Scalar("NumGoodOfflineTracks", inDetTrackParticlesHandle->size());
          auto ntrk = Scalar( "nTrk", trkCountsHandle->at(0)->getDetail<int>("ntrks") );
          auto NumGoodOnlineTracks = Scalar("NumGoodOnlineTracks", trkCountsHandle->at(0)->getDetail<int>("ntrks"));//set appropriate condition for selection later
          auto whichtrigger =  Scalar("whichTrigger",trig);

          fill(thisTrig+"_Eff",ntrk,NumGoodOnlineTracks,xaodntrk,decision,NumGoodOfflineTracks,whichtrigger);
          fill("EffAll",decision,whichtrigger);
        }

        if ( trigDecTool->isPassed(trig) ) {
          auto whichtrigger =  Scalar("whichTrigger",trig);
          auto PurityPassed = Scalar<int>("PurityPassed", trigDecTool->isPassed(trig) ? 1 : 0);
          fill("EffAll",PurityPassed,whichtrigger);
        }
        if(counter<m_triggerList.size())counter++;
      }
      return StatusCode::SUCCESS;
    }
