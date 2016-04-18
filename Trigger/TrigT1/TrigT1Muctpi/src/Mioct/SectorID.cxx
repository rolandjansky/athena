/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SectorID.cxx 700318 2015-10-13 14:13:15Z wengler $

// STL include(s):
#include "SectorID.h"
#include "../Common/SectorConstants.h"

/*******************************************************************
 * $Date: 2015-10-13 16:13:15 +0200 (Tue, 13 Oct 2015) $
 *
 * Implementation of class SectorID
 * @author   Author: Thorsten Wengler
 * @version $Revision: 700318 $
 ******************************************************************/

namespace LVL1MUCTPI {

   /**
    * This comparison operator should implement a monotone ordering
    * between the SectorID objects.
    *
    * @param id1 First SectorID object
    * @param id2 Second SectorID object
    * @returns <code>true</code> if id1 is "smaller" than id2,
    *          <code>false</code> otherwise
    */
   bool operator< ( const SectorID& id1, const SectorID& id2 ) {

      if( id1.m_detectorType != id2.m_detectorType ) {
         return ( id1.m_detectorType < id2.m_detectorType );
      } else if( id1.m_rapidityRegion != id2.m_rapidityRegion  ) {
         return ( id1.m_rapidityRegion < id2.m_rapidityRegion );
      } else {
         return ( id1.m_sectorNumber < id2.m_sectorNumber );
      }

   }

   // constructor of class SectorID with no initialisation
   SectorID::SectorID()
      : m_detectorType( BARREL ), m_rapidityRegion( NEGATIVE ),
        m_sectorNumber( 0 ) {

   }

   SectorID::SectorID( int code ) {

      const int rapidity_border = ( MAX_BARREL_SECTOR + 1 ) +
         ( MAX_ENDCAP_SECTOR + 1 ) + ( MAX_FORWARD_SECTOR + 1 );

      if( code >= rapidity_border ) {
         code -= rapidity_border;
         m_rapidityRegion = POSITIVE;
      } else {
         m_rapidityRegion = NEGATIVE;
      }

      const int forward_border = ( MAX_BARREL_SECTOR + 1 ) +
         ( MAX_ENDCAP_SECTOR + 1 );
      const int endcap_border = MAX_BARREL_SECTOR + 1;

      if( code >= forward_border ) {
         code -= forward_border;
         m_detectorType = FORWARD;
      } else if( code >= endcap_border ) {
         code -= endcap_border;
         m_detectorType = ENDCAP;
      } else {
         m_detectorType = BARREL;
      }

      m_sectorNumber = code;

   }

   // constructor of class SectorID
   SectorID::SectorID( Detector theDetector, Hemisphere theRegion,
                       unsigned int theNumber )
      : m_detectorType( theDetector ), m_rapidityRegion( theRegion ),
        m_sectorNumber( theNumber ) {

   }

   // constructor of class SectorID taking numerical input values 
   // as used in Lvl1MuCTPIInput
   SectorID::SectorID( unsigned int theDetector, unsigned int theRegion,
                       unsigned int theNumber ) {

      this->setVal( theDetector, theRegion, theNumber );
   }

   // set method taking numerical input values
   // as used in Lvl1MuCTPIInput
   void SectorID::setVal( unsigned int theDetector, unsigned int theRegion,
                          unsigned int theNumber ) {

      if( theDetector == 0 ) m_detectorType = BARREL;
      if( theDetector == 1 ) m_detectorType = ENDCAP;
      if( theDetector == 2 ) m_detectorType = FORWARD;

      if( theRegion == 0 ) m_rapidityRegion = NEGATIVE;
      if( theRegion == 1 ) m_rapidityRegion = POSITIVE;

      m_sectorNumber = theNumber;
      return;

   }

   // set method taking numerical input values
   // as used in Lvl1MuCTPIInput
   SectorID SectorID::ret( unsigned int theDetector, unsigned int theRegion,
                           unsigned int theNumber ){

      this->setVal( theDetector, theRegion, theNumber );
      return *this;

   }

   // implementation of method getDetectorType
   unsigned int SectorID::getDetectorType() const {

      if( m_detectorType == BARREL )  return 0;
      if( m_detectorType == ENDCAP )  return 1;
      if( m_detectorType == FORWARD ) return 2;

      return 0;
   }

   // implementation of method getRapidityRegion
   unsigned int SectorID::getRapidityRegion() const {

      if( m_rapidityRegion == NEGATIVE ) return 0;
      if( m_rapidityRegion == POSITIVE ) return 1;

      return 0;
   }

   // comparison operator
   bool SectorID::operator==( const SectorID& original ) const {

      if ( ( this->m_detectorType == original.m_detectorType ) &&
           ( this->m_rapidityRegion == original.m_rapidityRegion ) &&
           ( this->m_sectorNumber == original.m_sectorNumber ) ) {
         return true;
      }
      return false;

   }

   // assignment operator
   SectorID& SectorID::operator=( const SectorID& original ) {

     if (this != &original) {
       this->m_detectorType = original.m_detectorType;
       this->m_rapidityRegion = original.m_rapidityRegion;
       this->m_sectorNumber = original.m_sectorNumber;
     }
     return *this;

   }

   SectorID::operator int() const {

      int retval = 0;

      if( m_rapidityRegion == POSITIVE ) retval += ( MAX_BARREL_SECTOR + 1 ) +
                                            ( MAX_ENDCAP_SECTOR + 1 ) +
                                            ( MAX_FORWARD_SECTOR + 1 );

      if( m_detectorType == ENDCAP ) retval += MAX_BARREL_SECTOR + 1;
      else if( m_detectorType == FORWARD ) retval += ( MAX_BARREL_SECTOR + 1 ) +
                                              ( MAX_ENDCAP_SECTOR + 1 );

      retval += m_sectorNumber;

      return retval;

   }

   // overload for <<
   std::ostream & operator<<( std::ostream& out, const SectorID& right ) {

      return out << "System: " << right.m_detectorType << " Hemisphere: "
                 << right.m_rapidityRegion << " SectorNo: " << right.m_sectorNumber;

   }

   // destructor of class SectorID
   SectorID::~SectorID() {

   }

} // namespace LVL1MUCTPI
