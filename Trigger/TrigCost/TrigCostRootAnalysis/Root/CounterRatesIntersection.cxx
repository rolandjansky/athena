// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <assert.h>

// ROOT include(s):
#include <TError.h>

// Local include(s):
#include "../TrigCostRootAnalysis/CounterRatesIntersection.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/RatesChainItem.h"

namespace TrigCostRootAnalysis {

  /**
   * Counter to monitor the rates of a single chain.
   * @param _costData Const pointer to the data store, not used by this counter at the moment.
   * @param _name Const ref to chain's name
   * @param _ID Chain's ID number.
   */
  CounterRatesIntersection::CounterRatesIntersection( const TrigCostData* _costData, const std::string& _name, Int_t _ID, UInt_t _detailLevel, MonitorBase* _parent ) :
    CounterBaseRates(_costData, _name, _ID, _detailLevel, _parent), m_redundanciesRemoved(kFALSE) {
  }

  /**
   * Counter destructor. Nothing currently to delete.
   */
  CounterRatesIntersection::~CounterRatesIntersection() {
  }

  /**
   * This is the naive method.
   * All L2 chains must pass raw and pass PS, plus at least one of each of their L1 chains must pass raw and pass PS too
   * @param _usePrescale - if set to kTRUE (default) then the prescale will be simulated, otherwise the prescale is taken to be 1.
   * @return 1 if all the chains passes, 0 if not.
   */
  Float_t CounterRatesIntersection::runDirect(Bool_t _usePrescale) {

    for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
      RatesChainItem* _L2 = (*_L2It);

      if (_L2->getPassRaw() == kFALSE ) return 0; // L2 did not pass - stop
      if (_usePrescale == kTRUE && _L2->getPassPS() == kFALSE) return 0.;

      Bool_t _l1Pass = kFALSE;
      for (ChainItemSetIt_t _lowerIt = _L2->getLowerStart(); _lowerIt != _L2->getLowerEnd(); ++_lowerIt) {
        RatesChainItem* _L1 = (*_lowerIt);

        if ( _L1->getPassRaw() == kFALSE ) continue; // L1 did not pass, try next
        if (_usePrescale == kTRUE && _L1->getPassPS() == kFALSE) continue;
        _l1Pass = kTRUE;
        break;

      }
      if (_l1Pass == kFALSE) return 0.; // No L1 passed - stop.
    }
    // All L2 passed with at least one L1 passing
    return 1.;
  }

  /**
   * The AND case of exclusive combinations of triggers is not that bad.
   * First we reduce the list of L1 items to the minimal set of items needed to seed all items in the L2 set.
   * This only needs to be done once per trigger configuration.
   * Then we simply check that all L1 and L2 items passed raw and tot up their prescale weights.
   */
  Double_t CounterRatesIntersection::runWeight() {

    if (m_redundanciesRemoved == kFALSE) removeRedundancies();
    Double_t _w = 1.;

    for (ChainItemSetIt_t _L2It = m_L2s.begin(); _L2It != m_L2s.end(); ++_L2It) {
      RatesChainItem* _L2 = (*_L2It);
      if ( _L2->getPassRaw() == kFALSE ) return 0.;
      _w *= _L2->getPSWeight();
    }

    for (ChainItemSetIt_t _L1It = m_L1s.begin(); _L1It != m_L1s.end(); ++_L1It) {
      RatesChainItem* _L1 = (*_L1It);
      if ( _L1->getPassRaw() == kFALSE ) return 0.;
      _w *= _L1->getPSWeight();
    }

    return _w;
  }

  /**
   * We need to get the minimal set of L1s which can still reach all L2s
   * Try removing one L1 at a time and see if all L2s are still reachable
   */
  void CounterRatesIntersection::removeRedundancies() {

    ChainItemSet_t _L1Set = m_L1s;
    ChainItemSet_t _L2Set;

    UInt_t _L1ToTryRemoving = 0;
    while (_L1ToTryRemoving < _L1Set.size()) {
      ChainItemSetIt_t _L1ToTryRemovingIt; // Keep a iterator to the L1 item we're testing to have the option to remove it
      UInt_t _N = 0; // Manual counter as we're iterating over a set
      for (ChainItemSetIt_t _L1It = _L1Set.begin(); _L1It != _L1Set.end(); ++_L1It) {
        RatesChainItem* _L1 = (*_L1It);
        if (_L1It == _L1Set.begin()) { // reset L2 list
          _L2Set = m_L2s;
        }

        if ( _L1ToTryRemoving != _N++ ) { // If I'm *not* the L1 we're querying

          for (ChainItemSetIt_t _L2It = _L2Set.begin(); _L2It != _L2Set.end(); /*note no increment*/ ) {
            RatesChainItem* _L2 = (*_L2It);   // Look at this all L2s left in the list.
            Bool_t _isSeeded = kFALSE;
            for (ChainItemSetIt_t _lowerIt = _L2->getLowerStart(); _lowerIt != _L2->getLowerEnd(); ++_lowerIt) {
              RatesChainItem* _lower = (*_lowerIt); // Now look at all the seeds of this L2
              if (_lower == _L1) {
                // This L2 is seeded by the L1 - we can remove it
                _isSeeded = kTRUE;
                break;
              }
            }
            if (_isSeeded == kTRUE) {
              // Remove this chain from the list of L2s
              _L2Set.erase( _L2It++ ); // Increment iterator after the delete
            } else {
              ++_L2It;
            }
          }

        } else { // If *am* the L1 we're querying, remember my location
          _L1ToTryRemovingIt = _L1It;
        }
      }
      // We have looped over all but one of the list of L1 items, have we found one we can remove?
      if (_L2Set.size() == 0) { // If true we could still reach all the counters *without* this L1
        if (Config::config().debug()) {
          Info("CounterRatesIntersection::removeRedundancies","Removing redundant L1 chain %s from %s",
            (*_L1ToTryRemovingIt)->getName().c_str(), getName().c_str());
        }
        _L1Set.erase(_L1ToTryRemovingIt);
        _L1ToTryRemoving = 0; // Go back to the beginning and try again
      } else { // We cannot remove this L1 and should try removing the next L1 up until there are no more to try removing
        ++_L1ToTryRemoving;
      }
    }

    // Update the new L1 set
    m_L1s = _L1Set;
    m_redundanciesRemoved = kTRUE;
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterRatesIntersection::debug(UInt_t _e) {
    UNUSED(_e);
  }

} // namespace TrigCostRootAnalysis
