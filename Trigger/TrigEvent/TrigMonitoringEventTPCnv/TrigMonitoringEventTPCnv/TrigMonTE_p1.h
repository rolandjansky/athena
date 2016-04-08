/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_TE_P1_H
#define TRIGMON_TE_P1_H

/**
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent copy of TrigMonTE.
*/

#include <stdint.h>
#include <vector>
#include "TrigMonitoringEvent/TrigMonVar.h"

class TrigMonTE_p1
{
 public:
  
  TrigMonTE_p1() :
    m_id(0),
    m_encoded(0),
    m_child(),
    m_parent(),
    m_roi(),
    m_clid(),
    m_var_key(),
    m_var_val() {}
  ~TrigMonTE_p1() {}
    
  friend class TrigMonTECnv_p1;

 private:
  
  uint32_t                 m_id;
  uint32_t                 m_encoded;
  
  std::vector<uint16_t>    m_child;
  std::vector<uint16_t>    m_parent;
  std::vector<uint8_t>     m_roi;
  std::vector<uint32_t>    m_clid;

  std::vector<uint16_t>    m_var_key;
  std::vector<float>       m_var_val;
};

#endif
