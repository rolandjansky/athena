// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_MonitorRatesUpgrade_H
#define TrigCostRootAnalysis_MonitorRatesUpgrade_H

// STL include(s):
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include <bitset>

// Local include(s):
#include "MonitorBase.h"
#include "WriteXML_JSON.h"

// ROOT include(s):
#include <TH1.h>
#include <TCanvas.h>
#include <TError.h>
#include <TMath.h>
#include <TRandom3.h>

namespace TrigCostRootAnalysis {
  enum class UpgradeScenario_t {
    UPPER, MIDDLE, LOWER
  };

  class CounterBaseRates;
  class CounterRatesUnion;

  /**
   * @struct TOB
   * Trigger object. Paramaterises the energy, location and details for a single L1 RoI in the
   * more descript Run-II EDM. Variables have different underlying meaning based on the RoI type.
   */
  struct TOB {
    ConfKey_t m_type; //!< RoI type specifier
    Float_t m_eta; //!< Eta coordinate of RoI
    Float_t m_phi; //!< Phi coordidate of RoI
    mutable Float_t m_et; //!< Energy/pT of RoI in counts
    mutable Float_t m_etLarge; //!< For jet RoIs, second energy type
    Int_t m_iso; //!< Isolation bits of RoI (5 bits, menu dependent)
    Int_t m_id; //!< Numeric ID
  };
  inline Bool_t operator < (const TOB& lhs, const TOB& rhs) {return lhs.m_id < rhs.m_id;}

  /**
   * @class TOBAccumulator
   * Holds multiple TOBs corresponding to a real event or a simulated high-pileup event with multiple event's worth of
   *TOB overlay
   * MET(TE) are re-calculated based on the vector(scalar) sums of energy TOBs (one per 'event').
   * TODO: Overlap removal?
   */
  class TOBAccumulator {
    std::set<TOB> m_TOBs; //!< List of TOBs in pseudo-event
    Float_t m_vX; //!< Vector energy in x axis
    Float_t m_vY; //!< Vector energy in y axis
    Float_t m_TE; //!< Scalar total energy in GeV
    Float_t m_vHX; //!< Vector HT in x
    Float_t m_vHY; //!< Vector HT in y
    Float_t m_HT; // Scalar HT
    Bool_t m_overflowMET;
    Bool_t m_overflowTE;
    Float_t m_mu;
  public:
    TOBAccumulator() : m_vX(0), m_vY(0), m_TE(0), m_vHX(0), m_vHY(0), m_HT(0), m_overflowMET(kFALSE), m_overflowTE(
        kFALSE), m_mu(0.) {}
    Float_t TE() {return m_TE;}
    Float_t vX() {return m_vX;}
    Float_t vY() {return m_vY;}
    Float_t HT() {return m_HT;}
    Float_t vHX() {return m_vHX;}
    Float_t vHY() {return m_vHY;}
    Bool_t METOverflow() {return m_overflowMET;}
    Bool_t TEOverflow() {return m_overflowTE;}
    Float_t MET() {return TMath::Sqrt((m_vX * m_vX) + (m_vY * m_vY));}
    Float_t MHT() {return TMath::Sqrt((m_vHX * m_vHX) + (m_vHY * m_vHY));}
    Float_t mu() {return m_mu;}
    void set(Float_t mu, Float_t vX, Float_t vY, Float_t TE, Bool_t ofX, Bool_t ofY, Bool_t ofTE) {
      m_mu = mu;
      m_vX = vX;
      m_vY = vY;
      m_TE = TE;
      m_overflowMET = (ofX | ofY);
      m_overflowTE = ofTE;
    }

    std::set<TOB>& TOBs() {return m_TOBs;}
    void add(const TOB tob) {
      static bool merge = Config::config().getInt(kUpgradeMergeTOBOverlap);

      if (merge) {
        checkMerge(tob);
      } else {
        m_TOBs.insert(tob);
      }
      addToMHT(tob); // merged or not - still count me in the HT if I'm a jet
    }

    void checkMerge(const TOB& tob) {
      bool canMerge = false, merged = false;
      Float_t minDR = 0.4;

      if (tob.m_type == kEmString || tob.m_type == kTauString || tob.m_type == kJetString) {
        canMerge = true;
        if (tob.m_type == kJetString) minDR = 0.6;
      }

      if (canMerge) {
        for (auto& myTob : m_TOBs) {
          if (tob.m_type != myTob.m_type) continue;
          if (tob.m_iso != myTob.m_iso) continue;
          if (deltaR(tob.m_phi, myTob.m_phi, tob.m_eta, myTob.m_eta) > minDR) continue;

          // Same type, and close - merge
          // I solemnly swear that I won't touch the weak ordering
          //TOB* nonConstTOB = const_cast<TOB*>(&myTob);
          myTob.m_et += tob.m_et;
          myTob.m_etLarge += tob.m_etLarge;
          merged = true;
          //if (Config::config().getDisplayMsg(kMsgTOBMerge) == kTRUE) Info("TOBAccumulator::checkMerge", "Merged %s
          // TOBs", Config::config().getStr(tob.m_type).c_str());
          break;
        }
      }

      if (!merged) m_TOBs.insert(tob);
    }

