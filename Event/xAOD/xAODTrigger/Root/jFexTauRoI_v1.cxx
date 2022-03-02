/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/jFexTauRoI.h"

namespace xAOD {

   jFexTauRoI_v1::jFexTauRoI_v1()
     : SG::AuxElement() {
   }
   void jFexTauRoI_v1::initialize( uint8_t jFexNumber,uint8_t fpgaNumber, uint32_t tobWord, int resolution, float_t eta, float_t phi) {
 
     setTobWord( tobWord );
     setjFexNumber( jFexNumber );
     setfpgaNumber(fpgaNumber);
     setTobLocalEta( unpackEtaIndex() );
     setTobLocalPhi( unpackPhiIndex() ); 
     setTobEt(unpackEtTOB());
     setTobIso(unpackIsoTOB());
     setTobSat(unpackSaturationIndex());
     setGlobalEta(unpackGlobalEta());
     setGlobalPhi(unpackGlobalPhi());
     setEta( eta );
     setPhi( phi ); 
     setResolution( resolution ); 
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


   /// Only calculable externally
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint32_t, tobWord    , setTobWord    )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint8_t , jFexNumber , setjFexNumber )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint8_t , fpgaNumber , setfpgaNumber )  
   
 
   /// Extracted from data words, stored for convenience
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint8_t , tobLocalEta , setTobLocalEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint8_t , tobLocalPhi , setTobLocalPhi )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint16_t, tobEt       , setTobEt    )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint16_t, tobIso      , setTobIso   )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint8_t , tobSat      , setTobSat   )
 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, int , globalEta, setGlobalEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint, globalPhi, setGlobalPhi )
   
  ///global coordinates, stored for furture use but not sent to L1Topo    
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, float, eta, setEta)
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, float, phi, setPhi)
   
   ///Setting the jFEX ET resolution
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, int  , tobEtScale, setResolution)
   
   
   //-----------------
   /// Methods to decode data from the TOB/RoI and return to the user
   //-----------------

  //include in future when xTOB in jFEX has been implemented.
   
   /// TOB or xTOB?
   //jFexTauRoI_v1::ObjectType jFexTauRoI_v1::type() const {
   //if (Word1() == 0) return TOB;
   //else              return xTOB;
   //}

   //Hardware coordinate elements  

   //Raw ET on TOB scale (200 MeV/count)
    unsigned int jFexTauRoI_v1::unpackEtTOB() const{
     return (tobWord() >> s_etBit) & s_etMask;
    }
    
    //Return the isolation , scale (200 MeV/count)
    unsigned int jFexTauRoI_v1::unpackIsoTOB() const{
     return (tobWord() >> s_isoBit) & s_isoMask;
    } 

   //Return an eta index
   unsigned int jFexTauRoI_v1::unpackEtaIndex() const {
     return (tobWord() >> s_etaBit) & s_etaMask;
   }
   //Return a phi index
   unsigned int jFexTauRoI_v1::unpackPhiIndex() const {
     return (tobWord() >> s_phiBit) & s_phiMask;
   }

   //Return sat flag
   unsigned int jFexTauRoI_v1::unpackSaturationIndex() const{
     return (tobWord() >> s_satBit) & s_satMask;
   }

   /// Methods that require combining results or applying scales

   /// ET on TOB scale
   unsigned int jFexTauRoI_v1::et() const {
     //return TOB Et in a 1 MeV scale
     return tobEt()*tobEtScale();
   }
   
   /// Iso on TOB scale
   unsigned int jFexTauRoI_v1::iso() const {
    //return TOB Isolation in a 1 MeV scale
     return tobIso()*tobEtScale();
   }

  //global coords
  
  /// As the Trigger towers are 1x1 in Eta - Phi coords (x10)
  int jFexTauRoI_v1::unpackGlobalEta() const{
      
    int globalEta = 0;
    if(jFexNumber()==0){
        globalEta = -25+tobLocalEta(); //-25 is the minimum eta for the most granular part of module 0 
    }
    else if(jFexNumber()==5){
        globalEta = 16+tobLocalEta(); //16 is the minimum eta for the most granular part of module 5
    }
    else{
        globalEta = tobLocalEta()+(8*(jFexNumber() - 3)) ;  // for module 1 to 4 
    }
    return globalEta;
  }

  uint jFexTauRoI_v1::unpackGlobalPhi() const{
     uint globalPhi = tobLocalPhi() + (fpgaNumber() * 16); 
     return globalPhi; 
  
  }
} // namespace xAOD

