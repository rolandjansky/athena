/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/jFexMETRoI.h"

namespace xAOD {

  const float jFexMETRoI_v1::s_tobEtScale = 200.;
  const float jFexMETRoI_v1::s_tobIsoScale = 200.;
  const float jFexMETRoI_v1::s_towerEtaWidth = 0.1;
  const float jFexMETRoI_v1::s_towerPhiWidth = 0.1;
  const float jFexMETRoI_v1::s_minEta = -2.5;

   jFexMETRoI_v1::jFexMETRoI_v1()
     : SG::AuxElement() {
   }
   void jFexMETRoI_v1::initialize(uint32_t word0) {
 
     setWord0( word0 );
     setE_x( E_x());
     setE_y( E_y());
     setRes( Res());
     setSat( Sat());

   //include in future when xTOB in jFEX has been implemented.

   // If the object is a TOB then the isTOB should be true.
   // For xTOB default is false, but should be set if a matching TOB is found 
   // if (type() == TOB) setIsTOB(1);
   // else               setIsTOB(0);

      return;
   }

   //----------------
   /// Raw data words
   //----------------

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, uint32_t, word0, setWord0)
   /// Only calculable externally
 
   /// Extracted from data words, stored for convenience
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, uint16_t, E_x, setE_x)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, uint16_t, E_y, setE_y)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, uint8_t, Res, setRes)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexMETRoI_v1, uint8_t, Sat, setSat)
   //-----------------
   /// Methods to decode data from the TOB/RoI and return to the user
   //-----------------

  //include in future when xTOB in jFEX has been implemented.
   
   /// TOB or xTOB?
   //jFexMETRoI_v1::ObjectType jFexMETRoI_v1::type() const {
   //if (Word1() == 0) return TOB;
   //else              return xTOB;
   //}

   //Hardware coordinate elements
   
   //Return Ex
   unsigned int jFexMETRoI_v1::getE_x() const{
     return (word0() >> s_E_xBit) & s_E_xMask;
    }

   unsigned int jFexMETRoI_v1::getE_y() const{
     return (word0() >> s_E_yBit) * s_E_yMask;
   }

   //Return sat upper flag
   unsigned int jFexMETRoI_v1::getSat() const{
     return (word0() >> s_SatBit) & s_SatMask;
   }

   //Return sat lower flag
   unsigned int jFexMETRoI_v1::getRes() const{
     return (word0() >> s_ResBit) & s_ResMask;
   }
} // namespace xAOD

