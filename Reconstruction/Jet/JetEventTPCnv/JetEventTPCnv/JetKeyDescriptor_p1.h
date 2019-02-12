/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETKEYDESCRIPTOR_P1_H
#define JETEVENT_JETKEYDESCRIPTOR_P1_H

#include <string>
#include <vector>

class JetKeyDescriptor_p1
{
  friend class JetKeyDescriptorCnv_p1;
  
 public:
  JetKeyDescriptor_p1() {};
  ~JetKeyDescriptor_p1() = default;
  
  std::vector<unsigned int>  m_keyStoreLength;
  std::vector<std::string>   m_keyStore;
  std::vector<std::string>   m_catStore;
};

#endif

