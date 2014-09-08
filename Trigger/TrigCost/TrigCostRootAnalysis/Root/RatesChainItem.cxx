// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// Local include(s):
#include "../TrigCostRootAnalysis/RatesChainItem.h"
#include "../TrigCostRootAnalysis/Config.h"

// ROOT includes
#include <TError.h>

namespace TrigCostRootAnalysis {

  UInt_t RatesChainItem::s_chainCount = 0; // Each ChainItem gets a sequential random seed

  /**
   * Construct new RatesChainItem with a given prescale.
   */
  RatesChainItem::RatesChainItem(std::string _name, Int_t _PS) : 
    m_name(_name),
    m_PS(_PS), // Integer prescale
    m_PSWeight(1./(Double_t)m_PS), // Reciprocal of the prescale - this is the basic weight quantity for this ChainItem
    m_R( ++s_chainCount ),
    m_ID( s_chainCount ),
    m_passRaw(kFALSE),
    m_passPS(kFALSE),
    m_inEvent(kFALSE)
  {
    if (Config::config().debug()) {
      Info("RatesChainItem::RatesChainItem","New ChainItem:%s, PS:%i", m_name.c_str(), m_PS);
    }
    m_forcePassRaw = (Bool_t) Config::config().getInt(kRatesForcePass);
  }

  /**
   * For HLT items, each seeding L1 item should be linked here by passing its pointer.
   * @param _lower The pointer to another RatesChainItem which seeds this instance.
   */
  void RatesChainItem::addLower(RatesChainItem* _lower) { 
    m_lower.insert(_lower);
  } 

  /**
   * @return A const itterator to the start of this counter's set of seeding counters
   */
  ChainItemSetIt_t RatesChainItem::getLowerStart() { 
    return m_lower.begin(); 
  }
  
  /**
   * @return A const itterator to the end of this counter's set of seeding counters
   */
  ChainItemSetIt_t RatesChainItem::getLowerEnd() { 
    return m_lower.end(); 
  }

  /**
   * @return A reference to the set of lower, seeding, items of this item
   */
  ChainItemSet_t& RatesChainItem::getLower() { 
    return m_lower; 
  }

  /**
   * @param _find A chain item pointer to find in this chain item's set of seeding triggers.
   * @return kTRUE if this ChainItem has the supplied ChainItem listed as one of its lower, seeding items.
   */
  Bool_t RatesChainItem::getLowerContains(RatesChainItem* _find) { 
    return static_cast<Bool_t>( m_lower.count( _find ) );
  }

  /**
   * @param _set Reference to a set of chain item pointers to test against.
   * @return kTRUE if *all* of the ChainItems supplied in _set are also listed as lower items of this ChainItem
   */
  Bool_t RatesChainItem::getLowerContainsAll( std::set<RatesChainItem*>& _set ) { 
    for (ChainItemSetIt_t _it = _set.begin(); _it != _set.end(); ++_it) { // Check we contain all these
      if (getLowerContains( (*_it) ) == kFALSE) return kFALSE;
    }
    return kTRUE;
  }

  /**
   * @return The configured prescale value
   */
  Int_t RatesChainItem::getPS() { 
    return m_PS;
  }

  /**
   * @return The chain item's name
   */
  const std::string& RatesChainItem::getName() { 
    return m_name;
  }

  /**
   * @return The chain sequential internal ID
   */
  UInt_t RatesChainItem::getID() { 
    return m_ID;
  }

  /**
   * @param _passRaw If this ChainItem passed raw in this event.
   */
  void RatesChainItem::beginEvent(Bool_t _passRaw) {
    m_passRaw = _passRaw;
    m_inEvent = kTRUE;
    newRandomPS();
  }

  /**
   * Reset all flags to zero
   */
  void RatesChainItem::endEvent() {
    m_passRaw = kFALSE;
    m_passPS = kFALSE;
    m_inEvent = kFALSE;
  }

  /**
   * Update the random prescale to a new value
   */
  void RatesChainItem::newRandomPS() {
    m_passPS = (m_R.Rndm() < m_PSWeight);
  }

  /**
   * @return If this chain item was present in the D3PD this event, regardless of whether or not it passed PS or passed Raw.
   */
  Bool_t RatesChainItem::getInEvent() {
    return m_inEvent;
  }

  /**
   * @return If this chain item passed (raw) in this event.
   */
  Bool_t RatesChainItem::getPassRaw() { 
    if (m_forcePassRaw == kTRUE) return kTRUE;
    return m_passRaw; 
  } 

  /**
   * @return If this chain item passed its prescale in this event.
   */
  Bool_t RatesChainItem::getPassPS() { 
    return m_passPS;
  }

  /**
   * @return 1/Prescale weighting factor for this event.
   */
  Double_t RatesChainItem::getPSWeight() { 
    return m_PSWeight;
  } 

  /**
   * @return Zero if this chain did not pass raw, else returns 1/Prescale
   */
  Double_t RatesChainItem::getPassRawOverPS() { 
    if (getPassRaw() == kFALSE) return 0.;
    return getPSWeight();
  }

  /**
   * @return If this chain passed both Raw and PS
   */
  Bool_t RatesChainItem::getPassRawAndPS() { 
    return (getPassRaw() && getPassPS());
  }

} // namespace TrigCostRootAnalysis
