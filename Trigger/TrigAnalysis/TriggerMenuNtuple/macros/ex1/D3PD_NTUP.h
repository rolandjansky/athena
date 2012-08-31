/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Mar 12 16:16:27 2010 by ROOT version 5.26/00b
// from TTree t/t
// found on file: d3pd_level1.root
//////////////////////////////////////////////////////////

#ifndef D3PD_NTUP_h
#define D3PD_NTUP_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class D3PD_NTUP {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           l2_emcl_n;
   vector<unsigned int> *l2_emcl_quality;
   vector<float>   *l2_emcl_energy;
   vector<float>   *l2_emcl_et;
   vector<float>   *l2_emcl_eta;
   vector<float>   *l2_emcl_phi;
   vector<float>   *l2_emcl_e237;
   vector<float>   *l2_emcl_e277;
   vector<float>   *l2_emcl_fracs1;
   vector<float>   *l2_emcl_weta2;
   vector<float>   *l2_emcl_ehad1;
   vector<float>   *l2_emcl_Eta1;
   vector<float>   *l2_emcl_emaxs1;
   vector<float>   *l2_emcl_e2tsts1;
   Int_t           l2_trk_idscan_eGamma_n;
   vector<int>     *l2_trk_idscan_eGamma_TrigL2Track_AlgorithmId;
   vector<float>   *l2_trk_idscan_eGamma_TrigL2Track_Chi2Ndof;
   vector<int>     *l2_trk_idscan_eGamma_TrigL2Track_NStrawHits;
   vector<int>     *l2_trk_idscan_eGamma_TrigL2Track_NStraw;
   vector<int>     *l2_trk_idscan_eGamma_TrigL2Track_NStrawTime;
   vector<int>     *l2_trk_idscan_eGamma_TrigL2Track_NHighThrHits;
   vector<int>     *l2_trk_idscan_eGamma_TrigL2Track_NPixelSpacePoints;
   vector<int>     *l2_trk_idscan_eGamma_TrigL2Track_NSCT_SpacePoints;
   vector<long>    *l2_trk_idscan_eGamma_TrigL2Track_HitPattern;
   vector<float>   *l2_trk_idscan_eGamma_TrigL2Track_a0;
   vector<float>   *l2_trk_idscan_eGamma_TrigL2Track_z0;
   vector<float>   *l2_trk_idscan_eGamma_TrigL2Track_phi0;
   vector<float>   *l2_trk_idscan_eGamma_TrigL2Track_eta;
   vector<float>   *l2_trk_idscan_eGamma_TrigL2Track_pT;
   vector<vector<float> > *l2_trk_idscan_eGamma_TrigL2Track_Covariance;
   vector<int>     *l2_trk_idscan_eGamma_TrigL2Track_TrackStatus;
   Int_t           l2_trk_sitrack_eGamma_n;
   vector<int>     *l2_trk_sitrack_eGamma_TrigL2Track_AlgorithmId;
   vector<float>   *l2_trk_sitrack_eGamma_TrigL2Track_Chi2Ndof;
   vector<int>     *l2_trk_sitrack_eGamma_TrigL2Track_NStrawHits;
   vector<int>     *l2_trk_sitrack_eGamma_TrigL2Track_NStraw;
   vector<int>     *l2_trk_sitrack_eGamma_TrigL2Track_NStrawTime;
   vector<int>     *l2_trk_sitrack_eGamma_TrigL2Track_NHighThrHits;
   vector<int>     *l2_trk_sitrack_eGamma_TrigL2Track_NPixelSpacePoints;
   vector<int>     *l2_trk_sitrack_eGamma_TrigL2Track_NSCT_SpacePoints;
   vector<long>    *l2_trk_sitrack_eGamma_TrigL2Track_HitPattern;
   vector<float>   *l2_trk_sitrack_eGamma_TrigL2Track_a0;
   vector<float>   *l2_trk_sitrack_eGamma_TrigL2Track_z0;
   vector<float>   *l2_trk_sitrack_eGamma_TrigL2Track_phi0;
   vector<float>   *l2_trk_sitrack_eGamma_TrigL2Track_eta;
   vector<float>   *l2_trk_sitrack_eGamma_TrigL2Track_pT;
   vector<vector<float> > *l2_trk_sitrack_eGamma_TrigL2Track_Covariance;
   vector<int>     *l2_trk_sitrack_eGamma_TrigL2Track_TrackStatus;
   Int_t           l2_el_n;
   vector<float>   *l2_el_E;
   vector<float>   *l2_el_Et;
   vector<float>   *l2_el_pt;
   vector<float>   *l2_el_m;
   vector<float>   *l2_el_eta;
   vector<float>   *l2_el_phi;
   vector<float>   *l2_el_px;
   vector<float>   *l2_el_py;
   vector<float>   *l2_el_pz;
   vector<int>     *l2_el_roiWord;
   vector<float>   *l2_el_Zvtx;
   vector<int>     *l2_el_charge;
   vector<int>     *l2_el_trackAlgo;
   vector<float>   *l2_el_nTRTHiTHitsRatio;
   vector<float>   *l2_el_DeltaEta1;
   vector<float>   *l2_el_DeltaPhi2;
   vector<float>   *l2_el_EtOverPt;
   vector<float>   *l2_el_Reta;
   vector<float>   *l2_el_Eratio;
   vector<float>   *l2_el_Ethad1;
   Int_t           l2_ph_n;
   vector<int>     *l2_ph_roiWord;
   vector<float>   *l2_ph_Et;
   vector<float>   *l2_ph_HadEt;
   vector<float>   *l2_ph_HadEt1;
   vector<float>   *l2_ph_energyRatio;
   vector<float>   *l2_ph_Eratio;
   vector<float>   *l2_ph_rCore;
   vector<float>   *l2_ph_Reta;
   vector<float>   *l2_ph_Fside;
   vector<float>   *l2_ph_Weta2;
   vector<float>   *l2_ph_dPhi;
   vector<float>   *l2_ph_dEta;
   vector<float>   *l2_ph_F0;
   vector<float>   *l2_ph_F1;
   vector<float>   *l2_ph_F2;
   vector<float>   *l2_ph_F3;
   vector<int>     *l2_ph_clusterIndex;
   Int_t           ef_emcl_n;
   vector<float>   *ef_emcl_E;
   vector<float>   *ef_emcl_pt;
   vector<float>   *ef_emcl_m;
   vector<float>   *ef_emcl_eta;
   vector<float>   *ef_emcl_phi;
   vector<float>   *ef_emcl_E_em;
   vector<float>   *ef_emcl_E_had;
   vector<float>   *ef_emcl_firstEdens;
   vector<float>   *ef_emcl_cellmaxfrac;
   vector<float>   *ef_emcl_longitudinal;
   vector<float>   *ef_emcl_secondlambda;
   vector<float>   *ef_emcl_lateral;
   vector<float>   *ef_emcl_secondR;
   vector<float>   *ef_emcl_centerlambda;
   vector<float>   *ef_emcl_deltaTheta;
   vector<float>   *ef_emcl_deltaPhi;
   vector<float>   *ef_emcl_time;
   Int_t           ef_emcl_slw_n;
   vector<float>   *ef_emcl_slw_E;
   vector<float>   *ef_emcl_slw_pt;
   vector<float>   *ef_emcl_slw_m;
   vector<float>   *ef_emcl_slw_eta;
   vector<float>   *ef_emcl_slw_phi;
   vector<float>   *ef_emcl_slw_E_em;
   vector<float>   *ef_emcl_slw_E_had;
   vector<float>   *ef_emcl_slw_firstEdens;
   vector<float>   *ef_emcl_slw_cellmaxfrac;
   vector<float>   *ef_emcl_slw_longitudinal;
   vector<float>   *ef_emcl_slw_secondlambda;
   vector<float>   *ef_emcl_slw_lateral;
   vector<float>   *ef_emcl_slw_secondR;
   vector<float>   *ef_emcl_slw_centerlambda;
   vector<float>   *ef_emcl_slw_deltaTheta;
   vector<float>   *ef_emcl_slw_deltaPhi;
   vector<float>   *ef_emcl_slw_time;
   Int_t           ef_el_n;
   vector<float>   *ef_el_E;
   vector<float>   *ef_el_Et;
   vector<float>   *ef_el_pt;
   vector<float>   *ef_el_m;
   vector<float>   *ef_el_eta;
   vector<float>   *ef_el_phi;
   vector<float>   *ef_el_px;
   vector<float>   *ef_el_py;
   vector<float>   *ef_el_pz;
   vector<float>   *ef_el_charge;
   vector<int>     *ef_el_author;
   vector<unsigned int> *ef_el_isEM;
   vector<int>     *ef_el_loose;
   vector<int>     *ef_el_medium;
   vector<int>     *ef_el_mediumIso;
   vector<int>     *ef_el_tight;
   vector<int>     *ef_el_tightIso;
   vector<float>   *ef_el_Ethad;
   vector<float>   *ef_el_Ethad1;
   vector<float>   *ef_el_f1;
   vector<float>   *ef_el_f1core;
   vector<float>   *ef_el_Emins1;
   vector<float>   *ef_el_fside;
   vector<float>   *ef_el_Emax2;
   vector<float>   *ef_el_ws3;
   vector<float>   *ef_el_wstot;
   vector<float>   *ef_el_emaxs1;
   vector<float>   *ef_el_deltaEs;
   vector<float>   *ef_el_E233;
   vector<float>   *ef_el_E237;
   vector<float>   *ef_el_E277;
   vector<float>   *ef_el_weta2;
   vector<float>   *ef_el_f3;
   vector<float>   *ef_el_f3core;
   vector<float>   *ef_el_rphiallcalo;
   vector<float>   *ef_el_Etcone45;
   vector<float>   *ef_el_Etcone20;
   vector<float>   *ef_el_Etcone30;
   vector<float>   *ef_el_Etcone40;
   vector<float>   *ef_el_pos7;
   vector<float>   *ef_el_etacorrmag;
   vector<float>   *ef_el_deltaeta1;
   vector<float>   *ef_el_deltaeta2;
   vector<float>   *ef_el_deltaphi2;
   vector<float>   *ef_el_reta;
   vector<float>   *ef_el_rphi;
   Int_t           trigNavi_n;
   vector<int>     *trigNavi_chain_ChainId;
   vector<vector<int> > *trigNavi_chain_RoIType;
   vector<vector<int> > *trigNavi_chain_RoIIndex;
   Int_t           RoI_L2_e_n;
   vector<int>     *RoI_L2_e_type;
   vector<int>     *RoI_L2_e_lastStep;
   vector<int>     *RoI_L2_e_TrigEMCluster;
   vector<int>     *RoI_L2_e_TrigEMClusterStatus;
   vector<int>     *RoI_L2_e_EmTau_ROI;
   vector<int>     *RoI_L2_e_EmTau_ROIStatus;
   vector<vector<int> > *RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma;
   vector<vector<int> > *RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus;
   vector<vector<int> > *RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma;
   vector<vector<int> > *RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus;
   vector<vector<int> > *RoI_L2_e_TrigElectronContainer;
   vector<vector<int> > *RoI_L2_e_TrigElectronContainerStatus;
   Int_t           RoI_EF_e_n;
   vector<int>     *RoI_EF_e_type;
   vector<int>     *RoI_EF_e_lastStep;
   vector<int>     *RoI_EF_e_EmTau_ROI;
   vector<int>     *RoI_EF_e_EmTau_ROIStatus;

   // List of branches
   TBranch        *b_l2_emcl_n;   //!
   TBranch        *b_l2_emcl_quality;   //!
   TBranch        *b_l2_emcl_energy;   //!
   TBranch        *b_l2_emcl_et;   //!
   TBranch        *b_l2_emcl_eta;   //!
   TBranch        *b_l2_emcl_phi;   //!
   TBranch        *b_l2_emcl_e237;   //!
   TBranch        *b_l2_emcl_e277;   //!
   TBranch        *b_l2_emcl_fracs1;   //!
   TBranch        *b_l2_emcl_weta2;   //!
   TBranch        *b_l2_emcl_ehad1;   //!
   TBranch        *b_l2_emcl_Eta1;   //!
   TBranch        *b_l2_emcl_emaxs1;   //!
   TBranch        *b_l2_emcl_e2tsts1;   //!
   TBranch        *b_l2_trk_idscan_eGamma_n;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_AlgorithmId;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_Chi2Ndof;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_NStrawHits;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_NStraw;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_NStrawTime;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_NHighThrHits;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_NPixelSpacePoints;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_NSCT_SpacePoints;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_HitPattern;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_a0;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_z0;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_phi0;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_eta;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_pT;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_Covariance;   //!
   TBranch        *b_l2_trk_idscan_eGamma_TrigL2Track_TrackStatus;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_n;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_AlgorithmId;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_Chi2Ndof;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_NStrawHits;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_NStraw;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_NStrawTime;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_NHighThrHits;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_NPixelSpacePoints;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_NSCT_SpacePoints;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_HitPattern;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_a0;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_z0;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_phi0;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_eta;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_pT;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_Covariance;   //!
   TBranch        *b_l2_trk_sitrack_eGamma_TrigL2Track_TrackStatus;   //!
   TBranch        *b_l2_el_n;   //!
   TBranch        *b_l2_el_E;   //!
   TBranch        *b_l2_el_Et;   //!
   TBranch        *b_l2_el_pt;   //!
   TBranch        *b_l2_el_m;   //!
   TBranch        *b_l2_el_eta;   //!
   TBranch        *b_l2_el_phi;   //!
   TBranch        *b_l2_el_px;   //!
   TBranch        *b_l2_el_py;   //!
   TBranch        *b_l2_el_pz;   //!
   TBranch        *b_l2_el_roiWord;   //!
   TBranch        *b_l2_el_Zvtx;   //!
   TBranch        *b_l2_el_charge;   //!
   TBranch        *b_l2_el_trackAlgo;   //!
   TBranch        *b_l2_el_nTRTHiTHitsRatio;   //!
   TBranch        *b_l2_el_DeltaEta1;   //!
   TBranch        *b_l2_el_DeltaPhi2;   //!
   TBranch        *b_l2_el_EtOverPt;   //!
   TBranch        *b_l2_el_Reta;   //!
   TBranch        *b_l2_el_Eratio;   //!
   TBranch        *b_l2_el_Ethad1;   //!
   TBranch        *b_l2_ph_n;   //!
   TBranch        *b_l2_ph_roiWord;   //!
   TBranch        *b_l2_ph_Et;   //!
   TBranch        *b_l2_ph_HadEt;   //!
   TBranch        *b_l2_ph_HadEt1;   //!
   TBranch        *b_l2_ph_energyRatio;   //!
   TBranch        *b_l2_ph_Eratio;   //!
   TBranch        *b_l2_ph_rCore;   //!
   TBranch        *b_l2_ph_Reta;   //!
   TBranch        *b_l2_ph_Fside;   //!
   TBranch        *b_l2_ph_Weta2;   //!
   TBranch        *b_l2_ph_dPhi;   //!
   TBranch        *b_l2_ph_dEta;   //!
   TBranch        *b_l2_ph_F0;   //!
   TBranch        *b_l2_ph_F1;   //!
   TBranch        *b_l2_ph_F2;   //!
   TBranch        *b_l2_ph_F3;   //!
   TBranch        *b_l2_ph_clusterIndex;   //!
   TBranch        *b_ef_emcl_n;   //!
   TBranch        *b_ef_emcl_E;   //!
   TBranch        *b_ef_emcl_pt;   //!
   TBranch        *b_ef_emcl_m;   //!
   TBranch        *b_ef_emcl_eta;   //!
   TBranch        *b_ef_emcl_phi;   //!
   TBranch        *b_ef_emcl_E_em;   //!
   TBranch        *b_ef_emcl_E_had;   //!
   TBranch        *b_ef_emcl_firstEdens;   //!
   TBranch        *b_ef_emcl_cellmaxfrac;   //!
   TBranch        *b_ef_emcl_longitudinal;   //!
   TBranch        *b_ef_emcl_secondlambda;   //!
   TBranch        *b_ef_emcl_lateral;   //!
   TBranch        *b_ef_emcl_secondR;   //!
   TBranch        *b_ef_emcl_centerlambda;   //!
   TBranch        *b_ef_emcl_deltaTheta;   //!
   TBranch        *b_ef_emcl_deltaPhi;   //!
   TBranch        *b_ef_emcl_time;   //!
   TBranch        *b_ef_emcl_slw_n;   //!
   TBranch        *b_ef_emcl_slw_E;   //!
   TBranch        *b_ef_emcl_slw_pt;   //!
   TBranch        *b_ef_emcl_slw_m;   //!
   TBranch        *b_ef_emcl_slw_eta;   //!
   TBranch        *b_ef_emcl_slw_phi;   //!
   TBranch        *b_ef_emcl_slw_E_em;   //!
   TBranch        *b_ef_emcl_slw_E_had;   //!
   TBranch        *b_ef_emcl_slw_firstEdens;   //!
   TBranch        *b_ef_emcl_slw_cellmaxfrac;   //!
   TBranch        *b_ef_emcl_slw_longitudinal;   //!
   TBranch        *b_ef_emcl_slw_secondlambda;   //!
   TBranch        *b_ef_emcl_slw_lateral;   //!
   TBranch        *b_ef_emcl_slw_secondR;   //!
   TBranch        *b_ef_emcl_slw_centerlambda;   //!
   TBranch        *b_ef_emcl_slw_deltaTheta;   //!
   TBranch        *b_ef_emcl_slw_deltaPhi;   //!
   TBranch        *b_ef_emcl_slw_time;   //!
   TBranch        *b_ef_el_n;   //!
   TBranch        *b_ef_el_E;   //!
   TBranch        *b_ef_el_Et;   //!
   TBranch        *b_ef_el_pt;   //!
   TBranch        *b_ef_el_m;   //!
   TBranch        *b_ef_el_eta;   //!
   TBranch        *b_ef_el_phi;   //!
   TBranch        *b_ef_el_px;   //!
   TBranch        *b_ef_el_py;   //!
   TBranch        *b_ef_el_pz;   //!
   TBranch        *b_ef_el_charge;   //!
   TBranch        *b_ef_el_author;   //!
   TBranch        *b_ef_el_isEM;   //!
   TBranch        *b_ef_el_loose;   //!
   TBranch        *b_ef_el_medium;   //!
   TBranch        *b_ef_el_mediumIso;   //!
   TBranch        *b_ef_el_tight;   //!
   TBranch        *b_ef_el_tightIso;   //!
   TBranch        *b_ef_el_Ethad;   //!
   TBranch        *b_ef_el_Ethad1;   //!
   TBranch        *b_ef_el_f1;   //!
   TBranch        *b_ef_el_f1core;   //!
   TBranch        *b_ef_el_Emins1;   //!
   TBranch        *b_ef_el_fside;   //!
   TBranch        *b_ef_el_Emax2;   //!
   TBranch        *b_ef_el_ws3;   //!
   TBranch        *b_ef_el_wstot;   //!
   TBranch        *b_ef_el_emaxs1;   //!
   TBranch        *b_ef_el_deltaEs;   //!
   TBranch        *b_ef_el_E233;   //!
   TBranch        *b_ef_el_E237;   //!
   TBranch        *b_ef_el_E277;   //!
   TBranch        *b_ef_el_weta2;   //!
   TBranch        *b_ef_el_f3;   //!
   TBranch        *b_ef_el_f3core;   //!
   TBranch        *b_ef_el_rphiallcalo;   //!
   TBranch        *b_ef_el_Etcone45;   //!
   TBranch        *b_ef_el_Etcone20;   //!
   TBranch        *b_ef_el_Etcone30;   //!
   TBranch        *b_ef_el_Etcone40;   //!
   TBranch        *b_ef_el_pos7;   //!
   TBranch        *b_ef_el_etacorrmag;   //!
   TBranch        *b_ef_el_deltaeta1;   //!
   TBranch        *b_ef_el_deltaeta2;   //!
   TBranch        *b_ef_el_deltaphi2;   //!
   TBranch        *b_ef_el_reta;   //!
   TBranch        *b_ef_el_rphi;   //!
   TBranch        *b_trigNavi_n;   //!
   TBranch        *b_trigNavi_chain_ChainId;   //!
   TBranch        *b_trigNavi_chain_RoIType;   //!
   TBranch        *b_trigNavi_chain_RoIIndex;   //!
   TBranch        *b_RoI_L2_e_n;   //!
   TBranch        *b_RoI_L2_e_type;   //!
   TBranch        *b_RoI_L2_e_lastStep;   //!
   TBranch        *b_RoI_L2_e_TrigEMCluster;   //!
   TBranch        *b_RoI_L2_e_TrigEMClusterStatus;   //!
   TBranch        *b_RoI_L2_e_EmTau_ROI;   //!
   TBranch        *b_RoI_L2_e_EmTau_ROIStatus;   //!
   TBranch        *b_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma;   //!
   TBranch        *b_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus;   //!
   TBranch        *b_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma;   //!
   TBranch        *b_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus;   //!
   TBranch        *b_RoI_L2_e_TrigElectronContainer;   //!
   TBranch        *b_RoI_L2_e_TrigElectronContainerStatus;   //!
   TBranch        *b_RoI_EF_e_n;   //!
   TBranch        *b_RoI_EF_e_type;   //!
   TBranch        *b_RoI_EF_e_lastStep;   //!
   TBranch        *b_RoI_EF_e_EmTau_ROI;   //!
   TBranch        *b_RoI_EF_e_EmTau_ROIStatus;   //!

   D3PD_NTUP(TTree *tree=0);
   virtual ~D3PD_NTUP();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef D3PD_NTUP_cxx
