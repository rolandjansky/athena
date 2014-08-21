/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML__IEVENTRECEIVER_H
#define JIVEXML__IEVENTRECEIVER_H

#include <JiveXML/IMessage.h>

//forward declarations
class StatusCode;

namespace JiveXML {

  //forward declarations
  class EventStreamID;

  /**
   * Pure abstract interface for all event serving classes that provide events,
   * streams and status information to the serving threads.
   */
  class IEventReceiver {

    public:

      //Need virtual destructor
      virtual ~IEventReceiver(){}

      /** @name Event receiving methods */
      //@{
      /** Put this event as new current event for stream given by name */
      virtual StatusCode UpdateEventForStream( const EventStreamID, const std::string& ) = 0;
      //@}
      
  };
 
} //namespace

#endif

