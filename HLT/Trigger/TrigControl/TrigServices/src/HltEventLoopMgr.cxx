/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "HltEventLoopMgr.h"
#include "TrigCOOLUpdateHelper.h"
#include "TrigKernel/HltExceptions.h"
#include "TrigSORFromPtreeHelper.h"
#include "TrigSteeringEvent/HLTResultMT.h"

// Athena includes
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/ByteStreamMetadata.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigSteeringEvent/HLTExtraData.h"

// Gaudi includes
#include "GaudiKernel/EventIDBase.h"
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
#include "GaudiKernel/ITHistSvc.h"
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

// =============================================================================
// Helper macros, typedefs and constants
// =============================================================================
// Base macro for handling failed events in a loop, see below for specific variants
#define HLT_LOOP_CHECK(scexpr,errmsg,errcode,evctx,retonfail) \
  {                                               \
    StatusCode sccopy = scexpr;                   \
    if (sccopy.isFailure()) {                     \
      ATH_REPORT_ERROR(sccopy) << errmsg;         \
      if (failedEvent(errcode,evctx).isFailure()) \
        return retonfail;                         \
      else                                        \
        continue;                                 \
    }                                             \
  }
// Handles failed event in the event loop, continues the loop if the failure is recoverable,
// otherwise returns StatusCode::FAILURE to break the loop
#define HLT_EVTLOOP_CHECK(scexpr,errmsg,errcode,evctx) \
  HLT_LOOP_CHECK(scexpr,errmsg,errcode,evctx,StatusCode::FAILURE)
// Same as above but returns DrainSchedulerStatusCode::FAILURE to break the loop
#define HLT_DRAINSCHED_CHECK(scexpr,errmsg,errcode,evctx) \
  HLT_LOOP_CHECK(scexpr,errmsg,errcode,evctx,DrainSchedulerStatusCode::FAILURE)
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
  m_robDataProviderSvc("ROBDataProviderSvc", name),
  m_THistSvc("THistSvc", name),
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

  // read DataFlow configuration properties
  updateDFProps();

  // JobOptions type
  SmartIF<IProperty> propMgr = serviceLocator()->service<IProperty>("ApplicationMgr");
  if (propMgr.isValid()) {
    if ( m_topAlgNames.value().empty() ) {
      if (setProperty(propMgr->getProperty("TopAlg")).isFailure()) {
        ATH_REPORT_MESSAGE(MSG::WARNING) << "Could not set the TopAlg property from ApplicationMgr";
      }
    }
  }
  else {
    ATH_REPORT_MESSAGE(MSG::WARNING) << "Error retrieving IProperty interface of ApplicationMgr";
  }

  ATH_CHECK( m_jobOptionsSvc.retrieve() );

  // print properties
  ATH_MSG_INFO(" ---> ApplicationName         = " << m_applicationName);
  ATH_MSG_INFO(" ---> HardTimeout             = " << m_hardTimeout.value());
  ATH_MSG_INFO(" ---> SoftTimeoutFraction     = " << m_softTimeoutFraction.value());
  ATH_MSG_INFO(" ---> MaxFrameworkErrors      = " << m_maxFrameworkErrors.value());
  ATH_MSG_INFO(" ---> FwkErrorDebugStreamName = " << m_fwkErrorDebugStreamName.value());
  ATH_MSG_INFO(" ---> AlgErrorDebugStreamName = " << m_algErrorDebugStreamName.value());
  ATH_MSG_INFO(" ---> EventContextWHKey       = " << m_eventContextWHKey.key());
  ATH_MSG_INFO(" ---> EventInfoRHKey          = " << m_eventInfoRHKey.key());

  const Gaudi::Details::PropertyBase* prop = m_jobOptionsSvc->getClientProperty("EventDataSvc","NSlots");
  if (prop)
    ATH_MSG_INFO(" ---> NumConcurrentEvents     = " << prop->toString());
  else
    ATH_REPORT_MESSAGE(MSG::WARNING) << "Failed to retrieve the job property EventDataSvc.NSlots";
  prop = m_jobOptionsSvc->getClientProperty("AvalancheSchedulerSvc","ThreadPoolSize");
  if (prop)
    ATH_MSG_INFO(" ---> NumThreads              = " << prop->toString());
  else
    ATH_REPORT_MESSAGE(MSG::WARNING) << "Failed to retrieve the job property AvalancheSchedulerSvc.ThreadPoolSize";

  //----------------------------------------------------------------------------
  // Create and initialise the top level algorithms
  //----------------------------------------------------------------------------
  SmartIF<IAlgManager> algMgr = serviceLocator()->as<IAlgManager>();
  if (!algMgr.isValid()) {
    ATH_MSG_FATAL("Failed to retrieve AlgManager - cannot initialise top level algorithms");
    return StatusCode::FAILURE;
  }
  std::vector<SmartIF<IAlgorithm>> topAlgList;
  topAlgList.reserve(m_topAlgNames.value().size());
  for (const auto& it : m_topAlgNames.value()) {
    Gaudi::Utils::TypeNameString item{it};
    std::string item_name = item.name();
    SmartIF<IAlgorithm> alg = algMgr->algorithm(item_name, /*createIf=*/ false);
    if (alg.isValid()) {
      ATH_MSG_DEBUG("Top Algorithm " << item_name << " already exists");
    }
    else {
      ATH_MSG_DEBUG("Creating Top Algorithm " << item.type() << " with name " << item_name);
      IAlgorithm* ialg = nullptr;
      ATH_CHECK(algMgr->createAlgorithm(item.type(), item_name, ialg));
      alg = ialg; // manage reference counting
    }
    m_topAlgList.push_back(alg);
  }

  for (auto& ita : m_topAlgList) {
    ATH_CHECK(ita->sysInitialize());
  }

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
  ATH_CHECK(m_robDataProviderSvc.retrieve());
  ATH_CHECK(m_THistSvc.retrieve());
  ATH_CHECK(m_evtSelector.retrieve());
  ATH_CHECK(m_evtSelector->createContext(m_evtSelContext)); // create an EvtSelectorContext
  ATH_CHECK(m_outputCnvSvc.retrieve());
  ATH_CHECK(m_ioCompMgr.retrieve());

  //----------------------------------------------------------------------------
  // Initialise tools
  //----------------------------------------------------------------------------
  // COOL helper
  ATH_CHECK(m_coolHelper.retrieve());
  // HLT result builder
  ATH_CHECK(m_hltResultMaker.retrieve());

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

  //----------------------------------------------------------------------------
  // Setup the HLT ROB Data Provider Service when configured
  //----------------------------------------------------------------------------
  if ( &*m_robDataProviderSvc ) {
    m_hltROBDataProviderSvc = SmartIF<ITrigROBDataProviderSvc>( &*m_robDataProviderSvc );
    if (m_hltROBDataProviderSvc.isValid())
      ATH_MSG_INFO("A ROBDataProviderSvc implementing the HLT interface ITrigROBDataProviderSvc was found");
    else
      ATH_MSG_INFO("No ROBDataProviderSvc implementing the HLT interface ITrigROBDataProviderSvc was found");
  }

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}



