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

// ROOT include(s):
#include <TSystem.h>
#include <TMath.h>

// Local include(s):
#include "../TrigCostRootAnalysis/MonitorRates.h"
#include "../TrigCostRootAnalysis/CounterRatesChain.h"
#include "../TrigCostRootAnalysis/CounterRatesUnion.h"
#include "../TrigCostRootAnalysis/CounterRatesIntersection.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"
#include "../TrigCostRootAnalysis/RatesChainItem.h"
#include "../TrigCostRootAnalysis/WriteXML_JSON.h"
#include "../TrigCostRootAnalysis/RatesCPSGroup.h"

namespace TrigCostRootAnalysis {

  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorRates::MonitorRates(const TrigCostData* _costData) : MonitorBase(_costData, "Rate") {
    m_dummyCounter = static_cast<CounterBase*>(new CounterRatesChain(_costData, Config::config().getStr(kDummyString), INT_MIN));
    m_globalRateHLTCounter = nullptr;
    m_globalRateL1Counter = nullptr;
    m_globalRatePhysicsMainCounter = nullptr;
    m_doingOverlaps = (Bool_t) (Config::config().getInt(kDoAllOverlap) 
      || Config::config().getInt(kDoGroupOverlap)
      || Config::config().getVecSize(kPatternsOverlap) > 0);
    if (m_doingOverlaps) Info("MonitorRates::MonitorRates", "Chain overlap monitoring is enabled for this processing.");
    m_doCPS = (Bool_t) Config::config().getInt(kDoCPS);   }

  /**
   * Destroy this monitor - note we have special RatesChainItems to destroy here too
   */
  MonitorRates::~MonitorRates() {
    for (ChainItemMapIt_t _it = m_chainItemsL1.begin(); _it != m_chainItemsL1.end(); ++_it) {
      delete _it->second;
    }
    for (ChainItemMapIt_t _it = m_chainItemsHLT.begin(); _it != m_chainItemsHLT.end(); ++_it) {
      delete _it->second;
    }
  }

  /**
   * Here we create a RatesChainItem for each trigger item at each level, it is these which will be loaded
   * per event with the raw decision of that trigger item and the counters will use the information of the L1
   * and HLT chain items given to them to calculate their event weight.
   */
  void MonitorRates::populateChainItemMaps() {
    // ##################################################################################################################
    // STEP ONE: Look over the menu and first populate the L1 items
    for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
      if (TrigConfInterface::getChainLevel(_i) != 1) continue; // Only L1 chains

      const std::string _chainName = TrigConfInterface::getChainName(_i);
      Double_t _chainPrescale = TrigXMLService::trigXMLService().getPrescale( _chainName );

      // Check for explicit exclusion
      if (Config::config().getVecMatches(kPatternsExclude, _chainName) == kTRUE ) {
        if (!isZero(_chainPrescale+1)) {
          Warning("MonitorRates::populateChainItemMaps", "Force-disabling chain %s (PS:%.2f) in rates prediction. New PS:-1.", _chainName.c_str(), _chainPrescale);
        }
        _chainPrescale = -1.;
      }

      m_chainItemsL1[ _chainName ] = new RatesChainItem(_chainName, /*chainLevel=*/ 1, _chainPrescale, /*epress prescale=*/ 0.);
    }
    // We have one extra one, AlwaysPass - which will be used for HLT chains with no L1 seed
    RatesChainItem* _alwaysPass = new RatesChainItem(Config::config().getStr(kAlwaysPassString), 1 , 1.); // Set level to 1, PS to 1
    CounterBaseRatesSet_t _dummy;
    _alwaysPass->beginEvent(kTRUE, _dummy); // Set pass raw to kTRUE. This will stay fixed like this forever. Second param is ignored
    m_chainItemsL1[ Config::config().getStr(kBlankString) ] = _alwaysPass; // Associate to a blank string

    // ##################################################################################################################
    // STEP TWO: Now we populate the HLT items
    for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
      if (TrigConfInterface::getChainLevel(_i) == 1) continue; // Only HLT chains

      const std::string _chainName = TrigConfInterface::getChainName(_i);
      const std::string _chainCPSGroup = TrigConfInterface::getChainCPSGroup(_i);
      Double_t _chainPrescale = TrigXMLService::trigXMLService().getPrescale( _chainName );

      Double_t _expressPS = 0.;
      if (TrigXMLService::trigXMLService().hasExpressPrescaleInfo()) {
        _expressPS = TrigXMLService::trigXMLService().getExpressPrescaleInfo(_chainName);
      }

      // Check for explicit exclusion
      if (Config::config().getVecMatches(kPatternsExclude, _chainName) == kTRUE ) {
        if (!isZero(_chainPrescale+1)) {
          Warning("MonitorRates::populateChainItemMaps", "Force-disabling chain %s (PS:%.2f) in rates prediction. New PS:-1.", _chainName.c_str(), _chainPrescale);
        }
        _chainPrescale = -1.;
      }

      RatesChainItem* _chainItemHLT = new RatesChainItem(_chainName, 2, _chainPrescale, _expressPS);
      m_chainItemsHLT[ _chainName ] = _chainItemHLT;

      // Is this a CPS chain? Needs to go in an additional group if so.
      if (m_doCPS == kTRUE && _chainCPSGroup != "") {
        if (m_cpsGroups.count(_chainCPSGroup) == 0) m_cpsGroups[ _chainCPSGroup ] = new RatesCPSGroup(_chainCPSGroup);
        m_cpsGroups[ _chainCPSGroup ]->add( _chainItemHLT );
      } 

