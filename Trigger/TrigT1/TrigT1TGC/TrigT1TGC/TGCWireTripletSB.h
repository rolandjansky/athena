/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1TGC_TGCWireTripletSB_hh
#define TrigT1TGC_TGCWireTripletSB_hh

#include "TrigT1TGC/TGCSlaveBoard.h"

namespace LVL1TGCTrigger {

class TGCWireTripletSB: public TGCSlaveBoard 
{
 private:
  static constexpr int s_LengthOfWTSBCoincidenceOut = 100;
  static constexpr int s_NChAdjInWTSB = 2;
  static constexpr int s_NumberOfWireTripletSBData = 3;

 public:
  TGCWireTripletSB(const TGCWireTripletSB& right) = delete;
  TGCWireTripletSB& operator = (const TGCWireTripletSB& right);

  TGCWireTripletSB();
  virtual ~TGCWireTripletSB(){};

 protected:
  virtual void doCoincidence();
  virtual void createSlaveBoardOut();

};


} //end of namespace bracket

#endif
