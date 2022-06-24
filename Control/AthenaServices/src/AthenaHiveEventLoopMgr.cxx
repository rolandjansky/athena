/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#define  ATHENASERVICES_ATHENAHIVEEVENTLOOPMGR_CPP

#include <cassert>
#include <ios>
#include <iostream>
#include <fstream> /* ofstream */
#include <iomanip>

// Athena includes
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "PersistentDataModel/AthenaAttributeList.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/IEventSeek.h"
#include "AthenaKernel/IEvtSelectorSeek.h"
#include "AthenaKernel/IAthenaEvtLoopPreSelectTool.h"
#include "AthenaKernel/ExtendedEventContext.h"
#include "AthenaKernel/EventContextClid.h"
#include "AthenaKernel/IEvtIdModifierSvc.h"

#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/AppReturnCode.h"
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "xAODEventInfo/EventInfo.h"
#include "EventInfoUtils/EventInfoFromxAOD.h"

#include "AthenaHiveEventLoopMgr.h"

#include <GaudiKernel/IScheduler.h>
// External libraries
#include "tbb/tick_count.h"



//=========================================================================
// Standard Constructor
//=========================================================================
AthenaHiveEventLoopMgr::AthenaHiveEventLoopMgr(const std::string& nam, 
				       ISvcLocator* svcLoc)
  : MinimalEventLoopMgr(nam, svcLoc), 
    AthMessaging (nam),
    m_incidentSvc ( "IncidentSvc",  nam ), 
    m_eventStore( "StoreGateSvc", nam ), 
    m_evtSelector(0), m_evtContext(0),
    m_histoDataMgrSvc( "HistogramDataSvc",         nam ), 
    m_histoPersSvc   ( "HistogramPersistencySvc",  nam ), 
    m_evtIdModSvc    ( "",         nam ),
    m_currentRun(0), m_firstRun(true), m_tools(this), m_nevt(0), m_writeHists(false),
    m_nev(0), m_proc(0), m_useTools(false),m_doEvtHeartbeat(false),
    m_conditionsCleaner( "Athena::ConditionsCleanerSvc", nam )
{
  declareProperty("EvtSel", m_evtsel, 
		  "Name of Event Selector to use. If empty string (default) "
		  "take value from ApplicationMgr");
  declareProperty("HistogramPersistency", m_histPersName="",
		  "Histogram persistency technology to use: ROOT, HBOOK, NONE. "
		  "By default (empty string) get property value from "
		  "ApplicationMgr");
  declareProperty("HistWriteInterval",    m_writeInterval=0 ,
		  "histogram write/update interval");
  declareProperty("FailureMode",          m_failureMode=1 , 
		  "Controls behaviour of event loop depending on return code of"
		  " Algorithms. 0: all non-SUCCESSes terminate job. "
		  "1: RECOVERABLE skips to next event, FAILURE terminates job "
		  "(DEFAULT). 2: RECOVERABLE and FAILURE skip to next events");
  declareProperty("EventPrintoutInterval", m_eventPrintoutInterval=1,
                  "Print event heartbeat printouts every m_eventPrintoutInterval events");
  declareProperty("PreSelectTools",m_tools,"AlgTools for event pre-selection")->
    declareUpdateHandler( &AthenaHiveEventLoopMgr::setupPreSelectTools, this ); ;
  
  declareProperty("SchedulerSvc", m_schedulerName="ForwardSchedulerSvc",
                  "Name of the scheduler to be used");
  
  declareProperty("WhiteboardSvc", m_whiteboardName="EventDataSvc",
		  "Name of the Whiteboard to be used");

  declareProperty("EventStore", m_eventStore);

  declareProperty("EvtIdModifierSvc", m_evtIdModSvc,
                  "ServiceHandle for EvtIdModifierSvc");

  declareProperty("FakeLumiBlockInterval", m_flmbi = 0,
                  "Event interval at which to increment lumiBlock# when "
                  "creating events without an EventSelector. Zero means " 
                  "don't increment it");
  declareProperty("FakeTimestampInterval", m_timeStampInt = 1,
                  "timestamp interval between events when creating Events "
                  "without an EventSelector");
  declareProperty("RequireInputAttributeList", m_requireInputAttributeList = false,
                  "Require valid input attribute list to be present");
  declareProperty("UseSecondaryEventNumber", m_useSecondaryEventNumber = false,
                  "In case of DoubleEventSelector use event number from secondary input");

  declareProperty("FirstEventAlone", m_firstEventAlone = true,
                  "process all of first event before scheduling any more");

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

  ATH_MSG_INFO ( "Initializing " << name() );
 

  StatusCode sc = MinimalEventLoopMgr::initialize();
  if ( !sc.isSuccess() ) 
  {
    ATH_MSG_ERROR ( "Failed to initialize base class MinimalEventLoopMgr" );
    return sc;
  } 

//-------------------------------------------------------------------------
// Setup stuff for hive
//-------------------------------------------------------------------------

  m_whiteboard = serviceLocator()->service(m_whiteboardName);
  if( !m_whiteboard.isValid() )  {
    ATH_MSG_FATAL ( "Error retrieving " << m_whiteboardName << " interface IHiveWhiteBoard." );
    return StatusCode::FAILURE;
  }
  
  m_schedulerSvc = serviceLocator()->service(m_schedulerName);
  if ( !m_schedulerSvc.isValid()){
    ATH_MSG_FATAL ( "Error retrieving SchedulerSvc interface ISchedulerSvc." );
    return StatusCode::FAILURE;    
  }
  // Setup algorithm resource pool
  m_algResourcePool = serviceLocator()->service("AlgResourcePool");
  if( !m_algResourcePool.isValid() ) {
    ATH_MSG_FATAL ( "Error retrieving AlgResourcePool" );
    return StatusCode::FAILURE;
  }

  m_aess = serviceLocator()->service("AlgExecStateSvc");
  if( !m_aess.isValid() ) {
    ATH_MSG_FATAL ( "Error retrieving AlgExecStateSvc" );
    return StatusCode::FAILURE;
  }

  sc = m_eventStore.retrieve();
  if( !sc.isSuccess() )  
  {
    ATH_MSG_FATAL ( "Error retrieving pointer to StoreGateSvc" );
    return sc;
  }

//--------------------------------------------------------------------------
// Get the references to the services that are needed by the ApplicationMgr 
// itself
//--------------------------------------------------------------------------
  sc = m_incidentSvc.retrieve();
  if( !sc.isSuccess() )  
  {
    ATH_MSG_FATAL ( "Error retrieving IncidentSvc." );
    return sc;
  }

//--------------------------------------------------------------------------
// Access Property Manager interface:
//--------------------------------------------------------------------------
  SmartIF<IProperty> prpMgr(serviceLocator());
  if ( !prpMgr.isValid() ) 
  {
    ATH_MSG_FATAL ( "IProperty interface not found in ApplicationMgr." );
    return StatusCode::FAILURE;
  }


//--------------------------------------------------------------------------
// Set up the Histogram Service
//--------------------------------------------------------------------------
  sc = m_histoDataMgrSvc.retrieve();
  if( !sc.isSuccess() )  
  {
    ATH_MSG_FATAL ( "Error retrieving HistogramDataSvc" );
    return sc;
  }
    
  const std::string& histPersName(m_histPersName.value());
  if ( histPersName.length() == 0 )    
  {
    CHECK(setProperty(prpMgr->getProperty("HistogramPersistency")));
  }

  if ( histPersName != "NONE" )   {

    m_histoPersSvc = IConversionSvc_t( "HistogramPersistencySvc", 
				       this->name() );

    IService *is = 0;
    if (histPersName == "ROOT") {
      sc = serviceLocator()->service("RootHistSvc", is);
    } else if ( histPersName == "HBOOK" ) {
      sc = serviceLocator()->service("HbookHistSvc", is);
    }

    if (sc.isFailure()) {
      ATH_MSG_ERROR ( "could not locate actual Histogram persistency service" );
    } else {
      Service *s = dynamic_cast<Service*>(is);
      if (s == 0) {
	ATH_MSG_ERROR ( "Could not dcast HistPersSvc to a Service" );
      } else {
	const Gaudi::Details::PropertyBase &prop = s->getProperty("OutputFile");
	std::string val;
	try {
	  const StringProperty &sprop = dynamic_cast<const StringProperty&>( prop );
	  val = sprop.value();
	} catch (...) {
	  ATH_MSG_VERBOSE ( "could not dcast OutputFile property to a StringProperty."
		    << " Need to fix Gaudi." );
	  val = prop.toString();
	}

	if (val != "" &&
	    val != "UndefinedROOTOutputFileName" &&
	    val != "UndefinedHbookOutputFileName" ) {
	  m_writeHists = true;
	}
      }
    }
  }  else {
    ATH_MSG_DEBUG ( "Histograms saving not required." );
  }


//--------------------------------------------------------------------------
// Set up the EventID modifier Service
//--------------------------------------------------------------------------
  if( m_evtIdModSvc.empty() ) {
    ATH_MSG_DEBUG ( "EventID modifier Service not set. No run number, ... overrides will be applied." );
  }
  else if ( !m_evtIdModSvc.retrieve().isSuccess() ) {
    ATH_MSG_INFO ( "Could not find EventID modifier Service. No run number, ... overrides will be applied." );
  }

//-------------------------------------------------------------------------
// Setup EventSelector service
//-------------------------------------------------------------------------
  const std::string& selName(m_evtsel.value());
  // the evt sel is usually specified as a property of ApplicationMgr
  if (selName.empty()) 
    sc = setProperty(prpMgr->getProperty("EvtSel"));
  if (sc.isFailure()) ATH_MSG_WARNING ( "Unable to set EvtSel property" );

  // We do not expect a Event Selector necessarily being declared
  if( !selName.empty() && selName != "NONE") {
    IEvtSelector* theEvtSel(0);
    StatusCode sc(serviceLocator()->service( selName, theEvtSel ));
    if( sc.isSuccess() && ( theEvtSel != m_evtSelector ) ) {
      // Event Selector changed (or setup for the first time)
      m_evtSelector = theEvtSel;
      
      // reset iterator
      if (m_evtSelector->createContext(m_evtContext).isFailure()) {
	ATH_MSG_FATAL ( "Can not create the event selector Context." );
	return StatusCode::FAILURE;
      }
      if (msgLevel(MSG::INFO)) {
	INamedInterface* named (dynamic_cast< INamedInterface* >(theEvtSel));
	if (0 != named) {
          ATH_MSG_INFO ( "Setup EventSelector service " << named->name( ) );
	}
      }
    } else if (sc.isFailure()) {
      ATH_MSG_FATAL ( "No valid event selector called " << selName );
      return StatusCode::FAILURE;
    }
  }  

  // Listen to the BeforeFork and EndAlgorithms incidents
  m_incidentSvc->addListener(this,"BeforeFork",0);
  m_incidentSvc->addListener(this, "EndAlgorithms",0);

  CHECK( m_conditionsCleaner.retrieve() );

  // Print if we override the event number using the one from secondary event
  if (m_useSecondaryEventNumber)
  {
    ATH_MSG_INFO ( "Using secondary event number." );
  }

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
AthenaHiveEventLoopMgr::setupPreSelectTools(Gaudi::Details::PropertyBase&) {

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
    for ( ; firstTool != lastTool; ++firstTool )
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
    ATH_MSG_ERROR ( "Error in Service base class Finalize" );
  }

  StatusCode sc2 = writeHistograms(true);
  if (sc2.isFailure()) 
  {
    ATH_MSG_ERROR ( "Error in writing Histograms" );
  }

  // Release all interfaces (ignore StatusCodes)
  m_histoDataMgrSvc.release().ignore();
  m_histoPersSvc.release().ignore();

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
    ATH_MSG_INFO ( "Summary of AthenaEvtLoopPreSelectTool invocation: (invoked/success/failure)" );
    ATH_MSG_INFO ( "-----------------------------------------------------" );

    for ( ; firstTool != lastTool; ++firstTool ) {
      ATH_MSG_INFO ( std::setw(2)     << std::setiosflags(std::ios_base::right)
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
             );
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
      ATH_MSG_ERROR ( "Error while traversing Histogram data store" );
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
          ATH_MSG_ERROR ( "Error while saving Histograms." );
        }
      }

      if (force || (writeInterval != 0 && m_nevt%writeInterval == 0) ) {
        ATH_MSG_DEBUG ( "committing Histograms" );
        m_histoPersSvc->conversionSvc()->commitOutput("",true).ignore();
      }
    }
          
  }
  
  return sc;
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
// executeEvent( EventContext &&ctx )
//=========================================================================
StatusCode AthenaHiveEventLoopMgr::executeEvent( EventContext &&ctx )
{

  // An incident may schedule a stop, in which case is better to exit before the actual execution.
  if ( m_scheduledStop ) {
    ATH_MSG_ALWAYS ( "A stopRun was requested by an incidentListener. "
             << "Do not process this event." );
    m_terminateLoop = true;
    return (StatusCode::SUCCESS);
  }

  m_aess->reset( ctx );

  // Make sure context with slot is set before calling es->next().
  Gaudi::Hive::setCurrentContext ( ctx );

  int declEvtRootSc = declareEventRootAddress( ctx );
  if (declEvtRootSc == 0 ) { // We ran out of events!
    m_terminateLoop = true;  // we have finished!
    return StatusCode::SUCCESS;
  } else if ( declEvtRootSc == -1) {
    ATH_MSG_ERROR ( "declareEventRootAddress for context " << ctx << " failed" );
    return StatusCode::FAILURE;
  }

  EventID::number_type evtNumber = ctx.eventID().event_number();
  unsigned int conditionsRun = ctx.eventID().run_number();
  if (!m_evtIdModSvc.isSet()) {
    const AthenaAttributeList* attr = nullptr;
    if (eventStore()->contains<AthenaAttributeList> ("Input") &&
        eventStore()->retrieve(attr, "Input").isSuccess()) {
      if (attr->exists ("ConditionsRun")) {
        conditionsRun = (*attr)["ConditionsRun"].data<unsigned int>();
      }
    }
  }
  Atlas::getExtendedEventContext(ctx).setConditionsRun (conditionsRun);
  Gaudi::Hive::setCurrentContext ( ctx );

  // Record EventContext in current whiteboard
  if (eventStore()->record(std::make_unique<EventContext> (ctx),
                           "EventContext").isFailure())
  {
    ATH_MSG_ERROR ( "Error recording event context object" );
    return (StatusCode::FAILURE);
  }

  /// Fire begin-Run incident if new run:
  if (m_firstRun || (m_currentRun != ctx.eventID().run_number()) ) {
    // Fire EndRun incident unless this is the first run
    if (!m_firstRun) {
      // FIXME!!!
      m_incidentSvc->fireIncident(Incident(name(), IncidentType::EndRun));
    }
    m_firstRun=false;
    m_currentRun = ctx.eventID().run_number();

    ATH_MSG_INFO ( "  ===>>>  start of run " << m_currentRun << "    <<<===" );

    // FIXME!!! Fire BeginRun "Incident"
    m_incidentSvc->fireIncident(Incident(name(),IncidentType::BeginRun,ctx));

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
        toolsPassed = (*theTool)->passEvent(ctx.eventID());
	m_toolInvoke[toolCtr]++;
        {toolsPassed ? m_toolAccept[toolCtr]++ : m_toolReject[toolCtr]++;}
        ++toolCtr;
        ++theTool;
      }
  }

  m_doEvtHeartbeat = (m_eventPrintoutInterval.value() > 0 && 
		 0 == (m_nev % m_eventPrintoutInterval.value()));
  if (m_doEvtHeartbeat)  {
    if(!m_useTools) {
      ATH_MSG_INFO ( "  ===>>>  start processing event #" << evtNumber << ", run #" << m_currentRun
                     << " on slot " << ctx.slot() << ",  " << m_proc
                     << " events processed so far  <<<===" );
    }
    else {
      ATH_MSG_INFO ( "  ===>>>  start processing event #" << evtNumber << ", run #" << m_currentRun
                     << " on slot " << ctx.slot() << ",  "
                     << m_nev << " events read and " << m_proc
                     << " events processed so far  <<<===" );
    }
  }

  // Reset the timeout singleton
  resetTimeout(Athena::Timeout::instance(ctx));
  if(toolsPassed) {

    CHECK( m_conditionsCleaner->event (ctx, true) );

    // Remember the last event context for after event processing finishes.
    m_lastEventContext = ctx;
    
    // Now add event to the scheduler 
    ATH_MSG_DEBUG ( "Adding event " << ctx.evt()
            << ", slot " << ctx.slot()
            << " to the scheduler" );
    
    m_incidentSvc->fireIncident(Incident(name(), IncidentType::BeginProcessing, 
					 ctx));
    StatusCode addEventStatus = m_schedulerSvc->pushNewEvent( new EventContext{ std::move(ctx) } );
    
    // If this fails, we need to wait for something to complete
    if (!addEventStatus.isSuccess()){
      ATH_MSG_FATAL ( "An event processing slot should be now free in the scheduler, but it appears not to be the case." );
    }
    
  }  // end of toolsPassed test

  ++m_nev;

  ++m_nevt;

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
    ATH_MSG_ERROR ( "Could not set return code of the application ("
            << Gaudi::ReturnCode::ScheduledStop << ")" );
  }
  m_scheduledStop = true;
  return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
