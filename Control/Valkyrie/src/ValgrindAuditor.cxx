/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ValgrindAuditor.cxx,v 1.4 2008-10-14 12:31:40 fwinkl Exp $

// Package includes
#include "ValgrindAuditor.h"

// Framework includes
#include "GaudiKernel/IIncidentSvc.h"
#include "AthenaKernel/IValgrindSvc.h"
#include "AthenaBaseComps/AthCheckMacros.h"

// STL includes
#include <algorithm>
#include <iterator>
#include <limits>

// Boost includes
#include <boost/algorithm/string.hpp>

using std::string;

// Constructor
ValgrindAuditor::ValgrindAuditor(const std::string& name,
                                 ISvcLocator* pSvcLocator)
  : Auditor(name, pSvcLocator),
    m_valSvc("ValgrindSvc", this->name()),
    m_eventCounter(0)
{
  declareProperty("ProfiledAlgs", m_algs,
                  "List of algorithms to run in valgrind");

  declareProperty("ProfiledIntervals", m_intervals,
                  "Intervals to profile (e.g. 'MyAlg.initialize:MyAlg.finalize'" );

  declareProperty("IgnoreFirstNEvents", m_ignoreFirstNEvents = 0,
                  "Do not profile the first N events");

  declareProperty("DumpAfterEachInterval", m_dumpAfterEachInterval = true,
                  "Dump separate profile after each interval in ProfiledIntervals" );
}

// Destructor
ValgrindAuditor::~ValgrindAuditor()
{
}


