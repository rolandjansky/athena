/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <iomanip>
#include <rpc/rpc.h>
#include "../JiveXML/ONCRPCServer.h"
#include "../JiveXML/ONCRPCXDRProcs.h"
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <cstdlib>

using namespace JiveXML;

//Some global stuff to be reported in the end
long NCalls = 0;  //total number of calls
long NCom = 0;    //total number of calls with server response
long NTransSucc = 0;  //total number of calls with successfull transfer
long NFailed = 0;  //total number of failed calls
long NBytesTot = 0;  //total number of transfered bytes
double NSecTot = 0;  //total number of seconds for transfer

//set the maximum polling intervall in microsecs
unsigned int sleepint=1000000;

//called on sigint
void SigIntFunc( int sig ) {

 std::cout << "\n----------------------------------------------------------" << std::endl;
 std::cout << " Summary:" << std::endl;
 std::cout << " \tTotal: " << NCalls << " calls,\t" << NCom << " answered,\t" << NFailed << " failed,\t";
 std::cout << std::setprecision(4) << 1.*NBytesTot/1024 << "kb transfered in\t"
           << NSecTot << " seconds" << std::endl; 
 std::cout << " \tAvg of " << NCalls << " calls:\t"
            << std::setprecision(3) << 100.*NCom/(NCom+NFailed) << "\% answered,\t"
            << 100.*NFailed/(NCom+NFailed) << "\% failed\t";
 std::cout << std::setprecision(4) << 1.*NBytesTot/1024/NTransSucc << " kb/event with\t"
           << std::setprecision(4) << 1000.*NSecTot/NTransSucc << " millisecs/evt" << std::endl; 
 std::cout << " \tAvg transfer speed:\t" << std::setprecision(4) 
           << 1.*NBytesTot/NSecTot/1024/1024 << " Mb/sec" << std::endl; 
 exit(0);

}

