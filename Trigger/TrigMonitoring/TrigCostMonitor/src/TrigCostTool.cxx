/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <cstdlib>
#include <sstream>

// ROOT
#include "TFile.h"
#include "TTree.h"

// Framework
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"
#include "EventInfo/EventInfo.h" // OLD
#include "EventInfo/EventID.h" // OLD
#include "EventInfo/EventType.h" // OLD
#include "EventInfo/TriggerInfo.h" // OLD
#include "xAODEventInfo/EventInfo.h"
// #include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h" // Get CTP ROB 

// Trigger
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/Navigation.icc"
#include "TrigNavigation/NavigationCore.icc"
#include "TrigSteering/SteeringChain.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigSteering/TrigSteer.h"
#include "TrigTimeAlgs/TrigTimer.h"
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"
#include "TrigSerializeResult/StringSerializer.h"
#include "TrigSteeringEvent/HLTResult.h" 
#include "TrigConfHLTData/HLTUtils.h"
// #include "TrigConfL1Data/CTPExtraWordsFormat.h"

// Local
#include "TrigCostMonitor/UtilCost.h"
#include "TrigCostMonitor/TrigCostTool.h"
#include "TrigCostMonitor/TrigNtConfTool.h"

//---------------------------------------------------------------------------------------
TrigCostTool::TrigCostTool(const std::string& type,
         const std::string& name,
         const IInterface* parent)
  :AlgTool(type, name, parent), 
   m_parentAlg(0),
   m_log(0),
   m_timer(0),
   m_storeGate("StoreGateSvc", name),
   m_timerSvc("TrigTimerSvc/TrigTimerSvc", name),
   m_toolBunchGroup("BunchGroupTool", this),
   m_scalerTool("HLT::RandomScaler/TrigCostScaler", this),
   m_toolConf("Trig::TrigNtConfTool/TrigNtConfTool", this),
   m_toolEBWeight("Trig::TrigNtEBWeightTool/TrigNtEBWeightTool", this),
   m_eventTools(this),
   m_scaleTools(this),
   m_alwaysTools(this),
   m_readLumiBlock(),
   m_active(true),
   m_run(0),
   m_lumi(0),
   m_countEvent(0),
   m_keyTimer(0),
   m_exportedConfig(0),
   m_exportedEvents(0)
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("toolConf",    m_toolConf,    "Trigger configuration tool");
  declareProperty("toolEBWeight",m_toolEBWeight,"Enhanced bias weighting tool");  
  declareProperty("eventTools",  m_eventTools,  "Cost sub-tools run on every event");
  declareProperty("scaleTools",  m_scaleTools,  "Cost sub-tools run on events after prescale");
  declareProperty("alwaysTools", m_alwaysTools, "Cost sub-tools run on every event if always flag is set");
  declareProperty("scalerTool",  m_scalerTool,  "Scaler used to prescale the scaleTools");
  
  declareProperty("level",            m_level            = "");
  declareProperty("monitoringLogic",  m_monitoringLogic  = "no-calib no-debug no-physics");
  declareProperty("monitoringStream", m_monitoringStream = "");
  declareProperty("monitoringTarget", m_monitoringTarget = "");

  declareProperty("useConfDb",        m_useConfDb        = true, "Get trigger configuration from database");
  declareProperty("useConfSvc",       m_useConfSvc       = true, "Get trigger configuration from configuration service");  
  declareProperty("costForCAF",       m_costForCAF       = false, "Enable Cost for CAF mode when (re)running the trigger outside of P1"); 
  declareProperty("doEBWeight",       m_doEBWeight       = false, "Calculate EnhancedBias (un)weighting factors.");
  declareProperty("keyResult",        m_hltResult        = "HLTResult_HLT", "Get the HLT result from storegate");  

  declareProperty("doTiming",         m_doTiming         = false);
  declareProperty("printEvent",       m_printEvent       = false);
  declareProperty("purgeCostStream",  m_purgeCostStream  = true);
  declareProperty("saveEventTimers",  m_saveEventTimers  = false, "Save event timers");
  declareProperty("writeAlways",      m_writeAlways      = false);
  declareProperty("writeConfig",      m_writeConfig      = true);
  declareProperty("writeConfigDB",    m_writeConfigDB    = false);

  declareProperty("stopAfterNEvent",  m_stopAfterNEvent  = 1000);
  declareProperty("execPrescale",     m_execPrescale     = 1.0);
  declareProperty("doOperationalInfo",m_doOperationalInfo= 1.0, "Parent steering value of doOperationalInfo, used to control how often CostMon executes"); // Only for reference here


}

