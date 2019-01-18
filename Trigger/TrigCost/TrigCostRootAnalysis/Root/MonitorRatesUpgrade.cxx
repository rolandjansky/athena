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
  MonitorRatesUpgrade::MonitorRatesUpgrade(const TrigCostData* costData)
    : MonitorBase(costData, "RateUpgrade"),
    m_R3(0),
    m_statsEventMu(0.),
    m_statsEventMu2(0.),
    m_statsEventMuN(0),
    m_thisEventPtr(nullptr) {
    m_dummyCounter =
      static_cast<CounterBase*>(new CounterRatesChain(costData, Config::config().getStr(kDummyString), 10));
    m_globalRateL1Counter = nullptr;
    m_globalRateL2Counter = nullptr;
    m_globalRateL3Counter = nullptr;
    m_scenario = Config::config().getStr(kUpgradeScenario);
    m_upgradePileupScaling = !Config::config().getInt(kNoUpgradePileupScaling);
    m_upgradeBunchScaling = !Config::config().getInt(kNoUpgradeBunchScaling);
    m_doDeadtimeScaling = Config::config().getIsSet(kOnlineDeadtime) == kTRUE && Config::config().getInt(
      kNoOnlineDeadtimeCorrection) == kFALSE;
    m_upgradeDeadtimeScaling = 1.;
    m_targetPairedBunches = 0;
    m_collidingBunchFactor = 1.;
    m_pileupFactor = 1.;
    m_collisionsToMix = 0;
  }

  /**
   * Destroy this monitor - note we have special RatesChainItems to destroy here too
   */
  MonitorRatesUpgrade::~MonitorRatesUpgrade() {
    for (auto item : m_chainItemsL1) delete item.second;
    for (auto item : m_chainItemsL2) delete item.second;
    for (auto item : m_chainItemsL3) delete item.second;
  }

  /**
   * Here we create a RatesChainItem for each trigger item at each level, it is these which will be loaded
   * per event with the raw decision of that trigger item and the counters will use the information of the L1
   * and HLT chain items given to them to calculate their event weight.
   */
  void MonitorRatesUpgrade::populateChainItemMaps() {
    TrigXMLService::trigXMLService().parseUpgradeXML(m_isoBits, m_upgradeChains);

    Info("MonitorRatesUpgrade::populateChainItemMaps",
         "*** CALCULATING EXTRAPOLATION SPECIFICS FOR UPGRADE RATES MONITOR ***");

    // We scale up unbiased events due to pileup. But by how much?
    if (m_upgradePileupScaling == kTRUE) {
      //m_pileupFactor = Config::config().getFloat(kTargetPeakMuAverage) / (Float_t)
      // Config::config().getFloat(kOnlinePeakMuAverage); // Should have been set by run XML
      m_pileupFactor = Config::config().getFloat(kTargetPeakMuAverage); // Should have been set by run XML

      // Info("MonitorRatesUpgrade::populateChainItemMaps","We will mix in to each event %.2fx extra unbiased event's
      // TOBs (Pileup %.2f->%.2f = x%.2f)",
      //   m_pileupFactor-1, Config::config().getFloat(kOnlinePeakMuAverage),
      // Config::config().getFloat(kTargetPeakMuAverage), m_pileupFactor);

      Info("MonitorRatesUpgrade::populateChainItemMaps",
           "We will mix in to each event extra overlay up to <mu> = %.2f ",
           m_pileupFactor);
    } else {
      Warning("MonitorRatesUpgrade::populateChainItemMaps", "Upgrade rates will not be scaled for extra pileup.");
    }

    if (m_upgradeBunchScaling == kTRUE) {
      m_targetPairedBunches = Config::config().getInt(kTargetPairedBunches);
      Int_t pairedBunches = Config::config().getInt(kPairedBunches);

      m_collidingBunchFactor = m_targetPairedBunches / (Float_t) pairedBunches; // Going to continue to assume that 1
                                                                                 // is PAIRED
      Info("MonitorRatesUpgrade::populateChainItemMaps",
           "We will scale x%.2f for the number of bunches in the ring (%i->%i) (note this may change mid-processing if using MultiRun)",
           m_collidingBunchFactor, pairedBunches, m_targetPairedBunches);
    } else {
      Warning("MonitorRatesUpgrade::populateChainItemMaps", "Upgrade rates will not be scaled for extra bunches.");
    }

    if (m_doDeadtimeScaling) {
      m_upgradeDeadtimeScaling = 1. + Config::config().getFloat(kOnlineDeadtime);
      Info("MonitorRatesUpgrade::populateChainItemMaps",
           "We will scale x%.2f for EB sample online deadtime (note this may change mid-processing if using MultiRun)",
           m_upgradeDeadtimeScaling);
    } else {
      Warning("MonitorRatesUpgrade::populateChainItemMaps", "Upgrade rates will not be scaled for online EB deadtime.");
    }

    Float_t upgradePredictionL = Config::config().getFloat(kRunLumiXML) * m_collidingBunchFactor *
                                   (m_pileupFactor / Config::config().getFloat(kOnlinePeakMuAverage));
    Info("MonitorRatesUpgrade::populateChainItemMaps",
         "*** THIS UPGRADE RATES PREDICTION IS PERFORMING A L EXTRAPOLATION OF %.2e -> %.2e ***",
         Config::config().getFloat(kRunLumiXML), upgradePredictionL);
    Config::config().setFloat(kPredictionLumiMenuXML, upgradePredictionL, "PredictionLumiMenuXML");

    if (Config::config().getInt(kDoUpgradeRatesScan) == kTRUE) {
      // Scan over a set of energies
      for (UInt_t e = 5; e <= 500; e += 5) { // Energy
        for (UInt_t n = 1; n <= 8; ++n) {
          if (n >= 5 && n <= 7) continue; // Don't do N of 5,6,7
          if (e > 100) {
            if (e % 25 != 0) continue;
          } else if (e > 50) {
            if (e % 10 != 0) continue;
          }
          for (UInt_t t = 0; t < 17; ++t) {
            if (n > 1 && (t == 9 || t == 10 || t == 12 || t == 13 || t == 14)) continue; // No multiplicity in
                                                                                               // TE,HT or XE,MHT or
                                                                                               // MU10_ME
            if ((t == 2 || t == 3) && e > 100) continue; // No EM after 100 GeV
            if ((t >= 4 && t <= 7) && e > 100) continue; // No TAU after 100 GeV
            if (t == 8 && e > 20) continue; // No MU after 20
            if ((t == 15 || t == 16) && n != 4) continue; // only 4J
            if ((t == 15 || t == 16) && e > 25) continue; // only 4J E up to 25

            Int_t iso = 0;
            Int_t etaMin = 0, etaMax = 49;
            std::string type, isoStr;
            switch (t) {
            case 0: type = "J";
              etaMax = 31;
              break;

            case 1: type = "EM";
              break;

            case 2: type = "EM";
              isoStr = "I";
              iso = m_isoBits["EM_I"];
              break;

            case 3: type = "EM";
              isoStr = "VHI";
              iso = m_isoBits["EM_VHI"];
              break;

            case 4: type = "TAU";
              break;

            case 5: type = "TAU";
              isoStr = "IL";
              iso = m_isoBits["TAU_IL"];
              break;

            case 6: type = "TAU";
              isoStr = "IM";
              iso = m_isoBits["TAU_IM"];
              break;

            case 7: type = "TAU";
              isoStr = "IT";
              iso = m_isoBits["TAU_IT"];
              break;

            case 8: type = "MU";
              break;

            case 9: type = "XE";
              break;

            case 10: type = "TE";
              break;

            case 11: type = "J";
              isoStr = "_32ETA49";
              etaMin = 32;
              etaMax = 49;
              break;

            case 12: type = "MHT";
              break;

            case 13: type = "HT";
              break;

            case 14: type = "EM";
              break; // plus mu10

            case 15: type = "J";
              etaMax = 31;
              break; // 4J plus TE

            case 16: type = "J";
              etaMax = 31;
              break; // 4J plus HT
            }
            //if (t != 0 || n != 1) continue;             // HACK
            std::string nStr = std::to_string(n);
            if (n == 1) nStr = "";
            std::string extra = "";
            if (t == 14) extra = "MU10_";

            int subE = 500;
            if (t == 15 || t == 16) subE = 250;
            for (int sE = subE; sE <= 500; sE += 50) {
              std::stringstream name;
              std::stringstream group;

              if (t == 15) extra = "TE" + std::to_string(sE) + "_";
              if (t == 16) extra = "HT" + std::to_string(sE) + "_";

              name << "L1_" << extra << nStr << type << e << isoStr;
              group << "RATE_Test_" << extra << nStr << type << isoStr;
              if (m_chainItemsL1.count(name.str()) == 1) continue;                                                                         // Already
                                                                                                                                            // has

              TriggerLogic triggerLogic;
              triggerLogic.addCondition(type, n, e, iso, etaMin, etaMax);

              if (t == 14) triggerLogic.addCondition("MU", 1, 10, 0, etaMin, etaMax);
              if (t == 15) triggerLogic.addCondition("TE", 1, sE, 0, 0, 49);
              if (t == 16) triggerLogic.addCondition("HT", 1, sE, 0, 0, 49);

              auto it = m_upgradeChains.insert(ChainInfo(name.str(), 1, triggerLogic, group.str(), "", 1., 1.));
              TriggerLogic* tl = const_cast<TriggerLogic*>(&(it->m_triggerLogic)); //Why is this const in the first
                                                                                     // place?

              RatesChainItem* L1 = new RatesChainItem(name.str(), /*chainLevel=*/ 1, 1.);
              m_chainItemsL1[ name.str() ] = L1;
              L1->setTriggerLogic(tl);
            }
          }
        }
      }
    }

    for (std::multiset<ChainInfo>::iterator it = m_upgradeChains.begin(); it != m_upgradeChains.end(); ++it) {
      //for (auto item : m_upgradeChains) {
      if (it->m_level == 2) {
        continue; // not doing HLT yet
      }
      const std::string nameL1 = it->m_name;

      if (m_chainItemsL1.count(nameL1) == 1) continue;                                                             // Already
                                                                                                                    // has

      Double_t chainPrescale = 1.; //TODO - nonzero prescales?
      RatesChainItem* L1 = new RatesChainItem(nameL1, /*chainLevel=*/ 1, chainPrescale);
      L1->setRateReductionFactor(it->m_weight0);
      m_chainItemsL1[ nameL1 ] = L1;

      std::string nameL2 = it->m_l2name;
      RatesChainItem* L2 = new RatesChainItem(nameL2, /*chainLevel=*/ 2, 1.);
      m_chainItemsL2[ nameL2 ] = L2;
      L2->setRateReductionFactor(it->m_weight1);

      L2->addLower(L1);
      L1->addUpper(L2);

      TriggerLogic* tl = const_cast<TriggerLogic*>(&(it->m_triggerLogic)); // Why are you const!?!

      L1->setTriggerLogic(tl);
      L2->setTriggerLogic(tl);
    }


    for (const auto chainItem : m_chainItemsL1) chainItem.second->classifyLumiAndRandom();
    //Should not be needed, do different lumi scaling here
    for (const auto chainItem : m_chainItemsL2) chainItem.second->classifyLumiAndRandom();
  }

  /**
   * In every other monitor, we make a new counter the first time we encounter it in the menu.
   * Rates are different, as some of the rates we want to monitor are the combinations of items in the
   * menu. Hence we pre-populate the counterMap with all combinations that we are interested in for the run.
   *
   * We make one counter per chain (L1 and HLT), one per intersection of each chain and other chains in its group
   * and one for the union of all chains in each group.
   */
  void MonitorRatesUpgrade::populateCounterMap(CounterMap_t* counterMap) {
    createGlobalCounters(counterMap);
    createL1Counters(counterMap);
    //createL2Counters(counterMap);
    //createL3Counters(counterMap);
  }

  void MonitorRatesUpgrade::createGlobalCounters(CounterMap_t* counterMap) {
    // Crate the global rates counter, this will be the OR of everything HLT
    m_globalRateL3Counter =
      new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalHLTString), 0, 10, (MonitorBase*) this); // Mint
                                                                                                                    // new
                                                                                                                    // counter
    m_globalRateL3Counter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateL3Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateL3Counter->decorate(kDecPrescaleValOnlineL1, (Float_t) 0);
    m_globalRateL3Counter->decorate(kDecType, "Union");
    // m_globalRateL3Counter->setAdvancedLumiScaling(kFALSE);
    (*counterMap)[Config::config().getStr(kRateGlobalHLTString)] = static_cast<CounterBase*>(m_globalRateL3Counter);

    // Crate the global L2 counter, this will be the OR of everything L2
    m_globalRateL2Counter =
      new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalL2String), 0, 10, (MonitorBase*) this); // Mint
                                                                                                                   // new
                                                                                                                   // counter
    m_globalRateL2Counter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateL2Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateL2Counter->decorate(kDecPrescaleValOnlineL1, (Float_t) 0);
    m_globalRateL2Counter->decorate(kDecType, "Union");
    // m_globalRateL2Counter->setAdvancedLumiScaling(kFALSE);
    (*counterMap)[Config::config().getStr(kRateGlobalL2String)] = static_cast<CounterBase*>(m_globalRateL2Counter);

    // Crate the global L1 counter, this will be the OR of everything L1
    m_globalRateL1Counter =
      new CounterRatesUnion(m_costData, Config::config().getStr(kRateGlobalL1String), 0, 10, (MonitorBase*) this); // Mint
                                                                                                                   // new
                                                                                                                   // counter
    m_globalRateL1Counter->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
    m_globalRateL1Counter->decorate(kDecRatesGroupName, Config::config().getStr(kAllString));
    m_globalRateL1Counter->decorate(kDecPrescaleValOnlineL1, (Float_t) 0);
    m_globalRateL1Counter->decorate(kDecType, "Union");
    // m_globalRateL1Counter->setAdvancedLumiScaling(kFALSE);
    (*counterMap)[Config::config().getStr(kRateGlobalL1String)] = static_cast<CounterBase*>(m_globalRateL1Counter);
  }

  /**
   * The starting point is to define what items we're interested at L1
   */
  void MonitorRatesUpgrade::createL1Counters(CounterMap_t* counterMap) {
    // // also overlaps
    // static int ID = 1000;
    // // TEMP
    // for (int jE = 10; jE <= 25; jE += 5) {
    //   for (int TE = 250; TE <= 500; TE += 50) {

    //     std::string chainName = "L1_4J" + std::to_string(jE) + "__AND__L1_TE" + std::to_string(TE);
    //     CounterRatesIntersection* L1Chain = new CounterRatesIntersection(m_costData, chainName, ID++, 10,
    // (MonitorBase*)this); // Mint new counter
    //     L1Chain->decorate(kDecRatesGroupName, "Intersection" );
    //     L1Chain->decorate(kDecPrescaleStr, "-");
    //     L1Chain->decorate(kDecType, "Intersection");
    //     L1Chain->addL1Item( m_chainItemsL1["L1_4J" + std::to_string(jE)] );
    //     L1Chain->addL1Item( m_chainItemsL1["L1_TE" + std::to_string(TE)] );
    //     assert( m_chainItemsL1.count("L1_4J" + std::to_string(jE)) +   m_chainItemsL1.count("L1_TE" +
    // std::to_string(TE)) == 2);
    //     (*counterMap)[chainName] = static_cast<CounterBase*>(L1Chain); // Insert into the counterMap
    //     Info("MonitorRatesUpgrade::createL1Counters","Made a L1 counter for: %s", L1Chain->getName().c_str() );
    //   }
    // }


    for (const auto item : m_upgradeChains) {
      if (item.m_level != 1) continue; // not doing HLT here

      const std::string chainName = item.m_name;
      const UInt_t chainID = item.m_ID;

      //Info("MonitorRatesUpgrade::createL1Counters","Create counter: %s", chainName.c_str() );

      // Find the ChainItem for this chain
      ChainItemMapIt_t it = m_chainItemsL1.find(chainName);
      if (it == m_chainItemsL1.end()) {
        Warning("MonitorRatesUpgrade::createL1Counters", "Cannot find L1 item: %s", chainName.c_str());
        continue;
      }
      RatesChainItem* chainItemL1 = it->second;

      const std::string prescaleStr = Config::config().getStr(kL1String) + ":" + doubleToString(
        chainItemL1->getPS(), 2);
      const std::string group = item.m_group + std::string("L1");


      CounterRatesChain* L1Chain = new CounterRatesChain(m_costData, chainName, chainID, 10, (MonitorBase*) this); // Mint
                                                                                                                      // new
                                                                                                                      // counter
      L1Chain->decorate(kDecRatesGroupName, group);
      L1Chain->decorate(kDecPrescaleStr, prescaleStr);
      L1Chain->decorate(kDecType, "L1");
      //L1Chain->setAdvancedLumiScaling(kFALSE);
      L1Chain->addL1Item(chainItemL1); // Link it to where it'll be getting its pass/fail info
      (*counterMap)[chainName] = static_cast<CounterBase*>(L1Chain); // Insert into the counterMap
      //Info("MonitorRatesUpgrade::createL1Counters","Made a L1 counter for: %s", L1Chain->getName().c_str() );

      //Add to global L1 counter
      m_globalRateL1Counter->addL1Item(chainItemL1);

      // Do group
      CounterMapIt_t findGroup = counterMap->find(group); // Do we have a counter for this group?
      if (findGroup != counterMap->end()) { // We do have a group already! Add to it.
        (static_cast<CounterRatesUnion*>(findGroup->second))->addL1Item(chainItemL1);
      } else {
        // We need a new group counter, this should be of type Union
        CounterRatesUnion* ratesGroup = new CounterRatesUnion(m_costData, group, 0, 10, (MonitorBase*) this); // Mint
                                                                                                                // new
                                                                                                                // counter
        //Info("MonitorRatesUpgrade::createL1Counters","Made a L1 Group counter for: %s", ratesGroup->getName().c_str()
        // );
        ratesGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
        ratesGroup->decorate(kDecPrescaleVal, (Float_t) 0.);
        ratesGroup->decorate(kDecRatesGroupName, item.m_group);
        ratesGroup->decorate(kDecType, "Union");
        //ratesGroup->setAdvancedLumiScaling(kFALSE);
        ratesGroup->addL1Item(chainItemL1); // Add initial counter
        (*counterMap)[group] = static_cast<CounterBase*>(ratesGroup); // Instert into the map
      }
    }
  }

  /**
   * L2 are like L1, but with another hardware layer added
   */
  void MonitorRatesUpgrade::createL2Counters(CounterMap_t* counterMap) {
    for (const auto item : m_upgradeChains) {
      if (item.m_level != 1) continue; // Note should be 1, L2 stored in L1 info

      std::string chainName = item.m_l2name;
      const UInt_t chainID = item.m_ID;

      // Find the ChainItem for this chain
      ChainItemMapIt_t it = m_chainItemsL2.find(chainName);
      if (it == m_chainItemsL2.end()) {
        //Warning("MonitorRatesUpgrade::createL2Counters","Cannot find L2 item: %s", chainName.c_str() );
        continue;
      }
      RatesChainItem* chainItemL2 = it->second;

      const std::string group = item.m_group + std::string("L2");
      const std::string prescaleStr = Config::config().getStr(kL1String) + ":" + doubleToString(
        chainItemL2->getPS(), 2);
      CounterRatesChain* L2Chain = new CounterRatesChain(m_costData, chainName, chainID, 10, (MonitorBase*) this); // Mint
                                                                                                                      // new
                                                                                                                      // counter
      L2Chain->decorate(kDecRatesGroupName, group);
      L2Chain->decorate(kDecPrescaleStr, prescaleStr);
      L2Chain->decorate(kDecType, "L2");
      L2Chain->addL2Item(chainItemL2); // Link it to where it'll be getting its pass/fail info
      (*counterMap)[chainName] = static_cast<CounterBase*>(L2Chain); // Insert into the counterMap

      //Info("MonitorRatesUpgrade::createL2Counters","Create counter: %s", chainName.c_str() );

      //Add to global L1 counter
      m_globalRateL2Counter->addL2Item(chainItemL2);

      // Do group
      CounterMapIt_t findGroup = counterMap->find(group); // Do we have a counter for this group?
      if (findGroup != counterMap->end()) { // We do have a group already! Add to it.
        (static_cast<CounterRatesUnion*>(findGroup->second))->addL1Item(chainItemL2);
      } else {
        // We need a new group counter, this should be of type Union
        CounterRatesUnion* ratesGroup = new CounterRatesUnion(m_costData, group, 0, 10, (MonitorBase*) this); // Mint
                                                                                                                // new
                                                                                                                // counter
        ratesGroup->decorate(kDecPrescaleStr, Config::config().getStr(kMultipleString));
        ratesGroup->decorate(kDecPrescaleVal, (Float_t) 0.);
        ratesGroup->decorate(kDecRatesGroupName, group);
        ratesGroup->decorate(kDecType, "Union");
        //ratesGroup->setAdvancedLumiScaling(kFALSE);
        ratesGroup->addL1Item(chainItemL2); // Add initial counter
        (*counterMap)[group] = static_cast<CounterBase*>(ratesGroup); // Instert into the map
      }
    }
  }

  void MonitorRatesUpgrade::createL3Counters(CounterMap_t* /*counterMap*/) {
  }

  /**
   * Get the current event's TOBs in a TOB accumulator. Read in from ntuple
   * Make sure to delete all these which go into the pileup database at the end.....
   */
  TOBAccumulator* MonitorRatesUpgrade::getEventTOBs() {
    static Float_t EMCountsPerGeV = Config::config().getInt(kUpgradeEMCountsPerGeV);
    static Float_t JetCountsPerGeV = Config::config().getInt(kUpgradeJetCountsPerGeV);
    TOBAccumulator* tobs = new TOBAccumulator();
    Bool_t hasE = kFALSE;

    for (UInt_t r = 0; r < m_costData->getNRoIs(); ++r) {
      //Info("MonitorRatesUpgrade::getEventTOBs", "Processing TOB %i of %i", r, (int) m_costData->getNRoIs());
      ConfKey_t t = kNoneString;
      if (m_costData->getIsRoINone(r) == kTRUE) continue;
      else if (m_costData->getIsRoIMuon(r) == kTRUE) t = kMuonString;
      else if (m_costData->getIsRoIEmTau(r) == kTRUE) {
        if (m_costData->getIsRoITau(r) == kTRUE) t = kTauString;
        else t = kEmString;
      } else if (m_costData->getIsRoIJet(r) == kTRUE) t = kJetString;
      else if (m_costData->getIsRoIJetEt(r) == kTRUE) continue;
      else if (m_costData->getIsRoIEnergy(r) == kTRUE) {
        //Info("MonitorRatesUpgrade::getEventTOBs", "   Is of type ENERGY");
        if (tobs->TE() > 0 && m_costData->getRoIEt(r) <= tobs->TE()) continue; // Get the largest (full width)
        hasE = kTRUE;
        tobs->set(TrigXMLService::trigXMLService().m_muPerLB.at(m_costData->getLumi()),
                   m_costData->getRoIVectorEX(r), m_costData->getRoIVectorEY(r), m_costData->getRoIEt(r),
                   m_costData->getRoIOverflowEX(r), m_costData->getRoIOverflowEY(r),
                   m_costData->getRoIOverflowET(r));
        continue;
      }
      //Info("MonitorRatesUpgrade::getEventTOBs", "   Is of type %s", Config::config().getName(t).c_str());
      TOB tob;
      tob.m_eta = m_costData->getRoIEta(r);
      tob.m_phi = m_costData->getRoIPhi(r);
      tob.m_et = m_costData->getRoIEt(r);
      tob.m_etLarge = m_costData->getRoIEtLarge(r);
      tob.m_iso = m_costData->getRoIEmTauIsoBits(r); // returns 0 for others
      tob.m_id = m_costData->getRoIID(r);
      tob.m_type = t;
      if (t == kTauString || t == kEmString) {
        tob.m_et /= EMCountsPerGeV; // Run 2 we have two counts per GeV for EM/TAU
      } else if (t == kJetString) {
        tob.m_et /= JetCountsPerGeV; // Run 2 is still usually 1 count per GeV for jets
      }
      if (tobs->TOBs().find(tob) != tobs->TOBs().end() && Config::config().getDisplayMsg(kMsgDupeTOB) == kTRUE) {
        Error("MonitorRatesUpgrade::getEventTOBs", "Event has multiple TOBs with ID %i\n%s", tob.m_id,
              tobs->print().c_str());
      }
      tobs->add(tob);
    }
    if (hasE == kFALSE && Config::config().getDisplayMsg(kMsgNoTETOB) == kTRUE) {
      Error("MonitorRatesUpgrade::getEventTOBs", "Event has no ENERGY TOB(s). Total TOBs: %i\n%s",
            m_costData->getNRoIs(), tobs->print().c_str());
      for (UInt_t r = 0; r < m_costData->getNRoIs(); ++r) Info("MonitorRatesUpgrade::getEventTOBs",
                                                                  " TOB %i/%i of id %i", r,
                                                                  (Int_t) m_costData->getNRoIs(),
                                                                  m_costData->getRoIID(r));
    }
    return tobs;
  }

  void MonitorRatesUpgrade::printEnergyTOBs() {
    for (UInt_t r = 0; r < m_costData->getNRoIs(); ++r) {
      if (m_costData->getIsRoIEnergy(r) == kTRUE) {
        Float_t met =
          TMath::Sqrt((m_costData->getRoIVectorEX(r) * m_costData->getRoIVectorEX(r)) +
                      (m_costData->getRoIVectorEY(r) * m_costData->getRoIVectorEY(r)));
        Info("MonitorRatesUpgrade::printEnergyTOBs",
             "Energy TOB x:%i y:%i (MET:%.2f) TE:%i | OverflowFlags: x:%i y:%i TE:%i",
             (Int_t) m_costData->getRoIVectorEX(r), (Int_t) m_costData->getRoIVectorEY(
               r), met, (Int_t) m_costData->getRoIEt(r),
             m_costData->getRoIOverflowEX(r), m_costData->getRoIOverflowEY(r), m_costData->getRoIOverflowET(r));
      }
    }
  }

  void MonitorRatesUpgrade::validateTriggerEmulation(CounterMap_t* counterMap, TOBAccumulator* thisAccum, Bool_t print) {
    static std::set<std::string> types;

    if (types.size() == 0) {
      types.insert("EM");
      types.insert("TAU");
      types.insert("J");
      types.insert("MU");
      types.insert("XE");
      types.insert("TE");
    }
    static std::vector<std::string> validationItems;
    if (validationItems.size() == 0) {
      validationItems.push_back("L1_MU15");
      //validationItems.push_back("L1_2MU10"); // Know broken
      validationItems.push_back("L1_EM15");
      validationItems.push_back("L1_TAU20");
      validationItems.push_back("L1_TE30");
      validationItems.push_back("L1_XE300");
      validationItems.push_back("L1_J100");
      //validationItems.push_back("L1_2EM10VH"); // Known broken
    }
    Int_t checkTrigEmulation[10] = {
      0
    };
    Int_t checkTrigOnline[10] = {
      0
    };
    if (print) Info("MonitorRatesUpgrade::validateTriggerEmulation",
                     " ###  ###  ###  ###  ###  ###  TRIGGER EMULATION CHECK  ###  ###  ###  ###  ###  ###");
    if (print) Info("MonitorRatesUpgrade::validateTriggerEmulation", " Event: %i TOBs, MET:(%.0f,%.0f)=%f TE:%f\n%s",
      (int) thisAccum->TOBs().size(), thisAccum->vX(), thisAccum->vY(), thisAccum->MET(), thisAccum->TE(), thisAccum->print().c_str());

    for (auto type : types) {
      if (print) Info("MonitorRatesUpgrade::validateTriggerEmulation", " *** %s ***", type.c_str());
      CounterMapIt_t it = counterMap->begin();
      std::stringstream ssEmu;
      for (; it != counterMap->end(); ++it) {
        if (it->second->getName().find(type) == std::string::npos) continue;
        if (it->second->getName().substr(0, 3) != "L1_") continue;
        if (isZero(((CounterRatesChain*) (it->second))->runWeight()) == kTRUE) continue;
        for (UInt_t v = 0; v < validationItems.size(); ++v) if (it->second->getName() == validationItems.at(v)) checkTrigEmulation[v] = 1;
        ssEmu << it->second->getName() << ", ";
      }
      if (print && ssEmu.str().size()) Info("MonitorRatesUpgrade::validateTriggerEmulation", " ^^^ EMULATED PASS: %s", ssEmu.str().c_str());
      std::stringstream ssOnline;
      for (UInt_t c = 0; c < m_costData->getNL1(); ++c) {
        if (m_costData->getIsL1PassedBeforePrescale(c) == kFALSE) continue;
        Int_t CTPID = m_costData->getL1CtpId(c);
        const std::string chainName = TrigConfInterface::getNameFromCtpId(CTPID);
        if (chainName.find(type) == std::string::npos) continue;
        if (chainName.find("-") != std::string::npos) continue;
        for (UInt_t v = 0; v < validationItems.size(); ++v) if (chainName == validationItems.at(v)) checkTrigOnline[v] = kTRUE;
        ssOnline << chainName << ", ";
      }
      if (print && ssOnline.str().size()) Info("MonitorRatesUpgrade::validateTriggerEmulation", " &&& ONLINE PASS: %s", ssOnline.str().c_str());
    }
    if (print) printEnergyTOBs();
    if (print == kFALSE) {
      for (UInt_t v = 0; v < validationItems.size(); ++v) {
        if (checkTrigEmulation[v] != checkTrigOnline[v]) {
          Info("MonitorRatesUpgrade::validateTriggerEmulation", " !!!!!!!!!!!!!!!!!!!!!!!!! %s !!!!!!!!!!!!!!!!!!!!!!!", validationItems.at(v).c_str());
          Info("MonitorRatesUpgrade::validateTriggerEmulation", " !!!!!!!!!!!!!!!!!!!! FAILED TO VALIDATE !!!!!!!!!!!!!!!!!!!!");
          validateTriggerEmulation(counterMap, thisAccum, kTRUE);
          break;
        }
      }
    }
  }

  /**
   * Process new event for this monitor.
   * For the chain rate monitor - we look at a chain's rate and its correlation with other chains in its group
   * @param weight We don't use the standard weight here - form it ourselves
   */
  void MonitorRatesUpgrade::newEvent(Float_t /*weight*/) {
    m_timer.start();
    if (Config::config().debug()) Info("MonitorRatesUpgrade::newEvent", "*** Processing Chain Rates ***");

    // First time? Setup the ChainItem maps to hold the decision and prescale information for the rates counters to use.
    if (m_chainItemsL1.size() == 0) populateChainItemMaps();

    // Require physics events here (not other bunchgroups)
    if (Config::config().getInt(kCurrentEventBunchGroupID) != 1) return;

    if (m_upgradePileupScaling == kTRUE) {
      // FIRST PASS
      // If FIRST PASS, all we do is build the PU database
      static UInt_t message = 0;
      if (message == 0) {
        message = 1;
        Info("MonitorRatesUpgrade::newEvent", "Building the Pileup Database...");
      }
      if (getPass() == 1) {
        if (Config::config().getInt(kCurrentEventWasRandomOnline) == kTRUE) m_pileupDatabase.push_back(getEventTOBs());
        return;
      }
      if (message == 1) {
        message = 2;
        Info("MonitorRatesUpgrade::newEvent", "Moving on to second pass, Pileup Database has:%i events", (Int_t) m_pileupDatabase.size());
      }
    }

    // This is now a fn. of the LB as we may be running in MultiRun mode
    // We always take the nPaired of the `primary' interaction
    if (m_upgradeBunchScaling) {
      assert(TrigXMLService::trigXMLService().m_pairedBunchesPerLB.count(m_costData->getLumi()) == 1);
      m_collidingBunchFactor = m_targetPairedBunches / (Float_t) TrigXMLService::trigXMLService().m_pairedBunchesPerLB[ m_costData->getLumi() ];
    }

    if (m_doDeadtimeScaling) {
      m_upgradeDeadtimeScaling = 1. + TrigXMLService::trigXMLService().m_deadtimePerLB[ m_costData->getLumi() ];
    }

    // We do all the weighting here ourselves
    Float_t weightUpgrade = Config::config().getFloat(kCurrentEventEBWeight) * m_collidingBunchFactor * m_upgradeDeadtimeScaling;

    // SECOND PASS
    static Int_t tobDebug = 0;
    // if (++tobDebug < 50) Info("MonitorRatesUpgrade::newEvent", " *******************************************");

    TOBAccumulator* thisEventPtr = getEventTOBs();

    if (m_upgradePileupScaling) {
      m_collisionsToMix += m_pileupFactor; // Want another ~200 collisions

      Int_t eventsAdded = 0;
      while (m_thisEventPtr->mu() < m_collisionsToMix) {
        UInt_t pileupLottery = m_R3.Integer(m_pileupDatabase.size());
        m_thisEventPtr->add(m_pileupDatabase.at(pileupLottery));
        ++eventsAdded;
      }

      if (++tobDebug < 50) Info("MonitorRatesUpgrade::newEvent", "Target collisions: %.2f | Adding %i events, <mu> = %.2f", m_collisionsToMix, eventsAdded, m_thisEventPtr->mu());
      m_collisionsToMix -= m_thisEventPtr->mu(); // Take this off so any excess is taken into account next time

      m_statsEventMu += thisEventPtr->mu();
      m_statsEventMu2 += pow(thisEventPtr->mu(), 2);
      ++m_statsEventMuN;
    }



    //if ( fabs( m_eventsToMix - round(m_eventsToMix) ) <  0.1) m_eventsToMix = round(m_eventsToMix);
    ////static Int_t debug2 = 0;
    // if (++debug2 < 20) Info("MonitorRatesUpgrade::newEvent", "Mixing %i events (f %f)", (Int_t)
    // round(m_eventsToMix), m_eventsToMix);
    // // Add pileup
    // for (Int_t pu = 0; pu <  (Int_t) round(m_eventsToMix); ++pu) { // Cast to int rounds down
    //   UInt_t pileupLottery = m_R3.Integer( m_pileupDatabase.size() );
    //   m_thisEventPtr->add( m_pileupDatabase.at( pileupLottery ) );
    // }
    // if (tobDebug < 20) Info("MonitorRatesUpgrade::newEvent", " ### MIXING %i EVENTS, NOW HAVE %i TOBs ### MET:%f
    // TE:%f\n%s", (Int_t) m_eventsToMix, (Int_t) m_thisEventPtr->TOBs().size(), m_thisEventPtr->MET(),
    // m_thisEventPtr->TE(),m_thisEventPtr->print().c_str());
    // m_eventsToMix -= (Int_t)m_eventsToMix; // Remove whole integer events mixed. Leave fractional part to accumulate
    // for next event.

    for (const auto chainItem : m_chainItemsL1) chainItem.second->beginEvent(m_thisEventPtr);
    for (const auto chainItem : m_chainItemsL2) chainItem.second->beginEvent(m_thisEventPtr);

    //Now loop over the counter collections;
    for (CounterMapSetIt_t cmsIt = m_collectionsToProcess.begin(); cmsIt != m_collectionsToProcess.end(); ++cmsIt) {
      CounterMap_t* counterMap = *cmsIt; // This counter map holds all the counters.

      // If the counter map is empty, then we need to populate it. We will pre-load a counter for every chain.
      if (counterMap->size() == 0) populateCounterMap(counterMap);

      // Loop over all counters, they will use their pre-linked RatesChainItems to get their weights.
      CounterMapIt_t it = counterMap->begin();
      for (; it != counterMap->end(); ++it) {
        it->second->processEventCounter(0, 1, weightUpgrade);
        m_countersInEvent.insert(it->second);
      }
      //if (m_upgradePileupScaling == kFALSE) validateTriggerEmulation(counterMap, thisEventPtr, kFALSE /*print*/);

      endEvent(0);
    }

    for (const auto chainItem : m_chainItemsL1) chainItem.second->endEvent();
    for (const auto chainItem : m_chainItemsL2) chainItem.second->endEvent();

    delete thisEventPtr;
    thisEventPtr = nullptr;

    m_timer.stop();
  }

  /**
   * Saves the topology of all counters to a JSON file linked to the counter name. Imported by web display.
   */
  void MonitorRatesUpgrade::saveRateGraphs() {
    const std::string output = Config::config().getStr(kOutputDirectory) + "/" + Config::config().getStr(kOutputRatesGraphFilename);

    std::ofstream fout(output.c_str());
    JsonExport json;
    json.addNode(fout, "ratesGaphs");

    std::set<std::string> configsSeen;
    for (CounterCollectionIt_t collectionIt = m_counterCollections.begin(); collectionIt != m_counterCollections.end(); ++collectionIt) {
      for (CounterMapIt_t counterMapIt = collectionIt->second.begin(); counterMapIt != collectionIt->second.end(); ++counterMapIt) {
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
          for (ChainItemSetIt_t L1It = counter->getL1ItemSet().begin(); L1It != counter->getL1ItemSet().end(); ++L1It) {
            RatesChainItem* L1 = (*L1It);
            const std::string source = L1->getName() + " [PS:" + doubleToString(L1->getPS()) + "]";
            json.addLeafCustom(fout, "source", source, "target", source);
          }
        } else if (counter->getL3ItemSet().size() == 0) { // If only L1 and L2
          for (ChainItemSetIt_t L2It = counter->getL2ItemSet().begin(); L2It != counter->getL2ItemSet().end(); ++L2It) {
            RatesChainItem* L2 = (*L2It);
            for (ChainItemSetIt_t L1It = L2->getLowerStart(); L1It != L2->getLowerEnd(); ++L1It) {
              RatesChainItem* L1 = (*L1It);
              const std::string source = L1->getName() + " [PS:" + doubleToString(L1->getPS()) + "]";
              const std::string target = L2->getName() + " [PS:" + doubleToString(L2->getPS()) + "]";
              json.addLeafCustom(fout, "source", source, "target", target);
            }
          }
        } else { // all three levels
          for (ChainItemSetIt_t L3It = counter->getL3ItemSet().begin(); L3It != counter->getL3ItemSet().end(); ++L3It) {
            RatesChainItem* L3 = (*L3It);
            for (ChainItemSetIt_t L2It = L3->getLowerStart(); L2It != L3->getLowerEnd(); ++L2It) {
              RatesChainItem* L2 = (*L2It);
              const std::string source = L2->getName() + " [PS:" + doubleToString(L2->getPS()) + "]";
              const std::string target = L3->getName() + " [PS:" + doubleToString(L3->getPS()) + "]";
              json.addLeafCustom(fout, "source", source, "target", target);
              for (ChainItemSetIt_t L1It = L2->getLowerStart(); L1It != L2->getLowerEnd(); ++L1It) {
                RatesChainItem* L1 = (*L1It);
                const std::string source = L1->getName() + " [PS:" + doubleToString(L1->getPS()) + "]";
                const std::string target = L2->getName() + " [PS:" + doubleToString(L2->getPS()) + "]";
                json.addLeafCustom(fout, "source", source, "target", target);
              }
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
  Bool_t MonitorRatesUpgrade::getIfActive(ConfKey_t mode) {
    switch (mode) {
    case kDoAllSummary:       return kTRUE;

    case kDoKeySummary:       return kTRUE;

    case kDoLumiBlockSummary: return kTRUE;

    default: Error("MonitorRatesUpgrade::getIfActive", "An invalid summary mode was provided (key %s)", Config::config().getName(mode).c_str());
    }
    return kFALSE;
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorRatesUpgrade::saveOutput() {
    // Send finalise calls

    Info("MonitorRatesUpgrade::saveOutput", "Target <mu> = %.4f, Obtained <mu> = %.4f", m_pileupFactor, m_statsEventMu / (Float_t) m_statsEventMuN);
    Info("MonitorRatesUpgrade::saveOutput", "Sum(Collisions)=%.3e Sum(Collisions^2)=%.3e Events:%i", m_statsEventMu, m_statsEventMu2, m_statsEventMuN);


    for (CounterCollectionIt_t collectionIt = m_counterCollections.begin(); collectionIt != m_counterCollections.end(); ++collectionIt) {
      const std::string counterCollectionName = collectionIt->first;
      // Finalise unique counters
      if (Config::config().getInt(kDoUniqueRates) == kTRUE) {
        for (CounterMapIt_t counterMapIt = collectionIt->second.begin(); counterMapIt != collectionIt->second.end(); ++counterMapIt) {
          CounterBaseRates* counter = static_cast<CounterBaseRates*>(counterMapIt->second);
          if (counter->getStrDecoration(kDecType) == "UniqueHLT" || counter->getStrDecoration(kDecType) == "UniqueL1") {
            counter->decorate(kDecLbLength, m_collectionLumiCollector[ counterCollectionName ]->getTotalLumiBlockTime());
            counter->finalise();
          }
        }
      }
      // Finalise all other counters (makes use of unique counters)
      for (CounterMapIt_t counterMapIt = collectionIt->second.begin(); counterMapIt != collectionIt->second.end(); ++counterMapIt) {
        //Note we have added a negation here
        CounterBaseRates* counter = static_cast<CounterBaseRates*>(counterMapIt->second);
        if (!(counter->getStrDecoration(kDecType) == "UniqueHLT" || counter->getStrDecoration(kDecType) == "UniqueL1")) {
          counter->decorate(kDecLbLength, m_collectionLumiCollector[ counterCollectionName ]->getTotalLumiBlockTime());
          counter->finalise();
        }
      }
    }

    TrigXMLService::trigXMLService().saveRuleBookXML(m_counterCollections, getLevelStr());

    if (Config::config().getInt(kOutputRatesGraph) == kTRUE) saveRateGraphs();

    VariableOptionVector_t toSavePlots = m_dummyCounter->getAllHistograms();

    std::vector<TableColumnFormatter> toSaveTable;

    toSaveTable.push_back(TableColumnFormatter("Active Time [s]",
                                                "Integrated length of all lumi blocks which contributed events to this rates prediction.",
                                                kVarEventsPassedNoPS /*ignored*/, kSavePerCall /*ignored*/, 2, kFormatOptionUseWallTime));

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
                                                  kDecUniqueFraction, kSavePerCall, 0, kFormatOptionUseStringDecoration));
    }

    toSaveTable.push_back(TableColumnFormatter("Unbiased Pass [%]",
                                                "The probability that this chain passes events selected online with zero trigger bias",
                                                kVarUnbiasedPassed, kSavePerCall, kVarUnbiasedRun, kSavePerCall, 5, kFormatOptionToPercentage));

    toSaveTable.push_back(TableColumnFormatter("Prescale",
                                                "The prescale of this chain. Only displayed for simple combinations.",
                                                kDecPrescaleStr, kSavePerCall, 0, kFormatOptionUseStringDecoration));

    if (Config::config().getInt(kRatesScaleByPS)) {
      toSaveTable.push_back(TableColumnFormatter("L1 Online Prescale",
                                                  "The online prescale of this chain at L1. Applied here as a weight.",
                                                  kDecPrescaleValOnlineL1, kSavePerCall, 0, kFormatOptionUseFloatDecoration));
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
                                                kVarEventsRun, kSavePerCall, 4, kFormatOptionNormaliseWallTime));

    toSaveTable.push_back(TableColumnFormatter("Pass Fraction before PS [%]",
                                                "Fraction of events which pass this trigger before prescale.",
                                                kVarEventsPassedNoPS, kSavePerCall, kVarEventsRun, kSavePerCall, 4, kFormatOptionToPercentage));

    toSaveTable.push_back(TableColumnFormatter("Pass Fraction after PS [%]",
                                                "Fraction of events which pass this trigger after prescale.",
                                                kVarEventsPassed, kSavePerCall, kVarEventsRun, kSavePerCall, 4, kFormatOptionToPercentage));

    toSaveTable.push_back(TableColumnFormatter("Pass Weighted PS",
                                                "Number of events this chain or combination passed after applying prescales as weighting factors.",
                                                kVarEventsPassed, kSavePerCall, 4));


    // We save in three steps to put the different categories in different folders
    // First the chains, veto on AND and Rate (group) counters
    m_filterOutput = kTRUE;
    setName("Rate_Upgrade_ChainL1");
    filterOutputOnStrDecoration(kDecType, "L1");
    sharedTableOutputRoutine(toSaveTable);
    sharedHistogramOutputRoutine(toSavePlots);

    setName("Rate_Upgrade_ChainL2");
    filterOutputOnStrDecoration(kDecType, "L2");
    sharedTableOutputRoutine(toSaveTable);
    sharedHistogramOutputRoutine(toSavePlots);

    setName("Rate_Upgrade_ChainHLT");
    filterOutputOnStrDecoration(kDecType, "HLT");
    sharedTableOutputRoutine(toSaveTable);
    sharedHistogramOutputRoutine(toSavePlots);

    // Now do the GROUP - note we can remove the "Group" option from the table.
    //toSaveTable.erase( toSaveTable.begin() + 1 );
    setName("Rate_Upgrade_Group");
    filterOutputOnStrDecoration(kDecType, "Union");
    sharedTableOutputRoutine(toSaveTable);
    sharedHistogramOutputRoutine(toSavePlots);

    setName("Rate_Upgrade_Combination");
    filterOutputOnStrDecoration(kDecType, "Intersection");
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
  CounterBase* MonitorRatesUpgrade::newCounter(const std::string& /*name*/, Int_t /*ID*/) {
    Error("MonitorRatesUpgrade::newCounter", "For Rates counters - please mint them yourself as the derived type is important (Chain, Union, Intersection etc.)");
    return 0;
  }
} // namespace TrigCostRootAnalysis
