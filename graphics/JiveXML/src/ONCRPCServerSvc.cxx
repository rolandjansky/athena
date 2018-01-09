/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StateMachine.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/GaudiHandle.h"

#include "JiveXML/ONCRPCServerSvc.h"
#include "JiveXML/ONCRPCServerThreads.h"
#include "JiveXML/ONCRPCThreadCollection.h"
#include "JiveXML/ONCRPCXDRProcs.h"
#include "JiveXML/ONCRPCServer.h"


namespace JiveXML {

  /**
   * Create the server by
   * - passing a set of arguments including a this-pointer
   * - setting the thread-running flag
   * - starting the server thread
   */
  StatusCode ONCRPCServerSvc::StartServer(){

    //Initialize access lock mechanism to ensure that the data map is never
    //accessed by more than one thread at the same time. NULL means default
    int retVal = pthread_mutex_init(&m_accessLock,NULL);
    if (retVal != 0){
      ATH_MSG_ERROR( "Unable to initialize access lock while starting server: " << strerror(retVal)  );
       return StatusCode::FAILURE;
    }

    //The arguments passed on to the server - create new object on the heap that
    //is persistent through the lifetime of the thread
    ServerThreadArguments* args = new ServerThreadArguments(this, m_portNumber);

    //set runServer flag to true, so the  thread will start
    m_runServerThread = true ;

    //create thread itself
    if ( ( pthread_create(&m_ServerThreadHandle, NULL , &JiveXML::ONCRPCServerThread, (void *) args )) != 0){
      
      //Create thread failed
      ATH_MSG_ERROR( "Thread creation failed"  );
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;

  }
 
  /**
   * Stop the server by
   * - unsetting the thread-running flag
   * - waiting till the server thread has finished
   * - destroying the thread handle
   */
  StatusCode ONCRPCServerSvc::StopServer(){

    ATH_MSG_VERBOSE( "StopServer()"  );

    /**
     * Ping the server which will cause another request
     * Otherwise the server won't update its loop condition
     */

    //Create a client - this will already cause an update of the file
    //descriptors on the sockets
    CLIENT* client = clnt_create("localhost", ONCRPCSERVERPROG,ONCRPCSERVERVERS, "tcp");
    if (!client){
      ATH_MSG_ERROR( "Unable to create shutdown client for local server"  );
      return StatusCode::FAILURE;
    }

    // Next unset the runServerThread flag, which will cause the loop to stop
    // This needs to happen after client creation, otherwise the next call won't
    // be serverd anymore
    m_runServerThread = false ; 

    // Now issue the call with a timeout
    struct timeval timeout; timeout.tv_sec = 1; timeout.tv_usec = 0;
    clnt_call(client, NULLPROC, (xdrproc_t)xdr_void, NULL, (xdrproc_t)xdr_void, NULL, timeout);

    // A pointer to the return value of the thread
    void* ret;
    // wait till the server thread has finished
    ATH_MSG_INFO( "Waiting for server thread to terminate ..."  );
    pthread_join(m_ServerThreadHandle, &ret);
    ATH_MSG_INFO( " ... finished server thread"  );

    //check if there was a return value
    if (ret){
      //Get the return value
      unsigned long NRequests = *(unsigned long*)ret;
      ATH_MSG_DEBUG( "Server thread stopped after handling " << NRequests << " requests"  );
    } else 
      ATH_MSG_WARNING( "Server thread had stopped unexpectedly"  );

    //Destroy the access lock
    int retVal = pthread_mutex_destroy(&m_accessLock);
    if (retVal != 0){
       ATH_MSG_ERROR( "Unable to destroy access lock after stopping server: " << strerror(retVal)  );
       return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }
  
  /**
   * This callback will get called whenever the server thread stops
   */
  void ONCRPCServerSvc::ServerThreadStopped(){
    //Check if the thread stopped while the run server flag was set
    if (m_runServerThread){
      //Deliver an error message
      ATH_MSG_ERROR( "Server thread stopped while run-server-flag is set!"  );
      //since we can't return statusCode failure (we're in a different thread!)
      //set the flag to mark the server as dead
      m_runServerThread = false;
    }
  }

  /** 
   * Deliver a message - possibly from another thread - to the athena msgSvc;
   */
  void ONCRPCServerSvc::Message( const MSG::Level level,
                                 const std::string mesg) const
  {
    //Deliver message
    msg() << level << mesg << endmsg;
    //Spit it out immediately
    msg().flush();
  }

  /** 
   * Return the logging level - possibly to detect whether it is worth
   * retrieving the information to be logged
   */
  MSG::Level ONCRPCServerSvc::LogLevel() const {
    //return the current logging level
    return msg().level();
  }


  /**
   * Return the current athena state
   */
  int ONCRPCServerSvc::GetState() const {
    /**
     * at the moment simply return state machine state - we might consider
     * checking for state transitions (m_state != m_targetState) and return
     * a "busy" value. At the moment, all state transitions should be
     * instantanious, so this is not necessary
     **/
    return (int)FSMState();
  }

  /**
   * Return an array with all the stream names
   */
  std::vector<std::string> ONCRPCServerSvc::GetStreamNames() const {
    
    //Create a vector that can be returned
    std::vector<std::string> StreamNames;
    
    //Obtain an exclusive access lock
    int retVal = pthread_mutex_lock(&m_accessLock);
    if ( retVal != 0 ){
      ATH_MSG_ERROR( "Unable to obtain access lock to get stream names: " << strerror(retVal)  );
      return StreamNames;
    }

    // Iterate over map to get entries
    EventStreamMap::const_iterator MapItr = m_eventStreamMap.begin();
    for ( ; MapItr != m_eventStreamMap.end(); ++MapItr){
      
      //Get the EventStreamID object
      EventStreamID EvtStrID = (*MapItr).first;

      //Add the name of this EventStreamID to the list of stream names
      StreamNames.push_back(EvtStrID.StreamName());
    }

    //Release the lock
    retVal = pthread_mutex_unlock(&m_accessLock);
    if ( retVal != 0 )
      ATH_MSG_ERROR( "Unable to release access lock after getting stream names: " << strerror(retVal)  );

    //Return the list of names
    return StreamNames;
  }

  /**
   * Return the EventStreamID for the last event of a given stream
   */
  const EventStreamID ONCRPCServerSvc::GetEventStreamID( const std::string StreamName) const {
    
    //Obtain an exclusive access lock
    int retVal = pthread_mutex_lock(&m_accessLock);
    if ( retVal != 0 ){
      ATH_MSG_ERROR( "Unable to obtain access lock to get stream ID: " << strerror(retVal)  );
      return EventStreamID("");
    }

    // Search the entry in the map
    EventStreamMap::const_iterator MapItr = m_eventStreamMap.find(EventStreamID(StreamName));

    //Release the lock
    retVal = pthread_mutex_unlock(&m_accessLock);
    if ( retVal != 0 )
      ATH_MSG_ERROR( "Unable to release access lock after getting stream ID: " << strerror(retVal)  );

    //If the element was not found return an invalid ID
    if ( MapItr == m_eventStreamMap.end()) return EventStreamID("");

    //Return the found event stream identifier
    return (*MapItr).first;
  }

  /**
   * Return the event for a given stream
   */
  const std::string ONCRPCServerSvc::GetEvent( const EventStreamID evtStreamID ) const {
    
    //Obtain an exclusive access lock
    int retVal = pthread_mutex_lock(&m_accessLock);
    if ( retVal != 0 ){
      ATH_MSG_ERROR( "Unable to obtain access lock to get event: " << strerror(retVal)  );
      return std::string("");
    }

    // Search the entry in the map
    EventStreamMap::const_iterator MapItr = m_eventStreamMap.find(evtStreamID);

    //Release the lock
    retVal = pthread_mutex_unlock(&m_accessLock);
    if ( retVal != 0 )
      ATH_MSG_ERROR( "Unable to release access lock after getting stream event: " << strerror(retVal)  );

    //If the element was not found return an empty string
    if ( MapItr == m_eventStreamMap.end()) return std::string("");

    //Return a copy of the found event string
    return std::string((*MapItr).second);
  }

  /**
   * Constructor
   **/
  ONCRPCServerSvc::ONCRPCServerSvc( const std::string& name, ISvcLocator* sl) :
    base_class( name, sl )
  {
      //declare port number property
      declareProperty("PortNumber",m_portNumber=0,"Port number to bind - assigned dynamically if set to zero [default: 0]");

      //Make sure ServerThread does not start unexpectedly
      m_runServerThread = false ;

      ATH_MSG_VERBOSE( "Output level is set to " << (int)msg().level()  );

  }

  /**
   * Destructor
   */
  ONCRPCServerSvc::~ONCRPCServerSvc(){

    ATH_MSG_VERBOSE( "Destructor() "  );
  }

  /**
   * Initialize - called once in beginning
   * - create server
   */
  StatusCode ONCRPCServerSvc::initialize() {

    //Initialize Service base
    if (Service::initialize().isFailure()) return StatusCode::FAILURE;
    
     //Initialize message stream level
    msg().setLevel(outputLevel());
    ATH_MSG_VERBOSE( "Initialize()"  );
    ATH_MSG_VERBOSE( "Output level is set to " << (int)msg().level()  );

    /**
     * Create the server itself
     */
    ATH_CHECK( StartServer() );
    return StatusCode::SUCCESS;
  }

  /**
   * Finalize - called once at the end
   * - shut down server
   */
  StatusCode ONCRPCServerSvc::finalize() {

    ATH_MSG_VERBOSE( "Finalize()"  );

    //Stop the server thread and return status code
    return StopServer();
  }

  /**
   * Implementation of IServerSvc method. Get one event and put it as the new
   * event for the this stream, which is identified by EventStreamID
   */
  StatusCode ONCRPCServerSvc::UpdateEventForStream( const EventStreamID evtStreamID, const std::string & event) {
 
    ATH_MSG_VERBOSE( "UpdateEventForStream()"  );

    //Make sure the server is still running
    if (! m_runServerThread ){
      ATH_MSG_ERROR( "Server thread is not running - refusing to update event"  );
      return StatusCode::FAILURE;
    }

    //Check that the event stream id is valid
    if (!evtStreamID.isValid()){
      ATH_MSG_ERROR( "Invalid event stream identifier - cannot add event"  );
      return StatusCode::FAILURE;
    }

    //Make sure we don't have already exceeded the maximum number of streams
    if (m_eventStreamMap.size() > NSTREAMMAX ){
      ATH_MSG_ERROR( "Reached max. allowed number of streams " << NSTREAMMAX << " - cannot add event"  );
      return StatusCode::FAILURE;
    }

    //Make sure the event is not larger than the allowed maximal size
    if (event.length() > NBYTESMAX ){
      ATH_MSG_ERROR( "Event is larger than allowed max. of " << NBYTESMAX << " bytes - cannot add event"  );
      return StatusCode::FAILURE;
    }

    //Make sure we are the only one accessing the data right now, by trying to
    //obtain a lock. If the lock can not be obtained after a certain time, an
    //error is reported
    
    //Timeout of 1 second and 0 nanoseconds
    struct timespec timeout = { 1, 0 };
    //Try to obtain the lock
#ifndef __APPLE__
    int retVal = pthread_mutex_timedlock(&m_accessLock, &timeout);
#else
    int retVal = pthread_mutex_lock(&m_accessLock);
#endif
    if ( retVal != 0 ){
      ATH_MSG_ERROR( "Unable to obtain access lock to update event: " << strerror(retVal)  );
      return StatusCode::FAILURE;
    }

    //Using try/catch to ensure the mutex gets unlocked in any case
    try {

      //Using std::map::operator[] and std::map::insert() will create a new event
      //if it did not exist, otherwise just replace the existing entry (making a
      //copy of the std::string) but would not update the key which holds new
      //event/run number. Therefore delete existing entry first.

      //Delete old entry if there is one
      EventStreamMap::iterator OldEvtItr = m_eventStreamMap.find(evtStreamID);
      if (OldEvtItr != m_eventStreamMap.end()) 
        m_eventStreamMap.erase(OldEvtItr);
      
      //Now add the new event
      m_eventStreamMap.insert(EventStreamPair(evtStreamID,event));
    
    } catch ( const std::exception& e ) {
      ATH_MSG_ERROR( "Exception caught while updating event for stream " << evtStreamID.StreamName()
                     << ": " << e.what()  );
      //Also release the lock in this case
      pthread_mutex_unlock(&m_accessLock);
      //before we return
      return StatusCode::FAILURE;
    }

    //Finally release the lock again
    retVal = pthread_mutex_unlock(&m_accessLock);
    if ( retVal != 0 ){
      ATH_MSG_ERROR( "Unable to release access lock after updating event: " << strerror(retVal)  );
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG( "Updated stream " << evtStreamID.StreamName() 
                   << " with event Nr. " << evtStreamID.EventNumber() 
                   << " from run Nr. " << evtStreamID.RunNumber()  );

    return StatusCode::SUCCESS;
  }

} //namespace
