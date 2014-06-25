/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C++  
// :-)
/**************************************************************************
 **
 **   FILE NAME:   TrigMuonJetHypo.h
 **   PACKAGE:     Trigger/TrigHypothesis/TrigJetHypo
 **
 **   DESCRIPTION: Hyphotesis algorithm header file
 **            
 **
 **   AUTHORS: Stefano.Giagu@cern.ch
 **            Antonio.Policicchio@cern.ch
 **
 **   CREATED:  November, 2008
 **   MODIFIED: ---  
 **              
 **************************************************************************/ 

#ifndef TRIGMUONJETHYPO_H
#define TRIGMUONJETHYPO_H

//** Trigger includes **//
#include "TrigInterfaces/HypoAlgo.h"

//** trigger EDM **//
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

class StoreGateSvc;
class MsgStream;
class TriggerElement;

class TrigMuonJetHypo : public HLT::HypoAlgo {

 public:

  TrigMuonJetHypo (const std::string&, ISvcLocator*);
  ~TrigMuonJetHypo ();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement*, bool&);
  HLT::ErrorCode hltFinalize(); 

 private:

  //** @brief DeclareProperty: if acceptAll flag is set to true, every event is taken. *// 
  bool   m_acceptAll;
  bool   m_deltaRMatching;
  double m_deltaRCut;
  bool   m_useL2Muons;
  std::string m_instance;


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


  /** monitored variables... see pythons **/
  double mJetEne;
  int mNumTrk;
  double mdeltaR;


};

#endif