// Implementation of IService::stop()
//-----------------------------------------------------------------------------
StatusCode AthenaHiveEventLoopMgr::stop()
{
  // To enable conditions access during stop we set an invalid EventContext
  // (no event/slot number) but with valid EventID (and extended EventContext).
  m_lastEventContext.setValid(false);
  Gaudi::Hive::setCurrentContext( m_lastEventContext );

  StatusCode sc = MinimalEventLoopMgr::stop();

  // If we exit the event loop early due to an error, some event stores
  // may not have been cleared.  This can lead to segfaults later,
  // as DetectorStore will usually get finalized before HiveSvcMgr.
  // So make sure that all stores have been cleared at this point.
  size_t nslot = m_whiteboard->getNumberOfStores();
  for (size_t islot = 0; islot < nslot; islot++) {
    sc &= clearWBSlot (islot);
  }

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

  // Reset the application return code.
  Gaudi::setAppReturnCode(m_appMgrProperty, Gaudi::ReturnCode::Success, true).ignore();  

  int finishedEvts =0;
  int createdEvts =0;
  ATH_MSG_INFO ( "Starting loop on events" );

  // loop over events if the maxevt (received as input) is different from -1.
  // if evtmax is -1 it means infinite loop (till time limit that is)
  //  int nevt(0);
  // CGL: FIXME
  // bool noTimeLimit(false);
  bool loop_ended=false;
  StatusCode sc(StatusCode::SUCCESS);

  bool newEvtAllowed = ! m_firstEventAlone;
  
  // Calculate runtime
  auto start_time = tbb::tick_count::now();
  auto secsFromStart = [&start_time]()->double{
    return (tbb::tick_count::now()-start_time).seconds();
  };

  while ( !loop_ended and ( (maxevt < 0) or (finishedEvts < maxevt) ) ){

    ATH_MSG_DEBUG ( " -> createdEvts: " << createdEvts );
    
    if ( ( !m_terminateLoop ) && // The events are not finished with an unlimited number of events
         (newEvtAllowed || createdEvts == 0) &&       // Launch first event alone
	 ( (createdEvts < maxevt) or (maxevt<0) ) &&  // The events are not finished with a limited number of events
	 (m_schedulerSvc->freeSlots()>0) ){ // There are still free slots in the scheduler
      
      ATH_MSG_DEBUG ( "createdEvts: " << createdEvts << ", freeslots: " << m_schedulerSvc->freeSlots() );
      
      auto ctx = createEventContext();
      
      if ( !ctx.valid() ) {
        sc = StatusCode::FAILURE;
      } else {
        sc = executeEvent( std::move(ctx) );
      }

      if (sc.isFailure()) {
	ATH_MSG_ERROR ( "Terminating event processing loop due to errors" );
	loop_ended = true;
      } else {
        ++createdEvts;
      }

    } // end if condition createdEvts < maxevt

    else { 
      // all the events were created but not all finished or the slots were 
      // all busy: the scheduler should finish its job

        ATH_MSG_DEBUG ( "Draining the scheduler" );

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
      newEvtAllowed = true;
      
    }
  } // end main loop on finished events  

  ATH_MSG_INFO ( "---> Loop Finished (seconds): " << secsFromStart() );


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
    ATH_MSG_ERROR ( "Seek failed; unsupported by event selector" );
    return StatusCode::FAILURE;
  }
  //cppcheck-suppress nullPointerRedundantCheck
  if (!m_evtContext) {
    if (m_evtSelector->createContext(m_evtContext).isFailure()) {
      ATH_MSG_FATAL ( "Can not create the event selector Context." );
      return StatusCode::FAILURE;
    }
  }
  //m_evtContext cannot be null if createContext succeeded
  //cppcheck-suppress nullPointerRedundantCheck
  StatusCode sc = is->seek (*m_evtContext, evt);
  if (sc.isSuccess()) {
    m_nevt = evt;
  }
  else {
    ATH_MSG_ERROR ( "Seek failed." );
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
    ATH_MSG_ERROR ( "Collection size unsupported by event selector" );
    return -1;
  }
  //cppcheck-suppress nullPointerRedundantCheck
  if (!m_evtContext) {
    if (m_evtSelector->createContext(m_evtContext).isFailure()) {
      ATH_MSG_FATAL ( "Can not create the event selector Context." );
      return -1;
    }
  }
  //m_evtContext cannot be null if createContext succeeded
  //cppcheck-suppress nullPointerRedundantCheck
  return cs->size (*m_evtContext);
}

