// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventSource.h 364083 2011-05-06 09:09:55Z krasznaa $
#ifndef TRIGT1MUCTPI_EVENTSOURCE_H
#define TRIGT1MUCTPI_EVENTSOURCE_H

namespace LVL1MUCTPI {

   /**
    *  @short Parent class of all EventSources
    *            
    *         This class only exists in order to be able to use 
    *         polmorphy with EventSources. The EventReader has an 
    *         abstract method setEventSource which needs an argument
    *         which is of a type EventSource. The idea is that the user
    *         only deals with abstract EventReaders. He does not need to
    *         know what concrete type of the EventReader which has been 
    *         instantiated. This was once done by the EventReaderFactory.
    *         But he needs to give to the EventReader the correct type
    *         of EventSource of course. In the concrete EventReaders 
    *         the EventSource is dynamic_casted to the concrete EventSource
    *         for the concrete EventReader. If the cast fails a error
    *         message is issued. Since dynamic_cast requires at least
    *         one abstract method in the base class, a dummy method has
    *         been introduced. 
    *            
    *         The design is questionable in the sense that at the 
    *         point when the user has to set the EventSource he also 
    *         needs to know which concrete EventReader has been 
    *         chosen.
    *            
    *    @see EventReader
    * @author Thorsten Wengler
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 364083 $
    * $Date: 2011-05-06 11:09:55 +0200 (Fri, 06 May 2011) $
    */
   class EventSource {

   public:
      virtual ~EventSource() {}
      virtual const char* printSource() const = 0;

   }; // class EventSource

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_EVENTSOURCE_H
