/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETTAGINFOBASE_P1_H
#define JETEVENT_JETTAGINFOBASE_P1_H

#include <string>
#include <vector>

class JetTagInfoBase_p1
{
  friend class JetTagInfoBaseCnv_p1;
  
 public:
  JetTagInfoBase_p1() {};
  ~JetTagInfoBase_p1() {};
  
  std::vector<unsigned int>  m_keyStoreLength;
  std::vector<std::string>   m_keyStore;
  std::vector<std::string>   m_catStore;
};

#endif