//=========================================================================
// Handle Incidents
//=========================================================================
void AthenaHiveEventLoopMgr::handle(const Incident& inc)
{

  if(inc.type() == "EndAlgorithms") {
    // Clear the store at the end of the event.
    // Do it here so that it executes in an algorithm context and thus
    // multiple stores can be cleared at the same time.
    StatusCode sc = m_whiteboard->clearStore(inc.context().slot());
    if( !sc.isSuccess() )  {
      ATH_MSG_WARNING ( "Clear of Event data store failed" );
    }
    return;
  }

  if(inc.type()!="BeforeFork")
    return;

  if(!m_evtContext || !m_firstRun) {
    ATH_MSG_WARNING ( "Skipping BeforeFork handler. Either no event selector is provided or begin run has already passed" );
  }

  // Initialize Algorithms and Output Streams
  StatusCode sc = initializeAlgorithms();
  if(sc.isFailure()) {
    ATH_MSG_ERROR ( "Failed to initialize Algorithms" );
    return; 
  }

  // Construct EventInfo
  sc = m_evtSelector->next(*m_evtContext);
  if(!sc.isSuccess()) {
    ATH_MSG_INFO ( "No more events in event selection " );
    return;
  }
  IOpaqueAddress* addr{nullptr};
  sc = m_evtSelector->createAddress(*m_evtContext, addr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "Could not create an IOpaqueAddress" );
    return;
  }
  if (0 != addr) {
    //create its proxy
    sc = eventStore()->recordAddress(addr);
    if(!sc.isSuccess()) {
      ATH_MSG_ERROR ( "Error declaring Event object" );
      return;
    }
  }

  if(eventStore()->loadEventProxies().isFailure()) {
    ATH_MSG_WARNING ( "Error loading Event proxies" );
    return;
  }

  // Retrieve the legacy EventInfo object
  const EventInfo* pEvent{nullptr};
  sc = eventStore()->retrieve(pEvent);
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR ( "Unable to retrieve Event root object" );
    return;
  }

  m_firstRun=false;
  m_currentRun = pEvent->event_ID()->run_number();

  // Clear Store
  sc = eventStore()->clearStore();
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR ( "Clear of Event data store failed" );
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
        ATH_MSG_WARNING ( "Error creating IOpaqueAddress." );
      }
    }
  }
  return sc;
}

