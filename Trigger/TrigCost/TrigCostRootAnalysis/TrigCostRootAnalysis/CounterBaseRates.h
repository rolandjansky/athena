// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterBaseRates_H
#define TrigCostRootAnalysis_CounterBaseRates_H

// STL include(s):
#include <map>
#include <string>
#include <assert.h>

// Local include(s):
#include "CounterBase.h"

// ROOT include(s):
#include <Rtypes.h>

namespace TrigCostRootAnalysis {

  //Forward declaration
  class TrigCostData;
  class ChainItem;
  
  /**
   * @class CounterBaseRates
   * Base class for all Rates counters. Adds some more rates-specific calls over the basic counter
   * @see CounterBase
   */
  class CounterBaseRates : public CounterBase {
  
   public:
   
    CounterBaseRates( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel = 10 );
    ~CounterBaseRates();

    void startEvent();
    void processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight = 1.);
    void endEvent();

    void   addL1Item  ( RatesChainItem* _toAdd );
    void   addHLTItem ( RatesChainItem* _toAdd );
    void   addHLTItems( ChainItemSet_t _toAdd );
    void   addOverlap ( CounterBase* _overlap );
    Bool_t getInEvent();

    ChainItemSet_t& getHLTItemSet();
    UInt_t getBasicPrescale();

    virtual void    finalise();
    virtual Float_t runDirect(Bool_t _usePrescale = kTRUE) = 0; // Pure virtual calls! Please override-me with appropriate trigger logic
    virtual Float_t runWeight() = 0;

  protected:

    CounterSet_t      m_ovelapCounters; //!< List of all counters to be queried at end of run to get my overlap with them 
    ChainItemSet_t    m_L2s;            //!< List of all L2 chains in this combination
    ChainItemSet_t    m_L1s;            //!< List of all L1 items which seed L2 chains in this combination (not always used)
   
  }; //class CounterBaseRates
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterBaseRates_H
