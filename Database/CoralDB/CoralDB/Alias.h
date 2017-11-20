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


class Alias {
  std::string m_alias;
  std::string m_convention;
  std::string m_id;

 public:
  Alias();
  Alias(const std::string& alias, const std::string& convention, const std::string& id);

  std::string alias() const { return m_alias; }
  std::string convention() const { return m_convention; }
  std::string id() const { return m_id; }
};

std::ostream& operator<<(std::ostream&, const Alias& c);

#if 0
{ // this is to make the number of braces even
#endif
} // namespace CoralDB

#endif/*ALIAS_H*/
