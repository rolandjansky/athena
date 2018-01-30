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

  //Soon to be depreciated 
  unsigned int mu_count=0;                                                                                                  
  unsigned int hit=0; // Tracks total number of LVL2 muons with matching level 1 triggers
  unsigned int total=0; // Total number of LVL2 muons
  //

  std::vector<const xAOD::Muon*> m_good_muons;
  //Soon to be depreciated
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
  //
  

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

  std::vector<float> m_eta_total_thr1; //Threshold 1, combined barrel and endcap
  std::vector<float> m_eta_passing_thr1;

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

  std::vector<float> m_eta_total_thr2; //Threshold 2, combined barrel and endcap
  std::vector<float> m_eta_passing_thr2;

  std::vector<float> m_eta_total_thr3_b; 
  std::vector<float> m_eta_passing_thr3_b;
  std::vector<float> m_phi_total_thr3_b;
  std::vector<float> m_phi_passing_thr3_b;
  std::vector<float> m_pt_total_thr3_b;
  std::vector<float> m_pt_passing_thr3_b;

  std::vector<float> m_eta_total_thr3_e;
  std::vector<float> m_eta_passing_thr3_e;
  std::vector<float> m_phi_total_thr3_e;
  std::vector<float> m_phi_passing_thr3_e;
  std::vector<float> m_pt_total_thr3_e;
  std::vector<float> m_pt_passing_thr3_e;

  std::vector<float> m_eta_total_thr3; 
  std::vector<float> m_eta_passing_thr3;


  std::vector<float> m_eta_total_thr4_b; 
  std::vector<float> m_eta_passing_thr4_b;
  std::vector<float> m_phi_total_thr4_b;
  std::vector<float> m_phi_passing_thr4_b;
  std::vector<float> m_pt_total_thr4_b;
  std::vector<float> m_pt_passing_thr4_b;

  std::vector<float> m_eta_total_thr4_e;
  std::vector<float> m_eta_passing_thr4_e;
  std::vector<float> m_phi_total_thr4_e;
  std::vector<float> m_phi_passing_thr4_e;
  std::vector<float> m_pt_total_thr4_e;
  std::vector<float> m_pt_passing_thr4_e;

  std::vector<float> m_eta_total_thr4;
  std::vector<float> m_eta_passing_thr4;


  std::vector<float> m_eta_total_thr5_b; 
  std::vector<float> m_eta_passing_thr5_b;
  std::vector<float> m_phi_total_thr5_b;
  std::vector<float> m_phi_passing_thr5_b;
  std::vector<float> m_pt_total_thr5_b;
  std::vector<float> m_pt_passing_thr5_b;

  std::vector<float> m_eta_total_thr5_e;
  std::vector<float> m_eta_passing_thr5_e;
  std::vector<float> m_phi_total_thr5_e;
  std::vector<float> m_phi_passing_thr5_e;
  std::vector<float> m_pt_total_thr5_e;
  std::vector<float> m_pt_passing_thr5_e;

  std::vector<float> m_eta_total_thr5; 
  std::vector<float> m_eta_passing_thr5;


  std::vector<float> m_eta_total_thr6_b; 
  std::vector<float> m_eta_passing_thr6_b;
  std::vector<float> m_phi_total_thr6_b;
  std::vector<float> m_phi_passing_thr6_b;
  std::vector<float> m_pt_total_thr6_b;
  std::vector<float> m_pt_passing_thr6_b;

  std::vector<float> m_eta_total_thr6_e;
  std::vector<float> m_eta_passing_thr6_e;
  std::vector<float> m_phi_total_thr6_e;
  std::vector<float> m_phi_passing_thr6_e;
  std::vector<float> m_pt_total_thr6_e;
  std::vector<float> m_pt_passing_thr6_e;

  std::vector<float> m_eta_total_thr6; 
  std::vector<float> m_eta_passing_thr6;



  std::vector<float> m_etaphi_eff_thr1_b;
  std::vector<float> m_pt_eff_thr1_b;
  std::vector<float> m_etaphi_eff_thr2_b;
  std::vector<float> m_pt_eff_thr2_b;
  std::vector<float> m_etaphi_eff_thr3_b;
  std::vector<float> m_pt_eff_thr3_b;
  std::vector<float> m_etaphi_eff_thr4_b;
  std::vector<float> m_pt_eff_thr4_b;
  std::vector<float> m_etaphi_eff_thr5_b;
  std::vector<float> m_pt_eff_thr5_b;
  std::vector<float> m_etaphi_eff_thr6_b;
  std::vector<float> m_pt_eff_thr6_b;


  std::vector<float> m_etaphi_eff_thr1_e;
  std::vector<float> m_pt_eff_thr1_e;
  std::vector<float> m_etaphi_eff_thr2_e;
  std::vector<float> m_pt_eff_thr2_e;
  std::vector<float> m_etaphi_eff_thr3_e;
  std::vector<float> m_pt_eff_thr3_e;
  std::vector<float> m_etaphi_eff_thr4_e;
  std::vector<float> m_pt_eff_thr4_e;
  std::vector<float> m_etaphi_eff_thr5_e;
  std::vector<float> m_pt_eff_thr5_e;
  std::vector<float> m_etaphi_eff_thr6_e;
  std::vector<float> m_pt_eff_thr6_e;


  std::vector<float> m_eta_eff_thr1;
  std::vector<float> m_eta_eff_thr2;
  std::vector<float> m_eta_eff_thr3;
  std::vector<float> m_eta_eff_thr4;
  std::vector<float> m_eta_eff_thr5;
  std::vector<float> m_eta_eff_thr6;





  std::vector<std::vector <float>* > Thresh_Mon; 

  
  const DataVector<LVL1::RecMuonRoI>* m_l1_muon_RoIs; //To access Storegate need to use DataVector pointer which always assumes pointer elements
  
  bool m_debug; //Flag for conditional DEBUG output
  bool m_verbose;

};
#endif
