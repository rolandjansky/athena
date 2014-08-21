/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <JiveXML/ONCRPCServerProcs.h>
#include <JiveXML/ONCRPCXDRProcs.h>
#include <JiveXML/ONCRPCServer.h>

#include <JiveXML/IServer.h>
#include <JiveXML/EventStream.h>

#include <sstream>
#include <errno.h>

namespace JiveXML {

  /**
   * Simple result checking routine, that will output an errorMsg throught the
   * ServerSvc if there was an error. Returns false on error;
   */
  bool checkResult( const int RetVal, const std::string Module, IMessage* const ServerSvc){

    //Immediately return on no error
    if (RetVal == 0) return true ;

    //If there was an error, assemble the message
    std::ostringstream msg;
    msg << "Error in " << Module << ": " << strerror(RetVal);
    
    //Output the message via the ServerSvc
    ServerSvc->Message(MSG::ERROR,msg.str());

    //In case of error return false
    return false ;
  }

  /**
   * Implementation of NULLPROC 
   * Return nothing - server has just been pinged
   */
  void ReturnNull( SVCXPRT* transp, IServer* const ServerSvc ){
    if (!svc_sendreply(transp, (xdrproc_t)xdr_void, 0))
      //check global errno variable on result
      checkResult(errno,"dispatch thread sending reply to NULLPROC call",ServerSvc);
  }

  /**
   * Implementation of ONCRPC_GETSTATUS_PROC 
   * Return the current athena status in XDR representation
   */
  void ReturnState( SVCXPRT* transp, IServer* const ServerSvc ){
    int state = ServerSvc->GetState();
    if (!svc_sendreply(transp, (xdrproc_t)xdr_int, (caddr_t)&state))
      //check global errno variable on result
      checkResult(errno,"dispatch thread sending reply to GETSTATUS call",ServerSvc);
  }

  /**
   * Return the names of all the streams that are available at the server
   */
  void ReturnStreams( SVCXPRT* transp, IServer* const ServerSvc ){

    // First of all get the list of names
    std::vector<std::string> StreamNames = ServerSvc->GetStreamNames();

    // Create a new XDR data structure to fill in the streams
    Streams XDRStreams;

    // Fill in number of streams
    XDRStreams.NStreams = StreamNames.size();

    //Create a new array with the stream names
    XDRStreams.StreamNames = (char**) malloc(StreamNames.size()*sizeof(char*));

    //Fill in all the names themselves
    for ( unsigned int iName = 0; iName < StreamNames.size(); ++iName)
      XDRStreams.StreamNames[iName] = strdup(StreamNames[iName].c_str());

    //return that object
    if (!svc_sendreply(transp, (xdrproc_t)xdr_streams, (caddr_t)&XDRStreams))
      checkResult(errno,"dispatch thread sending reply to GETSTREAMS call",ServerSvc);

    //Afterwards free the object
    xdr_free((xdrproc_t)xdr_streams,(caddr_t)&XDRStreams);
  }

  /**
   * Return the actual event for a requested stream, only if
   * - the stream is available
   * - this is not the same event as the user had already asked for
   * - otherwise, set flags accordingly in the return structure
   */
  void ReturnEvent( SVCXPRT* transp, const EventRequest* eventReq, IServer* const ServerSvc ){

    //Check if there was an event request
    std::ostringstream msg;
    if (! eventReq ) {
      msg << "Request to return event with void event request structure";
      ServerSvc->Message(MSG::ERROR,msg.str());
      svcerr_systemerr(transp);
      return ;
    }
   
    //Be a bit verbose
    msg << "Request for stream " << eventReq->StreamName
        << " with existing event " << eventReq->EventNumber 
        << " from run " << eventReq->RunNumber;
    //Send message an clear stream
    ServerSvc->Message(MSG::DEBUG,msg.str()); msg.str("");

    //Next get the current eventStreamID from that stream
    EventStreamID currEvtStreamID = ServerSvc->GetEventStreamID(eventReq->StreamName);

    //Also get an event stream ID for the requested event
    EventStreamID reqEvtStreamID(eventReq->EventNumber,eventReq->RunNumber,eventReq->StreamName);

    //We got all the data from the request, so delete it
    xdr_free((xdrproc_t)xdr_event_req,(caddr_t)eventReq);

    //Prepare response structure
    Event event;
    event.isAvailable = false ;
    event.isIdentical = false ;
    event.isCompressed = false ;
    event.EventNumber = currEvtStreamID.EventNumber();
    event.RunNumber = currEvtStreamID.RunNumber();
    event.StreamName = (char*) currEvtStreamID.StreamName().c_str();
    event.NBytes = 0; event.EventData = NULL ;

    //Check if such a stream was currently found
    if (!currEvtStreamID.isValid()) {
      // Sending reply with "no such stream" - i.e unmodified event structure
      msg << "Sending response NO SUCH STREAM "; ServerSvc->Message(MSG::DEBUG,msg.str()); 
      if (!svc_sendreply(transp, (xdrproc_t)xdr_event, (caddr_t)&event))
        checkResult(errno,"dispatch thread sending reply to GETEVENT call",ServerSvc);
      return ;
    } else
      //We have an event in that stream
      event.isAvailable = true ;

    //Check if this is the same event the user already has
    if ( currEvtStreamID.isSameEvent(reqEvtStreamID) ){
      // Set the same event flag
      event.isIdentical = true ;
      // Sending reply with "same event"
      msg << "Sending response SAME EVENT "; ServerSvc->Message(MSG::DEBUG,msg.str()); 
      if (!svc_sendreply(transp, (xdrproc_t)xdr_event, (caddr_t)&event))
        checkResult(errno,"dispatch thread sending reply to GETEVENT call",ServerSvc);
      return ;
    } 

    //Otherwise just get the event itself
    std::string evt = ServerSvc->GetEvent( currEvtStreamID );
    event.EventData = evt.c_str(); 
    event.NBytes = strlen(evt.c_str())+1; //including null-termination character
    //Send reply with an actual event
    msg << "Sending response NEW EVENT (" << event.EventNumber << ","<< event.RunNumber << ")" ;
    ServerSvc->Message(MSG::DEBUG,msg.str()); 
    if (!svc_sendreply(transp, (xdrproc_t)xdr_event, (caddr_t)&event))
      checkResult(errno,"dispatch thread sending reply to GETEVENT call",ServerSvc);

    //No need to use xdr_free to free the returned event object since we did
    //not allocate resources for it (just point the pointers to the proper place).
  }

