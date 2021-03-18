/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

// This file is really -*- C++ -*-.
#ifndef TRIGT1MUCTPIPHASE1_SIMCONTROLLER_H
#define TRIGT1MUCTPIPHASE1_SIMCONTROLLER_H

#include <vector>
#include <list>
#include <string>

#include "TrigT1MuctpiPhase1/Configuration.h"
#include "TrigT1MuctpiPhase1/L1TopoLUT.h"

namespace LVL1MUONIF {
  class Lvl1MuCTPIInputPhase1;
}

namespace LVL1
{
  class MuCTPIL1Topo;
}

namespace LVL1MUCTPIPHASE1 {

  class MuonSectorProcessor;
  class TriggerProcessor;
  class SimController
  {
    
  public:
    
    SimController();
    ~SimController();

    void configureTopo(const std::string& barrelFileName,
		       const std::string& ecfFileName,
		       const std::string& side0LUTFileName,
		       const std::string& side1LUTFileName);
    void configureOverlapRemoval(const std::string& lutFile);

    bool processData(LVL1MUONIF::Lvl1MuCTPIInputPhase1* input, int bcid=0);
    void setConfiguration( const Configuration& conf );

    LVL1::MuCTPIL1Topo getL1TopoData(int bcid);

    TriggerProcessor* getTriggerProcessor();

  private:

    bool m_doZeroSuppression;
    unsigned int m_threshold1Candidate;
    unsigned int m_threshold2Candidate;
    int m_suppressionMode;
    unsigned int m_maxCandPerPtvalue;
    unsigned int m_maxCandSendToRoib;
    unsigned int m_candBcidOffset;
    std::vector< std::vector< unsigned int > > m_ptSorterBuckets;

    L1TopoLUT m_l1topoLUT;

    TriggerProcessor* m_triggerProcessor;
    std::vector<MuonSectorProcessor*> m_muonSectorProcessors;

  };

}

#endif // TRIGT1MUCTPIPHASE1_SIMCONTROLLER_H
