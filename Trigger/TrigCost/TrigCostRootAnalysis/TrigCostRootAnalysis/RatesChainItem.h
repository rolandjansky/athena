/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Dear emacs, this is -*- c++ -*-
// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_RatesChainItem_H
#define TrigCostRootAnalysis_RatesChainItem_H

// STL include(s):
#include <set>
#include <string>

// Local include(s):
#include "Utility.h"

// ROOT includes
#include <TRandom3.h>

namespace TrigCostRootAnalysis {
  
  /**
   * @class RatesChainItem
   * One instance is needed for each L1 and each HLT item in the menu.
   * HLT items are connected to the L1 items which seed them.
   * The object is loaded with its prescale and (per event) if it passed raw.
   * They is used by single-chain and combination-chain rates counters to calculate the correct weight per event. 
   */
  class RatesChainItem {
   public:

    RatesChainItem(std::string _name, Int_t _level, Double_t _PS);

    void addLower(RatesChainItem* _lower);
    ChainItemSetIt_t getLowerStart();
    ChainItemSetIt_t getLowerEnd();
    ChainItemSet_t& getLower();
    Bool_t getLowerContains(RatesChainItem* _find);
    Bool_t getLowerContainsAll( std::set<RatesChainItem*>& _set );

    void beginEvent(Bool_t _passRaw);
    void endEvent();
    void newRandomPS();

    void classifyBunchGroup();

    Bool_t getInEvent();
    Bool_t getPassRaw();
    Bool_t getPassPS();
    // Bool_t getIsNotPhysics();
    Double_t getPSWeight();
    Double_t getPS();
    Double_t getPassRawOverPS();
    Bool_t getPassRawAndPS();
    const std::string& getName();
    UInt_t getID();

   private:
   
    std::string        m_name; //!< This chain item's name, more for debug
    Int_t              m_level; //!> Which level this item's at
    Double_t           m_PS; //!< The prescale to be applied to this item
    Double_t           m_PSWeight; //!< The equivalent PS weight, = 1/PS
    TRandom3           m_R; //!< Random number generator for when applying the PS directly.
    UInt_t             m_ID; //!< ID number, sequential, used for random seed
    static UInt_t      s_chainCount; //!< Static counter of how many RatesChainItems have been made, used as random seed.
    EBBunchGroupType_t m_bunchGroupType; //!< If L1 chain, what bunch group I have been identified to. Used with EB weighting.

    Bool_t        m_passRaw; //!< If this chain item passed in this event
    Bool_t        m_passPS; //!< If this chain item passed just the PS check in this event
    Bool_t        m_inEvent; //!< If this chain item was at least run in this event.
    Bool_t        m_forcePassRaw; //!< A debug setting, when true all chains will always accept. Allow investigation of PS weighting.
    Bool_t        m_doEBWeighting; //!< If enhanced bias weights are to be applied

    ChainItemSet_t m_lower; // Pointers to seeding chains from lower levels


  }; // Class RatesChainItem

  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_RatesChainItem_H


