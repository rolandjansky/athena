/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMPToolBase.h"
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

#include <iterator>

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
  msg(MSG::DEBUG) << "In initialize" << endreq;
  StatusCode sc = m_evtProcessor.retrieve();
  if(!sc.isSuccess()) {
    msg(MSG::ERROR) << "Error retrieving wrapped Event Loop Manager" <<  endreq;
    return sc;
  }

  sc = m_appMgr.retrieve();
  if(!sc.isSuccess()) {
    msg(MSG::ERROR) << "Error retrieving ApplicationMgr" <<  endreq;
    return sc;
  }

  SmartIF<IProperty> prpMgr(serviceLocator());
  if(prpMgr.isValid()) {
    // Get event selector name. Retrieve EventSelector and EventSeek
    m_evtSelName = prpMgr->getProperty("EvtSel").toString();
    sc = serviceLocator()->service(m_evtSelName,m_evtSelector);
    if(sc.isFailure() || m_evtSelector==0) {
      msg(MSG::ERROR) << "Error retrieving EventSelector" << endreq;
      return StatusCode::FAILURE;
    }
  }
  else {
    msg(MSG::ERROR) << "IProperty interface not found in ApplicationMgr" << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_fileMgr.retrieve();
  if(!sc.isSuccess()) {
    msg(MSG::ERROR) << "Error retrieving FileMgr" <<  endreq;
    return sc;
  }

  SmartIF<IProperty> prpMgr1(&*m_fileMgr);
  if(prpMgr1.isValid()) {
    m_fileMgrLog = prpMgr1->getProperty("LogFile").toString();
  }
  else {
    msg(MSG::ERROR) << "IProperty interface not found in FileMgr" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode AthenaMPToolBase::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode AthenaMPToolBase::wait_once(int& numFinishedProc)
{
  numFinishedProc = 0;
  bool processSuccessfull(true);
  pid_t pid = m_processGroup->wait_once(processSuccessfull);
  if(pid<0) {// wait failed
    msg(MSG::ERROR) << "Wait failed on the Process Group!" << endreq;
    return StatusCode::FAILURE;
  }
  else if(pid==0) // none of the processes changed it status
    return StatusCode::SUCCESS;

  // One of the processes actually finished
  // TODO: Attempt to restart? 
  numFinishedProc = 1;
  if(processSuccessfull)
    return StatusCode::SUCCESS;

  msg(MSG::INFO) << "Abnormal termination of the process PID=" << pid << endreq; 
  return StatusCode::FAILURE;
}

void AthenaMPToolBase::reportSubprocessStatuses()
{
  msg(MSG::INFO) << "Statuses of sub-processes" << endreq;
  const std::vector<AthenaInterprocess::ProcessStatus>& statuses = m_processGroup->getStatuses();
  for(size_t i=0; i<statuses.size(); ++i)
    msg(MSG::INFO) << "*** Process PID=" << statuses[i].pid << ". Status " << ((statuses[i].exitcode)?"FAILURE":"SUCCESS") << endreq;
}

AthenaMP::AllWorkerOutputs_ptr AthenaMPToolBase::generateOutputReport()
{
  AthenaMP::AllWorkerOutputs_ptr jobOutputs(new AthenaMP::AllWorkerOutputs());

  if(m_fileMgrLog.empty()) {
    msg(MSG::WARNING) << name() << " cannot make output report because FileMgr has not been configured to write log file!" << endreq;
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
        msg(MSG::WARNING) << logFile.string() << " either does not exist or is not a regular file. Skipping" << endreq;
        continue;
      }

      msg(MSG::DEBUG) << "FileMgr log file (" << i << ") " << logFile << endreq;

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

AthenaInterprocess::ScheduledWork* AthenaMPToolBase::operator()(const AthenaInterprocess::ScheduledWork& param)
{
  AthenaInterprocess::ScheduledWork* outwork(0);
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
	msg(MSG::ERROR) << "Unexpected value for the function flag" << endreq;
        all_ok = false;
      }
    }
  }
  else {
    msg(MSG::ERROR) << "Unexpected parameter size" << endreq;
    all_ok = false;
  }

  if(!all_ok) {
    int* errcode = new int(1); // For now use 0 success, 1 failure
    outwork = new AthenaInterprocess::ScheduledWork;
    outwork->data = (void*)errcode;
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
      msg(MSG::ERROR) << "Unable to map the flag on subprocess pid=" << pid << endreq;
    else
      msg(MSG::ERROR) << "Unable to map the flag on all subprocesses in the group" << endreq;
    return -1;
  }
  return 0;
}

