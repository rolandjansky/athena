//create
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGGER_VERSIONS_JFEXSUMETROI_V1_H
#define XAODTRIGGER_VERSIONS_JFEXSUMETROI_V1_H

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

   class jFexSumETRoI_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      jFexSumETRoI_v1();

      /// In future initialze the xTOB as well, word1
      void initialize( uint8_t jFexNumber,uint8_t fpgaNumber, uint32_t tobWord, int resolution) ;

      /// The "raw" 32-bit word describing the object candidate
      uint32_t  tobWord()      const;
      uint8_t   fpgaNumber()   const;
      uint8_t   jFexNumber()   const;
      uint16_t  tobEt_lower()  const;
      uint8_t   tobSat_lower() const;
      uint16_t  tobEt_upper()  const;
      uint8_t   tobSat_upper() const;
      int       tobEtScale()    const;

      /// Set the "raw" 32-bit words describing the object candidate
      void setTobWord   ( uint32_t tobWord   ) ;
      void setjFexNumber( uint8_t  jFexNumber) ;
      void setfpgaNumber( uint8_t  fpgaNumber) ;
      
      //Unpack Et_lower from the SumEt TOB word     
      uint16_t unpackEt_lower() const;
      void setTobEt_lower( uint16_t value) ; 
      int Et_lower() const;
      
      //Unpack Et_upper from the SumEt TOB word     
      uint16_t unpackEt_upper() const;
      void setTobEt_upper( uint16_t value) ; 
      int Et_upper() const;
      
      //Unpack saturation for lower from the SumEt TOB word     
      uint8_t unpackSat_lower() const;
      void setTobSat_lower( uint8_t value) ; 
      
      //Unpack saturation for upper from the SumEt TOB word     
      uint8_t unpackSat_upper() const;
      void setTobSat_upper( uint8_t value) ; 
            
      //Et resolution
      void setResolution(int value);
    
   private:

      // Data locations within word
      static const int s_Sat_upperBit = 31;
      static const int s_Et_upperBit  = 16;
      static const int s_Et_lowerBit  = 1; 
      static const int s_Sat_lowerBit = 0; 
      
      //Data masks
      static const int s_Sat_upperMask  = 0x1;
      static const int s_Et_upperMask   = 0x7fff;
      static const int s_Et_lowerMask   = 0x7fff;
      static const int s_Sat_lowerMask  = 0x1;
      

   }; // class jFexSumETRoI_v1

} // namespace xAOD
// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::jFexSumETRoI_v1, SG::AuxElement );

#endif //XAODTRIGGER_VERSIONS_JFEXSumETROI_V1_H
