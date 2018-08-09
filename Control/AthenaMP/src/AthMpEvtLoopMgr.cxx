/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthMpEvtLoopMgr.h"

#include "AthenaMPTools/IAthenaMPTool.h"
#include "AthenaInterprocess/SharedQueue.h"
#include "AthenaInterprocess/Utilities.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IIoComponent.h"
#include "StoreGate/StoreGateSvc.h"

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
#include <functional>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace athenaMP_MemHelper
{
  int getPss(pid_t, unsigned long&, unsigned long&, unsigned long&, unsigned long&, bool verbose=false);
}

AthMpEvtLoopMgr::AthMpEvtLoopMgr(const std::string& name
				 , ISvcLocator* svcLocator)
  : AthService(name,svcLocator)
  , m_evtProcessor("AthenaEventLoopMgr", name)
  , m_evtSelector(nullptr)
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
}

StatusCode AthMpEvtLoopMgr::initialize()
{
  ATH_MSG_DEBUG("in initialize() ... ");

  SmartIF<IProperty> prpMgr(serviceLocator());
  if(!prpMgr.isValid()) {
    ATH_MSG_ERROR("Failed to get hold of the Property Manager");
    return StatusCode::FAILURE;
  }

  std::string evtSelName = prpMgr->getProperty("EvtSel").toString();
  ATH_CHECK(serviceLocator()->service(evtSelName,m_evtSelector));

  if(m_strategy=="EventService") {
    // ES with non-zero events before forking makes no sense
    if(m_nEventsBeforeFork!=0) {
      ATH_MSG_ERROR("The EventService strategy cannot run with non-zero value for EventsBeforeFork");
      return StatusCode::FAILURE;
    }

    // We need to ignore SkipEvents in ES
    if(updateSkipEvents(0).isFailure()) {
      ATH_MSG_ERROR("Failed to set skipEvents=0 in Event Service");
      return StatusCode::FAILURE;
    }
  }

  if(m_isPileup) {
    m_evtProcessor = ServiceHandle<IEventProcessor>("PileUpEventLoopMgr",name());
    ATH_MSG_INFO("ELM: The job running in pileup mode");
  }
  else {
    ATH_MSG_INFO("ELM: The job running in non-pileup mode");
  }

  ATH_CHECK(m_evtProcessor.retrieve());
  ATH_CHECK(m_tools.retrieve());

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
  ATH_MSG_DEBUG("in executeRun()");

  // Generate random component of the Shared Memory and Shared Queue names
  srand(time(NULL));
  std::ostringstream randStream;
  randStream << getpid() << '_' << AthenaInterprocess::randString();
  ATH_MSG_INFO("Using random components for IPC object names: " << randStream.str());

  ServiceHandle<StoreGateSvc> pDetStore("DetectorStore",name());
  ATH_CHECK(pDetStore.retrieve());

  // Create Shared Event queue if necessary and make it available to the tools
  if(m_strategy=="SharedQueue" 
     || m_strategy=="SharedReader"
     || m_strategy=="RoundRobin") {
    AthenaInterprocess::SharedQueue* evtQueue = new AthenaInterprocess::SharedQueue("AthenaMPEventQueue_"+randStream.str(),2000,sizeof(long));
    if(pDetStore->record(evtQueue,"AthenaMPEventQueue_"+randStream.str()).isFailure()) {
      ATH_MSG_FATAL("Unable to record the pointer to the Shared Event queue into Detector Store");
      delete evtQueue;
      return StatusCode::FAILURE;
    }
  }

  // For the Event Service: create a queue for connecting EvtRangeProcessor in the master with EvtRangeScatterer subprocess
  // The TokenProcessor master will be sending pid-s of failed processes to Token Scatterer
  if(m_strategy=="EventService") {
    AthenaInterprocess::SharedQueue* failedPidQueue = new AthenaInterprocess::SharedQueue("AthenaMPFailedPidQueue_"+randStream.str(),100,sizeof(pid_t));
    if(pDetStore->record(failedPidQueue,"AthenaMPFailedPidQueue_"+randStream.str()).isFailure()) {
      ATH_MSG_FATAL("Unable to record the pointer to the Failed PID queue into Detector Store");
      delete failedPidQueue;
      return StatusCode::FAILURE;
    }
  }

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

	ATH_MSG_WARNING("The top directory " << m_workerTopDir << " already exists");
	ATH_MSG_WARNING("The job will attempt to save it with the name " << backupDir <<  " and create new top directory from scratch");

	if(rename(m_workerTopDir.c_str(),backupDir.c_str())!=0) {
	  ATH_MSG_ERROR("Unable to make backup directory! " << strerror(errno));
	  return StatusCode::FAILURE;
	}

	if(mkdir(m_workerTopDir.c_str(),S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)==0) 
	  break;
      }
    default:
      {
	ATH_MSG_ERROR("Unable to make top directory " << m_workerTopDir << " for children processes! " << strerror(errno));
	return StatusCode::FAILURE;
      }
    }
  }

  // When forking before 1st event, fire BeforeFork incident in non-pileup jobs
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc",name());
  ATH_CHECK(incSvc.retrieve());

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
	ATH_MSG_FATAL("Unable to process first " << nEventsToProcess << " events in the master");
      else
	ATH_MSG_FATAL("Unable to process first event in the master");
      return scEvtProc;
    }
  }

  // Finalize I/O (close input files) by IoComponents
  ServiceHandle<IIoComponentMgr> ioMgr("IoComponentMgr",name());
  ATH_CHECK(ioMgr.retrieve());
  ATH_CHECK(ioMgr->io_finalize());
  ATH_MSG_DEBUG("Successfully finalized I/O before forking");

  // Extract process file descriptors
  boost::shared_ptr<AthenaInterprocess::FdsRegistry> registry = extractFds();

  // Flush stream buffers
  fflush(NULL);

  int maxEvents(maxevt); // This can be modified after restart

  ToolHandleArray<IAthenaMPTool>::iterator it = m_tools.begin(),
    itLast = m_tools.end();

  // Make worker pools
  for(; it!=itLast; ++it) {
    (*it)->useFdsRegistry(registry);
    (*it)->setRandString(randStream.str());
    if(it==m_tools.begin())
      incSvc->fireIncident(Incident(name(),"PreFork")); // Do it only once
    int nChildren = (*it)->makePool(maxEvents,m_nWorkers,m_workerTopDir);
    if(nChildren==-1) {
      ATH_MSG_FATAL("makePool failed for " << (*it)->name());
      return StatusCode::FAILURE;
    }
    else {
      m_nChildProcesses+=nChildren;
    }
  }

  if(m_nChildProcesses==0) {
    ATH_MSG_ERROR("No child processes were created");
    return StatusCode::FAILURE;
  }

  // Assign work to child processes
  for(it=m_tools.begin(); it!=itLast; ++it) {
    if((*it)->exec().isFailure()) {
      ATH_MSG_FATAL("Unable to submit work to the tool " << (*it)->name());
      return StatusCode::FAILURE;
    }
  }

  StatusCode sc = wait();

  if(m_nMemSamplingInterval>0) {
    ATH_MSG_INFO("*** *** Memory Usage *** ***");
    ATH_MSG_INFO("*** MAX PSS  "  << (*std::max_element(m_samplesPss.cbegin(),m_samplesPss.cend()))/1024 << "MB");
    ATH_MSG_INFO("*** MAX RSS  "  << (*std::max_element(m_samplesRss.cbegin(),m_samplesRss.cend()))/1024 << "MB");
    ATH_MSG_INFO("*** MAX SIZE " << (*std::max_element(m_samplesSize.cbegin(),m_samplesSize.cend()))/1024 << "MB");
    ATH_MSG_INFO("*** MAX SWAP " << (*std::max_element(m_samplesSwap.cbegin(),m_samplesSwap.cend()))/1024 << "MB");
    ATH_MSG_INFO("*** *** Memory Usage *** ***");
  }

  if(m_collectSubprocessLogs) {
    ATH_MSG_INFO("BEGIN collecting sub-process logs");
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
    ATH_MSG_INFO("END collecting sub-process logs");
  }

  // Restart the event selector in order to avoid segfault at stop()
  // Removing the next line to fix https://its.cern.ch/jira/browse/ATLASG-935: might need to be revisited
  //ATH_CHECK(m_evtSelector->start());

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
  ATH_MSG_INFO("Waiting for sub-processes");
  ToolHandleArray<IAthenaMPTool>::iterator it = m_tools.begin(),
    itLast = m_tools.end();
  pid_t pid(0);
  bool all_ok(true);

  auto memMonTime = std::chrono::system_clock::now();

  while(m_nChildProcesses>0) {
    for(it = m_tools.begin(); it!=itLast; ++it) {
      if((*it)->wait_once(pid).isFailure()) {
	all_ok = false;
	ATH_MSG_ERROR("Failure in waiting or sub-process finished abnormally");
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
	  ATH_MSG_WARNING("Unable to get memory sample");
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

  if(!all_ok) {
    for(it=m_tools.begin(); it!=itLast; ++it)
      (*it)->killChildren();
  }

  return (all_ok?StatusCode::SUCCESS:StatusCode::FAILURE);
}

StatusCode AthMpEvtLoopMgr::generateOutputReport()
{
  // Loop over tools, collect their output reports and put them all together into a single file.
  // If m_nEventsBeforeFork!=0 then take into account the outputs made by the master process too

  std::ofstream ofs;
  ofs.open(m_outputReportName.c_str());
  if(!ofs) {
    ATH_MSG_ERROR("Unable to open AthenaMPOutputs for writing!");
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
  ATH_MSG_DEBUG("Extracting file descriptors");
  using namespace boost::filesystem;
  boost::shared_ptr<AthenaInterprocess::FdsRegistry> registry(new AthenaInterprocess::FdsRegistry());

  // Extract file descriptors associated with the current process
  // 1. Store only those regular files in the registry, which
  // don't contain substrings from the "exclusion pattern" set
  // 2. Skip also stdout and stderr

  std::vector<std::string> excludePatterns {
    "/root/etc/plugins/"
      ,"/root/cint/cint/"
      ,"/root/include/"
      ,"/var/tmp/"
      ,"/var/lock/"
      };
  
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
	    ATH_MSG_DEBUG(realpath.string() << " Excluded from the registry by the pattern");
	  }
          else {
            registry->push_back(AthenaInterprocess::FdsRegistryEntry(fd,realpath.string()));
	  }
        }
        else {
	  ATH_MSG_DEBUG(realpath.string() << " is not a regular file"); // TODO: deal with these?
	}
      } // File exists
    }
    else
      ATH_MSG_WARNING("UNEXPECTED. " << fdIt->path().string() << " Not a symlink");
  } // Directory iteration

  ATH_MSG_DEBUG("Fds Reistry created. Contents:");
  for(size_t ii(0); ii<registry->size(); ++ii)
    ATH_MSG_DEBUG((*registry)[ii].fd << " " << (*registry)[ii].name);

  return registry;
}

StatusCode AthMpEvtLoopMgr::updateSkipEvents(int skipEvents)
{
  IProperty* propertyServer = dynamic_cast<IProperty*>(m_evtSelector);
  if(!propertyServer) {
    ATH_MSG_ERROR("Unable to dyn-cast the event selector to IProperty");
    return StatusCode::FAILURE;
  }

  IntegerProperty skipEventsProperty("SkipEvents", skipEvents);
  if(propertyServer->setProperty(skipEventsProperty).isFailure()) {
    ATH_MSG_ERROR("Unable to update " << skipEventsProperty.name() << " property on the Event Selector");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Updated the SkipEvents property of the event selector. New value: " << skipEvents);
  return StatusCode::SUCCESS;
}
