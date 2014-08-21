/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML__ONCRPCSERVERPROCS_H
#define JIVEXML__ONCRPCSERVERPROCS_H

#include <string>

/**
 * This is the collection of the functions that are actually called from the
 * dispatch threads to collect the information from the server service and
 * return them to the caller.
 */

//forward declaration
#ifndef __APPLE__
struct SVCXPRT;
#else
#include <rpc/rpc.h>
#endif
struct EventRequest;
struct Event;

namespace JiveXML{

  //Forward declarations
  class IServer;
  class IMessage;

  /**
   * Simple result checking routine, that will output an errorMsg throught the
   * ServerSvc if there was an error. Returns false on error;
   */
  bool checkResult( const int RetVal, const std::string Module, IMessage* const ServerSvc);

  /**
   * Implementation of NULLPROC 
   * Return nothing - server has just been pinged
   */
  void ReturnNull( SVCXPRT* transp, IServer* const ServerSvc ) ;

  /**
   * Implementation of ONCRPC_ATHENASTATUS_PROC 
   * Return the current athena status in XDR representation
   */
  void ReturnState( SVCXPRT* transp, IServer* const ServerSvc ) ;

  /**
   * Implementation of ONCRPC_GETSTREAMS_PROC 
   * Return the currently available event streams
   */
  void ReturnStreams( SVCXPRT* transp, IServer* const ServerSvc ) ;

  /**
   * Implementation of ONCRPC_GETEVENT_PROC 
   * Return an event from a certain streams
   */
  void ReturnEvent( SVCXPRT* transp, const EventRequest* eventReq, IServer* const ServerSvc ) ;

  /**
   * Implementation of ONCRPC_SETEVENT_PROC 
   * Set a new event for a certain streams
   */
  void SetNewEvent( SVCXPRT* transp, const Event* event, IServer* const ServerSvc ) ;
}
#endif
