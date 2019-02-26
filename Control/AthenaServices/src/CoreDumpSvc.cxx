/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   CoreDumpSvc.cxx
 * @brief  Implementation of the CoreDumpSvc
 * @author Frank Winklmeier
 *
 */

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

// Package includes
#include "CoreDumpSvc.h"
#include "SetFatalHandler.h"

// Gaudi includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/IAlgExecStateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/ConcurrencyFlags.h"
#include "GaudiKernel/EventContext.h"

// Athena includes
#include "AthenaKernel/IAthenaSummarySvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "CxxUtils/SealCommon.h"
#include "CxxUtils/SealSignal.h"
#include "CxxUtils/SealDebug.h"
#include "CxxUtils/read_athena_statm.h"


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
  CoreDumpSvc* coreDumpSvc(nullptr);      ///< pointer to CoreDumpSvc
  
  /**
   * Signal handler for the CoreDumpSvc
   */
  void action( int sig, siginfo_t *info, void* extra )
  {
    // Protect against additional signals while we are handling this one
    static std::atomic<int> inHandler {0};
    if (inHandler++ > 0) {
      if (inHandler > 100) _exit (99);
      return;
    }
    
    // setup timeout
    int timeoutMilliseconds = int(coreDumpSvc->m_timeout * 1e-6);
    
    coreDumpSvc->m_abortTimer.start(timeoutMilliseconds);
    
    std::cout.flush();
    std::cerr.flush();
    
    if (coreDumpSvc) {
      coreDumpSvc->setSigInfo(info);
      coreDumpSvc->print();
    }

    if (callOldHandler) {
    
      // Call previous signal handler
      // Need to distinguish between the two different types
      if (oldSigHandler[sig].sa_flags & SA_SIGINFO) {
        if (oldSigHandler[sig].sa_handler) oldSigHandler[sig].sa_sigaction(sig,info,extra);
      }
      else {
        if (oldSigHandler[sig].sa_handler) oldSigHandler[sig].sa_handler(sig);
      }      
    }

    if (coreDumpSvc && (sig == SIGSEGV || sig == SIGBUS || sig == SIGABRT) ) {
      // Exit now on a fatal signal; otherwise, we can hang.
      _exit (99);
    }
  }

}

//================================================================================
// C'tor, D'tor, Property handler
//================================================================================
CoreDumpSvc::CoreDumpSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  base_class( name, pSvcLocator ),
  m_abortTimer(0, 0, Athena::AlgorithmTimer::AlgorithmTimerConfig(Athena::AlgorithmTimer::USEREALTIME | 
                                                                  Athena::AlgorithmTimer::DELIVERYBYTHREAD ) )
{
  // Set us as the current instance
  CoreDumpSvcHandler::coreDumpSvc = this;
  
  m_callOldHandler.declareUpdateHandler(&CoreDumpSvc::propertyHandler, this);
  m_coreDumpStream.declareUpdateHandler(&CoreDumpSvc::propertyHandler, this);
  m_fatalHandlerFlags.declareUpdateHandler(&CoreDumpSvc::propertyHandler, this);
  
  // Allocate for 2 slots just for now.
  m_usrCoreDumps.resize(2);
  m_sysCoreDumps.resize(2); 
}

CoreDumpSvc::~CoreDumpSvc()
{
  CoreDumpSvcHandler::coreDumpSvc = nullptr;
}

