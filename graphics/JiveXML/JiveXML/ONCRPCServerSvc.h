/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_ONCRPCSERVERSVC_H
#define JIVEXML_ONCRPCSERVERSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "JiveXML/EventStream.h"
#include "JiveXML/IServerSvc.h"
#include "JiveXML/IServer.h"

namespace JiveXML {

  //forward declarations
  class ThreadCollection;

  /**
   * This athena service will create an ONC/RPC (aka SunRPC) server, that can
   * provide the athena status as well as data strings from events. It can
   * handle several different event streams, but for each stream only the latest
   * event is available. This service is usually used by the Athena JiveXML
   * algorithm providing XML events for an Atlantis Java client.
   *
   * The server will create a single server thread, which loops over the sockets
   * until the m_runServerThread flag is set to false. On each request, it will
   * call the ONCRPCRequestHandler, which will generate a new thread for each
   * request. The actual response to the requests happens in these ONCRPCDispatchThread.
   * A list of the thread handles is kept in the a ThreadCollection, where each
   * thread will add and remove itself. In the finalization stage, the server
   * will wait for all dispatch threats to finish before exiting.
   * 
   * On all stages, the server thread will have a handle to the athena service
   * via the thread specific key ServerSvcKey. It is thus possible to have many
   * servers running simultaneously. Note however, that currently RPC will
   * forbid you to start several servers with the same ONCRPCSERVERPROG.
   */

  class ONCRPCServerSvc : public extends1<Service, IServerSvc>, 
                          virtual public IServer {
   public:
    
    /** Default constructor */
    ONCRPCServerSvc(const std::string& name, ISvcLocator* sl);
    /** Destructor */
    virtual ~ONCRPCServerSvc();

    /** Gaudi default methods */
    StatusCode initialize();
    StatusCode finalize();


    /** @name Server methods */
    //@{
    /** get the Status of the application */
    int GetState() const ;
    /** get the names of all the streams */
    std::vector<std::string> GetStreamNames() const ;
    /** get the current EventStreamID for a particular stream */
    const EventStreamID GetEventStreamID(std::string streamName) const ;
    /** get the current event for a particular stream */
    const std::string GetEvent( const EventStreamID evtStreamID ) const ;
    /** Put this event as new current event for stream given by name */
    virtual StatusCode UpdateEventForStream( const EventStreamID evtStreamID, const std::string & event);
    //@}
    
    /**
     * This function is exposed to allow using athena messaging service from
     * other threads
     */
    void Message( const MSG::Level level, const std::string msg ) const ;
    /** Get the logging level **/
    MSG::Level LogLevel() const ;


    /** The server thread will stop once this flag is set to false */
    bool GetRunServerFlag () const { return m_runServerThread; };
    /** Start the server thread */
    StatusCode StartServer();
    /** Stop the server thread */
    StatusCode StopServer();
    /** Callback when server thread terminates */
    void ServerThreadStopped();
 
  private:

    //Port number property - defaults to zero in which case
    //it is dynamically assigned
    int m_portNumber;

    //A map of the streams and their current events
    EventStreamMap m_eventStreamMap;

    //A mutex (mutual exclusive) lock for the data map
    mutable pthread_mutex_t m_accessLock;

    //A handle to the server thread
    pthread_t m_ServerThreadHandle;

    //Once this flag is set to false, the thread will stop after handling its
    //last request
    bool m_runServerThread;

    //Messaging service
    mutable MsgStream log;

  };
 
} //namespace

#endif

