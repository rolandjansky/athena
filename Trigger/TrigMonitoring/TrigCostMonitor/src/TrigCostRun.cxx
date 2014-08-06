/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/MsgStream.h"

// Reconstruction
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

// Trigger
#include "TrigConfHLTData/HLTUtils.h"
#include "TrigTimeAlgs/TrigTimer.h"

// Trigger daata
#include "TrigMonitoringEvent/TrigMonConfigCollection.h"
#include "TrigMonitoringEvent/TrigMonEventCollection.h"
#include "TrigNavigation/Navigation.h"
#include "TrigSerializeResult/StringSerializer.h"
#include "TrigSteeringEvent/HLTResult.h"

// Local
#include "TrigCostMonitor/UtilCost.h"
#include "TrigCostMonitor/TrigCostRun.h"


//---------------------------------------------------------------------------------------
TrigCostRun::TrigCostRun(const std::string& name,
			 ISvcLocator* pSvcLocator)
  :Algorithm(name, pSvcLocator),
   m_log(0),
   m_timerExec(0),
   m_timerNavig(0),
   m_timerTotal(0),
   m_storeGate("StoreGateSvc", name),
   m_timerSvc("TrigTimerSvc/TrigTimerSvc", name),
   m_navigation("HLT::Navigation/Navigation", this),
   m_tools(this),
   m_toolsSave(this)
{
  declareProperty("navigation",    m_navigation, "Trigger navigation tool");
  declareProperty("tools",         m_tools,      "Tools array");
  declareProperty("toolsSave",     m_toolsSave,  "Tools array for writing output");

  declareProperty("doTiming",      m_doTiming      = true);
  declareProperty("printEvent",    m_printEvent    = false);
  declareProperty("keyStream",     m_keyStream   = "");

  declareProperty("keyResultL2",   m_readL2.keyResult = "");
  declareProperty("keyConfigL2",   m_readL2.keyConfig = "");
  declareProperty("keyEventL2",    m_readL2.keyEvent  = "");
  declareProperty("doL2",          m_readL2.doLevel   = true);

  declareProperty("keyResultEF",   m_readEF.keyResult = "");
  declareProperty("keyConfigEF",   m_readEF.keyConfig = "");
  declareProperty("keyEventEF",    m_readEF.keyEvent  = "");
  declareProperty("doEF",          m_readEF.doLevel   = true);
  
  declareProperty("keyResultHLT",   m_readHLT.keyResult = "");
  declareProperty("keyConfigHLT",   m_readHLT.keyConfig = "");
  declareProperty("keyEventHLT",    m_readHLT.keyEvent  = "");
  declareProperty("doHLT",          m_readHLT.doLevel   = true);

}

