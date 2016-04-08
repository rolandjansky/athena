/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2MbtsBits_p3
 *
 * @brief persistent partner for TrigT2MbtsBits
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2MBTSBITS_P3_H
#define TRIGCALOEVENTTPCNV_TRIGT2MBTSBITS_P3_H

#include <vector>

class TrigT2MbtsBits_p3 {
  friend class TrigT2MbtsBitsCnv_p3;

 public:
  TrigT2MbtsBits_p3 () {}
  virtual ~TrigT2MbtsBits_p3 () {}

 private:
  std::vector<float> m_triggerEnergies;
  std::vector<float> m_triggerTimes;
};

#endif
