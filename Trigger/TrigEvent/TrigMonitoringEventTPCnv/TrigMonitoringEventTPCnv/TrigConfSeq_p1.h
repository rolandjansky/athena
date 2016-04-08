/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_SEQ_P1_H
#define TRIGCONF_SEQ_P1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent partner for TrigConfSeq.
*/

#include <stdint.h>
#include <string>
#include <vector>

#include "TrigMonitoringEventTPCnv/TrigConfAlg_p1.h"

class TrigConfSeq_p1
{
 public:
  
  TrigConfSeq_p1() :
    m_output_te_name(),
    m_output_te_index(0),
    m_output_te_id(0),
    m_topo_te(0),
    m_alg(),
    m_input_te() {}
  ~TrigConfSeq_p1() {}

  friend class TrigConfSeqCnv_p1;
  
 private:
  
  std::string                 m_output_te_name;
  uint16_t                    m_output_te_index;
  uint32_t                    m_output_te_id;
  uint32_t                    m_topo_te;
  std::vector<TrigConfAlg_p1> m_alg;
  std::vector<uint32_t>       m_input_te;
};

#endif
