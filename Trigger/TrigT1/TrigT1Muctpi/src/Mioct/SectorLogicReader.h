// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: SectorLogicReader.h 364083 2011-05-06 09:09:55Z krasznaa $
#ifndef TRIGT1MUCTPI_SECTORLOGICREADER_H
#define TRIGT1MUCTPI_SECTORLOGICREADER_H

// STL include(s):
#include <list>
#include <string>

// Local include(s):
#include "EventReader.h"
#include "../Common/EventID.h"
#include "SectorLogicSource.h"

namespace LVL1MUCTPI {

   /**
    *  @short EventReader to read from the Sector Logic interface.
    *
    *         The SectorLogicReader Singleton tries to read an input 
    *         word for each registered sector from the interface class
    *         of the Muon Sector Logic simulation. If no input word is 
    *         found for a sector, the program terminates with a fatal
    *         error, as all inputs should always be present.
    *           
    *    @see EventReaderFactory
    *    @see EventReader
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 364083 $
    * $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
    */
   class SectorLogicReader : public EventReader {

   public:
      SectorLogicReader();
      virtual ~SectorLogicReader();
      /**
       * This routine returns true if all registered sectors are available
       * as input from the SectorLogic interface
       * @return true if the SectorLogicReader has been correctly installed
       *         and can be used.
       */
      virtual bool isValid() const { return m_validReader; }
      /**
       * After a call to isValid() this routine gives back a string which
       * contains the reason why the MifFileReader is not valid.
       * This can be for example the case if a sector has been registered
       * for which no corresponding input could be found.
       * @return string contains some text indicating the reason why
       *         the EventReader is not vaild.
       * @see EventReader::isValid()
       */
      virtual std::string getInfoString() const { return m_infoString; }
      void setEventSource ( EventSource & source );
      SectorLogicSource& getSource();
      /**
       * This functions stores the pointer to the sector which it needs to
       * update the sectors data field after an event-read.
       * @param newSector Pointer to a Sector which wants to be updated after
       *                  a new event has been read in.
       */
      virtual void registerSector( Sector* newSector );
      /**
       * This function is called in order to erase the pointer to a Sector
       * which is hold in the list of currently available sectors.
       * @param obsoleteSector is the pointer to the Sector to be erased from the
       *                       sector-list
       */
      virtual void unregisterSector( Sector* obsoleteSector );
      /**
       * The next event is read in for all Sectors.
       * @return If the read process fails to read data for a sector which
       *         is registered, false is returned.
       */
      virtual bool putNextEvent();

   private:
      /// a helper class for internal Sector administration
      class InputSector {

      public:
         /**
          * This functon is used by the unregister method of the sectors.
          * It is the predicate used to find an InputSector if the corresponding
          * pointer to the Sector is given.
          */
         class Sector_eq : 
            public std::unary_function< SectorLogicReader::InputSector* , bool > {

         public:
            Sector_eq( Sector* sectorToSearch) : 
               m_searchSector( sectorToSearch ) {}
            bool operator () ( const SectorLogicReader::InputSector* argumentSector ) {
               return ( argumentSector->getSectorPointer() == m_searchSector );
            }

         private:
            Sector* m_searchSector;
         }; // class Sector_eq

         /**
          * This inline function returns the Sector* pointer of this
          * InputSector
          */
         inline Sector* getSectorPointer() const { return m_sectorPointer; }
         /**
          * The constructor of the InputSector helper class gets the 
          * Sector pointer of the Sector in question and a pointer
          * to the event source. This information is used to resolve
          * where in the EventSource data for this Sector should be read
          */
         InputSector( Sector* theSector, SectorLogicSource & theSource );
         /**
          * method to refresh the data source for this sector in case
          * there is a new input object in each event
          */
         void getDataSource();
         /** 
          * method to get the data for this sector from the event source
          */
         void putEventData();
         /** 
          * destructor of the InputSector Helper class
          */
         ~InputSector();

      private:
         // private data members of the InputSector helper class
         Sector*                                  m_sectorPointer;
         const LVL1MUONIF::Lvl1MuSectorLogicData* m_sectorLogicPointer;
         SectorLogicSource *                      m_rememberTheSource;

      }; // class InputSector

      // private data members of the SectorLogicReader
      EventID                   m_eventID;
      bool                      m_validReader;
      bool                      m_registerFlag;
      bool                      m_eventSourceFlag;
      std::list< InputSector* > m_inputSectorList;
      SectorLogicSource         m_source;

   }; // class SectorLogicReader

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_SECTORLOGICREADER_H
