/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_SIG_P1_H
#define TRIGCONF_SIG_P1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent partner for TrigConfSig;
*/

#include <stdint.h>
#include <string>
#include <vector>

class TrigConfSig_p1
{
 public:
  
  TrigConfSig_p1() :
    m_counter(0),
    m_logic(0),
    m_label(),
    m_output_te() {}
  ~TrigConfSig_p1() {}
  
  friend class TrigConfSigCnv_p1;
  
 private:
  
  uint32_t                 m_counter;
  int                      m_logic;
  std::string              m_label;
  std::vector<uint32_t>    m_output_te;
};

#endif
