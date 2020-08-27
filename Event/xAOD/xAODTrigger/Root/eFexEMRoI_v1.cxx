/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/eFexEMRoI_v1.h"

namespace xAOD {

  /// Constants used in converting to ATLAS units
  const float eFexEMRoI_v1::s_tobEtScale = 100.;
  const float eFexEMRoI_v1::s_xTobEtScale = 25.;
  const float eFexEMRoI_v1::s_towerEtaWidth = 0.1;
  const float eFexEMRoI_v1::s_minEta = -2.5;

   eFexEMRoI_v1::eFexEMRoI_v1()
      : SG::AuxElement() {

   }

   void eFexEMRoI_v1::initialize( uint8_t eFexNumber, uint32_t word0, uint32_t word1 ) {

      setWord0( word0 );
      setWord1( word1 );
      seteFexNumber( eFexNumber );
      setTobEt( etTOB() );
      setEta( etaIndex() );
      setPhi( phiIndex() );

      /** If the object is a TOB then the isTOB should be true.
          For xTOB default is false, but should be set if a matching TOB is found */
      if (type() == TOB) setIsTOB(1);
      else               setIsTOB(0);

      return;
   }


   /// Raw data words
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint32_t, Word0,
                                         setWord0 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint32_t, Word1,
                                         setWord1 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint8_t, eFexNumber,
                                         seteFexNumber )

   /// Only calculable externally
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint16_t, RetaCore,
                                         setRetaCore )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint16_t, RetaEnv,
                                         setRetaEnv )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint16_t, RhadEM,
                                         setRhadEM )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint16_t, RhadHad,
                                         setRhadHad )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint16_t, WstotNumerator,
                                         setWstotNumerator )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint16_t, WstotDenominator,
                                         setWstotDenominator )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint32_t, thrPattern,
                                         setThrPattern )


   /// Should be set for xTOB if there is a matching TOB
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, char, isTOB,
                                         setIsTOB )

   /// Extracted from data words, stored for convenience
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint16_t, tobEt,
                                         setTobEt )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint8_t, iEta,
                                         setEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexEMRoI_v1, uint8_t, iPhi,
                                         setPhi )


   /// Methods to decode data from the TOB/RoI and return to the user

   /// TOB or xTOB?
   eFexEMRoI_v1::ObjectType eFexEMRoI_v1::type() const {
     if (Word1() == 0) return TOB;
     else              return xTOB;
   }
    
   /// Hardware coordinate elements
   unsigned int eFexEMRoI_v1::fpga() const {
     return (Word0() >> s_fpgaBit) & s_fpgaMask;
   }
    
   unsigned int eFexEMRoI_v1::fpgaEta() const {
     return (Word0() >> s_etaBit) & s_etaMask;
   }
    
   unsigned int eFexEMRoI_v1::fpgaPhi() const {
     return (Word0() >> s_phiBit) & s_phiMask;
   }
    
   unsigned int eFexEMRoI_v1::UpNotDown() const {
     return (Word0() >> s_updownBit) & s_updownMask;
   }
    
   unsigned int eFexEMRoI_v1::seed() const {
     return (Word0() >> s_seedBit) & s_seedMask;
   }
    
   unsigned int eFexEMRoI_v1::seedMax() const {
     return (Word0() >> s_maxBit) & s_maxMask;
   }

   /// Raw ET on TOB scale (100 MeV/count)
   unsigned int eFexEMRoI_v1::etTOB() const {
     // Data content = TOB
     if (Word1() == 0) {
       return (Word0() >> s_etBit) & s_etMask;
     }
     // Data Content = xTOB. Need to remove lower bits and cap range
     else {
       unsigned int etWord = (etXTOB() >> s_etBitXTOB);
       if (etWord > s_etMask) etWord = s_etMask; 
       return etWord;
     }
   }
   
   /// Full precision ET (25 MeV/count, only available if object is an xTOB
   unsigned int eFexEMRoI_v1::etXTOB() const {
     /// If the object is not an xTOB return 0 as high-precision ET unavailable
     if (Word1() == 0) { 
       //return etTOB()*s_tobEtScale/s_xTobEtScale;
       return 0; 
     }
     else {
       return (Word1() >> s_etBit) & s_etFullMask;
     }
   }
    
   /// Results of the 3 jet discriminant algorithms
   unsigned int eFexEMRoI_v1::RetaThresholds() const {
     return (Word0() >> s_veto1Bit) & s_veto1Mask;
   }
    
   unsigned int eFexEMRoI_v1::RhadThresholds() const {
     return (Word0() >> s_veto2Bit) & s_veto2Mask;
   }
    
   unsigned int eFexEMRoI_v1::WstotThresholds() const {
     return (Word0() >> s_veto3Bit) & s_veto3Mask;
   }
    
   unsigned int eFexEMRoI_v1::bcn4() const {
     return (Word1() >> s_bcn4Bit) & s_bcn4Mask;
   }

   /// Return single 32-bit TOB word from an xTOB 
   uint32_t eFexEMRoI_v1::tobWord() const {
     // Do something sensible if called for a TOB
     if (Word1() == 0) return Word0();
     // When called for xTOB
     else {
       uint32_t word = Word0() + etTOB();
       return word;
     }
   }

   /// Methods that require combining results or applying scales
   /// ET on TOB scale
   float eFexEMRoI_v1::et() const {
     return tobEt()*s_tobEtScale;
   }

   /// Floating point coordinates
   float eFexEMRoI_v1::eta() const {
     return (s_minEta + iEta()*s_towerEtaWidth + (seed()+0.5)*s_towerEtaWidth/4);
   }

   float eFexEMRoI_v1::phi() const {
     float value = iPhi() * M_PI/32. + M_PI/64.;
     if (value > M_PI) value = value - 2.*M_PI;
     return value;
   }

   /// Jet discriminant values. 
   /// Note that these are for convenience & intelligibility, but these should
   /// not be used for bitwise-accurate menu studies
   float eFexEMRoI_v1::Reta() const {
     float sum = RetaCore() + RetaEnv();
     if (sum > 0) return (1. - RetaCore()/sum);
     else         return -1.;
   }

   float eFexEMRoI_v1::Rhad() const {
     float sum = RhadEM() + RhadHad();
     if (sum > 0) return (RhadHad()/sum);
     else         return -1.;
   }

   float eFexEMRoI_v1::Wstot() const {
     if (WstotDenominator() > 0) return (WstotNumerator()/WstotDenominator());
     else         return -1.;
   }

   /// Methods that decode the eFEX number

  /// Return phi index in the range 0-63
  int eFexEMRoI_v1::phiIndex() const {

     /// Get the eFEX index in phi (1-8, unfortunately)
     unsigned int eFEX = (eFexNumber() >> s_eFexPhiBit) & s_eFexPhiMask;

     /// Find global phi index (0-63) for this window in this eFEX (the -1 is to correct for the eFEX phi index)
     unsigned int index = s_eFexPhiWidth*eFEX + s_eFexPhiOffset  + fpgaPhi() -1;
     if (index >= s_numPhi) index -= s_numPhi;
     return index;
   }

   /// Return an eta index in the range 0-49
   /// Note that this may not be the final format!
   /// And you need to combine with the seed() value to get full eta precision
   int eFexEMRoI_v1::etaIndex() const {

     /// Get the eFEX number
     uint8_t eFEX = (eFexNumber() >> s_eFexEtaBit) & s_eFexEtaMask;

     /// FPGA min eta
     uint8_t index = 99; /// Define a default value in case of invalid eFEX number

     switch (eFEX) {
       case eFexC: index = s_EtaCOffset + fpga()*s_fpgaEtaWidth + (fpga() > 0 ? 1 : 0);
                   break;
       case eFexB: index = s_EtaBOffset + fpga()*s_fpgaEtaWidth;
                   break;
       case eFexA: index = s_EtaAOffset + fpga()*s_fpgaEtaWidth;
                   break;
     }

     /// Add eta location within the FPGA & return value
     return index + fpgaEta();

   }



} // namespace xAOD
