/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_EVENT_P1_H
#define TRIGMON_EVENT_P1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent copy of TrigMonEvent.
*/

#include <vector>
#include "TrigMonitoringEventTPCnv/TrigMonROB_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonRoi_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonSeq_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonTE_p1.h"

class TrigMonEvent_p1
{
 public:
    
  TrigMonEvent_p1() {}
  ~TrigMonEvent_p1() {}
  
  friend class TrigMonEventCnv_p1;
  
 private:
  
  uint32_t                     m_event;
  uint32_t                     m_lumi;
  uint32_t                     m_bxid;
  uint32_t                     m_run;
  uint32_t                     m_sec;
  uint32_t                     m_nsec;
  std::vector<uint32_t>        m_word;

  std::vector<TrigMonROB_p1>   m_rob;
  std::vector<TrigMonRoi_p1>   m_roi;
  std::vector<TrigMonSeq_p1>   m_seq;
  std::vector<TrigMonTE_p1>    m_te;

  std::vector<uint16_t>        m_l1;
  std::vector<uint32_t>        m_hlt;

  std::vector<uint32_t>        m_var_key;
  std::vector<float>           m_var_val;
};

#endif
