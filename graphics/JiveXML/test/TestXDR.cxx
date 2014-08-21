/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include <rpc/rpc.h>
#include "../JiveXML/ONCRPCServer.h"
#include "../JiveXML/ONCRPCXDRProcs.h"
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

using namespace JiveXML;

int main( int argc, char** argv){

  long BUFSIZE=200000;
  XDR xdrs;
  char buffer[BUFSIZE];

  //Make an event request 
  EventRequest inReq;
  inReq.EventNumber = 3;
  inReq.RunNumber = 4;
  inReq.StreamName = "Stream3";
  //Make an xdr stream to encode into
  xdrmem_create(&xdrs, buffer, BUFSIZE, XDR_ENCODE);
  if (! xdr_event_req(&xdrs, &inReq)) 
     std::cout << "Encoding event request failed!" << std::endl;
  else  std::cout << "Encoding event request okay!" << std::endl;

  //Create output structure
  EventRequest outReq; outReq.StreamName=NULL;
  //Now decode from same stream
  xdrmem_create(&xdrs, buffer, BUFSIZE, XDR_DECODE);
  if (! xdr_event_req(&xdrs, &outReq)) 
     std::cout << "Decoding event request failed!" << std::endl;
  else  std::cout << "Decoding event request okay!" << std::endl;
  
  std::cout << "Returned output is: " 
             << " Event: " << outReq.EventNumber 
             << " Run: " << outReq.RunNumber 
             << " Stream: " << outReq.StreamName  << std::endl;

  xdr_free((xdrproc_t)xdr_event_req,(caddr_t)&outReq);

  //Make an event
  Event* inEvt = new Event;
  inEvt->EventNumber = 4;
  inEvt->RunNumber = 5;
  inEvt->StreamName = "Stream4";
  inEvt->EventData="<MY MUCH MORE AND EVEN MORE DATA>";
  inEvt->NBytes=strlen(inEvt->EventData)+1;
  //Make an xdr stream to encode into
  xdrmem_create(&xdrs, buffer, BUFSIZE, XDR_ENCODE);
  if (! xdr_event(&xdrs, inEvt)) 
     std::cout << "Encoding event failed!" << std::endl;
  else  std::cout << "Encoding event okay!" << std::endl;

  delete inEvt;

  //Create output structure
  Event outEvt;
  //Note that both string parameters need to be set to NULL
  //otherwise XDR assumes memory has been allocated
  outEvt.StreamName=NULL;
  outEvt.EventData=NULL;
  outEvt.EventNumber=-2;
  outEvt.RunNumber=-2;
  outEvt.NBytes=0;
  //Free the storage
  //Now decode from same stream
  xdrmem_create(&xdrs, buffer, BUFSIZE, XDR_DECODE);
  if (! xdr_event(&xdrs, &outEvt)) 
     std::cout << "Decoding event failed!" << std::endl;
  else  std::cout << "Decoding event okay!" << std::endl;
  
  std::cout << "Returned output is: " 
             << " Event: " << outEvt.EventNumber 
             << " Run: " << outEvt.RunNumber 
             << " Data: " << outEvt.EventData
             << " Stream: " << outEvt.StreamName  << std::endl;


  xdr_free((xdrproc_t)xdr_event,(caddr_t)&outEvt);

  return 0;
}
