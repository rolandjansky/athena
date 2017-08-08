/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_COREDUMPSVC_H
#define ATHENASERVICES_COREDUMPSVC_H 1

// System includes
#include <signal.h>
#include <string>
#include <atomic>
#include <tbb/concurrent_unordered_map.h>

// Package includes
#include "AthenaKernel/ICoreDumpSvc.h"
#include "AthenaKernel/AlgorithmTimer.h"

// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"
#include "EventInfo/EventID.h"


// Forward declarations
template <class TYPE> class SvcFactory;

namespace CoreDumpSvcHandler {
  void action( int sig, siginfo_t *info, void* extra );
}

/**
 * @class  CoreDumpSvc
 * @brief  Service to print additional information before a crash
 * @author Frank Winklmeier
 * @author Sami Kama
 *
 * This service will catch fatal signals and print its internal core
 * dump record. The service collects some information during event
 * processing. Additional information can be added via setCoreDumpInfo().
 *
 * To use this service do:                                  @verbatim
      from AthenaServices.Configurables import CoreDumpSvc
      svcMgr += CoreDumpSvc()                               @endverbatim
 *
 * For a list of job option properties see CoreDumpSvc::CoreDumpSvc(). 
 */

class CoreDumpSvc : public extends<AthService, 
                                   ICoreDumpSvc,
                                   IIncidentListener> {

protected:
  friend class SvcFactory<CoreDumpSvc>;
  friend void CoreDumpSvcHandler::action( int sig, siginfo_t *info, void* extra );
  
  /// Default constructor (do not use)
  CoreDumpSvc();
  
public: 

  /// Constructor with parameters
  CoreDumpSvc( const std::string& name, ISvcLocator* pSvcLocator );
  
  /// Destructor
  virtual ~CoreDumpSvc(); 
  
  /// \name ICoreDumpSvc implementation
  //@{  
  /// Set a name/value pair in the core dump record
  virtual void setCoreDumpInfo( const std::string& name, const std::string& value );

  /// Set a name/value pair in the core dump record for given EventContext
  virtual void setCoreDumpInfo( const EventContext& ctx, const std::string& name, const std::string& value );

  /// Print all core dump records
  virtual std::string dump() const;
  //@}


  /// \name Gaudi implementation
  //@{
  virtual StatusCode initialize();
  virtual StatusCode start();
  virtual StatusCode finalize();
  
  /// Incident listener
  virtual void handle( const Incident& incident );
  //@}

    
private:
  struct sysDumpRec{
    std::string LastInc;
    std::string EvId;
  };
  typedef tbb::concurrent_unordered_map<std::string,std::string > UserCore_t;
  std::vector<UserCore_t>  m_usrCoreDumps;               ///< User defined core dump info
  std::vector<sysDumpRec> m_sysCoreDumps;                ///< Core dump info collected by this service  
  siginfo_t* m_siginfo{nullptr};                         ///< Pointer to siginfo_t struct (set by signal handler)
  std::atomic<EventID::number_type> m_eventCounter{0};   ///< Event counter
  
  ///@{ Properties

  Gaudi::Property<std::vector<int>> m_signals{this, "Signals", {SIGSEGV,SIGBUS,SIGILL,SIGFPE}, 
      "List of signals to catch"};

  Gaudi::Property<bool> m_callOldHandler{this, "CallOldHandler", true, 
      "Call previous signal handler"};

  Gaudi::Property<std::string> m_coreDumpStream{this, "CoreDumpStream", "stdout",
      "Stream to use for core dump [stdout,stderr,MsgStream]"};

  Gaudi::Property<int> m_fatalHandlerFlags{this, "FatalHandler", 0, 
      "Flags given to the fatal handler this service installs\n"
      "if the flag is zero, no fatal handler is installed"};

  Gaudi::Property<double> m_timeout{this, "TimeOut", 30.0*60*1e9,
      "Terminate job after it this reaches the time out in Wallclock time, "
      "usually due to hanging during stack unwinding. Timeout given in Nanoseconds."};
	   
  ///@}

  /// Property handler
  void propertyHandler(Property& p);

  /// Print core dump records to configured stream
  void print();
  
  /// Set pointer to siginfo_t struct
  void setSigInfo(siginfo_t* info) { m_siginfo = info; }  
  
  /// Install signal handlers
  StatusCode installSignalHandler();
  
  /// Uninstall signal handlers
  StatusCode uninstallSignalHandler(); 
  
  /// Algorithm timer to terminate job if it's looping during stack unwinding
  Athena::AlgorithmTimer m_abortTimer;  
}; 


#endif
