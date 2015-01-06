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

namespace TrigCostRootAnalysis {

  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorRates::MonitorRates(const TrigCostData* _costData) : MonitorBase(_costData, "Rate") {
    m_dummyCounter = static_cast<CounterBase*>(new CounterRatesChain(_costData, Config::config().getStr(kDummyString), INT_MIN));
    m_globalRateHLTCounter = nullptr;
    m_globalRateL1Counter = nullptr;
  }

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

      m_chainItemsL1[ _chainName ] = new RatesChainItem(_chainName, /*chainLevel=*/ 1, _chainPrescale);
    }
    // We have one extra one, AlwaysPass - which will be used for HLT chains with no L1 seed
    RatesChainItem* _alwaysPass = new RatesChainItem(Config::config().getStr(kAlwaysPassString), 1 , 1.); // Set level to 1, PS to 1
    _alwaysPass->beginEvent(kTRUE); // Set pass raw to kTRUE. This will stay fixed like this forever.
    m_chainItemsL1[ Config::config().getStr(kBlankString) ] = _alwaysPass; // Associate to a blank string

    // ##################################################################################################################
    // STEP TWO: Now we populate the HLT items
    for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
      if (TrigConfInterface::getChainLevel(_i) == 1) continue; // Only HLT chains

      const std::string _chainName = TrigConfInterface::getChainName(_i);
      const std::string _chainGroup = TrigConfInterface::getChainRatesGroupName(_i);
      Double_t _chainPrescale = TrigXMLService::trigXMLService().getPrescale( _chainName );

      RatesChainItem* _chainItemHLT = new RatesChainItem(_chainName, 2, _chainPrescale);
      m_chainItemsHLT[ _chainName ] = _chainItemHLT;

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
          _chainItemHLT->addLower( _it->second );
        } else {
          Warning("MonitorRates::populateChainItemMaps","Cannot find HLT item %s's L1 seeding chain: %s",
            _chainName.c_str(), _L1NameVector.at(_i).c_str() );
        }
      }
    }
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
    if (Config::config().getInt(kDoGroupOverlap) || Config::config().getInt(kDoAllOverlap)) {
      createOverlapCounters(_counterMap);
    }

  }

  void MonitorRates::createGlobalCounters(CounterMap_t* _counterMap) {
    // Crate the global rates counter, this will be the OR of everything HLT
    m_globalRateHLTCounter = new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalHLTString), 0); // Mint new counter
    m_globalRateHLTCounter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateHLTCounter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateHLTCounter->decorate(kDecPrescaleValOnlineL1, (Float_t)0);
    m_globalRateHLTCounter->decorate(kDecType, "Global");
    (*_counterMap)[Config::config().getStr(kRateGlobalHLTString)] = static_cast<CounterBase*>(m_globalRateHLTCounter);

    // Crate the global L1 counter, this will be the OR of everything L1
    m_globalRateL1Counter = new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalL1String), 0); // Mint new counter
    m_globalRateL1Counter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateL1Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateL1Counter->decorate(kDecPrescaleValOnlineL1, (Float_t)0);
    m_globalRateL1Counter->decorate(kDecType, "Global");
    (*_counterMap)[Config::config().getStr(kRateGlobalL1String)] = static_cast<CounterBase*>(m_globalRateL1Counter);
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

        const std::string _uniqueName = Config::config().getStr(kRateUniqueString) + _chainName;
        CounterRatesUnion* _uniqueChain = new CounterRatesUnion(m_costData, _uniqueName, _chainID); // Mint new counter
        _uniqueChain->decorate(kDecRatesGroupName, Config::config().getStr(kNoneString));
        _uniqueChain->decorate(kDecType, "UniqueL1");
        _uniqueChain->setGlobalRateCounter(m_globalRateL1Counter);
        (*_counterMap)[_uniqueName] = static_cast<CounterBase*>(_uniqueChain); // Insert into the counterMap

      }
    } // End unique rates

    // ##################################################################################################################
    // Create and add the L1 rates counters while also filling in the L1 items to the unique counters
    for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
      if (TrigConfInterface::getChainLevel(_i) != 1) continue; // Only HLT chains

      const UInt_t _chainID = TrigConfInterface::getChainCounter(_i);
      const std::string _chainName = TrigConfInterface::getChainName(_i);
      const std::string _uniqueName = Config::config().getStr(kRateUniqueString) + _chainName;

      // Find the ChainItem for this chain 
      ChainItemMapIt_t _it = m_chainItemsL1.find( _chainName );
      if (_it == m_chainItemsHLT.end()) {
        Warning("MonitorRates::populateCounterMap","Cannot find L1 item: %s", _chainName.c_str() );
        continue;
      }
      RatesChainItem* _chainItemL1 = _it->second;

      // Construct a string displaying the PS for this chain
      const std::string _prescaleStr = Config::config().getStr(kL1String) + ":" + doubleToString( _chainItemL1->getPS() );
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

      CounterRatesChain* _L1Chain = new CounterRatesChain(m_costData, _chainName, _chainID); // Mint new counter
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
        const std::string _chainGroup = TrigConfInterface::getChainRatesGroupName(_i);
        const std::string _uniqueName = Config::config().getStr(kRateUniqueString) + _chainName;

        const std::string _L1Name = TrigConfInterface::getLowerChainName(_chainName);
        if ( _L1Name == Config::config().getStr(kBlankString) ) {
          Warning("MonitorRates::populateCounterMap", "Skipping Chain %s. No L1 seed. Unsupported.", _uniqueName.c_str());
          continue;
        }

        CounterRatesUnion* _uniqueChain = new CounterRatesUnion(m_costData, _uniqueName, _chainID); // Mint new counter
        _uniqueChain->decorate(kDecRatesGroupName, _chainGroup);
        _uniqueChain->decorate(kDecType, "UniqueHLT");
        _uniqueChain->setGlobalRateCounter(m_globalRateHLTCounter);
        (*_counterMap)[_uniqueName] = static_cast<CounterBase*>(_uniqueChain); // Insert into the counterMap

      }
    } // End unique rates

    // ##################################################################################################################
    // STEP ONE: We populate the HLT items
    for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
      if (TrigConfInterface::getChainLevel(_i) == 1) continue; // Only HLT chains

      const UInt_t _chainID = TrigConfInterface::getChainCounter(_i);
      const std::string _chainName = TrigConfInterface::getChainName(_i);
      const std::string _uniqueName = Config::config().getStr(kRateUniqueString) + _chainName;
      const std::string _chainGroup = TrigConfInterface::getChainRatesGroupName(_i);

      if ( _chainName == Config::config().getStr(kBlankString) ) {
        Warning("MonitorRates::populateCounterMap", "Skipping Chain ID %i. Cannot get name from current configuration.", _chainID);
        continue;
      }

      // Currently veto HLT chains with no (see: all!) seeds
      const std::string _L1Name = TrigConfInterface::getLowerChainName(_chainName);
      if ( _L1Name == Config::config().getStr(kBlankString) ) {
        Warning("MonitorRates::populateCounterMap", "Skipping Chain %s. No L1 seed. Unsupported.", _chainName.c_str());
        continue;
      }

      // Are we running over this chain?
      if ( checkPatternNameMonitor( _chainName ) == kFALSE ) continue;

      // Find the ChainItem for this chain 
      ChainItemMapIt_t _it = m_chainItemsHLT.find( _chainName );
      if (_it == m_chainItemsHLT.end()) {
        Warning("MonitorRates::populateCounterMap","Cannot find HLT item: %s", _chainName.c_str() );
        continue;
      }
      RatesChainItem* _chainItemHLT = _it->second;

      // Construct a string displaying the PS for this chain
      Float_t _prescaleVal = 0.;
      Float_t _prescaleValOnlineL1 = 0.;
      std::string _L1PSString = Config::config().getStr(kMultipleString);
      if (_chainItemHLT->getLower().size() == 1) {
        _prescaleVal = (*(_chainItemHLT->getLowerStart()))->getPS();
        _prescaleValOnlineL1 = TrigConfInterface::getPrescale( (*(_chainItemHLT->getLowerStart()))->getName() );

        _L1PSString = floatToString( _prescaleVal, 0 );
      } else if (_chainItemHLT->getLower().size() == 0) {
        _L1PSString = Config::config().getStr(kAlwaysPassString); // "UNSEEDED"
      }
      const std::string _prescaleStr = Config::config().getStr(kL1String) + ":" + _L1PSString 
        + " " + Config::config().getStr(kHLTString) + ":" + floatToString( _chainItemHLT->getPS() );
      _prescaleVal *= _chainItemHLT->getPS();


      // ################################################################################################################
      // STEP TWO: Do the unique rate for this chain and the Global rates

      // Each unique chain does the OR of _everything_ *EXCEPT* the chain we're interested in
      // Then at the end it subtracts this rate from the global rate. So we need to add *all* chains *but* this one.

      CounterBaseRates* _thisChainsUniqueCounter = 0;
      if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
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
          _counter->addHLTItem( _chainItemHLT );
        }
      }

      // Insert into the global rates counter
      m_globalRateHLTCounter->addHLTItem( _chainItemHLT );

      // ################################################################################################################
      // STEP THREE: Make a new counter for this HLT Chain

      // And add a new counter to get the rates for this chainItem
      CounterRatesChain* _ratesChain = new CounterRatesChain(m_costData, _chainName, _chainID); // Mint new counter
      _ratesChain->decorate(kDecRatesGroupName, _chainGroup);
      _ratesChain->decorate(kDecPrescaleStr, _prescaleStr);
      _ratesChain->decorate(kDecPrescaleVal, _prescaleVal);
      _ratesChain->decorate(kDecPrescaleValOnlineL1, _prescaleValOnlineL1);
      _ratesChain->decorate(kDecType, "Chain");
      _ratesChain->setMyUniqueCounter( _thisChainsUniqueCounter ); // Link it to its corresponding unique counter.
      _ratesChain->setGlobalRateCounter(m_globalRateHLTCounter);
      _ratesChain->addHLTItem( _chainItemHLT ); // Link it to where it'll be getting its pass/fail info
      (*_counterMap)[_chainName] = static_cast<CounterBase*>(_ratesChain); // Insert into the counterMap


      // Now we also want to do GroupRates
      // ################################################################################################################
      // STEP FOUR: Create a new group, or add this new chain rates counter to a pre-existing group
      if (_chainGroup == Config::config().getStr(kBlankString)) continue;

      // Do we have a counter for this group?
      CounterMapIt_t _findGroup = _counterMap->find( _chainGroup );
      if ( _findGroup != _counterMap->end() ) {
        // We do have a group already! Add to it.
        (static_cast<CounterRatesUnion*>( _findGroup->second ))->addHLTItem( _chainItemHLT );
      } else {
        // We need a new group counter, this should be of type Union
        CounterRatesUnion* _ratesGroup = new CounterRatesUnion(m_costData, _chainGroup, 0); // Mint new counter
        _ratesGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
        _ratesGroup->decorate(kDecPrescaleVal, (Float_t)0.);
        _ratesGroup->decorate(kDecPrescaleValOnlineL1, (Float_t)0.);
        _ratesGroup->decorate(kDecRatesGroupName, _chainGroup);
        _ratesGroup->setGlobalRateCounter(m_globalRateHLTCounter);
        _ratesGroup->decorate(kDecType, "Union");
        (*_counterMap)[_chainGroup] = static_cast<CounterBase*>(_ratesGroup);
      }
    }
  }

  void MonitorRates::createOverlapCounters(CounterMap_t* _counterMap) {

    for (CounterMapIt_t _itA = _counterMap->begin(); _itA != _counterMap->end(); ++_itA) {
      // Look for all Chain counters
      CounterBaseRates* _counterA = static_cast<CounterBaseRates*>( _itA->second );
      if ( _counterA->getStrDecoration(kDecType) != "Chain" ) {
        continue; // I'm not a HLT chain counter - next
      }
      
      // Now look at all other counters - find others in my group
      CounterMapIt_t _itB = _itA;
      for (++_itB; _itB != _counterMap->end(); ++_itB) {
        CounterBaseRates* _counterB = static_cast<CounterBaseRates*>( _itB->second );
        if ( _counterB->getStrDecoration(kDecType) != "Chain" ) {
          continue; // I'm not a HLT chain counter - next
        }

        // Same group?
        if ( Config::config().getInt(kDoAllOverlap) == kFALSE // Implies that kDoGroupOverlap is true
          && _counterA->getStrDecoration(kDecRatesGroupName) != _counterB->getStrDecoration(kDecRatesGroupName)) {
          continue;
        }

        // Construct name for overlap counter
        const std::string _name = _counterA->getName() + Config::config().getStr(kAndString) + _counterB->getName();

        // Add new overlap counter!
        CounterRatesIntersection* _overlapCounter = new CounterRatesIntersection(m_costData, _name, 0);
        _overlapCounter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
        _overlapCounter->decorate(kDecPrescaleValOnlineL1, (Float_t)0.);
        _overlapCounter->decorate(kDecRatesGroupName, _counterA->getStrDecoration(kDecRatesGroupName));
        _overlapCounter->decorate(kDecType, "Intersection");
        _overlapCounter->addHLTItems( _counterA->getHLTItemSet() );
        _overlapCounter->addHLTItems( _counterB->getHLTItemSet() );
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
  
    if ( Config::config().debug() ) Info("MonitorRates::newEvent", "*** Processing Chain Rates ***");
    
    // Populate the counter collections we'll be using for this event.
    collateCounterCollectionsForEvent( m_costData->getLumi(), TrigConfInterface::getCurrentDBKey() );

    // First time? Setup the ChainItem maps to hold the decision and prescale information for the rates counters to use.
    if (m_chainItemsHLT.size() == 0 || m_chainItemsL1.size() == 0) populateChainItemMaps();
    
    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt;

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
        _chainItem->beginEvent( m_costData->getIsChainPassedRaw(_c) );
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

        Bool_t _desicison = m_costData->getIsL1PassedBeforePrescale(_c);
        if ( Config::config().getInt(kDoEBWeighting) == 0) {
          _desicison = m_costData->getIsL1PassedAfterVeto(_c);
        }

        RatesChainItem* _chainItem = _it->second;
        _chainItem->beginEvent( _desicison );
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
      CounterMapIt_t _it = _counterMap->begin();
      for (; _it != _counterMap->end(); ++_it) {
        _it->second->processEventCounter(0, 0, _weight);
      }
      
      // ##################################################################################################################
      // PASS THREE: Now we just need to reset the ChainItems which were included in the event.
      for (ChainItemSetIt_t _setIt = _chainItemsInEvent.begin(); _setIt != _chainItemsInEvent.end(); ++_setIt) {
        RatesChainItem* _item = (*_setIt);
        _item->endEvent();
      }
      _chainItemsInEvent.clear();
      m_countersInEvent.clear();

    }
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

        if (_counter->getHLTItemSet().size() == 0) {
            _json.addLeafCustom(_fout, "source", "TEST", "target", "TEST");
        }

        for (ChainItemSetIt_t _L2It = _counter->getHLTItemSet().begin(); _L2It != _counter->getHLTItemSet().end(); ++_L2It ) {
          RatesChainItem* _L2 = (*_L2It);
          for (ChainItemSetIt_t _L1It = _L2->getLowerStart(); _L1It != _L2->getLowerEnd(); ++_L1It) {
            RatesChainItem* _L1 = (*_L1It);

            const std::string _source = _L1->getName() + " [PS:" + doubleToString(_L1->getPS()) + "]";
            const std::string _target = _L2->getName() + " [PS:" + doubleToString(_L2->getPS()) + "]";
            _json.addLeafCustom(_fout, "source", _source, "target", _target);

          }
        }
        _json.endNode(_fout);
      }
    }
    _json.endNode(_fout); // ratesGraphs
    _fout.close();
  }
  
  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorRates::saveOutput() {
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

    saveRuleBookXML();

    if (Config::config().getInt(kOutputRatesGraph) == kTRUE) saveRateGraphs();

    VariableOptionVector_t _toSavePlots;// = m_dummyCounter->getAllHistograms();

    std::vector<TableColumnFormatter> _toSaveTable;

    _toSaveTable.push_back( TableColumnFormatter("Active Time [s]", 
      "Integrated length of all lumi blocks which contributed events to this rates prediction.",
      kVarEventsPassedNoPS /*ignored*/, kSavePerCall /*ignored*/, 2, kFormatOptionUseWallTime) );

    _toSaveTable.push_back( TableColumnFormatter("Group", 
      "The group this chain belongs to.",
      kDecRatesGroupName, kSavePerCall, 0, kFormatOptionUseStringDecoration) );

    _toSaveTable.push_back( TableColumnFormatter("Prescale", 
      "The prescale of this chain. Only displayed for simple combinations.",
      kDecPrescaleStr, kSavePerCall, 0, kFormatOptionUseStringDecoration) );

    if (Config::config().getInt(kRatesScaleByPS)) {
      _toSaveTable.push_back( TableColumnFormatter("L1 Online Prescale", 
        "The online prescale of this chain at L1. Applied here as a weight.",
        kDecPrescaleValOnlineL1, kSavePerCall, 0, kFormatOptionUseFloatDecoration) );
    }

    _toSaveTable.push_back( TableColumnFormatter("Raw Active Events", 
      "Raw underlying statistics on the number events processed for this chain.",
      kVarEventsRunRawStat, kSavePerCall, 0) );

    _toSaveTable.push_back( TableColumnFormatter("Active Events", 
      "Number of events in which the chain - or at least one chain in the combination - was executed at the HLT.",
      kVarEventsRun, kSavePerCall, 4) );

    _toSaveTable.push_back( TableColumnFormatter("Passed Before PS", 
      "Number of events in which this chain or combination passed. Irrespective of prescale.",
      kVarEventsPassedNoPS, kSavePerCall, 4 ) );

    _toSaveTable.push_back( TableColumnFormatter("Pass Weighted PS", 
      "Number of events this chain or combination passed after applying prescales as weighting factors.",
      kVarEventsPassed, kSavePerCall, 4 ) );

    _toSaveTable.push_back( TableColumnFormatter("Weighted PS Rate [Hz]", 
      "Rate after applying all prescale(s) as weights.",
      kVarEventsPassed, kSavePerCall, 4, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Weighted PS Rate Err [Hz]", 
      "Error on rate after applying all prescale(s) as weights",
      &tableFnRateGetWeightedRateErr, 4 ) );

    if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
      _toSaveTable.push_back( TableColumnFormatter("Unique Rate [Hz]", 
        "Rate unique to this chain or combination of chains.",
        kDecUniqueRate, kSavePerCall, 4, kFormatOptionUseFloatDecoration) );

      _toSaveTable.push_back( TableColumnFormatter("Unique Fraction [%]", 
        "Fraction of rate unique to this chain or combination of chains.",
        kDecUniqueFraction, kSavePerCall, 4, kFormatOptionUseFloatDecoration) );
    }

    // We save in three steps to put the different categories in different folders
    // First the chains, veto on AND and Rate (group) counters
    m_filterOutput = kTRUE;
    m_invertFilter = kTRUE;
    setName("Rate_ChainL1");
    Config::config().addVecEntry(kPatternsOutput, Config::config().getStr(kAndString));
    Config::config().addVecEntry(kPatternsOutput, "UNIQUE_");
    Config::config().addVecEntry(kPatternsOutput, "RATE_");
    Config::config().addVecEntry(kPatternsOutput, "Rate_");
    Config::config().addVecEntry(kPatternsOutput, "HLT_");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );

    m_invertFilter = kTRUE;
    setName("Rate_ChainHLT");
    Config::config().clearVec(kPatternsOutput);
    Config::config().addVecEntry(kPatternsOutput, Config::config().getStr(kAndString));
    Config::config().addVecEntry(kPatternsOutput, "UNIQUE_");
    Config::config().addVecEntry(kPatternsOutput, "RATE_");
    Config::config().addVecEntry(kPatternsOutput, "Rate_");
    Config::config().addVecEntry(kPatternsOutput, "L1_");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );

    m_invertFilter = kFALSE;
    Config::config().clearVec(kPatternsOutput);
    Config::config().addVecEntry(kPatternsOutput, Config::config().getStr(kAndString));
    _toSaveTable.erase( _toSaveTable.begin() + 2);   // Now do the AND - note we can remove the "Prescale" option from the table.
    setName("Rate_Combination"); 
    if (Config::config().getInt(kDoGroupOverlap) || Config::config().getInt(kDoAllOverlap)) {
      sharedTableOutputRoutine( _toSaveTable );
      sharedHistogramOutputRoutine( _toSavePlots );
    }

    // Now do the GROUP - note we can remove the "Group" option from the table.
    m_invertFilter = kFALSE;
    Config::config().clearVec(kPatternsOutput);
    Config::config().addVecEntry(kPatternsOutput, "RATE_");
    Config::config().addVecEntry(kPatternsOutput, "Rate_");
    _toSaveTable.erase( _toSaveTable.begin() + 1 );
    setName("Rate_Group");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );

  }

  void MonitorRates::saveRuleBookXML() {

    if (Config::config().getInt(kOutputXML) == kFALSE) return;

    // Save tables. Loop over counter collections.
    CounterCollectionIt_t _colIt = m_counterCollections.begin();
    for (; _colIt != m_counterCollections.end(); ++_colIt) {
      const std::string _counterCollectionName = _colIt->first;
      const CounterMap_t* _counterMap = &(_colIt->second);
      // Skip if there are no counters to process
      if ( _counterMap->size() == 0) continue;

      const std::string _outputFolder = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(kOutputXMLDirectory);
      gSystem->mkdir( _outputFolder.c_str(), kTRUE);
        
      const std::string _xmlName = _outputFolder 
        + "/TrigRate_" 
        + Config::config().getStr(kOutputTag) + "_"
        + getLevelStr() + "_" 
        + _counterCollectionName 
        + ".xml";
      std::ofstream _fout;
      _fout.open( _xmlName.c_str() );
      _fout << std::fixed; // Use fixed width output

      if (Config::config().debug()) {
        Info("MonitorRates::saveRuleBookXML","Doing XML output with path %s.", _xmlName.c_str());
      }

      XMLExport _xml(_fout);
      _xml.setSpaces(2);
      _xml.addNode(_fout, "trigger");

      // Do General Info 
      _xml.addNode(_fout, "Xsection", intToString(0)); // TODO
      _xml.addNode(_fout, "Luminosity", intToString(0)); // TODO
      _xml.addNode(_fout, "GenEff", intToString(0)); // TODO
      _xml.addNode(_fout, "n_evts", intToString(Config::config().getInt(kEventsProcessed)) ); 
      _xml.addNode(_fout, "PredictionLumi", intToString(0)); // TODO 
      for (UInt_t _f = 0; _f < Config::config().getVecSize(kInputFiles); ++_f) {
        _xml.addNode(_fout, "Dataset", Config::config().getVecEntry(kInputFiles, _f));
      }
      _xml.addNode(_fout, "AtlasProject", TrigConfInterface::getMetaStringVal("AtlasProject") );
      _xml.addNode(_fout, "AtlasVersion", TrigConfInterface::getMetaStringVal("AtlasVersion") );
      _xml.addNode(_fout, "triggerMenuSetup", TrigConfInterface::getMetaStringVal("triggerMenuSetup") );
      _xml.addNode(_fout, "L1PrescaleSet", TrigConfInterface::getMetaStringVal("L1PrescaleSet") );
      _xml.addNode(_fout, "HLTPrescaleSet", TrigConfInterface::getMetaStringVal("HLTPrescaleSet") );
      _xml.addNode(_fout, "CMTPATH", TrigConfInterface::getMetaStringVal("CMTPATH") );

      // Do Bunch Group info
      StringIntMap_t _bunchGroups = TrigConfInterface::getBunchGroupSetup();
      UInt_t _bgCounter = 0;
      for (StringIntMapIt_t _it = _bunchGroups.begin(); _it != _bunchGroups.end(); ++_it) {
        _xml.addNode(_fout, "bunchgrouptype");
        _xml.addNode(_fout, "bunchgroup_keynum", intToString(_bgCounter++) );
        _xml.addNode(_fout, "bunchgroup_key", _it->first );
        _xml.addNode(_fout, "bunchgroup_size", intToString(_it->second) );
        _xml.endNode(_fout); // bunchgrouptype
      }

      _xml.addNode(_fout, "level");
      //Add L1 data
      _xml.addNode(_fout, "lvl_name", "L1");
      for (CounterMapIt_t _counterMapIt = _colIt->second.begin(); _counterMapIt != _colIt->second.end(); ++_counterMapIt ) {
        CounterBaseRates* _counter = static_cast<CounterBaseRates*>( _counterMapIt->second );
        if (_counter->getStrDecoration(kDecType) != "L1") continue;
        saveXMLElement(_fout, _xml, _counter);
      }
      _xml.endNode(_fout); //level
      _xml.addNode(_fout, "level");
      //Add HLT data
      _xml.addNode(_fout, "lvl_name", "HLT");
      for (CounterMapIt_t _counterMapIt = _colIt->second.begin(); _counterMapIt != _colIt->second.end(); ++_counterMapIt ) {
        CounterBaseRates* _counter = static_cast<CounterBaseRates*>( _counterMapIt->second );
        if (_counter->getStrDecoration(kDecType) != "Chain") continue;
        saveXMLElement(_fout, _xml, _counter);
      }
      _xml.endNode(_fout); //level
      _xml.endNode(_fout); //trigger

      _fout.close();
    }
  }

  /**
   * Save all rate data for a single chain into an output XML stream
   * @param _fout Reference to output stream.
   * @param _xml Reference to XML export engine.
   * @returns _counter Counter to export.
   */
  void MonitorRates::saveXMLElement(std::ofstream& _fout, XMLExport& _xml, CounterBaseRates* _counter) {
    const UInt_t _xmlPrecision = 7;

    Float_t _evPassWeight = _counter->getValue(kVarEventsPassed, kSavePerCall);
    Float_t _evPassWeightErr = _counter->getValueError(kVarEventsPassed, kSavePerCall); // Get sqrt(sumW2)

    //Float_t _evPassDirect = _counter->getValue(kVarEventsPassedDP, kSavePerCall);
    //Float_t _evPassDirectErr = _counter->getValueError(kVarEventsPassedDP, kSavePerCall); //unused
    
    Float_t _evRun = _counter->getValue(kVarEventsRun, kSavePerCall); // EB Weighted
    Float_t _evPassNoPS = _counter->getValue(kVarEventsPassedNoPS, kSavePerCall); // EB Weighted

    Float_t _evPassRawStat = _counter->getValue(kVarEventsPassRawStat, kSavePerCall); // Not EB Weighted
    //Float_t _evRunRawStat  = _counter->getValue(kVarEventsRunRawStat,  kSavePerCall); // Not EB Weighted

    Float_t _walltime = _counter->getDecoration(kDecLbLength);

    Float_t _rate = _evPassWeight / _walltime;
    Float_t _rateErr = _evPassWeightErr / _walltime;    // B. Peterson # err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)

    Float_t _uniqueRate = _counter->getDecoration(kDecUniqueRate);
    Float_t _uniqueFraction = _counter->getDecoration(kDecUniqueFraction) / 100.; // Was in percent, change back to 0-1

    Bool_t _isL1 = kFALSE;
    if (_counter->getStrDecoration(kDecType) == "L1") _isL1 = kTRUE;

    // TODO - THIS IS WRONG FOR WEIGHTED EVENTS
    Float_t _eff = 0., _effErr = 0., _psEff = 0., _psEffErr = 0.;
    if (_evRun) {
      _eff = _evPassNoPS / _evRun;
      _effErr = (1./_evRun) * TMath::Sqrt( _evPassNoPS * (1. - _eff) ); // Binomal
      _psEff = _evPassWeight / _evRun;
      _psEffErr =  (1./_evRun) * TMath::Sqrt( _evPassWeight * (1. - _psEff) ); // Binomal
    }

    UNUSED(_effErr);
    UNUSED(_psEffErr);

    _xml.addNode(_fout, "signature");
    _xml.addNode(_fout, "sig_name", _counter->getName());
    _xml.addNode(_fout, "express_stream", intToString(0) ); // TODO add me!
    _xml.addNode(_fout, "chain_prescale", floatToString( _counter->getBasicPrescale() )); // This holds the *item* PS
    if (_isL1) {
      _xml.addNode(_fout, "passthrough", "0" );
      _xml.addNode(_fout, "lower_chain_name", "" );
    } else {
      _xml.addNode(_fout, "passthrough", floatToString( TrigConfInterface::getPassthrough(_counter->getName()), _xmlPrecision ) );
      _xml.addNode(_fout, "lower_chain_name", TrigConfInterface::getLowerChainName(_counter->getName()) );
    }
    _xml.addNode(_fout, "evts_passed", floatToString( _evPassRawStat, _xmlPrecision));
    _xml.addNode(_fout, "evts_passed_weighted", floatToString( _evPassWeight, _xmlPrecision));
    _xml.addNode(_fout, "efficiency", floatToString( _eff, _xmlPrecision));
    //_xml.addNode(_fout, "efficiency_err", floatToString( _effErr, _xmlPrecision)); // TODO FIX ME
    _xml.addNode(_fout, "efficiency_err", floatToString((Float_t)0.));
    _xml.addNode(_fout, "prescaled_efficiency", floatToString( _psEff, _xmlPrecision));
    //_xml.addNode(_fout, "prescaled_efficiency_err", floatToString( _psEffErr, _xmlPrecision)); // TODO FIX ME
    _xml.addNode(_fout, "prescaled_efficiency_err",  floatToString((Float_t)0.));
    _xml.addNode(_fout, "rate", floatToString( _rate, _xmlPrecision));
    _xml.addNode(_fout, "rate_err", floatToString( _rateErr, _xmlPrecision));
    _xml.addNode(_fout, "unique_fraction", floatToString(_uniqueFraction, _xmlPrecision)); 
    _xml.addNode(_fout, "unique_rate", floatToString(_uniqueRate, _xmlPrecision));
    _xml.endNode(_fout); //signature

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
