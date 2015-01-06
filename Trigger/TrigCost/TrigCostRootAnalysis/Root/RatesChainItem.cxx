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
  RatesChainItem::RatesChainItem(std::string _name, Int_t _level, Double_t _PS) : 
    m_name(_name),
    m_level(_level),
    m_PS(_PS), // Integer prescale
    m_PSWeight(1./m_PS), // Reciprocal of the prescale - this is the basic weight quantity for this ChainItem
    m_R( ++s_chainCount ),
    m_ID( s_chainCount ),
    m_bunchGroupType(kBG_UNSET),
    m_passRaw(kFALSE),
    m_passPS(kFALSE),
    m_inEvent(kFALSE)
  {
    if (Config::config().debug()) {
      Info("RatesChainItem::RatesChainItem","New ChainItem:%s, Level:%i PS:%f", m_name.c_str(), m_level, m_PS);
    }

    m_doEBWeighting = Config::config().getInt(kDoEBWeighting); // Cache for speed

    // If L1: then classify my bunchgroup
    if (m_level == 1) classifyBunchGroup();

    if (m_PS < 0.) m_PSWeight = 0.;
    m_forcePassRaw = (Bool_t) Config::config().getInt(kRatesForcePass);
  }

  /**
   * Look at L1 items from this counter and classify from their name what type of BG they triggered on
   */
  void RatesChainItem::classifyBunchGroup() {

    if (getName().find("_BPTX") != std::string::npos || getName().find("_BGRP") != std::string::npos) { // Ignore the beam pickup triggers
      m_bunchGroupType = kBG_NONE;
    } else if (getName().find("_FIRSTEMPTY") != std::string::npos) {
      m_bunchGroupType = kBG_FIRSTEMPTY;
    } else if (getName().find("_EMPTY") != std::string::npos) {
      m_bunchGroupType = kBG_EMPTY;
    } else if (getName().find("_UNPAIRED_ISO") != std::string::npos) {
      m_bunchGroupType = kBG_UNPAIRED_ISO;
    } else if (getName().find("_UNPAIRED_NONISO") != std::string::npos) {
      m_bunchGroupType = kBG_UNPAIRED_NONISO;
    } else {
      m_bunchGroupType = kBG_FILLED;
    }

    if (Config::config().debug()) {
      Info("RatesChainItem::classifyBunchGroup","Item %s classified as %s.",
        getName().c_str(), BunchGroupNameStr[m_bunchGroupType].c_str() );
    }

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
  Double_t RatesChainItem::getPS() { 
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
    if (m_PS < 0.) {
      m_passPS = kFALSE;
    } else {
      m_passPS = (m_R.Rndm() < m_PSWeight);
    }
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

    // L1 can only pass raw if this is the correct bunchgroup, only if doing EB weighting
    // TODO - This check is probably not needed as the trigger should have known what chains to run for each BG when it was running
    if (m_level == 1 && m_doEBWeighting == kTRUE) {
      if (m_bunchGroupType != Config::config().getInt(kCurrentEventBunchGroupID)) {
        if (Config::config().debug() && Config::config().getInt(kCurrentEventBunchGroupID) >= 0) {
          Info("RatesChainItem::getPassRaw","Item %s forced FALSE as bunchgroup ID is %s.",
            getName().c_str(), BunchGroupNameStr[Config::config().getInt(kCurrentEventBunchGroupID)].c_str() );
        }
        return kFALSE;
      }
    }

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
