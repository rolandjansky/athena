// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <bitset>

// Local include(s):
#include "../TrigCostRootAnalysis/RatesChainItem.h"
#include "../TrigCostRootAnalysis/CounterBaseRates.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"

// ROOT includes
#include <TError.h>
#include <TMath.h>

namespace TrigCostRootAnalysis {
  UInt_t RatesChainItem::s_chainCount = 0; // Each ChainItem gets a sequential random seed

  /**
   * Construct new RatesChainItem with a given prescale.
   */
  RatesChainItem::RatesChainItem(std::string name, Int_t level, Double_t PS, Double_t PSExpress) :
    m_name(name),
    m_level(level),
    m_PS(PS), // Integer prescale
    m_PSWeight(1. / m_PS), // Reciprocal of the prescale - this is the basic weight quantity for this ChainItem
    m_PSReduced(1.),
    m_PSReducedWeight(1.),
    m_PSExpressWeight(1. / PSExpress),
    m_extraEfficiency(1.),
    m_R(++s_chainCount),
    m_ID(s_chainCount),
    m_bunchGroupType(kBG_UNSET),
    m_lumiExtrapolationMap(nullptr),
    m_passRaw(kFALSE),
    m_passPS(kFALSE),
    m_inEvent(kFALSE),
    m_doDirectPS(kFALSE),
    m_matchRandomToOnline(kFALSE),
    m_advancedLumiScaling(kFALSE),
    m_iAmRandom(kFALSE),
    m_triggerLogic(nullptr),
    m_proxy(nullptr),
    m_bufferMu(0) {
    if (Config::config().debug()) {
      Info("RatesChainItem::RatesChainItem", "New ChainItem:%s, Level:%i PS:%f", m_name.c_str(), m_level, m_PS);
    }

    m_doEBWeighting = Config::config().getInt(kDoEBWeighting); // Cache for speed
    m_doDirectPS = Config::config().getInt(kDirectlyApplyPrescales); // Cache for speed
    m_advancedLumiScaling = Config::config().getInt(kDoAdvancedLumiScaling);

    if (m_PS <= 0.) m_PSWeight = 0.;
    m_forcePassRaw = (Bool_t) Config::config().getInt(kRatesForcePass);
    m_matchRandomToOnline = (Bool_t) Config::config().getInt(kMatchL1RandomToOnline);
  }

  /**
   * Look at myself and classify from their name what type of BG I trigger on
   */
  void RatesChainItem::classifyBunchGroup() {
    RatesChainItem* toCheck = this;

    if (m_level > 1 && m_lower.size() == 1) toCheck = *(m_lower.begin());

    if (toCheck->getName().find("_BPTX") != std::string::npos ||
        toCheck->getName().find("_BGRP") != std::string::npos) { // Ignore the beam pickup & specialist triggers
      m_bunchGroupType = kBG_NONE;
    } else if (toCheck->getName().find("_FIRSTEMPTY") != std::string::npos) {
      m_bunchGroupType = kBG_FIRSTEMPTY;
    } else if (toCheck->getName().find("_EMPTY") != std::string::npos) {
      m_bunchGroupType = kBG_EMPTY;
    } else if (toCheck->getName().find("_UNPAIRED_ISO") != std::string::npos) {
      m_bunchGroupType = kBG_UNPAIRED_ISO;
    } else if (toCheck->getName().find("_UNPAIRED_NONISO") != std::string::npos) {
      m_bunchGroupType = kBG_UNPAIRED_NONISO;
    } else if (toCheck->getName().find("_ABORTGAPNOTCALIB") != std::string::npos) {
      m_bunchGroupType = kBG_ABORTGAPNOTCALIB;
    } else if (toCheck->getName().find("_CALREQ") != std::string::npos) {
      m_bunchGroupType = kBG_CALREQ;
    } else {
      m_bunchGroupType = kBG_FILLED;
    }

    if (Config::config().debug()) {
      Info("RatesChainItem::classifyBunchGroup", "Item %s classified as %s.",
           getName().c_str(), BunchGroupNameStr[m_bunchGroupType].c_str());
    }
  }

