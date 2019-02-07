/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "TrigInterfaces/HypoAlgo.h"
namespace HLT{
  class TriggerElement;
}

class LbyLTopoClusterHypo : public HLT::HypoAlgo {
public:
  LbyLTopoClusterHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~LbyLTopoClusterHypo();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

private:
  
  // cuts
  float    m_et1Threshold;
  float    m_et2Threshold;
  float m_dPhiCut;

  std::vector<float> m_allDPhi;

};
