/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTALG_H
#define TRIGCOSTALG_H

//
// Algorithm for processing TrigMonEvent and TrigMonConfig:
//
// a) Run tools and fill TrigMonEvent and TrigMonConfig
// b) Read TrigMonEvent and TrigMonConfig from StoreGate
//    and merge with other objects or save them to file
//

// C/C++
#include <set>

// Framework
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Trigger
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostMonitor/ITrigNtTool.h"

class StoreGateSvc;

class TrigCostAlg : public AthAlgorithm
{
public:

   TrigCostAlg(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~TrigCostAlg();
   
   StatusCode initialize();
   StatusCode execute(); 
   StatusCode finalize();
   
private:
   
   void ExtractConfig(const std::string &key);
   void ExtractEvent (const std::string &key, const int level);

private:

   // Properties
   bool          m_doTiming;         // Collect timing information
   bool          m_writeConfig;      // Write out configuration
   bool          m_writeEvent;       // Write out event
   bool          m_mergeEvent;       // Merge events
   bool          m_printEvent;       // Print event

   std::string   m_keyConfigL2;      // Key for L2 TrigMonConfigCollection
   std::string   m_keyConfigEF;      // Key for EF TrigMonConfigCollection
   std::string   m_keyConfigHLT;     // Key for HLT TrigMonConfigCollection
   std::string   m_keyEventL2;       // Key for L2 TrigMonEventCollection
   std::string   m_keyEventEF;       // Key for EF TrigMonEventCollection
   std::string   m_keyEventHLT;      // Key for HLT TrigMonEventCollection

   // Tools and services
   TrigTimer                          *m_timerTotal;

   ServiceHandle<ITrigTimerSvc>        m_timerSvc;
   ToolHandleArray<Trig::ITrigNtTool>  m_tools;

   // Variables
   TrigMonConfig                       m_config;   // Common config for L2 and EF
   TrigMonEvent                        m_event;    // Common event  for L2 and EF   
};

#endif
