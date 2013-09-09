/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ALG_P1_H
#define TRIGMON_ALG_P1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent version of TrigMonAlg.
*/

#include <stdint.h>
#include <vector>

class TrigMonAlg_p1
{
 public:
  
  TrigMonAlg_p1() {}
  ~TrigMonAlg_p1() {}

  friend class TrigMonAlgCnv_p1;
  
 private:
  
  std::vector<uint8_t>      m_byte;
  std::vector<uint32_t>     m_word;
};

#endif
