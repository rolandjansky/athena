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
  MonitorRates::MonitorRates(const TrigCostData* costData) : MonitorBase(costData, "Rate") {
    m_dummyCounter =
      static_cast<CounterBase*>(new CounterRatesChain(costData, Config::config().getStr(kDummyString), INT_MIN));
    m_globalRateHLTCounter = nullptr;
    m_globalRateL1Counter = nullptr;
    m_globalRatePhysicsMainCounter = nullptr;
    m_doingOverlaps = (Bool_t) (Config::config().getInt(kDoAllOverlap)
                                || Config::config().getInt(kDoGroupOverlap)
                                || Config::config().getVecSize(kPatternsOverlap) > 0);
    if (m_doingOverlaps) Info("MonitorRates::MonitorRates", "Chain overlap monitoring is enabled for this processing.");

    m_doCPS = (Bool_t) Config::config().getInt(kDoCPS);
  }

  /**
   * Destroy this monitor - note we have special RatesChainItems to destroy here too
   */
  MonitorRates::~MonitorRates() {
    for (ChainItemMapIt_t it = m_chainItemsL1.begin(); it != m_chainItemsL1.end(); ++it) {
      delete it->second;
    }
    for (ChainItemMapIt_t it = m_chainItemsHLT.begin(); it != m_chainItemsHLT.end(); ++it) {
      delete it->second;
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
    for (UInt_t i = 0; i < TrigConfInterface::getChainN(); ++i) {
      if (TrigConfInterface::getChainLevel(i) != 1) continue; // Only L1 chains

      const std::string chainName = TrigConfInterface::getChainName(i);
      Double_t chainPrescale = TrigXMLService::trigXMLService().getPrescale(chainName);

      // Check for explicit exclusion
      if (Config::config().getVecMatches(kPatternsExclude, chainName) == kTRUE) {
        if (!isZero(chainPrescale + 1)) {
          Warning("MonitorRates::populateChainItemMaps",
                  "Force-disabling chain %s (PS:%.2f) in rates prediction. New PS:-1.",
                  chainName.c_str(), chainPrescale);
        }
        chainPrescale = -1.;
      }

      m_chainItemsL1[ chainName ] = new RatesChainItem(chainName, /*chainLevel=*/ 1, chainPrescale,
                                                        /*express prescale=*/ 1.);
    }
    // We have one extra one, AlwaysPass - which will be used for HLT chains with no L1 seed
    RatesChainItem* alwaysPass = new RatesChainItem(Config::config().getStr(kAlwaysPassString), 1, 1.); // Set level to
                                                                                                         // 1, PS to 1
    CounterBaseRatesSet_t dummy;
    alwaysPass->beginEvent(kTRUE, dummy); // Set pass raw to kTRUE. This will stay fixed like this forever. Second
                                            // param is ignored
    m_chainItemsL1[ Config::config().getStr(kBlankString) ] = alwaysPass; // Associate to a blank string

    // ##################################################################################################################
    // STEP TWO: Now we populate the HLT items
    for (UInt_t i = 0; i < TrigConfInterface::getChainN(); ++i) {
      if (TrigConfInterface::getChainLevel(i) == 1) continue; // Only HLT chains

      const std::string chainName = TrigConfInterface::getChainName(i);
      const std::string chainCPSGroup = TrigConfInterface::getChainCPSGroup(i);
      Double_t chainPrescale = TrigXMLService::trigXMLService().getPrescale(chainName);

      Double_t expressPS = 0.;
      if (TrigXMLService::trigXMLService().hasExpressPrescaleInfo()) {
        expressPS = TrigXMLService::trigXMLService().getExpressPrescaleInfo(chainName);
      }

      // Check for explicit exclusion
      if (Config::config().getVecMatches(kPatternsExclude, chainName) == kTRUE) {
        if (!isZero(chainPrescale + 1)) {
          Warning("MonitorRates::populateChainItemMaps",
                  "Force-disabling chain %s (PS:%.2f) in rates prediction. New PS:-1.",
                  chainName.c_str(), chainPrescale);
        }
        chainPrescale = -1.;
      }

      RatesChainItem* chainItemHLT = new RatesChainItem(chainName, 2, chainPrescale, expressPS);
      m_chainItemsHLT[ chainName ] = chainItemHLT;

      // Is this a CPS chain? Needs to go in an additional group if so.
      if (m_doCPS == kTRUE && chainCPSGroup != "") {
        if (m_cpsGroups.count(chainCPSGroup) == 0) m_cpsGroups[ chainCPSGroup ] = new RatesCPSGroup(chainCPSGroup);
        m_cpsGroups[ chainCPSGroup ]->add(chainItemHLT);
      }

      // Now we link the seeding.
      // ################################################################################################################
      // STEP THREE: The HLT chain may be seeded by many L1 items. Need to link them all together
      const std::string L1Name = TrigConfInterface::getLowerChainName(chainName);
      std::vector<std::string> L1NameVector;
      if (L1Name.find(",") != std::string::npos) { // Multiple seeds
        //Get list of individual items.
        std::istringstream ss(L1Name);
        std::string item;
        while (std::getline(ss, item, ',')) {
          // Remove any leading space
          if (item.substr(0, 1) == " ") item = item.substr(1, item.length());
          L1NameVector.push_back(item);
        }
      } else { // Only one seed
        L1NameVector.push_back(L1Name);
      }

      for (UInt_t i = 0; i < L1NameVector.size(); ++i) {
        ChainItemMapIt_t it = m_chainItemsL1.find(L1NameVector.at(i));
        if (it != m_chainItemsL1.end()) {
          RatesChainItem* chainItemL1 = it->second;
          chainItemHLT->addLower(chainItemL1);
          chainItemL1->addUpper(chainItemHLT);
        } else {
          Warning("MonitorRates::populateChainItemMaps", "Cannot find HLT item %s's L1 seeding chain: %s",
                  chainName.c_str(), L1NameVector.at(i).c_str());
        }
      }
    }

    for (const auto chainItem : m_chainItemsHLT) chainItem.second->classifyLumiAndRandom();
    for (const auto chainItem : m_chainItemsL1) chainItem.second->classifyLumiAndRandom();

    if (Config::config().getVecSize(kListOfNoLumiWeightChains) > 0) {
      Info("MonitorRates::populateChainItemMaps",
           "Special cases, these chains will get no luminosity extrapolation: %s",
           Config::config().getStr(kListOfNoLumiWeightChains).c_str());
    }
    if (Config::config().getVecSize(kListOfNoMuLumiWeightChains) > 0) {
      Info("MonitorRates::populateChainItemMaps",
           "Special cases, these chains will only get lumi extrapolation based on number of bunches (not <mu>): %s",
           Config::config().getStr(kListOfNoMuLumiWeightChains).c_str());
    }
    if (Config::config().getVecSize(kListOfNoBunchLumiWeightChains) > 0) {
      Info("MonitorRates::populateChainItemMaps",
           "Special cases, these chains will only get lumi extrapolation based on change in <mu> (not change in bunches): %s",
           Config::config().getStr(kListOfNoBunchLumiWeightChains).c_str());
    }
    if (Config::config().getVecSize(kListOfExpoMuLumiWeightChains) > 0) {
      Info("MonitorRates::populateChainItemMaps",
           "Special cases, these chains will get exponential extrapolation in <mu>: %s",
           Config::config().getStr(kListOfExpoMuLumiWeightChains).c_str());
    }

    // Get the common factor of all the CPS groups
    for (const auto cpsGroup : m_cpsGroups) cpsGroup.second->calculateCPSFactor();
  }

  /**
   * In every other monitor, we make a new counter the first time we encounter it in the menu.
   * Rates are different, as some of the rates we want to monitor are the combinations of items in the
   * menu. Hence we pre-populate the counterMap with all combinations that we are interested in for the run.
   *
   * We make one counter per chain (L1 and HLT), one per intersection of each chain and other chains in its group
   * and one for the union of all chains in each group.
   */
  void MonitorRates::populateCounterMap(CounterMap_t* counterMap) {
    createGlobalCounters(counterMap);
    createL1Counters(counterMap);
    createHLTCounters(counterMap);
    createCPSGroupCounters(counterMap);
    createGroupCounters(counterMap);
    if (m_doingOverlaps == kTRUE) createOverlapCounters(counterMap);
  }

  void MonitorRates::createGlobalCounters(CounterMap_t* /*counterMap*/) {
    // Crate the global rates counter, this will be the OR of everything HLT
    m_globalRateHLTCounter =
      new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalHLTString), 0, 0, (MonitorBase*) this); // Mint
                                                                                                                   // new
                                                                                                                   // counter
    m_globalRateHLTCounter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateHLTCounter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateHLTCounter->decorate(kDecPrescaleValOnlineL1, (Float_t) 0);
    m_globalRateHLTCounter->decorate(kDecComment, "");
    m_globalRateHLTCounter->decorate(kDecType, "Union");

    // I will be the OR of everything which has a stream tag of Main
    m_globalRatePhysicsMainCounter =
      new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalPhysicsMainString), 0, 0,
                            (MonitorBase*) this); // Mint new counter
    m_globalRatePhysicsMainCounter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRatePhysicsMainCounter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRatePhysicsMainCounter->decorate(kDecPrescaleValOnlineL1, (Float_t) 0);
    m_globalRatePhysicsMainCounter->decorate(kDecComment, "");
    m_globalRatePhysicsMainCounter->decorate(kDecType, "Union");

    // Crate the global L1 counter, this will be the OR of everything L1
    m_globalRateL1Counter =
      new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalL1String), 0, 0, (MonitorBase*) this); // Mint
                                                                                                                  // new
                                                                                                                  // counter
    m_globalRateL1Counter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateL1Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateL1Counter->decorate(kDecPrescaleValOnlineL1, (Float_t) 0);
    m_globalRateL1Counter->decorate(kDecComment, "");
    m_globalRateL1Counter->decorate(kDecType, "Union");
    m_chainItemsL1[ Config::config().getStr(kBlankString) ]->setProxy(m_globalRateL1Counter);
  }

  void MonitorRates::createL1Counters(CounterMap_t* counterMap) {
    // ##################################################################################################################
    // Create the L1 unique rates counters, these will be the OR of everything L1 *EXCEPT* for one chain
    // Note we don't assign any L1 items to the counters in this loop. We're just minting the counters here.
    if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
      for (UInt_t i = 0; i < TrigConfInterface::getChainN(); ++i) {
        if (TrigConfInterface::getChainLevel(i) != 1) continue; // Only L1 chains

        const UInt_t chainID = TrigConfInterface::getChainCounter(i);
        const std::string chainName = TrigConfInterface::getChainName(i);

        if (checkPatternUnique(chainName, kFALSE) == kFALSE) continue;
        // In the list of unique counters? Does not get inverted
        if (checkPatternNameMonitor(chainName, m_invertFilter) == kFALSE) continue;

        const std::string uniqueName = Config::config().getStr(kRateUniqueString) + chainName;
        CounterRatesUnion* uniqueChain = new CounterRatesUnion(m_costData, uniqueName, chainID, 0,
                                                                (MonitorBase*) this); // Mint new counter
        uniqueChain->decorate(kDecRatesGroupName, Config::config().getStr(kNoneString));
        uniqueChain->decorate(kDecType, "UniqueL1");
        uniqueChain->setGlobalRateCounter(m_globalRateL1Counter);
        (*counterMap)[uniqueName] = static_cast<CounterBase*>(uniqueChain); // Insert into the counterMap

        if (Config::config().getDisplayMsg(kMsgNewUniqueCounter) == kTRUE) {
          Info("MonitorRates::createL1Counters", "Preparing unique rates for %s", chainName.c_str());
        }
      }
    } // End unique rates counter minting

    // ##################################################################################################################
    // Create and add the L1 rates counters while also filling in the L1 items to the unique counters
    for (UInt_t i = 0; i < TrigConfInterface::getChainN(); ++i) {
      if (TrigConfInterface::getChainLevel(i) != 1) continue; // Only L1 chains

      const UInt_t chainID = TrigConfInterface::getChainCounter(i);
      const std::string chainName = TrigConfInterface::getChainName(i);
      const std::string uniqueName = Config::config().getStr(kRateUniqueString) + chainName;
      const std::string comment = TrigXMLService::trigXMLService().getChainComment(chainName);

      if (checkPatternNameMonitor(chainName, m_invertFilter) == kFALSE) continue;

      // Find the ChainItem for this chain
      ChainItemMapIt_t it = m_chainItemsL1.find(chainName);
      if (it == m_chainItemsL1.end()) {
        Warning("MonitorRates::populateCounterMap", "Cannot find L1 item: %s", chainName.c_str());
        continue;
      }
      RatesChainItem* chainItemL1 = it->second;

      // Construct a string displaying the PS for this chain
      const std::string prescaleStr = Config::config().getStr(kL1String) + ":" + doubleToString(
        chainItemL1->getPS(), 2);
      Double_t prescaleVal = chainItemL1->getPS();

      // Also get online PS
      Float_t prescaleValOnlineL1 = TrigConfInterface::getPrescale(chainName);

      // Do add to unique counters
      CounterBaseRates* thisChainsUniqueCounter = 0;
      if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
        for (CounterMapIt_t itA = counterMap->begin(); itA != counterMap->end(); ++itA) {
          CounterBaseRates* counter = static_cast<CounterBaseRates*>(itA->second);
          if (counter->getStrDecoration(kDecType) != "UniqueL1") continue;  
          // I'm not a unique counter - next
          if (counter->getName() == uniqueName) {
            thisChainsUniqueCounter = counter;
            thisChainsUniqueCounter->decorate(kDecPrescaleStr, prescaleStr);
            thisChainsUniqueCounter->decorate(kDecPrescaleVal, (Float_t) prescaleVal);
            thisChainsUniqueCounter->decorate(kDecPrescaleValOnlineL1, (Float_t) prescaleValOnlineL1);
            thisChainsUniqueCounter->decorate(kDecComment, comment);
            continue; // I'm the unique counter for *this* chain - *don't* add me!
          }
          counter->addL1Item(chainItemL1); // Add *all other* L1 items [remember unique = global - (global except for
                                             // one chain)]
        }
      }

      CounterRatesChain* L1Chain = new CounterRatesChain(m_costData, chainName, chainID, 0, (MonitorBase*) this); // Mint
                                                                                                                     // new
                                                                                                                     // counter
      L1Chain->decorate(kDecRatesGroupName, Config::config().getStr(kNoneString));
      L1Chain->decorate(kDecPrescaleStr, prescaleStr);
      L1Chain->decorate(kDecPrescaleVal, (Float_t) prescaleVal);
      L1Chain->decorate(kDecPrescaleValOnlineL1, prescaleValOnlineL1);
      L1Chain->decorate(kDecComment, comment);
      L1Chain->decorate(kDecType, "L1");
      L1Chain->addL1Item(chainItemL1); // Link it to where it'll be getting its pass/fail info
      L1Chain->setMyUniqueCounter(thisChainsUniqueCounter); // Link it to its corresponding unique counter.
      L1Chain->setGlobalRateCounter(m_globalRateL1Counter);
      (*counterMap)[chainName] = static_cast<CounterBase*>(L1Chain); // Insert into the counterMap

      //Add to global L1 counter
      m_globalRateL1Counter->addL1Item(chainItemL1);
    }
  }

  void MonitorRates::createHLTCounters(CounterMap_t* counterMap) {
    // ##################################################################################################################
    // Create the HLT unique rates counters, these will be the OR of everything HLT *EXCEPT* for one chain
    // Note, as with L1 - we are just minting counters here. Not associating them yet.
    if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
      for (UInt_t i = 0; i < TrigConfInterface::getChainN(); ++i) {
        if (TrigConfInterface::getChainLevel(i) == 1) continue; // Only HLT chains

        const UInt_t chainID = TrigConfInterface::getChainCounter(i);
        const std::string chainName = TrigConfInterface::getChainName(i);
        const std::string uniqueName = Config::config().getStr(kRateUniqueString) + chainName;
        if (checkPatternUnique(chainName, kFALSE) == kFALSE) continue;
        // In the list of unique counters?
        if (checkPatternNameMonitor(chainName, m_invertFilter) == kFALSE) continue;

        // LIMITATION - cannot do unique for CPS chains
        if (m_doCPS == kTRUE && TrigConfInterface::getChainCPSGroup(i) != "") {
          Error("MonitorRates::createHLTCounters",
                "Unique rates for chains in CPS groups are not currently supported (%s) - bug atlas-trigger-rate-expert@cern.ch if you really need this",
                chainName.c_str());
          continue;
        }

        CounterRatesUnion* uniqueChain = new CounterRatesUnion(m_costData, uniqueName, chainID, 0,
                                                                (MonitorBase*) this); // Mint new counter
        uniqueChain->decorate(kDecRatesGroupName, Config::config().getStr(kNoneString)); // Not needed
        uniqueChain->decorate(kDecType, "UniqueHLT");
        uniqueChain->setGlobalRateCounter(m_globalRateHLTCounter);
        (*counterMap)[uniqueName] = static_cast<CounterBase*>(uniqueChain); // Insert into the counterMap

        if (Config::config().getDisplayMsg(kMsgNewUniqueCounter) == kTRUE) {
          Info("MonitorRates::createHLTCounters", "Preparing unique rates for %s", chainName.c_str());
        }
      }
    } // End unique rates counter minting

    // ##################################################################################################################
    // STEP ONE: We populate the HLT items
    for (UInt_t i = 0; i < TrigConfInterface::getChainN(); ++i) {
      if (TrigConfInterface::getChainLevel(i) == 1) continue; // Only HLT chains

      const UInt_t chainID = TrigConfInterface::getChainCounter(i);
      const std::string chainName = TrigConfInterface::getChainName(i);
      const std::string comment = TrigXMLService::trigXMLService().getChainComment(chainName);
      const std::string uniqueName = Config::config().getStr(kRateUniqueString) + chainName;
      const std::vector<std::string> chainGroups = TrigConfInterface::getChainRatesGroupNames(i);
      std::string chainGroupsText = "";
      for (UInt_t g = 0; g < chainGroups.size(); ++g) {
        chainGroupsText += chainGroups.at(g);
        if (g != chainGroups.size() - 1) chainGroupsText += " ";
      }
      //Add also the stream tags
      const std::vector<std::string> chainStreams = TrigConfInterface::getChainStreamNames(i);

      if (chainName == Config::config().getStr(kBlankString)) {
        Warning("MonitorRates::createHLTCounters", "Skipping Chain ID %i. Cannot get name from current configuration.",
                chainID);
        continue;
      }

      // HLT chains with no (see: all!) seeds need special treatment
      const std::string L1Name = TrigConfInterface::getLowerChainName(chainName);

      // Find the L1 Counter for this chain. We do this so we can query the counter for its rates
      CounterMapIt_t itCm = counterMap->find(L1Name);
      CounterBaseRates* L1Counter = nullptr;
      if (itCm != counterMap->end()) L1Counter = (CounterBaseRates*) itCm->second;

      //bool hasL1Seed = kTRUE;
      if (L1Name == Config::config().getStr(kBlankString)) {
        //hasL1Seed = kFALSE;
        L1Counter = m_globalRateL1Counter;
      }

      // Are we running over this chain?
      if (checkPatternNameMonitor(chainName, m_invertFilter) == kFALSE) continue;

      // Find the m_ChainItem for this chain
      ChainItemMapIt_t it = m_chainItemsHLT.find(chainName);
      if (it == m_chainItemsHLT.end()) {
        Warning("MonitorRates::createHLTCounters", "Cannot find HLT item: %s", chainName.c_str());
        continue;
      }
      RatesChainItem* chainItemHLT = it->second;


      // Construct a string displaying the PS for this chain
      Float_t prescaleVal = 1.;
      Float_t prescaleValOnlineL1 = 1.;
      Float_t prescaleValOnlineHLT = TrigConfInterface::getPrescale(chainItemHLT->getName());
      std::string L1PSString = Config::config().getStr(kMultipleString);
      if (chainItemHLT->getLower().size() == 1) {
        prescaleVal = (*(chainItemHLT->getLowerStart()))->getPS();
        prescaleValOnlineL1 = TrigConfInterface::getPrescale((*(chainItemHLT->getLowerStart()))->getName());

        L1PSString = floatToString(prescaleVal, 2);
      } else if (chainItemHLT->getLower().size() == 1 && (*(chainItemHLT->getLower().begin()))->getName() == "") {
        L1PSString = Config::config().getStr(kAlwaysPassString); // "UNSEEDED"
      }
      const std::string prescaleStr = Config::config().getStr(kL1String) + ":" + L1PSString
                                       + " " + Config::config().getStr(kHLTString) + ":" + floatToString(
        chainItemHLT->getPS(), 2);
      prescaleVal *= chainItemHLT->getPS();

      // Was this chain prescaled out online?
      if (isZero(prescaleValOnlineHLT + 1.) == kTRUE) continue; // Don't need to process if prescaled out online
                                                                 // EFFICIENCY

      // ################################################################################################################
      // STEP TWO: Do the unique rate for this chain

      // Each unique chain does the OR of _everything_ *EXCEPT* the chain we're interested in
      // Then at the end it subtracts this rate from the global rate. So we need to add *all* chains *but* this one.

      CounterBaseRates* thisChainsUniqueCounter = 0;
      if (Config::config().getInt(kDoUniqueRates) == kTRUE /*&& hasL1Seed == kTRUE*/) {
        for (CounterMapIt_t itA = counterMap->begin(); itA != counterMap->end(); ++itA) {
          CounterBaseRates* counter = static_cast<CounterBaseRates*>(itA->second);
          if (counter->getStrDecoration(kDecType) != "UniqueHLT") continue;
          // I'm not a unique counter - next
          if (counter->getName() == uniqueName) {
            thisChainsUniqueCounter = counter;
            thisChainsUniqueCounter->decorate(kDecPrescaleStr, prescaleStr);
            thisChainsUniqueCounter->decorate(kDecPrescaleVal, prescaleVal);
            thisChainsUniqueCounter->decorate(kDecPrescaleValOnlineL1, prescaleValOnlineL1);
            thisChainsUniqueCounter->decorate(kDecComment, comment);
            continue; // I'm the unique counter for *this* chain - *don't* add me!
          }
          // Chain *may* be part of a CPS group
          RatesCPSGroup* cpsGroup = nullptr;
          const std::string chainCPSGroup = TrigConfInterface::getChainCPSGroup(i);
          if (m_doCPS && chainCPSGroup != "") {
            CPSGroupMapIt_t it = m_cpsGroups.find(chainCPSGroup);
            if (it != m_cpsGroups.end()) cpsGroup = it->second;
          }
          // Add this chain to it, or if it's in a CPS group add the group.
          // This will result in the CPS group being added multiple times, but it's a set of pointers so this is fine.
          if (cpsGroup != nullptr) {
            counter->addCPSItem(cpsGroup, chainName);
          } else {
            counter->addL2Item(chainItemHLT);
          }
        }
      }

      // ################################################################################################################
      // STEP THREE: Make a new counter for this HLT Chain

      // And add a new counter to get the rates for this chainItem
      CounterRatesChain* ratesChain = new CounterRatesChain(m_costData, chainName, chainID, 0, (MonitorBase*) this); // Mint
                                                                                                                        // new
                                                                                                                        // counter
      ratesChain->decorate(kDecRatesGroupName, chainGroupsText);
      ratesChain->decorate(kDecPrescaleStr, prescaleStr);
      ratesChain->decorate(kDecPrescaleVal, prescaleVal);
      ratesChain->decorate(kDecPrescaleValOnlineL1, prescaleValOnlineL1);
      ratesChain->decorate(kDecComment, comment);
      ratesChain->decorate(kDecType, "Chain");
      ratesChain->setMyUniqueCounter(thisChainsUniqueCounter); // Link it to its corresponding unique counter.
      ratesChain->setGlobalRateCounter(m_globalRateHLTCounter);
      ratesChain->setLowerRateCounter(L1Counter);
      ratesChain->addL2Item(chainItemHLT); // Link it to where it'll be getting its pass/fail info
      (*counterMap)[chainName] = static_cast<CounterBase*>(ratesChain); // Insert into the counterMap
      //Info("MonitorRates::createHLTCounters","Created counter for: %s", chainName.c_str() );

      if (TrigXMLService::trigXMLService().hasExpressPrescaleInfo() &&
          TrigXMLService::trigXMLService().getExpressPrescaleInfo(chainName) > 0) {
        ratesChain->decorate(kDecDoExpressChain, 1);
      }
    }
  }

  /**
   * Create one union counter per CPS group and add the group to it. Nothing more than that here.
   */
  void MonitorRates::createCPSGroupCounters(CounterMap_t* counterMap) {
    if (m_doCPS == kFALSE) return;

    for (UInt_t i = 0; i < TrigConfInterface::getChainN(); ++i) {
      if (TrigConfInterface::getChainLevel(i) == 1) continue; // Only HLT chains
      const std::string chainName = TrigConfInterface::getChainName(i);
      if (checkPatternNameMonitor(chainName, m_invertFilter) == kFALSE) continue;

      Float_t prescaleValOnlineHLT = TrigConfInterface::getPrescale(chainName);
      //Double_t prescaleValHLT = TrigXMLService::trigXMLService().getPrescale( TrigConfInterface::getChainName(i) );
      if (isZero(prescaleValOnlineHLT + 1.) == kTRUE) continue; // Don't need to process if prescaled out online.
                                                                 // EFFICIENCY CODE
      //if (isZero(prescaleValHLT + 1.) == kTRUE) continue; // Don't need to process if prescaled out in prediction.
      // EFFICIENCY CODE

      const std::string chainCPSGroup = TrigConfInterface::getChainCPSGroup(i);
      if (chainCPSGroup == "") continue; // Only chains in a CPS group

      // Check we don't have a counter already
      // (the chains are already grouped in a RatesCPSGroup so we don't need to add them all individually again here)
      // But we do need to tell the group counter about this chain, so it knows that it's part of it.
      // Otherwise it wouldn't know what part of the CPS group to process or not
      if (counterMap->count(chainCPSGroup) == 1) {
        CounterBaseRates* ptr = dynamic_cast<CounterBaseRates*>(counterMap->find(chainCPSGroup)->second);
        if (ptr != nullptr) ptr->addCPSItem(nullptr, chainName);
        else Error("MonitorRates::createCPSGroups", "Could not get the group counter for %s", chainCPSGroup.c_str());
        continue;
      }

      CPSGroupMapIt_t it = m_cpsGroups.find(chainCPSGroup);
      if (it == m_cpsGroups.end()) {
        Warning("MonitorRates::createCPSGroups", "Cannot find the CPS group for %s", chainCPSGroup.c_str());
        continue;
      }
      RatesCPSGroup* cpsGroup = it->second;

      CounterRatesUnion* ratesCpsGroup = new CounterRatesUnion(m_costData, chainCPSGroup, 0, 0, (MonitorBase*) this); // Mint
                                                                                                                        // new
                                                                                                                        // counter
      ratesCpsGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
      ratesCpsGroup->decorate(kDecPrescaleVal, (Float_t) 0.);
      ratesCpsGroup->decorate(kDecPrescaleValOnlineL1, (Float_t) 0.);
      ratesCpsGroup->decorate(kDecComment, "");
      ratesCpsGroup->decorate(kDecRatesGroupName, chainCPSGroup);
      ratesCpsGroup->decorate(kDecType, "Union");
      ratesCpsGroup->setGlobalRateCounter(m_globalRateHLTCounter);
      ratesCpsGroup->addCPSItem(cpsGroup, chainName); // Add the group
      (*counterMap)[chainCPSGroup] = static_cast<CounterBase*>(ratesCpsGroup); // Instert into the map
    }
  }

  /**
   * Create one counter per RATES group and add to it a combination of CPS groups and non-CPS chains.
   */
  void MonitorRates::createGroupCounters(CounterMap_t* counterMap) {
    CounterRatesUnion* expressGroup = nullptr;

    if (TrigXMLService::trigXMLService().hasExpressPrescaleInfo() == kTRUE) {
      expressGroup = new CounterRatesUnion(m_costData, Config::config().getStr(
                                              kRateExpressString), 0, 0, (MonitorBase*) this); // Mint new counter,
                                                                                               // kRateExpressString is
                                                                                               // a special string - it
                                                                                               // will trigger special
                                                                                               // behaviour
      expressGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
      expressGroup->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
      expressGroup->decorate(kDecPrescaleValOnlineL1, (Float_t) 0);
      expressGroup->decorate(kDecComment, "");
      expressGroup->decorate(kDecType, "Union");
      (*counterMap)[Config::config().getStr(kRateExpressString)] = static_cast<CounterBase*>(expressGroup); // Instert
                                                                                                              // into
                                                                                                              // the map
    }

    for (UInt_t i = 0; i < TrigConfInterface::getChainN(); ++i) {
      if (TrigConfInterface::getChainLevel(i) == 1) continue; // Only HLT chains
      if (checkPatternNameMonitor(TrigConfInterface::getChainName(i), m_invertFilter) == kFALSE) continue;
      const std::string chainName = TrigConfInterface::getChainName(i);

      Float_t prescaleValOnlineHLT = TrigConfInterface::getPrescale(chainName);
      if (isZero(prescaleValOnlineHLT + 1.) == kTRUE) continue; // Don't need to process if prescaled out online.
                                                                 // EFFICIENCY CODE

      const Bool_t isMain = TrigConfInterface::getChainIsMainStream(i);
      std::vector<std::string> chainGroups = TrigConfInterface::getChainRatesGroupNames(i);
      const Bool_t isExpress =
        (TrigXMLService::trigXMLService().hasExpressPrescaleInfo() &&
         TrigXMLService::trigXMLService().getExpressPrescaleInfo(chainName) > 0);

      RatesCPSGroup* cpsGroup = nullptr;
      const std::string chainCPSGroup = TrigConfInterface::getChainCPSGroup(i);
      if (m_doCPS && chainCPSGroup != "") {
        CPSGroupMapIt_t it = m_cpsGroups.find(chainCPSGroup);
        if (it != m_cpsGroups.end()) cpsGroup = it->second;
      }

      // They are still groups, can still get their basic rate
      if (m_doCPS == false && chainCPSGroup != "") chainGroups.push_back(chainCPSGroup);

      // Also add STREAM rates
      std::vector<std::string> chainStreams = TrigConfInterface::getChainStreamNames(i);
      chainGroups.insert(chainGroups.end(), chainStreams.begin(), chainStreams.end());

      // Debug
      // std::stringstream ss;
      // for (UInt_t group = 0; group < chainGroups.size(); ++group) ss << chainGroups.at(group) << " ";
      // ss << TrigConfInterface::getChainCPSGroup(i);
      // Info("MonitorRates::createGroupCounters","Chain %s in groups: %s", TrigConfInterface::getChainName(i).c_str(),
      // ss.str().c_str());

      for (UInt_t group = 0; group < chainGroups.size(); ++group) {
        std::string chainGroup = chainGroups.at(group);

        ChainItemMapIt_t it = m_chainItemsHLT.find(chainName);
        if (it == m_chainItemsHLT.end()) {
          Warning("MonitorRates::createGroups", "Cannot find a RatesChainItem for %s when doing group %s", TrigConfInterface::getChainName(
                    i).c_str(), chainGroup.c_str());
          continue;
        }
        RatesChainItem* chainItemHLT = it->second;

        CounterRatesUnion* ratesGroup = nullptr;
        // Do we have a counter for this group?
        CounterMapIt_t findGroup = counterMap->find(chainGroup);
        if (findGroup != counterMap->end()) {
          // We do have a group already!
          ratesGroup = static_cast<CounterRatesUnion*>(findGroup->second);
        } else {
          // We need a new group counter, this should be of type Union
          ratesGroup = new CounterRatesUnion(m_costData, chainGroup, 0, 0, (MonitorBase*) this); // Mint new counter
          ratesGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
          ratesGroup->decorate(kDecPrescaleVal, (Float_t) 0.);
          ratesGroup->decorate(kDecPrescaleValOnlineL1, (Float_t) 0.);
          ratesGroup->decorate(kDecComment, "");
          ratesGroup->decorate(kDecRatesGroupName, chainGroup);
          ratesGroup->decorate(kDecType, "Union");
          ratesGroup->setGlobalRateCounter(m_globalRateHLTCounter);
          (*counterMap)[chainGroup] = static_cast<CounterBase*>(ratesGroup); // Insert into the map
        }

        // Add this chain to it, or if it's in a CPS group add the group.
        // This will result in the CPS group being added multiple times, but it's a set of pointers so this is fine.
        if (cpsGroup != nullptr) {
          ratesGroup->addCPSItem(cpsGroup, chainName);
          if (group == 0) { // Might be adding to many groups, but only want (need) to add to the globals once
            m_globalRateHLTCounter->addCPSItem(cpsGroup, chainName);
            if (isMain) m_globalRatePhysicsMainCounter->addCPSItem(cpsGroup, chainName);
            if (isExpress) expressGroup->addCPSItem(cpsGroup, chainName);
          }
        } else {
          ratesGroup->addL2Item(chainItemHLT);
          if (group == 0) {
            m_globalRateHLTCounter->addL2Item(chainItemHLT);
            if (isMain) m_globalRatePhysicsMainCounter->addL2Item(chainItemHLT);
            if (isExpress) expressGroup->addL2Item(chainItemHLT);
          }
        }
      } // loop over chain groups
    } // loop over chains
  } // createGroups

  void MonitorRates::createOverlapCounters(CounterMap_t* counterMap) {
    for (CounterMapIt_t itA = counterMap->begin(); itA != counterMap->end(); ++itA) {
      // Look for all Chain counters
      CounterBaseRates* counterA = static_cast<CounterBaseRates*>(itA->second);
      if (counterA->getStrDecoration(kDecType) != "Chain") {
        continue; // I'm not a HLT chain counter - next
      }

      if (checkPatternOverlap(counterA->getName(), kFALSE) == kFALSE) continue;

      // Now look at all other counters - find others in my group
      for (CounterMapIt_t itB = counterMap->begin(); itB != counterMap->end(); ++itB) {
        if (itA == itB) continue;

        CounterBaseRates* counterB = static_cast<CounterBaseRates*>(itB->second);
        if (counterB->getStrDecoration(kDecType) != "Chain") {
          continue; // I'm not a HLT chain counter - next
        }

        // Same group?
        if (Config::config().getInt(kDoGroupOverlap) == kTRUE
            && counterA->getStrDecoration(kDecRatesGroupName) != counterB->getStrDecoration(kDecRatesGroupName)) {
          continue;
        }

        // Construct name for overlap counter
        const std::string name = counterA->getName() + Config::config().getStr(kAndString) + counterB->getName();

        // check we don't have this already the other way around!
        const std::string altName = counterB->getName() + Config::config().getStr(kAndString) + counterA->getName();
        if (counterMap->count(altName) > 0) continue;

        // Add new overlap counter!
        CounterRatesIntersection* overlapCounter = new CounterRatesIntersection(m_costData, name, 0, 0,
                                                                                 (MonitorBase*) this); // Mint new
                                                                                                       // counter
        overlapCounter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
        overlapCounter->decorate(kDecPrescaleValOnlineL1, (Float_t) 0.);
        overlapCounter->decorate(kDecComment, "");
        overlapCounter->decorate(kDecRatesGroupName, counterA->getStrDecoration(kDecRatesGroupName));
        overlapCounter->decorate(kDecType, "Intersection");
        overlapCounter->addL2Items(counterA->getL2ItemSet());
        overlapCounter->addL2Items(counterB->getL2ItemSet());
        (*counterMap)[name] = static_cast<CounterBase*>(overlapCounter);

        // Give the two chain counters pointers to this overlap counter to be able to calculate their respective overlap
        // with each other
        counterA->addOverlap(static_cast<CounterBase*>(overlapCounter));
        counterB->addOverlap(static_cast<CounterBase*>(overlapCounter));
      }
    }
  }

  /**
   * Process new event for this monitor.
   * For the chain rate monitor - we look at a chain's rate and its correlation with other chains in its group
   * @param weight The event weight.
   */
  void MonitorRates::newEvent(Float_t weight) {
    if (getPass() == 2) return; // We don't do 2-pass

    m_timer.start();
    if (Config::config().debug()) Info("MonitorRates::newEvent", "*** Processing Chain Rates ***");

    // IMPORTANT NOTE - we REMOVE the Lumi extrap weight as this is now added more intelligently (weighted average per
    // chain combo)
    weight /= Config::config().getFloat(kLumiExtrapWeight);

    // First time? Setup the m_ChainItem maps to hold the decision and prescale information for the rates counters to use.
    if (m_chainItemsHLT.size() == 0 || m_chainItemsL1.size() == 0) populateChainItemMaps();

    //Now loop over the counter collections;
    for (CounterMapSetIt_t cmsIt = m_collectionsToProcess.begin(); cmsIt != m_collectionsToProcess.end(); ++cmsIt) {
      CounterMap_t* counterMap = *cmsIt; // This counter map holds all the counters. But we don't really want to
                                           // process every one
      CounterBaseRatesSet_t inEventCounterMap; // This set contains all the counters we actually do need to iterate
                                                // over

      // If the counter map is empty, then we need to populate it. We will pre-load a counter for every chain.
      if (counterMap->size() == 0) populateCounterMap(counterMap);


      // ##################################################################################################################
      // PASS ONE: Fill the event information into HLT RatesChainItems - this is their only job, to remember this stuff.
      ChainItemSet_t chainItemsInEvent;
      for (UInt_t c = 0; c < m_costData->getNChains(); ++c) {
        // Get the name of the chain (Supplying L2 or EF helps, but is not needed)
        Int_t chainID = m_costData->getChainID(c);
        const std::string chainName =
          TrigConfInterface::getHLTNameFromChainID(chainID, m_costData->getChainLevel(c));
        // Did we fail?
        if (chainName == Config::config().getStr(kBlankString)) {
          Warning("MonitorRates::newEvent", "Skipping Chain ID %i. Cannot get name from current configuration.",
                  chainID);
          continue;
        }

        // Find the m_ChainItem for this chain so we can set it's PassedRaw bit for this event
        ChainItemMapIt_t it = m_chainItemsHLT.find(chainName);
        if (it == m_chainItemsHLT.end()) {
          Warning("MonitorRates::newEvent", "Cannot find HLT item: %s", chainName.c_str());
          continue;
        }

        RatesChainItem* chainItem = it->second;
        Bool_t decision = kFALSE;
        if (m_isCPUPrediction) decision = m_costData->getIsChainPassed(c);
        else decision = m_costData->getIsChainPassedRaw(c);
        chainItem->beginEvent(decision, inEventCounterMap);
        chainItemsInEvent.insert(chainItem);

        if (Config::config().debug()) {
          Info("MonitorRates::newEvent", "Storing Decision:%i for HLT m_ChainItem:%s with PS:%f",
               (Int_t) m_costData->getIsChainPassedRaw(c),
               chainItem->getName().c_str(), chainItem->getPS());
        }
      }

      // ##################################################################################################################
      // Also fill into L1 items
      for (UInt_t c = 0; c < m_costData->getNL1(); ++c) {
        Int_t CTPID = m_costData->getL1CtpId(c);
        const std::string chainName = TrigConfInterface::getNameFromCtpId(CTPID);
        // Did we fail?
        if (chainName == Config::config().getStr(kBlankString)) {
          Warning("MonitorRates::newEvent", "Skipping L1 Chain ID %i. Cannot get name from current configuration.",
                  CTPID);
          continue;
        }

        // Find the m_ChainItem for this chain so we can set it's PassedRaw bit for this event, could be L1 or HLT
        ChainItemMapIt_t it = m_chainItemsL1.find(chainName);
        if (it == m_chainItemsL1.end()) {
          Warning("MonitorRates::newEvent", "Cannot find L1 item: %s", chainName.c_str());
          continue;
        }


        // If we are applying EB weights, then we can apply whatever PS we want at L1. And hence, like at HLT, we should
        // use the TBP bit.
        // However. If *not* doing weights, then we cannot undo the L1 prescale and hence the rate is fixed by what is
        // in the file.
        // We should therefore use TAV.

        //XXX TODO READDRESS THIS - NO LONGER TRUE I THINK

        // We definitly want to use TAV when prescales were applied to EB on the grid (CPU prediction mode)

        Bool_t decision = kFALSE;
        if (m_isCPUPrediction == kTRUE) decision = m_costData->getIsL1PassedAfterVeto(c);
        else decision = m_costData->getIsL1PassedBeforePrescale(c);
        // if ( Config::config().getInt(kDoEBWeighting) == 0) {
        //   decision = m_costData->getIsL1PassedAfterVeto(c);
        // }

        RatesChainItem* chainItem = it->second;
        chainItem->beginEvent(decision, inEventCounterMap);
        chainItemsInEvent.insert(chainItem);


        if (Config::config().debug()) {
          Info("MonitorRates::newEvent", "Storing Decision:%i for L1 m_ChainItem:%s with PS:%f",
               (Int_t) decision,
               chainItem->getName().c_str(), chainItem->getPS());
        }
      }

      // If we are debugging such that chains always pass - then we need to re-roll everyone's random PS numbers.
      // This can be done anyway, but it normally not necessary.
      if (Config::config().getInt(kRatesForcePass) == kTRUE) {
        for (ChainItemMapIt_t it = m_chainItemsHLT.begin(); it != m_chainItemsHLT.end(); ++it) {
          it->second->newRandomPS();
        }
        for (ChainItemMapIt_t it = m_chainItemsL1.begin(); it != m_chainItemsL1.end(); ++it) {
          it->second->newRandomPS();
        }
      }

      // ##################################################################################################################
      // PASS TWO: Now loop over all counters, they will use their pre-linked RatesChainItems to get their weights.
      // OLD - inefficient, does them all
      // Do the globals first
      m_globalRateL1Counter->processEventCounter(0, 1, weight);
      m_globalRateHLTCounter->processEventCounter(0, 1, weight);
      m_globalRatePhysicsMainCounter->processEventCounter(0, 1, weight);
      CounterMapIt_t it = counterMap->begin();
      for (; it != counterMap->end(); ++it) {
        it->second->processEventCounter(0, 1, weight);
      }
      // NEW - better - only does the ones we need to
      // CounterBaseRatesSetIt_t cbrsit = inEventCounterMap.begin();
      // for (; cbrsit != inEventCounterMap.end(); ++cbrsit) {
      //   (*cbrsit)->processEventCounter(0, 0, weight);
      // }

      // ##################################################################################################################
      // PASS THREE: Now we just need to reset the ChainItems which were included in the event.
      for (ChainItemSetIt_t setIt = chainItemsInEvent.begin(); setIt != chainItemsInEvent.end(); ++setIt) {
        RatesChainItem* item = (*setIt);
        item->endEvent();
      }
      chainItemsInEvent.clear();
      m_countersInEvent.clear(); // unused at the moment
    }
    m_timer.stop();
  }

  /**
   * Saves the topology of all counters to a JSON file linked to the counter name. Imported by web display.
   */
  void MonitorRates::saveRateGraphs() {
    const std::string output = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(
      kOutputRatesGraphFilename);

    std::ofstream fout(output.c_str());
    JsonExport json;
    json.addNode(fout, "ratesGaphs");

    std::set<std::string> configsSeen;
    for (CounterCollectionIt_t collectionIt =
           m_counterCollections.begin(); collectionIt != m_counterCollections.end();
         ++collectionIt) {
      for (CounterMapIt_t counterMapIt = collectionIt->second.begin(); counterMapIt != collectionIt->second.end();
           ++counterMapIt) {
        CounterBaseRates* counter = static_cast<CounterBaseRates*>(counterMapIt->second);

        if (configsSeen.count(counter->getName()) == 1) continue;
        configsSeen.insert(counter->getName());
        json.addNode(fout, counter->getName());

        // If a unique then continue
        if (counter->getStrDecoration(kDecType) == "UniqueHLT" || counter->getStrDecoration(kDecType) == "UniqueL1") {
          json.endNode(fout);
          continue;
        }

        if (counter->getL2ItemSet().size() == 0) { // If only L1
          for (ChainItemSetIt_t L1It = counter->getL1ItemSet().begin(); L1It != counter->getL1ItemSet().end(); ++L1It ) {
            RatesChainItem* L1 = (*L1It);
            const std::string source = L1->getName() + " [PS:" + doubleToString(L1->getPS()) + "]";
            json.addLeafCustom(fout, "source", source, "target", source);
          }
        }

        for (ChainItemSetIt_t L2It = counter->getL2ItemSet().begin(); L2It != counter->getL2ItemSet().end(); ++L2It ) {
          RatesChainItem* L2 = (*L2It);
          for (ChainItemSetIt_t L1It = L2->getLowerStart(); L1It != L2->getLowerEnd(); ++L1It) {
            RatesChainItem* L1 = (*L1It);
            const std::string source = L1->getName() + " [PS:" + doubleToString(L1->getPS()) + "]";
            const std::string target = L2->getName() + " [PS:" + doubleToString(L2->getPS()) + "]";
            json.addLeafCustom(fout, "source", source, "target", target);

          }
        }

        // Don't forget the CPS chain groups
        for (CPSGroupSetIt_t CPSIt = counter->getCPSGroupSet().begin(); CPSIt != counter->getCPSGroupSet().end();
             ++CPSIt) {
          RatesCPSGroup* cpsGroup = (*CPSIt);
          for (ChainItemSetIt_t L2It = cpsGroup->getChainStart(); L2It != cpsGroup->getChainEnd(); ++L2It) {
            RatesChainItem* L2 = (*L2It);
            for (ChainItemSetIt_t L1It = L2->getLowerStart(); L1It != L2->getLowerEnd(); ++L1It) {
              RatesChainItem* L1 = (*L1It);
              const std::string source = L1->getName() + " [PS:" + doubleToString(L1->getPS()) + "]";
              const std::string target = L2->getName() + " [PS:" + doubleToString(L2->getPS()) + "]";
              json.addLeafCustom(fout, "source", source, "target", target);
            }
          }
        }

        json.endNode(fout);
      }
    }
    json.endNode(fout); // ratesGraphs
    fout.close();
  }

  /**
   * Do we use this monitor for this particular mode? Try and keep things managable in terms of output created!
   * Note these are currently hard-coded. We may want to make them configurable
   * @return If this monitor should be active for a given mode.
   */
  Bool_t MonitorRates::getIfActive(ConfKey_t mode) {
    switch (mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kTRUE;

    default: Error("MonitorRates::getIfActive", "An invalid summary mode was provided (key %s)",
                   Config::config().getName(mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorRates::saveOutput() {
    // Send finalise calls

    for (CounterCollectionNonConstIt_t collectionIt = m_counterCollections.begin();
         collectionIt != m_counterCollections.end(); ++collectionIt) {
      // (Finally) add these to the maps such that they get exported with everything else
      // We didn't add them so far so we could always process these three first.
      (collectionIt->second)[Config::config().getStr(kRateGlobalL1String)] =
        static_cast<CounterBase*>(m_globalRateL1Counter);
      (collectionIt->second)[Config::config().getStr(kRateGlobalHLTString)] =
        static_cast<CounterBase*>(m_globalRateHLTCounter);
      (collectionIt->second)[Config::config().getStr(kRateGlobalPhysicsMainString)] =
        static_cast<CounterBase*>(m_globalRatePhysicsMainCounter);

      const std::string counterCollectionName = collectionIt->first;
      // Finalise unique counters
      if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
        for (CounterMapIt_t counterMapIt = collectionIt->second.begin(); counterMapIt != collectionIt->second.end();
             ++counterMapIt) {
          CounterBaseRates* counter = static_cast<CounterBaseRates*>(counterMapIt->second);
          if (counter->getStrDecoration(kDecType) == "UniqueHLT" ||
              counter->getStrDecoration(kDecType) == "UniqueL1") {
            counter->decorate(kDecLbLength,
                               m_collectionLumiCollector[ counterCollectionName ]->getTotalLumiBlockTime());
            counter->finalise();
          }
        }
      }
      // Finalise all other counters (makes use of unique counters)
      for (CounterMapIt_t counterMapIt = collectionIt->second.begin(); counterMapIt != collectionIt->second.end();
           ++counterMapIt) {
        //Note we have added a negation here
        CounterBaseRates* counter = static_cast<CounterBaseRates*>(counterMapIt->second);
        if (!(counter->getStrDecoration(kDecType) == "UniqueHLT" ||
              counter->getStrDecoration(kDecType) == "UniqueL1")) {
          counter->decorate(kDecLbLength,
                             m_collectionLumiCollector[ counterCollectionName ]->getTotalLumiBlockTime());
          counter->finalise();
        }
      }
    }

    TrigXMLService::trigXMLService().saveRuleBookXML(m_counterCollections, getLevelStr());

    // Also copy any PS files
    if (Config::config().getIsSet(kPrescaleXMLPath1)) {
      const std::string outputFile = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(
        kOutputXMLDirectory) + "/prescales1.xml";
      gSystem->CopyFile(Config::config().getStr(kPrescaleXMLPath1).c_str(), outputFile.c_str());
    }
    if (Config::config().getIsSet(kPrescaleXMLPath2)) {
      const std::string outputFile = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(
        kOutputXMLDirectory) + "/prescales2.xml";
      gSystem->CopyFile(Config::config().getStr(kPrescaleXMLPath2).c_str(), outputFile.c_str());
    }

    if (Config::config().getInt(kOutputRatesGraph) == kTRUE) saveRateGraphs();

    VariableOptionVector_t toSavePlots;// = m_dummyCounter->getAllHistograms();

    std::vector<TableColumnFormatter> toSaveTable;

    toSaveTable.push_back(TableColumnFormatter("Active Time [s]",
                                                "Integrated length of all lumi blocks which contributed events to this rates prediction.",
                                                kVarEventsPassedNoPS /*ignored*/, kSavePerCall /*ignored*/, 2,
                                                kFormatOptionUseWallTime));

    toSaveTable.push_back(TableColumnFormatter("Group",
                                                "The group this chain belongs to.",
                                                kDecRatesGroupName, kSavePerCall, 0, kFormatOptionUseStringDecoration));

    toSaveTable.push_back(TableColumnFormatter("Weighted PS Rate [Hz]",
                                                "Rate after applying all prescale(s) as weights.",
                                                kVarEventsPassed, kSavePerCall, 4, kFormatOptionNormaliseWallTime));

    toSaveTable.push_back(TableColumnFormatter("Weighted PS Rate Err [Hz]",
                                                "Error on rate after applying all prescale(s) as weights",
                                                &tableFnRateGetWeightedRateErr, 4));

    if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
      toSaveTable.push_back(TableColumnFormatter("Unique Rate [Hz]",
                                                  "Rate unique to this chain or combination of chains.",
                                                  kDecUniqueRate, kSavePerCall, 0, kFormatOptionUseStringDecoration));

      toSaveTable.push_back(TableColumnFormatter("Unique Fraction [%]",
                                                  "Fraction of rate of this chain with no overlap with any other chain.",
                                                  kDecUniqueFraction, kSavePerCall, 0,
                                                  kFormatOptionUseStringDecoration));
    }

    if (TrigXMLService::trigXMLService().hasExpressPrescaleInfo() == kTRUE) {
      toSaveTable.push_back(TableColumnFormatter("Express Rate [Hz]",
                                                  "Rate to the express stream.",
                                                  kDecExpressRate, kSavePerCall, 0, kFormatOptionUseStringDecoration));
    }

    toSaveTable.push_back(TableColumnFormatter("Prescale",
                                                "The prescale of this chain. Only displayed for simple combinations.",
                                                kDecPrescaleStr, kSavePerCall, 0, kFormatOptionUseStringDecoration));

    if (Config::config().getInt(kRatesScaleByPS)) {
      toSaveTable.push_back(TableColumnFormatter("L1 Online Prescale",
                                                  "The online prescale of this chain at L1. Applied here as a weight.",
                                                  kDecPrescaleValOnlineL1, kSavePerCall, 0,
                                                  kFormatOptionUseFloatDecoration));
    }

    toSaveTable.push_back(TableColumnFormatter("ID",
                                                "The CPTID or HLT Chain ID",
                                                kDecID, kSavePerCall, 0, kFormatOptionUseIntDecoration));

    toSaveTable.push_back(TableColumnFormatter("Raw Active Events",
                                                "Raw underlying statistics on the number events processed for this chain.",
                                                kVarEventsRunRawStat, kSavePerCall, 0));

    toSaveTable.push_back(TableColumnFormatter("Raw Pass Events",
                                                "Raw underlying statistics on the number events passed by this chain.",
                                                kVarEventsPassRawStat, kSavePerCall, 0));

    toSaveTable.push_back(TableColumnFormatter("Active Events",
                                                "Number of events in which the chain - or at least one chain in the combination - was executed.",
                                                kVarEventsRun, kSavePerCall, 4));

    toSaveTable.push_back(TableColumnFormatter("Input Rate [Hz]",
                                                "Input rate to this chain or combination of chains. At L1 this will be the collision frequency for the bunch pattern.",
                                                kDecInputRate, kSavePerCall, 0, kFormatOptionUseStringDecoration));
//    kVarEventsRun, kSavePerCall, 4, kFormatOptionNormaliseWallTime) );

    toSaveTable.push_back(TableColumnFormatter("Pass Fraction before PS [%]",
                                                "Fraction of events which pass this trigger before prescale.",
                                                kVarEventsPassedNoPS, kSavePerCall, kVarEventsRun, kSavePerCall, 4,
                                                kFormatOptionToPercentage));

    toSaveTable.push_back(TableColumnFormatter("Pass Fraction after PS [%]",
                                                "Fraction of events which pass this trigger after prescale.",
                                                kVarEventsPassed, kSavePerCall, kVarEventsRun, kSavePerCall, 4,
                                                kFormatOptionToPercentage));

    toSaveTable.push_back(TableColumnFormatter("Pass Weighted PS",
                                                "Number of events this chain or combination passed after applying prescales as weighting factors.",
                                                kVarEventsPassed, kSavePerCall, 4));

    if (TrigXMLService::trigXMLService().hasChainComments() == kTRUE) {
      toSaveTable.push_back(TableColumnFormatter("Comment",
                                                  "Comment from the rulebook.",
                                                  kDecComment, kSavePerCall, 0, kFormatOptionUseStringDecoration));
    }



    // We save in three steps to put the different categories in different folders
    // First the chains, veto on AND and Rate (group) counters
    m_filterOutput = kTRUE;
    setName("Rate_ChainL1");
    filterOutputOnStrDecoration(kDecType, "L1");
    sharedTableOutputRoutine(toSaveTable);
    sharedHistogramOutputRoutine(toSavePlots);

    setName("Rate_ChainHLT");
    filterOutputOnStrDecoration(kDecType, "Chain");
    sharedTableOutputRoutine(toSaveTable);
    sharedHistogramOutputRoutine(toSavePlots);

    //toSaveTable.erase( toSaveTable.begin() + 2);   // Now do the AND - note we can remove the "Prescale" option from
    // the table.
    if (m_doingOverlaps == kTRUE) {
      setName("Rate_Combination");
      filterOutputOnStrDecoration(kDecType, "Intersection");
      sharedTableOutputRoutine(toSaveTable);
      sharedHistogramOutputRoutine(toSavePlots);
    }

    // Now do the GROUP - note we can remove the "Group" option from the table.
    //toSaveTable.erase( toSaveTable.begin() + 1 );
    setName("Rate_Group");
    filterOutputOnStrDecoration(kDecType, "Union");
    sharedTableOutputRoutine(toSaveTable);
    sharedHistogramOutputRoutine(toSavePlots);
  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &name, Int_t ID )
   * @param name Cost reference to name of counter.
   * @param ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorRates::newCounter(const std::string& /*name*/, Int_t /*ID*/) {
    Error("MonitorRates::newCounter",
          "For Rates counters - please mint them yourself as the derived type is important (Chain, Union, Intersection etc.)");
    return 0;
  }
} // namespace TrigCostRootAnalysis
