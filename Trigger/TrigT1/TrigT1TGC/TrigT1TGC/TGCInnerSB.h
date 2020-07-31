/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCInnerSB_hh
#define TGCInnerSB_hh

#include "TrigT1TGC/TGCSlaveBoard.h"

namespace LVL1TGCTrigger {

const int NumberOfInnerSBData = 8;

class TGCInnerSB: public TGCSlaveBoard {

public:
  TGCInnerSB( const TGCArguments* );
  virtual ~TGCInnerSB(){};

protected:
  virtual void doCoincidence();
  virtual void createSlaveBoardOut();
};


} //end of namespace bracket

#endif