void CoreDumpSvc::propertyHandler(Property& p)
{
  CoreDumpSvcHandler::callOldHandler = m_callOldHandler;

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
      if (m_fatalHandlerFlags != 0) {
        AthenaServices::SetFatalHandler( m_fatalHandlerFlags );
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
  if (m_fatalHandlerFlags != 0) {
    ATH_MSG_INFO("install f-a-t-a-l handler... (flag = " << m_fatalHandlerFlags.value() << ")");
    AthenaServices::SetFatalHandler(m_fatalHandlerFlags);
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

StatusCode CoreDumpSvc::start()
{
  auto numSlots = std::max<size_t>(1, Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents());
  m_usrCoreDumps.resize(numSlots);
  m_sysCoreDumps.resize(numSlots);
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

//================================================================================
// ICoreDumpSvc implementation
//================================================================================

//----------------------------------------------------------------------
// Set a name/value pair in the core dump record
//----------------------------------------------------------------------
void CoreDumpSvc::setCoreDumpInfo( const std::string& name, const std::string& value )
{
  setCoreDumpInfo(Gaudi::Hive::currentContext(), name, value);
}

void CoreDumpSvc::setCoreDumpInfo( const EventContext& ctx, const std::string& name, const std::string& value )
{
  auto slot = ctx.valid() ? ctx.slot() : 0;  
  m_usrCoreDumps.at(slot)[name] = value;
}

//----------------------------------------------------------------------
// Print all core dump records
//----------------------------------------------------------------------
void CoreDumpSvc::print()
{
  if (m_coreDumpStream == "MsgStream") {
    ATH_MSG_FATAL ("Core dump follows:" << std::endl << dump());
  }
  else if (m_coreDumpStream == "stderr") {
    std::cerr << dump() << std::endl;
  }
  else {
    std::cout << dump() << std::endl;
  }  
}

//----------------------------------------------------------------------
// Print all core dump records
//----------------------------------------------------------------------
std::string CoreDumpSvc::dump() const
{
  std::ostringstream os;
  const time_t now = time(nullptr);
  
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
  
  os << "Event counter: " << m_eventCounter << "\n";  


  IAlgExecStateSvc* algExecStateSvc(nullptr);
  IAlgContextSvc* algContextSvc(nullptr);

  // Use AlgExecStateSvc in MT, otherwise AlgContextSvc
  if (Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents() > 0) {
    service("AlgExecStateSvc", algExecStateSvc, /*createIf=*/ false).ignore();
  }
  else {
    service("AlgContextSvc", algContextSvc, /*createIf=*/ false).ignore();
  }

  // Loop over all slots
  for (size_t t=0; t < m_sysCoreDumps.size(); ++t){

    // Currently executing algorithm(s)
    std::string currentAlg;
    if (algExecStateSvc) {
      ATH_MSG_DEBUG("Using AlgExecStateSvc to determine current algorithm(s)");
      try {
        // We copy on purpose to avoid modification while we examine it
        auto states = algExecStateSvc->algExecStates(EventContext(0,t));
        for (const auto& kv : states) {
          if (kv.second.state()==AlgExecState::State::Executing)
            currentAlg += (kv.first + " ");
        }
      }
      catch (const GaudiException&) {  // can happen if we get called before any algo execution
        ATH_MSG_INFO("No information from AlgExecStateSvc because no algorithm was executed yet.");
      }
    }
    else if (algContextSvc) {
      ATH_MSG_DEBUG("Using AlgContextSvc to determine current algorithm");
      IAlgorithm* alg = algContextSvc->currentAlg();
      if (alg) currentAlg = alg->name();
    }
    else {
      ATH_MSG_WARNING("AlgExecStateSvc or AlgContextSvc not available. Cannot determine current algorithm.");
    }

    if (currentAlg.empty()) currentAlg = "<NONE>";
    os << "Slot " << std::setw(3) << t << " : Current algorithm = " << currentAlg << std::endl;
        
    // System core dump
    auto &sys = m_sysCoreDumps.at(t);
    if (!sys.LastInc.empty()) {
      os << "         : Last Incident = " << sys.LastInc << std::endl
         << "         : Event ID      = " << sys.EvId << std::endl;
    }
    
    // User core dump
    auto &usr = m_usrCoreDumps.at(t);
    if (!usr.empty()) {
      for (auto &s : usr) {
        os << "         : (usr) " << s.first << " = " << s.second << std::endl;
      }
    }
  }

  if (algContextSvc) {
    os << "Algorithm stack: ";
    if ( algContextSvc->algorithms().empty() ) os << "<EMPTY>" << "\n";
    else {
      os << "\n";
      for (auto alg : algContextSvc->algorithms()) {
        if (alg) os << "   " << alg->name() << "\n";
      }
    }
  }

  os << "-------------------------------------------------------------------------------------\n";
  os << "| AtlasBaseDir : " << std::setw(66) << getenv("AtlasBaseDir")  << " |\n";
  os << "| AtlasVersion : " << std::setw(66) << getenv("AtlasVersion")  << " |\n";
  os << "| CMTCONFIG    : " << std::setw(66) << getenv("CMTCONFIG")  << " |\n";
  os << "-------------------------------------------------------------------------------------\n";
  os << " Note: to see line numbers in below stacktrace you might consider running following :\n";
  os << "  atlasAddress2Line --file <logfile>\n";
  os << "-------------------------------------------------------------------------------------";

  IAthenaSummarySvc *iass(nullptr);
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
  //handle is single threaded in context;
  auto slot = incident.context().valid() ? incident.context().slot() : 0;  
  auto &currRec = m_sysCoreDumps.at(slot);

  currRec.LastInc = incident.source() + ":" + incident.type();

  std::ostringstream oss;
  oss << incident.context().eventID();
  currRec.EvId = oss.str();

  if (incident.type()==IncidentType::BeginEvent) {
    ++m_eventCounter;
  } else if (incident.type() == "StoreCleared") {
    // Try to force reallocation.
    auto newstr = currRec.EvId;
    // Intentional:
    // cppcheck-suppress selfAssignment
    newstr[0] = newstr[0];
    currRec.EvId = newstr;
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

  for (auto sig : m_signals) {
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

  for (const auto& kv : CoreDumpSvcHandler::oldSigHandler) {
    int ret = sigaction(kv.first, &(kv.second), nullptr);
    if ( ret!=0 ) {
      sc = StatusCode::FAILURE;
      ATH_MSG_WARNING("Error on uninstalling handler for signal " << kv.first
                      << ": " << strerror(errno));
    }
  }
  return sc;
}