  /**
   * Look at myself and classify if I am a random seeded L1 or HLT
   * If RANDOM and L1, then the rate is independent of lumi. This item gets a lumi extrap. factor of 1
   * If RANDOM and HLT, then the L1 rate is fixed, and the only lumi extrapolation comes from the increase in \<mu\>
   * If NOT RANDOM, then simple linear extrapolation holds.
   */
  void RatesChainItem::classifyLumiAndRandom() {
    classifyBunchGroup();

    RatesChainItem* toCheck = this;
    if (m_level > 1 && m_lower.size() == 1) toCheck = *(m_lower.begin());

    if (toCheck->getName().find("_RD0") != std::string::npos ||
        toCheck->getName().find("_RD1") != std::string::npos ||
        toCheck->getName().find("_RD2") != std::string::npos ||
        toCheck->getName().find("_RD3") != std::string::npos ||
        toCheck->getName().find("_L1RD0") != std::string::npos ||
        toCheck->getName().find("_L1RD1") != std::string::npos ||
        toCheck->getName().find("_L1RD2") != std::string::npos ||
        toCheck->getName().find("_L1RD3") != std::string::npos) {
      m_iAmRandom = kTRUE;
      if (Config::config().debug()) Info("RatesChainItem::classifyLumiAndRandom", "Item %s classified as random",
                                         getName().c_str());
    } else {
      m_iAmRandom = kFALSE;
    }

    if (m_advancedLumiScaling == kFALSE) { // Just do linear extrapolation
      m_lumiExtrapolationMap = &(TrigXMLService::trigXMLService().m_lumiScalingFactorLinear); // Linear (including
                                                                                              // deadtime)
    } else { // m_advancedLumiScaling == kTRUE. Chains can have different extrapolation
      Bool_t specialCase1 = kFALSE, specialCase3 = kFALSE;

      if (m_iAmRandom == kTRUE && m_level == 1) specialCase1 = kTRUE;
      else if (m_iAmRandom == kTRUE && m_level > 1) specialCase3 = kTRUE;

      if (checkPatternNoLumiWeight(getName()) || specialCase1) { // SPECIAL CASE #1
        m_lumiExtrapolationMap = &(TrigXMLService::trigXMLService().m_lumiScalingFactorDeadtimeOnly);
        Config::config().addVecEntry(kListOfNoLumiWeightChains, getName());
      } else if (checkPatternNoMuLumiWeight(getName())) { // SPECIAL CASE #2
        m_lumiExtrapolationMap = &(TrigXMLService::trigXMLService().m_lumiScalingFactorBunchOnly);
        Config::config().addVecEntry(kListOfNoMuLumiWeightChains, getName());
      } else if (checkPatternNoBunchLumiWeight(getName()) || specialCase3) { // SPECIAL CASE #3
        m_lumiExtrapolationMap = &(TrigXMLService::trigXMLService().m_lumiScalingFactorMuOnly);
        Config::config().addVecEntry(kListOfNoBunchLumiWeightChains, getName());
      } else if (checkPatternExponentialWithMu(getName())) { // SPECIAL CASE #4
        if (m_level == 1) { // We allow for different slopes at L1 and HLT
          m_lumiExtrapolationMap = &(TrigXMLService::trigXMLService().m_lumiScalingFactorExpoL1);
        } else {
          m_lumiExtrapolationMap = &(TrigXMLService::trigXMLService().m_lumiScalingFactorExpoHLT);
        }
        Config::config().addVecEntry(kListOfExpoMuLumiWeightChains, getName());
      } else if (m_bunchGroupType == kBG_EMPTY || m_bunchGroupType == kBG_FIRSTEMPTY) {
        // For empty BG, we scale with the *inverse* number of bunches, i.e. more bunches is less empty hence less empty
        // rate
        // Fist empty is a bit of a fudge - assumes that NBunch is proportional to NTrain
        // No deadtime correction here
        m_lumiExtrapolationMap = &(TrigXMLService::trigXMLService().m_lumiScalingFactorEmpty);
      } else if (m_bunchGroupType == kBG_UNPAIRED_ISO || m_bunchGroupType == kBG_UNPAIRED_NONISO ||
                 m_bunchGroupType == kBG_ABORTGAPNOTCALIB || m_bunchGroupType == kBG_CALREQ) {
        // We have no idea how the UNPAIRED items will change in the prediction - for now, zero extrapolation
        // Abort gap and calreq are always the same so again, no extrap here
        m_lumiExtrapolationMap = &(TrigXMLService::trigXMLService().m_lumiScalingFactorUnity);
      } else {
        // The vast majority of chains will get this. This already includes the deadtime weight
        m_lumiExtrapolationMap = &(TrigXMLService::trigXMLService().m_lumiScalingFactorLinear);
      }
    }
  }

