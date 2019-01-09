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
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "GaudiKernel/Incident.h"

using namespace TrigDec;

TrigDecisionMakerMT::TrigDecisionMakerMT(const std::string &name, ISvcLocator *pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator),
    m_trigConfigSvc("", name),
    m_lvl1Tool("", this)
{}

TrigDecisionMakerMT::~TrigDecisionMakerMT() {}

StatusCode TrigDecisionMakerMT::initialize()
{
  ATH_CHECK( m_HLTSummaryKeyIn.initialize() );
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

  // Output bitsets (stored in a vector<uint32_t>)
  std::vector<uint32_t> hltPassBits;
  std::vector<uint32_t> hltPrescaledBits;
  std::vector<uint32_t> hltRerunBits;

  std::set< std::vector<uint32_t>* > outputVectors;
  outputVectors.insert( &hltPassBits );
  outputVectors.insert( &hltPrescaledBits );
  outputVectors.insert( &hltRerunBits );

  if (hltResult) {
    ATH_MSG_DEBUG("Got a DecisionContainer '" << m_HLTSummaryKeyIn.key() << "' of size " << hltResult->size() << " (typically expect size=3)");
    TrigCompositeUtils::DecisionIDContainer passRawInput; //!< The chains which returned a positive decision
    TrigCompositeUtils::DecisionIDContainer prescaledInput; //!< The chains which did not run due to being prescaled out
    TrigCompositeUtils::DecisionIDContainer rerunInput; //!< The chains which were activate only in the rerun (not physics decisions)

    // Read the sets of chain IDs
    for (const TrigCompositeUtils::Decision* decisionObject : *hltResult) {
      // Collect all decisions (IDs of passed/prescaled/rerun chains) from named decisionObjects
      if (decisionObject->name() == "HLTPassRaw") {
        TrigCompositeUtils::decisionIDs(decisionObject, passRawInput);
      } else if (decisionObject->name() == "HLTPrescaled") {
        TrigCompositeUtils::decisionIDs(decisionObject, prescaledInput);
      } else if (decisionObject->name() == "HLTRerun") {
        TrigCompositeUtils::decisionIDs(decisionObject, rerunInput);
      } else {
        ATH_MSG_WARNING("TrigDecisionMakerMT encountered an unknown set of decisions with name '" << decisionObject->name() << "'");
      }
    }
    if (passRawInput.size()) {
      ++m_hltPassed;
    }
  
    // Make bitmap for passed raw
    size_t countHltPass = makeBitMap(passRawInput, hltPassBits, outputVectors);
    ATH_MSG_DEBUG ("Number of HLT chains passed raw: " << countHltPass);
    trigDec->setEFPassedRaw(hltPassBits);

    // Make bitmap for prescaled
    size_t countHltPrescaled = makeBitMap(prescaledInput, hltPrescaledBits, outputVectors);
    ATH_MSG_DEBUG ("Number of HLT chains prescaled out: " << countHltPrescaled);
    trigDec->setEFPrescaled(hltPrescaledBits);

    // Make bitmap for rerun a.k.a. resurrection
    size_t countHLTRerun = makeBitMap(rerunInput, hltRerunBits, outputVectors);
    ATH_MSG_DEBUG ("Number of HLT chains in rerun / second pass / resurrection : " << countHLTRerun);
    trigDec->setEFResurrected(hltRerunBits);
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


size_t TrigDecisionMakerMT::makeBitMap(
  const TrigCompositeUtils::DecisionIDContainer& passedIDs,
  std::vector<uint32_t>& bitsVector,
  std::set< std::vector<uint32_t>* >& allOutputVectors) const
{
  size_t count = 0;
  for (const TrigCompositeUtils::DecisionID id : passedIDs) {
    const int32_t chainCounter = getChainCounter(id);
    if (chainCounter == -1) continue; // Could not decode, prints error
    resizeVectors(chainCounter, allOutputVectors); // Make sure we have enough room to be able to set the required bit
    setBit(chainCounter, bitsVector);
    ++count;
  }
  return count;
}


void TrigDecisionMakerMT::resizeVectors(const size_t bit, const std::set< std::vector<uint32_t>* >& vectors) const {
  const size_t block = bit / std::numeric_limits<uint32_t>::digits;
  const size_t requiredSize = block + 1;
  for (std::vector<uint32_t>* vecPtr : vectors) {
    vecPtr->resize(requiredSize, 0);
  }
  return;
}


void TrigDecisionMakerMT::setBit(const size_t bit, std::vector<uint32_t>& bits) const {
  const size_t block = bit / std::numeric_limits<uint32_t>::digits; // = 32
  const size_t offset = bit % std::numeric_limits<uint32_t>::digits;
  bits.at(block) |= static_cast<uint32_t>(1) << offset;
}


int32_t TrigDecisionMakerMT::getChainCounter(const TrigCompositeUtils::DecisionID chainID) const {
  // Need to go from hash-ID to chain-counter. HLTChain counter currently does not give this a category
  const std::string chainName = TrigConf::HLTUtils::hash2string(chainID);
  if (chainName == "UNKNOWN HASH ID" || chainName == "UNKNOWN CATEGORY") {
    ATH_MSG_ERROR("Unable to locate chain with hashID:" << chainID << " in the TrigConf, the error reported was: " << chainName);
    return -1;
  }
  const TrigConf::HLTChain* chain = m_trigConfigSvc->chains().chain(chainName);
  if (chain == nullptr) {
    ATH_MSG_ERROR("Unable to fetch HLTChain object for chain with hashID:" << chainID << " and name:'" << chainName << "' (number of chains:" << m_trigConfigSvc->chains().size() << ")");
    return -1;        
  }
  return chain->chain_counter();
}
