/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "TrigServices/HltEventLoopMgr.h"
#include "TrigCOOLUpdateHelper.h"
#include "TrigKernel/HltExceptions.h"
#include "TrigSORFromPtreeHelper.h"
#include "TrigSteeringEvent/HLTResult.h"

// Athena includes
#include "AthenaKernel/EventContextClid.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/ByteStreamMetadata.h"
#include "EventInfo/EventInfo.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

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
#include "GaudiKernel/ThreadLocalContext.h"

// TDAQ includes
#include "hltinterface/DataCollector.h"
#include "owl/time.h"

// System includes
#include <sstream>

#define ST_WHERE "HltEventLoopMgr::" << __func__ << "(): "

using namespace boost::property_tree;
using SOR = TrigSORFromPtreeHelper::SOR;


// =============================================================================
// Standard constructor
// =============================================================================
HltEventLoopMgr::HltEventLoopMgr(const std::string& name, ISvcLocator* svcLoc)
: base_class(name, svcLoc),
  m_incidentSvc("IncidentSvc", name),
  m_evtStore("StoreGateSvc", name),
  m_detectorStore("DetectorStore", name),
  m_inputMetaDataStore("StoreGateSvc/InputMetaDataStore", name),
  m_robDataProviderSvc("ROBDataProviderSvc", name),
  m_THistSvc("THistSvc", name),
  m_evtSelector("EvtSel", name),
  m_coolHelper("TrigCOOLUpdateHelper", this),
  m_eventInfoCnvTool("xAODMaker::EventInfoCnvTool/EventInfoCnvTool", this),
  m_detector_mask(0xffffffff, 0xffffffff, 0, 0),
  m_nevt(0),
  m_threadPoolSize(-1),
  m_evtSelContext(nullptr),
  m_softTimeoutValue(10000),
  m_runEventTimer(true)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  declareProperty("JobOptionsType",           m_jobOptionsType="NONE");
  declareProperty("ApplicationName",          m_applicationName="None");
  declareProperty("PartitionName",            m_partitionName="None");
  declareProperty("enabledROBs",              m_enabledROBs);
  declareProperty("enabledSubDetectors",      m_enabledSubDetectors);
  declareProperty("EvtSel",                   m_evtSelector);
  declareProperty("CoolUpdateTool",           m_coolHelper);
  declareProperty("EventInfoCnvTool",         m_eventInfoCnvTool);
  declareProperty("SchedulerSvc",             m_schedulerName="AvalancheSchedulerSvc",
                  "Name of the scheduler to be used");
  declareProperty("WhiteboardSvc",            m_whiteboardName="EventDataSvc",
                  "Name of the Whiteboard to be used");
  declareProperty("TopAlg",                   m_topAlgNames={},
                  "List of top level algorithms names");
  declareProperty("xEventInfoWHKey",          m_xEventInfoWHKey="EventInfo");
  declareProperty("HardTimeout",              m_hardTimeout=10*60*1000/*=10min*/,
                  "Hard event processing timeout in milliseconds");
  declareProperty("SoftTimeoutFraction",      m_softTimeoutFraction=0.8,
                  "Fraction of the hard timeout to be set as the soft timeout");

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
}

// =============================================================================
// Standard destructor
// =============================================================================
HltEventLoopMgr::~HltEventLoopMgr()
{
}

