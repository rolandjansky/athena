//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Apr  8 10:02:30 2011 by ROOT version 5.28/00
// from TTree physics/physics
// found on file: physics.root
//////////////////////////////////////////////////////////

#ifndef physics_h
#define physics_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class TTree;
class TBranch;

class physics {

public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
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
   Int_t           lar_ncellA;
   Int_t           lar_ncellC;
   Float_t         lar_energyA;
   Float_t         lar_energyC;
   Float_t         lar_timeA;
   Float_t         lar_timeC;
   Float_t         lar_timeDiff;
   Int_t           er_n;
   vector<vector<float> > *er_rings;
   Int_t           er_nt_n;
   vector<vector<float> > *er_nt_rings;
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
   vector<unsigned int> *ph_OQ;
   vector<unsigned int> *ph_OQRecalc;
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
   vector<int>     *ph_truth_index;
   vector<int>     *ph_truth_matched;
   vector<int>     *ph_loose;
   vector<int>     *ph_tight;
   vector<int>     *ph_tightIso;
   vector<int>     *ph_goodOQ;
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
   vector<float>   *ph_Etcone15;
   vector<float>   *ph_Etcone20;
   vector<float>   *ph_Etcone25;
   vector<float>   *ph_Etcone30;
   vector<float>   *ph_Etcone35;
   vector<float>   *ph_Etcone40;
   vector<float>   *ph_ptcone20;
   vector<float>   *ph_ptcone30;
   vector<float>   *ph_ptcone40;
   vector<float>   *ph_nucone20;
   vector<float>   *ph_nucone30;
   vector<float>   *ph_nucone40;
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
   vector<float>   *ph_Etcone35_pt_corrected;
   vector<float>   *ph_Etcone30_pt_corrected;
   vector<float>   *ph_Etcone25_pt_corrected;
   vector<float>   *ph_Etcone20_pt_corrected;
   vector<float>   *ph_Etcone15_pt_corrected;
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
   Bool_t          EF_2g10_loose;
   Bool_t          EF_2g15_loose;
   Bool_t          EF_2g20_loose;
   Bool_t          EF_2g5_loose;
   Bool_t          EF_2g7_loose;
   Bool_t          EF_2j35_jetNoEF_xe20_noMu;
   Bool_t          EF_2j35_jetNoEF_xe30_noMu;
   Bool_t          EF_2j35_jetNoEF_xe40_noMu;
   Bool_t          EF_2j35_xe20_noMu;
   Bool_t          EF_2j35_xe30_noMu;
   Bool_t          EF_2j35_xe40_noMu;
   Bool_t          EF_2mu0_NoAlg;
   Bool_t          EF_2mu10;
   Bool_t          EF_2mu10_NoAlg;
   Bool_t          EF_2mu4;
   Bool_t          EF_2mu4_Bmumu;
   Bool_t          EF_2mu4_Bmumux;
   Bool_t          EF_2mu4_DiMu;
   Bool_t          EF_2mu4_DiMu_SiTrk;
   Bool_t          EF_2mu4_DiMu_noVtx;
   Bool_t          EF_2mu4_DiMu_noVtx_noOS;
   Bool_t          EF_2mu4_Jpsimumu;
   Bool_t          EF_2mu4_Jpsimumu_IDTrkNoCut;
   Bool_t          EF_2mu4_Upsimumu;
   Bool_t          EF_2mu6;
   Bool_t          EF_2mu6_Bmumu;
   Bool_t          EF_2mu6_Bmumux;
   Bool_t          EF_2mu6_DiMu;
   Bool_t          EF_2mu6_Jpsimumu;
   Bool_t          EF_2mu6_MG;
   Bool_t          EF_2mu6_NoAlg;
   Bool_t          EF_2mu6_Upsimumu;
   Bool_t          EF_2mu6_g10_loose;
   Bool_t          EF_e20_loose_xe20;
   Bool_t          EF_e20_loose_xe20_noMu;
   Bool_t          EF_e20_loose_xe30;
   Bool_t          EF_e20_loose_xe30_noMu;
   Bool_t          EF_g10_loose;
   Bool_t          EF_g10_loose_larcalib;
   Bool_t          EF_g11_etcut;
   Bool_t          EF_g15_loose;
   Bool_t          EF_g17_etcut;
   Bool_t          EF_g17_etcut_EFxe20_noMu;
   Bool_t          EF_g17_etcut_EFxe30_noMu;
   Bool_t          EF_g20_loose;
   Bool_t          EF_g20_loose_cnv;
   Bool_t          EF_g20_loose_larcalib;
   Bool_t          EF_g20_loose_xe20_noMu;
   Bool_t          EF_g20_loose_xe30_noMu;
   Bool_t          EF_g20_tight;
   Bool_t          EF_g25_loose_xe30_noMu;
   Bool_t          EF_g30_loose;
   Bool_t          EF_g30_tight;
   Bool_t          EF_g3_NoCut_unpaired_iso;
   Bool_t          EF_g3_NoCut_unpaired_noniso;
   Bool_t          EF_g40_loose;
   Bool_t          EF_g40_loose_larcalib;
   Bool_t          EF_g40_tight;
   Bool_t          EF_g50_loose;
   Bool_t          EF_g50_loose_larcalib;
   Bool_t          EF_g5_NoCut_cosmic;
   Bool_t          EF_g5_loose;
   Bool_t          EF_g5_loose_cnv;
   Bool_t          EF_g5_loose_larcalib;
   Bool_t          EF_g7_loose;
   Bool_t          EF_j35_jetNoCut_xe30_e15_medium;
   Bool_t          EF_j35_jetNoCut_xe30_mu15;
   Bool_t          EF_j35_xe30_e15_medium;
   Bool_t          EF_j35_xe30_mu15;
   Bool_t          EF_j50_jetNoEF_xe20_noMu;
   Bool_t          EF_j50_jetNoEF_xe30_noMu;
   Bool_t          EF_j50_jetNoEF_xe40_noMu;
   Bool_t          EF_j50_xe20_noMu;
   Bool_t          EF_j50_xe30_noMu;
   Bool_t          EF_j50_xe40_noMu;
   Bool_t          EF_mu0_NoAlg;
   Bool_t          EF_mu0_comm_NoAlg;
   Bool_t          EF_mu0_comm_empty_NoAlg;
   Bool_t          EF_mu0_comm_firstempty_NoAlg;
   Bool_t          EF_mu0_comm_unpaired_iso_NoAlg;
   Bool_t          EF_mu0_comm_unpaired_noniso_NoAlg;
   Bool_t          EF_mu0_empty_NoAlg;
   Bool_t          EF_mu0_firstempty_NoAlg;
   Bool_t          EF_mu0_missingRoi;
   Bool_t          EF_mu0_outOfTime1;
   Bool_t          EF_mu0_outOfTime2;
   Bool_t          EF_mu0_rpcOnly;
   Bool_t          EF_mu0_unpaired_iso_NoAlg;
   Bool_t          EF_mu0_unpaired_noniso_NoAlg;
   Bool_t          EF_mu10;
   Bool_t          EF_mu10_Ecut12;
   Bool_t          EF_mu10_Ecut123;
   Bool_t          EF_mu10_Ecut13;
   Bool_t          EF_mu10_IDTrkNoCut;
   Bool_t          EF_mu10_MG;
   Bool_t          EF_mu10_MSonly;
   Bool_t          EF_mu10_MSonly_Ecut12;
   Bool_t          EF_mu10_MSonly_Ecut123;
   Bool_t          EF_mu10_MSonly_Ecut13;
   Bool_t          EF_mu10_MSonly_tight;
   Bool_t          EF_mu10_NoAlg;
   Bool_t          EF_mu10_SiTrk;
   Bool_t          EF_mu10_j30;
   Bool_t          EF_mu10_tight;
   Bool_t          EF_mu10i_loose;
   Bool_t          EF_mu13;
   Bool_t          EF_mu13_MG;
   Bool_t          EF_mu13_MG_tight;
   Bool_t          EF_mu13_tight;
   Bool_t          EF_mu15;
   Bool_t          EF_mu15_MG;
   Bool_t          EF_mu15_NoAlg;
   Bool_t          EF_mu20;
   Bool_t          EF_mu20_MSonly;
   Bool_t          EF_mu20_NoAlg;
   Bool_t          EF_mu20_passHLT;
   Bool_t          EF_mu20_slow;
   Bool_t          EF_mu30_MSonly;
   Bool_t          EF_mu4;
   Bool_t          EF_mu40_MSonly;
   Bool_t          EF_mu4_Bmumu;
   Bool_t          EF_mu4_BmumuX;
   Bool_t          EF_mu4_DiMu;
   Bool_t          EF_mu4_DiMu_FS;
   Bool_t          EF_mu4_DiMu_FS_noOS;
   Bool_t          EF_mu4_DiMu_MG;
   Bool_t          EF_mu4_DiMu_MG_FS;
   Bool_t          EF_mu4_DiMu_SiTrk;
   Bool_t          EF_mu4_DiMu_SiTrk_FS;
   Bool_t          EF_mu4_DiMu_noOS;
   Bool_t          EF_mu4_IDTrkNoCut;
   Bool_t          EF_mu4_Jpsie5e3;
   Bool_t          EF_mu4_Jpsie5e3_FS;
   Bool_t          EF_mu4_Jpsie5e3_SiTrk;
   Bool_t          EF_mu4_Jpsie5e3_SiTrk_FS;
   Bool_t          EF_mu4_Jpsimumu;
   Bool_t          EF_mu4_Jpsimumu_FS;
   Bool_t          EF_mu4_Jpsimumu_SiTrk_FS;
   Bool_t          EF_mu4_L1J10_matched;
   Bool_t          EF_mu4_L1J15_matched;
   Bool_t          EF_mu4_L1J30_matched;
   Bool_t          EF_mu4_L1J55_matched;
   Bool_t          EF_mu4_L1J5_matched;
   Bool_t          EF_mu4_L2MSonly_EFFS_passL2;
   Bool_t          EF_mu4_MG;
   Bool_t          EF_mu4_MSonly;
   Bool_t          EF_mu4_MSonly_EFFS_passL2;
   Bool_t          EF_mu4_MSonly_MB2_noL2_EFFS;
   Bool_t          EF_mu4_MSonly_cosmic;
   Bool_t          EF_mu4_MSonly_outOfTime;
   Bool_t          EF_mu4_MV;
   Bool_t          EF_mu4_SiTrk;
   Bool_t          EF_mu4_Trk_Jpsi;
   Bool_t          EF_mu4_Trk_Jpsi_FS;
   Bool_t          EF_mu4_Trk_Jpsi_loose;
   Bool_t          EF_mu4_Trk_Upsi_FS;
   Bool_t          EF_mu4_Trk_Upsi_loose_FS;
   Bool_t          EF_mu4_Upsimumu_FS;
   Bool_t          EF_mu4_Upsimumu_SiTrk_FS;
   Bool_t          EF_mu4_comm_MSonly_cosmic;
   Bool_t          EF_mu4_comm_cosmic;
   Bool_t          EF_mu4_comm_firstempty;
   Bool_t          EF_mu4_comm_unpaired_iso;
   Bool_t          EF_mu4_cosmic;
   Bool_t          EF_mu4_firstempty;
   Bool_t          EF_mu4_j20;
   Bool_t          EF_mu4_j20_jetNoEF;
   Bool_t          EF_mu4_j30;
   Bool_t          EF_mu4_j30_jetNoEF;
   Bool_t          EF_mu4_mu6;
   Bool_t          EF_mu4_muCombTag;
   Bool_t          EF_mu4_tile;
   Bool_t          EF_mu4_tile_cosmic;
   Bool_t          EF_mu4_unpaired_iso;
   Bool_t          EF_mu4mu6_Bmumu;
   Bool_t          EF_mu4mu6_BmumuX;
   Bool_t          EF_mu4mu6_DiMu;
   Bool_t          EF_mu4mu6_Jpsimumu;
   Bool_t          EF_mu4mu6_Jpsimumu_IDTrkNoCut;
   Bool_t          EF_mu4mu6_Upsimumu;
   Bool_t          EF_mu6;
   Bool_t          EF_mu6_Bmumu;
   Bool_t          EF_mu6_BmumuX;
   Bool_t          EF_mu6_DiMu;
   Bool_t          EF_mu6_Ecut12;
   Bool_t          EF_mu6_Ecut123;
   Bool_t          EF_mu6_Ecut13;
   Bool_t          EF_mu6_Ecut2;
   Bool_t          EF_mu6_Ecut3;
   Bool_t          EF_mu6_IDTrkNoCut;
   Bool_t          EF_mu6_Jpsie5e3;
   Bool_t          EF_mu6_Jpsie5e3_FS;
   Bool_t          EF_mu6_Jpsie5e3_SiTrk;
   Bool_t          EF_mu6_Jpsie5e3_SiTrk_FS;
   Bool_t          EF_mu6_Jpsimumu;
   Bool_t          EF_mu6_MG;
   Bool_t          EF_mu6_MSonly;
   Bool_t          EF_mu6_MSonly_Ecut12;
   Bool_t          EF_mu6_MSonly_Ecut123;
   Bool_t          EF_mu6_MSonly_Ecut13;
   Bool_t          EF_mu6_MSonly_Ecut2;
   Bool_t          EF_mu6_MSonly_Ecut3;
   Bool_t          EF_mu6_MSonly_outOfTime;
   Bool_t          EF_mu6_NoAlg;
   Bool_t          EF_mu6_SiTrk;
   Bool_t          EF_mu6_Trk_Jpsi;
   Bool_t          EF_mu6_Upsimumu_FS;
   Bool_t          EF_mu6_muCombTag;
   Bool_t          EF_tau12_loose_IdScan_xe15_noMu;
   Bool_t          EF_tau12_loose_xe15_noMu;
   Bool_t          EF_tau12_loose_xe20_noMu;
   Bool_t          EF_tau16_loose_xe20_noMu;
   Bool_t          EF_tau16_loose_xe25_noMu;
   Bool_t          EF_tau16_loose_xe25_tight_noMu;
   Bool_t          EF_tau16_loose_xe30_noMu;
   Bool_t          EF_tau16_medium_xe22_noMu;
   Bool_t          EF_tau16_medium_xe25_noMu;
   Bool_t          EF_tau16_medium_xe25_tight_noMu;
   Bool_t          EF_tau20_loose_xe25_noMu;
   Bool_t          EF_tauNoCut_hasTrk6_IdScan_xe20_noMu;
   Bool_t          EF_tauNoCut_hasTrk6_xe20_noMu;
   Bool_t          EF_tauNoCut_hasTrk9_xe20_noMu;
   Bool_t          EF_tauNoCut_hasTrk_xe20_noMu;
   Bool_t          EF_xe15;
   Bool_t          EF_xe15_noMu;
   Bool_t          EF_xe15_unbiased_noMu;
   Bool_t          EF_xe20;
   Bool_t          EF_xe20_noMu;
   Bool_t          EF_xe20_tight_noMu;
   Bool_t          EF_xe20_tight_vfj_noMu;
   Bool_t          EF_xe25;
   Bool_t          EF_xe25_medium;
   Bool_t          EF_xe25_medium_noMu;
   Bool_t          EF_xe25_noMu;
   Bool_t          EF_xe25_tight_noMu;
   Bool_t          EF_xe25_tight_vfj_noMu;
   Bool_t          EF_xe25_vfj_noMu;
   Bool_t          EF_xe30;
   Bool_t          EF_xe30_allL1;
   Bool_t          EF_xe30_allL1_FEB;
   Bool_t          EF_xe30_allL1_allCells;
   Bool_t          EF_xe30_allL1_noMu;
   Bool_t          EF_xe30_loose;
   Bool_t          EF_xe30_loose_noMu;
   Bool_t          EF_xe30_medium;
   Bool_t          EF_xe30_medium_noMu;
   Bool_t          EF_xe30_medium_vfj_noMu;
   Bool_t          EF_xe30_noMu;
   Bool_t          EF_xe30_tight_noMu;
   Bool_t          EF_xe30_tight_vfj_noMu;
   Bool_t          EF_xe30_vfj_noMu;
   Bool_t          EF_xe35;
   Bool_t          EF_xe35_loose_noMu;
   Bool_t          EF_xe35_noMu;
   Bool_t          EF_xe35_tight_noMu;
   Bool_t          EF_xe40;
   Bool_t          EF_xe40_noMu;
   Bool_t          EF_xe40_tight_noMu;
   Bool_t          EF_xe45;
   Bool_t          EF_xe45_noMu;
   Bool_t          EF_xe55;
   Bool_t          EF_xe55_noMu;
   Bool_t          EF_xe60_medium;
   Bool_t          EF_xe60_medium_noMu;
   Bool_t          EF_xe80_medium;
   Bool_t          EF_xe80_medium_noMu;
   Bool_t          L1_2EM10;
   Bool_t          L1_2EM14;
   Bool_t          L1_2EM2;
   Bool_t          L1_2EM3;
   Bool_t          L1_2EM5;
   Bool_t          L1_2J15_XE10;
   Bool_t          L1_2J15_XE15;
   Bool_t          L1_2J15_XE25;
   Bool_t          L1_2MU0;
   Bool_t          L1_2MU0_FIRSTEMPTY;
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
   Bool_t          L1_EM2;
   Bool_t          L1_EM2_UNPAIRED_ISO;
   Bool_t          L1_EM2_UNPAIRED_NONISO;
   Bool_t          L1_EM3;
   Bool_t          L1_EM3_EMPTY;
   Bool_t          L1_EM3_FIRSTEMPTY;
   Bool_t          L1_EM3_MV;
   Bool_t          L1_EM5;
   Bool_t          L1_EM5_MU10;
   Bool_t          L1_EM5_MU6;
   Bool_t          L1_EM85;
   Bool_t          L1_J15_XE15_EM10;
   Bool_t          L1_J15_XE15_MU15;
   Bool_t          L1_J30_XE10;
   Bool_t          L1_J30_XE15;
   Bool_t          L1_J30_XE25;
   Bool_t          L1_MU0;
   Bool_t          L1_MU0_COMM;
   Bool_t          L1_MU0_COMM_EMPTY;
   Bool_t          L1_MU0_COMM_FIRSTEMPTY;
   Bool_t          L1_MU0_COMM_UNPAIRED_ISO;
   Bool_t          L1_MU0_COMM_UNPAIRED_NONISO;
   Bool_t          L1_MU0_EM3;
   Bool_t          L1_MU0_EMPTY;
   Bool_t          L1_MU0_FIRSTEMPTY;
   Bool_t          L1_MU0_J10;
   Bool_t          L1_MU0_J15;
   Bool_t          L1_MU0_J30;
   Bool_t          L1_MU0_J5;
   Bool_t          L1_MU0_J55;
   Bool_t          L1_MU0_MV;
   Bool_t          L1_MU0_UNPAIRED_ISO;
   Bool_t          L1_MU0_UNPAIRED_NONISO;
   Bool_t          L1_MU10;
   Bool_t          L1_MU10_FIRSTEMPTY;
   Bool_t          L1_MU10_J10;
   Bool_t          L1_MU15;
   Bool_t          L1_MU20;
   Bool_t          L1_MU6;
   Bool_t          L1_MU6_EM3;
   Bool_t          L1_MU6_FIRSTEMPTY;
   Bool_t          L1_MU6_J5;
   Bool_t          L1_TAU5_XE10;
   Bool_t          L1_TAU6_XE10;
   Bool_t          L1_XE10;
   Bool_t          L1_XE15;
   Bool_t          L1_XE20;
   Bool_t          L1_XE25;
   Bool_t          L1_XE30;
   Bool_t          L1_XE35;
   Bool_t          L1_XE40;
   Bool_t          L1_XE50;
   Bool_t          L2_2g10_loose;
   Bool_t          L2_2g15_loose;
   Bool_t          L2_2g20_loose;
   Bool_t          L2_2g5_loose;
   Bool_t          L2_2g7_loose;
   Bool_t          L2_2j30_xe12_noMu;
   Bool_t          L2_2j30_xe20_noMu;
   Bool_t          L2_2j30_xe30_noMu;
   Bool_t          L2_2j35_jetNoEF_xe20_noMu;
   Bool_t          L2_2j35_jetNoEF_xe30_noMu;
   Bool_t          L2_2j35_jetNoEF_xe40_noMu;
   Bool_t          L2_2mu0_NoAlg;
   Bool_t          L2_2mu10;
   Bool_t          L2_2mu10_NoAlg;
   Bool_t          L2_2mu4;
   Bool_t          L2_2mu4_Bmumu;
   Bool_t          L2_2mu4_Bmumux;
   Bool_t          L2_2mu4_DiMu;
   Bool_t          L2_2mu4_DiMu_SiTrk;
   Bool_t          L2_2mu4_DiMu_noVtx;
   Bool_t          L2_2mu4_DiMu_noVtx_noOS;
   Bool_t          L2_2mu4_Jpsimumu;
   Bool_t          L2_2mu4_Jpsimumu_IDTrkNoCut;
   Bool_t          L2_2mu4_Upsimumu;
   Bool_t          L2_2mu6;
   Bool_t          L2_2mu6_Bmumu;
   Bool_t          L2_2mu6_Bmumux;
   Bool_t          L2_2mu6_DiMu;
   Bool_t          L2_2mu6_Jpsimumu;
   Bool_t          L2_2mu6_MG;
   Bool_t          L2_2mu6_NoAlg;
   Bool_t          L2_2mu6_Upsimumu;
   Bool_t          L2_2mu6_g10_loose;
   Bool_t          L2_e20_loose_xe20;
   Bool_t          L2_e20_loose_xe20_noMu;
   Bool_t          L2_e20_loose_xe30;
   Bool_t          L2_e20_loose_xe30_noMu;
   Bool_t          L2_g10_loose;
   Bool_t          L2_g11_etcut;
   Bool_t          L2_g15_loose;
   Bool_t          L2_g17_etcut;
   Bool_t          L2_g17_etcut_EFxe20_noMu;
   Bool_t          L2_g17_etcut_EFxe30_noMu;
   Bool_t          L2_g20_loose;
   Bool_t          L2_g20_loose_cnv;
   Bool_t          L2_g20_loose_xe20_noMu;
   Bool_t          L2_g20_loose_xe30_noMu;
   Bool_t          L2_g20_tight;
   Bool_t          L2_g25_loose_xe30_noMu;
   Bool_t          L2_g30_loose;
   Bool_t          L2_g30_tight;
   Bool_t          L2_g3_NoCut_unpaired_iso;
   Bool_t          L2_g3_NoCut_unpaired_noniso;
   Bool_t          L2_g40_loose;
   Bool_t          L2_g40_tight;
   Bool_t          L2_g50_loose;
   Bool_t          L2_g5_NoCut_cosmic;
   Bool_t          L2_g5_loose;
   Bool_t          L2_g5_loose_cnv;
   Bool_t          L2_g7_loose;
   Bool_t          L2_j30_jetNoCut_xe20_e15_medium;
   Bool_t          L2_j30_xe20_e15_medium;
   Bool_t          L2_j35_jetNoCut_xe30_mu15;
   Bool_t          L2_j35_xe30_mu15;
   Bool_t          L2_j45_xe12_noMu;
   Bool_t          L2_j45_xe20_noMu;
   Bool_t          L2_j45_xe30_noMu;
   Bool_t          L2_mu0_NoAlg;
   Bool_t          L2_mu0_comm_NoAlg;
   Bool_t          L2_mu0_comm_empty_NoAlg;
   Bool_t          L2_mu0_comm_firstempty_NoAlg;
   Bool_t          L2_mu0_comm_unpaired_iso_NoAlg;
   Bool_t          L2_mu0_comm_unpaired_noniso_NoAlg;
   Bool_t          L2_mu0_empty_NoAlg;
   Bool_t          L2_mu0_firstempty_NoAlg;
   Bool_t          L2_mu0_missingRoi;
   Bool_t          L2_mu0_outOfTime1;
   Bool_t          L2_mu0_outOfTime2;
   Bool_t          L2_mu0_rpcOnly;
   Bool_t          L2_mu0_unpaired_iso_NoAlg;
   Bool_t          L2_mu0_unpaired_noniso_NoAlg;
   Bool_t          L2_mu10;
   Bool_t          L2_mu10_Ecut12;
   Bool_t          L2_mu10_Ecut123;
   Bool_t          L2_mu10_Ecut13;
   Bool_t          L2_mu10_IDTrkNoCut;
   Bool_t          L2_mu10_MG;
   Bool_t          L2_mu10_MSonly;
   Bool_t          L2_mu10_MSonly_Ecut12;
   Bool_t          L2_mu10_MSonly_Ecut123;
   Bool_t          L2_mu10_MSonly_Ecut13;
   Bool_t          L2_mu10_MSonly_tight;
   Bool_t          L2_mu10_NoAlg;
   Bool_t          L2_mu10_SiTrk;
   Bool_t          L2_mu10_j30;
   Bool_t          L2_mu10_tight;
   Bool_t          L2_mu10i_loose;
   Bool_t          L2_mu13;
   Bool_t          L2_mu13_MG;
   Bool_t          L2_mu13_MG_tight;
   Bool_t          L2_mu13_tight;
   Bool_t          L2_mu15;
   Bool_t          L2_mu15_MG;
   Bool_t          L2_mu15_NoAlg;
   Bool_t          L2_mu20;
   Bool_t          L2_mu20_MSonly;
   Bool_t          L2_mu20_NoAlg;
   Bool_t          L2_mu20_passHLT;
   Bool_t          L2_mu20_slow;
   Bool_t          L2_mu30_MSonly;
   Bool_t          L2_mu4;
   Bool_t          L2_mu40_MSonly;
   Bool_t          L2_mu4_Bmumu;
   Bool_t          L2_mu4_BmumuX;
   Bool_t          L2_mu4_DiMu;
   Bool_t          L2_mu4_DiMu_FS;
   Bool_t          L2_mu4_DiMu_FS_noOS;
   Bool_t          L2_mu4_DiMu_MG;
   Bool_t          L2_mu4_DiMu_MG_FS;
   Bool_t          L2_mu4_DiMu_SiTrk;
   Bool_t          L2_mu4_DiMu_SiTrk_FS;
   Bool_t          L2_mu4_DiMu_noOS;
   Bool_t          L2_mu4_IDTrkNoCut;
   Bool_t          L2_mu4_Jpsie5e3;
   Bool_t          L2_mu4_Jpsie5e3_FS;
   Bool_t          L2_mu4_Jpsie5e3_SiTrk;
   Bool_t          L2_mu4_Jpsie5e3_SiTrk_FS;
   Bool_t          L2_mu4_Jpsimumu;
   Bool_t          L2_mu4_Jpsimumu_FS;
   Bool_t          L2_mu4_Jpsimumu_SiTrk_FS;
   Bool_t          L2_mu4_L1J10_matched;
   Bool_t          L2_mu4_L1J15_matched;
   Bool_t          L2_mu4_L1J30_matched;
   Bool_t          L2_mu4_L1J55_matched;
   Bool_t          L2_mu4_L1J5_matched;
   Bool_t          L2_mu4_L2MSonly_EFFS_passL2;
   Bool_t          L2_mu4_MG;
   Bool_t          L2_mu4_MSonly;
   Bool_t          L2_mu4_MSonly_EFFS_passL2;
   Bool_t          L2_mu4_MSonly_MB2_noL2_EFFS;
   Bool_t          L2_mu4_MSonly_cosmic;
   Bool_t          L2_mu4_MSonly_outOfTime;
   Bool_t          L2_mu4_MV;
   Bool_t          L2_mu4_SiTrk;
   Bool_t          L2_mu4_Trk_Jpsi;
   Bool_t          L2_mu4_Trk_Jpsi_FS;
   Bool_t          L2_mu4_Trk_Jpsi_loose;
   Bool_t          L2_mu4_Trk_Upsi_FS;
   Bool_t          L2_mu4_Trk_Upsi_loose_FS;
   Bool_t          L2_mu4_Upsimumu_FS;
   Bool_t          L2_mu4_Upsimumu_SiTrk_FS;
   Bool_t          L2_mu4_comm_MSonly_cosmic;
   Bool_t          L2_mu4_comm_cosmic;
   Bool_t          L2_mu4_comm_firstempty;
   Bool_t          L2_mu4_comm_unpaired_iso;
   Bool_t          L2_mu4_cosmic;
   Bool_t          L2_mu4_firstempty;
   Bool_t          L2_mu4_j20;
   Bool_t          L2_mu4_j20_jetNoEF;
   Bool_t          L2_mu4_j25;
   Bool_t          L2_mu4_mu6;
   Bool_t          L2_mu4_muCombTag;
   Bool_t          L2_mu4_tile;
   Bool_t          L2_mu4_tile_cosmic;
   Bool_t          L2_mu4_unpaired_iso;
   Bool_t          L2_mu4mu6_Bmumu;
   Bool_t          L2_mu4mu6_BmumuX;
   Bool_t          L2_mu4mu6_DiMu;
   Bool_t          L2_mu4mu6_Jpsimumu;
   Bool_t          L2_mu4mu6_Jpsimumu_IDTrkNoCut;
   Bool_t          L2_mu4mu6_Upsimumu;
   Bool_t          L2_mu6;
   Bool_t          L2_mu6_Bmumu;
   Bool_t          L2_mu6_BmumuX;
   Bool_t          L2_mu6_DiMu;
   Bool_t          L2_mu6_Ecut12;
   Bool_t          L2_mu6_Ecut123;
   Bool_t          L2_mu6_Ecut13;
   Bool_t          L2_mu6_Ecut2;
   Bool_t          L2_mu6_Ecut3;
   Bool_t          L2_mu6_IDTrkNoCut;
   Bool_t          L2_mu6_Jpsie5e3;
   Bool_t          L2_mu6_Jpsie5e3_FS;
   Bool_t          L2_mu6_Jpsie5e3_SiTrk;
   Bool_t          L2_mu6_Jpsie5e3_SiTrk_FS;
   Bool_t          L2_mu6_Jpsimumu;
   Bool_t          L2_mu6_MG;
   Bool_t          L2_mu6_MSonly;
   Bool_t          L2_mu6_MSonly_Ecut12;
   Bool_t          L2_mu6_MSonly_Ecut123;
   Bool_t          L2_mu6_MSonly_Ecut13;
   Bool_t          L2_mu6_MSonly_Ecut2;
   Bool_t          L2_mu6_MSonly_Ecut3;
   Bool_t          L2_mu6_MSonly_outOfTime;
   Bool_t          L2_mu6_NoAlg;
   Bool_t          L2_mu6_SiTrk;
   Bool_t          L2_mu6_Trk_Jpsi;
   Bool_t          L2_mu6_Upsimumu_FS;
   Bool_t          L2_mu6_muCombTag;
   Bool_t          L2_tau12_loose_IdScan_xe15_noMu;
   Bool_t          L2_tau12_loose_xe15_noMu;
   Bool_t          L2_tau12_loose_xe20_noMu;
   Bool_t          L2_tau16_loose_xe20_noMu;
   Bool_t          L2_tau16_loose_xe25_noMu;
   Bool_t          L2_tau16_loose_xe25_tight_noMu;
   Bool_t          L2_tau16_loose_xe30_noMu;
   Bool_t          L2_tau16_medium_xe22_noMu;
   Bool_t          L2_tau16_medium_xe25_noMu;
   Bool_t          L2_tau16_medium_xe25_tight_noMu;
   Bool_t          L2_tau20_loose_xe25_noMu;
   Bool_t          L2_tauNoCut_hasTrk6_IdScan_xe20_noMu;
   Bool_t          L2_tauNoCut_hasTrk6_xe20_noMu;
   Bool_t          L2_tauNoCut_hasTrk9_xe20_noMu;
   Bool_t          L2_tauNoCut_hasTrk_xe20_noMu;
   Bool_t          L2_xe12;
   Bool_t          L2_xe12_loose;
   Bool_t          L2_xe12_loose_noMu;
   Bool_t          L2_xe12_medium;
   Bool_t          L2_xe12_medium_noMu;
   Bool_t          L2_xe12_noMu;
   Bool_t          L2_xe15;
   Bool_t          L2_xe15_medium;
   Bool_t          L2_xe15_medium_noMu;
   Bool_t          L2_xe15_medium_vfj_noMu;
   Bool_t          L2_xe15_noMu;
   Bool_t          L2_xe15_tight_noMu;
   Bool_t          L2_xe15_tight_vfj_noMu;
   Bool_t          L2_xe15_unbiased_noMu;
   Bool_t          L2_xe15_vfj_noMu;
   Bool_t          L2_xe17_tight_noMu;
   Bool_t          L2_xe17_tight_vfj_noMu;
   Bool_t          L2_xe20;
   Bool_t          L2_xe20_noMu;
   Bool_t          L2_xe20_vfj_noMu;
   Bool_t          L2_xe22_loose_noMu;
   Bool_t          L2_xe22_tight_noMu;
   Bool_t          L2_xe22_tight_vfj_noMu;
   Bool_t          L2_xe25;
   Bool_t          L2_xe25_noMu;
   Bool_t          L2_xe27_tight_noMu;
   Bool_t          L2_xe30;
   Bool_t          L2_xe30_allL1;
   Bool_t          L2_xe30_allL1_FEB;
   Bool_t          L2_xe30_allL1_allCells;
   Bool_t          L2_xe30_allL1_noMu;
   Bool_t          L2_xe30_noMu;
   Bool_t          L2_xe32_tight_noMu;
   Bool_t          L2_xe35;
   Bool_t          L2_xe35_noMu;
   Bool_t          L2_xe40_medium;
   Bool_t          L2_xe40_medium_noMu;
   Bool_t          L2_xe45;
   Bool_t          L2_xe45_noMu;
   Bool_t          L2_xe60_medium;
   Bool_t          L2_xe60_medium_noMu;
   Int_t           mu_muid_n;
   vector<float>   *mu_muid_E;
   vector<float>   *mu_muid_pt;
   vector<float>   *mu_muid_m;
   vector<float>   *mu_muid_eta;
   vector<float>   *mu_muid_phi;
   vector<float>   *mu_muid_px;
   vector<float>   *mu_muid_py;
   vector<float>   *mu_muid_pz;
   vector<float>   *mu_muid_charge;
   vector<unsigned short> *mu_muid_allauthor;
   vector<int>     *mu_muid_author;
   vector<float>   *mu_muid_beta;
   vector<float>   *mu_muid_isMuonLikelihood;
   vector<float>   *mu_muid_matchchi2;
   vector<int>     *mu_muid_matchndof;
   vector<float>   *mu_muid_etcone20;
   vector<float>   *mu_muid_etcone30;
   vector<float>   *mu_muid_etcone40;
   vector<float>   *mu_muid_nucone20;
   vector<float>   *mu_muid_nucone30;
   vector<float>   *mu_muid_nucone40;
   vector<float>   *mu_muid_ptcone20;
   vector<float>   *mu_muid_ptcone30;
   vector<float>   *mu_muid_ptcone40;
   vector<float>   *mu_muid_energyLossPar;
   vector<float>   *mu_muid_energyLossErr;
   vector<float>   *mu_muid_etCore;
   vector<float>   *mu_muid_energyLossType;
   vector<unsigned short> *mu_muid_caloMuonIdTag;
   vector<double>  *mu_muid_caloLRLikelihood;
   vector<int>     *mu_muid_bestMatch;
   vector<int>     *mu_muid_isStandAloneMuon;
   vector<int>     *mu_muid_isCombinedMuon;
   vector<int>     *mu_muid_isLowPtReconstructedMuon;
   vector<int>     *mu_muid_loose;
   vector<int>     *mu_muid_medium;
   vector<int>     *mu_muid_tight;
   vector<float>   *mu_muid_d0_exPV;
   vector<float>   *mu_muid_z0_exPV;
   vector<float>   *mu_muid_phi_exPV;
   vector<float>   *mu_muid_theta_exPV;
   vector<float>   *mu_muid_qoverp_exPV;
   vector<float>   *mu_muid_cb_d0_exPV;
   vector<float>   *mu_muid_cb_z0_exPV;
   vector<float>   *mu_muid_cb_phi_exPV;
   vector<float>   *mu_muid_cb_theta_exPV;
   vector<float>   *mu_muid_cb_qoverp_exPV;
   vector<float>   *mu_muid_id_d0_exPV;
   vector<float>   *mu_muid_id_z0_exPV;
   vector<float>   *mu_muid_id_phi_exPV;
   vector<float>   *mu_muid_id_theta_exPV;
   vector<float>   *mu_muid_id_qoverp_exPV;
   vector<float>   *mu_muid_me_d0_exPV;
   vector<float>   *mu_muid_me_z0_exPV;
   vector<float>   *mu_muid_me_phi_exPV;
   vector<float>   *mu_muid_me_theta_exPV;
   vector<float>   *mu_muid_me_qoverp_exPV;
   vector<float>   *mu_muid_ie_d0_exPV;
   vector<float>   *mu_muid_ie_z0_exPV;
   vector<float>   *mu_muid_ie_phi_exPV;
   vector<float>   *mu_muid_ie_theta_exPV;
   vector<float>   *mu_muid_ie_qoverp_exPV;
   vector<vector<int> > *mu_muid_SpaceTime_detID;
   vector<vector<float> > *mu_muid_SpaceTime_t;
   vector<vector<float> > *mu_muid_SpaceTime_tError;
   vector<vector<float> > *mu_muid_SpaceTime_weight;
   vector<vector<float> > *mu_muid_SpaceTime_x;
   vector<vector<float> > *mu_muid_SpaceTime_y;
   vector<vector<float> > *mu_muid_SpaceTime_z;
   vector<vector<float> > *mu_muid_SpaceTime_t_Tile;
   vector<vector<float> > *mu_muid_SpaceTime_tError_Tile;
   vector<vector<float> > *mu_muid_SpaceTime_weight_Tile;
   vector<vector<float> > *mu_muid_SpaceTime_x_Tile;
   vector<vector<float> > *mu_muid_SpaceTime_y_Tile;
   vector<vector<float> > *mu_muid_SpaceTime_z_Tile;
   vector<vector<float> > *mu_muid_SpaceTime_t_TRT;
   vector<vector<float> > *mu_muid_SpaceTime_tError_TRT;
   vector<vector<float> > *mu_muid_SpaceTime_weight_TRT;
   vector<vector<float> > *mu_muid_SpaceTime_x_TRT;
   vector<vector<float> > *mu_muid_SpaceTime_y_TRT;
   vector<vector<float> > *mu_muid_SpaceTime_z_TRT;
   vector<vector<float> > *mu_muid_SpaceTime_t_MDT;
   vector<vector<float> > *mu_muid_SpaceTime_tError_MDT;
   vector<vector<float> > *mu_muid_SpaceTime_weight_MDT;
   vector<vector<float> > *mu_muid_SpaceTime_x_MDT;
   vector<vector<float> > *mu_muid_SpaceTime_y_MDT;
   vector<vector<float> > *mu_muid_SpaceTime_z_MDT;
   vector<float>   *mu_muid_TileCellEnergyLayer1;
   vector<float>   *mu_muid_TileTimeLayer1;
   vector<float>   *mu_muid_TileCellRmsNoiseLayer1;
   vector<float>   *mu_muid_TileCellSignLayer1;
   vector<float>   *mu_muid_TileCellEnergyLayer2;
   vector<float>   *mu_muid_TileTimeLayer2;
   vector<float>   *mu_muid_TileCellRmsNoiseLayer2;
   vector<float>   *mu_muid_TileCellSignLayer2;
   vector<float>   *mu_muid_TileCellEnergyLayer3;
   vector<float>   *mu_muid_TileTimeLayer3;
   vector<float>   *mu_muid_TileCellRmsNoiseLayer3;
   vector<float>   *mu_muid_TileCellSignLayer3;
   vector<float>   *mu_muid_MSTrkT0_1;
   vector<float>   *mu_muid_MSTrkT0_2;
   vector<float>   *mu_muid_MSTrkT0_3;
   vector<float>   *mu_muid_cov_d0_exPV;
   vector<float>   *mu_muid_cov_z0_exPV;
   vector<float>   *mu_muid_cov_phi_exPV;
   vector<float>   *mu_muid_cov_theta_exPV;
   vector<float>   *mu_muid_cov_qoverp_exPV;
   vector<float>   *mu_muid_cov_d0_z0_exPV;
   vector<float>   *mu_muid_cov_d0_phi_exPV;
   vector<float>   *mu_muid_cov_d0_theta_exPV;
   vector<float>   *mu_muid_cov_d0_qoverp_exPV;
   vector<float>   *mu_muid_cov_z0_phi_exPV;
   vector<float>   *mu_muid_cov_z0_theta_exPV;
   vector<float>   *mu_muid_cov_z0_qoverp_exPV;
   vector<float>   *mu_muid_cov_phi_theta_exPV;
   vector<float>   *mu_muid_cov_phi_qoverp_exPV;
   vector<float>   *mu_muid_cov_theta_qoverp_exPV;
   vector<float>   *mu_muid_id_cov_d0_exPV;
   vector<float>   *mu_muid_id_cov_z0_exPV;
   vector<float>   *mu_muid_id_cov_phi_exPV;
   vector<float>   *mu_muid_id_cov_theta_exPV;
   vector<float>   *mu_muid_id_cov_qoverp_exPV;
   vector<float>   *mu_muid_id_cov_d0_z0_exPV;
   vector<float>   *mu_muid_id_cov_d0_phi_exPV;
   vector<float>   *mu_muid_id_cov_d0_theta_exPV;
   vector<float>   *mu_muid_id_cov_d0_qoverp_exPV;
   vector<float>   *mu_muid_id_cov_z0_phi_exPV;
   vector<float>   *mu_muid_id_cov_z0_theta_exPV;
   vector<float>   *mu_muid_id_cov_z0_qoverp_exPV;
   vector<float>   *mu_muid_id_cov_phi_theta_exPV;
   vector<float>   *mu_muid_id_cov_phi_qoverp_exPV;
   vector<float>   *mu_muid_id_cov_theta_qoverp_exPV;
   vector<float>   *mu_muid_me_cov_d0_exPV;
   vector<float>   *mu_muid_me_cov_z0_exPV;
   vector<float>   *mu_muid_me_cov_phi_exPV;
   vector<float>   *mu_muid_me_cov_theta_exPV;
   vector<float>   *mu_muid_me_cov_qoverp_exPV;
   vector<float>   *mu_muid_me_cov_d0_z0_exPV;
   vector<float>   *mu_muid_me_cov_d0_phi_exPV;
   vector<float>   *mu_muid_me_cov_d0_theta_exPV;
   vector<float>   *mu_muid_me_cov_d0_qoverp_exPV;
   vector<float>   *mu_muid_me_cov_z0_phi_exPV;
   vector<float>   *mu_muid_me_cov_z0_theta_exPV;
   vector<float>   *mu_muid_me_cov_z0_qoverp_exPV;
   vector<float>   *mu_muid_me_cov_phi_theta_exPV;
   vector<float>   *mu_muid_me_cov_phi_qoverp_exPV;
   vector<float>   *mu_muid_me_cov_theta_qoverp_exPV;
   vector<float>   *mu_muid_ms_d0;
   vector<float>   *mu_muid_ms_z0;
   vector<float>   *mu_muid_ms_phi;
   vector<float>   *mu_muid_ms_theta;
   vector<float>   *mu_muid_ms_qoverp;
   vector<float>   *mu_muid_id_d0;
   vector<float>   *mu_muid_id_z0;
   vector<float>   *mu_muid_id_phi;
   vector<float>   *mu_muid_id_theta;
   vector<float>   *mu_muid_id_qoverp;
   vector<float>   *mu_muid_me_d0;
   vector<float>   *mu_muid_me_z0;
   vector<float>   *mu_muid_me_phi;
   vector<float>   *mu_muid_me_theta;
   vector<float>   *mu_muid_me_qoverp;
   vector<float>   *mu_muid_ie_d0;
   vector<float>   *mu_muid_ie_z0;
   vector<float>   *mu_muid_ie_phi;
   vector<float>   *mu_muid_ie_theta;
   vector<float>   *mu_muid_ie_qoverp;
   vector<int>     *mu_muid_nOutliersOnTrack;
   vector<int>     *mu_muid_nBLHits;
   vector<int>     *mu_muid_nPixHits;
   vector<int>     *mu_muid_nSCTHits;
   vector<int>     *mu_muid_nTRTHits;
   vector<int>     *mu_muid_nTRTHighTHits;
   vector<int>     *mu_muid_nBLSharedHits;
   vector<int>     *mu_muid_nPixSharedHits;
   vector<int>     *mu_muid_nPixHoles;
   vector<int>     *mu_muid_nSCTSharedHits;
   vector<int>     *mu_muid_nSCTHoles;
   vector<int>     *mu_muid_nTRTOutliers;
   vector<int>     *mu_muid_nTRTHighTOutliers;
   vector<int>     *mu_muid_nGangedPixels;
   vector<int>     *mu_muid_nPixelDeadSensors;
   vector<int>     *mu_muid_nSCTDeadSensors;
   vector<int>     *mu_muid_nTRTDeadStraws;
   vector<int>     *mu_muid_expectBLayerHit;
   vector<int>     *mu_muid_nMDTHits;
   vector<int>     *mu_muid_nMDTHoles;
   vector<int>     *mu_muid_nCSCEtaHits;
   vector<int>     *mu_muid_nCSCEtaHoles;
   vector<int>     *mu_muid_nCSCPhiHits;
   vector<int>     *mu_muid_nCSCPhiHoles;
   vector<int>     *mu_muid_nRPCEtaHits;
   vector<int>     *mu_muid_nRPCEtaHoles;
   vector<int>     *mu_muid_nRPCPhiHits;
   vector<int>     *mu_muid_nRPCPhiHoles;
   vector<int>     *mu_muid_nTGCEtaHits;
   vector<int>     *mu_muid_nTGCEtaHoles;
   vector<int>     *mu_muid_nTGCPhiHits;
   vector<int>     *mu_muid_nTGCPhiHoles;
   vector<int>     *mu_muid_nMDTBIHits;
   vector<int>     *mu_muid_nMDTBMHits;
   vector<int>     *mu_muid_nMDTBOHits;
   vector<int>     *mu_muid_nMDTBEEHits;
   vector<int>     *mu_muid_nMDTBIS78Hits;
   vector<int>     *mu_muid_nMDTEIHits;
   vector<int>     *mu_muid_nMDTEMHits;
   vector<int>     *mu_muid_nMDTEOHits;
   vector<int>     *mu_muid_nMDTEEHits;
   vector<int>     *mu_muid_nRPCLayer1EtaHits;
   vector<int>     *mu_muid_nRPCLayer2EtaHits;
   vector<int>     *mu_muid_nRPCLayer3EtaHits;
   vector<int>     *mu_muid_nRPCLayer1PhiHits;
   vector<int>     *mu_muid_nRPCLayer2PhiHits;
   vector<int>     *mu_muid_nRPCLayer3PhiHits;
   vector<int>     *mu_muid_nTGCLayer1EtaHits;
   vector<int>     *mu_muid_nTGCLayer2EtaHits;
   vector<int>     *mu_muid_nTGCLayer3EtaHits;
   vector<int>     *mu_muid_nTGCLayer4EtaHits;
   vector<int>     *mu_muid_nTGCLayer1PhiHits;
   vector<int>     *mu_muid_nTGCLayer2PhiHits;
   vector<int>     *mu_muid_nTGCLayer3PhiHits;
   vector<int>     *mu_muid_nTGCLayer4PhiHits;
   vector<int>     *mu_muid_barrelSectors;
   vector<int>     *mu_muid_endcapSectors;
   vector<float>   *mu_muid_trackd0;
   vector<float>   *mu_muid_trackz0;
   vector<float>   *mu_muid_trackphi;
   vector<float>   *mu_muid_tracktheta;
   vector<float>   *mu_muid_trackqoverp;
   vector<float>   *mu_muid_trackcov_d0;
   vector<float>   *mu_muid_trackcov_z0;
   vector<float>   *mu_muid_trackcov_phi;
   vector<float>   *mu_muid_trackcov_theta;
   vector<float>   *mu_muid_trackcov_qoverp;
   vector<float>   *mu_muid_trackcov_d0_z0;
   vector<float>   *mu_muid_trackcov_d0_phi;
   vector<float>   *mu_muid_trackcov_d0_theta;
   vector<float>   *mu_muid_trackcov_d0_qoverp;
   vector<float>   *mu_muid_trackcov_z0_phi;
   vector<float>   *mu_muid_trackcov_z0_theta;
   vector<float>   *mu_muid_trackcov_z0_qoverp;
   vector<float>   *mu_muid_trackcov_phi_theta;
   vector<float>   *mu_muid_trackcov_phi_qoverp;
   vector<float>   *mu_muid_trackcov_theta_qoverp;
   vector<float>   *mu_muid_trackfitchi2;
   vector<int>     *mu_muid_trackfitndof;
   vector<int>     *mu_muid_hastrack;
   vector<float>   *mu_muid_truth_dr;
   vector<float>   *mu_muid_truth_E;
   vector<float>   *mu_muid_truth_pt;
   vector<float>   *mu_muid_truth_eta;
   vector<float>   *mu_muid_truth_phi;
   vector<int>     *mu_muid_truth_type;
   vector<int>     *mu_muid_truth_status;
   vector<int>     *mu_muid_truth_barcode;
   vector<int>     *mu_muid_truth_mothertype;
   vector<int>     *mu_muid_truth_motherbarcode;
   vector<int>     *mu_muid_truth_matched;
   vector<float>   *mu_muid_EFCB_dr;
   vector<int>     *mu_muid_EFCB_index;
   vector<float>   *mu_muid_EFMG_dr;
   vector<int>     *mu_muid_EFMG_index;
   vector<float>   *mu_muid_EFME_dr;
   vector<int>     *mu_muid_EFME_index;
   vector<float>   *mu_muid_L2CB_dr;
   vector<int>     *mu_muid_L2CB_index;
   vector<float>   *mu_muid_L1_dr;
   vector<int>     *mu_muid_L1_index;
   Int_t           mu_staco_n;
   vector<float>   *mu_staco_E;
   vector<float>   *mu_staco_pt;
   vector<float>   *mu_staco_m;
   vector<float>   *mu_staco_eta;
   vector<float>   *mu_staco_phi;
   vector<float>   *mu_staco_px;
   vector<float>   *mu_staco_py;
   vector<float>   *mu_staco_pz;
   vector<float>   *mu_staco_charge;
   vector<unsigned short> *mu_staco_allauthor;
   vector<int>     *mu_staco_author;
   vector<float>   *mu_staco_beta;
   vector<float>   *mu_staco_isMuonLikelihood;
   vector<float>   *mu_staco_matchchi2;
   vector<int>     *mu_staco_matchndof;
   vector<float>   *mu_staco_etcone20;
   vector<float>   *mu_staco_etcone30;
   vector<float>   *mu_staco_etcone40;
   vector<float>   *mu_staco_nucone20;
   vector<float>   *mu_staco_nucone30;
   vector<float>   *mu_staco_nucone40;
   vector<float>   *mu_staco_ptcone20;
   vector<float>   *mu_staco_ptcone30;
   vector<float>   *mu_staco_ptcone40;
   vector<float>   *mu_staco_energyLossPar;
   vector<float>   *mu_staco_energyLossErr;
   vector<float>   *mu_staco_etCore;
   vector<float>   *mu_staco_energyLossType;
   vector<unsigned short> *mu_staco_caloMuonIdTag;
   vector<double>  *mu_staco_caloLRLikelihood;
   vector<int>     *mu_staco_bestMatch;
   vector<int>     *mu_staco_isStandAloneMuon;
   vector<int>     *mu_staco_isCombinedMuon;
   vector<int>     *mu_staco_isLowPtReconstructedMuon;
   vector<int>     *mu_staco_loose;
   vector<int>     *mu_staco_medium;
   vector<int>     *mu_staco_tight;
   vector<float>   *mu_staco_d0_exPV;
   vector<float>   *mu_staco_z0_exPV;
   vector<float>   *mu_staco_phi_exPV;
   vector<float>   *mu_staco_theta_exPV;
   vector<float>   *mu_staco_qoverp_exPV;
   vector<float>   *mu_staco_cb_d0_exPV;
   vector<float>   *mu_staco_cb_z0_exPV;
   vector<float>   *mu_staco_cb_phi_exPV;
   vector<float>   *mu_staco_cb_theta_exPV;
   vector<float>   *mu_staco_cb_qoverp_exPV;
   vector<float>   *mu_staco_id_d0_exPV;
   vector<float>   *mu_staco_id_z0_exPV;
   vector<float>   *mu_staco_id_phi_exPV;
   vector<float>   *mu_staco_id_theta_exPV;
   vector<float>   *mu_staco_id_qoverp_exPV;
   vector<float>   *mu_staco_me_d0_exPV;
   vector<float>   *mu_staco_me_z0_exPV;
   vector<float>   *mu_staco_me_phi_exPV;
   vector<float>   *mu_staco_me_theta_exPV;
   vector<float>   *mu_staco_me_qoverp_exPV;
   vector<float>   *mu_staco_ie_d0_exPV;
   vector<float>   *mu_staco_ie_z0_exPV;
   vector<float>   *mu_staco_ie_phi_exPV;
   vector<float>   *mu_staco_ie_theta_exPV;
   vector<float>   *mu_staco_ie_qoverp_exPV;
   vector<vector<int> > *mu_staco_SpaceTime_detID;
   vector<vector<float> > *mu_staco_SpaceTime_t;
   vector<vector<float> > *mu_staco_SpaceTime_tError;
   vector<vector<float> > *mu_staco_SpaceTime_weight;
   vector<vector<float> > *mu_staco_SpaceTime_x;
   vector<vector<float> > *mu_staco_SpaceTime_y;
   vector<vector<float> > *mu_staco_SpaceTime_z;
   vector<vector<float> > *mu_staco_SpaceTime_t_Tile;
   vector<vector<float> > *mu_staco_SpaceTime_tError_Tile;
   vector<vector<float> > *mu_staco_SpaceTime_weight_Tile;
   vector<vector<float> > *mu_staco_SpaceTime_x_Tile;
   vector<vector<float> > *mu_staco_SpaceTime_y_Tile;
   vector<vector<float> > *mu_staco_SpaceTime_z_Tile;
   vector<vector<float> > *mu_staco_SpaceTime_t_TRT;
   vector<vector<float> > *mu_staco_SpaceTime_tError_TRT;
   vector<vector<float> > *mu_staco_SpaceTime_weight_TRT;
   vector<vector<float> > *mu_staco_SpaceTime_x_TRT;
   vector<vector<float> > *mu_staco_SpaceTime_y_TRT;
   vector<vector<float> > *mu_staco_SpaceTime_z_TRT;
   vector<vector<float> > *mu_staco_SpaceTime_t_MDT;
   vector<vector<float> > *mu_staco_SpaceTime_tError_MDT;
   vector<vector<float> > *mu_staco_SpaceTime_weight_MDT;
   vector<vector<float> > *mu_staco_SpaceTime_x_MDT;
   vector<vector<float> > *mu_staco_SpaceTime_y_MDT;
   vector<vector<float> > *mu_staco_SpaceTime_z_MDT;
   vector<float>   *mu_staco_TileCellEnergyLayer1;
   vector<float>   *mu_staco_TileTimeLayer1;
   vector<float>   *mu_staco_TileCellRmsNoiseLayer1;
   vector<float>   *mu_staco_TileCellSignLayer1;
   vector<float>   *mu_staco_TileCellEnergyLayer2;
   vector<float>   *mu_staco_TileTimeLayer2;
   vector<float>   *mu_staco_TileCellRmsNoiseLayer2;
   vector<float>   *mu_staco_TileCellSignLayer2;
   vector<float>   *mu_staco_TileCellEnergyLayer3;
   vector<float>   *mu_staco_TileTimeLayer3;
   vector<float>   *mu_staco_TileCellRmsNoiseLayer3;
   vector<float>   *mu_staco_TileCellSignLayer3;
   vector<float>   *mu_staco_MSTrkT0_1;
   vector<float>   *mu_staco_MSTrkT0_2;
   vector<float>   *mu_staco_MSTrkT0_3;
   vector<float>   *mu_staco_cov_d0_exPV;
   vector<float>   *mu_staco_cov_z0_exPV;
   vector<float>   *mu_staco_cov_phi_exPV;
   vector<float>   *mu_staco_cov_theta_exPV;
   vector<float>   *mu_staco_cov_qoverp_exPV;
   vector<float>   *mu_staco_cov_d0_z0_exPV;
   vector<float>   *mu_staco_cov_d0_phi_exPV;
   vector<float>   *mu_staco_cov_d0_theta_exPV;
   vector<float>   *mu_staco_cov_d0_qoverp_exPV;
   vector<float>   *mu_staco_cov_z0_phi_exPV;
   vector<float>   *mu_staco_cov_z0_theta_exPV;
   vector<float>   *mu_staco_cov_z0_qoverp_exPV;
   vector<float>   *mu_staco_cov_phi_theta_exPV;
   vector<float>   *mu_staco_cov_phi_qoverp_exPV;
   vector<float>   *mu_staco_cov_theta_qoverp_exPV;
   vector<float>   *mu_staco_id_cov_d0_exPV;
   vector<float>   *mu_staco_id_cov_z0_exPV;
   vector<float>   *mu_staco_id_cov_phi_exPV;
   vector<float>   *mu_staco_id_cov_theta_exPV;
   vector<float>   *mu_staco_id_cov_qoverp_exPV;
   vector<float>   *mu_staco_id_cov_d0_z0_exPV;
   vector<float>   *mu_staco_id_cov_d0_phi_exPV;
   vector<float>   *mu_staco_id_cov_d0_theta_exPV;
   vector<float>   *mu_staco_id_cov_d0_qoverp_exPV;
   vector<float>   *mu_staco_id_cov_z0_phi_exPV;
   vector<float>   *mu_staco_id_cov_z0_theta_exPV;
   vector<float>   *mu_staco_id_cov_z0_qoverp_exPV;
   vector<float>   *mu_staco_id_cov_phi_theta_exPV;
   vector<float>   *mu_staco_id_cov_phi_qoverp_exPV;
   vector<float>   *mu_staco_id_cov_theta_qoverp_exPV;
   vector<float>   *mu_staco_me_cov_d0_exPV;
   vector<float>   *mu_staco_me_cov_z0_exPV;
   vector<float>   *mu_staco_me_cov_phi_exPV;
   vector<float>   *mu_staco_me_cov_theta_exPV;
   vector<float>   *mu_staco_me_cov_qoverp_exPV;
   vector<float>   *mu_staco_me_cov_d0_z0_exPV;
   vector<float>   *mu_staco_me_cov_d0_phi_exPV;
   vector<float>   *mu_staco_me_cov_d0_theta_exPV;
   vector<float>   *mu_staco_me_cov_d0_qoverp_exPV;
   vector<float>   *mu_staco_me_cov_z0_phi_exPV;
   vector<float>   *mu_staco_me_cov_z0_theta_exPV;
   vector<float>   *mu_staco_me_cov_z0_qoverp_exPV;
   vector<float>   *mu_staco_me_cov_phi_theta_exPV;
   vector<float>   *mu_staco_me_cov_phi_qoverp_exPV;
   vector<float>   *mu_staco_me_cov_theta_qoverp_exPV;
   vector<float>   *mu_staco_ms_d0;
   vector<float>   *mu_staco_ms_z0;
   vector<float>   *mu_staco_ms_phi;
   vector<float>   *mu_staco_ms_theta;
   vector<float>   *mu_staco_ms_qoverp;
   vector<float>   *mu_staco_id_d0;
   vector<float>   *mu_staco_id_z0;
   vector<float>   *mu_staco_id_phi;
   vector<float>   *mu_staco_id_theta;
   vector<float>   *mu_staco_id_qoverp;
   vector<float>   *mu_staco_me_d0;
   vector<float>   *mu_staco_me_z0;
   vector<float>   *mu_staco_me_phi;
   vector<float>   *mu_staco_me_theta;
   vector<float>   *mu_staco_me_qoverp;
   vector<float>   *mu_staco_ie_d0;
   vector<float>   *mu_staco_ie_z0;
   vector<float>   *mu_staco_ie_phi;
   vector<float>   *mu_staco_ie_theta;
   vector<float>   *mu_staco_ie_qoverp;
   vector<int>     *mu_staco_nOutliersOnTrack;
   vector<int>     *mu_staco_nBLHits;
   vector<int>     *mu_staco_nPixHits;
   vector<int>     *mu_staco_nSCTHits;
   vector<int>     *mu_staco_nTRTHits;
   vector<int>     *mu_staco_nTRTHighTHits;
   vector<int>     *mu_staco_nBLSharedHits;
   vector<int>     *mu_staco_nPixSharedHits;
   vector<int>     *mu_staco_nPixHoles;
   vector<int>     *mu_staco_nSCTSharedHits;
   vector<int>     *mu_staco_nSCTHoles;
   vector<int>     *mu_staco_nTRTOutliers;
   vector<int>     *mu_staco_nTRTHighTOutliers;
   vector<int>     *mu_staco_nGangedPixels;
   vector<int>     *mu_staco_nPixelDeadSensors;
   vector<int>     *mu_staco_nSCTDeadSensors;
   vector<int>     *mu_staco_nTRTDeadStraws;
   vector<int>     *mu_staco_expectBLayerHit;
   vector<int>     *mu_staco_nMDTHits;
   vector<int>     *mu_staco_nMDTHoles;
   vector<int>     *mu_staco_nCSCEtaHits;
   vector<int>     *mu_staco_nCSCEtaHoles;
   vector<int>     *mu_staco_nCSCPhiHits;
   vector<int>     *mu_staco_nCSCPhiHoles;
   vector<int>     *mu_staco_nRPCEtaHits;
   vector<int>     *mu_staco_nRPCEtaHoles;
   vector<int>     *mu_staco_nRPCPhiHits;
   vector<int>     *mu_staco_nRPCPhiHoles;
   vector<int>     *mu_staco_nTGCEtaHits;
   vector<int>     *mu_staco_nTGCEtaHoles;
   vector<int>     *mu_staco_nTGCPhiHits;
   vector<int>     *mu_staco_nTGCPhiHoles;
   vector<int>     *mu_staco_nMDTBIHits;
   vector<int>     *mu_staco_nMDTBMHits;
   vector<int>     *mu_staco_nMDTBOHits;
   vector<int>     *mu_staco_nMDTBEEHits;
   vector<int>     *mu_staco_nMDTBIS78Hits;
   vector<int>     *mu_staco_nMDTEIHits;
   vector<int>     *mu_staco_nMDTEMHits;
   vector<int>     *mu_staco_nMDTEOHits;
   vector<int>     *mu_staco_nMDTEEHits;
   vector<int>     *mu_staco_nRPCLayer1EtaHits;
   vector<int>     *mu_staco_nRPCLayer2EtaHits;
   vector<int>     *mu_staco_nRPCLayer3EtaHits;
   vector<int>     *mu_staco_nRPCLayer1PhiHits;
   vector<int>     *mu_staco_nRPCLayer2PhiHits;
   vector<int>     *mu_staco_nRPCLayer3PhiHits;
   vector<int>     *mu_staco_nTGCLayer1EtaHits;
   vector<int>     *mu_staco_nTGCLayer2EtaHits;
   vector<int>     *mu_staco_nTGCLayer3EtaHits;
   vector<int>     *mu_staco_nTGCLayer4EtaHits;
   vector<int>     *mu_staco_nTGCLayer1PhiHits;
   vector<int>     *mu_staco_nTGCLayer2PhiHits;
   vector<int>     *mu_staco_nTGCLayer3PhiHits;
   vector<int>     *mu_staco_nTGCLayer4PhiHits;
   vector<int>     *mu_staco_barrelSectors;
   vector<int>     *mu_staco_endcapSectors;
   vector<float>   *mu_staco_trackd0;
   vector<float>   *mu_staco_trackz0;
   vector<float>   *mu_staco_trackphi;
   vector<float>   *mu_staco_tracktheta;
   vector<float>   *mu_staco_trackqoverp;
   vector<float>   *mu_staco_trackcov_d0;
   vector<float>   *mu_staco_trackcov_z0;
   vector<float>   *mu_staco_trackcov_phi;
   vector<float>   *mu_staco_trackcov_theta;
   vector<float>   *mu_staco_trackcov_qoverp;
   vector<float>   *mu_staco_trackcov_d0_z0;
   vector<float>   *mu_staco_trackcov_d0_phi;
   vector<float>   *mu_staco_trackcov_d0_theta;
   vector<float>   *mu_staco_trackcov_d0_qoverp;
   vector<float>   *mu_staco_trackcov_z0_phi;
   vector<float>   *mu_staco_trackcov_z0_theta;
   vector<float>   *mu_staco_trackcov_z0_qoverp;
   vector<float>   *mu_staco_trackcov_phi_theta;
   vector<float>   *mu_staco_trackcov_phi_qoverp;
   vector<float>   *mu_staco_trackcov_theta_qoverp;
   vector<float>   *mu_staco_trackfitchi2;
   vector<int>     *mu_staco_trackfitndof;
   vector<int>     *mu_staco_hastrack;
   vector<float>   *mu_staco_truth_dr;
   vector<float>   *mu_staco_truth_E;
   vector<float>   *mu_staco_truth_pt;
   vector<float>   *mu_staco_truth_eta;
   vector<float>   *mu_staco_truth_phi;
   vector<int>     *mu_staco_truth_type;
   vector<int>     *mu_staco_truth_status;
   vector<int>     *mu_staco_truth_barcode;
   vector<int>     *mu_staco_truth_mothertype;
   vector<int>     *mu_staco_truth_motherbarcode;
   vector<int>     *mu_staco_truth_matched;
   vector<float>   *mu_staco_EFCB_dr;
   vector<int>     *mu_staco_EFCB_index;
   vector<float>   *mu_staco_EFMG_dr;
   vector<int>     *mu_staco_EFMG_index;
   vector<float>   *mu_staco_EFME_dr;
   vector<int>     *mu_staco_EFME_index;
   vector<float>   *mu_staco_L2CB_dr;
   vector<int>     *mu_staco_L2CB_index;
   vector<float>   *mu_staco_L1_dr;
   vector<int>     *mu_staco_L1_index;
   Int_t           mu_calo_n;
   vector<float>   *mu_calo_E;
   vector<float>   *mu_calo_pt;
   vector<float>   *mu_calo_m;
   vector<float>   *mu_calo_eta;
   vector<float>   *mu_calo_phi;
   vector<float>   *mu_calo_px;
   vector<float>   *mu_calo_py;
   vector<float>   *mu_calo_pz;
   vector<float>   *mu_calo_charge;
   vector<unsigned short> *mu_calo_allauthor;
   vector<int>     *mu_calo_hastrack;
   vector<float>   *mu_calo_truth_dr;
   vector<float>   *mu_calo_truth_E;
   vector<float>   *mu_calo_truth_pt;
   vector<float>   *mu_calo_truth_eta;
   vector<float>   *mu_calo_truth_phi;
   vector<int>     *mu_calo_truth_type;
   vector<int>     *mu_calo_truth_status;
   vector<int>     *mu_calo_truth_barcode;
   vector<int>     *mu_calo_truth_mothertype;
   vector<int>     *mu_calo_truth_motherbarcode;
   vector<int>     *mu_calo_truth_matched;
   Int_t           jet_akt4topoem_n;
   vector<float>   *jet_akt4topoem_E;
   vector<float>   *jet_akt4topoem_pt;
   vector<float>   *jet_akt4topoem_m;
   vector<float>   *jet_akt4topoem_eta;
   vector<float>   *jet_akt4topoem_phi;
   vector<float>   *jet_akt4topoem_EtaOrigin;
   vector<float>   *jet_akt4topoem_PhiOrigin;
   vector<float>   *jet_akt4topoem_MOrigin;
   vector<float>   *jet_akt4topoem_EtaOriginEM;
   vector<float>   *jet_akt4topoem_PhiOriginEM;
   vector<float>   *jet_akt4topoem_MOriginEM;
   vector<float>   *jet_akt4topoem_WIDTH;
   vector<float>   *jet_akt4topoem_n90;
   vector<float>   *jet_akt4topoem_Timing;
   vector<float>   *jet_akt4topoem_LArQuality;
   vector<float>   *jet_akt4topoem_nTrk;
   vector<float>   *jet_akt4topoem_sumPtTrk;
   vector<float>   *jet_akt4topoem_OriginIndex;
   vector<float>   *jet_akt4topoem_HECQuality;
   vector<float>   *jet_akt4topoem_NegativeE;
   vector<float>   *jet_akt4topoem_YFlip12;
   vector<float>   *jet_akt4topoem_YFlip23;
   vector<float>   *jet_akt4topoem_BCH_CORR_CELL;
   vector<float>   *jet_akt4topoem_BCH_CORR_DOTX;
   vector<float>   *jet_akt4topoem_BCH_CORR_JET;
   vector<float>   *jet_akt4topoem_BCH_CORR_JET_FORCELL;
   vector<float>   *jet_akt4topoem_ENG_BAD_CELLS;
   vector<float>   *jet_akt4topoem_N_BAD_CELLS;
   vector<float>   *jet_akt4topoem_N_BAD_CELLS_CORR;
   vector<float>   *jet_akt4topoem_BAD_CELLS_CORR_E;
   vector<float>   *jet_akt4topoem_NumTowers;
   vector<int>     *jet_akt4topoem_SamplingMax;
   vector<float>   *jet_akt4topoem_fracSamplingMax;
   vector<float>   *jet_akt4topoem_hecf;
   vector<float>   *jet_akt4topoem_tgap3f;
   vector<int>     *jet_akt4topoem_isUgly;
   vector<int>     *jet_akt4topoem_isBadLoose;
   vector<int>     *jet_akt4topoem_isBadMedium;
   vector<int>     *jet_akt4topoem_isBadTight;
   vector<float>   *jet_akt4topoem_emfrac;
   vector<float>   *jet_akt4topoem_Offset;
   vector<float>   *jet_akt4topoem_EMJES;
   vector<float>   *jet_akt4topoem_EMJES_EtaCorr;
   vector<float>   *jet_akt4topoem_EMJESnooffset;
   vector<float>   *jet_akt4topoem_GCWJES;
   vector<float>   *jet_akt4topoem_GCWJES_EtaCorr;
   vector<float>   *jet_akt4topoem_CB;
   vector<float>   *jet_akt4topoem_emscale_E;
   vector<float>   *jet_akt4topoem_emscale_pt;
   vector<float>   *jet_akt4topoem_emscale_m;
   vector<float>   *jet_akt4topoem_emscale_eta;
   vector<float>   *jet_akt4topoem_emscale_phi;
   vector<float>   *jet_akt4topoem_jvtxf;
   vector<float>   *jet_akt4topoem_jvtx_x;
   vector<float>   *jet_akt4topoem_jvtx_y;
   vector<float>   *jet_akt4topoem_jvtx_z;
   vector<float>   *jet_akt4topoem_GSCFactorF;
   vector<float>   *jet_akt4topoem_WidthFraction;
   vector<double>  *jet_akt4topoem_flavor_component_ip2d_pu;
   vector<double>  *jet_akt4topoem_flavor_component_ip2d_pb;
   vector<double>  *jet_akt4topoem_flavor_component_ip2d_ntrk;
   vector<double>  *jet_akt4topoem_flavor_component_ip3d_pu;
   vector<double>  *jet_akt4topoem_flavor_component_ip3d_pb;
   vector<double>  *jet_akt4topoem_flavor_component_ip3d_ntrk;
   vector<double>  *jet_akt4topoem_flavor_component_sv1_pu;
   vector<double>  *jet_akt4topoem_flavor_component_sv1_pb;
   vector<double>  *jet_akt4topoem_flavor_component_sv2_pu;
   vector<double>  *jet_akt4topoem_flavor_component_sv2_pb;
   vector<double>  *jet_akt4topoem_flavor_component_svp_mass;
   vector<double>  *jet_akt4topoem_flavor_component_svp_n2t;
   vector<double>  *jet_akt4topoem_flavor_component_svp_svok;
   vector<double>  *jet_akt4topoem_flavor_component_svp_ntrk;
   vector<double>  *jet_akt4topoem_flavor_component_svp_ntrkv;
   vector<double>  *jet_akt4topoem_flavor_component_svp_ntrkj;
   vector<double>  *jet_akt4topoem_flavor_component_svp_efrc;
   vector<double>  *jet_akt4topoem_flavor_component_sv0p_mass;
   vector<double>  *jet_akt4topoem_flavor_component_sv0p_n2t;
   vector<double>  *jet_akt4topoem_flavor_component_sv0p_svok;
   vector<double>  *jet_akt4topoem_flavor_component_sv0p_ntrk;
   vector<double>  *jet_akt4topoem_flavor_component_sv0p_ntrkv;
   vector<double>  *jet_akt4topoem_flavor_component_sv0p_ntrkj;
   vector<double>  *jet_akt4topoem_flavor_component_sv0p_efrc;
   vector<double>  *jet_akt4topoem_flavor_component_jfit_pu;
   vector<double>  *jet_akt4topoem_flavor_component_jfit_pb;
   vector<double>  *jet_akt4topoem_flavor_component_jfit_nvtxnt;
   vector<double>  *jet_akt4topoem_flavor_component_jfit_nvtx1t;
   vector<double>  *jet_akt4topoem_flavor_component_jfit_ntrk;
   vector<double>  *jet_akt4topoem_flavor_component_jfit_efrc;
   vector<double>  *jet_akt4topoem_flavor_component_jfit_mass;
   vector<double>  *jet_akt4topoem_flavor_component_jfit_sig3d;
   vector<double>  *jet_akt4topoem_flavor_weight_TrackCounting2D;
   vector<double>  *jet_akt4topoem_flavor_weight_JetProb;
   vector<double>  *jet_akt4topoem_flavor_weight_IP1D;
   vector<double>  *jet_akt4topoem_flavor_weight_IP2D;
   vector<double>  *jet_akt4topoem_flavor_weight_IP3D;
   vector<double>  *jet_akt4topoem_flavor_weight_SV0;
   vector<double>  *jet_akt4topoem_flavor_weight_SV1;
   vector<double>  *jet_akt4topoem_flavor_weight_SV2;
   vector<double>  *jet_akt4topoem_flavor_weight_JetFitterTag;
   vector<double>  *jet_akt4topoem_flavor_weight_JetFitterCOMB;
   vector<double>  *jet_akt4topoem_flavor_weight_JetFitterTagNN;
   vector<double>  *jet_akt4topoem_flavor_weight_JetFitterCOMBNN;
   vector<double>  *jet_akt4topoem_flavor_weight_SoftMuonTag;
   vector<double>  *jet_akt4topoem_flavor_weight_SoftElectronTag;
   vector<double>  *jet_akt4topoem_flavor_weight_IP3DSV1;
   vector<double>  *jet_akt4topoem_flavor_truth_dRminToB;
   vector<double>  *jet_akt4topoem_flavor_truth_dRminToC;
   vector<double>  *jet_akt4topoem_flavor_truth_dRminToT;
   vector<int>     *jet_akt4topoem_flavor_truth_BHadronpdg;
   vector<int>     *jet_akt4topoem_flavor_truth_trueflav;
   vector<float>   *jet_akt4topoem_el_dr;
   vector<int>     *jet_akt4topoem_el_matched;
   vector<float>   *jet_akt4topoem_mu_dr;
   vector<int>     *jet_akt4topoem_mu_matched;
   vector<float>   *jet_akt4topoem_L1_dr;
   vector<int>     *jet_akt4topoem_L1_matched;
   vector<float>   *jet_akt4topoem_L2_dr;
   vector<int>     *jet_akt4topoem_L2_matched;
   vector<float>   *jet_akt4topoem_EF_dr;
   vector<int>     *jet_akt4topoem_EF_matched;
   Int_t           jet_akt6topoem_n;
   vector<float>   *jet_akt6topoem_E;
   vector<float>   *jet_akt6topoem_pt;
   vector<float>   *jet_akt6topoem_m;
   vector<float>   *jet_akt6topoem_eta;
   vector<float>   *jet_akt6topoem_phi;
   vector<float>   *jet_akt6topoem_EtaOrigin;
   vector<float>   *jet_akt6topoem_PhiOrigin;
   vector<float>   *jet_akt6topoem_MOrigin;
   vector<float>   *jet_akt6topoem_EtaOriginEM;
   vector<float>   *jet_akt6topoem_PhiOriginEM;
   vector<float>   *jet_akt6topoem_MOriginEM;
   vector<float>   *jet_akt6topoem_WIDTH;
   vector<float>   *jet_akt6topoem_n90;
   vector<float>   *jet_akt6topoem_Timing;
   vector<float>   *jet_akt6topoem_LArQuality;
   vector<float>   *jet_akt6topoem_nTrk;
   vector<float>   *jet_akt6topoem_sumPtTrk;
   vector<float>   *jet_akt6topoem_OriginIndex;
   vector<float>   *jet_akt6topoem_HECQuality;
   vector<float>   *jet_akt6topoem_NegativeE;
   vector<float>   *jet_akt6topoem_YFlip12;
   vector<float>   *jet_akt6topoem_YFlip23;
   vector<float>   *jet_akt6topoem_BCH_CORR_CELL;
   vector<float>   *jet_akt6topoem_BCH_CORR_DOTX;
   vector<float>   *jet_akt6topoem_BCH_CORR_JET;
   vector<float>   *jet_akt6topoem_BCH_CORR_JET_FORCELL;
   vector<float>   *jet_akt6topoem_ENG_BAD_CELLS;
   vector<float>   *jet_akt6topoem_N_BAD_CELLS;
   vector<float>   *jet_akt6topoem_N_BAD_CELLS_CORR;
   vector<float>   *jet_akt6topoem_BAD_CELLS_CORR_E;
   vector<float>   *jet_akt6topoem_NumTowers;
   vector<int>     *jet_akt6topoem_SamplingMax;
   vector<float>   *jet_akt6topoem_fracSamplingMax;
   vector<float>   *jet_akt6topoem_hecf;
   vector<float>   *jet_akt6topoem_tgap3f;
   vector<int>     *jet_akt6topoem_isUgly;
   vector<int>     *jet_akt6topoem_isBadLoose;
   vector<int>     *jet_akt6topoem_isBadMedium;
   vector<int>     *jet_akt6topoem_isBadTight;
   vector<float>   *jet_akt6topoem_emfrac;
   vector<float>   *jet_akt6topoem_Offset;
   vector<float>   *jet_akt6topoem_EMJES;
   vector<float>   *jet_akt6topoem_EMJES_EtaCorr;
   vector<float>   *jet_akt6topoem_EMJESnooffset;
   vector<float>   *jet_akt6topoem_GCWJES;
   vector<float>   *jet_akt6topoem_GCWJES_EtaCorr;
   vector<float>   *jet_akt6topoem_CB;
   vector<float>   *jet_akt6topoem_emscale_E;
   vector<float>   *jet_akt6topoem_emscale_pt;
   vector<float>   *jet_akt6topoem_emscale_m;
   vector<float>   *jet_akt6topoem_emscale_eta;
   vector<float>   *jet_akt6topoem_emscale_phi;
   vector<float>   *jet_akt6topoem_jvtxf;
   vector<float>   *jet_akt6topoem_jvtx_x;
   vector<float>   *jet_akt6topoem_jvtx_y;
   vector<float>   *jet_akt6topoem_jvtx_z;
   vector<float>   *jet_akt6topoem_GSCFactorF;
   vector<float>   *jet_akt6topoem_WidthFraction;
   vector<double>  *jet_akt6topoem_flavor_component_ip2d_pu;
   vector<double>  *jet_akt6topoem_flavor_component_ip2d_pb;
   vector<double>  *jet_akt6topoem_flavor_component_ip2d_ntrk;
   vector<double>  *jet_akt6topoem_flavor_component_ip3d_pu;
   vector<double>  *jet_akt6topoem_flavor_component_ip3d_pb;
   vector<double>  *jet_akt6topoem_flavor_component_ip3d_ntrk;
   vector<double>  *jet_akt6topoem_flavor_component_sv1_pu;
   vector<double>  *jet_akt6topoem_flavor_component_sv1_pb;
   vector<double>  *jet_akt6topoem_flavor_component_sv2_pu;
   vector<double>  *jet_akt6topoem_flavor_component_sv2_pb;
   vector<double>  *jet_akt6topoem_flavor_component_svp_mass;
   vector<double>  *jet_akt6topoem_flavor_component_svp_n2t;
   vector<double>  *jet_akt6topoem_flavor_component_svp_svok;
   vector<double>  *jet_akt6topoem_flavor_component_svp_ntrk;
   vector<double>  *jet_akt6topoem_flavor_component_svp_ntrkv;
   vector<double>  *jet_akt6topoem_flavor_component_svp_ntrkj;
   vector<double>  *jet_akt6topoem_flavor_component_svp_efrc;
   vector<double>  *jet_akt6topoem_flavor_component_sv0p_mass;
   vector<double>  *jet_akt6topoem_flavor_component_sv0p_n2t;
   vector<double>  *jet_akt6topoem_flavor_component_sv0p_svok;
   vector<double>  *jet_akt6topoem_flavor_component_sv0p_ntrk;
   vector<double>  *jet_akt6topoem_flavor_component_sv0p_ntrkv;
   vector<double>  *jet_akt6topoem_flavor_component_sv0p_ntrkj;
   vector<double>  *jet_akt6topoem_flavor_component_sv0p_efrc;
   vector<double>  *jet_akt6topoem_flavor_component_jfit_pu;
   vector<double>  *jet_akt6topoem_flavor_component_jfit_pb;
   vector<double>  *jet_akt6topoem_flavor_component_jfit_nvtxnt;
   vector<double>  *jet_akt6topoem_flavor_component_jfit_nvtx1t;
   vector<double>  *jet_akt6topoem_flavor_component_jfit_ntrk;
   vector<double>  *jet_akt6topoem_flavor_component_jfit_efrc;
   vector<double>  *jet_akt6topoem_flavor_component_jfit_mass;
   vector<double>  *jet_akt6topoem_flavor_component_jfit_sig3d;
   vector<double>  *jet_akt6topoem_flavor_weight_TrackCounting2D;
   vector<double>  *jet_akt6topoem_flavor_weight_JetProb;
   vector<double>  *jet_akt6topoem_flavor_weight_IP1D;
   vector<double>  *jet_akt6topoem_flavor_weight_IP2D;
   vector<double>  *jet_akt6topoem_flavor_weight_IP3D;
   vector<double>  *jet_akt6topoem_flavor_weight_SV0;
   vector<double>  *jet_akt6topoem_flavor_weight_SV1;
   vector<double>  *jet_akt6topoem_flavor_weight_SV2;
   vector<double>  *jet_akt6topoem_flavor_weight_JetFitterTag;
   vector<double>  *jet_akt6topoem_flavor_weight_JetFitterCOMB;
   vector<double>  *jet_akt6topoem_flavor_weight_JetFitterTagNN;
   vector<double>  *jet_akt6topoem_flavor_weight_JetFitterCOMBNN;
   vector<double>  *jet_akt6topoem_flavor_weight_SoftMuonTag;
   vector<double>  *jet_akt6topoem_flavor_weight_SoftElectronTag;
   vector<double>  *jet_akt6topoem_flavor_weight_IP3DSV1;
   vector<double>  *jet_akt6topoem_flavor_truth_dRminToB;
   vector<double>  *jet_akt6topoem_flavor_truth_dRminToC;
   vector<double>  *jet_akt6topoem_flavor_truth_dRminToT;
   vector<int>     *jet_akt6topoem_flavor_truth_BHadronpdg;
   vector<int>     *jet_akt6topoem_flavor_truth_trueflav;
   vector<float>   *jet_akt6topoem_el_dr;
   vector<int>     *jet_akt6topoem_el_matched;
   vector<float>   *jet_akt6topoem_mu_dr;
   vector<int>     *jet_akt6topoem_mu_matched;
   vector<float>   *jet_akt6topoem_L1_dr;
   vector<int>     *jet_akt6topoem_L1_matched;
   vector<float>   *jet_akt6topoem_L2_dr;
   vector<int>     *jet_akt6topoem_L2_matched;
   vector<float>   *jet_akt6topoem_EF_dr;
   vector<int>     *jet_akt6topoem_EF_matched;
   Int_t           top_hfor_type;
   Float_t         MET_RefFinal_phi;
   Float_t         MET_RefFinal_et;
   Float_t         MET_RefFinal_sumet;
   Float_t         MET_Cryo_phi;
   Float_t         MET_Cryo_et;
   Float_t         MET_Cryo_sumet;
   Float_t         MET_RefEle_phi;
   Float_t         MET_RefEle_et;
   Float_t         MET_RefEle_sumet;
   Float_t         MET_RefJet_phi;
   Float_t         MET_RefJet_et;
   Float_t         MET_RefJet_sumet;
   Float_t         MET_SoftJets_phi;
   Float_t         MET_SoftJets_et;
   Float_t         MET_SoftJets_sumet;
   Float_t         MET_RefMuon_phi;
   Float_t         MET_RefMuon_et;
   Float_t         MET_RefMuon_sumet;
   Float_t         MET_RefMuon_Staco_phi;
   Float_t         MET_RefMuon_Staco_et;
   Float_t         MET_RefMuon_Staco_sumet;
   Float_t         MET_RefMuon_Muid_phi;
   Float_t         MET_RefMuon_Muid_et;
   Float_t         MET_RefMuon_Muid_sumet;
   Float_t         MET_RefGamma_phi;
   Float_t         MET_RefGamma_et;
   Float_t         MET_RefGamma_sumet;
   Float_t         MET_RefTau_phi;
   Float_t         MET_RefTau_et;
   Float_t         MET_RefTau_sumet;
   Float_t         MET_CellOut_phi;
   Float_t         MET_CellOut_et;
   Float_t         MET_CellOut_sumet;
   Float_t         MET_Track_phi;
   Float_t         MET_Track_et;
   Float_t         MET_Track_sumet;
   Float_t         MET_RefFinal_em_etx;
   Float_t         MET_RefFinal_em_ety;
   Float_t         MET_RefFinal_em_phi;
   Float_t         MET_RefFinal_em_et;
   Float_t         MET_RefFinal_em_sumet;
   Float_t         MET_RefEle_em_phi;
   Float_t         MET_RefEle_em_et;
   Float_t         MET_RefEle_em_sumet;
   Float_t         MET_RefJet_em_phi;
   Float_t         MET_RefJet_em_et;
   Float_t         MET_RefJet_em_sumet;
   Float_t         MET_SoftJets_em_phi;
   Float_t         MET_SoftJets_em_et;
   Float_t         MET_SoftJets_em_sumet;
   Float_t         MET_RefMuon_em_phi;
   Float_t         MET_RefMuon_em_et;
   Float_t         MET_RefMuon_em_sumet;
   Float_t         MET_RefMuon_Track_em_phi;
   Float_t         MET_RefMuon_Track_em_et;
   Float_t         MET_RefMuon_Track_em_sumet;
   Float_t         MET_RefGamma_em_phi;
   Float_t         MET_RefGamma_em_et;
   Float_t         MET_RefGamma_em_sumet;
   Float_t         MET_RefTau_em_phi;
   Float_t         MET_RefTau_em_et;
   Float_t         MET_RefTau_em_sumet;
   Float_t         MET_CellOut_em_etx;
   Float_t         MET_CellOut_em_ety;
   Float_t         MET_CellOut_em_phi;
   Float_t         MET_CellOut_em_et;
   Float_t         MET_CellOut_em_sumet;
   Float_t         MET_Muon_Isol_Staco_phi;
   Float_t         MET_Muon_Isol_Staco_et;
   Float_t         MET_Muon_Isol_Staco_sumet;
   Float_t         MET_Muon_NonIsol_Staco_phi;
   Float_t         MET_Muon_NonIsol_Staco_et;
   Float_t         MET_Muon_NonIsol_Staco_sumet;
   Float_t         MET_Muon_Total_Staco_phi;
   Float_t         MET_Muon_Total_Staco_et;
   Float_t         MET_Muon_Total_Staco_sumet;
   Float_t         MET_Muon_Isol_Muid_phi;
   Float_t         MET_Muon_Isol_Muid_et;
   Float_t         MET_Muon_Isol_Muid_sumet;
   Float_t         MET_Muon_NonIsol_Muid_phi;
   Float_t         MET_Muon_NonIsol_Muid_et;
   Float_t         MET_Muon_NonIsol_Muid_sumet;
   Float_t         MET_Muon_Total_Muid_phi;
   Float_t         MET_Muon_Total_Muid_et;
   Float_t         MET_Muon_Total_Muid_sumet;
   Float_t         MET_MuonBoy_phi;
   Float_t         MET_MuonBoy_et;
   Float_t         MET_MuonBoy_sumet;
   Float_t         MET_RefMuon_Track_phi;
   Float_t         MET_RefMuon_Track_et;
   Float_t         MET_RefMuon_Track_sumet;
   Float_t         MET_RefMuon_Track_Staco_phi;
   Float_t         MET_RefMuon_Track_Staco_et;
   Float_t         MET_RefMuon_Track_Staco_sumet;
   Float_t         MET_RefMuon_Track_Muid_phi;
   Float_t         MET_RefMuon_Track_Muid_et;
   Float_t         MET_RefMuon_Track_Muid_sumet;
   Float_t         MET_CryoCone_phi;
   Float_t         MET_CryoCone_et;
   Float_t         MET_CryoCone_sumet;
   Float_t         MET_Final_phi;
   Float_t         MET_Final_et;
   Float_t         MET_Final_sumet;
   Float_t         MET_MuonBoy_Spectro_phi;
   Float_t         MET_MuonBoy_Spectro_et;
   Float_t         MET_MuonBoy_Spectro_sumet;
   Float_t         MET_MuonBoy_Track_phi;
   Float_t         MET_MuonBoy_Track_et;
   Float_t         MET_MuonBoy_Track_sumet;
   Float_t         MET_MuonMuid_phi;
   Float_t         MET_MuonMuid_et;
   Float_t         MET_MuonMuid_sumet;
   Float_t         MET_Muid_phi;
   Float_t         MET_Muid_et;
   Float_t         MET_Muid_sumet;
   Float_t         MET_Muid_Spectro_phi;
   Float_t         MET_Muid_Spectro_et;
   Float_t         MET_Muid_Spectro_sumet;
   Float_t         MET_Muid_Track_phi;
   Float_t         MET_Muid_Track_et;
   Float_t         MET_Muid_Track_sumet;
   Float_t         MET_Muon_phi;
   Float_t         MET_Muon_et;
   Float_t         MET_Muon_sumet;
   Float_t         MET_TopoObj_phi;
   Float_t         MET_TopoObj_et;
   Float_t         MET_TopoObj_sumet;
   Float_t         MET_LocHadTopoObj_phi;
   Float_t         MET_LocHadTopoObj_et;
   Float_t         MET_LocHadTopoObj_sumet;
   Float_t         MET_Topo_phi;
   Float_t         MET_Topo_et;
   Float_t         MET_Topo_sumet;
   Float_t         MET_Topo_SUMET_EMFrac;
   Float_t         MET_Topo_etx_PEMB;
   Float_t         MET_Topo_ety_PEMB;
   Float_t         MET_Topo_sumet_PEMB;
   Float_t         MET_Topo_phi_PEMB;
   Float_t         MET_Topo_etx_EMB;
   Float_t         MET_Topo_ety_EMB;
   Float_t         MET_Topo_sumet_EMB;
   Float_t         MET_Topo_phi_EMB;
   Float_t         MET_Topo_etx_PEMEC;
   Float_t         MET_Topo_ety_PEMEC;
   Float_t         MET_Topo_sumet_PEMEC;
   Float_t         MET_Topo_phi_PEMEC;
   Float_t         MET_Topo_etx_EME;
   Float_t         MET_Topo_ety_EME;
   Float_t         MET_Topo_sumet_EME;
   Float_t         MET_Topo_phi_EME;
   Float_t         MET_Topo_etx_TILE;
   Float_t         MET_Topo_ety_TILE;
   Float_t         MET_Topo_sumet_TILE;
   Float_t         MET_Topo_phi_TILE;
   Float_t         MET_Topo_etx_HEC;
   Float_t         MET_Topo_ety_HEC;
   Float_t         MET_Topo_sumet_HEC;
   Float_t         MET_Topo_phi_HEC;
   Float_t         MET_Topo_etx_FCAL;
   Float_t         MET_Topo_ety_FCAL;
   Float_t         MET_Topo_sumet_FCAL;
   Float_t         MET_Topo_phi_FCAL;
   Float_t         MET_Topo_nCell_PEMB;
   Float_t         MET_Topo_nCell_EMB;
   Float_t         MET_Topo_nCell_PEMEC;
   Float_t         MET_Topo_nCell_EME;
   Float_t         MET_Topo_nCell_TILE;
   Float_t         MET_Topo_nCell_HEC;
   Float_t         MET_Topo_nCell_FCAL;
   Float_t         MET_Topo_etx_CentralReg;
   Float_t         MET_Topo_ety_CentralReg;
   Float_t         MET_Topo_sumet_CentralReg;
   Float_t         MET_Topo_phi_CentralReg;
   Float_t         MET_Topo_etx_EndcapRegion;
   Float_t         MET_Topo_ety_EndcapRegion;
   Float_t         MET_Topo_sumet_EndcapRegion;
   Float_t         MET_Topo_phi_EndcapRegion;
   Float_t         MET_Topo_etx_ForwardReg;
   Float_t         MET_Topo_ety_ForwardReg;
   Float_t         MET_Topo_sumet_ForwardReg;
   Float_t         MET_Topo_phi_ForwardReg;
   Float_t         MET_CorrTopo_phi;
   Float_t         MET_CorrTopo_et;
   Float_t         MET_CorrTopo_sumet;
   Float_t         MET_CorrTopo_SUMET_EMFrac;
   Float_t         MET_CorrTopo_etx_PEMB;
   Float_t         MET_CorrTopo_ety_PEMB;
   Float_t         MET_CorrTopo_sumet_PEMB;
   Float_t         MET_CorrTopo_phi_PEMB;
   Float_t         MET_CorrTopo_etx_EMB;
   Float_t         MET_CorrTopo_ety_EMB;
   Float_t         MET_CorrTopo_sumet_EMB;
   Float_t         MET_CorrTopo_phi_EMB;
   Float_t         MET_CorrTopo_etx_PEMEC;
   Float_t         MET_CorrTopo_ety_PEMEC;
   Float_t         MET_CorrTopo_sumet_PEMEC;
   Float_t         MET_CorrTopo_phi_PEMEC;
   Float_t         MET_CorrTopo_etx_EME;
   Float_t         MET_CorrTopo_ety_EME;
   Float_t         MET_CorrTopo_sumet_EME;
   Float_t         MET_CorrTopo_phi_EME;
   Float_t         MET_CorrTopo_etx_TILE;
   Float_t         MET_CorrTopo_ety_TILE;
   Float_t         MET_CorrTopo_sumet_TILE;
   Float_t         MET_CorrTopo_phi_TILE;
   Float_t         MET_CorrTopo_etx_HEC;
   Float_t         MET_CorrTopo_ety_HEC;
   Float_t         MET_CorrTopo_sumet_HEC;
   Float_t         MET_CorrTopo_phi_HEC;
   Float_t         MET_CorrTopo_etx_FCAL;
   Float_t         MET_CorrTopo_ety_FCAL;
   Float_t         MET_CorrTopo_sumet_FCAL;
   Float_t         MET_CorrTopo_phi_FCAL;
   Float_t         MET_CorrTopo_nCell_PEMB;
   Float_t         MET_CorrTopo_nCell_EMB;
   Float_t         MET_CorrTopo_nCell_PEMEC;
   Float_t         MET_CorrTopo_nCell_EME;
   Float_t         MET_CorrTopo_nCell_TILE;
   Float_t         MET_CorrTopo_nCell_HEC;
   Float_t         MET_CorrTopo_nCell_FCAL;
   Float_t         MET_CorrTopo_etx_CentralReg;
   Float_t         MET_CorrTopo_ety_CentralReg;
   Float_t         MET_CorrTopo_sumet_CentralReg;
   Float_t         MET_CorrTopo_phi_CentralReg;
   Float_t         MET_CorrTopo_etx_EndcapRegion;
   Float_t         MET_CorrTopo_ety_EndcapRegion;
   Float_t         MET_CorrTopo_sumet_EndcapRegion;
   Float_t         MET_CorrTopo_phi_EndcapRegion;
   Float_t         MET_CorrTopo_etx_ForwardReg;
   Float_t         MET_CorrTopo_ety_ForwardReg;
   Float_t         MET_CorrTopo_sumet_ForwardReg;
   Float_t         MET_CorrTopo_phi_ForwardReg;
   Float_t         MET_LocHadTopo_phi;
   Float_t         MET_LocHadTopo_et;
   Float_t         MET_LocHadTopo_sumet;
   Float_t         MET_LocHadTopo_SUMET_EMFrac;
   Float_t         MET_LocHadTopo_etx_PEMB;
   Float_t         MET_LocHadTopo_ety_PEMB;
   Float_t         MET_LocHadTopo_sumet_PEMB;
   Float_t         MET_LocHadTopo_phi_PEMB;
   Float_t         MET_LocHadTopo_etx_EMB;
   Float_t         MET_LocHadTopo_ety_EMB;
   Float_t         MET_LocHadTopo_sumet_EMB;
   Float_t         MET_LocHadTopo_phi_EMB;
   Float_t         MET_LocHadTopo_etx_PEMEC;
   Float_t         MET_LocHadTopo_ety_PEMEC;
   Float_t         MET_LocHadTopo_sumet_PEMEC;
   Float_t         MET_LocHadTopo_phi_PEMEC;
   Float_t         MET_LocHadTopo_etx_EME;
   Float_t         MET_LocHadTopo_ety_EME;
   Float_t         MET_LocHadTopo_sumet_EME;
   Float_t         MET_LocHadTopo_phi_EME;
   Float_t         MET_LocHadTopo_etx_TILE;
   Float_t         MET_LocHadTopo_ety_TILE;
   Float_t         MET_LocHadTopo_sumet_TILE;
   Float_t         MET_LocHadTopo_phi_TILE;
   Float_t         MET_LocHadTopo_etx_HEC;
   Float_t         MET_LocHadTopo_ety_HEC;
   Float_t         MET_LocHadTopo_sumet_HEC;
   Float_t         MET_LocHadTopo_phi_HEC;
   Float_t         MET_LocHadTopo_etx_FCAL;
   Float_t         MET_LocHadTopo_ety_FCAL;
   Float_t         MET_LocHadTopo_sumet_FCAL;
   Float_t         MET_LocHadTopo_phi_FCAL;
   Float_t         MET_LocHadTopo_nCell_PEMB;
   Float_t         MET_LocHadTopo_nCell_EMB;
   Float_t         MET_LocHadTopo_nCell_PEMEC;
   Float_t         MET_LocHadTopo_nCell_EME;
   Float_t         MET_LocHadTopo_nCell_TILE;
   Float_t         MET_LocHadTopo_nCell_HEC;
   Float_t         MET_LocHadTopo_nCell_FCAL;
   Float_t         MET_LocHadTopo_etx_CentralReg;
   Float_t         MET_LocHadTopo_ety_CentralReg;
   Float_t         MET_LocHadTopo_sumet_CentralReg;
   Float_t         MET_LocHadTopo_phi_CentralReg;
   Float_t         MET_LocHadTopo_etx_EndcapRegion;
   Float_t         MET_LocHadTopo_ety_EndcapRegion;
   Float_t         MET_LocHadTopo_sumet_EndcapRegion;
   Float_t         MET_LocHadTopo_phi_EndcapRegion;
   Float_t         MET_LocHadTopo_etx_ForwardReg;
   Float_t         MET_LocHadTopo_ety_ForwardReg;
   Float_t         MET_LocHadTopo_sumet_ForwardReg;
   Float_t         MET_LocHadTopo_phi_ForwardReg;
   Float_t         MET_Calib_phi;
   Float_t         MET_Calib_et;
   Float_t         MET_Calib_sumet;
   Float_t         MET_Calib_SUMET_EMFrac;
   Float_t         MET_Calib_etx_PEMB;
   Float_t         MET_Calib_ety_PEMB;
   Float_t         MET_Calib_sumet_PEMB;
   Float_t         MET_Calib_phi_PEMB;
   Float_t         MET_Calib_etx_EMB;
   Float_t         MET_Calib_ety_EMB;
   Float_t         MET_Calib_sumet_EMB;
   Float_t         MET_Calib_phi_EMB;
   Float_t         MET_Calib_etx_PEMEC;
   Float_t         MET_Calib_ety_PEMEC;
   Float_t         MET_Calib_sumet_PEMEC;
   Float_t         MET_Calib_phi_PEMEC;
   Float_t         MET_Calib_etx_EME;
   Float_t         MET_Calib_ety_EME;
   Float_t         MET_Calib_sumet_EME;
   Float_t         MET_Calib_phi_EME;
   Float_t         MET_Calib_etx_TILE;
   Float_t         MET_Calib_ety_TILE;
   Float_t         MET_Calib_sumet_TILE;
   Float_t         MET_Calib_phi_TILE;
   Float_t         MET_Calib_etx_HEC;
   Float_t         MET_Calib_ety_HEC;
   Float_t         MET_Calib_sumet_HEC;
   Float_t         MET_Calib_phi_HEC;
   Float_t         MET_Calib_etx_FCAL;
   Float_t         MET_Calib_ety_FCAL;
   Float_t         MET_Calib_sumet_FCAL;
   Float_t         MET_Calib_phi_FCAL;
   Float_t         MET_Calib_nCell_PEMB;
   Float_t         MET_Calib_nCell_EMB;
   Float_t         MET_Calib_nCell_PEMEC;
   Float_t         MET_Calib_nCell_EME;
   Float_t         MET_Calib_nCell_TILE;
   Float_t         MET_Calib_nCell_HEC;
   Float_t         MET_Calib_nCell_FCAL;
   Float_t         MET_Calib_etx_CentralReg;
   Float_t         MET_Calib_ety_CentralReg;
   Float_t         MET_Calib_sumet_CentralReg;
   Float_t         MET_Calib_phi_CentralReg;
   Float_t         MET_Calib_etx_EndcapRegion;
   Float_t         MET_Calib_ety_EndcapRegion;
   Float_t         MET_Calib_sumet_EndcapRegion;
   Float_t         MET_Calib_phi_EndcapRegion;
   Float_t         MET_Calib_etx_ForwardReg;
   Float_t         MET_Calib_ety_ForwardReg;
   Float_t         MET_Calib_sumet_ForwardReg;
   Float_t         MET_Calib_phi_ForwardReg;
   Float_t         MET_Truth_NonInt_etx;
   Float_t         MET_Truth_NonInt_ety;
   Float_t         MET_Truth_NonInt_phi;
   Float_t         MET_Truth_NonInt_et;
   Float_t         MET_Truth_NonInt_sumet;
   Float_t         MET_Truth_Int_phi;
   Float_t         MET_Truth_IntCentral_phi;
   Float_t         MET_Truth_IntFwd_phi;
   Float_t         MET_Truth_IntOutCover_phi;
   Float_t         MET_Truth_IntMuons_phi;
   Float_t         MET_Truth_Int_et;
   Float_t         MET_Truth_IntCentral_et;
   Float_t         MET_Truth_IntFwd_et;
   Float_t         MET_Truth_IntOutCover_et;
   Float_t         MET_Truth_IntMuons_et;
   Float_t         MET_Truth_Int_sumet;
   Float_t         MET_Truth_IntCentral_sumet;
   Float_t         MET_Truth_IntFwd_sumet;
   Float_t         MET_Truth_IntOutCover_sumet;
   Float_t         MET_Truth_IntMuons_sumet;
   Float_t         MET_Truth_PileUp_NonInt_etx;
   Float_t         MET_Truth_PileUp_NonInt_ety;
   Float_t         MET_Truth_PileUp_NonInt_phi;
   Float_t         MET_Truth_PileUp_NonInt_et;
   Float_t         MET_Truth_PileUp_NonInt_sumet;
   Float_t         MET_Truth_PileUp_Int_etx;
   Float_t         MET_Truth_PileUp_Int_ety;
   Float_t         MET_Truth_PileUp_IntCentral_etx;
   Float_t         MET_Truth_PileUp_IntCentral_ety;
   Float_t         MET_Truth_PileUp_IntFwd_etx;
   Float_t         MET_Truth_PileUp_IntFwd_ety;
   Float_t         MET_Truth_PileUp_IntOutCover_etx;
   Float_t         MET_Truth_PileUp_IntOutCover_ety;
   Float_t         MET_Truth_PileUp_IntMuons_etx;
   Float_t         MET_Truth_PileUp_IntMuons_ety;
   Float_t         MET_Truth_PileUp_Int_phi;
   Float_t         MET_Truth_PileUp_IntCentral_phi;
   Float_t         MET_Truth_PileUp_IntFwd_phi;
   Float_t         MET_Truth_PileUp_IntOutCover_phi;
   Float_t         MET_Truth_PileUp_IntMuons_phi;
   Float_t         MET_Truth_PileUp_Int_et;
   Float_t         MET_Truth_PileUp_IntCentral_et;
   Float_t         MET_Truth_PileUp_IntFwd_et;
   Float_t         MET_Truth_PileUp_IntOutCover_et;
   Float_t         MET_Truth_PileUp_IntMuons_et;
   Float_t         MET_Truth_PileUp_Int_sumet;
   Float_t         MET_Truth_PileUp_IntCentral_sumet;
   Float_t         MET_Truth_PileUp_IntFwd_sumet;
   Float_t         MET_Truth_PileUp_IntOutCover_sumet;
   Float_t         MET_Truth_PileUp_IntMuons_sumet;
   Float_t         MET_DM_Crack1_etx;
   Float_t         MET_DM_Crack1_ety;
   Float_t         MET_DM_Crack1_phi;
   Float_t         MET_DM_Crack1_et;
   Float_t         MET_DM_Crack1_sumet;
   Float_t         MET_DM_Crack2_etx;
   Float_t         MET_DM_Crack2_ety;
   Float_t         MET_DM_Crack2_phi;
   Float_t         MET_DM_Crack2_et;
   Float_t         MET_DM_Crack2_sumet;
   Float_t         MET_DM_All_etx;
   Float_t         MET_DM_All_ety;
   Float_t         MET_DM_All_phi;
   Float_t         MET_DM_All_et;
   Float_t         MET_DM_All_sumet;
   Float_t         MET_DM_Cryo_etx;
   Float_t         MET_DM_Cryo_ety;
   Float_t         MET_DM_Cryo_phi;
   Float_t         MET_DM_Cryo_et;
   Float_t         MET_DM_Cryo_sumet;
   Float_t         METJetsInfo_JetPtWeightedEventEMFraction;
   Float_t         METJetsInfo_JetPtWeightedNumAssociatedTracks;
   Float_t         METJetsInfo_JetPtWeightedSize;
   Float_t         METJetsInfo_LeadingJetEt;
   Float_t         METJetsInfo_LeadingJetEta;
   Int_t           cl_n;
   vector<float>   *cl_pt;
   vector<float>   *cl_eta;
   vector<float>   *cl_phi;
   Int_t           clpt10_n;
   vector<float>   *clpt10_pt;
   vector<float>   *clpt10_eta;
   vector<float>   *clpt10_phi;
   vector<float>   *clpt10_E_em;
   vector<float>   *clpt10_E_had;
   vector<float>   *clpt10_firstEdens;
   vector<float>   *clpt10_cellmaxfrac;
   vector<float>   *clpt10_longitudinal;
   vector<float>   *clpt10_secondlambda;
   vector<float>   *clpt10_lateral;
   vector<float>   *clpt10_secondR;
   vector<float>   *clpt10_centerlambda;
   vector<float>   *clpt10_deltaTheta;
   vector<float>   *clpt10_deltaPhi;
   vector<float>   *clpt10_time;
   vector<float>   *clpt10_E_PreSamplerB;
   vector<float>   *clpt10_E_EMB1;
   vector<float>   *clpt10_E_EMB2;
   vector<float>   *clpt10_E_EMB3;
   vector<float>   *clpt10_E_PreSamplerE;
   vector<float>   *clpt10_E_EME1;
   vector<float>   *clpt10_E_EME2;
   vector<float>   *clpt10_E_EME3;
   vector<float>   *clpt10_E_HEC0;
   vector<float>   *clpt10_E_HEC1;
   vector<float>   *clpt10_E_HEC2;
   vector<float>   *clpt10_E_HEC3;
   vector<float>   *clpt10_E_TileBar0;
   vector<float>   *clpt10_E_TileBar1;
   vector<float>   *clpt10_E_TileBar2;
   vector<float>   *clpt10_E_TileGap1;
   vector<float>   *clpt10_E_TileGap2;
   vector<float>   *clpt10_E_TileGap3;
   vector<float>   *clpt10_E_TileExt0;
   vector<float>   *clpt10_E_TileExt1;
   vector<float>   *clpt10_E_TileExt2;
   vector<float>   *clpt10_E_FCAL0;
   vector<float>   *clpt10_E_FCAL1;
   vector<float>   *clpt10_E_FCAL2;
   Int_t           emclpt10_n;
   vector<float>   *emclpt10_pt;
   vector<float>   *emclpt10_eta;
   vector<float>   *emclpt10_phi;
   vector<float>   *emclpt10_E_em;
   vector<float>   *emclpt10_E_had;
   vector<float>   *emclpt10_firstEdens;
   vector<float>   *emclpt10_cellmaxfrac;
   vector<float>   *emclpt10_longitudinal;
   vector<float>   *emclpt10_secondlambda;
   vector<float>   *emclpt10_lateral;
   vector<float>   *emclpt10_secondR;
   vector<float>   *emclpt10_centerlambda;
   vector<float>   *emclpt10_deltaTheta;
   vector<float>   *emclpt10_deltaPhi;
   vector<float>   *emclpt10_time;
   vector<float>   *emclpt10_E_PreSamplerB;
   vector<float>   *emclpt10_E_EMB1;
   vector<float>   *emclpt10_E_EMB2;
   vector<float>   *emclpt10_E_EMB3;
   vector<float>   *emclpt10_E_PreSamplerE;
   vector<float>   *emclpt10_E_EME1;
   vector<float>   *emclpt10_E_EME2;
   vector<float>   *emclpt10_E_EME3;
   vector<float>   *emclpt10_E_HEC0;
   vector<float>   *emclpt10_E_HEC1;
   vector<float>   *emclpt10_E_HEC2;
   vector<float>   *emclpt10_E_HEC3;
   vector<float>   *emclpt10_E_TileBar0;
   vector<float>   *emclpt10_E_TileBar1;
   vector<float>   *emclpt10_E_TileBar2;
   vector<float>   *emclpt10_E_TileGap1;
   vector<float>   *emclpt10_E_TileGap2;
   vector<float>   *emclpt10_E_TileGap3;
   vector<float>   *emclpt10_E_TileExt0;
   vector<float>   *emclpt10_E_TileExt1;
   vector<float>   *emclpt10_E_TileExt2;
   vector<float>   *emclpt10_E_FCAL0;
   vector<float>   *emclpt10_E_FCAL1;
   vector<float>   *emclpt10_E_FCAL2;
   vector<float>   *emclpt10_eta_PreSamplerB;
   vector<float>   *emclpt10_phi_PreSamplerB;
   vector<float>   *emclpt10_eta_EMB1;
   vector<float>   *emclpt10_phi_EMB1;
   vector<float>   *emclpt10_eta_EMB2;
   vector<float>   *emclpt10_phi_EMB2;
   vector<float>   *emclpt10_eta_EMB3;
   vector<float>   *emclpt10_phi_EMB3;
   vector<float>   *emclpt10_eta_PreSamplerE;
   vector<float>   *emclpt10_phi_PreSamplerE;
   vector<float>   *emclpt10_eta_EME1;
   vector<float>   *emclpt10_phi_EME1;
   vector<float>   *emclpt10_eta_EME2;
   vector<float>   *emclpt10_phi_EME2;
   vector<float>   *emclpt10_eta_EME3;
   vector<float>   *emclpt10_phi_EME3;
   vector<float>   *emclpt10_eta_HEC0;
   vector<float>   *emclpt10_phi_HEC0;
   vector<float>   *emclpt10_eta_HEC1;
   vector<float>   *emclpt10_phi_HEC1;
   vector<float>   *emclpt10_eta_HEC2;
   vector<float>   *emclpt10_phi_HEC2;
   vector<float>   *emclpt10_eta_HEC3;
   vector<float>   *emclpt10_phi_HEC3;
   vector<float>   *emclpt10_eta_TileBar0;
   vector<float>   *emclpt10_phi_TileBar0;
   vector<float>   *emclpt10_eta_TileBar1;
   vector<float>   *emclpt10_phi_TileBar1;
   vector<float>   *emclpt10_eta_TileBar2;
   vector<float>   *emclpt10_phi_TileBar2;
   vector<float>   *emclpt10_eta_TileGap1;
   vector<float>   *emclpt10_phi_TileGap1;
   vector<float>   *emclpt10_eta_TileGap2;
   vector<float>   *emclpt10_phi_TileGap2;
   vector<float>   *emclpt10_eta_TileGap3;
   vector<float>   *emclpt10_phi_TileGap3;
   vector<float>   *emclpt10_eta_TileExt0;
   vector<float>   *emclpt10_phi_TileExt0;
   vector<float>   *emclpt10_eta_TileExt1;
   vector<float>   *emclpt10_phi_TileExt1;
   vector<float>   *emclpt10_eta_TileExt2;
   vector<float>   *emclpt10_phi_TileExt2;
   vector<float>   *emclpt10_eta_FCAL0;
   vector<float>   *emclpt10_phi_FCAL0;
   vector<float>   *emclpt10_eta_FCAL1;
   vector<float>   *emclpt10_phi_FCAL1;
   vector<float>   *emclpt10_eta_FCAL2;
   vector<float>   *emclpt10_phi_FCAL2;
   Int_t           trk_n;
   vector<float>   *trk_pt;
   vector<float>   *trk_eta;
   vector<float>   *trk_d0_wrtPV;
   vector<float>   *trk_z0_wrtPV;
   vector<float>   *trk_phi_wrtPV;
   vector<float>   *trk_theta_wrtPV;
   vector<float>   *trk_qoverp_wrtPV;
   vector<float>   *trk_chi2;
   vector<int>     *trk_ndof;
   Int_t           trkpt5_n;
   vector<float>   *trkpt5_d0;
   vector<float>   *trkpt5_z0;
   vector<float>   *trkpt5_phi;
   vector<float>   *trkpt5_theta;
   vector<float>   *trkpt5_qoverp;
   vector<float>   *trkpt5_pt;
   vector<float>   *trkpt5_eta;
   vector<float>   *trkpt5_err_d0;
   vector<float>   *trkpt5_err_z0;
   vector<float>   *trkpt5_err_phi;
   vector<float>   *trkpt5_err_theta;
   vector<float>   *trkpt5_err_qoverp;
   vector<float>   *trkpt5_d0_wrtPV;
   vector<float>   *trkpt5_z0_wrtPV;
   vector<float>   *trkpt5_phi_wrtPV;
   vector<float>   *trkpt5_err_d0_wrtPV;
   vector<float>   *trkpt5_err_z0_wrtPV;
   vector<float>   *trkpt5_err_phi_wrtPV;
   vector<float>   *trkpt5_err_theta_wrtPV;
   vector<float>   *trkpt5_err_qoverp_wrtPV;
   vector<float>   *trkpt5_cov_d0_z0_wrtPV;
   vector<float>   *trkpt5_cov_d0_phi_wrtPV;
   vector<float>   *trkpt5_cov_d0_theta_wrtPV;
   vector<float>   *trkpt5_cov_d0_qoverp_wrtPV;
   vector<float>   *trkpt5_cov_z0_phi_wrtPV;
   vector<float>   *trkpt5_cov_z0_theta_wrtPV;
   vector<float>   *trkpt5_cov_z0_qoverp_wrtPV;
   vector<float>   *trkpt5_cov_phi_theta_wrtPV;
   vector<float>   *trkpt5_cov_phi_qoverp_wrtPV;
   vector<float>   *trkpt5_cov_theta_qoverp_wrtPV;
   vector<float>   *trkpt5_chi2;
   vector<int>     *trkpt5_ndof;
   vector<int>     *trkpt5_nBLHits;
   vector<int>     *trkpt5_nPixHits;
   vector<int>     *trkpt5_nSCTHits;
   vector<int>     *trkpt5_nTRTHits;
   vector<int>     *trkpt5_nTRTHighTHits;
   vector<int>     *trkpt5_nPixHoles;
   vector<int>     *trkpt5_nSCTHoles;
   vector<int>     *trkpt5_nTRTHoles;
   vector<int>     *trkpt5_expectBLayerHit;
   vector<int>     *trkpt5_nMDTHits;
   vector<int>     *trkpt5_nCSCEtaHits;
   vector<int>     *trkpt5_nCSCPhiHits;
   vector<int>     *trkpt5_nRPCEtaHits;
   vector<int>     *trkpt5_nRPCPhiHits;
   vector<int>     *trkpt5_nTGCEtaHits;
   vector<int>     *trkpt5_nTGCPhiHits;
   vector<int>     *trkpt5_nHits;
   vector<int>     *trkpt5_nHoles;
   vector<int>     *trkpt5_hitPattern;
   vector<float>   *trkpt5_TRTHighTHitsRatio;
   vector<float>   *trkpt5_TRTHighTOutliersRatio;
   vector<float>   *trkpt5_mc_probability;
   vector<int>     *trkpt5_mc_barcode;
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
   vector<int>     *vxp_trk_n;
   vector<vector<float> > *vxp_trk_weight;
   vector<vector<int> > *vxp_trk_index;
   Int_t           mb_n;
   vector<float>   *mb_E;
   vector<float>   *mb_eta;
   vector<float>   *mb_phi;
   vector<float>   *mb_time;
   vector<int>     *mb_quality;
   vector<int>     *mb_type;
   vector<int>     *mb_module;
   vector<int>     *mb_channel;
   Float_t         mbtime_timeDiff;
   Float_t         mbtime_timeA;
   Float_t         mbtime_timeC;
   Int_t           mbtime_countA;
   Int_t           mbtime_countC;
   Bool_t          L1_MBTS_1;
   Bool_t          L1_MBTS_1_1;
   Bool_t          L1_MBTS_1_1_EMPTY;
   Bool_t          L1_MBTS_1_1_UNPAIRED_ISO;
   Bool_t          L1_MBTS_1_EMPTY;
   Bool_t          L1_MBTS_1_UNPAIRED_ISO;
   Bool_t          L1_MBTS_2;
   Bool_t          L1_MBTS_2_EMPTY;
   Bool_t          L1_MBTS_2_UNPAIRED_ISO;
   Bool_t          L1_MBTS_2_UNPAIRED_NONISO;
   Bool_t          L1_MBTS_4_4;
   Bool_t          L1_MBTS_4_4_UNPAIRED_ISO;
   Int_t           muonTruth_n;
   vector<float>   *muonTruth_pt;
   vector<float>   *muonTruth_m;
   vector<float>   *muonTruth_eta;
   vector<float>   *muonTruth_phi;
   vector<float>   *muonTruth_charge;
   vector<int>     *muonTruth_PDGID;
   vector<int>     *muonTruth_barcode;
   vector<int>     *muonTruth_type;
   vector<int>     *muonTruth_origin;
   Int_t           mcevt_n;
   vector<int>     *mcevt_signal_process_id;
   vector<int>     *mcevt_event_number;
   vector<double>  *mcevt_event_scale;
   vector<double>  *mcevt_alphaQCD;
   vector<double>  *mcevt_alphaQED;
   vector<int>     *mcevt_pdf_id1;
   vector<int>     *mcevt_pdf_id2;
   vector<double>  *mcevt_pdf_x1;
   vector<double>  *mcevt_pdf_x2;
   vector<double>  *mcevt_pdf_scale;
   vector<double>  *mcevt_pdf1;
   vector<double>  *mcevt_pdf2;
   vector<double>  *mcevt_weight;
   vector<int>     *mcevt_nparticle;
   vector<short>   *mcevt_pileUpType;
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
   Int_t           jet_antikt4truth_n;
   vector<float>   *jet_antikt4truth_E;
   vector<float>   *jet_antikt4truth_pt;
   vector<float>   *jet_antikt4truth_m;
   vector<float>   *jet_antikt4truth_eta;
   vector<float>   *jet_antikt4truth_phi;
   vector<float>   *jet_antikt4truth_EtaOrigin;
   vector<float>   *jet_antikt4truth_PhiOrigin;
   vector<float>   *jet_antikt4truth_MOrigin;
   vector<float>   *jet_antikt4truth_EtaOriginEM;
   vector<float>   *jet_antikt4truth_PhiOriginEM;
   vector<float>   *jet_antikt4truth_MOriginEM;
   vector<float>   *jet_antikt4truth_WIDTH;
   vector<float>   *jet_antikt4truth_n90;
   vector<float>   *jet_antikt4truth_Timing;
   vector<float>   *jet_antikt4truth_LArQuality;
   vector<float>   *jet_antikt4truth_nTrk;
   vector<float>   *jet_antikt4truth_sumPtTrk;
   vector<float>   *jet_antikt4truth_OriginIndex;
   vector<float>   *jet_antikt4truth_HECQuality;
   vector<float>   *jet_antikt4truth_NegativeE;
   vector<float>   *jet_antikt4truth_YFlip12;
   vector<float>   *jet_antikt4truth_YFlip23;
   vector<float>   *jet_antikt4truth_BCH_CORR_CELL;
   vector<float>   *jet_antikt4truth_BCH_CORR_DOTX;
   vector<float>   *jet_antikt4truth_BCH_CORR_JET;
   vector<float>   *jet_antikt4truth_BCH_CORR_JET_FORCELL;
   vector<float>   *jet_antikt4truth_ENG_BAD_CELLS;
   vector<float>   *jet_antikt4truth_N_BAD_CELLS;
   vector<float>   *jet_antikt4truth_N_BAD_CELLS_CORR;
   vector<float>   *jet_antikt4truth_BAD_CELLS_CORR_E;
   vector<float>   *jet_antikt4truth_NumTowers;
   vector<int>     *jet_antikt4truth_SamplingMax;
   vector<float>   *jet_antikt4truth_fracSamplingMax;
   vector<float>   *jet_antikt4truth_hecf;
   vector<float>   *jet_antikt4truth_tgap3f;
   vector<int>     *jet_antikt4truth_isUgly;
   vector<int>     *jet_antikt4truth_isBadLoose;
   vector<int>     *jet_antikt4truth_isBadMedium;
   vector<int>     *jet_antikt4truth_isBadTight;
   vector<float>   *jet_antikt4truth_emfrac;
   vector<float>   *jet_antikt4truth_Offset;
   vector<float>   *jet_antikt4truth_EMJES;
   vector<float>   *jet_antikt4truth_EMJES_EtaCorr;
   vector<float>   *jet_antikt4truth_EMJESnooffset;
   vector<float>   *jet_antikt4truth_GCWJES;
   vector<float>   *jet_antikt4truth_GCWJES_EtaCorr;
   vector<float>   *jet_antikt4truth_CB;
   vector<float>   *jet_antikt4truth_emscale_E;
   vector<float>   *jet_antikt4truth_emscale_pt;
   vector<float>   *jet_antikt4truth_emscale_m;
   vector<float>   *jet_antikt4truth_emscale_eta;
   vector<float>   *jet_antikt4truth_emscale_phi;
   vector<float>   *jet_antikt4truth_el_dr;
   vector<int>     *jet_antikt4truth_el_matched;
   vector<float>   *jet_antikt4truth_mu_dr;
   vector<int>     *jet_antikt4truth_mu_matched;
   vector<float>   *jet_antikt4truth_L1_dr;
   vector<int>     *jet_antikt4truth_L1_matched;
   vector<float>   *jet_antikt4truth_L2_dr;
   vector<int>     *jet_antikt4truth_L2_matched;
   vector<float>   *jet_antikt4truth_EF_dr;
   vector<int>     *jet_antikt4truth_EF_matched;
   Int_t           jet_antikt6truth_n;
   vector<float>   *jet_antikt6truth_E;
   vector<float>   *jet_antikt6truth_pt;
   vector<float>   *jet_antikt6truth_m;
   vector<float>   *jet_antikt6truth_eta;
   vector<float>   *jet_antikt6truth_phi;
   vector<float>   *jet_antikt6truth_EtaOrigin;
   vector<float>   *jet_antikt6truth_PhiOrigin;
   vector<float>   *jet_antikt6truth_MOrigin;
   vector<float>   *jet_antikt6truth_EtaOriginEM;
   vector<float>   *jet_antikt6truth_PhiOriginEM;
   vector<float>   *jet_antikt6truth_MOriginEM;
   vector<float>   *jet_antikt6truth_WIDTH;
   vector<float>   *jet_antikt6truth_n90;
   vector<float>   *jet_antikt6truth_Timing;
   vector<float>   *jet_antikt6truth_LArQuality;
   vector<float>   *jet_antikt6truth_nTrk;
   vector<float>   *jet_antikt6truth_sumPtTrk;
   vector<float>   *jet_antikt6truth_OriginIndex;
   vector<float>   *jet_antikt6truth_HECQuality;
   vector<float>   *jet_antikt6truth_NegativeE;
   vector<float>   *jet_antikt6truth_YFlip12;
   vector<float>   *jet_antikt6truth_YFlip23;
   vector<float>   *jet_antikt6truth_BCH_CORR_CELL;
   vector<float>   *jet_antikt6truth_BCH_CORR_DOTX;
   vector<float>   *jet_antikt6truth_BCH_CORR_JET;
   vector<float>   *jet_antikt6truth_BCH_CORR_JET_FORCELL;
   vector<float>   *jet_antikt6truth_ENG_BAD_CELLS;
   vector<float>   *jet_antikt6truth_N_BAD_CELLS;
   vector<float>   *jet_antikt6truth_N_BAD_CELLS_CORR;
   vector<float>   *jet_antikt6truth_BAD_CELLS_CORR_E;
   vector<float>   *jet_antikt6truth_NumTowers;
   vector<int>     *jet_antikt6truth_SamplingMax;
   vector<float>   *jet_antikt6truth_fracSamplingMax;
   vector<float>   *jet_antikt6truth_hecf;
   vector<float>   *jet_antikt6truth_tgap3f;
   vector<int>     *jet_antikt6truth_isUgly;
   vector<int>     *jet_antikt6truth_isBadLoose;
   vector<int>     *jet_antikt6truth_isBadMedium;
   vector<int>     *jet_antikt6truth_isBadTight;
   vector<float>   *jet_antikt6truth_emfrac;
   vector<float>   *jet_antikt6truth_Offset;
   vector<float>   *jet_antikt6truth_EMJES;
   vector<float>   *jet_antikt6truth_EMJES_EtaCorr;
   vector<float>   *jet_antikt6truth_EMJESnooffset;
   vector<float>   *jet_antikt6truth_GCWJES;
   vector<float>   *jet_antikt6truth_GCWJES_EtaCorr;
   vector<float>   *jet_antikt6truth_CB;
   vector<float>   *jet_antikt6truth_emscale_E;
   vector<float>   *jet_antikt6truth_emscale_pt;
   vector<float>   *jet_antikt6truth_emscale_m;
   vector<float>   *jet_antikt6truth_emscale_eta;
   vector<float>   *jet_antikt6truth_emscale_phi;
   vector<float>   *jet_antikt6truth_el_dr;
   vector<int>     *jet_antikt6truth_el_matched;
   vector<float>   *jet_antikt6truth_mu_dr;
   vector<int>     *jet_antikt6truth_mu_matched;
   vector<float>   *jet_antikt6truth_L1_dr;
   vector<int>     *jet_antikt6truth_L1_matched;
   vector<float>   *jet_antikt6truth_L2_dr;
   vector<int>     *jet_antikt6truth_L2_matched;
   vector<float>   *jet_antikt6truth_EF_dr;
   vector<int>     *jet_antikt6truth_EF_matched;
   Int_t           jet_antikt4truthALL_n;
   vector<float>   *jet_antikt4truthALL_E;
   vector<float>   *jet_antikt4truthALL_pt;
   vector<float>   *jet_antikt4truthALL_m;
   vector<float>   *jet_antikt4truthALL_eta;
   vector<float>   *jet_antikt4truthALL_phi;
   vector<float>   *jet_antikt4truthALL_EtaOrigin;
   vector<float>   *jet_antikt4truthALL_PhiOrigin;
   vector<float>   *jet_antikt4truthALL_MOrigin;
   vector<float>   *jet_antikt4truthALL_EtaOriginEM;
   vector<float>   *jet_antikt4truthALL_PhiOriginEM;
   vector<float>   *jet_antikt4truthALL_MOriginEM;
   vector<float>   *jet_antikt4truthALL_WIDTH;
   vector<float>   *jet_antikt4truthALL_n90;
   vector<float>   *jet_antikt4truthALL_Timing;
   vector<float>   *jet_antikt4truthALL_LArQuality;
   vector<float>   *jet_antikt4truthALL_nTrk;
   vector<float>   *jet_antikt4truthALL_sumPtTrk;
   vector<float>   *jet_antikt4truthALL_OriginIndex;
   vector<float>   *jet_antikt4truthALL_HECQuality;
   vector<float>   *jet_antikt4truthALL_NegativeE;
   vector<float>   *jet_antikt4truthALL_YFlip12;
   vector<float>   *jet_antikt4truthALL_YFlip23;
   vector<float>   *jet_antikt4truthALL_BCH_CORR_CELL;
   vector<float>   *jet_antikt4truthALL_BCH_CORR_DOTX;
   vector<float>   *jet_antikt4truthALL_BCH_CORR_JET;
   vector<float>   *jet_antikt4truthALL_BCH_CORR_JET_FORCELL;
   vector<float>   *jet_antikt4truthALL_ENG_BAD_CELLS;
   vector<float>   *jet_antikt4truthALL_N_BAD_CELLS;
   vector<float>   *jet_antikt4truthALL_N_BAD_CELLS_CORR;
   vector<float>   *jet_antikt4truthALL_BAD_CELLS_CORR_E;
   vector<float>   *jet_antikt4truthALL_NumTowers;
   vector<int>     *jet_antikt4truthALL_SamplingMax;
   vector<float>   *jet_antikt4truthALL_fracSamplingMax;
   vector<float>   *jet_antikt4truthALL_hecf;
   vector<float>   *jet_antikt4truthALL_tgap3f;
   vector<int>     *jet_antikt4truthALL_isUgly;
   vector<int>     *jet_antikt4truthALL_isBadLoose;
   vector<int>     *jet_antikt4truthALL_isBadMedium;
   vector<int>     *jet_antikt4truthALL_isBadTight;
   vector<float>   *jet_antikt4truthALL_emfrac;
   vector<float>   *jet_antikt4truthALL_Offset;
   vector<float>   *jet_antikt4truthALL_EMJES;
   vector<float>   *jet_antikt4truthALL_EMJES_EtaCorr;
   vector<float>   *jet_antikt4truthALL_EMJESnooffset;
   vector<float>   *jet_antikt4truthALL_GCWJES;
   vector<float>   *jet_antikt4truthALL_GCWJES_EtaCorr;
   vector<float>   *jet_antikt4truthALL_CB;
   vector<float>   *jet_antikt4truthALL_emscale_E;
   vector<float>   *jet_antikt4truthALL_emscale_pt;
   vector<float>   *jet_antikt4truthALL_emscale_m;
   vector<float>   *jet_antikt4truthALL_emscale_eta;
   vector<float>   *jet_antikt4truthALL_emscale_phi;
   vector<float>   *jet_antikt4truthALL_el_dr;
   vector<int>     *jet_antikt4truthALL_el_matched;
   vector<float>   *jet_antikt4truthALL_mu_dr;
   vector<int>     *jet_antikt4truthALL_mu_matched;
   vector<float>   *jet_antikt4truthALL_L1_dr;
   vector<int>     *jet_antikt4truthALL_L1_matched;
   vector<float>   *jet_antikt4truthALL_L2_dr;
   vector<int>     *jet_antikt4truthALL_L2_matched;
   vector<float>   *jet_antikt4truthALL_EF_dr;
   vector<int>     *jet_antikt4truthALL_EF_matched;
   Int_t           jet_antikt6truthALL_n;
   vector<float>   *jet_antikt6truthALL_E;
   vector<float>   *jet_antikt6truthALL_pt;
   vector<float>   *jet_antikt6truthALL_m;
   vector<float>   *jet_antikt6truthALL_eta;
   vector<float>   *jet_antikt6truthALL_phi;
   vector<float>   *jet_antikt6truthALL_EtaOrigin;
   vector<float>   *jet_antikt6truthALL_PhiOrigin;
   vector<float>   *jet_antikt6truthALL_MOrigin;
   vector<float>   *jet_antikt6truthALL_EtaOriginEM;
   vector<float>   *jet_antikt6truthALL_PhiOriginEM;
   vector<float>   *jet_antikt6truthALL_MOriginEM;
   vector<float>   *jet_antikt6truthALL_WIDTH;
   vector<float>   *jet_antikt6truthALL_n90;
   vector<float>   *jet_antikt6truthALL_Timing;
   vector<float>   *jet_antikt6truthALL_LArQuality;
   vector<float>   *jet_antikt6truthALL_nTrk;
   vector<float>   *jet_antikt6truthALL_sumPtTrk;
   vector<float>   *jet_antikt6truthALL_OriginIndex;
   vector<float>   *jet_antikt6truthALL_HECQuality;
   vector<float>   *jet_antikt6truthALL_NegativeE;
   vector<float>   *jet_antikt6truthALL_YFlip12;
   vector<float>   *jet_antikt6truthALL_YFlip23;
   vector<float>   *jet_antikt6truthALL_BCH_CORR_CELL;
   vector<float>   *jet_antikt6truthALL_BCH_CORR_DOTX;
   vector<float>   *jet_antikt6truthALL_BCH_CORR_JET;
   vector<float>   *jet_antikt6truthALL_BCH_CORR_JET_FORCELL;
   vector<float>   *jet_antikt6truthALL_ENG_BAD_CELLS;
   vector<float>   *jet_antikt6truthALL_N_BAD_CELLS;
   vector<float>   *jet_antikt6truthALL_N_BAD_CELLS_CORR;
   vector<float>   *jet_antikt6truthALL_BAD_CELLS_CORR_E;
   vector<float>   *jet_antikt6truthALL_NumTowers;
   vector<int>     *jet_antikt6truthALL_SamplingMax;
   vector<float>   *jet_antikt6truthALL_fracSamplingMax;
   vector<float>   *jet_antikt6truthALL_hecf;
   vector<float>   *jet_antikt6truthALL_tgap3f;
   vector<int>     *jet_antikt6truthALL_isUgly;
   vector<int>     *jet_antikt6truthALL_isBadLoose;
   vector<int>     *jet_antikt6truthALL_isBadMedium;
   vector<int>     *jet_antikt6truthALL_isBadTight;
   vector<float>   *jet_antikt6truthALL_emfrac;
   vector<float>   *jet_antikt6truthALL_Offset;
   vector<float>   *jet_antikt6truthALL_EMJES;
   vector<float>   *jet_antikt6truthALL_EMJES_EtaCorr;
   vector<float>   *jet_antikt6truthALL_EMJESnooffset;
   vector<float>   *jet_antikt6truthALL_GCWJES;
   vector<float>   *jet_antikt6truthALL_GCWJES_EtaCorr;
   vector<float>   *jet_antikt6truthALL_CB;
   vector<float>   *jet_antikt6truthALL_emscale_E;
   vector<float>   *jet_antikt6truthALL_emscale_pt;
   vector<float>   *jet_antikt6truthALL_emscale_m;
   vector<float>   *jet_antikt6truthALL_emscale_eta;
   vector<float>   *jet_antikt6truthALL_emscale_phi;
   vector<float>   *jet_antikt6truthALL_el_dr;
   vector<int>     *jet_antikt6truthALL_el_matched;
   vector<float>   *jet_antikt6truthALL_mu_dr;
   vector<int>     *jet_antikt6truthALL_mu_matched;
   vector<float>   *jet_antikt6truthALL_L1_dr;
   vector<int>     *jet_antikt6truthALL_L1_matched;
   vector<float>   *jet_antikt6truthALL_L2_dr;
   vector<int>     *jet_antikt6truthALL_L2_matched;
   vector<float>   *jet_antikt6truthALL_EF_dr;
   vector<int>     *jet_antikt6truthALL_EF_matched;
   Int_t           jet_antikt4truthWZ_n;
   vector<float>   *jet_antikt4truthWZ_E;
   vector<float>   *jet_antikt4truthWZ_pt;
   vector<float>   *jet_antikt4truthWZ_m;
   vector<float>   *jet_antikt4truthWZ_eta;
   vector<float>   *jet_antikt4truthWZ_phi;
   vector<float>   *jet_antikt4truthWZ_EtaOrigin;
   vector<float>   *jet_antikt4truthWZ_PhiOrigin;
   vector<float>   *jet_antikt4truthWZ_MOrigin;
   vector<float>   *jet_antikt4truthWZ_EtaOriginEM;
   vector<float>   *jet_antikt4truthWZ_PhiOriginEM;
   vector<float>   *jet_antikt4truthWZ_MOriginEM;
   vector<float>   *jet_antikt4truthWZ_WIDTH;
   vector<float>   *jet_antikt4truthWZ_n90;
   vector<float>   *jet_antikt4truthWZ_Timing;
   vector<float>   *jet_antikt4truthWZ_LArQuality;
   vector<float>   *jet_antikt4truthWZ_nTrk;
   vector<float>   *jet_antikt4truthWZ_sumPtTrk;
   vector<float>   *jet_antikt4truthWZ_OriginIndex;
   vector<float>   *jet_antikt4truthWZ_HECQuality;
   vector<float>   *jet_antikt4truthWZ_NegativeE;
   vector<float>   *jet_antikt4truthWZ_YFlip12;
   vector<float>   *jet_antikt4truthWZ_YFlip23;
   vector<float>   *jet_antikt4truthWZ_BCH_CORR_CELL;
   vector<float>   *jet_antikt4truthWZ_BCH_CORR_DOTX;
   vector<float>   *jet_antikt4truthWZ_BCH_CORR_JET;
   vector<float>   *jet_antikt4truthWZ_BCH_CORR_JET_FORCELL;
   vector<float>   *jet_antikt4truthWZ_ENG_BAD_CELLS;
   vector<float>   *jet_antikt4truthWZ_N_BAD_CELLS;
   vector<float>   *jet_antikt4truthWZ_N_BAD_CELLS_CORR;
   vector<float>   *jet_antikt4truthWZ_BAD_CELLS_CORR_E;
   vector<float>   *jet_antikt4truthWZ_NumTowers;
   vector<int>     *jet_antikt4truthWZ_SamplingMax;
   vector<float>   *jet_antikt4truthWZ_fracSamplingMax;
   vector<float>   *jet_antikt4truthWZ_hecf;
   vector<float>   *jet_antikt4truthWZ_tgap3f;
   vector<int>     *jet_antikt4truthWZ_isUgly;
   vector<int>     *jet_antikt4truthWZ_isBadLoose;
   vector<int>     *jet_antikt4truthWZ_isBadMedium;
   vector<int>     *jet_antikt4truthWZ_isBadTight;
   vector<float>   *jet_antikt4truthWZ_emfrac;
   vector<float>   *jet_antikt4truthWZ_Offset;
   vector<float>   *jet_antikt4truthWZ_EMJES;
   vector<float>   *jet_antikt4truthWZ_EMJES_EtaCorr;
   vector<float>   *jet_antikt4truthWZ_EMJESnooffset;
   vector<float>   *jet_antikt4truthWZ_GCWJES;
   vector<float>   *jet_antikt4truthWZ_GCWJES_EtaCorr;
   vector<float>   *jet_antikt4truthWZ_CB;
   vector<float>   *jet_antikt4truthWZ_emscale_E;
   vector<float>   *jet_antikt4truthWZ_emscale_pt;
   vector<float>   *jet_antikt4truthWZ_emscale_m;
   vector<float>   *jet_antikt4truthWZ_emscale_eta;
   vector<float>   *jet_antikt4truthWZ_emscale_phi;
   vector<float>   *jet_antikt4truthWZ_el_dr;
   vector<int>     *jet_antikt4truthWZ_el_matched;
   vector<float>   *jet_antikt4truthWZ_mu_dr;
   vector<int>     *jet_antikt4truthWZ_mu_matched;
   vector<float>   *jet_antikt4truthWZ_L1_dr;
   vector<int>     *jet_antikt4truthWZ_L1_matched;
   vector<float>   *jet_antikt4truthWZ_L2_dr;
   vector<int>     *jet_antikt4truthWZ_L2_matched;
   vector<float>   *jet_antikt4truthWZ_EF_dr;
   vector<int>     *jet_antikt4truthWZ_EF_matched;
   Int_t           jet_antikt6truthWZ_n;
   vector<float>   *jet_antikt6truthWZ_E;
   vector<float>   *jet_antikt6truthWZ_pt;
   vector<float>   *jet_antikt6truthWZ_m;
   vector<float>   *jet_antikt6truthWZ_eta;
   vector<float>   *jet_antikt6truthWZ_phi;
   vector<float>   *jet_antikt6truthWZ_EtaOrigin;
   vector<float>   *jet_antikt6truthWZ_PhiOrigin;
   vector<float>   *jet_antikt6truthWZ_MOrigin;
   vector<float>   *jet_antikt6truthWZ_EtaOriginEM;
   vector<float>   *jet_antikt6truthWZ_PhiOriginEM;
   vector<float>   *jet_antikt6truthWZ_MOriginEM;
   vector<float>   *jet_antikt6truthWZ_WIDTH;
   vector<float>   *jet_antikt6truthWZ_n90;
   vector<float>   *jet_antikt6truthWZ_Timing;
   vector<float>   *jet_antikt6truthWZ_LArQuality;
   vector<float>   *jet_antikt6truthWZ_nTrk;
   vector<float>   *jet_antikt6truthWZ_sumPtTrk;
   vector<float>   *jet_antikt6truthWZ_OriginIndex;
   vector<float>   *jet_antikt6truthWZ_HECQuality;
   vector<float>   *jet_antikt6truthWZ_NegativeE;
   vector<float>   *jet_antikt6truthWZ_YFlip12;
   vector<float>   *jet_antikt6truthWZ_YFlip23;
   vector<float>   *jet_antikt6truthWZ_BCH_CORR_CELL;
   vector<float>   *jet_antikt6truthWZ_BCH_CORR_DOTX;
   vector<float>   *jet_antikt6truthWZ_BCH_CORR_JET;
   vector<float>   *jet_antikt6truthWZ_BCH_CORR_JET_FORCELL;
   vector<float>   *jet_antikt6truthWZ_ENG_BAD_CELLS;
   vector<float>   *jet_antikt6truthWZ_N_BAD_CELLS;
   vector<float>   *jet_antikt6truthWZ_N_BAD_CELLS_CORR;
   vector<float>   *jet_antikt6truthWZ_BAD_CELLS_CORR_E;
   vector<float>   *jet_antikt6truthWZ_NumTowers;
   vector<int>     *jet_antikt6truthWZ_SamplingMax;
   vector<float>   *jet_antikt6truthWZ_fracSamplingMax;
   vector<float>   *jet_antikt6truthWZ_hecf;
   vector<float>   *jet_antikt6truthWZ_tgap3f;
   vector<int>     *jet_antikt6truthWZ_isUgly;
   vector<int>     *jet_antikt6truthWZ_isBadLoose;
   vector<int>     *jet_antikt6truthWZ_isBadMedium;
   vector<int>     *jet_antikt6truthWZ_isBadTight;
   vector<float>   *jet_antikt6truthWZ_emfrac;
   vector<float>   *jet_antikt6truthWZ_Offset;
   vector<float>   *jet_antikt6truthWZ_EMJES;
   vector<float>   *jet_antikt6truthWZ_EMJES_EtaCorr;
   vector<float>   *jet_antikt6truthWZ_EMJESnooffset;
   vector<float>   *jet_antikt6truthWZ_GCWJES;
   vector<float>   *jet_antikt6truthWZ_GCWJES_EtaCorr;
   vector<float>   *jet_antikt6truthWZ_CB;
   vector<float>   *jet_antikt6truthWZ_emscale_E;
   vector<float>   *jet_antikt6truthWZ_emscale_pt;
   vector<float>   *jet_antikt6truthWZ_emscale_m;
   vector<float>   *jet_antikt6truthWZ_emscale_eta;
   vector<float>   *jet_antikt6truthWZ_emscale_phi;
   vector<float>   *jet_antikt6truthWZ_el_dr;
   vector<int>     *jet_antikt6truthWZ_el_matched;
   vector<float>   *jet_antikt6truthWZ_mu_dr;
   vector<int>     *jet_antikt6truthWZ_mu_matched;
   vector<float>   *jet_antikt6truthWZ_L1_dr;
   vector<int>     *jet_antikt6truthWZ_L1_matched;
   vector<float>   *jet_antikt6truthWZ_L2_dr;
   vector<int>     *jet_antikt6truthWZ_L2_matched;
   vector<float>   *jet_antikt6truthWZ_EF_dr;
   vector<int>     *jet_antikt6truthWZ_EF_matched;
   vector<unsigned int> *trig_L1_TAV;
   vector<short>   *trig_L2_passedPhysics;
   vector<short>   *trig_EF_passedPhysics;
   vector<unsigned int> *trig_L1_TBP;
   vector<unsigned int> *trig_L1_TAP;
   vector<short>   *trig_L2_passedRaw;
   vector<short>   *trig_EF_passedRaw;
   Bool_t          trig_L2_truncated;
   Bool_t          trig_EF_truncated;
   vector<short>   *trig_L2_resurrected;
   vector<short>   *trig_EF_resurrected;
   vector<short>   *trig_L2_passedThrough;
   vector<short>   *trig_EF_passedThrough;
   UInt_t          trig_DB_SMK;
   UInt_t          trig_DB_L1PSK;
   UInt_t          trig_DB_HLTPSK;
   Char_t          trig_bgCode;
   Int_t           trig_L1_emtau_n;
   vector<float>   *trig_L1_emtau_eta;
   vector<float>   *trig_L1_emtau_phi;
   vector<vector<string> > *trig_L1_emtau_thrNames;
   vector<vector<float> > *trig_L1_emtau_thrValues;
   Int_t           trig_L2_emcl_n;
   vector<unsigned int> *trig_L2_emcl_quality;
   vector<float>   *trig_L2_emcl_E;
   vector<float>   *trig_L2_emcl_Et;
   vector<float>   *trig_L2_emcl_eta;
   vector<float>   *trig_L2_emcl_phi;
   Int_t           trig_L2_trk_idscan_eGamma_n;
   vector<int>     *trig_L2_trk_idscan_eGamma_algorithmId;
   vector<short>   *trig_L2_trk_idscan_eGamma_trackStatus;
   vector<float>   *trig_L2_trk_idscan_eGamma_chi2Ndof;
   vector<float>   *trig_L2_trk_idscan_eGamma_param_a0;
   vector<float>   *trig_L2_trk_idscan_eGamma_param_z0;
   vector<float>   *trig_L2_trk_idscan_eGamma_param_phi0;
   vector<float>   *trig_L2_trk_idscan_eGamma_param_eta;
   vector<float>   *trig_L2_trk_idscan_eGamma_param_pt;
   Int_t           trig_L2_trk_sitrack_eGamma_n;
   vector<int>     *trig_L2_trk_sitrack_eGamma_algorithmId;
   vector<short>   *trig_L2_trk_sitrack_eGamma_trackStatus;
   vector<float>   *trig_L2_trk_sitrack_eGamma_chi2Ndof;
   vector<float>   *trig_L2_trk_sitrack_eGamma_param_a0;
   vector<float>   *trig_L2_trk_sitrack_eGamma_param_z0;
   vector<float>   *trig_L2_trk_sitrack_eGamma_param_phi0;
   vector<float>   *trig_L2_trk_sitrack_eGamma_param_eta;
   vector<float>   *trig_L2_trk_sitrack_eGamma_param_pt;
   Int_t           trig_L2_el_n;
   vector<float>   *trig_L2_el_E;
   vector<float>   *trig_L2_el_Et;
   vector<float>   *trig_L2_el_pt;
   vector<float>   *trig_L2_el_eta;
   vector<float>   *trig_L2_el_phi;
   vector<int>     *trig_L2_el_RoIWord;
   vector<float>   *trig_L2_el_zvertex;
   vector<int>     *trig_L2_el_charge;
   Int_t           trig_L2_ph_n;
   vector<float>   *trig_L2_ph_E;
   vector<float>   *trig_L2_ph_Et;
   vector<float>   *trig_L2_ph_pt;
   vector<float>   *trig_L2_ph_eta;
   vector<float>   *trig_L2_ph_phi;
   vector<int>     *trig_L2_ph_RoIWord;
   Int_t           trig_EF_emcl_n;
   vector<float>   *trig_EF_emcl_pt;
   vector<float>   *trig_EF_emcl_eta;
   vector<float>   *trig_EF_emcl_phi;
   vector<float>   *trig_EF_emcl_E_em;
   vector<float>   *trig_EF_emcl_E_had;
   Int_t           trig_EF_emcl_slw_n;
   vector<float>   *trig_EF_emcl_slw_pt;
   vector<float>   *trig_EF_emcl_slw_eta;
   vector<float>   *trig_EF_emcl_slw_phi;
   vector<float>   *trig_EF_emcl_slw_E_em;
   vector<float>   *trig_EF_emcl_slw_E_had;
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
   vector<float>   *trig_EF_el_vertweight;
   vector<int>     *trig_EF_el_hastrack;
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
   vector<int>     *trig_EF_ph_author;
   vector<int>     *trig_EF_ph_isRecovered;
   vector<unsigned int> *trig_EF_ph_isEM;
   vector<int>     *trig_EF_ph_convFlag;
   vector<int>     *trig_EF_ph_isConv;
   vector<int>     *trig_EF_ph_nConv;
   vector<int>     *trig_EF_ph_nSingleTrackConv;
   vector<int>     *trig_EF_ph_nDoubleTrackConv;
   vector<int>     *trig_EF_ph_loose;
   vector<int>     *trig_EF_ph_tight;
   vector<int>     *trig_EF_ph_tightIso;
   Int_t           trig_Nav_n;
   vector<short>   *trig_Nav_chain_ChainId;
   vector<vector<int> > *trig_Nav_chain_RoIType;
   vector<vector<int> > *trig_Nav_chain_RoIIndex;
   Int_t           trig_RoI_L2_e_n;
   Int_t           trig_RoI_EF_e_n;
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
   vector<float>   *trig_L2_isomuonfeature_EtInnerConeEC;
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
   vector<float>   *trig_L2_sitrack_muon_param_a0;
   vector<float>   *trig_L2_sitrack_muon_param_z0;
   vector<float>   *trig_L2_sitrack_muon_param_phi0;
   vector<float>   *trig_L2_sitrack_muon_param_eta;
   vector<float>   *trig_L2_sitrack_muon_param_pt;
   Int_t           trig_L2_idscan_muon_n;
   vector<int>     *trig_L2_idscan_muon_algorithmId;
   vector<short>   *trig_L2_idscan_muon_trackStatus;
   vector<float>   *trig_L2_idscan_muon_chi2Ndof;
   vector<float>   *trig_L2_idscan_muon_param_a0;
   vector<float>   *trig_L2_idscan_muon_param_z0;
   vector<float>   *trig_L2_idscan_muon_param_phi0;
   vector<float>   *trig_L2_idscan_muon_param_eta;
   vector<float>   *trig_L2_idscan_muon_param_pt;
   Int_t           trig_L2_sitrack_isomuon_n;
   vector<int>     *trig_L2_sitrack_isomuon_algorithmId;
   vector<short>   *trig_L2_sitrack_isomuon_trackStatus;
   vector<float>   *trig_L2_sitrack_isomuon_chi2Ndof;
   vector<float>   *trig_L2_sitrack_isomuon_param_a0;
   vector<float>   *trig_L2_sitrack_isomuon_param_z0;
   vector<float>   *trig_L2_sitrack_isomuon_param_phi0;
   vector<float>   *trig_L2_sitrack_isomuon_param_eta;
   vector<float>   *trig_L2_sitrack_isomuon_param_pt;
   Int_t           trig_L2_idscan_isomuon_n;
   vector<int>     *trig_L2_idscan_isomuon_algorithmId;
   vector<short>   *trig_L2_idscan_isomuon_trackStatus;
   vector<float>   *trig_L2_idscan_isomuon_chi2Ndof;
   vector<float>   *trig_L2_idscan_isomuon_param_a0;
   vector<float>   *trig_L2_idscan_isomuon_param_z0;
   vector<float>   *trig_L2_idscan_isomuon_param_phi0;
   vector<float>   *trig_L2_idscan_isomuon_param_eta;
   vector<float>   *trig_L2_idscan_isomuon_param_pt;
   Int_t           trig_roidescriptor_forID_n;
   vector<float>   *trig_roidescriptor_forID_phi;
   vector<float>   *trig_roidescriptor_forID_eta;
   vector<float>   *trig_roidescriptor_forID_zed0;
   Int_t           trig_RoI_L2_mu_n;
   vector<short>   *trig_RoI_L2_mu_type;
   vector<short>   *trig_RoI_L2_mu_lastStep;
   vector<int>     *trig_RoI_L2_mu_TrigRoiDescriptor_forID;
   vector<int>     *trig_RoI_L2_mu_TrigRoiDescriptor_forIDStatus;
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
   vector<string>  *trig_L1_esum_thrNames;
   Float_t         trig_L1_esum_ExMiss;
   Float_t         trig_L1_esum_EyMiss;
   Float_t         trig_L1_esum_energyT;
   Bool_t          trig_L1_esum_overflowX;
   Bool_t          trig_L1_esum_overflowY;
   Bool_t          trig_L1_esum_overflowT;
   UInt_t          trig_L1_esum_RoIWord0;
   UInt_t          trig_L1_esum_RoIWord1;
   UInt_t          trig_L1_esum_RoIWord2;
   Float_t         trig_L2_met_MEx;
   Float_t         trig_L2_met_MEy;
   Float_t         trig_L2_met_MEz;
   Float_t         trig_L2_met_sumEt;
   Float_t         trig_L2_met_sumE;
   Int_t           trig_L2_met_flag;
   vector<string>  *trig_L2_met_nameOfComponent;
   vector<float>   *trig_L2_met_MExComponent;
   vector<float>   *trig_L2_met_MEyComponent;
   vector<float>   *trig_L2_met_MEzComponent;
   vector<float>   *trig_L2_met_sumEtComponent;
   vector<float>   *trig_L2_met_sumEComponent;
   vector<float>   *trig_L2_met_componentCalib0;
   vector<float>   *trig_L2_met_componentCalib1;
   vector<short>   *trig_L2_met_sumOfSigns;
   vector<unsigned short> *trig_L2_met_usedChannels;
   vector<short>   *trig_L2_met_status;
   Float_t         trig_EF_met_MEx;
   Float_t         trig_EF_met_MEy;
   Float_t         trig_EF_met_MEz;
   Float_t         trig_EF_met_sumEt;
   Float_t         trig_EF_met_sumE;
   Int_t           trig_EF_met_flag;
   vector<string>  *trig_EF_met_nameOfComponent;
   vector<float>   *trig_EF_met_MExComponent;
   vector<float>   *trig_EF_met_MEyComponent;
   vector<float>   *trig_EF_met_MEzComponent;
   vector<float>   *trig_EF_met_sumEtComponent;
   vector<float>   *trig_EF_met_sumEComponent;
   vector<float>   *trig_EF_met_componentCalib0;
   vector<float>   *trig_EF_met_componentCalib1;
   vector<short>   *trig_EF_met_sumOfSigns;
   vector<unsigned short> *trig_EF_met_usedChannels;
   vector<short>   *trig_EF_met_status;

   // List of branches
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
   TBranch        *b_lar_ncellA;   //!
   TBranch        *b_lar_ncellC;   //!
   TBranch        *b_lar_energyA;   //!
   TBranch        *b_lar_energyC;   //!
   TBranch        *b_lar_timeA;   //!
   TBranch        *b_lar_timeC;   //!
   TBranch        *b_lar_timeDiff;   //!
   TBranch        *b_er_n;   //!
   TBranch        *b_er_rings;   //!
   TBranch        *b_er_nt_n;   //!
   TBranch        *b_er_nt_rings;   //!
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
   TBranch        *b_ph_OQ;   //!
   TBranch        *b_ph_OQRecalc;   //!
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
   TBranch        *b_ph_truth_index;   //!
   TBranch        *b_ph_truth_matched;   //!
   TBranch        *b_ph_loose;   //!
   TBranch        *b_ph_tight;   //!
   TBranch        *b_ph_tightIso;   //!
   TBranch        *b_ph_goodOQ;   //!
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
   TBranch        *b_ph_Etcone15;   //!
   TBranch        *b_ph_Etcone20;   //!
   TBranch        *b_ph_Etcone25;   //!
   TBranch        *b_ph_Etcone30;   //!
   TBranch        *b_ph_Etcone35;   //!
   TBranch        *b_ph_Etcone40;   //!
   TBranch        *b_ph_ptcone20;   //!
   TBranch        *b_ph_ptcone30;   //!
   TBranch        *b_ph_ptcone40;   //!
   TBranch        *b_ph_nucone20;   //!
   TBranch        *b_ph_nucone30;   //!
   TBranch        *b_ph_nucone40;   //!
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
   TBranch        *b_ph_Etcone35_pt_corrected;   //!
   TBranch        *b_ph_Etcone30_pt_corrected;   //!
   TBranch        *b_ph_Etcone25_pt_corrected;   //!
   TBranch        *b_ph_Etcone20_pt_corrected;   //!
   TBranch        *b_ph_Etcone15_pt_corrected;   //!
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
   TBranch        *b_EF_2g10_loose;   //!
   TBranch        *b_EF_2g15_loose;   //!
   TBranch        *b_EF_2g20_loose;   //!
   TBranch        *b_EF_2g5_loose;   //!
   TBranch        *b_EF_2g7_loose;   //!
   TBranch        *b_EF_2j35_jetNoEF_xe20_noMu;   //!
   TBranch        *b_EF_2j35_jetNoEF_xe30_noMu;   //!
   TBranch        *b_EF_2j35_jetNoEF_xe40_noMu;   //!
   TBranch        *b_EF_2j35_xe20_noMu;   //!
   TBranch        *b_EF_2j35_xe30_noMu;   //!
   TBranch        *b_EF_2j35_xe40_noMu;   //!
   TBranch        *b_EF_2mu0_NoAlg;   //!
   TBranch        *b_EF_2mu10;   //!
   TBranch        *b_EF_2mu10_NoAlg;   //!
   TBranch        *b_EF_2mu4;   //!
   TBranch        *b_EF_2mu4_Bmumu;   //!
   TBranch        *b_EF_2mu4_Bmumux;   //!
   TBranch        *b_EF_2mu4_DiMu;   //!
   TBranch        *b_EF_2mu4_DiMu_SiTrk;   //!
   TBranch        *b_EF_2mu4_DiMu_noVtx;   //!
   TBranch        *b_EF_2mu4_DiMu_noVtx_noOS;   //!
   TBranch        *b_EF_2mu4_Jpsimumu;   //!
   TBranch        *b_EF_2mu4_Jpsimumu_IDTrkNoCut;   //!
   TBranch        *b_EF_2mu4_Upsimumu;   //!
   TBranch        *b_EF_2mu6;   //!
   TBranch        *b_EF_2mu6_Bmumu;   //!
   TBranch        *b_EF_2mu6_Bmumux;   //!
   TBranch        *b_EF_2mu6_DiMu;   //!
   TBranch        *b_EF_2mu6_Jpsimumu;   //!
   TBranch        *b_EF_2mu6_MG;   //!
   TBranch        *b_EF_2mu6_NoAlg;   //!
   TBranch        *b_EF_2mu6_Upsimumu;   //!
   TBranch        *b_EF_2mu6_g10_loose;   //!
   TBranch        *b_EF_e20_loose_xe20;   //!
   TBranch        *b_EF_e20_loose_xe20_noMu;   //!
   TBranch        *b_EF_e20_loose_xe30;   //!
   TBranch        *b_EF_e20_loose_xe30_noMu;   //!
   TBranch        *b_EF_g10_loose;   //!
   TBranch        *b_EF_g10_loose_larcalib;   //!
   TBranch        *b_EF_g11_etcut;   //!
   TBranch        *b_EF_g15_loose;   //!
   TBranch        *b_EF_g17_etcut;   //!
   TBranch        *b_EF_g17_etcut_EFxe20_noMu;   //!
   TBranch        *b_EF_g17_etcut_EFxe30_noMu;   //!
   TBranch        *b_EF_g20_loose;   //!
   TBranch        *b_EF_g20_loose_cnv;   //!
   TBranch        *b_EF_g20_loose_larcalib;   //!
   TBranch        *b_EF_g20_loose_xe20_noMu;   //!
   TBranch        *b_EF_g20_loose_xe30_noMu;   //!
   TBranch        *b_EF_g20_tight;   //!
   TBranch        *b_EF_g25_loose_xe30_noMu;   //!
   TBranch        *b_EF_g30_loose;   //!
   TBranch        *b_EF_g30_tight;   //!
   TBranch        *b_EF_g3_NoCut_unpaired_iso;   //!
   TBranch        *b_EF_g3_NoCut_unpaired_noniso;   //!
   TBranch        *b_EF_g40_loose;   //!
   TBranch        *b_EF_g40_loose_larcalib;   //!
   TBranch        *b_EF_g40_tight;   //!
   TBranch        *b_EF_g50_loose;   //!
   TBranch        *b_EF_g50_loose_larcalib;   //!
   TBranch        *b_EF_g5_NoCut_cosmic;   //!
   TBranch        *b_EF_g5_loose;   //!
   TBranch        *b_EF_g5_loose_cnv;   //!
   TBranch        *b_EF_g5_loose_larcalib;   //!
   TBranch        *b_EF_g7_loose;   //!
   TBranch        *b_EF_j35_jetNoCut_xe30_e15_medium;   //!
   TBranch        *b_EF_j35_jetNoCut_xe30_mu15;   //!
   TBranch        *b_EF_j35_xe30_e15_medium;   //!
   TBranch        *b_EF_j35_xe30_mu15;   //!
   TBranch        *b_EF_j50_jetNoEF_xe20_noMu;   //!
   TBranch        *b_EF_j50_jetNoEF_xe30_noMu;   //!
   TBranch        *b_EF_j50_jetNoEF_xe40_noMu;   //!
   TBranch        *b_EF_j50_xe20_noMu;   //!
   TBranch        *b_EF_j50_xe30_noMu;   //!
   TBranch        *b_EF_j50_xe40_noMu;   //!
   TBranch        *b_EF_mu0_NoAlg;   //!
   TBranch        *b_EF_mu0_comm_NoAlg;   //!
   TBranch        *b_EF_mu0_comm_empty_NoAlg;   //!
   TBranch        *b_EF_mu0_comm_firstempty_NoAlg;   //!
   TBranch        *b_EF_mu0_comm_unpaired_iso_NoAlg;   //!
   TBranch        *b_EF_mu0_comm_unpaired_noniso_NoAlg;   //!
   TBranch        *b_EF_mu0_empty_NoAlg;   //!
   TBranch        *b_EF_mu0_firstempty_NoAlg;   //!
   TBranch        *b_EF_mu0_missingRoi;   //!
   TBranch        *b_EF_mu0_outOfTime1;   //!
   TBranch        *b_EF_mu0_outOfTime2;   //!
   TBranch        *b_EF_mu0_rpcOnly;   //!
   TBranch        *b_EF_mu0_unpaired_iso_NoAlg;   //!
   TBranch        *b_EF_mu0_unpaired_noniso_NoAlg;   //!
   TBranch        *b_EF_mu10;   //!
   TBranch        *b_EF_mu10_Ecut12;   //!
   TBranch        *b_EF_mu10_Ecut123;   //!
   TBranch        *b_EF_mu10_Ecut13;   //!
   TBranch        *b_EF_mu10_IDTrkNoCut;   //!
   TBranch        *b_EF_mu10_MG;   //!
   TBranch        *b_EF_mu10_MSonly;   //!
   TBranch        *b_EF_mu10_MSonly_Ecut12;   //!
   TBranch        *b_EF_mu10_MSonly_Ecut123;   //!
   TBranch        *b_EF_mu10_MSonly_Ecut13;   //!
   TBranch        *b_EF_mu10_MSonly_tight;   //!
   TBranch        *b_EF_mu10_NoAlg;   //!
   TBranch        *b_EF_mu10_SiTrk;   //!
   TBranch        *b_EF_mu10_j30;   //!
   TBranch        *b_EF_mu10_tight;   //!
   TBranch        *b_EF_mu10i_loose;   //!
   TBranch        *b_EF_mu13;   //!
   TBranch        *b_EF_mu13_MG;   //!
   TBranch        *b_EF_mu13_MG_tight;   //!
   TBranch        *b_EF_mu13_tight;   //!
   TBranch        *b_EF_mu15;   //!
   TBranch        *b_EF_mu15_MG;   //!
   TBranch        *b_EF_mu15_NoAlg;   //!
   TBranch        *b_EF_mu20;   //!
   TBranch        *b_EF_mu20_MSonly;   //!
   TBranch        *b_EF_mu20_NoAlg;   //!
   TBranch        *b_EF_mu20_passHLT;   //!
   TBranch        *b_EF_mu20_slow;   //!
   TBranch        *b_EF_mu30_MSonly;   //!
   TBranch        *b_EF_mu4;   //!
   TBranch        *b_EF_mu40_MSonly;   //!
   TBranch        *b_EF_mu4_Bmumu;   //!
   TBranch        *b_EF_mu4_BmumuX;   //!
   TBranch        *b_EF_mu4_DiMu;   //!
   TBranch        *b_EF_mu4_DiMu_FS;   //!
   TBranch        *b_EF_mu4_DiMu_FS_noOS;   //!
   TBranch        *b_EF_mu4_DiMu_MG;   //!
   TBranch        *b_EF_mu4_DiMu_MG_FS;   //!
   TBranch        *b_EF_mu4_DiMu_SiTrk;   //!
   TBranch        *b_EF_mu4_DiMu_SiTrk_FS;   //!
   TBranch        *b_EF_mu4_DiMu_noOS;   //!
   TBranch        *b_EF_mu4_IDTrkNoCut;   //!
   TBranch        *b_EF_mu4_Jpsie5e3;   //!
   TBranch        *b_EF_mu4_Jpsie5e3_FS;   //!
   TBranch        *b_EF_mu4_Jpsie5e3_SiTrk;   //!
   TBranch        *b_EF_mu4_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_EF_mu4_Jpsimumu;   //!
   TBranch        *b_EF_mu4_Jpsimumu_FS;   //!
   TBranch        *b_EF_mu4_Jpsimumu_SiTrk_FS;   //!
   TBranch        *b_EF_mu4_L1J10_matched;   //!
   TBranch        *b_EF_mu4_L1J15_matched;   //!
   TBranch        *b_EF_mu4_L1J30_matched;   //!
   TBranch        *b_EF_mu4_L1J55_matched;   //!
   TBranch        *b_EF_mu4_L1J5_matched;   //!
   TBranch        *b_EF_mu4_L2MSonly_EFFS_passL2;   //!
   TBranch        *b_EF_mu4_MG;   //!
   TBranch        *b_EF_mu4_MSonly;   //!
   TBranch        *b_EF_mu4_MSonly_EFFS_passL2;   //!
   TBranch        *b_EF_mu4_MSonly_MB2_noL2_EFFS;   //!
   TBranch        *b_EF_mu4_MSonly_cosmic;   //!
   TBranch        *b_EF_mu4_MSonly_outOfTime;   //!
   TBranch        *b_EF_mu4_MV;   //!
   TBranch        *b_EF_mu4_SiTrk;   //!
   TBranch        *b_EF_mu4_Trk_Jpsi;   //!
   TBranch        *b_EF_mu4_Trk_Jpsi_FS;   //!
   TBranch        *b_EF_mu4_Trk_Jpsi_loose;   //!
   TBranch        *b_EF_mu4_Trk_Upsi_FS;   //!
   TBranch        *b_EF_mu4_Trk_Upsi_loose_FS;   //!
   TBranch        *b_EF_mu4_Upsimumu_FS;   //!
   TBranch        *b_EF_mu4_Upsimumu_SiTrk_FS;   //!
   TBranch        *b_EF_mu4_comm_MSonly_cosmic;   //!
   TBranch        *b_EF_mu4_comm_cosmic;   //!
   TBranch        *b_EF_mu4_comm_firstempty;   //!
   TBranch        *b_EF_mu4_comm_unpaired_iso;   //!
   TBranch        *b_EF_mu4_cosmic;   //!
   TBranch        *b_EF_mu4_firstempty;   //!
   TBranch        *b_EF_mu4_j20;   //!
   TBranch        *b_EF_mu4_j20_jetNoEF;   //!
   TBranch        *b_EF_mu4_j30;   //!
   TBranch        *b_EF_mu4_j30_jetNoEF;   //!
   TBranch        *b_EF_mu4_mu6;   //!
   TBranch        *b_EF_mu4_muCombTag;   //!
   TBranch        *b_EF_mu4_tile;   //!
   TBranch        *b_EF_mu4_tile_cosmic;   //!
   TBranch        *b_EF_mu4_unpaired_iso;   //!
   TBranch        *b_EF_mu4mu6_Bmumu;   //!
   TBranch        *b_EF_mu4mu6_BmumuX;   //!
   TBranch        *b_EF_mu4mu6_DiMu;   //!
   TBranch        *b_EF_mu4mu6_Jpsimumu;   //!
   TBranch        *b_EF_mu4mu6_Jpsimumu_IDTrkNoCut;   //!
   TBranch        *b_EF_mu4mu6_Upsimumu;   //!
   TBranch        *b_EF_mu6;   //!
   TBranch        *b_EF_mu6_Bmumu;   //!
   TBranch        *b_EF_mu6_BmumuX;   //!
   TBranch        *b_EF_mu6_DiMu;   //!
   TBranch        *b_EF_mu6_Ecut12;   //!
   TBranch        *b_EF_mu6_Ecut123;   //!
   TBranch        *b_EF_mu6_Ecut13;   //!
   TBranch        *b_EF_mu6_Ecut2;   //!
   TBranch        *b_EF_mu6_Ecut3;   //!
   TBranch        *b_EF_mu6_IDTrkNoCut;   //!
   TBranch        *b_EF_mu6_Jpsie5e3;   //!
   TBranch        *b_EF_mu6_Jpsie5e3_FS;   //!
   TBranch        *b_EF_mu6_Jpsie5e3_SiTrk;   //!
   TBranch        *b_EF_mu6_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_EF_mu6_Jpsimumu;   //!
   TBranch        *b_EF_mu6_MG;   //!
   TBranch        *b_EF_mu6_MSonly;   //!
   TBranch        *b_EF_mu6_MSonly_Ecut12;   //!
   TBranch        *b_EF_mu6_MSonly_Ecut123;   //!
   TBranch        *b_EF_mu6_MSonly_Ecut13;   //!
   TBranch        *b_EF_mu6_MSonly_Ecut2;   //!
   TBranch        *b_EF_mu6_MSonly_Ecut3;   //!
   TBranch        *b_EF_mu6_MSonly_outOfTime;   //!
   TBranch        *b_EF_mu6_NoAlg;   //!
   TBranch        *b_EF_mu6_SiTrk;   //!
   TBranch        *b_EF_mu6_Trk_Jpsi;   //!
   TBranch        *b_EF_mu6_Upsimumu_FS;   //!
   TBranch        *b_EF_mu6_muCombTag;   //!
   TBranch        *b_EF_tau12_loose_IdScan_xe15_noMu;   //!
   TBranch        *b_EF_tau12_loose_xe15_noMu;   //!
   TBranch        *b_EF_tau12_loose_xe20_noMu;   //!
   TBranch        *b_EF_tau16_loose_xe20_noMu;   //!
   TBranch        *b_EF_tau16_loose_xe25_noMu;   //!
   TBranch        *b_EF_tau16_loose_xe25_tight_noMu;   //!
   TBranch        *b_EF_tau16_loose_xe30_noMu;   //!
   TBranch        *b_EF_tau16_medium_xe22_noMu;   //!
   TBranch        *b_EF_tau16_medium_xe25_noMu;   //!
   TBranch        *b_EF_tau16_medium_xe25_tight_noMu;   //!
   TBranch        *b_EF_tau20_loose_xe25_noMu;   //!
   TBranch        *b_EF_tauNoCut_hasTrk6_IdScan_xe20_noMu;   //!
   TBranch        *b_EF_tauNoCut_hasTrk6_xe20_noMu;   //!
   TBranch        *b_EF_tauNoCut_hasTrk9_xe20_noMu;   //!
   TBranch        *b_EF_tauNoCut_hasTrk_xe20_noMu;   //!
   TBranch        *b_EF_xe15;   //!
   TBranch        *b_EF_xe15_noMu;   //!
   TBranch        *b_EF_xe15_unbiased_noMu;   //!
   TBranch        *b_EF_xe20;   //!
   TBranch        *b_EF_xe20_noMu;   //!
   TBranch        *b_EF_xe20_tight_noMu;   //!
   TBranch        *b_EF_xe20_tight_vfj_noMu;   //!
   TBranch        *b_EF_xe25;   //!
   TBranch        *b_EF_xe25_medium;   //!
   TBranch        *b_EF_xe25_medium_noMu;   //!
   TBranch        *b_EF_xe25_noMu;   //!
   TBranch        *b_EF_xe25_tight_noMu;   //!
   TBranch        *b_EF_xe25_tight_vfj_noMu;   //!
   TBranch        *b_EF_xe25_vfj_noMu;   //!
   TBranch        *b_EF_xe30;   //!
   TBranch        *b_EF_xe30_allL1;   //!
   TBranch        *b_EF_xe30_allL1_FEB;   //!
   TBranch        *b_EF_xe30_allL1_allCells;   //!
   TBranch        *b_EF_xe30_allL1_noMu;   //!
   TBranch        *b_EF_xe30_loose;   //!
   TBranch        *b_EF_xe30_loose_noMu;   //!
   TBranch        *b_EF_xe30_medium;   //!
   TBranch        *b_EF_xe30_medium_noMu;   //!
   TBranch        *b_EF_xe30_medium_vfj_noMu;   //!
   TBranch        *b_EF_xe30_noMu;   //!
   TBranch        *b_EF_xe30_tight_noMu;   //!
   TBranch        *b_EF_xe30_tight_vfj_noMu;   //!
   TBranch        *b_EF_xe30_vfj_noMu;   //!
   TBranch        *b_EF_xe35;   //!
   TBranch        *b_EF_xe35_loose_noMu;   //!
   TBranch        *b_EF_xe35_noMu;   //!
   TBranch        *b_EF_xe35_tight_noMu;   //!
   TBranch        *b_EF_xe40;   //!
   TBranch        *b_EF_xe40_noMu;   //!
   TBranch        *b_EF_xe40_tight_noMu;   //!
   TBranch        *b_EF_xe45;   //!
   TBranch        *b_EF_xe45_noMu;   //!
   TBranch        *b_EF_xe55;   //!
   TBranch        *b_EF_xe55_noMu;   //!
   TBranch        *b_EF_xe60_medium;   //!
   TBranch        *b_EF_xe60_medium_noMu;   //!
   TBranch        *b_EF_xe80_medium;   //!
   TBranch        *b_EF_xe80_medium_noMu;   //!
   TBranch        *b_L1_2EM10;   //!
   TBranch        *b_L1_2EM14;   //!
   TBranch        *b_L1_2EM2;   //!
   TBranch        *b_L1_2EM3;   //!
   TBranch        *b_L1_2EM5;   //!
   TBranch        *b_L1_2J15_XE10;   //!
   TBranch        *b_L1_2J15_XE15;   //!
   TBranch        *b_L1_2J15_XE25;   //!
   TBranch        *b_L1_2MU0;   //!
   TBranch        *b_L1_2MU0_FIRSTEMPTY;   //!
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
   TBranch        *b_L1_EM2;   //!
   TBranch        *b_L1_EM2_UNPAIRED_ISO;   //!
   TBranch        *b_L1_EM2_UNPAIRED_NONISO;   //!
   TBranch        *b_L1_EM3;   //!
   TBranch        *b_L1_EM3_EMPTY;   //!
   TBranch        *b_L1_EM3_FIRSTEMPTY;   //!
   TBranch        *b_L1_EM3_MV;   //!
   TBranch        *b_L1_EM5;   //!
   TBranch        *b_L1_EM5_MU10;   //!
   TBranch        *b_L1_EM5_MU6;   //!
   TBranch        *b_L1_EM85;   //!
   TBranch        *b_L1_J15_XE15_EM10;   //!
   TBranch        *b_L1_J15_XE15_MU15;   //!
   TBranch        *b_L1_J30_XE10;   //!
   TBranch        *b_L1_J30_XE15;   //!
   TBranch        *b_L1_J30_XE25;   //!
   TBranch        *b_L1_MU0;   //!
   TBranch        *b_L1_MU0_COMM;   //!
   TBranch        *b_L1_MU0_COMM_EMPTY;   //!
   TBranch        *b_L1_MU0_COMM_FIRSTEMPTY;   //!
   TBranch        *b_L1_MU0_COMM_UNPAIRED_ISO;   //!
   TBranch        *b_L1_MU0_COMM_UNPAIRED_NONISO;   //!
   TBranch        *b_L1_MU0_EM3;   //!
   TBranch        *b_L1_MU0_EMPTY;   //!
   TBranch        *b_L1_MU0_FIRSTEMPTY;   //!
   TBranch        *b_L1_MU0_J10;   //!
   TBranch        *b_L1_MU0_J15;   //!
   TBranch        *b_L1_MU0_J30;   //!
   TBranch        *b_L1_MU0_J5;   //!
   TBranch        *b_L1_MU0_J55;   //!
   TBranch        *b_L1_MU0_MV;   //!
   TBranch        *b_L1_MU0_UNPAIRED_ISO;   //!
   TBranch        *b_L1_MU0_UNPAIRED_NONISO;   //!
   TBranch        *b_L1_MU10;   //!
   TBranch        *b_L1_MU10_FIRSTEMPTY;   //!
   TBranch        *b_L1_MU10_J10;   //!
   TBranch        *b_L1_MU15;   //!
   TBranch        *b_L1_MU20;   //!
   TBranch        *b_L1_MU6;   //!
   TBranch        *b_L1_MU6_EM3;   //!
   TBranch        *b_L1_MU6_FIRSTEMPTY;   //!
   TBranch        *b_L1_MU6_J5;   //!
   TBranch        *b_L1_TAU5_XE10;   //!
   TBranch        *b_L1_TAU6_XE10;   //!
   TBranch        *b_L1_XE10;   //!
   TBranch        *b_L1_XE15;   //!
   TBranch        *b_L1_XE20;   //!
   TBranch        *b_L1_XE25;   //!
   TBranch        *b_L1_XE30;   //!
   TBranch        *b_L1_XE35;   //!
   TBranch        *b_L1_XE40;   //!
   TBranch        *b_L1_XE50;   //!
   TBranch        *b_L2_2g10_loose;   //!
   TBranch        *b_L2_2g15_loose;   //!
   TBranch        *b_L2_2g20_loose;   //!
   TBranch        *b_L2_2g5_loose;   //!
   TBranch        *b_L2_2g7_loose;   //!
   TBranch        *b_L2_2j30_xe12_noMu;   //!
   TBranch        *b_L2_2j30_xe20_noMu;   //!
   TBranch        *b_L2_2j30_xe30_noMu;   //!
   TBranch        *b_L2_2j35_jetNoEF_xe20_noMu;   //!
   TBranch        *b_L2_2j35_jetNoEF_xe30_noMu;   //!
   TBranch        *b_L2_2j35_jetNoEF_xe40_noMu;   //!
   TBranch        *b_L2_2mu0_NoAlg;   //!
   TBranch        *b_L2_2mu10;   //!
   TBranch        *b_L2_2mu10_NoAlg;   //!
   TBranch        *b_L2_2mu4;   //!
   TBranch        *b_L2_2mu4_Bmumu;   //!
   TBranch        *b_L2_2mu4_Bmumux;   //!
   TBranch        *b_L2_2mu4_DiMu;   //!
   TBranch        *b_L2_2mu4_DiMu_SiTrk;   //!
   TBranch        *b_L2_2mu4_DiMu_noVtx;   //!
   TBranch        *b_L2_2mu4_DiMu_noVtx_noOS;   //!
   TBranch        *b_L2_2mu4_Jpsimumu;   //!
   TBranch        *b_L2_2mu4_Jpsimumu_IDTrkNoCut;   //!
   TBranch        *b_L2_2mu4_Upsimumu;   //!
   TBranch        *b_L2_2mu6;   //!
   TBranch        *b_L2_2mu6_Bmumu;   //!
   TBranch        *b_L2_2mu6_Bmumux;   //!
   TBranch        *b_L2_2mu6_DiMu;   //!
   TBranch        *b_L2_2mu6_Jpsimumu;   //!
   TBranch        *b_L2_2mu6_MG;   //!
   TBranch        *b_L2_2mu6_NoAlg;   //!
   TBranch        *b_L2_2mu6_Upsimumu;   //!
   TBranch        *b_L2_2mu6_g10_loose;   //!
   TBranch        *b_L2_e20_loose_xe20;   //!
   TBranch        *b_L2_e20_loose_xe20_noMu;   //!
   TBranch        *b_L2_e20_loose_xe30;   //!
   TBranch        *b_L2_e20_loose_xe30_noMu;   //!
   TBranch        *b_L2_g10_loose;   //!
   TBranch        *b_L2_g11_etcut;   //!
   TBranch        *b_L2_g15_loose;   //!
   TBranch        *b_L2_g17_etcut;   //!
   TBranch        *b_L2_g17_etcut_EFxe20_noMu;   //!
   TBranch        *b_L2_g17_etcut_EFxe30_noMu;   //!
   TBranch        *b_L2_g20_loose;   //!
   TBranch        *b_L2_g20_loose_cnv;   //!
   TBranch        *b_L2_g20_loose_xe20_noMu;   //!
   TBranch        *b_L2_g20_loose_xe30_noMu;   //!
   TBranch        *b_L2_g20_tight;   //!
   TBranch        *b_L2_g25_loose_xe30_noMu;   //!
   TBranch        *b_L2_g30_loose;   //!
   TBranch        *b_L2_g30_tight;   //!
   TBranch        *b_L2_g3_NoCut_unpaired_iso;   //!
   TBranch        *b_L2_g3_NoCut_unpaired_noniso;   //!
   TBranch        *b_L2_g40_loose;   //!
   TBranch        *b_L2_g40_tight;   //!
   TBranch        *b_L2_g50_loose;   //!
   TBranch        *b_L2_g5_NoCut_cosmic;   //!
   TBranch        *b_L2_g5_loose;   //!
   TBranch        *b_L2_g5_loose_cnv;   //!
   TBranch        *b_L2_g7_loose;   //!
   TBranch        *b_L2_j30_jetNoCut_xe20_e15_medium;   //!
   TBranch        *b_L2_j30_xe20_e15_medium;   //!
   TBranch        *b_L2_j35_jetNoCut_xe30_mu15;   //!
   TBranch        *b_L2_j35_xe30_mu15;   //!
   TBranch        *b_L2_j45_xe12_noMu;   //!
   TBranch        *b_L2_j45_xe20_noMu;   //!
   TBranch        *b_L2_j45_xe30_noMu;   //!
   TBranch        *b_L2_mu0_NoAlg;   //!
   TBranch        *b_L2_mu0_comm_NoAlg;   //!
   TBranch        *b_L2_mu0_comm_empty_NoAlg;   //!
   TBranch        *b_L2_mu0_comm_firstempty_NoAlg;   //!
   TBranch        *b_L2_mu0_comm_unpaired_iso_NoAlg;   //!
   TBranch        *b_L2_mu0_comm_unpaired_noniso_NoAlg;   //!
   TBranch        *b_L2_mu0_empty_NoAlg;   //!
   TBranch        *b_L2_mu0_firstempty_NoAlg;   //!
   TBranch        *b_L2_mu0_missingRoi;   //!
   TBranch        *b_L2_mu0_outOfTime1;   //!
   TBranch        *b_L2_mu0_outOfTime2;   //!
   TBranch        *b_L2_mu0_rpcOnly;   //!
   TBranch        *b_L2_mu0_unpaired_iso_NoAlg;   //!
   TBranch        *b_L2_mu0_unpaired_noniso_NoAlg;   //!
   TBranch        *b_L2_mu10;   //!
   TBranch        *b_L2_mu10_Ecut12;   //!
   TBranch        *b_L2_mu10_Ecut123;   //!
   TBranch        *b_L2_mu10_Ecut13;   //!
   TBranch        *b_L2_mu10_IDTrkNoCut;   //!
   TBranch        *b_L2_mu10_MG;   //!
   TBranch        *b_L2_mu10_MSonly;   //!
   TBranch        *b_L2_mu10_MSonly_Ecut12;   //!
   TBranch        *b_L2_mu10_MSonly_Ecut123;   //!
   TBranch        *b_L2_mu10_MSonly_Ecut13;   //!
   TBranch        *b_L2_mu10_MSonly_tight;   //!
   TBranch        *b_L2_mu10_NoAlg;   //!
   TBranch        *b_L2_mu10_SiTrk;   //!
   TBranch        *b_L2_mu10_j30;   //!
   TBranch        *b_L2_mu10_tight;   //!
   TBranch        *b_L2_mu10i_loose;   //!
   TBranch        *b_L2_mu13;   //!
   TBranch        *b_L2_mu13_MG;   //!
   TBranch        *b_L2_mu13_MG_tight;   //!
   TBranch        *b_L2_mu13_tight;   //!
   TBranch        *b_L2_mu15;   //!
   TBranch        *b_L2_mu15_MG;   //!
   TBranch        *b_L2_mu15_NoAlg;   //!
   TBranch        *b_L2_mu20;   //!
   TBranch        *b_L2_mu20_MSonly;   //!
   TBranch        *b_L2_mu20_NoAlg;   //!
   TBranch        *b_L2_mu20_passHLT;   //!
   TBranch        *b_L2_mu20_slow;   //!
   TBranch        *b_L2_mu30_MSonly;   //!
   TBranch        *b_L2_mu4;   //!
   TBranch        *b_L2_mu40_MSonly;   //!
   TBranch        *b_L2_mu4_Bmumu;   //!
   TBranch        *b_L2_mu4_BmumuX;   //!
   TBranch        *b_L2_mu4_DiMu;   //!
   TBranch        *b_L2_mu4_DiMu_FS;   //!
   TBranch        *b_L2_mu4_DiMu_FS_noOS;   //!
   TBranch        *b_L2_mu4_DiMu_MG;   //!
   TBranch        *b_L2_mu4_DiMu_MG_FS;   //!
   TBranch        *b_L2_mu4_DiMu_SiTrk;   //!
   TBranch        *b_L2_mu4_DiMu_SiTrk_FS;   //!
   TBranch        *b_L2_mu4_DiMu_noOS;   //!
   TBranch        *b_L2_mu4_IDTrkNoCut;   //!
   TBranch        *b_L2_mu4_Jpsie5e3;   //!
   TBranch        *b_L2_mu4_Jpsie5e3_FS;   //!
   TBranch        *b_L2_mu4_Jpsie5e3_SiTrk;   //!
   TBranch        *b_L2_mu4_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_L2_mu4_Jpsimumu;   //!
   TBranch        *b_L2_mu4_Jpsimumu_FS;   //!
   TBranch        *b_L2_mu4_Jpsimumu_SiTrk_FS;   //!
   TBranch        *b_L2_mu4_L1J10_matched;   //!
   TBranch        *b_L2_mu4_L1J15_matched;   //!
   TBranch        *b_L2_mu4_L1J30_matched;   //!
   TBranch        *b_L2_mu4_L1J55_matched;   //!
   TBranch        *b_L2_mu4_L1J5_matched;   //!
   TBranch        *b_L2_mu4_L2MSonly_EFFS_passL2;   //!
   TBranch        *b_L2_mu4_MG;   //!
   TBranch        *b_L2_mu4_MSonly;   //!
   TBranch        *b_L2_mu4_MSonly_EFFS_passL2;   //!
   TBranch        *b_L2_mu4_MSonly_MB2_noL2_EFFS;   //!
   TBranch        *b_L2_mu4_MSonly_cosmic;   //!
   TBranch        *b_L2_mu4_MSonly_outOfTime;   //!
   TBranch        *b_L2_mu4_MV;   //!
   TBranch        *b_L2_mu4_SiTrk;   //!
   TBranch        *b_L2_mu4_Trk_Jpsi;   //!
   TBranch        *b_L2_mu4_Trk_Jpsi_FS;   //!
   TBranch        *b_L2_mu4_Trk_Jpsi_loose;   //!
   TBranch        *b_L2_mu4_Trk_Upsi_FS;   //!
   TBranch        *b_L2_mu4_Trk_Upsi_loose_FS;   //!
   TBranch        *b_L2_mu4_Upsimumu_FS;   //!
   TBranch        *b_L2_mu4_Upsimumu_SiTrk_FS;   //!
   TBranch        *b_L2_mu4_comm_MSonly_cosmic;   //!
   TBranch        *b_L2_mu4_comm_cosmic;   //!
   TBranch        *b_L2_mu4_comm_firstempty;   //!
   TBranch        *b_L2_mu4_comm_unpaired_iso;   //!
   TBranch        *b_L2_mu4_cosmic;   //!
   TBranch        *b_L2_mu4_firstempty;   //!
   TBranch        *b_L2_mu4_j20;   //!
   TBranch        *b_L2_mu4_j20_jetNoEF;   //!
   TBranch        *b_L2_mu4_j25;   //!
   TBranch        *b_L2_mu4_mu6;   //!
   TBranch        *b_L2_mu4_muCombTag;   //!
   TBranch        *b_L2_mu4_tile;   //!
   TBranch        *b_L2_mu4_tile_cosmic;   //!
   TBranch        *b_L2_mu4_unpaired_iso;   //!
   TBranch        *b_L2_mu4mu6_Bmumu;   //!
   TBranch        *b_L2_mu4mu6_BmumuX;   //!
   TBranch        *b_L2_mu4mu6_DiMu;   //!
   TBranch        *b_L2_mu4mu6_Jpsimumu;   //!
   TBranch        *b_L2_mu4mu6_Jpsimumu_IDTrkNoCut;   //!
   TBranch        *b_L2_mu4mu6_Upsimumu;   //!
   TBranch        *b_L2_mu6;   //!
   TBranch        *b_L2_mu6_Bmumu;   //!
   TBranch        *b_L2_mu6_BmumuX;   //!
   TBranch        *b_L2_mu6_DiMu;   //!
   TBranch        *b_L2_mu6_Ecut12;   //!
   TBranch        *b_L2_mu6_Ecut123;   //!
   TBranch        *b_L2_mu6_Ecut13;   //!
   TBranch        *b_L2_mu6_Ecut2;   //!
   TBranch        *b_L2_mu6_Ecut3;   //!
   TBranch        *b_L2_mu6_IDTrkNoCut;   //!
   TBranch        *b_L2_mu6_Jpsie5e3;   //!
   TBranch        *b_L2_mu6_Jpsie5e3_FS;   //!
   TBranch        *b_L2_mu6_Jpsie5e3_SiTrk;   //!
   TBranch        *b_L2_mu6_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_L2_mu6_Jpsimumu;   //!
   TBranch        *b_L2_mu6_MG;   //!
   TBranch        *b_L2_mu6_MSonly;   //!
   TBranch        *b_L2_mu6_MSonly_Ecut12;   //!
   TBranch        *b_L2_mu6_MSonly_Ecut123;   //!
   TBranch        *b_L2_mu6_MSonly_Ecut13;   //!
   TBranch        *b_L2_mu6_MSonly_Ecut2;   //!
   TBranch        *b_L2_mu6_MSonly_Ecut3;   //!
   TBranch        *b_L2_mu6_MSonly_outOfTime;   //!
   TBranch        *b_L2_mu6_NoAlg;   //!
   TBranch        *b_L2_mu6_SiTrk;   //!
   TBranch        *b_L2_mu6_Trk_Jpsi;   //!
   TBranch        *b_L2_mu6_Upsimumu_FS;   //!
   TBranch        *b_L2_mu6_muCombTag;   //!
   TBranch        *b_L2_tau12_loose_IdScan_xe15_noMu;   //!
   TBranch        *b_L2_tau12_loose_xe15_noMu;   //!
   TBranch        *b_L2_tau12_loose_xe20_noMu;   //!
   TBranch        *b_L2_tau16_loose_xe20_noMu;   //!
   TBranch        *b_L2_tau16_loose_xe25_noMu;   //!
   TBranch        *b_L2_tau16_loose_xe25_tight_noMu;   //!
   TBranch        *b_L2_tau16_loose_xe30_noMu;   //!
   TBranch        *b_L2_tau16_medium_xe22_noMu;   //!
   TBranch        *b_L2_tau16_medium_xe25_noMu;   //!
   TBranch        *b_L2_tau16_medium_xe25_tight_noMu;   //!
   TBranch        *b_L2_tau20_loose_xe25_noMu;   //!
   TBranch        *b_L2_tauNoCut_hasTrk6_IdScan_xe20_noMu;   //!
   TBranch        *b_L2_tauNoCut_hasTrk6_xe20_noMu;   //!
   TBranch        *b_L2_tauNoCut_hasTrk9_xe20_noMu;   //!
   TBranch        *b_L2_tauNoCut_hasTrk_xe20_noMu;   //!
   TBranch        *b_L2_xe12;   //!
   TBranch        *b_L2_xe12_loose;   //!
   TBranch        *b_L2_xe12_loose_noMu;   //!
   TBranch        *b_L2_xe12_medium;   //!
   TBranch        *b_L2_xe12_medium_noMu;   //!
   TBranch        *b_L2_xe12_noMu;   //!
   TBranch        *b_L2_xe15;   //!
   TBranch        *b_L2_xe15_medium;   //!
   TBranch        *b_L2_xe15_medium_noMu;   //!
   TBranch        *b_L2_xe15_medium_vfj_noMu;   //!
   TBranch        *b_L2_xe15_noMu;   //!
   TBranch        *b_L2_xe15_tight_noMu;   //!
   TBranch        *b_L2_xe15_tight_vfj_noMu;   //!
   TBranch        *b_L2_xe15_unbiased_noMu;   //!
   TBranch        *b_L2_xe15_vfj_noMu;   //!
   TBranch        *b_L2_xe17_tight_noMu;   //!
   TBranch        *b_L2_xe17_tight_vfj_noMu;   //!
   TBranch        *b_L2_xe20;   //!
   TBranch        *b_L2_xe20_noMu;   //!
   TBranch        *b_L2_xe20_vfj_noMu;   //!
   TBranch        *b_L2_xe22_loose_noMu;   //!
   TBranch        *b_L2_xe22_tight_noMu;   //!
   TBranch        *b_L2_xe22_tight_vfj_noMu;   //!
   TBranch        *b_L2_xe25;   //!
   TBranch        *b_L2_xe25_noMu;   //!
   TBranch        *b_L2_xe27_tight_noMu;   //!
   TBranch        *b_L2_xe30;   //!
   TBranch        *b_L2_xe30_allL1;   //!
   TBranch        *b_L2_xe30_allL1_FEB;   //!
   TBranch        *b_L2_xe30_allL1_allCells;   //!
   TBranch        *b_L2_xe30_allL1_noMu;   //!
   TBranch        *b_L2_xe30_noMu;   //!
   TBranch        *b_L2_xe32_tight_noMu;   //!
   TBranch        *b_L2_xe35;   //!
   TBranch        *b_L2_xe35_noMu;   //!
   TBranch        *b_L2_xe40_medium;   //!
   TBranch        *b_L2_xe40_medium_noMu;   //!
   TBranch        *b_L2_xe45;   //!
   TBranch        *b_L2_xe45_noMu;   //!
   TBranch        *b_L2_xe60_medium;   //!
   TBranch        *b_L2_xe60_medium_noMu;   //!
   TBranch        *b_mu_muid_n;   //!
   TBranch        *b_mu_muid_E;   //!
   TBranch        *b_mu_muid_pt;   //!
   TBranch        *b_mu_muid_m;   //!
   TBranch        *b_mu_muid_eta;   //!
   TBranch        *b_mu_muid_phi;   //!
   TBranch        *b_mu_muid_px;   //!
   TBranch        *b_mu_muid_py;   //!
   TBranch        *b_mu_muid_pz;   //!
   TBranch        *b_mu_muid_charge;   //!
   TBranch        *b_mu_muid_allauthor;   //!
   TBranch        *b_mu_muid_author;   //!
   TBranch        *b_mu_muid_beta;   //!
   TBranch        *b_mu_muid_isMuonLikelihood;   //!
   TBranch        *b_mu_muid_matchchi2;   //!
   TBranch        *b_mu_muid_matchndof;   //!
   TBranch        *b_mu_muid_etcone20;   //!
   TBranch        *b_mu_muid_etcone30;   //!
   TBranch        *b_mu_muid_etcone40;   //!
   TBranch        *b_mu_muid_nucone20;   //!
   TBranch        *b_mu_muid_nucone30;   //!
   TBranch        *b_mu_muid_nucone40;   //!
   TBranch        *b_mu_muid_ptcone20;   //!
   TBranch        *b_mu_muid_ptcone30;   //!
   TBranch        *b_mu_muid_ptcone40;   //!
   TBranch        *b_mu_muid_energyLossPar;   //!
   TBranch        *b_mu_muid_energyLossErr;   //!
   TBranch        *b_mu_muid_etCore;   //!
   TBranch        *b_mu_muid_energyLossType;   //!
   TBranch        *b_mu_muid_caloMuonIdTag;   //!
   TBranch        *b_mu_muid_caloLRLikelihood;   //!
   TBranch        *b_mu_muid_bestMatch;   //!
   TBranch        *b_mu_muid_isStandAloneMuon;   //!
   TBranch        *b_mu_muid_isCombinedMuon;   //!
   TBranch        *b_mu_muid_isLowPtReconstructedMuon;   //!
   TBranch        *b_mu_muid_loose;   //!
   TBranch        *b_mu_muid_medium;   //!
   TBranch        *b_mu_muid_tight;   //!
   TBranch        *b_mu_muid_d0_exPV;   //!
   TBranch        *b_mu_muid_z0_exPV;   //!
   TBranch        *b_mu_muid_phi_exPV;   //!
   TBranch        *b_mu_muid_theta_exPV;   //!
   TBranch        *b_mu_muid_qoverp_exPV;   //!
   TBranch        *b_mu_muid_cb_d0_exPV;   //!
   TBranch        *b_mu_muid_cb_z0_exPV;   //!
   TBranch        *b_mu_muid_cb_phi_exPV;   //!
   TBranch        *b_mu_muid_cb_theta_exPV;   //!
   TBranch        *b_mu_muid_cb_qoverp_exPV;   //!
   TBranch        *b_mu_muid_id_d0_exPV;   //!
   TBranch        *b_mu_muid_id_z0_exPV;   //!
   TBranch        *b_mu_muid_id_phi_exPV;   //!
   TBranch        *b_mu_muid_id_theta_exPV;   //!
   TBranch        *b_mu_muid_id_qoverp_exPV;   //!
   TBranch        *b_mu_muid_me_d0_exPV;   //!
   TBranch        *b_mu_muid_me_z0_exPV;   //!
   TBranch        *b_mu_muid_me_phi_exPV;   //!
   TBranch        *b_mu_muid_me_theta_exPV;   //!
   TBranch        *b_mu_muid_me_qoverp_exPV;   //!
   TBranch        *b_mu_muid_ie_d0_exPV;   //!
   TBranch        *b_mu_muid_ie_z0_exPV;   //!
   TBranch        *b_mu_muid_ie_phi_exPV;   //!
   TBranch        *b_mu_muid_ie_theta_exPV;   //!
   TBranch        *b_mu_muid_ie_qoverp_exPV;   //!
   TBranch        *b_mu_muid_SpaceTime_detID;   //!
   TBranch        *b_mu_muid_SpaceTime_t;   //!
   TBranch        *b_mu_muid_SpaceTime_tError;   //!
   TBranch        *b_mu_muid_SpaceTime_weight;   //!
   TBranch        *b_mu_muid_SpaceTime_x;   //!
   TBranch        *b_mu_muid_SpaceTime_y;   //!
   TBranch        *b_mu_muid_SpaceTime_z;   //!
   TBranch        *b_mu_muid_SpaceTime_t_Tile;   //!
   TBranch        *b_mu_muid_SpaceTime_tError_Tile;   //!
   TBranch        *b_mu_muid_SpaceTime_weight_Tile;   //!
   TBranch        *b_mu_muid_SpaceTime_x_Tile;   //!
   TBranch        *b_mu_muid_SpaceTime_y_Tile;   //!
   TBranch        *b_mu_muid_SpaceTime_z_Tile;   //!
   TBranch        *b_mu_muid_SpaceTime_t_TRT;   //!
   TBranch        *b_mu_muid_SpaceTime_tError_TRT;   //!
   TBranch        *b_mu_muid_SpaceTime_weight_TRT;   //!
   TBranch        *b_mu_muid_SpaceTime_x_TRT;   //!
   TBranch        *b_mu_muid_SpaceTime_y_TRT;   //!
   TBranch        *b_mu_muid_SpaceTime_z_TRT;   //!
   TBranch        *b_mu_muid_SpaceTime_t_MDT;   //!
   TBranch        *b_mu_muid_SpaceTime_tError_MDT;   //!
   TBranch        *b_mu_muid_SpaceTime_weight_MDT;   //!
   TBranch        *b_mu_muid_SpaceTime_x_MDT;   //!
   TBranch        *b_mu_muid_SpaceTime_y_MDT;   //!
   TBranch        *b_mu_muid_SpaceTime_z_MDT;   //!
   TBranch        *b_mu_muid_TileCellEnergyLayer1;   //!
   TBranch        *b_mu_muid_TileTimeLayer1;   //!
   TBranch        *b_mu_muid_TileCellRmsNoiseLayer1;   //!
   TBranch        *b_mu_muid_TileCellSignLayer1;   //!
   TBranch        *b_mu_muid_TileCellEnergyLayer2;   //!
   TBranch        *b_mu_muid_TileTimeLayer2;   //!
   TBranch        *b_mu_muid_TileCellRmsNoiseLayer2;   //!
   TBranch        *b_mu_muid_TileCellSignLayer2;   //!
   TBranch        *b_mu_muid_TileCellEnergyLayer3;   //!
   TBranch        *b_mu_muid_TileTimeLayer3;   //!
   TBranch        *b_mu_muid_TileCellRmsNoiseLayer3;   //!
   TBranch        *b_mu_muid_TileCellSignLayer3;   //!
   TBranch        *b_mu_muid_MSTrkT0_1;   //!
   TBranch        *b_mu_muid_MSTrkT0_2;   //!
   TBranch        *b_mu_muid_MSTrkT0_3;   //!
   TBranch        *b_mu_muid_cov_d0_exPV;   //!
   TBranch        *b_mu_muid_cov_z0_exPV;   //!
   TBranch        *b_mu_muid_cov_phi_exPV;   //!
   TBranch        *b_mu_muid_cov_theta_exPV;   //!
   TBranch        *b_mu_muid_cov_qoverp_exPV;   //!
   TBranch        *b_mu_muid_cov_d0_z0_exPV;   //!
   TBranch        *b_mu_muid_cov_d0_phi_exPV;   //!
   TBranch        *b_mu_muid_cov_d0_theta_exPV;   //!
   TBranch        *b_mu_muid_cov_d0_qoverp_exPV;   //!
   TBranch        *b_mu_muid_cov_z0_phi_exPV;   //!
   TBranch        *b_mu_muid_cov_z0_theta_exPV;   //!
   TBranch        *b_mu_muid_cov_z0_qoverp_exPV;   //!
   TBranch        *b_mu_muid_cov_phi_theta_exPV;   //!
   TBranch        *b_mu_muid_cov_phi_qoverp_exPV;   //!
   TBranch        *b_mu_muid_cov_theta_qoverp_exPV;   //!
   TBranch        *b_mu_muid_id_cov_d0_exPV;   //!
   TBranch        *b_mu_muid_id_cov_z0_exPV;   //!
   TBranch        *b_mu_muid_id_cov_phi_exPV;   //!
   TBranch        *b_mu_muid_id_cov_theta_exPV;   //!
   TBranch        *b_mu_muid_id_cov_qoverp_exPV;   //!
   TBranch        *b_mu_muid_id_cov_d0_z0_exPV;   //!
   TBranch        *b_mu_muid_id_cov_d0_phi_exPV;   //!
   TBranch        *b_mu_muid_id_cov_d0_theta_exPV;   //!
   TBranch        *b_mu_muid_id_cov_d0_qoverp_exPV;   //!
   TBranch        *b_mu_muid_id_cov_z0_phi_exPV;   //!
   TBranch        *b_mu_muid_id_cov_z0_theta_exPV;   //!
   TBranch        *b_mu_muid_id_cov_z0_qoverp_exPV;   //!
   TBranch        *b_mu_muid_id_cov_phi_theta_exPV;   //!
   TBranch        *b_mu_muid_id_cov_phi_qoverp_exPV;   //!
   TBranch        *b_mu_muid_id_cov_theta_qoverp_exPV;   //!
   TBranch        *b_mu_muid_me_cov_d0_exPV;   //!
   TBranch        *b_mu_muid_me_cov_z0_exPV;   //!
   TBranch        *b_mu_muid_me_cov_phi_exPV;   //!
   TBranch        *b_mu_muid_me_cov_theta_exPV;   //!
   TBranch        *b_mu_muid_me_cov_qoverp_exPV;   //!
   TBranch        *b_mu_muid_me_cov_d0_z0_exPV;   //!
   TBranch        *b_mu_muid_me_cov_d0_phi_exPV;   //!
   TBranch        *b_mu_muid_me_cov_d0_theta_exPV;   //!
   TBranch        *b_mu_muid_me_cov_d0_qoverp_exPV;   //!
   TBranch        *b_mu_muid_me_cov_z0_phi_exPV;   //!
   TBranch        *b_mu_muid_me_cov_z0_theta_exPV;   //!
   TBranch        *b_mu_muid_me_cov_z0_qoverp_exPV;   //!
   TBranch        *b_mu_muid_me_cov_phi_theta_exPV;   //!
   TBranch        *b_mu_muid_me_cov_phi_qoverp_exPV;   //!
   TBranch        *b_mu_muid_me_cov_theta_qoverp_exPV;   //!
   TBranch        *b_mu_muid_ms_d0;   //!
   TBranch        *b_mu_muid_ms_z0;   //!
   TBranch        *b_mu_muid_ms_phi;   //!
   TBranch        *b_mu_muid_ms_theta;   //!
   TBranch        *b_mu_muid_ms_qoverp;   //!
   TBranch        *b_mu_muid_id_d0;   //!
   TBranch        *b_mu_muid_id_z0;   //!
   TBranch        *b_mu_muid_id_phi;   //!
   TBranch        *b_mu_muid_id_theta;   //!
   TBranch        *b_mu_muid_id_qoverp;   //!
   TBranch        *b_mu_muid_me_d0;   //!
   TBranch        *b_mu_muid_me_z0;   //!
   TBranch        *b_mu_muid_me_phi;   //!
   TBranch        *b_mu_muid_me_theta;   //!
   TBranch        *b_mu_muid_me_qoverp;   //!
   TBranch        *b_mu_muid_ie_d0;   //!
   TBranch        *b_mu_muid_ie_z0;   //!
   TBranch        *b_mu_muid_ie_phi;   //!
   TBranch        *b_mu_muid_ie_theta;   //!
   TBranch        *b_mu_muid_ie_qoverp;   //!
   TBranch        *b_mu_muid_nOutliersOnTrack;   //!
   TBranch        *b_mu_muid_nBLHits;   //!
   TBranch        *b_mu_muid_nPixHits;   //!
   TBranch        *b_mu_muid_nSCTHits;   //!
   TBranch        *b_mu_muid_nTRTHits;   //!
   TBranch        *b_mu_muid_nTRTHighTHits;   //!
   TBranch        *b_mu_muid_nBLSharedHits;   //!
   TBranch        *b_mu_muid_nPixSharedHits;   //!
   TBranch        *b_mu_muid_nPixHoles;   //!
   TBranch        *b_mu_muid_nSCTSharedHits;   //!
   TBranch        *b_mu_muid_nSCTHoles;   //!
   TBranch        *b_mu_muid_nTRTOutliers;   //!
   TBranch        *b_mu_muid_nTRTHighTOutliers;   //!
   TBranch        *b_mu_muid_nGangedPixels;   //!
   TBranch        *b_mu_muid_nPixelDeadSensors;   //!
   TBranch        *b_mu_muid_nSCTDeadSensors;   //!
   TBranch        *b_mu_muid_nTRTDeadStraws;   //!
   TBranch        *b_mu_muid_expectBLayerHit;   //!
   TBranch        *b_mu_muid_nMDTHits;   //!
   TBranch        *b_mu_muid_nMDTHoles;   //!
   TBranch        *b_mu_muid_nCSCEtaHits;   //!
   TBranch        *b_mu_muid_nCSCEtaHoles;   //!
   TBranch        *b_mu_muid_nCSCPhiHits;   //!
   TBranch        *b_mu_muid_nCSCPhiHoles;   //!
   TBranch        *b_mu_muid_nRPCEtaHits;   //!
   TBranch        *b_mu_muid_nRPCEtaHoles;   //!
   TBranch        *b_mu_muid_nRPCPhiHits;   //!
   TBranch        *b_mu_muid_nRPCPhiHoles;   //!
   TBranch        *b_mu_muid_nTGCEtaHits;   //!
   TBranch        *b_mu_muid_nTGCEtaHoles;   //!
   TBranch        *b_mu_muid_nTGCPhiHits;   //!
   TBranch        *b_mu_muid_nTGCPhiHoles;   //!
   TBranch        *b_mu_muid_nMDTBIHits;   //!
   TBranch        *b_mu_muid_nMDTBMHits;   //!
   TBranch        *b_mu_muid_nMDTBOHits;   //!
   TBranch        *b_mu_muid_nMDTBEEHits;   //!
   TBranch        *b_mu_muid_nMDTBIS78Hits;   //!
   TBranch        *b_mu_muid_nMDTEIHits;   //!
   TBranch        *b_mu_muid_nMDTEMHits;   //!
   TBranch        *b_mu_muid_nMDTEOHits;   //!
   TBranch        *b_mu_muid_nMDTEEHits;   //!
   TBranch        *b_mu_muid_nRPCLayer1EtaHits;   //!
   TBranch        *b_mu_muid_nRPCLayer2EtaHits;   //!
   TBranch        *b_mu_muid_nRPCLayer3EtaHits;   //!
   TBranch        *b_mu_muid_nRPCLayer1PhiHits;   //!
   TBranch        *b_mu_muid_nRPCLayer2PhiHits;   //!
   TBranch        *b_mu_muid_nRPCLayer3PhiHits;   //!
   TBranch        *b_mu_muid_nTGCLayer1EtaHits;   //!
   TBranch        *b_mu_muid_nTGCLayer2EtaHits;   //!
   TBranch        *b_mu_muid_nTGCLayer3EtaHits;   //!
   TBranch        *b_mu_muid_nTGCLayer4EtaHits;   //!
   TBranch        *b_mu_muid_nTGCLayer1PhiHits;   //!
   TBranch        *b_mu_muid_nTGCLayer2PhiHits;   //!
   TBranch        *b_mu_muid_nTGCLayer3PhiHits;   //!
   TBranch        *b_mu_muid_nTGCLayer4PhiHits;   //!
   TBranch        *b_mu_muid_barrelSectors;   //!
   TBranch        *b_mu_muid_endcapSectors;   //!
   TBranch        *b_mu_muid_trackd0;   //!
   TBranch        *b_mu_muid_trackz0;   //!
   TBranch        *b_mu_muid_trackphi;   //!
   TBranch        *b_mu_muid_tracktheta;   //!
   TBranch        *b_mu_muid_trackqoverp;   //!
   TBranch        *b_mu_muid_trackcov_d0;   //!
   TBranch        *b_mu_muid_trackcov_z0;   //!
   TBranch        *b_mu_muid_trackcov_phi;   //!
   TBranch        *b_mu_muid_trackcov_theta;   //!
   TBranch        *b_mu_muid_trackcov_qoverp;   //!
   TBranch        *b_mu_muid_trackcov_d0_z0;   //!
   TBranch        *b_mu_muid_trackcov_d0_phi;   //!
   TBranch        *b_mu_muid_trackcov_d0_theta;   //!
   TBranch        *b_mu_muid_trackcov_d0_qoverp;   //!
   TBranch        *b_mu_muid_trackcov_z0_phi;   //!
   TBranch        *b_mu_muid_trackcov_z0_theta;   //!
   TBranch        *b_mu_muid_trackcov_z0_qoverp;   //!
   TBranch        *b_mu_muid_trackcov_phi_theta;   //!
   TBranch        *b_mu_muid_trackcov_phi_qoverp;   //!
   TBranch        *b_mu_muid_trackcov_theta_qoverp;   //!
   TBranch        *b_mu_muid_trackfitchi2;   //!
   TBranch        *b_mu_muid_trackfitndof;   //!
   TBranch        *b_mu_muid_hastrack;   //!
   TBranch        *b_mu_muid_truth_dr;   //!
   TBranch        *b_mu_muid_truth_E;   //!
   TBranch        *b_mu_muid_truth_pt;   //!
   TBranch        *b_mu_muid_truth_eta;   //!
   TBranch        *b_mu_muid_truth_phi;   //!
   TBranch        *b_mu_muid_truth_type;   //!
   TBranch        *b_mu_muid_truth_status;   //!
   TBranch        *b_mu_muid_truth_barcode;   //!
   TBranch        *b_mu_muid_truth_mothertype;   //!
   TBranch        *b_mu_muid_truth_motherbarcode;   //!
   TBranch        *b_mu_muid_truth_matched;   //!
   TBranch        *b_mu_muid_EFCB_dr;   //!
   TBranch        *b_mu_muid_EFCB_index;   //!
   TBranch        *b_mu_muid_EFMG_dr;   //!
   TBranch        *b_mu_muid_EFMG_index;   //!
   TBranch        *b_mu_muid_EFME_dr;   //!
   TBranch        *b_mu_muid_EFME_index;   //!
   TBranch        *b_mu_muid_L2CB_dr;   //!
   TBranch        *b_mu_muid_L2CB_index;   //!
   TBranch        *b_mu_muid_L1_dr;   //!
   TBranch        *b_mu_muid_L1_index;   //!
   TBranch        *b_mu_staco_n;   //!
   TBranch        *b_mu_staco_E;   //!
   TBranch        *b_mu_staco_pt;   //!
   TBranch        *b_mu_staco_m;   //!
   TBranch        *b_mu_staco_eta;   //!
   TBranch        *b_mu_staco_phi;   //!
   TBranch        *b_mu_staco_px;   //!
   TBranch        *b_mu_staco_py;   //!
   TBranch        *b_mu_staco_pz;   //!
   TBranch        *b_mu_staco_charge;   //!
   TBranch        *b_mu_staco_allauthor;   //!
   TBranch        *b_mu_staco_author;   //!
   TBranch        *b_mu_staco_beta;   //!
   TBranch        *b_mu_staco_isMuonLikelihood;   //!
   TBranch        *b_mu_staco_matchchi2;   //!
   TBranch        *b_mu_staco_matchndof;   //!
   TBranch        *b_mu_staco_etcone20;   //!
   TBranch        *b_mu_staco_etcone30;   //!
   TBranch        *b_mu_staco_etcone40;   //!
   TBranch        *b_mu_staco_nucone20;   //!
   TBranch        *b_mu_staco_nucone30;   //!
   TBranch        *b_mu_staco_nucone40;   //!
   TBranch        *b_mu_staco_ptcone20;   //!
   TBranch        *b_mu_staco_ptcone30;   //!
   TBranch        *b_mu_staco_ptcone40;   //!
   TBranch        *b_mu_staco_energyLossPar;   //!
   TBranch        *b_mu_staco_energyLossErr;   //!
   TBranch        *b_mu_staco_etCore;   //!
   TBranch        *b_mu_staco_energyLossType;   //!
   TBranch        *b_mu_staco_caloMuonIdTag;   //!
   TBranch        *b_mu_staco_caloLRLikelihood;   //!
   TBranch        *b_mu_staco_bestMatch;   //!
   TBranch        *b_mu_staco_isStandAloneMuon;   //!
   TBranch        *b_mu_staco_isCombinedMuon;   //!
   TBranch        *b_mu_staco_isLowPtReconstructedMuon;   //!
   TBranch        *b_mu_staco_loose;   //!
   TBranch        *b_mu_staco_medium;   //!
   TBranch        *b_mu_staco_tight;   //!
   TBranch        *b_mu_staco_d0_exPV;   //!
   TBranch        *b_mu_staco_z0_exPV;   //!
   TBranch        *b_mu_staco_phi_exPV;   //!
   TBranch        *b_mu_staco_theta_exPV;   //!
   TBranch        *b_mu_staco_qoverp_exPV;   //!
   TBranch        *b_mu_staco_cb_d0_exPV;   //!
   TBranch        *b_mu_staco_cb_z0_exPV;   //!
   TBranch        *b_mu_staco_cb_phi_exPV;   //!
   TBranch        *b_mu_staco_cb_theta_exPV;   //!
   TBranch        *b_mu_staco_cb_qoverp_exPV;   //!
   TBranch        *b_mu_staco_id_d0_exPV;   //!
   TBranch        *b_mu_staco_id_z0_exPV;   //!
   TBranch        *b_mu_staco_id_phi_exPV;   //!
   TBranch        *b_mu_staco_id_theta_exPV;   //!
   TBranch        *b_mu_staco_id_qoverp_exPV;   //!
   TBranch        *b_mu_staco_me_d0_exPV;   //!
   TBranch        *b_mu_staco_me_z0_exPV;   //!
   TBranch        *b_mu_staco_me_phi_exPV;   //!
   TBranch        *b_mu_staco_me_theta_exPV;   //!
   TBranch        *b_mu_staco_me_qoverp_exPV;   //!
   TBranch        *b_mu_staco_ie_d0_exPV;   //!
   TBranch        *b_mu_staco_ie_z0_exPV;   //!
   TBranch        *b_mu_staco_ie_phi_exPV;   //!
   TBranch        *b_mu_staco_ie_theta_exPV;   //!
   TBranch        *b_mu_staco_ie_qoverp_exPV;   //!
   TBranch        *b_mu_staco_SpaceTime_detID;   //!
   TBranch        *b_mu_staco_SpaceTime_t;   //!
   TBranch        *b_mu_staco_SpaceTime_tError;   //!
   TBranch        *b_mu_staco_SpaceTime_weight;   //!
   TBranch        *b_mu_staco_SpaceTime_x;   //!
   TBranch        *b_mu_staco_SpaceTime_y;   //!
   TBranch        *b_mu_staco_SpaceTime_z;   //!
   TBranch        *b_mu_staco_SpaceTime_t_Tile;   //!
   TBranch        *b_mu_staco_SpaceTime_tError_Tile;   //!
   TBranch        *b_mu_staco_SpaceTime_weight_Tile;   //!
   TBranch        *b_mu_staco_SpaceTime_x_Tile;   //!
   TBranch        *b_mu_staco_SpaceTime_y_Tile;   //!
   TBranch        *b_mu_staco_SpaceTime_z_Tile;   //!
   TBranch        *b_mu_staco_SpaceTime_t_TRT;   //!
   TBranch        *b_mu_staco_SpaceTime_tError_TRT;   //!
   TBranch        *b_mu_staco_SpaceTime_weight_TRT;   //!
   TBranch        *b_mu_staco_SpaceTime_x_TRT;   //!
   TBranch        *b_mu_staco_SpaceTime_y_TRT;   //!
   TBranch        *b_mu_staco_SpaceTime_z_TRT;   //!
   TBranch        *b_mu_staco_SpaceTime_t_MDT;   //!
   TBranch        *b_mu_staco_SpaceTime_tError_MDT;   //!
   TBranch        *b_mu_staco_SpaceTime_weight_MDT;   //!
   TBranch        *b_mu_staco_SpaceTime_x_MDT;   //!
   TBranch        *b_mu_staco_SpaceTime_y_MDT;   //!
   TBranch        *b_mu_staco_SpaceTime_z_MDT;   //!
   TBranch        *b_mu_staco_TileCellEnergyLayer1;   //!
   TBranch        *b_mu_staco_TileTimeLayer1;   //!
   TBranch        *b_mu_staco_TileCellRmsNoiseLayer1;   //!
   TBranch        *b_mu_staco_TileCellSignLayer1;   //!
   TBranch        *b_mu_staco_TileCellEnergyLayer2;   //!
   TBranch        *b_mu_staco_TileTimeLayer2;   //!
   TBranch        *b_mu_staco_TileCellRmsNoiseLayer2;   //!
   TBranch        *b_mu_staco_TileCellSignLayer2;   //!
   TBranch        *b_mu_staco_TileCellEnergyLayer3;   //!
   TBranch        *b_mu_staco_TileTimeLayer3;   //!
   TBranch        *b_mu_staco_TileCellRmsNoiseLayer3;   //!
   TBranch        *b_mu_staco_TileCellSignLayer3;   //!
   TBranch        *b_mu_staco_MSTrkT0_1;   //!
   TBranch        *b_mu_staco_MSTrkT0_2;   //!
   TBranch        *b_mu_staco_MSTrkT0_3;   //!
   TBranch        *b_mu_staco_cov_d0_exPV;   //!
   TBranch        *b_mu_staco_cov_z0_exPV;   //!
   TBranch        *b_mu_staco_cov_phi_exPV;   //!
   TBranch        *b_mu_staco_cov_theta_exPV;   //!
   TBranch        *b_mu_staco_cov_qoverp_exPV;   //!
   TBranch        *b_mu_staco_cov_d0_z0_exPV;   //!
   TBranch        *b_mu_staco_cov_d0_phi_exPV;   //!
   TBranch        *b_mu_staco_cov_d0_theta_exPV;   //!
   TBranch        *b_mu_staco_cov_d0_qoverp_exPV;   //!
   TBranch        *b_mu_staco_cov_z0_phi_exPV;   //!
   TBranch        *b_mu_staco_cov_z0_theta_exPV;   //!
   TBranch        *b_mu_staco_cov_z0_qoverp_exPV;   //!
   TBranch        *b_mu_staco_cov_phi_theta_exPV;   //!
   TBranch        *b_mu_staco_cov_phi_qoverp_exPV;   //!
   TBranch        *b_mu_staco_cov_theta_qoverp_exPV;   //!
   TBranch        *b_mu_staco_id_cov_d0_exPV;   //!
   TBranch        *b_mu_staco_id_cov_z0_exPV;   //!
   TBranch        *b_mu_staco_id_cov_phi_exPV;   //!
   TBranch        *b_mu_staco_id_cov_theta_exPV;   //!
   TBranch        *b_mu_staco_id_cov_qoverp_exPV;   //!
   TBranch        *b_mu_staco_id_cov_d0_z0_exPV;   //!
   TBranch        *b_mu_staco_id_cov_d0_phi_exPV;   //!
   TBranch        *b_mu_staco_id_cov_d0_theta_exPV;   //!
   TBranch        *b_mu_staco_id_cov_d0_qoverp_exPV;   //!
   TBranch        *b_mu_staco_id_cov_z0_phi_exPV;   //!
   TBranch        *b_mu_staco_id_cov_z0_theta_exPV;   //!
   TBranch        *b_mu_staco_id_cov_z0_qoverp_exPV;   //!
   TBranch        *b_mu_staco_id_cov_phi_theta_exPV;   //!
   TBranch        *b_mu_staco_id_cov_phi_qoverp_exPV;   //!
   TBranch        *b_mu_staco_id_cov_theta_qoverp_exPV;   //!
   TBranch        *b_mu_staco_me_cov_d0_exPV;   //!
   TBranch        *b_mu_staco_me_cov_z0_exPV;   //!
   TBranch        *b_mu_staco_me_cov_phi_exPV;   //!
   TBranch        *b_mu_staco_me_cov_theta_exPV;   //!
   TBranch        *b_mu_staco_me_cov_qoverp_exPV;   //!
   TBranch        *b_mu_staco_me_cov_d0_z0_exPV;   //!
   TBranch        *b_mu_staco_me_cov_d0_phi_exPV;   //!
   TBranch        *b_mu_staco_me_cov_d0_theta_exPV;   //!
   TBranch        *b_mu_staco_me_cov_d0_qoverp_exPV;   //!
   TBranch        *b_mu_staco_me_cov_z0_phi_exPV;   //!
   TBranch        *b_mu_staco_me_cov_z0_theta_exPV;   //!
   TBranch        *b_mu_staco_me_cov_z0_qoverp_exPV;   //!
   TBranch        *b_mu_staco_me_cov_phi_theta_exPV;   //!
   TBranch        *b_mu_staco_me_cov_phi_qoverp_exPV;   //!
   TBranch        *b_mu_staco_me_cov_theta_qoverp_exPV;   //!
   TBranch        *b_mu_staco_ms_d0;   //!
   TBranch        *b_mu_staco_ms_z0;   //!
   TBranch        *b_mu_staco_ms_phi;   //!
   TBranch        *b_mu_staco_ms_theta;   //!
   TBranch        *b_mu_staco_ms_qoverp;   //!
   TBranch        *b_mu_staco_id_d0;   //!
   TBranch        *b_mu_staco_id_z0;   //!
   TBranch        *b_mu_staco_id_phi;   //!
   TBranch        *b_mu_staco_id_theta;   //!
   TBranch        *b_mu_staco_id_qoverp;   //!
   TBranch        *b_mu_staco_me_d0;   //!
   TBranch        *b_mu_staco_me_z0;   //!
   TBranch        *b_mu_staco_me_phi;   //!
   TBranch        *b_mu_staco_me_theta;   //!
   TBranch        *b_mu_staco_me_qoverp;   //!
   TBranch        *b_mu_staco_ie_d0;   //!
   TBranch        *b_mu_staco_ie_z0;   //!
   TBranch        *b_mu_staco_ie_phi;   //!
   TBranch        *b_mu_staco_ie_theta;   //!
   TBranch        *b_mu_staco_ie_qoverp;   //!
   TBranch        *b_mu_staco_nOutliersOnTrack;   //!
   TBranch        *b_mu_staco_nBLHits;   //!
   TBranch        *b_mu_staco_nPixHits;   //!
   TBranch        *b_mu_staco_nSCTHits;   //!
   TBranch        *b_mu_staco_nTRTHits;   //!
   TBranch        *b_mu_staco_nTRTHighTHits;   //!
   TBranch        *b_mu_staco_nBLSharedHits;   //!
   TBranch        *b_mu_staco_nPixSharedHits;   //!
   TBranch        *b_mu_staco_nPixHoles;   //!
   TBranch        *b_mu_staco_nSCTSharedHits;   //!
   TBranch        *b_mu_staco_nSCTHoles;   //!
   TBranch        *b_mu_staco_nTRTOutliers;   //!
   TBranch        *b_mu_staco_nTRTHighTOutliers;   //!
   TBranch        *b_mu_staco_nGangedPixels;   //!
   TBranch        *b_mu_staco_nPixelDeadSensors;   //!
   TBranch        *b_mu_staco_nSCTDeadSensors;   //!
   TBranch        *b_mu_staco_nTRTDeadStraws;   //!
   TBranch        *b_mu_staco_expectBLayerHit;   //!
   TBranch        *b_mu_staco_nMDTHits;   //!
   TBranch        *b_mu_staco_nMDTHoles;   //!
   TBranch        *b_mu_staco_nCSCEtaHits;   //!
   TBranch        *b_mu_staco_nCSCEtaHoles;   //!
   TBranch        *b_mu_staco_nCSCPhiHits;   //!
   TBranch        *b_mu_staco_nCSCPhiHoles;   //!
   TBranch        *b_mu_staco_nRPCEtaHits;   //!
   TBranch        *b_mu_staco_nRPCEtaHoles;   //!
   TBranch        *b_mu_staco_nRPCPhiHits;   //!
   TBranch        *b_mu_staco_nRPCPhiHoles;   //!
   TBranch        *b_mu_staco_nTGCEtaHits;   //!
   TBranch        *b_mu_staco_nTGCEtaHoles;   //!
   TBranch        *b_mu_staco_nTGCPhiHits;   //!
   TBranch        *b_mu_staco_nTGCPhiHoles;   //!
   TBranch        *b_mu_staco_nMDTBIHits;   //!
   TBranch        *b_mu_staco_nMDTBMHits;   //!
   TBranch        *b_mu_staco_nMDTBOHits;   //!
   TBranch        *b_mu_staco_nMDTBEEHits;   //!
   TBranch        *b_mu_staco_nMDTBIS78Hits;   //!
   TBranch        *b_mu_staco_nMDTEIHits;   //!
   TBranch        *b_mu_staco_nMDTEMHits;   //!
   TBranch        *b_mu_staco_nMDTEOHits;   //!
   TBranch        *b_mu_staco_nMDTEEHits;   //!
   TBranch        *b_mu_staco_nRPCLayer1EtaHits;   //!
   TBranch        *b_mu_staco_nRPCLayer2EtaHits;   //!
   TBranch        *b_mu_staco_nRPCLayer3EtaHits;   //!
   TBranch        *b_mu_staco_nRPCLayer1PhiHits;   //!
   TBranch        *b_mu_staco_nRPCLayer2PhiHits;   //!
   TBranch        *b_mu_staco_nRPCLayer3PhiHits;   //!
   TBranch        *b_mu_staco_nTGCLayer1EtaHits;   //!
   TBranch        *b_mu_staco_nTGCLayer2EtaHits;   //!
   TBranch        *b_mu_staco_nTGCLayer3EtaHits;   //!
   TBranch        *b_mu_staco_nTGCLayer4EtaHits;   //!
   TBranch        *b_mu_staco_nTGCLayer1PhiHits;   //!
   TBranch        *b_mu_staco_nTGCLayer2PhiHits;   //!
   TBranch        *b_mu_staco_nTGCLayer3PhiHits;   //!
   TBranch        *b_mu_staco_nTGCLayer4PhiHits;   //!
   TBranch        *b_mu_staco_barrelSectors;   //!
   TBranch        *b_mu_staco_endcapSectors;   //!
   TBranch        *b_mu_staco_trackd0;   //!
   TBranch        *b_mu_staco_trackz0;   //!
   TBranch        *b_mu_staco_trackphi;   //!
   TBranch        *b_mu_staco_tracktheta;   //!
   TBranch        *b_mu_staco_trackqoverp;   //!
   TBranch        *b_mu_staco_trackcov_d0;   //!
   TBranch        *b_mu_staco_trackcov_z0;   //!
   TBranch        *b_mu_staco_trackcov_phi;   //!
   TBranch        *b_mu_staco_trackcov_theta;   //!
   TBranch        *b_mu_staco_trackcov_qoverp;   //!
   TBranch        *b_mu_staco_trackcov_d0_z0;   //!
   TBranch        *b_mu_staco_trackcov_d0_phi;   //!
   TBranch        *b_mu_staco_trackcov_d0_theta;   //!
   TBranch        *b_mu_staco_trackcov_d0_qoverp;   //!
   TBranch        *b_mu_staco_trackcov_z0_phi;   //!
   TBranch        *b_mu_staco_trackcov_z0_theta;   //!
   TBranch        *b_mu_staco_trackcov_z0_qoverp;   //!
   TBranch        *b_mu_staco_trackcov_phi_theta;   //!
   TBranch        *b_mu_staco_trackcov_phi_qoverp;   //!
   TBranch        *b_mu_staco_trackcov_theta_qoverp;   //!
   TBranch        *b_mu_staco_trackfitchi2;   //!
   TBranch        *b_mu_staco_trackfitndof;   //!
   TBranch        *b_mu_staco_hastrack;   //!
   TBranch        *b_mu_staco_truth_dr;   //!
   TBranch        *b_mu_staco_truth_E;   //!
   TBranch        *b_mu_staco_truth_pt;   //!
   TBranch        *b_mu_staco_truth_eta;   //!
   TBranch        *b_mu_staco_truth_phi;   //!
   TBranch        *b_mu_staco_truth_type;   //!
   TBranch        *b_mu_staco_truth_status;   //!
   TBranch        *b_mu_staco_truth_barcode;   //!
   TBranch        *b_mu_staco_truth_mothertype;   //!
   TBranch        *b_mu_staco_truth_motherbarcode;   //!
   TBranch        *b_mu_staco_truth_matched;   //!
   TBranch        *b_mu_staco_EFCB_dr;   //!
   TBranch        *b_mu_staco_EFCB_index;   //!
   TBranch        *b_mu_staco_EFMG_dr;   //!
   TBranch        *b_mu_staco_EFMG_index;   //!
   TBranch        *b_mu_staco_EFME_dr;   //!
   TBranch        *b_mu_staco_EFME_index;   //!
   TBranch        *b_mu_staco_L2CB_dr;   //!
   TBranch        *b_mu_staco_L2CB_index;   //!
   TBranch        *b_mu_staco_L1_dr;   //!
   TBranch        *b_mu_staco_L1_index;   //!
   TBranch        *b_mu_calo_n;   //!
   TBranch        *b_mu_calo_E;   //!
   TBranch        *b_mu_calo_pt;   //!
   TBranch        *b_mu_calo_m;   //!
   TBranch        *b_mu_calo_eta;   //!
   TBranch        *b_mu_calo_phi;   //!
   TBranch        *b_mu_calo_px;   //!
   TBranch        *b_mu_calo_py;   //!
   TBranch        *b_mu_calo_pz;   //!
   TBranch        *b_mu_calo_charge;   //!
   TBranch        *b_mu_calo_allauthor;   //!
   TBranch        *b_mu_calo_hastrack;   //!
   TBranch        *b_mu_calo_truth_dr;   //!
   TBranch        *b_mu_calo_truth_E;   //!
   TBranch        *b_mu_calo_truth_pt;   //!
   TBranch        *b_mu_calo_truth_eta;   //!
   TBranch        *b_mu_calo_truth_phi;   //!
   TBranch        *b_mu_calo_truth_type;   //!
   TBranch        *b_mu_calo_truth_status;   //!
   TBranch        *b_mu_calo_truth_barcode;   //!
   TBranch        *b_mu_calo_truth_mothertype;   //!
   TBranch        *b_mu_calo_truth_motherbarcode;   //!
   TBranch        *b_mu_calo_truth_matched;   //!
   TBranch        *b_jet_akt4topoem_n;   //!
   TBranch        *b_jet_akt4topoem_E;   //!
   TBranch        *b_jet_akt4topoem_pt;   //!
   TBranch        *b_jet_akt4topoem_m;   //!
   TBranch        *b_jet_akt4topoem_eta;   //!
   TBranch        *b_jet_akt4topoem_phi;   //!
   TBranch        *b_jet_akt4topoem_EtaOrigin;   //!
   TBranch        *b_jet_akt4topoem_PhiOrigin;   //!
   TBranch        *b_jet_akt4topoem_MOrigin;   //!
   TBranch        *b_jet_akt4topoem_EtaOriginEM;   //!
   TBranch        *b_jet_akt4topoem_PhiOriginEM;   //!
   TBranch        *b_jet_akt4topoem_MOriginEM;   //!
   TBranch        *b_jet_akt4topoem_WIDTH;   //!
   TBranch        *b_jet_akt4topoem_n90;   //!
   TBranch        *b_jet_akt4topoem_Timing;   //!
   TBranch        *b_jet_akt4topoem_LArQuality;   //!
   TBranch        *b_jet_akt4topoem_nTrk;   //!
   TBranch        *b_jet_akt4topoem_sumPtTrk;   //!
   TBranch        *b_jet_akt4topoem_OriginIndex;   //!
   TBranch        *b_jet_akt4topoem_HECQuality;   //!
   TBranch        *b_jet_akt4topoem_NegativeE;   //!
   TBranch        *b_jet_akt4topoem_YFlip12;   //!
   TBranch        *b_jet_akt4topoem_YFlip23;   //!
   TBranch        *b_jet_akt4topoem_BCH_CORR_CELL;   //!
   TBranch        *b_jet_akt4topoem_BCH_CORR_DOTX;   //!
   TBranch        *b_jet_akt4topoem_BCH_CORR_JET;   //!
   TBranch        *b_jet_akt4topoem_BCH_CORR_JET_FORCELL;   //!
   TBranch        *b_jet_akt4topoem_ENG_BAD_CELLS;   //!
   TBranch        *b_jet_akt4topoem_N_BAD_CELLS;   //!
   TBranch        *b_jet_akt4topoem_N_BAD_CELLS_CORR;   //!
   TBranch        *b_jet_akt4topoem_BAD_CELLS_CORR_E;   //!
   TBranch        *b_jet_akt4topoem_NumTowers;   //!
   TBranch        *b_jet_akt4topoem_SamplingMax;   //!
   TBranch        *b_jet_akt4topoem_fracSamplingMax;   //!
   TBranch        *b_jet_akt4topoem_hecf;   //!
   TBranch        *b_jet_akt4topoem_tgap3f;   //!
   TBranch        *b_jet_akt4topoem_isUgly;   //!
   TBranch        *b_jet_akt4topoem_isBadLoose;   //!
   TBranch        *b_jet_akt4topoem_isBadMedium;   //!
   TBranch        *b_jet_akt4topoem_isBadTight;   //!
   TBranch        *b_jet_akt4topoem_emfrac;   //!
   TBranch        *b_jet_akt4topoem_Offset;   //!
   TBranch        *b_jet_akt4topoem_EMJES;   //!
   TBranch        *b_jet_akt4topoem_EMJES_EtaCorr;   //!
   TBranch        *b_jet_akt4topoem_EMJESnooffset;   //!
   TBranch        *b_jet_akt4topoem_GCWJES;   //!
   TBranch        *b_jet_akt4topoem_GCWJES_EtaCorr;   //!
   TBranch        *b_jet_akt4topoem_CB;   //!
   TBranch        *b_jet_akt4topoem_emscale_E;   //!
   TBranch        *b_jet_akt4topoem_emscale_pt;   //!
   TBranch        *b_jet_akt4topoem_emscale_m;   //!
   TBranch        *b_jet_akt4topoem_emscale_eta;   //!
   TBranch        *b_jet_akt4topoem_emscale_phi;   //!
   TBranch        *b_jet_akt4topoem_jvtxf;   //!
   TBranch        *b_jet_akt4topoem_jvtx_x;   //!
   TBranch        *b_jet_akt4topoem_jvtx_y;   //!
   TBranch        *b_jet_akt4topoem_jvtx_z;   //!
   TBranch        *b_jet_akt4topoem_GSCFactorF;   //!
   TBranch        *b_jet_akt4topoem_WidthFraction;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_ip2d_pu;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_ip2d_pb;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_ip2d_ntrk;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_ip3d_pu;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_ip3d_pb;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_ip3d_ntrk;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_sv1_pu;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_sv1_pb;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_sv2_pu;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_sv2_pb;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_svp_mass;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_svp_n2t;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_svp_svok;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_svp_ntrk;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_svp_ntrkv;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_svp_ntrkj;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_svp_efrc;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_sv0p_mass;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_sv0p_n2t;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_sv0p_svok;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_sv0p_ntrk;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_sv0p_ntrkv;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_sv0p_ntrkj;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_sv0p_efrc;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_jfit_pu;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_jfit_pb;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_jfit_nvtxnt;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_jfit_nvtx1t;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_jfit_ntrk;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_jfit_efrc;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_jfit_mass;   //!
   TBranch        *b_jet_akt4topoem_flavor_component_jfit_sig3d;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_TrackCounting2D;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_JetProb;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_IP1D;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_IP2D;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_IP3D;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_SV0;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_SV1;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_SV2;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_JetFitterTag;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_JetFitterCOMB;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_JetFitterTagNN;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_JetFitterCOMBNN;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_SoftMuonTag;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_SoftElectronTag;   //!
   TBranch        *b_jet_akt4topoem_flavor_weight_IP3DSV1;   //!
   TBranch        *b_jet_akt4topoem_flavor_truth_dRminToB;   //!
   TBranch        *b_jet_akt4topoem_flavor_truth_dRminToC;   //!
   TBranch        *b_jet_akt4topoem_flavor_truth_dRminToT;   //!
   TBranch        *b_jet_akt4topoem_flavor_truth_BHadronpdg;   //!
   TBranch        *b_jet_akt4topoem_flavor_truth_trueflav;   //!
   TBranch        *b_jet_akt4topoem_el_dr;   //!
   TBranch        *b_jet_akt4topoem_el_matched;   //!
   TBranch        *b_jet_akt4topoem_mu_dr;   //!
   TBranch        *b_jet_akt4topoem_mu_matched;   //!
   TBranch        *b_jet_akt4topoem_L1_dr;   //!
   TBranch        *b_jet_akt4topoem_L1_matched;   //!
   TBranch        *b_jet_akt4topoem_L2_dr;   //!
   TBranch        *b_jet_akt4topoem_L2_matched;   //!
   TBranch        *b_jet_akt4topoem_EF_dr;   //!
   TBranch        *b_jet_akt4topoem_EF_matched;   //!
   TBranch        *b_jet_akt6topoem_n;   //!
   TBranch        *b_jet_akt6topoem_E;   //!
   TBranch        *b_jet_akt6topoem_pt;   //!
   TBranch        *b_jet_akt6topoem_m;   //!
   TBranch        *b_jet_akt6topoem_eta;   //!
   TBranch        *b_jet_akt6topoem_phi;   //!
   TBranch        *b_jet_akt6topoem_EtaOrigin;   //!
   TBranch        *b_jet_akt6topoem_PhiOrigin;   //!
   TBranch        *b_jet_akt6topoem_MOrigin;   //!
   TBranch        *b_jet_akt6topoem_EtaOriginEM;   //!
   TBranch        *b_jet_akt6topoem_PhiOriginEM;   //!
   TBranch        *b_jet_akt6topoem_MOriginEM;   //!
   TBranch        *b_jet_akt6topoem_WIDTH;   //!
   TBranch        *b_jet_akt6topoem_n90;   //!
   TBranch        *b_jet_akt6topoem_Timing;   //!
   TBranch        *b_jet_akt6topoem_LArQuality;   //!
   TBranch        *b_jet_akt6topoem_nTrk;   //!
   TBranch        *b_jet_akt6topoem_sumPtTrk;   //!
   TBranch        *b_jet_akt6topoem_OriginIndex;   //!
   TBranch        *b_jet_akt6topoem_HECQuality;   //!
   TBranch        *b_jet_akt6topoem_NegativeE;   //!
   TBranch        *b_jet_akt6topoem_YFlip12;   //!
   TBranch        *b_jet_akt6topoem_YFlip23;   //!
   TBranch        *b_jet_akt6topoem_BCH_CORR_CELL;   //!
   TBranch        *b_jet_akt6topoem_BCH_CORR_DOTX;   //!
   TBranch        *b_jet_akt6topoem_BCH_CORR_JET;   //!
   TBranch        *b_jet_akt6topoem_BCH_CORR_JET_FORCELL;   //!
   TBranch        *b_jet_akt6topoem_ENG_BAD_CELLS;   //!
   TBranch        *b_jet_akt6topoem_N_BAD_CELLS;   //!
   TBranch        *b_jet_akt6topoem_N_BAD_CELLS_CORR;   //!
   TBranch        *b_jet_akt6topoem_BAD_CELLS_CORR_E;   //!
   TBranch        *b_jet_akt6topoem_NumTowers;   //!
   TBranch        *b_jet_akt6topoem_SamplingMax;   //!
   TBranch        *b_jet_akt6topoem_fracSamplingMax;   //!
   TBranch        *b_jet_akt6topoem_hecf;   //!
   TBranch        *b_jet_akt6topoem_tgap3f;   //!
   TBranch        *b_jet_akt6topoem_isUgly;   //!
   TBranch        *b_jet_akt6topoem_isBadLoose;   //!
   TBranch        *b_jet_akt6topoem_isBadMedium;   //!
   TBranch        *b_jet_akt6topoem_isBadTight;   //!
   TBranch        *b_jet_akt6topoem_emfrac;   //!
   TBranch        *b_jet_akt6topoem_Offset;   //!
   TBranch        *b_jet_akt6topoem_EMJES;   //!
   TBranch        *b_jet_akt6topoem_EMJES_EtaCorr;   //!
   TBranch        *b_jet_akt6topoem_EMJESnooffset;   //!
   TBranch        *b_jet_akt6topoem_GCWJES;   //!
   TBranch        *b_jet_akt6topoem_GCWJES_EtaCorr;   //!
   TBranch        *b_jet_akt6topoem_CB;   //!
   TBranch        *b_jet_akt6topoem_emscale_E;   //!
   TBranch        *b_jet_akt6topoem_emscale_pt;   //!
   TBranch        *b_jet_akt6topoem_emscale_m;   //!
   TBranch        *b_jet_akt6topoem_emscale_eta;   //!
   TBranch        *b_jet_akt6topoem_emscale_phi;   //!
   TBranch        *b_jet_akt6topoem_jvtxf;   //!
   TBranch        *b_jet_akt6topoem_jvtx_x;   //!
   TBranch        *b_jet_akt6topoem_jvtx_y;   //!
   TBranch        *b_jet_akt6topoem_jvtx_z;   //!
   TBranch        *b_jet_akt6topoem_GSCFactorF;   //!
   TBranch        *b_jet_akt6topoem_WidthFraction;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_ip2d_pu;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_ip2d_pb;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_ip2d_ntrk;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_ip3d_pu;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_ip3d_pb;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_ip3d_ntrk;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_sv1_pu;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_sv1_pb;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_sv2_pu;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_sv2_pb;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_svp_mass;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_svp_n2t;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_svp_svok;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_svp_ntrk;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_svp_ntrkv;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_svp_ntrkj;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_svp_efrc;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_sv0p_mass;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_sv0p_n2t;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_sv0p_svok;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_sv0p_ntrk;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_sv0p_ntrkv;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_sv0p_ntrkj;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_sv0p_efrc;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_jfit_pu;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_jfit_pb;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_jfit_nvtxnt;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_jfit_nvtx1t;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_jfit_ntrk;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_jfit_efrc;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_jfit_mass;   //!
   TBranch        *b_jet_akt6topoem_flavor_component_jfit_sig3d;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_TrackCounting2D;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_JetProb;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_IP1D;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_IP2D;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_IP3D;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_SV0;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_SV1;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_SV2;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_JetFitterTag;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_JetFitterCOMB;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_JetFitterTagNN;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_JetFitterCOMBNN;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_SoftMuonTag;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_SoftElectronTag;   //!
   TBranch        *b_jet_akt6topoem_flavor_weight_IP3DSV1;   //!
   TBranch        *b_jet_akt6topoem_flavor_truth_dRminToB;   //!
   TBranch        *b_jet_akt6topoem_flavor_truth_dRminToC;   //!
   TBranch        *b_jet_akt6topoem_flavor_truth_dRminToT;   //!
   TBranch        *b_jet_akt6topoem_flavor_truth_BHadronpdg;   //!
   TBranch        *b_jet_akt6topoem_flavor_truth_trueflav;   //!
   TBranch        *b_jet_akt6topoem_el_dr;   //!
   TBranch        *b_jet_akt6topoem_el_matched;   //!
   TBranch        *b_jet_akt6topoem_mu_dr;   //!
   TBranch        *b_jet_akt6topoem_mu_matched;   //!
   TBranch        *b_jet_akt6topoem_L1_dr;   //!
   TBranch        *b_jet_akt6topoem_L1_matched;   //!
   TBranch        *b_jet_akt6topoem_L2_dr;   //!
   TBranch        *b_jet_akt6topoem_L2_matched;   //!
   TBranch        *b_jet_akt6topoem_EF_dr;   //!
   TBranch        *b_jet_akt6topoem_EF_matched;   //!
   TBranch        *b_top_hfor_type;   //!
   TBranch        *b_MET_RefFinal_phi;   //!
   TBranch        *b_MET_RefFinal_et;   //!
   TBranch        *b_MET_RefFinal_sumet;   //!
   TBranch        *b_MET_Cryo_phi;   //!
   TBranch        *b_MET_Cryo_et;   //!
   TBranch        *b_MET_Cryo_sumet;   //!
   TBranch        *b_MET_RefEle_phi;   //!
   TBranch        *b_MET_RefEle_et;   //!
   TBranch        *b_MET_RefEle_sumet;   //!
   TBranch        *b_MET_RefJet_phi;   //!
   TBranch        *b_MET_RefJet_et;   //!
   TBranch        *b_MET_RefJet_sumet;   //!
   TBranch        *b_MET_SoftJets_phi;   //!
   TBranch        *b_MET_SoftJets_et;   //!
   TBranch        *b_MET_SoftJets_sumet;   //!
   TBranch        *b_MET_RefMuon_phi;   //!
   TBranch        *b_MET_RefMuon_et;   //!
   TBranch        *b_MET_RefMuon_sumet;   //!
   TBranch        *b_MET_RefMuon_Staco_phi;   //!
   TBranch        *b_MET_RefMuon_Staco_et;   //!
   TBranch        *b_MET_RefMuon_Staco_sumet;   //!
   TBranch        *b_MET_RefMuon_Muid_phi;   //!
   TBranch        *b_MET_RefMuon_Muid_et;   //!
   TBranch        *b_MET_RefMuon_Muid_sumet;   //!
   TBranch        *b_MET_RefGamma_phi;   //!
   TBranch        *b_MET_RefGamma_et;   //!
   TBranch        *b_MET_RefGamma_sumet;   //!
   TBranch        *b_MET_RefTau_phi;   //!
   TBranch        *b_MET_RefTau_et;   //!
   TBranch        *b_MET_RefTau_sumet;   //!
   TBranch        *b_MET_CellOut_phi;   //!
   TBranch        *b_MET_CellOut_et;   //!
   TBranch        *b_MET_CellOut_sumet;   //!
   TBranch        *b_MET_Track_phi;   //!
   TBranch        *b_MET_Track_et;   //!
   TBranch        *b_MET_Track_sumet;   //!
   TBranch        *b_MET_RefFinal_em_etx;   //!
   TBranch        *b_MET_RefFinal_em_ety;   //!
   TBranch        *b_MET_RefFinal_em_phi;   //!
   TBranch        *b_MET_RefFinal_em_et;   //!
   TBranch        *b_MET_RefFinal_em_sumet;   //!
   TBranch        *b_MET_RefEle_em_phi;   //!
   TBranch        *b_MET_RefEle_em_et;   //!
   TBranch        *b_MET_RefEle_em_sumet;   //!
   TBranch        *b_MET_RefJet_em_phi;   //!
   TBranch        *b_MET_RefJet_em_et;   //!
   TBranch        *b_MET_RefJet_em_sumet;   //!
   TBranch        *b_MET_SoftJets_em_phi;   //!
   TBranch        *b_MET_SoftJets_em_et;   //!
   TBranch        *b_MET_SoftJets_em_sumet;   //!
   TBranch        *b_MET_RefMuon_em_phi;   //!
   TBranch        *b_MET_RefMuon_em_et;   //!
   TBranch        *b_MET_RefMuon_em_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_em_phi;   //!
   TBranch        *b_MET_RefMuon_Track_em_et;   //!
   TBranch        *b_MET_RefMuon_Track_em_sumet;   //!
   TBranch        *b_MET_RefGamma_em_phi;   //!
   TBranch        *b_MET_RefGamma_em_et;   //!
   TBranch        *b_MET_RefGamma_em_sumet;   //!
   TBranch        *b_MET_RefTau_em_phi;   //!
   TBranch        *b_MET_RefTau_em_et;   //!
   TBranch        *b_MET_RefTau_em_sumet;   //!
   TBranch        *b_MET_CellOut_em_etx;   //!
   TBranch        *b_MET_CellOut_em_ety;   //!
   TBranch        *b_MET_CellOut_em_phi;   //!
   TBranch        *b_MET_CellOut_em_et;   //!
   TBranch        *b_MET_CellOut_em_sumet;   //!
   TBranch        *b_MET_Muon_Isol_Staco_phi;   //!
   TBranch        *b_MET_Muon_Isol_Staco_et;   //!
   TBranch        *b_MET_Muon_Isol_Staco_sumet;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_phi;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_et;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_sumet;   //!
   TBranch        *b_MET_Muon_Total_Staco_phi;   //!
   TBranch        *b_MET_Muon_Total_Staco_et;   //!
   TBranch        *b_MET_Muon_Total_Staco_sumet;   //!
   TBranch        *b_MET_Muon_Isol_Muid_phi;   //!
   TBranch        *b_MET_Muon_Isol_Muid_et;   //!
   TBranch        *b_MET_Muon_Isol_Muid_sumet;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_phi;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_et;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_sumet;   //!
   TBranch        *b_MET_Muon_Total_Muid_phi;   //!
   TBranch        *b_MET_Muon_Total_Muid_et;   //!
   TBranch        *b_MET_Muon_Total_Muid_sumet;   //!
   TBranch        *b_MET_MuonBoy_phi;   //!
   TBranch        *b_MET_MuonBoy_et;   //!
   TBranch        *b_MET_MuonBoy_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_phi;   //!
   TBranch        *b_MET_RefMuon_Track_et;   //!
   TBranch        *b_MET_RefMuon_Track_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_phi;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_et;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_phi;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_et;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_sumet;   //!
   TBranch        *b_MET_CryoCone_phi;   //!
   TBranch        *b_MET_CryoCone_et;   //!
   TBranch        *b_MET_CryoCone_sumet;   //!
   TBranch        *b_MET_Final_phi;   //!
   TBranch        *b_MET_Final_et;   //!
   TBranch        *b_MET_Final_sumet;   //!
   TBranch        *b_MET_MuonBoy_Spectro_phi;   //!
   TBranch        *b_MET_MuonBoy_Spectro_et;   //!
   TBranch        *b_MET_MuonBoy_Spectro_sumet;   //!
   TBranch        *b_MET_MuonBoy_Track_phi;   //!
   TBranch        *b_MET_MuonBoy_Track_et;   //!
   TBranch        *b_MET_MuonBoy_Track_sumet;   //!
   TBranch        *b_MET_MuonMuid_phi;   //!
   TBranch        *b_MET_MuonMuid_et;   //!
   TBranch        *b_MET_MuonMuid_sumet;   //!
   TBranch        *b_MET_Muid_phi;   //!
   TBranch        *b_MET_Muid_et;   //!
   TBranch        *b_MET_Muid_sumet;   //!
   TBranch        *b_MET_Muid_Spectro_phi;   //!
   TBranch        *b_MET_Muid_Spectro_et;   //!
   TBranch        *b_MET_Muid_Spectro_sumet;   //!
   TBranch        *b_MET_Muid_Track_phi;   //!
   TBranch        *b_MET_Muid_Track_et;   //!
   TBranch        *b_MET_Muid_Track_sumet;   //!
   TBranch        *b_MET_Muon_phi;   //!
   TBranch        *b_MET_Muon_et;   //!
   TBranch        *b_MET_Muon_sumet;   //!
   TBranch        *b_MET_TopoObj_phi;   //!
   TBranch        *b_MET_TopoObj_et;   //!
   TBranch        *b_MET_TopoObj_sumet;   //!
   TBranch        *b_MET_LocHadTopoObj_phi;   //!
   TBranch        *b_MET_LocHadTopoObj_et;   //!
   TBranch        *b_MET_LocHadTopoObj_sumet;   //!
   TBranch        *b_MET_Topo_phi;   //!
   TBranch        *b_MET_Topo_et;   //!
   TBranch        *b_MET_Topo_sumet;   //!
   TBranch        *b_MET_Topo_SUMET_EMFrac;   //!
   TBranch        *b_MET_Topo_etx_PEMB;   //!
   TBranch        *b_MET_Topo_ety_PEMB;   //!
   TBranch        *b_MET_Topo_sumet_PEMB;   //!
   TBranch        *b_MET_Topo_phi_PEMB;   //!
   TBranch        *b_MET_Topo_etx_EMB;   //!
   TBranch        *b_MET_Topo_ety_EMB;   //!
   TBranch        *b_MET_Topo_sumet_EMB;   //!
   TBranch        *b_MET_Topo_phi_EMB;   //!
   TBranch        *b_MET_Topo_etx_PEMEC;   //!
   TBranch        *b_MET_Topo_ety_PEMEC;   //!
   TBranch        *b_MET_Topo_sumet_PEMEC;   //!
   TBranch        *b_MET_Topo_phi_PEMEC;   //!
   TBranch        *b_MET_Topo_etx_EME;   //!
   TBranch        *b_MET_Topo_ety_EME;   //!
   TBranch        *b_MET_Topo_sumet_EME;   //!
   TBranch        *b_MET_Topo_phi_EME;   //!
   TBranch        *b_MET_Topo_etx_TILE;   //!
   TBranch        *b_MET_Topo_ety_TILE;   //!
   TBranch        *b_MET_Topo_sumet_TILE;   //!
   TBranch        *b_MET_Topo_phi_TILE;   //!
   TBranch        *b_MET_Topo_etx_HEC;   //!
   TBranch        *b_MET_Topo_ety_HEC;   //!
   TBranch        *b_MET_Topo_sumet_HEC;   //!
   TBranch        *b_MET_Topo_phi_HEC;   //!
   TBranch        *b_MET_Topo_etx_FCAL;   //!
   TBranch        *b_MET_Topo_ety_FCAL;   //!
   TBranch        *b_MET_Topo_sumet_FCAL;   //!
   TBranch        *b_MET_Topo_phi_FCAL;   //!
   TBranch        *b_MET_Topo_nCell_PEMB;   //!
   TBranch        *b_MET_Topo_nCell_EMB;   //!
   TBranch        *b_MET_Topo_nCell_PEMEC;   //!
   TBranch        *b_MET_Topo_nCell_EME;   //!
   TBranch        *b_MET_Topo_nCell_TILE;   //!
   TBranch        *b_MET_Topo_nCell_HEC;   //!
   TBranch        *b_MET_Topo_nCell_FCAL;   //!
   TBranch        *b_MET_Topo_etx_CentralReg;   //!
   TBranch        *b_MET_Topo_ety_CentralReg;   //!
   TBranch        *b_MET_Topo_sumet_CentralReg;   //!
   TBranch        *b_MET_Topo_phi_CentralReg;   //!
   TBranch        *b_MET_Topo_etx_EndcapRegion;   //!
   TBranch        *b_MET_Topo_ety_EndcapRegion;   //!
   TBranch        *b_MET_Topo_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Topo_phi_EndcapRegion;   //!
   TBranch        *b_MET_Topo_etx_ForwardReg;   //!
   TBranch        *b_MET_Topo_ety_ForwardReg;   //!
   TBranch        *b_MET_Topo_sumet_ForwardReg;   //!
   TBranch        *b_MET_Topo_phi_ForwardReg;   //!
   TBranch        *b_MET_CorrTopo_phi;   //!
   TBranch        *b_MET_CorrTopo_et;   //!
   TBranch        *b_MET_CorrTopo_sumet;   //!
   TBranch        *b_MET_CorrTopo_SUMET_EMFrac;   //!
   TBranch        *b_MET_CorrTopo_etx_PEMB;   //!
   TBranch        *b_MET_CorrTopo_ety_PEMB;   //!
   TBranch        *b_MET_CorrTopo_sumet_PEMB;   //!
   TBranch        *b_MET_CorrTopo_phi_PEMB;   //!
   TBranch        *b_MET_CorrTopo_etx_EMB;   //!
   TBranch        *b_MET_CorrTopo_ety_EMB;   //!
   TBranch        *b_MET_CorrTopo_sumet_EMB;   //!
   TBranch        *b_MET_CorrTopo_phi_EMB;   //!
   TBranch        *b_MET_CorrTopo_etx_PEMEC;   //!
   TBranch        *b_MET_CorrTopo_ety_PEMEC;   //!
   TBranch        *b_MET_CorrTopo_sumet_PEMEC;   //!
   TBranch        *b_MET_CorrTopo_phi_PEMEC;   //!
   TBranch        *b_MET_CorrTopo_etx_EME;   //!
   TBranch        *b_MET_CorrTopo_ety_EME;   //!
   TBranch        *b_MET_CorrTopo_sumet_EME;   //!
   TBranch        *b_MET_CorrTopo_phi_EME;   //!
   TBranch        *b_MET_CorrTopo_etx_TILE;   //!
   TBranch        *b_MET_CorrTopo_ety_TILE;   //!
   TBranch        *b_MET_CorrTopo_sumet_TILE;   //!
   TBranch        *b_MET_CorrTopo_phi_TILE;   //!
   TBranch        *b_MET_CorrTopo_etx_HEC;   //!
   TBranch        *b_MET_CorrTopo_ety_HEC;   //!
   TBranch        *b_MET_CorrTopo_sumet_HEC;   //!
   TBranch        *b_MET_CorrTopo_phi_HEC;   //!
   TBranch        *b_MET_CorrTopo_etx_FCAL;   //!
   TBranch        *b_MET_CorrTopo_ety_FCAL;   //!
   TBranch        *b_MET_CorrTopo_sumet_FCAL;   //!
   TBranch        *b_MET_CorrTopo_phi_FCAL;   //!
   TBranch        *b_MET_CorrTopo_nCell_PEMB;   //!
   TBranch        *b_MET_CorrTopo_nCell_EMB;   //!
   TBranch        *b_MET_CorrTopo_nCell_PEMEC;   //!
   TBranch        *b_MET_CorrTopo_nCell_EME;   //!
   TBranch        *b_MET_CorrTopo_nCell_TILE;   //!
   TBranch        *b_MET_CorrTopo_nCell_HEC;   //!
   TBranch        *b_MET_CorrTopo_nCell_FCAL;   //!
   TBranch        *b_MET_CorrTopo_etx_CentralReg;   //!
   TBranch        *b_MET_CorrTopo_ety_CentralReg;   //!
   TBranch        *b_MET_CorrTopo_sumet_CentralReg;   //!
   TBranch        *b_MET_CorrTopo_phi_CentralReg;   //!
   TBranch        *b_MET_CorrTopo_etx_EndcapRegion;   //!
   TBranch        *b_MET_CorrTopo_ety_EndcapRegion;   //!
   TBranch        *b_MET_CorrTopo_sumet_EndcapRegion;   //!
   TBranch        *b_MET_CorrTopo_phi_EndcapRegion;   //!
   TBranch        *b_MET_CorrTopo_etx_ForwardReg;   //!
   TBranch        *b_MET_CorrTopo_ety_ForwardReg;   //!
   TBranch        *b_MET_CorrTopo_sumet_ForwardReg;   //!
   TBranch        *b_MET_CorrTopo_phi_ForwardReg;   //!
   TBranch        *b_MET_LocHadTopo_phi;   //!
   TBranch        *b_MET_LocHadTopo_et;   //!
   TBranch        *b_MET_LocHadTopo_sumet;   //!
   TBranch        *b_MET_LocHadTopo_SUMET_EMFrac;   //!
   TBranch        *b_MET_LocHadTopo_etx_PEMB;   //!
   TBranch        *b_MET_LocHadTopo_ety_PEMB;   //!
   TBranch        *b_MET_LocHadTopo_sumet_PEMB;   //!
   TBranch        *b_MET_LocHadTopo_phi_PEMB;   //!
   TBranch        *b_MET_LocHadTopo_etx_EMB;   //!
   TBranch        *b_MET_LocHadTopo_ety_EMB;   //!
   TBranch        *b_MET_LocHadTopo_sumet_EMB;   //!
   TBranch        *b_MET_LocHadTopo_phi_EMB;   //!
   TBranch        *b_MET_LocHadTopo_etx_PEMEC;   //!
   TBranch        *b_MET_LocHadTopo_ety_PEMEC;   //!
   TBranch        *b_MET_LocHadTopo_sumet_PEMEC;   //!
   TBranch        *b_MET_LocHadTopo_phi_PEMEC;   //!
   TBranch        *b_MET_LocHadTopo_etx_EME;   //!
   TBranch        *b_MET_LocHadTopo_ety_EME;   //!
   TBranch        *b_MET_LocHadTopo_sumet_EME;   //!
   TBranch        *b_MET_LocHadTopo_phi_EME;   //!
   TBranch        *b_MET_LocHadTopo_etx_TILE;   //!
   TBranch        *b_MET_LocHadTopo_ety_TILE;   //!
   TBranch        *b_MET_LocHadTopo_sumet_TILE;   //!
   TBranch        *b_MET_LocHadTopo_phi_TILE;   //!
   TBranch        *b_MET_LocHadTopo_etx_HEC;   //!
   TBranch        *b_MET_LocHadTopo_ety_HEC;   //!
   TBranch        *b_MET_LocHadTopo_sumet_HEC;   //!
   TBranch        *b_MET_LocHadTopo_phi_HEC;   //!
   TBranch        *b_MET_LocHadTopo_etx_FCAL;   //!
   TBranch        *b_MET_LocHadTopo_ety_FCAL;   //!
   TBranch        *b_MET_LocHadTopo_sumet_FCAL;   //!
   TBranch        *b_MET_LocHadTopo_phi_FCAL;   //!
   TBranch        *b_MET_LocHadTopo_nCell_PEMB;   //!
   TBranch        *b_MET_LocHadTopo_nCell_EMB;   //!
   TBranch        *b_MET_LocHadTopo_nCell_PEMEC;   //!
   TBranch        *b_MET_LocHadTopo_nCell_EME;   //!
   TBranch        *b_MET_LocHadTopo_nCell_TILE;   //!
   TBranch        *b_MET_LocHadTopo_nCell_HEC;   //!
   TBranch        *b_MET_LocHadTopo_nCell_FCAL;   //!
   TBranch        *b_MET_LocHadTopo_etx_CentralReg;   //!
   TBranch        *b_MET_LocHadTopo_ety_CentralReg;   //!
   TBranch        *b_MET_LocHadTopo_sumet_CentralReg;   //!
   TBranch        *b_MET_LocHadTopo_phi_CentralReg;   //!
   TBranch        *b_MET_LocHadTopo_etx_EndcapRegion;   //!
   TBranch        *b_MET_LocHadTopo_ety_EndcapRegion;   //!
   TBranch        *b_MET_LocHadTopo_sumet_EndcapRegion;   //!
   TBranch        *b_MET_LocHadTopo_phi_EndcapRegion;   //!
   TBranch        *b_MET_LocHadTopo_etx_ForwardReg;   //!
   TBranch        *b_MET_LocHadTopo_ety_ForwardReg;   //!
   TBranch        *b_MET_LocHadTopo_sumet_ForwardReg;   //!
   TBranch        *b_MET_LocHadTopo_phi_ForwardReg;   //!
   TBranch        *b_MET_Calib_phi;   //!
   TBranch        *b_MET_Calib_et;   //!
   TBranch        *b_MET_Calib_sumet;   //!
   TBranch        *b_MET_Calib_SUMET_EMFrac;   //!
   TBranch        *b_MET_Calib_etx_PEMB;   //!
   TBranch        *b_MET_Calib_ety_PEMB;   //!
   TBranch        *b_MET_Calib_sumet_PEMB;   //!
   TBranch        *b_MET_Calib_phi_PEMB;   //!
   TBranch        *b_MET_Calib_etx_EMB;   //!
   TBranch        *b_MET_Calib_ety_EMB;   //!
   TBranch        *b_MET_Calib_sumet_EMB;   //!
   TBranch        *b_MET_Calib_phi_EMB;   //!
   TBranch        *b_MET_Calib_etx_PEMEC;   //!
   TBranch        *b_MET_Calib_ety_PEMEC;   //!
   TBranch        *b_MET_Calib_sumet_PEMEC;   //!
   TBranch        *b_MET_Calib_phi_PEMEC;   //!
   TBranch        *b_MET_Calib_etx_EME;   //!
   TBranch        *b_MET_Calib_ety_EME;   //!
   TBranch        *b_MET_Calib_sumet_EME;   //!
   TBranch        *b_MET_Calib_phi_EME;   //!
   TBranch        *b_MET_Calib_etx_TILE;   //!
   TBranch        *b_MET_Calib_ety_TILE;   //!
   TBranch        *b_MET_Calib_sumet_TILE;   //!
   TBranch        *b_MET_Calib_phi_TILE;   //!
   TBranch        *b_MET_Calib_etx_HEC;   //!
   TBranch        *b_MET_Calib_ety_HEC;   //!
   TBranch        *b_MET_Calib_sumet_HEC;   //!
   TBranch        *b_MET_Calib_phi_HEC;   //!
   TBranch        *b_MET_Calib_etx_FCAL;   //!
   TBranch        *b_MET_Calib_ety_FCAL;   //!
   TBranch        *b_MET_Calib_sumet_FCAL;   //!
   TBranch        *b_MET_Calib_phi_FCAL;   //!
   TBranch        *b_MET_Calib_nCell_PEMB;   //!
   TBranch        *b_MET_Calib_nCell_EMB;   //!
   TBranch        *b_MET_Calib_nCell_PEMEC;   //!
   TBranch        *b_MET_Calib_nCell_EME;   //!
   TBranch        *b_MET_Calib_nCell_TILE;   //!
   TBranch        *b_MET_Calib_nCell_HEC;   //!
   TBranch        *b_MET_Calib_nCell_FCAL;   //!
   TBranch        *b_MET_Calib_etx_CentralReg;   //!
   TBranch        *b_MET_Calib_ety_CentralReg;   //!
   TBranch        *b_MET_Calib_sumet_CentralReg;   //!
   TBranch        *b_MET_Calib_phi_CentralReg;   //!
   TBranch        *b_MET_Calib_etx_EndcapRegion;   //!
   TBranch        *b_MET_Calib_ety_EndcapRegion;   //!
   TBranch        *b_MET_Calib_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Calib_phi_EndcapRegion;   //!
   TBranch        *b_MET_Calib_etx_ForwardReg;   //!
   TBranch        *b_MET_Calib_ety_ForwardReg;   //!
   TBranch        *b_MET_Calib_sumet_ForwardReg;   //!
   TBranch        *b_MET_Calib_phi_ForwardReg;   //!
   TBranch        *b_MET_Truth_NonInt_etx;   //!
   TBranch        *b_MET_Truth_NonInt_ety;   //!
   TBranch        *b_MET_Truth_NonInt_phi;   //!
   TBranch        *b_MET_Truth_NonInt_et;   //!
   TBranch        *b_MET_Truth_NonInt_sumet;   //!
   TBranch        *b_MET_Truth_Int_phi;   //!
   TBranch        *b_MET_Truth_IntCentral_phi;   //!
   TBranch        *b_MET_Truth_IntFwd_phi;   //!
   TBranch        *b_MET_Truth_IntOutCover_phi;   //!
   TBranch        *b_MET_Truth_IntMuons_phi;   //!
   TBranch        *b_MET_Truth_Int_et;   //!
   TBranch        *b_MET_Truth_IntCentral_et;   //!
   TBranch        *b_MET_Truth_IntFwd_et;   //!
   TBranch        *b_MET_Truth_IntOutCover_et;   //!
   TBranch        *b_MET_Truth_IntMuons_et;   //!
   TBranch        *b_MET_Truth_Int_sumet;   //!
   TBranch        *b_MET_Truth_IntCentral_sumet;   //!
   TBranch        *b_MET_Truth_IntFwd_sumet;   //!
   TBranch        *b_MET_Truth_IntOutCover_sumet;   //!
   TBranch        *b_MET_Truth_IntMuons_sumet;   //!
   TBranch        *b_MET_Truth_PileUp_NonInt_etx;   //!
   TBranch        *b_MET_Truth_PileUp_NonInt_ety;   //!
   TBranch        *b_MET_Truth_PileUp_NonInt_phi;   //!
   TBranch        *b_MET_Truth_PileUp_NonInt_et;   //!
   TBranch        *b_MET_Truth_PileUp_NonInt_sumet;   //!
   TBranch        *b_MET_Truth_PileUp_Int_etx;   //!
   TBranch        *b_MET_Truth_PileUp_Int_ety;   //!
   TBranch        *b_MET_Truth_PileUp_IntCentral_etx;   //!
   TBranch        *b_MET_Truth_PileUp_IntCentral_ety;   //!
   TBranch        *b_MET_Truth_PileUp_IntFwd_etx;   //!
   TBranch        *b_MET_Truth_PileUp_IntFwd_ety;   //!
   TBranch        *b_MET_Truth_PileUp_IntOutCover_etx;   //!
   TBranch        *b_MET_Truth_PileUp_IntOutCover_ety;   //!
   TBranch        *b_MET_Truth_PileUp_IntMuons_etx;   //!
   TBranch        *b_MET_Truth_PileUp_IntMuons_ety;   //!
   TBranch        *b_MET_Truth_PileUp_Int_phi;   //!
   TBranch        *b_MET_Truth_PileUp_IntCentral_phi;   //!
   TBranch        *b_MET_Truth_PileUp_IntFwd_phi;   //!
   TBranch        *b_MET_Truth_PileUp_IntOutCover_phi;   //!
   TBranch        *b_MET_Truth_PileUp_IntMuons_phi;   //!
   TBranch        *b_MET_Truth_PileUp_Int_et;   //!
   TBranch        *b_MET_Truth_PileUp_IntCentral_et;   //!
   TBranch        *b_MET_Truth_PileUp_IntFwd_et;   //!
   TBranch        *b_MET_Truth_PileUp_IntOutCover_et;   //!
   TBranch        *b_MET_Truth_PileUp_IntMuons_et;   //!
   TBranch        *b_MET_Truth_PileUp_Int_sumet;   //!
   TBranch        *b_MET_Truth_PileUp_IntCentral_sumet;   //!
   TBranch        *b_MET_Truth_PileUp_IntFwd_sumet;   //!
   TBranch        *b_MET_Truth_PileUp_IntOutCover_sumet;   //!
   TBranch        *b_MET_Truth_PileUp_IntMuons_sumet;   //!
   TBranch        *b_MET_DM_Crack1_etx;   //!
   TBranch        *b_MET_DM_Crack1_ety;   //!
   TBranch        *b_MET_DM_Crack1_phi;   //!
   TBranch        *b_MET_DM_Crack1_et;   //!
   TBranch        *b_MET_DM_Crack1_sumet;   //!
   TBranch        *b_MET_DM_Crack2_etx;   //!
   TBranch        *b_MET_DM_Crack2_ety;   //!
   TBranch        *b_MET_DM_Crack2_phi;   //!
   TBranch        *b_MET_DM_Crack2_et;   //!
   TBranch        *b_MET_DM_Crack2_sumet;   //!
   TBranch        *b_MET_DM_All_etx;   //!
   TBranch        *b_MET_DM_All_ety;   //!
   TBranch        *b_MET_DM_All_phi;   //!
   TBranch        *b_MET_DM_All_et;   //!
   TBranch        *b_MET_DM_All_sumet;   //!
   TBranch        *b_MET_DM_Cryo_etx;   //!
   TBranch        *b_MET_DM_Cryo_ety;   //!
   TBranch        *b_MET_DM_Cryo_phi;   //!
   TBranch        *b_MET_DM_Cryo_et;   //!
   TBranch        *b_MET_DM_Cryo_sumet;   //!
   TBranch        *b_METJetsInfo_JetPtWeightedEventEMFraction;   //!
   TBranch        *b_METJetsInfo_JetPtWeightedNumAssociatedTracks;   //!
   TBranch        *b_METJetsInfo_JetPtWeightedSize;   //!
   TBranch        *b_METJetsInfo_LeadingJetEt;   //!
   TBranch        *b_METJetsInfo_LeadingJetEta;   //!
   TBranch        *b_cl_n;   //!
   TBranch        *b_cl_pt;   //!
   TBranch        *b_cl_eta;   //!
   TBranch        *b_cl_phi;   //!
   TBranch        *b_clpt10_n;   //!
   TBranch        *b_clpt10_pt;   //!
   TBranch        *b_clpt10_eta;   //!
   TBranch        *b_clpt10_phi;   //!
   TBranch        *b_clpt10_E_em;   //!
   TBranch        *b_clpt10_E_had;   //!
   TBranch        *b_clpt10_firstEdens;   //!
   TBranch        *b_clpt10_cellmaxfrac;   //!
   TBranch        *b_clpt10_longitudinal;   //!
   TBranch        *b_clpt10_secondlambda;   //!
   TBranch        *b_clpt10_lateral;   //!
   TBranch        *b_clpt10_secondR;   //!
   TBranch        *b_clpt10_centerlambda;   //!
   TBranch        *b_clpt10_deltaTheta;   //!
   TBranch        *b_clpt10_deltaPhi;   //!
   TBranch        *b_clpt10_time;   //!
   TBranch        *b_clpt10_E_PreSamplerB;   //!
   TBranch        *b_clpt10_E_EMB1;   //!
   TBranch        *b_clpt10_E_EMB2;   //!
   TBranch        *b_clpt10_E_EMB3;   //!
   TBranch        *b_clpt10_E_PreSamplerE;   //!
   TBranch        *b_clpt10_E_EME1;   //!
   TBranch        *b_clpt10_E_EME2;   //!
   TBranch        *b_clpt10_E_EME3;   //!
   TBranch        *b_clpt10_E_HEC0;   //!
   TBranch        *b_clpt10_E_HEC1;   //!
   TBranch        *b_clpt10_E_HEC2;   //!
   TBranch        *b_clpt10_E_HEC3;   //!
   TBranch        *b_clpt10_E_TileBar0;   //!
   TBranch        *b_clpt10_E_TileBar1;   //!
   TBranch        *b_clpt10_E_TileBar2;   //!
   TBranch        *b_clpt10_E_TileGap1;   //!
   TBranch        *b_clpt10_E_TileGap2;   //!
   TBranch        *b_clpt10_E_TileGap3;   //!
   TBranch        *b_clpt10_E_TileExt0;   //!
   TBranch        *b_clpt10_E_TileExt1;   //!
   TBranch        *b_clpt10_E_TileExt2;   //!
   TBranch        *b_clpt10_E_FCAL0;   //!
   TBranch        *b_clpt10_E_FCAL1;   //!
   TBranch        *b_clpt10_E_FCAL2;   //!
   TBranch        *b_emclpt10_n;   //!
   TBranch        *b_emclpt10_pt;   //!
   TBranch        *b_emclpt10_eta;   //!
   TBranch        *b_emclpt10_phi;   //!
   TBranch        *b_emclpt10_E_em;   //!
   TBranch        *b_emclpt10_E_had;   //!
   TBranch        *b_emclpt10_firstEdens;   //!
   TBranch        *b_emclpt10_cellmaxfrac;   //!
   TBranch        *b_emclpt10_longitudinal;   //!
   TBranch        *b_emclpt10_secondlambda;   //!
   TBranch        *b_emclpt10_lateral;   //!
   TBranch        *b_emclpt10_secondR;   //!
   TBranch        *b_emclpt10_centerlambda;   //!
   TBranch        *b_emclpt10_deltaTheta;   //!
   TBranch        *b_emclpt10_deltaPhi;   //!
   TBranch        *b_emclpt10_time;   //!
   TBranch        *b_emclpt10_E_PreSamplerB;   //!
   TBranch        *b_emclpt10_E_EMB1;   //!
   TBranch        *b_emclpt10_E_EMB2;   //!
   TBranch        *b_emclpt10_E_EMB3;   //!
   TBranch        *b_emclpt10_E_PreSamplerE;   //!
   TBranch        *b_emclpt10_E_EME1;   //!
   TBranch        *b_emclpt10_E_EME2;   //!
   TBranch        *b_emclpt10_E_EME3;   //!
   TBranch        *b_emclpt10_E_HEC0;   //!
   TBranch        *b_emclpt10_E_HEC1;   //!
   TBranch        *b_emclpt10_E_HEC2;   //!
   TBranch        *b_emclpt10_E_HEC3;   //!
   TBranch        *b_emclpt10_E_TileBar0;   //!
   TBranch        *b_emclpt10_E_TileBar1;   //!
   TBranch        *b_emclpt10_E_TileBar2;   //!
   TBranch        *b_emclpt10_E_TileGap1;   //!
   TBranch        *b_emclpt10_E_TileGap2;   //!
   TBranch        *b_emclpt10_E_TileGap3;   //!
   TBranch        *b_emclpt10_E_TileExt0;   //!
   TBranch        *b_emclpt10_E_TileExt1;   //!
   TBranch        *b_emclpt10_E_TileExt2;   //!
   TBranch        *b_emclpt10_E_FCAL0;   //!
   TBranch        *b_emclpt10_E_FCAL1;   //!
   TBranch        *b_emclpt10_E_FCAL2;   //!
   TBranch        *b_emclpt10_eta_PreSamplerB;   //!
   TBranch        *b_emclpt10_phi_PreSamplerB;   //!
   TBranch        *b_emclpt10_eta_EMB1;   //!
   TBranch        *b_emclpt10_phi_EMB1;   //!
   TBranch        *b_emclpt10_eta_EMB2;   //!
   TBranch        *b_emclpt10_phi_EMB2;   //!
   TBranch        *b_emclpt10_eta_EMB3;   //!
   TBranch        *b_emclpt10_phi_EMB3;   //!
   TBranch        *b_emclpt10_eta_PreSamplerE;   //!
   TBranch        *b_emclpt10_phi_PreSamplerE;   //!
   TBranch        *b_emclpt10_eta_EME1;   //!
   TBranch        *b_emclpt10_phi_EME1;   //!
   TBranch        *b_emclpt10_eta_EME2;   //!
   TBranch        *b_emclpt10_phi_EME2;   //!
   TBranch        *b_emclpt10_eta_EME3;   //!
   TBranch        *b_emclpt10_phi_EME3;   //!
   TBranch        *b_emclpt10_eta_HEC0;   //!
   TBranch        *b_emclpt10_phi_HEC0;   //!
   TBranch        *b_emclpt10_eta_HEC1;   //!
   TBranch        *b_emclpt10_phi_HEC1;   //!
   TBranch        *b_emclpt10_eta_HEC2;   //!
   TBranch        *b_emclpt10_phi_HEC2;   //!
   TBranch        *b_emclpt10_eta_HEC3;   //!
   TBranch        *b_emclpt10_phi_HEC3;   //!
   TBranch        *b_emclpt10_eta_TileBar0;   //!
   TBranch        *b_emclpt10_phi_TileBar0;   //!
   TBranch        *b_emclpt10_eta_TileBar1;   //!
   TBranch        *b_emclpt10_phi_TileBar1;   //!
   TBranch        *b_emclpt10_eta_TileBar2;   //!
   TBranch        *b_emclpt10_phi_TileBar2;   //!
   TBranch        *b_emclpt10_eta_TileGap1;   //!
   TBranch        *b_emclpt10_phi_TileGap1;   //!
   TBranch        *b_emclpt10_eta_TileGap2;   //!
   TBranch        *b_emclpt10_phi_TileGap2;   //!
   TBranch        *b_emclpt10_eta_TileGap3;   //!
   TBranch        *b_emclpt10_phi_TileGap3;   //!
   TBranch        *b_emclpt10_eta_TileExt0;   //!
   TBranch        *b_emclpt10_phi_TileExt0;   //!
   TBranch        *b_emclpt10_eta_TileExt1;   //!
   TBranch        *b_emclpt10_phi_TileExt1;   //!
   TBranch        *b_emclpt10_eta_TileExt2;   //!
   TBranch        *b_emclpt10_phi_TileExt2;   //!
   TBranch        *b_emclpt10_eta_FCAL0;   //!
   TBranch        *b_emclpt10_phi_FCAL0;   //!
   TBranch        *b_emclpt10_eta_FCAL1;   //!
   TBranch        *b_emclpt10_phi_FCAL1;   //!
   TBranch        *b_emclpt10_eta_FCAL2;   //!
   TBranch        *b_emclpt10_phi_FCAL2;   //!
   TBranch        *b_trk_n;   //!
   TBranch        *b_trk_pt;   //!
   TBranch        *b_trk_eta;   //!
   TBranch        *b_trk_d0_wrtPV;   //!
   TBranch        *b_trk_z0_wrtPV;   //!
   TBranch        *b_trk_phi_wrtPV;   //!
   TBranch        *b_trk_theta_wrtPV;   //!
   TBranch        *b_trk_qoverp_wrtPV;   //!
   TBranch        *b_trk_chi2;   //!
   TBranch        *b_trk_ndof;   //!
   TBranch        *b_trkpt5_n;   //!
   TBranch        *b_trkpt5_d0;   //!
   TBranch        *b_trkpt5_z0;   //!
   TBranch        *b_trkpt5_phi;   //!
   TBranch        *b_trkpt5_theta;   //!
   TBranch        *b_trkpt5_qoverp;   //!
   TBranch        *b_trkpt5_pt;   //!
   TBranch        *b_trkpt5_eta;   //!
   TBranch        *b_trkpt5_err_d0;   //!
   TBranch        *b_trkpt5_err_z0;   //!
   TBranch        *b_trkpt5_err_phi;   //!
   TBranch        *b_trkpt5_err_theta;   //!
   TBranch        *b_trkpt5_err_qoverp;   //!
   TBranch        *b_trkpt5_d0_wrtPV;   //!
   TBranch        *b_trkpt5_z0_wrtPV;   //!
   TBranch        *b_trkpt5_phi_wrtPV;   //!
   TBranch        *b_trkpt5_err_d0_wrtPV;   //!
   TBranch        *b_trkpt5_err_z0_wrtPV;   //!
   TBranch        *b_trkpt5_err_phi_wrtPV;   //!
   TBranch        *b_trkpt5_err_theta_wrtPV;   //!
   TBranch        *b_trkpt5_err_qoverp_wrtPV;   //!
   TBranch        *b_trkpt5_cov_d0_z0_wrtPV;   //!
   TBranch        *b_trkpt5_cov_d0_phi_wrtPV;   //!
   TBranch        *b_trkpt5_cov_d0_theta_wrtPV;   //!
   TBranch        *b_trkpt5_cov_d0_qoverp_wrtPV;   //!
   TBranch        *b_trkpt5_cov_z0_phi_wrtPV;   //!
   TBranch        *b_trkpt5_cov_z0_theta_wrtPV;   //!
   TBranch        *b_trkpt5_cov_z0_qoverp_wrtPV;   //!
   TBranch        *b_trkpt5_cov_phi_theta_wrtPV;   //!
   TBranch        *b_trkpt5_cov_phi_qoverp_wrtPV;   //!
   TBranch        *b_trkpt5_cov_theta_qoverp_wrtPV;   //!
   TBranch        *b_trkpt5_chi2;   //!
   TBranch        *b_trkpt5_ndof;   //!
   TBranch        *b_trkpt5_nBLHits;   //!
   TBranch        *b_trkpt5_nPixHits;   //!
   TBranch        *b_trkpt5_nSCTHits;   //!
   TBranch        *b_trkpt5_nTRTHits;   //!
   TBranch        *b_trkpt5_nTRTHighTHits;   //!
   TBranch        *b_trkpt5_nPixHoles;   //!
   TBranch        *b_trkpt5_nSCTHoles;   //!
   TBranch        *b_trkpt5_nTRTHoles;   //!
   TBranch        *b_trkpt5_expectBLayerHit;   //!
   TBranch        *b_trkpt5_nMDTHits;   //!
   TBranch        *b_trkpt5_nCSCEtaHits;   //!
   TBranch        *b_trkpt5_nCSCPhiHits;   //!
   TBranch        *b_trkpt5_nRPCEtaHits;   //!
   TBranch        *b_trkpt5_nRPCPhiHits;   //!
   TBranch        *b_trkpt5_nTGCEtaHits;   //!
   TBranch        *b_trkpt5_nTGCPhiHits;   //!
   TBranch        *b_trkpt5_nHits;   //!
   TBranch        *b_trkpt5_nHoles;   //!
   TBranch        *b_trkpt5_hitPattern;   //!
   TBranch        *b_trkpt5_TRTHighTHitsRatio;   //!
   TBranch        *b_trkpt5_TRTHighTOutliersRatio;   //!
   TBranch        *b_trkpt5_mc_probability;   //!
   TBranch        *b_trkpt5_mc_barcode;   //!
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
   TBranch        *b_vxp_trk_n;   //!
   TBranch        *b_vxp_trk_weight;   //!
   TBranch        *b_vxp_trk_index;   //!
   TBranch        *b_mb_n;   //!
   TBranch        *b_mb_E;   //!
   TBranch        *b_mb_eta;   //!
   TBranch        *b_mb_phi;   //!
   TBranch        *b_mb_time;   //!
   TBranch        *b_mb_quality;   //!
   TBranch        *b_mb_type;   //!
   TBranch        *b_mb_module;   //!
   TBranch        *b_mb_channel;   //!
   TBranch        *b_mbtime_timeDiff;   //!
   TBranch        *b_mbtime_timeA;   //!
   TBranch        *b_mbtime_timeC;   //!
   TBranch        *b_mbtime_countA;   //!
   TBranch        *b_mbtime_countC;   //!
   TBranch        *b_L1_MBTS_1;   //!
   TBranch        *b_L1_MBTS_1_1;   //!
   TBranch        *b_L1_MBTS_1_1_EMPTY;   //!
   TBranch        *b_L1_MBTS_1_1_UNPAIRED_ISO;   //!
   TBranch        *b_L1_MBTS_1_EMPTY;   //!
   TBranch        *b_L1_MBTS_1_UNPAIRED_ISO;   //!
   TBranch        *b_L1_MBTS_2;   //!
   TBranch        *b_L1_MBTS_2_EMPTY;   //!
   TBranch        *b_L1_MBTS_2_UNPAIRED_ISO;   //!
   TBranch        *b_L1_MBTS_2_UNPAIRED_NONISO;   //!
   TBranch        *b_L1_MBTS_4_4;   //!
   TBranch        *b_L1_MBTS_4_4_UNPAIRED_ISO;   //!
   TBranch        *b_muonTruth_n;   //!
   TBranch        *b_muonTruth_pt;   //!
   TBranch        *b_muonTruth_m;   //!
   TBranch        *b_muonTruth_eta;   //!
   TBranch        *b_muonTruth_phi;   //!
   TBranch        *b_muonTruth_charge;   //!
   TBranch        *b_muonTruth_PDGID;   //!
   TBranch        *b_muonTruth_barcode;   //!
   TBranch        *b_muonTruth_type;   //!
   TBranch        *b_muonTruth_origin;   //!
   TBranch        *b_mcevt_n;   //!
   TBranch        *b_mcevt_signal_process_id;   //!
   TBranch        *b_mcevt_event_number;   //!
   TBranch        *b_mcevt_event_scale;   //!
   TBranch        *b_mcevt_alphaQCD;   //!
   TBranch        *b_mcevt_alphaQED;   //!
   TBranch        *b_mcevt_pdf_id1;   //!
   TBranch        *b_mcevt_pdf_id2;   //!
   TBranch        *b_mcevt_pdf_x1;   //!
   TBranch        *b_mcevt_pdf_x2;   //!
   TBranch        *b_mcevt_pdf_scale;   //!
   TBranch        *b_mcevt_pdf1;   //!
   TBranch        *b_mcevt_pdf2;   //!
   TBranch        *b_mcevt_weight;   //!
   TBranch        *b_mcevt_nparticle;   //!
   TBranch        *b_mcevt_pileUpType;   //!
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
   TBranch        *b_jet_antikt4truth_n;   //!
   TBranch        *b_jet_antikt4truth_E;   //!
   TBranch        *b_jet_antikt4truth_pt;   //!
   TBranch        *b_jet_antikt4truth_m;   //!
   TBranch        *b_jet_antikt4truth_eta;   //!
   TBranch        *b_jet_antikt4truth_phi;   //!
   TBranch        *b_jet_antikt4truth_EtaOrigin;   //!
   TBranch        *b_jet_antikt4truth_PhiOrigin;   //!
   TBranch        *b_jet_antikt4truth_MOrigin;   //!
   TBranch        *b_jet_antikt4truth_EtaOriginEM;   //!
   TBranch        *b_jet_antikt4truth_PhiOriginEM;   //!
   TBranch        *b_jet_antikt4truth_MOriginEM;   //!
   TBranch        *b_jet_antikt4truth_WIDTH;   //!
   TBranch        *b_jet_antikt4truth_n90;   //!
   TBranch        *b_jet_antikt4truth_Timing;   //!
   TBranch        *b_jet_antikt4truth_LArQuality;   //!
   TBranch        *b_jet_antikt4truth_nTrk;   //!
   TBranch        *b_jet_antikt4truth_sumPtTrk;   //!
   TBranch        *b_jet_antikt4truth_OriginIndex;   //!
   TBranch        *b_jet_antikt4truth_HECQuality;   //!
   TBranch        *b_jet_antikt4truth_NegativeE;   //!
   TBranch        *b_jet_antikt4truth_YFlip12;   //!
   TBranch        *b_jet_antikt4truth_YFlip23;   //!
   TBranch        *b_jet_antikt4truth_BCH_CORR_CELL;   //!
   TBranch        *b_jet_antikt4truth_BCH_CORR_DOTX;   //!
   TBranch        *b_jet_antikt4truth_BCH_CORR_JET;   //!
   TBranch        *b_jet_antikt4truth_BCH_CORR_JET_FORCELL;   //!
   TBranch        *b_jet_antikt4truth_ENG_BAD_CELLS;   //!
   TBranch        *b_jet_antikt4truth_N_BAD_CELLS;   //!
   TBranch        *b_jet_antikt4truth_N_BAD_CELLS_CORR;   //!
   TBranch        *b_jet_antikt4truth_BAD_CELLS_CORR_E;   //!
   TBranch        *b_jet_antikt4truth_NumTowers;   //!
   TBranch        *b_jet_antikt4truth_SamplingMax;   //!
   TBranch        *b_jet_antikt4truth_fracSamplingMax;   //!
   TBranch        *b_jet_antikt4truth_hecf;   //!
   TBranch        *b_jet_antikt4truth_tgap3f;   //!
   TBranch        *b_jet_antikt4truth_isUgly;   //!
   TBranch        *b_jet_antikt4truth_isBadLoose;   //!
   TBranch        *b_jet_antikt4truth_isBadMedium;   //!
   TBranch        *b_jet_antikt4truth_isBadTight;   //!
   TBranch        *b_jet_antikt4truth_emfrac;   //!
   TBranch        *b_jet_antikt4truth_Offset;   //!
   TBranch        *b_jet_antikt4truth_EMJES;   //!
   TBranch        *b_jet_antikt4truth_EMJES_EtaCorr;   //!
   TBranch        *b_jet_antikt4truth_EMJESnooffset;   //!
   TBranch        *b_jet_antikt4truth_GCWJES;   //!
   TBranch        *b_jet_antikt4truth_GCWJES_EtaCorr;   //!
   TBranch        *b_jet_antikt4truth_CB;   //!
   TBranch        *b_jet_antikt4truth_emscale_E;   //!
   TBranch        *b_jet_antikt4truth_emscale_pt;   //!
   TBranch        *b_jet_antikt4truth_emscale_m;   //!
   TBranch        *b_jet_antikt4truth_emscale_eta;   //!
   TBranch        *b_jet_antikt4truth_emscale_phi;   //!
   TBranch        *b_jet_antikt4truth_el_dr;   //!
   TBranch        *b_jet_antikt4truth_el_matched;   //!
   TBranch        *b_jet_antikt4truth_mu_dr;   //!
   TBranch        *b_jet_antikt4truth_mu_matched;   //!
   TBranch        *b_jet_antikt4truth_L1_dr;   //!
   TBranch        *b_jet_antikt4truth_L1_matched;   //!
   TBranch        *b_jet_antikt4truth_L2_dr;   //!
   TBranch        *b_jet_antikt4truth_L2_matched;   //!
   TBranch        *b_jet_antikt4truth_EF_dr;   //!
   TBranch        *b_jet_antikt4truth_EF_matched;   //!
   TBranch        *b_jet_antikt6truth_n;   //!
   TBranch        *b_jet_antikt6truth_E;   //!
   TBranch        *b_jet_antikt6truth_pt;   //!
   TBranch        *b_jet_antikt6truth_m;   //!
   TBranch        *b_jet_antikt6truth_eta;   //!
   TBranch        *b_jet_antikt6truth_phi;   //!
   TBranch        *b_jet_antikt6truth_EtaOrigin;   //!
   TBranch        *b_jet_antikt6truth_PhiOrigin;   //!
   TBranch        *b_jet_antikt6truth_MOrigin;   //!
   TBranch        *b_jet_antikt6truth_EtaOriginEM;   //!
   TBranch        *b_jet_antikt6truth_PhiOriginEM;   //!
   TBranch        *b_jet_antikt6truth_MOriginEM;   //!
   TBranch        *b_jet_antikt6truth_WIDTH;   //!
   TBranch        *b_jet_antikt6truth_n90;   //!
   TBranch        *b_jet_antikt6truth_Timing;   //!
   TBranch        *b_jet_antikt6truth_LArQuality;   //!
   TBranch        *b_jet_antikt6truth_nTrk;   //!
   TBranch        *b_jet_antikt6truth_sumPtTrk;   //!
   TBranch        *b_jet_antikt6truth_OriginIndex;   //!
   TBranch        *b_jet_antikt6truth_HECQuality;   //!
   TBranch        *b_jet_antikt6truth_NegativeE;   //!
   TBranch        *b_jet_antikt6truth_YFlip12;   //!
   TBranch        *b_jet_antikt6truth_YFlip23;   //!
   TBranch        *b_jet_antikt6truth_BCH_CORR_CELL;   //!
   TBranch        *b_jet_antikt6truth_BCH_CORR_DOTX;   //!
   TBranch        *b_jet_antikt6truth_BCH_CORR_JET;   //!
   TBranch        *b_jet_antikt6truth_BCH_CORR_JET_FORCELL;   //!
   TBranch        *b_jet_antikt6truth_ENG_BAD_CELLS;   //!
   TBranch        *b_jet_antikt6truth_N_BAD_CELLS;   //!
   TBranch        *b_jet_antikt6truth_N_BAD_CELLS_CORR;   //!
   TBranch        *b_jet_antikt6truth_BAD_CELLS_CORR_E;   //!
   TBranch        *b_jet_antikt6truth_NumTowers;   //!
   TBranch        *b_jet_antikt6truth_SamplingMax;   //!
   TBranch        *b_jet_antikt6truth_fracSamplingMax;   //!
   TBranch        *b_jet_antikt6truth_hecf;   //!
   TBranch        *b_jet_antikt6truth_tgap3f;   //!
   TBranch        *b_jet_antikt6truth_isUgly;   //!
   TBranch        *b_jet_antikt6truth_isBadLoose;   //!
   TBranch        *b_jet_antikt6truth_isBadMedium;   //!
   TBranch        *b_jet_antikt6truth_isBadTight;   //!
   TBranch        *b_jet_antikt6truth_emfrac;   //!
   TBranch        *b_jet_antikt6truth_Offset;   //!
   TBranch        *b_jet_antikt6truth_EMJES;   //!
   TBranch        *b_jet_antikt6truth_EMJES_EtaCorr;   //!
   TBranch        *b_jet_antikt6truth_EMJESnooffset;   //!
   TBranch        *b_jet_antikt6truth_GCWJES;   //!
   TBranch        *b_jet_antikt6truth_GCWJES_EtaCorr;   //!
   TBranch        *b_jet_antikt6truth_CB;   //!
   TBranch        *b_jet_antikt6truth_emscale_E;   //!
   TBranch        *b_jet_antikt6truth_emscale_pt;   //!
   TBranch        *b_jet_antikt6truth_emscale_m;   //!
   TBranch        *b_jet_antikt6truth_emscale_eta;   //!
   TBranch        *b_jet_antikt6truth_emscale_phi;   //!
   TBranch        *b_jet_antikt6truth_el_dr;   //!
   TBranch        *b_jet_antikt6truth_el_matched;   //!
   TBranch        *b_jet_antikt6truth_mu_dr;   //!
   TBranch        *b_jet_antikt6truth_mu_matched;   //!
   TBranch        *b_jet_antikt6truth_L1_dr;   //!
   TBranch        *b_jet_antikt6truth_L1_matched;   //!
   TBranch        *b_jet_antikt6truth_L2_dr;   //!
   TBranch        *b_jet_antikt6truth_L2_matched;   //!
   TBranch        *b_jet_antikt6truth_EF_dr;   //!
   TBranch        *b_jet_antikt6truth_EF_matched;   //!
   TBranch        *b_jet_antikt4truthALL_n;   //!
   TBranch        *b_jet_antikt4truthALL_E;   //!
   TBranch        *b_jet_antikt4truthALL_pt;   //!
   TBranch        *b_jet_antikt4truthALL_m;   //!
   TBranch        *b_jet_antikt4truthALL_eta;   //!
   TBranch        *b_jet_antikt4truthALL_phi;   //!
   TBranch        *b_jet_antikt4truthALL_EtaOrigin;   //!
   TBranch        *b_jet_antikt4truthALL_PhiOrigin;   //!
   TBranch        *b_jet_antikt4truthALL_MOrigin;   //!
   TBranch        *b_jet_antikt4truthALL_EtaOriginEM;   //!
   TBranch        *b_jet_antikt4truthALL_PhiOriginEM;   //!
   TBranch        *b_jet_antikt4truthALL_MOriginEM;   //!
   TBranch        *b_jet_antikt4truthALL_WIDTH;   //!
   TBranch        *b_jet_antikt4truthALL_n90;   //!
   TBranch        *b_jet_antikt4truthALL_Timing;   //!
   TBranch        *b_jet_antikt4truthALL_LArQuality;   //!
   TBranch        *b_jet_antikt4truthALL_nTrk;   //!
   TBranch        *b_jet_antikt4truthALL_sumPtTrk;   //!
   TBranch        *b_jet_antikt4truthALL_OriginIndex;   //!
   TBranch        *b_jet_antikt4truthALL_HECQuality;   //!
   TBranch        *b_jet_antikt4truthALL_NegativeE;   //!
   TBranch        *b_jet_antikt4truthALL_YFlip12;   //!
   TBranch        *b_jet_antikt4truthALL_YFlip23;   //!
   TBranch        *b_jet_antikt4truthALL_BCH_CORR_CELL;   //!
   TBranch        *b_jet_antikt4truthALL_BCH_CORR_DOTX;   //!
   TBranch        *b_jet_antikt4truthALL_BCH_CORR_JET;   //!
   TBranch        *b_jet_antikt4truthALL_BCH_CORR_JET_FORCELL;   //!
   TBranch        *b_jet_antikt4truthALL_ENG_BAD_CELLS;   //!
   TBranch        *b_jet_antikt4truthALL_N_BAD_CELLS;   //!
   TBranch        *b_jet_antikt4truthALL_N_BAD_CELLS_CORR;   //!
   TBranch        *b_jet_antikt4truthALL_BAD_CELLS_CORR_E;   //!
   TBranch        *b_jet_antikt4truthALL_NumTowers;   //!
   TBranch        *b_jet_antikt4truthALL_SamplingMax;   //!
   TBranch        *b_jet_antikt4truthALL_fracSamplingMax;   //!
   TBranch        *b_jet_antikt4truthALL_hecf;   //!
   TBranch        *b_jet_antikt4truthALL_tgap3f;   //!
   TBranch        *b_jet_antikt4truthALL_isUgly;   //!
   TBranch        *b_jet_antikt4truthALL_isBadLoose;   //!
   TBranch        *b_jet_antikt4truthALL_isBadMedium;   //!
   TBranch        *b_jet_antikt4truthALL_isBadTight;   //!
   TBranch        *b_jet_antikt4truthALL_emfrac;   //!
   TBranch        *b_jet_antikt4truthALL_Offset;   //!
   TBranch        *b_jet_antikt4truthALL_EMJES;   //!
   TBranch        *b_jet_antikt4truthALL_EMJES_EtaCorr;   //!
   TBranch        *b_jet_antikt4truthALL_EMJESnooffset;   //!
   TBranch        *b_jet_antikt4truthALL_GCWJES;   //!
   TBranch        *b_jet_antikt4truthALL_GCWJES_EtaCorr;   //!
   TBranch        *b_jet_antikt4truthALL_CB;   //!
   TBranch        *b_jet_antikt4truthALL_emscale_E;   //!
   TBranch        *b_jet_antikt4truthALL_emscale_pt;   //!
   TBranch        *b_jet_antikt4truthALL_emscale_m;   //!
   TBranch        *b_jet_antikt4truthALL_emscale_eta;   //!
   TBranch        *b_jet_antikt4truthALL_emscale_phi;   //!
   TBranch        *b_jet_antikt4truthALL_el_dr;   //!
   TBranch        *b_jet_antikt4truthALL_el_matched;   //!
   TBranch        *b_jet_antikt4truthALL_mu_dr;   //!
   TBranch        *b_jet_antikt4truthALL_mu_matched;   //!
   TBranch        *b_jet_antikt4truthALL_L1_dr;   //!
   TBranch        *b_jet_antikt4truthALL_L1_matched;   //!
   TBranch        *b_jet_antikt4truthALL_L2_dr;   //!
   TBranch        *b_jet_antikt4truthALL_L2_matched;   //!
   TBranch        *b_jet_antikt4truthALL_EF_dr;   //!
   TBranch        *b_jet_antikt4truthALL_EF_matched;   //!
   TBranch        *b_jet_antikt6truthALL_n;   //!
   TBranch        *b_jet_antikt6truthALL_E;   //!
   TBranch        *b_jet_antikt6truthALL_pt;   //!
   TBranch        *b_jet_antikt6truthALL_m;   //!
   TBranch        *b_jet_antikt6truthALL_eta;   //!
   TBranch        *b_jet_antikt6truthALL_phi;   //!
   TBranch        *b_jet_antikt6truthALL_EtaOrigin;   //!
   TBranch        *b_jet_antikt6truthALL_PhiOrigin;   //!
   TBranch        *b_jet_antikt6truthALL_MOrigin;   //!
   TBranch        *b_jet_antikt6truthALL_EtaOriginEM;   //!
   TBranch        *b_jet_antikt6truthALL_PhiOriginEM;   //!
   TBranch        *b_jet_antikt6truthALL_MOriginEM;   //!
   TBranch        *b_jet_antikt6truthALL_WIDTH;   //!
   TBranch        *b_jet_antikt6truthALL_n90;   //!
   TBranch        *b_jet_antikt6truthALL_Timing;   //!
   TBranch        *b_jet_antikt6truthALL_LArQuality;   //!
   TBranch        *b_jet_antikt6truthALL_nTrk;   //!
   TBranch        *b_jet_antikt6truthALL_sumPtTrk;   //!
   TBranch        *b_jet_antikt6truthALL_OriginIndex;   //!
   TBranch        *b_jet_antikt6truthALL_HECQuality;   //!
   TBranch        *b_jet_antikt6truthALL_NegativeE;   //!
   TBranch        *b_jet_antikt6truthALL_YFlip12;   //!
   TBranch        *b_jet_antikt6truthALL_YFlip23;   //!
   TBranch        *b_jet_antikt6truthALL_BCH_CORR_CELL;   //!
   TBranch        *b_jet_antikt6truthALL_BCH_CORR_DOTX;   //!
   TBranch        *b_jet_antikt6truthALL_BCH_CORR_JET;   //!
   TBranch        *b_jet_antikt6truthALL_BCH_CORR_JET_FORCELL;   //!
   TBranch        *b_jet_antikt6truthALL_ENG_BAD_CELLS;   //!
   TBranch        *b_jet_antikt6truthALL_N_BAD_CELLS;   //!
   TBranch        *b_jet_antikt6truthALL_N_BAD_CELLS_CORR;   //!
   TBranch        *b_jet_antikt6truthALL_BAD_CELLS_CORR_E;   //!
   TBranch        *b_jet_antikt6truthALL_NumTowers;   //!
   TBranch        *b_jet_antikt6truthALL_SamplingMax;   //!
   TBranch        *b_jet_antikt6truthALL_fracSamplingMax;   //!
   TBranch        *b_jet_antikt6truthALL_hecf;   //!
   TBranch        *b_jet_antikt6truthALL_tgap3f;   //!
   TBranch        *b_jet_antikt6truthALL_isUgly;   //!
   TBranch        *b_jet_antikt6truthALL_isBadLoose;   //!
   TBranch        *b_jet_antikt6truthALL_isBadMedium;   //!
   TBranch        *b_jet_antikt6truthALL_isBadTight;   //!
   TBranch        *b_jet_antikt6truthALL_emfrac;   //!
   TBranch        *b_jet_antikt6truthALL_Offset;   //!
   TBranch        *b_jet_antikt6truthALL_EMJES;   //!
   TBranch        *b_jet_antikt6truthALL_EMJES_EtaCorr;   //!
   TBranch        *b_jet_antikt6truthALL_EMJESnooffset;   //!
   TBranch        *b_jet_antikt6truthALL_GCWJES;   //!
   TBranch        *b_jet_antikt6truthALL_GCWJES_EtaCorr;   //!
   TBranch        *b_jet_antikt6truthALL_CB;   //!
   TBranch        *b_jet_antikt6truthALL_emscale_E;   //!
   TBranch        *b_jet_antikt6truthALL_emscale_pt;   //!
   TBranch        *b_jet_antikt6truthALL_emscale_m;   //!
   TBranch        *b_jet_antikt6truthALL_emscale_eta;   //!
   TBranch        *b_jet_antikt6truthALL_emscale_phi;   //!
   TBranch        *b_jet_antikt6truthALL_el_dr;   //!
   TBranch        *b_jet_antikt6truthALL_el_matched;   //!
   TBranch        *b_jet_antikt6truthALL_mu_dr;   //!
   TBranch        *b_jet_antikt6truthALL_mu_matched;   //!
   TBranch        *b_jet_antikt6truthALL_L1_dr;   //!
   TBranch        *b_jet_antikt6truthALL_L1_matched;   //!
   TBranch        *b_jet_antikt6truthALL_L2_dr;   //!
   TBranch        *b_jet_antikt6truthALL_L2_matched;   //!
   TBranch        *b_jet_antikt6truthALL_EF_dr;   //!
   TBranch        *b_jet_antikt6truthALL_EF_matched;   //!
   TBranch        *b_jet_antikt4truthWZ_n;   //!
   TBranch        *b_jet_antikt4truthWZ_E;   //!
   TBranch        *b_jet_antikt4truthWZ_pt;   //!
   TBranch        *b_jet_antikt4truthWZ_m;   //!
   TBranch        *b_jet_antikt4truthWZ_eta;   //!
   TBranch        *b_jet_antikt4truthWZ_phi;   //!
   TBranch        *b_jet_antikt4truthWZ_EtaOrigin;   //!
   TBranch        *b_jet_antikt4truthWZ_PhiOrigin;   //!
   TBranch        *b_jet_antikt4truthWZ_MOrigin;   //!
   TBranch        *b_jet_antikt4truthWZ_EtaOriginEM;   //!
   TBranch        *b_jet_antikt4truthWZ_PhiOriginEM;   //!
   TBranch        *b_jet_antikt4truthWZ_MOriginEM;   //!
   TBranch        *b_jet_antikt4truthWZ_WIDTH;   //!
   TBranch        *b_jet_antikt4truthWZ_n90;   //!
   TBranch        *b_jet_antikt4truthWZ_Timing;   //!
   TBranch        *b_jet_antikt4truthWZ_LArQuality;   //!
   TBranch        *b_jet_antikt4truthWZ_nTrk;   //!
   TBranch        *b_jet_antikt4truthWZ_sumPtTrk;   //!
   TBranch        *b_jet_antikt4truthWZ_OriginIndex;   //!
   TBranch        *b_jet_antikt4truthWZ_HECQuality;   //!
   TBranch        *b_jet_antikt4truthWZ_NegativeE;   //!
   TBranch        *b_jet_antikt4truthWZ_YFlip12;   //!
   TBranch        *b_jet_antikt4truthWZ_YFlip23;   //!
   TBranch        *b_jet_antikt4truthWZ_BCH_CORR_CELL;   //!
   TBranch        *b_jet_antikt4truthWZ_BCH_CORR_DOTX;   //!
   TBranch        *b_jet_antikt4truthWZ_BCH_CORR_JET;   //!
   TBranch        *b_jet_antikt4truthWZ_BCH_CORR_JET_FORCELL;   //!
   TBranch        *b_jet_antikt4truthWZ_ENG_BAD_CELLS;   //!
   TBranch        *b_jet_antikt4truthWZ_N_BAD_CELLS;   //!
   TBranch        *b_jet_antikt4truthWZ_N_BAD_CELLS_CORR;   //!
   TBranch        *b_jet_antikt4truthWZ_BAD_CELLS_CORR_E;   //!
   TBranch        *b_jet_antikt4truthWZ_NumTowers;   //!
   TBranch        *b_jet_antikt4truthWZ_SamplingMax;   //!
   TBranch        *b_jet_antikt4truthWZ_fracSamplingMax;   //!
   TBranch        *b_jet_antikt4truthWZ_hecf;   //!
   TBranch        *b_jet_antikt4truthWZ_tgap3f;   //!
   TBranch        *b_jet_antikt4truthWZ_isUgly;   //!
   TBranch        *b_jet_antikt4truthWZ_isBadLoose;   //!
   TBranch        *b_jet_antikt4truthWZ_isBadMedium;   //!
   TBranch        *b_jet_antikt4truthWZ_isBadTight;   //!
   TBranch        *b_jet_antikt4truthWZ_emfrac;   //!
   TBranch        *b_jet_antikt4truthWZ_Offset;   //!
   TBranch        *b_jet_antikt4truthWZ_EMJES;   //!
   TBranch        *b_jet_antikt4truthWZ_EMJES_EtaCorr;   //!
   TBranch        *b_jet_antikt4truthWZ_EMJESnooffset;   //!
   TBranch        *b_jet_antikt4truthWZ_GCWJES;   //!
   TBranch        *b_jet_antikt4truthWZ_GCWJES_EtaCorr;   //!
   TBranch        *b_jet_antikt4truthWZ_CB;   //!
   TBranch        *b_jet_antikt4truthWZ_emscale_E;   //!
   TBranch        *b_jet_antikt4truthWZ_emscale_pt;   //!
   TBranch        *b_jet_antikt4truthWZ_emscale_m;   //!
   TBranch        *b_jet_antikt4truthWZ_emscale_eta;   //!
   TBranch        *b_jet_antikt4truthWZ_emscale_phi;   //!
   TBranch        *b_jet_antikt4truthWZ_el_dr;   //!
   TBranch        *b_jet_antikt4truthWZ_el_matched;   //!
   TBranch        *b_jet_antikt4truthWZ_mu_dr;   //!
   TBranch        *b_jet_antikt4truthWZ_mu_matched;   //!
   TBranch        *b_jet_antikt4truthWZ_L1_dr;   //!
   TBranch        *b_jet_antikt4truthWZ_L1_matched;   //!
   TBranch        *b_jet_antikt4truthWZ_L2_dr;   //!
   TBranch        *b_jet_antikt4truthWZ_L2_matched;   //!
   TBranch        *b_jet_antikt4truthWZ_EF_dr;   //!
   TBranch        *b_jet_antikt4truthWZ_EF_matched;   //!
   TBranch        *b_jet_antikt6truthWZ_n;   //!
   TBranch        *b_jet_antikt6truthWZ_E;   //!
   TBranch        *b_jet_antikt6truthWZ_pt;   //!
   TBranch        *b_jet_antikt6truthWZ_m;   //!
   TBranch        *b_jet_antikt6truthWZ_eta;   //!
   TBranch        *b_jet_antikt6truthWZ_phi;   //!
   TBranch        *b_jet_antikt6truthWZ_EtaOrigin;   //!
   TBranch        *b_jet_antikt6truthWZ_PhiOrigin;   //!
   TBranch        *b_jet_antikt6truthWZ_MOrigin;   //!
   TBranch        *b_jet_antikt6truthWZ_EtaOriginEM;   //!
   TBranch        *b_jet_antikt6truthWZ_PhiOriginEM;   //!
   TBranch        *b_jet_antikt6truthWZ_MOriginEM;   //!
   TBranch        *b_jet_antikt6truthWZ_WIDTH;   //!
   TBranch        *b_jet_antikt6truthWZ_n90;   //!
   TBranch        *b_jet_antikt6truthWZ_Timing;   //!
   TBranch        *b_jet_antikt6truthWZ_LArQuality;   //!
   TBranch        *b_jet_antikt6truthWZ_nTrk;   //!
   TBranch        *b_jet_antikt6truthWZ_sumPtTrk;   //!
   TBranch        *b_jet_antikt6truthWZ_OriginIndex;   //!
   TBranch        *b_jet_antikt6truthWZ_HECQuality;   //!
   TBranch        *b_jet_antikt6truthWZ_NegativeE;   //!
   TBranch        *b_jet_antikt6truthWZ_YFlip12;   //!
   TBranch        *b_jet_antikt6truthWZ_YFlip23;   //!
   TBranch        *b_jet_antikt6truthWZ_BCH_CORR_CELL;   //!
   TBranch        *b_jet_antikt6truthWZ_BCH_CORR_DOTX;   //!
   TBranch        *b_jet_antikt6truthWZ_BCH_CORR_JET;   //!
   TBranch        *b_jet_antikt6truthWZ_BCH_CORR_JET_FORCELL;   //!
   TBranch        *b_jet_antikt6truthWZ_ENG_BAD_CELLS;   //!
   TBranch        *b_jet_antikt6truthWZ_N_BAD_CELLS;   //!
   TBranch        *b_jet_antikt6truthWZ_N_BAD_CELLS_CORR;   //!
   TBranch        *b_jet_antikt6truthWZ_BAD_CELLS_CORR_E;   //!
   TBranch        *b_jet_antikt6truthWZ_NumTowers;   //!
   TBranch        *b_jet_antikt6truthWZ_SamplingMax;   //!
   TBranch        *b_jet_antikt6truthWZ_fracSamplingMax;   //!
   TBranch        *b_jet_antikt6truthWZ_hecf;   //!
   TBranch        *b_jet_antikt6truthWZ_tgap3f;   //!
   TBranch        *b_jet_antikt6truthWZ_isUgly;   //!
   TBranch        *b_jet_antikt6truthWZ_isBadLoose;   //!
   TBranch        *b_jet_antikt6truthWZ_isBadMedium;   //!
   TBranch        *b_jet_antikt6truthWZ_isBadTight;   //!
   TBranch        *b_jet_antikt6truthWZ_emfrac;   //!
   TBranch        *b_jet_antikt6truthWZ_Offset;   //!
   TBranch        *b_jet_antikt6truthWZ_EMJES;   //!
   TBranch        *b_jet_antikt6truthWZ_EMJES_EtaCorr;   //!
   TBranch        *b_jet_antikt6truthWZ_EMJESnooffset;   //!
   TBranch        *b_jet_antikt6truthWZ_GCWJES;   //!
   TBranch        *b_jet_antikt6truthWZ_GCWJES_EtaCorr;   //!
   TBranch        *b_jet_antikt6truthWZ_CB;   //!
   TBranch        *b_jet_antikt6truthWZ_emscale_E;   //!
   TBranch        *b_jet_antikt6truthWZ_emscale_pt;   //!
   TBranch        *b_jet_antikt6truthWZ_emscale_m;   //!
   TBranch        *b_jet_antikt6truthWZ_emscale_eta;   //!
   TBranch        *b_jet_antikt6truthWZ_emscale_phi;   //!
   TBranch        *b_jet_antikt6truthWZ_el_dr;   //!
   TBranch        *b_jet_antikt6truthWZ_el_matched;   //!
   TBranch        *b_jet_antikt6truthWZ_mu_dr;   //!
   TBranch        *b_jet_antikt6truthWZ_mu_matched;   //!
   TBranch        *b_jet_antikt6truthWZ_L1_dr;   //!
   TBranch        *b_jet_antikt6truthWZ_L1_matched;   //!
   TBranch        *b_jet_antikt6truthWZ_L2_dr;   //!
   TBranch        *b_jet_antikt6truthWZ_L2_matched;   //!
   TBranch        *b_jet_antikt6truthWZ_EF_dr;   //!
   TBranch        *b_jet_antikt6truthWZ_EF_matched;   //!
   TBranch        *b_trig_L1_TAV;   //!
   TBranch        *b_trig_L2_passedPhysics;   //!
   TBranch        *b_trig_EF_passedPhysics;   //!
   TBranch        *b_trig_L1_TBP;   //!
   TBranch        *b_trig_L1_TAP;   //!
   TBranch        *b_trig_L2_passedRaw;   //!
   TBranch        *b_trig_EF_passedRaw;   //!
   TBranch        *b_trig_L2_truncated;   //!
   TBranch        *b_trig_EF_truncated;   //!
   TBranch        *b_trig_L2_resurrected;   //!
   TBranch        *b_trig_EF_resurrected;   //!
   TBranch        *b_trig_L2_passedThrough;   //!
   TBranch        *b_trig_EF_passedThrough;   //!
   TBranch        *b_trig_DB_SMK;   //!
   TBranch        *b_trig_DB_L1PSK;   //!
   TBranch        *b_trig_DB_HLTPSK;   //!
   TBranch        *b_trig_bgCode;   //!
   TBranch        *b_trig_L1_emtau_n;   //!
   TBranch        *b_trig_L1_emtau_eta;   //!
   TBranch        *b_trig_L1_emtau_phi;   //!
   TBranch        *b_trig_L1_emtau_thrNames;   //!
   TBranch        *b_trig_L1_emtau_thrValues;   //!
   TBranch        *b_trig_L2_emcl_n;   //!
   TBranch        *b_trig_L2_emcl_quality;   //!
   TBranch        *b_trig_L2_emcl_E;   //!
   TBranch        *b_trig_L2_emcl_Et;   //!
   TBranch        *b_trig_L2_emcl_eta;   //!
   TBranch        *b_trig_L2_emcl_phi;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_n;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_algorithmId;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_trackStatus;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_chi2Ndof;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_param_a0;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_param_z0;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_param_phi0;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_param_eta;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_param_pt;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_n;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_algorithmId;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_trackStatus;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_chi2Ndof;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_param_a0;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_param_z0;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_param_phi0;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_param_eta;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_param_pt;   //!
   TBranch        *b_trig_L2_el_n;   //!
   TBranch        *b_trig_L2_el_E;   //!
   TBranch        *b_trig_L2_el_Et;   //!
   TBranch        *b_trig_L2_el_pt;   //!
   TBranch        *b_trig_L2_el_eta;   //!
   TBranch        *b_trig_L2_el_phi;   //!
   TBranch        *b_trig_L2_el_RoIWord;   //!
   TBranch        *b_trig_L2_el_zvertex;   //!
   TBranch        *b_trig_L2_el_charge;   //!
   TBranch        *b_trig_L2_ph_n;   //!
   TBranch        *b_trig_L2_ph_E;   //!
   TBranch        *b_trig_L2_ph_Et;   //!
   TBranch        *b_trig_L2_ph_pt;   //!
   TBranch        *b_trig_L2_ph_eta;   //!
   TBranch        *b_trig_L2_ph_phi;   //!
   TBranch        *b_trig_L2_ph_RoIWord;   //!
   TBranch        *b_trig_EF_emcl_n;   //!
   TBranch        *b_trig_EF_emcl_pt;   //!
   TBranch        *b_trig_EF_emcl_eta;   //!
   TBranch        *b_trig_EF_emcl_phi;   //!
   TBranch        *b_trig_EF_emcl_E_em;   //!
   TBranch        *b_trig_EF_emcl_E_had;   //!
   TBranch        *b_trig_EF_emcl_slw_n;   //!
   TBranch        *b_trig_EF_emcl_slw_pt;   //!
   TBranch        *b_trig_EF_emcl_slw_eta;   //!
   TBranch        *b_trig_EF_emcl_slw_phi;   //!
   TBranch        *b_trig_EF_emcl_slw_E_em;   //!
   TBranch        *b_trig_EF_emcl_slw_E_had;   //!
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
   TBranch        *b_trig_EF_el_vertweight;   //!
   TBranch        *b_trig_EF_el_hastrack;   //!
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
   TBranch        *b_trig_EF_ph_author;   //!
   TBranch        *b_trig_EF_ph_isRecovered;   //!
   TBranch        *b_trig_EF_ph_isEM;   //!
   TBranch        *b_trig_EF_ph_convFlag;   //!
   TBranch        *b_trig_EF_ph_isConv;   //!
   TBranch        *b_trig_EF_ph_nConv;   //!
   TBranch        *b_trig_EF_ph_nSingleTrackConv;   //!
   TBranch        *b_trig_EF_ph_nDoubleTrackConv;   //!
   TBranch        *b_trig_EF_ph_loose;   //!
   TBranch        *b_trig_EF_ph_tight;   //!
   TBranch        *b_trig_EF_ph_tightIso;   //!
   TBranch        *b_trig_Nav_n;   //!
   TBranch        *b_trig_Nav_chain_ChainId;   //!
   TBranch        *b_trig_Nav_chain_RoIType;   //!
   TBranch        *b_trig_Nav_chain_RoIIndex;   //!
   TBranch        *b_trig_RoI_L2_e_n;   //!
   TBranch        *b_trig_RoI_EF_e_n;   //!
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
   TBranch        *b_trig_L2_isomuonfeature_EtInnerConeEC;   //!
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
   TBranch        *b_trig_L2_sitrack_muon_param_a0;   //!
   TBranch        *b_trig_L2_sitrack_muon_param_z0;   //!
   TBranch        *b_trig_L2_sitrack_muon_param_phi0;   //!
   TBranch        *b_trig_L2_sitrack_muon_param_eta;   //!
   TBranch        *b_trig_L2_sitrack_muon_param_pt;   //!
   TBranch        *b_trig_L2_idscan_muon_n;   //!
   TBranch        *b_trig_L2_idscan_muon_algorithmId;   //!
   TBranch        *b_trig_L2_idscan_muon_trackStatus;   //!
   TBranch        *b_trig_L2_idscan_muon_chi2Ndof;   //!
   TBranch        *b_trig_L2_idscan_muon_param_a0;   //!
   TBranch        *b_trig_L2_idscan_muon_param_z0;   //!
   TBranch        *b_trig_L2_idscan_muon_param_phi0;   //!
   TBranch        *b_trig_L2_idscan_muon_param_eta;   //!
   TBranch        *b_trig_L2_idscan_muon_param_pt;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_n;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_algorithmId;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_trackStatus;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_chi2Ndof;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_param_a0;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_param_z0;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_param_phi0;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_param_eta;   //!
   TBranch        *b_trig_L2_sitrack_isomuon_param_pt;   //!
   TBranch        *b_trig_L2_idscan_isomuon_n;   //!
   TBranch        *b_trig_L2_idscan_isomuon_algorithmId;   //!
   TBranch        *b_trig_L2_idscan_isomuon_trackStatus;   //!
   TBranch        *b_trig_L2_idscan_isomuon_chi2Ndof;   //!
   TBranch        *b_trig_L2_idscan_isomuon_param_a0;   //!
   TBranch        *b_trig_L2_idscan_isomuon_param_z0;   //!
   TBranch        *b_trig_L2_idscan_isomuon_param_phi0;   //!
   TBranch        *b_trig_L2_idscan_isomuon_param_eta;   //!
   TBranch        *b_trig_L2_idscan_isomuon_param_pt;   //!
   TBranch        *b_trig_roidescriptor_forID_n;   //!
   TBranch        *b_trig_roidescriptor_forID_phi;   //!
   TBranch        *b_trig_roidescriptor_forID_eta;   //!
   TBranch        *b_trig_roidescriptor_forID_zed0;   //!
   TBranch        *b_trig_RoI_L2_mu_n;   //!
   TBranch        *b_trig_RoI_L2_mu_type;   //!
   TBranch        *b_trig_RoI_L2_mu_lastStep;   //!
   TBranch        *b_trig_RoI_L2_mu_TrigRoiDescriptor_forID;   //!
   TBranch        *b_trig_RoI_L2_mu_TrigRoiDescriptor_forIDStatus;   //!
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
   TBranch        *b_trig_L1_esum_thrNames;   //!
   TBranch        *b_trig_L1_esum_ExMiss;   //!
   TBranch        *b_trig_L1_esum_EyMiss;   //!
   TBranch        *b_trig_L1_esum_energyT;   //!
   TBranch        *b_trig_L1_esum_overflowX;   //!
   TBranch        *b_trig_L1_esum_overflowY;   //!
   TBranch        *b_trig_L1_esum_overflowT;   //!
   TBranch        *b_trig_L1_esum_RoIWord0;   //!
   TBranch        *b_trig_L1_esum_RoIWord1;   //!
   TBranch        *b_trig_L1_esum_RoIWord2;   //!
   TBranch        *b_trig_L2_met_MEx;   //!
   TBranch        *b_trig_L2_met_MEy;   //!
   TBranch        *b_trig_L2_met_MEz;   //!
   TBranch        *b_trig_L2_met_sumEt;   //!
   TBranch        *b_trig_L2_met_sumE;   //!
   TBranch        *b_trig_L2_met_flag;   //!
   TBranch        *b_trig_L2_met_nameOfComponent;   //!
   TBranch        *b_trig_L2_met_MExComponent;   //!
   TBranch        *b_trig_L2_met_MEyComponent;   //!
   TBranch        *b_trig_L2_met_MEzComponent;   //!
   TBranch        *b_trig_L2_met_sumEtComponent;   //!
   TBranch        *b_trig_L2_met_sumEComponent;   //!
   TBranch        *b_trig_L2_met_componentCalib0;   //!
   TBranch        *b_trig_L2_met_componentCalib1;   //!
   TBranch        *b_trig_L2_met_sumOfSigns;   //!
   TBranch        *b_trig_L2_met_usedChannels;   //!
   TBranch        *b_trig_L2_met_status;   //!
   TBranch        *b_trig_EF_met_MEx;   //!
   TBranch        *b_trig_EF_met_MEy;   //!
   TBranch        *b_trig_EF_met_MEz;   //!
   TBranch        *b_trig_EF_met_sumEt;   //!
   TBranch        *b_trig_EF_met_sumE;   //!
   TBranch        *b_trig_EF_met_flag;   //!
   TBranch        *b_trig_EF_met_nameOfComponent;   //!
   TBranch        *b_trig_EF_met_MExComponent;   //!
   TBranch        *b_trig_EF_met_MEyComponent;   //!
   TBranch        *b_trig_EF_met_MEzComponent;   //!
   TBranch        *b_trig_EF_met_sumEtComponent;   //!
   TBranch        *b_trig_EF_met_sumEComponent;   //!
   TBranch        *b_trig_EF_met_componentCalib0;   //!
   TBranch        *b_trig_EF_met_componentCalib1;   //!
   TBranch        *b_trig_EF_met_sumOfSigns;   //!
   TBranch        *b_trig_EF_met_usedChannels;   //!
   TBranch        *b_trig_EF_met_status;   //!

   physics(TTree *tree=0);
   virtual ~physics();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

 private :

  virtual void fill();

   
};

#endif

#ifdef physics_cxx
physics::physics(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("physics.root");
      if (!f) {
         f = new TFile("physics.root");
      }
      tree = (TTree*)gDirectory->Get("physics");

   }
   Init(tree);
}

physics::~physics()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}


void physics::fill() {
  cout << "this method is virtual" << endl;

}

#endif // #ifdef physics_cxx
