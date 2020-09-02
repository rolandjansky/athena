/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "HltEventLoopMgr.h"
#include "TrigCOOLUpdateHelper.h"
#include "TrigKernel/HltExceptions.h"
#include "TrigSORFromPtreeHelper.h"
#include "TrigRDBManager.h"
#include "TrigSteeringEvent/HLTResultMT.h"

// Athena includes
#include "AthenaInterprocess/Incidents.h"
#include "AthenaKernel/AthStatusCode.h"
#include "ByteStreamData/ByteStreamMetadata.h"
#include "ByteStreamData/ByteStreamMetadataContainer.h"
#include "EventInfoUtils/EventInfoFromxAOD.h"
#include "StoreGate/StoreGateSvc.h"

// Gaudi includes
#include "GaudiKernel/ConcurrencyFlags.h"
#include "GaudiKernel/IAlgExecStateSvc.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IAlgResourcePool.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/IScheduler.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IIoComponent.h"
#include "GaudiKernel/ThreadLocalContext.h"

// TDAQ includes
#include "eformat/StreamTag.h"
#include "owl/time.h"

// Boost includes
#include <boost/filesystem.hpp>

// System includes
#include <sstream>
#include <string>

// =============================================================================
// Helper macros, typedefs and constants
// =============================================================================
// Base macro for handling failed events in a loop, see below for specific variants
#define HLT_LOOP_CHECK(scexpr,errmsg,errcode,evctx,delctx,retonfail) \
  {                                               \
    StatusCode sccopy = scexpr;                   \
    if (sccopy.isFailure()) {                     \
      ATH_MSG_ERROR(errmsg);                      \
      const EventContext& ctx                     \
        = evctx ? *evctx : EventContext();        \
      if (failedEvent(errcode,ctx).isFailure()) { \
        if (delctx) {delete evctx;}               \
        return retonfail;                         \
      }                                           \
      else                                        \
        continue;                                 \
    }                                             \
  }
// Handles failed event in the event loop, continues the loop if the failure is recoverable,
// otherwise returns StatusCode::FAILURE to break the loop
#define HLT_EVTLOOP_CHECK(scexpr,errmsg,errcode,evctx) \
  HLT_LOOP_CHECK(scexpr,errmsg,errcode,evctx,false,StatusCode::FAILURE)
// Same as above but deletes the EventContext and returns DrainSchedulerStatusCode::FAILURE to break the loop
#define HLT_DRAINSCHED_CHECK(scexpr,errmsg,errcode,evctx) \
  HLT_LOOP_CHECK(scexpr,errmsg,errcode,evctx,true,DrainSchedulerStatusCode::FAILURE)
namespace {
  bool isTimedOut(const std::unordered_map<std::string_view,StatusCode>& algErrors) {
    for (const auto& [key, sc] : algErrors) {
      if (sc == Athena::Status::TIMEOUT) return true;
    }
    return false;
  }
}
using namespace boost::property_tree;

// =============================================================================
// Standard constructor
// =============================================================================
HltEventLoopMgr::HltEventLoopMgr(const std::string& name, ISvcLocator* svcLoc)
: base_class(name, svcLoc),
  m_incidentSvc("IncidentSvc", name),
  m_jobOptionsSvc("JobOptionsSvc", name),
  m_evtStore("StoreGateSvc", name),
  m_detectorStore("DetectorStore", name),
  m_inputMetaDataStore("StoreGateSvc/InputMetaDataStore", name),
  m_ioCompMgr("IoComponentMgr", name)
{
}

// =============================================================================
// Standard destructor
// =============================================================================
HltEventLoopMgr::~HltEventLoopMgr()
{
}

