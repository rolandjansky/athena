/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TokenScatterer.h"
#include "AthenaInterprocess/ProcessGroup.h"

#include "AthenaKernel/IEventShare.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IFileMgr.h"

#include <sys/stat.h>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#include "yampl/SocketFactory.h"
#include <queue>
#include <fstream>
#include <cstdlib>

TokenScatterer::TokenScatterer(const std::string& type
			       , const std::string& name
			       , const IInterface* parent)
  : AthenaMPToolBase(type,name,parent)
  , m_processorChannel("")
  , m_eventRangeChannel("")
  , m_doCaching(false)
{
  m_subprocDirPrefix = "token_scatterer";
  declareProperty("ProcessorChannel", m_processorChannel);
  declareProperty("EventRangeChannel", m_eventRangeChannel);
  declareProperty("DoCaching",m_doCaching);
}

TokenScatterer::~TokenScatterer()
{
}

StatusCode TokenScatterer::initialize()
{
  msg(MSG::DEBUG) << "In initialize" << endreq;

  StatusCode sc = AthenaMPToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  return StatusCode::SUCCESS;
}

StatusCode TokenScatterer::finalize()
{
  return StatusCode::SUCCESS;
}

int TokenScatterer::makePool(int maxevt, int nprocs, const std::string& topdir)
{
  msg(MSG::DEBUG) << "In makePool " << getpid() << endreq;

  if(maxevt < -1) {
    msg(MSG::ERROR) << "Invalid number of events requested: " << maxevt << endreq;
    return -1;
  }

  if(topdir.empty()) {
    msg(MSG::ERROR) << "Empty name for the top directory!" << endreq;
    return -1;
  }

  m_nprocs = (nprocs==-1?sysconf(_SC_NPROCESSORS_ONLN):nprocs);
  m_subprocTopDir = topdir;

  // Create the process group with only one process and map_async bootstrap
  m_processGroup = new AthenaInterprocess::ProcessGroup(1);
  msg(MSG::INFO) << "Token Scatterer process created" << endreq;
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_BOOTSTRAP))
    return -1;

  return 1;  
}

