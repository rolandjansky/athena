// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterSequence_H
#define TrigCostRootAnalysis_CounterSequence_H

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
   * @class CounterSequence
   * One counter is created for each sequence in the run, it records all monitoring information for each instance
   * of this sequence in the run.
   * @see CounterBase
   */
  class CounterSequence: public CounterBase {
  public:
    CounterSequence(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel = 10,
                    MonitorBase* parent = 0);
    ~CounterSequence();
    void startEvent();
    void processEventCounter(UInt_t e, UInt_t f, Float_t weight = 1.);
    void endEvent(Float_t weight = 1.);
    void debug(UInt_t e);
  private:
    Double_t getPrescaleFactor(UInt_t e = INT_MAX);

    static Float_t s_eventTimeExecute; //!< Global static tabulator of the total time for all sequences in an event.

    Float_t m_eventWeight; //<! What weight to use for the active events variable
    std::set< std::string > m_chainsSeen; //<! List of chains which have used this in a given event
  }; //class CounterSequence
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterSequence_H