    std::string print() {
      std::stringstream ss;
      for (const auto& tob : TOBs()) {
        std::bitset<5> iso = tob.m_iso;
        ss << "-- " << Config::config().getName(tob.m_type) << "\tET:" << tob.m_et
            << "\tiso:" << iso.to_string() << "\teta:" << tob.m_eta << " \tphi:" << tob.m_phi;
        if (tob.m_type == kJetString) ss << "\tETLarge:" << tob.m_etLarge;
        ss << std::endl;
      }
      return ss.str();
    }

    /**
     * Add another accumulator's TOBs and MET
     **/
    void add(const TOBAccumulator* b) {
      for (auto& tob : b->m_TOBs) add(tob);
      m_mu += b->m_mu;
      m_vX += b->m_vX;
      m_vY += b->m_vY;
      m_TE += b->m_TE;
      m_overflowTE |= b->m_overflowTE;
      m_overflowMET |= b->m_overflowMET;
    }

    void addToMHT(const TOB& tob) {
      static Bool_t largeJetWindow = Config::config().getInt(kUpgradeJetLargeWindow);

      if (tob.m_type != kJetString) return;

      if (TMath::Abs(tob.m_eta) * 10 > 31) return; // eta too high

      Float_t et = tob.m_et;
      if (largeJetWindow == kTRUE) et = tob.m_etLarge;
      else Warning("TOBAccumulator::addMHT", "Using small jet window size - really?");
      m_vHX += et * TMath::Cos(tob.m_phi);
      m_vHY += et * TMath::Sin(tob.m_phi);
      m_HT += et;
    }
  };

  /**
   * @struct TriggerCondition
   * Parameterization of a set of cuts to be applied to one or more TOBs in a TOBAccumulator
   */
  struct TriggerCondition {
    ConfKey_t m_type; //!< Type of TOB to apply this condition to
    UInt_t m_multi; //!< Multiplicity of TOBs required to pass selection
    Int_t m_iso; //!< Isolation bits to require TOB to pass
    UInt_t m_min; //!< Minimum eta to consider
    UInt_t m_max; //!< Maximum eta to consider
    UInt_t m_thresh; //!< Energy threshold in GeV
  };

  /**
   * @class TriggerLogic
   * Holds multiple TOBs corresponding to a real event or a simulated high-pileup event with multiple event's worth of
   *TOB overlay
   * MET(TE) are re-calculated based on the vector(scalar) sums of energy TOBs (one per 'event').
   * TODO: Overlap removal?
   */
  class TriggerLogic {
    std::vector<TriggerCondition> m_conditions; //!< Set of TriggerConditions. All of these must pass
  public:
    TriggerLogic() {}
    void addCondition(std::string name, UInt_t multi, UInt_t thresh, Int_t iso, UInt_t min, UInt_t max) {
      TriggerCondition c;

      if (name == "MU") c.m_type = kMuonString;
      else if (name == "EM") c.m_type = kEmString;
      else if (name == "TAU") c.m_type = kTauString;
      else if (name == "JET" || name == "J") c.m_type = kJetString;
      else if (name == "XE") c.m_type = kMissingEnergyString;
      else if (name == "TE") c.m_type = kEnergyString;
      else if (name == "MHT") c.m_type = kMHTString;
      else if (name == "HT") c.m_type = kHTString;
      else Error("TriggerLogic::addAND", "Unknown TriggerCondition name %s", name.c_str());
      c.m_multi = multi;
      c.m_iso = iso;
      c.m_thresh = thresh;
      c.m_min = min;
      c.m_max = max;
      m_conditions.push_back(c);
    }

    std::string print() {
      std::stringstream ss;
      for (const auto tc : m_conditions) {
        ss << "[" << Config::config().getName(tc.m_type) << " N:" << tc.m_multi << " E>=" << tc.m_thresh <<
        " ISO:" << tc.m_iso
            << " etaMin:" << tc.m_min << " etaMax:" << tc.m_max << "]";
      }
      return ss.str();
    }

    const std::vector<TriggerCondition>& conditions() {return m_conditions;}
  };

