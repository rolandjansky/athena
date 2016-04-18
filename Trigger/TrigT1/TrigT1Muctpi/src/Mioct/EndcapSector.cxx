/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EndcapSector.cxx 700318 2015-10-13 14:13:15Z wengler $

// STL include(s):
#include <iomanip>
#include <sstream>

// Local include(s):
#include "EndcapSector.h"
#include "EventReader.h"
#include "../Common/MuctpiBitMasks.h"
#include "../Common/SectorConstants.h"

namespace LVL1MUCTPI {

   const unsigned int EndcapSector::MIN_SECTOR = MIN_ENDCAP_SECTOR;
   const unsigned int EndcapSector::MAX_SECTOR = MAX_ENDCAP_SECTOR;

   EndcapSector::EndcapSector( Hemisphere hemisphere, unsigned int sectorNumber,
                               EventReader* reader )
      : Sector( hemisphere, sectorNumber, reader ), m_logger( "EndcapSector" ) {

      if ( m_sectorNumber > EndcapSector::MAX_SECTOR ) {
         m_sectorNumber = ( sectorNumber % EndcapSector::MAX_SECTOR ) - 1;
      }

      m_reader->registerSector( this );
      m_registered = true;
   }

   EndcapSector::EndcapSector( const EndcapSector& sector )
      : Sector( sector.m_rapidityRegion, sector.m_sectorNumber, sector.m_reader ),
        m_logger( "EndcapSector" ) {

      m_bitField = sector.m_bitField;
      m_registered = false;
   }

   EndcapSector::~EndcapSector() {

      if( m_registered ) m_reader->unregisterSector( this );
   }

   void EndcapSector::set( const unsigned int bitfield ) {

      // check if any of the reservered bits is set. This should not
      // be the case. If so, give a warning but set the bitfield anyway.
      // these fields are not respected by input simluations, skip
      //      if( ( EndcapReservedMask & bitfield ) != 0 ) {
      //  m_logger << WARNING << "illegal bitfield : " << std::hex << bitfield
      //           << " (hex) --> Bitfield set anyway" << MsgLogger::endmsg;
      //}

      m_bitField = bitfield;
      return;
   }

   bool EndcapSector::isCand1BarrelOverlap() const {
      return static_cast< bool >( getValue( EndcapBarrelOverlap1Mask ) );
   }

   bool EndcapSector::isCand2BarrelOverlap() const {
      return static_cast< bool >( getValue( EndcapBarrelOverlap2Mask ) );
   }

   unsigned int EndcapSector::getROI1() const {
      return getValue( EndcapROI1Mask );
   }
   unsigned int EndcapSector::getROI2() const {
      return getValue( EndcapROI2Mask );
   }

   void EndcapSector::print( std::string& result ) const {

      int BCID, SecOf, ROI1, ROI2, Pt1, Pt2, ROI1Of, ROI2Of, Ba1Ov, Ba2Ov;

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
      Ba1Ov  = this->isCand1BarrelOverlap();
      Ba2Ov  = this->isCand2BarrelOverlap();

      std::ostringstream outStream;
      outStream << " \n\n  ENDCAP SECTOR DATA $Revision: 700318 $" << std::endl;
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
      outStream << " Barrel Ov   " << Ba1Ov
                << "                  " << Ba2Ov << std::endl;
      outStream << std::endl;

      result = outStream.str();
      return;

   }
  
   SectorID EndcapSector::getSectorID() const {

      return SectorID( ENDCAP, m_rapidityRegion, m_sectorNumber );

   }

} // namespace LVL1MUCTPI
