/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JiveXML/ExternalONCRPCServerSvc.h"
#include "JiveXML/ONCRPCServer.h"
#include "JiveXML/ONCRPCXDRProcs.h"

#include <rpc/rpc.h>

namespace JiveXML {


  /**
   * Constructor
   */
  ExternalONCRPCServerSvc::ExternalONCRPCServerSvc( const std::string& name, ISvcLocator* sl) :
    base_class ( name, sl ),
    m_client(NULL),
    log(msgSvc(),name)
  {
      //declare properties
      declareProperty("Hostname",m_hostName="localhost","The name of the host the external server is running on [ default: \"localhost\" ]");
      declareProperty("Timeout",m_timeOut=10,"Timeout when calling the server in seconds [ default: 10 ]");
  }

  /**
   * Destructor 
   */
  ExternalONCRPCServerSvc::~ExternalONCRPCServerSvc(){
      log << MSG::VERBOSE << "Destructor() " << endreq;
  }

  /**
   * Initialize - called once in beginning
   * - create server
   */
  StatusCode ExternalONCRPCServerSvc::initialize() {
    log << MSG::VERBOSE << "Initialize()" << endreq; 
    
    //Initialize Service base
    if (Service::initialize().isFailure()) return StatusCode::FAILURE;
    
     //Initialize message stream level
    log.setLevel(outputLevel());
    log << MSG::VERBOSE << "Output level is set to " << (int)log.level() << endreq;

    /**
     * Try preemtivly creating a client right here - but ignore the result
     */
    GetClient().ignore();
    
    return StatusCode::SUCCESS;
  }

  /**
   * Obtain a client handle by
   * - creating a CLIENT object
   * - pinging the server using NULLPROC
   */
  StatusCode ExternalONCRPCServerSvc::GetClient(){

    log << MSG::VERBOSE << "GetClient()" << endreq;

    //Try to create client if not there yet
#ifndef __APPLE__
    if (!m_client) m_client = clnt_create(m_hostName.c_str(), ONCRPCSERVERPROG,ONCRPCSERVERVERS, "tcp");
#else
	char proto[] = "tcp";
    if (!m_client) m_client = clnt_create((char*)m_hostName.c_str(), ONCRPCSERVERPROG,ONCRPCSERVERVERS, proto);
#endif    
    //Fail if we still don't have a client
    if (!m_client){
      log << MSG::WARNING << "Unable to create client for server on host " << m_hostName << " using TCP/IP " << endreq;
      return StatusCode::FAILURE;
    }

    //Now try pinging the server
    clnt_stat ret = clnt_call(m_client, NULLPROC, (xdrproc_t)xdr_void, NULL,
                             (xdrproc_t)xdr_void, NULL, GetTimeout());

    //And check for the result
    if (ret != RPC_SUCCESS){
      log << MSG::WARNING << "Failed calling the server on host " << m_hostName << " : " << clnt_sperrno(ret) << endreq;
      //Also destroy the client in this case - otherwise we might be stranded
      //with an invalid client object
      ReleaseClient().ignore();
      //and return failure
      return StatusCode::FAILURE;
    } 

    //Be verbose on success
    log << MSG::VERBOSE << "Successfully connected to server on host " << m_hostName << endreq;

    return StatusCode::SUCCESS;
  }

  /**
   * Convert timeout double in seconds to struct timeval
   **/
  struct timeval ExternalONCRPCServerSvc::GetTimeout(){
    //Declare return value
    struct timeval timeout = { 0, 0 };
    //Catch for negatives
    if (m_timeOut < 0)  return timeout;
    //Get seconds as integer part
    timeout.tv_sec = static_cast<long>(m_timeOut);
    //Get microseconds as fractional part
    timeout.tv_usec = static_cast<long>((m_timeOut - timeout.tv_sec)*1e6);
    //Finally return it
    return timeout;
  }

