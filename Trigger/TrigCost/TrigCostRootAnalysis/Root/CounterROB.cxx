// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------

// STL include(s):
#include <iostream>

// Local include(s):
#include "../TrigCostRootAnalysis/MonitorROSCommon.h"
#include "../TrigCostRootAnalysis/CounterROB.h"
#include "../TrigCostRootAnalysis/TrigCostData.h"
#include "../TrigCostRootAnalysis/Config.h"
#include "../TrigCostRootAnalysis/TrigConfInterface.h"
#include "../TrigCostRootAnalysis/TrigXMLService.h"

namespace TrigCostRootAnalysis {
  /**
   * Read Out Buffer counter constructor. Sets values of internal variables and sets up data store.
   * @param name Const ref to name of Read Out Buffer
   * @param ID ID number of Read Out Buffer
   */
  CounterROB::CounterROB(const TrigCostData* costData, const std::string& name, Int_t ID, UInt_t detailLevel,
                         MonitorBase* parent)
    : CounterBase(costData, name, ID, detailLevel, parent) {
    if (m_detailLevel == 0) m_dataStore.setHistogramming(kFALSE);

    m_dataStore.newVariable(kVarEventsActive).setSavePerEvent();

    m_dataStore.newVariable(kVarCalls)
     .setSavePerEvent("Number Of Calls Per Event;Number of Calls;Events");

    m_dataStore.newVariable(kVarTime)
     .setSavePerCall("Time Per Call;Time [ms];Calls")
     .setSavePerEvent("Time Per Event;Time [ms];Events");

    m_dataStore.newVariable(kVarROBRets)
     .setSavePerCall()
     .setSavePerEvent("Number Of Retrieved ROBs Per Event;Retrieved ROBs;Events");

    m_dataStore.newVariable(kVarROBRetSize)
     .setSavePerCall("Size Of Retrieved ROBs Per Call;Retrieved ROBs Size [kB];Calls")
     .setSavePerEvent("Size Of Retrieved ROBs Per Event;Retrieved ROBs Size [kB];Events");

    m_dataStore.newVariable(kVarROBReqs)
     .setSavePerCall()
     .setSavePerEvent("Number Of Cached ROBs Per Event;Cached ROBs;Events");

    m_dataStore.newVariable(kVarROBReqSize)
     .setSavePerCall("Size Of Cached ROBs Per Call;Cached ROBs Size [kB];Calls")
     .setSavePerEvent("Size Of Cached ROBs Per Event;Cached ROBs Size [kB];Events");

    m_dataStore.newVariable(kVarROBDisabled).setSavePerEvent();
    m_dataStore.newVariable(kVarROBIgnored).setSavePerEvent();
    m_dataStore.newVariable(kVarROBNotOK).setSavePerEvent();
    m_dataStore.newVariable(kVarROBUnclassified).setSavePerEvent();
    m_dataStore.newVariable(kVarROBPrefetched).setSavePerEvent();
  }

  /**
   * Counter destructor.
   */
  CounterROB::~CounterROB() {
  }

  /**
   * Reset per-event counter(s). Not used currently for ROB counter.
   */
  void CounterROB::startEvent() {
  }

