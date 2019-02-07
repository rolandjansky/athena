/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JiveXML/ONCRPCServerThreads.h"
#include "JiveXML/ONCRPCThreadCollection.h"
#include "JiveXML/ONCRPCServerProcs.h"
#include "JiveXML/ONCRPCXDRProcs.h"

#include "JiveXML/IServer.h"

//Decoding of caller IP address
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#include <sstream>
#include <errno.h>
#ifdef __APPLE__
#include <rpc/rpc.h>
extern "C" void get_myaddress (struct sockaddr_in *);
#endif

namespace JiveXML {

  //Global data
  //This variable is visible in all threads
  ThreadCollection* const dispatchThreads = new ThreadCollection();
 
  //Thread specific data keys
  //These variables are global only within a single thread
  pthread_key_t ServerSvcKey;    //<! A pointer to the the Athena Service associated with this thread

  /**
   * This is the dispatch function, which does the actual calls of the
   * ServerSvc routines. Many of these dispatch requests may be running at the
   * same time, each in its own thread.
   */
  void* ONCRPCDispatchThread ( void* args ){
    
    //Get the thread arguments
    const struct svc_req* rqstp = ((DispatchThreadArguments*)args)->rqstp;
    const EventRequest* eventRequest = ((DispatchThreadArguments*)args)->evtReq;
    const Event* event = ((DispatchThreadArguments*)args)->evt;
    IServer* const ServerSvc = ((DispatchThreadArguments*)args)->ServerSvcPtr;

    //we have stored the arguments, so delete the object
    delete (DispatchThreadArguments*)args;

    try {

      //Now first of all register myself with the ServerSvc;
      dispatchThreads->AddThread(pthread_self());
      //Tell them the thread number if they want to know
      std::ostringstream msg; msg << "Created new dispatch thread " << pthread_self();
      ServerSvc->Message(MSG::VERBOSE,msg.str());

      //Process the request
      switch (rqstp->rq_proc){
        //NULLPROC - no data exchanged, just pinging for response
        case NULLPROC:
          ReturnNull(rqstp->rq_xprt, ServerSvc);
          break ; 
        //GETSTATUS - return the current athena status
        case ONCRPC_GETSTATUS_PROC:
          ReturnState(rqstp->rq_xprt, ServerSvc);
          break ;
        //GETSTREAMS - return the names of the available streams
        case ONCRPC_GETSTREAMS_PROC:
          ReturnStreams(rqstp->rq_xprt, ServerSvc);
          break ;
        //GETEVENT - return the current event in a given stream
        case ONCRPC_GETEVENT_PROC:
          ReturnEvent(rqstp->rq_xprt, eventRequest, ServerSvc);
          break ;
        //GETEVENT - return the current event in a given stream
        case ONCRPC_SETEVENT_PROC:
          SetNewEvent(rqstp->rq_xprt, event, ServerSvc);
          break ;
        default:
          std::ostringstream msg;
          msg << "Client request for server procedure #" << rqstp->rq_proc << " which is not defined";
          ServerSvc->Message(MSG::WARNING, msg.str());
          svcerr_noproc(rqstp->rq_xprt);
          break ;
      }

      //We have dealt with the request, delete it
      delete rqstp;
      delete event;
      delete eventRequest;

      //Finally unregister myself with the ServerSvc
      //Note that this will also detach the thread, so this has to be the last
      //statement before the thread returns
      dispatchThreads->RemoveThread(pthread_self());

    } catch (std::exception &e){
      std::ostringstream msg; msg << "Caught exception in DispatchThread: " << e.what();
      ServerSvc->Message(MSG::ERROR,msg.str());
    }

    return NULL;
  }


