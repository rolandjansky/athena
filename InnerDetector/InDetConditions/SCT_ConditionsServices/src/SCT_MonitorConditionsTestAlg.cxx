/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MonitorConditionsTestAlg.cxx
 *
 * @brief Implementation file for the SCT_MonitorConditionsTestAlg class 
 * in package SCT_ConditionsServices
 *
 * @author Kazu Hanagaki
 **/

#include "SCT_MonitorConditionsTestAlg.h"

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

//local includes
#include "SCT_ConditionsServices/ISCT_MonitorConditionsSvc.h"
#include "SCT_MonitorConditionsSvc.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "StoreGate/ReadHandle.h"

SCT_MonitorConditionsTestAlg::SCT_MonitorConditionsTestAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ), 
  m_pMonitorConditionsSvc("SCT_MonitorConditionsSvc",name),
  m_sctId{nullptr},
  m_evtKey(std::string("EventInfo")),
  m_select_run(1),
  m_select_event(1)
{
  declareProperty("RunNumber",m_select_run);
  declareProperty("EventNumber",m_select_event);
}

SCT_MonitorConditionsTestAlg::~SCT_MonitorConditionsTestAlg()
{ 
  msg(MSG::INFO) << "Calling destructor" << endmsg;
}

// -------------------------------------------------------------------------

StatusCode SCT_MonitorConditionsTestAlg::initialize()
{

  StatusCode sc;

  sc = detStore()->retrieve(m_sctId, "SCT_ID");  
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not get SCT_ID helper !" << endmsg;      
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG) << "Found SCT_ID Tool" << endmsg;

  sc = m_pMonitorConditionsSvc.retrieve();
  if (StatusCode::SUCCESS not_eq sc) {
    msg(MSG::ERROR) << "Could not retrieve the monitor conditions service" << endmsg;
    return StatusCode::FAILURE;
  }
  if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG) << "Found SCT_MoniotorConditinosSvc" << endmsg;

  // Read Handle
  ATH_CHECK( m_evtKey.initialize() );

  return StatusCode::SUCCESS;

}

