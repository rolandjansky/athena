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
  class CounterBaseRates: public CounterBase {
  public:
    CounterBaseRates(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel = 10,
                     MonitorBase* parent = 0);
    ~CounterBaseRates();

    void startEvent();
    void processEventCounter(UInt_t e, UInt_t f, Float_t weight = 0.);
    void endEvent(Float_t weight = 1);

    void addL1Item(RatesChainItem* toAdd);
    void addL2Item(RatesChainItem* toAdd);
    void addL2Items(ChainItemSet_t toAdd);
    void addL3Item(RatesChainItem* toAdd);
    void addL3Items(ChainItemSet_t toAdd);
    void addCPSItem(RatesCPSGroup* toAdd, std::string name);
    void addOverlap(CounterBase* overlap);
    void setMyUniqueCounter(CounterBaseRates* c) {m_myUniqueCounter = c;}
    void setGlobalRateCounter(CounterBaseRates* c) {m_globalRates = c;}
    void setLowerRateCounter(CounterBaseRates* c) {m_lowerRates = c;}

    Bool_t getInEvent();

    CPSGroupSet_t& getCPSGroupSet();
    ChainItemSet_t& getL3ItemSet();
    ChainItemSet_t& getL2ItemSet();
    ChainItemSet_t& getL1ItemSet();
    UInt_t getBasicPrescale();
    CounterBaseRates* getMyUniqueCounter() {return m_myUniqueCounter;}
    CounterBaseRates* getGlobalRateCounter() {return m_globalRates;}
    Double_t getLastWeight();

    virtual void finalise();
    virtual Float_t runDirect(Bool_t usePrescale = kTRUE) = 0; // Pure virtual calls! Please override-me with
                                                                // appropriate trigger logic
    virtual Double_t runWeight(Bool_t includeExpress = kFALSE) = 0;
  protected:
    void startRun();
    Double_t getPrescaleFactor(UInt_t e = INT_MAX); // Unused here
    Bool_t checkMultiSeed(RatesChainItem* toAdd);

    CounterSet_t m_ovelapCounters; //!< List of all counters to be queried at end of run to get my overlap with them
    CPSGroupSet_t m_cpsGroups;      //!< List of groups of L2 chains in this combination which are to be treated
                                    // coherently
    ChainItemSet_t m_L3s;            //!< List of all L3 chains in this combination
    ChainItemSet_t m_L2s;            //!< List of all L2 chains in this combination
    ChainItemSet_t m_L1s;            //!< List of all L1 items which seed L2 chains in this combination (not always
                                     // used)
    Bool_t m_cannotCompute;  //!< Chain rate cannot be computed for whatever reason. Will always return weight 0;
    CounterBaseRates* m_myUniqueCounter;//!< For L1 and HLT chains, this pointer is set to the counter responsible for
                                        // getting the unique rate for the chain.
    CounterBaseRates* m_globalRates;    //!< Pointer to the global rates counter. Used currently by Unique
                                        // CounterRatesUnion derived counters.
    CounterBaseRates* m_lowerRates;     //!< Pointer to the rates counter at lower level, for getting "Input Rate"
    Bool_t m_doScaleByPS;    //!< If we are configured to scale all rates by their PS
    Bool_t m_doDirectPS;     //!< If we are applying prescales directly (not with weights)
    Double_t m_cachedWeight;   //!< Holds the most recently calculated weight. Used so other chains can get the global
                               // monitor's weight for the current event.
    Bool_t m_alwaysDoExpressPS; //!< Used only by the RATE_EPRESS monitor to always use the chain's EXPRESS prescale
                                // when calculating the rates for the monitor
    StringSet_t m_myCPSChains;    //!< HLT chains in my group (if I'm a group) Used by the coherent prescale logic.
    Double_t m_eventLumiExtrapolation; //!< Filled during runWeight(), this is the weighted average L extrapolation
                                       // factor to use for this counter.
    Bool_t m_disableEventLumiExtrapolation; //!< Do not apply the RateChainItem's averaged L extrapolation factor. Used
                                            // in UpgradeRates mode
  }; //class CounterBaseRates
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterBaseRates_H
