/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         TrigDecisionMaker.h
 **
 **   Description:  - Algorithm-derived class to run after the Steering to create the
 **                   transient TrigDecision object
 **
 **
 **
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
 **
 **   Created:      Tue May  09 14:55:56 GMT 2006
 **   Modified:
 **
 **************************************************************************/

#include "TrigDecisionMaker.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

#include "TrigSteeringEvent/Lvl1Result.h"

#include "TrigConfData/L1BunchGroupSet.h"


// all Trigger EDM ()


using namespace TrigDec;

TrigDecisionMaker::TrigDecisionMaker(const std::string &name, ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator),
    m_nEvents(0),
    m_l1_error(0), m_l2_error(0), m_ef_error(0), 
    m_hlt_error(0),
    m_td_error(0), m_td_skip(0),
    m_l1_notFound(0), m_l2_notFound(0), m_ef_notFound(0),
    m_hlt_notFound(0),
    m_l1_notReq(0), m_l2_notReq(0), m_ef_notReq(0),
    m_hlt_notReq(0),
    m_l1_passed(0), m_l2_passed(0), m_ef_passed(0),
    m_hlt_passed(0)
{
}


TrigDecisionMaker::~TrigDecisionMaker() {}


StatusCode TrigDecisionMaker::initialize()
{
  m_nEvents = 0;
  m_l1_error = 0;
  m_l2_error = 0;
  m_ef_error = 0;
  m_hlt_error = 0;
  m_td_error = 0;
  m_td_skip = 0;
  m_l1_notFound = 0;
  m_l2_notFound = 0;
  m_ef_notFound = 0;
  m_hlt_notFound = 0;
  m_l1_notReq = 0;
  m_l2_notReq = 0;
  m_ef_notReq = 0;
  m_hlt_notReq = 0;
  m_l1_passed = 0;
  m_l2_passed = 0;
  m_ef_passed = 0;
  m_hlt_passed = 0;

  ATH_MSG_DEBUG ( "Initializing TrigDecisionMaker..." ) ;
  ATH_MSG_DEBUG ( "Properties:" ) ;
  ATH_MSG_DEBUG ( " doL1            = " << (m_doL1 ? "True":"False") ) ;
  ATH_MSG_DEBUG ( " doL2            = " << (m_doL2 ? "True":"False") ) ;
  ATH_MSG_DEBUG ( " doEF            = " << (m_doEF ? "True":"False") ) ;
  ATH_MSG_DEBUG ( " doHLT           = " << (m_doHLT ? "True":"False") ) ;
  ATH_MSG_DEBUG ( " TrigDecisionKey = " << m_trigDecisionKey ) ;
  ATH_MSG_DEBUG ( " TrigL1ResultKey = " << m_l1ResultKey ) ;
  ATH_MSG_DEBUG ( " TrigROIBL1ResultKey = " << m_l1roibResultKey ) ;
  ATH_MSG_DEBUG ( " TrigL2ResultKey = " << m_l2ResultKey ) ;
  ATH_MSG_DEBUG ( " TrigEFResultKey = " << m_efResultKey ) ;
  ATH_MSG_DEBUG ( " TrigHLTResultKey= " << m_hltResultKey ) ;

  ATH_CHECK( m_lvl1Tool.retrieve() );

  ATH_CHECK( m_bgKey.initialize() );
  ATH_CHECK( m_HLTMenuKey.initialize() );
  ATH_CHECK( m_trigDecisionKey.initialize() );
  ATH_CHECK( m_l1ResultKey.initialize(m_doL1) );
  ATH_CHECK( m_l1roibResultKey.initialize(m_doL1) );
  ATH_CHECK( m_l2ResultKey.initialize(m_doL2) );
  ATH_CHECK( m_efResultKey.initialize(m_doEF) );
  ATH_CHECK( m_hltResultKey.initialize(m_doHLT) );

  return StatusCode::SUCCESS;
}



