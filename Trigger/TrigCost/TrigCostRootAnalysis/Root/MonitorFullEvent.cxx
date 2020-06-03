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

//Local include(s):
#include "../TrigCostRootAnalysis/MonitorFullEvent.h"
#include "../TrigCostRootAnalysis/CounterAlgorithm.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Utility.h"
#include "../TrigCostRootAnalysis/Config.h"

namespace TrigCostRootAnalysis {
  /**
   * Monitor constructor. Sets name and calls base constructor.
   */
  MonitorFullEvent::MonitorFullEvent(const TrigCostData* costData) : MonitorBase(costData, "Full_Evnt"),
    m_probability(0.),
    m_usePickList(kFALSE),
    m_rando(4), // Seed chosen by fair dice role.
    m_randomString("random"),
    m_slowString("slow"),
    m_doPick(kFALSE),
    m_doRND(kFALSE) {
    m_dummyCounter = newCounter(Config::config().getStr(kDummyString), INT_MIN);
    //allowSameNamedCounters(); //Allow 'degenerate' counter names - get a different counter for each alg execution.
    setDetailLevel(0); // Don't save any histograms for all counters spawned from here

    m_probability = 1. / (Float_t) Config::config().getInt(kFullEventSaveOnePer);

    m_usePickList = Config::config().getIsSet(kEventPickList);
  }

  /**
   * Overloaded.
   * This monitor is interested in events at random or events on a pick list. Check these criteria
   */
  Int_t MonitorFullEvent::getNCollectionsToProcess() {
    m_doPick = kFALSE;
    m_doRND = kFALSE;

    if (m_usePickList == kTRUE) {
      m_doPick = Config::config().getIntVecMatches(kEventPickList, m_costData->getEventNumber());
    } else {
      if (m_rando.Rndm() < m_probability) m_doRND = kTRUE;
    }

    if (m_doPick || m_doRND) return 1;

    return 0;
  }

  /**
   * Overloaded.
   * @return false - as this monitor needs the Globals monitor to have run to know if the event was slow or not
   */
  Bool_t MonitorFullEvent::isThreadable() {
    return kTRUE;
  }

