// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_RatesCPSGroup_H
#define TrigCostRootAnalysis_RatesCPSGroup_H

// STL include(s):
#include <set>
#include <string>

// Local include(s):
#include "Utility.h"

namespace TrigCostRootAnalysis {
  class RatesChainItem;

  /**
   * @class RatesCPSSet
   * Holds all RatesChainItems in a CPS set
   */
  class RatesCPSGroup {
  public:
    RatesCPSGroup(std::string name);
    const std::string& getName();


    void add(RatesChainItem* item);
    void calculateCPSFactor();

    RatesChainItem* getL1();
    ChainItemSetIt_t getChainStart();
    ChainItemSetIt_t getChainEnd();
    Double_t getCommonWeight();
  private:
    std::string m_name; //!< This chain item's name, more for debug

    Double_t m_commonPS; //!< The common CPS factor to factor out
    Double_t m_commonPSWeight;

    ChainItemSet_t m_items; //!< Set of (HLT) chain items which are to be treated coherently

    RatesChainItem* m_l1; //!< As we know we're only going to have 1 L1 item, we can keep note of it.
  }; // Class RatesCPSGroup
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_RatesCPSGroup_H
