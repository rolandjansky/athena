/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROB_P1_H
#define TRIGMON_ROB_P1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent copy of TrigMonROB.
*/

#include <stdint.h>
#include <vector>

#include "TrigMonitoringEventTPCnv/TrigMonROBData_p1.h"

class TrigMonROB_p1
{
 public:
  
  TrigMonROB_p1() {}
  ~TrigMonROB_p1() {}
  
  friend class TrigMonROBCnv_p1;

 private:
  
  std::vector<uint32_t>          m_word;
  std::vector<TrigMonROBData_p1> m_data;  
};

#endif
