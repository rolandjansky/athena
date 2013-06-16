/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MioctID.cxx 364083 2011-05-06 09:09:55Z krasznaa $

// Local include(s):
#include "MioctID.h"
#include "../Common/SectorConstants.h"
#include "../Common/BitOp.h"

//***********************************************************************
//
//       Version : $Revision: 364083 $
//
//   Description : creates the Sectors because the Sector IDs are
//                 uniquely determined by the MioctID.
//
//        Author : $Author: krasznaa $
//          Date : $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
//
//
//
//***********************************************************************

namespace LVL1MUCTPI {

   MioctID::MioctID( const unsigned int aNumber, const Hemisphere aHemisphere )
      : m_rapidityRegion( aHemisphere ), m_number( aNumber ) {

   }

   // first MIOCT receives data from sectors 30,31,0,1, second
   // from 2,3,4,5, third from 6,7,8,9, ... and so on
   unsigned int MioctID::getFirstBarrelID() const {

      unsigned int result;
      if( m_number == 0 ) {
         result = 30;
      } else {
         result = m_number*4 - 2;
      }
      return result;
   }

   // First MIOCT receives data from sectors 47,0,1,2,3,4 second from
   // 5,6,7,8,9,10, ... and so on
   unsigned int MioctID::getFirstEndcapID() const {

      unsigned int result;
      if( m_number == 0 ) {
         result = 47;
      } else {
         result = m_number*6 - 1;
      }
      return result ;
   }

   // First MIOCT receives data from Sectors 0,1,2, second
   // from 3,4,5 ... and so on
   unsigned int MioctID::getFirstForwardID() const {

      return m_number*3;
   }

   unsigned int MioctID::getMirodSecAddress(unsigned int mioctSectorID) {

      unsigned int sectorAddress = 0;
      unsigned int sectorID = 0;

      // Determine if this is BARREL, ENDCAP, or FORWARD, and
      // translate the mioctSectorID into the subsystem sectorID
      // -> finally set the address word according to the MIORD
      // Eventreader standart
      if (mioctSectorID <= 3) {                                 // Barrel
         sectorID = mioctSectorID + (this)->getFirstBarrelID();
         if( sectorID > MAX_BARREL_SECTOR )
            sectorID = ( sectorID % MAX_BARREL_SECTOR ) - 1;
         if(m_rapidityRegion == POSITIVE) BitOp::setBit( &sectorAddress, 0 );
         BitOp::sImposeNBits( &sectorAddress, 1, sectorID );
      }
      else if (mioctSectorID > 3 && mioctSectorID <= 9) {     // Endcap
         sectorID = (mioctSectorID - 4) + (this)->getFirstEndcapID();
         if( sectorID > MAX_ENDCAP_SECTOR )
            sectorID = ( sectorID % MAX_ENDCAP_SECTOR ) - 1;
         BitOp::setBit( &sectorAddress, 7 );
         if( m_rapidityRegion == POSITIVE ) BitOp::setBit( &sectorAddress, 0 );
         BitOp::sImposeNBits( &sectorAddress, 1, sectorID );
      }
      else if (mioctSectorID > 9) {                             // Forward
         sectorID = ( mioctSectorID - 10 ) + (this)->getFirstForwardID();
         if( sectorID > MAX_FORWARD_SECTOR)
            sectorID = ( sectorID % MAX_FORWARD_SECTOR ) - 1;
         BitOp::setBit( &sectorAddress, 6 );
         if( m_rapidityRegion == POSITIVE ) BitOp::setBit( &sectorAddress, 0 );
         BitOp::sImposeNBits( &sectorAddress, 1, sectorID );
      }

      return sectorAddress;
   }

   bool operator< ( const MioctID& id1, const MioctID& id2 ) {

      if( id1.m_number != id2.m_number ) return ( id1.m_number < id2.m_number );
      else {
         if( id1.m_rapidityRegion != id2.m_rapidityRegion ) {
            if( id1.m_rapidityRegion == POSITIVE ) return false;
            else return true;
         } else {
            return false;
         }
      }

      return true;
   }

   MsgLogger& operator<<( MsgLogger& out, const MioctID& id ) {

      out << "MIOCT " << id.getNumber() << ", "
          << ( id.getRapidityRegion() == POSITIVE ? "POSITIVE" : "NEGATIVE" )
          << " Hemisphere";

      return out;
   }

} // namespace LVL1MUCTPI
