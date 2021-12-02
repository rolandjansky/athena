
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/gFexJetRoI_v1.h"

namespace xAOD {

  /// Constants used in converting to ATLAS units
  const float gFexJetRoI_v1::s_tobEtScale = 200.; ///3.2 GeV is the energy range size (step between two adjiacent bits)
  const float gFexJetRoI_v1::s_centralPhiWidth = M_PI/32; ///Phi is 32-bit starting from 0 with steps of 0.2; gPhi = 0 is 0.0 < phi < 0.2 while gPhi = 31 is 6.2 < phi < 6.4 
  const float gFexJetRoI_v1::s_forwardPhiWidth = M_PI/16; ///Phi is 32-bit starting from 0 with steps of 0.4; gPhi = 0 is 0.0 < phi < 0.4 while gPhi = 15 is 6.2 < phi < 6.4 
  const std::vector<float> gFexJetRoI_v1::s_EtaEdge     = { -4.9, -4.45, -4.0, -3.5, -3.3, -3.1, 
                                                            -2.9, -2.7, -2.5, -2.2, -2.0, -1.8, -1.6, -1.4, -1.2, -1.0,  
                                                            -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0,                                                 
                                                            1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.5, 2.7, 2.9,
                                                            3.1, 3.3, 3.5, 4.0, 4.45, 4.9 }; //Indices 0-3 and 37-40 are niot currently used in hardware (don't receive energy info)
  
  const std::vector<float> gFexJetRoI_v1::s_EtaCenter   = { -4.7, -4.2, -3.7, -3.4, -3.2, -3, 
                                                            -2.8, -2.6, -2.35, -2.1, -1.9, -1.7, -1.5, -1.3, -1.1, -0.9,  
                                                            -0.7, -0.5, -0.3, -0.1, 0.1, 0.3, 0.5, 0.7, 0.9, 1.1,                                                 
                                                            1.3, 1.5, 1.7, 1.9, 2.1, 2.35, 2.6, 2.8, 3.0,
                                                            3.2, 3.4, 3.7, 4.2, 4.7};


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
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( gFexJetRoI_v1, uint8_t, saturated,
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

   uint8_t gFexJetRoI_v1::status() const {
   static const Accessor< uint8_t > acc( "gFexJetStatus" );
   return acc( *this );                                   
   }
   void gFexJetRoI_v1::setStatus ( uint8_t value ) {
      static const Accessor< uint8_t > acc( "gFexJetStatus" );
      acc( *this ) = value;
      return;
   }


   /// Methods to decode data from the TOB/RoI and return to the user


   /// Object disambiguation ()
   int gFexJetRoI_v1::unpackType() const {
     auto tobID = (word() >> s_tobIDBit) & s_tobIDMask;
     if (tobID == 0 ){
      return gRho;
     }  
     else if (tobID == 1 || tobID == 2) {
       return gBlockLead;
     } 
     else if (tobID == 3 || tobID == 4 ) {
       return gBlockSub;
     }      
     else if (tobID == 5 || tobID == 6) {
      return gJet;
     }  
     else return -999;
   }

   bool gFexJetRoI_v1::isgBlockLead() const {
     return gFexType() == gBlockLead;
   }

   bool gFexJetRoI_v1::isgBlockSub() const {
     return gFexType() == gBlockSub;
   }

   bool gFexJetRoI_v1::isgJet() const {
     return gFexType() == gJet;
   }

   bool gFexJetRoI_v1::isgRho() const {
     return gFexType() == gRho;
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

   /// Floating point coordinates. Return the center of Eta. 
   float gFexJetRoI_v1::eta() const {
     if (gFexType() != gRho){
         return s_EtaCenter[iEta()];
     } 
     return -999;

   }

   /// Floating point coordinates. Return he minimum Eta and he maximum Eta of the Eta range. 
   float gFexJetRoI_v1::etaMin() const {
     if (gFexType() != gRho){
         return s_EtaEdge[iEta()];
     } 
     return -999;

   }
   
   float gFexJetRoI_v1::etaMax() const {
     if (gFexType() != gRho){
       return s_EtaEdge[iEta()+1];
     } 
     return -999;

   }

   /// Floating point coordinates. 
   float gFexJetRoI_v1::phi() const {
     if (gFexType() != gRho){
       if (( iEta() <= 5 ) || ( (iEta() >= 32) && (iEta() <= 37) )){
        return (iPhi() * s_forwardPhiWidth) + (s_forwardPhiWidth/2);
       }
       else if ( iEta() > 6 && iEta() < 37 ){
        return (iPhi() * s_centralPhiWidth) + s_centralPhiWidth/2;
       } 
       else return -999; 
     } 
     return -999; 
   }

   /// Floating point coordinates. 
   float gFexJetRoI_v1::phiMin() const {
     if (gFexType() != gRho){
       if (( iEta() <= 6 ) || ( (iEta() >= 32) && (iEta() <= 37) )){
        return iPhi() * s_forwardPhiWidth;
       }
       else if ( iEta() > 6 && iEta() < 32 ){
        return iPhi() * s_centralPhiWidth;
       } 
       else return -999; 
     } 
     return -999; 
   }

   float gFexJetRoI_v1::phiMax() const {
     if (gFexType() != gRho){
       if (( iEta() <= 6 ) || ( iEta() >= 32 && iEta() <= 37 )){
        return iPhi() * s_forwardPhiWidth + s_forwardPhiWidth;
       }
       else if ( iEta() > 6 && iEta() < 32 ){
        return iPhi() * s_centralPhiWidth + s_centralPhiWidth;
       } 
       else return -999; 
     } 
     return -999; 
   }

  


} // namespace xAOD
