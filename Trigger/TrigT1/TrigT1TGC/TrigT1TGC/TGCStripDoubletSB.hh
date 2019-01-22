/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCStripDoubletSB_hh
#define TGCStripDoubletSB_hh

#include "TrigT1TGC/TGCDoubletSB.hh"

namespace LVL1TGCTrigger {

class TGCStripDoubletSB : public TGCDoubletSB {
public:
  TGCStripDoubletSB(){
    m_maxDev=3;
    m_nChAdj=0;
    m_iChBase=1;
    m_posMaxDev=3;
    m_negMaxDev=-3;

    m_SType = STRIP;
  };

  ~TGCStripDoubletSB(){};
  
protected:

};

} //end of namespace bracket

#endif
