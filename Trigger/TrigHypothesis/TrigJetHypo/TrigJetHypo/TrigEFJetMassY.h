/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFJetMassY.cxx
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   Oliver Endner
 *
 *********************************************************************/
#ifndef TRIGEFJETMASSY_H
#define TRIGEFJETMASSY_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigEFJetMassY : public HLT::HypoAlgo {

 public:

  TrigEFJetMassY(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFJetMassY();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied:

  double     m_MjjCut;
  double     m_YstarCut;



  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.

  bool m_acceptAll;

  // Switch on Monitoring:
  
  bool m_doMonitoring;

  int m_cutCounter;
  int m_jetnr;
  double m_ystar;
  double m_mjj;
  double m_ystar_cuted;
  double m_mjj_cuted;

  double m_pt;
  double m_pt_cuted;
  double m_eta;  
  double m_eta_cuted;
  

  //StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;

};
#endif

