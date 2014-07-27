/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigJetHypo/TrigL2JetDphiHypo.h
 **
 **   Description: - Hypothesis algorithm that cuts on Et and Dphi of 2 L2 Jets
 **
 **   Author: S. Owen  <simon.owen@sheffield.ac.uk>, Teresa Fonseca Martin, Ignacio Aracena
 **
 **   Created:   Dec 17 2009
 **
 **************************************************************************/ 

#ifndef TRIGJETHYPO_TRIGL2JETDPHIHYPO_H 
#define TRIGJETHYPO_TRIGL2JETDPHIHYPO_H 

// standard stuff
#include <string>
#include <map>
#include <cmath> 
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
//#include "StoreGate/StoreGateSvc.h"
// trigger includes
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
//class TriggerElement;

class TrigL2JetDphiHypo: public HLT::HypoAlgo  {
  
 public:
  TrigL2JetDphiHypo(const std::string & name, ISvcLocator* pSvcLocator);
  ~TrigL2JetDphiHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:
  //Monitoring variables
  bool m_doMonitoring;
  //Monitoring
  float m_Passed_dphi;
  int m_nTeDphiHypoCounter, m_nJetsTe1, m_nJetsTe2, m_dphiHypoResult;
  // m_dphi is a monitored variale but also used in the trigger
  float m_ptLeadingJet, m_ptNextLeadingJet, m_etaLeadingJet, m_etaNextLeadingJet, m_phiLeadingJet, m_phiNextLeadingJet;
  
  // No timing internal to alg. Should be done automatically.
  // Timing:
  //ITrigTimerSvc*            m_timersvc;
  //std::vector<TrigTimer*>   m_timers;
  
  //Not sure: do we want double or float?
  // variable to be cut    
  float m_dphi;



  // cut
  float m_JetDphiMaxCut;
  float m_JetDphiMinCut;
  float m_ptHardJetCut;
  float m_ptSoftJetCut;
  
  // auxiliary vectors to range jets in Et order
  std::vector<float> m_vjetet1;
  std::vector<float> m_vjeteta1;
  std::vector<float> m_vjetphi1;
  std::vector<float> m_vjetet2;
  std::vector<float> m_vjeteta2;
  std::vector<float> m_vjetphi2;


  // Not sure:
  // to set Accept-All mode: should be done with force-accept when possible
  // bool m_acceptAll;  //!<  true will accept all events
  //Not sure: for sure we need monitoring
};

#endif
