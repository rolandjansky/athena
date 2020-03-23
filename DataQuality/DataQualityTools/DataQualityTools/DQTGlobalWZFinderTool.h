/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTGlobalWZFinderTool.h
// PACKAGE:  DataQualityTools
//
// AUTHORS:   Jahred Adelman (jahred.adelman@cern.ch)
//            Simon Viel (svielcern.ch)
//            Koos van Nieuwkoop (jvannieu@cern.ch)
//       
//
// ********************************************************************
#ifndef DQTGlobalWZFinderTool_H
#define DQTGlobalWZFinderTool_H

#include "GaudiKernel/ToolHandle.h"
#include <stdint.h>
#include "DataQualityTools/DataQualityFatherMonTool.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigEgammaMatchingTool/ITrigEgammaMatchingTool.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

#include "TMath.h"
#include "TH3.h"
#include <string>
#include <iostream>

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthParticle.h"

class TProfile;
class TH1F_LW;
class TH2F_LW;
class TH3F_LW;

namespace xAOD{
  class EventInfo_v1; typedef EventInfo_v1 EventInfo; 
  class TruthParticle_v1; typedef TruthParticle_v1 TruthParticle;
}

namespace CP {
  class IMuonSelectionTool;
  class IIsolationSelectionTool;
}

namespace Trig {
  class ITrigMuonMatching;
  class ITrigEgammaMatchingTool;
}

class DQTGlobalWZFinderTool: public DataQualityFatherMonTool
{

 public:
  
  DQTGlobalWZFinderTool(const std::string & type, const std::string & name, const IInterface* parent);

  ~DQTGlobalWZFinderTool();

  StatusCode initialize();
    
  virtual StatusCode bookHistograms();
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms( );
  virtual StatusCode checkHists(bool fromFinalize);

  bool bookDQTGlobalWZFinderTool();

private:
  void setDQTGlobalWZFinderBranches();
  void setBCIDBranches(TTree *tree, std::string variable_name);


  void doMuonTriggerTP(const xAOD::Muon* , const xAOD::Muon*);
  void doMuonTruthEff(std::vector<const xAOD::Muon*>&);
  void doMuonLooseTP(std::vector<const xAOD::Muon*>& goodmuonsZ, const xAOD::Vertex* pVtx);
  void doMuonInDetTP(std::vector<const xAOD::Muon*>& goodmuonsZ, const xAOD::Vertex* pVtx);

  void doEleTriggerTP(const xAOD::Electron* el1, const xAOD::Electron* el2, bool os, bool ss);
  void doEleTP(const xAOD::Electron*, const xAOD::Electron*, const xAOD::Vertex*, const xAOD::EventInfo*, bool);
  void doEleContainerTP(std::vector<const xAOD::Electron*>, std::vector<const xAOD::Electron*>);

  bool goodElectrons(const xAOD::EventInfo*, const xAOD::Electron*, const xAOD::Vertex*, bool); 
  bool antiGoodElectrons(const xAOD::EventInfo* thisEventInfo, const xAOD::Electron* electron_itr, const xAOD::Vertex* pVtx, bool isBad);
  bool kinematicCuts(const xAOD::Electron*);
  void fillEleEffHistos(bool tag_good, bool probe_good, bool probe_anti_good, bool os, double mass);
  
  bool checkTruthElectron(const xAOD::Electron* electron);
  bool checkTruthTrack(const xAOD::TrackParticle* trk);
  bool checkTruthMuon(const xAOD::Muon* muon);

  bool m_isSimulation;
  bool m_writeTTrees;

  // --- Initialise order ---
  TH1F_LW *m_muon_Pt;
  TH1F_LW *m_muon_Eta;
  TH1F_LW *m_ele_Et;
  TH1F_LW *m_ele_Eta;
  std::string m_electronContainerName;
  std::string m_egDetailContainerName;
  std::string m_VxPrimContainerName;
  std::string m_VxContainerName;
  std::string m_METName;
  std::string m_muonContainerName;
  std::string m_jetCollectionName;
  std::string m_tracksName;
  float m_electronEtCut;
  float m_muonPtCut;
  float m_metCut;
  float m_zCutLow;
  float m_zCutHigh;
  float m_muonMaxEta;
  ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;
  ToolHandle<CP::IIsolationSelectionTool> m_isolationSelectionTool;
  ToolHandle<Trig::ITrigMuonMatching> m_muTrigMatchTool;
  bool m_useOwnMuonSelection;
  std::vector<std::string> m_Jpsi_mm_trigger;
  std::vector<std::string> m_Z_mm_trigger;
  std::vector<std::string> m_Z_ee_trigger;
  ToolHandle<Trig::ITrigEgammaMatchingTool> m_elTrigMatchTool;
  ToolHandle<IMCTruthClassifier> m_truthClassifier;
  ToolHandle<Trig::IBunchCrossingTool> m_bcTool;
  // ------
  
