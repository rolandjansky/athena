/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGNTEBWEIGHT_H
#define TRIG_TRIGNTEBWEIGHT_H

//
// Calculate the EB unweighting
//

// Framework
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Trigger
#include "TrigSteering/HLTResultAccessTool.h"

// Local
#include "TrigCostMonitor/ITrigNtTool.h"

#include <map>

namespace Trig
{

  /**
   * @enum Trig::EBTriggerType
   * Enumerates the different enhanced bias trigger types. Make sure to also change the static names below too.
   */
  enum EBTriggerType {
    kEb_random_physics,
    kEb_low_physics,
    kEb_high_physics,
    kEb_low_physics_noL1PS,
    kEb_high_physics_noL1PS,

    kEb_random_empty,
    kEb_empty_noL1PS,
    kEb_firstempty_noL1PS,
    kEb_unpaired_iso_noL1PS,
    kEb_unpaired_noniso_noL1PS,
    kEb_abortgapnotcalib_noL1PS,

    kEbTrggerType_SIZE // This must always be the final entry.
  };

  enum EBBunchGroupType {
    kBG_NONE = 0,
    kBG_FILLED = 1,
    kBG_CALREQ = 2, // Not used
    kBG_EMPTY = 3,
    kBG_UNPAIRED_ISO = 4,
    kBG_UNPAIRED_NONISO = 5,
    kBG_FIRSTEMPTY = 6,
    kBG_ABORTGAPNOTCALIB = 7
  };

  static const std::string EBBunchGroupNames[] = {
    "NONE", "FILLED", "CALREQ", "EMPTY", "UNPAIRED_ISO", "UNPAIRED_NONISO", "FIRSTEMPTY", "ABORTGAPNOTCALIB"
  };

  ////////////////

  static const EBBunchGroupType EBBunchGroupHLT[] = {
    kBG_FILLED,
    kBG_FILLED,
    kBG_FILLED,
    kBG_FILLED,
    kBG_FILLED,

    kBG_EMPTY,
    kBG_EMPTY,
    kBG_FIRSTEMPTY,
    kBG_UNPAIRED_ISO,
    kBG_UNPAIRED_NONISO,
    kBG_ABORTGAPNOTCALIB
  }; 

  static const std::string EBMappingNameHLT[] = {
    "HLT_noalg_eb_L1RD3_FILLED",
    "L1ItemNames HLT_eb_low_L1RD2_FILLED",
    "L1ItemNames HLT_eb_high_L1RD2_FILLED",
    "HLT_noalg_eb_L1PhysicsLow_noPS",
    "HLT_noalg_eb_L1PhysicsHigh_noPS",

    "HLT_noalg_eb_L1RD3_EMPTY",
    "HLT_noalg_eb_L1EMPTY_noPS",
    "HLT_noalg_eb_L1FIRSTEMPTY_noPS",
    "HLT_noalg_eb_L1UNPAIRED_ISO_noPS",
    "HLT_noalg_eb_L1UNPAIRED_NONISO_noPS",
    "HLT_noalg_eb_L1ABORTGAPNOTCALIB_noPS"
  }; //!< Static string array of keys mapped to L1Items, this comes direct from the HLT configuration (L1ItemNames) or via XML.

  static const std::string EBTriggerNameHLT[] = {
    "HLT_noalg_eb_L1RD3_FILLED",
    "HLT_eb_low_L1RD2_FILLED",
    "HLT_eb_high_L1RD2_FILLED",
    "HLT_noalg_eb_L1PhysicsLow_noPS",
    "HLT_noalg_eb_L1PhysicsHigh_noPS",

    "HLT_noalg_eb_L1RD3_EMPTY",
    "HLT_noalg_eb_L1EMPTY_noPS",
    "HLT_noalg_eb_L1FIRSTEMPTY_noPS",
    "HLT_noalg_eb_L1UNPAIRED_ISO_noPS",
    "HLT_noalg_eb_L1UNPAIRED_NONISO_noPS",
    "HLT_noalg_eb_L1ABORTGAPNOTCALIB_noPS"
  };  //!< Static string array of text versions of EB trigger names for HLT level

  static const InterfaceID IID_TrigNtEBWeightTool("Trig::TrigNtEBWeightTool", 1, 0);

  class TrigNtEBWeightTool : virtual public Trig::ITrigNtTool, public AthAlgTool {
  public:

    static const InterfaceID& interfaceID() { return IID_TrigNtEBWeightTool; }

    TrigNtEBWeightTool(const std::string &, const std::string &, const IInterface *);
    virtual ~TrigNtEBWeightTool() {}

    StatusCode initialize();
    StatusCode finalize();

    bool Fill(TrigMonConfig *confg);
    bool Fill(TrigMonEvent &event);


  private:

    void readEBConfigFromXML(unsigned runNumber);

    std::multimap<std::string, std::string> m_filterInfo; //!< Holds the Enhanced Bias mapping L1 -> HLT mapping information. From orig. online menu or XML.

    std::map<EBTriggerType, float>       m_chainPrescale; //!< Holds the total multiplicative prescale of a chain, including any additional RD0/RD1 factors.
    std::map<EBTriggerType, std::string> m_chainL1Seeds; //!< Holds the L1 item(s) which seed each EB chain.
    std::map<EBTriggerType, bool>        m_chainEnabled; //!< True if all prescales in the chain are > 0
    std::map<std::string, unsigned>      m_L1NameToCPTID; //!< Holds map of L1 item to CPTID for the loaded menu.
    std::map<unsigned, std::string>      m_CPTIDToL1Name; //!< Reverse map, for debug output.
    std::map<unsigned, EBBunchGroupType> m_L1IDToBunchGroup; //!< Holds map of L1 item CTPID to the bunchgroup

    // Tools and services:
    bool       m_isConfigured;
  };
}

#endif