int main( int argc, char** argv){

  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " <host> <instream> [outstream] " << std::endl;
    return 1;
  }

  signal(SIGINT, SigIntFunc);

  //Loop forever
  while ( true ) {
     ++NCalls;
     CLIENT* client = clnt_create(argv[1], ONCRPCSERVERPROG,ONCRPCSERVERVERS, "tcp");
     if (client){
       struct timeval timeout; timeout.tv_sec = 20; timeout.tv_usec = 0;
       clnt_stat ret = clnt_call(client, NULLPROC, (xdrproc_t)xdr_void, NULL,
                                             (xdrproc_t)xdr_void, NULL, timeout);
       //Check if there is a server
       if (ret == RPC_SUCCESS){
         std::cout << std::setw(6) << NCalls << " Found Server" << std::endl;

         //Send an event request for that stream, using -1,-1 to indicate we
         //don't have an event yet
         EventRequest eventReq; eventReq.RunNumber=-1; eventReq.EventNumber=-1; eventReq.StreamName=argv[2];

         //While there is a server call it
         while (ret == RPC_SUCCESS){
           //Wait for random time
           usleep(rand() % sleepint);
           ++NCalls;
           int state = -1;
           ret = clnt_call(client, ONCRPC_GETSTATUS_PROC, (xdrproc_t)xdr_void, NULL,
                           (xdrproc_t)xdr_int, (caddr_t)&state, timeout);
           std::cout << std::setw(6) << NCalls << " Athena state is " << state << std::endl;
           if (ret != RPC_SUCCESS) {
              ++NFailed; 
              std::cout << "\t client call failed with error:" << strerror(errno) << std::endl;
              continue ;
           } else ++NCom;
           
           //Only ask for streams if status is three
           if (state < 3) continue ;
           Streams streams; streams.StreamNames=NULL;
           ret = clnt_call(client, ONCRPC_GETSTREAMS_PROC, (xdrproc_t)xdr_void, NULL,
                           (xdrproc_t)xdr_streams, (caddr_t)&streams, timeout);
           //Failed calling server?
           if (ret != 0){
              ++NFailed;
              std::cout << "\t client call failed with error:" << strerror(errno) << std::endl;
              continue ;
           } else ++NCom;
           std::cout << "\t Server has " << streams.NStreams << " streams :"; 
           //Output names of all streams
           for (int iName = 0; iName < streams.NStreams; ++iName)
             std::cout << " " << streams.StreamNames[iName];
           std::cout << std::endl;
           //Free the storage
           xdr_free((xdrproc_t)xdr_streams,(caddr_t)&streams);


           //Now ask for event from the stream
           std::cout << "\t Sending request with event " << eventReq.EventNumber << " from run " << eventReq.RunNumber << std::endl;

           //Measure real time for call (can't use clock, which only measures
           //CPU time, and does not include I/O  
           //clock_t start, finish;
           //start = clock();
           
           struct timeval start, finish;

           //This structure will hold the return value
           //Note that both string parameters need to be set to NULL
           //otherwise XDR assumes memory has been allocated
           Event event;
           event.isAvailable = false ; event.isIdentical = false ; event.isCompressed = false ;
           event.NBytes = 0; event.EventData = NULL ; event.StreamName=NULL;

           //start timer
           gettimeofday(&start,NULL);
           //call client
           ret = clnt_call(client, ONCRPC_GETEVENT_PROC, (xdrproc_t)xdr_event_req, (caddr_t)&eventReq,
                           (xdrproc_t)xdr_event, (caddr_t)&event, timeout);
           //stop timer
           gettimeofday(&finish,NULL);

           //Failed calling server?
           if (ret != 0){
             ++NFailed;
             std::cout << "\t client call failed with error:" << strerror(errno) << std::endl;
             xdr_free((xdrproc_t)xdr_event,(caddr_t)&event);
             continue ;
           } else ++NCom;
           //Check if the stream is available at the server
           if (!event.isAvailable){
             std::cout << "\t No event available for stream " << eventReq.StreamName << std::endl;
             xdr_free((xdrproc_t)xdr_event,(caddr_t)&event);
             continue ;
           }
           //Check if we already have that event
           if (event.isIdentical){
             std::cout << "\t Already have event" << event.EventNumber << " from run " << event.RunNumber << std::endl;
             xdr_free((xdrproc_t)xdr_event,(caddr_t)&event);
             continue ;
           }
           //Calculate number of seconds it took
           if (finish.tv_usec < start.tv_usec){
            finish.tv_usec += 1000000 ;
            finish.tv_sec -= 1;
           }
           double elapsed = (finish.tv_sec - start.tv_sec) + 1e-6 * (finish.tv_usec - start.tv_usec);

           //Otherwise we have an event
           std::cout << "\t Got event " << event.EventNumber << " from run "
                     << event.RunNumber << " in stream " << event.StreamName << " with " << event.NBytes << " bytes" 
                     << " in " << std::setprecision(6) << elapsed << " secs " << std::endl;
           ++NTransSucc; NBytesTot += event.NBytes; NSecTot += elapsed ;
           //Show only the bit with the event and run number number
           std::string evtcpy = event.EventData;
           std::cout << "\t " << evtcpy.substr(evtcpy.find("<Event"),evtcpy.find("dateTime")+12-evtcpy.find("<Event")) << std::endl;
           //Update event request
           eventReq.EventNumber = event.EventNumber;
           eventReq.RunNumber = event.RunNumber;

           //Check if we shall send back the events
           if (argc !=4) { 
             xdr_free((xdrproc_t)xdr_event,(caddr_t)&event);
             continue ;
           }
           //Pump back the event to the new stream
           event.StreamName = argv[3];
           //returns boolean
           bool_t isSuccess = false;
           gettimeofday(&start,NULL);
           //call client
           ret = clnt_call(client, ONCRPC_SETEVENT_PROC, (xdrproc_t)xdr_event, (caddr_t)&event,
                           (xdrproc_t)xdr_bool, (caddr_t)&isSuccess, timeout);
           //stop timer
           gettimeofday(&finish,NULL);

           //Failed calling server?
           if (ret != 0){
              ++NFailed;
              std::cout << "\t client call failed with error:" << strerror(errno) << std::endl;
              continue ;
           } else ++NCom;
           
           //Calculate number of seconds it took
           if (finish.tv_usec < start.tv_usec){
            finish.tv_usec += 1000000 ;
            finish.tv_sec -= 1;
           }
           
           //Check the return value
           elapsed = (finish.tv_sec - start.tv_sec) + 1e-6 * (finish.tv_usec - start.tv_usec);
           if (isSuccess) std::cout << "\t Successfully set new event for stream " << event.StreamName;
             else std::cout << "\t Failed to set new event for stream " << event.StreamName;
           std::cout  << " in " << std::setprecision(6) << elapsed << " secs " << std::endl;
           
           //Free the storage
           xdr_free((xdrproc_t)xdr_event,(caddr_t)&event);
         }
         //Free also event request
         xdr_free((xdrproc_t)xdr_event,(caddr_t)&eventReq); 
       }
       else {
         ++NFailed;
         std::cout << std::setw(6) << NCalls << " Error: " << clnt_sperrno(ret) << std::endl;
       }
    } else {
      std::cout << std::setw(6) << NCalls << " No Server!" << std::endl;
    }
    usleep(rand() % sleepint);
    //usleep(sleepint);
  }
}
