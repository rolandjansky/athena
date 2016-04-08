/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2MbtsBits_p1
 *
 * @brief persistent partner for TrigT2MbtsBits
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2MBTSBITS_P1_H
#define TRIGCALOEVENTTPCNV_TRIGT2MBTSBITS_P1_H

#include <vector>
#include <cstdint>

class TrigT2MbtsBits_p1 {
  friend class TrigT2MbtsBitsCnv_p1;

 public:
  TrigT2MbtsBits_p1 () : m_mbtsWord(0) {}
  virtual ~TrigT2MbtsBits_p1 () {}

 private:
  uint32_t m_mbtsWord;
  std::vector<float> m_triggerTimes;
};

#endif