//---------------------------------------------------------------------------------------
TrigCostRun::~TrigCostRun()
{
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostRun::initialize()
{    
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
    
    m_timerExec  = m_timerSvc->addItem(name()+":ExecTime");
    m_timerNavig = m_timerSvc->addItem(name()+":NavigTime");
    m_timerTotal = m_timerSvc->addItem(name()+":TotalTime");
    
    m_timerTotal -> start(); 
  }

  if(m_navigation.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retrieve navigation tool: " << m_navigation << endreq;
    return  StatusCode::FAILURE;
  } 

  log() << MSG::DEBUG << "Retrieved navigation tool: " << m_navigation << endreq;

  if(m_tools.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retrieve " << m_tools << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log() << MSG::INFO << "Retrieved " << m_tools << endreq;
  }
  
  if(m_toolsSave.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retrieve " << m_toolsSave << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log() << MSG::INFO << "Retrieved " << m_toolsSave << endreq;
  }

  //
  // Pass global configuration pointer to sub-tools
  //
  for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_tools.begin(); it != m_tools.end(); ++it) {
    (*it)->Fill(&m_config);
  }

  //
  // Configure HLTResult extraction objects
  //
  m_readL2.hltLevel     = "L2";
  m_readL2.outputLevel  = outputLevel();
  m_readL2.msgStream    = m_log;
  m_readL2.globalConfig = &m_config;
  m_readL2.timerNavig   = m_timerNavig;
  m_readL2.PrintInit();

  m_readEF.hltLevel     = "EF";  
  m_readEF.outputLevel  = outputLevel();
  m_readEF.msgStream    = m_log;
  m_readEF.globalConfig = &m_config;
  m_readEF.timerNavig   = m_timerNavig;
  m_readEF.PrintInit();
  
  m_readHLT.hltLevel     = "HLT";  
  m_readHLT.outputLevel  = outputLevel();
  m_readHLT.msgStream    = m_log;
  m_readHLT.globalConfig = &m_config;
  m_readHLT.timerNavig   = m_timerNavig;
  m_readHLT.PrintInit();
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostRun::execute()
{
  //
  // Process current event
  //
  
  static int firstTwo = 0;
  if (firstTwo++ < 2) {
	  log() << MSG::WARNING << m_storeGate->dump() << endreq;
  }
  
  const DataHandle<EventInfo> event_handle;
  if(m_storeGate -> retrieve(event_handle).isFailure()) {
    log() << MSG::WARNING << "Failed to read EventInfo" << endreq;
    return StatusCode::SUCCESS;
  }

  ScopeResumePauseTimer execTimer(m_timerExec, false);

  //
  // Print EventInfo and stream tags
  //
  TriggerInfo *trig = event_handle->trigger_info();
  if(!trig) {
    log() << MSG::WARNING << "Failed to get TriggerInfo" << endreq;
    return StatusCode::SUCCESS;
  }

  const std::vector<TriggerInfo::StreamTag> &streams = trig->streamTags();

  if(m_printEvent) {
    log() << MSG::INFO << ">>>>>>>>>>>>>>>>"
	  << " run #" << event_handle->event_ID()->run_number()
	  << " lumi #" << event_handle->event_ID()->lumi_block()
	  << " event #" << event_handle->event_ID()->event_number() 
	  << " has " << streams.size() << " streams " << endreq;
  }
  
  bool found_stream = false;
  for(unsigned i = 0; i < streams.size(); ++i) {
    const TriggerInfo::StreamTag &stag = streams[i];
    if(stag.name() == m_keyStream) {
      found_stream = true;
      break;
    }

    if(m_printEvent) {
      log() << MSG::INFO << "  stream " << i << ": " << stag.name() << "/" << stag.type() << endreq;
    }
  }

  if(!m_keyStream.empty() && !found_stream) {
    log() << MSG::DEBUG << "Failed to find stream: " << m_keyStream << endreq;
    return StatusCode::SUCCESS;
  }

  //
  // Extract HLT result and read TrigMonConfig/TrigCostEvent collections
  //
  {
    ScopeResumePauseTimer scopeTimer(m_timerNavig, false);
    m_navigation->reset();
  }

  m_readL2.ProcessEvent(m_storeGate, m_navigation);
  m_readEF.ProcessEvent(m_storeGate, m_navigation);
  m_readHLT.ProcessEvent(m_storeGate, m_navigation);

  if(m_printEvent) {
    m_readL2.PrintEvent();
    m_readEF.PrintEvent();
    m_readHLT.PrintEvent();
  }

  //
  // Run post-processing tools for TrigMonConfig
  //
  for(unsigned i = 0; i < m_readL2.vecConfig.size(); ++i) {
    for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_toolsSave.begin(); it != m_toolsSave.end(); ++it) {
      (*it)->Fill(&m_readL2.vecConfig.at(i));
    }
  }
  
  for(unsigned i = 0; i < m_readEF.vecConfig.size(); ++i) {
    for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_toolsSave.begin(); it != m_toolsSave.end(); ++it) {
      (*it)->Fill(&m_readEF.vecConfig.at(i));
    }
  }
  
  for(unsigned i = 0; i < m_readHLT.vecConfig.size(); ++i) {
    for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_toolsSave.begin(); it != m_toolsSave.end(); ++it) {
      (*it)->Fill(&m_readHLT.vecConfig.at(i));
    }
  }

  //
  // Check if we have L2 and EF events for the same L1 event: 
  //
  if(MatchL2andEF(m_readL2.vecEvent, m_readEF.vecEvent)) {
    log() << MSG::DEBUG << "L2 and EF events match - merge two events" << endreq;
    //
    // Merge L2 into EF: use EF because it has correct (non-L1) event number
    //    
    const std::string msg = Trig::MergeEvent(m_readEF.vecEvent.front(), m_readL2.vecEvent.front(), 2);
    if(!msg.empty()) {
      log() << MSG::WARNING << msg << endreq;
    }

    //
    // Run post-processing tools
    //
    for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_tools.begin(); it != m_tools.end(); ++it) {
      (*it)->Fill(m_readEF.vecEvent.front());
    }
    for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_toolsSave.begin(); it != m_toolsSave.end(); ++it) {

      (*it)->Fill(m_readEF.vecEvent.front());
    }
  }
  else {
    log() << MSG::DEBUG << "L2 and EF events do not match - record them separately" << endreq;

    //
    // Run post-processing tools for TrigMonEvent
    //
    for(unsigned i = 0; i < m_readL2.vecEvent.size(); ++i) {
      for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_tools.begin(); it != m_tools.end(); ++it) {

		(*it)->Fill(m_readL2.vecEvent.at(i));
      }
      for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_toolsSave.begin(); it != m_toolsSave.end(); ++it) {
        (*it)->Fill(m_readL2.vecEvent.at(i));
      }
    }
    for(unsigned i = 0; i < m_readEF.vecEvent.size(); ++i) {
      for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_tools.begin(); it != m_tools.end(); ++it) {

		(*it)->Fill(m_readEF.vecEvent.at(i));
      }
      for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_toolsSave.begin(); it != m_toolsSave.end(); ++it) {

        (*it)->Fill(m_readEF.vecEvent.at(i));
      }
    }
  }
  
  //
  // Run post-processing tools for TrigMonEvent for unified HLT
  //
  for(unsigned i = 0; i < m_readHLT.vecEvent.size(); ++i) {
    for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_tools.begin(); it != m_tools.end(); ++it) {

  	(*it)->Fill(m_readHLT.vecEvent.at(i));
    }
    for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_toolsSave.begin(); it != m_toolsSave.end(); ++it) {

  	(*it)->Fill(m_readHLT.vecEvent.at(i));
  	}
	}


  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostRun::finalize()
{
  //
  // Finalize state
  //
  log() << MSG::INFO << "finalize()" << endreq;

  for(ToolHandleArray<Trig::ITrigNtTool>::iterator it = m_toolsSave.begin(); it != m_toolsSave.end(); ++it) {

    (*it)->Fill(&m_config);
  }

  m_readL2.PrintSummary();
  m_readEF.PrintSummary();
  m_readHLT.PrintSummary();

  if(m_doTiming) {
    m_timerExec ->stop();
    m_timerNavig->stop();
    m_timerTotal->stop();

    log() << MSG::INFO 
	  << m_timerExec ->name() << ": " << m_timerExec ->elapsed()/1000.0 << "s" << endreq
	  << m_timerNavig->name() << ": " << m_timerNavig->elapsed()/1000.0 << "s" << endreq
	  << m_timerTotal->name() << ": " << m_timerTotal->elapsed()/1000.0 << "s" << endreq;
  }

  delete m_log; m_log = 0;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool TrigCostRun::MatchL2andEF(const std::vector<TrigMonEvent> &events_l2,
			       const std::vector<TrigMonEvent> &events_ef)
{
  //
  // Check if L2 and EF vectors contain the same single event
  //
  if(!(events_l2.size() == 1 && events_ef.size() == 1)) {
    return false;
  }
  
  const TrigMonEvent &evt_l2 = events_l2.front();
  const TrigMonEvent &evt_ef = events_ef.front();

  if(evt_l2.getRun()     != evt_ef.getRun())     return false;  
  if(evt_l2.getLumi()    != evt_ef.getLumi())    return false;
  if(evt_l2.getSec()     != evt_ef.getSec())     return false;
  if(evt_l2.getNanoSec() != evt_ef.getNanoSec()) return false;
  if(evt_l2.getBunchId() != evt_ef.getBunchId()) return false;

  return true;
}

//---------------------------------------------------------------------------------------
TrigCostRun::ReadHLTResult::ReadHLTResult()
  :outputLevel(3),
   msgStream(0),
   globalConfig(0),
   timerNavig(0),
   appId(0),
   countTrunc(0),
   countInvalid(0),
   countValid(0),
   countEvent(0),
   countConfig(0)
{
}

//-----------------------------------------------------------------------------
bool TrigCostRun::ReadHLTResult::ProcessEvent(ServiceHandle<StoreGateSvc> &storeGate, 
					      ToolHandle<HLT::Navigation> &navigation)
{
  //
  // Process one event:
  //  - clear state
  //  - read HLT result
  //  - read TrigMonEvent  collection
  //  - read TrigMonConfig collection
  //
  
  if (doLevel == false) return false;
  
  vecConfig.clear();
  vecEvent .clear();

  appId = 0;
  appName.clear();

  //
  // Read HLT result
  //
  if(!ReadResult(storeGate, navigation)) {
    ++countInvalid;
    return false;
  }
  
  ++countValid;

  //
  // Read events and configs
  //
  ReadConfig(storeGate);
  ReadEvent (storeGate);
  
  return true;
}

//-----------------------------------------------------------------------------
bool TrigCostRun::ReadHLTResult::ReadResult(ServiceHandle<StoreGateSvc> &storeGate, 
					    ToolHandle<HLT::Navigation> &navigation)
{
  //
  // Find HLTResult and extract application id
  //  
  if(!storeGate->contains<HLT::HLTResult>(keyResult)) {
    log() << MSG::DEBUG << "StoreGate does not contain HLTResult: " << keyResult << endreq;
    return false;
  }

  const HLT::HLTResult *hlt_result = 0;
  if(storeGate->retrieve<HLT::HLTResult>(hlt_result, keyResult).isFailure() || !hlt_result) {
    log() << MSG::WARNING << "Failed to retrieve HLTResult: " << keyResult << endreq;
    return false;
  }
  else {
    if(outputLevel <= MSG::DEBUG) {
      log() << MSG::DEBUG << "Found HLTResult: " << keyResult << endreq;
    }
  }

  if(hlt_result->isHLTResultTruncated()) {
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "HLTResult is truncated" << endreq;
    ++countTrunc;
  }

  const std::vector<uint32_t>& navData = hlt_result->getNavigationResult();
  if(navData.empty()) {
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Navigation data is empty" << endreq;
    return false;
  }
  
  {
    ScopeResumePauseTimer scopeTimer(timerNavig, false);
    
    if(!navigation->deserialize(navData)) {
      log() << MSG::WARNING << "Failed to deserialize navigation" << endreq;
      return false;
    }
  }

  const std::vector<uint32_t> &extraData = hlt_result->getExtras();
  if(extraData.empty()) {
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Extra data is empty" << endreq;
    return true;
  }
  
  //
  // Extract application name into appName variable
  //
  StringSerializer().deserialize(extraData, appName); 
  
  if(appName.empty()) {    
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Application name is empty" << endreq;
    return true;
  }

  if(hlt_result->getHLTLevel() == HLT::L2) {
    if(hltLevel != "L2") {
      log() << MSG::WARNING << "HLT level error: " << hltLevel << "!=" << hlt_result->getHLTLevel() << endreq;
    }
  }
  else if(hlt_result->getHLTLevel() == HLT::EF) {
    if(hltLevel != "EF") {
      log() << MSG::WARNING << "HLT level error: " << hltLevel << "!=" << hlt_result->getHLTLevel() << endreq;
    }
  }
  else if(hlt_result->getHLTLevel() == HLT::HLT) {
    if(hltLevel != "HLT") {
      log() << MSG::WARNING << "HLT level error: " << hltLevel << "!=" << hlt_result->getHLTLevel() << endreq;
    }
  }
  else {
    log() << MSG::WARNING << "Unknown level for HLTResult: " << hlt_result->getHLTLevel() << endreq;
  }

  //
  // Append application ID with level string
  //
  appName = "APP_"+hltLevel+":"+appName;
  if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Extracted App Name: " << appName << endreq;


  //
  // Get hash id for HLT result
  //
  appId = TrigConf::HLTUtils::string2hash(appName, "APP_ID");
  
  //
  // Save map between application name and application id (hashed name)
  //
  const std::vector<uint32_t> &ids = globalConfig->getVarId();
  
  if(!std::count(ids.begin(), ids.end(), appId)) {
    globalConfig->add<TrigConfVar>(TrigConfVar(appName, appId));
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool TrigCostRun::ReadHLTResult::ReadConfig(ServiceHandle<StoreGateSvc> &storeGate)
{
  //
  // Read TrigMonConfig collection
  //
  const TrigMonConfigCollection *configCol = 0;

  {
    ScopeResumePauseTimer scopeTimer(timerNavig, false);

    if(!storeGate->contains<TrigMonConfigCollection>(keyConfig)) {
      if(outputLevel <= MSG::DEBUG) {
	log() << MSG::DEBUG  << "TrigMonConfigCollection does not exist: " << keyConfig << endreq;
      }
      return false;
    }
    
    if(storeGate->retrieve(configCol, keyConfig).isFailure() || !configCol) {
      log() << MSG::WARNING << "Failed to retrieve TrigMonConfigCollection: " << keyConfig << endreq;
      return false;
    }
    
    if(outputLevel <= MSG::DEBUG) {
      log() << MSG::DEBUG 
	    << "Found TrigMonConfigCollection: " << keyConfig << " size=" << configCol->size() << endreq;
    }
  }


  for(TrigMonConfigCollection::const_iterator it = configCol->begin(); it != configCol->end(); ++it) {
    TrigMonConfig *ptr = *it;
    if(!ptr) continue;
    
    //
    // Save TrigMonConfig
    //
    vecConfig.push_back(*ptr);

    if(outputLevel <= MSG::DEBUG) {
      log() << MSG::DEBUG 
	    << "ExtractConfig #" << ptr->getEvent() << " lumi #" << ptr->getLumi() << endreq;
    }
  }
  
  //
  // Count total number of read TrigMonConfig objects
  //
  countConfig += vecConfig.size();

  return true;
}

//---------------------------------------------------------------------------------------
bool TrigCostRun::ReadHLTResult::ReadEvent(ServiceHandle<StoreGateSvc> &storeGate)
{

  if (doLevel == false) return false;
  
  //
  // Read TrigMonintoringEvent collection
  //
  const TrigMonEventCollection *eventCol = 0;

  {
    ScopeResumePauseTimer scopeTimer(timerNavig, false);

    if(!storeGate->contains<TrigMonEventCollection>(keyEvent)) {
      if(outputLevel <= MSG::DEBUG) {
	log() << MSG::DEBUG  << "TrigMonEventCollection does not exist: " << keyEvent << endreq;
      }
      return false;
    }
    
    if(storeGate->retrieve(eventCol, keyEvent).isFailure() || !eventCol) {
      log() << MSG::WARNING << "Failed to retrieve TrigMonEventCollection: " << keyEvent << endreq;
      return false;
    }
  }

  bool fill_size = true;

  std::vector<std::string> &vars = globalConfig->getVarName();
  std::vector<uint32_t>    &vals = globalConfig->getVarId();

  for(TrigMonEventCollection::const_iterator it = eventCol->begin(); it != eventCol->end(); ++it) {
    TrigMonEvent *ptr = *it;
    if(!ptr) continue;

    ptr->addWord(appId); //Backward compatability
    ptr->addVar(42, appId); //For more security, also store this in location 42

    if(fill_size) {
      ptr->addVar(9, float(eventCol->size()));
      fill_size = false;
    }
    
    if(vars.size() == vals.size() && std::count(vals.begin(), vals.end(), ptr->getLumi()) == 0) {
      //
      // Save all lumi blocks processed by this job
      //
      vars.push_back("LB");
      vals.push_back(ptr->getLumi());
    }

    //
    // Save the lumi block length
    //
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Reading lumi length" << endreq;
    if (m_readLumiBlock.getTriedSetup() == false) m_readLumiBlock.updateLumiBlocks( ptr->getRun() );
    ptr->addVar(43, m_readLumiBlock.getLumiBlockLength(ptr->getLumi())); // 43 is lumi block length
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "LB "<< ptr->getLumi()<<" is Length " << m_readLumiBlock.getLumiBlockLength( ptr->getLumi()) << endreq;
    std::string _msg = m_readLumiBlock.infos();
    if (_msg.size()) log() << MSG::INFO << _msg;
    std::string _dbg = m_readLumiBlock.debug();
    if (_dbg.size() && outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << _dbg;
    m_readLumiBlock.clearMsg();


    //
    // Save events: so that later L2 and EF events can be merged
    //
    vecEvent.push_back(*ptr);
  }

  //
  // Count total number of read TrigMonEvent objects
  //
  countEvent += vecEvent.size();

  return true;
}

//---------------------------------------------------------------------------------------
void TrigCostRun::ReadHLTResult::PrintInit()
{
  //
  // Print summary counters
  //
  log() << MSG::INFO
	<< "  " << hltLevel << ": HLTResult key     = " << keyResult << endreq
	<< "  " << hltLevel << ": TrigMonConfig key = " << keyConfig << endreq
	<< "  " << hltLevel << ": TrigMonEvent  key = " << keyEvent  << endreq
	<< "  " << hltLevel << ": Enabled = "           << (doLevel ? "Yes" : "NO")  << endreq;
}

//---------------------------------------------------------------------------------------
void TrigCostRun::ReadHLTResult::PrintEvent()
{

  if (doLevel == false) return;
  
  //
  // Print current events in memory
  //
  log() << MSG::INFO << "Extracted TrigMonEventCollection: " << keyEvent 
	<< " with: " << vecEvent.size() << " event(s)" << endreq; 

  for(unsigned i = 0; i < vecEvent.size(); ++i) {
    const TrigMonEvent &event = vecEvent.at(i);

    log() << MSG::INFO
	  << "Run #" << event.getRun() 
	  << " lumi #" << event.getLumi() 
	  << " event #" << event.getEvent()<< endreq;
    
    Trig::Print(event, *globalConfig, log(), MSG::INFO);
  }
}

//---------------------------------------------------------------------------------------
void TrigCostRun::ReadHLTResult::PrintSummary()
{

  if (doLevel == false) return;
  
  //
  // Print summary counters
  //
  log() << MSG::INFO
	<< "  " << hltLevel << ": POST_HLT: # of valid     HLT results = " << countValid   << endreq
	<< "  " << hltLevel << ": POST_HLT: # of invalid   HLT results = " << countInvalid << endreq
	<< "  " << hltLevel << ": POST_HLT: # of truncated HLT results = " << countTrunc   << endreq
	<< "  " << hltLevel << ": POST_HLT: # of read TrigMonEvent     = " << countEvent   << endreq
	<< "  " << hltLevel << ": POST_HLT: # of read TrigMonConfig    = " << countConfig  << endreq;
}
