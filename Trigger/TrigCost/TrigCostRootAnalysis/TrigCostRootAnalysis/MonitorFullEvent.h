// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorFullEvent_H
#define TrigCostRootAnalysis_MonitorFullEvent_H

// STL include(s):
#include <map>
#include <string>
#include <vector>

// Local include(s):
#include "MonitorBase.h"

// ROOT includes
#include <TRandom3.h>

namespace TrigCostRootAnalysis {
  //Forward declaration
  class Data;

  /**
   * @class MonitorFullEvent
   * Per event mapping of the trigger element hieararchy.
   */
  class MonitorFullEvent: public MonitorBase {
  public:
    MonitorFullEvent(const TrigCostData* costData);
    void newEvent(Float_t weight = 1.);
    CounterBase* newCounter(const std::string& name, Int_t ID);
    Bool_t getIfActive(ConfKey_t mode);
    void saveOutput();
    Int_t getNCollectionsToProcess();
    Bool_t isThreadable();
  private:
    Float_t m_probability; //!< Probability that an event will be saved.
    Bool_t m_usePickList; //!< Use user supplied list of events
    TRandom3 m_rando; //!< Random number generator used to decide if the event is to be saved.
    const std::string m_randomString; //!< uses as decorator
    const std::string m_slowString; //<! used as decorator
    Bool_t m_doPick; //<! Holds if the event was picked for export from a file list
    Bool_t m_doRND; //<! Holds if the event was picked for export at random
  }; //class MonitorFullEvent
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorFullEvent_H
