// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <map>
#include <utility> //std::pair
#include <iostream>
#include <vector>
#include <stdio.h> // command exec
#include <bitset> // command exec

// ROOT include(s):
#include <TSystem.h>
#include <TXMLEngine.h>
#include <TObjString.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLDocument.h>
#include <TXMLAttr.h>
#include <TError.h>
#include <TMath.h>
#include <TFile.h>

// Local include(s):
#include "../TrigCostRootAnalysis/MonitorRatesUpgrade.h"
#include "../TrigCostRootAnalysis/CounterRatesChain.h"
#include "../TrigCostRootAnalysis/CounterRatesUnion.h"
#include "../TrigCostRootAnalysis/CounterRatesIntersection.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"
#include "../TrigCostRootAnalysis/RatesChainItem.h"
#include "../TrigCostRootAnalysis/WriteXML_JSON.h"

// ATHENA Includes
#ifndef ROOTCORE
#include "PathResolver/PathResolver.h"
#endif // not ROOTCORE

namespace TrigCostRootAnalysis {

  Int_t ChainInfo::m_instances = 0; // Static

  /**
   * Monitor constructor. Sets name and calls base constructor.
   * This monitor will be used to emulate the future trigger setup
   * It will simulate L1, L2 and HLT rates using Run2 EB data as input
   * It will apply efficiencies to simulate expected rates yeilds
   */
  MonitorRatesUpgrade::MonitorRatesUpgrade(const TrigCostData* _costData)
    : MonitorBase(_costData, "RateUpgrade"),
      m_R3(0)
  {
    m_dummyCounter = static_cast<CounterBase*>(new CounterRatesChain(_costData, Config::config().getStr(kDummyString), INT_MIN));
    m_globalRateL1Counter = nullptr;
    m_globalRateL2Counter = nullptr;
    m_globalRateL3Counter = nullptr;
    m_scenario = Config::config().getStr(kUpgradeScenario);
    m_upgradePileupScaling = !Config::config().getInt(kNoUpgradePileupScaling);
    m_upgradeBunchScaling = !Config::config().getInt(kNoUpgradeBunchScaling);
    m_upgradeDeadtimeScaling = 1.;

    m_collidingBunchFactor = 1.;
    m_pileupFactor = 1.;
    m_eventsToMix = 0;
  }

  /**
   * Destroy this monitor - note we have special RatesChainItems to destroy here too
   */
  MonitorRatesUpgrade::~MonitorRatesUpgrade() {
    for (auto _item : m_chainItemsL1) delete _item.second;
    for (auto _item : m_chainItemsL2) delete _item.second;
    for (auto _item : m_chainItemsL3) delete _item.second;
  }

