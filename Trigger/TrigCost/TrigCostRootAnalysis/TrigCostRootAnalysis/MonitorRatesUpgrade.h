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

  enum class UpgradeScenario_t {UPPER,MIDDLE,LOWER}; 

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
  inline Bool_t operator<(const TOB& lhs, const TOB& rhs) { return lhs.m_id < rhs.m_id; }

  /**
   * @class TOBAccumulator
   * Holds multiple TOBs corresponding to a real event or a simulated high-pileup event with multiple event's worth of TOB overlay
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
    TOBAccumulator() : m_vX(0), m_vY(0), m_TE(0), m_vHX(0), m_vHY(0), m_HT(0), m_overflowMET(kFALSE), m_overflowTE(kFALSE), m_mu(0.) {}
    Float_t TE() { return m_TE; }
    Float_t vX() { return m_vX; }
    Float_t vY() { return m_vY; }
    Float_t HT() { return m_HT; }
    Float_t vHX() { return m_vHX; }
    Float_t vHY() { return m_vHY; }
    Bool_t METOverflow() { return m_overflowMET; }
    Bool_t TEOverflow() { return m_overflowTE; }
    Float_t MET() { return TMath::Sqrt((m_vX  * m_vX)  + (m_vY  * m_vY)); }
    Float_t MHT() { return TMath::Sqrt((m_vHX * m_vHX) + (m_vHY * m_vHY)); }
    Float_t mu() { return m_mu; }
    void set(Float_t _mu, Float_t _vX, Float_t _vY, Float_t _TE, Bool_t _ofX, Bool_t _ofY, Bool_t _ofTE) {
      m_mu = _mu; 
      m_vX = _vX; m_vY = _vY; m_TE = _TE; 
      m_overflowMET = (_ofX | _ofY);
      m_overflowTE = _ofTE;
    }
    std::set<TOB>& TOBs() { return m_TOBs; } 
    void add(const TOB _tob) {  
      static bool _merge = Config::config().getInt(kUpgradeMergeTOBOverlap);
      if (_merge) {
        checkMerge( _tob );
      } else { 
        m_TOBs.insert( _tob );
      }
      addToMHT(_tob); // merged or not - still count me in the HT if I'm a jet
    }
    void checkMerge(const TOB& _tob) {
      bool _canMerge = false, _merged = false;
      Float_t _minDR = 0.4;
      if (_tob.m_type == kEmString || _tob.m_type == kTauString || _tob.m_type == kJetString) {
        _canMerge = true; 
        if (_tob.m_type == kJetString) _minDR = 0.6;
      }

      if (_canMerge) {
        for (auto& _myTob : m_TOBs) {
          if (_tob.m_type != _myTob.m_type) continue;
          if (_tob.m_iso  != _myTob.m_iso) continue;
          if (deltaR(_tob.m_phi, _myTob.m_phi, _tob.m_eta, _myTob.m_eta) > _minDR) continue;

          // Same type, and close - merge
          // I solemnly swear that I won't touch the weak ordering
          //TOB* _nonConstTOB = const_cast<TOB*>(&_myTob);
          _myTob.m_et += _tob.m_et;
          _myTob.m_etLarge += _tob.m_etLarge;
          _merged = true;
          //if (Config::config().getDisplayMsg(kMsgTOBMerge) == kTRUE) Info("TOBAccumulator::checkMerge", "Merged %s TOBs", Config::config().getStr(_tob.m_type).c_str());
          break;
        }
      }

      if (!_merged) m_TOBs.insert( _tob );
    }
    std::string print() {
      std::stringstream _ss;
      for (const auto& _tob : TOBs()) {
        std::bitset<5> _iso = _tob.m_iso;
        _ss << "-- " << Config::config().getName(_tob.m_type) << "\tET:" << _tob.m_et 
          << "\tiso:" << _iso.to_string() << "\teta:" << _tob.m_eta << " \tphi:" << _tob.m_phi;
        if (_tob.m_type == kJetString) _ss << "\tETLarge:" << _tob.m_etLarge;
        _ss << std::endl;
      }
      return _ss.str();
    }
    /**
     * Add another accumulator's TOBs and MET
     **/
    void add(const TOBAccumulator* _b) {
      for (auto& _tob : _b->m_TOBs) add(_tob);
      m_mu += _b->m_mu;
      m_vX += _b->m_vX;
      m_vY += _b->m_vY;
      m_TE += _b->m_TE;
      m_overflowTE |= _b->m_overflowTE;
      m_overflowMET |= _b->m_overflowMET;
    }
    void addToMHT(const TOB& _tob) {
      static Bool_t _largeJetWindow = Config::config().getInt(kUpgradeJetLargeWindow);
      if (_tob.m_type != kJetString) return;
      if (TMath::Abs(_tob.m_eta) * 10 > 31) return; // eta too high
      Float_t _et = _tob.m_et;
      if (_largeJetWindow == kTRUE) _et = _tob.m_etLarge;
      else Warning("TOBAccumulator::addMHT","Using small jet window size - really?");
      m_vHX += _et * TMath::Cos( _tob.m_phi );
      m_vHY += _et * TMath::Sin( _tob.m_phi );
      m_HT += _et;
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
   * Holds multiple TOBs corresponding to a real event or a simulated high-pileup event with multiple event's worth of TOB overlay
   * MET(TE) are re-calculated based on the vector(scalar) sums of energy TOBs (one per 'event').
   * TODO: Overlap removal?
   */
  class TriggerLogic {
    std::vector<TriggerCondition> m_conditions; //!< Set of TriggerConditions. All of these must pass
   public:
    TriggerLogic() {}
    void addCondition(std::string _name, UInt_t _multi, UInt_t _thresh, Int_t _iso, UInt_t _min, UInt_t _max) {
      TriggerCondition _c;
      if      (_name == "MU")  _c.m_type = kMuonString; 
      else if (_name == "EM")  _c.m_type = kEmString; 
      else if (_name == "TAU") _c.m_type = kTauString;
      else if (_name == "JET" || _name == "J") _c.m_type = kJetString;
      else if (_name == "XE")  _c.m_type = kMissingEnergyString;
      else if (_name == "TE")  _c.m_type = kEnergyString;
      else if (_name == "MHT")  _c.m_type = kMHTString;
      else if (_name == "HT")  _c.m_type = kHTString;
      else Error("TriggerLogic::addAND", "Unknown TriggerCondition name %s", _name.c_str());
      _c.m_multi = _multi;
      _c.m_iso = _iso;
      _c.m_thresh = _thresh;
      _c.m_min = _min;
      _c.m_max = _max;
      m_conditions.push_back(_c);
    }
    std::string print() {
      std::stringstream _ss;
      for(const auto _tc : m_conditions) {
        _ss << "[" << Config::config().getName(_tc.m_type) << " N:" << _tc.m_multi << " E>=" << _tc.m_thresh << " ISO:" << _tc.m_iso
          << " etaMin:" << _tc.m_min << " etaMax:" << _tc.m_max << "]";
      }
      return _ss.str();
    } 
    const std::vector<TriggerCondition>& conditions() { return m_conditions; }
  };

  /**
   * @struct ChainInfo
   * Holds all the information 
   */
  struct ChainInfo {
    ChainInfo() = default;
    ChainInfo(std::string _name) : m_name(_name) {}
    ChainInfo(std::string _name, Int_t _level, TriggerLogic _logic, std::string _group, std::string _comment, Double_t _weight0, Double_t _weight1) : 
      m_name(_name),
      m_l2name(_name),
      m_triggerLogic(_logic),
      m_level(_level),
      m_lower(),
      m_group(_group),
      m_comment(_comment),
      m_weight0(_weight0),
      m_weight1(_weight1) {
        m_l2name.replace(0, 2, "L2");
        m_ID = m_instances++;
      }; // For L1/L2
    ChainInfo(std::string _name, Int_t _level, std::string _proxy, std::string _lower, std::string _comment, Double_t _weight) : 
      m_name(_name),
      m_l2name(),
      m_triggerLogic(),
      m_level(_level),
      m_proxy(_proxy),
      m_lower(_lower),
      m_group(),
      m_comment(_comment),
      m_weight0(_weight),
      m_weight1(0) {
        m_ID = m_instances++;
      }; // For HLT
    bool operator< (const ChainInfo &rhs) const { if (m_level != rhs.m_level) return m_level < rhs.m_level; return hash_fn(m_name) < hash_fn(rhs.m_name); }
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
  class MonitorRatesUpgrade : public MonitorBase {
  
   public:
   
    MonitorRatesUpgrade(const TrigCostData* _costData);
    ~MonitorRatesUpgrade();
    void newEvent(Float_t _weight = 1.);
    CounterBase* newCounter( const std::string &_name, Int_t _ID );
    Bool_t getIfActive(ConfKey_t _mode);
    void saveOutput();
    Float_t getCollidingBunchFactor() { return m_collidingBunchFactor; }

   private:

    void parseUpgradeXML();
    void populateCounterMap(CounterMap_t* _counterMap);
    void populateChainItemMaps();
    void saveRateGraphs();

    void createGlobalCounters(CounterMap_t* _counterMap);
    void createL1Counters(CounterMap_t* _counterMap);
    void createL2Counters(CounterMap_t* _counterMap);
    void createL3Counters(CounterMap_t* _counterMap);

    TOBAccumulator* getEventTOBs();
    void validateTriggerEmulation(CounterMap_t* _counterMap, TOBAccumulator* _this, Bool_t _print);
    void printEnergyTOBs();

    std::string m_scenario; //<! What scenario XML to load
    Bool_t m_upgradePileupScaling; //<! If we are doing two passes to do pileup scaling
    Bool_t m_upgradeBunchScaling; //<! If we are linearly scaling for more bunches
    Bool_t m_doDeadtimeScaling; //!< If we are scaling for online deadtime 
    
    Int_t   m_targetPairedBunches; //!< How many colliding bunches we're scaling to
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
    Int_t   m_statsEventMuN;

    TOBAccumulator* m_thisEventPtr;

    std::multiset<ChainInfo> m_upgradeChains; //<! Holds the set of chains to construct as read in from XML
    StringIntMap_t m_isoBits; //!< Holds isolation bit pattersn from the XML and assigns them names
    std::vector<TOBAccumulator*> m_pileupDatabase; //<! Holds a collection of unbiased event's TOBs to latter be mixed with other events
    
  }; //class MonitorRatesUpgrade
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorRatesUpgrade_H
