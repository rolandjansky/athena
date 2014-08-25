/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileSchedulingTool.h"
#include "copy_file_icc_hack.h"
#include "AthenaInterprocess/ProcessGroup.h"

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IFileMgr.h"

#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdexcept>

FileSchedulingTool::FileSchedulingTool(const std::string& type
				       , const std::string& name
				       , const IInterface* parent)
  : AthenaMPToolBase(type,name,parent)
  , m_nevts(-1)
  , m_rankId(-1)
  , m_sharedRankQueue(0)
  , m_sharedFinQueue(0)
  , m_isPileup(false)
{
  declareInterface<IAthenaMPTool>(this);
  declareProperty("IsPileup",m_isPileup);
  m_subprocDirPrefix = "worker_";
}

FileSchedulingTool::~FileSchedulingTool()
{
}

StatusCode FileSchedulingTool::initialize()
{
  msg(MSG::DEBUG) << "In initialize" << endreq;
  if(m_isPileup) {
    m_evtProcessor = ServiceHandle<IEventProcessor>("PileUpEventLoopMgr",name());
    msg(MSG::INFO) << "The job running in pileup mode" << endreq;
  }
  else {
    msg(MSG::INFO) << "The job running in non-pileup mode" << endreq;
  }
  
  return AthenaMPToolBase::initialize();
}

StatusCode FileSchedulingTool::finalize()
{
  delete m_sharedRankQueue;
  delete m_sharedFinQueue;
  return StatusCode::SUCCESS;
}

int FileSchedulingTool::makePool(int maxevt, int nprocs, const std::string& topdir)
{
  msg(MSG::DEBUG) << "In makePool " << getpid() << endreq;

  if(nprocs==0 || nprocs<-1) {
    msg(MSG::ERROR) << "Invalid value for the nprocs parameter: " << nprocs << endreq;
    return -1;
  }

  if(maxevt < -1) {
    msg(MSG::ERROR) << "Invalid number of events requested: " << maxevt << endreq;
    return -1;
  }

  if(topdir.empty()) {
    msg(MSG::ERROR) << "Empty name for the top directory!" << endreq;
    return -1;
  }

  m_nevts = maxevt;
  m_nprocs = (nprocs==-1?sysconf(_SC_NPROCESSORS_ONLN):nprocs);
  m_subprocTopDir = topdir;

  // Create rank queue and fill it
  std::ostringstream rankQueueName;
  rankQueueName << "FileSchedulingTool_RankQueue_" << getpid();
  m_sharedRankQueue = new AthenaInterprocess::SharedQueue(rankQueueName.str(),m_nprocs,sizeof(int));
  for(int i=0; i<m_nprocs; ++i)
    if(!m_sharedRankQueue->send_basic<int>(i)) {
      msg(MSG::ERROR) << "Unable to send int to the ranks queue!" << endreq;
      return -1;
    }

  // Create finalization scheduling queue
  std::ostringstream finQueueName;
  finQueueName << "FileSchedulingTool_FinQueue_" << getpid();
  m_sharedFinQueue = new AthenaInterprocess::SharedQueue(finQueueName.str(),m_nprocs,sizeof(int));
  for(int i=0; i<m_nprocs; ++i)
    if(!m_sharedFinQueue->send_basic<int>(i*10)) {  // TO DO: this '3' could be made configurable
      msg(MSG::ERROR) << "Unable to send int to the finalization queue!" << endreq;
      return -1;
    }

  // Create the process group and map_async bootstrap
  m_processGroup = new AthenaInterprocess::ProcessGroup(m_nprocs);
  msg(MSG::INFO) << "Created Pool of " << m_nprocs << " worker processes" << endreq;
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_BOOTSTRAP))
    return -1;
  msg(MSG::INFO) << "Workers bootstraped" << endreq;

  return m_nprocs;
}

