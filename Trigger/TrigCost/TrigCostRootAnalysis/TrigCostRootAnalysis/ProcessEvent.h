// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_ProcessEvent_H
#define TrigCostRootAnalysis_ProcessEvent_H

// STL include(s):
#include <map>
#include <string>
#include <vector>

// Local include(s):
#include "TrigCostData.h"
#include "Utility.h"
#include "Timer.h"

// ROOT include(s):
#include <TNamed.h>

namespace TrigCostRootAnalysis {

  //Forward declaration
  class MonitorBase;
  
  /**
   * @class ProcessEvent
   * Calculate monitoring properties on supplied event.
   * This class keeps track of which monitoring processes are active
   * and passes down the event data.
   */
  class ProcessEvent {
  
   public:
    
    ProcessEvent(const TrigCostData* _costData, UInt_t _level, const std::string& _name);
    ~ProcessEvent();
    void setMonitoringMode(ConfKey_t _type, Bool_t _isActive);
    Bool_t newEvent(Float_t _weight = 1.);
    const monitorMap_t& getMonitors();
    MonitorBase* getMonitor(ConfKey_t _type);
    void saveOutput();
    void setLevel(UInt_t _level);
    void setPass(UInt_t _pass);
    UInt_t getLevel();
    UInt_t getPass();
    const std::string& getLevelStr();
    const std::string& getName() { return m_name; }
    
   private:

    static void newEventThreaded(MonitorBase* _monitor, Float_t _weight);

    void (*m_threadFnPtr)(MonitorBase*,Float_t); //<! Pointer to my static run function - used to spawn threads
    const TrigCostData* m_costData; //!< Provider of my data
    monitorMap_t m_monitorCollections; //!< Map monitoring modes to associated collections
    UInt_t m_level; //!< Trigger level (to be deprecated)
    const std::string m_name; //!< My name
    Bool_t m_ratesOnly; //<! Don't do buffering if this is true
    UInt_t m_nThread; //!< Number of concurrent threads to spawn in multi thread mode
    Bool_t m_isCPUPrediction; //!< If we're predicting CPU useage then we want to skip events which _only_ pass the cost chain (after bookkeeping)
    Bool_t m_doNotLumiWeightUnbiased; //!< Gives better total unprescales, but underpredicts low thresholds
    UInt_t m_pass; //!< Holds which pass through the input files
    Timer m_threadTimer; //!< Record how long it takes to spawn the event processing threads
    Timer m_takeEventTimer; //!< Record how long it takes to classify if the event is to be processed
    Timer m_cacheAlgTimer; //!< Record how long it take to run static functions to prepare input data to be executed by the monitors
    Timer m_cacheROSTimer; //!< Record how long it take to run static functions to prepare input data to be executed by the monitors
    Timer m_dataPrepTimer; //!< Record how long it takes to link together all the flat info in the D3PD
    Timer m_processTime; //!< Record how long it takes to execute all the event processors

  }; //class ProcessEvent
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_ProcessEvent_H
