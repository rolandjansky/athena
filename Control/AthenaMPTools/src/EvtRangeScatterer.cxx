/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EvtRangeScatterer.h"
#include "AthenaInterprocess/ProcessGroup.h"
#include "AthenaInterprocess/SharedQueue.h"

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

EvtRangeScatterer::EvtRangeScatterer(const std::string& type
			       , const std::string& name
			       , const IInterface* parent)
  : AthenaMPToolBase(type,name,parent)
  , m_processorChannel("")
  , m_eventRangeChannel("")
  , m_doCaching(false)
{
  m_subprocDirPrefix = "range_scatterer";
  declareProperty("ProcessorChannel", m_processorChannel);
  declareProperty("EventRangeChannel", m_eventRangeChannel);
  declareProperty("DoCaching",m_doCaching);
}

EvtRangeScatterer::~EvtRangeScatterer()
{
}

StatusCode EvtRangeScatterer::initialize()
{
  ATH_MSG_DEBUG("In initialize");

  StatusCode sc = AthenaMPToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  return StatusCode::SUCCESS;
}

StatusCode EvtRangeScatterer::finalize()
{
  return StatusCode::SUCCESS;
}

int EvtRangeScatterer::makePool(int maxevt, int nprocs, const std::string& topdir)
{
  ATH_MSG_DEBUG("In makePool " << getpid());

  if(maxevt < -1) {
    ATH_MSG_ERROR("Invalid number of events requested: " << maxevt);
    return -1;
  }

  if(topdir.empty()) {
    ATH_MSG_ERROR("Empty name for the top directory!");
    return -1;
  }

  m_nprocs = (nprocs==-1?sysconf(_SC_NPROCESSORS_ONLN):nprocs);
  m_subprocTopDir = topdir;

  // Create the process group with only one process and map_async bootstrap
  m_processGroup = new AthenaInterprocess::ProcessGroup(1);
  ATH_MSG_INFO("Token Scatterer process created");
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_BOOTSTRAP))
    return -1;

  return 1;  
}

