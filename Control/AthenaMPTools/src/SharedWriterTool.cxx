/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SharedWriterTool.h"
#include "AthenaInterprocess/ProcessGroup.h"

#include "AthenaKernel/IEventShare.h"
#include "AthenaKernel/IDataShare.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IIoComponentMgr.h"

#include "AthenaBaseComps/AthCnvSvc.h"

SharedWriterTool::SharedWriterTool(const std::string& type
				   , const std::string& name
				   , const IInterface* parent)
  : AthenaMPToolBase(type,name,parent)
  , m_rankId(0)
  , m_writer(0)
  , m_cnvSvc(0)
{
  m_subprocDirPrefix = "shared_writer_";
}

SharedWriterTool::~SharedWriterTool()
{
}

StatusCode SharedWriterTool::initialize()
{
  ATH_MSG_DEBUG("In initialize");

  StatusCode sc = AthenaMPToolBase::initialize();
  if(!sc.isSuccess()) return sc;

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

int SharedWriterTool::makePool(int /*maxevt*/, int nprocs, const std::string& topdir)
{
  ATH_MSG_DEBUG("In makePool " << getpid());

  if(topdir.empty()) {
    ATH_MSG_ERROR("Empty name for the top directory!");
    return -1;
  }

  m_nprocs = (nprocs==-1?sysconf(_SC_NPROCESSORS_ONLN):nprocs);
  m_subprocTopDir = topdir;

  m_writer = 0;
  IProperty* propertyServer = dynamic_cast<IProperty*>(m_cnvSvc);
  if(propertyServer==0) {
    ATH_MSG_ERROR("Unable to cast conversion service to IProperty");
    return -1;
  }
  else {
    std::string propertyName("OutputStreamingTool");
    std::vector<std::string> writeClients(m_writer);
    StringArrayProperty writeClientsProp(propertyName,writeClients);
    if(propertyServer->getProperty(&writeClientsProp).isFailure()) {
      ATH_MSG_INFO("Conversion service does not have OutputStreamingTool property");
    }
    else {
      m_writer = writeClientsProp.value().size();
    }
  }

  // Create rank queue and fill it
  m_sharedRankQueue = new AthenaInterprocess::SharedQueue("SharedWriterTool_RankQueue_"+m_randStr,m_writer,sizeof(int));
  for(int i=0; i<m_writer; ++i)
    if(!m_sharedRankQueue->send_basic<int>(i)) {
      ATH_MSG_ERROR("Unable to send int to the ranks queue!");
      return -1;
    }

  // Create the process group and map_async bootstrap
  m_processGroup = new AthenaInterprocess::ProcessGroup(m_writer);
  ATH_MSG_INFO("Created Pool of " << m_writer << " shared writer processes");
  if(mapAsyncFlag(AthenaMPToolBase::FUNC_BOOTSTRAP))
    return -1;
  ATH_MSG_INFO("Shared writer processes bootstraped");
  return 1;
}

StatusCode SharedWriterTool::exec()
{
  ATH_MSG_DEBUG("In exec " << getpid());

  if(mapAsyncFlag(AthenaMPToolBase::FUNC_EXEC))
    return StatusCode::FAILURE;
  ATH_MSG_INFO("Shared writer started write events");

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
  for(int i=0; i<m_writer; ++i) {
    std::ostringstream workerIndex;
    workerIndex << i;
    boost::filesystem::path writer_rundir(m_subprocTopDir);
    writer_rundir/= boost::filesystem::path(m_subprocDirPrefix+workerIndex.str());
    filenames.push_back(writer_rundir.string()+std::string("/AthenaMP.log"));
  }
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

  // ________________________ Get RankID ________________________
  //
  if(!m_sharedRankQueue->receive_basic<int>(m_rankId)) {
    ATH_MSG_ERROR("Unable to get rank ID!");
    return outwork;
  }
  std::ostringstream workerIndex;
  workerIndex<<m_rankId;

  // Writer dir: mkdir
  boost::filesystem::path writer_rundir(m_subprocTopDir);
  writer_rundir /= boost::filesystem::path(m_subprocDirPrefix+workerIndex.str());

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

  // Use IDataShare to make ConversionSvc a Share Server
  IDataShare* cnvSvc = dynamic_cast<IDataShare*>(m_cnvSvc);
  if (cnvSvc == 0 || !cnvSvc->makeServer(-m_nprocs - 1 - 1024 * m_rankId).isSuccess()) {
    ATH_MSG_ERROR("Failed to make the conversion service a share server");
    return outwork;
  }
  else {
    ATH_MSG_DEBUG("Successfully made the conversion service a share server");
  }

  // ________________________ I/O reinit ________________________
  if(!m_ioMgr->io_reinitialize().isSuccess()) {
    ATH_MSG_ERROR("Failed to reinitialize I/O");
    return outwork;
  } else {
    ATH_MSG_DEBUG("Successfully reinitialized I/O");
  }

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

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedWriterTool::fin_func()
{
  // Dummy
  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);
  outwork->data = malloc(sizeof(int));
  *(int*)(outwork->data) = 0; // Error code: for now use 0 success, 1 failure
  outwork->size = sizeof(int);
  return outwork;
}
