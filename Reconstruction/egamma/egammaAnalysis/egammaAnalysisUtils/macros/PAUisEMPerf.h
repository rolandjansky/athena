/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Feb  9 21:29:01 2010 by ROOT version 5.24/00
// from TChain PAUReco/
//////////////////////////////////////////////////////////

#ifndef PAUisEMPerf_h
#define PAUisEMPerf_h

#include <vector>
#include <string>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

using namespace std;

class PAUisEMPerf {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Run;
   Int_t           Event;
   vector<unsigned int> *ph_L1_matchPass;
   vector<unsigned int> *ph_L2_matchPass;
   vector<unsigned int> *ph_EF_matchPass;
   vector<string>  *TriggersRun;
   vector<int>     *PassL1;
   vector<int>     *PassL2;
   vector<int>     *PassEF;
   Int_t           hard_process_MC;
   Int_t           isHgg_MC;
   Float_t         zHiggs_MC;
   Float_t         mHiggs_MC;
   vector<int>     *HiggsParents;
   vector<int>     *HiggsDaughters;
   vector<float>   *HP_in_eta;
   vector<float>   *HP_in_phi;
   vector<float>   *HP_in_pt;
   vector<float>   *HP_in_e;
   vector<int>     *HP_in_pdg;
   vector<float>   *HP_out_eta;
   vector<float>   *HP_out_phi;
   vector<float>   *HP_out_pt;
   vector<float>   *HP_out_e;
   vector<int>     *HP_out_pdg;
   vector<int>     *HP_out_iRecPhoton;
   vector<float>   *HP_out_deltaR_RecPhoton;
   Float_t         MCweight;
   vector<float>   *PV_ID_zvertex;
   vector<float>   *truth_xvertex;
   vector<float>   *truth_yvertex;
   vector<float>   *truth_zvertex;
   vector<float>   *truth_ph_eta;
   vector<float>   *truth_ph_phi;
   vector<float>   *truth_ph_pt;
   vector<float>   *truth_ph_E;
   vector<float>   *truth_ph_zvertex;
   vector<int>     *truth_ph_isConv;
   vector<float>   *truth_ph_RConv;
   vector<float>   *truth_ph_XConv;
   vector<float>   *truth_ph_YConv;
   vector<float>   *truth_ph_ZConv;
   vector<int>     *truth_ph_mother;
   vector<int>     *truth_ph_isBrem;
   vector<int>     *truth_ph_isFromHardProc;
   vector<int>     *truth_ph_iRecPhoton;
   vector<float>   *truth_ph_deltaR_RecPhoton;
   vector<float>   *truth_jet_eta;
   vector<float>   *truth_jet_phi;
   vector<float>   *truth_jet_pt;
   vector<float>   *truth_jet_E;
   vector<int>     *truth_jet_parton;
   vector<int>     *truth_jet_iRecPhoton;
   vector<float>   *truth_jet_deltaR_RecPhoton;
   vector<float>   *ph_author;
   vector<float>   *ph_eta;
   vector<float>   *ph_etaS0;
   vector<float>   *ph_etaS1;
   vector<float>   *ph_etaS2;
   vector<float>   *ph_phi;
   vector<float>   *ph_pt;
   vector<float>   *ph_Tclus;
   vector<float>   *ph_E;
   vector<float>   *ph_ES0;
   vector<float>   *ph_ES1;
   vector<float>   *ph_ES2;
   vector<float>   *ph_ES3;
   vector<float>   *ph_EcellS0;
   vector<float>   *ph_etacellS0;
   vector<int>     *ph_convFlag;
   vector<int>     *ph_isConv;
   vector<int>     *ph_isRecovered;
   vector<float>   *ph_RConv;
   vector<float>   *ph_ZConv;
   vector<float>   *ph_XConv;
   vector<float>   *ph_YConv;
   vector<float>   *ph_Pt1Conv;
   vector<float>   *ph_Pt2Conv;
   vector<float>   *ph_PtConv;
   vector<float>   *ph_PzConv;
   vector<float>   *ph_ConvIP;
   vector<float>   *ph_ConvIPRev;
   vector<int>     *ph_convTrk1_numBLHits;
   vector<int>     *ph_convTrk1_numPixelHits;
   vector<int>     *ph_convTrk1_numSCTHits;
   vector<int>     *ph_convTrk1_numTRTHits;
   vector<int>     *ph_convTrk2_numBLHits;
   vector<int>     *ph_convTrk2_numPixelHits;
   vector<int>     *ph_convTrk2_numSCTHits;
   vector<int>     *ph_convTrk2_numTRTHits;
   vector<int>     *ph_truth_convTrk1_pdgID;
   vector<int>     *ph_truth_convTrk1_pdgMOTHER;
   vector<int>     *ph_truth_convTrk1_pdgGrandMOTHER;
   vector<int>     *ph_truth_convTrk1_Type;
   vector<int>     *ph_truth_convTrk1_Origin;
   vector<int>     *ph_truth_convTrk1_phtType;
   vector<int>     *ph_truth_convTrk1_phtOrigin;
   vector<int>     *ph_truth_convTrk2_pdgID;
   vector<int>     *ph_truth_convTrk2_pdgMOTHER;
   vector<int>     *ph_truth_convTrk2_pdgGrandMOTHER;
   vector<int>     *ph_truth_convTrk2_Type;
   vector<int>     *ph_truth_convTrk2_Origin;
   vector<int>     *ph_truth_convTrk2_phtType;
   vector<int>     *ph_truth_convTrk2_phtOrigin;
   vector<int>     *ph_truth_Conv;
   vector<int>     *ph_isEM;
   vector<int>     *ph_isPhotonLoose;
   vector<int>     *ph_isPhotonTight;
   vector<int>     *ph_itune1;
   vector<int>     *ph_itune2;
   vector<int>     *ph_itune3;
   vector<float>   *ph_logLikelihood;
   vector<float>   *ph_PtIsolationCone;
   vector<float>   *ph_PtIsolationConePhAngle;
   vector<float>   *ph_trackIsol;
   vector<int>     *ph_selected;
   vector<float>   *ph_shwr_EtHad;
   vector<float>   *ph_shwr_EtHad1;
   vector<float>   *ph_shwr_E277;
   vector<float>   *ph_shwr_E237;
   vector<float>   *ph_shwr_E233;
   vector<float>   *ph_shwr_weta2;
   vector<float>   *ph_shwr_f1;
   vector<float>   *ph_shwr_Emax2;
   vector<float>   *ph_shwr_Emax1;
   vector<float>   *ph_shwr_Emin;
   vector<float>   *ph_shwr_fracm;
   vector<float>   *ph_shwr_wtot;
   vector<float>   *ph_shwr_w1;
   vector<float>   *ph_shwr_EtCone20;
   vector<float>   *ph_shwr_EtCone30;
   vector<float>   *ph_shwr_EtCone40;
   vector<float>   *ph_shwr_EtCone45;
   vector<float>   *ph_shwr_EtRing1030sig2;
   vector<float>   *ph_shwr_EtRing1030sig3;
   vector<float>   *ph_shwr_EtRing1030sig4;
   vector<float>   *ph_topo_EtCone20;
   vector<float>   *ph_topo_EtCone40;
   vector<float>   *ph_topo_EtCone60;
   vector<float>   *ph_topo_eta;
   vector<float>   *ph_topo_phi;
   vector<float>   *ph_topo_E;
   vector<float>   *ph_topo_pt;
   vector<float>   *ph_topoEM_eta;
   vector<float>   *ph_topoEM_phi;
   vector<float>   *ph_topoEM_E;
   vector<float>   *ph_topoEM_pt;
   vector<int>     *ph_imatchRecJet;
   vector<float>   *ph_deltaR_RecJet;
   vector<float>   *ph_L1_e;
   vector<float>   *ph_L1_eta;
   vector<float>   *ph_L1_phi;
   vector<float>   *ph_L1_EMIsol;
   vector<float>   *ph_L1_HadIsol;
   vector<float>   *ph_L1_HadCore;
   vector<float>   *ph_L2_e;
   vector<float>   *ph_L2_et;
   vector<float>   *ph_L2_eta;
   vector<float>   *ph_L2_phi;
   vector<float>   *ph_L2_e237;
   vector<float>   *ph_L2_e277;
   vector<float>   *ph_L2_fracs1;
   vector<float>   *ph_L2_weta2;
   vector<float>   *ph_L2_ehad1;
   vector<float>   *ph_L2_emaxs1;
   vector<float>   *ph_L2_e2tsts1;
   vector<float>   *ph_EF_e;
   vector<float>   *ph_EF_et;
   vector<float>   *ph_EF_eta;
   vector<float>   *ph_EF_phi;
   vector<int>     *ph_EF_IsEM;
   vector<float>   *ph_EF_ethad;
   vector<float>   *ph_EF_ethad1;
   vector<float>   *ph_EF_e233;
   vector<float>   *ph_EF_e237;
   vector<float>   *ph_EF_e277;
   vector<float>   *ph_EF_emaxs1;
   vector<float>   *ph_EF_e2tsts1;
   vector<float>   *ph_EF_fracs1;
   vector<float>   *ph_EF_weta1;
   vector<float>   *ph_EF_weta2;
   vector<float>   *ph_EF_wtots1;
   vector<float>   *ph_EF_f1;
   vector<float>   *ph_EF_emins1;
   vector<int>     *ph_matchMC;
   vector<float>   *ph_eta_MC;
   vector<float>   *ph_phi_MC;
   vector<float>   *ph_pt_MC;
   vector<float>   *ph_E_MC;
   vector<float>   *ph_etaS2_MC;
   vector<float>   *ph_phiS2_MC;
   vector<int>     *ph_isConv_MC;
   vector<float>   *ph_RConv_MC;
   vector<float>   *ph_ZConv_MC;
   vector<float>   *ph_XConv_MC;
   vector<float>   *ph_YConv_MC;
   vector<int>     *ph_PDG;
   vector<int>     *ph_motherPDG;
   vector<int>     *ph_isBrem_MC;
   vector<int>     *ph_isFromHardProc_MC;
   vector<int>     *ph_isPhotonFromHardProc_MC;
   vector<int>     *ph_imatchParton_MC;
   vector<float>   *ph_shwr_EtCone10;
   vector<float>   *ph_shwr_EtCone15;
   vector<float>   *ph_shwr_EtCone25;
   vector<float>   *ph_shwr_EtCone35;
   vector<float>   *ph_EtCone20Iso;
   vector<float>   *ph_EtCone30Iso;
   vector<float>   *ph_EtCone40Iso;
   vector<float>   *ph_cells55_EtCone20;
   vector<float>   *ph_cells55_EtCone30;
   vector<float>   *ph_cells55_EtCone40;
   vector<float>   *ph_cells55_EtCone20Iso;
   vector<float>   *ph_cells55_EtCone30Iso;
   vector<float>   *ph_cells55_EtCone40Iso;
   vector<float>   *ph_cells35_EtCone20;
   vector<float>   *ph_cells35_EtCone30;
   vector<float>   *ph_cells35_EtCone40;
   vector<float>   *ph_cells35_EtCone20Iso;
   vector<float>   *ph_cells35_EtCone30Iso;
   vector<float>   *ph_cells35_EtCone40Iso;
   Int_t           Ncells;
   vector<int>     *cell_layer;
   vector<float>   *cell_energy;
   vector<float>   *cell_pt;
   vector<float>   *cell_eta;
   vector<float>   *cell_phi;
   Int_t           NTracks;
   vector<float>   *trk_eta;
   vector<float>   *trk_phi;
   vector<float>   *trk_pt;
   vector<float>   *trk_x0;
   vector<float>   *trk_y0;
   vector<float>   *trk_z0;
   vector<float>   *trk_nhits_b;
   vector<float>   *trk_nhits_p;
   vector<float>   *trk_nhits_s;
   vector<float>   *trk_nhits_trt;
   vector<float>   *trk_nhits_trth;
   vector<float>   *trk_chiSquared;
   vector<float>   *trk_numberDoF;
   vector<int>     *trk_gConv;
   vector<float>   *jet_eta;
   vector<float>   *jet_phi;
   vector<float>   *jet_pt;
   vector<float>   *jet_E;
   vector<int>     *jet_selected_1j;
   vector<int>     *jet_selected_2j;
   vector<int>     *jet_imatchTrueJet_MC;
   vector<float>   *jet_eta_MC;
   vector<float>   *jet_phi_MC;
   vector<float>   *jet_pt_MC;
   vector<float>   *jet_E_MC;
   vector<float>   *el_author;
   vector<float>   *el_eta;
   vector<float>   *el_phi;
   vector<float>   *el_pt;
   vector<float>   *el_E;
   vector<float>   *el_etaS0;
   vector<float>   *el_etaS1;
   vector<float>   *el_etaS2;
   vector<float>   *el_ES0;
   vector<float>   *el_ES1;
   vector<float>   *el_ES2;
   vector<float>   *el_ES3;
   vector<float>   *el_eta_clus;
   vector<float>   *el_phi_clus;
   vector<float>   *el_pt_clus;
   vector<float>   *el_E_clus;
   vector<float>   *el_Tclus;
   vector<float>   *el_Q;
   vector<int>     *el_isEM;
   vector<int>     *el_isElectronLoose;
   vector<int>     *el_isElectronMedium;
   vector<int>     *el_isElectronTight;
   vector<float>   *el_shwr_EtHad;
   vector<float>   *el_shwr_EtHad1;
   vector<float>   *el_shwr_E277;
   vector<float>   *el_shwr_E237;
   vector<float>   *el_shwr_E233;
   vector<float>   *el_shwr_weta2;
   vector<float>   *el_shwr_f1;
   vector<float>   *el_shwr_Emax2;
   vector<float>   *el_shwr_Emax1;
   vector<float>   *el_shwr_Emin;
   vector<float>   *el_shwr_fracm;
   vector<float>   *el_shwr_wtot;
   vector<float>   *el_shwr_w1;
   vector<int>     *el_itune1;
   vector<int>     *el_itune2;
   vector<int>     *el_itune3;
   vector<float>   *el_logLikelihood;
   vector<float>   *el_PtIsolationCone;
   vector<float>   *el_trackIsol;
   vector<float>   *el_dR_isol;
   vector<int>     *el_selected_etmiss;
   vector<int>     *el_selected_etlep;
   vector<int>     *el_matchMC;
   vector<float>   *el_eta_MC;
   vector<float>   *el_phi_MC;
   vector<float>   *el_pt_MC;
   vector<float>   *el_E_MC;
   vector<int>     *el_PDG;
   vector<int>     *el_motherPDG;
   vector<float>   *mu_eta;
   vector<float>   *mu_phi;
   vector<float>   *mu_pt;
   vector<float>   *mu_E;
   vector<float>   *mu_Q;
   vector<int>     *mu_selected_etmiss;
   vector<int>     *mu_selected_etlep;
   vector<int>     *mu_matchMC;
   vector<float>   *mu_eta_MC;
   vector<float>   *mu_phi_MC;
   vector<float>   *mu_pt_MC;
   vector<float>   *mu_E_MC;
   vector<int>     *mu_PDG;
   vector<int>     *mu_motherPDG;
   Float_t         mEt_ex;
   Float_t         mEt_ey;
   Float_t         mEt_et;
   Float_t         mEt_sumet;
   Float_t         mEt_ex_MC;
   Float_t         mEt_ey_MC;
   Float_t         mEt_et_MC;
   Int_t           NElectrons;
   Int_t           NPhotons;
   Int_t           NGammas;
   Int_t           hgg_iPhotonLead;
   Int_t           hgg_iPhotonSublead;
   vector<int>     *hgg_iGoodPhoton;
   Int_t           hgg_okInclusive;
   Int_t           hgg1jet_iJetLead;
   Int_t           hgg1jet_cutStatus;
   Int_t           hgg1jet_ok;
   Int_t           hgg2jet_iJetLead;
   Int_t           hgg2jet_iJetSublead;
   Int_t           hgg2jet_cutStatus;
   Int_t           hgg2jet_ok;
   Int_t           hggEtmiss_ok;
   Int_t           hggLepton_ok;
   Int_t           hggZWHjj_ok;
   Float_t         hggLepton_DeltaRmujet;
   vector<int>     *hgg_iGoodMuon;
   vector<int>     *hgg_iGoodElectron;
   Float_t         hggZWjj_mjjZW_tracks;
   Float_t         hggZWjj_mjjZW;
   Float_t         hggZWjj_mjjZW4_tracks;
   Float_t         hggZWjj_mjjZW4;
   Float_t         hggZWjj_PTjjZW_tracks;
   Float_t         hggZWjj_PTjjZW4_tracks;
   Float_t         hggZWjj_PTjjZW;
   Float_t         hggZWjj_PTjjZW4;
   Int_t           hggZWjj_ZW7Njet;
   Int_t           hggZWjj_ZW4Njet;
   Float_t         hggZWjj_SumTracksPT;
   Float_t         hggZWjj_SumTracksPT4;
   Float_t         hggZWjj_DeltaRphotons;
   Float_t         hggZWjj_DeltaRjj;
   vector<float>   *hggZWjj_PTjZW4Tracks;
   vector<float>   *hggZWjj_PTjZW4;
   vector<float>   *hggZWjj_PTjZWTracks;
   vector<float>   *hggZWjj_PTjZW;
   vector<float>   *hggZWjj_EtajZW4;
   vector<float>   *hggZWjj_EtajZW;
   vector<float>   *hgg_fit_eta_zcommon;
   vector<float>   *hgg_fit_eta_zcommon_primVtxLH;
   Float_t         hgg_fit_zcommon;
   Float_t         hgg_fit_zcommon_primVtxLH;
   Float_t         hgg_fit_sigma_zcommon;
   Float_t         hgg_fit_sigma_zcommon_primVtxLH;
   Float_t         hgg_fit_mgg;
   Float_t         hgg_fit_mgg_trueDir;
   Float_t         hgg_fit_mgg_trueE;
   Int_t           hgg_fit_fitFlag;
   Float_t         hgg_fit_vtxLikelihood;
   vector<float>   *hgg_fit_mgg_vec;
   Float_t         hgg_fit_ptgg;
   Float_t         hgg_fit_detagg;
   Float_t         hgg_fit_cosThetaStar;
   Float_t         hgg1jet_mgg;
   Float_t         hgg1jet_mggj;
   Float_t         hgg2jet_mgg;
   Float_t         hgg2jet_mjj;
   vector<float>   *ph_HPV_eta;
   vector<float>   *ph_HPV_sigma_eta;
   vector<float>   *hgg_HPV_zcommon;
   vector<float>   *hgg_HPV_zcommon_primVtxLH;
   vector<float>   *hgg_HPV_mgg;
   vector<float>   *hgg_HPV_mgg_primVtxLH;
   Float_t         hgg_HPV_ptgg;
   Float_t         hgg_HPV_detagg;
   Float_t         hgg_HPV_dphigg;
   Float_t         hgg_HPV_cosThetaStar;
   Float_t         hgg_HPV_cosThetaStarCS;
   vector<float>   *hgg_HPV_eta_zcommon;
   vector<float>   *hgg_HPV_eta_zcommon_primVtxLH;
   Float_t         hgg_HPV_vtxLikelihood;
   vector<float>   *hgg_HPV_mgg_Ztruth;
   vector<float>   *ph_HPV_convTrk1_zvertex;
   vector<float>   *ph_HPV_convTrk2_zvertex;
   vector<float>   *ph_HPV_convTrk1_sigma_zvertex;
   vector<float>   *ph_HPV_convTrk2_sigma_zvertex;
   vector<float>   *ph_HPV_zvertex;
   vector<float>   *ph_HPV_sigma_zvertex;
   vector<float>   *ph_HPV_zvertex_neutral;
   vector<float>   *ph_HPV_zvertex_charged;
   vector<float>   *ph_HPV_sigma_zvertex_neutral;
   vector<float>   *ph_HPV_sigma_zvertex_charged;
   vector<float>   *ph_E_clus;
   vector<float>   *ph_eta_clus;
   vector<float>   *ph_phi_clus;
   vector<float>   *ph_pt_clus;
   vector<float>   *testOleg_ph_particleType;
   vector<float>   *testOleg_ph_particleOrig;
   vector<float>   *clus_SW_eta;
   vector<float>   *clus_SW_phi;
   vector<float>   *clus_SW_E;
   vector<int>     *clus_SW_size;
   vector<int>     *clus_SW_etasize;
   vector<int>     *clus_SW_phisize;
   vector<float>   *clus_SW_time;
   vector<float>   *clus_EMtopo_eta;
   vector<float>   *clus_EMtopo_phi;
   vector<float>   *clus_EMtopo_E;
   vector<float>   *clus_EMtopo_time;
   vector<float>   *clus_EMtopo_trackIsol05;
   vector<float>   *clus_EMtopo_trackIsol07;
   vector<float>   *clus_EMtopo_trackIsol10;
   vector<float>   *clus_EMtopo_trackIsol20;
   vector<float>   *clus_EMtopo_CENTER_LAMBDA;
   vector<float>   *clus_EMtopo_LATERAL;
   vector<float>   *clus_EMtopo_LONGITUDINAL;
   vector<float>   *clus_EMtopo_ENG_FRAC_EM;
   vector<float>   *clus_EMtopo_ENG_FRAC_MAX;
   vector<float>   *clus_EMtopo_ENG_FRAC_CORE;
   vector<float>   *clus_EMtopo_FIRST_ENG_DENS;
   vector<float>   *clus_EMtopo_SECOND_R;
   vector<float>   *clus_EMtopo_SECOND_LAMBDA;
   vector<float>   *clus_EMtopo_SECOND_ENG_DENS;
   vector<float>   *clus_EMtopo_ISOLATION;
   vector<float>   *clus_EMtopo_EM_PROBABILITY;
   vector<float>   *clus_EMtopo_HAD_WEIGHT;
   vector<float>   *clus_EMtopo_OOC_WEIGHT;

