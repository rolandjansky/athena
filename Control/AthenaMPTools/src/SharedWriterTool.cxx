/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SharedWriterTool.h"
#include "AthenaInterprocess/ProcessGroup.h"

#include "AthenaKernel/IEventShare.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IIoComponentMgr.h"

#include "AthenaBaseComps/AthCnvSvc.h"

SharedWriterTool::SharedWriterTool(const std::string& type
				   , const std::string& name
				   , const IInterface* parent)
  : AthenaMPToolBase(type,name,parent)
  , m_evtShare(0)
  , m_cnvSvc(0)
{
  m_subprocDirPrefix = "shared_writer";
}

SharedWriterTool::~SharedWriterTool()
{
}

StatusCode SharedWriterTool::initialize()
{
  ATH_MSG_DEBUG("In initialize");

  StatusCode sc = AthenaMPToolBase::initialize();
  if(!sc.isSuccess()) return sc;

  sc = serviceLocator()->service(m_evtSelName,m_evtShare);
  if(sc.isFailure() || m_evtShare==0) {
    ATH_MSG_ERROR("Error retrieving IEventShare");
    return StatusCode::FAILURE;
  }

//FIXME: AthenaPool dependent for now
  sc = serviceLocator()->service("AthenaPoolCnvSvc", m_cnvSvc);
  if(sc.isFailure() || m_cnvSvc==0) {
    ATH_MSG_ERROR("Error retrieving AthenaPoolCnvSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SharedWriterTool::finalize()
{
  return StatusCode::SUCCESS;
}

int SharedWriterTool::makePool(int maxevt, int nprocs, const std::string& topdir)
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
  ATH_MSG_INFO("Shared Writer process created");
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_BOOTSTRAP))
    return -1;

  return 1;  
}

StatusCode SharedWriterTool::exec()
{
  ATH_MSG_DEBUG("In exec " << getpid());

  if(mapAsyncFlag(AthenaMPToolBase::FUNC_EXEC))
    return StatusCode::FAILURE;

  // Set exit flag on writer
  if(m_processGroup->map_async(0,0)){
    ATH_MSG_ERROR("Unable to set exit to the writer");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void SharedWriterTool::subProcessLogs(std::vector<std::string>& filenames)
{
  filenames.clear();
  boost::filesystem::path writer_rundir(m_subprocTopDir);
  writer_rundir/= boost::filesystem::path(m_subprocDirPrefix);
  filenames.push_back(writer_rundir.string()+std::string("/AthenaMP.log"));
}

AthenaMP::AllWorkerOutputs_ptr SharedWriterTool::generateOutputReport()
{
  AthenaMP::AllWorkerOutputs_ptr jobOutputs(new AthenaMP::AllWorkerOutputs());
  return jobOutputs;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedWriterTool::bootstrap_func()
{
  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = 1; // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);

  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...

  // Writer dir: mkdir 
  boost::filesystem::path writer_rundir(m_subprocTopDir);
  writer_rundir /= boost::filesystem::path(m_subprocDirPrefix);

  if(mkdir(writer_rundir.string().c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)==-1) {
    ATH_MSG_ERROR("Unable to make writer run directory: " << writer_rundir.string() << ". " << strerror(errno));
    return outwork;
  }

  // Redirect logs
  if(redirectLog(writer_rundir.string()))
    return outwork;

  ATH_MSG_INFO("Logs redirected in the AthenaMP Shared Writer PID=" << getpid());
     
  // Update Io Registry   
  if(updateIoReg(writer_rundir.string()))
    return outwork;

  ATH_MSG_INFO("Io registry updated in the AthenaMP Shared Writer PID=" << getpid());

  // _______________________ Handle saved PFC (if any) ______________________
  boost::filesystem::path abs_writer_rundir = boost::filesystem::absolute(writer_rundir);
  if(handleSavedPfc(abs_writer_rundir))
    return outwork;

  // Reopen file descriptors
  if(reopenFds())
    return outwork;

  ATH_MSG_INFO("File descriptors re-opened in the AthenaMP Shared Writer PID=" << getpid());

  // FIXME: IEventShare ...
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
/*
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
*/

  // Writer dir: chdir 
  if(chdir(writer_rundir.string().c_str())==-1) {
    ATH_MSG_ERROR("Failed to chdir to " << writer_rundir.string());
    return outwork;
  }

  // Declare success and return
  *(int*)(outwork->data) = 0;
  return outwork;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedWriterTool::exec_func()
{
  ATH_MSG_INFO("Exec function in the AthenaMP Shared Writer PID=" << getpid());
  bool all_ok=true;

  StatusCode sc = m_cnvSvc->commitOutput("", false);
  while(sc.isSuccess() || sc.isRecoverable()) {
    if (sc.isRecoverable()) {
       usleep(100);
    }
    sc = m_cnvSvc->commitOutput("", false);
  }
  AthCnvSvc* cnvSvc = dynamic_cast<AthCnvSvc*>(m_cnvSvc);
  if (cnvSvc == 0 || !cnvSvc->disconnectOutput().isSuccess()) {
    ATH_MSG_ERROR("Exec function could not disconnectOutput");
    all_ok=false;
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

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedWriterTool::fin_func()
{
  // Dummy
  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = 0; // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);
  return outwork;
}
