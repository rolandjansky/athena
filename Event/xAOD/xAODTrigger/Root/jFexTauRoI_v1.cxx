/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/jFexTauRoI.h"

namespace xAOD {

  const float jFexTauRoI_v1::s_tobEtScale = 200.;
  const float jFexTauRoI_v1::s_tobIsoScale = 200.;
  const float jFexTauRoI_v1::s_towerEtaWidth = 0.1;
  const float jFexTauRoI_v1::s_towerPhiWidth = 0.1;
  const float jFexTauRoI_v1::s_minEta = -2.5;

   jFexTauRoI_v1::jFexTauRoI_v1()
     : SG::AuxElement() {
   }
   void jFexTauRoI_v1::initialize( uint8_t jFexNumber, uint32_t word0) {
 
     setWord0( word0 );
     setjFexNumber( jFexNumber );

     setEta( unpackEtaIndex() );
     setPhi( unpackPhiIndex() ); 
     setTobEt(unpackEtTOB());
     setTobIso(unpackIsoTOB());
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

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint32_t, word0     , setWord0     )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint8_t, jFexNumber,setjFexNumber )
   /// Only calculable externally
 
   /// Extracted from data words, stored for convenience
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint8_t , iEta   ,    setEta  )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint8_t , iPhi   ,    setPhi  )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint16_t, tobEt  ,  setTobEt  )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint16_t, tobIso , setTobIso  )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( jFexTauRoI_v1, uint8_t , satFlag, setSatFlag )
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
     return (word0() >> s_etBit) & s_etMask;
    }
    
    //Return the isolation , scale (200 MeV/count)
    unsigned int jFexTauRoI_v1::unpackIsoTOB() const{
     return (word0() >> s_isoBit) & s_isoMask;
    } 

   //Return an eta index
   unsigned int jFexTauRoI_v1::unpackEtaIndex() const {
     return (word0() >> s_etaBit) & s_etaMask;
   }
   //Return a phi index
   unsigned int jFexTauRoI_v1::unpackPhiIndex() const {
     return (word0() >> s_phiBit) & s_phiMask;
   }

   //Return sat flag
   unsigned int jFexTauRoI_v1::unpackSaturationIndex() const{
     return (word0() >> s_satBit) & s_satMask;
   }

   /// Methods that require combining results or applying scales

   /// ET on TOB scale
   unsigned int jFexTauRoI_v1::et() const {
     //return TOB Et in a 200 MeV scale
     return tobEt();
   }
   
   /// Iso on TOB scale
   unsigned int jFexTauRoI_v1::iso() const {
    //return TOB Isolation in a 200 MeV scale
     return tobIso();
   }

   /// Return the local coordinated within the FPGA core area
   unsigned int jFexTauRoI_v1::eta() const{
       return iEta();
   }

  unsigned int jFexTauRoI_v1::phi() const {
     return iPhi();
   }
   
  unsigned int jFexTauRoI_v1::tob() const {
     return word0();
   }
} // namespace xAOD

