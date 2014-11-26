// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMRoI_v1.h 631127 2014-11-26 09:54:48Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_CMMROI_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMMROI_V1_H

extern "C" {
#   include <stdint.h>
}

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {
  
  /// Description of CMMRoI_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631127 $
  /// $Date: 2014-11-26 10:54:48 +0100 (Wed, 26 Nov 2014) $  
  
  class CMMRoI_v1 : public SG::AuxElement{
    public:
      // Default constructor
      CMMRoI_v1();
      
      /// get jetEtRoiWord
      uint32_t jetEtRoiWord() const;
      /// set jetEtRoiWord
      void setJetEtRoiWord(uint32_t);
      
      /// get energyRoiWord0
      uint32_t energyRoiWord0() const;
      /// set energyRoiWord0
      void setEnergyRoiWord0(uint32_t);
      
      /// get energyRoiWord1
      uint32_t energyRoiWord1() const;
      /// set energyRoiWord1
      void setEnergyRoiWord1(uint32_t);     
      
      /// get energyRoiWord2
      uint32_t energyRoiWord2() const;
      /// set energyRoiWord2
      void setEnergyRoiWord2(uint32_t); 
      
      /// Set RoI word with ID check
      /// Internally works out the type of roiWord
      bool setRoiWord(uint32_t roiWord);
      
      /// Return Jet-ET hits
      int jetEtHits()         const;
      /// Return Sum-ET hits
      int sumEtHits()         const;
      /// Return Missing-ET hits
      int missingEtHits()     const;
      /// Return Missing-ET-Sig hits
      int missingEtSigHits()  const;
      /// Return Ex
      int ex()                const;
      /// Return Ey
      int ey()                const;
      /// Return Et
      int et()                const;
      /// Return Jet-ET error flag (bit 1 Parity)
      int jetEtError()        const;
      /// Return Sum-ET error flag (bit 1 Parity)
      int sumEtError()        const;
      /// Return Missing-ET error flag (bit 1 Parity)
      int missingEtError()    const;
      /// Return Missing-ET-Sig error flag (bit 1 Parity)
      int missingEtSigError() const;
      /// Return Ex error flags (bit 0 Overflow, bit 1 Parity)
      int exError()           const;
      /// Return Ey error flags (bit 0 Overflow, bit 1 Parity)
      int eyError()           const;
      /// Return Et error flags (bit 0 Overflow, bit 1 Parity)
      int etError()           const;        
      /// Return Energy parity error (0/1)
      int parity(uint32_t roiWord) const;
      /// Return Energy overflow flag (0/1)
      int overflow(uint32_t roiWord) const;    
    
    private:   
      //  RoI word IDs
      static const int s_jetEtWordIdVal       = 0x5;
      static const int s_wordIdVal0           = 0x4;
      static const int s_wordIdVal1           = 0x6;
      static const int s_wordIdVal2           = 0x5;
      //  Data locations
      static const int s_jetEtWordIdBit       = 29;
      static const int s_wordIdBit            = 28;
      static const int s_jetEtHitsBit         = 0;
      static const int s_sumEtHitsBit         = 16;
      static const int s_missingEtHitsBit     = 16;
      static const int s_missingEtSigHitsBit  = 16;
      static const int s_energyBit            = 0;
      static const int s_jetEtParityBit       = 28;
      static const int s_parityBit            = 27;
      static const int s_overflowBit          = 15;
      //  Data masks
      static const int s_jetEtWordIdMask      = 0x7;
      static const int s_wordIdMask           = 0xf;
      static const int s_jetEtHitsMask        = 0xf;
      static const int s_sumEtHitsMask        = 0xff;
      static const int s_missingEtHitsMask    = 0xff;
      static const int s_missingEtSigHitsMask = 0xff;
      static const int s_energyMask           = 0x7fff;     
        
  };
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_CMMROI_V1_H
