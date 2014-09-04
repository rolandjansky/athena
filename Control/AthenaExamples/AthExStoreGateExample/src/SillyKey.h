/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_SILLYKEY_H
#define STOREGATE_SILLYKEY_H 1

#include <string>

class SillyKey {
public:
  SillyKey(const std::string aString) : m_string(aString) {}
  inline operator std::string() const {return m_string;}

private:
  std::string m_string;

};

#endif
