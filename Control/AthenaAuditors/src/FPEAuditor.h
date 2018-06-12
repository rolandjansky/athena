///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// FPEAuditor.h 
// Header file for class FPEAuditor
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_FPEAUDITOR_H 
#define ATHENASERVICES_FPEAUDITOR_H 1

// STL includes
#include <string>
#include <list>
#include <utility> // for std::pair

// FrameWork includes
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMessaging.h"

#include <signal.h>
#include <fenv.h>

// Forward declaration
class INamedInterface;


class FPEAuditor : virtual public Auditor, public AthMessaging
{ 
  using Auditor::before;
  using Auditor::after;
  using AthMessaging::msg;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor
  FPEAuditor(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~FPEAuditor();

  /// Gaudi hooks
  virtual StatusCode initialize();
  
  virtual StatusCode finalize();
  
  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  virtual void beforeInitialize(INamedInterface* alg);
  virtual void afterInitialize(INamedInterface* alg);
  virtual void beforeReinitialize(INamedInterface* alg);
  virtual void afterReinitialize(INamedInterface* alg);
  virtual void beforeExecute(INamedInterface* alg);
  virtual void afterExecute(INamedInterface* alg, const StatusCode&);
  virtual void beforeBeginRun(INamedInterface* alg);
  virtual void afterBeginRun(INamedInterface *alg);
  virtual void beforeEndRun(INamedInterface* alg);
  virtual void afterEndRun(INamedInterface *alg);
  virtual void beforeFinalize(INamedInterface* alg);
  virtual void afterFinalize(INamedInterface* alg);

  // custom event auditing...

  /// Audit the start of a custom "event".
  virtual void before(IAuditor::CustomEventTypeRef evt, 
		      INamedInterface* caller)
  { return this->before (evt, caller->name()); }

  /**
   * Audit the start of a custom "event" for callers that do not implement 
   * the @c INamedInterface.
   */
  virtual void before (IAuditor::CustomEventTypeRef evt, 
		       const std::string& caller);
  
  /// Audit the end of a custom "event".
  virtual void after (IAuditor::CustomEventTypeRef evt, 
		      INamedInterface* caller, 
		      const StatusCode& sc)
  { return this->after (evt, caller->name(), sc); }
  
  /**
   * Audit the end of a custom "event" for callers that do not implement 
   * the @c INamedInterface.
   */
  virtual void after  (CustomEventTypeRef evt, const std::string& caller,
		       const StatusCode& sc);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  std::string m_evtInfoKey;

  /** report fpes which happened during step 'step' on behalf of 'caller'
   */
  void report_fpe(const std::string& step, const std::string& caller);

  /** add an fpe node
   */
  void add_fpe_node();

  /** pop an fpe node
   */
  void pop_fpe_node();

  typedef std::pair<int,int>   FpeNode_t;
  typedef std::list<FpeNode_t> FpeStack_t;
  /** a stack of FPE exceptions which have been raised
   */
  FpeStack_t m_fpe_stack;
  
  enum { FPEAUDITOR_OVERFLOW=0, FPEAUDITOR_INVALID=1, FPEAUDITOR_DIVBYZERO=2, FPEAUDITOR_ARRAYSIZE=3 };
  
  unsigned int m_CountFPEs[FPEAUDITOR_ARRAYSIZE];
  
  unsigned int m_NstacktracesOnFPE;
  
  void InstallHandler();

  void UninstallHandler();
  
  bool m_SigHandInstalled;
  
  //fexcept_t m_flagp;
  
  /// The FP environment before we initialize.
  fenv_t m_env;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ATHENASERVICES_FPEAUDITOR_H
