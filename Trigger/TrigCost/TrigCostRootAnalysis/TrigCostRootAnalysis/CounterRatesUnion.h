// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterRatesUnion_H
#define TrigCostRootAnalysis_CounterRatesUnion_H

// STL include(s):
#include <map>
#include <string>

// Local include(s):
#include "CounterBaseRates.h"

// ROOT include(s):
#include <Rtypes.h>

namespace TrigCostRootAnalysis {

  //Forward declaration
  class TrigCostData;

  /**
   * @enum CombinationClassification Internal classification of the topology of the supplied set of chains. In increasing order of complexity.
   */
  enum CombinationClassification {
    kOnlyL1, //!< Only L1 chain items - therefore straightforward OR of all of them.
    kAllToAll, //!< Every L2 chain is seeded by every L1 chains
    kAllOneToOne, //!< Parallel chains where each L2 chain has a different, single, L1 chain
    kAllOneToMany, //!< Common case, set of L1s with potentially multiple seeded triggers but all L2s have exactly one seed
    kManyToMany, //!< Any arbitrary set of connections, inc. L1's which connect to many L2s and L2's which are seeded by many L1s
    kUnset //!< Topology has not yet been classified
  };
  
  /**
   * @class CounterRatesUnion
   * Calculate and record rates for the union of multiple chains
   * @see CounterBase
   */
  class CounterRatesUnion : public CounterBaseRates {
  
   public:
   
    CounterRatesUnion( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel = 10 );
    ~CounterRatesUnion();
    void debug(UInt_t _e);

    void    finalise();
    Float_t runDirect(Bool_t _usePrescale = kTRUE); 
    Float_t runWeight();

    void setGlobalRateCounter(CounterBaseRates* _c) { m_globalRates = _c; }

   protected:

    void classify();
    Float_t runWeight_OnlyL1();
    Float_t runWeight_AllToAll();
    Float_t runWeight_AllOneToOne();
    Float_t runWeight_AllOneToMany();
    Float_t runWeight_ManyToMany();

    CounterBaseRates* m_globalRates;    //!< Pointer to the global rates counter. Used by Unique CounterRatesUnion derived counters. 
    CombinationClassification m_combinationClassification; //!< Hold the classified topology of the set of chains in this Union.
       
  }; //class CounterRatesUnion
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterRatesUnion_H
