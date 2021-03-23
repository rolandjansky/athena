/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCStripTripletSB_hh
#define TGCStripTripletSB_hh

#include "TrigT1TGC/TGCSlaveBoard.h"

namespace LVL1TGCTrigger {

const int NumberOfStripTripletSBData = 8;

class TGCStripTripletSB: public TGCSlaveBoard
{
 public:
  TGCStripTripletSB();
  virtual ~TGCStripTripletSB(){};

 protected:
  virtual void doCoincidence();
  virtual void createSlaveBoardOut();
};


} //end of namespace bracket

#endif
