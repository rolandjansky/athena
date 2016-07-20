/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTTOOL_H
#define TRIGCOSTTOOL_H

//
// Tool for collecting timing and other operational information online
//

// Framework
#include "AthenaMonitoring/IMonitorToolBase.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "CoolLumiUtilities/BunchGroupTool.h"

// Trigger
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigSteering/Scaler.h"
#include "TrigSteering/EventInfoAccessTool.h"

#include "xAODEventInfo/EventInfo.h"

// Trigger
#include "TrigMonitoringEvent/TrigMonConfigCollection.h"
#include "TrigMonitoringEvent/TrigMonEventCollection.h"

#include "TrigCostMonitor/ITrigNtTool.h"
#include "TrigCostMonitor/ReadLumiBlock.h"

class TrigTimer;
class TriggerInfo;

class TrigCostTool : public AthAlgTool, virtual public IMonitorToolBase {
 public:
  
  TrigCostTool(const std::string &type,
	       const std::string &name,
	       const IInterface* parent);
  virtual ~TrigCostTool();
  
  StatusCode initialize();
  StatusCode finalize();

  StatusCode bookHists();
  StatusCode fillHists();
  StatusCode finalHists();

  StatusCode setupOutputStreams(std::vector<std::string> Mapping
				= std::vector<std::string>());

  bool preSelector();

  StatusCode runStat()         { return StatusCode::SUCCESS; }
  StatusCode checkHists(bool)  { return StatusCode::SUCCESS; }
  StatusCode convertLWHists()  { return StatusCode::SUCCESS; }

 private:
  
  void ProcessConfig(xAOD::EventInfo* info);

  void ProcessEvent(TrigMonEvent &event);
  void SavePrevLumi(TrigMonEvent &event);

  StatusCode IsMonitoringEvent(bool& monitoringEvent);
  
  void ClearBeforeNewRun(unsigned run = 0);

  uint32_t GetL1PSKFromCTPfragment();

 private:
  
  // Tool properties:
  std::string  m_level;            // Trigger level
  std::string  m_monitoringLogic;  // Logic for deciding output streams3
  std::string  m_monitoringStream; // SFO stream where data is sent
  std::string  m_monitoringTarget; // Monitoring target value
  std::string  m_hltResult;        // Storegate key of trigger result

  unsigned     m_appId;   // Application id   for current event (HLT result) - TimM moving into CostTool from CostRun
  std::string  m_appName; // Application name for current event (HLT result) - TimM moving into CostTool from CostRun

  bool         m_doTiming;         // Collect timing information
  bool         m_printEvent;       // Print event summary
  bool         m_purgeCostStream;  // Remove cost stream when no cost data is attached
  bool         m_useConfDb;        // Use the trigger configuration database
  bool         m_useConfSvc;       // Use the trigger configuration service
  bool         m_costForCAF;       // (Re)running the trigger outside of P1
  bool         m_doEBWeight;       // Calculate EB weight
  bool         m_saveEventTimers;  // Save event timers: from TrigSteer and this tool
  bool         m_writeAlways;      // Always write out data for every event
  bool         m_writeConfig;      // Write out configuration data
  bool         m_writeConfigDB;    // Write out configuration data loaded from the DB
  bool         m_onlySaveCostEvent;// Only save events which pass the OPI `prescale'
  bool         m_obeyCostChainPS;  // Only monitor HLT events for prescale sets where the cost chain PS > 0


  unsigned int m_stopAfterNEvent;  // Stop collecting data after nevents
  float        m_execPrescale;     // Prescale for collecting extended data
  float        m_doOperationalInfo;// Value of doOperationalInfo in parent steering alg. Only for reference here

  unsigned int  m_configReductionValue; // Random chance, 1/Value, that this PU gets chosen to write out configs to T0.
  bool          m_doConfigReduction; // Reduce duplicate configs being sent by every PU online. Only send from 1/configReuctionValue PUs

  // Athena tool and service handles
  const HLT::TrigSteer                  *m_parentAlg;
  TrigTimer                             *m_timer;
  ServiceHandle<ITrigTimerSvc>          m_timerSvc; 
  ToolHandle<IBunchGroupTool>           m_toolBunchGroup;
  ToolHandle<HLT::IReusableScaler>      m_scalerTool;
  ToolHandle<HLT::IEventInfoAccessTool> m_eventInfoAccessTool;

  // Tools for operational data collection
  ToolHandle<Trig::ITrigNtTool>      m_toolConf;
  ToolHandle<Trig::ITrigNtTool>      m_toolEBWeight;
  ToolHandleArray<Trig::ITrigNtTool> m_eventTools;
  ToolHandleArray<Trig::ITrigNtTool> m_scaleTools;
  ToolHandleArray<Trig::ITrigNtTool> m_alwaysTools;

  // Ad hoc tool for lb length
  Trig::ReadLumiBlock                m_readLumiBlock;

  // Tool variables:
  std::string                        m_keySteerOPI;   // Key for reading steer OPI from SG
  bool                               m_active;        // Can I do any work?
  unsigned                           m_run;           // Current run number
  unsigned                           m_lumi;          // Current run luminosity block
  unsigned                           m_countEvent;    // Continous count of events
  unsigned                           m_exportedConfig;// Number of configs exported to the HLT result
  unsigned                           m_exportedEvents;// Number of events exported to the HLT result
  float                              m_costChainPS;      // Set to value of the cost chain prescale on load of new config


  TrigMonConfig                      m_config_db;     // Trigger configuration from DB (live config)
  TrigMonConfig                      m_config_sv;     // Trigger configuration from conf. servivce
  std::vector<TrigMonConfig *>       m_bufferConfig;  // Queue for output trigger event
  std::vector<TrigMonEvent *>        m_bufferEvents;  // Queue for output trigger event

  std::map<unsigned, unsigned>       m_lumiCount;     // Count events per lumi block
};

#endif
