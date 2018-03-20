// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <map>
#include <iostream>
#include <sstream>
#include <utility> //std::pair
#include <iomanip> // std::setprecision

// ROOT include(s):
#include <TMath.h>

//Local include(s):
#include "../TrigCostRootAnalysis/TableValueFunctions.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/CounterAlgorithm.h"

/**
 * @file TableFunctions.cxx
 *
 * Functions are collated here, each of which calculates a value to be exported in the tables of results.
 * These are passed around as function pointers.
 *
 */

namespace TrigCostRootAnalysis {
  /////////////////////////////////////
  /// BEGIN CHAIN MONITOR FUNCTIONS ///
  /////////////////////////////////////

  /**
   * Global "table function" to caclulate a given value in a table column.
   * Buffer the total chain execution time for all chains in a counterMap.
   * Return a chains fractional time w.r.t. this total.
   * @param map Map of CounterBase pointers.
   * @param TCCB Pointer to the counter being assessed for this row.
   * @returns The table column value.
   */
  Float_t tableFnChainGetTotalFracTime(CounterMap_t* map, CounterBase* TCCB) {
    static Float_t bufferedTotalTime = 0.;
    static CounterMap_t* bufferedMap = 0;

    // Get this chain's fraction of the total time.
    // We first need to get what the total time of all chains is for a given counterMap.
    if (bufferedMap != map) {
      bufferedMap = map;
      bufferedTotalTime = 0.;
      CounterMapIt_t counterMapIt = map->begin();
      for (; counterMapIt != map->end(); ++counterMapIt) {
        CounterBase* counter = counterMapIt->second;
        bufferedTotalTime += counter->getValue(kVarTime, kSavePerEvent);
      }
    }

    // Now we can return a fractional value (check for /0)
    if (isZero(bufferedTotalTime)) return 0.;

    Float_t thisChainTime = TCCB->getValue(kVarTime, kSavePerEvent);

    return (thisChainTime / bufferedTotalTime) * 100; // In %
  }

  /**
   * Returns the fractional statistical error of the kVarTime variable (kSavePerEvent)
   * @param map Unused
   * @param TCCB Pointer to the counter being assessed.
   * @returns The % error.
   */
  Float_t tableFnChainGetTotalTimeErr(CounterMap_t* /*map*/, CounterBase* TCCB) {
    const Float_t val = TCCB->getValue(kVarTime, kSavePerEvent); 
    const Float_t err = TCCB->getValueError(kVarTime, kSavePerEvent); 
    return 100. * (err / val);
  }

  ///////////////////////////////////
  /// BEGIN ROS MONITOR FUNCTIONS ///
  ///////////////////////////////////

  ///////////////////////////////////
  /// BEGIN ALG MONITOR FUNCTIONS ///
  ///////////////////////////////////

  //////////////////////////////////////
  /// BEGIN GLOBAL MONITOR FUNCTIONS ///
  //////////////////////////////////////

  Float_t tableFnGlobalGetSteeringFarmUse(CounterMap_t* /*map*/, CounterBase* TCCB) {
    UInt_t nPUs = TCCB->getValue(kVarHLTPUs, kSavePerCall); //Filled with '1' for every unique processing unit seen by
                                                              // this counter
    Float_t lbTime = TCCB->getDecoration(kDecLbLength); //Time in s of this counter's LB
    Float_t steeringTime = TCCB->getValue(kVarSteeringTime, kSavePerEvent) / 1000.; // Total steering time, convert to
                                                                                      // seconds
    return 100. * (steeringTime / static_cast<Float_t>(nPUs * lbTime)); // Convert to %
  }

  Float_t tableFnGlobalGetHLTNodePrediction(CounterMap_t* /*map*/, CounterBase* TCCB) {
    Float_t lbTime = TCCB->getDecoration(kDecLbLength); //Time in s of this counter's LB - taking into account how
                                                          // much of the run we've seen
    Float_t algTime = TCCB->getValue(kVarAlgTime, kSavePerEvent) / 1000.; // Total alg wall time, convert to seconds
    if (isZero(lbTime) == kTRUE) return 0.;

    // We enough HLT XPUs to process algTime amount of info in lbTime.
    return algTime / lbTime;
  }

  Float_t tableFnGlobalGetHLTNodePredictionErr(CounterMap_t* /*map*/, CounterBase* TCCB) {
    // err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)
    Float_t algTimeErr = TCCB->getValueError(kVarAlgTime, kSavePerCall); // Get sqrt(sumW2)
    Float_t walltime = TCCB->getDecoration(kDecLbLength);
    return algTimeErr / walltime;
  }

  Float_t tableFnGlobalGetHLTNodePredictionSteering(CounterMap_t* /*map*/, CounterBase* TCCB) {
    Float_t lbTime = TCCB->getDecoration(kDecLbLength); //Time in s of this counter's LB - taking into account how
                                                          // much of the run we've seen
    Float_t algTime = TCCB->getValue(kVarSteeringTime, kSavePerEvent) / 1000.; // Total alg wall time, convert to
                                                                                 // seconds
    if (isZero(lbTime) == kTRUE) return 0.;

    // We enough HLT XPUs to process algTime amount of info in lbTime.
    return algTime / lbTime;
  }

Float_t tableFnGlobalGetHLTNodePredictionErrSteering(CounterMap_t* /*map*/, CounterBase* TCCB) {
    // err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)
    Float_t algTimeErr = TCCB->getValueError(kVarSteeringTime, kSavePerEvent); // Get sqrt(sumW2)
    Float_t walltime = TCCB->getDecoration(kDecLbLength);
    return algTimeErr / walltime;
  }

  /////////////////////////////////////
  /// BEGIN RATES MONITOR FUNCTIONS ///
  /////////////////////////////////////

  Float_t tableFnRateGetWeightedRateErr(CounterMap_t* /*map*/, CounterBase* TCCB) {
    // err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)
    Float_t evPassErr = TCCB->getValueError(kVarEventsPassed, kSavePerCall); // Get sqrt(sumW2)
    Float_t walltime = TCCB->getDecoration(kDecLbLength);
    return evPassErr / walltime;
  }

  Float_t tableFnRateGetDirectRateErr(CounterMap_t* /*map*/, CounterBase* TCCB) {
    Float_t evPassErr = TCCB->getValueError(kVarEventsPassedDP, kSavePerCall); // Get sqrt(sumW2)
    Float_t walltime = TCCB->getDecoration(kDecLbLength);
    return evPassErr / walltime;
  }
} // namespace TrigCostRootAnalysis
