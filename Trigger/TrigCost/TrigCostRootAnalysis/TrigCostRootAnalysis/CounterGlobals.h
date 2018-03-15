// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterGlobals_H
#define TrigCostRootAnalysis_CounterGlobals_H

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
   * @class CounterGlobals
   * Counter to monitor overall statistics for a whole lumi block
   * @see CounterBase
   */
  class CounterGlobals: public CounterBase {
  public:
    CounterGlobals(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel = 10,
                   MonitorBase* parent = 0);
    ~CounterGlobals();
    void startEvent();
    void processEventCounter(UInt_t e, UInt_t f, Float_t weight = 1.);
    void endEvent(Float_t weight = 1.);
    void debug(UInt_t e);
  private:
    Double_t getPrescaleFactor(UInt_t e = INT_MAX);

    Float_t m_earliestTimestamp; //!< Time of start first alg call in event
    Float_t m_latestTimestamp; //!< Time of end of last alg call in event
    Float_t m_steeringTime; //!< Time taken in the steering
    Float_t m_algTime; //!< Time of all algs in event - only used for debug output
    Float_t m_rosTime; //!< Time of all ROS reqs (by algs) in event - only used for debug output
    Float_t m_cpuTime; //!< CPU time of all algs in event - only used for debug output
    Int_t m_rosCalls; //!< ROS calls of all algs in event - only used for debug output
    Int_t m_algCalls; //!< N algorithm calls - only used for debug output
    Bool_t m_CPUBreakDown; //!< Do CPU breakdown per CPU type online

    std::map<UInt_t, UInt_t> m_processingUnits; //!< Keep track of processing units seen in this LB and how many events
                                                // each processes
  }; //class CounterGlobals
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterGlobals_H
