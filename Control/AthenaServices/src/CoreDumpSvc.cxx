/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   CoreDumpSvc.cxx
 * @brief  Implementation of the CoreDumpSvc
 * @author Frank Winklmeier
 *
 * $Id: CoreDumpSvc.cxx,v 1.12 2008-12-15 13:24:23 fwinkl Exp $
 */

// Package includes
#include "CoreDumpSvc.h"
#include "SetFatalHandler.h"

// Gaudi includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/System.h"

// Athena includes
#include "AthenaKernel/IAthenaSummarySvc.h"
#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "StoreGate/StoreGateSvc.h"
#include "CxxUtils/SealCommon.h"
#include "CxxUtils/SealSignal.h"
#include "CxxUtils/SealDebug.h"
#include "CxxUtils/read_athena_statm.h"

// System includes
#include <ctime>
#include <cstdio>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#ifndef __APPLE__
#include <sys/sysinfo.h>
#else
#include <mach/task.h> 
#include <mach/mach_init.h>
#include <unistd.h>
#endif

using std::endl;

/**
 * @brief  Signal handler for CoreDumpSvc
 *
 * All information accessible from the signal handler is in this namespace.
 * It carries a pointer to the CoreDumpSvc instance. Therefore no static
 * methods are needed in the CoreDumpSvc to provide a function pointer.
 */
namespace CoreDumpSvcHandler
{
  typedef std::map<int, struct sigaction> SigHandler_t;
  
  SigHandler_t oldSigHandler;       ///< old signal handlers
  bool callOldHandler(true);        ///< forward calls to old handlers?
  CoreDumpSvc* coreDumpSvc(0);      ///< pointer to CoreDumpSvc
  
  /**
   * Signal handler for the CoreDumpSvc
   */
  void action( int sig, siginfo_t *info, void* extra )
  {
    // Protect against additional signals while we are handling this one
    static bool inHandler = false;
    if ( !inHandler ) inHandler = true;
    else return;
    
    // setup timeout
    int timeoutMilliseconds = int(coreDumpSvc->m_timeout * 1e-6);
    
    coreDumpSvc->m_abortTimer.start(timeoutMilliseconds);
    
    std::cout.flush();
    std::cerr.flush();
    
    if (coreDumpSvc) {
      coreDumpSvc->setSigInfo(info);
      coreDumpSvc->print();
    }

    if (!callOldHandler) return;
    
    // Call previous signal handler
    // Need to distinguish between the two different types
    if (oldSigHandler[sig].sa_flags & SA_SIGINFO) {
      if (oldSigHandler[sig].sa_handler) oldSigHandler[sig].sa_sigaction(sig,info,extra);
    }
    else {
      if (oldSigHandler[sig].sa_handler) oldSigHandler[sig].sa_handler(sig);
    }      
  }

}

//================================================================================
// C'tor, D'tor, Property handler
//================================================================================
CoreDumpSvc::CoreDumpSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService( name, pSvcLocator ),
  m_siginfo(0),
  m_eventCounter(0),
  m_abortTimer(0, NULL, Athena::AlgorithmTimer::AlgorithmTimerConfig(Athena::AlgorithmTimer::USEREALTIME | Athena::AlgorithmTimer::DELIVERYBYTHREAD ) )
{
  // Set us as the current instance
  CoreDumpSvcHandler::coreDumpSvc = this;
  
  declareProperty( "Signals", m_signals ,
                   "List of signals to catch" );
  
  declareProperty( "CallOldHandler", m_callOldHandler = true ,
                   "Call previous signal handler" );
  
  declareProperty( "CoreDumpStream", m_coreDumpStream = "stdout",
                   "Stream to use for core dump [stdout,stderr,MsgStream]" );

  /*
    USR1_DUMP_CORE        =   1
    FATAL_ON_QUIT         =   2 x
    FATAL_ON_INT          =   4 x
    FATAL_DUMP_CORE       =   8
    FATAL_DUMP_SIG        =  16 x
    FATAL_DUMP_STACK      =  32 x
    FATAL_DUMP_LIBS       =  64 
    FATAL_DUMP_CONTEXT    = 128 x
    FATAL_AUTO_EXIT       = 256 x
     ==> 438
  */
  declareProperty( "FatalHandler",
		   m_fatalHandlerFlags = 0,
		   "Flags given to the fatal handler this service installs"
		   "\n"
		   "if the flag is zero, no fatal handler is installed"
		   );

  declareProperty( "TimeOut",
		   m_timeout=30.0*60* 1000* 1000* 1000,
		   "Terminate job after it reaches the time out in Wallclock time, usually due to hanging during the stack unwinding. Timeout given in Nanoseconds (official ATLAS units), despite its millisecond resolution. Default is 30 minutes" );
  
  m_callOldHandler.declareUpdateHandler(&CoreDumpSvc::propertyHandler, this);
  m_coreDumpStream.declareUpdateHandler(&CoreDumpSvc::propertyHandler, this);
  m_fatalHandlerFlags.declareUpdateHandler(&CoreDumpSvc::propertyHandler, this);
  
  // Default signals to catch
  std::vector<int> sigs;
  sigs.push_back(SIGSEGV);
  sigs.push_back(SIGBUS);
  sigs.push_back(SIGILL);
  sigs.push_back(SIGFPE);
  m_signals.setValue(sigs);
}