  bool m_doTrigger;

  TH1F *m_ZBosonCounter_El_os;
  TH1F *m_ZBosonCounter_El_ss;

  TH1F_LW *m_eltrigtp_matches_os;
  TH1F_LW *m_eltrigtp_matches_ss;
  
  // BCID info
  bool  m_do_BCID;
  float m_pileup_per_lb;
  float m_distance_from_front;
  float m_mass;
  bool  m_is_in_train;
  bool  m_isOS;
  int   m_train_length;
  int   m_nTrig_matches;

  TTree *m_bcid_ele_tight_good;
  TTree *m_bcid_ele_tight_bad;
  TTree *m_bcid_ele_template;
  TTree *m_bcid_ele_trigtp;
  
  TTree *m_bcid_mu_tight_good;
  TTree *m_bcid_mu_tight_bad;
  TTree *m_bcid_mu_trigtp;

  // Electron channel TP histos
  TH1F_LW *m_ele_tight_bad_os;
  TH1F_LW *m_ele_tight_bad_ss;
  TH1F_LW *m_ele_tight_good_os;
  TH1F_LW *m_ele_tight_good_ss;
  TH1F_LW *m_ele_template_os;
  TH1F_LW *m_ele_template_ss;

  //TH1F_LW *m_elContainertp_match;
  TH1F_LW *m_elContainertp_nomatch;
  TH1F_LW *m_ele_tight_passkine;

  TH1F_LW *m_totalSumWeights;
  TH1F_LW *m_fiducialSumWeights_el;
  TH1F_LW *m_fiducialSumWeights_mu;

  TH1F_LW *m_W_mt_ele;
  TH1F_LW *m_W_mt_mu;
  TH2F_LW *m_W_pt_v_met_ele;
  TH2F_LW *m_W_pt_v_met_mu;

  TH1F_LW *m_metHist;
  TH1F_LW *m_metPhiHist;

  TH1F_LW *m_Z_mass_opsele;
  TH1F_LW *m_Z_mass_opsmu;
  TH1F_LW *m_JPsi_mass_opsmu;
  TH1F_LW *m_Upsilon_mass_opsmu;
  TH1F_LW *m_Z_mass_ssele;
  TH1F_LW *m_Z_mass_ssmu;
  TH1F_LW *m_Z_Q_ele;
  TH1F_LW *m_Z_Q_mu;

  TProfile *m_livetime_lb;
  TProfile *m_lblength_lb;
  TProfile *m_mu_lb;
  TProfile *m_Z_ee_trig_ps;
  TProfile *m_Z_mm_trig_ps;
      

  //Resonance Counters
  float m_minLumiBlock;
  float m_maxLumiBlock;
  float m_numBins;

  TH1F_LW *m_JPsiCounter_Mu;
  TH1F_LW *m_UpsilonCounter_Mu;
  TH1F *m_ZBosonCounter_Mu;

  //Trigger T&P
  TH1F_LW *m_mutrigtp_matches;

  //Reco T&P
  TH1F_LW *m_muloosetp_match_os;
  TH1F_LW *m_muloosetp_match_ss;
  TH1F_LW *m_muloosetp_nomatch_os;
  TH1F_LW *m_muloosetp_nomatch_ss;

  // Inner detector T&P
  TH1F_LW *m_mu_InDet_tp_match_os;
  TH1F_LW *m_mu_InDet_tp_match_ss;
  TH1F_LW *m_mu_InDet_tp_nomatch_os;
  TH1F_LW *m_mu_InDet_tp_nomatch_ss;

  // MC truth
  TH1F_LW *m_mcmatch;
      
  //Second component of counter array is for trigger aware counter
  int m_JPsiCounterSBG[2];
  int m_JPsiCounter2BG[2];
  int m_JPsiCounterSignal[2];

  int m_UpsilonCounterSBG[2];
  int m_UpsilonCounterBG[2];
  int m_UpsilonCounterSignal[2];

  int m_ZBosonCounterSBG_El[2];
  int m_ZBosonCounterSBG_Mu[2];


