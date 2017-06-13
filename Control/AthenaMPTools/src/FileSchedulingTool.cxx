/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileSchedulingTool.h"
#include "copy_file_icc_hack.h"
#include "AthenaInterprocess/ProcessGroup.h"

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IFileMgr.h"
#include "GaudiKernel/IIncidentSvc.h"

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
  , m_debug(false)
  , m_sharedRankQueue(0)
  , m_sharedFinQueue(0)
  , m_isPileup(false)
{
  declareInterface<IAthenaMPTool>(this);

  declareProperty("IsPileup",m_isPileup);
  declareProperty("Debug", m_debug);

  m_subprocDirPrefix = "worker_";
}

FileSchedulingTool::~FileSchedulingTool()
{
}

StatusCode FileSchedulingTool::initialize()
{
  ATH_MSG_DEBUG("In initialize");
  if(m_isPileup) {
    m_evtProcessor = ServiceHandle<IEventProcessor>("PileUpEventLoopMgr",name());
    ATH_MSG_INFO("The job running in pileup mode");
  }
  else {
    ATH_MSG_INFO("The job running in non-pileup mode");
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
  ATH_MSG_DEBUG("In makePool " << getpid());

  if(nprocs==0 || nprocs<-1) {
    ATH_MSG_ERROR("Invalid value for the nprocs parameter: " << nprocs);
    return -1;
  }

  if(maxevt < -1) {
    ATH_MSG_ERROR("Invalid number of events requested: " << maxevt);
    return -1;
  }

  if(topdir.empty()) {
    ATH_MSG_ERROR("Empty name for the top directory!");
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
      ATH_MSG_ERROR("Unable to send int to the ranks queue!");
      return -1;
    }

  // Create finalization scheduling queue
  std::ostringstream finQueueName;
  finQueueName << "FileSchedulingTool_FinQueue_" << getpid();
  m_sharedFinQueue = new AthenaInterprocess::SharedQueue(finQueueName.str(),m_nprocs,sizeof(int));
  for(int i=0; i<m_nprocs; ++i)
    if(!m_sharedFinQueue->send_basic<int>(i*10)) {  // TO DO: this '3' could be made configurable
      ATH_MSG_ERROR("Unable to send int to the finalization queue!");
      return -1;
    }

  // Create the process group and map_async bootstrap
  m_processGroup = new AthenaInterprocess::ProcessGroup(m_nprocs);
  ATH_MSG_INFO("Created Pool of " << m_nprocs << " worker processes");
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_BOOTSTRAP))
    return -1;
  ATH_MSG_INFO("Workers bootstraped");

  return m_nprocs;
}

