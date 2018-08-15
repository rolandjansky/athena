/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         TrigDecisionMakerMT.h
 **
 **   Description:  - Re-entrant Algorithm-derived class to run after the MT Trigger to create the
 **                   xAOD::TrigDecision object. Based on TrigDecisionMaker.cxx
 **
 * @author Tim Martin      <Tim.Martin@cern.ch>      - University of Warwick
 **
 **   Created:      16 July 2018
 **
 **************************************************************************/

#include "TrigDecisionMakerMT.h"

#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteering/Lvl1ResultAccessTool.h"

#include "xAODTrigger/TrigDecisionAuxInfo.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfL1Data/BunchGroupSet.h"

#include "GaudiKernel/Incident.h"

using namespace TrigDec;

TrigDecisionMakerMT::TrigDecisionMakerMT(const std::string &name, ISvcLocator *pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator),
    m_trigConfigSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_lvl1Tool("HLT::Lvl1ResultAccessTool/Lvl1ResultAccessTool", this)
{}

TrigDecisionMakerMT::~TrigDecisionMakerMT() {}

StatusCode TrigDecisionMakerMT::initialize()
{
  ATH_CHECK( m_HLTSummaryKeyIn.initialize() );
  ATH_CHECK( m_ROIBResultKeyIn.initialize() );
  ATH_CHECK( m_EventInfoKeyIn.initialize() );

  ATH_CHECK( m_trigDecisionKeyOut.initialize() );

  ATH_CHECK( m_lvl1Tool.retrieve() );
  ATH_CHECK( m_trigConfigSvc.retrieve() );

  return StatusCode::SUCCESS;
}


StatusCode TrigDecisionMakerMT::finalize()
{
  // print out stats: used also to do regression tests
  ATH_MSG_DEBUG ("=============================================");
  ATH_MSG_DEBUG ("REGTEST Run summary:");
  ATH_MSG_DEBUG ("REGTEST  Events processed         : " << m_nEvents);
  ATH_MSG_DEBUG ("REGTEST  Level 1  : passed        = " << m_l1Passed);
  ATH_MSG_DEBUG ("REGTEST  HLT      : passed        = " << m_hltPassed);
  ATH_MSG_DEBUG ("=============================================");

  return StatusCode::SUCCESS;
}


StatusCode TrigDecisionMakerMT::execute_r(const EventContext& context) const 
{
  // increment event counter
  m_nEvents++;

  // Retrieve the results
  const LVL1CTP::Lvl1Result* l1Result = nullptr;
  if (m_doL1) {
    ATH_CHECK(getL1Result(l1Result, context));
    if (l1Result->isAccepted()) {
      ++m_l1Passed;
    }
  }

  const TrigCompositeUtils::DecisionContainer* hltResult = nullptr;
  if (m_doHLT) {
    ATH_CHECK(getHLTResult(hltResult, context));
  }

  std::unique_ptr<xAOD::TrigDecision>        trigDec    = std::make_unique<xAOD::TrigDecision>();
  std::unique_ptr<xAOD::TrigDecisionAuxInfo> trigDecAux = std::make_unique<xAOD::TrigDecisionAuxInfo>();
  trigDec->setStore(trigDecAux.get());

  trigDec->setSMK( m_trigConfigSvc->masterKey() );

  if (l1Result) {
    trigDec->setTAV(l1Result->itemsAfterVeto());
    trigDec->setTAP(l1Result->itemsAfterPrescale());
    trigDec->setTBP(l1Result->itemsBeforePrescale());
  }

  if (hltResult) {
    ATH_MSG_DEBUG("Got a DecisionContainer '" << m_HLTSummaryKeyIn.key() << "' of size " << hltResult->size());
    TrigCompositeUtils::DecisionIDContainer allPassedSet;
    // Expect only one object in this container, but allow many.
    // Accumulate in a set to preserve ordering and remove any duplicates
    for (const TrigCompositeUtils::Decision* decisionObject : *hltResult) {
      // Collect all decisions (IDs of passed chains) from decisionObject into allPassedSet
      TrigCompositeUtils::decisionIDs(decisionObject, allPassedSet);
    }
    if (allPassedSet.size()) {
      ++m_hltPassed;
    }
    std::vector<TrigCompositeUtils::DecisionID> toRecordPassed(allPassedSet.begin(), allPassedSet.end());
    trigDec->setChainMTPassedRaw(toRecordPassed);
  }

  // TODO prescaled

  // TODO resurrected

  // get the bunch crossing id
  const xAOD::EventInfo* eventInfo = SG::get(m_EventInfoKeyIn, context);
  const char bgByte = getBGByte(eventInfo->bcid());
  trigDec->setBGCode(bgByte);
  ATH_MSG_DEBUG ( "Run '" << eventInfo->runNumber()
                  << "'; Event '" << eventInfo->eventNumber()
                  << "'; BCID '" << eventInfo->bcid()
                  << "'; BG Code '" << (size_t)bgByte << "'" ) ;

  ATH_MSG_DEBUG ("Decision object dump: " << *(trigDec.get()));
  auto trigDecWriteHandle = SG::makeHandle( m_trigDecisionKeyOut, context );
  ATH_CHECK( trigDecWriteHandle.record( std::move( trigDec ), std::move( trigDecAux ) ) );
  ATH_MSG_DEBUG ("Recorded xAOD::TrigDecision to StoreGate with key = " << m_trigDecisionKeyOut.key());

  return StatusCode::SUCCESS;
}


StatusCode TrigDecisionMakerMT::getL1Result(const LVL1CTP::Lvl1Result*& result, const EventContext& context) const
{
  const ROIB::RoIBResult* roIBResult = SG::get(m_ROIBResultKeyIn, context);

  ATH_CHECK(m_lvl1Tool->updateItemsConfigOnly());

  if (roIBResult && (roIBResult->cTPResult()).isComplete()) {  
    m_lvl1Tool->createL1Items(*roIBResult, true);
    result = m_lvl1Tool->getLvl1Result();
    ATH_MSG_DEBUG ( "Built LVL1CTP::Lvl1Result from valid CTPResult.");
  }

  if (result == nullptr) {
    ATH_MSG_DEBUG ( "Could not construct L1 result from roIBResult");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigDecisionMakerMT::getHLTResult(const TrigCompositeUtils::DecisionContainer*& result, const EventContext& context) const
{
  result = SG::get(m_HLTSummaryKeyIn, context);
  return StatusCode::SUCCESS;
}


char TrigDecisionMakerMT::getBGByte(int BCId) const {
  const TrigConf::BunchGroupSet* bgs = m_trigConfigSvc->bunchGroupSet();
  if (!bgs) {
    ATH_MSG_WARNING ("Could not get BunchGroupSet to calculate BGByte");
    return 0;
  }
  if ((unsigned int)BCId >= bgs->bgPattern().size()) {
    ATH_MSG_WARNING ("Could not return BGCode for BCid " << BCId << ", since size of BGpattern is " <<  bgs->bgPattern().size() ) ;
    return 0;
  }
  return bgs->bgPattern()[BCId];  
}
