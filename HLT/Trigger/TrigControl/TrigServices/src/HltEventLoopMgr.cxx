/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi includes
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IThreadPoolSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IAlgResourcePool.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "GaudiKernel/IScheduler.h"
#include "GaudiKernel/IAlgExecStateSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"

// Athena includes
#include "AthenaKernel/EventContextClid.h"
#include "StoreGate/StoreGateSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

// Trigger includes
#include "TrigServices/HltEventLoopMgr.h"
#include "TrigSORFromPtreeHelper.h"
#include "TrigCOOLUpdateHelper.h"

// TDAQ includes
#include "hltinterface/DataCollector.h"
#include "eformat/write/FullEventFragment.h"

#include "owl/time.h"

#define ST_WHERE "HltEventLoopMgr::" << __func__ << "(): "

using namespace boost::property_tree;
using SOR = TrigSORFromPtreeHelper::SOR;


// =============================================================================
// Standard constructor
// =============================================================================
HltEventLoopMgr::HltEventLoopMgr(const std::string& name, ISvcLocator* svcLoc)
: MinimalEventLoopMgr(name, svcLoc),
  m_incidentSvc("IncidentSvc", name),
  m_evtStore("StoreGateSvc", name),
  m_detectorStore("DetectorStore", name),
  m_inputMetaDataStore("StoreGateSvc/InputMetaDataStore", name),
  m_robDataProviderSvc("ROBDataProviderSvc", name),
  m_THistSvc("THistSvc", name),
  m_evtSelector("EvtSel", name),
  m_coolHelper("TrigCOOLUpdateHelper", this),
  m_detector_mask(0xffffffff, 0xffffffff, 0, 0),
  m_nevt(0),
  m_threadPoolSize(-1)
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  
  declareProperty("JobOptionsType",           m_jobOptionsType="NONE");
  declareProperty("ApplicationName",          m_applicationName="None");
  declareProperty("PartitionName",            m_partitionName="None");
  declareProperty("enabledROBs",              m_enabledROBs);
  declareProperty("enabledSubDetectors",      m_enabledSubDetectors);
  declareProperty("EvtSel",                   m_evtSelector);
  declareProperty("CoolUpdateTool",           m_coolHelper);
  declareProperty("SchedulerSvc",             m_schedulerName="ForwardSchedulerSvc",
                  "Name of the scheduler to be used");
  declareProperty("WhiteboardSvc",            m_whiteboardName="EventDataSvc",
                  "Name of the Whiteboard to be used");

  verbose() << "end of " << __FUNCTION__ << endmsg;
}

// =============================================================================
// Standard destructor
// =============================================================================
HltEventLoopMgr::~HltEventLoopMgr()
{
}

// =============================================================================
// Implementation of IInterface::queryInterface
// =============================================================================
StatusCode HltEventLoopMgr::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  if(!ppvInterface)
      return StatusCode::FAILURE;

  if(ITrigEventLoopMgr::interfaceID().versionMatch(riid))
    *ppvInterface = static_cast<ITrigEventLoopMgr*>(this);
  else
    return MinimalEventLoopMgr::queryInterface(riid, ppvInterface);

  addRef();
  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IService::initalize
