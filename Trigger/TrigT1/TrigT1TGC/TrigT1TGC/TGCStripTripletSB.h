/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCStripTripletSB_h
#define TrigT1TGC_TGCStripTripletSB_h

#include "TrigT1TGC/TGCSlaveBoard.h"

namespace LVL1TGCTrigger {

class TGCStripTripletSB: public TGCSlaveBoard
{
 private:
  static constexpr int s_NumberOfStripTripletSBData = 8;

 public:
  TGCStripTripletSB();
  virtual ~TGCStripTripletSB(){};

 protected:
  virtual void doCoincidence();
  virtual void createSlaveBoardOut();
};


} //end of namespace bracket

#endif