//---------------------------------------------------------------------------

int AthenaHiveEventLoopMgr::declareEventRootAddress(EventContext& ctx){

  // return codes:
  //   -1 : error
  //    0 : no more events in selection
  //    1 : ok
  
  StatusCode sc(StatusCode::SUCCESS);

  //-----------------------------------------------------------------------
  // we need an EventInfo Object to fire the incidents. 
  //-----------------------------------------------------------------------
  const EventInfo* pEvent{};
  if ( m_evtContext ) {
    // Deal with the case when an EventSelector is provided
    std::unique_ptr<EventInfo> pEventPtr;
    //
    // FIXME: flow control if no more events in selector, etc.
    //

    IOpaqueAddress* addr{};

    sc = m_evtSelector->next(*m_evtContext);
    
    if ( !sc.isSuccess() ) {
      // This is the end of the loop. No more events in the selection
      ATH_MSG_INFO ( "No more events in event selection " );
      return 0;
    }
    
    if (m_evtSelector->createAddress(*m_evtContext, addr).isFailure()) {
      ATH_MSG_ERROR ( "Could not create an IOpaqueAddress" );
      return -1;
    }
    
      
    // Most iterators provide the IOA of an event header (EventInfo, DataHeader)
    if (0 != addr) {
      //create its proxy
      sc = eventStore()->recordAddress(addr);
      if( !sc.isSuccess() ) {
	//! FIXME ???
	ATH_MSG_WARNING ( "Error declaring Event object" );
	return 0;
      }
    } if ((sc=eventStore()->loadEventProxies()).isFailure()) {
      ATH_MSG_ERROR ( "Error loading Event proxies" );
      return -1;
    }

    bool consume_modifier_stream = false;
    // First try to build a legacy EventInfo object from the TAG information
    // Read the attribute list
    const AthenaAttributeList* pAttrList = eventStore()->tryConstRetrieve<AthenaAttributeList>("Input");
    if ( pAttrList != nullptr && pAttrList->size() > 6 ) { // Try making EventID-only EventInfo object from in-file TAG
      try {
        unsigned int runNumber = (*pAttrList)["RunNumber"].data<unsigned int>();
        unsigned long long eventNumber = (*pAttrList)["EventNumber"].data<unsigned long long>();
        unsigned int eventTime = (*pAttrList)["EventTime"].data<unsigned int>();
        unsigned int eventTimeNS = (*pAttrList)["EventTimeNanoSec"].data<unsigned int>();
        unsigned int lumiBlock = (*pAttrList)["LumiBlockN"].data<unsigned int>();
        unsigned int bunchId = (*pAttrList)["BunchId"].data<unsigned int>();

        ATH_MSG_DEBUG ( "use TAG with runNumber=" << runNumber );
        consume_modifier_stream = true;
        // an option to override primary eventNumber with the secondary one in case of DoubleEventSelector
        if ( m_useSecondaryEventNumber ) {
            unsigned long long eventNumberSecondary{};
            if ( !(pAttrList->exists("hasSecondaryInput") && (*pAttrList)["hasSecondaryInput"].data<bool>()) ) {
                ATH_MSG_FATAL ( "Secondary EventNumber requested, but secondary input does not exist!" );
                return -1;
            }
            if ( pAttrList->exists("EventNumber_secondary") ) {
                eventNumberSecondary = (*pAttrList)["EventNumber_secondary"].data<unsigned long long>();
            }
            else {
                // try legacy EventInfo if secondary input did not have attribute list
                // primary input should not have this EventInfo type
                const EventInfo* pEventSecondary = eventStore()->tryConstRetrieve<EventInfo>();
                if (pEventSecondary) {
                    eventNumberSecondary = pEventSecondary->event_ID()->event_number();
                }
                else {
                    ATH_MSG_FATAL ( "Secondary EventNumber requested, but it does not exist!" );
                    return -1;
                }
            }
            if (eventNumberSecondary != 0) {
                m_doEvtHeartbeat = (m_eventPrintoutInterval.value() > 0 && 
                  0 == (m_nev % m_eventPrintoutInterval.value()));
                if (m_doEvtHeartbeat) {
                    ATH_MSG_INFO ( "  ===>>>  using secondary event #" << eventNumberSecondary << " instead of #" << eventNumber << "  <<<===" );
                }
                eventNumber = eventNumberSecondary;
            }
        }

        pEventPtr = std::make_unique<EventInfo>
          (new EventID(runNumber, eventNumber, eventTime, eventTimeNS, lumiBlock, bunchId), nullptr);
        pEvent = pEventPtr.get();
      } catch (...) {
      }
    } else if (m_requireInputAttributeList) {
      ATH_MSG_FATAL ( "Valid input attribute list required but not present!" );
      return -1;
    }
    // In the case that there is no TAG information
    if (!pEvent) {
      // Secondly try to retrieve a legacy EventInfo object from the input file
      pEvent = eventStore()->tryConstRetrieve<EventInfo>();
      if ( pEvent ) {
        consume_modifier_stream = false; // stream will already have been consumed during EventInfo TP conversion
        ATH_MSG_DEBUG ( "use EventInfo" );
      }
      else {
        // Finally try to retrieve an xAOD::EventInfo object from the
        // input file and build a legacy EventInfo object from that.
        const xAOD::EventInfo* pXEvent{nullptr};
        sc = eventStore()->retrieve(pXEvent);
        if( !sc.isSuccess() ) {
          ATH_MSG_ERROR ( "Unable to retrieve Event root object" );
          return -1;
        }
        consume_modifier_stream = true;
        ATH_MSG_DEBUG ( "use xAOD::EventInfo with runNumber=" << pXEvent->runNumber() );
        // Build the old-style Event Info object for those clients that still need it
        pEventPtr = std::make_unique<EventInfo>(new EventID(eventIDFromxAOD(pXEvent))
                                                , new EventType(eventTypeFromxAOD(pXEvent)));
        pEvent = pEventPtr.get();
        sc = eventStore()->record(std::move(pEventPtr),"");
        if( !sc.isSuccess() )  {
          ATH_MSG_ERROR ( "Error declaring event data object" );
          return -1;
        }
      }
    }

    modifyEventContext(ctx,*(pEvent->event_ID()), consume_modifier_stream);

  }
  else  {
    // No EventSelector is provided, so with no iterator it's up to us
    // to create an EventInfo
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

    bool consume_modifier_stream = true;
    // EventInfo TP Conversion not called in this case, so we would
    // want to consume the next IoV from the list in the
    // EvtIdModifierSvc.
    modifyEventContext(ctx,*(pEvent->event_ID()), consume_modifier_stream);

    ATH_MSG_DEBUG ( "selecting store: " << ctx.slot() );

    m_whiteboard->selectStore( ctx.slot() ).ignore();

    ATH_MSG_DEBUG ( "recording EventInfo " << *pEvent->event_ID() << " in "
            << eventStore()->name() );

    sc = eventStore()->record(pEvent,"McEventInfo");
    if( !sc.isSuccess() )  {
      ATH_MSG_ERROR ( "Error declaring event data object" );
      return -1;
    } 
  }
  
  return 1;
}