StatusCode ValgrindAuditor::initialize()
{
  if ( !m_valSvc.retrieve().isSuccess())  {
    msgStream() << MSG::ERROR << "Could not retrieve the ValgrindSvc" << endmsg;
    return StatusCode::FAILURE;
  }

  const IProperty* valSvcProp = dynamic_cast<const IProperty*>(&(*m_valSvc));
  if ( !valSvcProp ) {
    msgStream() << MSG::ERROR
                << "Could not retrieve IProperty interface to ValgrindSvc."
                << endmsg;
    return StatusCode::FAILURE;
  }

  // We inherit the OutputLevel from ValgrindSvc
  ATH_CHECK(setProperty(valSvcProp->getProperty("OutputLevel")));

  msgStream() << MSG::VERBOSE
              << "Initializing " << name() << "..." 
              << endmsg;

  // Copy some properties from ValgrindSvc
  std::string properties[] = {"ProfiledAlgs",
                              "ProfiledIntervals",
                              "IgnoreFirstNEvents",
                              "DumpAfterEachInterval"};

  for( std::string prop : properties ) {
    if ( !setProperty(valSvcProp->getProperty(prop)) ) {
      msgStream() << MSG::ERROR << "Cannot set " << prop << " property." << endmsg;
      return StatusCode::FAILURE;
    }     
  }

  // Reset internal event counter
  m_eventCounter = 0;

  // Create regular expressions from algorithm names
  for( const std::string& re : m_algs ) {
    try {
      m_algsRegEx.push_back( boost::regex(re) );
    }
    catch ( const boost::regex_error& ) {
      msgStream() << MSG::ERROR << "Ignoring invalid regular expression: " << re << endmsg;
    }
  }
    
  if ( msgLevel() <= MSG::INFO ) {
    std::ostringstream out;
    out << "[ ";
    std::copy( m_algs.begin(), m_algs.end(),
               std::ostream_iterator<std::string>( out, " " ) );
    out << "]";
    msgStream() << MSG::INFO << "Profiled algorithms: " << out.str() << endmsg;

    out.str("");
    out << "[ ";
    std::copy( m_intervals.begin(), m_intervals.end(),
               std::ostream_iterator<std::string>( out, " " ) );
    out << "]";
    msgStream() << MSG::INFO << "Profiled intervals: " << out.str() << endmsg;
  }

  if (!m_intervals.empty()) {
    if ( decodeIntervals().isFailure() ) {
      msgStream() << MSG::ERROR << "Syntax error in ProfiledIntervals" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Register incidents
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  if ( !incSvc.retrieve().isSuccess() ) {
    msgStream() << MSG::ERROR << "Unable to get the IncidentSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  // Try to be the first incident handler to be called
  const long prio = std::numeric_limits<long>::max();
  
  incSvc->addListener( this, IncidentType::BeginEvent, prio );

  for( const std::pair<NameEvt,NameEvt>& h : m_hooks ) {
    // No regular expressions allowed for incidents. Take the original string.
    if ( h.first.second=="incident" ) incSvc->addListener( this, h.first.first.str(), prio );
    if ( h.second.second=="incident" ) incSvc->addListener( this, h.second.first.str(), prio );      
  }
  
  return StatusCode::SUCCESS;
}


/********************************************************************************
 * Incident handler
 */
void ValgrindAuditor::handle( const Incident& inc )
{
  // Internal event counter
  if ( inc.type() == IncidentType::BeginEvent ) {
    m_eventCounter++;
  }

  // Check if the incident appears at beginning or end of interval
  std::vector< std::pair<NameEvt,NameEvt> >::const_iterator h;
  for (h=m_hooks.begin(); h!=m_hooks.end(); ++h) {
    if ( h->first.second=="incident" ) do_before(inc.type(), "incident");
    if ( h->second.second=="incident" ) do_after(inc.type(), "incident");
  }
}


/********************************************************************************
 * Gaudi auditor hooks
 */
void ValgrindAuditor::before (StandardEventType evt, const std::string& name)
{
  if ( evt == IAuditor::Execute ) do_beforeExecute(name);
  else {
    std::ostringstream os;
    os << evt;
    do_before(name, boost::to_lower_copy(os.str()));
  }
}

void ValgrindAuditor::after (StandardEventType evt, const std::string& name, const StatusCode&)
{
  if ( evt == IAuditor::Execute ) do_afterExecute(name);
  else {
    std::ostringstream os;
    os << evt;
    do_after(name, boost::to_lower_copy(os.str()));
  }
}


/********************************************************************************
 * Enable callgrind instrumentation before algorithm execution
 */
void ValgrindAuditor::do_beforeExecute(const std::string& name)
{
  if ( algMatch(name) ) {
    if (m_eventCounter > m_ignoreFirstNEvents) {
      if ( msgLevel() <= MSG::DEBUG )
        msgStream() << MSG::DEBUG << "Starting callgrind before execute of " << name
                    << " [event #" << m_eventCounter << "]" << endmsg;
      
      m_valSvc->callgrindStartInstrumentation();
    }
  }
}


/********************************************************************************
 * Disable callgrind instrumentation after algorithm execution
 */
void ValgrindAuditor::do_afterExecute(const std::string& name)
{
  if ( algMatch(name) ) {
    if (m_eventCounter > m_ignoreFirstNEvents) {
      m_valSvc->callgrindStopInstrumentation();      
      if ( msgLevel() <= MSG::DEBUG )
        msgStream() << MSG::DEBUG << "Stopping callgrind after execute of " << name
                    << " [event #" << m_eventCounter << "]" << endmsg;
    }
  }
}


/********************************************************************************
 * Enable callgrind instrumentation
 */
void ValgrindAuditor::do_before(const std::string& name, const std::string& hook)
{
  std::vector< std::pair<NameEvt,NameEvt> >::const_iterator iter;

  for (iter=m_hooks.begin(); iter!=m_hooks.end(); ++iter) {
    if ( boost::regex_match(name, iter->first.first) &&
         iter->first.second == hook ) {
      m_valSvc->callgrindStartInstrumentation();
      if ( msgLevel() <= MSG::DEBUG )
        msgStream() << MSG::DEBUG << "Starting callgrind before " << hook
                    << " of " << name << endmsg;
    }
  }
}

/********************************************************************************
 * Disable callgrind instrumentation
 */
void ValgrindAuditor::do_after(const std::string& name, const std::string& hook)
{
  std::vector< std::pair<NameEvt,NameEvt> >::const_iterator iter;

  for (iter=m_hooks.begin(); iter!=m_hooks.end(); ++iter) {
    if ( boost::regex_match(name, iter->second.first) &&
         iter->second.second == hook ) {
      m_valSvc->callgrindStopInstrumentation();
      if ( msgLevel() <= MSG::DEBUG )
        msgStream() << MSG::DEBUG << "Stopping callgrind after " << hook
                    << " of " << name << endmsg;

      if ( m_dumpAfterEachInterval ) {
        m_valSvc->callgrindDumpStats(msgStream().stream());
        msgStream() << MSG::INFO << "Creating callgrind profile #" << m_valSvc->profileCount()
                    << " after " << hook << " of " << name << endmsg;
      }
    }
  }
}


/********************************************************************************
 * Decodes the intervals
 */

namespace {
  // Helper to decode name/event pair from string (e.g. MyAlg.initialize)
  StatusCode decodeNameEvt(const std::string& s, ValgrindAuditor::NameEvt& nameEvt)
  {
    // Find last(!) "." delimiter (earlier ones might be part of regexp)
    string::size_type loc = s.rfind(".");
    if ( loc==string::npos ) return StatusCode::FAILURE;

    try {
      nameEvt.first = boost::regex(s.substr(0,loc));
    }
    catch ( const boost::regex_error& ) {
      return StatusCode::FAILURE;
    }
    
    nameEvt.second = s.substr(loc+1);
    
    return StatusCode::SUCCESS;
  }
}

StatusCode ValgrindAuditor::decodeIntervals()
{  
  m_hooks.clear();
  m_hooks.reserve(m_intervals.size());
  
  std::vector<string>::const_iterator iter = m_intervals.begin();
  for (; iter!=m_intervals.end(); ++iter) {
    const string& spec = *iter;
    string::size_type loc = spec.find(":");

    // If there is no delimiter interpret as [a,a]
    string s1, s2;
    if (loc==string::npos) s1 = s2 = spec;
    else {  // Extract interval
      s1 = spec.substr(0,loc);
      s2 = spec.substr(loc+1);
    }

    NameEvt ne1, ne2;
    if ( s1=="" || s2=="" ||
         decodeNameEvt(s1,ne1).isFailure() || decodeNameEvt(s2,ne2).isFailure() ) {
      msgStream() << MSG::ERROR << "Invalid profiling interval [" << spec << "]" << endmsg;
      return StatusCode::FAILURE;
    }

    std::pair<NameEvt,NameEvt> p(ne1,ne2);
    m_hooks.push_back(p);    
  }

  return StatusCode::SUCCESS;
}

/********************************************************************************
 * Match the algorithm name against all regular expressions
 */
bool ValgrindAuditor::algMatch(const std::string& name)
{
  std::vector<boost::regex>::const_iterator iter;
  for ( iter=m_algsRegEx.begin(); iter!=m_algsRegEx.end(); ++iter ) {
    if ( boost::regex_match(name,*iter) ) return true;
  }
  return false;
}
    
