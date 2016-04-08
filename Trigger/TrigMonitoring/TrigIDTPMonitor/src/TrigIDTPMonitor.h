/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigMonitoring/TrigIDTPMonitor/TrigIDTPMonitor.h
 **
 **   Description: Monitoring algorithm for ID T&P
 **                
 **
 **   Author: Sebastian Sanchez Herrera
 **           Johnny Raine <johnny.raine@cern.ch
 **
 **   Created:   19.01.2015
 **   Modified:  04.05.2015
 **
 **
 ***************************************************************************/

#ifndef TRIGIDTPMONITOR_H
#define TRIGIDTPMONITOR_H


#include "TrigInterfaces/FexAlgo.h"
#include "DataModel/ElementLink.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>

class TrigIDTPMonitor: public HLT::FexAlgo{
 public:

  TrigIDTPMonitor(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigIDTPMonitor();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltBeginRun(); 
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode acceptInput(const HLT::TriggerElement* inputTE, bool& pass);
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
  
  
 private:

  double TrackInvMass(const xAOD::TrackParticle* track1, const xAOD::TrackParticle* track2);
  double compatibleIDTrack(const xAOD::TrackParticle* tag, const xAOD::TrackParticle* probe, const xAOD::TrackParticle* IDtrack);
  
  //variables
  double m_mumuMass;

  //cuts
  double m_massAcceptanceTP;
  double m_massAcceptanceTID;
  double m_lowerPtCutP;
  double m_lowerPtCutT;
  double m_lowerPtCutID;
  double m_etaUpperCut;
  
  //monitored containers
  std::vector<double> m_inv;
  std::vector<double> m_lines;
 
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

  //monitored values
    
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

};


#endif
