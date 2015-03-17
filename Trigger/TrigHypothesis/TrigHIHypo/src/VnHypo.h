/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/HypoAlgo.h"
namespace HLT{
  class TriggerElement;
}

class VnHypo : public HLT::HypoAlgo {
public:
  VnHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~VnHypo();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

private:

  // monitoring
  float m_Tot_Et;
  float m_Tot_Et_passing;
  float m_qn;
  float m_qn_passing;
  int   m_icent;
  int   m_icent_passing;

  //Configuration
  int m_ThresholdVnBin;
  int m_FlowHarmonic  ;
};