//---------------------------------------------------------------------------------------
TrigCostTool::~TrigCostTool()
{
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostTool::initialize()
{    
  //
  // Get services/tools and print out properties
  //
  m_log = new MsgStream(msgSvc(), name());

  m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
  if(!m_parentAlg) {
    log() << MSG::ERROR << " Unable to cast the parent algorithm to HLT::TrigSteer!" << endreq;
    return StatusCode::FAILURE;
  }

  //
  // Check trigger level against actual trigger level
  //
  if(!m_level.empty()) {
    if((m_level == "L2"  && m_parentAlg->getAlgoConfig()->getHLTLevel() != HLT::L2 ) || 
       (m_level == "EF"  && m_parentAlg->getAlgoConfig()->getHLTLevel() != HLT::EF ) ||
       (m_level == "HLT" && m_parentAlg->getAlgoConfig()->getHLTLevel() != HLT::HLT)
      ) { 
      m_active = false;      
    }
    else if(m_level != "L2" && m_level != "EF" && m_level != "HLT") {
      m_active = false;
    }
  }

  if(!m_active) {
    log() << MSG::INFO << "Set inactive state. Disabling TrigCost." << endreq;
    return StatusCode::SUCCESS;
  }

  // Update from old style "HLT_OPI_HLT"
  if(m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2) {
    m_keySteerOPI = "HLT_TrigOperationalInfoCollection_OPI_L2";
    m_keyTimer    = 100;
  }
  else if(m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF) {
    m_keySteerOPI = "HLT_TrigOperationalInfoCollection_OPI_EF";
    m_keyTimer    = 110;
  }
  else if(m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::HLT) {
    m_keySteerOPI = "HLT_TrigOperationalInfoCollection_OPI_HLT";
    m_keyTimer    = 100;
  }

  if(m_storeGate.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve: " << m_storeGate << endreq;
    return StatusCode::FAILURE;
  }

  // Only get this tool offline
  if (m_costForCAF == true) {
    if(m_toolBunchGroup.retrieve().isFailure()) {
      log() << MSG::ERROR << "Could not retrieve BunchGroupTool: " << m_toolBunchGroup << endreq;
      return StatusCode::FAILURE;
    }  else {
      log() << MSG::INFO << "Retrieved " << m_toolBunchGroup << endreq;
    }
  }

  if(m_toolConf.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve TrigNtConfTool!" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log() << MSG::INFO << "Retrieved " << m_toolConf << endreq;
  }

  if(m_toolEBWeight.retrieve().isFailure()) {
    log() << MSG::ERROR << "Could not retrieve TrigNtEBWeightTool!" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log() << MSG::INFO << "Retrieved " << m_toolEBWeight << endreq;
  }

  if(m_eventTools.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retrieve tools: " << m_eventTools << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log() << MSG::INFO << "Retrieved " << m_eventTools << endreq;
  }
  if(m_scaleTools.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retrieve tools: " << m_scaleTools << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log() << MSG::INFO << "Retrieved " << m_scaleTools << endreq;
  }
  if(m_alwaysTools.retrieve().isFailure()) {
    log() << MSG::ERROR << "Failed to retrieve tools: " << m_alwaysTools << endreq;
    return StatusCode::FAILURE;
  }
  else {
    log() << MSG::INFO << "Retrieved " << m_alwaysTools << endreq;
  }

  if(m_doTiming) { 
    if(m_timerSvc.retrieve().isFailure()) {
      log() << MSG::ERROR << "Requested timing measurements but can't retrieve TrigTimerSvc" << endreq;
      return  StatusCode::FAILURE;
    }
    log() << MSG::DEBUG << "Retrieved TrigTimerSvc" << endreq;

    m_timer = m_timerSvc->addItem(m_parentAlg->name()+":CostMonitor:TotalTime");
  }

  if(m_execPrescale > 0.0) {
    if(m_scalerTool.retrieve().isFailure()) {
      log() << MSG::FATAL << "Failed to retrieve scaler tool: " << m_scalerTool << endreq;
      return StatusCode::FAILURE;
    }
    else {
      log() << MSG::INFO << "Retrieved " << m_scalerTool << endreq;
    }
  }

  m_toolConf -> SetSteer(m_parentAlg);
  m_toolEBWeight -> SetSteer(m_parentAlg);

  for(unsigned i = 0; i < m_eventTools.size(); ++i) {
    m_eventTools[i]->SetSteer(m_parentAlg);
  }
  for(unsigned i = 0; i < m_scaleTools.size(); ++i) {
    m_scaleTools[i]->SetSteer(m_parentAlg);
  }
  if(m_writeAlways) {
    for(unsigned i = 0; i < m_alwaysTools.size(); ++i) {
      m_alwaysTools[i]->SetSteer(m_parentAlg);
    }
  }

  log() << MSG::INFO 
  << "level            = " << m_level            << endreq
  << "monitoringLogic  = " << m_monitoringLogic  << endreq
  << "monitoringStream = " << m_monitoringStream << endreq
  << "monitoringTarget = " << m_monitoringTarget << endreq
  << "purgeCostStream  = " << m_purgeCostStream  << endreq
  << "useConfDb        = " << m_useConfDb        << endreq
  << "useConfSvc       = " << m_useConfSvc       << endreq
  << "costForCAF       = " << m_costForCAF       << endreq 
  << "doEBWeight       = " << m_doEBWeight       << endreq 
  << "saveEventTimers  = " << m_saveEventTimers  << endreq
  << "writeAlways      = " << m_writeAlways      << endreq
  << "writeConfig      = " << m_writeConfig      << endreq
  << "writeConfigDB    = " << m_writeConfigDB    << endreq
  << "stopAfterNEvent  = " << m_stopAfterNEvent  << endreq
  << "execPrescale     = " << m_execPrescale     << endreq
  << "doOperationalInfo= " << m_doOperationalInfo<< endreq
  << "keyTimer         = " << m_keyTimer         << endreq
  << "printEvent       = " << m_printEvent       << endreq;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostTool::finalize()
{
  //
  // Clean up
  //
  log() << MSG::DEBUG  << "finalize()" << endreq;

  log() << MSG::INFO
      << "Cost tool active? " << (m_active == true ? "YES" : "NO!!") << endreq
      << "During run, attached to Navigation (exported): " << m_exportedConfig << " TrigMonConfigs" << endreq
      << "During run, attached to Navigation (exported): " << m_exportedEvents << " TrigMonEvents" << endreq;

  delete m_log; m_log = 0; 

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostTool::bookHists()
{ 
  //
  // Book hists - executed once per offline job and for every new run online
  //
  if(!m_active) return StatusCode::SUCCESS;

  log() << MSG::DEBUG  << "bookHists()" << endreq;

  //
  // Starting new online run or offline job: cleanup everything from previous state
  //
  ClearBeforeNewRun();

  // const DataHandle<EventInfo> event_handle;
  // if(m_storeGate -> transientContains<EventInfo>("") &&
  //    m_storeGate -> retrieve(event_handle).isSuccess()) {

  //   // TimM Disabled configuration dumping here - we check every event for a change in Run Number and LB now
  //   //ProcessConfig(*event_handle);
  // }
  // else {
  //   log() << MSG::DEBUG << "No EventInfo in bookHists()... will try again in fillHists()" << endreq;
  // }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostTool::fillHists()
{
  //
  // Process new event
  //

  //
  // First: check for consitent internal state
  //
  if(!m_active) return StatusCode::SUCCESS;  

  ++m_countEvent;

  ScopeTimer scopeTimer(m_timer);

  xAOD::EventInfo* eventInfo = 0;
  if(m_storeGate -> retrieve(eventInfo).isFailure()) {
    log() << MSG::WARNING << "Failed to read event info - set state to inactive. CostMon is now DISABLED." << endreq;
    m_active = false;
    return StatusCode::SUCCESS;
  }

  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG << "---------------> starting new event #" << m_countEvent << " (online #" << eventInfo->eventNumber() << ")" << endreq; 
  }

  //
  // Sanity check: make sure that we are not in a new online run
  //
  ClearBeforeNewRun(eventInfo->runNumber());

  // Check if we need to load any new config
  ProcessConfig(eventInfo);

  // Second check for status - in case configuration has failed
  if(!m_active) return StatusCode::SUCCESS;

  //
  // Make new event and call tools
  //  
  const bool monitoringEvent = IsMonitoringEvent(eventInfo); // Have I passed the CostMon chain and no other chains?
  const unsigned opiLevel = m_parentAlg->getAlgoConfig()->getSteeringOPILevel(); // Have I passed the OPI "prescale"?

  if(m_stopAfterNEvent > 0 && m_bufferEvents.size() >= m_stopAfterNEvent) { // Have we filled up the buffer? This is bad!
    delete m_bufferEvents.front();
    m_bufferEvents.erase(m_bufferEvents.begin());

    if(outputLevel() <= MSG::WARNING) {
      log() << MSG::WARNING << "Reached CostMon buffer limit... deleted 1st event. Lost statistics!! "
      << " Event buffer size = " << m_bufferEvents.size() << endreq;
    }
  }

  TrigMonEvent *event = new TrigMonEvent();
  m_bufferEvents.push_back(event);
  
  event->setEventID(eventInfo->eventNumber(),
        eventInfo->lumiBlock(),
        eventInfo->bcid(),
        eventInfo->runNumber(),
        eventInfo->timeStamp(),
        eventInfo->timeStampNSOffset());    

  // Run the per event tools
  for(unsigned i = 0; i < m_eventTools.size(); ++i) { 
    if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Running EventTool " << i << ", " << m_eventTools[i]->type() << endreq;
    m_eventTools[i]->Fill(*event); 
  }
  
  // Decide if we run the scale tools
  bool _prescaleDecision = true;
  int _ranSacleTools = 0;
  if (m_execPrescale) {
    _prescaleDecision = m_scalerTool->decision(m_execPrescale);
  } 
  if (outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG << "Deciding if we run the ScaleTools on this event:"
       << " doOperationalInfo=" << m_doOperationalInfo
       << " [descision=" << opiLevel << "]"
       << ". execPrescale=" << m_execPrescale 
       << " [descision=" << _prescaleDecision << "]" << endreq;
  }
  if(m_writeAlways || (opiLevel > 0 && m_execPrescale > 0.0 && _prescaleDecision == true)) {
    for(unsigned i = 0; i < m_scaleTools.size(); ++i) { 
      if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Running Event ScaleTools " << i << ", " << m_scaleTools[i]->type() << endreq;
      m_scaleTools[i]->Fill(*event); 
    }
    // Also use the EB weight tool
    if (m_doEBWeight) m_toolEBWeight->Fill(*event);
    _ranSacleTools = 1;
  } else if (outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG << "NOT Running ScaleTools on this event," << endreq;
  }
  event->addVar(47, _ranSacleTools);

  //
  // Fill basic event data
  //
  ProcessEvent(*event);
  
  if(monitoringEvent || m_writeAlways) { // If passed CostMon chain or WriteAlways is true (i.e. CAF mode)
    //
    // Write counts for previous events
    //
    SavePrevLumi(*event);
    
    //
    // Process always tools
    //
    if(m_writeAlways) {
      for(unsigned i = 0; i < m_alwaysTools.size(); ++i) {
        if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Running Event AlwaysTools " << i << ", " << m_alwaysTools[i]->type() << endreq;
        m_alwaysTools[i]->Fill(*event);
      }
    }

    //
    // Write out data with monitoring event
    //
    HLT::Navigation *navig =
      const_cast<HLT::Navigation *>(m_parentAlg->getAlgoConfig()->getNavigation());
    const std::string level = m_parentAlg->getAlgoConfig()->getInstance();

    if(!navig) {
      log() << MSG::WARNING << "Failed to get HLT::Navigation pointer. Cannot save this cost event!" << endreq;
      return StatusCode::SUCCESS;
    }    

    if(!m_bufferConfig.empty()) {

      std::string config_key;
      std::string config_label = "OPI"+level+"_monitoring_config";
      
      log() << MSG::DEBUG << "Write collection: " << config_label << endreq;

      for(unsigned int i = 0; i < m_bufferConfig.size(); ++i) {
        navig -> attachFeature<TrigMonConfig>(navig->getInitialNode(),
                m_bufferConfig[i],
                HLT::Navigation::ObjectCreatedByNew,
                config_key, config_label);
        log() << MSG::DEBUG << "Attached Config " << config_key << ": keyset " << m_bufferConfig[i]->getMasterKey()
          << "," << m_bufferConfig[i]->getLV1PrescaleKey() << "," << m_bufferConfig[i]->getHLTPrescaleKey() << endreq;

      }
    }

    if(!m_bufferEvents.empty()) {

      std::string event_key;
      std::string event_label = "OPI"+level+"_monitoring_event";

      log() << MSG::DEBUG << "Write collection: " << event_label << endreq;
      
      for(unsigned int i = 0; i < m_bufferEvents.size(); ++i) {
        navig -> attachFeature<TrigMonEvent>(navig->getInitialNode(),
               m_bufferEvents[i],
               HLT::Navigation::ObjectCreatedByNew,
               event_key, event_label);
        log() << MSG::DEBUG << "Attached Event " << event_key << ": event " << m_bufferEvents[i]->getEvent()
          << " lumi " << m_bufferEvents[i]->getLumi() << endreq;
      }
    }

    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG
      << "Attached to Navigation " << m_bufferConfig.size() << " TrigMonConfigs" << endreq
      << "Attached to Navigation " << m_bufferEvents.size()  << " TrigMonEvents" << endreq;
    }

    m_exportedEvents += m_bufferEvents.size();
    m_exportedConfig += m_bufferConfig.size();
    m_bufferEvents.clear();
    m_bufferConfig.clear();
  }

  double event_timer = 0.0;
  if(m_timer) {
    m_timer->stop();
    event_timer = m_timer->elapsed();

    if(m_saveEventTimers) {
      event->addVar(m_keyTimer, m_timer->elapsed());
    }
  }

  if(outputLevel() <= MSG::DEBUG) { 
    log() << MSG::DEBUG 
    << "Processed run #" << eventInfo->runNumber()
    << " lb #" << eventInfo->lumiBlock()
    << " event #" << eventInfo->eventNumber() << endreq
    << "  steeringOPILevel  = " << opiLevel << endreq
    << "  isMonitoringEvent = " << monitoringEvent << endreq
    << "  event buffer size = " << m_bufferEvents.size() << endreq
    << "  elapsed time=" << event_timer << " ms" << endreq;

    if(m_countEvent <= 1 && outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "First two events StoreGate Dump:" << endreq;
      log() << MSG::DEBUG << m_storeGate->dump() << endreq;
    }

    if(m_printEvent) Trig::Print(*event, m_config_sv, log(), MSG::DEBUG);
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostTool::finalHists()
{
  //
  // Finalize output and cleanup internal state
  //

  if(!m_active) return StatusCode::SUCCESS;

  log() << MSG::DEBUG  << "finalHists()" << endreq;

  //
  // Online run or offline job has ended: cleanup everything
  //
  ClearBeforeNewRun();
  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostTool::setupOutputStreams(std::vector<std::string>)
{
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool TrigCostTool::preSelector()
{
  //
  // If this function returns false than the tool will be run
  //
  return false;
}

//---------------------------------------------------------------------------------------
void TrigCostTool::ProcessConfig(xAOD::EventInfo* info)
{ 
  //
  // Process configuration
  //
  
  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG << "Attempting to fill configuration..." << endreq;
  }

  // TimM - add LB check here too - we do want to be able to get different key sets in differnt LBs
  if(m_run == info->runNumber() && m_lumi == info->lumiBlock()) {
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "Trigger configuration for run " << m_run << " LB " << m_lumi << " already saved." << endreq;
    }
    return;
  }

  m_run  = info->runNumber();
  m_lumi = info->lumiBlock();

  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG
    << "ProcessConfig - run number = " << m_run << " lumi block = " << m_lumi << " event number = " << info->eventNumber() << endreq;
  }

  // Should we use the Config Service to collect the current keys for this LB and Run Number? (trying this one first)
  if (m_useConfSvc && m_toolConf) {

    //m_config_sv.clear(); //TimM - don't clear here as the code will think it only needs to update the lumiblock info
    // TimM TODO in future this should not be quite so round-the-house-ey
    m_config_sv.setEventID(info->eventNumber(),
               info->lumiBlock(),
               info->runNumber(),
               info->timeStamp(),
               info->timeStampNSOffset());

    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "ProcessConfig - Attempting load from ConfService " << endreq;
    }

    m_toolConf->SetOption(1); //1 = ConfSvc
    if(!m_toolConf->Fill(&m_config_sv)) {
      log() << MSG::WARNING << "Failed to fill TrigMonConfig from ConfSvc... set inactive state. CostMon is now DISABLED." << endreq;
      m_active = false; return;
    }

    for(unsigned i = 0; i < m_eventTools.size(); ++i) {
      if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Running ConvSvc Config Tool " << i << ", " << m_eventTools[i]->type() << endreq;
      m_eventTools[i]->Fill(&m_config_sv);
    }
    for(unsigned i = 0; i < m_scaleTools.size(); ++i) {
      if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Running ConvSvc Config Scale Tool " << i << ", " << m_scaleTools[i]->type() << endreq;
      m_scaleTools[i]->Fill(&m_config_sv);
    }
    if(m_writeAlways) {
      for(unsigned i = 0; i < m_alwaysTools.size(); ++i) {
        if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Running ConvSvc Config Always Tool " << i << ", " << m_alwaysTools[i]->type() << endreq;
        m_alwaysTools[i]->Fill(&m_config_sv);
      }
    }

    // Online EBWeighting
    if (m_costForCAF == false && m_doEBWeight == true) {
      m_toolEBWeight->Fill(&m_config_sv);
    }

    //Offline, get the BunchGroup info at this point. TODO: Migrate to 16x BGs when the tool updates
    if (m_costForCAF == true) {
      unsigned _bunchGroupLength[8] = {0};
      _bunchGroupLength[0] = m_toolBunchGroup->nBunchGroup0();
      _bunchGroupLength[1] = m_toolBunchGroup->nBunchGroup1();
      _bunchGroupLength[2] = m_toolBunchGroup->nBunchGroup2();
      _bunchGroupLength[3] = m_toolBunchGroup->nBunchGroup3();
      _bunchGroupLength[4] = m_toolBunchGroup->nBunchGroup4();   
      _bunchGroupLength[5] = m_toolBunchGroup->nBunchGroup5();
      _bunchGroupLength[6] = m_toolBunchGroup->nBunchGroup6();
      _bunchGroupLength[7] = m_toolBunchGroup->nBunchGroup7();
      for (unsigned _bg = 0; _bg < 8; ++_bg) {
        std::stringstream _ssKey, _ssVal;
        _ssKey << "DB:BGRP" << _bg;
        _ssVal << _bunchGroupLength[_bg];
        if (outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Database DB:BGRP" << _bg << " size:" << _bunchGroupLength[_bg] << endreq;
        m_config_sv.addValue(_ssKey.str(), _ssVal.str());
      }
    }

    // Save the OPI prescale
    std::stringstream _ss1, _ss2;
    _ss1 << m_execPrescale;
    _ss2 << m_doOperationalInfo;
    m_config_sv.addValue("ExecPrescale", _ss1.str() );
    m_config_sv.addValue("doOperationalInfo", _ss2.str() );
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "ProcessConfig - Exporting the operational prescale " << m_execPrescale << endreq;
      log() << MSG::DEBUG << "ProcessConfig - Exporting the operational info frequency " << m_doOperationalInfo << endreq;
    }

    if(m_writeConfig || (m_writeAlways && m_level == "EF") || (m_writeAlways && m_level == "HLT")) {
      m_bufferConfig.push_back(new TrigMonConfig(m_config_sv));
      log() << MSG::INFO << "ProcessConfig - writing out full svc configuration" << endreq;
    }

  }

  // Should we use the DB to collect the online keys for this LB and Run Number?
  if (m_useConfDb && m_toolConf && m_costForCAF && m_doEBWeight) {

    //m_config_db.clear(); //see above - currently a bit messy the way this is run 
    m_config_db.setEventID(info->eventNumber(),
               info->lumiBlock(),
               info->runNumber(),
               info->timeStamp(),
               info->timeStampNSOffset());

    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "ProcessConfig - Attempting load from DB " << endreq;
    }

    m_toolConf->SetOption(2); //2 = DBAccess
    if(!m_toolConf->Fill(&m_config_db)) {
      log() << MSG::WARNING << "Failed to fill TrigMonConfig from DB..." << endreq;
    } else {

      // Load into EBWeighting (this is assuming CAF style processing, NOT online)
      m_toolEBWeight->Fill(&m_config_db);

      if(m_writeConfigDB || (m_writeAlways && m_level == "EF") || (m_writeAlways && m_level == "HLT")) {
        m_bufferConfig.push_back(new TrigMonConfig(m_config_db));
        log() << MSG::INFO << "ProcessConfig - writing out full DB configuration" << endreq; 
      }
    }
  }

  //
  // Write out trigger configuration if TRIG_COST_ENV="SaveTrigConfig"
  //
  char *cost_env = getenv("TRIG_COST_ENV");
  if(cost_env && 
     (std::strcmp(cost_env, "SaveTrigConfig") == 0 ||
      std::strcmp(cost_env, "Save") == 0)) {
    
    std::stringstream nameS;
    nameS << "TrigConf"
    << ".run_"     << std::setw(10) << std::setfill('0') << m_config_sv.getRun()
    << ".smk_"     << std::setw(10) << std::setfill('0') << m_config_sv.getMasterKey()
    << ".lv1_key_" << std::setw(10) << std::setfill('0') << m_config_sv.getLV1PrescaleKey()
    << ".hlt_key_" << std::setw(10) << std::setfill('0') << m_config_sv.getHLTPrescaleKey()
    << ".root";

    TDirectory *dir_cur = gDirectory;
    TFile file(nameS.str().c_str(), "RECREATE");
    
    if(file.IsOpen()) {
      log() << MSG::INFO << "Opened ROOT file:\n" << nameS.str() << endreq;
      
      TTree *tree = new TTree("config", "config");
      tree -> SetDirectory(&file);
      
      TrigMonConfig *config = new TrigMonConfig(m_config_sv);
      tree->Branch("conf", "TrigMonConfig", &config);
      tree->Fill();
      delete config;
    }
    
    file.Write();
    file.Close();
    gDirectory = dir_cur;
  }
}

//---------------------------------------------------------------------------------------
void TrigCostTool::ProcessEvent(TrigMonEvent &event)
{ 
  //
  // Counter number of events per lumi block
  //
  ++m_lumiCount.insert(std::map<unsigned, unsigned>::value_type(event.getLumi(), 0)).first->second;

  //
  // Save local host time and event count
  //
  struct timeval currentTime; currentTime.tv_sec = 0; currentTime.tv_usec = 0;
  gettimeofday(&currentTime, 0);
  event.addWord(currentTime.tv_sec);
  event.addWord(currentTime.tv_usec);

  //
  // Save DB keys
  //
  event.addVar(66, m_config_sv.getMasterKey());
  event.addVar(67, m_config_sv.getLV1PrescaleKey());
  event.addVar(68, m_config_sv.getHLTPrescaleKey());

  //
  // Read OPI from TrigSteer
  //
  float texec = 0.0, tproc = 0.0, tres = 0.0, tmon = 0.0;

  if(m_storeGate->transientContains<TrigOperationalInfoCollection>(m_keySteerOPI)) {

    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "Found TrigOperationalInfoCollection " << m_keySteerOPI << endreq;
    }    

    const TrigOperationalInfoCollection *opi = 0;
    if(m_storeGate->retrieve<TrigOperationalInfoCollection>(opi, m_keySteerOPI).isSuccess()) {

      for(TrigOperationalInfoCollection::const_iterator it = opi->begin(); it != opi->end(); ++it) {
        const TrigOperationalInfo *ptr = *it;
        if(!ptr) continue;
  
        if(ptr -> defined("Texec") == 1) texec = ptr -> get("Texec");
        if(ptr -> defined("Tproc") == 1) tproc = ptr -> get("Tproc");
        if(ptr -> defined("Tres")  == 1) tres  = ptr -> get("Tres");
        if(ptr -> defined("Tmon")  == 1) tmon  = ptr -> get("Tmon");
      }
    }
    
    if(m_saveEventTimers) {
      event.addVar(m_keyTimer+1, texec);
      event.addVar(m_keyTimer+2, tproc);
      event.addVar(m_keyTimer+3, tres);
      event.addVar(m_keyTimer+4, tmon);
    }
  } else if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG << "Could not find TrigOperationalInfoCollection " << m_keySteerOPI << " did the HLT process anything?" << endreq;
  }

  if(texec > 0.0) event.setTimer(texec);
  else            event.setTimer(tproc);

  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG << "Timer check: texec:" << texec 
      << ", tproc:" << tproc 
      << ", tres:" << tres 
      << ", tmon:" << tmon 
      << ", event.getTimer():" << event.getTimer() << endreq;
  }

  //
  // Read the lumi block length, only for offline - obviously!
  //
  if (m_costForCAF) {
    if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "Reading lumi length" << endreq;
    if (m_readLumiBlock.getTriedSetup() == false) {
      log() << MSG::INFO << "LumiBlockReader not setup (tried="<<m_readLumiBlock.getTriedSetup()<<") - fetching lumiblock lengths from COOL" << endreq;
      m_readLumiBlock.updateLumiBlocks(event.getRun());
      std::string _msg = m_readLumiBlock.infos();
      if (_msg.size()) log() << MSG::INFO << _msg;
      std::string _dbg = m_readLumiBlock.debug();
      if (_dbg.size() && outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << _dbg;
      m_readLumiBlock.clearMsg();
    }
    if(outputLevel() <= MSG::DEBUG) log() << MSG::DEBUG << "LB " << event.getLumi() << " is Length " << m_readLumiBlock.getLumiBlockLength(event.getLumi()) << endreq;
    event.addVar(43, m_readLumiBlock.getLumiBlockLength(event.getLumi())); // 43 is lumi block length location
  }

}

