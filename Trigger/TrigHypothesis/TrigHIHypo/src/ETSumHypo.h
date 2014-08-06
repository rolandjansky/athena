/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include "TrigInterfaces/HypoAlgo.h"
namespace HLT{
  class TriggerElement;
}

class ETSumHypo : public HLT::HypoAlgo {
public:
  ETSumHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~ETSumHypo();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

private:
  std::vector<std::string> m_componentsProp;
  std::vector<float> m_calibrationProp;

  typedef std::map<std::string, float> CompSelection;
  CompSelection m_comps;
  
  // cuts
  float    m_lower;
  float    m_upper;

  // monitoring
  float m_sum_Et;
  float m_sum_Et_passing;

};
