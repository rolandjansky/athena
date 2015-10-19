/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ForwardSector.cxx 700318 2015-10-13 14:13:15Z wengler $

// STL include(s):
#include <iomanip>
#include <sstream>

// Local include(s):
#include "ForwardSector.h"
#include "EventReader.h"
#include "../Common/SectorConstants.h"

namespace LVL1MUCTPI {

   const unsigned int ForwardSector::MIN_SECTOR = MIN_FORWARD_SECTOR;
   const unsigned int ForwardSector::MAX_SECTOR = MAX_FORWARD_SECTOR;

   ForwardSector::ForwardSector( Hemisphere hemisphere, unsigned int sectorNumber,
                                 EventReader* reader )
      : Sector( hemisphere, sectorNumber, reader ), m_logger( "ForwardSector" ) {

      if( m_sectorNumber > ForwardSector::MAX_SECTOR ) {
         m_sectorNumber = ( sectorNumber % ForwardSector::MAX_SECTOR ) - 1;
      }

      m_reader->registerSector( this );
      m_registered = true;
   }

   ForwardSector::ForwardSector( const ForwardSector& sector )
      : Sector( sector.m_rapidityRegion, sector.m_sectorNumber, sector.m_reader ),
        m_logger( "ForwardSector" ) {

      m_bitField = sector.m_bitField;
      m_registered = false;
   }

   ForwardSector::~ForwardSector() {

      if( m_registered ) m_reader->unregisterSector( this );
   }

   void ForwardSector::set( const unsigned int bitfield ) {

      // check if any of the reservered bits is set. This should not
      // be the case. If so, give a warning but set the bitfield anyway.
      // these fields are not respected by input simultion - skip
      // if( ( ForwardReservedMask & bitfield ) != 0 ) {
      //   m_logger << WARNING << "illegal bitfield : " << std::hex << bitfield
      //            << " (hex) --> Bitfield set anyway" << MsgLogger::endmsg;
      //}

      m_bitField = bitfield;
      return;
   }

   unsigned int ForwardSector::getROI1() const {

      return getValue( ForwardROI1Mask );
   }

   unsigned int ForwardSector::getROI2() const {

      return getValue( ForwardROI2Mask );
   }

   void ForwardSector::print( std::string& result ) const {

      int BCID, SecOf, ROI1, ROI2, Pt1, Pt2, ROI1Of, ROI2Of;

      std::string myId;
      this->printID( myId );

      BCID   = this->getValue( BCIDMask );
      SecOf  = this->getValue( SectorOverflowMask );
      ROI1   = this->getROI1();
      ROI2   = this->getROI2();
      Pt1    = this->getValue( Pt1Mask );
      Pt2    = this->getValue( Pt2Mask );
      ROI1Of = this->getValue( ROI1OverflowMask );
      ROI2Of = this->getValue( ROI2OverflowMask );

      std::ostringstream outStream;
      outStream << " \n\n  FORWARD SECTOR DATA $Revision: 700318 $" << std::endl;
      outStream << "==============================================" << std::endl;
      outStream << myId << std::endl;
      outStream << "BCID :            " << BCID  << std::endl;
      outStream << "Sector Overflow : " << SecOf << std::endl;
      outStream << std::endl;
      outStream << "          Candidate 1       Candidate 2 " << std::endl;
      outStream << "  ROI        " << ROI1
                << "                  " << ROI2 << std::endl;
      outStream << "  Pt         " << Pt1
                << "                  " << Pt2 << std::endl;
      outStream << "  ROIOF      " << ROI1Of
                << "                  " << ROI2Of << std::endl;
      outStream << std::endl;

      result = outStream.str();
      return;
   }

   SectorID ForwardSector::getSectorID() const {

      return SectorID( FORWARD, m_rapidityRegion, m_sectorNumber );
   }

} // namespace LVL1MUCTPI
