/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Dec  9 23:03:56 2009 by ROOT version 5.22/00d
// from TTree TrigMuon/HLT muon monitoring and validation
// found on file: ntuple/trigntuple.root
//////////////////////////////////////////////////////////

#ifndef TrigMuon_h
#define TrigMuon_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
   const Int_t kMaxEventTrigInfo = 1;
   const Int_t kMaxL1MuonRoI = 5;
   const Int_t kMaxMufast = 10;
   const Int_t kMaxmuFast_esd = 1;
   const Int_t kMaxInDet_SiTrk_Muon = 18;
   const Int_t kMaxInDet_IDSCAN_Muon = 14;
   const Int_t kMaxMucomb = 46;
   const Int_t kMaxMuisol = 15;
   const Int_t kMaxTilemu = 8;
   const Int_t kMaxTileTrackMu = 8;
   const Int_t kMaxTrigMuonEF = 25;
   const Int_t kMaxTrigMuGirl = 22;

class TrigMuon {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           EventTrigInfo_;
   UInt_t          EventTrigInfo_fUniqueID[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_fBits[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_run[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_lumiblock[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_event[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_time[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_timens[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_BCID[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_detectormask0[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_detectormask1[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_statusElement[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_extendedLevel1ID[kMaxEventTrigInfo];   //[EventTrigInfo_]
   UInt_t          EventTrigInfo_level1TriggerType[kMaxEventTrigInfo];   //[EventTrigInfo_]
   vector<unsigned int> EventTrigInfo_level1TriggerInfo[kMaxEventTrigInfo];
   vector<unsigned int> EventTrigInfo_level2TriggerInfo[kMaxEventTrigInfo];
   vector<unsigned int> EventTrigInfo_eventFilterInfo[kMaxEventTrigInfo];
   Int_t           L1MuonRoI_N;
   Int_t           L1MuonRoI_;
   UInt_t          L1MuonRoI_fUniqueID[kMaxL1MuonRoI];   //[L1MuonRoI_]
   UInt_t          L1MuonRoI_fBits[kMaxL1MuonRoI];   //[L1MuonRoI_]
   UInt_t          L1MuonRoI_L1_RoIWord[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Int_t           L1MuonRoI_L1_RoISource[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Int_t           L1MuonRoI_L1_ThrNumber[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Int_t           L1MuonRoI_L1_RoINumber[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Int_t           L1MuonRoI_L1_SectorAddress[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Bool_t          L1MuonRoI_L1_isAside[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Bool_t          L1MuonRoI_L1_isFirstCandidate[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Bool_t          L1MuonRoI_L1_isMoreCandInRoI[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Bool_t          L1MuonRoI_L1_isMoreCandInSector[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Float_t         L1MuonRoI_L1_pt[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Float_t         L1MuonRoI_L1_eta[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Float_t         L1MuonRoI_L1_phi[kMaxL1MuonRoI];   //[L1MuonRoI_]
   Int_t           Mufast_N;
   Int_t           Mufast_;
   UInt_t          Mufast_fUniqueID[kMaxMufast];   //[Mufast_]
   UInt_t          Mufast_fBits[kMaxMufast];   //[Mufast_]
   Bool_t          Mufast_MuFast[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_pt[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_eta[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_phi[kMaxMufast];   //[Mufast_]
   Int_t           Mufast_MuFast_algoID[kMaxMufast];   //[Mufast_]
   Int_t           Mufast_MuFast_RoIID[kMaxMufast];   //[Mufast_]
   Int_t           Mufast_MuFast_sAddress[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_radius[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_dirPhi[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_zeta[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_dirZeta[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_beta[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_sp1R[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_sp1Z[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_sp1Slope[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_sp2R[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_sp2Z[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_sp2Slope[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_sp3R[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_sp3Z[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_sp3Slope[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_brRadius[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_brSagitta[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_ECAlpha[kMaxMufast];   //[Mufast_]
   Float_t         Mufast_MuFast_ECBeta[kMaxMufast];   //[Mufast_]
   Int_t           muFast_esd_;
   UInt_t          muFast_esd_fUniqueID[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_fBits[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_id[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_te_id[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_error[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_lvl1_id[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_lumi_block[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_muondetmask[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_RoIID[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_RoISystem[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_RoISubsystem[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_RoISector[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_RoINumber[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_RoIThreshold[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_RoIEta[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_RoIPhi[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_rpc_pad_error[kMaxmuFast_esd];   //[muFast_esd_]
   UInt_t          muFast_esd_m_tgc_rdo_error[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_rpc1_x[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_rpc1_y[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_rpc1_z[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_rpc2_x[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_rpc2_y[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_rpc2_z[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_rpc3_x[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_rpc3_y[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_rpc3_z[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Mid1_eta[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Mid1_phi[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Mid1_r[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Mid1_z[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Mid2_eta[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Mid2_phi[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Mid2_r[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Mid2_z[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Mid_rho_chi2[kMaxmuFast_esd];   //[muFast_esd_]
   Long_t          muFast_esd_m_tgc_Mid_rho_N[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Mid_phi_chi2[kMaxmuFast_esd];   //[muFast_esd_]
   Long_t          muFast_esd_m_tgc_Mid_phi_N[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Inn_eta[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Inn_phi[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Inn_r[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Inn_z[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Inn_rho_std[kMaxmuFast_esd];   //[muFast_esd_]
   Long_t          muFast_esd_m_tgc_Inn_rho_N[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_Inn_phi_std[kMaxmuFast_esd];   //[muFast_esd_]
   Long_t          muFast_esd_m_tgc_Inn_phi_N[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_tgc_PT[kMaxmuFast_esd];   //[muFast_esd_]
   Long_t          muFast_esd_m_type[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_t_x_center[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_t_y_center[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_t_radius[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_t_ap[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_t_aphi[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_t_phi[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_extrInnerEta[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_extrInnerPhi[kMaxmuFast_esd];   //[muFast_esd_]
   vector<int>     muFast_esd_m_tr_chamber_type_1[kMaxmuFast_esd];
   vector<int>     muFast_esd_m_tr_chamber_type_2[kMaxmuFast_esd];
   vector<int>     muFast_esd_m_tr_pos[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tr_aw[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tr_bw[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tr_zetaMin[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tr_zetaMax[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tr_radMin[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tr_radMax[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tr_etaMin[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tr_etaMax[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tr_st_phi[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tr_st_roads[kMaxmuFast_esd];
   vector<int>     muFast_esd_m_ov_chamber_type_1[kMaxmuFast_esd];
   vector<int>     muFast_esd_m_ov_chamber_type_2[kMaxmuFast_esd];
   vector<int>     muFast_esd_m_ov_pos[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_ov_aw[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_ov_bw[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_ov_zetaMin[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_ov_zetaMax[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_ov_radMin[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_ov_radMax[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_ov_etaMin[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_ov_etaMax[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_ov_st_phi[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_ov_st_roads[kMaxmuFast_esd];
   Float_t         muFast_esd_m_mdt_Inner_slope[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Inner_intercept[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Inner_Z[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Inner_R[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Inner_fit_chi[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Middle_slope[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Middle_intercept[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Middle_Z[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Middle_R[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Middle_fit_chi[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Outer_slope[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Outer_intercept[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Outer_Z[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Outer_R[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_mdt_Outer_fit_chi[kMaxmuFast_esd];   //[muFast_esd_]
   Int_t           muFast_esd_m_Address[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_Sagitta[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_Radius[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_Slope[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_Intercept[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_Alpha[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_Beta[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_DeltaR[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_Speed_over_c[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_Eta[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_PhiMap[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_Phi[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_PhiDir[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_Pt[kMaxmuFast_esd];   //[muFast_esd_]
   Float_t         muFast_esd_m_Charge[kMaxmuFast_esd];   //[muFast_esd_]
   vector<unsigned int> muFast_esd_m_extension0[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_extension1[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_extension2[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_extension3[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_extension4[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_extension5[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_extension6[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_extension7[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_extension8[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_extension9[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_lvl1_emulation[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_rob_id[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_csm_id[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_csm_size[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_csm_error[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_removed_rob_id[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_removed_csm_id[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_pad_hit_onlineId[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_pad_hit_code[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_pad_hit_x[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_pad_hit_y[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_pad_hit_z[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_pad_hit_r[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_pad_hit_p[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Inn_rho_hit_phi[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Inn_rho_hit_r[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Inn_rho_hit_z[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Inn_rho_hit_width[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_tgc_Inn_rho_hit_in_seg[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Inn_phi_hit_phi[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Inn_phi_hit_r[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Inn_phi_hit_z[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Inn_phi_hit_width[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_tgc_Inn_phi_hit_in_seg[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Mid_rho_hit_phi[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Mid_rho_hit_r[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Mid_rho_hit_z[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Mid_rho_hit_width[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_tgc_Mid_rho_hit_in_seg[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Mid_phi_hit_phi[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Mid_phi_hit_r[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Mid_phi_hit_z[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_tgc_Mid_phi_hit_width[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_tgc_Mid_phi_hit_in_seg[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_mdt_onlineId[kMaxmuFast_esd];
   vector<unsigned int> muFast_esd_m_mdt_offlineId[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_mdt_tube_r[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_mdt_tube_z[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_mdt_tube_residual[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_mdt_tube_time[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_mdt_tube_space[kMaxmuFast_esd];
   vector<float>   muFast_esd_m_mdt_tube_sigma[kMaxmuFast_esd];
   Int_t           InDet_SiTrk_Muon_N;
   Int_t           InDet_SiTrk_Muon_;
   UInt_t          InDet_SiTrk_Muon_fUniqueID[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   UInt_t          InDet_SiTrk_Muon_fBits[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Int_t           InDet_SiTrk_Muon_InDet_algo[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Float_t         InDet_SiTrk_Muon_InDet_pt[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Float_t         InDet_SiTrk_Muon_InDet_eta[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Float_t         InDet_SiTrk_Muon_InDet_phi[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Float_t         InDet_SiTrk_Muon_InDet_a0[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Float_t         InDet_SiTrk_Muon_InDet_z0[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Float_t         InDet_SiTrk_Muon_InDet_chi2[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Float_t         InDet_SiTrk_Muon_InDet_ept[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Float_t         InDet_SiTrk_Muon_InDet_eeta[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Float_t         InDet_SiTrk_Muon_InDet_ephi[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Float_t         InDet_SiTrk_Muon_InDet_ea0[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Float_t         InDet_SiTrk_Muon_InDet_ez0[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Int_t           InDet_SiTrk_Muon_InDet_nPIX[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Int_t           InDet_SiTrk_Muon_InDet_nSCT[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Int_t           InDet_SiTrk_Muon_InDet_hitPattern[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Int_t           InDet_SiTrk_Muon_InDet_nStrawHits[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Int_t           InDet_SiTrk_Muon_InDet_nStraw[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Int_t           InDet_SiTrk_Muon_InDet_nStrawTime[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Int_t           InDet_SiTrk_Muon_InDet_nTRHits[kMaxInDet_SiTrk_Muon];   //[InDet_SiTrk_Muon_]
   Int_t           InDet_IDSCAN_Muon_N;
   Int_t           InDet_IDSCAN_Muon_;
   UInt_t          InDet_IDSCAN_Muon_fUniqueID[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   UInt_t          InDet_IDSCAN_Muon_fBits[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Int_t           InDet_IDSCAN_Muon_InDet_algo[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Float_t         InDet_IDSCAN_Muon_InDet_pt[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Float_t         InDet_IDSCAN_Muon_InDet_eta[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Float_t         InDet_IDSCAN_Muon_InDet_phi[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Float_t         InDet_IDSCAN_Muon_InDet_a0[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Float_t         InDet_IDSCAN_Muon_InDet_z0[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Float_t         InDet_IDSCAN_Muon_InDet_chi2[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Float_t         InDet_IDSCAN_Muon_InDet_ept[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Float_t         InDet_IDSCAN_Muon_InDet_eeta[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Float_t         InDet_IDSCAN_Muon_InDet_ephi[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Float_t         InDet_IDSCAN_Muon_InDet_ea0[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Float_t         InDet_IDSCAN_Muon_InDet_ez0[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Int_t           InDet_IDSCAN_Muon_InDet_nPIX[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Int_t           InDet_IDSCAN_Muon_InDet_nSCT[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Int_t           InDet_IDSCAN_Muon_InDet_hitPattern[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Int_t           InDet_IDSCAN_Muon_InDet_nStrawHits[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Int_t           InDet_IDSCAN_Muon_InDet_nStraw[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Int_t           InDet_IDSCAN_Muon_InDet_nStrawTime[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Int_t           InDet_IDSCAN_Muon_InDet_nTRHits[kMaxInDet_IDSCAN_Muon];   //[InDet_IDSCAN_Muon_]
   Int_t           Mucomb_N;
   Int_t           Mucomb_;
   UInt_t          Mucomb_fUniqueID[kMaxMucomb];   //[Mucomb_]
   UInt_t          Mucomb_fBits[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_pt[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_eta[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_phi[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_sigmaPt[kMaxMucomb];   //[Mucomb_]
   Int_t           Mucomb_MuComb_ID_algo[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_ID_pt[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_ID_eta[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_ID_phi[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_ID_a0[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_ID_z0[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_ID_chi2[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_ID_ept[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_ID_eeta[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_ID_ephi[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_ID_ea0[kMaxMucomb];   //[Mucomb_]
   Float_t         Mucomb_MuComb_ID_ez0[kMaxMucomb];   //[Mucomb_]
   Int_t           Mucomb_MuComb_ID_nPIX[kMaxMucomb];   //[Mucomb_]
   Int_t           Mucomb_MuComb_ID_nSCT[kMaxMucomb];   //[Mucomb_]
   Int_t           Mucomb_MuComb_ID_hitPattern[kMaxMucomb];   //[Mucomb_]
   Int_t           Mucomb_MuComb_ID_nStrawHits[kMaxMucomb];   //[Mucomb_]
   Int_t           Mucomb_MuComb_ID_nStraw[kMaxMucomb];   //[Mucomb_]
   Int_t           Mucomb_MuComb_ID_nStrawTime[kMaxMucomb];   //[Mucomb_]
   Int_t           Mucomb_MuComb_ID_nTRHits[kMaxMucomb];   //[Mucomb_]
   Int_t           Muisol_N;
   Int_t           Muisol_;
   UInt_t          Muisol_fUniqueID[kMaxMuisol];   //[Muisol_]
   UInt_t          Muisol_fBits[kMaxMuisol];   //[Muisol_]
   Float_t         Muisol_MuIso_pt[kMaxMuisol];   //[Muisol_]
   Float_t         Muisol_MuIso_eta[kMaxMuisol];   //[Muisol_]
   Float_t         Muisol_MuIso_phi[kMaxMuisol];   //[Muisol_]
   Float_t         Muisol_MuIso_EtICEC[kMaxMuisol];   //[Muisol_]
   Float_t         Muisol_MuIso_EtOCEC[kMaxMuisol];   //[Muisol_]
   Float_t         Muisol_MuIso_EtICHC[kMaxMuisol];   //[Muisol_]
   Float_t         Muisol_MuIso_EtOCHC[kMaxMuisol];   //[Muisol_]
   Int_t           Muisol_MuIso_nTracks[kMaxMuisol];   //[Muisol_]
   Float_t         Muisol_MuIso_sumPtTracks[kMaxMuisol];   //[Muisol_]
   Float_t         Muisol_MuIso_ptMuTracks[kMaxMuisol];   //[Muisol_]
   Int_t           Muisol_MuIso_RoIId[kMaxMuisol];   //[Muisol_]
   Float_t         Muisol_MuIso_LArWeight[kMaxMuisol];   //[Muisol_]
   Float_t         Muisol_MuIso_TileWeight[kMaxMuisol];   //[Muisol_]
   Int_t           Tilemu_N;
   Int_t           Tilemu_;
   UInt_t          Tilemu_fUniqueID[kMaxTilemu];   //[Tilemu_]
   UInt_t          Tilemu_fBits[kMaxTilemu];   //[Tilemu_]
   Float_t         Tilemu_TileMu_eta[kMaxTilemu];   //[Tilemu_]
   Float_t         Tilemu_TileMu_phi[kMaxTilemu];   //[Tilemu_]
   vector<float>   Tilemu_TileMu_enedep[kMaxTilemu];
   Float_t         Tilemu_TileMu_quality[kMaxTilemu];   //[Tilemu_]
   Int_t           TileTrackMu_N;
   Int_t           TileTrackMu_;
   UInt_t          TileTrackMu_fUniqueID[kMaxTileTrackMu];   //[TileTrackMu_]
   UInt_t          TileTrackMu_fBits[kMaxTileTrackMu];   //[TileTrackMu_]
   Float_t         TileTrackMu_TileMu_pt[kMaxTileTrackMu];   //[TileTrackMu_]
   Float_t         TileTrackMu_TileMu_eta[kMaxTileTrackMu];   //[TileTrackMu_]
   Float_t         TileTrackMu_TileMu_phi[kMaxTileTrackMu];   //[TileTrackMu_]
   Int_t           TrigMuonEF_N;
   Int_t           TrigMuonEF_;
   UInt_t          TrigMuonEF_fUniqueID[kMaxTrigMuonEF];   //[TrigMuonEF_]
   UInt_t          TrigMuonEF_fBits[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEF_muonType[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEF_pt[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEF_eta[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEF_phi[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEF_nTracks[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEF_nSegments[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEF_nMDT[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEF_nRPC[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEF_nTGC[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEF_nCSC[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFMS_pt[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFMS_eta[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFMS_phi[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFMS_chi2[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFMS_chi2prob[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFMS_d0[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFMS_z0[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFMS_charge[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEFMS_nCSCeta[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEFMS_nCSCphi[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEFMS_nRPCeta[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEFMS_nRPCphi[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEFMS_nTGCeta[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEFMS_nTGCphi[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFSA_pt[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFSA_eta[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFSA_phi[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFSA_chi2[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFSA_chi2prob[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFSA_d0[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFSA_z0[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFSA_charge[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFCB_pt[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFCB_eta[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFCB_phi[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFCB_chi2[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFCB_chi2prob[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFCB_d0[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFCB_z0[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFCB_charge[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Float_t         TrigMuonEF_MuonEFCB_matchChi2[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEFCB_nSCT[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEFCB_nPIX[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuonEF_MuonEFCB_nTRT[kMaxTrigMuonEF];   //[TrigMuonEF_]
   Int_t           TrigMuGirl_N;
   Int_t           TrigMuGirl_;
   UInt_t          TrigMuGirl_fUniqueID[kMaxTrigMuGirl];   //[TrigMuGirl_]
   UInt_t          TrigMuGirl_fBits[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEF_muonType[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEF_pt[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEF_eta[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEF_phi[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEF_nTracks[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEF_nSegments[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEF_nMDT[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEF_nRPC[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEF_nTGC[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEF_nCSC[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFMS_pt[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFMS_eta[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFMS_phi[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFMS_chi2[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFMS_chi2prob[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFMS_d0[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFMS_z0[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFMS_charge[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEFMS_nCSCeta[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEFMS_nCSCphi[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEFMS_nRPCeta[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEFMS_nRPCphi[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEFMS_nTGCeta[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEFMS_nTGCphi[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFSA_pt[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFSA_eta[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFSA_phi[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFSA_chi2[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFSA_chi2prob[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFSA_d0[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFSA_z0[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFSA_charge[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFCB_pt[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFCB_eta[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFCB_phi[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFCB_chi2[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFCB_chi2prob[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFCB_d0[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFCB_z0[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFCB_charge[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Float_t         TrigMuGirl_MuonEFCB_matchChi2[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEFCB_nSCT[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEFCB_nPIX[kMaxTrigMuGirl];   //[TrigMuGirl_]
   Int_t           TrigMuGirl_MuonEFCB_nTRT[kMaxTrigMuGirl];   //[TrigMuGirl_]

   // List of branches
   TBranch        *b_EventTrigInfo_;   //!
   TBranch        *b_EventTrigInfo_fUniqueID;   //!
   TBranch        *b_EventTrigInfo_fBits;   //!
   TBranch        *b_EventTrigInfo_run;   //!
   TBranch        *b_EventTrigInfo_lumiblock;   //!
   TBranch        *b_EventTrigInfo_event;   //!
   TBranch        *b_EventTrigInfo_time;   //!
   TBranch        *b_EventTrigInfo_timens;   //!
   TBranch        *b_EventTrigInfo_BCID;   //!
   TBranch        *b_EventTrigInfo_detectormask0;   //!
   TBranch        *b_EventTrigInfo_detectormask1;   //!
   TBranch        *b_EventTrigInfo_statusElement;   //!
   TBranch        *b_EventTrigInfo_extendedLevel1ID;   //!
   TBranch        *b_EventTrigInfo_level1TriggerType;   //!
   TBranch        *b_EventTrigInfo_level1TriggerInfo;   //!
   TBranch        *b_EventTrigInfo_level2TriggerInfo;   //!
   TBranch        *b_EventTrigInfo_eventFilterInfo;   //!
   TBranch        *b_L1MuonRoI_N;   //!
   TBranch        *b_L1MuonRoI_;   //!
   TBranch        *b_L1MuonRoI_fUniqueID;   //!
   TBranch        *b_L1MuonRoI_fBits;   //!
   TBranch        *b_L1MuonRoI_L1_RoIWord;   //!
   TBranch        *b_L1MuonRoI_L1_RoISource;   //!
   TBranch        *b_L1MuonRoI_L1_ThrNumber;   //!
   TBranch        *b_L1MuonRoI_L1_RoINumber;   //!
   TBranch        *b_L1MuonRoI_L1_SectorAddress;   //!
   TBranch        *b_L1MuonRoI_L1_isAside;   //!
   TBranch        *b_L1MuonRoI_L1_isFirstCandidate;   //!
   TBranch        *b_L1MuonRoI_L1_isMoreCandInRoI;   //!
   TBranch        *b_L1MuonRoI_L1_isMoreCandInSector;   //!
   TBranch        *b_L1MuonRoI_L1_pt;   //!
   TBranch        *b_L1MuonRoI_L1_eta;   //!
   TBranch        *b_L1MuonRoI_L1_phi;   //!
   TBranch        *b_Mufast_N;   //!
   TBranch        *b_Mufast_;   //!
   TBranch        *b_Mufast_fUniqueID;   //!
   TBranch        *b_Mufast_fBits;   //!
   TBranch        *b_Mufast_MuFast;   //!
   TBranch        *b_Mufast_MuFast_pt;   //!
   TBranch        *b_Mufast_MuFast_eta;   //!
   TBranch        *b_Mufast_MuFast_phi;   //!
   TBranch        *b_Mufast_MuFast_algoID;   //!
   TBranch        *b_Mufast_MuFast_RoIID;   //!
   TBranch        *b_Mufast_MuFast_sAddress;   //!
   TBranch        *b_Mufast_MuFast_radius;   //!
   TBranch        *b_Mufast_MuFast_dirPhi;   //!
   TBranch        *b_Mufast_MuFast_zeta;   //!
   TBranch        *b_Mufast_MuFast_dirZeta;   //!
   TBranch        *b_Mufast_MuFast_beta;   //!
   TBranch        *b_Mufast_MuFast_sp1R;   //!
   TBranch        *b_Mufast_MuFast_sp1Z;   //!
   TBranch        *b_Mufast_MuFast_sp1Slope;   //!
   TBranch        *b_Mufast_MuFast_sp2R;   //!
   TBranch        *b_Mufast_MuFast_sp2Z;   //!
   TBranch        *b_Mufast_MuFast_sp2Slope;   //!
   TBranch        *b_Mufast_MuFast_sp3R;   //!
   TBranch        *b_Mufast_MuFast_sp3Z;   //!
   TBranch        *b_Mufast_MuFast_sp3Slope;   //!
   TBranch        *b_Mufast_MuFast_brRadius;   //!
   TBranch        *b_Mufast_MuFast_brSagitta;   //!
   TBranch        *b_Mufast_MuFast_ECAlpha;   //!
   TBranch        *b_Mufast_MuFast_ECBeta;   //!
   TBranch        *b_muFast_esd_;   //!
   TBranch        *b_muFast_esd_fUniqueID;   //!
   TBranch        *b_muFast_esd_fBits;   //!
   TBranch        *b_muFast_esd_m_id;   //!
   TBranch        *b_muFast_esd_m_te_id;   //!
   TBranch        *b_muFast_esd_m_error;   //!
   TBranch        *b_muFast_esd_m_lvl1_id;   //!
   TBranch        *b_muFast_esd_m_lumi_block;   //!
   TBranch        *b_muFast_esd_m_muondetmask;   //!
   TBranch        *b_muFast_esd_m_RoIID;   //!
   TBranch        *b_muFast_esd_m_RoISystem;   //!
   TBranch        *b_muFast_esd_m_RoISubsystem;   //!
   TBranch        *b_muFast_esd_m_RoISector;   //!
   TBranch        *b_muFast_esd_m_RoINumber;   //!
   TBranch        *b_muFast_esd_m_RoIThreshold;   //!
   TBranch        *b_muFast_esd_m_RoIEta;   //!
   TBranch        *b_muFast_esd_m_RoIPhi;   //!
   TBranch        *b_muFast_esd_m_rpc_pad_error;   //!
   TBranch        *b_muFast_esd_m_tgc_rdo_error;   //!
   TBranch        *b_muFast_esd_m_rpc1_x;   //!
   TBranch        *b_muFast_esd_m_rpc1_y;   //!
   TBranch        *b_muFast_esd_m_rpc1_z;   //!
   TBranch        *b_muFast_esd_m_rpc2_x;   //!
   TBranch        *b_muFast_esd_m_rpc2_y;   //!
   TBranch        *b_muFast_esd_m_rpc2_z;   //!
   TBranch        *b_muFast_esd_m_rpc3_x;   //!
   TBranch        *b_muFast_esd_m_rpc3_y;   //!
   TBranch        *b_muFast_esd_m_rpc3_z;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid1_eta;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid1_phi;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid1_r;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid1_z;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid2_eta;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid2_phi;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid2_r;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid2_z;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_rho_chi2;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_rho_N;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_phi_chi2;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_phi_N;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_eta;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_phi;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_r;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_z;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_rho_std;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_rho_N;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_phi_std;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_phi_N;   //!
   TBranch        *b_muFast_esd_m_tgc_PT;   //!
   TBranch        *b_muFast_esd_m_type;   //!
   TBranch        *b_muFast_esd_m_t_x_center;   //!
   TBranch        *b_muFast_esd_m_t_y_center;   //!
   TBranch        *b_muFast_esd_m_t_radius;   //!
   TBranch        *b_muFast_esd_m_t_ap;   //!
   TBranch        *b_muFast_esd_m_t_aphi;   //!
   TBranch        *b_muFast_esd_m_t_phi;   //!
   TBranch        *b_muFast_esd_m_extrInnerEta;   //!
   TBranch        *b_muFast_esd_m_extrInnerPhi;   //!
   TBranch        *b_muFast_esd_m_tr_chamber_type_1;   //!
   TBranch        *b_muFast_esd_m_tr_chamber_type_2;   //!
   TBranch        *b_muFast_esd_m_tr_pos;   //!
   TBranch        *b_muFast_esd_m_tr_aw;   //!
   TBranch        *b_muFast_esd_m_tr_bw;   //!
   TBranch        *b_muFast_esd_m_tr_zetaMin;   //!
   TBranch        *b_muFast_esd_m_tr_zetaMax;   //!
   TBranch        *b_muFast_esd_m_tr_radMin;   //!
   TBranch        *b_muFast_esd_m_tr_radMax;   //!
   TBranch        *b_muFast_esd_m_tr_etaMin;   //!
   TBranch        *b_muFast_esd_m_tr_etaMax;   //!
   TBranch        *b_muFast_esd_m_tr_st_phi;   //!
   TBranch        *b_muFast_esd_m_tr_st_roads;   //!
   TBranch        *b_muFast_esd_m_ov_chamber_type_1;   //!
   TBranch        *b_muFast_esd_m_ov_chamber_type_2;   //!
   TBranch        *b_muFast_esd_m_ov_pos;   //!
   TBranch        *b_muFast_esd_m_ov_aw;   //!
   TBranch        *b_muFast_esd_m_ov_bw;   //!
   TBranch        *b_muFast_esd_m_ov_zetaMin;   //!
   TBranch        *b_muFast_esd_m_ov_zetaMax;   //!
   TBranch        *b_muFast_esd_m_ov_radMin;   //!
   TBranch        *b_muFast_esd_m_ov_radMax;   //!
   TBranch        *b_muFast_esd_m_ov_etaMin;   //!
   TBranch        *b_muFast_esd_m_ov_etaMax;   //!
   TBranch        *b_muFast_esd_m_ov_st_phi;   //!
   TBranch        *b_muFast_esd_m_ov_st_roads;   //!
   TBranch        *b_muFast_esd_m_mdt_Inner_slope;   //!
   TBranch        *b_muFast_esd_m_mdt_Inner_intercept;   //!
   TBranch        *b_muFast_esd_m_mdt_Inner_Z;   //!
   TBranch        *b_muFast_esd_m_mdt_Inner_R;   //!
   TBranch        *b_muFast_esd_m_mdt_Inner_fit_chi;   //!
   TBranch        *b_muFast_esd_m_mdt_Middle_slope;   //!
   TBranch        *b_muFast_esd_m_mdt_Middle_intercept;   //!
   TBranch        *b_muFast_esd_m_mdt_Middle_Z;   //!
   TBranch        *b_muFast_esd_m_mdt_Middle_R;   //!
   TBranch        *b_muFast_esd_m_mdt_Middle_fit_chi;   //!
   TBranch        *b_muFast_esd_m_mdt_Outer_slope;   //!
   TBranch        *b_muFast_esd_m_mdt_Outer_intercept;   //!
   TBranch        *b_muFast_esd_m_mdt_Outer_Z;   //!
   TBranch        *b_muFast_esd_m_mdt_Outer_R;   //!
   TBranch        *b_muFast_esd_m_mdt_Outer_fit_chi;   //!
   TBranch        *b_muFast_esd_m_Address;   //!
   TBranch        *b_muFast_esd_m_Sagitta;   //!
   TBranch        *b_muFast_esd_m_Radius;   //!
   TBranch        *b_muFast_esd_m_Slope;   //!
   TBranch        *b_muFast_esd_m_Intercept;   //!
   TBranch        *b_muFast_esd_m_Alpha;   //!
   TBranch        *b_muFast_esd_m_Beta;   //!
   TBranch        *b_muFast_esd_m_DeltaR;   //!
   TBranch        *b_muFast_esd_m_Speed_over_c;   //!
   TBranch        *b_muFast_esd_m_Eta;   //!
   TBranch        *b_muFast_esd_m_PhiMap;   //!
   TBranch        *b_muFast_esd_m_Phi;   //!
   TBranch        *b_muFast_esd_m_PhiDir;   //!
   TBranch        *b_muFast_esd_m_Pt;   //!
   TBranch        *b_muFast_esd_m_Charge;   //!
   TBranch        *b_muFast_esd_m_extension0;   //!
   TBranch        *b_muFast_esd_m_extension1;   //!
   TBranch        *b_muFast_esd_m_extension2;   //!
   TBranch        *b_muFast_esd_m_extension3;   //!
   TBranch        *b_muFast_esd_m_extension4;   //!
   TBranch        *b_muFast_esd_m_extension5;   //!
   TBranch        *b_muFast_esd_m_extension6;   //!
   TBranch        *b_muFast_esd_m_extension7;   //!
   TBranch        *b_muFast_esd_m_extension8;   //!
   TBranch        *b_muFast_esd_m_extension9;   //!
   TBranch        *b_muFast_esd_m_lvl1_emulation;   //!
   TBranch        *b_muFast_esd_m_rob_id;   //!
   TBranch        *b_muFast_esd_m_csm_id;   //!
   TBranch        *b_muFast_esd_m_csm_size;   //!
   TBranch        *b_muFast_esd_m_csm_error;   //!
   TBranch        *b_muFast_esd_m_removed_rob_id;   //!
   TBranch        *b_muFast_esd_m_removed_csm_id;   //!
   TBranch        *b_muFast_esd_m_pad_hit_onlineId;   //!
   TBranch        *b_muFast_esd_m_pad_hit_code;   //!
   TBranch        *b_muFast_esd_m_pad_hit_x;   //!
   TBranch        *b_muFast_esd_m_pad_hit_y;   //!
   TBranch        *b_muFast_esd_m_pad_hit_z;   //!
   TBranch        *b_muFast_esd_m_pad_hit_r;   //!
   TBranch        *b_muFast_esd_m_pad_hit_p;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_rho_hit_phi;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_rho_hit_r;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_rho_hit_z;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_rho_hit_width;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_rho_hit_in_seg;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_phi_hit_phi;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_phi_hit_r;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_phi_hit_z;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_phi_hit_width;   //!
   TBranch        *b_muFast_esd_m_tgc_Inn_phi_hit_in_seg;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_rho_hit_phi;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_rho_hit_r;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_rho_hit_z;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_rho_hit_width;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_rho_hit_in_seg;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_phi_hit_phi;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_phi_hit_r;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_phi_hit_z;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_phi_hit_width;   //!
   TBranch        *b_muFast_esd_m_tgc_Mid_phi_hit_in_seg;   //!
   TBranch        *b_muFast_esd_m_mdt_onlineId;   //!
   TBranch        *b_muFast_esd_m_mdt_offlineId;   //!
   TBranch        *b_muFast_esd_m_mdt_tube_r;   //!
   TBranch        *b_muFast_esd_m_mdt_tube_z;   //!
   TBranch        *b_muFast_esd_m_mdt_tube_residual;   //!
   TBranch        *b_muFast_esd_m_mdt_tube_time;   //!
   TBranch        *b_muFast_esd_m_mdt_tube_space;   //!
   TBranch        *b_muFast_esd_m_mdt_tube_sigma;   //!
   TBranch        *b_InDet_SiTrk_Muon_N;   //!
   TBranch        *b_InDet_SiTrk_Muon_;   //!
   TBranch        *b_InDet_SiTrk_Muon_fUniqueID;   //!
   TBranch        *b_InDet_SiTrk_Muon_fBits;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_algo;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_pt;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_eta;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_phi;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_a0;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_z0;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_chi2;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_ept;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_eeta;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_ephi;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_ea0;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_ez0;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_nPIX;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_nSCT;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_hitPattern;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_nStrawHits;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_nStraw;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_nStrawTime;   //!
   TBranch        *b_InDet_SiTrk_Muon_InDet_nTRHits;   //!
   TBranch        *b_InDet_IDSCAN_Muon_N;   //!
   TBranch        *b_InDet_IDSCAN_Muon_;   //!
   TBranch        *b_InDet_IDSCAN_Muon_fUniqueID;   //!
   TBranch        *b_InDet_IDSCAN_Muon_fBits;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_algo;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_pt;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_eta;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_phi;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_a0;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_z0;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_chi2;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_ept;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_eeta;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_ephi;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_ea0;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_ez0;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_nPIX;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_nSCT;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_hitPattern;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_nStrawHits;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_nStraw;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_nStrawTime;   //!
   TBranch        *b_InDet_IDSCAN_Muon_InDet_nTRHits;   //!
   TBranch        *b_Mucomb_N;   //!
   TBranch        *b_Mucomb_;   //!
   TBranch        *b_Mucomb_fUniqueID;   //!
   TBranch        *b_Mucomb_fBits;   //!
   TBranch        *b_Mucomb_MuComb_pt;   //!
   TBranch        *b_Mucomb_MuComb_eta;   //!
   TBranch        *b_Mucomb_MuComb_phi;   //!
   TBranch        *b_Mucomb_MuComb_sigmaPt;   //!
   TBranch        *b_Mucomb_MuComb_ID_algo;   //!
   TBranch        *b_Mucomb_MuComb_ID_pt;   //!
   TBranch        *b_Mucomb_MuComb_ID_eta;   //!
   TBranch        *b_Mucomb_MuComb_ID_phi;   //!
   TBranch        *b_Mucomb_MuComb_ID_a0;   //!
   TBranch        *b_Mucomb_MuComb_ID_z0;   //!
   TBranch        *b_Mucomb_MuComb_ID_chi2;   //!
   TBranch        *b_Mucomb_MuComb_ID_ept;   //!
   TBranch        *b_Mucomb_MuComb_ID_eeta;   //!
   TBranch        *b_Mucomb_MuComb_ID_ephi;   //!
   TBranch        *b_Mucomb_MuComb_ID_ea0;   //!
   TBranch        *b_Mucomb_MuComb_ID_ez0;   //!
   TBranch        *b_Mucomb_MuComb_ID_nPIX;   //!
   TBranch        *b_Mucomb_MuComb_ID_nSCT;   //!
   TBranch        *b_Mucomb_MuComb_ID_hitPattern;   //!
   TBranch        *b_Mucomb_MuComb_ID_nStrawHits;   //!
   TBranch        *b_Mucomb_MuComb_ID_nStraw;   //!
   TBranch        *b_Mucomb_MuComb_ID_nStrawTime;   //!
   TBranch        *b_Mucomb_MuComb_ID_nTRHits;   //!
   TBranch        *b_Muisol_N;   //!
   TBranch        *b_Muisol_;   //!
   TBranch        *b_Muisol_fUniqueID;   //!
   TBranch        *b_Muisol_fBits;   //!
   TBranch        *b_Muisol_MuIso_pt;   //!
   TBranch        *b_Muisol_MuIso_eta;   //!
   TBranch        *b_Muisol_MuIso_phi;   //!
   TBranch        *b_Muisol_MuIso_EtICEC;   //!
   TBranch        *b_Muisol_MuIso_EtOCEC;   //!
   TBranch        *b_Muisol_MuIso_EtICHC;   //!
   TBranch        *b_Muisol_MuIso_EtOCHC;   //!
   TBranch        *b_Muisol_MuIso_nTracks;   //!
   TBranch        *b_Muisol_MuIso_sumPtTracks;   //!
   TBranch        *b_Muisol_MuIso_ptMuTracks;   //!
   TBranch        *b_Muisol_MuIso_RoIId;   //!
   TBranch        *b_Muisol_MuIso_LArWeight;   //!
   TBranch        *b_Muisol_MuIso_TileWeight;   //!
   TBranch        *b_Tilemu_N;   //!
   TBranch        *b_Tilemu_;   //!
   TBranch        *b_Tilemu_fUniqueID;   //!
   TBranch        *b_Tilemu_fBits;   //!
   TBranch        *b_Tilemu_TileMu_eta;   //!
   TBranch        *b_Tilemu_TileMu_phi;   //!
   TBranch        *b_Tilemu_TileMu_enedep;   //!
   TBranch        *b_Tilemu_TileMu_quality;   //!
   TBranch        *b_TileTrackMu_N;   //!
   TBranch        *b_TileTrackMu_;   //!
   TBranch        *b_TileTrackMu_fUniqueID;   //!
   TBranch        *b_TileTrackMu_fBits;   //!
   TBranch        *b_TileTrackMu_TileMu_pt;   //!
   TBranch        *b_TileTrackMu_TileMu_eta;   //!
   TBranch        *b_TileTrackMu_TileMu_phi;   //!
   TBranch        *b_TrigMuonEF_N;   //!
   TBranch        *b_TrigMuonEF_;   //!
   TBranch        *b_TrigMuonEF_fUniqueID;   //!
   TBranch        *b_TrigMuonEF_fBits;   //!
   TBranch        *b_TrigMuonEF_MuonEF_muonType;   //!
   TBranch        *b_TrigMuonEF_MuonEF_pt;   //!
   TBranch        *b_TrigMuonEF_MuonEF_eta;   //!
   TBranch        *b_TrigMuonEF_MuonEF_phi;   //!
   TBranch        *b_TrigMuonEF_MuonEF_nTracks;   //!
   TBranch        *b_TrigMuonEF_MuonEF_nSegments;   //!
   TBranch        *b_TrigMuonEF_MuonEF_nMDT;   //!
   TBranch        *b_TrigMuonEF_MuonEF_nRPC;   //!
   TBranch        *b_TrigMuonEF_MuonEF_nTGC;   //!
   TBranch        *b_TrigMuonEF_MuonEF_nCSC;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_pt;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_eta;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_phi;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_chi2;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_chi2prob;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_d0;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_z0;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_charge;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_nCSCeta;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_nCSCphi;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_nRPCeta;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_nRPCphi;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_nTGCeta;   //!
   TBranch        *b_TrigMuonEF_MuonEFMS_nTGCphi;   //!
   TBranch        *b_TrigMuonEF_MuonEFSA_pt;   //!
   TBranch        *b_TrigMuonEF_MuonEFSA_eta;   //!
   TBranch        *b_TrigMuonEF_MuonEFSA_phi;   //!
   TBranch        *b_TrigMuonEF_MuonEFSA_chi2;   //!
   TBranch        *b_TrigMuonEF_MuonEFSA_chi2prob;   //!
   TBranch        *b_TrigMuonEF_MuonEFSA_d0;   //!
   TBranch        *b_TrigMuonEF_MuonEFSA_z0;   //!
   TBranch        *b_TrigMuonEF_MuonEFSA_charge;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_pt;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_eta;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_phi;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_chi2;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_chi2prob;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_d0;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_z0;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_charge;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_matchChi2;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_nSCT;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_nPIX;   //!
   TBranch        *b_TrigMuonEF_MuonEFCB_nTRT;   //!
   TBranch        *b_TrigMuGirl_N;   //!
   TBranch        *b_TrigMuGirl_;   //!
   TBranch        *b_TrigMuGirl_fUniqueID;   //!
   TBranch        *b_TrigMuGirl_fBits;   //!
   TBranch        *b_TrigMuGirl_MuonEF_muonType;   //!
   TBranch        *b_TrigMuGirl_MuonEF_pt;   //!
   TBranch        *b_TrigMuGirl_MuonEF_eta;   //!
   TBranch        *b_TrigMuGirl_MuonEF_phi;   //!
   TBranch        *b_TrigMuGirl_MuonEF_nTracks;   //!
   TBranch        *b_TrigMuGirl_MuonEF_nSegments;   //!
   TBranch        *b_TrigMuGirl_MuonEF_nMDT;   //!
   TBranch        *b_TrigMuGirl_MuonEF_nRPC;   //!
   TBranch        *b_TrigMuGirl_MuonEF_nTGC;   //!
   TBranch        *b_TrigMuGirl_MuonEF_nCSC;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_pt;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_eta;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_phi;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_chi2;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_chi2prob;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_d0;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_z0;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_charge;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_nCSCeta;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_nCSCphi;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_nRPCeta;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_nRPCphi;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_nTGCeta;   //!
   TBranch        *b_TrigMuGirl_MuonEFMS_nTGCphi;   //!
   TBranch        *b_TrigMuGirl_MuonEFSA_pt;   //!
   TBranch        *b_TrigMuGirl_MuonEFSA_eta;   //!
   TBranch        *b_TrigMuGirl_MuonEFSA_phi;   //!
   TBranch        *b_TrigMuGirl_MuonEFSA_chi2;   //!
   TBranch        *b_TrigMuGirl_MuonEFSA_chi2prob;   //!
   TBranch        *b_TrigMuGirl_MuonEFSA_d0;   //!
   TBranch        *b_TrigMuGirl_MuonEFSA_z0;   //!
   TBranch        *b_TrigMuGirl_MuonEFSA_charge;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_pt;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_eta;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_phi;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_chi2;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_chi2prob;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_d0;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_z0;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_charge;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_matchChi2;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_nSCT;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_nPIX;   //!
   TBranch        *b_TrigMuGirl_MuonEFCB_nTRT;   //!

   TrigMuon(TTree *tree=0);
   virtual ~TrigMuon();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TrigMuon_cxx
TrigMuon::TrigMuon(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("ntuple/trigntuple.root");
      if (!f) {
         f = new TFile("ntuple/trigntuple.root");
         f->cd("ntuple/trigntuple.root:/Muon");
      }
      tree = (TTree*)gDirectory->Get("TrigMuon");

   }
   Init(tree);
}

TrigMuon::~TrigMuon()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TrigMuon::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TrigMuon::LoadTree(Long64_t entry)
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

void TrigMuon::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("EventTrigInfo", &EventTrigInfo_, &b_EventTrigInfo_);
   fChain->SetBranchAddress("EventTrigInfo.fUniqueID", EventTrigInfo_fUniqueID, &b_EventTrigInfo_fUniqueID);
   fChain->SetBranchAddress("EventTrigInfo.fBits", EventTrigInfo_fBits, &b_EventTrigInfo_fBits);
   fChain->SetBranchAddress("EventTrigInfo.run", EventTrigInfo_run, &b_EventTrigInfo_run);
   fChain->SetBranchAddress("EventTrigInfo.lumiblock", EventTrigInfo_lumiblock, &b_EventTrigInfo_lumiblock);
   fChain->SetBranchAddress("EventTrigInfo.event", EventTrigInfo_event, &b_EventTrigInfo_event);
   fChain->SetBranchAddress("EventTrigInfo.time", EventTrigInfo_time, &b_EventTrigInfo_time);
   fChain->SetBranchAddress("EventTrigInfo.timens", EventTrigInfo_timens, &b_EventTrigInfo_timens);
   fChain->SetBranchAddress("EventTrigInfo.BCID", EventTrigInfo_BCID, &b_EventTrigInfo_BCID);
   fChain->SetBranchAddress("EventTrigInfo.detectormask0", EventTrigInfo_detectormask0, &b_EventTrigInfo_detectormask0);
   fChain->SetBranchAddress("EventTrigInfo.detectormask1", EventTrigInfo_detectormask1, &b_EventTrigInfo_detectormask1);
   fChain->SetBranchAddress("EventTrigInfo.statusElement", EventTrigInfo_statusElement, &b_EventTrigInfo_statusElement);
   fChain->SetBranchAddress("EventTrigInfo.extendedLevel1ID", EventTrigInfo_extendedLevel1ID, &b_EventTrigInfo_extendedLevel1ID);
   fChain->SetBranchAddress("EventTrigInfo.level1TriggerType", EventTrigInfo_level1TriggerType, &b_EventTrigInfo_level1TriggerType);
   fChain->SetBranchAddress("EventTrigInfo.level1TriggerInfo", EventTrigInfo_level1TriggerInfo, &b_EventTrigInfo_level1TriggerInfo);
   fChain->SetBranchAddress("EventTrigInfo.level2TriggerInfo", EventTrigInfo_level2TriggerInfo, &b_EventTrigInfo_level2TriggerInfo);
   fChain->SetBranchAddress("EventTrigInfo.eventFilterInfo", EventTrigInfo_eventFilterInfo, &b_EventTrigInfo_eventFilterInfo);
   fChain->SetBranchAddress("L1MuonRoI_N", &L1MuonRoI_N, &b_L1MuonRoI_N);
   fChain->SetBranchAddress("L1MuonRoI", &L1MuonRoI_, &b_L1MuonRoI_);
   fChain->SetBranchAddress("L1MuonRoI.fUniqueID", L1MuonRoI_fUniqueID, &b_L1MuonRoI_fUniqueID);
   fChain->SetBranchAddress("L1MuonRoI.fBits", L1MuonRoI_fBits, &b_L1MuonRoI_fBits);
   fChain->SetBranchAddress("L1MuonRoI.L1_RoIWord", L1MuonRoI_L1_RoIWord, &b_L1MuonRoI_L1_RoIWord);
   fChain->SetBranchAddress("L1MuonRoI.L1_RoISource", L1MuonRoI_L1_RoISource, &b_L1MuonRoI_L1_RoISource);
   fChain->SetBranchAddress("L1MuonRoI.L1_ThrNumber", L1MuonRoI_L1_ThrNumber, &b_L1MuonRoI_L1_ThrNumber);
   fChain->SetBranchAddress("L1MuonRoI.L1_RoINumber", L1MuonRoI_L1_RoINumber, &b_L1MuonRoI_L1_RoINumber);
   fChain->SetBranchAddress("L1MuonRoI.L1_SectorAddress", L1MuonRoI_L1_SectorAddress, &b_L1MuonRoI_L1_SectorAddress);
   fChain->SetBranchAddress("L1MuonRoI.L1_isAside", L1MuonRoI_L1_isAside, &b_L1MuonRoI_L1_isAside);
   fChain->SetBranchAddress("L1MuonRoI.L1_isFirstCandidate", L1MuonRoI_L1_isFirstCandidate, &b_L1MuonRoI_L1_isFirstCandidate);
   fChain->SetBranchAddress("L1MuonRoI.L1_isMoreCandInRoI", L1MuonRoI_L1_isMoreCandInRoI, &b_L1MuonRoI_L1_isMoreCandInRoI);
   fChain->SetBranchAddress("L1MuonRoI.L1_isMoreCandInSector", L1MuonRoI_L1_isMoreCandInSector, &b_L1MuonRoI_L1_isMoreCandInSector);
   fChain->SetBranchAddress("L1MuonRoI.L1_pt", L1MuonRoI_L1_pt, &b_L1MuonRoI_L1_pt);
   fChain->SetBranchAddress("L1MuonRoI.L1_eta", L1MuonRoI_L1_eta, &b_L1MuonRoI_L1_eta);
   fChain->SetBranchAddress("L1MuonRoI.L1_phi", L1MuonRoI_L1_phi, &b_L1MuonRoI_L1_phi);
   fChain->SetBranchAddress("Mufast_N", &Mufast_N, &b_Mufast_N);
   fChain->SetBranchAddress("Mufast", &Mufast_, &b_Mufast_);
   fChain->SetBranchAddress("Mufast.fUniqueID", Mufast_fUniqueID, &b_Mufast_fUniqueID);
   fChain->SetBranchAddress("Mufast.fBits", Mufast_fBits, &b_Mufast_fBits);
   fChain->SetBranchAddress("Mufast.MuFast", Mufast_MuFast, &b_Mufast_MuFast);
   fChain->SetBranchAddress("Mufast.MuFast_pt", Mufast_MuFast_pt, &b_Mufast_MuFast_pt);
   fChain->SetBranchAddress("Mufast.MuFast_eta", Mufast_MuFast_eta, &b_Mufast_MuFast_eta);
   fChain->SetBranchAddress("Mufast.MuFast_phi", Mufast_MuFast_phi, &b_Mufast_MuFast_phi);
   fChain->SetBranchAddress("Mufast.MuFast_algoID", Mufast_MuFast_algoID, &b_Mufast_MuFast_algoID);
   fChain->SetBranchAddress("Mufast.MuFast_RoIID", Mufast_MuFast_RoIID, &b_Mufast_MuFast_RoIID);
   fChain->SetBranchAddress("Mufast.MuFast_sAddress", Mufast_MuFast_sAddress, &b_Mufast_MuFast_sAddress);
   fChain->SetBranchAddress("Mufast.MuFast_radius", Mufast_MuFast_radius, &b_Mufast_MuFast_radius);
   fChain->SetBranchAddress("Mufast.MuFast_dirPhi", Mufast_MuFast_dirPhi, &b_Mufast_MuFast_dirPhi);
   fChain->SetBranchAddress("Mufast.MuFast_zeta", Mufast_MuFast_zeta, &b_Mufast_MuFast_zeta);
   fChain->SetBranchAddress("Mufast.MuFast_dirZeta", Mufast_MuFast_dirZeta, &b_Mufast_MuFast_dirZeta);
   fChain->SetBranchAddress("Mufast.MuFast_beta", Mufast_MuFast_beta, &b_Mufast_MuFast_beta);
   fChain->SetBranchAddress("Mufast.MuFast_sp1R", Mufast_MuFast_sp1R, &b_Mufast_MuFast_sp1R);
   fChain->SetBranchAddress("Mufast.MuFast_sp1Z", Mufast_MuFast_sp1Z, &b_Mufast_MuFast_sp1Z);
   fChain->SetBranchAddress("Mufast.MuFast_sp1Slope", Mufast_MuFast_sp1Slope, &b_Mufast_MuFast_sp1Slope);
   fChain->SetBranchAddress("Mufast.MuFast_sp2R", Mufast_MuFast_sp2R, &b_Mufast_MuFast_sp2R);
   fChain->SetBranchAddress("Mufast.MuFast_sp2Z", Mufast_MuFast_sp2Z, &b_Mufast_MuFast_sp2Z);
   fChain->SetBranchAddress("Mufast.MuFast_sp2Slope", Mufast_MuFast_sp2Slope, &b_Mufast_MuFast_sp2Slope);
   fChain->SetBranchAddress("Mufast.MuFast_sp3R", Mufast_MuFast_sp3R, &b_Mufast_MuFast_sp3R);
   fChain->SetBranchAddress("Mufast.MuFast_sp3Z", Mufast_MuFast_sp3Z, &b_Mufast_MuFast_sp3Z);
   fChain->SetBranchAddress("Mufast.MuFast_sp3Slope", Mufast_MuFast_sp3Slope, &b_Mufast_MuFast_sp3Slope);
   fChain->SetBranchAddress("Mufast.MuFast_brRadius", Mufast_MuFast_brRadius, &b_Mufast_MuFast_brRadius);
   fChain->SetBranchAddress("Mufast.MuFast_brSagitta", Mufast_MuFast_brSagitta, &b_Mufast_MuFast_brSagitta);
   fChain->SetBranchAddress("Mufast.MuFast_ECAlpha", Mufast_MuFast_ECAlpha, &b_Mufast_MuFast_ECAlpha);
   fChain->SetBranchAddress("Mufast.MuFast_ECBeta", Mufast_MuFast_ECBeta, &b_Mufast_MuFast_ECBeta);
   fChain->SetBranchAddress("muFast_esd", &muFast_esd_, &b_muFast_esd_);
   fChain->SetBranchAddress("muFast_esd.fUniqueID", &muFast_esd_fUniqueID, &b_muFast_esd_fUniqueID);
   fChain->SetBranchAddress("muFast_esd.fBits", &muFast_esd_fBits, &b_muFast_esd_fBits);
   fChain->SetBranchAddress("muFast_esd.m_id", &muFast_esd_m_id, &b_muFast_esd_m_id);
   fChain->SetBranchAddress("muFast_esd.m_te_id", &muFast_esd_m_te_id, &b_muFast_esd_m_te_id);
   fChain->SetBranchAddress("muFast_esd.m_error", &muFast_esd_m_error, &b_muFast_esd_m_error);
   fChain->SetBranchAddress("muFast_esd.m_lvl1_id", &muFast_esd_m_lvl1_id, &b_muFast_esd_m_lvl1_id);
   fChain->SetBranchAddress("muFast_esd.m_lumi_block", &muFast_esd_m_lumi_block, &b_muFast_esd_m_lumi_block);
   fChain->SetBranchAddress("muFast_esd.m_muondetmask", &muFast_esd_m_muondetmask, &b_muFast_esd_m_muondetmask);
   fChain->SetBranchAddress("muFast_esd.m_RoIID", &muFast_esd_m_RoIID, &b_muFast_esd_m_RoIID);
   fChain->SetBranchAddress("muFast_esd.m_RoISystem", &muFast_esd_m_RoISystem, &b_muFast_esd_m_RoISystem);
   fChain->SetBranchAddress("muFast_esd.m_RoISubsystem", &muFast_esd_m_RoISubsystem, &b_muFast_esd_m_RoISubsystem);
   fChain->SetBranchAddress("muFast_esd.m_RoISector", &muFast_esd_m_RoISector, &b_muFast_esd_m_RoISector);
   fChain->SetBranchAddress("muFast_esd.m_RoINumber", &muFast_esd_m_RoINumber, &b_muFast_esd_m_RoINumber);
   fChain->SetBranchAddress("muFast_esd.m_RoIThreshold", &muFast_esd_m_RoIThreshold, &b_muFast_esd_m_RoIThreshold);
   fChain->SetBranchAddress("muFast_esd.m_RoIEta", &muFast_esd_m_RoIEta, &b_muFast_esd_m_RoIEta);
   fChain->SetBranchAddress("muFast_esd.m_RoIPhi", &muFast_esd_m_RoIPhi, &b_muFast_esd_m_RoIPhi);
   fChain->SetBranchAddress("muFast_esd.m_rpc_pad_error", &muFast_esd_m_rpc_pad_error, &b_muFast_esd_m_rpc_pad_error);
   fChain->SetBranchAddress("muFast_esd.m_tgc_rdo_error", &muFast_esd_m_tgc_rdo_error, &b_muFast_esd_m_tgc_rdo_error);
   fChain->SetBranchAddress("muFast_esd.m_rpc1_x", &muFast_esd_m_rpc1_x, &b_muFast_esd_m_rpc1_x);
   fChain->SetBranchAddress("muFast_esd.m_rpc1_y", &muFast_esd_m_rpc1_y, &b_muFast_esd_m_rpc1_y);
   fChain->SetBranchAddress("muFast_esd.m_rpc1_z", &muFast_esd_m_rpc1_z, &b_muFast_esd_m_rpc1_z);
   fChain->SetBranchAddress("muFast_esd.m_rpc2_x", &muFast_esd_m_rpc2_x, &b_muFast_esd_m_rpc2_x);
   fChain->SetBranchAddress("muFast_esd.m_rpc2_y", &muFast_esd_m_rpc2_y, &b_muFast_esd_m_rpc2_y);
   fChain->SetBranchAddress("muFast_esd.m_rpc2_z", &muFast_esd_m_rpc2_z, &b_muFast_esd_m_rpc2_z);
   fChain->SetBranchAddress("muFast_esd.m_rpc3_x", &muFast_esd_m_rpc3_x, &b_muFast_esd_m_rpc3_x);
   fChain->SetBranchAddress("muFast_esd.m_rpc3_y", &muFast_esd_m_rpc3_y, &b_muFast_esd_m_rpc3_y);
   fChain->SetBranchAddress("muFast_esd.m_rpc3_z", &muFast_esd_m_rpc3_z, &b_muFast_esd_m_rpc3_z);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid1_eta", &muFast_esd_m_tgc_Mid1_eta, &b_muFast_esd_m_tgc_Mid1_eta);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid1_phi", &muFast_esd_m_tgc_Mid1_phi, &b_muFast_esd_m_tgc_Mid1_phi);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid1_r", &muFast_esd_m_tgc_Mid1_r, &b_muFast_esd_m_tgc_Mid1_r);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid1_z", &muFast_esd_m_tgc_Mid1_z, &b_muFast_esd_m_tgc_Mid1_z);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid2_eta", &muFast_esd_m_tgc_Mid2_eta, &b_muFast_esd_m_tgc_Mid2_eta);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid2_phi", &muFast_esd_m_tgc_Mid2_phi, &b_muFast_esd_m_tgc_Mid2_phi);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid2_r", &muFast_esd_m_tgc_Mid2_r, &b_muFast_esd_m_tgc_Mid2_r);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid2_z", &muFast_esd_m_tgc_Mid2_z, &b_muFast_esd_m_tgc_Mid2_z);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_rho_chi2", &muFast_esd_m_tgc_Mid_rho_chi2, &b_muFast_esd_m_tgc_Mid_rho_chi2);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_rho_N", &muFast_esd_m_tgc_Mid_rho_N, &b_muFast_esd_m_tgc_Mid_rho_N);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_phi_chi2", &muFast_esd_m_tgc_Mid_phi_chi2, &b_muFast_esd_m_tgc_Mid_phi_chi2);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_phi_N", &muFast_esd_m_tgc_Mid_phi_N, &b_muFast_esd_m_tgc_Mid_phi_N);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_eta", &muFast_esd_m_tgc_Inn_eta, &b_muFast_esd_m_tgc_Inn_eta);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_phi", &muFast_esd_m_tgc_Inn_phi, &b_muFast_esd_m_tgc_Inn_phi);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_r", &muFast_esd_m_tgc_Inn_r, &b_muFast_esd_m_tgc_Inn_r);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_z", &muFast_esd_m_tgc_Inn_z, &b_muFast_esd_m_tgc_Inn_z);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_rho_std", &muFast_esd_m_tgc_Inn_rho_std, &b_muFast_esd_m_tgc_Inn_rho_std);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_rho_N", &muFast_esd_m_tgc_Inn_rho_N, &b_muFast_esd_m_tgc_Inn_rho_N);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_phi_std", &muFast_esd_m_tgc_Inn_phi_std, &b_muFast_esd_m_tgc_Inn_phi_std);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_phi_N", &muFast_esd_m_tgc_Inn_phi_N, &b_muFast_esd_m_tgc_Inn_phi_N);
   fChain->SetBranchAddress("muFast_esd.m_tgc_PT", &muFast_esd_m_tgc_PT, &b_muFast_esd_m_tgc_PT);
   fChain->SetBranchAddress("muFast_esd.m_type", &muFast_esd_m_type, &b_muFast_esd_m_type);
   fChain->SetBranchAddress("muFast_esd.m_t_x_center", &muFast_esd_m_t_x_center, &b_muFast_esd_m_t_x_center);
   fChain->SetBranchAddress("muFast_esd.m_t_y_center", &muFast_esd_m_t_y_center, &b_muFast_esd_m_t_y_center);
   fChain->SetBranchAddress("muFast_esd.m_t_radius", &muFast_esd_m_t_radius, &b_muFast_esd_m_t_radius);
   fChain->SetBranchAddress("muFast_esd.m_t_ap", &muFast_esd_m_t_ap, &b_muFast_esd_m_t_ap);
   fChain->SetBranchAddress("muFast_esd.m_t_aphi", &muFast_esd_m_t_aphi, &b_muFast_esd_m_t_aphi);
   fChain->SetBranchAddress("muFast_esd.m_t_phi", &muFast_esd_m_t_phi, &b_muFast_esd_m_t_phi);
   fChain->SetBranchAddress("muFast_esd.m_extrInnerEta", &muFast_esd_m_extrInnerEta, &b_muFast_esd_m_extrInnerEta);
   fChain->SetBranchAddress("muFast_esd.m_extrInnerPhi", &muFast_esd_m_extrInnerPhi, &b_muFast_esd_m_extrInnerPhi);
   fChain->SetBranchAddress("muFast_esd.m_tr_chamber_type_1", &muFast_esd_m_tr_chamber_type_1, &b_muFast_esd_m_tr_chamber_type_1);
   fChain->SetBranchAddress("muFast_esd.m_tr_chamber_type_2", &muFast_esd_m_tr_chamber_type_2, &b_muFast_esd_m_tr_chamber_type_2);
   fChain->SetBranchAddress("muFast_esd.m_tr_pos", &muFast_esd_m_tr_pos, &b_muFast_esd_m_tr_pos);
   fChain->SetBranchAddress("muFast_esd.m_tr_aw", &muFast_esd_m_tr_aw, &b_muFast_esd_m_tr_aw);
   fChain->SetBranchAddress("muFast_esd.m_tr_bw", &muFast_esd_m_tr_bw, &b_muFast_esd_m_tr_bw);
   fChain->SetBranchAddress("muFast_esd.m_tr_zetaMin", &muFast_esd_m_tr_zetaMin, &b_muFast_esd_m_tr_zetaMin);
   fChain->SetBranchAddress("muFast_esd.m_tr_zetaMax", &muFast_esd_m_tr_zetaMax, &b_muFast_esd_m_tr_zetaMax);
   fChain->SetBranchAddress("muFast_esd.m_tr_radMin", &muFast_esd_m_tr_radMin, &b_muFast_esd_m_tr_radMin);
   fChain->SetBranchAddress("muFast_esd.m_tr_radMax", &muFast_esd_m_tr_radMax, &b_muFast_esd_m_tr_radMax);
   fChain->SetBranchAddress("muFast_esd.m_tr_etaMin", &muFast_esd_m_tr_etaMin, &b_muFast_esd_m_tr_etaMin);
   fChain->SetBranchAddress("muFast_esd.m_tr_etaMax", &muFast_esd_m_tr_etaMax, &b_muFast_esd_m_tr_etaMax);
   fChain->SetBranchAddress("muFast_esd.m_tr_st_phi", &muFast_esd_m_tr_st_phi, &b_muFast_esd_m_tr_st_phi);
   fChain->SetBranchAddress("muFast_esd.m_tr_st_roads", &muFast_esd_m_tr_st_roads, &b_muFast_esd_m_tr_st_roads);
   fChain->SetBranchAddress("muFast_esd.m_ov_chamber_type_1", &muFast_esd_m_ov_chamber_type_1, &b_muFast_esd_m_ov_chamber_type_1);
   fChain->SetBranchAddress("muFast_esd.m_ov_chamber_type_2", &muFast_esd_m_ov_chamber_type_2, &b_muFast_esd_m_ov_chamber_type_2);
   fChain->SetBranchAddress("muFast_esd.m_ov_pos", &muFast_esd_m_ov_pos, &b_muFast_esd_m_ov_pos);
   fChain->SetBranchAddress("muFast_esd.m_ov_aw", &muFast_esd_m_ov_aw, &b_muFast_esd_m_ov_aw);
   fChain->SetBranchAddress("muFast_esd.m_ov_bw", &muFast_esd_m_ov_bw, &b_muFast_esd_m_ov_bw);
   fChain->SetBranchAddress("muFast_esd.m_ov_zetaMin", &muFast_esd_m_ov_zetaMin, &b_muFast_esd_m_ov_zetaMin);
   fChain->SetBranchAddress("muFast_esd.m_ov_zetaMax", &muFast_esd_m_ov_zetaMax, &b_muFast_esd_m_ov_zetaMax);
   fChain->SetBranchAddress("muFast_esd.m_ov_radMin", &muFast_esd_m_ov_radMin, &b_muFast_esd_m_ov_radMin);
   fChain->SetBranchAddress("muFast_esd.m_ov_radMax", &muFast_esd_m_ov_radMax, &b_muFast_esd_m_ov_radMax);
   fChain->SetBranchAddress("muFast_esd.m_ov_etaMin", &muFast_esd_m_ov_etaMin, &b_muFast_esd_m_ov_etaMin);
   fChain->SetBranchAddress("muFast_esd.m_ov_etaMax", &muFast_esd_m_ov_etaMax, &b_muFast_esd_m_ov_etaMax);
   fChain->SetBranchAddress("muFast_esd.m_ov_st_phi", &muFast_esd_m_ov_st_phi, &b_muFast_esd_m_ov_st_phi);
   fChain->SetBranchAddress("muFast_esd.m_ov_st_roads", &muFast_esd_m_ov_st_roads, &b_muFast_esd_m_ov_st_roads);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Inner_slope", &muFast_esd_m_mdt_Inner_slope, &b_muFast_esd_m_mdt_Inner_slope);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Inner_intercept", &muFast_esd_m_mdt_Inner_intercept, &b_muFast_esd_m_mdt_Inner_intercept);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Inner_Z", &muFast_esd_m_mdt_Inner_Z, &b_muFast_esd_m_mdt_Inner_Z);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Inner_R", &muFast_esd_m_mdt_Inner_R, &b_muFast_esd_m_mdt_Inner_R);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Inner_fit_chi", &muFast_esd_m_mdt_Inner_fit_chi, &b_muFast_esd_m_mdt_Inner_fit_chi);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Middle_slope", &muFast_esd_m_mdt_Middle_slope, &b_muFast_esd_m_mdt_Middle_slope);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Middle_intercept", &muFast_esd_m_mdt_Middle_intercept, &b_muFast_esd_m_mdt_Middle_intercept);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Middle_Z", &muFast_esd_m_mdt_Middle_Z, &b_muFast_esd_m_mdt_Middle_Z);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Middle_R", &muFast_esd_m_mdt_Middle_R, &b_muFast_esd_m_mdt_Middle_R);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Middle_fit_chi", &muFast_esd_m_mdt_Middle_fit_chi, &b_muFast_esd_m_mdt_Middle_fit_chi);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Outer_slope", &muFast_esd_m_mdt_Outer_slope, &b_muFast_esd_m_mdt_Outer_slope);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Outer_intercept", &muFast_esd_m_mdt_Outer_intercept, &b_muFast_esd_m_mdt_Outer_intercept);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Outer_Z", &muFast_esd_m_mdt_Outer_Z, &b_muFast_esd_m_mdt_Outer_Z);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Outer_R", &muFast_esd_m_mdt_Outer_R, &b_muFast_esd_m_mdt_Outer_R);
   fChain->SetBranchAddress("muFast_esd.m_mdt_Outer_fit_chi", &muFast_esd_m_mdt_Outer_fit_chi, &b_muFast_esd_m_mdt_Outer_fit_chi);
   fChain->SetBranchAddress("muFast_esd.m_Address", &muFast_esd_m_Address, &b_muFast_esd_m_Address);
   fChain->SetBranchAddress("muFast_esd.m_Sagitta", &muFast_esd_m_Sagitta, &b_muFast_esd_m_Sagitta);
   fChain->SetBranchAddress("muFast_esd.m_Radius", &muFast_esd_m_Radius, &b_muFast_esd_m_Radius);
   fChain->SetBranchAddress("muFast_esd.m_Slope", &muFast_esd_m_Slope, &b_muFast_esd_m_Slope);
   fChain->SetBranchAddress("muFast_esd.m_Intercept", &muFast_esd_m_Intercept, &b_muFast_esd_m_Intercept);
   fChain->SetBranchAddress("muFast_esd.m_Alpha", &muFast_esd_m_Alpha, &b_muFast_esd_m_Alpha);
   fChain->SetBranchAddress("muFast_esd.m_Beta", &muFast_esd_m_Beta, &b_muFast_esd_m_Beta);
   fChain->SetBranchAddress("muFast_esd.m_DeltaR", &muFast_esd_m_DeltaR, &b_muFast_esd_m_DeltaR);
   fChain->SetBranchAddress("muFast_esd.m_Speed_over_c", &muFast_esd_m_Speed_over_c, &b_muFast_esd_m_Speed_over_c);
   fChain->SetBranchAddress("muFast_esd.m_Eta", &muFast_esd_m_Eta, &b_muFast_esd_m_Eta);
   fChain->SetBranchAddress("muFast_esd.m_PhiMap", &muFast_esd_m_PhiMap, &b_muFast_esd_m_PhiMap);
   fChain->SetBranchAddress("muFast_esd.m_Phi", &muFast_esd_m_Phi, &b_muFast_esd_m_Phi);
   fChain->SetBranchAddress("muFast_esd.m_PhiDir", &muFast_esd_m_PhiDir, &b_muFast_esd_m_PhiDir);
   fChain->SetBranchAddress("muFast_esd.m_Pt", &muFast_esd_m_Pt, &b_muFast_esd_m_Pt);
   fChain->SetBranchAddress("muFast_esd.m_Charge", &muFast_esd_m_Charge, &b_muFast_esd_m_Charge);
   fChain->SetBranchAddress("muFast_esd.m_extension0", &muFast_esd_m_extension0, &b_muFast_esd_m_extension0);
   fChain->SetBranchAddress("muFast_esd.m_extension1", &muFast_esd_m_extension1, &b_muFast_esd_m_extension1);
   fChain->SetBranchAddress("muFast_esd.m_extension2", &muFast_esd_m_extension2, &b_muFast_esd_m_extension2);
   fChain->SetBranchAddress("muFast_esd.m_extension3", &muFast_esd_m_extension3, &b_muFast_esd_m_extension3);
   fChain->SetBranchAddress("muFast_esd.m_extension4", &muFast_esd_m_extension4, &b_muFast_esd_m_extension4);
   fChain->SetBranchAddress("muFast_esd.m_extension5", &muFast_esd_m_extension5, &b_muFast_esd_m_extension5);
   fChain->SetBranchAddress("muFast_esd.m_extension6", &muFast_esd_m_extension6, &b_muFast_esd_m_extension6);
   fChain->SetBranchAddress("muFast_esd.m_extension7", &muFast_esd_m_extension7, &b_muFast_esd_m_extension7);
   fChain->SetBranchAddress("muFast_esd.m_extension8", &muFast_esd_m_extension8, &b_muFast_esd_m_extension8);
   fChain->SetBranchAddress("muFast_esd.m_extension9", &muFast_esd_m_extension9, &b_muFast_esd_m_extension9);
   fChain->SetBranchAddress("muFast_esd.m_lvl1_emulation", &muFast_esd_m_lvl1_emulation, &b_muFast_esd_m_lvl1_emulation);
   fChain->SetBranchAddress("muFast_esd.m_rob_id", &muFast_esd_m_rob_id, &b_muFast_esd_m_rob_id);
   fChain->SetBranchAddress("muFast_esd.m_csm_id", &muFast_esd_m_csm_id, &b_muFast_esd_m_csm_id);
   fChain->SetBranchAddress("muFast_esd.m_csm_size", &muFast_esd_m_csm_size, &b_muFast_esd_m_csm_size);
   fChain->SetBranchAddress("muFast_esd.m_csm_error", &muFast_esd_m_csm_error, &b_muFast_esd_m_csm_error);
   fChain->SetBranchAddress("muFast_esd.m_removed_rob_id", &muFast_esd_m_removed_rob_id, &b_muFast_esd_m_removed_rob_id);
   fChain->SetBranchAddress("muFast_esd.m_removed_csm_id", &muFast_esd_m_removed_csm_id, &b_muFast_esd_m_removed_csm_id);
   fChain->SetBranchAddress("muFast_esd.m_pad_hit_onlineId", &muFast_esd_m_pad_hit_onlineId, &b_muFast_esd_m_pad_hit_onlineId);
   fChain->SetBranchAddress("muFast_esd.m_pad_hit_code", &muFast_esd_m_pad_hit_code, &b_muFast_esd_m_pad_hit_code);
   fChain->SetBranchAddress("muFast_esd.m_pad_hit_x", &muFast_esd_m_pad_hit_x, &b_muFast_esd_m_pad_hit_x);
   fChain->SetBranchAddress("muFast_esd.m_pad_hit_y", &muFast_esd_m_pad_hit_y, &b_muFast_esd_m_pad_hit_y);
   fChain->SetBranchAddress("muFast_esd.m_pad_hit_z", &muFast_esd_m_pad_hit_z, &b_muFast_esd_m_pad_hit_z);
   fChain->SetBranchAddress("muFast_esd.m_pad_hit_r", &muFast_esd_m_pad_hit_r, &b_muFast_esd_m_pad_hit_r);
   fChain->SetBranchAddress("muFast_esd.m_pad_hit_p", &muFast_esd_m_pad_hit_p, &b_muFast_esd_m_pad_hit_p);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_rho_hit_phi", &muFast_esd_m_tgc_Inn_rho_hit_phi, &b_muFast_esd_m_tgc_Inn_rho_hit_phi);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_rho_hit_r", &muFast_esd_m_tgc_Inn_rho_hit_r, &b_muFast_esd_m_tgc_Inn_rho_hit_r);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_rho_hit_z", &muFast_esd_m_tgc_Inn_rho_hit_z, &b_muFast_esd_m_tgc_Inn_rho_hit_z);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_rho_hit_width", &muFast_esd_m_tgc_Inn_rho_hit_width, &b_muFast_esd_m_tgc_Inn_rho_hit_width);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_rho_hit_in_seg", &muFast_esd_m_tgc_Inn_rho_hit_in_seg, &b_muFast_esd_m_tgc_Inn_rho_hit_in_seg);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_phi_hit_phi", &muFast_esd_m_tgc_Inn_phi_hit_phi, &b_muFast_esd_m_tgc_Inn_phi_hit_phi);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_phi_hit_r", &muFast_esd_m_tgc_Inn_phi_hit_r, &b_muFast_esd_m_tgc_Inn_phi_hit_r);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_phi_hit_z", &muFast_esd_m_tgc_Inn_phi_hit_z, &b_muFast_esd_m_tgc_Inn_phi_hit_z);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_phi_hit_width", &muFast_esd_m_tgc_Inn_phi_hit_width, &b_muFast_esd_m_tgc_Inn_phi_hit_width);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Inn_phi_hit_in_seg", &muFast_esd_m_tgc_Inn_phi_hit_in_seg, &b_muFast_esd_m_tgc_Inn_phi_hit_in_seg);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_rho_hit_phi", &muFast_esd_m_tgc_Mid_rho_hit_phi, &b_muFast_esd_m_tgc_Mid_rho_hit_phi);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_rho_hit_r", &muFast_esd_m_tgc_Mid_rho_hit_r, &b_muFast_esd_m_tgc_Mid_rho_hit_r);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_rho_hit_z", &muFast_esd_m_tgc_Mid_rho_hit_z, &b_muFast_esd_m_tgc_Mid_rho_hit_z);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_rho_hit_width", &muFast_esd_m_tgc_Mid_rho_hit_width, &b_muFast_esd_m_tgc_Mid_rho_hit_width);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_rho_hit_in_seg", &muFast_esd_m_tgc_Mid_rho_hit_in_seg, &b_muFast_esd_m_tgc_Mid_rho_hit_in_seg);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_phi_hit_phi", &muFast_esd_m_tgc_Mid_phi_hit_phi, &b_muFast_esd_m_tgc_Mid_phi_hit_phi);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_phi_hit_r", &muFast_esd_m_tgc_Mid_phi_hit_r, &b_muFast_esd_m_tgc_Mid_phi_hit_r);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_phi_hit_z", &muFast_esd_m_tgc_Mid_phi_hit_z, &b_muFast_esd_m_tgc_Mid_phi_hit_z);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_phi_hit_width", &muFast_esd_m_tgc_Mid_phi_hit_width, &b_muFast_esd_m_tgc_Mid_phi_hit_width);
   fChain->SetBranchAddress("muFast_esd.m_tgc_Mid_phi_hit_in_seg", &muFast_esd_m_tgc_Mid_phi_hit_in_seg, &b_muFast_esd_m_tgc_Mid_phi_hit_in_seg);
   fChain->SetBranchAddress("muFast_esd.m_mdt_onlineId", &muFast_esd_m_mdt_onlineId, &b_muFast_esd_m_mdt_onlineId);
   fChain->SetBranchAddress("muFast_esd.m_mdt_offlineId", &muFast_esd_m_mdt_offlineId, &b_muFast_esd_m_mdt_offlineId);
   fChain->SetBranchAddress("muFast_esd.m_mdt_tube_r", &muFast_esd_m_mdt_tube_r, &b_muFast_esd_m_mdt_tube_r);
   fChain->SetBranchAddress("muFast_esd.m_mdt_tube_z", &muFast_esd_m_mdt_tube_z, &b_muFast_esd_m_mdt_tube_z);
   fChain->SetBranchAddress("muFast_esd.m_mdt_tube_residual", &muFast_esd_m_mdt_tube_residual, &b_muFast_esd_m_mdt_tube_residual);
   fChain->SetBranchAddress("muFast_esd.m_mdt_tube_time", &muFast_esd_m_mdt_tube_time, &b_muFast_esd_m_mdt_tube_time);
   fChain->SetBranchAddress("muFast_esd.m_mdt_tube_space", &muFast_esd_m_mdt_tube_space, &b_muFast_esd_m_mdt_tube_space);
   fChain->SetBranchAddress("muFast_esd.m_mdt_tube_sigma", &muFast_esd_m_mdt_tube_sigma, &b_muFast_esd_m_mdt_tube_sigma);
   fChain->SetBranchAddress("InDet_SiTrk_Muon_N", &InDet_SiTrk_Muon_N, &b_InDet_SiTrk_Muon_N);
   fChain->SetBranchAddress("InDet_SiTrk_Muon", &InDet_SiTrk_Muon_, &b_InDet_SiTrk_Muon_);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.fUniqueID", InDet_SiTrk_Muon_fUniqueID, &b_InDet_SiTrk_Muon_fUniqueID);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.fBits", InDet_SiTrk_Muon_fBits, &b_InDet_SiTrk_Muon_fBits);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_algo", InDet_SiTrk_Muon_InDet_algo, &b_InDet_SiTrk_Muon_InDet_algo);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_pt", InDet_SiTrk_Muon_InDet_pt, &b_InDet_SiTrk_Muon_InDet_pt);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_eta", InDet_SiTrk_Muon_InDet_eta, &b_InDet_SiTrk_Muon_InDet_eta);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_phi", InDet_SiTrk_Muon_InDet_phi, &b_InDet_SiTrk_Muon_InDet_phi);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_a0", InDet_SiTrk_Muon_InDet_a0, &b_InDet_SiTrk_Muon_InDet_a0);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_z0", InDet_SiTrk_Muon_InDet_z0, &b_InDet_SiTrk_Muon_InDet_z0);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_chi2", InDet_SiTrk_Muon_InDet_chi2, &b_InDet_SiTrk_Muon_InDet_chi2);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_ept", InDet_SiTrk_Muon_InDet_ept, &b_InDet_SiTrk_Muon_InDet_ept);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_eeta", InDet_SiTrk_Muon_InDet_eeta, &b_InDet_SiTrk_Muon_InDet_eeta);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_ephi", InDet_SiTrk_Muon_InDet_ephi, &b_InDet_SiTrk_Muon_InDet_ephi);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_ea0", InDet_SiTrk_Muon_InDet_ea0, &b_InDet_SiTrk_Muon_InDet_ea0);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_ez0", InDet_SiTrk_Muon_InDet_ez0, &b_InDet_SiTrk_Muon_InDet_ez0);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_nPIX", InDet_SiTrk_Muon_InDet_nPIX, &b_InDet_SiTrk_Muon_InDet_nPIX);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_nSCT", InDet_SiTrk_Muon_InDet_nSCT, &b_InDet_SiTrk_Muon_InDet_nSCT);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_hitPattern", InDet_SiTrk_Muon_InDet_hitPattern, &b_InDet_SiTrk_Muon_InDet_hitPattern);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_nStrawHits", InDet_SiTrk_Muon_InDet_nStrawHits, &b_InDet_SiTrk_Muon_InDet_nStrawHits);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_nStraw", InDet_SiTrk_Muon_InDet_nStraw, &b_InDet_SiTrk_Muon_InDet_nStraw);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_nStrawTime", InDet_SiTrk_Muon_InDet_nStrawTime, &b_InDet_SiTrk_Muon_InDet_nStrawTime);
   fChain->SetBranchAddress("InDet_SiTrk_Muon.InDet_nTRHits", InDet_SiTrk_Muon_InDet_nTRHits, &b_InDet_SiTrk_Muon_InDet_nTRHits);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon_N", &InDet_IDSCAN_Muon_N, &b_InDet_IDSCAN_Muon_N);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon", &InDet_IDSCAN_Muon_, &b_InDet_IDSCAN_Muon_);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.fUniqueID", InDet_IDSCAN_Muon_fUniqueID, &b_InDet_IDSCAN_Muon_fUniqueID);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.fBits", InDet_IDSCAN_Muon_fBits, &b_InDet_IDSCAN_Muon_fBits);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_algo", InDet_IDSCAN_Muon_InDet_algo, &b_InDet_IDSCAN_Muon_InDet_algo);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_pt", InDet_IDSCAN_Muon_InDet_pt, &b_InDet_IDSCAN_Muon_InDet_pt);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_eta", InDet_IDSCAN_Muon_InDet_eta, &b_InDet_IDSCAN_Muon_InDet_eta);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_phi", InDet_IDSCAN_Muon_InDet_phi, &b_InDet_IDSCAN_Muon_InDet_phi);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_a0", InDet_IDSCAN_Muon_InDet_a0, &b_InDet_IDSCAN_Muon_InDet_a0);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_z0", InDet_IDSCAN_Muon_InDet_z0, &b_InDet_IDSCAN_Muon_InDet_z0);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_chi2", InDet_IDSCAN_Muon_InDet_chi2, &b_InDet_IDSCAN_Muon_InDet_chi2);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_ept", InDet_IDSCAN_Muon_InDet_ept, &b_InDet_IDSCAN_Muon_InDet_ept);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_eeta", InDet_IDSCAN_Muon_InDet_eeta, &b_InDet_IDSCAN_Muon_InDet_eeta);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_ephi", InDet_IDSCAN_Muon_InDet_ephi, &b_InDet_IDSCAN_Muon_InDet_ephi);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_ea0", InDet_IDSCAN_Muon_InDet_ea0, &b_InDet_IDSCAN_Muon_InDet_ea0);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_ez0", InDet_IDSCAN_Muon_InDet_ez0, &b_InDet_IDSCAN_Muon_InDet_ez0);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_nPIX", InDet_IDSCAN_Muon_InDet_nPIX, &b_InDet_IDSCAN_Muon_InDet_nPIX);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_nSCT", InDet_IDSCAN_Muon_InDet_nSCT, &b_InDet_IDSCAN_Muon_InDet_nSCT);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_hitPattern", InDet_IDSCAN_Muon_InDet_hitPattern, &b_InDet_IDSCAN_Muon_InDet_hitPattern);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_nStrawHits", InDet_IDSCAN_Muon_InDet_nStrawHits, &b_InDet_IDSCAN_Muon_InDet_nStrawHits);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_nStraw", InDet_IDSCAN_Muon_InDet_nStraw, &b_InDet_IDSCAN_Muon_InDet_nStraw);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_nStrawTime", InDet_IDSCAN_Muon_InDet_nStrawTime, &b_InDet_IDSCAN_Muon_InDet_nStrawTime);
   fChain->SetBranchAddress("InDet_IDSCAN_Muon.InDet_nTRHits", InDet_IDSCAN_Muon_InDet_nTRHits, &b_InDet_IDSCAN_Muon_InDet_nTRHits);
   fChain->SetBranchAddress("Mucomb_N", &Mucomb_N, &b_Mucomb_N);
   fChain->SetBranchAddress("Mucomb", &Mucomb_, &b_Mucomb_);
   fChain->SetBranchAddress("Mucomb.fUniqueID", Mucomb_fUniqueID, &b_Mucomb_fUniqueID);
   fChain->SetBranchAddress("Mucomb.fBits", Mucomb_fBits, &b_Mucomb_fBits);
   fChain->SetBranchAddress("Mucomb.MuComb_pt", Mucomb_MuComb_pt, &b_Mucomb_MuComb_pt);
   fChain->SetBranchAddress("Mucomb.MuComb_eta", Mucomb_MuComb_eta, &b_Mucomb_MuComb_eta);
   fChain->SetBranchAddress("Mucomb.MuComb_phi", Mucomb_MuComb_phi, &b_Mucomb_MuComb_phi);
   fChain->SetBranchAddress("Mucomb.MuComb_sigmaPt", Mucomb_MuComb_sigmaPt, &b_Mucomb_MuComb_sigmaPt);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_algo", Mucomb_MuComb_ID_algo, &b_Mucomb_MuComb_ID_algo);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_pt", Mucomb_MuComb_ID_pt, &b_Mucomb_MuComb_ID_pt);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_eta", Mucomb_MuComb_ID_eta, &b_Mucomb_MuComb_ID_eta);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_phi", Mucomb_MuComb_ID_phi, &b_Mucomb_MuComb_ID_phi);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_a0", Mucomb_MuComb_ID_a0, &b_Mucomb_MuComb_ID_a0);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_z0", Mucomb_MuComb_ID_z0, &b_Mucomb_MuComb_ID_z0);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_chi2", Mucomb_MuComb_ID_chi2, &b_Mucomb_MuComb_ID_chi2);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_ept", Mucomb_MuComb_ID_ept, &b_Mucomb_MuComb_ID_ept);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_eeta", Mucomb_MuComb_ID_eeta, &b_Mucomb_MuComb_ID_eeta);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_ephi", Mucomb_MuComb_ID_ephi, &b_Mucomb_MuComb_ID_ephi);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_ea0", Mucomb_MuComb_ID_ea0, &b_Mucomb_MuComb_ID_ea0);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_ez0", Mucomb_MuComb_ID_ez0, &b_Mucomb_MuComb_ID_ez0);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_nPIX", Mucomb_MuComb_ID_nPIX, &b_Mucomb_MuComb_ID_nPIX);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_nSCT", Mucomb_MuComb_ID_nSCT, &b_Mucomb_MuComb_ID_nSCT);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_hitPattern", Mucomb_MuComb_ID_hitPattern, &b_Mucomb_MuComb_ID_hitPattern);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_nStrawHits", Mucomb_MuComb_ID_nStrawHits, &b_Mucomb_MuComb_ID_nStrawHits);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_nStraw", Mucomb_MuComb_ID_nStraw, &b_Mucomb_MuComb_ID_nStraw);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_nStrawTime", Mucomb_MuComb_ID_nStrawTime, &b_Mucomb_MuComb_ID_nStrawTime);
   fChain->SetBranchAddress("Mucomb.MuComb_ID_nTRHits", Mucomb_MuComb_ID_nTRHits, &b_Mucomb_MuComb_ID_nTRHits);
   fChain->SetBranchAddress("Muisol_N", &Muisol_N, &b_Muisol_N);
   fChain->SetBranchAddress("Muisol", &Muisol_, &b_Muisol_);
   fChain->SetBranchAddress("Muisol.fUniqueID", Muisol_fUniqueID, &b_Muisol_fUniqueID);
   fChain->SetBranchAddress("Muisol.fBits", Muisol_fBits, &b_Muisol_fBits);
   fChain->SetBranchAddress("Muisol.MuIso_pt", Muisol_MuIso_pt, &b_Muisol_MuIso_pt);
   fChain->SetBranchAddress("Muisol.MuIso_eta", Muisol_MuIso_eta, &b_Muisol_MuIso_eta);
   fChain->SetBranchAddress("Muisol.MuIso_phi", Muisol_MuIso_phi, &b_Muisol_MuIso_phi);
   fChain->SetBranchAddress("Muisol.MuIso_EtICEC", Muisol_MuIso_EtICEC, &b_Muisol_MuIso_EtICEC);
   fChain->SetBranchAddress("Muisol.MuIso_EtOCEC", Muisol_MuIso_EtOCEC, &b_Muisol_MuIso_EtOCEC);
   fChain->SetBranchAddress("Muisol.MuIso_EtICHC", Muisol_MuIso_EtICHC, &b_Muisol_MuIso_EtICHC);
   fChain->SetBranchAddress("Muisol.MuIso_EtOCHC", Muisol_MuIso_EtOCHC, &b_Muisol_MuIso_EtOCHC);
   fChain->SetBranchAddress("Muisol.MuIso_nTracks", Muisol_MuIso_nTracks, &b_Muisol_MuIso_nTracks);
   fChain->SetBranchAddress("Muisol.MuIso_sumPtTracks", Muisol_MuIso_sumPtTracks, &b_Muisol_MuIso_sumPtTracks);
   fChain->SetBranchAddress("Muisol.MuIso_ptMuTracks", Muisol_MuIso_ptMuTracks, &b_Muisol_MuIso_ptMuTracks);
   fChain->SetBranchAddress("Muisol.MuIso_RoIId", Muisol_MuIso_RoIId, &b_Muisol_MuIso_RoIId);
   fChain->SetBranchAddress("Muisol.MuIso_LArWeight", Muisol_MuIso_LArWeight, &b_Muisol_MuIso_LArWeight);
   fChain->SetBranchAddress("Muisol.MuIso_TileWeight", Muisol_MuIso_TileWeight, &b_Muisol_MuIso_TileWeight);
   fChain->SetBranchAddress("Tilemu_N", &Tilemu_N, &b_Tilemu_N);
   fChain->SetBranchAddress("Tilemu", &Tilemu_, &b_Tilemu_);
   fChain->SetBranchAddress("Tilemu.fUniqueID", Tilemu_fUniqueID, &b_Tilemu_fUniqueID);
   fChain->SetBranchAddress("Tilemu.fBits", Tilemu_fBits, &b_Tilemu_fBits);
   fChain->SetBranchAddress("Tilemu.TileMu_eta", Tilemu_TileMu_eta, &b_Tilemu_TileMu_eta);
   fChain->SetBranchAddress("Tilemu.TileMu_phi", Tilemu_TileMu_phi, &b_Tilemu_TileMu_phi);
   fChain->SetBranchAddress("Tilemu.TileMu_enedep", Tilemu_TileMu_enedep, &b_Tilemu_TileMu_enedep);
   fChain->SetBranchAddress("Tilemu.TileMu_quality", Tilemu_TileMu_quality, &b_Tilemu_TileMu_quality);
   fChain->SetBranchAddress("TileTrackMu_N", &TileTrackMu_N, &b_TileTrackMu_N);
   fChain->SetBranchAddress("TileTrackMu", &TileTrackMu_, &b_TileTrackMu_);
   fChain->SetBranchAddress("TileTrackMu.fUniqueID", TileTrackMu_fUniqueID, &b_TileTrackMu_fUniqueID);
   fChain->SetBranchAddress("TileTrackMu.fBits", TileTrackMu_fBits, &b_TileTrackMu_fBits);
   fChain->SetBranchAddress("TileTrackMu.TileMu_pt", TileTrackMu_TileMu_pt, &b_TileTrackMu_TileMu_pt);
   fChain->SetBranchAddress("TileTrackMu.TileMu_eta", TileTrackMu_TileMu_eta, &b_TileTrackMu_TileMu_eta);
   fChain->SetBranchAddress("TileTrackMu.TileMu_phi", TileTrackMu_TileMu_phi, &b_TileTrackMu_TileMu_phi);
   fChain->SetBranchAddress("TrigMuonEF_N", &TrigMuonEF_N, &b_TrigMuonEF_N);
   fChain->SetBranchAddress("TrigMuonEF", &TrigMuonEF_, &b_TrigMuonEF_);
   fChain->SetBranchAddress("TrigMuonEF.fUniqueID", TrigMuonEF_fUniqueID, &b_TrigMuonEF_fUniqueID);
   fChain->SetBranchAddress("TrigMuonEF.fBits", TrigMuonEF_fBits, &b_TrigMuonEF_fBits);
   fChain->SetBranchAddress("TrigMuonEF.MuonEF_muonType", TrigMuonEF_MuonEF_muonType, &b_TrigMuonEF_MuonEF_muonType);
   fChain->SetBranchAddress("TrigMuonEF.MuonEF_pt", TrigMuonEF_MuonEF_pt, &b_TrigMuonEF_MuonEF_pt);
   fChain->SetBranchAddress("TrigMuonEF.MuonEF_eta", TrigMuonEF_MuonEF_eta, &b_TrigMuonEF_MuonEF_eta);
   fChain->SetBranchAddress("TrigMuonEF.MuonEF_phi", TrigMuonEF_MuonEF_phi, &b_TrigMuonEF_MuonEF_phi);
   fChain->SetBranchAddress("TrigMuonEF.MuonEF_nTracks", TrigMuonEF_MuonEF_nTracks, &b_TrigMuonEF_MuonEF_nTracks);
   fChain->SetBranchAddress("TrigMuonEF.MuonEF_nSegments", TrigMuonEF_MuonEF_nSegments, &b_TrigMuonEF_MuonEF_nSegments);
   fChain->SetBranchAddress("TrigMuonEF.MuonEF_nMDT", TrigMuonEF_MuonEF_nMDT, &b_TrigMuonEF_MuonEF_nMDT);
   fChain->SetBranchAddress("TrigMuonEF.MuonEF_nRPC", TrigMuonEF_MuonEF_nRPC, &b_TrigMuonEF_MuonEF_nRPC);
   fChain->SetBranchAddress("TrigMuonEF.MuonEF_nTGC", TrigMuonEF_MuonEF_nTGC, &b_TrigMuonEF_MuonEF_nTGC);
   fChain->SetBranchAddress("TrigMuonEF.MuonEF_nCSC", TrigMuonEF_MuonEF_nCSC, &b_TrigMuonEF_MuonEF_nCSC);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_pt", TrigMuonEF_MuonEFMS_pt, &b_TrigMuonEF_MuonEFMS_pt);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_eta", TrigMuonEF_MuonEFMS_eta, &b_TrigMuonEF_MuonEFMS_eta);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_phi", TrigMuonEF_MuonEFMS_phi, &b_TrigMuonEF_MuonEFMS_phi);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_chi2", TrigMuonEF_MuonEFMS_chi2, &b_TrigMuonEF_MuonEFMS_chi2);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_chi2prob", TrigMuonEF_MuonEFMS_chi2prob, &b_TrigMuonEF_MuonEFMS_chi2prob);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_d0", TrigMuonEF_MuonEFMS_d0, &b_TrigMuonEF_MuonEFMS_d0);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_z0", TrigMuonEF_MuonEFMS_z0, &b_TrigMuonEF_MuonEFMS_z0);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_charge", TrigMuonEF_MuonEFMS_charge, &b_TrigMuonEF_MuonEFMS_charge);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_nCSCeta", TrigMuonEF_MuonEFMS_nCSCeta, &b_TrigMuonEF_MuonEFMS_nCSCeta);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_nCSCphi", TrigMuonEF_MuonEFMS_nCSCphi, &b_TrigMuonEF_MuonEFMS_nCSCphi);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_nRPCeta", TrigMuonEF_MuonEFMS_nRPCeta, &b_TrigMuonEF_MuonEFMS_nRPCeta);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_nRPCphi", TrigMuonEF_MuonEFMS_nRPCphi, &b_TrigMuonEF_MuonEFMS_nRPCphi);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_nTGCeta", TrigMuonEF_MuonEFMS_nTGCeta, &b_TrigMuonEF_MuonEFMS_nTGCeta);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFMS_nTGCphi", TrigMuonEF_MuonEFMS_nTGCphi, &b_TrigMuonEF_MuonEFMS_nTGCphi);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFSA_pt", TrigMuonEF_MuonEFSA_pt, &b_TrigMuonEF_MuonEFSA_pt);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFSA_eta", TrigMuonEF_MuonEFSA_eta, &b_TrigMuonEF_MuonEFSA_eta);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFSA_phi", TrigMuonEF_MuonEFSA_phi, &b_TrigMuonEF_MuonEFSA_phi);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFSA_chi2", TrigMuonEF_MuonEFSA_chi2, &b_TrigMuonEF_MuonEFSA_chi2);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFSA_chi2prob", TrigMuonEF_MuonEFSA_chi2prob, &b_TrigMuonEF_MuonEFSA_chi2prob);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFSA_d0", TrigMuonEF_MuonEFSA_d0, &b_TrigMuonEF_MuonEFSA_d0);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFSA_z0", TrigMuonEF_MuonEFSA_z0, &b_TrigMuonEF_MuonEFSA_z0);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFSA_charge", TrigMuonEF_MuonEFSA_charge, &b_TrigMuonEF_MuonEFSA_charge);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_pt", TrigMuonEF_MuonEFCB_pt, &b_TrigMuonEF_MuonEFCB_pt);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_eta", TrigMuonEF_MuonEFCB_eta, &b_TrigMuonEF_MuonEFCB_eta);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_phi", TrigMuonEF_MuonEFCB_phi, &b_TrigMuonEF_MuonEFCB_phi);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_chi2", TrigMuonEF_MuonEFCB_chi2, &b_TrigMuonEF_MuonEFCB_chi2);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_chi2prob", TrigMuonEF_MuonEFCB_chi2prob, &b_TrigMuonEF_MuonEFCB_chi2prob);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_d0", TrigMuonEF_MuonEFCB_d0, &b_TrigMuonEF_MuonEFCB_d0);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_z0", TrigMuonEF_MuonEFCB_z0, &b_TrigMuonEF_MuonEFCB_z0);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_charge", TrigMuonEF_MuonEFCB_charge, &b_TrigMuonEF_MuonEFCB_charge);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_matchChi2", TrigMuonEF_MuonEFCB_matchChi2, &b_TrigMuonEF_MuonEFCB_matchChi2);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_nSCT", TrigMuonEF_MuonEFCB_nSCT, &b_TrigMuonEF_MuonEFCB_nSCT);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_nPIX", TrigMuonEF_MuonEFCB_nPIX, &b_TrigMuonEF_MuonEFCB_nPIX);
   fChain->SetBranchAddress("TrigMuonEF.MuonEFCB_nTRT", TrigMuonEF_MuonEFCB_nTRT, &b_TrigMuonEF_MuonEFCB_nTRT);
   fChain->SetBranchAddress("TrigMuGirl_N", &TrigMuGirl_N, &b_TrigMuGirl_N);
   fChain->SetBranchAddress("TrigMuGirl", &TrigMuGirl_, &b_TrigMuGirl_);
   fChain->SetBranchAddress("TrigMuGirl.fUniqueID", TrigMuGirl_fUniqueID, &b_TrigMuGirl_fUniqueID);
   fChain->SetBranchAddress("TrigMuGirl.fBits", TrigMuGirl_fBits, &b_TrigMuGirl_fBits);
   fChain->SetBranchAddress("TrigMuGirl.MuonEF_muonType", TrigMuGirl_MuonEF_muonType, &b_TrigMuGirl_MuonEF_muonType);
   fChain->SetBranchAddress("TrigMuGirl.MuonEF_pt", TrigMuGirl_MuonEF_pt, &b_TrigMuGirl_MuonEF_pt);
   fChain->SetBranchAddress("TrigMuGirl.MuonEF_eta", TrigMuGirl_MuonEF_eta, &b_TrigMuGirl_MuonEF_eta);
   fChain->SetBranchAddress("TrigMuGirl.MuonEF_phi", TrigMuGirl_MuonEF_phi, &b_TrigMuGirl_MuonEF_phi);
   fChain->SetBranchAddress("TrigMuGirl.MuonEF_nTracks", TrigMuGirl_MuonEF_nTracks, &b_TrigMuGirl_MuonEF_nTracks);
   fChain->SetBranchAddress("TrigMuGirl.MuonEF_nSegments", TrigMuGirl_MuonEF_nSegments, &b_TrigMuGirl_MuonEF_nSegments);
   fChain->SetBranchAddress("TrigMuGirl.MuonEF_nMDT", TrigMuGirl_MuonEF_nMDT, &b_TrigMuGirl_MuonEF_nMDT);
   fChain->SetBranchAddress("TrigMuGirl.MuonEF_nRPC", TrigMuGirl_MuonEF_nRPC, &b_TrigMuGirl_MuonEF_nRPC);
   fChain->SetBranchAddress("TrigMuGirl.MuonEF_nTGC", TrigMuGirl_MuonEF_nTGC, &b_TrigMuGirl_MuonEF_nTGC);
   fChain->SetBranchAddress("TrigMuGirl.MuonEF_nCSC", TrigMuGirl_MuonEF_nCSC, &b_TrigMuGirl_MuonEF_nCSC);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_pt", TrigMuGirl_MuonEFMS_pt, &b_TrigMuGirl_MuonEFMS_pt);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_eta", TrigMuGirl_MuonEFMS_eta, &b_TrigMuGirl_MuonEFMS_eta);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_phi", TrigMuGirl_MuonEFMS_phi, &b_TrigMuGirl_MuonEFMS_phi);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_chi2", TrigMuGirl_MuonEFMS_chi2, &b_TrigMuGirl_MuonEFMS_chi2);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_chi2prob", TrigMuGirl_MuonEFMS_chi2prob, &b_TrigMuGirl_MuonEFMS_chi2prob);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_d0", TrigMuGirl_MuonEFMS_d0, &b_TrigMuGirl_MuonEFMS_d0);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_z0", TrigMuGirl_MuonEFMS_z0, &b_TrigMuGirl_MuonEFMS_z0);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_charge", TrigMuGirl_MuonEFMS_charge, &b_TrigMuGirl_MuonEFMS_charge);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_nCSCeta", TrigMuGirl_MuonEFMS_nCSCeta, &b_TrigMuGirl_MuonEFMS_nCSCeta);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_nCSCphi", TrigMuGirl_MuonEFMS_nCSCphi, &b_TrigMuGirl_MuonEFMS_nCSCphi);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_nRPCeta", TrigMuGirl_MuonEFMS_nRPCeta, &b_TrigMuGirl_MuonEFMS_nRPCeta);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_nRPCphi", TrigMuGirl_MuonEFMS_nRPCphi, &b_TrigMuGirl_MuonEFMS_nRPCphi);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_nTGCeta", TrigMuGirl_MuonEFMS_nTGCeta, &b_TrigMuGirl_MuonEFMS_nTGCeta);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFMS_nTGCphi", TrigMuGirl_MuonEFMS_nTGCphi, &b_TrigMuGirl_MuonEFMS_nTGCphi);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFSA_pt", TrigMuGirl_MuonEFSA_pt, &b_TrigMuGirl_MuonEFSA_pt);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFSA_eta", TrigMuGirl_MuonEFSA_eta, &b_TrigMuGirl_MuonEFSA_eta);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFSA_phi", TrigMuGirl_MuonEFSA_phi, &b_TrigMuGirl_MuonEFSA_phi);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFSA_chi2", TrigMuGirl_MuonEFSA_chi2, &b_TrigMuGirl_MuonEFSA_chi2);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFSA_chi2prob", TrigMuGirl_MuonEFSA_chi2prob, &b_TrigMuGirl_MuonEFSA_chi2prob);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFSA_d0", TrigMuGirl_MuonEFSA_d0, &b_TrigMuGirl_MuonEFSA_d0);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFSA_z0", TrigMuGirl_MuonEFSA_z0, &b_TrigMuGirl_MuonEFSA_z0);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFSA_charge", TrigMuGirl_MuonEFSA_charge, &b_TrigMuGirl_MuonEFSA_charge);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_pt", TrigMuGirl_MuonEFCB_pt, &b_TrigMuGirl_MuonEFCB_pt);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_eta", TrigMuGirl_MuonEFCB_eta, &b_TrigMuGirl_MuonEFCB_eta);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_phi", TrigMuGirl_MuonEFCB_phi, &b_TrigMuGirl_MuonEFCB_phi);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_chi2", TrigMuGirl_MuonEFCB_chi2, &b_TrigMuGirl_MuonEFCB_chi2);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_chi2prob", TrigMuGirl_MuonEFCB_chi2prob, &b_TrigMuGirl_MuonEFCB_chi2prob);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_d0", TrigMuGirl_MuonEFCB_d0, &b_TrigMuGirl_MuonEFCB_d0);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_z0", TrigMuGirl_MuonEFCB_z0, &b_TrigMuGirl_MuonEFCB_z0);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_charge", TrigMuGirl_MuonEFCB_charge, &b_TrigMuGirl_MuonEFCB_charge);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_matchChi2", TrigMuGirl_MuonEFCB_matchChi2, &b_TrigMuGirl_MuonEFCB_matchChi2);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_nSCT", TrigMuGirl_MuonEFCB_nSCT, &b_TrigMuGirl_MuonEFCB_nSCT);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_nPIX", TrigMuGirl_MuonEFCB_nPIX, &b_TrigMuGirl_MuonEFCB_nPIX);
   fChain->SetBranchAddress("TrigMuGirl.MuonEFCB_nTRT", TrigMuGirl_MuonEFCB_nTRT, &b_TrigMuGirl_MuonEFCB_nTRT);
   Notify();
}

Bool_t TrigMuon::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TrigMuon::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TrigMuon::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TrigMuon_cxx
