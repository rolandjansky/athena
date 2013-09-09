/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_CHAIN_P1_H
#define TRIGCONF_CHAIN_P1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent partner for TrigConfChain.
*/

#include <stdint.h>
#include <string>
#include <vector>

#include "TrigMonitoringEventTPCnv/TrigConfSig_p1.h"

class TrigConfChain_p1
{
 public:
  
  TrigConfChain_p1() {}
  ~TrigConfChain_p1() {}
  
  friend class TrigConfChainCnv_p1;
  
 private:
  
  std::string                   m_chain_name;
  std::string                   m_lower_name;
  uint32_t                      m_chain_id;
  uint32_t                      m_lower_id;
  uint16_t                      m_chain_counter;
  uint16_t                      m_lower_counter;
  uint8_t                       m_level;
  float                         m_prescale;
  float                         m_pass_through;

  std::vector<uint32_t>         m_lower_ids;
  std::vector<float>            m_stream_prescale;
  std::vector<TrigConfSig_p1>   m_signature;
  std::vector<std::string>      m_stream_name;
  std::vector<std::string>      m_group;
};

#endif
