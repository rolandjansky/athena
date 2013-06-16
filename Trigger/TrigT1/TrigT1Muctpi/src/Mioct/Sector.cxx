/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Sector.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// STL include(s):
#include <sstream>

// Local include(s):
#include "Sector.h"
#include "../Common/PtMultiplicitySet.h"
#include "../Common/SectorConstants.h"

namespace LVL1MUCTPI {

   Sector::Sector( const Hemisphere theHemisphere, const unsigned int theSectorNumber,
                   EventReader* reader )
      : m_registered( false ), m_rapidityRegion( theHemisphere ),
        m_sectorNumber( theSectorNumber ), m_reader( reader ),
        m_cand1Supressed( false ), m_cand2Supressed( false ) {

   }

   Sector::~Sector() {

   }


   unsigned int Sector::getValue( const BitMask mask ) const {

      unsigned int result;
      unsigned int maskcopy;

      // make a copy of the mask, because a mask is a mask and
      // one should stay a mask (i.e. should be something constant!)
      maskcopy = mask;
      result   = m_bitField & mask;
      if( mask != 0 ) {
         while( ( maskcopy & 0x00000001 ) == 0 ) {
            maskcopy =  maskcopy >> 1;
            result   =  result   >> 1;
         }
      }
      return result;

   }

   unsigned int Sector::getPtCand1() const {

      return this->getValue( Pt1Mask );

   }

   unsigned int Sector::getPtCand2() const {

      return this->getValue( Pt2Mask );

   }

   unsigned int Sector::getBCID() const {

      return this->getValue( BCIDMask );

   }

   std::string Sector::getRapidityString() const {

      std::string result;
      if( m_rapidityRegion == POSITIVE ) result = "+";
      if( m_rapidityRegion == NEGATIVE ) result = "-";
      return result;

   }

   void Sector::printID( std::string& IDString ) const {

      std::ostringstream outStream ;
      outStream << "Sector : " << m_sectorNumber
                << "  Detector : " << this->getDetectorString()
                << "  Rapidity region : " << this->getRapidityString();
      IDString = outStream.str();

      return;
   }

} // namespace LVL1MUCTPI
