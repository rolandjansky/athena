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
      void initialize(uint32_t word0 );

      /// The "raw" 32-bit word describing the object candidate
      uint32_t word0() const;
      uint16_t E_x() const;
      uint16_t E_y() const;
      uint8_t Res() const;
      uint8_t Sat() const; 

      /// Set the "raw" 32-bit words describing the object candidate
      void setWord0( uint32_t word0 );
      
      unsigned int getE_x() const;
      void setE_x(uint16_t value);

      unsigned int getE_y() const;
      void setE_y(uint16_t value);

      unsigned int getRes() const;
      void setRes(uint8_t value);

      unsigned int getSat() const;
      void setSat(uint8_t value);

   private:
      //Constants used in converting to ATLAS units
      static const float s_tobEtScale;
      static const float s_tobIsoScale;
      static const float s_towerEtaWidth;
      static const float s_towerPhiWidth;
      static const float s_minEta;

      // Data locations within word
      static const int s_E_xBit = 27; 
      static const int s_E_yBit = 23; 
      static const int s_ResBit = 12; 
      static const int s_SatBit =  1;

      //Data masks
      static const int s_E_xMask  = 0x1f;
      static const int s_E_yMask  = 0xf;
      static const int s_ResMask  = 0x7ff;
      static const int s_SatMask  = 0x7ff;  

   }; // class jFexMETRoI_v1

} // namespace xAOD
// Declare the inheritance of the type:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::jFexMETRoI_v1, SG::AuxElement );

#endif //XAODTRIGGER_VERSIONS_JFEXMETROI_V1_H
