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
  , m_shmemSegment()
  , m_evtShare(0)
{
  m_subprocDirPrefix = "shared_reader";
}

SharedReaderTool::~SharedReaderTool()
{
}

StatusCode SharedReaderTool::initialize()
{
  msg(MSG::DEBUG) << "In initialize" << endreq;

  StatusCode sc = AthenaMPToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  sc = serviceLocator()->service(m_evtSelName,m_evtShare);
  if(sc.isFailure() || m_evtShare==0) {
    msg(MSG::ERROR) << "Error retrieving IEventShare" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SharedReaderTool::finalize()
{
  return StatusCode::SUCCESS;
}

int SharedReaderTool::makePool(int maxevt, int, const std::string& topdir)
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
  m_subprocTopDir = topdir;

  // Create the process group with only one process and map_async bootstrap
  m_processGroup = new AthenaInterprocess::ProcessGroup(1);
  msg(MSG::INFO) << "Shared Reader process created" << endreq;
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_BOOTSTRAP))
    return -1;

  return 1;  
}

StatusCode SharedReaderTool::exec()
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

AthenaInterprocess::ScheduledWork* SharedReaderTool::bootstrap_func()
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

  // IEventShare ...
  if(!m_evtShare->makeServer(1).isSuccess()) {
    msg(MSG::ERROR) << "Failed to make the event selector a share server" << endreq;
    return outwork;
  }
  else {
    msg(MSG::DEBUG) << "Successfully made the event selector a share server" << endreq;
  }

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

  // Instantiate shared memory segment in the reader
  try {
    m_shmemSegment = boost::shared_ptr<yampl::SharedMemory>(new yampl::SharedMemory("/athmp-shmem-"+m_randStr,2*sizeof(int)));
  }
  catch(yampl::ErrnoException& ex) {
    msg(MSG::ERROR) << "Exception caught when trying to acquire shared memory segment: " << ex.what() << endreq;
    return outwork;
  }

  // Declare success and return
  *errcode = 0;
  return outwork;
}

AthenaInterprocess::ScheduledWork* SharedReaderTool::exec_func()
{
  msg(MSG::INFO) << "Exec function in the AthenaMP Shared Reader PID=" << getpid() << endreq;

  int eventsRead(0);
  int* shmemCountedEvts = (int*)m_shmemSegment->getMemory();
  int* shmemCountFinal = shmemCountedEvts+1;

  int ctFinal(*shmemCountFinal), evtCounted(*shmemCountedEvts);
  while(evtCounted==0 && ctFinal==0) { 
    usleep(10);
    ctFinal = *shmemCountFinal;
    evtCounted = *shmemCountedEvts;
  }
  msg(MSG::DEBUG) << "SharedReaderTool::exec_func evtCounted=" << evtCounted << ", CountFinal=" << (ctFinal?"YES":"NO") << endreq;

  bool all_ok=true;
  msg(MSG::DEBUG) << "SharedReaderTool::exec_func entering loop " << endreq;
  while(eventsRead<evtCounted) {
    msg(MSG::DEBUG) << "SharedReaderTool::exec_func loop " << endreq;
    if(m_evtShare->readEvent(evtCounted-eventsRead).isFailure()) {
      msg(MSG::ERROR) << "Failed to read " << evtCounted-eventsRead << " events" << endreq;
      all_ok=false;
    }
    else {

      msg(MSG::DEBUG) << "readEvent succeeded" << endreq;
      if(ctFinal)
	break;

      eventsRead=evtCounted;
      ctFinal = *shmemCountFinal;
      evtCounted = *shmemCountedEvts;
      msg(MSG::DEBUG) << "SharedReaderTool::exec_func evtCounted=" << evtCounted << ", CountFinal=" << (ctFinal?"YES":"NO") << endreq;

      while(eventsRead==evtCounted 
	    && !ctFinal) {
	usleep(10);
	ctFinal = *shmemCountFinal;
	evtCounted = *shmemCountedEvts;
      }
      msg(MSG::DEBUG) << "SharedReaderTool::exec_func evtCounted=" << evtCounted << ", CountFinal=" << (ctFinal?"YES":"NO") << endreq;
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
  return outwork;

}

AthenaInterprocess::ScheduledWork* SharedReaderTool::fin_func()
{
  // Dummy
  int* errcode = new int(0); 
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = (void*)errcode;
  outwork->size = sizeof(int);
  return outwork;
}
