/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IOVTIME_H
#define ATHENAKERNEL_IOVTIME_H

/**
 *
 *  @file IOVTime.h
 *  @brief Basic time unit for IOVSvc. 
 *  Hold time as a combination of run and event numbers
 *
 *  @author Charles Leggett
 *  $Id: IOVTime.h,v 1.8 2007-06-14 01:57:23 calaf Exp $
 *
 *
 *****************************************************************************/

#include <iosfwd>
#include <string>
#include <stdint.h>

class MsgStream;
class EventIDBase;

/**
 *  @class IOVTime
 *  @brief Basic time unit for IOVSvc. 
 *  Hold time as a combination of run and event numbers
 */
class IOVTime {

private:
  enum IOVTime_type {
    UNDEF = 0,
    TIMESTAMP,
    RUN_EVT,
    BOTH
  };

public:
  static const uint32_t MINRUN;
  static const uint32_t MAXRUN;

  static const uint32_t MINEVENT;
  static const uint32_t MAXEVENT;
  
  static const uint64_t MAXRETIME;
  static const uint64_t UNDEFRETIME;

  static const uint64_t MINTIMESTAMP;
  static const uint64_t MAXTIMESTAMP;
  static const uint64_t UNDEFTIMESTAMP;

public:
  /**
   *  @class SortByTimeStamp
   *  @brief Predicate. Used to sort by time stamp
   */
  class SortByTimeStamp {
  public:
    bool operator() ( const IOVTime& t1, const IOVTime& t2 ) const {
      return t1.timestamp() > t2.timestamp();
    }
    bool operator() ( const IOVTime* t1, const IOVTime* t2 ) const {
      return t1->timestamp() > t2->timestamp();
    }
  };

  /**
   *  @class  SortByRunEvent
   *  @brief Predicate. Used to sort by run and event number
   */
  class SortByRunEvent {
  public:
    bool operator() ( const IOVTime& t1, const IOVTime& t2 ) const {
      return t1.re_time() > t2.re_time();
    }
    bool operator() ( const IOVTime* t1, const IOVTime* t2 ) const {
      return t1->re_time() > t2->re_time();
    }
  };

public:
  IOVTime(): m_status(IOVTime::UNDEF), m_time(UNDEFRETIME), 
    m_timestamp(UNDEFTIMESTAMP){};
  IOVTime( const uint64_t& timestamp ): m_status(IOVTime::TIMESTAMP),
    m_time(IOVTime::UNDEFRETIME), m_timestamp(timestamp){};
  IOVTime( const uint32_t& run, const uint32_t& event );
  IOVTime( const uint32_t& run, const uint32_t& event, 
	   const uint64_t& timestamp );
  IOVTime( const EventIDBase& eid);

  void setTimestamp( const uint64_t& timestamp );
  void setRETime( const uint64_t& time );
  void setRunEvent( const uint32_t& run, const uint32_t& event );
  void reset();

  inline uint32_t run() const { return static_cast<uint32_t> (m_time>>32); }
  inline uint32_t event() const { return static_cast<uint32_t> (m_time & 0xFFFFFFFF); }
  inline uint64_t re_time() const { return m_time; }
  inline uint64_t timestamp() const { return m_timestamp; }

  bool isValid() const;
  inline bool isTimestamp() const { return (m_status == IOVTime::TIMESTAMP ||
				     m_status== IOVTime::BOTH) ? 1 : 0; }
  inline bool isRunEvent() const { return (m_status == IOVTime::RUN_EVT ||
				    m_status == IOVTime::BOTH) ? 1 : 0; }
  inline bool isBoth() const { return (m_status == IOVTime::BOTH) ? 1 : 0; }

  operator std::string() const;
  operator EventIDBase() const;

  friend bool operator<(const IOVTime& lhs, const IOVTime& rhs);
  friend bool operator>(const IOVTime& lhs, const IOVTime& rhs);
  friend bool operator==(const IOVTime& lhs, const IOVTime& rhs);
  friend bool operator!=(const IOVTime& lhs, const IOVTime& rhs);
  friend bool operator>=(const IOVTime& lhs, const IOVTime& rhs);
  friend bool operator<=(const IOVTime& lhs, const IOVTime& rhs);

  friend std::ostream& operator<<(std::ostream& os, const IOVTime& rhs);
  friend MsgStream& operator<<(MsgStream& os, const IOVTime& rhs);

private:

  IOVTime_type m_status;
  uint64_t m_time;
  uint64_t m_timestamp;
};

inline bool operator<(const IOVTime& lhs, const IOVTime& rhs) {
  if (lhs.isTimestamp() && rhs.isTimestamp()) {
    return lhs.m_timestamp < rhs.m_timestamp;
  } else {
    return lhs.m_time < rhs.m_time;
  }
}
inline bool operator>(const IOVTime& lhs, const IOVTime& rhs) {
  if (lhs.isTimestamp() && rhs.isTimestamp()) {
    return lhs.m_timestamp > rhs.m_timestamp;
  } else {
    return lhs.m_time > rhs.m_time;
  }
}
inline bool operator==(const IOVTime& lhs, const IOVTime& rhs) {
  if (lhs.isTimestamp() && rhs.isTimestamp()) {
    return lhs.m_timestamp == rhs.m_timestamp;
  } else {
    return lhs.m_time == rhs.m_time;
  }
}
inline bool operator!=(const IOVTime& lhs, const IOVTime& rhs) {
  return !(lhs == rhs) ;
}
inline bool operator>=(const IOVTime& lhs, const IOVTime& rhs) {
  return !( lhs < rhs );
}
inline bool operator<=(const IOVTime& lhs, const IOVTime& rhs) {
  return !( lhs > rhs );
}

// template < class STR >
// inline STR& operator << (STR& os, const IOVTime& rhs) {
//   os << rhs.m_time << ":  [" << (rhs.m_time>>32) << "," 
//      << ( rhs.m_time & 0xFFFFFFFF ) << "]";
//   return os;
// }

#endif
  