  /**
   * Process new event for this monitor.
   * The full event execution will be summarised.
   * @param weight The event weight.
   */
  void MonitorFullEvent::newEvent(Float_t weight) {
    m_timer.start();
    Bool_t doSlow = (Bool_t) Config::config().getInt(kCurrentEventIsSlow);

    if ((m_doPick || doSlow || m_doRND) == kFALSE) {
      m_timer.stop();
      return;
    }

    // Check this is a monitoring event - no point saving it if not
    if (m_costData->getIsMonitoringEvent() == kFALSE) {
      m_timer.stop();
      return;
    }

    std::string type;
    if (m_doPick == kTRUE || m_doRND == kTRUE) type = m_randomString;
    else type = m_slowString;

    // Check if we have already saved our max number of events.
    // In this monitor, we use a new counter collection for every event.
    // Gets a little confusing as we need to see what type of saved event it was by looking at one of the counters.
    if (m_doPick == kFALSE) {
      Int_t count = 0;
      for (CounterCollectionIt_t ccIt = m_counterCollections.begin(); ccIt != m_counterCollections.end(); ++ccIt) {
        // Get first counter
        if ((ccIt->second).size() == 0) continue;
        CounterMapIt_t mapIt = (ccIt->second).begin();
        if (m_doRND == kTRUE && mapIt->second->getStrDecoration(kDecCounterClassification) == m_randomString) ++count;

        else if (doSlow == kTRUE &&
                 mapIt->second->getStrDecoration(kDecCounterClassification) == m_slowString) ++count;
      }
      if (count >= Config::config().getInt(kFullEventMaxNumToSave)) {
        if (Config::config().debug()) {
          Warning("MonitorFullEvent::newEvent", "Reached maximum number of Full-Events to save (%i). Skipping.",
                  (Int_t) m_counterCollections.size());
        }
        m_timer.stop();
        return;
      }
    }

    // For this monitor - we have a CounterCollection per event.
    // The event number is used as the key. Pad with 6 0's
    const std::string eventNumberStr = std::string("Event_") + intToString(m_costData->getEventNumber(), 6);
    CounterMap_t* eventMap = getCounterCollection(eventNumberStr, kDoAllSummary); // We don't care what this counter
                                                                                    // collection is classified as so
                                                                                    // second parameter is not important

    // Reset this var
    Config::config().setFloat(kEventStartTime, 0, "EventStart", kUnlocked);

    if (Config::config().getDisplayMsg(kMsgSaveFullEvent) == kTRUE) {
      Info("MonitorFullEvent::newEvent", "Saving full %s event for event number %s",
           type.c_str(), eventNumberStr.c_str());
    }

    // Loop over all sequences.
    Int_t location = 0; // Algs are in the D3PD in execution order - note this order so we can sort by it later
    for (UInt_t s = 0; s < m_costData->getNSequences(); ++s) {
      // Get the name of the chain this sequence instance is in (Supplying L2 or EF helps, but is not needed)
      Int_t chainID = m_costData->getSequenceChannelCounter(s);
      const std::string chainName =
        TrigConfInterface::getHLTNameFromChainID(chainID, m_costData->getSequenceLevel(s));

      // Loop over all algorithms in sequence
      for (UInt_t a = 0; a < m_costData->getNSeqAlgs(s); ++a) {
        Int_t seqIndex = m_costData->getSequenceIndex(s);
        const std::string seqName = TrigConfInterface::getHLTSeqNameFromIndex(seqIndex);
        Int_t seqAlgPos = m_costData->getSeqAlgPosition(s, a);

        const std::string algName = TrigConfInterface::getHLTAlgNameFromSeqIDAndAlgPos(seqIndex, seqAlgPos);
        const std::string algType = TrigConfInterface::getHLTAlgClassNameFromSeqIDAndAlgPos(seqIndex, seqAlgPos);

        Int_t seqAlgNameHash = TrigConfInterface::getHLTAlgClassNameIDFromSeqIDAndAlgPos(seqIndex, seqAlgPos);

        // Count how many we have already saved
        UInt_t nCountersOfThisAlg = 0;
        for (CounterMapIt_t it = eventMap->begin(); it != eventMap->end(); ++it) {
          if (it->first.find(algName) != std::string::npos) ++nCountersOfThisAlg;
        }

        // Change the name such that we always get a new counter
        const std::string algNewName = algName + std::string("[") + intToString(nCountersOfThisAlg) +
                                        std::string("]");
        CounterBase* counter = getCounter(eventMap, algNewName, seqAlgNameHash);
        counter->decorate(kDecAlgClassName, algType);
        counter->decorate(kDecChainName, chainName);
        counter->decorate(kDecSeqName, seqName);
        counter->decorate(kDecCounterClassification, type);
        counter->decorate(kDecID, location++);
        counter->processEventCounter(s, a, weight);
      }
    }

    endEvent(weight);
    m_timer.stop();
  }

  /**
   * The full event monitor does not care for ranges, this should never be called
   * @return false
   */
  Bool_t MonitorFullEvent::getIfActive(ConfKey_t mode) {
    Error("MonitorFullEvent::getIfActive", "Does not make sense to call for the FullEvent monitor (key %s)",
          Config::config().getName(mode).c_str());
    return kFALSE;
  }

  /**
   * Construct new counter of correct derived type, pass back as base type.
   * This function must be implemented by all derived monitor types.
   * @see MonitorBase::addCounter( const std::string &_name, Int_t ID )
   * @param name Cost reference to name of counter.
   * @param ID Reference to ID number of counter.
   * @returns Base class pointer to new counter object of correct serived type.
   */
  CounterBase* MonitorFullEvent::newCounter(const std::string& name, Int_t ID) {
    // Note - we use Algorithm counter objects in this monitor
    return new CounterAlgorithm(m_costData, name, ID, m_detailLevel, (MonitorBase*) this);
  }

