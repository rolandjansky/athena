// This file is really -*- C++ -*-.

/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

#ifndef TRIGT1MUCTPIPHASE1_MUONSECTORPROCESSOR_H
#define TRIGT1MUCTPIPHASE1_MUONSECTORPROCESSOR_H


#include <string>
#include <map>
#include <unordered_map>
#include <vector>

namespace LVL1MUONIF {
  class Lvl1MuCTPIInputPhase1;
}

namespace LVL1 {
  class MuCTPIL1Topo;
}

namespace TrigConf {
  class L1Menu;
}

namespace LVL1MUCTPIPHASE1 {
  class OverlapHelper;
  class L1TopoLUT;

  class MuonSectorProcessor
  {
    
  public:
    
    MuonSectorProcessor(bool side /*1=A,0=C*/);
    ~MuonSectorProcessor();

    void setMenu(const TrigConf::L1Menu* l1menu);
    void setL1TopoLUT(const L1TopoLUT* l1topoLUT) {m_l1topoLUT=l1topoLUT;}
    void configureOverlapRemoval(const std::string& lutFile);
    bool configurePtEncoding();
    void setInput(LVL1MUONIF::Lvl1MuCTPIInputPhase1* input);
    void runOverlapRemoval(int bcid);
    std::string makeL1TopoData(int bcid);
    LVL1::MuCTPIL1Topo* getL1TopoData(int bcid);
    LVL1MUONIF::Lvl1MuCTPIInputPhase1* getOutput();

  private:
    
    LVL1MUONIF::Lvl1MuCTPIInputPhase1* m_muctpiInput;
    std::unordered_map<int, LVL1::MuCTPIL1Topo*> m_bcid_to_l1topo;
    OverlapHelper* m_overlapHelper;
    const TrigConf::L1Menu* m_l1menu;
    const L1TopoLUT* m_l1topoLUT;
    bool m_side;

    //encoding between threshold index and pt value in barrel, endcap, and forward regions
    std::vector<std::map<int, int> > m_ptEncoding;
  };
}

#endif // TRIGT1MUCTPIPHASE1_MUONSECTORPROCESSOR_H
