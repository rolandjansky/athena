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

#include "TrigSteeringEvent/OnlineErrorCode.h"
#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteering/Lvl1ResultAccessTool.h"

#include "xAODTrigger/TrigDecisionAuxInfo.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTUtils.h"

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

  bool resultObjectIsUsed = true;
  if (!m_bitsMakerTool.empty()) {
    ATH_MSG_INFO("TrigDecisionMakerMT is setting to run after the Trigger in a job with POOL output. "
      "The TriggerBitsMakerTool will be used directly to create the xAOD::TrigDecision.");
    ATH_CHECK( m_bitsMakerTool.retrieve() );
    resultObjectIsUsed = false;
  } else {
    ATH_MSG_INFO("TrigDecisionMakerMT is setting up to read the trigger bits from trigger bytestream. "
      "The HLTResultMT object will be used as the source of the trigger bits.");
  }
  ATH_CHECK( m_hltResultKeyIn.initialize(resultObjectIsUsed) ); // If false, this removes the ReadHandle

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

  std::unique_ptr<xAOD::TrigDecision>        trigDec    = std::make_unique<xAOD::TrigDecision>();
  std::unique_ptr<xAOD::TrigDecisionAuxInfo> trigDecAux = std::make_unique<xAOD::TrigDecisionAuxInfo>();
  trigDec->setStore(trigDecAux.get());

  trigDec->setSMK( m_trigConfigSvc->masterKey() );

  if (m_doL1) {
    const LVL1CTP::Lvl1Result* l1Result = nullptr;
    ATH_CHECK(getL1Result(l1Result, context));

    trigDec->setTAV(l1Result->itemsAfterVeto());
    trigDec->setTAP(l1Result->itemsAfterPrescale());
    trigDec->setTBP(l1Result->itemsBeforePrescale());

    if (l1Result->isAccepted()) {
      ++m_l1Passed;
    }
  }

  if (m_doHLT) {

    boost::dynamic_bitset<uint32_t> passRawBitset, prescaledBitset, rerunBitset;

    if (m_bitsMakerTool.isSet()) {

      ATH_MSG_DEBUG ("MC Mode: Creating bits with TriggerBitsMakerTool");
      ATH_CHECK(m_bitsMakerTool->getBits(passRawBitset, prescaledBitset, rerunBitset, context));

     } else {

      ATH_MSG_DEBUG ("Data Mode: Reading bits from HLTResultMT");
      SG::ReadHandle<HLT::HLTResultMT> hltResult = SG::makeHandle<HLT::HLTResultMT>(m_hltResultKeyIn, context);
      ATH_CHECK(hltResult.isValid());

      passRawBitset = hltResult->getHltPassRawBits();
      prescaledBitset = hltResult->getHltPrescaledBits();
      rerunBitset = hltResult->getHltRerunBits();

      const std::vector<HLT::OnlineErrorCode> errorCodes = hltResult->getErrorCodes();
      bool truncated = false;
      uint32_t code = 0;
      for (size_t i = 0; i < errorCodes.size(); ++i) {
        truncated |= (errorCodes.at(i) == HLT::OnlineErrorCode::RESULT_TRUNCATION);
        if (i == 0) {
          code = static_cast<uint32_t>(errorCodes.at(i));
        }
      }
      trigDec->setEFErrorBits(code);
      trigDec->setEFTruncated(truncated);

    }

    ATH_MSG_DEBUG ("Number of HLT chains passed raw: " << passRawBitset.count());
    ATH_MSG_DEBUG ("Number of HLT chains prescaled out: " << prescaledBitset.count());
    ATH_MSG_DEBUG ("Number of HLT chains in rerun / second pass / resurrection : " << rerunBitset.count());

    if (passRawBitset.any()) {
      ++m_hltPassed;
    }

    std::vector<uint32_t> passRaw, prescaled, rerun;

    passRaw.resize(passRawBitset.num_blocks());
    prescaled.resize(prescaledBitset.num_blocks());
    rerun.resize(rerunBitset.num_blocks());

    boost::to_block_range(passRawBitset, passRaw.begin());
    boost::to_block_range(prescaledBitset, prescaled.begin());
    boost::to_block_range(rerunBitset, rerun.begin());

    if (passRaw.size() != prescaled.size() or passRaw.size() != rerun.size()) {
      ATH_MSG_ERROR("Trigger bitsets are not all the same size! passRaw:" 
        << passRaw.size() << " prescaled:" << prescaled.size() << " rerun:" << rerun.size() );
      return StatusCode::FAILURE;
    } 

    trigDec->setEFPassedRaw(passRaw);
    trigDec->setEFPrescaled(prescaled);
    trigDec->setEFResurrected(rerun);

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
  SG::ReadHandle<ROIB::RoIBResult> roIBResult = SG::makeHandle<ROIB::RoIBResult>(m_ROIBResultKeyIn, context);
  ATH_CHECK(roIBResult.isValid());

  std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> > itemConfig = m_lvl1Tool->makeLvl1ItemConfig();

  if (roIBResult->cTPResult().isComplete()) {
    m_lvl1Tool->createL1Items(itemConfig, *roIBResult, &result);
    ATH_MSG_DEBUG ( "Built LVL1CTP::Lvl1Result from valid CTPResult.");
  }

  if (result == nullptr) {
    ATH_MSG_ERROR ( "Could not construct L1 result from roIBResult");
    return StatusCode::FAILURE;
  }

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
