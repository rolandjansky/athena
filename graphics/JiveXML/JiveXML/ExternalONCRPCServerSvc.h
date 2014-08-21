/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_ExternalONCRPCSERVERSVC_H
#define JIVEXML_ExternalONCRPCSERVERSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "JiveXML/EventStream.h"
#include "JiveXML/IServerSvc.h"

//Forward declaration
extern "C" struct CLIENT;

namespace JiveXML {

  /**
   * This athena service will provide a link to an external ONCRPC server, e.g
   * running as standalone application or in another athena application. It only
   * provides means for streaming events to that server.
   */
  class ExternalONCRPCServerSvc : public extends1<Service, IServerSvc> {

  public:
    
    /** Default constructor */
    ExternalONCRPCServerSvc(const std::string& name, ISvcLocator* sl);
    /** Destructor */
    virtual ~ExternalONCRPCServerSvc();

    /** Gaudi default methods */
    StatusCode initialize();
    StatusCode finalize();

    /** @name Server methods */
    //@{
    /** Put this event as new current event for stream given by name */
    virtual StatusCode UpdateEventForStream( const EventStreamID evtStreamID, const std::string & event);
    //@}
     
  private:

    //Instantiate and release the client handle
    StatusCode GetClient();
    StatusCode ReleaseClient();

    //Get a timeval struct from the timeout double
    struct timeval GetTimeout();

    //The name of the machine the external server is running on
    //defaults to "localhost"
    std::string m_hostName;

    //The timeout when calling the server in seconds
    double m_timeOut;

    //A client handle
    CLIENT* m_client;

    //Messaging service
    mutable MsgStream log;

  };
 
} //namespace

#endif

