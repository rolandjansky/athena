/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_SEQ_P1_H
#define TRIGMON_SEQ_P1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent copy of TrigMonSeq.
*/

#include <stdint.h>
#include <vector>
#include "TrigMonitoringEventTPCnv/TrigMonAlg_p1.h"

class TrigMonSeq_p1
{
 public:
  
  TrigMonSeq_p1() :
    m_encoded(0),
    m_alg(),
    m_var_key(),
    m_var_val() {}
  ~TrigMonSeq_p1() {} 

  friend class TrigMonSeqCnv_p1;
  
 private:
  
  uint32_t                   m_encoded;
  std::vector<TrigMonAlg_p1> m_alg;
  std::vector<uint16_t>      m_var_key;
  std::vector<float>         m_var_val;
};

#endif
