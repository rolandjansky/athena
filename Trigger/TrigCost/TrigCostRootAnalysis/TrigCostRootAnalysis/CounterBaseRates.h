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

    CounterBaseRates( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel = 10, MonitorBase* _parent = 0 );
    ~CounterBaseRates();

    void startEvent();
    void processEventCounter(UInt_t _e, UInt_t _f, Float_t _weight = 0.);
    void endEvent(Float_t _weight = 1);

    void   addL1Item  ( RatesChainItem* _toAdd );
    void   addL2Item ( RatesChainItem* _toAdd );
    void   addL2Items( ChainItemSet_t _toAdd );
    void   addL3Item ( RatesChainItem* _toAdd );
    void   addL3Items( ChainItemSet_t _toAdd );
    void   addCPSItem( RatesCPSGroup* _toAdd, std::string _name);
    void   addOverlap ( CounterBase* _overlap );
    void   setMyUniqueCounter( CounterBaseRates* _c ) { m_myUniqueCounter = _c; }
    void   setGlobalRateCounter(CounterBaseRates* _c) { m_globalRates = _c; }
    void   setLowerGlobalRateCounter(CounterBaseRates* _c) { m_lowerGlobalRates = _c; }

    Bool_t getInEvent();

    CPSGroupSet_t&  getCPSGroupSet();
    ChainItemSet_t& getL3ItemSet();
    ChainItemSet_t& getL2ItemSet();
    ChainItemSet_t& getL1ItemSet();
    UInt_t getBasicPrescale();
    CounterBaseRates* getMyUniqueCounter() { return m_myUniqueCounter; }
    CounterBaseRates* getGlobalRateCounter() { return m_globalRates; }
    Double_t getLastWeight();
    Double_t getLumiExtrapolationFactor();

    virtual void    finalise();
    virtual Float_t runDirect(Bool_t _usePrescale = kTRUE) = 0; // Pure virtual calls! Please override-me with appropriate trigger logic
    virtual Double_t runWeight(Bool_t _includeExpress = kFALSE) = 0;

  protected:

    void startRun();
    Double_t getPrescaleFactor(UInt_t _e = INT_MAX); // Unused here

    CounterSet_t       m_ovelapCounters; //!< List of all counters to be queried at end of run to get my overlap with them
    CPSGroupSet_t      m_cpsGroups;      //!< List of groups of L2 chains in this combination which are to be treated coherently
    ChainItemSet_t     m_L3s;            //!< List of all L3 chains in this combination
    ChainItemSet_t     m_L2s;            //!< List of all L2 chains in this combination
    ChainItemSet_t     m_L1s;            //!< List of all L1 items which seed L2 chains in this combination (not always used)
    Bool_t             m_cannotCompute;  //!< Chain rate cannot be computed for whatever reason. Will always return weight 0;
    CounterBaseRates*  m_myUniqueCounter;//!< For L1 and HLT chains, this pointer is set to the counter responsible for getting the unique rate for the chain.
    CounterBaseRates*  m_globalRates;    //!< Pointer to the global rates counter. Used currently by Unique CounterRatesUnion derived counters.
    CounterBaseRates*  m_lowerGlobalRates;//!< Pointer to the global rates counter at lower level.
    Bool_t             m_doSacleByPS;    //!< If we are configured to scale all rates by their PS
    Bool_t             m_doDirectPS;     //!< If we are applying prescales directly (not with weights)
    Double_t           m_cachedWeight;   //!< Holds the most recently calculated weight. Used so other chains can get the global monitor's weight for the current event.
    Bool_t             m_alwaysDoExpressPS; //!< Used only by the RATE_EPRESS monitor to always use the chain's EXPRESS prescale when calculating the rates for the monitor
    StringSet_t        m_myCPSChains;    //!< HLT chains in my group (if I'm a group) Used by the coherent prescale logic.
    Double_t           m_eventLumiExtrapolation; //!< Filled during runWeight(), this is the weighted average L extrapolation factor to use for this counter.


  }; //class CounterBaseRates

} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterBaseRates_H