CoreDumpSvc::~CoreDumpSvc()
{
  CoreDumpSvcHandler::coreDumpSvc = 0;
}

void CoreDumpSvc::propertyHandler(Property& p)
{
  CoreDumpSvcHandler::callOldHandler = m_callOldHandler.value();

  if ( p.name()==m_coreDumpStream.name() ) {
    const std::string val = p.toString();
    if ( val=="stdout" || val=="stderr" || val=="MsgStream" ) return;
    else {
      ATH_MSG_WARNING
	("'" << val << "' not valid for " << m_coreDumpStream.name()
	 << ": " << m_coreDumpStream.documentation());
    }
  } else if ( p.name() == m_fatalHandlerFlags.name() ) {
    if (m_fatalHandlerFlags.fromString(p.toString()).isSuccess()) {
      const int flag = m_fatalHandlerFlags.value();
      if (flag != 0) {
	AthenaServices::SetFatalHandler(flag);
      }
    } else {
      ATH_MSG_INFO("could not convert [" << p.toString() << "] to integer");
    }
  }

}

//================================================================================
// IService implementation
//================================================================================
StatusCode CoreDumpSvc::initialize()
{
  // Reset members
  m_usrCoreDump.clear();
  m_sysCoreDump.clear();
  m_eventCounter = 0;
  
  ATH_MSG_DEBUG
    ("Initializing " << name()
     << " package version " << PACKAGE_VERSION);
   
  {
    int flag = m_fatalHandlerFlags.value();
    if (flag != 0) {
      ATH_MSG_INFO("install f-a-t-a-l handler... (flag = " << flag << ")");
      AthenaServices::SetFatalHandler(flag);
    }
  }

  if ( installSignalHandler().isFailure() ) {
    ATH_MSG_ERROR ("Could not install signal handlers");
    return StatusCode::FAILURE;
  }

  // Register incident handler
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
  if ( !incSvc.retrieve().isSuccess() ) {
    ATH_MSG_WARNING ("Unable to retrieve the IncidentSvc");
  }
  else {
    incSvc->addListener(this, IncidentType::BeginRun);
    incSvc->addListener(this, IncidentType::BeginEvent);
    incSvc->addListener(this, IncidentType::EndRun);
    incSvc->addListener(this, IncidentType::EndEvent);
    incSvc->addListener(this,"StoreCleared");
  }
  
  return StatusCode::SUCCESS;
}

StatusCode CoreDumpSvc::finalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name());

  if ( uninstallSignalHandler().isFailure() ) {
    ATH_MSG_WARNING ("Could not uninstall signal handlers");
    return StatusCode::FAILURE;
  }
   
  return StatusCode::SUCCESS;
}

StatusCode CoreDumpSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( ppvInterface==0 ) return StatusCode::FAILURE;
  
  if ( ICoreDumpSvc::interfaceID().versionMatch(riid) )
    *ppvInterface = static_cast<ICoreDumpSvc*>(this);
  else return Service::queryInterface(riid, ppvInterface);
  
  addRef();
  return StatusCode::SUCCESS;
}


//================================================================================
// ICoreDumpSvc implementation
//================================================================================

