/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCWireTripletSB_hh
#define TGCWireTripletSB_hh

#include "TrigT1TGC/TGCSlaveBoard.hh"

namespace LVL1TGCTrigger {

const int LengthOfWTSBCoincidenceOut = 100;
const int NChAdjInWTSB = 2;
const int NumberOfWireTripletSBData = 3;

class TGCWireTripletSB: public TGCSlaveBoard {

public:
  TGCWireTripletSB(const TGCWireTripletSB& right);
  TGCWireTripletSB& operator=(const TGCWireTripletSB& right);

  TGCWireTripletSB();
  virtual ~TGCWireTripletSB(){};

protected:
  virtual void doCoincidence();
  virtual void createSlaveBoardOut();

};


} //end of namespace bracket

#endif
