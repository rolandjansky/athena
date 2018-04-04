// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterFullEvent_H
#define TrigCostRootAnalysis_CounterFullEvent_H

// STL include(s):
#include <map>
#include <string>

// Local include(s):
#include "CounterBase.h"

// ROOT include(s):
#include <Rtypes.h>

namespace TrigCostRootAnalysis {
  //Forward declaration
  class TrigCostData;
  class CounterAlgorithm;

  /**
   * @class CounterFullEvent
   * Depricated
   * @see CounterBase
   */
  class CounterFullEvent: public CounterBase {
  public:
    CounterFullEvent(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel = 10,
                     MonitorBase* parent = 0);
    ~CounterFullEvent();
    void startEvent();
    void processEventCounter(UInt_t e, UInt_t f, Float_t weight = 1.);
    void endEvent(Float_t weight = 1.);
    void debug(UInt_t e);
    const std::vector< CounterAlgorithm* >& getEventCounterAlgorithms() {return m_algCounters;}
  private:
    Double_t getPrescaleFactor(UInt_t e = INT_MAX);

    Bool_t m_isRun; //!< Each of these counters is only designed to be run on a single event.

    std::vector< CounterAlgorithm* > m_algCounters;
  }; //class CounterFullEvent
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterFullEvent_H
