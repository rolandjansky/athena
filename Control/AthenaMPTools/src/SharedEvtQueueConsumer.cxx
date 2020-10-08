/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SharedEvtQueueConsumer.h"
#include "copy_file_icc_hack.h"
#include "AthenaInterprocess/ProcessGroup.h"
#include "AthenaInterprocess/Incidents.h"

#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "AthenaKernel/IEventShare.h"
#include "AthenaKernel/IDataShare.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IFileMgr.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IConversionSvc.h"

#include <boost/filesystem.hpp>

#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdexcept>
#include <cmath> // For pow

SharedEvtQueueConsumer::SharedEvtQueueConsumer(const std::string& type
					       , const std::string& name
					       , const IInterface* parent)
  : AthenaMPToolBase(type,name,parent)
  , m_useSharedReader(false)
  , m_useSharedWriter(false)
  , m_isPileup(false)
  , m_isRoundRobin(false)
  , m_nEventsBeforeFork(0)
  , m_debug(false)
  , m_rankId(-1)
  , m_chronoStatSvc("ChronoStatSvc", name)
  , m_evtSeek(0)
  , m_evtSelSeek(0)
  , m_evtContext(0)
  , m_evtShare(0)
  , m_dataShare(0)
  , m_sharedEventQueue(0)
  , m_sharedRankQueue(0)
  , m_readEventOrders(false)
  , m_eventOrdersFile("athenamp_eventorders.txt")
  , m_masterPid(getpid())
{
  declareInterface<IAthenaMPTool>(this);

  declareProperty("UseSharedReader",m_useSharedReader);
  declareProperty("UseSharedWriter",m_useSharedWriter);
  declareProperty("IsPileup",m_isPileup);
  declareProperty("IsRoundRobin",m_isRoundRobin);
  declareProperty("EventsBeforeFork",m_nEventsBeforeFork);
  declareProperty("Debug", m_debug);
  declareProperty("ReadEventOrders",m_readEventOrders);
  declareProperty("EventOrdersFile",m_eventOrdersFile);

  m_subprocDirPrefix = "worker_";
}

SharedEvtQueueConsumer::~SharedEvtQueueConsumer()
{
}

