/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_IEVENTSERVER_H
#define JIVEXML_IEVENTSERVER_H

#include <vector>
#include <JiveXML/IMessage.h>

namespace JiveXML {

  //forward declarations
  class EventStreamID;

  /**
   * Pure abstract interface for all event serving classes that provide events,
   * streams and status information to the serving threads.
   */
  class IEventServer {

    public:

      //Need virtual destructor
      virtual ~IEventServer(){}

      /** @name Event serving methods */
      //@{
      /** get the names of all the streams */
      virtual std::vector<std::string> GetStreamNames() const = 0;
      /** get the current EventStreamID for a particular stream */
      virtual const EventStreamID GetEventStreamID(std::string streamName) const = 0;
      /** get the current event for a particular stream */
      virtual const std::string GetEvent( const EventStreamID evtStreamID ) const = 0;
      /** get the Status of the application */
      virtual int GetState() const = 0;
      /** The server thread will stop once this flag is set to false */
      virtual bool GetRunServerFlag () const = 0;
      //@}

  };
 
} //namespace

#endif