// =============================================================================
// Implementation of IInterface::queryInterface (overriding other implementations)
// =============================================================================
StatusCode HltEventLoopMgr::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  if(!ppvInterface)
      return StatusCode::FAILURE;

  if(ITrigEventLoopMgr::interfaceID().versionMatch(riid))
    *ppvInterface = static_cast<ITrigEventLoopMgr*>(this);
  else if(IEventProcessor::interfaceID().versionMatch(riid))
    *ppvInterface = static_cast<IEventProcessor*>(this);
  else
    return base_class::queryInterface(riid, ppvInterface);

  addRef();
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
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
    try {
      if (m_jobOptionsType.assign( propMgr->getProperty("JobOptionsType") ))
        ATH_MSG_DEBUG(" ---> Read from DataFlow configuration: " << m_jobOptionsType);
    }
    catch (...) {
      ATH_MSG_WARNING("Could not set Property '" << m_jobOptionsType.name() << "' from DataFlow");
    }

    if ( m_topAlgNames.value().empty() ) {
      if (setProperty(propMgr->getProperty("TopAlg")).isFailure()) {
        ATH_MSG_WARNING("Could not set the TopAlg property from ApplicationMgr");
      }
    }
  }
  else {
    ATH_MSG_WARNING("Error retrieving IProperty interface of ApplicationMgr" );
  }

  // get JobOptionsSvc to read the configuration of NumConcurrentEvents and NumThreads
  ServiceHandle<IJobOptionsSvc> jobOptionsSvc("JobOptionsSvc", name());
  if ((jobOptionsSvc.retrieve()).isFailure()) {
    ATH_MSG_WARNING("Could not find JobOptionsSvc to read configuration");
  }

  // print properties
  ATH_MSG_INFO(" ---> ApplicationName        = " << m_applicationName);
  ATH_MSG_INFO(" ---> PartitionName          = " << m_partitionName);
  ATH_MSG_INFO(" ---> JobOptionsType         = " << m_jobOptionsType);
  ATH_MSG_INFO(" ---> HardTimeout            = " << m_hardTimeout);
  ATH_MSG_INFO(" ---> SoftTimeoutFraction    = " << m_softTimeoutFraction);
  if (jobOptionsSvc.isValid()) {
    const Gaudi::Details::PropertyBase* prop = jobOptionsSvc->getClientProperty("EventDataSvc","NSlots");
    if (prop)
      ATH_MSG_INFO(" ---> NumConcurrentEvents    = " << *prop);
    else
      ATH_MSG_WARNING("Failed to retrieve the job property EventDataSvc.NSlots");
    prop = jobOptionsSvc->getClientProperty("AvalancheSchedulerSvc","ThreadPoolSize");
    if (prop)
      ATH_MSG_INFO(" ---> NumThreads             = " << *prop);
    else
      ATH_MSG_WARNING("Failed to retrieve the job property AvalancheSchedulerSvc.ThreadPoolSize");
  }
  ATH_MSG_INFO(" ---> Enabled ROBs: size = " << m_enabledROBs.value().size()
               << (m_enabledROBs.value().size()==0 ? ". No check will be performed" : " "));
  ATH_MSG_INFO(" ---> Enabled Sub Detectors: size = " << m_enabledSubDetectors.value().size()
               << (m_enabledSubDetectors.value().size()==0 ? ". No check will be performed" : " "));

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
  // Setup stuff for hive - taken from AthenaHiveEventLoopMgr
  //----------------------------------------------------------------------------
  m_whiteboard = serviceLocator()->service(m_whiteboardName);
  if( !m_whiteboard.isValid() )  {
    ATH_MSG_FATAL("Error retrieving " << m_whiteboardName << " interface IHiveWhiteBoard");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Initialised " << m_whiteboardName << " interface IHiveWhiteBoard");

  ATH_MSG_DEBUG("Commented out scheduler initialisation was here, now it's moved to hltUpdateAfterFork");
  /*
  m_schedulerSvc = serviceLocator()->service(m_schedulerName);
  if ( !m_schedulerSvc.isValid()){
    ATH_MSG_FATAL("Error retrieving " << m_schedulerName << " interface ISchedulerSvc");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("initialised " << m_schedulerName << " interface ISchedulerSvc");
  */
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
  // Setup the IncidentSvc
  //----------------------------------------------------------------------------
  ATH_CHECK(m_incidentSvc.retrieve());

  //----------------------------------------------------------------------------
  // Setup the StoreGateSvc
  //----------------------------------------------------------------------------
  ATH_CHECK(m_evtStore.retrieve());

  //----------------------------------------------------------------------------
  // Setup the DetectorStore
  //----------------------------------------------------------------------------
  ATH_CHECK(m_detectorStore.retrieve());

  //----------------------------------------------------------------------------
  // Setup the InputMetaDataStore
  //----------------------------------------------------------------------------
  ATH_CHECK(m_inputMetaDataStore.retrieve());

  //----------------------------------------------------------------------------
  // Setup the ROBDataProviderSvc
  //----------------------------------------------------------------------------
  ATH_CHECK(m_robDataProviderSvc.retrieve());

  //----------------------------------------------------------------------------
  // Setup the Histogram Service
  //----------------------------------------------------------------------------
  ATH_CHECK(m_THistSvc.retrieve());

  //----------------------------------------------------------------------------
  // Setup the Event Selector
  //----------------------------------------------------------------------------
  ATH_CHECK(m_evtSelector.retrieve());
  ATH_CHECK(m_evtSelector->createContext(m_evtSelContext));

  //----------------------------------------------------------------------------
  // Setup the COOL helper
  //----------------------------------------------------------------------------
  ATH_CHECK(m_coolHelper.retrieve());

  //----------------------------------------------------------------------------
  // Setup the EventInfo conversion tool and WriteHandleKey
  //----------------------------------------------------------------------------
  ATH_CHECK(m_eventInfoCnvTool.retrieve());
  ATH_CHECK(m_xEventInfoWHKey.initialize());

  //----------------------------------------------------------------------------
  // Setup the HLT Histogram Service when configured
  //----------------------------------------------------------------------------
  if ( &*m_THistSvc ) {
    m_hltTHistSvc = SmartIF<IHltTHistSvc>( &*m_THistSvc );
    if (m_hltTHistSvc.isValid())
      ATH_MSG_INFO("A THistSvc implementing the HLT interface IHltTHistSvc was found");
    else
      ATH_MSG_INFO("No THistSvc implementing the HLT interface IHltTHistSvc was found");
  }

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
// Reimplementation of AthService::start (IStateful interface)
// =============================================================================
StatusCode HltEventLoopMgr::start()
{
  ATH_CHECK(AthService::start());

  // start top level algorithms
  for (auto& ita : m_topAlgList) {
    ATH_CHECK(ita->sysStart());
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
// Reimplementation of AthService::stop (IStateful interface)
// =============================================================================
StatusCode HltEventLoopMgr::stop()
{
  ATH_CHECK(AthService::stop());

  // stop top level algorithms
  for (auto& ita : m_topAlgList) {
    ATH_CHECK(ita->sysStop());
  }

  return StatusCode::SUCCESS;
}

// =============================================================================
// Reimplementation of AthService::finalize (IStateful interface)
// =============================================================================
StatusCode HltEventLoopMgr::finalize()
{
  ATH_MSG_INFO(" ---> HltEventLoopMgr = " << name() << " finalize ");
  ATH_MSG_INFO(" Total number of events processed " << m_nevt);

  // Need to release now - automatic release in destructor is too late since services are already gone
  m_hltTHistSvc.reset();
  m_hltROBDataProviderSvc.reset();

  // Finalise top level algorithms
  for (auto& ita : m_topAlgList) {
    if (ita->sysFinalize().isFailure())
      ATH_MSG_WARNING("Finalisation of algorithm " << ita->name() << " failed");
  }
  // Release top level algorithms
  SmartIF<IAlgManager> algMgr = serviceLocator()->as<IAlgManager>();
  if (!algMgr.isValid()) {
    ATH_MSG_WARNING("Failed to retrieve AlgManager - cannot finalise top level algorithms");
  }
  else {
    for (auto& ita : m_topAlgList) {
      if (algMgr->removeAlgorithm(ita).isFailure())
        ATH_MSG_WARNING("Problems removing Algorithm " << ita->name());
    }
  }
  m_topAlgList.clear();

  // Release service handles
  if (m_incidentSvc.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_incidentSvc.typeAndName());
  if (m_robDataProviderSvc.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_robDataProviderSvc.typeAndName());
  if (m_evtStore.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_evtStore.typeAndName());
  if (m_detectorStore.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_detectorStore.typeAndName());
  if (m_inputMetaDataStore.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_inputMetaDataStore.typeAndName());
  if (m_THistSvc.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_THistSvc.typeAndName());

  // Release tool handles
  if (m_coolHelper.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_coolHelper.typeAndName());
  if (m_eventInfoCnvTool.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_eventInfoCnvTool.typeAndName());

  // Release SmartIFs
  m_whiteboard.reset();
  m_algResourcePool.reset();
  m_aess.reset();
  m_schedulerSvc.reset();
  m_hltTHistSvc.reset();
  m_hltROBDataProviderSvc.reset();

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
    // internalPrepareResets() was here
    StatusCode sc = clearTemporaryStores();
    if (sc.isFailure()) {
      ATH_MSG_ERROR("Clearing temporary stores failed");
      return sc;
    }

    const SOR* sor;
    // update SOR in det store and get it back
    if(!(sor = processRunParams(pt)))
      return StatusCode::FAILURE;

    auto& soral = getSorAttrList(sor);

    updateInternal(soral);     // update internally kept info
    updateMetadataStore(soral);  // update metadata store

    /*
    const EventInfo * evinfo;
    if(updMagField(pt).isFailure() ||     // update mag field when appropriate
       updHLTConfigSvc().isFailure() ||   // update config svc when appropriate
       resetCoolValidity().isFailure() || // reset selected proxies/IOV folders
       prepXAODEventInfo().isFailure() || // update xAOD event data in SG
       !(evinfo = prepEventInfo()))       // update old event data in SG
      return StatusCode::FAILURE;

    bookAllHistograms();

    ATH_MSG_VERBOSE("end of " << __FUNCTION__);
    if(prepareAlgs(*evinfo).isSuccess())
      return StatusCode::SUCCESS;
    */

    ATH_MSG_VERBOSE("end of " << __FUNCTION__);
    return StatusCode::SUCCESS;
  }
  catch(const ptree_bad_path & e)
  {
    ATH_MSG_ERROR(ST_WHERE << "Bad ptree path: \"" << e.path<ptree::path_type>().dump() << "\" - " << e.what());
  }
  catch(const ptree_bad_data & e)
  {
    ATH_MSG_ERROR(ST_WHERE << "Bad ptree data: \"" << e.data<ptree::data_type>() << "\" - " << e.what());
  }
  catch(const std::runtime_error& e)
  {
    ATH_MSG_ERROR(ST_WHERE << "Runtime error: " << e.what());
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
    ATH_MSG_WARNING("Could not retrieve CoreDumpSvc");
  }

  // Start the timeout thread
  m_timeoutThread.reset(new std::thread(std::bind(&HltEventLoopMgr::runEventTimer,this)));

  // Initialise vector of time points for event timeout monitoring
  {
    std::unique_lock<std::mutex> lock(m_timeoutMutex);
    m_eventTimerStartPoint.clear();
    m_eventTimerStartPoint.resize(m_whiteboard->getNumberOfStores(), std::chrono::steady_clock::time_point());
    m_isSlotProcessing.resize(m_whiteboard->getNumberOfStores(), false);
    m_timeoutCond.notify_all();
  }

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of ITrigEventLoopMgr::processRoIs
// DUE TO BE REMOVED
// =============================================================================
StatusCode HltEventLoopMgr::processRoIs(
             const std::vector<eformat::ROBFragment<const uint32_t*> >& /*l1_result*/,
             hltinterface::HLTResult& /*hlt_result*/,
             const hltinterface::EventId& /*evId*/)
{
  ATH_MSG_VERBOSE("dummy implementation of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of ITrigEventLoopMgr::timeOutReached
// DUE TO BE REMOVED
// =============================================================================
StatusCode HltEventLoopMgr::timeOutReached(const boost::property_tree::ptree& /*pt*/)
{
  ATH_MSG_VERBOSE("dummy implementation of " << __FUNCTION__);
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
    ATH_MSG_ERROR("Event loop failed");
    // some special cleanup here?
  }

  // do some cleanup here

  // stop the timer thread - this should be in the stop() transition, but it is not called at the moment
  {
    ATH_MSG_DEBUG("Stopping the timeout thread");
    std::unique_lock<std::mutex> lock(m_timeoutMutex);
    m_runEventTimer = false;
    m_timeoutCond.notify_all();
    m_timeoutThread->join();
    ATH_MSG_DEBUG("The timeout thread finished");
  }

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
  bool loop_ended = false;
  bool events_available = true; // DataCollector has more events

  while (!loop_ended) {
    ATH_MSG_DEBUG("Free slots = " << m_schedulerSvc->freeSlots());
    if (m_schedulerSvc->freeSlots()>0 && events_available) {
      ATH_MSG_DEBUG("Free slots = " << m_schedulerSvc->freeSlots() << ". Reading the next event.");

      eformat::write::FullEventFragment l1r; // to be removed

      //-----------------------------------------------------------------------
      // Create new EventContext
      //-----------------------------------------------------------------------
      ++m_nevt;
      EventContext* evtContext = nullptr;
      if (createEventContext(evtContext).isFailure()){
        ATH_MSG_ERROR("Failed to create event context");
        // what do we do now? we haven't requested the next event from DataCollector yet,
        // we have a failure while not processing an event
        continue;
      }

      // do we need this duplication? it's selected already in createEventContext
      if (m_whiteboard->selectStore(evtContext->slot()).isFailure()){
        ATH_MSG_ERROR("Slot " << evtContext->slot() << " could not be selected for the WhiteBoard");
        // what do we do now? we haven't requested the next event from DataCollector yet,
        // we have a failure while not processing an event
        continue;
      }

      // Not so nice behind-the-scenes way to inform the InputSvc / ROBDataProvider about the slot for the new event
      // to be read. This is likely also used by other services.
      Gaudi::Hive::setCurrentContext(*evtContext);

      //-----------------------------------------------------------------------
      // Get the next event
      //-----------------------------------------------------------------------
      StatusCode sc = StatusCode::SUCCESS;
      try {
        sc = m_evtSelector->next(*m_evtSelContext);
      }
      catch (hltonl::Exception::NoMoreEvents e) {
        sc = StatusCode::SUCCESS;
        events_available = false;
        sc = clearWBSlot(evtContext->slot());
        if (sc.isFailure()) {
          ATH_MSG_WARNING("Failed to clear the whiteboard slot " << evtContext->slot()
                          << " after NoMoreEvents detected");
          // do we need to do anything here?
        }
        continue;
      }
      catch (...) {
        sc = StatusCode::FAILURE;
      }
      if (sc.isFailure()) {
        ATH_MSG_ERROR("Failed to get the next event");
        // we called getNext and some failure happened, but we don't know the event number
        // should we return anything to the DataCollector?
        failedEvent(evtContext);
        continue;
      }

      // set event processing start time for timeout monitoring and reset timeout flag
      {
        std::unique_lock<std::mutex> lock(m_timeoutMutex);
        m_eventTimerStartPoint[evtContext->slot()] = std::chrono::steady_clock::now();
        m_isSlotProcessing[evtContext->slot()] = true;
        resetTimeout(Athena::Timeout::instance(*evtContext));
        m_timeoutCond.notify_all();
      }

      IOpaqueAddress* addr = nullptr;
      if (m_evtSelector->createAddress(*m_evtSelContext, addr).isFailure()) {
        ATH_MSG_ERROR("Could not create an IOpaqueAddress");
        // we cannot get the EventInfo, so we don't know the event number
        // should we return anything to the DataCollector?
        failedEvent(evtContext);
        continue;
      }

      if (addr != nullptr) {
        /* do we need this???
        sc = m_evtStore->recordAddress(addr);
        if (sc.isFailure()) {
          ATH_MSG_WARNING("Failed to record IOpaqueAddress in the event store");
          // we cannot get the EventInfo, so we don't know the event number
          // should we return anything to the DataCollector?
          // failedEvent(evtContext);
        }
        */
        sc = m_evtStore->loadEventProxies();
        if (sc.isFailure()) {
          ATH_MSG_ERROR("Failed to load event proxies");
          // we cannot get the EventInfo, so we don't know the event number
          // should we return anything to the DataCollector?
          failedEvent(evtContext);
        }
      }

      const EventInfo* oldEventInfo = nullptr;
      sc = m_evtStore->retrieve(oldEventInfo);
      if (sc.isFailure()) {
        ATH_MSG_ERROR("Failed to retrieve EventInfo");
        // we cannot get the EventInfo, so we don't know the event number
        // should we return anything to the DataCollector?
        failedEvent(evtContext);
      }

      // convert EventInfo to xAOD::EventInfo using a cnv tool explicitly, like in EventInfoCnvAlg
      // - this should be done by an automatic conversion through proxy in the future,
      // replacing the code above and below
      SG::WriteHandle<xAOD::EventInfo> eventInfoWH(m_xEventInfoWHKey, *evtContext);
      if (eventInfoWH.record(
        std::make_unique<xAOD::EventInfo>(),
        std::make_unique<xAOD::EventAuxInfo>()
      ).isFailure()) {
        ATH_MSG_ERROR("Failed to record xAOD::EventInfo in event store");
        failedEvent(evtContext);
      }

      xAOD::EventInfo* eventInfo = eventInfoWH.ptr();
      if (m_eventInfoCnvTool->convert(oldEventInfo, eventInfo).isFailure()) {
        ATH_MSG_ERROR("Failed to convert EventInfo to xAOD::EventInfo");
        failedEvent(evtContext);
      }

      ATH_MSG_DEBUG("Retrieved event info for the new event " << *eventInfo);

      // create EventIDBase for the EventContext
      EventIDBase eid(eventInfo->runNumber(),
                      eventInfo->eventNumber(),
                      eventInfo->timeStamp(),
                      eventInfo->timeStampNSOffset(),
                      eventInfo->lumiBlock(),
                      eventInfo->bcid());
      evtContext->setEventID(eid);

      // set run number for conditions
      evtContext->template getExtension<Atlas::ExtendedEventContext>()->setConditionsRun(m_currentRun);

      // update thread-local EventContext after setting EventID and conditions runNumber
      Gaudi::Hive::setCurrentContext(*evtContext);

      // Record EventContext in current whiteboard
      if (m_evtStore->record(std::make_unique<EventContext> (*evtContext), "EventContext").isFailure()) {
        ATH_MSG_ERROR("Error recording event context object");
        failedEvent(evtContext,eventInfo);
        continue;
      }

      if (executeEvent(evtContext).isFailure()) {
        ATH_MSG_ERROR("Error processing event");
        failedEvent(evtContext,eventInfo);
        continue;
      }

    }
    else {
      ATH_MSG_DEBUG("No free slots or no more events to process - draining the scheduler");
      int ir = drainScheduler();
      if (ir<0) {
        ATH_MSG_ERROR("Error draining scheduler");
        continue;
      }
      else if (ir==0 && !events_available) {
        ATH_MSG_INFO("All events processed, finalising the event loop");
        loop_ended = true;
      }
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

  EventContext* evtContext = static_cast<EventContext*>(pEvtContext);
  if (!evtContext) {
    ATH_MSG_ERROR("Failed to cast the call parameter to EventContext*");
    return StatusCode::FAILURE;
  }

  resetTimeout(Athena::Timeout::instance(*evtContext));

  // Now add event to the scheduler
  ATH_MSG_DEBUG("Adding event " << evtContext->evt() << ", slot " << evtContext->slot() << " to the scheduler");
  StatusCode addEventStatus = m_schedulerSvc->pushNewEvent(evtContext);

  // If this fails, we need to wait for something to complete
  if (!addEventStatus.isSuccess()){
    ATH_MSG_ERROR("Failed adding event " << evtContext->evt() << ", slot " << evtContext->slot()
                  << " to the scheduler");
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
void HltEventLoopMgr::updateDFProps()
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ServiceHandle<IJobOptionsSvc> p_jobOptionsSvc("JobOptionsSvc", name());
  if ((p_jobOptionsSvc.retrieve()).isFailure()) {
    ATH_MSG_WARNING("Could not find JobOptionsSvc to set DataFlow properties");
  } else {
    auto dfprops = p_jobOptionsSvc->getProperties("DataFlowConfig");

    // Application name
    auto pname = "DF_ApplicationName";
    const auto * prop = Gaudi::Utils::getProperty(dfprops, pname);
    if(prop && m_applicationName.assign(*prop)) {
      ATH_MSG_DEBUG(" ---> Read from DataFlow configuration: " << m_applicationName);
    } else {
      ATH_MSG_WARNING("Could not set Property '" << pname << "' from DataFlow");
    }

    // Partition name
    pname = "DF_PartitionName";
    prop = Gaudi::Utils::getProperty(dfprops, pname);
    if (prop && m_partitionName.assign(*prop)) {
      ATH_MSG_DEBUG(" ---> Read from DataFlow configuration: " << m_partitionName);
    } else {
      ATH_MSG_WARNING("Could not set Property '" << pname << "' from DataFlow");
    }

    // get the list of enabled ROBs
    pname = "DF_Enabled_ROB_IDs";
    prop = Gaudi::Utils::getProperty(dfprops, pname);
    if (prop && m_enabledROBs.assign(*prop)) {
      ATH_MSG_DEBUG(" ---> Read from DataFlow configuration: "
                    << m_enabledROBs.value().size() << " enabled ROB IDs");
    } else {
      // this is only info, because it is normal in athenaHLT
      ATH_MSG_INFO("Could not set Property '" << pname << "' from DataFlow");
    }

    // get the list of enabled Sub Detectors
    pname = "DF_Enabled_SubDet_IDs";
    prop = Gaudi::Utils::getProperty(dfprops, pname);
    if (prop && m_enabledSubDetectors.assign(*prop)) {
      ATH_MSG_DEBUG(" ---> Read from DataFlow configuration: "
                    << m_enabledSubDetectors.value().size() << " enabled Sub Detector IDs");
    } else {
      // this is only info, because it is normal in athenaHLT
      ATH_MSG_INFO("Could not set Property '" << pname << "' from DataFlow");
    }
  }

  p_jobOptionsSvc.release().ignore();
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
}

// =============================================================================
const SOR* HltEventLoopMgr::processRunParams(const ptree & pt)
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  // update the run number
  m_currentRun = pt.get<uint32_t>("RunParams.run_number");

  // need to provide an event context extended with a run number, down the line passed to IOVDBSvc::signalBeginRun
  EventContext runStartEventContext = {}; // with invalid evt number and slot number
  runStartEventContext.setExtension(Atlas::ExtendedEventContext(m_evtStore->hiveProxyDict(), m_currentRun));

  // Fill SOR parameters from the ptree
  TrigSORFromPtreeHelper sorhelp{msgStream()};
  const SOR* sor = sorhelp.fillSOR(pt.get_child("RunParams"),runStartEventContext);
  if(!sor) {
    ATH_MSG_ERROR(ST_WHERE << "setup of SOR from ptree failed");
  }

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return sor;
}

// =============================================================================
void HltEventLoopMgr::updateInternal(const coral::AttributeList & sor_attrlist)
{
  auto detMaskFst = sor_attrlist["DetectorMaskFst"].data<unsigned long long>();
  auto detMaskSnd = sor_attrlist["DetectorMaskSnd"].data<unsigned long long>();
  updateDetMask({detMaskFst, detMaskSnd});

  // auto sorTime = sor_attrlist["SORTime"].data<unsigned long long>();
  // updSorTime(sorTime);

  if(msgLevel() <= MSG::DEBUG)
  {
    // save current stream flags for later reset
    // cast needed (stream thing returns long, but doesn't take it back)
    auto previous_stream_flags = static_cast<std::ios::fmtflags>(msgStream().flags());
    ATH_MSG_DEBUG(ST_WHERE
                  << "Full detector mask (128 bits) = 0x"
                  << MSG::hex << std::setfill('0')
                  << std::setw(8) << std::get<3>(m_detector_mask)
                  << std::setw(8) << std::get<2>(m_detector_mask)
                  << std::setw(8) << std::get<1>(m_detector_mask)
                  << std::setw(8) << std::get<0>(m_detector_mask));
    msgStream().flags(previous_stream_flags);

    // ATH_MSG_DEBUG(ST_WHERE << "sorTimeStamp[0] [sec] = " << m_sorTime_stamp[0]);
    // ATH_MSG_DEBUG(ST_WHERE << "sorTimeStamp[1] [ns]  = " << m_sorTime_stamp[1]);
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
    ATH_MSG_WARNING(ST_WHERE << "Unable to record MetaData in InputMetaDataStore");
    delete metadata;
  }
  else {
    ATH_MSG_DEBUG(ST_WHERE << "Recorded MetaData in InputMetaDataStore");
  }

}

// ==============================================================================
StatusCode HltEventLoopMgr::clearTemporaryStores()
{
  //----------------------------------------------------------------------------
  // Clear the event store, if used in the event loop
  //----------------------------------------------------------------------------
  auto sc = m_evtStore->clearStore();
  ATH_MSG_DEBUG(ST_WHERE << "clear of Event Store " << sc);
  if(sc.isFailure()) {
    ATH_MSG_ERROR(ST_WHERE << "clear of Event Store failed");
    return sc;
  }

  //----------------------------------------------------------------------------
  // Clear the InputMetaDataStore
  //----------------------------------------------------------------------------
  sc = m_inputMetaDataStore->clearStore();
  ATH_MSG_DEBUG(ST_WHERE << "clear of InputMetaDataStore store " << sc);
  if(sc.isFailure())
    ATH_MSG_ERROR(ST_WHERE << "clear of InputMetaDataStore failed");

  return sc;
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

// ==============================================================================
const coral::AttributeList& HltEventLoopMgr::getSorAttrList(const SOR* sor) const
{
  if(sor->size() != 1)
  {
    // This branch should never be entered (the CondAttrListCollection
    // corresponding to the SOR should contain one single AttrList). Since
    // that's required by code ahead but not checked at compile time, we
    // explicitly guard against any potential future mistake with this check
    ATH_MSG_ERROR(ST_WHERE << "Wrong SOR: size = " << sor->size());
    throw std::runtime_error("SOR record should have one and one only attribute list, but it has " + sor->size());
  }

  const auto & soral = sor->begin()->second;
  printSORAttrList(soral);
  return soral;
}

// ==============================================================================
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

// ==============================================================================
void HltEventLoopMgr::failedEvent(EventContext* eventContext,
                                  const xAOD::EventInfo* eventInfo,
                                  HLT::HLTResult* hltResult) const
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  // in case EventInfo is unavailable we try to make a temporary one just to build the HLT result
  // - this unique_ptr deletes it when going out of scope
  std::unique_ptr<xAOD::EventInfo> tmpEventInfo;

  if (eventContext) {
    // try to retrieve the event info from event store
    if (m_whiteboard->selectStore(eventContext->slot()).isSuccess()) {
      if (!eventInfo && m_evtStore->retrieve(eventInfo).isFailure()) {
        EventIDBase eid = eventContext->eventID();
        ATH_MSG_WARNING("Failed to retrieve event info. Will try to build one from EventContext (" << *eventContext
                        << ") with eventID = " << eid);
        tmpEventInfo = std::make_unique<xAOD::EventInfo>();
        tmpEventInfo->setRunNumber(eid.run_number());
        tmpEventInfo->setEventNumber(eid.event_number());
        tmpEventInfo->setTimeStamp(eid.time_stamp());
        tmpEventInfo->setTimeStampNSOffset(eid.time_stamp_ns_offset());
        tmpEventInfo->setLumiBlock(eid.lumi_block());
        tmpEventInfo->setBCID(eid.bunch_crossing_id());
        eventInfo = tmpEventInfo.get();
      }
      if (!hltResult && m_evtStore->retrieve(hltResult).isFailure()) {
        ATH_MSG_DEBUG("Failed to retrieve HLT result");
      }
    }
    else {
      ATH_MSG_DEBUG("Failed to select whiteboard store for slot " << eventContext->slot());
    }
  }

  if (!eventContext && !eventInfo) {
    ATH_MSG_ERROR("Failure occurred while we don't have either EventContext or EventInfo, so don't know which event "
                    << "was being processed. No result will be produced for this event.");
    return;
  }

  if (eventContext && clearWBSlot(eventContext->slot()).isFailure()) {
    ATH_MSG_WARNING("Failed to clear the whiteboard slot " << eventContext->slot());
  }

  // need to create and return a result with debug stream flags
  // for now, only creating a dummy result
  eformat::write::FullEventFragment* hltrFragment = fullHltResult(eventInfo,hltResult);

  // serialize and send the result
  eventDone(hltrFragment);
  // should we delete hltrFragment now?

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
}

// ==============================================================================
eformat::write::FullEventFragment* HltEventLoopMgr::fullHltResult(const xAOD::EventInfo* eventInfo,
                                                                  HLT::HLTResult* hltResult) const
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_MSG_DEBUG("Creating an HLT result for event " << *eventInfo);
  eformat::write::FullEventFragment* hltrFragment = new eformat::write::FullEventFragment;

  // need to handle eventInfo==nullptr here?

  //----------------------------------------------------------------------------
  // Fill header data from EventInfo
  //----------------------------------------------------------------------------
  hltrFragment->global_id(eventInfo->eventNumber());
  hltrFragment->bc_time_seconds(eventInfo->timeStamp());
  hltrFragment->bc_time_nanoseconds(eventInfo->timeStampNSOffset());
  hltrFragment->run_no(eventInfo->runNumber());
  hltrFragment->bc_id(eventInfo->bcid());
  hltrFragment->lvl1_id(eventInfo->extendedLevel1ID());
  hltrFragment->lvl1_trigger_type(eventInfo->level1TriggerType());
  /* not available in xAOD::EventInfo
  hltrFragment->lvl1_trigger_info(eventInfo->trigger_info()->level1TriggerInfo().size(),
                                  eventInfo->trigger_info()->level1TriggerInfo().data());
  */

  // need to handle hltResult==nullptr here?

  ATH_MSG_DEBUG("Event " << eventInfo->eventNumber() << " is "
                << (hltResult->accepted() ? "accepted" : "rejected"));

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return hltrFragment;
}

// ==============================================================================
void HltEventLoopMgr::eventDone(eformat::write::FullEventFragment* hltrFragment) const
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  const eformat::write::node_t* top = hltrFragment->bind();
  auto hltrFragmentSize = hltrFragment->size_word();
  auto hltrPtr = std::make_unique<uint32_t[]>(hltrFragmentSize);
  auto copiedSize = eformat::write::copy(*top,hltrPtr.get(),hltrFragmentSize);
  if(copiedSize!=hltrFragmentSize){
    ATH_MSG_ERROR("HLT result FullEventFragment serialization failed");
    // missing error handling
  }
  ATH_MSG_DEBUG("Sending the HLT result to DataCollector");
  hltinterface::DataCollector::instance()->eventDone(std::move(hltrPtr));
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
}

// ==============================================================================
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
        EventContext ctx(i,0); // we only need the slot number for Athena::Timeout instance
        // don't duplicate the actions if the timeout was already reached
        if (!Athena::Timeout::instance(ctx).reached()) {
          auto procTime = now - m_eventTimerStartPoint.at(i);
          auto procTimeMillisec = std::chrono::duration_cast<std::chrono::milliseconds>(procTime);
          ATH_MSG_WARNING("Soft timeout in slot " << i << ". Processing time = "
                          << procTimeMillisec.count() << " ms");
          setTimeout(Athena::Timeout::instance(ctx));
        }
      }
    }
  }
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
}