  /**
   * This function handles all the requests that are received from the server
   * by creating a new thread for each request. For request with an input value
   * the input parameters are extracted first, as svc_getreqset is not thread
   * safe.
   */
  extern "C" void ONCRPCRequestHandler( struct svc_req* rqstp, SVCXPRT* /*transp*/){

    //Get the ServerSvc pointer for this thread
    JiveXML::IServer* const ServerSvc = (JiveXML::IServer*)pthread_getspecific(ServerSvcKey);
    std::ostringstream msg; msg << "Request handler in thread " << pthread_self();
    ServerSvc->Message(MSG::VERBOSE,msg.str()); msg.str("");

    try {

      //Check wether it is worth retrievin caller information
      if ( ServerSvc->LogLevel() <= MSG::DEBUG ){

        //Get information about the requester
        struct sockaddr_in* caller = svc_getcaller(rqstp->rq_xprt);
        //Get port number
        int port = caller->sin_port;
        //Get IP-address
        std::string IPAddr = inet_ntoa(caller->sin_addr);
        //Try to get the hosts name
        struct hostent *he = gethostbyaddr(&caller->sin_addr, sizeof ( struct in_addr), AF_INET);
        //assemble a message
        msg << "Request from host ";
        //Add host name if we have one
        if (he) msg << he->h_name << " ";
        //Add ip-address and port
        msg << "(" << IPAddr << ") on port " << port;
        //Deliver the message
        ServerSvc->Message(MSG::DEBUG,msg.str()); msg.str("");
      }
      
      //Prepare the dispatch thread argument structure making a copy of the
      //request, so it sticks around if this thread is gone
      DispatchThreadArguments DpThreadArgs(ServerSvc, new svc_req(*rqstp), NULL, NULL);

      //Check for input data to the request
      switch (rqstp->rq_proc){

        case ONCRPC_GETEVENT_PROC: {
          //Create structure to hold request arguments on the head so we can
          //forward it to the dispatch thread.
          //NOTE: This is a C-struct that MUST be initalized.
          //It will be cleaned up by the dispatch thread
          EventRequest* evtReq = new EventRequest;
          evtReq->EventNumber=-2;evtReq->RunNumber=-2;evtReq->StreamName=NULL;

          //Retrieve the request arguments
          if (!svc_getargs(rqstp->rq_xprt,(xdrproc_t)xdr_event_req,(caddr_t)evtReq)){
              checkResult(errno,"creating dispatch tread arguments from GETEVENT call",ServerSvc);
              //tell the client his request failed
              svcerr_decode(rqstp->rq_xprt);
              //return immediately from this request
              return ;
           }

          //Set the event request argument
          DpThreadArgs.evtReq = evtReq;
          break;
        
        }  case ONCRPC_SETEVENT_PROC: {
          //Create structure to hold request arguments on the HEAP so we can
          //forward it to the dispatch thread.
          //NOTE: This is a C-struct that MUST be initalized
          //It will be cleaned up by the dispatch thread
          Event* evt = new Event;
          evt->EventNumber=-2;evt->RunNumber=-2;evt->StreamName=NULL;evt->NBytes=0;evt->EventData=NULL;

          //Retrieve the request arguments
          if (!svc_getargs(rqstp->rq_xprt,(xdrproc_t)xdr_event,(caddr_t)evt)){
              checkResult(errno,"creating dispatch tread arguments from SETEVENT call",ServerSvc);
              //tell the client his request failed
              svcerr_decode(rqstp->rq_xprt);
              //return immediately from this request
              return ;
           }

          //Set the event request argument
          DpThreadArgs.evt = evt;
          break;
        }
      }
        
      //Check for errors in all pthread functions
      int retVal = 0;

      //Generate thread attributes
      pthread_attr_t attr; retVal = pthread_attr_init (&attr);
      if ( ! checkResult(retVal,"request handler initializing thread attributes",ServerSvc)) return ;

      //Removing the limit on the thread memory usage as a test. Suspect that some threads do not have enough memory to finish and therefore eat up all the memory.
      //retVal = pthread_attr_setstacksize(&attr,10*PTHREAD_STACK_MIN);
      if ( ! checkResult(retVal,"request handler setting thread stacksize",ServerSvc)) return ;
      
      //NOTE: All threads are first created joinable, so we can wait for the to
      //finish using pthread_detach. Yet, when the thread removes itself from
      //the ThreadCollection, it will detach itself, so no memory is lost.
      retVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
      if ( ! checkResult(retVal,"request handler setting thread detach state",ServerSvc)) return;

      //Create a new thread
      pthread_t dispatchThread;
      retVal = pthread_create(&dispatchThread,&attr,&ONCRPCDispatchThread,new DispatchThreadArguments(DpThreadArgs));
      if ( ! checkResult(retVal,"request handler creating dispatch thread",ServerSvc)) return;

      //And wait till it has registered itself with the ThreadCollection
      dispatchThreads->WaitAdd();

      //release thread attributs
      retVal = pthread_attr_destroy(&attr);
      if ( ! checkResult(retVal,"request handler destroying thread attributes",ServerSvc)) return;

    } catch (std::exception &e){
      std::ostringstream msg; msg << "Caught exception in RequestHandler: " << e.what();
      ServerSvc->Message(MSG::ERROR,msg.str());
    }
  }

