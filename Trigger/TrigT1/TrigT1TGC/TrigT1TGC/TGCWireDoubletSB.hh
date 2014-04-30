/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCWireDoubletSB_hh
#define TGCWireDoubletSB_hh
#include "TrigT1TGC/TGCDoubletSB.hh"

namespace LVL1TGCTrigger {

class TGCWireDoubletSB : public TGCDoubletSB {
public:
  TGCWireDoubletSB(){
    maxDev=7;
    nChAdj=4;
    iChBase=9;
    posMaxDev=7;
    negMaxDev=-7;
    SType = WIREGROUP;
  };
  ~TGCWireDoubletSB(){};

protected:

};

} //end of namespace bracket

#endif