//---------------------------------------------------------------------------------------
void TrigCostTool::SavePrevLumi(TrigMonEvent &event)
{ 
  //
  // Save event count for current lumi block
  //
  const std::map<unsigned, unsigned>::const_iterator icurr = m_lumiCount.find(event.getLumi());
  unsigned curr_count = 0;

  if(icurr != m_lumiCount.end()) {
    curr_count = icurr->second;
  }
  else {
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "Missing current event count for lumi: " << event.getLumi() << endreq;
    }
  }

  //
  // Save counts for all previous lumi blocks
  //  
  for(std::map<unsigned, unsigned>::const_iterator it = m_lumiCount.begin(); it != m_lumiCount.end(); ++it) {
    //
    // Ignore current lumi block
    //
    if(it->first == event.getLumi()) {
      if(outputLevel() <= MSG::DEBUG) {
        log() << MSG::DEBUG << "Current event lumi=" << event.getLumi() << " - still counting..." << endreq;
      }
      continue;
    }
    
    const unsigned lumi = it->first;
    const unsigned nevt = it->second;

    event.addVar(10000+lumi, nevt);

    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "Current event lumi=" << event.getLumi() 
      << ": prev lumi=" << lumi << " event count=" << nevt << endreq;
    }
  }
  
  //
  // Save current event number
  //
  event.addVar(9999, m_countEvent);

  //
  // Erase all lumi block counts except for current lumi block
  //
  m_lumiCount.clear();
  m_lumiCount[event.getLumi()] = curr_count;
}

