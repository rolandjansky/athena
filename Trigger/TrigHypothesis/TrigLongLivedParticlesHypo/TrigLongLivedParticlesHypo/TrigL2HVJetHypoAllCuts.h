/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2HVJETHYPOALLCUTS_H
#define TRIGL2HVJETHYPOALLCUTS_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigL2HVJetHypoAllCuts.h
/// Package : Trigger/TrigHypothesis/TrigJetHypo/
/// Authors  : Stefano Giagu <stefano.giagu@cern.ch>, Antonio Policicchio <antonio.policicchio@cern.ch>
/// Created : Sept. 2008
///
/// DESCRIPTION: simple hypo algorithms for Lvl2 Hidden Valley Jet, cut on energy and log10 of 
///              had/em energy ratio; cut on number of ID tracks with Pt>1GeV in the 
///              range (etaJet-etaTrack)x(phiJet-phiTrack)
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigInDetEvent/TrigInDetTrack.h"


class StoreGateSvc;
class TriggerElement;

class TrigL2HVJetHypoAllCuts : public HLT::HypoAlgo {

 public:

  TrigL2HVJetHypoAllCuts(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigL2HVJetHypoAllCuts();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);


 private:

  // define the properties:
  //----------------------------

  // Cuts to be applied for Jets
  /** cut on jet Et */
  double     m_EtCut_L2;
  /** cut on Log(H/E) */ 
  double     m_LRatioCut_L2;
  /** cut on jet eta */
  double     m_JetEta;

  //ID
  /** Use ID. */
  BooleanProperty           m_UseID;
  /** ID Algorthm to use. */
  StringProperty            m_ID_algo_to_use;
  /** ID algorithm. */
  TrigInDetTrack::AlgoId m_algoId;
  /** Minimum Pt for tracks in ID based isolation. */
  double     m_PtMinID;
  /** Cut for (JetEta-TrackEta) */
  double     m_Deta;
  /** Cut for (JetPhi-TrackPhi) */
  double     m_Dphi;
  /** Cut on Tracks in the range (JetEta-TrackEta)x(JetPhi-TrackPhi) */
  int m_nTracks;



  // cleaning:
  // ---------
  // flags (do specific jet cleaning or don't):
  bool m_doHECfnLeadingCellsCleaning;
  bool m_doEMfJetQualityCleaning;
  bool m_doHECfJetQualityCleaning;
  bool m_doJetTimeCleaning;
  bool m_doLowEMfCleaning;
  // thresholds for cleaning cuts: 
  int m_leadingCellsThr;        // threshold for number of leading cells in (hecf > m_leadingCellsThr &&  nLeadingCells <= m_leadingCellsThr) cut
  float m_hecfThrN;             // hecf threshold for cut in combination with nLeadingCells (i.e. hecf > m_leadingCellsThr &&  nLeadingCells <= m_leadingCellsThr)
  float m_hecfThrQ;             // hecf threshold for cut in combination with jet Quality (i.e. hecf > m_hecfThrQ && |quality| > m_jetQualityThrH)
  float m_jetQualityThrH;       // quality threshold for cut in combination with hecf (i.e. hecf > m_hecfThrQ && |quality| > m_jetQualityThrH)
  float m_jetQualityThrE;       // quality threshold for cut in combination with emf (i.e. emf > m_emfThrQ && |quality| > m_jetQualityThrE)
  float m_emfThrQ;              // emf threshold for cut in combination with m_jetQualityThrE (i.e. emf > m_emfThrQ && quality > m_jetQualityThrE)
  float m_emfThrE;              // emf threshold for Exclusive emf cut (i.e. emf < m_emfThrE)
  float m_jetTimeCellsThr;      // threshold for identification of out-of-time jets

  float m_hecf;
  int m_nLeadingCells;
  float m_emf;
  float m_fabsjetQuality;
  float m_fabsjetTimeCells;


  //Events accepted, rejected and with some error
  int m_accepted_L2;
  int m_rejected_L2;
  int m_errors_L2;

  // Switch to accept all the events.
  bool m_acceptAll;

  // Switch on Monitoring:
  bool m_doMonitoring;

  //Monitored Variables
  int m_cutCounter;
  double m_JetEt;
  double m_logRatio;
  double m_NumberOfTracks;
};
#endif