StatusCode SCT_MonitorConditionsTestAlg::execute()
{
  //This method is only used to test the summary service, and only used within this package,
  // so the INFO level messages have no impact on performance of these services when used by clients
  //  A) Writing conditions data to the database
  //     * This is no longer done in this package so the test has been removed * 
  //  B) Reading back in conditions data to analyse it
  //
  //    1) Aside from specifying the correct jobOptions, this is
  //       simply done by doing a standard StoreGate retrieve from
  //       the DetectorStore.

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " in execute()" << endmsg;

  SG::ReadHandle<xAOD::EventInfo> evt(m_evtKey);
  if ( not evt.isValid() ) {
    msg(MSG::ERROR) << "could not get event info " 
                    << endmsg;
    return( StatusCode::FAILURE);
  }
  else {
    if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG) << "Event: [" << evt->runNumber()
                                             << "," << evt->eventNumber()
                                             << ":" << evt->timeStamp()
                                             << "]" << endmsg;
  }
 
  // We create the conditions objects only for a specified run and event
  if ( evt->runNumber() != m_select_run || 
       evt->eventNumber() != m_select_event ) {
    if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG) << "Event NOT selected for creating conditions objects " << endmsg;
    return StatusCode::SUCCESS;
  }
  else {
    if (msgLvl(MSG::DEBUG))  msg(MSG::DEBUG) << "Creating condtions objects " << endmsg;
  }

  std::string defectlist; 
  std::string EfficiencyTable;
  Identifier stripid1;
  Identifier waferid1;
  Identifier moduleid1;

  // For testing of reading back from database
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Testing isGood() function" << endmsg;

  SCT_ID::const_id_iterator waferItr  = m_sctId->wafer_begin();
  SCT_ID::const_id_iterator waferItrE = m_sctId->wafer_end();
  int n_bad=0;
  for( ; waferItr != waferItrE; ++waferItr ) {
    Identifier waferId = *waferItr;
    for(int i=0; i<768; i++){
      Identifier stripId = m_sctId->strip_id(waferId,i);
      if(!(m_pMonitorConditionsSvc->isGood(stripId, InDetConditions::SCT_STRIP)))
	n_bad++;
    }
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "(MonitorTest): #bad strips(Total) = " << n_bad << endmsg;

  // check if strip is noisy
  stripid1 = m_sctId->strip_id(0,3,41,-4,1,703);
  waferid1 = m_sctId->wafer_id(stripid1);
  moduleid1 = m_sctId->module_id(waferid1);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "(MonitorTest): stripid  = " << stripid1 << endmsg;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "(MonitorTest): moduleid = " << moduleid1 << endmsg;
  //    SCT_ComponentIdentifier compid = SCT_ComponentIdentifier(stripid1,"STRIP");
  //    SCT_Conditions::SCT_ComponentIdentifier compid(stripid1,"STRIP");
  bool isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_STRIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): strip(0,3,41,-4,1,703) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): strip(0,3,41,-4,1,703) is noisy " << endmsg;
  }
  
  stripid1 = m_sctId->strip_id(0,3,41,-4,0,703);
  waferid1 = m_sctId->wafer_id(stripid1);
  moduleid1 = m_sctId->module_id(waferid1);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_STRIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): strip(0,3,41,-4,0,703) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): strip(0,3,41,-4,0,703) is noisy " << endmsg;
  }
  
  // Added 04/06/09
  stripid1 = m_sctId->strip_id(0,2,39,-1,0,397);
  waferid1 = m_sctId->wafer_id(stripid1);
  moduleid1 = m_sctId->module_id(waferid1);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "(MonitorTest): stripid  = " << stripid1 << endmsg;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "(MonitorTest): moduleid = " << moduleid1 << endmsg;
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_STRIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): strip(0,2,39,-1,0,397) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): strip(0,2,39,-1,0,397) is noisy " << endmsg;
  }
  
  stripid1 = m_sctId->strip_id(0,2,39,-1,0,396);
  waferid1 = m_sctId->wafer_id(stripid1);
  moduleid1 = m_sctId->module_id(waferid1);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "(MonitorTest): stripid  = " << stripid1 << endmsg;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "(MonitorTest): moduleid = " << moduleid1 << endmsg;
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_STRIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): strip(0,2,39,-1,0,396) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): strip(0,2,39,-1,0,396) is noisy " << endmsg;
  }
  
  stripid1 = m_sctId->strip_id(0,2,39,-1,0,398);
  waferid1 = m_sctId->wafer_id(stripid1);
  moduleid1 = m_sctId->module_id(waferid1);
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "(MonitorTest): stripid  = " << stripid1 << endmsg;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "(MonitorTest): moduleid = " << moduleid1 << endmsg;
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_STRIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): strip(0,2,39,-1,0,398) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): strip(0,2,39,-1,0,398) is noisy " << endmsg;
  }
  
  stripid1 = m_sctId->strip_id(0,3,13,-3,0,567);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_STRIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): strip(0,3,13,-3,0,567) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): strip(0,3,13,-3,0,567) is noisy " << endmsg;
  }
  
  stripid1 = m_sctId->strip_id(0,3,13,-3,0,566);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_STRIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): strip(0,3,13,-3,0,566) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): strip(0,3,13,-3,0,566) is noisy " << endmsg;
  }
  
  stripid1 = m_sctId->strip_id(0,3,13,-3,1,567);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_STRIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): strip(0,3,13,-3,1,567) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): strip(0,3,13,-3,1,567) is noisy " << endmsg;
  }

  stripid1 = m_sctId->strip_id(0,0,7,2,0,700);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_STRIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): strip(0,0,7,2,0,700) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): strip(0,0,7,2,0,700) is noisy " << endmsg;
  }
  
  stripid1 = m_sctId->strip_id(0,0,7,2,1,700);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"STRIP");
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_STRIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): strip(0,0,7,2,1,700) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): strip(0,0,7,2,1,700) is noisy " << endmsg;
  }

  // check if chip is noisy
  stripid1 = m_sctId->strip_id(0,0,8,-4,0,100);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"CHIP");
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_CHIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): chip(0,0,8,-4,0,100) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): chip(0,0,8,-4,0,100) is noisy " << endmsg;
  }
  
  stripid1 = m_sctId->strip_id(0,3,13,-3,0,567);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"CHIP");
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_CHIP);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): chip(0,3,13,-3,0,567) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): chip(0,3,13,-3,0,567) is noisy " << endmsg;
  }
  
  // check if wafer is noisy
  stripid1 = m_sctId->strip_id(0,0,8,-4,0,100);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"WAFER");
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_SIDE);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): wafer(0,0,8,-4,0,100) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): wafer(0,0,8,-4,0,100) is noisy " << endmsg;
  }
  
  // check if module is noisy
  stripid1 = m_sctId->strip_id(0,0,8,-4,0,100);
  //    compid = SCT_Conditions::SCT_ComponentIdentifier(stripid1,"MODULE");
  isthisGood = m_pMonitorConditionsSvc->isGood(stripid1, InDetConditions::SCT_MODULE);
  if( isthisGood ){
    msg(MSG::INFO) << "isGood(): module(0,0,8,-4,0,100) is not noisy " << endmsg;
  } else {
    msg(MSG::INFO) << "isGood(): module(0,0,8,-4,0,100) is noisy " << endmsg;
  }

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "(MonitorTest): execute finished successfully." << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode SCT_MonitorConditionsTestAlg::finalize()
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "SCT_MonitorConditionsTestAlg::finalize" <<endmsg;

  return StatusCode::SUCCESS;
}

// ------------------------------------------------------------------------------
