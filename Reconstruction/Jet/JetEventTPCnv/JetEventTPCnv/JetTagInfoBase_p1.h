/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETTAGINFOBASE_P1_H
#define JETEVENT_JETTAGINFOBASE_P1_H

#include <string>
#include <vector>

#include "JetEvent/JetTagInfoBase.h"

class JetTagInfoBase_p1
{
  friend class JetTagInfoBaseCnv_p1;
  
 public:
  typedef JetTagInfoBase::storelist_t   storelist_t;
  typedef JetTagInfoBase::catlist_t     catlist_t;
  
  JetTagInfoBase_p1() {};
  ~JetTagInfoBase_p1() {};
  
  // storelist_t   m_keyStore;
  std::vector<unsigned int>  m_keyStoreLength;
  std::vector<std::string>   m_keyStore;
  std::vector<std::string>   m_catStore;
};

#endif

