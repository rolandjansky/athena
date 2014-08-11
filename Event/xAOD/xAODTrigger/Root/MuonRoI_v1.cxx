/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonRoI_v1.cxx 579914 2014-01-24 10:15:15Z krasznaa $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODTrigger/versions/MuonRoI_v1.h"

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

      return ( ( roiWord() >> 11 ) & 0x7 );
   }

   /// A muon's spacial location is identified from the sector number and the
   /// hemisphere that detected it. Each sector is additionally divided into
   /// multiple areas, called <i>RoIs</i> that tell you which part of the
   /// sector the muon candidate was detected in.
   ///
   /// @return Integer number smaller than 147
   ///
   int MuonRoI_v1::getRoI() const {

      if( this->getSource() == Forward ) {
         return ( ( roiWord() >> 2 ) & 0x3f );
      } else if( this->getSource() == Endcap ) {
         return ( ( roiWord() >> 2 ) & 0xff );
      } else if( this->getSource() == Barrel ) {
         return ( ( roiWord() >> 2 ) & 0x1f );
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

      return ( ( roiWord() >> 14 ) & 0xff );
   }

   /// Each muon trigger sector can only send information about a maximum of
   /// two LVL1 muon candidate to the central trigger. If this flag is
   /// <code>true</code>, this candidate had the highest p<sub>T</sub> threshold
   /// assigned to it in its sector. If it's <code>false</code>, it was the
   /// <i>second candidate</i> in its sector.
   ///
   bool MuonRoI_v1::isFirstCandidate() const {

      return ( ( roiWord() >> 22 ) & 0x1 );
   }

   /// One RoI (one part of the trigger sector) can only send information about
   /// one LVL1 muon candidate to the central trigger. This flag is
   /// <code>true</code> if the particular RoI that detected this muon candidate
   /// also detected another candidate with lower p<sub>T</sub>.
   ///
   bool MuonRoI_v1::isMoreCandInRoI() const {

      return ( ( roiWord() >> 1 ) & 0x1 );
   }

   /// This flag is set to <code>true</code> if the sector that this muon
   /// candidate came from, detected more than 2 LVL1 muon candidates.
   ///
   /// @see MuonRoI_v1::isFirstCandidate()
   ///
   bool MuonRoI_v1::isMoreCandInSector() const {

      return ( roiWord() & 0x1 );
   }

   /// The function decodes the sector type encoded in the 8-bit sector address
   /// into an easy to understand enumeration.
   ///
   /// @see MuonRoI_v1::getSectorAddress()
   /// @see MuonRoI_v1::RoISource
   ///
   MuonRoI_v1::RoISource MuonRoI_v1::getSource() const {

      if( this->getSectorAddress() & 0x80 ) {
         return Endcap;
      } else if( this->getSectorAddress() & 0x40 ) {
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

      if( this->getSectorAddress() & 0x1 ) {
         return Positive;
      } else {
         return Negative;
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

      if( roiWord() & 0x8000000 ) {
         return Pos;
      } else {
         return Neg;
      }
   }

   /// When the overlap handling is activated in the MuCTPI, candidates can be
   /// ignored in the multiplicity sum sent to the CTP. This flag tells you
   /// whether this particular candidate was ignored in the multiplicity sum.
   ///
   bool MuonRoI_v1::isVetoed() const {

      return ( roiWord() & 0x10000000 );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD
