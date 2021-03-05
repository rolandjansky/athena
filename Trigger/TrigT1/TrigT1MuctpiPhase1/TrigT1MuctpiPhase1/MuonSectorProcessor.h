// This file is really -*- C++ -*-.

/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

#ifndef TRIGT1MUCTPIPHASE1_MUONSECTORPROCESSOR_H
#define TRIGT1MUCTPIPHASE1_MUONSECTORPROCESSOR_H


#include <map>
#include <string>

namespace LVL1MUONIF {
  class Lvl1MuCTPIInputPhase1;
}

namespace LVL1 {
  class MuCTPIL1Topo;
}

namespace LVL1MUCTPIPHASE1 {
  class OverlapHelper;
  class L1TopoLUT;

  class MuonSectorProcessor
  {
    
  public:
    
    MuonSectorProcessor(bool side /*1=A,0=C*/);
    ~MuonSectorProcessor();
    
    void setL1TopoLUT(const L1TopoLUT* l1topoLUT) {m_l1topoLUT=l1topoLUT;}
    void configureOverlapRemoval(const std::string& lutFile);
    void setInput(LVL1MUONIF::Lvl1MuCTPIInputPhase1* input);
    void runOverlapRemoval(int bcid);
    void makeL1TopoData(int bcid);
    LVL1::MuCTPIL1Topo* getL1TopoData(int bcid);
    LVL1MUONIF::Lvl1MuCTPIInputPhase1* getOutput();

  private:
    
    LVL1MUONIF::Lvl1MuCTPIInputPhase1* m_muctpiInput;
    std::map<int, LVL1::MuCTPIL1Topo*> m_bcid_to_l1topo;
    OverlapHelper* m_overlapHelper;
    const L1TopoLUT* m_l1topoLUT;
    bool m_side;
  };
}

#endif // TRIGT1MUCTPIPHASE1_MUONSECTORPROCESSOR_H
