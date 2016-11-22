/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2HTFULLSCANHYPO_H
#define TRIGL2HTFULLSCANHYPO_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigL2HTFullScanHypo.h
/// Package : Trigger/TrigHypothesis/TrigMultiJetHypo/
/// Author  : Matthew Tamsett (tamsett@cern.ch)
///           Marco Agustoni (marco.agustoni@cern.ch)
/// Created : November 2011
///
/// DESCRIPTION: simple hypo algorithms for Lvl2 Jet, wich compute HT (sum of jet transverse energies)
/// Cuts on transverse energy, eta and HT are applied
/// This is based on the TrigL2JetHypo and the TrigEFJetHypo
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "TrigInterfaces/HypoAlgo.h"
//#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigL2HTFullScanHypo : public HLT::HypoAlgo {

 public:

  TrigL2HTFullScanHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigL2HTFullScanHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);


 private:

  // define the properties:
  //----------------------------

  std::string  m_jetInputKey;

  // Cuts to be applied:
  double m_EtCut;
  double m_HTCut;
  double m_etaMinCut;
  double m_etaMaxCut;
  
  int m_accepted_L2;
  int m_rejected_L2;
  int m_errors_L2;

  // Switch to accept all the events.

  bool m_acceptAll;

  // Switch on Monitoring:

  bool m_doMonitoring;
  
  // Monitored Variables
  double	      m_HT;		// sum of jet transverse energies 
  double              m_n_tot_Jets;     // Total number of jets
  double              m_n_acc_Jets;     // The number of jets used to compute HT
  double              m_n_rej_Jets;     // The number of jets not used to compute HT    
  int                 m_cutCounter;     // flag for event rejection
  std::vector<double> m_e;              // energy
  std::vector<double> m_et_acc;         // transverse energy for jets used to compute HT
  std::vector<double> m_eta_acc;        // eta for jets used to compute HT
  std::vector<double> m_phi_acc;        // phi for jets used to compute HT
  std::vector<double> m_et_rej;         // transverse energy for jets not in events that satisfy the trigger
  std::vector<double> m_eta_rej;        // eta for jets not in events that satisfy the trigger
  std::vector<double> m_phi_rej;        // phi for jets not in events that satisfy the trigger
  std::vector<int>    m_nLeadingTowers; // number of leading towers

  
  std::vector<int>    m_accepted_jets;    // a vector of the jets used to compute HT
  
  // Histogram Service:
  //IHistogramSvc* m_histsvc;

  //StoreGateSvc*                           m_storeGate;

  // Timing:

//  ITrigTimerSvc*            m_timersvc;
//  std::vector<TrigTimer*>   m_timers;
  
  /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */
  MsgStream* m_log;
};
#endif
