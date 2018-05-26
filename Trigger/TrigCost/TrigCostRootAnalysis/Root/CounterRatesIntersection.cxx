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
   * @param costData Const pointer to the data store, not used by this counter at the moment.
   * @param name Const ref to chain's name
   * @param ID Chain's ID number.
   */
  CounterRatesIntersection::CounterRatesIntersection(const TrigCostData* costData, const std::string& name, Int_t ID,
                                                     UInt_t detailLevel, MonitorBase* parent) :
    CounterBaseRates(costData, name, ID, detailLevel, parent), m_redundanciesRemoved(kFALSE) {
  }

  /**
   * Counter destructor. Nothing currently to delete.
   */
  CounterRatesIntersection::~CounterRatesIntersection() {
  }

  /**
   * This is the naive method.
   * All L2 chains must pass raw and pass PS, plus at least one of each of their L1 chains must pass raw and pass PS too
   * @param usePrescale - if set to kTRUE (default) then the prescale will be simulated, otherwise the prescale is
   *taken to be 1.
   * @return 1 if all the chains passes, 0 if not.
   */
  Float_t CounterRatesIntersection::runDirect(Bool_t usePrescale) {
    for (ChainItemSetIt_t L2It = m_L2s.begin(); L2It != m_L2s.end(); ++L2It) {
      RatesChainItem* L2 = (*L2It);

      if (L2->getPassRaw() == kFALSE) return 0;                                                     // L2 did not pass
                                                                                                     // - stop

      if (usePrescale == kTRUE && L2->getPassPS() == kFALSE) return 0.;

      Bool_t l1Pass = kFALSE;
      for (ChainItemSetIt_t lowerIt = L2->getLowerStart(); lowerIt != L2->getLowerEnd(); ++lowerIt) {
        RatesChainItem* L1 = (*lowerIt);

        if (L1->getPassRaw() == kFALSE) continue;                                                        // L1 did not
                                                                                                          // pass, try
                                                                                                          // next
        if (usePrescale == kTRUE && L1->getPassPS() == kFALSE) continue;
        l1Pass = kTRUE;
        break;
      }
      if (l1Pass == kFALSE) return 0.; // No L1 passed - stop.
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
  Double_t CounterRatesIntersection::runWeight(Bool_t includeExpress) {
    m_eventLumiExtrapolation = 0;
    m_cachedWeight = 0.;
    Double_t lumiExtrapNumerator = 0., lumiExtrapDenominator = 0.;

    if (m_redundanciesRemoved == kFALSE) removeRedundancies();
    Double_t w = 1.;

    for (ChainItemSetIt_t L2It = m_L2s.begin(); L2It != m_L2s.end(); ++L2It) {
      RatesChainItem* L2 = (*L2It);
      if (L2->getPassRaw() == kFALSE) return 0.;

      w *= L2->getPSWeight(includeExpress);

      lumiExtrapNumerator += L2->getPassRawOverPS(includeExpress) * L2->getLumiExtrapolationFactor(
        m_costData->getLumi(), m_disableEventLumiExtrapolation);
      lumiExtrapDenominator += L2->getPassRawOverPS(includeExpress);
    }

    for (ChainItemSetIt_t L1It = m_L1s.begin(); L1It != m_L1s.end(); ++L1It) {
      RatesChainItem* L1 = (*L1It);
      if (L1->getPassRaw() == kFALSE) return 0.;

      w *= L1->getPSWeight();
    }

    if (lumiExtrapDenominator) m_eventLumiExtrapolation = lumiExtrapNumerator / lumiExtrapDenominator;
    m_cachedWeight = w;
    return m_cachedWeight;
  }

  /**
   * We need to get the minimal set of L1s which can still reach all L2s
   * Try removing one L1 at a time and see if all L2s are still reachable
   */
  void CounterRatesIntersection::removeRedundancies() {
    ChainItemSet_t L1Set = m_L1s;
    ChainItemSet_t L2Set;

    UInt_t L1ToTryRemoving = 0;

    while (L1ToTryRemoving < L1Set.size()) {
      ChainItemSetIt_t L1ToTryRemovingIt; // Keep a iterator to the L1 item we're testing to have the option to remove
                                           // it
      UInt_t N = 0; // Manual counter as we're iterating over a set
      for (ChainItemSetIt_t L1It = L1Set.begin(); L1It != L1Set.end(); ++L1It) {
        RatesChainItem* L1 = (*L1It);
        if (L1It == L1Set.begin()) { // reset L2 list
          L2Set = m_L2s;
        }

        if (L1ToTryRemoving != N++) { // If I'm *not* the L1 we're querying
          for (ChainItemSetIt_t L2It = L2Set.begin(); L2It != L2Set.end(); /*note no increment*/) {
            RatesChainItem* L2 = (*L2It);   // Look at this all L2s left in the list.
            Bool_t isSeeded = kFALSE;
            for (ChainItemSetIt_t lowerIt = L2->getLowerStart(); lowerIt != L2->getLowerEnd(); ++lowerIt) {
              RatesChainItem* lower = (*lowerIt); // Now look at all the seeds of this L2
              if (lower == L1) {
                // This L2 is seeded by the L1 - we can remove it
                isSeeded = kTRUE;
                break;
              }
            }
            if (isSeeded == kTRUE) {
              // Remove this chain from the list of L2s
              L2Set.erase(L2It++); // Increment iterator after the delete
            } else {
              ++L2It;
            }
          }
        } else { // If *am* the L1 we're querying, remember my location
          L1ToTryRemovingIt = L1It;
        }
      }
      // We have looped over all but one of the list of L1 items, have we found one we can remove?
      if (L2Set.size() == 0) { // If true we could still reach all the counters *without* this L1
        if (Config::config().debug()) {
          Info("CounterRatesIntersection::removeRedundancies", "Removing redundant L1 chain %s from %s",
               (*L1ToTryRemovingIt)->getName().c_str(), getName().c_str());
        }
        L1Set.erase(L1ToTryRemovingIt);
        L1ToTryRemoving = 0; // Go back to the beginning and try again
      } else { // We cannot remove this L1 and should try removing the next L1 up until there are no more to try
               // removing
        ++L1ToTryRemoving;
      }
    }

    // Update the new L1 set
    m_L1s = L1Set;
    m_redundanciesRemoved = kTRUE;
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterRatesIntersection::debug(UInt_t /*e*/) {
  }
} // namespace TrigCostRootAnalysis