  /**
   * Destroy the private client handle
   */
  StatusCode ExternalONCRPCServerSvc::ReleaseClient(){
    //Destroy the client
    if (m_client) clnt_destroy(m_client);
    //And set pointer to NULL
    m_client=NULL;
    //For now always return success
    return StatusCode::SUCCESS;
  }

  /**
   * Finalize - called once at the end
   * - shut down server
   */
  StatusCode ExternalONCRPCServerSvc::finalize() {

    log << MSG::VERBOSE << "Finalize()" << endreq; 

    /**
     * Release the client
     */
    ReleaseClient().ignore();

    return StatusCode::SUCCESS;
  }

  /**
   * Implementation of IServerSvc method. Get one event and put it as the new
   * event for the this stream, which is identified by EventStreamID
   */
  StatusCode ExternalONCRPCServerSvc::UpdateEventForStream( const EventStreamID evtStreamID, const std::string & eventStr) {
 
    log << MSG::VERBOSE << "UpdateEventForStream()" << endreq;

    //Check that the event stream id is valid
    if (!evtStreamID.isValid()){
      log << MSG::ERROR << "Invalid event stream identifier - cannot add event" << endreq;
      return StatusCode::FAILURE;
    }

    //Now create an event C-struct that we can send to the server
    //NOTE: the struct has to be properly initialized
    Event event;
    event.isAvailable = false ; event.isIdentical = false ; event.isCompressed = false ;
    event.RunNumber = evtStreamID.RunNumber();
    event.EventNumber=evtStreamID.EventNumber();
    event.StreamName=strdup(evtStreamID.StreamName().c_str());
    event.NBytes = strlen(eventStr.c_str())+1;
    event.EventData = strdup(eventStr.c_str());

    log << MSG::VERBOSE << "Created event structure for event " << event.EventNumber 
                        << " from run " << event.RunNumber 
                        << " to be put in stream " << event.StreamName 
                        << " with " << event.NBytes << " bytes" << endreq;

    //Try to get a client
    if (GetClient().isFailure()) {
      log << MSG::WARNING << "Failed obtaining a client for sever on host " << m_hostName << endreq;
      log << MSG::WARNING << " while updating stream " << evtStreamID.StreamName()
                          << " with event " << evtStreamID.EventNumber() 
                          << " from run " << evtStreamID.RunNumber() << endreq;
      return StatusCode::FAILURE;
    }

    //Call the client to update the event
    bool isSuccess = false;
    clnt_stat ret = clnt_call(m_client, ONCRPC_SETEVENT_PROC, (xdrproc_t)xdr_event, (caddr_t)&event,
                             (xdrproc_t)xdr_bool,(caddr_t)&isSuccess, GetTimeout());

    //And check for the result
    if (ret != RPC_SUCCESS){
      log << MSG::WARNING << "Failed calling the server on host " << m_hostName << " : " << clnt_sperrno(ret) << endreq; 
      log << MSG::WARNING << " while updating stream " << evtStreamID.StreamName()
                          << " with event " << evtStreamID.EventNumber() 
                          << " from run " << evtStreamID.RunNumber() << endreq;
      return StatusCode::FAILURE;
    } 

    //Check if the server managed to update the event from return value
    if (!isSuccess){
      log << MSG::WARNING << "Server on host " << m_hostName << " returned failure " << endreq; 
      log << MSG::WARNING << " while updating stream " << evtStreamID.StreamName()
                          << " with event " << evtStreamID.EventNumber() 
                          << " from run " << evtStreamID.RunNumber() << endreq;
      return StatusCode::FAILURE;
    } 

    //Be verbose on success
    log << MSG::DEBUG << "Server on host " << m_hostName << " returned success "<< endreq;
    log << MSG::DEBUG << " while updating stream " << evtStreamID.StreamName()
                        << " with event " << evtStreamID.EventNumber() 
                        << " from run " << evtStreamID.RunNumber() << endreq;

    //Finally free the memory allocated for the event structure
    xdr_free((xdrproc_t)xdr_event,(caddr_t)&event);

    return StatusCode::SUCCESS;
  }

} //namespace
