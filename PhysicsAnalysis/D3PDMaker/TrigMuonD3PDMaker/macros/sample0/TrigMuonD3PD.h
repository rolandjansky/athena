/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Oct 28 20:09:07 2010 by ROOT version 5.22/00j
// from TTree trigger/trigger
// found on file: TrigMuonD3PD_lvl0.data10_7TeV.00162690.physics_Muons.merge.AOD.f287_m588._lb0559-lb0560.1.root
//////////////////////////////////////////////////////////

#ifndef TrigMuonD3PD_h
#define TrigMuonD3PD_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class TrigMuonD3PD {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           trig_L1_mu_n;
   vector<float>   *trig_L1_mu_pt;
   vector<float>   *trig_L1_mu_eta;
   vector<float>   *trig_L1_mu_phi;
   vector<string>  *trig_L1_mu_thrName;
   Int_t           trig_L2_muonfeature_n;
   vector<float>   *trig_L2_muonfeature_pt;
   vector<float>   *trig_L2_muonfeature_eta;
   vector<float>   *trig_L2_muonfeature_phi;
   Int_t           trig_L2_muonfeaturedetails_n;
   Int_t           trig_L2_combmuonfeature_n;
   vector<float>   *trig_L2_combmuonfeature_pt;
   vector<float>   *trig_L2_combmuonfeature_eta;
   vector<float>   *trig_L2_combmuonfeature_phi;
   vector<int>     *trig_L2_combmuonfeature_idscantrk_index;
   vector<int>     *trig_L2_combmuonfeature_sitracktrk_index;
   Int_t           trig_L2_isomuonfeature_n;
   vector<float>   *trig_L2_isomuonfeature_pt;
   vector<float>   *trig_L2_isomuonfeature_eta;
   vector<float>   *trig_L2_isomuonfeature_phi;
   vector<float>   *trig_L2_isomuonfeature_EtInnerConeEC ;
   vector<float>   *trig_L2_isomuonfeature_EtOuterConeEC;
   vector<float>   *trig_L2_isomuonfeature_EtInnerConeHC;
   vector<float>   *trig_L2_isomuonfeature_EtOuterConeHC;
   vector<int>     *trig_L2_isomuonfeature_NTracksCone;
   vector<float>   *trig_L2_isomuonfeature_SumPtTracksCone;
   vector<float>   *trig_L2_isomuonfeature_PtMuTracksCone;
   Int_t           trig_L2_tilemufeature_n;
   vector<float>   *trig_L2_tilemufeature_eta;
   vector<float>   *trig_L2_tilemufeature_phi;
   Int_t           trig_L2_tiletrackmufeature_n;
   vector<float>   *trig_L2_tiletrackmufeature_pt;
   vector<float>   *trig_L2_tiletrackmufeature_eta;
   vector<float>   *trig_L2_tiletrackmufeature_phi;
   vector<int>     *trig_L2_tiletrackmufeature_tilemu_index;
   vector<int>     *trig_L2_tiletrackmufeature_idtrk_algorithmId;
   vector<short>   *trig_L2_tiletrackmufeature_idtrk_trackStatus;
   vector<float>   *trig_L2_tiletrackmufeature_idtrk_chi2Ndof;
   vector<float>   *trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_a0;
   vector<float>   *trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_z0;
   vector<float>   *trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_phi0;
   vector<float>   *trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_eta;
   vector<float>   *trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_pt;
   vector<int>     *trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_hasidtrkfitpar;
   vector<int>     *trig_L2_tiletrackmufeature_idtrk_hasidtrk;
   Int_t           trig_EF_trigmuonef_n;
   vector<int>     *trig_EF_trigmuonef_track_n;
   vector<vector<int> > *trig_EF_trigmuonef_track_MuonType;
   vector<vector<float> > *trig_EF_trigmuonef_track_MS_pt;
   vector<vector<float> > *trig_EF_trigmuonef_track_MS_eta;
   vector<vector<float> > *trig_EF_trigmuonef_track_MS_phi;
   vector<vector<int> > *trig_EF_trigmuonef_track_MS_hasMS;
   vector<vector<float> > *trig_EF_trigmuonef_track_SA_pt;
   vector<vector<float> > *trig_EF_trigmuonef_track_SA_eta;
   vector<vector<float> > *trig_EF_trigmuonef_track_SA_phi;
   vector<vector<int> > *trig_EF_trigmuonef_track_SA_hasSA;
   vector<vector<float> > *trig_EF_trigmuonef_track_CB_pt;
   vector<vector<float> > *trig_EF_trigmuonef_track_CB_eta;
   vector<vector<float> > *trig_EF_trigmuonef_track_CB_phi;
   vector<vector<int> > *trig_EF_trigmuonef_track_CB_hasCB;
   Int_t           trig_EF_trigmugirl_n;
   vector<int>     *trig_EF_trigmugirl_track_n;
   vector<vector<int> > *trig_EF_trigmugirl_track_MuonType;
   vector<vector<float> > *trig_EF_trigmugirl_track_MS_pt;
   vector<vector<float> > *trig_EF_trigmugirl_track_MS_eta;
   vector<vector<float> > *trig_EF_trigmugirl_track_MS_phi;
   vector<vector<int> > *trig_EF_trigmugirl_track_MS_hasMS;
   vector<vector<float> > *trig_EF_trigmugirl_track_SA_pt;
   vector<vector<float> > *trig_EF_trigmugirl_track_SA_eta;
   vector<vector<float> > *trig_EF_trigmugirl_track_SA_phi;
   vector<vector<int> > *trig_EF_trigmugirl_track_SA_hasSA;
   vector<vector<float> > *trig_EF_trigmugirl_track_CB_pt;
   vector<vector<float> > *trig_EF_trigmugirl_track_CB_eta;
   vector<vector<float> > *trig_EF_trigmugirl_track_CB_phi;
   vector<vector<int> > *trig_EF_trigmugirl_track_CB_hasCB;
   Int_t           trig_L2_sitrack_muon_n;
   vector<int>     *trig_L2_sitrack_muon_algorithmId;
   vector<short>   *trig_L2_sitrack_muon_trackStatus;
   vector<float>   *trig_L2_sitrack_muon_chi2Ndof;
   vector<float>   *trig_L2_sitrack_muon_a0;
   vector<float>   *trig_L2_sitrack_muon_z0;
   vector<float>   *trig_L2_sitrack_muon_phi0;
   vector<float>   *trig_L2_sitrack_muon_eta;
   vector<float>   *trig_L2_sitrack_muon_pt;
   Int_t           trig_L2_idscan_muon_n;
   vector<int>     *trig_L2_idscan_muon_algorithmId;
   vector<short>   *trig_L2_idscan_muon_trackStatus;
   vector<float>   *trig_L2_idscan_muon_chi2Ndof;
   vector<float>   *trig_L2_idscan_muon_a0;
   vector<float>   *trig_L2_idscan_muon_z0;
   vector<float>   *trig_L2_idscan_muon_phi0;
   vector<float>   *trig_L2_idscan_muon_eta;
   vector<float>   *trig_L2_idscan_muon_pt;
   Int_t           trig_L2_sitrack_isomuon_n;
   vector<int>     *trig_L2_sitrack_isomuon_algorithmId;
   vector<short>   *trig_L2_sitrack_isomuon_trackStatus;
   vector<float>   *trig_L2_sitrack_isomuon_chi2Ndof;
   vector<float>   *trig_L2_sitrack_isomuon_a0;
   vector<float>   *trig_L2_sitrack_isomuon_z0;
   vector<float>   *trig_L2_sitrack_isomuon_phi0;
   vector<float>   *trig_L2_sitrack_isomuon_eta;
   vector<float>   *trig_L2_sitrack_isomuon_pt;
   Int_t           trig_L2_idscan_isomuon_n;
   vector<int>     *trig_L2_idscan_isomuon_algorithmId;
   vector<short>   *trig_L2_idscan_isomuon_trackStatus;
   vector<float>   *trig_L2_idscan_isomuon_chi2Ndof;
   vector<float>   *trig_L2_idscan_isomuon_a0;
   vector<float>   *trig_L2_idscan_isomuon_z0;
   vector<float>   *trig_L2_idscan_isomuon_phi0;
   vector<float>   *trig_L2_idscan_isomuon_eta;
   vector<float>   *trig_L2_idscan_isomuon_pt;
   Int_t           trig_Nav_n;
   vector<short>   *trig_Nav_chain_ChainId;
   vector<vector<int> > *trig_Nav_chain_RoIType;
   vector<vector<int> > *trig_Nav_chain_RoIIndex;
   Int_t           trig_RoI_L2_mu_n;
   vector<short>   *trig_RoI_L2_mu_type;
   vector<short>   *trig_RoI_L2_mu_lastStep;
   vector<int>     *trig_RoI_L2_mu_MuonFeature;
   vector<int>     *trig_RoI_L2_mu_MuonFeatureStatus;
   vector<int>     *trig_RoI_L2_mu_MuonFeatureDetails;
   vector<int>     *trig_RoI_L2_mu_MuonFeatureDetailsStatus;
   vector<int>     *trig_RoI_L2_mu_CombinedMuonFeature;
   vector<int>     *trig_RoI_L2_mu_CombinedMuonFeatureStatus;
   vector<int>     *trig_RoI_L2_mu_IsoMuonFeature;
   vector<int>     *trig_RoI_L2_mu_IsoMuonFeatureStatus;
   vector<int>     *trig_RoI_L2_mu_Muon_ROI;
   vector<int>     *trig_RoI_L2_mu_Muon_ROIStatus;
   vector<vector<int> > *trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon;
   vector<vector<int> > *trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus;
   vector<vector<int> > *trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon;
   vector<vector<int> > *trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus;
   vector<vector<int> > *trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIso;
   vector<vector<int> > *trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIsoStatus;
   vector<vector<int> > *trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIso;
   vector<vector<int> > *trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIsoStatus;
   Int_t           trig_RoI_L2_TileMu_n;
   vector<short>   *trig_RoI_L2_TileMu_type;
   vector<short>   *trig_RoI_L2_TileMu_lastStep;
   vector<int>     *trig_RoI_L2_TileMu_TileMuFeature;
   vector<int>     *trig_RoI_L2_TileMu_TileMuFeatureStatus;
   vector<int>     *trig_RoI_L2_TileMu_TileTrackMuFeature;
   vector<int>     *trig_RoI_L2_TileMu_TileTrackMuFeatureStatus;
   vector<int>     *trig_RoI_L2_TileMu_Muon_ROI;
   vector<int>     *trig_RoI_L2_TileMu_Muon_ROIStatus;
   vector<vector<int> > *trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Tile;
   vector<vector<int> > *trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_TileStatus;
   vector<vector<int> > *trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Tile;
   vector<vector<int> > *trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_TileStatus;
   Int_t           trig_RoI_EF_mu_n;
   vector<short>   *trig_RoI_EF_mu_type;
   vector<short>   *trig_RoI_EF_mu_lastStep;
   vector<int>     *trig_RoI_EF_mu_Muon_ROI;
   vector<int>     *trig_RoI_EF_mu_Muon_ROIStatus;
   vector<vector<int> > *trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFID;
   vector<vector<int> > *trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFIDStatus;
   vector<vector<int> > *trig_RoI_EF_mu_TrigMuonEFInfoContainer;
   vector<vector<int> > *trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus;
   vector<vector<int> > *trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirl;
   vector<vector<int> > *trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirlStatus;
   vector<vector<int> > *trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EF;
   vector<vector<int> > *trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus;
   Int_t           trig_RoI_EF_TileMu_n;
   vector<short>   *trig_RoI_EF_TileMu_type;
   vector<short>   *trig_RoI_EF_TileMu_lastStep;
   vector<int>     *trig_RoI_EF_TileMu_Muon_ROI;
   vector<int>     *trig_RoI_EF_TileMu_Muon_ROIStatus;
   vector<vector<int> > *trig_RoI_EF_TileMu_TrigMuonEFContainer;
   vector<vector<int> > *trig_RoI_EF_TileMu_TrigMuonEFContainerStatus;
   vector<vector<int> > *trig_RoI_EF_TileMu_TrigMuonEFInfoContainer;
   vector<vector<int> > *trig_RoI_EF_TileMu_TrigMuonEFInfoContainerStatus;
   vector<vector<int> > *trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirl;
   vector<vector<int> > *trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirlStatus;
   vector<vector<int> > *trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EF;
   vector<vector<int> > *trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus;
   UInt_t          trig_DB_SMK;
   UInt_t          trig_DB_L1PSK;
   UInt_t          trig_DB_HLTPSK;
   UInt_t          RunNumber;
   UInt_t          EventNumber;
   UInt_t          timestamp;
   UInt_t          timestamp_ns;
   UInt_t          lbn;
   UInt_t          bcid;
   UInt_t          detmask0;
   UInt_t          detmask1;
   UInt_t          pixelFlags;
   UInt_t          sctFlags;
   UInt_t          trtFlags;
   UInt_t          larFlags;
   UInt_t          tileFlags;
   UInt_t          muonFlags;
   UInt_t          fwdFlags;
   UInt_t          coreFlags;
   UInt_t          pixelError;
   UInt_t          sctError;
   UInt_t          trtError;
   UInt_t          larError;
   UInt_t          tileError;
   UInt_t          muonError;
   UInt_t          fwdError;
   UInt_t          coreError;
   vector<unsigned int> *trig_L1_TAV;
   vector<short>   *trig_L2_passedPhysics;
   vector<short>   *trig_EF_passedPhysics;
   Int_t           muon_muid_n;
   vector<float>   *muon_muid_E;
   vector<float>   *muon_muid_pt;
   vector<float>   *muon_muid_m;
   vector<float>   *muon_muid_eta;
   vector<float>   *muon_muid_phi;
   vector<float>   *muon_muid_px;
   vector<float>   *muon_muid_py;
   vector<float>   *muon_muid_pz;
   vector<float>   *muon_muid_charge;
   vector<unsigned short> *muon_muid_allauthor;
   vector<int>     *muon_muid_hastrack;
   Bool_t          EF_2mu10;
   Bool_t          EF_2mu4;
   Bool_t          EF_mu10;
   Bool_t          EF_mu20;
   Bool_t          EF_mu20_MSonly;
   Bool_t          EF_mu4;
   Bool_t          EF_mu6;
   Bool_t          L1_MU0;
   Bool_t          L1_MU10;
   Bool_t          L1_MU20;
   Bool_t          L1_MU6;
   Bool_t          L2_2mu4;
   Bool_t          L2_mu10;
   Bool_t          L2_mu20;
   Bool_t          L2_mu20_MSonly;
   Bool_t          L2_mu4;
   Bool_t          L2_mu6;
   Int_t           muon_staco_n;
   vector<float>   *muon_staco_E;
   vector<float>   *muon_staco_pt;
   vector<float>   *muon_staco_m;
   vector<float>   *muon_staco_eta;
   vector<float>   *muon_staco_phi;
   vector<float>   *muon_staco_px;
   vector<float>   *muon_staco_py;
   vector<float>   *muon_staco_pz;
   vector<float>   *muon_staco_charge;
   vector<unsigned short> *muon_staco_allauthor;
   vector<int>     *muon_staco_hastrack;

   // List of branches
   TBranch        *b_trig_L1_mu_n;   //!
   TBranch        *b_trig_L1_mu_pt;   //!
   TBranch        *b_trig_L1_mu_eta;   //!
   TBranch        *b_trig_L1_mu_phi;   //!
   TBranch        *b_trig_L1_mu_thrName;   //!
   TBranch        *b_trig_L2_muonfeature_n;   //!
   TBranch        *b_trig_L2_muonfeature_pt;   //!
   TBranch        *b_trig_L2_muonfeature_eta;   //!
   TBranch        *b_trig_L2_muonfeature_phi;   //!
   TBranch        *b_trig_L2_muonfeaturedetails_n;   //!
   TBranch        *b_trig_L2_combmuonfeature_n;   //!
   TBranch        *b_trig_L2_combmuonfeature_pt;   //!
   TBranch        *b_trig_L2_combmuonfeature_eta;   //!
   TBranch        *b_trig_L2_combmuonfeature_phi;   //!
   TBranch        *b_trig_L2_combmuonfeature_idscantrk_index;   //!
   TBranch        *b_trig_L2_combmuonfeature_sitracktrk_index;   //!
   TBranch        *b_trig_L2_isomuonfeature_n;   //!
   TBranch        *b_trig_L2_isomuonfeature_pt;   //!
   TBranch        *b_trig_L2_isomuonfeature_eta;   //!
   TBranch        *b_trig_L2_isomuonfeature_phi;   //!
   TBranch        *b_trig_L2_isomuonfeature_EtInnerConeEC ;   //!
   TBranch        *b_trig_L2_isomuonfeature_EtOuterConeEC;   //!
   TBranch        *b_trig_L2_isomuonfeature_EtInnerConeHC;   //!
   TBranch        *b_trig_L2_isomuonfeature_EtOuterConeHC;   //!
   TBranch        *b_trig_L2_isomuonfeature_NTracksCone;   //!
   TBranch        *b_trig_L2_isomuonfeature_SumPtTracksCone;   //!
   TBranch        *b_trig_L2_isomuonfeature_PtMuTracksCone;   //!
   TBranch        *b_trig_L2_tilemufeature_n;   //!
   TBranch        *b_trig_L2_tilemufeature_eta;   //!
   TBranch        *b_trig_L2_tilemufeature_phi;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_n;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_pt;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_eta;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_phi;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_tilemu_index;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_idtrk_algorithmId;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_idtrk_trackStatus;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_idtrk_chi2Ndof;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_a0;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_z0;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_phi0;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_eta;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_pt;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_hasidtrkfitpar;   //!
   TBranch        *b_trig_L2_tiletrackmufeature_idtrk_hasidtrk;   //!
   TBranch        *b_trig_EF_trigmuonef_n;   //!
   TBranch        *b_trig_EF_trigmuonef_track_n;   //!
   TBranch        *b_trig_EF_trigmuonef_track_MuonType;   //!
   TBranch        *b_trig_EF_trigmuonef_track_MS_pt;   //!
   TBranch        *b_trig_EF_trigmuonef_track_MS_eta;   //!
   TBranch        *b_trig_EF_trigmuonef_track_MS_phi;   //!
   TBranch        *b_trig_EF_trigmuonef_track_MS_hasMS;   //!
   TBranch        *b_trig_EF_trigmuonef_track_SA_pt;   //!
   TBranch        *b_trig_EF_trigmuonef_track_SA_eta;   //!
   TBranch        *b_trig_EF_trigmuonef_track_SA_phi;   //!
   TBranch        *b_trig_EF_trigmuonef_track_SA_hasSA;   //!
   TBranch        *b_trig_EF_trigmuonef_track_CB_pt;   //!
   TBranch        *b_trig_EF_trigmuonef_track_CB_eta;   //!
   TBranch        *b_trig_EF_trigmuonef_track_CB_phi;   //!
   TBranch        *b_trig_EF_trigmuonef_track_CB_hasCB;   //!
   TBranch        *b_trig_EF_trigmugirl_n;   //!
   TBranch        *b_trig_EF_trigmugirl_track_n;   //!
   TBranch        *b_trig_EF_trigmugirl_track_MuonType;   //!
   TBranch        *b_trig_EF_trigmugirl_track_MS_pt;   //!
   TBranch        *b_trig_EF_trigmugirl_track_MS_eta;   //!
   TBranch        *b_trig_EF_trigmugirl_track_MS_phi;   //!
   TBranch        *b_trig_EF_trigmugirl_track_MS_hasMS;   //!
   TBranch        *b_trig_EF_trigmugirl_track_SA_pt;   //!
   TBranch        *b_trig_EF_trigmugirl_track_SA_eta;   //!
   TBranch        *b_trig_EF_trigmugirl_track_SA_phi;   //!
   TBranch        *b_trig_EF_trigmugirl_track_SA_hasSA;   //!
   TBranch        *b_trig_EF_trigmugirl_track_CB_pt;   //!
   TBranch        *b_trig_EF_trigmugirl_track_CB_eta;   //!
   TBranch        *b_trig_EF_trigmugirl_track_CB_phi;   //!
   TBranch        *b_trig_EF_trigmugirl_track_CB_hasCB;   //!
   TBranch        *b_trig_L2_sitrack_muon_n;   //!
   TBranch        *b_trig_L2_sitrack_muon_algorithmId;   //!
   TBranch        *b_trig_L2_sitrack_muon_trackStatus;   //!
   TBranch        *b_trig_L2_sitrack_muon_chi2Ndof;   //!
   TBranch        *b_trig_L2_sitrack_muon_a0;   //!
   TBranch        *b_trig_L2_sitrack_muon_z0;   //!
   TBranch        *b_trig_L2_sitrack_muon_phi0;   //!
   TBranch        *b_trig_L2_sitrack_muon_eta;   //!
   TBranch        *b_trig_L2_sitrack_muon_pt;   //!
   TBranch        *b_trig_L2_idscan_muon_n;   //!
   TBranch        *b_trig_L2_idscan_muon_algorithmId;   //!
   TBranch        *b_trig_L2_idscan_muon_trackStatus;   //!
   TBranch        *b_trig_L2_idscan_muon_chi2Ndof;   //!
   TBranch        *b_trig_L2_idscan_muon_a0;   //!
   TBranch        *b_trig_L2_idscan_muon_z0;   //!
   TBranch        *b_trig_L2_idscan_muon_phi0;   //!
   TBranch        *b_trig_L2_idscan_muon_eta;   //!
   TBranch        *b_trig_L2_idscan_muon_pt;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_n;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_algorithmId;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_trackStatus;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_chi2Ndof;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_a0;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_z0;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_phi0;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_eta;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_pt;   //!
   TBranch        *b_trig_L2_idscan_isomuon_n;   //!
   TBranch        *b_trig_L2_idscan_isomuon_algorithmId;   //!
   TBranch        *b_trig_L2_idscan_isomuon_trackStatus;   //!
   TBranch        *b_trig_L2_idscan_isomuon_chi2Ndof;   //!
   TBranch        *b_trig_L2_idscan_isomuon_a0;   //!
   TBranch        *b_trig_L2_idscan_isomuon_z0;   //!
   TBranch        *b_trig_L2_idscan_isomuon_phi0;   //!
   TBranch        *b_trig_L2_idscan_isomuon_eta;   //!
   TBranch        *b_trig_L2_idscan_isomuon_pt;   //!
   TBranch        *b_trig_Nav_n;   //!
   TBranch        *b_trig_Nav_chain_ChainId;   //!
   TBranch        *b_trig_Nav_chain_RoIType;   //!
   TBranch        *b_trig_Nav_chain_RoIIndex;   //!
   TBranch        *b_trig_RoI_L2_mu_n;   //!
   TBranch        *b_trig_RoI_L2_mu_type;   //!
   TBranch        *b_trig_RoI_L2_mu_lastStep;   //!
   TBranch        *b_trig_RoI_L2_mu_MuonFeature;   //!
   TBranch        *b_trig_RoI_L2_mu_MuonFeatureStatus;   //!
   TBranch        *b_trig_RoI_L2_mu_MuonFeatureDetails;   //!
   TBranch        *b_trig_RoI_L2_mu_MuonFeatureDetailsStatus;   //!
   TBranch        *b_trig_RoI_L2_mu_CombinedMuonFeature;   //!
   TBranch        *b_trig_RoI_L2_mu_CombinedMuonFeatureStatus;   //!
   TBranch        *b_trig_RoI_L2_mu_IsoMuonFeature;   //!
   TBranch        *b_trig_RoI_L2_mu_IsoMuonFeatureStatus;   //!
   TBranch        *b_trig_RoI_L2_mu_Muon_ROI;   //!
   TBranch        *b_trig_RoI_L2_mu_Muon_ROIStatus;   //!
   TBranch        *b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon;   //!
   TBranch        *b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus;   //!
   TBranch        *b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon;   //!
   TBranch        *b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus;   //!
   TBranch        *b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIso;   //!
   TBranch        *b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIsoStatus;   //!
   TBranch        *b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIso;   //!
   TBranch        *b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIsoStatus;   //!
   TBranch        *b_trig_RoI_L2_TileMu_n;   //!
   TBranch        *b_trig_RoI_L2_TileMu_type;   //!
   TBranch        *b_trig_RoI_L2_TileMu_lastStep;   //!
   TBranch        *b_trig_RoI_L2_TileMu_TileMuFeature;   //!
   TBranch        *b_trig_RoI_L2_TileMu_TileMuFeatureStatus;   //!
   TBranch        *b_trig_RoI_L2_TileMu_TileTrackMuFeature;   //!
   TBranch        *b_trig_RoI_L2_TileMu_TileTrackMuFeatureStatus;   //!
   TBranch        *b_trig_RoI_L2_TileMu_Muon_ROI;   //!
   TBranch        *b_trig_RoI_L2_TileMu_Muon_ROIStatus;   //!
   TBranch        *b_trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Tile;   //!
   TBranch        *b_trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_TileStatus;   //!
   TBranch        *b_trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Tile;   //!
   TBranch        *b_trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_TileStatus;   //!
   TBranch        *b_trig_RoI_EF_mu_n;   //!
   TBranch        *b_trig_RoI_EF_mu_type;   //!
   TBranch        *b_trig_RoI_EF_mu_lastStep;   //!
   TBranch        *b_trig_RoI_EF_mu_Muon_ROI;   //!
   TBranch        *b_trig_RoI_EF_mu_Muon_ROIStatus;   //!
   TBranch        *b_trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFID;   //!
   TBranch        *b_trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFIDStatus;   //!
   TBranch        *b_trig_RoI_EF_mu_TrigMuonEFInfoContainer;   //!
   TBranch        *b_trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus;   //!
   TBranch        *b_trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirl;   //!
   TBranch        *b_trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirlStatus;   //!
   TBranch        *b_trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EF;   //!
   TBranch        *b_trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus;   //!
   TBranch        *b_trig_RoI_EF_TileMu_n;   //!
   TBranch        *b_trig_RoI_EF_TileMu_type;   //!
   TBranch        *b_trig_RoI_EF_TileMu_lastStep;   //!
   TBranch        *b_trig_RoI_EF_TileMu_Muon_ROI;   //!
   TBranch        *b_trig_RoI_EF_TileMu_Muon_ROIStatus;   //!
   TBranch        *b_trig_RoI_EF_TileMu_TrigMuonEFContainer;   //!
   TBranch        *b_trig_RoI_EF_TileMu_TrigMuonEFContainerStatus;   //!
   TBranch        *b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainer;   //!
   TBranch        *b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainerStatus;   //!
   TBranch        *b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirl;   //!
   TBranch        *b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirlStatus;   //!
   TBranch        *b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EF;   //!
   TBranch        *b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus;   //!
   TBranch        *b_trig_DB_SMK;   //!
   TBranch        *b_trig_DB_L1PSK;   //!
   TBranch        *b_trig_DB_HLTPSK;   //!
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_timestamp;   //!
   TBranch        *b_timestamp_ns;   //!
   TBranch        *b_lbn;   //!
   TBranch        *b_bcid;   //!
   TBranch        *b_detmask0;   //!
   TBranch        *b_detmask1;   //!
   TBranch        *b_pixelFlags;   //!
   TBranch        *b_sctFlags;   //!
   TBranch        *b_trtFlags;   //!
   TBranch        *b_larFlags;   //!
   TBranch        *b_tileFlags;   //!
   TBranch        *b_muonFlags;   //!
   TBranch        *b_fwdFlags;   //!
   TBranch        *b_coreFlags;   //!
   TBranch        *b_pixelError;   //!
   TBranch        *b_sctError;   //!
   TBranch        *b_trtError;   //!
   TBranch        *b_larError;   //!
   TBranch        *b_tileError;   //!
   TBranch        *b_muonError;   //!
   TBranch        *b_fwdError;   //!
   TBranch        *b_coreError;   //!
   TBranch        *b_trig_L1_TAV;   //!
   TBranch        *b_trig_L2_passedPhysics;   //!
   TBranch        *b_trig_EF_passedPhysics;   //!
   TBranch        *b_muon_muid_n;   //!
   TBranch        *b_muon_muid_E;   //!
   TBranch        *b_muon_muid_pt;   //!
   TBranch        *b_muon_muid_m;   //!
   TBranch        *b_muon_muid_eta;   //!
   TBranch        *b_muon_muid_phi;   //!
   TBranch        *b_muon_muid_px;   //!
   TBranch        *b_muon_muid_py;   //!
   TBranch        *b_muon_muid_pz;   //!
   TBranch        *b_muon_muid_charge;   //!
   TBranch        *b_muon_muid_allauthor;   //!
   TBranch        *b_muon_muid_hastrack;   //!
   TBranch        *b_EF_2mu10;   //!
   TBranch        *b_EF_2mu4;   //!
   TBranch        *b_EF_mu10;   //!
   TBranch        *b_EF_mu20;   //!
   TBranch        *b_EF_mu20_MSonly;   //!
   TBranch        *b_EF_mu4;   //!
   TBranch        *b_EF_mu6;   //!
   TBranch        *b_L1_MU0;   //!
   TBranch        *b_L1_MU10;   //!
   TBranch        *b_L1_MU20;   //!
   TBranch        *b_L1_MU6;   //!
   TBranch        *b_L2_2mu4;   //!
   TBranch        *b_L2_mu10;   //!
   TBranch        *b_L2_mu20;   //!
   TBranch        *b_L2_mu20_MSonly;   //!
   TBranch        *b_L2_mu4;   //!
   TBranch        *b_L2_mu6;   //!
   TBranch        *b_muon_staco_n;   //!
   TBranch        *b_muon_staco_E;   //!
   TBranch        *b_muon_staco_pt;   //!
   TBranch        *b_muon_staco_m;   //!
   TBranch        *b_muon_staco_eta;   //!
   TBranch        *b_muon_staco_phi;   //!
   TBranch        *b_muon_staco_px;   //!
   TBranch        *b_muon_staco_py;   //!
   TBranch        *b_muon_staco_pz;   //!
   TBranch        *b_muon_staco_charge;   //!
   TBranch        *b_muon_staco_allauthor;   //!
   TBranch        *b_muon_staco_hastrack;   //!

   TrigMuonD3PD(TTree *tree=0);
   virtual ~TrigMuonD3PD();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TrigMuonD3PD_cxx