//---------------------------------------------------------------------------

void AthenaHiveEventLoopMgr::modifyEventContext(EventContext& ctx, const EventID& eID, bool consume_modifier_stream) {

  if(m_evtIdModSvc.isSet()) {
    EventID* new_eID=new EventID(eID);
    // interface to m_evtIdModSvc->modify_evtid wants to be able to
    // update the pointer itself, but in reality function doesn't need
    // it. And cannot obviously use a smart pointer here, as the
    // pointer itself can get updated by modify_evtid, so plain
    // pointer for now.
    // CHECK: Update evtIdModSvc method modify_evtid
    // interface to use just a pointer or even better a reference?
    m_evtIdModSvc->modify_evtid(new_eID, consume_modifier_stream);
    if (msgLevel(MSG::DEBUG)) {
      unsigned int oldrunnr=eID.run_number();
      unsigned int oldLB=eID.lumi_block();
      unsigned int oldTS=eID.time_stamp();
      unsigned int oldTSno=eID.time_stamp_ns_offset();
      ATH_MSG_DEBUG ( "modifyEventContext: use evtIdModSvc runnr=" << oldrunnr << " -> " << new_eID->run_number() );
      ATH_MSG_DEBUG ( "modifyEventContext: use evtIdModSvc LB=" << oldLB << " -> " << new_eID->lumi_block() );
      ATH_MSG_DEBUG ( "modifyEventContext: use evtIdModSvc TimeStamp=" << oldTS << " -> " << new_eID->time_stamp() );
      ATH_MSG_DEBUG ( "modifyEventContext: use evtIdModSvc TimeStamp ns Offset=" << oldTSno << " -> " << new_eID->time_stamp_ns_offset() );
    }
    ctx.setEventID( *new_eID );
    Atlas::getExtendedEventContext(ctx).setConditionsRun( ctx.eventID().run_number() );
    delete new_eID;
    return;
  }

  ctx.setEventID( eID );
}

