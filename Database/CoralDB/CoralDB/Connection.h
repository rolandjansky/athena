/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Connection.h,v 1.3 2007-08-27 23:19:32 andr Exp $

#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <ostream>

namespace CoralDB {
#if 0
} // this is for better indentation in Emacs
#endif

using namespace std;

class Connection {
 public:
  Connection();
  Connection(string fromId, string fromSlot, string toId, string toSlot);

  string fromId() const { return m_fromId; }
  string fromSlot() const { return m_fromSlot; }
  string toId() const { return m_toId; }
  string toSlot() const { return m_toSlot; }

  void setConnection(string fromId, string fromSlot, string toId, string toSlot);

  void print(ostream& out) const;

  bool operator<(const Connection& b) const;

 private:
  string m_fromId;
  string m_fromSlot;
  string m_toId;
  string m_toSlot;
};

std::ostream& operator<<(std::ostream&, const Connection& c);

#if 0
{ // this is to make the number of braces even
#endif
} // namespace CoralDB

#endif/*CONNECTION_H*/
