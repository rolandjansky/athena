// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventReader.h 364083 2011-05-06 09:09:55Z krasznaa $
#ifndef TRIGT1MUCTPI_EVENTREADER_H
#define TRIGT1MUCTPI_EVENTREADER_H

// STL include(s):
#include <string>

// Local include(s):
#include "../Logging/MsgLogger.h"

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class Sector;
   class EventSource;

   /**
    *  @short Interface for classes reading in event data and passing it to the Sectors.
    *
    *         The EventReader is a purely abstract class defining the
    *         interface for a concrete Event Reader. In addition to its
    *         two abstract mamber functions it contains a static pointer
    *         to itself since Event Readers are Singletons.
    *
    *    @see EventReaderFactory
    *    @see MifFileReader
    *    @see SimMessageLogger
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 364083 $
    * $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
    */
   class EventReader {

   public:
      /**
       * This routine checks if the installation of the EventReader
       * has been successfull. If it does not return "true" the result
       * of putNextEvent() might be useless. In case the method returns
       * false getInfoString() can be called in order to find the reason.
       * @return true if the EventReader has been correctly installed
       *         and can be used.
       * @see EventReader::getInfoString()
       */
      virtual bool isValid() const = 0;
      /**
       * After a call to isValid() this routine gives back a string which
       * contains the reason why the EventReader is not valid.
       * @return string contains some text indicating the reason why
       *         the EventReader is not vaild.
       * @see EventReader::isValid()
       */
      virtual std::string getInfoString() const = 0;
      /**
       * If a sector got a pointer to the concrete EventReader it has to
       * register with this function so that the concrete EventReader can
       * can update the data field of the sector after having read a new
       * event.
       * @see Sector::set()
       */
      virtual void registerSector( Sector* ) = 0;
      /**
       * If a sector is destroyed it has to notify the EventReader about
       * the fact that its existence is about to cease. Therefore a unregister
       * method is needed. (The EventReader might live longer than a Sector)
       */
      virtual void unregisterSector( Sector* ) = 0;
      /**
       * This function reads in the data of the next events and stores them
       * in the correponding Sectors.
       * @return false if the end of the input data is reached (after the
       *         last successfull event read)
       * @see Sector::set()
       */
      virtual bool putNextEvent() = 0;
      /**
       * This method is intended for EventReaders which read events from
       * a sequential media. The default implementation does nothing. It
       * has been decided to put this method in the EventReader interface
       * in order to allow for polymorphism (i.e. in order to allow to
       * invoke the method from an EventReader.). This might not be the
       * best solution. Anyway, at the moment the method has only been
       * implemented in order to play around with the CORBA MUCTPI
       * implementation via a demo java application. In the real life
       * this method is not expected to be of any use.
       */
      virtual void rewind() { }
      virtual void setEventSource( EventSource & ) = 0;

      EventReader();
      virtual ~EventReader();

   protected:
      mutable MsgLogger m_logger;
      std::string m_infoString;

   }; // class EventReader

} // namespace LVL1MUCTPI

#endif // LVL1MUCTPITEST_EVENTREADER_H