// =============================================================================
StatusCode HltEventLoopMgr::initialize()
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  
  //----------------------------------------------------------------------------
  // Initialize the base class
  //----------------------------------------------------------------------------
  StatusCode sc = MinimalEventLoopMgr::initialize();
  
  
  info() << " ---> HltEventLoopMgr = " << name()
         << " initialize - package version " << PACKAGE_VERSION << endmsg;

  if (sc.isFailure()) {
    error() << "Failed to initialize base class MinimalEventLoopMgr" << endmsg;
    return sc;
  }
  
  //----------------------------------------------------------------------------
  // Setup properties
  //----------------------------------------------------------------------------
  
  // read DataFlow configuration properties
  updateDFProps();
  
  // JobOptions type
  SmartIF<IProperty> propMgr(Gaudi::createApplicationMgr());
  if (propMgr.isValid()) {
    try {
      if (m_jobOptionsType.assign( propMgr->getProperty("JobOptionsType") ))
        debug() << " ---> Read from DataFlow configuration: " << m_jobOptionsType << endmsg;
    }
    catch (...) {
      warning() << "Could not set Property '" << m_jobOptionsType.name() << "' from DataFlow." << endmsg;
    }

  }
  else {
    warning() << "Error retrieving IProperty interface of ApplicationMgr"  << endmsg;
  }

  // print properties
  info() << " ---> ApplicationName        = " << m_applicationName << endmsg;
  info() << " ---> PartitionName          = " << m_partitionName << endmsg;
  info() << " ---> JobOptionsType         = " << m_jobOptionsType << endmsg;
  
  info() << " ---> Enabled ROBs: size = " << m_enabledROBs.value().size();
  if (m_enabledROBs.value().size() == 0)
    info() << ". No check will be performed";
  info() << endmsg;

  info() << " ---> Enabled Sub Detectors: size = " << m_enabledSubDetectors.value().size();
  if (m_enabledSubDetectors.value().size() == 0)
    info() << ". No check will be performed" << endmsg;
  info() << endmsg;

  //----------------------------------------------------------------------------
  // Setup stuff for hive - taken from AthenaHiveEventLoopMgr
  //----------------------------------------------------------------------------

  m_whiteboard = serviceLocator()->service(m_whiteboardName);
  if( !m_whiteboard.isValid() )  {
    fatal() << "Error retrieving " << m_whiteboardName << " interface IHiveWhiteBoard." << endmsg;
    return StatusCode::FAILURE;
  }
  debug() << "initialised " << m_whiteboardName << " interface IHiveWhiteBoard." << endmsg;

  debug() << "commented out scheduler initialisation was here, now it's moved to hltUpdateAfterFork" << endmsg;
  /*
  m_schedulerSvc = serviceLocator()->service(m_schedulerName);
  if ( !m_schedulerSvc.isValid()){
    fatal() << "Error retrieving " << m_schedulerName << " interface ISchedulerSvc." << endmsg;
    return StatusCode::FAILURE;    
  }
  debug() << "initialised " << m_schedulerName << " interface ISchedulerSvc." << endmsg;
  */
  m_algResourcePool = serviceLocator()->service("AlgResourcePool");
  if( !m_algResourcePool.isValid() ) {
    fatal() << "Error retrieving AlgResourcePool" << endmsg;
    return StatusCode::FAILURE;
  }
  debug() << "initialised AlgResourcePool" << endmsg;

  m_aess = serviceLocator()->service("AlgExecStateSvc");
  if( !m_aess.isValid() ) {
    fatal() << "Error retrieving AlgExecStateSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  debug() << "initialised AlgExecStateSvc" << endmsg;


  //----------------------------------------------------------------------------
  // Setup the IncidentSvc
  //----------------------------------------------------------------------------
  sc = m_incidentSvc.retrieve();
  if(sc.isFailure()) {
    fatal() << "Error retrieving IncidentSvc " + m_incidentSvc << endmsg;
    return sc;
  }
  
  //----------------------------------------------------------------------------
  // Setup the StoreGateSvc
  //----------------------------------------------------------------------------
  sc = m_evtStore.retrieve();
  if(sc.isFailure()) {
    fatal() << "Error retrieving StoreGateSvc " + m_evtStore << endmsg;
    return sc;
  }

  //----------------------------------------------------------------------------
  // Setup the DetectorStore
  //----------------------------------------------------------------------------
  sc = m_detectorStore.retrieve();
  if(sc.isFailure()) {
    fatal() << "Error retrieving DetectorStore " + m_detectorStore << endmsg;
    return sc;
  }

  //----------------------------------------------------------------------------
  // Setup the InputMetaDataStore
  //----------------------------------------------------------------------------
  sc = m_inputMetaDataStore.retrieve();
  if(sc.isFailure()) {
    fatal() << "Error retrieving InputMetaDataStore" + m_inputMetaDataStore << endmsg;
    return sc;
  }

  //----------------------------------------------------------------------------
  // Setup the ROBDataProviderSvc 
  //----------------------------------------------------------------------------
  sc = m_robDataProviderSvc.retrieve();
  if(sc.isFailure()) {
    fatal() << "Error retrieving ROBDataProviderSvc " + m_robDataProviderSvc << endmsg;
    return sc;
  }

  //----------------------------------------------------------------------------
  // Setup the Histogram Service
  //----------------------------------------------------------------------------
  sc = m_THistSvc.retrieve();
  if(sc.isFailure()) {
    fatal() << "Error retrieving THistSvc " + m_THistSvc << endmsg;
    return sc;
  }

  //----------------------------------------------------------------------------
  // Setup the Event Selector
  //----------------------------------------------------------------------------
  sc = m_evtSelector.retrieve();
  if(sc.isFailure()) {
    fatal() << "Error retrieving EvtSel " + m_THistSvc << endmsg;
    return sc;
  }

  //----------------------------------------------------------------------------
  // Setup the COOL helper
  //----------------------------------------------------------------------------
  if (m_coolHelper.retrieve().isFailure()) {
    fatal() << "Error retrieving" << m_coolHelper << endmsg;
    return StatusCode::FAILURE;
  }

  //----------------------------------------------------------------------------
  // Setup the AlgContextSvc (optional)
  //----------------------------------------------------------------------------
  if (service("AlgContextSvc", m_algContextSvc, /*createIf=*/ true).isFailure()) {
    m_algContextSvc = 0;
    debug() << "No AlgContextSvc available" << endmsg;
  }

  //----------------------------------------------------------------------------
  // Setup the ThreadPoolSvc (optional)
  //----------------------------------------------------------------------------
  if (service("ThreadPoolSvc", m_threadPoolSvc, /*createIf=*/ false).isFailure()) {
    m_threadPoolSvc = 0;
    debug() << "No ThreadPoolSvc available" << endmsg;
  }
  else {
    m_threadPoolSize = m_threadPoolSvc->poolSize();
    debug() << "ThreadPoolSvc available with pool size " << m_threadPoolSize << endmsg;
  }

  //----------------------------------------------------------------------------
  // Setup the HLT Histogram Service when configured
  //----------------------------------------------------------------------------
  if ( &*m_THistSvc ) {
    m_hltTHistSvc = SmartIF<IHltTHistSvc>( &*m_THistSvc );
    if (m_hltTHistSvc.isValid())
      info() << "A THistSvc implementing the HLT interface IHltTHistSvc was found." << endmsg;
    else
      info() << "No THistSvc implementing the HLT interface IHltTHistSvc was found." << endmsg;
  }

  //----------------------------------------------------------------------------
  // Setup the HLT ROB Data Provider Service when configured
  //----------------------------------------------------------------------------
  if ( &*m_robDataProviderSvc ) {
    m_hltROBDataProviderSvc = SmartIF<ITrigROBDataProviderSvc>( &*m_robDataProviderSvc );
    if (m_hltROBDataProviderSvc.isValid())
      info() << "A ROBDataProviderSvc implementing the HLT interface ITrigROBDataProviderSvc was found." << endmsg;
    else
      info() << "No ROBDataProviderSvc implementing the HLT interface ITrigROBDataProviderSvc was found." << endmsg;
  }

  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of ITrigEventLoopMgr::prepareForRun
// =============================================================================
StatusCode HltEventLoopMgr::prepareForRun(const ptree & pt)
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  try
  {
    // (void)TClass::GetClass("vector<unsigned short>"); // preload to overcome an issue with dangling references in serialization
    // (void)TClass::GetClass("vector<unsigned long>");
    
    // do the necessary resets
    // internalPrepareResets() was here
    StatusCode sc = clearTemporaryStores();
    if (sc.isFailure()) {
      error() << "Clearing temporary stores failed" << endmsg;
      return sc;
    }
    
    const SOR* sor;
    // update SOR in det store and get it back
    if(!(sor = processRunParams(pt)))
      return StatusCode::FAILURE;
      
    auto& soral = getSorAttrList(sor);
    
    updInternal(soral);     // update internally kept info
    /*
    updMetadaStore(soral);  // update metadata store
    
    const EventInfo * evinfo;
    if(updMagField(pt).isFailure() ||     // update mag field when appropriate
       updHLTConfigSvc().isFailure() ||   // update config svc when appropriate
       resetCoolValidity().isFailure() || // reset selected proxies/IOV folders
       prepXAODEventInfo().isFailure() || // update xAOD event data in SG
       !(evinfo = prepEventInfo()))       // update old event data in SG
      return StatusCode::FAILURE;

    bookAllHistograms();

    verbose() << "end of " << __FUNCTION__ << endmsg;
    if(prepareAlgs(*evinfo).isSuccess())
      return StatusCode::SUCCESS;
    */
    
    //-------------------------------------------------------------------------
    // Shut down the hive pool thread
    //-------------------------------------------------------------------------
    // if (m_threadPoolSvc) {
    //   debug() << "Terminating thread pool" << endmsg;
    //   m_threadPoolSvc->terminatePool();
    // }

    verbose() << "end of " << __FUNCTION__ << endmsg;
    return StatusCode::SUCCESS;
  }
  catch(const ptree_bad_path & e)
  {
    error() << ST_WHERE << "Bad ptree path: \""
            << e.path<ptree::path_type>().dump()
            << "\" - " << e.what() << endmsg;
  }
  catch(const ptree_bad_data & e)
  {
    error() << ST_WHERE << "Bad ptree data: \""
            << e.data<ptree::data_type>()
            << "\" - " << e.what() << endmsg;
  }
  catch(const std::runtime_error& e)
  {
    error() << ST_WHERE << "Runtime error: " << e.what() << endmsg;
  }

  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::FAILURE;
}

