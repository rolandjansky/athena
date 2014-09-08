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
      Int_t _chainPrescale = TrigXMLService::trigXMLService().getPrescale( _chainName );

      m_chainItemsL1[ _chainName ] = new RatesChainItem(_chainName, _chainPrescale);
    }
    // We have one extra one, AlwaysPass - which will be used for HLT chains with no L1 seed
    RatesChainItem* _alwaysPass = new RatesChainItem(Config::config().getStr(kAlwaysPassString), 1); // Set PS to 1
    _alwaysPass->beginEvent(kTRUE); // Set pass raw to kTRUE. This will stay fixed like this forever.
    m_chainItemsL1[ Config::config().getStr(kBlankString) ] = _alwaysPass; // Associate to a blank string

    // ##################################################################################################################
    // STEP TWO: Now we populate the HLT items
    for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
      if (TrigConfInterface::getChainLevel(_i) == 1) continue; // Only HLT chains

      const std::string _chainName = TrigConfInterface::getChainName(_i);
      const std::string _chainGroup = TrigConfInterface::getChainRatesGroupName(_i);
      Int_t _chainPrescale = TrigXMLService::trigXMLService().getPrescale( _chainName );

      RatesChainItem* _chainItemHLT = new RatesChainItem(_chainName, _chainPrescale);
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

    // Crate the global rates counter, this will be the OR of everyting HLT
    CounterRatesUnion* _globalRateCounter = new CounterRatesUnion(0, Config::config().getStr(kRateGlobalHLTString), 0); // Mint new counter
    _globalRateCounter->decorate(kDecPrescale, Config::config().getStr(kMultipleString));
    _globalRateCounter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    _globalRateCounter->decorate(kDecType, "Global");
    (*_counterMap)[Config::config().getStr(kRateGlobalHLTString)] = static_cast<CounterBase*>(_globalRateCounter);

    CounterRatesUnion* _globalL1Counter = new CounterRatesUnion(0, Config::config().getStr(kRateGlobalL1String), 0); // Mint new counter
    _globalL1Counter->decorate(kDecPrescale, Config::config().getStr(kMultipleString));
    _globalL1Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    _globalL1Counter->decorate(kDecType, "Global");
    (*_counterMap)[Config::config().getStr(kRateGlobalL1String)] = static_cast<CounterBase*>(_globalL1Counter);

    // ##################################################################################################################
    // Create the L1 unique rates counters, these will be the OR of everything L1 *EXCEPT* for one chain
    if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
      for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
        if (TrigConfInterface::getChainLevel(_i) != 1) continue; // Only L1 chains

        const UInt_t _chainID = TrigConfInterface::getChainCounter(_i);
        const std::string _chainName = TrigConfInterface::getChainName(_i);

        // Find the ChainItem for this chain 
        ChainItemMapIt_t _it = m_chainItemsL1.find( _chainName );
        if (_it == m_chainItemsL1.end()) {
          Warning("MonitorRates::populateCounterMap","Cannot find HLT item: %s", _chainName.c_str() );
          continue;
        }
        RatesChainItem* _chainItemL1 = _it->second;

        // Construct a string displaying the PS for this chain
        const std::string _prescaleStr = Config::config().getStr(kL1String) + ":" + intToString( _chainItemL1->getPS() );

        const std::string _uniqueName = Config::config().getStr(kRateUniqueString) + _chainName;
        CounterRatesUnion* _uniqueChain = new CounterRatesUnion(0, _uniqueName, _chainID); // Mint new counter
        _uniqueChain->decorate(kDecRatesGroupName, Config::config().getStr(kNoneString));
        _uniqueChain->decorate(kDecPrescale, _prescaleStr);
        _uniqueChain->decorate(kDecType, "UniqueL1");
        _uniqueChain->setGlobalRateCounter(_globalL1Counter);
        (*_counterMap)[_uniqueName] = static_cast<CounterBase*>(_uniqueChain); // Insert into the counterMap

      }
    } // End unique rates

    // ##################################################################################################################
    // Create and add the L1 rates counters
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
      const std::string _prescaleStr = Config::config().getStr(kL1String) + ":" + intToString( _chainItemL1->getPS() );

      // Do add to unique counters
      if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
        for (CounterMapIt_t _itA = _counterMap->begin(); _itA != _counterMap->end(); ++_itA) {
          CounterBaseRates* _uniqueCounter = static_cast<CounterBaseRates*>( _itA->second );
          if ( _uniqueCounter->getStrDecoration(kDecType) != "UniqueL1" ) continue; // I'm not a unique counter - next
          if ( _uniqueCounter->getName() == _uniqueName) continue; // I'm the unique counter for *this* chain - *don't* add me!
          _uniqueCounter->addL1Item( _chainItemL1 );
        }
      }

      CounterRatesChain* _L1Chain = new CounterRatesChain(0, _chainName, _chainID); // Mint new counter
      _L1Chain->decorate(kDecRatesGroupName, Config::config().getStr(kNoneString) );
      _L1Chain->decorate(kDecPrescale, _prescaleStr);
      _L1Chain->decorate(kDecType, "L1");
      _L1Chain->addL1Item( _chainItemL1 ); // Link it to where it'll be getting its pass/fail info
      (*_counterMap)[_chainName] = static_cast<CounterBase*>(_L1Chain); // Insert into the counterMap

      //Add to global L1 counter
      _globalL1Counter->addL1Item( _chainItemL1 );
    }

    // ##################################################################################################################
    // Create the HLT unique rates counters, these will be the OR of everything HLT *EXCEPT* for one chain
    if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
      for (UInt_t _i = 0; _i < TrigConfInterface::getChainN(); ++_i) {
        if (TrigConfInterface::getChainLevel(_i) == 1) continue; // Only HLT chains

        const UInt_t _chainID = TrigConfInterface::getChainCounter(_i);
        const std::string _chainName = TrigConfInterface::getChainName(_i);
        const std::string _chainGroup = TrigConfInterface::getChainRatesGroupName(_i);

        // Find the ChainItem for this chain 
        ChainItemMapIt_t _it = m_chainItemsHLT.find( _chainName );
        if (_it == m_chainItemsHLT.end()) {
          Warning("MonitorRates::populateCounterMap","Cannot find HLT item: %s", _chainName.c_str() );
          continue;
        }
        RatesChainItem* _chainItemHLT = _it->second;

        // Construct a string displaying the PS for this chain
        std::string _L1PSString = Config::config().getStr(kMultipleString);
        if (_chainItemHLT->getLower().size() == 1) {
          _L1PSString = intToString( (*(_chainItemHLT->getLowerStart()))->getPS() );
        }
        const std::string _prescaleStr = Config::config().getStr(kL1String) + ":" + _L1PSString 
          + " " + Config::config().getStr(kHLTString) + ":" + intToString( _chainItemHLT->getPS() );

        const std::string _uniqueName = Config::config().getStr(kRateUniqueString) + _chainName;
        CounterRatesUnion* _uniqueChain = new CounterRatesUnion(0, _uniqueName, _chainID); // Mint new counter
        _uniqueChain->decorate(kDecRatesGroupName, _chainGroup);
        _uniqueChain->decorate(kDecPrescale, _prescaleStr);
        _uniqueChain->decorate(kDecType, "UniqueHLT");
        _uniqueChain->setGlobalRateCounter(_globalRateCounter);
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
        Warning("MonitorRates::populateCounterMap", "Skipping Chain %s. No L1 seed. Unsuported.", _chainName.c_str());
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
      std::string _L1PSString = Config::config().getStr(kMultipleString);
      if (_chainItemHLT->getLower().size() == 1) {
        _L1PSString = intToString( (*(_chainItemHLT->getLowerStart()))->getPS() );
      }
      const std::string _prescaleStr = Config::config().getStr(kL1String) + ":" + _L1PSString 
        + " " + Config::config().getStr(kHLTString) + ":" + intToString( _chainItemHLT->getPS() );

      // ################################################################################################################
      // STEP TWO: Do the unique rate for this chain and the Global rates

      // Each unique chain does the OR of _everything_ *EXCEPT* the chain we're interested in
      // Then at the end it subtracts this rate from the global rate. So we need to add *all* chains *but* this one.

      if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
        for (CounterMapIt_t _itA = _counterMap->begin(); _itA != _counterMap->end(); ++_itA) {
          CounterBaseRates* _uniqueCounter = static_cast<CounterBaseRates*>( _itA->second );
          if ( _uniqueCounter->getStrDecoration(kDecType) != "UniqueHLT" ) continue; // I'm not a unique counter - next
          if ( _uniqueCounter->getName() == _uniqueName) continue; // I'm the unique counter for *this* chain - *don't* add me!
          _uniqueCounter->addHLTItem( _chainItemHLT );
        }
      }

      // Insert into the global rates counter
      _globalRateCounter->addHLTItem( _chainItemHLT );

      // ################################################################################################################
      // STEP THREE: Make a new counter for this HLT Chain

      // And add a new counter to get the rates for this chainItem
      CounterRatesChain* _ratesChain = new CounterRatesChain(0, _chainName, _chainID); // Mint new counter
      _ratesChain->decorate(kDecRatesGroupName, _chainGroup);
      _ratesChain->decorate(kDecPrescale, _prescaleStr);
      _ratesChain->decorate(kDecType, "Chain");
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
        CounterRatesUnion* _ratesGroup = new CounterRatesUnion(0, _chainGroup, 0); // Mint new counter
        _ratesGroup->decorate(kDecPrescale, Config::config().getStr(kMultipleString));
        _ratesGroup->decorate(kDecRatesGroupName, _chainGroup);
        _ratesGroup->decorate(kDecType, "Union");
        (*_counterMap)[_chainGroup] = static_cast<CounterBase*>(_ratesGroup);
      }

    }

    // ##################################################################################################################
    // STEP FIVE: Finally loop over all counters, we want to do the AND of each HLT chain with other chains in its group
    for (CounterMapIt_t _itA = _counterMap->begin(); _itA != _counterMap->end(); ++_itA) {
      // Look for all Chain counters
      CounterBaseRates* _counterA = static_cast<CounterBaseRates*>( _itA->second );
      if ( _counterA->getStrDecoration(kDecType) != "Chain" ) continue; // I'm not a HLT chain counter - next
      
      // Now look at all other counters - find others in my group
      CounterMapIt_t _itB = _itA;
      for (++_itB; _itB != _counterMap->end(); ++_itB) {
        CounterBaseRates* _counterB = static_cast<CounterBaseRates*>( _itB->second );
        if ( _counterB->getStrDecoration(kDecType) != "Chain" ) continue; // I'm not a chain counter - next

        // Same group?
        if ( _counterA->getStrDecoration(kDecRatesGroupName) != _counterB->getStrDecoration(kDecRatesGroupName)) continue;

        // Construct name for overlap counter
        const std::string _name = _counterA->getName() + Config::config().getStr(kAndString) + _counterB->getName();

        // Add new overlap counter!
        CounterRatesIntersection* _overlapCounter = new CounterRatesIntersection(0, _name, 0);
        _overlapCounter->decorate(kDecPrescale, Config::config().getStr(kMultipleString));
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

        // Find the ChainItem for this chain so we can set it's PassedRaw bit for this event, could be L1 or HLT
        ChainItemMapIt_t _it = m_chainItemsHLT.find( _chainName );
        if (_it == m_chainItemsHLT.end()) {
          Warning("MonitorRates::newEvent","Cannot find HLT item: %s", _chainName.c_str() );
          continue;
        }

        RatesChainItem* _chainItem = _it->second;
        _chainItem->beginEvent( m_costData->getIsChainPassedRaw(_c) );
        _chainItemsInEvent.insert( _chainItem );

        if (Config::config().debug()) {
          Info("MonitorRates::newEvent","Storing Decision:%i for HLT ChainItem:%s with PS:%i",
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

        RatesChainItem* _chainItem = _it->second;
        _chainItem->beginEvent( m_costData->getIsL1PassedBeforePrescale(_c) );
        _chainItemsInEvent.insert( _chainItem );


        if (Config::config().debug()) {
          Info("MonitorRates::newEvent","Storing Decision:%i for L1 ChainItem:%s with PS:%i",
            (Int_t)m_costData->getIsL1PassedBeforePrescale(_c),
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

            const std::string _source = _L1->getName() + " [PS:" + intToString(_L1->getPS()) + "]";
            const std::string _target = _L2->getName() + " [PS:" + intToString(_L2->getPS()) + "]";
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

    // const std::string _outDir = Config::config().getStr(kOutputDirectory) + "/xdot";
    // gSystem->mkdir( _outDir.c_str(), kTRUE );

    for (CounterCollectionIt_t _collectionIt = m_counterCollections.begin(); _collectionIt != m_counterCollections.end(); ++_collectionIt) {
      for (CounterMapIt_t _counterMapIt = _collectionIt->second.begin(); _counterMapIt != _collectionIt->second.end(); ++_counterMapIt ) {
        CounterBaseRates* _counter = static_cast<CounterBaseRates*>( _counterMapIt->second );
        const std::string _counterCollectionName = _collectionIt->first;

        _counter->finalise();

        // This is picked up later in TableValueFunctions so needs to be associated directly with a counter
        _counter->decorate(kDecLbLength, m_collectionLumiCollector[ _counterCollectionName ]->getTotalLumiBlockTime());

        // std::string _graph;
        // _graph += "digraph G { ";
        // for (ChainItemSetIt_t _L2It = _counter->getHLTItemSet().begin(); _L2It != _counter->getHLTItemSet().end(); ++_L2It ) {
        //   RatesChainItem* _L2 = (*_L2It);
        //   ChainItemSet_t _exportedL1Names;
        //   _graph += intToString(_L2->getID()) + " [label=\"" + _L2->getName() + "\\nPS:" + intToString(_L2->getPS()) + "\"]; ";
        //   for (ChainItemSetIt_t _L1It = _L2->getLowerStart(); _L1It != _L2->getLowerEnd(); ++_L1It) {
        //     RatesChainItem* _L1 = (*_L1It);
        //     if (_exportedL1Names.count(_L1) == 0) {
        //       _graph += intToString(_L1->getID()) + " [label=\"" + _L1->getName() + "\\nPS:" + intToString(_L1->getPS()) + "\"]; ";
        //     }
        //     _graph += intToString(_L1->getID()) + " -> " + intToString(_L2->getID()) + "; ";
        //   }
        // }
        // _graph += "}";
        // std::string _exec = "echo '" + _graph + "'";
        // _exec += " | dot -Tpdf -o" + _outDir + "/" + _counter->getName() + ".xdot";
        // gSystem->Exec( _exec.c_str() );
      }
    }

    saveRuleBookXML();

    if (Config::config().getInt(kOutputRatesGraph) == kTRUE) saveRateGraphs();

    VariableOptionVector_t _toSavePlots;// = m_dummyCounter->getAllHistograms();

    std::vector<TableColumnFormatter> _toSaveTable;

    _toSaveTable.push_back( TableColumnFormatter("Group", 
      "The group this chain belongs to.",
      kDecRatesGroupName, kSavePerCall, 0, kFormatOptionUseStringDecoration) );

    _toSaveTable.push_back( TableColumnFormatter("Prescale", 
      "The prescale of this chain. Only displayed for simple combinations.",
      kDecPrescale, kSavePerCall, 0, kFormatOptionUseStringDecoration) );

    _toSaveTable.push_back( TableColumnFormatter("Active Events", 
      "Number of events in which the chain - or at least one chain in the combination - was executed at the HLT.",
      kVarEventsPassedRaw, kSavePerCall, 0, kFormatOptionUseEntries) );

    _toSaveTable.push_back( TableColumnFormatter("Passed Raw", 
      "Number of events in which this chain or combination passed. Irrespective of prescale.",
      kVarEventsPassedRaw, kSavePerCall, 0 ) );

    _toSaveTable.push_back( TableColumnFormatter("Active Time [s]", 
      "Integrated length of all lumi blocks which contributed events to this rates prediction.",
      kVarEventsPassedRaw /*ignored*/, kSavePerCall /*ignored*/, 2, kFormatOptionUseWallTime) );

    if (Config::config().getInt(kDirectlyApplyPrescales) == kTRUE) {
      _toSaveTable.push_back( TableColumnFormatter("Pass Direct PS", 
        "Number of events this chain or combination passed after directly applying probabilistic prescales.",
        kVarEventsPassedDP, kSavePerCall, 0 ) );
    }

    _toSaveTable.push_back( TableColumnFormatter("Pass Weighted PS", 
      "Number of events this chain or combination passed after applying prescales as weighting factors.",
      kVarEventsPassed, kSavePerCall, 4 ) );

    if (Config::config().getInt(kDirectlyApplyPrescales) == kTRUE) {
      _toSaveTable.push_back( TableColumnFormatter("Direct PS Rate [Hz]", 
        "Rate after applying all prescales(s) directly.",
        kVarEventsPassedDP, kSavePerCall, 4, kFormatOptionNormaliseWallTime ) );

      // _toSaveTable.push_back( TableColumnFormatter("Direct PS Rate Err [Hz]", 
      //   "Error on rate after applying all prescales(s) directly",
      //   &tableFnRateGetDirectRateErr, 4 ) );
    }

    _toSaveTable.push_back( TableColumnFormatter("Weighted PS Rate [Hz]", 
      "Rate after applying all prescale(s) as weights.",
      kVarEventsPassed, kSavePerCall, 4, kFormatOptionNormaliseWallTime ) );

    _toSaveTable.push_back( TableColumnFormatter("Weighted PS Rate Err [Hz]", 
      "Error on rate after applying all prescale(s) as weights",
      &tableFnRateGetWeightedRateErr, 4 ) );

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


    // Now do the UNIQUE rate
    m_invertFilter = kFALSE;
    Config::config().clearVec(kPatternsOutput);
    Config::config().addVecEntry(kPatternsOutput, "RATE_GLOBAL");
    Config::config().addVecEntry(kPatternsOutput, "UNIQUE_");
    if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
      setName("Rate_Unique");
      sharedTableOutputRoutine( _toSaveTable );
      sharedHistogramOutputRoutine( _toSavePlots );
    }

    // Now do the AND - note we can remove the "Prescale" option from the table.
    m_invertFilter = kFALSE;
    Config::config().clearVec(kPatternsOutput);
    Config::config().addVecEntry(kPatternsOutput, Config::config().getStr(kAndString));
    _toSaveTable.erase( _toSaveTable.begin() + 1);
    setName("Rate_Combination");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );

    // Now do the GROUP - note we can remove the "Group" option from the table.
    m_invertFilter = kFALSE;
    Config::config().clearVec(kPatternsOutput);
    Config::config().addVecEntry(kPatternsOutput, "RATE_");
    Config::config().addVecEntry(kPatternsOutput, "Rate_");
    _toSaveTable.erase( _toSaveTable.begin() );
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
      _xml.addNode(_fout, "trigger");
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
    Float_t _evPassDirect = _counter->getValue(kVarEventsPassedDP, kSavePerCall);
    Float_t _evPassRaw = _counter->getValue(kVarEventsPassedRaw, kSavePerCall);

    Float_t _evRun = _counter->getEntries(kVarEventsPassedRaw, kSavePerCall);
    Float_t _walltime = _counter->getDecoration(kDecLbLength);

    Float_t _rate = _evPassWeight / _walltime;
    Float_t _rateErr = TMath::Sqrt( _rate / _walltime );    // B. Peterson # err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)

    Float_t _eff = 0., _effErr = 0., _psEff = 0., _psEffErr = 0.;
    if (_evRun) {
      _eff = _evPassRaw / _evRun;
      _effErr = (1./_evRun) * TMath::Sqrt( _evPassRaw * (1. - _eff) ); // Binomal
      _psEff = _evPassWeight / _evRun;
      _psEffErr =  (1./_evRun) * TMath::Sqrt( _evPassWeight * (1. - _psEff) ); // Binomal
   }

    _xml.addNode(_fout, "signature");
    _xml.addNode(_fout, "sig_name", _counter->getName());
    _xml.addNode(_fout, "sig_counter", intToString( _counter->getID() ));
    _xml.addNode(_fout, "evts_passed", floatToString( _evPassDirect, _xmlPrecision));
    _xml.addNode(_fout, "evts_passed_weighted", floatToString( _evPassWeight, _xmlPrecision));
    _xml.addNode(_fout, "rate", floatToString( _rate, _xmlPrecision));
    _xml.addNode(_fout, "rate_err", floatToString( _rateErr, _xmlPrecision));
    _xml.addNode(_fout, "prescale", intToString( _counter->getBasicPrescale() ));
    if (_counter->getStrDecoration(kDecType) != "L1") {
      _xml.addNode(_fout, "lower_chain_name", TrigConfInterface::getLowerChainName(_counter->getName()) );
    } else {
      _xml.addNode(_fout, "lower_chain_name", "" );
    }
    _xml.addNode(_fout, "efficiency", floatToString( _eff, _xmlPrecision));
    _xml.addNode(_fout, "efficiency_err", floatToString( _effErr, _xmlPrecision));
    _xml.addNode(_fout, "prescaled_efficiency", floatToString( _psEff, _xmlPrecision));
    _xml.addNode(_fout, "prescaled_efficiency_err", floatToString( _psEffErr, _xmlPrecision));
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