StatusCode TrigDecisionMaker::finalize()
{
  // print out stats: use also to do regression tests
  ATH_MSG_DEBUG ("=============================================" ) ;
  ATH_MSG_DEBUG ("REGTEST Run summary:"            ) ;
  ATH_MSG_DEBUG ("REGTEST  Events processed    : " << m_nEvents ) ;

  ATH_MSG_DEBUG ("REGTEST  Level 1  : passed        = " << m_l1_passed ) ;
  ATH_MSG_DEBUG ("REGTEST  Level 1  : not found     = " << m_l1_notFound ) ;
  ATH_MSG_DEBUG ("REGTEST  Level 1  : not requested = " << m_l1_notReq ) ;
  ATH_MSG_DEBUG ("REGTEST  Level 1  : SG errors     = " << m_l1_error ) ;

  ATH_MSG_DEBUG ("REGTEST  Level 2  : passed        = " << m_l2_passed ) ;
  ATH_MSG_DEBUG ("REGTEST  Level 2  : not found     = " << m_l2_notFound ) ;
  ATH_MSG_DEBUG ("REGTEST  Level 2  : not requested = " << m_l2_notReq ) ;
  ATH_MSG_DEBUG ("REGTEST  Level 2  : SG errors     = " << m_l2_error ) ;

  ATH_MSG_DEBUG ("REGTEST  EvFilter : passed        = " << m_ef_passed ) ;
  ATH_MSG_DEBUG ("REGTEST  EvFilter : not found     = " << m_ef_notFound ) ;
  ATH_MSG_DEBUG ("REGTEST  EvFilter : not requested = " << m_ef_notReq ) ;
  ATH_MSG_DEBUG ("REGTEST  EvFilter : SG errors     = " << m_ef_error ) ;

  ATH_MSG_DEBUG ("REGTEST  HLT : passed        = " << m_hlt_passed ) ;
  ATH_MSG_DEBUG ("REGTEST  HLT : not found     = " << m_hlt_notFound ) ;
  ATH_MSG_DEBUG ("REGTEST  HLT : not requested = " << m_hlt_notReq ) ;
  ATH_MSG_DEBUG ("REGTEST  HLT : SG errors     = " << m_hlt_error ) ;

  ATH_MSG_DEBUG ("REGTEST  TrigDecision not written : " << m_td_skip  ) ;
  ATH_MSG_DEBUG ("REGTEST  SG errors in storing TD  : " << m_td_error ) ;
  ATH_MSG_DEBUG ("=============================================" ) ;

  return StatusCode::SUCCESS;
}