// =============================================================================
// Implementation of ITrigEventLoopMgr::hltUpdateAfterFork
// =============================================================================
StatusCode HltEventLoopMgr::hltUpdateAfterFork(const ptree & pt)
{
  verbose() << "start of " << __FUNCTION__ << endmsg;

  debug() << "trying to initialise the scheduler after forking" << endmsg;
  m_schedulerSvc = serviceLocator()->service(m_schedulerName);
  if ( !m_schedulerSvc.isValid()){
    fatal() << "Error retrieving " << m_schedulerName << " interface ISchedulerSvc." << endmsg;
    return StatusCode::FAILURE;    
  }
  debug() << "initialised " << m_schedulerName << " interface ISchedulerSvc." << endmsg;

  //-------------------------------------------------------------------------
  // Reopen the hive pool thread
  //-------------------------------------------------------------------------
  // if (m_threadPoolSvc) {
  //   debug() << "Reopening thread pool with size " << m_threadPoolSize << " after fork" << endmsg;
  //   m_threadPoolSvc->initPool(m_threadPoolSize);
  // }
  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of ITrigEventLoopMgr::processRoIs
// DUE TO BE REMOVED
// =============================================================================
StatusCode HltEventLoopMgr::processRoIs(
             const std::vector<eformat::ROBFragment<const uint32_t*> >& l1_result,
             hltinterface::HLTResult& hlt_result,
             const hltinterface::EventId& evId)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of ITrigEventLoopMgr::timeOutReached
// =============================================================================
StatusCode HltEventLoopMgr::timeOutReached(const boost::property_tree::ptree& pt)
{
  verbose() << "dummy implementation of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IEventProcessor::executeRun
// =============================================================================
StatusCode HltEventLoopMgr::executeRun(int maxevt)
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  StatusCode sc = nextEvent(maxevt);
  if (sc.isFailure()) {
    error() << "Event loop failed" << endmsg;
    // some special cleanup here?
  }
  
  // do some cleanup here
  verbose() << "end of " << __FUNCTION__ << endmsg;
  return sc;
}

// =============================================================================
// Implementation of IEventProcessor::nextEvent
// maxevt is not used - we always want to process all events delivered
// =============================================================================
StatusCode HltEventLoopMgr::nextEvent(int /*maxevt*/)
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  
  info() << "Starting loop on events" << endmsg;
  bool loop_ended = false;
  bool events_available = true; // DataCollector has more events
  
  while (!loop_ended) {
    debug() << "Free slots = " << m_schedulerSvc->freeSlots() << endmsg;
    if (m_schedulerSvc->freeSlots()>0 && events_available) {
      debug() << "Free slots = " << m_schedulerSvc->freeSlots() << ". Reading the next event." << endmsg;
      
      //-----------------------------------------------------------------------
      // Get the next event
      //-----------------------------------------------------------------------
      eformat::write::FullEventFragment l1r; // can we allocate new each time?
      // should the getNext call be protected by try{} and catch()?
      hltinterface::DataCollector::Status status = hltinterface::DataCollector::instance()->getNext(l1r);
      if (status == hltinterface::DataCollector::Status::STOP) {
        debug() << "No more events available, the loop will finish when all events on the fly are processed" << endmsg;
        events_available = false;
      }
      else if (status != hltinterface::DataCollector::Status::OK) {
        error() << "Unhandled return Status " << static_cast<int>(status) << " from DataCollector::getNext" << endmsg;
        // continue running?
      }

      //-----------------------------------------------------------------------
      // Start processing the new event
      //-----------------------------------------------------------------------
      ++m_nevt;
      EventContext* evtContext(nullptr);
      if (createEventContext(evtContext).isFailure()){
        error() << "Failed to create event context" << endmsg;
        // needs careful handling, make a framework error, send event to debug
        continue;
      }

      // do we need this duplication? it's selected already in createEventContext
      if (m_whiteboard->selectStore(evtContext->slot()).isFailure()){
        error() << "Slot " << evtContext->slot() << " could not be selected for the WhiteBoard" << endmsg;
        // needs careful handling, make a framework error, send event to debug
        continue;
      }

      EventID* eventID = new EventID(m_currentRun, // l1r.run_no(),
                                     l1r.global_id(),
                                     l1r.bc_time_seconds(),
                                     l1r.bc_time_nanoseconds(),
                                     l1r.lumi_block(),
                                     l1r.bc_id(),
                                     std::get<0>(m_detector_mask),
                                     std::get<1>(m_detector_mask),
                                     std::get<2>(m_detector_mask),
                                     std::get<3>(m_detector_mask));
      TriggerInfo* triggerInfo = new TriggerInfo(0, // FIXME
                                                 l1r.lvl1_id(),
                                                 l1r.lvl1_trigger_type(),
                                                 std::vector<uint32_t>(
                                                   l1r.lvl1_trigger_info(),
                                                   l1r.lvl1_trigger_info()+l1r.nlvl1_trigger_info())
                                                );
      EventInfo* eventInfo = new EventInfo(eventID, new EventType(), triggerInfo);

      debug() << "recording EventInfo " << *eventID << " in " << m_evtStore->name() << endmsg;
      
      if(m_evtStore->record(eventInfo, "EventInfo").isFailure()) {
        error() << "Error recording EventInfo object in SG" << endmsg;
        // needs careful handling, make a framework error, send event to debug
        continue;
      }

      evtContext->setEventID( *static_cast<EventIDBase*>(eventID) );
      evtContext->template getExtension<Atlas::ExtendedEventContext>()->setConditionsRun(m_currentRun);
      Gaudi::Hive::setCurrentContext(*evtContext);
      // Record EventContext in current whiteboard
      if (m_evtStore->record(std::make_unique<EventContext> (*evtContext), "EventContext").isFailure()) {
        error() << "Error recording event context object" << endmsg;
        // needs careful handling, make a framework error, send event to debug
        continue;
      }

      if (executeEvent(evtContext).isFailure()) {
        error() << "Error processing event" << endmsg;
        // needs careful handling, make a framework error, send event to debug
        continue;
      }

    }
    else {
      debug() << "No free slots or no more events to process - draining the scheduler" << endmsg;
      int ir = drainScheduler();
      if (ir<0) {
        error() << "Error draining scheduler" << endmsg;
        continue;
      }
      else if (ir==0 && !events_available) {
        info() << "All events processed, finalising the event loop" << endmsg;
        loop_ended = true;
      }
    }
  }

  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of IEventProcessor::executeEvent
// =============================================================================
StatusCode HltEventLoopMgr::executeEvent(void* pEvtContext)
{
  verbose() << "start of " << __FUNCTION__ << endmsg;

  EventContext* evtContext = static_cast<EventContext*>(pEvtContext);
  if (!evtContext) {
    error() << "Failed to cast the call parameter to EventContext*" << endmsg;
    return StatusCode::FAILURE;
  }
  
  resetTimeout(Athena::Timeout::instance(*evtContext));

  // Now add event to the scheduler
  debug() << "Adding event " << evtContext->evt() << ", slot " << evtContext->slot() << " to the scheduler" << endmsg;
  StatusCode addEventStatus = m_schedulerSvc->pushNewEvent(evtContext);

  // If this fails, we need to wait for something to complete
  if (!addEventStatus.isSuccess()){
    error() << "Failed adding event " << evtContext->evt() << ", slot " << evtContext->slot()
            << " to the scheduler" << endmsg;
    return StatusCode::FAILURE;
  }
  
  verbose() << "end of " << __FUNCTION__ << endmsg;
  return StatusCode::SUCCESS;
}

// =============================================================================
void HltEventLoopMgr::updateDFProps()
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  ServiceHandle<IJobOptionsSvc> p_jobOptionsSvc("JobOptionsSvc", name());
  if ((p_jobOptionsSvc.retrieve()).isFailure()) {
    warning() << "Could not find JobOptionsSvc to set DataFlow properties" << endmsg;
  } else {
    auto dfprops = p_jobOptionsSvc->getProperties("DataFlowConfig");

    // Application name
    auto pname = "DF_ApplicationName";
    const auto * prop = Gaudi::Utils::getProperty(dfprops, pname);
    if(prop && m_applicationName.assign(*prop)) {
      debug() << " ---> Read from DataFlow configuration: " << m_applicationName << endmsg;
    } else {
      warning() << "Could not set Property '" << pname << "' from DataFlow." << endmsg;
    }

    // Partition name
    pname = "DF_PartitionName";
    prop = Gaudi::Utils::getProperty(dfprops, pname);
    if (prop && m_partitionName.assign(*prop)) {
      debug() << " ---> Read from DataFlow configuration: " << m_partitionName << endmsg;
    } else {
      warning() << "Could not set Property '" << pname << "' from DataFlow." << endmsg;
    }

    // get the list of enabled ROBs
    pname = "DF_Enabled_ROB_IDs";
    prop = Gaudi::Utils::getProperty(dfprops, pname);
    if (prop && m_enabledROBs.assign(*prop)) {
      debug() << " ---> Read from DataFlow configuration: "
              << m_enabledROBs.value().size() << " enabled ROB IDs." << endmsg;
    } else {
      // this is only info, because it is normal in athenaHLT
      info() << "Could not set Property '" << pname << "' from DataFlow." << endmsg;
    }

    // get the list of enabled Sub Detectors
    pname = "DF_Enabled_SubDet_IDs";
    prop = Gaudi::Utils::getProperty(dfprops, pname);
    if (prop && m_enabledSubDetectors.assign(*prop)) {
      debug() << " ---> Read from DataFlow configuration: "
              << m_enabledSubDetectors.value().size() << " enabled Sub Detector IDs." << endmsg;
    } else {
      // this is only info, because it is normal in athenaHLT
      info() << "Could not set Property '" << pname << "' from DataFlow." << endmsg;
    }
  }

  p_jobOptionsSvc.release().ignore();
  verbose() << "end of " << __FUNCTION__ << endmsg;
}

