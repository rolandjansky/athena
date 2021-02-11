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
#include "xAODTrigger/TrigDecision.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigSteeringEvent/HLTResultMT.h"
#include "TrigOutputHandling/ITriggerBitsMakerTool.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfInterfaces/IHLTConfigSvc.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigSteering/Lvl1ResultAccessTool.h"

// containers
#include <vector>
#include <string>
#include <map>

namespace LVL1CTP {
  class Lvl1Result;
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

  private:

    StatusCode getL1Result (const LVL1CTP::Lvl1Result*& result, const EventContext& context) const; //!< retrieve LVL1 result (called in execute)

    char getBGByte(unsigned int BCId) const; //!< to get the BG byte encoded for a given BC

    Gaudi::Property<bool> m_doL1{this, "doL1",  true, "Read L1 trigger information"};
    Gaudi::Property<bool> m_doHLT{this, "doHLT", true, "Read HLT trigger information"};

    Gaudi::Property<bool> m_useNewConfigL1{this, "UseNewConfigL1", true, "When true, read the menu from detector store, when false use the L1ConfigSvc"};
    Gaudi::Property<bool> m_useNewConfigHLT{this, "UseNewConfigHLT", true, "When true, read the menu from detector store, when false use the HLTConfigSvc"};

    // Tools & services
    ServiceHandle<TrigConf::ILVL1ConfigSvc> m_l1ConfigSvc{this, "LVL1ConfigSvc", "LVL1ConfigSvc", "The LVL1ConfigSvc providing L1 configuration for Run 2"};
    ServiceHandle<TrigConf::IHLTConfigSvc> m_hltConfigSvc{this, "HLTConfigSvc", "HLTConfigSvc", "The HLTConfigSvc providing HLT configuration for Run 2"};

    ToolHandle<HLT::ILvl1ResultAccessTool> m_lvl1Tool{this, "Lvl1ResultAccessTool", "HLT::Lvl1ResultAccessTool/Lvl1ResultAccessTool", "L1 tool to fetch"}; //!< tool to ease the access to the L1 results (RoIs, items, etc)

    ToolHandle<ITriggerBitsMakerTool> m_bitsMakerTool{this, "BitsMakerTool", "", "Tool to create trigger bits for MC"};

    // Input keys configuration
    SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu key, for use if IsJSONConfig=True"};

    SG::ReadHandleKey<HLT::HLTResultMT> m_hltResultKeyIn {this, "HLTResultMT", "HLTResultMT", "Key of the HLTResultMT object to get bits from online bytestream" };
    SG::ReadHandleKey<ROIB::RoIBResult> m_ROIBResultKeyIn {this, "RoIBResult", "RoIBResult", "RoIB Result Object Key"};
    SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoKeyIn {this, "EventInfo", "EventInfo", "Event Info Object Key"};

    // Output configuration
    SG::WriteHandleKey<xAOD::TrigDecision> m_trigDecisionKeyOut {this, "TrigDecisionKey", "xTrigDecision", "Output trigger decision object key"};

    // For statistics
    mutable std::atomic<uint32_t> m_nEvents{0}, m_l1Passed{0}, m_hltPassed{0};
  };
}

#endif