  /**
   * Save the results from this monitors counters as specified in the configuration.
   */
  void MonitorFullEvent::saveOutput() {
    // No histogram output for MonitorFullEvent

    // Define output table
    std::vector<TableColumnFormatter> toSave;

    toSave.push_back(TableColumnFormatter("Execution ID",
                                           "0 is the first algorithm to execute in the event. 1 is the second and so on.",
                                           kDecID, kSavePerCall, 0, kFormatOptionUseIntDecoration));

    //toSave.push_back( TableColumnFormatter("Start Time (s)",
    //  "Timestamp of the begining of this algorithms execution.",
    //  kDecStartTime, kSavePerCall, 4, kFormatOptionUseFloatDecoration ) );

    toSave.push_back(TableColumnFormatter("Elapsed Time (s)",
                                           "How long into the execution was it before this alg was called.",
                                           kDecElapsedTime, kSavePerCall, 4, kFormatOptionUseFloatDecoration));

    toSave.push_back(TableColumnFormatter("Chain Name",
                                           "Chain which requested this algorithm call.",
                                           kDecChainName, kSavePerCall, 0, kFormatOptionUseStringDecoration));

    toSave.push_back(TableColumnFormatter("Sequence Step",
                                           "Sequence step of this algorithm.",
                                           kDecSeqName, kSavePerCall, 0, kFormatOptionUseStringDecoration));

    toSave.push_back(TableColumnFormatter("Algorithm Class",
                                           "Class name of this algorithm instance.",
                                           kDecAlgClassName, kSavePerCall, 0, kFormatOptionUseStringDecoration));

    toSave.push_back(TableColumnFormatter("RoI ID(s)",
                                           "Regions of interest to be processed by this algorithm.",
                                           kDecROIString, kSavePerCall, 0, kFormatOptionUseStringDecoration));

    toSave.push_back(TableColumnFormatter("Algorithm Status",
                                           "If the result of this algorithm call was cached.",
                                           kDecCallOrCache, kSavePerCall, 0, kFormatOptionUseStringDecoration));

    toSave.push_back(TableColumnFormatter("Algorithm Total Time (ms)",
                                           "Total (CPU + ROS) time for this algorithm execution",
                                           kVarTime, kSavePerCall, 2));

    toSave.push_back(TableColumnFormatter("# Data Requests",
                                           "Number of calls to the ROS made by this algorithm execution",
                                           kVarROSCalls, kSavePerCall, 0));

    toSave.push_back(TableColumnFormatter("ROB Time (ms)",
                                           "Time spent retrieving data from ROBs for this algorithm execution.",
                                           kVarROSTime, kSavePerCall, 2));

    toSave.push_back(TableColumnFormatter("# Cached ROBs",
                                           "Number of cached ROBs fetched by this algorithm execution.",
                                           kVarROBReqs, kSavePerCall, 0));

    toSave.push_back(TableColumnFormatter("# Retrieved ROBs",
                                           "Number of ROBs retrieved by this algorithm execution",
                                           kVarROBRets, kSavePerCall, 0));

    toSave.push_back(TableColumnFormatter("Cached ROBs Size (kB)",
                                           "Size of cached ROBs fetched by this algorithm execution.",
                                           kVarROBReqSize, kSavePerCall, 2));

    toSave.push_back(TableColumnFormatter("Retrieved ROBs Size (kB)",
                                           "Size of ROBs fetched by this algorithm execution.",
                                           kVarROBRetSize, kSavePerCall, 2));

    toSave.push_back(TableColumnFormatter("Data Requests",
                                           "List of ROS calls by this algorithm and their multiplicity.",
                                           kDecROSString, kSavePerCall, 0, kFormatOptionUseStringDecoration));

    m_filterOutput = kTRUE;
    filterOutputOnStrDecoration(kDecCounterClassification, m_randomString);
    sharedTableOutputRoutine(toSave);

    setName("Slow_Evnt");
    filterOutputOnStrDecoration(kDecCounterClassification, m_slowString);
    sharedTableOutputRoutine(toSave);
  }
} // namespace TrigCostRootAnalysis

// Store some old code here - could come in handy again
// void CounterFullEvent::debug(UInt_t /*e*/) {
//   // Loop over TEs
//   std::ofstream fout(std::string("te_test"+getName()+".dot").c_str());
//   fout << "digraph G{" << std::endl;
//   for (UInt_t i = 0; i < m_costData->getNTEs(); ++i) {
//     fout << "\t" << m_costData->getTEIndex(i) << "[label=\"<f0>" <<

//          (m_costData->getIsTERegularTe(i) ? "R|" : "" ) <<
//          (m_costData->getIsTEInitial(i) ? " I|" : "" ) <<
//          (m_costData->getIsTERoITe(i) ? "RoI|" : "" ) <<
//          (m_costData->getIsTEL1Threshold(i) ? "L1|" : "") <<

//          "<f1> " <<

//          (m_costData->getIsTEActiveState(i) ? "A|" : "" ) <<
//          (m_costData->getIsTEErrorState(i) ? "E|" : "" ) <<
//          (m_costData->getIsTEOutputEFNode(i) ? "OEF|" : "" ) <<
//          (m_costData->getIsTEOutputL2Node(i) ? "OL2|" : "" ) <<
//          (m_costData->getIsTETerminalNode(i) ? "TRM|" : "" ) <<
//          (m_costData->getIsTETopologicalTe(i) ? "TOPO|" : "" ) <<

