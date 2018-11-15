/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_TRIGMUONEFCOMBINERHYPO_H 
#define TRIG_TRIGMUONEFCOMBINERHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"
#include "LumiBlockComps/ILuminosityTool.h"

class TriggerElement;

class TrigMuonEFCombinerHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
 public:
  TrigMuonEFCombinerHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigMuonEFCombinerHypo();

  HLT::ErrorCode hltInitialize();    
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
  HLT::ErrorCode hltFinalize();

 private:

  // Properties:
  std::vector<float> m_ptBins;
  std::vector<float> m_ptThresholds;
  BooleanProperty m_acceptAll;
  BooleanProperty m_rejectCBmuons;
  ToolHandle<ILuminosityTool> m_lumiTool;

  // Other members:   
  std::vector<float>::size_type m_bins;
  std::vector<float> m_fex_pt;
  std::vector<float> m_fex_eta;
  std::vector<float> m_fex_phi;
  float m_fex_rate;
  float m_fex_bcid;
};

#endif
