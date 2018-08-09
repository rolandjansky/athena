// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterRatesIntersection_H
#define TrigCostRootAnalysis_CounterRatesIntersection_H

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
   * @class CounterRatesIntersection
   * Record rates for the intersection of multiple chains
   * @see CounterBase
   */
  class CounterRatesIntersection: public CounterBaseRates {
  public:
    CounterRatesIntersection(const TrigCostData* costData, const std::string& name, Int_t ID,
                             UInt_t detailLevel = 10, MonitorBase* parent = 0);
    ~CounterRatesIntersection();
    void debug(UInt_t e);

    Float_t runDirect(Bool_t usePrescale = kTRUE);
    Double_t runWeight(Bool_t includeExpress = kFALSE);
  private:
    void removeRedundancies();

    Bool_t m_redundanciesRemoved;
  }; //class CounterRatesIntersection
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterRatesIntersection_H
