//create
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGGER_VERSIONS_JFEXTAUROI_V1_H
#define XAODTRIGGER_VERSIONS_JFEXTAUROI_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>
#include <string>

// xAOD include(s):
#include "AthContainers/AuxElement.h"
namespace xAOD {

   /// Class describing properties of a LVL1 jFEX global Trigger Object (TOB)
   /// in the xAOD format.

   class jFexTauRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      jFexTauRoI_v1();

      /// In future initialze the xTOB as well, word1
      void initialize(uint8_t jFexNumber,uint8_t fpgaNumber, uint32_t tobWord, float_t eta, float_t phi );

      /// The "raw" 32-bit word describing the object candidate
      uint32_t  tobWord()       const;
      uint8_t   jFexNumber()    const;
      uint8_t   fpgaNumber()    const;
      uint16_t  tobEt()         const;
      uint16_t  tobIso()        const;
      uint8_t   tobLocalEta()   const;
      uint8_t   tobLocalPhi()   const; 
      uint8_t   tobSat()        const;
      int       globalEta()     const;
      uint      globalPhi()     const;
      float     eta()           const;
      float     phi()           const;

      /// Set the "raw" 32-bit words describing the object candidate
      void setTobWord( uint32_t tobWord );
      void setjFexNumber( uint8_t jFexNumber);
      void setfpgaNumber(uint8_t fpgaNumber);  
          
      /// TOB ET (decoded from TOB, stored for convenience)
      unsigned int unpackEtTOB() const;
      void setTobEt( uint16_t value); 
      unsigned int et() const;
       
      /// TOB Isolation (decoded from TOB, stored for convenience)
      unsigned int unpackIsoTOB() const;
      void setTobIso( uint16_t value); 
      unsigned int iso() const; 

      ///Eta coordinates
      unsigned int unpackEtaIndex() const;
      void setTobLocalEta(uint8_t value);


      ///Phi coordinates
      unsigned int unpackPhiIndex() const;
      void setTobLocalPhi(uint8_t value);

  
      //Saturation flag
      unsigned int unpackSaturationIndex() const;
      void setTobSat(uint8_t value);
      

      //Global coords
      int unpackGlobalEta() const;
      void setGlobalEta(int value);
      void setEta(float value);

      uint unpackGlobalPhi() const;
      void setGlobalPhi(uint value);
      void setPhi(float value);
      
      
   private:
      //Constants used in converting to ATLAS units
      static const float s_tobEtScale;
      static const float s_tobIsoScale;
      static const float s_towerEtaWidth;
      static const float s_towerPhiWidth;
      static const float s_minEta;

      // Data locations within word
      static const int s_etaBit = 27; 
      static const int s_phiBit = 23; 
      static const int s_etBit  = 12; 
      static const int s_isoBit = 1;
      static const int s_satBit = 0;

      //Data masks
      static const int s_etaMask  = 0x1f;
      static const int s_phiMask  = 0xf;
      static const int s_etMask   = 0x7ff;
      static const int s_isoMask  = 0x7ff; 
      static const int s_satMask  = 0x1; 

   }; // class jFexTauRoI_v1

} // namespace xAOD
// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::jFexTauRoI_v1, SG::AuxElement );

#endif //XAODTRIGGER_VERSIONS_JFEXTAUROI_V1_H
