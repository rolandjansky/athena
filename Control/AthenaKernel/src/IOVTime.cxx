/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 *
 *  IOVTime.cxx
 *   IOVSvc
 *
 *  Author: Charles Leggett
 *  $Id: IOVTime.cxx,v 1.10 2005-10-05 13:42:31 schaffer Exp $
 *
 *  Basic time unit for IOVSvc. 
 *  Hold time as a combination of run and event numbers
 *
 *****************************************************************************/

#include "AthenaKernel/IOVTime.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/EventIDBase.h"

//
///////////////////////////////////////////////////////////////////////////
//

IOVTime::IOVTime(uint32_t run, uint32_t event):
  m_status(IOVTime::RUN_EVT),
  m_timestamp(IOVTime::UNDEFTIMESTAMP)
{
  m_time =  ( (uint64_t) run << 32) + event;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IOVTime::IOVTime( uint32_t run,  uint32_t event,
		  uint64_t time):  
  m_status(IOVTime::BOTH), m_timestamp(time)
{
  m_time =  ( (uint64_t) run << 32) + event;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

IOVTime::IOVTime(const EventIDBase& eid) 
{
  m_time = 0;
  m_timestamp = eid.time_stamp()*1000000000LL + eid.time_stamp_ns_offset();

  if (eid.isRunEvent()) {
    m_time = ( (uint64_t) eid.run_number() << 32) + eid.event_number();
    if (eid.isTimeStamp()) {
      m_status = IOVTime::BOTH;
    } else {
      m_status = IOVTime::RUN_EVT;
    }
  } else if (eid.isLumiEvent()) {
    m_time = ( (uint64_t) eid.lumi_block() << 32) + eid.event_number();
    if (eid.isTimeStamp()) {
      m_status = IOVTime::BOTH;
    } else {
      m_status = IOVTime::RUN_EVT;
    }
  } else {
    m_status = IOVTime::TIMESTAMP;
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
IOVTime::setTimestamp( uint64_t timestamp ) noexcept {
  if (isRunEvent()) { 
    m_status = IOVTime::BOTH;
  } else {
    m_status = IOVTime::TIMESTAMP;
  }
  m_timestamp = timestamp;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
IOVTime::setRETime( uint64_t time ) noexcept {
  if (isTimestamp()) { 
    m_status = IOVTime::BOTH;
  } else {
    m_status = IOVTime::RUN_EVT;
  }
  m_time = time;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
IOVTime::setRunEvent( uint32_t run, uint32_t event ) noexcept {
  if (isTimestamp()) { 
    m_status = IOVTime::BOTH;
  } else {
    m_status = IOVTime::RUN_EVT;
  }
  m_time =  ( (uint64_t) run << 32) + event;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
IOVTime::reset() noexcept {
  m_status = IOVTime::UNDEF;
  m_time = IOVTime::UNDEFRETIME;
  m_timestamp = IOVTime::UNDEFTIMESTAMP;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool
IOVTime::isValid() const noexcept {

  // Cannot have BOTH undefined
  if (m_timestamp == IOVTime::UNDEFTIMESTAMP && 
      m_time == IOVTime::UNDEFRETIME) {
      return 0 ;
  }
  
  // Check run/event to be < max
  if (m_timestamp == IOVTime::UNDEFTIMESTAMP) {
      if ( run() > IOVTime::MAXRUN || event() > IOVTime::MAXEVENT ) {
	  return 0;
      }
  }
      
  // Check time < max
  if (m_time == IOVTime::UNDEFRETIME) {
      if ( m_timestamp > IOVTime::MAXTIMESTAMP ) {
	  return 0;
      }
  }

  if (m_timestamp != IOVTime::UNDEFTIMESTAMP &&
      m_time      != IOVTime::UNDEFRETIME) {
      // May have both timestamp and run/event set 
      if ( run() > IOVTime::MAXRUN || event() > IOVTime::MAXEVENT ) {
	  return 0;
      }
      if ( m_timestamp > IOVTime::MAXTIMESTAMP ) {
	  return 0;
      }
  }
  
  return 1;

}

IOVTime::operator std::string () const {
  std::ostringstream os;
  os << "[";
  if (isRunEvent()) {
      os << (m_time>>32) << "," << ( m_time & 0xFFFFFFFF );
  }
  if (isTimestamp()) {
     if (isRunEvent()) os << ":";
     os << m_timestamp;
  }
  os << "]";
  return os.str();
}

IOVTime::operator EventIDBase() const {
  if (isBoth()) {
    return EventIDBase(run(),EventIDBase::UNDEFEVT,
		       std::min(timestamp()/1000000000LL,(unsigned long long)(std::numeric_limits<unsigned int>::max()-1)),timestamp()%1000000000LL,
                       event());
  } else if (isTimestamp()) {
    return EventIDBase(EventIDBase::UNDEFNUM,EventIDBase::UNDEFEVT,
		       std::min(timestamp()/1000000000LL,(unsigned long long)(std::numeric_limits<unsigned int>::max()-1)),timestamp()%1000000000LL);
  } else if (isRunEvent()) {
    return EventIDBase(run(),EventIDBase::UNDEFEVT,
                       EventIDBase::UNDEFNUM,0,
                       event());
  } else {
    return EventIDBase();
  }
}

MsgStream &operator << (MsgStream& os, const IOVTime& rhs) {
  os << (std::string) rhs;
  return os;
}

std::ostream& operator << (std::ostream& os, const IOVTime& rhs) {
  os << (std::string) rhs;
  return os;
}
  
// std::ostrstream& operator << (std::ostrstream& os, const IOVTime& rhs) {
//   os << (int) rhs.m_time << ":  [" << (int) (rhs.m_time>>32) << "," 
//      << (int) ( rhs.m_time & 0xFFFFFFFF ) << "]";
//   return os;
// }