StatusCode SharedEvtQueueConsumer::initialize()
{
  ATH_MSG_DEBUG("In initialize");
  if(m_isPileup) {
    m_evtProcessor = ServiceHandle<IEventProcessor>("PileUpEventLoopMgr",name());
    ATH_MSG_INFO("The job running in pileup mode");
  }
  else {
    ATH_MSG_INFO("The job running in non-pileup mode");
  }

  StatusCode sc = AthenaMPToolBase::initialize();
  if(!sc.isSuccess())
    return sc;

  // For pile-up jobs use event loop manager for seeking
  // otherwise use event selector
  if(m_isPileup) {
    m_evtSeek = dynamic_cast<IEventSeek*>(m_evtProcessor.operator->());
    if(!m_evtSeek) {
      ATH_MSG_ERROR("Unable to dyn-cast PileUpEventLoopMgr to IEventSeek");
      return StatusCode::FAILURE;
    }
  }
  else {
    sc = serviceLocator()->service(m_evtSelName,m_evtSelSeek);
    if(sc.isFailure() || m_evtSelSeek==0) {
      ATH_MSG_ERROR("Error retrieving IEvtSelectorSeek");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( evtSelector()->createContext (m_evtContext) );
  }

  sc = serviceLocator()->service(m_evtSelName,m_evtShare);
  if(sc.isFailure() || m_evtShare==0) {
    if(m_useSharedReader) {
      ATH_MSG_ERROR("Error retrieving IEventShare");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Could not retrieve IEventShare");
  }

  //FIXME: AthenaPool dependent for now
  IConversionSvc* cnvSvc = 0;
  sc = serviceLocator()->service("AthenaPoolCnvSvc",cnvSvc);
  m_dataShare = dynamic_cast<IDataShare*>(cnvSvc);
  if(sc.isFailure() || m_dataShare==0) {
    if(m_useSharedWriter) {
      ATH_MSG_ERROR("Error retrieving AthenaPoolCnvSvc " << cnvSvc);
      return StatusCode::FAILURE;
    }
  }

  sc = m_chronoStatSvc.retrieve();
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Cannot get ChronoStatSvc.");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode SharedEvtQueueConsumer::finalize()
{
  if(getpid()==m_masterPid) {
    ATH_MSG_INFO("finalize() in the master process");
    // Merge saved event orders into one in the master run directory

    // 1. Check if master run directory already contains a file with saved orders
    // If so, then rename it with random suffix
    boost::filesystem::path ordersFile(m_eventOrdersFile);
    if(boost::filesystem::exists(ordersFile)) {
      srand((unsigned)time(0));
      std::ostringstream randname;
      randname << rand();
      std::string ordersFileBak = m_eventOrdersFile+std::string("-bak-")+randname.str();
      ATH_MSG_WARNING("File " << m_eventOrdersFile << " already exists in the master run directory!");
      ATH_MSG_WARNING("Saving a backup with new name " << ordersFileBak);

      boost::filesystem::path ordersFileBakpath(ordersFileBak);
      boost::filesystem::rename(ordersFile,ordersFileBakpath);
    }

    // 2. Merge workers event orders into the master file
    std::fstream fs(m_eventOrdersFile.c_str(),std::fstream::out);
    for(int i=0; i<m_nprocs; ++i) {
      std::ostringstream workerIndex;
      workerIndex << i;
      boost::filesystem::path worker_rundir(m_subprocTopDir);
      worker_rundir /= boost::filesystem::path(m_subprocDirPrefix+workerIndex.str());
      std::string ordersFileWorker(worker_rundir.string()+std::string("/")+m_eventOrdersFile);
      ATH_MSG_INFO("Processing " << ordersFileWorker << " ...");
      std::fstream fs_worker(ordersFileWorker.c_str(),std::fstream::in);
      std::string line;
      while(fs_worker.good()) {
	std::getline(fs_worker,line);
	fs << line << std::endl;
      }
      fs_worker.close();
    }
    fs.close();
  } // if(getpid()==m_masterPid)

  if (m_evtContext) {
    ATH_CHECK( evtSelector()->releaseContext (m_evtContext) );
    m_evtContext = nullptr;
  }

  delete m_sharedRankQueue;
  return StatusCode::SUCCESS;
}

int SharedEvtQueueConsumer::makePool(int, int nprocs, const std::string& topdir)
{
  ATH_MSG_DEBUG("In makePool " << getpid());

  if(nprocs==0 || nprocs<-1) {
    ATH_MSG_ERROR("Invalid value for the nprocs parameter: " << nprocs);
    return -1;
  }

  if(topdir.empty()) {
    ATH_MSG_ERROR("Empty name for the top directory!");
    return -1;
  }

  m_nprocs = (nprocs==-1?sysconf(_SC_NPROCESSORS_ONLN):nprocs);
  m_subprocTopDir = topdir;

  // Get the shared event queue
  ATH_MSG_DEBUG("Event queue name AthenaMPEventQueue_" << m_randStr);
  StatusCode sc = detStore()->retrieve(m_sharedEventQueue,"AthenaMPEventQueue_"+m_randStr);
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve the pointer to Shared Event Queue");
    return -1;
  }


  // Create rank queue and fill it
  m_sharedRankQueue = new AthenaInterprocess::SharedQueue("SharedEvtQueueConsumer_RankQueue_"+m_randStr,m_nprocs,sizeof(int));
  for(int i=0; i<m_nprocs; ++i)
    if(!m_sharedRankQueue->send_basic<int>(i)) {
      ATH_MSG_ERROR("Unable to send int to the ranks queue!");
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

StatusCode SharedEvtQueueConsumer::exec()
{
  ATH_MSG_DEBUG("In exec " << getpid());

  if(mapAsyncFlag(AthenaMPToolBase::FUNC_EXEC))
    return StatusCode::FAILURE;
  ATH_MSG_INFO("Workers started processing events");

  return StatusCode::SUCCESS;
}

StatusCode SharedEvtQueueConsumer::wait_once(pid_t& pid)
{
  StatusCode sc = AthenaMPToolBase::wait_once(pid);
  AthenaInterprocess::ProcessResult* presult(0);
  if(sc.isFailure()) {
    // We are to stop waiting. Pull all available ProcessResults from the queue
    // Don't serialize finalizations
    do {
      presult = m_processGroup->pullOneResult();
      if(presult && (unsigned)(presult->output.size)>sizeof(int)) 
	decodeProcessResult(presult,false);
      if(presult) free(presult->output.data);
      delete presult;
    } while(presult);
  }
  else {
    // Pull one result and decode it if necessary
    presult = m_processGroup->pullOneResult();
    int res(0);
    if(presult && (unsigned)(presult->output.size)>sizeof(int)) 
      res = decodeProcessResult(presult,true);
    if(presult) free(presult->output.data);
    delete presult;
    if(res) return StatusCode::FAILURE;
  }
  return sc;
}

void SharedEvtQueueConsumer::reportSubprocessStatuses()
{
  ATH_MSG_INFO("Statuses of event processors");
  const std::vector<AthenaInterprocess::ProcessStatus>& statuses = m_processGroup->getStatuses();
  for(size_t i=0; i<statuses.size(); ++i) {
    // Get the number of events processed by this worker
    auto it = m_eventStat.find(statuses[i].pid);
    msg(MSG::INFO) << "*** Process PID=" << statuses[i].pid 
		   << ". Status " << ((statuses[i].exitcode)?"FAILURE":"SUCCESS") 
		   << ". Number of events processed: ";
    if(it==m_eventStat.end())
      msg(MSG::INFO) << "N/A" << endmsg;
    else
      msg(MSG::INFO) << it->second.first
		     << ", Event Loop Time: " << it->second.second << "sec."
		     << endmsg;
  }
}

void SharedEvtQueueConsumer::subProcessLogs(std::vector<std::string>& filenames)
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

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedEvtQueueConsumer::bootstrap_func()
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
    ATH_MSG_ERROR("Unable to retrieve IncidentSvc");
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

  // __________ Redirect logs unless we want to attach debugger ____________
  if(!m_debug) {
    if(redirectLog(worker_rundir.string()))
      return outwork;

    ATH_MSG_INFO("Logs redirected in the AthenaMP event worker PID=" << getpid());
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

  
  // ________________________ Make Shared Reader/Writer Client ________________________
  if(m_useSharedReader && m_evtShare) {
    if(!m_evtShare->makeClient(m_rankId).isSuccess()) {
      ATH_MSG_ERROR("Failed to make the event selector a share client");
      return outwork;
    } else {
      ATH_MSG_DEBUG("Successfully made the event selector a share client");
    }
  }

  if(m_useSharedWriter && m_dataShare) {
    IProperty* propertyServer = dynamic_cast<IProperty*>(m_dataShare);
    if (propertyServer==0 || propertyServer->setProperty("MakeStreamingToolClient", m_rankId + 1).isFailure()) {
      ATH_MSG_ERROR("Could not change AthenaPoolCnvSvc MakeClient Property");
      return outwork;
    } else {
      ATH_MSG_DEBUG("Successfully made the conversion service a share client");
    }
  }

  // ________________________ I/O reinit ________________________
  if(!m_ioMgr->io_reinitialize().isSuccess()) {
    ATH_MSG_ERROR("Failed to reinitialize I/O");
    return outwork;
  } else {
    ATH_MSG_DEBUG("Successfully reinitialized I/O");
  }

  // ________________________ Event selector restart ________________________
  IService* evtSelSvc = dynamic_cast<IService*>(evtSelector());
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
      if(evtsel && (evtsel != evtSelector())) {
	if((*itSvc)->start().isSuccess())
	  ATH_MSG_DEBUG("Restarted event selector " << (*itSvc)->name());
	else {
	  ATH_MSG_ERROR("Failed to restart event selector " << (*itSvc)->name());
	  return outwork;
	}
      }
    }
  }

  // _______________________ Event orders for debugging ________________________________
  if(m_readEventOrders) {
    std::fstream fs(m_eventOrdersFile.c_str(),std::fstream::in);
    if(fs.good()) {
      ATH_MSG_INFO("Reading predefined event orders from " << m_eventOrdersFile);
      while(fs.good()){
	std::string line;
	std::getline(fs,line);
	if(line.empty())continue;

	// Parse the string
	size_t idx(0);
	int rank = std::stoi(line,&idx);
	if(rank==m_rankId) {
	  msg(MSG::INFO) << "This worker will proces the following events #";
	  while(idx<line.size()-1) {
	    line = line.substr(idx+1);
	    int evtnum = std::stoi(line,&idx);
	    m_eventOrders.push_back(evtnum);
	    msg(MSG::INFO) << " " << evtnum;
	  }
	  msg(MSG::INFO) << endmsg;
	}
      }
      if(m_eventOrders.empty()) {
	ATH_MSG_ERROR("Could not read event orders for the rank " << m_rankId);
	return outwork;
      }
      fs.close();
    }
    else {
      ATH_MSG_ERROR("Unable to read predefined event orders from " << m_eventOrdersFile);
      return outwork;
    }
  }

  // ________________________ Worker dir: chdir ________________________
  if(chdir(worker_rundir.string().c_str())==-1) {
    ATH_MSG_ERROR("Failed to chdir to " << worker_rundir.string());
    return outwork;
  }

  // ___________________ Fire UpdateAfterFork incident _________________
  p_incidentSvc->fireIncident(AthenaInterprocess::UpdateAfterFork(m_rankId,getpid(),name()));

  // Declare success and return
  *(int*)(outwork->data) = 0;
  return outwork;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedEvtQueueConsumer::exec_func()
{
  ATH_MSG_INFO("Exec function in the AthenaMP worker PID=" << getpid());

  bool all_ok(true);

  // Get the value of SkipEvent
  int skipEvents(0);
  IProperty* propertyServer = dynamic_cast<IProperty*>(evtSelector());
  if(propertyServer==0) {
    ATH_MSG_ERROR("Unable to cast event selector to IProperty");
    all_ok = false;
  }
  else {
    std::string propertyName("SkipEvents");
    IntegerProperty skipEventsProp(propertyName,skipEvents);
    if(propertyServer->getProperty(&skipEventsProp).isFailure()) {
      ATH_MSG_INFO("Event Selector does not have SkipEvents property");
    }
    else {
      skipEvents = skipEventsProp.value();
    }
  }


  // ________________________ This is needed only for PileUp jobs __________________________________
  // **
  // If either EventsBeforeFork or SkipEvents is nonzero, first we need to advance the event selector
  // by EventsBeforeFork+SkipEvents and only after that start seeking on the PileUpEventLoopMgr
  // **
  if(m_isPileup && all_ok) {
    if (!m_evtSelSeek) {
      StatusCode sc = serviceLocator()->service(m_evtSelName,m_evtSelSeek);
      if(sc.isFailure() || m_evtSelSeek==0) {
        ATH_MSG_ERROR("Error retrieving Event Selector with IEvtSelectorSeek interface for PileUp job");
        all_ok = false;
      }
      if (evtSelector()->createContext (m_evtContext).isFailure()) {
        ATH_MSG_ERROR("Error creating IEventSelector context.");
        all_ok = false;
      }
    }
    if (all_ok) {
      if((m_nEventsBeforeFork+skipEvents)
	 && m_evtSelSeek->seek(*m_evtContext, m_nEventsBeforeFork+skipEvents).isFailure()) {
	ATH_MSG_ERROR("Unable to seek to " << m_nEventsBeforeFork+skipEvents);
	all_ok = false;
      }
    }
  }
  // ________________________ This is needed only for PileUp jobs __________________________________


  long intmask = pow(0x100,sizeof(int))-1; // Mask for decoding event number from the value posted to the queue
  int nEvt(m_nEventsBeforeFork);
  int nEventsProcessed(0);
  long evtnumAndChunk(0);

  unsigned evtCounter(0);
  int evtnum(0), chunkSize(1);
  auto predefinedEvt = m_eventOrders.cbegin();

  // If the event orders file already exists in worker's run directory, then it's an unexpected error!
  boost::filesystem::path ordersFile(m_eventOrdersFile);
  if(boost::filesystem::exists(ordersFile)) {
    ATH_MSG_ERROR(m_eventOrdersFile << " already exists in the worker's run directory!");
    all_ok = false;
  }
  
  // For the round robin we need to know the maximum number of events for this job
  if(m_isRoundRobin) {
    evtnumAndChunk = 1;
    while(evtnumAndChunk>0) {
      if(!m_sharedEventQueue->try_receive_basic<long>(evtnumAndChunk)) {
	usleep(1000);
      }
    }
    evtnumAndChunk *= -1;
  }

  System::ProcessTime time_start = System::getProcessTime();
  if(all_ok) {
    std::fstream fs(m_eventOrdersFile.c_str(),std::fstream::out);
    fs << m_rankId;
    bool firstOrder(true);
    while(true) {
      if(m_isRoundRobin) {
	evtnum = skipEvents + m_nprocs*evtCounter + m_rankId; 
	if(evtnum>=evtnumAndChunk+skipEvents) { 
	  break;
	}
	evtCounter++;
      }
      else {
	if(m_readEventOrders) {
	  if(predefinedEvt==m_eventOrders.cend()) break;
	  evtnum = *predefinedEvt;
	  predefinedEvt++;
	  fs << (firstOrder?":":",") << evtnum;
	  fs.flush();
	  firstOrder=false;
	  ATH_MSG_INFO("Read event number from the orders file: " << evtnum);
	}
	else {
	  if(!m_sharedEventQueue->try_receive_basic<long>(evtnumAndChunk)) {
	    // The event queue is empty, but we should check whether there are more events to come or not
	    ATH_MSG_DEBUG("Event queue is empty"); 
	    usleep(1000);
	    continue;
	  }
	  if(evtnumAndChunk<=0) {
	    evtnumAndChunk *= -1;
	    ATH_MSG_DEBUG("No more events are expected. The total number of events for this job = " << evtnumAndChunk);
	    break;
	  }
	  ATH_MSG_DEBUG("Received value from the queue 0x" << std::hex << evtnumAndChunk << std::dec);
	  chunkSize = evtnumAndChunk >> (sizeof(int)*8);
	  evtnum = evtnumAndChunk & intmask;
	  ATH_MSG_INFO("Received from the queue: event num=" << evtnum << " chunk size=" << chunkSize);

	  // Save event order
	  for(int i(0);i<chunkSize;++i) {
	    fs << (firstOrder?":":",") << evtnum+i;
	    firstOrder=false;
	  }
	  fs.flush();
	} // Get event numbers from the shared queue
      } // Not RoundRobin
      nEvt+=chunkSize;
      StatusCode sc;
      if(m_useSharedReader) {
	sc = m_evtShare->share(evtnum);
	if(sc.isFailure()){
	  ATH_MSG_ERROR("Unable to share " << evtnum);
	  all_ok=false;
	  break;
	}
	else {
	  ATH_MSG_INFO("Share of " << evtnum << " succeeded");
	}
      }
      else {
	m_chronoStatSvc->chronoStart("AthenaMP_seek");
        if (m_evtSeek) {
          sc=m_evtSeek->seek(evtnum);
        }
        else {
          sc=m_evtSelSeek->seek(*m_evtContext, evtnum);
        }
	if(sc.isFailure()){
	  ATH_MSG_ERROR("Unable to seek to " << evtnum);
	  all_ok=false;
	  break;
	}
	else {
	  ATH_MSG_INFO("Seek to " << evtnum << " succeeded");
	}
	m_chronoStatSvc->chronoStop("AthenaMP_seek");
      }
      m_chronoStatSvc->chronoStart("AthenaMP_nextEvent");
      sc = m_evtProcessor->nextEvent(nEvt);
      nEventsProcessed += chunkSize;
      if(sc.isFailure()){
	if(chunkSize==1) {
	  ATH_MSG_ERROR("Unable to process event " << evtnum);
	}
	else {
	  ATH_MSG_ERROR("Unable to process the chunk (" << evtnum << "," << evtnum+chunkSize-1 << ")");
	}
	all_ok=false;
	break;
      }
      m_chronoStatSvc->chronoStop("AthenaMP_nextEvent"); 
    }
    fs.close();
  }
  System::ProcessTime time_delta = System::getProcessTime() - time_start;
  TimeValType elapsedTime = time_delta.elapsedTime<System::Sec>();

  if(all_ok) {
    if(m_evtProcessor->executeRun(0).isFailure()) {
      ATH_MSG_ERROR("Could not finalize the Run");
      all_ok=false;
    }
    else if(!m_useSharedReader) {
      StatusCode sc;
      if (m_evtSeek) {
        sc = m_evtSeek->seek(evtnumAndChunk+skipEvents);
      }
      else {
        sc = m_evtSelSeek->seek(*m_evtContext, evtnumAndChunk+skipEvents);
      }
      if(sc.isFailure()) {
	ATH_MSG_WARNING("Seek past maxevt to " << evtnumAndChunk+skipEvents << " returned failure.");
      }
    }
  }

  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);

  // Return value: "ERRCODE|Func_Flag|NEvt|EvtLoopTime"
  int outsize = 2*sizeof(int)+sizeof(AthenaMPToolBase::Func_Flag)+sizeof(elapsedTime);
  void* outdata = malloc(outsize);
  *(int*)(outdata) = (all_ok?0:1); // Error code: for now use 0 success, 1 failure
  AthenaMPToolBase::Func_Flag func = AthenaMPToolBase::FUNC_EXEC;
  memcpy((char*)outdata+sizeof(int),&func,sizeof(func));
  memcpy((char*)outdata+sizeof(int)+sizeof(func),&nEventsProcessed,sizeof(int));
  memcpy((char*)outdata+2*sizeof(int)+sizeof(func),&elapsedTime,sizeof(elapsedTime));
  outwork->data = outdata;
  outwork->size = outsize;
  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...
  return outwork;
}

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedEvtQueueConsumer::fin_func()
{
  ATH_MSG_INFO("Fin function in the AthenaMP worker PID=" << getpid());

  bool all_ok(true);

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

  // Return value: "ERRCODE|Func_Flag|NEvt|EvtLoopTime"  (Here NEvt=-1 and EvtLoopTime=-1)
  int outsize = 2*sizeof(int)+sizeof(AthenaMPToolBase::Func_Flag)+sizeof(TimeValType);
  void* outdata = malloc(outsize);
  *(int*)(outdata) = (all_ok?0:1); // Error code: for now use 0 success, 1 failure
  AthenaMPToolBase::Func_Flag func = AthenaMPToolBase::FUNC_FIN;
  memcpy((char*)outdata+sizeof(int),&func,sizeof(func));
  int nEvt = -1;
  memcpy((char*)outdata+sizeof(int)+sizeof(func),&nEvt,sizeof(int));
  TimeValType elapsed = -1;
  memcpy((char*)outdata+2*sizeof(int)+sizeof(func),&elapsed,sizeof(elapsed));

  outwork->data = outdata;
  outwork->size = outsize;

  return outwork;
}

int SharedEvtQueueConsumer::decodeProcessResult(const AthenaInterprocess::ProcessResult* presult, bool doFinalize)
{
  if(!presult) return 0;
  const AthenaInterprocess::ScheduledWork& output = presult->output;
  ATH_MSG_DEBUG("Decoding the output of PID=" << presult->pid << " with the size=" << output.size);
  if(output.size!=2*sizeof(int)+sizeof(AthenaMPToolBase::Func_Flag)+sizeof(TimeValType)) return 0;
  
  AthenaMPToolBase::Func_Flag func;
  memcpy(&func,(char*)output.data+sizeof(int),sizeof(func));
  if(func==AthenaMPToolBase::FUNC_EXEC) {
    // Store the number of processed events
    int nevt(0);
    TimeValType elapsed(0);
    memcpy(&nevt,(char*)output.data+sizeof(int)+sizeof(func),sizeof(int));
    memcpy(&elapsed,(char*)output.data+2*+sizeof(int)+sizeof(func),sizeof(TimeValType));
    m_eventStat[presult->pid]=std::pair<int,TimeValType>(nevt,elapsed);
    ATH_MSG_DEBUG("PID=" << presult->pid << " processed " << nevt << " events in " << elapsed << "sec.");

    if(doFinalize) {
      // Add PID to the finalization queue
      m_finQueue.push(presult->pid);
      ATH_MSG_DEBUG("Added PID=" << presult->pid << " to the finalization queue");

      // If this is the only element in the queue then start its finalization
      // Otherwise it has to wait its turn until all previous processes have been finalized
      if(m_finQueue.size()==1) {
	if(mapAsyncFlag(AthenaMPToolBase::FUNC_FIN,presult->pid)
	   || m_processGroup->map_async(0,0,presult->pid)) {
	  ATH_MSG_ERROR("Problem scheduling finalization on PID=" << presult->pid);
	  return 1;
	}
	else {
	  ATH_MSG_DEBUG("Scheduled finalization of PID=" << presult->pid);
	}
      }
    }
  }
  else if(doFinalize && func==AthenaMPToolBase::FUNC_FIN) {
    ATH_MSG_DEBUG("Finished finalization of PID=" << presult->pid);
    pid_t pid = m_finQueue.front();
    if(pid==presult->pid) {
      // pid received as expected. Remove it from the queue
      m_finQueue.pop();
      ATH_MSG_DEBUG("PID=" << presult->pid << " removed from the queue");
      // Schedule finalization of the next processe in the queue
      if(m_finQueue.size()) {
	if(mapAsyncFlag(AthenaMPToolBase::FUNC_FIN,m_finQueue.front())
           || m_processGroup->map_async(0,0,m_finQueue.front())) {
	  ATH_MSG_ERROR("Problem scheduling finalization on PID=" << m_finQueue.front());
          return 1;
	}
	else  {
	  ATH_MSG_DEBUG("Scheduled finalization of PID=" << m_finQueue.front());
	}
      }
    }
    else {
      // Error: unexpected pid received from presult
      ATH_MSG_ERROR("Finalized PID=" << presult->pid << " while PID=" << pid << " was expected");
      return 1;
    }
  }

  return 0;
}
