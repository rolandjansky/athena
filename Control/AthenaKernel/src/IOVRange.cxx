/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IOVRANGE_H
 #include "AthenaKernel/IOVRange.h"
#endif
#ifndef GAUDIKERNEL_MSGSTREAM_H
 #include "GaudiKernel/MsgStream.h"
#endif

#include "GaudiKernel/EventIDRange.h"
#include <sstream>
#include <stdexcept>
#include <iostream>

/*****************************************************************************
 *
 *  IOVRange.cxx
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *  $Id: IOVRange.cxx,v 1.5 2008-05-08 15:57:15 leggett Exp $
 *
 *  Validity Range object. Holds two IOVTimes (start and stop)
 *
 *****************************************************************************/

IOVRange::IOVRange( const IOVTime& start, const IOVTime& stop ): 
  m_start(start), m_stop(stop) {
}

IOVRange::IOVRange( const EventIDRange& eir ):
  m_start(eir.start()), m_stop(eir.stop()) {
    static_assert(std::is_trivially_copyable<IOVRange>::value);
    static_assert(std::is_trivially_destructible<IOVRange>::value);
    static_assert(std::is_trivially_copyable<IOVTime>::value);
    static_assert(std::is_trivially_destructible<IOVTime>::value);
}


IOVRange::operator EventIDRange() const {
  return EventIDRange( EventIDBase(m_start), EventIDBase(m_stop) );
}


std::ostream& operator << (std::ostream& os, const IOVRange& rhs) {
  os << (std::string) rhs;
  return os;
}

MsgStream& operator<< (MsgStream &msg, const IOVRange& rhs) {
  msg << (std::string) rhs;
  return msg;
}

IOVRange::operator std::string () const {
  std::ostringstream os;
  os << '{' << m_start << " - " << m_stop << '}';
  return os.str();
}