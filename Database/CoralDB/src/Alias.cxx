/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Alias.cxx,v 1.3 2008-05-06 22:37:30 beringer Exp $

#include "CoralDB/Alias.h"

#include <iostream>


using namespace std;


namespace CoralDB {
#if 0
} // this is for better indentation in Emacs
#endif


Alias::Alias(const string& alias, const string& convention, const string& id)
  : m_alias(alias)
  , m_convention(convention)
  , m_id(id)
{}


std::ostream& operator<<(std::ostream& os, const Alias& c) {
  return os<<"Alias(alias="<<c.alias()<<", convention="<<c.convention()<<", id="<<c.id()<<")";
}

#if 0
{ // this is to make the number of braces even
#endif
} // namespace CoralDB