  /**
   * This cleanup handler is called whenever the server thread exits
   */
  void ONCRPCCleanupHandler(void* args){
    //Since this cleanup handler is associated with the IServer* 
    //we will get exactly this as an argument
    ((JiveXML::IServer*)args)->Message(MSG::INFO,"Finished ONCRPC server thread");
    //Also call the servers callback
    ((JiveXML::IServer*)args)->ServerThreadStopped();
  }

  /**
   * Main server therad
   */
  void* ONCRPCServerThread( void* args ){

    //Get a pointer to the Service that started this thread
    JiveXML::IServer* const ServerSvc  = (*( ServerThreadArguments* )args).ServerPtr;
    //Get the port number from the thread arguments
    const int PortNumber = (*( ServerThreadArguments* )args).PortNumber;

    //We've got the arguments and can delete the struct
    delete (ServerThreadArguments*)args;
    
    //Save the arguments in a thread-specific keys
    //Once the thread exits, the specified cleanup handler is called,
    //which in our case is used to notify the IServer of the thread termination
    int retVal = 0;
    retVal = pthread_key_create(&ServerSvcKey,ONCRPCCleanupHandler);
    if (! checkResult(retVal,"server thread creating thread-specific key",ServerSvc)) pthread_exit(NULL);
    retVal = pthread_setspecific(ServerSvcKey, ServerSvc);
    if (! checkResult(retVal,"server thread setting thread-specific key",ServerSvc)) pthread_exit(NULL);

    try {

      //Also install cleanup handlers if they are given as arguments

      //Send a message to the messaging function of the service
      ServerSvc->Message(MSG::INFO,"Started ONCRPC server thread");

      //Check wether the runServerThread flag is set
      if (!ServerSvc->GetRunServerFlag()){
        ServerSvc->Message(MSG::WARNING,"The run server flag is not set - stopping server thread immediately");
        pthread_exit(NULL);
      }

      /**
       * Check wether the the portmapper is up and if it already has another instance
       * of this server registered
       */
      //Get my address
      struct sockaddr_in my_addr; get_myaddress(&my_addr);
      //Try get the portnumber of that process
      unsigned short port = pmap_getport(&my_addr, ONCRPCSERVERPROG,ONCRPCSERVERVERS,IPPROTO_TCP);
      //Check if some program has already been registered
      if ( port != 0 ){
        //First report it
        std::ostringstream msg; msg << "Program with program number " << ONCRPCSERVERPROG 
                                   << " already registered with portmapper on local host";
        ServerSvc->Message(MSG::INFO,msg.str());
        //Next check if it is alive by creating a client and calling its NULLPROC
        CLIENT* client = clnt_create("localhost", ONCRPCSERVERPROG,ONCRPCSERVERVERS, "tcp");
        if (client != NULL){
          struct timeval timeout; timeout.tv_sec = 1; timeout.tv_usec = 0;
          clnt_stat ret = clnt_call(client, NULLPROC, (xdrproc_t)xdr_void, NULL, (xdrproc_t)xdr_void, NULL, timeout);
          if (ret == RPC_SUCCESS){
            //So we already have a server, and it is alive -- then we don't start
            ServerSvc->Message(MSG::ERROR,"Server exists and is alive on local host - stopping this thread");
            pthread_exit(NULL);
          } else ServerSvc->Message(MSG::WARNING,"Existing server does not respond");
        } else ServerSvc->Message(MSG::WARNING,"Can not create client for existing server");

        //Now just force a clearing of the portmap entry
        ServerSvc->Message(MSG::WARNING,"Clearing existing portmap entry!");
        pmap_unset(ONCRPCSERVERPROG, ONCRPCSERVERVERS);

        //Finally clear the error entry by trying to get the port number again
        port = pmap_getport(&my_addr, ONCRPCSERVERPROG,ONCRPCSERVERVERS,IPPROTO_TCP);
      }

      //We should have left either the first or the second call to pmap_getport
      // with "program not registered" , which is returned as RPC_PROGNOTREGISTERD
      //or RPC_SYSTEMERROR. However, we could also have failed contacting the portmapper
      if ((rpc_createerr.cf_stat != RPC_PROGNOTREGISTERED) &&
          (rpc_createerr.cf_stat != RPC_SYSTEMERROR )){
        ServerSvc->Message(MSG::ERROR,clnt_spcreateerror("Failed querying portmapper on local host for existing servers"));
        pthread_exit(NULL);
      } 

      //The socket to connect to, not bound to a specifc port by default
      int server_socket = RPC_ANYSOCK;

      //Check if a specific port was requested
      if (PortNumber != 0){

        //Create a socket
        if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) != 0) {
          checkResult(errno,"server thread creating socket",ServerSvc);
          pthread_exit(NULL);
        }

        //Create an inet address with the given port number
        struct sockaddr_in server_addr;    
        server_addr.sin_family = AF_INET;          //IPv4 address
        server_addr.sin_addr.s_addr = INADDR_ANY;  //will bind all interfaces
        server_addr.sin_port = htons(PortNumber);  //port number in network byte order   
        memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero)); //zero padding

        //Now bind the socket to that inet address
        if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) != 0){
          std::ostringstream msg; msg << "server thread binding socket to port " << PortNumber;
          checkResult(errno,msg.str(),ServerSvc);
          pthread_exit(NULL);
        }
        std::ostringstream msg; msg << "Successfully bound port " << PortNumber;
        ServerSvc->Message(MSG::INFO,msg.str());
      }
      
      //Get a transport handle using default buffer sizes
      SVCXPRT * transp = svctcp_create(server_socket, 0, 0);

      //Check result
      if (transp == NULL) {
        ServerSvc->Message(MSG::ERROR,"Opening TCP socket failed");
        pthread_exit(NULL);
      }

      //Next register our server with the RPC daemon
