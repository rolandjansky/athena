/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEtaHypo_H
#define TrigEtaHypo_H

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigEtaHypo.h
/// Package : Trigger/TrigAlgorithms/TrigDetCalib
/// Author  : Nicolas Berger
/// Created : Sep. 2008
///
/// DESCRIPTION: simple hypo algorithms to limit eta range
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <string>

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class TrigEtaHypo : public HLT::HypoAlgo {

 public:

  TrigEtaHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigEtaHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);


 private:

  double m_etaMinCut, m_etaMaxCut;
  std::string m_roILabel;
  
  int m_acceptedEvts;
  int m_rejectedEvts;
  int m_errorEvts;

  // Switch to accept all the events.
  bool m_acceptAll;

  // Switch on Monitoring:
  bool m_doMonitoring;

  // Timing:
  ITrigTimerSvc*            m_timerSvc;
  std::vector<TrigTimer*>   m_timers;
};
#endif
