//create
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGGER_VERSIONS_JFEXMETROI_V1_H
#define XAODTRIGGER_VERSIONS_JFEXMETROI_V1_H

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

   class jFexMETRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      jFexMETRoI_v1();

      /// In future initialze the xTOB as well, word1
      void initialize( uint8_t jFexNumber,uint8_t fpgaNumber, uint32_t tobWord, int resolution);

      /// The "raw" 32-bit word describing the object candidate
      uint32_t  tobWord()       const;
      uint8_t   jFexNumber()    const;
      uint8_t   fpgaNumber()    const;
      int       tobEx()         const;
      int       tobEy()         const;
      uint8_t   tobRes()        const;
      uint8_t   tobSat()        const; 
      int       tobEtScale()    const;

      /// Set the "raw" 32-bit words describing the object candidate
      void setTobWord   ( uint32_t tobWord   );
      void setjFexNumber( uint8_t  jFexNumber);
      void setfpgaNumber( uint8_t  fpgaNumber);   
      
      //Unpack the Met Ex bits from the TOB word in 200 MeV
      int unpackEx() const;
      void setTobEx( int value); 
      int Ex() const;

      //Unpack the Met Ey bits from the TOB word in 200 MeV
      int unpackEy() const;
      void setTobEy( int value); 
      int Ey() const;
      
      //Unpack the reserved bits from the TOB word
      unsigned int  unpackRes() const;
      void setTobRes(uint8_t value);

      //Unpack the saturarion bits from the TOB word
      unsigned int  unpackSat() const;
      void setTobSat(uint8_t value);
      
      //Et resolution
      void setResolution(int value);      

   private:
      //Constants used in converting to ATLAS units
      static const float s_tobEtScale;

      // Data locations within word
      static const int s_ResBit   = 31;
      static const int s_Ey_Bit   = 16;
      static const int s_Ex_Bit   = 1;
      static const int s_SatBit   = 0;
      
      //bit to check if MET is negative
      static const int s_signed_E = 14;

      //Data masks
      static const int s_E_xMask  = 0x7fff;
      static const int s_E_yMask  = 0x7fff;
      static const int s_ResMask  = 0x1;
      static const int s_SatMask  = 0x1;  
        

   }; // class jFexMETRoI_v1

} // namespace xAOD
// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::jFexMETRoI_v1, SG::AuxElement );

#endif //XAODTRIGGER_VERSIONS_JFEXMETROI_V1_H
