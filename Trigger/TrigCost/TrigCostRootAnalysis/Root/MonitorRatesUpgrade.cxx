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
// ROOT includes
#include <TSystem.h>
#include <TXMLEngine.h>
#include <TObjString.h>
#include <TDOMParser.h>
#include <TXMLNode.h>
#include <TXMLDocument.h>
#include <TXMLAttr.h>
#include <TError.h>
#include <TMath.h>

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

  /**
   * Monitor constructor. Sets name and calls base constructor.
   * This monitor will be used to emulate the future trigger setup
   * It will simulate L0, L1 and HLT rates using Run2 EB data as input
   * It will apply efficiencies to simulate expected rates yeilds
   */
  MonitorRatesUpgrade::MonitorRatesUpgrade(const TrigCostData* _costData)
    : MonitorBase(_costData, "RateUpgrade"),
      m_unbiasedWeight(0),
      m_unbiasedWeightRound(0)
  {
    m_dummyCounter = static_cast<CounterBase*>(new CounterRatesChain(_costData, Config::config().getStr(kDummyString), INT_MIN));
    m_globalRateL1Counter = nullptr;
    m_globalRateL2Counter = nullptr;
    m_globalRateL3Counter = nullptr;
    m_scenario = Config::config().getStr(kUpgradeScenario);
    m_targetPairedBCID = 0.;
    m_targetPeakMuAv = 0;


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
   * We need extra information which is kept in upgrade XMLs. These share similarity with standard rate XMLs but have additional fields
   */
  void MonitorRatesUpgrade::parseUpgradeXML() {

    const std::string _file = std::string("upgrade_") + m_scenario + std::string("_menu.xml");
    std::string _path;

    if (Config::config().getInt(kIsRootCore) == kTRUE) {
      _path = std::string( Config::config().getStr(kDataDir) + _file );
    } else {
// CAUTION - "ATHENA ONLY" CODE
#ifndef ROOTCORE
      _path = PathResolverFindDataFile( _file );
#endif // not ROOTCORE
    }

    // If file exists
    TXMLEngine* _xml = new TXMLEngine();
    XMLDocPointer_t _xmlDoc = 0;
    std::string _level = "none";

    if (_path != Config::config().getStr(kBlankString) && access( _path.c_str(), F_OK ) != -1 ) {
      _xmlDoc = _xml->ParseFile( _path.c_str() );
    }

    if (_xmlDoc == 0) {
      Error("MonitorRatesUpgrade::parseUpgradeXML","Cannot find upgrade XML %s, critical error - must be supplied. Use --upgradeScenario.", _file.c_str() );
      delete _xml;
      abort();
    }

    XMLNodePointer_t _mainNode = _xml->DocGetRootElement(_xmlDoc);
    assert( _xml->GetNodeName(_mainNode) == std::string("trigger") );
    XMLNodePointer_t _listNode = _xml->GetChild( _mainNode );

    while ( _listNode != 0 ) { // Loop over all menu elements
      const std::string _listName = _xml->GetNodeName(_listNode);
      if (_listName == "lumivalues") {
        XMLNodePointer_t _node = _xml->GetChild( _listNode );
        while( _node != 0) {
          if ( _xml->GetNodeName(_node) == std::string("target_paired_bcid") ) {
            m_targetPairedBCID = stringToFloat( _xml->GetNodeContent(_node) );
          } else if ( _xml->GetNodeName(_node) == std::string("target_peak_mu_av") ) {
            m_targetPeakMuAv = stringToFloat( _xml->GetNodeContent(_node) );
          }
          _node = _xml->GetNext(_node);
        }
      } else if (_listName == "level") {
        XMLNodePointer_t _node = _xml->GetChild( _listNode );
        while (_node != 0) {
          if( _xml->GetNodeName(_node) == std::string("lvl_name") ) {
            _level = _xml->GetNodeContent( _node );
            _node = _xml->GetNext( _node );
          }
          assert( _xml->GetNodeName(_node) == std::string("signature") );
          XMLNodePointer_t _detailsNode = _xml->GetChild( _node );
          std::string _sigName = "", _l0Proxy = "", _group = "", _comment = "", _hltProxy = "", _lower = "";
          Float_t _l0ReductionFactor = 1., _l1ReductionFactor = 1., _hltReductionFactor = 1.;

          while( _detailsNode != 0) {
            if (_xml->GetNodeContent(_detailsNode) == nullptr) { // Avoid NULL
              _detailsNode = _xml->GetNext(_detailsNode);
              continue;
            }
            const std::string _detail = _xml->GetNodeName(_detailsNode);
            if (_detail == "sig_name") _sigName = _xml->GetNodeContent(_detailsNode);
            else if (_detail == "l0_proxy") _l0Proxy = _xml->GetNodeContent(_detailsNode);
            else if (_detail == "hlt_proxy") _hltProxy = _xml->GetNodeContent(_detailsNode);
            else if (_detail == "group") _group = _xml->GetNodeContent(_detailsNode);
            else if (_detail == "comment") _comment = _xml->GetNodeContent(_detailsNode);
            else if (_detail == "lower_chain_name") _lower = _xml->GetNodeContent(_detailsNode);
            else if (_detail == "l0_reduction_factor") _l0ReductionFactor = stringToFloat( _xml->GetNodeContent(_detailsNode) );
            else if (_detail == "l1_reduction_factor") _l1ReductionFactor = stringToFloat( _xml->GetNodeContent(_detailsNode) );
            else if (_detail == "hlt_reduction_factor") _hltReductionFactor = stringToFloat( _xml->GetNodeContent(_detailsNode) );
            else Error("MonitorRatesUpgrade::parseUpgradeXML", "Unknown XML detail: %s", _detail.c_str());
            _detailsNode = _xml->GetNext(_detailsNode);
          }
          if (_level == "L0") {
            Info("MonitorRatesUpgrade::parseUpgradeXML","Will do L0/L1 chain: %s with proxy %s", _sigName.c_str(), _l0Proxy.c_str());
            m_upgradeChains.insert( ChainInfo(_sigName, 0, _l0Proxy, _group, _comment, _l0ReductionFactor, _l1ReductionFactor) );
          } else if (_level == "HLT") {
            m_upgradeChains.insert( ChainInfo(_sigName, 2, _hltProxy, _lower, _comment, _hltReductionFactor) );
          } else {
            Error("MonitorRatesUpgrade::parseUpgradeXML", "Signatures from unknown trigger level: %s", _level.c_str());
          }
          _node = _xml->GetNext(_node);
        }

      }

      _listNode = _xml->GetNext(_listNode);
    }

    delete _xml;
  }

  /**
   * Here we create a RatesChainItem for each trigger item at each level, it is these which will be loaded
   * per event with the raw decision of that trigger item and the counters will use the information of the L1
   * and HLT chain items given to them to calculate their event weight.
   */
  void MonitorRatesUpgrade::populateChainItemMaps() {

    parseUpgradeXML();

    // We scale up unbiased events due to pileup. But by how much?
    if (Config::config().getInt(kNoUpgradePileupScaling) == kFALSE) {
      Float_t _collidingBunchFactor = m_targetPairedBCID / TrigXMLService::trigXMLService().getBunchGroupSize(1); // Going to continue to assume that 1 is PAIRED
      Float_t _pileupFactor = m_targetPeakMuAv / Config::config().getFloat(kOnlinePeakMuAverage); // Should have been set by run XML
      m_unbiasedWeight = _collidingBunchFactor * _pileupFactor;
      m_unbiasedWeightRound = (Int_t) (m_unbiasedWeight + 0.5); // Round to nearest int (technical reasons)
      Info("MonitorRatesUpgrade::populateChainItemMaps","Unbiased events will get x%i extra rate. From x%.2f more BCIDs and x%.2f more pileup.",
        m_unbiasedWeightRound, _collidingBunchFactor, _pileupFactor);
    } else {
      m_unbiasedWeightRound = 0;
      Warning("MonitorRatesUpgrade::populateChainItemMaps","Upgrade rates will not be scaled for extra pileup.");
    }


    for (const auto _item : m_upgradeChains) {
      if (_item.m_level == 2) {
        continue; // not doing HLT yet
      }
      const std::string _nameL1 = _item.m_name;
      Double_t _chainPrescale = TrigXMLService::trigXMLService().getPrescale( _item.m_proxy );
      RatesChainItem* _L1 = new RatesChainItem(_nameL1, /*chainLevel=*/ 1, _chainPrescale);
      _L1->setRateReductionFactor( _item.m_weight0 );
      m_chainItemsL1[ _nameL1 ] = _L1;

      std::string _nameL2 = _item.m_l1name;
      RatesChainItem* _L2 = new RatesChainItem(_nameL2, /*chainLevel=*/ 2, 1.);
      m_chainItemsL2[ _nameL2 ] = _L2;
      _L2->setRateReductionFactor( _item.m_weight1 );

      _L2->addLower( _L1 );
      _L1->addUpper( _L2 );
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
    (*_counterMap)[Config::config().getStr(kRateGlobalHLTString)] = static_cast<CounterBase*>(m_globalRateL3Counter);

    // Crate the global L1 counter, this will be the OR of everything L1
    m_globalRateL2Counter = new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalL1String), 0, 10, (MonitorBase*)this); // Mint new counter
    m_globalRateL2Counter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateL2Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateL2Counter->decorate(kDecPrescaleValOnlineL1, (Float_t)0);
    m_globalRateL2Counter->decorate(kDecType, "Union");
    (*_counterMap)[Config::config().getStr(kRateGlobalL1String)] = static_cast<CounterBase*>(m_globalRateL2Counter);

    // Crate the global L1 counter, this will be the OR of everything L0
    m_globalRateL1Counter = new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalL0String), 0, 10, (MonitorBase*)this); // Mint new counter
    m_globalRateL1Counter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateL1Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateL1Counter->decorate(kDecPrescaleValOnlineL1, (Float_t)0);
    m_globalRateL1Counter->decorate(kDecType, "Union");
    (*_counterMap)[Config::config().getStr(kRateGlobalL0String)] = static_cast<CounterBase*>(m_globalRateL1Counter);
  }

  /**
   * The starting point is to define what items we're interested at L0, and to map current L1 items to these
   */
  void MonitorRatesUpgrade::createL1Counters(CounterMap_t* _counterMap) {
    for (const auto _item : m_upgradeChains) {
      if (_item.m_level == 2) continue; // not doing HLT here

      const std::string _chainName = _item.m_name;
      const UInt_t _chainID = TrigConfInterface::getCtpId(_item.m_proxy);

      //Info("MonitorRatesUpgrade::createL1Counters","Create counter: %s", _chainName.c_str() );

      // Find the ChainItem for this chain
      ChainItemMapIt_t _it = m_chainItemsL1.find( _chainName );
      if (_it == m_chainItemsL1.end()) {
        Warning("MonitorRatesUpgrade::createL1Counters","Cannot find L1 item: %s", _chainName.c_str() );
        continue;
      }
      RatesChainItem* _chainItemL1 = _it->second;

      const std::string _prescaleStr = Config::config().getStr(kL0String) + ":" + doubleToString( _chainItemL1->getPS(), 2 );
      CounterRatesChain* _L1Chain = new CounterRatesChain(m_costData, _chainName, _chainID, 10, (MonitorBase*)this); // Mint new counter
      _L1Chain->decorate(kDecRatesGroupName, Config::config().getStr(kNoneString) );
      _L1Chain->decorate(kDecPrescaleStr, _prescaleStr);
      _L1Chain->decorate(kDecType, "L0");
      _L1Chain->addL1Item( _chainItemL1 ); // Link it to where it'll be getting its pass/fail info
      (*_counterMap)[_chainName] = static_cast<CounterBase*>(_L1Chain); // Insert into the counterMap


      //Add to global L1 counter
      m_globalRateL1Counter->addL1Item( _chainItemL1 );

      // Do group
      const std::string _group = _item.m_group + std::string("_L0");
      CounterMapIt_t _findGroup = _counterMap->find( _group ); // Do we have a counter for this group?
      if ( _findGroup != _counterMap->end() ) { // We do have a group already! Add to it.
        (static_cast<CounterRatesUnion*>( _findGroup->second ))->addL1Item( _chainItemL1 );
      } else {
        // We need a new group counter, this should be of type Union
        CounterRatesUnion* _ratesGroup = new CounterRatesUnion(m_costData, _group, 0, 10, (MonitorBase*)this); // Mint new counter
        _ratesGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
        _ratesGroup->decorate(kDecPrescaleVal, (Float_t)0.);
        _ratesGroup->decorate(kDecRatesGroupName, _group);
        _ratesGroup->decorate(kDecType, "Union");
        _ratesGroup->addL1Item( _chainItemL1 ); // Add initial counter
        (*_counterMap)[_group] = static_cast<CounterBase*>(_ratesGroup); // Instert into the map
      }
    }
  }

  /**
   * The starting point is to define what items we're interested at L0, and to map current L1 items to these
   */
  void MonitorRatesUpgrade::createL2Counters(CounterMap_t* _counterMap) {
    for (const auto _item : m_upgradeChains) {
      if (_item.m_level == 2) continue; // not doing HLT here

      std::string _chainName = _item.m_l1name;
      const UInt_t _chainID = TrigConfInterface::getCtpId(_item.m_proxy);

      // Find the ChainItem for this chain
      ChainItemMapIt_t _it = m_chainItemsL2.find( _chainName );
      if (_it == m_chainItemsL2.end()) {
        Warning("MonitorRatesUpgrade::createL2Counters","Cannot find L1 item: %s", _chainName.c_str() );
        continue;
      }
      RatesChainItem* _chainItemL2 = _it->second;

      const std::string _prescaleStr = Config::config().getStr(kL1String) + ":" + doubleToString( _chainItemL2->getPS(), 2 );
      CounterRatesChain* _L2Chain = new CounterRatesChain(m_costData, _chainName, _chainID, 10, (MonitorBase*)this); // Mint new counter
      _L2Chain->decorate(kDecRatesGroupName, Config::config().getStr(kNoneString) );
      _L2Chain->decorate(kDecPrescaleStr, _prescaleStr);
      _L2Chain->decorate(kDecType, "L1");
      _L2Chain->addL2Item( _chainItemL2 ); // Link it to where it'll be getting its pass/fail info
      (*_counterMap)[_chainName] = static_cast<CounterBase*>(_L2Chain); // Insert into the counterMap

      //Info("MonitorRatesUpgrade::createL2Counters","Create counter: %s", _chainName.c_str() );

      //Add to global L1 counter
      m_globalRateL2Counter->addL2Item( _chainItemL2 );

      // Do group
      const std::string _group = _item.m_group + std::string("_L1");
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
        _ratesGroup->addL1Item( _chainItemL2 ); // Add initial counter
        (*_counterMap)[_group] = static_cast<CounterBase*>(_ratesGroup); // Instert into the map
      }
    }
  }


  void MonitorRatesUpgrade::createL3Counters(CounterMap_t* _counterMap) {

    UNUSED(_counterMap);
  }

  /**
   * Process new event for this monitor.
   * For the chain rate monitor - we look at a chain's rate and its correlation with other chains in its group
   * @param _weight The event weight.
   */
  void MonitorRatesUpgrade::newEvent(Float_t _weight) {
    m_timer.start();
    if ( Config::config().debug() ) Info("MonitorRatesUpgrade::newEvent", "*** Processing Chain Rates ***");

    // First time? Setup the ChainItem maps to hold the decision and prescale information for the rates counters to use.
    if (m_chainItemsL1.size() == 0) populateChainItemMaps();

    //Now loop over the counter collections;
    for (CounterMapSetIt_t _cmsIt = m_collectionsToProcess.begin(); _cmsIt != m_collectionsToProcess.end(); ++_cmsIt) {
      CounterMap_t* _counterMap = *_cmsIt; // This counter map holds all the counters. But we don't really want to process every one
      CounterBaseRatesSet_t _inEventCounterMap; // This set contains all the counters we actually do need to iterate over
      ChainItemSet_t _chainItemsInEvent;

      // If the counter map is empty, then we need to populate it. We will pre-load a counter for every chain.
      if (_counterMap->size() == 0) populateCounterMap(_counterMap);

      // Fill into L1 items
      for (const auto _item : m_upgradeChains) {
        if (_item.m_level == 2) continue; // not doing HLT here

        const Int_t _location = m_costData->getL1Location(_item.m_proxy);
        if (_location == -1) continue; // Chain was not in the event
        ChainItemMapIt_t _itL1 = m_chainItemsL1.find( _item.m_name );
        ChainItemMapIt_t _itL2 = m_chainItemsL2.find( _item.m_l1name );
        if (_itL1 == m_chainItemsL1.end()) {
          Warning("MonitorRatesUpgrade::newEvent","Cannot find L1 item: %s",  _item.m_name.c_str() );
          continue;
        }
        const Bool_t _desicison = m_costData->getIsL1PassedBeforePrescale(_location);
        RatesChainItem* _chainItemL1 = _itL1->second;
        _chainItemL1->beginEvent( _desicison,  _inEventCounterMap );
        _chainItemsInEvent.insert( _chainItemL1 );
        RatesChainItem* _chainItemL2 = _itL2->second;
        _chainItemL2->beginEvent( _desicison,  _inEventCounterMap );
        _chainItemsInEvent.insert( _chainItemL2 );
      }

      // If we are debugging such that chains always pass - then we need to re-roll everyone's random PS numbers.
      // This can be done anyway, but it normally not necessary.
      if (Config::config().getInt(kRatesForcePass) == kTRUE) {
        for (const auto _chainItem : m_chainItemsL1) _chainItem.second->newRandomPS();
        for (const auto _chainItem : m_chainItemsL2) _chainItem.second->newRandomPS();
        for (const auto _chainItem : m_chainItemsL3) _chainItem.second->newRandomPS();
      }

      // ##################################################################################################################
      // PASS TWO: Now loop over all counters, they will use their pre-linked RatesChainItems to get their weights.
      CounterMapIt_t _it = _counterMap->begin();
      const UInt_t _isUnbiased = Config::config().getInt(kCurrentEventWasRandomOnline);
      for (; _it != _counterMap->end(); ++_it) {
        _it->second->processEventCounter(_isUnbiased, m_unbiasedWeightRound, _weight);
      }

      // ##################################################################################################################
      // PASS THREE: Now we just need to reset the ChainItems which were included in the event.
      for (ChainItemSetIt_t _setIt = _chainItemsInEvent.begin(); _setIt != _chainItemsInEvent.end(); ++_setIt) {
        (*_setIt)->endEvent();
      }
      _chainItemsInEvent.clear();
      m_countersInEvent.clear();

    }
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
    setName("Rate_Upgrade_ChainL0");
    filterOutputOnStrDecoration(kDecType, "L0");
    sharedTableOutputRoutine( _toSaveTable );
    sharedHistogramOutputRoutine( _toSavePlots );

    setName("Rate_Upgrade_ChainL1");
    filterOutputOnStrDecoration(kDecType, "L1");
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

  void MonitorRatesUpgrade::saveRuleBookXML() {

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
        Info("MonitorRatesUpgrade::saveRuleBookXML","Doing XML output with path %s.", _xmlName.c_str());
      }

      XMLExport _xml(_fout);
      _xml.setSpaces(2);
      _xml.addNode(_fout, "trigger");

      // Do General Info
      _xml.addNode(_fout, "Xsection", intToString(0)); // TODO

      Float_t _runLumi = 0.;
      if ( Config::config().getIsSet(kRunLumi) ) _runLumi = Config::config().getFloat(kRunLumi);
      else if ( Config::config().getIsSet(kRunLumiXML) ) _runLumi = Config::config().getFloat(kRunLumiXML);
      _xml.addNode(_fout, "Luminosity", floatToString(_runLumi) );

      _xml.addNode(_fout, "GenEff", intToString(0)); // TODO
      _xml.addNode(_fout, "n_evts", intToString(Config::config().getInt(kEventsProcessed)) );

      Float_t _predictionLumi = 0.;
      if ( Config::config().getIsSet(kPredictionLumi) ) _predictionLumi = Config::config().getFloat(kPredictionLumi);
      else if ( Config::config().getIsSet(kPredictionLumiMenuXML) ) _predictionLumi = Config::config().getFloat(kPredictionLumiMenuXML);
      else if ( Config::config().getIsSet(kPredictionLumiRunXML) ) _predictionLumi = Config::config().getFloat(kPredictionLumiRunXML);
      _xml.addNode(_fout, "PredictionLumi", floatToString(_predictionLumi) );

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
      // Currently we prefer XML
      if ( TrigXMLService::trigXMLService().getParsedRunXML() == kTRUE ) {
        _xml.addNode(_fout, "bunchgroup");
        for (Int_t _bg = 0; _bg < TrigXMLService::trigXMLService().getNBunchGroups(); ++_bg) {
          _xml.addNode(_fout, "bunchgrouptype");
          _xml.addNode(_fout, "bunchgroup_keynum", intToString(_bg) );
          _xml.addNode(_fout, "bunchgroup_key", TrigXMLService::trigXMLService().getBunchGroupName(_bg) );
          _xml.addNode(_fout, "bunchgroup_size", intToString(TrigXMLService::trigXMLService().getBunchGroupSize(_bg)) );
          _xml.endNode(_fout); // bunchgrouptype
        }
        _xml.endNode(_fout); //bunchgroup
      } else {
          // Otherwise try from ntuple (this is broken 06/15) TODO fix in athena
        StringIntMap_t _bunchGroups = TrigConfInterface::getBunchGroupSetup();
        UInt_t _bgCounter = 0;
        _xml.addNode(_fout, "bunchgroup");
        for (StringIntMapIt_t _it = _bunchGroups.begin(); _it != _bunchGroups.end(); ++_it) {
          _xml.addNode(_fout, "bunchgrouptype");
          _xml.addNode(_fout, "bunchgroup_keynum", intToString(_bgCounter++) );
          _xml.addNode(_fout, "bunchgroup_key", _it->first );
          _xml.addNode(_fout, "bunchgroup_size", intToString(_it->second) );
          _xml.endNode(_fout); // bunchgrouptype
        }
        _xml.endNode(_fout); //bunchgroup
      }

      _xml.addNode(_fout, "level");
      //Add L0 data
      _xml.addNode(_fout, "lvl_name", "L0");
      for (CounterMapIt_t _counterMapIt = _colIt->second.begin(); _counterMapIt != _colIt->second.end(); ++_counterMapIt ) {
        CounterBaseRates* _counter = static_cast<CounterBaseRates*>( _counterMapIt->second );
        if (_counter->getStrDecoration(kDecType) != "L0") continue;
        saveXMLElement(_fout, _xml, _counter);
      }
      _xml.endNode(_fout); //level
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
  void MonitorRatesUpgrade::saveXMLElement(std::ofstream& _fout, XMLExport& _xml, CounterBaseRates* _counter) {
    const UInt_t _xmlPrecision = 7;

    Float_t _evPassWeight = _counter->getValue(kVarEventsPassed, kSavePerCall);
    Float_t _evPassWeightErr = _counter->getValueError(kVarEventsPassed, kSavePerCall); // Get sqrt(sumW2)

    Float_t _evRun = _counter->getValue(kVarEventsRun, kSavePerCall); // EB Weighted
    Float_t _evPassNoPS = _counter->getValue(kVarEventsPassedNoPS, kSavePerCall); // EB Weighted

    Float_t _evPassRawStat = _counter->getValue(kVarEventsPassRawStat, kSavePerCall); // Not EB Weighted
    //Float_t _evRunRawStat  = _counter->getValue(kVarEventsRunRawStat,  kSavePerCall); // Not EB Weighted

    Float_t _walltime = _counter->getDecoration(kDecLbLength);

    Float_t _rate = _evPassWeight / _walltime;
    Float_t _rateErr = _evPassWeightErr / _walltime;    // err = sqrt(events in time T)/T = sqrt(rate*T/T^2) = sqrt(rate/T)

    Float_t _uniqueRate = _counter->getDecoration(kDecUniqueRate);
    Float_t _uniqueFraction = _counter->getDecoration(kDecUniqueFraction); // Was in percent, change back to 0-1

    Bool_t _isL1 = kFALSE;
    if (_counter->getStrDecoration(kDecType) == "L1") _isL1 = kTRUE;
    UNUSED(_isL1);

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
    _xml.addNode(_fout, "passthrough", "0" );
    _xml.addNode(_fout, "lower_chain_name", "" );
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
  CounterBase* MonitorRatesUpgrade::newCounter(  const std::string &_name, Int_t _ID  ) {
    UNUSED( _name );
    UNUSED( _ID );
    Error("MonitorRatesUpgrade::newCounter","For Rates counters - please mint them yourself as the derived type is important (Chain, Union, Intersection etc.)");
    return 0;
  }


} // namespace TrigCostRootAnalysis
