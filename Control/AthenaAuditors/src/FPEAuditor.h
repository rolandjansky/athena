///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include <atomic>

// FrameWork includes
#include "GaudiKernel/Auditor.h"
#include "AthenaBaseComps/AthCommonMsg.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"


#include <signal.h>
#include <fenv.h>

// Forward declaration
class INamedInterface;


class FPEAuditor : public AthCommonMsg<Auditor>
{ 
  using Auditor::before;
  using Auditor::after;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor
  FPEAuditor(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~FPEAuditor();

  /// Gaudi hooks
  virtual StatusCode initialize() override;
  
  virtual StatusCode finalize() override;
  

  // standard event auditing...
  virtual void before(StandardEventType evt, INamedInterface* comp) override;
  virtual void after(StandardEventType evt, INamedInterface* comp, const StatusCode& sc) override;

  // custom event auditing...

  /// Audit the start of a custom "event".
  virtual void before(IAuditor::CustomEventTypeRef evt, 
		      INamedInterface* caller) override
  { return this->before (evt, caller->name()); }

  /**
   * Audit the start of a custom "event" for callers that do not implement 
   * the @c INamedInterface.
   */
  virtual void before (IAuditor::CustomEventTypeRef evt, 
		       const std::string& caller) override;
  
  /// Audit the end of a custom "event".
  virtual void after (IAuditor::CustomEventTypeRef evt, 
		      INamedInterface* caller, 
		      const StatusCode& sc) override
  { return this->after (evt, caller->name(), sc); }
  
  /**
   * Audit the end of a custom "event" for callers that do not implement 
   * the @c INamedInterface.
   */
  virtual void after  (CustomEventTypeRef evt, const std::string& caller,
		       const StatusCode& sc) override;

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

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
  static thread_local FpeStack_t s_fpe_stack;
  
  enum { FPEAUDITOR_OVERFLOW=0, FPEAUDITOR_INVALID=1, FPEAUDITOR_DIVBYZERO=2, FPEAUDITOR_ARRAYSIZE=3 };
  
  std::atomic<unsigned int> m_CountFPEs[FPEAUDITOR_ARRAYSIZE];
  
  unsigned int m_NstacktracesOnFPE;
  
  void InstallHandler();

  void UninstallHandler();
  
  //fexcept_t m_flagp;
  
  /// The FP environment before we initialize.
  fenv_t m_env;

  std::atomic<int> m_nexceptions;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> ATHENASERVICES_FPEAUDITOR_H
