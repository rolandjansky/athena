/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VALKYRIE_VALGRINDAUDITOR_H
#define VALKYRIE_VALGRINDAUDITOR_H

// STL/Boost includes
#include <string>
#include <vector>
#include <utility>
#include <boost/regex.hpp>

// FrameWork includes
#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GAUDI_VERSION.h"

// Forward declaration
class INamedInterface;
class IValgrindSvc;

//////////////////////////////////////////////////////////////////////
/// Valgrind auditor.
///
/// Gaudi auditor to programmatically control valgrind.
/// Currently only callgrind controls are implemented.
/// Turns callgrind instrumentation on/off before/afterExecute.
///
/// @author Frank Winklmeier
/// $Id: ValgrindAuditor.h,v 1.4 2008-10-14 12:31:40 fwinkl Exp $
//////////////////////////////////////////////////////////////////////

class ValgrindAuditor : public Auditor,
                        virtual public IIncidentListener
{
public:  
  ValgrindAuditor(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ValgrindAuditor();
     
  virtual StatusCode initialize();

  /// Incident handler
  virtual void handle( const Incident& incident );


  /// \name before Auditor hooks
  //@{
  virtual void before (StandardEventType evt, const std::string& name);
  
  virtual void before (StandardEventType evt, INamedInterface* alg)
  { if (alg) before(evt, alg->name()); }

  virtual void before (CustomEventTypeRef evt, INamedInterface* alg)
  { if (alg) before(evt, alg->name()); }

  virtual void before (CustomEventTypeRef evt, const std::string& name)
  { do_before(name, evt); }
  //@}

  /// \name after Auditor hooks
  //@{
  virtual void after (StandardEventType evt, const std::string& name, const StatusCode& sc);
  
  virtual void after (StandardEventType evt, INamedInterface* alg, const StatusCode& sc)
  { if (alg) after(evt, alg->name(), sc); }
  
  virtual void after (CustomEventTypeRef evt, INamedInterface* alg, const StatusCode& sc)
  { if (alg) after(evt, alg->name(), sc); }
  
  virtual void after (CustomEventTypeRef evt, const std::string& name, const StatusCode&)
  { do_after(name, evt); }
  
  //@}
  
  /// Start callgrind instrumentation
  virtual void do_beforeExecute(const std::string& name);

  /// Stop callgrind instrumentation
  virtual void do_afterExecute(const std::string& name);

public:
  /// Typedef for algorithm/event pair, e.g. ("MyAlg","initialize")
  typedef std::pair<boost::regex,std::string> NameEvt;


private:
#if GAUDI_MAJOR_VERSION <= 26
  int msgLevel() const { return outputLevel(); }
#endif

  /// Handle to ValgrindSvc
  ServiceHandle<IValgrindSvc> m_valSvc;

  /// List of algorithms to profile
  std::vector<std::string> m_algs;

  /// List of auditor intervals to profile
  std::vector<std::string> m_intervals;
  
  /// Don't profile on the first N events
  unsigned int m_ignoreFirstNEvents;

  /// Dump profile after each interval
  bool m_dumpAfterEachInterval;
  
  /// Message stream 
  MsgStream m_msg;

  /// Internal event counter for BeginEvent incident
  unsigned int m_eventCounter;

  /// Regular expressions for algorithm name matching
  std::vector<boost::regex> m_algsRegEx;

  /// Internal storage of intervals
  std::vector< std::pair<NameEvt,NameEvt> > m_hooks;

  void do_before(const std::string& name, const std::string& hook);
  void do_after(const std::string& name, const std::string& hook);
  StatusCode decodeIntervals();
    
  bool algMatch(const std::string& name);
}; 

#endif 
