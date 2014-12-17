/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <sys/time.h>

// Framework
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"

// Reconstruction
#include "xAODEventInfo/EventInfo.h"

// DAQ
#include "eformat/SourceIdentifier.h"

// Trigger
#include "TrigConfHLTData/HLTUtils.h"
#include "TrigSerializeResult/StringSerializer.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigMonitoringEvent/TrigMonConfigCollection.h"
#include "TrigMonitoringEvent/TrigMonEventCollection.h"
#include "TrigTimeAlgs/TrigTimer.h"

// Local
#include "TrigCostMonitor/UtilCost.h"
#include "TrigCostMonitor/TrigCostAlg.h"

//---------------------------------------------------------------------------------------
TrigCostAlg::TrigCostAlg(const std::string& name,
			 ISvcLocator* pSvcLocator)
   :Algorithm(name, pSvcLocator),
    m_log(0),
    m_timerTotal(0),
    m_storeGate("StoreGateSvc", name),
    m_timerSvc("TrigTimerSvc/TrigTimerSvc", name),
    m_tools(this)
{
  declareProperty("tools",         m_tools,      "Tools array");

  declareProperty("doTiming",      m_doTiming      = true);
  declareProperty("mergeEvent",    m_mergeEvent    = true);
  declareProperty("printEvent",    m_printEvent    = false);

  declareProperty("keyConfigL2",   m_keyConfigL2 = "HLT_OPI_L2_monitoring_config");
  declareProperty("keyConfigEF",   m_keyConfigEF = "HLT_OPI_EF_monitoring_config");
  declareProperty("keyEventL2",    m_keyEventL2  = "HLT_OPI_L2_monitoring_event");
  declareProperty("keyEventEF",    m_keyEventEF  = "HLT_OPI_EF_monitoring_event");
}

//---------------------------------------------------------------------------------------
TrigCostAlg::~TrigCostAlg()
{
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostAlg::initialize()
{    
  // Get message service and print out properties
  m_log = new MsgStream(msgSvc(), name()); 
  
  if(m_storeGate.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve StoreGateSvc!" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_doTiming) { 
    if(m_timerSvc.retrieve().isFailure()) {
      log() << MSG::ERROR << "Failed to retrieve TrigTimerSvc" << endreq;
      return  StatusCode::FAILURE;
    }
    
    m_timerTotal = m_timerSvc->addItem(name()+":TotalTime");
  }

  if(m_tools.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retrieve " << m_tools << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log() << MSG::INFO << "Retrieved " << m_tools << endreq;
  }
  

  xAOD::EventInfo* eventInfo = 0;
  if(m_storeGate -> retrieve(eventInfo).isSuccess()) {
    
    m_config.setEventID(eventInfo->eventNumber(),
			eventInfo->lumiBlock(),
			eventInfo->runNumber(),
			eventInfo->timeStamp(),
			eventInfo->timeStampNSOffset());
    
    for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_tools.begin(); it != m_tools.end(); ++it) {
      (*it)->Fill(&m_config);
    }
  }
  else {
    log() << MSG::INFO << "Missing xAOD::EventInfo in initialize()... will try again later" << endreq;
  }

  log() << MSG::INFO 
	<< "mergeEvent    = " << m_mergeEvent    << endreq
	<< "printEvent    = " << m_printEvent    << endreq
	<< "keyEventL2    = " << m_keyEventL2    << endreq
	<< "keyEventEF    = " << m_keyEventEF    << endreq;
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostAlg::execute()
{
  //
  // Process current event
  //
  if(m_timerTotal) m_timerTotal -> resume();

  xAOD::EventInfo* eventInfo = 0;
  if(m_storeGate -> retrieve(eventInfo).isFailure()) {
    log() << MSG::ERROR << "Faied to get xAOD::EventID" << endreq;
    return StatusCode::FAILURE;
  }

  m_event.clear();
  m_event.setEventID(eventInfo->eventNumber(),
      eventInfo->lumiBlock(),
      eventInfo->bcid(),
      eventInfo->runNumber(),
      eventInfo->timeStamp(),
      eventInfo->timeStampNSOffset());

  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG 
	  << "run #" << m_event.getRun() << " event #" << m_event.getEvent() << endreq;
  }

  //
  // Read configuration not filled before or reread configuration for new run 
  //
  if(m_config.getRun() != m_event.getRun()) {
    m_config.setEventID(eventInfo->eventNumber(),
      eventInfo->lumiBlock(),
      eventInfo->runNumber(),
      eventInfo->timeStamp(),
      eventInfo->timeStampNSOffset());
    
    for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_tools.begin(); it != m_tools.end(); ++it) {
      (*it)->Fill(&m_config);
    }
  }  

  ExtractEvent(m_keyEventL2, 2);
  ExtractEvent(m_keyEventEF, 3);

  for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_tools.begin(); it != m_tools.end(); ++it) {
    (*it)->Fill(m_event);
  }

  if(outputLevel() <= MSG::DEBUG && m_printEvent) {
    Trig::Print(m_event, m_config, log(), MSG::DEBUG);
  }

  if(m_timerTotal) m_timerTotal -> pause();

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostAlg::finalize()
{
  //
  // Finalize output
  //

  // Process configuration one last time: if tools need to save updated configuration
  for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_tools.begin(); it != m_tools.end(); ++it) {
    (*it)->Fill(&m_config);
  }

  if(m_timerTotal) {
    m_timerTotal->stop();

    log() << MSG::INFO 
	  << m_timerTotal->name() << ": " << m_timerTotal->elapsed()/1000.0 << "s" << endreq;
  }

  delete m_log; m_log = 0;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
