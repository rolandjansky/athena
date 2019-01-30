/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#define  ATHENASERVICES_ATHENAHIVEEVENTLOOPMGR_CPP

#include <cassert>
#include <ios>
#include <iostream>
#include <fstream> /* ofstream */
#include <iomanip>

#include "PersistentDataModel/AthenaAttributeList.h"
#include "AthenaKernel/ITimeKeeper.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "AthenaKernel/IAthenaEvtLoopPreSelectTool.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaKernel/EventContextClid.h"
#include "AthenaKernel/errorcheck.h"

#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/AppReturnCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"

#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

#include "AthenaHiveEventLoopMgr.h"

#include <GaudiKernel/IScheduler.h>
// External libraries
#include "tbb/tick_count.h"

#include "ClearStorePolicy.h"


//=========================================================================
// Standard Constructor
//=========================================================================
AthenaHiveEventLoopMgr::AthenaHiveEventLoopMgr(const std::string& nam, 
				       ISvcLocator* svcLoc)
  : MinimalEventLoopMgr(nam, svcLoc), 
    m_incidentSvc ( "IncidentSvc",  nam ), 
    m_eventStore( "StoreGateSvc", nam ), 
    m_evtSelector(0), m_evtContext(0),
    m_histoDataMgrSvc( "HistogramDataSvc",         nam ), 
    m_histoPersSvc   ( "HistogramPersistencySvc",  nam ), 
    m_activeStoreSvc ( "ActiveStoreSvc",           nam ),
    m_pITK(0), 
    m_currentRun(0), m_firstRun(true), m_tools(this), m_nevt(0), m_writeHists(false),
    m_nev(0), m_proc(0), m_useTools(false),m_doEvtHeartbeat(false),
    m_pEvent(nullptr),
    m_conditionsCleaner( "Athena::ConditionsCleanerSvc", nam )
{
  declareProperty("EvtSel", m_evtsel, 
		  "Name of Event Selector to use. If empty string (default) "
		  "take value from ApplicationMgr");
  declareProperty("HistogramPersistency", m_histPersName="",
		  "Histogram persistency technology to use: ROOT, HBOOK, NONE. "
		  "By default (empty string) get property value from "
		  "ApplicationMgr");
  declareProperty("TimeKeeper", m_timeKeeperName, 
		  "Name of TimeKeeper to use. NONE or empty string (default) "
		  "means no time limit control on event loop");
  m_timeKeeperName.declareUpdateHandler(&AthenaHiveEventLoopMgr::setupTimeKeeper, this);
  declareProperty("HistWriteInterval",    m_writeInterval=0 ,
		  "histogram write/update interval");
  declareProperty("FailureMode",          m_failureMode=1 , 
		  "Controls behaviour of event loop depending on return code of"
		  " Algorithms. 0: all non-SUCCESSes terminate job. "
		  "1: RECOVERABLE skips to next event, FAILURE terminates job "
		  "(DEFAULT). 2: RECOVERABLE and FAILURE skip to next events");
  declareProperty("EventPrintoutInterval", m_eventPrintoutInterval=1,
                  "Print event heartbeat printouts every m_eventPrintoutInterval events");
  declareProperty("ClearStorePolicy",
		  m_clearStorePolicy = "EndEvent",
		  "Configure the policy wrt handling of when the "
		  "'clear-the-event-store' event shall happen: at EndEvent "
		  "(default as it is makes things easier for memory management"
		  ") or at BeginEvent (easier e.g. for interactive use)");
  declareProperty("PreSelectTools",m_tools,"AlgTools for event pre-selection")->
    declareUpdateHandler( &AthenaHiveEventLoopMgr::setupPreSelectTools, this ); ;
  
  declareProperty("SchedulerSvc", m_schedulerName="ForwardSchedulerSvc",
                  "Name of the scheduler to be used");
  
  declareProperty("WhiteboardSvc", m_whiteboardName="EventDataSvc",
		  "Name of the Whiteboard to be used");

  declareProperty("EventStore", m_eventStore);

  declareProperty("FakeLumiBlockInterval", m_flmbi = 0,
                  "Event interval at which to increment lumiBlock# when "
                  "creating events without an EventSelector. Zero means " 
                  "don't increment it");
  declareProperty("FakeTimestampInterval", m_timeStampInt = 1,
                  "timestamp interval between events when creating Events "
                  "without an EventSelector");

  m_scheduledStop = false;

}

//=========================================================================
// Standard Destructor
//=========================================================================
AthenaHiveEventLoopMgr::~AthenaHiveEventLoopMgr()   
{
}

