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
#include "../TrigCostRootAnalysis/CounterRatesChain.h"
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
  CounterRatesChain::CounterRatesChain(const TrigCostData* costData, const std::string& name, Int_t ID,
                                       UInt_t detailLevel, MonitorBase* parent) :
    CounterBaseRates(costData, name, ID, detailLevel, parent) {
  }

  /**
   * Counter destructor. Nothing currently to delete.
   */
  CounterRatesChain::~CounterRatesChain() {
  }

  /**
   * Return if this chain passes the HLT and at least one L1 by checking both pass-raw and pass-prescale bits.
   * This is the naive method.
   * @param _usePrescale - if set to kTRUE (default) then the prescale will be simulated, otherwise the prescale is
   *taken to be 1.
   * @return 1 if the chain passes, 0 if not.
   */
  Float_t CounterRatesChain::runDirect(Bool_t usePrescale) {
    assert(m_L2s.size() == 1 || m_L1s.size() == 1); // We should only be one chain
    if (m_L2s.size() == 1) { // A HLT Chain
      RatesChainItem* L2 = (*m_L2s.begin());
      // L2 must pass raw and pass PS
      if (L2->getPassRaw() == kFALSE) return 0.;

      if (usePrescale == kTRUE && L2->getPassPS() == kFALSE) return 0.;

      // At least one L1 seed must pass Raw and pass PS
      for (ChainItemSetIt_t lowerIt = L2->getLowerStart(); lowerIt != L2->getLowerEnd(); ++lowerIt) {
        RatesChainItem* L1 = (*lowerIt);

        if (L1->getPassRaw() == kFALSE) continue;
        if (usePrescale == kTRUE && L1->getPassPS() == kFALSE) continue;
        return 1; // This chain and lower chain have both passed raw and passed PS
      }
      return 0.;
    } else { // m_L1s.size() == 1
      RatesChainItem* L1 = (*m_L1s.begin());
      // L1 must pass raw and pass PS
      if (L1->getPassRaw() == kFALSE) return 0.;

      if (usePrescale == kTRUE && L1->getPassPS() == kFALSE) return 0.;

      return 1;
    }
  }

  /**
   * If this chain passes-raw at both the HLT and L1 levels, then calculate the effective weight from all prescale
   *factors.
   * @return Event prescale weight for this chain 0 < PS weight < 1
   */
  Double_t CounterRatesChain::runWeight(Bool_t includeExpress) {
    // This is a sub-case of the ChainOR, however here we know we only have one chain at L2.
    // However, this chain may have been seeded by many L1's.

    // See Eq 33 from http://arxiv.org/abs/0901.4118

    assert(m_L2s.size() == 1 || m_L1s.size() == 1); // We should only be one chain
    m_cachedWeight = 0.;

    if (m_L2s.size() == 1) { // A HLT Chain
      RatesChainItem* L2 = (*m_L2s.begin());

      // First we check that the one L2 passed
      if (L2->getPassRaw() == kFALSE) return 0.;

      Double_t L1Weight = 1.;
      for (ChainItemSetIt_t lowerIt = L2->getLowerStart(); lowerIt != L2->getLowerEnd(); ++lowerIt) {
        RatesChainItem* L1 = (*lowerIt);
        L1Weight *= (1. - L1->getPassRawOverPS());
      }

      m_eventLumiExtrapolation =
        L2->getLumiExtrapolationFactor(m_costData->getLumi(), m_disableEventLumiExtrapolation);
      //if (getName() == "HLT_cscmon_L1All") Info("DEBUG", "WL1:%f, NL1:%s, 1-L1: %f, HLT:%f, total: %f, lumi%f",
      // m_lowerRates->getLastWeight(), m_lowerRates->getName().c_str(),  1. - L1Weight,
      // L2->getPSWeight(includeExpress), L2->getPSWeight(includeExpress) * ( 1. - L1Weight ),
      //  L2->getLumiExtrapolationFactor()  );
      m_cachedWeight = L2->getPSWeight(includeExpress) * (1. - L1Weight);
    } else { // A L1Chain
      RatesChainItem* L1 = (*m_L1s.begin());
      m_eventLumiExtrapolation =
        L1->getLumiExtrapolationFactor(m_costData->getLumi(), m_disableEventLumiExtrapolation);
      m_cachedWeight = L1->getPassRawOverPS();
    }
    return m_cachedWeight;
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterRatesChain::debug(UInt_t /*e*/) {
  }
} // namespace TrigCostRootAnalysis