int AthenaMPToolBase::redirectLog(const std::string& rundir)
{
  // Redirect both stdout and stderr to the same file AthenaMP.log
  int dup2result1(0), dup2result2(0);

  int newout = open(std::string(rundir+"/AthenaMP.log").c_str(),O_CREAT | O_RDWR, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
  if(newout==-1) {
    msg(MSG::ERROR) << "Unable to open log file in the run directory. " << strerror(errno) << endreq;
    return -1;
  }
  dup2result1 = dup2(newout, STDOUT_FILENO);
  dup2result2 = dup2(newout, STDERR_FILENO);
  TEMP_FAILURE_RETRY(close(newout));
  if(dup2result1==-1) {
    msg(MSG::ERROR) << "Unable to redirect standard output. " << strerror(errno) << endreq;
    return -1;
  }
  if(dup2result2==-1) {
    msg(MSG::ERROR) << "Unable to redirect standard error. " << strerror(errno) << endreq;
    return -1;
  }

  return 0;
}

int AthenaMPToolBase::updateIoReg(const std::string& rundir)
{
  if (!m_ioMgr.retrieve().isSuccess()) {
    msg(MSG::ERROR) << "Error retrieving IoComponentMgr" << endreq;
    return -1;
  } else {
    msg(MSG::DEBUG) << "Successfully retrieved IoComponentMgr" << endreq;
  }

  // update the IoRegistry for the new workdir - make sure we use absolute path
  boost::filesystem::path abs_rundir = boost::filesystem::absolute(rundir);
  if(!m_ioMgr->io_update_all(abs_rundir.string()).isSuccess()) {
    msg(MSG::ERROR) << "Error updating IoRegistry" << endreq;
    return -1;
  } else {
    msg(MSG::DEBUG) << "Successfully updated IoRegistry" << endreq;
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
    msg(MSG::WARNING) << "getFiles returned " << filenum << " while vector size is " << filemgrFiles.size() << endreq;
  
  for(itFile=filemgrFiles.begin();itFile!=filemgrFiles.end();++itFile) {
    msg(MSG::DEBUG) << "* " << **itFile << endreq;
    const std::string& filename = (**itFile).name();
    Io::Fd fd = (**itFile).fd();
    msg(MSG::DEBUG) << "FLAGS: " << (**itFile).flags() << endreq;
    
    if(reopenFd(fd,filename))
      return -1;
    
    fdLog.insert(fd);
  }
  
  // Check the FdsRegistry
  AthenaInterprocess::FdsRegistry::const_iterator it(m_fdsRegistry->begin()), itEnd(m_fdsRegistry->end());
  for(;it!=itEnd;++it) {
    AthenaInterprocess::FdsRegistryEntry regEntry = *it;
    if(fdLog.find(regEntry.fd)!=fdLog.end()) {
      msg(MSG::DEBUG) << "The file from FdsRegistry " << regEntry.name << " was registered with FileMgr. Skip reopening" << endreq;
    }
    else {
      msg(MSG::WARNING) << "The file " << regEntry.name << " has not been registered with the FileMgr!" << endreq;
      
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

int AthenaMPToolBase::reopenFd(int fd, const std::string& name)
{
  msg(MSG::DEBUG) << "Attempting to reopen descriptor for " << name << endreq;
  int old_openflags = fcntl(fd,F_GETFL,0);
  msg(MSG::DEBUG) << "The File Access Mode is ";
  switch(old_openflags & O_ACCMODE) {
  case O_RDONLY: {
    msg(MSG::DEBUG) << "RDONLY" << endreq;
    break;
  }
  case O_WRONLY: {
    msg(MSG::DEBUG) << "WRONLY" << endreq;
    break;
  }
  case O_RDWR: {
    msg(MSG::DEBUG) << "RDWR" << endreq;
    break;
  }
  }
   
  int old_descflags = fcntl(fd,F_GETFD,0);
  off_t oldpos = lseek(fd,0,SEEK_CUR);
  if(oldpos==-1) {
    if(errno==ESPIPE) {
      msg(MSG::WARNING) << "Dealing with PIPE. Skipping ... (FIXME!)" << endreq;
    }
    else {
      msg(MSG::ERROR) << "When re-opening file descriptors lseek failed on " << name << ". " << strerror(errno) << endreq;
      return -1;
    }
  }
  else {
    Io::Fd newfd = open(name.c_str(),old_openflags);
    if(newfd==-1) {
      msg(MSG::ERROR) << "When re-opening file descriptors unable to open " << name << " for reading. " << strerror(errno) << endreq;
      return -1;
    }
    if(lseek(newfd,oldpos,SEEK_SET)==-1){
      msg(MSG::ERROR) << "When re-opening file descriptors lseek failed on the newly opened " << name << ". " << strerror(errno) << endreq;
      TEMP_FAILURE_RETRY(close(newfd));
      return -1;
    }
    TEMP_FAILURE_RETRY(close(fd));
    if(dup2(newfd,fd)==-1) {
      msg(MSG::ERROR) << "When re-opening file descriptors unable to duplicate descriptor for " << name << ". " << strerror(errno) << endreq;
      TEMP_FAILURE_RETRY(close(newfd));
      return -1;
    }
    if(fcntl(fd,F_SETFD,old_descflags)==-1) {
      msg(MSG::ERROR) << "When re-opening file descriptors unable to set descriptor flags for " << name << ". " << strerror(errno) << endreq;
      TEMP_FAILURE_RETRY(close(newfd));
      return -1;
    }
    TEMP_FAILURE_RETRY(close(newfd));
  }
  return 0;
}
