//   --*- c++ -*--

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAAUDITORS_COWAUDITOR
#define ATHENAAUDITORS_COWAUDITOR
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include "CoWTools/CoWLibrary.h"
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TTree.h"

class INamedInterface;
class Incident;

class CoWAuditor : virtual public Auditor, public AthMessaging,public IIncidentListener
{ 
  using Auditor::before;
  using Auditor::after;
  using AthMessaging::msg;

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor
  CoWAuditor(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~CoWAuditor();

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

  virtual void handle(const Incident&);

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 
  typedef std::unordered_map<std::string,std::shared_ptr<CoWTools::CoWLibrary> > LibMap_t;
  bool checkChange(std::shared_ptr<LibMap_t> &capture);
  void diffDetailedMaps(std::shared_ptr<LibMap_t> &dst,std::shared_ptr<LibMap_t> &src);
  bool m_dumpFinalize,m_dumpInfo,m_detailed;
  std::string m_streamName;
  LibMap_t m_libMap;
  std::vector<std::shared_ptr<CoWTools::CoWLibrary> > m_summaryStack;
  std::vector<std::shared_ptr<LibMap_t> > m_detailedStack;
  bool pushStats(const std::string&);
  bool popStats(const std::string&);
  std::shared_ptr<LibMap_t> parseDetailedSmaps();
  std::shared_ptr<CoWTools::CoWLibrary> parseSmaps();
  std::unordered_map<std::string,CoWTools::CoWRecordStats> m_runTotals;
  std::unordered_set<std::size_t> m_existingHashes;//for potential collisions
  std::unordered_map<std::string,std::size_t> m_algHashes;
  TTree *m_hashTree;
  TTree *m_snapshotTree;
  std::size_t m_algId;
  long m_vmem,m_rss,m_pss,m_shared,m_private,m_anon,m_swap;
  ULong64_t m_currTime;
  std::hash<std::string> m_hasher;
  CoWTools::CoWRecordStats m_ms;
}; 

#endif
