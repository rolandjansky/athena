/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigSteeringEventTPCnv
 * @Class  : TrigPassBits_p1
 *
 * @brief persistent partner for TrigPassBits
 *
 *
 * File and Version Information:
 * $Id: 
 **********************************************************************************/
#ifndef TRIGSTEERINGEVENTTPCNV_TRIGPASSBITS_P1_H
#define TRIGSTEERINGEVENTTPCNV_TRIGPASSBITS_P1_H
#include <stdint.h>

class TrigPassBits_p1
{
 public:
  
  TrigPassBits_p1() {}
  friend class TrigPassBitsCnv_p1;
  
 private:
  unsigned int m_size = 0;
  std::vector<uint32_t> m_serialized;
};

#endif
