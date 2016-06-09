/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IOVRANGE_H
#define ATHENAKERNEL_IOVRANGE_H

/**
 *
 *  @file IOVRange.h
 *  @brief Validity Range object. Holds two IOVTime instances (start and stop)
 *
 *  @author Charles Leggett
 *  $Id: IOVRange.h,v 1.4 2008-10-31 22:43:07 leggett Exp $
 *
 *
 *****************************************************************************/

#ifndef ATHENAKERNEL_IOVTIME_H
 #include "AthenaKernel/IOVTime.h"
#endif
#ifndef _CPP_IOSTREAM
 #include <iostream>
#endif
#ifndef _CPP_STRING
 #include <string>
#endif
#ifndef _CPP_SSTREAM
 #include <sstream>
#endif

class MsgStream;
class EventIDRange;

/**
 *  @class IOVRange
 *  @brief Validity Range object. Holds two IOVTimes (start and stop)
 */
class IOVRange {
public:
  IOVRange(): m_start(0),m_stop(0) {};
  IOVRange( const IOVTime& start, const IOVTime& stop );
  IOVRange( const EventIDRange& eir);
  IOVRange( const IOVRange& r ):m_start(r.m_start),m_stop(r.m_stop) {};
  IOVRange& operator= (const IOVRange& r);

  IOVTime start() const { return m_start; }
  IOVTime stop() const { return m_stop; }

  bool isInRange(const IOVTime& t) const {
    return ( t>=m_start && t<m_stop );
  }

  friend bool operator==(const IOVRange& lhs, const IOVRange& rhs);
  friend bool operator!=(const IOVRange& lhs, const IOVRange& rhs);

  friend std::ostream& operator<<(std::ostream& os, const IOVRange& rhs);
  friend MsgStream& operator<<(MsgStream& os, const IOVRange& rhs);

  operator std::string() const;

private:

  IOVTime m_start;
  IOVTime m_stop;

};


inline bool operator==(const IOVRange& lhs, const IOVRange& rhs) {
   return lhs.m_start==rhs.m_start && 
          lhs.m_stop==rhs.m_stop ;
}

inline bool operator!=(const IOVRange& lhs, const IOVRange& rhs) {
  return ! (lhs == rhs);
}


inline IOVRange::operator std::string () const {
  std::ostringstream os;
  os << "{" << m_start << " - " << m_stop << "}";
  return os.str();
}

#endif