//---------------------------------------------------------------------------------------
namespace Cost
{
  void PrintStreams(const TriggerInfo &trig, MsgStream &msg, MSG::Level level)
  {    
    const std::vector<TriggerInfo::StreamTag> &streams = trig.streamTags();
    for(unsigned i = 0; i < streams.size(); ++i) {
      const TriggerInfo::StreamTag &stag = streams[i];      
      msg << level << "   stream " << i << ": " << stag.name() << "/" << stag.type() << endreq;
    }
  }
}

// NEW
namespace Cost
{
  void PrintStreams(xAOD::EventInfo* trig, MsgStream &msg, MSG::Level level)
  {    
    const std::vector<xAOD::EventInfo::StreamTag> &streams = trig->streamTags();
    for(unsigned i = 0; i < streams.size(); ++i) {
      const xAOD::EventInfo::StreamTag &stag = streams[i];      
      msg << level << "   stream " << i << ": " << stag.name() << "/" << stag.type() << endreq;
    }
  }
}

//---------------------------------------------------------------------------------------
bool TrigCostTool::IsMonitoringEvent(xAOD::EventInfo* info)
{
  //
  // Check if this event has monitoring stream
  // 

  // TIMM DO NOT MIGRATE TO xAOD HERE YET - NEEDS FIXING

  const std::vector< xAOD::EventInfo::StreamTag > &streams = info->streamTags(); //New

  // OLD METHOD
  const DataHandle<EventInfo> event_handle_old;
  m_storeGate -> retrieve(event_handle_old);
  TriggerInfo* info_OLD = event_handle_old->trigger_info();
  const std::vector<TriggerInfo::StreamTag> &streams_OLD = info_OLD->streamTags();
  // OLD METHOD

  // LOOK @ NEW METHOD
  if(streams.empty()) {
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "[xAOD::EventInfo] Event has no trigger streams" << endreq;
    }
  }
  else {
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "[xAOD::EventInfo] Event has " << streams.size() << " stream(s)" << endreq;
      Cost::PrintStreams(info, log(), MSG::DEBUG);
    }
  }

  
  if(streams_OLD.empty()) {
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "[EventInfo] Event has no trigger streams" << endreq;
    }

    return false;
  }
  else {
    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "[EventInfo] Event has " << streams_OLD.size() << " stream(s)" << endreq;
      Cost::PrintStreams(*info_OLD, log(), MSG::DEBUG);
    }
  }

  const bool noCalib   = m_monitoringLogic.find("no-calib")   != std::string::npos;
  const bool noDebug   = m_monitoringLogic.find("no-debug")   != std::string::npos;
  const bool noPhysics = m_monitoringLogic.find("no-physics") != std::string::npos;
  const bool calibOnly = m_monitoringLogic.find("calib-only") != std::string::npos;
  const bool oneStream = m_monitoringLogic.find("one-stream") != std::string::npos;

  bool costEvent = false;
  bool passLogic = true;

  //std::vector< xAOD::EventInfo::StreamTag > new_streams; //New
  std::vector<TriggerInfo::StreamTag> new_streams;

  for(unsigned i = 0; i < streams_OLD.size(); ++i) {
    const TriggerInfo::StreamTag &stag = streams_OLD.at(i);

    if(noCalib && stag.type() == "calibration" && stag.name() != m_monitoringStream) {
      passLogic = false;
      if(outputLevel() <= MSG::DEBUG)
        log() << MSG::DEBUG << "Calibration case - ignoring event because of stream: " << stag.name() << endreq;
    }

    if(noDebug && stag.type() == "debug") {
      passLogic = false;
      if(outputLevel() <= MSG::DEBUG)
        log() << MSG::DEBUG << "Debug case       - ignoring event because of stream: " << stag.name() << endreq;
    }

    if(noPhysics && stag.type() == "physics") {
      passLogic = false;
      if(outputLevel() <= MSG::DEBUG)
        log() << MSG::DEBUG << "Physics case     - ignoring event because of stream: " << stag.name() << endreq;
    }
  
    if(calibOnly && stag.type() != "calibration") {
      passLogic = false;
      if(outputLevel() <= MSG::DEBUG)
        log() << MSG::DEBUG << "Calibration only - ignoring event because of stream: " << stag.name() << endreq;
    }
  
    if(stag.name() == m_monitoringStream) {
      costEvent = true;
      if(outputLevel() <= MSG::DEBUG)
        log() << MSG::DEBUG << "Event has monitoring stream: " << m_monitoringStream << endreq;
    }
    else {
      new_streams.push_back(stag);
    }
  }

  if(oneStream && streams_OLD.size() != 1) {
    passLogic = false;
    if(outputLevel() <= MSG::DEBUG)
      log() << MSG::DEBUG << "Case 4 - ignoring event because of stream size=: " << streams_OLD.size() << endreq;
  }

  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG 
    << "  passLogic = " << passLogic << endreq
    << "  costEvent = " << costEvent << endreq;
  }

  if(m_purgeCostStream && costEvent && !passLogic) {
    
    info_OLD->setStreamTags(new_streams);

    //const std::vector< xAOD::EventInfo::StreamTag > new_streams_const(new_streams); // New
    //info->setStreamTags(new_streams_const); // New

    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "After stream purge event has " 
      << info_OLD->streamTags().size() << " stream(s)" << endreq;
      Cost::PrintStreams(*info_OLD, log(), MSG::DEBUG);
    }
  }

  return (costEvent && passLogic);
}

//---------------------------------------------------------------------------------------
void TrigCostTool::ClearBeforeNewRun(unsigned run)
{
  //
  // Clear internal state in preparation for new run
  //

  //
  // Run number is not zero and has not changed: do nothing
  //
  if(run != 0 && run == m_run) return;
  
  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG << "ClearBeforeNewRun: new run=" << run << " prev=" << m_run << endreq;
  }

  //
  // Reset internal state
  //
  m_run        = 0;
  m_lumi       = 0;
  m_countEvent = 0;
  
  //
  // Delete events from buffer
  //
  for(unsigned i = 0; i < m_bufferEvents.size(); ++i) {
    delete m_bufferEvents[i];
  }
  for(unsigned i = 0; i < m_bufferConfig.size(); ++i) {
    delete m_bufferConfig[i];
  }

  m_bufferEvents.clear();
  m_bufferConfig.clear();

  //
  // Clear event count for luminosity blocks
  //
  m_lumiCount.clear();
}
