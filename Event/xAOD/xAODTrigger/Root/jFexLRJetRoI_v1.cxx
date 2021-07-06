/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/jFexLRJetRoI.h"

namespace xAOD {

  const float jFexLRJetRoI_v1::s_tobEtScale = 200.;
  const float jFexLRJetRoI_v1::s_towerEtaWidth = 0.1;
  const float jFexLRJetRoI_v1::s_towerPhiWidth = 0.1;
  const float jFexLRJetRoI_v1::s_minEta = -4.9;

   jFexLRJetRoI_v1::jFexLRJetRoI_v1()
     : SG::AuxElement() {
   }
   void jFexLRJetRoI_v1::initialize( uint8_t jFexNumber, uint8_t fpgaNumber, uint32_t tobWord) {
 
     setTobWord( tobWord );
     setjFexNumber( jFexNumber );
     setfpgaNumber(fpgaNumber);
     setTobEt(unpackEtTOB());
     setTobLocalEta( unpackEtaIndex() );
     setTobLocalPhi( unpackPhiIndex() );
     setTobSat(unpackSaturationIndex());
     setGlobalEta(unpackGlobalEta());
     setGlobalPhi(unpackGlobalPhi());
     setEta( (unpackGlobalEta()+0.5)/10 ); 
     setPhi( (unpackGlobalPhi()+0.5)/10 ); 
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

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexLRJetRoI_v1, uint32_t, tobWord     , setTobWord    )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexLRJetRoI_v1, uint8_t , jFexNumber  , setjFexNumber )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexLRJetRoI_v1, uint8_t , fpgaNumber  , setfpgaNumber )   
   /// Extracted from data words
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexLRJetRoI_v1, uint16_t, tobEt       , setTobEt       )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexLRJetRoI_v1, uint8_t , tobLocalEta , setTobLocalEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexLRJetRoI_v1, uint8_t , tobLocalPhi , setTobLocalPhi )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexLRJetRoI_v1, uint8_t , tobSat      , setTobSat      )
 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexLRJetRoI_v1, int , globalEta, setGlobalEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexLRJetRoI_v1, uint, globalPhi, setGlobalPhi )
   
  ///global coordinates, stored for furture use but not sent to L1Topo    
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexLRJetRoI_v1, float, eta, setEta)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexLRJetRoI_v1, float, phi, setPhi)

   //-----------------
   /// Methods to decode data from the TOB/RoI and return to the user
   //-----------------

  //include in future when xTOB in jFEX has been implemented.
   
   /// TOB or xTOB?
   //jFexLRJetRoI_v1::ObjectType jFexLRJetRoI_v1::type() const {
   //if (Word1() == 0) return TOB;
   //else              return xTOB;
   //}

   //Hardware coordinate elements  

   //Raw ET on TOB scale (200 MeV/count)
    unsigned int jFexLRJetRoI_v1::unpackEtTOB() const{
     //Data content = TOB
     return (tobWord() >> s_etBit) & s_etMask;

    } 

   //Return an eta index
   unsigned int jFexLRJetRoI::unpackEtaIndex() const {
     return (tobWord() >> s_etaBit) & s_etaMask;
   }
   //Return a phi index
   unsigned int jFexLRJetRoI::unpackPhiIndex() const {
     return (tobWord() >> s_phiBit) & s_phiMask;
   }

   //Return sat flag
   unsigned int jFexLRJetRoI::unpackSaturationIndex() const{
     return (tobWord() >> s_satBit) & s_satMask;
   }

   /// Methods that require combining results or applying scales

   /// ET on TOB scale
   unsigned int jFexLRJetRoI_v1::et() const {
    //Return the TOB Et in a 1 MeV scale
     return tobEt()*s_tobEtScale;
   }

  //global coords
  
  /// As the Trigger towers are 1x1 in Eta - Phi coords (x10), we add 0.5 to centre them
  int jFexLRJetRoI_v1::unpackGlobalEta() const{
      
    int globalEta = 0;
    if(jFexNumber()==0){
        globalEta = -25+tobLocalEta(); //-24.5 is the minimum eta for the most granular part of module 0 - needs to be modified for the EMEC/HEC and FCAL
    }
    else if(jFexNumber()==5){
        globalEta = 16+tobLocalEta(); //16.5 is the minimum eta for the most granular part of module 5 - needs to be modified for the EMEC/HEC and FCAL
    }
    else{
        globalEta = tobLocalEta()+(8*(jFexNumber() - 3)) ;  // for module 1 to 4 
    }
    return globalEta;
  }

  uint jFexLRJetRoI_v1::unpackGlobalPhi() const{
     uint globalPhi = tobLocalPhi() + (fpgaNumber() * 16); 
     return globalPhi; 
  
  }


} // namespace xAOD