   // List of branches
   TBranch        *b_Run;   //!
   TBranch        *b_Event;   //!
   TBranch        *b_ph_L1_matchPass;   //!
   TBranch        *b_ph_L2_matchPass;   //!
   TBranch        *b_ph_EF_matchPass;   //!
   TBranch        *b_TriggersRun;   //!
   TBranch        *b_PassL1;   //!
   TBranch        *b_PassL2;   //!
   TBranch        *b_PassEF;   //!
   TBranch        *b_hard_process_MC;   //!
   TBranch        *b_isHgg_MC;   //!
   TBranch        *b_zHiggs_MC;   //!
   TBranch        *b_mHiggs_MC;   //!
   TBranch        *b_HiggsParents;   //!
   TBranch        *b_HiggsDaughters;   //!
   TBranch        *b_HP_in_eta;   //!
   TBranch        *b_HP_in_phi;   //!
   TBranch        *b_HP_in_pt;   //!
   TBranch        *b_HP_in_e;   //!
   TBranch        *b_HP_in_pdg;   //!
   TBranch        *b_HP_out_eta;   //!
   TBranch        *b_HP_out_phi;   //!
   TBranch        *b_HP_out_pt;   //!
   TBranch        *b_HP_out_e;   //!
   TBranch        *b_HP_out_pdg;   //!
   TBranch        *b_HP_out_iRecPhoton;   //!
   TBranch        *b_HP_out_deltaR_RecPhoton;   //!
   TBranch        *b_MCweight;   //!
   TBranch        *b_PV_ID_zvertex;   //!
   TBranch        *b_truth_xvertex;   //!
   TBranch        *b_truth_yvertex;   //!
   TBranch        *b_truth_zvertex;   //!
   TBranch        *b_truth_ph_eta;   //!
   TBranch        *b_truth_ph_phi;   //!
   TBranch        *b_truth_ph_pt;   //!
   TBranch        *b_truth_ph_E;   //!
   TBranch        *b_truth_ph_zvertex;   //!
   TBranch        *b_truth_ph_isConv;   //!
   TBranch        *b_truth_ph_RConv;   //!
   TBranch        *b_truth_ph_XConv;   //!
   TBranch        *b_truth_ph_YConv;   //!
   TBranch        *b_truth_ph_ZConv;   //!
   TBranch        *b_truth_ph_mother;   //!
   TBranch        *b_truth_ph_isBrem;   //!
   TBranch        *b_truth_ph_isFromHardProc;   //!
   TBranch        *b_truth_ph_iRecPhoton;   //!
   TBranch        *b_truth_ph_deltaR_RecPhoton;   //!
   TBranch        *b_truth_jet_eta;   //!
   TBranch        *b_truth_jet_phi;   //!
   TBranch        *b_truth_jet_pt;   //!
   TBranch        *b_truth_jet_E;   //!
   TBranch        *b_truth_jet_parton;   //!
   TBranch        *b_truth_jet_iRecPhoton;   //!
   TBranch        *b_truth_jet_deltaR_RecPhoton;   //!
   TBranch        *b_ph_author;   //!
   TBranch        *b_ph_eta;   //!
   TBranch        *b_ph_etaS0;   //!
   TBranch        *b_ph_etaS1;   //!
   TBranch        *b_ph_etaS2;   //!
   TBranch        *b_ph_phi;   //!
   TBranch        *b_ph_pt;   //!
   TBranch        *b_ph_Tclus;   //!
   TBranch        *b_ph_E;   //!
   TBranch        *b_ph_ES0;   //!
   TBranch        *b_ph_ES1;   //!
   TBranch        *b_ph_ES2;   //!
   TBranch        *b_ph_ES3;   //!
   TBranch        *b_ph_EcellS0;   //!
   TBranch        *b_ph_etacellS0;   //!
   TBranch        *b_ph_convFlag;   //!
   TBranch        *b_ph_isConv;   //!
   TBranch        *b_ph_isRecovered;   //!
   TBranch        *b_ph_RConv;   //!
   TBranch        *b_ph_ZConv;   //!
   TBranch        *b_ph_XConv;   //!
   TBranch        *b_ph_YConv;   //!
   TBranch        *b_ph_Pt1Conv;   //!
   TBranch        *b_ph_Pt2Conv;   //!
   TBranch        *b_ph_PtConv;   //!
   TBranch        *b_ph_PzConv;   //!
   TBranch        *b_ph_ConvIP;   //!
   TBranch        *b_ph_ConvIPRev;   //!
   TBranch        *b_ph_convTrk1_numBLHits;   //!
   TBranch        *b_ph_convTrk1_numPixelHits;   //!
   TBranch        *b_ph_convTrk1_numSCTHits;   //!
   TBranch        *b_ph_convTrk1_numTRTHits;   //!
   TBranch        *b_ph_convTrk2_numBLHits;   //!
   TBranch        *b_ph_convTrk2_numPixelHits;   //!
   TBranch        *b_ph_convTrk2_numSCTHits;   //!
   TBranch        *b_ph_convTrk2_numTRTHits;   //!
   TBranch        *b_ph_truth_convTrk1_pdgID;   //!
   TBranch        *b_ph_truth_convTrk1_pdgMOTHER;   //!
   TBranch        *b_ph_truth_convTrk1_pdgGrandMOTHER;   //!
   TBranch        *b_ph_truth_convTrk1_Type;   //!
   TBranch        *b_ph_truth_convTrk1_Origin;   //!
   TBranch        *b_ph_truth_convTrk1_phtType;   //!
   TBranch        *b_ph_truth_convTrk1_phtOrigin;   //!
   TBranch        *b_ph_truth_convTrk2_pdgID;   //!
   TBranch        *b_ph_truth_convTrk2_pdgMOTHER;   //!
   TBranch        *b_ph_truth_convTrk2_pdgGrandMOTHER;   //!
   TBranch        *b_ph_truth_convTrk2_Type;   //!
   TBranch        *b_ph_truth_convTrk2_Origin;   //!
   TBranch        *b_ph_truth_convTrk2_phtType;   //!
   TBranch        *b_ph_truth_convTrk2_phtOrigin;   //!
   TBranch        *b_ph_truth_Conv;   //!
   TBranch        *b_ph_isEM;   //!
   TBranch        *b_ph_isPhotonLoose;   //!
   TBranch        *b_ph_isPhotonTight;   //!
   TBranch        *b_ph_itune1;   //!
   TBranch        *b_ph_itune2;   //!
   TBranch        *b_ph_itune3;   //!
   TBranch        *b_ph_logLikelihood;   //!
   TBranch        *b_ph_PtIsolationCone;   //!
   TBranch        *b_ph_PtIsolationConePhAngle;   //!
   TBranch        *b_ph_trackIsol;   //!
   TBranch        *b_ph_selected;   //!
   TBranch        *b_ph_shwr_EtHad;   //!
   TBranch        *b_ph_shwr_EtHad1;   //!
   TBranch        *b_ph_shwr_E277;   //!
   TBranch        *b_ph_shwr_E237;   //!
   TBranch        *b_ph_shwr_E233;   //!
   TBranch        *b_ph_shwr_weta2;   //!
   TBranch        *b_ph_shwr_f1;   //!
   TBranch        *b_ph_shwr_Emax2;   //!
   TBranch        *b_ph_shwr_Emax1;   //!
   TBranch        *b_ph_shwr_Emin;   //!
   TBranch        *b_ph_shwr_fracm;   //!
   TBranch        *b_ph_shwr_wtot;   //!
   TBranch        *b_ph_shwr_w1;   //!
   TBranch        *b_ph_shwr_EtCone20;   //!
   TBranch        *b_ph_shwr_EtCone30;   //!
   TBranch        *b_ph_shwr_EtCone40;   //!
   TBranch        *b_ph_shwr_EtCone45;   //!
   TBranch        *b_ph_shwr_EtRing1030sig2;   //!
   TBranch        *b_ph_shwr_EtRing1030sig3;   //!
   TBranch        *b_ph_shwr_EtRing1030sig4;   //!
   TBranch        *b_ph_topo_EtCone20;   //!
   TBranch        *b_ph_topo_EtCone40;   //!
   TBranch        *b_ph_topo_EtCone60;   //!
   TBranch        *b_ph_topo_eta;   //!
   TBranch        *b_ph_topo_phi;   //!
   TBranch        *b_ph_topo_E;   //!
   TBranch        *b_ph_topo_pt;   //!
   TBranch        *b_ph_topoEM_eta;   //!
   TBranch        *b_ph_topoEM_phi;   //!
   TBranch        *b_ph_topoEM_E;   //!
   TBranch        *b_ph_topoEM_pt;   //!
   TBranch        *b_ph_imatchRecJet;   //!
   TBranch        *b_ph_deltaR_RecJet;   //!
   TBranch        *b_ph_L1_e;   //!
   TBranch        *b_ph_L1_eta;   //!
   TBranch        *b_ph_L1_phi;   //!
   TBranch        *b_ph_L1_EMIsol;   //!
   TBranch        *b_ph_L1_HadIsol;   //!
   TBranch        *b_ph_L1_HadCore;   //!
   TBranch        *b_ph_L2_e;   //!
   TBranch        *b_ph_L2_et;   //!
   TBranch        *b_ph_L2_eta;   //!
   TBranch        *b_ph_L2_phi;   //!
   TBranch        *b_ph_L2_e237;   //!
   TBranch        *b_ph_L2_e277;   //!
   TBranch        *b_ph_L2_fracs1;   //!
   TBranch        *b_ph_L2_weta2;   //!
   TBranch        *b_ph_L2_ehad1;   //!
   TBranch        *b_ph_L2_emaxs1;   //!
   TBranch        *b_ph_L2_e2tsts1;   //!
   TBranch        *b_ph_EF_e;   //!
   TBranch        *b_ph_EF_et;   //!
   TBranch        *b_ph_EF_eta;   //!
   TBranch        *b_ph_EF_phi;   //!
   TBranch        *b_ph_EF_IsEM;   //!
   TBranch        *b_ph_EF_ethad;   //!
   TBranch        *b_ph_EF_ethad1;   //!
   TBranch        *b_ph_EF_e233;   //!
   TBranch        *b_ph_EF_e237;   //!
   TBranch        *b_ph_EF_e277;   //!
   TBranch        *b_ph_EF_emaxs1;   //!
   TBranch        *b_ph_EF_e2tsts1;   //!
   TBranch        *b_ph_EF_fracs1;   //!
   TBranch        *b_ph_EF_weta1;   //!
   TBranch        *b_ph_EF_weta2;   //!
   TBranch        *b_ph_EF_wtots1;   //!
   TBranch        *b_ph_EF_f1;   //!
   TBranch        *b_ph_EF_emins1;   //!
   TBranch        *b_ph_matchMC;   //!
   TBranch        *b_ph_eta_MC;   //!
   TBranch        *b_ph_phi_MC;   //!
   TBranch        *b_ph_pt_MC;   //!
   TBranch        *b_ph_E_MC;   //!
   TBranch        *b_ph_etaS2_MC;   //!
   TBranch        *b_ph_phiS2_MC;   //!
   TBranch        *b_ph_isConv_MC;   //!
   TBranch        *b_ph_RConv_MC;   //!
   TBranch        *b_ph_ZConv_MC;   //!
   TBranch        *b_ph_XConv_MC;   //!
   TBranch        *b_ph_YConv_MC;   //!
   TBranch        *b_ph_PDG;   //!
   TBranch        *b_ph_motherPDG;   //!
   TBranch        *b_ph_isBrem_MC;   //!
   TBranch        *b_ph_isFromHardProc_MC;   //!
   TBranch        *b_ph_isPhotonFromHardProc_MC;   //!
   TBranch        *b_ph_imatchParton_MC;   //!
   TBranch        *b_ph_shwr_EtCone10;   //!
   TBranch        *b_ph_shwr_EtCone15;   //!
   TBranch        *b_ph_shwr_EtCone25;   //!
   TBranch        *b_ph_shwr_EtCone35;   //!
   TBranch        *b_ph_EtCone20Iso;   //!
   TBranch        *b_ph_EtCone30Iso;   //!
   TBranch        *b_ph_EtCone40Iso;   //!
   TBranch        *b_ph_cells55_EtCone20;   //!
   TBranch        *b_ph_cells55_EtCone30;   //!
   TBranch        *b_ph_cells55_EtCone40;   //!
   TBranch        *b_ph_cells55_EtCone20Iso;   //!
   TBranch        *b_ph_cells55_EtCone30Iso;   //!
   TBranch        *b_ph_cells55_EtCone40Iso;   //!
   TBranch        *b_ph_cells35_EtCone20;   //!
   TBranch        *b_ph_cells35_EtCone30;   //!
   TBranch        *b_ph_cells35_EtCone40;   //!
   TBranch        *b_ph_cells35_EtCone20Iso;   //!
   TBranch        *b_ph_cells35_EtCone30Iso;   //!
   TBranch        *b_ph_cells35_EtCone40Iso;   //!
   TBranch        *b_Ncells;   //!
   TBranch        *b_cell_layer;   //!
   TBranch        *b_cell_energy;   //!
   TBranch        *b_cell_pt;   //!
   TBranch        *b_cell_eta;   //!
   TBranch        *b_cell_phi;   //!
   TBranch        *b_NTracks;   //!
   TBranch        *b_trk_eta;   //!
   TBranch        *b_trk_phi;   //!
   TBranch        *b_trk_pt;   //!
   TBranch        *b_trk_x0;   //!
   TBranch        *b_trk_y0;   //!
   TBranch        *b_trk_z0;   //!
   TBranch        *b_trk_nhits_b;   //!
   TBranch        *b_trk_nhits_p;   //!
   TBranch        *b_trk_nhits_s;   //!
   TBranch        *b_trk_nhits_trt;   //!
   TBranch        *b_trk_nhits_trth;   //!
   TBranch        *b_trk_chiSquared;   //!
   TBranch        *b_trk_numberDoF;   //!
   TBranch        *b_trk_gConv;   //!
   TBranch        *b_jet_eta;   //!
   TBranch        *b_jet_phi;   //!
   TBranch        *b_jet_pt;   //!
   TBranch        *b_jet_E;   //!
   TBranch        *b_jet_selected_1j;   //!
   TBranch        *b_jet_selected_2j;   //!
   TBranch        *b_jet_imatchTrueJet_MC;   //!
   TBranch        *b_jet_eta_MC;   //!
   TBranch        *b_jet_phi_MC;   //!
   TBranch        *b_jet_pt_MC;   //!
   TBranch        *b_jet_E_MC;   //!
   TBranch        *b_el_author;   //!
   TBranch        *b_el_eta;   //!
   TBranch        *b_el_phi;   //!
   TBranch        *b_el_pt;   //!
   TBranch        *b_el_E;   //!
   TBranch        *b_el_etaS0;   //!
   TBranch        *b_el_etaS1;   //!
   TBranch        *b_el_etaS2;   //!
   TBranch        *b_el_ES0;   //!
   TBranch        *b_el_ES1;   //!
   TBranch        *b_el_ES2;   //!
   TBranch        *b_el_ES3;   //!
   TBranch        *b_el_eta_clus;   //!
   TBranch        *b_el_phi_clus;   //!
   TBranch        *b_el_pt_clus;   //!
   TBranch        *b_el_E_clus;   //!
   TBranch        *b_el_Tclus;   //!
   TBranch        *b_el_Q;   //!
   TBranch        *b_el_isEM;   //!
   TBranch        *b_el_isElectronLoose;   //!
   TBranch        *b_el_isElectronMedium;   //!
   TBranch        *b_el_isElectronTight;   //!
   TBranch        *b_el_shwr_EtHad;   //!
   TBranch        *b_el_shwr_EtHad1;   //!
   TBranch        *b_el_shwr_E277;   //!
   TBranch        *b_el_shwr_E237;   //!
   TBranch        *b_el_shwr_E233;   //!
   TBranch        *b_el_shwr_weta2;   //!
   TBranch        *b_el_shwr_f1;   //!
   TBranch        *b_el_shwr_Emax2;   //!
   TBranch        *b_el_shwr_Emax1;   //!
   TBranch        *b_el_shwr_Emin;   //!
   TBranch        *b_el_shwr_fracm;   //!
   TBranch        *b_el_shwr_wtot;   //!
   TBranch        *b_el_shwr_w1;   //!
   TBranch        *b_el_itune1;   //!
   TBranch        *b_el_itune2;   //!
   TBranch        *b_el_itune3;   //!
   TBranch        *b_el_logLikelihood;   //!
   TBranch        *b_el_PtIsolationCone;   //!
   TBranch        *b_el_trackIsol;   //!
   TBranch        *b_el_dR_isol;   //!
   TBranch        *b_el_selected_etmiss;   //!
   TBranch        *b_el_selected_etlep;   //!
   TBranch        *b_el_matchMC;   //!
   TBranch        *b_el_eta_MC;   //!
   TBranch        *b_el_phi_MC;   //!
   TBranch        *b_el_pt_MC;   //!
   TBranch        *b_el_E_MC;   //!
   TBranch        *b_el_PDG;   //!
   TBranch        *b_el_motherPDG;   //!
   TBranch        *b_mu_eta;   //!
   TBranch        *b_mu_phi;   //!
   TBranch        *b_mu_pt;   //!
   TBranch        *b_mu_E;   //!
   TBranch        *b_mu_Q;   //!
   TBranch        *b_mu_selected_etmiss;   //!
   TBranch        *b_mu_selected_etlep;   //!
   TBranch        *b_mu_matchMC;   //!
   TBranch        *b_mu_eta_MC;   //!
   TBranch        *b_mu_phi_MC;   //!
   TBranch        *b_mu_pt_MC;   //!
   TBranch        *b_mu_E_MC;   //!
   TBranch        *b_mu_PDG;   //!
   TBranch        *b_mu_motherPDG;   //!
   TBranch        *b_mEt_ex;   //!
   TBranch        *b_mEt_ey;   //!
   TBranch        *b_mEt_et;   //!
   TBranch        *b_mEt_sumet;   //!
   TBranch        *b_mEt_ex_MC;   //!
   TBranch        *b_mEt_ey_MC;   //!
   TBranch        *b_mEt_et_MC;   //!
   TBranch        *b_NElectrons;   //!
   TBranch        *b_NPhotons;   //!
   TBranch        *b_NGammas;   //!
   TBranch        *b_hgg_iPhotonLead;   //!
   TBranch        *b_hgg_iPhotonSublead;   //!
   TBranch        *b_hgg_iGoodPhoton;   //!
   TBranch        *b_hgg_okInclusive;   //!
   TBranch        *b_hgg1jet_iJetLead;   //!
   TBranch        *b_hgg1jet_cutStatus;   //!
   TBranch        *b_hgg1jet_ok;   //!
   TBranch        *b_hgg2jet_iJetLead;   //!
   TBranch        *b_hgg2jet_iJetSublead;   //!
   TBranch        *b_hgg2jet_cutStatus;   //!
   TBranch        *b_hgg2jet_ok;   //!
   TBranch        *b_hggEtmiss_ok;   //!
   TBranch        *b_hggLepton_ok;   //!
   TBranch        *b_hggZWHjj_ok;   //!
   TBranch        *b_DeltaRmujet;   //!
   TBranch        *b_hgg_iGoodMuon;   //!
   TBranch        *b_hgg_iGoodElectron;   //!
   TBranch        *b_mjjZW_tracks;   //!
   TBranch        *b_mjjZW;   //!
   TBranch        *b_mjjZW4_tracks;   //!
   TBranch        *b_mjjZW4;   //!
   TBranch        *b_PTjjZW_tracks;   //!
   TBranch        *b_PTjjZW4_tracks;   //!
   TBranch        *b_PTjjZW;   //!
   TBranch        *b_PTjjZW4;   //!
   TBranch        *b_ZW7Njet;   //!
   TBranch        *b_ZW4Njet;   //!
   TBranch        *b_SumTracksPT;   //!
   TBranch        *b_SumTracksPT4;   //!
   TBranch        *b_DeltaRphotons;   //!
   TBranch        *b_DeltaRjj;   //!
   TBranch        *b_hggZWjj_PTjZW4Tracks;   //!
   TBranch        *b_hggZWjj_PTjZW4;   //!
   TBranch        *b_hggZWjj_PTjZWTracks;   //!
   TBranch        *b_hggZWjj_PTjZW;   //!
   TBranch        *b_hggZWjj_EtajZW4;   //!
   TBranch        *b_hggZWjj_EtajZW;   //!
   TBranch        *b_hgg_fit_eta_zcommon;   //!
   TBranch        *b_hgg_fit_eta_zcommon_primVtxLH;   //!
   TBranch        *b_hgg_fit_zCalo;   //!
   TBranch        *b_hgg_fit_zVtx;   //!
   TBranch        *b_hgg_fit_zCaloRes;   //!
   TBranch        *b_hgg_fit_zVtxRes;   //!
   TBranch        *b_hgg_fit_mgg;   //!
   TBranch        *b_hgg_fit_mgg_trueDir;   //!
   TBranch        *b_hgg_fit_mgg_trueE;   //!
   TBranch        *b_hgg_fit_fitFlag;   //!
   TBranch        *b_hgg_fit_vtxLikelihood;   //!
   TBranch        *b_hgg_fit_mgg_vec;   //!
   TBranch        *b_hgg_fit_ptgg;   //!
   TBranch        *b_hgg_fit_detagg;   //!
   TBranch        *b_hgg_fit_cosThetaStar;   //!
   TBranch        *b_hgg1jet_mgg;   //!
   TBranch        *b_hgg1jet_mggj;   //!
   TBranch        *b_hgg2jet_mgg;   //!
   TBranch        *b_hgg2jet_mjj;   //!
   TBranch        *b_ph_HPV_eta;   //!
   TBranch        *b_ph_HPV_sigma_eta;   //!
   TBranch        *b_hgg_HPV_zcommon;   //!
   TBranch        *b_hgg_HPV_zcommon_primVtxLH;   //!
   TBranch        *b_hgg_HPV_mgg;   //!
   TBranch        *b_hgg_HPV_mgg_primVtxLH;   //!
   TBranch        *b_hgg_HPV_ptgg;   //!
   TBranch        *b_hgg_HPV_detagg;   //!
   TBranch        *b_hgg_HPV_dphigg;   //!
   TBranch        *b_hgg_HPV_cosThetaStar;   //!
   TBranch        *b_hgg_HPV_cosThetaStarCS;   //!
   TBranch        *b_hgg_HPV_eta_zcommon;   //!
   TBranch        *b_hgg_HPV_eta_zcommon_primVtxLH;   //!
   TBranch        *b_hgg_HPV_vtxLikelihood;   //!
   TBranch        *b_hgg_HPV_mgg_Ztruth;   //!
   TBranch        *b_ph_HPV_convTrk1_zvertex;   //!
   TBranch        *b_ph_HPV_convTrk2_zvertex;   //!
   TBranch        *b_ph_HPV_convTrk1_sigma_zvertex;   //!
   TBranch        *b_ph_HPV_convTrk2_sigma_zvertex;   //!
   TBranch        *b_ph_HPV_zvertex;   //!
   TBranch        *b_ph_HPV_sigma_zvertex;   //!
   TBranch        *b_ph_HPV_zvertex_neutral;   //!
   TBranch        *b_ph_HPV_zvertex_charged;   //!
   TBranch        *b_ph_HPV_sigma_zvertex_neutral;   //!
   TBranch        *b_ph_HPV_sigma_zvertex_charged;   //!
   TBranch        *b_ph_E_clus;   //!
   TBranch        *b_ph_eta_clus;   //!
   TBranch        *b_ph_phi_clus;   //!
   TBranch        *b_ph_pt_clus;   //!
   TBranch        *b_testOleg_ph_particleType;   //!
   TBranch        *b_testOleg_ph_particleOrig;   //!
   TBranch        *b_clus_SW_eta;   //!
   TBranch        *b_clus_SW_phi;   //!
   TBranch        *b_clus_SW_E;   //!
   TBranch        *b_clus_SW_size;   //!
   TBranch        *b_clus_SW_etasize;   //!
   TBranch        *b_clus_SW_phisize;   //!
   TBranch        *b_clus_SW_time;   //!
   TBranch        *b_clus_EMtopo_eta;   //!
   TBranch        *b_clus_EMtopo_phi;   //!
   TBranch        *b_clus_EMtopo_E;   //!
   TBranch        *b_clus_EMtopo_time;   //!
   TBranch        *b_clus_EMtopo_trackIsol05;   //!
   TBranch        *b_clus_EMtopo_trackIsol07;   //!
   TBranch        *b_clus_EMtopo_trackIsol10;   //!
   TBranch        *b_clus_EMtopo_trackIsol20;   //!
   TBranch        *b_clus_EMtopo_CENTER_LAMBDA;   //!
   TBranch        *b_clus_EMtopo_LATERAL;   //!
   TBranch        *b_clus_EMtopo_LONGITUDINAL;   //!
   TBranch        *b_clus_EMtopo_ENG_FRAC_EM;   //!
   TBranch        *b_clus_EMtopo_ENG_FRAC_MAX;   //!
   TBranch        *b_clus_EMtopo_ENG_FRAC_CORE;   //!
   TBranch        *b_clus_EMtopo_FIRST_ENG_DENS;   //!
   TBranch        *b_clus_EMtopo_SECOND_R;   //!
   TBranch        *b_clus_EMtopo_SECOND_LAMBDA;   //!
   TBranch        *b_clus_EMtopo_SECOND_ENG_DENS;   //!
   TBranch        *b_clus_EMtopo_ISOLATION;   //!
   TBranch        *b_clus_EMtopo_EM_PROBABILITY;   //!
   TBranch        *b_clus_EMtopo_HAD_WEIGHT;   //!
   TBranch        *b_clus_EMtopo_OOC_WEIGHT;   //!

