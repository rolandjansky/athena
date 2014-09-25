/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SharedEvtQueueProvider.h"
#include "AthenaInterprocess/ProcessGroup.h"

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "EventInfo/EventStreamInfo.h"
#include "ByteStreamData/ByteStreamMetadataContainer.h"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdexcept>

SharedEvtQueueProvider::SharedEvtQueueProvider(const std::string& type
					       , const std::string& name
					       , const IInterface* parent)
  : AthenaMPToolBase(type,name,parent)
  , m_isPileup(false)
  , m_nEvtRequested(-1)
  , m_skipEvents(0)
  , m_nEvtCounted(0)
  , m_nEvtAddPending(0)
  , m_preCountedEvents(-1)
  , m_needCountEvents(false)
  , m_nEventsBeforeFork(0)
  , m_nEventsInInpFiles(0)
  , m_sharedEventQueue(0)
{
  declareInterface<IAthenaMPTool>(this);

  declareProperty("IsPileup",m_isPileup);
  declareProperty("PreCountedEvents",m_preCountedEvents);
  declareProperty("EventsBeforeFork",m_nEventsBeforeFork);

  m_subprocDirPrefix = "evt_counter";
}

SharedEvtQueueProvider::~SharedEvtQueueProvider()
{
}

StatusCode SharedEvtQueueProvider::initialize()
{
  msg(MSG::DEBUG) << "In initialize" << endreq;
  if(m_isPileup) {
    m_evtProcessor = ServiceHandle<IEventProcessor>("PileUpEventLoopMgr",name());
    msg(MSG::INFO) << "The job running in pileup mode" << endreq;
  }
  else {
    msg(MSG::INFO) << "The job running in non-pileup mode" << endreq;
  }

  StatusCode sc = AthenaMPToolBase::initialize();
  if(!sc.isSuccess())
    return sc;

  // Get SkipEvents property of the event selector and store it locally
  IProperty* propertyServer = dynamic_cast<IProperty*>(m_evtSelector);
  if(propertyServer==0) {
    msg(MSG::ERROR) << "Unable to cast event selector to IProperty" << endreq;
    return StatusCode::FAILURE;
  }
  std::string propertyName("SkipEvents");
  int skipEvents(0);
  IntegerProperty skipEventsProp(propertyName,skipEvents);
  sc = propertyServer->getProperty(&skipEventsProp);
  if(sc.isFailure()) {
    msg(MSG::INFO) << "Event Selector does not have SkipEvents property" << endreq;
  }
  else {
    m_skipEvents = skipEventsProp.value();
    // Set SkipEvents of the ByteStreamevent selector to 0 - this is to work around ByteStream event selector problems
    // Are we running over byte-stream input?
    ServiceHandle<StoreGateSvc> p_SGinMeta("StoreGateSvc/InputMetaDataStore",name());
    sc=p_SGinMeta.retrieve();
    if(sc.isSuccess() && p_SGinMeta!=0) {
      if(p_SGinMeta->contains<ByteStreamMetadata>("ByteStreamMetadata")) {
	IntegerProperty newSkipEventsProp(propertyName, 0);
	if(propertyServer->setProperty(newSkipEventsProp).isFailure()) {
	  msg(MSG::ERROR) << "Unable to set SkipEvents=0 on the ByteStream event selector" << endreq;
	  return StatusCode::FAILURE;
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode SharedEvtQueueProvider::finalize()
{
  return StatusCode::SUCCESS;
}

int SharedEvtQueueProvider::makePool(int maxevt, int, const std::string& topdir)
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

  m_nEvtRequested = maxevt;
  m_subprocTopDir = topdir;

  // Create event queue
  msg(MSG::DEBUG) << "Event queue name " << "AthenaMPEventQueue_" << m_randStr << endreq;
  StatusCode sc = detStore()->retrieve(m_sharedEventQueue,"AthenaMPEventQueue_"+m_randStr);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve the pointer to Shared Event Queue" << endreq;
    return -1;
  }

  // Create the process group and map_async bootstrap
  m_processGroup = new AthenaInterprocess::ProcessGroup(1);
  msg(MSG::INFO) << "Event Counter process created" << endreq;
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_BOOTSTRAP))
    return -1;
  msg(MSG::INFO) << "Event Counter bootstraped" << endreq; 

  return 1;
}

StatusCode SharedEvtQueueProvider::exec()
{
  msg(MSG::DEBUG) << "In exec " << getpid() << endreq;

  // Map exec flag
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_EXEC)) {
    msg(MSG::ERROR) << "Unable to send work to the event counter" << endreq;
    return StatusCode::FAILURE;
  }

  // Map exit flag 
  if(m_processGroup->map_async(0,0)){
    msg(MSG::ERROR) << "Unable to set exit to the event counter" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void SharedEvtQueueProvider::subProcessLogs(std::vector<std::string>& filenames)
{
  filenames.clear();
  boost::filesystem::path counter_rundir(m_subprocTopDir);
  counter_rundir /= boost::filesystem::path(m_subprocDirPrefix);
  filenames.push_back(counter_rundir.string()+std::string("/AthenaMP.log"));
}

AthenaMP::AllWorkerOutputs_ptr SharedEvtQueueProvider::generateOutputReport()
{
  AthenaMP::AllWorkerOutputs_ptr jobOutputs(new AthenaMP::AllWorkerOutputs());
  return jobOutputs;
}

AthenaInterprocess::ScheduledWork* SharedEvtQueueProvider::bootstrap_func()
{
  int* errcode = new int(1); // For now use 0 success, 1 failure
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = (void*)errcode;
  outwork->size = sizeof(int);
  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...

  // ________________________ mkdir ________________________
  boost::filesystem::path counter_rundir(m_subprocTopDir);
  counter_rundir /= boost::filesystem::path(m_subprocDirPrefix);

  if(mkdir(counter_rundir.string().c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)==-1) {
    msg(MSG::ERROR) << "Unable to make event counter run directory: " << counter_rundir.string() << ". " << strerror(errno) << endreq;
    return outwork;
  }

  // ________________________ Redirect logs ________________________
  if(redirectLog(counter_rundir.string()))
    return outwork;

  msg(MSG::INFO) << "Logs redirected in the AthenaMP event event counter PID=" << getpid() << endreq;

  // ________________________ Update Io Registry ____________________________
  if(updateIoReg(counter_rundir.string()))
    return outwork;

  msg(MSG::INFO) << "Io registry updated in the AthenaMP event event counter PID=" << getpid() << endreq;

  // _______________________ Handle saved PFC (if any) ______________________
  boost::filesystem::path abs_counter_rundir = boost::filesystem::absolute(counter_rundir);
  if(handleSavedPfc(abs_counter_rundir))
    return outwork;

  // ________________________  reopen descriptors ____________________________
  if(reopenFds())
    return outwork;

  msg(MSG::INFO) << "File descriptors re-opened in the AthenaMP event event counter PID=" << getpid() << endreq;

  // ________________________ I/O reinit ________________________
  if(!m_ioMgr->io_reinitialize().isSuccess()) {
    msg(MSG::ERROR) << "Failed to reinitialize I/O" << endreq;
    return outwork;
  } else {
    msg(MSG::DEBUG) << "Successfully reinitialized I/O" << endreq;
  }

  // ________________________ Event selector restart ________________________
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

  // ________________________ chdir ________________________
  if(chdir(counter_rundir.string().c_str())==-1) {
    msg(MSG::ERROR) << "Failed to chdir to " << counter_rundir.string() << endreq;
    return outwork;
  }

  // Declare success and return
  *errcode = 0;
  return outwork;
}

AthenaInterprocess::ScheduledWork* SharedEvtQueueProvider::exec_func()
{
  msg(MSG::INFO) << "Exec function in the AthenaMP Event Counter PID=" << getpid() << endreq;
  bool all_ok(true);

  if(m_preCountedEvents>-1) {
    m_nEvtAddPending = m_preCountedEvents;
  }
  else {
    if(nEventsInFile()) {
      msg(MSG::ERROR) << "Failed to get number of events in the input file" << endreq;
      all_ok=false;
    }
  }

  // Send first block of events to the shared queue
  if(all_ok) {
    if(addEventsToQueue()) {
      msg(MSG::ERROR) << "Failed to insert event numbers to the shared queue" << endreq;
      all_ok=false;
    }
    else {
      msg(MSG::DEBUG) << "need to count more events " << (m_needCountEvents?"YES":"NO") << endreq;
      
      // Count more events if necessary
      if(m_needCountEvents) {
	// Use incident service for registering a BeginInputFile handler 
	IIncidentSvc* incsvc(0);
	StatusCode sc = serviceLocator()->service("IncidentSvc",incsvc);
	if(sc.isFailure() || incsvc==0) {
	  msg(MSG::ERROR) << "Error retrieving IncidentSvc" << endreq;
	  all_ok=false;
	}
	else {
	  incsvc->addListener(this,"BeginInputFile");
	  msg(MSG::DEBUG) << "Added self as listener to BeginInputFile" << endreq;
	  
	  unsigned evtNum(0);
	  IEvtSelector::Context* evtContext(0);
	  sc = m_evtSelector->createContext(evtContext);
	  if(sc.isFailure()) {
	    msg(MSG::ERROR) << "Failed to create the event selector context" << endreq;
	    all_ok=false;
	  }
	  else {
	    
	    // Now read all input events in order to count them and add entries to the shared event queue
	    // Events are added to the queue in chunks when new input file gets opened and BeginInputFile incident is thrown
	    while(sc.isSuccess() && m_needCountEvents) {
	      sc = m_evtSelector->next(*evtContext);
	      msg(MSG::DEBUG) << "Evt num=" << ++evtNum << endreq;
	    }
	    
	    // We are done counting events. Set the Final flag to True
	    updateShmem(m_nEvtCounted,true);
	  }
	}
      }
    }

    msg(MSG::INFO) << "Done counting events and populating shared queue. Total number of events to be processed: " << std::max(m_nEvtCounted - m_nEventsBeforeFork,0) << endreq;
  }

  if(all_ok) {
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
  }

  int* errcode = new int(all_ok?0:1); // For now use 0 success, 1 failure
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = (void*)errcode;
  outwork->size = sizeof(int);
  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...
  return outwork;
}

AthenaInterprocess::ScheduledWork* SharedEvtQueueProvider::fin_func()
{
  // Dummy
  int* errcode = new int(0); 
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = (void*)errcode;
  outwork->size = sizeof(int);
  return outwork;
}

void SharedEvtQueueProvider::handle(const Incident& inc)
{
  msg(MSG::DEBUG) << "In handle" << endreq;

  const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
  if(fileInc == 0) {
    msg(MSG::WARNING) << "Failed to dyn-cast the incident" << endreq;
    return;
  } 

  if(nEventsInFile()) {
    msg(MSG::ERROR) << "Failed to get number of events in the input file" << endreq;
    throw std::runtime_error("Failed to get number of events in the input file");
  }
  
  if(addEventsToQueue()) {
    msg(MSG::ERROR) << "Failed to insert event numbers into the shared queue" << endreq;
    throw std::runtime_error("Failed to insert event numbers into the shared queue");
  }
}

int SharedEvtQueueProvider::nEventsInFile()
{
  msg(MSG::DEBUG) << "SharedEvtQueueProvider::nEventsInFile()" << endreq;

  ServiceHandle<StoreGateSvc> p_SGinMeta("StoreGateSvc/InputMetaDataStore",name());
  StatusCode sc=p_SGinMeta.retrieve();
  if(sc.isSuccess() && p_SGinMeta!=0) {
    // For BS files use ByteStreamMetadata
    if(p_SGinMeta->contains<ByteStreamMetadata>("ByteStreamMetadata")) {
      const DataHandle<ByteStreamMetadata> bsmd;
      sc = p_SGinMeta->retrieve(bsmd, "ByteStreamMetadata");
      if(sc.isFailure() || !bsmd) {
	msg(MSG::ERROR) << "Cannot retrieve ByteStreamMetadata from meta data store: unable to count events" << endreq;
	return 1; 
      }
      else {
	msg(MSG::DEBUG) << "ByteStreamMetadata: Number of events = " << bsmd->getNumEvents() << endreq;
	m_nEventsInInpFiles += bsmd->getNumEvents();
      }
    }
    else {
      // For POOL files use EventStreamInfo
      const DataHandle<EventStreamInfo> esi1,esi2;
      sc = p_SGinMeta->retrieve(esi1,esi2);
      if(sc.isFailure() || esi1 == esi2) {
	msg(MSG::ERROR) << "Cannot retrieve EventStreamInfo from meta data store: unable to count events" << endreq;
	return 1;
      }
      else {
	msg(MSG::DEBUG) << "EventStreamInfo: Number of events = " << esi1->getNumberOfEvents() << endreq;
	m_nEventsInInpFiles += esi1->getNumberOfEvents();
	if(++esi1!=esi2) {
	  msg(MSG::WARNING) << "More than one instance of EventStreamInfo found in the meta data store! Used first instance for event number" << endreq;
	  msg(MSG::WARNING) << "EventStreamInfo keys:";
	  std::vector<std::string> esiKeys;
	  p_SGinMeta->keys<EventStreamInfo>(esiKeys);
	  for(size_t i=0;i<esiKeys.size();++i)	  
	    msg(MSG::WARNING) << " " << esiKeys[i];
	  msg(MSG::WARNING) << endreq;
	}
      }
    }
  }
  else {
    msg(MSG::ERROR) << "Cannot get InputMetaDataStore: unable to count events" << endreq;
    return 1;
  }

  // Adjust the number of pending events if necessary
  // Taking into account the cases when SkipEvents is larger than number of events in first one or several files,
  // in which case m_nEventsInInpFiles-(m_nEvtCounted+m_skipEvents)<0
  m_nEvtAddPending = std::max(0,
			      (m_nEvtRequested==-1
			       ? m_nEventsInInpFiles-(m_nEvtCounted+m_skipEvents)
			       : std::min(m_nEvtRequested-m_nEvtCounted,m_nEventsInInpFiles-(m_nEvtCounted+m_skipEvents))
			       )
			      );

  return 0;
}

int SharedEvtQueueProvider::addEventsToQueue()
{
  msg(MSG::DEBUG) << "in addEventsToQueue" << endreq;

  // Add events to the queue
  while(m_nEvtAddPending>0) {
    // Don't add those events which have already been processed by the master before forking
    assert(m_nEvtRequested>m_nEvtCounted);
    if(m_nEvtCounted<m_nEventsBeforeFork 
       || m_sharedEventQueue->try_send_basic<int>(m_nEvtCounted+m_skipEvents)) {
      m_nEvtCounted++;
      m_nEvtAddPending--;
    }
    else {
      // The queue reached maximum capacity
      // Update shared memory information
      updateShmem(m_nEvtCounted,false);
      usleep(100);
    }
  }

  // All pending events have been added. Do we need to keep counting?
  m_needCountEvents = ((m_preCountedEvents==-1)&&(m_nEvtRequested==-1 || m_nEvtCounted<m_nEvtRequested));

  // Update shared memory with this information
  updateShmem(m_nEvtCounted,!m_needCountEvents);
  return 0;
}

void SharedEvtQueueProvider::updateShmem(int eventCount, bool countFinal)
{
  std::string shmemName("/athmp-shmem-"+m_randStr);
  boost::interprocess::shared_memory_object shmemSegment(boost::interprocess::open_only
							 , shmemName.c_str()
							 , boost::interprocess::read_write);
  boost::interprocess::mapped_region shmemRegion(shmemSegment,boost::interprocess::read_write);
  int* shmemCountedEvts = (int*)shmemRegion.get_address();
  int* shmemCountFinal = shmemCountedEvts+1;
  *shmemCountedEvts = eventCount;
  *shmemCountFinal = countFinal;
}

