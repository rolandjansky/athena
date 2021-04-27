/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/jFexSRJetRoI.h"

namespace xAOD {

  const float jFexSRJetRoI_v1::s_tobEtScale = 200.;
  const float jFexSRJetRoI_v1::s_towerEtaWidth = 0.1;
  const float jFexSRJetRoI_v1::s_towerPhiWidth = 0.1;
  const float jFexSRJetRoI_v1::s_minEta = -4.9;

   jFexSRJetRoI_v1::jFexSRJetRoI_v1()
     : SG::AuxElement() {
   }
   void jFexSRJetRoI_v1::initialize( uint8_t jFexNumber, uint32_t word0) {
 
     setWord0( word0 );
      //setWord1( word1);
   
     setjFexNumber( jFexNumber );
        
     setTobEt(unpackEtTOB());
     setEta( unpackEtaIndex() );
     setPhi( unpackPhiIndex() ); 
     setSatFlag(unpackSaturationIndex());
      
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

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint32_t, word0,
                                         setWord0 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint8_t, jFexNumber,
                                         setjFexNumber )
   /// Only calculable externally
 
   /// Extracted from data words, stored for convenience
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint16_t, tobEt,
                                         setTobEt )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint8_t, iEta,
                                         setEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint8_t, iPhi,
                                         setPhi )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexSRJetRoI_v1, uint8_t, satFlag,
                                         setSatFlag)
   //-----------------
   /// Methods to decode data from the TOB/RoI and return to the user
   //-----------------

  //include in future when xTOB in jFEX has been implemented.
   
   /// TOB or xTOB?
   //jFexSRJetRoI_v1::ObjectType jFexSRJetRoI_v1::type() const {
   //if (Word1() == 0) return TOB;
   //else              return xTOB;
   //}

   //Hardware coordinate elements  

   //Raw ET on TOB scale (200 MeV/count)
    unsigned int jFexSRJetRoI_v1::unpackEtTOB() const{
     //Data content = TOB
     return (word0() >> s_etBit) & s_etMask;

    } 

   //Return an eta index
   unsigned int jFexSRJetRoI::unpackEtaIndex() const {
     return (word0() >> s_etaBit) & s_etaMask;
   }
   //Return a phi index
   unsigned int jFexSRJetRoI::unpackPhiIndex() const {
     return (word0() >> s_phiBit) & s_phiMask;
   }

   //Return sat flag
   unsigned int jFexSRJetRoI::unpackSaturationIndex() const{
     return (word0() >> s_satBit) & s_satMask;
   }

   /// Methods that require combining results or applying scales

   /// ET on TOB scale
   unsigned int jFexSRJetRoI_v1::et() const {
     return tobEt()*s_tobEtScale;
   }

   /// Floating point coordinates
   unsigned int jFexSRJetRoI_v1::eta() const{
       float eta_value = iEta() * s_towerEtaWidth;
       if(eta_value > 2.5 || eta_value < -2.5 ) return  0;
       // in future, change this to include the different eta phi width depending on eta value. eta_value = iEta * s_towerEtaForwardWidth; 
       return eta_value;
   }

  unsigned int jFexSRJetRoI_v1::phi() const {
     float value = iPhi() * M_PI/32. + M_PI/64.;
     if (value > M_PI) value = value - 2.*M_PI;
     return value;
   }
} // namespace xAOD

