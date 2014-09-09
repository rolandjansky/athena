// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENASERVICES_COREDUMPSVC_H
#define ATHENASERVICES_COREDUMPSVC_H 1

/**
 * @file   CoreDumpSvc.h
 * @brief  Interface of the CoreDumpSvc
 * @author Frank Winklmeier
 *
 * $Id: CoreDumpSvc.h,v 1.6 2008-12-15 13:24:23 fwinkl Exp $
 */

// Package includes
#include "AthenaKernel/ICoreDumpSvc.h"
#include "AthenaKernel/AlgorithmTimer.h"

// FrameWork includes
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"
#include "EventInfo/EventID.h"

// System includes
#include <string>
#include <map>
#include <signal.h>

// Forward declarations
template <class TYPE> class SvcFactory;

namespace CoreDumpSvcHandler {
  void action( int sig, siginfo_t *info, void* extra );
}

/**
 * @class  CoreDumpSvc
 * @brief  Service to print additional information before a crash
 * @author Frank Winklmeier
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

class CoreDumpSvc : virtual public ICoreDumpSvc,
                    virtual public IIncidentListener,
                    public AthService {

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

  /// Print all core dump records
  virtual std::string dump() const;
  //@}


  /// \name Gaudi implementation
  //@{
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  /// Incident listener
  virtual void handle( const Incident& incident );
  //@}

    
private:
  typedef std::map<std::string, std::string> CoreDump_t;   ///< Type of core dump records
  
  CoreDump_t m_usrCoreDump;   ///< User defined core dump info
  CoreDump_t m_sysCoreDump;   ///< Core dump info collected by this service  
  siginfo_t* m_siginfo;       ///< Pointer to siginfo_t struct (set by signal handler)
  EventID::number_type m_eventCounter;       ///< Event counter
  
  // Properties
  IntegerArrayProperty  m_signals;         ///< Signals to catch
  BooleanProperty       m_callOldHandler;  ///< Call previous handlers?
  StringProperty        m_coreDumpStream;  ///< Stream to use for printouts
  IntegerProperty       m_fatalHandlerFlags; ///< configuration flag(s) given
                                             ///< to the fatal handler

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
  
  /// set time out during above protection, default is 15min
  double m_timeout;

}; 


#endif