  /**
   * @return What this item needs to be scaled by to extrapolate its lumi to the target
   * @see RatesChainItem::classifyRandom
   * @param lb The Lumi Block to return the extrapolation weight for
   * @param disableEventLumiExtrapolation if extrapolation is disabled e.g. for UpgradeRatesMonitor which does this via
   *overlay
   */
  Double_t RatesChainItem::getLumiExtrapolationFactor(UInt_t lb, Bool_t disableEventLumiExtrapolation) {
    if (disableEventLumiExtrapolation) return 1.;

    return m_lumiExtrapolationMap->at(lb); //  Simple. One number per run | Advanced. Different strategies per chain,
                                            // varying per lumi block
  }

  /**
   * User can supply additional scaling factors which will alter the effective efficiency of this chain and hence the
   *rate
   * @param extraEfficiency Additional scaling factor which will scale the rate when the item fires
   */
  void RatesChainItem::setExtraEfficiency(Double_t extraEfficiency) {
    m_extraEfficiency *= extraEfficiency;
  }

  /**
   * Equiv to reciprocal of @see RatesChainItem::setExtraEfficiency
   * @param reductionFactor Scale rate down by this factor
   */
  void RatesChainItem::setRateReductionFactor(Double_t reductionFactor) {
    m_extraEfficiency *= 1. / reductionFactor;
  }

  /**
   * For HLT items, each seeding L1 item should be linked here by passing its pointer.
   * Note we do not own the lower chainItem
   * @param lower The pointer to another RatesChainItem which seeds this instance.
   */
  void RatesChainItem::addLower(RatesChainItem* lower) {
    m_lower.insert(lower);
  }

  /**
   * For L1 items a link to any HLT chanins seeded should be added here
   * @param lower The pointer to another RatesChainItem which is seeded by this instance.
   */
  void RatesChainItem::addUpper(RatesChainItem* upper) {
    m_upper.insert(upper);
  }

  /**
   * Registers that a rates counter makes use of this ChainItem. We can use this info to speed up
   * execution by only processing the counters which we need to.
   * Note we do not own the CounterRates object
   * @param client The pointer to a CounterRates object which makes use of this ChainItem to calculate the event
   *weight.
   */
  void RatesChainItem::addCounter(CounterBaseRates* client) {
    m_clients.insert(client);
  }

  /**
   * @return A const iterator to the start of this counter's set of seeding counters
   */
  ChainItemSetIt_t RatesChainItem::getLowerStart() {
    return m_lower.begin();
  }

  /**
   * @return A const iterator to the end of this counter's set of seeding counters
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
   * @return A const iterator to the start of this counter's set of seeded counters
   */
  ChainItemSetIt_t RatesChainItem::getUpperStart() {
    return m_upper.begin();
  }

  /**
   * @return A const iterator to the end of this counter's set of seeded counters
   */
  ChainItemSetIt_t RatesChainItem::getUpperEnd() {
    return m_upper.end();
  }

  /**
   * @return A reference to the set of seeded items of this item
   */
  ChainItemSet_t& RatesChainItem::getUpper() {
    return m_upper;
  }

  /**
   * @param find A chain item pointer to find in this chain item's set of seeding triggers.
   * @return kTRUE if this ChainItem has the supplied ChainItem listed as one of its lower, seeding items.
   */
  Bool_t RatesChainItem::getLowerContains(RatesChainItem* find) {
    return static_cast<Bool_t>(m_lower.count(find));
  }

