/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JiveXML/ONCRPCXDRProcs.h"

namespace JiveXML {


  /**
   * Implementation of XDR converters for ONCRPC_GETSTREAMS_PROC 
   * Return the currently available event streams in XDR representation
   */

  //De-/Encoding of Streams_t
  bool_t xdr_streams( XDR* xdrsp, Streams* streams ){

    // As we do not know the string length, we use xdr_wrap_string
    return (xdr_array( xdrsp, (caddr_t*)&streams->StreamNames, &streams->NStreams,
            NSTREAMMAX, sizeof(char*), (xdrproc_t)xdr_wrapstring));
  }

  /**
   * Implementation of XDR converters for ONCRPC_GETEVENT_PROC 
   * Return the currently available eventRequest or event in XDR representation
   */

  //De-/Encoding of EventRequest_t
  bool_t xdr_event_req( XDR* xdrsp, EventRequest* eventReq ){

    //de-/encode event number
    if (! xdr_int64_t( xdrsp, &(eventReq->EventNumber)))
      return false ;
    
    //de-/encode run number
    if (! xdr_int64_t( xdrsp, &(eventReq->RunNumber)))
      return false ;

    //de-/encode stream name
    if (! xdr_wrapstring( xdrsp, &(eventReq->StreamName)))
      return false ;

    // All done successfully
    return true ;
  }

  //De-/Encoding of Event_t
  bool_t xdr_event( XDR* xdrsp, Event* event ){

    //de-/encode is-available bit
    if (! xdr_bool( xdrsp, &(event->isAvailable)))
      return false ;

    //de-/encode is-available bit
    if (! xdr_bool( xdrsp, &(event->isIdentical)))
      return false ;

    //de-/encode is-available bit
    if (! xdr_bool( xdrsp, &(event->isCompressed)))
      return false ;
    
    //de-/encode stream name
    if (! xdr_wrapstring( xdrsp, &(event->StreamName)))
      return false ;

    //de-/encode event number
    if (! xdr_int64_t( xdrsp, &(event->EventNumber)))
      return false ;
    
    //de-/encode run number
    if (! xdr_int64_t( xdrsp, &(event->RunNumber)))
      return false ;

    // wrap data as an array of bytes with length NBytes - this allows non-null
    // terminated byte arrays to be send. Also - in contrast to an array - no
    // subroutine is called for each byte. Instead, the string is just padded
    // with null-char to obtain a length divisible by four
   return (xdr_bytes( xdrsp, (caddr_t*)&event->EventData, &event->NBytes, NBYTESMAX));
  }
}


