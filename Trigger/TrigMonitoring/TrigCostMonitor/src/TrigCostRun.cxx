/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/errorcheck.h"

// Reconstruction
#include "xAODEventInfo/EventInfo.h"	

// Trigger
#include "TrigConfHLTData/HLTUtils.h"
#include "TrigTimeAlgs/TrigTimer.h"

// Trigger data
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
  :AthAlgorithm(name, pSvcLocator),
   m_timerExec(0),
   m_timerNavig(0),
   m_timerTotal(0),
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
  

  if(m_doTiming) { 
    CHECK(m_timerSvc.retrieve());
    
    m_timerExec  = m_timerSvc->addItem(name()+":ExecTime");
    m_timerNavig = m_timerSvc->addItem(name()+":NavigTime");
    m_timerTotal = m_timerSvc->addItem(name()+":TotalTime");
    
    m_timerTotal -> start(); 
  }

  CHECK(m_navigation.retrieve());
  ATH_MSG_DEBUG("Retrieved navigation tool: " << m_navigation);

  CHECK(m_tools.retrieve());
  ATH_MSG_DEBUG("Retrieved : " << m_tools);

  CHECK(m_toolsSave.retrieve());
  ATH_MSG_DEBUG("Retrieved " << m_toolsSave);


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
  m_readL2.outputLevel  = msgLevel();
  m_readL2.msgStream    = &msg();
  m_readL2.globalConfig = &m_config;
  m_readL2.timerNavig   = m_timerNavig;
  m_readL2.PrintInit();

  m_readEF.hltLevel     = "EF";  
  m_readEF.outputLevel  = msgLevel();
  m_readEF.msgStream    = &msg();
  m_readEF.globalConfig = &m_config;
  m_readEF.timerNavig   = m_timerNavig;
  m_readEF.PrintInit();
  
  m_readHLT.hltLevel     = "HLT";  
  m_readHLT.outputLevel  = msgLevel();
  m_readHLT.msgStream    = &msg();
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
  if (m_printEvent && firstTwo++ < 2) {
	  ATH_MSG_WARNING( evtStore()->dump() );
  }

  const xAOD::EventInfo* event_info(0);
  CHECK(evtStore()->retrieve(event_info));  

  ScopeResumePauseTimer execTimer(m_timerExec, false);

  if(m_printEvent) {
    ATH_MSG_INFO(">>>>>>>>>>>>>>>>"
	  << " run #" << event_info->runNumber()
	  << " lumi #" << event_info->lumiBlock()
	  << " event #" << event_info->eventNumber() 
	  << " has " << event_info->streamTags().size() << " streams ");
  }
  
  bool found_stream = false;
  for(unsigned i = 0; i < event_info->streamTags().size(); ++i) {
    const xAOD::EventInfo::StreamTag &stag = event_info->streamTags().at(i);

    if(m_printEvent) {
      ATH_MSG_INFO("  stream " << i << ": " << stag.name() << "/" << stag.type());
    }

    if(stag.name() == m_keyStream) {
      found_stream = true;
      break;
    }

  }

  if(!m_keyStream.empty() && !found_stream) {
    ATH_MSG_WARNING("Failed to find stream: " << m_keyStream);
    return StatusCode::SUCCESS;
  }

  //
  // Extract HLT result and read TrigMonConfig/TrigCostEvent collections
  //
  {
    ScopeResumePauseTimer scopeTimer(m_timerNavig, false);
    m_navigation->reset();
  }

  m_readL2.ProcessEvent(evtStore(), m_navigation);
  m_readEF.ProcessEvent(evtStore(), m_navigation);
  m_readHLT.ProcessEvent(evtStore(), m_navigation);

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
  if(m_readL2.vecEvent.size() && m_readEF.vecEvent.size() && MatchL2andEF(m_readL2.vecEvent, m_readEF.vecEvent)) {
    ATH_MSG_DEBUG("L2 and EF events match - merge two events");
    //
    // Merge L2 into EF: use EF because it has correct (non-L1) event number
    //    
    const std::string msg = Trig::MergeEvent(m_readEF.vecEvent.front(), m_readL2.vecEvent.front(), 2);
    if(!msg.empty()) {
      ATH_MSG_WARNING(msg);
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
  else if (m_readL2.vecEvent.size() || m_readEF.vecEvent.size()) {
    ATH_MSG_DEBUG("L2 and EF events do not match - record them separately");

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
  ATH_MSG_INFO("finalize()");

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

    ATH_MSG_INFO(m_timerExec ->name() << ": " << m_timerExec ->elapsed()/1000.0 << "s");
    ATH_MSG_INFO(m_timerNavig->name() << ": " << m_timerNavig->elapsed()/1000.0 << "s");
    ATH_MSG_INFO(m_timerTotal->name() << ": " << m_timerTotal->elapsed()/1000.0 << "s");
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool TrigCostRun::MatchL2andEF(const std::vector<TrigMonEvent> &events_l2,
			       const std::vector<TrigMonEvent> &events_ef)
{
  //
  // Check if L2 and EF vectors contain the same single event
  //

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
  :doLevel(0),
   outputLevel(3),
   msgStream(0),
   globalConfig(0),
   timerNavig(0),
   appId(0),
   countTrunc(0),
   countInvalid(0),
   countValid(0),
   countCostEvent(0),
   countEvent(0),
   countConfig(0),
   resultPrint(0)
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
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "StoreGate does not contain HLTResult: " << keyResult << endmsg;
    return false;
  }

  const HLT::HLTResult *hlt_result = 0;
  if(storeGate->retrieve<HLT::HLTResult>(hlt_result, keyResult).isFailure() || !hlt_result) {
    log() << MSG::WARNING << "Failed to retrieve HLTResult: " << keyResult << endmsg;
    return false;
  }
  else {
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Found HLTResult: " << keyResult << endmsg;
  }

  if(hlt_result->isHLTResultTruncated()) {
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "HLTResult is truncated" << endmsg;
    ++countTrunc;
  }

  const std::vector<uint32_t>& navData = hlt_result->getNavigationResult();
  if(navData.empty()) {
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Navigation data is empty" << endmsg;
    return false;
  }
  
  {
    ScopeResumePauseTimer scopeTimer(timerNavig, false);
    
    if(!navigation->deserialize(navData)) {
      log() << MSG::WARNING << "Failed to deserialize navigation" << endmsg;
      return false;
    }
  }

  const std::vector<uint32_t> &extraData = hlt_result->getExtras();
  if(extraData.empty()) {
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Extra data is empty" << endmsg;
    return true;
  }
  
  //
  // Extract application name into appName variable
  //
  StringSerializer().deserialize(extraData, appName); 
  
  if(appName.empty()) {    
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Application name is empty" << endmsg;
    return true;
  }

  if(hlt_result->getHLTLevel() == HLT::L2) {
    if(hltLevel != "L2") {
      log() << MSG::WARNING << "HLT level error: " << hltLevel << "!=" << hlt_result->getHLTLevel() << endmsg;
    }
  }
  else if(hlt_result->getHLTLevel() == HLT::EF) {
    if(hltLevel != "EF") {
      log() << MSG::WARNING << "HLT level error: " << hltLevel << "!=" << hlt_result->getHLTLevel() << endmsg;
    }
  }
  else if(hlt_result->getHLTLevel() == HLT::HLT) {
    if(hltLevel != "HLT") {
      log() << MSG::WARNING << "HLT level error: " << hltLevel << "!=" << hlt_result->getHLTLevel() << endmsg;
    }
  }
  else {
    log() << MSG::WARNING << "Unknown level for HLTResult: " << hlt_result->getHLTLevel() << endmsg;
  }

  //
  // Append application ID with level string
  //
  appName = "APP_"+hltLevel+":"+appName;
  if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Extracted App Name: " << appName << endmsg;

  //
  // Save a map between application name and application id (hashed name) in global config
  //
  const std::vector<uint32_t> &ids = globalConfig->getVarId();
  if(!std::count(ids.begin(), ids.end(), appId)) {
    globalConfig->add<TrigConfVar>(TrigConfVar(appName, appId));
  }

  //
  // Get hash id for HLT result
  //
  appId = TrigConf::HLTUtils::string2hash(appName, "APP_ID");
  
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
      if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG  << "TrigMonConfigCollection does not exist: " << keyConfig << endmsg;
      return false;
    }
    
    if(storeGate->retrieve(configCol, keyConfig).isFailure() || !configCol) {
      log() << MSG::WARNING << "Failed to retrieve TrigMonConfigCollection: " << keyConfig << endmsg;
      return false;
    }
    
    if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Found TrigMonConfigCollection: " << keyConfig << " size=" << configCol->size() << endmsg;
  }

  for(TrigMonConfigCollection::const_iterator it = configCol->begin(); it != configCol->end(); ++it) {
    const TrigMonConfig *ptr = *it;
    if(!ptr) continue;

    // Have we already saved this config?
    std::pair<int,int> prescaleKeys( ptr->getLV1PrescaleKey(), ptr->getHLTPrescaleKey() );
    if ( exportedConfigs.find(prescaleKeys) != exportedConfigs.end() ) continue;
    exportedConfigs.insert( prescaleKeys );

    //
    // Save a map between application name and application id (hashed name) in exported config
    //
    const std::vector<uint32_t> &ids = ptr->getVarId();
    if(!std::count(ids.begin(), ids.end(), appId)) {
      // FIXME: const_cast changing SG
      const_cast<TrigMonConfig*>(ptr)->add<TrigConfVar>(TrigConfVar(appName, appId));
      if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Attaching App Name map to Config " << appName << " = " << appId << endmsg;
    }

    //
    // Save TrigMonConfig
    //
    vecConfig.push_back(*ptr);

    log() << MSG::INFO << "ExtractConfig M:" <<  ptr->getMasterKey() 
          << " L1:" << ptr->getLV1PrescaleKey() << " HLT:" << ptr->getHLTPrescaleKey() 
          << " Lumi:" << ptr->getLumi() << endmsg;
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
      if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG  << "TrigMonEventCollection does not exist: " << keyEvent << endmsg;
      return false;
    }
    
    if(storeGate->retrieve(eventCol, keyEvent).isFailure() || !eventCol) {
      log() << MSG::WARNING << "Failed to retrieve TrigMonEventCollection: " << keyEvent << endmsg;
      return false;
    }
  }

  bool fill_size = true;

  std::vector<std::string> &vars = globalConfig->getVarName();
  std::vector<uint32_t>    &vals = globalConfig->getVarId();

  for(TrigMonEventCollection::const_iterator it = eventCol->begin(); it != eventCol->end(); ++it) {
    const TrigMonEvent *ptr = *it;
    if(!ptr) continue;

    // Add my HLT node
    // FIXME: const_cast changing SG
    const_cast<TrigMonEvent*>(ptr)->addWord(appId); //Backward compatability

    if(fill_size) {
      // FIXME: const_cast changing SG
      const_cast<TrigMonEvent*>(ptr)->addVar(Trig::kEventBufferSize, float(eventCol->size()));
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
    // Save the lumi block length, if not already in file
    //
    bool haveLumiLength = false;
    for (unsigned i=0; i < ptr->getVarKey().size(); ++i) {
      if (ptr->getVarKey().at(i) == Trig::kEventLumiBlockLength) {
        haveLumiLength = true;
        if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Lumi length already stored in event" << endmsg;
        break;
      }
    }

    if (haveLumiLength == false) {
      if (m_readLumiBlock.getTriedSetup() == false) {
        log() << MSG::INFO << "Reading lumi length" << endmsg;
        m_readLumiBlock.updateLumiBlocks( ptr->getRun() );
      }
      // FIXME: const-cast changing SG
      const_cast<TrigMonEvent*>(ptr)->addVar(Trig::kEventLumiBlockLength, m_readLumiBlock.getLumiBlockLength(ptr->getLumi())); // 43 is lumi block length
      if(outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << "Decorating Event:" << ptr->getEvent() << "  LB:"<< ptr->getLumi()<<" with LB Length " << m_readLumiBlock.getLumiBlockLength( ptr->getLumi()) << endmsg;
      std::string msg = m_readLumiBlock.infos();
      if (msg.size()) log() << MSG::INFO << msg;
      std::string dbg = m_readLumiBlock.debug();
      if (dbg.size() && outputLevel <= MSG::DEBUG) log() << MSG::DEBUG << dbg;
      m_readLumiBlock.clearMsg();
    }

    //
    // Save events: so that later L2 and EF events can be merged
    //
    vecEvent.push_back(*ptr);

    float isCostEvent = 0.;
    ptr->getVar(Trig::kIsCostEvent, isCostEvent);
    if (isCostEvent > 0.5) ++countCostEvent;
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
       << "  " << hltLevel << ": HLTResult key     = " << keyResult << endmsg
       << "  " << hltLevel << ": TrigMonConfig key = " << keyConfig << endmsg
       << "  " << hltLevel << ": TrigMonEvent  key = " << keyEvent  << endmsg
       << "  " << hltLevel << ": Enabled = "           << (doLevel ? "Yes" : "NO")  << endmsg;
}

//---------------------------------------------------------------------------------------
void TrigCostRun::ReadHLTResult::PrintEvent()
{

  if (doLevel == false) return;
  
  //
  // Print current events in memory
  //
  log() << MSG::INFO << "Extracted TrigMonEventCollection: " << keyEvent 
        << " with: " << vecEvent.size() << " event(s)" << endmsg; 

  static unsigned nPrinted = 0;
  for(unsigned i = 0; i < vecEvent.size(); ++i) {
    const TrigMonEvent &event = vecEvent.at(i);
    ++nPrinted;
    if (nPrinted < 10 || nPrinted % 10 == 0) {
     Trig::Print(event, *globalConfig, log(), MSG::INFO, /*verbosity=*/ 0);
    }
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
        << "  " << hltLevel << ": POST_HLT: # of valid     HLT results = " << countValid   << endmsg
        << "  " << hltLevel << ": POST_HLT: # of invalid   HLT results = " << countInvalid << endmsg
        << "  " << hltLevel << ": POST_HLT: # of truncated HLT results = " << countTrunc   << endmsg
        << "  " << hltLevel << ": POST_HLT: # of read TrigMonEvent     = " << countEvent   << endmsg
        << "  " << hltLevel << ": POST_HLT: # of read TrigMonCostEvent = " << countCostEvent << "(In which scale tools were run)" << endmsg
        << "  " << hltLevel << ": POST_HLT: # of read TrigMonConfig    = " << countConfig  << endmsg;
}