  /**
   * Here we create a RatesChainItem for each trigger item at each level, it is these which will be loaded
   * per event with the raw decision of that trigger item and the counters will use the information of the L1
   * and HLT chain items given to them to calculate their event weight.
   */
  void MonitorRatesUpgrade::populateChainItemMaps() {

    TrigXMLService::trigXMLService().parseUpgradeXML(m_isoBits, m_upgradeChains);

    Info("MonitorRatesUpgrade::populateChainItemMaps", "*** CALCULATING EXTRAPOLATION SPECIFICS FOR UPGRADE RATES MONITOR ***");

    // We scale up unbiased events due to pileup. But by how much?
    if (m_upgradePileupScaling == kTRUE) {
      m_pileupFactor = Config::config().getFloat(kTargetPeakMuAverage) / (Float_t) Config::config().getFloat(kOnlinePeakMuAverage); // Should have been set by run XML
      Info("MonitorRatesUpgrade::populateChainItemMaps","We will mix in to each event %.2fx extra unbiased event's TOBs (Pileup %.2f->%.2f = x%.2f)",
        m_pileupFactor-1, Config::config().getFloat(kOnlinePeakMuAverage), Config::config().getFloat(kTargetPeakMuAverage), m_pileupFactor);
    } else {
      Warning("MonitorRatesUpgrade::populateChainItemMaps","Upgrade rates will not be scaled for extra pileup.");
    }

    if (m_upgradeBunchScaling == kTRUE) {
      m_collidingBunchFactor = Config::config().getInt(kTargetPairedBunches) / (Float_t) TrigXMLService::trigXMLService().getBunchGroupSize(1); // Going to continue to assume that 1 is PAIRED
      Info("MonitorRatesUpgrade::populateChainItemMaps","We will scale x%.2f for the number of bunches in the ring (%i->%i)",
        m_collidingBunchFactor, TrigXMLService::trigXMLService().getBunchGroupSize(1), Config::config().getInt(kTargetPairedBunches));
    } else {
      Warning("MonitorRatesUpgrade::populateChainItemMaps","Upgrade rates will not be scaled for extra bunches.");
    }

    if (Config::config().getIsSet(kOnlineDeadtime) == kTRUE && Config::config().getInt(kNoOnlineDeadtimeCorrection) == kFALSE) {
      m_upgradeDeadtimeScaling = 1. + Config::config().getFloat(kOnlineDeadtime);
      Info("MonitorRatesUpgrade::populateChainItemMaps","We will scale x%.2f for EB sample online deadtime", m_upgradeDeadtimeScaling);
    } else {
      Warning("MonitorRatesUpgrade::populateChainItemMaps","Upgrade rates will not be scaled for online EB deadtime.");
    }

    Float_t m_upgradePredictionL = Config::config().getFloat(kRunLumiXML) * m_collidingBunchFactor * m_pileupFactor * m_upgradeDeadtimeScaling;
    Info("MonitorRatesUpgrade::populateChainItemMaps", "*** THIS UPGRADE RATES PREDICTION IS PERFORMING A L EXTRAPOLATION OF %.2e -> %.2e ***",
      Config::config().getFloat(kRunLumiXML), m_upgradePredictionL);
    Config::config().setFloat(kPredictionLumiMenuXML, m_upgradePredictionL, "PredictionLumiMenuXML");

    for (std::multiset<ChainInfo>::iterator _it = m_upgradeChains.begin(); _it != m_upgradeChains.end(); ++_it) {
    //for (auto _item : m_upgradeChains) {
      if (_it->m_level == 2) {
        continue; // not doing HLT yet
      }
      const std::string _nameL1 = _it->m_name;
      Double_t _chainPrescale = 1.; //TODO - nonzero prescales?
      RatesChainItem* _L1 = new RatesChainItem(_nameL1, /*chainLevel=*/ 1, _chainPrescale);
      _L1->setRateReductionFactor( _it->m_weight0 );
      m_chainItemsL1[ _nameL1 ] = _L1;

      std::string _nameL2 = _it->m_l2name;
      RatesChainItem* _L2 = new RatesChainItem(_nameL2, /*chainLevel=*/ 2, 1.);
      m_chainItemsL2[ _nameL2 ] = _L2;
      _L2->setRateReductionFactor( _it->m_weight1 );

      _L2->addLower( _L1 );
      _L1->addUpper( _L2 );

      TriggerLogic* _tl = const_cast<TriggerLogic*>(&(_it->m_triggerLogic)); // Why are you const!?!

      _L1->setTriggerLogic( _tl );
      _L2->setTriggerLogic( _tl );
    }

    if (Config::config().getInt(kDoUpgradeRatesScan) == kTRUE) {
      // Scan over a set of energies
      for (UInt_t _e = 5; _e <= 500; _e += 5) {
        if      (_e > 100) { if (_e % 50 != 0) continue; }
        else if (_e > 50)  { if (_e % 20 != 0) continue; }
        else if (_e > 20)  { if (_e % 10 != 0) continue; } //reduce granularity
        for (UInt_t _t = 0; _t < 11; ++_t) {
          std::stringstream _name;
          std::stringstream _group;
          std::string _type, _isoStr;
          Int_t _iso = 0;
          Int_t _etaMin = 0, _etaMax = 49;
          switch (_t) {
            case 0: _type = "J"; _etaMax = 31; break;
            case 1: _type = "EM"; break;
            case 2: _type = "EM"; _isoStr= "I"; _iso = m_isoBits["EM_I"]; break;
            case 3: _type = "EM"; _isoStr= "VHI"; _iso = m_isoBits["EM_VHI"]; break;
            case 4: _type = "TAU"; break;
            case 5: _type = "TAU"; _isoStr= "IL"; _iso = m_isoBits["TAU_IL"]; break;
            case 6: _type = "TAU"; _isoStr= "IM"; _iso = m_isoBits["TAU_IM"]; break;
            case 7: _type = "TAU"; _isoStr= "IT"; _iso = m_isoBits["TAU_IT"]; break;
            case 8: _type = "MU"; break;
            case 9: _type = "XE"; break;
            case 10: _type = "TE"; break;
          }
          _name << "L1_" << _type << _e << _isoStr;
          _group << "RATE_Test_" << _type << _isoStr; 
          if (m_chainItemsL1.count( _name.str() ) == 1) continue; // Already has

          TriggerLogic _triggerLogic;
          _triggerLogic.addCondition(_type, 1, _e, _iso, _etaMin, _etaMax);
          auto _it = m_upgradeChains.insert( ChainInfo(_name.str(), 1, _triggerLogic, _group.str(), "", 1., 1.) );
          TriggerLogic* _tl = const_cast<TriggerLogic*>( &(_it->m_triggerLogic) ); //Why is this const in the first place?

          RatesChainItem* _L1 = new RatesChainItem(_name.str(), /*chainLevel=*/ 1, 1.);
          m_chainItemsL1[ _name.str() ] = _L1;
          _L1->setTriggerLogic( _tl );
        }
      }
    }

    for (const auto _chainItem : m_chainItemsL1)  _chainItem.second->classifyLumiAndRandom(); // should not be needed, do different lumi scaling here
    for (const auto _chainItem : m_chainItemsL2)  _chainItem.second->classifyLumiAndRandom();
  }

  /**
   * In every other monitor, we make a new counter the first time we encounter it in the menu.
   * Rates are different, as some of the rates we want to monitor are the combinations of items in the
   * menu. Hence we pre-populate the counterMap with all combinations that we are interested in for the run.
   *
   * We make one counter per chain (L1 and HLT), one per intersection of each chain and other chains in its group
   * and one for the union of all chains in each group.
   */
  void MonitorRatesUpgrade::populateCounterMap(CounterMap_t* _counterMap) {

    createGlobalCounters(_counterMap);
    createL1Counters(_counterMap);
    createL2Counters(_counterMap);
    createL3Counters(_counterMap);
  }