//----------------------------------------------------------------------
// Set a name/value pair in the core dump record
//----------------------------------------------------------------------
void CoreDumpSvc::setCoreDumpInfo( const std::string& name, const std::string& value )
{
  m_usrCoreDump[name] = value;
}


//----------------------------------------------------------------------
// Print all core dump records
//----------------------------------------------------------------------
void CoreDumpSvc::print()
{
  if (m_coreDumpStream.value()=="MsgStream") {
    ATH_MSG_FATAL ("Core dump follows:" << endl << dump());
  }
  else if (m_coreDumpStream.value()=="stderr") {
    std::cerr << dump() << endl;
  }
  else {
    std::cout << dump() << endl;
  }  
}

//----------------------------------------------------------------------
// Print all core dump records
//----------------------------------------------------------------------
std::string CoreDumpSvc::dump() const
{
  std::ostringstream os;
  const time_t now = time(0);
  
  os << "-------------------------------------------------------------------------------------" << "\n";
  os << "Core dump from " << name() << " on " << System::hostName()
     << " at " << ctime(&now) /*<< "\n"*/; // ctime adds "\n"
  os << "\n";

  // Print additional information if available
  if (m_siginfo) {
    int signo = m_siginfo->si_signo;  // shorthand
    
    os << "Caught signal " << signo
       << "(" << strsignal(signo) << "). Details: "
       << "\n";   

    os << "  errno = " << m_siginfo->si_errno
       << ", code = " << m_siginfo->si_code
       << " (" << Athena::Signal::describe(signo, m_siginfo->si_code) << ")"
       << "\n";
    
    os << "  pid   = " << m_siginfo->si_pid
       << ", uid = " << m_siginfo->si_uid
       << "\n";
    
#ifndef __APPLE__
    // These are set if the POSIX signal sender passed them.
    os << "  value = (" << m_siginfo->si_int << ", "
       << std::hex << m_siginfo->si_ptr << ")" << std::dec << "\n";
#endif

    // memory usage informations
    athena_statm s = read_athena_statm();
    
    const long pagesz = sysconf(_SC_PAGESIZE);
    os << "  vmem = " << s.vm_pages*pagesz/1024./1024.  << " MB\n"
       << "  rss  = " << s.rss_pages*pagesz/1024./1024. << " MB\n";

#ifndef __APPLE__
    // more memory usage informations (system wide stuff)
    // see sysinfo(2)

    {
      struct sysinfo sys;
      if ( 0 == sysinfo(&sys) ) {
        // all sizes are reported in sys.mem_unit bytes
        const float mem_units = sys.mem_unit/(1024.*1024.);
        os << "  total-ram = " << sys.totalram * mem_units << " MB\n"
           << "  free-ram  = " << sys.freeram  * mem_units << " MB\n"
           << "  buffer-ram= " << sys.bufferram* mem_units << " MB\n"
           << "  total-swap= " << sys.totalswap* mem_units << " MB\n"
           << "  free-swap = " << sys.freeswap * mem_units << " MB\n";
      }
    }
#endif

    // This is the interesting address for memory faults.
    if (signo == SIGILL || signo == SIGFPE || signo == SIGSEGV || signo == SIGBUS)
      os << "  addr  = " << std::hex << m_siginfo->si_addr << std::dec << "\n";
    
    os << "\n";
  }
  
  CoreDump_t::const_iterator iter;
  // System core dump
  os << "Event counter: " << m_eventCounter << "\n";  
  for ( iter=m_sysCoreDump.begin(); iter!=m_sysCoreDump.end(); ++iter ) {
    os << iter->first << ": " << iter->second << "\n";
  }
  // User core dump
  for ( iter=m_usrCoreDump.begin(); iter!=m_usrCoreDump.end(); ++iter ) {
    os << iter->first << ": " << iter->second << "\n";
  }

  // Check if the AlgContextSvc is running
  IAlgContextSvc* algContextSvc(0);
  if (service("AlgContextSvc", algContextSvc, /*createIf=*/ false).isSuccess() && algContextSvc) {

    IAlgorithm* alg = algContextSvc->currentAlg();
    os << "Current algorithm: " << (alg ? alg->name() : "<NONE>") << "\n";

    os << "Algorithm stack: ";
    if ( algContextSvc->algorithms().size()==0 ) os << "<EMPTY>" << "\n";
    else {
      os << "\n";
      IAlgContextSvc::Algorithms::const_iterator iter;
      for ( iter=algContextSvc->algorithms().begin();
            iter!=algContextSvc->algorithms().end();
            ++iter) {
        if (*iter) os << "   " << (*iter)->name() << "\n";
      }
    }
    algContextSvc->release();
  }
  os << "-------------------------------------------------------------------------------------\n";
  os << "| AtlasBaseDir : " << std::setw(66) << getenv("AtlasBaseDir")  << " |\n";
  os << "| AtlasVersion : " << std::setw(66) << getenv("AtlasVersion")  << " |\n";
  os << "| CMTCONFIG    : " << std::setw(66) << getenv("CMTCONFIG")  << " |\n";
  os << "-------------------------------------------------------------------------------------\n";
  os << " Note: to see line numbers in below stacktrace you might consider running following :\n";
  os << "  atlasAddress2Line --file <logfile>\n";
  os << "-------------------------------------------------------------------------------------";

  IAthenaSummarySvc *iass(0);
  if (service("AthenaSummarySvc",iass,false).isSuccess() && iass) {
    iass->addSummary("CoreDumpSvc",os.str());
    iass->setStatus(1);
    iass->createSummary().ignore();
  }

  
  return os.str();
}

