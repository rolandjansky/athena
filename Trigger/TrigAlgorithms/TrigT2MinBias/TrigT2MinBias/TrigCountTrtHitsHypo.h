/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIAS_TRIGTRTHITCOUNTERHYPO_H 
#define TRIGMINBIAS_TRIGTRTHITCOUNTERHYPO_H 

#include "GaudiKernel/IToolSvc.h"
#include "TrigInterfaces/HypoAlgo.h"

class TrigCountTrtHitsHypo: public HLT::HypoAlgo {
    
 public:
  TrigCountTrtHitsHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigCountTrtHitsHypo();
    
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltBeginRun();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    
 private:

  HLT::ErrorCode checkDetectorMask();
  Bool_t m_hltExecuteInitialisationRun; //!< Flag to run extra initialisation on the first event when xAOD::EventInfo is present

  MsgStream m_log;
  bool m_acceptAll;

  /** detector mask flags */
  bool m_trt_barrel_a_side;
  bool m_trt_barrel_c_side;
  bool m_trt_endcap_a_side;
  bool m_trt_endcap_c_side;

  double m_nTrtHits;
  double m_nTrtHits_B;
  double m_nTrtHits_A;
  double m_nTrtHits_C;
  float m_maxTrtHitsEC;
  float m_totalTrtHits_cut;
  float m_trtHitsEC_cut;
  float m_timeOverThresholdCut;

};
#endif
