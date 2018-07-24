// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorRates_H
#define TrigCostRootAnalysis_MonitorRates_H

// STL include(s):
#include <map>
#include <string>
#include <vector>
#include <iostream>

// Local include(s):
#include "MonitorBase.h"
#include "WriteXML_JSON.h"

// ROOT include(s):
#include <TH1.h>
#include <TCanvas.h>

namespace TrigCostRootAnalysis {
  class CounterBaseRates;
  class CounterRatesUnion;

  /**
   * @class MonitorRates
   */
  class MonitorRates: public MonitorBase {
  public:
    MonitorRates(const TrigCostData* costData);
    ~MonitorRates();
    void newEvent(Float_t weight = 1.);
    CounterBase* newCounter(const std::string& name, Int_t ID);
    Bool_t getIfActive(ConfKey_t mode);
    void saveOutput();
  private:
    void populateCounterMap(CounterMap_t* counterMap);
    void populateChainItemMaps();
    void saveRateGraphs();


    void createGlobalCounters(CounterMap_t* counterMap);
    void createL1Counters(CounterMap_t* counterMap);
    void createHLTCounters(CounterMap_t* counterMap);
    void createOverlapCounters(CounterMap_t* counterMap);
    void createCPSGroupCounters(CounterMap_t* counterMap);
    void createGroupCounters(CounterMap_t* counterMap);

    CounterRatesUnion* m_globalRateHLTCounter;
    CounterRatesUnion* m_globalRateL1Counter;
    CounterRatesUnion* m_globalRatePhysicsMainCounter;

    ChainItemMap_t m_chainItemsL1;
    ChainItemMap_t m_chainItemsHLT;
    CPSGroupMap_t m_cpsGroups;

    Bool_t m_doingOverlaps; //!< Flag for if one or more overlaps is being calculated
    Bool_t m_doCPS; //!< Flag to switch on special handling for chains which are in CPS groups
  }; //class MonitorRates
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorRates_H
