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


class Connection {
 public:
  Connection();
  Connection(std::string fromId, std::string fromSlot, std::string toId, std::string toSlot);

  std::string fromId() const { return m_fromId; }
  std::string fromSlot() const { return m_fromSlot; }
  std::string toId() const { return m_toId; }
  std::string toSlot() const { return m_toSlot; }

  void setConnection(std::string fromId, std::string fromSlot, std::string toId, std::string toSlot);

  void print(std::ostream& out) const;

  bool operator<(const Connection& b) const;

 private:
  std::string m_fromId;
  std::string m_fromSlot;
  std::string m_toId;
  std::string m_toSlot;
};

std::ostream& operator<<(std::ostream&, const Connection& c);

#if 0
{ // this is to make the number of braces even
#endif
} // namespace CoralDB

#endif/*CONNECTION_H*/
