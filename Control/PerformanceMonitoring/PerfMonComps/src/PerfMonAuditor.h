///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PerfMonAuditor.h 
// Header file for class Athena::PerfMonAuditor
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONCOMPS_PERFMONAUDITOR_H 
#define PERFMONCOMPS_PERFMONAUDITOR_H 

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/ServiceHandle.h"

// Forward declaration
class INamedInterface;
class IPerfMonSvc;

namespace Athena {
class PerfMonAuditor : virtual public Auditor
{ 
  using Auditor::before;
  using Auditor::after;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor
  PerfMonAuditor(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~PerfMonAuditor();

  /// Gaudi hooks
  virtual StatusCode initialize();

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

  typedef ServiceHandle<IPerfMonSvc> PerfMonSvc_t;
  /// Pointer to the @c IPerfMonSvc
  PerfMonSvc_t m_monSvc;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} // !namespace Athena

#endif //> PERFMONCOMPS_PERFMONAUDITOR_H
