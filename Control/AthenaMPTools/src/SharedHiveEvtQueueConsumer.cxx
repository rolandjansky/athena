/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SharedHiveEvtQueueConsumer.h"
#include "copy_file_icc_hack.h"
#include "AthenaInterprocess/ProcessGroup.h"
#include "AthenaInterprocess/Incidents.h"

#include "AthenaKernel/IDataShare.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "AthenaKernel/IHybridProcessorHelper.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IFileMgr.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IConversionSvc.h"

#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdexcept>
#include <cmath> // For pow

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
  , m_rankId(-1)
  , m_chronoStatSvc("ChronoStatSvc", name)
  , m_evtSelSeek(0)
  , m_evtContext(0)
  , m_sharedEventQueue(0)
  , m_sharedRankQueue(0)
{
  declareInterface<IAthenaMPTool>(this);

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

  StatusCode sc = AthenaMPToolBase::initialize();
  if(!sc.isSuccess())
    return sc;

  sc = serviceLocator()->service(m_evtSelName,m_evtSelSeek);
  if(sc.isFailure() || m_evtSelSeek==0) {
    ATH_MSG_ERROR("Error retrieving IEvtSelectorSeek");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( evtSelector()->createContext (m_evtContext) );

  ATH_CHECK(m_chronoStatSvc.retrieve());

  IConversionSvc* cnvSvc = 0;
  sc = serviceLocator()->service("AthenaPoolCnvSvc",cnvSvc);
  m_dataShare = dynamic_cast<IDataShare*>(cnvSvc);
  if(sc.isFailure() || m_dataShare==0) {
    if(m_useSharedWriter) {
      ATH_MSG_ERROR("Error retrieving AthenaPoolCnvSvc " << cnvSvc);
      return StatusCode::FAILURE;
    }
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
  
  // ________________________ Make Shared Writer Client ________________________

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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::unique_ptr<AthenaInterprocess::ScheduledWork> SharedHiveEvtQueueConsumer::exec_func()
{
  ATH_MSG_INFO("Exec function in the AthenaMP worker PID=" << getpid());

  bool all_ok(true);

  if (!initHive().isSuccess()) {
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

  IHybridProcessorHelper* hybridHelper = dynamic_cast<IHybridProcessorHelper*>(m_evtProcessor.get());
  if(!hybridHelper) {
    ATH_MSG_FATAL("Failed to acquire IHybridProcessorHelper interface");
    all_ok = false;
    return std::unique_ptr<AthenaInterprocess::ScheduledWork>();
  }
  // Reset the application return code.
  hybridHelper->resetAppReturnCode();

  int finishedEvts =0;
  int createdEvts =0;
  long intmask = pow(0x100,sizeof(int))-1; // Mask for decoding event number from the value posted to the queue
  long evtnumAndChunk(0);
//  unsigned evtCounter(0);
  int evtnum(0), chunkSize(1);

  ATH_MSG_INFO("Starting loop on events");

  StatusCode sc(StatusCode::SUCCESS);

  while(!m_sharedEventQueue->try_receive_basic<long>(evtnumAndChunk)) {
    ATH_MSG_DEBUG("Event queue is empty");
    usleep(1000);
  }
  bool loop_ended = (evtnumAndChunk<0);
  if(!loop_ended) {
    ATH_MSG_DEBUG("Received value from the queue 0x" << std::hex << evtnumAndChunk << std::dec);
    chunkSize = evtnumAndChunk >> (sizeof(int)*8);
    evtnum = evtnumAndChunk & intmask;
    ATH_MSG_INFO("Received from the queue: event num=" << evtnum << " chunk size=" << chunkSize);
    hybridHelper->setCurrentEventNum(++evtnum);
  }

  bool no_more_events = false;

  while(!loop_ended) {
    ATH_MSG_DEBUG(" -> createdEvts: " << createdEvts);

    if(!hybridHelper->terminateLoop()         // No scheduled loop termination
       && !no_more_events                     // We are not yet done getting events
       && m_schedulerSvc->freeSlots()>0) {    // There are still free slots in the scheduler
      ATH_MSG_DEBUG("createdEvts: " << createdEvts << ", freeslots: " << m_schedulerSvc->freeSlots());

      auto ctx = m_evtProcessor->createEventContext();
      if(!ctx.valid()) {
        sc = StatusCode::FAILURE;
      }
      else {
        sc = m_evtProcessor->executeEvent(std::move(ctx));
      }

      if (sc.isFailure()) {
        ATH_MSG_ERROR("Terminating event processing loop due to errors");
        loop_ended = true;
      }
      else {
	++createdEvts;
	if(--chunkSize==0) {
	  // Fetch next chunk
	  while(!m_sharedEventQueue->try_receive_basic<long>(evtnumAndChunk)) {
	    ATH_MSG_DEBUG("Event queue is empty");
	    usleep(1000);
	  }
	  if(evtnumAndChunk<0) {
	    no_more_events = true;
	    evtnumAndChunk *= -1;
	    ATH_MSG_DEBUG("No more events are expected. The total number of events for this job = " << evtnumAndChunk);
	  }
	  else {
	    ATH_MSG_DEBUG("Received value from the queue 0x" << std::hex << evtnumAndChunk << std::dec);
	    chunkSize = evtnumAndChunk >> (sizeof(int)*8);
	    evtnum = evtnumAndChunk & intmask;
	    ATH_MSG_INFO("Received from the queue: event num=" << evtnum << " chunk size=" << chunkSize);
	  }
	}
	// Advance to the next event
	if(!no_more_events) {
	  hybridHelper->setCurrentEventNum(++evtnum);
	}
      }
    }
    else {
      // all the events were created but not all finished or the slots were
      // all busy: the scheduler should finish its job
      ATH_MSG_DEBUG("Draining the scheduler");

      // Pull out of the scheduler the finished events
      int ir = hybridHelper->drainScheduler(finishedEvts,true);
      if(ir < 0) {
        // some sort of error draining scheduler;
        loop_ended = true;
        sc = StatusCode::FAILURE;
      }
      else if(ir == 0) {
	// no more events in scheduler
        if(no_more_events) {
          // We are done
          loop_ended = true;
          sc = StatusCode::SUCCESS;
        }
      }
      else {
        // keep going!
      }
    }
  } // end main loop on finished events

  if(all_ok) {
    if(m_evtProcessor->executeRun(0).isFailure()) {
      ATH_MSG_ERROR("Could not finalize the Run");
      all_ok=false;
    } else {
      if(m_evtSelSeek->seek(*m_evtContext, evtnumAndChunk+skipEvents).isFailure()) { 
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
  memcpy((char*)outdata+sizeof(int)+sizeof(func),&createdEvts,sizeof(int));

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

    m_evtProcessor = ServiceHandle<IEventProcessor>("AthenaMtesEventLoopMgr",name());

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