// =============================================================================
// Reimplementation of AthService::stop (IStateful interface)
// =============================================================================
StatusCode HltEventLoopMgr::stop()
{
  // temporary: endRun will eventually be deprecated
  for (auto& ita : m_topAlgList) ATH_CHECK(ita->sysEndRun());

  // Stop top level algorithms
  for (auto& ita : m_topAlgList) ATH_CHECK(ita->sysStop());

  return AthService::stop();
}

// =============================================================================
// Reimplementation of AthService::finalize (IStateful interface)
// =============================================================================
StatusCode HltEventLoopMgr::finalize()
{
  ATH_MSG_INFO(" ---> HltEventLoopMgr/" << name() << " finalize ");
  // Usually (but not necessarily) corresponds to the number of processed events +1
  ATH_MSG_INFO("Total number of EventContext objects created " << m_localEventNumber);

  // Need to release now - automatic release in destructor is too late since services are already gone
  m_hltROBDataProviderSvc.reset();

  // Finalise top level algorithms
  for (auto& ita : m_topAlgList) {
    if (ita->sysFinalize().isFailure())
      ATH_REPORT_MESSAGE(MSG::WARNING) << "Finalisation of algorithm " << ita->name() << " failed";
  }
  // Release top level algorithms
  SmartIF<IAlgManager> algMgr = serviceLocator()->as<IAlgManager>();
  if (!algMgr.isValid()) {
    ATH_REPORT_MESSAGE(MSG::WARNING) << "Failed to retrieve AlgManager - cannot finalise top level algorithms";
  }
  else {
    for (auto& ita : m_topAlgList) {
      if (algMgr->removeAlgorithm(ita).isFailure())
        ATH_REPORT_MESSAGE(MSG::WARNING) << "Problems removing Algorithm " << ita->name();
    }
  }
  m_topAlgList.clear();

  // Release all handles
  auto releaseAndCheck = [&](auto& handle, std::string handleType) {
    if (handle.release().isFailure())
      ATH_MSG_WARNING("finalize(): Failed to release " << handleType << " " << handle.typeAndName());
  };
  auto releaseService = [&](auto&&... args) { (releaseAndCheck(args,"service"), ...); };
  auto releaseTool = [&](auto&&... args) { (releaseAndCheck(args,"tool"), ...); };
  auto releaseSmartIF = [](auto&&... args) { (args.reset(), ...); };

  releaseService(m_incidentSvc,
                 m_robDataProviderSvc,
                 m_evtStore,
                 m_detectorStore,
                 m_inputMetaDataStore,
                 m_THistSvc,
                 m_evtSelector,
                 m_outputCnvSvc);

  releaseTool(m_coolHelper,
              m_hltResultMaker);

  releaseSmartIF(m_whiteboard,
                 m_algResourcePool,
                 m_aess,
                 m_schedulerSvc,
                 m_hltROBDataProviderSvc);

  return StatusCode::SUCCESS;
}

