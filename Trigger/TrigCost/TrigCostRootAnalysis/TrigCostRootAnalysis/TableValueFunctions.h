// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_TableValueFunctions_H
#define TrigCostRootAnalysis_TableValueFunctions_H

// ROOT include(s):
#include <Rtypes.h>

//Local include(s):
#include "Utility.h" //Typedefs

namespace TrigCostRootAnalysis {

  //Forward declaration
  class CounterBase;

  Float_t tableFnChainGetTotalFracTime(CounterMap_t* _map, CounterBase* _TCCB);

  Float_t tableFnGlobalGetSteeringFarmUse(CounterMap_t* _map, CounterBase* _TCCB);
  Float_t tableFnGlobalGetHLTNodePrediction(CounterMap_t* _map, CounterBase* _TCCB);
  Float_t tableFnGlobalGetHLTNodePredictionErr(CounterMap_t* _map, CounterBase* _TCCB);
  Float_t tableFnGlobalGetHLTNodePredictionSteering(CounterMap_t* _map, CounterBase* _TCCB);
  Float_t tableFnGlobalGetHLTNodePredictionErrSteering(CounterMap_t* _map, CounterBase* _TCCB);

  Float_t tableFnRateGetWeightedRateErr(CounterMap_t* _map, CounterBase* _TCCB);
  Float_t tableFnRateGetDirectRateErr(CounterMap_t* _map, CounterBase* _TCCB);

}

#endif //TrigCostRootAnalysis_TableValueFunctions_H
