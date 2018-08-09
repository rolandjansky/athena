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
   * @enum CombinationClassification Internal classification of the topology of the supplied set of chains. In
   *increasing order of complexity.
   */
  enum CombinationClassification {
    kOnlyL1, //!< Only L1 chain items - therefore straightforward OR of all of them.
    kAllToAll, //!< Every L2 chain is seeded by every L1 chains
    kAllOneToOne, //!< Parallel chains where each L2 chain has a different, single, L1 chain
    kAllOneToMany, //!< Common case, set of L1s with potentially multiple seeded triggers but all L2s have exactly one
                   // seed
    kManyToMany, //!< Any arbitrary set of connections, inc. L1's which connect to many L2s and L2's which are seeded by
                 // many L1s
    kUnset //!< Topology has not yet been classified
  };

  /**
   * @class CounterRatesUnion
   * Calculate and record rates for the union of multiple chains
   * @see CounterBase
   */
  class CounterRatesUnion: public CounterBaseRates {
  public:
    CounterRatesUnion(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel = 10,
                      MonitorBase* parent = 0);
    ~CounterRatesUnion();
    void debug(UInt_t e);

    void finalise();
    Float_t runDirect(Bool_t usePrescale = kTRUE);
    Double_t runWeight(Bool_t includeExpress = kFALSE);
  protected:
    void classify();
    Double_t runWeight_OnlyL1();
    Double_t runWeight_AllToAll(Bool_t includeExpress = kFALSE);
    Double_t runWeight_AllOneToOne(Bool_t includeExpress = kFALSE);
    Double_t runWeight_AllOneToMany(Bool_t includeExpress = kFALSE);
    Double_t runWeight_ManyToMany(Bool_t includeExpress = kFALSE);

    CombinationClassification m_combinationClassification; //!< Hold the classified topology of the set of chains in
                                                           // this Union.

    ChainItemSet_t m_l1ToProcess; //!< Holds combination of regular chain L1 seeds and CPS L1 seeds
  }; //class CounterRatesUnion
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterRatesUnion_H