// =============================================================================
const SOR* HltEventLoopMgr::processRunParams(const ptree & pt)
{
  verbose() << "start of " << __FUNCTION__ << endmsg;
  // update the run number
  m_currentRun = pt.get<uint32_t>("RunParams.run_number");

  // need to provide an event context extended with a run number, down the line passed to IOVDBSvc::signalBeginRun
  EventContext runStartEventContext = {}; // with invalid evt number and slot number
  runStartEventContext.setExtension(Atlas::ExtendedEventContext(m_evtStore->hiveProxyDict(), m_currentRun));
    
  // Fill SOR parameters from the ptree
  TrigSORFromPtreeHelper sorhelp{msgStream()};
  const SOR* sor = sorhelp.fillSOR(pt.get_child("RunParams"),runStartEventContext);
  if(!sor) {
    error() << ST_WHERE << "setup of SOR from ptree failed" << endmsg;
  }

  verbose() << "end of " << __FUNCTION__ << endmsg;
  return sor;
}

//=============================================================================
void HltEventLoopMgr::updInternal(const coral::AttributeList & sor_attrlist)
{
  auto detMaskFst = sor_attrlist["DetectorMaskFst"].data<unsigned long long>();
  auto detMaskSnd = sor_attrlist["DetectorMaskSnd"].data<unsigned long long>();
  updDetMask({detMaskFst, detMaskSnd});

  // auto sorTime = sor_attrlist["SORTime"].data<unsigned long long>();
  // updSorTime(sorTime);

  if(msgLevel() <= MSG::DEBUG)
  {
    // save current stream flags for later reset
    // cast needed (stream thing returns long, but doesn't take it back)
    auto previous_stream_flags = static_cast<std::ios::fmtflags>(msgStream().flags());
    debug() << ST_WHERE
            << "Full detector mask (128 bits) = 0x"
            << MSG::hex << std::setfill('0')
            << std::setw(8) << std::get<3>(m_detector_mask)
            << std::setw(8) << std::get<2>(m_detector_mask)
            << std::setw(8) << std::get<1>(m_detector_mask)
            << std::setw(8) << std::get<0>(m_detector_mask) << endmsg;
    msgStream().flags(previous_stream_flags);

    // debug() << ST_WHERE << "sorTimeStamp[0] [sec] = " << m_sorTime_stamp[0] << endmsg;
    // debug() << ST_WHERE << "sorTimeStamp[1] [ns]  = " << m_sorTime_stamp[1] << endmsg;
  }
}

