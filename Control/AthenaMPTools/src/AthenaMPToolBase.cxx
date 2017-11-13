/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMPToolBase.h"
#include "copy_file_icc_hack.h"
#include "AthenaInterprocess/ProcessGroup.h"

#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IFileMgr.h"
#include "GaudiKernel/IMessageSvc.h"

#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>

#include <iterator>

#include "unistd.h"
#include <signal.h>

namespace AthenaMPToolBase_d {
  bool sig_done = false;
  void pauseForDebug(int /*sig*/) {
    sig_done = true;
  }
}

AthenaMPToolBase::AthenaMPToolBase(const std::string& type
				   , const std::string& name
				   , const IInterface* parent)
  : AthAlgTool(type,name,parent)
  , m_nprocs(-1)
  , m_subprocTopDir("")
  , m_subprocDirPrefix("")     // To be set in the derived classes
  , m_evtSelName("")
  , m_processGroup(0)
  , m_evtProcessor("AthenaEventLoopMgr",name)
  , m_appMgr("ApplicationMgr",name)
  , m_fileMgr("FileMgr",name)
  , m_ioMgr("IoComponentMgr",name)
  , m_evtSelector(0)
  , m_fileMgrLog("")
{
  declareInterface<IAthenaMPTool>(this);
}

AthenaMPToolBase::~AthenaMPToolBase()
{
}

