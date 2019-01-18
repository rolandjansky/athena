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
  // Configuration   
  float    m_FcalEt_min;
  float    m_FcalEt_max;
  float    m_Eta_min;
  float    m_Eta_max;
  bool     m_isFgap;
};