      // Now we link the seeding.
      // ################################################################################################################
      // STEP THREE: The HLT chain may be seeded by many L1 items. Need to link them all together
      const std::string _L1Name = TrigConfInterface::getLowerChainName(_chainName);
      std::vector<std::string> _L1NameVector;
      if ( _L1Name.find(",") != std::string::npos ) { // Multiple seeds
        //Get list of individual items.
        std::istringstream _ss(_L1Name);
        std::string _item;
        while(std::getline(_ss, _item, ',')) {
          // Remove any leading space
          if (_item.substr(0,1) == " ") _item = _item.substr(1, _item.length());
          _L1NameVector.push_back( _item );
        }
      } else { // Only one seed
        _L1NameVector.push_back(_L1Name);
      }

      for (UInt_t _i = 0; _i < _L1NameVector.size(); ++_i) {
        ChainItemMapIt_t _it = m_chainItemsL1.find( _L1NameVector.at(_i) );
        if (_it != m_chainItemsL1.end()) {
          RatesChainItem* _chainItemL1 = _it->second;
          _chainItemHLT->addLower( _chainItemL1 );
          _chainItemL1->addUpper( _chainItemHLT );
        } else {
          Warning("MonitorRates::populateChainItemMaps","Cannot find HLT item %s's L1 seeding chain: %s",
            _chainName.c_str(), _L1NameVector.at(_i).c_str() );
        }
      }
    }

    for (const auto _chainItem : m_chainItemsHLT) _chainItem.second->classifyLumiAndRandom();
    for (const auto _chainItem : m_chainItemsL1)  _chainItem.second->classifyLumiAndRandom();

    // Get the common factor of all the CPS groups
    for (const auto _cpsGroup : m_cpsGroups) _cpsGroup.second->calculateCPSFactor();
  }

  /**
   * In every other monitor, we make a new counter the first time we encounter it in the menu.
   * Rates are different, as some of the rates we want to monitor are the combinations of items in the
   * menu. Hence we pre-populate the counterMap with all combinations that we are interested in for the run.
   *
   * We make one counter per chain (L1 and HLT), one per intersection of each chain and other chains in its group
   * and one for the union of all chains in each group.
   */
  void MonitorRates::populateCounterMap(CounterMap_t* _counterMap) {

    createGlobalCounters(_counterMap);
    createL1Counters(_counterMap);
    createHLTCounters(_counterMap);
    createCPSGroupCounters(_counterMap);
    createGroupCounters(_counterMap);
    if (m_doingOverlaps == kTRUE) createOverlapCounters(_counterMap);
  }

  void MonitorRates::createGlobalCounters(CounterMap_t* _counterMap) {
    UNUSED(_counterMap);

    // Crate the global rates counter, this will be the OR of everything HLT
    m_globalRateHLTCounter = new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalHLTString), 0, 10, (MonitorBase*)this); // Mint new counter
    m_globalRateHLTCounter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateHLTCounter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateHLTCounter->decorate(kDecPrescaleValOnlineL1, (Float_t)0);
    m_globalRateHLTCounter->decorate(kDecType, "Union");

    // I will be the OR of everything which has a stream tag of Main
    m_globalRatePhysicsMainCounter = new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalPhysicsMainString), 0, 10, (MonitorBase*)this); // Mint new counter
    m_globalRatePhysicsMainCounter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRatePhysicsMainCounter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRatePhysicsMainCounter->decorate(kDecPrescaleValOnlineL1, (Float_t)0);
    m_globalRatePhysicsMainCounter->decorate(kDecType, "Union");

    // Crate the global L1 counter, this will be the OR of everything L1
    m_globalRateL1Counter = new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalL1String), 0, 10, (MonitorBase*)this); // Mint new counter
    m_globalRateL1Counter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateL1Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateL1Counter->decorate(kDecPrescaleValOnlineL1, (Float_t)0);
    m_globalRateL1Counter->decorate(kDecType, "Union");
  }

  void MonitorRates::createL1Counters(CounterMap_t* _counterMap) {
    // ##################################################################################################################
    // Create the L1 unique rates counters, these will be the OR of everything L1 *EXCEPT* for one chain
    // Note we don't assign any L1 items to the counters in this loop. We're just minting the counters here.
    if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
      for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
        if (TrigConfInterface::getChainLevel(_i) != 1) continue; // Only L1 chains

        const UInt_t _chainID = TrigConfInterface::getChainCounter(_i);
        const std::string _chainName = TrigConfInterface::getChainName(_i);

        if ( checkPatternUnique( _chainName, kFALSE ) == kFALSE ) continue; // In the list of unique counters? Does not get inverted
        if ( checkPatternNameMonitor( _chainName, m_invertFilter ) == kFALSE ) continue;

        const std::string _uniqueName = Config::config().getStr(kRateUniqueString) + _chainName;
        CounterRatesUnion* _uniqueChain = new CounterRatesUnion(m_costData, _uniqueName, _chainID, 10, (MonitorBase*)this); // Mint new counter
        _uniqueChain->decorate(kDecRatesGroupName, Config::config().getStr(kNoneString));
        _uniqueChain->decorate(kDecType, "UniqueL1");
        _uniqueChain->setGlobalRateCounter(m_globalRateL1Counter);
        (*_counterMap)[_uniqueName] = static_cast<CounterBase*>(_uniqueChain); // Insert into the counterMap

        if (Config::config().getDisplayMsg(kMsgNewUniqueCounter) == kTRUE) {
          Info("MonitorRates::createL1Counters", "Preparing unique rates for %s", _chainName.c_str());
        }
      }
    } // End unique rates counter minting

    // ##################################################################################################################
    // Create and add the L1 rates counters while also filling in the L1 items to the unique counters
    for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
      if (TrigConfInterface::getChainLevel(_i) != 1) continue; // Only L1 chains

      const UInt_t _chainID = TrigConfInterface::getChainCounter(_i);
      const std::string _chainName = TrigConfInterface::getChainName(_i);
      const std::string _uniqueName = Config::config().getStr(kRateUniqueString) + _chainName;

      if ( checkPatternNameMonitor( _chainName, m_invertFilter ) == kFALSE ) continue;

      // Find the ChainItem for this chain
      ChainItemMapIt_t _it = m_chainItemsL1.find( _chainName );
      if (_it == m_chainItemsL1.end()) {
        Warning("MonitorRates::populateCounterMap","Cannot find L1 item: %s", _chainName.c_str() );
        continue;
      }
      RatesChainItem* _chainItemL1 = _it->second;

      // Construct a string displaying the PS for this chain
      const std::string _prescaleStr = Config::config().getStr(kL1String) + ":" + doubleToString( _chainItemL1->getPS(), 2 );
      Double_t _prescaleVal = _chainItemL1->getPS();

      // Also get online PS
      Float_t _prescaleValOnlineL1 = TrigConfInterface::getPrescale( _chainName );

      // Do add to unique counters
      CounterBaseRates* _thisChainsUniqueCounter = 0;
      if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
        for (CounterMapIt_t _itA = _counterMap->begin(); _itA != _counterMap->end(); ++_itA) {
          CounterBaseRates* _counter = static_cast<CounterBaseRates*>( _itA->second );
          if ( _counter->getStrDecoration(kDecType) != "UniqueL1" ) continue; // I'm not a unique counter - next
          if ( _counter->getName() == _uniqueName) {
            _thisChainsUniqueCounter = _counter;
            _thisChainsUniqueCounter->decorate(kDecPrescaleStr, _prescaleStr);
            _thisChainsUniqueCounter->decorate(kDecPrescaleVal, (Float_t)_prescaleVal);
            _thisChainsUniqueCounter->decorate(kDecPrescaleValOnlineL1, (Float_t)_prescaleValOnlineL1);
            continue; // I'm the unique counter for *this* chain - *don't* add me!
          }
          _counter->addL1Item( _chainItemL1 ); // Add *all other* L1 items [remember unique = global - (global except for one chain)]
        }
      }

      CounterRatesChain* _L1Chain = new CounterRatesChain(m_costData, _chainName, _chainID, 10, (MonitorBase*)this); // Mint new counter
      _L1Chain->decorate(kDecRatesGroupName, Config::config().getStr(kNoneString) );
      _L1Chain->decorate(kDecPrescaleStr, _prescaleStr);
      _L1Chain->decorate(kDecPrescaleVal, (Float_t)_prescaleVal);
      _L1Chain->decorate(kDecPrescaleValOnlineL1, _prescaleValOnlineL1);
      _L1Chain->decorate(kDecType, "L1");
      _L1Chain->addL1Item( _chainItemL1 ); // Link it to where it'll be getting its pass/fail info
      _L1Chain->setMyUniqueCounter( _thisChainsUniqueCounter ); // Link it to its corresponding unique counter.
      _L1Chain->setGlobalRateCounter(m_globalRateL1Counter);
      (*_counterMap)[_chainName] = static_cast<CounterBase*>(_L1Chain); // Insert into the counterMap

      //Add to global L1 counter
      m_globalRateL1Counter->addL1Item( _chainItemL1 );
    }
  }

  void MonitorRates::createHLTCounters(CounterMap_t* _counterMap) {
   // ##################################################################################################################
    // Create the HLT unique rates counters, these will be the OR of everything HLT *EXCEPT* for one chain
    // Note, as with L1 - we are just minting counters here. Not associating them yet.
    if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
      for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
        if (TrigConfInterface::getChainLevel(_i) == 1) continue; // Only HLT chains

        const UInt_t _chainID = TrigConfInterface::getChainCounter(_i);
        const std::string _chainName = TrigConfInterface::getChainName(_i);
        const std::string _uniqueName = Config::config().getStr(kRateUniqueString) + _chainName;

        if ( checkPatternUnique( _chainName, kFALSE ) == kFALSE ) continue; // In the list of unique counters?
        if ( checkPatternNameMonitor( _chainName, m_invertFilter ) == kFALSE ) continue;

        const std::string _L1Name = TrigConfInterface::getLowerChainName(_chainName);
        if ( _L1Name == Config::config().getStr(kBlankString) ) {
          Error("MonitorRates::createHLTCounters", "Cannot do unique rates for %s. No L1 seed. Unsupported.", _uniqueName.c_str());
          continue;
        }

        // LIMITATION - cannot do unique for CPS chains
        if (m_doCPS == kTRUE && TrigConfInterface::getChainCPSGroup(_i) != "") {
          Error("MonitorRates::createHLTCounters", "Unique rates for chains in CPS groups are not currently supported - bug atlas-trigger-rate-expert@cern.ch if you really need this");
          continue;
        }

        CounterRatesUnion* _uniqueChain = new CounterRatesUnion(m_costData, _uniqueName, _chainID, 10, (MonitorBase*)this); // Mint new counter
        _uniqueChain->decorate(kDecRatesGroupName, Config::config().getStr(kNoneString)); // Not needed
        _uniqueChain->decorate(kDecType, "UniqueHLT");
        _uniqueChain->setGlobalRateCounter(m_globalRateHLTCounter);
        _uniqueChain->setLowerGlobalRateCounter(m_globalRateL1Counter);
        (*_counterMap)[_uniqueName] = static_cast<CounterBase*>(_uniqueChain); // Insert into the counterMap

        if (Config::config().getDisplayMsg(kMsgNewUniqueCounter) == kTRUE) {
          Info("MonitorRates::createHLTCounters", "Preparing unique rates for %s", _chainName.c_str());
        }

      }
    } // End unique rates counter minting

    // ##################################################################################################################
    // STEP ONE: We populate the HLT items
    for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
      if (TrigConfInterface::getChainLevel(_i) == 1) continue; // Only HLT chains

      const UInt_t _chainID = TrigConfInterface::getChainCounter(_i);
      const std::string _chainName = TrigConfInterface::getChainName(_i);
      const std::string _uniqueName = Config::config().getStr(kRateUniqueString) + _chainName;
      const std::vector<std::string> _chainGroups = TrigConfInterface::getChainRatesGroupNames(_i);
      std::string _chainGroupsText = "";
      for (UInt_t _g = 0; _g < _chainGroups.size(); ++_g) {
        _chainGroupsText += _chainGroups.at(_g);
        if (_g != _chainGroups.size()-1) _chainGroupsText += " ";
      }
      //Add also the stream tags 
      const std::vector<std::string> _chainStreams = TrigConfInterface::getChainStreamNames(_i);

      if ( _chainName == Config::config().getStr(kBlankString) ) {
        Warning("MonitorRates::createHLTCounters", "Skipping Chain ID %i. Cannot get name from current configuration.", _chainID);
        continue;
      }

      // HLT chains with no (see: all!) seeds need special treatment
      const std::string _L1Name = TrigConfInterface::getLowerChainName(_chainName);
      bool _hasL1Seed = kTRUE;
      if ( _L1Name == Config::config().getStr(kBlankString) ) _hasL1Seed = kFALSE;

      // Are we running over this chain?
      if ( checkPatternNameMonitor( _chainName, m_invertFilter ) == kFALSE ) continue;

      // Find the ChainItem for this chain
      ChainItemMapIt_t _it = m_chainItemsHLT.find( _chainName );
      if (_it == m_chainItemsHLT.end()) {
        Warning("MonitorRates::createHLTCounters","Cannot find HLT item: %s", _chainName.c_str() );
        continue;
      }
      RatesChainItem* _chainItemHLT = _it->second;

      // Construct a string displaying the PS for this chain
      Float_t _prescaleVal = 1.;
      Float_t _prescaleValOnlineL1 = 1.;
      Float_t _prescaleValOnlineHLT = TrigConfInterface::getPrescale( _chainItemHLT->getName() );
      std::string _L1PSString = Config::config().getStr(kMultipleString);
      if (_chainItemHLT->getLower().size() == 1) {
        _prescaleVal = (*(_chainItemHLT->getLowerStart()))->getPS();
        _prescaleValOnlineL1 = TrigConfInterface::getPrescale( (*(_chainItemHLT->getLowerStart()))->getName() );

        _L1PSString = floatToString( _prescaleVal, 2 );
      } else if (_chainItemHLT->getLower().size() == 0) {
        _L1PSString = Config::config().getStr(kAlwaysPassString); // "UNSEEDED"
      }
      const std::string _prescaleStr = Config::config().getStr(kL1String) + ":" + _L1PSString
        + " " + Config::config().getStr(kHLTString) + ":" + floatToString( _chainItemHLT->getPS(), 2 );
      _prescaleVal *= _chainItemHLT->getPS();

      // Was this chain prescaled out online? 
      if (isZero(_prescaleValOnlineHLT + 1.) == kTRUE) continue; // Don't need to process if prescaled out online EFFICIENCY

      // ################################################################################################################
      // STEP TWO: Do the unique rate for this chain 

      // Each unique chain does the OR of _everything_ *EXCEPT* the chain we're interested in
      // Then at the end it subtracts this rate from the global rate. So we need to add *all* chains *but* this one.

      CounterBaseRates* _thisChainsUniqueCounter = 0;
      if (Config::config().getInt(kDoUniqueRates) == kTRUE && _hasL1Seed == kTRUE) {
        for (CounterMapIt_t _itA = _counterMap->begin(); _itA != _counterMap->end(); ++_itA) {
          CounterBaseRates* _counter = static_cast<CounterBaseRates*>( _itA->second );
          if ( _counter->getStrDecoration(kDecType) != "UniqueHLT" ) continue; // I'm not a unique counter - next
          if ( _counter->getName() == _uniqueName) {
            _thisChainsUniqueCounter = _counter;
            _thisChainsUniqueCounter->decorate(kDecPrescaleStr, _prescaleStr);
            _thisChainsUniqueCounter->decorate(kDecPrescaleVal, _prescaleVal);
            _thisChainsUniqueCounter->decorate(kDecPrescaleValOnlineL1, _prescaleValOnlineL1);
            continue; // I'm the unique counter for *this* chain - *don't* add me!
          }
          // Chain *may* be part of a CPS group
          RatesCPSGroup* _cpsGroup = nullptr;
          const std::string _chainCPSGroup = TrigConfInterface::getChainCPSGroup(_i);
          if (m_doCPS && _chainCPSGroup != "") {
            CPSGroupMapIt_t _it = m_cpsGroups.find( _chainCPSGroup );
            if (_it != m_cpsGroups.end()) _cpsGroup = _it->second;
          }
          // Add this chain to it, or if it's in a CPS group add the group.
          // This will result in the CPS group being added multiple times, but it's a set of pointers so this is fine.
          if (_cpsGroup != nullptr) {
            _counter->addCPSItem( _cpsGroup, _chainName );
          } else {
            _counter->addL2Item( _chainItemHLT );
          }
        }
      }

      // ################################################################################################################
      // STEP THREE: Make a new counter for this HLT Chain

      // And add a new counter to get the rates for this chainItem
      CounterRatesChain* _ratesChain = new CounterRatesChain(m_costData, _chainName, _chainID, 10, (MonitorBase*)this); // Mint new counter
      _ratesChain->decorate(kDecRatesGroupName, _chainGroupsText);
      _ratesChain->decorate(kDecPrescaleStr, _prescaleStr);
      _ratesChain->decorate(kDecPrescaleVal, _prescaleVal);
      _ratesChain->decorate(kDecPrescaleValOnlineL1, _prescaleValOnlineL1);
      _ratesChain->decorate(kDecType, "Chain");
      _ratesChain->setMyUniqueCounter( _thisChainsUniqueCounter ); // Link it to its corresponding unique counter.
      _ratesChain->setGlobalRateCounter(m_globalRateHLTCounter);
      _ratesChain->setLowerGlobalRateCounter(m_globalRateL1Counter);
      _ratesChain->addL2Item( _chainItemHLT ); // Link it to where it'll be getting its pass/fail info
      (*_counterMap)[_chainName] = static_cast<CounterBase*>(_ratesChain); // Insert into the counterMap
      //Info("MonitorRates::createHLTCounters","Created counter for: %s", _chainName.c_str() );

      if (TrigXMLService::trigXMLService().hasExpressPrescaleInfo() && TrigXMLService::trigXMLService().getExpressPrescaleInfo(_chainName) > 0) {
        _ratesChain->decorate(kDecDoExpressChain, 1);
      }
      
    }
  }

  /**
   * Create one union counter per CPS group and add the group to it. Nothing more than that here.
   */
  void MonitorRates::createCPSGroupCounters(CounterMap_t* _counterMap) {
    if (m_doCPS == kFALSE) return;

    for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
      if (TrigConfInterface::getChainLevel(_i) == 1) continue; // Only HLT chains
      const std::string _chainName = TrigConfInterface::getChainName(_i);
      if ( checkPatternNameMonitor( _chainName, m_invertFilter ) == kFALSE ) continue;

      Float_t _prescaleValOnlineHLT = TrigConfInterface::getPrescale( _chainName );
      //Double_t _prescaleValHLT = TrigXMLService::trigXMLService().getPrescale( TrigConfInterface::getChainName(_i) );
      if (isZero(_prescaleValOnlineHLT + 1.) == kTRUE) continue; // Don't need to process if prescaled out online. EFFICIENCY CODE
      //if (isZero(_prescaleValHLT + 1.) == kTRUE) continue; // Don't need to process if prescaled out in prediction. EFFICIENCY CODE

      const std::string _chainCPSGroup = TrigConfInterface::getChainCPSGroup(_i);
      if (_chainCPSGroup == "") continue; // Only chains in a CPS group

      // Check we don't have a counter already
      // (the chains are already grouped in a RatesCPSGroup so we don't need to add them all individually again here)
      if (_counterMap->count(_chainCPSGroup) == 1) {
        dynamic_cast<CounterBaseRates*>(_counterMap->find(_chainCPSGroup)->second)->addCPSItem(nullptr, _chainName );
        continue;
      }

      CPSGroupMapIt_t _it = m_cpsGroups.find( _chainCPSGroup );
      if (_it == m_cpsGroups.end()) {
        Warning("MonitorRates::createCPSGroups", "Cannot find the CPS group for %s", _chainCPSGroup.c_str());
        continue;
      }
      RatesCPSGroup* _cpsGroup = _it->second;

      CounterRatesUnion* _ratesCpsGroup = new CounterRatesUnion(m_costData, _chainCPSGroup, 0, 10, (MonitorBase*)this); // Mint new counter
      _ratesCpsGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
      _ratesCpsGroup->decorate(kDecPrescaleVal, (Float_t)0.);
      _ratesCpsGroup->decorate(kDecPrescaleValOnlineL1, (Float_t)0.);
      _ratesCpsGroup->decorate(kDecRatesGroupName, _chainCPSGroup);
      _ratesCpsGroup->decorate(kDecType, "Union");
      _ratesCpsGroup->setGlobalRateCounter(m_globalRateHLTCounter);
      _ratesCpsGroup->addCPSItem( _cpsGroup, _chainName ); // Add the group
      (*_counterMap)[_chainCPSGroup] = static_cast<CounterBase*>(_ratesCpsGroup); // Instert into the map




    }
  }

  /**
   * Create one counter per RATES group and add to it a combination of CPS groups and non-CPS chains.
   */
  void MonitorRates::createGroupCounters(CounterMap_t* _counterMap) {

    CounterRatesUnion* _expressGroup = nullptr;
    if (TrigXMLService::trigXMLService().hasExpressPrescaleInfo() == kTRUE) {
      _expressGroup = new CounterRatesUnion(m_costData, Config::config().getStr(kRateExpressString), 0, 10, (MonitorBase*)this); // Mint new counter, kRateExpressString is a special string - it will trigger special behaviour
      _expressGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
      _expressGroup->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
      _expressGroup->decorate(kDecPrescaleValOnlineL1, (Float_t)0);
      _expressGroup->decorate(kDecType, "Union");
      (*_counterMap)[Config::config().getStr(kRateExpressString)] = static_cast<CounterBase*>(_expressGroup); // Instert into the map
    }

    for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
      if (TrigConfInterface::getChainLevel(_i) == 1) continue; // Only HLT chains
      if ( checkPatternNameMonitor( TrigConfInterface::getChainName(_i), m_invertFilter ) == kFALSE ) continue;
      const std::string _chainName = TrigConfInterface::getChainName(_i);

      Float_t _prescaleValOnlineHLT = TrigConfInterface::getPrescale( _chainName );
      if (isZero(_prescaleValOnlineHLT + 1.) == kTRUE) continue; // Don't need to process if prescaled out online. EFFICIENCY CODE

      const Bool_t _isMain = TrigConfInterface::getChainIsMainStream(_i);
      std::vector<std::string> _chainGroups = TrigConfInterface::getChainRatesGroupNames(_i);
      const Bool_t _isExpress = (TrigXMLService::trigXMLService().hasExpressPrescaleInfo() && TrigXMLService::trigXMLService().getExpressPrescaleInfo( _chainName ) > 0);

      RatesCPSGroup* _cpsGroup = nullptr;
      const std::string _chainCPSGroup = TrigConfInterface::getChainCPSGroup(_i);
      if (m_doCPS && _chainCPSGroup != "") {
        CPSGroupMapIt_t _it = m_cpsGroups.find( _chainCPSGroup );
        if (_it != m_cpsGroups.end()) _cpsGroup = _it->second;
      }

      // They are still groups, can still get their basic rate
      if (m_doCPS == false && _chainCPSGroup != "") _chainGroups.push_back( _chainCPSGroup );

      // Debug
      // std::stringstream _ss;
      // for (UInt_t _group = 0; _group < _chainGroups.size(); ++_group) _ss << _chainGroups.at(_group) << " ";
      // _ss << TrigConfInterface::getChainCPSGroup(_i);
      // Info("MonitorRates::createGroupCounters","Chain %s in groups: %s", TrigConfInterface::getChainName(_i).c_str(), _ss.str().c_str());

      for (UInt_t _group = 0; _group < _chainGroups.size(); ++_group) {
        std::string _chainGroup = _chainGroups.at(_group);

        ChainItemMapIt_t _it = m_chainItemsHLT.find( _chainName );
        if (_it == m_chainItemsHLT.end()) {
          Warning("MonitorRates::createGroups", "Cannot find a RatesChainItem for %s when doing group %s", TrigConfInterface::getChainName(_i).c_str(), _chainGroup.c_str());
          continue;
        }
        RatesChainItem* _chainItemHLT = _it->second;

        CounterRatesUnion* _ratesGroup = nullptr;
        // Do we have a counter for this group?
        CounterMapIt_t _findGroup = _counterMap->find( _chainGroup );
        if ( _findGroup != _counterMap->end() ) {
          // We do have a group already!
          _ratesGroup = static_cast<CounterRatesUnion*>( _findGroup->second );
        } else {
          // We need a new group counter, this should be of type Union
          _ratesGroup = new CounterRatesUnion(m_costData, _chainGroup, 0, 10, (MonitorBase*)this); // Mint new counter
          _ratesGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
          _ratesGroup->decorate(kDecPrescaleVal, (Float_t)0.);
          _ratesGroup->decorate(kDecPrescaleValOnlineL1, (Float_t)0.);
          _ratesGroup->decorate(kDecRatesGroupName, _chainGroup);
          _ratesGroup->decorate(kDecType, "Union");
          _ratesGroup->setGlobalRateCounter(m_globalRateHLTCounter);
          (*_counterMap)[_chainGroup] = static_cast<CounterBase*>(_ratesGroup); // Insert into the map
        }

        // Add this chain to it, or if it's in a CPS group add the group.
        // This will result in the CPS group being added multiple times, but it's a set of pointers so this is fine.
        if (_cpsGroup != nullptr) {
          _ratesGroup->addCPSItem( _cpsGroup, _chainName );
          if (_group == 0) { // Might be adding to many groups, but only want (need) to add to the globals once
            m_globalRateHLTCounter->addCPSItem( _cpsGroup, _chainName );
            if (_isMain) m_globalRatePhysicsMainCounter->addCPSItem( _cpsGroup, _chainName );
            if (_isExpress) _expressGroup->addCPSItem( _cpsGroup, _chainName );
          }
        } else {
          _ratesGroup->addL2Item( _chainItemHLT );
          if (_group == 0) {
            m_globalRateHLTCounter->addL2Item( _chainItemHLT );
            if (_isMain) m_globalRatePhysicsMainCounter->addL2Item( _chainItemHLT );
            if (_isExpress) _expressGroup->addL2Item( _chainItemHLT );
          }
        }

      } // loop over chain groups
    } // loop over chains
  } // createGroups

  void MonitorRates::createOverlapCounters(CounterMap_t* _counterMap) {

    for (CounterMapIt_t _itA = _counterMap->begin(); _itA != _counterMap->end(); ++_itA) {
      // Look for all Chain counters
      CounterBaseRates* _counterA = static_cast<CounterBaseRates*>( _itA->second );
      if ( _counterA->getStrDecoration(kDecType) != "Chain" ) {
        continue; // I'm not a HLT chain counter - next
      }

      if (checkPatternOverlap(_counterA->getName(), kFALSE) == kFALSE) continue;

      // Now look at all other counters - find others in my group
      for (CounterMapIt_t _itB = _counterMap->begin(); _itB != _counterMap->end(); ++_itB) {
        if (_itA == _itB) continue;

        CounterBaseRates* _counterB = static_cast<CounterBaseRates*>( _itB->second );
        if ( _counterB->getStrDecoration(kDecType) != "Chain" ) {
          continue; // I'm not a HLT chain counter - next
        }

        // Same group?
        if ( Config::config().getInt(kDoGroupOverlap) == kTRUE 
          && _counterA->getStrDecoration(kDecRatesGroupName) != _counterB->getStrDecoration(kDecRatesGroupName)) {
          continue;
        }

        // Construct name for overlap counter
        const std::string _name = _counterA->getName() + Config::config().getStr(kAndString) + _counterB->getName();

        // check we don't have this already the other way around!
        const std::string _altName = _counterB->getName() + Config::config().getStr(kAndString) + _counterA->getName();
        if (_counterMap->count(_altName) > 0) continue;

        // Add new overlap counter!
        CounterRatesIntersection* _overlapCounter = new CounterRatesIntersection(m_costData, _name, 0, 10, (MonitorBase*)this); // Mint new counter
        _overlapCounter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
        _overlapCounter->decorate(kDecPrescaleValOnlineL1, (Float_t)0.);
        _overlapCounter->decorate(kDecRatesGroupName, _counterA->getStrDecoration(kDecRatesGroupName));
        _overlapCounter->decorate(kDecType, "Intersection");
        _overlapCounter->addL2Items( _counterA->getL2ItemSet() );
        _overlapCounter->addL2Items( _counterB->getL2ItemSet() );
        (*_counterMap)[_name] = static_cast<CounterBase*>(_overlapCounter);

        // Give the two chain counters pointers to this overlap counter to be able to calculate their respective overlap with each other
        _counterA->addOverlap( static_cast<CounterBase*>(_overlapCounter) );
        _counterB->addOverlap( static_cast<CounterBase*>(_overlapCounter) );
      }
    }
  }



  /**
   * Process new event for this monitor.
   * For the chain rate monitor - we look at a chain's rate and its correlation with other chains in its group
   * @param _weight The event weight.
   */
  void MonitorRates::newEvent(Float_t _weight) {
    if (getPass() == 2) return; // We don't do 2-pass

    m_timer.start();
    if ( Config::config().debug() ) Info("MonitorRates::newEvent", "*** Processing Chain Rates ***");

    // IMPORTANT NOTE - we REMOVE the Lumi extrap weight as this is now added more intelligently (weighted average per chain combo)
    _weight /= Config::config().getFloat(kLumiExtrapWeight);

    // First time? Setup the ChainItem maps to hold the decision and prescale information for the rates counters to use.
    if (m_chainItemsHLT.size() == 0 || m_chainItemsL1.size() == 0) populateChainItemMaps();

    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt; // This counter map holds all the counters. But we don't really want to process every one
      CounterBaseRatesSet_t _inEventCounterMap; // This set contains all the counters we actually do need to iterate over

      // If the counter map is empty, then we need to populate it. We will pre-load a counter for every chain.
      if (_counterMap->size() == 0) populateCounterMap(_counterMap);


      // ##################################################################################################################
      // PASS ONE: Fill the event information into HLT RatesChainItems - this is their only job, to remember this stuff.
      ChainItemSet_t _chainItemsInEvent;
      for (UInt_t _c = 0; _c < m_costData->getNChains(); ++_c) {
        // Get the name of the chain (Supplying L2 or EF helps, but is not needed)
        Int_t _chainID = m_costData->getChainID(_c);
        const std::string _chainName = TrigConfInterface::getHLTNameFromChainID( _chainID, m_costData->getChainLevel(_c) );
        // Did we fail?
        if ( _chainName == Config::config().getStr(kBlankString) ) {
          Warning("MonitorRates::newEvent", "Skipping Chain ID %i. Cannot get name from current configuration.", _chainID);
          continue;
        }

        // Find the ChainItem for this chain so we can set it's PassedRaw bit for this event
        ChainItemMapIt_t _it = m_chainItemsHLT.find( _chainName );
        if (_it == m_chainItemsHLT.end()) {
          Warning("MonitorRates::newEvent","Cannot find HLT item: %s", _chainName.c_str() );
          continue;
        }

        RatesChainItem* _chainItem = _it->second;
        Bool_t _desicison = kFALSE;
        if (m_isCPUPrediction) _desicison = m_costData->getIsChainPassed(_c);
        else                   _desicison = m_costData->getIsChainPassedRaw(_c);
        _chainItem->beginEvent( _desicison, _inEventCounterMap );
        _chainItemsInEvent.insert( _chainItem );

        if (Config::config().debug()) {
          Info("MonitorRates::newEvent","Storing Decision:%i for HLT ChainItem:%s with PS:%f",
            (Int_t)m_costData->getIsChainPassedRaw(_c),
            _chainItem->getName().c_str(), _chainItem->getPS() );
        }

      }

      // ##################################################################################################################
      // Also fill into L1 items
      for (UInt_t _c = 0; _c < m_costData->getNL1(); ++_c) {

        Int_t _CTPID = m_costData->getL1CtpId(_c);
        const std::string _chainName = TrigConfInterface::getNameFromCtpId( _CTPID );
        // Did we fail?
        if ( _chainName == Config::config().getStr(kBlankString) ) {
          Warning("MonitorRates::newEvent", "Skipping L1 Chain ID %i. Cannot get name from current configuration.", _CTPID);
          continue;
        }

        // Find the ChainItem for this chain so we can set it's PassedRaw bit for this event, could be L1 or HLT
        ChainItemMapIt_t _it = m_chainItemsL1.find( _chainName );
        if (_it == m_chainItemsL1.end()) {
          Warning("MonitorRates::newEvent","Cannot find L1 item: %s", _chainName.c_str() );
          continue;
        }


        // If we are applying EB weights, then we can apply whatever PS we want at L1. And hence, like at HLT, we should use the TBP bit.
        // However. If *not* doing weights, then we cannot undo the L1 prescale and hence the rate is fixed by what is in the file.
        // We should therefore use TAV.

        //XXX TODO READDRESS THIS - NO LONGER TRUE I THINK

        // We definitly want to use TAV when prescales were applied to EB on the grid (CPU prediction mode)

        Bool_t _desicison = kFALSE;
        if (m_isCPUPrediction == kTRUE) _desicison = m_costData->getIsL1PassedAfterVeto(_c);
        else                            _desicison = m_costData->getIsL1PassedBeforePrescale(_c);
        // if ( Config::config().getInt(kDoEBWeighting) == 0) {
        //   _desicison = m_costData->getIsL1PassedAfterVeto(_c);
        // }

        RatesChainItem* _chainItem = _it->second;
        _chainItem->beginEvent( _desicison,  _inEventCounterMap );
        _chainItemsInEvent.insert( _chainItem );


        if (Config::config().debug()) {
          Info("MonitorRates::newEvent","Storing Decision:%i for L1 ChainItem:%s with PS:%f",
            (Int_t) _desicison,
            _chainItem->getName().c_str(), _chainItem->getPS() );
        }
      }

      // If we are debugging such that chains always pass - then we need to re-roll everyone's random PS numbers.
      // This can be done anyway, but it normally not necessary.
      if (Config::config().getInt(kRatesForcePass) == kTRUE) {
        for (ChainItemMapIt_t _it = m_chainItemsHLT.begin(); _it != m_chainItemsHLT.end(); ++_it) {
          _it->second->newRandomPS();
        }
        for (ChainItemMapIt_t _it = m_chainItemsL1.begin(); _it != m_chainItemsL1.end(); ++_it) {
          _it->second->newRandomPS();
        }
      }

      // ##################################################################################################################
      // PASS TWO: Now loop over all counters, they will use their pre-linked RatesChainItems to get their weights.
      // OLD - inefficient, does them all
      // Do the globals first
      m_globalRateL1Counter->processEventCounter(0, 1, _weight);
      m_globalRateHLTCounter->processEventCounter(0, 1, _weight);
      m_globalRatePhysicsMainCounter->processEventCounter(0, 1, _weight);
      CounterMapIt_t _it = _counterMap->begin();
      for (; _it != _counterMap->end(); ++_it) {
        _it->second->processEventCounter(0, 1, _weight);
      }
      // NEW - better - only does the ones we need to
      // CounterBaseRatesSetIt_t _cbrsit = _inEventCounterMap.begin();
      // for (; _cbrsit != _inEventCounterMap.end(); ++_cbrsit) {
      //   (*_cbrsit)->processEventCounter(0, 0, _weight);
      // }

      // ##################################################################################################################
      // PASS THREE: Now we just need to reset the ChainItems which were included in the event.
      for (ChainItemSetIt_t _setIt = _chainItemsInEvent.begin(); _setIt != _chainItemsInEvent.end(); ++_setIt) {
        RatesChainItem* _item = (*_setIt);
        _item->endEvent();
      }
      _chainItemsInEvent.clear();
      m_countersInEvent.clear(); // unused at the moment

    }
    m_timer.stop();
  }

  /**
   * Saves the topology of all counters to a JSON file linked to the counter name. Imported by web display.
   */
  void MonitorRates::saveRateGraphs() {
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
        }

        for (ChainItemSetIt_t _L2It = _counter->getL2ItemSet().begin(); _L2It != _counter->getL2ItemSet().end(); ++_L2It ) {
          RatesChainItem* _L2 = (*_L2It);
          for (ChainItemSetIt_t _L1It = _L2->getLowerStart(); _L1It != _L2->getLowerEnd(); ++_L1It) {
            RatesChainItem* _L1 = (*_L1It);
            const std::string _source = _L1->getName() + " [PS:" + doubleToString(_L1->getPS()) + "]";
            const std::string _target = _L2->getName() + " [PS:" + doubleToString(_L2->getPS()) + "]";
            _json.addLeafCustom(_fout, "source", _source, "target", _target);

          }
        }

        // Don't forget the CPS chain groups
        for (CPSGroupSetIt_t _CPSIt = _counter->getCPSGroupSet().begin(); _CPSIt != _counter->getCPSGroupSet().end(); ++_CPSIt ) {
          RatesCPSGroup* _cpsGroup = (*_CPSIt);
          for (ChainItemSetIt_t _L2It = _cpsGroup->getChainStart(); _L2It != _cpsGroup->getChainEnd(); ++_L2It ) {
            RatesChainItem* _L2 = (*_L2It);
            for (ChainItemSetIt_t _L1It = _L2->getLowerStart(); _L1It != _L2->getLowerEnd(); ++_L1It) {
              RatesChainItem* _L1 = (*_L1It);
              const std::string _source = _L1->getName() + " [PS:" + doubleToString(_L1->getPS()) + "]";
              const std::string _target = _L2->getName() + " [PS:" + doubleToString(_L2->getPS()) + "]";
              _json.addLeafCustom(_fout, "source", _source, "target", _target);
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
  Bool_t MonitorRates::getIfActive(ConfKey_t _mode) {
    switch(_mode) {
      case kDoAllSummary:       return kTRUE;
      case kDoKeySummary:       return kTRUE;
      case kDoLumiBlockSummary: return kTRUE;
      default: Error("MonitorRates::getIfActive", "An invalid summary mode was provided (key %s)", Config::config().getName(_mode).c_str() );
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorRates::saveOutput() {
    // Send finalise calls

    for (CounterCollectionNonConstIt_t _collectionIt = m_counterCollections.begin(); _collectionIt != m_counterCollections.end(); ++_collectionIt) {

      // (Finally) add these to the maps such that they get exported with everything else
      // We didn't add them so far so we could always process these three first.
      (_collectionIt->second)[Config::config().getStr(kRateGlobalL1String)] = static_cast<CounterBase*>(m_globalRateL1Counter);
      (_collectionIt->second)[Config::config().getStr(kRateGlobalHLTString)] = static_cast<CounterBase*>(m_globalRateHLTCounter);
      (_collectionIt->second)[Config::config().getStr(kRateGlobalPhysicsMainString)] = static_cast<CounterBase*>(m_globalRatePhysicsMainCounter);

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

    if (TrigXMLService::trigXMLService().hasExpressPrescaleInfo() == kTRUE) {
      _toSaveTable.push_back( TableColumnFormatter("Express Rate [Hz]",
        "Rate to the express stream.",
        kDecExpressRate, kSavePerCall, 0, kFormatOptionUseStringDecoration) );
    }

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
    setName("Rate_ChainL1");
    filterOutputOnStrDecoration(kDecType, "L1");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );

    setName("Rate_ChainHLT");
    filterOutputOnStrDecoration(kDecType, "Chain");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );

    //_toSaveTable.erase( _toSaveTable.begin() + 2);   // Now do the AND - note we can remove the "Prescale" option from the table.
    if (m_doingOverlaps == kTRUE) {
      setName("Rate_Combination");
      filterOutputOnStrDecoration(kDecType, "Intersection");
      sharedTableOutputRoutine( _toSaveTable );
      sharedHistogramOutputRoutine( _toSavePlots );
    }

    // Now do the GROUP - note we can remove the "Group" option from the table.
    //_toSaveTable.erase( _toSaveTable.begin() + 1 );
    setName("Rate_Group");
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
  CounterBase* MonitorRates::newCounter(  const std::string &_name, Int_t _ID  ) {
    UNUSED( _name );
    UNUSED( _ID );
    Error("MonitorRates::newCounter","For Rates counters - please mint them yourself as the derived type is important (Chain, Union, Intersection etc.)");
    return 0;
  }


} // namespace TrigCostRootAnalysis
