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

// Local include(s):
#include "MonitorBase.h"
#include "WriteXML_JSON.h"

// ROOT include(s):
#include <TH1.h>
#include <TCanvas.h>
#include <TError.h>


namespace TrigCostRootAnalysis {

  enum class UpgradeScenario_t {UPPER,MIDDLE,LOWER}; 

  class CounterBaseRates;
  class CounterRatesUnion;

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

   private:

    void parseUpgradeXML();
    void populateCounterMap(CounterMap_t* _counterMap);
    void populateChainItemMaps();
    void saveRateGraphs();
    void saveRuleBookXML();
    void saveXMLElement(std::ofstream& _fout, XMLExport& _xml, CounterBaseRates* _counter);

    void createGlobalCounters(CounterMap_t* _counterMap);
    void createL1Counters(CounterMap_t* _counterMap);
    void createL2Counters(CounterMap_t* _counterMap);
    void createL3Counters(CounterMap_t* _counterMap);

    struct ChainInfo {
      ChainInfo() = default;
      ChainInfo(std::string _name, Int_t _level, std::string _proxy, std::string _group, std::string _comment, Double_t _weight0, Double_t _weight1) : 
        m_name(_name),
        m_l1name(_name),
        m_level(_level),
        m_proxy(_proxy),
        m_lower(),
        m_group(_group),
        m_comment(_comment),
        m_weight0(_weight0),
        m_weight1(_weight1) {
          m_l1name.replace(0, 2, "L1");
        }; // For L0/L1
      ChainInfo(std::string _name, Int_t _level, std::string _proxy, std::string _lower, std::string _comment, Double_t _weight) : 
        m_name(_name),
        m_l1name(),
        m_level(_level),
        m_proxy(_proxy),
        m_lower(_lower),
        m_group(),
        m_comment(_comment),
        m_weight0(_weight),
        m_weight1(0) {}; // For HLT
      bool operator< (const ChainInfo &rhs) const { if (m_level != rhs.m_level) return m_level < rhs.m_level; return hash_fn(m_name) < hash_fn(rhs.m_name); }
      std::hash<std::string> hash_fn;
      std::string m_name;
      std::string m_l1name;
      Int_t m_level;
      std::string m_proxy;
      std::string m_lower;
      std::string m_group;
      std::string m_comment;
      Double_t m_weight0;
      Double_t m_weight1;
    };

    //Bool_t checkPatternUnique( const std::string& _patternName );
    //Bool_t checkPatternOverlap( const std::string& _patternName );

    std::string m_scenario;

    //std::map<std::string, ChainInfo> m_newToCurrent; // obs

    std::set<ChainInfo> m_upgradeChains;

    Float_t m_unbiasedWeight;
    Int_t m_unbiasedWeightRound;
    Float_t m_targetPairedBCID;
    Float_t m_targetPeakMuAv;

    CounterRatesUnion* m_globalRateL3Counter;
    CounterRatesUnion* m_globalRateL1Counter;
    CounterRatesUnion* m_globalRateL2Counter;

    ChainItemMap_t m_chainItemsL1;
    ChainItemMap_t m_chainItemsL2;
    ChainItemMap_t m_chainItemsL3;

    
  }; //class MonitorRatesUpgrade
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_MonitorRatesUpgrade_H
