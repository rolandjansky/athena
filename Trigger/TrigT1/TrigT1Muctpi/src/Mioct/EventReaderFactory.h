// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventReaderFactory.h 364083 2011-05-06 09:09:55Z krasznaa $
#ifndef TRIGT1MUCTPI_EVENTREADERFACTORY_H
#define TRIGT1MUCTPI_EVENTREADERFACTORY_H

namespace LVL1MUCTPI {

   // Forward declaration(s):
   class EventReader;

   /**
    *  @short Generates the Event Reader depending on Environment Variable MUCTPI_EVENT_READER.
    *
    *         The EventReaderFactory  is  responsible for  instantiating
    *         the Event  Reader  depending on  the Environment  Variable
    *         MUCTPI_EVENT_READER. The  only allowed  value is currently
    *         MIF_FILE_READER   because there  is  no   other   concrete
    *         EventReader implemented at the moment.  If the Variable is
    *         not set, a default Event Reader is instantiated (currently
    *         the MIF_FILE_READER).
    *
    *         The  EventReaderFactory is implemented  as a Singleton. It
    *         is a   concrete class but might be  changed to an abstract
    *         class in future resleases. In that case it will correspond
    *         to  the design pattern of  an abstract factory of E.Gamma,
    *         R.Helm,     R.Johnson,   J.Vlissides "Design     Patterns"
    *         (Addison-Wesley).
    *
    *    @see muon_devil
    *    @see muon_cooker
    *    @see EventReader
    *    @see MifFileReader
    *
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 364083 $
    * $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
    */
   class EventReaderFactory {

   public:
      /**
       * The Event Reader Factory is implemented as a Singleton. This 
       * static member funtion instantiates the Factory in case it has
       * not yet been instantiated. In addition it instantiates a 
       * concrete Event Reader depending on the value of the Environment
       * Variable MUCTPI_EVENT_READER in case it has not been yet 
       * instantiated.
       * 
       * @return A pointer to the instantiated concrete Event Reader. 
       */
      static EventReader* getEventReader();

   }; // class EventReaderFactory

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_EVENTREADERFACTORY_H
