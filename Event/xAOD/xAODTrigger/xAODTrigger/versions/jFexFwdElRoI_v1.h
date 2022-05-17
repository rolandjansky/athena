//create
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGGER_VERSIONS_JFEXFWDELROI_V1_H
#define XAODTRIGGER_VERSIONS_JFEXFWDELROI_V1_H

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

   class jFexFwdElRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      jFexFwdElRoI_v1();

      /// initialze the EDM
      void initialize(uint8_t jFexNumber,uint8_t fpgaNumber, uint32_t tobWord, char istob, int resolution, float_t eta, float_t phi );

      /// The "raw" 32-bit word describing the object candidate, 27 bit-word used at hardware level
      uint32_t  tobWord()       const; //need 27/32 bits
      /// Decoded from Tob (for convenience)
      uint16_t  tobEt()         const; //need 11 bits
      uint8_t   tobEMIso()      const; //need 2 bits
      uint8_t   tobEMf1()       const; //need 2 bits
      uint8_t   tobEMf2()       const; //need 2 bits
      uint8_t   tobLocalEta()   const; //need 5 bits  
      uint8_t   tobLocalPhi()   const; //need 4 bits 
      uint8_t   tobSat()        const; //need 1 bits
      /// Calculated from Tob 
      int       globalEta()     const;
      uint      globalPhi()     const;
      /// Additional info (initialization)
      uint8_t   jFexNumber()    const;
      uint8_t   fpgaNumber()    const;
      float     eta()           const;
      float     phi()           const;
      int       tobEtScale()    const;
      char      isTOB()         const;
      ///  Isolation and emfraction variables (not available yet) 
      ///     Bitwise: EtEMiso, EtEM, EtHad1, ETHad2
      ///     also possible floats: EtEMiso/(tobEt+EtEMiso), EtHad1/(EtEM+EtHad1), EtHad2/(EtEM+EtHad2)  
      uint16_t  EtEMiso()        const; 
      uint16_t  EtEM()           const;
      uint16_t  EtHad1()         const;
      uint16_t  EtHad2()         const;

      /// Set the isTOB variable (TOB or xTOB)
      void setIsTOB( char value);

      /// Set the "raw" words describing the object candidate, need 27/32 bits
      void setTobWord(uint32_t tobWord );
      void setjFexNumber(uint8_t jFexNumber);
      void setfpgaNumber(uint8_t fpgaNumber); 
     
      /// TOB ET (decoded from TOB, stored for convenience)
      unsigned int unpackEtTOB() const;
      void setTobEt( uint16_t value); 
      unsigned int et() const;
       
      /// TOB Isolation bits (decoded from TOB, stored for convenience)
      unsigned int unpackEMIsoTOB() const;
      void setTobEMIso( uint8_t value); 

      /// TOB EM fraction bits (decoded from TOB, stored for convenience)
      unsigned int unpackEMf1TOB() const;
      void setTobEMf1( uint8_t value);
      unsigned int unpackEMf2TOB() const;
      void setTobEMf2( uint8_t value);

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
      int  getGlobalEta() const;
      void setGlobalEta(int value);
      void setEta(float value);

      uint getGlobalPhi() const;
      void setGlobalPhi(uint value);
      void setPhi(float value);
      
      //Et resolution
      void setResolution(int value);      

      // additional variables (not available yet)
      unsigned int getEtEMiso() const;
      void setEtEMiso(uint16_t value);
      unsigned int getEtEM() const;
      void setEtEM(uint16_t value);
      unsigned int getEtHad1() const;
      void setEtHad1(uint16_t value);
      unsigned int getEtHad2() const;
      void setEtHad2(uint16_t value);



   private:
      //Constants used in converting to ATLAS units (see SR jets)
      static const std::vector<int> s_FWD_EtaPosition;
      static const std::vector<int> s_FCAL_EtaPosition;

      // Data locations within word
      static const int s_resBit  = 27;
      static const int s_emf2Bit = 25;
      static const int s_emf1Bit = 23;
      static const int s_isoBit  = 21;
      static const int s_etBit   = 10;
      static const int s_etaBit  = 5;
      static const int s_phiBit  = 1;
      static const int s_satBit  = 0;

      //Data masks
      static const int s_resMask  = 0x1f;
      static const int s_emf2Mask = 0x3;
      static const int s_emf1Mask = 0x3;
      static const int s_isoMask  = 0x3;
      static const int s_etMask   = 0x7ff;
      static const int s_etaMask  = 0x1f;
      static const int s_phiMask  = 0xf;
      static const int s_satMask  = 0x1; 

   }; // class jFexFwdElRoI_v1

} // namespace xAOD
// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::jFexFwdElRoI_v1, SG::AuxElement );

#endif //XAODTRIGGER_VERSIONS_JFEXFWDELROI_V1_H