  /**
   * Perform monitoring of a Read Out Buffer call within an event.
   * @param e Read Out Buffer index in D3PD.
   * @param f ROBIN index within ROB
   * @param weight Event weight.
   */
  void CounterROB::processEventCounter(UInt_t e, UInt_t /*f*/, Float_t weight) {
    ++m_calls;

    weight *= getPrescaleFactor(e);
    if (isZero(weight) == kTRUE) return;


    Bool_t isBad = kFALSE;

    IntSet_t* ROBsForCounter(nullptr);

    // Collect data to fill. This depends on if we are looking at a ROBIN or a ROS

    MonitorROSCommon* parent = (MonitorROSCommon*) getParent(); // Up-cast

    if (getStrDecoration(kDecType) == Config::config().getStr(kROSString)) {
      // I'm monitoring a ROS. I need to collate all my ROBINs in this request
      ROBsForCounter = &(parent->getROSMapping(e).find(getStrDecoration(kDecMyROS))->second); // Get my mapping
    } else if (getStrDecoration(kDecType) == Config::config().getStr(kROBINString)) {
      //Monitoring a ROBIN, need to collate all ROBS
      ROBsForCounter = &(parent->getROBINMapping(e).find(getStrDecoration(kDecMyROS))->second); //Get ROBIN mapping
    } else {
      // Else we're just monitoring a ROBIN hence it's only me!
      //ROBsForCounter.insert( f );
      Error("CounterROB::processEventCounter", "Does not match ROS or ROBIN");
    }


    // Put nullptr check here so covertiry sees it
    if (ROBsForCounter == nullptr) return;

    //Int_t _nROBSForCounter = ROBsForCounter->size();

    // Loop over the ROBINs in this ROS request
    for (IntSetIt_t it = ROBsForCounter->begin(); it != ROBsForCounter->end(); ++it) {
      UInt_t ROBIndex = (*it);

      bool isRetrieved = m_costData->getIsROBDataRetrieved(e, ROBIndex);
      bool isPrefetch = m_costData->getIsROBDataStatusPrefetched(e, ROBIndex);
      bool isCached = m_costData->getIsROBDataCached(e, ROBIndex);
      isRetrieved = isRetrieved || isPrefetch;

      m_dataStore.store(kVarROBRets, m_costData->getIsROBDataRetrieved(e, ROBIndex), weight);
      if (isRetrieved) m_dataStore.store(kVarROBRetSize, m_costData->getROBDataSize(e, ROBIndex), weight);

      m_dataStore.store(kVarROBReqs, m_costData->getIsROBDataCached(e, ROBIndex), weight);

      if (isCached) m_dataStore.store(kVarROBReqSize, m_costData->getROBDataSize(e, ROBIndex), weight);

      if (!isPrefetch) {
        m_dataStore.store(kVarROBUnclassified, m_costData->getIsROBDataUnclassified(e, ROBIndex), weight);
      }

      m_dataStore.store(kVarROBPrefetched, m_costData->getIsROBDataStatusPrefetched(e, ROBIndex), weight);
      m_dataStore.store(kVarROBIgnored, m_costData->getIsROBDataIgnored(e, ROBIndex), weight);
      m_dataStore.store(kVarROBDisabled, m_costData->getIsROBDataDisabled(e, ROBIndex), weight);

      if (m_costData->getIsROBDataStatusOK(e, ROBIndex) == kFALSE) isBad = kTRUE;
      m_dataStore.store(kVarROBNotOK, (int) isBad, weight);

      if (isBad == kTRUE && Config::config().getDisplayMsg(kMsgBadROB) == kTRUE) {
        Warning("CounterROB::processEventCounter", "ROB data from %s (ID 0x%x) is flagged as NOT OK.",
                getName().c_str(), m_costData->getROBDataID(e, ROBIndex));
      }

      // We divide this time evenly to all ROBINs which were part of this request
      Float_t rosReqTime = m_costData->getROBTimer(e); // Total time
      rosReqTime /= (Float_t) m_costData->getROBDataN(e); // Time per ROBIN
      m_dataStore.store(kVarTime, rosReqTime, weight);
    }

    m_dataStore.store(kVarCalls, 1, weight);
  }

  /**
   * Perform end-of-event monitoring on the DataStore.
   */
  void CounterROB::endEvent(Float_t weight) {
    m_dataStore.store(kVarEventsActive, 1., weight);
    m_dataStore.endEvent();
  }

  /**
   * When running with prescales applied. This function returns how the counter should be scaled for the current call.
   * We associate ROS with Alg, then get the alg's chain, then get the weighting factor for the chain
   * @return Multiplicative weighting factor
   */
  Double_t CounterROB::getPrescaleFactor(UInt_t e) {
    Int_t seqID = m_costData->getROBAlgLocation(e).first;

    if (seqID < 0) return 1.; // Could not find associated alg - no additonal scaling

    return TrigXMLService::trigXMLService().getHLTCostWeightingFactor(
      TrigConfInterface::getHLTNameFromChainID(m_costData->getSequenceChannelCounter(seqID),
                                               m_costData->getSequenceLevel(seqID)));
  }

  /**
   * Output debug information on this call to the console
   */
  void CounterROB::debug(UInt_t e, UInt_t f) {
    if (f == 0) { //For first ROB in ROSReq
      Info("CounterROB::debug", "ROS %s: ReqID:%i Timer:%.2f NData:%i NSum:%i",
           getName().c_str(),
           m_costData->getROBReqID(e),
           m_costData->getROBTimer(e),
           m_costData->getROBDataN(e),
           m_costData->getROBSumN(e));
    }
    Info("CounterROB::debug",
         "\t%i] ROB %s: Size:%.2f Cache:%i Disabled:%i Ignored:%i Retr.:%i Prefetched:%i Unclassified:%i OK:%i",
         f,
         getName().c_str(),
         m_costData->getROBDataSize(e, f),
         m_costData->getIsROBDataCached(e, f),
         m_costData->getIsROBDataDisabled(e, f),
         m_costData->getIsROBDataIgnored(e, f),
         m_costData->getIsROBDataRetrieved(e, f),
         m_costData->getIsROBDataStatusPrefetched(e, f),
         m_costData->getIsROBDataUnclassified(e, f),
         m_costData->getIsROBDataStatusOK(e, f));
  }
} // namespace TrigCostRootAnalysis