//================================================================================
// IIncidentHandler implementation
//================================================================================

void CoreDumpSvc::handle(const Incident& incident)
{
  m_sysCoreDump["Last incident"] = incident.source() + ":" + incident.type();

  const EventIncident* eventInc(0);
  if (0 != (eventInc = dynamic_cast<const EventIncident*>(&incident))) {
    const EventID* eventID = eventInc->eventInfo().event_ID();
    if (eventID) {
      std::ostringstream oss;
      oss << *eventID;
      m_sysCoreDump["EventID"] = oss.str();
    }
  }

  if (incident.type()==IncidentType::BeginEvent) {
    ++m_eventCounter;
  }
  else if (incident.type() == "StoreCleared") {
    // Try to force reallocation.
    std::string newstr = m_sysCoreDump["EventID"];
    newstr[0] = newstr[0];
    m_sysCoreDump["EventID"] = newstr;
  }

}

//================================================================================
// Helpers for signal handler
//================================================================================

//----------------------------------------------------------------------
// Install signal handler
//----------------------------------------------------------------------
StatusCode CoreDumpSvc::installSignalHandler()
{
  ATH_MSG_DEBUG ("Installing signal handler");
  std::ostringstream oss;
  std::vector<int>::const_iterator iter;

  for ( iter=m_signals.value().begin(); iter!=m_signals.value().end(); ++iter ) {
    const int sig = *iter;
#ifndef __APPLE__
    if (sig<1 || sig>SIGRTMAX) {
      ATH_MSG_WARNING ("Invalid signal number " << sig << ". Ignoring.");
      continue;
    }
#endif
    oss << sig << "(" << strsignal(sig) << ") ";
    
    // Install new signal handler and backup old one
    struct sigaction sigact;
    memset (&sigact, 0, sizeof(sigact));
    sigact.sa_sigaction = CoreDumpSvcHandler::action;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    int ret = sigaction(sig, &sigact, &(CoreDumpSvcHandler::oldSigHandler[sig]));
    if ( ret!=0 ) {
      ATH_MSG_ERROR ("Error on installing handler for signal " << sig
		     << ": " << strerror(errno));
      return StatusCode::FAILURE;
    }     
  }
  ATH_MSG_INFO ("Handling signals: " << oss.str());
  
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Uninstall signal handler
//----------------------------------------------------------------------
StatusCode CoreDumpSvc::uninstallSignalHandler()
{
  ATH_MSG_DEBUG ("Uninstalling signal handler");

  StatusCode sc = StatusCode::SUCCESS;
  CoreDumpSvcHandler::SigHandler_t::const_iterator iter;
  for ( iter=CoreDumpSvcHandler::oldSigHandler.begin();
        iter!=CoreDumpSvcHandler::oldSigHandler.end(); ++iter ) {
    
    int ret = sigaction(iter->first, &(iter->second), 0);
    if ( ret!=0 ) {
      sc = StatusCode::FAILURE;
      ATH_MSG_WARNING
	("Error on uninstalling handler for signal " << iter->first
	 << ": " << strerror(errno));
    }
  }
  return sc;
}
