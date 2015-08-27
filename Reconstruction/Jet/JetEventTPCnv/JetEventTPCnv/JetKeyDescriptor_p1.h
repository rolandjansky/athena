/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETKEYDESCRIPTOR_P1_H
#define JETEVENT_JETKEYDESCRIPTOR_P1_H

#include <string>
#include <vector>

#include "JetEvent/JetKeyDescriptor.h"

class JetKeyDescriptor_p1
{
  friend class JetKeyDescriptorCnv_p1;
  
 public:
  typedef JetKeyDescriptor::storelist_t   storelist_t;
  typedef JetKeyDescriptor::catlist_t     catlist_t;
  
  JetKeyDescriptor_p1() {};
  ~JetKeyDescriptor_p1() {};
  
  // storelist_t   m_keyStore;
  std::vector<unsigned int>  m_keyStoreLength;
  std::vector<std::string>   m_keyStore;
  std::vector<std::string>   m_catStore;
};

#endif