StatusCode FileSchedulingTool::exec()
{
  msg(MSG::DEBUG) << "In exec " << getpid() << endreq;

  if(mapAsyncFlag(AthenaMPToolBase::FUNC_EXEC))
    return StatusCode::FAILURE;
  msg(MSG::INFO) << "Workers started processing events" << endreq;

  // Map exit flag on children
  if(m_processGroup->map_async(0,0)){
    msg(MSG::ERROR) << "Unable to set exit to the workers" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void FileSchedulingTool::subProcessLogs(std::vector<std::string>& filenames)
{
  filenames.clear();
  for(int i=0; i<m_nprocs; ++i) {
    std::ostringstream workerIndex;
    workerIndex << i;
    boost::filesystem::path worker_rundir(m_subprocTopDir);
    worker_rundir /= boost::filesystem::path(m_subprocDirPrefix+workerIndex.str());
    filenames.push_back(worker_rundir.string()+std::string("/AthenaMP.log"));
  }
}

AthenaInterprocess::ScheduledWork* FileSchedulingTool::bootstrap_func()
{
  int* errcode = new int(1); // For now use 0 success, 1 failure
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = (void*)errcode;
  outwork->size = sizeof(int);
  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...

  // ________________________ Get RankID ________________________
  //
  if(!m_sharedRankQueue->receive_basic<int>(m_rankId)) {
    msg(MSG::ERROR) << "Unable to get rank ID!" << endreq;
    return outwork;
  }
  std::ostringstream workindex;
  workindex<<m_rankId;

  // ________________________ Worker dir: mkdir ________________________ 
  boost::filesystem::path worker_rundir(m_subprocTopDir);
  worker_rundir /= boost::filesystem::path(m_subprocDirPrefix+workindex.str());
  // TODO: this "worker_" can be made configurable too

  if(mkdir(worker_rundir.string().c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)==-1) {
    msg(MSG::ERROR) << "Unable to make worker run directory: " << worker_rundir.string() << ". " << strerror(errno) << endreq;
    return outwork;
  }

  // ________________________ Redirect logs ________________________
  if(redirectLog(worker_rundir.string()))
    return outwork;

  msg(MSG::INFO) << "Logs redirected in the AthenaMP event worker PID=" << getpid() << endreq;

  // ________________________ Update event selector property ________________________
  IProperty* propertyServer(0);
  std::string propertyName("");
  // -- Try BS first
  if(serviceLocator()->existsService("ByteStreamInputSvc")) {
    if(serviceLocator()->service("ByteStreamInputSvc",propertyServer).isFailure()) {
      msg(MSG::ERROR) << "Unable to retrieve ByteStreamInputSvc" << endreq;
      return outwork;
    }
    propertyName = "FullFileName";
  }
  else {
    // -- Fail over to the POOL event selector
    propertyServer = dynamic_cast<IProperty*>(m_evtSelector);
    if(!propertyServer) {
      msg(MSG::ERROR) << "Unable to dyn-cast the event selector to IProperty" << endreq;
      return outwork;
    }
    propertyName = "InputCollections";
  }
  
  std::vector<std::string> vect;
  StringArrayProperty inputFileList(propertyName, vect);
  StatusCode sc = propertyServer->getProperty(&inputFileList);
  if(sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to get " << propertyName << " property from the property server" << endreq;
    return outwork;
  }
  for(std::size_t i=0, imax=inputFileList.value().size(); i!=imax; ++i)
    if((int)i%m_nprocs==m_rankId)
      vect.push_back(inputFileList.value()[i]);
  StringArrayProperty newInputFileList(propertyName, vect);
  if(propertyServer->setProperty(newInputFileList).isFailure()) {
    msg(MSG::ERROR) << "Unable to update " << propertyName << " property on the property server" << endreq;
    return outwork;
  }

  // ________________________ Update Io Registry ____________________________
  if(updateIoReg(worker_rundir.string()))
    return outwork;

  msg(MSG::INFO) << "Io registry updated in the AthenaMP event worker PID=" << getpid() << endreq;

  // ________________________ SimParams & DigiParams ____________________________
  boost::filesystem::path abs_worker_rundir = boost::filesystem::absolute(worker_rundir);
  if(boost::filesystem::is_regular_file("SimParams.db"))
    COPY_FILE_HACK("SimParams.db", abs_worker_rundir.string()+"/SimParams.db");
  if(boost::filesystem::is_regular_file("DigitParams.db"))
    COPY_FILE_HACK("DigitParams.db", abs_worker_rundir.string()+"/DigitParams.db");

  // _______________________ Handle saved PFC (if any) ______________________
  if(handleSavedPfc(abs_worker_rundir))
    return outwork;

  // ________________________  reopen descriptors ____________________________ 
  if(reopenFds())
    return outwork;

  msg(MSG::INFO) << "File descriptors re-opened in the AthenaMP event worker PID=" << getpid() << endreq;

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

  // ________________________ Restart background event selectors in pileup jobs ________________________
  if(m_isPileup) {
    const std::list<IService*>& service_list = serviceLocator()->getServices();
    std::list<IService*>::const_iterator itSvc = service_list.begin(),
      itSvcLast = service_list.end();
    for(;itSvc!=itSvcLast;++itSvc) {
      IEvtSelector* evtsel = dynamic_cast<IEvtSelector*>(*itSvc);
      if(evtsel && (evtsel != m_evtSelector)) {
	if((*itSvc)->start().isSuccess())
	  msg(MSG::DEBUG) << "Restarted event selector " << (*itSvc)->name() << endreq;
	else {
	  msg(MSG::ERROR) << "Failed to restart event selector " << (*itSvc)->name() << endreq;
	  return outwork;
	}
      }
    }
  }

  // ________________________ Worker dir: chdir ________________________
  if(chdir(worker_rundir.string().c_str())==-1) {
    msg(MSG::ERROR) << "Failed to chdir to " << worker_rundir.string() << endreq;
    return outwork;
  }

  // Declare success and return
  *errcode = 0;
  return outwork;
}

AthenaInterprocess::ScheduledWork* FileSchedulingTool::exec_func()
{
  msg(MSG::INFO) << "Exec function in the AthenaMP worker PID=" << getpid() << endreq;

  bool all_ok(true);
  if(m_evtProcessor->executeRun(m_nevts).isFailure()) {
    msg(MSG::ERROR) << "Could not finalize the Run" << endreq;
    all_ok=false;
  }

  // Schedule finalization
  int waittime(-1);
  if(!m_sharedFinQueue->receive_basic<int>(waittime)) {
    msg(MSG::ERROR) << "Unable to value from the finalization queue" << endreq;
    all_ok = false;
  }
  else {
    usleep(waittime*1000000);
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

AthenaInterprocess::ScheduledWork* FileSchedulingTool::fin_func()
{
  // Dummy
  int* errcode = new int(0); 
  AthenaInterprocess::ScheduledWork* outwork = new AthenaInterprocess::ScheduledWork;
  outwork->data = (void*)errcode;
  outwork->size = sizeof(int);
  return outwork;
}
