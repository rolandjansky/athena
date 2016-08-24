/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/HypoAlgo.h"
#include "VnHypoHelper.h"
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
  VnHypoHelper m_helper;
  // monitoring
  float m_Tot_Et;
  float m_Tot_Et_passing;
  float m_qn;
  float m_qnx;
  float m_qny;
  float m_qn_passing;
  int   m_icent;
  int   m_icent_passing;
  std::vector<int > m_decision;
  //Configuration
  //  int m_ThresholdVnBin;
  int m_FlowHarmonic;
  
  //double m_q0xShift; // shifts
  //double m_q0yShift;
  bool m_upperLimit;
  float m_minEta; 
  float m_maxEta;

};
