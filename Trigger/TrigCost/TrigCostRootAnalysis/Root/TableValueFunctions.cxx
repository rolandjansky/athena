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
   * @param _map Map of CounterBase pointers.
   * @param _TCCB Pointer to the counter being assessed for this row.
   * @returns The table column value.
   */
  Float_t tableFnChainGetTotalFracTime(CounterMap_t* _map, CounterBase* _TCCB) {
    static Float_t _bufferedTotalTime = 0.;
    static CounterMap_t* _bufferedMap = 0;
    
    // Get this chain's fraction of the total time.
    // We first need to get what the total time of all chains is for a given counterMap.
    if (_bufferedMap != _map) {
      _bufferedMap = _map;
      _bufferedTotalTime = 0.;
      CounterMapIt_t _counterMapIt = _map->begin();
      for (; _counterMapIt != _map->end(); ++_counterMapIt) {
        CounterBase* _counter = _counterMapIt->second;
        _bufferedTotalTime += _counter->getValue(kVarTime, kSavePerEvent);
      }
    }
    
    // Now we can return a fractional value (check for /0)
    if ( isZero(_bufferedTotalTime) ) return 0.;
    Float_t _thisChainTime = _TCCB->getValue(kVarTime, kSavePerEvent);
    
    return (_thisChainTime / _bufferedTotalTime) * 100; // In %
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

  Float_t tableFnGlobalGetSteeringFarmUse(CounterMap_t* _map, CounterBase* _TCCB) {
    UNUSED( _map );

    UInt_t _nPUs = _TCCB->getValue(kVarHLTPUs, kSavePerCall); //Filled with '1' for every unique processing unit seen by this counter
    Float_t _lbTime = _TCCB->getDecoration(kDecLbLength); //Time in s of this counter's LB
    Float_t _steeringTime = _TCCB->getValue(kVarSteeringTime, kSavePerEvent) / 1000.; // Total steering time, convert to seconds
    return 100. * (_steeringTime / static_cast<Float_t>( _nPUs * _lbTime )); // Convert to %

  }

  Float_t tableFnGlobalGetHLTNodePrediction(CounterMap_t* _map, CounterBase* _TCCB) {
    UNUSED( _map );

    Float_t _lbTime = _TCCB->getDecoration(kDecLbLength); //Time in s of this counter's LB - taking into account how much of the run we've seen
    Float_t _algTime = _TCCB->getValue(kVarAlgTime, kSavePerEvent) / 1000.; // Total alg wall time, convert to seconds  
    if (isZero(_lbTime) == kTRUE) return 0.;
    // We enough HLT XPUs to process algTime amount of info in lbTime. 
    return _algTime / _lbTime;
  }

  Float_t tableFnGlobalGetHLTNodePredictionErr(CounterMap_t* _map, CounterBase* _TCCB) {
    // err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)
    UNUSED( _map );
    Float_t _algTimeErr = _TCCB->getValueError(kVarAlgTime, kSavePerCall); // Get sqrt(sumW2)
    Float_t _walltime = _TCCB->getDecoration(kDecLbLength);
    return _algTimeErr / _walltime;  
  }

    Float_t tableFnGlobalGetHLTNodePredictionSteering(CounterMap_t* _map, CounterBase* _TCCB) {
    UNUSED( _map );

    Float_t _lbTime = _TCCB->getDecoration(kDecLbLength); //Time in s of this counter's LB - taking into account how much of the run we've seen
    Float_t _algTime = _TCCB->getValue(kVarSteeringTime, kSavePerEvent) / 1000.; // Total alg wall time, convert to seconds  
    if (isZero(_lbTime) == kTRUE) return 0.;
    // We enough HLT XPUs to process algTime amount of info in lbTime. 
    return _algTime / _lbTime;
  }

  Float_t tableFnGlobalGetHLTNodePredictionErrSteering(CounterMap_t* _map, CounterBase* _TCCB) {
    // err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)
    UNUSED( _map );
    Float_t _algTimeErr = _TCCB->getValueError(kVarSteeringTime, kSavePerEvent); // Get sqrt(sumW2)
    Float_t _walltime = _TCCB->getDecoration(kDecLbLength);
    return _algTimeErr / _walltime;  
  }

  /////////////////////////////////////
  /// BEGIN RATES MONITOR FUNCTIONS ///
  /////////////////////////////////////

  Float_t tableFnRateGetWeightedRateErr(CounterMap_t* _map, CounterBase* _TCCB) {
    // err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)
    UNUSED( _map );
    Float_t _evPassErr = _TCCB->getValueError(kVarEventsPassed, kSavePerCall); // Get sqrt(sumW2)
    Float_t _walltime = _TCCB->getDecoration(kDecLbLength);
    return _evPassErr / _walltime;  
  }

  Float_t tableFnRateGetDirectRateErr(CounterMap_t* _map, CounterBase* _TCCB) {
    UNUSED( _map );
    Float_t _evPassErr = _TCCB->getValueError(kVarEventsPassedDP, kSavePerCall); // Get sqrt(sumW2)
    Float_t _walltime = _TCCB->getDecoration(kDecLbLength);
    return _evPassErr / _walltime; 
  }

} // namespace TrigCostRootAnalysis