  int m_this_lb; 
  int m_eventNumber; 
  float m_evtWeight;
      
            
  // to guard against endless messages 
  bool m_printedErrorEleContainer;
  bool m_printedErrorMuColl;
  bool m_printedErrorMet;
  bool m_printedErrorTrackContainer;


  // Here we define all the TTree variables
  // Muon TTrees
  TTree *m_muontree;
  bool m_muontree_isTruth;
  float m_muontree_eta1;
  float m_muontree_eta2;
  float m_muontree_phi1;
  float m_muontree_phi2;
  float m_muontree_pT1;
  float m_muontree_pT2;
  float m_muontree_weight;
  float m_muontree_mass;
  int m_muontree_lb;
  int m_muontree_runnumber;
  unsigned long long  m_muontree_eventnumber;

  TTree *m_muon_reco_tptree;
  bool m_muon_reco_tptree_isTruth;
  float m_muon_reco_tptree_pT;
  float m_muon_reco_tptree_eta;
  float m_muon_reco_tptree_phi;
  float m_muon_reco_tptree_mass;
  float m_muon_reco_tptree_weight;
  int m_muon_reco_tptree_mtype;
  int m_muon_reco_tptree_lb;
  int m_muon_reco_tptree_runnumber;
  unsigned long long  m_muon_reco_tptree_eventnumber;

  TTree *m_muon_indet_tptree;
  bool m_muon_indet_tptree_isTruth;
  float m_muon_indet_tptree_pT;
  float m_muon_indet_tptree_eta;
  float m_muon_indet_tptree_phi;
  float m_muon_indet_tptree_mass;
  float m_muon_indet_tptree_weight;
  int m_muon_indet_tptree_mtype;
  int m_muon_indet_tptree_lb;
  int m_muon_indet_tptree_runnumber;
  unsigned long long  m_muon_indet_tptree_eventnumber;

  TTree *m_muon_trig_tptree;
  bool m_muon_trig_tptree_isTruth;
  float m_muon_trig_tptree_pT;
  float m_muon_trig_tptree_eta;
  float m_muon_trig_tptree_phi;
  float m_muon_trig_tptree_mass;
  float m_muon_trig_tptree_weight;
  int m_muon_trig_tptree_mtype;
  int m_muon_trig_tptree_lb;
  int m_muon_trig_tptree_runnumber;
  unsigned long long  m_muon_trig_tptree_eventnumber;

  // Electron TTrees
  TTree *m_electrontree;
  bool m_electrontree_isTruth;
  float m_electrontree_eta1;
  float m_electrontree_eta2;
  float m_electrontree_phi1;
  float m_electrontree_phi2;
  float m_electrontree_pT1;
  float m_electrontree_pT2;
  float m_electrontree_weight;
  float m_electrontree_mass;
  int m_electrontree_lb;
  int m_electrontree_runnumber;
  unsigned long long  m_electrontree_eventnumber;

  TTree *m_electron_reco_tptree;
  bool m_electron_reco_tptree_isTruth;
  float m_electron_reco_tptree_pT;
  float m_electron_reco_tptree_eta;
  float m_electron_reco_tptree_phi;
  float m_electron_reco_tptree_mass;
  float m_electron_reco_tptree_weight;
  int m_electron_reco_tptree_mtype;
  int m_electron_reco_tptree_lb;
  int m_electron_reco_tptree_runnumber;
  unsigned long long  m_electron_reco_tptree_eventnumber;

  TTree *m_electron_container_tptree;
  bool m_electron_container_tptree_isTruth;
  float m_electron_container_tptree_pT;
  float m_electron_container_tptree_eta;
  float m_electron_container_tptree_phi;
  float m_electron_container_tptree_mass;
  float m_electron_container_tptree_weight;
  int m_electron_container_tptree_mtype;
  int m_electron_container_tptree_lb;
  int m_electron_container_tptree_runnumber;
  unsigned long long  m_electron_container_tptree_eventnumber;

  TTree *m_electron_trig_tptree;
  bool m_electron_trig_tptree_isTruth;
  float m_electron_trig_tptree_pT;
  float m_electron_trig_tptree_eta;
  float m_electron_trig_tptree_phi;
  float m_electron_trig_tptree_mass;
  float m_electron_trig_tptree_weight;
  int m_electron_trig_tptree_mtype;
  int m_electron_trig_tptree_lb;
  int m_electron_trig_tptree_runnumber;
  unsigned long long  m_electron_trig_tptree_eventnumber;

};

#endif
