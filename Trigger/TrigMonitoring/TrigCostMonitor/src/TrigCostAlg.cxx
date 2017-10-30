/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <sys/time.h>

// Framework
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/errorcheck.h"

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
   :AthAlgorithm(name, pSvcLocator),
    m_timerTotal(0),
    m_timerSvc("TrigTimerSvc/TrigTimerSvc", name),
    m_tools(this)
{
  declareProperty("tools",         m_tools,      "Tools array");

  declareProperty("doTiming",      m_doTiming      = true);
  declareProperty("writeConfig",   m_writeConfig   = true);
  declareProperty("writeEvent",    m_writeEvent    = true);
  declareProperty("mergeEvent",    m_mergeEvent    = true);
  declareProperty("printEvent",    m_printEvent    = false);

  declareProperty("keyConfigL2",   m_keyConfigL2  = "HLT_TrigMonConfigCollection_OPI_L2_monitoring_config");
  declareProperty("keyConfigEF",   m_keyConfigEF  = "HLT_TrigMonConfigCollection_OPI_EF_monitoring_config");
  declareProperty("keyConfigHLT",  m_keyConfigHLT = "HLT_TrigMonConfigCollection_OPI_HLT_monitoring_config");
  declareProperty("keyEventL2",    m_keyEventL2   = "HLT_TrigMonEventCollection_OPI_L2_monitoring_event");
  declareProperty("keyEventEF",    m_keyEventEF   = "HLT_TrigMonEventCollection_OPI_EF_monitoring_event");
  declareProperty("keyEventHLT",   m_keyEventHLT  = "HLT_TrigMonEventCollection_OPI_HLT_monitoring_event");

}

//---------------------------------------------------------------------------------------
TrigCostAlg::~TrigCostAlg()
{
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostAlg::initialize()
{    

  if(m_doTiming) { 
    CHECK(m_timerSvc.retrieve());
    m_timerTotal = m_timerSvc->addItem(name()+":TotalTime");
  }

  CHECK(m_tools.retrieve());

  xAOD::EventInfo* eventInfo = 0;
  if(evtStore() -> retrieve(eventInfo).isSuccess()) {
    
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
    ATH_MSG_INFO("Missing xAOD::EventInfo in initialize()... will try again later");
  }

  ATH_MSG_INFO("mergeEvent    = " << m_mergeEvent);
  ATH_MSG_INFO("printEvent    = " << m_printEvent);
  ATH_MSG_INFO("keyEventL2    = " << m_keyEventL2);
  ATH_MSG_INFO("keyEventEF    = " << m_keyEventEF);
  ATH_MSG_INFO("keyEventHLT   = " << m_keyEventHLT);
 
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
  CHECK(evtStore() -> retrieve(eventInfo));

  m_event.clear();
  m_event.setEventID(eventInfo->eventNumber(),
      eventInfo->lumiBlock(),
      eventInfo->bcid(),
      eventInfo->runNumber(),
      eventInfo->timeStamp(),
      eventInfo->timeStampNSOffset());

  ATH_MSG_DEBUG("run #" << m_event.getRun() << " event #" << m_event.getEvent());

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

  ExtractEvent(m_keyEventL2,  2);
  ExtractEvent(m_keyEventEF,  3);
  ExtractEvent(m_keyEventHLT, 2);

  for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_tools.begin(); it != m_tools.end(); ++it) {
    (*it)->Fill(m_event);
  }

  if(msgLvl(MSG::DEBUG) && m_printEvent) {
    Trig::Print(m_event, m_config, msg(), MSG::DEBUG);
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

    ATH_MSG_INFO(m_timerTotal->name() << ": " << m_timerTotal->elapsed()/1000.0 << "s");
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
void TrigCostAlg::ExtractConfig(const std::string &key)
{
  //
  // Read TrigMonConfig
  //
  if(!evtStore() -> transientContains<TrigMonConfigCollection>(key)) {
    ATH_MSG_DEBUG("TrigMonConfigCollection does not exist");
    return;
  }

  const TrigMonConfigCollection *mon_col = 0;
  if(evtStore()->retrieve(mon_col, key).isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve TrigMonConfigCollection at: " << key);
    return;
  }

  ATH_MSG_DEBUG("Found TrigMonConfigCollection: " << key << ", size=" << mon_col->size());
    
  for(TrigMonConfigCollection::const_iterator it = mon_col->begin(); it != mon_col->end(); ++it) {
    const TrigMonConfig *ptr = *it;
    if(!ptr) continue;
  }
}

//---------------------------------------------------------------------------------------
void TrigCostAlg::ExtractEvent(const std::string &key, const int level)
{
  //
  // Read TrigMonEvent
  //
  if(!evtStore() -> transientContains<TrigMonEventCollection>(key)) {
    ATH_MSG_DEBUG("TrigMonEventCollection does not exist at: " << key);
    return;
  }

  const TrigMonEventCollection *mon_col = 0;
  if(evtStore()->retrieve(mon_col, key).isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve TrigMonEventCollection at: " << key);
    return;
  }

  ATH_MSG_DEBUG("Found TrigMonEventCollection: " << key << ", size=" << mon_col->size() );

  for(TrigMonEventCollection::const_iterator it = mon_col->begin(); it != mon_col->end(); ++it) {
    const TrigMonEvent *ptr = *it;
    if(!ptr) continue;
    
    if(m_mergeEvent) {
      const std::string msg = Trig::MergeEvent(m_event, *ptr, level);
      if(!msg.empty()) {
	      ATH_MSG_WARNING(msg);
      }
    } else {
      m_event = *ptr;
    }
  }
}
