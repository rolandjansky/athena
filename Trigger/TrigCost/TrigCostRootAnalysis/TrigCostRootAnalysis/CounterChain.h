// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterChain_H
#define TrigCostRootAnalysis_CounterChain_H

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

  /**
   * @class CounterChain
   * One counter chain is created per monitored chain in the run. It is responsible for
   * recording the monitoring information for that chain in every event in which it
   * is run.
   * @see CounterBase
   */
  class CounterChain: public CounterBase {
  public:
    CounterChain(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel = 10,
                 MonitorBase* parent = 0);
    ~CounterChain();
    void startEvent();
    void processEventCounter(UInt_t e, UInt_t f, Float_t weight = 1.);
    void endEvent(Float_t weight = 1.);
    void debug(UInt_t e);
  private:
    Double_t getPrescaleFactor(UInt_t e = INT_MAX);

    static Float_t s_eventTimeExecute; //<! Global tabulator of total chain time for all chains in an event
    Float_t m_prescaleWeight; //<! Chain's effective HLT*L1 prescale, cached
  }; //class CounterChain
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterChain_H
