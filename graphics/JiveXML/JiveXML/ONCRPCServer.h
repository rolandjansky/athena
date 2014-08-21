/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML__ONCRPCSERVER_H
#define JIVEXML__ONCRPCSERVER_H

  /**
   * This header is shared inbetween the C-style server thread
   * and the C++ Athena ServerSvc. It defines
   * - The constants by which RPC program, version and procedure are idenfied
   * - The definition of the data types exchanged inbetween the client and the
   *   server
   */

extern "C" {
  #include <rpc/rpc.h>
  #include <rpc/pmap_clnt.h>
}

#include <math.h>

//Safeguard external includes
#ifdef __cplusplus
extern "C" {
#endif 

namespace JiveXML{

  #define ONCRPCSERVERPROG 200048965  //By this number the server is identified
  #define ONCRPCSERVERVERS 2      //By this number the version of the server is identified
  #define ONCRPC_GETSTATUS_PROC 1   //This number identifies the procedure returning the current athena status
  #define ONCRPC_GETSTREAMS_PROC 2   //This number identifies the procedure returning the names of the available streams
  #define ONCRPC_GETEVENT_PROC 3   //This number identifies the procedure returning an event for a specific stream
  #define ONCRPC_SETEVENT_PROC 4   //This number identifies the procedure setting a new event for a specific stream

  //Maximum allowed number of streams
  const unsigned int NSTREAMMAX = 1024 ;
  //Maximum allowd number of bytes in one event (2^31 for 4-byte ints)
  const unsigned int NBYTESMAX = 0x1 << (sizeof( unsigned int )*8 - 1) ; //2147483648 ; 

  /**
   * Data structures for GetStreams functions
   */
  typedef struct Streams_t Streams;
  struct Streams_t {
    unsigned int NStreams;
    char** StreamNames;
  };

  /**
   * Data structures for GetEvent functions
   */

  // When requesting an event from a particular stream, the user give the eventID
  // if the event he already has This avoids duplicate transfer of the same event
  typedef struct EventRequest_t EventRequest;
  struct EventRequest_t {
    int64_t EventNumber;
    int64_t RunNumber;
    char* StreamName;
  };

  // The event return structure, which holds several parameters of the event
  typedef struct Event_t Event;
  struct Event_t {
    bool_t isAvailable;    //This flag is set false if no event is available for the requested stream
    bool_t isIdentical;    //This flag is set true if the event is the same as the one the user had asked for
    bool_t isCompressed;   //This flag is set true if the event is compressed
    char* StreamName;        //Then name of the stream of this event
    int64_t EventNumber;     //The event number of the event that is send back
    int64_t RunNumber;       //The run number of the event that is send back
    unsigned int NBytes;       //Number of bytes in the event data
    const char* EventData;     //Address of event data - if there is any
  };

}//namespace

#ifdef __cplusplus
}
#endif 

#endif // include guards

