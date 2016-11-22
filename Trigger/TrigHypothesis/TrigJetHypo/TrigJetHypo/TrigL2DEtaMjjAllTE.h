/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2DETAALLTE_H
#define TRIGL2DETAALLTE_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigL2DEtaMjjAllTE.h
/// Package : Trigger/TrigHypothesis/TrigL2DEtaMjjAllTE
/// Author  : Justin Griffiths (griffith@cern.ch
///           Modeled after TrigL2MultiJetAllTE.h
/// Created : September 2012
///
/// DESCRIPTION: will read in all TEs output from the L1.5 hypo, check that these jets satisfy the hypothesis, 
/// then attach all those who pass the cuts to outputTEs
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

class TrigL2DEtaMjjAllTE : public HLT::AllTEAlgo {

 public:

  TrigL2DEtaMjjAllTE(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigL2DEtaMjjAllTE();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
                            unsigned int output);


 private:

  // define the properties:
  //----------------------------
  // input jet collection
  std::string  m_jetInputKey;

  // Cuts to be applied:

  double     m_EtCut;
  double     m_etaMinCut;
  double     m_etaMaxCut;

  // Cuts to select passing events
  double     m_dEtaCut;
  double     m_MjjCut;

  int        m_accepted_L2;
  int        m_rejected_L2;
  int        m_errors_L2;

  // Switch on Monitoring:

  bool m_doMonitoring;
  
  // Monitored Variables
  double              m_nJets;          // The number of jets accepted  
  int                 m_cutCounter;     // The number of events which pass
  double              m_dEta;
  double              m_Mjj;
  std::vector<double> m_e;              // energy
  std::vector<double> m_et;             // transverse energy
  std::vector<double> m_eta;            // eta
  std::vector<double> m_phi;            // phi
  
  // Histogram Service:
  //IHistogramSvc* m_histsvc;

  //StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;
  
  /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */
  MsgStream* m_log;
  
};
#endif
