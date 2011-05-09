/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Muon_ROI.cxx,v 1.2 2008-05-08 15:00:12 krasznaa Exp $

// Local include(s):
#include "AnalysisTriggerEvent/Muon_ROI.h"

/**
 * The constructor just initialises all the member variables to the values given to it.
 */
Muon_ROI::Muon_ROI( uint32_t roiWord, double eta, double phi, const std::string &thrName,
                    double thrValue )
   : m_roiWord( roiWord ), m_eta( eta ), m_phi( phi ), m_thrName( thrName ),
     m_thrValue( thrValue ) {

}

/**
 * The constructor initialises all member variables to dummy initial values
 */
Muon_ROI::Muon_ROI()
   : m_roiWord( 0 ), m_eta( 0. ), m_phi( 0. ), m_thrName( "" ), m_thrValue( 0. ) {

}

/**
 * The destructor is virtual, since the base classes have virtual
 * functions...
 */
Muon_ROI::~Muon_ROI() {

}

/**
 * Each detected LVL1 muon candidate is assigned a p<sub>T</sub> threshold.
 * The hardware can assign one of 6 p<sub>T</sub> thresholds to the candidate,
 * which are numbered from 1 to 6.
 *
 * @return Integer number in the [1, 6] range
 */
int Muon_ROI::getThrNumber() const {

   return ( ( m_roiWord >> 11 ) & 0x7 );
}

/**
 * A muon's spacial location is identified from the sector number and the hemisphere
 * that detected it. Each sector is additionally divided into multiple areas,
 * called <i>RoIs</i> that tell you which part of the sector the muon candidate
 * was detected in.
 *
 * @return Integer number smaller than 147
 */
int Muon_ROI::getRoI() const {

   if( this->getSource() == Forward ) {
      return ( ( m_roiWord >> 2 ) & 0x3f );
   } else if( this->getSource() == Endcap ) {
      return ( ( m_roiWord >> 2 ) & 0xff );
   } else if( this->getSource() == Barrel ) {
      return ( ( m_roiWord >> 2 ) & 0x1f );
   }

   return 0;
}

/**
 * The sector address is an 8-bit identifier of the sector. For its detailed
 * description, see page 38 of https://edms.cern.ch/file/248757/1/mirod.pdf
 *
 * The least significant bit shown which hemisphere the sector is in
 * (0: positive side, 1: negative side), the upper (1 or 2) bits show
 * what kind of sector it is, the rest of the address is the number of the
 * sector.
 *
 * @return An 8-bit identifier
 */
int Muon_ROI::getSectorAddress() const {

   return ( ( m_roiWord >> 14 ) & 0xff );
}

/**
 * Each muon trigger sector can only send information about a maximum of
 * two LVL1 muon candidate to the central trigger. If this flag is
 * <code>true</code>, this candidate had the highest p<sub>T</sub> threshold
 * assigned to it in its sector. If it's <code>false</code>, it was the
 * <i>second candidate</i> in its sector.
 */
bool Muon_ROI::isFirstCandidate() const {

   return ( ( m_roiWord >> 22 ) & 0x1 );
}

/**
 * One RoI (one part of the trigger sector) can only send information about
 * one LVL1 muon candidate to the central trigger. This flag is
 * <code>true</code> if the particular RoI that detected this muon candidate
 * also detected another candidate with lower p<sub>T</sub>.
 */
bool Muon_ROI::isMoreCandInRoI() const {

   return ( ( m_roiWord >> 1 ) & 0x1 );
}

/**
 * This flag is set to <code>true</code> if the sector that this muon candidate
 * came from, detected more than 2 LVL1 muon candidates.
 *
 * @see Muon_ROI::isFirstCandidate()
 */
bool Muon_ROI::isMoreCandInSector() const {

   return ( m_roiWord & 0x1 );
}

/**
 * The function decodes the sector type encoded in the 8-bit sector address
 * into an easy to understand enumeration.
 *
 * @see Muon_ROI::getSectorAddress()
 * @see Muon_ROI::RoISource
 */
Muon_ROI::RoISource Muon_ROI::getSource() const {

   if( this->getSectorAddress() & 0x80 ) {
      return Endcap;
   } else if( this->getSectorAddress() & 0x40 ) {
      return Forward;
   } else {
      return Barrel;
   }
}

/**
 * The function decodes the sector hemisphere encoded in the 8-bit sector address
 * into an easy to understand enumeration.
 *
 * @see Muon_ROI::getSectorAddress()
 * @see Muon_ROI::Hemisphere
 */
Muon_ROI::Hemisphere Muon_ROI::getHemisphere() const {

   if( this->getSectorAddress() & 0x1 ) {
      return Positive;
   } else {
      return Negative;
   }
}

/**
 * Endcap and forward sectors can tell you what was the charge of the muon candidate.
 * Barrel candidates can't do this.
 *
 * @see Muon_ROI::RoISource
 * @see Muon_ROI::getSource()
 */
Muon_ROI::Charge Muon_ROI::getCharge() const {

   if( getSource() == Barrel ) return Undef;
   if( m_roiWord & 0x8000000 ) {
      return Pos;
   } else {
      return Neg;
   }
}

/**
 * When the overlap handling is activated in the MuCTPI, candidates can be ignored
 * in the multiplicity sum sent to the CTP. This flag tells you whether this
 * particular candidate was ignored in the multiplicity sum.
 */
bool Muon_ROI::isVetoed() const {

   return ( m_roiWord & 0x10000000 );
}
