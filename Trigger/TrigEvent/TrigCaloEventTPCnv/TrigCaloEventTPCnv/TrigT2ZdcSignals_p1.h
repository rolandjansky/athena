/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2ZdcSignals_p1
 *
 * @brief persistent partner for TrigT2ZdcSignals
 *
 * @author Denis Oliveira Damazio <Denis.Oliveira.Damazio@cern.ch>
 *
 * File and Version Information:
 * $Id: TrigT2ZdcSignals_p1.h 441930 2011-10-17 22:04:50Z damazio $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2ZDCSIGNALS_P1_H
#define TRIGCALOEVENTTPCNV_TRIGT2ZDCSIGNALS_P1_H

#include <vector>

class TrigT2ZdcSignals_p1
{
 public:
  
  TrigT2ZdcSignals_p1() {}
  friend class TrigT2ZdcSignalsCnv_p1;
  
 private:

  std::vector<float> m_triggerEnergies;
  std::vector<float> m_triggerTimes;

};

#endif