  /**
   * @param set Reference to a set of chain item pointers to test against.
   * @return kTRUE if *all* of the ChainItems supplied in set are also listed as lower items of this ChainItem
   */
  Bool_t RatesChainItem::getLowerContainsAll(std::set<RatesChainItem*>& set) {
    for (ChainItemSetIt_t it = set.begin(); it != set.end(); ++it) { // Check we contain all these
      if (getLowerContains((*it)) == kFALSE) return kFALSE;
    }
    return kTRUE;
  }

  /**
   * @param find A chain item pointer to find in this chain item's set of seeded triggers.
   * @return kTRUE if this ChainItem has the supplied ChainItem listed as one of its upper, seeded items.
   */
  Bool_t RatesChainItem::getUpperContains(RatesChainItem* find) {
    return static_cast<Bool_t>(m_upper.count(find));
  }

  /**
   * @param set Reference to a set of chain item pointers to test against.
   * @return kTRUE if *all* of the ChainItems supplied in set are also listed as upper items of this ChainItem
   */
  Bool_t RatesChainItem::getUpperContainsAll(std::set<RatesChainItem*>& set) {
    for (ChainItemSetIt_t it = set.begin(); it != set.end(); ++it) { // Check we contain all these
      if (getUpperContains((*it)) == kFALSE) return kFALSE;
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
   * Sets a new prescale value
   */
  void RatesChainItem::setPS(Double_t PS) {
    m_PS = PS;
    m_PSWeight = 1. / m_PS;
    if (m_PS <= 0.) m_PSWeight = 0.;
  }

  /**
   * Sets a reduced PS value. This is the component of the prescale which is not coherent with other chains in the CPS
   *group
   */
  void RatesChainItem::setPSReduced(Double_t PSReduced) {
    m_PSReduced = PSReduced;
    m_PSReducedWeight = 1. / m_PSReduced;
    if (m_PSReduced <= 0.) m_PSReducedWeight = 0.;
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
   * @param passRaw If this ChainItem passed raw in this event.
   * @param counterSet Set of counters we will process, add to it counters that I influence. This is pass-by-reference
   *and is modified.
   */
  void RatesChainItem::beginEvent(Bool_t passRaw, CounterBaseRatesSet_t& counterSet) {
    m_passRaw = passRaw;
    m_inEvent = kTRUE;
    counterSet.insert(m_clients.begin(), m_clients.end());

    if (m_doDirectPS) newRandomPS(); //TODO - check this is only used for DirectPS application. Saves many calls to
                                     // TRandom3

    // For random seeded triggers where the HLT was re-run, we need to check that we only run over unbiased events in
    // the sample
    if (m_matchRandomToOnline == kTRUE && m_iAmRandom == kTRUE) {
      if (Config::config().getInt(kCurrentEventWasRandomOnline) == kFALSE) {
        m_passRaw = kFALSE;
        m_inEvent = kFALSE;
      }
    }
  }

  /**
   * @param eventTOBs A TOBAccumulator of all TOBs in this event or pseudo-event (simulated high pileup TOB overlay).
   * Note - this function call requires a TriggerLogic pointer to be set, this logic will be used against the set of
   *TOBs
   */
  void RatesChainItem::beginEvent(TOBAccumulator* eventTOBs) {
    m_inEvent = kTRUE;
    static Bool_t largeJetWindow = Config::config().getInt(kUpgradeJetLargeWindow);

    // For random seeded triggers where the HLT was re-run, we need to check that we only run over unbiased events in
    // the sample
    if (m_matchRandomToOnline == kTRUE && m_iAmRandom == kTRUE) {
      if (Config::config().getInt(kCurrentEventWasRandomOnline) == kFALSE) {
        m_passRaw = kFALSE;
        m_inEvent = kFALSE;
        return;
      }
    }

    m_bufferMu = eventTOBs->mu();

    // Loop over logic
    m_passRaw = kTRUE; // Assume we passed, see if we didn't
    for (const TriggerCondition& condition : getTriggerLogic()->conditions()) {
      if (condition.m_type == kMissingEnergyString) {
        if (eventTOBs->METOverflow() == kFALSE && eventTOBs->MET() <= condition.m_thresh) {
          m_passRaw = kFALSE;
          break;
        }
      } else if (condition.m_type == kEnergyString) {
        if (eventTOBs->TEOverflow() == kFALSE && eventTOBs->TE() <= condition.m_thresh) {
          m_passRaw = kFALSE;
          break;
        }
      } else if (condition.m_type == kHTString) {
        if (eventTOBs->HT() <= condition.m_thresh) {
          m_passRaw = kFALSE;
          break;
        }
      } else if (condition.m_type == kMHTString) {
        if (eventTOBs->MHT() <= condition.m_thresh) {
          m_passRaw = kFALSE;
          break;
        }
      } else {  // For EM/JET/TAU/MU
        UInt_t tobsPassingCondition = 0;
        for (const auto& tob : eventTOBs->TOBs()) {
          if (tob.m_type != condition.m_type) continue; // Incorrect type (EM/TAU/MU etc.). Don't discriminate on this
                                                          // one
          Float_t et = tob.m_et;
          if (tob.m_type == kJetString && largeJetWindow == kTRUE) et = tob.m_etLarge;
          // Energy too low ?
          if (tob.m_type == kMuonString) {
            if (et < condition.m_thresh) continue; // Muons are at set thresholds so should be <
          } else {
            if (et <= condition.m_thresh) continue; // From testing on jets, really does seem to be <=
          }
          if (TMath::Abs(tob.m_eta) * 10 < condition.m_min) continue; // eta too low
          if (TMath::Abs(tob.m_eta) * 10 > condition.m_max) continue; // eta too high
          if (condition.m_iso != 0) { // Check isolation bits (if conditions require isolation)
            std::bitset<5> tobIso = tob.m_iso;
            std::bitset<5> conditionIso = condition.m_iso;
            Bool_t pass = kTRUE;
            for (UInt_t b = 0; b < 5; ++b) {
              if (conditionIso.test(b) == kTRUE && tobIso.test(b) == kFALSE) pass = kFALSE;
            }
            if (pass == kFALSE) continue; // A required isolation bit was not found
          }
          ++tobsPassingCondition; // All requirements met
          // Histogram
          if (tob.m_type == kJetString) m_bufferJetRoIEta.push_back(tob.m_eta);
          else if (tob.m_type == kMuonString) m_bufferMuRoIEta.push_back(tob.m_eta);
          else if (tob.m_type == kEmString) m_bufferEmRoIEta.push_back(tob.m_eta);
          else if (tob.m_type == kTauString) m_bufferTauRoIEta.push_back(tob.m_eta);
          //if (tobsPassingCondition == condition.m_multi) break; // Do we have enough TOBs passing this condition?
          // Bail out if so, don't need more
        }
        if (tobsPassingCondition < condition.m_multi) {
          m_passRaw = kFALSE; // A condition was not satisfied :( all must be satisfied. We cannot accept this event.
          break;
        }
      }
    }

    //Info("RatesChainItem::beginEvent","%s applying logic to %i TOBs (passed - %i) MET is %f TE is %f",
    // getName().c_str(), eventTOBs->TOBs().size(), (Int_t)m_passRaw, eventTOBs->MET(), eventTOBs->TE());
  }

  /**
   * Used in Upgrade Rates mode - we plot the eta distribution of the thresholds we pass and the multiplicity
   */
  void RatesChainItem::fillHistograms(DataStore& dataStore, Float_t weight, Float_t bunchWeight) {
    for (const Float_t value : m_bufferJetRoIEta) dataStore.store(kVarJetEta, value, weight);
    for (const Float_t value : m_bufferMuRoIEta) dataStore.store(kVarMuEta, value, weight);
    for (const Float_t value : m_bufferEmRoIEta) dataStore.store(kVarEmEta, value, weight);
    for (const Float_t value : m_bufferTauRoIEta) dataStore.store(kVarTauEta, value, weight);

    if (m_bufferJetRoIEta.size() > 0) dataStore.store(kVarJetNThresh, m_bufferJetRoIEta.size(), weight);
    if (m_bufferMuRoIEta.size() > 0) dataStore.store(kVarMuNThresh, m_bufferMuRoIEta.size(), weight);
    if (m_bufferEmRoIEta.size() > 0) dataStore.store(kVarEmNThresh, m_bufferEmRoIEta.size(), weight);
    if (m_bufferTauRoIEta.size() > 0) dataStore.store(kVarTauNThresh, m_bufferTauRoIEta.size(), weight);

    dataStore.store(kVarMu, m_bufferMu, weight);
    dataStore.store(kVarBunchWeight, bunchWeight, weight); // What part of the extrapolation was explicitly due to
                                                              // change in number of bunches
  }

  /**
   * Reset all flags to zero
   */
  void RatesChainItem::endEvent() {
    m_passRaw = kFALSE;
    m_passPS = kFALSE;
    m_inEvent = kFALSE;

    m_bufferJetRoIEta.clear();
    m_bufferMuRoIEta.clear();
    m_bufferEmRoIEta.clear();
    m_bufferTauRoIEta.clear();
  }

  /**
   * Update the random prescale to a new value
   */
  void RatesChainItem::newRandomPS() {
    if (m_PS <= 0.) {
      m_passPS = kFALSE;
    } else {
      m_passPS = (m_R.Rndm() < m_PSWeight);
    }
  }

  /**
   * @return If this chain item was executed this event, regardless of whether or not it passed PS or passed Raw.
   * For L1 items, this means we need to check the bunch group
   */
  Bool_t RatesChainItem::getInEvent() {
    if (m_level == 1) {
      // If this L1 item passed then it must have been in the event, we only store L1s which pass so return this.
      // Also, if doEBWeighting is false we do not know what the bunch group was online
      if (m_inEvent == kTRUE || m_doEBWeighting == kFALSE) return m_inEvent;

      // Otherwise we check the bunchgroup
      return(Config::config().getInt(kCurrentEventBunchGroupID) == m_bunchGroupType);
    } else { // HLT
      return m_inEvent;
    }
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
   * @return 1/Prescale weighting factor for this event. This is scaled by an optional user supplied extra efficiency
   *factor which can modulate the rate
   */
  Double_t RatesChainItem::getPSWeight(Bool_t includeExpress) {
    if (m_proxy != nullptr) return m_proxy->getLastWeight();

    if (includeExpress == kTRUE) return m_PSWeight * m_PSExpressWeight * m_extraEfficiency;

    return m_PSWeight * m_extraEfficiency;
  }

  /**
   * The reduced prescacle is the chain prescale divided by the lowest prescale of any chain within
   * the same coherent prescale group.
   * @return 1/PrescaleReduced weighting factor for this event. This is scaled by an optional user supplied extra
   *efficiency factor which can modulate the rate
   */
  Double_t RatesChainItem::getPSReducedWeight(Bool_t includeExpress) {
    if (includeExpress == kTRUE) return m_PSReducedWeight * m_PSExpressWeight * m_extraEfficiency;

    return m_PSReducedWeight * m_extraEfficiency;
  }

  /**
   * @return Zero if this chain did not pass raw, else returns 1/Prescale
   */
  Double_t RatesChainItem::getPassRawOverPS(Bool_t includeExpress) {
    if (getPassRaw() == kFALSE) return 0.;

    return getPSWeight(includeExpress);
  }

  /**
   * The reduced prescacle is the chain prescale divided by the lowest prescale of any chain within
   * the same coherent prescale group.
   * @return Zero if this chain did not pass raw, else returns 1/PrescaleReduced
   */
  Double_t RatesChainItem::getPassRawOverPSReduced(Bool_t includeExpress) {
    if (getPassRaw() == kFALSE) return 0.;

    return getPSReducedWeight(includeExpress);
  }

  /**
   * @return If this chain passed both Raw and PS
   */
  Bool_t RatesChainItem::getPassRawAndPS() {
    return(getPassRaw() && getPassPS());
  }

  /**
   * @param tl Use a TriggerLogic to generate the pass/fail for this chain
   * Note this is required to use void beginEvent(TOBAccumulator* eventTOBs, CounterBaseRatesSet_t& counterSet)
   * RatesChainItem object does not own the trigger logic.
   */
  void RatesChainItem::setTriggerLogic(TriggerLogic* tl) {
    m_triggerLogic = tl;
  }

  /**
   * @return Pointer to any registered TriggerLogic item
   */
  TriggerLogic* RatesChainItem::getTriggerLogic() {
    return m_triggerLogic;
  }
} // namespace TrigCostRootAnalysis
