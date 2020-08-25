///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ValgrindSvc.cxx 
// Implementation file for class ValgrindSvc
// Author: S.Binet<binet@cern.ch>
//         Frank Winklmeier
/////////////////////////////////////////////////////////////////// 

// valgrind includes
#include "valgrind/valgrind.h"
#include "valgrind/memcheck.h"
#include "valgrind/callgrind.h"

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Memory.h"

// Valkyrie includes
#include "ValgrindSvc.h"


/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
ValgrindSvc::ValgrindSvc( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_eventCounter(0),
  m_profileCounter(0)
{
  //
  // Property declaration
  // 
  declareProperty( "ProfiledAlgs", m_algs,
                   "List of profiled algorithms" );

  declareProperty( "ProfiledIntervals", m_intervals,
                   "Intervals to profile (e.g. 'MyAlg.initialize:MyAlg.finalize'" );
   
  declareProperty( "DumpAfterEachEvent", m_dumpAfterEachEvent = false,
                   "Dump separate profile after each event" );

  declareProperty( "DumpAfterEachInterval", m_dumpAfterEachInterval = true,
                   "Dump separate profile after each interval in ProfiledIntervals" );
     
  declareProperty( "DumpAfterIncident", m_dumpAfterIncident,
                   "List of incidents on which to dump a profile");
  
  declareProperty( "IgnoreFirstNEvents", m_ignoreFirstNEvents = 0,
                   "Do not profile the first N events");

  // for SvcHandle ? someday ?
  //declareInterface<IValgrindSvc>(this);
}

// Destructor
///////////////
ValgrindSvc::~ValgrindSvc()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ValgrindSvc::initialize()
{
  ATH_MSG_DEBUG
    ("Initializing " << name() << " package version " << PACKAGE_VERSION);
  
  ATH_MSG_INFO ("My process ID is [" << System::procID() << "]");
  
  const bool insideValgrind = static_cast<bool>(RUNNING_ON_VALGRIND);
  if ( insideValgrind ) {
    ATH_MSG_INFO ("=== Running from inside Valgrind ! Hi there! ===");
  }
  else {
    ATH_MSG_WARNING ("=== Valgrind is not running! ===");
  }
  
  // Use incidents in case there is no auditor configured
  if (!m_algs.empty() || !m_intervals.empty()) {

    // FIXME: We create the auditor here in C++ due to the lack of a
    // configurable auditor.
    makeAuditor("ValgrindAuditor",auditorSvc()).ignore();
    
    if ((auditorSvc()==0) || auditorSvc()->getAuditor("ValgrindAuditor")==0) {
      ATH_MSG_ERROR ("Cannot find [ValgrindAuditor]. ");
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_VERBOSE ("Found [ValgrindAuditor].");
    }
  }

  // Register incidents
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  ATH_CHECK(incSvc.retrieve());
  incSvc->addListener( this, IncidentType::BeginEvent );
  incSvc->addListener( this, IncidentType::EndEvent   );

  for(const std::string& incident : m_dumpAfterIncident) {
    incSvc->addListener( this, incident );
  }

  return StatusCode::SUCCESS;
 
}

StatusCode ValgrindSvc::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}


StatusCode ValgrindSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IValgrindSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IValgrindSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void ValgrindSvc::handle( const Incident& inc )
{
  // Only use Begin/EndEvent incident if no profiled algorithm or interval is set
  bool useEventIncident = m_intervals.empty() && m_algs.empty();

  // BeginEvent
  if ( inc.type() == IncidentType::BeginEvent ) {
    m_eventCounter++;
    if ( useEventIncident && m_eventCounter > m_ignoreFirstNEvents) {
      ATH_MSG_DEBUG ("[BeginEvent] Callgrind instrumentation ON.");
      callgrindStartInstrumentation();
    }
  }

  // EndEvent
  if ( inc.type() == IncidentType::EndEvent && (m_eventCounter > m_ignoreFirstNEvents) ) {    
    if ( useEventIncident ) {
      callgrindStopInstrumentation();
      ATH_MSG_DEBUG ("[EndEvent] Callgrind instrumentation OFF.");
    }
    if ( !m_algs.empty() && m_dumpAfterEachEvent ) {
      callgrindDumpStats(msg().stream());
      ATH_MSG_INFO ("Creating callgrind profile #" << profileCount()
                    << " for event #" << m_eventCounter);
    }    
  }

  // Optional additional dumps for user specified incidents
  if ( std::find(m_dumpAfterIncident.begin(),
                 m_dumpAfterIncident.end(),
                 inc.type()) != m_dumpAfterIncident.end() ) {
    callgrindDumpStats(msg().stream());
    ATH_MSG_INFO (" Creating callgrind profile #" << profileCount()
		  << " after incident [" << inc.type() << "].");
  }

}


/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

void ValgrindSvc::callgrindStartInstrumentation()
{
  CALLGRIND_START_INSTRUMENTATION;
}

void ValgrindSvc::callgrindStopInstrumentation()
{
  CALLGRIND_STOP_INSTRUMENTATION;
}

void ValgrindSvc::callgrindToggleCollect()
{
  CALLGRIND_TOGGLE_COLLECT;
}

void ValgrindSvc::callgrindDumpStats( std::ostream& /*out*/ )
{
  CALLGRIND_DUMP_STATS;
  m_profileCounter++;
}

void ValgrindSvc::valgrindDoLeakCheck()
{
  VALGRIND_DO_LEAK_CHECK;
}


// Copied from PerfMonComps/PerfMonUtils
// Would rather create the auditor from python but couldn't make it work
StatusCode ValgrindSvc::makeAuditor (const std::string& audName, 
				     IAuditorSvc* audSvc)
{
  if ( 0 == audSvc ) {
    ATH_MSG_ERROR ("Null pointer to IAuditorSvc !!");
    return StatusCode::FAILURE;
  }

  if ( 0 != audSvc->getAuditor( audName ) ) {
    ATH_MSG_VERBOSE ("AuditorSvc already knows about ["
		     << audName << "]... good.");
    return StatusCode::SUCCESS;
  }

  const std::string propName = "Auditors";
  IProperty * audSvcProp = dynamic_cast<IProperty*>(audSvc);

  if ( 0 == audSvcProp ) {
    ATH_MSG_ERROR ("Could not dyn-cast IAuditorSvc to an IProperty !!");
    return StatusCode::FAILURE;
  }

  StringArrayProperty audNames;
  audNames.assign( audSvcProp->getProperty(propName) );
  std::vector<std::string> updatedNames( audNames.value() );
  updatedNames.push_back( audName );
  audNames.set( updatedNames );
  audNames.setName( propName );
  if ( !audSvcProp->setProperty( audNames ).isSuccess() ) {
    ATH_MSG_ERROR
      ("Could not add [" << audName 
       << "] to the list of auditors of [AuditorSvc] !!"
	<< endmsg
       << audSvcProp->getProperty(propName));
    return StatusCode::FAILURE;
  }

  // make sure the auditor has been created...
  if ( 0 == audSvc->getAuditor( audName ) ) {
    ATH_MSG_ERROR ("Failed to make AuditorSvc instantiating [" 
		   << audName << "] !!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_VERBOSE ("[" << audName << "] successfully created.");
  return StatusCode::SUCCESS;
}
