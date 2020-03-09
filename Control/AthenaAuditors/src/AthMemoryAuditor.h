///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// AthMemoryAuditor.h 
// Header file for class AthMemoryAuditor
// Author: Rolf Seuster
/////////////////////////////////////////////////////////////////// 
#ifndef ATHENASERVICES_ATHMEMORYAUDITOR_H 
#define ATHENASERVICES_ATHMEMORYAUDITOR_H 1

// This auditor is not thread-safe without signiicant work.
// Disable checking for now.
// We'll also report an ERROR is this is used in an MT job.
#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/Auditor.h"
#include "AthenaBaseComps/AthMessaging.h"

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
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual void before(StandardEventType evt, INamedInterface* comp) override;
  virtual void after(StandardEventType evt, INamedInterface* comp, const StatusCode& sc) override;
  
  static bool m_usetcmalloc;
  
private:

  bool m_reported;
  int m_stmax;
  int m_defaultStacktraceDepth;
  std::vector<std::string> m_depthPerAlg;
  
  void report ();
  std::string stageToString(long);
  uint64_t sum(uint32_t a, uint32_t b) { return ((((uint64_t)a)<<32) + b); };  
}; 
#endif //> ATHENASERVICES_ATHMEMORYAUDITOR_H
