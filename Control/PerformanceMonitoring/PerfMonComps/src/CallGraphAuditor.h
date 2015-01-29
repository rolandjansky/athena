///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CallGraphAuditor.h 
// Header file for class CallGraphAuditor
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONCOMPS_CALLGRAPHAUDITOR_H 
#define PERFMONCOMPS_CALLGRAPHAUDITOR_H 

// STL includes

// FrameWork includes
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/ServiceHandle.h"

// Forward declaration
class INamedInterface;
class ICallGraphBuilderSvc;

namespace PerfMon {

class CallGraphAuditor : virtual public Auditor
{ 
 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor
  CallGraphAuditor(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~CallGraphAuditor();

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

  /////////////////////////////////////////////////////////////////// 
  // Private methods: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  typedef ServiceHandle<ICallGraphBuilderSvc> ICallGraphSvc_t;
  /// Handle to the @c ICallGraphSvc service which will build the call graph
  /// tree (and annotate it)
  ICallGraphSvc_t m_callGraphSvc;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} // end namespace PerfMon

#endif //> PERFMONCOMPS_CALLGRAPHAUDITOR_H
