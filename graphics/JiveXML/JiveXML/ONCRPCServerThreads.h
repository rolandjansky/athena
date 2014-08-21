/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_ONCRPCSERVERPROC_H
#define JIVEXML_ONCRPCSERVERPROC_H

#include "JiveXML/ONCRPCServer.h"

namespace JiveXML{

  //Forward declarations
  class IServer;
  class ThreadCollection;

  //Global link to thread collection
  extern ThreadCollection* const dispatchThreads;

  /**
   * Arguments handed over fromt the main (Athena) thread to the server thread
   */
  typedef struct ServerThreadArguments_t ServerThreadArguments;
  struct ServerThreadArguments_t {
    ServerThreadArguments_t(JiveXML::IServer* const serverPtr, const int portNumber, void (* const cleanupHandler)(void*)=NULL):
    ServerPtr(serverPtr), PortNumber(portNumber), CleanUpHandler(cleanupHandler) {}          //Constructor
    JiveXML::IServer* const ServerPtr;                     //const pointer to the gaudi service
    const int PortNumber;                                  //port number to bind to
    void (* const CleanUpHandler)(void*);                         //Cleanup handler to be run when the main thread exits
  };

  /**
   * This is the actual server thread, which takes above arguments
   */
  void* ONCRPCServerThread( void* args );

  /**
   * Arguments handed over fromt the main server thread to the thread
   * dispatching the request (one for each request)
   */
  typedef struct DispatchThreadArguments_t DispatchThreadArguments;
  struct DispatchThreadArguments_t {
    DispatchThreadArguments_t(JiveXML::IServer* const serverPtr, const svc_req* rqp, const Event* event, const EventRequest* req):
      ServerSvcPtr(serverPtr),rqstp(rqp),evt(event),evtReq(req){}          //Constructor
    JiveXML::IServer* const ServerSvcPtr;               //cannot modify pointer, only object
    const struct svc_req* rqstp;
    const Event* evt;
    const EventRequest* evtReq;
  };
  
  /**
   * This is the thread handling the request - one thread per request
   */
  void* ONCRPCDispatchThread( void* args );

  /**
   * The Request handler is called from the RPC kernel routines - hence extern C
   * It creates a new dispatch thread and returns
   */
  extern "C" void ONCRPCRequestHandler( struct svc_req* rqstp, SVCXPRT* transp);

}
#endif