StatusCode FileSchedulingTool::exec()
{
  ATH_MSG_DEBUG("In exec " << getpid());

  if(mapAsyncFlag(AthenaMPToolBase::FUNC_EXEC))
    return StatusCode::FAILURE;
  ATH_MSG_INFO("Workers started processing events");

  // Map exit flag on children
  if(m_processGroup->map_async(0,0)){
    ATH_MSG_ERROR("Unable to set exit to the workers");
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

std::unique_ptr<AthenaInterprocess::ScheduledWork> FileSchedulingTool::bootstrap_func()
{
  if(m_debug) waitForSignal();

  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = 1; // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);

  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...

  // ________________________ Get IncidentSvc and fire PostFork ________________________
  IIncidentSvc* p_incidentSvc(0);
  if(!serviceLocator()->service("IncidentSvc", p_incidentSvc).isSuccess()) {
    msg(MSG::ERROR) << "Unable to retrieve IncidentSvc" << endreq;
    return outwork;
  }
  p_incidentSvc->fireIncident(Incident(name(),"PostFork"));

  // ________________________ Get RankID ________________________
  //
  if(!m_sharedRankQueue->receive_basic<int>(m_rankId)) {
    ATH_MSG_ERROR("Unable to get rank ID!");
    return outwork;
  }
  std::ostringstream workindex;
  workindex<<m_rankId;

  // ________________________ Worker dir: mkdir ________________________ 
  boost::filesystem::path worker_rundir(m_subprocTopDir);
  worker_rundir /= boost::filesystem::path(m_subprocDirPrefix+workindex.str());
  // TODO: this "worker_" can be made configurable too

  if(mkdir(worker_rundir.string().c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)==-1) {
    ATH_MSG_ERROR("Unable to make worker run directory: " << worker_rundir.string() << ". " << strerror(errno));
    return outwork;
  }

  // ________________________ Redirect logs and add timestamps to them________________________
  if(!m_debug) {
    if(redirectLog(worker_rundir.string()))
      return outwork;
    
    ATH_MSG_INFO("Logs redirected in the AthenaMP event worker PID=" << getpid());
  }

  // ________________________ Update event selector property ________________________
  IProperty* propertyServer = dynamic_cast<IProperty*>(m_evtSelector);
  if(!propertyServer) {
     ATH_MSG_ERROR("Unable to dyn-cast the event selector to IProperty");
     return outwork;
  }
  std::string propertyName("");
  // -- Try BS first
  if(serviceLocator()->existsService("ByteStreamInputSvc")) {
     propertyName = "Input";
  }
  else {
     // -- Fail over to the POOL event selector
     propertyName = "InputCollections";
  }

  std::vector<std::string> vect;
  StringArrayProperty inputFileList(propertyName, vect);
  StatusCode sc = propertyServer->getProperty(&inputFileList);
  if(sc.isFailure() && propertyName == "Input") {
     if(serviceLocator()->service("ByteStreamInputSvc",propertyServer).isFailure()) {
        ATH_MSG_ERROR("Unable to retrieve ByteStreamInputSvc");
        return outwork;
     }
     inputFileList.setName("FullFileName");
     sc = propertyServer->getProperty(&inputFileList);
  }
  if(sc.isFailure()) {
     ATH_MSG_WARNING("Unable to get " << inputFileList.name() << " property from the property server");
     //return outwork;
  }
  if(inputFileList.value().size()>0) {
    for(std::size_t i=0, imax=inputFileList.value().size(); i!=imax; ++i) {
      if((int)i%m_nprocs==m_rankId) {
        vect.push_back(inputFileList.value()[i]);
        ATH_MSG_INFO("Assigning input file #" << i << ": " << inputFileList.value()[i]);
      }
    }
    StringArrayProperty newInputFileList(inputFileList.name(), vect);
    if(propertyServer->setProperty(newInputFileList).isFailure()) {
      ATH_MSG_ERROR("Unable to update " << newInputFileList.name() << " property on the property server");
      return outwork;
    }
  }

  // ________________________ Update Io Registry ____________________________
  if(updateIoReg(worker_rundir.string()))
    return outwork;

  ATH_MSG_INFO("Io registry updated in the AthenaMP event worker PID=" << getpid());

  // ________________________ SimParams & DigiParams & PDGTABLE.MeV ____________________________
  boost::filesystem::path abs_worker_rundir = boost::filesystem::absolute(worker_rundir);
  if(boost::filesystem::is_regular_file("SimParams.db"))
    COPY_FILE_HACK("SimParams.db", abs_worker_rundir.string()+"/SimParams.db");
  if(boost::filesystem::is_regular_file("DigitParams.db"))
    COPY_FILE_HACK("DigitParams.db", abs_worker_rundir.string()+"/DigitParams.db");
  if(boost::filesystem::is_regular_file("PDGTABLE.MeV"))
    COPY_FILE_HACK("PDGTABLE.MeV", abs_worker_rundir.string()+"/PDGTABLE.MeV");

  // _______________________ Handle saved PFC (if any) ______________________
  if(handleSavedPfc(abs_worker_rundir))
    return outwork;

  // ________________________  reopen descriptors ____________________________ 
  if(reopenFds())
    return outwork;

  ATH_MSG_INFO("File descriptors re-opened in the AthenaMP event worker PID=" << getpid());

  // ________________________ I/O reinit ________________________
  if(!m_ioMgr->io_reinitialize().isSuccess()) {
    ATH_MSG_ERROR("Failed to reinitialize I/O");
    return outwork;
  } else {
    ATH_MSG_DEBUG("Successfully reinitialized I/O");
  }

  // ________________________ Event selector restart ________________________
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

  // ________________________ Restart background event selectors in pileup jobs ________________________
  if(m_isPileup) {
    const std::list<IService*>& service_list = serviceLocator()->getServices();
    std::list<IService*>::const_iterator itSvc = service_list.begin(),
      itSvcLast = service_list.end();
    for(;itSvc!=itSvcLast;++itSvc) {
      IEvtSelector* evtsel = dynamic_cast<IEvtSelector*>(*itSvc);
      if(evtsel && (evtsel != m_evtSelector)) {
	if((*itSvc)->start().isSuccess())
	  ATH_MSG_DEBUG("Restarted event selector " << (*itSvc)->name());
	else {
	  ATH_MSG_ERROR("Failed to restart event selector " << (*itSvc)->name());
	  return outwork;
	}
      }
    }
  }

  // ________________________ Worker dir: chdir ________________________
  if(chdir(worker_rundir.string().c_str())==-1) {
    ATH_MSG_ERROR("Failed to chdir to " << worker_rundir.string());
    return outwork;
  }

  // Declare success and return
  *(int*)(outwork->data) = 0;
  return outwork;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> FileSchedulingTool::exec_func()
{
  ATH_MSG_INFO("Exec function in the AthenaMP worker PID=" << getpid());

  bool all_ok(true);
  if(m_evtProcessor->executeRun(m_nevts).isFailure()) {
    ATH_MSG_ERROR("Could not finalize the Run");
    all_ok=false;
  }

  // Schedule finalization
  int waittime(-1);
  if(!m_sharedFinQueue->receive_basic<int>(waittime)) {
    ATH_MSG_ERROR("Unable to value from the finalization queue");
    all_ok = false;
  }
  else {
    usleep(waittime*1000000);
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
  return outwork;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> FileSchedulingTool::fin_func()
{
  // Dummy
  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = 0; // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);
  return outwork;
}