//==============================================================================
StatusCode HltEventLoopMgr::clearTemporaryStores()
{
  //----------------------------------------------------------------------------
  // Clear the event store, if used in the event loop
  //----------------------------------------------------------------------------
  auto sc = m_evtStore->clearStore();
  debug() << ST_WHERE << "clear of Event Store " << sc << endmsg;
  if(sc.isFailure()) {
    error() << ST_WHERE << "clear of Event Store failed" << endmsg;
    return sc;
  }

  //----------------------------------------------------------------------------
  // Clear the InputMetaDataStore
  //----------------------------------------------------------------------------
  sc = m_inputMetaDataStore->clearStore();
  debug() << ST_WHERE << "clear of InputMetaDataStore store " << sc << endmsg;
  if(sc.isFailure())
    error() << ST_WHERE << "clear of InputMetaDataStore failed" << endmsg;

  return sc;
}

//=============================================================================
void HltEventLoopMgr::updDetMask(const std::pair<uint64_t, uint64_t>& dm)
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

//==============================================================================
const coral::AttributeList& HltEventLoopMgr::getSorAttrList(const SOR* sor) const
{
  if(sor->size() != 1)
  {
    // This branch should never be entered (the CondAttrListCollection
    // corresponding to the SOR should contain one single AttrList). Since
    // that's required by code ahead but not checked at compile time, we
    // explicitly guard against any potential future mistake with this check
    error() << ST_WHERE << "Wrong SOR: size = " << sor->size() << endmsg;
    throw std::runtime_error("SOR record should have one and one only attribute list, but it has " + sor->size());
  }

  const auto & soral = sor->begin()->second;
  printSORAttrList(soral, info());
  return soral;
}