StatusCode TrigDecisionMaker::execute(const EventContext& ctx) const
{
  // increment event counter
  m_nEvents++;

  // Retrieve the results
  const LVL1CTP::Lvl1Result* l1Result = 0;
  const HLT::HLTResult*   l2Result = 0;
  const HLT::HLTResult*   efResult = 0;
  const HLT::HLTResult*   hltResult = 0;

  ResultStatus l1Stat = getL1Result(l1Result, ctx);
  if (!l1Result) {
    if (l1Stat == NotRequested ) m_l1_notReq++;
    else if (l1Stat == SGError || l1Stat == ProcError)  m_l1_error++;
    else if (l1Stat == NotFound) m_l1_notFound++;
  }
  else if (l1Result->isAccepted()) m_l1_passed++;

  ResultStatus l2Stat = getHLTResult(l2Result, L2, ctx);
  if (!l2Result) {
    if (l2Stat == NotRequested ) m_l2_notReq++;
    else if (l2Stat == SGError || l2Stat == ProcError)  m_l2_error++;
    else if (l2Stat == NotFound) m_l2_notFound++;
  }
  else if (l2Result->isAccepted()) m_l2_passed++;

  ResultStatus efStat = getHLTResult(efResult, EF, ctx);
  if (!efResult) {
    if (efStat == NotRequested ) m_ef_notReq++;
    else if (efStat == SGError || efStat == ProcError)  m_ef_error++;
    else if (efStat == NotFound) m_ef_notFound++;
  }
  else if (efResult->isAccepted()) m_ef_passed++;


  ResultStatus hltStat = getHLTResult(hltResult, HLT, ctx);
  if (!hltResult) {
    if (hltStat == NotRequested ) m_hlt_notReq++;
    else if (hltStat == SGError || hltStat == ProcError)  m_hlt_error++;
    else if (hltStat == NotFound) m_hlt_notFound++;
  }
  else if (hltResult->isAccepted()) m_hlt_passed++;


  if (!l1Result && !l2Result && !efResult && !hltResult) {
    ATH_MSG_ERROR ("The whole trigger seems off for this event (no L1/L2/EF/HLT results) - no TrigDecision produced");
    m_td_skip++;
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<TrigDecision> trigDec = std::make_unique<TrigDecision>();

  SG::ReadHandle<TrigConf::HLTMenu> hltMenu(m_HLTMenuKey, ctx);
  ATH_CHECK( hltMenu.isValid() );
  trigDec->m_configMasterKey = hltMenu->smk();

  if (l1Result) trigDec->m_l1_result = *l1Result;
  if (l2Result) trigDec->m_l2_result = *l2Result;
  if (efResult) trigDec->m_ef_result = *efResult;
  if (hltResult){
    trigDec->m_ef_result = *hltResult;//store the merged result into ef_result to propagate with getEFResult
  }

  // get the bunch crossing id
  ATH_MSG_DEBUG ( "Run " << ctx.eventID().run_number()
                  << "; Event " << ctx.eventID().event_number()
                  << "; BC-ID " << ctx.eventID().bunch_crossing_id() ) ;
  SG::ReadCondHandle<TrigConf::L1BunchGroupSet> bgkey(m_bgKey, ctx);
  ATH_CHECK(bgkey.isValid());
  const TrigConf::L1BunchGroupSet* l1bgs = *bgkey;
  if (l1bgs) {
    // We currently only support 8 bits/bunchgroups (ATR-24030)
    trigDec->m_bgCode = static_cast<char>(l1bgs->bgPattern(ctx.eventID().bunch_crossing_id()));
  }
  else {
    ATH_MSG_WARNING("Could not read " << m_bgKey);
  }

  SG::WriteHandle<TrigDecision> writeHandle{m_trigDecisionKey, ctx};
  if (writeHandle.record(std::move(trigDec)).isFailure()) {
    ATH_MSG_ERROR ( "Failed to record TrigDecision to StoreGate with key "
                    << m_trigDecisionKey << "!" ) ;

    m_td_error++;
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ( "Recorded TrigDecision to StoreGate with key = "
                  << m_trigDecisionKey << "." ) ;
  

  return StatusCode::SUCCESS;
}

TrigDecisionMaker::ResultStatus TrigDecisionMaker::getL1Result(const LVL1CTP::Lvl1Result*& result, const EventContext& ctx) const
{
  result = 0;
  if (!m_doL1) return NotRequested;

  SG::ReadHandle<LVL1CTP::Lvl1Result> l1RH{m_l1ResultKey, ctx};
  if (l1RH.isValid()) {
    result = l1RH.cptr();
    return OK;
  }

  SG::ReadHandle<ROIB::RoIBResult> l1roibRH{m_l1roibResultKey, ctx};
  if (!l1roibRH.isValid()) {
    result = nullptr;
    ATH_MSG_WARNING ( "Trying to do L1, but RoIBResult not found" ) ;
    return NotFound;
  }

  const ROIB::RoIBResult* roIBResult = l1roibRH.cptr();

  ATH_MSG_DEBUG ( "Got ROIBResult from StoreGate with key " << m_l1roibResultKey ) ;

  std::vector< std::unique_ptr<LVL1CTP::Lvl1Item> > itemConfig = m_lvl1Tool->makeLvl1ItemConfig(ctx);

  if ((roIBResult->cTPResult()).isComplete()) {  
    m_lvl1Tool->createL1Items(itemConfig, *roIBResult,&result);
    ATH_MSG_DEBUG ( "Build LVL1CTP::Lvl1Result from valid CTPResult.") ;
  } else {
    ATH_MSG_DEBUG ( "No LVL1CTP::Lvl1Result build since no valid CTPResult is available.") ;
  }

  return OK;
}


TrigDecisionMaker::ResultStatus TrigDecisionMaker::getHLTResult(const HLT::HLTResult*& result,
                                                                TrigLevel level, const EventContext& ctx) const
{
  result = 0;

  if (level != L2 && level != EF  && level != HLT) {
    ATH_MSG_ERROR ("Level must be either L2 or EF or HLT in getHLTResult!");
    return Unknown;
  }

  if ((level == L2 && !m_doL2) || (level == EF && !m_doEF) || (level == HLT && !m_doHLT)) return NotRequested;
  
  const SG::ReadHandleKey<HLT::HLTResult>& key = (level == L2 ? m_l2ResultKey : (level == EF) ? m_efResultKey : m_hltResultKey);

  SG::ReadHandle<HLT::HLTResult> hltRH{key, ctx};

  if (!hltRH.isValid()) {
    ATH_MSG_ERROR ( "Error retrieving HLTResult from StoreGate" ) ;
    result = nullptr;
    return SGError;
  }

  result = hltRH.cptr();

  ATH_MSG_DEBUG ( "Got HLTResult from StoreGate with key " << key ) ;

  return OK;
}
