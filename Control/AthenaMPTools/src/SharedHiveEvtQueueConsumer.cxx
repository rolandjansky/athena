/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SharedHiveEvtQueueConsumer.h"
#include "copy_file_icc_hack.h"
#include "AthenaInterprocess/ProcessGroup.h"
#include "AthenaInterprocess/Incidents.h"

#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "AthenaKernel/IEventShare.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IFileMgr.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"

#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdexcept>
#include <cmath> // For pow

#include "unistd.h"
#include <signal.h>

namespace SharedHiveEvtQueueConsumer_d {
  bool sig_done = false;
  void pauseForDebug(int /*sig*/) {
    // std::cout << "Continuing after receiving signal " 
    // 	    << sig << std::endl;
    sig_done = true;
  }
}

SharedHiveEvtQueueConsumer::SharedHiveEvtQueueConsumer(const std::string& type
					       , const std::string& name
					       , const IInterface* parent)
  : AthenaMPToolBase(type,name,parent)
  , m_useSharedReader(false)
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
  , m_sharedEventQueue(0)
  , m_sharedRankQueue(0)
{
  declareInterface<IAthenaMPTool>(this);

  declareProperty("UseSharedReader",m_useSharedReader);
  declareProperty("IsPileup",m_isPileup);
  declareProperty("IsRoundRobin",m_isRoundRobin);
  declareProperty("EventsBeforeFork",m_nEventsBeforeFork);
  declareProperty("Debug", m_debug);

  m_subprocDirPrefix = "worker_";
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

SharedHiveEvtQueueConsumer::~SharedHiveEvtQueueConsumer()
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode SharedHiveEvtQueueConsumer::initialize()
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

  if(m_useSharedReader) {
    sc = serviceLocator()->service(m_evtSelName,m_evtShare);
    if(sc.isFailure() || m_evtShare==0) {
      ATH_MSG_ERROR("Error retrieving IEventShare");
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
SharedHiveEvtQueueConsumer::finalize()
{
  if (m_evtContext) {
    ATH_CHECK( evtSelector()->releaseContext (m_evtContext) );
    m_evtContext = nullptr;
  }

  delete m_sharedRankQueue;
  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int 
SharedHiveEvtQueueConsumer::makePool(int, int nprocs, const std::string& topdir)
{
  ATH_MSG_DEBUG("In makePool " << getpid());

  if(nprocs==0 || nprocs<-1) {
    ATH_MSG_ERROR("Invalid value for the nprocs parameter: ");
    return -1;
  }

  if(topdir.empty()) {
    ATH_MSG_ERROR("Empty name for the top directory!");
    return -1;
  }

  m_nprocs = (nprocs==-1?sysconf(_SC_NPROCESSORS_ONLN):nprocs);
  m_subprocTopDir = topdir;

  // Get the shared event queue
  ATH_MSG_DEBUG("Event queue name " << "AthenaMPEventQueue_" << m_randStr);
  StatusCode sc = detStore()->retrieve(m_sharedEventQueue,"AthenaMPEventQueue_"+m_randStr);
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve the pointer to Shared Event Queue");
    return -1;
  }


  // Create rank queue and fill it
  m_sharedRankQueue = new AthenaInterprocess::SharedQueue("SharedHiveEvtQueueConsumer_RankQueue_"+m_randStr,m_nprocs,sizeof(int));
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
SharedHiveEvtQueueConsumer::exec()
{
  ATH_MSG_DEBUG("In exec " << getpid());

  if(mapAsyncFlag(AthenaMPToolBase::FUNC_EXEC))
    return StatusCode::FAILURE;
  ATH_MSG_INFO("Workers started processing events");

  return StatusCode::SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode 
SharedHiveEvtQueueConsumer::wait_once(pid_t& pid)
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
  } else {
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
SharedHiveEvtQueueConsumer::reportSubprocessStatuses()
{
  ATH_MSG_INFO("Statuses of event processors");
  const std::vector<AthenaInterprocess::ProcessStatus>& statuses = m_processGroup->getStatuses();
  for(size_t i=0; i<statuses.size(); ++i) {
    // Get the number of events processed by this worker
    std::map<pid_t,int>::const_iterator it = m_nProcessedEvents.find(statuses[i].pid);
    msg(MSG::INFO) << "*** Process PID=" << statuses[i].pid 
		   << ". Status " << ((statuses[i].exitcode)?"FAILURE":"SUCCESS") 
		   << ". Number of events processed: ";
    if(it==m_nProcessedEvents.end())
      msg(MSG::INFO) << "N/A" << endmsg;
    else
      msg(MSG::INFO) << it->second << endmsg;
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void 
SharedHiveEvtQueueConsumer::subProcessLogs(std::vector<std::string>& filenames)
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::unique_ptr<AthenaInterprocess::ScheduledWork>
SharedHiveEvtQueueConsumer::bootstrap_func()
{

  if (m_debug) {
    ATH_MSG_INFO("Bootstrap worker PID " << getpid() << " - waiting for SIGUSR1");
    sigset_t mask, oldmask;
    
    signal(SIGUSR1, SharedHiveEvtQueueConsumer_d::pauseForDebug);
    
    sigemptyset (&mask);
    sigaddset (&mask, SIGUSR1);
    
    sigprocmask (SIG_BLOCK, &mask, &oldmask);
    while (!SharedHiveEvtQueueConsumer_d::sig_done)
      sigsuspend (&oldmask);
    sigprocmask (SIG_UNBLOCK, &mask, NULL);
  }

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

  // ________________________ Redirect logs ________________________
  if(redirectLog(worker_rundir.string()))
    return outwork;

  ATH_MSG_INFO("Logs redirected in the AthenaMP event worker PID=" << getpid());

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

  
  // ________________________ Make Shared RAW Reader Client ________________________
  if(m_useSharedReader) {
    if(!m_evtShare->makeClient(m_rankId).isSuccess()) {
      ATH_MSG_ERROR("Failed to make the event selector a share client");
      return outwork;
    }
    else {
      ATH_MSG_DEBUG("Successfully made the event selector a share client");
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

  // ___________________ Fire UpdateAfterFork incident _________________
  p_incidentSvc->fireIncident(AthenaInterprocess::UpdateAfterFork(m_rankId,getpid(),name()));

  p_incidentSvc->fireIncident(Incident(name(),"ReloadProxies"));

  // Declare success and return
  *(int*)(outwork->data) = 0;
  return outwork;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::unique_ptr<AthenaInterprocess::ScheduledWork> 
SharedHiveEvtQueueConsumer::exec_func()
{
  ATH_MSG_INFO("Exec function in the AthenaMP worker PID=" << getpid());

  bool all_ok(true);

  if (! initHive().isSuccess()) {
    ATH_MSG_FATAL("unable to initialize Hive");
    all_ok = false;
  }

  // Get the value of SkipEvent
  int skipEvents(0);
  IProperty* propertyServer = dynamic_cast<IProperty*>(m_evtSelector);
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
  
  if(all_ok) {
    while(true) {
      if(m_isRoundRobin) {
	evtnum = skipEvents + m_nprocs*evtCounter + m_rankId; 
	if(evtnum>=evtnumAndChunk+skipEvents) {
          break;
        }
	evtCounter++;
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
	while (m_schedulerSvc->freeSlots() < 1) {
	  ATH_MSG_DEBUG("waiting for a free scheduler slot");
	  usleep(1000000);
	}

	ATH_MSG_DEBUG("Received value from the queue 0x" << std::hex << evtnumAndChunk << std::dec);
	chunkSize = evtnumAndChunk >> (sizeof(int)*8);
	evtnum = evtnumAndChunk & intmask;
	ATH_MSG_INFO("Received from the queue: event num=" << evtnum << " chunk size=" << chunkSize);
      }
      nEvt+=chunkSize;
      StatusCode sc;
      if(m_useSharedReader) {
	sc = m_evtShare->share(evtnum);
	if(sc.isFailure()){
	  ATH_MSG_ERROR("Unable to share " << evtnum);
	  all_ok=false;
	  break;
	} else {
	  ATH_MSG_INFO("Share of " << evtnum << " succeeded");
	}
      } else {
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
	} else {
	  ATH_MSG_INFO("Seek to " << evtnum << " succeeded");
	}
	m_chronoStatSvc->chronoStop("AthenaMP_seek");
      }
      m_chronoStatSvc->chronoStart("AthenaMP_nextEvent");
      sc = m_evtProcessor->nextEvent(chunkSize);
      nEventsProcessed += chunkSize;
      if(sc.isFailure()){
	if(chunkSize==1)
	  ATH_MSG_ERROR("Unable to process event " << evtnum);
	else
	  ATH_MSG_ERROR("Unable to process the chunk (" << evtnum << "," << evtnum+chunkSize-1 << ")");
	all_ok=false;
	break;
      }
      m_chronoStatSvc->chronoStop("AthenaMP_nextEvent"); 
    }
  }

  if(all_ok) {
    if(m_evtProcessor->executeRun(0).isFailure()) {
      ATH_MSG_ERROR("Could not finalize the Run");
      all_ok=false;
    } else {
      StatusCode sc;
      if (m_evtSeek) {
        sc = m_evtSeek->seek(evtnumAndChunk+skipEvents);
      }
      else {
        sc = m_evtSelSeek->seek(*m_evtContext, evtnumAndChunk+skipEvents);
      }
      if(sc.isFailure()) { 
	ATH_MSG_DEBUG("Seek past maxevt to " << evtnumAndChunk+skipEvents << " returned failure. As expected...");
      }
    }
  }

  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);

  // Return value: "ERRCODE|Func_Flag|NEvt"
  int outsize = 2*sizeof(int)+sizeof(AthenaMPToolBase::Func_Flag);
  void* outdata = malloc(outsize);
  *(int*)(outdata) = (all_ok?0:1); // Error code: for now use 0 success, 1 failure
  AthenaMPToolBase::Func_Flag func = AthenaMPToolBase::FUNC_EXEC;
  memcpy((char*)outdata+sizeof(int),&func,sizeof(func));
  memcpy((char*)outdata+sizeof(int)+sizeof(func),&nEventsProcessed,sizeof(int));

  outwork->data = outdata;
  outwork->size = outsize;
  // ...
  // (possible) TODO: extend outwork with some error message, which will be eventually
  // reported in the master proces
  // ...
  return outwork;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::unique_ptr<AthenaInterprocess::ScheduledWork> 
SharedHiveEvtQueueConsumer::fin_func()
{
  ATH_MSG_INFO("Fin function in the AthenaMP worker PID=" << getpid());

  bool all_ok(true);

  if(m_appMgr->stop().isFailure()) {
    ATH_MSG_ERROR("Unable to stop AppMgr"); 
    all_ok=false;
  } else { 
    if(m_appMgr->finalize().isFailure()) {
      ATH_MSG_ERROR("Unable to finalize AppMgr");
      all_ok=false;
    }
  }

  std::unique_ptr<AthenaInterprocess::ScheduledWork> outwork(new AthenaInterprocess::ScheduledWork);

  // Return value: "ERRCODE|Func_Flag|NEvt"  (Here NEvt=-1)
  int outsize = 2*sizeof(int)+sizeof(AthenaMPToolBase::Func_Flag);
  void* outdata = malloc(outsize);
  *(int*)(outdata) = (all_ok?0:1); // Error code: for now use 0 success, 1 failure
  AthenaMPToolBase::Func_Flag func = AthenaMPToolBase::FUNC_FIN;
  memcpy((char*)outdata+sizeof(int),&func,sizeof(func));
  int nEvt = -1;
  memcpy((char*)outdata+sizeof(int)+sizeof(func),&nEvt,sizeof(int));

  outwork->data = outdata;
  outwork->size = outsize;

  return outwork;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int 
SharedHiveEvtQueueConsumer::decodeProcessResult(const AthenaInterprocess::ProcessResult* presult, bool doFinalize)
{
  if(!presult) return 0;
  const AthenaInterprocess::ScheduledWork& output = presult->output;
  ATH_MSG_DEBUG("Decoding the output of PID=" << presult->pid << " with the size=" << output.size);
  if(output.size!=2*sizeof(int)+sizeof(AthenaMPToolBase::Func_Flag)) return 0;
  
  AthenaMPToolBase::Func_Flag func;
  memcpy(&func,(char*)output.data+sizeof(int),sizeof(func));
  if(func==AthenaMPToolBase::FUNC_EXEC) {
    // Store the number of processed events
    int nevt(0);
    memcpy(&nevt,(char*)output.data+sizeof(int)+sizeof(func),sizeof(int));
    m_nProcessedEvents[presult->pid]=nevt;
    ATH_MSG_DEBUG("PID=" << presult->pid << " processed " << nevt << " events");

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
	} else {
	  ATH_MSG_DEBUG("Scheduled finalization of PID=" << presult->pid);
	}
      }
    }
  } else if(doFinalize && func==AthenaMPToolBase::FUNC_FIN) {
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
	} else  {
	  ATH_MSG_DEBUG("Scheduled finalization of PID=" << m_finQueue.front());
	}
      }
    } else {
      // Error: unexpected pid received from presult
      ATH_MSG_ERROR("Finalized PID=" << presult->pid << " while PID=" << pid << " was expected");
      return 1;
    }
  }

  return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

StatusCode
SharedHiveEvtQueueConsumer::initHive() {

    if (m_evtProcessor.release().isFailure()) {
      ATH_MSG_INFO("could not release old EventProcessor ");
    }

    ISvcManager* pISM(dynamic_cast<ISvcManager*>(serviceLocator().get()));
    if (pISM == 0) {
      ATH_MSG_ERROR("initHive: Could not get SvcManager");
    } else {
      if (pISM->removeService(m_evtProcessor.name()).isFailure()) {
	ATH_MSG_ERROR("initHive: could not remove " << m_evtProcessor.name() 
		      << " from SvcManager");
      }
    }      

    m_evtProcessor = ServiceHandle<IEventProcessor>("AthenaHiveEventLoopMgr",name());

    if (m_evtProcessor.retrieve().isFailure()) {
      ATH_MSG_ERROR("could not setup " << m_evtProcessor.typeAndName());
      return StatusCode::FAILURE;
    }

    m_schedulerSvc = serviceLocator()->service("AvalancheSchedulerSvc");

  // m_whiteboard = serviceLocator()->service(m_whiteboardName);
  // if( !m_whiteboard.isValid() )  {
  //   ATH_MSG_FATAL( "Error retrieving " << m_whiteboardName 
  // 		   << " interface IHiveWhiteBoard." );
  //   return StatusCode::FAILURE;
  // }
  
  // m_schedulerSvc = serviceLocator()->service(m_schedulerName);
  // if ( !m_schedulerSvc.isValid()){
  //   ATH_MSG_FATAL( "Error retrieving SchedulerSvc interface ISchedulerSvc." );
  //   return StatusCode::FAILURE;    
  // }
  // // Setup algorithm resource pool
  // m_algResourcePool = serviceLocator()->service("AlgResourcePool");
  // if( !m_algResourcePool.isValid() ) {
  //   ATH_MSG_FATAL ("Error retrieving AlgResourcePool");
  //   return StatusCode::FAILURE;
  // }

  // sc = m_eventStore.retrieve();
  // if( !sc.isSuccess() )  {
  //   ATH_MSG_FATAL("Error retrieving pointer to StoreGateSvc");
  //   return sc;
  // }


  return StatusCode::SUCCESS;

}

