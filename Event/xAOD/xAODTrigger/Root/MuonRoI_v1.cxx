/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonRoI_v1.cxx 579914 2014-01-24 10:15:15Z krasznaa $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/MuonRoI_v1.h"

// get bitsmasks from common definition source:
#include "TrigT1MuctpiBits/MuCTPI_Bits.h"
using namespace LVL1::MuCTPIBits;

namespace xAOD{

   MuonRoI_v1::MuonRoI_v1()
      : SG::AuxElement() {

   }

   /// It's just a convenience function, forwarding the call to all the
   /// individual property setter functions.
   ///
   /// @param roiword The 32-bit RoI word coming from the MuCTPI
   /// @param eta The pseudorapidity of the candidate
   /// @param phi The aximuthal angle of the candidate
   /// @param thrname The name of the highest threshold passed by the candidate
   /// @param thrvalue The value (in MeV) of the threshold passed
   ///
   void MuonRoI_v1::initialize( uint32_t roiword, float eta, float phi,
                                const std::string& thrname, float thrvalue ) {

      setRoIWord( roiword );
      setEta( eta );
      setPhi( phi );
      setThrValue( thrvalue );
      setThrName( thrname );

      return;
   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //                    Raw/simple data accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MuonRoI_v1, float, eta, setEta )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MuonRoI_v1, float, phi, setPhi )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MuonRoI_v1, uint32_t, roiWord,
                                         setRoIWord )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MuonRoI_v1, float, thrValue,
                                         setThrValue )


   AUXSTORE_OBJECT_SETTER_AND_GETTER( MuonRoI_v1, std::string, thrName,
                                      setThrName )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //             Implementation of the RoI word decoder functions
   //

   /// Each detected LVL1 muon candidate is assigned a p<sub>T</sub> threshold.
   /// The hardware can assign one of 6 p<sub>T</sub> thresholds to the
   /// candidate, which are numbered from 1 to 6.
   ///
   /// @return Integer number in the [1, 6] range
   ///
   int MuonRoI_v1::getThrNumber() const {

     if (isRun3()) return ( ( roiWord() >> RUN3_CAND_PT_SHIFT ) & RUN3_CAND_PT_MASK );
     else return ( ( roiWord() >> CAND_PT_SHIFT ) & CAND_PT_MASK );
   }

   /// A muon's spacial location is identified from the sector number and the
   /// hemisphere that detected it. Each sector is additionally divided into
   /// multiple areas, called <i>RoIs</i> that tell you which part of the
   /// sector the muon candidate was detected in.
   ///
   /// @return Integer number smaller than 147
   ///
   int MuonRoI_v1::getRoI() const {

     if (isRun3()) {
       if( this->getSource() == Forward ) {
	 return ( ( roiWord() >> RUN3_ROI_SHIFT ) & FORWARD_ROI_MASK );
       } else if( this->getSource() == Endcap ) {
	 return ( ( roiWord() >> RUN3_ROI_SHIFT ) & ENDCAP_ROI_MASK );
       } else if( this->getSource() == Barrel ) {
	 return ( ( roiWord() >> RUN3_ROI_SHIFT ) & BARREL_ROI_MASK );
       }
     }
     else
     {
       if( this->getSource() == Forward ) {
	 return ( ( roiWord() >> ROI_SHIFT ) & FORWARD_ROI_MASK );
       } else if( this->getSource() == Endcap ) {
	 return ( ( roiWord() >> ROI_SHIFT ) & ENDCAP_ROI_MASK );
       } else if( this->getSource() == Barrel ) {
	 return ( ( roiWord() >> ROI_SHIFT ) & BARREL_ROI_MASK );
       }
     }
     return 0;
   }

   /// The sector address is an 8-bit identifier of the sector. For its detailed
   /// description, see page 38 of https://edms.cern.ch/file/248757/1/mirod.pdf
   ///
   /// The least significant bit shown which hemisphere the sector is in
   /// (0: positive side, 1: negative side), the upper (1 or 2) bits show
   /// what kind of sector it is, the rest of the address is the number of the
   /// sector.
   ///
   /// @return An 8-bit identifier
   ///
   int MuonRoI_v1::getSectorAddress() const {

     if (isRun3()) return ( ( roiWord() >> RUN3_CAND_SECTOR_ADDRESS_SHIFT ) & CAND_SECTOR_ADDRESS_MASK );
     else return ( ( roiWord() >> CAND_SECTOR_ADDRESS_SHIFT ) & CAND_SECTOR_ADDRESS_MASK );
   }

   /// The sector ID is a 5- or 6-bit identifier of the sector number. For its detailed
   /// description, see page 38 of https://edms.cern.ch/file/248757/1/mirod.pdf
   ///
   /// @return A 5- or 6-bit identifier
   ///
   int MuonRoI_v1::getSectorID() const {

     if (isRun3()) {
       if( this->getSource() == Forward ) {
	 return ( ( roiWord() >> RUN3OFFLINE_CAND_SECTORID_SHIFT ) & FORWARD_SECTORID_MASK );
       } else if( this->getSource() == Endcap ) {
	 return ( ( roiWord() >> RUN3OFFLINE_CAND_SECTORID_SHIFT ) & ENDCAP_SECTORID_MASK );
       } else if( this->getSource() == Barrel ) {
	 return ( ( roiWord() >> RUN3OFFLINE_CAND_SECTORID_SHIFT ) & BARREL_SECTORID_MASK );
       }
     } else {
       if( this->getSource() == Forward ) {
	 return ( ( roiWord() >> CAND_SECTORID_SHIFT ) & FORWARD_SECTORID_MASK );
       } else if( this->getSource() == Endcap ) {
	 return ( ( roiWord() >> CAND_SECTORID_SHIFT ) & ENDCAP_SECTORID_MASK );
       } else if( this->getSource() == Barrel ) {
	 return ( ( roiWord() >> CAND_SECTORID_SHIFT ) & BARREL_SECTORID_MASK );
       }
     }
     return 0;
   }

   /// Each muon trigger sector can only send information about a maximum of
   /// two LVL1 muon candidate to the central trigger. If this flag is
   /// <code>true</code>, this candidate had the highest p<sub>T</sub> threshold
   /// assigned to it in its sector. If it's <code>false</code>, it was the
   /// <i>second candidate</i> in its sector.
   ///
   /// actually v1 only ... 
   bool MuonRoI_v1::isFirstCandidate() const {

     if (isRun3()) return true; // undefined in run3, return default true
     else return ( ( roiWord() >> CAND_HIGHEST_PT_SHIFT ) & CAND_HIGHEST_PT_MASK );
   }

   /// One RoI (one part of the trigger sector) can only send information about
   /// one LVL1 muon candidate to the central trigger. This flag is
   /// <code>true</code> if the particular RoI that detected this muon candidate
   /// also detected another candidate with lower p<sub>T</sub>.
   ///
   bool MuonRoI_v1::isMoreCandInRoI() const {

     if (isRun3()) {
       if (getSource() == Barrel) return ( ( roiWord() >> RUN3_ROI_OVERFLOW_SHIFT ) & ROI_OVERFLOW_MASK );
       else return false; // Endcap + Fwd have no flag for this
     } else return ( ( roiWord() >> ROI_OVERFLOW_SHIFT ) & ROI_OVERFLOW_MASK );
   }

   /// This flag is set to <code>true</code> if the sector that this muon
   /// candidate came from, detected more than 2 LVL1 muon candidates.
   ///
   /// @see MuonRoI_v1::isFirstCandidate()
   ///
   bool MuonRoI_v1::isMoreCandInSector() const {

     if (isRun3()) return ( ( roiWord() >> RUN3_CAND_OVERFLOW_SHIFT ) & CAND_OVERFLOW_MASK );
     else return ( ( roiWord() >> CAND_OVERFLOW_SHIFT ) & CAND_OVERFLOW_MASK );
   }

   /// The function decodes the sector type encoded in the 8-bit sector address
   /// into an easy to understand enumeration.
   ///
   /// @see MuonRoI_v1::getSectorAddress()
   /// @see MuonRoI_v1::RoISource
   ///
   MuonRoI_v1::RoISource MuonRoI_v1::getSource() const {

     //same mask for run2 and run3
     if( this->getSectorAddress() & ENDCAP_ADDRESS_MASK ) {
         return Endcap;
     } else if( this->getSectorAddress() & FORWARD_ADDRESS_MASK ) {
         return Forward;
      } else {
         return Barrel;
      }
   }

   /// The function decodes the sector hemisphere encoded in the 8-bit sector
   /// address into an easy to understand enumeration.
   ///
   /// @see MuonRoI_v1::getSectorAddress()
   /// @see MuonRoI_v1::Hemisphere
   ///
   MuonRoI_v1::Hemisphere MuonRoI_v1::getHemisphere() const {

     //same mask for run2 and run3
     if( this->getSectorAddress() & SECTOR_HEMISPHERE_MASK ) {
         return Positive;
      } else {
         return Negative;
      }
   }

   /// Get whether or not there was a phi overlap between barrel sectors at SL-level.
   /// This is different from the overlap removal performed in the MUCTPI.
   /// Valid for Run-2 and Run-3 RPC candidates only
   ///
   bool MuonRoI_v1::getPhiOverlap() const {
     if (isRun3()) {
       if (getSource() == Barrel) return (roiWord() >> RUN3_BARREL_OL_SHIFT) & RUN3_BARREL_OL_MASK;
       else return false;
     } else {
       if (getSource() == Barrel) return (roiWord() >> BARREL_OL_SHIFT) & BARREL_PHI_OL_MASK;
       else return false;
     }
   }

   /// Get whether or not there was a eta overlap between barrel and endcap sectors at SL-level.
   /// This is different from the overlap removal performed in the MUCTPI.
   /// Valid for Run-2 RPC/TGC candidates only
   ///
   bool MuonRoI_v1::getEtaOverlap() const {
     if (isRun3()) return false;
     else {
       if (getSource() == Barrel) return (roiWord() >> BARREL_OL_SHIFT) & BARREL_ETA_OL_MASK;
       else return (roiWord() >> ENDCAP_OL_SHIFT) & ENDCAP_OL_MASK;
     }
   }

   /// Endcap and forward sectors can tell you what was the charge of the muon
   /// candidate. Barrel candidates can't do this.
   ///
   /// @see MuonRoI_v1::RoISource
   /// @see MuonRoI_v1::getSource()
   ///
   MuonRoI_v1::Charge MuonRoI_v1::getCharge() const {

      if( getSource() == Barrel ) return Undef;

      if (isRun3()) {
	if( ( roiWord() >> RUN3_CAND_TGC_CHARGE_SIGN_SHIFT) & 0x1 ) {
	  return Pos;
	} else {
	  return Neg;
	}
      } else {
	if( ( roiWord() >> CAND_TGC_CHARGE_SIGN_SHIFT) & 0x1 ) {
	  return Pos;
	} else {
	  return Neg;
	}
      }
   }

   /// Get whether or not there was a 3-station coincidence in the TGC.
   /// Valid for Run-3 candidates only.
   ///
   bool MuonRoI_v1::getBW3Coincidence() const {
     if (isRun3() && getSource() != Barrel) return (roiWord() >> RUN3_CAND_TGC_BW2OR3_SHIFT) & 0x1;
     else return false;
   }


   /// Get whether or not there was an inner coincidence with the TGC.
   /// Valid for Run-3 candidates only.
   ///
   bool MuonRoI_v1::getInnerCoincidence() const {
     if (isRun3() && getSource() != Barrel) return (roiWord() >> RUN3_CAND_TGC_INNERCOIN_SHIFT) & 0x1;
     else return false;
   }


   /// Get whether or not there was a good magnetic field quality in the TGC.
   /// Valid for Run-3 candidates only.
   ///
   bool MuonRoI_v1::getGoodMF() const {
     if (isRun3() && getSource() != Barrel) return (roiWord() >> RUN3_CAND_TGC_GOODMF_SHIFT) & 0x1;
     else return false;
   }

   /// When the overlap handling is activated in the MuCTPI, candidates can be
   /// ignored in the multiplicity sum sent to the CTP. This flag tells you
   /// whether this particular candidate was ignored in the multiplicity sum.
   ///
   bool MuonRoI_v1::isVetoed() const {

     if (isRun3()) return ( ( roiWord() >> RUN3_CAND_VETO_SHIFT) & 0x1 );
     else return ( ( roiWord() >> CAND_VETO_SHIFT) & 0x1 );
   }

   /// An extra bit is added at the end of the RoI word for run3 candidates
   /// in the EDM for technical purposes to distinguish whether we want to use
   /// the run2 or run3 bitmasks in decoding the word
   bool MuonRoI_v1::isRun3() const {

      return ( roiWord() >> 31 & 0x1 );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD
