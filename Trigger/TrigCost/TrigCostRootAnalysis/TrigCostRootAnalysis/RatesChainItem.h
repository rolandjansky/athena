// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
#include "MonitorRatesUpgrade.h" // Special class defs

// ROOT includes
#include <TRandom3.h>

namespace TrigCostRootAnalysis {
  //Forward declaration
  class CounterBaseRates;

  /**
   * @class RatesChainItem
   * One instance is needed for each L1 and each HLT item in the menu.
   * HLT items are connected to the L1 items which seed them.
   * The object is loaded with its prescale and (per event) if it passed raw.
   * They is used by single-chain and combination-chain rates counters to calculate the correct weight per event.
   */
  class RatesChainItem {
  public:
    RatesChainItem(std::string name, Int_t level, Double_t PS, Double_t PSExpress = 1.);

    void addLower(RatesChainItem* lower);
    void addUpper(RatesChainItem* upper);
    void addCounter(CounterBaseRates* client);
    ChainItemSetIt_t getLowerStart();
    ChainItemSetIt_t getLowerEnd();
    ChainItemSet_t& getLower();
    ChainItemSetIt_t getUpperStart();
    ChainItemSetIt_t getUpperEnd();
    ChainItemSet_t& getUpper();
    Bool_t getLowerContains(RatesChainItem* find);
    Bool_t getLowerContainsAll(std::set<RatesChainItem*>& set);
    Bool_t getUpperContains(RatesChainItem* find);
    Bool_t getUpperContainsAll(std::set<RatesChainItem*>& set);
    void setExtraEfficiency(Double_t extraEfficiency);
    void setRateReductionFactor(Double_t reductionFactor);
    void setTriggerLogic(TriggerLogic* tl);
    void setProxy(CounterBaseRates* c) {m_proxy = c;}
    void fillHistograms(DataStore& dataStore, Float_t weight, Float_t bunchWeight);

    void beginEvent(Bool_t passRaw, CounterBaseRatesSet_t& counterSet);
    void beginEvent(TOBAccumulator* eventTOBs );

    void endEvent();
    void newRandomPS();

    void classifyBunchGroup();
    void classifyLumiAndRandom();

    Bool_t getInEvent();
    Bool_t getPassRaw();
    Bool_t getPassPS();
    // Bool_t getIsNotPhysics();
    Double_t getPSWeight(Bool_t includeExpress = kFALSE);
    Double_t getPSReducedWeight(Bool_t includeExpress = kFALSE);
    Double_t getPS();
    void setPS(Double_t PS);
    void setPSReduced(Double_t PSReduced);
    Double_t getPassRawOverPS(Bool_t includeExpress = kFALSE);
    Double_t getPassRawOverPSReduced(Bool_t includeExpress = kFALSE);
    Bool_t getPassRawAndPS();
    const std::string& getName();
    UInt_t getID();
    TriggerLogic* getTriggerLogic();
    Double_t getLumiExtrapolationFactor(UInt_t lb, Bool_t disableEventLumiExtrapolation);
  private:
    std::string m_name; //!< This chain item's name, more for debug
    Int_t m_level; //!> Which level this item's at
    Double_t m_PS; //!< The prescale to be applied to this item
    Double_t m_PSWeight; //!< The equivalent PS weight, = 1/PS
    Double_t m_PSReduced; //!< prescaled with any coherent part factored out
    Double_t m_PSReducedWeight; //!< = 1/PSReduced
    Double_t m_PSExpressWeight; //!< 1/Express stream prescale
    Double_t m_extraEfficiency; //<! Extra factor which can be supplied to scale the rate of this trigger
    TRandom3 m_R; //!< Random number generator for when applying the PS directly.
    UInt_t m_ID; //!< ID number, sequential, used for random seed
    static UInt_t s_chainCount; //!< Static counter of how many RatesChainItems have been made, used as random seed.
    EBBunchGroupType_t m_bunchGroupType; //!< If L1 chain, what bunch group I have been identified to. Used with EB
                                         // weighting.

    IntDoubleMap_t* m_lumiExtrapolationMap; //!< Luminosity scaling varies per LB

    Bool_t m_passRaw; //!< If this chain item passed in this event
    Bool_t m_passPS; //!< If this chain item passed just the PS check in this event
    Bool_t m_inEvent; //!< If this chain item was at least run in this event.
    Bool_t m_forcePassRaw; //!< A debug setting, when true all chains will always accept. Allow investigation of PS
                           // weighting.
    Bool_t m_doEBWeighting; //!< If enhanced bias weights are to be applied
    Bool_t m_doDirectPS; //<! True if calculating the direct application of prescales
    Bool_t m_matchRandomToOnline; //!< Butter setting, if true and I am a L1_RDX trigger then I should online fire when
                                  // online unbiased fired
    Bool_t m_advancedLumiScaling; //!< Use Mu and Bunches info in select cases
    Bool_t m_iAmRandom; //!< True if I am a L1 item of type L1_RDX or seed from one of these

    ChainItemSet_t m_lower; //!< Pointers to seeding chains from lower levels
    ChainItemSet_t m_upper; //!< Pointers to seeding chains from lower levels

    TriggerLogic* m_triggerLogic; //!< If set, this logic is to be used to determine if the event passed or not.

    CounterBaseRates* m_proxy; //!< Only currently used by the "UNSEEDED" item, this exceptionally gets its weight from
                               // the L1 rates counter. m_proxy should point to this counter.

    CounterBaseRatesSet_t m_clients; //!< Set of pointers to client rates counters which use this chain item.

    std::vector<Float_t> m_bufferJetRoIEta; //!< For upgrade rates, eta of RoI(s) passing this item's threshold(s)
    std::vector<Float_t> m_bufferMuRoIEta; //!< For upgrade rates, eta of RoI(s) passing this item's threshold(s)
    std::vector<Float_t> m_bufferEmRoIEta; //!< For upgrade rates, eta of RoI(s) passing this item's threshold(s)
    std::vector<Float_t> m_bufferTauRoIEta; //!< For upgrade rates, eta of RoI(s) passing this item's threshold(s)
    Float_t m_bufferMu;       //!< For upgrade rates, <mu> of the event
  }; // Class RatesChainItem
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_RatesChainItem_H