#ifndef __APPLE__
      if (! svc_register(transp,ONCRPCSERVERPROG,ONCRPCSERVERVERS,ONCRPCRequestHandler,IPPROTO_TCP)){
#else
	  if (! svc_register(transp,ONCRPCSERVERPROG,ONCRPCSERVERVERS,(void (*)())ONCRPCRequestHandler,IPPROTO_TCP)){
#endif
        ServerSvc->Message(MSG::ERROR,"Could not register ONCRPC Server with RPC daemon");
        checkResult(errno,"registering ONCRPC Server with RPC daemon",ServerSvc);
        pthread_exit(NULL);
      }
     
      //A set of socket file descriptors
      fd_set SocketFileDescriptorSet;
      //Size of the socket handle table
      int NFileDescriptors = FD_SETSIZE;

      //Count the number of requests we have received while running
      unsigned long NRequests = 0;

      do {  

        ServerSvc->Message(MSG::VERBOSE,"Waiting for call...");

        //Get the file descriptor set
        SocketFileDescriptorSet = svc_fdset;

        //Wait for input on any of these using select:
        // - first NULL: not cecking for readiness to write to socket
        // - second NULL: not checking for socket conditions
        // - third NULL: timeout - wait forever
        int ret = select(NFileDescriptors,&SocketFileDescriptorSet,NULL,NULL,NULL);
        // Check result
        switch (ret){
          // an error occured
          case -1: 
            // If just an interrupt continue
            if ( errno == EINTR ) continue ;
            // Else show error
            checkResult(errno,"server thread returning from select",ServerSvc);
            return NULL;
          // a timeout occured - break the loop
          case 0:
            break ;
          // activity on one of the sockets
          default:
            //Count number of requests
            ++NRequests;
            // Get the requests from the sockets
            svc_getreqset(&SocketFileDescriptorSet);
        }
      } while ( ServerSvc->GetRunServerFlag() );

      ServerSvc->Message(MSG::DEBUG,"Serving loop finished");

      //Unregister the service, so no clients can connect anymore
      svc_unregister(ONCRPCSERVERPROG,ONCRPCSERVERVERS);
      
      //Also stop all the dispatching threads, which need to finish before this
      //one. Otherwise the server will become invalid, before all requests are
      //served
      
      //Tell how many threads we are waiting for
      std::ostringstream msg; msg << "Waiting for " << dispatchThreads->NumberOfThreads() << " open dispatch threads to terminate ... ";
      ServerSvc->Message(MSG::INFO,msg.str());
      //Then wait for them all to finish
      dispatchThreads->JoinAll();
      //Tell when we are done
      ServerSvc->Message(MSG::INFO, " ... finished all dispatch threads");

      //Now that the threads are stopped we can finish the transport protocol and
      //close the port
      svc_destroy(transp);

      //Return number of requests - as all local variables will disappear, 
      //the return value needs to be allocated on the heap;
      unsigned long* NReqPtr = new unsigned long(NRequests);
      //Terminate this thread
      pthread_exit(NReqPtr);

    } catch (std::exception &e){
      std::ostringstream msg; msg << "Caught exception in ServerThread: " << e.what();
      ServerSvc->Message(MSG::ERROR,msg.str());
    }

    return NULL;
  }
} //namespace

