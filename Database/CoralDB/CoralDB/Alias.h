/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Alias.h,v 1.2 2007-08-27 21:11:56 andr Exp $

#ifndef ALIAS_H
#define ALIAS_H

#include <string>
#include <ostream>

namespace CoralDB {
#if 0
} // this is for better indentation in Emacs
#endif

using namespace std;

class Alias {
  string m_alias;
  string m_convention;
  string m_id;

 public:
  Alias();
  Alias(const string& alias, const string& convention, const string& id);

  string alias() const { return m_alias; }
  string convention() const { return m_convention; }
  string id() const { return m_id; }
};

std::ostream& operator<<(std::ostream&, const Alias& c);

#if 0
{ // this is to make the number of braces even
#endif
} // namespace CoralDB

#endif/*ALIAS_H*/