D3PD_NTUP::D3PD_NTUP(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("d3pd_level1.root");
      if (!f) {
         f = new TFile("d3pd_level1.root");
      }
      tree = (TTree*)gDirectory->Get("t");

   }
   Init(tree);
}

D3PD_NTUP::~D3PD_NTUP()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t D3PD_NTUP::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t D3PD_NTUP::LoadTree(Long64_t entry)
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

void D3PD_NTUP::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   l2_emcl_quality = 0;
   l2_emcl_energy = 0;
   l2_emcl_et = 0;
   l2_emcl_eta = 0;
   l2_emcl_phi = 0;
   l2_emcl_e237 = 0;
   l2_emcl_e277 = 0;
   l2_emcl_fracs1 = 0;
   l2_emcl_weta2 = 0;
   l2_emcl_ehad1 = 0;
   l2_emcl_Eta1 = 0;
   l2_emcl_emaxs1 = 0;
   l2_emcl_e2tsts1 = 0;
   l2_trk_idscan_eGamma_TrigL2Track_AlgorithmId = 0;
   l2_trk_idscan_eGamma_TrigL2Track_Chi2Ndof = 0;
   l2_trk_idscan_eGamma_TrigL2Track_NStrawHits = 0;
   l2_trk_idscan_eGamma_TrigL2Track_NStraw = 0;
   l2_trk_idscan_eGamma_TrigL2Track_NStrawTime = 0;
   l2_trk_idscan_eGamma_TrigL2Track_NHighThrHits = 0;
   l2_trk_idscan_eGamma_TrigL2Track_NPixelSpacePoints = 0;
   l2_trk_idscan_eGamma_TrigL2Track_NSCT_SpacePoints = 0;
   l2_trk_idscan_eGamma_TrigL2Track_HitPattern = 0;
   l2_trk_idscan_eGamma_TrigL2Track_a0 = 0;
   l2_trk_idscan_eGamma_TrigL2Track_z0 = 0;
   l2_trk_idscan_eGamma_TrigL2Track_phi0 = 0;
   l2_trk_idscan_eGamma_TrigL2Track_eta = 0;
   l2_trk_idscan_eGamma_TrigL2Track_pT = 0;
   l2_trk_idscan_eGamma_TrigL2Track_Covariance = 0;
   l2_trk_idscan_eGamma_TrigL2Track_TrackStatus = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_AlgorithmId = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_Chi2Ndof = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_NStrawHits = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_NStraw = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_NStrawTime = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_NHighThrHits = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_NPixelSpacePoints = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_NSCT_SpacePoints = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_HitPattern = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_a0 = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_z0 = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_phi0 = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_eta = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_pT = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_Covariance = 0;
   l2_trk_sitrack_eGamma_TrigL2Track_TrackStatus = 0;
   l2_el_E = 0;
   l2_el_Et = 0;
   l2_el_pt = 0;
   l2_el_m = 0;
   l2_el_eta = 0;
   l2_el_phi = 0;
   l2_el_px = 0;
   l2_el_py = 0;
   l2_el_pz = 0;
   l2_el_roiWord = 0;
   l2_el_Zvtx = 0;
   l2_el_charge = 0;
   l2_el_trackAlgo = 0;
   l2_el_nTRTHiTHitsRatio = 0;
   l2_el_DeltaEta1 = 0;
   l2_el_DeltaPhi2 = 0;
   l2_el_EtOverPt = 0;
   l2_el_Reta = 0;
   l2_el_Eratio = 0;
   l2_el_Ethad1 = 0;
   l2_ph_roiWord = 0;
   l2_ph_Et = 0;
   l2_ph_HadEt = 0;
   l2_ph_HadEt1 = 0;
   l2_ph_energyRatio = 0;
   l2_ph_Eratio = 0;
   l2_ph_rCore = 0;
   l2_ph_Reta = 0;
   l2_ph_Fside = 0;
   l2_ph_Weta2 = 0;
   l2_ph_dPhi = 0;
   l2_ph_dEta = 0;
   l2_ph_F0 = 0;
   l2_ph_F1 = 0;
   l2_ph_F2 = 0;
   l2_ph_F3 = 0;
   l2_ph_clusterIndex = 0;
   ef_emcl_E = 0;
   ef_emcl_pt = 0;
   ef_emcl_m = 0;
   ef_emcl_eta = 0;
   ef_emcl_phi = 0;
   ef_emcl_E_em = 0;
   ef_emcl_E_had = 0;
   ef_emcl_firstEdens = 0;
   ef_emcl_cellmaxfrac = 0;
   ef_emcl_longitudinal = 0;
   ef_emcl_secondlambda = 0;
   ef_emcl_lateral = 0;
   ef_emcl_secondR = 0;
   ef_emcl_centerlambda = 0;
   ef_emcl_deltaTheta = 0;
   ef_emcl_deltaPhi = 0;
   ef_emcl_time = 0;
   ef_emcl_slw_E = 0;
   ef_emcl_slw_pt = 0;
   ef_emcl_slw_m = 0;
   ef_emcl_slw_eta = 0;
   ef_emcl_slw_phi = 0;
   ef_emcl_slw_E_em = 0;
   ef_emcl_slw_E_had = 0;
   ef_emcl_slw_firstEdens = 0;
   ef_emcl_slw_cellmaxfrac = 0;
   ef_emcl_slw_longitudinal = 0;
   ef_emcl_slw_secondlambda = 0;
   ef_emcl_slw_lateral = 0;
   ef_emcl_slw_secondR = 0;
   ef_emcl_slw_centerlambda = 0;
   ef_emcl_slw_deltaTheta = 0;
   ef_emcl_slw_deltaPhi = 0;
   ef_emcl_slw_time = 0;
   ef_el_E = 0;
   ef_el_Et = 0;
   ef_el_pt = 0;
   ef_el_m = 0;
   ef_el_eta = 0;
   ef_el_phi = 0;
   ef_el_px = 0;
   ef_el_py = 0;
   ef_el_pz = 0;
   ef_el_charge = 0;
   ef_el_author = 0;
   ef_el_isEM = 0;
   ef_el_loose = 0;
   ef_el_medium = 0;
   ef_el_mediumIso = 0;
   ef_el_tight = 0;
   ef_el_tightIso = 0;
   ef_el_Ethad = 0;
   ef_el_Ethad1 = 0;
   ef_el_f1 = 0;
   ef_el_f1core = 0;
   ef_el_Emins1 = 0;
   ef_el_fside = 0;
   ef_el_Emax2 = 0;
   ef_el_ws3 = 0;
   ef_el_wstot = 0;
   ef_el_emaxs1 = 0;
   ef_el_deltaEs = 0;
   ef_el_E233 = 0;
   ef_el_E237 = 0;
   ef_el_E277 = 0;
   ef_el_weta2 = 0;
   ef_el_f3 = 0;
   ef_el_f3core = 0;
   ef_el_rphiallcalo = 0;
   ef_el_Etcone45 = 0;
   ef_el_Etcone20 = 0;
   ef_el_Etcone30 = 0;
   ef_el_Etcone40 = 0;
   ef_el_pos7 = 0;
   ef_el_etacorrmag = 0;
   ef_el_deltaeta1 = 0;
   ef_el_deltaeta2 = 0;
   ef_el_deltaphi2 = 0;
   ef_el_reta = 0;
   ef_el_rphi = 0;
   trigNavi_chain_ChainId = 0;
   trigNavi_chain_RoIType = 0;
   trigNavi_chain_RoIIndex = 0;
   RoI_L2_e_type = 0;
   RoI_L2_e_lastStep = 0;
   RoI_L2_e_TrigEMCluster = 0;
   RoI_L2_e_TrigEMClusterStatus = 0;
   RoI_L2_e_EmTau_ROI = 0;
   RoI_L2_e_EmTau_ROIStatus = 0;
   RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma = 0;
   RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus = 0;
   RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma = 0;
   RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus = 0;
   RoI_L2_e_TrigElectronContainer = 0;
   RoI_L2_e_TrigElectronContainerStatus = 0;
   RoI_EF_e_type = 0;
   RoI_EF_e_lastStep = 0;
   RoI_EF_e_EmTau_ROI = 0;
   RoI_EF_e_EmTau_ROIStatus = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("l2_emcl_n", &l2_emcl_n, &b_l2_emcl_n);
   fChain->SetBranchAddress("l2_emcl_quality", &l2_emcl_quality, &b_l2_emcl_quality);
   fChain->SetBranchAddress("l2_emcl_energy", &l2_emcl_energy, &b_l2_emcl_energy);
   fChain->SetBranchAddress("l2_emcl_et", &l2_emcl_et, &b_l2_emcl_et);
   fChain->SetBranchAddress("l2_emcl_eta", &l2_emcl_eta, &b_l2_emcl_eta);
   fChain->SetBranchAddress("l2_emcl_phi", &l2_emcl_phi, &b_l2_emcl_phi);
   fChain->SetBranchAddress("l2_emcl_e237", &l2_emcl_e237, &b_l2_emcl_e237);
   fChain->SetBranchAddress("l2_emcl_e277", &l2_emcl_e277, &b_l2_emcl_e277);
   fChain->SetBranchAddress("l2_emcl_fracs1", &l2_emcl_fracs1, &b_l2_emcl_fracs1);
   fChain->SetBranchAddress("l2_emcl_weta2", &l2_emcl_weta2, &b_l2_emcl_weta2);
   fChain->SetBranchAddress("l2_emcl_ehad1", &l2_emcl_ehad1, &b_l2_emcl_ehad1);
   fChain->SetBranchAddress("l2_emcl_Eta1", &l2_emcl_Eta1, &b_l2_emcl_Eta1);
   fChain->SetBranchAddress("l2_emcl_emaxs1", &l2_emcl_emaxs1, &b_l2_emcl_emaxs1);
   fChain->SetBranchAddress("l2_emcl_e2tsts1", &l2_emcl_e2tsts1, &b_l2_emcl_e2tsts1);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_n", &l2_trk_idscan_eGamma_n, &b_l2_trk_idscan_eGamma_n);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_AlgorithmId", &l2_trk_idscan_eGamma_TrigL2Track_AlgorithmId, &b_l2_trk_idscan_eGamma_TrigL2Track_AlgorithmId);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_Chi2Ndof", &l2_trk_idscan_eGamma_TrigL2Track_Chi2Ndof, &b_l2_trk_idscan_eGamma_TrigL2Track_Chi2Ndof);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_NStrawHits", &l2_trk_idscan_eGamma_TrigL2Track_NStrawHits, &b_l2_trk_idscan_eGamma_TrigL2Track_NStrawHits);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_NStraw", &l2_trk_idscan_eGamma_TrigL2Track_NStraw, &b_l2_trk_idscan_eGamma_TrigL2Track_NStraw);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_NStrawTime", &l2_trk_idscan_eGamma_TrigL2Track_NStrawTime, &b_l2_trk_idscan_eGamma_TrigL2Track_NStrawTime);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_NHighThrHits", &l2_trk_idscan_eGamma_TrigL2Track_NHighThrHits, &b_l2_trk_idscan_eGamma_TrigL2Track_NHighThrHits);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_NPixelSpacePoints", &l2_trk_idscan_eGamma_TrigL2Track_NPixelSpacePoints, &b_l2_trk_idscan_eGamma_TrigL2Track_NPixelSpacePoints);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_NSCT_SpacePoints", &l2_trk_idscan_eGamma_TrigL2Track_NSCT_SpacePoints, &b_l2_trk_idscan_eGamma_TrigL2Track_NSCT_SpacePoints);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_HitPattern", &l2_trk_idscan_eGamma_TrigL2Track_HitPattern, &b_l2_trk_idscan_eGamma_TrigL2Track_HitPattern);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_a0", &l2_trk_idscan_eGamma_TrigL2Track_a0, &b_l2_trk_idscan_eGamma_TrigL2Track_a0);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_z0", &l2_trk_idscan_eGamma_TrigL2Track_z0, &b_l2_trk_idscan_eGamma_TrigL2Track_z0);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_phi0", &l2_trk_idscan_eGamma_TrigL2Track_phi0, &b_l2_trk_idscan_eGamma_TrigL2Track_phi0);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_eta", &l2_trk_idscan_eGamma_TrigL2Track_eta, &b_l2_trk_idscan_eGamma_TrigL2Track_eta);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_pT", &l2_trk_idscan_eGamma_TrigL2Track_pT, &b_l2_trk_idscan_eGamma_TrigL2Track_pT);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_Covariance", &l2_trk_idscan_eGamma_TrigL2Track_Covariance, &b_l2_trk_idscan_eGamma_TrigL2Track_Covariance);
   fChain->SetBranchAddress("l2_trk_idscan_eGamma_TrigL2Track_TrackStatus", &l2_trk_idscan_eGamma_TrigL2Track_TrackStatus, &b_l2_trk_idscan_eGamma_TrigL2Track_TrackStatus);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_n", &l2_trk_sitrack_eGamma_n, &b_l2_trk_sitrack_eGamma_n);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_AlgorithmId", &l2_trk_sitrack_eGamma_TrigL2Track_AlgorithmId, &b_l2_trk_sitrack_eGamma_TrigL2Track_AlgorithmId);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_Chi2Ndof", &l2_trk_sitrack_eGamma_TrigL2Track_Chi2Ndof, &b_l2_trk_sitrack_eGamma_TrigL2Track_Chi2Ndof);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_NStrawHits", &l2_trk_sitrack_eGamma_TrigL2Track_NStrawHits, &b_l2_trk_sitrack_eGamma_TrigL2Track_NStrawHits);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_NStraw", &l2_trk_sitrack_eGamma_TrigL2Track_NStraw, &b_l2_trk_sitrack_eGamma_TrigL2Track_NStraw);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_NStrawTime", &l2_trk_sitrack_eGamma_TrigL2Track_NStrawTime, &b_l2_trk_sitrack_eGamma_TrigL2Track_NStrawTime);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_NHighThrHits", &l2_trk_sitrack_eGamma_TrigL2Track_NHighThrHits, &b_l2_trk_sitrack_eGamma_TrigL2Track_NHighThrHits);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_NPixelSpacePoints", &l2_trk_sitrack_eGamma_TrigL2Track_NPixelSpacePoints, &b_l2_trk_sitrack_eGamma_TrigL2Track_NPixelSpacePoints);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_NSCT_SpacePoints", &l2_trk_sitrack_eGamma_TrigL2Track_NSCT_SpacePoints, &b_l2_trk_sitrack_eGamma_TrigL2Track_NSCT_SpacePoints);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_HitPattern", &l2_trk_sitrack_eGamma_TrigL2Track_HitPattern, &b_l2_trk_sitrack_eGamma_TrigL2Track_HitPattern);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_a0", &l2_trk_sitrack_eGamma_TrigL2Track_a0, &b_l2_trk_sitrack_eGamma_TrigL2Track_a0);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_z0", &l2_trk_sitrack_eGamma_TrigL2Track_z0, &b_l2_trk_sitrack_eGamma_TrigL2Track_z0);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_phi0", &l2_trk_sitrack_eGamma_TrigL2Track_phi0, &b_l2_trk_sitrack_eGamma_TrigL2Track_phi0);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_eta", &l2_trk_sitrack_eGamma_TrigL2Track_eta, &b_l2_trk_sitrack_eGamma_TrigL2Track_eta);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_pT", &l2_trk_sitrack_eGamma_TrigL2Track_pT, &b_l2_trk_sitrack_eGamma_TrigL2Track_pT);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_Covariance", &l2_trk_sitrack_eGamma_TrigL2Track_Covariance, &b_l2_trk_sitrack_eGamma_TrigL2Track_Covariance);
   fChain->SetBranchAddress("l2_trk_sitrack_eGamma_TrigL2Track_TrackStatus", &l2_trk_sitrack_eGamma_TrigL2Track_TrackStatus, &b_l2_trk_sitrack_eGamma_TrigL2Track_TrackStatus);
   fChain->SetBranchAddress("l2_el_n", &l2_el_n, &b_l2_el_n);
   fChain->SetBranchAddress("l2_el_E", &l2_el_E, &b_l2_el_E);
   fChain->SetBranchAddress("l2_el_Et", &l2_el_Et, &b_l2_el_Et);
   fChain->SetBranchAddress("l2_el_pt", &l2_el_pt, &b_l2_el_pt);
   fChain->SetBranchAddress("l2_el_m", &l2_el_m, &b_l2_el_m);
   fChain->SetBranchAddress("l2_el_eta", &l2_el_eta, &b_l2_el_eta);
   fChain->SetBranchAddress("l2_el_phi", &l2_el_phi, &b_l2_el_phi);
   fChain->SetBranchAddress("l2_el_px", &l2_el_px, &b_l2_el_px);
   fChain->SetBranchAddress("l2_el_py", &l2_el_py, &b_l2_el_py);
   fChain->SetBranchAddress("l2_el_pz", &l2_el_pz, &b_l2_el_pz);
   fChain->SetBranchAddress("l2_el_roiWord", &l2_el_roiWord, &b_l2_el_roiWord);
   fChain->SetBranchAddress("l2_el_Zvtx", &l2_el_Zvtx, &b_l2_el_Zvtx);
   fChain->SetBranchAddress("l2_el_charge", &l2_el_charge, &b_l2_el_charge);
   fChain->SetBranchAddress("l2_el_trackAlgo", &l2_el_trackAlgo, &b_l2_el_trackAlgo);
   fChain->SetBranchAddress("l2_el_nTRTHiTHitsRatio", &l2_el_nTRTHiTHitsRatio, &b_l2_el_nTRTHiTHitsRatio);
   fChain->SetBranchAddress("l2_el_DeltaEta1", &l2_el_DeltaEta1, &b_l2_el_DeltaEta1);
   fChain->SetBranchAddress("l2_el_DeltaPhi2", &l2_el_DeltaPhi2, &b_l2_el_DeltaPhi2);
   fChain->SetBranchAddress("l2_el_EtOverPt", &l2_el_EtOverPt, &b_l2_el_EtOverPt);
   fChain->SetBranchAddress("l2_el_Reta", &l2_el_Reta, &b_l2_el_Reta);
   fChain->SetBranchAddress("l2_el_Eratio", &l2_el_Eratio, &b_l2_el_Eratio);
   fChain->SetBranchAddress("l2_el_Ethad1", &l2_el_Ethad1, &b_l2_el_Ethad1);
   fChain->SetBranchAddress("l2_ph_n", &l2_ph_n, &b_l2_ph_n);
   fChain->SetBranchAddress("l2_ph_roiWord", &l2_ph_roiWord, &b_l2_ph_roiWord);
   fChain->SetBranchAddress("l2_ph_Et", &l2_ph_Et, &b_l2_ph_Et);
   fChain->SetBranchAddress("l2_ph_HadEt", &l2_ph_HadEt, &b_l2_ph_HadEt);
   fChain->SetBranchAddress("l2_ph_HadEt1", &l2_ph_HadEt1, &b_l2_ph_HadEt1);
   fChain->SetBranchAddress("l2_ph_energyRatio", &l2_ph_energyRatio, &b_l2_ph_energyRatio);
   fChain->SetBranchAddress("l2_ph_Eratio", &l2_ph_Eratio, &b_l2_ph_Eratio);
   fChain->SetBranchAddress("l2_ph_rCore", &l2_ph_rCore, &b_l2_ph_rCore);
   fChain->SetBranchAddress("l2_ph_Reta", &l2_ph_Reta, &b_l2_ph_Reta);
   fChain->SetBranchAddress("l2_ph_Fside", &l2_ph_Fside, &b_l2_ph_Fside);
   fChain->SetBranchAddress("l2_ph_Weta2", &l2_ph_Weta2, &b_l2_ph_Weta2);
   fChain->SetBranchAddress("l2_ph_dPhi", &l2_ph_dPhi, &b_l2_ph_dPhi);
   fChain->SetBranchAddress("l2_ph_dEta", &l2_ph_dEta, &b_l2_ph_dEta);
   fChain->SetBranchAddress("l2_ph_F0", &l2_ph_F0, &b_l2_ph_F0);
   fChain->SetBranchAddress("l2_ph_F1", &l2_ph_F1, &b_l2_ph_F1);
   fChain->SetBranchAddress("l2_ph_F2", &l2_ph_F2, &b_l2_ph_F2);
   fChain->SetBranchAddress("l2_ph_F3", &l2_ph_F3, &b_l2_ph_F3);
   fChain->SetBranchAddress("l2_ph_clusterIndex", &l2_ph_clusterIndex, &b_l2_ph_clusterIndex);
   fChain->SetBranchAddress("ef_emcl_n", &ef_emcl_n, &b_ef_emcl_n);
   fChain->SetBranchAddress("ef_emcl_E", &ef_emcl_E, &b_ef_emcl_E);
   fChain->SetBranchAddress("ef_emcl_pt", &ef_emcl_pt, &b_ef_emcl_pt);
   fChain->SetBranchAddress("ef_emcl_m", &ef_emcl_m, &b_ef_emcl_m);
   fChain->SetBranchAddress("ef_emcl_eta", &ef_emcl_eta, &b_ef_emcl_eta);
   fChain->SetBranchAddress("ef_emcl_phi", &ef_emcl_phi, &b_ef_emcl_phi);
   fChain->SetBranchAddress("ef_emcl_E_em", &ef_emcl_E_em, &b_ef_emcl_E_em);
   fChain->SetBranchAddress("ef_emcl_E_had", &ef_emcl_E_had, &b_ef_emcl_E_had);
   fChain->SetBranchAddress("ef_emcl_firstEdens", &ef_emcl_firstEdens, &b_ef_emcl_firstEdens);
   fChain->SetBranchAddress("ef_emcl_cellmaxfrac", &ef_emcl_cellmaxfrac, &b_ef_emcl_cellmaxfrac);
   fChain->SetBranchAddress("ef_emcl_longitudinal", &ef_emcl_longitudinal, &b_ef_emcl_longitudinal);
   fChain->SetBranchAddress("ef_emcl_secondlambda", &ef_emcl_secondlambda, &b_ef_emcl_secondlambda);
   fChain->SetBranchAddress("ef_emcl_lateral", &ef_emcl_lateral, &b_ef_emcl_lateral);
   fChain->SetBranchAddress("ef_emcl_secondR", &ef_emcl_secondR, &b_ef_emcl_secondR);
   fChain->SetBranchAddress("ef_emcl_centerlambda", &ef_emcl_centerlambda, &b_ef_emcl_centerlambda);
   fChain->SetBranchAddress("ef_emcl_deltaTheta", &ef_emcl_deltaTheta, &b_ef_emcl_deltaTheta);
   fChain->SetBranchAddress("ef_emcl_deltaPhi", &ef_emcl_deltaPhi, &b_ef_emcl_deltaPhi);
   fChain->SetBranchAddress("ef_emcl_time", &ef_emcl_time, &b_ef_emcl_time);
   fChain->SetBranchAddress("ef_emcl_slw_n", &ef_emcl_slw_n, &b_ef_emcl_slw_n);
   fChain->SetBranchAddress("ef_emcl_slw_E", &ef_emcl_slw_E, &b_ef_emcl_slw_E);
   fChain->SetBranchAddress("ef_emcl_slw_pt", &ef_emcl_slw_pt, &b_ef_emcl_slw_pt);
   fChain->SetBranchAddress("ef_emcl_slw_m", &ef_emcl_slw_m, &b_ef_emcl_slw_m);
   fChain->SetBranchAddress("ef_emcl_slw_eta", &ef_emcl_slw_eta, &b_ef_emcl_slw_eta);
   fChain->SetBranchAddress("ef_emcl_slw_phi", &ef_emcl_slw_phi, &b_ef_emcl_slw_phi);
   fChain->SetBranchAddress("ef_emcl_slw_E_em", &ef_emcl_slw_E_em, &b_ef_emcl_slw_E_em);
   fChain->SetBranchAddress("ef_emcl_slw_E_had", &ef_emcl_slw_E_had, &b_ef_emcl_slw_E_had);
   fChain->SetBranchAddress("ef_emcl_slw_firstEdens", &ef_emcl_slw_firstEdens, &b_ef_emcl_slw_firstEdens);
   fChain->SetBranchAddress("ef_emcl_slw_cellmaxfrac", &ef_emcl_slw_cellmaxfrac, &b_ef_emcl_slw_cellmaxfrac);
   fChain->SetBranchAddress("ef_emcl_slw_longitudinal", &ef_emcl_slw_longitudinal, &b_ef_emcl_slw_longitudinal);
   fChain->SetBranchAddress("ef_emcl_slw_secondlambda", &ef_emcl_slw_secondlambda, &b_ef_emcl_slw_secondlambda);
   fChain->SetBranchAddress("ef_emcl_slw_lateral", &ef_emcl_slw_lateral, &b_ef_emcl_slw_lateral);
   fChain->SetBranchAddress("ef_emcl_slw_secondR", &ef_emcl_slw_secondR, &b_ef_emcl_slw_secondR);
   fChain->SetBranchAddress("ef_emcl_slw_centerlambda", &ef_emcl_slw_centerlambda, &b_ef_emcl_slw_centerlambda);
   fChain->SetBranchAddress("ef_emcl_slw_deltaTheta", &ef_emcl_slw_deltaTheta, &b_ef_emcl_slw_deltaTheta);
   fChain->SetBranchAddress("ef_emcl_slw_deltaPhi", &ef_emcl_slw_deltaPhi, &b_ef_emcl_slw_deltaPhi);
   fChain->SetBranchAddress("ef_emcl_slw_time", &ef_emcl_slw_time, &b_ef_emcl_slw_time);
   fChain->SetBranchAddress("ef_el_n", &ef_el_n, &b_ef_el_n);
   fChain->SetBranchAddress("ef_el_E", &ef_el_E, &b_ef_el_E);
   fChain->SetBranchAddress("ef_el_Et", &ef_el_Et, &b_ef_el_Et);
   fChain->SetBranchAddress("ef_el_pt", &ef_el_pt, &b_ef_el_pt);
   fChain->SetBranchAddress("ef_el_m", &ef_el_m, &b_ef_el_m);
   fChain->SetBranchAddress("ef_el_eta", &ef_el_eta, &b_ef_el_eta);
   fChain->SetBranchAddress("ef_el_phi", &ef_el_phi, &b_ef_el_phi);
   fChain->SetBranchAddress("ef_el_px", &ef_el_px, &b_ef_el_px);
   fChain->SetBranchAddress("ef_el_py", &ef_el_py, &b_ef_el_py);
   fChain->SetBranchAddress("ef_el_pz", &ef_el_pz, &b_ef_el_pz);
   fChain->SetBranchAddress("ef_el_charge", &ef_el_charge, &b_ef_el_charge);
   fChain->SetBranchAddress("ef_el_author", &ef_el_author, &b_ef_el_author);
   fChain->SetBranchAddress("ef_el_isEM", &ef_el_isEM, &b_ef_el_isEM);
   fChain->SetBranchAddress("ef_el_loose", &ef_el_loose, &b_ef_el_loose);
   fChain->SetBranchAddress("ef_el_medium", &ef_el_medium, &b_ef_el_medium);
   fChain->SetBranchAddress("ef_el_mediumIso", &ef_el_mediumIso, &b_ef_el_mediumIso);
   fChain->SetBranchAddress("ef_el_tight", &ef_el_tight, &b_ef_el_tight);
   fChain->SetBranchAddress("ef_el_tightIso", &ef_el_tightIso, &b_ef_el_tightIso);
   fChain->SetBranchAddress("ef_el_Ethad", &ef_el_Ethad, &b_ef_el_Ethad);
   fChain->SetBranchAddress("ef_el_Ethad1", &ef_el_Ethad1, &b_ef_el_Ethad1);
   fChain->SetBranchAddress("ef_el_f1", &ef_el_f1, &b_ef_el_f1);
   fChain->SetBranchAddress("ef_el_f1core", &ef_el_f1core, &b_ef_el_f1core);
   fChain->SetBranchAddress("ef_el_Emins1", &ef_el_Emins1, &b_ef_el_Emins1);
   fChain->SetBranchAddress("ef_el_fside", &ef_el_fside, &b_ef_el_fside);
   fChain->SetBranchAddress("ef_el_Emax2", &ef_el_Emax2, &b_ef_el_Emax2);
   fChain->SetBranchAddress("ef_el_ws3", &ef_el_ws3, &b_ef_el_ws3);
   fChain->SetBranchAddress("ef_el_wstot", &ef_el_wstot, &b_ef_el_wstot);
   fChain->SetBranchAddress("ef_el_emaxs1", &ef_el_emaxs1, &b_ef_el_emaxs1);
   fChain->SetBranchAddress("ef_el_deltaEs", &ef_el_deltaEs, &b_ef_el_deltaEs);
   fChain->SetBranchAddress("ef_el_E233", &ef_el_E233, &b_ef_el_E233);
   fChain->SetBranchAddress("ef_el_E237", &ef_el_E237, &b_ef_el_E237);
   fChain->SetBranchAddress("ef_el_E277", &ef_el_E277, &b_ef_el_E277);
   fChain->SetBranchAddress("ef_el_weta2", &ef_el_weta2, &b_ef_el_weta2);
   fChain->SetBranchAddress("ef_el_f3", &ef_el_f3, &b_ef_el_f3);
   fChain->SetBranchAddress("ef_el_f3core", &ef_el_f3core, &b_ef_el_f3core);
   fChain->SetBranchAddress("ef_el_rphiallcalo", &ef_el_rphiallcalo, &b_ef_el_rphiallcalo);
   fChain->SetBranchAddress("ef_el_Etcone45", &ef_el_Etcone45, &b_ef_el_Etcone45);
   fChain->SetBranchAddress("ef_el_Etcone20", &ef_el_Etcone20, &b_ef_el_Etcone20);
   fChain->SetBranchAddress("ef_el_Etcone30", &ef_el_Etcone30, &b_ef_el_Etcone30);
   fChain->SetBranchAddress("ef_el_Etcone40", &ef_el_Etcone40, &b_ef_el_Etcone40);
   fChain->SetBranchAddress("ef_el_pos7", &ef_el_pos7, &b_ef_el_pos7);
   fChain->SetBranchAddress("ef_el_etacorrmag", &ef_el_etacorrmag, &b_ef_el_etacorrmag);
   fChain->SetBranchAddress("ef_el_deltaeta1", &ef_el_deltaeta1, &b_ef_el_deltaeta1);
   fChain->SetBranchAddress("ef_el_deltaeta2", &ef_el_deltaeta2, &b_ef_el_deltaeta2);
   fChain->SetBranchAddress("ef_el_deltaphi2", &ef_el_deltaphi2, &b_ef_el_deltaphi2);
   fChain->SetBranchAddress("ef_el_reta", &ef_el_reta, &b_ef_el_reta);
   fChain->SetBranchAddress("ef_el_rphi", &ef_el_rphi, &b_ef_el_rphi);
   fChain->SetBranchAddress("trigNavi_n", &trigNavi_n, &b_trigNavi_n);
   fChain->SetBranchAddress("trigNavi_chain_ChainId", &trigNavi_chain_ChainId, &b_trigNavi_chain_ChainId);
   fChain->SetBranchAddress("trigNavi_chain_RoIType", &trigNavi_chain_RoIType, &b_trigNavi_chain_RoIType);
   fChain->SetBranchAddress("trigNavi_chain_RoIIndex", &trigNavi_chain_RoIIndex, &b_trigNavi_chain_RoIIndex);
   fChain->SetBranchAddress("RoI_L2_e_n", &RoI_L2_e_n, &b_RoI_L2_e_n);
   fChain->SetBranchAddress("RoI_L2_e_type", &RoI_L2_e_type, &b_RoI_L2_e_type);
   fChain->SetBranchAddress("RoI_L2_e_lastStep", &RoI_L2_e_lastStep, &b_RoI_L2_e_lastStep);
   fChain->SetBranchAddress("RoI_L2_e_TrigEMCluster", &RoI_L2_e_TrigEMCluster, &b_RoI_L2_e_TrigEMCluster);
   fChain->SetBranchAddress("RoI_L2_e_TrigEMClusterStatus", &RoI_L2_e_TrigEMClusterStatus, &b_RoI_L2_e_TrigEMClusterStatus);
   fChain->SetBranchAddress("RoI_L2_e_EmTau_ROI", &RoI_L2_e_EmTau_ROI, &b_RoI_L2_e_EmTau_ROI);
   fChain->SetBranchAddress("RoI_L2_e_EmTau_ROIStatus", &RoI_L2_e_EmTau_ROIStatus, &b_RoI_L2_e_EmTau_ROIStatus);
   fChain->SetBranchAddress("RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma", &RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma, &b_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma);
   fChain->SetBranchAddress("RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus", &RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus, &b_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus);
   fChain->SetBranchAddress("RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma", &RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma, &b_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma);
   fChain->SetBranchAddress("RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus", &RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus, &b_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus);
   fChain->SetBranchAddress("RoI_L2_e_TrigElectronContainer", &RoI_L2_e_TrigElectronContainer, &b_RoI_L2_e_TrigElectronContainer);
   fChain->SetBranchAddress("RoI_L2_e_TrigElectronContainerStatus", &RoI_L2_e_TrigElectronContainerStatus, &b_RoI_L2_e_TrigElectronContainerStatus);
   fChain->SetBranchAddress("RoI_EF_e_n", &RoI_EF_e_n, &b_RoI_EF_e_n);
   fChain->SetBranchAddress("RoI_EF_e_type", &RoI_EF_e_type, &b_RoI_EF_e_type);
   fChain->SetBranchAddress("RoI_EF_e_lastStep", &RoI_EF_e_lastStep, &b_RoI_EF_e_lastStep);
   fChain->SetBranchAddress("RoI_EF_e_EmTau_ROI", &RoI_EF_e_EmTau_ROI, &b_RoI_EF_e_EmTau_ROI);
   fChain->SetBranchAddress("RoI_EF_e_EmTau_ROIStatus", &RoI_EF_e_EmTau_ROIStatus, &b_RoI_EF_e_EmTau_ROIStatus);
   Notify();
}

Bool_t D3PD_NTUP::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void D3PD_NTUP::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t D3PD_NTUP::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef D3PD_NTUP_cxx
