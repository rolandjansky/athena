/*                                                                                                                      
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// This file is really -*- C++ -*-.
#ifndef TRIGT1MUCTPIPHASE1_SIMCONTROLLER_H
#define TRIGT1MUCTPIPHASE1_SIMCONTROLLER_H

#include <vector>
#include <list>
#include <string>

#include "Configuration.h"
#include "L1TopoLUT.h"
#include "MuonSectorProcessor.h"
#include "TriggerProcessor.h"

#include "TrigT1Interfaces/Lvl1MuCTPIInputPhase1.h"

namespace LVL1
{
  class MuCTPIL1Topo;
}

namespace LVL1MUCTPIPHASE1 {

  class SimController
  {
    
  public:
    typedef std::array<MuonSectorProcessor,
                       LVL1MUONIF::Lvl1MuCTPIInputPhase1::NumberOfMuonSubSystem> MuonSectorProcessors;

    SimController() = default;

    std::vector<std::string> configureTopo(const std::string& barrelFileName,
					   const std::string& ecfFileName,
					   const std::string& side0LUTFileName,
					   const std::string& side1LUTFileName);

    std::string processData(LVL1MUONIF::Lvl1MuCTPIInputPhase1* input, int bcid=0);
    void setConfiguration( const Configuration& conf );

    LVL1::MuCTPIL1Topo getL1TopoData(int bcid) const;

    TriggerProcessor& getTriggerProcessor() { return m_triggerProcessor; }
    MuonSectorProcessors& getMuonSectorProcessors() { return m_muonSectorProcessors; }

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

    TriggerProcessor m_triggerProcessor;
    MuonSectorProcessors m_muonSectorProcessors{
      MuonSectorProcessor(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideA()),
      MuonSectorProcessor(LVL1MUONIF::Lvl1MuCTPIInputPhase1::idSideC())};
  };

}

#endif // TRIGT1MUCTPIPHASE1_SIMCONTROLLER_H