  /**
   * @struct ChainInfo
   * Holds all the information
   */
  struct ChainInfo {
    ChainInfo() = default;
    ChainInfo(std::string name) : m_name(name) {}
    ChainInfo(std::string name, Int_t level, TriggerLogic logic, std::string group, std::string comment,
              Double_t weight0, Double_t weight1) :
      m_name(name),
      m_l2name(name),
      m_triggerLogic(logic),
      m_level(level),
      m_lower(),
      m_group(group),
      m_comment(comment),
      m_weight0(weight0),
      m_weight1(weight1) {
      m_l2name.replace(0, 2, "L2");
      m_ID = m_instances++;
    }; // For L1/L2
    ChainInfo(std::string name, Int_t level, std::string proxy, std::string lower, std::string comment,
              Double_t weight) :
      m_name(name),
      m_l2name(),
      m_triggerLogic(),
      m_level(level),
      m_proxy(proxy),
      m_lower(lower),
      m_group(),
      m_comment(comment),
      m_weight0(weight),
      m_weight1(0) {
      m_ID = m_instances++;
    }; // For HLT
    bool operator < (const ChainInfo& rhs) const {
      if (m_level != rhs.m_level) return m_level < rhs.m_level;

      return hash_fn(m_name) < hash_fn(rhs.m_name);
    }

    static Int_t m_instances;
    std::hash<std::string> hash_fn; //!< C++11 awesomeness, easy hashes
    std::string m_name; //!< My name (L1 or HLT)
    std::string m_l2name; //!< My name with L1 -> L2
    TriggerLogic m_triggerLogic; //!< My L1 & L2 trigger logic
    Int_t m_level; //!< 0 (means 0 and 1), or 2
    Int_t m_ID; //!< Generated CTPID/ChainCounter
    std::string m_proxy; //!< HLT - what HLT chain am I proxying off of?
    std::string m_lower; //!< Lower chain name
    std::string m_group; //!< Group name, for group rates
    std::string m_comment; //!< Free text
    Double_t m_weight0; //!< Extra efficiency weighting factor for L1 or HLT
    Double_t m_weight1; //!< Extra efficiency weighting factor for L2
  };

  /**
   * @class MonitorRatesUpgrade
   */
  class MonitorRatesUpgrade: public MonitorBase {
  public:
    MonitorRatesUpgrade(const TrigCostData* costData);
    ~MonitorRatesUpgrade();
    void newEvent(Float_t weight = 1.);
    CounterBase* newCounter(const std::string& name, Int_t ID);
    Bool_t getIfActive(ConfKey_t mode);
    void saveOutput();
    Float_t getCollidingBunchFactor() {return m_collidingBunchFactor;}
  private:
    void parseUpgradeXML();
    void populateCounterMap(CounterMap_t* counterMap);
    void populateChainItemMaps();
    void saveRateGraphs();

    void createGlobalCounters(CounterMap_t* counterMap);
    void createL1Counters(CounterMap_t* counterMap);
    void createL2Counters(CounterMap_t* counterMap);
    void createL3Counters(CounterMap_t* counterMap);

    TOBAccumulator* getEventTOBs();
    void validateTriggerEmulation(CounterMap_t* counterMap, TOBAccumulator* thisAccum, Bool_t print);
    void printEnergyTOBs();

    std::string m_scenario; //<! What scenario XML to load
    Bool_t m_upgradePileupScaling; //<! If we are doing two passes to do pileup scaling
    Bool_t m_upgradeBunchScaling; //<! If we are linearly scaling for more bunches
    Bool_t m_doDeadtimeScaling; //!< If we are scaling for online deadtime

    Int_t m_targetPairedBunches; //!< How many colliding bunches we're scaling to
    Float_t m_collidingBunchFactor; //!< targetPairedBCID / EBPairedBCID, what fraction of bunches w.r.t. the EB data?
    Float_t m_pileupFactor; //<! targetPeakMuAv / EBPeakMuAv, how much more pileup do we want?
    Float_t m_collisionsToMix; //!< How many collisions we want to mix into this event
    Float_t m_upgradeDeadtimeScaling; //<! Scaling due to deadtime

    TRandom3 m_R3;

    CounterRatesUnion* m_globalRateL3Counter;
    CounterRatesUnion* m_globalRateL1Counter;
    CounterRatesUnion* m_globalRateL2Counter;

    ChainItemMap_t m_chainItemsL1;
    ChainItemMap_t m_chainItemsL2;
    ChainItemMap_t m_chainItemsL3;

    Float_t m_statsEventMu;
    Float_t m_statsEventMu2;
    Int_t m_statsEventMuN;

    TOBAccumulator* m_thisEventPtr;

    std::multiset<ChainInfo> m_upgradeChains; //<! Holds the set of chains to construct as read in from XML
    StringIntMap_t m_isoBits; //!< Holds isolation bit pattersn from the XML and assigns them names
    std::vector<TOBAccumulator*> m_pileupDatabase; //<! Holds a collection of unbiased event's TOBs to latter be mixed
                                                   // with other events
  }; //class MonitorRatesUpgrade
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorRatesUpgrade_H
