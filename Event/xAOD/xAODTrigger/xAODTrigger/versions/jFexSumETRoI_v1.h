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
      void initialize( uint8_t jFexNumber,uint8_t fpgaNumber, uint32_t word0) ;

      /// The "raw" 32-bit word describing the object candidate
      uint32_t word0() const;
      uint8_t fpgaNumber() const;
      uint8_t jFexNumber() const;
      uint16_t Et_lower() const;
      uint8_t Sat_lower() const;
      uint16_t Et_upper() const;
      uint8_t Sat_upper() const;

      /// Set the "raw" 32-bit words describing the object candidate
      void setWord0( uint32_t value );
      void setfpgaNumber( uint8_t fpgaNumber );      
      void setjFexNumber( uint8_t jFexNumber );      
      void setEt_lower(uint16_t value);
      void setSat_lower(uint8_t value);
      void setEt_upper(uint16_t value);
      void setSat_upper(uint8_t value); 

      
      //get the variables from the 32-bit tob word
      uint32_t gettob() const;
      unsigned int getjFexNumber() const;
      unsigned int getfpgaNumber() const;
      unsigned int getEt_lower() const;
      unsigned int getSat_lower() const;
      unsigned int getEt_upper() const;
      unsigned int getSat_upper() const;
    
   private:
      //Constants used in converting to ATLAS units
      static const float s_tobEtScale;
      static const float s_tobIsoScale;
      static const float s_towerEtaWidth;
      static const float s_towerPhiWidth;
      static const float s_minEta;

      // Data locations within word
      static const int s_Et_lowerBit  = 17; 
      static const int s_Sat_lowerBit = 16; 
      static const int s_Et_upperBit  = 1; 
      static const int s_Sat_upperBit = 0;

      //Data masks
      static const int s_Et_lowerMask   = 0x7fff;
      static const int s_Sat_lowerMask  = 0x1;
      static const int s_Et_upperMask   = 0x7fff;
      static const int s_Sat_upperMask  = 0x1; 

   }; // class jFexSumETRoI_v1

} // namespace xAOD
// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::jFexSumETRoI_v1, SG::AuxElement );

#endif //XAODTRIGGER_VERSIONS_JFEXSumETROI_V1_H
