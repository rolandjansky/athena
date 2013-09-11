/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1CTP/EventInfo.h"

#include <sstream>
#include <iomanip>

namespace LVL1CTP {

  std::auto_ptr<EventInfo> EventInfo::m_instance;

  EventInfo::EventInfo()
    : m_runNumber( 0 ), m_eventNumber ( 0 ),
      m_eventType( 0 ), m_triggerType( 0 ), m_l1id( 0 ), m_bcid( 0 ), m_utcTime( 0 ), m_timeNsOffset( 0 ) 
  {
  }

  EventInfo& EventInfo::instance() 
  {
    if (m_instance.get() == 0) m_instance.reset(new EventInfo());
    return *m_instance;
  }

  void EventInfo::clear() 
  {
    m_runNumber    = 0;
    m_eventNumber  = 0;
    m_eventType    = 0;
    m_triggerType  = 0;
    m_l1id         = 0;
    m_bcid         = 0;
    m_utcTime      = 0;
    m_timeNsOffset = 0;
  }

  const std::string EventInfo::dump() const
  {
    std::ostringstream s;
    
    s << " time: " << m_utcTime << " " << m_timeNsOffset
      << " Run: " << m_runNumber
      << " Event: " << m_eventNumber
      << " L1ID: " << m_l1id
      << " BCID: " << m_bcid
      << " triggerType: " << m_triggerType
      << " eventType: " << m_eventType;
    
    return s.str();
  }

  unsigned int EventInfo::runNumber() const { return m_runNumber; }
  void EventInfo::setRunNumber( unsigned int value ) { m_runNumber = value; }

  unsigned int EventInfo::eventNumber() const { return m_eventNumber; }
  void EventInfo::setEventNumber( unsigned int value ) { m_eventNumber = value; }

  unsigned int EventInfo::eventType() const { return m_eventType; }
  void EventInfo::setEventType( unsigned int type ) { m_eventType = type; }

  unsigned int EventInfo::triggerType() const { return m_triggerType; }
  void EventInfo::setTriggerType( unsigned int type ) { m_triggerType = type; }

  unsigned int EventInfo::extendedL1ID() const { return m_l1id; }
  void EventInfo::setExtendedL1ID( unsigned int l1id ) { m_l1id = l1id; }

  unsigned int EventInfo::bcid() const { return m_bcid; }
  void EventInfo::setBCID( unsigned int bcid ) { m_bcid = bcid; }

  unsigned int EventInfo::time() const { return m_utcTime; }
  void EventInfo::setTime( unsigned int time ) { m_utcTime = time; }

  unsigned int EventInfo::timeNs() const { return m_timeNsOffset; }
  void EventInfo::setTimeNs( unsigned int time ) { m_timeNsOffset = time; }

} // namespace LVL1CTP