// ==============================================================================
StatusCode HltEventLoopMgr::createEventContext(EventContext*& evtContext) const
{
  evtContext = new EventContext(m_nevt, m_whiteboard->allocateStore(m_nevt));

  m_aess->reset(*evtContext);

  StatusCode sc = m_whiteboard->selectStore(evtContext->slot());
  if (sc.isFailure()){
    ATH_MSG_WARNING("Slot " << evtContext->slot() << " could not be selected for the WhiteBoard");
  } else {
    evtContext->setExtension( Atlas::ExtendedEventContext( m_evtStore->hiveProxyDict() ) );
    ATH_MSG_DEBUG("created EventContext, num: " << evtContext->evt()  << "  in slot: " << evtContext->slot());
  }
  return sc;
}

// ==============================================================================
int HltEventLoopMgr::drainScheduler()
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  // method copied from AthenaHiveEventLoopMgr

  StatusCode sc(StatusCode::SUCCESS);

  // maybe we can do better
  std::vector<EventContext*> finishedEvtContexts;

  EventContext* finishedEvtContext(nullptr);

  // Here we wait not to loose cpu resources
  // ATH_MSG_DEBUG("drainScheduler: [" << finishedEvts << "] Waiting for a context");
  ATH_MSG_DEBUG("drainScheduler: Waiting for a context");
  sc = m_schedulerSvc->popFinishedEvent(finishedEvtContext);

  // We got past it: cache the pointer
  if (sc.isSuccess()){
    ATH_MSG_DEBUG("drainScheduler: scheduler not empty: Context "
	    << finishedEvtContext);
    finishedEvtContexts.push_back(finishedEvtContext);
  } else{
    // no more events left in scheduler to be drained
    ATH_MSG_DEBUG("drainScheduler: scheduler empty");
    return 0;
  }

  // Let's see if we can pop other event contexts
  while (m_schedulerSvc->tryPopFinishedEvent(finishedEvtContext).isSuccess()){
    finishedEvtContexts.push_back(finishedEvtContext);
  }

  // Now we flush them
  bool fail(false);
  for (auto& thisFinishedEvtContext : finishedEvtContexts) {
    if (!thisFinishedEvtContext) {
      ATH_MSG_FATAL("Detected nullptr ctxt while clearing WB!");
      fail = true;
      continue;
    }

    if (m_aess->eventStatus(*thisFinishedEvtContext) != EventStatus::Success) {
      ATH_MSG_FATAL("Failed event detected on " << thisFinishedEvtContext
                    << " with fail mode: " << m_aess->eventStatus(*thisFinishedEvtContext));
      delete thisFinishedEvtContext;
      fail = true;
      continue;
    }

    // EventID::number_type n_run(0);
    // EventID::number_type n_evt(0);

    const xAOD::EventInfo* eventInfo = nullptr;
    HLT::HLTResult* hltResult = nullptr;
    if (m_whiteboard->selectStore(thisFinishedEvtContext->slot()).isSuccess()) {
      if (m_evtStore->retrieve(eventInfo).isFailure()) {
        ATH_MSG_ERROR(__FUNCTION__ << ": Failed to retrieve EventInfo");
        delete thisFinishedEvtContext;
        fail = true;
        continue;
      } else {
        // n_run = eventInfo->runNumber();
        // n_evt = eventInfo->eventNumber();
        if (m_evtStore->retrieve(hltResult).isFailure()) {
          ATH_MSG_ERROR(__FUNCTION__ << ": Failed to retrieve HltResult");
        }
      }
    } else {
      ATH_MSG_ERROR("DrainSched: unable to select store " << thisFinishedEvtContext->slot());
      delete thisFinishedEvtContext;
      fail = true;
      continue;
    }

    // m_incidentSvc->fireIncident(Incident(name(), IncidentType::EndEvent,
    // 					 *thisFinishedEvtContext ));

    if (!hltResult) {
      failedEvent(thisFinishedEvtContext,eventInfo);
      continue;
    }

    // create HLT result FullEventFragment
    eformat::write::FullEventFragment* hltrFragment = fullHltResult(eventInfo,hltResult);
    // serialize and send the result
    eventDone(hltrFragment);
    // should we delete hltrFragment now?

    { // flag idle slot to the timeout thread and reset the timer
      std::unique_lock<std::mutex> lock(m_timeoutMutex);
      m_eventTimerStartPoint[thisFinishedEvtContext->slot()] = std::chrono::steady_clock::now();
      m_isSlotProcessing[thisFinishedEvtContext->slot()] = false;
      resetTimeout(Athena::Timeout::instance(*thisFinishedEvtContext));
      m_timeoutCond.notify_all();
    }

    ATH_MSG_DEBUG("Clearing slot " << thisFinishedEvtContext->slot()
                  << " (event " << thisFinishedEvtContext->evt() << ") of the whiteboard");

    StatusCode sc = clearWBSlot(thisFinishedEvtContext->slot());
    if (!sc.isSuccess()) {
      ATH_MSG_ERROR("Whiteboard slot " << thisFinishedEvtContext->slot() << " could not be properly cleared");
      fail = true;
      delete thisFinishedEvtContext;
      continue;
    }

/*
    finishedEvts++;

    writeHistograms().ignore();
    ++m_proc;

    if (m_doEvtHeartbeat) {
      if(!m_useTools)
        ATH_MSG_INFO("  ===>>>  done processing event #" << n_evt << ", run #" << n_run
                     << " on slot " << thisFinishedEvtContext->slot() << ",  "
                     << m_proc << " events processed so far  <<<===");
      else
	ATH_MSG_INFO("  ===>>>  done processing event #" << n_evt << ", run #" << n_run
	             << " on slot " << thisFinishedEvtContext->slot() << ",  "
	             << m_nev << " events read and " << m_proc
	             << " events processed so far <<<===");
      std::ofstream outfile( "eventLoopHeartBeat.txt");
      if ( !outfile ) {
	ATH_MSG_ERROR(" unable to open: eventLoopHeartBeat.txt");
	fail = true;
	delete thisFinishedEvtContext;
	continue;
      } else {
	outfile << "  done processing event #" << n_evt << ", run #" << n_run
		<< " " << m_nev << " events read so far  <<<===" << std::endl;
	outfile.close();
      }
    }
*/
    ATH_MSG_DEBUG("drainScheduler thisFinishedEvtContext: " << thisFinishedEvtContext);


    m_incidentSvc->fireIncident(Incident(name(),
                                         IncidentType::EndProcessing,
                                         *thisFinishedEvtContext));

    delete thisFinishedEvtContext;

  }

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return (  fail ? -1 : 1 );
}

// ==============================================================================
StatusCode HltEventLoopMgr::clearWBSlot(int evtSlot) const
{
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  StatusCode sc = m_whiteboard->clearStore(evtSlot);
  if( !sc.isSuccess() )  {
    ATH_MSG_WARNING("Clear of Event data store failed");
  }
  ATH_MSG_VERBOSE("end of " << __FUNCTION__ << ", returning m_whiteboard->freeStore(evtSlot=" << evtSlot << ")");
  return m_whiteboard->freeStore(evtSlot);
}