//=========================================================================
// implementation of IAppMgrUI::initalize
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::initialize()    
{

  info() << "Initializing " << name()
         << " - package version " << PACKAGE_VERSION << endmsg ;
 

  StatusCode sc = MinimalEventLoopMgr::initialize();
  if ( !sc.isSuccess() ) 
  {
    error() << "Failed to initialize base class MinimalEventLoopMgr"
            << endmsg;
    return sc;
  } 

//-------------------------------------------------------------------------
// Setup stuff for hive
//-------------------------------------------------------------------------

  m_whiteboard = serviceLocator()->service(m_whiteboardName);
  if( !m_whiteboard.isValid() )  {
    fatal() << "Error retrieving " << m_whiteboardName << " interface IHiveWhiteBoard." 
	    << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_schedulerSvc = serviceLocator()->service(m_schedulerName);
  if ( !m_schedulerSvc.isValid()){
    fatal() << "Error retrieving SchedulerSvc interface ISchedulerSvc." 
	    << endmsg;
    return StatusCode::FAILURE;    
  }
  // Setup algorithm resource pool
  m_algResourcePool = serviceLocator()->service("AlgResourcePool");
  if( !m_algResourcePool.isValid() ) {
    fatal() << "Error retrieving AlgResourcePool" << endmsg;
    return StatusCode::FAILURE;
  }

  m_aess = serviceLocator()->service("AlgExecStateSvc");
  if( !m_aess.isValid() ) {
    fatal() << "Error retrieving AlgExecStateSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  sc = m_eventStore.retrieve();
  if( !sc.isSuccess() )  
  {
    fatal() << "Error retrieving pointer to StoreGateSvc"
            << endmsg;
    return sc;
  }

//--------------------------------------------------------------------------
// Get the references to the services that are needed by the ApplicationMgr 
// itself
//--------------------------------------------------------------------------
  sc = m_incidentSvc.retrieve();
  if( !sc.isSuccess() )  
  {
    fatal() << "Error retrieving IncidentSvc." << endmsg;
    return sc;
  }

//--------------------------------------------------------------------------
// Access Property Manager interface:
//--------------------------------------------------------------------------
  SmartIF<IProperty> prpMgr(serviceLocator());
  if ( !prpMgr.isValid() ) 
  {
    fatal() << "IProperty interface not found in ApplicationMgr." 
            << endmsg;
    return StatusCode::FAILURE;
  }


//--------------------------------------------------------------------------
// Set up the Histogram Service
//--------------------------------------------------------------------------
  sc = m_histoDataMgrSvc.retrieve();
  if( !sc.isSuccess() )  
  {
    fatal() << "Error retrieving HistogramDataSvc" 
            << endmsg;
    return sc;
  }
    
  const std::string& histPersName(m_histPersName.value());
  if ( histPersName.length() == 0 )    
  {
    setProperty(prpMgr->getProperty("HistogramPersistency")).ignore();
  }

  if ( histPersName != "NONE" )   {

    m_histoPersSvc = IConversionSvc_t( "HistogramPersistencySvc", 
				       this->name() );

    if( !sc.isSuccess() )  {
      warning() << "Histograms cannot not be saved - though required." 
                << endmsg;
    } else {

      IService *is = 0;
      if (histPersName == "ROOT") {
	sc = serviceLocator()->service("RootHistSvc", is);
      } else if ( histPersName == "HBOOK" ) {
	sc = serviceLocator()->service("HbookHistSvc", is);
      }

      if (sc.isFailure()) {
        error() << "could not locate actual Histogram persistency service"
                << endmsg;
      } else {
	Service *s = dynamic_cast<Service*>(is);
	if (s == 0) {
	  error() << "Could not dcast HistPersSvc to a Service"
                  << endmsg;
	} else {
	  const Property &prop = s->getProperty("OutputFile");
	  std::string val;
	  try {
	    const StringProperty &sprop = dynamic_cast<const StringProperty&>( prop );

	    val = sprop.value();

	  } catch (...) {
	    verbose() << "could not dcast OutputFile property to a StringProperty."
                      << " Need to fix Gaudi."
                      << endmsg;

	    val = prop.toString();

	    //	    val.erase(0,val.find(":")+1);
	    //	    val.erase(0,val.find("\"")+1);
	    //	    val.erase(val.find("\""),val.length());
	  }

	  if (val != "" && 
	      val != "UndefinedROOTOutputFileName" && 
	      val != "UndefinedHbookOutputFileName" ) {
	    m_writeHists = true;
	  }

	}
      }
    }
    

  }  else { if (msgLevel(MSG::DEBUG)) {
      debug() << "Histograms saving not required." 
              << endmsg; }
  }
//-------------------------------------------------------------------------
// Setup EventSelector service
//-------------------------------------------------------------------------
  const std::string& selName(m_evtsel.value());
  // the evt sel is usually specified as a property of ApplicationMgr
  if (selName.empty()) 
    sc = setProperty(prpMgr->getProperty("EvtSel"));
  if (sc.isFailure()) warning() << "Unable to set EvtSel property" << endmsg;

  // We do not expect a Event Selector necessarily being declared
  if( !selName.empty() && selName != "NONE") {
    IEvtSelector* theEvtSel(0);
    StatusCode sc(serviceLocator()->service( selName, theEvtSel ));
    if( sc.isSuccess() && ( theEvtSel != m_evtSelector ) ) {
      // Event Selector changed (or setup for the first time)
      m_evtSelector = theEvtSel;
      
      // reset iterator
      if (m_evtSelector->createContext(m_evtContext).isFailure()) {
	fatal() << "Can not create the event selector Context." 
                << endmsg;
	return StatusCode::FAILURE;
      }
      if (msgLevel(MSG::INFO)) {
	INamedInterface* named (dynamic_cast< INamedInterface* >(theEvtSel));
	if (0 != named) {
          info() << "Setup EventSelector service " << named->name( ) 
                 << endmsg;
	}
      }
    } else if (sc.isFailure()) {
      fatal() << "No valid event selector called " << selName 
              << endmsg;
      return StatusCode::FAILURE;
    }
  }  
//-------------------------------------------------------------------------
// Setup TimeKeeper service
//-------------------------------------------------------------------------
  // the time keeper may one day be specified as a property of ApplicationMgr
  //  setProperty(prpMgr->getProperty("TimeKeeper"));

//-------------------------------------------------------------------------
// Setup 'Clear-Store' policy
//-------------------------------------------------------------------------
  try {
    setClearStorePolicy( m_clearStorePolicy );
  } catch(...) {
    return StatusCode::FAILURE;
  }
//-------------------------------------------------------------------------
// Make sure the ActiveStoreSvc is initialized.
// We don't use this, but want to be sure that it gets created
// during initialization, to avoid heap fragmentation.
//-------------------------------------------------------------------------
  sc = m_activeStoreSvc.retrieve();
  if( !sc.isSuccess() )  
  {
    fatal() << "Error retrieving ActiveStoreSvc." << endmsg;
    return sc;
  }

  // Listen to the BeforeFork incident
  m_incidentSvc->addListener(this,"BeforeFork",0);

  CHECK( m_conditionsCleaner.retrieve() );

  return sc;
}

inline
StoreGateSvc* 
AthenaHiveEventLoopMgr::eventStore() const {
  return m_eventStore.get();
}

//=========================================================================
// property handlers
//=========================================================================
void 
AthenaHiveEventLoopMgr::setupTimeKeeper(Property&) {
  const std::string& tkName(m_timeKeeperName.value());
  // We do not expect a TimeKeeper necessarily being declared  
  if( tkName != "NONE" && tkName.length() != 0) {
    if (!(serviceLocator()->service( tkName, m_pITK, true)).isSuccess()) 
      error() << "TimeKeeper not found." << endmsg;
    else info() << "No TimeKeeper selected. "
	        << "No time limit control on event loop." 
	        << endmsg;
  }
}

void 
AthenaHiveEventLoopMgr::setClearStorePolicy(Property&) {
  const std::string& policyName = m_clearStorePolicy.value();

  if ( policyName != "BeginEvent" &&
       policyName != "EndEvent" ) {

    fatal() << "Unknown policy [" << policyName 
            << "] for the 'ClearStore-policy !\n"
            << "           Valid values are: BeginEvent, EndEvent"
            << endmsg;
    throw GaudiException("Can not setup 'ClearStore'-policy",
			 name(),
			 StatusCode::FAILURE);
  }

  return;
}

void
AthenaHiveEventLoopMgr::setupPreSelectTools(Property&) {

  m_toolInvoke.clear();
  m_toolReject.clear();
  m_toolAccept.clear();

  m_tools.retrieve().ignore();
  if(m_tools.size() > 0) {
    m_useTools=true;
    m_toolInvoke.resize(m_tools.size());
    m_toolReject.resize(m_tools.size());
    m_toolAccept.resize(m_tools.size());

    tool_iterator firstTool = m_tools.begin();
    tool_iterator lastTool  = m_tools.end();
    unsigned int toolCtr = 0;
    for ( ; firstTool != lastTool; firstTool++ )
      {
	// reset statistics
	m_toolInvoke[toolCtr] = 0;
	m_toolReject[toolCtr] = 0;
	m_toolAccept[toolCtr] = 0;
        toolCtr++;
      }
  }

  return;

}

//=========================================================================
// implementation of IAppMgrUI::finalize
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::finalize()    
{

  StatusCode sc = MinimalEventLoopMgr::finalize();
  if (sc.isFailure()) 
  {
    error() << "Error in Service base class Finalize" 
            << endmsg;
  }

  StatusCode sc2 = writeHistograms(true);
  if (sc2.isFailure()) 
  {
    error() << "Error in writing Histograms"
	    << endmsg;
  }

  // Release all interfaces (ignore StatusCodes)
  m_histoDataMgrSvc.release().ignore();
  m_histoPersSvc.release().ignore();
  m_activeStoreSvc.release().ignore();

  m_whiteboard = 0;
  m_algResourcePool = 0;
  m_schedulerSvc = 0;
  // m_evtDataSvc = 0;

  m_incidentSvc.release().ignore();

  // Release event selector context
  if ( m_evtSelector && m_evtContext )   {
    m_evtSelector->releaseContext(m_evtContext).ignore();
    //    m_evtSelector   = releaseInterface(m_evtSelector);
    delete m_evtContext; m_evtContext = 0;
  }


  if(m_useTools) {
    tool_iterator firstTool = m_tools.begin();
    tool_iterator lastTool  = m_tools.end();
    unsigned int toolCtr = 0;
    info() << "Summary of AthenaEvtLoopPreSelectTool invocation: (invoked/success/failure)" << endmsg;
    info() << "-----------------------------------------------------" << endmsg;

    for ( ; firstTool != lastTool; firstTool++ ) {
      info() << std::setw(2)     << std::setiosflags(std::ios_base::right)
             << toolCtr+1 << ".) " << std::resetiosflags(std::ios_base::right)
             << std::setw(48) << std::setfill('.')
             << std::setiosflags(std::ios_base::left)
             << (*firstTool)->name() << std::resetiosflags(std::ios_base::left)
             << std::setfill(' ')
             << " ("
             << std::setw(6) << std::setiosflags(std::ios_base::right)
             << m_toolInvoke[toolCtr]
             << "/"
             << m_toolAccept[toolCtr]
             << "/"
             << m_toolReject[toolCtr]
             << ")"
             << endmsg;
      toolCtr++;
    }
  }
  return ( sc.isFailure() || sc2.isFailure() ) ? StatusCode::FAILURE :
    StatusCode::SUCCESS;

}

//=========================================================================
// write out the histograms
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::writeHistograms(bool force) {

  StatusCode sc (StatusCode::SUCCESS);
  
  if ( 0 != m_histoPersSvc && m_writeHists ) {
    std::vector<DataObject*> objects;
    sc = m_histoDataMgrSvc->traverseTree( [&objects]( IRegistry* reg, int ) {
        DataObject* obj = reg->object();
        if ( !obj || obj->clID() == CLID_StatisticsFile ) return false;
        objects.push_back( obj );
        return true;
      } );

    if ( !sc.isSuccess() ) {
      error() << "Error while traversing Histogram data store" << endmsg;
      return sc;
    }

    if ( objects.size() > 0) {
      int writeInterval(m_writeInterval.value());

      if ( m_nevt == 1 || force || 
           (writeInterval != 0 && m_nevt%writeInterval == 0) ) {

        // skip /stat entry!
        sc = std::accumulate( begin( objects ), end( objects ), sc, [&]( StatusCode isc, auto& i ) {
            IOpaqueAddress* pAddr = nullptr;
            StatusCode      iret  = m_histoPersSvc->createRep( i, pAddr );
            if ( iret.isFailure() ) return iret;
            i->registry()->setAddress( pAddr );
            return isc;
          } );
        sc = std::accumulate( begin( objects ), end( objects ), sc, [&]( StatusCode isc, auto& i ) {
            IRegistry* reg  = i->registry();
            StatusCode iret = m_histoPersSvc->fillRepRefs( reg->address(), i );
            return iret.isFailure() ? iret : isc;
          } );
        if ( ! sc.isSuccess() ) {
          error() << "Error while saving Histograms." << endmsg;
        }
      }

      if (force || (writeInterval != 0 && m_nevt%writeInterval == 0) ) {
        if (msgLevel(MSG::DEBUG)) { debug() << "committing Histograms" << endmsg; }
        m_histoPersSvc->conversionSvc()->commitOutput("",true).ignore();
      }
    }
          
  }
  
  return sc;
}

//=========================================================================
// Run the algorithms beginRun hook
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::beginRunAlgorithms(const EventInfo& /*event */) {

  // Fire BeginRun "Incident"
  //  m_incidentSvc->fireIncident(EventIncident(event, name(),"BeginRun"));

  return StatusCode::SUCCESS;
}

//=========================================================================
// Run the algorithms endRun hook
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::endRunAlgorithms() {

  // Fire EndRun Incident
  //  m_incidentSvc->fireIncident(Incident(name(),"EndRun"));

  return StatusCode::SUCCESS;
}

//=========================================================================
// Call sysInitialize() on all algorithms and output streams
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::initializeAlgorithms() {

  return StatusCode::SUCCESS;
}

//=========================================================================
// Run the algorithms for the current event
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::executeAlgorithms() {

  return StatusCode::SUCCESS;
}


//=========================================================================
// executeEvent(void* par)
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::executeEvent(void* createdEvts_IntPtr )
{

  // Leave the interface intact and swallow this C trick.
  int& createdEvts = *((int*)createdEvts_IntPtr);

  std::unique_ptr<EventContext> evtContext = createEventContext(createdEvts);
  if ( !evtContext ){
    fatal() << "Impossible to create event context" << endmsg;
    return StatusCode::FAILURE;
  }

  // An incident may schedule a stop, in which case is better to exit before the actual execution.
  if ( m_scheduledStop ) {
    always() << "A stopRun was requested by an incidentListener. "
             << "Do not process this event." 
             << endmsg;
    createdEvts = -1;
    return (StatusCode::SUCCESS);
  }

  // Make sure context with slot is set before calling es->next().
  Gaudi::Hive::setCurrentContext (*evtContext);

  int declEvtRootSc = declareEventRootAddress(*evtContext);
  if (declEvtRootSc == 0 ) { // We ran out of events!
    createdEvts = -1;  // Set created event to a negative value: we finished!
    return StatusCode::SUCCESS;
  } else if ( declEvtRootSc == -1) {
    error() << "declareEventRootAddress for context " << *evtContext << " failed"
	    << endmsg;
    return StatusCode::FAILURE;
  }



  // const EventInfo* pEvent(reinterpret_cast<EventInfo*>(createdEvts_IntPtr)); //AUIII!
  const EventInfo* pEvent = m_pEvent;
  assert(pEvent);

  // Make sure context is set before firing any incidents.
  //Gaudi::Hive::setCurrentContext (*evtContext);

  /// Fire begin-Run incident if new run:
  if (m_firstRun || (m_currentRun != pEvent->event_ID()->run_number()) ) {
    // Fire EndRun incident unless this is the first run
    if (!m_firstRun) {
      // FIXME!!!
      m_incidentSvc->fireIncident(Incident(name(), IncidentType::EndRun));
    }
    m_firstRun=false;
    m_currentRun = pEvent->event_ID()->run_number();

    info() << "  ===>>>  start of run " << m_currentRun << "    <<<==="
           << endmsg;

    // FIXME!!! Fire BeginRun "Incident"
    m_incidentSvc->fireIncident(EventIncident(*pEvent, name(),
                                              IncidentType::BeginRun));

  }

  bool toolsPassed=true;
  // CGL: FIXME
  // bool eventFailed = false;

  // Call any attached tools to reject events early
  unsigned int toolCtr=0;
  if(m_useTools) {
    tool_store::iterator theTool = m_tools.begin();
    tool_store::iterator lastTool  = m_tools.end();
    while(toolsPassed && theTool!=lastTool ) 
      {
        toolsPassed = (*theTool)->passEvent(pEvent);
	m_toolInvoke[toolCtr]++;
        {toolsPassed ? m_toolAccept[toolCtr]++ : m_toolReject[toolCtr]++;}
        toolCtr++;
        theTool++;
      }
  }

  EventID::number_type evtNumber = pEvent->event_ID()->event_number();
  evtContext->setEventID( *((EventIDBase*) pEvent->event_ID()) );

  unsigned int conditionsRun = pEvent->event_ID()->run_number();
  const AthenaAttributeList* attr = nullptr;
  if (eventStore()->contains<AthenaAttributeList> ("Input") &&
      eventStore()->retrieve(attr, "Input").isSuccess())
  {
    if (attr->exists ("ConditionsRun")) {
      conditionsRun = (*attr)["ConditionsRun"].data<unsigned int>();
    }
  }
  evtContext->template getExtension<Atlas::ExtendedEventContext>().setConditionsRun (conditionsRun);

  // Make sure context global context has event id
  Gaudi::Hive::setCurrentContext (*evtContext);

  // Record a copy of the EventContext in current whiteboard
  if (eventStore()->record(std::make_unique<EventContext> (*evtContext),
                           "EventContext").isFailure())
  {
    error() << "Error recording event context object" << endmsg;
    return (StatusCode::FAILURE);
  }

  m_doEvtHeartbeat = (m_eventPrintoutInterval.value() > 0 && 
		 0 == (m_nev % m_eventPrintoutInterval.value()));
  if (m_doEvtHeartbeat)  {
   if(!m_useTools) 
     info() << "  ===>>>  start processing event #" << evtNumber << ", run #" << m_currentRun 
            << " on slot " << evtContext->slot() << ",  " << m_proc 
            << " events processed so far  <<<===" << endmsg;
   else 
     info() << "  ===>>>  start processing event #" << evtNumber << ", run #" << m_currentRun 
            << " on slot " << evtContext->slot() << ",  " 
            << m_nev << " events read and " << m_proc 
            << " events processed so far  <<<===" << endmsg;   
  }

  // Reset the timeout singleton
  resetTimeout(Athena::Timeout::instance(*evtContext));
  if(toolsPassed) {
    // Fire BeginEvent "Incident"
    //m_incidentSvc->fireIncident(EventIncident(*pEvent, name(),"BeginEvent",*evtContext));


    CHECK( m_conditionsCleaner->event (*evtContext, true) );

    // Remember the last event context for after event processing finishes.
    m_lastEventContext = *evtContext;
    
    // Now add event to the scheduler 
    debug() << "Adding event " << evtContext->evt() 
            << ", slot " << evtContext->slot()
            << " to the scheduler" << endmsg;
    
    m_incidentSvc->fireIncident(Incident(name(), IncidentType::BeginProcessing, 
					 *evtContext));
    StatusCode addEventStatus = m_schedulerSvc->pushNewEvent(evtContext.release());
    
    // If this fails, we need to wait for something to complete
    if (!addEventStatus.isSuccess()){
      fatal() << "An event processing slot should be now free in the scheduler, but it appears not to be the case." << endmsg;
    }
    
  }  // end of toolsPassed test

  ++m_nev;

  createdEvts++;

  // invalidate thread local context once outside of event execute loop
  Gaudi::Hive::setCurrentContext( EventContext() );

  return StatusCode::SUCCESS;

}

//=========================================================================
// implementation of IEventProcessor::executeRun
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::executeRun(int maxevt)
{

  StatusCode  sc;
  bool eventfailed = false;
  
  // Call now the nextEvent(...)
  sc = nextEvent(maxevt);
  if (!sc.isSuccess())
    eventfailed = true;

  if (eventfailed)
    return StatusCode::FAILURE;

  m_incidentSvc->fireIncident(Incident(name(),"EndEvtLoop"));
  return StatusCode::SUCCESS;
}
//-----------------------------------------------------------------------------
// Implementation of IEventProcessor::stopRun()
//-----------------------------------------------------------------------------
StatusCode AthenaHiveEventLoopMgr::stopRun() {
  // Set the application return code
  SmartIF<IProperty> appmgr(serviceLocator());
  if(Gaudi::setAppReturnCode(appmgr, Gaudi::ReturnCode::ScheduledStop).isFailure()) {
    error() << "Could not set return code of the application ("
            << Gaudi::ReturnCode::ScheduledStop << ")" << endmsg;
  }
  m_scheduledStop = true;
  return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Implementation of IService::stop()
//-----------------------------------------------------------------------------
StatusCode AthenaHiveEventLoopMgr::stop()
{
  // Need to be sure we have a valid EventContext during stop()))).
  Gaudi::Hive::setCurrentContext( m_lastEventContext );
  StatusCode sc = MinimalEventLoopMgr::stop();
  Gaudi::Hive::setCurrentContext( EventContext() );
  return sc;
}


//=========================================================================
// implementation of IAppMgrUI::nextEvent
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::nextEvent(int maxevt)   
{
  // make nextEvent(0) a dummy call
  if (0 == maxevt) return StatusCode::SUCCESS;

  // CGL: FIXME  
  // static int        total_nevt = 0;

  // CGL: FIXME  
  // the current 'clear-store' policy
  // static const ClearStorePolicy::Type s_clearStore = 
  //   clearStorePolicy( m_clearStorePolicy.value(), m_msg );

  // Reset the application return code.
  Gaudi::setAppReturnCode(m_appMgrProperty, Gaudi::ReturnCode::Success, true).ignore();  

  int finishedEvts =0;
  int createdEvts =0;
  info() << "Starting loop on events" << endmsg;

  // loop over events if the maxevt (received as input) is different from -1.
  // if evtmax is -1 it means infinite loop (till time limit that is)
  //  int nevt(0);
  // CGL: FIXME
  // bool noTimeLimit(false);
  bool loop_ended=false;
  StatusCode sc(StatusCode::SUCCESS,true);

  // Calculate runtime
  auto start_time = tbb::tick_count::now();
  auto secsFromStart = [&start_time]()->double{
    return (tbb::tick_count::now()-start_time).seconds();
  };

  while ( !loop_ended and ( (maxevt < 0) or (finishedEvts < maxevt) ) ){
    debug() << " -> createdEvts: " << createdEvts << endmsg;
    
    if ( (createdEvts >= 0) && // The events are not finished with an unlimited number of events
	 ( (createdEvts < maxevt) or (maxevt<0) ) &&  // The events are not finished with a limited number of events
	 (m_schedulerSvc->freeSlots()>0) ){ // There are still free slots in the scheduler
      
      debug() << "createdEvts: " << createdEvts << ", freeslots: " << m_schedulerSvc->freeSlots() << endmsg;
      
      sc = executeEvent(&createdEvts);
      if (sc.isFailure()) {
	error() << "Terminating event processing loop due to errors" << endmsg;
	loop_ended = true;
      }

    } // end if condition createdEvts < maxevt

    else { 
      // all the events were created but not all finished or the slots were 
      // all busy: the scheduler should finish its job

      debug() << "Draining the scheduler" << endmsg;

      // Pull out of the scheduler the finished events
      int ir = drainScheduler(finishedEvts);
      if (ir < 0) {
	// some sort of error draining scheduler;
        loop_ended = true;
	sc = StatusCode::FAILURE;
      } else if (ir == 0) {
	// no more events in scheduler. we're done
	loop_ended = true;
	sc = StatusCode::SUCCESS;
      } else {
	// keep going!
      }
      
    }
  } // end main loop on finished events  

  info() << "---> Loop Finished (seconds): " << secsFromStart() <<endmsg;


  return sc;


}


//=========================================================================
// Seek to a given event.
// The event selector must support the IEventSeek interface for this to work.
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::seek (int evt)
{
  IEvtSelectorSeek* is = dynamic_cast<IEvtSelectorSeek*> (m_evtSelector);
  if (is == 0) {
    error() << "Seek failed; unsupported by event selector"
            << endmsg;
    return StatusCode::FAILURE;
  }

  if (!m_evtContext) {
    if (m_evtSelector->createContext(m_evtContext).isFailure()) {
      fatal() << "Can not create the event selector Context."
              << endmsg;
      return StatusCode::FAILURE;
    }
  }

  StatusCode sc = is->seek (*m_evtContext, evt);
  if (sc.isSuccess()) {
    m_nevt = evt;
  }
  else {
    error() << "Seek failed." << endmsg;
  }
  return sc;
}


//=========================================================================
// Return the current event count.
//=========================================================================
int AthenaHiveEventLoopMgr::curEvent() const
{
  return m_nevt;
}

//=========================================================================
// Return the collection size
//=========================================================================
int AthenaHiveEventLoopMgr::size()
{
  IEvtSelectorSeek* cs = dynamic_cast<IEvtSelectorSeek*> (m_evtSelector);
  if (cs == 0) {
    error() << "Collection size unsupported by event selector"
            << endmsg;
    return -1;
  }

  if (!m_evtContext) {
    if (m_evtSelector->createContext(m_evtContext).isFailure()) {
      fatal() << "Can not create the event selector Context."
              << endmsg;
      return -1;
    }
  }

  return cs->size (*m_evtContext);
}

//=========================================================================
// Handle Incidents
//=========================================================================
void AthenaHiveEventLoopMgr::handle(const Incident& inc)
{

  if(inc.type()!="BeforeFork")
    return;

  if(!m_evtContext || !m_firstRun) {
    warning() << "Skipping BeforeFork handler. Either no event selector is provided or begin run has already passed" << endmsg;
  }

  // Initialize Algorithms and Output Streams
  StatusCode sc = initializeAlgorithms();
  if(sc.isFailure()) {
    error() << "Failed to initialize Algorithms" << endmsg;
    return; 
  }

  // Construct EventInfo
  const EventInfo* pEvent(0);
  IOpaqueAddress* addr = 0;
  sc = m_evtSelector->next(*m_evtContext);
  if(!sc.isSuccess()) {
    info() << "No more events in event selection " << endmsg;
    return;
  }
  sc = m_evtSelector->createAddress(*m_evtContext, addr);
  if (sc.isFailure()) {
    error() << "Could not create an IOpaqueAddress" << endmsg;
    return; 
  }
  if (0 != addr) {
    //create its proxy
    sc = eventStore()->recordAddress(addr);
    if(!sc.isSuccess()) {
      error() << "Error declaring Event object" << endmsg;
      return;
    }
  } 
  
  if(eventStore()->loadEventProxies().isFailure()) {
    warning() << "Error loading Event proxies" << endmsg;
    return;
  }

  // Retrieve the Event object
  sc = eventStore()->retrieve(pEvent);
  if(!sc.isSuccess()) {
    error() << "Unable to retrieve Event root object" << endmsg;
    return;
  }

  m_firstRun=false;
  m_currentRun = pEvent->event_ID()->run_number();

  m_pEvent = pEvent;

  // Clear Store
  const ClearStorePolicy::Type s_clearStore = clearStorePolicy( m_clearStorePolicy.value(), msgStream() );
  if(s_clearStore==ClearStorePolicy::EndEvent) {
    sc = eventStore()->clearStore();
    if(!sc.isSuccess()) {
      error() << "Clear of Event data store failed" << endmsg;
    }
  }
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
AthenaHiveEventLoopMgr::queryInterface(const InterfaceID& riid, 
				   void** ppvInterface) 
{
  if ( IEventSeek::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IEventSeek*>(this);
  }
  else if ( IEventProcessor::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IEventProcessor*>(this);
  }
  else if ( ICollectionSize::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ICollectionSize*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return MinimalEventLoopMgr::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

/// Create event address using event selector
StatusCode AthenaHiveEventLoopMgr::getEventRoot(IOpaqueAddress*& refpAddr)  {
  refpAddr = 0;
  StatusCode sc = m_evtSelector->next(*m_evtContext);
  if ( !sc.isSuccess() )  {
    return sc;
  }
  // Create root address and assign address to data service
  sc = m_evtSelector->createAddress(*m_evtContext,refpAddr);
  if( !sc.isSuccess() )  {
    sc = m_evtSelector->next(*m_evtContext);
    if ( sc.isSuccess() )  {
      sc = m_evtSelector->createAddress(*m_evtContext,refpAddr);
      if ( !sc.isSuccess() )  {
        warning() << "Error creating IOpaqueAddress." << endmsg;
      }
    }
  }
  return sc;
}

//---------------------------------------------------------------------------

int AthenaHiveEventLoopMgr::declareEventRootAddress(const EventContext& ctx){

  // return codes:
  //   -1 : error
  //    0 : no more events in selection
  //    1 : ok
  
  StatusCode sc(StatusCode::SUCCESS);

  //-----------------------------------------------------------------------
  // we need an EventInfo Object to fire the incidents. 
  //-----------------------------------------------------------------------
  const EventInfo* pEvent(0);
  m_pEvent = pEvent;
  if ( m_evtContext ) {
    // Deal with the case when an EventSelector is provided

    //
    // FIXME: flow control if no more events in selector, etc.
    //
    
    IOpaqueAddress* addr = 0;
    
    sc = m_evtSelector->next(*m_evtContext);
    
    if ( !sc.isSuccess() ) {
      // This is the end of the loop. No more events in the selection
      info() << "No more events in event selection " << endmsg;
      return 0;
    }
    
    if (m_evtSelector->createAddress(*m_evtContext, addr).isFailure()) {
      error() << "Could not create an IOpaqueAddress" << endmsg;
      return -1;
    }
    
      
    // Most iterators provide the IOA of an event header (EventInfo, DataHeader)
    if (0 != addr) {
      //create its proxy
      sc = eventStore()->recordAddress(addr);
      if( !sc.isSuccess() ) {
	//! FIXME ???
	warning() << "Error declaring Event object" << endmsg;
	return 0;
      }
    } if ((sc=eventStore()->loadEventProxies()).isFailure()) {
      error() << "Error loading Event proxies" << endmsg;
      return -1;
    } 
    
    // Retrieve the Event object
    sc = eventStore()->retrieve(pEvent);
    if( !sc.isSuccess() ) {
      error() << "Unable to retrieve Event root object" << endmsg;
      return -1;
    }

    m_pEvent = pEvent;

  }  else  {

    // with no iterator it's up to us to create an EventInfo
    // first event # == 1
    unsigned int runNmb{1}, evtNmb{m_nevt + 1};

    // increment the run/lumiBlock number if desired
    if (m_flmbi != 0) {
      runNmb = m_nevt / m_flmbi + 1;
      evtNmb = m_nevt % m_flmbi + 1;
    }
    auto eid = std::make_unique<EventID> (runNmb,evtNmb, m_timeStamp);
    // Change lumiBlock# to match runNumber
    eid->set_lumi_block( runNmb );

    m_timeStamp += m_timeStampInt;
    
    pEvent = new EventInfo(eid.release(), new EventType());

    m_pEvent = pEvent;

    debug() << "selecting store: " << ctx.slot() << endmsg;

    m_whiteboard->selectStore( ctx.slot() ).ignore();

    debug() << "recording EventInfo " << *pEvent->event_ID() << " in "
            << eventStore()->name() << endmsg;
    sc = eventStore()->record(pEvent,"McEventInfo");
    if( !sc.isSuccess() )  {
      error() << "Error declaring event data object" << endmsg;
      return -1;
    } 
  }
  
  return 1;
}

//---------------------------------------------------------------------------

std::unique_ptr<EventContext>  AthenaHiveEventLoopMgr::createEventContext(int createdEvts){
  auto evtContext = std::make_unique<EventContext>(createdEvts, m_whiteboard->allocateStore(createdEvts));

  // evtContext->m_evt_num = createdEvts;
  // evtContext->m_evt_slot = m_whiteboard->allocateStore(createdEvts);
  // evtContext->m_evt_failed = false;

  m_nevt = createdEvts;
  
  m_aess->reset(*evtContext);

  StatusCode sc = m_whiteboard->selectStore(evtContext->slot());
  if (sc.isFailure()){
    warning() << "Slot " << evtContext->slot()
              << " could not be selected for the WhiteBoard" << endmsg;
    evtContext.reset();
  } else {
    evtContext->setExtension( Atlas::ExtendedEventContext( eventStore()->hiveProxyDict() ) );

    debug() << "created EventContext, num: " << evtContext->evt()  << "  in slot: " 
	    << evtContext->slot() << endmsg;
  }
  return evtContext;
}

//---------------------------------------------------------------------------

int 
AthenaHiveEventLoopMgr::drainScheduler(int& finishedEvts){

  StatusCode sc(StatusCode::SUCCESS);
    
  // maybe we can do better
  std::vector<EventContext*> finishedEvtContexts;

  EventContext* finishedEvtContext(nullptr);

  // Here we wait not to loose cpu resources
  debug() << "drainScheduler: [" << finishedEvts << "] Waiting for a context" << endmsg;
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
  for (auto& thisFinishedEvtContext : finishedEvtContexts){
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
      if (eventStore()->retrieve(pEvent).isFailure()) {
        error() << "DrainSched: unable to get EventInfo obj" << endmsg;
	delete thisFinishedEvtContext;
	fail = true;
	continue;
      } else {
	n_run = pEvent->event_ID()->run_number();
	n_evt = pEvent->event_ID()->event_number();
      }
    } else {
      error() << "DrainSched: unable to select store "
	      << thisFinishedEvtContext->slot() << endmsg;
      delete thisFinishedEvtContext;
      fail = true;
      continue;
    }

    // m_incidentSvc->fireIncident(Incident(name(), IncidentType::EndEvent,
    // 					 *thisFinishedEvtContext ));
    m_incidentSvc->fireIncident(Incident(name(), IncidentType::EndProcessing, *thisFinishedEvtContext ));

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

    debug() << "drainScheduler thisFinishedEvtContext: " << thisFinishedEvtContext
	    << endmsg;

    delete thisFinishedEvtContext;
  }

  return (  fail ? -1 : 1 );

}

//---------------------------------------------------------------------------

StatusCode AthenaHiveEventLoopMgr::clearWBSlot(int evtSlot)  {
  StatusCode sc = m_whiteboard->clearStore(evtSlot);
  if( !sc.isSuccess() )  {
    warning() << "Clear of Event data store failed" << endmsg;    
  }
  return m_whiteboard->freeStore(evtSlot);  
}
//---------------------------------------------------------------------------

