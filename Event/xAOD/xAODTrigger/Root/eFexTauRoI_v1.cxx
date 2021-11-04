/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <stdexcept>

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/eFexTauRoI_v1.h"

namespace xAOD {

  /// Constants used in converting to ATLAS units
  const float eFexTauRoI_v1::s_tobEtScale = 100.;
  const float eFexTauRoI_v1::s_xTobEtScale = 25.;
  const float eFexTauRoI_v1::s_towerEtaWidth = 0.1;

   eFexTauRoI_v1::eFexTauRoI_v1()
      : SG::AuxElement() {

   }

   void eFexTauRoI_v1::initialize( unsigned int eFexNumber, unsigned int shelf, uint32_t word0 ) {

      // xTOBs will have eFEX and Shelf numbers in word 1 
      // To save space, use the second word of this object, which is not part of a TOB, to store these values
      uint32_t word1 = 0;
      word1 |= (eFexNumber&s_eFexMask)<<s_eFexBit;
      word1 |= (shelf&s_shelfMask)<<s_shelfBit;
      setWord0( word0 );
      setWord1( word1 );

      /** Quantities derived from TOB data, stored for convenience */
      setEt( etTOB()*s_tobEtScale );
      float etaVal = iEta()*s_towerEtaWidth + (seed()+0.5)*s_towerEtaWidth/4;
      setEta( etaVal );
      float phiVal = iPhi() * M_PI/32. + M_PI/64.;
      if (phiVal > M_PI) phiVal = phiVal - 2.*M_PI;
      setPhi( phiVal );

      /** If the object is a TOB then the isTOB should be true.
          For xTOB default is false, but should be set by the user if a matching TOB is found */
      if (type() == TOB) setIsTOB(1);
      else               setIsTOB(0);

      return;
   }


   void eFexTauRoI_v1::initialize( uint32_t word0, uint32_t word1 ) {

      // xTOBs will have eFEX and Shelf numbers in word 1 
      // So all we need to do is set the TOB words
      setWord0( word0 );
      setWord1( word1 );

      /** Quantities derived from TOB data, stored for convenience */
      setEt( etTOB()*s_tobEtScale );
      float etaVal = iEta()*s_towerEtaWidth + (seed()+0.5)*s_towerEtaWidth/4;
      setEta( etaVal );
      float phiVal = iPhi() * M_PI/32. + M_PI/64.;
      if (phiVal > M_PI) phiVal = phiVal - 2.*M_PI;
      setPhi( phiVal );

      /** If the object is a TOB then the isTOB should be true.
          For xTOB default is false, but should be set by the user if a matching TOB is found */
      if (type() == TOB) setIsTOB(1);
      else               setIsTOB(0);

      return;
   }

