/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMRoI_v1.cxx 638578 2015-01-09 17:00:15Z morrisj $

// System include(s):
#include <cmath>

// EDM includes(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/CMMRoI_v1.h"

namespace xAOD{ 
  
  CMMRoI_v1::CMMRoI_v1() :
    SG::AuxElement()
  {        
  }  
  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMRoI_v1 , uint32_t , jetEtRoiWord , setJetEtRoiWord )  
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMRoI_v1 , uint32_t , energyRoiWord0 , setEnergyRoiWord0 ) 
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMRoI_v1 , uint32_t , energyRoiWord1 , setEnergyRoiWord1 ) 
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( CMMRoI_v1 , uint32_t , energyRoiWord2 , setEnergyRoiWord2 ) 
   
  /// Set RoI word with ID check
  bool CMMRoI_v1::setRoiWord(uint32_t roiWord)
  {
    bool valid(true);
    int wordId = (roiWord >> s_jetEtWordIdBit) & s_jetEtWordIdMask;
    if (wordId == s_jetEtWordIdVal) setJetEtRoiWord( roiWord );
      else {
        wordId = (roiWord >> s_wordIdBit) & s_wordIdMask;
        if      (wordId == s_wordIdVal0) setEnergyRoiWord0( roiWord );
        else if (wordId == s_wordIdVal1) setEnergyRoiWord1( roiWord );
        else if (wordId == s_wordIdVal2) setEnergyRoiWord2( roiWord );
        else valid = false;
      }
    return valid;    
  }
  
  /// Return Jet-ET hits
  int CMMRoI_v1::jetEtHits()         const
  {
    return (jetEtRoiWord() >> s_jetEtHitsBit) & s_jetEtHitsMask;  
  }
  
  /// Return Sum-ET hits
  int CMMRoI_v1::sumEtHits()         const
  {
    return (energyRoiWord1() >> s_sumEtHitsBit) & s_sumEtHitsMask;  
  }
  
  /// Return Missing-ET hits
  int CMMRoI_v1::missingEtHits()     const
  {
    return (energyRoiWord2() >> s_missingEtHitsBit) & s_missingEtHitsMask; 
  }
  
  /// Return Missing-ET-Sig hits
  int CMMRoI_v1::missingEtSigHits()  const
  {
    return (energyRoiWord0() >> s_missingEtSigHitsBit) & s_missingEtSigHitsMask; 
  }
  
  /// Return Ex
  int CMMRoI_v1::ex()                const
  {
    return (energyRoiWord0() >> s_energyBit) & s_energyMask; 
  }
  
  /// Return Ey
  int CMMRoI_v1::ey()                const
  {
    return (energyRoiWord1() >> s_energyBit) & s_energyMask; 
  }
  
  /// Return Et
  int CMMRoI_v1::et()                const
  {
    return (energyRoiWord2() >> s_energyBit) & s_energyMask;  
  }
  
  /// Return Jet-ET error flag (bit 1 Parity)
  int CMMRoI_v1::jetEtError()        const
  {
    return ((jetEtRoiWord() >> s_jetEtParityBit) & 0x1) << 1;  
  }
  
  /// Return Sum-ET error flag (bit 1 Parity)
  int CMMRoI_v1::sumEtError()        const
  {
    return parity(energyRoiWord1()) << 1;  
  }
  
  /// Return Missing-ET error flag (bit 1 Parity)
  int CMMRoI_v1::missingEtError()    const
  {
    return parity(energyRoiWord2()) << 1;
  }
  
  /// Return Missing-ET-Sig error flag (bit 1 Parity)
  int CMMRoI_v1::missingEtSigError() const
  {
    return parity(energyRoiWord0()) << 1;  
  }
  
  /// Return Ex error flags (bit 0 Overflow, bit 1 Parity)
  int CMMRoI_v1::exError()           const
  {
    return (parity(energyRoiWord0()) << 1) | overflow(energyRoiWord0());  
  }
  
  /// Return Ey error flags (bit 0 Overflow, bit 1 Parity)
  int CMMRoI_v1::eyError()           const
  {
    return (parity(energyRoiWord1()) << 1) | overflow(energyRoiWord1());  
  }
  
  /// Return Et error flags (bit 0 Overflow, bit 1 Parity)
  int CMMRoI_v1::etError()           const
  {
    return (parity(energyRoiWord2()) << 1) | overflow(energyRoiWord2()); 
  }
  
  /// Return Energy parity error (0/1)
  int CMMRoI_v1::parity(uint32_t roiWord) const
  {
    return (roiWord >> s_parityBit) & 0x1;  
  }
  
  /// Return Energy overflow flag (0/1)
  int CMMRoI_v1::overflow(uint32_t roiWord) const
  {
    return (roiWord >> s_overflowBit) & 0x1; 
  }
  
} // namespace xAOD
