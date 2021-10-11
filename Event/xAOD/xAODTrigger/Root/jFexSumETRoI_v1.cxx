/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/jFexSumETRoI.h"

namespace xAOD {

  const float jFexSumETRoI_v1::s_tobEtScale = 200.;
  const float jFexSumETRoI_v1::s_tobIsoScale = 200.;
  const float jFexSumETRoI_v1::s_towerEtaWidth = 0.1;
  const float jFexSumETRoI_v1::s_towerPhiWidth = 0.1;
  const float jFexSumETRoI_v1::s_minEta = -2.5;

   jFexSumETRoI_v1::jFexSumETRoI_v1()
     : SG::AuxElement() {
   }
   void jFexSumETRoI_v1::initialize( uint8_t jFexNumber,uint8_t fpgaNumber, uint32_t word0) {
 
     setWord0( word0 );
     setjFexNumber( jFexNumber );
     setfpgaNumber( fpgaNumber );     
     setEt_upper(getEt_upper());
     setSat_upper(getSat_upper());
     setEt_lower(getEt_lower());
     setSat_lower(getSat_lower());
  
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

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint32_t, word0, setWord0)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint8_t, jFexNumber, setjFexNumber)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint8_t, fpgaNumber, setfpgaNumber)
   /// Only calculable externally
 
   /// Extracted from data words, stored for convenience

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint16_t, Et_upper, setEt_upper)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint8_t, Sat_upper, setSat_upper)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint16_t, Et_lower, setEt_lower)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSumETRoI_v1, uint8_t, Sat_lower, setSat_lower)

     //-----------------
   /// Methods to decode data from the TOB/RoI and return to the user
   //-----------------

  //include in future when xTOB in jFEX has been implemented.
   
   /// TOB or xTOB?
   //jFexSumETRoI_v1::ObjectType jFexSumETRoI_v1::type() const {
   //if (Word1() == 0) return TOB;
   //else              return xTOB;
   //}

   //Hardware coordinate elements  
   
    uint32_t jFexSumETRoI_v1::gettob() const {
        return word0();
    }
    
    unsigned int jFexSumETRoI_v1::getjFexNumber() const {
        return jFexNumber();
    }

    unsigned int jFexSumETRoI_v1::getfpgaNumber() const {
        return fpgaNumber();
    }
   
   //Raw ET on TOB scale (200 MeV/count)
    unsigned int jFexSumETRoI_v1::getEt_upper() const{
     return (word0() >> s_Et_upperBit) & s_Et_upperMask;
    }
   unsigned int jFexSumETRoI_v1::getEt_lower() const{
     return (word0() >> s_Et_lowerBit) & s_Et_lowerMask;
   }

   //Return sat upper flag
   unsigned int jFexSumETRoI_v1::getSat_upper() const{
     return (word0() >> s_Sat_upperBit) & s_Sat_upperMask;
   }

   //Return sat lower flag
   unsigned int jFexSumETRoI_v1::getSat_lower() const{
     return (word0() >> s_Sat_lowerBit) & s_Sat_lowerMask;
   }

} // namespace xAOD