// =============================================================================
// Reimplementation of AthService::initalize (IStateful interface)
// =============================================================================
StatusCode HltEventLoopMgr::initialize()
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  ATH_MSG_INFO(" ---> HltEventLoopMgr = " << name() << " initialize - package version " << PACKAGE_VERSION);

  //----------------------------------------------------------------------------
  // Setup properties
  //----------------------------------------------------------------------------

  // Set the timeout value (cast float to int)
  m_softTimeoutValue = std::chrono::milliseconds(static_cast<int>(m_hardTimeout.value() * m_softTimeoutFraction.value()));

  // Read DataFlow configuration properties
  updateDFProps();

  // print properties
  ATH_MSG_INFO(" ---> ApplicationName           = " << m_applicationName);
  ATH_MSG_INFO(" ---> HardTimeout               = " << m_hardTimeout.value());
  ATH_MSG_INFO(" ---> SoftTimeoutFraction       = " << m_softTimeoutFraction.value());
  ATH_MSG_INFO(" ---> SoftTimeoutValue          = " << m_softTimeoutValue.count());
  ATH_MSG_INFO(" ---> MaxFrameworkErrors        = " << m_maxFrameworkErrors.value());
  ATH_MSG_INFO(" ---> FwkErrorDebugStreamName   = " << m_fwkErrorDebugStreamName.value());
  ATH_MSG_INFO(" ---> AlgErrorDebugStreamName   = " << m_algErrorDebugStreamName.value());
  ATH_MSG_INFO(" ---> TimeoutDebugStreamName    = " << m_timeoutDebugStreamName.value());
  ATH_MSG_INFO(" ---> TruncationDebugStreamName = " << m_truncationDebugStreamName.value());
  ATH_MSG_INFO(" ---> SORPath                   = " << m_sorPath.value());
  ATH_MSG_INFO(" ---> setMagFieldFromPtree      = " << m_setMagFieldFromPtree.value());
  ATH_MSG_INFO(" ---> forceRunNumber            = " << m_forceRunNumber.value());
  ATH_MSG_INFO(" ---> forceStartOfRunTime       = " << m_forceSOR_ns.value());
  ATH_MSG_INFO(" ---> RewriteLVL1               = " << m_rewriteLVL1.value());
  ATH_MSG_INFO(" ---> EventContextWHKey         = " << m_eventContextWHKey.key());
  ATH_MSG_INFO(" ---> EventInfoRHKey            = " << m_eventInfoRHKey.key());

  ATH_CHECK( m_jobOptionsSvc.retrieve() );
  const Gaudi::Details::PropertyBase* prop = m_jobOptionsSvc->getClientProperty("EventDataSvc","NSlots");
  if (prop)
    ATH_MSG_INFO(" ---> NumConcurrentEvents     = " << prop->toString());
  else
    ATH_MSG_WARNING("Failed to retrieve the job property EventDataSvc.NSlots");
  prop = m_jobOptionsSvc->getClientProperty("AvalancheSchedulerSvc","ThreadPoolSize");
  if (prop)
    ATH_MSG_INFO(" ---> NumThreads              = " << prop->toString());
  else
   ATH_MSG_WARNING("Failed to retrieve the job property AvalancheSchedulerSvc.ThreadPoolSize");

  //----------------------------------------------------------------------------
  // Setup all Hive services for multithreaded event processing with the exception of SchedulerSvc,
  // which has to be initialised after forking because it opens new threads
  //----------------------------------------------------------------------------
  m_whiteboard = serviceLocator()->service(m_whiteboardName);
  if( !m_whiteboard.isValid() )  {
    ATH_MSG_FATAL("Error retrieving " << m_whiteboardName << " interface IHiveWhiteBoard");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Initialised " << m_whiteboardName << " interface IHiveWhiteBoard");

  m_algResourcePool = serviceLocator()->service("AlgResourcePool");
  if( !m_algResourcePool.isValid() ) {
    ATH_MSG_FATAL("Error retrieving AlgResourcePool");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("initialised AlgResourcePool");

  m_aess = serviceLocator()->service("AlgExecStateSvc");
  if( !m_aess.isValid() ) {
    ATH_MSG_FATAL("Error retrieving AlgExecStateSvc");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("initialised AlgExecStateSvc");

  //----------------------------------------------------------------------------
  // Initialise services
  //----------------------------------------------------------------------------
  ATH_CHECK(m_incidentSvc.retrieve());
  ATH_CHECK(m_evtStore.retrieve());
  ATH_CHECK(m_detectorStore.retrieve());
  ATH_CHECK(m_inputMetaDataStore.retrieve());
  ATH_CHECK(m_evtSelector.retrieve());
  ATH_CHECK(m_evtSelector->createContext(m_evtSelContext)); // create an EvtSelectorContext
  ATH_CHECK(m_outputCnvSvc.retrieve());
  ATH_CHECK(m_ioCompMgr.retrieve());
  if (m_monitorScheduler) {
    ATH_CHECK(m_schedulerMonSvc.retrieve());
  }

  //----------------------------------------------------------------------------
  // Initialise tools
  //----------------------------------------------------------------------------
  // COOL helper
  ATH_CHECK(m_coolHelper.retrieve());
  // HLT result builder
  ATH_CHECK(m_hltResultMaker.retrieve());
  // Monitoring tool
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());

  //----------------------------------------------------------------------------
  // Initialise data handle keys
  //----------------------------------------------------------------------------
  // EventContext WriteHandle
  ATH_CHECK(m_eventContextWHKey.initialize());
  // EventInfo ReadHandle
  ATH_CHECK(m_eventInfoRHKey.initialize());
  // HLTResultMT ReadHandle (created dynamically from the result builder property)
  m_hltResultRHKey = m_hltResultMaker->resultName();
  ATH_CHECK(m_hltResultRHKey.initialize());
  // L1TriggerResult ReadHandle
  ATH_CHECK(m_l1TriggerResultRHKey.initialize(m_rewriteLVL1.value()));

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Reimplementation of AthService::stop (IStateful interface)
// =============================================================================
StatusCode HltEventLoopMgr::stop()
{
  // Need to reinitialize IO in the mother process
  if (m_workerID==0) {
    ATH_CHECK(m_ioCompMgr->io_reinitialize());
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
// Reimplementation of AthService::finalize (IStateful interface)
// =============================================================================
StatusCode HltEventLoopMgr::finalize()
{
  ATH_MSG_INFO(" ---> HltEventLoopMgr/" << name() << " finalize ");
  // Usually (but not necessarily) corresponds to the number of processed events +1
  ATH_MSG_INFO("Total number of EventContext objects created " << m_localEventNumber);

  // Release all handles
  auto releaseAndCheck = [&](auto& handle, std::string handleType) {
    if (handle.release().isFailure())
      ATH_MSG_WARNING("finalize(): Failed to release " << handleType << " " << handle.typeAndName());
  };
  auto releaseService = [&](auto&&... args) { (releaseAndCheck(args,"service"), ...); };
  auto releaseTool = [&](auto&&... args) { (releaseAndCheck(args,"tool"), ...); };
  auto releaseSmartIF = [](auto&&... args) { (args.reset(), ...); };

  releaseService(m_incidentSvc,
                 m_evtStore,
                 m_detectorStore,
                 m_inputMetaDataStore,
                 m_evtSelector,
                 m_outputCnvSvc,
                 m_schedulerMonSvc);

  releaseTool(m_coolHelper,
              m_hltResultMaker,
              m_monTool);

  releaseSmartIF(m_whiteboard,
                 m_algResourcePool,
                 m_aess,
                 m_schedulerSvc);

  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of ITrigEventLoopMgr::prepareForStart
// =============================================================================
StatusCode HltEventLoopMgr::prepareForStart(const ptree& pt)
{
  try {
    const auto& rparams = pt.get_child("RunParams");
    m_sorHelper = std::make_unique<TrigSORFromPtreeHelper>(msgSvc(), m_detectorStore, m_sorPath, rparams);
  }
  catch(ptree_bad_path& e) {
    ATH_MSG_ERROR("Bad ptree path: \"" << e.path<ptree::path_type>().dump() << "\" - " << e.what());
    return StatusCode::FAILURE;
  }

  // Override run/timestamp if needed
  if (m_forceRunNumber > 0) {
    m_sorHelper->setRunNumber(m_forceRunNumber);
    ATH_MSG_WARNING("Run number overwrite:" << m_forceRunNumber);
  }
  if (m_forceSOR_ns > 0) {
    m_sorHelper->setSORtime_ns(m_forceSOR_ns);
    ATH_MSG_WARNING("SOR time overwrite:" << m_forceSOR_ns);
  }

  // Set our "run context" (invalid event/slot)
  m_currentRunCtx.setEventID( m_sorHelper->eventID() );
  m_currentRunCtx.setExtension(Atlas::ExtendedEventContext(m_evtStore->hiveProxyDict(),
                                                           m_currentRunCtx.eventID().run_number()));

  // Some algorithms expect a valid context during start()
  ATH_MSG_DEBUG("Setting context for start transition: " << m_currentRunCtx.eventID());
  Gaudi::Hive::setCurrentContext(m_currentRunCtx);

  return StatusCode::SUCCESS;
}


// =============================================================================
// Implementation of ITrigEventLoopMgr::prepareForRun
// =============================================================================
StatusCode HltEventLoopMgr::prepareForRun(const ptree& pt)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  try
  {
    // (void)TClass::GetClass("vector<unsigned short>"); // preload to overcome an issue with dangling references in serialization
    // (void)TClass::GetClass("vector<unsigned long>");

    ATH_CHECK( clearTemporaryStores() );                 // do the necessary resets
    ATH_CHECK( m_sorHelper->fillSOR(m_currentRunCtx) );  // update SOR in det store
    ATH_CHECK( updateMagField(pt) );                     // update magnetic field

    auto& soral = getSorAttrList();

    updateInternal(soral);       // update internally kept info
    updateMetadataStore(soral);  // update metadata store

    m_incidentSvc->fireIncident(Incident(name(), IncidentType::BeginRun, m_currentRunCtx));

    // Initialize COOL helper (needs to be done after IOVDbSvc has loaded all folders)
    ATH_CHECK(m_coolHelper->readFolderInfo());

    // close any open files (e.g. THistSvc)
    ATH_CHECK(m_ioCompMgr->io_finalize());

    // close open DB connections
    ATH_CHECK(TrigRDBManager::closeDBConnections(msg()));

    // Assert that scheduler has not been initialised before forking
    SmartIF<IService> svc = serviceLocator()->service(m_schedulerName, /*createIf=*/ false);
    if (svc.isValid()) {
      ATH_MSG_FATAL("Misconfiguration - Scheduler was initialised before forking!");
      return StatusCode::FAILURE;
    }

    ATH_MSG_VERBOSE("end of " << __FUNCTION__);
    return StatusCode::SUCCESS;
  }
  catch(const std::runtime_error& e)
  {
    ATH_MSG_ERROR("Runtime error: " << e.what());
  }

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::FAILURE;
}

// =============================================================================
// Implementation of ITrigEventLoopMgr::hltUpdateAfterFork
// =============================================================================
StatusCode HltEventLoopMgr::hltUpdateAfterFork(const ptree& /*pt*/)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  updateDFProps();
  ATH_MSG_INFO("Post-fork initialization for " << m_applicationName);

  ATH_MSG_DEBUG("Initialising the scheduler after forking");
  m_schedulerSvc = serviceLocator()->service(m_schedulerName, /*createIf=*/ true);
  if ( !m_schedulerSvc.isValid()){
    ATH_MSG_FATAL("Error retrieving " << m_schedulerName << " interface ISchedulerSvc");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Initialised " << m_schedulerName << " interface ISchedulerSvc");

  ATH_MSG_DEBUG("Trying a stop-start of CoreDumpSvc");
  SmartIF<IService> svc = serviceLocator()->service("CoreDumpSvc", /*createIf=*/ false);
  if (svc.isValid()) {
    StatusCode sc = svc->stop();
    sc &= svc->start();
    if (sc.isFailure()) {
      ATH_MSG_WARNING("Could not perform stop/start for CoreDumpSvc");
    }
    else {
      ATH_MSG_DEBUG("Done a stop-start of CoreDumpSvc");
    }
  }
  else {
    ATH_MSG_WARNING("Could not retrieve CoreDumpSvc");
  }

  // Make sure output files, i.e. histograms are written to their own directory.
  // Nothing happens if the online TrigMonTHistSvc is used as there are no output files.
  SmartIF<IIoComponent> histsvc = serviceLocator()->service("THistSvc", /*createIf=*/ false).as<IIoComponent>();
  if ( !m_ioCompMgr->io_retrieve(histsvc.get()).empty() ) {
    boost::filesystem::path worker_dir = boost::filesystem::absolute("athenaHLT_workers");
    std::ostringstream oss;
    oss << "athenaHLT-" << std::setfill('0') << std::setw(2) << m_workerID;
    worker_dir /= oss.str();
    // Delete worker directory if it exists already
    if ( boost::filesystem::exists(worker_dir) ) {
      if ( !boost::filesystem::remove_all(worker_dir) ) {
        ATH_MSG_FATAL("Cannot delete previous worker directory " << worker_dir);
        return StatusCode::FAILURE;
      }
    }
    if ( !boost::filesystem::create_directories(worker_dir) ) {
      ATH_MSG_FATAL("Cannot create worker directory " << worker_dir);
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Writing worker output files to " << worker_dir);
    ATH_CHECK(m_ioCompMgr->io_update_all(worker_dir.string()));
  }
  ATH_CHECK(m_ioCompMgr->io_reinitialize());

  // Start the timeout thread
  ATH_MSG_DEBUG("Starting the timeout thread");
  m_timeoutThread.reset(new std::thread(std::bind(&HltEventLoopMgr::runEventTimer,this)));

  // Initialise vector of time points for event timeout monitoring
  {
    std::unique_lock<std::mutex> lock(m_timeoutMutex);
    m_eventTimerStartPoint.clear();
    m_eventTimerStartPoint.resize(m_whiteboard->getNumberOfStores(), std::chrono::steady_clock::now());
    m_isSlotProcessing.resize(m_whiteboard->getNumberOfStores(), false);
  }
  m_timeoutCond.notify_all();

  // Initialise vector of time points for free slots monitoring
  m_freeSlotStartPoint.clear();
  m_freeSlotStartPoint.resize(m_whiteboard->getNumberOfStores(), std::chrono::steady_clock::now());

  // Fire incident to update listeners after forking
  m_incidentSvc->fireIncident(AthenaInterprocess::UpdateAfterFork(m_workerID, m_workerPID, name(), m_currentRunCtx));

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IEventProcessor::executeRun
// =============================================================================
StatusCode HltEventLoopMgr::executeRun(int maxevt)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  if (m_monitorScheduler) ATH_CHECK(m_schedulerMonSvc->startMonitoring());

  StatusCode sc = StatusCode::SUCCESS;
  try {
    sc = nextEvent(maxevt);
    if (sc.isFailure()) ATH_MSG_FATAL("Event loop failed");
  }
  catch (const std::exception& e) {
    ATH_MSG_FATAL("Event loop failed, std::exception caught: " << e.what());
    sc = StatusCode::FAILURE;
  }
  catch (...) {
    ATH_MSG_FATAL("Event loop failed, unknown exception caught");
    sc = StatusCode::FAILURE;
  }

  if (m_monitorScheduler) ATH_CHECK(m_schedulerMonSvc->stopMonitoring());

  // Stop the timer thread
  {
    ATH_MSG_DEBUG("Stopping the timeout thread");
    std::unique_lock<std::mutex> lock(m_timeoutMutex);
    m_runEventTimer = false;
  }
  m_timeoutCond.notify_all();
  m_timeoutThread->join();
  m_timeoutThread.reset();
  ATH_MSG_DEBUG("The timeout thread finished");

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return sc;
}

// =============================================================================
// Implementation of IEventProcessor::nextEvent
// maxevt is not used - we always want to process all events delivered
// =============================================================================
StatusCode HltEventLoopMgr::nextEvent(int /*maxevt*/)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  ATH_MSG_INFO("Starting loop on events");

  EventID::number_type maxLB = 0;  // Max lumiblock number we have seen
  bool events_available = true; // Event source has more events
  bool trigger_on_hold = false; // Event source temporarily paused providing events
  bool loop_ended = false; // No more events available and all ongoing processing has finished

  while (!loop_ended) {
    ATH_MSG_DEBUG("Free processing slots = " << m_schedulerSvc->freeSlots());
    ATH_MSG_DEBUG("Free event data slots = " << m_whiteboard->freeSlots());

    if (m_schedulerSvc->freeSlots() != m_whiteboard->freeSlots()) {
      // Starvation detected - try to recover and return FAILURE if the recovery fails. This can only happen if there
      // is an unhandled error after popping an event from the scheduler and before clearing the event data slot for
      // this finished event. It's an extra protection in the unlikely case that failedEvent doesn't cover all errors.
      ATH_CHECK(recoverFromStarvation());
    }

    // Decide what to do in this event loop step
    bool do_start_next_event = m_schedulerSvc->freeSlots()>0 && events_available && !trigger_on_hold;
    bool do_drain_scheduler = !do_start_next_event;
    // Clear the trigger_on_hold flag
    if (trigger_on_hold) trigger_on_hold = false;

    // Read in and start processing another event
    if (do_start_next_event) {
      ATH_MSG_DEBUG("Free slots = " << m_schedulerSvc->freeSlots() << ". Reading the next event.");

      //------------------------------------------------------------------------
      // Allocate event slot and create new EventContext
      //------------------------------------------------------------------------
      ++m_localEventNumber;

      // Create an EventContext, allocating and selecting a whiteboard slot
      std::unique_ptr<EventContext> eventContextPtr = std::make_unique<EventContext>( createEventContext() );

      HLT_EVTLOOP_CHECK(( eventContextPtr->valid() ? StatusCode(StatusCode::SUCCESS) : StatusCode(StatusCode::FAILURE)),
                        "Failed to allocate slot for a new event",
                        HLT::OnlineErrorCode::BEFORE_NEXT_EVENT, eventContextPtr.get());
      HLT_EVTLOOP_CHECK(m_whiteboard->selectStore(eventContextPtr->slot()),
                        "Failed to select event store slot number " << eventContextPtr->slot(),
                        HLT::OnlineErrorCode::BEFORE_NEXT_EVENT, eventContextPtr.get());

      // We can completely avoid using ThreadLocalContext if we store the EventContext in the event store. Any
      // service/tool method which does not allow to pass EventContext as argument, can const-retrieve it from the
      // event store rather than using ThreadLocalContext.

      // We link the current store in the extension of the EventContext we just created. Only then we create
      // a WriteHandle for the EventContext using the EventContext itself. The handle will use the linked hiveProxyDict
      // to record the context in the current store.
      eventContextPtr->setExtension( Atlas::ExtendedEventContext(m_evtStore->hiveProxyDict(),
                                                                 m_currentRunCtx.eventID().run_number()) );
      auto eventContext = SG::makeHandle(m_eventContextWHKey,*eventContextPtr);
      HLT_EVTLOOP_CHECK(eventContext.record(std::move(eventContextPtr)),
                        "Failed to record new EventContext in the event store",
                        HLT::OnlineErrorCode::BEFORE_NEXT_EVENT, eventContextPtr.get());

      // Reset the AlgExecStateSvc
      m_aess->reset(*eventContext);

      ATH_MSG_DEBUG("Created new EventContext with number: " << eventContext->evt()
                    << ", slot: " << eventContext->slot());

      // This ThreadLocalContext call is a not-so-nice behind-the-scenes way to inform some services about the current
      // context. If possible, services should use EventContext from the event store as recorded above. We have to set
      // the ThreadLocalContext here because some services still use it.
      Gaudi::Hive::setCurrentContext(*eventContext);

      //------------------------------------------------------------------------
      // Create a new address for EventInfo to facilitate automatic conversion from input data
      //------------------------------------------------------------------------
      IOpaqueAddress* addr = nullptr;
      HLT_EVTLOOP_CHECK(m_evtSelector->createAddress(*m_evtSelContext, addr),
                        "Event selector failed to create an IOpaqueAddress",
                        HLT::OnlineErrorCode::BEFORE_NEXT_EVENT, eventContext.ptr());

      //------------------------------------------------------------------------
      // Get the next event
      //------------------------------------------------------------------------
      StatusCode sc = StatusCode::SUCCESS;
      try {
        sc = m_evtSelector->next(*m_evtSelContext);
      }
      catch (const hltonl::Exception::NoMoreEvents& e) {
        sc = StatusCode::SUCCESS;
        events_available = false;
        sc = clearWBSlot(eventContext->slot());
        if (sc.isFailure()) {
          ATH_MSG_WARNING("Failed to clear the whiteboard slot " << eventContext->slot()
                          << " after NoMoreEvents detected");
        }
        continue;
      }
      catch (const hltonl::Exception::NoEventsTemporarily& e) {
        sc = StatusCode::SUCCESS;
        trigger_on_hold = true;
        sc = clearWBSlot(eventContext->slot());
        if (sc.isFailure()) {
          ATH_MSG_WARNING("Failed to clear the whiteboard slot " << eventContext->slot()
                          << " after NoEventsTemporarily detected");
        }
        continue;
      }
      catch (const std::exception& e) {
        ATH_MSG_ERROR("Failed to get next event from the event source, std::exception caught: " << e.what());
        sc = StatusCode::FAILURE;
      }
      catch (...) {
        ATH_MSG_ERROR("Failed to get next event from the event source, unknown exception caught");
        sc = StatusCode::FAILURE;
      }
      HLT_EVTLOOP_CHECK(sc, "Failed to get the next event",
                        HLT::OnlineErrorCode::CANNOT_RETRIEVE_EVENT, eventContext.ptr());

      //------------------------------------------------------------------------
      // Set event processing start time for timeout monitoring and reset timeout flag
      //------------------------------------------------------------------------
      {
        std::unique_lock<std::mutex> lock(m_timeoutMutex);
        m_eventTimerStartPoint[eventContext->slot()] = std::chrono::steady_clock::now();
        m_isSlotProcessing[eventContext->slot()] = true;
        resetTimeout(Athena::Timeout::instance(*eventContext));
      }
      m_timeoutCond.notify_all();

      //------------------------------------------------------------------------
      // Load event proxies and get event info
      //------------------------------------------------------------------------
      HLT_EVTLOOP_CHECK(m_evtStore->loadEventProxies(), "Failed to load event proxies",
                        HLT::OnlineErrorCode::NO_EVENT_INFO, eventContext.ptr());

      auto eventInfo = SG::makeHandle(m_eventInfoRHKey,*eventContext);
      HLT_EVTLOOP_CHECK((eventInfo.isValid() ? StatusCode(StatusCode::SUCCESS) : StatusCode(StatusCode::FAILURE)),
                        "Failed to retrieve EventInfo",
                        HLT::OnlineErrorCode::NO_EVENT_INFO, eventContext.ptr());

      ATH_MSG_DEBUG("Retrieved event info for the new event " << *eventInfo);

      // Set EventID for the EventContext
      EventIDBase eid = eventIDFromxAOD(eventInfo.cptr());
      // Override run/LB/timestamp if needed
      if (m_forceRunNumber > 0) {
        eid.set_run_number(m_forceRunNumber);
      }
      if (m_forceLumiblock > 0) {
        eid.set_lumi_block(m_forceLumiblock);
      }
      if (m_forceSOR_ns > 0) {
        eid.set_time_stamp(m_forceSOR_ns / 1000000000);
        eid.set_time_stamp_ns_offset(m_forceSOR_ns % 1000000000);
      }
      eventContext->setEventID(eid);

      // Update thread-local EventContext after setting EventID
      Gaudi::Hive::setCurrentContext(*eventContext);

      //-----------------------------------------------------------------------
      // COOL updates for LB changes
      //-----------------------------------------------------------------------
      // Schedule COOL folder updates based on CTP fragment
      HLT_EVTLOOP_CHECK(m_coolHelper->scheduleFolderUpdates(*eventContext), "Failure reading CTP extra payload",
                        HLT::OnlineErrorCode::COOL_UPDATE, eventContext.ptr());

      // Do an update if this is a new LB
      if ( maxLB < eventContext->eventID().lumi_block() ) {
        maxLB = eventContext->eventID().lumi_block();
        HLT_EVTLOOP_CHECK(m_coolHelper->hltCoolUpdate(*eventContext), "Failure during COOL update",
                          HLT::OnlineErrorCode::COOL_UPDATE, eventContext.ptr());
      }

      //------------------------------------------------------------------------
      // Process the event
      //------------------------------------------------------------------------
      // We need to make a copy of eventContext, as executeEvent uses move semantics and eventContext is already owned
      // by the event store. The copy we create here is pushed to the scheduler and retrieved back in drainScheduler
      // where we have to delete it.
      HLT_EVTLOOP_CHECK(executeEvent( EventContext(*eventContext) ),
                        "Failed to schedule event processing",
                        HLT::OnlineErrorCode::SCHEDULING_FAILURE, eventContext.ptr());

      //------------------------------------------------------------------------
      // Set ThreadLocalContext to an invalid context
      //------------------------------------------------------------------------
      // We have passed the event to the scheduler and we are entering back a context-less environment
      Gaudi::Hive::setCurrentContext( EventContext() );

    } // End of if(do_start_next_event)

    // Wait for events to finish processing and write their output
    if (do_drain_scheduler) {
      ATH_MSG_DEBUG("No free slots or no more events to process - draining the scheduler");
      DrainSchedulerStatusCode drainResult = drainScheduler();
      if (drainResult==DrainSchedulerStatusCode::FAILURE) {
        ATH_MSG_ERROR("Error in draining scheduler, exiting the event loop");
        return StatusCode::FAILURE;
      }
      if (drainResult==DrainSchedulerStatusCode::RECOVERABLE) {
        ATH_MSG_WARNING("Recoverable error in draining scheduler, continuing the event loop");
        continue;
      }
      else if (drainResult==DrainSchedulerStatusCode::SCHEDULER_EMPTY && !events_available) {
        ATH_MSG_INFO("All events processed, finalising the event loop");
        loop_ended = true;
      }
      // else drainResult is SUCCESS, so we just continue
    }
  }

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IEventProcessor::stopRun (obsolete for online runnning)
// =============================================================================
StatusCode HltEventLoopMgr::stopRun() {
  ATH_MSG_FATAL("Misconfiguration - the method HltEventLoopMgr::stopRun() cannot be used online");
  return StatusCode::FAILURE;
}

// =============================================================================
// Implementation of IEventProcessor::createEventContext
// =============================================================================
EventContext HltEventLoopMgr::createEventContext() {

  auto slot = m_whiteboard->allocateStore(m_localEventNumber); // returns npos on failure
  if (slot == std::string::npos) {
    // return an invalid EventContext
    return EventContext();
  } else {
    return EventContext{ m_localEventNumber, slot };
  }

}

// =============================================================================
// Implementation of IEventProcessor::executeEvent
// =============================================================================
StatusCode HltEventLoopMgr::executeEvent(EventContext &&ctx)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  resetTimeout(Athena::Timeout::instance(ctx));

  // Monitor slot idle time (between scheduler popFinishedEvent and pushNewEvent)
  // Note this is time of a scheduler slot being free, not equal to the time of a whiteboard slot being free
  const auto slotIdleTime = std::chrono::steady_clock::now() - m_freeSlotStartPoint[ctx.slot()];
  Monitored::Scalar<int64_t> monSlotIdleTime("SlotIdleTime", std::chrono::duration_cast<std::chrono::milliseconds>(slotIdleTime).count());
  auto mon = Monitored::Group(m_monTool, monSlotIdleTime);

  // Now add event to the scheduler
  ATH_MSG_DEBUG("Adding event " <<  ctx.evt() << ", slot " << ctx.slot() << " to the scheduler");
  StatusCode addEventStatus = m_schedulerSvc->pushNewEvent( new EventContext{std::move(ctx)} );

  // If this fails, we need to wait for something to complete
  if (addEventStatus.isFailure()){
    ATH_MSG_ERROR("Failed adding event " << ctx.evt() << ", slot " << ctx.slot() << " to the scheduler");
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
void HltEventLoopMgr::updateDFProps()
{
  auto getDFProp = [&](const std::string& name, std::string& value, bool required = true) {
                     const auto* prop = m_jobOptionsSvc->getClientProperty("DataFlowConfig", name);
                     if (prop) {
                       value = prop->toString();
                       ATH_MSG_INFO(" ---> Read from DataFlow configuration: " << name << " = " << value);
                     } else {
                       msg() << (required ? MSG::WARNING : MSG::INFO)
                             << "Could not set Property " << name << " from DataFlow" << endmsg;
                     }
                   };

  getDFProp( "DF_ApplicationName", m_applicationName );
  std::string wid, wpid;
  getDFProp( "DF_WorkerId", wid, false );
  getDFProp( "DF_Pid", wpid, false );
  if (!wid.empty()) m_workerID = std::stoi(wid);
  if (!wpid.empty()) m_workerPID = std::stoi(wpid);
}

// =============================================================================
void HltEventLoopMgr::updateInternal(const coral::AttributeList & sor_attrlist)
{
  auto detMaskFst = sor_attrlist["DetectorMaskFst"].data<unsigned long long>();
  auto detMaskSnd = sor_attrlist["DetectorMaskSnd"].data<unsigned long long>();
  updateDetMask({detMaskFst, detMaskSnd});

  if(msgLevel() <= MSG::DEBUG)
  {
    // save current stream flags for later reset
    // cast needed (stream thing returns long, but doesn't take it back)
    auto previous_stream_flags = static_cast<std::ios::fmtflags>(msgStream().flags());
    ATH_MSG_DEBUG("Full detector mask (128 bits) = 0x"
                  << MSG::hex << std::setfill('0')
                  << std::setw(8) << std::get<3>(m_detector_mask)
                  << std::setw(8) << std::get<2>(m_detector_mask)
                  << std::setw(8) << std::get<1>(m_detector_mask)
                  << std::setw(8) << std::get<0>(m_detector_mask));
    msgStream().flags(previous_stream_flags);
  }
}

// =============================================================================
void HltEventLoopMgr::updateMetadataStore(const coral::AttributeList & sor_attrlist) const
{
  // least significant part is "snd" in sor but "fst" for ByteStreamMetadata
  auto bs_dm_fst = sor_attrlist["DetectorMaskSnd"].data<unsigned long long>();
  // most significant part is "fst" in sor but "snd" for ByteStreamMetadata
  auto bs_dm_snd = sor_attrlist["DetectorMaskFst"].data<unsigned long long>();

  auto metadatacont = std::make_unique<ByteStreamMetadataContainer>();
  metadatacont->push_back(std::make_unique<ByteStreamMetadata>(
    sor_attrlist["RunNumber"].data<unsigned int>(),
    0,
    0,
    sor_attrlist["RecordingEnabled"].data<bool>(),
    0,
    bs_dm_fst,
    bs_dm_snd,
    0,
    0,
    "",
    "",
    "",
    0,
    std::vector<std::string>()
  ));
  // Record ByteStreamMetadataContainer in MetaData Store
  if(m_inputMetaDataStore->record(std::move(metadatacont),"ByteStreamMetadata").isFailure()) {
    ATH_MSG_WARNING("Unable to record MetaData in InputMetaDataStore");
  }
  else {
    ATH_MSG_DEBUG("Recorded MetaData in InputMetaDataStore");
  }
}

//=========================================================================
StatusCode HltEventLoopMgr::updateMagField(const ptree& pt) const
{
  if (m_setMagFieldFromPtree) {
    try {
      auto tor_cur = pt.get<float>("Magnets.ToroidsCurrent.value");
      auto sol_cur = pt.get<float>("Magnets.SolenoidCurrent.value");

      // Set currents on service (deprecated: ATLASRECTS-4687)
      IProperty* fieldSvc{nullptr};
      service("AtlasFieldSvc", fieldSvc, /*createIf=*/false).ignore();
      if ( fieldSvc ) {
        ATH_MSG_INFO("Setting field currents on AtlasFieldSvc");
        ATH_CHECK( Gaudi::Utils::setProperty(fieldSvc, "UseSoleCurrent", sol_cur) );
        ATH_CHECK( Gaudi::Utils::setProperty(fieldSvc, "UseToroCurrent", tor_cur) );
      }
      else ATH_MSG_WARNING("Cannot retrieve AtlasFieldSvc");

      // Set current on conditions alg
      const IAlgManager* algMgr = Gaudi::svcLocator()->as<IAlgManager>();
      IAlgorithm* fieldAlg{nullptr};
      algMgr->getAlgorithm("AtlasFieldMapCondAlg", fieldAlg).ignore();
      if ( fieldAlg ) {
        ATH_MSG_INFO("Setting field currents on AtlasFieldMapCondAlg");
        ATH_CHECK( Gaudi::Utils::setProperty(fieldAlg, "MapSoleCurrent", sol_cur) );
        ATH_CHECK( Gaudi::Utils::setProperty(fieldAlg, "MapToroCurrent", tor_cur) );
      }
      else ATH_MSG_WARNING("Cannot retrieve AtlasFieldMapCondAlg");

      ATH_MSG_INFO("*****************************************");
      ATH_MSG_INFO("  Auto-configuration of magnetic field:  ");
      ATH_MSG_INFO("    solenoid current from IS = " << sol_cur);
      ATH_MSG_INFO("     torroid current from IS = " << tor_cur);
      ATH_MSG_INFO("*****************************************");
    }
    catch(ptree_bad_path& e) {
      ATH_MSG_ERROR( "Cannot read magnet currents from ptree: " << e.what() );
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}


// =============================================================================
StatusCode HltEventLoopMgr::clearTemporaryStores()
{
  //----------------------------------------------------------------------------
  // Clear the event store, if used in the event loop
  //----------------------------------------------------------------------------
  ATH_CHECK(m_evtStore->clearStore());
  ATH_MSG_DEBUG("Cleared the EventStore");

  //----------------------------------------------------------------------------
  // Clear the InputMetaDataStore
  //----------------------------------------------------------------------------
  ATH_CHECK(m_inputMetaDataStore->clearStore());
  ATH_MSG_DEBUG("Cleared the InputMetaDataStore");

  return StatusCode::SUCCESS;
}

// =============================================================================
void HltEventLoopMgr::updateDetMask(const std::pair<uint64_t, uint64_t>& dm)
{
  m_detector_mask = std::make_tuple(
                      // least significant 4 bytes
                      static_cast<EventIDBase::number_type>(dm.second),
                      // next least significant 4 bytes
                      static_cast<EventIDBase::number_type>(dm.second >> 32),
                      // next least significant 4 bytes
                      static_cast<EventIDBase::number_type>(dm.first),
                      // most significant 4 bytes
                      static_cast<EventIDBase::number_type>(dm.first >> 32)
                    );
}

// =============================================================================
const coral::AttributeList& HltEventLoopMgr::getSorAttrList() const
{
  auto sor = m_detectorStore->retrieve<const TrigSORFromPtreeHelper::SOR>(m_sorPath);
  if (sor==nullptr) {
    throw std::runtime_error("Cannot retrieve " + m_sorPath);
  }
  if(sor->size() != 1)
  {
    // This branch should never be entered (the CondAttrListCollection
    // corresponding to the SOR should contain one single AttrList). Since
    // that's required by code ahead but not checked at compile time, we
    // explicitly guard against any potential future mistake with this check
    throw std::runtime_error("SOR record should have one and one only attribute list, but it has " + std::to_string(sor->size()));
  }

  const auto & soral = sor->begin()->second;
  printSORAttrList(soral);
  return soral;
}

// =============================================================================
void HltEventLoopMgr::printSORAttrList(const coral::AttributeList& atr) const
{
  unsigned long long sorTime_ns(atr["SORTime"].data<unsigned long long>());

  // Human readable format of SOR time if available
  time_t sorTime_sec = sorTime_ns/1000000000;
  const auto sorTime_readable = OWLTime(sorTime_sec);

  ATH_MSG_INFO("SOR parameters:");
  ATH_MSG_INFO("   RunNumber             = " << atr["RunNumber"].data<unsigned int>());
  ATH_MSG_INFO("   SORTime [ns]          = " << sorTime_ns << " (" << sorTime_readable << ") ");

  // Use string stream for fixed-width hex detector mask formatting
  auto dmfst = atr["DetectorMaskFst"].data<unsigned long long>();
  auto dmsnd = atr["DetectorMaskSnd"].data<unsigned long long>();
  std::ostringstream ss;
  ss.setf(std::ios_base::hex,std::ios_base::basefield);
  ss << std::setw(16) << std::setfill('0') << dmfst;
  ATH_MSG_INFO("   DetectorMaskFst       = 0x" << ss.str());
  ss.str(""); // reset the string stream
  ss << std::setw(16) << std::setfill('0') << dmsnd;
  ATH_MSG_INFO("   DetectorMaskSnd       = 0x" << ss.str());
  ss.str(""); // reset the string stream
  ss << std::setw(16) << std::setfill('0') << dmfst;
  ss << std::setw(16) << std::setfill('0') << dmsnd;
  ATH_MSG_INFO("   Complete DetectorMask = 0x" << ss.str());

  ATH_MSG_INFO("   RunType               = " << atr["RunType"].data<std::string>());
  ATH_MSG_INFO("   RecordingEnabled      = " << (atr["RecordingEnabled"].data<bool>() ? "true" : "false"));
}

// =============================================================================
StatusCode HltEventLoopMgr::failedEvent(HLT::OnlineErrorCode errorCode, const EventContext& eventContext)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__ << " with errorCode = " << errorCode
                  << ", context = " << eventContext << " eventID = " << eventContext.eventID());

  // Used by MsgSvc (and possibly others but not relevant here)
  Gaudi::Hive::setCurrentContext(eventContext);

  auto drainAllAndProceed = [&]() -> StatusCode {
    ATH_CHECK(drainAllSlots()); // break the event loop on failure
    if ( m_maxFrameworkErrors.value()>=0 && ((++m_nFrameworkErrors)>m_maxFrameworkErrors.value()) ) {
      ATH_MSG_ERROR("The number of tolerable framework errors for this HltEventLoopMgr instance, which is "
                    << m_maxFrameworkErrors.value() << ", was exceeded. Exiting the event loop.");
      return StatusCode::FAILURE; // break the event loop
    }
    else return StatusCode::SUCCESS; // continue the event loop
  };

  //----------------------------------------------------------------------------
  // Handle cases where we can only try to clear all slots and continue
  //----------------------------------------------------------------------------

  if (errorCode==HLT::OnlineErrorCode::BEFORE_NEXT_EVENT) {
    ATH_MSG_ERROR("Failure occurred with OnlineErrorCode=" << errorCode
      << " meaning there was a framework error before requesting a new event. No output will be produced and"
      << " all slots of this HltEventLoopMgr instance will be drained before proceeding.");
    return drainAllAndProceed();
  }
  else if (errorCode==HLT::OnlineErrorCode::AFTER_RESULT_SENT) {
    ATH_MSG_ERROR("Failure occurred with OnlineErrorCode=" << errorCode
      << " meaning there was a framework error after HLT result was already sent out."
      << " All slots of this HltEventLoopMgr instance will be drained before proceeding.");
    return drainAllAndProceed();
  }
  else if (errorCode==HLT::OnlineErrorCode::CANNOT_ACCESS_SLOT) {
    ATH_MSG_ERROR("Failed to access the slot for the processed event, cannot produce output. OnlineErrorCode=" << errorCode
      << ". All slots of this HltEventLoopMgr instance will be drained before proceeding, then either the loop will"
      << " exit with a failure code or the failed event will reach a hard timeout.");
    return drainAllAndProceed();
  }
  else if (!eventContext.valid()) {
    ATH_MSG_ERROR("Failure occurred with an invalid EventContext. Likely there was a framework error before"
      << " requesting a new event or after sending the result of a finished event. OnlineErrorCode=" << errorCode
      << ". All slots of this HltEventLoopMgr instance will be drained before proceeding.");
    return drainAllAndProceed();
  }

  //----------------------------------------------------------------------------
  // In case of event source failure, drain the scheduler and break the loop
  //----------------------------------------------------------------------------
  if (errorCode==HLT::OnlineErrorCode::CANNOT_RETRIEVE_EVENT) {
    ATH_MSG_ERROR("Failure occurred with OnlineErrorCode=" << errorCode
      << " meaning a new event could not be correctly read. No output will be produced for this event."
      << " All slots of this HltEventLoopMgr instance will be drained and the loop will exit.");
    ATH_CHECK(drainAllSlots());
    return StatusCode::FAILURE;
  }

  //----------------------------------------------------------------------------
  // Make sure we are using the right store
  //----------------------------------------------------------------------------
  if (m_whiteboard->selectStore(eventContext.slot()).isFailure()) {
    return failedEvent(HLT::OnlineErrorCode::CANNOT_ACCESS_SLOT,eventContext);
  }

  //----------------------------------------------------------------------------
  // Handle SCHEDULING_FAILURE
  //----------------------------------------------------------------------------
  if (errorCode==HLT::OnlineErrorCode::SCHEDULING_FAILURE) {
    // Here we cannot be certain if the scheduler started processing the event or not, so we can only try to drain
    // the scheduler and continue. Trying to create a debug stream result for this event and clear the event slot may
    // lead to further problems if the event is being processed
    ATH_MSG_ERROR("Failure occurred with OnlineErrorCode=" << errorCode
      << ". Cannot determine if the event processing started or not. Current local event number is "
      << eventContext.evt() << ", slot " << eventContext.slot()
      << ". All slots of this HltEventLoopMgr instance will be drained before proceeding.");
    return drainAllAndProceed();
  }

  //----------------------------------------------------------------------------
  // Create an HLT result for the failed event (copy one if it exists)
  //----------------------------------------------------------------------------
  auto hltResultRH = SG::makeHandle(m_hltResultRHKey,eventContext);
  if (!hltResultRH.isValid()) {
    // Try to build a result if not available
    m_hltResultMaker->makeResult(eventContext).ignore();
  }

  std::unique_ptr<HLT::HLTResultMT> hltResultPtr;
  if (!hltResultRH.isValid())
    hltResultPtr = std::make_unique<HLT::HLTResultMT>();
  else
    hltResultPtr = std::make_unique<HLT::HLTResultMT>(*hltResultRH);

  SG::WriteHandleKey<HLT::HLTResultMT> hltResultWHK(m_hltResultRHKey.key()+"_FailedEvent");
  ATH_CHECK(hltResultWHK.initialize());
  auto hltResultWH = SG::makeHandle(hltResultWHK,eventContext);
  if (hltResultWH.record(std::move(hltResultPtr)).isFailure()) {
    ATH_MSG_ERROR("Failed to record the HLT Result in event store while handling a failed event."
      << " Likely an issue with the store. OnlineErrorCode=" << errorCode
      << ", local event number " << eventContext.evt() << ", slot " << eventContext.slot()
      << ". All slots of this HltEventLoopMgr instance will be drained before proceeding.");
    return drainAllAndProceed();
  }

  //----------------------------------------------------------------------------
  // Set error code and make sure the debug stream tag is added
  //----------------------------------------------------------------------------
  hltResultWH->addErrorCode(errorCode);
  switch (errorCode) {
    case HLT::OnlineErrorCode::PROCESSING_FAILURE:
      ATH_CHECK(hltResultWH->addStreamTag({m_algErrorDebugStreamName.value(), eformat::DEBUG_TAG, true}));
      break;
    case HLT::OnlineErrorCode::TIMEOUT:
      ATH_CHECK(hltResultWH->addStreamTag({m_timeoutDebugStreamName.value(), eformat::DEBUG_TAG, true}));
      break;
    case HLT::OnlineErrorCode::RESULT_TRUNCATION:
      ATH_CHECK(hltResultWH->addStreamTag({m_truncationDebugStreamName.value(), eformat::DEBUG_TAG, true}));
      break;
    default:
      ATH_CHECK(hltResultWH->addStreamTag({m_fwkErrorDebugStreamName.value(), eformat::DEBUG_TAG, true}));
      break;
  }

  //----------------------------------------------------------------------------
  // Monitor event processing time for the failed (force-accepted) event
  //----------------------------------------------------------------------------
  auto eventTime = std::chrono::steady_clock::now() - m_eventTimerStartPoint[eventContext.slot()];
  int64_t eventTimeMillisec = std::chrono::duration_cast<std::chrono::milliseconds>(eventTime).count();
  auto monTimeAny = Monitored::Scalar<int64_t>("TotalTime", eventTimeMillisec);
  auto monTimeAcc = Monitored::Scalar<int64_t>("TotalTimeAccepted", eventTimeMillisec);
  auto mon = Monitored::Group(m_monTool, monTimeAny, monTimeAcc);

  //----------------------------------------------------------------------------
  // Try to build and send the output
  //----------------------------------------------------------------------------
  if (m_outputCnvSvc->connectOutput("").isFailure()) {
    ATH_MSG_ERROR("The output conversion service failed in connectOutput() while handling a failed event."
      << " No HLT result can be recorded for this event. OnlineErrorCode=" << errorCode
      << ", local event number " << eventContext.evt() << ", slot " << eventContext.slot()
      << ". All slots of this HltEventLoopMgr instance will be drained before proceeding.");
    return drainAllAndProceed();
  }

  DataObject* hltResultDO = m_evtStore->accessData(hltResultWH.clid(),hltResultWH.key());
  if (!hltResultDO) {
    ATH_MSG_ERROR("Failed to retrieve DataObject for the HLT result object while handling a failed event."
      << " No HLT result can be recorded for this event. OnlineErrorCode=" << errorCode
      << ", local event number " << eventContext.evt() << ", slot " << eventContext.slot()
      << ". All slots of this HltEventLoopMgr instance will be drained before proceeding.");
    return drainAllAndProceed();
  }

  IOpaqueAddress* addr = nullptr;
  if (m_outputCnvSvc->createRep(hltResultDO,addr).isFailure() || !addr) {
    ATH_MSG_ERROR("Conversion of HLT result object to the output format failed while handling a failed event."
      << " No HLT result can be recorded for this event. OnlineErrorCode=" << errorCode
      << ", local event number " << eventContext.evt() << ", slot " << eventContext.slot()
      << ". All slots of this HltEventLoopMgr instance will be drained before proceeding.");
    delete addr;
    return drainAllAndProceed();
  }

  if (m_outputCnvSvc->commitOutput("",true).isFailure()) {
    ATH_MSG_ERROR("The output conversion service failed in commitOutput() while handling a failed event."
      << " No HLT result can be recorded for this event. OnlineErrorCode=" << errorCode
      << ", local event number " << eventContext.evt() << ", slot " << eventContext.slot()
      << ". All slots of this HltEventLoopMgr instance will be drained before proceeding.");
    delete addr;
    return drainAllAndProceed();
  }

  // The output has been sent out, the ByteStreamAddress can be deleted
  delete addr;

  //----------------------------------------------------------------------------
  // Clear the event data slot
  //----------------------------------------------------------------------------
  // Need to copy the event context because it's managed by the event store and clearWBSlot deletes it
  EventContext eventContextCopy = eventContext;
  if (clearWBSlot(eventContext.slot()).isFailure())
    return failedEvent(HLT::OnlineErrorCode::AFTER_RESULT_SENT,eventContextCopy);

  //----------------------------------------------------------------------------
  // Finish handling the failed event
  //----------------------------------------------------------------------------

  // Unless this is a timeout, truncation or processing (i.e. algorithm) failure, increment the number of framework failures
  if (errorCode != HLT::OnlineErrorCode::TIMEOUT
      && errorCode != HLT::OnlineErrorCode::RESULT_TRUNCATION
      && errorCode != HLT::OnlineErrorCode::PROCESSING_FAILURE) {
    if ( m_maxFrameworkErrors.value()>=0 && ((++m_nFrameworkErrors)>m_maxFrameworkErrors.value()) ) {
      ATH_MSG_ERROR("Failure with OnlineErrorCode=" << errorCode
        << " was successfully handled, but the number of tolerable framework errors for this HltEventLoopMgr instance,"
        << " which is " << m_maxFrameworkErrors.value() << ", was exceeded. Current local event number is "
        << eventContextCopy.evt() << ", slot " << eventContextCopy.slot()
        << ". All slots of this HltEventLoopMgr instance will be drained and the loop will exit.");
      ATH_CHECK(drainAllSlots());
      return StatusCode::FAILURE;
    }
  }

  // Even if handling the failed event succeeded, print an error message with failed event details
  ATH_MSG_ERROR("Failed event with OnlineErrorCode=" << errorCode
    << " Current local event number is " << eventContextCopy.evt() << ", slot " << eventContextCopy.slot());

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS; // continue the event loop
}

// =============================================================================
void HltEventLoopMgr::runEventTimer()
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  std::unique_lock<std::mutex> lock(m_timeoutMutex);
  while (m_runEventTimer) {
    m_timeoutCond.wait_for(lock,std::chrono::seconds(1));
    auto now=std::chrono::steady_clock::now();
    for (size_t i=0; i<m_eventTimerStartPoint.size(); ++i) {
      // iterate over all slots and check for timeout
      if (!m_isSlotProcessing.at(i)) continue;
      if (now > m_eventTimerStartPoint.at(i) + m_softTimeoutValue) {
        EventContext ctx(0,i); // we only need the slot number for Athena::Timeout instance
        // don't duplicate the actions if the timeout was already reached
        if (!Athena::Timeout::instance(ctx).reached()) {
          auto procTime = now - m_eventTimerStartPoint.at(i);
          auto procTimeMillisec = std::chrono::duration_cast<std::chrono::milliseconds>(procTime);
          ATH_MSG_ERROR("Soft timeout in slot " << i << ". Processing time = " << procTimeMillisec.count() << " ms");
          setTimeout(Athena::Timeout::instance(ctx));
        }
      }
    }
  }
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
}

// =============================================================================
std::unordered_map<std::string_view,StatusCode> HltEventLoopMgr::algExecErrors(const EventContext& eventContext) const {
  std::unordered_map<std::string_view,StatusCode> algErrors;
  for (const auto& [key, state] : m_aess->algExecStates(eventContext)) {
    if (!state.execStatus().isSuccess()) {
      ATH_MSG_DEBUG("Algorithm " << key << " returned StatusCode " << state.execStatus().message()
                    << " in event " << eventContext.eventID());
      algErrors[key.str()] = state.execStatus();
      auto monErrorAlgName = Monitored::Scalar<std::string>("ErrorAlgName", key.str());
      auto monErrorCode = Monitored::Scalar<std::string>("ErrorCode", state.execStatus().message());
      auto mon = Monitored::Group(m_monTool, monErrorAlgName, monErrorCode);
    }
  }
  return algErrors;
}

// =============================================================================
/**
 * @brief Retrieves finished events from the scheduler, processes their output and cleans up the slots
 * @return SUCCESS if at least one event was finished, SCHEDULER_EMPTY if there are no events being processed,
 * RECOVERABLE if there was an error which was handled correctly, FAILURE if the error should break the event loop
 **/
HltEventLoopMgr::DrainSchedulerStatusCode HltEventLoopMgr::drainScheduler()
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  //----------------------------------------------------------------------------
  // Pop events from the Scheduler
  //----------------------------------------------------------------------------
  std::vector<EventContext*> finishedEvtContexts;
  EventContext* finishedEvtContext(nullptr);

  ATH_MSG_DEBUG("Waiting for a finished event from the Scheduler");
  if (m_schedulerSvc->popFinishedEvent(finishedEvtContext).isSuccess()) {
    // An event was popped from the scheduler
    ATH_MSG_DEBUG("Scheduler returned a finished event: " << finishedEvtContext);
    finishedEvtContexts.push_back(finishedEvtContext);
  }
  else {
    // Failure means there are no more events left in the scheduler
    ATH_MSG_DEBUG("Scheduler empty");
    return DrainSchedulerStatusCode::SCHEDULER_EMPTY;
  }

  // Try to pop other events
  while (m_schedulerSvc->tryPopFinishedEvent(finishedEvtContext).isSuccess()) {
    ATH_MSG_DEBUG("Scheduler returned a finished event: " << finishedEvtContext);
    finishedEvtContexts.push_back(finishedEvtContext);
  }

  //----------------------------------------------------------------------------
  // Post-process the finished events
  //----------------------------------------------------------------------------
  bool atLeastOneFailed = false;
  for (EventContext* thisFinishedEvtContext : finishedEvtContexts) {
    StatusCode sc = StatusCode::SUCCESS;
    auto markFailed = [&sc,&atLeastOneFailed](){sc = StatusCode::FAILURE; atLeastOneFailed = true;};

    //--------------------------------------------------------------------------
    // Basic checks, select slot, retrieve event info
    //--------------------------------------------------------------------------
    // Check if the EventContext object exists
    if (!thisFinishedEvtContext) markFailed();
    HLT_DRAINSCHED_CHECK(sc, "Detected nullptr EventContext while finalising a processed event",
                         HLT::OnlineErrorCode::CANNOT_ACCESS_SLOT, thisFinishedEvtContext);

    // Reset free slot timer for monitoring
    m_freeSlotStartPoint[thisFinishedEvtContext->slot()] = std::chrono::steady_clock::now();

    // Set ThreadLocalContext to the currently processed finished context
    Gaudi::Hive::setCurrentContext(thisFinishedEvtContext);

    // Check the event processing status
    if (m_aess->eventStatus(*thisFinishedEvtContext) != EventStatus::Success) {
      markFailed();
      auto algErrors = algExecErrors(*thisFinishedEvtContext);
      HLT::OnlineErrorCode errCode = isTimedOut(algErrors) ?
                                     HLT::OnlineErrorCode::TIMEOUT : HLT::OnlineErrorCode::PROCESSING_FAILURE;
      HLT_DRAINSCHED_CHECK(sc, "Processing event with context " << *thisFinishedEvtContext
                           << " failed with status " << m_aess->eventStatus(*thisFinishedEvtContext),
                           errCode, thisFinishedEvtContext);
    }

    // Select the whiteboard slot
    sc = m_whiteboard->selectStore(thisFinishedEvtContext->slot());
    if (sc.isFailure()) atLeastOneFailed = true;
    HLT_DRAINSCHED_CHECK(sc, "Failed to select event store slot " << thisFinishedEvtContext->slot(),
                         HLT::OnlineErrorCode::CANNOT_ACCESS_SLOT, thisFinishedEvtContext);

    // Fire EndProcessing incident - some services may depend on this
    m_incidentSvc->fireIncident(Incident(name(), IncidentType::EndProcessing, *thisFinishedEvtContext));

    //--------------------------------------------------------------------------
    // HLT output handling
    //--------------------------------------------------------------------------
    // Call the result builder to record HLTResultMT in SG
    sc = m_hltResultMaker->makeResult(*thisFinishedEvtContext);
    if (sc.isFailure()) atLeastOneFailed = true;
    HLT_DRAINSCHED_CHECK(sc, "Failed to create the HLT result object",
                         HLT::OnlineErrorCode::NO_HLT_RESULT, thisFinishedEvtContext);

    // Connect output (create the output container) - the argument is currently not used
    sc = m_outputCnvSvc->connectOutput("");
    if (sc.isFailure()) atLeastOneFailed = true;
    HLT_DRAINSCHED_CHECK(sc, "Conversion service failed to connectOutput",
                         HLT::OnlineErrorCode::OUTPUT_BUILD_FAILURE, thisFinishedEvtContext);

    // Retrieve the HLT result and the corresponding DataObject
    auto hltResult = SG::makeHandle(m_hltResultRHKey,*thisFinishedEvtContext);
    if (!hltResult.isValid()) markFailed();
    HLT_DRAINSCHED_CHECK(sc, "Failed to retrieve the HLT result",
                         HLT::OnlineErrorCode::NO_HLT_RESULT, thisFinishedEvtContext);

    DataObject* hltResultDO = m_evtStore->accessData(hltResult.clid(),hltResult.key());
    if (!hltResultDO) markFailed();
    HLT_DRAINSCHED_CHECK(sc, "Failed to retrieve the HLTResult DataObject",
                         HLT::OnlineErrorCode::NO_HLT_RESULT, thisFinishedEvtContext);

    // Check for result truncation
    if (!hltResult->getTruncatedModuleIds().empty()) markFailed();
    HLT_DRAINSCHED_CHECK(sc, "HLT result truncation",
                         HLT::OnlineErrorCode::RESULT_TRUNCATION, thisFinishedEvtContext);

    // Convert the HLT result to the output data format
    IOpaqueAddress* addr = nullptr;
    sc = m_outputCnvSvc->createRep(hltResultDO,addr);
    if (sc.isFailure()) {
      delete addr;
      atLeastOneFailed = true;
    }
    HLT_DRAINSCHED_CHECK(sc, "Conversion service failed to convert HLTResult",
                         HLT::OnlineErrorCode::OUTPUT_BUILD_FAILURE, thisFinishedEvtContext);

    // Retrieve and convert the L1 result to the output data format
    IOpaqueAddress* l1addr = nullptr;
    if (m_rewriteLVL1) {
      auto l1TriggerResult = SG::makeHandle(m_l1TriggerResultRHKey, *thisFinishedEvtContext);
      if (!l1TriggerResult.isValid()) markFailed();
      HLT_DRAINSCHED_CHECK(sc, "Failed to retrieve the L1 Trigger Result for RewriteLVL1",
                          HLT::OnlineErrorCode::OUTPUT_BUILD_FAILURE, thisFinishedEvtContext);

      DataObject* l1TriggerResultDO = m_evtStore->accessData(l1TriggerResult.clid(),l1TriggerResult.key());
      if (!l1TriggerResultDO) markFailed();
      HLT_DRAINSCHED_CHECK(sc, "Failed to retrieve the L1 Trigger Result DataObject for RewriteLVL1",
                          HLT::OnlineErrorCode::OUTPUT_BUILD_FAILURE, thisFinishedEvtContext);

      sc = m_outputCnvSvc->createRep(l1TriggerResultDO,l1addr);
      if (sc.isFailure()) {
        delete l1addr;
        atLeastOneFailed = true;
      }
      HLT_DRAINSCHED_CHECK(sc, "Conversion service failed to convert L1 Trigger Result for RewriteLVL1",
                          HLT::OnlineErrorCode::OUTPUT_BUILD_FAILURE, thisFinishedEvtContext);
    }

    // Save event processing time before sending output
    bool eventAccepted = !hltResult->getStreamTags().empty();
    auto eventTime = std::chrono::steady_clock::now() - m_eventTimerStartPoint[thisFinishedEvtContext->slot()];
    int64_t eventTimeMillisec = std::chrono::duration_cast<std::chrono::milliseconds>(eventTime).count();

    // Commit output (write/send the output data) - the arguments are currently not used
    sc = m_outputCnvSvc->commitOutput("",true);
    if (sc.isFailure()) {
      delete addr;
      atLeastOneFailed = true;
    }
    HLT_DRAINSCHED_CHECK(sc, "Conversion service failed to commitOutput",
                         HLT::OnlineErrorCode::OUTPUT_SEND_FAILURE, thisFinishedEvtContext);

    // The output has been sent out, the ByteStreamAddress can be deleted
    delete addr;
    delete l1addr;

    //--------------------------------------------------------------------------
    // Flag idle slot to the timeout thread and reset the timer
    //--------------------------------------------------------------------------
    {
      std::unique_lock<std::mutex> lock(m_timeoutMutex);
      m_eventTimerStartPoint[thisFinishedEvtContext->slot()] = std::chrono::steady_clock::now();
      m_isSlotProcessing[thisFinishedEvtContext->slot()] = false;
      resetTimeout(Athena::Timeout::instance(*thisFinishedEvtContext));
    }
    m_timeoutCond.notify_all();

    //--------------------------------------------------------------------------
    // Clear the slot
    //--------------------------------------------------------------------------
    ATH_MSG_DEBUG("Clearing slot " << thisFinishedEvtContext->slot()
                  << " (event " << thisFinishedEvtContext->evt() << ") of the whiteboard");

    HLT_DRAINSCHED_CHECK(clearWBSlot(thisFinishedEvtContext->slot()),
                         "Whiteboard slot " << thisFinishedEvtContext->slot() << " could not be properly cleared",
                         HLT::OnlineErrorCode::AFTER_RESULT_SENT, thisFinishedEvtContext);

    ATH_MSG_DEBUG("Finished processing " << (eventAccepted ? "accepted" : "rejected")
                  << " event with context " << *thisFinishedEvtContext
                  << " which took " << eventTimeMillisec << " ms");

    // Fill the time monitoring histograms
    auto monTimeAny = Monitored::Scalar<int64_t>("TotalTime", eventTimeMillisec);
    auto monTimeAcc = Monitored::Scalar<int64_t>(eventAccepted ? "TotalTimeAccepted" : "TotalTimeRejected", eventTimeMillisec);
    auto mon = Monitored::Group(m_monTool, monTimeAny, monTimeAcc);

    // Set ThreadLocalContext to an invalid context as we entering a context-less environment
    Gaudi::Hive::setCurrentContext( EventContext() );

    // Delete the EventContext which was created when calling executeEvent( EventContext(*eventContext) )
    delete thisFinishedEvtContext;
  }

  // Set ThreadLocalContext to an invalid context again in case of failure breaking the loop above
  if (atLeastOneFailed) Gaudi::Hive::setCurrentContext( EventContext() );

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return ( atLeastOneFailed ? DrainSchedulerStatusCode::RECOVERABLE : DrainSchedulerStatusCode::SUCCESS );
}

// =============================================================================
StatusCode HltEventLoopMgr::clearWBSlot(size_t evtSlot) const
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  StatusCode sc = m_whiteboard->clearStore(evtSlot);
  if( !sc.isSuccess() )  {
    ATH_MSG_WARNING("Clear of event data store failed");
  }
  ATH_MSG_VERBOSE("end of " << __FUNCTION__ << ", returning m_whiteboard->freeStore(evtSlot=" << evtSlot << ")");
  return m_whiteboard->freeStore(evtSlot);
}

// =============================================================================
StatusCode HltEventLoopMgr::recoverFromStarvation()
{
  auto freeSlotsScheduler = m_schedulerSvc->freeSlots();
  auto freeSlotsWhiteboard = m_whiteboard->freeSlots();
  if (freeSlotsScheduler == freeSlotsWhiteboard) {
    ATH_MSG_WARNING("Starvation recovery was requested but not needed, so it was not attempted. "
                    << "This method should not have been called.");
    return StatusCode::SUCCESS;
  }

  if (drainAllSlots().isFailure()) {
    ATH_MSG_ERROR("Starvation recovery failed. Scheduler saw " << freeSlotsScheduler << " free slots,"
      << " whereas whiteboard saw " << freeSlotsWhiteboard << " free slots. Total number of slots is "
      << m_isSlotProcessing.size() << ". Now scheduler sees " << m_schedulerSvc->freeSlots()
      << " free slots, whereas whiteboard sees " << m_whiteboard->freeSlots() << " free slots");
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_WARNING("Starvation detected, but successfully recovered. Scheduler saw " << freeSlotsScheduler
      << " free slots, whereas whiteboard saw " << freeSlotsWhiteboard << " free slots. All slots have been cleared,"
      << " now scheduler sees " << m_schedulerSvc->freeSlots() << " free slots and whiteboard sees "
      << m_whiteboard->freeSlots() << " free slots");
    return StatusCode::SUCCESS;
  }
}

// =============================================================================
StatusCode HltEventLoopMgr::drainAllSlots()
{
  size_t nslots = m_isSlotProcessing.size(); // size is fixed in hltUpdateAfterFork after configuring scheduler

  // First try to drain the scheduler to free all processing slots
  DrainSchedulerStatusCode drainResult = DrainSchedulerStatusCode::SUCCESS;
  do {
    drainResult = drainScheduler();
    // fail on recoverable, because it means an error while handling an error
    // (drainAllSlots is a "clean up on failure" method)
    if (drainResult == DrainSchedulerStatusCode::FAILURE || drainResult == DrainSchedulerStatusCode::RECOVERABLE) {
      ATH_MSG_ERROR("Failed to drain the scheduler");
      return StatusCode::FAILURE;
    }
  } while (drainResult != DrainSchedulerStatusCode::SCHEDULER_EMPTY); // while there were still events to finish

  // Now try to clear all event data slots (should have no effect if done already)
  for (size_t islot=0; islot<nslots; ++islot) {
    if (clearWBSlot(islot).isFailure()) {
      ATH_MSG_ERROR("Failed to clear whiteboard slot " << islot);
      return StatusCode::FAILURE;
    }
  }

  // Check if the cleanup succeeded
  if (m_schedulerSvc->freeSlots() == nslots && m_whiteboard->freeSlots() == nslots) {
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}