//==============================================================================
void HltEventLoopMgr::printSORAttrList(const coral::AttributeList& atr, MsgStream& log) const
{
  unsigned long long sorTime_ns(atr["SORTime"].data<unsigned long long>());

  // Human readable format of SOR time if available
  time_t sorTime_sec = sorTime_ns/1000000000;
  const auto sorTime_readable = OWLTime(sorTime_sec);

  log << "SOR parameters:" << endmsg;
  log << "   RunNumber        = "
      << atr["RunNumber"].data<unsigned int>() << endmsg;
  log << "   SORTime [ns]     = "
      << sorTime_ns << " (" << sorTime_readable << ") " << endmsg;

  // save current stream flags for later reset
  // cast needed (stream thing returns long, but doesn't take it back)
  auto previous_stream_flags = static_cast<std::ios::fmtflags>(log.flags());
  auto dmfst = atr["DetectorMaskFst"].data<unsigned long long>();
  auto dmsnd = atr["DetectorMaskSnd"].data<unsigned long long>();
  log << MSG::hex << std::setfill('0');
  log << "   DetectorMaskFst     = 0x" << std::setw(16) << dmfst << endmsg;
  log << "   DetectorMaskSnd     = 0x" << std::setw(16) << dmsnd << endmsg;
  log << "   (complete DetectorMask = 0x"
      << std::setw(16) << dmfst << std::setw(16) << dmsnd << ")" << endmsg;
  // reset stream flags
  log.flags(previous_stream_flags);

  log << "   RunType          = "
      << atr["RunType"].data<std::string>() << endmsg;
  log << "   RecordingEnabled = "
      << (atr["RecordingEnabled"].data<bool>() ? "true" : "false") << endmsg;
}

