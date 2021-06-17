//create
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGGER_VERSIONS_JFEXSRJETROI_V1_H
#define XAODTRIGGER_VERSIONS_JFEXSRJETROI_V1_H

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

   class jFexSRJetRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      jFexSRJetRoI_v1();

      /// In future initialze the xTOB as well, word1
      void initialize(uint8_t jFexNumber,uint8_t fpgaNumber, uint32_t word0 );

      /// The "raw" 32-bit word describing the object candidate
      uint32_t word0() const;
      uint8_t jFexNumber() const;
      uint8_t fpgaNumber() const;
      uint16_t tobEt() const;
      uint8_t iEta() const;
      uint8_t iPhi() const; 
      uint8_t satFlag() const;
      int8_t globalEta() const;
      uint8_t globalPhi() const;

      /// Set the "raw" 32-bit words describing the object candidate
      void setWord0( uint32_t word0 );
      void setjFexNumber( uint8_t jFexNumber);
      void setfpgaNumber(uint8_t fpgaNumber); 

      /// TOB ET (decoded from TOB, stored for convenience)
      unsigned int unpackEtTOB() const;
      void setTobEt( uint16_t value); 
      unsigned int et() const; 

      ///Eta coordinates
      unsigned int unpackEtaIndex() const;
      void setEta(uint8_t value);
      unsigned int eta() const;

      ///Phi coordinates
      unsigned int unpackPhiIndex() const;
      void setPhi(uint8_t value);
      unsigned int phi() const;
  
      //Saturation flag
      unsigned int unpackSaturationIndex() const;
      void setSatFlag(uint8_t value);
  
      //Global coords
      int8_t getGlobalEta() const;
      void setGlobalEta(int8_t value);

      uint8_t getGlobalPhi() const;
      void setGlobalPhi(uint8_t value);

   private:
      //Constants used in converting to ATLAS units
      static const float s_tobEtScale;
      static const float s_towerEtaWidth;
      static const float s_towerPhiWidth;
      static const float s_minEta;


      // Data locations within word 
      static const int s_satBit =0;
      //static const int s_ResBit =1;
      static const int s_etBit  = 12;
      static const int s_phiBit = 23;
      static const int s_etaBit = 27;
                       
      //Data masks
      static const int s_etMask   = 0x7ff;
      static const int s_etaMask  = 0x1f;
      static const int s_phiMask  = 0xf;
      //static const int s_resMask  = 0x7ff; 
      static const int s_satMask = 0x1; 

   }; // class jFexSRJetRoI_v1

} // namespace xAOD

// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::jFexSRJetRoI_v1, SG::AuxElement );

#endif //XAODTRIGGER_VERSIONS_JFEXSRJETROI_V1_H