TrigMuonD3PD::TrigMuonD3PD(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("TrigMuonD3PD_lvl0.data10_7TeV.00162690.physics_Muons.merge.AOD.f287_m588._lb0559-lb0560.1.root");
      if (!f) {
         f = new TFile("TrigMuonD3PD_lvl0.data10_7TeV.00162690.physics_Muons.merge.AOD.f287_m588._lb0559-lb0560.1.root");
      }
      tree = (TTree*)gDirectory->Get("trigger");

   }
   Init(tree);
}

TrigMuonD3PD::~TrigMuonD3PD()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TrigMuonD3PD::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TrigMuonD3PD::LoadTree(Long64_t entry)
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

void TrigMuonD3PD::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   trig_L1_mu_pt = 0;
   trig_L1_mu_eta = 0;
   trig_L1_mu_phi = 0;
   trig_L1_mu_thrName = 0;
   trig_L2_muonfeature_pt = 0;
   trig_L2_muonfeature_eta = 0;
   trig_L2_muonfeature_phi = 0;
   trig_L2_combmuonfeature_pt = 0;
   trig_L2_combmuonfeature_eta = 0;
   trig_L2_combmuonfeature_phi = 0;
   trig_L2_combmuonfeature_idscantrk_index = 0;
   trig_L2_combmuonfeature_sitracktrk_index = 0;
   trig_L2_isomuonfeature_pt = 0;
   trig_L2_isomuonfeature_eta = 0;
   trig_L2_isomuonfeature_phi = 0;
   trig_L2_isomuonfeature_EtInnerConeEC  = 0;
   trig_L2_isomuonfeature_EtOuterConeEC = 0;
   trig_L2_isomuonfeature_EtInnerConeHC = 0;
   trig_L2_isomuonfeature_EtOuterConeHC = 0;
   trig_L2_isomuonfeature_NTracksCone = 0;
   trig_L2_isomuonfeature_SumPtTracksCone = 0;
   trig_L2_isomuonfeature_PtMuTracksCone = 0;
   trig_L2_tilemufeature_eta = 0;
   trig_L2_tilemufeature_phi = 0;
   trig_L2_tiletrackmufeature_pt = 0;
   trig_L2_tiletrackmufeature_eta = 0;
   trig_L2_tiletrackmufeature_phi = 0;
   trig_L2_tiletrackmufeature_tilemu_index = 0;
   trig_L2_tiletrackmufeature_idtrk_algorithmId = 0;
   trig_L2_tiletrackmufeature_idtrk_trackStatus = 0;
   trig_L2_tiletrackmufeature_idtrk_chi2Ndof = 0;
   trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_a0 = 0;
   trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_z0 = 0;
   trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_phi0 = 0;
   trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_eta = 0;
   trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_pt = 0;
   trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_hasidtrkfitpar = 0;
   trig_L2_tiletrackmufeature_idtrk_hasidtrk = 0;
   trig_EF_trigmuonef_track_n = 0;
   trig_EF_trigmuonef_track_MuonType = 0;
   trig_EF_trigmuonef_track_MS_pt = 0;
   trig_EF_trigmuonef_track_MS_eta = 0;
   trig_EF_trigmuonef_track_MS_phi = 0;
   trig_EF_trigmuonef_track_MS_hasMS = 0;
   trig_EF_trigmuonef_track_SA_pt = 0;
   trig_EF_trigmuonef_track_SA_eta = 0;
   trig_EF_trigmuonef_track_SA_phi = 0;
   trig_EF_trigmuonef_track_SA_hasSA = 0;
   trig_EF_trigmuonef_track_CB_pt = 0;
   trig_EF_trigmuonef_track_CB_eta = 0;
   trig_EF_trigmuonef_track_CB_phi = 0;
   trig_EF_trigmuonef_track_CB_hasCB = 0;
   trig_EF_trigmugirl_track_n = 0;
   trig_EF_trigmugirl_track_MuonType = 0;
   trig_EF_trigmugirl_track_MS_pt = 0;
   trig_EF_trigmugirl_track_MS_eta = 0;
   trig_EF_trigmugirl_track_MS_phi = 0;
   trig_EF_trigmugirl_track_MS_hasMS = 0;
   trig_EF_trigmugirl_track_SA_pt = 0;
   trig_EF_trigmugirl_track_SA_eta = 0;
   trig_EF_trigmugirl_track_SA_phi = 0;
   trig_EF_trigmugirl_track_SA_hasSA = 0;
   trig_EF_trigmugirl_track_CB_pt = 0;
   trig_EF_trigmugirl_track_CB_eta = 0;
   trig_EF_trigmugirl_track_CB_phi = 0;
   trig_EF_trigmugirl_track_CB_hasCB = 0;
   trig_L2_sitrack_muon_algorithmId = 0;
   trig_L2_sitrack_muon_trackStatus = 0;
   trig_L2_sitrack_muon_chi2Ndof = 0;
   trig_L2_sitrack_muon_a0 = 0;
   trig_L2_sitrack_muon_z0 = 0;
   trig_L2_sitrack_muon_phi0 = 0;
   trig_L2_sitrack_muon_eta = 0;
   trig_L2_sitrack_muon_pt = 0;
   trig_L2_idscan_muon_algorithmId = 0;
   trig_L2_idscan_muon_trackStatus = 0;
   trig_L2_idscan_muon_chi2Ndof = 0;
   trig_L2_idscan_muon_a0 = 0;
   trig_L2_idscan_muon_z0 = 0;
   trig_L2_idscan_muon_phi0 = 0;
   trig_L2_idscan_muon_eta = 0;
   trig_L2_idscan_muon_pt = 0;
   trig_L2_sitrack_isomuon_algorithmId = 0;
   trig_L2_sitrack_isomuon_trackStatus = 0;
   trig_L2_sitrack_isomuon_chi2Ndof = 0;
   trig_L2_sitrack_isomuon_a0 = 0;
   trig_L2_sitrack_isomuon_z0 = 0;
   trig_L2_sitrack_isomuon_phi0 = 0;
   trig_L2_sitrack_isomuon_eta = 0;
   trig_L2_sitrack_isomuon_pt = 0;
   trig_L2_idscan_isomuon_algorithmId = 0;
   trig_L2_idscan_isomuon_trackStatus = 0;
   trig_L2_idscan_isomuon_chi2Ndof = 0;
   trig_L2_idscan_isomuon_a0 = 0;
   trig_L2_idscan_isomuon_z0 = 0;
   trig_L2_idscan_isomuon_phi0 = 0;
   trig_L2_idscan_isomuon_eta = 0;
   trig_L2_idscan_isomuon_pt = 0;
   trig_Nav_chain_ChainId = 0;
   trig_Nav_chain_RoIType = 0;
   trig_Nav_chain_RoIIndex = 0;
   trig_RoI_L2_mu_type = 0;
   trig_RoI_L2_mu_lastStep = 0;
   trig_RoI_L2_mu_MuonFeature = 0;
   trig_RoI_L2_mu_MuonFeatureStatus = 0;
   trig_RoI_L2_mu_MuonFeatureDetails = 0;
   trig_RoI_L2_mu_MuonFeatureDetailsStatus = 0;
   trig_RoI_L2_mu_CombinedMuonFeature = 0;
   trig_RoI_L2_mu_CombinedMuonFeatureStatus = 0;
   trig_RoI_L2_mu_IsoMuonFeature = 0;
   trig_RoI_L2_mu_IsoMuonFeatureStatus = 0;
   trig_RoI_L2_mu_Muon_ROI = 0;
   trig_RoI_L2_mu_Muon_ROIStatus = 0;
   trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon = 0;
   trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus = 0;
   trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon = 0;
   trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus = 0;
   trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIso = 0;
   trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIsoStatus = 0;
   trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIso = 0;
   trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIsoStatus = 0;
   trig_RoI_L2_TileMu_type = 0;
   trig_RoI_L2_TileMu_lastStep = 0;
   trig_RoI_L2_TileMu_TileMuFeature = 0;
   trig_RoI_L2_TileMu_TileMuFeatureStatus = 0;
   trig_RoI_L2_TileMu_TileTrackMuFeature = 0;
   trig_RoI_L2_TileMu_TileTrackMuFeatureStatus = 0;
   trig_RoI_L2_TileMu_Muon_ROI = 0;
   trig_RoI_L2_TileMu_Muon_ROIStatus = 0;
   trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Tile = 0;
   trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_TileStatus = 0;
   trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Tile = 0;
   trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_TileStatus = 0;
   trig_RoI_EF_mu_type = 0;
   trig_RoI_EF_mu_lastStep = 0;
   trig_RoI_EF_mu_Muon_ROI = 0;
   trig_RoI_EF_mu_Muon_ROIStatus = 0;
   trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFID = 0;
   trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFIDStatus = 0;
   trig_RoI_EF_mu_TrigMuonEFInfoContainer = 0;
   trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus = 0;
   trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirl = 0;
   trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirlStatus = 0;
   trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EF = 0;
   trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus = 0;
   trig_RoI_EF_TileMu_type = 0;
   trig_RoI_EF_TileMu_lastStep = 0;
   trig_RoI_EF_TileMu_Muon_ROI = 0;
   trig_RoI_EF_TileMu_Muon_ROIStatus = 0;
   trig_RoI_EF_TileMu_TrigMuonEFContainer = 0;
   trig_RoI_EF_TileMu_TrigMuonEFContainerStatus = 0;
   trig_RoI_EF_TileMu_TrigMuonEFInfoContainer = 0;
   trig_RoI_EF_TileMu_TrigMuonEFInfoContainerStatus = 0;
   trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirl = 0;
   trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirlStatus = 0;
   trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EF = 0;
   trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus = 0;
   trig_L1_TAV = 0;
   trig_L2_passedPhysics = 0;
   trig_EF_passedPhysics = 0;
   muon_muid_E = 0;
   muon_muid_pt = 0;
   muon_muid_m = 0;
   muon_muid_eta = 0;
   muon_muid_phi = 0;
   muon_muid_px = 0;
   muon_muid_py = 0;
   muon_muid_pz = 0;
   muon_muid_charge = 0;
   muon_muid_allauthor = 0;
   muon_muid_hastrack = 0;
   muon_staco_E = 0;
   muon_staco_pt = 0;
   muon_staco_m = 0;
   muon_staco_eta = 0;
   muon_staco_phi = 0;
   muon_staco_px = 0;
   muon_staco_py = 0;
   muon_staco_pz = 0;
   muon_staco_charge = 0;
   muon_staco_allauthor = 0;
   muon_staco_hastrack = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("trig_L1_mu_n", &trig_L1_mu_n, &b_trig_L1_mu_n);
   fChain->SetBranchAddress("trig_L1_mu_pt", &trig_L1_mu_pt, &b_trig_L1_mu_pt);
   fChain->SetBranchAddress("trig_L1_mu_eta", &trig_L1_mu_eta, &b_trig_L1_mu_eta);
   fChain->SetBranchAddress("trig_L1_mu_phi", &trig_L1_mu_phi, &b_trig_L1_mu_phi);
   fChain->SetBranchAddress("trig_L1_mu_thrName", &trig_L1_mu_thrName, &b_trig_L1_mu_thrName);
   fChain->SetBranchAddress("trig_L2_muonfeature_n", &trig_L2_muonfeature_n, &b_trig_L2_muonfeature_n);
   fChain->SetBranchAddress("trig_L2_muonfeature_pt", &trig_L2_muonfeature_pt, &b_trig_L2_muonfeature_pt);
   fChain->SetBranchAddress("trig_L2_muonfeature_eta", &trig_L2_muonfeature_eta, &b_trig_L2_muonfeature_eta);
   fChain->SetBranchAddress("trig_L2_muonfeature_phi", &trig_L2_muonfeature_phi, &b_trig_L2_muonfeature_phi);
   fChain->SetBranchAddress("trig_L2_muonfeaturedetails_n", &trig_L2_muonfeaturedetails_n, &b_trig_L2_muonfeaturedetails_n);
   fChain->SetBranchAddress("trig_L2_combmuonfeature_n", &trig_L2_combmuonfeature_n, &b_trig_L2_combmuonfeature_n);
   fChain->SetBranchAddress("trig_L2_combmuonfeature_pt", &trig_L2_combmuonfeature_pt, &b_trig_L2_combmuonfeature_pt);
   fChain->SetBranchAddress("trig_L2_combmuonfeature_eta", &trig_L2_combmuonfeature_eta, &b_trig_L2_combmuonfeature_eta);
   fChain->SetBranchAddress("trig_L2_combmuonfeature_phi", &trig_L2_combmuonfeature_phi, &b_trig_L2_combmuonfeature_phi);
   fChain->SetBranchAddress("trig_L2_combmuonfeature_idscantrk_index", &trig_L2_combmuonfeature_idscantrk_index, &b_trig_L2_combmuonfeature_idscantrk_index);
   fChain->SetBranchAddress("trig_L2_combmuonfeature_sitracktrk_index", &trig_L2_combmuonfeature_sitracktrk_index, &b_trig_L2_combmuonfeature_sitracktrk_index);
   fChain->SetBranchAddress("trig_L2_isomuonfeature_n", &trig_L2_isomuonfeature_n, &b_trig_L2_isomuonfeature_n);
   fChain->SetBranchAddress("trig_L2_isomuonfeature_pt", &trig_L2_isomuonfeature_pt, &b_trig_L2_isomuonfeature_pt);
   fChain->SetBranchAddress("trig_L2_isomuonfeature_eta", &trig_L2_isomuonfeature_eta, &b_trig_L2_isomuonfeature_eta);
   fChain->SetBranchAddress("trig_L2_isomuonfeature_phi", &trig_L2_isomuonfeature_phi, &b_trig_L2_isomuonfeature_phi);
   fChain->SetBranchAddress("trig_L2_isomuonfeature_EtInnerConeEC ", &trig_L2_isomuonfeature_EtInnerConeEC , &b_trig_L2_isomuonfeature_EtInnerConeEC );
   fChain->SetBranchAddress("trig_L2_isomuonfeature_EtOuterConeEC", &trig_L2_isomuonfeature_EtOuterConeEC, &b_trig_L2_isomuonfeature_EtOuterConeEC);
   fChain->SetBranchAddress("trig_L2_isomuonfeature_EtInnerConeHC", &trig_L2_isomuonfeature_EtInnerConeHC, &b_trig_L2_isomuonfeature_EtInnerConeHC);
   fChain->SetBranchAddress("trig_L2_isomuonfeature_EtOuterConeHC", &trig_L2_isomuonfeature_EtOuterConeHC, &b_trig_L2_isomuonfeature_EtOuterConeHC);
   fChain->SetBranchAddress("trig_L2_isomuonfeature_NTracksCone", &trig_L2_isomuonfeature_NTracksCone, &b_trig_L2_isomuonfeature_NTracksCone);
   fChain->SetBranchAddress("trig_L2_isomuonfeature_SumPtTracksCone", &trig_L2_isomuonfeature_SumPtTracksCone, &b_trig_L2_isomuonfeature_SumPtTracksCone);
   fChain->SetBranchAddress("trig_L2_isomuonfeature_PtMuTracksCone", &trig_L2_isomuonfeature_PtMuTracksCone, &b_trig_L2_isomuonfeature_PtMuTracksCone);
   fChain->SetBranchAddress("trig_L2_tilemufeature_n", &trig_L2_tilemufeature_n, &b_trig_L2_tilemufeature_n);
   fChain->SetBranchAddress("trig_L2_tilemufeature_eta", &trig_L2_tilemufeature_eta, &b_trig_L2_tilemufeature_eta);
   fChain->SetBranchAddress("trig_L2_tilemufeature_phi", &trig_L2_tilemufeature_phi, &b_trig_L2_tilemufeature_phi);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_n", &trig_L2_tiletrackmufeature_n, &b_trig_L2_tiletrackmufeature_n);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_pt", &trig_L2_tiletrackmufeature_pt, &b_trig_L2_tiletrackmufeature_pt);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_eta", &trig_L2_tiletrackmufeature_eta, &b_trig_L2_tiletrackmufeature_eta);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_phi", &trig_L2_tiletrackmufeature_phi, &b_trig_L2_tiletrackmufeature_phi);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_tilemu_index", &trig_L2_tiletrackmufeature_tilemu_index, &b_trig_L2_tiletrackmufeature_tilemu_index);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_idtrk_algorithmId", &trig_L2_tiletrackmufeature_idtrk_algorithmId, &b_trig_L2_tiletrackmufeature_idtrk_algorithmId);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_idtrk_trackStatus", &trig_L2_tiletrackmufeature_idtrk_trackStatus, &b_trig_L2_tiletrackmufeature_idtrk_trackStatus);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_idtrk_chi2Ndof", &trig_L2_tiletrackmufeature_idtrk_chi2Ndof, &b_trig_L2_tiletrackmufeature_idtrk_chi2Ndof);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_a0", &trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_a0, &b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_a0);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_z0", &trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_z0, &b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_z0);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_phi0", &trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_phi0, &b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_phi0);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_eta", &trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_eta, &b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_eta);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_pt", &trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_pt, &b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_pt);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_hasidtrkfitpar", &trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_hasidtrkfitpar, &b_trig_L2_tiletrackmufeature_idtrk_idtrkfitpar_hasidtrkfitpar);
   fChain->SetBranchAddress("trig_L2_tiletrackmufeature_idtrk_hasidtrk", &trig_L2_tiletrackmufeature_idtrk_hasidtrk, &b_trig_L2_tiletrackmufeature_idtrk_hasidtrk);
   fChain->SetBranchAddress("trig_EF_trigmuonef_n", &trig_EF_trigmuonef_n, &b_trig_EF_trigmuonef_n);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_n", &trig_EF_trigmuonef_track_n, &b_trig_EF_trigmuonef_track_n);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_MuonType", &trig_EF_trigmuonef_track_MuonType, &b_trig_EF_trigmuonef_track_MuonType);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_MS_pt", &trig_EF_trigmuonef_track_MS_pt, &b_trig_EF_trigmuonef_track_MS_pt);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_MS_eta", &trig_EF_trigmuonef_track_MS_eta, &b_trig_EF_trigmuonef_track_MS_eta);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_MS_phi", &trig_EF_trigmuonef_track_MS_phi, &b_trig_EF_trigmuonef_track_MS_phi);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_MS_hasMS", &trig_EF_trigmuonef_track_MS_hasMS, &b_trig_EF_trigmuonef_track_MS_hasMS);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_SA_pt", &trig_EF_trigmuonef_track_SA_pt, &b_trig_EF_trigmuonef_track_SA_pt);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_SA_eta", &trig_EF_trigmuonef_track_SA_eta, &b_trig_EF_trigmuonef_track_SA_eta);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_SA_phi", &trig_EF_trigmuonef_track_SA_phi, &b_trig_EF_trigmuonef_track_SA_phi);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_SA_hasSA", &trig_EF_trigmuonef_track_SA_hasSA, &b_trig_EF_trigmuonef_track_SA_hasSA);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_CB_pt", &trig_EF_trigmuonef_track_CB_pt, &b_trig_EF_trigmuonef_track_CB_pt);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_CB_eta", &trig_EF_trigmuonef_track_CB_eta, &b_trig_EF_trigmuonef_track_CB_eta);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_CB_phi", &trig_EF_trigmuonef_track_CB_phi, &b_trig_EF_trigmuonef_track_CB_phi);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_CB_hasCB", &trig_EF_trigmuonef_track_CB_hasCB, &b_trig_EF_trigmuonef_track_CB_hasCB);
   fChain->SetBranchAddress("trig_EF_trigmugirl_n", &trig_EF_trigmugirl_n, &b_trig_EF_trigmugirl_n);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_n", &trig_EF_trigmugirl_track_n, &b_trig_EF_trigmugirl_track_n);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_MuonType", &trig_EF_trigmugirl_track_MuonType, &b_trig_EF_trigmugirl_track_MuonType);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_MS_pt", &trig_EF_trigmugirl_track_MS_pt, &b_trig_EF_trigmugirl_track_MS_pt);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_MS_eta", &trig_EF_trigmugirl_track_MS_eta, &b_trig_EF_trigmugirl_track_MS_eta);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_MS_phi", &trig_EF_trigmugirl_track_MS_phi, &b_trig_EF_trigmugirl_track_MS_phi);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_MS_hasMS", &trig_EF_trigmugirl_track_MS_hasMS, &b_trig_EF_trigmugirl_track_MS_hasMS);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_SA_pt", &trig_EF_trigmugirl_track_SA_pt, &b_trig_EF_trigmugirl_track_SA_pt);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_SA_eta", &trig_EF_trigmugirl_track_SA_eta, &b_trig_EF_trigmugirl_track_SA_eta);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_SA_phi", &trig_EF_trigmugirl_track_SA_phi, &b_trig_EF_trigmugirl_track_SA_phi);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_SA_hasSA", &trig_EF_trigmugirl_track_SA_hasSA, &b_trig_EF_trigmugirl_track_SA_hasSA);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_CB_pt", &trig_EF_trigmugirl_track_CB_pt, &b_trig_EF_trigmugirl_track_CB_pt);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_CB_eta", &trig_EF_trigmugirl_track_CB_eta, &b_trig_EF_trigmugirl_track_CB_eta);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_CB_phi", &trig_EF_trigmugirl_track_CB_phi, &b_trig_EF_trigmugirl_track_CB_phi);
   fChain->SetBranchAddress("trig_EF_trigmugirl_track_CB_hasCB", &trig_EF_trigmugirl_track_CB_hasCB, &b_trig_EF_trigmugirl_track_CB_hasCB);
   fChain->SetBranchAddress("trig_L2_sitrack_muon_n", &trig_L2_sitrack_muon_n, &b_trig_L2_sitrack_muon_n);
   fChain->SetBranchAddress("trig_L2_sitrack_muon_algorithmId", &trig_L2_sitrack_muon_algorithmId, &b_trig_L2_sitrack_muon_algorithmId);
   fChain->SetBranchAddress("trig_L2_sitrack_muon_trackStatus", &trig_L2_sitrack_muon_trackStatus, &b_trig_L2_sitrack_muon_trackStatus);
   fChain->SetBranchAddress("trig_L2_sitrack_muon_chi2Ndof", &trig_L2_sitrack_muon_chi2Ndof, &b_trig_L2_sitrack_muon_chi2Ndof);
   fChain->SetBranchAddress("trig_L2_sitrack_muon_a0", &trig_L2_sitrack_muon_a0, &b_trig_L2_sitrack_muon_a0);
   fChain->SetBranchAddress("trig_L2_sitrack_muon_z0", &trig_L2_sitrack_muon_z0, &b_trig_L2_sitrack_muon_z0);
   fChain->SetBranchAddress("trig_L2_sitrack_muon_phi0", &trig_L2_sitrack_muon_phi0, &b_trig_L2_sitrack_muon_phi0);
   fChain->SetBranchAddress("trig_L2_sitrack_muon_eta", &trig_L2_sitrack_muon_eta, &b_trig_L2_sitrack_muon_eta);
   fChain->SetBranchAddress("trig_L2_sitrack_muon_pt", &trig_L2_sitrack_muon_pt, &b_trig_L2_sitrack_muon_pt);
   fChain->SetBranchAddress("trig_L2_idscan_muon_n", &trig_L2_idscan_muon_n, &b_trig_L2_idscan_muon_n);
   fChain->SetBranchAddress("trig_L2_idscan_muon_algorithmId", &trig_L2_idscan_muon_algorithmId, &b_trig_L2_idscan_muon_algorithmId);
   fChain->SetBranchAddress("trig_L2_idscan_muon_trackStatus", &trig_L2_idscan_muon_trackStatus, &b_trig_L2_idscan_muon_trackStatus);
   fChain->SetBranchAddress("trig_L2_idscan_muon_chi2Ndof", &trig_L2_idscan_muon_chi2Ndof, &b_trig_L2_idscan_muon_chi2Ndof);
   fChain->SetBranchAddress("trig_L2_idscan_muon_a0", &trig_L2_idscan_muon_a0, &b_trig_L2_idscan_muon_a0);
   fChain->SetBranchAddress("trig_L2_idscan_muon_z0", &trig_L2_idscan_muon_z0, &b_trig_L2_idscan_muon_z0);
   fChain->SetBranchAddress("trig_L2_idscan_muon_phi0", &trig_L2_idscan_muon_phi0, &b_trig_L2_idscan_muon_phi0);
   fChain->SetBranchAddress("trig_L2_idscan_muon_eta", &trig_L2_idscan_muon_eta, &b_trig_L2_idscan_muon_eta);
   fChain->SetBranchAddress("trig_L2_idscan_muon_pt", &trig_L2_idscan_muon_pt, &b_trig_L2_idscan_muon_pt);
   fChain->SetBranchAddress("trig_L2_sitrack_isomuon_n", &trig_L2_sitrack_isomuon_n, &b_trig_L2_sitrack_isomuon_n);
   fChain->SetBranchAddress("trig_L2_sitrack_isomuon_algorithmId", &trig_L2_sitrack_isomuon_algorithmId, &b_trig_L2_sitrack_isomuon_algorithmId);
   fChain->SetBranchAddress("trig_L2_sitrack_isomuon_trackStatus", &trig_L2_sitrack_isomuon_trackStatus, &b_trig_L2_sitrack_isomuon_trackStatus);
   fChain->SetBranchAddress("trig_L2_sitrack_isomuon_chi2Ndof", &trig_L2_sitrack_isomuon_chi2Ndof, &b_trig_L2_sitrack_isomuon_chi2Ndof);
   fChain->SetBranchAddress("trig_L2_sitrack_isomuon_a0", &trig_L2_sitrack_isomuon_a0, &b_trig_L2_sitrack_isomuon_a0);
   fChain->SetBranchAddress("trig_L2_sitrack_isomuon_z0", &trig_L2_sitrack_isomuon_z0, &b_trig_L2_sitrack_isomuon_z0);
   fChain->SetBranchAddress("trig_L2_sitrack_isomuon_phi0", &trig_L2_sitrack_isomuon_phi0, &b_trig_L2_sitrack_isomuon_phi0);
   fChain->SetBranchAddress("trig_L2_sitrack_isomuon_eta", &trig_L2_sitrack_isomuon_eta, &b_trig_L2_sitrack_isomuon_eta);
   fChain->SetBranchAddress("trig_L2_sitrack_isomuon_pt", &trig_L2_sitrack_isomuon_pt, &b_trig_L2_sitrack_isomuon_pt);
   fChain->SetBranchAddress("trig_L2_idscan_isomuon_n", &trig_L2_idscan_isomuon_n, &b_trig_L2_idscan_isomuon_n);
   fChain->SetBranchAddress("trig_L2_idscan_isomuon_algorithmId", &trig_L2_idscan_isomuon_algorithmId, &b_trig_L2_idscan_isomuon_algorithmId);
   fChain->SetBranchAddress("trig_L2_idscan_isomuon_trackStatus", &trig_L2_idscan_isomuon_trackStatus, &b_trig_L2_idscan_isomuon_trackStatus);
   fChain->SetBranchAddress("trig_L2_idscan_isomuon_chi2Ndof", &trig_L2_idscan_isomuon_chi2Ndof, &b_trig_L2_idscan_isomuon_chi2Ndof);
   fChain->SetBranchAddress("trig_L2_idscan_isomuon_a0", &trig_L2_idscan_isomuon_a0, &b_trig_L2_idscan_isomuon_a0);
   fChain->SetBranchAddress("trig_L2_idscan_isomuon_z0", &trig_L2_idscan_isomuon_z0, &b_trig_L2_idscan_isomuon_z0);
   fChain->SetBranchAddress("trig_L2_idscan_isomuon_phi0", &trig_L2_idscan_isomuon_phi0, &b_trig_L2_idscan_isomuon_phi0);
   fChain->SetBranchAddress("trig_L2_idscan_isomuon_eta", &trig_L2_idscan_isomuon_eta, &b_trig_L2_idscan_isomuon_eta);
   fChain->SetBranchAddress("trig_L2_idscan_isomuon_pt", &trig_L2_idscan_isomuon_pt, &b_trig_L2_idscan_isomuon_pt);
   fChain->SetBranchAddress("trig_Nav_n", &trig_Nav_n, &b_trig_Nav_n);
   fChain->SetBranchAddress("trig_Nav_chain_ChainId", &trig_Nav_chain_ChainId, &b_trig_Nav_chain_ChainId);
   fChain->SetBranchAddress("trig_Nav_chain_RoIType", &trig_Nav_chain_RoIType, &b_trig_Nav_chain_RoIType);
   fChain->SetBranchAddress("trig_Nav_chain_RoIIndex", &trig_Nav_chain_RoIIndex, &b_trig_Nav_chain_RoIIndex);
   fChain->SetBranchAddress("trig_RoI_L2_mu_n", &trig_RoI_L2_mu_n, &b_trig_RoI_L2_mu_n);
   fChain->SetBranchAddress("trig_RoI_L2_mu_type", &trig_RoI_L2_mu_type, &b_trig_RoI_L2_mu_type);
   fChain->SetBranchAddress("trig_RoI_L2_mu_lastStep", &trig_RoI_L2_mu_lastStep, &b_trig_RoI_L2_mu_lastStep);
   fChain->SetBranchAddress("trig_RoI_L2_mu_MuonFeature", &trig_RoI_L2_mu_MuonFeature, &b_trig_RoI_L2_mu_MuonFeature);
   fChain->SetBranchAddress("trig_RoI_L2_mu_MuonFeatureStatus", &trig_RoI_L2_mu_MuonFeatureStatus, &b_trig_RoI_L2_mu_MuonFeatureStatus);
   fChain->SetBranchAddress("trig_RoI_L2_mu_MuonFeatureDetails", &trig_RoI_L2_mu_MuonFeatureDetails, &b_trig_RoI_L2_mu_MuonFeatureDetails);
   fChain->SetBranchAddress("trig_RoI_L2_mu_MuonFeatureDetailsStatus", &trig_RoI_L2_mu_MuonFeatureDetailsStatus, &b_trig_RoI_L2_mu_MuonFeatureDetailsStatus);
   fChain->SetBranchAddress("trig_RoI_L2_mu_CombinedMuonFeature", &trig_RoI_L2_mu_CombinedMuonFeature, &b_trig_RoI_L2_mu_CombinedMuonFeature);
   fChain->SetBranchAddress("trig_RoI_L2_mu_CombinedMuonFeatureStatus", &trig_RoI_L2_mu_CombinedMuonFeatureStatus, &b_trig_RoI_L2_mu_CombinedMuonFeatureStatus);
   fChain->SetBranchAddress("trig_RoI_L2_mu_IsoMuonFeature", &trig_RoI_L2_mu_IsoMuonFeature, &b_trig_RoI_L2_mu_IsoMuonFeature);
   fChain->SetBranchAddress("trig_RoI_L2_mu_IsoMuonFeatureStatus", &trig_RoI_L2_mu_IsoMuonFeatureStatus, &b_trig_RoI_L2_mu_IsoMuonFeatureStatus);
   fChain->SetBranchAddress("trig_RoI_L2_mu_Muon_ROI", &trig_RoI_L2_mu_Muon_ROI, &b_trig_RoI_L2_mu_Muon_ROI);
   fChain->SetBranchAddress("trig_RoI_L2_mu_Muon_ROIStatus", &trig_RoI_L2_mu_Muon_ROIStatus, &b_trig_RoI_L2_mu_Muon_ROIStatus);
   fChain->SetBranchAddress("trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon", &trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon, &b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_Muon);
   fChain->SetBranchAddress("trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus", &trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus, &b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_MuonStatus);
   fChain->SetBranchAddress("trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon", &trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon, &b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_Muon);
   fChain->SetBranchAddress("trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus", &trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus, &b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_MuonStatus);
   fChain->SetBranchAddress("trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIso", &trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIso, &b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIso);
   fChain->SetBranchAddress("trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIsoStatus", &trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIsoStatus, &b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigSiTrack_muonIsoStatus);
   fChain->SetBranchAddress("trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIso", &trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIso, &b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIso);
   fChain->SetBranchAddress("trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIsoStatus", &trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIsoStatus, &b_trig_RoI_L2_mu_TrigInDetTrackCollection_TrigIDSCAN_muonIsoStatus);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_n", &trig_RoI_L2_TileMu_n, &b_trig_RoI_L2_TileMu_n);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_type", &trig_RoI_L2_TileMu_type, &b_trig_RoI_L2_TileMu_type);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_lastStep", &trig_RoI_L2_TileMu_lastStep, &b_trig_RoI_L2_TileMu_lastStep);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_TileMuFeature", &trig_RoI_L2_TileMu_TileMuFeature, &b_trig_RoI_L2_TileMu_TileMuFeature);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_TileMuFeatureStatus", &trig_RoI_L2_TileMu_TileMuFeatureStatus, &b_trig_RoI_L2_TileMu_TileMuFeatureStatus);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_TileTrackMuFeature", &trig_RoI_L2_TileMu_TileTrackMuFeature, &b_trig_RoI_L2_TileMu_TileTrackMuFeature);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_TileTrackMuFeatureStatus", &trig_RoI_L2_TileMu_TileTrackMuFeatureStatus, &b_trig_RoI_L2_TileMu_TileTrackMuFeatureStatus);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_Muon_ROI", &trig_RoI_L2_TileMu_Muon_ROI, &b_trig_RoI_L2_TileMu_Muon_ROI);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_Muon_ROIStatus", &trig_RoI_L2_TileMu_Muon_ROIStatus, &b_trig_RoI_L2_TileMu_Muon_ROIStatus);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Tile", &trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Tile, &b_trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_Tile);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_TileStatus", &trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_TileStatus, &b_trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigSiTrack_TileStatus);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Tile", &trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Tile, &b_trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_Tile);
   fChain->SetBranchAddress("trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_TileStatus", &trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_TileStatus, &b_trig_RoI_L2_TileMu_TrigInDetTrackCollection_TrigIDSCAN_TileStatus);
   fChain->SetBranchAddress("trig_RoI_EF_mu_n", &trig_RoI_EF_mu_n, &b_trig_RoI_EF_mu_n);
   fChain->SetBranchAddress("trig_RoI_EF_mu_type", &trig_RoI_EF_mu_type, &b_trig_RoI_EF_mu_type);
   fChain->SetBranchAddress("trig_RoI_EF_mu_lastStep", &trig_RoI_EF_mu_lastStep, &b_trig_RoI_EF_mu_lastStep);
   fChain->SetBranchAddress("trig_RoI_EF_mu_Muon_ROI", &trig_RoI_EF_mu_Muon_ROI, &b_trig_RoI_EF_mu_Muon_ROI);
   fChain->SetBranchAddress("trig_RoI_EF_mu_Muon_ROIStatus", &trig_RoI_EF_mu_Muon_ROIStatus, &b_trig_RoI_EF_mu_Muon_ROIStatus);
   fChain->SetBranchAddress("trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFID", &trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFID, &b_trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFID);
   fChain->SetBranchAddress("trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFIDStatus", &trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFIDStatus, &b_trig_RoI_EF_mu_TrackCollection_InDetTrigTrackSlimmer_Muon_EFIDStatus);
   fChain->SetBranchAddress("trig_RoI_EF_mu_TrigMuonEFInfoContainer", &trig_RoI_EF_mu_TrigMuonEFInfoContainer, &b_trig_RoI_EF_mu_TrigMuonEFInfoContainer);
   fChain->SetBranchAddress("trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus", &trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus, &b_trig_RoI_EF_mu_TrigMuonEFInfoContainerStatus);
   fChain->SetBranchAddress("trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirl", &trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirl, &b_trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirl);
   fChain->SetBranchAddress("trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirlStatus", &trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirlStatus, &b_trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuGirlStatus);
   fChain->SetBranchAddress("trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EF", &trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EF, &b_trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EF);
   fChain->SetBranchAddress("trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus", &trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus, &b_trig_RoI_EF_mu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_n", &trig_RoI_EF_TileMu_n, &b_trig_RoI_EF_TileMu_n);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_type", &trig_RoI_EF_TileMu_type, &b_trig_RoI_EF_TileMu_type);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_lastStep", &trig_RoI_EF_TileMu_lastStep, &b_trig_RoI_EF_TileMu_lastStep);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_Muon_ROI", &trig_RoI_EF_TileMu_Muon_ROI, &b_trig_RoI_EF_TileMu_Muon_ROI);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_Muon_ROIStatus", &trig_RoI_EF_TileMu_Muon_ROIStatus, &b_trig_RoI_EF_TileMu_Muon_ROIStatus);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_TrigMuonEFContainer", &trig_RoI_EF_TileMu_TrigMuonEFContainer, &b_trig_RoI_EF_TileMu_TrigMuonEFContainer);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_TrigMuonEFContainerStatus", &trig_RoI_EF_TileMu_TrigMuonEFContainerStatus, &b_trig_RoI_EF_TileMu_TrigMuonEFContainerStatus);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_TrigMuonEFInfoContainer", &trig_RoI_EF_TileMu_TrigMuonEFInfoContainer, &b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainer);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_TrigMuonEFInfoContainerStatus", &trig_RoI_EF_TileMu_TrigMuonEFInfoContainerStatus, &b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainerStatus);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirl", &trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirl, &b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirl);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirlStatus", &trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirlStatus, &b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuGirlStatus);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EF", &trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EF, &b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EF);
   fChain->SetBranchAddress("trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus", &trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus, &b_trig_RoI_EF_TileMu_TrigMuonEFInfoContainer_MuTagIMO_EFStatus);
   fChain->SetBranchAddress("trig_DB_SMK", &trig_DB_SMK, &b_trig_DB_SMK);
   fChain->SetBranchAddress("trig_DB_L1PSK", &trig_DB_L1PSK, &b_trig_DB_L1PSK);
   fChain->SetBranchAddress("trig_DB_HLTPSK", &trig_DB_HLTPSK, &b_trig_DB_HLTPSK);
   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
   fChain->SetBranchAddress("timestamp", &timestamp, &b_timestamp);
   fChain->SetBranchAddress("timestamp_ns", &timestamp_ns, &b_timestamp_ns);
   fChain->SetBranchAddress("lbn", &lbn, &b_lbn);
   fChain->SetBranchAddress("bcid", &bcid, &b_bcid);
   fChain->SetBranchAddress("detmask0", &detmask0, &b_detmask0);
   fChain->SetBranchAddress("detmask1", &detmask1, &b_detmask1);
   fChain->SetBranchAddress("pixelFlags", &pixelFlags, &b_pixelFlags);
   fChain->SetBranchAddress("sctFlags", &sctFlags, &b_sctFlags);
   fChain->SetBranchAddress("trtFlags", &trtFlags, &b_trtFlags);
   fChain->SetBranchAddress("larFlags", &larFlags, &b_larFlags);
   fChain->SetBranchAddress("tileFlags", &tileFlags, &b_tileFlags);
   fChain->SetBranchAddress("muonFlags", &muonFlags, &b_muonFlags);
   fChain->SetBranchAddress("fwdFlags", &fwdFlags, &b_fwdFlags);
   fChain->SetBranchAddress("coreFlags", &coreFlags, &b_coreFlags);
   fChain->SetBranchAddress("pixelError", &pixelError, &b_pixelError);
   fChain->SetBranchAddress("sctError", &sctError, &b_sctError);
   fChain->SetBranchAddress("trtError", &trtError, &b_trtError);
   fChain->SetBranchAddress("larError", &larError, &b_larError);
   fChain->SetBranchAddress("tileError", &tileError, &b_tileError);
   fChain->SetBranchAddress("muonError", &muonError, &b_muonError);
   fChain->SetBranchAddress("fwdError", &fwdError, &b_fwdError);
   fChain->SetBranchAddress("coreError", &coreError, &b_coreError);
   fChain->SetBranchAddress("trig_L1_TAV", &trig_L1_TAV, &b_trig_L1_TAV);
   fChain->SetBranchAddress("trig_L2_passedPhysics", &trig_L2_passedPhysics, &b_trig_L2_passedPhysics);
   fChain->SetBranchAddress("trig_EF_passedPhysics", &trig_EF_passedPhysics, &b_trig_EF_passedPhysics);
   fChain->SetBranchAddress("muon_muid_n", &muon_muid_n, &b_muon_muid_n);
   fChain->SetBranchAddress("muon_muid_E", &muon_muid_E, &b_muon_muid_E);
   fChain->SetBranchAddress("muon_muid_pt", &muon_muid_pt, &b_muon_muid_pt);
   fChain->SetBranchAddress("muon_muid_m", &muon_muid_m, &b_muon_muid_m);
   fChain->SetBranchAddress("muon_muid_eta", &muon_muid_eta, &b_muon_muid_eta);
   fChain->SetBranchAddress("muon_muid_phi", &muon_muid_phi, &b_muon_muid_phi);
   fChain->SetBranchAddress("muon_muid_px", &muon_muid_px, &b_muon_muid_px);
   fChain->SetBranchAddress("muon_muid_py", &muon_muid_py, &b_muon_muid_py);
   fChain->SetBranchAddress("muon_muid_pz", &muon_muid_pz, &b_muon_muid_pz);
   fChain->SetBranchAddress("muon_muid_charge", &muon_muid_charge, &b_muon_muid_charge);
   fChain->SetBranchAddress("muon_muid_allauthor", &muon_muid_allauthor, &b_muon_muid_allauthor);
   fChain->SetBranchAddress("muon_muid_hastrack", &muon_muid_hastrack, &b_muon_muid_hastrack);
   fChain->SetBranchAddress("EF_2mu10", &EF_2mu10, &b_EF_2mu10);
   fChain->SetBranchAddress("EF_2mu4", &EF_2mu4, &b_EF_2mu4);
   fChain->SetBranchAddress("EF_mu10", &EF_mu10, &b_EF_mu10);
   fChain->SetBranchAddress("EF_mu20", &EF_mu20, &b_EF_mu20);
   fChain->SetBranchAddress("EF_mu20_MSonly", &EF_mu20_MSonly, &b_EF_mu20_MSonly);
   fChain->SetBranchAddress("EF_mu4", &EF_mu4, &b_EF_mu4);
   fChain->SetBranchAddress("EF_mu6", &EF_mu6, &b_EF_mu6);
   fChain->SetBranchAddress("L1_MU0", &L1_MU0, &b_L1_MU0);
   fChain->SetBranchAddress("L1_MU10", &L1_MU10, &b_L1_MU10);
   fChain->SetBranchAddress("L1_MU20", &L1_MU20, &b_L1_MU20);
   fChain->SetBranchAddress("L1_MU6", &L1_MU6, &b_L1_MU6);
   fChain->SetBranchAddress("L2_2mu4", &L2_2mu4, &b_L2_2mu4);
   fChain->SetBranchAddress("L2_mu10", &L2_mu10, &b_L2_mu10);
   fChain->SetBranchAddress("L2_mu20", &L2_mu20, &b_L2_mu20);
   fChain->SetBranchAddress("L2_mu20_MSonly", &L2_mu20_MSonly, &b_L2_mu20_MSonly);
   fChain->SetBranchAddress("L2_mu4", &L2_mu4, &b_L2_mu4);
   fChain->SetBranchAddress("L2_mu6", &L2_mu6, &b_L2_mu6);
   fChain->SetBranchAddress("muon_staco_n", &muon_staco_n, &b_muon_staco_n);
   fChain->SetBranchAddress("muon_staco_E", &muon_staco_E, &b_muon_staco_E);
   fChain->SetBranchAddress("muon_staco_pt", &muon_staco_pt, &b_muon_staco_pt);
   fChain->SetBranchAddress("muon_staco_m", &muon_staco_m, &b_muon_staco_m);
   fChain->SetBranchAddress("muon_staco_eta", &muon_staco_eta, &b_muon_staco_eta);
   fChain->SetBranchAddress("muon_staco_phi", &muon_staco_phi, &b_muon_staco_phi);
   fChain->SetBranchAddress("muon_staco_px", &muon_staco_px, &b_muon_staco_px);
   fChain->SetBranchAddress("muon_staco_py", &muon_staco_py, &b_muon_staco_py);
   fChain->SetBranchAddress("muon_staco_pz", &muon_staco_pz, &b_muon_staco_pz);
   fChain->SetBranchAddress("muon_staco_charge", &muon_staco_charge, &b_muon_staco_charge);
   fChain->SetBranchAddress("muon_staco_allauthor", &muon_staco_allauthor, &b_muon_staco_allauthor);
   fChain->SetBranchAddress("muon_staco_hastrack", &muon_staco_hastrack, &b_muon_staco_hastrack);
   Notify();
}

Bool_t TrigMuonD3PD::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TrigMuonD3PD::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TrigMuonD3PD::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TrigMuonD3PD_cxx
