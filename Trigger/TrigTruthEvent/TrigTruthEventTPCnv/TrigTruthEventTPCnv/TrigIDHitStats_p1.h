/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigTruthEventTPCnv
 * @class  : TrigIDHitStats_p1
 *
 * @brief persistent partner for TrigIDHitStats
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigIDHitStats_p1.h,v 1.1 2008-04-04 14:46:41 ahamil Exp $
 **********************************************************************************/
#ifndef TRIGEVENTTPCNV_TRIGIDHITSTATS_P1_H
#define TRIGEVENTTPCNV_TRIGIDHITSTATS_P1_H

#include <cstring>

#include "TrigInDetTruthEvent/TrigIDHitStats.h"

class TrigIDHitStats_p1 
{
  friend class TrigIDHitStatsCnv_p1;

 public:
  
  TrigIDHitStats_p1() {}
  virtual ~TrigIDHitStats_p1() = default;


 private:
  
  //the 3 comes from NUM_SUBDETECTORS enum in TrigIDHitStats
  unsigned char numHits[3];

};

#endif
