
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/gFexJetRoI_v1.h"

namespace xAOD {

  /// Constants used in converting to ATLAS units
  const float gFexJetRoI_v1::s_tobEtScale = 3200.; ///3.2 GeV is the energy range size (step between two adjiacent bits)
  const float gFexJetRoI_v1::s_centralPhiWidth = 0.2; ///Phi is 32-bit starting from 0 with steps of 0.2; gPhi = 0 is 0.0 < phi < 0.2 while gPhi = 31 is 6.2 < phi < 6.4 
  const float gFexJetRoI_v1::s_forwardPhiWidth = 0.4; ///Phi is 32-bit starting from 0 with steps of 0.4; gPhi = 0 is 0.0 < phi < 0.4 while gPhi = 15 is 6.2 < phi < 6.4 
  const std::vector<float> gFexJetRoI_v1::s_EtaPosition = { -4.9, -4.45, -4.0, -3.5, -3.2, -3.1, 
                                                            -2.9, -2.7, -2.5, -2.2, -2.0, -1.8, -1.6, -1.4, -1.2, -1.0,  
                                                            -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0,                                                 
                                                            1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.5, 2.7, 2.9,
                                                            3.1, 3.2, 3.5, 4.0, 4.45, 4.9 };

  //vector<float> gFexJetRoI_v1::s_maxEta = {}; 

   gFexJetRoI_v1::gFexJetRoI_v1()
      : SG::AuxElement() {

   }

   void gFexJetRoI_v1::initialize( uint32_t word ) {

      setWord( word );
      setgFexType(unpackType());
      setTobEt( unpackEtIndex() );
      setEta( unpackEtaIndex() );
      setPhi( unpackPhiIndex() );
      setStatus( unpackStatus());
      setSaturated(unpackSaturated());

   }


   /// Raw data words
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( gFexJetRoI_v1, uint32_t, word,
                                         setWord )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( gFexJetRoI_v1, char, status,
                                         setStatus )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( gFexJetRoI_v1, char, saturated,
                                         setSaturated )

   /// Only calculable externally
   ///eFex puts here RetaCore, RetaEnv, RhadEM, RhadHad, WstotNumerator etc (coming from algorithms)
   
   /// Extracted from data words
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( gFexJetRoI_v1, int, gFexType,
                                         setgFexType )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( gFexJetRoI_v1, uint16_t, tobEt,
                                         setTobEt )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( gFexJetRoI_v1, uint8_t, iEta,
                                         setEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( gFexJetRoI_v1, uint8_t, iPhi,
                                         setPhi )


   /// Methods to decode data from the TOB/RoI and return to the user


   /// Object disambiguation ()
   int gFexJetRoI_v1::unpackType() const {
     auto tobID = (word() >> s_tobIDBit) & s_tobIDMask;
     if (tobID == 0 ){
      return gRho;
     }  
     else if (tobID == 1 || tobID == 2 || tobID == 3 || tobID == 4 ) {
       return gBlock;
     }      
     else if (tobID == 5 || tobID == 6) {
      return gJet;
     }  
     else return -999;
   }

   bool gFexJetRoI_v1::isgBlock() const {
     if (gFexType() == gBlock){
        return true;
     }
     return false;
   }

   bool gFexJetRoI_v1::isgJet() const {
     if (gFexType() == gJet){
        return true;
     }
     return false;
   }

   bool gFexJetRoI_v1::isgRho() const {
     if (gFexType() == gRho){
        return true;
     }
     return false;
   }

   bool gFexJetRoI_v1::isLeadingJet() const {
     auto tobID = (word() >> s_tobIDBit) & s_tobIDMask;
     if (tobID == 1 || tobID == 2 || tobID == 5 || tobID == 6){
        return true;
     }
     return false;
   }

   unsigned int gFexJetRoI_v1::unpackStatus() const{
    return (word() >> s_statusBit) & s_statusMask;
   }

   unsigned int gFexJetRoI_v1::unpackSaturated() const{
    return (word() >> s_saturBit) & s_saturMask;
   }
    

   /// Raw ET on TOB scale (3200 MeV/count)
   unsigned int gFexJetRoI_v1::unpackEtIndex() const {
    // Data content = TOB
    return (word() >> s_etBit) & s_etMask;
   
   }

   /// Return an eta index in the range 0-49
   unsigned int gFexJetRoI_v1::unpackEtaIndex() const {
     return (word() >> s_etaBit) & s_etaMask;

   }
   
   /// Return an eta index in the range 0-32
   unsigned int gFexJetRoI_v1::unpackPhiIndex() const {
     return (word() >> s_phiBit) & s_phiMask;

   }

   /// Methods that require combining results or applying scales

   /// ET on TOB scale
   float gFexJetRoI_v1::etMin() const {
     return tobEt()*s_tobEtScale;
   }

   float gFexJetRoI_v1::etMax() const {
     return tobEt()*s_tobEtScale + s_tobEtScale;
   }

   /// Floating point coordinates. Return he minimum Eta and he maximum Eta of the Eta range. 
   float gFexJetRoI_v1::etaMin() const {
     if (gFexType() != gRho){
         return s_EtaPosition[iEta()];
     } 
     return -999;

   }
   
   float gFexJetRoI_v1::etaMax() const {
     if (gFexType() != gRho){
       return s_EtaPosition[iEta()+1];
     } 
     return -999;

   }

   /// Floating point coordinates. 
   float gFexJetRoI_v1::phiMin() const {
     if (gFexType() != gRho){
       if (( iEta() <= 4 ) || ( (iEta() >= 34) && (iEta() <= 38) )){
        return iPhi() * s_forwardPhiWidth;
       }
       else if ( iEta() > 4 && iEta() < 34 ){
        return iPhi() * s_centralPhiWidth;
       } 
       else return -999; 
     } 
     return -999; 
   }

   float gFexJetRoI_v1::phiMax() const {
     if (gFexType() != gRho){
       if (( iEta() <= 4 ) || ( iEta() >= 34 && iEta() <= 38 )){
        return iPhi() * s_forwardPhiWidth + s_forwardPhiWidth;
       }
       else if ( iEta() > 4 && iEta() < 34 ){
        return iPhi() * s_centralPhiWidth + s_centralPhiWidth;
       } 
       else return -999; 
     } 
     return -999; 
   }

  


} // namespace xAOD
