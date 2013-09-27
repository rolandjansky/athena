/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigTrtHitCounts_p2
 *
 * @brief persistent partner for TrigTrtHitCounts
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTS_P2_H
#define TRIGINDETEVENTTPCNV_TRIGTRTHITCOUNTS_P2_H

#include "TrigInDetEventTPCnv/TrigHisto1D_p1.h"

class TrigTrtHitCounts_p2 {
  friend class TrigTrtHitCountsCnv_p2;

 public:
  TrigTrtHitCounts_p2 () {}
  virtual ~TrigTrtHitCounts_p2 () {}

 private:
  TrigHisto1D_p1 m_endcapC;
  TrigHisto1D_p1 m_barrel;
  TrigHisto1D_p1 m_endcapA;
};

#endif