StatusCode TokenScatterer::exec()
{
  msg(MSG::DEBUG) << "In exec " << getpid() << endreq;

  if(mapAsyncFlag(AthenaMPToolBase::FUNC_EXEC))
    return StatusCode::FAILURE;

  // Set exit flag on reader
  if(m_processGroup->map_async(0,0)){
    msg(MSG::ERROR) << "Unable to set exit to the reader" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void TokenScatterer::subProcessLogs(std::vector<std::string>& filenames)
{
  filenames.clear();
  boost::filesystem::path reader_rundir(m_subprocTopDir);
  reader_rundir/= boost::filesystem::path(m_subprocDirPrefix);
  filenames.push_back(reader_rundir.string()+std::string("/AthenaMP.log"));
}

AthenaMP::AllWorkerOutputs_ptr TokenScatterer::generateOutputReport()
{
  AthenaMP::AllWorkerOutputs_ptr jobOutputs(new AthenaMP::AllWorkerOutputs());
  return jobOutputs;
}

AthenaInterprocess::ScheduledWork* TokenScatterer::bootstrap_func()
{
  int* errcode = new int(1); // For now use 0 success, 1 failure
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = (void*)errcode;
  outwork->size = sizeof(int);
  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...

  // Reader dir: mkdir 
  boost::filesystem::path reader_rundir(m_subprocTopDir);
  reader_rundir /= boost::filesystem::path(m_subprocDirPrefix);

  if(mkdir(reader_rundir.string().c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)==-1) {
    msg(MSG::ERROR) << "Unable to make reader run directory: " << reader_rundir.string() << ". " << strerror(errno) << endreq;
    return outwork;
  }

  // Redirect logs
  if(redirectLog(reader_rundir.string()))
    return outwork;

  msg(MSG::INFO) << "Logs redirected in the AthenaMP ByteStream reader PID=" << getpid() << endreq;
     
  // Update Io Registry   
  if(updateIoReg(reader_rundir.string()))
    return outwork;

  msg(MSG::INFO) << "Io registry updated in the AthenaMP ByteStream reader PID=" << getpid() << endreq;

  // _______________________ Handle saved PFC (if any) ______________________
  boost::filesystem::path abs_reader_rundir = boost::filesystem::absolute(reader_rundir);
  if(handleSavedPfc(abs_reader_rundir))
    return outwork;

  // Reopen file descriptors
  if(reopenFds())
    return outwork;

  msg(MSG::INFO) << "File descriptors re-opened in the AthenaMP ByteStream reader PID=" << getpid() << endreq;

  // ________________________ I/O reinit ________________________
  if(!m_ioMgr->io_reinitialize().isSuccess()) {
    msg(MSG::ERROR) << "Failed to reinitialize I/O" << endreq;
    return outwork;
  } else {
    msg(MSG::DEBUG) << "Successfully reinitialized I/O" << endreq;
  }

  // Start the event selector 
  IService* evtSelSvc = dynamic_cast<IService*>(m_evtSelector);
  if(!evtSelSvc) {
    msg(MSG::ERROR) << "Failed to dyncast event selector to IService" << endreq;
    return outwork;
  }
  if(!evtSelSvc->start().isSuccess()) {
    msg(MSG::ERROR) << "Failed to restart the event selector" << endreq;
    return outwork;
  } else {
    msg(MSG::DEBUG) << "Successfully restarted the event selector" << endreq;
  }

  // Reader dir: chdir 
  if(chdir(reader_rundir.string().c_str())==-1) {
    msg(MSG::ERROR) << "Failed to chdir to " << reader_rundir.string() << endreq;
    return outwork;
  }

  // Declare success and return
  *errcode = 0;
  return outwork;
}

AthenaInterprocess::ScheduledWork* TokenScatterer::exec_func()
{
  msg(MSG::INFO) << "Exec function in the AthenaMP Token Scatterer PID=" << getpid() << endreq;

  yampl::ISocketFactory* socketFactory = new yampl::SocketFactory();
  // Create a socket to communicate with the Pilot
  yampl::ISocket* socket2Pilot = socketFactory->createClientSocket(yampl::Channel(m_eventRangeChannel.value(),yampl::LOCAL),yampl::MOVE_DATA);
  msg(MSG::DEBUG) << "Created CLIENT socket for communicating Event Ranges with the Pilot" << endreq;
  // Create a socket to communicate with TokenProcessors
  yampl::ISocket* socket2Processor = socketFactory->createServerSocket(yampl::Channel(m_processorChannel.value(),yampl::LOCAL_PIPE),yampl::MOVE_DATA);
  msg(MSG::DEBUG)<< "Created SERVER socket to token processors: " << m_processorChannel.value() << endreq;
  // Create a socket to communicate with the token extractor
  yampl::ISocket* socket2Extractor = socketFactory->createClientSocket(yampl::Channel("TokenExtractorChannel",yampl::LOCAL_PIPE),yampl::MOVE_DATA);
  msg(MSG::DEBUG) << "Created CLIENT socket to the extractor: TokenExtractorChannel" << endreq;

  bool all_ok=true;
  int procReportPending(0);  // Keep track of how many output files are yet to be reported by Token Processors

  // Communication protocol with the Pilot
  std::string strReady("Ready for events");
  std::string strStopProcessing("No more events");

  while(true) {
    // NO CACHING MODE: first get a request from one of the processors and only after that request the next event range from the pilot
    if(!m_doCaching) {
      msg(MSG::DEBUG) << "Start waiting for event range request from one of the processors" << endreq;
      while(getNewRangeRequest(socket2Processor,socket2Pilot,procReportPending).empty()) {}
      msg(MSG::DEBUG) << "One of the processors is ready for the next range" << endreq;
    }    

    // Signal the Pilot that AthenaMP is ready for event processing
    void* ready_message = malloc(strReady.size());
    memcpy(ready_message,strReady.data(),strReady.size());
    socket2Pilot->send(ready_message,strReady.size());
    void* eventRangeMessage;
    ssize_t eventRangeSize = socket2Pilot->recv(eventRangeMessage);
    std::string eventRange((const char*)eventRangeMessage,eventRangeSize);
    size_t carRet = eventRange.find('\n');
    if(carRet!=std::string::npos)
      eventRange = eventRange.substr(0,carRet);

    // Break the loop if no more ranges are expected
    if(eventRange.compare(strStopProcessing)==0) {
      msg(MSG::DEBUG) << "Stopped the loop. Last message from the Event Range Channel: " << eventRange << endreq;
      break;
    }
    msg(MSG::DEBUG) << "Got Event Range from the pilot: " << eventRange << endreq;

    // Parse the Event Range string 
    // Expected the following format: [{KEY:VALUE[,KEY:VALUE]}]
    // Firs get rid of the leading '[{' and the trailing '}]'
    if(eventRange.find("[{")==0) eventRange=eventRange.substr(2);
    if(eventRange.rfind("}]")==eventRange.size()-2) eventRange=eventRange.substr(0,eventRange.size()-2);

    std::map<std::string,std::string> eventRangeMap;
    size_t startpos(0);
    size_t endpos = eventRange.find(',');
    while(endpos!=std::string::npos) {
      // Get the Key-Value pair
      std::string keyValue(eventRange.substr(startpos,endpos-startpos));
      size_t colonPos = keyValue.find(":");
      std::string strKey = keyValue.substr(0,colonPos);
      std::string strVal = keyValue.substr(colonPos+1);
      trimRangeStrings(strKey);
      trimRangeStrings(strVal);
      eventRangeMap[strKey]=strVal;
      // Next iteration
      startpos = endpos+1;
      endpos = eventRange.find(',',startpos);
    }
    // Get the final Key-Value pair
    std::string keyValue(eventRange.substr(startpos));
    size_t colonPos = keyValue.find(":");
    std::string strKey = keyValue.substr(0,colonPos);
    std::string strVal = keyValue.substr(colonPos+1);
    trimRangeStrings(strKey);
    trimRangeStrings(strVal);
    eventRangeMap[strKey]=strVal;
			 
    if(eventRangeMap.find("eventRangeID")==eventRangeMap.end()
       || eventRangeMap.find("startEvent")==eventRangeMap.end()
       || eventRangeMap.find("lastEvent")==eventRangeMap.end()
       || eventRangeMap.find("GUID")==eventRangeMap.end()) {
      msg(MSG::ERROR) << "Wrong format of the event range: " << eventRange << endreq;
      all_ok = false;
      break;
    }
    else {
      msg(MSG::DEBUG) << "*** Decoded Event Range ***" << endreq;
      std::map<std::string,std::string>::const_iterator it = eventRangeMap.begin(),
	itEnd = eventRangeMap.end();
      for(;it!=itEnd;++it)
	msg(MSG::DEBUG) << it->first << ":" << it->second << endreq;
      msg(MSG::DEBUG) << "*** ***" << endreq;
    }

    std::string rangeID = eventRangeMap["eventRangeID"];
    std::string guid = eventRangeMap["GUID"];
    int startEvent = std::atoi(eventRangeMap["startEvent"].c_str());
    int lastEvent = std::atoi(eventRangeMap["lastEvent"].c_str());
    if(rangeID.empty() || lastEvent < startEvent) {
      msg(MSG::ERROR) << "Wrong values in the Event Range: " << eventRange << endreq;
      all_ok = false;
      break;
    }

    // Compose a string to be sent to the TokenExtractor
    // Format GUID,Event1,Event2,...EventN
    std::ostringstream streamMessage2Extractor;
    for(int i(startEvent); i<=lastEvent; ++i)
      streamMessage2Extractor << "," << i;
    std::string message2Extractor = guid + streamMessage2Extractor.str();
    msg(MSG::DEBUG) << "Composed a message to Token Extractor : " << message2Extractor << endreq;

    // Send the message to Token Extractor
    char *requestBuffer(0), *responseBuffer(0), *message2Processor(0);
    requestBuffer = (char*)malloc(message2Extractor.size());
    memcpy(requestBuffer,message2Extractor.data(),message2Extractor.size());
    socket2Extractor->send(requestBuffer,message2Extractor.size());
    msg(MSG::DEBUG) << "Sent message to the Token Extractor" << endreq;
    
    // Get the response: list of tokens
    ssize_t responseSize = socket2Extractor->recv(responseBuffer);    
    if(responseSize==1) {
      // Empty message received. Report an ERROR and break the loop
      msg(MSG::ERROR) << "Token Extractor failed to deliver tokens for the RangeID=" << rangeID << endreq;
      all_ok = false;
      break;
    }
    std::string responseStr(responseBuffer,responseSize);
    msg(MSG::DEBUG) << "Received response from the token extractor with the tokens: " << responseStr << endreq;

    // CACHING MODE: first get an event range from the pilot, transform it into the tokens
    // and only after that wait for a new range request by one of the processors
    if(m_doCaching) {
      msg(MSG::DEBUG) << "Start waiting for event range request from one of the processors" << endreq;
      while(getNewRangeRequest(socket2Processor,socket2Pilot,procReportPending).empty()) {}
      msg(MSG::DEBUG) << "One of the processors is ready for the next range" << endreq;
    }
    
    // Send to the Processor: RangeID,evtToken[,evtToken] 
    std::string message2ProcessorStr(rangeID+std::string(",")+responseStr);
    message2Processor = (char*)malloc(message2ProcessorStr.size());
    memcpy(message2Processor,message2ProcessorStr.data(),message2ProcessorStr.size());
    socket2Processor->send(message2Processor,message2ProcessorStr.size());
    procReportPending++;
    msg(MSG::DEBUG) << "Sent response to the processor : " << message2ProcessorStr << endreq;
  }

  // Send an empty message to the Token Extractor, by this way informing that it can exit
  // Here (and everywhere) the empty message is in fact a dummy message with size=1, as I'm having troubles sending 0 size messages
  //
  // NB!!! This should be done only IF Token Scatterer and Token Extractor run on the same machine.
  //       I.e. we have one extractor for each scatterer. Otherwise the scatterer should not be stopping the extractor
  //
  void* emptyMessage = malloc(1);
  socket2Extractor->send(emptyMessage,1);

  if(all_ok) {
    // We are done distributing event tokens. 
    // Tell the workers that the event processing is over
    // i.e. send out m_nprocs empty messages
    void* emptyMess4Processor(0);
    if(!m_doCaching) {
      // We already have one processor waiting for the answer
      emptyMess4Processor = malloc(1);
      socket2Processor->send(emptyMess4Processor,1);
    }
    for(int i(0); i<(m_doCaching?m_nprocs:m_nprocs-1); ++i) {
      while(getNewRangeRequest(socket2Processor,socket2Pilot,procReportPending).empty()) {}
      emptyMess4Processor = malloc(1);
      socket2Processor->send(emptyMess4Processor,1);
    }
    
    // Final round of colecting output file names from processors
    while(procReportPending>0) {
      std::string strProcessorRequest = getNewRangeRequest(socket2Processor,socket2Pilot,procReportPending);
      if(!strProcessorRequest.empty()) {
	msg(MSG::ERROR) << "Unexpected message received from a processor at this stage : " << strProcessorRequest << endreq;
	all_ok = false;
	break;
      }
    }
  }

  if(m_appMgr->stop().isFailure()) {
    msg(MSG::ERROR) << "Unable to stop AppMgr" << endreq; 
    all_ok=false;
  }
  else { 
    if(m_appMgr->finalize().isFailure()) {
      msg(MSG::ERROR) << "Unable to finalize AppMgr" << endreq;
      all_ok=false;
    }
  }
  
  int* errcode = new int(all_ok?0:1); // For now use 0 success, 1 failure
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = (void*)errcode;
  outwork->size = sizeof(int);
  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...
  delete socket2Extractor;
  delete socket2Processor;
  delete socket2Pilot;
  delete socketFactory;

  return outwork;
}

AthenaInterprocess::ScheduledWork* TokenScatterer::fin_func()
{
  // Dummy
  int* errcode = new int(0); 
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = (void*)errcode;
  outwork->size = sizeof(int);
  return outwork;
}

void TokenScatterer::trimRangeStrings(std::string& str)
{
  size_t i(0);
  // get rid of leading spaces
  while(i<str.size() && str[i]==' ') i++;
  if(i) str = str.substr(i);

  if(str.empty()) return;  // Corner case: string consists only of spaces

  // get rid of trailing spaces
  i=str.size()-1;
  while(str[i]==' ') i--;
  if(i) str = str.substr(0,i+1);
  
  // the string might be enclosed by either
  // "u\'" and "\'"
  // or
  // "\"" and "\""
  // Get rid of them!
  if(str.find("u\'")==0) {
    str = str.substr(2);
    if(str.rfind("\'")==str.size()-1) {
      str = str.substr(0,str.size()-1);
    }
  }
  else if(str.find("\"")==0) {
    str = str.substr(1);
    if(str.rfind("\"")==str.size()-1) {
      str = str.substr(0,str.size()-1);
    }
  }
}

std::string TokenScatterer::getNewRangeRequest(yampl::ISocket* socket2Processor
					       , yampl::ISocket* socket2Pilot
					       , int& procReportPending)
{
  msg(MSG::DEBUG) << "In getNewRangeRequest ..."  << endreq;
  void* processor_request(0);
  ssize_t processorRequestSize = socket2Processor->recv(processor_request);
  std::string strProcessorRequest((const char*)processor_request,processorRequestSize);
  msg(MSG::DEBUG) << "Received request from a processor: " << strProcessorRequest << endreq;
  // Decode the request. If it contains output file name then pass it over to the pilot and keep waiting
  if(strProcessorRequest.find('/')==0) {
    void* outpFileNameMessage = malloc(strProcessorRequest.size());
    memcpy(outpFileNameMessage,strProcessorRequest.data(),strProcessorRequest.size());
    socket2Pilot->send(outpFileNameMessage,strProcessorRequest.size());
    procReportPending--;
    return std::string("");
  }
  return strProcessorRequest;
}
