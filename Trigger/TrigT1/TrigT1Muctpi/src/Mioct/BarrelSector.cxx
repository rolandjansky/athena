/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BarrelSector.cxx 700318 2015-10-13 14:13:15Z wengler $

// STL include(s):
#include <iomanip>
#include <sstream>

// Local include(s):
#include "BarrelSector.h"
#include "../Common/SectorConstants.h"
#include "EventReader.h"

namespace LVL1MUCTPI {

   const unsigned int BarrelSector::MIN_SECTOR = MIN_BARREL_SECTOR;
   const unsigned int BarrelSector::MAX_SECTOR = MAX_BARREL_SECTOR;

   BarrelSector::BarrelSector( Hemisphere hemisphere, unsigned int sectorNumber,
                               EventReader* reader )
      : Sector( hemisphere, sectorNumber, reader ), m_logger( "BarrelSector" ) {

      if( m_sectorNumber > BarrelSector::MAX_SECTOR ) {
         m_sectorNumber = (sectorNumber % BarrelSector::MAX_SECTOR) - 1;
      }
      m_reader->registerSector( this );
      m_registered = true;
   }

   BarrelSector::BarrelSector( const BarrelSector& sector )
      : Sector( sector.m_rapidityRegion, sector.m_sectorNumber, sector.m_reader ),
        m_logger( "BarrelSector" ) {

    m_bitField = sector.m_bitField;
    m_registered = false;
  }

   BarrelSector::~BarrelSector() {

      if( m_registered ) m_reader->unregisterSector( this );
   }

   void BarrelSector::set( const unsigned int bitfield ) {

      // check if any of the reservered bits is set. This should not
      // be the case. If so, give a warning but set the bitfield anyway.
      // these bits are not always respected by the simulation inputs, so skip this 
      //      if( ( BarrelReservedMask & bitfield ) != 0 ) {
      //   m_logger << WARNING << "illegal bitfield : " << std::hex << bitfield
      //            << " (hex) --> Bitfield set anyway" << MsgLogger::endmsg;
      // }

      m_bitField = bitfield;
      return;

   }

   void BarrelSector::print( std::string& result ) const {

      int SecOf, ROI1Of, ROI2Of;

      SecOf  = this->getValue( SectorOverflowMask );
      ROI1Of = this->getValue( ROI1OverflowMask );
      ROI2Of = this->getValue( ROI2OverflowMask );

      std::string myId;
      this->printID( myId );

      std::ostringstream outStream;
      outStream << " \n\n  BARREL SECTOR DATA $Revision: 700318 $" << std::endl;
      outStream << "==============================================" << std::endl;
      outStream << myId << std::endl;
      outStream << "BCID :            " << getBCID() << std::endl;
      outStream << "Sector Overflow : " << SecOf << std::endl;
      outStream << std::endl;
      outStream << "          Candidate 1       Candidate 2 " << std::endl;
      outStream << "  ROI        " << getROI1()
                << "                  " << getROI2() << std::endl;
      outStream << "  Pt         " << getPtCand1()
                << "                  " << getPtCand2() << std::endl;
      outStream << "  ROIOF      " << ROI1Of
                << "                  " << ROI2Of << std::endl;
      outStream << " Barrel Ov   " << isCand1BarrelOverlap()
                << "                  " << isCand2BarrelOverlap() << std::endl;
      outStream << " Endcap Ov   " << isCand1EndcapOverlap()
                << "                  " << isCand2EndcapOverlap() << std::endl;
      outStream << std::endl;

      result = outStream.str();

      return;
   }

   ///////////////////////////////////////////////////////////////////////////
   // functions needed only to determine the overlap. They are
   // only called by other Barrel Sectors. May be worth declaring
   // friends...
   ///////////////////////////////////////////////////////////////////////////

   bool BarrelSector::isCand1BarrelOverlap() const {
      return static_cast< bool >( this->getValue( BarrelBarrelOverlap1Mask ) );
   }

   bool BarrelSector::isCand2BarrelOverlap() const {
      return static_cast< bool >( this->getValue( BarrelBarrelOverlap2Mask ) );
   }

   bool BarrelSector::isCand1EndcapOverlap() const {
      return static_cast< bool >( this->getValue( BarrelEndcapOverlap1Mask ) );
   }

   bool BarrelSector::isCand2EndcapOverlap() const {
      return static_cast< bool >( this->getValue( BarrelEndcapOverlap2Mask ) );
   }

   unsigned int BarrelSector::getROI1() const {
      return this->getValue( BarrelROI1Mask );
   }
   unsigned int BarrelSector::getROI2() const {
      return this->getValue( BarrelROI2Mask );
   }
  
   SectorID BarrelSector::getSectorID() const {
      return SectorID( BARREL, m_rapidityRegion, m_sectorNumber );
   }

} // namespace LVL1MUCTPI
