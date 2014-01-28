/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMRoI_v1.cxx 579924 2014-01-24 10:26:51Z krasznaa $

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
    if (wordId == s_jetEtWordIdVal) this->setJetEtRoiWord( roiWord );
      else {
        wordId = (roiWord >> s_wordIdBit) & s_wordIdMask;
        if      (wordId == s_wordIdVal0) this->setEnergyRoiWord0( roiWord );
        else if (wordId == s_wordIdVal1) this->setEnergyRoiWord1( roiWord );
        else if (wordId == s_wordIdVal2) this->setEnergyRoiWord2( roiWord );
        else valid = false;
      }
    return valid;    
  }
  
  /// Return Jet-ET hits
  int CMMRoI_v1::jetEtHits()         const
  {
    return (this->jetEtRoiWord() >> s_jetEtHitsBit) & s_jetEtHitsMask;  
  }
  
  /// Return Sum-ET hits
  int CMMRoI_v1::sumEtHits()         const
  {
    return (this->energyRoiWord1() >> s_sumEtHitsBit) & s_sumEtHitsMask;  
  }
  
  /// Return Missing-ET hits
  int CMMRoI_v1::missingEtHits()     const
  {
    return (this->energyRoiWord2() >> s_missingEtHitsBit) & s_missingEtHitsMask; 
  }
  
  /// Return Missing-ET-Sig hits
  int CMMRoI_v1::missingEtSigHits()  const
  {
    return (this->energyRoiWord0() >> s_missingEtSigHitsBit) & s_missingEtSigHitsMask; 
  }
  
  /// Return Ex
  int CMMRoI_v1::ex()                const
  {
    return (this->energyRoiWord0() >> s_energyBit) & s_energyMask; 
  }
  
  /// Return Ey
  int CMMRoI_v1::ey()                const
  {
    return (this->energyRoiWord1() >> s_energyBit) & s_energyMask; 
  }
  
  /// Return Et
  int CMMRoI_v1::et()                const
  {
    return (this->energyRoiWord2() >> s_energyBit) & s_energyMask;  
  }
  
  /// Return Jet-ET error flag (bit 1 Parity)
  int CMMRoI_v1::jetEtError()        const
  {
    return ((this->jetEtRoiWord() >> s_jetEtParityBit) & 0x1) << 1;  
  }
  
  /// Return Sum-ET error flag (bit 1 Parity)
  int CMMRoI_v1::sumEtError()        const
  {
    return parity(this->energyRoiWord1()) << 1;  
  }
  
  /// Return Missing-ET error flag (bit 1 Parity)
  int CMMRoI_v1::missingEtError()    const
  {
    return parity(this->energyRoiWord2()) << 1;
  }
  
  /// Return Missing-ET-Sig error flag (bit 1 Parity)
  int CMMRoI_v1::missingEtSigError() const
  {
    return parity(this->energyRoiWord0()) << 1;  
  }
  
  /// Return Ex error flags (bit 0 Overflow, bit 1 Parity)
  int CMMRoI_v1::exError()           const
  {
    return (parity(this->energyRoiWord0()) << 1) | overflow(this->energyRoiWord0());  
  }
  
  /// Return Ey error flags (bit 0 Overflow, bit 1 Parity)
  int CMMRoI_v1::eyError()           const
  {
    return (parity(this->energyRoiWord1()) << 1) | overflow(this->energyRoiWord1());  
  }
  
  /// Return Et error flags (bit 0 Overflow, bit 1 Parity)
  int CMMRoI_v1::etError()           const
  {
    return (parity(this->energyRoiWord2()) << 1) | overflow(this->energyRoiWord2()); 
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
