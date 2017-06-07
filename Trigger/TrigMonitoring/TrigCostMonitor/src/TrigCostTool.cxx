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
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/DataHandle.h"
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
#include "TrigCostMonitor/TrigCostTool.h"
#include "TrigCostMonitor/TrigNtConfTool.h"
#include "TrigCostMonitor/UtilCost.h"

//---------------------------------------------------------------------------------------
TrigCostTool::TrigCostTool(const std::string& type,
         const std::string& name,
         const IInterface* parent)
  :AthAlgTool(type, name, parent),
   m_appId(0),
   m_appName(), 
   m_configReductionValue(0),
   m_doConfigReduction(0),
   m_parentAlg(0),
   m_timer(0),
   m_timerSvc("TrigTimerSvc/TrigTimerSvc", name),
   m_toolBunchGroup("BunchGroupTool", this),
   m_scalerTool("HLT::RandomScaler/TrigCostScaler", this),
   m_eventInfoAccessTool("HLT::EventInfoAccessTool/EventInfoAccessTool", this),
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
   m_exportedConfig(0),
   m_exportedEvents(0),
   m_costChainPS(0)
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("toolConf",      m_toolConf,            "Trigger configuration tool");
  declareProperty("toolEBWeight",  m_toolEBWeight,        "Enhanced bias weighting tool");  
  declareProperty("eventTools",    m_eventTools,          "Cost sub-tools run on every event");
  declareProperty("scaleTools",    m_scaleTools,          "Cost sub-tools run on events after prescale");
  declareProperty("alwaysTools",   m_alwaysTools,         "Cost sub-tools run on every event if always flag is set");
  declareProperty("scalerTool",    m_scalerTool,          "Scaler used to prescale the scaleTools");
  declareProperty("eventInfoTool", m_eventInfoAccessTool, "Tool to manipulate the event streaming"); 
  
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
  declareProperty("onlySaveCostEvent",m_onlySaveCostEvent= true, "Only save events which have passed the OPI prescale which run all scale tools");
  declareProperty("obeyCostChainPS",  m_obeyCostChainPS  = true, "Only monitor events if the cost chain prescale is > 0");

  declareProperty("configReductionValue", m_configReductionValue = 50, "Random chance, 1/Value, that this PU gets chosen to write out configs to T0.");
  declareProperty("doConfigReduction", m_doConfigReduction = false, "Reduce duplicate configs being sent by every PU online. Only send from 1/configReuctionValue PUs.");

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

  m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
  if(!m_parentAlg) {
    ATH_MSG_ERROR(" Unable to cast the parent algorithm to HLT::TrigSteer!");
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
    ATH_MSG_INFO("Set inactive state. Disabling TrigCost.");
    return StatusCode::SUCCESS;
  }

  // Update from old style "HLT_OPI_HLT"
  if(m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2) {
    m_keySteerOPI = "HLT_TrigOperationalInfoCollection_OPI_L2";
  }
  else if(m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF) {
    m_keySteerOPI = "HLT_TrigOperationalInfoCollection_OPI_EF";
  }
  else if(m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::HLT) {
    m_keySteerOPI = "HLT_TrigOperationalInfoCollection_OPI_HLT";
  }

  // Only get this tool offline
  if (m_costForCAF == true) {
    CHECK(m_toolBunchGroup.retrieve());
    ATH_MSG_INFO("Retrieved " << m_toolBunchGroup);
  }

  CHECK(m_toolConf.retrieve());
  ATH_MSG_INFO("Retrieved " << m_toolConf);

  CHECK(m_toolEBWeight.retrieve());
  ATH_MSG_INFO("Retrieved " << m_toolEBWeight);

  CHECK(m_eventTools.retrieve());
  ATH_MSG_INFO("Retrieved " << m_eventTools);

  CHECK(m_scaleTools.retrieve());
  ATH_MSG_INFO("Retrieved " << m_scaleTools);

  CHECK(m_eventInfoAccessTool.retrieve());
  ATH_MSG_INFO("Retrieved " << m_eventInfoAccessTool);
  
  CHECK(m_alwaysTools.retrieve());
  ATH_MSG_INFO("Retrieved " << m_alwaysTools);
  
  if(m_doTiming) { 
    CHECK(m_timerSvc.retrieve());
    ATH_MSG_INFO("Retrieved TrigTimerSvc");
    m_timer = m_timerSvc->addItem(m_parentAlg->name()+":CostMonitor:TotalTime");
  }

  if(m_execPrescale > 0.0) {
    CHECK(m_scalerTool.retrieve());
    ATH_MSG_INFO("Retrieved " << m_scalerTool);
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

  m_appId = m_configReductionValue;

  ATH_MSG_INFO("level            = " << m_level            );
  ATH_MSG_INFO("monitoringLogic  = " << m_monitoringLogic  );
  ATH_MSG_INFO("monitoringStream = " << m_monitoringStream );
  ATH_MSG_INFO("monitoringTarget = " << m_monitoringTarget );
  ATH_MSG_INFO("purgeCostStream  = " << m_purgeCostStream  );
  ATH_MSG_INFO("useConfDb        = " << m_useConfDb        );
  ATH_MSG_INFO("useConfSvc       = " << m_useConfSvc       );
  ATH_MSG_INFO("costForCAF       = " << m_costForCAF       ); 
  ATH_MSG_INFO("doEBWeight       = " << m_doEBWeight       ); 
  ATH_MSG_INFO("saveEventTimers  = " << m_saveEventTimers  );
  ATH_MSG_INFO("writeAlways      = " << m_writeAlways      );
  ATH_MSG_INFO("writeConfig      = " << m_writeConfig      );
  ATH_MSG_INFO("writeConfigDB    = " << m_writeConfigDB    );
  ATH_MSG_INFO("stopAfterNEvent  = " << m_stopAfterNEvent  );
  ATH_MSG_INFO("execPrescale     = " << m_execPrescale     );
  ATH_MSG_INFO("doOperationalInfo= " << m_doOperationalInfo);
  ATH_MSG_INFO("printEvent       = " << m_printEvent       );
  ATH_MSG_INFO("obeyCostChainPS  = " << m_obeyCostChainPS  );
  ATH_MSG_INFO("doConfigReduction= " << m_doConfigReduction);
  ATH_MSG_INFO("configReducValue = " << m_configReductionValue);

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostTool::finalize()
{
  //
  // Clean up
  //
  ATH_MSG_DEBUG("finalize()");

  ATH_MSG_INFO("Cost tool active? " << (m_active == true ? "YES" : "NO!!"));
  ATH_MSG_INFO("During run, attached to Navigation (exported): " << m_exportedConfig << " TrigMonConfigs");
  ATH_MSG_INFO("During run, attached to Navigation (exported): " << m_exportedEvents << " TrigMonEvents");

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostTool::bookHists()
{ 
  //
  // Book hists - executed once per offline job and for every new run online
  //
  if(!m_active) return StatusCode::SUCCESS;

  ATH_MSG_DEBUG("bookHists()");

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
  //   ATH_MSG_DEBUG("No EventInfo in bookHists()... will try again in fillHists()");
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
  if(evtStore() -> retrieve(eventInfo).isFailure()) {
    ATH_MSG_WARNING("Failed to read event info - set state to inactive. CostMon is now DISABLED.");
    m_active = false;
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG( "---------------> starting new event #" << m_countEvent << " (online #" << eventInfo->eventNumber() << ")" );

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
  bool monitoringEvent = false; // Only write out on monitoring events
  CHECK( IsMonitoringEvent(monitoringEvent) ); // Have I passed the CostMon chain and no other chains? (changes value of monitoringEvent)

  const unsigned opiLevel = m_parentAlg->getAlgoConfig()->getSteeringOPILevel(); // Have I passed the OPI "prescale"?

  // Get My PU name
  if (m_appId == m_configReductionValue) {
    const HLT::HLTResult *hlt_result = 0;
    if(!evtStore()->contains<HLT::HLTResult>(m_hltResult)) {
      ATH_MSG_DEBUG("StoreGate does not contain HLTResult: " << m_hltResult);
    } else if(evtStore()->retrieve<HLT::HLTResult>(hlt_result, m_hltResult).isFailure() || !hlt_result) {
       ATH_MSG_DEBUG("Failed to retrieve HLTResult: " << m_hltResult);
    } else {
      const std::vector<uint32_t> &extraData = hlt_result->getExtras();
      if(extraData.empty()) {
        ATH_MSG_DEBUG("Extra data is empty");
      } else {
        StringSerializer().deserialize(extraData, m_appName); 
        m_appId = TrigConf::HLTUtils::string2hash(m_appName, "APP_ID_TCT");
        ATH_MSG_DEBUG("Running on " << m_appName << " with hash " << m_appId);
      }
    }
  }

  // Have I passed my own personal "prescale" (not this is currently not used)
  bool prescaleDecision = true;
  if (m_execPrescale) {
    prescaleDecision = m_scalerTool->decision(m_execPrescale);
  } 

  // If only saving full cost events then make sure pass internal and OPI prescales
  bool runTools = true; // Do we create a new TrigMonEvent and fill it with stuff?
  int ranSacleTools = 0; // Set to 1 if we additioanlly run the scale tools (full mon event)
  if (m_onlySaveCostEvent == true && m_writeAlways == false) {
    if (opiLevel == 0 || m_execPrescale == 0 || prescaleDecision == 0) {
      ATH_MSG_DEBUG("Not a full monitoring event, not processing. To save basic info for all events set onlySaveCostEvent=0");
      runTools = false;
    }
  }

  // If only running when the chain is active (default) and we're online (costForCAF false)
  if (m_obeyCostChainPS == true && m_costForCAF == false) {
    if (m_costChainPS <= 0.) runTools = false;
  }
  ATH_MSG_DEBUG("Obey cost chain PS=" << m_obeyCostChainPS << ", CostChainPS=" << m_costChainPS << " costForCAF=" << m_costForCAF << " and runTools=" << runTools);

  if(m_stopAfterNEvent > 0 && m_bufferEvents.size() >= m_stopAfterNEvent) { // Have we filled up the buffer? This is bad!
    delete m_bufferEvents.front();
    m_bufferEvents.erase(m_bufferEvents.begin());
    ATH_MSG_WARNING("Reached CostMon buffer limit... deleted 1st event. Lost statistics!! Buffer size = " << m_bufferEvents.size());
  }

  // Only for full monitoring events, or for all events if m_onlySaveCostEvent=false (but scale tools are still not run in this case)
  // Note that if writeAlways is true, runTools will be true too
  TrigMonEvent *event = 0;
  if (runTools) {

    event = new TrigMonEvent();
    m_bufferEvents.push_back(event);
    
    event->setEventID(eventInfo->eventNumber(),
          eventInfo->lumiBlock(),
          eventInfo->bcid(),
          eventInfo->runNumber(),
          eventInfo->timeStamp(),
          eventInfo->timeStampNSOffset());    

    // Run the per event tools
    for(unsigned i = 0; i < m_eventTools.size(); ++i) { 
      ATH_MSG_DEBUG( "Running EventTool " << i << ", " << m_eventTools[i]->type() );
      m_eventTools[i]->Fill(*event); 
    }
  

    ATH_MSG_DEBUG( "Deciding if we run the ScaleTools on this event (full monitorng event):"
         << " doOperationalInfo=" << m_doOperationalInfo
         << " [descision=" << opiLevel << "]"
         << ". execPrescale=" << m_execPrescale 
         << " [descision=" << prescaleDecision << "]" );
    if(m_writeAlways || (opiLevel > 0 && m_execPrescale > 0.0 && prescaleDecision == true)) {
      for(unsigned i = 0; i < m_scaleTools.size(); ++i) { 
        ATH_MSG_DEBUG( "Running Event ScaleTools " << i << ", " << m_scaleTools[i]->type() );
        m_scaleTools[i]->Fill(*event); 
      }
      // Also use the EB weight tool
      if (m_doEBWeight) m_toolEBWeight->Fill(*event);
      ranSacleTools = 1;
    } else {
      ATH_MSG_DEBUG( "NOT Running ScaleTools on this event. Not a full monitoring event." );
    }
    event->addVar(Trig::kIsCostEvent, ranSacleTools);

    //
    // Fill basic event data
    //
    ProcessEvent(*event);
  }
  
  if(monitoringEvent || m_writeAlways) { // If passed CostMon chain or WriteAlways is true (i.e. CAF mode)
    //
    // Write counts for previous events
    //
    if (event) SavePrevLumi(*event);
    
    //
    // Process always tools. m_writeAlways implied runTools, but check 'event' non null to be careful. Also we currenly have no alwaysTools
    //
    if(m_writeAlways && event) {
      for(unsigned i = 0; i < m_alwaysTools.size(); ++i) {
        ATH_MSG_DEBUG( "Running Event AlwaysTools " << i << ", " << m_alwaysTools[i]->type() );
        m_alwaysTools[i]->Fill(*event);
      }
    }

    //
    // Write out data with monitoring event
    //
    HLT::Navigation *navig = const_cast<HLT::Navigation *>(m_parentAlg->getAlgoConfig()->getNavigation());
    const std::string level = m_parentAlg->getAlgoConfig()->getInstance();

    if(!navig) {
      ATH_MSG_WARNING("Failed to get HLT::Navigation pointer. Cannot save this cost event!");
      return StatusCode::SUCCESS;
    }    

    // See if this PU is writing CONFIG data
    if (m_doConfigReduction == true) {
      if (m_appId % m_configReductionValue != 0) {
        m_bufferConfig.clear();
        ATH_MSG_DEBUG( "This PU, " << m_appName << ", hash " << m_appId << " is not divisible by " << m_configReductionValue << " NOT writing");
      } else {
        ATH_MSG_DEBUG( "This PU, " << m_appName << ", hash " << m_appId << " is divisible by " << m_configReductionValue << " writing config");
      }
    }

    if(!m_bufferConfig.empty()) {

      std::string config_key;
      std::string config_label = "OPI"+level+"_monitoring_config";
      
      ATH_MSG_DEBUG( "Write collection: " << config_label );

      for(unsigned int i = 0; i < m_bufferConfig.size(); ++i) {
        navig -> attachFeature<TrigMonConfig>(navig->getInitialNode(),
                m_bufferConfig[i],
                HLT::Navigation::ObjectCreatedByNew,
                config_key, config_label);
        ATH_MSG_DEBUG( "Attached Config " << config_key << ": keyset " << m_bufferConfig[i]->getMasterKey()
          << "," << m_bufferConfig[i]->getLV1PrescaleKey() << "," << m_bufferConfig[i]->getHLTPrescaleKey() );

      }
    }

    if(!m_bufferEvents.empty()) {

      std::string event_key;
      std::string event_label = "OPI"+level+"_monitoring_event";

      ATH_MSG_DEBUG( "Write collection: " << event_label );
      
      for(unsigned int i = 0; i < m_bufferEvents.size(); ++i) {
        navig -> attachFeature<TrigMonEvent>(navig->getInitialNode(),
               m_bufferEvents[i],
               HLT::Navigation::ObjectCreatedByNew,
               event_key, event_label);
        ATH_MSG_DEBUG( "Attached Event " << event_key << ": event " << m_bufferEvents[i]->getEvent() << " lumi " << m_bufferEvents[i]->getLumi() );
      }
    }

    ATH_MSG_DEBUG("Attached to Navigation " << m_bufferConfig.size() << " TrigMonConfigs" );
    ATH_MSG_DEBUG("Attached to Navigation " << m_bufferEvents.size()  << " TrigMonEvents" );

    m_exportedEvents += m_bufferEvents.size();
    m_exportedConfig += m_bufferConfig.size();
    m_bufferEvents.clear();
    m_bufferConfig.clear();
  }

  double event_timer = 0.0;
  if(m_timer) {
    m_timer->stop();
    event_timer = m_timer->elapsed();

    if(m_saveEventTimers && event) {
      event->addVar(Trig::kTimeCostMonitoring, m_timer->elapsed());
    }
  }

  if(msgLvl(MSG::DEBUG)) { 
    ATH_MSG_DEBUG("Processed run #" << eventInfo->runNumber() << " lb #" << eventInfo->lumiBlock() << " event #" << eventInfo->eventNumber() );
    ATH_MSG_DEBUG("  steeringOPILevel  = " << opiLevel );
    ATH_MSG_DEBUG("  isMonitoringEvent = " << monitoringEvent << ", ranTools = " << runTools << ", ranScaleTools = " << ranSacleTools );
    ATH_MSG_DEBUG("  event buffer size = " << m_bufferEvents.size() );
    ATH_MSG_DEBUG("  elapsed time=" << event_timer << " ms" );

    if(m_countEvent <= 1) {
      ATH_MSG_DEBUG("First two events StoreGate Dump:" );
      ATH_MSG_DEBUG( evtStore()->dump() );
    }

    if(m_printEvent && event) Trig::Print(*event, m_config_sv, msg(), MSG::DEBUG);
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

  ATH_MSG_DEBUG( "finalHists()" );

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
  
  ATH_MSG_DEBUG( "Attempting to fill configuration..." );

  // TimM - add LB check here too - we do want to be able to get different key sets in differnt LBs
  if(m_run == info->runNumber() && m_lumi == info->lumiBlock()) {
    ATH_MSG_DEBUG( "Trigger configuration for run " << m_run << " LB " << m_lumi << " already saved." );
    return;
  }

  m_run  = info->runNumber();
  m_lumi = info->lumiBlock();

  ATH_MSG_DEBUG( "ProcessConfig - run number = " << m_run << " lumi block = " << m_lumi << " event number = " << info->eventNumber() );

  // Should we use the Config Service to collect the current keys for this LB and Run Number? (trying this one first)
  if (m_useConfSvc && m_toolConf) {

    //m_config_sv.clear(); //TimM - don't clear here as the code will think it only needs to update the lumiblock info
    // TimM TODO in future this should not be quite so round-the-house-ey
    m_config_sv.setEventID(info->eventNumber(),
               info->lumiBlock(),
               info->runNumber(),
               info->timeStamp(),
               info->timeStampNSOffset());

    ATH_MSG_DEBUG( "ProcessConfig - Attempting load from ConfService " );
    m_toolConf->SetOption(1); //1 = ConfSvc  


    if(!m_toolConf->Fill(&m_config_sv)) {
      ATH_MSG_DEBUG( "Failed to fill TrigConf from ConfigSvc - but this could have been because it did not change.");
    } else { // FILL WAS SUCCESSFULL

      for(unsigned i = 0; i < m_eventTools.size(); ++i) {
        ATH_MSG_DEBUG( "Running ConvSvc Config Tool " << i << ", " << m_eventTools[i]->type() );
        m_eventTools[i]->Fill(&m_config_sv);
      }
      for(unsigned i = 0; i < m_scaleTools.size(); ++i) {
        ATH_MSG_DEBUG( "Running ConvSvc Config Scale Tool " << i << ", " << m_scaleTools[i]->type() );
        m_scaleTools[i]->Fill(&m_config_sv);
      }
      if(m_writeAlways) {
        for(unsigned i = 0; i < m_alwaysTools.size(); ++i) {
          ATH_MSG_DEBUG( "Running ConvSvc Config Always Tool " << i << ", " << m_alwaysTools[i]->type() );
          m_alwaysTools[i]->Fill(&m_config_sv);
        }
      }

      // Online EBWeighting
      if (m_costForCAF == false && m_doEBWeight == true) {
        m_toolEBWeight->Fill(&m_config_sv);
      }

      //Offline, get the BunchGroup info at this point. TODO: Migrate to 16x BGs when the tool updates
      if (m_costForCAF == true) {
        unsigned bunchGroupLength[8] = {0};
        bunchGroupLength[0] = m_toolBunchGroup->nBunchGroup0();
        bunchGroupLength[1] = m_toolBunchGroup->nBunchGroup1();
        bunchGroupLength[2] = m_toolBunchGroup->nBunchGroup2();
        bunchGroupLength[3] = m_toolBunchGroup->nBunchGroup3();
        bunchGroupLength[4] = m_toolBunchGroup->nBunchGroup4();   
        bunchGroupLength[5] = m_toolBunchGroup->nBunchGroup5();
        bunchGroupLength[6] = m_toolBunchGroup->nBunchGroup6();
        bunchGroupLength[7] = m_toolBunchGroup->nBunchGroup7();
        for (unsigned bg = 0; bg < 8; ++bg) {
          std::stringstream ssKey, ssVal;
          ssKey << "DB:BGRP" << bg;
          ssVal << bunchGroupLength[bg];
          ATH_MSG_DEBUG( "Database DB:BGRP" << bg << " size:" << bunchGroupLength[bg] );
          m_config_sv.addValue(ssKey.str(), ssVal.str());
        }
      }

      // Save the OPI prescale
      std::stringstream ss1, ss2;
      ss1 << m_execPrescale;
      ss2 << m_doOperationalInfo;
      m_config_sv.addValue("ExecPrescale", ss1.str() );
      m_config_sv.addValue("doOperationalInfo", ss2.str() );
      ATH_MSG_DEBUG("ProcessConfig - Exporting the operational prescale " << m_execPrescale );
      ATH_MSG_DEBUG("ProcessConfig - Exporting the operational info frequency " << m_doOperationalInfo );

      //Get the prescale of the cost chain
      const std::vector<TrigConfChain> chains = m_config_sv.getVec<TrigConfChain>();
      for(unsigned int i = 0; i < chains.size(); ++i) {
        if ( chains.at(i).getChainName().find("costmonitor") != std::string::npos ) {
          m_costChainPS =  chains.at(i).getPrescale();
          ATH_MSG_DEBUG( "Got CostChain prescale: " << m_costChainPS);
        }
      }

      bool writeConfig = true;
      if      (m_writeAlways == true) writeConfig = true; // First check WriteAlways flag
      else if (m_writeConfig == false) writeConfig = false; // Next comes WriteConfig flag. This is normally true.
      else if (m_costForCAF == true) writeConfig = true; // If offline, then we don't have any other preconditions
      else if (m_obeyCostChainPS == true && m_costChainPS > 0) writeConfig = true; // Online, we normally only write out if we are in stable beams
      else if (m_obeyCostChainPS == true && m_costChainPS <= 0) writeConfig = false;
      
      if(writeConfig) {
        m_bufferConfig.push_back(new TrigMonConfig(m_config_sv));
        ATH_MSG_DEBUG( "ProcessConfig - writing out full svc configuration" );
      }
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

    ATH_MSG_DEBUG( "ProcessConfig - Attempting load from DB " );

    m_toolConf->SetOption(2); //2 = DBAccess
    if(!m_toolConf->Fill(&m_config_db)) {
      ATH_MSG_DEBUG( "Failed to fill TrigMonConfig from DB... but this could be because it did not change" );
    } else {
      // Load into EBWeighting (this is assuming CAF style processing, NOT online)
      m_toolEBWeight->Fill(&m_config_db);

      if(m_writeConfigDB || (m_writeAlways && m_level == "EF") || (m_writeAlways && m_level == "HLT")) {
        m_bufferConfig.push_back(new TrigMonConfig(m_config_db));
        ATH_MSG_DEBUG("ProcessConfig - writing out full DB configuration");
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
      ATH_MSG_INFO("Opened ROOT file:\n" << nameS.str());
      
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
  event.addVar(Trig::kSMK, m_config_sv.getMasterKey());
  event.addVar(Trig::kL1PSK, m_config_sv.getLV1PrescaleKey());
  event.addVar(Trig::kHLTPSK, m_config_sv.getHLTPrescaleKey());

  //
  // Read OPI from TrigSteer
  //
  float texec = 0.0, tproc = 0.0, tres = 0.0, tmon = 0.0;

  if(evtStore()->transientContains<TrigOperationalInfoCollection>(m_keySteerOPI)) {

    ATH_MSG_DEBUG( "Found TrigOperationalInfoCollection " << m_keySteerOPI );

    const TrigOperationalInfoCollection *opi = 0;
    if(evtStore()->retrieve<TrigOperationalInfoCollection>(opi, m_keySteerOPI).isSuccess()) {

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
      event.addVar(Trig::kTimeExec, texec);
      event.addVar(Trig::kTimeProc, tproc);
      event.addVar(Trig::kTimeRes, tres);
      event.addVar(Trig::kTimeMon, tmon);
    }
  } else {
   ATH_MSG_DEBUG( "Could not find TrigOperationalInfoCollection " << m_keySteerOPI << " did the HLT process anything?" );
  }

  if(texec > 0.0) event.setTimer(texec);
  else            event.setTimer(tproc);

  ATH_MSG_DEBUG( "Timer check: texec:" << texec 
      << ", tproc:" << tproc 
      << ", tres:" << tres 
      << ", tmon:" << tmon 
      << ", event.getTimer():" << event.getTimer() );

  //
  // Read the lumi block length, only for offline - obviously!
  //
  if (m_costForCAF) {
    ATH_MSG_DEBUG( "Reading lumi length" );
    if (m_readLumiBlock.getTriedSetup() == false) {
      ATH_MSG_INFO( "LumiBlockReader not setup (tried="<<m_readLumiBlock.getTriedSetup()<<") - fetching lumiblock lengths from COOL" );
      m_readLumiBlock.updateLumiBlocks(event.getRun());
      std::string msg = m_readLumiBlock.infos();
      if (msg.size()) ATH_MSG_INFO(msg);
      std::string dbg = m_readLumiBlock.debug();
      if (dbg.size()) ATH_MSG_DEBUG(dbg);
      m_readLumiBlock.clearMsg();
    }
    ATH_MSG_DEBUG( "LB " << event.getLumi() << " is Length " << m_readLumiBlock.getLumiBlockLength(event.getLumi()) );
    event.addVar(Trig::kEventLumiBlockLength, m_readLumiBlock.getLumiBlockLength(event.getLumi())); // 43 is lumi block length location
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
    ATH_MSG_DEBUG( "Missing current event count for lumi: " << event.getLumi() );
  }

  //
  // Save counts for all previous lumi blocks
  //  
  for(std::map<unsigned, unsigned>::const_iterator it = m_lumiCount.begin(); it != m_lumiCount.end(); ++it) {
    //
    // Ignore current lumi block
    //
    if(it->first == event.getLumi()) {
      ATH_MSG_DEBUG("Current event lumi=" << event.getLumi() << " - still counting...");
      continue;
    }
    
    const unsigned lumi = it->first;
    const unsigned nevt = it->second;

    event.addVar(10000+lumi, nevt);

    ATH_MSG_DEBUG("Current event lumi=" << event.getLumi() << ": prev lumi=" << lumi << " event count=" << nevt);
  }
  
  //
  // Save current event number
  //
  event.addVar(Trig::kEventNumber, m_countEvent);

  //
  // Erase all lumi block counts except for current lumi block
  //
  m_lumiCount.clear();
  m_lumiCount[event.getLumi()] = curr_count;
}

//---------------------------------------------------------------------------------------
StatusCode TrigCostTool::IsMonitoringEvent(bool& monitoringEvent)
{
  //
  // Check if this event has monitoring stream
  // 
  std::vector< xAOD::EventInfo::StreamTag > streams;
  CHECK( m_eventInfoAccessTool->getStreamTags(streams) ); //Get streams

  const bool noCalib   = m_monitoringLogic.find("no-calib")   != std::string::npos;
  const bool noDebug   = m_monitoringLogic.find("no-debug")   != std::string::npos;
  const bool noPhysics = m_monitoringLogic.find("no-physics") != std::string::npos;
  const bool calibOnly = m_monitoringLogic.find("calib-only") != std::string::npos;
  const bool oneStream = m_monitoringLogic.find("one-stream") != std::string::npos;

  bool costEvent = false;
  bool passLogic = true;

  std::vector< xAOD::EventInfo::StreamTag > streamTagPurgeList; // List of StreamTags to be removed if not saving cost data

  for(unsigned i = 0; i < streams.size(); ++i) {
    const xAOD::EventInfo::StreamTag &stag = streams.at(i);

    if(noCalib && stag.type() == "calibration" && stag.name() != m_monitoringStream) {
      passLogic = false;
      ATH_MSG_DEBUG("Calibration case - ignoring event because of stream: " << stag.name());
    }

    if(noDebug && stag.type() == "debug") {
      passLogic = false;
      ATH_MSG_DEBUG("Debug case       - ignoring event because of stream: " << stag.name());
    }

    if(noPhysics && stag.type() == "physics") {
      passLogic = false;
      ATH_MSG_DEBUG("Physics case     - ignoring event because of stream: " << stag.name());
    }
  
    if(calibOnly && stag.type() != "calibration") {
      passLogic = false;
      ATH_MSG_DEBUG("Calibration only - ignoring event because of stream: " << stag.name());
    }
  
    if(stag.name() == m_monitoringStream) {
      costEvent = true;
      ATH_MSG_DEBUG("Event has monitoring stream: " << m_monitoringStream);
      // If the pass logic ends up false and stream purge is on, we'll want to remove this stream
      streamTagPurgeList.push_back( stag );
    }

  }

  if(oneStream && streams.size() != 1) {
    passLogic = false;
    ATH_MSG_DEBUG("Case 4 - ignoring event because of stream size=: " << streams.size());
  }

  ATH_MSG_DEBUG("  passLogic = " << passLogic);
  ATH_MSG_DEBUG("  costEvent = " << costEvent);

  if(m_purgeCostStream && costEvent && !passLogic) {
    ATH_MSG_DEBUG("Failed Pass Logic, purge the Cost stream tag from the event to save writing out empty RAW event to SFO");
    CHECK( m_eventInfoAccessTool->removeStreamTags(streamTagPurgeList) ); // Remove stream tags 
  }

  monitoringEvent = (costEvent && passLogic);
  return StatusCode::SUCCESS;
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
  
  ATH_MSG_DEBUG("ClearBeforeNewRun: new run=" << run << " prev=" << m_run);

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
