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
  : Algorithm(name, pSvcLocator),
    m_log(0),
    m_trigConfigSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_lvl1Tool("HLT::Lvl1ResultAccessTool/Lvl1ResultAccessTool", this),
    m_storeGate("StoreGateSvc", name),
    m_nEvents(0),
    m_l1_error(0), m_l2_error(0), m_ef_error(0), m_td_error(0), m_td_skip(0),
    m_l1_notFound(0), m_l2_notFound(0), m_ef_notFound(0),
    m_l1_notReq(0), m_l2_notReq(0), m_ef_notReq(0),
    m_l1_passed(0), m_l2_passed(0), m_ef_passed(0)
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
  declareProperty("EvtStore", m_storeGate);
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

  // get message service and print out properties
  m_log = new MsgStream(messageService(), name());

  StatusCode sc;

  if (outputLevel() <= MSG::DEBUG) {
    (*m_log) << MSG::DEBUG << "Initializing TrigDecisionMaker..." << endreq;
    (*m_log) << MSG::DEBUG << "Properties:" << endreq;
    (*m_log) << MSG::DEBUG << " doL1            = " << (m_doL1 ? "True":"False") << endreq;
    (*m_log) << MSG::DEBUG << " doL2            = " << (m_doL2 ? "True":"False") << endreq;
    (*m_log) << MSG::DEBUG << " doEF            = " << (m_doEF ? "True":"False") << endreq;
    (*m_log) << MSG::DEBUG << " doHLT           = " << (m_doHLT ? "True":"False") << endreq;
    (*m_log) << MSG::DEBUG << " TrigDecisionKey = " << m_trigDecisionKey << endreq;
    (*m_log) << MSG::DEBUG << " TrigL1ResultKey = " << m_l1ResultKey << endreq;
    (*m_log) << MSG::DEBUG << " TrigL2ResultKey = " << m_l2ResultKey << endreq;
    (*m_log) << MSG::DEBUG << " TrigEFResultKey = " << m_efResultKey << endreq;
    (*m_log) << MSG::DEBUG << " TrigHLTResultKey= " << m_hltResultKey << endreq;
  }

  // get StoreGate
  sc = m_storeGate.retrieve();

  if( sc.isFailure() ) {
    if (outputLevel() <= MSG::ERROR) {
      (*m_log) << MSG::ERROR << "Unable to locate Service StoreGate!" << endreq;
    }
    return sc;
  }

  sc = m_lvl1Tool.retrieve();
  if ( sc.isFailure() ) {
    (*m_log) << MSG::ERROR << "Unable to retrieve lvl1 result access tool: " << m_lvl1Tool << endreq;
    return sc;
  }

  sc = m_trigConfigSvc.retrieve();
  if ( sc.isFailure() ) {
    (*m_log) << MSG::ERROR << "Unable to retrieve trigger config service " << m_trigConfigSvc << endreq;
    return sc;
  }


  return StatusCode::SUCCESS;
}



StatusCode TrigDecisionMaker::finalize()
{
  // print out stats: use also to do regression tests
  if (outputLevel() <= MSG::DEBUG) {
    (*m_log) <<MSG::DEBUG <<"=============================================" <<endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST Run summary:"            << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  Events processed    : " << m_nEvents << endreq;

    (*m_log) <<MSG::DEBUG <<"REGTEST  Level 1  : passed        = " << m_l1_passed << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  Level 1  : not found     = " << m_l1_notFound << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  Level 1  : not requested = " << m_l1_notReq << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  Level 1  : SG errors     = " << m_l1_error << endreq;

    (*m_log) <<MSG::DEBUG <<"REGTEST  Level 2  : passed        = " << m_l2_passed << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  Level 2  : not found     = " << m_l2_notFound << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  Level 2  : not requested = " << m_l2_notReq << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  Level 2  : SG errors     = " << m_l2_error << endreq;

    (*m_log) <<MSG::DEBUG <<"REGTEST  EvFilter : passed        = " << m_ef_passed << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  EvFilter : not found     = " << m_ef_notFound << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  EvFilter : not requested = " << m_ef_notReq << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  EvFilter : SG errors     = " << m_ef_error << endreq;

    (*m_log) <<MSG::DEBUG <<"REGTEST  HLT : passed        = " << m_hlt_passed << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  HLT : not found     = " << m_hlt_notFound << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  HLT : not requested = " << m_hlt_notReq << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  HLT : SG errors     = " << m_hlt_error << endreq;

    (*m_log) <<MSG::DEBUG <<"REGTEST  TrigDecision not written : " << m_td_skip  << endreq;
    (*m_log) <<MSG::DEBUG <<"REGTEST  SG errors in storing TD  : " << m_td_error << endreq;
    (*m_log) <<MSG::DEBUG <<"=============================================" <<endreq;
  }

  delete m_log; m_log = 0;

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
    (*m_log) << "The whole trigger seems off for this event (no L1/L2/EF/HLT results) - no TrigDecision produced" << endreq;
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
  if ( m_storeGate->retrieve(eventInfo).isFailure() ) {
    if (outputLevel() <= MSG::WARNING) {
      (*m_log) << MSG::WARNING << "Failed to retrieve event info"
               << endreq;
    }
  } else {
    EventID* myEventID = eventInfo->event_ID();
    if (outputLevel() <= MSG::DEBUG) {
      (*m_log) << MSG::DEBUG << "Run " << myEventID->run_number()
               << "; Event " << myEventID->event_number()
               << "; BC-ID " << myEventID->bunch_crossing_id()
               << endreq;
    }
    char x = getBGByte(myEventID->bunch_crossing_id());
    trigDec->m_bgCode = x;
  }
  

  std::string tdKey = updatedDecisionKey();
  StatusCode sc = m_storeGate->record(trigDec, tdKey, true);

  if (sc.isFailure()) {
    if (outputLevel() <= MSG::ERROR) {
      (*m_log) << MSG::ERROR << "Failed to record TrigDecision to StoreGate with key " << tdKey << "!"
	       << endreq;
    }

    m_td_error++;
    return StatusCode::FAILURE;
  }

  (*m_log) << MSG::DEBUG << "Recorded TrigDecision to StoreGate with key = "
      << tdKey << "." << endreq;
  

  return StatusCode::SUCCESS;
}

