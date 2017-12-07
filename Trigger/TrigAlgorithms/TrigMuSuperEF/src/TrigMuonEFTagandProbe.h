/*                 
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

  In progress header for TrigMuonEFTagandProbe algorithm
  JJamieson, University of Glasgow, 09/10/2017, Edit:01/12/17
*/

#ifndef TRIGMUONEFTAGANDPROBE_TRIGMUONEFTAGANDPROBE_H
#define TRIGMUONEFTAGANDPROBE_TRIGMUONEFTAGANDPROBE_H

#include <string>
#include <vector>
#include <map>

//Will almost definitely need these

#include "TrigInterfaces/Algo.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigT1Interfaces/RecMuonRoI.h"

//Structure for holding sets of useful Tag and Probe muon information
struct TaPMuon { 
  const xAOD::Muon* Mu;
  float DR;
  unsigned int Thresh;
  const LVL1::RecMuonRoI* RoI;
};


class TriggerElement;

class TrigMuonEFTagandProbe: public virtual HLT::FexAlgo { //TaP algorithm will inherit from HLT::FexAlgo
 public:

  //constructor has to have same inputs as HLT::FexAlgo constructor, these are algorithm name and Service locator
  TrigMuonEFTagandProbe (const std::string &name, ISvcLocator *pSvcLocator); 

  virtual ~TrigMuonEFTagandProbe(); //destructor

  virtual HLT::ErrorCode hltInitialize();
  virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*); //Execute takes in two TEs
  //  virtual HLT::ErrorCode hltEndEvent();
  virtual HLT::ErrorCode hltFinalize();

 private:

  void trim_container(std::vector<const xAOD::Muon*>& good_muons);
  void match_thresh(TaPMuon tap,std::vector<std::vector <float>* >& thresh_mon);
  void match_thresh(const xAOD::Muon* muon,std::vector<std::vector <float>* >& thresh_mon);

  unsigned int mu_count=0;                                                                                                  
  unsigned int hit=0; // Tracks total number of LVL2 muons with matching level 1 triggers
  unsigned int total=0; // Total number of LVL2 muons


  std::vector<const xAOD::Muon*> m_good_muons;
  std::vector<float> m_dimuon_invmass;
  std::vector<float> m_delta_r_tag;
  std::vector<float> m_delta_r_probepass;
  std::vector<float> m_delta_z;
  

  std::vector<float> m_delta_r_thresh1;
  std::vector<float> m_delta_r_thresh2;
  std::vector<float> m_delta_r_thresh3;
  std::vector<float> m_delta_r_thresh4;
  std::vector<float> m_delta_r_thresh5;
  std::vector<float> m_delta_r_thresh6;

  

  //Ideally I would want to change this to have each histogram set in it's own TDirectory but I don't know how yet

  std::vector<float> m_eta_total_thr1_b; //Threshold 1, barrel
  std::vector<float> m_eta_passing_thr1_b;
  std::vector<float> m_phi_total_thr1_b;
  std::vector<float> m_phi_passing_thr1_b;
  std::vector<float> m_pt_total_thr1_b;
  std::vector<float> m_pt_passing_thr1_b;

  std::vector<float> m_eta_total_thr1_e; //Threshold 1, endcap
  std::vector<float> m_eta_passing_thr1_e;
  std::vector<float> m_phi_total_thr1_e;
  std::vector<float> m_phi_passing_thr1_e;
  std::vector<float> m_pt_total_thr1_e;
  std::vector<float> m_pt_passing_thr1_e;


  std::vector<float> m_eta_total_thr2_b; //Threshold 2, barrel
  std::vector<float> m_eta_passing_thr2_b;
  std::vector<float> m_phi_total_thr2_b;
  std::vector<float> m_phi_passing_thr2_b;
  std::vector<float> m_pt_total_thr2_b;
  std::vector<float> m_pt_passing_thr2_b;

  std::vector<float> m_eta_total_thr2_e; //Threshold 2, endcap
  std::vector<float> m_eta_passing_thr2_e;
  std::vector<float> m_phi_total_thr2_e;
  std::vector<float> m_phi_passing_thr2_e;
  std::vector<float> m_pt_total_thr2_e;
  std::vector<float> m_pt_passing_thr2_e;


  std::vector<float> m_eta_total_thr3_b; //Threshold 3, barrel
  std::vector<float> m_eta_passing_thr3_b;
  std::vector<float> m_phi_total_thr3_b;
  std::vector<float> m_phi_passing_thr3_b;
  std::vector<float> m_pt_total_thr3_b;
  std::vector<float> m_pt_passing_thr3_b;

  std::vector<float> m_eta_total_thr3_e; //Threshold 3, endcap
  std::vector<float> m_eta_passing_thr3_e;
  std::vector<float> m_phi_total_thr3_e;
  std::vector<float> m_phi_passing_thr3_e;
  std::vector<float> m_pt_total_thr3_e;
  std::vector<float> m_pt_passing_thr3_e;


  std::vector<float> m_eta_total_thr4_b; //Threshold 4, barrel
  std::vector<float> m_eta_passing_thr4_b;
  std::vector<float> m_phi_total_thr4_b;
  std::vector<float> m_phi_passing_thr4_b;
  std::vector<float> m_pt_total_thr4_b;
  std::vector<float> m_pt_passing_thr4_b;

  std::vector<float> m_eta_total_thr4_e; //Threshold 4, endcap
  std::vector<float> m_eta_passing_thr4_e;
  std::vector<float> m_phi_total_thr4_e;
  std::vector<float> m_phi_passing_thr4_e;
  std::vector<float> m_pt_total_thr4_e;
  std::vector<float> m_pt_passing_thr4_e;


  std::vector<float> m_eta_total_thr5_b; //Threshold 5, barrel
  std::vector<float> m_eta_passing_thr5_b;
  std::vector<float> m_phi_total_thr5_b;
  std::vector<float> m_phi_passing_thr5_b;
  std::vector<float> m_pt_total_thr5_b;
  std::vector<float> m_pt_passing_thr5_b;

  std::vector<float> m_eta_total_thr5_e; //Threshold 5, endcap
  std::vector<float> m_eta_passing_thr5_e;
  std::vector<float> m_phi_total_thr5_e;
  std::vector<float> m_phi_passing_thr5_e;
  std::vector<float> m_pt_total_thr5_e;
  std::vector<float> m_pt_passing_thr5_e;


  std::vector<float> m_eta_total_thr6_b; //Threshold 6, barrel
  std::vector<float> m_eta_passing_thr6_b;
  std::vector<float> m_phi_total_thr6_b;
  std::vector<float> m_phi_passing_thr6_b;
  std::vector<float> m_pt_total_thr6_b;
  std::vector<float> m_pt_passing_thr6_b;

  std::vector<float> m_eta_total_thr6_e; //Threshold 6, endcap
  std::vector<float> m_eta_passing_thr6_e;
  std::vector<float> m_phi_total_thr6_e;
  std::vector<float> m_phi_passing_thr6_e;
  std::vector<float> m_pt_total_thr6_e;
  std::vector<float> m_pt_passing_thr6_e;

  std::vector<std::vector <float>* > Thresh_Mon; 
  
  const DataVector<LVL1::RecMuonRoI>* m_l1_muon_RoIs; //To access Storegate need to use DataVector pointer which always assumes pointer elements
  
  //std::vector<const LVL1::RecMuonRoI*> m_l1_muon_RoIs; //Keep in case we need a container of good RoIs
  
  bool m_debug; //Flag for conditional DEBUG output
  bool m_verbose;

};

#endif
