// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterAlgorithm_H
#define TrigCostRootAnalysis_CounterAlgorithm_H

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
   * @class CounterAlgorithm
   * A Counter object, responsible for all the low-level monitoring performed on individual HLT algorithms.
   * Used by MonitorAlgorithm and by MonitorFullEvent, where one instance of CounterAlgorithm is used for each alg 
   * in each monitored full event.
   * @see CounterBase
   */
  class CounterAlgorithm : public CounterBase {
  
   public:
   
    CounterAlgorithm( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel = 10 );
    ~CounterAlgorithm();
    void startEvent();
    void processEventCounter(UInt_t _e, UInt_t _f,  Float_t _weight = 1.);
    void endEvent();
    void debug(UInt_t _e, UInt_t _a);

    // // Comparator - sort based on time
    // bool operator<(CounterAlgorithm& _other) const {
    //   return m_lastCallStartTime > _other.m_lastCallStartTime;
    // }
   
   private:
   
    void fullExecutionInformation(UInt_t _e, UInt_t _f, Float_t _weight);

    static Float_t s_eventTimeExecute; //!< Static tabulation of execute time for all algorithms in an event.
    Float_t m_firstAlgStartTime; //!< Used to keep track of the earliest (first) execution of this counter in any one event
    Float_t m_firstAlgTime; //<! Used to buffer the execution time for the first call in an event
    Float_t m_firstAlgTimeWeight; //<! Used to buffer the weight of m_firstAlgTime
    
  }; //class CounterAlgorithm
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterAlgorithm_H
