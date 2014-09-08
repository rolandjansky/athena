// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//  author: Tim Martin <Tim.Martin@cern.ch>
// -------------------------------------------------------------
#ifndef TrigCostRootAnalysis_EnhancedBiasWeighting_H
#define TrigCostRootAnalysis_EnhancedBiasWeighting_H

// STL include(s):
#include <string>
#include <map>
#include <limits.h>

// ROOT include(s):
#include <Rtypes.h>

namespace TrigCostRootAnalysis {

  //Forward declaration
  class TrigCostData;
  
  /**
   * @enum EBTrigger
   * Enumerates the different enhanced bias trigger types. Make sure to also change the static names below too.
   */
  enum EBTriggerType {
    kEb_physics,
    kHigh_eb_physics,
    kEb_random,
    kEb_physics_noL1PS,
    kEbTrggerType_SIZE // This must always be the final entry.
  };
  
  
  static const std::string EBMappingNameEF[] = {
    "L1ItemNames EF_eb_physics",
    "L1ItemNames EF_high_eb_physics",
    "", // Random chains apply a uniform weighting factor
    "EF_eb_physics_noL1PS"
  }; //!< Static string array of keys mapped to L1Items, this comes direct from the HLT configuration (L1ItemNames) or via XML.
  
  static const std::string EBMappingNameHLT[] = {
    "L1ItemNames HLT_eb_physics",
    "L1ItemNames HLT_high_eb_physics",
    "", // Random chains apply a uniform weighting factor
    "HLT_eb_physics_noL1PS"
  }; //!< Static string array of keys mapped to L1Items, this comes direct from the HLT configuration (L1ItemNames) or via XML.
  
  static const std::string EBTriggerNameL2[] = {
    "L2_eb_physics",
    "L2_high_eb_physics",
    "L2_eb_random",
    "L2_eb_physics_noL1PS"
  };  //!< Static string array of text versions of EB trigger names at L2
  
  static const std::string EBTriggerNameEF[] = {
    "EF_eb_physics",
    "EF_high_eb_physics",
    "EF_eb_random",
    "EF_eb_physics_noL1PS"
  };  //!< Static string array of text versions of EB trigger names at EF
  
  static const std::string EBTriggerNameHLT[] = {
    "HLT_eb_physics",
    "HLT_high_eb_physics",
    "HLT_eb_random",
    "HLT_eb_physics_noL1PS"
  };  //!< Static string array of text versions of EB trigger names for HLT level
  
  /**
   * @class EnhancedBiasWeighting
   *
   *
   */
  class EnhancedBiasWeighting {
  
   public:
   
    EnhancedBiasWeighting();
    EnhancedBiasWeighting(UInt_t _runNumber);
    ~EnhancedBiasWeighting();
    
    Bool_t setup(UInt_t _runNumber);
    Bool_t isConfigured() {
      return m_isConfigured;
    }
    
    Float_t getWeight(const TrigCostData &_costData_L1, const TrigCostData &_costData_HLT);
    
   private:
   
    Bool_t readMapFromXML();
    void   loadPrescales(Int_t _online_SMK, Int_t _online_L1PSK, Int_t _online_HLTPSK);
    
    Int_t m_runNumber; //!< The run to load EB data for
    Bool_t m_isConfigured; //!< Flag to hold if this object is initialised and ready to calculate weights.
    
    Int_t m_online_SMK; //!< Currently configured online supermaster key
    Int_t m_online_L1PSK; //<! Currently configured online level 1 prescale key
    Int_t m_online_HLTPSK; //<! Curretnly configured HLT prescale key
    
    Float_t m_randomSeedWeight; //<! Extra weight given random L1 seeded triggers due to difference in clock (5 instead of 40 MHz)
    
    std::multimap<std::string, std::string> m_filterInfo; //!< Holds the Enhanced Bias mapping L1 -> HLT mapping information. From orig. online menu or XML.
    
    std::map<EBTriggerType, Float_t> m_chainPrescale; //!< Holds the total multiplicative prescale of a chain, including any additional RD0/RD1 factors.
    std::map<EBTriggerType, std::string> m_chainL1Seeds; //!< Holds the L1 item(s) which seed each EB chain.
    //std::map<EBTriggerType, Bool_t> _chainEnabled; //!< Holds if the chain is active in the menu or disabled (PS <= 0)
    
    
  }; //class EnhancedBiasWeighting
  
} // namespace TrigCostRootAnalysis

#endif //TrigCostRootAnalysis_EnhancedBiasWeighting_H
