/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SectorLogicReader.cxx 364083 2011-05-06 09:09:55Z krasznaa $

// STL include(s):
#include <cassert>
#include <stdexcept>
#include <typeinfo>
#include <algorithm>

// TrigT1 include(s):
#include "TrigT1Interfaces/Lvl1MuSectorLogicData.h"

// Local include(s):
#include "SectorLogicReader.h"
#include "Sector.h"

namespace LVL1MUCTPI {

   // constructor of the SectorLogicReader
   SectorLogicReader::SectorLogicReader() : EventReader() {

      m_validReader = false;
      m_eventSourceFlag = false;
      m_registerFlag = false;
      m_infoString = "No Source set yet; no Sectors registered yet." ;
      m_logger.send( INFO, "SectorLogicReader installed as Event Reader" );
   }

   SectorLogicSource& SectorLogicReader::getSource() {

      return m_source;
   }

   void SectorLogicReader::setEventSource( EventSource& source ) {

      try {

         SectorLogicSource& theSectorLogicSource =
            dynamic_cast< SectorLogicSource& >( source );

         if( ! m_eventSourceFlag ) {

            m_source = theSectorLogicSource;
            m_eventSourceFlag = true;

            if( m_registerFlag ) {
               m_infoString = "valid Sector Logic Reader installed";
               m_validReader = true;
            } else {
               m_infoString = "no Sectors registered yet";
            }

         } else {
            REPORT_ERROR_MSG( "changing the EventSource of SectorLogicReader is "
                              "not supported" );
            m_logger << INFO << "continuing with previously installed "
                     << "EventSource"  << MsgLogger::endmsg;
         }

      } catch ( std::bad_cast ) {
         REPORT_ERROR_MSG( "The SectorLogicReader needs a SectorLogicSource as "
                           "EventSource !" );
         return;
      }

      return;
   }

   void SectorLogicReader::registerSector( Sector* newSector ) {

      InputSector* newInputSector = new InputSector( newSector, m_source );
      m_inputSectorList.push_back( newInputSector );
      m_registerFlag = true;
      if( m_eventSourceFlag ) {
         m_infoString = "valid SectorLogicReader installed";
         m_validReader = true;
      } else {
         m_infoString = "No Source set yet";
      }
      return;
  }

   void SectorLogicReader::unregisterSector( Sector* obsoleteSector ) {

      std::list< SectorLogicReader::InputSector* >::iterator it;
      it = std::find_if( m_inputSectorList.begin(), m_inputSectorList.end(),
                         SectorLogicReader::InputSector::Sector_eq( obsoleteSector ) );
      if( it != m_inputSectorList.end() ) {
         delete *it;
         m_inputSectorList.erase( it );
      } else {
         m_logger.send( WARNING,
                        "Trying to unregister a non-existent Sector. \n"
                        "Continue without having done anything...");
      }
      return;
   }

   bool SectorLogicReader::putNextEvent() {

      std::list< SectorLogicReader::InputSector* >::iterator it;
      for( it =  m_inputSectorList.begin(); it != m_inputSectorList.end(); ++it ) {

         // I have to cope with getting a new object each event here from
         // StoreGate - so need to re-get the address of the SectorLogicWord
         ( *it )->getDataSource();

         // put the event data
         ( *it )->putEventData();
      }

      // increase the Event ID by one
      m_eventID.incrementEvID();

      // there is no reason why this could go wrong --> return always true
      return true;
   }

   SectorLogicReader::~SectorLogicReader() {

      std::list< SectorLogicReader::InputSector* >::iterator it;
      for( it =  m_inputSectorList.begin(); it != m_inputSectorList.end(); ++it ) {
         delete ( *it );
      }
   }

   // contructor of the input sector helper class
   SectorLogicReader::InputSector::InputSector( Sector* theSector,
                                                SectorLogicSource& theSource ) {

      m_sectorPointer = theSector;
      m_rememberTheSource = &theSource;
      m_sectorLogicPointer = &theSource.getSectorLogicAddress(theSector);
   }

   // re-get data source since there might be a new object each event
   void SectorLogicReader::InputSector::getDataSource() {

      m_sectorLogicPointer = &m_rememberTheSource->getSectorLogicAddress( m_sectorPointer );
      return;
   }

   // get new data word for this sector from the input
   void SectorLogicReader::InputSector::putEventData() {

      m_sectorPointer->set( m_sectorLogicPointer->getWordFormat() );
      return;
   }

   // destructor of the input sector helper class
   SectorLogicReader::InputSector::~InputSector() {

   }

} // namespace LVL1MUCTPI