StatusCode EvtRangeScatterer::exec()
{
  ATH_MSG_DEBUG("In exec " << getpid());

  if(mapAsyncFlag(AthenaMPToolBase::FUNC_EXEC))
    return StatusCode::FAILURE;

  // Set exit flag on reader
  if(m_processGroup->map_async(0,0)){
    ATH_MSG_ERROR("Unable to set exit to the reader");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void EvtRangeScatterer::subProcessLogs(std::vector<std::string>& filenames)
{
  filenames.clear();
  boost::filesystem::path reader_rundir(m_subprocTopDir);
  reader_rundir/= boost::filesystem::path(m_subprocDirPrefix);
  filenames.push_back(reader_rundir.string()+std::string("/AthenaMP.log"));
}

AthenaMP::AllWorkerOutputs_ptr EvtRangeScatterer::generateOutputReport()
{
  AthenaMP::AllWorkerOutputs_ptr jobOutputs(new AthenaMP::AllWorkerOutputs());
  return jobOutputs;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> EvtRangeScatterer::bootstrap_func()
{
  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = 1; // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);

  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...

  // Reader dir: mkdir 
  boost::filesystem::path reader_rundir(m_subprocTopDir);
  reader_rundir /= boost::filesystem::path(m_subprocDirPrefix);

  if(mkdir(reader_rundir.string().c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)==-1) {
    ATH_MSG_ERROR("Unable to make reader run directory: " << reader_rundir.string() << ". " << strerror(errno));
    return outwork;
  }

  // Redirect logs
  if(redirectLog(reader_rundir.string()))
    return outwork;

  ATH_MSG_INFO("Logs redirected in the AthenaMP Event Range Scatterer PID=" << getpid());
     
  // Update Io Registry   
  if(updateIoReg(reader_rundir.string()))
    return outwork;

  ATH_MSG_INFO("Io registry updated in the AthenaMP Event Range Scatterer PID=" << getpid());

  // _______________________ Handle saved PFC (if any) ______________________
  boost::filesystem::path abs_reader_rundir = boost::filesystem::absolute(reader_rundir);
  if(handleSavedPfc(abs_reader_rundir))
    return outwork;

  // Reopen file descriptors
  if(reopenFds())
    return outwork;

  ATH_MSG_INFO("File descriptors re-opened in the AthenaMP Event Range Scatterer PID=" << getpid());

  // ________________________ I/O reinit ________________________
  if(!m_ioMgr->io_reinitialize().isSuccess()) {
    ATH_MSG_ERROR("Failed to reinitialize I/O");
    return outwork;
  } else {
    ATH_MSG_DEBUG("Successfully reinitialized I/O");
  }

  // Start the event selector 
  IService* evtSelSvc = dynamic_cast<IService*>(m_evtSelector);
  if(!evtSelSvc) {
    ATH_MSG_ERROR("Failed to dyncast event selector to IService");
    return outwork;
  }
  if(!evtSelSvc->start().isSuccess()) {
    ATH_MSG_ERROR("Failed to restart the event selector");
    return outwork;
  } else {
    ATH_MSG_DEBUG("Successfully restarted the event selector");
  }

  // Reader dir: chdir 
  if(chdir(reader_rundir.string().c_str())==-1) {
    ATH_MSG_ERROR("Failed to chdir to " << reader_rundir.string());
    return outwork;
  }

  // Declare success and return
  *(int*)(outwork->data) = 0;
  return outwork;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> EvtRangeScatterer::exec_func()
{
  ATH_MSG_INFO("Exec function in the AthenaMP Token Scatterer PID=" << getpid());

  yampl::ISocketFactory* socketFactory = new yampl::SocketFactory();
  // Create a socket to communicate with the Pilot
  yampl::ISocket* socket2Pilot = socketFactory->createClientSocket(yampl::Channel(m_eventRangeChannel.value(),yampl::LOCAL),yampl::MOVE_DATA);
  ATH_MSG_INFO("Created CLIENT socket for communicating Event Ranges with the Pilot");
  // Create a socket to communicate with EvtRangeProcessors
  std::string socket2ProcessorName = m_processorChannel.value() + std::string("_") + m_randStr;
  yampl::ISocket* socket2Processor = socketFactory->createServerSocket(yampl::Channel(socket2ProcessorName,yampl::LOCAL),yampl::MOVE_DATA);
  ATH_MSG_INFO("Created SERVER socket to token processors: " << socket2ProcessorName);

  bool all_ok=true;
  int procReportPending(0);  // Keep track of how many output files are yet to be reported by Token Processors

  AthenaInterprocess::SharedQueue*  sharedFailedPidQueue(0);
  if(detStore()->retrieve(sharedFailedPidQueue,"AthenaMPFailedPidQueue_"+m_randStr).isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve the pointer to Shared Failed PID Queue");
    all_ok=false;
  }

  // Communication protocol with the Pilot
  std::string strReady("Ready for events");
  std::string strStopProcessing("No more events");
  std::string processorWaitRequest("");

  ATH_MSG_INFO("Starting main loop");

  while(all_ok) {
    // NO CACHING MODE: first get a request from one of the processors and only after that request the next event range from the pilot
    if(!m_doCaching && processorWaitRequest.empty()) {
      ATH_MSG_DEBUG("Waiting for event range request from one of the processors ... ");
      while(processorWaitRequest.empty()) {
	processorWaitRequest = getNewRangeRequest(socket2Processor,socket2Pilot,procReportPending);
	pollFailedPidQueue(sharedFailedPidQueue,socket2Pilot,procReportPending);
	usleep(1000);
      }
      ATH_MSG_INFO("One of the processors is ready for the next range");
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
      ATH_MSG_INFO("Stopped the loop. Last message from the Event Range Channel: " << eventRange);
      break;
    }
    ATH_MSG_INFO("Got Event Range from the pilot: " << eventRange);

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
      std::string errorStr("ERR_ATHENAMP_PARSE \"" + eventRange + "\": Wrong format");
      ATH_MSG_ERROR(errorStr);
      ATH_MSG_INFO("Ignoring this event range ");
      void* errorMessage = malloc(errorStr.size());
      memcpy(errorMessage,errorStr.data(),errorStr.size());
      socket2Pilot->send(errorMessage,errorStr.size());
      continue;
    }
    else {
      ATH_MSG_DEBUG("*** Decoded Event Range ***");
      std::map<std::string,std::string>::const_iterator it = eventRangeMap.begin(),
	itEnd = eventRangeMap.end();
      for(;it!=itEnd;++it)
	ATH_MSG_DEBUG(it->first << ":" << it->second);
      ATH_MSG_DEBUG("*** ***");
    }

    std::string rangeID = eventRangeMap["eventRangeID"];
    std::string guid = eventRangeMap["GUID"];
    int startEvent = std::atoi(eventRangeMap["startEvent"].c_str());
    int lastEvent = std::atoi(eventRangeMap["lastEvent"].c_str());
    if(rangeID.empty() 
       || guid.empty()
       || lastEvent < startEvent) {
      std::string errorStr("ERR_ATHENAMP_PARSE \"" + eventRange + "\": Wrong values of range fields");
      ATH_MSG_ERROR(errorStr);
      ATH_MSG_INFO("Ignoring this event range ");
      void* errorMessage = malloc(errorStr.size());
      memcpy(errorMessage,errorStr.data(),errorStr.size());
      socket2Pilot->send(errorMessage,errorStr.size());
      continue;
    }

    std::string message2ProcessorStr;
    char* message2Processor(0);

    std::ostringstream ostr;
    ostr << rangeID;
    if(eventRangeMap.find("PFN")!=eventRangeMap.end()) {
      ostr << "," << "PFN:" << eventRangeMap["PFN"];
    }
    ostr << "," << eventRangeMap["startEvent"]
         << "," << eventRangeMap["lastEvent"];
    message2ProcessorStr = ostr.str();

    // CACHING MODE: first get an event range from the pilot, transform it into the tokens
    // and only after that wait for a new range request by one of the processors
    if(m_doCaching) {
      ATH_MSG_DEBUG("Waiting for event range request from one of the processors");
      while(processorWaitRequest.empty()) {
	processorWaitRequest = getNewRangeRequest(socket2Processor,socket2Pilot,procReportPending);
	pollFailedPidQueue(sharedFailedPidQueue,socket2Pilot,procReportPending);
	usleep(1000);
      }
      ATH_MSG_INFO("One of the processors is ready for the next range");
    }
    
    // Send to the Processor: RangeID,evtToken[,evtToken] 
    message2Processor = (char*)malloc(message2ProcessorStr.size());
    memcpy(message2Processor,message2ProcessorStr.data(),message2ProcessorStr.size());
    socket2Processor->send(message2Processor,message2ProcessorStr.size());
    procReportPending++;

    // Get PID from the request and Update m_pid2RangeID
    int pid = std::atoi(processorWaitRequest.c_str());
    m_pid2RangeID[pid] = rangeID;
    processorWaitRequest.clear();

    ATH_MSG_INFO("Sent response to the processor : " << message2ProcessorStr);
  }

  if(all_ok) {
    // We are done distributing event tokens. 
    // Tell the workers that the event processing is over
    // i.e. send out m_nprocs empty messages
    void* emptyMess4Processor(0);
    if(!processorWaitRequest.empty()) {
      // We already have one processor waiting for the answer
      emptyMess4Processor = malloc(1);
      socket2Processor->send(emptyMess4Processor,1);
      ATH_MSG_INFO("Set one processor free");
    }
    for(int i(0); i<(processorWaitRequest.empty()?m_nprocs:m_nprocs-1); ++i) {
      ATH_MSG_DEBUG("Going to set another processor free");
      while(getNewRangeRequest(socket2Processor,socket2Pilot,procReportPending).empty()) {
	pollFailedPidQueue(sharedFailedPidQueue,socket2Pilot,procReportPending);
	usleep(1000);
      }
      emptyMess4Processor = malloc(1);
      socket2Processor->send(emptyMess4Processor,1);
      ATH_MSG_INFO("Set one processor free");
    }

    ATH_MSG_INFO("Still " << procReportPending << " pending reports");
    
    // Final round of colecting output file names from processors
    while(procReportPending>0) {
      std::string strProcessorRequest = getNewRangeRequest(socket2Processor,socket2Pilot,procReportPending);
      if(!strProcessorRequest.empty()) {
	ATH_MSG_WARNING("Unexpected message received from a processor at this stage : " << strProcessorRequest);
      }
      pollFailedPidQueue(sharedFailedPidQueue,socket2Pilot,procReportPending);
      usleep(1000);
    }
  }

  if(m_appMgr->stop().isFailure()) {
    ATH_MSG_ERROR("Unable to stop AppMgr"); 
    all_ok=false;
  }
  else { 
    if(m_appMgr->finalize().isFailure()) {
      ATH_MSG_ERROR("Unable to finalize AppMgr");
      all_ok=false;
    }
  }
  
  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = (all_ok?0:1); // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);

  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...
  delete socket2Processor;
  delete socket2Pilot;
  delete socketFactory;

  return outwork;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> EvtRangeScatterer::fin_func()
{
  // Dummy
  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = 0; // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);
  return outwork;
}

void EvtRangeScatterer::trimRangeStrings(std::string& str)
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

std::string EvtRangeScatterer::getNewRangeRequest(yampl::ISocket* socket2Processor
					       , yampl::ISocket* socket2Pilot
					       , int& procReportPending)
{
  void* processor_request(0);
  ssize_t processorRequestSize = socket2Processor->tryRecv(processor_request);

  if(processorRequestSize==-1) return std::string("");
  if(processorRequestSize==sizeof(pid_t)+sizeof(AthenaMPToolBase::ESRange_Status)) {
    ATH_MSG_INFO("Processor reported event range processing error");
    pid_t pid = *((pid_t*)processor_request);
    AthenaMPToolBase::ESRange_Status status = *((AthenaMPToolBase::ESRange_Status*)((pid_t*)processor_request+1));
    std::string errorStr("ERR_ATHENAMP_PROCESS "+ m_pid2RangeID[pid] + ": ");
    switch(status) {
    case AthenaMPToolBase::ESRANGE_NOTFOUND:
      errorStr+=std::string("Not found in the input file");
      break;
    case AthenaMPToolBase::ESRANGE_SEEKFAILED :	
      errorStr+=std::string("Seek failed");
      break;
    case AthenaMPToolBase::ESRANGE_PROCFAILED :
      errorStr+=std::string("Failed to process event range");
      break;
    case AthenaMPToolBase::ESRANGE_FILENOTMADE :
      errorStr+=std::string("Failed to make output file");
      break;
    case AthenaMPToolBase::ESRANGE_BADINPFILE :
      errorStr+=std::string("Failed to set input file");
      break;
    default:
      break;
    }
    void* errorMessage = malloc(errorStr.size());
    memcpy(errorMessage,errorStr.data(),errorStr.size());
    socket2Pilot->send(errorMessage,errorStr.size());
    procReportPending--;
    ATH_MSG_INFO("Error reported to the pilot. Reports pending: " << procReportPending);
    return std::string("");
  }
  std::string strProcessorRequest((const char*)processor_request,processorRequestSize);
  ATH_MSG_INFO("Received request from a processor: " << strProcessorRequest);
  // Decode the request. If it contains output file name then pass it over to the pilot and return empty string
  if(strProcessorRequest.find('/')==0) {
    void* outpFileNameMessage = malloc(strProcessorRequest.size());
    memcpy(outpFileNameMessage,strProcessorRequest.data(),strProcessorRequest.size());
    socket2Pilot->send(outpFileNameMessage,strProcessorRequest.size());
    procReportPending--;
    ATH_MSG_INFO("Output file reported to the pilot. Reports pending: " << procReportPending);
    return std::string("");
  }
  return strProcessorRequest;
}

void EvtRangeScatterer::pollFailedPidQueue(AthenaInterprocess::SharedQueue*  sharedFailedPidQueue
					, yampl::ISocket* socket2Pilot
					, int& procReportPending)
{
  pid_t pid;
  if(sharedFailedPidQueue->try_receive_basic<pid_t>(pid)) {
    ATH_MSG_INFO("Procesor with PID=" << pid << " has failed!");
    if(m_pid2RangeID.find(pid)!=m_pid2RangeID.end()) {
      ATH_MSG_WARNING("The failed RangeID = " << m_pid2RangeID[pid] << " will be reported to Pilot");

      std::string errorStr("ERR_ATHENAMP_PROCESS " + m_pid2RangeID[pid] + ": Failed to process event range");
      void* errorMessage = malloc(errorStr.size());
      memcpy(errorMessage,errorStr.data(),errorStr.size());
      socket2Pilot->send(errorMessage,errorStr.size());
    }
    procReportPending--;
    ATH_MSG_INFO("Reports pending: " << procReportPending);
  } 
}
