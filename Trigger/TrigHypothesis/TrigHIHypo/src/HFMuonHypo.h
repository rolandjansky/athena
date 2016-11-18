/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInterfaces/HypoAlgo.h"
namespace HLT{
  class TriggerElement;
}

class HFMuonHypo : public HLT::HypoAlgo {
public:
  HFMuonHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~HFMuonHypo();
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

private:

  // monitoring
  int   m_eloss_all;
  int   m_eloss_passing;

  //Configuration
  float m_eloss_cut;
};
