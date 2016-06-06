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
  class MonitorRates : public MonitorBase {
  
   public:
   
    MonitorRates(const TrigCostData* _costData);
    ~MonitorRates();
    void newEvent(Float_t _weight = 1.);
    CounterBase* newCounter( const std::string &_name, Int_t _ID );
    Bool_t getIfActive(ConfKey_t _mode);
    void saveOutput();

   private:

    void populateCounterMap(CounterMap_t* _counterMap);
    void populateChainItemMaps();
    void saveRateGraphs();


    void createGlobalCounters(CounterMap_t* _counterMap);
    void createL1Counters(CounterMap_t* _counterMap);
    void createHLTCounters(CounterMap_t* _counterMap);
    void createOverlapCounters(CounterMap_t* _counterMap);
    void createCPSGroupCounters(CounterMap_t* _counterMap);
    void createGroupCounters(CounterMap_t* _counterMap);

    CounterRatesUnion* m_globalRateHLTCounter;
    CounterRatesUnion* m_globalRateL1Counter;
    CounterRatesUnion* m_globalRatePhysicsMainCounter;

    ChainItemMap_t m_chainItemsL1;
    ChainItemMap_t m_chainItemsHLT;
    CPSGroupMap_t  m_cpsGroups;

    Bool_t m_doingOverlaps; //!< Flag for if one or more overlaps is being calculated
    Bool_t m_doCPS; //!< Flag to switch on special handling for chains which are in CPS groups

    
  }; //class MonitorRates
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorRates_H
