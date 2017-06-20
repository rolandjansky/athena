/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTRUN_H
#define TRIGCOSTRUN_H

//
// Algorithm for offline post-processing of TrigMonEvent and TrigMonConfig objects
// which were recorded online or during reprocessing of debug streams. Both L2 and EF
// are processed in a single algorithm since during reprocessing cost data from both levels
// could be saved to a single BS file.
//

// C/C++
#include <set>
#include <utility> //pair

// Framework
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "CoolLumiUtilities/BunchGroupTool.h"

// Trigger
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostMonitor/TrigNtSaveTool.h"
#include "TrigCostMonitor/ReadLumiBlock.h"

class EventInfo;
class StoreGateSvc;

namespace HLT {
  class Navigation;
}

class TrigCostRun : public AthAlgorithm {
public:

   TrigCostRun(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~TrigCostRun();
   
   StatusCode initialize();
   StatusCode execute(); 
   StatusCode finalize();

private:
      
   bool MatchL2andEF(const std::vector<TrigMonEvent> &events_l2,
		     const std::vector<TrigMonEvent> &events_ef);

private:

   //
   // Helper class for reading L2 or EF HLT result
   //
   class ReadHLTResult {
   public:

     ReadHLTResult();
     ~ReadHLTResult() {}

     bool ProcessEvent(ServiceHandle<StoreGateSvc> &storeGate, 
		       ToolHandle<HLT::Navigation> &navigation,
           ToolHandle<Trig::ITrigNtTool> &confTool);
     
     void PrintInit();
     void PrintEvent();
     void PrintSummary();

   private:

     // Ad hoc tool for lb length
     Trig::ReadLumiBlock                        m_readLumiBlock;

     bool ReadResult(ServiceHandle<StoreGateSvc> &storeGate, 
		     ToolHandle<HLT::Navigation> &navigation);

     bool ReadConfig(ServiceHandle<StoreGateSvc> &storeGate);
     bool ReadConfigDB(ServiceHandle<StoreGateSvc> &storeGate, 
                       std::set<const TrigMonEvent*> trigMonEvents,
                       ToolHandle<Trig::ITrigNtTool> &confTool);

     bool ReadEvent (ServiceHandle<StoreGateSvc> &storeGate, 
                     const bool gotConfigSG, 
                     ToolHandle<Trig::ITrigNtTool> &confTool);

     MsgStream& log() const { return *msgStream; }

   public:

     // These variables must be initialized:
     std::string                  keyResult;    // StoreGate key for HLTResult
     std::string                  keyConfig;    // StoreGate key for TrigMonConfigCollection
     std::string                  keyEvent;     // StoreGate key for TrigMonEventCollection
     std::string                  hltLevel;     // HLT trigger level
	   bool                         doLevel;      // If level is active and to be processed

     unsigned                     outputLevel;  // Output level of parent TrigCostRun instance
     MsgStream                   *msgStream;    // MsgStream    of parent TrigCostRun instance
     TrigMonConfig               *globalConfig; // Global config pointer - shared by sub-tools
     TrigTimer                   *timerNavig;   // Timer for Navigation and reading from StoreGate

     // These variables are read from event:
     std::vector<TrigMonEvent>    vecEvent;     // Extracted TrigMonEvent  for current event (HLT result)
     std::vector<TrigMonConfig>   vecConfig;    // Extracted TrigMonConfig for current event (HLT result)

     unsigned                     appId;        // Application id   for current event (HLT result)
     std::string                  appName;      // Application name for current event (HLT result)

     unsigned                     countTrunc;   // Count truncated HLT results
     unsigned                     countInvalid; // Count wrong     HLT results
     unsigned                     countValid;   // Count valid     HLT results 

     unsigned                     countCostEvent;//Count all extracted TrigMonEvent objects where the scale tools were run
     unsigned                     countEvent;   // Count all extracted TrigMonEvent  objects
     unsigned                     countConfig;  // Count all extracted TrigMonConfig 
     unsigned                     resultPrint;  // Count all fully printed results


     std::set< std::pair<int,int> > exportedConfigs; // L1 and HLT keys of configurations which have already been exported to D3PD
   };

private:

   // Properties
   bool                               m_doTiming;         // Collect timing information
   bool                               m_printEvent;       // Print event
   std::string                        m_keyStream;        // Key for trigger stream name (optional)

   // Tools and services
   TrigTimer                          *m_timerExec;
   TrigTimer                          *m_timerNavig;
   TrigTimer                          *m_timerTotal;

   ServiceHandle<ITrigTimerSvc>        m_timerSvc;

   ToolHandle<HLT::Navigation>         m_navigation;

   ToolHandleArray<Trig::ITrigNtTool>  m_tools;
   ToolHandleArray<Trig::ITrigNtTool>  m_toolsSave;

   ToolHandle<Trig::ITrigNtTool>       m_toolConf; //!< To read in TrigConf from the DB at T0

   // Variables   
   ReadHLTResult                       m_readL2;          // Helper class for reading L2 result
   ReadHLTResult                       m_readEF;          // Helper class for reading EF result
   ReadHLTResult                       m_readHLT;         // Helper class for reading HLT result
   
   TrigMonConfig                       m_config;          // Global config - shared by sub-tools
};

#endif