// =============================================================================
// Reimplementation of AthService::reinitalize (IStateful interface)
// =============================================================================
StatusCode HltEventLoopMgr::reinitialize()
{
  ATH_CHECK(AthService::reinitialize());

  // reinitialise top level algorithms
  for (auto& ita : m_topAlgList) {
    ATH_CHECK(ita->sysReinitialize());
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
// Reimplementation of AthService::restart (IStateful interface)
// =============================================================================
StatusCode HltEventLoopMgr::restart()
{
  ATH_CHECK(AthService::restart());

  // restart top level algorithms
  for (auto& ita : m_topAlgList) {
    m_aess->resetErrorCount(ita);
    ATH_CHECK(ita->sysRestart());
  }

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

    // do the necessary resets
    ATH_CHECK(clearTemporaryStores());

    // update SOR in det store
    ATH_CHECK( processRunParams(pt) );

    auto& soral = getSorAttrList();

    updateInternal(soral);       // update internally kept info
    updateMetadataStore(soral);  // update metadata store

    /* Old code: kept for reference for the moment
    const EventInfo * evinfo;
    if(updMagField(pt).isFailure() ||     // update mag field when appropriate
       updHLTConfigSvc().isFailure() ||   // update config svc when appropriate
       prepXAODEventInfo().isFailure() || // update xAOD event data in SG
       !(evinfo = prepEventInfo()))       // update old event data in SG
      return StatusCode::FAILURE;

    bookAllHistograms();
    */

    // start top level algorithms
    for (auto& ita : m_topAlgList) {
      ATH_CHECK(ita->sysStart());
    }

    m_incidentSvc->fireIncident(Incident(name(), IncidentType::BeginRun, m_currentRunCtx));

    for (auto& ita : m_topAlgList) {
      ATH_CHECK(ita->sysBeginRun());   // TEMPORARY: beginRun is deprecated
    }
    // Initialize COOL helper (needs to be done after IOVDbSvc has loaded all folders)
    m_coolHelper->readFolderInfo();

    // close any open files (e.g. THistSvc)
    ATH_CHECK(m_ioCompMgr->io_finalize());

    ATH_MSG_VERBOSE("end of " << __FUNCTION__);
    return StatusCode::SUCCESS;
  }
  catch(const ptree_bad_path & e)
  {
    ATH_REPORT_MESSAGE(MSG::ERROR) << "Bad ptree path: \"" << e.path<ptree::path_type>().dump() << "\" - " << e.what();
  }
  catch(const ptree_bad_data & e)
  {
    ATH_REPORT_MESSAGE(MSG::ERROR) << "Bad ptree data: \"" << e.data<ptree::data_type>() << "\" - " << e.what();
  }
  catch(const std::runtime_error& e)
  {
    ATH_REPORT_MESSAGE(MSG::ERROR) << "Runtime error: " << e.what();
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
    svc->stop();
    svc->start();
    ATH_MSG_DEBUG("Done a stop-start of CoreDumpSvc");
  }
  else {
    ATH_REPORT_MESSAGE(MSG::WARNING) << "Could not retrieve CoreDumpSvc";
  }

  // Make sure output files, i.e. histograms are written to their own directory.
  // Nothing happens if the online TrigMonTHistSvc is used as there are no output files.
  SmartIF<IIoComponent> histsvc = serviceLocator()->service("THistSvc", /*createIf=*/ false).as<IIoComponent>();
  if ( !m_ioCompMgr->io_retrieve(histsvc.get()).empty() ) {
    boost::filesystem::path worker_dir = boost::filesystem::absolute("athenaHLT_workers");
    std::ostringstream oss;
    oss << "athenaHLT-" << std::setfill('0') << std::setw(2) << m_workerId;
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
    m_eventTimerStartPoint.resize(m_whiteboard->getNumberOfStores(), std::chrono::steady_clock::time_point());
    m_isSlotProcessing.resize(m_whiteboard->getNumberOfStores(), false);
  }
  m_timeoutCond.notify_all();

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IEventProcessor::executeRun
// =============================================================================
StatusCode HltEventLoopMgr::executeRun(int maxevt)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  StatusCode sc = nextEvent(maxevt);
  if (sc.isFailure()) {
    ATH_REPORT_ERROR(sc) << "Event loop failed";
    // Extra clean-up may be needed here after the failure
  }

  // Stop the timer thread
  {
    ATH_MSG_DEBUG("Stopping the timeout thread");
    std::unique_lock<std::mutex> lock(m_timeoutMutex);
    m_runEventTimer = false;
  }
  m_timeoutCond.notify_all();
  m_timeoutThread->join();
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
  bool loop_ended = false;
  bool events_available = true; // DataCollector has more events

  while (!loop_ended) {
    ATH_MSG_DEBUG("Free processing slots = " << m_schedulerSvc->freeSlots());
    ATH_MSG_DEBUG("Free event data slots = " << m_whiteboard->freeSlots());
    if (m_schedulerSvc->freeSlots() != m_whiteboard->freeSlots()) {
      // Starvation detected - try to recover and return FAILURE if the recovery fails. This can only happen if there
      // is an unhandled error after popping an event from the scheduler and before clearing the event data slot for
      // this finished event. It's an extra protection in the unlikely case that failedEvent doesn't cover all errors.
      ATH_CHECK(recoverFromStarvation());
    }
    // Normal event processing
    else if (m_schedulerSvc->freeSlots()>0 && events_available) {
      ATH_MSG_DEBUG("Free slots = " << m_schedulerSvc->freeSlots() << ". Reading the next event.");

      //------------------------------------------------------------------------
      // Allocate event slot and create new EventContext
      //------------------------------------------------------------------------
      ++m_localEventNumber;
      // Allocate and select a whiteboard slot
      auto slot = m_whiteboard->allocateStore(m_localEventNumber); // returns npos on failure
      HLT_EVTLOOP_CHECK(((slot==std::string::npos) ? StatusCode(StatusCode::FAILURE) : StatusCode(StatusCode::SUCCESS)),
                        "Failed to allocate slot for a new event",
                        hltonl::PSCErrorCode::BEFORE_NEXT_EVENT, EventContext());
      HLT_EVTLOOP_CHECK(m_whiteboard->selectStore(slot),
                        "Failed to select event store slot number " << slot,
                        hltonl::PSCErrorCode::BEFORE_NEXT_EVENT, EventContext());

      // We can completely avoid using ThreadLocalContext if we store the EventContext in the event store. Any
      // service/tool method which does not allow to pass EventContext as argument, can const-retrieve it from the
      // event store rather than using ThreadLocalContext.

      // We create the EventContext here and link the current store in its extension. Only then we create a WriteHandle
      // for the EventContext using the EventContext itself. The handle will use the linked hiveProxyDict to record
      // the context in the current store.
      auto eventContextPtr = std::make_unique<EventContext>(m_localEventNumber, slot);
      eventContextPtr->setExtension( Atlas::ExtendedEventContext(m_evtStore->hiveProxyDict(),
                                                                 m_currentRunCtx.eventID().run_number()) );
      auto eventContext = SG::makeHandle(m_eventContextWHKey,*eventContextPtr);
      HLT_EVTLOOP_CHECK(eventContext.record(std::move(eventContextPtr)),
                        "Failed to record new EventContext in the event store",
                        hltonl::PSCErrorCode::BEFORE_NEXT_EVENT, EventContext());

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
                        hltonl::PSCErrorCode::BEFORE_NEXT_EVENT, *eventContext);

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
          ATH_REPORT_MESSAGE(MSG::WARNING) << "Failed to clear the whiteboard slot " << eventContext->slot()
                                           << " after NoMoreEvents detected";
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
                        hltonl::PSCErrorCode::CANNOT_RETRIEVE_EVENT, *eventContext);

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
                        hltonl::PSCErrorCode::NO_EVENT_INFO, *eventContext);

      auto eventInfo = SG::makeHandle(m_eventInfoRHKey,*eventContext);
      HLT_EVTLOOP_CHECK((eventInfo.isValid() ? StatusCode(StatusCode::SUCCESS) : StatusCode(StatusCode::FAILURE)),
                        "Failed to retrieve EventInfo",
                        hltonl::PSCErrorCode::NO_EVENT_INFO, *eventContext);

      ATH_MSG_DEBUG("Retrieved event info for the new event " << *eventInfo->event_ID());

      // Set EventID for the EventContext
      eventContext->setEventID(*eventInfo->event_ID());

      // Update thread-local EventContext after setting EventID
      Gaudi::Hive::setCurrentContext(*eventContext);

      //-----------------------------------------------------------------------
      // COOL updates for LB changes
      //-----------------------------------------------------------------------
      // Schedule COOL folder updates based on CTP fragment
      HLT_EVTLOOP_CHECK(m_coolHelper->scheduleFolderUpdates(*eventContext), "Failure reading CTP extra payload",
                        hltonl::PSCErrorCode::COOL_UPDATE, *eventContext);

      // Do an update if this is a new LB
      if ( maxLB < eventContext->eventID().lumi_block() ) {
        maxLB = eventContext->eventID().lumi_block();
        HLT_EVTLOOP_CHECK(m_coolHelper->hltCoolUpdate(*eventContext), "Failure during COOL update",
                          hltonl::PSCErrorCode::COOL_UPDATE, *eventContext);
      }

      //------------------------------------------------------------------------
      // Process the event
      //------------------------------------------------------------------------
      // Need to pass the argument as void* because the IEventProcessor interface doesn't allow explicitly
      // passing EventContext to executeEvent
      HLT_EVTLOOP_CHECK(executeEvent( static_cast<void*>(eventContext.ptr()) ),
                        "Failed to schedule event processing",
                        hltonl::PSCErrorCode::SCHEDULING_FAILURE, *eventContext);

      //------------------------------------------------------------------------
      // Set ThreadLocalContext to an invalid context
      //------------------------------------------------------------------------
      // We have passed the event to the scheduler and we are entering back a context-less environment
      Gaudi::Hive::setCurrentContext( EventContext() );

    } // End of if(free slots && events available)
    else {
      ATH_MSG_DEBUG("No free slots or no more events to process - draining the scheduler");
      DrainSchedulerStatusCode drainResult = drainScheduler();
      if (drainResult==DrainSchedulerStatusCode::FAILURE) {
        ATH_REPORT_MESSAGE(MSG::ERROR) << "Error in draining scheduler, exiting the event loop";
        return StatusCode::FAILURE;
      }
      if (drainResult==DrainSchedulerStatusCode::RECOVERABLE) {
        ATH_REPORT_MESSAGE(MSG::WARNING) << "Recoverable error in draining scheduler, continuing the event loop";
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
// Implementation of IEventProcessor::executeEvent
// =============================================================================
StatusCode HltEventLoopMgr::executeEvent(void* pEvtContext)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  EventContext* eventContext = static_cast<EventContext*>(pEvtContext);
  if (!eventContext) {
    ATH_REPORT_MESSAGE(MSG::ERROR) << "Failed to cast the call parameter to EventContext*";
    return StatusCode::FAILURE;
  }

  resetTimeout(Athena::Timeout::instance(*eventContext));

  // Now add event to the scheduler
  ATH_MSG_DEBUG("Adding event " << eventContext->evt() << ", slot " << eventContext->slot() << " to the scheduler");
  StatusCode addEventStatus = m_schedulerSvc->pushNewEvent(eventContext);

  // If this fails, we need to wait for something to complete
  if (addEventStatus.isFailure()){
    ATH_REPORT_MESSAGE(MSG::ERROR) << "Failed adding event " << eventContext->evt() << ", slot " << eventContext->slot()
                                   << " to the scheduler";
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
  getDFProp( "DF_WorkerId", m_workerId, false );
}

// =============================================================================
StatusCode HltEventLoopMgr::processRunParams(const ptree & pt)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  TrigSORFromPtreeHelper sorhelp(msgSvc(), m_detectorStore, m_sorPath);
  const auto& rparams = pt.get_child("RunParams");

  // Set our "run context" (invalid event/slot)
  m_currentRunCtx.setEventID( sorhelp.eventID(rparams) );
  m_currentRunCtx.setExtension(Atlas::ExtendedEventContext(m_evtStore->hiveProxyDict(),
                                                           m_currentRunCtx.eventID().run_number()));

  // Fill SOR parameters from ptree and inform IOVDbSvc
  ATH_CHECK( sorhelp.fillSOR(rparams, m_currentRunCtx) );

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
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

  auto metadata = new ByteStreamMetadata(
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
    std::vector<std::string>());

  // Record ByteStreamMetadata in MetaData Store
  if(m_inputMetaDataStore->record(metadata,"ByteStreamMetadata").isFailure()) {
    ATH_REPORT_MESSAGE(MSG::WARNING) << "Unable to record MetaData in InputMetaDataStore";
    delete metadata;
  }
  else {
    ATH_MSG_DEBUG("Recorded MetaData in InputMetaDataStore");
  }

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
                      static_cast<EventID::number_type>(dm.second),
                      // next least significant 4 bytes
                      static_cast<EventID::number_type>(dm.second >> 32),
                      // next least significant 4 bytes
                      static_cast<EventID::number_type>(dm.first),
                      // most significant 4 bytes
                      static_cast<EventID::number_type>(dm.first >> 32)
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
    throw std::runtime_error("SOR record should have one and one only attribute list, but it has " + sor->size());
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
StatusCode HltEventLoopMgr::failedEvent(hltonl::PSCErrorCode errorCode, const EventContext& eventContext)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  // Used by MsgSvc (and possibly others but not relevant here)
  Gaudi::Hive::setCurrentContext(eventContext);

  auto drainAllAndProceed = [&]() -> StatusCode {
    ATH_CHECK(drainAllSlots()); // break the event loop on failure
    if ( m_maxFrameworkErrors.value()>=0 && ((++m_nFrameworkErrors)<=m_maxFrameworkErrors.value()) )
      return StatusCode::SUCCESS; // continue the event loop
    else {
      ATH_REPORT_MESSAGE(MSG::ERROR)
        << "The number of tolerable framework errors for this HltEventLoopMgr instance, which is "
        << m_maxFrameworkErrors.value() << ", was exceeded. Exiting the event loop.";
      return StatusCode::FAILURE; // break the event loop
    }
  };

  //----------------------------------------------------------------------------
  // Handle cases where we can only try to clear all slots and continue
  //----------------------------------------------------------------------------

  if (errorCode==hltonl::PSCErrorCode::BEFORE_NEXT_EVENT) {
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "Failure occurred with PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode)
      << " meaning there was a framework error before requesting a new event. No output will be produced and all slots"
      << " of this HltEventLoopMgr instance will be drained before proceeding.";
    return drainAllAndProceed();
  }
  else if (errorCode==hltonl::PSCErrorCode::AFTER_RESULT_SENT) {
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "Failure occurred with PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode)
      << " meaning there was a framework error after HLT result was already sent out."
      << " All slots of this HltEventLoopMgr instance will be drained before proceeding.";
    return drainAllAndProceed();
  }
  else if (errorCode==hltonl::PSCErrorCode::CANNOT_ACCESS_SLOT) {
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "Failed to access the slot for the processed event, cannot produce output. PSCErrorCode="
      << hltonl::PrintPscErrorCode(errorCode)
      << ". All slots of this HltEventLoopMgr instance will be drained before proceeding, then either the loop will"
      << " exit with a failure code or the failed event will reach a hard timeout.";
    return drainAllAndProceed();
  }
  else if (!eventContext.valid()) {
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "Failure occurred with an invalid EventContext. Likely there was a framework error before requesting a new"
      << " event or after sending the result of a finished event. PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode)
      << ". All slots of this HltEventLoopMgr instance will be drained before proceeding.";
    return drainAllAndProceed();
  }

  //----------------------------------------------------------------------------
  // In case of event source failure, drain the scheduler and break the loop
  //----------------------------------------------------------------------------
  if (errorCode==hltonl::PSCErrorCode::CANNOT_RETRIEVE_EVENT) {
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "Failure occurred with PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode)
      << " meaning a new event could not be correctly read. No output will be produced for this event. All slots of"
      << " this HltEventLoopMgr instance will be drained and the loop will exit.";
    ATH_CHECK(drainAllSlots());
    return StatusCode::FAILURE;
  }

  //----------------------------------------------------------------------------
  // Make sure we are using the right store
  //----------------------------------------------------------------------------
  if (m_whiteboard->selectStore(eventContext.slot()).isFailure()) {
    return failedEvent(hltonl::PSCErrorCode::CANNOT_ACCESS_SLOT,eventContext);
  }

  //----------------------------------------------------------------------------
  // Try to retrieve event info for printouts
  //----------------------------------------------------------------------------
  auto eventInfo = SG::makeHandle(m_eventInfoRHKey,eventContext);
  std::ostringstream ss;
  if (eventInfo.isValid()) {
    ss << *eventInfo->event_ID();
  }
  std::string eventInfoString = ss.str();

  if (errorCode==hltonl::PSCErrorCode::SCHEDULING_FAILURE) {
    // Here we cannot be certain if the scheduler started processing the event or not, so we can only try to drain
    // the scheduler and continue. Trying to create a debug stream result for this event and clear the event slot may
    // lead to further problems if the event is being processed
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "Failure occurred with PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode) << ". Cannot determine if the"
      << " event processing started or not. Current local event number is " << eventContext.evt()
      << ", slot " << eventContext.slot() << ". " << eventInfoString
      << " All slots of this HltEventLoopMgr instance will be drained before proceeding.";
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
  hltResultWHK.initialize();
  auto hltResultWH = SG::makeHandle(hltResultWHK,eventContext);
  if (hltResultWH.record(std::move(hltResultPtr)).isFailure()) {
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "Failed to record the HLT Result in event store while handling a failed event. Likely an issue with the store."
      << " PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode) << ", local event number " << eventContext.evt()
      << ", slot " << eventContext.slot() << ". " << eventInfoString
      << " All slots of this HltEventLoopMgr instance will be drained before proceeding.";
    return drainAllAndProceed();
  }

  //----------------------------------------------------------------------------
  // Set error code and make sure the debug stream tag is added
  //----------------------------------------------------------------------------
  hltResultWH->addErrorCode( static_cast<uint32_t>(errorCode) );
  std::string debugStreamName = (errorCode==hltonl::PSCErrorCode::PROCESSING_FAILURE) ?
                                m_algErrorDebugStreamName.value() : m_fwkErrorDebugStreamName.value();
  hltResultWH->addStreamTag({debugStreamName,eformat::DEBUG_TAG,true});

  //----------------------------------------------------------------------------
  // Try to build and send the output
  //----------------------------------------------------------------------------
  if (m_outputCnvSvc->connectOutput("").isFailure()) {
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "The output conversion service failed in connectOutput() while handling a failed event. No HLT result can be"
      << " recorded for this event. PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode) << ", local event number "
      << eventContext.evt() << ", slot " << eventContext.slot() << ". " << eventInfoString
      << " All slots of this HltEventLoopMgr instance will be drained before proceeding.";
    return drainAllAndProceed();
  }

  DataObject* hltResultDO = m_evtStore->accessData(hltResultWH.clid(),hltResultWH.key());
  if (!hltResultDO) {
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "Failed to retrieve DataObject for the HLT result object while handling a failed event. No HLT result"
      << " can be recorded for this event. PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode)
      << ", local event number " << eventContext.evt() << ", slot " << eventContext.slot() << ". " << eventInfoString
      << " All slots of this HltEventLoopMgr instance will be drained before proceeding.";
    return drainAllAndProceed();
  }

  IOpaqueAddress* addr = nullptr;
  if (m_outputCnvSvc->createRep(hltResultDO,addr).isFailure() || !addr) {
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "Conversion of HLT result object to the output format failed while handling a failed event. No HLT result"
      << " can be recorded for this event. PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode)
      << ", local event number " << eventContext.evt() << ", slot " << eventContext.slot() << ". " << eventInfoString
      << " All slots of this HltEventLoopMgr instance will be drained before proceeding.";
    return drainAllAndProceed();
  }

  if (m_outputCnvSvc->commitOutput("",true).isFailure()) {
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "The output conversion service failed in commitOutput() while handling a failed event. No HLT result can be"
      << " recorded for this event. PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode) << ", local event number "
      << eventContext.evt() << ", slot " << eventContext.slot() << ". " << eventInfoString
      << " All slots of this HltEventLoopMgr instance will be drained before proceeding.";
    return drainAllAndProceed();
  }

  //----------------------------------------------------------------------------
  // Clear the event data slot
  //----------------------------------------------------------------------------
  // Need to copy the event context because it's managed by the event store and clearWBSlot deletes it
  EventContext eventContextCopy = eventContext;
  if (clearWBSlot(eventContext.slot()).isFailure())
    return failedEvent(hltonl::PSCErrorCode::AFTER_RESULT_SENT,eventContextCopy);

  //----------------------------------------------------------------------------
  // Finish handling the failed event
  //----------------------------------------------------------------------------

  // Unless this is a processing (i.e. algorithm) failure, increment the number of framework failures
  if (errorCode != hltonl::PSCErrorCode::PROCESSING_FAILURE) {
    if ( (++m_nFrameworkErrors)>m_maxFrameworkErrors.value() ) {
      ATH_REPORT_MESSAGE(MSG::ERROR)
        << "Failure with PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode) << " was successfully handled, but the"
        << " number of tolerable framework errors for this HltEventLoopMgr instance, which is "
        << m_maxFrameworkErrors.value() << ", was exceeded. Current local event number is " << eventContextCopy.evt()
        << ", slot " << eventContextCopy.slot() << ". " << eventInfoString
        << " All slots of this HltEventLoopMgr instance will be drained and the loop will exit.";
      ATH_CHECK(drainAllSlots());
      return StatusCode::FAILURE;
    }
  }

  // Even if handling the failed event succeeded, print an error message with failed event details
  ATH_REPORT_MESSAGE(MSG::ERROR)
    << "Failed event with PSCErrorCode=" << hltonl::PrintPscErrorCode(errorCode) << " Current local event number is "
    << eventContextCopy.evt() << ", slot " << eventContextCopy.slot() << ". " << eventInfoString;

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS; // continue the event loop
}

// =============================================================================
void HltEventLoopMgr::runEventTimer()
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  auto softDuration = std::chrono::milliseconds(m_softTimeoutValue);
  std::unique_lock<std::mutex> lock(m_timeoutMutex);
  while (m_runEventTimer) {
    m_timeoutCond.wait_for(lock,std::chrono::seconds(1));
    auto now=std::chrono::steady_clock::now();
    for (size_t i=0; i<m_eventTimerStartPoint.size(); ++i) {
      // iterate over all slots and check for timeout
      if (!m_isSlotProcessing.at(i)) continue;
      if (now > m_eventTimerStartPoint.at(i) + softDuration) {
        EventContext ctx(0,i); // we only need the slot number for Athena::Timeout instance
        // don't duplicate the actions if the timeout was already reached
        if (!Athena::Timeout::instance(ctx).reached()) {
          auto procTime = now - m_eventTimerStartPoint.at(i);
          auto procTimeMillisec = std::chrono::duration_cast<std::chrono::milliseconds>(procTime);
          ATH_REPORT_MESSAGE(MSG::ERROR) << "Soft timeout in slot " << i << ". Processing time = "
                                         << procTimeMillisec.count() << " ms";
          setTimeout(Athena::Timeout::instance(ctx));
        }
      }
    }
  }
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
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
                         hltonl::PSCErrorCode::CANNOT_ACCESS_SLOT, EventContext());

    // Set ThreadLocalContext to the currently processed finished context
    Gaudi::Hive::setCurrentContext(thisFinishedEvtContext);

    // Check the event processing status
    if (m_aess->eventStatus(*thisFinishedEvtContext) != EventStatus::Success) markFailed();
    HLT_DRAINSCHED_CHECK(sc, "Processing event with context " << *thisFinishedEvtContext
                         << " failed with status " << m_aess->eventStatus(*thisFinishedEvtContext),
                         hltonl::PSCErrorCode::PROCESSING_FAILURE, *thisFinishedEvtContext);

    // Select the whiteboard slot
    sc = m_whiteboard->selectStore(thisFinishedEvtContext->slot());
    if (sc.isFailure()) atLeastOneFailed = true;
    HLT_DRAINSCHED_CHECK(sc, "Failed to select event store slot " << thisFinishedEvtContext->slot(),
                         hltonl::PSCErrorCode::CANNOT_ACCESS_SLOT, *thisFinishedEvtContext);

    // Fire EndProcessing incident - some services may depend on this
    m_incidentSvc->fireIncident(Incident(name(), IncidentType::EndProcessing, *thisFinishedEvtContext));

    //--------------------------------------------------------------------------
    // HLT output handling
    //--------------------------------------------------------------------------
    // Call the result builder to record HLTResultMT in SG
    sc = m_hltResultMaker->makeResult(*thisFinishedEvtContext);
    if (sc.isFailure()) atLeastOneFailed = true;
    HLT_DRAINSCHED_CHECK(sc, "Failed to create the HLT result object",
                         hltonl::PSCErrorCode::NO_HLT_RESULT, *thisFinishedEvtContext);

    // Connect output (create the output container) - the argument is currently not used
    sc = m_outputCnvSvc->connectOutput("");
    if (sc.isFailure()) atLeastOneFailed = true;
    HLT_DRAINSCHED_CHECK(sc, "Conversion service failed to connectOutput",
                         hltonl::PSCErrorCode::OUTPUT_BUILD_FAILURE, *thisFinishedEvtContext);

    // Retrieve the HLT result and the corresponding DataObject
    auto hltResult = SG::makeHandle(m_hltResultRHKey,*thisFinishedEvtContext);
    if (!hltResult.isValid()) markFailed();
    HLT_DRAINSCHED_CHECK(sc, "Failed to retrieve the HLT result",
                         hltonl::PSCErrorCode::NO_HLT_RESULT, *thisFinishedEvtContext);

    DataObject* hltResultDO = m_evtStore->accessData(hltResult.clid(),hltResult.key());
    if (!hltResultDO) markFailed();
    HLT_DRAINSCHED_CHECK(sc, "Failed to retrieve the HLTResult DataObject",
                         hltonl::PSCErrorCode::NO_HLT_RESULT, *thisFinishedEvtContext);

    // Convert the HLT result to the output data format
    IOpaqueAddress* addr = nullptr;
    sc = m_outputCnvSvc->createRep(hltResultDO,addr);
    if (sc.isFailure()) atLeastOneFailed = true;
    HLT_DRAINSCHED_CHECK(sc, "Conversion service failed to convert HLTResult",
                         hltonl::PSCErrorCode::OUTPUT_BUILD_FAILURE, *thisFinishedEvtContext);

    // Commit output (write/send the output data) - the arguments are currently not used
    sc = m_outputCnvSvc->commitOutput("",true);
    if (sc.isFailure()) atLeastOneFailed = true;
    HLT_DRAINSCHED_CHECK(sc, "Conversion service failed to commitOutput",
                         hltonl::PSCErrorCode::OUTPUT_SEND_FAILURE, *thisFinishedEvtContext);

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
                         hltonl::PSCErrorCode::AFTER_RESULT_SENT, EventContext());

    // Cannot print out EventContext anymore here because it was managed by event store and deleted in clearWBSlot
    ATH_MSG_DEBUG("Finished processing event");

    // Set ThreadLocalContext to an invalid context as we entering a context-less environment
    Gaudi::Hive::setCurrentContext( EventContext() );
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
    ATH_REPORT_MESSAGE(MSG::WARNING) << "Clear of event data store failed";
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
    ATH_REPORT_MESSAGE(MSG::WARNING)
      << "Starvation recovery was requested but not needed, so it was not attempted. "
      << "This method should not have been called.";
    return StatusCode::SUCCESS;
  }

  if (drainAllSlots().isFailure()) {
    ATH_REPORT_MESSAGE(MSG::ERROR)
      << "Starvation recovery failed. Scheduler saw " << freeSlotsScheduler << " free slots, whereas whiteboard saw "
      << freeSlotsWhiteboard << " free slots. Total number of slots is " << m_isSlotProcessing.size()
      << ". Now scheduler sees " << m_schedulerSvc->freeSlots() << " free slots, whereas whiteboard sees "
      << m_whiteboard->freeSlots() << " free slots";
    return StatusCode::FAILURE;
  }
  else {
    ATH_REPORT_MESSAGE(MSG::WARNING)
      << "Starvation detected, but successfully recovered. Scheduler saw " << freeSlotsScheduler << " free slots"
      << ", whereas whiteboard saw " << freeSlotsWhiteboard << " free slots. All slots have been cleared, "
      << " now scheduler sees " << m_schedulerSvc->freeSlots() << " free slots and whiteboard sees "
      << m_whiteboard->freeSlots() << " free slots";
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
      ATH_REPORT_MESSAGE(MSG::ERROR) << "Failed to drain the scheduler";
      return StatusCode::FAILURE;
    }
  } while (drainResult != DrainSchedulerStatusCode::SCHEDULER_EMPTY); // while there were still events to finish

  // Now try to clear all event data slots (should have no effect if done already)
  for (size_t islot=0; islot<nslots; ++islot) {
    if (clearWBSlot(islot).isFailure()) {
      ATH_REPORT_MESSAGE(MSG::ERROR) << "Failed to clear whiteboard slot " << islot;
      return StatusCode::FAILURE;
    }
  }

  // Check if the cleanup succeeded
  if (m_schedulerSvc->freeSlots() == nslots && m_whiteboard->freeSlots() == nslots) {
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}
