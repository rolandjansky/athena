/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthMpEvtLoopMgr.h"

#include "AthenaMPTools/IAthenaMPTool.h"
#include "AthenaInterprocess/SharedQueue.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "StoreGate/StoreGateSvc.h"
#include "PersistentDataModel/Guid.h"

#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctime>
#include <fcntl.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <time.h>
#include <chrono>
#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace athenaMP_MemHelper
{
  int getPss(pid_t, unsigned long&, unsigned long&, unsigned long&, unsigned long&, bool verbose=false);
}

AthMpEvtLoopMgr::AthMpEvtLoopMgr(const std::string& name
				 , ISvcLocator* svcLocator)
  : AthService(name,svcLocator)
  , m_evtProcessor("AthenaEventLoopMgr", name)
  , m_nWorkers(0)
  , m_workerTopDir("athenaMP_workers")
  , m_outputReportName("AthenaMPOutputs")
  , m_strategy("")
  , m_isPileup(false)
  , m_collectSubprocessLogs(false)
  , m_tools(this)
  , m_nChildProcesses(0)
  , m_nPollingInterval(100) // 0.1 second
  , m_nMemSamplingInterval(0) // no sampling by default
  , m_nEventsBeforeFork(0)
  , m_shmemName("")
  , m_masterPid(getpid())
{
  declareProperty("NWorkers",m_nWorkers);
  declareProperty("WorkerTopDir",m_workerTopDir);
  declareProperty("OutputReportFile",m_outputReportName);
  declareProperty("Strategy",m_strategy);
  declareProperty("IsPileup",m_isPileup);
  declareProperty("CollectSubprocessLogs",m_collectSubprocessLogs);
  declareProperty("Tools",m_tools);
  declareProperty("PollingInterval",m_nPollingInterval);
  declareProperty("MemSamplingInterval",m_nMemSamplingInterval);
  declareProperty("EventsBeforeFork",m_nEventsBeforeFork);
}

AthMpEvtLoopMgr::~AthMpEvtLoopMgr()
{
  if(!m_shmemName.empty()
     && m_masterPid==getpid())
    boost::interprocess::shared_memory_object::remove(m_shmemName.c_str());
}

