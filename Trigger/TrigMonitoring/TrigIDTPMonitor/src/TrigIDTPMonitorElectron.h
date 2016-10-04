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
 **   Author: Jonathon Langford <jonathon.langford@cern.ch>
 **           ATLAS Summer Student - private email: jonno_1995@hotmail.co.uk
 **                                  (for queries after my CERN account has been closed)
 **
 ** 
 **   Created:   04.07.2016
 **   Modified:  25.08.2016
 **
 **
 ***************************************************************************/

#ifndef TRIGIDTPMONITORELECTRON_H
#define TRIGIDTPMONITORELECTRON_H


#include "TrigInterfaces/FexAlgo.h"
#include "AthLinks/ElementLink.h"
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
  
  //monitored containers
  std::vector<double> m_PTinv;
  std::vector<double> m_FTFinv;
  std::vector<double> m_PTOptinv;
  std::vector<double> m_FTFOptinv;
  std::vector<double> m_PTinvTrack;
  std::vector<double> m_FTFinvTrack;
  std::vector<double> m_invCluster;
  std::vector<double> m_invCluster_before_cuts;
  std::vector<double> m_invClusterTagTrack;
  std::vector<double> m_invClusterTagTrack_before_cuts;
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
  std::vector<double> m_FTFdeltaR;
  std::vector<double> m_PTOptDeltaR;
  std::vector<double> m_FTFOptDeltaR;
  std::vector<double> m_tagClusterEta;
  std::vector<double> m_tagClusterPhi;
  std::vector<double> m_tagClusterEta_before_cuts;
  std::vector<double> m_tagClusterPhi_before_cuts;
  std::vector<double> m_dZ0;


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
  double m_PTeta;
  double m_PTphi;
  double m_ptEfficiencyProfile;
  double m_invMassEfficiencyProfile;
  double m_phiEfficiencyProfile;
  double m_etaEfficiencyProfile;
  double m_PTfound;
  double m_PTfoundAlt;
  double m_PTfoundCombined;
  double m_FTFfound;
  int m_PTnumberOfPixelHits;
  int m_PTnumberOfSCTHits;
  int m_PTnumberOfTRTHits;
  double m_PTpixelFound;
  double m_PTpixelNextToFound;

};


#endif
  //temporary fix
  //double m_PTphiNextToInnermost;
  //double m_PTetaNextToInnermost;
  //double m_FTFphiEfficiencyProfile;
  //double m_FTFetaEfficiencyProfile;
