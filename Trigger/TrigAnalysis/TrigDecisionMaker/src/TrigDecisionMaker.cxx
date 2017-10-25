/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "TrigT1Result/RoIBResult.h"

#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigSteering/Lvl1ResultAccessTool.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfL1Data/BunchGroupSet.h"


#include "GaudiKernel/Incident.h"
// all Trigger EDM ()


using namespace TrigDec;

TrigDecisionMaker::TrigDecisionMaker(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_trigConfigSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_lvl1Tool("HLT::Lvl1ResultAccessTool/Lvl1ResultAccessTool", this),
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
  declareProperty("TrigConfigSvc", m_trigConfigSvc, "Trigger config service");

  declareProperty("doL1", m_doL1 = true);
  declareProperty("doL2", m_doL2 = true);
  declareProperty("doEF", m_doEF = true);
  declareProperty("doHLT", m_doHLT = true);
//  declareProperty("doEvtInfo", m_doEvtInfo = false);

  declareProperty("TrigDecisionKey", m_trigDecisionKey = "TrigDecision");
  declareProperty("L1ResultKey",     m_l1ResultKey = "");
  declareProperty("L2ResultKey",     m_l2ResultKey = "HLTResult_L2");
  declareProperty("EFResultKey",     m_efResultKey = "HLTResult_EF");
  declareProperty("HLTResultKey",    m_hltResultKey = "HLTResult_HLT");
//  declareProperty("EventInfoKey",    m_evtInfoKey  = "McEventInfo");
//  declareProperty("TrigDecisionTool", m_trigDec, "The tool to access TrigDecision");
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
  ATH_MSG_DEBUG ( " TrigL2ResultKey = " << m_l2ResultKey ) ;
  ATH_MSG_DEBUG ( " TrigEFResultKey = " << m_efResultKey ) ;
  ATH_MSG_DEBUG ( " TrigHLTResultKey= " << m_hltResultKey ) ;

  ATH_CHECK( m_lvl1Tool.retrieve() );
  ATH_CHECK( m_trigConfigSvc.retrieve() );

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


StatusCode TrigDecisionMaker::execute()
{
  // increment event counter
  m_nEvents++;

  // Retrieve the results
  const LVL1CTP::Lvl1Result* l1Result = 0;
  const HLT::HLTResult*   l2Result = 0;
  const HLT::HLTResult*   efResult = 0;
  const HLT::HLTResult*   hltResult = 0;

  ResultStatus l1Stat = getL1Result(l1Result);
  if (!l1Result) {
    if (l1Stat == NotRequested ) m_l1_notReq++;
    else if (l1Stat == SGError || l1Stat == ProcError)  m_l1_error++;
    else if (l1Stat == NotFound) m_l1_notFound++;
  }
  else if (l1Result->isAccepted()) m_l1_passed++;

  ResultStatus l2Stat = getHLTResult(l2Result, L2);
  if (!l2Result) {
    if (l2Stat == NotRequested ) m_l2_notReq++;
    else if (l2Stat == SGError || l2Stat == ProcError)  m_l2_error++;
    else if (l2Stat == NotFound) m_l2_notFound++;
  }
  else if (l2Result->isAccepted()) m_l2_passed++;

  ResultStatus efStat = getHLTResult(efResult, EF);
  if (!efResult) {
    if (efStat == NotRequested ) m_ef_notReq++;
    else if (efStat == SGError || efStat == ProcError)  m_ef_error++;
    else if (efStat == NotFound) m_ef_notFound++;
  }
  else if (efResult->isAccepted()) m_ef_passed++;


  ResultStatus hltStat = getHLTResult(hltResult, HLT);
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

  TrigDecision* trigDec = new TrigDecision();

  trigDec->m_configMasterKey = m_trigConfigSvc->masterKey();

  if (l1Result) trigDec->m_l1_result = *l1Result;
  if (l2Result) trigDec->m_l2_result = *l2Result;
  if (efResult) trigDec->m_ef_result = *efResult;
  if (hltResult){
    trigDec->m_ef_result = *hltResult;//store the merged result into ef_result to propagate with getEFResult
  }
  //  std::cout << "Acc state: " << l2Result->isAccepted() << " " <<  trigDec->m_l2_result.isAccepted() << std::endl;


  // get the bunch crossing id
  const EventInfo* eventInfo = 0;  
  if ( evtStore()->retrieve(eventInfo).isFailure() ) {
    ATH_MSG_WARNING ( "Failed to retrieve event info" ) ;
  } else {
    EventID* myEventID = eventInfo->event_ID();
    ATH_MSG_DEBUG ( "Run " << myEventID->run_number()
                    << "; Event " << myEventID->event_number()
                    << "; BC-ID " << myEventID->bunch_crossing_id() ) ;
    char x = getBGByte(myEventID->bunch_crossing_id());
    trigDec->m_bgCode = x;
  }
  

  std::string tdKey = updatedDecisionKey();
  StatusCode sc = evtStore()->record(trigDec, tdKey, true);

  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "Failed to record TrigDecision to StoreGate with key "
                    << tdKey << "!" ) ;

    m_td_error++;
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG ( "Recorded TrigDecision to StoreGate with key = "
                  << tdKey << "." ) ;
  

  return StatusCode::SUCCESS;
}

