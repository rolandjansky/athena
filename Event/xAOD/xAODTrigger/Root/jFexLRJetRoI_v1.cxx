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

  // globalEta/Phi calculation in the FCAL varies depending on position in eta space due to TT granularity change.
  //| Region          |      eta region     | TT (eta x phi)
  //---------------------------------------------------------
  // Region 1  EMB    |      |eta| <  25    | (1 x 1)
  // Region 2  EMIE   | 25 < |eta|< 31      | (2 x 2)
  // Region 3  TRANS  | 31 < |eta| < 32     | (1 x 2)
  // Region 4  FCAL   |      |eta| > 32     | (2 x 4)                 

  //eta position in FCAL FPGAs
  const std::vector<int> jFexLRJetRoI_v1::s_FWD_EtaPosition  =   {0,  8,   //Region 1 
                                                                  9, 11,   //Region 2
                                                                     12,   //Region 3
                                                                 13, 23};  //Region 4

    //eta position of FCAL EM layer as an integer
    //Needs to be modified with firmware values
    const std::vector<int> jFexLRJetRoI_v1::s_FCAL_EtaPosition = {32,34,35,37,38,40,41,43,44,46,47,49};
    
 
   jFexLRJetRoI_v1::jFexLRJetRoI_v1()
     : SG::AuxElement() {
   }
   void jFexLRJetRoI_v1::initialize( uint8_t jFexNumber, uint8_t fpgaNumber, uint32_t tobWord, float_t eta, float_t phi) {
 
     setTobWord( tobWord );
     setjFexNumber( jFexNumber );
     setfpgaNumber(fpgaNumber);
     setTobEt(unpackEtTOB());
     setTobLocalEta( unpackEtaIndex() );
     setTobLocalPhi( unpackPhiIndex() );
     setTobSat(unpackSaturationIndex());
     setGlobalEta(unpackGlobalEta());
     setGlobalPhi(unpackGlobalPhi());
     setEta( eta );
     setPhi( phi );

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
   //
   //Methods to decode data from the TOB/RoI and return to the user
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

  // Global coords
  // As the Trigger towers are 1x1 in Eta - Phi coords (x10). This changes in the FCAL, and depends on the eta position
    int jFexLRJetRoI_v1::unpackGlobalEta() const {
        int globalEta = 0;

        if(jFexNumber()==5 ) {

            if(tobLocalEta() <=s_FWD_EtaPosition[1]) { //Region 1
                globalEta = (tobLocalEta() + (8*(jFexNumber() -3)) );
            }
            else if(tobLocalEta() <=s_FWD_EtaPosition[3]) { //Region 2
                globalEta = 25 +2*(tobLocalEta()-9);
            }
            else if(tobLocalEta() == s_FWD_EtaPosition[4] ) { //Region 3
                globalEta = 31;
            }
            else if(tobLocalEta() <= s_FWD_EtaPosition[6]) { //Region 4
                globalEta = s_FCAL_EtaPosition[tobLocalEta()-13]-1;
            }

        }
        else if(jFexNumber()==0) {

            if(tobLocalEta() <=s_FWD_EtaPosition[1]) { //Region 1
                globalEta = (8-tobLocalEta() + (8*(jFexNumber() -3)) )-1;
            }
            else if(tobLocalEta() <=s_FWD_EtaPosition[3]) { //Region 2
                globalEta = -27 -2*(tobLocalEta()-9);
            }
            else if(tobLocalEta() == s_FWD_EtaPosition[4] ) { //Region 3
                globalEta = -32;
            }
            else if(tobLocalEta() <= s_FWD_EtaPosition[6]) { //Region 4
                globalEta = -s_FCAL_EtaPosition[tobLocalEta()-13];
            }
        }
        else { //Module 1-4
            globalEta = (tobLocalEta() + (8*(jFexNumber() -3)) );
        }

        return globalEta;
    }

    uint jFexLRJetRoI_v1::unpackGlobalPhi() const {
        uint globalPhi = 0;

        //16 is the phi height of an FPGA
        if(jFexNumber() == 0 || jFexNumber() == 5) {

            if(tobLocalEta() <=s_FWD_EtaPosition[1]) { //Region 1
                globalPhi = tobLocalPhi() + (fpgaNumber() * 16);
            }
            else if(tobLocalEta() <=s_FWD_EtaPosition[4]) {//Region 2 and Region 3 have the same granularity
                globalPhi = (16*fpgaNumber()) + 2*(tobLocalPhi());
            }
            else if(tobLocalEta() <=s_FWD_EtaPosition[6]) {//Region 4
                globalPhi = (16*fpgaNumber()) + 4*(tobLocalPhi());
            }
        }
        else{ //Modules 1-4
            globalPhi = tobLocalPhi() + (fpgaNumber() * 16);
        }

        return globalPhi;

    }


} // namespace xAOD


