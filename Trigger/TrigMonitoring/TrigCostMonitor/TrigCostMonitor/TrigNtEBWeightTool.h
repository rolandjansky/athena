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
    kEb_low_physics,
    kEb_high_physics,
    kEb_random,
    kEb_physics_noL1PS,

    kEb_physics_empty,
    kEb_random_empty,

    kEb_physics_firstempty,
    kEb_random_firstempty,

    kEb_physics_unpaired_iso, 
    kEb_random_unpaired_iso,

    kEb_physics_unpaired_noniso,
    kEb_random_unpaired_noniso,

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
    kBG_UNPAIRED = 7 // Not used
  };

  static const std::string EBBunchGroupNames[] = {
    "NONE", "FILLED", "CALREQ", "EMPTY", "UNPAIRED_ISO", "UNPAIRED_NONISO", "FIRSTEMPTY", "UNPAIRED"
  };

  ////////////////

  static const std::string EBMappingNameEF[] = {
    "L1ItemNames EF_eb_physics",
    "L1ItemNames EF_high_eb_physics",
    "", // Random chains apply a uniform weighting factor
    "EF_eb_physics_noL1PS",

    "EF_eb_physics_empty",
    "", // Random chains apply a uniform weighting factor

    "EF_eb_physics_firstempty",
    "", // Random chains apply a uniform weighting factor

    "EF_eb_physics_unpaired_iso", 
    "", // Random chains apply a uniform weighting factor

    "EF_eb_physics_unpaired_noniso",
    "" // Random chains apply a uniform weighting factor
  }; //!< Static string array of keys mapped to L1Items, this comes direct from the HLT configuration (L1ItemNames) or via XML.
  
  static const std::string EBMappingNameHLT[] = {
    "L1ItemNames HLT_eb_low_L1RD0_FILLED",
    "L1ItemNames HLT_eb_high_L1RD0_FILLED",
    "", // Random chains apply a uniform weighting factor
    "HLT_noalg_eb_L1Physics_noPS",

    "L1ItemNames HLT_eb_empty_L1RD0_EMPTY",
    "", //"HLT_noalg_eb_L1RD0_EMPTY", // Random chains apply a uniform weighting factor

    "L1ItemNames HLT_eb_firstempty_L1RD0_FIRSTEMPTY",
    "",//"HLT_noalg_eb_L1RD0_FIRSTEMPTY", // Random chains apply a uniform weighting factor

    "L1ItemNames HLT_eb_unpairediso_L1RD0_UNPAIRED_ISO", 
    "",//"HLT_noalg_eb_L1RD0_UNPAIRED_ISO", // Random chains apply a uniform weighting factor

    "", // Here will go the unpaired noniso
    "" // Random chains apply a uniform weighting factor
  }; //!< Static string array of keys mapped to L1Items, this comes direct from the HLT configuration (L1ItemNames) or via XML.
  
  static const std::string EBTriggerNameL2[] = {
    "L2_eb_physics",
    "L2_high_eb_physics",
    "L2_eb_random",
    "L2_eb_physics_noL1PS",

    "L2_eb_physics_empty",
    "L2_eb_random_empty",

    "L2_eb_physics_firstempty",
    "L2_eb_random_firstempty",

    "L2_eb_physics_unpaired_iso", 
    "L2_eb_random_unpaired_iso",

    "L2_eb_physics_unpaired_noniso",
    ""
  };  //!< Static string array of text versions of EB trigger names at L2
  
  static const std::string EBTriggerNameEF[] = {
    "EF_eb_physics",
    "EF_high_eb_physics",
    "EF_eb_random",
    "EF_eb_physics_noL1PS",

    "EF_eb_physics_empty",
    "EF_eb_random_empty",

    "EF_eb_physics_firstempty",
    "EF_eb_random_firstempty",

    "EF_eb_physics_unpaired_iso", 
    "EF_eb_random_unpaired_iso",

    "EF_eb_physics_unpaired_noniso",
    ""
  };  //!< Static string array of text versions of EB trigger names at EF
  
  static const std::string EBTriggerNameHLT[] = {
    "HLT_eb_low_L1RD0_FILLED",
    "HLT_eb_high_L1RD0_FILLED",
    "HLT_noalg_eb_L1RD1_FILLED",
    "HLT_noalg_eb_L1Physics_noPS",

    "HLT_eb_empty_L1RD0_EMPTY",
    "HLT_noalg_eb_L1RD0_EMPTY",

    "HLT_eb_firstempty_L1RD0_FIRSTEMPTY",
    "HLT_noalg_eb_L1RD0_FIRSTEMPTY",

    "HLT_eb_unpairediso_L1RD0_UNPAIRED_ISO", 
    "HLT_noalg_eb_L1RD0_UNPAIRED_ISO",

    "", //unpaired non iso
    ""
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
    
    // Properties:
    float m_randomSeedWeight; //<! Extra weight given random L1 seeded triggers due to difference in clock (5 instead of 40 MHz)
    
    std::multimap<std::string, std::string> m_filterInfo; //!< Holds the Enhanced Bias mapping L1 -> HLT mapping information. From orig. online menu or XML.
    
    std::map<EBTriggerType, float>       m_chainPrescale; //!< Holds the total multiplicative prescale of a chain, including any additional RD0/RD1 factors.
    std::map<EBTriggerType, std::string> m_chainL1Seeds; //!< Holds the L1 item(s) which seed each EB chain.
    std::map<EBTriggerType, bool>        m_chainEnabled; //!< True if all prescales in the chain are > 0
    std::map<std::string, unsigned>      m_L1NameToCPTID; //!< Holds map of L1 item to CPTID for the loaded menu.
    std::map<unsigned, std::string>      m_CPTIDToL1Name; //!< Reverse map, for debug output.
    std::map<unsigned, EBBunchGroupType> m_L1IDToBunchGroup; //!< Holds map of L1 item CTPID to the bunchgroup

    // Tools and services:
    unsigned   m_isRun1;
    bool       m_isConfigured;
  };
}

#endif
