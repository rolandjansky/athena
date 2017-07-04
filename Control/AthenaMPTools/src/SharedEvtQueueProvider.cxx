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
#include "AthenaKernel/IEventShare.h"

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
  , m_nprocesses(-1)
  , m_useSharedReader(false)
  , m_nEventsBeforeFork(0)
  , m_nChunkSize(1)
  , m_nChunkStart(0)
  , m_nPositionInChunk(0)
  , m_nEvtRequested(-1)
  , m_nEvtCounted(0)
  , m_sharedEventQueue(0)
  , m_evtShare(0)
{
  declareInterface<IAthenaMPTool>(this);

  declareProperty("IsPileup",m_isPileup);
  declareProperty("UseSharedReader",m_useSharedReader);
  declareProperty("EventsBeforeFork",m_nEventsBeforeFork);
  declareProperty("ChunkSize",m_nChunkSize);

  m_subprocDirPrefix = "evt_counter";
}

SharedEvtQueueProvider::~SharedEvtQueueProvider()
{
}

StatusCode SharedEvtQueueProvider::initialize()
{
  ATH_MSG_DEBUG( "In initialize" );
  if(m_isPileup) {
    m_evtProcessor = ServiceHandle<IEventProcessor>("PileUpEventLoopMgr",name());
    ATH_MSG_INFO( "The job running in pileup mode" );
  }
  else {
    ATH_MSG_INFO( "The job running in non-pileup mode" );
  }

  StatusCode sc = AthenaMPToolBase::initialize();
  if(!sc.isSuccess())
    return sc;

  return StatusCode::SUCCESS;
}

StatusCode SharedEvtQueueProvider::finalize()
{
  return StatusCode::SUCCESS;
}

int SharedEvtQueueProvider::makePool(int maxevt, int nprocs, const std::string& topdir)
{
  ATH_MSG_DEBUG( "In makePool " << getpid() );

  if(maxevt < -1) {
    ATH_MSG_ERROR( "Invalid number of events requested: " << maxevt );
    return -1;
  }

  if(topdir.empty()) {
    ATH_MSG_ERROR( "Empty name for the top directory!" );
    return -1;
  }

  m_nEvtRequested = maxevt;
  m_nprocs = (nprocs==-1?sysconf(_SC_NPROCESSORS_ONLN):nprocs);
  m_nprocesses = m_nprocs;
  m_subprocTopDir = topdir;

  // Create event queue
  ATH_MSG_DEBUG( "Event queue name " << "AthenaMPEventQueue_" << m_randStr );
  StatusCode sc = detStore()->retrieve(m_sharedEventQueue,"AthenaMPEventQueue_"+m_randStr);
  if(sc.isFailure()) {
    ATH_MSG_ERROR( "Unable to retrieve the pointer to Shared Event Queue" );
    return -1;
  }

  // Create the process group and map_async bootstrap
  m_processGroup = new AthenaInterprocess::ProcessGroup(1);
  ATH_MSG_INFO( "Event Counter process created" );
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_BOOTSTRAP))
    return -1;
  ATH_MSG_INFO( "Event Counter bootstraped" ); 

  return 1;
}

