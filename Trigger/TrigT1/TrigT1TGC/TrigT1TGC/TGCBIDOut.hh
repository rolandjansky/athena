/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCBIDOut_hh
#define TGCBIDOut_hh

#include "TrigT1TGC/TGCASDOut.h"

namespace LVL1TGCTrigger {

class TGCBIDOut : public TGCASDOut {

public:
  TGCBIDOut();
  TGCBIDOut(const TGCASDOut* ASDOut);

  virtual ~TGCBIDOut() = default;

  void setBid(int bidIn){ bid=bidIn;};
  int getBid() const { return bid;};

private:
  int bid;
};


} //end of namespace bracket

#endif
