/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
       log << MSG::ERROR << "Unable to initialize access lock while starting server: " << strerror(retVal) << endreq;
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
      log << MSG::ERROR << "Thread creation failed" << endreq;
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

    log << MSG::VERBOSE << "StopServer()" << endreq;

    /**
     * Ping the server which will cause another request
     * Otherwise the server won't update its loop condition
     */

    //Create a client - this will already cause an update of the file
    //descriptors on the sockets
    CLIENT* client = clnt_create("localhost", ONCRPCSERVERPROG,ONCRPCSERVERVERS, "tcp");
    if (!client){
      log << MSG::ERROR << "Unable to create shutdown client for local server" << endreq;
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
    log << MSG::INFO << "Waiting for server thread to terminate ..." << endreq;
    pthread_join(m_ServerThreadHandle, &ret);
    log << MSG::INFO << " ... finished server thread" << endreq;

    //check if there was a return value
    if (ret){
      //Get the return value
      unsigned long NRequests = *(unsigned long*)ret;
      log << MSG::DEBUG << "Server thread stopped after handling " << NRequests << " requests" << endreq;
    } else 
      log << MSG::WARNING << "Server thread had stopped unexpectedly" << endreq;

    //Destroy the access lock
    int retVal = pthread_mutex_destroy(&m_accessLock);
    if (retVal != 0){
       log << MSG::ERROR << "Unable to destroy access lock after stopping server: " << strerror(retVal) << endreq;
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
      log << MSG::ERROR << "Server thread stopped while run-server-flag is set!" << endreq;
      //since we can't return statusCode failure (we're in a different thread!)
      //set the flag to mark the server as dead
      m_runServerThread = false;
    }
  }

  /** 
   * Deliver a message - possibly from another thread - to the athena msgSvc;
   */
  void ONCRPCServerSvc::Message( const MSG::Level level, const std::string msg) const {
    //Deliver message
    log << level << msg << endreq;
    //Spit it out immediately
    log.flush();
  }

  /** 
   * Return the logging level - possibly to detect whether it is worth
   * retrieving the information to be logged
   */
  MSG::Level ONCRPCServerSvc::LogLevel() const {
    //return the current logging level
    return log.level();
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
      log << MSG::ERROR << "Unable to obtain access lock to get stream names: " << strerror(retVal) << endreq;
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
      log << MSG::ERROR << "Unable to release access lock after getting stream names: " << strerror(retVal) << endreq;

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
      log << MSG::ERROR << "Unable to obtain access lock to get stream ID: " << strerror(retVal) << endreq;
      return EventStreamID("");
    }

    // Search the entry in the map
    EventStreamMap::const_iterator MapItr = m_eventStreamMap.find(EventStreamID(StreamName));

    //Release the lock
    retVal = pthread_mutex_unlock(&m_accessLock);
    if ( retVal != 0 )
      log << MSG::ERROR << "Unable to release access lock after getting stream ID: " << strerror(retVal) << endreq;

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
      log << MSG::ERROR << "Unable to obtain access lock to get event: " << strerror(retVal) << endreq;
      return std::string("");
    }

    // Search the entry in the map
    EventStreamMap::const_iterator MapItr = m_eventStreamMap.find(evtStreamID);

    //Release the lock
    retVal = pthread_mutex_unlock(&m_accessLock);
    if ( retVal != 0 )
      log << MSG::ERROR << "Unable to release access lock after getting stream event: " << strerror(retVal) << endreq;

    //If the element was not found return an empty string
    if ( MapItr == m_eventStreamMap.end()) return std::string("");

    //Return a copy of the found event string
    return std::string((*MapItr).second);
  }

  /**
   * Constructor
   **/
  ONCRPCServerSvc::ONCRPCServerSvc( const std::string& name, ISvcLocator* sl) :
    base_class( name, sl ),
    log(msgSvc(),name)
  {
      //declare port number property
      declareProperty("PortNumber",m_portNumber=0,"Port number to bind - assigned dynamically if set to zero [default: 0]");

      //Make sure ServerThread does not start unexpectedly
      m_runServerThread = false ;

      log << MSG::VERBOSE << "Output level is set to " << (int)log.level() << endreq;

  }

  /**
   * Destructor
   */
  ONCRPCServerSvc::~ONCRPCServerSvc(){

      log << MSG::VERBOSE << "Destructor() " << endreq;
  }

  /**
   * Initialize - called once in beginning
   * - create server
   */
  StatusCode ONCRPCServerSvc::initialize() {

    //Initialize Service base
    if (Service::initialize().isFailure()) return StatusCode::FAILURE;
    
     //Initialize message stream level
    log.setLevel(outputLevel());
    log << MSG::VERBOSE << "Initialize()" << endreq; 
    log << MSG::VERBOSE << "Output level is set to " << (int)log.level() << endreq;

    /**
     * Create the server itself
     */
    if (StartServer().isFailure()){
      log << MSG::ERROR << "Unable to start server" << endreq;
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }

  /**
   * Finalize - called once at the end
   * - shut down server
   */
  StatusCode ONCRPCServerSvc::finalize() {

    log << MSG::VERBOSE << "Finalize()" << endreq; 

    //Stop the server thread and return status code
    return StopServer();
  }

  /**
   * Implementation of IServerSvc method. Get one event and put it as the new
   * event for the this stream, which is identified by EventStreamID
   */
  StatusCode ONCRPCServerSvc::UpdateEventForStream( const EventStreamID evtStreamID, const std::string & event) {
 
    log << MSG::VERBOSE << "UpdateEventForStream()" << endreq;

    //Make sure the server is still running
    if (! m_runServerThread ){
      log << MSG::ERROR << "Server thread is not running - refusing to update event" << endreq;
      return StatusCode::FAILURE;
    }

    //Check that the event stream id is valid
    if (!evtStreamID.isValid()){
      log << MSG::ERROR << "Invalid event stream identifier - cannot add event" << endreq;
      return StatusCode::FAILURE;
    }

    //Make sure we don't have already exceeded the maximum number of streams
    if (m_eventStreamMap.size() > NSTREAMMAX ){
      log << MSG::ERROR << "Reached max. allowed number of streams " << NSTREAMMAX << " - cannot add event" << endreq;
      return StatusCode::FAILURE;
    }

    //Make sure the event is not larger than the allowed maximal size
    if (event.length() > NBYTESMAX ){
      log << MSG::ERROR << "Event is larger than allowed max. of " << NBYTESMAX << " bytes - cannot add event" << endreq;
      return StatusCode::FAILURE;
    }

    //Make sure we are the only one accessing the data right now, by trying to
    //obtain a lock. If the lock can not be obtained after a certain time, an
    //error is reported
    
    //Timeout of 5 second and 0 nanoseconds
    struct timespec timeout = { 5, 0 };
    //Try to obtain the lock
#ifndef __APPLE__
    int retVal = pthread_mutex_timedlock(&m_accessLock, &timeout);
#else
    int retVal = pthread_mutex_lock(&m_accessLock);
#endif
    if ( retVal != 0 ){
      log << MSG::ERROR << "Unable to obtain access lock to update event: " << strerror(retVal) << endreq;
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
    
    } catch ( std::exception e ) {
      log << MSG::ERROR << "Exception caught while updating event for stream " << evtStreamID.StreamName()
          << ": " << e.what() << endreq;
      //Also release the lock in this case
      pthread_mutex_unlock(&m_accessLock);
      //before we return
      return StatusCode::FAILURE;
    }

    //Finally release the lock again
    retVal = pthread_mutex_unlock(&m_accessLock);
    if ( retVal != 0 ){
      log << MSG::ERROR << "Unable to release access lock after updating event: " << strerror(retVal) << endreq;
      return StatusCode::FAILURE;
    }

    log << MSG::DEBUG << "Updated stream " << evtStreamID.StreamName() 
                      << " with event Nr. " << evtStreamID.EventNumber() 
                      << " from run Nr. " << evtStreamID.RunNumber() << endreq;

    return StatusCode::SUCCESS;
  }

} //namespace
