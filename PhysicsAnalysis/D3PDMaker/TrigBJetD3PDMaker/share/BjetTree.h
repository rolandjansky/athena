//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Dec  8 13:12:15 2010 by ROOT version 5.26/00e
// from TTree BjetTree/BjetTree
// found on file: bjet.root
//////////////////////////////////////////////////////////

#ifndef BjetTree_h
#define BjetTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>

using namespace std;

class BjetTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          ei_RunNumber;
   UInt_t          ei_EventNumber;
   UInt_t          ei_timestamp;
   UInt_t          ei_timestamp_ns;
   UInt_t          ei_lbn;
   UInt_t          ei_bcid;
   UInt_t          ei_detmask0;
   UInt_t          ei_detmask1;
   UInt_t          ei_pixelFlags;
   UInt_t          ei_sctFlags;
   UInt_t          ei_trtFlags;
   UInt_t          ei_larFlags;
   UInt_t          ei_tileFlags;
   UInt_t          ei_muonFlags;
   UInt_t          ei_fwdFlags;
   UInt_t          ei_coreFlags;
   UInt_t          ei_pixelError;
   UInt_t          ei_sctError;
   UInt_t          ei_trtError;
   UInt_t          ei_larError;
   UInt_t          ei_tileError;
   UInt_t          ei_muonError;
   UInt_t          ei_fwdError;
   UInt_t          ei_coreError;
   Int_t           AntiKt4H1Topo_n;
   vector<float>   *AntiKt4H1Topo_E;
   vector<float>   *AntiKt4H1Topo_pt;
   vector<float>   *AntiKt4H1Topo_m;
   vector<float>   *AntiKt4H1Topo_eta;
   vector<float>   *AntiKt4H1Topo_phi;
   vector<float>   *AntiKt4H1Topo_EtaPhys;
   vector<float>   *AntiKt4H1Topo_PhiPhys;
   vector<float>   *AntiKt4H1Topo_MPhys;
   vector<float>   *AntiKt4H1Topo_WIDTH;
   vector<float>   *AntiKt4H1Topo_n90;
   vector<float>   *AntiKt4H1Topo_Timing;
   vector<float>   *AntiKt4H1Topo_LArQuality;
   vector<float>   *AntiKt4H1Topo_nTrk;
   vector<float>   *AntiKt4H1Topo_sumPtTrk;
   vector<float>   *AntiKt4H1Topo_OriginIndex;
   vector<float>   *AntiKt4H1Topo_HECQuality;
   vector<float>   *AntiKt4H1Topo_NegativeE;
   vector<float>   *AntiKt4H1Topo_BCH_CORR_CELL;
   vector<float>   *AntiKt4H1Topo_BCH_CORR_DOTX;
   vector<float>   *AntiKt4H1Topo_BCH_CORR_JET;
   vector<float>   *AntiKt4H1Topo_BCH_CORR_JET_FORCELL;
   vector<float>   *AntiKt4H1Topo_ENG_BAD_CELLS;
   vector<float>   *AntiKt4H1Topo_N_BAD_CELLS;
   vector<float>   *AntiKt4H1Topo_N_BAD_CELLS_CORR;
   vector<float>   *AntiKt4H1Topo_BAD_CELLS_CORR_E;
   vector<int>     *AntiKt4H1Topo_SamplingMax;
   vector<double>  *AntiKt4H1Topo_fracSamplingMax;
   vector<float>   *AntiKt4H1Topo_hecf;
   vector<float>   *AntiKt4H1Topo_tgap3f;
   vector<int>     *AntiKt4H1Topo_isGood;
   vector<float>   *AntiKt4H1Topo_emfrac;
   vector<float>   *AntiKt4H1Topo_GCWJES;
   vector<float>   *AntiKt4H1Topo_EMJES;
   vector<float>   *AntiKt4H1Topo_CB;
   vector<float>   *AntiKt4H1Topo_emscale_E;
   vector<float>   *AntiKt4H1Topo_emscale_pt;
   vector<float>   *AntiKt4H1Topo_emscale_m;
   vector<float>   *AntiKt4H1Topo_emscale_eta;
   vector<float>   *AntiKt4H1Topo_emscale_phi;
   vector<float>   *AntiKt4H1Topo_jvtxf;
   vector<float>   *AntiKt4H1Topo_jvtx_x;
   vector<float>   *AntiKt4H1Topo_jvtx_y;
   vector<float>   *AntiKt4H1Topo_jvtx_z;
   vector<float>   *AntiKt4H1Topo_e_PreSamplerB;
   vector<float>   *AntiKt4H1Topo_e_EMB1;
   vector<float>   *AntiKt4H1Topo_e_EMB2;
   vector<float>   *AntiKt4H1Topo_e_EMB3;
   vector<float>   *AntiKt4H1Topo_e_PreSamplerE;
   vector<float>   *AntiKt4H1Topo_e_EME1;
   vector<float>   *AntiKt4H1Topo_e_EME2;
   vector<float>   *AntiKt4H1Topo_e_EME3;
   vector<float>   *AntiKt4H1Topo_e_HEC0;
   vector<float>   *AntiKt4H1Topo_e_HEC1;
   vector<float>   *AntiKt4H1Topo_e_HEC2;
   vector<float>   *AntiKt4H1Topo_e_HEC3;
   vector<float>   *AntiKt4H1Topo_e_TileBar0;
   vector<float>   *AntiKt4H1Topo_e_TileBar1;
   vector<float>   *AntiKt4H1Topo_e_TileBar2;
   vector<float>   *AntiKt4H1Topo_e_TileGap1;
   vector<float>   *AntiKt4H1Topo_e_TileGap2;
   vector<float>   *AntiKt4H1Topo_e_TileGap3;
   vector<float>   *AntiKt4H1Topo_e_TileExt0;
   vector<float>   *AntiKt4H1Topo_e_TileExt1;
   vector<float>   *AntiKt4H1Topo_e_TileExt2;
   vector<float>   *AntiKt4H1Topo_e_FCAL0;
   vector<float>   *AntiKt4H1Topo_e_FCAL1;
   vector<float>   *AntiKt4H1Topo_e_FCAL2;
   vector<vector<float> > *AntiKt4H1Topo_shapeBins;
   vector<int>     *AntiKt4H1Topo_Nconst;
   vector<vector<float> > *AntiKt4H1Topo_ptconst_emscale;
   vector<vector<float> > *AntiKt4H1Topo_econst_emscale;
   vector<vector<float> > *AntiKt4H1Topo_etaconst_emscale;
   vector<vector<float> > *AntiKt4H1Topo_phiconst_emscale;
   vector<vector<float> > *AntiKt4H1Topo_weightconst_emscale;
   vector<float>   *AntiKt4H1Topo_constscale_E;
   vector<float>   *AntiKt4H1Topo_constscale_pt;
   vector<float>   *AntiKt4H1Topo_constscale_m;
   vector<float>   *AntiKt4H1Topo_constscale_eta;
   vector<float>   *AntiKt4H1Topo_constscale_phi;
   vector<double>  *AntiKt4H1Topo_flavor_component_ip2d_pu;
   vector<double>  *AntiKt4H1Topo_flavor_component_ip2d_pb;
   vector<double>  *AntiKt4H1Topo_flavor_component_ip2d_ntrk;
   vector<double>  *AntiKt4H1Topo_flavor_component_ip3d_pu;
   vector<double>  *AntiKt4H1Topo_flavor_component_ip3d_pb;
   vector<double>  *AntiKt4H1Topo_flavor_component_ip3d_ntrk;
   vector<double>  *AntiKt4H1Topo_flavor_component_sv1_pu;
   vector<double>  *AntiKt4H1Topo_flavor_component_sv1_pb;
   vector<double>  *AntiKt4H1Topo_flavor_component_sv2_pu;
   vector<double>  *AntiKt4H1Topo_flavor_component_sv2_pb;
   vector<double>  *AntiKt4H1Topo_flavor_component_svp_mass;
   vector<double>  *AntiKt4H1Topo_flavor_component_svp_n2t;
   vector<double>  *AntiKt4H1Topo_flavor_component_svp_svok;
   vector<double>  *AntiKt4H1Topo_flavor_component_svp_ntrk;
   vector<double>  *AntiKt4H1Topo_flavor_component_svp_ntrkv;
   vector<double>  *AntiKt4H1Topo_flavor_component_svp_ntrkj;
   vector<double>  *AntiKt4H1Topo_flavor_component_svp_efrc;
   vector<double>  *AntiKt4H1Topo_flavor_component_sv0p_mass;
   vector<double>  *AntiKt4H1Topo_flavor_component_sv0p_n2t;
   vector<double>  *AntiKt4H1Topo_flavor_component_sv0p_svok;
   vector<double>  *AntiKt4H1Topo_flavor_component_sv0p_ntrk;
   vector<double>  *AntiKt4H1Topo_flavor_component_sv0p_ntrkv;
   vector<double>  *AntiKt4H1Topo_flavor_component_sv0p_ntrkj;
   vector<double>  *AntiKt4H1Topo_flavor_component_sv0p_efrc;
   vector<double>  *AntiKt4H1Topo_flavor_component_jfit_pu;
   vector<double>  *AntiKt4H1Topo_flavor_component_jfit_pb;
   vector<double>  *AntiKt4H1Topo_flavor_component_jfit_nvtxnt;
   vector<double>  *AntiKt4H1Topo_flavor_component_jfit_nvtx1t;
   vector<double>  *AntiKt4H1Topo_flavor_component_jfit_ntrk;
   vector<double>  *AntiKt4H1Topo_flavor_component_jfit_efrc;
   vector<double>  *AntiKt4H1Topo_flavor_component_jfit_mass;
   vector<double>  *AntiKt4H1Topo_flavor_component_jfit_sig3d;
   vector<double>  *AntiKt4H1Topo_flavor_weight;
   vector<double>  *AntiKt4H1Topo_flavor_weight_TrackCounting2D;
   vector<double>  *AntiKt4H1Topo_flavor_weight_JetProb;
   vector<double>  *AntiKt4H1Topo_flavor_weight_IP1D;
   vector<double>  *AntiKt4H1Topo_flavor_weight_IP2D;
   vector<double>  *AntiKt4H1Topo_flavor_weight_IP3D;
   vector<double>  *AntiKt4H1Topo_flavor_weight_SV0;
   vector<double>  *AntiKt4H1Topo_flavor_weight_SV1;
   vector<double>  *AntiKt4H1Topo_flavor_weight_SV2;
   vector<double>  *AntiKt4H1Topo_flavor_weight_JetFitterTag;
   vector<double>  *AntiKt4H1Topo_flavor_weight_JetFitterCOMB;
   vector<double>  *AntiKt4H1Topo_flavor_weight_JetFitterTagNN;
   vector<double>  *AntiKt4H1Topo_flavor_weight_JetFitterCOMBNN;
   vector<double>  *AntiKt4H1Topo_flavor_weight_SoftMuonTag;
   vector<double>  *AntiKt4H1Topo_flavor_weight_SoftElectronTag;
   vector<unsigned int> *AntiKt4H1Topo_RoIword;
   vector<double>  *AntiKt4H1Topo_flavor_truth_dRminToB;
   vector<double>  *AntiKt4H1Topo_flavor_truth_dRminToC;
   vector<double>  *AntiKt4H1Topo_flavor_truth_dRminToT;
   vector<int>     *AntiKt4H1Topo_flavor_truth_BHadronpdg;
   vector<int>     *AntiKt4H1Topo_flavor_truth_trueflav;
   vector<float>   *AntiKt4H1Topo_el_dr;
   vector<int>     *AntiKt4H1Topo_el_matched;
   vector<float>   *AntiKt4H1Topo_mu_dr;
   vector<int>     *AntiKt4H1Topo_mu_matched;
   Int_t           trig_Nav_n;
   vector<short>   *trig_Nav_chain_ChainId;
   vector<vector<int> > *trig_Nav_chain_RoIType;
   vector<vector<int> > *trig_Nav_chain_RoIIndex;
   Int_t           trig_RoI_L2_b_n;
   vector<short>   *trig_RoI_L2_b_type;
   vector<short>   *trig_RoI_L2_b_lastStep;
   vector<int>     *trig_RoI_L2_b_Jet_ROI;
   vector<int>     *trig_RoI_L2_b_Jet_ROIStatus;
   vector<vector<int> > *trig_RoI_L2_b_TrigL2BjetContainer;
   vector<vector<int> > *trig_RoI_L2_b_TrigL2BjetContainerStatus;
   vector<vector<int> > *trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_Jet;
   vector<vector<int> > *trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_JetStatus;
   vector<vector<int> > *trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_Jet;
   vector<vector<int> > *trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_JetStatus;
   Int_t           trig_RoI_EF_b_n;
   vector<short>   *trig_RoI_EF_b_type;
   vector<short>   *trig_RoI_EF_b_lastStep;
   vector<int>     *trig_RoI_EF_b_Jet_ROI;
   vector<int>     *trig_RoI_EF_b_Jet_ROIStatus;
   vector<vector<int> > *trig_RoI_EF_b_TrigEFBjetContainer;
   vector<vector<int> > *trig_RoI_EF_b_TrigEFBjetContainerStatus;
   vector<vector<int> > *trig_RoI_EF_b_Rec__TrackParticleContainer;
   vector<vector<int> > *trig_RoI_EF_b_Rec__TrackParticleContainerStatus;
   Int_t           trig_L1_jet_n;
   vector<float>   *trig_L1_jet_eta;
   vector<float>   *trig_L1_jet_phi;
   vector<vector<string> > *trig_L1_jet_thrNames;
   vector<vector<float> > *trig_L1_jet_thrValues;
   vector<unsigned int> *trig_L1_jet_thrPattern;
   vector<float>   *trig_L1_jet_et4x4;
   vector<float>   *trig_L1_jet_et6x6;
   vector<float>   *trig_L1_jet_et8x8;
   vector<unsigned int> *trig_L1_jet_RoIWord;
   UInt_t          trig_DB_SMK;
   UInt_t          trig_DB_L1PSK;
   UInt_t          trig_DB_HLTPSK;
   vector<unsigned int> *trig_L1_TAV;
   vector<short>   *trig_L2_passedPhysics;
   vector<short>   *trig_EF_passedPhysics;
   vector<unsigned int> *trig_L1_TBP;
   vector<unsigned int> *trig_L1_TAP;
   vector<short>   *trig_L2_passedRaw;
   vector<short>   *trig_EF_passedRaw;
   vector<short>   *trig_L2_resurrected;
   vector<short>   *trig_EF_resurrected;
   vector<short>   *trig_L2_passedThrough;
   vector<short>   *trig_EF_passedThrough;
   Int_t           trig_l2sitrk_jet_n;
   vector<int>     *trig_l2sitrk_jet_algorithmId;
   vector<short>   *trig_l2sitrk_jet_trackStatus;
   vector<float>   *trig_l2sitrk_jet_chi2Ndof;
   vector<int>     *trig_l2sitrk_jet_nStrawHits;
   vector<int>     *trig_l2sitrk_jet_nHighThrHits;
   vector<int>     *trig_l2sitrk_jet_nPixelSpacePoints;
   vector<int>     *trig_l2sitrk_jet_nSCT_SpacePoints;
   vector<long>    *trig_l2sitrk_jet_hitPattern;
   vector<int>     *trig_l2sitrk_jet_nStraw;
   vector<int>     *trig_l2sitrk_jet_nStrawTime;
   vector<float>   *trig_l2sitrk_jet_a0;
   vector<float>   *trig_l2sitrk_jet_z0;
   vector<float>   *trig_l2sitrk_jet_phi0;
   vector<float>   *trig_l2sitrk_jet_eta;
   vector<float>   *trig_l2sitrk_jet_pt;
   vector<vector<float> > *trig_l2sitrk_jet_covariance;
   Int_t           trig_l2idtrk_jet_n;
   vector<int>     *trig_l2idtrk_jet_algorithmId;
   vector<short>   *trig_l2idtrk_jet_trackStatus;
   vector<float>   *trig_l2idtrk_jet_chi2Ndof;
   vector<int>     *trig_l2idtrk_jet_nStrawHits;
   vector<int>     *trig_l2idtrk_jet_nHighThrHits;
   vector<int>     *trig_l2idtrk_jet_nPixelSpacePoints;
   vector<int>     *trig_l2idtrk_jet_nSCT_SpacePoints;
   vector<long>    *trig_l2idtrk_jet_hitPattern;
   vector<int>     *trig_l2idtrk_jet_nStraw;
   vector<int>     *trig_l2idtrk_jet_nStrawTime;
   vector<float>   *trig_l2idtrk_jet_a0;
   vector<float>   *trig_l2idtrk_jet_z0;
   vector<float>   *trig_l2idtrk_jet_phi0;
   vector<float>   *trig_l2idtrk_jet_eta;
   vector<float>   *trig_l2idtrk_jet_pt;
   vector<vector<float> > *trig_l2idtrk_jet_covariance;
   Int_t           trig_bjefid_n;
   vector<float>   *trig_bjefid_d0;
   vector<float>   *trig_bjefid_z0;
   vector<float>   *trig_bjefid_phi;
   vector<float>   *trig_bjefid_theta;
   vector<float>   *trig_bjefid_qoverp;
   vector<float>   *trig_bjefid_pt;
   vector<float>   *trig_bjefid_eta;
   vector<float>   *trig_bjefid_err_d0;
   vector<float>   *trig_bjefid_err_z0;
   vector<float>   *trig_bjefid_err_phi;
   vector<float>   *trig_bjefid_err_theta;
   vector<float>   *trig_bjefid_err_qoverp;
   vector<float>   *trig_bjefid_cov_d0_z0;
   vector<float>   *trig_bjefid_cov_d0_phi;
   vector<float>   *trig_bjefid_cov_d0_theta;
   vector<float>   *trig_bjefid_cov_d0_qoverp;
   vector<float>   *trig_bjefid_cov_z0_phi;
   vector<float>   *trig_bjefid_cov_z0_theta;
   vector<float>   *trig_bjefid_cov_z0_qoverp;
   vector<float>   *trig_bjefid_cov_phi_theta;
   vector<float>   *trig_bjefid_cov_phi_qoverp;
   vector<float>   *trig_bjefid_cov_theta_qoverp;
   vector<float>   *trig_bjefid_d0_wrtPV;
   vector<float>   *trig_bjefid_z0_wrtPV;
   vector<float>   *trig_bjefid_phi_wrtPV;
   vector<float>   *trig_bjefid_err_d0_wrtPV;
   vector<float>   *trig_bjefid_err_z0_wrtPV;
   vector<float>   *trig_bjefid_err_phi_wrtPV;
   vector<float>   *trig_bjefid_err_theta_wrtPV;
   vector<float>   *trig_bjefid_err_qoverp_wrtPV;
   vector<float>   *trig_bjefid_cov_d0_z0_wrtPV;
   vector<float>   *trig_bjefid_cov_d0_phi_wrtPV;
   vector<float>   *trig_bjefid_cov_d0_theta_wrtPV;
   vector<float>   *trig_bjefid_cov_d0_qoverp_wrtPV;
   vector<float>   *trig_bjefid_cov_z0_phi_wrtPV;
   vector<float>   *trig_bjefid_cov_z0_theta_wrtPV;
   vector<float>   *trig_bjefid_cov_z0_qoverp_wrtPV;
   vector<float>   *trig_bjefid_cov_phi_theta_wrtPV;
   vector<float>   *trig_bjefid_cov_phi_qoverp_wrtPV;
   vector<float>   *trig_bjefid_cov_theta_qoverp_wrtPV;
   vector<float>   *trig_bjefid_d0_wrtBS;
   vector<float>   *trig_bjefid_z0_wrtBS;
   vector<float>   *trig_bjefid_phi_wrtBS;
   vector<float>   *trig_bjefid_err_d0_wrtBS;
   vector<float>   *trig_bjefid_err_z0_wrtBS;
   vector<float>   *trig_bjefid_err_phi_wrtBS;
   vector<float>   *trig_bjefid_err_theta_wrtBS;
   vector<float>   *trig_bjefid_err_qoverp_wrtBS;
   vector<float>   *trig_bjefid_cov_d0_z0_wrtBS;
   vector<float>   *trig_bjefid_cov_d0_phi_wrtBS;
   vector<float>   *trig_bjefid_cov_d0_theta_wrtBS;
   vector<float>   *trig_bjefid_cov_d0_qoverp_wrtBS;
   vector<float>   *trig_bjefid_cov_z0_phi_wrtBS;
   vector<float>   *trig_bjefid_cov_z0_theta_wrtBS;
   vector<float>   *trig_bjefid_cov_z0_qoverp_wrtBS;
   vector<float>   *trig_bjefid_cov_phi_theta_wrtBS;
   vector<float>   *trig_bjefid_cov_phi_qoverp_wrtBS;
   vector<float>   *trig_bjefid_cov_theta_qoverp_wrtBS;
   vector<float>   *trig_bjefid_chi2;
   vector<int>     *trig_bjefid_ndof;
   vector<int>     *trig_bjefid_nBLHits;
   vector<int>     *trig_bjefid_nPixHits;
   vector<int>     *trig_bjefid_nSCTHits;
   vector<int>     *trig_bjefid_nTRTHits;
   vector<int>     *trig_bjefid_nPixHoles;
   vector<int>     *trig_bjefid_nSCTHoles;
   vector<int>     *trig_bjefid_nMDTHits;
   vector<int>     *trig_bjefid_nCSCEtaHits;
   vector<int>     *trig_bjefid_nCSCPhiHits;
   vector<int>     *trig_bjefid_nRPCEtaHits;
   vector<int>     *trig_bjefid_nRPCPhiHits;
   vector<int>     *trig_bjefid_nTGCEtaHits;
   vector<int>     *trig_bjefid_nTGCPhiHits;
   vector<int>     *trig_bjefid_nHits;
   vector<int>     *trig_bjefid_nHoles;
   vector<int>     *trig_bjefid_hitPattern;
   vector<float>   *trig_bjefid_TRTHighTHitsRatio;
   vector<float>   *trig_bjefid_pixeldEdx;
   vector<int>     *trig_bjefid_fitter;
   vector<int>     *trig_bjefid_patternReco1;
   vector<int>     *trig_bjefid_patternReco2;
   vector<int>     *trig_bjefid_seedFinder;
   vector<int>     *trig_bjefid_blayerPrediction_expectHit;
   vector<vector<float> > *trig_bjefid_blayerPrediction_x;
   vector<vector<float> > *trig_bjefid_blayerPrediction_y;
   vector<vector<float> > *trig_bjefid_blayerPrediction_z;
   vector<vector<float> > *trig_bjefid_blayerPrediction_locX;
   vector<vector<float> > *trig_bjefid_blayerPrediction_locY;
   vector<vector<float> > *trig_bjefid_blayerPrediction_err_locX;
   vector<vector<float> > *trig_bjefid_blayerPrediction_err_locY;
   vector<vector<float> > *trig_bjefid_blayerPrediction_etaDistToEdge;
   vector<vector<float> > *trig_bjefid_blayerPrediction_phiDistToEdge;
   vector<vector<unsigned int> > *trig_bjefid_blayerPrediction_detElementId;
   vector<vector<int> > *trig_bjefid_blayerPrediction_row;
   vector<vector<int> > *trig_bjefid_blayerPrediction_col;
   vector<vector<int> > *trig_bjefid_blayerPrediction_type;
   vector<float>   *trig_bjefid_mc_probability;
   vector<int>     *trig_bjefid_mc_barcode;
   vector<int>     *trig_bjefid_mc_index;
   vector<float>   *trig_bjefid_vertx;
   vector<float>   *trig_bjefid_verty;
   vector<float>   *trig_bjefid_vertz;
   Int_t           trig_l2bj_n;
   vector<int>     *trig_l2bj_roiId;
   vector<int>     *trig_l2bj_valid;
   vector<float>   *trig_l2bj_prmVtx;
   vector<float>   *trig_l2bj_xComb;
   vector<float>   *trig_l2bj_xIP1D;
   vector<float>   *trig_l2bj_xIP2D;
   vector<float>   *trig_l2bj_xIP3D;
   vector<float>   *trig_l2bj_xCHI2;
   vector<float>   *trig_l2bj_xSV;
   vector<float>   *trig_l2bj_xMVtx;
   vector<float>   *trig_l2bj_xEVtx;
   vector<float>   *trig_l2bj_xNVtx;
   vector<float>   *trig_l2bj_eta;
   vector<float>   *trig_l2bj_phi;
   vector<int>     *trig_l2bj_trksi_n;
   vector<vector<int> > *trig_l2bj_trksi_index;
   vector<int>     *trig_l2bj_trkid_n;
   vector<vector<int> > *trig_l2bj_trkid_index;
   Int_t           trig_efbj_n;
   vector<int>     *trig_efbj_roiId;
   vector<int>     *trig_efbj_valid;
   vector<float>   *trig_efbj_prmVtx;
   vector<float>   *trig_efbj_xComb;
   vector<float>   *trig_efbj_xIP1D;
   vector<float>   *trig_efbj_xIP2D;
   vector<float>   *trig_efbj_xIP3D;
   vector<float>   *trig_efbj_xCHI2;
   vector<float>   *trig_efbj_xSV;
   vector<float>   *trig_efbj_xMVtx;
   vector<float>   *trig_efbj_xEVtx;
   vector<float>   *trig_efbj_xNVtx;
   vector<float>   *trig_efbj_eta;
   vector<float>   *trig_efbj_phi;
   vector<int>     *trig_efbj_trk_n;
   vector<vector<int> > *trig_efbj_trk_index;
   Int_t           el_n;
   vector<float>   *el_E;
   vector<float>   *el_Et;
   vector<float>   *el_pt;
   vector<float>   *el_m;
   vector<float>   *el_eta;
   vector<float>   *el_phi;
   vector<float>   *el_px;
   vector<float>   *el_py;
   vector<float>   *el_pz;
   vector<float>   *el_charge;
   vector<int>     *el_author;
   vector<unsigned int> *el_isEM;
   vector<int>     *el_convFlag;
   vector<int>     *el_isConv;
   vector<int>     *el_nConv;
   vector<int>     *el_nSingleTrackConv;
   vector<int>     *el_nDoubleTrackConv;
   vector<int>     *el_type;
   vector<int>     *el_origin;
   vector<float>   *el_truth_E;
   vector<float>   *el_truth_pt;
   vector<float>   *el_truth_eta;
   vector<float>   *el_truth_phi;
   vector<int>     *el_truth_type;
   vector<int>     *el_truth_status;
   vector<int>     *el_truth_barcode;
   vector<int>     *el_truth_mothertype;
   vector<int>     *el_truth_motherbarcode;
   vector<int>     *el_truth_hasHardBrem;
   vector<int>     *el_truth_matched;
   vector<int>     *el_loose;
   vector<int>     *el_medium;
   vector<int>     *el_mediumIso;
   vector<int>     *el_tight;
   vector<int>     *el_tightIso;
   vector<float>   *el_Ethad;
   vector<float>   *el_Ethad1;
   vector<float>   *el_f1;
   vector<float>   *el_f1core;
   vector<float>   *el_Emins1;
   vector<float>   *el_fside;
   vector<float>   *el_Emax2;
   vector<float>   *el_ws3;
   vector<float>   *el_wstot;
   vector<float>   *el_emaxs1;
   vector<float>   *el_deltaEs;
   vector<float>   *el_E233;
   vector<float>   *el_E237;
   vector<float>   *el_E277;
   vector<float>   *el_weta2;
   vector<float>   *el_f3;
   vector<float>   *el_f3core;
   vector<float>   *el_rphiallcalo;
   vector<float>   *el_Etcone45;
   vector<float>   *el_Etcone20;
   vector<float>   *el_Etcone30;
   vector<float>   *el_Etcone40;
   vector<float>   *el_ptcone30;
   vector<float>   *el_convanglematch;
   vector<float>   *el_convtrackmatch;
   vector<int>     *el_hasconv;
   vector<float>   *el_convvtxx;
   vector<float>   *el_convvtxy;
   vector<float>   *el_convvtxz;
   vector<float>   *el_Rconv;
   vector<float>   *el_zconv;
   vector<float>   *el_convvtxchi2;
   vector<float>   *el_pt1conv;
   vector<int>     *el_convtrk1nBLHits;
   vector<int>     *el_convtrk1nPixHits;
   vector<int>     *el_convtrk1nSCTHits;
   vector<int>     *el_convtrk1nTRTHits;
   vector<float>   *el_pt2conv;
   vector<int>     *el_convtrk2nBLHits;
   vector<int>     *el_convtrk2nPixHits;
   vector<int>     *el_convtrk2nSCTHits;
   vector<int>     *el_convtrk2nTRTHits;
   vector<float>   *el_ptconv;
   vector<float>   *el_pzconv;
   vector<float>   *el_pos7;
   vector<float>   *el_etacorrmag;
   vector<float>   *el_deltaeta1;
   vector<float>   *el_deltaeta2;
   vector<float>   *el_deltaphi2;
   vector<float>   *el_deltaphiRescaled;
   vector<float>   *el_expectHitInBLayer;
   vector<float>   *el_reta;
   vector<float>   *el_rphi;
   vector<float>   *el_EtringnoisedR03sig2;
   vector<float>   *el_EtringnoisedR03sig3;
   vector<float>   *el_EtringnoisedR03sig4;
   vector<double>  *el_isolationlikelihoodjets;
   vector<double>  *el_isolationlikelihoodhqelectrons;
   vector<double>  *el_electronweight;
   vector<double>  *el_electronbgweight;
   vector<double>  *el_softeweight;
   vector<double>  *el_softebgweight;
   vector<double>  *el_neuralnet;
   vector<double>  *el_Hmatrix;
   vector<double>  *el_Hmatrix5;
   vector<double>  *el_adaboost;
   vector<double>  *el_softeneuralnet;
   vector<float>   *el_zvertex;
   vector<float>   *el_errz;
   vector<float>   *el_etap;
   vector<float>   *el_depth;
   vector<int>     *el_refittedTrack_n;
   vector<vector<int> > *el_refittedTrack_author;
   vector<vector<float> > *el_refittedTrack_chi2;
   vector<vector<float> > *el_refittedTrack_qoverp;
   vector<vector<float> > *el_refittedTrack_d0;
   vector<vector<float> > *el_refittedTrack_z0;
   vector<vector<float> > *el_refittedTrack_theta;
   vector<vector<float> > *el_refittedTrack_phi;
   vector<vector<float> > *el_refittedTrack_LMqoverp;
   vector<vector<int> > *el_refittedTrack_hasBrem;
   vector<vector<float> > *el_refittedTrack_bremRadius;
   vector<vector<float> > *el_refittedTrack_bremZ;
   vector<vector<float> > *el_refittedTrack_bremRadiusErr;
   vector<vector<float> > *el_refittedTrack_bremZErr;
   vector<float>   *el_Es0;
   vector<float>   *el_etas0;
   vector<float>   *el_phis0;
   vector<float>   *el_Es1;
   vector<float>   *el_etas1;
   vector<float>   *el_phis1;
   vector<float>   *el_Es2;
   vector<float>   *el_etas2;
   vector<float>   *el_phis2;
   vector<float>   *el_Es3;
   vector<float>   *el_etas3;
   vector<float>   *el_phis3;
   vector<float>   *el_E_PreSamplerB;
   vector<float>   *el_E_EMB1;
   vector<float>   *el_E_EMB2;
   vector<float>   *el_E_EMB3;
   vector<float>   *el_E_PreSamplerE;
   vector<float>   *el_E_EME1;
   vector<float>   *el_E_EME2;
   vector<float>   *el_E_EME3;
   vector<float>   *el_E_HEC0;
   vector<float>   *el_E_HEC1;
   vector<float>   *el_E_HEC2;
   vector<float>   *el_E_HEC3;
   vector<float>   *el_E_TileBar0;
   vector<float>   *el_E_TileBar1;
   vector<float>   *el_E_TileBar2;
   vector<float>   *el_E_TileGap1;
   vector<float>   *el_E_TileGap2;
   vector<float>   *el_E_TileGap3;
   vector<float>   *el_E_TileExt0;
   vector<float>   *el_E_TileExt1;
   vector<float>   *el_E_TileExt2;
   vector<float>   *el_E_FCAL0;
   vector<float>   *el_E_FCAL1;
   vector<float>   *el_E_FCAL2;
   vector<float>   *el_cl_E;
   vector<float>   *el_cl_pt;
   vector<float>   *el_cl_eta;
   vector<float>   *el_cl_phi;
   vector<float>   *el_firstEdens;
   vector<float>   *el_cellmaxfrac;
   vector<float>   *el_longitudinal;
   vector<float>   *el_secondlambda;
   vector<float>   *el_lateral;
   vector<float>   *el_secondR;
   vector<float>   *el_centerlambda;
   vector<float>   *el_rawcl_Es0;
   vector<float>   *el_rawcl_etas0;
   vector<float>   *el_rawcl_phis0;
   vector<float>   *el_rawcl_Es1;
   vector<float>   *el_rawcl_etas1;
   vector<float>   *el_rawcl_phis1;
   vector<float>   *el_rawcl_Es2;
   vector<float>   *el_rawcl_etas2;
   vector<float>   *el_rawcl_phis2;
   vector<float>   *el_rawcl_Es3;
   vector<float>   *el_rawcl_etas3;
   vector<float>   *el_rawcl_phis3;
   vector<float>   *el_rawcl_E;
   vector<float>   *el_rawcl_pt;
   vector<float>   *el_rawcl_eta;
   vector<float>   *el_rawcl_phi;
   vector<float>   *el_refittedtrackcovd0;
   vector<float>   *el_refittedtrackcovz0;
   vector<float>   *el_refittedtrackcovphi;
   vector<float>   *el_refittedtrackcovtheta;
   vector<float>   *el_refittedtrackcovqoverp;
   vector<float>   *el_refittedtrackcovd0z0;
   vector<float>   *el_refittedtrackcovz0phi;
   vector<float>   *el_refittedtrackcovz0theta;
   vector<float>   *el_refittedtrackcovz0qoverp;
   vector<float>   *el_refittedtrackcovd0phi;
   vector<float>   *el_refittedtrackcovd0theta;
   vector<float>   *el_refittedtrackcovd0qoverp;
   vector<float>   *el_refittedtrackcovphitheta;
   vector<float>   *el_refittedtrackcovphiqoverp;
   vector<float>   *el_refittedtrackcovthetaqoverp;
   vector<float>   *el_trackd0;
   vector<float>   *el_trackz0;
   vector<float>   *el_trackphi;
   vector<float>   *el_tracktheta;
   vector<float>   *el_trackqoverp;
   vector<float>   *el_trackpt;
   vector<float>   *el_tracketa;
   vector<float>   *el_trackcov_d0;
   vector<float>   *el_trackcov_z0;
   vector<float>   *el_trackcov_phi;
   vector<float>   *el_trackcov_theta;
   vector<float>   *el_trackcov_qoverp;
   vector<float>   *el_trackcov_d0_z0;
   vector<float>   *el_trackcov_d0_phi;
   vector<float>   *el_trackcov_d0_theta;
   vector<float>   *el_trackcov_d0_qoverp;
   vector<float>   *el_trackcov_z0_phi;
   vector<float>   *el_trackcov_z0_theta;
   vector<float>   *el_trackcov_z0_qoverp;
   vector<float>   *el_trackcov_phi_theta;
   vector<float>   *el_trackcov_phi_qoverp;
   vector<float>   *el_trackcov_theta_qoverp;
   vector<float>   *el_trackfitchi2;
   vector<int>     *el_trackfitndof;
   vector<int>     *el_nBLHits;
   vector<int>     *el_nPixHits;
   vector<int>     *el_nSCTHits;
   vector<int>     *el_nTRTHits;
   vector<int>     *el_nPixHoles;
   vector<int>     *el_nSCTHoles;
   vector<int>     *el_nBLSharedHits;
   vector<int>     *el_nPixSharedHits;
   vector<int>     *el_nSCTSharedHits;
   vector<int>     *el_nTRTHighTHits;
   vector<int>     *el_nTRTOutliers;
   vector<int>     *el_nTRTHighTOutliers;
   vector<int>     *el_nSiHits;
   vector<float>   *el_TRTHighTHitsRatio;
   vector<float>   *el_pixeldEdx;
   vector<float>   *el_eProbabilityComb;
   vector<float>   *el_eProbabilityHT;
   vector<float>   *el_eProbabilityToT;
   vector<float>   *el_eProbabilityBrem;
   vector<float>   *el_vertx;
   vector<float>   *el_verty;
   vector<float>   *el_vertz;
   vector<int>     *el_hastrack;
   vector<float>   *el_deltaEmax2;
   vector<float>   *el_calibHitsShowerDepth;
   vector<float>   *el_trackd0beam;
   vector<float>   *el_tracksigd0beam;
   vector<float>   *el_trackd0pv;
   vector<float>   *el_tracksigd0pv;
   vector<float>   *el_trackz0pv;
   vector<float>   *el_tracksigz0pv;
   vector<float>   *el_trackd0pvunbiased;
   vector<float>   *el_tracksigd0pvunbiased;
   vector<float>   *el_trackz0pvunbiased;
   vector<float>   *el_tracksigz0pvunbiased;
   vector<unsigned int> *el_isIso;
   vector<float>   *el_mvaptcone20;
   vector<float>   *el_mvaptcone30;
   vector<float>   *el_mvaptcone40;
   vector<float>   *el_jet_dr;
   vector<float>   *el_jet_E;
   vector<float>   *el_jet_pt;
   vector<float>   *el_jet_m;
   vector<float>   *el_jet_eta;
   vector<float>   *el_jet_phi;
   vector<float>   *el_jet_truth_dr;
   vector<float>   *el_jet_truth_E;
   vector<float>   *el_jet_truth_pt;
   vector<float>   *el_jet_truth_m;
   vector<float>   *el_jet_truth_eta;
   vector<float>   *el_jet_truth_phi;
   vector<int>     *el_jet_truth_matched;
   vector<int>     *el_jet_matched;
   vector<float>   *el_Etcone40_pt_corrected;
   vector<float>   *el_Etcone40_ED_corrected;
   vector<float>   *el_Etcone40_corrected;
   vector<float>   *el_EF_dr;
   vector<int>     *el_EF_index;
   vector<float>   *el_L2_dr;
   vector<int>     *el_L2_index;
   vector<float>   *el_L1_dr;
   vector<int>     *el_L1_index;
   Bool_t          EF_2e12_tight;
   Bool_t          EF_2e15_medium;
   Bool_t          EF_2e20_loose;
   Bool_t          EF_2e5_medium;
   Bool_t          EF_2e5_medium1;
   Bool_t          EF_2g10_loose;
   Bool_t          EF_2g10_loose_mu6;
   Bool_t          EF_2g17i_tight;
   Bool_t          EF_2g20_loose;
   Bool_t          EF_2mu10;
   Bool_t          EF_2mu20;
   Bool_t          EF_2mu4;
   Bool_t          EF_2mu4_Bmumu;
   Bool_t          EF_2mu4_Bmumux;
   Bool_t          EF_2mu4_DiMu;
   Bool_t          EF_2mu4_DiMu_SiTrk;
   Bool_t          EF_2mu4_DiMu_noVtx;
   Bool_t          EF_2mu4_DiMu_noVtx_noOS;
   Bool_t          EF_2mu4_Jpsimumu;
   Bool_t          EF_2mu4_Upsimumu;
   Bool_t          EF_2mu6;
   Bool_t          EF_2mu6_Bmumu;
   Bool_t          EF_2mu6_Bmumux;
   Bool_t          EF_2mu6_DiMu;
   Bool_t          EF_2mu6_Jpsimumu;
   Bool_t          EF_2mu6_Upsimumu;
   Bool_t          EF_2mu6_e10_loose;
   Bool_t          EF_2mu6_g10_loose;
   Bool_t          EF_e105_loose1;
   Bool_t          EF_e10_NoCut;
   Bool_t          EF_e10_loose;
   Bool_t          EF_e10_loose_IdScan;
   Bool_t          EF_e10_loose_SiTrk;
   Bool_t          EF_e10_loose_TRT;
   Bool_t          EF_e10_loose_mu6;
   Bool_t          EF_e10_medium;
   Bool_t          EF_e10_medium_SiTrk;
   Bool_t          EF_e10_medium_SiTrk_robust;
   Bool_t          EF_e12_medium;
   Bool_t          EF_e15_medium;
   Bool_t          EF_e15i_medium;
   Bool_t          EF_e20_loose;
   Bool_t          EF_e20_loose_IdScan;
   Bool_t          EF_e20_loose_NoIDTrkCut;
   Bool_t          EF_e20_loose_SiTrk;
   Bool_t          EF_e20_loose_SiTrk_robust;
   Bool_t          EF_e20_loose_TRT;
   Bool_t          EF_e20_loose_passEF;
   Bool_t          EF_e20_loose_passL2;
   Bool_t          EF_e20_loose_xe20;
   Bool_t          EF_e20_loose_xe30;
   Bool_t          EF_e20_medium;
   Bool_t          EF_e20i_loose;
   Bool_t          EF_e22i_tight;
   Bool_t          EF_e25_loose;
   Bool_t          EF_e25_medium;
   Bool_t          EF_e25i_loose;
   Bool_t          EF_e25i_medium;
   Bool_t          EF_e55_loose1;
   Bool_t          EF_e5_medium;
   Bool_t          EF_e5_medium1;
   Bool_t          EF_e5_medium_L2SW;
   Bool_t          EF_e5_medium_MV;
   Bool_t          EF_e5_medium_Ringer;
   Bool_t          EF_e5_medium_e10_loose;
   Bool_t          EF_em105_passHLT;
   Bool_t          EF_em15_passHLT;
   Bool_t          EF_em15i_passHLT;
   Bool_t          EF_em20_passHLT;
   Bool_t          EF_em20i_passHLT;
   Bool_t          EF_g105;
   Bool_t          EF_g10_loose;
   Bool_t          EF_g10_loose_larcalib;
   Bool_t          EF_g10_nocut;
   Bool_t          EF_g150;
   Bool_t          EF_g15_loose;
   Bool_t          EF_g20_loose;
   Bool_t          EF_g20_loose_cnv;
   Bool_t          EF_g20_loose_larcalib;
   Bool_t          EF_g20_loose_xe20;
   Bool_t          EF_g20i_loose;
   Bool_t          EF_g20i_loose_larcalib;
   Bool_t          EF_g25_loose;
   Bool_t          EF_g25_loose_xe30;
   Bool_t          EF_g25i_loose;
   Bool_t          EF_g55_tight;
   Bool_t          EF_mu10;
   Bool_t          EF_mu10_DsPhiPi;
   Bool_t          EF_mu10_DsPhiPi_FS;
   Bool_t          EF_mu10_MG;
   Bool_t          EF_mu10_MSonly;
   Bool_t          EF_mu10_MuTagIMO;
   Bool_t          EF_mu10_NoIDTrkCut;
   Bool_t          EF_mu10_SiTrk;
   Bool_t          EF_mu10i_loose;
   Bool_t          EF_mu13;
   Bool_t          EF_mu15;
   Bool_t          EF_mu15_xe20;
   Bool_t          EF_mu15i_loose;
   Bool_t          EF_mu20;
   Bool_t          EF_mu20_MSonly;
   Bool_t          EF_mu20_passHLT;
   Bool_t          EF_mu20_slow;
   Bool_t          EF_mu20_xe30;
   Bool_t          EF_mu20i_loose;
   Bool_t          EF_mu20i_loose_MSonly;
   Bool_t          EF_mu4;
   Bool_t          EF_mu4_Bmumu;
   Bool_t          EF_mu4_BmumuX;
   Bool_t          EF_mu4_Bmumu_FS;
   Bool_t          EF_mu4_DiMu;
   Bool_t          EF_mu4_DiMu_FS;
   Bool_t          EF_mu4_DiMu_FS_noOS;
   Bool_t          EF_mu4_DiMu_MG;
   Bool_t          EF_mu4_DiMu_MG_FS;
   Bool_t          EF_mu4_DiMu_SiTrk;
   Bool_t          EF_mu4_DiMu_SiTrk_FS;
   Bool_t          EF_mu4_DiMu_noOS;
   Bool_t          EF_mu4_DsPhiPi;
   Bool_t          EF_mu4_DsPhiPi_FS;
   Bool_t          EF_mu4_Jpsie5e3;
   Bool_t          EF_mu4_Jpsie5e3_FS;
   Bool_t          EF_mu4_Jpsie5e3_SiTrk;
   Bool_t          EF_mu4_Jpsie5e3_SiTrk_FS;
   Bool_t          EF_mu4_Jpsimumu;
   Bool_t          EF_mu4_Jpsimumu_FS;
   Bool_t          EF_mu4_MG;
   Bool_t          EF_mu4_MSonly;
   Bool_t          EF_mu4_MV;
   Bool_t          EF_mu4_SiTrk;
   Bool_t          EF_mu4_Trk_Jpsi;
   Bool_t          EF_mu4_Trk_Jpsi_FS;
   Bool_t          EF_mu4_Trk_Jpsi_loose;
   Bool_t          EF_mu4_Trk_Upsi_FS;
   Bool_t          EF_mu4_Trk_Upsi_loose_FS;
   Bool_t          EF_mu4_Upsimumu;
   Bool_t          EF_mu4_Upsimumu_FS;
   Bool_t          EF_mu4_j10;
   Bool_t          EF_mu4_j10_matched;
   Bool_t          EF_mu4_j20;
   Bool_t          EF_mu4_j20_matched;
   Bool_t          EF_mu4_j40_matched;
   Bool_t          EF_mu4_j70_matched;
   Bool_t          EF_mu4_mu6;
   Bool_t          EF_mu4_tile;
   Bool_t          EF_mu4_trod;
   Bool_t          EF_mu4mu6_Bmumu;
   Bool_t          EF_mu4mu6_DiMu;
   Bool_t          EF_mu4mu6_Jpsimumu;
   Bool_t          EF_mu4mu6_Upsimumu;
   Bool_t          EF_mu6;
   Bool_t          EF_mu6_Bmumu;
   Bool_t          EF_mu6_BmumuX;
   Bool_t          EF_mu6_Bmumu_FS;
   Bool_t          EF_mu6_DiMu;
   Bool_t          EF_mu6_DiMu_FS;
   Bool_t          EF_mu6_DsPhiPi;
   Bool_t          EF_mu6_DsPhiPi_FS;
   Bool_t          EF_mu6_Jpsie5e3;
   Bool_t          EF_mu6_Jpsie5e3_FS;
   Bool_t          EF_mu6_Jpsie5e3_SiTrk;
   Bool_t          EF_mu6_Jpsie5e3_SiTrk_FS;
   Bool_t          EF_mu6_Jpsimumu;
   Bool_t          EF_mu6_Jpsimumu_FS;
   Bool_t          EF_mu6_MG;
   Bool_t          EF_mu6_MSonly;
   Bool_t          EF_mu6_Trk_Jpsi;
   Bool_t          EF_mu6_Upsimumu_FS;
   Bool_t          L1_2EM10;
   Bool_t          L1_2EM10I;
   Bool_t          L1_2EM14;
   Bool_t          L1_2EM3;
   Bool_t          L1_2EM3_EM5;
   Bool_t          L1_2EM5;
   Bool_t          L1_2EM5_MU6;
   Bool_t          L1_2MU0;
   Bool_t          L1_2MU0_MU6;
   Bool_t          L1_2MU10;
   Bool_t          L1_2MU20;
   Bool_t          L1_2MU6;
   Bool_t          L1_2MU6_EM5;
   Bool_t          L1_EM10;
   Bool_t          L1_EM10I;
   Bool_t          L1_EM14;
   Bool_t          L1_EM14I;
   Bool_t          L1_EM14_XE10;
   Bool_t          L1_EM14_XE15;
   Bool_t          L1_EM19I;
   Bool_t          L1_EM3;
   Bool_t          L1_EM3_MV;
   Bool_t          L1_EM5;
   Bool_t          L1_EM5_MU6;
   Bool_t          L1_EM85;
   Bool_t          L1_MU0;
   Bool_t          L1_MU0_COMM;
   Bool_t          L1_MU0_EM3;
   Bool_t          L1_MU0_J10;
   Bool_t          L1_MU0_J15;
   Bool_t          L1_MU0_J30;
   Bool_t          L1_MU0_J5;
   Bool_t          L1_MU0_J55;
   Bool_t          L1_MU0_MV;
   Bool_t          L1_MU10;
   Bool_t          L1_MU10_J5;
   Bool_t          L1_MU15;
   Bool_t          L1_MU15_XE10;
   Bool_t          L1_MU20;
   Bool_t          L1_MU20_XE15;
   Bool_t          L1_MU6;
   Bool_t          L1_MU6_EM3;
   Bool_t          L1_MU6_J5;
   Bool_t          L2_2e12_tight;
   Bool_t          L2_2e15_medium;
   Bool_t          L2_2e20_loose;
   Bool_t          L2_2e5_medium;
   Bool_t          L2_2e5_medium1;
   Bool_t          L2_2g10_loose;
   Bool_t          L2_2g10_loose_mu6;
   Bool_t          L2_2g17i_tight;
   Bool_t          L2_2g20_loose;
   Bool_t          L2_2mu10;
   Bool_t          L2_2mu20;
   Bool_t          L2_2mu4;
   Bool_t          L2_2mu4_Bmumu;
   Bool_t          L2_2mu4_Bmumux;
   Bool_t          L2_2mu4_DiMu;
   Bool_t          L2_2mu4_DiMu_SiTrk;
   Bool_t          L2_2mu4_DiMu_noVtx;
   Bool_t          L2_2mu4_DiMu_noVtx_noOS;
   Bool_t          L2_2mu4_Jpsimumu;
   Bool_t          L2_2mu4_Upsimumu;
   Bool_t          L2_2mu6;
   Bool_t          L2_2mu6_Bmumu;
   Bool_t          L2_2mu6_Bmumux;
   Bool_t          L2_2mu6_DiMu;
   Bool_t          L2_2mu6_Jpsimumu;
   Bool_t          L2_2mu6_Upsimumu;
   Bool_t          L2_2mu6_e10_loose;
   Bool_t          L2_2mu6_g10_loose;
   Bool_t          L2_e105_loose1;
   Bool_t          L2_e10_NoCut;
   Bool_t          L2_e10_loose;
   Bool_t          L2_e10_loose_IdScan;
   Bool_t          L2_e10_loose_SiTrk;
   Bool_t          L2_e10_loose_TRT;
   Bool_t          L2_e10_loose_mu6;
   Bool_t          L2_e10_medium;
   Bool_t          L2_e10_medium_SiTrk;
   Bool_t          L2_e10_medium_SiTrk_robust;
   Bool_t          L2_e12_medium;
   Bool_t          L2_e15_medium;
   Bool_t          L2_e15i_medium;
   Bool_t          L2_e20_loose;
   Bool_t          L2_e20_loose_IdScan;
   Bool_t          L2_e20_loose_NoIDTrkCut;
   Bool_t          L2_e20_loose_SiTrk;
   Bool_t          L2_e20_loose_SiTrk_robust;
   Bool_t          L2_e20_loose_TRT;
   Bool_t          L2_e20_loose_passEF;
   Bool_t          L2_e20_loose_passL2;
   Bool_t          L2_e20_loose_xe20;
   Bool_t          L2_e20_loose_xe30;
   Bool_t          L2_e20_medium;
   Bool_t          L2_e20i_loose;
   Bool_t          L2_e22i_tight;
   Bool_t          L2_e25_loose;
   Bool_t          L2_e25_medium;
   Bool_t          L2_e25i_loose;
   Bool_t          L2_e25i_medium;
   Bool_t          L2_e55_loose1;
   Bool_t          L2_e5_medium;
   Bool_t          L2_e5_medium1;
   Bool_t          L2_e5_medium_L2SW;
   Bool_t          L2_e5_medium_MV;
   Bool_t          L2_e5_medium_Ringer;
   Bool_t          L2_e5_medium_e10_loose;
   Bool_t          L2_em105_passHLT;
   Bool_t          L2_em15_passHLT;
   Bool_t          L2_em15i_passHLT;
   Bool_t          L2_em20_passHLT;
   Bool_t          L2_em20i_passHLT;
   Bool_t          L2_g105;
   Bool_t          L2_g10_loose;
   Bool_t          L2_g10_nocut;
   Bool_t          L2_g150;
   Bool_t          L2_g15_loose;
   Bool_t          L2_g20_loose;
   Bool_t          L2_g20_loose_cnv;
   Bool_t          L2_g20_loose_xe20;
   Bool_t          L2_g20i_loose;
   Bool_t          L2_g25_loose;
   Bool_t          L2_g25_loose_xe30;
   Bool_t          L2_g25i_loose;
   Bool_t          L2_g55_tight;
   Bool_t          L2_mu10;
   Bool_t          L2_mu10_DsPhiPi;
   Bool_t          L2_mu10_DsPhiPi_FS;
   Bool_t          L2_mu10_MG;
   Bool_t          L2_mu10_MSonly;
   Bool_t          L2_mu10_MuTagIMO;
   Bool_t          L2_mu10_NoIDTrkCut;
   Bool_t          L2_mu10_SiTrk;
   Bool_t          L2_mu10i_loose;
   Bool_t          L2_mu13;
   Bool_t          L2_mu15;
   Bool_t          L2_mu15_xe20;
   Bool_t          L2_mu15i_loose;
   Bool_t          L2_mu20;
   Bool_t          L2_mu20_MSonly;
   Bool_t          L2_mu20_passHLT;
   Bool_t          L2_mu20_slow;
   Bool_t          L2_mu20_xe30;
   Bool_t          L2_mu20i_loose;
   Bool_t          L2_mu20i_loose_MSonly;
   Bool_t          L2_mu4;
   Bool_t          L2_mu4_Bmumu;
   Bool_t          L2_mu4_BmumuX;
   Bool_t          L2_mu4_Bmumu_FS;
   Bool_t          L2_mu4_DiMu;
   Bool_t          L2_mu4_DiMu_FS;
   Bool_t          L2_mu4_DiMu_FS_noOS;
   Bool_t          L2_mu4_DiMu_MG;
   Bool_t          L2_mu4_DiMu_MG_FS;
   Bool_t          L2_mu4_DiMu_SiTrk;
   Bool_t          L2_mu4_DiMu_SiTrk_FS;
   Bool_t          L2_mu4_DiMu_noOS;
   Bool_t          L2_mu4_DsPhiPi;
   Bool_t          L2_mu4_DsPhiPi_FS;
   Bool_t          L2_mu4_Jpsie5e3;
   Bool_t          L2_mu4_Jpsie5e3_FS;
   Bool_t          L2_mu4_Jpsie5e3_SiTrk;
   Bool_t          L2_mu4_Jpsie5e3_SiTrk_FS;
   Bool_t          L2_mu4_Jpsimumu;
   Bool_t          L2_mu4_Jpsimumu_FS;
   Bool_t          L2_mu4_MG;
   Bool_t          L2_mu4_MSonly;
   Bool_t          L2_mu4_MV;
   Bool_t          L2_mu4_SiTrk;
   Bool_t          L2_mu4_Trk_Jpsi;
   Bool_t          L2_mu4_Trk_Jpsi_FS;
   Bool_t          L2_mu4_Trk_Jpsi_loose;
   Bool_t          L2_mu4_Trk_Upsi_FS;
   Bool_t          L2_mu4_Trk_Upsi_loose_FS;
   Bool_t          L2_mu4_Upsimumu;
   Bool_t          L2_mu4_Upsimumu_FS;
   Bool_t          L2_mu4_j10;
   Bool_t          L2_mu4_j10_matched;
   Bool_t          L2_mu4_j15;
   Bool_t          L2_mu4_j20_matched;
   Bool_t          L2_mu4_j40_matched;
   Bool_t          L2_mu4_j70_matched;
   Bool_t          L2_mu4_mu6;
   Bool_t          L2_mu4_tile;
   Bool_t          L2_mu4_trod;
   Bool_t          L2_mu4mu6_Bmumu;
   Bool_t          L2_mu4mu6_DiMu;
   Bool_t          L2_mu4mu6_Jpsimumu;
   Bool_t          L2_mu4mu6_Upsimumu;
   Bool_t          L2_mu6;
   Bool_t          L2_mu6_Bmumu;
   Bool_t          L2_mu6_BmumuX;
   Bool_t          L2_mu6_Bmumu_FS;
   Bool_t          L2_mu6_DiMu;
   Bool_t          L2_mu6_DiMu_FS;
   Bool_t          L2_mu6_DsPhiPi;
   Bool_t          L2_mu6_DsPhiPi_FS;
   Bool_t          L2_mu6_Jpsie5e3;
   Bool_t          L2_mu6_Jpsie5e3_FS;
   Bool_t          L2_mu6_Jpsie5e3_SiTrk;
   Bool_t          L2_mu6_Jpsie5e3_SiTrk_FS;
   Bool_t          L2_mu6_Jpsimumu;
   Bool_t          L2_mu6_Jpsimumu_FS;
   Bool_t          L2_mu6_MG;
   Bool_t          L2_mu6_MSonly;
   Bool_t          L2_mu6_Trk_Jpsi;
   Bool_t          L2_mu6_Upsimumu_FS;
   Int_t           ph_n;
   vector<float>   *ph_E;
   vector<float>   *ph_Et;
   vector<float>   *ph_pt;
   vector<float>   *ph_m;
   vector<float>   *ph_eta;
   vector<float>   *ph_phi;
   vector<float>   *ph_px;
   vector<float>   *ph_py;
   vector<float>   *ph_pz;
   vector<int>     *ph_author;
   vector<int>     *ph_isRecovered;
   vector<unsigned int> *ph_isEM;
   vector<int>     *ph_convFlag;
   vector<int>     *ph_isConv;
   vector<int>     *ph_nConv;
   vector<int>     *ph_nSingleTrackConv;
   vector<int>     *ph_nDoubleTrackConv;
   vector<int>     *ph_type;
   vector<int>     *ph_origin;
   vector<float>   *ph_truth_deltaRRecPhoton;
   vector<float>   *ph_truth_E;
   vector<float>   *ph_truth_pt;
   vector<float>   *ph_truth_eta;
   vector<float>   *ph_truth_phi;
   vector<int>     *ph_truth_type;
   vector<int>     *ph_truth_status;
   vector<int>     *ph_truth_barcode;
   vector<int>     *ph_truth_mothertype;
   vector<int>     *ph_truth_motherbarcode;
   vector<int>     *ph_truth_matched;
   vector<int>     *ph_loose;
   vector<int>     *ph_tight;
   vector<int>     *ph_tightIso;
   vector<float>   *ph_Ethad;
   vector<float>   *ph_Ethad1;
   vector<float>   *ph_E033;
   vector<float>   *ph_f1;
   vector<float>   *ph_f1core;
   vector<float>   *ph_Emins1;
   vector<float>   *ph_fside;
   vector<float>   *ph_Emax2;
   vector<float>   *ph_ws3;
   vector<float>   *ph_wstot;
   vector<float>   *ph_E132;
   vector<float>   *ph_E1152;
   vector<float>   *ph_emaxs1;
   vector<float>   *ph_deltaEs;
   vector<float>   *ph_E233;
   vector<float>   *ph_E237;
   vector<float>   *ph_E277;
   vector<float>   *ph_weta2;
   vector<float>   *ph_f3;
   vector<float>   *ph_f3core;
   vector<float>   *ph_rphiallcalo;
   vector<float>   *ph_Etcone45;
   vector<float>   *ph_Etcone20;
   vector<float>   *ph_Etcone30;
   vector<float>   *ph_Etcone40;
   vector<float>   *ph_ptcone30;
   vector<float>   *ph_convanglematch;
   vector<float>   *ph_convtrackmatch;
   vector<int>     *ph_hasconv;
   vector<float>   *ph_convvtxx;
   vector<float>   *ph_convvtxy;
   vector<float>   *ph_convvtxz;
   vector<float>   *ph_Rconv;
   vector<float>   *ph_zconv;
   vector<float>   *ph_convvtxchi2;
   vector<float>   *ph_pt1conv;
   vector<int>     *ph_convtrk1nBLHits;
   vector<int>     *ph_convtrk1nPixHits;
   vector<int>     *ph_convtrk1nSCTHits;
   vector<int>     *ph_convtrk1nTRTHits;
   vector<float>   *ph_pt2conv;
   vector<int>     *ph_convtrk2nBLHits;
   vector<int>     *ph_convtrk2nPixHits;
   vector<int>     *ph_convtrk2nSCTHits;
   vector<int>     *ph_convtrk2nTRTHits;
   vector<float>   *ph_ptconv;
   vector<float>   *ph_pzconv;
   vector<float>   *ph_reta;
   vector<float>   *ph_rphi;
   vector<float>   *ph_EtringnoisedR03sig2;
   vector<float>   *ph_EtringnoisedR03sig3;
   vector<float>   *ph_EtringnoisedR03sig4;
   vector<double>  *ph_isolationlikelihoodjets;
   vector<double>  *ph_isolationlikelihoodhqelectrons;
   vector<double>  *ph_loglikelihood;
   vector<double>  *ph_photonweight;
   vector<double>  *ph_photonbgweight;
   vector<double>  *ph_neuralnet;
   vector<double>  *ph_Hmatrix;
   vector<double>  *ph_Hmatrix5;
   vector<double>  *ph_adaboost;
   vector<float>   *ph_zvertex;
   vector<float>   *ph_errz;
   vector<float>   *ph_etap;
   vector<float>   *ph_depth;
   vector<float>   *ph_cl_E;
   vector<float>   *ph_cl_pt;
   vector<float>   *ph_cl_eta;
   vector<float>   *ph_cl_phi;
   vector<float>   *ph_Es0;
   vector<float>   *ph_etas0;
   vector<float>   *ph_phis0;
   vector<float>   *ph_Es1;
   vector<float>   *ph_etas1;
   vector<float>   *ph_phis1;
   vector<float>   *ph_Es2;
   vector<float>   *ph_etas2;
   vector<float>   *ph_phis2;
   vector<float>   *ph_Es3;
   vector<float>   *ph_etas3;
   vector<float>   *ph_phis3;
   vector<float>   *ph_rawcl_Es0;
   vector<float>   *ph_rawcl_etas0;
   vector<float>   *ph_rawcl_phis0;
   vector<float>   *ph_rawcl_Es1;
   vector<float>   *ph_rawcl_etas1;
   vector<float>   *ph_rawcl_phis1;
   vector<float>   *ph_rawcl_Es2;
   vector<float>   *ph_rawcl_etas2;
   vector<float>   *ph_rawcl_phis2;
   vector<float>   *ph_rawcl_Es3;
   vector<float>   *ph_rawcl_etas3;
   vector<float>   *ph_rawcl_phis3;
   vector<float>   *ph_rawcl_E;
   vector<float>   *ph_rawcl_pt;
   vector<float>   *ph_rawcl_eta;
   vector<float>   *ph_rawcl_phi;
   vector<int>     *ph_truth_isConv;
   vector<int>     *ph_truth_isBrem;
   vector<int>     *ph_truth_isFromHardProc;
   vector<int>     *ph_truth_isPhotonFromHardProc;
   vector<float>   *ph_truth_Rconv;
   vector<float>   *ph_truth_zconv;
   vector<float>   *ph_deltaEmax2;
   vector<float>   *ph_calibHitsShowerDepth;
   vector<unsigned int> *ph_isIso;
   vector<float>   *ph_mvaptcone20;
   vector<float>   *ph_mvaptcone30;
   vector<float>   *ph_mvaptcone40;
   vector<float>   *ph_topoEtcone20;
   vector<float>   *ph_topoEtcone40;
   vector<float>   *ph_topoEtcone60;
   vector<float>   *ph_jet_dr;
   vector<float>   *ph_jet_E;
   vector<float>   *ph_jet_pt;
   vector<float>   *ph_jet_m;
   vector<float>   *ph_jet_eta;
   vector<float>   *ph_jet_phi;
   vector<float>   *ph_jet_truth_dr;
   vector<float>   *ph_jet_truth_E;
   vector<float>   *ph_jet_truth_pt;
   vector<float>   *ph_jet_truth_m;
   vector<float>   *ph_jet_truth_eta;
   vector<float>   *ph_jet_truth_phi;
   vector<int>     *ph_jet_truth_matched;
   vector<int>     *ph_jet_matched;
   vector<float>   *ph_convIP;
   vector<float>   *ph_convIPRev;
   vector<float>   *ph_ptIsolationCone;
   vector<float>   *ph_ptIsolationConePhAngle;
   vector<float>   *ph_Etcone40_pt_corrected;
   vector<float>   *ph_Etcone40_ED_corrected;
   vector<float>   *ph_Etcone40_corrected;
   vector<float>   *ph_topodr;
   vector<float>   *ph_topopt;
   vector<float>   *ph_topoeta;
   vector<float>   *ph_topophi;
   vector<int>     *ph_topomatched;
   vector<float>   *ph_topoEMdr;
   vector<float>   *ph_topoEMpt;
   vector<float>   *ph_topoEMeta;
   vector<float>   *ph_topoEMphi;
   vector<int>     *ph_topoEMmatched;
   vector<float>   *ph_EF_dr;
   vector<int>     *ph_EF_index;
   vector<float>   *ph_L2_dr;
   vector<int>     *ph_L2_index;
   vector<float>   *ph_L1_dr;
   vector<int>     *ph_L1_index;
   Int_t           mu_n;
   vector<float>   *mu_E;
   vector<float>   *mu_pt;
   vector<float>   *mu_m;
   vector<float>   *mu_eta;
   vector<float>   *mu_phi;
   vector<float>   *mu_px;
   vector<float>   *mu_py;
   vector<float>   *mu_pz;
   vector<float>   *mu_charge;
   vector<unsigned short> *mu_allauthor;
   vector<int>     *mu_author;
   vector<float>   *mu_matchchi2;
   vector<int>     *mu_matchndof;
   vector<float>   *mu_etcone20;
   vector<float>   *mu_etcone30;
   vector<float>   *mu_etcone40;
   vector<float>   *mu_nucone20;
   vector<float>   *mu_nucone30;
   vector<float>   *mu_nucone40;
   vector<float>   *mu_ptcone20;
   vector<float>   *mu_ptcone30;
   vector<float>   *mu_ptcone40;
   vector<float>   *mu_energyLossPar;
   vector<float>   *mu_energyLossErr;
   vector<float>   *mu_etCore;
   vector<float>   *mu_energyLossType;
   vector<int>     *mu_bestMatch;
   vector<int>     *mu_isStandAloneMuon;
   vector<int>     *mu_isCombinedMuon;
   vector<int>     *mu_isLowPtReconstructedMuon;
   vector<int>     *mu_loose;
   vector<int>     *mu_medium;
   vector<int>     *mu_tight;
   vector<float>   *mu_d0_exPV;
   vector<float>   *mu_z0_exPV;
   vector<float>   *mu_phi_exPV;
   vector<float>   *mu_theta_exPV;
   vector<float>   *mu_qoverp_exPV;
   vector<float>   *mu_cb_d0_exPV;
   vector<float>   *mu_cb_z0_exPV;
   vector<float>   *mu_cb_phi_exPV;
   vector<float>   *mu_cb_theta_exPV;
   vector<float>   *mu_cb_qoverp_exPV;
   vector<float>   *mu_id_d0_exPV;
   vector<float>   *mu_id_z0_exPV;
   vector<float>   *mu_id_phi_exPV;
   vector<float>   *mu_id_theta_exPV;
   vector<float>   *mu_id_qoverp_exPV;
   vector<float>   *mu_me_d0_exPV;
   vector<float>   *mu_me_z0_exPV;
   vector<float>   *mu_me_phi_exPV;
   vector<float>   *mu_me_theta_exPV;
   vector<float>   *mu_me_qoverp_exPV;
   vector<float>   *mu_ie_d0_exPV;
   vector<float>   *mu_ie_z0_exPV;
   vector<float>   *mu_ie_phi_exPV;
   vector<float>   *mu_ie_theta_exPV;
   vector<float>   *mu_ie_qoverp_exPV;
   vector<float>   *mu_cov_d0_exPV;
   vector<float>   *mu_cov_z0_exPV;
   vector<float>   *mu_cov_phi_exPV;
   vector<float>   *mu_cov_theta_exPV;
   vector<float>   *mu_cov_qoverp_exPV;
   vector<float>   *mu_cov_d0_z0_exPV;
   vector<float>   *mu_cov_d0_phi_exPV;
   vector<float>   *mu_cov_d0_theta_exPV;
   vector<float>   *mu_cov_d0_qoverp_exPV;
   vector<float>   *mu_cov_z0_phi_exPV;
   vector<float>   *mu_cov_z0_theta_exPV;
   vector<float>   *mu_cov_z0_qoverp_exPV;
   vector<float>   *mu_cov_phi_theta_exPV;
   vector<float>   *mu_cov_phi_qoverp_exPV;
   vector<float>   *mu_cov_theta_qoverp_exPV;
   vector<float>   *mu_ms_d0;
   vector<float>   *mu_ms_z0;
   vector<float>   *mu_ms_phi;
   vector<float>   *mu_ms_theta;
   vector<float>   *mu_ms_qoverp;
   vector<float>   *mu_id_d0;
   vector<float>   *mu_id_z0;
   vector<float>   *mu_id_phi;
   vector<float>   *mu_id_theta;
   vector<float>   *mu_id_qoverp;
   vector<float>   *mu_me_d0;
   vector<float>   *mu_me_z0;
   vector<float>   *mu_me_phi;
   vector<float>   *mu_me_theta;
   vector<float>   *mu_me_qoverp;
   vector<float>   *mu_ie_d0;
   vector<float>   *mu_ie_z0;
   vector<float>   *mu_ie_phi;
   vector<float>   *mu_ie_theta;
   vector<float>   *mu_ie_qoverp;
   vector<int>     *mu_nBLHits;
   vector<int>     *mu_nPixHits;
   vector<int>     *mu_nSCTHits;
   vector<int>     *mu_nTRTHits;
   vector<int>     *mu_nTRTHighTHits;
   vector<int>     *mu_nBLSharedHits;
   vector<int>     *mu_nPixSharedHits;
   vector<int>     *mu_nPixHoles;
   vector<int>     *mu_nSCTSharedHits;
   vector<int>     *mu_nSCTHoles;
   vector<int>     *mu_nTRTOutliers;
   vector<int>     *mu_nTRTHighTOutliers;
   vector<int>     *mu_nMDTHits;
   vector<int>     *mu_nMDTHoles;
   vector<int>     *mu_nCSCEtaHits;
   vector<int>     *mu_nCSCEtaHoles;
   vector<int>     *mu_nCSCPhiHits;
   vector<int>     *mu_nCSCPhiHoles;
   vector<int>     *mu_nRPCEtaHits;
   vector<int>     *mu_nRPCEtaHoles;
   vector<int>     *mu_nRPCPhiHits;
   vector<int>     *mu_nRPCPhiHoles;
   vector<int>     *mu_nTGCEtaHits;
   vector<int>     *mu_nTGCEtaHoles;
   vector<int>     *mu_nTGCPhiHits;
   vector<int>     *mu_nTGCPhiHoles;
   vector<int>     *mu_nGangedPixels;
   vector<int>     *mu_nOutliersOnTrack;
   vector<int>     *mu_nMDTBIHits;
   vector<int>     *mu_nMDTBMHits;
   vector<int>     *mu_nMDTBOHits;
   vector<int>     *mu_nMDTBEEHits;
   vector<int>     *mu_nMDTBIS78Hits;
   vector<int>     *mu_nMDTEIHits;
   vector<int>     *mu_nMDTEMHits;
   vector<int>     *mu_nMDTEOHits;
   vector<int>     *mu_nMDTEEHits;
   vector<int>     *mu_nRPCLayer1EtaHits;
   vector<int>     *mu_nRPCLayer2EtaHits;
   vector<int>     *mu_nRPCLayer3EtaHits;
   vector<int>     *mu_nRPCLayer1PhiHits;
   vector<int>     *mu_nRPCLayer2PhiHits;
   vector<int>     *mu_nRPCLayer3PhiHits;
   vector<int>     *mu_nTGCLayer1EtaHits;
   vector<int>     *mu_nTGCLayer2EtaHits;
   vector<int>     *mu_nTGCLayer3EtaHits;
   vector<int>     *mu_nTGCLayer4EtaHits;
   vector<int>     *mu_nTGCLayer1PhiHits;
   vector<int>     *mu_nTGCLayer2PhiHits;
   vector<int>     *mu_nTGCLayer3PhiHits;
   vector<int>     *mu_nTGCLayer4PhiHits;
   vector<int>     *mu_barrelSectors;
   vector<int>     *mu_endcapSectors;
   vector<float>   *mu_trackd0;
   vector<float>   *mu_trackz0;
   vector<float>   *mu_trackphi;
   vector<float>   *mu_tracktheta;
   vector<float>   *mu_trackqoverp;
   vector<float>   *mu_trackcov_d0;
   vector<float>   *mu_trackcov_z0;
   vector<float>   *mu_trackcov_phi;
   vector<float>   *mu_trackcov_theta;
   vector<float>   *mu_trackcov_qoverp;
   vector<float>   *mu_trackcov_d0_z0;
   vector<float>   *mu_trackcov_d0_phi;
   vector<float>   *mu_trackcov_d0_theta;
   vector<float>   *mu_trackcov_d0_qoverp;
   vector<float>   *mu_trackcov_z0_phi;
   vector<float>   *mu_trackcov_z0_theta;
   vector<float>   *mu_trackcov_z0_qoverp;
   vector<float>   *mu_trackcov_phi_theta;
   vector<float>   *mu_trackcov_phi_qoverp;
   vector<float>   *mu_trackcov_theta_qoverp;
   vector<float>   *mu_trackfitchi2;
   vector<int>     *mu_trackfitndof;
   vector<int>     *mu_hastrack;
   vector<float>   *mu_truth_dr;
   vector<float>   *mu_truth_E;
   vector<float>   *mu_truth_pt;
   vector<float>   *mu_truth_eta;
   vector<float>   *mu_truth_phi;
   vector<int>     *mu_truth_type;
   vector<int>     *mu_truth_status;
   vector<int>     *mu_truth_barcode;
   vector<int>     *mu_truth_mothertype;
   vector<int>     *mu_truth_motherbarcode;
   vector<int>     *mu_truth_matched;
   vector<float>   *mu_EF_dr;
   vector<float>   *mu_EF_cb_eta;
   vector<float>   *mu_EF_cb_phi;
   vector<float>   *mu_EF_cb_pt;
   vector<float>   *mu_EF_ms_eta;
   vector<float>   *mu_EF_ms_phi;
   vector<float>   *mu_EF_ms_pt;
   vector<float>   *mu_EF_me_eta;
   vector<float>   *mu_EF_me_phi;
   vector<float>   *mu_EF_me_pt;
   vector<int>     *mu_EF_matched;
   vector<float>   *mu_L2CB_dr;
   vector<float>   *mu_L2CB_pt;
   vector<float>   *mu_L2CB_eta;
   vector<float>   *mu_L2CB_phi;
   vector<float>   *mu_L2CB_id_pt;
   vector<float>   *mu_L2CB_ms_pt;
   vector<int>     *mu_L2CB_nPixHits;
   vector<int>     *mu_L2CB_nSCTHits;
   vector<int>     *mu_L2CB_nTRTHits;
   vector<int>     *mu_L2CB_nTRTHighTHits;
   vector<int>     *mu_L2CB_matched;
   vector<float>   *mu_L1_dr;
   vector<float>   *mu_L1_pt;
   vector<float>   *mu_L1_eta;
   vector<float>   *mu_L1_phi;
   vector<short>   *mu_L1_thrNumber;
   vector<short>   *mu_L1_RoINumber;
   vector<short>   *mu_L1_sectorAddress;
   vector<int>     *mu_L1_firstCandidate;
   vector<int>     *mu_L1_moreCandInRoI;
   vector<int>     *mu_L1_moreCandInSector;
   vector<short>   *mu_L1_source;
   vector<short>   *mu_L1_hemisphere;
   vector<int>     *mu_L1_matched;
   Int_t           trig_L1_emtau_n;
   vector<float>   *trig_L1_emtau_eta;
   vector<float>   *trig_L1_emtau_phi;
   vector<vector<string> > *trig_L1_emtau_thrNames;
   vector<vector<float> > *trig_L1_emtau_thrValues;
   vector<float>   *trig_L1_emtau_core;
   vector<float>   *trig_L1_emtau_EMClus;
   vector<float>   *trig_L1_emtau_tauClus;
   vector<float>   *trig_L1_emtau_EMIsol;
   vector<float>   *trig_L1_emtau_hadIsol;
   vector<float>   *trig_L1_emtau_hadCore;
   vector<unsigned int> *trig_L1_emtau_thrPattern;
   vector<int>     *trig_L1_emtau_L1_2EM10;
   vector<int>     *trig_L1_emtau_L1_2EM10I;
   vector<int>     *trig_L1_emtau_L1_2EM14;
   vector<int>     *trig_L1_emtau_L1_2EM3;
   vector<int>     *trig_L1_emtau_L1_2EM3_EM5;
   vector<int>     *trig_L1_emtau_L1_2EM5;
   vector<int>     *trig_L1_emtau_L1_2EM5_MU6;
   vector<int>     *trig_L1_emtau_L1_EM10;
   vector<int>     *trig_L1_emtau_L1_EM10I;
   vector<int>     *trig_L1_emtau_L1_EM14;
   vector<int>     *trig_L1_emtau_L1_EM14I;
   vector<int>     *trig_L1_emtau_L1_EM14_XE10;
   vector<int>     *trig_L1_emtau_L1_EM14_XE15;
   vector<int>     *trig_L1_emtau_L1_EM19I;
   vector<int>     *trig_L1_emtau_L1_EM3;
   vector<int>     *trig_L1_emtau_L1_EM3_MV;
   vector<int>     *trig_L1_emtau_L1_EM5;
   vector<int>     *trig_L1_emtau_L1_EM5_MU6;
   vector<int>     *trig_L1_emtau_L1_EM85;
   vector<unsigned int> *trig_L1_emtau_RoIWord;
   Int_t           trig_L2_emcl_n;
   vector<unsigned int> *trig_L2_emcl_quality;
   vector<float>   *trig_L2_emcl_E;
   vector<float>   *trig_L2_emcl_Et;
   vector<float>   *trig_L2_emcl_eta;
   vector<float>   *trig_L2_emcl_phi;
   vector<float>   *trig_L2_emcl_E237;
   vector<float>   *trig_L2_emcl_E277;
   vector<float>   *trig_L2_emcl_fracs1;
   vector<float>   *trig_L2_emcl_weta2;
   vector<float>   *trig_L2_emcl_Ehad1;
   vector<float>   *trig_L2_emcl_eta1;
   vector<float>   *trig_L2_emcl_emaxs1;
   vector<float>   *trig_L2_emcl_e2tsts1;
   vector<float>   *trig_L2_emcl_rawEnergy;
   vector<float>   *trig_L2_emcl_rawEta;
   vector<float>   *trig_L2_emcl_rawPhi;
   vector<long>    *trig_L2_emcl_RoIWord;
   vector<int>     *trig_L2_emcl_nCells;
   vector<vector<float> > *trig_L2_emcl_energyInSample;
   Int_t           trig_L2_trk_idscan_eGamma_n;
   vector<int>     *trig_L2_trk_idscan_eGamma_algorithmId;
   vector<short>   *trig_L2_trk_idscan_eGamma_trackStatus;
   vector<float>   *trig_L2_trk_idscan_eGamma_chi2Ndof;
   vector<int>     *trig_L2_trk_idscan_eGamma_nStrawHits;
   vector<int>     *trig_L2_trk_idscan_eGamma_nHighThrHits;
   vector<int>     *trig_L2_trk_idscan_eGamma_nPixelSpacePoints;
   vector<int>     *trig_L2_trk_idscan_eGamma_nSCT_SpacePoints;
   vector<long>    *trig_L2_trk_idscan_eGamma_hitPattern;
   vector<int>     *trig_L2_trk_idscan_eGamma_nStraw;
   vector<int>     *trig_L2_trk_idscan_eGamma_nStrawTime;
   vector<float>   *trig_L2_trk_idscan_eGamma_a0;
   vector<float>   *trig_L2_trk_idscan_eGamma_z0;
   vector<float>   *trig_L2_trk_idscan_eGamma_phi0;
   vector<float>   *trig_L2_trk_idscan_eGamma_eta;
   vector<float>   *trig_L2_trk_idscan_eGamma_pt;
   vector<vector<float> > *trig_L2_trk_idscan_eGamma_covariance;
   Int_t           trig_L2_trk_sitrack_eGamma_n;
   vector<int>     *trig_L2_trk_sitrack_eGamma_algorithmId;
   vector<short>   *trig_L2_trk_sitrack_eGamma_trackStatus;
   vector<float>   *trig_L2_trk_sitrack_eGamma_chi2Ndof;
   vector<int>     *trig_L2_trk_sitrack_eGamma_nStrawHits;
   vector<int>     *trig_L2_trk_sitrack_eGamma_nHighThrHits;
   vector<int>     *trig_L2_trk_sitrack_eGamma_nPixelSpacePoints;
   vector<int>     *trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints;
   vector<long>    *trig_L2_trk_sitrack_eGamma_hitPattern;
   vector<int>     *trig_L2_trk_sitrack_eGamma_nStraw;
   vector<int>     *trig_L2_trk_sitrack_eGamma_nStrawTime;
   vector<float>   *trig_L2_trk_sitrack_eGamma_a0;
   vector<float>   *trig_L2_trk_sitrack_eGamma_z0;
   vector<float>   *trig_L2_trk_sitrack_eGamma_phi0;
   vector<float>   *trig_L2_trk_sitrack_eGamma_eta;
   vector<float>   *trig_L2_trk_sitrack_eGamma_pt;
   vector<vector<float> > *trig_L2_trk_sitrack_eGamma_covariance;
   Int_t           trig_L2_el_n;
   vector<float>   *trig_L2_el_E;
   vector<float>   *trig_L2_el_Et;
   vector<float>   *trig_L2_el_pt;
   vector<float>   *trig_L2_el_eta;
   vector<float>   *trig_L2_el_phi;
   vector<int>     *trig_L2_el_RoIWord;
   vector<float>   *trig_L2_el_zvertex;
   vector<int>     *trig_L2_el_charge;
   vector<int>     *trig_L2_el_trackAlgo;
   vector<float>   *trig_L2_el_TRTHighTHitsRatio;
   vector<float>   *trig_L2_el_deltaeta1;
   vector<float>   *trig_L2_el_deltaphi2;
   vector<float>   *trig_L2_el_EtOverPt;
   vector<float>   *trig_L2_el_reta;
   vector<float>   *trig_L2_el_Eratio;
   vector<float>   *trig_L2_el_Ethad1;
   vector<int>     *trig_L2_el_L2_2e12_tight;
   vector<int>     *trig_L2_el_L2_2e15_medium;
   vector<int>     *trig_L2_el_L2_2e20_loose;
   vector<int>     *trig_L2_el_L2_2e5_medium;
   vector<int>     *trig_L2_el_L2_2e5_medium1;
   vector<int>     *trig_L2_el_L2_e105_loose1;
   vector<int>     *trig_L2_el_L2_e10_NoCut;
   vector<int>     *trig_L2_el_L2_e10_loose;
   vector<int>     *trig_L2_el_L2_e10_loose_IdScan;
   vector<int>     *trig_L2_el_L2_e10_loose_SiTrk;
   vector<int>     *trig_L2_el_L2_e10_loose_TRT;
   vector<int>     *trig_L2_el_L2_e10_loose_mu6;
   vector<int>     *trig_L2_el_L2_e10_medium;
   vector<int>     *trig_L2_el_L2_e10_medium_SiTrk;
   vector<int>     *trig_L2_el_L2_e10_medium_SiTrk_robust;
   vector<int>     *trig_L2_el_L2_e12_medium;
   vector<int>     *trig_L2_el_L2_e15_medium;
   vector<int>     *trig_L2_el_L2_e15i_medium;
   vector<int>     *trig_L2_el_L2_e20_loose;
   vector<int>     *trig_L2_el_L2_e20_loose_IdScan;
   vector<int>     *trig_L2_el_L2_e20_loose_NoIDTrkCut;
   vector<int>     *trig_L2_el_L2_e20_loose_SiTrk;
   vector<int>     *trig_L2_el_L2_e20_loose_SiTrk_robust;
   vector<int>     *trig_L2_el_L2_e20_loose_TRT;
   vector<int>     *trig_L2_el_L2_e20_loose_passEF;
   vector<int>     *trig_L2_el_L2_e20_loose_passL2;
   vector<int>     *trig_L2_el_L2_e20_loose_xe20;
   vector<int>     *trig_L2_el_L2_e20_loose_xe30;
   vector<int>     *trig_L2_el_L2_e20_medium;
   vector<int>     *trig_L2_el_L2_e20i_loose;
   vector<int>     *trig_L2_el_L2_e22i_tight;
   vector<int>     *trig_L2_el_L2_e25_loose;
   vector<int>     *trig_L2_el_L2_e25_medium;
   vector<int>     *trig_L2_el_L2_e25i_loose;
   vector<int>     *trig_L2_el_L2_e25i_medium;
   vector<int>     *trig_L2_el_L2_e55_loose1;
   vector<int>     *trig_L2_el_L2_e5_medium;
   vector<int>     *trig_L2_el_L2_e5_medium1;
   vector<int>     *trig_L2_el_L2_e5_medium_L2SW;
   vector<int>     *trig_L2_el_L2_e5_medium_MV;
   vector<int>     *trig_L2_el_L2_e5_medium_Ringer;
   vector<int>     *trig_L2_el_L2_e5_medium_e10_loose;
   vector<int>     *trig_L2_el_L2_em105_passHLT;
   vector<int>     *trig_L2_el_L2_em15_passHLT;
   vector<int>     *trig_L2_el_L2_em15i_passHLT;
   vector<int>     *trig_L2_el_L2_em20_passHLT;
   vector<int>     *trig_L2_el_L2_em20i_passHLT;
   vector<int>     *trig_L2_el_nTRTHits;
   vector<int>     *trig_L2_el_nTRTHighTHits;
   vector<int>     *trig_L2_el_trackIndx;
   vector<float>   *trig_L2_el_trkPt;
   vector<float>   *trig_L2_el_err_trkPt;
   vector<float>   *trig_L2_el_err_eta;
   vector<float>   *trig_L2_el_err_phi;
   vector<float>   *trig_L2_el_errz;
   vector<float>   *trig_L2_el_trkEtaAtCalo;
   vector<float>   *trig_L2_el_trkPhiAtCalo;
   vector<float>   *trig_L2_el_caloEta;
   vector<float>   *trig_L2_el_caloPhi;
   vector<float>   *trig_L2_el_F0;
   vector<float>   *trig_L2_el_F1;
   vector<float>   *trig_L2_el_F2;
   vector<float>   *trig_L2_el_F3;
   Int_t           trig_L2_ph_n;
   vector<float>   *trig_L2_ph_E;
   vector<float>   *trig_L2_ph_Et;
   vector<float>   *trig_L2_ph_pt;
   vector<float>   *trig_L2_ph_eta;
   vector<float>   *trig_L2_ph_phi;
   vector<int>     *trig_L2_ph_RoIWord;
   vector<float>   *trig_L2_ph_HadEt1;
   vector<float>   *trig_L2_ph_Eratio;
   vector<float>   *trig_L2_ph_Reta;
   vector<float>   *trig_L2_ph_dPhi;
   vector<float>   *trig_L2_ph_dEta;
   vector<float>   *trig_L2_ph_F1;
   vector<int>     *trig_L2_ph_L2_2g10_loose;
   vector<int>     *trig_L2_ph_L2_2g10_loose_mu6;
   vector<int>     *trig_L2_ph_L2_2g17i_tight;
   vector<int>     *trig_L2_ph_L2_2g20_loose;
   vector<int>     *trig_L2_ph_L2_g105;
   vector<int>     *trig_L2_ph_L2_g10_loose;
   vector<int>     *trig_L2_ph_L2_g10_nocut;
   vector<int>     *trig_L2_ph_L2_g150;
   vector<int>     *trig_L2_ph_L2_g15_loose;
   vector<int>     *trig_L2_ph_L2_g20_loose;
   vector<int>     *trig_L2_ph_L2_g20_loose_cnv;
   vector<int>     *trig_L2_ph_L2_g20_loose_xe20;
   vector<int>     *trig_L2_ph_L2_g20i_loose;
   vector<int>     *trig_L2_ph_L2_g25_loose;
   vector<int>     *trig_L2_ph_L2_g25_loose_xe30;
   vector<int>     *trig_L2_ph_L2_g25i_loose;
   vector<int>     *trig_L2_ph_L2_g55_tight;
   vector<float>   *trig_L2_ph_Fside;
   vector<float>   *trig_L2_ph_Weta2;
   vector<float>   *trig_L2_ph_F0;
   vector<float>   *trig_L2_ph_F2;
   vector<float>   *trig_L2_ph_F3;
   Int_t           trig_EF_emcl_n;
   vector<float>   *trig_EF_emcl_E;
   vector<float>   *trig_EF_emcl_pt;
   vector<float>   *trig_EF_emcl_m;
   vector<float>   *trig_EF_emcl_eta;
   vector<float>   *trig_EF_emcl_phi;
   vector<float>   *trig_EF_emcl_E_em;
   vector<float>   *trig_EF_emcl_E_had;
   vector<float>   *trig_EF_emcl_firstEdens;
   vector<float>   *trig_EF_emcl_cellmaxfrac;
   vector<float>   *trig_EF_emcl_longitudinal;
   vector<float>   *trig_EF_emcl_secondlambda;
   vector<float>   *trig_EF_emcl_lateral;
   vector<float>   *trig_EF_emcl_secondR;
   vector<float>   *trig_EF_emcl_centerlambda;
   vector<float>   *trig_EF_emcl_deltaTheta;
   vector<float>   *trig_EF_emcl_deltaPhi;
   vector<float>   *trig_EF_emcl_time;
   vector<float>   *trig_EF_emcl_E_PreSamplerB;
   vector<float>   *trig_EF_emcl_E_EMB1;
   vector<float>   *trig_EF_emcl_E_EMB2;
   vector<float>   *trig_EF_emcl_E_EMB3;
   vector<float>   *trig_EF_emcl_E_PreSamplerE;
   vector<float>   *trig_EF_emcl_E_EME1;
   vector<float>   *trig_EF_emcl_E_EME2;
   vector<float>   *trig_EF_emcl_E_EME3;
   vector<float>   *trig_EF_emcl_E_HEC0;
   vector<float>   *trig_EF_emcl_E_HEC1;
   vector<float>   *trig_EF_emcl_E_HEC2;
   vector<float>   *trig_EF_emcl_E_HEC3;
   vector<float>   *trig_EF_emcl_E_TileBar0;
   vector<float>   *trig_EF_emcl_E_TileBar1;
   vector<float>   *trig_EF_emcl_E_TileBar2;
   vector<float>   *trig_EF_emcl_E_TileGap1;
   vector<float>   *trig_EF_emcl_E_TileGap2;
   vector<float>   *trig_EF_emcl_E_TileGap3;
   vector<float>   *trig_EF_emcl_E_TileExt0;
   vector<float>   *trig_EF_emcl_E_TileExt1;
   vector<float>   *trig_EF_emcl_E_TileExt2;
   vector<float>   *trig_EF_emcl_E_FCAL0;
   vector<float>   *trig_EF_emcl_E_FCAL1;
   vector<float>   *trig_EF_emcl_E_FCAL2;
   vector<float>   *trig_EF_emcl_eta_PreSamplerB;
   vector<float>   *trig_EF_emcl_phi_PreSamplerB;
   vector<float>   *trig_EF_emcl_eta_EMB1;
   vector<float>   *trig_EF_emcl_phi_EMB1;
   vector<float>   *trig_EF_emcl_eta_EMB2;
   vector<float>   *trig_EF_emcl_phi_EMB2;
   vector<float>   *trig_EF_emcl_eta_EMB3;
   vector<float>   *trig_EF_emcl_phi_EMB3;
   vector<float>   *trig_EF_emcl_eta_PreSamplerE;
   vector<float>   *trig_EF_emcl_phi_PreSamplerE;
   vector<float>   *trig_EF_emcl_eta_EME1;
   vector<float>   *trig_EF_emcl_phi_EME1;
   vector<float>   *trig_EF_emcl_eta_EME2;
   vector<float>   *trig_EF_emcl_phi_EME2;
   vector<float>   *trig_EF_emcl_eta_EME3;
   vector<float>   *trig_EF_emcl_phi_EME3;
   vector<float>   *trig_EF_emcl_eta_HEC0;
   vector<float>   *trig_EF_emcl_phi_HEC0;
   vector<float>   *trig_EF_emcl_eta_HEC1;
   vector<float>   *trig_EF_emcl_phi_HEC1;
   vector<float>   *trig_EF_emcl_eta_HEC2;
   vector<float>   *trig_EF_emcl_phi_HEC2;
   vector<float>   *trig_EF_emcl_eta_HEC3;
   vector<float>   *trig_EF_emcl_phi_HEC3;
   vector<float>   *trig_EF_emcl_eta_TileBar0;
   vector<float>   *trig_EF_emcl_phi_TileBar0;
   vector<float>   *trig_EF_emcl_eta_TileBar1;
   vector<float>   *trig_EF_emcl_phi_TileBar1;
   vector<float>   *trig_EF_emcl_eta_TileBar2;
   vector<float>   *trig_EF_emcl_phi_TileBar2;
   vector<float>   *trig_EF_emcl_eta_TileGap1;
   vector<float>   *trig_EF_emcl_phi_TileGap1;
   vector<float>   *trig_EF_emcl_eta_TileGap2;
   vector<float>   *trig_EF_emcl_phi_TileGap2;
   vector<float>   *trig_EF_emcl_eta_TileGap3;
   vector<float>   *trig_EF_emcl_phi_TileGap3;
   vector<float>   *trig_EF_emcl_eta_TileExt0;
   vector<float>   *trig_EF_emcl_phi_TileExt0;
   vector<float>   *trig_EF_emcl_eta_TileExt1;
   vector<float>   *trig_EF_emcl_phi_TileExt1;
   vector<float>   *trig_EF_emcl_eta_TileExt2;
   vector<float>   *trig_EF_emcl_phi_TileExt2;
   vector<float>   *trig_EF_emcl_eta_FCAL0;
   vector<float>   *trig_EF_emcl_phi_FCAL0;
   vector<float>   *trig_EF_emcl_eta_FCAL1;
   vector<float>   *trig_EF_emcl_phi_FCAL1;
   vector<float>   *trig_EF_emcl_eta_FCAL2;
   vector<float>   *trig_EF_emcl_phi_FCAL2;
   vector<int>     *trig_EF_emcl_cell_n;
   vector<vector<float> > *trig_EF_emcl_cell_E;
   vector<vector<float> > *trig_EF_emcl_cell_pt;
   vector<vector<float> > *trig_EF_emcl_cell_eta;
   vector<vector<float> > *trig_EF_emcl_cell_phi;
   vector<vector<int> > *trig_EF_emcl_cell_QCells;
   vector<vector<int> > *trig_EF_emcl_cell_GainCells;
   vector<vector<int> > *trig_EF_emcl_cell_DetCells;
   vector<vector<float> > *trig_EF_emcl_cell_TimeCells;
   vector<vector<float> > *trig_EF_emcl_cell_xCells;
   vector<vector<float> > *trig_EF_emcl_cell_yCells;
   vector<vector<float> > *trig_EF_emcl_cell_zCells;
   vector<vector<int> > *trig_EF_emcl_cell_BadCells;
   vector<vector<unsigned int> > *trig_EF_emcl_cell_IdCells;
   Int_t           trig_EF_emcl_slw_n;
   vector<float>   *trig_EF_emcl_slw_E;
   vector<float>   *trig_EF_emcl_slw_pt;
   vector<float>   *trig_EF_emcl_slw_m;
   vector<float>   *trig_EF_emcl_slw_eta;
   vector<float>   *trig_EF_emcl_slw_phi;
   vector<float>   *trig_EF_emcl_slw_E_em;
   vector<float>   *trig_EF_emcl_slw_E_had;
   vector<float>   *trig_EF_emcl_slw_firstEdens;
   vector<float>   *trig_EF_emcl_slw_cellmaxfrac;
   vector<float>   *trig_EF_emcl_slw_longitudinal;
   vector<float>   *trig_EF_emcl_slw_secondlambda;
   vector<float>   *trig_EF_emcl_slw_lateral;
   vector<float>   *trig_EF_emcl_slw_secondR;
   vector<float>   *trig_EF_emcl_slw_centerlambda;
   vector<float>   *trig_EF_emcl_slw_deltaTheta;
   vector<float>   *trig_EF_emcl_slw_deltaPhi;
   vector<float>   *trig_EF_emcl_slw_time;
   vector<float>   *trig_EF_emcl_slw_E_PreSamplerB;
   vector<float>   *trig_EF_emcl_slw_E_EMB1;
   vector<float>   *trig_EF_emcl_slw_E_EMB2;
   vector<float>   *trig_EF_emcl_slw_E_EMB3;
   vector<float>   *trig_EF_emcl_slw_E_PreSamplerE;
   vector<float>   *trig_EF_emcl_slw_E_EME1;
   vector<float>   *trig_EF_emcl_slw_E_EME2;
   vector<float>   *trig_EF_emcl_slw_E_EME3;
   vector<float>   *trig_EF_emcl_slw_E_HEC0;
   vector<float>   *trig_EF_emcl_slw_E_HEC1;
   vector<float>   *trig_EF_emcl_slw_E_HEC2;
   vector<float>   *trig_EF_emcl_slw_E_HEC3;
   vector<float>   *trig_EF_emcl_slw_E_TileBar0;
   vector<float>   *trig_EF_emcl_slw_E_TileBar1;
   vector<float>   *trig_EF_emcl_slw_E_TileBar2;
   vector<float>   *trig_EF_emcl_slw_E_TileGap1;
   vector<float>   *trig_EF_emcl_slw_E_TileGap2;
   vector<float>   *trig_EF_emcl_slw_E_TileGap3;
   vector<float>   *trig_EF_emcl_slw_E_TileExt0;
   vector<float>   *trig_EF_emcl_slw_E_TileExt1;
   vector<float>   *trig_EF_emcl_slw_E_TileExt2;
   vector<float>   *trig_EF_emcl_slw_E_FCAL0;
   vector<float>   *trig_EF_emcl_slw_E_FCAL1;
   vector<float>   *trig_EF_emcl_slw_E_FCAL2;
   vector<float>   *trig_EF_emcl_slw_eta_PreSamplerB;
   vector<float>   *trig_EF_emcl_slw_phi_PreSamplerB;
   vector<float>   *trig_EF_emcl_slw_eta_EMB1;
   vector<float>   *trig_EF_emcl_slw_phi_EMB1;
   vector<float>   *trig_EF_emcl_slw_eta_EMB2;
   vector<float>   *trig_EF_emcl_slw_phi_EMB2;
   vector<float>   *trig_EF_emcl_slw_eta_EMB3;
   vector<float>   *trig_EF_emcl_slw_phi_EMB3;
   vector<float>   *trig_EF_emcl_slw_eta_PreSamplerE;
   vector<float>   *trig_EF_emcl_slw_phi_PreSamplerE;
   vector<float>   *trig_EF_emcl_slw_eta_EME1;
   vector<float>   *trig_EF_emcl_slw_phi_EME1;
   vector<float>   *trig_EF_emcl_slw_eta_EME2;
   vector<float>   *trig_EF_emcl_slw_phi_EME2;
   vector<float>   *trig_EF_emcl_slw_eta_EME3;
   vector<float>   *trig_EF_emcl_slw_phi_EME3;
   vector<float>   *trig_EF_emcl_slw_eta_HEC0;
   vector<float>   *trig_EF_emcl_slw_phi_HEC0;
   vector<float>   *trig_EF_emcl_slw_eta_HEC1;
   vector<float>   *trig_EF_emcl_slw_phi_HEC1;
   vector<float>   *trig_EF_emcl_slw_eta_HEC2;
   vector<float>   *trig_EF_emcl_slw_phi_HEC2;
   vector<float>   *trig_EF_emcl_slw_eta_HEC3;
   vector<float>   *trig_EF_emcl_slw_phi_HEC3;
   vector<float>   *trig_EF_emcl_slw_eta_TileBar0;
   vector<float>   *trig_EF_emcl_slw_phi_TileBar0;
   vector<float>   *trig_EF_emcl_slw_eta_TileBar1;
   vector<float>   *trig_EF_emcl_slw_phi_TileBar1;
   vector<float>   *trig_EF_emcl_slw_eta_TileBar2;
   vector<float>   *trig_EF_emcl_slw_phi_TileBar2;
   vector<float>   *trig_EF_emcl_slw_eta_TileGap1;
   vector<float>   *trig_EF_emcl_slw_phi_TileGap1;
   vector<float>   *trig_EF_emcl_slw_eta_TileGap2;
   vector<float>   *trig_EF_emcl_slw_phi_TileGap2;
   vector<float>   *trig_EF_emcl_slw_eta_TileGap3;
   vector<float>   *trig_EF_emcl_slw_phi_TileGap3;
   vector<float>   *trig_EF_emcl_slw_eta_TileExt0;
   vector<float>   *trig_EF_emcl_slw_phi_TileExt0;
   vector<float>   *trig_EF_emcl_slw_eta_TileExt1;
   vector<float>   *trig_EF_emcl_slw_phi_TileExt1;
   vector<float>   *trig_EF_emcl_slw_eta_TileExt2;
   vector<float>   *trig_EF_emcl_slw_phi_TileExt2;
   vector<float>   *trig_EF_emcl_slw_eta_FCAL0;
   vector<float>   *trig_EF_emcl_slw_phi_FCAL0;
   vector<float>   *trig_EF_emcl_slw_eta_FCAL1;
   vector<float>   *trig_EF_emcl_slw_phi_FCAL1;
   vector<float>   *trig_EF_emcl_slw_eta_FCAL2;
   vector<float>   *trig_EF_emcl_slw_phi_FCAL2;
   vector<int>     *trig_EF_emcl_slw_cell_n;
   vector<vector<float> > *trig_EF_emcl_slw_cell_E;
   vector<vector<float> > *trig_EF_emcl_slw_cell_pt;
   vector<vector<float> > *trig_EF_emcl_slw_cell_eta;
   vector<vector<float> > *trig_EF_emcl_slw_cell_phi;
   vector<vector<int> > *trig_EF_emcl_slw_cell_QCells;
   vector<vector<int> > *trig_EF_emcl_slw_cell_GainCells;
   vector<vector<int> > *trig_EF_emcl_slw_cell_DetCells;
   vector<vector<float> > *trig_EF_emcl_slw_cell_TimeCells;
   vector<vector<float> > *trig_EF_emcl_slw_cell_xCells;
   vector<vector<float> > *trig_EF_emcl_slw_cell_yCells;
   vector<vector<float> > *trig_EF_emcl_slw_cell_zCells;
   vector<vector<int> > *trig_EF_emcl_slw_cell_BadCells;
   vector<vector<unsigned int> > *trig_EF_emcl_slw_cell_IdCells;
   Int_t           trig_EF_el_n;
   vector<float>   *trig_EF_el_E;
   vector<float>   *trig_EF_el_Et;
   vector<float>   *trig_EF_el_pt;
   vector<float>   *trig_EF_el_m;
   vector<float>   *trig_EF_el_eta;
   vector<float>   *trig_EF_el_phi;
   vector<float>   *trig_EF_el_px;
   vector<float>   *trig_EF_el_py;
   vector<float>   *trig_EF_el_pz;
   vector<float>   *trig_EF_el_charge;
   vector<int>     *trig_EF_el_author;
   vector<unsigned int> *trig_EF_el_isEM;
   vector<int>     *trig_EF_el_loose;
   vector<int>     *trig_EF_el_medium;
   vector<int>     *trig_EF_el_mediumIso;
   vector<int>     *trig_EF_el_tight;
   vector<int>     *trig_EF_el_tightIso;
   vector<float>   *trig_EF_el_Ethad;
   vector<float>   *trig_EF_el_Ethad1;
   vector<float>   *trig_EF_el_f1;
   vector<float>   *trig_EF_el_f1core;
   vector<float>   *trig_EF_el_Emins1;
   vector<float>   *trig_EF_el_fside;
   vector<float>   *trig_EF_el_Emax2;
   vector<float>   *trig_EF_el_ws3;
   vector<float>   *trig_EF_el_wstot;
   vector<float>   *trig_EF_el_emaxs1;
   vector<float>   *trig_EF_el_deltaEs;
   vector<float>   *trig_EF_el_E233;
   vector<float>   *trig_EF_el_E237;
   vector<float>   *trig_EF_el_E277;
   vector<float>   *trig_EF_el_weta2;
   vector<float>   *trig_EF_el_f3;
   vector<float>   *trig_EF_el_f3core;
   vector<float>   *trig_EF_el_rphiallcalo;
   vector<float>   *trig_EF_el_Etcone45;
   vector<float>   *trig_EF_el_Etcone20;
   vector<float>   *trig_EF_el_Etcone30;
   vector<float>   *trig_EF_el_Etcone40;
   vector<float>   *trig_EF_el_pos7;
   vector<float>   *trig_EF_el_etacorrmag;
   vector<float>   *trig_EF_el_deltaeta1;
   vector<float>   *trig_EF_el_deltaeta2;
   vector<float>   *trig_EF_el_deltaphi2;
   vector<float>   *trig_EF_el_reta;
   vector<float>   *trig_EF_el_rphi;
   vector<float>   *trig_EF_el_EtringnoisedR03sig2;
   vector<float>   *trig_EF_el_EtringnoisedR03sig3;
   vector<float>   *trig_EF_el_EtringnoisedR03sig4;
   vector<double>  *trig_EF_el_isolationlikelihoodjets;
   vector<double>  *trig_EF_el_isolationlikelihoodhqelectrons;
   vector<double>  *trig_EF_el_electronweight;
   vector<double>  *trig_EF_el_electronbgweight;
   vector<double>  *trig_EF_el_softeweight;
   vector<double>  *trig_EF_el_softebgweight;
   vector<double>  *trig_EF_el_neuralnet;
   vector<double>  *trig_EF_el_Hmatrix;
   vector<double>  *trig_EF_el_Hmatrix5;
   vector<double>  *trig_EF_el_adaboost;
   vector<double>  *trig_EF_el_softeneuralnet;
   vector<float>   *trig_EF_el_zvertex;
   vector<float>   *trig_EF_el_errz;
   vector<float>   *trig_EF_el_etap;
   vector<float>   *trig_EF_el_depth;
   vector<float>   *trig_EF_el_refittedtrackcovd0;
   vector<float>   *trig_EF_el_refittedtrackcovz0;
   vector<float>   *trig_EF_el_refittedtrackcovphi;
   vector<float>   *trig_EF_el_refittedtrackcovtheta;
   vector<float>   *trig_EF_el_refittedtrackcovqoverp;
   vector<float>   *trig_EF_el_refittedtrackcovd0z0;
   vector<float>   *trig_EF_el_refittedtrackcovz0phi;
   vector<float>   *trig_EF_el_refittedtrackcovz0theta;
   vector<float>   *trig_EF_el_refittedtrackcovz0qoverp;
   vector<float>   *trig_EF_el_refittedtrackcovd0phi;
   vector<float>   *trig_EF_el_refittedtrackcovd0theta;
   vector<float>   *trig_EF_el_refittedtrackcovd0qoverp;
   vector<float>   *trig_EF_el_refittedtrackcovphitheta;
   vector<float>   *trig_EF_el_refittedtrackcovphiqoverp;
   vector<float>   *trig_EF_el_refittedtrackcovthetaqoverp;
   vector<int>     *trig_EF_el_EF_2e12_tight;
   vector<int>     *trig_EF_el_EF_2e15_medium;
   vector<int>     *trig_EF_el_EF_2e20_loose;
   vector<int>     *trig_EF_el_EF_2e5_medium;
   vector<int>     *trig_EF_el_EF_2e5_medium1;
   vector<int>     *trig_EF_el_EF_e105_loose1;
   vector<int>     *trig_EF_el_EF_e10_NoCut;
   vector<int>     *trig_EF_el_EF_e10_loose;
   vector<int>     *trig_EF_el_EF_e10_loose_IdScan;
   vector<int>     *trig_EF_el_EF_e10_loose_SiTrk;
   vector<int>     *trig_EF_el_EF_e10_loose_TRT;
   vector<int>     *trig_EF_el_EF_e10_loose_mu6;
   vector<int>     *trig_EF_el_EF_e10_medium;
   vector<int>     *trig_EF_el_EF_e10_medium_SiTrk;
   vector<int>     *trig_EF_el_EF_e10_medium_SiTrk_robust;
   vector<int>     *trig_EF_el_EF_e12_medium;
   vector<int>     *trig_EF_el_EF_e15_medium;
   vector<int>     *trig_EF_el_EF_e15i_medium;
   vector<int>     *trig_EF_el_EF_e20_loose;
   vector<int>     *trig_EF_el_EF_e20_loose_IdScan;
   vector<int>     *trig_EF_el_EF_e20_loose_NoIDTrkCut;
   vector<int>     *trig_EF_el_EF_e20_loose_SiTrk;
   vector<int>     *trig_EF_el_EF_e20_loose_SiTrk_robust;
   vector<int>     *trig_EF_el_EF_e20_loose_TRT;
   vector<int>     *trig_EF_el_EF_e20_loose_passEF;
   vector<int>     *trig_EF_el_EF_e20_loose_passL2;
   vector<int>     *trig_EF_el_EF_e20_loose_xe20;
   vector<int>     *trig_EF_el_EF_e20_loose_xe30;
   vector<int>     *trig_EF_el_EF_e20_medium;
   vector<int>     *trig_EF_el_EF_e20i_loose;
   vector<int>     *trig_EF_el_EF_e22i_tight;
   vector<int>     *trig_EF_el_EF_e25_loose;
   vector<int>     *trig_EF_el_EF_e25_medium;
   vector<int>     *trig_EF_el_EF_e25i_loose;
   vector<int>     *trig_EF_el_EF_e25i_medium;
   vector<int>     *trig_EF_el_EF_e55_loose1;
   vector<int>     *trig_EF_el_EF_e5_medium;
   vector<int>     *trig_EF_el_EF_e5_medium1;
   vector<int>     *trig_EF_el_EF_e5_medium_L2SW;
   vector<int>     *trig_EF_el_EF_e5_medium_MV;
   vector<int>     *trig_EF_el_EF_e5_medium_Ringer;
   vector<int>     *trig_EF_el_EF_e5_medium_e10_loose;
   vector<int>     *trig_EF_el_EF_em105_passHLT;
   vector<int>     *trig_EF_el_EF_em15_passHLT;
   vector<int>     *trig_EF_el_EF_em15i_passHLT;
   vector<int>     *trig_EF_el_EF_em20_passHLT;
   vector<int>     *trig_EF_el_EF_em20i_passHLT;
   Int_t           trig_EF_ph_n;
   vector<float>   *trig_EF_ph_E;
   vector<float>   *trig_EF_ph_Et;
   vector<float>   *trig_EF_ph_pt;
   vector<float>   *trig_EF_ph_m;
   vector<float>   *trig_EF_ph_eta;
   vector<float>   *trig_EF_ph_phi;
   vector<float>   *trig_EF_ph_px;
   vector<float>   *trig_EF_ph_py;
   vector<float>   *trig_EF_ph_pz;
   vector<int>     *trig_EF_ph_EF_2g10_loose;
   vector<int>     *trig_EF_ph_EF_2g10_loose_mu6;
   vector<int>     *trig_EF_ph_EF_2g17i_tight;
   vector<int>     *trig_EF_ph_EF_2g20_loose;
   vector<int>     *trig_EF_ph_EF_g105;
   vector<int>     *trig_EF_ph_EF_g10_loose;
   vector<int>     *trig_EF_ph_EF_g10_loose_larcalib;
   vector<int>     *trig_EF_ph_EF_g10_nocut;
   vector<int>     *trig_EF_ph_EF_g150;
   vector<int>     *trig_EF_ph_EF_g15_loose;
   vector<int>     *trig_EF_ph_EF_g20_loose;
   vector<int>     *trig_EF_ph_EF_g20_loose_cnv;
   vector<int>     *trig_EF_ph_EF_g20_loose_larcalib;
   vector<int>     *trig_EF_ph_EF_g20_loose_xe20;
   vector<int>     *trig_EF_ph_EF_g20i_loose;
   vector<int>     *trig_EF_ph_EF_g20i_loose_larcalib;
   vector<int>     *trig_EF_ph_EF_g25_loose;
   vector<int>     *trig_EF_ph_EF_g25_loose_xe30;
   vector<int>     *trig_EF_ph_EF_g25i_loose;
   vector<int>     *trig_EF_ph_EF_g55_tight;
   Int_t           trig_RoI_L2_e_n;
   vector<short>   *trig_RoI_L2_e_type;
   vector<short>   *trig_RoI_L2_e_lastStep;
   vector<int>     *trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgamma;
   vector<int>     *trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgammaStatus;
   vector<int>     *trig_RoI_L2_e_TrigEMCluster;
   vector<int>     *trig_RoI_L2_e_TrigEMClusterStatus;
   vector<int>     *trig_RoI_L2_e_EmTau_ROI;
   vector<int>     *trig_RoI_L2_e_EmTau_ROIStatus;
   vector<vector<int> > *trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma;
   vector<vector<int> > *trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus;
   vector<vector<int> > *trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma;
   vector<vector<int> > *trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus;
   vector<vector<int> > *trig_RoI_L2_e_TrigElectronContainer;
   vector<vector<int> > *trig_RoI_L2_e_TrigElectronContainerStatus;
   vector<vector<int> > *trig_RoI_L2_e_TrigPhotonContainer;
   vector<vector<int> > *trig_RoI_L2_e_TrigPhotonContainerStatus;
   Int_t           trig_RoI_EF_e_n;
   vector<short>   *trig_RoI_EF_e_type;
   vector<short>   *trig_RoI_EF_e_lastStep;
   vector<int>     *trig_RoI_EF_e_EmTau_ROI;
   vector<int>     *trig_RoI_EF_e_EmTau_ROIStatus;
   vector<vector<int> > *trig_RoI_EF_e_Rec__TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFID;
   vector<vector<int> > *trig_RoI_EF_e_Rec__TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFIDStatus;
   vector<vector<int> > *trig_RoI_EF_e_egammaContainer_egamma_Electrons;
   vector<vector<int> > *trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus;
   vector<vector<int> > *trig_RoI_EF_e_egammaContainer_egamma_Photons;
   vector<vector<int> > *trig_RoI_EF_e_egammaContainer_egamma_PhotonsStatus;
   Int_t           trk_n;
   vector<float>   *trk_pt;
   vector<float>   *trk_eta;
   vector<float>   *trk_d0_wrtPV;
   vector<float>   *trk_z0_wrtPV;
   vector<float>   *trk_phi_wrtPV;
   vector<float>   *trk_cov_d0_wrtPV;
   vector<float>   *trk_cov_z0_wrtPV;
   vector<float>   *trk_cov_phi_wrtPV;
   vector<float>   *trk_cov_theta_wrtPV;
   vector<float>   *trk_cov_qoverp_wrtPV;
   vector<float>   *trk_chi2;
   vector<int>     *trk_ndof;
   vector<int>     *trk_nBLHits;
   vector<int>     *trk_nPixHits;
   vector<int>     *trk_nSCTHits;
   vector<int>     *trk_nTRTHits;
   vector<int>     *trk_nPixHoles;
   vector<int>     *trk_nSCTHoles;
   vector<int>     *trk_nMDTHits;
   vector<int>     *trk_nCSCEtaHits;
   vector<int>     *trk_nCSCPhiHits;
   vector<int>     *trk_nRPCEtaHits;
   vector<int>     *trk_nRPCPhiHits;
   vector<int>     *trk_nTGCEtaHits;
   vector<int>     *trk_nTGCPhiHits;
   vector<int>     *trk_nHits;
   vector<int>     *trk_nHoles;
   vector<int>     *trk_hitPattern;
   vector<float>   *trk_TRTHighTHitsRatio;
   vector<float>   *trk_pixeldEdx;
   vector<int>     *trk_fitter;
   vector<int>     *trk_patternReco1;
   vector<int>     *trk_patternReco2;
   vector<int>     *trk_seedFinder;
   vector<int>     *trk_blayerPrediction_expectHit;
   vector<vector<float> > *trk_blayerPrediction_x;
   vector<vector<float> > *trk_blayerPrediction_y;
   vector<vector<float> > *trk_blayerPrediction_z;
   vector<vector<float> > *trk_blayerPrediction_locX;
   vector<vector<float> > *trk_blayerPrediction_locY;
   vector<vector<float> > *trk_blayerPrediction_err_locX;
   vector<vector<float> > *trk_blayerPrediction_err_locY;
   vector<vector<float> > *trk_blayerPrediction_etaDistToEdge;
   vector<vector<float> > *trk_blayerPrediction_phiDistToEdge;
   vector<vector<unsigned int> > *trk_blayerPrediction_detElementId;
   vector<vector<int> > *trk_blayerPrediction_row;
   vector<vector<int> > *trk_blayerPrediction_col;
   vector<vector<int> > *trk_blayerPrediction_type;
   vector<float>   *trk_mc_probability;
   vector<int>     *trk_mc_barcode;
   vector<int>     *trk_mc_index;
   Int_t           vxp_n;
   vector<float>   *vxp_x;
   vector<float>   *vxp_y;
   vector<float>   *vxp_z;
   vector<float>   *vxp_err_x;
   vector<float>   *vxp_err_y;
   vector<float>   *vxp_err_z;
   vector<float>   *vxp_chi2;
   vector<int>     *vxp_ndof;
   vector<float>   *vxp_px;
   vector<float>   *vxp_py;
   vector<float>   *vxp_pz;
   vector<float>   *vxp_E;
   vector<float>   *vxp_m;
   vector<int>     *vxp_nTracks;
   vector<float>   *vxp_sumPt;
   vector<int>     *vxp_type;
   vector<float>   *vxp_sumWeightInliers;
   vector<float>   *vxp_sumWeightOutliers;
   vector<float>   *vxp_sumWeightPileup;
   vector<float>   *vxp_sumWeightFakes;
   vector<int>     *vxp_nInliers;
   vector<int>     *vxp_nOutliers;
   vector<int>     *vxp_nPileUp;
   vector<int>     *vxp_nFakes;
   vector<float>   *vxp_purity;
   vector<int>     *vxp_trk_n;
   vector<vector<float> > *vxp_trk_chi2;
   vector<vector<float> > *vxp_trk_d0;
   vector<vector<float> > *vxp_trk_z0;
   vector<vector<float> > *vxp_trk_unbiased_d0;
   vector<vector<float> > *vxp_trk_unbiased_z0;
   vector<vector<float> > *vxp_trk_err_unbiased_d0;
   vector<vector<float> > *vxp_trk_err_unbiased_z0;
   vector<vector<float> > *vxp_trk_phi;
   vector<vector<float> > *vxp_trk_theta;
   vector<vector<float> > *vxp_trk_weight;
   vector<vector<int> > *vxp_trk_index;
   Bool_t          jetMETTriggerBitsFiller_L1_J10;
   Bool_t          jetMETTriggerBitsFiller_L1_J15;
   Bool_t          jetMETTriggerBitsFiller_L1_J5;
   Int_t           mcVx_n;
   vector<float>   *mcVx_x;
   vector<float>   *mcVx_y;
   vector<float>   *mcVx_z;
   Int_t           mcTrk_n;
   vector<float>   *mcTrk_gen_pt;
   vector<float>   *mcTrk_gen_eta;
   vector<float>   *mcTrk_gen_phi;
   vector<float>   *mcTrk_gen_energy;
   vector<int>     *mcTrk_perigee_ok;
   vector<float>   *mcTrk_perigee_d0;
   vector<float>   *mcTrk_perigee_z0;
   vector<float>   *mcTrk_perigee_phi;
   vector<float>   *mcTrk_perigee_theta;
   vector<float>   *mcTrk_perigee_qoverp;
   vector<int>     *mcTrk_pdg;
   vector<float>   *mcTrk_charge;
   vector<int>     *mcTrk_barcode;
   vector<int>     *mcTrk_status;
   vector<int>     *mcTrk_mother_pdg;
   vector<int>     *mcTrk_mother_barcode;
   vector<int>     *mcTrk_child_n;
   vector<vector<int> > *mcTrk_child_barcode;
   vector<float>   *mcTrk_begVtx_x;
   vector<float>   *mcTrk_begVtx_y;
   vector<float>   *mcTrk_begVtx_z;
   vector<float>   *mcTrk_begVtx_barcode;
   vector<float>   *mcTrk_endVtx_x;
   vector<float>   *mcTrk_endVtx_y;
   vector<float>   *mcTrk_endVtx_z;
   vector<float>   *mcTrk_endVtx_barcode;
   Int_t           mc_n;
   vector<float>   *mc_pt;
   vector<float>   *mc_m;
   vector<float>   *mc_eta;
   vector<float>   *mc_phi;
   vector<int>     *mc_status;
   vector<int>     *mc_barcode;
   vector<vector<int> > *mc_parents;
   vector<vector<int> > *mc_children;
   vector<int>     *mc_pdgId;
   vector<float>   *mc_charge;
   vector<float>   *mc_vx_x;
   vector<float>   *mc_vx_y;
   vector<float>   *mc_vx_z;
   vector<vector<int> > *mc_child_index;
   vector<vector<int> > *mc_parent_index;
   Int_t           mcEvt_n;
   vector<int>     *mcEvt_signal_process_id;
   vector<int>     *mcEvt_event_number;
   vector<double>  *mcEvt_event_scale;
   vector<double>  *mcEvt_alphaQCD;
   vector<double>  *mcEvt_alphaQED;
   vector<int>     *mcEvt_pdf_id1;
   vector<int>     *mcEvt_pdf_id2;
   vector<double>  *mcEvt_pdf_x1;
   vector<double>  *mcEvt_pdf_x2;
   vector<double>  *mcEvt_pdf_scale;
   vector<double>  *mcEvt_pdf1;
   vector<double>  *mcEvt_pdf2;
   vector<double>  *mcEvt_weight;
   Int_t           AntiKt4Truth_n;
   vector<float>   *AntiKt4Truth_E;
   vector<float>   *AntiKt4Truth_pt;
   vector<float>   *AntiKt4Truth_m;
   vector<float>   *AntiKt4Truth_eta;
   vector<float>   *AntiKt4Truth_phi;
   vector<float>   *AntiKt4Truth_EtaPhys;
   vector<float>   *AntiKt4Truth_PhiPhys;
   vector<float>   *AntiKt4Truth_MPhys;
   vector<float>   *AntiKt4Truth_WIDTH;
   vector<float>   *AntiKt4Truth_n90;
   vector<float>   *AntiKt4Truth_Timing;
   vector<float>   *AntiKt4Truth_LArQuality;
   vector<float>   *AntiKt4Truth_nTrk;
   vector<float>   *AntiKt4Truth_sumPtTrk;
   vector<float>   *AntiKt4Truth_OriginIndex;
   vector<float>   *AntiKt4Truth_HECQuality;
   vector<float>   *AntiKt4Truth_NegativeE;
   vector<float>   *AntiKt4Truth_BCH_CORR_CELL;
   vector<float>   *AntiKt4Truth_BCH_CORR_DOTX;
   vector<float>   *AntiKt4Truth_BCH_CORR_JET;
   vector<float>   *AntiKt4Truth_BCH_CORR_JET_FORCELL;
   vector<float>   *AntiKt4Truth_ENG_BAD_CELLS;
   vector<float>   *AntiKt4Truth_N_BAD_CELLS;
   vector<float>   *AntiKt4Truth_N_BAD_CELLS_CORR;
   vector<float>   *AntiKt4Truth_BAD_CELLS_CORR_E;
   vector<int>     *AntiKt4Truth_SamplingMax;
   vector<double>  *AntiKt4Truth_fracSamplingMax;
   vector<float>   *AntiKt4Truth_hecf;
   vector<float>   *AntiKt4Truth_tgap3f;
   vector<int>     *AntiKt4Truth_isGood;
   vector<float>   *AntiKt4Truth_emfrac;
   vector<float>   *AntiKt4Truth_GCWJES;
   vector<float>   *AntiKt4Truth_EMJES;
   vector<float>   *AntiKt4Truth_CB;
   vector<float>   *AntiKt4Truth_emscale_E;
   vector<float>   *AntiKt4Truth_emscale_pt;
   vector<float>   *AntiKt4Truth_emscale_m;
   vector<float>   *AntiKt4Truth_emscale_eta;
   vector<float>   *AntiKt4Truth_emscale_phi;
   vector<float>   *AntiKt4Truth_jvtxf;
   vector<float>   *AntiKt4Truth_jvtx_x;
   vector<float>   *AntiKt4Truth_jvtx_y;
   vector<float>   *AntiKt4Truth_jvtx_z;
   vector<float>   *AntiKt4Truth_e_PreSamplerB;
   vector<float>   *AntiKt4Truth_e_EMB1;
   vector<float>   *AntiKt4Truth_e_EMB2;
   vector<float>   *AntiKt4Truth_e_EMB3;
   vector<float>   *AntiKt4Truth_e_PreSamplerE;
   vector<float>   *AntiKt4Truth_e_EME1;
   vector<float>   *AntiKt4Truth_e_EME2;
   vector<float>   *AntiKt4Truth_e_EME3;
   vector<float>   *AntiKt4Truth_e_HEC0;
   vector<float>   *AntiKt4Truth_e_HEC1;
   vector<float>   *AntiKt4Truth_e_HEC2;
   vector<float>   *AntiKt4Truth_e_HEC3;
   vector<float>   *AntiKt4Truth_e_TileBar0;
   vector<float>   *AntiKt4Truth_e_TileBar1;
   vector<float>   *AntiKt4Truth_e_TileBar2;
   vector<float>   *AntiKt4Truth_e_TileGap1;
   vector<float>   *AntiKt4Truth_e_TileGap2;
   vector<float>   *AntiKt4Truth_e_TileGap3;
   vector<float>   *AntiKt4Truth_e_TileExt0;
   vector<float>   *AntiKt4Truth_e_TileExt1;
   vector<float>   *AntiKt4Truth_e_TileExt2;
   vector<float>   *AntiKt4Truth_e_FCAL0;
   vector<float>   *AntiKt4Truth_e_FCAL1;
   vector<float>   *AntiKt4Truth_e_FCAL2;
   vector<vector<float> > *AntiKt4Truth_shapeBins;
   vector<int>     *AntiKt4Truth_Nconst;
   vector<vector<float> > *AntiKt4Truth_ptconst_emscale;
   vector<vector<float> > *AntiKt4Truth_econst_emscale;
   vector<vector<float> > *AntiKt4Truth_etaconst_emscale;
   vector<vector<float> > *AntiKt4Truth_phiconst_emscale;
   vector<vector<float> > *AntiKt4Truth_weightconst_emscale;
   vector<float>   *AntiKt4Truth_constscale_E;
   vector<float>   *AntiKt4Truth_constscale_pt;
   vector<float>   *AntiKt4Truth_constscale_m;
   vector<float>   *AntiKt4Truth_constscale_eta;
   vector<float>   *AntiKt4Truth_constscale_phi;
   vector<double>  *AntiKt4Truth_flavor_component_ip2d_pu;
   vector<double>  *AntiKt4Truth_flavor_component_ip2d_pb;
   vector<double>  *AntiKt4Truth_flavor_component_ip2d_ntrk;
   vector<double>  *AntiKt4Truth_flavor_component_ip3d_pu;
   vector<double>  *AntiKt4Truth_flavor_component_ip3d_pb;
   vector<double>  *AntiKt4Truth_flavor_component_ip3d_ntrk;
   vector<double>  *AntiKt4Truth_flavor_component_sv1_pu;
   vector<double>  *AntiKt4Truth_flavor_component_sv1_pb;
   vector<double>  *AntiKt4Truth_flavor_component_sv2_pu;
   vector<double>  *AntiKt4Truth_flavor_component_sv2_pb;
   vector<double>  *AntiKt4Truth_flavor_component_svp_mass;
   vector<double>  *AntiKt4Truth_flavor_component_svp_n2t;
   vector<double>  *AntiKt4Truth_flavor_component_svp_svok;
   vector<double>  *AntiKt4Truth_flavor_component_svp_ntrk;
   vector<double>  *AntiKt4Truth_flavor_component_svp_ntrkv;
   vector<double>  *AntiKt4Truth_flavor_component_svp_ntrkj;
   vector<double>  *AntiKt4Truth_flavor_component_svp_efrc;
   vector<double>  *AntiKt4Truth_flavor_component_sv0p_mass;
   vector<double>  *AntiKt4Truth_flavor_component_sv0p_n2t;
   vector<double>  *AntiKt4Truth_flavor_component_sv0p_svok;
   vector<double>  *AntiKt4Truth_flavor_component_sv0p_ntrk;
   vector<double>  *AntiKt4Truth_flavor_component_sv0p_ntrkv;
   vector<double>  *AntiKt4Truth_flavor_component_sv0p_ntrkj;
   vector<double>  *AntiKt4Truth_flavor_component_sv0p_efrc;
   vector<double>  *AntiKt4Truth_flavor_component_jfit_pu;
   vector<double>  *AntiKt4Truth_flavor_component_jfit_pb;
   vector<double>  *AntiKt4Truth_flavor_component_jfit_nvtxnt;
   vector<double>  *AntiKt4Truth_flavor_component_jfit_nvtx1t;
   vector<double>  *AntiKt4Truth_flavor_component_jfit_ntrk;
   vector<double>  *AntiKt4Truth_flavor_component_jfit_efrc;
   vector<double>  *AntiKt4Truth_flavor_component_jfit_mass;
   vector<double>  *AntiKt4Truth_flavor_component_jfit_sig3d;
   vector<double>  *AntiKt4Truth_flavor_weight;
   vector<double>  *AntiKt4Truth_flavor_weight_TrackCounting2D;
   vector<double>  *AntiKt4Truth_flavor_weight_JetProb;
   vector<double>  *AntiKt4Truth_flavor_weight_IP1D;
   vector<double>  *AntiKt4Truth_flavor_weight_IP2D;
   vector<double>  *AntiKt4Truth_flavor_weight_IP3D;
   vector<double>  *AntiKt4Truth_flavor_weight_SV0;
   vector<double>  *AntiKt4Truth_flavor_weight_SV1;
   vector<double>  *AntiKt4Truth_flavor_weight_SV2;
   vector<double>  *AntiKt4Truth_flavor_weight_JetFitterTag;
   vector<double>  *AntiKt4Truth_flavor_weight_JetFitterCOMB;
   vector<double>  *AntiKt4Truth_flavor_weight_JetFitterTagNN;
   vector<double>  *AntiKt4Truth_flavor_weight_JetFitterCOMBNN;
   vector<double>  *AntiKt4Truth_flavor_weight_SoftMuonTag;
   vector<double>  *AntiKt4Truth_flavor_weight_SoftElectronTag;
   vector<unsigned int> *AntiKt4Truth_RoIword;
   vector<double>  *AntiKt4Truth_flavor_truth_dRminToB;
   vector<double>  *AntiKt4Truth_flavor_truth_dRminToC;
   vector<double>  *AntiKt4Truth_flavor_truth_dRminToT;
   vector<int>     *AntiKt4Truth_flavor_truth_BHadronpdg;
   vector<int>     *AntiKt4Truth_flavor_truth_trueflav;
   vector<float>   *AntiKt4Truth_el_dr;
   vector<int>     *AntiKt4Truth_el_matched;
   vector<float>   *AntiKt4Truth_mu_dr;
   vector<int>     *AntiKt4Truth_mu_matched;

   // List of branches
   TBranch        *b_ei_RunNumber;   //!
   TBranch        *b_ei_EventNumber;   //!
   TBranch        *b_ei_timestamp;   //!
   TBranch        *b_ei_timestamp_ns;   //!
   TBranch        *b_ei_lbn;   //!
   TBranch        *b_ei_bcid;   //!
   TBranch        *b_ei_detmask0;   //!
   TBranch        *b_ei_detmask1;   //!
   TBranch        *b_ei_pixelFlags;   //!
   TBranch        *b_ei_sctFlags;   //!
   TBranch        *b_ei_trtFlags;   //!
   TBranch        *b_ei_larFlags;   //!
   TBranch        *b_ei_tileFlags;   //!
   TBranch        *b_ei_muonFlags;   //!
   TBranch        *b_ei_fwdFlags;   //!
   TBranch        *b_ei_coreFlags;   //!
   TBranch        *b_ei_pixelError;   //!
   TBranch        *b_ei_sctError;   //!
   TBranch        *b_ei_trtError;   //!
   TBranch        *b_ei_larError;   //!
   TBranch        *b_ei_tileError;   //!
   TBranch        *b_ei_muonError;   //!
   TBranch        *b_ei_fwdError;   //!
   TBranch        *b_ei_coreError;   //!
   TBranch        *b_AntiKt4H1Topo_n;   //!
   TBranch        *b_AntiKt4H1Topo_E;   //!
   TBranch        *b_AntiKt4H1Topo_pt;   //!
   TBranch        *b_AntiKt4H1Topo_m;   //!
   TBranch        *b_AntiKt4H1Topo_eta;   //!
   TBranch        *b_AntiKt4H1Topo_phi;   //!
   TBranch        *b_AntiKt4H1Topo_EtaPhys;   //!
   TBranch        *b_AntiKt4H1Topo_PhiPhys;   //!
   TBranch        *b_AntiKt4H1Topo_MPhys;   //!
   TBranch        *b_AntiKt4H1Topo_WIDTH;   //!
   TBranch        *b_AntiKt4H1Topo_n90;   //!
   TBranch        *b_AntiKt4H1Topo_Timing;   //!
   TBranch        *b_AntiKt4H1Topo_LArQuality;   //!
   TBranch        *b_AntiKt4H1Topo_nTrk;   //!
   TBranch        *b_AntiKt4H1Topo_sumPtTrk;   //!
   TBranch        *b_AntiKt4H1Topo_OriginIndex;   //!
   TBranch        *b_AntiKt4H1Topo_HECQuality;   //!
   TBranch        *b_AntiKt4H1Topo_NegativeE;   //!
   TBranch        *b_AntiKt4H1Topo_BCH_CORR_CELL;   //!
   TBranch        *b_AntiKt4H1Topo_BCH_CORR_DOTX;   //!
   TBranch        *b_AntiKt4H1Topo_BCH_CORR_JET;   //!
   TBranch        *b_AntiKt4H1Topo_BCH_CORR_JET_FORCELL;   //!
   TBranch        *b_AntiKt4H1Topo_ENG_BAD_CELLS;   //!
   TBranch        *b_AntiKt4H1Topo_N_BAD_CELLS;   //!
   TBranch        *b_AntiKt4H1Topo_N_BAD_CELLS_CORR;   //!
   TBranch        *b_AntiKt4H1Topo_BAD_CELLS_CORR_E;   //!
   TBranch        *b_AntiKt4H1Topo_SamplingMax;   //!
   TBranch        *b_AntiKt4H1Topo_fracSamplingMax;   //!
   TBranch        *b_AntiKt4H1Topo_hecf;   //!
   TBranch        *b_AntiKt4H1Topo_tgap3f;   //!
   TBranch        *b_AntiKt4H1Topo_isGood;   //!
   TBranch        *b_AntiKt4H1Topo_emfrac;   //!
   TBranch        *b_AntiKt4H1Topo_GCWJES;   //!
   TBranch        *b_AntiKt4H1Topo_EMJES;   //!
   TBranch        *b_AntiKt4H1Topo_CB;   //!
   TBranch        *b_AntiKt4H1Topo_emscale_E;   //!
   TBranch        *b_AntiKt4H1Topo_emscale_pt;   //!
   TBranch        *b_AntiKt4H1Topo_emscale_m;   //!
   TBranch        *b_AntiKt4H1Topo_emscale_eta;   //!
   TBranch        *b_AntiKt4H1Topo_emscale_phi;   //!
   TBranch        *b_AntiKt4H1Topo_jvtxf;   //!
   TBranch        *b_AntiKt4H1Topo_jvtx_x;   //!
   TBranch        *b_AntiKt4H1Topo_jvtx_y;   //!
   TBranch        *b_AntiKt4H1Topo_jvtx_z;   //!
   TBranch        *b_AntiKt4H1Topo_e_PreSamplerB;   //!
   TBranch        *b_AntiKt4H1Topo_e_EMB1;   //!
   TBranch        *b_AntiKt4H1Topo_e_EMB2;   //!
   TBranch        *b_AntiKt4H1Topo_e_EMB3;   //!
   TBranch        *b_AntiKt4H1Topo_e_PreSamplerE;   //!
   TBranch        *b_AntiKt4H1Topo_e_EME1;   //!
   TBranch        *b_AntiKt4H1Topo_e_EME2;   //!
   TBranch        *b_AntiKt4H1Topo_e_EME3;   //!
   TBranch        *b_AntiKt4H1Topo_e_HEC0;   //!
   TBranch        *b_AntiKt4H1Topo_e_HEC1;   //!
   TBranch        *b_AntiKt4H1Topo_e_HEC2;   //!
   TBranch        *b_AntiKt4H1Topo_e_HEC3;   //!
   TBranch        *b_AntiKt4H1Topo_e_TileBar0;   //!
   TBranch        *b_AntiKt4H1Topo_e_TileBar1;   //!
   TBranch        *b_AntiKt4H1Topo_e_TileBar2;   //!
   TBranch        *b_AntiKt4H1Topo_e_TileGap1;   //!
   TBranch        *b_AntiKt4H1Topo_e_TileGap2;   //!
   TBranch        *b_AntiKt4H1Topo_e_TileGap3;   //!
   TBranch        *b_AntiKt4H1Topo_e_TileExt0;   //!
   TBranch        *b_AntiKt4H1Topo_e_TileExt1;   //!
   TBranch        *b_AntiKt4H1Topo_e_TileExt2;   //!
   TBranch        *b_AntiKt4H1Topo_e_FCAL0;   //!
   TBranch        *b_AntiKt4H1Topo_e_FCAL1;   //!
   TBranch        *b_AntiKt4H1Topo_e_FCAL2;   //!
   TBranch        *b_AntiKt4H1Topo_shapeBins;   //!
   TBranch        *b_AntiKt4H1Topo_Nconst;   //!
   TBranch        *b_AntiKt4H1Topo_ptconst_emscale;   //!
   TBranch        *b_AntiKt4H1Topo_econst_emscale;   //!
   TBranch        *b_AntiKt4H1Topo_etaconst_emscale;   //!
   TBranch        *b_AntiKt4H1Topo_phiconst_emscale;   //!
   TBranch        *b_AntiKt4H1Topo_weightconst_emscale;   //!
   TBranch        *b_AntiKt4H1Topo_constscale_E;   //!
   TBranch        *b_AntiKt4H1Topo_constscale_pt;   //!
   TBranch        *b_AntiKt4H1Topo_constscale_m;   //!
   TBranch        *b_AntiKt4H1Topo_constscale_eta;   //!
   TBranch        *b_AntiKt4H1Topo_constscale_phi;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_ip2d_pu;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_ip2d_pb;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_ip2d_ntrk;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_ip3d_pu;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_ip3d_pb;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_ip3d_ntrk;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_sv1_pu;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_sv1_pb;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_sv2_pu;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_sv2_pb;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_svp_mass;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_svp_n2t;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_svp_svok;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_svp_ntrk;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_svp_ntrkv;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_svp_ntrkj;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_svp_efrc;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_sv0p_mass;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_sv0p_n2t;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_sv0p_svok;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_sv0p_ntrk;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_sv0p_ntrkv;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_sv0p_ntrkj;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_sv0p_efrc;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_jfit_pu;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_jfit_pb;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_jfit_nvtxnt;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_jfit_nvtx1t;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_jfit_ntrk;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_jfit_efrc;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_jfit_mass;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_component_jfit_sig3d;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_TrackCounting2D;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_JetProb;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_IP1D;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_IP2D;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_IP3D;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_SV0;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_SV1;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_SV2;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_JetFitterTag;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_JetFitterCOMB;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_JetFitterTagNN;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_JetFitterCOMBNN;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_SoftMuonTag;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_weight_SoftElectronTag;   //!
   TBranch        *b_AntiKt4H1Topo_RoIword;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_truth_dRminToB;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_truth_dRminToC;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_truth_dRminToT;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_truth_BHadronpdg;   //!
   TBranch        *b_AntiKt4H1Topo_flavor_truth_trueflav;   //!
   TBranch        *b_AntiKt4H1Topo_el_dr;   //!
   TBranch        *b_AntiKt4H1Topo_el_matched;   //!
   TBranch        *b_AntiKt4H1Topo_mu_dr;   //!
   TBranch        *b_AntiKt4H1Topo_mu_matched;   //!
   TBranch        *b_trig_Nav_n;   //!
   TBranch        *b_trig_Nav_chain_ChainId;   //!
   TBranch        *b_trig_Nav_chain_RoIType;   //!
   TBranch        *b_trig_Nav_chain_RoIIndex;   //!
   TBranch        *b_trig_RoI_L2_b_n;   //!
   TBranch        *b_trig_RoI_L2_b_type;   //!
   TBranch        *b_trig_RoI_L2_b_lastStep;   //!
   TBranch        *b_trig_RoI_L2_b_Jet_ROI;   //!
   TBranch        *b_trig_RoI_L2_b_Jet_ROIStatus;   //!
   TBranch        *b_trig_RoI_L2_b_TrigL2BjetContainer;   //!
   TBranch        *b_trig_RoI_L2_b_TrigL2BjetContainerStatus;   //!
   TBranch        *b_trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_Jet;   //!
   TBranch        *b_trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_JetStatus;   //!
   TBranch        *b_trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_Jet;   //!
   TBranch        *b_trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_JetStatus;   //!
   TBranch        *b_trig_RoI_EF_b_n;   //!
   TBranch        *b_trig_RoI_EF_b_type;   //!
   TBranch        *b_trig_RoI_EF_b_lastStep;   //!
   TBranch        *b_trig_RoI_EF_b_Jet_ROI;   //!
   TBranch        *b_trig_RoI_EF_b_Jet_ROIStatus;   //!
   TBranch        *b_trig_RoI_EF_b_TrigEFBjetContainer;   //!
   TBranch        *b_trig_RoI_EF_b_TrigEFBjetContainerStatus;   //!
   TBranch        *b_trig_RoI_EF_b_Rec__TrackParticleContainer;   //!
   TBranch        *b_trig_RoI_EF_b_Rec__TrackParticleContainerStatus;   //!
   TBranch        *b_trig_L1_jet_n;   //!
   TBranch        *b_trig_L1_jet_eta;   //!
   TBranch        *b_trig_L1_jet_phi;   //!
   TBranch        *b_trig_L1_jet_thrNames;   //!
   TBranch        *b_trig_L1_jet_thrValues;   //!
   TBranch        *b_trig_L1_jet_thrPattern;   //!
   TBranch        *b_trig_L1_jet_et4x4;   //!
   TBranch        *b_trig_L1_jet_et6x6;   //!
   TBranch        *b_trig_L1_jet_et8x8;   //!
   TBranch        *b_trig_L1_jet_RoIWord;   //!
   TBranch        *b_trig_DB_SMK;   //!
   TBranch        *b_trig_DB_L1PSK;   //!
   TBranch        *b_trig_DB_HLTPSK;   //!
   TBranch        *b_trig_L1_TAV;   //!
   TBranch        *b_trig_L2_passedPhysics;   //!
   TBranch        *b_trig_EF_passedPhysics;   //!
   TBranch        *b_trig_L1_TBP;   //!
   TBranch        *b_trig_L1_TAP;   //!
   TBranch        *b_trig_L2_passedRaw;   //!
   TBranch        *b_trig_EF_passedRaw;   //!
   TBranch        *b_trig_L2_resurrected;   //!
   TBranch        *b_trig_EF_resurrected;   //!
   TBranch        *b_trig_L2_passedThrough;   //!
   TBranch        *b_trig_EF_passedThrough;   //!
   TBranch        *b_trig_l2sitrk_jet_n;   //!
   TBranch        *b_trig_l2sitrk_jet_algorithmId;   //!
   TBranch        *b_trig_l2sitrk_jet_trackStatus;   //!
   TBranch        *b_trig_l2sitrk_jet_chi2Ndof;   //!
   TBranch        *b_trig_l2sitrk_jet_nStrawHits;   //!
   TBranch        *b_trig_l2sitrk_jet_nHighThrHits;   //!
   TBranch        *b_trig_l2sitrk_jet_nPixelSpacePoints;   //!
   TBranch        *b_trig_l2sitrk_jet_nSCT_SpacePoints;   //!
   TBranch        *b_trig_l2sitrk_jet_hitPattern;   //!
   TBranch        *b_trig_l2sitrk_jet_nStraw;   //!
   TBranch        *b_trig_l2sitrk_jet_nStrawTime;   //!
   TBranch        *b_trig_l2sitrk_jet_a0;   //!
   TBranch        *b_trig_l2sitrk_jet_z0;   //!
   TBranch        *b_trig_l2sitrk_jet_phi0;   //!
   TBranch        *b_trig_l2sitrk_jet_eta;   //!
   TBranch        *b_trig_l2sitrk_jet_pt;   //!
   TBranch        *b_trig_l2sitrk_jet_covariance;   //!
   TBranch        *b_trig_l2idtrk_jet_n;   //!
   TBranch        *b_trig_l2idtrk_jet_algorithmId;   //!
   TBranch        *b_trig_l2idtrk_jet_trackStatus;   //!
   TBranch        *b_trig_l2idtrk_jet_chi2Ndof;   //!
   TBranch        *b_trig_l2idtrk_jet_nStrawHits;   //!
   TBranch        *b_trig_l2idtrk_jet_nHighThrHits;   //!
   TBranch        *b_trig_l2idtrk_jet_nPixelSpacePoints;   //!
   TBranch        *b_trig_l2idtrk_jet_nSCT_SpacePoints;   //!
   TBranch        *b_trig_l2idtrk_jet_hitPattern;   //!
   TBranch        *b_trig_l2idtrk_jet_nStraw;   //!
   TBranch        *b_trig_l2idtrk_jet_nStrawTime;   //!
   TBranch        *b_trig_l2idtrk_jet_a0;   //!
   TBranch        *b_trig_l2idtrk_jet_z0;   //!
   TBranch        *b_trig_l2idtrk_jet_phi0;   //!
   TBranch        *b_trig_l2idtrk_jet_eta;   //!
   TBranch        *b_trig_l2idtrk_jet_pt;   //!
   TBranch        *b_trig_l2idtrk_jet_covariance;   //!
   TBranch        *b_trig_bjefid_n;   //!
   TBranch        *b_trig_bjefid_d0;   //!
   TBranch        *b_trig_bjefid_z0;   //!
   TBranch        *b_trig_bjefid_phi;   //!
   TBranch        *b_trig_bjefid_theta;   //!
   TBranch        *b_trig_bjefid_qoverp;   //!
   TBranch        *b_trig_bjefid_pt;   //!
   TBranch        *b_trig_bjefid_eta;   //!
   TBranch        *b_trig_bjefid_err_d0;   //!
   TBranch        *b_trig_bjefid_err_z0;   //!
   TBranch        *b_trig_bjefid_err_phi;   //!
   TBranch        *b_trig_bjefid_err_theta;   //!
   TBranch        *b_trig_bjefid_err_qoverp;   //!
   TBranch        *b_trig_bjefid_cov_d0_z0;   //!
   TBranch        *b_trig_bjefid_cov_d0_phi;   //!
   TBranch        *b_trig_bjefid_cov_d0_theta;   //!
   TBranch        *b_trig_bjefid_cov_d0_qoverp;   //!
   TBranch        *b_trig_bjefid_cov_z0_phi;   //!
   TBranch        *b_trig_bjefid_cov_z0_theta;   //!
   TBranch        *b_trig_bjefid_cov_z0_qoverp;   //!
   TBranch        *b_trig_bjefid_cov_phi_theta;   //!
   TBranch        *b_trig_bjefid_cov_phi_qoverp;   //!
   TBranch        *b_trig_bjefid_cov_theta_qoverp;   //!
   TBranch        *b_trig_bjefid_d0_wrtPV;   //!
   TBranch        *b_trig_bjefid_z0_wrtPV;   //!
   TBranch        *b_trig_bjefid_phi_wrtPV;   //!
   TBranch        *b_trig_bjefid_err_d0_wrtPV;   //!
   TBranch        *b_trig_bjefid_err_z0_wrtPV;   //!
   TBranch        *b_trig_bjefid_err_phi_wrtPV;   //!
   TBranch        *b_trig_bjefid_err_theta_wrtPV;   //!
   TBranch        *b_trig_bjefid_err_qoverp_wrtPV;   //!
   TBranch        *b_trig_bjefid_cov_d0_z0_wrtPV;   //!
   TBranch        *b_trig_bjefid_cov_d0_phi_wrtPV;   //!
   TBranch        *b_trig_bjefid_cov_d0_theta_wrtPV;   //!
   TBranch        *b_trig_bjefid_cov_d0_qoverp_wrtPV;   //!
   TBranch        *b_trig_bjefid_cov_z0_phi_wrtPV;   //!
   TBranch        *b_trig_bjefid_cov_z0_theta_wrtPV;   //!
   TBranch        *b_trig_bjefid_cov_z0_qoverp_wrtPV;   //!
   TBranch        *b_trig_bjefid_cov_phi_theta_wrtPV;   //!
   TBranch        *b_trig_bjefid_cov_phi_qoverp_wrtPV;   //!
   TBranch        *b_trig_bjefid_cov_theta_qoverp_wrtPV;   //!
   TBranch        *b_trig_bjefid_d0_wrtBS;   //!
   TBranch        *b_trig_bjefid_z0_wrtBS;   //!
   TBranch        *b_trig_bjefid_phi_wrtBS;   //!
   TBranch        *b_trig_bjefid_err_d0_wrtBS;   //!
   TBranch        *b_trig_bjefid_err_z0_wrtBS;   //!
   TBranch        *b_trig_bjefid_err_phi_wrtBS;   //!
   TBranch        *b_trig_bjefid_err_theta_wrtBS;   //!
   TBranch        *b_trig_bjefid_err_qoverp_wrtBS;   //!
   TBranch        *b_trig_bjefid_cov_d0_z0_wrtBS;   //!
   TBranch        *b_trig_bjefid_cov_d0_phi_wrtBS;   //!
   TBranch        *b_trig_bjefid_cov_d0_theta_wrtBS;   //!
   TBranch        *b_trig_bjefid_cov_d0_qoverp_wrtBS;   //!
   TBranch        *b_trig_bjefid_cov_z0_phi_wrtBS;   //!
   TBranch        *b_trig_bjefid_cov_z0_theta_wrtBS;   //!
   TBranch        *b_trig_bjefid_cov_z0_qoverp_wrtBS;   //!
   TBranch        *b_trig_bjefid_cov_phi_theta_wrtBS;   //!
   TBranch        *b_trig_bjefid_cov_phi_qoverp_wrtBS;   //!
   TBranch        *b_trig_bjefid_cov_theta_qoverp_wrtBS;   //!
   TBranch        *b_trig_bjefid_chi2;   //!
   TBranch        *b_trig_bjefid_ndof;   //!
   TBranch        *b_trig_bjefid_nBLHits;   //!
   TBranch        *b_trig_bjefid_nPixHits;   //!
   TBranch        *b_trig_bjefid_nSCTHits;   //!
   TBranch        *b_trig_bjefid_nTRTHits;   //!
   TBranch        *b_trig_bjefid_nPixHoles;   //!
   TBranch        *b_trig_bjefid_nSCTHoles;   //!
   TBranch        *b_trig_bjefid_nMDTHits;   //!
   TBranch        *b_trig_bjefid_nCSCEtaHits;   //!
   TBranch        *b_trig_bjefid_nCSCPhiHits;   //!
   TBranch        *b_trig_bjefid_nRPCEtaHits;   //!
   TBranch        *b_trig_bjefid_nRPCPhiHits;   //!
   TBranch        *b_trig_bjefid_nTGCEtaHits;   //!
   TBranch        *b_trig_bjefid_nTGCPhiHits;   //!
   TBranch        *b_trig_bjefid_nHits;   //!
   TBranch        *b_trig_bjefid_nHoles;   //!
   TBranch        *b_trig_bjefid_hitPattern;   //!
   TBranch        *b_trig_bjefid_TRTHighTHitsRatio;   //!
   TBranch        *b_trig_bjefid_pixeldEdx;   //!
   TBranch        *b_trig_bjefid_fitter;   //!
   TBranch        *b_trig_bjefid_patternReco1;   //!
   TBranch        *b_trig_bjefid_patternReco2;   //!
   TBranch        *b_trig_bjefid_seedFinder;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_expectHit;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_x;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_y;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_z;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_locX;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_locY;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_err_locX;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_err_locY;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_etaDistToEdge;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_phiDistToEdge;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_detElementId;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_row;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_col;   //!
   TBranch        *b_trig_bjefid_blayerPrediction_type;   //!
   TBranch        *b_trig_bjefid_mc_probability;   //!
   TBranch        *b_trig_bjefid_mc_barcode;   //!
   TBranch        *b_trig_bjefid_mc_index;   //!
   TBranch        *b_trig_bjefid_vertx;   //!
   TBranch        *b_trig_bjefid_verty;   //!
   TBranch        *b_trig_bjefid_vertz;   //!
   TBranch        *b_trig_l2bj_n;   //!
   TBranch        *b_trig_l2bj_roiId;   //!
   TBranch        *b_trig_l2bj_valid;   //!
   TBranch        *b_trig_l2bj_prmVtx;   //!
   TBranch        *b_trig_l2bj_xComb;   //!
   TBranch        *b_trig_l2bj_xIP1D;   //!
   TBranch        *b_trig_l2bj_xIP2D;   //!
   TBranch        *b_trig_l2bj_xIP3D;   //!
   TBranch        *b_trig_l2bj_xCHI2;   //!
   TBranch        *b_trig_l2bj_xSV;   //!
   TBranch        *b_trig_l2bj_xMVtx;   //!
   TBranch        *b_trig_l2bj_xEVtx;   //!
   TBranch        *b_trig_l2bj_xNVtx;   //!
   TBranch        *b_trig_l2bj_eta;   //!
   TBranch        *b_trig_l2bj_phi;   //!
   TBranch        *b_trig_l2bj_trksi_n;   //!
   TBranch        *b_trig_l2bj_trksi_index;   //!
   TBranch        *b_trig_l2bj_trkid_n;   //!
   TBranch        *b_trig_l2bj_trkid_index;   //!
   TBranch        *b_trig_efbj_n;   //!
   TBranch        *b_trig_efbj_roiId;   //!
   TBranch        *b_trig_efbj_valid;   //!
   TBranch        *b_trig_efbj_prmVtx;   //!
   TBranch        *b_trig_efbj_xComb;   //!
   TBranch        *b_trig_efbj_xIP1D;   //!
   TBranch        *b_trig_efbj_xIP2D;   //!
   TBranch        *b_trig_efbj_xIP3D;   //!
   TBranch        *b_trig_efbj_xCHI2;   //!
   TBranch        *b_trig_efbj_xSV;   //!
   TBranch        *b_trig_efbj_xMVtx;   //!
   TBranch        *b_trig_efbj_xEVtx;   //!
   TBranch        *b_trig_efbj_xNVtx;   //!
   TBranch        *b_trig_efbj_eta;   //!
   TBranch        *b_trig_efbj_phi;   //!
   TBranch        *b_trig_efbj_trk_n;   //!
   TBranch        *b_trig_efbj_trk_index;   //!
   TBranch        *b_el_n;   //!
   TBranch        *b_el_E;   //!
   TBranch        *b_el_Et;   //!
   TBranch        *b_el_pt;   //!
   TBranch        *b_el_m;   //!
   TBranch        *b_el_eta;   //!
   TBranch        *b_el_phi;   //!
   TBranch        *b_el_px;   //!
   TBranch        *b_el_py;   //!
   TBranch        *b_el_pz;   //!
   TBranch        *b_el_charge;   //!
   TBranch        *b_el_author;   //!
   TBranch        *b_el_isEM;   //!
   TBranch        *b_el_convFlag;   //!
   TBranch        *b_el_isConv;   //!
   TBranch        *b_el_nConv;   //!
   TBranch        *b_el_nSingleTrackConv;   //!
   TBranch        *b_el_nDoubleTrackConv;   //!
   TBranch        *b_el_type;   //!
   TBranch        *b_el_origin;   //!
   TBranch        *b_el_truth_E;   //!
   TBranch        *b_el_truth_pt;   //!
   TBranch        *b_el_truth_eta;   //!
   TBranch        *b_el_truth_phi;   //!
   TBranch        *b_el_truth_type;   //!
   TBranch        *b_el_truth_status;   //!
   TBranch        *b_el_truth_barcode;   //!
   TBranch        *b_el_truth_mothertype;   //!
   TBranch        *b_el_truth_motherbarcode;   //!
   TBranch        *b_el_truth_hasHardBrem;   //!
   TBranch        *b_el_truth_matched;   //!
   TBranch        *b_el_loose;   //!
   TBranch        *b_el_medium;   //!
   TBranch        *b_el_mediumIso;   //!
   TBranch        *b_el_tight;   //!
   TBranch        *b_el_tightIso;   //!
   TBranch        *b_el_Ethad;   //!
   TBranch        *b_el_Ethad1;   //!
   TBranch        *b_el_f1;   //!
   TBranch        *b_el_f1core;   //!
   TBranch        *b_el_Emins1;   //!
   TBranch        *b_el_fside;   //!
   TBranch        *b_el_Emax2;   //!
   TBranch        *b_el_ws3;   //!
   TBranch        *b_el_wstot;   //!
   TBranch        *b_el_emaxs1;   //!
   TBranch        *b_el_deltaEs;   //!
   TBranch        *b_el_E233;   //!
   TBranch        *b_el_E237;   //!
   TBranch        *b_el_E277;   //!
   TBranch        *b_el_weta2;   //!
   TBranch        *b_el_f3;   //!
   TBranch        *b_el_f3core;   //!
   TBranch        *b_el_rphiallcalo;   //!
   TBranch        *b_el_Etcone45;   //!
   TBranch        *b_el_Etcone20;   //!
   TBranch        *b_el_Etcone30;   //!
   TBranch        *b_el_Etcone40;   //!
   TBranch        *b_el_ptcone30;   //!
   TBranch        *b_el_convanglematch;   //!
   TBranch        *b_el_convtrackmatch;   //!
   TBranch        *b_el_hasconv;   //!
   TBranch        *b_el_convvtxx;   //!
   TBranch        *b_el_convvtxy;   //!
   TBranch        *b_el_convvtxz;   //!
   TBranch        *b_el_Rconv;   //!
   TBranch        *b_el_zconv;   //!
   TBranch        *b_el_convvtxchi2;   //!
   TBranch        *b_el_pt1conv;   //!
   TBranch        *b_el_convtrk1nBLHits;   //!
   TBranch        *b_el_convtrk1nPixHits;   //!
   TBranch        *b_el_convtrk1nSCTHits;   //!
   TBranch        *b_el_convtrk1nTRTHits;   //!
   TBranch        *b_el_pt2conv;   //!
   TBranch        *b_el_convtrk2nBLHits;   //!
   TBranch        *b_el_convtrk2nPixHits;   //!
   TBranch        *b_el_convtrk2nSCTHits;   //!
   TBranch        *b_el_convtrk2nTRTHits;   //!
   TBranch        *b_el_ptconv;   //!
   TBranch        *b_el_pzconv;   //!
   TBranch        *b_el_pos7;   //!
   TBranch        *b_el_etacorrmag;   //!
   TBranch        *b_el_deltaeta1;   //!
   TBranch        *b_el_deltaeta2;   //!
   TBranch        *b_el_deltaphi2;   //!
   TBranch        *b_el_deltaphiRescaled;   //!
   TBranch        *b_el_expectHitInBLayer;   //!
   TBranch        *b_el_reta;   //!
   TBranch        *b_el_rphi;   //!
   TBranch        *b_el_EtringnoisedR03sig2;   //!
   TBranch        *b_el_EtringnoisedR03sig3;   //!
   TBranch        *b_el_EtringnoisedR03sig4;   //!
   TBranch        *b_el_isolationlikelihoodjets;   //!
   TBranch        *b_el_isolationlikelihoodhqelectrons;   //!
   TBranch        *b_el_electronweight;   //!
   TBranch        *b_el_electronbgweight;   //!
   TBranch        *b_el_softeweight;   //!
   TBranch        *b_el_softebgweight;   //!
   TBranch        *b_el_neuralnet;   //!
   TBranch        *b_el_Hmatrix;   //!
   TBranch        *b_el_Hmatrix5;   //!
   TBranch        *b_el_adaboost;   //!
   TBranch        *b_el_softeneuralnet;   //!
   TBranch        *b_el_zvertex;   //!
   TBranch        *b_el_errz;   //!
   TBranch        *b_el_etap;   //!
   TBranch        *b_el_depth;   //!
   TBranch        *b_el_refittedTrack_n;   //!
   TBranch        *b_el_refittedTrack_author;   //!
   TBranch        *b_el_refittedTrack_chi2;   //!
   TBranch        *b_el_refittedTrack_qoverp;   //!
   TBranch        *b_el_refittedTrack_d0;   //!
   TBranch        *b_el_refittedTrack_z0;   //!
   TBranch        *b_el_refittedTrack_theta;   //!
   TBranch        *b_el_refittedTrack_phi;   //!
   TBranch        *b_el_refittedTrack_LMqoverp;   //!
   TBranch        *b_el_refittedTrack_hasBrem;   //!
   TBranch        *b_el_refittedTrack_bremRadius;   //!
   TBranch        *b_el_refittedTrack_bremZ;   //!
   TBranch        *b_el_refittedTrack_bremRadiusErr;   //!
   TBranch        *b_el_refittedTrack_bremZErr;   //!
   TBranch        *b_el_Es0;   //!
   TBranch        *b_el_etas0;   //!
   TBranch        *b_el_phis0;   //!
   TBranch        *b_el_Es1;   //!
   TBranch        *b_el_etas1;   //!
   TBranch        *b_el_phis1;   //!
   TBranch        *b_el_Es2;   //!
   TBranch        *b_el_etas2;   //!
   TBranch        *b_el_phis2;   //!
   TBranch        *b_el_Es3;   //!
   TBranch        *b_el_etas3;   //!
   TBranch        *b_el_phis3;   //!
   TBranch        *b_el_E_PreSamplerB;   //!
   TBranch        *b_el_E_EMB1;   //!
   TBranch        *b_el_E_EMB2;   //!
   TBranch        *b_el_E_EMB3;   //!
   TBranch        *b_el_E_PreSamplerE;   //!
   TBranch        *b_el_E_EME1;   //!
   TBranch        *b_el_E_EME2;   //!
   TBranch        *b_el_E_EME3;   //!
   TBranch        *b_el_E_HEC0;   //!
   TBranch        *b_el_E_HEC1;   //!
   TBranch        *b_el_E_HEC2;   //!
   TBranch        *b_el_E_HEC3;   //!
   TBranch        *b_el_E_TileBar0;   //!
   TBranch        *b_el_E_TileBar1;   //!
   TBranch        *b_el_E_TileBar2;   //!
   TBranch        *b_el_E_TileGap1;   //!
   TBranch        *b_el_E_TileGap2;   //!
   TBranch        *b_el_E_TileGap3;   //!
   TBranch        *b_el_E_TileExt0;   //!
   TBranch        *b_el_E_TileExt1;   //!
   TBranch        *b_el_E_TileExt2;   //!
   TBranch        *b_el_E_FCAL0;   //!
   TBranch        *b_el_E_FCAL1;   //!
   TBranch        *b_el_E_FCAL2;   //!
   TBranch        *b_el_cl_E;   //!
   TBranch        *b_el_cl_pt;   //!
   TBranch        *b_el_cl_eta;   //!
   TBranch        *b_el_cl_phi;   //!
   TBranch        *b_el_firstEdens;   //!
   TBranch        *b_el_cellmaxfrac;   //!
   TBranch        *b_el_longitudinal;   //!
   TBranch        *b_el_secondlambda;   //!
   TBranch        *b_el_lateral;   //!
   TBranch        *b_el_secondR;   //!
   TBranch        *b_el_centerlambda;   //!
   TBranch        *b_el_rawcl_Es0;   //!
   TBranch        *b_el_rawcl_etas0;   //!
   TBranch        *b_el_rawcl_phis0;   //!
   TBranch        *b_el_rawcl_Es1;   //!
   TBranch        *b_el_rawcl_etas1;   //!
   TBranch        *b_el_rawcl_phis1;   //!
   TBranch        *b_el_rawcl_Es2;   //!
   TBranch        *b_el_rawcl_etas2;   //!
   TBranch        *b_el_rawcl_phis2;   //!
   TBranch        *b_el_rawcl_Es3;   //!
   TBranch        *b_el_rawcl_etas3;   //!
   TBranch        *b_el_rawcl_phis3;   //!
   TBranch        *b_el_rawcl_E;   //!
   TBranch        *b_el_rawcl_pt;   //!
   TBranch        *b_el_rawcl_eta;   //!
   TBranch        *b_el_rawcl_phi;   //!
   TBranch        *b_el_refittedtrackcovd0;   //!
   TBranch        *b_el_refittedtrackcovz0;   //!
   TBranch        *b_el_refittedtrackcovphi;   //!
   TBranch        *b_el_refittedtrackcovtheta;   //!
   TBranch        *b_el_refittedtrackcovqoverp;   //!
   TBranch        *b_el_refittedtrackcovd0z0;   //!
   TBranch        *b_el_refittedtrackcovz0phi;   //!
   TBranch        *b_el_refittedtrackcovz0theta;   //!
   TBranch        *b_el_refittedtrackcovz0qoverp;   //!
   TBranch        *b_el_refittedtrackcovd0phi;   //!
   TBranch        *b_el_refittedtrackcovd0theta;   //!
   TBranch        *b_el_refittedtrackcovd0qoverp;   //!
   TBranch        *b_el_refittedtrackcovphitheta;   //!
   TBranch        *b_el_refittedtrackcovphiqoverp;   //!
   TBranch        *b_el_refittedtrackcovthetaqoverp;   //!
   TBranch        *b_el_trackd0;   //!
   TBranch        *b_el_trackz0;   //!
   TBranch        *b_el_trackphi;   //!
   TBranch        *b_el_tracktheta;   //!
   TBranch        *b_el_trackqoverp;   //!
   TBranch        *b_el_trackpt;   //!
   TBranch        *b_el_tracketa;   //!
   TBranch        *b_el_trackcov_d0;   //!
   TBranch        *b_el_trackcov_z0;   //!
   TBranch        *b_el_trackcov_phi;   //!
   TBranch        *b_el_trackcov_theta;   //!
   TBranch        *b_el_trackcov_qoverp;   //!
   TBranch        *b_el_trackcov_d0_z0;   //!
   TBranch        *b_el_trackcov_d0_phi;   //!
   TBranch        *b_el_trackcov_d0_theta;   //!
   TBranch        *b_el_trackcov_d0_qoverp;   //!
   TBranch        *b_el_trackcov_z0_phi;   //!
   TBranch        *b_el_trackcov_z0_theta;   //!
   TBranch        *b_el_trackcov_z0_qoverp;   //!
   TBranch        *b_el_trackcov_phi_theta;   //!
   TBranch        *b_el_trackcov_phi_qoverp;   //!
   TBranch        *b_el_trackcov_theta_qoverp;   //!
   TBranch        *b_el_trackfitchi2;   //!
   TBranch        *b_el_trackfitndof;   //!
   TBranch        *b_el_nBLHits;   //!
   TBranch        *b_el_nPixHits;   //!
   TBranch        *b_el_nSCTHits;   //!
   TBranch        *b_el_nTRTHits;   //!
   TBranch        *b_el_nPixHoles;   //!
   TBranch        *b_el_nSCTHoles;   //!
   TBranch        *b_el_nBLSharedHits;   //!
   TBranch        *b_el_nPixSharedHits;   //!
   TBranch        *b_el_nSCTSharedHits;   //!
   TBranch        *b_el_nTRTHighTHits;   //!
   TBranch        *b_el_nTRTOutliers;   //!
   TBranch        *b_el_nTRTHighTOutliers;   //!
   TBranch        *b_el_nSiHits;   //!
   TBranch        *b_el_TRTHighTHitsRatio;   //!
   TBranch        *b_el_pixeldEdx;   //!
   TBranch        *b_el_eProbabilityComb;   //!
   TBranch        *b_el_eProbabilityHT;   //!
   TBranch        *b_el_eProbabilityToT;   //!
   TBranch        *b_el_eProbabilityBrem;   //!
   TBranch        *b_el_vertx;   //!
   TBranch        *b_el_verty;   //!
   TBranch        *b_el_vertz;   //!
   TBranch        *b_el_hastrack;   //!
   TBranch        *b_el_deltaEmax2;   //!
   TBranch        *b_el_calibHitsShowerDepth;   //!
   TBranch        *b_el_trackd0beam;   //!
   TBranch        *b_el_tracksigd0beam;   //!
   TBranch        *b_el_trackd0pv;   //!
   TBranch        *b_el_tracksigd0pv;   //!
   TBranch        *b_el_trackz0pv;   //!
   TBranch        *b_el_tracksigz0pv;   //!
   TBranch        *b_el_trackd0pvunbiased;   //!
   TBranch        *b_el_tracksigd0pvunbiased;   //!
   TBranch        *b_el_trackz0pvunbiased;   //!
   TBranch        *b_el_tracksigz0pvunbiased;   //!
   TBranch        *b_el_isIso;   //!
   TBranch        *b_el_mvaptcone20;   //!
   TBranch        *b_el_mvaptcone30;   //!
   TBranch        *b_el_mvaptcone40;   //!
   TBranch        *b_el_jet_dr;   //!
   TBranch        *b_el_jet_E;   //!
   TBranch        *b_el_jet_pt;   //!
   TBranch        *b_el_jet_m;   //!
   TBranch        *b_el_jet_eta;   //!
   TBranch        *b_el_jet_phi;   //!
   TBranch        *b_el_jet_truth_dr;   //!
   TBranch        *b_el_jet_truth_E;   //!
   TBranch        *b_el_jet_truth_pt;   //!
   TBranch        *b_el_jet_truth_m;   //!
   TBranch        *b_el_jet_truth_eta;   //!
   TBranch        *b_el_jet_truth_phi;   //!
   TBranch        *b_el_jet_truth_matched;   //!
   TBranch        *b_el_jet_matched;   //!
   TBranch        *b_el_Etcone40_pt_corrected;   //!
   TBranch        *b_el_Etcone40_ED_corrected;   //!
   TBranch        *b_el_Etcone40_corrected;   //!
   TBranch        *b_el_EF_dr;   //!
   TBranch        *b_el_EF_index;   //!
   TBranch        *b_el_L2_dr;   //!
   TBranch        *b_el_L2_index;   //!
   TBranch        *b_el_L1_dr;   //!
   TBranch        *b_el_L1_index;   //!
   TBranch        *b_EF_2e12_tight;   //!
   TBranch        *b_EF_2e15_medium;   //!
   TBranch        *b_EF_2e20_loose;   //!
   TBranch        *b_EF_2e5_medium;   //!
   TBranch        *b_EF_2e5_medium1;   //!
   TBranch        *b_EF_2g10_loose;   //!
   TBranch        *b_EF_2g10_loose_mu6;   //!
   TBranch        *b_EF_2g17i_tight;   //!
   TBranch        *b_EF_2g20_loose;   //!
   TBranch        *b_EF_2mu10;   //!
   TBranch        *b_EF_2mu20;   //!
   TBranch        *b_EF_2mu4;   //!
   TBranch        *b_EF_2mu4_Bmumu;   //!
   TBranch        *b_EF_2mu4_Bmumux;   //!
   TBranch        *b_EF_2mu4_DiMu;   //!
   TBranch        *b_EF_2mu4_DiMu_SiTrk;   //!
   TBranch        *b_EF_2mu4_DiMu_noVtx;   //!
   TBranch        *b_EF_2mu4_DiMu_noVtx_noOS;   //!
   TBranch        *b_EF_2mu4_Jpsimumu;   //!
   TBranch        *b_EF_2mu4_Upsimumu;   //!
   TBranch        *b_EF_2mu6;   //!
   TBranch        *b_EF_2mu6_Bmumu;   //!
   TBranch        *b_EF_2mu6_Bmumux;   //!
   TBranch        *b_EF_2mu6_DiMu;   //!
   TBranch        *b_EF_2mu6_Jpsimumu;   //!
   TBranch        *b_EF_2mu6_Upsimumu;   //!
   TBranch        *b_EF_2mu6_e10_loose;   //!
   TBranch        *b_EF_2mu6_g10_loose;   //!
   TBranch        *b_EF_e105_loose1;   //!
   TBranch        *b_EF_e10_NoCut;   //!
   TBranch        *b_EF_e10_loose;   //!
   TBranch        *b_EF_e10_loose_IdScan;   //!
   TBranch        *b_EF_e10_loose_SiTrk;   //!
   TBranch        *b_EF_e10_loose_TRT;   //!
   TBranch        *b_EF_e10_loose_mu6;   //!
   TBranch        *b_EF_e10_medium;   //!
   TBranch        *b_EF_e10_medium_SiTrk;   //!
   TBranch        *b_EF_e10_medium_SiTrk_robust;   //!
   TBranch        *b_EF_e12_medium;   //!
   TBranch        *b_EF_e15_medium;   //!
   TBranch        *b_EF_e15i_medium;   //!
   TBranch        *b_EF_e20_loose;   //!
   TBranch        *b_EF_e20_loose_IdScan;   //!
   TBranch        *b_EF_e20_loose_NoIDTrkCut;   //!
   TBranch        *b_EF_e20_loose_SiTrk;   //!
   TBranch        *b_EF_e20_loose_SiTrk_robust;   //!
   TBranch        *b_EF_e20_loose_TRT;   //!
   TBranch        *b_EF_e20_loose_passEF;   //!
   TBranch        *b_EF_e20_loose_passL2;   //!
   TBranch        *b_EF_e20_loose_xe20;   //!
   TBranch        *b_EF_e20_loose_xe30;   //!
   TBranch        *b_EF_e20_medium;   //!
   TBranch        *b_EF_e20i_loose;   //!
   TBranch        *b_EF_e22i_tight;   //!
   TBranch        *b_EF_e25_loose;   //!
   TBranch        *b_EF_e25_medium;   //!
   TBranch        *b_EF_e25i_loose;   //!
   TBranch        *b_EF_e25i_medium;   //!
   TBranch        *b_EF_e55_loose1;   //!
   TBranch        *b_EF_e5_medium;   //!
   TBranch        *b_EF_e5_medium1;   //!
   TBranch        *b_EF_e5_medium_L2SW;   //!
   TBranch        *b_EF_e5_medium_MV;   //!
   TBranch        *b_EF_e5_medium_Ringer;   //!
   TBranch        *b_EF_e5_medium_e10_loose;   //!
   TBranch        *b_EF_em105_passHLT;   //!
   TBranch        *b_EF_em15_passHLT;   //!
   TBranch        *b_EF_em15i_passHLT;   //!
   TBranch        *b_EF_em20_passHLT;   //!
   TBranch        *b_EF_em20i_passHLT;   //!
   TBranch        *b_EF_g105;   //!
   TBranch        *b_EF_g10_loose;   //!
   TBranch        *b_EF_g10_loose_larcalib;   //!
   TBranch        *b_EF_g10_nocut;   //!
   TBranch        *b_EF_g150;   //!
   TBranch        *b_EF_g15_loose;   //!
   TBranch        *b_EF_g20_loose;   //!
   TBranch        *b_EF_g20_loose_cnv;   //!
   TBranch        *b_EF_g20_loose_larcalib;   //!
   TBranch        *b_EF_g20_loose_xe20;   //!
   TBranch        *b_EF_g20i_loose;   //!
   TBranch        *b_EF_g20i_loose_larcalib;   //!
   TBranch        *b_EF_g25_loose;   //!
   TBranch        *b_EF_g25_loose_xe30;   //!
   TBranch        *b_EF_g25i_loose;   //!
   TBranch        *b_EF_g55_tight;   //!
   TBranch        *b_EF_mu10;   //!
   TBranch        *b_EF_mu10_DsPhiPi;   //!
   TBranch        *b_EF_mu10_DsPhiPi_FS;   //!
   TBranch        *b_EF_mu10_MG;   //!
   TBranch        *b_EF_mu10_MSonly;   //!
   TBranch        *b_EF_mu10_MuTagIMO;   //!
   TBranch        *b_EF_mu10_NoIDTrkCut;   //!
   TBranch        *b_EF_mu10_SiTrk;   //!
   TBranch        *b_EF_mu10i_loose;   //!
   TBranch        *b_EF_mu13;   //!
   TBranch        *b_EF_mu15;   //!
   TBranch        *b_EF_mu15_xe20;   //!
   TBranch        *b_EF_mu15i_loose;   //!
   TBranch        *b_EF_mu20;   //!
   TBranch        *b_EF_mu20_MSonly;   //!
   TBranch        *b_EF_mu20_passHLT;   //!
   TBranch        *b_EF_mu20_slow;   //!
   TBranch        *b_EF_mu20_xe30;   //!
   TBranch        *b_EF_mu20i_loose;   //!
   TBranch        *b_EF_mu20i_loose_MSonly;   //!
   TBranch        *b_EF_mu4;   //!
   TBranch        *b_EF_mu4_Bmumu;   //!
   TBranch        *b_EF_mu4_BmumuX;   //!
   TBranch        *b_EF_mu4_Bmumu_FS;   //!
   TBranch        *b_EF_mu4_DiMu;   //!
   TBranch        *b_EF_mu4_DiMu_FS;   //!
   TBranch        *b_EF_mu4_DiMu_FS_noOS;   //!
   TBranch        *b_EF_mu4_DiMu_MG;   //!
   TBranch        *b_EF_mu4_DiMu_MG_FS;   //!
   TBranch        *b_EF_mu4_DiMu_SiTrk;   //!
   TBranch        *b_EF_mu4_DiMu_SiTrk_FS;   //!
   TBranch        *b_EF_mu4_DiMu_noOS;   //!
   TBranch        *b_EF_mu4_DsPhiPi;   //!
   TBranch        *b_EF_mu4_DsPhiPi_FS;   //!
   TBranch        *b_EF_mu4_Jpsie5e3;   //!
   TBranch        *b_EF_mu4_Jpsie5e3_FS;   //!
   TBranch        *b_EF_mu4_Jpsie5e3_SiTrk;   //!
   TBranch        *b_EF_mu4_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_EF_mu4_Jpsimumu;   //!
   TBranch        *b_EF_mu4_Jpsimumu_FS;   //!
   TBranch        *b_EF_mu4_MG;   //!
   TBranch        *b_EF_mu4_MSonly;   //!
   TBranch        *b_EF_mu4_MV;   //!
   TBranch        *b_EF_mu4_SiTrk;   //!
   TBranch        *b_EF_mu4_Trk_Jpsi;   //!
   TBranch        *b_EF_mu4_Trk_Jpsi_FS;   //!
   TBranch        *b_EF_mu4_Trk_Jpsi_loose;   //!
   TBranch        *b_EF_mu4_Trk_Upsi_FS;   //!
   TBranch        *b_EF_mu4_Trk_Upsi_loose_FS;   //!
   TBranch        *b_EF_mu4_Upsimumu;   //!
   TBranch        *b_EF_mu4_Upsimumu_FS;   //!
   TBranch        *b_EF_mu4_j10;   //!
   TBranch        *b_EF_mu4_j10_matched;   //!
   TBranch        *b_EF_mu4_j20;   //!
   TBranch        *b_EF_mu4_j20_matched;   //!
   TBranch        *b_EF_mu4_j40_matched;   //!
   TBranch        *b_EF_mu4_j70_matched;   //!
   TBranch        *b_EF_mu4_mu6;   //!
   TBranch        *b_EF_mu4_tile;   //!
   TBranch        *b_EF_mu4_trod;   //!
   TBranch        *b_EF_mu4mu6_Bmumu;   //!
   TBranch        *b_EF_mu4mu6_DiMu;   //!
   TBranch        *b_EF_mu4mu6_Jpsimumu;   //!
   TBranch        *b_EF_mu4mu6_Upsimumu;   //!
   TBranch        *b_EF_mu6;   //!
   TBranch        *b_EF_mu6_Bmumu;   //!
   TBranch        *b_EF_mu6_BmumuX;   //!
   TBranch        *b_EF_mu6_Bmumu_FS;   //!
   TBranch        *b_EF_mu6_DiMu;   //!
   TBranch        *b_EF_mu6_DiMu_FS;   //!
   TBranch        *b_EF_mu6_DsPhiPi;   //!
   TBranch        *b_EF_mu6_DsPhiPi_FS;   //!
   TBranch        *b_EF_mu6_Jpsie5e3;   //!
   TBranch        *b_EF_mu6_Jpsie5e3_FS;   //!
   TBranch        *b_EF_mu6_Jpsie5e3_SiTrk;   //!
   TBranch        *b_EF_mu6_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_EF_mu6_Jpsimumu;   //!
   TBranch        *b_EF_mu6_Jpsimumu_FS;   //!
   TBranch        *b_EF_mu6_MG;   //!
   TBranch        *b_EF_mu6_MSonly;   //!
   TBranch        *b_EF_mu6_Trk_Jpsi;   //!
   TBranch        *b_EF_mu6_Upsimumu_FS;   //!
   TBranch        *b_L1_2EM10;   //!
   TBranch        *b_L1_2EM10I;   //!
   TBranch        *b_L1_2EM14;   //!
   TBranch        *b_L1_2EM3;   //!
   TBranch        *b_L1_2EM3_EM5;   //!
   TBranch        *b_L1_2EM5;   //!
   TBranch        *b_L1_2EM5_MU6;   //!
   TBranch        *b_L1_2MU0;   //!
   TBranch        *b_L1_2MU0_MU6;   //!
   TBranch        *b_L1_2MU10;   //!
   TBranch        *b_L1_2MU20;   //!
   TBranch        *b_L1_2MU6;   //!
   TBranch        *b_L1_2MU6_EM5;   //!
   TBranch        *b_L1_EM10;   //!
   TBranch        *b_L1_EM10I;   //!
   TBranch        *b_L1_EM14;   //!
   TBranch        *b_L1_EM14I;   //!
   TBranch        *b_L1_EM14_XE10;   //!
   TBranch        *b_L1_EM14_XE15;   //!
   TBranch        *b_L1_EM19I;   //!
   TBranch        *b_L1_EM3;   //!
   TBranch        *b_L1_EM3_MV;   //!
   TBranch        *b_L1_EM5;   //!
   TBranch        *b_L1_EM5_MU6;   //!
   TBranch        *b_L1_EM85;   //!
   TBranch        *b_L1_MU0;   //!
   TBranch        *b_L1_MU0_COMM;   //!
   TBranch        *b_L1_MU0_EM3;   //!
   TBranch        *b_L1_MU0_J10;   //!
   TBranch        *b_L1_MU0_J15;   //!
   TBranch        *b_L1_MU0_J30;   //!
   TBranch        *b_L1_MU0_J5;   //!
   TBranch        *b_L1_MU0_J55;   //!
   TBranch        *b_L1_MU0_MV;   //!
   TBranch        *b_L1_MU10;   //!
   TBranch        *b_L1_MU10_J5;   //!
   TBranch        *b_L1_MU15;   //!
   TBranch        *b_L1_MU15_XE10;   //!
   TBranch        *b_L1_MU20;   //!
   TBranch        *b_L1_MU20_XE15;   //!
   TBranch        *b_L1_MU6;   //!
   TBranch        *b_L1_MU6_EM3;   //!
   TBranch        *b_L1_MU6_J5;   //!
   TBranch        *b_L2_2e12_tight;   //!
   TBranch        *b_L2_2e15_medium;   //!
   TBranch        *b_L2_2e20_loose;   //!
   TBranch        *b_L2_2e5_medium;   //!
   TBranch        *b_L2_2e5_medium1;   //!
   TBranch        *b_L2_2g10_loose;   //!
   TBranch        *b_L2_2g10_loose_mu6;   //!
   TBranch        *b_L2_2g17i_tight;   //!
   TBranch        *b_L2_2g20_loose;   //!
   TBranch        *b_L2_2mu10;   //!
   TBranch        *b_L2_2mu20;   //!
   TBranch        *b_L2_2mu4;   //!
   TBranch        *b_L2_2mu4_Bmumu;   //!
   TBranch        *b_L2_2mu4_Bmumux;   //!
   TBranch        *b_L2_2mu4_DiMu;   //!
   TBranch        *b_L2_2mu4_DiMu_SiTrk;   //!
   TBranch        *b_L2_2mu4_DiMu_noVtx;   //!
   TBranch        *b_L2_2mu4_DiMu_noVtx_noOS;   //!
   TBranch        *b_L2_2mu4_Jpsimumu;   //!
   TBranch        *b_L2_2mu4_Upsimumu;   //!
   TBranch        *b_L2_2mu6;   //!
   TBranch        *b_L2_2mu6_Bmumu;   //!
   TBranch        *b_L2_2mu6_Bmumux;   //!
   TBranch        *b_L2_2mu6_DiMu;   //!
   TBranch        *b_L2_2mu6_Jpsimumu;   //!
   TBranch        *b_L2_2mu6_Upsimumu;   //!
   TBranch        *b_L2_2mu6_e10_loose;   //!
   TBranch        *b_L2_2mu6_g10_loose;   //!
   TBranch        *b_L2_e105_loose1;   //!
   TBranch        *b_L2_e10_NoCut;   //!
   TBranch        *b_L2_e10_loose;   //!
   TBranch        *b_L2_e10_loose_IdScan;   //!
   TBranch        *b_L2_e10_loose_SiTrk;   //!
   TBranch        *b_L2_e10_loose_TRT;   //!
   TBranch        *b_L2_e10_loose_mu6;   //!
   TBranch        *b_L2_e10_medium;   //!
   TBranch        *b_L2_e10_medium_SiTrk;   //!
   TBranch        *b_L2_e10_medium_SiTrk_robust;   //!
   TBranch        *b_L2_e12_medium;   //!
   TBranch        *b_L2_e15_medium;   //!
   TBranch        *b_L2_e15i_medium;   //!
   TBranch        *b_L2_e20_loose;   //!
   TBranch        *b_L2_e20_loose_IdScan;   //!
   TBranch        *b_L2_e20_loose_NoIDTrkCut;   //!
   TBranch        *b_L2_e20_loose_SiTrk;   //!
   TBranch        *b_L2_e20_loose_SiTrk_robust;   //!
   TBranch        *b_L2_e20_loose_TRT;   //!
   TBranch        *b_L2_e20_loose_passEF;   //!
   TBranch        *b_L2_e20_loose_passL2;   //!
   TBranch        *b_L2_e20_loose_xe20;   //!
   TBranch        *b_L2_e20_loose_xe30;   //!
   TBranch        *b_L2_e20_medium;   //!
   TBranch        *b_L2_e20i_loose;   //!
   TBranch        *b_L2_e22i_tight;   //!
   TBranch        *b_L2_e25_loose;   //!
   TBranch        *b_L2_e25_medium;   //!
   TBranch        *b_L2_e25i_loose;   //!
   TBranch        *b_L2_e25i_medium;   //!
   TBranch        *b_L2_e55_loose1;   //!
   TBranch        *b_L2_e5_medium;   //!
   TBranch        *b_L2_e5_medium1;   //!
   TBranch        *b_L2_e5_medium_L2SW;   //!
   TBranch        *b_L2_e5_medium_MV;   //!
   TBranch        *b_L2_e5_medium_Ringer;   //!
   TBranch        *b_L2_e5_medium_e10_loose;   //!
   TBranch        *b_L2_em105_passHLT;   //!
   TBranch        *b_L2_em15_passHLT;   //!
   TBranch        *b_L2_em15i_passHLT;   //!
   TBranch        *b_L2_em20_passHLT;   //!
   TBranch        *b_L2_em20i_passHLT;   //!
   TBranch        *b_L2_g105;   //!
   TBranch        *b_L2_g10_loose;   //!
   TBranch        *b_L2_g10_nocut;   //!
   TBranch        *b_L2_g150;   //!
   TBranch        *b_L2_g15_loose;   //!
   TBranch        *b_L2_g20_loose;   //!
   TBranch        *b_L2_g20_loose_cnv;   //!
   TBranch        *b_L2_g20_loose_xe20;   //!
   TBranch        *b_L2_g20i_loose;   //!
   TBranch        *b_L2_g25_loose;   //!
   TBranch        *b_L2_g25_loose_xe30;   //!
   TBranch        *b_L2_g25i_loose;   //!
   TBranch        *b_L2_g55_tight;   //!
   TBranch        *b_L2_mu10;   //!
   TBranch        *b_L2_mu10_DsPhiPi;   //!
   TBranch        *b_L2_mu10_DsPhiPi_FS;   //!
   TBranch        *b_L2_mu10_MG;   //!
   TBranch        *b_L2_mu10_MSonly;   //!
   TBranch        *b_L2_mu10_MuTagIMO;   //!
   TBranch        *b_L2_mu10_NoIDTrkCut;   //!
   TBranch        *b_L2_mu10_SiTrk;   //!
   TBranch        *b_L2_mu10i_loose;   //!
   TBranch        *b_L2_mu13;   //!
   TBranch        *b_L2_mu15;   //!
   TBranch        *b_L2_mu15_xe20;   //!
   TBranch        *b_L2_mu15i_loose;   //!
   TBranch        *b_L2_mu20;   //!
   TBranch        *b_L2_mu20_MSonly;   //!
   TBranch        *b_L2_mu20_passHLT;   //!
   TBranch        *b_L2_mu20_slow;   //!
   TBranch        *b_L2_mu20_xe30;   //!
   TBranch        *b_L2_mu20i_loose;   //!
   TBranch        *b_L2_mu20i_loose_MSonly;   //!
   TBranch        *b_L2_mu4;   //!
   TBranch        *b_L2_mu4_Bmumu;   //!
   TBranch        *b_L2_mu4_BmumuX;   //!
   TBranch        *b_L2_mu4_Bmumu_FS;   //!
   TBranch        *b_L2_mu4_DiMu;   //!
   TBranch        *b_L2_mu4_DiMu_FS;   //!
   TBranch        *b_L2_mu4_DiMu_FS_noOS;   //!
   TBranch        *b_L2_mu4_DiMu_MG;   //!
   TBranch        *b_L2_mu4_DiMu_MG_FS;   //!
   TBranch        *b_L2_mu4_DiMu_SiTrk;   //!
   TBranch        *b_L2_mu4_DiMu_SiTrk_FS;   //!
   TBranch        *b_L2_mu4_DiMu_noOS;   //!
   TBranch        *b_L2_mu4_DsPhiPi;   //!
   TBranch        *b_L2_mu4_DsPhiPi_FS;   //!
   TBranch        *b_L2_mu4_Jpsie5e3;   //!
   TBranch        *b_L2_mu4_Jpsie5e3_FS;   //!
   TBranch        *b_L2_mu4_Jpsie5e3_SiTrk;   //!
   TBranch        *b_L2_mu4_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_L2_mu4_Jpsimumu;   //!
   TBranch        *b_L2_mu4_Jpsimumu_FS;   //!
   TBranch        *b_L2_mu4_MG;   //!
   TBranch        *b_L2_mu4_MSonly;   //!
   TBranch        *b_L2_mu4_MV;   //!
   TBranch        *b_L2_mu4_SiTrk;   //!
   TBranch        *b_L2_mu4_Trk_Jpsi;   //!
   TBranch        *b_L2_mu4_Trk_Jpsi_FS;   //!
   TBranch        *b_L2_mu4_Trk_Jpsi_loose;   //!
   TBranch        *b_L2_mu4_Trk_Upsi_FS;   //!
   TBranch        *b_L2_mu4_Trk_Upsi_loose_FS;   //!
   TBranch        *b_L2_mu4_Upsimumu;   //!
   TBranch        *b_L2_mu4_Upsimumu_FS;   //!
   TBranch        *b_L2_mu4_j10;   //!
   TBranch        *b_L2_mu4_j10_matched;   //!
   TBranch        *b_L2_mu4_j15;   //!
   TBranch        *b_L2_mu4_j20_matched;   //!
   TBranch        *b_L2_mu4_j40_matched;   //!
   TBranch        *b_L2_mu4_j70_matched;   //!
   TBranch        *b_L2_mu4_mu6;   //!
   TBranch        *b_L2_mu4_tile;   //!
   TBranch        *b_L2_mu4_trod;   //!
   TBranch        *b_L2_mu4mu6_Bmumu;   //!
   TBranch        *b_L2_mu4mu6_DiMu;   //!
   TBranch        *b_L2_mu4mu6_Jpsimumu;   //!
   TBranch        *b_L2_mu4mu6_Upsimumu;   //!
   TBranch        *b_L2_mu6;   //!
   TBranch        *b_L2_mu6_Bmumu;   //!
   TBranch        *b_L2_mu6_BmumuX;   //!
   TBranch        *b_L2_mu6_Bmumu_FS;   //!
   TBranch        *b_L2_mu6_DiMu;   //!
   TBranch        *b_L2_mu6_DiMu_FS;   //!
   TBranch        *b_L2_mu6_DsPhiPi;   //!
   TBranch        *b_L2_mu6_DsPhiPi_FS;   //!
   TBranch        *b_L2_mu6_Jpsie5e3;   //!
   TBranch        *b_L2_mu6_Jpsie5e3_FS;   //!
   TBranch        *b_L2_mu6_Jpsie5e3_SiTrk;   //!
   TBranch        *b_L2_mu6_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_L2_mu6_Jpsimumu;   //!
   TBranch        *b_L2_mu6_Jpsimumu_FS;   //!
   TBranch        *b_L2_mu6_MG;   //!
   TBranch        *b_L2_mu6_MSonly;   //!
   TBranch        *b_L2_mu6_Trk_Jpsi;   //!
   TBranch        *b_L2_mu6_Upsimumu_FS;   //!
   TBranch        *b_ph_n;   //!
   TBranch        *b_ph_E;   //!
   TBranch        *b_ph_Et;   //!
   TBranch        *b_ph_pt;   //!
   TBranch        *b_ph_m;   //!
   TBranch        *b_ph_eta;   //!
   TBranch        *b_ph_phi;   //!
   TBranch        *b_ph_px;   //!
   TBranch        *b_ph_py;   //!
   TBranch        *b_ph_pz;   //!
   TBranch        *b_ph_author;   //!
   TBranch        *b_ph_isRecovered;   //!
   TBranch        *b_ph_isEM;   //!
   TBranch        *b_ph_convFlag;   //!
   TBranch        *b_ph_isConv;   //!
   TBranch        *b_ph_nConv;   //!
   TBranch        *b_ph_nSingleTrackConv;   //!
   TBranch        *b_ph_nDoubleTrackConv;   //!
   TBranch        *b_ph_type;   //!
   TBranch        *b_ph_origin;   //!
   TBranch        *b_ph_truth_deltaRRecPhoton;   //!
   TBranch        *b_ph_truth_E;   //!
   TBranch        *b_ph_truth_pt;   //!
   TBranch        *b_ph_truth_eta;   //!
   TBranch        *b_ph_truth_phi;   //!
   TBranch        *b_ph_truth_type;   //!
   TBranch        *b_ph_truth_status;   //!
   TBranch        *b_ph_truth_barcode;   //!
   TBranch        *b_ph_truth_mothertype;   //!
   TBranch        *b_ph_truth_motherbarcode;   //!
   TBranch        *b_ph_truth_matched;   //!
   TBranch        *b_ph_loose;   //!
   TBranch        *b_ph_tight;   //!
   TBranch        *b_ph_tightIso;   //!
   TBranch        *b_ph_Ethad;   //!
   TBranch        *b_ph_Ethad1;   //!
   TBranch        *b_ph_E033;   //!
   TBranch        *b_ph_f1;   //!
   TBranch        *b_ph_f1core;   //!
   TBranch        *b_ph_Emins1;   //!
   TBranch        *b_ph_fside;   //!
   TBranch        *b_ph_Emax2;   //!
   TBranch        *b_ph_ws3;   //!
   TBranch        *b_ph_wstot;   //!
   TBranch        *b_ph_E132;   //!
   TBranch        *b_ph_E1152;   //!
   TBranch        *b_ph_emaxs1;   //!
   TBranch        *b_ph_deltaEs;   //!
   TBranch        *b_ph_E233;   //!
   TBranch        *b_ph_E237;   //!
   TBranch        *b_ph_E277;   //!
   TBranch        *b_ph_weta2;   //!
   TBranch        *b_ph_f3;   //!
   TBranch        *b_ph_f3core;   //!
   TBranch        *b_ph_rphiallcalo;   //!
   TBranch        *b_ph_Etcone45;   //!
   TBranch        *b_ph_Etcone20;   //!
   TBranch        *b_ph_Etcone30;   //!
   TBranch        *b_ph_Etcone40;   //!
   TBranch        *b_ph_ptcone30;   //!
   TBranch        *b_ph_convanglematch;   //!
   TBranch        *b_ph_convtrackmatch;   //!
   TBranch        *b_ph_hasconv;   //!
   TBranch        *b_ph_convvtxx;   //!
   TBranch        *b_ph_convvtxy;   //!
   TBranch        *b_ph_convvtxz;   //!
   TBranch        *b_ph_Rconv;   //!
   TBranch        *b_ph_zconv;   //!
   TBranch        *b_ph_convvtxchi2;   //!
   TBranch        *b_ph_pt1conv;   //!
   TBranch        *b_ph_convtrk1nBLHits;   //!
   TBranch        *b_ph_convtrk1nPixHits;   //!
   TBranch        *b_ph_convtrk1nSCTHits;   //!
   TBranch        *b_ph_convtrk1nTRTHits;   //!
   TBranch        *b_ph_pt2conv;   //!
   TBranch        *b_ph_convtrk2nBLHits;   //!
   TBranch        *b_ph_convtrk2nPixHits;   //!
   TBranch        *b_ph_convtrk2nSCTHits;   //!
   TBranch        *b_ph_convtrk2nTRTHits;   //!
   TBranch        *b_ph_ptconv;   //!
   TBranch        *b_ph_pzconv;   //!
   TBranch        *b_ph_reta;   //!
   TBranch        *b_ph_rphi;   //!
   TBranch        *b_ph_EtringnoisedR03sig2;   //!
   TBranch        *b_ph_EtringnoisedR03sig3;   //!
   TBranch        *b_ph_EtringnoisedR03sig4;   //!
   TBranch        *b_ph_isolationlikelihoodjets;   //!
   TBranch        *b_ph_isolationlikelihoodhqelectrons;   //!
   TBranch        *b_ph_loglikelihood;   //!
   TBranch        *b_ph_photonweight;   //!
   TBranch        *b_ph_photonbgweight;   //!
   TBranch        *b_ph_neuralnet;   //!
   TBranch        *b_ph_Hmatrix;   //!
   TBranch        *b_ph_Hmatrix5;   //!
   TBranch        *b_ph_adaboost;   //!
   TBranch        *b_ph_zvertex;   //!
   TBranch        *b_ph_errz;   //!
   TBranch        *b_ph_etap;   //!
   TBranch        *b_ph_depth;   //!
   TBranch        *b_ph_cl_E;   //!
   TBranch        *b_ph_cl_pt;   //!
   TBranch        *b_ph_cl_eta;   //!
   TBranch        *b_ph_cl_phi;   //!
   TBranch        *b_ph_Es0;   //!
   TBranch        *b_ph_etas0;   //!
   TBranch        *b_ph_phis0;   //!
   TBranch        *b_ph_Es1;   //!
   TBranch        *b_ph_etas1;   //!
   TBranch        *b_ph_phis1;   //!
   TBranch        *b_ph_Es2;   //!
   TBranch        *b_ph_etas2;   //!
   TBranch        *b_ph_phis2;   //!
   TBranch        *b_ph_Es3;   //!
   TBranch        *b_ph_etas3;   //!
   TBranch        *b_ph_phis3;   //!
   TBranch        *b_ph_rawcl_Es0;   //!
   TBranch        *b_ph_rawcl_etas0;   //!
   TBranch        *b_ph_rawcl_phis0;   //!
   TBranch        *b_ph_rawcl_Es1;   //!
   TBranch        *b_ph_rawcl_etas1;   //!
   TBranch        *b_ph_rawcl_phis1;   //!
   TBranch        *b_ph_rawcl_Es2;   //!
   TBranch        *b_ph_rawcl_etas2;   //!
   TBranch        *b_ph_rawcl_phis2;   //!
   TBranch        *b_ph_rawcl_Es3;   //!
   TBranch        *b_ph_rawcl_etas3;   //!
   TBranch        *b_ph_rawcl_phis3;   //!
   TBranch        *b_ph_rawcl_E;   //!
   TBranch        *b_ph_rawcl_pt;   //!
   TBranch        *b_ph_rawcl_eta;   //!
   TBranch        *b_ph_rawcl_phi;   //!
   TBranch        *b_ph_truth_isConv;   //!
   TBranch        *b_ph_truth_isBrem;   //!
   TBranch        *b_ph_truth_isFromHardProc;   //!
   TBranch        *b_ph_truth_isPhotonFromHardProc;   //!
   TBranch        *b_ph_truth_Rconv;   //!
   TBranch        *b_ph_truth_zconv;   //!
   TBranch        *b_ph_deltaEmax2;   //!
   TBranch        *b_ph_calibHitsShowerDepth;   //!
   TBranch        *b_ph_isIso;   //!
   TBranch        *b_ph_mvaptcone20;   //!
   TBranch        *b_ph_mvaptcone30;   //!
   TBranch        *b_ph_mvaptcone40;   //!
   TBranch        *b_ph_topoEtcone20;   //!
   TBranch        *b_ph_topoEtcone40;   //!
   TBranch        *b_ph_topoEtcone60;   //!
   TBranch        *b_ph_jet_dr;   //!
   TBranch        *b_ph_jet_E;   //!
   TBranch        *b_ph_jet_pt;   //!
   TBranch        *b_ph_jet_m;   //!
   TBranch        *b_ph_jet_eta;   //!
   TBranch        *b_ph_jet_phi;   //!
   TBranch        *b_ph_jet_truth_dr;   //!
   TBranch        *b_ph_jet_truth_E;   //!
   TBranch        *b_ph_jet_truth_pt;   //!
   TBranch        *b_ph_jet_truth_m;   //!
   TBranch        *b_ph_jet_truth_eta;   //!
   TBranch        *b_ph_jet_truth_phi;   //!
   TBranch        *b_ph_jet_truth_matched;   //!
   TBranch        *b_ph_jet_matched;   //!
   TBranch        *b_ph_convIP;   //!
   TBranch        *b_ph_convIPRev;   //!
   TBranch        *b_ph_ptIsolationCone;   //!
   TBranch        *b_ph_ptIsolationConePhAngle;   //!
   TBranch        *b_ph_Etcone40_pt_corrected;   //!
   TBranch        *b_ph_Etcone40_ED_corrected;   //!
   TBranch        *b_ph_Etcone40_corrected;   //!
   TBranch        *b_ph_topodr;   //!
   TBranch        *b_ph_topopt;   //!
   TBranch        *b_ph_topoeta;   //!
   TBranch        *b_ph_topophi;   //!
   TBranch        *b_ph_topomatched;   //!
   TBranch        *b_ph_topoEMdr;   //!
   TBranch        *b_ph_topoEMpt;   //!
   TBranch        *b_ph_topoEMeta;   //!
   TBranch        *b_ph_topoEMphi;   //!
   TBranch        *b_ph_topoEMmatched;   //!
   TBranch        *b_ph_EF_dr;   //!
   TBranch        *b_ph_EF_index;   //!
   TBranch        *b_ph_L2_dr;   //!
   TBranch        *b_ph_L2_index;   //!
   TBranch        *b_ph_L1_dr;   //!
   TBranch        *b_ph_L1_index;   //!
   TBranch        *b_mu_n;   //!
   TBranch        *b_mu_E;   //!
   TBranch        *b_mu_pt;   //!
   TBranch        *b_mu_m;   //!
   TBranch        *b_mu_eta;   //!
   TBranch        *b_mu_phi;   //!
   TBranch        *b_mu_px;   //!
   TBranch        *b_mu_py;   //!
   TBranch        *b_mu_pz;   //!
   TBranch        *b_mu_charge;   //!
   TBranch        *b_mu_allauthor;   //!
   TBranch        *b_mu_author;   //!
   TBranch        *b_mu_matchchi2;   //!
   TBranch        *b_mu_matchndof;   //!
   TBranch        *b_mu_etcone20;   //!
   TBranch        *b_mu_etcone30;   //!
   TBranch        *b_mu_etcone40;   //!
   TBranch        *b_mu_nucone20;   //!
   TBranch        *b_mu_nucone30;   //!
   TBranch        *b_mu_nucone40;   //!
   TBranch        *b_mu_ptcone20;   //!
   TBranch        *b_mu_ptcone30;   //!
   TBranch        *b_mu_ptcone40;   //!
   TBranch        *b_mu_energyLossPar;   //!
   TBranch        *b_mu_energyLossErr;   //!
   TBranch        *b_mu_etCore;   //!
   TBranch        *b_mu_energyLossType;   //!
   TBranch        *b_mu_bestMatch;   //!
   TBranch        *b_mu_isStandAloneMuon;   //!
   TBranch        *b_mu_isCombinedMuon;   //!
   TBranch        *b_mu_isLowPtReconstructedMuon;   //!
   TBranch        *b_mu_loose;   //!
   TBranch        *b_mu_medium;   //!
   TBranch        *b_mu_tight;   //!
   TBranch        *b_mu_d0_exPV;   //!
   TBranch        *b_mu_z0_exPV;   //!
   TBranch        *b_mu_phi_exPV;   //!
   TBranch        *b_mu_theta_exPV;   //!
   TBranch        *b_mu_qoverp_exPV;   //!
   TBranch        *b_mu_cb_d0_exPV;   //!
   TBranch        *b_mu_cb_z0_exPV;   //!
   TBranch        *b_mu_cb_phi_exPV;   //!
   TBranch        *b_mu_cb_theta_exPV;   //!
   TBranch        *b_mu_cb_qoverp_exPV;   //!
   TBranch        *b_mu_id_d0_exPV;   //!
   TBranch        *b_mu_id_z0_exPV;   //!
   TBranch        *b_mu_id_phi_exPV;   //!
   TBranch        *b_mu_id_theta_exPV;   //!
   TBranch        *b_mu_id_qoverp_exPV;   //!
   TBranch        *b_mu_me_d0_exPV;   //!
   TBranch        *b_mu_me_z0_exPV;   //!
   TBranch        *b_mu_me_phi_exPV;   //!
   TBranch        *b_mu_me_theta_exPV;   //!
   TBranch        *b_mu_me_qoverp_exPV;   //!
   TBranch        *b_mu_ie_d0_exPV;   //!
   TBranch        *b_mu_ie_z0_exPV;   //!
   TBranch        *b_mu_ie_phi_exPV;   //!
   TBranch        *b_mu_ie_theta_exPV;   //!
   TBranch        *b_mu_ie_qoverp_exPV;   //!
   TBranch        *b_mu_cov_d0_exPV;   //!
   TBranch        *b_mu_cov_z0_exPV;   //!
   TBranch        *b_mu_cov_phi_exPV;   //!
   TBranch        *b_mu_cov_theta_exPV;   //!
   TBranch        *b_mu_cov_qoverp_exPV;   //!
   TBranch        *b_mu_cov_d0_z0_exPV;   //!
   TBranch        *b_mu_cov_d0_phi_exPV;   //!
   TBranch        *b_mu_cov_d0_theta_exPV;   //!
   TBranch        *b_mu_cov_d0_qoverp_exPV;   //!
   TBranch        *b_mu_cov_z0_phi_exPV;   //!
   TBranch        *b_mu_cov_z0_theta_exPV;   //!
   TBranch        *b_mu_cov_z0_qoverp_exPV;   //!
   TBranch        *b_mu_cov_phi_theta_exPV;   //!
   TBranch        *b_mu_cov_phi_qoverp_exPV;   //!
   TBranch        *b_mu_cov_theta_qoverp_exPV;   //!
   TBranch        *b_mu_ms_d0;   //!
   TBranch        *b_mu_ms_z0;   //!
   TBranch        *b_mu_ms_phi;   //!
   TBranch        *b_mu_ms_theta;   //!
   TBranch        *b_mu_ms_qoverp;   //!
   TBranch        *b_mu_id_d0;   //!
   TBranch        *b_mu_id_z0;   //!
   TBranch        *b_mu_id_phi;   //!
   TBranch        *b_mu_id_theta;   //!
   TBranch        *b_mu_id_qoverp;   //!
   TBranch        *b_mu_me_d0;   //!
   TBranch        *b_mu_me_z0;   //!
   TBranch        *b_mu_me_phi;   //!
   TBranch        *b_mu_me_theta;   //!
   TBranch        *b_mu_me_qoverp;   //!
   TBranch        *b_mu_ie_d0;   //!
   TBranch        *b_mu_ie_z0;   //!
   TBranch        *b_mu_ie_phi;   //!
   TBranch        *b_mu_ie_theta;   //!
   TBranch        *b_mu_ie_qoverp;   //!
   TBranch        *b_mu_nBLHits;   //!
   TBranch        *b_mu_nPixHits;   //!
   TBranch        *b_mu_nSCTHits;   //!
   TBranch        *b_mu_nTRTHits;   //!
   TBranch        *b_mu_nTRTHighTHits;   //!
   TBranch        *b_mu_nBLSharedHits;   //!
   TBranch        *b_mu_nPixSharedHits;   //!
   TBranch        *b_mu_nPixHoles;   //!
   TBranch        *b_mu_nSCTSharedHits;   //!
   TBranch        *b_mu_nSCTHoles;   //!
   TBranch        *b_mu_nTRTOutliers;   //!
   TBranch        *b_mu_nTRTHighTOutliers;   //!
   TBranch        *b_mu_nMDTHits;   //!
   TBranch        *b_mu_nMDTHoles;   //!
   TBranch        *b_mu_nCSCEtaHits;   //!
   TBranch        *b_mu_nCSCEtaHoles;   //!
   TBranch        *b_mu_nCSCPhiHits;   //!
   TBranch        *b_mu_nCSCPhiHoles;   //!
   TBranch        *b_mu_nRPCEtaHits;   //!
   TBranch        *b_mu_nRPCEtaHoles;   //!
   TBranch        *b_mu_nRPCPhiHits;   //!
   TBranch        *b_mu_nRPCPhiHoles;   //!
   TBranch        *b_mu_nTGCEtaHits;   //!
   TBranch        *b_mu_nTGCEtaHoles;   //!
   TBranch        *b_mu_nTGCPhiHits;   //!
   TBranch        *b_mu_nTGCPhiHoles;   //!
   TBranch        *b_mu_nGangedPixels;   //!
   TBranch        *b_mu_nOutliersOnTrack;   //!
   TBranch        *b_mu_nMDTBIHits;   //!
   TBranch        *b_mu_nMDTBMHits;   //!
   TBranch        *b_mu_nMDTBOHits;   //!
   TBranch        *b_mu_nMDTBEEHits;   //!
   TBranch        *b_mu_nMDTBIS78Hits;   //!
   TBranch        *b_mu_nMDTEIHits;   //!
   TBranch        *b_mu_nMDTEMHits;   //!
   TBranch        *b_mu_nMDTEOHits;   //!
   TBranch        *b_mu_nMDTEEHits;   //!
   TBranch        *b_mu_nRPCLayer1EtaHits;   //!
   TBranch        *b_mu_nRPCLayer2EtaHits;   //!
   TBranch        *b_mu_nRPCLayer3EtaHits;   //!
   TBranch        *b_mu_nRPCLayer1PhiHits;   //!
   TBranch        *b_mu_nRPCLayer2PhiHits;   //!
   TBranch        *b_mu_nRPCLayer3PhiHits;   //!
   TBranch        *b_mu_nTGCLayer1EtaHits;   //!
   TBranch        *b_mu_nTGCLayer2EtaHits;   //!
   TBranch        *b_mu_nTGCLayer3EtaHits;   //!
   TBranch        *b_mu_nTGCLayer4EtaHits;   //!
   TBranch        *b_mu_nTGCLayer1PhiHits;   //!
   TBranch        *b_mu_nTGCLayer2PhiHits;   //!
   TBranch        *b_mu_nTGCLayer3PhiHits;   //!
   TBranch        *b_mu_nTGCLayer4PhiHits;   //!
   TBranch        *b_mu_barrelSectors;   //!
   TBranch        *b_mu_endcapSectors;   //!
   TBranch        *b_mu_trackd0;   //!
   TBranch        *b_mu_trackz0;   //!
   TBranch        *b_mu_trackphi;   //!
   TBranch        *b_mu_tracktheta;   //!
   TBranch        *b_mu_trackqoverp;   //!
   TBranch        *b_mu_trackcov_d0;   //!
   TBranch        *b_mu_trackcov_z0;   //!
   TBranch        *b_mu_trackcov_phi;   //!
   TBranch        *b_mu_trackcov_theta;   //!
   TBranch        *b_mu_trackcov_qoverp;   //!
   TBranch        *b_mu_trackcov_d0_z0;   //!
   TBranch        *b_mu_trackcov_d0_phi;   //!
   TBranch        *b_mu_trackcov_d0_theta;   //!
   TBranch        *b_mu_trackcov_d0_qoverp;   //!
   TBranch        *b_mu_trackcov_z0_phi;   //!
   TBranch        *b_mu_trackcov_z0_theta;   //!
   TBranch        *b_mu_trackcov_z0_qoverp;   //!
   TBranch        *b_mu_trackcov_phi_theta;   //!
   TBranch        *b_mu_trackcov_phi_qoverp;   //!
   TBranch        *b_mu_trackcov_theta_qoverp;   //!
   TBranch        *b_mu_trackfitchi2;   //!
   TBranch        *b_mu_trackfitndof;   //!
   TBranch        *b_mu_hastrack;   //!
   TBranch        *b_mu_truth_dr;   //!
   TBranch        *b_mu_truth_E;   //!
   TBranch        *b_mu_truth_pt;   //!
   TBranch        *b_mu_truth_eta;   //!
   TBranch        *b_mu_truth_phi;   //!
   TBranch        *b_mu_truth_type;   //!
   TBranch        *b_mu_truth_status;   //!
   TBranch        *b_mu_truth_barcode;   //!
   TBranch        *b_mu_truth_mothertype;   //!
   TBranch        *b_mu_truth_motherbarcode;   //!
   TBranch        *b_mu_truth_matched;   //!
   TBranch        *b_mu_EF_dr;   //!
   TBranch        *b_mu_EF_cb_eta;   //!
   TBranch        *b_mu_EF_cb_phi;   //!
   TBranch        *b_mu_EF_cb_pt;   //!
   TBranch        *b_mu_EF_ms_eta;   //!
   TBranch        *b_mu_EF_ms_phi;   //!
   TBranch        *b_mu_EF_ms_pt;   //!
   TBranch        *b_mu_EF_me_eta;   //!
   TBranch        *b_mu_EF_me_phi;   //!
   TBranch        *b_mu_EF_me_pt;   //!
   TBranch        *b_mu_EF_matched;   //!
   TBranch        *b_mu_L2CB_dr;   //!
   TBranch        *b_mu_L2CB_pt;   //!
   TBranch        *b_mu_L2CB_eta;   //!
   TBranch        *b_mu_L2CB_phi;   //!
   TBranch        *b_mu_L2CB_id_pt;   //!
   TBranch        *b_mu_L2CB_ms_pt;   //!
   TBranch        *b_mu_L2CB_nPixHits;   //!
   TBranch        *b_mu_L2CB_nSCTHits;   //!
   TBranch        *b_mu_L2CB_nTRTHits;   //!
   TBranch        *b_mu_L2CB_nTRTHighTHits;   //!
   TBranch        *b_mu_L2CB_matched;   //!
   TBranch        *b_mu_L1_dr;   //!
   TBranch        *b_mu_L1_pt;   //!
   TBranch        *b_mu_L1_eta;   //!
   TBranch        *b_mu_L1_phi;   //!
   TBranch        *b_mu_L1_thrNumber;   //!
   TBranch        *b_mu_L1_RoINumber;   //!
   TBranch        *b_mu_L1_sectorAddress;   //!
   TBranch        *b_mu_L1_firstCandidate;   //!
   TBranch        *b_mu_L1_moreCandInRoI;   //!
   TBranch        *b_mu_L1_moreCandInSector;   //!
   TBranch        *b_mu_L1_source;   //!
   TBranch        *b_mu_L1_hemisphere;   //!
   TBranch        *b_mu_L1_matched;   //!
   TBranch        *b_trig_L1_emtau_n;   //!
   TBranch        *b_trig_L1_emtau_eta;   //!
   TBranch        *b_trig_L1_emtau_phi;   //!
   TBranch        *b_trig_L1_emtau_thrNames;   //!
   TBranch        *b_trig_L1_emtau_thrValues;   //!
   TBranch        *b_trig_L1_emtau_core;   //!
   TBranch        *b_trig_L1_emtau_EMClus;   //!
   TBranch        *b_trig_L1_emtau_tauClus;   //!
   TBranch        *b_trig_L1_emtau_EMIsol;   //!
   TBranch        *b_trig_L1_emtau_hadIsol;   //!
   TBranch        *b_trig_L1_emtau_hadCore;   //!
   TBranch        *b_trig_L1_emtau_thrPattern;   //!
   TBranch        *b_trig_L1_emtau_L1_2EM10;   //!
   TBranch        *b_trig_L1_emtau_L1_2EM10I;   //!
   TBranch        *b_trig_L1_emtau_L1_2EM14;   //!
   TBranch        *b_trig_L1_emtau_L1_2EM3;   //!
   TBranch        *b_trig_L1_emtau_L1_2EM3_EM5;   //!
   TBranch        *b_trig_L1_emtau_L1_2EM5;   //!
   TBranch        *b_trig_L1_emtau_L1_2EM5_MU6;   //!
   TBranch        *b_trig_L1_emtau_L1_EM10;   //!
   TBranch        *b_trig_L1_emtau_L1_EM10I;   //!
   TBranch        *b_trig_L1_emtau_L1_EM14;   //!
   TBranch        *b_trig_L1_emtau_L1_EM14I;   //!
   TBranch        *b_trig_L1_emtau_L1_EM14_XE10;   //!
   TBranch        *b_trig_L1_emtau_L1_EM14_XE15;   //!
   TBranch        *b_trig_L1_emtau_L1_EM19I;   //!
   TBranch        *b_trig_L1_emtau_L1_EM3;   //!
   TBranch        *b_trig_L1_emtau_L1_EM3_MV;   //!
   TBranch        *b_trig_L1_emtau_L1_EM5;   //!
   TBranch        *b_trig_L1_emtau_L1_EM5_MU6;   //!
   TBranch        *b_trig_L1_emtau_L1_EM85;   //!
   TBranch        *b_trig_L1_emtau_RoIWord;   //!
   TBranch        *b_trig_L2_emcl_n;   //!
   TBranch        *b_trig_L2_emcl_quality;   //!
   TBranch        *b_trig_L2_emcl_E;   //!
   TBranch        *b_trig_L2_emcl_Et;   //!
   TBranch        *b_trig_L2_emcl_eta;   //!
   TBranch        *b_trig_L2_emcl_phi;   //!
   TBranch        *b_trig_L2_emcl_E237;   //!
   TBranch        *b_trig_L2_emcl_E277;   //!
   TBranch        *b_trig_L2_emcl_fracs1;   //!
   TBranch        *b_trig_L2_emcl_weta2;   //!
   TBranch        *b_trig_L2_emcl_Ehad1;   //!
   TBranch        *b_trig_L2_emcl_eta1;   //!
   TBranch        *b_trig_L2_emcl_emaxs1;   //!
   TBranch        *b_trig_L2_emcl_e2tsts1;   //!
   TBranch        *b_trig_L2_emcl_rawEnergy;   //!
   TBranch        *b_trig_L2_emcl_rawEta;   //!
   TBranch        *b_trig_L2_emcl_rawPhi;   //!
   TBranch        *b_trig_L2_emcl_RoIWord;   //!
   TBranch        *b_trig_L2_emcl_nCells;   //!
   TBranch        *b_trig_L2_emcl_energyInSample;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_n;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_algorithmId;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_trackStatus;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_chi2Ndof;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_nStrawHits;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_nHighThrHits;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_nPixelSpacePoints;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_nSCT_SpacePoints;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_hitPattern;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_nStraw;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_nStrawTime;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_a0;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_z0;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_phi0;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_eta;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_pt;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_covariance;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_n;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_algorithmId;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_trackStatus;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_chi2Ndof;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_nStrawHits;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_nHighThrHits;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_nPixelSpacePoints;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_hitPattern;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_nStraw;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_nStrawTime;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_a0;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_z0;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_phi0;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_eta;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_pt;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_covariance;   //!
   TBranch        *b_trig_L2_el_n;   //!
   TBranch        *b_trig_L2_el_E;   //!
   TBranch        *b_trig_L2_el_Et;   //!
   TBranch        *b_trig_L2_el_pt;   //!
   TBranch        *b_trig_L2_el_eta;   //!
   TBranch        *b_trig_L2_el_phi;   //!
   TBranch        *b_trig_L2_el_RoIWord;   //!
   TBranch        *b_trig_L2_el_zvertex;   //!
   TBranch        *b_trig_L2_el_charge;   //!
   TBranch        *b_trig_L2_el_trackAlgo;   //!
   TBranch        *b_trig_L2_el_TRTHighTHitsRatio;   //!
   TBranch        *b_trig_L2_el_deltaeta1;   //!
   TBranch        *b_trig_L2_el_deltaphi2;   //!
   TBranch        *b_trig_L2_el_EtOverPt;   //!
   TBranch        *b_trig_L2_el_reta;   //!
   TBranch        *b_trig_L2_el_Eratio;   //!
   TBranch        *b_trig_L2_el_Ethad1;   //!
   TBranch        *b_trig_L2_el_L2_2e12_tight;   //!
   TBranch        *b_trig_L2_el_L2_2e15_medium;   //!
   TBranch        *b_trig_L2_el_L2_2e20_loose;   //!
   TBranch        *b_trig_L2_el_L2_2e5_medium;   //!
   TBranch        *b_trig_L2_el_L2_2e5_medium1;   //!
   TBranch        *b_trig_L2_el_L2_e105_loose1;   //!
   TBranch        *b_trig_L2_el_L2_e10_NoCut;   //!
   TBranch        *b_trig_L2_el_L2_e10_loose;   //!
   TBranch        *b_trig_L2_el_L2_e10_loose_IdScan;   //!
   TBranch        *b_trig_L2_el_L2_e10_loose_SiTrk;   //!
   TBranch        *b_trig_L2_el_L2_e10_loose_TRT;   //!
   TBranch        *b_trig_L2_el_L2_e10_loose_mu6;   //!
   TBranch        *b_trig_L2_el_L2_e10_medium;   //!
   TBranch        *b_trig_L2_el_L2_e10_medium_SiTrk;   //!
   TBranch        *b_trig_L2_el_L2_e10_medium_SiTrk_robust;   //!
   TBranch        *b_trig_L2_el_L2_e12_medium;   //!
   TBranch        *b_trig_L2_el_L2_e15_medium;   //!
   TBranch        *b_trig_L2_el_L2_e15i_medium;   //!
   TBranch        *b_trig_L2_el_L2_e20_loose;   //!
   TBranch        *b_trig_L2_el_L2_e20_loose_IdScan;   //!
   TBranch        *b_trig_L2_el_L2_e20_loose_NoIDTrkCut;   //!
   TBranch        *b_trig_L2_el_L2_e20_loose_SiTrk;   //!
   TBranch        *b_trig_L2_el_L2_e20_loose_SiTrk_robust;   //!
   TBranch        *b_trig_L2_el_L2_e20_loose_TRT;   //!
   TBranch        *b_trig_L2_el_L2_e20_loose_passEF;   //!
   TBranch        *b_trig_L2_el_L2_e20_loose_passL2;   //!
   TBranch        *b_trig_L2_el_L2_e20_loose_xe20;   //!
   TBranch        *b_trig_L2_el_L2_e20_loose_xe30;   //!
   TBranch        *b_trig_L2_el_L2_e20_medium;   //!
   TBranch        *b_trig_L2_el_L2_e20i_loose;   //!
   TBranch        *b_trig_L2_el_L2_e22i_tight;   //!
   TBranch        *b_trig_L2_el_L2_e25_loose;   //!
   TBranch        *b_trig_L2_el_L2_e25_medium;   //!
   TBranch        *b_trig_L2_el_L2_e25i_loose;   //!
   TBranch        *b_trig_L2_el_L2_e25i_medium;   //!
   TBranch        *b_trig_L2_el_L2_e55_loose1;   //!
   TBranch        *b_trig_L2_el_L2_e5_medium;   //!
   TBranch        *b_trig_L2_el_L2_e5_medium1;   //!
   TBranch        *b_trig_L2_el_L2_e5_medium_L2SW;   //!
   TBranch        *b_trig_L2_el_L2_e5_medium_MV;   //!
   TBranch        *b_trig_L2_el_L2_e5_medium_Ringer;   //!
   TBranch        *b_trig_L2_el_L2_e5_medium_e10_loose;   //!
   TBranch        *b_trig_L2_el_L2_em105_passHLT;   //!
   TBranch        *b_trig_L2_el_L2_em15_passHLT;   //!
   TBranch        *b_trig_L2_el_L2_em15i_passHLT;   //!
   TBranch        *b_trig_L2_el_L2_em20_passHLT;   //!
   TBranch        *b_trig_L2_el_L2_em20i_passHLT;   //!
   TBranch        *b_trig_L2_el_nTRTHits;   //!
   TBranch        *b_trig_L2_el_nTRTHighTHits;   //!
   TBranch        *b_trig_L2_el_trackIndx;   //!
   TBranch        *b_trig_L2_el_trkPt;   //!
   TBranch        *b_trig_L2_el_err_trkPt;   //!
   TBranch        *b_trig_L2_el_err_eta;   //!
   TBranch        *b_trig_L2_el_err_phi;   //!
   TBranch        *b_trig_L2_el_errz;   //!
   TBranch        *b_trig_L2_el_trkEtaAtCalo;   //!
   TBranch        *b_trig_L2_el_trkPhiAtCalo;   //!
   TBranch        *b_trig_L2_el_caloEta;   //!
   TBranch        *b_trig_L2_el_caloPhi;   //!
   TBranch        *b_trig_L2_el_F0;   //!
   TBranch        *b_trig_L2_el_F1;   //!
   TBranch        *b_trig_L2_el_F2;   //!
   TBranch        *b_trig_L2_el_F3;   //!
   TBranch        *b_trig_L2_ph_n;   //!
   TBranch        *b_trig_L2_ph_E;   //!
   TBranch        *b_trig_L2_ph_Et;   //!
   TBranch        *b_trig_L2_ph_pt;   //!
   TBranch        *b_trig_L2_ph_eta;   //!
   TBranch        *b_trig_L2_ph_phi;   //!
   TBranch        *b_trig_L2_ph_RoIWord;   //!
   TBranch        *b_trig_L2_ph_HadEt1;   //!
   TBranch        *b_trig_L2_ph_Eratio;   //!
   TBranch        *b_trig_L2_ph_Reta;   //!
   TBranch        *b_trig_L2_ph_dPhi;   //!
   TBranch        *b_trig_L2_ph_dEta;   //!
   TBranch        *b_trig_L2_ph_F1;   //!
   TBranch        *b_trig_L2_ph_L2_2g10_loose;   //!
   TBranch        *b_trig_L2_ph_L2_2g10_loose_mu6;   //!
   TBranch        *b_trig_L2_ph_L2_2g17i_tight;   //!
   TBranch        *b_trig_L2_ph_L2_2g20_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g105;   //!
   TBranch        *b_trig_L2_ph_L2_g10_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g10_nocut;   //!
   TBranch        *b_trig_L2_ph_L2_g150;   //!
   TBranch        *b_trig_L2_ph_L2_g15_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g20_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g20_loose_cnv;   //!
   TBranch        *b_trig_L2_ph_L2_g20_loose_xe20;   //!
   TBranch        *b_trig_L2_ph_L2_g20i_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g25_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g25_loose_xe30;   //!
   TBranch        *b_trig_L2_ph_L2_g25i_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g55_tight;   //!
   TBranch        *b_trig_L2_ph_Fside;   //!
   TBranch        *b_trig_L2_ph_Weta2;   //!
   TBranch        *b_trig_L2_ph_F0;   //!
   TBranch        *b_trig_L2_ph_F2;   //!
   TBranch        *b_trig_L2_ph_F3;   //!
   TBranch        *b_trig_EF_emcl_n;   //!
   TBranch        *b_trig_EF_emcl_E;   //!
   TBranch        *b_trig_EF_emcl_pt;   //!
   TBranch        *b_trig_EF_emcl_m;   //!
   TBranch        *b_trig_EF_emcl_eta;   //!
   TBranch        *b_trig_EF_emcl_phi;   //!
   TBranch        *b_trig_EF_emcl_E_em;   //!
   TBranch        *b_trig_EF_emcl_E_had;   //!
   TBranch        *b_trig_EF_emcl_firstEdens;   //!
   TBranch        *b_trig_EF_emcl_cellmaxfrac;   //!
   TBranch        *b_trig_EF_emcl_longitudinal;   //!
   TBranch        *b_trig_EF_emcl_secondlambda;   //!
   TBranch        *b_trig_EF_emcl_lateral;   //!
   TBranch        *b_trig_EF_emcl_secondR;   //!
   TBranch        *b_trig_EF_emcl_centerlambda;   //!
   TBranch        *b_trig_EF_emcl_deltaTheta;   //!
   TBranch        *b_trig_EF_emcl_deltaPhi;   //!
   TBranch        *b_trig_EF_emcl_time;   //!
   TBranch        *b_trig_EF_emcl_E_PreSamplerB;   //!
   TBranch        *b_trig_EF_emcl_E_EMB1;   //!
   TBranch        *b_trig_EF_emcl_E_EMB2;   //!
   TBranch        *b_trig_EF_emcl_E_EMB3;   //!
   TBranch        *b_trig_EF_emcl_E_PreSamplerE;   //!
   TBranch        *b_trig_EF_emcl_E_EME1;   //!
   TBranch        *b_trig_EF_emcl_E_EME2;   //!
   TBranch        *b_trig_EF_emcl_E_EME3;   //!
   TBranch        *b_trig_EF_emcl_E_HEC0;   //!
   TBranch        *b_trig_EF_emcl_E_HEC1;   //!
   TBranch        *b_trig_EF_emcl_E_HEC2;   //!
   TBranch        *b_trig_EF_emcl_E_HEC3;   //!
   TBranch        *b_trig_EF_emcl_E_TileBar0;   //!
   TBranch        *b_trig_EF_emcl_E_TileBar1;   //!
   TBranch        *b_trig_EF_emcl_E_TileBar2;   //!
   TBranch        *b_trig_EF_emcl_E_TileGap1;   //!
   TBranch        *b_trig_EF_emcl_E_TileGap2;   //!
   TBranch        *b_trig_EF_emcl_E_TileGap3;   //!
   TBranch        *b_trig_EF_emcl_E_TileExt0;   //!
   TBranch        *b_trig_EF_emcl_E_TileExt1;   //!
   TBranch        *b_trig_EF_emcl_E_TileExt2;   //!
   TBranch        *b_trig_EF_emcl_E_FCAL0;   //!
   TBranch        *b_trig_EF_emcl_E_FCAL1;   //!
   TBranch        *b_trig_EF_emcl_E_FCAL2;   //!
   TBranch        *b_trig_EF_emcl_eta_PreSamplerB;   //!
   TBranch        *b_trig_EF_emcl_phi_PreSamplerB;   //!
   TBranch        *b_trig_EF_emcl_eta_EMB1;   //!
   TBranch        *b_trig_EF_emcl_phi_EMB1;   //!
   TBranch        *b_trig_EF_emcl_eta_EMB2;   //!
   TBranch        *b_trig_EF_emcl_phi_EMB2;   //!
   TBranch        *b_trig_EF_emcl_eta_EMB3;   //!
   TBranch        *b_trig_EF_emcl_phi_EMB3;   //!
   TBranch        *b_trig_EF_emcl_eta_PreSamplerE;   //!
   TBranch        *b_trig_EF_emcl_phi_PreSamplerE;   //!
   TBranch        *b_trig_EF_emcl_eta_EME1;   //!
   TBranch        *b_trig_EF_emcl_phi_EME1;   //!
   TBranch        *b_trig_EF_emcl_eta_EME2;   //!
   TBranch        *b_trig_EF_emcl_phi_EME2;   //!
   TBranch        *b_trig_EF_emcl_eta_EME3;   //!
   TBranch        *b_trig_EF_emcl_phi_EME3;   //!
   TBranch        *b_trig_EF_emcl_eta_HEC0;   //!
   TBranch        *b_trig_EF_emcl_phi_HEC0;   //!
   TBranch        *b_trig_EF_emcl_eta_HEC1;   //!
   TBranch        *b_trig_EF_emcl_phi_HEC1;   //!
   TBranch        *b_trig_EF_emcl_eta_HEC2;   //!
   TBranch        *b_trig_EF_emcl_phi_HEC2;   //!
   TBranch        *b_trig_EF_emcl_eta_HEC3;   //!
   TBranch        *b_trig_EF_emcl_phi_HEC3;   //!
   TBranch        *b_trig_EF_emcl_eta_TileBar0;   //!
   TBranch        *b_trig_EF_emcl_phi_TileBar0;   //!
   TBranch        *b_trig_EF_emcl_eta_TileBar1;   //!
   TBranch        *b_trig_EF_emcl_phi_TileBar1;   //!
   TBranch        *b_trig_EF_emcl_eta_TileBar2;   //!
   TBranch        *b_trig_EF_emcl_phi_TileBar2;   //!
   TBranch        *b_trig_EF_emcl_eta_TileGap1;   //!
   TBranch        *b_trig_EF_emcl_phi_TileGap1;   //!
   TBranch        *b_trig_EF_emcl_eta_TileGap2;   //!
   TBranch        *b_trig_EF_emcl_phi_TileGap2;   //!
   TBranch        *b_trig_EF_emcl_eta_TileGap3;   //!
   TBranch        *b_trig_EF_emcl_phi_TileGap3;   //!
   TBranch        *b_trig_EF_emcl_eta_TileExt0;   //!
   TBranch        *b_trig_EF_emcl_phi_TileExt0;   //!
   TBranch        *b_trig_EF_emcl_eta_TileExt1;   //!
   TBranch        *b_trig_EF_emcl_phi_TileExt1;   //!
   TBranch        *b_trig_EF_emcl_eta_TileExt2;   //!
   TBranch        *b_trig_EF_emcl_phi_TileExt2;   //!
   TBranch        *b_trig_EF_emcl_eta_FCAL0;   //!
   TBranch        *b_trig_EF_emcl_phi_FCAL0;   //!
   TBranch        *b_trig_EF_emcl_eta_FCAL1;   //!
   TBranch        *b_trig_EF_emcl_phi_FCAL1;   //!
   TBranch        *b_trig_EF_emcl_eta_FCAL2;   //!
   TBranch        *b_trig_EF_emcl_phi_FCAL2;   //!
   TBranch        *b_trig_EF_emcl_cell_n;   //!
   TBranch        *b_trig_EF_emcl_cell_E;   //!
   TBranch        *b_trig_EF_emcl_cell_pt;   //!
   TBranch        *b_trig_EF_emcl_cell_eta;   //!
   TBranch        *b_trig_EF_emcl_cell_phi;   //!
   TBranch        *b_trig_EF_emcl_cell_QCells;   //!
   TBranch        *b_trig_EF_emcl_cell_GainCells;   //!
   TBranch        *b_trig_EF_emcl_cell_DetCells;   //!
   TBranch        *b_trig_EF_emcl_cell_TimeCells;   //!
   TBranch        *b_trig_EF_emcl_cell_xCells;   //!
   TBranch        *b_trig_EF_emcl_cell_yCells;   //!
   TBranch        *b_trig_EF_emcl_cell_zCells;   //!
   TBranch        *b_trig_EF_emcl_cell_BadCells;   //!
   TBranch        *b_trig_EF_emcl_cell_IdCells;   //!
   TBranch        *b_trig_EF_emcl_slw_n;   //!
   TBranch        *b_trig_EF_emcl_slw_E;   //!
   TBranch        *b_trig_EF_emcl_slw_pt;   //!
   TBranch        *b_trig_EF_emcl_slw_m;   //!
   TBranch        *b_trig_EF_emcl_slw_eta;   //!
   TBranch        *b_trig_EF_emcl_slw_phi;   //!
   TBranch        *b_trig_EF_emcl_slw_E_em;   //!
   TBranch        *b_trig_EF_emcl_slw_E_had;   //!
   TBranch        *b_trig_EF_emcl_slw_firstEdens;   //!
   TBranch        *b_trig_EF_emcl_slw_cellmaxfrac;   //!
   TBranch        *b_trig_EF_emcl_slw_longitudinal;   //!
   TBranch        *b_trig_EF_emcl_slw_secondlambda;   //!
   TBranch        *b_trig_EF_emcl_slw_lateral;   //!
   TBranch        *b_trig_EF_emcl_slw_secondR;   //!
   TBranch        *b_trig_EF_emcl_slw_centerlambda;   //!
   TBranch        *b_trig_EF_emcl_slw_deltaTheta;   //!
   TBranch        *b_trig_EF_emcl_slw_deltaPhi;   //!
   TBranch        *b_trig_EF_emcl_slw_time;   //!
   TBranch        *b_trig_EF_emcl_slw_E_PreSamplerB;   //!
   TBranch        *b_trig_EF_emcl_slw_E_EMB1;   //!
   TBranch        *b_trig_EF_emcl_slw_E_EMB2;   //!
   TBranch        *b_trig_EF_emcl_slw_E_EMB3;   //!
   TBranch        *b_trig_EF_emcl_slw_E_PreSamplerE;   //!
   TBranch        *b_trig_EF_emcl_slw_E_EME1;   //!
   TBranch        *b_trig_EF_emcl_slw_E_EME2;   //!
   TBranch        *b_trig_EF_emcl_slw_E_EME3;   //!
   TBranch        *b_trig_EF_emcl_slw_E_HEC0;   //!
   TBranch        *b_trig_EF_emcl_slw_E_HEC1;   //!
   TBranch        *b_trig_EF_emcl_slw_E_HEC2;   //!
   TBranch        *b_trig_EF_emcl_slw_E_HEC3;   //!
   TBranch        *b_trig_EF_emcl_slw_E_TileBar0;   //!
   TBranch        *b_trig_EF_emcl_slw_E_TileBar1;   //!
   TBranch        *b_trig_EF_emcl_slw_E_TileBar2;   //!
   TBranch        *b_trig_EF_emcl_slw_E_TileGap1;   //!
   TBranch        *b_trig_EF_emcl_slw_E_TileGap2;   //!
   TBranch        *b_trig_EF_emcl_slw_E_TileGap3;   //!
   TBranch        *b_trig_EF_emcl_slw_E_TileExt0;   //!
   TBranch        *b_trig_EF_emcl_slw_E_TileExt1;   //!
   TBranch        *b_trig_EF_emcl_slw_E_TileExt2;   //!
   TBranch        *b_trig_EF_emcl_slw_E_FCAL0;   //!
   TBranch        *b_trig_EF_emcl_slw_E_FCAL1;   //!
   TBranch        *b_trig_EF_emcl_slw_E_FCAL2;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_PreSamplerB;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_PreSamplerB;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_EMB1;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_EMB1;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_EMB2;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_EMB2;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_EMB3;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_EMB3;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_PreSamplerE;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_PreSamplerE;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_EME1;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_EME1;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_EME2;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_EME2;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_EME3;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_EME3;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_HEC0;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_HEC0;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_HEC1;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_HEC1;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_HEC2;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_HEC2;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_HEC3;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_HEC3;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_TileBar0;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_TileBar0;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_TileBar1;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_TileBar1;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_TileBar2;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_TileBar2;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_TileGap1;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_TileGap1;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_TileGap2;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_TileGap2;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_TileGap3;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_TileGap3;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_TileExt0;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_TileExt0;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_TileExt1;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_TileExt1;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_TileExt2;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_TileExt2;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_FCAL0;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_FCAL0;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_FCAL1;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_FCAL1;   //!
   TBranch        *b_trig_EF_emcl_slw_eta_FCAL2;   //!
   TBranch        *b_trig_EF_emcl_slw_phi_FCAL2;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_n;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_E;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_pt;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_eta;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_phi;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_QCells;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_GainCells;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_DetCells;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_TimeCells;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_xCells;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_yCells;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_zCells;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_BadCells;   //!
   TBranch        *b_trig_EF_emcl_slw_cell_IdCells;   //!
   TBranch        *b_trig_EF_el_n;   //!
   TBranch        *b_trig_EF_el_E;   //!
   TBranch        *b_trig_EF_el_Et;   //!
   TBranch        *b_trig_EF_el_pt;   //!
   TBranch        *b_trig_EF_el_m;   //!
   TBranch        *b_trig_EF_el_eta;   //!
   TBranch        *b_trig_EF_el_phi;   //!
   TBranch        *b_trig_EF_el_px;   //!
   TBranch        *b_trig_EF_el_py;   //!
   TBranch        *b_trig_EF_el_pz;   //!
   TBranch        *b_trig_EF_el_charge;   //!
   TBranch        *b_trig_EF_el_author;   //!
   TBranch        *b_trig_EF_el_isEM;   //!
   TBranch        *b_trig_EF_el_loose;   //!
   TBranch        *b_trig_EF_el_medium;   //!
   TBranch        *b_trig_EF_el_mediumIso;   //!
   TBranch        *b_trig_EF_el_tight;   //!
   TBranch        *b_trig_EF_el_tightIso;   //!
   TBranch        *b_trig_EF_el_Ethad;   //!
   TBranch        *b_trig_EF_el_Ethad1;   //!
   TBranch        *b_trig_EF_el_f1;   //!
   TBranch        *b_trig_EF_el_f1core;   //!
   TBranch        *b_trig_EF_el_Emins1;   //!
   TBranch        *b_trig_EF_el_fside;   //!
   TBranch        *b_trig_EF_el_Emax2;   //!
   TBranch        *b_trig_EF_el_ws3;   //!
   TBranch        *b_trig_EF_el_wstot;   //!
   TBranch        *b_trig_EF_el_emaxs1;   //!
   TBranch        *b_trig_EF_el_deltaEs;   //!
   TBranch        *b_trig_EF_el_E233;   //!
   TBranch        *b_trig_EF_el_E237;   //!
   TBranch        *b_trig_EF_el_E277;   //!
   TBranch        *b_trig_EF_el_weta2;   //!
   TBranch        *b_trig_EF_el_f3;   //!
   TBranch        *b_trig_EF_el_f3core;   //!
   TBranch        *b_trig_EF_el_rphiallcalo;   //!
   TBranch        *b_trig_EF_el_Etcone45;   //!
   TBranch        *b_trig_EF_el_Etcone20;   //!
   TBranch        *b_trig_EF_el_Etcone30;   //!
   TBranch        *b_trig_EF_el_Etcone40;   //!
   TBranch        *b_trig_EF_el_pos7;   //!
   TBranch        *b_trig_EF_el_etacorrmag;   //!
   TBranch        *b_trig_EF_el_deltaeta1;   //!
   TBranch        *b_trig_EF_el_deltaeta2;   //!
   TBranch        *b_trig_EF_el_deltaphi2;   //!
   TBranch        *b_trig_EF_el_reta;   //!
   TBranch        *b_trig_EF_el_rphi;   //!
   TBranch        *b_trig_EF_el_EtringnoisedR03sig2;   //!
   TBranch        *b_trig_EF_el_EtringnoisedR03sig3;   //!
   TBranch        *b_trig_EF_el_EtringnoisedR03sig4;   //!
   TBranch        *b_trig_EF_el_isolationlikelihoodjets;   //!
   TBranch        *b_trig_EF_el_isolationlikelihoodhqelectrons;   //!
   TBranch        *b_trig_EF_el_electronweight;   //!
   TBranch        *b_trig_EF_el_electronbgweight;   //!
   TBranch        *b_trig_EF_el_softeweight;   //!
   TBranch        *b_trig_EF_el_softebgweight;   //!
   TBranch        *b_trig_EF_el_neuralnet;   //!
   TBranch        *b_trig_EF_el_Hmatrix;   //!
   TBranch        *b_trig_EF_el_Hmatrix5;   //!
   TBranch        *b_trig_EF_el_adaboost;   //!
   TBranch        *b_trig_EF_el_softeneuralnet;   //!
   TBranch        *b_trig_EF_el_zvertex;   //!
   TBranch        *b_trig_EF_el_errz;   //!
   TBranch        *b_trig_EF_el_etap;   //!
   TBranch        *b_trig_EF_el_depth;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovd0;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovz0;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovphi;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovtheta;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovqoverp;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovd0z0;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovz0phi;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovz0theta;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovz0qoverp;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovd0phi;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovd0theta;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovd0qoverp;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovphitheta;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovphiqoverp;   //!
   TBranch        *b_trig_EF_el_refittedtrackcovthetaqoverp;   //!
   TBranch        *b_trig_EF_el_EF_2e12_tight;   //!
   TBranch        *b_trig_EF_el_EF_2e15_medium;   //!
   TBranch        *b_trig_EF_el_EF_2e20_loose;   //!
   TBranch        *b_trig_EF_el_EF_2e5_medium;   //!
   TBranch        *b_trig_EF_el_EF_2e5_medium1;   //!
   TBranch        *b_trig_EF_el_EF_e105_loose1;   //!
   TBranch        *b_trig_EF_el_EF_e10_NoCut;   //!
   TBranch        *b_trig_EF_el_EF_e10_loose;   //!
   TBranch        *b_trig_EF_el_EF_e10_loose_IdScan;   //!
   TBranch        *b_trig_EF_el_EF_e10_loose_SiTrk;   //!
   TBranch        *b_trig_EF_el_EF_e10_loose_TRT;   //!
   TBranch        *b_trig_EF_el_EF_e10_loose_mu6;   //!
   TBranch        *b_trig_EF_el_EF_e10_medium;   //!
   TBranch        *b_trig_EF_el_EF_e10_medium_SiTrk;   //!
   TBranch        *b_trig_EF_el_EF_e10_medium_SiTrk_robust;   //!
   TBranch        *b_trig_EF_el_EF_e12_medium;   //!
   TBranch        *b_trig_EF_el_EF_e15_medium;   //!
   TBranch        *b_trig_EF_el_EF_e15i_medium;   //!
   TBranch        *b_trig_EF_el_EF_e20_loose;   //!
   TBranch        *b_trig_EF_el_EF_e20_loose_IdScan;   //!
   TBranch        *b_trig_EF_el_EF_e20_loose_NoIDTrkCut;   //!
   TBranch        *b_trig_EF_el_EF_e20_loose_SiTrk;   //!
   TBranch        *b_trig_EF_el_EF_e20_loose_SiTrk_robust;   //!
   TBranch        *b_trig_EF_el_EF_e20_loose_TRT;   //!
   TBranch        *b_trig_EF_el_EF_e20_loose_passEF;   //!
   TBranch        *b_trig_EF_el_EF_e20_loose_passL2;   //!
   TBranch        *b_trig_EF_el_EF_e20_loose_xe20;   //!
   TBranch        *b_trig_EF_el_EF_e20_loose_xe30;   //!
   TBranch        *b_trig_EF_el_EF_e20_medium;   //!
   TBranch        *b_trig_EF_el_EF_e20i_loose;   //!
   TBranch        *b_trig_EF_el_EF_e22i_tight;   //!
   TBranch        *b_trig_EF_el_EF_e25_loose;   //!
   TBranch        *b_trig_EF_el_EF_e25_medium;   //!
   TBranch        *b_trig_EF_el_EF_e25i_loose;   //!
   TBranch        *b_trig_EF_el_EF_e25i_medium;   //!
   TBranch        *b_trig_EF_el_EF_e55_loose1;   //!
   TBranch        *b_trig_EF_el_EF_e5_medium;   //!
   TBranch        *b_trig_EF_el_EF_e5_medium1;   //!
   TBranch        *b_trig_EF_el_EF_e5_medium_L2SW;   //!
   TBranch        *b_trig_EF_el_EF_e5_medium_MV;   //!
   TBranch        *b_trig_EF_el_EF_e5_medium_Ringer;   //!
   TBranch        *b_trig_EF_el_EF_e5_medium_e10_loose;   //!
   TBranch        *b_trig_EF_el_EF_em105_passHLT;   //!
   TBranch        *b_trig_EF_el_EF_em15_passHLT;   //!
   TBranch        *b_trig_EF_el_EF_em15i_passHLT;   //!
   TBranch        *b_trig_EF_el_EF_em20_passHLT;   //!
   TBranch        *b_trig_EF_el_EF_em20i_passHLT;   //!
   TBranch        *b_trig_EF_ph_n;   //!
   TBranch        *b_trig_EF_ph_E;   //!
   TBranch        *b_trig_EF_ph_Et;   //!
   TBranch        *b_trig_EF_ph_pt;   //!
   TBranch        *b_trig_EF_ph_m;   //!
   TBranch        *b_trig_EF_ph_eta;   //!
   TBranch        *b_trig_EF_ph_phi;   //!
   TBranch        *b_trig_EF_ph_px;   //!
   TBranch        *b_trig_EF_ph_py;   //!
   TBranch        *b_trig_EF_ph_pz;   //!
   TBranch        *b_trig_EF_ph_EF_2g10_loose;   //!
   TBranch        *b_trig_EF_ph_EF_2g10_loose_mu6;   //!
   TBranch        *b_trig_EF_ph_EF_2g17i_tight;   //!
   TBranch        *b_trig_EF_ph_EF_2g20_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g105;   //!
   TBranch        *b_trig_EF_ph_EF_g10_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g10_loose_larcalib;   //!
   TBranch        *b_trig_EF_ph_EF_g10_nocut;   //!
   TBranch        *b_trig_EF_ph_EF_g150;   //!
   TBranch        *b_trig_EF_ph_EF_g15_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g20_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g20_loose_cnv;   //!
   TBranch        *b_trig_EF_ph_EF_g20_loose_larcalib;   //!
   TBranch        *b_trig_EF_ph_EF_g20_loose_xe20;   //!
   TBranch        *b_trig_EF_ph_EF_g20i_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g20i_loose_larcalib;   //!
   TBranch        *b_trig_EF_ph_EF_g25_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g25_loose_xe30;   //!
   TBranch        *b_trig_EF_ph_EF_g25i_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g55_tight;   //!
   TBranch        *b_trig_RoI_L2_e_n;   //!
   TBranch        *b_trig_RoI_L2_e_type;   //!
   TBranch        *b_trig_RoI_L2_e_lastStep;   //!
   TBranch        *b_trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgamma;   //!
   TBranch        *b_trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgammaStatus;   //!
   TBranch        *b_trig_RoI_L2_e_TrigEMCluster;   //!
   TBranch        *b_trig_RoI_L2_e_TrigEMClusterStatus;   //!
   TBranch        *b_trig_RoI_L2_e_EmTau_ROI;   //!
   TBranch        *b_trig_RoI_L2_e_EmTau_ROIStatus;   //!
   TBranch        *b_trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma;   //!
   TBranch        *b_trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus;   //!
   TBranch        *b_trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma;   //!
   TBranch        *b_trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus;   //!
   TBranch        *b_trig_RoI_L2_e_TrigElectronContainer;   //!
   TBranch        *b_trig_RoI_L2_e_TrigElectronContainerStatus;   //!
   TBranch        *b_trig_RoI_L2_e_TrigPhotonContainer;   //!
   TBranch        *b_trig_RoI_L2_e_TrigPhotonContainerStatus;   //!
   TBranch        *b_trig_RoI_EF_e_n;   //!
   TBranch        *b_trig_RoI_EF_e_type;   //!
   TBranch        *b_trig_RoI_EF_e_lastStep;   //!
   TBranch        *b_trig_RoI_EF_e_EmTau_ROI;   //!
   TBranch        *b_trig_RoI_EF_e_EmTau_ROIStatus;   //!
   TBranch        *b_trig_RoI_EF_e_Rec__TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFID;   //!
   TBranch        *b_trig_RoI_EF_e_Rec__TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFIDStatus;   //!
   TBranch        *b_trig_RoI_EF_e_egammaContainer_egamma_Electrons;   //!
   TBranch        *b_trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus;   //!
   TBranch        *b_trig_RoI_EF_e_egammaContainer_egamma_Photons;   //!
   TBranch        *b_trig_RoI_EF_e_egammaContainer_egamma_PhotonsStatus;   //!
   TBranch        *b_trk_n;   //!
   TBranch        *b_trk_pt;   //!
   TBranch        *b_trk_eta;   //!
   TBranch        *b_trk_d0_wrtPV;   //!
   TBranch        *b_trk_z0_wrtPV;   //!
   TBranch        *b_trk_phi_wrtPV;   //!
   TBranch        *b_trk_cov_d0_wrtPV;   //!
   TBranch        *b_trk_cov_z0_wrtPV;   //!
   TBranch        *b_trk_cov_phi_wrtPV;   //!
   TBranch        *b_trk_cov_theta_wrtPV;   //!
   TBranch        *b_trk_cov_qoverp_wrtPV;   //!
   TBranch        *b_trk_chi2;   //!
   TBranch        *b_trk_ndof;   //!
   TBranch        *b_trk_nBLHits;   //!
   TBranch        *b_trk_nPixHits;   //!
   TBranch        *b_trk_nSCTHits;   //!
   TBranch        *b_trk_nTRTHits;   //!
   TBranch        *b_trk_nPixHoles;   //!
   TBranch        *b_trk_nSCTHoles;   //!
   TBranch        *b_trk_nMDTHits;   //!
   TBranch        *b_trk_nCSCEtaHits;   //!
   TBranch        *b_trk_nCSCPhiHits;   //!
   TBranch        *b_trk_nRPCEtaHits;   //!
   TBranch        *b_trk_nRPCPhiHits;   //!
   TBranch        *b_trk_nTGCEtaHits;   //!
   TBranch        *b_trk_nTGCPhiHits;   //!
   TBranch        *b_trk_nHits;   //!
   TBranch        *b_trk_nHoles;   //!
   TBranch        *b_trk_hitPattern;   //!
   TBranch        *b_trk_TRTHighTHitsRatio;   //!
   TBranch        *b_trk_pixeldEdx;   //!
   TBranch        *b_trk_fitter;   //!
   TBranch        *b_trk_patternReco1;   //!
   TBranch        *b_trk_patternReco2;   //!
   TBranch        *b_trk_seedFinder;   //!
   TBranch        *b_trk_blayerPrediction_expectHit;   //!
   TBranch        *b_trk_blayerPrediction_x;   //!
   TBranch        *b_trk_blayerPrediction_y;   //!
   TBranch        *b_trk_blayerPrediction_z;   //!
   TBranch        *b_trk_blayerPrediction_locX;   //!
   TBranch        *b_trk_blayerPrediction_locY;   //!
   TBranch        *b_trk_blayerPrediction_err_locX;   //!
   TBranch        *b_trk_blayerPrediction_err_locY;   //!
   TBranch        *b_trk_blayerPrediction_etaDistToEdge;   //!
   TBranch        *b_trk_blayerPrediction_phiDistToEdge;   //!
   TBranch        *b_trk_blayerPrediction_detElementId;   //!
   TBranch        *b_trk_blayerPrediction_row;   //!
   TBranch        *b_trk_blayerPrediction_col;   //!
   TBranch        *b_trk_blayerPrediction_type;   //!
   TBranch        *b_trk_mc_probability;   //!
   TBranch        *b_trk_mc_barcode;   //!
   TBranch        *b_trk_mc_index;   //!
   TBranch        *b_vxp_n;   //!
   TBranch        *b_vxp_x;   //!
   TBranch        *b_vxp_y;   //!
   TBranch        *b_vxp_z;   //!
   TBranch        *b_vxp_err_x;   //!
   TBranch        *b_vxp_err_y;   //!
   TBranch        *b_vxp_err_z;   //!
   TBranch        *b_vxp_chi2;   //!
   TBranch        *b_vxp_ndof;   //!
   TBranch        *b_vxp_px;   //!
   TBranch        *b_vxp_py;   //!
   TBranch        *b_vxp_pz;   //!
   TBranch        *b_vxp_E;   //!
   TBranch        *b_vxp_m;   //!
   TBranch        *b_vxp_nTracks;   //!
   TBranch        *b_vxp_sumPt;   //!
   TBranch        *b_vxp_type;   //!
   TBranch        *b_vxp_sumWeightInliers;   //!
   TBranch        *b_vxp_sumWeightOutliers;   //!
   TBranch        *b_vxp_sumWeightPileup;   //!
   TBranch        *b_vxp_sumWeightFakes;   //!
   TBranch        *b_vxp_nInliers;   //!
   TBranch        *b_vxp_nOutliers;   //!
   TBranch        *b_vxp_nPileUp;   //!
   TBranch        *b_vxp_nFakes;   //!
   TBranch        *b_vxp_purity;   //!
   TBranch        *b_vxp_trk_n;   //!
   TBranch        *b_vxp_trk_chi2;   //!
   TBranch        *b_vxp_trk_d0;   //!
   TBranch        *b_vxp_trk_z0;   //!
   TBranch        *b_vxp_trk_unbiased_d0;   //!
   TBranch        *b_vxp_trk_unbiased_z0;   //!
   TBranch        *b_vxp_trk_err_unbiased_d0;   //!
   TBranch        *b_vxp_trk_err_unbiased_z0;   //!
   TBranch        *b_vxp_trk_phi;   //!
   TBranch        *b_vxp_trk_theta;   //!
   TBranch        *b_vxp_trk_weight;   //!
   TBranch        *b_vxp_trk_index;   //!
   TBranch        *b_jetMETTriggerBitsFiller_L1_J10;   //!
   TBranch        *b_jetMETTriggerBitsFiller_L1_J15;   //!
   TBranch        *b_jetMETTriggerBitsFiller_L1_J5;   //!
   TBranch        *b_mcVx_n;   //!
   TBranch        *b_mcVx_x;   //!
   TBranch        *b_mcVx_y;   //!
   TBranch        *b_mcVx_z;   //!
   TBranch        *b_mcTrk_n;   //!
   TBranch        *b_mcTrk_gen_pt;   //!
   TBranch        *b_mcTrk_gen_eta;   //!
   TBranch        *b_mcTrk_gen_phi;   //!
   TBranch        *b_mcTrk_gen_energy;   //!
   TBranch        *b_mcTrk_perigee_ok;   //!
   TBranch        *b_mcTrk_perigee_d0;   //!
   TBranch        *b_mcTrk_perigee_z0;   //!
   TBranch        *b_mcTrk_perigee_phi;   //!
   TBranch        *b_mcTrk_perigee_theta;   //!
   TBranch        *b_mcTrk_perigee_qoverp;   //!
   TBranch        *b_mcTrk_pdg;   //!
   TBranch        *b_mcTrk_charge;   //!
   TBranch        *b_mcTrk_barcode;   //!
   TBranch        *b_mcTrk_status;   //!
   TBranch        *b_mcTrk_mother_pdg;   //!
   TBranch        *b_mcTrk_mother_barcode;   //!
   TBranch        *b_mcTrk_child_n;   //!
   TBranch        *b_mcTrk_child_barcode;   //!
   TBranch        *b_mcTrk_begVtx_x;   //!
   TBranch        *b_mcTrk_begVtx_y;   //!
   TBranch        *b_mcTrk_begVtx_z;   //!
   TBranch        *b_mcTrk_begVtx_barcode;   //!
   TBranch        *b_mcTrk_endVtx_x;   //!
   TBranch        *b_mcTrk_endVtx_y;   //!
   TBranch        *b_mcTrk_endVtx_z;   //!
   TBranch        *b_mcTrk_endVtx_barcode;   //!
   TBranch        *b_mc_n;   //!
   TBranch        *b_mc_pt;   //!
   TBranch        *b_mc_m;   //!
   TBranch        *b_mc_eta;   //!
   TBranch        *b_mc_phi;   //!
   TBranch        *b_mc_status;   //!
   TBranch        *b_mc_barcode;   //!
   TBranch        *b_mc_parents;   //!
   TBranch        *b_mc_children;   //!
   TBranch        *b_mc_pdgId;   //!
   TBranch        *b_mc_charge;   //!
   TBranch        *b_mc_vx_x;   //!
   TBranch        *b_mc_vx_y;   //!
   TBranch        *b_mc_vx_z;   //!
   TBranch        *b_mc_child_index;   //!
   TBranch        *b_mc_parent_index;   //!
   TBranch        *b_mcEvt_n;   //!
   TBranch        *b_mcEvt_signal_process_id;   //!
   TBranch        *b_mcEvt_event_number;   //!
   TBranch        *b_mcEvt_event_scale;   //!
   TBranch        *b_mcEvt_alphaQCD;   //!
   TBranch        *b_mcEvt_alphaQED;   //!
   TBranch        *b_mcEvt_pdf_id1;   //!
   TBranch        *b_mcEvt_pdf_id2;   //!
   TBranch        *b_mcEvt_pdf_x1;   //!
   TBranch        *b_mcEvt_pdf_x2;   //!
   TBranch        *b_mcEvt_pdf_scale;   //!
   TBranch        *b_mcEvt_pdf1;   //!
   TBranch        *b_mcEvt_pdf2;   //!
   TBranch        *b_mcEvt_weight;   //!
   TBranch        *b_AntiKt4Truth_n;   //!
   TBranch        *b_AntiKt4Truth_E;   //!
   TBranch        *b_AntiKt4Truth_pt;   //!
   TBranch        *b_AntiKt4Truth_m;   //!
   TBranch        *b_AntiKt4Truth_eta;   //!
   TBranch        *b_AntiKt4Truth_phi;   //!
   TBranch        *b_AntiKt4Truth_EtaPhys;   //!
   TBranch        *b_AntiKt4Truth_PhiPhys;   //!
   TBranch        *b_AntiKt4Truth_MPhys;   //!
   TBranch        *b_AntiKt4Truth_WIDTH;   //!
   TBranch        *b_AntiKt4Truth_n90;   //!
   TBranch        *b_AntiKt4Truth_Timing;   //!
   TBranch        *b_AntiKt4Truth_LArQuality;   //!
   TBranch        *b_AntiKt4Truth_nTrk;   //!
   TBranch        *b_AntiKt4Truth_sumPtTrk;   //!
   TBranch        *b_AntiKt4Truth_OriginIndex;   //!
   TBranch        *b_AntiKt4Truth_HECQuality;   //!
   TBranch        *b_AntiKt4Truth_NegativeE;   //!
   TBranch        *b_AntiKt4Truth_BCH_CORR_CELL;   //!
   TBranch        *b_AntiKt4Truth_BCH_CORR_DOTX;   //!
   TBranch        *b_AntiKt4Truth_BCH_CORR_JET;   //!
   TBranch        *b_AntiKt4Truth_BCH_CORR_JET_FORCELL;   //!
   TBranch        *b_AntiKt4Truth_ENG_BAD_CELLS;   //!
   TBranch        *b_AntiKt4Truth_N_BAD_CELLS;   //!
   TBranch        *b_AntiKt4Truth_N_BAD_CELLS_CORR;   //!
   TBranch        *b_AntiKt4Truth_BAD_CELLS_CORR_E;   //!
   TBranch        *b_AntiKt4Truth_SamplingMax;   //!
   TBranch        *b_AntiKt4Truth_fracSamplingMax;   //!
   TBranch        *b_AntiKt4Truth_hecf;   //!
   TBranch        *b_AntiKt4Truth_tgap3f;   //!
   TBranch        *b_AntiKt4Truth_isGood;   //!
   TBranch        *b_AntiKt4Truth_emfrac;   //!
   TBranch        *b_AntiKt4Truth_GCWJES;   //!
   TBranch        *b_AntiKt4Truth_EMJES;   //!
   TBranch        *b_AntiKt4Truth_CB;   //!
   TBranch        *b_AntiKt4Truth_emscale_E;   //!
   TBranch        *b_AntiKt4Truth_emscale_pt;   //!
   TBranch        *b_AntiKt4Truth_emscale_m;   //!
   TBranch        *b_AntiKt4Truth_emscale_eta;   //!
   TBranch        *b_AntiKt4Truth_emscale_phi;   //!
   TBranch        *b_AntiKt4Truth_jvtxf;   //!
   TBranch        *b_AntiKt4Truth_jvtx_x;   //!
   TBranch        *b_AntiKt4Truth_jvtx_y;   //!
   TBranch        *b_AntiKt4Truth_jvtx_z;   //!
   TBranch        *b_AntiKt4Truth_e_PreSamplerB;   //!
   TBranch        *b_AntiKt4Truth_e_EMB1;   //!
   TBranch        *b_AntiKt4Truth_e_EMB2;   //!
   TBranch        *b_AntiKt4Truth_e_EMB3;   //!
   TBranch        *b_AntiKt4Truth_e_PreSamplerE;   //!
   TBranch        *b_AntiKt4Truth_e_EME1;   //!
   TBranch        *b_AntiKt4Truth_e_EME2;   //!
   TBranch        *b_AntiKt4Truth_e_EME3;   //!
   TBranch        *b_AntiKt4Truth_e_HEC0;   //!
   TBranch        *b_AntiKt4Truth_e_HEC1;   //!
   TBranch        *b_AntiKt4Truth_e_HEC2;   //!
   TBranch        *b_AntiKt4Truth_e_HEC3;   //!
   TBranch        *b_AntiKt4Truth_e_TileBar0;   //!
   TBranch        *b_AntiKt4Truth_e_TileBar1;   //!
   TBranch        *b_AntiKt4Truth_e_TileBar2;   //!
   TBranch        *b_AntiKt4Truth_e_TileGap1;   //!
   TBranch        *b_AntiKt4Truth_e_TileGap2;   //!
   TBranch        *b_AntiKt4Truth_e_TileGap3;   //!
   TBranch        *b_AntiKt4Truth_e_TileExt0;   //!
   TBranch        *b_AntiKt4Truth_e_TileExt1;   //!
   TBranch        *b_AntiKt4Truth_e_TileExt2;   //!
   TBranch        *b_AntiKt4Truth_e_FCAL0;   //!
   TBranch        *b_AntiKt4Truth_e_FCAL1;   //!
   TBranch        *b_AntiKt4Truth_e_FCAL2;   //!
   TBranch        *b_AntiKt4Truth_shapeBins;   //!
   TBranch        *b_AntiKt4Truth_Nconst;   //!
   TBranch        *b_AntiKt4Truth_ptconst_emscale;   //!
   TBranch        *b_AntiKt4Truth_econst_emscale;   //!
   TBranch        *b_AntiKt4Truth_etaconst_emscale;   //!
   TBranch        *b_AntiKt4Truth_phiconst_emscale;   //!
   TBranch        *b_AntiKt4Truth_weightconst_emscale;   //!
   TBranch        *b_AntiKt4Truth_constscale_E;   //!
   TBranch        *b_AntiKt4Truth_constscale_pt;   //!
   TBranch        *b_AntiKt4Truth_constscale_m;   //!
   TBranch        *b_AntiKt4Truth_constscale_eta;   //!
   TBranch        *b_AntiKt4Truth_constscale_phi;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_ip2d_pu;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_ip2d_pb;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_ip2d_ntrk;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_ip3d_pu;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_ip3d_pb;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_ip3d_ntrk;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_sv1_pu;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_sv1_pb;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_sv2_pu;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_sv2_pb;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_svp_mass;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_svp_n2t;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_svp_svok;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_svp_ntrk;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_svp_ntrkv;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_svp_ntrkj;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_svp_efrc;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_sv0p_mass;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_sv0p_n2t;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_sv0p_svok;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_sv0p_ntrk;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_sv0p_ntrkv;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_sv0p_ntrkj;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_sv0p_efrc;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_jfit_pu;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_jfit_pb;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_jfit_nvtxnt;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_jfit_nvtx1t;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_jfit_ntrk;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_jfit_efrc;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_jfit_mass;   //!
   TBranch        *b_AntiKt4Truth_flavor_component_jfit_sig3d;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_TrackCounting2D;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_JetProb;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_IP1D;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_IP2D;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_IP3D;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_SV0;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_SV1;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_SV2;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_JetFitterTag;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_JetFitterCOMB;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_JetFitterTagNN;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_JetFitterCOMBNN;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_SoftMuonTag;   //!
   TBranch        *b_AntiKt4Truth_flavor_weight_SoftElectronTag;   //!
   TBranch        *b_AntiKt4Truth_RoIword;   //!
   TBranch        *b_AntiKt4Truth_flavor_truth_dRminToB;   //!
   TBranch        *b_AntiKt4Truth_flavor_truth_dRminToC;   //!
   TBranch        *b_AntiKt4Truth_flavor_truth_dRminToT;   //!
   TBranch        *b_AntiKt4Truth_flavor_truth_BHadronpdg;   //!
   TBranch        *b_AntiKt4Truth_flavor_truth_trueflav;   //!
   TBranch        *b_AntiKt4Truth_el_dr;   //!
   TBranch        *b_AntiKt4Truth_el_matched;   //!
   TBranch        *b_AntiKt4Truth_mu_dr;   //!
   TBranch        *b_AntiKt4Truth_mu_matched;   //!

   BjetTree(TTree *tree=0);
   virtual ~BjetTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef BjetTree_cxx
BjetTree::BjetTree(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("bjet.root");
      if (!f) {
         f = new TFile("bjet.root");
      }
      tree = (TTree*)gDirectory->Get("BjetTree");

   }
   Init(tree);
}

BjetTree::~BjetTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t BjetTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t BjetTree::LoadTree(Long64_t entry)
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

void BjetTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   AntiKt4H1Topo_E = 0;
   AntiKt4H1Topo_pt = 0;
   AntiKt4H1Topo_m = 0;
   AntiKt4H1Topo_eta = 0;
   AntiKt4H1Topo_phi = 0;
   AntiKt4H1Topo_EtaPhys = 0;
   AntiKt4H1Topo_PhiPhys = 0;
   AntiKt4H1Topo_MPhys = 0;
   AntiKt4H1Topo_WIDTH = 0;
   AntiKt4H1Topo_n90 = 0;
   AntiKt4H1Topo_Timing = 0;
   AntiKt4H1Topo_LArQuality = 0;
   AntiKt4H1Topo_nTrk = 0;
   AntiKt4H1Topo_sumPtTrk = 0;
   AntiKt4H1Topo_OriginIndex = 0;
   AntiKt4H1Topo_HECQuality = 0;
   AntiKt4H1Topo_NegativeE = 0;
   AntiKt4H1Topo_BCH_CORR_CELL = 0;
   AntiKt4H1Topo_BCH_CORR_DOTX = 0;
   AntiKt4H1Topo_BCH_CORR_JET = 0;
   AntiKt4H1Topo_BCH_CORR_JET_FORCELL = 0;
   AntiKt4H1Topo_ENG_BAD_CELLS = 0;
   AntiKt4H1Topo_N_BAD_CELLS = 0;
   AntiKt4H1Topo_N_BAD_CELLS_CORR = 0;
   AntiKt4H1Topo_BAD_CELLS_CORR_E = 0;
   AntiKt4H1Topo_SamplingMax = 0;
   AntiKt4H1Topo_fracSamplingMax = 0;
   AntiKt4H1Topo_hecf = 0;
   AntiKt4H1Topo_tgap3f = 0;
   AntiKt4H1Topo_isGood = 0;
   AntiKt4H1Topo_emfrac = 0;
   AntiKt4H1Topo_GCWJES = 0;
   AntiKt4H1Topo_EMJES = 0;
   AntiKt4H1Topo_CB = 0;
   AntiKt4H1Topo_emscale_E = 0;
   AntiKt4H1Topo_emscale_pt = 0;
   AntiKt4H1Topo_emscale_m = 0;
   AntiKt4H1Topo_emscale_eta = 0;
   AntiKt4H1Topo_emscale_phi = 0;
   AntiKt4H1Topo_jvtxf = 0;
   AntiKt4H1Topo_jvtx_x = 0;
   AntiKt4H1Topo_jvtx_y = 0;
   AntiKt4H1Topo_jvtx_z = 0;
   AntiKt4H1Topo_e_PreSamplerB = 0;
   AntiKt4H1Topo_e_EMB1 = 0;
   AntiKt4H1Topo_e_EMB2 = 0;
   AntiKt4H1Topo_e_EMB3 = 0;
   AntiKt4H1Topo_e_PreSamplerE = 0;
   AntiKt4H1Topo_e_EME1 = 0;
   AntiKt4H1Topo_e_EME2 = 0;
   AntiKt4H1Topo_e_EME3 = 0;
   AntiKt4H1Topo_e_HEC0 = 0;
   AntiKt4H1Topo_e_HEC1 = 0;
   AntiKt4H1Topo_e_HEC2 = 0;
   AntiKt4H1Topo_e_HEC3 = 0;
   AntiKt4H1Topo_e_TileBar0 = 0;
   AntiKt4H1Topo_e_TileBar1 = 0;
   AntiKt4H1Topo_e_TileBar2 = 0;
   AntiKt4H1Topo_e_TileGap1 = 0;
   AntiKt4H1Topo_e_TileGap2 = 0;
   AntiKt4H1Topo_e_TileGap3 = 0;
   AntiKt4H1Topo_e_TileExt0 = 0;
   AntiKt4H1Topo_e_TileExt1 = 0;
   AntiKt4H1Topo_e_TileExt2 = 0;
   AntiKt4H1Topo_e_FCAL0 = 0;
   AntiKt4H1Topo_e_FCAL1 = 0;
   AntiKt4H1Topo_e_FCAL2 = 0;
   AntiKt4H1Topo_shapeBins = 0;
   AntiKt4H1Topo_Nconst = 0;
   AntiKt4H1Topo_ptconst_emscale = 0;
   AntiKt4H1Topo_econst_emscale = 0;
   AntiKt4H1Topo_etaconst_emscale = 0;
   AntiKt4H1Topo_phiconst_emscale = 0;
   AntiKt4H1Topo_weightconst_emscale = 0;
   AntiKt4H1Topo_constscale_E = 0;
   AntiKt4H1Topo_constscale_pt = 0;
   AntiKt4H1Topo_constscale_m = 0;
   AntiKt4H1Topo_constscale_eta = 0;
   AntiKt4H1Topo_constscale_phi = 0;
   AntiKt4H1Topo_flavor_component_ip2d_pu = 0;
   AntiKt4H1Topo_flavor_component_ip2d_pb = 0;
   AntiKt4H1Topo_flavor_component_ip2d_ntrk = 0;
   AntiKt4H1Topo_flavor_component_ip3d_pu = 0;
   AntiKt4H1Topo_flavor_component_ip3d_pb = 0;
   AntiKt4H1Topo_flavor_component_ip3d_ntrk = 0;
   AntiKt4H1Topo_flavor_component_sv1_pu = 0;
   AntiKt4H1Topo_flavor_component_sv1_pb = 0;
   AntiKt4H1Topo_flavor_component_sv2_pu = 0;
   AntiKt4H1Topo_flavor_component_sv2_pb = 0;
   AntiKt4H1Topo_flavor_component_svp_mass = 0;
   AntiKt4H1Topo_flavor_component_svp_n2t = 0;
   AntiKt4H1Topo_flavor_component_svp_svok = 0;
   AntiKt4H1Topo_flavor_component_svp_ntrk = 0;
   AntiKt4H1Topo_flavor_component_svp_ntrkv = 0;
   AntiKt4H1Topo_flavor_component_svp_ntrkj = 0;
   AntiKt4H1Topo_flavor_component_svp_efrc = 0;
   AntiKt4H1Topo_flavor_component_sv0p_mass = 0;
   AntiKt4H1Topo_flavor_component_sv0p_n2t = 0;
   AntiKt4H1Topo_flavor_component_sv0p_svok = 0;
   AntiKt4H1Topo_flavor_component_sv0p_ntrk = 0;
   AntiKt4H1Topo_flavor_component_sv0p_ntrkv = 0;
   AntiKt4H1Topo_flavor_component_sv0p_ntrkj = 0;
   AntiKt4H1Topo_flavor_component_sv0p_efrc = 0;
   AntiKt4H1Topo_flavor_component_jfit_pu = 0;
   AntiKt4H1Topo_flavor_component_jfit_pb = 0;
   AntiKt4H1Topo_flavor_component_jfit_nvtxnt = 0;
   AntiKt4H1Topo_flavor_component_jfit_nvtx1t = 0;
   AntiKt4H1Topo_flavor_component_jfit_ntrk = 0;
   AntiKt4H1Topo_flavor_component_jfit_efrc = 0;
   AntiKt4H1Topo_flavor_component_jfit_mass = 0;
   AntiKt4H1Topo_flavor_component_jfit_sig3d = 0;
   AntiKt4H1Topo_flavor_weight = 0;
   AntiKt4H1Topo_flavor_weight_TrackCounting2D = 0;
   AntiKt4H1Topo_flavor_weight_JetProb = 0;
   AntiKt4H1Topo_flavor_weight_IP1D = 0;
   AntiKt4H1Topo_flavor_weight_IP2D = 0;
   AntiKt4H1Topo_flavor_weight_IP3D = 0;
   AntiKt4H1Topo_flavor_weight_SV0 = 0;
   AntiKt4H1Topo_flavor_weight_SV1 = 0;
   AntiKt4H1Topo_flavor_weight_SV2 = 0;
   AntiKt4H1Topo_flavor_weight_JetFitterTag = 0;
   AntiKt4H1Topo_flavor_weight_JetFitterCOMB = 0;
   AntiKt4H1Topo_flavor_weight_JetFitterTagNN = 0;
   AntiKt4H1Topo_flavor_weight_JetFitterCOMBNN = 0;
   AntiKt4H1Topo_flavor_weight_SoftMuonTag = 0;
   AntiKt4H1Topo_flavor_weight_SoftElectronTag = 0;
   AntiKt4H1Topo_RoIword = 0;
   AntiKt4H1Topo_flavor_truth_dRminToB = 0;
   AntiKt4H1Topo_flavor_truth_dRminToC = 0;
   AntiKt4H1Topo_flavor_truth_dRminToT = 0;
   AntiKt4H1Topo_flavor_truth_BHadronpdg = 0;
   AntiKt4H1Topo_flavor_truth_trueflav = 0;
   AntiKt4H1Topo_el_dr = 0;
   AntiKt4H1Topo_el_matched = 0;
   AntiKt4H1Topo_mu_dr = 0;
   AntiKt4H1Topo_mu_matched = 0;
   trig_Nav_chain_ChainId = 0;
   trig_Nav_chain_RoIType = 0;
   trig_Nav_chain_RoIIndex = 0;
   trig_RoI_L2_b_type = 0;
   trig_RoI_L2_b_lastStep = 0;
   trig_RoI_L2_b_Jet_ROI = 0;
   trig_RoI_L2_b_Jet_ROIStatus = 0;
   trig_RoI_L2_b_TrigL2BjetContainer = 0;
   trig_RoI_L2_b_TrigL2BjetContainerStatus = 0;
   trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_Jet = 0;
   trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_JetStatus = 0;
   trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_Jet = 0;
   trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_JetStatus = 0;
   trig_RoI_EF_b_type = 0;
   trig_RoI_EF_b_lastStep = 0;
   trig_RoI_EF_b_Jet_ROI = 0;
   trig_RoI_EF_b_Jet_ROIStatus = 0;
   trig_RoI_EF_b_TrigEFBjetContainer = 0;
   trig_RoI_EF_b_TrigEFBjetContainerStatus = 0;
   trig_RoI_EF_b_Rec__TrackParticleContainer = 0;
   trig_RoI_EF_b_Rec__TrackParticleContainerStatus = 0;
   trig_L1_jet_eta = 0;
   trig_L1_jet_phi = 0;
   trig_L1_jet_thrNames = 0;
   trig_L1_jet_thrValues = 0;
   trig_L1_jet_thrPattern = 0;
   trig_L1_jet_et4x4 = 0;
   trig_L1_jet_et6x6 = 0;
   trig_L1_jet_et8x8 = 0;
   trig_L1_jet_RoIWord = 0;
   trig_L1_TAV = 0;
   trig_L2_passedPhysics = 0;
   trig_EF_passedPhysics = 0;
   trig_L1_TBP = 0;
   trig_L1_TAP = 0;
   trig_L2_passedRaw = 0;
   trig_EF_passedRaw = 0;
   trig_L2_resurrected = 0;
   trig_EF_resurrected = 0;
   trig_L2_passedThrough = 0;
   trig_EF_passedThrough = 0;
   trig_l2sitrk_jet_algorithmId = 0;
   trig_l2sitrk_jet_trackStatus = 0;
   trig_l2sitrk_jet_chi2Ndof = 0;
   trig_l2sitrk_jet_nStrawHits = 0;
   trig_l2sitrk_jet_nHighThrHits = 0;
   trig_l2sitrk_jet_nPixelSpacePoints = 0;
   trig_l2sitrk_jet_nSCT_SpacePoints = 0;
   trig_l2sitrk_jet_hitPattern = 0;
   trig_l2sitrk_jet_nStraw = 0;
   trig_l2sitrk_jet_nStrawTime = 0;
   trig_l2sitrk_jet_a0 = 0;
   trig_l2sitrk_jet_z0 = 0;
   trig_l2sitrk_jet_phi0 = 0;
   trig_l2sitrk_jet_eta = 0;
   trig_l2sitrk_jet_pt = 0;
   trig_l2sitrk_jet_covariance = 0;
   trig_l2idtrk_jet_algorithmId = 0;
   trig_l2idtrk_jet_trackStatus = 0;
   trig_l2idtrk_jet_chi2Ndof = 0;
   trig_l2idtrk_jet_nStrawHits = 0;
   trig_l2idtrk_jet_nHighThrHits = 0;
   trig_l2idtrk_jet_nPixelSpacePoints = 0;
   trig_l2idtrk_jet_nSCT_SpacePoints = 0;
   trig_l2idtrk_jet_hitPattern = 0;
   trig_l2idtrk_jet_nStraw = 0;
   trig_l2idtrk_jet_nStrawTime = 0;
   trig_l2idtrk_jet_a0 = 0;
   trig_l2idtrk_jet_z0 = 0;
   trig_l2idtrk_jet_phi0 = 0;
   trig_l2idtrk_jet_eta = 0;
   trig_l2idtrk_jet_pt = 0;
   trig_l2idtrk_jet_covariance = 0;
   trig_bjefid_d0 = 0;
   trig_bjefid_z0 = 0;
   trig_bjefid_phi = 0;
   trig_bjefid_theta = 0;
   trig_bjefid_qoverp = 0;
   trig_bjefid_pt = 0;
   trig_bjefid_eta = 0;
   trig_bjefid_err_d0 = 0;
   trig_bjefid_err_z0 = 0;
   trig_bjefid_err_phi = 0;
   trig_bjefid_err_theta = 0;
   trig_bjefid_err_qoverp = 0;
   trig_bjefid_cov_d0_z0 = 0;
   trig_bjefid_cov_d0_phi = 0;
   trig_bjefid_cov_d0_theta = 0;
   trig_bjefid_cov_d0_qoverp = 0;
   trig_bjefid_cov_z0_phi = 0;
   trig_bjefid_cov_z0_theta = 0;
   trig_bjefid_cov_z0_qoverp = 0;
   trig_bjefid_cov_phi_theta = 0;
   trig_bjefid_cov_phi_qoverp = 0;
   trig_bjefid_cov_theta_qoverp = 0;
   trig_bjefid_d0_wrtPV = 0;
   trig_bjefid_z0_wrtPV = 0;
   trig_bjefid_phi_wrtPV = 0;
   trig_bjefid_err_d0_wrtPV = 0;
   trig_bjefid_err_z0_wrtPV = 0;
   trig_bjefid_err_phi_wrtPV = 0;
   trig_bjefid_err_theta_wrtPV = 0;
   trig_bjefid_err_qoverp_wrtPV = 0;
   trig_bjefid_cov_d0_z0_wrtPV = 0;
   trig_bjefid_cov_d0_phi_wrtPV = 0;
   trig_bjefid_cov_d0_theta_wrtPV = 0;
   trig_bjefid_cov_d0_qoverp_wrtPV = 0;
   trig_bjefid_cov_z0_phi_wrtPV = 0;
   trig_bjefid_cov_z0_theta_wrtPV = 0;
   trig_bjefid_cov_z0_qoverp_wrtPV = 0;
   trig_bjefid_cov_phi_theta_wrtPV = 0;
   trig_bjefid_cov_phi_qoverp_wrtPV = 0;
   trig_bjefid_cov_theta_qoverp_wrtPV = 0;
   trig_bjefid_d0_wrtBS = 0;
   trig_bjefid_z0_wrtBS = 0;
   trig_bjefid_phi_wrtBS = 0;
   trig_bjefid_err_d0_wrtBS = 0;
   trig_bjefid_err_z0_wrtBS = 0;
   trig_bjefid_err_phi_wrtBS = 0;
   trig_bjefid_err_theta_wrtBS = 0;
   trig_bjefid_err_qoverp_wrtBS = 0;
   trig_bjefid_cov_d0_z0_wrtBS = 0;
   trig_bjefid_cov_d0_phi_wrtBS = 0;
   trig_bjefid_cov_d0_theta_wrtBS = 0;
   trig_bjefid_cov_d0_qoverp_wrtBS = 0;
   trig_bjefid_cov_z0_phi_wrtBS = 0;
   trig_bjefid_cov_z0_theta_wrtBS = 0;
   trig_bjefid_cov_z0_qoverp_wrtBS = 0;
   trig_bjefid_cov_phi_theta_wrtBS = 0;
   trig_bjefid_cov_phi_qoverp_wrtBS = 0;
   trig_bjefid_cov_theta_qoverp_wrtBS = 0;
   trig_bjefid_chi2 = 0;
   trig_bjefid_ndof = 0;
   trig_bjefid_nBLHits = 0;
   trig_bjefid_nPixHits = 0;
   trig_bjefid_nSCTHits = 0;
   trig_bjefid_nTRTHits = 0;
   trig_bjefid_nPixHoles = 0;
   trig_bjefid_nSCTHoles = 0;
   trig_bjefid_nMDTHits = 0;
   trig_bjefid_nCSCEtaHits = 0;
   trig_bjefid_nCSCPhiHits = 0;
   trig_bjefid_nRPCEtaHits = 0;
   trig_bjefid_nRPCPhiHits = 0;
   trig_bjefid_nTGCEtaHits = 0;
   trig_bjefid_nTGCPhiHits = 0;
   trig_bjefid_nHits = 0;
   trig_bjefid_nHoles = 0;
   trig_bjefid_hitPattern = 0;
   trig_bjefid_TRTHighTHitsRatio = 0;
   trig_bjefid_pixeldEdx = 0;
   trig_bjefid_fitter = 0;
   trig_bjefid_patternReco1 = 0;
   trig_bjefid_patternReco2 = 0;
   trig_bjefid_seedFinder = 0;
   trig_bjefid_blayerPrediction_expectHit = 0;
   trig_bjefid_blayerPrediction_x = 0;
   trig_bjefid_blayerPrediction_y = 0;
   trig_bjefid_blayerPrediction_z = 0;
   trig_bjefid_blayerPrediction_locX = 0;
   trig_bjefid_blayerPrediction_locY = 0;
   trig_bjefid_blayerPrediction_err_locX = 0;
   trig_bjefid_blayerPrediction_err_locY = 0;
   trig_bjefid_blayerPrediction_etaDistToEdge = 0;
   trig_bjefid_blayerPrediction_phiDistToEdge = 0;
   trig_bjefid_blayerPrediction_detElementId = 0;
   trig_bjefid_blayerPrediction_row = 0;
   trig_bjefid_blayerPrediction_col = 0;
   trig_bjefid_blayerPrediction_type = 0;
   trig_bjefid_mc_probability = 0;
   trig_bjefid_mc_barcode = 0;
   trig_bjefid_mc_index = 0;
   trig_bjefid_vertx = 0;
   trig_bjefid_verty = 0;
   trig_bjefid_vertz = 0;
   trig_l2bj_roiId = 0;
   trig_l2bj_valid = 0;
   trig_l2bj_prmVtx = 0;
   trig_l2bj_xComb = 0;
   trig_l2bj_xIP1D = 0;
   trig_l2bj_xIP2D = 0;
   trig_l2bj_xIP3D = 0;
   trig_l2bj_xCHI2 = 0;
   trig_l2bj_xSV = 0;
   trig_l2bj_xMVtx = 0;
   trig_l2bj_xEVtx = 0;
   trig_l2bj_xNVtx = 0;
   trig_l2bj_eta = 0;
   trig_l2bj_phi = 0;
   trig_l2bj_trksi_n = 0;
   trig_l2bj_trksi_index = 0;
   trig_l2bj_trkid_n = 0;
   trig_l2bj_trkid_index = 0;
   trig_efbj_roiId = 0;
   trig_efbj_valid = 0;
   trig_efbj_prmVtx = 0;
   trig_efbj_xComb = 0;
   trig_efbj_xIP1D = 0;
   trig_efbj_xIP2D = 0;
   trig_efbj_xIP3D = 0;
   trig_efbj_xCHI2 = 0;
   trig_efbj_xSV = 0;
   trig_efbj_xMVtx = 0;
   trig_efbj_xEVtx = 0;
   trig_efbj_xNVtx = 0;
   trig_efbj_eta = 0;
   trig_efbj_phi = 0;
   trig_efbj_trk_n = 0;
   trig_efbj_trk_index = 0;
   el_E = 0;
   el_Et = 0;
   el_pt = 0;
   el_m = 0;
   el_eta = 0;
   el_phi = 0;
   el_px = 0;
   el_py = 0;
   el_pz = 0;
   el_charge = 0;
   el_author = 0;
   el_isEM = 0;
   el_convFlag = 0;
   el_isConv = 0;
   el_nConv = 0;
   el_nSingleTrackConv = 0;
   el_nDoubleTrackConv = 0;
   el_type = 0;
   el_origin = 0;
   el_truth_E = 0;
   el_truth_pt = 0;
   el_truth_eta = 0;
   el_truth_phi = 0;
   el_truth_type = 0;
   el_truth_status = 0;
   el_truth_barcode = 0;
   el_truth_mothertype = 0;
   el_truth_motherbarcode = 0;
   el_truth_hasHardBrem = 0;
   el_truth_matched = 0;
   el_loose = 0;
   el_medium = 0;
   el_mediumIso = 0;
   el_tight = 0;
   el_tightIso = 0;
   el_Ethad = 0;
   el_Ethad1 = 0;
   el_f1 = 0;
   el_f1core = 0;
   el_Emins1 = 0;
   el_fside = 0;
   el_Emax2 = 0;
   el_ws3 = 0;
   el_wstot = 0;
   el_emaxs1 = 0;
   el_deltaEs = 0;
   el_E233 = 0;
   el_E237 = 0;
   el_E277 = 0;
   el_weta2 = 0;
   el_f3 = 0;
   el_f3core = 0;
   el_rphiallcalo = 0;
   el_Etcone45 = 0;
   el_Etcone20 = 0;
   el_Etcone30 = 0;
   el_Etcone40 = 0;
   el_ptcone30 = 0;
   el_convanglematch = 0;
   el_convtrackmatch = 0;
   el_hasconv = 0;
   el_convvtxx = 0;
   el_convvtxy = 0;
   el_convvtxz = 0;
   el_Rconv = 0;
   el_zconv = 0;
   el_convvtxchi2 = 0;
   el_pt1conv = 0;
   el_convtrk1nBLHits = 0;
   el_convtrk1nPixHits = 0;
   el_convtrk1nSCTHits = 0;
   el_convtrk1nTRTHits = 0;
   el_pt2conv = 0;
   el_convtrk2nBLHits = 0;
   el_convtrk2nPixHits = 0;
   el_convtrk2nSCTHits = 0;
   el_convtrk2nTRTHits = 0;
   el_ptconv = 0;
   el_pzconv = 0;
   el_pos7 = 0;
   el_etacorrmag = 0;
   el_deltaeta1 = 0;
   el_deltaeta2 = 0;
   el_deltaphi2 = 0;
   el_deltaphiRescaled = 0;
   el_expectHitInBLayer = 0;
   el_reta = 0;
   el_rphi = 0;
   el_EtringnoisedR03sig2 = 0;
   el_EtringnoisedR03sig3 = 0;
   el_EtringnoisedR03sig4 = 0;
   el_isolationlikelihoodjets = 0;
   el_isolationlikelihoodhqelectrons = 0;
   el_electronweight = 0;
   el_electronbgweight = 0;
   el_softeweight = 0;
   el_softebgweight = 0;
   el_neuralnet = 0;
   el_Hmatrix = 0;
   el_Hmatrix5 = 0;
   el_adaboost = 0;
   el_softeneuralnet = 0;
   el_zvertex = 0;
   el_errz = 0;
   el_etap = 0;
   el_depth = 0;
   el_refittedTrack_n = 0;
   el_refittedTrack_author = 0;
   el_refittedTrack_chi2 = 0;
   el_refittedTrack_qoverp = 0;
   el_refittedTrack_d0 = 0;
   el_refittedTrack_z0 = 0;
   el_refittedTrack_theta = 0;
   el_refittedTrack_phi = 0;
   el_refittedTrack_LMqoverp = 0;
   el_refittedTrack_hasBrem = 0;
   el_refittedTrack_bremRadius = 0;
   el_refittedTrack_bremZ = 0;
   el_refittedTrack_bremRadiusErr = 0;
   el_refittedTrack_bremZErr = 0;
   el_Es0 = 0;
   el_etas0 = 0;
   el_phis0 = 0;
   el_Es1 = 0;
   el_etas1 = 0;
   el_phis1 = 0;
   el_Es2 = 0;
   el_etas2 = 0;
   el_phis2 = 0;
   el_Es3 = 0;
   el_etas3 = 0;
   el_phis3 = 0;
   el_E_PreSamplerB = 0;
   el_E_EMB1 = 0;
   el_E_EMB2 = 0;
   el_E_EMB3 = 0;
   el_E_PreSamplerE = 0;
   el_E_EME1 = 0;
   el_E_EME2 = 0;
   el_E_EME3 = 0;
   el_E_HEC0 = 0;
   el_E_HEC1 = 0;
   el_E_HEC2 = 0;
   el_E_HEC3 = 0;
   el_E_TileBar0 = 0;
   el_E_TileBar1 = 0;
   el_E_TileBar2 = 0;
   el_E_TileGap1 = 0;
   el_E_TileGap2 = 0;
   el_E_TileGap3 = 0;
   el_E_TileExt0 = 0;
   el_E_TileExt1 = 0;
   el_E_TileExt2 = 0;
   el_E_FCAL0 = 0;
   el_E_FCAL1 = 0;
   el_E_FCAL2 = 0;
   el_cl_E = 0;
   el_cl_pt = 0;
   el_cl_eta = 0;
   el_cl_phi = 0;
   el_firstEdens = 0;
   el_cellmaxfrac = 0;
   el_longitudinal = 0;
   el_secondlambda = 0;
   el_lateral = 0;
   el_secondR = 0;
   el_centerlambda = 0;
   el_rawcl_Es0 = 0;
   el_rawcl_etas0 = 0;
   el_rawcl_phis0 = 0;
   el_rawcl_Es1 = 0;
   el_rawcl_etas1 = 0;
   el_rawcl_phis1 = 0;
   el_rawcl_Es2 = 0;
   el_rawcl_etas2 = 0;
   el_rawcl_phis2 = 0;
   el_rawcl_Es3 = 0;
   el_rawcl_etas3 = 0;
   el_rawcl_phis3 = 0;
   el_rawcl_E = 0;
   el_rawcl_pt = 0;
   el_rawcl_eta = 0;
   el_rawcl_phi = 0;
   el_refittedtrackcovd0 = 0;
   el_refittedtrackcovz0 = 0;
   el_refittedtrackcovphi = 0;
   el_refittedtrackcovtheta = 0;
   el_refittedtrackcovqoverp = 0;
   el_refittedtrackcovd0z0 = 0;
   el_refittedtrackcovz0phi = 0;
   el_refittedtrackcovz0theta = 0;
   el_refittedtrackcovz0qoverp = 0;
   el_refittedtrackcovd0phi = 0;
   el_refittedtrackcovd0theta = 0;
   el_refittedtrackcovd0qoverp = 0;
   el_refittedtrackcovphitheta = 0;
   el_refittedtrackcovphiqoverp = 0;
   el_refittedtrackcovthetaqoverp = 0;
   el_trackd0 = 0;
   el_trackz0 = 0;
   el_trackphi = 0;
   el_tracktheta = 0;
   el_trackqoverp = 0;
   el_trackpt = 0;
   el_tracketa = 0;
   el_trackcov_d0 = 0;
   el_trackcov_z0 = 0;
   el_trackcov_phi = 0;
   el_trackcov_theta = 0;
   el_trackcov_qoverp = 0;
   el_trackcov_d0_z0 = 0;
   el_trackcov_d0_phi = 0;
   el_trackcov_d0_theta = 0;
   el_trackcov_d0_qoverp = 0;
   el_trackcov_z0_phi = 0;
   el_trackcov_z0_theta = 0;
   el_trackcov_z0_qoverp = 0;
   el_trackcov_phi_theta = 0;
   el_trackcov_phi_qoverp = 0;
   el_trackcov_theta_qoverp = 0;
   el_trackfitchi2 = 0;
   el_trackfitndof = 0;
   el_nBLHits = 0;
   el_nPixHits = 0;
   el_nSCTHits = 0;
   el_nTRTHits = 0;
   el_nPixHoles = 0;
   el_nSCTHoles = 0;
   el_nBLSharedHits = 0;
   el_nPixSharedHits = 0;
   el_nSCTSharedHits = 0;
   el_nTRTHighTHits = 0;
   el_nTRTOutliers = 0;
   el_nTRTHighTOutliers = 0;
   el_nSiHits = 0;
   el_TRTHighTHitsRatio = 0;
   el_pixeldEdx = 0;
   el_eProbabilityComb = 0;
   el_eProbabilityHT = 0;
   el_eProbabilityToT = 0;
   el_eProbabilityBrem = 0;
   el_vertx = 0;
   el_verty = 0;
   el_vertz = 0;
   el_hastrack = 0;
   el_deltaEmax2 = 0;
   el_calibHitsShowerDepth = 0;
   el_trackd0beam = 0;
   el_tracksigd0beam = 0;
   el_trackd0pv = 0;
   el_tracksigd0pv = 0;
   el_trackz0pv = 0;
   el_tracksigz0pv = 0;
   el_trackd0pvunbiased = 0;
   el_tracksigd0pvunbiased = 0;
   el_trackz0pvunbiased = 0;
   el_tracksigz0pvunbiased = 0;
   el_isIso = 0;
   el_mvaptcone20 = 0;
   el_mvaptcone30 = 0;
   el_mvaptcone40 = 0;
   el_jet_dr = 0;
   el_jet_E = 0;
   el_jet_pt = 0;
   el_jet_m = 0;
   el_jet_eta = 0;
   el_jet_phi = 0;
   el_jet_truth_dr = 0;
   el_jet_truth_E = 0;
   el_jet_truth_pt = 0;
   el_jet_truth_m = 0;
   el_jet_truth_eta = 0;
   el_jet_truth_phi = 0;
   el_jet_truth_matched = 0;
   el_jet_matched = 0;
   el_Etcone40_pt_corrected = 0;
   el_Etcone40_ED_corrected = 0;
   el_Etcone40_corrected = 0;
   el_EF_dr = 0;
   el_EF_index = 0;
   el_L2_dr = 0;
   el_L2_index = 0;
   el_L1_dr = 0;
   el_L1_index = 0;
   ph_E = 0;
   ph_Et = 0;
   ph_pt = 0;
   ph_m = 0;
   ph_eta = 0;
   ph_phi = 0;
   ph_px = 0;
   ph_py = 0;
   ph_pz = 0;
   ph_author = 0;
   ph_isRecovered = 0;
   ph_isEM = 0;
   ph_convFlag = 0;
   ph_isConv = 0;
   ph_nConv = 0;
   ph_nSingleTrackConv = 0;
   ph_nDoubleTrackConv = 0;
   ph_type = 0;
   ph_origin = 0;
   ph_truth_deltaRRecPhoton = 0;
   ph_truth_E = 0;
   ph_truth_pt = 0;
   ph_truth_eta = 0;
   ph_truth_phi = 0;
   ph_truth_type = 0;
   ph_truth_status = 0;
   ph_truth_barcode = 0;
   ph_truth_mothertype = 0;
   ph_truth_motherbarcode = 0;
   ph_truth_matched = 0;
   ph_loose = 0;
   ph_tight = 0;
   ph_tightIso = 0;
   ph_Ethad = 0;
   ph_Ethad1 = 0;
   ph_E033 = 0;
   ph_f1 = 0;
   ph_f1core = 0;
   ph_Emins1 = 0;
   ph_fside = 0;
   ph_Emax2 = 0;
   ph_ws3 = 0;
   ph_wstot = 0;
   ph_E132 = 0;
   ph_E1152 = 0;
   ph_emaxs1 = 0;
   ph_deltaEs = 0;
   ph_E233 = 0;
   ph_E237 = 0;
   ph_E277 = 0;
   ph_weta2 = 0;
   ph_f3 = 0;
   ph_f3core = 0;
   ph_rphiallcalo = 0;
   ph_Etcone45 = 0;
   ph_Etcone20 = 0;
   ph_Etcone30 = 0;
   ph_Etcone40 = 0;
   ph_ptcone30 = 0;
   ph_convanglematch = 0;
   ph_convtrackmatch = 0;
   ph_hasconv = 0;
   ph_convvtxx = 0;
   ph_convvtxy = 0;
   ph_convvtxz = 0;
   ph_Rconv = 0;
   ph_zconv = 0;
   ph_convvtxchi2 = 0;
   ph_pt1conv = 0;
   ph_convtrk1nBLHits = 0;
   ph_convtrk1nPixHits = 0;
   ph_convtrk1nSCTHits = 0;
   ph_convtrk1nTRTHits = 0;
   ph_pt2conv = 0;
   ph_convtrk2nBLHits = 0;
   ph_convtrk2nPixHits = 0;
   ph_convtrk2nSCTHits = 0;
   ph_convtrk2nTRTHits = 0;
   ph_ptconv = 0;
   ph_pzconv = 0;
   ph_reta = 0;
   ph_rphi = 0;
   ph_EtringnoisedR03sig2 = 0;
   ph_EtringnoisedR03sig3 = 0;
   ph_EtringnoisedR03sig4 = 0;
   ph_isolationlikelihoodjets = 0;
   ph_isolationlikelihoodhqelectrons = 0;
   ph_loglikelihood = 0;
   ph_photonweight = 0;
   ph_photonbgweight = 0;
   ph_neuralnet = 0;
   ph_Hmatrix = 0;
   ph_Hmatrix5 = 0;
   ph_adaboost = 0;
   ph_zvertex = 0;
   ph_errz = 0;
   ph_etap = 0;
   ph_depth = 0;
   ph_cl_E = 0;
   ph_cl_pt = 0;
   ph_cl_eta = 0;
   ph_cl_phi = 0;
   ph_Es0 = 0;
   ph_etas0 = 0;
   ph_phis0 = 0;
   ph_Es1 = 0;
   ph_etas1 = 0;
   ph_phis1 = 0;
   ph_Es2 = 0;
   ph_etas2 = 0;
   ph_phis2 = 0;
   ph_Es3 = 0;
   ph_etas3 = 0;
   ph_phis3 = 0;
   ph_rawcl_Es0 = 0;
   ph_rawcl_etas0 = 0;
   ph_rawcl_phis0 = 0;
   ph_rawcl_Es1 = 0;
   ph_rawcl_etas1 = 0;
   ph_rawcl_phis1 = 0;
   ph_rawcl_Es2 = 0;
   ph_rawcl_etas2 = 0;
   ph_rawcl_phis2 = 0;
   ph_rawcl_Es3 = 0;
   ph_rawcl_etas3 = 0;
   ph_rawcl_phis3 = 0;
   ph_rawcl_E = 0;
   ph_rawcl_pt = 0;
   ph_rawcl_eta = 0;
   ph_rawcl_phi = 0;
   ph_truth_isConv = 0;
   ph_truth_isBrem = 0;
   ph_truth_isFromHardProc = 0;
   ph_truth_isPhotonFromHardProc = 0;
   ph_truth_Rconv = 0;
   ph_truth_zconv = 0;
   ph_deltaEmax2 = 0;
   ph_calibHitsShowerDepth = 0;
   ph_isIso = 0;
   ph_mvaptcone20 = 0;
   ph_mvaptcone30 = 0;
   ph_mvaptcone40 = 0;
   ph_topoEtcone20 = 0;
   ph_topoEtcone40 = 0;
   ph_topoEtcone60 = 0;
   ph_jet_dr = 0;
   ph_jet_E = 0;
   ph_jet_pt = 0;
   ph_jet_m = 0;
   ph_jet_eta = 0;
   ph_jet_phi = 0;
   ph_jet_truth_dr = 0;
   ph_jet_truth_E = 0;
   ph_jet_truth_pt = 0;
   ph_jet_truth_m = 0;
   ph_jet_truth_eta = 0;
   ph_jet_truth_phi = 0;
   ph_jet_truth_matched = 0;
   ph_jet_matched = 0;
   ph_convIP = 0;
   ph_convIPRev = 0;
   ph_ptIsolationCone = 0;
   ph_ptIsolationConePhAngle = 0;
   ph_Etcone40_pt_corrected = 0;
   ph_Etcone40_ED_corrected = 0;
   ph_Etcone40_corrected = 0;
   ph_topodr = 0;
   ph_topopt = 0;
   ph_topoeta = 0;
   ph_topophi = 0;
   ph_topomatched = 0;
   ph_topoEMdr = 0;
   ph_topoEMpt = 0;
   ph_topoEMeta = 0;
   ph_topoEMphi = 0;
   ph_topoEMmatched = 0;
   ph_EF_dr = 0;
   ph_EF_index = 0;
   ph_L2_dr = 0;
   ph_L2_index = 0;
   ph_L1_dr = 0;
   ph_L1_index = 0;
   mu_E = 0;
   mu_pt = 0;
   mu_m = 0;
   mu_eta = 0;
   mu_phi = 0;
   mu_px = 0;
   mu_py = 0;
   mu_pz = 0;
   mu_charge = 0;
   mu_allauthor = 0;
   mu_author = 0;
   mu_matchchi2 = 0;
   mu_matchndof = 0;
   mu_etcone20 = 0;
   mu_etcone30 = 0;
   mu_etcone40 = 0;
   mu_nucone20 = 0;
   mu_nucone30 = 0;
   mu_nucone40 = 0;
   mu_ptcone20 = 0;
   mu_ptcone30 = 0;
   mu_ptcone40 = 0;
   mu_energyLossPar = 0;
   mu_energyLossErr = 0;
   mu_etCore = 0;
   mu_energyLossType = 0;
   mu_bestMatch = 0;
   mu_isStandAloneMuon = 0;
   mu_isCombinedMuon = 0;
   mu_isLowPtReconstructedMuon = 0;
   mu_loose = 0;
   mu_medium = 0;
   mu_tight = 0;
   mu_d0_exPV = 0;
   mu_z0_exPV = 0;
   mu_phi_exPV = 0;
   mu_theta_exPV = 0;
   mu_qoverp_exPV = 0;
   mu_cb_d0_exPV = 0;
   mu_cb_z0_exPV = 0;
   mu_cb_phi_exPV = 0;
   mu_cb_theta_exPV = 0;
   mu_cb_qoverp_exPV = 0;
   mu_id_d0_exPV = 0;
   mu_id_z0_exPV = 0;
   mu_id_phi_exPV = 0;
   mu_id_theta_exPV = 0;
   mu_id_qoverp_exPV = 0;
   mu_me_d0_exPV = 0;
   mu_me_z0_exPV = 0;
   mu_me_phi_exPV = 0;
   mu_me_theta_exPV = 0;
   mu_me_qoverp_exPV = 0;
   mu_ie_d0_exPV = 0;
   mu_ie_z0_exPV = 0;
   mu_ie_phi_exPV = 0;
   mu_ie_theta_exPV = 0;
   mu_ie_qoverp_exPV = 0;
   mu_cov_d0_exPV = 0;
   mu_cov_z0_exPV = 0;
   mu_cov_phi_exPV = 0;
   mu_cov_theta_exPV = 0;
   mu_cov_qoverp_exPV = 0;
   mu_cov_d0_z0_exPV = 0;
   mu_cov_d0_phi_exPV = 0;
   mu_cov_d0_theta_exPV = 0;
   mu_cov_d0_qoverp_exPV = 0;
   mu_cov_z0_phi_exPV = 0;
   mu_cov_z0_theta_exPV = 0;
   mu_cov_z0_qoverp_exPV = 0;
   mu_cov_phi_theta_exPV = 0;
   mu_cov_phi_qoverp_exPV = 0;
   mu_cov_theta_qoverp_exPV = 0;
   mu_ms_d0 = 0;
   mu_ms_z0 = 0;
   mu_ms_phi = 0;
   mu_ms_theta = 0;
   mu_ms_qoverp = 0;
   mu_id_d0 = 0;
   mu_id_z0 = 0;
   mu_id_phi = 0;
   mu_id_theta = 0;
   mu_id_qoverp = 0;
   mu_me_d0 = 0;
   mu_me_z0 = 0;
   mu_me_phi = 0;
   mu_me_theta = 0;
   mu_me_qoverp = 0;
   mu_ie_d0 = 0;
   mu_ie_z0 = 0;
   mu_ie_phi = 0;
   mu_ie_theta = 0;
   mu_ie_qoverp = 0;
   mu_nBLHits = 0;
   mu_nPixHits = 0;
   mu_nSCTHits = 0;
   mu_nTRTHits = 0;
   mu_nTRTHighTHits = 0;
   mu_nBLSharedHits = 0;
   mu_nPixSharedHits = 0;
   mu_nPixHoles = 0;
   mu_nSCTSharedHits = 0;
   mu_nSCTHoles = 0;
   mu_nTRTOutliers = 0;
   mu_nTRTHighTOutliers = 0;
   mu_nMDTHits = 0;
   mu_nMDTHoles = 0;
   mu_nCSCEtaHits = 0;
   mu_nCSCEtaHoles = 0;
   mu_nCSCPhiHits = 0;
   mu_nCSCPhiHoles = 0;
   mu_nRPCEtaHits = 0;
   mu_nRPCEtaHoles = 0;
   mu_nRPCPhiHits = 0;
   mu_nRPCPhiHoles = 0;
   mu_nTGCEtaHits = 0;
   mu_nTGCEtaHoles = 0;
   mu_nTGCPhiHits = 0;
   mu_nTGCPhiHoles = 0;
   mu_nGangedPixels = 0;
   mu_nOutliersOnTrack = 0;
   mu_nMDTBIHits = 0;
   mu_nMDTBMHits = 0;
   mu_nMDTBOHits = 0;
   mu_nMDTBEEHits = 0;
   mu_nMDTBIS78Hits = 0;
   mu_nMDTEIHits = 0;
   mu_nMDTEMHits = 0;
   mu_nMDTEOHits = 0;
   mu_nMDTEEHits = 0;
   mu_nRPCLayer1EtaHits = 0;
   mu_nRPCLayer2EtaHits = 0;
   mu_nRPCLayer3EtaHits = 0;
   mu_nRPCLayer1PhiHits = 0;
   mu_nRPCLayer2PhiHits = 0;
   mu_nRPCLayer3PhiHits = 0;
   mu_nTGCLayer1EtaHits = 0;
   mu_nTGCLayer2EtaHits = 0;
   mu_nTGCLayer3EtaHits = 0;
   mu_nTGCLayer4EtaHits = 0;
   mu_nTGCLayer1PhiHits = 0;
   mu_nTGCLayer2PhiHits = 0;
   mu_nTGCLayer3PhiHits = 0;
   mu_nTGCLayer4PhiHits = 0;
   mu_barrelSectors = 0;
   mu_endcapSectors = 0;
   mu_trackd0 = 0;
   mu_trackz0 = 0;
   mu_trackphi = 0;
   mu_tracktheta = 0;
   mu_trackqoverp = 0;
   mu_trackcov_d0 = 0;
   mu_trackcov_z0 = 0;
   mu_trackcov_phi = 0;
   mu_trackcov_theta = 0;
   mu_trackcov_qoverp = 0;
   mu_trackcov_d0_z0 = 0;
   mu_trackcov_d0_phi = 0;
   mu_trackcov_d0_theta = 0;
   mu_trackcov_d0_qoverp = 0;
   mu_trackcov_z0_phi = 0;
   mu_trackcov_z0_theta = 0;
   mu_trackcov_z0_qoverp = 0;
   mu_trackcov_phi_theta = 0;
   mu_trackcov_phi_qoverp = 0;
   mu_trackcov_theta_qoverp = 0;
   mu_trackfitchi2 = 0;
   mu_trackfitndof = 0;
   mu_hastrack = 0;
   mu_truth_dr = 0;
   mu_truth_E = 0;
   mu_truth_pt = 0;
   mu_truth_eta = 0;
   mu_truth_phi = 0;
   mu_truth_type = 0;
   mu_truth_status = 0;
   mu_truth_barcode = 0;
   mu_truth_mothertype = 0;
   mu_truth_motherbarcode = 0;
   mu_truth_matched = 0;
   mu_EF_dr = 0;
   mu_EF_cb_eta = 0;
   mu_EF_cb_phi = 0;
   mu_EF_cb_pt = 0;
   mu_EF_ms_eta = 0;
   mu_EF_ms_phi = 0;
   mu_EF_ms_pt = 0;
   mu_EF_me_eta = 0;
   mu_EF_me_phi = 0;
   mu_EF_me_pt = 0;
   mu_EF_matched = 0;
   mu_L2CB_dr = 0;
   mu_L2CB_pt = 0;
   mu_L2CB_eta = 0;
   mu_L2CB_phi = 0;
   mu_L2CB_id_pt = 0;
   mu_L2CB_ms_pt = 0;
   mu_L2CB_nPixHits = 0;
   mu_L2CB_nSCTHits = 0;
   mu_L2CB_nTRTHits = 0;
   mu_L2CB_nTRTHighTHits = 0;
   mu_L2CB_matched = 0;
   mu_L1_dr = 0;
   mu_L1_pt = 0;
   mu_L1_eta = 0;
   mu_L1_phi = 0;
   mu_L1_thrNumber = 0;
   mu_L1_RoINumber = 0;
   mu_L1_sectorAddress = 0;
   mu_L1_firstCandidate = 0;
   mu_L1_moreCandInRoI = 0;
   mu_L1_moreCandInSector = 0;
   mu_L1_source = 0;
   mu_L1_hemisphere = 0;
   mu_L1_matched = 0;
   trig_L1_emtau_eta = 0;
   trig_L1_emtau_phi = 0;
   trig_L1_emtau_thrNames = 0;
   trig_L1_emtau_thrValues = 0;
   trig_L1_emtau_core = 0;
   trig_L1_emtau_EMClus = 0;
   trig_L1_emtau_tauClus = 0;
   trig_L1_emtau_EMIsol = 0;
   trig_L1_emtau_hadIsol = 0;
   trig_L1_emtau_hadCore = 0;
   trig_L1_emtau_thrPattern = 0;
   trig_L1_emtau_L1_2EM10 = 0;
   trig_L1_emtau_L1_2EM10I = 0;
   trig_L1_emtau_L1_2EM14 = 0;
   trig_L1_emtau_L1_2EM3 = 0;
   trig_L1_emtau_L1_2EM3_EM5 = 0;
   trig_L1_emtau_L1_2EM5 = 0;
   trig_L1_emtau_L1_2EM5_MU6 = 0;
   trig_L1_emtau_L1_EM10 = 0;
   trig_L1_emtau_L1_EM10I = 0;
   trig_L1_emtau_L1_EM14 = 0;
   trig_L1_emtau_L1_EM14I = 0;
   trig_L1_emtau_L1_EM14_XE10 = 0;
   trig_L1_emtau_L1_EM14_XE15 = 0;
   trig_L1_emtau_L1_EM19I = 0;
   trig_L1_emtau_L1_EM3 = 0;
   trig_L1_emtau_L1_EM3_MV = 0;
   trig_L1_emtau_L1_EM5 = 0;
   trig_L1_emtau_L1_EM5_MU6 = 0;
   trig_L1_emtau_L1_EM85 = 0;
   trig_L1_emtau_RoIWord = 0;
   trig_L2_emcl_quality = 0;
   trig_L2_emcl_E = 0;
   trig_L2_emcl_Et = 0;
   trig_L2_emcl_eta = 0;
   trig_L2_emcl_phi = 0;
   trig_L2_emcl_E237 = 0;
   trig_L2_emcl_E277 = 0;
   trig_L2_emcl_fracs1 = 0;
   trig_L2_emcl_weta2 = 0;
   trig_L2_emcl_Ehad1 = 0;
   trig_L2_emcl_eta1 = 0;
   trig_L2_emcl_emaxs1 = 0;
   trig_L2_emcl_e2tsts1 = 0;
   trig_L2_emcl_rawEnergy = 0;
   trig_L2_emcl_rawEta = 0;
   trig_L2_emcl_rawPhi = 0;
   trig_L2_emcl_RoIWord = 0;
   trig_L2_emcl_nCells = 0;
   trig_L2_emcl_energyInSample = 0;
   trig_L2_trk_idscan_eGamma_algorithmId = 0;
   trig_L2_trk_idscan_eGamma_trackStatus = 0;
   trig_L2_trk_idscan_eGamma_chi2Ndof = 0;
   trig_L2_trk_idscan_eGamma_nStrawHits = 0;
   trig_L2_trk_idscan_eGamma_nHighThrHits = 0;
   trig_L2_trk_idscan_eGamma_nPixelSpacePoints = 0;
   trig_L2_trk_idscan_eGamma_nSCT_SpacePoints = 0;
   trig_L2_trk_idscan_eGamma_hitPattern = 0;
   trig_L2_trk_idscan_eGamma_nStraw = 0;
   trig_L2_trk_idscan_eGamma_nStrawTime = 0;
   trig_L2_trk_idscan_eGamma_a0 = 0;
   trig_L2_trk_idscan_eGamma_z0 = 0;
   trig_L2_trk_idscan_eGamma_phi0 = 0;
   trig_L2_trk_idscan_eGamma_eta = 0;
   trig_L2_trk_idscan_eGamma_pt = 0;
   trig_L2_trk_idscan_eGamma_covariance = 0;
   trig_L2_trk_sitrack_eGamma_algorithmId = 0;
   trig_L2_trk_sitrack_eGamma_trackStatus = 0;
   trig_L2_trk_sitrack_eGamma_chi2Ndof = 0;
   trig_L2_trk_sitrack_eGamma_nStrawHits = 0;
   trig_L2_trk_sitrack_eGamma_nHighThrHits = 0;
   trig_L2_trk_sitrack_eGamma_nPixelSpacePoints = 0;
   trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints = 0;
   trig_L2_trk_sitrack_eGamma_hitPattern = 0;
   trig_L2_trk_sitrack_eGamma_nStraw = 0;
   trig_L2_trk_sitrack_eGamma_nStrawTime = 0;
   trig_L2_trk_sitrack_eGamma_a0 = 0;
   trig_L2_trk_sitrack_eGamma_z0 = 0;
   trig_L2_trk_sitrack_eGamma_phi0 = 0;
   trig_L2_trk_sitrack_eGamma_eta = 0;
   trig_L2_trk_sitrack_eGamma_pt = 0;
   trig_L2_trk_sitrack_eGamma_covariance = 0;
   trig_L2_el_E = 0;
   trig_L2_el_Et = 0;
   trig_L2_el_pt = 0;
   trig_L2_el_eta = 0;
   trig_L2_el_phi = 0;
   trig_L2_el_RoIWord = 0;
   trig_L2_el_zvertex = 0;
   trig_L2_el_charge = 0;
   trig_L2_el_trackAlgo = 0;
   trig_L2_el_TRTHighTHitsRatio = 0;
   trig_L2_el_deltaeta1 = 0;
   trig_L2_el_deltaphi2 = 0;
   trig_L2_el_EtOverPt = 0;
   trig_L2_el_reta = 0;
   trig_L2_el_Eratio = 0;
   trig_L2_el_Ethad1 = 0;
   trig_L2_el_L2_2e12_tight = 0;
   trig_L2_el_L2_2e15_medium = 0;
   trig_L2_el_L2_2e20_loose = 0;
   trig_L2_el_L2_2e5_medium = 0;
   trig_L2_el_L2_2e5_medium1 = 0;
   trig_L2_el_L2_e105_loose1 = 0;
   trig_L2_el_L2_e10_NoCut = 0;
   trig_L2_el_L2_e10_loose = 0;
   trig_L2_el_L2_e10_loose_IdScan = 0;
   trig_L2_el_L2_e10_loose_SiTrk = 0;
   trig_L2_el_L2_e10_loose_TRT = 0;
   trig_L2_el_L2_e10_loose_mu6 = 0;
   trig_L2_el_L2_e10_medium = 0;
   trig_L2_el_L2_e10_medium_SiTrk = 0;
   trig_L2_el_L2_e10_medium_SiTrk_robust = 0;
   trig_L2_el_L2_e12_medium = 0;
   trig_L2_el_L2_e15_medium = 0;
   trig_L2_el_L2_e15i_medium = 0;
   trig_L2_el_L2_e20_loose = 0;
   trig_L2_el_L2_e20_loose_IdScan = 0;
   trig_L2_el_L2_e20_loose_NoIDTrkCut = 0;
   trig_L2_el_L2_e20_loose_SiTrk = 0;
   trig_L2_el_L2_e20_loose_SiTrk_robust = 0;
   trig_L2_el_L2_e20_loose_TRT = 0;
   trig_L2_el_L2_e20_loose_passEF = 0;
   trig_L2_el_L2_e20_loose_passL2 = 0;
   trig_L2_el_L2_e20_loose_xe20 = 0;
   trig_L2_el_L2_e20_loose_xe30 = 0;
   trig_L2_el_L2_e20_medium = 0;
   trig_L2_el_L2_e20i_loose = 0;
   trig_L2_el_L2_e22i_tight = 0;
   trig_L2_el_L2_e25_loose = 0;
   trig_L2_el_L2_e25_medium = 0;
   trig_L2_el_L2_e25i_loose = 0;
   trig_L2_el_L2_e25i_medium = 0;
   trig_L2_el_L2_e55_loose1 = 0;
   trig_L2_el_L2_e5_medium = 0;
   trig_L2_el_L2_e5_medium1 = 0;
   trig_L2_el_L2_e5_medium_L2SW = 0;
   trig_L2_el_L2_e5_medium_MV = 0;
   trig_L2_el_L2_e5_medium_Ringer = 0;
   trig_L2_el_L2_e5_medium_e10_loose = 0;
   trig_L2_el_L2_em105_passHLT = 0;
   trig_L2_el_L2_em15_passHLT = 0;
   trig_L2_el_L2_em15i_passHLT = 0;
   trig_L2_el_L2_em20_passHLT = 0;
   trig_L2_el_L2_em20i_passHLT = 0;
   trig_L2_el_nTRTHits = 0;
   trig_L2_el_nTRTHighTHits = 0;
   trig_L2_el_trackIndx = 0;
   trig_L2_el_trkPt = 0;
   trig_L2_el_err_trkPt = 0;
   trig_L2_el_err_eta = 0;
   trig_L2_el_err_phi = 0;
   trig_L2_el_errz = 0;
   trig_L2_el_trkEtaAtCalo = 0;
   trig_L2_el_trkPhiAtCalo = 0;
   trig_L2_el_caloEta = 0;
   trig_L2_el_caloPhi = 0;
   trig_L2_el_F0 = 0;
   trig_L2_el_F1 = 0;
   trig_L2_el_F2 = 0;
   trig_L2_el_F3 = 0;
   trig_L2_ph_E = 0;
   trig_L2_ph_Et = 0;
   trig_L2_ph_pt = 0;
   trig_L2_ph_eta = 0;
   trig_L2_ph_phi = 0;
   trig_L2_ph_RoIWord = 0;
   trig_L2_ph_HadEt1 = 0;
   trig_L2_ph_Eratio = 0;
   trig_L2_ph_Reta = 0;
   trig_L2_ph_dPhi = 0;
   trig_L2_ph_dEta = 0;
   trig_L2_ph_F1 = 0;
   trig_L2_ph_L2_2g10_loose = 0;
   trig_L2_ph_L2_2g10_loose_mu6 = 0;
   trig_L2_ph_L2_2g17i_tight = 0;
   trig_L2_ph_L2_2g20_loose = 0;
   trig_L2_ph_L2_g105 = 0;
   trig_L2_ph_L2_g10_loose = 0;
   trig_L2_ph_L2_g10_nocut = 0;
   trig_L2_ph_L2_g150 = 0;
   trig_L2_ph_L2_g15_loose = 0;
   trig_L2_ph_L2_g20_loose = 0;
   trig_L2_ph_L2_g20_loose_cnv = 0;
   trig_L2_ph_L2_g20_loose_xe20 = 0;
   trig_L2_ph_L2_g20i_loose = 0;
   trig_L2_ph_L2_g25_loose = 0;
   trig_L2_ph_L2_g25_loose_xe30 = 0;
   trig_L2_ph_L2_g25i_loose = 0;
   trig_L2_ph_L2_g55_tight = 0;
   trig_L2_ph_Fside = 0;
   trig_L2_ph_Weta2 = 0;
   trig_L2_ph_F0 = 0;
   trig_L2_ph_F2 = 0;
   trig_L2_ph_F3 = 0;
   trig_EF_emcl_E = 0;
   trig_EF_emcl_pt = 0;
   trig_EF_emcl_m = 0;
   trig_EF_emcl_eta = 0;
   trig_EF_emcl_phi = 0;
   trig_EF_emcl_E_em = 0;
   trig_EF_emcl_E_had = 0;
   trig_EF_emcl_firstEdens = 0;
   trig_EF_emcl_cellmaxfrac = 0;
   trig_EF_emcl_longitudinal = 0;
   trig_EF_emcl_secondlambda = 0;
   trig_EF_emcl_lateral = 0;
   trig_EF_emcl_secondR = 0;
   trig_EF_emcl_centerlambda = 0;
   trig_EF_emcl_deltaTheta = 0;
   trig_EF_emcl_deltaPhi = 0;
   trig_EF_emcl_time = 0;
   trig_EF_emcl_E_PreSamplerB = 0;
   trig_EF_emcl_E_EMB1 = 0;
   trig_EF_emcl_E_EMB2 = 0;
   trig_EF_emcl_E_EMB3 = 0;
   trig_EF_emcl_E_PreSamplerE = 0;
   trig_EF_emcl_E_EME1 = 0;
   trig_EF_emcl_E_EME2 = 0;
   trig_EF_emcl_E_EME3 = 0;
   trig_EF_emcl_E_HEC0 = 0;
   trig_EF_emcl_E_HEC1 = 0;
   trig_EF_emcl_E_HEC2 = 0;
   trig_EF_emcl_E_HEC3 = 0;
   trig_EF_emcl_E_TileBar0 = 0;
   trig_EF_emcl_E_TileBar1 = 0;
   trig_EF_emcl_E_TileBar2 = 0;
   trig_EF_emcl_E_TileGap1 = 0;
   trig_EF_emcl_E_TileGap2 = 0;
   trig_EF_emcl_E_TileGap3 = 0;
   trig_EF_emcl_E_TileExt0 = 0;
   trig_EF_emcl_E_TileExt1 = 0;
   trig_EF_emcl_E_TileExt2 = 0;
   trig_EF_emcl_E_FCAL0 = 0;
   trig_EF_emcl_E_FCAL1 = 0;
   trig_EF_emcl_E_FCAL2 = 0;
   trig_EF_emcl_eta_PreSamplerB = 0;
   trig_EF_emcl_phi_PreSamplerB = 0;
   trig_EF_emcl_eta_EMB1 = 0;
   trig_EF_emcl_phi_EMB1 = 0;
   trig_EF_emcl_eta_EMB2 = 0;
   trig_EF_emcl_phi_EMB2 = 0;
   trig_EF_emcl_eta_EMB3 = 0;
   trig_EF_emcl_phi_EMB3 = 0;
   trig_EF_emcl_eta_PreSamplerE = 0;
   trig_EF_emcl_phi_PreSamplerE = 0;
   trig_EF_emcl_eta_EME1 = 0;
   trig_EF_emcl_phi_EME1 = 0;
   trig_EF_emcl_eta_EME2 = 0;
   trig_EF_emcl_phi_EME2 = 0;
   trig_EF_emcl_eta_EME3 = 0;
   trig_EF_emcl_phi_EME3 = 0;
   trig_EF_emcl_eta_HEC0 = 0;
   trig_EF_emcl_phi_HEC0 = 0;
   trig_EF_emcl_eta_HEC1 = 0;
   trig_EF_emcl_phi_HEC1 = 0;
   trig_EF_emcl_eta_HEC2 = 0;
   trig_EF_emcl_phi_HEC2 = 0;
   trig_EF_emcl_eta_HEC3 = 0;
   trig_EF_emcl_phi_HEC3 = 0;
   trig_EF_emcl_eta_TileBar0 = 0;
   trig_EF_emcl_phi_TileBar0 = 0;
   trig_EF_emcl_eta_TileBar1 = 0;
   trig_EF_emcl_phi_TileBar1 = 0;
   trig_EF_emcl_eta_TileBar2 = 0;
   trig_EF_emcl_phi_TileBar2 = 0;
   trig_EF_emcl_eta_TileGap1 = 0;
   trig_EF_emcl_phi_TileGap1 = 0;
   trig_EF_emcl_eta_TileGap2 = 0;
   trig_EF_emcl_phi_TileGap2 = 0;
   trig_EF_emcl_eta_TileGap3 = 0;
   trig_EF_emcl_phi_TileGap3 = 0;
   trig_EF_emcl_eta_TileExt0 = 0;
   trig_EF_emcl_phi_TileExt0 = 0;
   trig_EF_emcl_eta_TileExt1 = 0;
   trig_EF_emcl_phi_TileExt1 = 0;
   trig_EF_emcl_eta_TileExt2 = 0;
   trig_EF_emcl_phi_TileExt2 = 0;
   trig_EF_emcl_eta_FCAL0 = 0;
   trig_EF_emcl_phi_FCAL0 = 0;
   trig_EF_emcl_eta_FCAL1 = 0;
   trig_EF_emcl_phi_FCAL1 = 0;
   trig_EF_emcl_eta_FCAL2 = 0;
   trig_EF_emcl_phi_FCAL2 = 0;
   trig_EF_emcl_cell_n = 0;
   trig_EF_emcl_cell_E = 0;
   trig_EF_emcl_cell_pt = 0;
   trig_EF_emcl_cell_eta = 0;
   trig_EF_emcl_cell_phi = 0;
   trig_EF_emcl_cell_QCells = 0;
   trig_EF_emcl_cell_GainCells = 0;
   trig_EF_emcl_cell_DetCells = 0;
   trig_EF_emcl_cell_TimeCells = 0;
   trig_EF_emcl_cell_xCells = 0;
   trig_EF_emcl_cell_yCells = 0;
   trig_EF_emcl_cell_zCells = 0;
   trig_EF_emcl_cell_BadCells = 0;
   trig_EF_emcl_cell_IdCells = 0;
   trig_EF_emcl_slw_E = 0;
   trig_EF_emcl_slw_pt = 0;
   trig_EF_emcl_slw_m = 0;
   trig_EF_emcl_slw_eta = 0;
   trig_EF_emcl_slw_phi = 0;
   trig_EF_emcl_slw_E_em = 0;
   trig_EF_emcl_slw_E_had = 0;
   trig_EF_emcl_slw_firstEdens = 0;
   trig_EF_emcl_slw_cellmaxfrac = 0;
   trig_EF_emcl_slw_longitudinal = 0;
   trig_EF_emcl_slw_secondlambda = 0;
   trig_EF_emcl_slw_lateral = 0;
   trig_EF_emcl_slw_secondR = 0;
   trig_EF_emcl_slw_centerlambda = 0;
   trig_EF_emcl_slw_deltaTheta = 0;
   trig_EF_emcl_slw_deltaPhi = 0;
   trig_EF_emcl_slw_time = 0;
   trig_EF_emcl_slw_E_PreSamplerB = 0;
   trig_EF_emcl_slw_E_EMB1 = 0;
   trig_EF_emcl_slw_E_EMB2 = 0;
   trig_EF_emcl_slw_E_EMB3 = 0;
   trig_EF_emcl_slw_E_PreSamplerE = 0;
   trig_EF_emcl_slw_E_EME1 = 0;
   trig_EF_emcl_slw_E_EME2 = 0;
   trig_EF_emcl_slw_E_EME3 = 0;
   trig_EF_emcl_slw_E_HEC0 = 0;
   trig_EF_emcl_slw_E_HEC1 = 0;
   trig_EF_emcl_slw_E_HEC2 = 0;
   trig_EF_emcl_slw_E_HEC3 = 0;
   trig_EF_emcl_slw_E_TileBar0 = 0;
   trig_EF_emcl_slw_E_TileBar1 = 0;
   trig_EF_emcl_slw_E_TileBar2 = 0;
   trig_EF_emcl_slw_E_TileGap1 = 0;
   trig_EF_emcl_slw_E_TileGap2 = 0;
   trig_EF_emcl_slw_E_TileGap3 = 0;
   trig_EF_emcl_slw_E_TileExt0 = 0;
   trig_EF_emcl_slw_E_TileExt1 = 0;
   trig_EF_emcl_slw_E_TileExt2 = 0;
   trig_EF_emcl_slw_E_FCAL0 = 0;
   trig_EF_emcl_slw_E_FCAL1 = 0;
   trig_EF_emcl_slw_E_FCAL2 = 0;
   trig_EF_emcl_slw_eta_PreSamplerB = 0;
   trig_EF_emcl_slw_phi_PreSamplerB = 0;
   trig_EF_emcl_slw_eta_EMB1 = 0;
   trig_EF_emcl_slw_phi_EMB1 = 0;
   trig_EF_emcl_slw_eta_EMB2 = 0;
   trig_EF_emcl_slw_phi_EMB2 = 0;
   trig_EF_emcl_slw_eta_EMB3 = 0;
   trig_EF_emcl_slw_phi_EMB3 = 0;
   trig_EF_emcl_slw_eta_PreSamplerE = 0;
   trig_EF_emcl_slw_phi_PreSamplerE = 0;
   trig_EF_emcl_slw_eta_EME1 = 0;
   trig_EF_emcl_slw_phi_EME1 = 0;
   trig_EF_emcl_slw_eta_EME2 = 0;
   trig_EF_emcl_slw_phi_EME2 = 0;
   trig_EF_emcl_slw_eta_EME3 = 0;
   trig_EF_emcl_slw_phi_EME3 = 0;
   trig_EF_emcl_slw_eta_HEC0 = 0;
   trig_EF_emcl_slw_phi_HEC0 = 0;
   trig_EF_emcl_slw_eta_HEC1 = 0;
   trig_EF_emcl_slw_phi_HEC1 = 0;
   trig_EF_emcl_slw_eta_HEC2 = 0;
   trig_EF_emcl_slw_phi_HEC2 = 0;
   trig_EF_emcl_slw_eta_HEC3 = 0;
   trig_EF_emcl_slw_phi_HEC3 = 0;
   trig_EF_emcl_slw_eta_TileBar0 = 0;
   trig_EF_emcl_slw_phi_TileBar0 = 0;
   trig_EF_emcl_slw_eta_TileBar1 = 0;
   trig_EF_emcl_slw_phi_TileBar1 = 0;
   trig_EF_emcl_slw_eta_TileBar2 = 0;
   trig_EF_emcl_slw_phi_TileBar2 = 0;
   trig_EF_emcl_slw_eta_TileGap1 = 0;
   trig_EF_emcl_slw_phi_TileGap1 = 0;
   trig_EF_emcl_slw_eta_TileGap2 = 0;
   trig_EF_emcl_slw_phi_TileGap2 = 0;
   trig_EF_emcl_slw_eta_TileGap3 = 0;
   trig_EF_emcl_slw_phi_TileGap3 = 0;
   trig_EF_emcl_slw_eta_TileExt0 = 0;
   trig_EF_emcl_slw_phi_TileExt0 = 0;
   trig_EF_emcl_slw_eta_TileExt1 = 0;
   trig_EF_emcl_slw_phi_TileExt1 = 0;
   trig_EF_emcl_slw_eta_TileExt2 = 0;
   trig_EF_emcl_slw_phi_TileExt2 = 0;
   trig_EF_emcl_slw_eta_FCAL0 = 0;
   trig_EF_emcl_slw_phi_FCAL0 = 0;
   trig_EF_emcl_slw_eta_FCAL1 = 0;
   trig_EF_emcl_slw_phi_FCAL1 = 0;
   trig_EF_emcl_slw_eta_FCAL2 = 0;
   trig_EF_emcl_slw_phi_FCAL2 = 0;
   trig_EF_emcl_slw_cell_n = 0;
   trig_EF_emcl_slw_cell_E = 0;
   trig_EF_emcl_slw_cell_pt = 0;
   trig_EF_emcl_slw_cell_eta = 0;
   trig_EF_emcl_slw_cell_phi = 0;
   trig_EF_emcl_slw_cell_QCells = 0;
   trig_EF_emcl_slw_cell_GainCells = 0;
   trig_EF_emcl_slw_cell_DetCells = 0;
   trig_EF_emcl_slw_cell_TimeCells = 0;
   trig_EF_emcl_slw_cell_xCells = 0;
   trig_EF_emcl_slw_cell_yCells = 0;
   trig_EF_emcl_slw_cell_zCells = 0;
   trig_EF_emcl_slw_cell_BadCells = 0;
   trig_EF_emcl_slw_cell_IdCells = 0;
   trig_EF_el_E = 0;
   trig_EF_el_Et = 0;
   trig_EF_el_pt = 0;
   trig_EF_el_m = 0;
   trig_EF_el_eta = 0;
   trig_EF_el_phi = 0;
   trig_EF_el_px = 0;
   trig_EF_el_py = 0;
   trig_EF_el_pz = 0;
   trig_EF_el_charge = 0;
   trig_EF_el_author = 0;
   trig_EF_el_isEM = 0;
   trig_EF_el_loose = 0;
   trig_EF_el_medium = 0;
   trig_EF_el_mediumIso = 0;
   trig_EF_el_tight = 0;
   trig_EF_el_tightIso = 0;
   trig_EF_el_Ethad = 0;
   trig_EF_el_Ethad1 = 0;
   trig_EF_el_f1 = 0;
   trig_EF_el_f1core = 0;
   trig_EF_el_Emins1 = 0;
   trig_EF_el_fside = 0;
   trig_EF_el_Emax2 = 0;
   trig_EF_el_ws3 = 0;
   trig_EF_el_wstot = 0;
   trig_EF_el_emaxs1 = 0;
   trig_EF_el_deltaEs = 0;
   trig_EF_el_E233 = 0;
   trig_EF_el_E237 = 0;
   trig_EF_el_E277 = 0;
   trig_EF_el_weta2 = 0;
   trig_EF_el_f3 = 0;
   trig_EF_el_f3core = 0;
   trig_EF_el_rphiallcalo = 0;
   trig_EF_el_Etcone45 = 0;
   trig_EF_el_Etcone20 = 0;
   trig_EF_el_Etcone30 = 0;
   trig_EF_el_Etcone40 = 0;
   trig_EF_el_pos7 = 0;
   trig_EF_el_etacorrmag = 0;
   trig_EF_el_deltaeta1 = 0;
   trig_EF_el_deltaeta2 = 0;
   trig_EF_el_deltaphi2 = 0;
   trig_EF_el_reta = 0;
   trig_EF_el_rphi = 0;
   trig_EF_el_EtringnoisedR03sig2 = 0;
   trig_EF_el_EtringnoisedR03sig3 = 0;
   trig_EF_el_EtringnoisedR03sig4 = 0;
   trig_EF_el_isolationlikelihoodjets = 0;
   trig_EF_el_isolationlikelihoodhqelectrons = 0;
   trig_EF_el_electronweight = 0;
   trig_EF_el_electronbgweight = 0;
   trig_EF_el_softeweight = 0;
   trig_EF_el_softebgweight = 0;
   trig_EF_el_neuralnet = 0;
   trig_EF_el_Hmatrix = 0;
   trig_EF_el_Hmatrix5 = 0;
   trig_EF_el_adaboost = 0;
   trig_EF_el_softeneuralnet = 0;
   trig_EF_el_zvertex = 0;
   trig_EF_el_errz = 0;
   trig_EF_el_etap = 0;
   trig_EF_el_depth = 0;
   trig_EF_el_refittedtrackcovd0 = 0;
   trig_EF_el_refittedtrackcovz0 = 0;
   trig_EF_el_refittedtrackcovphi = 0;
   trig_EF_el_refittedtrackcovtheta = 0;
   trig_EF_el_refittedtrackcovqoverp = 0;
   trig_EF_el_refittedtrackcovd0z0 = 0;
   trig_EF_el_refittedtrackcovz0phi = 0;
   trig_EF_el_refittedtrackcovz0theta = 0;
   trig_EF_el_refittedtrackcovz0qoverp = 0;
   trig_EF_el_refittedtrackcovd0phi = 0;
   trig_EF_el_refittedtrackcovd0theta = 0;
   trig_EF_el_refittedtrackcovd0qoverp = 0;
   trig_EF_el_refittedtrackcovphitheta = 0;
   trig_EF_el_refittedtrackcovphiqoverp = 0;
   trig_EF_el_refittedtrackcovthetaqoverp = 0;
   trig_EF_el_EF_2e12_tight = 0;
   trig_EF_el_EF_2e15_medium = 0;
   trig_EF_el_EF_2e20_loose = 0;
   trig_EF_el_EF_2e5_medium = 0;
   trig_EF_el_EF_2e5_medium1 = 0;
   trig_EF_el_EF_e105_loose1 = 0;
   trig_EF_el_EF_e10_NoCut = 0;
   trig_EF_el_EF_e10_loose = 0;
   trig_EF_el_EF_e10_loose_IdScan = 0;
   trig_EF_el_EF_e10_loose_SiTrk = 0;
   trig_EF_el_EF_e10_loose_TRT = 0;
   trig_EF_el_EF_e10_loose_mu6 = 0;
   trig_EF_el_EF_e10_medium = 0;
   trig_EF_el_EF_e10_medium_SiTrk = 0;
   trig_EF_el_EF_e10_medium_SiTrk_robust = 0;
   trig_EF_el_EF_e12_medium = 0;
   trig_EF_el_EF_e15_medium = 0;
   trig_EF_el_EF_e15i_medium = 0;
   trig_EF_el_EF_e20_loose = 0;
   trig_EF_el_EF_e20_loose_IdScan = 0;
   trig_EF_el_EF_e20_loose_NoIDTrkCut = 0;
   trig_EF_el_EF_e20_loose_SiTrk = 0;
   trig_EF_el_EF_e20_loose_SiTrk_robust = 0;
   trig_EF_el_EF_e20_loose_TRT = 0;
   trig_EF_el_EF_e20_loose_passEF = 0;
   trig_EF_el_EF_e20_loose_passL2 = 0;
   trig_EF_el_EF_e20_loose_xe20 = 0;
   trig_EF_el_EF_e20_loose_xe30 = 0;
   trig_EF_el_EF_e20_medium = 0;
   trig_EF_el_EF_e20i_loose = 0;
   trig_EF_el_EF_e22i_tight = 0;
   trig_EF_el_EF_e25_loose = 0;
   trig_EF_el_EF_e25_medium = 0;
   trig_EF_el_EF_e25i_loose = 0;
   trig_EF_el_EF_e25i_medium = 0;
   trig_EF_el_EF_e55_loose1 = 0;
   trig_EF_el_EF_e5_medium = 0;
   trig_EF_el_EF_e5_medium1 = 0;
   trig_EF_el_EF_e5_medium_L2SW = 0;
   trig_EF_el_EF_e5_medium_MV = 0;
   trig_EF_el_EF_e5_medium_Ringer = 0;
   trig_EF_el_EF_e5_medium_e10_loose = 0;
   trig_EF_el_EF_em105_passHLT = 0;
   trig_EF_el_EF_em15_passHLT = 0;
   trig_EF_el_EF_em15i_passHLT = 0;
   trig_EF_el_EF_em20_passHLT = 0;
   trig_EF_el_EF_em20i_passHLT = 0;
   trig_EF_ph_E = 0;
   trig_EF_ph_Et = 0;
   trig_EF_ph_pt = 0;
   trig_EF_ph_m = 0;
   trig_EF_ph_eta = 0;
   trig_EF_ph_phi = 0;
   trig_EF_ph_px = 0;
   trig_EF_ph_py = 0;
   trig_EF_ph_pz = 0;
   trig_EF_ph_EF_2g10_loose = 0;
   trig_EF_ph_EF_2g10_loose_mu6 = 0;
   trig_EF_ph_EF_2g17i_tight = 0;
   trig_EF_ph_EF_2g20_loose = 0;
   trig_EF_ph_EF_g105 = 0;
   trig_EF_ph_EF_g10_loose = 0;
   trig_EF_ph_EF_g10_loose_larcalib = 0;
   trig_EF_ph_EF_g10_nocut = 0;
   trig_EF_ph_EF_g150 = 0;
   trig_EF_ph_EF_g15_loose = 0;
   trig_EF_ph_EF_g20_loose = 0;
   trig_EF_ph_EF_g20_loose_cnv = 0;
   trig_EF_ph_EF_g20_loose_larcalib = 0;
   trig_EF_ph_EF_g20_loose_xe20 = 0;
   trig_EF_ph_EF_g20i_loose = 0;
   trig_EF_ph_EF_g20i_loose_larcalib = 0;
   trig_EF_ph_EF_g25_loose = 0;
   trig_EF_ph_EF_g25_loose_xe30 = 0;
   trig_EF_ph_EF_g25i_loose = 0;
   trig_EF_ph_EF_g55_tight = 0;
   trig_RoI_L2_e_type = 0;
   trig_RoI_L2_e_lastStep = 0;
   trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgamma = 0;
   trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgammaStatus = 0;
   trig_RoI_L2_e_TrigEMCluster = 0;
   trig_RoI_L2_e_TrigEMClusterStatus = 0;
   trig_RoI_L2_e_EmTau_ROI = 0;
   trig_RoI_L2_e_EmTau_ROIStatus = 0;
   trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma = 0;
   trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus = 0;
   trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma = 0;
   trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus = 0;
   trig_RoI_L2_e_TrigElectronContainer = 0;
   trig_RoI_L2_e_TrigElectronContainerStatus = 0;
   trig_RoI_L2_e_TrigPhotonContainer = 0;
   trig_RoI_L2_e_TrigPhotonContainerStatus = 0;
   trig_RoI_EF_e_type = 0;
   trig_RoI_EF_e_lastStep = 0;
   trig_RoI_EF_e_EmTau_ROI = 0;
   trig_RoI_EF_e_EmTau_ROIStatus = 0;
   trig_RoI_EF_e_Rec__TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFID = 0;
   trig_RoI_EF_e_Rec__TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFIDStatus = 0;
   trig_RoI_EF_e_egammaContainer_egamma_Electrons = 0;
   trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus = 0;
   trig_RoI_EF_e_egammaContainer_egamma_Photons = 0;
   trig_RoI_EF_e_egammaContainer_egamma_PhotonsStatus = 0;
   trk_pt = 0;
   trk_eta = 0;
   trk_d0_wrtPV = 0;
   trk_z0_wrtPV = 0;
   trk_phi_wrtPV = 0;
   trk_cov_d0_wrtPV = 0;
   trk_cov_z0_wrtPV = 0;
   trk_cov_phi_wrtPV = 0;
   trk_cov_theta_wrtPV = 0;
   trk_cov_qoverp_wrtPV = 0;
   trk_chi2 = 0;
   trk_ndof = 0;
   trk_nBLHits = 0;
   trk_nPixHits = 0;
   trk_nSCTHits = 0;
   trk_nTRTHits = 0;
   trk_nPixHoles = 0;
   trk_nSCTHoles = 0;
   trk_nMDTHits = 0;
   trk_nCSCEtaHits = 0;
   trk_nCSCPhiHits = 0;
   trk_nRPCEtaHits = 0;
   trk_nRPCPhiHits = 0;
   trk_nTGCEtaHits = 0;
   trk_nTGCPhiHits = 0;
   trk_nHits = 0;
   trk_nHoles = 0;
   trk_hitPattern = 0;
   trk_TRTHighTHitsRatio = 0;
   trk_pixeldEdx = 0;
   trk_fitter = 0;
   trk_patternReco1 = 0;
   trk_patternReco2 = 0;
   trk_seedFinder = 0;
   trk_blayerPrediction_expectHit = 0;
   trk_blayerPrediction_x = 0;
   trk_blayerPrediction_y = 0;
   trk_blayerPrediction_z = 0;
   trk_blayerPrediction_locX = 0;
   trk_blayerPrediction_locY = 0;
   trk_blayerPrediction_err_locX = 0;
   trk_blayerPrediction_err_locY = 0;
   trk_blayerPrediction_etaDistToEdge = 0;
   trk_blayerPrediction_phiDistToEdge = 0;
   trk_blayerPrediction_detElementId = 0;
   trk_blayerPrediction_row = 0;
   trk_blayerPrediction_col = 0;
   trk_blayerPrediction_type = 0;
   trk_mc_probability = 0;
   trk_mc_barcode = 0;
   trk_mc_index = 0;
   vxp_x = 0;
   vxp_y = 0;
   vxp_z = 0;
   vxp_err_x = 0;
   vxp_err_y = 0;
   vxp_err_z = 0;
   vxp_chi2 = 0;
   vxp_ndof = 0;
   vxp_px = 0;
   vxp_py = 0;
   vxp_pz = 0;
   vxp_E = 0;
   vxp_m = 0;
   vxp_nTracks = 0;
   vxp_sumPt = 0;
   vxp_type = 0;
   vxp_sumWeightInliers = 0;
   vxp_sumWeightOutliers = 0;
   vxp_sumWeightPileup = 0;
   vxp_sumWeightFakes = 0;
   vxp_nInliers = 0;
   vxp_nOutliers = 0;
   vxp_nPileUp = 0;
   vxp_nFakes = 0;
   vxp_purity = 0;
   vxp_trk_n = 0;
   vxp_trk_chi2 = 0;
   vxp_trk_d0 = 0;
   vxp_trk_z0 = 0;
   vxp_trk_unbiased_d0 = 0;
   vxp_trk_unbiased_z0 = 0;
   vxp_trk_err_unbiased_d0 = 0;
   vxp_trk_err_unbiased_z0 = 0;
   vxp_trk_phi = 0;
   vxp_trk_theta = 0;
   vxp_trk_weight = 0;
   vxp_trk_index = 0;
   mcVx_x = 0;
   mcVx_y = 0;
   mcVx_z = 0;
   mcTrk_gen_pt = 0;
   mcTrk_gen_eta = 0;
   mcTrk_gen_phi = 0;
   mcTrk_gen_energy = 0;
   mcTrk_perigee_ok = 0;
   mcTrk_perigee_d0 = 0;
   mcTrk_perigee_z0 = 0;
   mcTrk_perigee_phi = 0;
   mcTrk_perigee_theta = 0;
   mcTrk_perigee_qoverp = 0;
   mcTrk_pdg = 0;
   mcTrk_charge = 0;
   mcTrk_barcode = 0;
   mcTrk_status = 0;
   mcTrk_mother_pdg = 0;
   mcTrk_mother_barcode = 0;
   mcTrk_child_n = 0;
   mcTrk_child_barcode = 0;
   mcTrk_begVtx_x = 0;
   mcTrk_begVtx_y = 0;
   mcTrk_begVtx_z = 0;
   mcTrk_begVtx_barcode = 0;
   mcTrk_endVtx_x = 0;
   mcTrk_endVtx_y = 0;
   mcTrk_endVtx_z = 0;
   mcTrk_endVtx_barcode = 0;
   mc_pt = 0;
   mc_m = 0;
   mc_eta = 0;
   mc_phi = 0;
   mc_status = 0;
   mc_barcode = 0;
   mc_parents = 0;
   mc_children = 0;
   mc_pdgId = 0;
   mc_charge = 0;
   mc_vx_x = 0;
   mc_vx_y = 0;
   mc_vx_z = 0;
   mc_child_index = 0;
   mc_parent_index = 0;
   mcEvt_signal_process_id = 0;
   mcEvt_event_number = 0;
   mcEvt_event_scale = 0;
   mcEvt_alphaQCD = 0;
   mcEvt_alphaQED = 0;
   mcEvt_pdf_id1 = 0;
   mcEvt_pdf_id2 = 0;
   mcEvt_pdf_x1 = 0;
   mcEvt_pdf_x2 = 0;
   mcEvt_pdf_scale = 0;
   mcEvt_pdf1 = 0;
   mcEvt_pdf2 = 0;
   mcEvt_weight = 0;
   AntiKt4Truth_E = 0;
   AntiKt4Truth_pt = 0;
   AntiKt4Truth_m = 0;
   AntiKt4Truth_eta = 0;
   AntiKt4Truth_phi = 0;
   AntiKt4Truth_EtaPhys = 0;
   AntiKt4Truth_PhiPhys = 0;
   AntiKt4Truth_MPhys = 0;
   AntiKt4Truth_WIDTH = 0;
   AntiKt4Truth_n90 = 0;
   AntiKt4Truth_Timing = 0;
   AntiKt4Truth_LArQuality = 0;
   AntiKt4Truth_nTrk = 0;
   AntiKt4Truth_sumPtTrk = 0;
   AntiKt4Truth_OriginIndex = 0;
   AntiKt4Truth_HECQuality = 0;
   AntiKt4Truth_NegativeE = 0;
   AntiKt4Truth_BCH_CORR_CELL = 0;
   AntiKt4Truth_BCH_CORR_DOTX = 0;
   AntiKt4Truth_BCH_CORR_JET = 0;
   AntiKt4Truth_BCH_CORR_JET_FORCELL = 0;
   AntiKt4Truth_ENG_BAD_CELLS = 0;
   AntiKt4Truth_N_BAD_CELLS = 0;
   AntiKt4Truth_N_BAD_CELLS_CORR = 0;
   AntiKt4Truth_BAD_CELLS_CORR_E = 0;
   AntiKt4Truth_SamplingMax = 0;
   AntiKt4Truth_fracSamplingMax = 0;
   AntiKt4Truth_hecf = 0;
   AntiKt4Truth_tgap3f = 0;
   AntiKt4Truth_isGood = 0;
   AntiKt4Truth_emfrac = 0;
   AntiKt4Truth_GCWJES = 0;
   AntiKt4Truth_EMJES = 0;
   AntiKt4Truth_CB = 0;
   AntiKt4Truth_emscale_E = 0;
   AntiKt4Truth_emscale_pt = 0;
   AntiKt4Truth_emscale_m = 0;
   AntiKt4Truth_emscale_eta = 0;
   AntiKt4Truth_emscale_phi = 0;
   AntiKt4Truth_jvtxf = 0;
   AntiKt4Truth_jvtx_x = 0;
   AntiKt4Truth_jvtx_y = 0;
   AntiKt4Truth_jvtx_z = 0;
   AntiKt4Truth_e_PreSamplerB = 0;
   AntiKt4Truth_e_EMB1 = 0;
   AntiKt4Truth_e_EMB2 = 0;
   AntiKt4Truth_e_EMB3 = 0;
   AntiKt4Truth_e_PreSamplerE = 0;
   AntiKt4Truth_e_EME1 = 0;
   AntiKt4Truth_e_EME2 = 0;
   AntiKt4Truth_e_EME3 = 0;
   AntiKt4Truth_e_HEC0 = 0;
   AntiKt4Truth_e_HEC1 = 0;
   AntiKt4Truth_e_HEC2 = 0;
   AntiKt4Truth_e_HEC3 = 0;
   AntiKt4Truth_e_TileBar0 = 0;
   AntiKt4Truth_e_TileBar1 = 0;
   AntiKt4Truth_e_TileBar2 = 0;
   AntiKt4Truth_e_TileGap1 = 0;
   AntiKt4Truth_e_TileGap2 = 0;
   AntiKt4Truth_e_TileGap3 = 0;
   AntiKt4Truth_e_TileExt0 = 0;
   AntiKt4Truth_e_TileExt1 = 0;
   AntiKt4Truth_e_TileExt2 = 0;
   AntiKt4Truth_e_FCAL0 = 0;
   AntiKt4Truth_e_FCAL1 = 0;
   AntiKt4Truth_e_FCAL2 = 0;
   AntiKt4Truth_shapeBins = 0;
   AntiKt4Truth_Nconst = 0;
   AntiKt4Truth_ptconst_emscale = 0;
   AntiKt4Truth_econst_emscale = 0;
   AntiKt4Truth_etaconst_emscale = 0;
   AntiKt4Truth_phiconst_emscale = 0;
   AntiKt4Truth_weightconst_emscale = 0;
   AntiKt4Truth_constscale_E = 0;
   AntiKt4Truth_constscale_pt = 0;
   AntiKt4Truth_constscale_m = 0;
   AntiKt4Truth_constscale_eta = 0;
   AntiKt4Truth_constscale_phi = 0;
   AntiKt4Truth_flavor_component_ip2d_pu = 0;
   AntiKt4Truth_flavor_component_ip2d_pb = 0;
   AntiKt4Truth_flavor_component_ip2d_ntrk = 0;
   AntiKt4Truth_flavor_component_ip3d_pu = 0;
   AntiKt4Truth_flavor_component_ip3d_pb = 0;
   AntiKt4Truth_flavor_component_ip3d_ntrk = 0;
   AntiKt4Truth_flavor_component_sv1_pu = 0;
   AntiKt4Truth_flavor_component_sv1_pb = 0;
   AntiKt4Truth_flavor_component_sv2_pu = 0;
   AntiKt4Truth_flavor_component_sv2_pb = 0;
   AntiKt4Truth_flavor_component_svp_mass = 0;
   AntiKt4Truth_flavor_component_svp_n2t = 0;
   AntiKt4Truth_flavor_component_svp_svok = 0;
   AntiKt4Truth_flavor_component_svp_ntrk = 0;
   AntiKt4Truth_flavor_component_svp_ntrkv = 0;
   AntiKt4Truth_flavor_component_svp_ntrkj = 0;
   AntiKt4Truth_flavor_component_svp_efrc = 0;
   AntiKt4Truth_flavor_component_sv0p_mass = 0;
   AntiKt4Truth_flavor_component_sv0p_n2t = 0;
   AntiKt4Truth_flavor_component_sv0p_svok = 0;
   AntiKt4Truth_flavor_component_sv0p_ntrk = 0;
   AntiKt4Truth_flavor_component_sv0p_ntrkv = 0;
   AntiKt4Truth_flavor_component_sv0p_ntrkj = 0;
   AntiKt4Truth_flavor_component_sv0p_efrc = 0;
   AntiKt4Truth_flavor_component_jfit_pu = 0;
   AntiKt4Truth_flavor_component_jfit_pb = 0;
   AntiKt4Truth_flavor_component_jfit_nvtxnt = 0;
   AntiKt4Truth_flavor_component_jfit_nvtx1t = 0;
   AntiKt4Truth_flavor_component_jfit_ntrk = 0;
   AntiKt4Truth_flavor_component_jfit_efrc = 0;
   AntiKt4Truth_flavor_component_jfit_mass = 0;
   AntiKt4Truth_flavor_component_jfit_sig3d = 0;
   AntiKt4Truth_flavor_weight = 0;
   AntiKt4Truth_flavor_weight_TrackCounting2D = 0;
   AntiKt4Truth_flavor_weight_JetProb = 0;
   AntiKt4Truth_flavor_weight_IP1D = 0;
   AntiKt4Truth_flavor_weight_IP2D = 0;
   AntiKt4Truth_flavor_weight_IP3D = 0;
   AntiKt4Truth_flavor_weight_SV0 = 0;
   AntiKt4Truth_flavor_weight_SV1 = 0;
   AntiKt4Truth_flavor_weight_SV2 = 0;
   AntiKt4Truth_flavor_weight_JetFitterTag = 0;
   AntiKt4Truth_flavor_weight_JetFitterCOMB = 0;
   AntiKt4Truth_flavor_weight_JetFitterTagNN = 0;
   AntiKt4Truth_flavor_weight_JetFitterCOMBNN = 0;
   AntiKt4Truth_flavor_weight_SoftMuonTag = 0;
   AntiKt4Truth_flavor_weight_SoftElectronTag = 0;
   AntiKt4Truth_RoIword = 0;
   AntiKt4Truth_flavor_truth_dRminToB = 0;
   AntiKt4Truth_flavor_truth_dRminToC = 0;
   AntiKt4Truth_flavor_truth_dRminToT = 0;
   AntiKt4Truth_flavor_truth_BHadronpdg = 0;
   AntiKt4Truth_flavor_truth_trueflav = 0;
   AntiKt4Truth_el_dr = 0;
   AntiKt4Truth_el_matched = 0;
   AntiKt4Truth_mu_dr = 0;
   AntiKt4Truth_mu_matched = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("ei_RunNumber", &ei_RunNumber, &b_ei_RunNumber);
   fChain->SetBranchAddress("ei_EventNumber", &ei_EventNumber, &b_ei_EventNumber);
   fChain->SetBranchAddress("ei_timestamp", &ei_timestamp, &b_ei_timestamp);
   fChain->SetBranchAddress("ei_timestamp_ns", &ei_timestamp_ns, &b_ei_timestamp_ns);
   fChain->SetBranchAddress("ei_lbn", &ei_lbn, &b_ei_lbn);
   fChain->SetBranchAddress("ei_bcid", &ei_bcid, &b_ei_bcid);
   fChain->SetBranchAddress("ei_detmask0", &ei_detmask0, &b_ei_detmask0);
   fChain->SetBranchAddress("ei_detmask1", &ei_detmask1, &b_ei_detmask1);
   fChain->SetBranchAddress("ei_pixelFlags", &ei_pixelFlags, &b_ei_pixelFlags);
   fChain->SetBranchAddress("ei_sctFlags", &ei_sctFlags, &b_ei_sctFlags);
   fChain->SetBranchAddress("ei_trtFlags", &ei_trtFlags, &b_ei_trtFlags);
   fChain->SetBranchAddress("ei_larFlags", &ei_larFlags, &b_ei_larFlags);
   fChain->SetBranchAddress("ei_tileFlags", &ei_tileFlags, &b_ei_tileFlags);
   fChain->SetBranchAddress("ei_muonFlags", &ei_muonFlags, &b_ei_muonFlags);
   fChain->SetBranchAddress("ei_fwdFlags", &ei_fwdFlags, &b_ei_fwdFlags);
   fChain->SetBranchAddress("ei_coreFlags", &ei_coreFlags, &b_ei_coreFlags);
   fChain->SetBranchAddress("ei_pixelError", &ei_pixelError, &b_ei_pixelError);
   fChain->SetBranchAddress("ei_sctError", &ei_sctError, &b_ei_sctError);
   fChain->SetBranchAddress("ei_trtError", &ei_trtError, &b_ei_trtError);
   fChain->SetBranchAddress("ei_larError", &ei_larError, &b_ei_larError);
   fChain->SetBranchAddress("ei_tileError", &ei_tileError, &b_ei_tileError);
   fChain->SetBranchAddress("ei_muonError", &ei_muonError, &b_ei_muonError);
   fChain->SetBranchAddress("ei_fwdError", &ei_fwdError, &b_ei_fwdError);
   fChain->SetBranchAddress("ei_coreError", &ei_coreError, &b_ei_coreError);
   fChain->SetBranchAddress("AntiKt4H1Topo_n", &AntiKt4H1Topo_n, &b_AntiKt4H1Topo_n);
   fChain->SetBranchAddress("AntiKt4H1Topo_E", &AntiKt4H1Topo_E, &b_AntiKt4H1Topo_E);
   fChain->SetBranchAddress("AntiKt4H1Topo_pt", &AntiKt4H1Topo_pt, &b_AntiKt4H1Topo_pt);
   fChain->SetBranchAddress("AntiKt4H1Topo_m", &AntiKt4H1Topo_m, &b_AntiKt4H1Topo_m);
   fChain->SetBranchAddress("AntiKt4H1Topo_eta", &AntiKt4H1Topo_eta, &b_AntiKt4H1Topo_eta);
   fChain->SetBranchAddress("AntiKt4H1Topo_phi", &AntiKt4H1Topo_phi, &b_AntiKt4H1Topo_phi);
   fChain->SetBranchAddress("AntiKt4H1Topo_EtaPhys", &AntiKt4H1Topo_EtaPhys, &b_AntiKt4H1Topo_EtaPhys);
   fChain->SetBranchAddress("AntiKt4H1Topo_PhiPhys", &AntiKt4H1Topo_PhiPhys, &b_AntiKt4H1Topo_PhiPhys);
   fChain->SetBranchAddress("AntiKt4H1Topo_MPhys", &AntiKt4H1Topo_MPhys, &b_AntiKt4H1Topo_MPhys);
   fChain->SetBranchAddress("AntiKt4H1Topo_WIDTH", &AntiKt4H1Topo_WIDTH, &b_AntiKt4H1Topo_WIDTH);
   fChain->SetBranchAddress("AntiKt4H1Topo_n90", &AntiKt4H1Topo_n90, &b_AntiKt4H1Topo_n90);
   fChain->SetBranchAddress("AntiKt4H1Topo_Timing", &AntiKt4H1Topo_Timing, &b_AntiKt4H1Topo_Timing);
   fChain->SetBranchAddress("AntiKt4H1Topo_LArQuality", &AntiKt4H1Topo_LArQuality, &b_AntiKt4H1Topo_LArQuality);
   fChain->SetBranchAddress("AntiKt4H1Topo_nTrk", &AntiKt4H1Topo_nTrk, &b_AntiKt4H1Topo_nTrk);
   fChain->SetBranchAddress("AntiKt4H1Topo_sumPtTrk", &AntiKt4H1Topo_sumPtTrk, &b_AntiKt4H1Topo_sumPtTrk);
   fChain->SetBranchAddress("AntiKt4H1Topo_OriginIndex", &AntiKt4H1Topo_OriginIndex, &b_AntiKt4H1Topo_OriginIndex);
   fChain->SetBranchAddress("AntiKt4H1Topo_HECQuality", &AntiKt4H1Topo_HECQuality, &b_AntiKt4H1Topo_HECQuality);
   fChain->SetBranchAddress("AntiKt4H1Topo_NegativeE", &AntiKt4H1Topo_NegativeE, &b_AntiKt4H1Topo_NegativeE);
   fChain->SetBranchAddress("AntiKt4H1Topo_BCH_CORR_CELL", &AntiKt4H1Topo_BCH_CORR_CELL, &b_AntiKt4H1Topo_BCH_CORR_CELL);
   fChain->SetBranchAddress("AntiKt4H1Topo_BCH_CORR_DOTX", &AntiKt4H1Topo_BCH_CORR_DOTX, &b_AntiKt4H1Topo_BCH_CORR_DOTX);
   fChain->SetBranchAddress("AntiKt4H1Topo_BCH_CORR_JET", &AntiKt4H1Topo_BCH_CORR_JET, &b_AntiKt4H1Topo_BCH_CORR_JET);
   fChain->SetBranchAddress("AntiKt4H1Topo_BCH_CORR_JET_FORCELL", &AntiKt4H1Topo_BCH_CORR_JET_FORCELL, &b_AntiKt4H1Topo_BCH_CORR_JET_FORCELL);
   fChain->SetBranchAddress("AntiKt4H1Topo_ENG_BAD_CELLS", &AntiKt4H1Topo_ENG_BAD_CELLS, &b_AntiKt4H1Topo_ENG_BAD_CELLS);
   fChain->SetBranchAddress("AntiKt4H1Topo_N_BAD_CELLS", &AntiKt4H1Topo_N_BAD_CELLS, &b_AntiKt4H1Topo_N_BAD_CELLS);
   fChain->SetBranchAddress("AntiKt4H1Topo_N_BAD_CELLS_CORR", &AntiKt4H1Topo_N_BAD_CELLS_CORR, &b_AntiKt4H1Topo_N_BAD_CELLS_CORR);
   fChain->SetBranchAddress("AntiKt4H1Topo_BAD_CELLS_CORR_E", &AntiKt4H1Topo_BAD_CELLS_CORR_E, &b_AntiKt4H1Topo_BAD_CELLS_CORR_E);
   fChain->SetBranchAddress("AntiKt4H1Topo_SamplingMax", &AntiKt4H1Topo_SamplingMax, &b_AntiKt4H1Topo_SamplingMax);
   fChain->SetBranchAddress("AntiKt4H1Topo_fracSamplingMax", &AntiKt4H1Topo_fracSamplingMax, &b_AntiKt4H1Topo_fracSamplingMax);
   fChain->SetBranchAddress("AntiKt4H1Topo_hecf", &AntiKt4H1Topo_hecf, &b_AntiKt4H1Topo_hecf);
   fChain->SetBranchAddress("AntiKt4H1Topo_tgap3f", &AntiKt4H1Topo_tgap3f, &b_AntiKt4H1Topo_tgap3f);
   fChain->SetBranchAddress("AntiKt4H1Topo_isGood", &AntiKt4H1Topo_isGood, &b_AntiKt4H1Topo_isGood);
   fChain->SetBranchAddress("AntiKt4H1Topo_emfrac", &AntiKt4H1Topo_emfrac, &b_AntiKt4H1Topo_emfrac);
   fChain->SetBranchAddress("AntiKt4H1Topo_GCWJES", &AntiKt4H1Topo_GCWJES, &b_AntiKt4H1Topo_GCWJES);
   fChain->SetBranchAddress("AntiKt4H1Topo_EMJES", &AntiKt4H1Topo_EMJES, &b_AntiKt4H1Topo_EMJES);
   fChain->SetBranchAddress("AntiKt4H1Topo_CB", &AntiKt4H1Topo_CB, &b_AntiKt4H1Topo_CB);
   fChain->SetBranchAddress("AntiKt4H1Topo_emscale_E", &AntiKt4H1Topo_emscale_E, &b_AntiKt4H1Topo_emscale_E);
   fChain->SetBranchAddress("AntiKt4H1Topo_emscale_pt", &AntiKt4H1Topo_emscale_pt, &b_AntiKt4H1Topo_emscale_pt);
   fChain->SetBranchAddress("AntiKt4H1Topo_emscale_m", &AntiKt4H1Topo_emscale_m, &b_AntiKt4H1Topo_emscale_m);
   fChain->SetBranchAddress("AntiKt4H1Topo_emscale_eta", &AntiKt4H1Topo_emscale_eta, &b_AntiKt4H1Topo_emscale_eta);
   fChain->SetBranchAddress("AntiKt4H1Topo_emscale_phi", &AntiKt4H1Topo_emscale_phi, &b_AntiKt4H1Topo_emscale_phi);
   fChain->SetBranchAddress("AntiKt4H1Topo_jvtxf", &AntiKt4H1Topo_jvtxf, &b_AntiKt4H1Topo_jvtxf);
   fChain->SetBranchAddress("AntiKt4H1Topo_jvtx_x", &AntiKt4H1Topo_jvtx_x, &b_AntiKt4H1Topo_jvtx_x);
   fChain->SetBranchAddress("AntiKt4H1Topo_jvtx_y", &AntiKt4H1Topo_jvtx_y, &b_AntiKt4H1Topo_jvtx_y);
   fChain->SetBranchAddress("AntiKt4H1Topo_jvtx_z", &AntiKt4H1Topo_jvtx_z, &b_AntiKt4H1Topo_jvtx_z);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_PreSamplerB", &AntiKt4H1Topo_e_PreSamplerB, &b_AntiKt4H1Topo_e_PreSamplerB);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_EMB1", &AntiKt4H1Topo_e_EMB1, &b_AntiKt4H1Topo_e_EMB1);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_EMB2", &AntiKt4H1Topo_e_EMB2, &b_AntiKt4H1Topo_e_EMB2);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_EMB3", &AntiKt4H1Topo_e_EMB3, &b_AntiKt4H1Topo_e_EMB3);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_PreSamplerE", &AntiKt4H1Topo_e_PreSamplerE, &b_AntiKt4H1Topo_e_PreSamplerE);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_EME1", &AntiKt4H1Topo_e_EME1, &b_AntiKt4H1Topo_e_EME1);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_EME2", &AntiKt4H1Topo_e_EME2, &b_AntiKt4H1Topo_e_EME2);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_EME3", &AntiKt4H1Topo_e_EME3, &b_AntiKt4H1Topo_e_EME3);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_HEC0", &AntiKt4H1Topo_e_HEC0, &b_AntiKt4H1Topo_e_HEC0);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_HEC1", &AntiKt4H1Topo_e_HEC1, &b_AntiKt4H1Topo_e_HEC1);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_HEC2", &AntiKt4H1Topo_e_HEC2, &b_AntiKt4H1Topo_e_HEC2);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_HEC3", &AntiKt4H1Topo_e_HEC3, &b_AntiKt4H1Topo_e_HEC3);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_TileBar0", &AntiKt4H1Topo_e_TileBar0, &b_AntiKt4H1Topo_e_TileBar0);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_TileBar1", &AntiKt4H1Topo_e_TileBar1, &b_AntiKt4H1Topo_e_TileBar1);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_TileBar2", &AntiKt4H1Topo_e_TileBar2, &b_AntiKt4H1Topo_e_TileBar2);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_TileGap1", &AntiKt4H1Topo_e_TileGap1, &b_AntiKt4H1Topo_e_TileGap1);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_TileGap2", &AntiKt4H1Topo_e_TileGap2, &b_AntiKt4H1Topo_e_TileGap2);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_TileGap3", &AntiKt4H1Topo_e_TileGap3, &b_AntiKt4H1Topo_e_TileGap3);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_TileExt0", &AntiKt4H1Topo_e_TileExt0, &b_AntiKt4H1Topo_e_TileExt0);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_TileExt1", &AntiKt4H1Topo_e_TileExt1, &b_AntiKt4H1Topo_e_TileExt1);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_TileExt2", &AntiKt4H1Topo_e_TileExt2, &b_AntiKt4H1Topo_e_TileExt2);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_FCAL0", &AntiKt4H1Topo_e_FCAL0, &b_AntiKt4H1Topo_e_FCAL0);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_FCAL1", &AntiKt4H1Topo_e_FCAL1, &b_AntiKt4H1Topo_e_FCAL1);
   fChain->SetBranchAddress("AntiKt4H1Topo_e_FCAL2", &AntiKt4H1Topo_e_FCAL2, &b_AntiKt4H1Topo_e_FCAL2);
   fChain->SetBranchAddress("AntiKt4H1Topo_shapeBins", &AntiKt4H1Topo_shapeBins, &b_AntiKt4H1Topo_shapeBins);
   fChain->SetBranchAddress("AntiKt4H1Topo_Nconst", &AntiKt4H1Topo_Nconst, &b_AntiKt4H1Topo_Nconst);
   fChain->SetBranchAddress("AntiKt4H1Topo_ptconst_emscale", &AntiKt4H1Topo_ptconst_emscale, &b_AntiKt4H1Topo_ptconst_emscale);
   fChain->SetBranchAddress("AntiKt4H1Topo_econst_emscale", &AntiKt4H1Topo_econst_emscale, &b_AntiKt4H1Topo_econst_emscale);
   fChain->SetBranchAddress("AntiKt4H1Topo_etaconst_emscale", &AntiKt4H1Topo_etaconst_emscale, &b_AntiKt4H1Topo_etaconst_emscale);
   fChain->SetBranchAddress("AntiKt4H1Topo_phiconst_emscale", &AntiKt4H1Topo_phiconst_emscale, &b_AntiKt4H1Topo_phiconst_emscale);
   fChain->SetBranchAddress("AntiKt4H1Topo_weightconst_emscale", &AntiKt4H1Topo_weightconst_emscale, &b_AntiKt4H1Topo_weightconst_emscale);
   fChain->SetBranchAddress("AntiKt4H1Topo_constscale_E", &AntiKt4H1Topo_constscale_E, &b_AntiKt4H1Topo_constscale_E);
   fChain->SetBranchAddress("AntiKt4H1Topo_constscale_pt", &AntiKt4H1Topo_constscale_pt, &b_AntiKt4H1Topo_constscale_pt);
   fChain->SetBranchAddress("AntiKt4H1Topo_constscale_m", &AntiKt4H1Topo_constscale_m, &b_AntiKt4H1Topo_constscale_m);
   fChain->SetBranchAddress("AntiKt4H1Topo_constscale_eta", &AntiKt4H1Topo_constscale_eta, &b_AntiKt4H1Topo_constscale_eta);
   fChain->SetBranchAddress("AntiKt4H1Topo_constscale_phi", &AntiKt4H1Topo_constscale_phi, &b_AntiKt4H1Topo_constscale_phi);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_ip2d_pu", &AntiKt4H1Topo_flavor_component_ip2d_pu, &b_AntiKt4H1Topo_flavor_component_ip2d_pu);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_ip2d_pb", &AntiKt4H1Topo_flavor_component_ip2d_pb, &b_AntiKt4H1Topo_flavor_component_ip2d_pb);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_ip2d_ntrk", &AntiKt4H1Topo_flavor_component_ip2d_ntrk, &b_AntiKt4H1Topo_flavor_component_ip2d_ntrk);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_ip3d_pu", &AntiKt4H1Topo_flavor_component_ip3d_pu, &b_AntiKt4H1Topo_flavor_component_ip3d_pu);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_ip3d_pb", &AntiKt4H1Topo_flavor_component_ip3d_pb, &b_AntiKt4H1Topo_flavor_component_ip3d_pb);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_ip3d_ntrk", &AntiKt4H1Topo_flavor_component_ip3d_ntrk, &b_AntiKt4H1Topo_flavor_component_ip3d_ntrk);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_sv1_pu", &AntiKt4H1Topo_flavor_component_sv1_pu, &b_AntiKt4H1Topo_flavor_component_sv1_pu);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_sv1_pb", &AntiKt4H1Topo_flavor_component_sv1_pb, &b_AntiKt4H1Topo_flavor_component_sv1_pb);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_sv2_pu", &AntiKt4H1Topo_flavor_component_sv2_pu, &b_AntiKt4H1Topo_flavor_component_sv2_pu);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_sv2_pb", &AntiKt4H1Topo_flavor_component_sv2_pb, &b_AntiKt4H1Topo_flavor_component_sv2_pb);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_svp_mass", &AntiKt4H1Topo_flavor_component_svp_mass, &b_AntiKt4H1Topo_flavor_component_svp_mass);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_svp_n2t", &AntiKt4H1Topo_flavor_component_svp_n2t, &b_AntiKt4H1Topo_flavor_component_svp_n2t);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_svp_svok", &AntiKt4H1Topo_flavor_component_svp_svok, &b_AntiKt4H1Topo_flavor_component_svp_svok);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_svp_ntrk", &AntiKt4H1Topo_flavor_component_svp_ntrk, &b_AntiKt4H1Topo_flavor_component_svp_ntrk);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_svp_ntrkv", &AntiKt4H1Topo_flavor_component_svp_ntrkv, &b_AntiKt4H1Topo_flavor_component_svp_ntrkv);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_svp_ntrkj", &AntiKt4H1Topo_flavor_component_svp_ntrkj, &b_AntiKt4H1Topo_flavor_component_svp_ntrkj);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_svp_efrc", &AntiKt4H1Topo_flavor_component_svp_efrc, &b_AntiKt4H1Topo_flavor_component_svp_efrc);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_sv0p_mass", &AntiKt4H1Topo_flavor_component_sv0p_mass, &b_AntiKt4H1Topo_flavor_component_sv0p_mass);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_sv0p_n2t", &AntiKt4H1Topo_flavor_component_sv0p_n2t, &b_AntiKt4H1Topo_flavor_component_sv0p_n2t);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_sv0p_svok", &AntiKt4H1Topo_flavor_component_sv0p_svok, &b_AntiKt4H1Topo_flavor_component_sv0p_svok);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_sv0p_ntrk", &AntiKt4H1Topo_flavor_component_sv0p_ntrk, &b_AntiKt4H1Topo_flavor_component_sv0p_ntrk);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_sv0p_ntrkv", &AntiKt4H1Topo_flavor_component_sv0p_ntrkv, &b_AntiKt4H1Topo_flavor_component_sv0p_ntrkv);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_sv0p_ntrkj", &AntiKt4H1Topo_flavor_component_sv0p_ntrkj, &b_AntiKt4H1Topo_flavor_component_sv0p_ntrkj);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_sv0p_efrc", &AntiKt4H1Topo_flavor_component_sv0p_efrc, &b_AntiKt4H1Topo_flavor_component_sv0p_efrc);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_jfit_pu", &AntiKt4H1Topo_flavor_component_jfit_pu, &b_AntiKt4H1Topo_flavor_component_jfit_pu);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_jfit_pb", &AntiKt4H1Topo_flavor_component_jfit_pb, &b_AntiKt4H1Topo_flavor_component_jfit_pb);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_jfit_nvtxnt", &AntiKt4H1Topo_flavor_component_jfit_nvtxnt, &b_AntiKt4H1Topo_flavor_component_jfit_nvtxnt);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_jfit_nvtx1t", &AntiKt4H1Topo_flavor_component_jfit_nvtx1t, &b_AntiKt4H1Topo_flavor_component_jfit_nvtx1t);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_jfit_ntrk", &AntiKt4H1Topo_flavor_component_jfit_ntrk, &b_AntiKt4H1Topo_flavor_component_jfit_ntrk);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_jfit_efrc", &AntiKt4H1Topo_flavor_component_jfit_efrc, &b_AntiKt4H1Topo_flavor_component_jfit_efrc);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_jfit_mass", &AntiKt4H1Topo_flavor_component_jfit_mass, &b_AntiKt4H1Topo_flavor_component_jfit_mass);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_component_jfit_sig3d", &AntiKt4H1Topo_flavor_component_jfit_sig3d, &b_AntiKt4H1Topo_flavor_component_jfit_sig3d);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight", &AntiKt4H1Topo_flavor_weight, &b_AntiKt4H1Topo_flavor_weight);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_TrackCounting2D", &AntiKt4H1Topo_flavor_weight_TrackCounting2D, &b_AntiKt4H1Topo_flavor_weight_TrackCounting2D);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_JetProb", &AntiKt4H1Topo_flavor_weight_JetProb, &b_AntiKt4H1Topo_flavor_weight_JetProb);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_IP1D", &AntiKt4H1Topo_flavor_weight_IP1D, &b_AntiKt4H1Topo_flavor_weight_IP1D);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_IP2D", &AntiKt4H1Topo_flavor_weight_IP2D, &b_AntiKt4H1Topo_flavor_weight_IP2D);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_IP3D", &AntiKt4H1Topo_flavor_weight_IP3D, &b_AntiKt4H1Topo_flavor_weight_IP3D);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_SV0", &AntiKt4H1Topo_flavor_weight_SV0, &b_AntiKt4H1Topo_flavor_weight_SV0);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_SV1", &AntiKt4H1Topo_flavor_weight_SV1, &b_AntiKt4H1Topo_flavor_weight_SV1);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_SV2", &AntiKt4H1Topo_flavor_weight_SV2, &b_AntiKt4H1Topo_flavor_weight_SV2);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_JetFitterTag", &AntiKt4H1Topo_flavor_weight_JetFitterTag, &b_AntiKt4H1Topo_flavor_weight_JetFitterTag);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_JetFitterCOMB", &AntiKt4H1Topo_flavor_weight_JetFitterCOMB, &b_AntiKt4H1Topo_flavor_weight_JetFitterCOMB);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_JetFitterTagNN", &AntiKt4H1Topo_flavor_weight_JetFitterTagNN, &b_AntiKt4H1Topo_flavor_weight_JetFitterTagNN);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_JetFitterCOMBNN", &AntiKt4H1Topo_flavor_weight_JetFitterCOMBNN, &b_AntiKt4H1Topo_flavor_weight_JetFitterCOMBNN);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_SoftMuonTag", &AntiKt4H1Topo_flavor_weight_SoftMuonTag, &b_AntiKt4H1Topo_flavor_weight_SoftMuonTag);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_weight_SoftElectronTag", &AntiKt4H1Topo_flavor_weight_SoftElectronTag, &b_AntiKt4H1Topo_flavor_weight_SoftElectronTag);
   fChain->SetBranchAddress("AntiKt4H1Topo_RoIword", &AntiKt4H1Topo_RoIword, &b_AntiKt4H1Topo_RoIword);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_truth_dRminToB", &AntiKt4H1Topo_flavor_truth_dRminToB, &b_AntiKt4H1Topo_flavor_truth_dRminToB);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_truth_dRminToC", &AntiKt4H1Topo_flavor_truth_dRminToC, &b_AntiKt4H1Topo_flavor_truth_dRminToC);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_truth_dRminToT", &AntiKt4H1Topo_flavor_truth_dRminToT, &b_AntiKt4H1Topo_flavor_truth_dRminToT);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_truth_BHadronpdg", &AntiKt4H1Topo_flavor_truth_BHadronpdg, &b_AntiKt4H1Topo_flavor_truth_BHadronpdg);
   fChain->SetBranchAddress("AntiKt4H1Topo_flavor_truth_trueflav", &AntiKt4H1Topo_flavor_truth_trueflav, &b_AntiKt4H1Topo_flavor_truth_trueflav);
   fChain->SetBranchAddress("AntiKt4H1Topo_el_dr", &AntiKt4H1Topo_el_dr, &b_AntiKt4H1Topo_el_dr);
   fChain->SetBranchAddress("AntiKt4H1Topo_el_matched", &AntiKt4H1Topo_el_matched, &b_AntiKt4H1Topo_el_matched);
   fChain->SetBranchAddress("AntiKt4H1Topo_mu_dr", &AntiKt4H1Topo_mu_dr, &b_AntiKt4H1Topo_mu_dr);
   fChain->SetBranchAddress("AntiKt4H1Topo_mu_matched", &AntiKt4H1Topo_mu_matched, &b_AntiKt4H1Topo_mu_matched);
   fChain->SetBranchAddress("trig_Nav_n", &trig_Nav_n, &b_trig_Nav_n);
   fChain->SetBranchAddress("trig_Nav_chain_ChainId", &trig_Nav_chain_ChainId, &b_trig_Nav_chain_ChainId);
   fChain->SetBranchAddress("trig_Nav_chain_RoIType", &trig_Nav_chain_RoIType, &b_trig_Nav_chain_RoIType);
   fChain->SetBranchAddress("trig_Nav_chain_RoIIndex", &trig_Nav_chain_RoIIndex, &b_trig_Nav_chain_RoIIndex);
   fChain->SetBranchAddress("trig_RoI_L2_b_n", &trig_RoI_L2_b_n, &b_trig_RoI_L2_b_n);
   fChain->SetBranchAddress("trig_RoI_L2_b_type", &trig_RoI_L2_b_type, &b_trig_RoI_L2_b_type);
   fChain->SetBranchAddress("trig_RoI_L2_b_lastStep", &trig_RoI_L2_b_lastStep, &b_trig_RoI_L2_b_lastStep);
   fChain->SetBranchAddress("trig_RoI_L2_b_Jet_ROI", &trig_RoI_L2_b_Jet_ROI, &b_trig_RoI_L2_b_Jet_ROI);
   fChain->SetBranchAddress("trig_RoI_L2_b_Jet_ROIStatus", &trig_RoI_L2_b_Jet_ROIStatus, &b_trig_RoI_L2_b_Jet_ROIStatus);
   fChain->SetBranchAddress("trig_RoI_L2_b_TrigL2BjetContainer", &trig_RoI_L2_b_TrigL2BjetContainer, &b_trig_RoI_L2_b_TrigL2BjetContainer);
   fChain->SetBranchAddress("trig_RoI_L2_b_TrigL2BjetContainerStatus", &trig_RoI_L2_b_TrigL2BjetContainerStatus, &b_trig_RoI_L2_b_TrigL2BjetContainerStatus);
   fChain->SetBranchAddress("trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_Jet", &trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_Jet, &b_trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_Jet);
   fChain->SetBranchAddress("trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_JetStatus", &trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_JetStatus, &b_trig_RoI_L2_b_TrigInDetTrackCollection_TrigSiTrack_JetStatus);
   fChain->SetBranchAddress("trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_Jet", &trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_Jet, &b_trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_Jet);
   fChain->SetBranchAddress("trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_JetStatus", &trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_JetStatus, &b_trig_RoI_L2_b_TrigInDetTrackCollection_TrigIDSCAN_JetStatus);
   fChain->SetBranchAddress("trig_RoI_EF_b_n", &trig_RoI_EF_b_n, &b_trig_RoI_EF_b_n);
   fChain->SetBranchAddress("trig_RoI_EF_b_type", &trig_RoI_EF_b_type, &b_trig_RoI_EF_b_type);
   fChain->SetBranchAddress("trig_RoI_EF_b_lastStep", &trig_RoI_EF_b_lastStep, &b_trig_RoI_EF_b_lastStep);
   fChain->SetBranchAddress("trig_RoI_EF_b_Jet_ROI", &trig_RoI_EF_b_Jet_ROI, &b_trig_RoI_EF_b_Jet_ROI);
   fChain->SetBranchAddress("trig_RoI_EF_b_Jet_ROIStatus", &trig_RoI_EF_b_Jet_ROIStatus, &b_trig_RoI_EF_b_Jet_ROIStatus);
   fChain->SetBranchAddress("trig_RoI_EF_b_TrigEFBjetContainer", &trig_RoI_EF_b_TrigEFBjetContainer, &b_trig_RoI_EF_b_TrigEFBjetContainer);
   fChain->SetBranchAddress("trig_RoI_EF_b_TrigEFBjetContainerStatus", &trig_RoI_EF_b_TrigEFBjetContainerStatus, &b_trig_RoI_EF_b_TrigEFBjetContainerStatus);
   fChain->SetBranchAddress("trig_RoI_EF_b_Rec::TrackParticleContainer", &trig_RoI_EF_b_Rec__TrackParticleContainer, &b_trig_RoI_EF_b_Rec__TrackParticleContainer);
   fChain->SetBranchAddress("trig_RoI_EF_b_Rec::TrackParticleContainerStatus", &trig_RoI_EF_b_Rec__TrackParticleContainerStatus, &b_trig_RoI_EF_b_Rec__TrackParticleContainerStatus);
   fChain->SetBranchAddress("trig_L1_jet_n", &trig_L1_jet_n, &b_trig_L1_jet_n);
   fChain->SetBranchAddress("trig_L1_jet_eta", &trig_L1_jet_eta, &b_trig_L1_jet_eta);
   fChain->SetBranchAddress("trig_L1_jet_phi", &trig_L1_jet_phi, &b_trig_L1_jet_phi);
   fChain->SetBranchAddress("trig_L1_jet_thrNames", &trig_L1_jet_thrNames, &b_trig_L1_jet_thrNames);
   fChain->SetBranchAddress("trig_L1_jet_thrValues", &trig_L1_jet_thrValues, &b_trig_L1_jet_thrValues);
   fChain->SetBranchAddress("trig_L1_jet_thrPattern", &trig_L1_jet_thrPattern, &b_trig_L1_jet_thrPattern);
   fChain->SetBranchAddress("trig_L1_jet_et4x4", &trig_L1_jet_et4x4, &b_trig_L1_jet_et4x4);
   fChain->SetBranchAddress("trig_L1_jet_et6x6", &trig_L1_jet_et6x6, &b_trig_L1_jet_et6x6);
   fChain->SetBranchAddress("trig_L1_jet_et8x8", &trig_L1_jet_et8x8, &b_trig_L1_jet_et8x8);
   fChain->SetBranchAddress("trig_L1_jet_RoIWord", &trig_L1_jet_RoIWord, &b_trig_L1_jet_RoIWord);
   fChain->SetBranchAddress("trig_DB_SMK", &trig_DB_SMK, &b_trig_DB_SMK);
   fChain->SetBranchAddress("trig_DB_L1PSK", &trig_DB_L1PSK, &b_trig_DB_L1PSK);
   fChain->SetBranchAddress("trig_DB_HLTPSK", &trig_DB_HLTPSK, &b_trig_DB_HLTPSK);
   fChain->SetBranchAddress("trig_L1_TAV", &trig_L1_TAV, &b_trig_L1_TAV);
   fChain->SetBranchAddress("trig_L2_passedPhysics", &trig_L2_passedPhysics, &b_trig_L2_passedPhysics);
   fChain->SetBranchAddress("trig_EF_passedPhysics", &trig_EF_passedPhysics, &b_trig_EF_passedPhysics);
   fChain->SetBranchAddress("trig_L1_TBP", &trig_L1_TBP, &b_trig_L1_TBP);
   fChain->SetBranchAddress("trig_L1_TAP", &trig_L1_TAP, &b_trig_L1_TAP);
   fChain->SetBranchAddress("trig_L2_passedRaw", &trig_L2_passedRaw, &b_trig_L2_passedRaw);
   fChain->SetBranchAddress("trig_EF_passedRaw", &trig_EF_passedRaw, &b_trig_EF_passedRaw);
   fChain->SetBranchAddress("trig_L2_resurrected", &trig_L2_resurrected, &b_trig_L2_resurrected);
   fChain->SetBranchAddress("trig_EF_resurrected", &trig_EF_resurrected, &b_trig_EF_resurrected);
   fChain->SetBranchAddress("trig_L2_passedThrough", &trig_L2_passedThrough, &b_trig_L2_passedThrough);
   fChain->SetBranchAddress("trig_EF_passedThrough", &trig_EF_passedThrough, &b_trig_EF_passedThrough);
   fChain->SetBranchAddress("trig_l2sitrk_jet_n", &trig_l2sitrk_jet_n, &b_trig_l2sitrk_jet_n);
   fChain->SetBranchAddress("trig_l2sitrk_jet_algorithmId", &trig_l2sitrk_jet_algorithmId, &b_trig_l2sitrk_jet_algorithmId);
   fChain->SetBranchAddress("trig_l2sitrk_jet_trackStatus", &trig_l2sitrk_jet_trackStatus, &b_trig_l2sitrk_jet_trackStatus);
   fChain->SetBranchAddress("trig_l2sitrk_jet_chi2Ndof", &trig_l2sitrk_jet_chi2Ndof, &b_trig_l2sitrk_jet_chi2Ndof);
   fChain->SetBranchAddress("trig_l2sitrk_jet_nStrawHits", &trig_l2sitrk_jet_nStrawHits, &b_trig_l2sitrk_jet_nStrawHits);
   fChain->SetBranchAddress("trig_l2sitrk_jet_nHighThrHits", &trig_l2sitrk_jet_nHighThrHits, &b_trig_l2sitrk_jet_nHighThrHits);
   fChain->SetBranchAddress("trig_l2sitrk_jet_nPixelSpacePoints", &trig_l2sitrk_jet_nPixelSpacePoints, &b_trig_l2sitrk_jet_nPixelSpacePoints);
   fChain->SetBranchAddress("trig_l2sitrk_jet_nSCT_SpacePoints", &trig_l2sitrk_jet_nSCT_SpacePoints, &b_trig_l2sitrk_jet_nSCT_SpacePoints);
   fChain->SetBranchAddress("trig_l2sitrk_jet_hitPattern", &trig_l2sitrk_jet_hitPattern, &b_trig_l2sitrk_jet_hitPattern);
   fChain->SetBranchAddress("trig_l2sitrk_jet_nStraw", &trig_l2sitrk_jet_nStraw, &b_trig_l2sitrk_jet_nStraw);
   fChain->SetBranchAddress("trig_l2sitrk_jet_nStrawTime", &trig_l2sitrk_jet_nStrawTime, &b_trig_l2sitrk_jet_nStrawTime);
   fChain->SetBranchAddress("trig_l2sitrk_jet_a0", &trig_l2sitrk_jet_a0, &b_trig_l2sitrk_jet_a0);
   fChain->SetBranchAddress("trig_l2sitrk_jet_z0", &trig_l2sitrk_jet_z0, &b_trig_l2sitrk_jet_z0);
   fChain->SetBranchAddress("trig_l2sitrk_jet_phi0", &trig_l2sitrk_jet_phi0, &b_trig_l2sitrk_jet_phi0);
   fChain->SetBranchAddress("trig_l2sitrk_jet_eta", &trig_l2sitrk_jet_eta, &b_trig_l2sitrk_jet_eta);
   fChain->SetBranchAddress("trig_l2sitrk_jet_pt", &trig_l2sitrk_jet_pt, &b_trig_l2sitrk_jet_pt);
   fChain->SetBranchAddress("trig_l2sitrk_jet_covariance", &trig_l2sitrk_jet_covariance, &b_trig_l2sitrk_jet_covariance);
   fChain->SetBranchAddress("trig_l2idtrk_jet_n", &trig_l2idtrk_jet_n, &b_trig_l2idtrk_jet_n);
   fChain->SetBranchAddress("trig_l2idtrk_jet_algorithmId", &trig_l2idtrk_jet_algorithmId, &b_trig_l2idtrk_jet_algorithmId);
   fChain->SetBranchAddress("trig_l2idtrk_jet_trackStatus", &trig_l2idtrk_jet_trackStatus, &b_trig_l2idtrk_jet_trackStatus);
   fChain->SetBranchAddress("trig_l2idtrk_jet_chi2Ndof", &trig_l2idtrk_jet_chi2Ndof, &b_trig_l2idtrk_jet_chi2Ndof);
   fChain->SetBranchAddress("trig_l2idtrk_jet_nStrawHits", &trig_l2idtrk_jet_nStrawHits, &b_trig_l2idtrk_jet_nStrawHits);
   fChain->SetBranchAddress("trig_l2idtrk_jet_nHighThrHits", &trig_l2idtrk_jet_nHighThrHits, &b_trig_l2idtrk_jet_nHighThrHits);
   fChain->SetBranchAddress("trig_l2idtrk_jet_nPixelSpacePoints", &trig_l2idtrk_jet_nPixelSpacePoints, &b_trig_l2idtrk_jet_nPixelSpacePoints);
   fChain->SetBranchAddress("trig_l2idtrk_jet_nSCT_SpacePoints", &trig_l2idtrk_jet_nSCT_SpacePoints, &b_trig_l2idtrk_jet_nSCT_SpacePoints);
   fChain->SetBranchAddress("trig_l2idtrk_jet_hitPattern", &trig_l2idtrk_jet_hitPattern, &b_trig_l2idtrk_jet_hitPattern);
   fChain->SetBranchAddress("trig_l2idtrk_jet_nStraw", &trig_l2idtrk_jet_nStraw, &b_trig_l2idtrk_jet_nStraw);
   fChain->SetBranchAddress("trig_l2idtrk_jet_nStrawTime", &trig_l2idtrk_jet_nStrawTime, &b_trig_l2idtrk_jet_nStrawTime);
   fChain->SetBranchAddress("trig_l2idtrk_jet_a0", &trig_l2idtrk_jet_a0, &b_trig_l2idtrk_jet_a0);
   fChain->SetBranchAddress("trig_l2idtrk_jet_z0", &trig_l2idtrk_jet_z0, &b_trig_l2idtrk_jet_z0);
   fChain->SetBranchAddress("trig_l2idtrk_jet_phi0", &trig_l2idtrk_jet_phi0, &b_trig_l2idtrk_jet_phi0);
   fChain->SetBranchAddress("trig_l2idtrk_jet_eta", &trig_l2idtrk_jet_eta, &b_trig_l2idtrk_jet_eta);
   fChain->SetBranchAddress("trig_l2idtrk_jet_pt", &trig_l2idtrk_jet_pt, &b_trig_l2idtrk_jet_pt);
   fChain->SetBranchAddress("trig_l2idtrk_jet_covariance", &trig_l2idtrk_jet_covariance, &b_trig_l2idtrk_jet_covariance);
   fChain->SetBranchAddress("trig_bjefid_n", &trig_bjefid_n, &b_trig_bjefid_n);
   fChain->SetBranchAddress("trig_bjefid_d0", &trig_bjefid_d0, &b_trig_bjefid_d0);
   fChain->SetBranchAddress("trig_bjefid_z0", &trig_bjefid_z0, &b_trig_bjefid_z0);
   fChain->SetBranchAddress("trig_bjefid_phi", &trig_bjefid_phi, &b_trig_bjefid_phi);
   fChain->SetBranchAddress("trig_bjefid_theta", &trig_bjefid_theta, &b_trig_bjefid_theta);
   fChain->SetBranchAddress("trig_bjefid_qoverp", &trig_bjefid_qoverp, &b_trig_bjefid_qoverp);
   fChain->SetBranchAddress("trig_bjefid_pt", &trig_bjefid_pt, &b_trig_bjefid_pt);
   fChain->SetBranchAddress("trig_bjefid_eta", &trig_bjefid_eta, &b_trig_bjefid_eta);
   fChain->SetBranchAddress("trig_bjefid_err_d0", &trig_bjefid_err_d0, &b_trig_bjefid_err_d0);
   fChain->SetBranchAddress("trig_bjefid_err_z0", &trig_bjefid_err_z0, &b_trig_bjefid_err_z0);
   fChain->SetBranchAddress("trig_bjefid_err_phi", &trig_bjefid_err_phi, &b_trig_bjefid_err_phi);
   fChain->SetBranchAddress("trig_bjefid_err_theta", &trig_bjefid_err_theta, &b_trig_bjefid_err_theta);
   fChain->SetBranchAddress("trig_bjefid_err_qoverp", &trig_bjefid_err_qoverp, &b_trig_bjefid_err_qoverp);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_z0", &trig_bjefid_cov_d0_z0, &b_trig_bjefid_cov_d0_z0);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_phi", &trig_bjefid_cov_d0_phi, &b_trig_bjefid_cov_d0_phi);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_theta", &trig_bjefid_cov_d0_theta, &b_trig_bjefid_cov_d0_theta);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_qoverp", &trig_bjefid_cov_d0_qoverp, &b_trig_bjefid_cov_d0_qoverp);
   fChain->SetBranchAddress("trig_bjefid_cov_z0_phi", &trig_bjefid_cov_z0_phi, &b_trig_bjefid_cov_z0_phi);
   fChain->SetBranchAddress("trig_bjefid_cov_z0_theta", &trig_bjefid_cov_z0_theta, &b_trig_bjefid_cov_z0_theta);
   fChain->SetBranchAddress("trig_bjefid_cov_z0_qoverp", &trig_bjefid_cov_z0_qoverp, &b_trig_bjefid_cov_z0_qoverp);
   fChain->SetBranchAddress("trig_bjefid_cov_phi_theta", &trig_bjefid_cov_phi_theta, &b_trig_bjefid_cov_phi_theta);
   fChain->SetBranchAddress("trig_bjefid_cov_phi_qoverp", &trig_bjefid_cov_phi_qoverp, &b_trig_bjefid_cov_phi_qoverp);
   fChain->SetBranchAddress("trig_bjefid_cov_theta_qoverp", &trig_bjefid_cov_theta_qoverp, &b_trig_bjefid_cov_theta_qoverp);
   fChain->SetBranchAddress("trig_bjefid_d0_wrtPV", &trig_bjefid_d0_wrtPV, &b_trig_bjefid_d0_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_z0_wrtPV", &trig_bjefid_z0_wrtPV, &b_trig_bjefid_z0_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_phi_wrtPV", &trig_bjefid_phi_wrtPV, &b_trig_bjefid_phi_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_err_d0_wrtPV", &trig_bjefid_err_d0_wrtPV, &b_trig_bjefid_err_d0_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_err_z0_wrtPV", &trig_bjefid_err_z0_wrtPV, &b_trig_bjefid_err_z0_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_err_phi_wrtPV", &trig_bjefid_err_phi_wrtPV, &b_trig_bjefid_err_phi_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_err_theta_wrtPV", &trig_bjefid_err_theta_wrtPV, &b_trig_bjefid_err_theta_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_err_qoverp_wrtPV", &trig_bjefid_err_qoverp_wrtPV, &b_trig_bjefid_err_qoverp_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_z0_wrtPV", &trig_bjefid_cov_d0_z0_wrtPV, &b_trig_bjefid_cov_d0_z0_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_phi_wrtPV", &trig_bjefid_cov_d0_phi_wrtPV, &b_trig_bjefid_cov_d0_phi_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_theta_wrtPV", &trig_bjefid_cov_d0_theta_wrtPV, &b_trig_bjefid_cov_d0_theta_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_qoverp_wrtPV", &trig_bjefid_cov_d0_qoverp_wrtPV, &b_trig_bjefid_cov_d0_qoverp_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_cov_z0_phi_wrtPV", &trig_bjefid_cov_z0_phi_wrtPV, &b_trig_bjefid_cov_z0_phi_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_cov_z0_theta_wrtPV", &trig_bjefid_cov_z0_theta_wrtPV, &b_trig_bjefid_cov_z0_theta_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_cov_z0_qoverp_wrtPV", &trig_bjefid_cov_z0_qoverp_wrtPV, &b_trig_bjefid_cov_z0_qoverp_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_cov_phi_theta_wrtPV", &trig_bjefid_cov_phi_theta_wrtPV, &b_trig_bjefid_cov_phi_theta_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_cov_phi_qoverp_wrtPV", &trig_bjefid_cov_phi_qoverp_wrtPV, &b_trig_bjefid_cov_phi_qoverp_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_cov_theta_qoverp_wrtPV", &trig_bjefid_cov_theta_qoverp_wrtPV, &b_trig_bjefid_cov_theta_qoverp_wrtPV);
   fChain->SetBranchAddress("trig_bjefid_d0_wrtBS", &trig_bjefid_d0_wrtBS, &b_trig_bjefid_d0_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_z0_wrtBS", &trig_bjefid_z0_wrtBS, &b_trig_bjefid_z0_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_phi_wrtBS", &trig_bjefid_phi_wrtBS, &b_trig_bjefid_phi_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_err_d0_wrtBS", &trig_bjefid_err_d0_wrtBS, &b_trig_bjefid_err_d0_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_err_z0_wrtBS", &trig_bjefid_err_z0_wrtBS, &b_trig_bjefid_err_z0_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_err_phi_wrtBS", &trig_bjefid_err_phi_wrtBS, &b_trig_bjefid_err_phi_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_err_theta_wrtBS", &trig_bjefid_err_theta_wrtBS, &b_trig_bjefid_err_theta_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_err_qoverp_wrtBS", &trig_bjefid_err_qoverp_wrtBS, &b_trig_bjefid_err_qoverp_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_z0_wrtBS", &trig_bjefid_cov_d0_z0_wrtBS, &b_trig_bjefid_cov_d0_z0_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_phi_wrtBS", &trig_bjefid_cov_d0_phi_wrtBS, &b_trig_bjefid_cov_d0_phi_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_theta_wrtBS", &trig_bjefid_cov_d0_theta_wrtBS, &b_trig_bjefid_cov_d0_theta_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_cov_d0_qoverp_wrtBS", &trig_bjefid_cov_d0_qoverp_wrtBS, &b_trig_bjefid_cov_d0_qoverp_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_cov_z0_phi_wrtBS", &trig_bjefid_cov_z0_phi_wrtBS, &b_trig_bjefid_cov_z0_phi_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_cov_z0_theta_wrtBS", &trig_bjefid_cov_z0_theta_wrtBS, &b_trig_bjefid_cov_z0_theta_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_cov_z0_qoverp_wrtBS", &trig_bjefid_cov_z0_qoverp_wrtBS, &b_trig_bjefid_cov_z0_qoverp_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_cov_phi_theta_wrtBS", &trig_bjefid_cov_phi_theta_wrtBS, &b_trig_bjefid_cov_phi_theta_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_cov_phi_qoverp_wrtBS", &trig_bjefid_cov_phi_qoverp_wrtBS, &b_trig_bjefid_cov_phi_qoverp_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_cov_theta_qoverp_wrtBS", &trig_bjefid_cov_theta_qoverp_wrtBS, &b_trig_bjefid_cov_theta_qoverp_wrtBS);
   fChain->SetBranchAddress("trig_bjefid_chi2", &trig_bjefid_chi2, &b_trig_bjefid_chi2);
   fChain->SetBranchAddress("trig_bjefid_ndof", &trig_bjefid_ndof, &b_trig_bjefid_ndof);
   fChain->SetBranchAddress("trig_bjefid_nBLHits", &trig_bjefid_nBLHits, &b_trig_bjefid_nBLHits);
   fChain->SetBranchAddress("trig_bjefid_nPixHits", &trig_bjefid_nPixHits, &b_trig_bjefid_nPixHits);
   fChain->SetBranchAddress("trig_bjefid_nSCTHits", &trig_bjefid_nSCTHits, &b_trig_bjefid_nSCTHits);
   fChain->SetBranchAddress("trig_bjefid_nTRTHits", &trig_bjefid_nTRTHits, &b_trig_bjefid_nTRTHits);
   fChain->SetBranchAddress("trig_bjefid_nPixHoles", &trig_bjefid_nPixHoles, &b_trig_bjefid_nPixHoles);
   fChain->SetBranchAddress("trig_bjefid_nSCTHoles", &trig_bjefid_nSCTHoles, &b_trig_bjefid_nSCTHoles);
   fChain->SetBranchAddress("trig_bjefid_nMDTHits", &trig_bjefid_nMDTHits, &b_trig_bjefid_nMDTHits);
   fChain->SetBranchAddress("trig_bjefid_nCSCEtaHits", &trig_bjefid_nCSCEtaHits, &b_trig_bjefid_nCSCEtaHits);
   fChain->SetBranchAddress("trig_bjefid_nCSCPhiHits", &trig_bjefid_nCSCPhiHits, &b_trig_bjefid_nCSCPhiHits);
   fChain->SetBranchAddress("trig_bjefid_nRPCEtaHits", &trig_bjefid_nRPCEtaHits, &b_trig_bjefid_nRPCEtaHits);
   fChain->SetBranchAddress("trig_bjefid_nRPCPhiHits", &trig_bjefid_nRPCPhiHits, &b_trig_bjefid_nRPCPhiHits);
   fChain->SetBranchAddress("trig_bjefid_nTGCEtaHits", &trig_bjefid_nTGCEtaHits, &b_trig_bjefid_nTGCEtaHits);
   fChain->SetBranchAddress("trig_bjefid_nTGCPhiHits", &trig_bjefid_nTGCPhiHits, &b_trig_bjefid_nTGCPhiHits);
   fChain->SetBranchAddress("trig_bjefid_nHits", &trig_bjefid_nHits, &b_trig_bjefid_nHits);
   fChain->SetBranchAddress("trig_bjefid_nHoles", &trig_bjefid_nHoles, &b_trig_bjefid_nHoles);
   fChain->SetBranchAddress("trig_bjefid_hitPattern", &trig_bjefid_hitPattern, &b_trig_bjefid_hitPattern);
   fChain->SetBranchAddress("trig_bjefid_TRTHighTHitsRatio", &trig_bjefid_TRTHighTHitsRatio, &b_trig_bjefid_TRTHighTHitsRatio);
   fChain->SetBranchAddress("trig_bjefid_pixeldEdx", &trig_bjefid_pixeldEdx, &b_trig_bjefid_pixeldEdx);
   fChain->SetBranchAddress("trig_bjefid_fitter", &trig_bjefid_fitter, &b_trig_bjefid_fitter);
   fChain->SetBranchAddress("trig_bjefid_patternReco1", &trig_bjefid_patternReco1, &b_trig_bjefid_patternReco1);
   fChain->SetBranchAddress("trig_bjefid_patternReco2", &trig_bjefid_patternReco2, &b_trig_bjefid_patternReco2);
   fChain->SetBranchAddress("trig_bjefid_seedFinder", &trig_bjefid_seedFinder, &b_trig_bjefid_seedFinder);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_expectHit", &trig_bjefid_blayerPrediction_expectHit, &b_trig_bjefid_blayerPrediction_expectHit);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_x", &trig_bjefid_blayerPrediction_x, &b_trig_bjefid_blayerPrediction_x);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_y", &trig_bjefid_blayerPrediction_y, &b_trig_bjefid_blayerPrediction_y);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_z", &trig_bjefid_blayerPrediction_z, &b_trig_bjefid_blayerPrediction_z);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_locX", &trig_bjefid_blayerPrediction_locX, &b_trig_bjefid_blayerPrediction_locX);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_locY", &trig_bjefid_blayerPrediction_locY, &b_trig_bjefid_blayerPrediction_locY);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_err_locX", &trig_bjefid_blayerPrediction_err_locX, &b_trig_bjefid_blayerPrediction_err_locX);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_err_locY", &trig_bjefid_blayerPrediction_err_locY, &b_trig_bjefid_blayerPrediction_err_locY);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_etaDistToEdge", &trig_bjefid_blayerPrediction_etaDistToEdge, &b_trig_bjefid_blayerPrediction_etaDistToEdge);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_phiDistToEdge", &trig_bjefid_blayerPrediction_phiDistToEdge, &b_trig_bjefid_blayerPrediction_phiDistToEdge);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_detElementId", &trig_bjefid_blayerPrediction_detElementId, &b_trig_bjefid_blayerPrediction_detElementId);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_row", &trig_bjefid_blayerPrediction_row, &b_trig_bjefid_blayerPrediction_row);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_col", &trig_bjefid_blayerPrediction_col, &b_trig_bjefid_blayerPrediction_col);
   fChain->SetBranchAddress("trig_bjefid_blayerPrediction_type", &trig_bjefid_blayerPrediction_type, &b_trig_bjefid_blayerPrediction_type);
   fChain->SetBranchAddress("trig_bjefid_mc_probability", &trig_bjefid_mc_probability, &b_trig_bjefid_mc_probability);
   fChain->SetBranchAddress("trig_bjefid_mc_barcode", &trig_bjefid_mc_barcode, &b_trig_bjefid_mc_barcode);
   fChain->SetBranchAddress("trig_bjefid_mc_index", &trig_bjefid_mc_index, &b_trig_bjefid_mc_index);
   fChain->SetBranchAddress("trig_bjefid_vertx", &trig_bjefid_vertx, &b_trig_bjefid_vertx);
   fChain->SetBranchAddress("trig_bjefid_verty", &trig_bjefid_verty, &b_trig_bjefid_verty);
   fChain->SetBranchAddress("trig_bjefid_vertz", &trig_bjefid_vertz, &b_trig_bjefid_vertz);
   fChain->SetBranchAddress("trig_l2bj_n", &trig_l2bj_n, &b_trig_l2bj_n);
   fChain->SetBranchAddress("trig_l2bj_roiId", &trig_l2bj_roiId, &b_trig_l2bj_roiId);
   fChain->SetBranchAddress("trig_l2bj_valid", &trig_l2bj_valid, &b_trig_l2bj_valid);
   fChain->SetBranchAddress("trig_l2bj_prmVtx", &trig_l2bj_prmVtx, &b_trig_l2bj_prmVtx);
   fChain->SetBranchAddress("trig_l2bj_xComb", &trig_l2bj_xComb, &b_trig_l2bj_xComb);
   fChain->SetBranchAddress("trig_l2bj_xIP1D", &trig_l2bj_xIP1D, &b_trig_l2bj_xIP1D);
   fChain->SetBranchAddress("trig_l2bj_xIP2D", &trig_l2bj_xIP2D, &b_trig_l2bj_xIP2D);
   fChain->SetBranchAddress("trig_l2bj_xIP3D", &trig_l2bj_xIP3D, &b_trig_l2bj_xIP3D);
   fChain->SetBranchAddress("trig_l2bj_xCHI2", &trig_l2bj_xCHI2, &b_trig_l2bj_xCHI2);
   fChain->SetBranchAddress("trig_l2bj_xSV", &trig_l2bj_xSV, &b_trig_l2bj_xSV);
   fChain->SetBranchAddress("trig_l2bj_xMVtx", &trig_l2bj_xMVtx, &b_trig_l2bj_xMVtx);
   fChain->SetBranchAddress("trig_l2bj_xEVtx", &trig_l2bj_xEVtx, &b_trig_l2bj_xEVtx);
   fChain->SetBranchAddress("trig_l2bj_xNVtx", &trig_l2bj_xNVtx, &b_trig_l2bj_xNVtx);
   fChain->SetBranchAddress("trig_l2bj_eta", &trig_l2bj_eta, &b_trig_l2bj_eta);
   fChain->SetBranchAddress("trig_l2bj_phi", &trig_l2bj_phi, &b_trig_l2bj_phi);
   fChain->SetBranchAddress("trig_l2bj_trksi_n", &trig_l2bj_trksi_n, &b_trig_l2bj_trksi_n);
   fChain->SetBranchAddress("trig_l2bj_trksi_index", &trig_l2bj_trksi_index, &b_trig_l2bj_trksi_index);
   fChain->SetBranchAddress("trig_l2bj_trkid_n", &trig_l2bj_trkid_n, &b_trig_l2bj_trkid_n);
   fChain->SetBranchAddress("trig_l2bj_trkid_index", &trig_l2bj_trkid_index, &b_trig_l2bj_trkid_index);
   fChain->SetBranchAddress("trig_efbj_n", &trig_efbj_n, &b_trig_efbj_n);
   fChain->SetBranchAddress("trig_efbj_roiId", &trig_efbj_roiId, &b_trig_efbj_roiId);
   fChain->SetBranchAddress("trig_efbj_valid", &trig_efbj_valid, &b_trig_efbj_valid);
   fChain->SetBranchAddress("trig_efbj_prmVtx", &trig_efbj_prmVtx, &b_trig_efbj_prmVtx);
   fChain->SetBranchAddress("trig_efbj_xComb", &trig_efbj_xComb, &b_trig_efbj_xComb);
   fChain->SetBranchAddress("trig_efbj_xIP1D", &trig_efbj_xIP1D, &b_trig_efbj_xIP1D);
   fChain->SetBranchAddress("trig_efbj_xIP2D", &trig_efbj_xIP2D, &b_trig_efbj_xIP2D);
   fChain->SetBranchAddress("trig_efbj_xIP3D", &trig_efbj_xIP3D, &b_trig_efbj_xIP3D);
   fChain->SetBranchAddress("trig_efbj_xCHI2", &trig_efbj_xCHI2, &b_trig_efbj_xCHI2);
   fChain->SetBranchAddress("trig_efbj_xSV", &trig_efbj_xSV, &b_trig_efbj_xSV);
   fChain->SetBranchAddress("trig_efbj_xMVtx", &trig_efbj_xMVtx, &b_trig_efbj_xMVtx);
   fChain->SetBranchAddress("trig_efbj_xEVtx", &trig_efbj_xEVtx, &b_trig_efbj_xEVtx);
   fChain->SetBranchAddress("trig_efbj_xNVtx", &trig_efbj_xNVtx, &b_trig_efbj_xNVtx);
   fChain->SetBranchAddress("trig_efbj_eta", &trig_efbj_eta, &b_trig_efbj_eta);
   fChain->SetBranchAddress("trig_efbj_phi", &trig_efbj_phi, &b_trig_efbj_phi);
   fChain->SetBranchAddress("trig_efbj_trk_n", &trig_efbj_trk_n, &b_trig_efbj_trk_n);
   fChain->SetBranchAddress("trig_efbj_trk_index", &trig_efbj_trk_index, &b_trig_efbj_trk_index);
   fChain->SetBranchAddress("el_n", &el_n, &b_el_n);
   fChain->SetBranchAddress("el_E", &el_E, &b_el_E);
   fChain->SetBranchAddress("el_Et", &el_Et, &b_el_Et);
   fChain->SetBranchAddress("el_pt", &el_pt, &b_el_pt);
   fChain->SetBranchAddress("el_m", &el_m, &b_el_m);
   fChain->SetBranchAddress("el_eta", &el_eta, &b_el_eta);
   fChain->SetBranchAddress("el_phi", &el_phi, &b_el_phi);
   fChain->SetBranchAddress("el_px", &el_px, &b_el_px);
   fChain->SetBranchAddress("el_py", &el_py, &b_el_py);
   fChain->SetBranchAddress("el_pz", &el_pz, &b_el_pz);
   fChain->SetBranchAddress("el_charge", &el_charge, &b_el_charge);
   fChain->SetBranchAddress("el_author", &el_author, &b_el_author);
   fChain->SetBranchAddress("el_isEM", &el_isEM, &b_el_isEM);
   fChain->SetBranchAddress("el_convFlag", &el_convFlag, &b_el_convFlag);
   fChain->SetBranchAddress("el_isConv", &el_isConv, &b_el_isConv);
   fChain->SetBranchAddress("el_nConv", &el_nConv, &b_el_nConv);
   fChain->SetBranchAddress("el_nSingleTrackConv", &el_nSingleTrackConv, &b_el_nSingleTrackConv);
   fChain->SetBranchAddress("el_nDoubleTrackConv", &el_nDoubleTrackConv, &b_el_nDoubleTrackConv);
   fChain->SetBranchAddress("el_type", &el_type, &b_el_type);
   fChain->SetBranchAddress("el_origin", &el_origin, &b_el_origin);
   fChain->SetBranchAddress("el_truth_E", &el_truth_E, &b_el_truth_E);
   fChain->SetBranchAddress("el_truth_pt", &el_truth_pt, &b_el_truth_pt);
   fChain->SetBranchAddress("el_truth_eta", &el_truth_eta, &b_el_truth_eta);
   fChain->SetBranchAddress("el_truth_phi", &el_truth_phi, &b_el_truth_phi);
   fChain->SetBranchAddress("el_truth_type", &el_truth_type, &b_el_truth_type);
   fChain->SetBranchAddress("el_truth_status", &el_truth_status, &b_el_truth_status);
   fChain->SetBranchAddress("el_truth_barcode", &el_truth_barcode, &b_el_truth_barcode);
   fChain->SetBranchAddress("el_truth_mothertype", &el_truth_mothertype, &b_el_truth_mothertype);
   fChain->SetBranchAddress("el_truth_motherbarcode", &el_truth_motherbarcode, &b_el_truth_motherbarcode);
   fChain->SetBranchAddress("el_truth_hasHardBrem", &el_truth_hasHardBrem, &b_el_truth_hasHardBrem);
   fChain->SetBranchAddress("el_truth_matched", &el_truth_matched, &b_el_truth_matched);
   fChain->SetBranchAddress("el_loose", &el_loose, &b_el_loose);
   fChain->SetBranchAddress("el_medium", &el_medium, &b_el_medium);
   fChain->SetBranchAddress("el_mediumIso", &el_mediumIso, &b_el_mediumIso);
   fChain->SetBranchAddress("el_tight", &el_tight, &b_el_tight);
   fChain->SetBranchAddress("el_tightIso", &el_tightIso, &b_el_tightIso);
   fChain->SetBranchAddress("el_Ethad", &el_Ethad, &b_el_Ethad);
   fChain->SetBranchAddress("el_Ethad1", &el_Ethad1, &b_el_Ethad1);
   fChain->SetBranchAddress("el_f1", &el_f1, &b_el_f1);
   fChain->SetBranchAddress("el_f1core", &el_f1core, &b_el_f1core);
   fChain->SetBranchAddress("el_Emins1", &el_Emins1, &b_el_Emins1);
   fChain->SetBranchAddress("el_fside", &el_fside, &b_el_fside);
   fChain->SetBranchAddress("el_Emax2", &el_Emax2, &b_el_Emax2);
   fChain->SetBranchAddress("el_ws3", &el_ws3, &b_el_ws3);
   fChain->SetBranchAddress("el_wstot", &el_wstot, &b_el_wstot);
   fChain->SetBranchAddress("el_emaxs1", &el_emaxs1, &b_el_emaxs1);
   fChain->SetBranchAddress("el_deltaEs", &el_deltaEs, &b_el_deltaEs);
   fChain->SetBranchAddress("el_E233", &el_E233, &b_el_E233);
   fChain->SetBranchAddress("el_E237", &el_E237, &b_el_E237);
   fChain->SetBranchAddress("el_E277", &el_E277, &b_el_E277);
   fChain->SetBranchAddress("el_weta2", &el_weta2, &b_el_weta2);
   fChain->SetBranchAddress("el_f3", &el_f3, &b_el_f3);
   fChain->SetBranchAddress("el_f3core", &el_f3core, &b_el_f3core);
   fChain->SetBranchAddress("el_rphiallcalo", &el_rphiallcalo, &b_el_rphiallcalo);
   fChain->SetBranchAddress("el_Etcone45", &el_Etcone45, &b_el_Etcone45);
   fChain->SetBranchAddress("el_Etcone20", &el_Etcone20, &b_el_Etcone20);
   fChain->SetBranchAddress("el_Etcone30", &el_Etcone30, &b_el_Etcone30);
   fChain->SetBranchAddress("el_Etcone40", &el_Etcone40, &b_el_Etcone40);
   fChain->SetBranchAddress("el_ptcone30", &el_ptcone30, &b_el_ptcone30);
   fChain->SetBranchAddress("el_convanglematch", &el_convanglematch, &b_el_convanglematch);
   fChain->SetBranchAddress("el_convtrackmatch", &el_convtrackmatch, &b_el_convtrackmatch);
   fChain->SetBranchAddress("el_hasconv", &el_hasconv, &b_el_hasconv);
   fChain->SetBranchAddress("el_convvtxx", &el_convvtxx, &b_el_convvtxx);
   fChain->SetBranchAddress("el_convvtxy", &el_convvtxy, &b_el_convvtxy);
   fChain->SetBranchAddress("el_convvtxz", &el_convvtxz, &b_el_convvtxz);
   fChain->SetBranchAddress("el_Rconv", &el_Rconv, &b_el_Rconv);
   fChain->SetBranchAddress("el_zconv", &el_zconv, &b_el_zconv);
   fChain->SetBranchAddress("el_convvtxchi2", &el_convvtxchi2, &b_el_convvtxchi2);
   fChain->SetBranchAddress("el_pt1conv", &el_pt1conv, &b_el_pt1conv);
   fChain->SetBranchAddress("el_convtrk1nBLHits", &el_convtrk1nBLHits, &b_el_convtrk1nBLHits);
   fChain->SetBranchAddress("el_convtrk1nPixHits", &el_convtrk1nPixHits, &b_el_convtrk1nPixHits);
   fChain->SetBranchAddress("el_convtrk1nSCTHits", &el_convtrk1nSCTHits, &b_el_convtrk1nSCTHits);
   fChain->SetBranchAddress("el_convtrk1nTRTHits", &el_convtrk1nTRTHits, &b_el_convtrk1nTRTHits);
   fChain->SetBranchAddress("el_pt2conv", &el_pt2conv, &b_el_pt2conv);
   fChain->SetBranchAddress("el_convtrk2nBLHits", &el_convtrk2nBLHits, &b_el_convtrk2nBLHits);
   fChain->SetBranchAddress("el_convtrk2nPixHits", &el_convtrk2nPixHits, &b_el_convtrk2nPixHits);
   fChain->SetBranchAddress("el_convtrk2nSCTHits", &el_convtrk2nSCTHits, &b_el_convtrk2nSCTHits);
   fChain->SetBranchAddress("el_convtrk2nTRTHits", &el_convtrk2nTRTHits, &b_el_convtrk2nTRTHits);
   fChain->SetBranchAddress("el_ptconv", &el_ptconv, &b_el_ptconv);
   fChain->SetBranchAddress("el_pzconv", &el_pzconv, &b_el_pzconv);
   fChain->SetBranchAddress("el_pos7", &el_pos7, &b_el_pos7);
   fChain->SetBranchAddress("el_etacorrmag", &el_etacorrmag, &b_el_etacorrmag);
   fChain->SetBranchAddress("el_deltaeta1", &el_deltaeta1, &b_el_deltaeta1);
   fChain->SetBranchAddress("el_deltaeta2", &el_deltaeta2, &b_el_deltaeta2);
   fChain->SetBranchAddress("el_deltaphi2", &el_deltaphi2, &b_el_deltaphi2);
   fChain->SetBranchAddress("el_deltaphiRescaled", &el_deltaphiRescaled, &b_el_deltaphiRescaled);
   fChain->SetBranchAddress("el_expectHitInBLayer", &el_expectHitInBLayer, &b_el_expectHitInBLayer);
   fChain->SetBranchAddress("el_reta", &el_reta, &b_el_reta);
   fChain->SetBranchAddress("el_rphi", &el_rphi, &b_el_rphi);
   fChain->SetBranchAddress("el_EtringnoisedR03sig2", &el_EtringnoisedR03sig2, &b_el_EtringnoisedR03sig2);
   fChain->SetBranchAddress("el_EtringnoisedR03sig3", &el_EtringnoisedR03sig3, &b_el_EtringnoisedR03sig3);
   fChain->SetBranchAddress("el_EtringnoisedR03sig4", &el_EtringnoisedR03sig4, &b_el_EtringnoisedR03sig4);
   fChain->SetBranchAddress("el_isolationlikelihoodjets", &el_isolationlikelihoodjets, &b_el_isolationlikelihoodjets);
   fChain->SetBranchAddress("el_isolationlikelihoodhqelectrons", &el_isolationlikelihoodhqelectrons, &b_el_isolationlikelihoodhqelectrons);
   fChain->SetBranchAddress("el_electronweight", &el_electronweight, &b_el_electronweight);
   fChain->SetBranchAddress("el_electronbgweight", &el_electronbgweight, &b_el_electronbgweight);
   fChain->SetBranchAddress("el_softeweight", &el_softeweight, &b_el_softeweight);
   fChain->SetBranchAddress("el_softebgweight", &el_softebgweight, &b_el_softebgweight);
   fChain->SetBranchAddress("el_neuralnet", &el_neuralnet, &b_el_neuralnet);
   fChain->SetBranchAddress("el_Hmatrix", &el_Hmatrix, &b_el_Hmatrix);
   fChain->SetBranchAddress("el_Hmatrix5", &el_Hmatrix5, &b_el_Hmatrix5);
   fChain->SetBranchAddress("el_adaboost", &el_adaboost, &b_el_adaboost);
   fChain->SetBranchAddress("el_softeneuralnet", &el_softeneuralnet, &b_el_softeneuralnet);
   fChain->SetBranchAddress("el_zvertex", &el_zvertex, &b_el_zvertex);
   fChain->SetBranchAddress("el_errz", &el_errz, &b_el_errz);
   fChain->SetBranchAddress("el_etap", &el_etap, &b_el_etap);
   fChain->SetBranchAddress("el_depth", &el_depth, &b_el_depth);
   fChain->SetBranchAddress("el_refittedTrack_n", &el_refittedTrack_n, &b_el_refittedTrack_n);
   fChain->SetBranchAddress("el_refittedTrack_author", &el_refittedTrack_author, &b_el_refittedTrack_author);
   fChain->SetBranchAddress("el_refittedTrack_chi2", &el_refittedTrack_chi2, &b_el_refittedTrack_chi2);
   fChain->SetBranchAddress("el_refittedTrack_qoverp", &el_refittedTrack_qoverp, &b_el_refittedTrack_qoverp);
   fChain->SetBranchAddress("el_refittedTrack_d0", &el_refittedTrack_d0, &b_el_refittedTrack_d0);
   fChain->SetBranchAddress("el_refittedTrack_z0", &el_refittedTrack_z0, &b_el_refittedTrack_z0);
   fChain->SetBranchAddress("el_refittedTrack_theta", &el_refittedTrack_theta, &b_el_refittedTrack_theta);
   fChain->SetBranchAddress("el_refittedTrack_phi", &el_refittedTrack_phi, &b_el_refittedTrack_phi);
   fChain->SetBranchAddress("el_refittedTrack_LMqoverp", &el_refittedTrack_LMqoverp, &b_el_refittedTrack_LMqoverp);
   fChain->SetBranchAddress("el_refittedTrack_hasBrem", &el_refittedTrack_hasBrem, &b_el_refittedTrack_hasBrem);
   fChain->SetBranchAddress("el_refittedTrack_bremRadius", &el_refittedTrack_bremRadius, &b_el_refittedTrack_bremRadius);
   fChain->SetBranchAddress("el_refittedTrack_bremZ", &el_refittedTrack_bremZ, &b_el_refittedTrack_bremZ);
   fChain->SetBranchAddress("el_refittedTrack_bremRadiusErr", &el_refittedTrack_bremRadiusErr, &b_el_refittedTrack_bremRadiusErr);
   fChain->SetBranchAddress("el_refittedTrack_bremZErr", &el_refittedTrack_bremZErr, &b_el_refittedTrack_bremZErr);
   fChain->SetBranchAddress("el_Es0", &el_Es0, &b_el_Es0);
   fChain->SetBranchAddress("el_etas0", &el_etas0, &b_el_etas0);
   fChain->SetBranchAddress("el_phis0", &el_phis0, &b_el_phis0);
   fChain->SetBranchAddress("el_Es1", &el_Es1, &b_el_Es1);
   fChain->SetBranchAddress("el_etas1", &el_etas1, &b_el_etas1);
   fChain->SetBranchAddress("el_phis1", &el_phis1, &b_el_phis1);
   fChain->SetBranchAddress("el_Es2", &el_Es2, &b_el_Es2);
   fChain->SetBranchAddress("el_etas2", &el_etas2, &b_el_etas2);
   fChain->SetBranchAddress("el_phis2", &el_phis2, &b_el_phis2);
   fChain->SetBranchAddress("el_Es3", &el_Es3, &b_el_Es3);
   fChain->SetBranchAddress("el_etas3", &el_etas3, &b_el_etas3);
   fChain->SetBranchAddress("el_phis3", &el_phis3, &b_el_phis3);
   fChain->SetBranchAddress("el_E_PreSamplerB", &el_E_PreSamplerB, &b_el_E_PreSamplerB);
   fChain->SetBranchAddress("el_E_EMB1", &el_E_EMB1, &b_el_E_EMB1);
   fChain->SetBranchAddress("el_E_EMB2", &el_E_EMB2, &b_el_E_EMB2);
   fChain->SetBranchAddress("el_E_EMB3", &el_E_EMB3, &b_el_E_EMB3);
   fChain->SetBranchAddress("el_E_PreSamplerE", &el_E_PreSamplerE, &b_el_E_PreSamplerE);
   fChain->SetBranchAddress("el_E_EME1", &el_E_EME1, &b_el_E_EME1);
   fChain->SetBranchAddress("el_E_EME2", &el_E_EME2, &b_el_E_EME2);
   fChain->SetBranchAddress("el_E_EME3", &el_E_EME3, &b_el_E_EME3);
   fChain->SetBranchAddress("el_E_HEC0", &el_E_HEC0, &b_el_E_HEC0);
   fChain->SetBranchAddress("el_E_HEC1", &el_E_HEC1, &b_el_E_HEC1);
   fChain->SetBranchAddress("el_E_HEC2", &el_E_HEC2, &b_el_E_HEC2);
   fChain->SetBranchAddress("el_E_HEC3", &el_E_HEC3, &b_el_E_HEC3);
   fChain->SetBranchAddress("el_E_TileBar0", &el_E_TileBar0, &b_el_E_TileBar0);
   fChain->SetBranchAddress("el_E_TileBar1", &el_E_TileBar1, &b_el_E_TileBar1);
   fChain->SetBranchAddress("el_E_TileBar2", &el_E_TileBar2, &b_el_E_TileBar2);
   fChain->SetBranchAddress("el_E_TileGap1", &el_E_TileGap1, &b_el_E_TileGap1);
   fChain->SetBranchAddress("el_E_TileGap2", &el_E_TileGap2, &b_el_E_TileGap2);
   fChain->SetBranchAddress("el_E_TileGap3", &el_E_TileGap3, &b_el_E_TileGap3);
   fChain->SetBranchAddress("el_E_TileExt0", &el_E_TileExt0, &b_el_E_TileExt0);
   fChain->SetBranchAddress("el_E_TileExt1", &el_E_TileExt1, &b_el_E_TileExt1);
   fChain->SetBranchAddress("el_E_TileExt2", &el_E_TileExt2, &b_el_E_TileExt2);
   fChain->SetBranchAddress("el_E_FCAL0", &el_E_FCAL0, &b_el_E_FCAL0);
   fChain->SetBranchAddress("el_E_FCAL1", &el_E_FCAL1, &b_el_E_FCAL1);
   fChain->SetBranchAddress("el_E_FCAL2", &el_E_FCAL2, &b_el_E_FCAL2);
   fChain->SetBranchAddress("el_cl_E", &el_cl_E, &b_el_cl_E);
   fChain->SetBranchAddress("el_cl_pt", &el_cl_pt, &b_el_cl_pt);
   fChain->SetBranchAddress("el_cl_eta", &el_cl_eta, &b_el_cl_eta);
   fChain->SetBranchAddress("el_cl_phi", &el_cl_phi, &b_el_cl_phi);
   fChain->SetBranchAddress("el_firstEdens", &el_firstEdens, &b_el_firstEdens);
   fChain->SetBranchAddress("el_cellmaxfrac", &el_cellmaxfrac, &b_el_cellmaxfrac);
   fChain->SetBranchAddress("el_longitudinal", &el_longitudinal, &b_el_longitudinal);
   fChain->SetBranchAddress("el_secondlambda", &el_secondlambda, &b_el_secondlambda);
   fChain->SetBranchAddress("el_lateral", &el_lateral, &b_el_lateral);
   fChain->SetBranchAddress("el_secondR", &el_secondR, &b_el_secondR);
   fChain->SetBranchAddress("el_centerlambda", &el_centerlambda, &b_el_centerlambda);
   fChain->SetBranchAddress("el_rawcl_Es0", &el_rawcl_Es0, &b_el_rawcl_Es0);
   fChain->SetBranchAddress("el_rawcl_etas0", &el_rawcl_etas0, &b_el_rawcl_etas0);
   fChain->SetBranchAddress("el_rawcl_phis0", &el_rawcl_phis0, &b_el_rawcl_phis0);
   fChain->SetBranchAddress("el_rawcl_Es1", &el_rawcl_Es1, &b_el_rawcl_Es1);
   fChain->SetBranchAddress("el_rawcl_etas1", &el_rawcl_etas1, &b_el_rawcl_etas1);
   fChain->SetBranchAddress("el_rawcl_phis1", &el_rawcl_phis1, &b_el_rawcl_phis1);
   fChain->SetBranchAddress("el_rawcl_Es2", &el_rawcl_Es2, &b_el_rawcl_Es2);
   fChain->SetBranchAddress("el_rawcl_etas2", &el_rawcl_etas2, &b_el_rawcl_etas2);
   fChain->SetBranchAddress("el_rawcl_phis2", &el_rawcl_phis2, &b_el_rawcl_phis2);
   fChain->SetBranchAddress("el_rawcl_Es3", &el_rawcl_Es3, &b_el_rawcl_Es3);
   fChain->SetBranchAddress("el_rawcl_etas3", &el_rawcl_etas3, &b_el_rawcl_etas3);
   fChain->SetBranchAddress("el_rawcl_phis3", &el_rawcl_phis3, &b_el_rawcl_phis3);
   fChain->SetBranchAddress("el_rawcl_E", &el_rawcl_E, &b_el_rawcl_E);
   fChain->SetBranchAddress("el_rawcl_pt", &el_rawcl_pt, &b_el_rawcl_pt);
   fChain->SetBranchAddress("el_rawcl_eta", &el_rawcl_eta, &b_el_rawcl_eta);
   fChain->SetBranchAddress("el_rawcl_phi", &el_rawcl_phi, &b_el_rawcl_phi);
   fChain->SetBranchAddress("el_refittedtrackcovd0", &el_refittedtrackcovd0, &b_el_refittedtrackcovd0);
   fChain->SetBranchAddress("el_refittedtrackcovz0", &el_refittedtrackcovz0, &b_el_refittedtrackcovz0);
   fChain->SetBranchAddress("el_refittedtrackcovphi", &el_refittedtrackcovphi, &b_el_refittedtrackcovphi);
   fChain->SetBranchAddress("el_refittedtrackcovtheta", &el_refittedtrackcovtheta, &b_el_refittedtrackcovtheta);
   fChain->SetBranchAddress("el_refittedtrackcovqoverp", &el_refittedtrackcovqoverp, &b_el_refittedtrackcovqoverp);
   fChain->SetBranchAddress("el_refittedtrackcovd0z0", &el_refittedtrackcovd0z0, &b_el_refittedtrackcovd0z0);
   fChain->SetBranchAddress("el_refittedtrackcovz0phi", &el_refittedtrackcovz0phi, &b_el_refittedtrackcovz0phi);
   fChain->SetBranchAddress("el_refittedtrackcovz0theta", &el_refittedtrackcovz0theta, &b_el_refittedtrackcovz0theta);
   fChain->SetBranchAddress("el_refittedtrackcovz0qoverp", &el_refittedtrackcovz0qoverp, &b_el_refittedtrackcovz0qoverp);
   fChain->SetBranchAddress("el_refittedtrackcovd0phi", &el_refittedtrackcovd0phi, &b_el_refittedtrackcovd0phi);
   fChain->SetBranchAddress("el_refittedtrackcovd0theta", &el_refittedtrackcovd0theta, &b_el_refittedtrackcovd0theta);
   fChain->SetBranchAddress("el_refittedtrackcovd0qoverp", &el_refittedtrackcovd0qoverp, &b_el_refittedtrackcovd0qoverp);
   fChain->SetBranchAddress("el_refittedtrackcovphitheta", &el_refittedtrackcovphitheta, &b_el_refittedtrackcovphitheta);
   fChain->SetBranchAddress("el_refittedtrackcovphiqoverp", &el_refittedtrackcovphiqoverp, &b_el_refittedtrackcovphiqoverp);
   fChain->SetBranchAddress("el_refittedtrackcovthetaqoverp", &el_refittedtrackcovthetaqoverp, &b_el_refittedtrackcovthetaqoverp);
   fChain->SetBranchAddress("el_trackd0", &el_trackd0, &b_el_trackd0);
   fChain->SetBranchAddress("el_trackz0", &el_trackz0, &b_el_trackz0);
   fChain->SetBranchAddress("el_trackphi", &el_trackphi, &b_el_trackphi);
   fChain->SetBranchAddress("el_tracktheta", &el_tracktheta, &b_el_tracktheta);
   fChain->SetBranchAddress("el_trackqoverp", &el_trackqoverp, &b_el_trackqoverp);
   fChain->SetBranchAddress("el_trackpt", &el_trackpt, &b_el_trackpt);
   fChain->SetBranchAddress("el_tracketa", &el_tracketa, &b_el_tracketa);
   fChain->SetBranchAddress("el_trackcov_d0", &el_trackcov_d0, &b_el_trackcov_d0);
   fChain->SetBranchAddress("el_trackcov_z0", &el_trackcov_z0, &b_el_trackcov_z0);
   fChain->SetBranchAddress("el_trackcov_phi", &el_trackcov_phi, &b_el_trackcov_phi);
   fChain->SetBranchAddress("el_trackcov_theta", &el_trackcov_theta, &b_el_trackcov_theta);
   fChain->SetBranchAddress("el_trackcov_qoverp", &el_trackcov_qoverp, &b_el_trackcov_qoverp);
   fChain->SetBranchAddress("el_trackcov_d0_z0", &el_trackcov_d0_z0, &b_el_trackcov_d0_z0);
   fChain->SetBranchAddress("el_trackcov_d0_phi", &el_trackcov_d0_phi, &b_el_trackcov_d0_phi);
   fChain->SetBranchAddress("el_trackcov_d0_theta", &el_trackcov_d0_theta, &b_el_trackcov_d0_theta);
   fChain->SetBranchAddress("el_trackcov_d0_qoverp", &el_trackcov_d0_qoverp, &b_el_trackcov_d0_qoverp);
   fChain->SetBranchAddress("el_trackcov_z0_phi", &el_trackcov_z0_phi, &b_el_trackcov_z0_phi);
   fChain->SetBranchAddress("el_trackcov_z0_theta", &el_trackcov_z0_theta, &b_el_trackcov_z0_theta);
   fChain->SetBranchAddress("el_trackcov_z0_qoverp", &el_trackcov_z0_qoverp, &b_el_trackcov_z0_qoverp);
   fChain->SetBranchAddress("el_trackcov_phi_theta", &el_trackcov_phi_theta, &b_el_trackcov_phi_theta);
   fChain->SetBranchAddress("el_trackcov_phi_qoverp", &el_trackcov_phi_qoverp, &b_el_trackcov_phi_qoverp);
   fChain->SetBranchAddress("el_trackcov_theta_qoverp", &el_trackcov_theta_qoverp, &b_el_trackcov_theta_qoverp);
   fChain->SetBranchAddress("el_trackfitchi2", &el_trackfitchi2, &b_el_trackfitchi2);
   fChain->SetBranchAddress("el_trackfitndof", &el_trackfitndof, &b_el_trackfitndof);
   fChain->SetBranchAddress("el_nBLHits", &el_nBLHits, &b_el_nBLHits);
   fChain->SetBranchAddress("el_nPixHits", &el_nPixHits, &b_el_nPixHits);
   fChain->SetBranchAddress("el_nSCTHits", &el_nSCTHits, &b_el_nSCTHits);
   fChain->SetBranchAddress("el_nTRTHits", &el_nTRTHits, &b_el_nTRTHits);
   fChain->SetBranchAddress("el_nPixHoles", &el_nPixHoles, &b_el_nPixHoles);
   fChain->SetBranchAddress("el_nSCTHoles", &el_nSCTHoles, &b_el_nSCTHoles);
   fChain->SetBranchAddress("el_nBLSharedHits", &el_nBLSharedHits, &b_el_nBLSharedHits);
   fChain->SetBranchAddress("el_nPixSharedHits", &el_nPixSharedHits, &b_el_nPixSharedHits);
   fChain->SetBranchAddress("el_nSCTSharedHits", &el_nSCTSharedHits, &b_el_nSCTSharedHits);
   fChain->SetBranchAddress("el_nTRTHighTHits", &el_nTRTHighTHits, &b_el_nTRTHighTHits);
   fChain->SetBranchAddress("el_nTRTOutliers", &el_nTRTOutliers, &b_el_nTRTOutliers);
   fChain->SetBranchAddress("el_nTRTHighTOutliers", &el_nTRTHighTOutliers, &b_el_nTRTHighTOutliers);
   fChain->SetBranchAddress("el_nSiHits", &el_nSiHits, &b_el_nSiHits);
   fChain->SetBranchAddress("el_TRTHighTHitsRatio", &el_TRTHighTHitsRatio, &b_el_TRTHighTHitsRatio);
   fChain->SetBranchAddress("el_pixeldEdx", &el_pixeldEdx, &b_el_pixeldEdx);
   fChain->SetBranchAddress("el_eProbabilityComb", &el_eProbabilityComb, &b_el_eProbabilityComb);
   fChain->SetBranchAddress("el_eProbabilityHT", &el_eProbabilityHT, &b_el_eProbabilityHT);
   fChain->SetBranchAddress("el_eProbabilityToT", &el_eProbabilityToT, &b_el_eProbabilityToT);
   fChain->SetBranchAddress("el_eProbabilityBrem", &el_eProbabilityBrem, &b_el_eProbabilityBrem);
   fChain->SetBranchAddress("el_vertx", &el_vertx, &b_el_vertx);
   fChain->SetBranchAddress("el_verty", &el_verty, &b_el_verty);
   fChain->SetBranchAddress("el_vertz", &el_vertz, &b_el_vertz);
   fChain->SetBranchAddress("el_hastrack", &el_hastrack, &b_el_hastrack);
   fChain->SetBranchAddress("el_deltaEmax2", &el_deltaEmax2, &b_el_deltaEmax2);
   fChain->SetBranchAddress("el_calibHitsShowerDepth", &el_calibHitsShowerDepth, &b_el_calibHitsShowerDepth);
   fChain->SetBranchAddress("el_trackd0beam", &el_trackd0beam, &b_el_trackd0beam);
   fChain->SetBranchAddress("el_tracksigd0beam", &el_tracksigd0beam, &b_el_tracksigd0beam);
   fChain->SetBranchAddress("el_trackd0pv", &el_trackd0pv, &b_el_trackd0pv);
   fChain->SetBranchAddress("el_tracksigd0pv", &el_tracksigd0pv, &b_el_tracksigd0pv);
   fChain->SetBranchAddress("el_trackz0pv", &el_trackz0pv, &b_el_trackz0pv);
   fChain->SetBranchAddress("el_tracksigz0pv", &el_tracksigz0pv, &b_el_tracksigz0pv);
   fChain->SetBranchAddress("el_trackd0pvunbiased", &el_trackd0pvunbiased, &b_el_trackd0pvunbiased);
   fChain->SetBranchAddress("el_tracksigd0pvunbiased", &el_tracksigd0pvunbiased, &b_el_tracksigd0pvunbiased);
   fChain->SetBranchAddress("el_trackz0pvunbiased", &el_trackz0pvunbiased, &b_el_trackz0pvunbiased);
   fChain->SetBranchAddress("el_tracksigz0pvunbiased", &el_tracksigz0pvunbiased, &b_el_tracksigz0pvunbiased);
   fChain->SetBranchAddress("el_isIso", &el_isIso, &b_el_isIso);
   fChain->SetBranchAddress("el_mvaptcone20", &el_mvaptcone20, &b_el_mvaptcone20);
   fChain->SetBranchAddress("el_mvaptcone30", &el_mvaptcone30, &b_el_mvaptcone30);
   fChain->SetBranchAddress("el_mvaptcone40", &el_mvaptcone40, &b_el_mvaptcone40);
   fChain->SetBranchAddress("el_jet_dr", &el_jet_dr, &b_el_jet_dr);
   fChain->SetBranchAddress("el_jet_E", &el_jet_E, &b_el_jet_E);
   fChain->SetBranchAddress("el_jet_pt", &el_jet_pt, &b_el_jet_pt);
   fChain->SetBranchAddress("el_jet_m", &el_jet_m, &b_el_jet_m);
   fChain->SetBranchAddress("el_jet_eta", &el_jet_eta, &b_el_jet_eta);
   fChain->SetBranchAddress("el_jet_phi", &el_jet_phi, &b_el_jet_phi);
   fChain->SetBranchAddress("el_jet_truth_dr", &el_jet_truth_dr, &b_el_jet_truth_dr);
   fChain->SetBranchAddress("el_jet_truth_E", &el_jet_truth_E, &b_el_jet_truth_E);
   fChain->SetBranchAddress("el_jet_truth_pt", &el_jet_truth_pt, &b_el_jet_truth_pt);
   fChain->SetBranchAddress("el_jet_truth_m", &el_jet_truth_m, &b_el_jet_truth_m);
   fChain->SetBranchAddress("el_jet_truth_eta", &el_jet_truth_eta, &b_el_jet_truth_eta);
   fChain->SetBranchAddress("el_jet_truth_phi", &el_jet_truth_phi, &b_el_jet_truth_phi);
   fChain->SetBranchAddress("el_jet_truth_matched", &el_jet_truth_matched, &b_el_jet_truth_matched);
   fChain->SetBranchAddress("el_jet_matched", &el_jet_matched, &b_el_jet_matched);
   fChain->SetBranchAddress("el_Etcone40_pt_corrected", &el_Etcone40_pt_corrected, &b_el_Etcone40_pt_corrected);
   fChain->SetBranchAddress("el_Etcone40_ED_corrected", &el_Etcone40_ED_corrected, &b_el_Etcone40_ED_corrected);
   fChain->SetBranchAddress("el_Etcone40_corrected", &el_Etcone40_corrected, &b_el_Etcone40_corrected);
   fChain->SetBranchAddress("el_EF_dr", &el_EF_dr, &b_el_EF_dr);
   fChain->SetBranchAddress("el_EF_index", &el_EF_index, &b_el_EF_index);
   fChain->SetBranchAddress("el_L2_dr", &el_L2_dr, &b_el_L2_dr);
   fChain->SetBranchAddress("el_L2_index", &el_L2_index, &b_el_L2_index);
   fChain->SetBranchAddress("el_L1_dr", &el_L1_dr, &b_el_L1_dr);
   fChain->SetBranchAddress("el_L1_index", &el_L1_index, &b_el_L1_index);
   fChain->SetBranchAddress("EF_2e12_tight", &EF_2e12_tight, &b_EF_2e12_tight);
   fChain->SetBranchAddress("EF_2e15_medium", &EF_2e15_medium, &b_EF_2e15_medium);
   fChain->SetBranchAddress("EF_2e20_loose", &EF_2e20_loose, &b_EF_2e20_loose);
   fChain->SetBranchAddress("EF_2e5_medium", &EF_2e5_medium, &b_EF_2e5_medium);
   fChain->SetBranchAddress("EF_2e5_medium1", &EF_2e5_medium1, &b_EF_2e5_medium1);
   fChain->SetBranchAddress("EF_2g10_loose", &EF_2g10_loose, &b_EF_2g10_loose);
   fChain->SetBranchAddress("EF_2g10_loose_mu6", &EF_2g10_loose_mu6, &b_EF_2g10_loose_mu6);
   fChain->SetBranchAddress("EF_2g17i_tight", &EF_2g17i_tight, &b_EF_2g17i_tight);
   fChain->SetBranchAddress("EF_2g20_loose", &EF_2g20_loose, &b_EF_2g20_loose);
   fChain->SetBranchAddress("EF_2mu10", &EF_2mu10, &b_EF_2mu10);
   fChain->SetBranchAddress("EF_2mu20", &EF_2mu20, &b_EF_2mu20);
   fChain->SetBranchAddress("EF_2mu4", &EF_2mu4, &b_EF_2mu4);
   fChain->SetBranchAddress("EF_2mu4_Bmumu", &EF_2mu4_Bmumu, &b_EF_2mu4_Bmumu);
   fChain->SetBranchAddress("EF_2mu4_Bmumux", &EF_2mu4_Bmumux, &b_EF_2mu4_Bmumux);
   fChain->SetBranchAddress("EF_2mu4_DiMu", &EF_2mu4_DiMu, &b_EF_2mu4_DiMu);
   fChain->SetBranchAddress("EF_2mu4_DiMu_SiTrk", &EF_2mu4_DiMu_SiTrk, &b_EF_2mu4_DiMu_SiTrk);
   fChain->SetBranchAddress("EF_2mu4_DiMu_noVtx", &EF_2mu4_DiMu_noVtx, &b_EF_2mu4_DiMu_noVtx);
   fChain->SetBranchAddress("EF_2mu4_DiMu_noVtx_noOS", &EF_2mu4_DiMu_noVtx_noOS, &b_EF_2mu4_DiMu_noVtx_noOS);
   fChain->SetBranchAddress("EF_2mu4_Jpsimumu", &EF_2mu4_Jpsimumu, &b_EF_2mu4_Jpsimumu);
   fChain->SetBranchAddress("EF_2mu4_Upsimumu", &EF_2mu4_Upsimumu, &b_EF_2mu4_Upsimumu);
   fChain->SetBranchAddress("EF_2mu6", &EF_2mu6, &b_EF_2mu6);
   fChain->SetBranchAddress("EF_2mu6_Bmumu", &EF_2mu6_Bmumu, &b_EF_2mu6_Bmumu);
   fChain->SetBranchAddress("EF_2mu6_Bmumux", &EF_2mu6_Bmumux, &b_EF_2mu6_Bmumux);
   fChain->SetBranchAddress("EF_2mu6_DiMu", &EF_2mu6_DiMu, &b_EF_2mu6_DiMu);
   fChain->SetBranchAddress("EF_2mu6_Jpsimumu", &EF_2mu6_Jpsimumu, &b_EF_2mu6_Jpsimumu);
   fChain->SetBranchAddress("EF_2mu6_Upsimumu", &EF_2mu6_Upsimumu, &b_EF_2mu6_Upsimumu);
   fChain->SetBranchAddress("EF_2mu6_e10_loose", &EF_2mu6_e10_loose, &b_EF_2mu6_e10_loose);
   fChain->SetBranchAddress("EF_2mu6_g10_loose", &EF_2mu6_g10_loose, &b_EF_2mu6_g10_loose);
   fChain->SetBranchAddress("EF_e105_loose1", &EF_e105_loose1, &b_EF_e105_loose1);
   fChain->SetBranchAddress("EF_e10_NoCut", &EF_e10_NoCut, &b_EF_e10_NoCut);
   fChain->SetBranchAddress("EF_e10_loose", &EF_e10_loose, &b_EF_e10_loose);
   fChain->SetBranchAddress("EF_e10_loose_IdScan", &EF_e10_loose_IdScan, &b_EF_e10_loose_IdScan);
   fChain->SetBranchAddress("EF_e10_loose_SiTrk", &EF_e10_loose_SiTrk, &b_EF_e10_loose_SiTrk);
   fChain->SetBranchAddress("EF_e10_loose_TRT", &EF_e10_loose_TRT, &b_EF_e10_loose_TRT);
   fChain->SetBranchAddress("EF_e10_loose_mu6", &EF_e10_loose_mu6, &b_EF_e10_loose_mu6);
   fChain->SetBranchAddress("EF_e10_medium", &EF_e10_medium, &b_EF_e10_medium);
   fChain->SetBranchAddress("EF_e10_medium_SiTrk", &EF_e10_medium_SiTrk, &b_EF_e10_medium_SiTrk);
   fChain->SetBranchAddress("EF_e10_medium_SiTrk_robust", &EF_e10_medium_SiTrk_robust, &b_EF_e10_medium_SiTrk_robust);
   fChain->SetBranchAddress("EF_e12_medium", &EF_e12_medium, &b_EF_e12_medium);
   fChain->SetBranchAddress("EF_e15_medium", &EF_e15_medium, &b_EF_e15_medium);
   fChain->SetBranchAddress("EF_e15i_medium", &EF_e15i_medium, &b_EF_e15i_medium);
   fChain->SetBranchAddress("EF_e20_loose", &EF_e20_loose, &b_EF_e20_loose);
   fChain->SetBranchAddress("EF_e20_loose_IdScan", &EF_e20_loose_IdScan, &b_EF_e20_loose_IdScan);
   fChain->SetBranchAddress("EF_e20_loose_NoIDTrkCut", &EF_e20_loose_NoIDTrkCut, &b_EF_e20_loose_NoIDTrkCut);
   fChain->SetBranchAddress("EF_e20_loose_SiTrk", &EF_e20_loose_SiTrk, &b_EF_e20_loose_SiTrk);
   fChain->SetBranchAddress("EF_e20_loose_SiTrk_robust", &EF_e20_loose_SiTrk_robust, &b_EF_e20_loose_SiTrk_robust);
   fChain->SetBranchAddress("EF_e20_loose_TRT", &EF_e20_loose_TRT, &b_EF_e20_loose_TRT);
   fChain->SetBranchAddress("EF_e20_loose_passEF", &EF_e20_loose_passEF, &b_EF_e20_loose_passEF);
   fChain->SetBranchAddress("EF_e20_loose_passL2", &EF_e20_loose_passL2, &b_EF_e20_loose_passL2);
   fChain->SetBranchAddress("EF_e20_loose_xe20", &EF_e20_loose_xe20, &b_EF_e20_loose_xe20);
   fChain->SetBranchAddress("EF_e20_loose_xe30", &EF_e20_loose_xe30, &b_EF_e20_loose_xe30);
   fChain->SetBranchAddress("EF_e20_medium", &EF_e20_medium, &b_EF_e20_medium);
   fChain->SetBranchAddress("EF_e20i_loose", &EF_e20i_loose, &b_EF_e20i_loose);
   fChain->SetBranchAddress("EF_e22i_tight", &EF_e22i_tight, &b_EF_e22i_tight);
   fChain->SetBranchAddress("EF_e25_loose", &EF_e25_loose, &b_EF_e25_loose);
   fChain->SetBranchAddress("EF_e25_medium", &EF_e25_medium, &b_EF_e25_medium);
   fChain->SetBranchAddress("EF_e25i_loose", &EF_e25i_loose, &b_EF_e25i_loose);
   fChain->SetBranchAddress("EF_e25i_medium", &EF_e25i_medium, &b_EF_e25i_medium);
   fChain->SetBranchAddress("EF_e55_loose1", &EF_e55_loose1, &b_EF_e55_loose1);
   fChain->SetBranchAddress("EF_e5_medium", &EF_e5_medium, &b_EF_e5_medium);
   fChain->SetBranchAddress("EF_e5_medium1", &EF_e5_medium1, &b_EF_e5_medium1);
   fChain->SetBranchAddress("EF_e5_medium_L2SW", &EF_e5_medium_L2SW, &b_EF_e5_medium_L2SW);
   fChain->SetBranchAddress("EF_e5_medium_MV", &EF_e5_medium_MV, &b_EF_e5_medium_MV);
   fChain->SetBranchAddress("EF_e5_medium_Ringer", &EF_e5_medium_Ringer, &b_EF_e5_medium_Ringer);
   fChain->SetBranchAddress("EF_e5_medium_e10_loose", &EF_e5_medium_e10_loose, &b_EF_e5_medium_e10_loose);
   fChain->SetBranchAddress("EF_em105_passHLT", &EF_em105_passHLT, &b_EF_em105_passHLT);
   fChain->SetBranchAddress("EF_em15_passHLT", &EF_em15_passHLT, &b_EF_em15_passHLT);
   fChain->SetBranchAddress("EF_em15i_passHLT", &EF_em15i_passHLT, &b_EF_em15i_passHLT);
   fChain->SetBranchAddress("EF_em20_passHLT", &EF_em20_passHLT, &b_EF_em20_passHLT);
   fChain->SetBranchAddress("EF_em20i_passHLT", &EF_em20i_passHLT, &b_EF_em20i_passHLT);
   fChain->SetBranchAddress("EF_g105", &EF_g105, &b_EF_g105);
   fChain->SetBranchAddress("EF_g10_loose", &EF_g10_loose, &b_EF_g10_loose);
   fChain->SetBranchAddress("EF_g10_loose_larcalib", &EF_g10_loose_larcalib, &b_EF_g10_loose_larcalib);
   fChain->SetBranchAddress("EF_g10_nocut", &EF_g10_nocut, &b_EF_g10_nocut);
   fChain->SetBranchAddress("EF_g150", &EF_g150, &b_EF_g150);
   fChain->SetBranchAddress("EF_g15_loose", &EF_g15_loose, &b_EF_g15_loose);
   fChain->SetBranchAddress("EF_g20_loose", &EF_g20_loose, &b_EF_g20_loose);
   fChain->SetBranchAddress("EF_g20_loose_cnv", &EF_g20_loose_cnv, &b_EF_g20_loose_cnv);
   fChain->SetBranchAddress("EF_g20_loose_larcalib", &EF_g20_loose_larcalib, &b_EF_g20_loose_larcalib);
   fChain->SetBranchAddress("EF_g20_loose_xe20", &EF_g20_loose_xe20, &b_EF_g20_loose_xe20);
   fChain->SetBranchAddress("EF_g20i_loose", &EF_g20i_loose, &b_EF_g20i_loose);
   fChain->SetBranchAddress("EF_g20i_loose_larcalib", &EF_g20i_loose_larcalib, &b_EF_g20i_loose_larcalib);
   fChain->SetBranchAddress("EF_g25_loose", &EF_g25_loose, &b_EF_g25_loose);
   fChain->SetBranchAddress("EF_g25_loose_xe30", &EF_g25_loose_xe30, &b_EF_g25_loose_xe30);
   fChain->SetBranchAddress("EF_g25i_loose", &EF_g25i_loose, &b_EF_g25i_loose);
   fChain->SetBranchAddress("EF_g55_tight", &EF_g55_tight, &b_EF_g55_tight);
   fChain->SetBranchAddress("EF_mu10", &EF_mu10, &b_EF_mu10);
   fChain->SetBranchAddress("EF_mu10_DsPhiPi", &EF_mu10_DsPhiPi, &b_EF_mu10_DsPhiPi);
   fChain->SetBranchAddress("EF_mu10_DsPhiPi_FS", &EF_mu10_DsPhiPi_FS, &b_EF_mu10_DsPhiPi_FS);
   fChain->SetBranchAddress("EF_mu10_MG", &EF_mu10_MG, &b_EF_mu10_MG);
   fChain->SetBranchAddress("EF_mu10_MSonly", &EF_mu10_MSonly, &b_EF_mu10_MSonly);
   fChain->SetBranchAddress("EF_mu10_MuTagIMO", &EF_mu10_MuTagIMO, &b_EF_mu10_MuTagIMO);
   fChain->SetBranchAddress("EF_mu10_NoIDTrkCut", &EF_mu10_NoIDTrkCut, &b_EF_mu10_NoIDTrkCut);
   fChain->SetBranchAddress("EF_mu10_SiTrk", &EF_mu10_SiTrk, &b_EF_mu10_SiTrk);
   fChain->SetBranchAddress("EF_mu10i_loose", &EF_mu10i_loose, &b_EF_mu10i_loose);
   fChain->SetBranchAddress("EF_mu13", &EF_mu13, &b_EF_mu13);
   fChain->SetBranchAddress("EF_mu15", &EF_mu15, &b_EF_mu15);
   fChain->SetBranchAddress("EF_mu15_xe20", &EF_mu15_xe20, &b_EF_mu15_xe20);
   fChain->SetBranchAddress("EF_mu15i_loose", &EF_mu15i_loose, &b_EF_mu15i_loose);
   fChain->SetBranchAddress("EF_mu20", &EF_mu20, &b_EF_mu20);
   fChain->SetBranchAddress("EF_mu20_MSonly", &EF_mu20_MSonly, &b_EF_mu20_MSonly);
   fChain->SetBranchAddress("EF_mu20_passHLT", &EF_mu20_passHLT, &b_EF_mu20_passHLT);
   fChain->SetBranchAddress("EF_mu20_slow", &EF_mu20_slow, &b_EF_mu20_slow);
   fChain->SetBranchAddress("EF_mu20_xe30", &EF_mu20_xe30, &b_EF_mu20_xe30);
   fChain->SetBranchAddress("EF_mu20i_loose", &EF_mu20i_loose, &b_EF_mu20i_loose);
   fChain->SetBranchAddress("EF_mu20i_loose_MSonly", &EF_mu20i_loose_MSonly, &b_EF_mu20i_loose_MSonly);
   fChain->SetBranchAddress("EF_mu4", &EF_mu4, &b_EF_mu4);
   fChain->SetBranchAddress("EF_mu4_Bmumu", &EF_mu4_Bmumu, &b_EF_mu4_Bmumu);
   fChain->SetBranchAddress("EF_mu4_BmumuX", &EF_mu4_BmumuX, &b_EF_mu4_BmumuX);
   fChain->SetBranchAddress("EF_mu4_Bmumu_FS", &EF_mu4_Bmumu_FS, &b_EF_mu4_Bmumu_FS);
   fChain->SetBranchAddress("EF_mu4_DiMu", &EF_mu4_DiMu, &b_EF_mu4_DiMu);
   fChain->SetBranchAddress("EF_mu4_DiMu_FS", &EF_mu4_DiMu_FS, &b_EF_mu4_DiMu_FS);
   fChain->SetBranchAddress("EF_mu4_DiMu_FS_noOS", &EF_mu4_DiMu_FS_noOS, &b_EF_mu4_DiMu_FS_noOS);
   fChain->SetBranchAddress("EF_mu4_DiMu_MG", &EF_mu4_DiMu_MG, &b_EF_mu4_DiMu_MG);
   fChain->SetBranchAddress("EF_mu4_DiMu_MG_FS", &EF_mu4_DiMu_MG_FS, &b_EF_mu4_DiMu_MG_FS);
   fChain->SetBranchAddress("EF_mu4_DiMu_SiTrk", &EF_mu4_DiMu_SiTrk, &b_EF_mu4_DiMu_SiTrk);
   fChain->SetBranchAddress("EF_mu4_DiMu_SiTrk_FS", &EF_mu4_DiMu_SiTrk_FS, &b_EF_mu4_DiMu_SiTrk_FS);
   fChain->SetBranchAddress("EF_mu4_DiMu_noOS", &EF_mu4_DiMu_noOS, &b_EF_mu4_DiMu_noOS);
   fChain->SetBranchAddress("EF_mu4_DsPhiPi", &EF_mu4_DsPhiPi, &b_EF_mu4_DsPhiPi);
   fChain->SetBranchAddress("EF_mu4_DsPhiPi_FS", &EF_mu4_DsPhiPi_FS, &b_EF_mu4_DsPhiPi_FS);
   fChain->SetBranchAddress("EF_mu4_Jpsie5e3", &EF_mu4_Jpsie5e3, &b_EF_mu4_Jpsie5e3);
   fChain->SetBranchAddress("EF_mu4_Jpsie5e3_FS", &EF_mu4_Jpsie5e3_FS, &b_EF_mu4_Jpsie5e3_FS);
   fChain->SetBranchAddress("EF_mu4_Jpsie5e3_SiTrk", &EF_mu4_Jpsie5e3_SiTrk, &b_EF_mu4_Jpsie5e3_SiTrk);
   fChain->SetBranchAddress("EF_mu4_Jpsie5e3_SiTrk_FS", &EF_mu4_Jpsie5e3_SiTrk_FS, &b_EF_mu4_Jpsie5e3_SiTrk_FS);
   fChain->SetBranchAddress("EF_mu4_Jpsimumu", &EF_mu4_Jpsimumu, &b_EF_mu4_Jpsimumu);
   fChain->SetBranchAddress("EF_mu4_Jpsimumu_FS", &EF_mu4_Jpsimumu_FS, &b_EF_mu4_Jpsimumu_FS);
   fChain->SetBranchAddress("EF_mu4_MG", &EF_mu4_MG, &b_EF_mu4_MG);
   fChain->SetBranchAddress("EF_mu4_MSonly", &EF_mu4_MSonly, &b_EF_mu4_MSonly);
   fChain->SetBranchAddress("EF_mu4_MV", &EF_mu4_MV, &b_EF_mu4_MV);
   fChain->SetBranchAddress("EF_mu4_SiTrk", &EF_mu4_SiTrk, &b_EF_mu4_SiTrk);
   fChain->SetBranchAddress("EF_mu4_Trk_Jpsi", &EF_mu4_Trk_Jpsi, &b_EF_mu4_Trk_Jpsi);
   fChain->SetBranchAddress("EF_mu4_Trk_Jpsi_FS", &EF_mu4_Trk_Jpsi_FS, &b_EF_mu4_Trk_Jpsi_FS);
   fChain->SetBranchAddress("EF_mu4_Trk_Jpsi_loose", &EF_mu4_Trk_Jpsi_loose, &b_EF_mu4_Trk_Jpsi_loose);
   fChain->SetBranchAddress("EF_mu4_Trk_Upsi_FS", &EF_mu4_Trk_Upsi_FS, &b_EF_mu4_Trk_Upsi_FS);
   fChain->SetBranchAddress("EF_mu4_Trk_Upsi_loose_FS", &EF_mu4_Trk_Upsi_loose_FS, &b_EF_mu4_Trk_Upsi_loose_FS);
   fChain->SetBranchAddress("EF_mu4_Upsimumu", &EF_mu4_Upsimumu, &b_EF_mu4_Upsimumu);
   fChain->SetBranchAddress("EF_mu4_Upsimumu_FS", &EF_mu4_Upsimumu_FS, &b_EF_mu4_Upsimumu_FS);
   fChain->SetBranchAddress("EF_mu4_j10", &EF_mu4_j10, &b_EF_mu4_j10);
   fChain->SetBranchAddress("EF_mu4_j10_matched", &EF_mu4_j10_matched, &b_EF_mu4_j10_matched);
   fChain->SetBranchAddress("EF_mu4_j20", &EF_mu4_j20, &b_EF_mu4_j20);
   fChain->SetBranchAddress("EF_mu4_j20_matched", &EF_mu4_j20_matched, &b_EF_mu4_j20_matched);
   fChain->SetBranchAddress("EF_mu4_j40_matched", &EF_mu4_j40_matched, &b_EF_mu4_j40_matched);
   fChain->SetBranchAddress("EF_mu4_j70_matched", &EF_mu4_j70_matched, &b_EF_mu4_j70_matched);
   fChain->SetBranchAddress("EF_mu4_mu6", &EF_mu4_mu6, &b_EF_mu4_mu6);
   fChain->SetBranchAddress("EF_mu4_tile", &EF_mu4_tile, &b_EF_mu4_tile);
   fChain->SetBranchAddress("EF_mu4_trod", &EF_mu4_trod, &b_EF_mu4_trod);
   fChain->SetBranchAddress("EF_mu4mu6_Bmumu", &EF_mu4mu6_Bmumu, &b_EF_mu4mu6_Bmumu);
   fChain->SetBranchAddress("EF_mu4mu6_DiMu", &EF_mu4mu6_DiMu, &b_EF_mu4mu6_DiMu);
   fChain->SetBranchAddress("EF_mu4mu6_Jpsimumu", &EF_mu4mu6_Jpsimumu, &b_EF_mu4mu6_Jpsimumu);
   fChain->SetBranchAddress("EF_mu4mu6_Upsimumu", &EF_mu4mu6_Upsimumu, &b_EF_mu4mu6_Upsimumu);
   fChain->SetBranchAddress("EF_mu6", &EF_mu6, &b_EF_mu6);
   fChain->SetBranchAddress("EF_mu6_Bmumu", &EF_mu6_Bmumu, &b_EF_mu6_Bmumu);
   fChain->SetBranchAddress("EF_mu6_BmumuX", &EF_mu6_BmumuX, &b_EF_mu6_BmumuX);
   fChain->SetBranchAddress("EF_mu6_Bmumu_FS", &EF_mu6_Bmumu_FS, &b_EF_mu6_Bmumu_FS);
   fChain->SetBranchAddress("EF_mu6_DiMu", &EF_mu6_DiMu, &b_EF_mu6_DiMu);
   fChain->SetBranchAddress("EF_mu6_DiMu_FS", &EF_mu6_DiMu_FS, &b_EF_mu6_DiMu_FS);
   fChain->SetBranchAddress("EF_mu6_DsPhiPi", &EF_mu6_DsPhiPi, &b_EF_mu6_DsPhiPi);
   fChain->SetBranchAddress("EF_mu6_DsPhiPi_FS", &EF_mu6_DsPhiPi_FS, &b_EF_mu6_DsPhiPi_FS);
   fChain->SetBranchAddress("EF_mu6_Jpsie5e3", &EF_mu6_Jpsie5e3, &b_EF_mu6_Jpsie5e3);
   fChain->SetBranchAddress("EF_mu6_Jpsie5e3_FS", &EF_mu6_Jpsie5e3_FS, &b_EF_mu6_Jpsie5e3_FS);
   fChain->SetBranchAddress("EF_mu6_Jpsie5e3_SiTrk", &EF_mu6_Jpsie5e3_SiTrk, &b_EF_mu6_Jpsie5e3_SiTrk);
   fChain->SetBranchAddress("EF_mu6_Jpsie5e3_SiTrk_FS", &EF_mu6_Jpsie5e3_SiTrk_FS, &b_EF_mu6_Jpsie5e3_SiTrk_FS);
   fChain->SetBranchAddress("EF_mu6_Jpsimumu", &EF_mu6_Jpsimumu, &b_EF_mu6_Jpsimumu);
   fChain->SetBranchAddress("EF_mu6_Jpsimumu_FS", &EF_mu6_Jpsimumu_FS, &b_EF_mu6_Jpsimumu_FS);
   fChain->SetBranchAddress("EF_mu6_MG", &EF_mu6_MG, &b_EF_mu6_MG);
   fChain->SetBranchAddress("EF_mu6_MSonly", &EF_mu6_MSonly, &b_EF_mu6_MSonly);
   fChain->SetBranchAddress("EF_mu6_Trk_Jpsi", &EF_mu6_Trk_Jpsi, &b_EF_mu6_Trk_Jpsi);
   fChain->SetBranchAddress("EF_mu6_Upsimumu_FS", &EF_mu6_Upsimumu_FS, &b_EF_mu6_Upsimumu_FS);
   fChain->SetBranchAddress("L1_2EM10", &L1_2EM10, &b_L1_2EM10);
   fChain->SetBranchAddress("L1_2EM10I", &L1_2EM10I, &b_L1_2EM10I);
   fChain->SetBranchAddress("L1_2EM14", &L1_2EM14, &b_L1_2EM14);
   fChain->SetBranchAddress("L1_2EM3", &L1_2EM3, &b_L1_2EM3);
   fChain->SetBranchAddress("L1_2EM3_EM5", &L1_2EM3_EM5, &b_L1_2EM3_EM5);
   fChain->SetBranchAddress("L1_2EM5", &L1_2EM5, &b_L1_2EM5);
   fChain->SetBranchAddress("L1_2EM5_MU6", &L1_2EM5_MU6, &b_L1_2EM5_MU6);
   fChain->SetBranchAddress("L1_2MU0", &L1_2MU0, &b_L1_2MU0);
   fChain->SetBranchAddress("L1_2MU0_MU6", &L1_2MU0_MU6, &b_L1_2MU0_MU6);
   fChain->SetBranchAddress("L1_2MU10", &L1_2MU10, &b_L1_2MU10);
   fChain->SetBranchAddress("L1_2MU20", &L1_2MU20, &b_L1_2MU20);
   fChain->SetBranchAddress("L1_2MU6", &L1_2MU6, &b_L1_2MU6);
   fChain->SetBranchAddress("L1_2MU6_EM5", &L1_2MU6_EM5, &b_L1_2MU6_EM5);
   fChain->SetBranchAddress("L1_EM10", &L1_EM10, &b_L1_EM10);
   fChain->SetBranchAddress("L1_EM10I", &L1_EM10I, &b_L1_EM10I);
   fChain->SetBranchAddress("L1_EM14", &L1_EM14, &b_L1_EM14);
   fChain->SetBranchAddress("L1_EM14I", &L1_EM14I, &b_L1_EM14I);
   fChain->SetBranchAddress("L1_EM14_XE10", &L1_EM14_XE10, &b_L1_EM14_XE10);
   fChain->SetBranchAddress("L1_EM14_XE15", &L1_EM14_XE15, &b_L1_EM14_XE15);
   fChain->SetBranchAddress("L1_EM19I", &L1_EM19I, &b_L1_EM19I);
   fChain->SetBranchAddress("L1_EM3", &L1_EM3, &b_L1_EM3);
   fChain->SetBranchAddress("L1_EM3_MV", &L1_EM3_MV, &b_L1_EM3_MV);
   fChain->SetBranchAddress("L1_EM5", &L1_EM5, &b_L1_EM5);
   fChain->SetBranchAddress("L1_EM5_MU6", &L1_EM5_MU6, &b_L1_EM5_MU6);
   fChain->SetBranchAddress("L1_EM85", &L1_EM85, &b_L1_EM85);
   fChain->SetBranchAddress("L1_MU0", &L1_MU0, &b_L1_MU0);
   fChain->SetBranchAddress("L1_MU0_COMM", &L1_MU0_COMM, &b_L1_MU0_COMM);
   fChain->SetBranchAddress("L1_MU0_EM3", &L1_MU0_EM3, &b_L1_MU0_EM3);
   fChain->SetBranchAddress("L1_MU0_J10", &L1_MU0_J10, &b_L1_MU0_J10);
   fChain->SetBranchAddress("L1_MU0_J15", &L1_MU0_J15, &b_L1_MU0_J15);
   fChain->SetBranchAddress("L1_MU0_J30", &L1_MU0_J30, &b_L1_MU0_J30);
   fChain->SetBranchAddress("L1_MU0_J5", &L1_MU0_J5, &b_L1_MU0_J5);
   fChain->SetBranchAddress("L1_MU0_J55", &L1_MU0_J55, &b_L1_MU0_J55);
   fChain->SetBranchAddress("L1_MU0_MV", &L1_MU0_MV, &b_L1_MU0_MV);
   fChain->SetBranchAddress("L1_MU10", &L1_MU10, &b_L1_MU10);
   fChain->SetBranchAddress("L1_MU10_J5", &L1_MU10_J5, &b_L1_MU10_J5);
   fChain->SetBranchAddress("L1_MU15", &L1_MU15, &b_L1_MU15);
   fChain->SetBranchAddress("L1_MU15_XE10", &L1_MU15_XE10, &b_L1_MU15_XE10);
   fChain->SetBranchAddress("L1_MU20", &L1_MU20, &b_L1_MU20);
   fChain->SetBranchAddress("L1_MU20_XE15", &L1_MU20_XE15, &b_L1_MU20_XE15);
   fChain->SetBranchAddress("L1_MU6", &L1_MU6, &b_L1_MU6);
   fChain->SetBranchAddress("L1_MU6_EM3", &L1_MU6_EM3, &b_L1_MU6_EM3);
   fChain->SetBranchAddress("L1_MU6_J5", &L1_MU6_J5, &b_L1_MU6_J5);
   fChain->SetBranchAddress("L2_2e12_tight", &L2_2e12_tight, &b_L2_2e12_tight);
   fChain->SetBranchAddress("L2_2e15_medium", &L2_2e15_medium, &b_L2_2e15_medium);
   fChain->SetBranchAddress("L2_2e20_loose", &L2_2e20_loose, &b_L2_2e20_loose);
   fChain->SetBranchAddress("L2_2e5_medium", &L2_2e5_medium, &b_L2_2e5_medium);
   fChain->SetBranchAddress("L2_2e5_medium1", &L2_2e5_medium1, &b_L2_2e5_medium1);
   fChain->SetBranchAddress("L2_2g10_loose", &L2_2g10_loose, &b_L2_2g10_loose);
   fChain->SetBranchAddress("L2_2g10_loose_mu6", &L2_2g10_loose_mu6, &b_L2_2g10_loose_mu6);
   fChain->SetBranchAddress("L2_2g17i_tight", &L2_2g17i_tight, &b_L2_2g17i_tight);
   fChain->SetBranchAddress("L2_2g20_loose", &L2_2g20_loose, &b_L2_2g20_loose);
   fChain->SetBranchAddress("L2_2mu10", &L2_2mu10, &b_L2_2mu10);
   fChain->SetBranchAddress("L2_2mu20", &L2_2mu20, &b_L2_2mu20);
   fChain->SetBranchAddress("L2_2mu4", &L2_2mu4, &b_L2_2mu4);
   fChain->SetBranchAddress("L2_2mu4_Bmumu", &L2_2mu4_Bmumu, &b_L2_2mu4_Bmumu);
   fChain->SetBranchAddress("L2_2mu4_Bmumux", &L2_2mu4_Bmumux, &b_L2_2mu4_Bmumux);
   fChain->SetBranchAddress("L2_2mu4_DiMu", &L2_2mu4_DiMu, &b_L2_2mu4_DiMu);
   fChain->SetBranchAddress("L2_2mu4_DiMu_SiTrk", &L2_2mu4_DiMu_SiTrk, &b_L2_2mu4_DiMu_SiTrk);
   fChain->SetBranchAddress("L2_2mu4_DiMu_noVtx", &L2_2mu4_DiMu_noVtx, &b_L2_2mu4_DiMu_noVtx);
   fChain->SetBranchAddress("L2_2mu4_DiMu_noVtx_noOS", &L2_2mu4_DiMu_noVtx_noOS, &b_L2_2mu4_DiMu_noVtx_noOS);
   fChain->SetBranchAddress("L2_2mu4_Jpsimumu", &L2_2mu4_Jpsimumu, &b_L2_2mu4_Jpsimumu);
   fChain->SetBranchAddress("L2_2mu4_Upsimumu", &L2_2mu4_Upsimumu, &b_L2_2mu4_Upsimumu);
   fChain->SetBranchAddress("L2_2mu6", &L2_2mu6, &b_L2_2mu6);
   fChain->SetBranchAddress("L2_2mu6_Bmumu", &L2_2mu6_Bmumu, &b_L2_2mu6_Bmumu);
   fChain->SetBranchAddress("L2_2mu6_Bmumux", &L2_2mu6_Bmumux, &b_L2_2mu6_Bmumux);
   fChain->SetBranchAddress("L2_2mu6_DiMu", &L2_2mu6_DiMu, &b_L2_2mu6_DiMu);
   fChain->SetBranchAddress("L2_2mu6_Jpsimumu", &L2_2mu6_Jpsimumu, &b_L2_2mu6_Jpsimumu);
   fChain->SetBranchAddress("L2_2mu6_Upsimumu", &L2_2mu6_Upsimumu, &b_L2_2mu6_Upsimumu);
   fChain->SetBranchAddress("L2_2mu6_e10_loose", &L2_2mu6_e10_loose, &b_L2_2mu6_e10_loose);
   fChain->SetBranchAddress("L2_2mu6_g10_loose", &L2_2mu6_g10_loose, &b_L2_2mu6_g10_loose);
   fChain->SetBranchAddress("L2_e105_loose1", &L2_e105_loose1, &b_L2_e105_loose1);
   fChain->SetBranchAddress("L2_e10_NoCut", &L2_e10_NoCut, &b_L2_e10_NoCut);
   fChain->SetBranchAddress("L2_e10_loose", &L2_e10_loose, &b_L2_e10_loose);
   fChain->SetBranchAddress("L2_e10_loose_IdScan", &L2_e10_loose_IdScan, &b_L2_e10_loose_IdScan);
   fChain->SetBranchAddress("L2_e10_loose_SiTrk", &L2_e10_loose_SiTrk, &b_L2_e10_loose_SiTrk);
   fChain->SetBranchAddress("L2_e10_loose_TRT", &L2_e10_loose_TRT, &b_L2_e10_loose_TRT);
   fChain->SetBranchAddress("L2_e10_loose_mu6", &L2_e10_loose_mu6, &b_L2_e10_loose_mu6);
   fChain->SetBranchAddress("L2_e10_medium", &L2_e10_medium, &b_L2_e10_medium);
   fChain->SetBranchAddress("L2_e10_medium_SiTrk", &L2_e10_medium_SiTrk, &b_L2_e10_medium_SiTrk);
   fChain->SetBranchAddress("L2_e10_medium_SiTrk_robust", &L2_e10_medium_SiTrk_robust, &b_L2_e10_medium_SiTrk_robust);
   fChain->SetBranchAddress("L2_e12_medium", &L2_e12_medium, &b_L2_e12_medium);
   fChain->SetBranchAddress("L2_e15_medium", &L2_e15_medium, &b_L2_e15_medium);
   fChain->SetBranchAddress("L2_e15i_medium", &L2_e15i_medium, &b_L2_e15i_medium);
   fChain->SetBranchAddress("L2_e20_loose", &L2_e20_loose, &b_L2_e20_loose);
   fChain->SetBranchAddress("L2_e20_loose_IdScan", &L2_e20_loose_IdScan, &b_L2_e20_loose_IdScan);
   fChain->SetBranchAddress("L2_e20_loose_NoIDTrkCut", &L2_e20_loose_NoIDTrkCut, &b_L2_e20_loose_NoIDTrkCut);
   fChain->SetBranchAddress("L2_e20_loose_SiTrk", &L2_e20_loose_SiTrk, &b_L2_e20_loose_SiTrk);
   fChain->SetBranchAddress("L2_e20_loose_SiTrk_robust", &L2_e20_loose_SiTrk_robust, &b_L2_e20_loose_SiTrk_robust);
   fChain->SetBranchAddress("L2_e20_loose_TRT", &L2_e20_loose_TRT, &b_L2_e20_loose_TRT);
   fChain->SetBranchAddress("L2_e20_loose_passEF", &L2_e20_loose_passEF, &b_L2_e20_loose_passEF);
   fChain->SetBranchAddress("L2_e20_loose_passL2", &L2_e20_loose_passL2, &b_L2_e20_loose_passL2);
   fChain->SetBranchAddress("L2_e20_loose_xe20", &L2_e20_loose_xe20, &b_L2_e20_loose_xe20);
   fChain->SetBranchAddress("L2_e20_loose_xe30", &L2_e20_loose_xe30, &b_L2_e20_loose_xe30);
   fChain->SetBranchAddress("L2_e20_medium", &L2_e20_medium, &b_L2_e20_medium);
   fChain->SetBranchAddress("L2_e20i_loose", &L2_e20i_loose, &b_L2_e20i_loose);
   fChain->SetBranchAddress("L2_e22i_tight", &L2_e22i_tight, &b_L2_e22i_tight);
   fChain->SetBranchAddress("L2_e25_loose", &L2_e25_loose, &b_L2_e25_loose);
   fChain->SetBranchAddress("L2_e25_medium", &L2_e25_medium, &b_L2_e25_medium);
   fChain->SetBranchAddress("L2_e25i_loose", &L2_e25i_loose, &b_L2_e25i_loose);
   fChain->SetBranchAddress("L2_e25i_medium", &L2_e25i_medium, &b_L2_e25i_medium);
   fChain->SetBranchAddress("L2_e55_loose1", &L2_e55_loose1, &b_L2_e55_loose1);
   fChain->SetBranchAddress("L2_e5_medium", &L2_e5_medium, &b_L2_e5_medium);
   fChain->SetBranchAddress("L2_e5_medium1", &L2_e5_medium1, &b_L2_e5_medium1);
   fChain->SetBranchAddress("L2_e5_medium_L2SW", &L2_e5_medium_L2SW, &b_L2_e5_medium_L2SW);
   fChain->SetBranchAddress("L2_e5_medium_MV", &L2_e5_medium_MV, &b_L2_e5_medium_MV);
   fChain->SetBranchAddress("L2_e5_medium_Ringer", &L2_e5_medium_Ringer, &b_L2_e5_medium_Ringer);
   fChain->SetBranchAddress("L2_e5_medium_e10_loose", &L2_e5_medium_e10_loose, &b_L2_e5_medium_e10_loose);
   fChain->SetBranchAddress("L2_em105_passHLT", &L2_em105_passHLT, &b_L2_em105_passHLT);
   fChain->SetBranchAddress("L2_em15_passHLT", &L2_em15_passHLT, &b_L2_em15_passHLT);
   fChain->SetBranchAddress("L2_em15i_passHLT", &L2_em15i_passHLT, &b_L2_em15i_passHLT);
   fChain->SetBranchAddress("L2_em20_passHLT", &L2_em20_passHLT, &b_L2_em20_passHLT);
   fChain->SetBranchAddress("L2_em20i_passHLT", &L2_em20i_passHLT, &b_L2_em20i_passHLT);
   fChain->SetBranchAddress("L2_g105", &L2_g105, &b_L2_g105);
   fChain->SetBranchAddress("L2_g10_loose", &L2_g10_loose, &b_L2_g10_loose);
   fChain->SetBranchAddress("L2_g10_nocut", &L2_g10_nocut, &b_L2_g10_nocut);
   fChain->SetBranchAddress("L2_g150", &L2_g150, &b_L2_g150);
   fChain->SetBranchAddress("L2_g15_loose", &L2_g15_loose, &b_L2_g15_loose);
   fChain->SetBranchAddress("L2_g20_loose", &L2_g20_loose, &b_L2_g20_loose);
   fChain->SetBranchAddress("L2_g20_loose_cnv", &L2_g20_loose_cnv, &b_L2_g20_loose_cnv);
   fChain->SetBranchAddress("L2_g20_loose_xe20", &L2_g20_loose_xe20, &b_L2_g20_loose_xe20);
   fChain->SetBranchAddress("L2_g20i_loose", &L2_g20i_loose, &b_L2_g20i_loose);
   fChain->SetBranchAddress("L2_g25_loose", &L2_g25_loose, &b_L2_g25_loose);
   fChain->SetBranchAddress("L2_g25_loose_xe30", &L2_g25_loose_xe30, &b_L2_g25_loose_xe30);
   fChain->SetBranchAddress("L2_g25i_loose", &L2_g25i_loose, &b_L2_g25i_loose);
   fChain->SetBranchAddress("L2_g55_tight", &L2_g55_tight, &b_L2_g55_tight);
   fChain->SetBranchAddress("L2_mu10", &L2_mu10, &b_L2_mu10);
   fChain->SetBranchAddress("L2_mu10_DsPhiPi", &L2_mu10_DsPhiPi, &b_L2_mu10_DsPhiPi);
   fChain->SetBranchAddress("L2_mu10_DsPhiPi_FS", &L2_mu10_DsPhiPi_FS, &b_L2_mu10_DsPhiPi_FS);
   fChain->SetBranchAddress("L2_mu10_MG", &L2_mu10_MG, &b_L2_mu10_MG);
   fChain->SetBranchAddress("L2_mu10_MSonly", &L2_mu10_MSonly, &b_L2_mu10_MSonly);
   fChain->SetBranchAddress("L2_mu10_MuTagIMO", &L2_mu10_MuTagIMO, &b_L2_mu10_MuTagIMO);
   fChain->SetBranchAddress("L2_mu10_NoIDTrkCut", &L2_mu10_NoIDTrkCut, &b_L2_mu10_NoIDTrkCut);
   fChain->SetBranchAddress("L2_mu10_SiTrk", &L2_mu10_SiTrk, &b_L2_mu10_SiTrk);
   fChain->SetBranchAddress("L2_mu10i_loose", &L2_mu10i_loose, &b_L2_mu10i_loose);
   fChain->SetBranchAddress("L2_mu13", &L2_mu13, &b_L2_mu13);
   fChain->SetBranchAddress("L2_mu15", &L2_mu15, &b_L2_mu15);
   fChain->SetBranchAddress("L2_mu15_xe20", &L2_mu15_xe20, &b_L2_mu15_xe20);
   fChain->SetBranchAddress("L2_mu15i_loose", &L2_mu15i_loose, &b_L2_mu15i_loose);
   fChain->SetBranchAddress("L2_mu20", &L2_mu20, &b_L2_mu20);
   fChain->SetBranchAddress("L2_mu20_MSonly", &L2_mu20_MSonly, &b_L2_mu20_MSonly);
   fChain->SetBranchAddress("L2_mu20_passHLT", &L2_mu20_passHLT, &b_L2_mu20_passHLT);
   fChain->SetBranchAddress("L2_mu20_slow", &L2_mu20_slow, &b_L2_mu20_slow);
   fChain->SetBranchAddress("L2_mu20_xe30", &L2_mu20_xe30, &b_L2_mu20_xe30);
   fChain->SetBranchAddress("L2_mu20i_loose", &L2_mu20i_loose, &b_L2_mu20i_loose);
   fChain->SetBranchAddress("L2_mu20i_loose_MSonly", &L2_mu20i_loose_MSonly, &b_L2_mu20i_loose_MSonly);
   fChain->SetBranchAddress("L2_mu4", &L2_mu4, &b_L2_mu4);
   fChain->SetBranchAddress("L2_mu4_Bmumu", &L2_mu4_Bmumu, &b_L2_mu4_Bmumu);
   fChain->SetBranchAddress("L2_mu4_BmumuX", &L2_mu4_BmumuX, &b_L2_mu4_BmumuX);
   fChain->SetBranchAddress("L2_mu4_Bmumu_FS", &L2_mu4_Bmumu_FS, &b_L2_mu4_Bmumu_FS);
   fChain->SetBranchAddress("L2_mu4_DiMu", &L2_mu4_DiMu, &b_L2_mu4_DiMu);
   fChain->SetBranchAddress("L2_mu4_DiMu_FS", &L2_mu4_DiMu_FS, &b_L2_mu4_DiMu_FS);
   fChain->SetBranchAddress("L2_mu4_DiMu_FS_noOS", &L2_mu4_DiMu_FS_noOS, &b_L2_mu4_DiMu_FS_noOS);
   fChain->SetBranchAddress("L2_mu4_DiMu_MG", &L2_mu4_DiMu_MG, &b_L2_mu4_DiMu_MG);
   fChain->SetBranchAddress("L2_mu4_DiMu_MG_FS", &L2_mu4_DiMu_MG_FS, &b_L2_mu4_DiMu_MG_FS);
   fChain->SetBranchAddress("L2_mu4_DiMu_SiTrk", &L2_mu4_DiMu_SiTrk, &b_L2_mu4_DiMu_SiTrk);
   fChain->SetBranchAddress("L2_mu4_DiMu_SiTrk_FS", &L2_mu4_DiMu_SiTrk_FS, &b_L2_mu4_DiMu_SiTrk_FS);
   fChain->SetBranchAddress("L2_mu4_DiMu_noOS", &L2_mu4_DiMu_noOS, &b_L2_mu4_DiMu_noOS);
   fChain->SetBranchAddress("L2_mu4_DsPhiPi", &L2_mu4_DsPhiPi, &b_L2_mu4_DsPhiPi);
   fChain->SetBranchAddress("L2_mu4_DsPhiPi_FS", &L2_mu4_DsPhiPi_FS, &b_L2_mu4_DsPhiPi_FS);
   fChain->SetBranchAddress("L2_mu4_Jpsie5e3", &L2_mu4_Jpsie5e3, &b_L2_mu4_Jpsie5e3);
   fChain->SetBranchAddress("L2_mu4_Jpsie5e3_FS", &L2_mu4_Jpsie5e3_FS, &b_L2_mu4_Jpsie5e3_FS);
   fChain->SetBranchAddress("L2_mu4_Jpsie5e3_SiTrk", &L2_mu4_Jpsie5e3_SiTrk, &b_L2_mu4_Jpsie5e3_SiTrk);
   fChain->SetBranchAddress("L2_mu4_Jpsie5e3_SiTrk_FS", &L2_mu4_Jpsie5e3_SiTrk_FS, &b_L2_mu4_Jpsie5e3_SiTrk_FS);
   fChain->SetBranchAddress("L2_mu4_Jpsimumu", &L2_mu4_Jpsimumu, &b_L2_mu4_Jpsimumu);
   fChain->SetBranchAddress("L2_mu4_Jpsimumu_FS", &L2_mu4_Jpsimumu_FS, &b_L2_mu4_Jpsimumu_FS);
   fChain->SetBranchAddress("L2_mu4_MG", &L2_mu4_MG, &b_L2_mu4_MG);
   fChain->SetBranchAddress("L2_mu4_MSonly", &L2_mu4_MSonly, &b_L2_mu4_MSonly);
   fChain->SetBranchAddress("L2_mu4_MV", &L2_mu4_MV, &b_L2_mu4_MV);
   fChain->SetBranchAddress("L2_mu4_SiTrk", &L2_mu4_SiTrk, &b_L2_mu4_SiTrk);
   fChain->SetBranchAddress("L2_mu4_Trk_Jpsi", &L2_mu4_Trk_Jpsi, &b_L2_mu4_Trk_Jpsi);
   fChain->SetBranchAddress("L2_mu4_Trk_Jpsi_FS", &L2_mu4_Trk_Jpsi_FS, &b_L2_mu4_Trk_Jpsi_FS);
   fChain->SetBranchAddress("L2_mu4_Trk_Jpsi_loose", &L2_mu4_Trk_Jpsi_loose, &b_L2_mu4_Trk_Jpsi_loose);
   fChain->SetBranchAddress("L2_mu4_Trk_Upsi_FS", &L2_mu4_Trk_Upsi_FS, &b_L2_mu4_Trk_Upsi_FS);
   fChain->SetBranchAddress("L2_mu4_Trk_Upsi_loose_FS", &L2_mu4_Trk_Upsi_loose_FS, &b_L2_mu4_Trk_Upsi_loose_FS);
   fChain->SetBranchAddress("L2_mu4_Upsimumu", &L2_mu4_Upsimumu, &b_L2_mu4_Upsimumu);
   fChain->SetBranchAddress("L2_mu4_Upsimumu_FS", &L2_mu4_Upsimumu_FS, &b_L2_mu4_Upsimumu_FS);
   fChain->SetBranchAddress("L2_mu4_j10", &L2_mu4_j10, &b_L2_mu4_j10);
   fChain->SetBranchAddress("L2_mu4_j10_matched", &L2_mu4_j10_matched, &b_L2_mu4_j10_matched);
   fChain->SetBranchAddress("L2_mu4_j15", &L2_mu4_j15, &b_L2_mu4_j15);
   fChain->SetBranchAddress("L2_mu4_j20_matched", &L2_mu4_j20_matched, &b_L2_mu4_j20_matched);
   fChain->SetBranchAddress("L2_mu4_j40_matched", &L2_mu4_j40_matched, &b_L2_mu4_j40_matched);
   fChain->SetBranchAddress("L2_mu4_j70_matched", &L2_mu4_j70_matched, &b_L2_mu4_j70_matched);
   fChain->SetBranchAddress("L2_mu4_mu6", &L2_mu4_mu6, &b_L2_mu4_mu6);
   fChain->SetBranchAddress("L2_mu4_tile", &L2_mu4_tile, &b_L2_mu4_tile);
   fChain->SetBranchAddress("L2_mu4_trod", &L2_mu4_trod, &b_L2_mu4_trod);
   fChain->SetBranchAddress("L2_mu4mu6_Bmumu", &L2_mu4mu6_Bmumu, &b_L2_mu4mu6_Bmumu);
   fChain->SetBranchAddress("L2_mu4mu6_DiMu", &L2_mu4mu6_DiMu, &b_L2_mu4mu6_DiMu);
   fChain->SetBranchAddress("L2_mu4mu6_Jpsimumu", &L2_mu4mu6_Jpsimumu, &b_L2_mu4mu6_Jpsimumu);
   fChain->SetBranchAddress("L2_mu4mu6_Upsimumu", &L2_mu4mu6_Upsimumu, &b_L2_mu4mu6_Upsimumu);
   fChain->SetBranchAddress("L2_mu6", &L2_mu6, &b_L2_mu6);
   fChain->SetBranchAddress("L2_mu6_Bmumu", &L2_mu6_Bmumu, &b_L2_mu6_Bmumu);
   fChain->SetBranchAddress("L2_mu6_BmumuX", &L2_mu6_BmumuX, &b_L2_mu6_BmumuX);
   fChain->SetBranchAddress("L2_mu6_Bmumu_FS", &L2_mu6_Bmumu_FS, &b_L2_mu6_Bmumu_FS);
   fChain->SetBranchAddress("L2_mu6_DiMu", &L2_mu6_DiMu, &b_L2_mu6_DiMu);
   fChain->SetBranchAddress("L2_mu6_DiMu_FS", &L2_mu6_DiMu_FS, &b_L2_mu6_DiMu_FS);
   fChain->SetBranchAddress("L2_mu6_DsPhiPi", &L2_mu6_DsPhiPi, &b_L2_mu6_DsPhiPi);
   fChain->SetBranchAddress("L2_mu6_DsPhiPi_FS", &L2_mu6_DsPhiPi_FS, &b_L2_mu6_DsPhiPi_FS);
   fChain->SetBranchAddress("L2_mu6_Jpsie5e3", &L2_mu6_Jpsie5e3, &b_L2_mu6_Jpsie5e3);
   fChain->SetBranchAddress("L2_mu6_Jpsie5e3_FS", &L2_mu6_Jpsie5e3_FS, &b_L2_mu6_Jpsie5e3_FS);
   fChain->SetBranchAddress("L2_mu6_Jpsie5e3_SiTrk", &L2_mu6_Jpsie5e3_SiTrk, &b_L2_mu6_Jpsie5e3_SiTrk);
   fChain->SetBranchAddress("L2_mu6_Jpsie5e3_SiTrk_FS", &L2_mu6_Jpsie5e3_SiTrk_FS, &b_L2_mu6_Jpsie5e3_SiTrk_FS);
   fChain->SetBranchAddress("L2_mu6_Jpsimumu", &L2_mu6_Jpsimumu, &b_L2_mu6_Jpsimumu);
   fChain->SetBranchAddress("L2_mu6_Jpsimumu_FS", &L2_mu6_Jpsimumu_FS, &b_L2_mu6_Jpsimumu_FS);
   fChain->SetBranchAddress("L2_mu6_MG", &L2_mu6_MG, &b_L2_mu6_MG);
   fChain->SetBranchAddress("L2_mu6_MSonly", &L2_mu6_MSonly, &b_L2_mu6_MSonly);
   fChain->SetBranchAddress("L2_mu6_Trk_Jpsi", &L2_mu6_Trk_Jpsi, &b_L2_mu6_Trk_Jpsi);
   fChain->SetBranchAddress("L2_mu6_Upsimumu_FS", &L2_mu6_Upsimumu_FS, &b_L2_mu6_Upsimumu_FS);
   fChain->SetBranchAddress("ph_n", &ph_n, &b_ph_n);
   fChain->SetBranchAddress("ph_E", &ph_E, &b_ph_E);
   fChain->SetBranchAddress("ph_Et", &ph_Et, &b_ph_Et);
   fChain->SetBranchAddress("ph_pt", &ph_pt, &b_ph_pt);
   fChain->SetBranchAddress("ph_m", &ph_m, &b_ph_m);
   fChain->SetBranchAddress("ph_eta", &ph_eta, &b_ph_eta);
   fChain->SetBranchAddress("ph_phi", &ph_phi, &b_ph_phi);
   fChain->SetBranchAddress("ph_px", &ph_px, &b_ph_px);
   fChain->SetBranchAddress("ph_py", &ph_py, &b_ph_py);
   fChain->SetBranchAddress("ph_pz", &ph_pz, &b_ph_pz);
   fChain->SetBranchAddress("ph_author", &ph_author, &b_ph_author);
   fChain->SetBranchAddress("ph_isRecovered", &ph_isRecovered, &b_ph_isRecovered);
   fChain->SetBranchAddress("ph_isEM", &ph_isEM, &b_ph_isEM);
   fChain->SetBranchAddress("ph_convFlag", &ph_convFlag, &b_ph_convFlag);
   fChain->SetBranchAddress("ph_isConv", &ph_isConv, &b_ph_isConv);
   fChain->SetBranchAddress("ph_nConv", &ph_nConv, &b_ph_nConv);
   fChain->SetBranchAddress("ph_nSingleTrackConv", &ph_nSingleTrackConv, &b_ph_nSingleTrackConv);
   fChain->SetBranchAddress("ph_nDoubleTrackConv", &ph_nDoubleTrackConv, &b_ph_nDoubleTrackConv);
   fChain->SetBranchAddress("ph_type", &ph_type, &b_ph_type);
   fChain->SetBranchAddress("ph_origin", &ph_origin, &b_ph_origin);
   fChain->SetBranchAddress("ph_truth_deltaRRecPhoton", &ph_truth_deltaRRecPhoton, &b_ph_truth_deltaRRecPhoton);
   fChain->SetBranchAddress("ph_truth_E", &ph_truth_E, &b_ph_truth_E);
   fChain->SetBranchAddress("ph_truth_pt", &ph_truth_pt, &b_ph_truth_pt);
   fChain->SetBranchAddress("ph_truth_eta", &ph_truth_eta, &b_ph_truth_eta);
   fChain->SetBranchAddress("ph_truth_phi", &ph_truth_phi, &b_ph_truth_phi);
   fChain->SetBranchAddress("ph_truth_type", &ph_truth_type, &b_ph_truth_type);
   fChain->SetBranchAddress("ph_truth_status", &ph_truth_status, &b_ph_truth_status);
   fChain->SetBranchAddress("ph_truth_barcode", &ph_truth_barcode, &b_ph_truth_barcode);
   fChain->SetBranchAddress("ph_truth_mothertype", &ph_truth_mothertype, &b_ph_truth_mothertype);
   fChain->SetBranchAddress("ph_truth_motherbarcode", &ph_truth_motherbarcode, &b_ph_truth_motherbarcode);
   fChain->SetBranchAddress("ph_truth_matched", &ph_truth_matched, &b_ph_truth_matched);
   fChain->SetBranchAddress("ph_loose", &ph_loose, &b_ph_loose);
   fChain->SetBranchAddress("ph_tight", &ph_tight, &b_ph_tight);
   fChain->SetBranchAddress("ph_tightIso", &ph_tightIso, &b_ph_tightIso);
   fChain->SetBranchAddress("ph_Ethad", &ph_Ethad, &b_ph_Ethad);
   fChain->SetBranchAddress("ph_Ethad1", &ph_Ethad1, &b_ph_Ethad1);
   fChain->SetBranchAddress("ph_E033", &ph_E033, &b_ph_E033);
   fChain->SetBranchAddress("ph_f1", &ph_f1, &b_ph_f1);
   fChain->SetBranchAddress("ph_f1core", &ph_f1core, &b_ph_f1core);
   fChain->SetBranchAddress("ph_Emins1", &ph_Emins1, &b_ph_Emins1);
   fChain->SetBranchAddress("ph_fside", &ph_fside, &b_ph_fside);
   fChain->SetBranchAddress("ph_Emax2", &ph_Emax2, &b_ph_Emax2);
   fChain->SetBranchAddress("ph_ws3", &ph_ws3, &b_ph_ws3);
   fChain->SetBranchAddress("ph_wstot", &ph_wstot, &b_ph_wstot);
   fChain->SetBranchAddress("ph_E132", &ph_E132, &b_ph_E132);
   fChain->SetBranchAddress("ph_E1152", &ph_E1152, &b_ph_E1152);
   fChain->SetBranchAddress("ph_emaxs1", &ph_emaxs1, &b_ph_emaxs1);
   fChain->SetBranchAddress("ph_deltaEs", &ph_deltaEs, &b_ph_deltaEs);
   fChain->SetBranchAddress("ph_E233", &ph_E233, &b_ph_E233);
   fChain->SetBranchAddress("ph_E237", &ph_E237, &b_ph_E237);
   fChain->SetBranchAddress("ph_E277", &ph_E277, &b_ph_E277);
   fChain->SetBranchAddress("ph_weta2", &ph_weta2, &b_ph_weta2);
   fChain->SetBranchAddress("ph_f3", &ph_f3, &b_ph_f3);
   fChain->SetBranchAddress("ph_f3core", &ph_f3core, &b_ph_f3core);
   fChain->SetBranchAddress("ph_rphiallcalo", &ph_rphiallcalo, &b_ph_rphiallcalo);
   fChain->SetBranchAddress("ph_Etcone45", &ph_Etcone45, &b_ph_Etcone45);
   fChain->SetBranchAddress("ph_Etcone20", &ph_Etcone20, &b_ph_Etcone20);
   fChain->SetBranchAddress("ph_Etcone30", &ph_Etcone30, &b_ph_Etcone30);
   fChain->SetBranchAddress("ph_Etcone40", &ph_Etcone40, &b_ph_Etcone40);
   fChain->SetBranchAddress("ph_ptcone30", &ph_ptcone30, &b_ph_ptcone30);
   fChain->SetBranchAddress("ph_convanglematch", &ph_convanglematch, &b_ph_convanglematch);
   fChain->SetBranchAddress("ph_convtrackmatch", &ph_convtrackmatch, &b_ph_convtrackmatch);
   fChain->SetBranchAddress("ph_hasconv", &ph_hasconv, &b_ph_hasconv);
   fChain->SetBranchAddress("ph_convvtxx", &ph_convvtxx, &b_ph_convvtxx);
   fChain->SetBranchAddress("ph_convvtxy", &ph_convvtxy, &b_ph_convvtxy);
   fChain->SetBranchAddress("ph_convvtxz", &ph_convvtxz, &b_ph_convvtxz);
   fChain->SetBranchAddress("ph_Rconv", &ph_Rconv, &b_ph_Rconv);
   fChain->SetBranchAddress("ph_zconv", &ph_zconv, &b_ph_zconv);
   fChain->SetBranchAddress("ph_convvtxchi2", &ph_convvtxchi2, &b_ph_convvtxchi2);
   fChain->SetBranchAddress("ph_pt1conv", &ph_pt1conv, &b_ph_pt1conv);
   fChain->SetBranchAddress("ph_convtrk1nBLHits", &ph_convtrk1nBLHits, &b_ph_convtrk1nBLHits);
   fChain->SetBranchAddress("ph_convtrk1nPixHits", &ph_convtrk1nPixHits, &b_ph_convtrk1nPixHits);
   fChain->SetBranchAddress("ph_convtrk1nSCTHits", &ph_convtrk1nSCTHits, &b_ph_convtrk1nSCTHits);
   fChain->SetBranchAddress("ph_convtrk1nTRTHits", &ph_convtrk1nTRTHits, &b_ph_convtrk1nTRTHits);
   fChain->SetBranchAddress("ph_pt2conv", &ph_pt2conv, &b_ph_pt2conv);
   fChain->SetBranchAddress("ph_convtrk2nBLHits", &ph_convtrk2nBLHits, &b_ph_convtrk2nBLHits);
   fChain->SetBranchAddress("ph_convtrk2nPixHits", &ph_convtrk2nPixHits, &b_ph_convtrk2nPixHits);
   fChain->SetBranchAddress("ph_convtrk2nSCTHits", &ph_convtrk2nSCTHits, &b_ph_convtrk2nSCTHits);
   fChain->SetBranchAddress("ph_convtrk2nTRTHits", &ph_convtrk2nTRTHits, &b_ph_convtrk2nTRTHits);
   fChain->SetBranchAddress("ph_ptconv", &ph_ptconv, &b_ph_ptconv);
   fChain->SetBranchAddress("ph_pzconv", &ph_pzconv, &b_ph_pzconv);
   fChain->SetBranchAddress("ph_reta", &ph_reta, &b_ph_reta);
   fChain->SetBranchAddress("ph_rphi", &ph_rphi, &b_ph_rphi);
   fChain->SetBranchAddress("ph_EtringnoisedR03sig2", &ph_EtringnoisedR03sig2, &b_ph_EtringnoisedR03sig2);
   fChain->SetBranchAddress("ph_EtringnoisedR03sig3", &ph_EtringnoisedR03sig3, &b_ph_EtringnoisedR03sig3);
   fChain->SetBranchAddress("ph_EtringnoisedR03sig4", &ph_EtringnoisedR03sig4, &b_ph_EtringnoisedR03sig4);
   fChain->SetBranchAddress("ph_isolationlikelihoodjets", &ph_isolationlikelihoodjets, &b_ph_isolationlikelihoodjets);
   fChain->SetBranchAddress("ph_isolationlikelihoodhqelectrons", &ph_isolationlikelihoodhqelectrons, &b_ph_isolationlikelihoodhqelectrons);
   fChain->SetBranchAddress("ph_loglikelihood", &ph_loglikelihood, &b_ph_loglikelihood);
   fChain->SetBranchAddress("ph_photonweight", &ph_photonweight, &b_ph_photonweight);
   fChain->SetBranchAddress("ph_photonbgweight", &ph_photonbgweight, &b_ph_photonbgweight);
   fChain->SetBranchAddress("ph_neuralnet", &ph_neuralnet, &b_ph_neuralnet);
   fChain->SetBranchAddress("ph_Hmatrix", &ph_Hmatrix, &b_ph_Hmatrix);
   fChain->SetBranchAddress("ph_Hmatrix5", &ph_Hmatrix5, &b_ph_Hmatrix5);
   fChain->SetBranchAddress("ph_adaboost", &ph_adaboost, &b_ph_adaboost);
   fChain->SetBranchAddress("ph_zvertex", &ph_zvertex, &b_ph_zvertex);
   fChain->SetBranchAddress("ph_errz", &ph_errz, &b_ph_errz);
   fChain->SetBranchAddress("ph_etap", &ph_etap, &b_ph_etap);
   fChain->SetBranchAddress("ph_depth", &ph_depth, &b_ph_depth);
   fChain->SetBranchAddress("ph_cl_E", &ph_cl_E, &b_ph_cl_E);
   fChain->SetBranchAddress("ph_cl_pt", &ph_cl_pt, &b_ph_cl_pt);
   fChain->SetBranchAddress("ph_cl_eta", &ph_cl_eta, &b_ph_cl_eta);
   fChain->SetBranchAddress("ph_cl_phi", &ph_cl_phi, &b_ph_cl_phi);
   fChain->SetBranchAddress("ph_Es0", &ph_Es0, &b_ph_Es0);
   fChain->SetBranchAddress("ph_etas0", &ph_etas0, &b_ph_etas0);
   fChain->SetBranchAddress("ph_phis0", &ph_phis0, &b_ph_phis0);
   fChain->SetBranchAddress("ph_Es1", &ph_Es1, &b_ph_Es1);
   fChain->SetBranchAddress("ph_etas1", &ph_etas1, &b_ph_etas1);
   fChain->SetBranchAddress("ph_phis1", &ph_phis1, &b_ph_phis1);
   fChain->SetBranchAddress("ph_Es2", &ph_Es2, &b_ph_Es2);
   fChain->SetBranchAddress("ph_etas2", &ph_etas2, &b_ph_etas2);
   fChain->SetBranchAddress("ph_phis2", &ph_phis2, &b_ph_phis2);
   fChain->SetBranchAddress("ph_Es3", &ph_Es3, &b_ph_Es3);
   fChain->SetBranchAddress("ph_etas3", &ph_etas3, &b_ph_etas3);
   fChain->SetBranchAddress("ph_phis3", &ph_phis3, &b_ph_phis3);
   fChain->SetBranchAddress("ph_rawcl_Es0", &ph_rawcl_Es0, &b_ph_rawcl_Es0);
   fChain->SetBranchAddress("ph_rawcl_etas0", &ph_rawcl_etas0, &b_ph_rawcl_etas0);
   fChain->SetBranchAddress("ph_rawcl_phis0", &ph_rawcl_phis0, &b_ph_rawcl_phis0);
   fChain->SetBranchAddress("ph_rawcl_Es1", &ph_rawcl_Es1, &b_ph_rawcl_Es1);
   fChain->SetBranchAddress("ph_rawcl_etas1", &ph_rawcl_etas1, &b_ph_rawcl_etas1);
   fChain->SetBranchAddress("ph_rawcl_phis1", &ph_rawcl_phis1, &b_ph_rawcl_phis1);
   fChain->SetBranchAddress("ph_rawcl_Es2", &ph_rawcl_Es2, &b_ph_rawcl_Es2);
   fChain->SetBranchAddress("ph_rawcl_etas2", &ph_rawcl_etas2, &b_ph_rawcl_etas2);
   fChain->SetBranchAddress("ph_rawcl_phis2", &ph_rawcl_phis2, &b_ph_rawcl_phis2);
   fChain->SetBranchAddress("ph_rawcl_Es3", &ph_rawcl_Es3, &b_ph_rawcl_Es3);
   fChain->SetBranchAddress("ph_rawcl_etas3", &ph_rawcl_etas3, &b_ph_rawcl_etas3);
   fChain->SetBranchAddress("ph_rawcl_phis3", &ph_rawcl_phis3, &b_ph_rawcl_phis3);
   fChain->SetBranchAddress("ph_rawcl_E", &ph_rawcl_E, &b_ph_rawcl_E);
   fChain->SetBranchAddress("ph_rawcl_pt", &ph_rawcl_pt, &b_ph_rawcl_pt);
   fChain->SetBranchAddress("ph_rawcl_eta", &ph_rawcl_eta, &b_ph_rawcl_eta);
   fChain->SetBranchAddress("ph_rawcl_phi", &ph_rawcl_phi, &b_ph_rawcl_phi);
   fChain->SetBranchAddress("ph_truth_isConv", &ph_truth_isConv, &b_ph_truth_isConv);
   fChain->SetBranchAddress("ph_truth_isBrem", &ph_truth_isBrem, &b_ph_truth_isBrem);
   fChain->SetBranchAddress("ph_truth_isFromHardProc", &ph_truth_isFromHardProc, &b_ph_truth_isFromHardProc);
   fChain->SetBranchAddress("ph_truth_isPhotonFromHardProc", &ph_truth_isPhotonFromHardProc, &b_ph_truth_isPhotonFromHardProc);
   fChain->SetBranchAddress("ph_truth_Rconv", &ph_truth_Rconv, &b_ph_truth_Rconv);
   fChain->SetBranchAddress("ph_truth_zconv", &ph_truth_zconv, &b_ph_truth_zconv);
   fChain->SetBranchAddress("ph_deltaEmax2", &ph_deltaEmax2, &b_ph_deltaEmax2);
   fChain->SetBranchAddress("ph_calibHitsShowerDepth", &ph_calibHitsShowerDepth, &b_ph_calibHitsShowerDepth);
   fChain->SetBranchAddress("ph_isIso", &ph_isIso, &b_ph_isIso);
   fChain->SetBranchAddress("ph_mvaptcone20", &ph_mvaptcone20, &b_ph_mvaptcone20);
   fChain->SetBranchAddress("ph_mvaptcone30", &ph_mvaptcone30, &b_ph_mvaptcone30);
   fChain->SetBranchAddress("ph_mvaptcone40", &ph_mvaptcone40, &b_ph_mvaptcone40);
   fChain->SetBranchAddress("ph_topoEtcone20", &ph_topoEtcone20, &b_ph_topoEtcone20);
   fChain->SetBranchAddress("ph_topoEtcone40", &ph_topoEtcone40, &b_ph_topoEtcone40);
   fChain->SetBranchAddress("ph_topoEtcone60", &ph_topoEtcone60, &b_ph_topoEtcone60);
   fChain->SetBranchAddress("ph_jet_dr", &ph_jet_dr, &b_ph_jet_dr);
   fChain->SetBranchAddress("ph_jet_E", &ph_jet_E, &b_ph_jet_E);
   fChain->SetBranchAddress("ph_jet_pt", &ph_jet_pt, &b_ph_jet_pt);
   fChain->SetBranchAddress("ph_jet_m", &ph_jet_m, &b_ph_jet_m);
   fChain->SetBranchAddress("ph_jet_eta", &ph_jet_eta, &b_ph_jet_eta);
   fChain->SetBranchAddress("ph_jet_phi", &ph_jet_phi, &b_ph_jet_phi);
   fChain->SetBranchAddress("ph_jet_truth_dr", &ph_jet_truth_dr, &b_ph_jet_truth_dr);
   fChain->SetBranchAddress("ph_jet_truth_E", &ph_jet_truth_E, &b_ph_jet_truth_E);
   fChain->SetBranchAddress("ph_jet_truth_pt", &ph_jet_truth_pt, &b_ph_jet_truth_pt);
   fChain->SetBranchAddress("ph_jet_truth_m", &ph_jet_truth_m, &b_ph_jet_truth_m);
   fChain->SetBranchAddress("ph_jet_truth_eta", &ph_jet_truth_eta, &b_ph_jet_truth_eta);
   fChain->SetBranchAddress("ph_jet_truth_phi", &ph_jet_truth_phi, &b_ph_jet_truth_phi);
   fChain->SetBranchAddress("ph_jet_truth_matched", &ph_jet_truth_matched, &b_ph_jet_truth_matched);
   fChain->SetBranchAddress("ph_jet_matched", &ph_jet_matched, &b_ph_jet_matched);
   fChain->SetBranchAddress("ph_convIP", &ph_convIP, &b_ph_convIP);
   fChain->SetBranchAddress("ph_convIPRev", &ph_convIPRev, &b_ph_convIPRev);
   fChain->SetBranchAddress("ph_ptIsolationCone", &ph_ptIsolationCone, &b_ph_ptIsolationCone);
   fChain->SetBranchAddress("ph_ptIsolationConePhAngle", &ph_ptIsolationConePhAngle, &b_ph_ptIsolationConePhAngle);
   fChain->SetBranchAddress("ph_Etcone40_pt_corrected", &ph_Etcone40_pt_corrected, &b_ph_Etcone40_pt_corrected);
   fChain->SetBranchAddress("ph_Etcone40_ED_corrected", &ph_Etcone40_ED_corrected, &b_ph_Etcone40_ED_corrected);
   fChain->SetBranchAddress("ph_Etcone40_corrected", &ph_Etcone40_corrected, &b_ph_Etcone40_corrected);
   fChain->SetBranchAddress("ph_topodr", &ph_topodr, &b_ph_topodr);
   fChain->SetBranchAddress("ph_topopt", &ph_topopt, &b_ph_topopt);
   fChain->SetBranchAddress("ph_topoeta", &ph_topoeta, &b_ph_topoeta);
   fChain->SetBranchAddress("ph_topophi", &ph_topophi, &b_ph_topophi);
   fChain->SetBranchAddress("ph_topomatched", &ph_topomatched, &b_ph_topomatched);
   fChain->SetBranchAddress("ph_topoEMdr", &ph_topoEMdr, &b_ph_topoEMdr);
   fChain->SetBranchAddress("ph_topoEMpt", &ph_topoEMpt, &b_ph_topoEMpt);
   fChain->SetBranchAddress("ph_topoEMeta", &ph_topoEMeta, &b_ph_topoEMeta);
   fChain->SetBranchAddress("ph_topoEMphi", &ph_topoEMphi, &b_ph_topoEMphi);
   fChain->SetBranchAddress("ph_topoEMmatched", &ph_topoEMmatched, &b_ph_topoEMmatched);
   fChain->SetBranchAddress("ph_EF_dr", &ph_EF_dr, &b_ph_EF_dr);
   fChain->SetBranchAddress("ph_EF_index", &ph_EF_index, &b_ph_EF_index);
   fChain->SetBranchAddress("ph_L2_dr", &ph_L2_dr, &b_ph_L2_dr);
   fChain->SetBranchAddress("ph_L2_index", &ph_L2_index, &b_ph_L2_index);
   fChain->SetBranchAddress("ph_L1_dr", &ph_L1_dr, &b_ph_L1_dr);
   fChain->SetBranchAddress("ph_L1_index", &ph_L1_index, &b_ph_L1_index);
   fChain->SetBranchAddress("mu_n", &mu_n, &b_mu_n);
   fChain->SetBranchAddress("mu_E", &mu_E, &b_mu_E);
   fChain->SetBranchAddress("mu_pt", &mu_pt, &b_mu_pt);
   fChain->SetBranchAddress("mu_m", &mu_m, &b_mu_m);
   fChain->SetBranchAddress("mu_eta", &mu_eta, &b_mu_eta);
   fChain->SetBranchAddress("mu_phi", &mu_phi, &b_mu_phi);
   fChain->SetBranchAddress("mu_px", &mu_px, &b_mu_px);
   fChain->SetBranchAddress("mu_py", &mu_py, &b_mu_py);
   fChain->SetBranchAddress("mu_pz", &mu_pz, &b_mu_pz);
   fChain->SetBranchAddress("mu_charge", &mu_charge, &b_mu_charge);
   fChain->SetBranchAddress("mu_allauthor", &mu_allauthor, &b_mu_allauthor);
   fChain->SetBranchAddress("mu_author", &mu_author, &b_mu_author);
   fChain->SetBranchAddress("mu_matchchi2", &mu_matchchi2, &b_mu_matchchi2);
   fChain->SetBranchAddress("mu_matchndof", &mu_matchndof, &b_mu_matchndof);
   fChain->SetBranchAddress("mu_etcone20", &mu_etcone20, &b_mu_etcone20);
   fChain->SetBranchAddress("mu_etcone30", &mu_etcone30, &b_mu_etcone30);
   fChain->SetBranchAddress("mu_etcone40", &mu_etcone40, &b_mu_etcone40);
   fChain->SetBranchAddress("mu_nucone20", &mu_nucone20, &b_mu_nucone20);
   fChain->SetBranchAddress("mu_nucone30", &mu_nucone30, &b_mu_nucone30);
   fChain->SetBranchAddress("mu_nucone40", &mu_nucone40, &b_mu_nucone40);
   fChain->SetBranchAddress("mu_ptcone20", &mu_ptcone20, &b_mu_ptcone20);
   fChain->SetBranchAddress("mu_ptcone30", &mu_ptcone30, &b_mu_ptcone30);
   fChain->SetBranchAddress("mu_ptcone40", &mu_ptcone40, &b_mu_ptcone40);
   fChain->SetBranchAddress("mu_energyLossPar", &mu_energyLossPar, &b_mu_energyLossPar);
   fChain->SetBranchAddress("mu_energyLossErr", &mu_energyLossErr, &b_mu_energyLossErr);
   fChain->SetBranchAddress("mu_etCore", &mu_etCore, &b_mu_etCore);
   fChain->SetBranchAddress("mu_energyLossType", &mu_energyLossType, &b_mu_energyLossType);
   fChain->SetBranchAddress("mu_bestMatch", &mu_bestMatch, &b_mu_bestMatch);
   fChain->SetBranchAddress("mu_isStandAloneMuon", &mu_isStandAloneMuon, &b_mu_isStandAloneMuon);
   fChain->SetBranchAddress("mu_isCombinedMuon", &mu_isCombinedMuon, &b_mu_isCombinedMuon);
   fChain->SetBranchAddress("mu_isLowPtReconstructedMuon", &mu_isLowPtReconstructedMuon, &b_mu_isLowPtReconstructedMuon);
   fChain->SetBranchAddress("mu_loose", &mu_loose, &b_mu_loose);
   fChain->SetBranchAddress("mu_medium", &mu_medium, &b_mu_medium);
   fChain->SetBranchAddress("mu_tight", &mu_tight, &b_mu_tight);
   fChain->SetBranchAddress("mu_d0_exPV", &mu_d0_exPV, &b_mu_d0_exPV);
   fChain->SetBranchAddress("mu_z0_exPV", &mu_z0_exPV, &b_mu_z0_exPV);
   fChain->SetBranchAddress("mu_phi_exPV", &mu_phi_exPV, &b_mu_phi_exPV);
   fChain->SetBranchAddress("mu_theta_exPV", &mu_theta_exPV, &b_mu_theta_exPV);
   fChain->SetBranchAddress("mu_qoverp_exPV", &mu_qoverp_exPV, &b_mu_qoverp_exPV);
   fChain->SetBranchAddress("mu_cb_d0_exPV", &mu_cb_d0_exPV, &b_mu_cb_d0_exPV);
   fChain->SetBranchAddress("mu_cb_z0_exPV", &mu_cb_z0_exPV, &b_mu_cb_z0_exPV);
   fChain->SetBranchAddress("mu_cb_phi_exPV", &mu_cb_phi_exPV, &b_mu_cb_phi_exPV);
   fChain->SetBranchAddress("mu_cb_theta_exPV", &mu_cb_theta_exPV, &b_mu_cb_theta_exPV);
   fChain->SetBranchAddress("mu_cb_qoverp_exPV", &mu_cb_qoverp_exPV, &b_mu_cb_qoverp_exPV);
   fChain->SetBranchAddress("mu_id_d0_exPV", &mu_id_d0_exPV, &b_mu_id_d0_exPV);
   fChain->SetBranchAddress("mu_id_z0_exPV", &mu_id_z0_exPV, &b_mu_id_z0_exPV);
   fChain->SetBranchAddress("mu_id_phi_exPV", &mu_id_phi_exPV, &b_mu_id_phi_exPV);
   fChain->SetBranchAddress("mu_id_theta_exPV", &mu_id_theta_exPV, &b_mu_id_theta_exPV);
   fChain->SetBranchAddress("mu_id_qoverp_exPV", &mu_id_qoverp_exPV, &b_mu_id_qoverp_exPV);
   fChain->SetBranchAddress("mu_me_d0_exPV", &mu_me_d0_exPV, &b_mu_me_d0_exPV);
   fChain->SetBranchAddress("mu_me_z0_exPV", &mu_me_z0_exPV, &b_mu_me_z0_exPV);
   fChain->SetBranchAddress("mu_me_phi_exPV", &mu_me_phi_exPV, &b_mu_me_phi_exPV);
   fChain->SetBranchAddress("mu_me_theta_exPV", &mu_me_theta_exPV, &b_mu_me_theta_exPV);
   fChain->SetBranchAddress("mu_me_qoverp_exPV", &mu_me_qoverp_exPV, &b_mu_me_qoverp_exPV);
   fChain->SetBranchAddress("mu_ie_d0_exPV", &mu_ie_d0_exPV, &b_mu_ie_d0_exPV);
   fChain->SetBranchAddress("mu_ie_z0_exPV", &mu_ie_z0_exPV, &b_mu_ie_z0_exPV);
   fChain->SetBranchAddress("mu_ie_phi_exPV", &mu_ie_phi_exPV, &b_mu_ie_phi_exPV);
   fChain->SetBranchAddress("mu_ie_theta_exPV", &mu_ie_theta_exPV, &b_mu_ie_theta_exPV);
   fChain->SetBranchAddress("mu_ie_qoverp_exPV", &mu_ie_qoverp_exPV, &b_mu_ie_qoverp_exPV);
   fChain->SetBranchAddress("mu_cov_d0_exPV", &mu_cov_d0_exPV, &b_mu_cov_d0_exPV);
   fChain->SetBranchAddress("mu_cov_z0_exPV", &mu_cov_z0_exPV, &b_mu_cov_z0_exPV);
   fChain->SetBranchAddress("mu_cov_phi_exPV", &mu_cov_phi_exPV, &b_mu_cov_phi_exPV);
   fChain->SetBranchAddress("mu_cov_theta_exPV", &mu_cov_theta_exPV, &b_mu_cov_theta_exPV);
   fChain->SetBranchAddress("mu_cov_qoverp_exPV", &mu_cov_qoverp_exPV, &b_mu_cov_qoverp_exPV);
   fChain->SetBranchAddress("mu_cov_d0_z0_exPV", &mu_cov_d0_z0_exPV, &b_mu_cov_d0_z0_exPV);
   fChain->SetBranchAddress("mu_cov_d0_phi_exPV", &mu_cov_d0_phi_exPV, &b_mu_cov_d0_phi_exPV);
   fChain->SetBranchAddress("mu_cov_d0_theta_exPV", &mu_cov_d0_theta_exPV, &b_mu_cov_d0_theta_exPV);
   fChain->SetBranchAddress("mu_cov_d0_qoverp_exPV", &mu_cov_d0_qoverp_exPV, &b_mu_cov_d0_qoverp_exPV);
   fChain->SetBranchAddress("mu_cov_z0_phi_exPV", &mu_cov_z0_phi_exPV, &b_mu_cov_z0_phi_exPV);
   fChain->SetBranchAddress("mu_cov_z0_theta_exPV", &mu_cov_z0_theta_exPV, &b_mu_cov_z0_theta_exPV);
   fChain->SetBranchAddress("mu_cov_z0_qoverp_exPV", &mu_cov_z0_qoverp_exPV, &b_mu_cov_z0_qoverp_exPV);
   fChain->SetBranchAddress("mu_cov_phi_theta_exPV", &mu_cov_phi_theta_exPV, &b_mu_cov_phi_theta_exPV);
   fChain->SetBranchAddress("mu_cov_phi_qoverp_exPV", &mu_cov_phi_qoverp_exPV, &b_mu_cov_phi_qoverp_exPV);
   fChain->SetBranchAddress("mu_cov_theta_qoverp_exPV", &mu_cov_theta_qoverp_exPV, &b_mu_cov_theta_qoverp_exPV);
   fChain->SetBranchAddress("mu_ms_d0", &mu_ms_d0, &b_mu_ms_d0);
   fChain->SetBranchAddress("mu_ms_z0", &mu_ms_z0, &b_mu_ms_z0);
   fChain->SetBranchAddress("mu_ms_phi", &mu_ms_phi, &b_mu_ms_phi);
   fChain->SetBranchAddress("mu_ms_theta", &mu_ms_theta, &b_mu_ms_theta);
   fChain->SetBranchAddress("mu_ms_qoverp", &mu_ms_qoverp, &b_mu_ms_qoverp);
   fChain->SetBranchAddress("mu_id_d0", &mu_id_d0, &b_mu_id_d0);
   fChain->SetBranchAddress("mu_id_z0", &mu_id_z0, &b_mu_id_z0);
   fChain->SetBranchAddress("mu_id_phi", &mu_id_phi, &b_mu_id_phi);
   fChain->SetBranchAddress("mu_id_theta", &mu_id_theta, &b_mu_id_theta);
   fChain->SetBranchAddress("mu_id_qoverp", &mu_id_qoverp, &b_mu_id_qoverp);
   fChain->SetBranchAddress("mu_me_d0", &mu_me_d0, &b_mu_me_d0);
   fChain->SetBranchAddress("mu_me_z0", &mu_me_z0, &b_mu_me_z0);
   fChain->SetBranchAddress("mu_me_phi", &mu_me_phi, &b_mu_me_phi);
   fChain->SetBranchAddress("mu_me_theta", &mu_me_theta, &b_mu_me_theta);
   fChain->SetBranchAddress("mu_me_qoverp", &mu_me_qoverp, &b_mu_me_qoverp);
   fChain->SetBranchAddress("mu_ie_d0", &mu_ie_d0, &b_mu_ie_d0);
   fChain->SetBranchAddress("mu_ie_z0", &mu_ie_z0, &b_mu_ie_z0);
   fChain->SetBranchAddress("mu_ie_phi", &mu_ie_phi, &b_mu_ie_phi);
   fChain->SetBranchAddress("mu_ie_theta", &mu_ie_theta, &b_mu_ie_theta);
   fChain->SetBranchAddress("mu_ie_qoverp", &mu_ie_qoverp, &b_mu_ie_qoverp);
   fChain->SetBranchAddress("mu_nBLHits", &mu_nBLHits, &b_mu_nBLHits);
   fChain->SetBranchAddress("mu_nPixHits", &mu_nPixHits, &b_mu_nPixHits);
   fChain->SetBranchAddress("mu_nSCTHits", &mu_nSCTHits, &b_mu_nSCTHits);
   fChain->SetBranchAddress("mu_nTRTHits", &mu_nTRTHits, &b_mu_nTRTHits);
   fChain->SetBranchAddress("mu_nTRTHighTHits", &mu_nTRTHighTHits, &b_mu_nTRTHighTHits);
   fChain->SetBranchAddress("mu_nBLSharedHits", &mu_nBLSharedHits, &b_mu_nBLSharedHits);
   fChain->SetBranchAddress("mu_nPixSharedHits", &mu_nPixSharedHits, &b_mu_nPixSharedHits);
   fChain->SetBranchAddress("mu_nPixHoles", &mu_nPixHoles, &b_mu_nPixHoles);
   fChain->SetBranchAddress("mu_nSCTSharedHits", &mu_nSCTSharedHits, &b_mu_nSCTSharedHits);
   fChain->SetBranchAddress("mu_nSCTHoles", &mu_nSCTHoles, &b_mu_nSCTHoles);
   fChain->SetBranchAddress("mu_nTRTOutliers", &mu_nTRTOutliers, &b_mu_nTRTOutliers);
   fChain->SetBranchAddress("mu_nTRTHighTOutliers", &mu_nTRTHighTOutliers, &b_mu_nTRTHighTOutliers);
   fChain->SetBranchAddress("mu_nMDTHits", &mu_nMDTHits, &b_mu_nMDTHits);
   fChain->SetBranchAddress("mu_nMDTHoles", &mu_nMDTHoles, &b_mu_nMDTHoles);
   fChain->SetBranchAddress("mu_nCSCEtaHits", &mu_nCSCEtaHits, &b_mu_nCSCEtaHits);
   fChain->SetBranchAddress("mu_nCSCEtaHoles", &mu_nCSCEtaHoles, &b_mu_nCSCEtaHoles);
   fChain->SetBranchAddress("mu_nCSCPhiHits", &mu_nCSCPhiHits, &b_mu_nCSCPhiHits);
   fChain->SetBranchAddress("mu_nCSCPhiHoles", &mu_nCSCPhiHoles, &b_mu_nCSCPhiHoles);
   fChain->SetBranchAddress("mu_nRPCEtaHits", &mu_nRPCEtaHits, &b_mu_nRPCEtaHits);
   fChain->SetBranchAddress("mu_nRPCEtaHoles", &mu_nRPCEtaHoles, &b_mu_nRPCEtaHoles);
   fChain->SetBranchAddress("mu_nRPCPhiHits", &mu_nRPCPhiHits, &b_mu_nRPCPhiHits);
   fChain->SetBranchAddress("mu_nRPCPhiHoles", &mu_nRPCPhiHoles, &b_mu_nRPCPhiHoles);
   fChain->SetBranchAddress("mu_nTGCEtaHits", &mu_nTGCEtaHits, &b_mu_nTGCEtaHits);
   fChain->SetBranchAddress("mu_nTGCEtaHoles", &mu_nTGCEtaHoles, &b_mu_nTGCEtaHoles);
   fChain->SetBranchAddress("mu_nTGCPhiHits", &mu_nTGCPhiHits, &b_mu_nTGCPhiHits);
   fChain->SetBranchAddress("mu_nTGCPhiHoles", &mu_nTGCPhiHoles, &b_mu_nTGCPhiHoles);
   fChain->SetBranchAddress("mu_nGangedPixels", &mu_nGangedPixels, &b_mu_nGangedPixels);
   fChain->SetBranchAddress("mu_nOutliersOnTrack", &mu_nOutliersOnTrack, &b_mu_nOutliersOnTrack);
   fChain->SetBranchAddress("mu_nMDTBIHits", &mu_nMDTBIHits, &b_mu_nMDTBIHits);
   fChain->SetBranchAddress("mu_nMDTBMHits", &mu_nMDTBMHits, &b_mu_nMDTBMHits);
   fChain->SetBranchAddress("mu_nMDTBOHits", &mu_nMDTBOHits, &b_mu_nMDTBOHits);
   fChain->SetBranchAddress("mu_nMDTBEEHits", &mu_nMDTBEEHits, &b_mu_nMDTBEEHits);
   fChain->SetBranchAddress("mu_nMDTBIS78Hits", &mu_nMDTBIS78Hits, &b_mu_nMDTBIS78Hits);
   fChain->SetBranchAddress("mu_nMDTEIHits", &mu_nMDTEIHits, &b_mu_nMDTEIHits);
   fChain->SetBranchAddress("mu_nMDTEMHits", &mu_nMDTEMHits, &b_mu_nMDTEMHits);
   fChain->SetBranchAddress("mu_nMDTEOHits", &mu_nMDTEOHits, &b_mu_nMDTEOHits);
   fChain->SetBranchAddress("mu_nMDTEEHits", &mu_nMDTEEHits, &b_mu_nMDTEEHits);
   fChain->SetBranchAddress("mu_nRPCLayer1EtaHits", &mu_nRPCLayer1EtaHits, &b_mu_nRPCLayer1EtaHits);
   fChain->SetBranchAddress("mu_nRPCLayer2EtaHits", &mu_nRPCLayer2EtaHits, &b_mu_nRPCLayer2EtaHits);
   fChain->SetBranchAddress("mu_nRPCLayer3EtaHits", &mu_nRPCLayer3EtaHits, &b_mu_nRPCLayer3EtaHits);
   fChain->SetBranchAddress("mu_nRPCLayer1PhiHits", &mu_nRPCLayer1PhiHits, &b_mu_nRPCLayer1PhiHits);
   fChain->SetBranchAddress("mu_nRPCLayer2PhiHits", &mu_nRPCLayer2PhiHits, &b_mu_nRPCLayer2PhiHits);
   fChain->SetBranchAddress("mu_nRPCLayer3PhiHits", &mu_nRPCLayer3PhiHits, &b_mu_nRPCLayer3PhiHits);
   fChain->SetBranchAddress("mu_nTGCLayer1EtaHits", &mu_nTGCLayer1EtaHits, &b_mu_nTGCLayer1EtaHits);
   fChain->SetBranchAddress("mu_nTGCLayer2EtaHits", &mu_nTGCLayer2EtaHits, &b_mu_nTGCLayer2EtaHits);
   fChain->SetBranchAddress("mu_nTGCLayer3EtaHits", &mu_nTGCLayer3EtaHits, &b_mu_nTGCLayer3EtaHits);
   fChain->SetBranchAddress("mu_nTGCLayer4EtaHits", &mu_nTGCLayer4EtaHits, &b_mu_nTGCLayer4EtaHits);
   fChain->SetBranchAddress("mu_nTGCLayer1PhiHits", &mu_nTGCLayer1PhiHits, &b_mu_nTGCLayer1PhiHits);
   fChain->SetBranchAddress("mu_nTGCLayer2PhiHits", &mu_nTGCLayer2PhiHits, &b_mu_nTGCLayer2PhiHits);
   fChain->SetBranchAddress("mu_nTGCLayer3PhiHits", &mu_nTGCLayer3PhiHits, &b_mu_nTGCLayer3PhiHits);
   fChain->SetBranchAddress("mu_nTGCLayer4PhiHits", &mu_nTGCLayer4PhiHits, &b_mu_nTGCLayer4PhiHits);
   fChain->SetBranchAddress("mu_barrelSectors", &mu_barrelSectors, &b_mu_barrelSectors);
   fChain->SetBranchAddress("mu_endcapSectors", &mu_endcapSectors, &b_mu_endcapSectors);
   fChain->SetBranchAddress("mu_trackd0", &mu_trackd0, &b_mu_trackd0);
   fChain->SetBranchAddress("mu_trackz0", &mu_trackz0, &b_mu_trackz0);
   fChain->SetBranchAddress("mu_trackphi", &mu_trackphi, &b_mu_trackphi);
   fChain->SetBranchAddress("mu_tracktheta", &mu_tracktheta, &b_mu_tracktheta);
   fChain->SetBranchAddress("mu_trackqoverp", &mu_trackqoverp, &b_mu_trackqoverp);
   fChain->SetBranchAddress("mu_trackcov_d0", &mu_trackcov_d0, &b_mu_trackcov_d0);
   fChain->SetBranchAddress("mu_trackcov_z0", &mu_trackcov_z0, &b_mu_trackcov_z0);
   fChain->SetBranchAddress("mu_trackcov_phi", &mu_trackcov_phi, &b_mu_trackcov_phi);
   fChain->SetBranchAddress("mu_trackcov_theta", &mu_trackcov_theta, &b_mu_trackcov_theta);
   fChain->SetBranchAddress("mu_trackcov_qoverp", &mu_trackcov_qoverp, &b_mu_trackcov_qoverp);
   fChain->SetBranchAddress("mu_trackcov_d0_z0", &mu_trackcov_d0_z0, &b_mu_trackcov_d0_z0);
   fChain->SetBranchAddress("mu_trackcov_d0_phi", &mu_trackcov_d0_phi, &b_mu_trackcov_d0_phi);
   fChain->SetBranchAddress("mu_trackcov_d0_theta", &mu_trackcov_d0_theta, &b_mu_trackcov_d0_theta);
   fChain->SetBranchAddress("mu_trackcov_d0_qoverp", &mu_trackcov_d0_qoverp, &b_mu_trackcov_d0_qoverp);
   fChain->SetBranchAddress("mu_trackcov_z0_phi", &mu_trackcov_z0_phi, &b_mu_trackcov_z0_phi);
   fChain->SetBranchAddress("mu_trackcov_z0_theta", &mu_trackcov_z0_theta, &b_mu_trackcov_z0_theta);
   fChain->SetBranchAddress("mu_trackcov_z0_qoverp", &mu_trackcov_z0_qoverp, &b_mu_trackcov_z0_qoverp);
   fChain->SetBranchAddress("mu_trackcov_phi_theta", &mu_trackcov_phi_theta, &b_mu_trackcov_phi_theta);
   fChain->SetBranchAddress("mu_trackcov_phi_qoverp", &mu_trackcov_phi_qoverp, &b_mu_trackcov_phi_qoverp);
   fChain->SetBranchAddress("mu_trackcov_theta_qoverp", &mu_trackcov_theta_qoverp, &b_mu_trackcov_theta_qoverp);
   fChain->SetBranchAddress("mu_trackfitchi2", &mu_trackfitchi2, &b_mu_trackfitchi2);
   fChain->SetBranchAddress("mu_trackfitndof", &mu_trackfitndof, &b_mu_trackfitndof);
   fChain->SetBranchAddress("mu_hastrack", &mu_hastrack, &b_mu_hastrack);
   fChain->SetBranchAddress("mu_truth_dr", &mu_truth_dr, &b_mu_truth_dr);
   fChain->SetBranchAddress("mu_truth_E", &mu_truth_E, &b_mu_truth_E);
   fChain->SetBranchAddress("mu_truth_pt", &mu_truth_pt, &b_mu_truth_pt);
   fChain->SetBranchAddress("mu_truth_eta", &mu_truth_eta, &b_mu_truth_eta);
   fChain->SetBranchAddress("mu_truth_phi", &mu_truth_phi, &b_mu_truth_phi);
   fChain->SetBranchAddress("mu_truth_type", &mu_truth_type, &b_mu_truth_type);
   fChain->SetBranchAddress("mu_truth_status", &mu_truth_status, &b_mu_truth_status);
   fChain->SetBranchAddress("mu_truth_barcode", &mu_truth_barcode, &b_mu_truth_barcode);
   fChain->SetBranchAddress("mu_truth_mothertype", &mu_truth_mothertype, &b_mu_truth_mothertype);
   fChain->SetBranchAddress("mu_truth_motherbarcode", &mu_truth_motherbarcode, &b_mu_truth_motherbarcode);
   fChain->SetBranchAddress("mu_truth_matched", &mu_truth_matched, &b_mu_truth_matched);
   fChain->SetBranchAddress("mu_EF_dr", &mu_EF_dr, &b_mu_EF_dr);
   fChain->SetBranchAddress("mu_EF_cb_eta", &mu_EF_cb_eta, &b_mu_EF_cb_eta);
   fChain->SetBranchAddress("mu_EF_cb_phi", &mu_EF_cb_phi, &b_mu_EF_cb_phi);
   fChain->SetBranchAddress("mu_EF_cb_pt", &mu_EF_cb_pt, &b_mu_EF_cb_pt);
   fChain->SetBranchAddress("mu_EF_ms_eta", &mu_EF_ms_eta, &b_mu_EF_ms_eta);
   fChain->SetBranchAddress("mu_EF_ms_phi", &mu_EF_ms_phi, &b_mu_EF_ms_phi);
   fChain->SetBranchAddress("mu_EF_ms_pt", &mu_EF_ms_pt, &b_mu_EF_ms_pt);
   fChain->SetBranchAddress("mu_EF_me_eta", &mu_EF_me_eta, &b_mu_EF_me_eta);
   fChain->SetBranchAddress("mu_EF_me_phi", &mu_EF_me_phi, &b_mu_EF_me_phi);
   fChain->SetBranchAddress("mu_EF_me_pt", &mu_EF_me_pt, &b_mu_EF_me_pt);
   fChain->SetBranchAddress("mu_EF_matched", &mu_EF_matched, &b_mu_EF_matched);
   fChain->SetBranchAddress("mu_L2CB_dr", &mu_L2CB_dr, &b_mu_L2CB_dr);
   fChain->SetBranchAddress("mu_L2CB_pt", &mu_L2CB_pt, &b_mu_L2CB_pt);
   fChain->SetBranchAddress("mu_L2CB_eta", &mu_L2CB_eta, &b_mu_L2CB_eta);
   fChain->SetBranchAddress("mu_L2CB_phi", &mu_L2CB_phi, &b_mu_L2CB_phi);
   fChain->SetBranchAddress("mu_L2CB_id_pt", &mu_L2CB_id_pt, &b_mu_L2CB_id_pt);
   fChain->SetBranchAddress("mu_L2CB_ms_pt", &mu_L2CB_ms_pt, &b_mu_L2CB_ms_pt);
   fChain->SetBranchAddress("mu_L2CB_nPixHits", &mu_L2CB_nPixHits, &b_mu_L2CB_nPixHits);
   fChain->SetBranchAddress("mu_L2CB_nSCTHits", &mu_L2CB_nSCTHits, &b_mu_L2CB_nSCTHits);
   fChain->SetBranchAddress("mu_L2CB_nTRTHits", &mu_L2CB_nTRTHits, &b_mu_L2CB_nTRTHits);
   fChain->SetBranchAddress("mu_L2CB_nTRTHighTHits", &mu_L2CB_nTRTHighTHits, &b_mu_L2CB_nTRTHighTHits);
   fChain->SetBranchAddress("mu_L2CB_matched", &mu_L2CB_matched, &b_mu_L2CB_matched);
   fChain->SetBranchAddress("mu_L1_dr", &mu_L1_dr, &b_mu_L1_dr);
   fChain->SetBranchAddress("mu_L1_pt", &mu_L1_pt, &b_mu_L1_pt);
   fChain->SetBranchAddress("mu_L1_eta", &mu_L1_eta, &b_mu_L1_eta);
   fChain->SetBranchAddress("mu_L1_phi", &mu_L1_phi, &b_mu_L1_phi);
   fChain->SetBranchAddress("mu_L1_thrNumber", &mu_L1_thrNumber, &b_mu_L1_thrNumber);
   fChain->SetBranchAddress("mu_L1_RoINumber", &mu_L1_RoINumber, &b_mu_L1_RoINumber);
   fChain->SetBranchAddress("mu_L1_sectorAddress", &mu_L1_sectorAddress, &b_mu_L1_sectorAddress);
   fChain->SetBranchAddress("mu_L1_firstCandidate", &mu_L1_firstCandidate, &b_mu_L1_firstCandidate);
   fChain->SetBranchAddress("mu_L1_moreCandInRoI", &mu_L1_moreCandInRoI, &b_mu_L1_moreCandInRoI);
   fChain->SetBranchAddress("mu_L1_moreCandInSector", &mu_L1_moreCandInSector, &b_mu_L1_moreCandInSector);
   fChain->SetBranchAddress("mu_L1_source", &mu_L1_source, &b_mu_L1_source);
   fChain->SetBranchAddress("mu_L1_hemisphere", &mu_L1_hemisphere, &b_mu_L1_hemisphere);
   fChain->SetBranchAddress("mu_L1_matched", &mu_L1_matched, &b_mu_L1_matched);
   fChain->SetBranchAddress("trig_L1_emtau_n", &trig_L1_emtau_n, &b_trig_L1_emtau_n);
   fChain->SetBranchAddress("trig_L1_emtau_eta", &trig_L1_emtau_eta, &b_trig_L1_emtau_eta);
   fChain->SetBranchAddress("trig_L1_emtau_phi", &trig_L1_emtau_phi, &b_trig_L1_emtau_phi);
   fChain->SetBranchAddress("trig_L1_emtau_thrNames", &trig_L1_emtau_thrNames, &b_trig_L1_emtau_thrNames);
   fChain->SetBranchAddress("trig_L1_emtau_thrValues", &trig_L1_emtau_thrValues, &b_trig_L1_emtau_thrValues);
   fChain->SetBranchAddress("trig_L1_emtau_core", &trig_L1_emtau_core, &b_trig_L1_emtau_core);
   fChain->SetBranchAddress("trig_L1_emtau_EMClus", &trig_L1_emtau_EMClus, &b_trig_L1_emtau_EMClus);
   fChain->SetBranchAddress("trig_L1_emtau_tauClus", &trig_L1_emtau_tauClus, &b_trig_L1_emtau_tauClus);
   fChain->SetBranchAddress("trig_L1_emtau_EMIsol", &trig_L1_emtau_EMIsol, &b_trig_L1_emtau_EMIsol);
   fChain->SetBranchAddress("trig_L1_emtau_hadIsol", &trig_L1_emtau_hadIsol, &b_trig_L1_emtau_hadIsol);
   fChain->SetBranchAddress("trig_L1_emtau_hadCore", &trig_L1_emtau_hadCore, &b_trig_L1_emtau_hadCore);
   fChain->SetBranchAddress("trig_L1_emtau_thrPattern", &trig_L1_emtau_thrPattern, &b_trig_L1_emtau_thrPattern);
   fChain->SetBranchAddress("trig_L1_emtau_L1_2EM10", &trig_L1_emtau_L1_2EM10, &b_trig_L1_emtau_L1_2EM10);
   fChain->SetBranchAddress("trig_L1_emtau_L1_2EM10I", &trig_L1_emtau_L1_2EM10I, &b_trig_L1_emtau_L1_2EM10I);
   fChain->SetBranchAddress("trig_L1_emtau_L1_2EM14", &trig_L1_emtau_L1_2EM14, &b_trig_L1_emtau_L1_2EM14);
   fChain->SetBranchAddress("trig_L1_emtau_L1_2EM3", &trig_L1_emtau_L1_2EM3, &b_trig_L1_emtau_L1_2EM3);
   fChain->SetBranchAddress("trig_L1_emtau_L1_2EM3_EM5", &trig_L1_emtau_L1_2EM3_EM5, &b_trig_L1_emtau_L1_2EM3_EM5);
   fChain->SetBranchAddress("trig_L1_emtau_L1_2EM5", &trig_L1_emtau_L1_2EM5, &b_trig_L1_emtau_L1_2EM5);
   fChain->SetBranchAddress("trig_L1_emtau_L1_2EM5_MU6", &trig_L1_emtau_L1_2EM5_MU6, &b_trig_L1_emtau_L1_2EM5_MU6);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM10", &trig_L1_emtau_L1_EM10, &b_trig_L1_emtau_L1_EM10);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM10I", &trig_L1_emtau_L1_EM10I, &b_trig_L1_emtau_L1_EM10I);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM14", &trig_L1_emtau_L1_EM14, &b_trig_L1_emtau_L1_EM14);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM14I", &trig_L1_emtau_L1_EM14I, &b_trig_L1_emtau_L1_EM14I);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM14_XE10", &trig_L1_emtau_L1_EM14_XE10, &b_trig_L1_emtau_L1_EM14_XE10);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM14_XE15", &trig_L1_emtau_L1_EM14_XE15, &b_trig_L1_emtau_L1_EM14_XE15);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM19I", &trig_L1_emtau_L1_EM19I, &b_trig_L1_emtau_L1_EM19I);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM3", &trig_L1_emtau_L1_EM3, &b_trig_L1_emtau_L1_EM3);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM3_MV", &trig_L1_emtau_L1_EM3_MV, &b_trig_L1_emtau_L1_EM3_MV);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM5", &trig_L1_emtau_L1_EM5, &b_trig_L1_emtau_L1_EM5);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM5_MU6", &trig_L1_emtau_L1_EM5_MU6, &b_trig_L1_emtau_L1_EM5_MU6);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM85", &trig_L1_emtau_L1_EM85, &b_trig_L1_emtau_L1_EM85);
   fChain->SetBranchAddress("trig_L1_emtau_RoIWord", &trig_L1_emtau_RoIWord, &b_trig_L1_emtau_RoIWord);
   fChain->SetBranchAddress("trig_L2_emcl_n", &trig_L2_emcl_n, &b_trig_L2_emcl_n);
   fChain->SetBranchAddress("trig_L2_emcl_quality", &trig_L2_emcl_quality, &b_trig_L2_emcl_quality);
   fChain->SetBranchAddress("trig_L2_emcl_E", &trig_L2_emcl_E, &b_trig_L2_emcl_E);
   fChain->SetBranchAddress("trig_L2_emcl_Et", &trig_L2_emcl_Et, &b_trig_L2_emcl_Et);
   fChain->SetBranchAddress("trig_L2_emcl_eta", &trig_L2_emcl_eta, &b_trig_L2_emcl_eta);
   fChain->SetBranchAddress("trig_L2_emcl_phi", &trig_L2_emcl_phi, &b_trig_L2_emcl_phi);
   fChain->SetBranchAddress("trig_L2_emcl_E237", &trig_L2_emcl_E237, &b_trig_L2_emcl_E237);
   fChain->SetBranchAddress("trig_L2_emcl_E277", &trig_L2_emcl_E277, &b_trig_L2_emcl_E277);
   fChain->SetBranchAddress("trig_L2_emcl_fracs1", &trig_L2_emcl_fracs1, &b_trig_L2_emcl_fracs1);
   fChain->SetBranchAddress("trig_L2_emcl_weta2", &trig_L2_emcl_weta2, &b_trig_L2_emcl_weta2);
   fChain->SetBranchAddress("trig_L2_emcl_Ehad1", &trig_L2_emcl_Ehad1, &b_trig_L2_emcl_Ehad1);
   fChain->SetBranchAddress("trig_L2_emcl_eta1", &trig_L2_emcl_eta1, &b_trig_L2_emcl_eta1);
   fChain->SetBranchAddress("trig_L2_emcl_emaxs1", &trig_L2_emcl_emaxs1, &b_trig_L2_emcl_emaxs1);
   fChain->SetBranchAddress("trig_L2_emcl_e2tsts1", &trig_L2_emcl_e2tsts1, &b_trig_L2_emcl_e2tsts1);
   fChain->SetBranchAddress("trig_L2_emcl_rawEnergy", &trig_L2_emcl_rawEnergy, &b_trig_L2_emcl_rawEnergy);
   fChain->SetBranchAddress("trig_L2_emcl_rawEta", &trig_L2_emcl_rawEta, &b_trig_L2_emcl_rawEta);
   fChain->SetBranchAddress("trig_L2_emcl_rawPhi", &trig_L2_emcl_rawPhi, &b_trig_L2_emcl_rawPhi);
   fChain->SetBranchAddress("trig_L2_emcl_RoIWord", &trig_L2_emcl_RoIWord, &b_trig_L2_emcl_RoIWord);
   fChain->SetBranchAddress("trig_L2_emcl_nCells", &trig_L2_emcl_nCells, &b_trig_L2_emcl_nCells);
   fChain->SetBranchAddress("trig_L2_emcl_energyInSample", &trig_L2_emcl_energyInSample, &b_trig_L2_emcl_energyInSample);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_n", &trig_L2_trk_idscan_eGamma_n, &b_trig_L2_trk_idscan_eGamma_n);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_algorithmId", &trig_L2_trk_idscan_eGamma_algorithmId, &b_trig_L2_trk_idscan_eGamma_algorithmId);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_trackStatus", &trig_L2_trk_idscan_eGamma_trackStatus, &b_trig_L2_trk_idscan_eGamma_trackStatus);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_chi2Ndof", &trig_L2_trk_idscan_eGamma_chi2Ndof, &b_trig_L2_trk_idscan_eGamma_chi2Ndof);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_nStrawHits", &trig_L2_trk_idscan_eGamma_nStrawHits, &b_trig_L2_trk_idscan_eGamma_nStrawHits);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_nHighThrHits", &trig_L2_trk_idscan_eGamma_nHighThrHits, &b_trig_L2_trk_idscan_eGamma_nHighThrHits);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_nPixelSpacePoints", &trig_L2_trk_idscan_eGamma_nPixelSpacePoints, &b_trig_L2_trk_idscan_eGamma_nPixelSpacePoints);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_nSCT_SpacePoints", &trig_L2_trk_idscan_eGamma_nSCT_SpacePoints, &b_trig_L2_trk_idscan_eGamma_nSCT_SpacePoints);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_hitPattern", &trig_L2_trk_idscan_eGamma_hitPattern, &b_trig_L2_trk_idscan_eGamma_hitPattern);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_nStraw", &trig_L2_trk_idscan_eGamma_nStraw, &b_trig_L2_trk_idscan_eGamma_nStraw);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_nStrawTime", &trig_L2_trk_idscan_eGamma_nStrawTime, &b_trig_L2_trk_idscan_eGamma_nStrawTime);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_a0", &trig_L2_trk_idscan_eGamma_a0, &b_trig_L2_trk_idscan_eGamma_a0);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_z0", &trig_L2_trk_idscan_eGamma_z0, &b_trig_L2_trk_idscan_eGamma_z0);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_phi0", &trig_L2_trk_idscan_eGamma_phi0, &b_trig_L2_trk_idscan_eGamma_phi0);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_eta", &trig_L2_trk_idscan_eGamma_eta, &b_trig_L2_trk_idscan_eGamma_eta);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_pt", &trig_L2_trk_idscan_eGamma_pt, &b_trig_L2_trk_idscan_eGamma_pt);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_covariance", &trig_L2_trk_idscan_eGamma_covariance, &b_trig_L2_trk_idscan_eGamma_covariance);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_n", &trig_L2_trk_sitrack_eGamma_n, &b_trig_L2_trk_sitrack_eGamma_n);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_algorithmId", &trig_L2_trk_sitrack_eGamma_algorithmId, &b_trig_L2_trk_sitrack_eGamma_algorithmId);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_trackStatus", &trig_L2_trk_sitrack_eGamma_trackStatus, &b_trig_L2_trk_sitrack_eGamma_trackStatus);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_chi2Ndof", &trig_L2_trk_sitrack_eGamma_chi2Ndof, &b_trig_L2_trk_sitrack_eGamma_chi2Ndof);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_nStrawHits", &trig_L2_trk_sitrack_eGamma_nStrawHits, &b_trig_L2_trk_sitrack_eGamma_nStrawHits);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_nHighThrHits", &trig_L2_trk_sitrack_eGamma_nHighThrHits, &b_trig_L2_trk_sitrack_eGamma_nHighThrHits);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_nPixelSpacePoints", &trig_L2_trk_sitrack_eGamma_nPixelSpacePoints, &b_trig_L2_trk_sitrack_eGamma_nPixelSpacePoints);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints", &trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints, &b_trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_hitPattern", &trig_L2_trk_sitrack_eGamma_hitPattern, &b_trig_L2_trk_sitrack_eGamma_hitPattern);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_nStraw", &trig_L2_trk_sitrack_eGamma_nStraw, &b_trig_L2_trk_sitrack_eGamma_nStraw);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_nStrawTime", &trig_L2_trk_sitrack_eGamma_nStrawTime, &b_trig_L2_trk_sitrack_eGamma_nStrawTime);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_a0", &trig_L2_trk_sitrack_eGamma_a0, &b_trig_L2_trk_sitrack_eGamma_a0);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_z0", &trig_L2_trk_sitrack_eGamma_z0, &b_trig_L2_trk_sitrack_eGamma_z0);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_phi0", &trig_L2_trk_sitrack_eGamma_phi0, &b_trig_L2_trk_sitrack_eGamma_phi0);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_eta", &trig_L2_trk_sitrack_eGamma_eta, &b_trig_L2_trk_sitrack_eGamma_eta);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_pt", &trig_L2_trk_sitrack_eGamma_pt, &b_trig_L2_trk_sitrack_eGamma_pt);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_covariance", &trig_L2_trk_sitrack_eGamma_covariance, &b_trig_L2_trk_sitrack_eGamma_covariance);
   fChain->SetBranchAddress("trig_L2_el_n", &trig_L2_el_n, &b_trig_L2_el_n);
   fChain->SetBranchAddress("trig_L2_el_E", &trig_L2_el_E, &b_trig_L2_el_E);
   fChain->SetBranchAddress("trig_L2_el_Et", &trig_L2_el_Et, &b_trig_L2_el_Et);
   fChain->SetBranchAddress("trig_L2_el_pt", &trig_L2_el_pt, &b_trig_L2_el_pt);
   fChain->SetBranchAddress("trig_L2_el_eta", &trig_L2_el_eta, &b_trig_L2_el_eta);
   fChain->SetBranchAddress("trig_L2_el_phi", &trig_L2_el_phi, &b_trig_L2_el_phi);
   fChain->SetBranchAddress("trig_L2_el_RoIWord", &trig_L2_el_RoIWord, &b_trig_L2_el_RoIWord);
   fChain->SetBranchAddress("trig_L2_el_zvertex", &trig_L2_el_zvertex, &b_trig_L2_el_zvertex);
   fChain->SetBranchAddress("trig_L2_el_charge", &trig_L2_el_charge, &b_trig_L2_el_charge);
   fChain->SetBranchAddress("trig_L2_el_trackAlgo", &trig_L2_el_trackAlgo, &b_trig_L2_el_trackAlgo);
   fChain->SetBranchAddress("trig_L2_el_TRTHighTHitsRatio", &trig_L2_el_TRTHighTHitsRatio, &b_trig_L2_el_TRTHighTHitsRatio);
   fChain->SetBranchAddress("trig_L2_el_deltaeta1", &trig_L2_el_deltaeta1, &b_trig_L2_el_deltaeta1);
   fChain->SetBranchAddress("trig_L2_el_deltaphi2", &trig_L2_el_deltaphi2, &b_trig_L2_el_deltaphi2);
   fChain->SetBranchAddress("trig_L2_el_EtOverPt", &trig_L2_el_EtOverPt, &b_trig_L2_el_EtOverPt);
   fChain->SetBranchAddress("trig_L2_el_reta", &trig_L2_el_reta, &b_trig_L2_el_reta);
   fChain->SetBranchAddress("trig_L2_el_Eratio", &trig_L2_el_Eratio, &b_trig_L2_el_Eratio);
   fChain->SetBranchAddress("trig_L2_el_Ethad1", &trig_L2_el_Ethad1, &b_trig_L2_el_Ethad1);
   fChain->SetBranchAddress("trig_L2_el_L2_2e12_tight", &trig_L2_el_L2_2e12_tight, &b_trig_L2_el_L2_2e12_tight);
   fChain->SetBranchAddress("trig_L2_el_L2_2e15_medium", &trig_L2_el_L2_2e15_medium, &b_trig_L2_el_L2_2e15_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_2e20_loose", &trig_L2_el_L2_2e20_loose, &b_trig_L2_el_L2_2e20_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_2e5_medium", &trig_L2_el_L2_2e5_medium, &b_trig_L2_el_L2_2e5_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_2e5_medium1", &trig_L2_el_L2_2e5_medium1, &b_trig_L2_el_L2_2e5_medium1);
   fChain->SetBranchAddress("trig_L2_el_L2_e105_loose1", &trig_L2_el_L2_e105_loose1, &b_trig_L2_el_L2_e105_loose1);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_NoCut", &trig_L2_el_L2_e10_NoCut, &b_trig_L2_el_L2_e10_NoCut);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_loose", &trig_L2_el_L2_e10_loose, &b_trig_L2_el_L2_e10_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_loose_IdScan", &trig_L2_el_L2_e10_loose_IdScan, &b_trig_L2_el_L2_e10_loose_IdScan);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_loose_SiTrk", &trig_L2_el_L2_e10_loose_SiTrk, &b_trig_L2_el_L2_e10_loose_SiTrk);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_loose_TRT", &trig_L2_el_L2_e10_loose_TRT, &b_trig_L2_el_L2_e10_loose_TRT);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_loose_mu6", &trig_L2_el_L2_e10_loose_mu6, &b_trig_L2_el_L2_e10_loose_mu6);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_medium", &trig_L2_el_L2_e10_medium, &b_trig_L2_el_L2_e10_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_medium_SiTrk", &trig_L2_el_L2_e10_medium_SiTrk, &b_trig_L2_el_L2_e10_medium_SiTrk);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_medium_SiTrk_robust", &trig_L2_el_L2_e10_medium_SiTrk_robust, &b_trig_L2_el_L2_e10_medium_SiTrk_robust);
   fChain->SetBranchAddress("trig_L2_el_L2_e12_medium", &trig_L2_el_L2_e12_medium, &b_trig_L2_el_L2_e12_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_e15_medium", &trig_L2_el_L2_e15_medium, &b_trig_L2_el_L2_e15_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_e15i_medium", &trig_L2_el_L2_e15i_medium, &b_trig_L2_el_L2_e15i_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_loose", &trig_L2_el_L2_e20_loose, &b_trig_L2_el_L2_e20_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_loose_IdScan", &trig_L2_el_L2_e20_loose_IdScan, &b_trig_L2_el_L2_e20_loose_IdScan);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_loose_NoIDTrkCut", &trig_L2_el_L2_e20_loose_NoIDTrkCut, &b_trig_L2_el_L2_e20_loose_NoIDTrkCut);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_loose_SiTrk", &trig_L2_el_L2_e20_loose_SiTrk, &b_trig_L2_el_L2_e20_loose_SiTrk);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_loose_SiTrk_robust", &trig_L2_el_L2_e20_loose_SiTrk_robust, &b_trig_L2_el_L2_e20_loose_SiTrk_robust);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_loose_TRT", &trig_L2_el_L2_e20_loose_TRT, &b_trig_L2_el_L2_e20_loose_TRT);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_loose_passEF", &trig_L2_el_L2_e20_loose_passEF, &b_trig_L2_el_L2_e20_loose_passEF);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_loose_passL2", &trig_L2_el_L2_e20_loose_passL2, &b_trig_L2_el_L2_e20_loose_passL2);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_loose_xe20", &trig_L2_el_L2_e20_loose_xe20, &b_trig_L2_el_L2_e20_loose_xe20);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_loose_xe30", &trig_L2_el_L2_e20_loose_xe30, &b_trig_L2_el_L2_e20_loose_xe30);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_medium", &trig_L2_el_L2_e20_medium, &b_trig_L2_el_L2_e20_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_e20i_loose", &trig_L2_el_L2_e20i_loose, &b_trig_L2_el_L2_e20i_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_e22i_tight", &trig_L2_el_L2_e22i_tight, &b_trig_L2_el_L2_e22i_tight);
   fChain->SetBranchAddress("trig_L2_el_L2_e25_loose", &trig_L2_el_L2_e25_loose, &b_trig_L2_el_L2_e25_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_e25_medium", &trig_L2_el_L2_e25_medium, &b_trig_L2_el_L2_e25_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_e25i_loose", &trig_L2_el_L2_e25i_loose, &b_trig_L2_el_L2_e25i_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_e25i_medium", &trig_L2_el_L2_e25i_medium, &b_trig_L2_el_L2_e25i_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_e55_loose1", &trig_L2_el_L2_e55_loose1, &b_trig_L2_el_L2_e55_loose1);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_medium", &trig_L2_el_L2_e5_medium, &b_trig_L2_el_L2_e5_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_medium1", &trig_L2_el_L2_e5_medium1, &b_trig_L2_el_L2_e5_medium1);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_medium_L2SW", &trig_L2_el_L2_e5_medium_L2SW, &b_trig_L2_el_L2_e5_medium_L2SW);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_medium_MV", &trig_L2_el_L2_e5_medium_MV, &b_trig_L2_el_L2_e5_medium_MV);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_medium_Ringer", &trig_L2_el_L2_e5_medium_Ringer, &b_trig_L2_el_L2_e5_medium_Ringer);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_medium_e10_loose", &trig_L2_el_L2_e5_medium_e10_loose, &b_trig_L2_el_L2_e5_medium_e10_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_em105_passHLT", &trig_L2_el_L2_em105_passHLT, &b_trig_L2_el_L2_em105_passHLT);
   fChain->SetBranchAddress("trig_L2_el_L2_em15_passHLT", &trig_L2_el_L2_em15_passHLT, &b_trig_L2_el_L2_em15_passHLT);
   fChain->SetBranchAddress("trig_L2_el_L2_em15i_passHLT", &trig_L2_el_L2_em15i_passHLT, &b_trig_L2_el_L2_em15i_passHLT);
   fChain->SetBranchAddress("trig_L2_el_L2_em20_passHLT", &trig_L2_el_L2_em20_passHLT, &b_trig_L2_el_L2_em20_passHLT);
   fChain->SetBranchAddress("trig_L2_el_L2_em20i_passHLT", &trig_L2_el_L2_em20i_passHLT, &b_trig_L2_el_L2_em20i_passHLT);
   fChain->SetBranchAddress("trig_L2_el_nTRTHits", &trig_L2_el_nTRTHits, &b_trig_L2_el_nTRTHits);
   fChain->SetBranchAddress("trig_L2_el_nTRTHighTHits", &trig_L2_el_nTRTHighTHits, &b_trig_L2_el_nTRTHighTHits);
   fChain->SetBranchAddress("trig_L2_el_trackIndx", &trig_L2_el_trackIndx, &b_trig_L2_el_trackIndx);
   fChain->SetBranchAddress("trig_L2_el_trkPt", &trig_L2_el_trkPt, &b_trig_L2_el_trkPt);
   fChain->SetBranchAddress("trig_L2_el_err_trkPt", &trig_L2_el_err_trkPt, &b_trig_L2_el_err_trkPt);
   fChain->SetBranchAddress("trig_L2_el_err_eta", &trig_L2_el_err_eta, &b_trig_L2_el_err_eta);
   fChain->SetBranchAddress("trig_L2_el_err_phi", &trig_L2_el_err_phi, &b_trig_L2_el_err_phi);
   fChain->SetBranchAddress("trig_L2_el_errz", &trig_L2_el_errz, &b_trig_L2_el_errz);
   fChain->SetBranchAddress("trig_L2_el_trkEtaAtCalo", &trig_L2_el_trkEtaAtCalo, &b_trig_L2_el_trkEtaAtCalo);
   fChain->SetBranchAddress("trig_L2_el_trkPhiAtCalo", &trig_L2_el_trkPhiAtCalo, &b_trig_L2_el_trkPhiAtCalo);
   fChain->SetBranchAddress("trig_L2_el_caloEta", &trig_L2_el_caloEta, &b_trig_L2_el_caloEta);
   fChain->SetBranchAddress("trig_L2_el_caloPhi", &trig_L2_el_caloPhi, &b_trig_L2_el_caloPhi);
   fChain->SetBranchAddress("trig_L2_el_F0", &trig_L2_el_F0, &b_trig_L2_el_F0);
   fChain->SetBranchAddress("trig_L2_el_F1", &trig_L2_el_F1, &b_trig_L2_el_F1);
   fChain->SetBranchAddress("trig_L2_el_F2", &trig_L2_el_F2, &b_trig_L2_el_F2);
   fChain->SetBranchAddress("trig_L2_el_F3", &trig_L2_el_F3, &b_trig_L2_el_F3);
   fChain->SetBranchAddress("trig_L2_ph_n", &trig_L2_ph_n, &b_trig_L2_ph_n);
   fChain->SetBranchAddress("trig_L2_ph_E", &trig_L2_ph_E, &b_trig_L2_ph_E);
   fChain->SetBranchAddress("trig_L2_ph_Et", &trig_L2_ph_Et, &b_trig_L2_ph_Et);
   fChain->SetBranchAddress("trig_L2_ph_pt", &trig_L2_ph_pt, &b_trig_L2_ph_pt);
   fChain->SetBranchAddress("trig_L2_ph_eta", &trig_L2_ph_eta, &b_trig_L2_ph_eta);
   fChain->SetBranchAddress("trig_L2_ph_phi", &trig_L2_ph_phi, &b_trig_L2_ph_phi);
   fChain->SetBranchAddress("trig_L2_ph_RoIWord", &trig_L2_ph_RoIWord, &b_trig_L2_ph_RoIWord);
   fChain->SetBranchAddress("trig_L2_ph_HadEt1", &trig_L2_ph_HadEt1, &b_trig_L2_ph_HadEt1);
   fChain->SetBranchAddress("trig_L2_ph_Eratio", &trig_L2_ph_Eratio, &b_trig_L2_ph_Eratio);
   fChain->SetBranchAddress("trig_L2_ph_Reta", &trig_L2_ph_Reta, &b_trig_L2_ph_Reta);
   fChain->SetBranchAddress("trig_L2_ph_dPhi", &trig_L2_ph_dPhi, &b_trig_L2_ph_dPhi);
   fChain->SetBranchAddress("trig_L2_ph_dEta", &trig_L2_ph_dEta, &b_trig_L2_ph_dEta);
   fChain->SetBranchAddress("trig_L2_ph_F1", &trig_L2_ph_F1, &b_trig_L2_ph_F1);
   fChain->SetBranchAddress("trig_L2_ph_L2_2g10_loose", &trig_L2_ph_L2_2g10_loose, &b_trig_L2_ph_L2_2g10_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_2g10_loose_mu6", &trig_L2_ph_L2_2g10_loose_mu6, &b_trig_L2_ph_L2_2g10_loose_mu6);
   fChain->SetBranchAddress("trig_L2_ph_L2_2g17i_tight", &trig_L2_ph_L2_2g17i_tight, &b_trig_L2_ph_L2_2g17i_tight);
   fChain->SetBranchAddress("trig_L2_ph_L2_2g20_loose", &trig_L2_ph_L2_2g20_loose, &b_trig_L2_ph_L2_2g20_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g105", &trig_L2_ph_L2_g105, &b_trig_L2_ph_L2_g105);
   fChain->SetBranchAddress("trig_L2_ph_L2_g10_loose", &trig_L2_ph_L2_g10_loose, &b_trig_L2_ph_L2_g10_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g10_nocut", &trig_L2_ph_L2_g10_nocut, &b_trig_L2_ph_L2_g10_nocut);
   fChain->SetBranchAddress("trig_L2_ph_L2_g150", &trig_L2_ph_L2_g150, &b_trig_L2_ph_L2_g150);
   fChain->SetBranchAddress("trig_L2_ph_L2_g15_loose", &trig_L2_ph_L2_g15_loose, &b_trig_L2_ph_L2_g15_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g20_loose", &trig_L2_ph_L2_g20_loose, &b_trig_L2_ph_L2_g20_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g20_loose_cnv", &trig_L2_ph_L2_g20_loose_cnv, &b_trig_L2_ph_L2_g20_loose_cnv);
   fChain->SetBranchAddress("trig_L2_ph_L2_g20_loose_xe20", &trig_L2_ph_L2_g20_loose_xe20, &b_trig_L2_ph_L2_g20_loose_xe20);
   fChain->SetBranchAddress("trig_L2_ph_L2_g20i_loose", &trig_L2_ph_L2_g20i_loose, &b_trig_L2_ph_L2_g20i_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g25_loose", &trig_L2_ph_L2_g25_loose, &b_trig_L2_ph_L2_g25_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g25_loose_xe30", &trig_L2_ph_L2_g25_loose_xe30, &b_trig_L2_ph_L2_g25_loose_xe30);
   fChain->SetBranchAddress("trig_L2_ph_L2_g25i_loose", &trig_L2_ph_L2_g25i_loose, &b_trig_L2_ph_L2_g25i_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g55_tight", &trig_L2_ph_L2_g55_tight, &b_trig_L2_ph_L2_g55_tight);
   fChain->SetBranchAddress("trig_L2_ph_Fside", &trig_L2_ph_Fside, &b_trig_L2_ph_Fside);
   fChain->SetBranchAddress("trig_L2_ph_Weta2", &trig_L2_ph_Weta2, &b_trig_L2_ph_Weta2);
   fChain->SetBranchAddress("trig_L2_ph_F0", &trig_L2_ph_F0, &b_trig_L2_ph_F0);
   fChain->SetBranchAddress("trig_L2_ph_F2", &trig_L2_ph_F2, &b_trig_L2_ph_F2);
   fChain->SetBranchAddress("trig_L2_ph_F3", &trig_L2_ph_F3, &b_trig_L2_ph_F3);
   fChain->SetBranchAddress("trig_EF_emcl_n", &trig_EF_emcl_n, &b_trig_EF_emcl_n);
   fChain->SetBranchAddress("trig_EF_emcl_E", &trig_EF_emcl_E, &b_trig_EF_emcl_E);
   fChain->SetBranchAddress("trig_EF_emcl_pt", &trig_EF_emcl_pt, &b_trig_EF_emcl_pt);
   fChain->SetBranchAddress("trig_EF_emcl_m", &trig_EF_emcl_m, &b_trig_EF_emcl_m);
   fChain->SetBranchAddress("trig_EF_emcl_eta", &trig_EF_emcl_eta, &b_trig_EF_emcl_eta);
   fChain->SetBranchAddress("trig_EF_emcl_phi", &trig_EF_emcl_phi, &b_trig_EF_emcl_phi);
   fChain->SetBranchAddress("trig_EF_emcl_E_em", &trig_EF_emcl_E_em, &b_trig_EF_emcl_E_em);
   fChain->SetBranchAddress("trig_EF_emcl_E_had", &trig_EF_emcl_E_had, &b_trig_EF_emcl_E_had);
   fChain->SetBranchAddress("trig_EF_emcl_firstEdens", &trig_EF_emcl_firstEdens, &b_trig_EF_emcl_firstEdens);
   fChain->SetBranchAddress("trig_EF_emcl_cellmaxfrac", &trig_EF_emcl_cellmaxfrac, &b_trig_EF_emcl_cellmaxfrac);
   fChain->SetBranchAddress("trig_EF_emcl_longitudinal", &trig_EF_emcl_longitudinal, &b_trig_EF_emcl_longitudinal);
   fChain->SetBranchAddress("trig_EF_emcl_secondlambda", &trig_EF_emcl_secondlambda, &b_trig_EF_emcl_secondlambda);
   fChain->SetBranchAddress("trig_EF_emcl_lateral", &trig_EF_emcl_lateral, &b_trig_EF_emcl_lateral);
   fChain->SetBranchAddress("trig_EF_emcl_secondR", &trig_EF_emcl_secondR, &b_trig_EF_emcl_secondR);
   fChain->SetBranchAddress("trig_EF_emcl_centerlambda", &trig_EF_emcl_centerlambda, &b_trig_EF_emcl_centerlambda);
   fChain->SetBranchAddress("trig_EF_emcl_deltaTheta", &trig_EF_emcl_deltaTheta, &b_trig_EF_emcl_deltaTheta);
   fChain->SetBranchAddress("trig_EF_emcl_deltaPhi", &trig_EF_emcl_deltaPhi, &b_trig_EF_emcl_deltaPhi);
   fChain->SetBranchAddress("trig_EF_emcl_time", &trig_EF_emcl_time, &b_trig_EF_emcl_time);
   fChain->SetBranchAddress("trig_EF_emcl_E_PreSamplerB", &trig_EF_emcl_E_PreSamplerB, &b_trig_EF_emcl_E_PreSamplerB);
   fChain->SetBranchAddress("trig_EF_emcl_E_EMB1", &trig_EF_emcl_E_EMB1, &b_trig_EF_emcl_E_EMB1);
   fChain->SetBranchAddress("trig_EF_emcl_E_EMB2", &trig_EF_emcl_E_EMB2, &b_trig_EF_emcl_E_EMB2);
   fChain->SetBranchAddress("trig_EF_emcl_E_EMB3", &trig_EF_emcl_E_EMB3, &b_trig_EF_emcl_E_EMB3);
   fChain->SetBranchAddress("trig_EF_emcl_E_PreSamplerE", &trig_EF_emcl_E_PreSamplerE, &b_trig_EF_emcl_E_PreSamplerE);
   fChain->SetBranchAddress("trig_EF_emcl_E_EME1", &trig_EF_emcl_E_EME1, &b_trig_EF_emcl_E_EME1);
   fChain->SetBranchAddress("trig_EF_emcl_E_EME2", &trig_EF_emcl_E_EME2, &b_trig_EF_emcl_E_EME2);
   fChain->SetBranchAddress("trig_EF_emcl_E_EME3", &trig_EF_emcl_E_EME3, &b_trig_EF_emcl_E_EME3);
   fChain->SetBranchAddress("trig_EF_emcl_E_HEC0", &trig_EF_emcl_E_HEC0, &b_trig_EF_emcl_E_HEC0);
   fChain->SetBranchAddress("trig_EF_emcl_E_HEC1", &trig_EF_emcl_E_HEC1, &b_trig_EF_emcl_E_HEC1);
   fChain->SetBranchAddress("trig_EF_emcl_E_HEC2", &trig_EF_emcl_E_HEC2, &b_trig_EF_emcl_E_HEC2);
   fChain->SetBranchAddress("trig_EF_emcl_E_HEC3", &trig_EF_emcl_E_HEC3, &b_trig_EF_emcl_E_HEC3);
   fChain->SetBranchAddress("trig_EF_emcl_E_TileBar0", &trig_EF_emcl_E_TileBar0, &b_trig_EF_emcl_E_TileBar0);
   fChain->SetBranchAddress("trig_EF_emcl_E_TileBar1", &trig_EF_emcl_E_TileBar1, &b_trig_EF_emcl_E_TileBar1);
   fChain->SetBranchAddress("trig_EF_emcl_E_TileBar2", &trig_EF_emcl_E_TileBar2, &b_trig_EF_emcl_E_TileBar2);
   fChain->SetBranchAddress("trig_EF_emcl_E_TileGap1", &trig_EF_emcl_E_TileGap1, &b_trig_EF_emcl_E_TileGap1);
   fChain->SetBranchAddress("trig_EF_emcl_E_TileGap2", &trig_EF_emcl_E_TileGap2, &b_trig_EF_emcl_E_TileGap2);
   fChain->SetBranchAddress("trig_EF_emcl_E_TileGap3", &trig_EF_emcl_E_TileGap3, &b_trig_EF_emcl_E_TileGap3);
   fChain->SetBranchAddress("trig_EF_emcl_E_TileExt0", &trig_EF_emcl_E_TileExt0, &b_trig_EF_emcl_E_TileExt0);
   fChain->SetBranchAddress("trig_EF_emcl_E_TileExt1", &trig_EF_emcl_E_TileExt1, &b_trig_EF_emcl_E_TileExt1);
   fChain->SetBranchAddress("trig_EF_emcl_E_TileExt2", &trig_EF_emcl_E_TileExt2, &b_trig_EF_emcl_E_TileExt2);
   fChain->SetBranchAddress("trig_EF_emcl_E_FCAL0", &trig_EF_emcl_E_FCAL0, &b_trig_EF_emcl_E_FCAL0);
   fChain->SetBranchAddress("trig_EF_emcl_E_FCAL1", &trig_EF_emcl_E_FCAL1, &b_trig_EF_emcl_E_FCAL1);
   fChain->SetBranchAddress("trig_EF_emcl_E_FCAL2", &trig_EF_emcl_E_FCAL2, &b_trig_EF_emcl_E_FCAL2);
   fChain->SetBranchAddress("trig_EF_emcl_eta_PreSamplerB", &trig_EF_emcl_eta_PreSamplerB, &b_trig_EF_emcl_eta_PreSamplerB);
   fChain->SetBranchAddress("trig_EF_emcl_phi_PreSamplerB", &trig_EF_emcl_phi_PreSamplerB, &b_trig_EF_emcl_phi_PreSamplerB);
   fChain->SetBranchAddress("trig_EF_emcl_eta_EMB1", &trig_EF_emcl_eta_EMB1, &b_trig_EF_emcl_eta_EMB1);
   fChain->SetBranchAddress("trig_EF_emcl_phi_EMB1", &trig_EF_emcl_phi_EMB1, &b_trig_EF_emcl_phi_EMB1);
   fChain->SetBranchAddress("trig_EF_emcl_eta_EMB2", &trig_EF_emcl_eta_EMB2, &b_trig_EF_emcl_eta_EMB2);
   fChain->SetBranchAddress("trig_EF_emcl_phi_EMB2", &trig_EF_emcl_phi_EMB2, &b_trig_EF_emcl_phi_EMB2);
   fChain->SetBranchAddress("trig_EF_emcl_eta_EMB3", &trig_EF_emcl_eta_EMB3, &b_trig_EF_emcl_eta_EMB3);
   fChain->SetBranchAddress("trig_EF_emcl_phi_EMB3", &trig_EF_emcl_phi_EMB3, &b_trig_EF_emcl_phi_EMB3);
   fChain->SetBranchAddress("trig_EF_emcl_eta_PreSamplerE", &trig_EF_emcl_eta_PreSamplerE, &b_trig_EF_emcl_eta_PreSamplerE);
   fChain->SetBranchAddress("trig_EF_emcl_phi_PreSamplerE", &trig_EF_emcl_phi_PreSamplerE, &b_trig_EF_emcl_phi_PreSamplerE);
   fChain->SetBranchAddress("trig_EF_emcl_eta_EME1", &trig_EF_emcl_eta_EME1, &b_trig_EF_emcl_eta_EME1);
   fChain->SetBranchAddress("trig_EF_emcl_phi_EME1", &trig_EF_emcl_phi_EME1, &b_trig_EF_emcl_phi_EME1);
   fChain->SetBranchAddress("trig_EF_emcl_eta_EME2", &trig_EF_emcl_eta_EME2, &b_trig_EF_emcl_eta_EME2);
   fChain->SetBranchAddress("trig_EF_emcl_phi_EME2", &trig_EF_emcl_phi_EME2, &b_trig_EF_emcl_phi_EME2);
   fChain->SetBranchAddress("trig_EF_emcl_eta_EME3", &trig_EF_emcl_eta_EME3, &b_trig_EF_emcl_eta_EME3);
   fChain->SetBranchAddress("trig_EF_emcl_phi_EME3", &trig_EF_emcl_phi_EME3, &b_trig_EF_emcl_phi_EME3);
   fChain->SetBranchAddress("trig_EF_emcl_eta_HEC0", &trig_EF_emcl_eta_HEC0, &b_trig_EF_emcl_eta_HEC0);
   fChain->SetBranchAddress("trig_EF_emcl_phi_HEC0", &trig_EF_emcl_phi_HEC0, &b_trig_EF_emcl_phi_HEC0);
   fChain->SetBranchAddress("trig_EF_emcl_eta_HEC1", &trig_EF_emcl_eta_HEC1, &b_trig_EF_emcl_eta_HEC1);
   fChain->SetBranchAddress("trig_EF_emcl_phi_HEC1", &trig_EF_emcl_phi_HEC1, &b_trig_EF_emcl_phi_HEC1);
   fChain->SetBranchAddress("trig_EF_emcl_eta_HEC2", &trig_EF_emcl_eta_HEC2, &b_trig_EF_emcl_eta_HEC2);
   fChain->SetBranchAddress("trig_EF_emcl_phi_HEC2", &trig_EF_emcl_phi_HEC2, &b_trig_EF_emcl_phi_HEC2);
   fChain->SetBranchAddress("trig_EF_emcl_eta_HEC3", &trig_EF_emcl_eta_HEC3, &b_trig_EF_emcl_eta_HEC3);
   fChain->SetBranchAddress("trig_EF_emcl_phi_HEC3", &trig_EF_emcl_phi_HEC3, &b_trig_EF_emcl_phi_HEC3);
   fChain->SetBranchAddress("trig_EF_emcl_eta_TileBar0", &trig_EF_emcl_eta_TileBar0, &b_trig_EF_emcl_eta_TileBar0);
   fChain->SetBranchAddress("trig_EF_emcl_phi_TileBar0", &trig_EF_emcl_phi_TileBar0, &b_trig_EF_emcl_phi_TileBar0);
   fChain->SetBranchAddress("trig_EF_emcl_eta_TileBar1", &trig_EF_emcl_eta_TileBar1, &b_trig_EF_emcl_eta_TileBar1);
   fChain->SetBranchAddress("trig_EF_emcl_phi_TileBar1", &trig_EF_emcl_phi_TileBar1, &b_trig_EF_emcl_phi_TileBar1);
   fChain->SetBranchAddress("trig_EF_emcl_eta_TileBar2", &trig_EF_emcl_eta_TileBar2, &b_trig_EF_emcl_eta_TileBar2);
   fChain->SetBranchAddress("trig_EF_emcl_phi_TileBar2", &trig_EF_emcl_phi_TileBar2, &b_trig_EF_emcl_phi_TileBar2);
   fChain->SetBranchAddress("trig_EF_emcl_eta_TileGap1", &trig_EF_emcl_eta_TileGap1, &b_trig_EF_emcl_eta_TileGap1);
   fChain->SetBranchAddress("trig_EF_emcl_phi_TileGap1", &trig_EF_emcl_phi_TileGap1, &b_trig_EF_emcl_phi_TileGap1);
   fChain->SetBranchAddress("trig_EF_emcl_eta_TileGap2", &trig_EF_emcl_eta_TileGap2, &b_trig_EF_emcl_eta_TileGap2);
   fChain->SetBranchAddress("trig_EF_emcl_phi_TileGap2", &trig_EF_emcl_phi_TileGap2, &b_trig_EF_emcl_phi_TileGap2);
   fChain->SetBranchAddress("trig_EF_emcl_eta_TileGap3", &trig_EF_emcl_eta_TileGap3, &b_trig_EF_emcl_eta_TileGap3);
   fChain->SetBranchAddress("trig_EF_emcl_phi_TileGap3", &trig_EF_emcl_phi_TileGap3, &b_trig_EF_emcl_phi_TileGap3);
   fChain->SetBranchAddress("trig_EF_emcl_eta_TileExt0", &trig_EF_emcl_eta_TileExt0, &b_trig_EF_emcl_eta_TileExt0);
   fChain->SetBranchAddress("trig_EF_emcl_phi_TileExt0", &trig_EF_emcl_phi_TileExt0, &b_trig_EF_emcl_phi_TileExt0);
   fChain->SetBranchAddress("trig_EF_emcl_eta_TileExt1", &trig_EF_emcl_eta_TileExt1, &b_trig_EF_emcl_eta_TileExt1);
   fChain->SetBranchAddress("trig_EF_emcl_phi_TileExt1", &trig_EF_emcl_phi_TileExt1, &b_trig_EF_emcl_phi_TileExt1);
   fChain->SetBranchAddress("trig_EF_emcl_eta_TileExt2", &trig_EF_emcl_eta_TileExt2, &b_trig_EF_emcl_eta_TileExt2);
   fChain->SetBranchAddress("trig_EF_emcl_phi_TileExt2", &trig_EF_emcl_phi_TileExt2, &b_trig_EF_emcl_phi_TileExt2);
   fChain->SetBranchAddress("trig_EF_emcl_eta_FCAL0", &trig_EF_emcl_eta_FCAL0, &b_trig_EF_emcl_eta_FCAL0);
   fChain->SetBranchAddress("trig_EF_emcl_phi_FCAL0", &trig_EF_emcl_phi_FCAL0, &b_trig_EF_emcl_phi_FCAL0);
   fChain->SetBranchAddress("trig_EF_emcl_eta_FCAL1", &trig_EF_emcl_eta_FCAL1, &b_trig_EF_emcl_eta_FCAL1);
   fChain->SetBranchAddress("trig_EF_emcl_phi_FCAL1", &trig_EF_emcl_phi_FCAL1, &b_trig_EF_emcl_phi_FCAL1);
   fChain->SetBranchAddress("trig_EF_emcl_eta_FCAL2", &trig_EF_emcl_eta_FCAL2, &b_trig_EF_emcl_eta_FCAL2);
   fChain->SetBranchAddress("trig_EF_emcl_phi_FCAL2", &trig_EF_emcl_phi_FCAL2, &b_trig_EF_emcl_phi_FCAL2);
   fChain->SetBranchAddress("trig_EF_emcl_cell_n", &trig_EF_emcl_cell_n, &b_trig_EF_emcl_cell_n);
   fChain->SetBranchAddress("trig_EF_emcl_cell_E", &trig_EF_emcl_cell_E, &b_trig_EF_emcl_cell_E);
   fChain->SetBranchAddress("trig_EF_emcl_cell_pt", &trig_EF_emcl_cell_pt, &b_trig_EF_emcl_cell_pt);
   fChain->SetBranchAddress("trig_EF_emcl_cell_eta", &trig_EF_emcl_cell_eta, &b_trig_EF_emcl_cell_eta);
   fChain->SetBranchAddress("trig_EF_emcl_cell_phi", &trig_EF_emcl_cell_phi, &b_trig_EF_emcl_cell_phi);
   fChain->SetBranchAddress("trig_EF_emcl_cell_QCells", &trig_EF_emcl_cell_QCells, &b_trig_EF_emcl_cell_QCells);
   fChain->SetBranchAddress("trig_EF_emcl_cell_GainCells", &trig_EF_emcl_cell_GainCells, &b_trig_EF_emcl_cell_GainCells);
   fChain->SetBranchAddress("trig_EF_emcl_cell_DetCells", &trig_EF_emcl_cell_DetCells, &b_trig_EF_emcl_cell_DetCells);
   fChain->SetBranchAddress("trig_EF_emcl_cell_TimeCells", &trig_EF_emcl_cell_TimeCells, &b_trig_EF_emcl_cell_TimeCells);
   fChain->SetBranchAddress("trig_EF_emcl_cell_xCells", &trig_EF_emcl_cell_xCells, &b_trig_EF_emcl_cell_xCells);
   fChain->SetBranchAddress("trig_EF_emcl_cell_yCells", &trig_EF_emcl_cell_yCells, &b_trig_EF_emcl_cell_yCells);
   fChain->SetBranchAddress("trig_EF_emcl_cell_zCells", &trig_EF_emcl_cell_zCells, &b_trig_EF_emcl_cell_zCells);
   fChain->SetBranchAddress("trig_EF_emcl_cell_BadCells", &trig_EF_emcl_cell_BadCells, &b_trig_EF_emcl_cell_BadCells);
   fChain->SetBranchAddress("trig_EF_emcl_cell_IdCells", &trig_EF_emcl_cell_IdCells, &b_trig_EF_emcl_cell_IdCells);
   fChain->SetBranchAddress("trig_EF_emcl_slw_n", &trig_EF_emcl_slw_n, &b_trig_EF_emcl_slw_n);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E", &trig_EF_emcl_slw_E, &b_trig_EF_emcl_slw_E);
   fChain->SetBranchAddress("trig_EF_emcl_slw_pt", &trig_EF_emcl_slw_pt, &b_trig_EF_emcl_slw_pt);
   fChain->SetBranchAddress("trig_EF_emcl_slw_m", &trig_EF_emcl_slw_m, &b_trig_EF_emcl_slw_m);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta", &trig_EF_emcl_slw_eta, &b_trig_EF_emcl_slw_eta);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi", &trig_EF_emcl_slw_phi, &b_trig_EF_emcl_slw_phi);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_em", &trig_EF_emcl_slw_E_em, &b_trig_EF_emcl_slw_E_em);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_had", &trig_EF_emcl_slw_E_had, &b_trig_EF_emcl_slw_E_had);
   fChain->SetBranchAddress("trig_EF_emcl_slw_firstEdens", &trig_EF_emcl_slw_firstEdens, &b_trig_EF_emcl_slw_firstEdens);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cellmaxfrac", &trig_EF_emcl_slw_cellmaxfrac, &b_trig_EF_emcl_slw_cellmaxfrac);
   fChain->SetBranchAddress("trig_EF_emcl_slw_longitudinal", &trig_EF_emcl_slw_longitudinal, &b_trig_EF_emcl_slw_longitudinal);
   fChain->SetBranchAddress("trig_EF_emcl_slw_secondlambda", &trig_EF_emcl_slw_secondlambda, &b_trig_EF_emcl_slw_secondlambda);
   fChain->SetBranchAddress("trig_EF_emcl_slw_lateral", &trig_EF_emcl_slw_lateral, &b_trig_EF_emcl_slw_lateral);
   fChain->SetBranchAddress("trig_EF_emcl_slw_secondR", &trig_EF_emcl_slw_secondR, &b_trig_EF_emcl_slw_secondR);
   fChain->SetBranchAddress("trig_EF_emcl_slw_centerlambda", &trig_EF_emcl_slw_centerlambda, &b_trig_EF_emcl_slw_centerlambda);
   fChain->SetBranchAddress("trig_EF_emcl_slw_deltaTheta", &trig_EF_emcl_slw_deltaTheta, &b_trig_EF_emcl_slw_deltaTheta);
   fChain->SetBranchAddress("trig_EF_emcl_slw_deltaPhi", &trig_EF_emcl_slw_deltaPhi, &b_trig_EF_emcl_slw_deltaPhi);
   fChain->SetBranchAddress("trig_EF_emcl_slw_time", &trig_EF_emcl_slw_time, &b_trig_EF_emcl_slw_time);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_PreSamplerB", &trig_EF_emcl_slw_E_PreSamplerB, &b_trig_EF_emcl_slw_E_PreSamplerB);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_EMB1", &trig_EF_emcl_slw_E_EMB1, &b_trig_EF_emcl_slw_E_EMB1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_EMB2", &trig_EF_emcl_slw_E_EMB2, &b_trig_EF_emcl_slw_E_EMB2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_EMB3", &trig_EF_emcl_slw_E_EMB3, &b_trig_EF_emcl_slw_E_EMB3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_PreSamplerE", &trig_EF_emcl_slw_E_PreSamplerE, &b_trig_EF_emcl_slw_E_PreSamplerE);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_EME1", &trig_EF_emcl_slw_E_EME1, &b_trig_EF_emcl_slw_E_EME1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_EME2", &trig_EF_emcl_slw_E_EME2, &b_trig_EF_emcl_slw_E_EME2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_EME3", &trig_EF_emcl_slw_E_EME3, &b_trig_EF_emcl_slw_E_EME3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_HEC0", &trig_EF_emcl_slw_E_HEC0, &b_trig_EF_emcl_slw_E_HEC0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_HEC1", &trig_EF_emcl_slw_E_HEC1, &b_trig_EF_emcl_slw_E_HEC1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_HEC2", &trig_EF_emcl_slw_E_HEC2, &b_trig_EF_emcl_slw_E_HEC2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_HEC3", &trig_EF_emcl_slw_E_HEC3, &b_trig_EF_emcl_slw_E_HEC3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_TileBar0", &trig_EF_emcl_slw_E_TileBar0, &b_trig_EF_emcl_slw_E_TileBar0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_TileBar1", &trig_EF_emcl_slw_E_TileBar1, &b_trig_EF_emcl_slw_E_TileBar1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_TileBar2", &trig_EF_emcl_slw_E_TileBar2, &b_trig_EF_emcl_slw_E_TileBar2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_TileGap1", &trig_EF_emcl_slw_E_TileGap1, &b_trig_EF_emcl_slw_E_TileGap1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_TileGap2", &trig_EF_emcl_slw_E_TileGap2, &b_trig_EF_emcl_slw_E_TileGap2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_TileGap3", &trig_EF_emcl_slw_E_TileGap3, &b_trig_EF_emcl_slw_E_TileGap3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_TileExt0", &trig_EF_emcl_slw_E_TileExt0, &b_trig_EF_emcl_slw_E_TileExt0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_TileExt1", &trig_EF_emcl_slw_E_TileExt1, &b_trig_EF_emcl_slw_E_TileExt1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_TileExt2", &trig_EF_emcl_slw_E_TileExt2, &b_trig_EF_emcl_slw_E_TileExt2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_FCAL0", &trig_EF_emcl_slw_E_FCAL0, &b_trig_EF_emcl_slw_E_FCAL0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_FCAL1", &trig_EF_emcl_slw_E_FCAL1, &b_trig_EF_emcl_slw_E_FCAL1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_E_FCAL2", &trig_EF_emcl_slw_E_FCAL2, &b_trig_EF_emcl_slw_E_FCAL2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_PreSamplerB", &trig_EF_emcl_slw_eta_PreSamplerB, &b_trig_EF_emcl_slw_eta_PreSamplerB);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_PreSamplerB", &trig_EF_emcl_slw_phi_PreSamplerB, &b_trig_EF_emcl_slw_phi_PreSamplerB);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_EMB1", &trig_EF_emcl_slw_eta_EMB1, &b_trig_EF_emcl_slw_eta_EMB1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_EMB1", &trig_EF_emcl_slw_phi_EMB1, &b_trig_EF_emcl_slw_phi_EMB1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_EMB2", &trig_EF_emcl_slw_eta_EMB2, &b_trig_EF_emcl_slw_eta_EMB2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_EMB2", &trig_EF_emcl_slw_phi_EMB2, &b_trig_EF_emcl_slw_phi_EMB2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_EMB3", &trig_EF_emcl_slw_eta_EMB3, &b_trig_EF_emcl_slw_eta_EMB3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_EMB3", &trig_EF_emcl_slw_phi_EMB3, &b_trig_EF_emcl_slw_phi_EMB3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_PreSamplerE", &trig_EF_emcl_slw_eta_PreSamplerE, &b_trig_EF_emcl_slw_eta_PreSamplerE);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_PreSamplerE", &trig_EF_emcl_slw_phi_PreSamplerE, &b_trig_EF_emcl_slw_phi_PreSamplerE);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_EME1", &trig_EF_emcl_slw_eta_EME1, &b_trig_EF_emcl_slw_eta_EME1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_EME1", &trig_EF_emcl_slw_phi_EME1, &b_trig_EF_emcl_slw_phi_EME1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_EME2", &trig_EF_emcl_slw_eta_EME2, &b_trig_EF_emcl_slw_eta_EME2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_EME2", &trig_EF_emcl_slw_phi_EME2, &b_trig_EF_emcl_slw_phi_EME2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_EME3", &trig_EF_emcl_slw_eta_EME3, &b_trig_EF_emcl_slw_eta_EME3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_EME3", &trig_EF_emcl_slw_phi_EME3, &b_trig_EF_emcl_slw_phi_EME3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_HEC0", &trig_EF_emcl_slw_eta_HEC0, &b_trig_EF_emcl_slw_eta_HEC0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_HEC0", &trig_EF_emcl_slw_phi_HEC0, &b_trig_EF_emcl_slw_phi_HEC0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_HEC1", &trig_EF_emcl_slw_eta_HEC1, &b_trig_EF_emcl_slw_eta_HEC1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_HEC1", &trig_EF_emcl_slw_phi_HEC1, &b_trig_EF_emcl_slw_phi_HEC1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_HEC2", &trig_EF_emcl_slw_eta_HEC2, &b_trig_EF_emcl_slw_eta_HEC2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_HEC2", &trig_EF_emcl_slw_phi_HEC2, &b_trig_EF_emcl_slw_phi_HEC2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_HEC3", &trig_EF_emcl_slw_eta_HEC3, &b_trig_EF_emcl_slw_eta_HEC3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_HEC3", &trig_EF_emcl_slw_phi_HEC3, &b_trig_EF_emcl_slw_phi_HEC3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_TileBar0", &trig_EF_emcl_slw_eta_TileBar0, &b_trig_EF_emcl_slw_eta_TileBar0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_TileBar0", &trig_EF_emcl_slw_phi_TileBar0, &b_trig_EF_emcl_slw_phi_TileBar0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_TileBar1", &trig_EF_emcl_slw_eta_TileBar1, &b_trig_EF_emcl_slw_eta_TileBar1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_TileBar1", &trig_EF_emcl_slw_phi_TileBar1, &b_trig_EF_emcl_slw_phi_TileBar1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_TileBar2", &trig_EF_emcl_slw_eta_TileBar2, &b_trig_EF_emcl_slw_eta_TileBar2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_TileBar2", &trig_EF_emcl_slw_phi_TileBar2, &b_trig_EF_emcl_slw_phi_TileBar2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_TileGap1", &trig_EF_emcl_slw_eta_TileGap1, &b_trig_EF_emcl_slw_eta_TileGap1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_TileGap1", &trig_EF_emcl_slw_phi_TileGap1, &b_trig_EF_emcl_slw_phi_TileGap1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_TileGap2", &trig_EF_emcl_slw_eta_TileGap2, &b_trig_EF_emcl_slw_eta_TileGap2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_TileGap2", &trig_EF_emcl_slw_phi_TileGap2, &b_trig_EF_emcl_slw_phi_TileGap2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_TileGap3", &trig_EF_emcl_slw_eta_TileGap3, &b_trig_EF_emcl_slw_eta_TileGap3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_TileGap3", &trig_EF_emcl_slw_phi_TileGap3, &b_trig_EF_emcl_slw_phi_TileGap3);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_TileExt0", &trig_EF_emcl_slw_eta_TileExt0, &b_trig_EF_emcl_slw_eta_TileExt0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_TileExt0", &trig_EF_emcl_slw_phi_TileExt0, &b_trig_EF_emcl_slw_phi_TileExt0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_TileExt1", &trig_EF_emcl_slw_eta_TileExt1, &b_trig_EF_emcl_slw_eta_TileExt1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_TileExt1", &trig_EF_emcl_slw_phi_TileExt1, &b_trig_EF_emcl_slw_phi_TileExt1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_TileExt2", &trig_EF_emcl_slw_eta_TileExt2, &b_trig_EF_emcl_slw_eta_TileExt2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_TileExt2", &trig_EF_emcl_slw_phi_TileExt2, &b_trig_EF_emcl_slw_phi_TileExt2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_FCAL0", &trig_EF_emcl_slw_eta_FCAL0, &b_trig_EF_emcl_slw_eta_FCAL0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_FCAL0", &trig_EF_emcl_slw_phi_FCAL0, &b_trig_EF_emcl_slw_phi_FCAL0);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_FCAL1", &trig_EF_emcl_slw_eta_FCAL1, &b_trig_EF_emcl_slw_eta_FCAL1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_FCAL1", &trig_EF_emcl_slw_phi_FCAL1, &b_trig_EF_emcl_slw_phi_FCAL1);
   fChain->SetBranchAddress("trig_EF_emcl_slw_eta_FCAL2", &trig_EF_emcl_slw_eta_FCAL2, &b_trig_EF_emcl_slw_eta_FCAL2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_phi_FCAL2", &trig_EF_emcl_slw_phi_FCAL2, &b_trig_EF_emcl_slw_phi_FCAL2);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_n", &trig_EF_emcl_slw_cell_n, &b_trig_EF_emcl_slw_cell_n);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_E", &trig_EF_emcl_slw_cell_E, &b_trig_EF_emcl_slw_cell_E);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_pt", &trig_EF_emcl_slw_cell_pt, &b_trig_EF_emcl_slw_cell_pt);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_eta", &trig_EF_emcl_slw_cell_eta, &b_trig_EF_emcl_slw_cell_eta);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_phi", &trig_EF_emcl_slw_cell_phi, &b_trig_EF_emcl_slw_cell_phi);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_QCells", &trig_EF_emcl_slw_cell_QCells, &b_trig_EF_emcl_slw_cell_QCells);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_GainCells", &trig_EF_emcl_slw_cell_GainCells, &b_trig_EF_emcl_slw_cell_GainCells);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_DetCells", &trig_EF_emcl_slw_cell_DetCells, &b_trig_EF_emcl_slw_cell_DetCells);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_TimeCells", &trig_EF_emcl_slw_cell_TimeCells, &b_trig_EF_emcl_slw_cell_TimeCells);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_xCells", &trig_EF_emcl_slw_cell_xCells, &b_trig_EF_emcl_slw_cell_xCells);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_yCells", &trig_EF_emcl_slw_cell_yCells, &b_trig_EF_emcl_slw_cell_yCells);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_zCells", &trig_EF_emcl_slw_cell_zCells, &b_trig_EF_emcl_slw_cell_zCells);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_BadCells", &trig_EF_emcl_slw_cell_BadCells, &b_trig_EF_emcl_slw_cell_BadCells);
   fChain->SetBranchAddress("trig_EF_emcl_slw_cell_IdCells", &trig_EF_emcl_slw_cell_IdCells, &b_trig_EF_emcl_slw_cell_IdCells);
   fChain->SetBranchAddress("trig_EF_el_n", &trig_EF_el_n, &b_trig_EF_el_n);
   fChain->SetBranchAddress("trig_EF_el_E", &trig_EF_el_E, &b_trig_EF_el_E);
   fChain->SetBranchAddress("trig_EF_el_Et", &trig_EF_el_Et, &b_trig_EF_el_Et);
   fChain->SetBranchAddress("trig_EF_el_pt", &trig_EF_el_pt, &b_trig_EF_el_pt);
   fChain->SetBranchAddress("trig_EF_el_m", &trig_EF_el_m, &b_trig_EF_el_m);
   fChain->SetBranchAddress("trig_EF_el_eta", &trig_EF_el_eta, &b_trig_EF_el_eta);
   fChain->SetBranchAddress("trig_EF_el_phi", &trig_EF_el_phi, &b_trig_EF_el_phi);
   fChain->SetBranchAddress("trig_EF_el_px", &trig_EF_el_px, &b_trig_EF_el_px);
   fChain->SetBranchAddress("trig_EF_el_py", &trig_EF_el_py, &b_trig_EF_el_py);
   fChain->SetBranchAddress("trig_EF_el_pz", &trig_EF_el_pz, &b_trig_EF_el_pz);
   fChain->SetBranchAddress("trig_EF_el_charge", &trig_EF_el_charge, &b_trig_EF_el_charge);
   fChain->SetBranchAddress("trig_EF_el_author", &trig_EF_el_author, &b_trig_EF_el_author);
   fChain->SetBranchAddress("trig_EF_el_isEM", &trig_EF_el_isEM, &b_trig_EF_el_isEM);
   fChain->SetBranchAddress("trig_EF_el_loose", &trig_EF_el_loose, &b_trig_EF_el_loose);
   fChain->SetBranchAddress("trig_EF_el_medium", &trig_EF_el_medium, &b_trig_EF_el_medium);
   fChain->SetBranchAddress("trig_EF_el_mediumIso", &trig_EF_el_mediumIso, &b_trig_EF_el_mediumIso);
   fChain->SetBranchAddress("trig_EF_el_tight", &trig_EF_el_tight, &b_trig_EF_el_tight);
   fChain->SetBranchAddress("trig_EF_el_tightIso", &trig_EF_el_tightIso, &b_trig_EF_el_tightIso);
   fChain->SetBranchAddress("trig_EF_el_Ethad", &trig_EF_el_Ethad, &b_trig_EF_el_Ethad);
   fChain->SetBranchAddress("trig_EF_el_Ethad1", &trig_EF_el_Ethad1, &b_trig_EF_el_Ethad1);
   fChain->SetBranchAddress("trig_EF_el_f1", &trig_EF_el_f1, &b_trig_EF_el_f1);
   fChain->SetBranchAddress("trig_EF_el_f1core", &trig_EF_el_f1core, &b_trig_EF_el_f1core);
   fChain->SetBranchAddress("trig_EF_el_Emins1", &trig_EF_el_Emins1, &b_trig_EF_el_Emins1);
   fChain->SetBranchAddress("trig_EF_el_fside", &trig_EF_el_fside, &b_trig_EF_el_fside);
   fChain->SetBranchAddress("trig_EF_el_Emax2", &trig_EF_el_Emax2, &b_trig_EF_el_Emax2);
   fChain->SetBranchAddress("trig_EF_el_ws3", &trig_EF_el_ws3, &b_trig_EF_el_ws3);
   fChain->SetBranchAddress("trig_EF_el_wstot", &trig_EF_el_wstot, &b_trig_EF_el_wstot);
   fChain->SetBranchAddress("trig_EF_el_emaxs1", &trig_EF_el_emaxs1, &b_trig_EF_el_emaxs1);
   fChain->SetBranchAddress("trig_EF_el_deltaEs", &trig_EF_el_deltaEs, &b_trig_EF_el_deltaEs);
   fChain->SetBranchAddress("trig_EF_el_E233", &trig_EF_el_E233, &b_trig_EF_el_E233);
   fChain->SetBranchAddress("trig_EF_el_E237", &trig_EF_el_E237, &b_trig_EF_el_E237);
   fChain->SetBranchAddress("trig_EF_el_E277", &trig_EF_el_E277, &b_trig_EF_el_E277);
   fChain->SetBranchAddress("trig_EF_el_weta2", &trig_EF_el_weta2, &b_trig_EF_el_weta2);
   fChain->SetBranchAddress("trig_EF_el_f3", &trig_EF_el_f3, &b_trig_EF_el_f3);
   fChain->SetBranchAddress("trig_EF_el_f3core", &trig_EF_el_f3core, &b_trig_EF_el_f3core);
   fChain->SetBranchAddress("trig_EF_el_rphiallcalo", &trig_EF_el_rphiallcalo, &b_trig_EF_el_rphiallcalo);
   fChain->SetBranchAddress("trig_EF_el_Etcone45", &trig_EF_el_Etcone45, &b_trig_EF_el_Etcone45);
   fChain->SetBranchAddress("trig_EF_el_Etcone20", &trig_EF_el_Etcone20, &b_trig_EF_el_Etcone20);
   fChain->SetBranchAddress("trig_EF_el_Etcone30", &trig_EF_el_Etcone30, &b_trig_EF_el_Etcone30);
   fChain->SetBranchAddress("trig_EF_el_Etcone40", &trig_EF_el_Etcone40, &b_trig_EF_el_Etcone40);
   fChain->SetBranchAddress("trig_EF_el_pos7", &trig_EF_el_pos7, &b_trig_EF_el_pos7);
   fChain->SetBranchAddress("trig_EF_el_etacorrmag", &trig_EF_el_etacorrmag, &b_trig_EF_el_etacorrmag);
   fChain->SetBranchAddress("trig_EF_el_deltaeta1", &trig_EF_el_deltaeta1, &b_trig_EF_el_deltaeta1);
   fChain->SetBranchAddress("trig_EF_el_deltaeta2", &trig_EF_el_deltaeta2, &b_trig_EF_el_deltaeta2);
   fChain->SetBranchAddress("trig_EF_el_deltaphi2", &trig_EF_el_deltaphi2, &b_trig_EF_el_deltaphi2);
   fChain->SetBranchAddress("trig_EF_el_reta", &trig_EF_el_reta, &b_trig_EF_el_reta);
   fChain->SetBranchAddress("trig_EF_el_rphi", &trig_EF_el_rphi, &b_trig_EF_el_rphi);
   fChain->SetBranchAddress("trig_EF_el_EtringnoisedR03sig2", &trig_EF_el_EtringnoisedR03sig2, &b_trig_EF_el_EtringnoisedR03sig2);
   fChain->SetBranchAddress("trig_EF_el_EtringnoisedR03sig3", &trig_EF_el_EtringnoisedR03sig3, &b_trig_EF_el_EtringnoisedR03sig3);
   fChain->SetBranchAddress("trig_EF_el_EtringnoisedR03sig4", &trig_EF_el_EtringnoisedR03sig4, &b_trig_EF_el_EtringnoisedR03sig4);
   fChain->SetBranchAddress("trig_EF_el_isolationlikelihoodjets", &trig_EF_el_isolationlikelihoodjets, &b_trig_EF_el_isolationlikelihoodjets);
   fChain->SetBranchAddress("trig_EF_el_isolationlikelihoodhqelectrons", &trig_EF_el_isolationlikelihoodhqelectrons, &b_trig_EF_el_isolationlikelihoodhqelectrons);
   fChain->SetBranchAddress("trig_EF_el_electronweight", &trig_EF_el_electronweight, &b_trig_EF_el_electronweight);
   fChain->SetBranchAddress("trig_EF_el_electronbgweight", &trig_EF_el_electronbgweight, &b_trig_EF_el_electronbgweight);
   fChain->SetBranchAddress("trig_EF_el_softeweight", &trig_EF_el_softeweight, &b_trig_EF_el_softeweight);
   fChain->SetBranchAddress("trig_EF_el_softebgweight", &trig_EF_el_softebgweight, &b_trig_EF_el_softebgweight);
   fChain->SetBranchAddress("trig_EF_el_neuralnet", &trig_EF_el_neuralnet, &b_trig_EF_el_neuralnet);
   fChain->SetBranchAddress("trig_EF_el_Hmatrix", &trig_EF_el_Hmatrix, &b_trig_EF_el_Hmatrix);
   fChain->SetBranchAddress("trig_EF_el_Hmatrix5", &trig_EF_el_Hmatrix5, &b_trig_EF_el_Hmatrix5);
   fChain->SetBranchAddress("trig_EF_el_adaboost", &trig_EF_el_adaboost, &b_trig_EF_el_adaboost);
   fChain->SetBranchAddress("trig_EF_el_softeneuralnet", &trig_EF_el_softeneuralnet, &b_trig_EF_el_softeneuralnet);
   fChain->SetBranchAddress("trig_EF_el_zvertex", &trig_EF_el_zvertex, &b_trig_EF_el_zvertex);
   fChain->SetBranchAddress("trig_EF_el_errz", &trig_EF_el_errz, &b_trig_EF_el_errz);
   fChain->SetBranchAddress("trig_EF_el_etap", &trig_EF_el_etap, &b_trig_EF_el_etap);
   fChain->SetBranchAddress("trig_EF_el_depth", &trig_EF_el_depth, &b_trig_EF_el_depth);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovd0", &trig_EF_el_refittedtrackcovd0, &b_trig_EF_el_refittedtrackcovd0);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovz0", &trig_EF_el_refittedtrackcovz0, &b_trig_EF_el_refittedtrackcovz0);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovphi", &trig_EF_el_refittedtrackcovphi, &b_trig_EF_el_refittedtrackcovphi);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovtheta", &trig_EF_el_refittedtrackcovtheta, &b_trig_EF_el_refittedtrackcovtheta);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovqoverp", &trig_EF_el_refittedtrackcovqoverp, &b_trig_EF_el_refittedtrackcovqoverp);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovd0z0", &trig_EF_el_refittedtrackcovd0z0, &b_trig_EF_el_refittedtrackcovd0z0);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovz0phi", &trig_EF_el_refittedtrackcovz0phi, &b_trig_EF_el_refittedtrackcovz0phi);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovz0theta", &trig_EF_el_refittedtrackcovz0theta, &b_trig_EF_el_refittedtrackcovz0theta);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovz0qoverp", &trig_EF_el_refittedtrackcovz0qoverp, &b_trig_EF_el_refittedtrackcovz0qoverp);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovd0phi", &trig_EF_el_refittedtrackcovd0phi, &b_trig_EF_el_refittedtrackcovd0phi);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovd0theta", &trig_EF_el_refittedtrackcovd0theta, &b_trig_EF_el_refittedtrackcovd0theta);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovd0qoverp", &trig_EF_el_refittedtrackcovd0qoverp, &b_trig_EF_el_refittedtrackcovd0qoverp);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovphitheta", &trig_EF_el_refittedtrackcovphitheta, &b_trig_EF_el_refittedtrackcovphitheta);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovphiqoverp", &trig_EF_el_refittedtrackcovphiqoverp, &b_trig_EF_el_refittedtrackcovphiqoverp);
   fChain->SetBranchAddress("trig_EF_el_refittedtrackcovthetaqoverp", &trig_EF_el_refittedtrackcovthetaqoverp, &b_trig_EF_el_refittedtrackcovthetaqoverp);
   fChain->SetBranchAddress("trig_EF_el_EF_2e12_tight", &trig_EF_el_EF_2e12_tight, &b_trig_EF_el_EF_2e12_tight);
   fChain->SetBranchAddress("trig_EF_el_EF_2e15_medium", &trig_EF_el_EF_2e15_medium, &b_trig_EF_el_EF_2e15_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_2e20_loose", &trig_EF_el_EF_2e20_loose, &b_trig_EF_el_EF_2e20_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_2e5_medium", &trig_EF_el_EF_2e5_medium, &b_trig_EF_el_EF_2e5_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_2e5_medium1", &trig_EF_el_EF_2e5_medium1, &b_trig_EF_el_EF_2e5_medium1);
   fChain->SetBranchAddress("trig_EF_el_EF_e105_loose1", &trig_EF_el_EF_e105_loose1, &b_trig_EF_el_EF_e105_loose1);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_NoCut", &trig_EF_el_EF_e10_NoCut, &b_trig_EF_el_EF_e10_NoCut);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_loose", &trig_EF_el_EF_e10_loose, &b_trig_EF_el_EF_e10_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_loose_IdScan", &trig_EF_el_EF_e10_loose_IdScan, &b_trig_EF_el_EF_e10_loose_IdScan);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_loose_SiTrk", &trig_EF_el_EF_e10_loose_SiTrk, &b_trig_EF_el_EF_e10_loose_SiTrk);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_loose_TRT", &trig_EF_el_EF_e10_loose_TRT, &b_trig_EF_el_EF_e10_loose_TRT);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_loose_mu6", &trig_EF_el_EF_e10_loose_mu6, &b_trig_EF_el_EF_e10_loose_mu6);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_medium", &trig_EF_el_EF_e10_medium, &b_trig_EF_el_EF_e10_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_medium_SiTrk", &trig_EF_el_EF_e10_medium_SiTrk, &b_trig_EF_el_EF_e10_medium_SiTrk);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_medium_SiTrk_robust", &trig_EF_el_EF_e10_medium_SiTrk_robust, &b_trig_EF_el_EF_e10_medium_SiTrk_robust);
   fChain->SetBranchAddress("trig_EF_el_EF_e12_medium", &trig_EF_el_EF_e12_medium, &b_trig_EF_el_EF_e12_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_e15_medium", &trig_EF_el_EF_e15_medium, &b_trig_EF_el_EF_e15_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_e15i_medium", &trig_EF_el_EF_e15i_medium, &b_trig_EF_el_EF_e15i_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_loose", &trig_EF_el_EF_e20_loose, &b_trig_EF_el_EF_e20_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_loose_IdScan", &trig_EF_el_EF_e20_loose_IdScan, &b_trig_EF_el_EF_e20_loose_IdScan);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_loose_NoIDTrkCut", &trig_EF_el_EF_e20_loose_NoIDTrkCut, &b_trig_EF_el_EF_e20_loose_NoIDTrkCut);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_loose_SiTrk", &trig_EF_el_EF_e20_loose_SiTrk, &b_trig_EF_el_EF_e20_loose_SiTrk);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_loose_SiTrk_robust", &trig_EF_el_EF_e20_loose_SiTrk_robust, &b_trig_EF_el_EF_e20_loose_SiTrk_robust);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_loose_TRT", &trig_EF_el_EF_e20_loose_TRT, &b_trig_EF_el_EF_e20_loose_TRT);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_loose_passEF", &trig_EF_el_EF_e20_loose_passEF, &b_trig_EF_el_EF_e20_loose_passEF);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_loose_passL2", &trig_EF_el_EF_e20_loose_passL2, &b_trig_EF_el_EF_e20_loose_passL2);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_loose_xe20", &trig_EF_el_EF_e20_loose_xe20, &b_trig_EF_el_EF_e20_loose_xe20);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_loose_xe30", &trig_EF_el_EF_e20_loose_xe30, &b_trig_EF_el_EF_e20_loose_xe30);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_medium", &trig_EF_el_EF_e20_medium, &b_trig_EF_el_EF_e20_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_e20i_loose", &trig_EF_el_EF_e20i_loose, &b_trig_EF_el_EF_e20i_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_e22i_tight", &trig_EF_el_EF_e22i_tight, &b_trig_EF_el_EF_e22i_tight);
   fChain->SetBranchAddress("trig_EF_el_EF_e25_loose", &trig_EF_el_EF_e25_loose, &b_trig_EF_el_EF_e25_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_e25_medium", &trig_EF_el_EF_e25_medium, &b_trig_EF_el_EF_e25_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_e25i_loose", &trig_EF_el_EF_e25i_loose, &b_trig_EF_el_EF_e25i_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_e25i_medium", &trig_EF_el_EF_e25i_medium, &b_trig_EF_el_EF_e25i_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_e55_loose1", &trig_EF_el_EF_e55_loose1, &b_trig_EF_el_EF_e55_loose1);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_medium", &trig_EF_el_EF_e5_medium, &b_trig_EF_el_EF_e5_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_medium1", &trig_EF_el_EF_e5_medium1, &b_trig_EF_el_EF_e5_medium1);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_medium_L2SW", &trig_EF_el_EF_e5_medium_L2SW, &b_trig_EF_el_EF_e5_medium_L2SW);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_medium_MV", &trig_EF_el_EF_e5_medium_MV, &b_trig_EF_el_EF_e5_medium_MV);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_medium_Ringer", &trig_EF_el_EF_e5_medium_Ringer, &b_trig_EF_el_EF_e5_medium_Ringer);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_medium_e10_loose", &trig_EF_el_EF_e5_medium_e10_loose, &b_trig_EF_el_EF_e5_medium_e10_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_em105_passHLT", &trig_EF_el_EF_em105_passHLT, &b_trig_EF_el_EF_em105_passHLT);
   fChain->SetBranchAddress("trig_EF_el_EF_em15_passHLT", &trig_EF_el_EF_em15_passHLT, &b_trig_EF_el_EF_em15_passHLT);
   fChain->SetBranchAddress("trig_EF_el_EF_em15i_passHLT", &trig_EF_el_EF_em15i_passHLT, &b_trig_EF_el_EF_em15i_passHLT);
   fChain->SetBranchAddress("trig_EF_el_EF_em20_passHLT", &trig_EF_el_EF_em20_passHLT, &b_trig_EF_el_EF_em20_passHLT);
   fChain->SetBranchAddress("trig_EF_el_EF_em20i_passHLT", &trig_EF_el_EF_em20i_passHLT, &b_trig_EF_el_EF_em20i_passHLT);
   fChain->SetBranchAddress("trig_EF_ph_n", &trig_EF_ph_n, &b_trig_EF_ph_n);
   fChain->SetBranchAddress("trig_EF_ph_E", &trig_EF_ph_E, &b_trig_EF_ph_E);
   fChain->SetBranchAddress("trig_EF_ph_Et", &trig_EF_ph_Et, &b_trig_EF_ph_Et);
   fChain->SetBranchAddress("trig_EF_ph_pt", &trig_EF_ph_pt, &b_trig_EF_ph_pt);
   fChain->SetBranchAddress("trig_EF_ph_m", &trig_EF_ph_m, &b_trig_EF_ph_m);
   fChain->SetBranchAddress("trig_EF_ph_eta", &trig_EF_ph_eta, &b_trig_EF_ph_eta);
   fChain->SetBranchAddress("trig_EF_ph_phi", &trig_EF_ph_phi, &b_trig_EF_ph_phi);
   fChain->SetBranchAddress("trig_EF_ph_px", &trig_EF_ph_px, &b_trig_EF_ph_px);
   fChain->SetBranchAddress("trig_EF_ph_py", &trig_EF_ph_py, &b_trig_EF_ph_py);
   fChain->SetBranchAddress("trig_EF_ph_pz", &trig_EF_ph_pz, &b_trig_EF_ph_pz);
   fChain->SetBranchAddress("trig_EF_ph_EF_2g10_loose", &trig_EF_ph_EF_2g10_loose, &b_trig_EF_ph_EF_2g10_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_2g10_loose_mu6", &trig_EF_ph_EF_2g10_loose_mu6, &b_trig_EF_ph_EF_2g10_loose_mu6);
   fChain->SetBranchAddress("trig_EF_ph_EF_2g17i_tight", &trig_EF_ph_EF_2g17i_tight, &b_trig_EF_ph_EF_2g17i_tight);
   fChain->SetBranchAddress("trig_EF_ph_EF_2g20_loose", &trig_EF_ph_EF_2g20_loose, &b_trig_EF_ph_EF_2g20_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g105", &trig_EF_ph_EF_g105, &b_trig_EF_ph_EF_g105);
   fChain->SetBranchAddress("trig_EF_ph_EF_g10_loose", &trig_EF_ph_EF_g10_loose, &b_trig_EF_ph_EF_g10_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g10_loose_larcalib", &trig_EF_ph_EF_g10_loose_larcalib, &b_trig_EF_ph_EF_g10_loose_larcalib);
   fChain->SetBranchAddress("trig_EF_ph_EF_g10_nocut", &trig_EF_ph_EF_g10_nocut, &b_trig_EF_ph_EF_g10_nocut);
   fChain->SetBranchAddress("trig_EF_ph_EF_g150", &trig_EF_ph_EF_g150, &b_trig_EF_ph_EF_g150);
   fChain->SetBranchAddress("trig_EF_ph_EF_g15_loose", &trig_EF_ph_EF_g15_loose, &b_trig_EF_ph_EF_g15_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g20_loose", &trig_EF_ph_EF_g20_loose, &b_trig_EF_ph_EF_g20_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g20_loose_cnv", &trig_EF_ph_EF_g20_loose_cnv, &b_trig_EF_ph_EF_g20_loose_cnv);
   fChain->SetBranchAddress("trig_EF_ph_EF_g20_loose_larcalib", &trig_EF_ph_EF_g20_loose_larcalib, &b_trig_EF_ph_EF_g20_loose_larcalib);
   fChain->SetBranchAddress("trig_EF_ph_EF_g20_loose_xe20", &trig_EF_ph_EF_g20_loose_xe20, &b_trig_EF_ph_EF_g20_loose_xe20);
   fChain->SetBranchAddress("trig_EF_ph_EF_g20i_loose", &trig_EF_ph_EF_g20i_loose, &b_trig_EF_ph_EF_g20i_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g20i_loose_larcalib", &trig_EF_ph_EF_g20i_loose_larcalib, &b_trig_EF_ph_EF_g20i_loose_larcalib);
   fChain->SetBranchAddress("trig_EF_ph_EF_g25_loose", &trig_EF_ph_EF_g25_loose, &b_trig_EF_ph_EF_g25_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g25_loose_xe30", &trig_EF_ph_EF_g25_loose_xe30, &b_trig_EF_ph_EF_g25_loose_xe30);
   fChain->SetBranchAddress("trig_EF_ph_EF_g25i_loose", &trig_EF_ph_EF_g25i_loose, &b_trig_EF_ph_EF_g25i_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g55_tight", &trig_EF_ph_EF_g55_tight, &b_trig_EF_ph_EF_g55_tight);
   fChain->SetBranchAddress("trig_RoI_L2_e_n", &trig_RoI_L2_e_n, &b_trig_RoI_L2_e_n);
   fChain->SetBranchAddress("trig_RoI_L2_e_type", &trig_RoI_L2_e_type, &b_trig_RoI_L2_e_type);
   fChain->SetBranchAddress("trig_RoI_L2_e_lastStep", &trig_RoI_L2_e_lastStep, &b_trig_RoI_L2_e_lastStep);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgamma", &trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgamma, &b_trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgamma);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgammaStatus", &trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgammaStatus, &b_trig_RoI_L2_e_TrigRoiDescriptor_TrigT2CaloEgammaStatus);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigEMCluster", &trig_RoI_L2_e_TrigEMCluster, &b_trig_RoI_L2_e_TrigEMCluster);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigEMClusterStatus", &trig_RoI_L2_e_TrigEMClusterStatus, &b_trig_RoI_L2_e_TrigEMClusterStatus);
   fChain->SetBranchAddress("trig_RoI_L2_e_EmTau_ROI", &trig_RoI_L2_e_EmTau_ROI, &b_trig_RoI_L2_e_EmTau_ROI);
   fChain->SetBranchAddress("trig_RoI_L2_e_EmTau_ROIStatus", &trig_RoI_L2_e_EmTau_ROIStatus, &b_trig_RoI_L2_e_EmTau_ROIStatus);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma", &trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma, &b_trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus", &trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus, &b_trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma", &trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma, &b_trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus", &trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus, &b_trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigElectronContainer", &trig_RoI_L2_e_TrigElectronContainer, &b_trig_RoI_L2_e_TrigElectronContainer);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigElectronContainerStatus", &trig_RoI_L2_e_TrigElectronContainerStatus, &b_trig_RoI_L2_e_TrigElectronContainerStatus);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigPhotonContainer", &trig_RoI_L2_e_TrigPhotonContainer, &b_trig_RoI_L2_e_TrigPhotonContainer);
   fChain->SetBranchAddress("trig_RoI_L2_e_TrigPhotonContainerStatus", &trig_RoI_L2_e_TrigPhotonContainerStatus, &b_trig_RoI_L2_e_TrigPhotonContainerStatus);
   fChain->SetBranchAddress("trig_RoI_EF_e_n", &trig_RoI_EF_e_n, &b_trig_RoI_EF_e_n);
   fChain->SetBranchAddress("trig_RoI_EF_e_type", &trig_RoI_EF_e_type, &b_trig_RoI_EF_e_type);
   fChain->SetBranchAddress("trig_RoI_EF_e_lastStep", &trig_RoI_EF_e_lastStep, &b_trig_RoI_EF_e_lastStep);
   fChain->SetBranchAddress("trig_RoI_EF_e_EmTau_ROI", &trig_RoI_EF_e_EmTau_ROI, &b_trig_RoI_EF_e_EmTau_ROI);
   fChain->SetBranchAddress("trig_RoI_EF_e_EmTau_ROIStatus", &trig_RoI_EF_e_EmTau_ROIStatus, &b_trig_RoI_EF_e_EmTau_ROIStatus);
   fChain->SetBranchAddress("trig_RoI_EF_e_Rec::TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFID", &trig_RoI_EF_e_Rec__TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFID, &b_trig_RoI_EF_e_Rec__TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFID);
   fChain->SetBranchAddress("trig_RoI_EF_e_Rec::TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFIDStatus", &trig_RoI_EF_e_Rec__TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFIDStatus, &b_trig_RoI_EF_e_Rec__TrackParticleContainer_HLT_InDetTrigParticleCreation_Electron_EFIDStatus);
   fChain->SetBranchAddress("trig_RoI_EF_e_egammaContainer_egamma_Electrons", &trig_RoI_EF_e_egammaContainer_egamma_Electrons, &b_trig_RoI_EF_e_egammaContainer_egamma_Electrons);
   fChain->SetBranchAddress("trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus", &trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus, &b_trig_RoI_EF_e_egammaContainer_egamma_ElectronsStatus);
   fChain->SetBranchAddress("trig_RoI_EF_e_egammaContainer_egamma_Photons", &trig_RoI_EF_e_egammaContainer_egamma_Photons, &b_trig_RoI_EF_e_egammaContainer_egamma_Photons);
   fChain->SetBranchAddress("trig_RoI_EF_e_egammaContainer_egamma_PhotonsStatus", &trig_RoI_EF_e_egammaContainer_egamma_PhotonsStatus, &b_trig_RoI_EF_e_egammaContainer_egamma_PhotonsStatus);
   fChain->SetBranchAddress("trk_n", &trk_n, &b_trk_n);
   fChain->SetBranchAddress("trk_pt", &trk_pt, &b_trk_pt);
   fChain->SetBranchAddress("trk_eta", &trk_eta, &b_trk_eta);
   fChain->SetBranchAddress("trk_d0_wrtPV", &trk_d0_wrtPV, &b_trk_d0_wrtPV);
   fChain->SetBranchAddress("trk_z0_wrtPV", &trk_z0_wrtPV, &b_trk_z0_wrtPV);
   fChain->SetBranchAddress("trk_phi_wrtPV", &trk_phi_wrtPV, &b_trk_phi_wrtPV);
   fChain->SetBranchAddress("trk_cov_d0_wrtPV", &trk_cov_d0_wrtPV, &b_trk_cov_d0_wrtPV);
   fChain->SetBranchAddress("trk_cov_z0_wrtPV", &trk_cov_z0_wrtPV, &b_trk_cov_z0_wrtPV);
   fChain->SetBranchAddress("trk_cov_phi_wrtPV", &trk_cov_phi_wrtPV, &b_trk_cov_phi_wrtPV);
   fChain->SetBranchAddress("trk_cov_theta_wrtPV", &trk_cov_theta_wrtPV, &b_trk_cov_theta_wrtPV);
   fChain->SetBranchAddress("trk_cov_qoverp_wrtPV", &trk_cov_qoverp_wrtPV, &b_trk_cov_qoverp_wrtPV);
   fChain->SetBranchAddress("trk_chi2", &trk_chi2, &b_trk_chi2);
   fChain->SetBranchAddress("trk_ndof", &trk_ndof, &b_trk_ndof);
   fChain->SetBranchAddress("trk_nBLHits", &trk_nBLHits, &b_trk_nBLHits);
   fChain->SetBranchAddress("trk_nPixHits", &trk_nPixHits, &b_trk_nPixHits);
   fChain->SetBranchAddress("trk_nSCTHits", &trk_nSCTHits, &b_trk_nSCTHits);
   fChain->SetBranchAddress("trk_nTRTHits", &trk_nTRTHits, &b_trk_nTRTHits);
   fChain->SetBranchAddress("trk_nPixHoles", &trk_nPixHoles, &b_trk_nPixHoles);
   fChain->SetBranchAddress("trk_nSCTHoles", &trk_nSCTHoles, &b_trk_nSCTHoles);
   fChain->SetBranchAddress("trk_nMDTHits", &trk_nMDTHits, &b_trk_nMDTHits);
   fChain->SetBranchAddress("trk_nCSCEtaHits", &trk_nCSCEtaHits, &b_trk_nCSCEtaHits);
   fChain->SetBranchAddress("trk_nCSCPhiHits", &trk_nCSCPhiHits, &b_trk_nCSCPhiHits);
   fChain->SetBranchAddress("trk_nRPCEtaHits", &trk_nRPCEtaHits, &b_trk_nRPCEtaHits);
   fChain->SetBranchAddress("trk_nRPCPhiHits", &trk_nRPCPhiHits, &b_trk_nRPCPhiHits);
   fChain->SetBranchAddress("trk_nTGCEtaHits", &trk_nTGCEtaHits, &b_trk_nTGCEtaHits);
   fChain->SetBranchAddress("trk_nTGCPhiHits", &trk_nTGCPhiHits, &b_trk_nTGCPhiHits);
   fChain->SetBranchAddress("trk_nHits", &trk_nHits, &b_trk_nHits);
   fChain->SetBranchAddress("trk_nHoles", &trk_nHoles, &b_trk_nHoles);
   fChain->SetBranchAddress("trk_hitPattern", &trk_hitPattern, &b_trk_hitPattern);
   fChain->SetBranchAddress("trk_TRTHighTHitsRatio", &trk_TRTHighTHitsRatio, &b_trk_TRTHighTHitsRatio);
   fChain->SetBranchAddress("trk_pixeldEdx", &trk_pixeldEdx, &b_trk_pixeldEdx);
   fChain->SetBranchAddress("trk_fitter", &trk_fitter, &b_trk_fitter);
   fChain->SetBranchAddress("trk_patternReco1", &trk_patternReco1, &b_trk_patternReco1);
   fChain->SetBranchAddress("trk_patternReco2", &trk_patternReco2, &b_trk_patternReco2);
   fChain->SetBranchAddress("trk_seedFinder", &trk_seedFinder, &b_trk_seedFinder);
   fChain->SetBranchAddress("trk_blayerPrediction_expectHit", &trk_blayerPrediction_expectHit, &b_trk_blayerPrediction_expectHit);
   fChain->SetBranchAddress("trk_blayerPrediction_x", &trk_blayerPrediction_x, &b_trk_blayerPrediction_x);
   fChain->SetBranchAddress("trk_blayerPrediction_y", &trk_blayerPrediction_y, &b_trk_blayerPrediction_y);
   fChain->SetBranchAddress("trk_blayerPrediction_z", &trk_blayerPrediction_z, &b_trk_blayerPrediction_z);
   fChain->SetBranchAddress("trk_blayerPrediction_locX", &trk_blayerPrediction_locX, &b_trk_blayerPrediction_locX);
   fChain->SetBranchAddress("trk_blayerPrediction_locY", &trk_blayerPrediction_locY, &b_trk_blayerPrediction_locY);
   fChain->SetBranchAddress("trk_blayerPrediction_err_locX", &trk_blayerPrediction_err_locX, &b_trk_blayerPrediction_err_locX);
   fChain->SetBranchAddress("trk_blayerPrediction_err_locY", &trk_blayerPrediction_err_locY, &b_trk_blayerPrediction_err_locY);
   fChain->SetBranchAddress("trk_blayerPrediction_etaDistToEdge", &trk_blayerPrediction_etaDistToEdge, &b_trk_blayerPrediction_etaDistToEdge);
   fChain->SetBranchAddress("trk_blayerPrediction_phiDistToEdge", &trk_blayerPrediction_phiDistToEdge, &b_trk_blayerPrediction_phiDistToEdge);
   fChain->SetBranchAddress("trk_blayerPrediction_detElementId", &trk_blayerPrediction_detElementId, &b_trk_blayerPrediction_detElementId);
   fChain->SetBranchAddress("trk_blayerPrediction_row", &trk_blayerPrediction_row, &b_trk_blayerPrediction_row);
   fChain->SetBranchAddress("trk_blayerPrediction_col", &trk_blayerPrediction_col, &b_trk_blayerPrediction_col);
   fChain->SetBranchAddress("trk_blayerPrediction_type", &trk_blayerPrediction_type, &b_trk_blayerPrediction_type);
   fChain->SetBranchAddress("trk_mc_probability", &trk_mc_probability, &b_trk_mc_probability);
   fChain->SetBranchAddress("trk_mc_barcode", &trk_mc_barcode, &b_trk_mc_barcode);
   fChain->SetBranchAddress("trk_mc_index", &trk_mc_index, &b_trk_mc_index);
   fChain->SetBranchAddress("vxp_n", &vxp_n, &b_vxp_n);
   fChain->SetBranchAddress("vxp_x", &vxp_x, &b_vxp_x);
   fChain->SetBranchAddress("vxp_y", &vxp_y, &b_vxp_y);
   fChain->SetBranchAddress("vxp_z", &vxp_z, &b_vxp_z);
   fChain->SetBranchAddress("vxp_err_x", &vxp_err_x, &b_vxp_err_x);
   fChain->SetBranchAddress("vxp_err_y", &vxp_err_y, &b_vxp_err_y);
   fChain->SetBranchAddress("vxp_err_z", &vxp_err_z, &b_vxp_err_z);
   fChain->SetBranchAddress("vxp_chi2", &vxp_chi2, &b_vxp_chi2);
   fChain->SetBranchAddress("vxp_ndof", &vxp_ndof, &b_vxp_ndof);
   fChain->SetBranchAddress("vxp_px", &vxp_px, &b_vxp_px);
   fChain->SetBranchAddress("vxp_py", &vxp_py, &b_vxp_py);
   fChain->SetBranchAddress("vxp_pz", &vxp_pz, &b_vxp_pz);
   fChain->SetBranchAddress("vxp_E", &vxp_E, &b_vxp_E);
   fChain->SetBranchAddress("vxp_m", &vxp_m, &b_vxp_m);
   fChain->SetBranchAddress("vxp_nTracks", &vxp_nTracks, &b_vxp_nTracks);
   fChain->SetBranchAddress("vxp_sumPt", &vxp_sumPt, &b_vxp_sumPt);
   fChain->SetBranchAddress("vxp_type", &vxp_type, &b_vxp_type);
   fChain->SetBranchAddress("vxp_sumWeightInliers", &vxp_sumWeightInliers, &b_vxp_sumWeightInliers);
   fChain->SetBranchAddress("vxp_sumWeightOutliers", &vxp_sumWeightOutliers, &b_vxp_sumWeightOutliers);
   fChain->SetBranchAddress("vxp_sumWeightPileup", &vxp_sumWeightPileup, &b_vxp_sumWeightPileup);
   fChain->SetBranchAddress("vxp_sumWeightFakes", &vxp_sumWeightFakes, &b_vxp_sumWeightFakes);
   fChain->SetBranchAddress("vxp_nInliers", &vxp_nInliers, &b_vxp_nInliers);
   fChain->SetBranchAddress("vxp_nOutliers", &vxp_nOutliers, &b_vxp_nOutliers);
   fChain->SetBranchAddress("vxp_nPileUp", &vxp_nPileUp, &b_vxp_nPileUp);
   fChain->SetBranchAddress("vxp_nFakes", &vxp_nFakes, &b_vxp_nFakes);
   fChain->SetBranchAddress("vxp_purity", &vxp_purity, &b_vxp_purity);
   fChain->SetBranchAddress("vxp_trk_n", &vxp_trk_n, &b_vxp_trk_n);
   fChain->SetBranchAddress("vxp_trk_chi2", &vxp_trk_chi2, &b_vxp_trk_chi2);
   fChain->SetBranchAddress("vxp_trk_d0", &vxp_trk_d0, &b_vxp_trk_d0);
   fChain->SetBranchAddress("vxp_trk_z0", &vxp_trk_z0, &b_vxp_trk_z0);
   fChain->SetBranchAddress("vxp_trk_unbiased_d0", &vxp_trk_unbiased_d0, &b_vxp_trk_unbiased_d0);
   fChain->SetBranchAddress("vxp_trk_unbiased_z0", &vxp_trk_unbiased_z0, &b_vxp_trk_unbiased_z0);
   fChain->SetBranchAddress("vxp_trk_err_unbiased_d0", &vxp_trk_err_unbiased_d0, &b_vxp_trk_err_unbiased_d0);
   fChain->SetBranchAddress("vxp_trk_err_unbiased_z0", &vxp_trk_err_unbiased_z0, &b_vxp_trk_err_unbiased_z0);
   fChain->SetBranchAddress("vxp_trk_phi", &vxp_trk_phi, &b_vxp_trk_phi);
   fChain->SetBranchAddress("vxp_trk_theta", &vxp_trk_theta, &b_vxp_trk_theta);
   fChain->SetBranchAddress("vxp_trk_weight", &vxp_trk_weight, &b_vxp_trk_weight);
   fChain->SetBranchAddress("vxp_trk_index", &vxp_trk_index, &b_vxp_trk_index);
   fChain->SetBranchAddress("jetMETTriggerBitsFiller_L1_J10", &jetMETTriggerBitsFiller_L1_J10, &b_jetMETTriggerBitsFiller_L1_J10);
   fChain->SetBranchAddress("jetMETTriggerBitsFiller_L1_J15", &jetMETTriggerBitsFiller_L1_J15, &b_jetMETTriggerBitsFiller_L1_J15);
   fChain->SetBranchAddress("jetMETTriggerBitsFiller_L1_J5", &jetMETTriggerBitsFiller_L1_J5, &b_jetMETTriggerBitsFiller_L1_J5);
   fChain->SetBranchAddress("mcVx_n", &mcVx_n, &b_mcVx_n);
   fChain->SetBranchAddress("mcVx_x", &mcVx_x, &b_mcVx_x);
   fChain->SetBranchAddress("mcVx_y", &mcVx_y, &b_mcVx_y);
   fChain->SetBranchAddress("mcVx_z", &mcVx_z, &b_mcVx_z);
   fChain->SetBranchAddress("mcTrk_n", &mcTrk_n, &b_mcTrk_n);
   fChain->SetBranchAddress("mcTrk_gen_pt", &mcTrk_gen_pt, &b_mcTrk_gen_pt);
   fChain->SetBranchAddress("mcTrk_gen_eta", &mcTrk_gen_eta, &b_mcTrk_gen_eta);
   fChain->SetBranchAddress("mcTrk_gen_phi", &mcTrk_gen_phi, &b_mcTrk_gen_phi);
   fChain->SetBranchAddress("mcTrk_gen_energy", &mcTrk_gen_energy, &b_mcTrk_gen_energy);
   fChain->SetBranchAddress("mcTrk_perigee_ok", &mcTrk_perigee_ok, &b_mcTrk_perigee_ok);
   fChain->SetBranchAddress("mcTrk_perigee_d0", &mcTrk_perigee_d0, &b_mcTrk_perigee_d0);
   fChain->SetBranchAddress("mcTrk_perigee_z0", &mcTrk_perigee_z0, &b_mcTrk_perigee_z0);
   fChain->SetBranchAddress("mcTrk_perigee_phi", &mcTrk_perigee_phi, &b_mcTrk_perigee_phi);
   fChain->SetBranchAddress("mcTrk_perigee_theta", &mcTrk_perigee_theta, &b_mcTrk_perigee_theta);
   fChain->SetBranchAddress("mcTrk_perigee_qoverp", &mcTrk_perigee_qoverp, &b_mcTrk_perigee_qoverp);
   fChain->SetBranchAddress("mcTrk_pdg", &mcTrk_pdg, &b_mcTrk_pdg);
   fChain->SetBranchAddress("mcTrk_charge", &mcTrk_charge, &b_mcTrk_charge);
   fChain->SetBranchAddress("mcTrk_barcode", &mcTrk_barcode, &b_mcTrk_barcode);
   fChain->SetBranchAddress("mcTrk_status", &mcTrk_status, &b_mcTrk_status);
   fChain->SetBranchAddress("mcTrk_mother_pdg", &mcTrk_mother_pdg, &b_mcTrk_mother_pdg);
   fChain->SetBranchAddress("mcTrk_mother_barcode", &mcTrk_mother_barcode, &b_mcTrk_mother_barcode);
   fChain->SetBranchAddress("mcTrk_child_n", &mcTrk_child_n, &b_mcTrk_child_n);
   fChain->SetBranchAddress("mcTrk_child_barcode", &mcTrk_child_barcode, &b_mcTrk_child_barcode);
   fChain->SetBranchAddress("mcTrk_begVtx_x", &mcTrk_begVtx_x, &b_mcTrk_begVtx_x);
   fChain->SetBranchAddress("mcTrk_begVtx_y", &mcTrk_begVtx_y, &b_mcTrk_begVtx_y);
   fChain->SetBranchAddress("mcTrk_begVtx_z", &mcTrk_begVtx_z, &b_mcTrk_begVtx_z);
   fChain->SetBranchAddress("mcTrk_begVtx_barcode", &mcTrk_begVtx_barcode, &b_mcTrk_begVtx_barcode);
   fChain->SetBranchAddress("mcTrk_endVtx_x", &mcTrk_endVtx_x, &b_mcTrk_endVtx_x);
   fChain->SetBranchAddress("mcTrk_endVtx_y", &mcTrk_endVtx_y, &b_mcTrk_endVtx_y);
   fChain->SetBranchAddress("mcTrk_endVtx_z", &mcTrk_endVtx_z, &b_mcTrk_endVtx_z);
   fChain->SetBranchAddress("mcTrk_endVtx_barcode", &mcTrk_endVtx_barcode, &b_mcTrk_endVtx_barcode);
   fChain->SetBranchAddress("mc_n", &mc_n, &b_mc_n);
   fChain->SetBranchAddress("mc_pt", &mc_pt, &b_mc_pt);
   fChain->SetBranchAddress("mc_m", &mc_m, &b_mc_m);
   fChain->SetBranchAddress("mc_eta", &mc_eta, &b_mc_eta);
   fChain->SetBranchAddress("mc_phi", &mc_phi, &b_mc_phi);
   fChain->SetBranchAddress("mc_status", &mc_status, &b_mc_status);
   fChain->SetBranchAddress("mc_barcode", &mc_barcode, &b_mc_barcode);
   fChain->SetBranchAddress("mc_parents", &mc_parents, &b_mc_parents);
   fChain->SetBranchAddress("mc_children", &mc_children, &b_mc_children);
   fChain->SetBranchAddress("mc_pdgId", &mc_pdgId, &b_mc_pdgId);
   fChain->SetBranchAddress("mc_charge", &mc_charge, &b_mc_charge);
   fChain->SetBranchAddress("mc_vx_x", &mc_vx_x, &b_mc_vx_x);
   fChain->SetBranchAddress("mc_vx_y", &mc_vx_y, &b_mc_vx_y);
   fChain->SetBranchAddress("mc_vx_z", &mc_vx_z, &b_mc_vx_z);
   fChain->SetBranchAddress("mc_child_index", &mc_child_index, &b_mc_child_index);
   fChain->SetBranchAddress("mc_parent_index", &mc_parent_index, &b_mc_parent_index);
   fChain->SetBranchAddress("mcEvt_n", &mcEvt_n, &b_mcEvt_n);
   fChain->SetBranchAddress("mcEvt_signal_process_id", &mcEvt_signal_process_id, &b_mcEvt_signal_process_id);
   fChain->SetBranchAddress("mcEvt_event_number", &mcEvt_event_number, &b_mcEvt_event_number);
   fChain->SetBranchAddress("mcEvt_event_scale", &mcEvt_event_scale, &b_mcEvt_event_scale);
   fChain->SetBranchAddress("mcEvt_alphaQCD", &mcEvt_alphaQCD, &b_mcEvt_alphaQCD);
   fChain->SetBranchAddress("mcEvt_alphaQED", &mcEvt_alphaQED, &b_mcEvt_alphaQED);
   fChain->SetBranchAddress("mcEvt_pdf_id1", &mcEvt_pdf_id1, &b_mcEvt_pdf_id1);
   fChain->SetBranchAddress("mcEvt_pdf_id2", &mcEvt_pdf_id2, &b_mcEvt_pdf_id2);
   fChain->SetBranchAddress("mcEvt_pdf_x1", &mcEvt_pdf_x1, &b_mcEvt_pdf_x1);
   fChain->SetBranchAddress("mcEvt_pdf_x2", &mcEvt_pdf_x2, &b_mcEvt_pdf_x2);
   fChain->SetBranchAddress("mcEvt_pdf_scale", &mcEvt_pdf_scale, &b_mcEvt_pdf_scale);
   fChain->SetBranchAddress("mcEvt_pdf1", &mcEvt_pdf1, &b_mcEvt_pdf1);
   fChain->SetBranchAddress("mcEvt_pdf2", &mcEvt_pdf2, &b_mcEvt_pdf2);
   fChain->SetBranchAddress("mcEvt_weight", &mcEvt_weight, &b_mcEvt_weight);
   fChain->SetBranchAddress("AntiKt4Truth_n", &AntiKt4Truth_n, &b_AntiKt4Truth_n);
   fChain->SetBranchAddress("AntiKt4Truth_E", &AntiKt4Truth_E, &b_AntiKt4Truth_E);
   fChain->SetBranchAddress("AntiKt4Truth_pt", &AntiKt4Truth_pt, &b_AntiKt4Truth_pt);
   fChain->SetBranchAddress("AntiKt4Truth_m", &AntiKt4Truth_m, &b_AntiKt4Truth_m);
   fChain->SetBranchAddress("AntiKt4Truth_eta", &AntiKt4Truth_eta, &b_AntiKt4Truth_eta);
   fChain->SetBranchAddress("AntiKt4Truth_phi", &AntiKt4Truth_phi, &b_AntiKt4Truth_phi);
   fChain->SetBranchAddress("AntiKt4Truth_EtaPhys", &AntiKt4Truth_EtaPhys, &b_AntiKt4Truth_EtaPhys);
   fChain->SetBranchAddress("AntiKt4Truth_PhiPhys", &AntiKt4Truth_PhiPhys, &b_AntiKt4Truth_PhiPhys);
   fChain->SetBranchAddress("AntiKt4Truth_MPhys", &AntiKt4Truth_MPhys, &b_AntiKt4Truth_MPhys);
   fChain->SetBranchAddress("AntiKt4Truth_WIDTH", &AntiKt4Truth_WIDTH, &b_AntiKt4Truth_WIDTH);
   fChain->SetBranchAddress("AntiKt4Truth_n90", &AntiKt4Truth_n90, &b_AntiKt4Truth_n90);
   fChain->SetBranchAddress("AntiKt4Truth_Timing", &AntiKt4Truth_Timing, &b_AntiKt4Truth_Timing);
   fChain->SetBranchAddress("AntiKt4Truth_LArQuality", &AntiKt4Truth_LArQuality, &b_AntiKt4Truth_LArQuality);
   fChain->SetBranchAddress("AntiKt4Truth_nTrk", &AntiKt4Truth_nTrk, &b_AntiKt4Truth_nTrk);
   fChain->SetBranchAddress("AntiKt4Truth_sumPtTrk", &AntiKt4Truth_sumPtTrk, &b_AntiKt4Truth_sumPtTrk);
   fChain->SetBranchAddress("AntiKt4Truth_OriginIndex", &AntiKt4Truth_OriginIndex, &b_AntiKt4Truth_OriginIndex);
   fChain->SetBranchAddress("AntiKt4Truth_HECQuality", &AntiKt4Truth_HECQuality, &b_AntiKt4Truth_HECQuality);
   fChain->SetBranchAddress("AntiKt4Truth_NegativeE", &AntiKt4Truth_NegativeE, &b_AntiKt4Truth_NegativeE);
   fChain->SetBranchAddress("AntiKt4Truth_BCH_CORR_CELL", &AntiKt4Truth_BCH_CORR_CELL, &b_AntiKt4Truth_BCH_CORR_CELL);
   fChain->SetBranchAddress("AntiKt4Truth_BCH_CORR_DOTX", &AntiKt4Truth_BCH_CORR_DOTX, &b_AntiKt4Truth_BCH_CORR_DOTX);
   fChain->SetBranchAddress("AntiKt4Truth_BCH_CORR_JET", &AntiKt4Truth_BCH_CORR_JET, &b_AntiKt4Truth_BCH_CORR_JET);
   fChain->SetBranchAddress("AntiKt4Truth_BCH_CORR_JET_FORCELL", &AntiKt4Truth_BCH_CORR_JET_FORCELL, &b_AntiKt4Truth_BCH_CORR_JET_FORCELL);
   fChain->SetBranchAddress("AntiKt4Truth_ENG_BAD_CELLS", &AntiKt4Truth_ENG_BAD_CELLS, &b_AntiKt4Truth_ENG_BAD_CELLS);
   fChain->SetBranchAddress("AntiKt4Truth_N_BAD_CELLS", &AntiKt4Truth_N_BAD_CELLS, &b_AntiKt4Truth_N_BAD_CELLS);
   fChain->SetBranchAddress("AntiKt4Truth_N_BAD_CELLS_CORR", &AntiKt4Truth_N_BAD_CELLS_CORR, &b_AntiKt4Truth_N_BAD_CELLS_CORR);
   fChain->SetBranchAddress("AntiKt4Truth_BAD_CELLS_CORR_E", &AntiKt4Truth_BAD_CELLS_CORR_E, &b_AntiKt4Truth_BAD_CELLS_CORR_E);
   fChain->SetBranchAddress("AntiKt4Truth_SamplingMax", &AntiKt4Truth_SamplingMax, &b_AntiKt4Truth_SamplingMax);
   fChain->SetBranchAddress("AntiKt4Truth_fracSamplingMax", &AntiKt4Truth_fracSamplingMax, &b_AntiKt4Truth_fracSamplingMax);
   fChain->SetBranchAddress("AntiKt4Truth_hecf", &AntiKt4Truth_hecf, &b_AntiKt4Truth_hecf);
   fChain->SetBranchAddress("AntiKt4Truth_tgap3f", &AntiKt4Truth_tgap3f, &b_AntiKt4Truth_tgap3f);
   fChain->SetBranchAddress("AntiKt4Truth_isGood", &AntiKt4Truth_isGood, &b_AntiKt4Truth_isGood);
   fChain->SetBranchAddress("AntiKt4Truth_emfrac", &AntiKt4Truth_emfrac, &b_AntiKt4Truth_emfrac);
   fChain->SetBranchAddress("AntiKt4Truth_GCWJES", &AntiKt4Truth_GCWJES, &b_AntiKt4Truth_GCWJES);
   fChain->SetBranchAddress("AntiKt4Truth_EMJES", &AntiKt4Truth_EMJES, &b_AntiKt4Truth_EMJES);
   fChain->SetBranchAddress("AntiKt4Truth_CB", &AntiKt4Truth_CB, &b_AntiKt4Truth_CB);
   fChain->SetBranchAddress("AntiKt4Truth_emscale_E", &AntiKt4Truth_emscale_E, &b_AntiKt4Truth_emscale_E);
   fChain->SetBranchAddress("AntiKt4Truth_emscale_pt", &AntiKt4Truth_emscale_pt, &b_AntiKt4Truth_emscale_pt);
   fChain->SetBranchAddress("AntiKt4Truth_emscale_m", &AntiKt4Truth_emscale_m, &b_AntiKt4Truth_emscale_m);
   fChain->SetBranchAddress("AntiKt4Truth_emscale_eta", &AntiKt4Truth_emscale_eta, &b_AntiKt4Truth_emscale_eta);
   fChain->SetBranchAddress("AntiKt4Truth_emscale_phi", &AntiKt4Truth_emscale_phi, &b_AntiKt4Truth_emscale_phi);
   fChain->SetBranchAddress("AntiKt4Truth_jvtxf", &AntiKt4Truth_jvtxf, &b_AntiKt4Truth_jvtxf);
   fChain->SetBranchAddress("AntiKt4Truth_jvtx_x", &AntiKt4Truth_jvtx_x, &b_AntiKt4Truth_jvtx_x);
   fChain->SetBranchAddress("AntiKt4Truth_jvtx_y", &AntiKt4Truth_jvtx_y, &b_AntiKt4Truth_jvtx_y);
   fChain->SetBranchAddress("AntiKt4Truth_jvtx_z", &AntiKt4Truth_jvtx_z, &b_AntiKt4Truth_jvtx_z);
   fChain->SetBranchAddress("AntiKt4Truth_e_PreSamplerB", &AntiKt4Truth_e_PreSamplerB, &b_AntiKt4Truth_e_PreSamplerB);
   fChain->SetBranchAddress("AntiKt4Truth_e_EMB1", &AntiKt4Truth_e_EMB1, &b_AntiKt4Truth_e_EMB1);
   fChain->SetBranchAddress("AntiKt4Truth_e_EMB2", &AntiKt4Truth_e_EMB2, &b_AntiKt4Truth_e_EMB2);
   fChain->SetBranchAddress("AntiKt4Truth_e_EMB3", &AntiKt4Truth_e_EMB3, &b_AntiKt4Truth_e_EMB3);
   fChain->SetBranchAddress("AntiKt4Truth_e_PreSamplerE", &AntiKt4Truth_e_PreSamplerE, &b_AntiKt4Truth_e_PreSamplerE);
   fChain->SetBranchAddress("AntiKt4Truth_e_EME1", &AntiKt4Truth_e_EME1, &b_AntiKt4Truth_e_EME1);
   fChain->SetBranchAddress("AntiKt4Truth_e_EME2", &AntiKt4Truth_e_EME2, &b_AntiKt4Truth_e_EME2);
   fChain->SetBranchAddress("AntiKt4Truth_e_EME3", &AntiKt4Truth_e_EME3, &b_AntiKt4Truth_e_EME3);
   fChain->SetBranchAddress("AntiKt4Truth_e_HEC0", &AntiKt4Truth_e_HEC0, &b_AntiKt4Truth_e_HEC0);
   fChain->SetBranchAddress("AntiKt4Truth_e_HEC1", &AntiKt4Truth_e_HEC1, &b_AntiKt4Truth_e_HEC1);
   fChain->SetBranchAddress("AntiKt4Truth_e_HEC2", &AntiKt4Truth_e_HEC2, &b_AntiKt4Truth_e_HEC2);
   fChain->SetBranchAddress("AntiKt4Truth_e_HEC3", &AntiKt4Truth_e_HEC3, &b_AntiKt4Truth_e_HEC3);
   fChain->SetBranchAddress("AntiKt4Truth_e_TileBar0", &AntiKt4Truth_e_TileBar0, &b_AntiKt4Truth_e_TileBar0);
   fChain->SetBranchAddress("AntiKt4Truth_e_TileBar1", &AntiKt4Truth_e_TileBar1, &b_AntiKt4Truth_e_TileBar1);
   fChain->SetBranchAddress("AntiKt4Truth_e_TileBar2", &AntiKt4Truth_e_TileBar2, &b_AntiKt4Truth_e_TileBar2);
   fChain->SetBranchAddress("AntiKt4Truth_e_TileGap1", &AntiKt4Truth_e_TileGap1, &b_AntiKt4Truth_e_TileGap1);
   fChain->SetBranchAddress("AntiKt4Truth_e_TileGap2", &AntiKt4Truth_e_TileGap2, &b_AntiKt4Truth_e_TileGap2);
   fChain->SetBranchAddress("AntiKt4Truth_e_TileGap3", &AntiKt4Truth_e_TileGap3, &b_AntiKt4Truth_e_TileGap3);
   fChain->SetBranchAddress("AntiKt4Truth_e_TileExt0", &AntiKt4Truth_e_TileExt0, &b_AntiKt4Truth_e_TileExt0);
   fChain->SetBranchAddress("AntiKt4Truth_e_TileExt1", &AntiKt4Truth_e_TileExt1, &b_AntiKt4Truth_e_TileExt1);
   fChain->SetBranchAddress("AntiKt4Truth_e_TileExt2", &AntiKt4Truth_e_TileExt2, &b_AntiKt4Truth_e_TileExt2);
   fChain->SetBranchAddress("AntiKt4Truth_e_FCAL0", &AntiKt4Truth_e_FCAL0, &b_AntiKt4Truth_e_FCAL0);
   fChain->SetBranchAddress("AntiKt4Truth_e_FCAL1", &AntiKt4Truth_e_FCAL1, &b_AntiKt4Truth_e_FCAL1);
   fChain->SetBranchAddress("AntiKt4Truth_e_FCAL2", &AntiKt4Truth_e_FCAL2, &b_AntiKt4Truth_e_FCAL2);
   fChain->SetBranchAddress("AntiKt4Truth_shapeBins", &AntiKt4Truth_shapeBins, &b_AntiKt4Truth_shapeBins);
   fChain->SetBranchAddress("AntiKt4Truth_Nconst", &AntiKt4Truth_Nconst, &b_AntiKt4Truth_Nconst);
   fChain->SetBranchAddress("AntiKt4Truth_ptconst_emscale", &AntiKt4Truth_ptconst_emscale, &b_AntiKt4Truth_ptconst_emscale);
   fChain->SetBranchAddress("AntiKt4Truth_econst_emscale", &AntiKt4Truth_econst_emscale, &b_AntiKt4Truth_econst_emscale);
   fChain->SetBranchAddress("AntiKt4Truth_etaconst_emscale", &AntiKt4Truth_etaconst_emscale, &b_AntiKt4Truth_etaconst_emscale);
   fChain->SetBranchAddress("AntiKt4Truth_phiconst_emscale", &AntiKt4Truth_phiconst_emscale, &b_AntiKt4Truth_phiconst_emscale);
   fChain->SetBranchAddress("AntiKt4Truth_weightconst_emscale", &AntiKt4Truth_weightconst_emscale, &b_AntiKt4Truth_weightconst_emscale);
   fChain->SetBranchAddress("AntiKt4Truth_constscale_E", &AntiKt4Truth_constscale_E, &b_AntiKt4Truth_constscale_E);
   fChain->SetBranchAddress("AntiKt4Truth_constscale_pt", &AntiKt4Truth_constscale_pt, &b_AntiKt4Truth_constscale_pt);
   fChain->SetBranchAddress("AntiKt4Truth_constscale_m", &AntiKt4Truth_constscale_m, &b_AntiKt4Truth_constscale_m);
   fChain->SetBranchAddress("AntiKt4Truth_constscale_eta", &AntiKt4Truth_constscale_eta, &b_AntiKt4Truth_constscale_eta);
   fChain->SetBranchAddress("AntiKt4Truth_constscale_phi", &AntiKt4Truth_constscale_phi, &b_AntiKt4Truth_constscale_phi);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_ip2d_pu", &AntiKt4Truth_flavor_component_ip2d_pu, &b_AntiKt4Truth_flavor_component_ip2d_pu);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_ip2d_pb", &AntiKt4Truth_flavor_component_ip2d_pb, &b_AntiKt4Truth_flavor_component_ip2d_pb);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_ip2d_ntrk", &AntiKt4Truth_flavor_component_ip2d_ntrk, &b_AntiKt4Truth_flavor_component_ip2d_ntrk);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_ip3d_pu", &AntiKt4Truth_flavor_component_ip3d_pu, &b_AntiKt4Truth_flavor_component_ip3d_pu);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_ip3d_pb", &AntiKt4Truth_flavor_component_ip3d_pb, &b_AntiKt4Truth_flavor_component_ip3d_pb);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_ip3d_ntrk", &AntiKt4Truth_flavor_component_ip3d_ntrk, &b_AntiKt4Truth_flavor_component_ip3d_ntrk);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_sv1_pu", &AntiKt4Truth_flavor_component_sv1_pu, &b_AntiKt4Truth_flavor_component_sv1_pu);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_sv1_pb", &AntiKt4Truth_flavor_component_sv1_pb, &b_AntiKt4Truth_flavor_component_sv1_pb);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_sv2_pu", &AntiKt4Truth_flavor_component_sv2_pu, &b_AntiKt4Truth_flavor_component_sv2_pu);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_sv2_pb", &AntiKt4Truth_flavor_component_sv2_pb, &b_AntiKt4Truth_flavor_component_sv2_pb);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_svp_mass", &AntiKt4Truth_flavor_component_svp_mass, &b_AntiKt4Truth_flavor_component_svp_mass);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_svp_n2t", &AntiKt4Truth_flavor_component_svp_n2t, &b_AntiKt4Truth_flavor_component_svp_n2t);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_svp_svok", &AntiKt4Truth_flavor_component_svp_svok, &b_AntiKt4Truth_flavor_component_svp_svok);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_svp_ntrk", &AntiKt4Truth_flavor_component_svp_ntrk, &b_AntiKt4Truth_flavor_component_svp_ntrk);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_svp_ntrkv", &AntiKt4Truth_flavor_component_svp_ntrkv, &b_AntiKt4Truth_flavor_component_svp_ntrkv);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_svp_ntrkj", &AntiKt4Truth_flavor_component_svp_ntrkj, &b_AntiKt4Truth_flavor_component_svp_ntrkj);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_svp_efrc", &AntiKt4Truth_flavor_component_svp_efrc, &b_AntiKt4Truth_flavor_component_svp_efrc);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_sv0p_mass", &AntiKt4Truth_flavor_component_sv0p_mass, &b_AntiKt4Truth_flavor_component_sv0p_mass);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_sv0p_n2t", &AntiKt4Truth_flavor_component_sv0p_n2t, &b_AntiKt4Truth_flavor_component_sv0p_n2t);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_sv0p_svok", &AntiKt4Truth_flavor_component_sv0p_svok, &b_AntiKt4Truth_flavor_component_sv0p_svok);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_sv0p_ntrk", &AntiKt4Truth_flavor_component_sv0p_ntrk, &b_AntiKt4Truth_flavor_component_sv0p_ntrk);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_sv0p_ntrkv", &AntiKt4Truth_flavor_component_sv0p_ntrkv, &b_AntiKt4Truth_flavor_component_sv0p_ntrkv);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_sv0p_ntrkj", &AntiKt4Truth_flavor_component_sv0p_ntrkj, &b_AntiKt4Truth_flavor_component_sv0p_ntrkj);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_sv0p_efrc", &AntiKt4Truth_flavor_component_sv0p_efrc, &b_AntiKt4Truth_flavor_component_sv0p_efrc);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_jfit_pu", &AntiKt4Truth_flavor_component_jfit_pu, &b_AntiKt4Truth_flavor_component_jfit_pu);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_jfit_pb", &AntiKt4Truth_flavor_component_jfit_pb, &b_AntiKt4Truth_flavor_component_jfit_pb);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_jfit_nvtxnt", &AntiKt4Truth_flavor_component_jfit_nvtxnt, &b_AntiKt4Truth_flavor_component_jfit_nvtxnt);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_jfit_nvtx1t", &AntiKt4Truth_flavor_component_jfit_nvtx1t, &b_AntiKt4Truth_flavor_component_jfit_nvtx1t);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_jfit_ntrk", &AntiKt4Truth_flavor_component_jfit_ntrk, &b_AntiKt4Truth_flavor_component_jfit_ntrk);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_jfit_efrc", &AntiKt4Truth_flavor_component_jfit_efrc, &b_AntiKt4Truth_flavor_component_jfit_efrc);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_jfit_mass", &AntiKt4Truth_flavor_component_jfit_mass, &b_AntiKt4Truth_flavor_component_jfit_mass);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_component_jfit_sig3d", &AntiKt4Truth_flavor_component_jfit_sig3d, &b_AntiKt4Truth_flavor_component_jfit_sig3d);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight", &AntiKt4Truth_flavor_weight, &b_AntiKt4Truth_flavor_weight);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_TrackCounting2D", &AntiKt4Truth_flavor_weight_TrackCounting2D, &b_AntiKt4Truth_flavor_weight_TrackCounting2D);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_JetProb", &AntiKt4Truth_flavor_weight_JetProb, &b_AntiKt4Truth_flavor_weight_JetProb);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_IP1D", &AntiKt4Truth_flavor_weight_IP1D, &b_AntiKt4Truth_flavor_weight_IP1D);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_IP2D", &AntiKt4Truth_flavor_weight_IP2D, &b_AntiKt4Truth_flavor_weight_IP2D);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_IP3D", &AntiKt4Truth_flavor_weight_IP3D, &b_AntiKt4Truth_flavor_weight_IP3D);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_SV0", &AntiKt4Truth_flavor_weight_SV0, &b_AntiKt4Truth_flavor_weight_SV0);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_SV1", &AntiKt4Truth_flavor_weight_SV1, &b_AntiKt4Truth_flavor_weight_SV1);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_SV2", &AntiKt4Truth_flavor_weight_SV2, &b_AntiKt4Truth_flavor_weight_SV2);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_JetFitterTag", &AntiKt4Truth_flavor_weight_JetFitterTag, &b_AntiKt4Truth_flavor_weight_JetFitterTag);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_JetFitterCOMB", &AntiKt4Truth_flavor_weight_JetFitterCOMB, &b_AntiKt4Truth_flavor_weight_JetFitterCOMB);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_JetFitterTagNN", &AntiKt4Truth_flavor_weight_JetFitterTagNN, &b_AntiKt4Truth_flavor_weight_JetFitterTagNN);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_JetFitterCOMBNN", &AntiKt4Truth_flavor_weight_JetFitterCOMBNN, &b_AntiKt4Truth_flavor_weight_JetFitterCOMBNN);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_SoftMuonTag", &AntiKt4Truth_flavor_weight_SoftMuonTag, &b_AntiKt4Truth_flavor_weight_SoftMuonTag);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_weight_SoftElectronTag", &AntiKt4Truth_flavor_weight_SoftElectronTag, &b_AntiKt4Truth_flavor_weight_SoftElectronTag);
   fChain->SetBranchAddress("AntiKt4Truth_RoIword", &AntiKt4Truth_RoIword, &b_AntiKt4Truth_RoIword);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_truth_dRminToB", &AntiKt4Truth_flavor_truth_dRminToB, &b_AntiKt4Truth_flavor_truth_dRminToB);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_truth_dRminToC", &AntiKt4Truth_flavor_truth_dRminToC, &b_AntiKt4Truth_flavor_truth_dRminToC);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_truth_dRminToT", &AntiKt4Truth_flavor_truth_dRminToT, &b_AntiKt4Truth_flavor_truth_dRminToT);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_truth_BHadronpdg", &AntiKt4Truth_flavor_truth_BHadronpdg, &b_AntiKt4Truth_flavor_truth_BHadronpdg);
   fChain->SetBranchAddress("AntiKt4Truth_flavor_truth_trueflav", &AntiKt4Truth_flavor_truth_trueflav, &b_AntiKt4Truth_flavor_truth_trueflav);
   fChain->SetBranchAddress("AntiKt4Truth_el_dr", &AntiKt4Truth_el_dr, &b_AntiKt4Truth_el_dr);
   fChain->SetBranchAddress("AntiKt4Truth_el_matched", &AntiKt4Truth_el_matched, &b_AntiKt4Truth_el_matched);
   fChain->SetBranchAddress("AntiKt4Truth_mu_dr", &AntiKt4Truth_mu_dr, &b_AntiKt4Truth_mu_dr);
   fChain->SetBranchAddress("AntiKt4Truth_mu_matched", &AntiKt4Truth_mu_matched, &b_AntiKt4Truth_mu_matched);
   Notify();
}

Bool_t BjetTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void BjetTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t BjetTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef BjetTree_cxx
