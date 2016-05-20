/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SharedReaderTool.h"
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

#include <boost/interprocess/managed_shared_memory.hpp>

SharedReaderTool::SharedReaderTool(const std::string& type
				   , const std::string& name
				   , const IInterface* parent)
  : AthenaMPToolBase(type,name,parent)
  , m_nEvtRequested(-1)
  , m_shmemSegment()
  , m_evtShare(0)
{
  m_subprocDirPrefix = "shared_reader";
}

SharedReaderTool::~SharedReaderTool()
{
  delete m_shmemSegment; m_shmemSegment = 0;
}

StatusCode SharedReaderTool::initialize()
{
  ATH_MSG_DEBUG("In initialize");

  StatusCode sc = AthenaMPToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  sc = serviceLocator()->service(m_evtSelName,m_evtShare);
  if(sc.isFailure() || m_evtShare==0) {
    ATH_MSG_ERROR("Error retrieving IEventShare");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SharedReaderTool::finalize()
{
  return StatusCode::SUCCESS;
}

int SharedReaderTool::makePool(int maxevt, int nprocs, const std::string& topdir)
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
  m_nEvtRequested = maxevt;
  m_subprocTopDir = topdir;

  // Create the process group with only one process and map_async bootstrap
  m_processGroup = new AthenaInterprocess::ProcessGroup(1);
  ATH_MSG_INFO("Shared Reader process created");
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_BOOTSTRAP))
    return -1;

  return 1;  
}

StatusCode SharedReaderTool::exec()
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

void SharedReaderTool::subProcessLogs(std::vector<std::string>& filenames)
{
  filenames.clear();
  boost::filesystem::path reader_rundir(m_subprocTopDir);
  reader_rundir/= boost::filesystem::path(m_subprocDirPrefix);
  filenames.push_back(reader_rundir.string()+std::string("/AthenaMP.log"));
}

AthenaMP::AllWorkerOutputs_ptr SharedReaderTool::generateOutputReport()
{
  AthenaMP::AllWorkerOutputs_ptr jobOutputs(new AthenaMP::AllWorkerOutputs());
  return jobOutputs;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedReaderTool::bootstrap_func()
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

  ATH_MSG_INFO("Logs redirected in the AthenaMP ByteStream reader PID=" << getpid());
     
  // Update Io Registry   
  if(updateIoReg(reader_rundir.string()))
    return outwork;

  ATH_MSG_INFO("Io registry updated in the AthenaMP ByteStream reader PID=" << getpid());

  // _______________________ Handle saved PFC (if any) ______________________
  boost::filesystem::path abs_reader_rundir = boost::filesystem::absolute(reader_rundir);
  if(handleSavedPfc(abs_reader_rundir))
    return outwork;

  // Reopen file descriptors
  if(reopenFds())
    return outwork;

  ATH_MSG_INFO("File descriptors re-opened in the AthenaMP ByteStream reader PID=" << getpid());

  // IEventShare ...
  if(!m_evtShare->makeServer(m_nprocs).isSuccess()) {
    ATH_MSG_ERROR("Failed to make the event selector a share server");
    return outwork;
  }
  else {
    ATH_MSG_DEBUG("Successfully made the event selector a share server");
  }

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

  // Instantiate shared memory segment in the reader
  try {
    std::string shmemName("/athmp-shmem-"+m_randStr);
    boost::interprocess::shared_memory_object shmemSegment(boost::interprocess::open_only, shmemName.c_str(), boost::interprocess::read_write);
    m_shmemSegment = new boost::interprocess::mapped_region(shmemSegment,boost::interprocess::read_write);
  }
  catch(yampl::ErrnoException& ex) {
    ATH_MSG_ERROR("Exception caught when trying to acquire shared memory segment: " << ex.what());
    return outwork;
  }

  // Declare success and return
  *(int*)(outwork->data) = 0;
  return outwork;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedReaderTool::exec_func()
{
  ATH_MSG_INFO("Exec function in the AthenaMP Shared Reader PID=" << getpid());

/*
  int eventsRead(0);
  int* shmemCountedEvts = (int*)m_shmemSegment->get_address();
  int* shmemCountFinal = shmemCountedEvts+1;

  int ctFinal(*shmemCountFinal), evtCounted(*shmemCountedEvts);
  while(evtCounted==0 && ctFinal==0) { 
    usleep(10);
    ctFinal = *shmemCountFinal;
    evtCounted = *shmemCountedEvts;
  }
  ATH_MSG_DEBUG("SharedReaderTool::exec_func evtCounted=" << evtCounted << ", CountFinal=" << (ctFinal?"YES":"NO"));
*/

  bool all_ok=true;
  ATH_MSG_DEBUG("SharedReaderTool::exec_func entering loop ");
//  while(eventsRead<evtCounted) {
    ATH_MSG_DEBUG("SharedReaderTool::exec_func loop ");
    if(m_evtShare->readEvent(m_nEvtRequested).isFailure()) {
      ATH_MSG_ERROR("Failed to read " << m_nEvtRequested << " events");
      all_ok=false;
    }
    else {
      ATH_MSG_DEBUG("readEvent succeeded");
/*
      if(ctFinal)
	break;
      eventsRead=evtCounted;
      ctFinal = *shmemCountFinal;
      evtCounted = *shmemCountedEvts;
      ATH_MSG_DEBUG("SharedReaderTool::exec_func evtCounted=" << evtCounted << ", CountFinal=" << (ctFinal?"YES":"NO"));

      while(eventsRead==evtCounted 
	    && !ctFinal) {
	usleep(10);
	ctFinal = *shmemCountFinal;
	evtCounted = *shmemCountedEvts;
      }
      ATH_MSG_DEBUG("SharedReaderTool::exec_func evtCounted=" << evtCounted << ", CountFinal=" << (ctFinal?"YES":"NO"));
*/
    }
//  }

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
  return outwork;

}

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedReaderTool::fin_func()
{
  // Dummy
  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = 0; // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);
  return outwork;
}
