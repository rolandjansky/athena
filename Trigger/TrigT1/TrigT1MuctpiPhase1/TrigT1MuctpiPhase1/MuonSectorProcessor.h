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

  class ROIObject
  {
  public:
    float etamin;
    float etamax;
    float phimin;
    float phimax;
    float eta;
    float phi;
    int ieta;
    int iphi;
  };

  class MuonSectorProcessor
  {
    
  public:
    
    MuonSectorProcessor(bool side /*1=A,0=C*/);
    ~MuonSectorProcessor();
    
    void configureTopo(const std::string& xmlName);
    void configureOverlapRemoval(const std::string& lutFile);
    void setInput(LVL1MUONIF::Lvl1MuCTPIInputPhase1* input);
    void runOverlapRemoval();
    void makeTriggerObjectSelections();
    void makeL1TopoData();
    LVL1::MuCTPIL1Topo getL1TopoData(int bcidOffset);
    LVL1MUONIF::Lvl1MuCTPIInputPhase1* getOutput();

  private:
    
    LVL1MUONIF::Lvl1MuCTPIInputPhase1* m_muctpiInput;
    LVL1::MuCTPIL1Topo* m_l1topo;
    std::map<std::string, std::map<unsigned int, ROIObject> > m_roiConfig;
    OverlapHelper* m_overlapHelper;
    bool m_side;
  };
}

#endif // TRIGT1MUCTPIPHASE1_MUONSECTORPROCESSOR_H
