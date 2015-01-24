/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCStripDoubletSB_hh
#define TGCStripDoubletSB_hh

#include "TrigT1TGC/TGCDoubletSB.hh"

namespace LVL1TGCTrigger {

class TGCStripDoubletSB : public TGCDoubletSB {
public:
  TGCStripDoubletSB(){
    maxDev=3;
    nChAdj=0;
    iChBase=1;
    posMaxDev=3;
    negMaxDev=-3;

    SType = STRIP;
  };

  ~TGCStripDoubletSB(){};
  
protected:

};

} //end of namespace bracket

#endif
