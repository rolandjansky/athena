/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_CONFIG_P1_H
#define TRIGMON_CONFIG_P1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent copy of TrigMonConfig;
*/

#include <stdint.h>
#include <string>
#include <vector>

#include "TrigMonitoringEventTPCnv/TrigConfChain_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfSeq_p1.h"

class TrigMonConfig_p1
{
 public:
  
  TrigMonConfig_p1() :
    m_event(0),
    m_lumi(0),
    m_run(0),
    m_sec(0),
    m_nsec(0),
    m_master_key(0),
    m_hlt_ps_key(0),
    m_lv1_ps_key(0),
    m_chain(),
    m_seq(),
    m_var_name(),
    m_var_id(),
    m_pair_key(),
    m_pair_val() {}
  ~TrigMonConfig_p1() {}

  friend class TrigMonConfigCnv_p1;
  
 private:
  
  uint32_t                      m_event;
  uint32_t                      m_lumi;
  uint32_t                      m_run;
  uint32_t                      m_sec;
  uint32_t                      m_nsec;

  uint32_t                      m_master_key;
  uint32_t                      m_hlt_ps_key;
  uint32_t                      m_lv1_ps_key;
  
  std::vector<TrigConfChain_p1> m_chain;
  std::vector<TrigConfSeq_p1>   m_seq;

  std::vector<std::string>      m_var_name;
  std::vector<uint32_t>         m_var_id; 
  
  std::vector<std::string>      m_pair_key;
  std::vector<std::string>      m_pair_val;
};

#endif