void TrigCostAlg::ExtractConfig(const std::string &key)
{
  //
  // Read TrigMonConfig
  //
  if(!m_storeGate -> transientContains<TrigMonConfigCollection>(key)) {
    if(outputLevel() <= MSG::DEBUG)
      log() << MSG::DEBUG  << "TrigMonConfigCollection does not exist" << endreq;
    return;
  }

  const TrigMonConfigCollection *mon_col = 0;
  if(m_storeGate->retrieve(mon_col, key).isFailure()) {
    log() << MSG::WARNING << "failed retrieving TrigMonConfigCollection" << endreq;
    return;
  }

  log() << MSG::DEBUG 
	<< "Found TrigMonConfigCollection: " << key << ", size=" << mon_col->size() << endreq;
    
  for(TrigMonConfigCollection::const_iterator it = mon_col->begin(); it != mon_col->end(); ++it) {
    TrigMonConfig *ptr = *it;
    if(!ptr) continue;
  }
}

//---------------------------------------------------------------------------------------
void TrigCostAlg::ExtractEvent(const std::string &key, const int level)
{
  //
  // Read TrigMonEvent
  //
  if(!m_storeGate -> transientContains<TrigMonEventCollection>(key)) {
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG  << "TrigMonEventCollection does not exist at: " << key << endreq;
    }
    return;
  }

  const TrigMonEventCollection *mon_col = 0;
  if(m_storeGate->retrieve(mon_col, key).isFailure()) {
    log() << MSG::WARNING << "Failed to retrieve TrigMonEventCollection at: " << key << endreq;
    return;
  }

  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG 
	  << "Found TrigMonEventCollection: " << key << ", size=" << mon_col->size() << endreq; 
  }

  for(TrigMonEventCollection::const_iterator it = mon_col->begin(); it != mon_col->end(); ++it) {
    const TrigMonEvent *ptr = *it;
    if(!ptr) continue;
    
    if(m_mergeEvent) {
      const std::string msg = Trig::MergeEvent(m_event, *ptr, level);
      if(!msg.empty()) {
	log() << MSG::WARNING << msg << endreq;
      }
    }
    else {
      m_event = *ptr;
    }
  }
}
