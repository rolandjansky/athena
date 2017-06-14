/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2HVJETHYPOTRK_H
#define TRIGL2HVJETHYPOTRK_H
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigL2HVJetHypoTrk.h
/// Package : Trigger/TrigHypothesis/TrigJetHypo/
/// Authors  : Antonio Policicchio <antonio.policicchio@cern.ch>
/// Created : Jun. 2011
///
/// DESCRIPTION: simple hypo algorithms for Lvl2 Hidden Valley Jet; 
///              cut on number of ID tracks with Pt>1GeV in the 
///              range (etaJet-etaTrack)x(phiJet-phiTrack)
///
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigInDetEvent/TrigInDetTrack.h"


class StoreGateSvc;
class TriggerElement;

class TrigL2HVJetHypoTrk : public HLT::HypoAlgo {

 public:

  TrigL2HVJetHypoTrk(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigL2HVJetHypoTrk();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);


 private:

  // define the properties:
  //----------------------------


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
  double m_NumberOfTracks;
};
#endif
