/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROI_P1_H
#define TRIGMON_ROI_P1_H

/*
  @author Rustem Ospanov
  @date July 2009 

  @brief Persistent copy of TrigMonRoi.
*/

#include <stdint.h>
#include <vector>

class TrigMonRoi_p1
{
 public:
  
  TrigMonRoi_p1() {}
  ~TrigMonRoi_p1() {}
  
  friend class TrigMonRoiCnv_p1;
  
 private:
  
  std::vector<uint32_t>   m_word;
  std::vector<uint16_t>   m_var_key;
  std::vector<float>      m_var_val;
};

#endif