TrigDecisionMaker::ResultStatus TrigDecisionMaker::getL1Result(const LVL1CTP::Lvl1Result*& result)
{
  result = 0;
  if (!m_doL1) return NotRequested;

  if ( evtStore()->contains<LVL1CTP::Lvl1Result>("Lvl1Result") ) {
    if ( evtStore()->retrieve(result, "Lvl1Result").isFailure() ) {
      return SGError;
    }
    return OK;
  }

  if (!evtStore()->contains<ROIB::RoIBResult>(m_l1ResultKey)) {
    ATH_MSG_WARNING ( "Trying to do L1, but RoIBResult not found" ) ;
    return NotFound;
  }

  const ROIB::RoIBResult* roIBResult = 0;

  StatusCode sc = evtStore()->retrieve(roIBResult, m_l1ResultKey);

  if (sc.isFailure() || !roIBResult) {
    ATH_MSG_ERROR ( "Error retrieving RoIBResult from StoreGate" ) ;
    result = 0;
    return SGError;
  }

  ATH_MSG_DEBUG ( "Got ROIBResult from StoreGate with key " << m_l1ResultKey ) ;

  sc = m_lvl1Tool->updateItemsConfigOnly();
  if (sc.isFailure()) return ProcError;

  if ((roIBResult->cTPResult()).isComplete()) {  
    m_lvl1Tool->createL1Items(*roIBResult,true);
    result = m_lvl1Tool->getLvl1Result();
    ATH_MSG_DEBUG ( "Build LVL1CTP::Lvl1Result from valid CTPResult.") ;
  } else {
    ATH_MSG_DEBUG ( "No LVL1CTP::Lvl1Result build since no valid CTPResult is available.") ;
  }

  return OK;
}


TrigDecisionMaker::ResultStatus TrigDecisionMaker::getHLTResult(const HLT::HLTResult*& result,
                                                                TrigLevel level)
{
  result = 0;

  if (level != L2 && level != EF  && level != HLT) {
    ATH_MSG_ERROR ("Level must be either L2 or EF or HLT in getHLTResult!");
    return Unknown;
  }

  if ((level == L2 && !m_doL2) || (level == EF && !m_doEF) || (level == HLT && !m_doHLT)) return NotRequested;
  
  const std::string& key = (level == L2 ? m_l2ResultKey : (level == EF) ? m_efResultKey : m_hltResultKey);

  if (!evtStore()->contains<HLT::HLTResult>(key)) {
    ATH_MSG_WARNING ( "Trying to get HLT result, but not found with key "
                      << key ) ;
    return NotFound;
  }

  StatusCode sc = evtStore()->retrieve(result, key);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "Error retrieving HLTResult from StoreGate" ) ;
    result = 0;
    return SGError;
  }

  ATH_MSG_DEBUG ( "Got HLTResult from StoreGate with key " << key ) ;

  return OK;
}


std::string TrigDecisionMaker::updatedDecisionKey()
{
  std::string key = m_trigDecisionKey;
  bool alreadyThere = true;

  while (alreadyThere) {
    if (evtStore()->contains<TrigDecision>( key )) {
      if (outputLevel() <= MSG::WARNING)
        ATH_MSG_WARNING ( key << " already exists: "
                          << " using new key " << key << "+." ) ;
      key += "+";
    }
    else alreadyThere = false;
  }

  return key;
}


char TrigDecisionMaker::getBGByte(int BCId) {

   const TrigConf::BunchGroupSet* bgs = m_trigConfigSvc->bunchGroupSet();
   if(!bgs) {
     ATH_MSG_WARNING ( " Could not get BunchGroupSet to calculate BGByte" ) ;
      return 0;
   }
   
   //   if(bgs->bunchGroups().size()!=8) {
   //     (*m_log) << MSG::WARNING << " Did not find 8 bunchgroups in the set (actual number of BGs is " 
   //              << bgs->bunchGroups().size()
   //              << ")" << endreq;
   //     return 0;
   //   }
   
   if((unsigned int)BCId>=bgs->bgPattern().size()) {
     ATH_MSG_WARNING ( " Could not return BGCode for BCid " << BCId << ", since size of BGpattern is " <<  bgs->bgPattern().size() ) ;
      return 0;
   }

   return bgs->bgPattern()[BCId];  
}
