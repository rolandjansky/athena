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
  class CounterRatesIntersection : public CounterBaseRates {

   public:

    CounterRatesIntersection( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel = 10, MonitorBase* _parent = 0 );
    ~CounterRatesIntersection();
    void debug(UInt_t _e);

    Float_t runDirect(Bool_t _usePrescale = kTRUE);
    Double_t runWeight();

   private:

    void removeRedundancies();

    Bool_t m_redundanciesRemoved;

  }; //class CounterRatesIntersection

} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_CounterRatesIntersection_H