StatusCode AthMpEvtLoopMgr::initialize()
{
  msg(MSG::DEBUG) << "in initialize() ... " << endreq;

  if(m_strategy=="EventServive" && m_nEventsBeforeFork!=0) {
    msg(MSG::ERROR) << "The EventServive strategy cannot run with non-zero value for EventsBeforeFork" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_isPileup) {
    m_evtProcessor = ServiceHandle<IEventProcessor>("PileUpEventLoopMgr",name());
    msg(MSG::INFO) << "ELM: The job running in pileup mode" << endreq;
  }
  else {
    msg(MSG::INFO) << "ELM: The job running in non-pileup mode" << endreq;
  }

  if(m_evtProcessor.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Unable to get the wrapped event loop manager" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_tools.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Unable to initialize tools" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode AthMpEvtLoopMgr::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode AthMpEvtLoopMgr::queryInterface(const InterfaceID& riid,
					   void** ppvInterface)
{
  if(IEventProcessor::interfaceID().versionMatch(riid)) {
    *ppvInterface = (IEventProcessor*)this;
    addRef();
    return StatusCode::SUCCESS;
  } 
  else {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
}

StatusCode AthMpEvtLoopMgr::nextEvent(int maxevt)
{
  // Perhaps there we should return StatusCode::FAILURE as this method shoud not be called directly
  return m_evtProcessor->nextEvent(maxevt);
}

StatusCode AthMpEvtLoopMgr::executeEvent(void* par)
{
  // Perhaps there we should return StatusCode::FAILURE as this method shoud not be called directly
  return m_evtProcessor->executeEvent(par);
}

StatusCode AthMpEvtLoopMgr::executeRun(int maxevt)
{
  msg(MSG::DEBUG) << "in executeRun()" << endreq;

  // Generate random component of the Shared Memory and Shared Queue names
  srand(time(NULL));
  Guid uuid;
  Guid::create(uuid);
  std::ostringstream randStream;
  randStream << getpid() << '_' << uuid.toString();
  ATH_MSG_INFO("Using random components for IPC object names: " << randStream.str());

  StoreGateSvc* pDetStore(0);
  if(service("DetectorStore", pDetStore).isFailure() || pDetStore==0) {
    msg(MSG::FATAL) << "Unable to access Detector Store" << endreq;
    return StatusCode::FAILURE;
  }

  // Create Shared Event queue if necessary and make it available to the tools
  if(m_strategy=="SharedQueue" || m_strategy=="SharedReader") {
    AthenaInterprocess::SharedQueue* evtQueue = new AthenaInterprocess::SharedQueue("AthenaMPEventQueue_"+randStream.str(),2000,sizeof(long));
    if(pDetStore->record(evtQueue,"AthenaMPEventQueue_"+randStream.str()).isFailure()) {
      msg(MSG::FATAL) << "Unable to record the pointer to the Shared Event queue into Detector Store" << endreq;
      delete evtQueue;
      return StatusCode::FAILURE;
    }
  }

  // For the Event Service: create a queue for connecting EvtRangeProcessor in the master with EvtRangeScatterer subprocess
  // The TokenProcessor master will be sending pid-s of failed processes to Token Scatterer
  if(m_strategy=="EventService") {
    AthenaInterprocess::SharedQueue* failedPidQueue = new AthenaInterprocess::SharedQueue("AthenaMPFailedPidQueue_"+randStream.str(),100,sizeof(pid_t));
    if(pDetStore->record(failedPidQueue,"AthenaMPFailedPidQueue_"+randStream.str()).isFailure()) {
      msg(MSG::FATAL) << "Unable to record the pointer to the Failed PID queue into Detector Store" << endreq;
      delete failedPidQueue;
      return StatusCode::FAILURE;
    }
  }

  // Initialize shared memory segment and by this way make sure it never gets re-initialized during the job
  m_shmemName = std::string("/athmp-shmem-"+randStream.str());
  boost::interprocess::shared_memory_object shmemSegment(boost::interprocess::create_only
							 , m_shmemName.c_str()
							 , boost::interprocess::read_write);
  shmemSegment.truncate(2*sizeof(int));
  boost::interprocess::mapped_region shmemRegion(shmemSegment,boost::interprocess::read_write);
  std::memset(shmemRegion.get_address(),0,shmemRegion.get_size());

  // Prepare work directory for sub-processes
  if(mkdir(m_workerTopDir.c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)!=0) {
    switch(errno) {
    case EEXIST:
      {
	// Top directory already exists, maybe a leftover from previous AthenaMP job in the same rundir
	// Rename it with m_workerTopDir+"-bak-rand"

	srand((unsigned)time(0));
	std::ostringstream randname;
	randname << rand();
	std::string backupDir = (m_workerTopDir.rfind("/")==(m_workerTopDir.size()-1)?m_workerTopDir.substr(0,m_workerTopDir.size()-1):m_workerTopDir)+std::string("-bak-")+randname.str(); 

	msg(MSG::WARNING) << "The top directory " << m_workerTopDir << " already exists" << endreq;
	msg(MSG::WARNING) << "The job will attempt to save it with the name " << backupDir <<  " and create new top directory from scratch" << endreq;

	if(rename(m_workerTopDir.c_str(),backupDir.c_str())!=0) {
	  msg(MSG::ERROR) << "Unable to make backup directory! " << strerror(errno) << endreq;
	  return StatusCode::FAILURE;
	}

	if(mkdir(m_workerTopDir.c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)==0) 
	  break;
      }
    default:
      {
	msg(MSG::ERROR) << "Unable to make top directory " << m_workerTopDir << " for children processes! " << strerror(errno) << endreq;
	return StatusCode::FAILURE;
      }
    }
  }

  // When forking before 1st event, fire BeforeFork incident in non-pileup jobs
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
  if(incSvc.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Unable to retrieve IncidentSvc" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_nEventsBeforeFork==0 && !m_isPileup) {
    incSvc->fireIncident(Incident(name(),"BeforeFork"));
  }

  // Try processing requested number of events here
  if(m_nEventsBeforeFork) {
    // Take into account a corner case: m_nEventsBeforeFork > maxevt
    int nEventsToProcess = ((maxevt>-1 && m_nEventsBeforeFork>maxevt)?maxevt:m_nEventsBeforeFork);
    StatusCode scEvtProc = m_evtProcessor->nextEvent(nEventsToProcess);
    if(!scEvtProc.isSuccess()) {
      if(nEventsToProcess)
	msg(MSG::FATAL) << "Unable to process first " << nEventsToProcess << " events in the master" << endreq;
      else
	msg(MSG::FATAL) << "Unable to process first event in the master" << endreq;
      return scEvtProc;
    }
  }

  // Finalize I/O (close input files) by IoComponents
  ServiceHandle<IIoComponentMgr> ioMgr("IoComponentMgr",name());
  if(ioMgr.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Unable to retrieve IoComponentMgr" << endreq;
    return StatusCode::FAILURE;
  }
  if(ioMgr->io_finalize().isFailure()) {
    msg(MSG::FATAL) << "Unable to finalize I/O before forking" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    msg(MSG::DEBUG) << "Successfully finalized I/O before forking" << endreq;
  }

  // Extract process file descriptors
  boost::shared_ptr<AthenaInterprocess::FdsRegistry> registry = extractFds();

  // Flush stream buffers
  fflush(NULL);

  ToolHandleArray<IAthenaMPTool>::iterator it = m_tools.begin(),
    itLast = m_tools.end();

  // Make worker pools
  for(; it!=itLast; ++it) {
    (*it)->useFdsRegistry(registry);
    (*it)->setRandString(randStream.str());
    if(it==m_tools.begin())
      incSvc->fireIncident(Incident(name(),"PreFork")); // Do it only once
    int nChildren = (*it)->makePool(maxevt,m_nWorkers,m_workerTopDir);
    if(nChildren==-1) {
      msg(MSG::FATAL) << "makePool failed for " << (*it)->name() << endreq;
      return StatusCode::FAILURE;
    }
    else {
      m_nChildProcesses+=nChildren;
    }
  }

  if(m_nChildProcesses==0) {
    msg(MSG::ERROR) << "No child processes were created" << endreq;
    return StatusCode::FAILURE;
  }

  // Assign work to child processes
  for(it=m_tools.begin(); it!=itLast; ++it) {
    if((*it)->exec().isFailure()) {
      msg(MSG::FATAL) << "Unable to submit work to the tool " << (*it)->name() <<  endreq;
      return StatusCode::FAILURE;
    }
  }

  StatusCode sc = wait();

  if(m_nMemSamplingInterval>0) {
    msg(MSG::INFO) << "*** *** Memory Usage *** ***" << endreq;
    msg(MSG::INFO) << "*** MAX PSS  "  << (*std::max_element(m_samplesPss.cbegin(),m_samplesPss.cend()))/1024 << "MB" << endreq;
    msg(MSG::INFO) << "*** MAX RSS  "  << (*std::max_element(m_samplesRss.cbegin(),m_samplesRss.cend()))/1024 << "MB" << endreq;
    msg(MSG::INFO) << "*** MAX SIZE " << (*std::max_element(m_samplesSize.cbegin(),m_samplesSize.cend()))/1024 << "MB" << endreq;
    msg(MSG::INFO) << "*** MAX SWAP " << (*std::max_element(m_samplesSwap.cbegin(),m_samplesSwap.cend()))/1024 << "MB" << endreq;
    msg(MSG::INFO) << "*** *** Memory Usage *** ***" << endreq;
  }

  if(m_collectSubprocessLogs) {
    msg(MSG::INFO) << "BEGIN collecting sub-process logs" << endreq;
    std::vector<std::string> logs;
    for(it=m_tools.begin(); it!=itLast; ++it) {
      (*it)->subProcessLogs(logs);
      for(size_t i=0;i<logs.size();++i) {
	std::cout << "\n File: " << logs[i] << "\n" << std::endl;
	std::ifstream log;
	log.open(logs[i].c_str(),std::ifstream::in);
	std::string line;
	while(!log.eof()) {
	  std::getline(log,line);
	  std::cout << line << std::endl;
	}
	log.close();
      }
    }
    msg(MSG::INFO) << "END collecting sub-process logs" << endreq;
  }

  // Restart the event selector in order to avoid segfault at stop()
  SmartIF<IProperty> prpMgr(serviceLocator());
  if(prpMgr.isValid()) {
    std::string evtSelName = prpMgr->getProperty("EvtSel").toString();
    IService* evtSelector(0);
    StatusCode scSvcLoc = serviceLocator()->service(evtSelName,evtSelector);
    if(scSvcLoc.isFailure()||evtSelector==0) {
      msg(MSG::ERROR) << "Failed to retrieve EventSelector" << endreq;
      return StatusCode::FAILURE;
    }
    if(evtSelector->start().isFailure()) {
      msg(MSG::ERROR) << "Failed to restart the Event Selector" << endreq;
      return StatusCode::FAILURE;
    }
  }
  else {
    msg(MSG::ERROR) << "Failed to get hold of the Property Manager" << endreq;
    return StatusCode::FAILURE;
  }

  if(sc.isSuccess())
    return generateOutputReport();
  else 
    return sc;
}

StatusCode AthMpEvtLoopMgr::stopRun()
{
  return m_evtProcessor->stopRun();
}

// !!! NB !!!
//
// Here we rely on fact that if master process finds that one of
// its sub-processes finished abnormally (either signal or non-zero exit code)
// it will stop waiting for other sub-processes and proceed with its finalization.
// Once master process exits the remaining sub-processes will receive SIGHUP and exit too.
//
// We could also change the behavior and broadcast termination signal on all remaining
// sub-processes once a problematic sub-process has been identified
//
StatusCode AthMpEvtLoopMgr::wait()
{
  msg(MSG::INFO) << "Waiting for sub-processes" << endreq;
  ToolHandleArray<IAthenaMPTool>::iterator it = m_tools.begin(),
    itLast = m_tools.end();
  pid_t pid(0);
  bool all_ok(true);

  auto memMonTime = std::chrono::system_clock::now();

  while(m_nChildProcesses>0) {
    for(it = m_tools.begin(); it!=itLast; ++it) {
      if((*it)->wait_once(pid).isFailure()) {
	all_ok = false;
	msg(MSG::ERROR) << "Failure in waiting or sub-process finished abnormally" << endreq;
	break;
      }
      else {
	if(pid>0) m_nChildProcesses -= 1;
      }
    }
    if(!all_ok) break;

    usleep(m_nPollingInterval*1000);

    if(m_nMemSamplingInterval>0) {
      auto currTime = std::chrono::system_clock::now();
      if(std::chrono::duration<double,std::ratio<1,1>>(currTime-memMonTime).count()>m_nMemSamplingInterval) {
	unsigned long size(0);
	unsigned long rss(0);
	unsigned long pss(0);
	unsigned long swap(0);

	if(athenaMP_MemHelper::getPss(getpid(), pss, swap, rss, size, msgLvl(MSG::DEBUG)))
	  msg(MSG::WARNING) << "Unable to get memory sample" << endreq;
	else {
	  m_samplesRss.push_back(rss);
	  m_samplesPss.push_back(pss);
	  m_samplesSize.push_back(size);
	  m_samplesSwap.push_back(swap);
	}
	memMonTime=currTime;
      }
    }
  }

  for(it=m_tools.begin(); it!=itLast; ++it) 
    (*it)->reportSubprocessStatuses();

  return (all_ok?StatusCode::SUCCESS:StatusCode::FAILURE);
}

StatusCode AthMpEvtLoopMgr::generateOutputReport()
{
  // Loop over tools, collect their output reports and put them all together into a single file.
  // If m_nEventsBeforeFork!=0 then take into account the outputs made by the master process too

  std::ofstream ofs;
  ofs.open(m_outputReportName.c_str());
  if(!ofs) {
    msg(MSG::ERROR) << "Unable to open AthenaMPOutputs for writing!" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    std::vector<AthenaMP::AllWorkerOutputs_ptr> allptrs;

    ToolHandleArray<IAthenaMPTool>::iterator it = m_tools.begin(),
      itLast = m_tools.end();
    for(it=m_tools.begin(); it!=itLast; ++it)
      allptrs.push_back((*it)->generateOutputReport());

    // First collect keys=file_names from all tools
    std::set<std::string> allkeys;
    for(size_t i=0; i<allptrs.size(); ++i) {
      AthenaMP::AllWorkerOutputsIterator it_wos = allptrs[i]->begin(),
	it_wosLast = allptrs[i]->end();
      for(;it_wos!=it_wosLast;++it_wos)
	allkeys.insert(it_wos->first);
    }

    // Generate XML
    ofs << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
    ofs << "<athenaFileReport>" << std::endl;
    std::set<std::string>::const_iterator keys_it = allkeys.begin(),
      keys_itLast = allkeys.end();
    for(;keys_it!=keys_itLast;++keys_it) {
      ofs << "  <Files OriginalName=\"" << (*keys_it) << "\">" << std::endl;
      for(size_t i=0; i<allptrs.size(); ++i) {
	AthenaMP::AllWorkerOutputsIterator it_wos = (allptrs[i])->find(*keys_it);
	if(it_wos!=(allptrs[i])->end()) {
	  for(size_t ii=0; ii<it_wos->second.size(); ++ii) {
	    AthenaMP::WorkerOutput& outp = it_wos->second[ii];
	    if(ii==0 && m_nEventsBeforeFork>0) {
	      boost::filesystem::path masterFile(boost::filesystem::current_path());
	      masterFile /= boost::filesystem::path(*keys_it);
	      if(boost::filesystem::exists(masterFile) && boost::filesystem::is_regular_file(masterFile))
		ofs << "    <File "
		    << "description=\"" << outp._description
		    << "\" mode=\"" << outp._access_mode
		    << "\" name=\"" << masterFile.string()
		    << "\" shared=\"" << (outp._shared?"True":"False")
		    << "\" technology=\"" << outp._technology
		    << "\"/>" << std::endl;
	    }
	    ofs << "    <File "
		<< "description=\"" << outp._description
		<< "\" mode=\"" << outp._access_mode
		<< "\" name=\"" << outp._filename
		<< "\" shared=\"" << (outp._shared?"True":"False")
		<< "\" technology=\"" << outp._technology
		<< "\"/>" << std::endl;
	  }
	}
      }
      ofs << "  </Files>" << std::endl;
    }
    ofs << "</athenaFileReport>" << std::endl;
    ofs.close();
  }

  return StatusCode::SUCCESS;
}

boost::shared_ptr<AthenaInterprocess::FdsRegistry> AthMpEvtLoopMgr::extractFds()
{
  msg(MSG::DEBUG) << "Extracting file descriptors" << endreq;
  using namespace boost::filesystem;
  boost::shared_ptr<AthenaInterprocess::FdsRegistry> registry(new AthenaInterprocess::FdsRegistry());

  // Extract file descriptors associated with the current process
  // 1. Store only those regular files in the registry, which
  // don't contain substrings from the "exclusion pattern" set
  // 2. Skip also stdout and stderr

  std::vector<std::string> excludePatterns;
  excludePatterns.reserve(3);
  excludePatterns.push_back("/root/etc/plugins/");
  excludePatterns.push_back("/root/cint/cint/");
  excludePatterns.push_back("/root/include/");
  excludePatterns.push_back("/var/tmp/");

  path fdPath("/proc/self/fd");
  for(directory_iterator fdIt(fdPath); fdIt!=directory_iterator(); fdIt++) {
    if(is_symlink(fdIt->path())) {
      path realpath = read_symlink(fdIt->path());
      int fd = atoi(fdIt->path().filename().string().c_str());

      if (fd==1 || fd==2) // Skip stdout and stderr
        continue;

      if(exists(realpath)) {
        if(is_regular_file(realpath)) {
	  // Check against the exclusion criteria
          bool exclude(false);
          for(size_t i=0;i<excludePatterns.size(); ++i) {
            if(realpath.string().find(excludePatterns[i])!=std::string::npos) {
              exclude = true;
              break;
            }
	  }
          if(exclude) {
	    msg(MSG::DEBUG) << realpath.string() << " Excluded from the registry by the pattern" << endreq;
	  }
          else {
            registry->push_back(AthenaInterprocess::FdsRegistryEntry(fd,realpath.string()));
	  }
        }
        else {
	  msg(MSG::DEBUG) << realpath.string() << " is not a regular file" << endreq; // TODO: deal with these?
	}
      } // File exists
    }
    else
      msg(MSG::WARNING) << "UNEXPECTED. " << fdIt->path().string() << " Not a symlink" << endreq;
  } // Directory iteration

  msg(MSG::DEBUG) << "Fds Reistry created. Contents:" << endreq;
  for(size_t ii(0); ii<registry->size(); ++ii)
    msg(MSG::DEBUG) << (*registry)[ii].fd << " " << (*registry)[ii].name << endreq;

  return registry;
}