StatusCode AthenaMPToolBase::initialize()
{
  ATH_MSG_DEBUG("In initialize");
  StatusCode sc = m_evtProcessor.retrieve();
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR("Error retrieving wrapped Event Loop Manager");
    return sc;
  }

  sc = m_appMgr.retrieve();
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR("Error retrieving ApplicationMgr");
    return sc;
  }

  SmartIF<IProperty> prpMgr(serviceLocator());
  if(prpMgr.isValid()) {
    // Get event selector name. Retrieve EventSelector and EventSeek
    m_evtSelName = prpMgr->getProperty("EvtSel").toString();
    sc = serviceLocator()->service(m_evtSelName,m_evtSelector);
    if(sc.isFailure() || m_evtSelector==0) {
      ATH_MSG_ERROR("Error retrieving EventSelector");
      return StatusCode::FAILURE;
    }
  }
  else {
    ATH_MSG_ERROR("IProperty interface not found in ApplicationMgr");
    return StatusCode::FAILURE;
  }

  sc = m_fileMgr.retrieve();
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR("Error retrieving FileMgr");
    return sc;
  }

  SmartIF<IProperty> prpMgr1(&*m_fileMgr);
  if(prpMgr1.isValid()) {
    m_fileMgrLog = prpMgr1->getProperty("LogFile").toString();
  }
  else {
    ATH_MSG_ERROR("IProperty interface not found in FileMgr");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode AthenaMPToolBase::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode AthenaMPToolBase::wait_once(pid_t& pid)
{
  bool flag(true);
  pid = m_processGroup->wait_once(flag);

  if(flag) { // Either none of the processes changed its status or one of the processes finished successfully
    return StatusCode::SUCCESS;
  }
  else {
    if(pid<0) { // Wait failed on the group
      ATH_MSG_ERROR("Wait failed on the Process Group!");
    }
    else {
      ATH_MSG_WARNING("Abnormal termination of the process PID=" << pid); 
    }
    return StatusCode::FAILURE;
  }
}

void AthenaMPToolBase::reportSubprocessStatuses()
{
  ATH_MSG_INFO("Statuses of sub-processes");
  const std::vector<AthenaInterprocess::ProcessStatus>& statuses = m_processGroup->getStatuses();
  for(size_t i=0; i<statuses.size(); ++i)
    ATH_MSG_INFO("*** Process PID=" << statuses[i].pid << ". Status " << ((statuses[i].exitcode)?"FAILURE":"SUCCESS"));
}

AthenaMP::AllWorkerOutputs_ptr AthenaMPToolBase::generateOutputReport()
{
  AthenaMP::AllWorkerOutputs_ptr jobOutputs(new AthenaMP::AllWorkerOutputs());

  if(m_fileMgrLog.empty()) {
    ATH_MSG_WARNING(name() << " cannot make output report because FileMgr has not been configured to write log file!");
  }
  else {
    // Collect output files made by the workers
    std::string line;
    for(int i=0;i<m_nprocs;++i) {
      // Get the name of FileMgr log
      std::ostringstream workindex;
      workindex<<i;
      boost::filesystem::path logFilePath(m_subprocTopDir);
      logFilePath /= boost::filesystem::path(m_subprocDirPrefix+workindex.str());
      boost::filesystem::path logFile(logFilePath);
      logFile /= boost::filesystem::path(m_fileMgrLog);
      if(!(boost::filesystem::exists(logFile)&&boost::filesystem::is_regular_file(logFile))) {
        ATH_MSG_WARNING(logFile.string() << " either does not exist or is not a regular file. Skipping");
        continue;
      }

      ATH_MSG_DEBUG("FileMgr log file (" << i << ") " << logFile);

      std::ifstream inpStream(logFile.string().c_str());
      std::set<std::string> reportedFiles; // Don't report the same file twice
      while(!inpStream.eof()) {
	std::getline(inpStream,line);
        if(line.find("WRITE")!=std::string::npos) {
          // Parse the entry
          size_t startpos(0);
	  std::vector<std::string> entries;
          while(startpos<line.size()) {
            while(line[startpos]==' ')
              startpos++;

            size_t endpos = line.find(' ',startpos);
            if(endpos==std::string::npos) endpos = line.size();
            entries.push_back(line.substr(startpos,endpos-startpos));
            startpos=endpos+1;
          }

          // enties[0] is filename
	  boost::filesystem::path filenamePath(entries[0]);
	  boost::filesystem::path basename = filenamePath.filename();
          if(reportedFiles.find(basename.string())==reportedFiles.end())
            reportedFiles.insert(basename.string());
          else
            continue;
	  boost::filesystem::path absolutename = basename.is_absolute() ? basename : boost::filesystem::absolute(boost::filesystem::path(logFilePath)/=basename);
	  AthenaMP::AllWorkerOutputsIterator it1 = jobOutputs->find(basename.string());
          if(it1==jobOutputs->end()) {
            (*jobOutputs)[basename.string()] = AthenaMP::SingleWorkerOutputs();
            (*jobOutputs)[basename.string()].reserve(m_nprocs);
          }

	  AthenaMP::WorkerOutput newOutput;
          newOutput._filename = absolutename.string();
          newOutput._technology = entries[1];
          newOutput._description = entries[2];
          newOutput._access_mode = entries[3];
          newOutput._shared = (line.find("SHARED")!=std::string::npos);

          (*jobOutputs)[basename.string()].push_back(newOutput);
        }
      }
    }
  }
  return jobOutputs;
}

void AthenaMPToolBase::useFdsRegistry(boost::shared_ptr<AthenaInterprocess::FdsRegistry> registry)
{
  m_fdsRegistry = registry;
}

void AthenaMPToolBase::setRandString(const std::string& randStr)
{
  m_randStr = randStr;
}

void AthenaMPToolBase::killChildren()
{
  for(auto child : m_processGroup->getChildren()) {
    kill(child.getProcessID(),SIGKILL);
  }
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> AthenaMPToolBase::operator()(const AthenaInterprocess::ScheduledWork& param)
{
  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork;
  bool all_ok(true);

  if(param.size==sizeof(Func_Flag)) {
    Func_Flag flag;
    memcpy(&flag,param.data,param.size);
    switch(flag) {
    case FUNC_BOOTSTRAP:
      {
        outwork = bootstrap_func();
        break;
      }
    case FUNC_EXEC:
      {
        outwork = exec_func();
        break;
      }
    case FUNC_FIN:
      {
        outwork = fin_func();
        break;
      }
    default:
      {
	ATH_MSG_ERROR("Unexpected value for the function flag");
        all_ok = false;
      }
    }
  }
  else {
    ATH_MSG_ERROR("Unexpected parameter size");
    all_ok = false;
  }

  if(!all_ok) {
    outwork = std::unique_ptr<AthenaInterprocess::ScheduledWork>(new AthenaInterprocess::ScheduledWork);
    outwork->data = malloc(sizeof(int));
    *(int*)(outwork->data) = 1; // Error code: for now use 0 success, 1 failure
    outwork->size = sizeof(int);
  }

  return outwork;
}

int AthenaMPToolBase::mapAsyncFlag(Func_Flag flag, pid_t pid)
{
  AthenaInterprocess::ScheduledWork params;
  params.data = (void*)(&flag);
  params.size = sizeof(flag);
  if(m_processGroup->map_async(this,&params,pid)){
    if(pid)
      ATH_MSG_ERROR("Unable to map the flag on subprocess pid=" << pid);
    else
      ATH_MSG_ERROR("Unable to map the flag on all subprocesses in the group");
    return -1;
  }
  return 0;
}

int AthenaMPToolBase::redirectLog(const std::string& rundir, bool addTimeStamp)
{
  // Redirect both stdout and stderr to the same file AthenaMP.log
  int dup2result1(0), dup2result2(0);

  int newout = open(std::string(rundir+"/AthenaMP.log").c_str(),O_CREAT | O_RDWR, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
  if(newout==-1) {
    ATH_MSG_ERROR("Unable to open log file in the run directory. " << strerror(errno));
    return -1;
  }
  dup2result1 = dup2(newout, STDOUT_FILENO);
  dup2result2 = dup2(newout, STDERR_FILENO);
  TEMP_FAILURE_RETRY(close(newout));
  if(dup2result1==-1) {
    ATH_MSG_ERROR("Unable to redirect standard output. " << strerror(errno));
    return -1;
  }
  if(dup2result2==-1) {
    ATH_MSG_ERROR("Unable to redirect standard error. " << strerror(errno));
    return -1;
  }

  if(addTimeStamp) {
    IMessageSvc* messageSvc(0);
    StatusCode sc = serviceLocator()->service("MessageSvc",messageSvc);
    if(sc.isFailure()) {
      ATH_MSG_ERROR("Error retrieving IMessageSvc");
      return -1;
    }

    IProperty* propertyServer = dynamic_cast<IProperty*>(messageSvc);
    if(propertyServer==0) {
      ATH_MSG_ERROR("Unable to cast message svc to IProperty");
      return -1;
    }

    std::string propertyName("Format");
    std::string oldFormat("");
    StringProperty formatProp(propertyName,oldFormat);
    sc = propertyServer->getProperty(&formatProp);
    if(sc.isFailure()) {
      ATH_MSG_WARNING("Message Service does not have Format property");
    }
    else {
      oldFormat = formatProp.value();
      if(oldFormat.find("%t")==std::string::npos) {
	// Add time stamps
	std::string newFormat("%t " + oldFormat);
	StringProperty newFormatProp(propertyName,newFormat);
	if(propertyServer->setProperty(newFormatProp).isFailure()) {
	  ATH_MSG_ERROR("Unable to set new Format property on the Message Service");
	  return -1;
	}
      }
      else {
	ATH_MSG_DEBUG("MsgSvc format already contains timestamps. Nothing to be done");
      }
    }
  }

  return 0;
}

int AthenaMPToolBase::updateIoReg(const std::string& rundir)
{
  if (!m_ioMgr.retrieve().isSuccess()) {
    ATH_MSG_ERROR("Error retrieving IoComponentMgr");
    return -1;
  } else {
    ATH_MSG_DEBUG("Successfully retrieved IoComponentMgr");
  }

  // update the IoRegistry for the new workdir - make sure we use absolute path
  boost::filesystem::path abs_rundir = boost::filesystem::absolute(rundir);
  if(!m_ioMgr->io_update_all(abs_rundir.string()).isSuccess()) {
    ATH_MSG_ERROR("Error updating IoRegistry");
    return -1;
  } else {
    ATH_MSG_DEBUG("Successfully updated IoRegistry");
  }

  return 0;
}

int AthenaMPToolBase::reopenFds()
{
  // Reopen file descriptors.
  // First go over all open files, which have been registered with the FileMgr
  // Then also check the FdsRegistry, in case it contains some files not registered with the FileMgr
  std::set<int> fdLog;
  
  // Query the FileMgr contents
  std::vector<const Io::FileAttr*> filemgrFiles;
  std::vector<const Io::FileAttr*>::const_iterator itFile;
  unsigned filenum = m_fileMgr->getFiles(filemgrFiles); // Get attributes for open files only. We don't care about closed ones at this point
  if(filenum!=filemgrFiles.size())
    ATH_MSG_WARNING("getFiles returned " << filenum << " while vector size is " << filemgrFiles.size());
  
  for(itFile=filemgrFiles.begin();itFile!=filemgrFiles.end();++itFile) {
    ATH_MSG_DEBUG("* " << **itFile);
    const std::string& filename = (**itFile).name();
    Io::Fd fd = (**itFile).fd();

    if(fd==-1) {
      // It is legal to have fd=-1 for remote inputs
      // On the other hand, these inputs should not remain open after fork. The issue being tracked at ATEAM-434.
      // So, this hopefully is a temporary patch
      ATH_MSG_WARNING("FD=-1 detected on an open file retrieved from FileMgr. Skip FD reopening. File name: " << filename);
      continue;
    }
    
    if(reopenFd(fd,filename))
      return -1;
    
    fdLog.insert(fd);
  }
  
  // Check the FdsRegistry
  AthenaInterprocess::FdsRegistry::const_iterator it(m_fdsRegistry->begin()), itEnd(m_fdsRegistry->end());
  for(;it!=itEnd;++it) {
    AthenaInterprocess::FdsRegistryEntry regEntry = *it;
    if(fdLog.find(regEntry.fd)!=fdLog.end()) {
      ATH_MSG_DEBUG("The file from FdsRegistry " << regEntry.name << " was registered with FileMgr. Skip reopening");
    }
    else {
      ATH_MSG_WARNING("The file " << regEntry.name << " has not been registered with the FileMgr!");
      
      if(regEntry.fd==-1) {
      // Same protection as the one above
	ATH_MSG_WARNING("FD=-1 detected on an open file retrieved from FD Registry. Skip FD reopening. File name: " << regEntry.name);
	continue;
      }

      if(reopenFd(regEntry.fd,regEntry.name))
        return -1;
      
      fdLog.insert(regEntry.fd);
    }
  }
  return 0;
}

int AthenaMPToolBase::handleSavedPfc(const boost::filesystem::path& dest_path)
{
  if(boost::filesystem::is_regular_file("PoolFileCatalog.xml.AthenaMP-saved"))
    COPY_FILE_HACK("PoolFileCatalog.xml.AthenaMP-saved",dest_path.string()+"/PoolFileCatalog.xml");
  return 0;
}

void AthenaMPToolBase::waitForSignal()
{
  ATH_MSG_INFO("Bootstrap worker PID " << getpid() << " - waiting for SIGUSR1");
  sigset_t mask, oldmask;
  
  signal(SIGUSR1, AthenaMPToolBase_d::pauseForDebug);
  
  sigemptyset (&mask);
  sigaddset (&mask, SIGUSR1);
  
  sigprocmask (SIG_BLOCK, &mask, &oldmask);
  while (!AthenaMPToolBase_d::sig_done)
    sigsuspend (&oldmask);
  sigprocmask (SIG_UNBLOCK, &mask, NULL);
}

int AthenaMPToolBase::reopenFd(int fd, const std::string& name)
{
  ATH_MSG_DEBUG("Attempting to reopen descriptor for " << name);
  int old_openflags = fcntl(fd,F_GETFL,0);
  switch(old_openflags & O_ACCMODE) {
  case O_RDONLY: {
    ATH_MSG_DEBUG("The File Access Mode is RDONLY");
    break;
  }
  case O_WRONLY: {
    ATH_MSG_DEBUG("The File Access Mode is WRONLY");
    break;
  }
  case O_RDWR: {
    ATH_MSG_DEBUG("The File Access Mode is RDWR");
    break;
  }
  }
   
  int old_descflags = fcntl(fd,F_GETFD,0);
  off_t oldpos = lseek(fd,0,SEEK_CUR);
  if(oldpos==-1) {
    if(errno==ESPIPE) {
      ATH_MSG_WARNING("Dealing with PIPE. Skipping ... (FIXME!)");
    }
    else {
      ATH_MSG_ERROR("When re-opening file descriptors lseek failed on " << name << ". " << strerror(errno));
      return -1;
    }
  }
  else {
    Io::Fd newfd = open(name.c_str(),old_openflags);
    if(newfd==-1) {
      ATH_MSG_ERROR("When re-opening file descriptors unable to open " << name << " for reading. " << strerror(errno));
      return -1;
    }
    if(lseek(newfd,oldpos,SEEK_SET)==-1){
      ATH_MSG_ERROR("When re-opening file descriptors lseek failed on the newly opened " << name << ". " << strerror(errno));
      TEMP_FAILURE_RETRY(close(newfd));
      return -1;
    }
    TEMP_FAILURE_RETRY(close(fd));
    if(dup2(newfd,fd)==-1) {
      ATH_MSG_ERROR("When re-opening file descriptors unable to duplicate descriptor for " << name << ". " << strerror(errno));
      TEMP_FAILURE_RETRY(close(newfd));
      return -1;
    }
    if(fcntl(fd,F_SETFD,old_descflags)==-1) {
      ATH_MSG_ERROR("When re-opening file descriptors unable to set descriptor flags for " << name << ". " << strerror(errno));
      TEMP_FAILURE_RETRY(close(newfd));
      return -1;
    }
    TEMP_FAILURE_RETRY(close(newfd));
  }
  return 0;
}