StatusCode SharedEvtQueueProvider::exec()
{
  ATH_MSG_DEBUG( "In exec " << getpid() );

  // Map exec flag
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_EXEC)) {
    ATH_MSG_ERROR( "Unable to send work to the event counter" );
    return StatusCode::FAILURE;
  }

  // Map exit flag 
  if(m_processGroup->map_async(0,0)){
    ATH_MSG_ERROR( "Unable to set exit to the event counter" );
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

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedEvtQueueProvider::bootstrap_func()
{
  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = 1; // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);

  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...

  // ________________________ mkdir ________________________
  boost::filesystem::path counter_rundir(m_subprocTopDir);
  counter_rundir /= boost::filesystem::path(m_subprocDirPrefix);

  if(mkdir(counter_rundir.string().c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)==-1) {
    ATH_MSG_ERROR( "Unable to make event counter run directory: " << counter_rundir.string() << ". " << strerror(errno) );
    return outwork;
  }

  // ________________________ Redirect logs ________________________
  if(redirectLog(counter_rundir.string()))
    return outwork;

  ATH_MSG_INFO( "Logs redirected in the AthenaMP event event counter PID=" << getpid() );

  // ________________________ Update Io Registry ____________________________
  if(updateIoReg(counter_rundir.string()))
    return outwork;

  ATH_MSG_INFO( "Io registry updated in the AthenaMP event event counter PID=" << getpid() );

  // _______________________ Handle saved PFC (if any) ______________________
  boost::filesystem::path abs_counter_rundir = boost::filesystem::absolute(counter_rundir);
  if(handleSavedPfc(abs_counter_rundir))
    return outwork;

  // ________________________ reopen descriptors ____________________________
  if(reopenFds())
    return outwork;

  ATH_MSG_INFO( "File descriptors re-opened in the AthenaMP event event counter PID=" << getpid() );

  // _______________________ event counting ________________________________
  // Use incident service for registering a EndInputFile handler 
  IIncidentSvc* incsvc(0);
  StatusCode sc = serviceLocator()->service("IncidentSvc",incsvc);
  if(sc.isFailure() || incsvc==0) {
    ATH_MSG_ERROR( "Error retrieving IncidentSvc" );
    return outwork;
  }
  
  incsvc->addListener(this,"EndInputFile");
  ATH_MSG_DEBUG( "Added self as listener to EndInputFile" );

  // _______________________ event sharing ________________________________
  // Use EventSelector as SharedReader (if configured) and enable output streaming
  if (m_useSharedReader) {
    m_evtShare  = dynamic_cast<IEventShare*>(m_evtSelector);
    if(!m_evtShare) {
      ATH_MSG_ERROR( "Failed to dyncast event selector to IEventShare" );
      return outwork;
    } else {
      if(!m_evtShare->makeServer(m_nprocs).isSuccess()) {
        ATH_MSG_ERROR("Failed to make the event selector a share server");
        return outwork;
      } else {
        ATH_MSG_DEBUG("Successfully made the event selector a share server");
      }
    }
  }

  // ________________________ I/O reinit ________________________
  if(!m_ioMgr->io_reinitialize().isSuccess()) {
    ATH_MSG_ERROR( "Failed to reinitialize I/O" );
    return outwork;
  } else {
    ATH_MSG_DEBUG( "Successfully reinitialized I/O" );
  }

  // ________________________ Event selector restart ________________________
  IService* evtSelSvc = dynamic_cast<IService*>(m_evtSelector);
  if(!evtSelSvc) {
    ATH_MSG_ERROR( "Failed to dyncast event selector to IService" );
    return outwork;
  }
  if(!evtSelSvc->start().isSuccess()) {
    ATH_MSG_ERROR( "Failed to restart the event selector" );
    return outwork;
  } else {
    ATH_MSG_DEBUG( "Successfully restarted the event selector" );
  }

  // ________________________ chdir ________________________
  if(chdir(counter_rundir.string().c_str())==-1) {
    ATH_MSG_ERROR( "Failed to chdir to " << counter_rundir.string() );
    return outwork;
  }

  // Declare success and return
  *(int*)(outwork->data) = 0; // Error code: for now use 0 success, 1 failure
  return outwork;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedEvtQueueProvider::exec_func()
{
  ATH_MSG_INFO("Exec function in the AthenaMP Event Counter PID=" << getpid());

  bool all_ok(true);

  // Get SkipEvents property of the event selector
  int skipEvents(0);
  IProperty* propertyServer = dynamic_cast<IProperty*>(m_evtSelector);
  if(propertyServer==0) {
    ATH_MSG_ERROR( "Unable to cast event selector to IProperty" );
    all_ok=false;
  }
  else {
    std::string propertyName("SkipEvents");
    IntegerProperty skipEventsProp(propertyName,skipEvents);
    if(propertyServer->getProperty(&skipEventsProp).isFailure()) {
      ATH_MSG_INFO( "Event Selector does not have SkipEvents property" );
    }
    else {
      skipEvents = skipEventsProp.value();
    }
  }

  IEvtSelector::Context* evtContext(0);
  if(all_ok) {
    StatusCode sc = m_evtSelector->createContext(evtContext);
    if(sc.isFailure()) {
      ATH_MSG_ERROR("Failed to create the event selector context");
      all_ok=false;
    }
    else {
      // advance to nEventsBeforeFork
      for(int i(0); i<m_nEventsBeforeFork;++i) {
	if(!m_evtSelector->next(*evtContext).isSuccess()) {
	  ATH_MSG_ERROR("Unexpected error: EventsBeforeFork>EventsInInputFiles");
	  all_ok=false;
	  break;
	}
      }
    }
  }

  if(all_ok) {
    if(m_nEvtRequested!=0) { // Take into account corner case with evtMax=0
      m_nChunkStart = skipEvents+m_nEventsBeforeFork;
      m_nEvtCounted = m_nEventsBeforeFork;
      m_nPositionInChunk = m_nChunkStart;      
      ATH_MSG_VERBOSE("Starting to go through events. Chunk start = " << m_nChunkStart+1);
      
      // Loop through all remaining events 
      while(m_evtSelector->next(*evtContext).isSuccess()) {
	m_nPositionInChunk++;
	m_nEvtCounted++;
	ATH_MSG_VERBOSE("Events Counted " << m_nEvtCounted << ", Position in Chunk " << m_nPositionInChunk);
	if(((m_nPositionInChunk-m_nChunkStart)==m_nChunkSize)
	   || (m_nEvtCounted==m_nEvtRequested)) {
	  addEventsToQueue();
	  m_nChunkStart = m_nPositionInChunk;
	  if(m_nEvtCounted==m_nEvtRequested) break;
	}
      }
    }

    // We are done. Add -m_nEvtCounted  m_nprocesses-times to the queue
    long newValueForQueue = (long)(-m_nEvtCounted);
    for(int i=0;i<m_nprocesses;++i) {
      while(!m_sharedEventQueue->try_send_basic<long>(newValueForQueue)) {
	usleep(1000);
      }
    }

    ATH_MSG_INFO("Done counting events and populating shared queue. Total number of events to be processed: " << std::max(m_nEvtCounted - m_nEventsBeforeFork,0) 
		 << ", Event Chunk size in the queue is " << m_nChunkSize);

    if(m_useSharedReader && m_evtShare) {
      if(m_evtShare->readEvent(0).isFailure()) {
        ATH_MSG_ERROR("Failed to read " << m_nEvtRequested << " events");
        all_ok=false;
      } else {
        ATH_MSG_DEBUG("readEvent succeeded");
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
  }


  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = (all_ok?0:1); // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);

  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...
  return outwork;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedEvtQueueProvider::fin_func()
{
  // Dummy
  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = 0; // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);
  return outwork;
}

void SharedEvtQueueProvider::handle(const Incident& inc)
{
  ATH_MSG_DEBUG( "Handling incident" );

  const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
  if(fileInc == 0) {
    ATH_MSG_WARNING( "Failed to dyn-cast the incident" );
    return;
  } 

  if(fileInc->type()=="EndInputFile") {
    ATH_MSG_DEBUG( "End Input File reached!" );

    if(m_nChunkStart!=m_nPositionInChunk) {
      addEventsToQueue();
      m_nChunkStart = m_nPositionInChunk;
    }
  }
}

void SharedEvtQueueProvider::addEventsToQueue()
{
  ATH_MSG_DEBUG("in addEventsToQueue");
  long newValueForQueue = ((long)(m_nPositionInChunk-m_nChunkStart)<<(sizeof(int)*8))|m_nChunkStart;
  while(!m_sharedEventQueue->try_send_basic<long>(newValueForQueue)) {
    usleep(100);
  }
  ATH_MSG_INFO("Sent to the queue 0x" << std::hex << newValueForQueue << std::dec
	       << " which corresponds to Chunks start " << m_nChunkStart
	       << " and chunk size " << m_nPositionInChunk-m_nChunkStart);
}

