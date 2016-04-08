/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML__ISERVER_H
#define JIVEXML__ISERVER_H

#include <JiveXML/IMessage.h>
#include <JiveXML/IEventServer.h>
#include <JiveXML/IEventReceiver.h>

namespace JiveXML {


  /**
   * Pure abstract interface for all full server implementations
   */
  class IServer: virtual public IEventServer,
                 virtual public IEventReceiver,
                 virtual public IMessage {

    public:
      //Need virtual destructor
      virtual ~IServer(){}

      //This termination handler shall be called from the server
      //whenever the serving thread is stopped
      virtual void ServerThreadStopped() = 0;
  };
 
} //namespace

#endif

