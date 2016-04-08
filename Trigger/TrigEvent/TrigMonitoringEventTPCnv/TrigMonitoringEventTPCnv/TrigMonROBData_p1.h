/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROBDATA_P1_H
#define TRIGMON_ROBDATA_P1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent copy of TrigMonROBData.
*/

#include <stdint.h>

class TrigMonROBData_p1
{
 public:
  
  TrigMonROBData_p1() : m_rob_id(0), m_rob_size(0), m_word(0) {}
  ~TrigMonROBData_p1() {}
  
  friend class TrigMonROBDataCnv_p1;
  
 private:
  
  uint32_t  m_rob_id;
  uint32_t  m_rob_size;
  uint8_t   m_word;
};

#endif
