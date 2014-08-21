/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_ONCRPCXDRPROCS_H
#define JIVEXML_ONCRPCXDRPROCS_H

/**
 * These are the XDR procedures that are used to encode/decode the data which is
 * transfered inbetween client and server in the XDR (eXternal Data Representation).
 * format. This format is platform independant ( see standards RFC 1832 and RFC 4506).
 */

#include <JiveXML/ONCRPCServer.h>

namespace JiveXML {

  /**
   * De- and Encoding of Streams_t
   */
  bool_t xdr_streams( XDR* xdrsp, Streams* streams );

  /**
   * De-/Encoding of EventRequest_t
   */
  bool_t xdr_event_req( XDR* xdrsp, EventRequest* eventReq );

  /**
   * De-/Encoding of Event_t
   */
  bool_t xdr_event( XDR* xdrsp, Event* event );

}
#endif
