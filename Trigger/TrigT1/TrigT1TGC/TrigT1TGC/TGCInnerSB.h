/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCInnerSB_h
#define TrigT1TGC_TGCInnerSB_h

#include "TrigT1TGC/TGCSlaveBoard.h"

namespace LVL1TGCTrigger {

class TGCInnerSB: public TGCSlaveBoard
{
 public:
  TGCInnerSB();
  virtual ~TGCInnerSB(){};

 protected:
  virtual void doCoincidence();
  virtual void createSlaveBoardOut();
};


}   // end of namespace

#endif
