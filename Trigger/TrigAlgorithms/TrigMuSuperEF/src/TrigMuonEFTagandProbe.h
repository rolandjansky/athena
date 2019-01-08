/*                 
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigMuonEFTagandProbe
  Package: Trigger/TrigAlgorithms/TrigMuSuperEF
  Authors: Jonathan Jamieson, University of Glasgow, Created 09/10/2017, Last edit: 28/05/18
*/

#ifndef TRIGMUONEFTAGANDPROBE_TRIGMUONEFTAGANDPROBE_H
#define TRIGMUONEFTAGANDPROBE_TRIGMUONEFTAGANDPROBE_H

#include <string>
#include <vector>

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


//Class to handle structuring and filling various monitoring variables by L1 trigger threshold
class MonitoringObject {

  std::vector<std::vector <float>* > m_Threshold1, m_Threshold2, m_Threshold3, m_Threshold4, m_Threshold5, m_Threshold6;
  std::vector<std::vector<std::vector <float>* > > m_ThreshMon;

 public:

  MonitoringObject (); //Constructor, Allows for setting up framework of vectors ahead of time

  void build_threshold (unsigned int, std::vector<float>&, std::vector<float>&, std::vector<float>&, std::vector<float>&, std::vector<float>&, std::vector<float>&, std::vector<float>&, std::vector<float>&, std::vector<float>&, std::vector<float>&, std::vector<float>&, std::vector<float>&); //(Threshold number, Threshold specific variables)

  void fill_eta_total_barrel (unsigned int, float); //Threshold number, monitoring value)
  void fill_phi_total_barrel (unsigned int, float);
  void fill_etaphi_matchfail_barrel (unsigned int, float);
  void fill_pt_total_barrel (unsigned int, float);
  void fill_pt_matchfail_barrel (unsigned int, float);
  void fill_eta_total_endcap (unsigned int, float);
  void fill_phi_total_endcap (unsigned int, float);
  void fill_etaphi_matchfail_endcap (unsigned int, float);
  void fill_pt_total_endcap (unsigned int, float);
  void fill_pt_matchfail_endcap (unsigned int, float);
  void fill_eta_total_merged (unsigned int, float);
  void fill_eta_matchfail_merged (unsigned int, float);


};


class TriggerElement;

class TrigMuonEFTagandProbe: public virtual HLT::FexAlgo { //TaP algorithm will inherit from HLT::FexAlgo
 public:

  TrigMuonEFTagandProbe (const std::string &name, ISvcLocator *pSvcLocator); 

  virtual ~TrigMuonEFTagandProbe(); //destructor

  virtual HLT::ErrorCode hltInitialize();
  virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*);
  virtual HLT::ErrorCode hltFinalize();

 private:

  void trim_container(std::vector<const xAOD::Muon*>& good_muons);

  void match_thresh(TaPMuon tap,MonitoringObject Thresh_Mon);
  void match_thresh(const xAOD::Muon* muon,MonitoringObject Thresh_Mon);

  std::vector<const xAOD::Muon*> m_good_muons;


  std::vector<float> m_eta_total_thr1_b; //Threshold 1, barrel
  std::vector<float> m_phi_total_thr1_b;
  std::vector<float> m_pt_total_thr1_b;
  std::vector<float> m_eta_total_thr1_e; //Threshold 1, endcap
  std::vector<float> m_phi_total_thr1_e;
  std::vector<float> m_pt_total_thr1_e;
  std::vector<float> m_eta_total_thr1; //Threshold 1, combined barrel and endcap

  std::vector<float> m_eta_total_thr2_b; //Threshold 2, barrel
  std::vector<float> m_phi_total_thr2_b;
  std::vector<float> m_pt_total_thr2_b;
  std::vector<float> m_eta_total_thr2_e; //Threshold 2, endcap
  std::vector<float> m_phi_total_thr2_e;
  std::vector<float> m_pt_total_thr2_e;
  std::vector<float> m_eta_total_thr2; //Threshold 2, combined barrel and endcap

  std::vector<float> m_eta_total_thr3_b; 
  std::vector<float> m_phi_total_thr3_b;
  std::vector<float> m_pt_total_thr3_b;
  std::vector<float> m_eta_total_thr3_e;
  std::vector<float> m_phi_total_thr3_e;
  std::vector<float> m_pt_total_thr3_e;
  std::vector<float> m_eta_total_thr3; 


  std::vector<float> m_eta_total_thr4_b; 
  std::vector<float> m_phi_total_thr4_b;
  std::vector<float> m_pt_total_thr4_b;
  std::vector<float> m_eta_total_thr4_e;
  std::vector<float> m_phi_total_thr4_e;
  std::vector<float> m_pt_total_thr4_e;
  std::vector<float> m_eta_total_thr4;


  std::vector<float> m_eta_total_thr5_b; 
  std::vector<float> m_phi_total_thr5_b;
  std::vector<float> m_pt_total_thr5_b;
  std::vector<float> m_eta_total_thr5_e;
  std::vector<float> m_phi_total_thr5_e;
  std::vector<float> m_pt_total_thr5_e;
  std::vector<float> m_eta_total_thr5; 


  std::vector<float> m_eta_total_thr6_b; 
  std::vector<float> m_phi_total_thr6_b;
  std::vector<float> m_pt_total_thr6_b;
  std::vector<float> m_eta_total_thr6_e;
  std::vector<float> m_phi_total_thr6_e;
  std::vector<float> m_pt_total_thr6_e;
  std::vector<float> m_eta_total_thr6; 



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


  //Sets up instance of MonitoringObject public to TrigMuonEFTagandProbe
  MonitoringObject m_Thresh_Mon;                                                              
                                
  bool m_debug; //Flag for conditional DEBUG output
  bool m_verbose; //Flag for conditional VEROSE output
  
};
#endif