TrigDecisionMaker::ResultStatus TrigDecisionMaker::getL1Result(const LVL1CTP::Lvl1Result*& result)
{
  result = 0;
  if (!m_doL1) return NotRequested;

  if ( m_storeGate->contains<LVL1CTP::Lvl1Result>("Lvl1Result") ) {
    if ( m_storeGate->retrieve(result, "Lvl1Result").isFailure() ) {
      return SGError;
    }
    return OK;
  }


  if (!m_storeGate->contains<ROIB::RoIBResult>(m_l1ResultKey)) {
    (*m_log) << MSG::WARNING << "Trying to do L1, but RoIBResult not found" << endreq;
    return NotFound;
  }

  const ROIB::RoIBResult* roIBResult = 0;

  StatusCode sc = m_storeGate->retrieve(roIBResult, m_l1ResultKey);

  if (sc.isFailure() || !roIBResult) {
    (*m_log) << MSG::ERROR << "Error retrieving RoIBResult from StoreGate" << endreq;

    result = 0;
    return SGError;
  }

  sc = m_lvl1Tool->updateItemsConfigOnly();
  if (sc.isFailure()) return ProcError;

  m_lvl1Tool->createL1Items(*roIBResult, true);
  result = m_lvl1Tool->getLvl1Result();

  (*m_log) << MSG::DEBUG << "Got ROIBResult from StoreGate with key "
           << m_l1ResultKey << endreq;

  return OK;
}



TrigDecisionMaker::ResultStatus TrigDecisionMaker::getHLTResult(const HLT::HLTResult*& result,
                                                                TrigLevel level)
{
  result = 0;

  if (level != L2 && level != EF  && level != HLT) {
    (*m_log) << "Level must be either L2 or EF or HLT in getHLTResult!" << endreq;
    return Unknown;
  }

  if ((level == L2 && !m_doL2) || (level == EF && !m_doEF) || (level == HLT && !m_doHLT)) return NotRequested;
  
  const std::string& key = (level == L2 ? m_l2ResultKey : (level == EF) ? m_efResultKey : m_hltResultKey);

  if (!m_storeGate->contains<HLT::HLTResult>(key)) {
    (*m_log) << MSG::WARNING << "Trying to get HLT result, but not found with key "
             << key << endreq;
    return NotFound;
  }

  StatusCode sc = m_storeGate->retrieve(result, key);
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Error retrieving HLTResult from StoreGate" << endreq;
    result = 0;
    return SGError;
  }

  (*m_log) << MSG::DEBUG << "Got HLTResult from StoreGate with key " << key << endreq;

  return OK;
}


std::string TrigDecisionMaker::updatedDecisionKey()
{
  std::string key = m_trigDecisionKey;
  bool alreadyThere = true;

  while (alreadyThere) {
    if (m_storeGate->contains<TrigDecision>( key )) {
      if (outputLevel() <= MSG::WARNING)
        (*m_log) << MSG::WARNING << key << " already exists: "
                 << " using new key " << key << "+." << endreq;
      key += "+";
    }
    else alreadyThere = false;
  }

  return key;
}


char TrigDecisionMaker::getBGByte(int BCId) {

   const TrigConf::BunchGroupSet* bgs = m_trigConfigSvc->bunchGroupSet();
   if(!bgs) {
      (*m_log) << MSG::WARNING << " Could not get BunchGroupSet to calculate BGByte" << endreq;
      return 0;
   }
   
   //   if(bgs->bunchGroups().size()!=8) {
   //     (*m_log) << MSG::WARNING << " Did not find 8 bunchgroups in the set (actual number of BGs is " 
   //              << bgs->bunchGroups().size()
   //              << ")" << endreq;
   //     return 0;
   //   }
   
   if((unsigned int)BCId>=bgs->bgPattern().size()) {
      (*m_log) << MSG::WARNING << " Could not return BGCode for BCid " << BCId << ", since size of BGpattern is " <<  bgs->bgPattern().size() << endreq;
      return 0;
   }

   return bgs->bgPattern()[BCId];  
}
