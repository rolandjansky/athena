/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/HypoAlgo.h"
#include <vector>
namespace HLT{
  class TriggerElement;
}

class UltraCentralHypo : public HLT::HypoAlgo {
public:
  UltraCentralHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~UltraCentralHypo();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

private:

  // monitoring
  float m_Tot_Et;
  float m_Tot_Et_passing;
  
  // Coniguration 
  std::vector<float>    m_FcalEt_min;
  std::vector<float>    m_FcalEt_max;
  int m_max_index; 
  int m_min_index;   
};