  void MonitorRatesUpgrade::createGlobalCounters(CounterMap_t* _counterMap) {
    // Crate the global rates counter, this will be the OR of everything HLT
    m_globalRateL3Counter = new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalHLTString), 0, 10, (MonitorBase*)this); // Mint new counter
    m_globalRateL3Counter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateL3Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateL3Counter->decorate(kDecPrescaleValOnlineL1, (Float_t)0);
    m_globalRateL3Counter->decorate(kDecType, "Union");
    m_globalRateL3Counter->setAdvancedLumiScaling(kFALSE);
    (*_counterMap)[Config::config().getStr(kRateGlobalHLTString)] = static_cast<CounterBase*>(m_globalRateL3Counter);

    // Crate the global L2 counter, this will be the OR of everything L2
    m_globalRateL2Counter = new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalL2String), 0, 10, (MonitorBase*)this); // Mint new counter
    m_globalRateL2Counter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateL2Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateL2Counter->decorate(kDecPrescaleValOnlineL1, (Float_t)0);
    m_globalRateL2Counter->decorate(kDecType, "Union");
    m_globalRateL2Counter->setAdvancedLumiScaling(kFALSE);;
    (*_counterMap)[Config::config().getStr(kRateGlobalL2String)] = static_cast<CounterBase*>(m_globalRateL2Counter);

    // Crate the global L1 counter, this will be the OR of everything L1
    m_globalRateL1Counter = new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalL1String), 0, 10, (MonitorBase*)this); // Mint new counter
    m_globalRateL1Counter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateL1Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateL1Counter->decorate(kDecPrescaleValOnlineL1, (Float_t)0);
    m_globalRateL1Counter->decorate(kDecType, "Union");
    m_globalRateL1Counter->setAdvancedLumiScaling(kFALSE);
    (*_counterMap)[Config::config().getStr(kRateGlobalL1String)] = static_cast<CounterBase*>(m_globalRateL1Counter);
  }

  /**
   * The starting point is to define what items we're interested at L1
   */
  void MonitorRatesUpgrade::createL1Counters(CounterMap_t* _counterMap) {
    for (const auto _item : m_upgradeChains) {
      if (_item.m_level != 1) continue; // not doing HLT here

      const std::string _chainName = _item.m_name;
      const UInt_t _chainID = _item.m_ID;

      //Info("MonitorRatesUpgrade::createL1Counters","Create counter: %s", _chainName.c_str() );

      // Find the ChainItem for this chain
      ChainItemMapIt_t _it = m_chainItemsL1.find( _chainName );
      if (_it == m_chainItemsL1.end()) {
        Warning("MonitorRatesUpgrade::createL1Counters","Cannot find L1 item: %s", _chainName.c_str() );
        continue;
      }
      RatesChainItem* _chainItemL1 = _it->second;

      const std::string _prescaleStr = Config::config().getStr(kL1String) + ":" + doubleToString( _chainItemL1->getPS(), 2 );
      const std::string _group = _item.m_group + std::string("_L1");

      
      CounterRatesChain* _L1Chain = new CounterRatesChain(m_costData, _chainName, _chainID, 10, (MonitorBase*)this); // Mint new counter
      _L1Chain->decorate(kDecRatesGroupName, _group );
      _L1Chain->decorate(kDecPrescaleStr, _prescaleStr);
      _L1Chain->decorate(kDecType, "L1");
      _L1Chain->setAdvancedLumiScaling(kFALSE);
      _L1Chain->addL1Item( _chainItemL1 ); // Link it to where it'll be getting its pass/fail info
      (*_counterMap)[_chainName] = static_cast<CounterBase*>(_L1Chain); // Insert into the counterMap
      Info("MonitorRatesUpgrade::createL1Counters","Made a L1 counter for: %s", _L1Chain->getName().c_str() );

      //Add to global L1 counter
      m_globalRateL1Counter->addL1Item( _chainItemL1 );

      // Do group
      CounterMapIt_t _findGroup = _counterMap->find( _group ); // Do we have a counter for this group?
      if ( _findGroup != _counterMap->end() ) { // We do have a group already! Add to it.
        (static_cast<CounterRatesUnion*>( _findGroup->second ))->addL1Item( _chainItemL1 );
      } else {
        // We need a new group counter, this should be of type Union
        CounterRatesUnion* _ratesGroup = new CounterRatesUnion(m_costData, _group, 0, 10, (MonitorBase*)this); // Mint new counter
        Info("MonitorRatesUpgrade::createL1Counters","Made a L1 Group counter for: %s", _ratesGroup->getName().c_str() );
        _ratesGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
        _ratesGroup->decorate(kDecPrescaleVal, (Float_t)0.);
        _ratesGroup->decorate(kDecRatesGroupName, _item.m_group);
        _ratesGroup->decorate(kDecType, "Union");
        _ratesGroup->setAdvancedLumiScaling(kFALSE);
        _ratesGroup->addL1Item( _chainItemL1 ); // Add initial counter
        (*_counterMap)[_group] = static_cast<CounterBase*>(_ratesGroup); // Instert into the map
      }
    }
  }

  /**
   * L2 are like L1, but with another hardware layer added
   */
  void MonitorRatesUpgrade::createL2Counters(CounterMap_t* _counterMap) {
    for (const auto _item : m_upgradeChains) {
      if (_item.m_level != 1) continue; // Note should be 1, L2 stored in L1 info

      std::string _chainName = _item.m_l2name;
      const UInt_t _chainID =  _item.m_ID;

      // Find the ChainItem for this chain
      ChainItemMapIt_t _it = m_chainItemsL2.find( _chainName );
      if (_it == m_chainItemsL2.end()) {
        Warning("MonitorRatesUpgrade::createL2Counters","Cannot find L2 item: %s", _chainName.c_str() );
        continue;
      }
      RatesChainItem* _chainItemL2 = _it->second;

      const std::string _group = _item.m_group + std::string("_L2");
      const std::string _prescaleStr = Config::config().getStr(kL1String) + ":" + doubleToString( _chainItemL2->getPS(), 2 );
      CounterRatesChain* _L2Chain = new CounterRatesChain(m_costData, _chainName, _chainID, 10, (MonitorBase*)this); // Mint new counter
      _L2Chain->decorate(kDecRatesGroupName, _group );
      _L2Chain->decorate(kDecPrescaleStr, _prescaleStr);
      _L2Chain->decorate(kDecType, "L2");
      _L2Chain->addL2Item( _chainItemL2 ); // Link it to where it'll be getting its pass/fail info
      (*_counterMap)[_chainName] = static_cast<CounterBase*>(_L2Chain); // Insert into the counterMap

      //Info("MonitorRatesUpgrade::createL2Counters","Create counter: %s", _chainName.c_str() );

      //Add to global L1 counter
      m_globalRateL2Counter->addL2Item( _chainItemL2 );

      // Do group
      CounterMapIt_t _findGroup = _counterMap->find( _group ); // Do we have a counter for this group?
      if ( _findGroup != _counterMap->end() ) { // We do have a group already! Add to it.
        (static_cast<CounterRatesUnion*>( _findGroup->second ))->addL1Item( _chainItemL2 );
      } else {
        // We need a new group counter, this should be of type Union
        CounterRatesUnion* _ratesGroup = new CounterRatesUnion(m_costData, _group, 0, 10, (MonitorBase*)this); // Mint new counter
        _ratesGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
        _ratesGroup->decorate(kDecPrescaleVal, (Float_t)0.);
        _ratesGroup->decorate(kDecRatesGroupName, _group);
        _ratesGroup->decorate(kDecType, "Union");
        _ratesGroup->setAdvancedLumiScaling(kFALSE);
        _ratesGroup->addL1Item( _chainItemL2 ); // Add initial counter
        (*_counterMap)[_group] = static_cast<CounterBase*>(_ratesGroup); // Instert into the map
      }
    }
  }


  void MonitorRatesUpgrade::createL3Counters(CounterMap_t* _counterMap) {
    UNUSED(_counterMap);
  }

  /**
   * Get the current event's TOBs in a TOB accumulator. Read in from ntuple
   * Make sure to delete all these which go into the pileup database at the end.....
   */
  TOBAccumulator* MonitorRatesUpgrade::getEventTOBs() {
    static Float_t _EMCountsPerGeV = Config::config().getInt(kUpgradeEMCountsPerGeV);
    static Float_t _JetCountsPerGeV = Config::config().getInt(kUpgradeJetCountsPerGeV);
    TOBAccumulator* _tobs = new TOBAccumulator();
    Bool_t _hasE = kFALSE;
    for (UInt_t _r = 0; _r < m_costData->getNRoIs(); ++_r) {
      //Info("MonitorRatesUpgrade::getEventTOBs", "Processing TOB %i of %i", _r, (int) m_costData->getNRoIs()); 
      ConfKey_t _t = kNoneString;
      if      (m_costData->getIsRoINone(_r) == kTRUE) continue;
      else if (m_costData->getIsRoIMuon(_r) == kTRUE) _t = kMuonString;
      else if (m_costData->getIsRoIEmTau(_r) == kTRUE) {
        if (m_costData->getIsRoITau(_r) == kTRUE) _t = kTauString;
        else _t = kEmString;
      }
      else if (m_costData->getIsRoIJet(_r) == kTRUE) _t = kJetString;
      else if (m_costData->getIsRoIJetEt(_r) == kTRUE) continue;
      else if (m_costData->getIsRoIEnergy(_r) == kTRUE) {
        //Info("MonitorRatesUpgrade::getEventTOBs", "   Is of type ENERGY");
        if (_tobs->HT() > 0 && m_costData->getRoIEt(_r) <= _tobs->HT()) continue; // Get the largest (full width)
        _hasE = kTRUE;
        _tobs->set(m_costData->getRoIVectorEX(_r), m_costData->getRoIVectorEY(_r), m_costData->getRoIEt(_r));
        continue;
      }
      //Info("MonitorRatesUpgrade::getEventTOBs", "   Is of type %s", Config::config().getName(_t).c_str());
      TOB _tob;
      _tob.m_eta  = m_costData->getRoIEta(_r);
      _tob.m_phi  = m_costData->getRoIPhi(_r);
      _tob.m_et   = m_costData->getRoIEt(_r);
      _tob.m_etLarge = m_costData->getRoIEtLarge(_r);
      _tob.m_iso  = m_costData->getRoIEmTauIsoBits(_r); // returns 0 for others
      _tob.m_id   = m_costData->getRoIID(_r);
      _tob.m_type = _t;
      if (_t == kTauString || _t == kEmString) {
        _tob.m_et /= _EMCountsPerGeV; // Run 2 we have two counts per GeV for EM/TAU
      } else if (_t == kJetString) {
        _tob.m_et /= _JetCountsPerGeV; // Run 2 is still usually 1 count per GeV for jets
      }
      if (_tobs->TOBs().find(_tob) != _tobs->TOBs().end() && Config::config().getDisplayMsg(kMsgDupeTOB) == kTRUE) {
        Error("MonitorRatesUpgrade::getEventTOBs", "Event has multiple TOBs with ID %i\n%s", _tob.m_id, _tobs->print().c_str());
      }
      _tobs->add( _tob );
    }
    if (_hasE == kFALSE && Config::config().getDisplayMsg(kMsgNoTETOB) == kTRUE) {
      Error("MonitorRatesUpgrade::getEventTOBs", "Event has no ENERGY TOB(s). Total TOBs: %i\n%s", m_costData->getNRoIs(), _tobs->print().c_str());
      for (UInt_t _r = 0; _r < m_costData->getNRoIs(); ++_r) Info("MonitorRatesUpgrade::getEventTOBs", " TOB %i/%i of id %i", _r, (Int_t)m_costData->getNRoIs(), m_costData->getRoIID(_r));
    } 
    return _tobs;
  }

  void MonitorRatesUpgrade::validateTriggerEmulation(CounterMap_t* _counterMap, TOBAccumulator* _this, Bool_t _print) {
    static std::set<std::string> _types;
    if (_types.size() == 0) {
      _types.insert("EM"); _types.insert("TAU");
      _types.insert("J"); _types.insert("MU");
      _types.insert("XE"); _types.insert("TE");
    }
    static std::vector<std::string> _validationItems;
    if (_validationItems.size() == 0) {
      _validationItems.push_back("L1_MU15");
      _validationItems.push_back("L1_2MU10");
      _validationItems.push_back("L1_EM15");
      _validationItems.push_back("L1_TAU20");
      _validationItems.push_back("L1_TE30");
      _validationItems.push_back("L1_2EM10VH");
    }
    Int_t _checkTrigEmulation[10] = {0};
    Int_t _checkTrigOnline[10] = {0};
    if (_print) Info("MonitorRatesUpgrade::validateTriggerEmulation", " ###  ###  ###  ###  ###  ###  TRIGGER EMULATION CHECK  ###  ###  ###  ###  ###  ###");
    if (_print) Info("MonitorRatesUpgrade::validateTriggerEmulation", " Event: %i TOBs, MET:(%.0f,%.0f)=%f HT:%f\n%s", (int)_this->TOBs().size(), _this->vX(), _this->vY(), _this->MET(), _this->HT(),_this->print().c_str());

    for (auto _type : _types) {
      if (_print) Info("MonitorRatesUpgrade::validateTriggerEmulation", " *** %s ***", _type.c_str());
      CounterMapIt_t _it = _counterMap->begin();
      std::stringstream _ssEmu;
      for (; _it != _counterMap->end(); ++_it) {
        if ( _it->second->getName().find(_type) == std::string::npos ) continue;
        if ( _it->second->getName().substr(0,3) != "L1_" ) continue;
        if (isZero( ((CounterRatesChain*)(_it->second))->runWeight() ) == kTRUE) continue;
        for (UInt_t _v = 0; _v < _validationItems.size(); ++_v) if (_it->second->getName() == _validationItems.at(_v)) _checkTrigEmulation[_v] = 1;
        _ssEmu << _it->second->getName() << ", ";
      }
      if (_print && _ssEmu.str().size()) Info("MonitorRatesUpgrade::validateTriggerEmulation", " ^^^ EMULATED PASS: %s", _ssEmu.str().c_str() );
      std::stringstream _ssOnline;
      for (UInt_t _c = 0; _c < m_costData->getNL1(); ++_c) {
        if (m_costData->getIsL1PassedBeforePrescale(_c) == kFALSE) continue;
        Int_t _CTPID = m_costData->getL1CtpId(_c);
        const std::string _chainName = TrigConfInterface::getNameFromCtpId( _CTPID );
        if ( _chainName.find(_type) == std::string::npos ) continue;
        if ( _chainName.find("-") != std::string::npos ) continue;
        for (UInt_t _v = 0; _v < _validationItems.size(); ++_v) if (_chainName == _validationItems.at(_v)) _checkTrigOnline[_v] = kTRUE;
        _ssOnline << _chainName << ", ";
      }
      if (_print && _ssOnline.str().size()) Info("MonitorRatesUpgrade::validateTriggerEmulation", " &&& ONLINE PASS: %s", _ssOnline.str().c_str() );
    }
    if (_print == kFALSE ) {
      for (UInt_t _v = 0; _v < _validationItems.size(); ++_v) {
        if (_checkTrigEmulation[_v] != _checkTrigOnline[_v]) {
          Info("MonitorRatesUpgrade::validateTriggerEmulation", " !!!!!!!!!!!!!!!!!!!!!!!!! %s !!!!!!!!!!!!!!!!!!!!!!!", _validationItems.at(_v).c_str() );
          Info("MonitorRatesUpgrade::validateTriggerEmulation", " !!!!!!!!!!!!!!!!!!!! FAILED TO VALIDATE !!!!!!!!!!!!!!!!!!!!");
          validateTriggerEmulation(_counterMap, _this, kTRUE);
          break;
        }
      }
    }
  }

  /**
   * Process new event for this monitor.
   * For the chain rate monitor - we look at a chain's rate and its correlation with other chains in its group
   * @param _weight We don't use the standard weight here - form it ourselves
   */
  void MonitorRatesUpgrade::newEvent(Float_t _weight) {
    UNUSED(_weight);
    m_timer.start();
    if ( Config::config().debug() ) Info("MonitorRatesUpgrade::newEvent", "*** Processing Chain Rates ***");

    // First time? Setup the ChainItem maps to hold the decision and prescale information for the rates counters to use.
    if (m_chainItemsL1.size() == 0) populateChainItemMaps();

    // Require physics events here (not other bunchgroups)
    if (Config::config().getInt(kCurrentEventBunchGroupID) != 1) return;

    if (m_upgradePileupScaling == kTRUE) {
      // FIRST PASS
      // If FIRST PASS, all we do is build the PU database
      static UInt_t _message = 0;
      if (_message == 0) {
        _message = 1;
        Info("MonitorRatesUpgrade::newEvent", "Building the Pileup Database...");
      }
      if (getPass() == 1) {
        if (Config::config().getInt(kCurrentEventWasRandomOnline) == kTRUE) m_pileupDatabase.push_back( getEventTOBs() );
        return;
      }
      if (_message == 1) {
        _message = 2;
        Info("MonitorRatesUpgrade::newEvent", "Moving on to second pass, Pileup Database has:%i events", (Int_t) m_pileupDatabase.size());
      }
    }

    // We do all the weighting here ourselves
    Float_t _weightUpgrade = Config::config().getFloat(kCurrentEventEBWeight) * m_collidingBunchFactor * m_upgradeDeadtimeScaling;

    // SECOND PASS
    static Int_t _tobDebug = 20;
    if (++_tobDebug < 20) Info("MonitorRatesUpgrade::newEvent", " *******************************************");
    TOBAccumulator* _thisEvent = getEventTOBs();
    m_eventsToMix += m_pileupFactor - 1; // -1 because the "main" event has pileup too
    // Add pileup
    for (Int_t _pu = 0; _pu < (Int_t)m_eventsToMix; ++_pu) { // Cast to int rounds down
      UInt_t _pileupLottery = m_R3.Integer( m_pileupDatabase.size() );
      _thisEvent->add( m_pileupDatabase.at( _pileupLottery ) ); 
    }
    if (_tobDebug < 20) Info("MonitorRatesUpgrade::newEvent", " ### MIXING %i EVENTS, NOW HAEV %i TOBs ### MET:%f HT:%f\n%s", (Int_t) m_eventsToMix, (Int_t) _thisEvent->TOBs().size(), _thisEvent->MET(), _thisEvent->HT(),_thisEvent->print().c_str());
    m_eventsToMix -= (Int_t)m_eventsToMix; // Remove whole integer events mixed. Leave fractional part to accumulate for next event.

    for (const auto _chainItem : m_chainItemsL1) _chainItem.second->beginEvent( _thisEvent );
    for (const auto _chainItem : m_chainItemsL2) _chainItem.second->beginEvent( _thisEvent );

    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt; // This counter map holds all the counters.

      // If the counter map is empty, then we need to populate it. We will pre-load a counter for every chain.
      if (_counterMap->size() == 0) populateCounterMap(_counterMap);

      // Loop over all counters, they will use their pre-linked RatesChainItems to get their weights.
      CounterMapIt_t _it = _counterMap->begin();
      for (; _it != _counterMap->end(); ++_it) {
        _it->second->processEventCounter(0, 1, _weightUpgrade);
      }
      if (m_upgradePileupScaling == kFALSE && 0) validateTriggerEmulation(_counterMap, _thisEvent, kFALSE /*print*/);
    }

    for (const auto _chainItem : m_chainItemsL1) _chainItem.second->endEvent();
    for (const auto _chainItem : m_chainItemsL2) _chainItem.second->endEvent();

    delete _thisEvent;

    m_timer.stop();
  }

  /**
   * Saves the topology of all counters to a JSON file linked to the counter name. Imported by web display.
   */
  void MonitorRatesUpgrade::saveRateGraphs() {
    const std::string _output = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(kOutputRatesGraphFilename);
    std::ofstream _fout(_output.c_str());
    JsonExport _json;
    _json.addNode(_fout, "ratesGaphs");

    std::set<std::string> _configsSeen;
    for (CounterCollectionIt_t _collectionIt = m_counterCollections.begin(); _collectionIt != m_counterCollections.end(); ++_collectionIt) {
      for (CounterMapIt_t _counterMapIt = _collectionIt->second.begin(); _counterMapIt != _collectionIt->second.end(); ++_counterMapIt ) {
        CounterBaseRates* _counter = static_cast<CounterBaseRates*>( _counterMapIt->second );

        if (_configsSeen.count( _counter->getName() ) == 1) continue;
        _configsSeen.insert( _counter->getName() );
        _json.addNode(_fout, _counter->getName());

        // If a unique then continue
        if (_counter->getStrDecoration(kDecType) == "UniqueHLT" || _counter->getStrDecoration(kDecType) == "UniqueL1") {
          _json.endNode(_fout);
          continue;
        }

        if (_counter->getL2ItemSet().size() == 0) { // If only L1
          for (ChainItemSetIt_t _L1It = _counter->getL1ItemSet().begin(); _L1It != _counter->getL1ItemSet().end(); ++_L1It ) {
            RatesChainItem* _L1 = (*_L1It);
            const std::string _source = _L1->getName() + " [PS:" + doubleToString(_L1->getPS()) + "]";
            _json.addLeafCustom(_fout, "source", _source, "target", _source);
          }
        } else if (_counter->getL3ItemSet().size() == 0) { // If only L1 and L2
          for (ChainItemSetIt_t _L2It = _counter->getL2ItemSet().begin(); _L2It != _counter->getL2ItemSet().end(); ++_L2It ) {
            RatesChainItem* _L2 = (*_L2It);
            for (ChainItemSetIt_t _L1It = _L2->getLowerStart(); _L1It != _L2->getLowerEnd(); ++_L1It) {
              RatesChainItem* _L1 = (*_L1It);
              const std::string _source = _L1->getName() + " [PS:" + doubleToString(_L1->getPS()) + "]";
              const std::string _target = _L2->getName() + " [PS:" + doubleToString(_L2->getPS()) + "]";
              _json.addLeafCustom(_fout, "source", _source, "target", _target);
            }
          }
        } else { // all three levels
          for (ChainItemSetIt_t _L3It = _counter->getL3ItemSet().begin(); _L3It != _counter->getL3ItemSet().end(); ++_L3It ) {
            RatesChainItem* _L3 = (*_L3It);
            for (ChainItemSetIt_t _L2It = _L3->getLowerStart(); _L2It != _L3->getLowerEnd(); ++_L2It) {
              RatesChainItem* _L2 = (*_L2It);
              const std::string _source = _L2->getName() + " [PS:" + doubleToString(_L2->getPS()) + "]";
              const std::string _target = _L3->getName() + " [PS:" + doubleToString(_L3->getPS()) + "]";
              _json.addLeafCustom(_fout, "source", _source, "target", _target);
              for (ChainItemSetIt_t _L1It = _L2->getLowerStart(); _L1It != _L2->getLowerEnd(); ++_L1It) {
                RatesChainItem* _L1 = (*_L1It);
                const std::string _source = _L1->getName() + " [PS:" + doubleToString(_L1->getPS()) + "]";
                const std::string _target = _L2->getName() + " [PS:" + doubleToString(_L2->getPS()) + "]";
                _json.addLeafCustom(_fout, "source", _source, "target", _target);
              }
            }
          }
        }
        _json.endNode(_fout);
      }
    }
    _json.endNode(_fout); // ratesGraphs
    _fout.close();
  }

  /**
   * Do we use this monitor for this particular mode? Try and keep things managable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorRatesUpgrade::getIfActive(ConfKey_t _mode) {
    switch(_mode) {
      case kDoAllSummary:       return kTRUE;
      case kDoKeySummary:       return kTRUE;
      case kDoLumiBlockSummary: return kTRUE;
      default: Error("MonitorRatesUpgrade::getIfActive", "An invalid summary mode was provided (key %s)", Config::config().getName(_mode).c_str() );
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorRatesUpgrade::saveOutput() {
    // Send finalise calls

    for (CounterCollectionIt_t _collectionIt = m_counterCollections.begin(); _collectionIt != m_counterCollections.end(); ++_collectionIt) {
      const std::string _counterCollectionName = _collectionIt->first;
      // Finalise unique counters
      if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
        for (CounterMapIt_t _counterMapIt = _collectionIt->second.begin(); _counterMapIt != _collectionIt->second.end(); ++_counterMapIt ) {
          CounterBaseRates* _counter = static_cast<CounterBaseRates*>( _counterMapIt->second );
          if (_counter->getStrDecoration(kDecType) == "UniqueHLT" || _counter->getStrDecoration(kDecType) == "UniqueL1") {
            _counter->decorate(kDecLbLength, m_collectionLumiCollector[ _counterCollectionName ]->getTotalLumiBlockTime());
            _counter->finalise();
          }
        }
      }
      // Finalise all other counters (makes use of unique counters)
      for (CounterMapIt_t _counterMapIt = _collectionIt->second.begin(); _counterMapIt != _collectionIt->second.end(); ++_counterMapIt ) {
        //Note we have added a negation here
        CounterBaseRates* _counter = static_cast<CounterBaseRates*>( _counterMapIt->second );
        if (!(_counter->getStrDecoration(kDecType) == "UniqueHLT" || _counter->getStrDecoration(kDecType) == "UniqueL1")) {
          _counter->decorate(kDecLbLength, m_collectionLumiCollector[ _counterCollectionName ]->getTotalLumiBlockTime());
          _counter->finalise();
        }
      }
    }

    TrigXMLService::trigXMLService().saveRuleBookXML(m_counterCollections, getLevelStr());

    if (Config::config().getInt(kOutputRatesGraph) == kTRUE) saveRateGraphs();

    VariableOptionVector_t _toSavePlots;// = m_dummyCounter->getAllHistograms();

    std::vector<TableColumnFormatter> _toSaveTable;

    _toSaveTable.push_back( TableColumnFormatter("Active Time [s]",
      "Integrated length of all lumi blocks which contributed events to this rates prediction.",
      kVarEventsPassedNoPS /*ignored*/, kSavePerCall /*ignored*/, 2, kFormatOptionUseWallTime) );

    _toSaveTable.push_back( TableColumnFormatter("Group",
      "The group this chain belongs to.",
      kDecRatesGroupName, kSavePerCall, 0, kFormatOptionUseStringDecoration) );

    _toSaveTable.push_back( TableColumnFormatter("Weighted PS Rate [Hz]",
      "Rate after applying all prescale(s) as weights.",
      kVarEventsPassed, kSavePerCall, 4, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Weighted PS Rate Err [Hz]",
      "Error on rate after applying all prescale(s) as weights",
      &tableFnRateGetWeightedRateErr, 4 ) );

    if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
      _toSaveTable.push_back( TableColumnFormatter("Unique Rate [Hz]",
        "Rate unique to this chain or combination of chains.",
        kDecUniqueRate, kSavePerCall, 0, kFormatOptionUseStringDecoration) );

      _toSaveTable.push_back( TableColumnFormatter("Unique Fraction [%]",
        "Fraction of rate of this chain with no overlap with any other chain.",
        kDecUniqueFraction, kSavePerCall, 0, kFormatOptionUseStringDecoration) );
    }

    _toSaveTable.push_back( TableColumnFormatter("Unbiased Pass [%]",
      "The probability that this chain passes events selected online with zero trigger bias",
      kVarUnbiasedPassed, kSavePerCall, kVarUnbiasedRun, kSavePerCall, 5, kFormatOptionToPercentage) );

    _toSaveTable.push_back( TableColumnFormatter("Prescale",
      "The prescale of this chain. Only displayed for simple combinations.",
      kDecPrescaleStr, kSavePerCall, 0, kFormatOptionUseStringDecoration) );

    if (Config::config().getInt(kRatesScaleByPS)) {
      _toSaveTable.push_back( TableColumnFormatter("L1 Online Prescale",
        "The online prescale of this chain at L1. Applied here as a weight.",
        kDecPrescaleValOnlineL1, kSavePerCall, 0, kFormatOptionUseFloatDecoration) );
    }

    _toSaveTable.push_back( TableColumnFormatter("ID",
      "The CPTID or HLT Chain ID",
      kDecID, kSavePerCall, 0, kFormatOptionUseIntDecoration) );

    _toSaveTable.push_back( TableColumnFormatter("Raw Active Events",
      "Raw underlying statistics on the number events processed for this chain.",
      kVarEventsRunRawStat, kSavePerCall, 0) );

    _toSaveTable.push_back( TableColumnFormatter("Raw Pass Events",
      "Raw underlying statistics on the number events passed by this chain.",
      kVarEventsPassRawStat, kSavePerCall, 0) );

    _toSaveTable.push_back( TableColumnFormatter("Active Events",
      "Number of events in which the chain - or at least one chain in the combination - was executed.",
      kVarEventsRun, kSavePerCall, 4) );

    _toSaveTable.push_back( TableColumnFormatter("Input Rate [Hz]",
      "Input rate to this chain or combination of chains. At L1 this will be the collision frequency for the bunch pattern.",
      kVarEventsRun, kSavePerCall, 4, kFormatOptionNormaliseWallTime) );

    _toSaveTable.push_back( TableColumnFormatter("Pass Fraction before PS [%]",
      "Fraction of events which pass this trigger before prescale.",
      kVarEventsPassedNoPS, kSavePerCall, kVarEventsRun, kSavePerCall, 4, kFormatOptionToPercentage) );

    _toSaveTable.push_back( TableColumnFormatter("Pass Fraction after PS [%]",
      "Fraction of events which pass this trigger after prescale.",
      kVarEventsPassed, kSavePerCall, kVarEventsRun, kSavePerCall, 4, kFormatOptionToPercentage) );

    _toSaveTable.push_back( TableColumnFormatter("Pass Weighted PS",
      "Number of events this chain or combination passed after applying prescales as weighting factors.",
      kVarEventsPassed, kSavePerCall, 4 ) );


    // We save in three steps to put the different categories in different folders
    // First the chains, veto on AND and Rate (group) counters
    m_filterOutput = kTRUE;
    setName("Rate_Upgrade_ChainL1");
    filterOutputOnStrDecoration(kDecType, "L1");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );

    setName("Rate_Upgrade_ChainL2");
    filterOutputOnStrDecoration(kDecType, "L2");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );

    setName("Rate_Upgrade_ChainHLT");
    filterOutputOnStrDecoration(kDecType, "HLT");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );

    // Now do the GROUP - note we can remove the "Group" option from the table.
    //_toSaveTable.erase( _toSaveTable.begin() + 1 );
    setName("Rate_Upgrade_Group");
    filterOutputOnStrDecoration(kDecType, "Union");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );

  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t _ID )
   * @param _name Cost reference to name of counter.
   * @param _ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorRatesUpgrade::newCounter(  const std::string &_name, Int_t _ID  ) {
    UNUSED( _name );
    UNUSED( _ID );
    Error("MonitorRatesUpgrade::newCounter","For Rates counters - please mint them yourself as the derived type is important (Chain, Union, Intersection etc.)");
    return 0;
  }


} // namespace TrigCostRootAnalysis