   PAUisEMPerf(TTree *tree=0);
   virtual ~PAUisEMPerf();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef PAUisEMPerf_cxx
PAUisEMPerf::PAUisEMPerf(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {

#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Memory Directory");
      if (!f) {
         f = new TFile("Memory Directory");
         f->cd("Rint:/");
      }
      tree = (TTree*)gDirectory->Get("PAUReco");

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("PAUReco","");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00001.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00002.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00003.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00004.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00005.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00006.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00007.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00008.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00009.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00010.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00011.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00012.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00013.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00014.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00015.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00016.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00017.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00018.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00019.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00020.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00021.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00022.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00023.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00024.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00025.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00026.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00027.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00028.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00029.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00030.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00031.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00032.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00033.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00034.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00035.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00036.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00037.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00038.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00039.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00040.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00041.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00042.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00043.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00044.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00045.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00046.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00047.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00048.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00049.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00050.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00051.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00052.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00053.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00054.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00055.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00056.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00057.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00058.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00059.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00060.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00061.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00062.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00063.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00064.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00065.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00066.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00067.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00068.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00069.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00070.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00071.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00072.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00073.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00074.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00075.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00076.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00077.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00078.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00079.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00080.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00081.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00082.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00083.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00084.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00085.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00086.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00087.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00088.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00089.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00090.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00091.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00092.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00093.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00094.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00095.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00096.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00097.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00098.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00099.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00100.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00101.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00102.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00103.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00104.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00105.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00106.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00107.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00108.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00109.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00110.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00111.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00112.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00113.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00114.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00115.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00116.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00117.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00118.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00119.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00120.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00121.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00122.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00123.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00124.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00125.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00126.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00127.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00128.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00129.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00130.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00131.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00132.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00133.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00134.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00135.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00136.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00137.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00138.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00139.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00140.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00141.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00142.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00143.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00144.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00145.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00146.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00147.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00148.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00149.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00150.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00151.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00152.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00153.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00154.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00155.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00156.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00157.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00158.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00159.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00160.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00161.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00162.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00163.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00164.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00165.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00166.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00167.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00168.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00169.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00170.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00171.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00172.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00173.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00174.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00175.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00176.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00177.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00178.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00179.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00180.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00181.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00182.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00183.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00184.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00185.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00186.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00187.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00188.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00189.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00190.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00191.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00192.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00193.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00194.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00195.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00196.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00197.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00198.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00199.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00200.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00201.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00202.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00203.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00204.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00205.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00206.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00207.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00208.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00209.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00210.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00211.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00212.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00213.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00214.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00215.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00216.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00217.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00218.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00219.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00220.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00221.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00222.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00223.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00224.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00226.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00227.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00228.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00229.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00230.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00231.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00232.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00233.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00234.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00235.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00236.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00237.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00238.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00239.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00240.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00241.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00242.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00243.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00244.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00245.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00246.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00247.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00248.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00249.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00250.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00251.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00252.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00253.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00254.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00255.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00256.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00257.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00258.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00259.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00260.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00261.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00262.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00263.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00264.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00265.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00266.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00267.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00268.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00269.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00270.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00271.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00272.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00273.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00274.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00275.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00276.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00277.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00278.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00279.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00280.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00281.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00282.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00283.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00284.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00285.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00286.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00287.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00288.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00289.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00290.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00291.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00292.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00293.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00294.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00295.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00296.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00297.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00298.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00299.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00300.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00301.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00302.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00303.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00304.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00305.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00306.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00307.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00308.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00309.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00310.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00311.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00312.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00313.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00314.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00315.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00316.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00317.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00318.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00319.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00320.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00321.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00322.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00323.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00324.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00325.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00326.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00327.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00328.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00330.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00331.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00332.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00333.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00334.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00335.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00336.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00337.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00338.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00339.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00340.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00341.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00342.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00343.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00344.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00345.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00346.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00347.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00348.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00349.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00350.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00351.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00352.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00353.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00354.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00355.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00356.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00357.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00358.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00359.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00360.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00361.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00362.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00363.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00364.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00365.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00366.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00367.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00368.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00369.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00370.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00371.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00372.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00373.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00374.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00375.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00376.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00377.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00378.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00379.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00380.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00381.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00382.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00383.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00384.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00385.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00386.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00387.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00388.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00389.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00390.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00391.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00392.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00393.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00394.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00395.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00396.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00397.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00398.PAUReco.root/PAUReco");
      chain->Add("~/Data/photons/15.6.1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1/group09.PhotonAnalysis.mc08.108086.PythiaPhotonJet_JetFilter_Nj2Et17.merge.AOD.e372_s462_s520_r808_r838.NTUP.rel1561.PAU-00-00-96.v1.EXT0._00399.PAUReco.root/PAUReco");
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

PAUisEMPerf::~PAUisEMPerf()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t PAUisEMPerf::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t PAUisEMPerf::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void PAUisEMPerf::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   ph_L1_matchPass = 0;
   ph_L2_matchPass = 0;
   ph_EF_matchPass = 0;
   TriggersRun = 0;
   PassL1 = 0;
   PassL2 = 0;
   PassEF = 0;
   HiggsParents = 0;
   HiggsDaughters = 0;
   HP_in_eta = 0;
   HP_in_phi = 0;
   HP_in_pt = 0;
   HP_in_e = 0;
   HP_in_pdg = 0;
   HP_out_eta = 0;
   HP_out_phi = 0;
   HP_out_pt = 0;
   HP_out_e = 0;
   HP_out_pdg = 0;
   HP_out_iRecPhoton = 0;
   HP_out_deltaR_RecPhoton = 0;
   PV_ID_zvertex = 0;
   truth_xvertex = 0;
   truth_yvertex = 0;
   truth_zvertex = 0;
   truth_ph_eta = 0;
   truth_ph_phi = 0;
   truth_ph_pt = 0;
   truth_ph_E = 0;
   truth_ph_zvertex = 0;
   truth_ph_isConv = 0;
   truth_ph_RConv = 0;
   truth_ph_XConv = 0;
   truth_ph_YConv = 0;
   truth_ph_ZConv = 0;
   truth_ph_mother = 0;
   truth_ph_isBrem = 0;
   truth_ph_isFromHardProc = 0;
   truth_ph_iRecPhoton = 0;
   truth_ph_deltaR_RecPhoton = 0;
   truth_jet_eta = 0;
   truth_jet_phi = 0;
   truth_jet_pt = 0;
   truth_jet_E = 0;
   truth_jet_parton = 0;
   truth_jet_iRecPhoton = 0;
   truth_jet_deltaR_RecPhoton = 0;
   ph_author = 0;
   ph_eta = 0;
   ph_etaS0 = 0;
   ph_etaS1 = 0;
   ph_etaS2 = 0;
   ph_phi = 0;
   ph_pt = 0;
   ph_Tclus = 0;
   ph_E = 0;
   ph_ES0 = 0;
   ph_ES1 = 0;
   ph_ES2 = 0;
   ph_ES3 = 0;
   ph_EcellS0 = 0;
   ph_etacellS0 = 0;
   ph_convFlag = 0;
   ph_isConv = 0;
   ph_isRecovered = 0;
   ph_RConv = 0;
   ph_ZConv = 0;
   ph_XConv = 0;
   ph_YConv = 0;
   ph_Pt1Conv = 0;
   ph_Pt2Conv = 0;
   ph_PtConv = 0;
   ph_PzConv = 0;
   ph_ConvIP = 0;
   ph_ConvIPRev = 0;
   ph_convTrk1_numBLHits = 0;
   ph_convTrk1_numPixelHits = 0;
   ph_convTrk1_numSCTHits = 0;
   ph_convTrk1_numTRTHits = 0;
   ph_convTrk2_numBLHits = 0;
   ph_convTrk2_numPixelHits = 0;
   ph_convTrk2_numSCTHits = 0;
   ph_convTrk2_numTRTHits = 0;
   ph_truth_convTrk1_pdgID = 0;
   ph_truth_convTrk1_pdgMOTHER = 0;
   ph_truth_convTrk1_pdgGrandMOTHER = 0;
   ph_truth_convTrk1_Type = 0;
   ph_truth_convTrk1_Origin = 0;
   ph_truth_convTrk1_phtType = 0;
   ph_truth_convTrk1_phtOrigin = 0;
   ph_truth_convTrk2_pdgID = 0;
   ph_truth_convTrk2_pdgMOTHER = 0;
   ph_truth_convTrk2_pdgGrandMOTHER = 0;
   ph_truth_convTrk2_Type = 0;
   ph_truth_convTrk2_Origin = 0;
   ph_truth_convTrk2_phtType = 0;
   ph_truth_convTrk2_phtOrigin = 0;
   ph_truth_Conv = 0;
   ph_isEM = 0;
   ph_isPhotonLoose = 0;
   ph_isPhotonTight = 0;
   ph_itune1 = 0;
   ph_itune2 = 0;
   ph_itune3 = 0;
   ph_logLikelihood = 0;
   ph_PtIsolationCone = 0;
   ph_PtIsolationConePhAngle = 0;
   ph_trackIsol = 0;
   ph_selected = 0;
   ph_shwr_EtHad = 0;
   ph_shwr_EtHad1 = 0;
   ph_shwr_E277 = 0;
   ph_shwr_E237 = 0;
   ph_shwr_E233 = 0;
   ph_shwr_weta2 = 0;
   ph_shwr_f1 = 0;
   ph_shwr_Emax2 = 0;
   ph_shwr_Emax1 = 0;
   ph_shwr_Emin = 0;
   ph_shwr_fracm = 0;
   ph_shwr_wtot = 0;
   ph_shwr_w1 = 0;
   ph_shwr_EtCone20 = 0;
   ph_shwr_EtCone30 = 0;
   ph_shwr_EtCone40 = 0;
   ph_shwr_EtCone45 = 0;
   ph_shwr_EtRing1030sig2 = 0;
   ph_shwr_EtRing1030sig3 = 0;
   ph_shwr_EtRing1030sig4 = 0;
   ph_topo_EtCone20 = 0;
   ph_topo_EtCone40 = 0;
   ph_topo_EtCone60 = 0;
   ph_topo_eta = 0;
   ph_topo_phi = 0;
   ph_topo_E = 0;
   ph_topo_pt = 0;
   ph_topoEM_eta = 0;
   ph_topoEM_phi = 0;
   ph_topoEM_E = 0;
   ph_topoEM_pt = 0;
   ph_imatchRecJet = 0;
   ph_deltaR_RecJet = 0;
   ph_L1_e = 0;
   ph_L1_eta = 0;
   ph_L1_phi = 0;
   ph_L1_EMIsol = 0;
   ph_L1_HadIsol = 0;
   ph_L1_HadCore = 0;
   ph_L2_e = 0;
   ph_L2_et = 0;
   ph_L2_eta = 0;
   ph_L2_phi = 0;
   ph_L2_e237 = 0;
   ph_L2_e277 = 0;
   ph_L2_fracs1 = 0;
   ph_L2_weta2 = 0;
   ph_L2_ehad1 = 0;
   ph_L2_emaxs1 = 0;
   ph_L2_e2tsts1 = 0;
   ph_EF_e = 0;
   ph_EF_et = 0;
   ph_EF_eta = 0;
   ph_EF_phi = 0;
   ph_EF_IsEM = 0;
   ph_EF_ethad = 0;
   ph_EF_ethad1 = 0;
   ph_EF_e233 = 0;
   ph_EF_e237 = 0;
   ph_EF_e277 = 0;
   ph_EF_emaxs1 = 0;
   ph_EF_e2tsts1 = 0;
   ph_EF_fracs1 = 0;
   ph_EF_weta1 = 0;
   ph_EF_weta2 = 0;
   ph_EF_wtots1 = 0;
   ph_EF_f1 = 0;
   ph_EF_emins1 = 0;
   ph_matchMC = 0;
   ph_eta_MC = 0;
   ph_phi_MC = 0;
   ph_pt_MC = 0;
   ph_E_MC = 0;
   ph_etaS2_MC = 0;
   ph_phiS2_MC = 0;
   ph_isConv_MC = 0;
   ph_RConv_MC = 0;
   ph_ZConv_MC = 0;
   ph_XConv_MC = 0;
   ph_YConv_MC = 0;
   ph_PDG = 0;
   ph_motherPDG = 0;
   ph_isBrem_MC = 0;
   ph_isFromHardProc_MC = 0;
   ph_isPhotonFromHardProc_MC = 0;
   ph_imatchParton_MC = 0;
   ph_shwr_EtCone10 = 0;
   ph_shwr_EtCone15 = 0;
   ph_shwr_EtCone25 = 0;
   ph_shwr_EtCone35 = 0;
   ph_EtCone20Iso = 0;
   ph_EtCone30Iso = 0;
   ph_EtCone40Iso = 0;
   ph_cells55_EtCone20 = 0;
   ph_cells55_EtCone30 = 0;
   ph_cells55_EtCone40 = 0;
   ph_cells55_EtCone20Iso = 0;
   ph_cells55_EtCone30Iso = 0;
   ph_cells55_EtCone40Iso = 0;
   ph_cells35_EtCone20 = 0;
   ph_cells35_EtCone30 = 0;
   ph_cells35_EtCone40 = 0;
   ph_cells35_EtCone20Iso = 0;
   ph_cells35_EtCone30Iso = 0;
   ph_cells35_EtCone40Iso = 0;
   cell_layer = 0;
   cell_energy = 0;
   cell_pt = 0;
   cell_eta = 0;
   cell_phi = 0;
   trk_eta = 0;
   trk_phi = 0;
   trk_pt = 0;
   trk_x0 = 0;
   trk_y0 = 0;
   trk_z0 = 0;
   trk_nhits_b = 0;
   trk_nhits_p = 0;
   trk_nhits_s = 0;
   trk_nhits_trt = 0;
   trk_nhits_trth = 0;
   trk_chiSquared = 0;
   trk_numberDoF = 0;
   trk_gConv = 0;
   jet_eta = 0;
   jet_phi = 0;
   jet_pt = 0;
   jet_E = 0;
   jet_selected_1j = 0;
   jet_selected_2j = 0;
   jet_imatchTrueJet_MC = 0;
   jet_eta_MC = 0;
   jet_phi_MC = 0;
   jet_pt_MC = 0;
   jet_E_MC = 0;
   el_author = 0;
   el_eta = 0;
   el_phi = 0;
   el_pt = 0;
   el_E = 0;
   el_etaS0 = 0;
   el_etaS1 = 0;
   el_etaS2 = 0;
   el_ES0 = 0;
   el_ES1 = 0;
   el_ES2 = 0;
   el_ES3 = 0;
   el_eta_clus = 0;
   el_phi_clus = 0;
   el_pt_clus = 0;
   el_E_clus = 0;
   el_Tclus = 0;
   el_Q = 0;
   el_isEM = 0;
   el_isElectronLoose = 0;
   el_isElectronMedium = 0;
   el_isElectronTight = 0;
   el_shwr_EtHad = 0;
   el_shwr_EtHad1 = 0;
   el_shwr_E277 = 0;
   el_shwr_E237 = 0;
   el_shwr_E233 = 0;
   el_shwr_weta2 = 0;
   el_shwr_f1 = 0;
   el_shwr_Emax2 = 0;
   el_shwr_Emax1 = 0;
   el_shwr_Emin = 0;
   el_shwr_fracm = 0;
   el_shwr_wtot = 0;
   el_shwr_w1 = 0;
   el_itune1 = 0;
   el_itune2 = 0;
   el_itune3 = 0;
   el_logLikelihood = 0;
   el_PtIsolationCone = 0;
   el_trackIsol = 0;
   el_dR_isol = 0;
   el_selected_etmiss = 0;
   el_selected_etlep = 0;
   el_matchMC = 0;
   el_eta_MC = 0;
   el_phi_MC = 0;
   el_pt_MC = 0;
   el_E_MC = 0;
   el_PDG = 0;
   el_motherPDG = 0;
   mu_eta = 0;
   mu_phi = 0;
   mu_pt = 0;
   mu_E = 0;
   mu_Q = 0;
   mu_selected_etmiss = 0;
   mu_selected_etlep = 0;
   mu_matchMC = 0;
   mu_eta_MC = 0;
   mu_phi_MC = 0;
   mu_pt_MC = 0;
   mu_E_MC = 0;
   mu_PDG = 0;
   mu_motherPDG = 0;
   hgg_iGoodPhoton = 0;
   hgg_iGoodMuon = 0;
   hgg_iGoodElectron = 0;
   hggZWjj_PTjZW4Tracks = 0;
   hggZWjj_PTjZW4 = 0;
   hggZWjj_PTjZWTracks = 0;
   hggZWjj_PTjZW = 0;
   hggZWjj_EtajZW4 = 0;
   hggZWjj_EtajZW = 0;
   hgg_fit_eta_zcommon = 0;
   hgg_fit_eta_zcommon_primVtxLH = 0;
   hgg_fit_mgg_vec = 0;
   ph_HPV_eta = 0;
   ph_HPV_sigma_eta = 0;
   hgg_HPV_zcommon = 0;
   hgg_HPV_zcommon_primVtxLH = 0;
   hgg_HPV_mgg = 0;
   hgg_HPV_mgg_primVtxLH = 0;
   hgg_HPV_eta_zcommon = 0;
   hgg_HPV_eta_zcommon_primVtxLH = 0;
   hgg_HPV_mgg_Ztruth = 0;
   ph_HPV_convTrk1_zvertex = 0;
   ph_HPV_convTrk2_zvertex = 0;
   ph_HPV_convTrk1_sigma_zvertex = 0;
   ph_HPV_convTrk2_sigma_zvertex = 0;
   ph_HPV_zvertex = 0;
   ph_HPV_sigma_zvertex = 0;
   ph_HPV_zvertex_neutral = 0;
   ph_HPV_zvertex_charged = 0;
   ph_HPV_sigma_zvertex_neutral = 0;
   ph_HPV_sigma_zvertex_charged = 0;
   ph_E_clus = 0;
   ph_eta_clus = 0;
   ph_phi_clus = 0;
   ph_pt_clus = 0;
   testOleg_ph_particleType = 0;
   testOleg_ph_particleOrig = 0;
   clus_SW_eta = 0;
   clus_SW_phi = 0;
   clus_SW_E = 0;
   clus_SW_size = 0;
   clus_SW_etasize = 0;
   clus_SW_phisize = 0;
   clus_SW_time = 0;
   clus_EMtopo_eta = 0;
   clus_EMtopo_phi = 0;
   clus_EMtopo_E = 0;
   clus_EMtopo_time = 0;
   clus_EMtopo_trackIsol05 = 0;
   clus_EMtopo_trackIsol07 = 0;
   clus_EMtopo_trackIsol10 = 0;
   clus_EMtopo_trackIsol20 = 0;
   clus_EMtopo_CENTER_LAMBDA = 0;
   clus_EMtopo_LATERAL = 0;
   clus_EMtopo_LONGITUDINAL = 0;
   clus_EMtopo_ENG_FRAC_EM = 0;
   clus_EMtopo_ENG_FRAC_MAX = 0;
   clus_EMtopo_ENG_FRAC_CORE = 0;
   clus_EMtopo_FIRST_ENG_DENS = 0;
   clus_EMtopo_SECOND_R = 0;
   clus_EMtopo_SECOND_LAMBDA = 0;
   clus_EMtopo_SECOND_ENG_DENS = 0;
   clus_EMtopo_ISOLATION = 0;
   clus_EMtopo_EM_PROBABILITY = 0;
   clus_EMtopo_HAD_WEIGHT = 0;
   clus_EMtopo_OOC_WEIGHT = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Run", &Run, &b_Run);
   fChain->SetBranchAddress("Event", &Event, &b_Event);
   fChain->SetBranchAddress("ph_L1_matchPass", &ph_L1_matchPass, &b_ph_L1_matchPass);
   fChain->SetBranchAddress("ph_L2_matchPass", &ph_L2_matchPass, &b_ph_L2_matchPass);
   fChain->SetBranchAddress("ph_EF_matchPass", &ph_EF_matchPass, &b_ph_EF_matchPass);
   fChain->SetBranchAddress("TriggersRun", &TriggersRun, &b_TriggersRun);
   fChain->SetBranchAddress("PassL1", &PassL1, &b_PassL1);
   fChain->SetBranchAddress("PassL2", &PassL2, &b_PassL2);
   fChain->SetBranchAddress("PassEF", &PassEF, &b_PassEF);
   fChain->SetBranchAddress("hard_process_MC", &hard_process_MC, &b_hard_process_MC);
   fChain->SetBranchAddress("isHgg_MC", &isHgg_MC, &b_isHgg_MC);
   fChain->SetBranchAddress("zHiggs_MC", &zHiggs_MC, &b_zHiggs_MC);
   fChain->SetBranchAddress("mHiggs_MC", &mHiggs_MC, &b_mHiggs_MC);
   fChain->SetBranchAddress("HiggsParents", &HiggsParents, &b_HiggsParents);
   fChain->SetBranchAddress("HiggsDaughters", &HiggsDaughters, &b_HiggsDaughters);
   fChain->SetBranchAddress("HP_in_eta", &HP_in_eta, &b_HP_in_eta);
   fChain->SetBranchAddress("HP_in_phi", &HP_in_phi, &b_HP_in_phi);
   fChain->SetBranchAddress("HP_in_pt", &HP_in_pt, &b_HP_in_pt);
   fChain->SetBranchAddress("HP_in_e", &HP_in_e, &b_HP_in_e);
   fChain->SetBranchAddress("HP_in_pdg", &HP_in_pdg, &b_HP_in_pdg);
   fChain->SetBranchAddress("HP_out_eta", &HP_out_eta, &b_HP_out_eta);
   fChain->SetBranchAddress("HP_out_phi", &HP_out_phi, &b_HP_out_phi);
   fChain->SetBranchAddress("HP_out_pt", &HP_out_pt, &b_HP_out_pt);
   fChain->SetBranchAddress("HP_out_e", &HP_out_e, &b_HP_out_e);
   fChain->SetBranchAddress("HP_out_pdg", &HP_out_pdg, &b_HP_out_pdg);
   fChain->SetBranchAddress("HP_out_iRecPhoton", &HP_out_iRecPhoton, &b_HP_out_iRecPhoton);
   fChain->SetBranchAddress("HP_out_deltaR_RecPhoton", &HP_out_deltaR_RecPhoton, &b_HP_out_deltaR_RecPhoton);
   fChain->SetBranchAddress("MCweight", &MCweight, &b_MCweight);
   fChain->SetBranchAddress("PV_ID_zvertex", &PV_ID_zvertex, &b_PV_ID_zvertex);
   fChain->SetBranchAddress("truth_xvertex", &truth_xvertex, &b_truth_xvertex);
   fChain->SetBranchAddress("truth_yvertex", &truth_yvertex, &b_truth_yvertex);
   fChain->SetBranchAddress("truth_zvertex", &truth_zvertex, &b_truth_zvertex);
   fChain->SetBranchAddress("truth_ph_eta", &truth_ph_eta, &b_truth_ph_eta);
   fChain->SetBranchAddress("truth_ph_phi", &truth_ph_phi, &b_truth_ph_phi);
   fChain->SetBranchAddress("truth_ph_pt", &truth_ph_pt, &b_truth_ph_pt);
   fChain->SetBranchAddress("truth_ph_E", &truth_ph_E, &b_truth_ph_E);
   fChain->SetBranchAddress("truth_ph_zvertex", &truth_ph_zvertex, &b_truth_ph_zvertex);
   fChain->SetBranchAddress("truth_ph_isConv", &truth_ph_isConv, &b_truth_ph_isConv);
   fChain->SetBranchAddress("truth_ph_RConv", &truth_ph_RConv, &b_truth_ph_RConv);
   fChain->SetBranchAddress("truth_ph_XConv", &truth_ph_XConv, &b_truth_ph_XConv);
   fChain->SetBranchAddress("truth_ph_YConv", &truth_ph_YConv, &b_truth_ph_YConv);
   fChain->SetBranchAddress("truth_ph_ZConv", &truth_ph_ZConv, &b_truth_ph_ZConv);
   fChain->SetBranchAddress("truth_ph_mother", &truth_ph_mother, &b_truth_ph_mother);
   fChain->SetBranchAddress("truth_ph_isBrem", &truth_ph_isBrem, &b_truth_ph_isBrem);
   fChain->SetBranchAddress("truth_ph_isFromHardProc", &truth_ph_isFromHardProc, &b_truth_ph_isFromHardProc);
   fChain->SetBranchAddress("truth_ph_iRecPhoton", &truth_ph_iRecPhoton, &b_truth_ph_iRecPhoton);
   fChain->SetBranchAddress("truth_ph_deltaR_RecPhoton", &truth_ph_deltaR_RecPhoton, &b_truth_ph_deltaR_RecPhoton);
   fChain->SetBranchAddress("truth_jet_eta", &truth_jet_eta, &b_truth_jet_eta);
   fChain->SetBranchAddress("truth_jet_phi", &truth_jet_phi, &b_truth_jet_phi);
   fChain->SetBranchAddress("truth_jet_pt", &truth_jet_pt, &b_truth_jet_pt);
   fChain->SetBranchAddress("truth_jet_E", &truth_jet_E, &b_truth_jet_E);
   fChain->SetBranchAddress("truth_jet_parton", &truth_jet_parton, &b_truth_jet_parton);
   fChain->SetBranchAddress("truth_jet_iRecPhoton", &truth_jet_iRecPhoton, &b_truth_jet_iRecPhoton);
   fChain->SetBranchAddress("truth_jet_deltaR_RecPhoton", &truth_jet_deltaR_RecPhoton, &b_truth_jet_deltaR_RecPhoton);
   fChain->SetBranchAddress("ph_author", &ph_author, &b_ph_author);
   fChain->SetBranchAddress("ph_eta", &ph_eta, &b_ph_eta);
   fChain->SetBranchAddress("ph_etaS0", &ph_etaS0, &b_ph_etaS0);
   fChain->SetBranchAddress("ph_etaS1", &ph_etaS1, &b_ph_etaS1);
   fChain->SetBranchAddress("ph_etaS2", &ph_etaS2, &b_ph_etaS2);
   fChain->SetBranchAddress("ph_phi", &ph_phi, &b_ph_phi);
   fChain->SetBranchAddress("ph_pt", &ph_pt, &b_ph_pt);
   fChain->SetBranchAddress("ph_Tclus", &ph_Tclus, &b_ph_Tclus);
   fChain->SetBranchAddress("ph_E", &ph_E, &b_ph_E);
   fChain->SetBranchAddress("ph_ES0", &ph_ES0, &b_ph_ES0);
   fChain->SetBranchAddress("ph_ES1", &ph_ES1, &b_ph_ES1);
   fChain->SetBranchAddress("ph_ES2", &ph_ES2, &b_ph_ES2);
   fChain->SetBranchAddress("ph_ES3", &ph_ES3, &b_ph_ES3);
   fChain->SetBranchAddress("ph_EcellS0", &ph_EcellS0, &b_ph_EcellS0);
   fChain->SetBranchAddress("ph_etacellS0", &ph_etacellS0, &b_ph_etacellS0);
   fChain->SetBranchAddress("ph_convFlag", &ph_convFlag, &b_ph_convFlag);
   fChain->SetBranchAddress("ph_isConv", &ph_isConv, &b_ph_isConv);
   fChain->SetBranchAddress("ph_isRecovered", &ph_isRecovered, &b_ph_isRecovered);
   fChain->SetBranchAddress("ph_RConv", &ph_RConv, &b_ph_RConv);
   fChain->SetBranchAddress("ph_ZConv", &ph_ZConv, &b_ph_ZConv);
   fChain->SetBranchAddress("ph_XConv", &ph_XConv, &b_ph_XConv);
   fChain->SetBranchAddress("ph_YConv", &ph_YConv, &b_ph_YConv);
   fChain->SetBranchAddress("ph_Pt1Conv", &ph_Pt1Conv, &b_ph_Pt1Conv);
   fChain->SetBranchAddress("ph_Pt2Conv", &ph_Pt2Conv, &b_ph_Pt2Conv);
   fChain->SetBranchAddress("ph_PtConv", &ph_PtConv, &b_ph_PtConv);
   fChain->SetBranchAddress("ph_PzConv", &ph_PzConv, &b_ph_PzConv);
   fChain->SetBranchAddress("ph_ConvIP", &ph_ConvIP, &b_ph_ConvIP);
   fChain->SetBranchAddress("ph_ConvIPRev", &ph_ConvIPRev, &b_ph_ConvIPRev);
   fChain->SetBranchAddress("ph_convTrk1_numBLHits", &ph_convTrk1_numBLHits, &b_ph_convTrk1_numBLHits);
   fChain->SetBranchAddress("ph_convTrk1_numPixelHits", &ph_convTrk1_numPixelHits, &b_ph_convTrk1_numPixelHits);
   fChain->SetBranchAddress("ph_convTrk1_numSCTHits", &ph_convTrk1_numSCTHits, &b_ph_convTrk1_numSCTHits);
   fChain->SetBranchAddress("ph_convTrk1_numTRTHits", &ph_convTrk1_numTRTHits, &b_ph_convTrk1_numTRTHits);
   fChain->SetBranchAddress("ph_convTrk2_numBLHits", &ph_convTrk2_numBLHits, &b_ph_convTrk2_numBLHits);
   fChain->SetBranchAddress("ph_convTrk2_numPixelHits", &ph_convTrk2_numPixelHits, &b_ph_convTrk2_numPixelHits);
   fChain->SetBranchAddress("ph_convTrk2_numSCTHits", &ph_convTrk2_numSCTHits, &b_ph_convTrk2_numSCTHits);
   fChain->SetBranchAddress("ph_convTrk2_numTRTHits", &ph_convTrk2_numTRTHits, &b_ph_convTrk2_numTRTHits);
   fChain->SetBranchAddress("ph_truth_convTrk1_pdgID", &ph_truth_convTrk1_pdgID, &b_ph_truth_convTrk1_pdgID);
   fChain->SetBranchAddress("ph_truth_convTrk1_pdgMOTHER", &ph_truth_convTrk1_pdgMOTHER, &b_ph_truth_convTrk1_pdgMOTHER);
   fChain->SetBranchAddress("ph_truth_convTrk1_pdgGrandMOTHER", &ph_truth_convTrk1_pdgGrandMOTHER, &b_ph_truth_convTrk1_pdgGrandMOTHER);
   fChain->SetBranchAddress("ph_truth_convTrk1_Type", &ph_truth_convTrk1_Type, &b_ph_truth_convTrk1_Type);
   fChain->SetBranchAddress("ph_truth_convTrk1_Origin", &ph_truth_convTrk1_Origin, &b_ph_truth_convTrk1_Origin);
   fChain->SetBranchAddress("ph_truth_convTrk1_phtType", &ph_truth_convTrk1_phtType, &b_ph_truth_convTrk1_phtType);
   fChain->SetBranchAddress("ph_truth_convTrk1_phtOrigin", &ph_truth_convTrk1_phtOrigin, &b_ph_truth_convTrk1_phtOrigin);
   fChain->SetBranchAddress("ph_truth_convTrk2_pdgID", &ph_truth_convTrk2_pdgID, &b_ph_truth_convTrk2_pdgID);
   fChain->SetBranchAddress("ph_truth_convTrk2_pdgMOTHER", &ph_truth_convTrk2_pdgMOTHER, &b_ph_truth_convTrk2_pdgMOTHER);
   fChain->SetBranchAddress("ph_truth_convTrk2_pdgGrandMOTHER", &ph_truth_convTrk2_pdgGrandMOTHER, &b_ph_truth_convTrk2_pdgGrandMOTHER);
   fChain->SetBranchAddress("ph_truth_convTrk2_Type", &ph_truth_convTrk2_Type, &b_ph_truth_convTrk2_Type);
   fChain->SetBranchAddress("ph_truth_convTrk2_Origin", &ph_truth_convTrk2_Origin, &b_ph_truth_convTrk2_Origin);
   fChain->SetBranchAddress("ph_truth_convTrk2_phtType", &ph_truth_convTrk2_phtType, &b_ph_truth_convTrk2_phtType);
   fChain->SetBranchAddress("ph_truth_convTrk2_phtOrigin", &ph_truth_convTrk2_phtOrigin, &b_ph_truth_convTrk2_phtOrigin);
   fChain->SetBranchAddress("ph_truth_Conv", &ph_truth_Conv, &b_ph_truth_Conv);
   fChain->SetBranchAddress("ph_isEM", &ph_isEM, &b_ph_isEM);
   fChain->SetBranchAddress("ph_isPhotonLoose", &ph_isPhotonLoose, &b_ph_isPhotonLoose);
   fChain->SetBranchAddress("ph_isPhotonTight", &ph_isPhotonTight, &b_ph_isPhotonTight);
   fChain->SetBranchAddress("ph_itune1", &ph_itune1, &b_ph_itune1);
   fChain->SetBranchAddress("ph_itune2", &ph_itune2, &b_ph_itune2);
   fChain->SetBranchAddress("ph_itune3", &ph_itune3, &b_ph_itune3);
   fChain->SetBranchAddress("ph_logLikelihood", &ph_logLikelihood, &b_ph_logLikelihood);
   fChain->SetBranchAddress("ph_PtIsolationCone", &ph_PtIsolationCone, &b_ph_PtIsolationCone);
   fChain->SetBranchAddress("ph_PtIsolationConePhAngle", &ph_PtIsolationConePhAngle, &b_ph_PtIsolationConePhAngle);
   fChain->SetBranchAddress("ph_trackIsol", &ph_trackIsol, &b_ph_trackIsol);
   fChain->SetBranchAddress("ph_selected", &ph_selected, &b_ph_selected);
   fChain->SetBranchAddress("ph_shwr_EtHad", &ph_shwr_EtHad, &b_ph_shwr_EtHad);
   fChain->SetBranchAddress("ph_shwr_EtHad1", &ph_shwr_EtHad1, &b_ph_shwr_EtHad1);
   fChain->SetBranchAddress("ph_shwr_E277", &ph_shwr_E277, &b_ph_shwr_E277);
   fChain->SetBranchAddress("ph_shwr_E237", &ph_shwr_E237, &b_ph_shwr_E237);
   fChain->SetBranchAddress("ph_shwr_E233", &ph_shwr_E233, &b_ph_shwr_E233);
   fChain->SetBranchAddress("ph_shwr_weta2", &ph_shwr_weta2, &b_ph_shwr_weta2);
   fChain->SetBranchAddress("ph_shwr_f1", &ph_shwr_f1, &b_ph_shwr_f1);
   fChain->SetBranchAddress("ph_shwr_Emax2", &ph_shwr_Emax2, &b_ph_shwr_Emax2);
   fChain->SetBranchAddress("ph_shwr_Emax1", &ph_shwr_Emax1, &b_ph_shwr_Emax1);
   fChain->SetBranchAddress("ph_shwr_Emin", &ph_shwr_Emin, &b_ph_shwr_Emin);
   fChain->SetBranchAddress("ph_shwr_fracm", &ph_shwr_fracm, &b_ph_shwr_fracm);
   fChain->SetBranchAddress("ph_shwr_wtot", &ph_shwr_wtot, &b_ph_shwr_wtot);
   fChain->SetBranchAddress("ph_shwr_w1", &ph_shwr_w1, &b_ph_shwr_w1);
   fChain->SetBranchAddress("ph_shwr_EtCone20", &ph_shwr_EtCone20, &b_ph_shwr_EtCone20);
   fChain->SetBranchAddress("ph_shwr_EtCone30", &ph_shwr_EtCone30, &b_ph_shwr_EtCone30);
   fChain->SetBranchAddress("ph_shwr_EtCone40", &ph_shwr_EtCone40, &b_ph_shwr_EtCone40);
   fChain->SetBranchAddress("ph_shwr_EtCone45", &ph_shwr_EtCone45, &b_ph_shwr_EtCone45);
   fChain->SetBranchAddress("ph_shwr_EtRing1030sig2", &ph_shwr_EtRing1030sig2, &b_ph_shwr_EtRing1030sig2);
   fChain->SetBranchAddress("ph_shwr_EtRing1030sig3", &ph_shwr_EtRing1030sig3, &b_ph_shwr_EtRing1030sig3);
   fChain->SetBranchAddress("ph_shwr_EtRing1030sig4", &ph_shwr_EtRing1030sig4, &b_ph_shwr_EtRing1030sig4);
   fChain->SetBranchAddress("ph_topo_EtCone20", &ph_topo_EtCone20, &b_ph_topo_EtCone20);
   fChain->SetBranchAddress("ph_topo_EtCone40", &ph_topo_EtCone40, &b_ph_topo_EtCone40);
   fChain->SetBranchAddress("ph_topo_EtCone60", &ph_topo_EtCone60, &b_ph_topo_EtCone60);
   fChain->SetBranchAddress("ph_topo_eta", &ph_topo_eta, &b_ph_topo_eta);
   fChain->SetBranchAddress("ph_topo_phi", &ph_topo_phi, &b_ph_topo_phi);
   fChain->SetBranchAddress("ph_topo_E", &ph_topo_E, &b_ph_topo_E);
   fChain->SetBranchAddress("ph_topo_pt", &ph_topo_pt, &b_ph_topo_pt);
   fChain->SetBranchAddress("ph_topoEM_eta", &ph_topoEM_eta, &b_ph_topoEM_eta);
   fChain->SetBranchAddress("ph_topoEM_phi", &ph_topoEM_phi, &b_ph_topoEM_phi);
   fChain->SetBranchAddress("ph_topoEM_E", &ph_topoEM_E, &b_ph_topoEM_E);
   fChain->SetBranchAddress("ph_topoEM_pt", &ph_topoEM_pt, &b_ph_topoEM_pt);
   fChain->SetBranchAddress("ph_imatchRecJet", &ph_imatchRecJet, &b_ph_imatchRecJet);
   fChain->SetBranchAddress("ph_deltaR_RecJet", &ph_deltaR_RecJet, &b_ph_deltaR_RecJet);
   fChain->SetBranchAddress("ph_L1_e", &ph_L1_e, &b_ph_L1_e);
   fChain->SetBranchAddress("ph_L1_eta", &ph_L1_eta, &b_ph_L1_eta);
   fChain->SetBranchAddress("ph_L1_phi", &ph_L1_phi, &b_ph_L1_phi);
   fChain->SetBranchAddress("ph_L1_EMIsol", &ph_L1_EMIsol, &b_ph_L1_EMIsol);
   fChain->SetBranchAddress("ph_L1_HadIsol", &ph_L1_HadIsol, &b_ph_L1_HadIsol);
   fChain->SetBranchAddress("ph_L1_HadCore", &ph_L1_HadCore, &b_ph_L1_HadCore);
   fChain->SetBranchAddress("ph_L2_e", &ph_L2_e, &b_ph_L2_e);
   fChain->SetBranchAddress("ph_L2_et", &ph_L2_et, &b_ph_L2_et);
   fChain->SetBranchAddress("ph_L2_eta", &ph_L2_eta, &b_ph_L2_eta);
   fChain->SetBranchAddress("ph_L2_phi", &ph_L2_phi, &b_ph_L2_phi);
   fChain->SetBranchAddress("ph_L2_e237", &ph_L2_e237, &b_ph_L2_e237);
   fChain->SetBranchAddress("ph_L2_e277", &ph_L2_e277, &b_ph_L2_e277);
   fChain->SetBranchAddress("ph_L2_fracs1", &ph_L2_fracs1, &b_ph_L2_fracs1);
   fChain->SetBranchAddress("ph_L2_weta2", &ph_L2_weta2, &b_ph_L2_weta2);
   fChain->SetBranchAddress("ph_L2_ehad1", &ph_L2_ehad1, &b_ph_L2_ehad1);
   fChain->SetBranchAddress("ph_L2_emaxs1", &ph_L2_emaxs1, &b_ph_L2_emaxs1);
   fChain->SetBranchAddress("ph_L2_e2tsts1", &ph_L2_e2tsts1, &b_ph_L2_e2tsts1);
   fChain->SetBranchAddress("ph_EF_e", &ph_EF_e, &b_ph_EF_e);
   fChain->SetBranchAddress("ph_EF_et", &ph_EF_et, &b_ph_EF_et);
   fChain->SetBranchAddress("ph_EF_eta", &ph_EF_eta, &b_ph_EF_eta);
   fChain->SetBranchAddress("ph_EF_phi", &ph_EF_phi, &b_ph_EF_phi);
   fChain->SetBranchAddress("ph_EF_IsEM", &ph_EF_IsEM, &b_ph_EF_IsEM);
   fChain->SetBranchAddress("ph_EF_ethad", &ph_EF_ethad, &b_ph_EF_ethad);
   fChain->SetBranchAddress("ph_EF_ethad1", &ph_EF_ethad1, &b_ph_EF_ethad1);
   fChain->SetBranchAddress("ph_EF_e233", &ph_EF_e233, &b_ph_EF_e233);
   fChain->SetBranchAddress("ph_EF_e237", &ph_EF_e237, &b_ph_EF_e237);
   fChain->SetBranchAddress("ph_EF_e277", &ph_EF_e277, &b_ph_EF_e277);
   fChain->SetBranchAddress("ph_EF_emaxs1", &ph_EF_emaxs1, &b_ph_EF_emaxs1);
   fChain->SetBranchAddress("ph_EF_e2tsts1", &ph_EF_e2tsts1, &b_ph_EF_e2tsts1);
   fChain->SetBranchAddress("ph_EF_fracs1", &ph_EF_fracs1, &b_ph_EF_fracs1);
   fChain->SetBranchAddress("ph_EF_weta1", &ph_EF_weta1, &b_ph_EF_weta1);
   fChain->SetBranchAddress("ph_EF_weta2", &ph_EF_weta2, &b_ph_EF_weta2);
   fChain->SetBranchAddress("ph_EF_wtots1", &ph_EF_wtots1, &b_ph_EF_wtots1);
   fChain->SetBranchAddress("ph_EF_f1", &ph_EF_f1, &b_ph_EF_f1);
   fChain->SetBranchAddress("ph_EF_emins1", &ph_EF_emins1, &b_ph_EF_emins1);
   fChain->SetBranchAddress("ph_matchMC", &ph_matchMC, &b_ph_matchMC);
   fChain->SetBranchAddress("ph_eta_MC", &ph_eta_MC, &b_ph_eta_MC);
   fChain->SetBranchAddress("ph_phi_MC", &ph_phi_MC, &b_ph_phi_MC);
   fChain->SetBranchAddress("ph_pt_MC", &ph_pt_MC, &b_ph_pt_MC);
   fChain->SetBranchAddress("ph_E_MC", &ph_E_MC, &b_ph_E_MC);
   fChain->SetBranchAddress("ph_etaS2_MC", &ph_etaS2_MC, &b_ph_etaS2_MC);
   fChain->SetBranchAddress("ph_phiS2_MC", &ph_phiS2_MC, &b_ph_phiS2_MC);
   fChain->SetBranchAddress("ph_isConv_MC", &ph_isConv_MC, &b_ph_isConv_MC);
   fChain->SetBranchAddress("ph_RConv_MC", &ph_RConv_MC, &b_ph_RConv_MC);
   fChain->SetBranchAddress("ph_ZConv_MC", &ph_ZConv_MC, &b_ph_ZConv_MC);
   fChain->SetBranchAddress("ph_XConv_MC", &ph_XConv_MC, &b_ph_XConv_MC);
   fChain->SetBranchAddress("ph_YConv_MC", &ph_YConv_MC, &b_ph_YConv_MC);
   fChain->SetBranchAddress("ph_PDG", &ph_PDG, &b_ph_PDG);
   fChain->SetBranchAddress("ph_motherPDG", &ph_motherPDG, &b_ph_motherPDG);
   fChain->SetBranchAddress("ph_isBrem_MC", &ph_isBrem_MC, &b_ph_isBrem_MC);
   fChain->SetBranchAddress("ph_isFromHardProc_MC", &ph_isFromHardProc_MC, &b_ph_isFromHardProc_MC);
   fChain->SetBranchAddress("ph_isPhotonFromHardProc_MC", &ph_isPhotonFromHardProc_MC, &b_ph_isPhotonFromHardProc_MC);
   fChain->SetBranchAddress("ph_imatchParton_MC", &ph_imatchParton_MC, &b_ph_imatchParton_MC);
   fChain->SetBranchAddress("ph_shwr_EtCone10", &ph_shwr_EtCone10, &b_ph_shwr_EtCone10);
   fChain->SetBranchAddress("ph_shwr_EtCone15", &ph_shwr_EtCone15, &b_ph_shwr_EtCone15);
   fChain->SetBranchAddress("ph_shwr_EtCone25", &ph_shwr_EtCone25, &b_ph_shwr_EtCone25);
   fChain->SetBranchAddress("ph_shwr_EtCone35", &ph_shwr_EtCone35, &b_ph_shwr_EtCone35);
   fChain->SetBranchAddress("ph_EtCone20Iso", &ph_EtCone20Iso, &b_ph_EtCone20Iso);
   fChain->SetBranchAddress("ph_EtCone30Iso", &ph_EtCone30Iso, &b_ph_EtCone30Iso);
   fChain->SetBranchAddress("ph_EtCone40Iso", &ph_EtCone40Iso, &b_ph_EtCone40Iso);
   fChain->SetBranchAddress("ph_cells55_EtCone20", &ph_cells55_EtCone20, &b_ph_cells55_EtCone20);
   fChain->SetBranchAddress("ph_cells55_EtCone30", &ph_cells55_EtCone30, &b_ph_cells55_EtCone30);
   fChain->SetBranchAddress("ph_cells55_EtCone40", &ph_cells55_EtCone40, &b_ph_cells55_EtCone40);
   fChain->SetBranchAddress("ph_cells55_EtCone20Iso", &ph_cells55_EtCone20Iso, &b_ph_cells55_EtCone20Iso);
   fChain->SetBranchAddress("ph_cells55_EtCone30Iso", &ph_cells55_EtCone30Iso, &b_ph_cells55_EtCone30Iso);
   fChain->SetBranchAddress("ph_cells55_EtCone40Iso", &ph_cells55_EtCone40Iso, &b_ph_cells55_EtCone40Iso);
   fChain->SetBranchAddress("ph_cells35_EtCone20", &ph_cells35_EtCone20, &b_ph_cells35_EtCone20);
   fChain->SetBranchAddress("ph_cells35_EtCone30", &ph_cells35_EtCone30, &b_ph_cells35_EtCone30);
   fChain->SetBranchAddress("ph_cells35_EtCone40", &ph_cells35_EtCone40, &b_ph_cells35_EtCone40);
   fChain->SetBranchAddress("ph_cells35_EtCone20Iso", &ph_cells35_EtCone20Iso, &b_ph_cells35_EtCone20Iso);
   fChain->SetBranchAddress("ph_cells35_EtCone30Iso", &ph_cells35_EtCone30Iso, &b_ph_cells35_EtCone30Iso);
   fChain->SetBranchAddress("ph_cells35_EtCone40Iso", &ph_cells35_EtCone40Iso, &b_ph_cells35_EtCone40Iso);
   fChain->SetBranchAddress("Ncells", &Ncells, &b_Ncells);
   fChain->SetBranchAddress("cell_layer", &cell_layer, &b_cell_layer);
   fChain->SetBranchAddress("cell_energy", &cell_energy, &b_cell_energy);
   fChain->SetBranchAddress("cell_pt", &cell_pt, &b_cell_pt);
   fChain->SetBranchAddress("cell_eta", &cell_eta, &b_cell_eta);
   fChain->SetBranchAddress("cell_phi", &cell_phi, &b_cell_phi);
   fChain->SetBranchAddress("NTracks", &NTracks, &b_NTracks);
   fChain->SetBranchAddress("trk_eta", &trk_eta, &b_trk_eta);
   fChain->SetBranchAddress("trk_phi", &trk_phi, &b_trk_phi);
   fChain->SetBranchAddress("trk_pt", &trk_pt, &b_trk_pt);
   fChain->SetBranchAddress("trk_x0", &trk_x0, &b_trk_x0);
   fChain->SetBranchAddress("trk_y0", &trk_y0, &b_trk_y0);
   fChain->SetBranchAddress("trk_z0", &trk_z0, &b_trk_z0);
   fChain->SetBranchAddress("trk_nhits_b", &trk_nhits_b, &b_trk_nhits_b);
   fChain->SetBranchAddress("trk_nhits_p", &trk_nhits_p, &b_trk_nhits_p);
   fChain->SetBranchAddress("trk_nhits_s", &trk_nhits_s, &b_trk_nhits_s);
   fChain->SetBranchAddress("trk_nhits_trt", &trk_nhits_trt, &b_trk_nhits_trt);
   fChain->SetBranchAddress("trk_nhits_trth", &trk_nhits_trth, &b_trk_nhits_trth);
   fChain->SetBranchAddress("trk_chiSquared", &trk_chiSquared, &b_trk_chiSquared);
   fChain->SetBranchAddress("trk_numberDoF", &trk_numberDoF, &b_trk_numberDoF);
   fChain->SetBranchAddress("trk_gConv", &trk_gConv, &b_trk_gConv);
   fChain->SetBranchAddress("jet_eta", &jet_eta, &b_jet_eta);
   fChain->SetBranchAddress("jet_phi", &jet_phi, &b_jet_phi);
   fChain->SetBranchAddress("jet_pt", &jet_pt, &b_jet_pt);
   fChain->SetBranchAddress("jet_E", &jet_E, &b_jet_E);
   fChain->SetBranchAddress("jet_selected_1j", &jet_selected_1j, &b_jet_selected_1j);
   fChain->SetBranchAddress("jet_selected_2j", &jet_selected_2j, &b_jet_selected_2j);
   fChain->SetBranchAddress("jet_imatchTrueJet_MC", &jet_imatchTrueJet_MC, &b_jet_imatchTrueJet_MC);
   fChain->SetBranchAddress("jet_eta_MC", &jet_eta_MC, &b_jet_eta_MC);
   fChain->SetBranchAddress("jet_phi_MC", &jet_phi_MC, &b_jet_phi_MC);
   fChain->SetBranchAddress("jet_pt_MC", &jet_pt_MC, &b_jet_pt_MC);
   fChain->SetBranchAddress("jet_E_MC", &jet_E_MC, &b_jet_E_MC);
   fChain->SetBranchAddress("el_author", &el_author, &b_el_author);
   fChain->SetBranchAddress("el_eta", &el_eta, &b_el_eta);
   fChain->SetBranchAddress("el_phi", &el_phi, &b_el_phi);
   fChain->SetBranchAddress("el_pt", &el_pt, &b_el_pt);
   fChain->SetBranchAddress("el_E", &el_E, &b_el_E);
   fChain->SetBranchAddress("el_etaS0", &el_etaS0, &b_el_etaS0);
   fChain->SetBranchAddress("el_etaS1", &el_etaS1, &b_el_etaS1);
   fChain->SetBranchAddress("el_etaS2", &el_etaS2, &b_el_etaS2);
   fChain->SetBranchAddress("el_ES0", &el_ES0, &b_el_ES0);
   fChain->SetBranchAddress("el_ES1", &el_ES1, &b_el_ES1);
   fChain->SetBranchAddress("el_ES2", &el_ES2, &b_el_ES2);
   fChain->SetBranchAddress("el_ES3", &el_ES3, &b_el_ES3);
   fChain->SetBranchAddress("el_eta_clus", &el_eta_clus, &b_el_eta_clus);
   fChain->SetBranchAddress("el_phi_clus", &el_phi_clus, &b_el_phi_clus);
   fChain->SetBranchAddress("el_pt_clus", &el_pt_clus, &b_el_pt_clus);
   fChain->SetBranchAddress("el_E_clus", &el_E_clus, &b_el_E_clus);
   fChain->SetBranchAddress("el_Tclus", &el_Tclus, &b_el_Tclus);
   fChain->SetBranchAddress("el_Q", &el_Q, &b_el_Q);
   fChain->SetBranchAddress("el_isEM", &el_isEM, &b_el_isEM);
   fChain->SetBranchAddress("el_isElectronLoose", &el_isElectronLoose, &b_el_isElectronLoose);
   fChain->SetBranchAddress("el_isElectronMedium", &el_isElectronMedium, &b_el_isElectronMedium);
   fChain->SetBranchAddress("el_isElectronTight", &el_isElectronTight, &b_el_isElectronTight);
   fChain->SetBranchAddress("el_shwr_EtHad", &el_shwr_EtHad, &b_el_shwr_EtHad);
   fChain->SetBranchAddress("el_shwr_EtHad1", &el_shwr_EtHad1, &b_el_shwr_EtHad1);
   fChain->SetBranchAddress("el_shwr_E277", &el_shwr_E277, &b_el_shwr_E277);
   fChain->SetBranchAddress("el_shwr_E237", &el_shwr_E237, &b_el_shwr_E237);
   fChain->SetBranchAddress("el_shwr_E233", &el_shwr_E233, &b_el_shwr_E233);
   fChain->SetBranchAddress("el_shwr_weta2", &el_shwr_weta2, &b_el_shwr_weta2);
   fChain->SetBranchAddress("el_shwr_f1", &el_shwr_f1, &b_el_shwr_f1);
   fChain->SetBranchAddress("el_shwr_Emax2", &el_shwr_Emax2, &b_el_shwr_Emax2);
   fChain->SetBranchAddress("el_shwr_Emax1", &el_shwr_Emax1, &b_el_shwr_Emax1);
   fChain->SetBranchAddress("el_shwr_Emin", &el_shwr_Emin, &b_el_shwr_Emin);
   fChain->SetBranchAddress("el_shwr_fracm", &el_shwr_fracm, &b_el_shwr_fracm);
   fChain->SetBranchAddress("el_shwr_wtot", &el_shwr_wtot, &b_el_shwr_wtot);
   fChain->SetBranchAddress("el_shwr_w1", &el_shwr_w1, &b_el_shwr_w1);
   fChain->SetBranchAddress("el_itune1", &el_itune1, &b_el_itune1);
   fChain->SetBranchAddress("el_itune2", &el_itune2, &b_el_itune2);
   fChain->SetBranchAddress("el_itune3", &el_itune3, &b_el_itune3);
   fChain->SetBranchAddress("el_logLikelihood", &el_logLikelihood, &b_el_logLikelihood);
   fChain->SetBranchAddress("el_PtIsolationCone", &el_PtIsolationCone, &b_el_PtIsolationCone);
   fChain->SetBranchAddress("el_trackIsol", &el_trackIsol, &b_el_trackIsol);
   fChain->SetBranchAddress("el_dR_isol", &el_dR_isol, &b_el_dR_isol);
   fChain->SetBranchAddress("el_selected_etmiss", &el_selected_etmiss, &b_el_selected_etmiss);
   fChain->SetBranchAddress("el_selected_etlep", &el_selected_etlep, &b_el_selected_etlep);
   fChain->SetBranchAddress("el_matchMC", &el_matchMC, &b_el_matchMC);
   fChain->SetBranchAddress("el_eta_MC", &el_eta_MC, &b_el_eta_MC);
   fChain->SetBranchAddress("el_phi_MC", &el_phi_MC, &b_el_phi_MC);
   fChain->SetBranchAddress("el_pt_MC", &el_pt_MC, &b_el_pt_MC);
   fChain->SetBranchAddress("el_E_MC", &el_E_MC, &b_el_E_MC);
   fChain->SetBranchAddress("el_PDG", &el_PDG, &b_el_PDG);
   fChain->SetBranchAddress("el_motherPDG", &el_motherPDG, &b_el_motherPDG);
   fChain->SetBranchAddress("mu_eta", &mu_eta, &b_mu_eta);
   fChain->SetBranchAddress("mu_phi", &mu_phi, &b_mu_phi);
   fChain->SetBranchAddress("mu_pt", &mu_pt, &b_mu_pt);
   fChain->SetBranchAddress("mu_E", &mu_E, &b_mu_E);
   fChain->SetBranchAddress("mu_Q", &mu_Q, &b_mu_Q);
   fChain->SetBranchAddress("mu_selected_etmiss", &mu_selected_etmiss, &b_mu_selected_etmiss);
   fChain->SetBranchAddress("mu_selected_etlep", &mu_selected_etlep, &b_mu_selected_etlep);
   fChain->SetBranchAddress("mu_matchMC", &mu_matchMC, &b_mu_matchMC);
   fChain->SetBranchAddress("mu_eta_MC", &mu_eta_MC, &b_mu_eta_MC);
   fChain->SetBranchAddress("mu_phi_MC", &mu_phi_MC, &b_mu_phi_MC);
   fChain->SetBranchAddress("mu_pt_MC", &mu_pt_MC, &b_mu_pt_MC);
   fChain->SetBranchAddress("mu_E_MC", &mu_E_MC, &b_mu_E_MC);
   fChain->SetBranchAddress("mu_PDG", &mu_PDG, &b_mu_PDG);
   fChain->SetBranchAddress("mu_motherPDG", &mu_motherPDG, &b_mu_motherPDG);
   fChain->SetBranchAddress("mEt_ex", &mEt_ex, &b_mEt_ex);
   fChain->SetBranchAddress("mEt_ey", &mEt_ey, &b_mEt_ey);
   fChain->SetBranchAddress("mEt_et", &mEt_et, &b_mEt_et);
   fChain->SetBranchAddress("mEt_sumet", &mEt_sumet, &b_mEt_sumet);
   fChain->SetBranchAddress("mEt_ex_MC", &mEt_ex_MC, &b_mEt_ex_MC);
   fChain->SetBranchAddress("mEt_ey_MC", &mEt_ey_MC, &b_mEt_ey_MC);
   fChain->SetBranchAddress("mEt_et_MC", &mEt_et_MC, &b_mEt_et_MC);
   fChain->SetBranchAddress("NElectrons", &NElectrons, &b_NElectrons);
   fChain->SetBranchAddress("NPhotons", &NPhotons, &b_NPhotons);
   fChain->SetBranchAddress("NGammas", &NGammas, &b_NGammas);
   fChain->SetBranchAddress("hgg_iPhotonLead", &hgg_iPhotonLead, &b_hgg_iPhotonLead);
   fChain->SetBranchAddress("hgg_iPhotonSublead", &hgg_iPhotonSublead, &b_hgg_iPhotonSublead);
   fChain->SetBranchAddress("hgg_iGoodPhoton", &hgg_iGoodPhoton, &b_hgg_iGoodPhoton);
   fChain->SetBranchAddress("hgg_okInclusive", &hgg_okInclusive, &b_hgg_okInclusive);
   fChain->SetBranchAddress("hgg1jet_iJetLead", &hgg1jet_iJetLead, &b_hgg1jet_iJetLead);
   fChain->SetBranchAddress("hgg1jet_cutStatus", &hgg1jet_cutStatus, &b_hgg1jet_cutStatus);
   fChain->SetBranchAddress("hgg1jet_ok", &hgg1jet_ok, &b_hgg1jet_ok);
   fChain->SetBranchAddress("hgg2jet_iJetLead", &hgg2jet_iJetLead, &b_hgg2jet_iJetLead);
   fChain->SetBranchAddress("hgg2jet_iJetSublead", &hgg2jet_iJetSublead, &b_hgg2jet_iJetSublead);
   fChain->SetBranchAddress("hgg2jet_cutStatus", &hgg2jet_cutStatus, &b_hgg2jet_cutStatus);
   fChain->SetBranchAddress("hgg2jet_ok", &hgg2jet_ok, &b_hgg2jet_ok);
   fChain->SetBranchAddress("hggEtmiss_ok", &hggEtmiss_ok, &b_hggEtmiss_ok);
   fChain->SetBranchAddress("hggLepton_ok", &hggLepton_ok, &b_hggLepton_ok);
   fChain->SetBranchAddress("hggZWHjj_ok", &hggZWHjj_ok, &b_hggZWHjj_ok);
   fChain->SetBranchAddress("hggLepton_DeltaRmujet", &hggLepton_DeltaRmujet, &b_DeltaRmujet);
   fChain->SetBranchAddress("hgg_iGoodMuon", &hgg_iGoodMuon, &b_hgg_iGoodMuon);
   fChain->SetBranchAddress("hgg_iGoodElectron", &hgg_iGoodElectron, &b_hgg_iGoodElectron);
   fChain->SetBranchAddress("hggZWjj_mjjZW_tracks", &hggZWjj_mjjZW_tracks, &b_mjjZW_tracks);
   fChain->SetBranchAddress("hggZWjj_mjjZW", &hggZWjj_mjjZW, &b_mjjZW);
   fChain->SetBranchAddress("hggZWjj_mjjZW4_tracks", &hggZWjj_mjjZW4_tracks, &b_mjjZW4_tracks);
   fChain->SetBranchAddress("hggZWjj_mjjZW4", &hggZWjj_mjjZW4, &b_mjjZW4);
   fChain->SetBranchAddress("hggZWjj_PTjjZW_tracks", &hggZWjj_PTjjZW_tracks, &b_PTjjZW_tracks);
   fChain->SetBranchAddress("hggZWjj_PTjjZW4_tracks", &hggZWjj_PTjjZW4_tracks, &b_PTjjZW4_tracks);
   fChain->SetBranchAddress("hggZWjj_PTjjZW", &hggZWjj_PTjjZW, &b_PTjjZW);
   fChain->SetBranchAddress("hggZWjj_PTjjZW4", &hggZWjj_PTjjZW4, &b_PTjjZW4);
   fChain->SetBranchAddress("hggZWjj_ZW7Njet", &hggZWjj_ZW7Njet, &b_ZW7Njet);
   fChain->SetBranchAddress("hggZWjj_ZW4Njet", &hggZWjj_ZW4Njet, &b_ZW4Njet);
   fChain->SetBranchAddress("hggZWjj_SumTracksPT", &hggZWjj_SumTracksPT, &b_SumTracksPT);
   fChain->SetBranchAddress("hggZWjj_SumTracksPT4", &hggZWjj_SumTracksPT4, &b_SumTracksPT4);
   fChain->SetBranchAddress("hggZWjj_DeltaRphotons", &hggZWjj_DeltaRphotons, &b_DeltaRphotons);
   fChain->SetBranchAddress("hggZWjj_DeltaRjj", &hggZWjj_DeltaRjj, &b_DeltaRjj);
   fChain->SetBranchAddress("hggZWjj_PTjZW4Tracks", &hggZWjj_PTjZW4Tracks, &b_hggZWjj_PTjZW4Tracks);
   fChain->SetBranchAddress("hggZWjj_PTjZW4", &hggZWjj_PTjZW4, &b_hggZWjj_PTjZW4);
   fChain->SetBranchAddress("hggZWjj_PTjZWTracks", &hggZWjj_PTjZWTracks, &b_hggZWjj_PTjZWTracks);
   fChain->SetBranchAddress("hggZWjj_PTjZW", &hggZWjj_PTjZW, &b_hggZWjj_PTjZW);
   fChain->SetBranchAddress("hggZWjj_EtajZW4", &hggZWjj_EtajZW4, &b_hggZWjj_EtajZW4);
   fChain->SetBranchAddress("hggZWjj_EtajZW", &hggZWjj_EtajZW, &b_hggZWjj_EtajZW);
   fChain->SetBranchAddress("hgg_fit_eta_zcommon", &hgg_fit_eta_zcommon, &b_hgg_fit_eta_zcommon);
   fChain->SetBranchAddress("hgg_fit_eta_zcommon_primVtxLH", &hgg_fit_eta_zcommon_primVtxLH, &b_hgg_fit_eta_zcommon_primVtxLH);
   fChain->SetBranchAddress("hgg_fit_zcommon", &hgg_fit_zcommon, &b_hgg_fit_zCalo);
   fChain->SetBranchAddress("hgg_fit_zcommon_primVtxLH", &hgg_fit_zcommon_primVtxLH, &b_hgg_fit_zVtx);
   fChain->SetBranchAddress("hgg_fit_sigma_zcommon", &hgg_fit_sigma_zcommon, &b_hgg_fit_zCaloRes);
   fChain->SetBranchAddress("hgg_fit_sigma_zcommon_primVtxLH", &hgg_fit_sigma_zcommon_primVtxLH, &b_hgg_fit_zVtxRes);
   fChain->SetBranchAddress("hgg_fit_mgg", &hgg_fit_mgg, &b_hgg_fit_mgg);
   fChain->SetBranchAddress("hgg_fit_mgg_trueDir", &hgg_fit_mgg_trueDir, &b_hgg_fit_mgg_trueDir);
   fChain->SetBranchAddress("hgg_fit_mgg_trueE", &hgg_fit_mgg_trueE, &b_hgg_fit_mgg_trueE);
   fChain->SetBranchAddress("hgg_fit_fitFlag", &hgg_fit_fitFlag, &b_hgg_fit_fitFlag);
   fChain->SetBranchAddress("hgg_fit_vtxLikelihood", &hgg_fit_vtxLikelihood, &b_hgg_fit_vtxLikelihood);
   fChain->SetBranchAddress("hgg_fit_mgg_vec", &hgg_fit_mgg_vec, &b_hgg_fit_mgg_vec);
   fChain->SetBranchAddress("hgg_fit_ptgg", &hgg_fit_ptgg, &b_hgg_fit_ptgg);
   fChain->SetBranchAddress("hgg_fit_detagg", &hgg_fit_detagg, &b_hgg_fit_detagg);
   fChain->SetBranchAddress("hgg_fit_cosThetaStar", &hgg_fit_cosThetaStar, &b_hgg_fit_cosThetaStar);
   fChain->SetBranchAddress("hgg1jet_mgg", &hgg1jet_mgg, &b_hgg1jet_mgg);
   fChain->SetBranchAddress("hgg1jet_mggj", &hgg1jet_mggj, &b_hgg1jet_mggj);
   fChain->SetBranchAddress("hgg2jet_mgg", &hgg2jet_mgg, &b_hgg2jet_mgg);
   fChain->SetBranchAddress("hgg2jet_mjj", &hgg2jet_mjj, &b_hgg2jet_mjj);
   fChain->SetBranchAddress("ph_HPV_eta", &ph_HPV_eta, &b_ph_HPV_eta);
   fChain->SetBranchAddress("ph_HPV_sigma_eta", &ph_HPV_sigma_eta, &b_ph_HPV_sigma_eta);
   fChain->SetBranchAddress("hgg_HPV_zcommon", &hgg_HPV_zcommon, &b_hgg_HPV_zcommon);
   fChain->SetBranchAddress("hgg_HPV_zcommon_primVtxLH", &hgg_HPV_zcommon_primVtxLH, &b_hgg_HPV_zcommon_primVtxLH);
   fChain->SetBranchAddress("hgg_HPV_mgg", &hgg_HPV_mgg, &b_hgg_HPV_mgg);
   fChain->SetBranchAddress("hgg_HPV_mgg_primVtxLH", &hgg_HPV_mgg_primVtxLH, &b_hgg_HPV_mgg_primVtxLH);
   fChain->SetBranchAddress("hgg_HPV_ptgg", &hgg_HPV_ptgg, &b_hgg_HPV_ptgg);
   fChain->SetBranchAddress("hgg_HPV_detagg", &hgg_HPV_detagg, &b_hgg_HPV_detagg);
   fChain->SetBranchAddress("hgg_HPV_dphigg", &hgg_HPV_dphigg, &b_hgg_HPV_dphigg);
   fChain->SetBranchAddress("hgg_HPV_cosThetaStar", &hgg_HPV_cosThetaStar, &b_hgg_HPV_cosThetaStar);
   fChain->SetBranchAddress("hgg_HPV_cosThetaStarCS", &hgg_HPV_cosThetaStarCS, &b_hgg_HPV_cosThetaStarCS);
   fChain->SetBranchAddress("hgg_HPV_eta_zcommon", &hgg_HPV_eta_zcommon, &b_hgg_HPV_eta_zcommon);
   fChain->SetBranchAddress("hgg_HPV_eta_zcommon_primVtxLH", &hgg_HPV_eta_zcommon_primVtxLH, &b_hgg_HPV_eta_zcommon_primVtxLH);
   fChain->SetBranchAddress("hgg_HPV_vtxLikelihood", &hgg_HPV_vtxLikelihood, &b_hgg_HPV_vtxLikelihood);
   fChain->SetBranchAddress("hgg_HPV_mgg_Ztruth", &hgg_HPV_mgg_Ztruth, &b_hgg_HPV_mgg_Ztruth);
   fChain->SetBranchAddress("ph_HPV_convTrk1_zvertex", &ph_HPV_convTrk1_zvertex, &b_ph_HPV_convTrk1_zvertex);
   fChain->SetBranchAddress("ph_HPV_convTrk2_zvertex", &ph_HPV_convTrk2_zvertex, &b_ph_HPV_convTrk2_zvertex);
   fChain->SetBranchAddress("ph_HPV_convTrk1_sigma_zvertex", &ph_HPV_convTrk1_sigma_zvertex, &b_ph_HPV_convTrk1_sigma_zvertex);
   fChain->SetBranchAddress("ph_HPV_convTrk2_sigma_zvertex", &ph_HPV_convTrk2_sigma_zvertex, &b_ph_HPV_convTrk2_sigma_zvertex);
   fChain->SetBranchAddress("ph_HPV_zvertex", &ph_HPV_zvertex, &b_ph_HPV_zvertex);
   fChain->SetBranchAddress("ph_HPV_sigma_zvertex", &ph_HPV_sigma_zvertex, &b_ph_HPV_sigma_zvertex);
   fChain->SetBranchAddress("ph_HPV_zvertex_neutral", &ph_HPV_zvertex_neutral, &b_ph_HPV_zvertex_neutral);
   fChain->SetBranchAddress("ph_HPV_zvertex_charged", &ph_HPV_zvertex_charged, &b_ph_HPV_zvertex_charged);
   fChain->SetBranchAddress("ph_HPV_sigma_zvertex_neutral", &ph_HPV_sigma_zvertex_neutral, &b_ph_HPV_sigma_zvertex_neutral);
   fChain->SetBranchAddress("ph_HPV_sigma_zvertex_charged", &ph_HPV_sigma_zvertex_charged, &b_ph_HPV_sigma_zvertex_charged);
   fChain->SetBranchAddress("ph_E_clus", &ph_E_clus, &b_ph_E_clus);
   fChain->SetBranchAddress("ph_eta_clus", &ph_eta_clus, &b_ph_eta_clus);
   fChain->SetBranchAddress("ph_phi_clus", &ph_phi_clus, &b_ph_phi_clus);
   fChain->SetBranchAddress("ph_pt_clus", &ph_pt_clus, &b_ph_pt_clus);
   fChain->SetBranchAddress("testOleg_ph_particleType", &testOleg_ph_particleType, &b_testOleg_ph_particleType);
   fChain->SetBranchAddress("testOleg_ph_particleOrig", &testOleg_ph_particleOrig, &b_testOleg_ph_particleOrig);
   fChain->SetBranchAddress("clus_SW_eta", &clus_SW_eta, &b_clus_SW_eta);
   fChain->SetBranchAddress("clus_SW_phi", &clus_SW_phi, &b_clus_SW_phi);
   fChain->SetBranchAddress("clus_SW_E", &clus_SW_E, &b_clus_SW_E);
   fChain->SetBranchAddress("clus_SW_size", &clus_SW_size, &b_clus_SW_size);
   fChain->SetBranchAddress("clus_SW_etasize", &clus_SW_etasize, &b_clus_SW_etasize);
   fChain->SetBranchAddress("clus_SW_phisize", &clus_SW_phisize, &b_clus_SW_phisize);
   fChain->SetBranchAddress("clus_SW_time", &clus_SW_time, &b_clus_SW_time);
   fChain->SetBranchAddress("clus_EMtopo_eta", &clus_EMtopo_eta, &b_clus_EMtopo_eta);
   fChain->SetBranchAddress("clus_EMtopo_phi", &clus_EMtopo_phi, &b_clus_EMtopo_phi);
   fChain->SetBranchAddress("clus_EMtopo_E", &clus_EMtopo_E, &b_clus_EMtopo_E);
   fChain->SetBranchAddress("clus_EMtopo_time", &clus_EMtopo_time, &b_clus_EMtopo_time);
   fChain->SetBranchAddress("clus_EMtopo_trackIsol05", &clus_EMtopo_trackIsol05, &b_clus_EMtopo_trackIsol05);
   fChain->SetBranchAddress("clus_EMtopo_trackIsol07", &clus_EMtopo_trackIsol07, &b_clus_EMtopo_trackIsol07);
   fChain->SetBranchAddress("clus_EMtopo_trackIsol10", &clus_EMtopo_trackIsol10, &b_clus_EMtopo_trackIsol10);
   fChain->SetBranchAddress("clus_EMtopo_trackIsol20", &clus_EMtopo_trackIsol20, &b_clus_EMtopo_trackIsol20);
   fChain->SetBranchAddress("clus_EMtopo_CENTER_LAMBDA", &clus_EMtopo_CENTER_LAMBDA, &b_clus_EMtopo_CENTER_LAMBDA);
   fChain->SetBranchAddress("clus_EMtopo_LATERAL", &clus_EMtopo_LATERAL, &b_clus_EMtopo_LATERAL);
   fChain->SetBranchAddress("clus_EMtopo_LONGITUDINAL", &clus_EMtopo_LONGITUDINAL, &b_clus_EMtopo_LONGITUDINAL);
   fChain->SetBranchAddress("clus_EMtopo_ENG_FRAC_EM", &clus_EMtopo_ENG_FRAC_EM, &b_clus_EMtopo_ENG_FRAC_EM);
   fChain->SetBranchAddress("clus_EMtopo_ENG_FRAC_MAX", &clus_EMtopo_ENG_FRAC_MAX, &b_clus_EMtopo_ENG_FRAC_MAX);
   fChain->SetBranchAddress("clus_EMtopo_ENG_FRAC_CORE", &clus_EMtopo_ENG_FRAC_CORE, &b_clus_EMtopo_ENG_FRAC_CORE);
   fChain->SetBranchAddress("clus_EMtopo_FIRST_ENG_DENS", &clus_EMtopo_FIRST_ENG_DENS, &b_clus_EMtopo_FIRST_ENG_DENS);
   fChain->SetBranchAddress("clus_EMtopo_SECOND_R", &clus_EMtopo_SECOND_R, &b_clus_EMtopo_SECOND_R);
   fChain->SetBranchAddress("clus_EMtopo_SECOND_LAMBDA", &clus_EMtopo_SECOND_LAMBDA, &b_clus_EMtopo_SECOND_LAMBDA);
   fChain->SetBranchAddress("clus_EMtopo_SECOND_ENG_DENS", &clus_EMtopo_SECOND_ENG_DENS, &b_clus_EMtopo_SECOND_ENG_DENS);
   fChain->SetBranchAddress("clus_EMtopo_ISOLATION", &clus_EMtopo_ISOLATION, &b_clus_EMtopo_ISOLATION);
   fChain->SetBranchAddress("clus_EMtopo_EM_PROBABILITY", &clus_EMtopo_EM_PROBABILITY, &b_clus_EMtopo_EM_PROBABILITY);
   fChain->SetBranchAddress("clus_EMtopo_HAD_WEIGHT", &clus_EMtopo_HAD_WEIGHT, &b_clus_EMtopo_HAD_WEIGHT);
   fChain->SetBranchAddress("clus_EMtopo_OOC_WEIGHT", &clus_EMtopo_OOC_WEIGHT, &b_clus_EMtopo_OOC_WEIGHT);
   Notify();
}

Bool_t PAUisEMPerf::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void PAUisEMPerf::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t PAUisEMPerf::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef PAUisEMPerf_cxx
