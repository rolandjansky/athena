/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFCentralJet.h (based on TrigEFJetHypo.h
 *
 *
 *********************************************************************/
#ifndef TRIGEFCENTRALJET_H
#define TRIGEFCENTRALJET_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigEFCentralJet : public HLT::HypoAlgo {

 public:

  TrigEFCentralJet(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEFCentralJet();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied:

  double     eta_range;

  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch on Monitoring:
  bool m_doMonitoring;
  
  double all_jet0_et ;
  double all_jet0_eta ;
  double passed_jet0_et ;
  double passed_jet0_eta ;

  // path for the histograms:

  //std::string m_path;

  // Histogram Service:
  //IHistogramSvc* m_histsvc;

  StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;

};
#endif