//---------------------------------------------------------------------------
EventContext AthenaHiveEventLoopMgr::createEventContext() {

  EventContext ctx{ m_nevt, m_whiteboard->allocateStore( m_nevt ) };

  StatusCode sc = m_whiteboard->selectStore( ctx.slot() );
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Slot " << ctx.slot()
            << " could not be selected for the WhiteBoard" );
    return EventContext{};       // invalid EventContext
  } else {
    Atlas::setExtendedEventContext(ctx,
                                   Atlas::ExtendedEventContext( eventStore()->hiveProxyDict() ) );

    ATH_MSG_DEBUG ( "created EventContext, num: " << ctx.evt()  << "  in slot: "
            << ctx.slot() );
  }

  return ctx;
}

//---------------------------------------------------------------------------

int 
AthenaHiveEventLoopMgr::drainScheduler(int& finishedEvts){

  StatusCode sc(StatusCode::SUCCESS);
    
  // maybe we can do better
  std::vector<EventContext*> finishedEvtContexts;

  EventContext* finishedEvtContext(nullptr);

  // Here we wait not to loose cpu resources
  ATH_MSG_DEBUG ( "drainScheduler: [" << finishedEvts << "] Waiting for a context" );
  sc = m_schedulerSvc->popFinishedEvent(finishedEvtContext);

  // We got past it: cache the pointer
  if (sc.isSuccess()){
    ATH_MSG_DEBUG ( "drainScheduler: scheduler not empty: Context " 
            << finishedEvtContext );
    finishedEvtContexts.push_back(finishedEvtContext);
  } else{
    // no more events left in scheduler to be drained
    ATH_MSG_DEBUG ( "drainScheduler: scheduler empty" );
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
      ATH_MSG_FATAL ( "Detected nullptr ctxt while clearing WB!");
      fail = true;
      continue;
    }

    if (m_aess->eventStatus(*thisFinishedEvtContext) != EventStatus::Success) {
      ATH_MSG_FATAL ( "Failed event detected on " << thisFinishedEvtContext 
              << " w/ fail mode: "
              << m_aess->eventStatus(*thisFinishedEvtContext) );
      delete thisFinishedEvtContext;
      fail = true;
      continue;
    }
    
    EventID::number_type n_run(0);
    EventID::number_type n_evt(0);

    if (m_whiteboard->selectStore(thisFinishedEvtContext->slot()).isSuccess()) {
      n_run = thisFinishedEvtContext->eventID().run_number();
      n_evt = thisFinishedEvtContext->eventID().event_number();
    } else {
      ATH_MSG_ERROR ( "DrainSched: unable to select store "
	      << thisFinishedEvtContext->slot() );
      delete thisFinishedEvtContext;
      fail = true;
      continue;
    }

    // m_incidentSvc->fireIncident(Incident(name(), IncidentType::EndEvent,
    // 					 *thisFinishedEvtContext ));
    
    // Some code still needs global context in addition to that passed in the incident
    Gaudi::Hive::setCurrentContext( *thisFinishedEvtContext );
    m_incidentSvc->fireIncident(Incident(name(), IncidentType::EndProcessing, *thisFinishedEvtContext ));

    ATH_MSG_DEBUG ( "Clearing slot " << thisFinishedEvtContext->slot() 
            << " (event " << thisFinishedEvtContext->evt()
            << ") of the whiteboard" );
    
    StatusCode sc = clearWBSlot(thisFinishedEvtContext->slot());
    if (!sc.isSuccess()) {
      ATH_MSG_ERROR ( "Whiteboard slot " << thisFinishedEvtContext->slot() 
                      << " could not be properly cleared" );
      fail = true;
      delete thisFinishedEvtContext;
      continue;
    }
    
    finishedEvts++;

    writeHistograms().ignore();
    ++m_proc;

    if (m_doEvtHeartbeat) {
      if(!m_useTools) 
        ATH_MSG_INFO ( "  ===>>>  done processing event #" << n_evt << ", run #" << n_run 
               << " on slot " << thisFinishedEvtContext->slot() << ",  "
               << m_proc << " events processed so far  <<<===" );
      else 
	ATH_MSG_INFO ( "  ===>>>  done processing event #" << n_evt << ", run #" << n_run 
	       << " on slot " << thisFinishedEvtContext->slot() << ",  "          
	       << m_nev << " events read and " << m_proc 
	       << " events processed so far <<<===" );
      std::ofstream outfile( "eventLoopHeartBeat.txt");
      if ( !outfile ) {
	ATH_MSG_ERROR ( " unable to open: eventLoopHeartBeat.txt" );
	fail = true;
	delete thisFinishedEvtContext;
	continue;
      } else {
	outfile << "  done processing event #" << n_evt << ", run #" << n_run 
		<< " " << m_nev << " events read so far  <<<===" << std::endl;
	outfile.close();
      }  
    }

    ATH_MSG_DEBUG ( "drainScheduler thisFinishedEvtContext: " << thisFinishedEvtContext );
  

    delete thisFinishedEvtContext;
  }

  return (  fail ? -1 : 1 );

}

//---------------------------------------------------------------------------

StatusCode AthenaHiveEventLoopMgr::clearWBSlot(int evtSlot)  {
  return m_whiteboard->freeStore(evtSlot);  
}
//---------------------------------------------------------------------------

