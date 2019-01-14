// -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         TrigDecisionMakerMT.h
 **
 **   Description:  - Re-entrant algorithm-derived class to run after the Steering
 **                 - Finds the list of defined chains at initialization
 **                 - For each event:
 **                   * Creates a xAODTriggerDecision object
 **                   * Fills it with the outcome of each defined chain
 **                   * Output to StoreGate via writehandle written to ESD/AOD
 **
 * @author Tim Martin      <Tim.Martin@cern.ch>      - University of Warwick
 **
 **   Created:      16 July 2018
 **
 **************************************************************************/
#ifndef TrigDecisionMaker_TrigDecisionMakerMT_H
#define TrigDecisionMaker_TrigDecisionMakerMT_H

// Base class
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "xAODEventInfo/EventInfo.h"

// trigger/configuration stuff
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigDecision.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigT1Result/RoIBResult.h"

// containers
#include <vector>
#include <string>
#include <map>

namespace HLT {
  class ILvl1ResultAccessTool;
}

namespace LVL1CTP {
  class Lvl1Result;
}

namespace TrigConf {
  class ITrigConfigSvc;
}

namespace TrigDec {

  /**
   * @class TrigDecisionMakerMT
   *  - Re-entrant algorithm-derived class to run after the MT Trigger
   *  - Finds the list of defined chains at initialization
   *  - For each event:
   *    * Creates a xAODTriggerDecision object
   *    * Fills it with the outcome of each defined chain & L1 data
   *    * Output to StoreGate via writehandle written to ESD/AOD
   *    * Based on TrigDecisionMaker.h
   *
   * @author Tim Martin      <Tim.Martin@cern.ch>      - University of Warwick
   */
  class TrigDecisionMakerMT : public ::AthReentrantAlgorithm
  {

  public:

    TrigDecisionMakerMT(const std::string &name, ISvcLocator *pSvcLocator); //!< std Gaudi Algorithm constructor
    ~TrigDecisionMakerMT();                                                 //!< std  deconstructor

    // IAlgorithm virtual methods to implement
    virtual StatusCode initialize() override; //!< std Gaudi initialize method -> read-in trigger configuration
    virtual StatusCode execute( const EventContext& context ) const override; //!< Re-entrant execute to create the xAOD::TrigDecision
    virtual StatusCode finalize() override;    //!< std Gaudi finalize method -> print out statistics

    StatusCode getL1Result (const LVL1CTP::Lvl1Result*& result, const EventContext& context) const; //!< retrieve LVL1 result (called in execute)
    StatusCode getHLTResult(const TrigCompositeUtils::DecisionContainer*& result, const EventContext& context) const; //!< retrieve HLT results (called in execute)

    char getBGByte(int BCId) const; //!< to get the BG byte encoded for a given BC

  private:

    /**
     * @brief Ensures that the supplied vectors have sufficient capacity to store the given bit, where bits are packed into uint32_t.
     * @param bit The bit we wish the vectors to be large enough to store
     * @param vectors Set of pointers to all vectors which need resizing. Note, while the set of pointers is const, the vectors are not const.
     **/  
    void resizeVectors(const size_t bit, const std::set< std::vector<uint32_t>* >& vectors) const;

    /**
     * @param bit The bit to set to 1 (bit 0 equates to the first bit). Requires the vector to have already been resized to be large enough.
     * @param bits The vector to set the bit in.
     **/
    void setBit(const size_t bit, std::vector<uint32_t>& bits) const;

    /**
     * @param chainID The identifier (name hash) of the chain to fetch the ChainCounter for.
     * @return the Chain Counter or -1 if error.
     **/
    int32_t getChainCounter(const TrigCompositeUtils::DecisionID chainID) const;

    /**
     * @param passedIDs Set of IDs of passed chains.
     * @param bitsVector Vector to set passed-bits in based off of passedIDs
     * @param allOutputVectors Set of pointers to *all* output vectors, keeps them all the same size
     * @return the number of positive bits set in the vector, should be the same as passedIDs.size()
     **/
    size_t makeBitMap(const TrigCompositeUtils::DecisionIDContainer& passedIDs,
      std::vector<uint32_t>& bitsVector, std::set< std::vector<uint32_t>* >& allOutputVectors) const;

    Gaudi::Property<bool> m_doL1{this, "doL1",  true, "Read L1 trigger information"};
    Gaudi::Property<bool> m_doHLT{this, "doHLT", true, "Read HLT trigger information"};

    // Tools & services
    ServiceHandle<TrigConf::ITrigConfigSvc> m_trigConfigSvc; //!< handle to the full (L1 & HLT) trigger config service
    Gaudi::Property<std::string> m_trigConfigLocation{this, "TrigConfigLocation", "TrigConf::TrigConfigSvc/TrigConfigSvc", "Trigger configuration service to fetch"};

    ToolHandle<HLT::ILvl1ResultAccessTool> m_lvl1Tool;  //!< tool to ease the access to the L1 results (RoIs, items, etc)
    Gaudi::Property<std::string> m_lvl1ToolLocation{this, "Lvl1ToolLocation", "HLT::Lvl1ResultAccessTool/Lvl1ResultAccessTool", "L1 tool to fetch"};

    // Input keys configuration
    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_HLTSummaryKeyIn {this, "HLTSummary", "HLTSummary", "HLT summary container Key"};
    SG::ReadHandleKey<LVL1CTP::Lvl1Result> m_L1ResultKeyIn {this, "Lvl1Result", "Lvl1Result", "Lvl1 Result Object Key"};
    SG::ReadHandleKey<ROIB::RoIBResult> m_ROIBResultKeyIn {this, "RoIBResult", "RoIBResult", "RoIB Result Object Key"};
    SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKeyIn {this, "EventInfo", "EventInfo", "Event Info Object Key"};

    // Output configuration
    SG::WriteHandleKey<xAOD::TrigDecision> m_trigDecisionKeyOut {this, "TrigDecisionKey", "xTrigDecision", "Output trigger decision object key"};

    // For statistics
    mutable std::atomic<uint32_t> m_nEvents{0}, m_l1Passed{0}, m_hltPassed{0};
  };
}

#endif
