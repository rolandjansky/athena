/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigMonitoring/TrigIDTPMonitor/TrigIDTPMonitorElectron.h
 **
 **   Description: Monitoring algorithm for ID T&P Z->ee candidates
 **                
 **
 **   Author: Sebastian Sanchez Herrera
 **           Johnny Raine <johnny.raine@cern.ch
 **   Modified by: Jonathon Langford
 **
 **   Created:   19.01.2015
 **   Modified:  05.07.2016
 **
 **
 ***************************************************************************/

#ifndef TRIGIDTPMONITORELECTRON_H
#define TRIGIDTPMONITORELECTRON_H


#include "TrigInterfaces/FexAlgo.h"
#include "DataModel/ElementLink.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>

class TrigIDTPMonitorElectron: public HLT::FexAlgo{
 public:

  TrigIDTPMonitorElectron(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigIDTPMonitorElectron();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltBeginRun(); 
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode acceptInput(const HLT::TriggerElement* inputTE, bool& pass);
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  
  
 private:
  double CombinedInvMass(double ptTag, double pxTag, double pyTag, double pzTag,const xAOD::TrackParticle* probe);
  double TrackInvMass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2);
  double ClusterInvMass(double ptTag, double pxTag, double pyTag, double pzTag, double ptCluster, double pxCluster, double pyCluster, double pzCluster);
  double ClusterTagTrackInvMass(const xAOD::TrackParticle* tag, double ptCluster, double pxCluster, double pyCluster, double pzCluster);
  // double compatibleIDTrack(const xAOD::TrackParticle* tag, const xAOD::TrackParticle* probe, const xAOD::TrackParticle* IDtrack);
  
  //variables
  //double m_mumuMass;

  //cuts
  //double m_massAcceptanceTP;
  //double m_massAcceptanceTID;
  //double m_lowerPtCutP;
  //double m_lowerPtCutT;
  //double m_lowerPtCutID;
  //double m_etaUpperCut;
  
  //monitored containers
  //  std::vector<double> m_trackInvMass;

  std::vector<double> m_PTinv;
  std::vector<double> m_FTFinv;
  std::vector<double> m_PTOptinv;
  std::vector<double> m_FTFOptinv;
  std::vector<double> m_PTinvTrack;
  std::vector<double> m_FTFinvTrack;
  //std::vector<double> m_PTinvTrack_before_cuts;
  //std::vector<double> m_FTFinvTrack_before_cuts;
  std::vector<double> m_invCluster;
  std::vector<double> m_invCluster_before_cuts;
  std::vector<double> m_invClusterTagTrack;
  std::vector<double> m_invClusterTagTrack_before_cuts;
  //std::vector<double> m_invTrial;
  std::vector<double> m_clusterPT;
  std::vector<double> m_clusterPhi;
  std::vector<double> m_clusterEta;
  std::vector<double> m_clusterPT_before_cuts;
  std::vector<double> m_clusterPhi_before_cuts;
  std::vector<double> m_clusterEta_before_cuts;
  std::vector<double> m_deltaPhi;
  std::vector<double> m_deltaPhi_before_cuts;
  std::vector<double> m_clusterPx;
  std::vector<double> m_clusterPy;
  std::vector<double> m_clusterPz;
  std::vector<double> m_PTdeltaR;  
  std::vector<double> m_PTOptDeltaR;

  std::vector<int> m_PTnumber;
  std::vector<int> m_FTFnumber;


  //std::vector<double> m_lines;
  /*
  std::vector<double> m_invTP;
  std::vector<double> m_linesTP;
 
  std::vector<double> m_pt;
  std::vector<double> m_linespt;
 
  std::vector<double> m_eta;
  std::vector<double> m_lineseta;
 
  std::vector<double> m_d0;
  std::vector<double> m_linesd0;
 
  std::vector<double> m_phi;
  std::vector<double> m_linesphi;
 
  std::vector<double> m_eff;
 
  std::vector<double> m_usedTracks;
  */
  //monitored values
  double m_PTOptInv;
  double m_FTFOptInv;
  double m_PTdeltaEta;
  double m_FTFdeltaEta;
  double m_PTdeltaPhi;
  double m_PTdeltaPhiPositron;
  double m_FTFdeltaPhi;
  double m_tagEta;
  double m_tagpT;
  double m_tagTrackpT;
  double m_tagDeltaEta;
  double m_tagDeltaPhi;
  double m_tagExpectedDeltaPhi;
  double m_tagPoverE;
  double m_tagZ0;
  double m_dZ0;

  double m_ptEfficiencyProfile;
  double m_invMassEfficiencyProfile;
  double m_phiEfficiencyProfile;
  double m_etaEfficiencyProfile;
  double m_PTfound;
  double m_PTfoundAlt;
  double m_PTfoundCombined;
  double m_FTFfound;
  int m_PTnumberTracks;
  int m_FTFnumberTracks;
  int m_PTnumberCuts;
  int m_FTFnumberCuts;

  int m_PTnumberOfPixelHits;
  int m_PTnumberOfSCTHits;
  int m_PTnumberOfTRTHits;

    /*     
  double m_invBS; //Invariant mass from TrigEFBPhys object before selection
  double m_invD; //Invariant mass from TrigEFBPhys object
  double m_invN; //Invariant mass from TrigEFBPhys object (if ID Track)
  double m_invTPD; //Invariant mass calculated from Tag and Probe Tracks
  double m_invTPN; //Difference between m_inv and m_invC
  double m_ptD; //pT of the Probe Muon
  double m_ptN; //pT of the Probe Muon (if ID Track)
  double m_etaD; //eta of the Probe Muon
  double m_etaN; //eta of the Probe Muon (if ID Track)
  double m_d0D; //d0 of the Probe Muon
  double m_d0N; //d0 of the Probe Muon (if ID Track)
  double m_phiD;//phi of the Probe Muon
  double m_phiN;//phi of the Probe Muon(if ID Track)
  double m_IDTmass; //Inveriant mass calculated from ID track and CombMuon
  double m_massDiff;//Difference between invariant masses
  double m_PTfound; //found probe PT track
  double m_FTFfound; //found probe FTF tack
  */
};


#endif