//==============================================================================
StatusCode HltEventLoopMgr::createEventContext(EventContext*& evtContext) const
{
  evtContext = new EventContext(m_nevt, m_whiteboard->allocateStore(m_nevt));
  
  m_aess->reset(*evtContext);

  StatusCode sc = m_whiteboard->selectStore(evtContext->slot());
  if (sc.isFailure()){
    warning() << "Slot " << evtContext->slot() << " could not be selected for the WhiteBoard" << endmsg;
  } else {
    evtContext->setExtension( Atlas::ExtendedEventContext( m_evtStore->hiveProxyDict() ) );
    debug() << "created EventContext, num: " << evtContext->evt()  << "  in slot: " << evtContext->slot() << endmsg;
  }
  return sc;
}

//==============================================================================
int HltEventLoopMgr::drainScheduler() const
{
  verbose() << "start of " << __FUNCTION__ << endmsg;

  // method copied from AthenaHiveEventLoopMgr

  StatusCode sc(StatusCode::SUCCESS);
    
  // maybe we can do better
  std::vector<EventContext*> finishedEvtContexts;

  EventContext* finishedEvtContext(nullptr);

  // Here we wait not to loose cpu resources
  // debug() << "drainScheduler: [" << finishedEvts << "] Waiting for a context" << endmsg;
  debug() << "drainScheduler: Waiting for a context" << endmsg;
  sc = m_schedulerSvc->popFinishedEvent(finishedEvtContext);

  // We got past it: cache the pointer
  if (sc.isSuccess()){
    debug() << "drainScheduler: scheduler not empty: Context " 
	    << finishedEvtContext << endmsg;
    finishedEvtContexts.push_back(finishedEvtContext);
  } else{
    // no more events left in scheduler to be drained
    debug() << "drainScheduler: scheduler empty" << endmsg;
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
      fatal() << "Detected nullptr ctxt while clearing WB!"<< endmsg;
      fail = true;
      continue;
    }

    if (m_aess->eventStatus(*thisFinishedEvtContext) != EventStatus::Success) {
      fatal() << "Failed event detected on " << thisFinishedEvtContext 
              << " w/ fail mode: "
              << m_aess->eventStatus(*thisFinishedEvtContext) << endmsg;
      delete thisFinishedEvtContext;
      fail = true;
      continue;
    }
    
    EventID::number_type n_run(0);
    EventID::number_type n_evt(0);

    const EventInfo* pEvent(0);
    if (m_whiteboard->selectStore(thisFinishedEvtContext->slot()).isSuccess()) {
      if (m_evtStore->retrieve(pEvent).isFailure()) {
        error() << "DrainSched: unable to get EventInfo obj" << endmsg;
        delete thisFinishedEvtContext;
        fail = true;
        continue;
      } else {
        n_run = pEvent->event_ID()->run_number();
        n_evt = pEvent->event_ID()->event_number();
      }
    } else {
      error() << "DrainSched: unable to select store " << thisFinishedEvtContext->slot() << endmsg;
      delete thisFinishedEvtContext;
      fail = true;
      continue;
    }

    // m_incidentSvc->fireIncident(Incident(name(), IncidentType::EndEvent,
    // 					 *thisFinishedEvtContext ));

    // create a dummy HLT result and call eventDone

    debug() << "Creating a dummy HLT Result" << endmsg;
    eformat::write::FullEventFragment hltrFragment;
    hltrFragment.global_id(pEvent->event_ID()->event_number());
    hltrFragment.bc_time_seconds(pEvent->event_ID()->time_stamp());
    hltrFragment.bc_time_nanoseconds(pEvent->event_ID()->time_stamp_ns_offset());
    hltrFragment.run_no(pEvent->event_ID()->run_number());
    hltrFragment.bc_id(pEvent->event_ID()->bunch_crossing_id());
    hltrFragment.lvl1_id(pEvent->trigger_info()->extendedLevel1ID());
    hltrFragment.lvl1_trigger_type(pEvent->trigger_info()->level1TriggerType());
    hltrFragment.lvl1_trigger_info(pEvent->trigger_info()->level1TriggerInfo().size(),
                                   pEvent->trigger_info()->level1TriggerInfo().data());

    const eformat::write::node_t* top = hltrFragment.bind();
    auto hltrFragmentSize = hltrFragment.size_word();
    auto hltrPtr = std::make_unique<uint32_t[]>(hltrFragmentSize);
    auto copiedSize = eformat::write::copy(*top,hltrPtr.get(),hltrFragmentSize);
    if(copiedSize!=hltrFragmentSize){
      error() << "Event serialization failed" << endmsg;
      // missing error handling
    }
    debug() << "Sending the HLT result to DataCollector" << endmsg;
    hltinterface::DataCollector::instance()->eventDone(std::move(hltrPtr));

    debug() << "Clearing slot " << thisFinishedEvtContext->slot() 
            << " (event " << thisFinishedEvtContext->evt()
            << ") of the whiteboard" << endmsg;
    
    StatusCode sc = clearWBSlot(thisFinishedEvtContext->slot());
    if (!sc.isSuccess()) {
      error() << "Whiteboard slot " << thisFinishedEvtContext->slot() 
	      << " could not be properly cleared";
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
        info() << "  ===>>>  done processing event #" << n_evt << ", run #" << n_run 
               << " on slot " << thisFinishedEvtContext->slot() << ",  "
               << m_proc << " events processed so far  <<<===" << endmsg;
      else 
	info() << "  ===>>>  done processing event #" << n_evt << ", run #" << n_run 
	       << " on slot " << thisFinishedEvtContext->slot() << ",  "          
	       << m_nev << " events read and " << m_proc 
	       << " events processed so far <<<===" << endmsg;
      std::ofstream outfile( "eventLoopHeartBeat.txt");
      if ( !outfile ) {
	error() << " unable to open: eventLoopHeartBeat.txt" << endmsg;
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
    debug() << "drainScheduler thisFinishedEvtContext: " << thisFinishedEvtContext << endmsg;
    
    
    m_incidentSvc->fireIncident(Incident(name(),
                                         IncidentType::EndProcessing,
                                         *thisFinishedEvtContext));

    delete thisFinishedEvtContext;

  }

  verbose() << "end of " << __FUNCTION__ << endmsg;
  return (  fail ? -1 : 1 );
}

//==============================================================================
StatusCode HltEventLoopMgr::clearWBSlot(int evtSlot) const {
  StatusCode sc = m_whiteboard->clearStore(evtSlot);
  if( !sc.isSuccess() )  {
    warning() << "Clear of Event data store failed" << endmsg;    
  }
  return m_whiteboard->freeStore(evtSlot);
}
