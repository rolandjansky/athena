/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Connection.cxx,v 1.4 2008-05-06 22:37:30 beringer Exp $

#include "CoralDB/Connection.h"

#include <iostream>


using namespace std;


namespace CoralDB {
#if 0
} // this is for better indentation in Emacs
#endif


Connection::Connection()
  : m_fromId("")
  , m_fromSlot("")
  , m_toId("")
  , m_toSlot("")
{}



Connection::Connection(string fromId, string fromSlot, string toId, string toSlot)
  : m_fromId(fromId)
  , m_fromSlot(fromSlot)
  , m_toId(toId)
  , m_toSlot(toSlot)
{}



void Connection::setConnection(string fromId, string fromSlot, string toId, string toSlot) {
  m_fromId = fromId;
  m_fromSlot = fromSlot;
  m_toId = toId;
  m_toSlot = toSlot;
}



void Connection::print(ostream& out) const {
  out << m_fromId <<  " / "
      << m_fromSlot  << "  --->   "
      << m_toId  << " / "
      << m_toSlot
      << endl;
}

bool Connection::operator<(const Connection& b) const {
  return 
    ((m_fromId < b.m_fromId) ||
     ((m_fromId == b.m_fromId) &&
      ((m_fromSlot < b.m_fromSlot) ||
       ((m_fromSlot == b.m_fromSlot) &&
	((m_toId < b.m_toId) ||
	 ((m_toId == b.m_toId) &&
	  (m_toSlot < b.m_toSlot)))))));
}


std::ostream& operator<<(std::ostream& os, const Connection& c) {
  return os<<"Connection(fromID="<<c.fromId()<<", fromSlot="<<c.fromSlot()<<", toID="<<c.toId()<<", toSlot="<<c.toSlot()<<")";
}

#if 0
{ // this is to make the number of braces even
#endif
} // namespace CoralDB