//          "<f2> ";


//     for (UInt_t c = 0; c < m_costData->getTERoIIDSize(i); ++c) {
//       Int_t r = m_costData->getRoIIndexFromId( m_costData->getTERoIIDIndex(i, c) );
//       if (r != -1 && r != 255) {
//         fout << "RoIThr:" << m_costData->getRoINL1Thresh(r) << "-";
//         if (m_costData->getIsRoIEmTau(r)) fout << "EMTAU";
//         if (m_costData->getIsRoIEnergy(r)) fout << "E";
//         if (m_costData->getIsRoIJet(r))  fout << "JET";
//         if (m_costData->getIsRoIJetEt(r))  fout << "JETET";
//         if (m_costData->getIsRoIMuon(r))  fout << "MU";
//         if (m_costData->getIsRoINone(r))  fout << "NONE";
//         fout << " (" << m_costData->getRoIEta(r) << "," << m_costData->getRoIPhi(r) << ")|";
//       }
//     }
//     fout << "\"";
//     fout << std::endl << "\tshape = \"record\"];" << std::endl;

//     std::cout << "TE Index:" << m_costData->getTEIndex(i) << " ID:" << m_costData->getTEID(i) << " Children:{";
//     for (UInt_t c = 0; c < m_costData->getTEChildSize(i); ++c) {
//       std::cout << m_costData->getTEChildIndex(i, c) << ",";
//       fout << "\t" << m_costData->getTEIndex(i)  << ":f0 -> " << m_costData->getTEChildIndex(i, c) << ";" <<
// std::endl;
//     }
//     std::cout << "} Parents:{";
//     for (UInt_t c = 0; c < m_costData->getTEParentSize(i); ++c) {
//       std::cout << m_costData->getTEParentIndex(i, c) << ",";
//     }
//     std::cout << "} CLID:{";
//     for (UInt_t c = 0; c < m_costData->getTECLIDSize(i); ++c) {
//       std::cout << m_costData->getTECLIDIndex(i, c) << ",";
//     }
//     std::cout << "} RoIID:{";
//     for (UInt_t c = 0; c < m_costData->getTERoIIDSize(i); ++c) {
//       std::cout << m_costData->getTERoIIDIndex(i, c) << "[";
//       Int_t r = m_costData->getRoIIndexFromId( m_costData->getTERoIIDIndex(i, c) );
//       if (r != -1) {
//         std::cout << "NL1Thresh:" << m_costData->getRoINL1Thresh(r) << "|";
//         if (m_costData->getIsRoIEmTau(r))  std::cout << "EMTAU|";
//         if (m_costData->getIsRoIEnergy(r))  std::cout << "ENERGY|";
//         if (m_costData->getIsRoIJet(r))  std::cout << "JET|";
//         if (m_costData->getIsRoIJetEt(r))  std::cout << "JETET|";
//         if (m_costData->getIsRoIMuon(r))  std::cout << "MU|";
//         if (m_costData->getIsRoINone(r))  std::cout << "NONE|";
//         std::cout << m_costData->getRoIEta(r) << "|" << m_costData->getRoIPhi(r);
//       }
//       std::cout << "],";
//     }
//     std::cout << "}";
//     if (m_costData->getIsTERegularTe(i)) std::cout << " <isRegularTE>";
//     if (m_costData->getIsTEInitial(i)) std::cout << " <isInitialTe>";
//     if (m_costData->getIsTERoITe(i)) std::cout << " <isRoITE>";
//     if (m_costData->getIsTEL1Threshold(i)) std::cout << " <isL1Thresh>";
//     if (m_costData->getIsTEActiveState(i)) std::cout << " isActiveState";
//     if (m_costData->getIsTEErrorState(i)) std::cout << " isErrorState";
//     if (m_costData->getIsTEOutputEFNode(i)) std::cout << " isOutputEFNode";
//     if (m_costData->getIsTEOutputL2Node(i)) std::cout << " isOutputL2Node";
//     if (m_costData->getIsTETerminalNode(i)) std::cout << " isTerminalNode";
//     if (m_costData->getIsTETopologicalTe(i)) std::cout << " isTopoTE";
//     std::cout << std::endl;
//   }
//   fout << "}";
//   fout.close();
// }