   /// Raw data words
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTauRoI_v1, uint32_t, word0,
                                         setWord0 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTauRoI_v1, uint32_t, word1,
                                         setWord1 )
 
   /// Only calculable externally
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTauRoI_v1, uint16_t, fCoreNumerator,
                                         setFCoreNumerator )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTauRoI_v1, uint16_t, fCoreDenominator,
                                         setFCoreDenominator )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTauRoI_v1, uint16_t, fHadNumerator,
                                         setFHadNumerator )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTauRoI_v1, uint16_t, fHadDenominator,
                                         setFHadDenominator )

   /// Should be set for xTOB if there is a matching TOB
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTauRoI_v1, char, isTOB,
                                         setIsTOB )

   /// Extracted from data words, stored for convenience
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTauRoI_v1, float, et,
                                         setEt )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTauRoI_v1, float, eta,
                                         setEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( eFexTauRoI_v1, float, phi,
                                         setPhi )


   /// Methods to decode data from the TOB/RoI and return to the user

   /// eFEX number
   unsigned int eFexTauRoI_v1::eFexNumber() const {
     return (word1() >> s_eFexBit) & s_eFexMask;
   }

   /// Shelf number
   unsigned int eFexTauRoI_v1::shelfNumber() const {
     return (word1() >> s_shelfBit) & s_shelfMask;
   }

   /// TOB or xTOB?
   eFexTauRoI_v1::ObjectType eFexTauRoI_v1::type() const {
     if (etXTOB() == 0) return TOB;
     else              return xTOB;
   }
    
   /// Hardware coordinate elements
   unsigned int eFexTauRoI_v1::fpga() const {
     return (word0() >> s_fpgaBit) & s_fpgaMask;
   }
    
   unsigned int eFexTauRoI_v1::fpgaEta() const {
     return (word0() >> s_etaBit) & s_etaMask;
   }
    
   unsigned int eFexTauRoI_v1::fpgaPhi() const {
     return (word0() >> s_phiBit) & s_phiMask;
   }
    
   bool eFexTauRoI_v1::upNotDown() const {
     return (word0() >> s_updownBit) & s_updownMask;
   }
    
   unsigned int eFexTauRoI_v1::seed() const {
     return (word0() >> s_seedBit) & s_seedMask;
   }
    
   bool eFexTauRoI_v1::seedMax() const {
     return (word0() >> s_maxBit) & s_maxMask;
   }

   /// Raw ET on TOB scale (100 MeV/count)
   unsigned int eFexTauRoI_v1::etTOB() const {
     // Data content = TOB
     if (etXTOB() == 0) {
       return (word0() >> s_etBit) & s_etMask;
     }
     // Data Content = xTOB. Need to remove lower bits and cap range
     else {
       unsigned int etWord = (etXTOB() >> s_etBitXTOB);
       if (etWord > s_etMask) etWord = s_etMask; 
       return etWord;
     }
   }
   
   /// Full precision ET (25 MeV/count, only available if object is an xTOB
   unsigned int eFexTauRoI_v1::etXTOB() const {
     /// If the object is not an xTOB this will return 0
     return (word1() >> s_etBit) & s_etFullMask;
   }
    
   /// Results of the 3 jet discriminant algorithms
   unsigned int eFexTauRoI_v1::fCoreThresholds() const {
     return (word0() >> s_veto1Bit) & s_veto1Mask;
   }
    
   unsigned int eFexTauRoI_v1::fHadThresholds() const {
     return (word0() >> s_veto2Bit) & s_veto2Mask;
   }
    
   unsigned int eFexTauRoI_v1::tauThreeThresholds() const {
     return (word0() >> s_veto3Bit) & s_veto3Mask;
   }
    
   unsigned int eFexTauRoI_v1::bcn4() const {
     return (word1() >> s_bcn4Bit) & s_bcn4Mask;
   }

   /// Return single 32-bit TOB word from a TOB or xTOB 
   uint32_t eFexTauRoI_v1::tobWord() const {
     // Do something sensible if called for a TOB
     if (etXTOB() == 0) return word0();
     // When called for xTOB
     else {
       uint32_t word = word0() + etTOB();
       return word;
     }
   }

   /// Tau condition value. 
   /// Note that this is for convenience & intelligibility, but should
   /// not be used for bitwise-accurate menu studies
   float eFexTauRoI_v1::fCore() const {
     if (fCoreDenominator() > 0) return (static_cast<float>(fCoreNumerator())/static_cast<float>(fCoreDenominator()));
     else         return 0.;
   }

   /// Methods that decode the eFEX number

  /// Return phi index in the range 0-63
  int eFexTauRoI_v1::iPhi() const {

    /// Calculate octant (0-7) from eFEX and shelf numbers
    unsigned int octant = int(eFexNumber()/3) + shelfNumber()*s_shelfPhiWidth;

    /// Find global phi index (0-63) for this window in this eFEX
    unsigned int index = s_eFexPhiWidth*octant + fpgaPhi() + s_eFexPhiOffset;
    if (index >= s_numPhi) index -= s_numPhi;

    return index;
  }

   /// Return an eta index in the range -25 -> +24
   /// Value corresponds to 10*lower eta edge of tower
   /// Note that this may not be the final format!
   /// And you need to combine with the seed() value to get full eta precision
   int eFexTauRoI_v1::iEta() const {

     /// With appropriate constants this should work in one line...
     int index = s_minEta + (eFexNumber()%3)*s_eFexEtaWidth + fpga()*s_fpgaEtaWidth + fpgaEta();

     /// Return value
     return index;

   }

  /// Return phi index in the range used by L1Topo (0->127)
  int eFexTauRoI_v1::iPhiTopo() const {

     /// Topo use pi/64 steps. Ours are pi/32, so we simply return 2* our integer index
     return iPhi()*2;

   }

   /// Return an eta index in the range used by L1Topo (-100->+99)
   int eFexTauRoI_v1::iEtaTopo() const {

     /// This returns e/g seed position as an integer index. 
     /// Value corresponds to 4*lower eta edge of supercell (so 0 means 0.0 -> 0.025) 
     return iEta()*4 + seed();

   }


} // namespace xAOD
