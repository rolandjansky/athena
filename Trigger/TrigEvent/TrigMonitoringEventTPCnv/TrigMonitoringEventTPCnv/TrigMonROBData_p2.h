/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROBDATA_P2_H
#define TRIGMON_ROBDATA_P2_H

/*
  @author Rustem Ospanov
  @date April 2010

  @brief Persistent copy of TrigMonROBData.
*/

#include <stdint.h>

class TrigMonROBData_p2
{
 public:
  
  TrigMonROBData_p2() : m_rob_id(0), m_word(0) {}
  ~TrigMonROBData_p2() {}
  
  friend class TrigMonROBDataCnv_p2;
  
 private:
  
  uint32_t  m_rob_id;
  uint32_t  m_word;
};

#endif
