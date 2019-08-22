/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "GaudiKernel/Incident.h"

#include <boost/dynamic_bitset.hpp>

using namespace TrigDec;

TrigDecisionMakerMT::TrigDecisionMakerMT(const std::string &name, ISvcLocator *pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator),
    m_trigConfigSvc("", name),
    m_lvl1Tool("", this)
{}

TrigDecisionMakerMT::~TrigDecisionMakerMT() {}

StatusCode TrigDecisionMakerMT::initialize()
{
  ATH_CHECK( m_hltResultKeyIn.initialize() );
  ATH_CHECK( m_ROIBResultKeyIn.initialize() );
  ATH_CHECK( m_EventInfoKeyIn.initialize() );

  ATH_CHECK( m_trigDecisionKeyOut.initialize() );

  m_lvl1Tool.setTypeAndName(m_lvl1ToolLocation);
  ATH_CHECK( m_lvl1Tool.retrieve() );
  m_trigConfigSvc.setTypeAndName(m_trigConfigLocation);
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


StatusCode TrigDecisionMakerMT::execute(const EventContext& context) const 
{
  using namespace TrigCompositeUtils;

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

  const HLT::HLTResultMT* hltResult = nullptr;
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

    std::vector<uint32_t> hltPassRawBits;
    std::vector<uint32_t> hltPrescaledBits;
    std::vector<uint32_t> hltRerunBits;

    hltPassRawBits.resize(hltResult->getHltPassRawBits().num_blocks());
    hltPrescaledBits.resize(hltResult->getHltPrescaledBits().num_blocks());
    hltRerunBits.resize(hltResult->getHltRerunBits().num_blocks());

    boost::to_block_range(hltResult->getHltPassRawBits(),hltPassRawBits.begin());
    boost::to_block_range(hltResult->getHltPrescaledBits(),hltPrescaledBits.begin());
    boost::to_block_range(hltResult->getHltRerunBits(),hltRerunBits.begin());

    ATH_MSG_DEBUG ("Number of HLT chains passed raw: " << hltResult->getHltPassRawBits().count());
    ATH_MSG_DEBUG ("Number of HLT chains prescaled out: " << hltResult->getHltPrescaledBits().count());
    ATH_MSG_DEBUG ("Number of HLT chains in rerun / second pass / resurrection : " << hltResult->getHltRerunBits().count());

    trigDec->setEFPassedRaw(hltPassRawBits);
    trigDec->setEFPrescaled(hltPrescaledBits);
    trigDec->setEFResurrected(hltRerunBits);

    if (hltResult->getHltPassRawBits().any()) {
      ++m_hltPassed;
    }

  }

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

  std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> > itemConfig = m_lvl1Tool->makeLvl1ItemConfig();

  if (roIBResult && (roIBResult->cTPResult()).isComplete()) {  
    m_lvl1Tool->createL1Items(itemConfig, *roIBResult, &result);
    ATH_MSG_DEBUG ( "Built LVL1CTP::Lvl1Result from valid CTPResult.");
  }

  if (result == nullptr) {
    ATH_MSG_DEBUG ( "Could not construct L1 result from roIBResult");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigDecisionMakerMT::getHLTResult(const HLT::HLTResultMT*& result, const EventContext& context) const
{
  result = SG::get(m_hltResultKeyIn, context);
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
