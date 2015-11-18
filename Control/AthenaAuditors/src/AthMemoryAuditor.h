///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthMemoryAuditor.h 
// Header file for class AthMemoryAuditor
// Author: Rolf Seuster
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_ATHMEMORYAUDITOR_H 
#define ATHENASERVICES_ATHMEMORYAUDITOR_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Incident.h"

// #include <time.h>

// Forward declaration
class INamedInterface;

class AthMemoryAuditor : virtual public Auditor, public AthMessaging
{ 
  using Auditor::before;
  using Auditor::after;
  using AthMessaging::msg;
  
public: 
  
  /// Constructor
  AthMemoryAuditor(const std::string& name, ISvcLocator* pSvcLocator);
  
  /// Destructor
  virtual ~AthMemoryAuditor();
  
  virtual StatusCode initialize();

  virtual StatusCode finalize();
  
  virtual void beforeInitialize(INamedInterface* alg);
  virtual void beforeReinitialize(INamedInterface* alg);
  virtual void beforeExecute(INamedInterface* alg);
  virtual void beforeBeginRun(INamedInterface* alg);
  virtual void beforeEndRun(INamedInterface* alg);
  virtual void beforeFinalize(INamedInterface* alg);

  // add after* hooks to reset name of algorithm to "framework"
  virtual void afterInitialize(INamedInterface* alg);
  virtual void afterReinitialize(INamedInterface* alg);
  virtual void afterExecute(INamedInterface* alg, const StatusCode& sc);
  virtual void afterBeginRun(INamedInterface* alg);
  virtual void afterEndRun(INamedInterface* alg);
  virtual void afterFinalize(INamedInterface* alg);

  bool m_reported;
  
  static bool m_usetcmalloc;
  
private: 

  int m_stmax;
  
  int m_defaultStacktraceDepth;
  
  std::vector<std::string> m_depthPerAlg;
  
  void report ();
  
  std::string stageToString(long);
  
  uint64_t sum(uint32_t a, uint32_t b) { return ((((uint64_t)a)<<32) + b); };  
}; 
#endif //> ATHENASERVICES_ATHMEMORYAUDITOR_H
