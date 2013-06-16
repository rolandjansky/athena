/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MioctSectorSet.cxx 472835 2011-12-06 21:05:16Z stelzer $

// STL include(s):
#include <string>

// Local include(s):
#include "MioctSectorSet.h"
#include "EventReader.h"

//******************************************************************
//
//       Version : $Revision: 472835 $
//
//   Description : This is a helper class which groups the different
//                 Sectors of one Mioct  Module.  It only serves  to
//                 guarantee that the Sectors in the Set have the
//                 right IDs so that they overlap. The class has
//                 been formed in order  to give the Sectors  in a
//                 compact  form to the various OverlapStrategies.
//
//        Author : $Author: krasznaa $
//          Date : $Date: 2011-12-06 22:05:16 +0100 (Tue, 06 Dec 2011) $
//
//
//
//******************************************************************

namespace LVL1MUCTPI {

   MioctSectorSet::MioctSectorSet( const MioctID& theID, EventReader* reader )
      : Barrel1( theID.getRapidityRegion(), theID.getFirstBarrelID() + 0, reader ),
        Barrel2( theID.getRapidityRegion(), theID.getFirstBarrelID() + 1, reader ),
        Barrel3( theID.getRapidityRegion(), theID.getFirstBarrelID() + 2, reader ),
        Barrel4( theID.getRapidityRegion(), theID.getFirstBarrelID() + 3, reader ),
        Endcap1( theID.getRapidityRegion(), theID.getFirstEndcapID() + 0, reader ),
        Endcap2( theID.getRapidityRegion(), theID.getFirstEndcapID() + 1, reader ),
        Endcap3( theID.getRapidityRegion(), theID.getFirstEndcapID() + 2, reader ),
        Endcap4( theID.getRapidityRegion(), theID.getFirstEndcapID() + 3, reader ),
        Endcap5( theID.getRapidityRegion(), theID.getFirstEndcapID() + 4, reader ),
        Endcap6( theID.getRapidityRegion(), theID.getFirstEndcapID() + 5, reader ),
        Forward1( theID.getRapidityRegion(), theID.getFirstForwardID() + 0, reader ),
        Forward2( theID.getRapidityRegion(), theID.getFirstForwardID() + 1, reader ),
        Forward3( theID.getRapidityRegion(), theID.getFirstForwardID() + 2, reader ),
        m_logger( "MioctSectorSet" ), m_id( theID ) {

      m_bitFieldList.push_back( Forward1.getBitFieldPtr() );
      m_bitFieldList.push_back( Forward2.getBitFieldPtr() );
      m_bitFieldList.push_back( Forward3.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap4.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap5.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap6.getBitFieldPtr() );
      m_bitFieldList.push_back( Barrel1.getBitFieldPtr() );
      m_bitFieldList.push_back( Barrel2.getBitFieldPtr() );
      m_bitFieldList.push_back( Barrel3.getBitFieldPtr() );
      m_bitFieldList.push_back( Barrel4.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap1.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap2.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap3.getBitFieldPtr() );
   }

   MioctSectorSet::MioctSectorSet( const MioctSectorSet& set )
      : Barrel1( set.Barrel1 ), Barrel2( set.Barrel2 ), Barrel3( set.Barrel3 ),
        Barrel4( set.Barrel4 ),
        Endcap1( set.Endcap1 ), Endcap2( set.Endcap2 ), Endcap3( set.Endcap3 ),
        Endcap4( set.Endcap4 ),
        Endcap5( set.Endcap5 ), Endcap6( set.Endcap6 ), Forward1( set.Forward1 ),
        Forward2( set.Forward2 ), Forward3( set.Forward3 ),
        m_logger( "MioctSectorSet" ), m_id( set.m_id ) {

      m_bitFieldList.push_back( Forward1.getBitFieldPtr() );
      m_bitFieldList.push_back( Forward2.getBitFieldPtr() );
      m_bitFieldList.push_back( Forward3.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap4.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap5.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap6.getBitFieldPtr() );
      m_bitFieldList.push_back( Barrel1.getBitFieldPtr() );
      m_bitFieldList.push_back( Barrel2.getBitFieldPtr() );
      m_bitFieldList.push_back( Barrel3.getBitFieldPtr() );
      m_bitFieldList.push_back( Barrel4.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap1.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap2.getBitFieldPtr() );
      m_bitFieldList.push_back( Endcap3.getBitFieldPtr() );
   }

   void MioctSectorSet::report() const {

      std::string sector_desc;

      REPORT_VERBOSE_MSG( "Printing sectors in MioctSectorSet:" );

      Barrel1.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Barrel1: " + sector_desc );
      Barrel2.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Barrel2: " + sector_desc );
      Barrel3.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Barrel3: " + sector_desc );
      Barrel4.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Barrel4: " + sector_desc );

      Endcap1.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Endcap1: " + sector_desc );
      Endcap2.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Endcap2: " + sector_desc );
      Endcap3.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Endcap3: " + sector_desc );
      Endcap4.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Endcap4: " + sector_desc );
      Endcap5.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Endcap5: " + sector_desc );
      Endcap6.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Endcap6: " + sector_desc );

      Forward1.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Forward1: " + sector_desc );
      Forward2.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Forward2: " + sector_desc );
      Forward3.printID( sector_desc );
      REPORT_VERBOSE_MSG( "  Forward3: " + sector_desc );

      return;
   }

   void MioctSectorSet::print() const {

      std::string sector_data;

      REPORT_VERBOSE_MSG( "Sector contents for " << m_id );

      Barrel1.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Barrel1 --" << sector_data );
      Barrel2.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Barrel2 --" << sector_data );
      Barrel3.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Barrel3 --" << sector_data );
      Barrel4.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Barrel4 --" << sector_data );

      Endcap1.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Endcap1 --" << sector_data );
      Endcap2.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Endcap2 --" << sector_data );
      Endcap3.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Endcap3 --" << sector_data );
      Endcap4.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Endcap4 --" << sector_data );
      Endcap5.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Endcap5 --" << sector_data );
      Endcap6.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Endcap6 --" << sector_data );

      Forward1.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Forward1 --" << sector_data );
      Forward2.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Forward2 --" << sector_data );
      Forward3.print( sector_data );
      REPORT_VERBOSE_MSG( "  -- Forward3 --" << sector_data );

      return;
   }

} // namespace LVL1MUCTPI
