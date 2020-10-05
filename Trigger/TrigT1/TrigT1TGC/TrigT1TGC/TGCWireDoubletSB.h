/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCWireDoubletSB_hh
#define TGCWireDoubletSB_hh
#include "TrigT1TGC/TGCDoubletSB.h"

namespace LVL1TGCTrigger {

class TGCWireDoubletSB : public TGCDoubletSB {
public:
  TGCWireDoubletSB(const TGCArguments* tgcargs):TGCDoubletSB(tgcargs){
    m_maxDev=7;
    m_nChAdj=4;
    m_iChBase=9;
    m_posMaxDev=7;
    m_negMaxDev=-7;
    m_SType = WIREGROUP;
  };
  ~TGCWireDoubletSB(){};

protected:

};

} //end of namespace bracket

#endif
