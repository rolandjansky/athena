/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaKernel/IOVTime.h"
#include <string>
#include <iosfwd>

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
  IOVRange( const IOVRange& r ) = default;
  IOVRange& operator= (const IOVRange& r) = default;

  const IOVTime& start() const { return m_start; }
  const IOVTime& stop() const { return m_stop; }

  bool isInRange(const IOVTime& t) const {
    return ( t>=m_start && t<m_stop );
  }

  friend bool operator==(const IOVRange& lhs, const IOVRange& rhs);
  friend bool operator!=(const IOVRange& lhs, const IOVRange& rhs);

  friend std::ostream& operator<<(std::ostream& os, const IOVRange& rhs);
  friend MsgStream& operator<<(MsgStream& os, const IOVRange& rhs);

  operator std::string() const;
  operator EventIDRange() const;

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


#endif
