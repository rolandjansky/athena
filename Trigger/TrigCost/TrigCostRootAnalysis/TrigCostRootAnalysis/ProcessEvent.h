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
    void newEvent(Float_t _weight = 1.);
    const monitorMap_t& getMonitors();
    MonitorBase* getMonitor(ConfKey_t _type);
    void saveOutput();
    void setLevel(UInt_t _level);
    UInt_t getLevel();
    const std::string& getLevelStr();
    const std::string& getName() { return m_name; }
    
   private:
   
    const TrigCostData* m_costData; //!< Provider of my data
    monitorMap_t m_monitorCollections; //!< Map monitoring modes to associated collections
    UInt_t m_level; //!< Trigger level (to be deprecated)
    const std::string m_name; //!< My name

  }; //class ProcessEvent
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_ProcessEvent_H
