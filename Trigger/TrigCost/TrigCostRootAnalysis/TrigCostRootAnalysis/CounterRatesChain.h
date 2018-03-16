// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_CounterRatesChain_H
#define TrigCostRootAnalysis_CounterRatesChain_H

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
   * @class CounterRatesChain
   * Record rates for a single chain
   * @see CounterBase
   */
  class CounterRatesChain: public CounterBaseRates {
  public:
    CounterRatesChain(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel = 10,
                      MonitorBase* parent = 0);
    ~CounterRatesChain();
    void debug(UInt_t e);

    Float_t runDirect(Bool_t usePrescale = kTRUE);
    Double_t runWeight(Bool_t includeExpress = kFALSE);
  private:
  }; //class CounterRatesChain
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterRatesChain_H