  /**
   * Set a new event for a given stream
   */
  void SetNewEvent( SVCXPRT* transp, const Event* event, IServer* const ServerSvc ){

    //Check if there was a valid event request
    //NOTE: Event number can be 0, run number can not
    std::ostringstream msg;
    if ( (!event) || (event->EventNumber<0) || (!event->RunNumber) || (!event->StreamName) )  {
      msg << "Request to set new event with void or invalid event structure";
      ServerSvc->Message(MSG::ERROR,msg.str());
      svcerr_systemerr(transp);
      return ;
    }

    //Be a bit verbose
    msg << "Request to set new event  " << event->EventNumber 
        << " from run " << event->RunNumber
        << " for stream " << event->StreamName;
    //Send message an clear stream
    ServerSvc->Message(MSG::DEBUG,msg.str()); msg.str("");

    //The success flag returned to the client 
    //Need to use bool_t (4bytes) instead of bool (1byte)!
    bool_t isSuccess=false;

    //Create a new EventStreamID from the data we have got
    EventStreamID newEvtStreamID(event->EventNumber,event->RunNumber,event->StreamName);

    //Make sure this ID is valid
    if (! newEvtStreamID.isValid()) { 
      msg << "Request to set new event with invalid stream name or event/run number";
      ServerSvc->Message(MSG::ERROR,msg.str());
      if (!svc_sendreply(transp, (xdrproc_t)xdr_bool, (caddr_t)&isSuccess))
        checkResult(errno,"dispatch thread sending reply to SETEVENT call",ServerSvc);
      return ;
    }

    //And make sure there is data
    if ((! event->EventData ) || (strlen(event->EventData) == 0)){
      msg << "Request to set new event with invalid event data";
      ServerSvc->Message(MSG::ERROR,msg.str());
      if (!svc_sendreply(transp, (xdrproc_t)xdr_bool, (caddr_t)&isSuccess))
        checkResult(errno,"dispatch thread sending reply to SETEVENT call",ServerSvc);
      return ;
    }

    //Also make sure it has the proper length
    if ( strlen(event->EventData) != event->NBytes -1 ){
      msg << "Event string length does not match claimed number of bytes (null-termination problem?)";
      ServerSvc->Message(MSG::ERROR,msg.str());
      if (!svc_sendreply(transp, (xdrproc_t)xdr_bool, (caddr_t)&isSuccess))
        checkResult(errno,"dispatch thread sending reply to SETEVENT call",ServerSvc);
      return ;
    }
    //Get the event data
    std::string EvtData(event->EventData);
   
    //Now set the event at the server
    if (ServerSvc->UpdateEventForStream( newEvtStreamID, EvtData).isSuccess())
      //and set the success flag
      isSuccess = true;

    //Show the result
    msg << "Sending response for set new event: ";
    if (isSuccess) msg << " SUCCESS!"; else msg << " FAILED!";
    ServerSvc->Message(MSG::DEBUG,msg.str()); 
    //Send the reply
    if (!svc_sendreply(transp, (xdrproc_t)xdr_bool, (caddr_t)&isSuccess))
      checkResult(errno,"dispatch thread sending reply to SETEVENT call",ServerSvc);

    //Afterwards free the object
    xdr_free((xdrproc_t)xdr_event,(caddr_t)event);
  }
      
} //namespace
