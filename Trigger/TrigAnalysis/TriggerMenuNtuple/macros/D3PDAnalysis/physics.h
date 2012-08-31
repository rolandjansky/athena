/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu May 27 05:29:11 2010 by ROOT version 5.22/00i
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
   Int_t           el_n;
   std::vector<float>   *el_E;
   std::vector<float>   *el_Et;
   std::vector<float>   *el_pt;
   std::vector<float>   *el_m;
   std::vector<float>   *el_eta;
   std::vector<float>   *el_phi;
   std::vector<float>   *el_px;
   std::vector<float>   *el_py;
   std::vector<float>   *el_pz;
   std::vector<float>   *el_charge;
   std::vector<int>     *el_author;
   std::vector<unsigned int> *el_isEM;
   std::vector<int>     *el_convFlag;
   std::vector<int>     *el_isConv;
   std::vector<int>     *el_nConv;
   std::vector<int>     *el_nSingleTrackConv;
   std::vector<int>     *el_nDoubleTrackConv;
   std::vector<int>     *el_type;
   std::vector<int>     *el_origin;
   std::vector<float>   *el_truth_E;
   std::vector<float>   *el_truth_pt;
   std::vector<float>   *el_truth_eta;
   std::vector<float>   *el_truth_phi;
   std::vector<int>     *el_truth_type;
   std::vector<int>     *el_truth_status;
   std::vector<int>     *el_truth_barcode;
   std::vector<int>     *el_truth_mothertype;
   std::vector<int>     *el_truth_motherbarcode;
   std::vector<int>     *el_truth_hasHardBrem;
   std::vector<int>     *el_truth_matched;
   std::vector<int>     *el_loose;
   std::vector<int>     *el_medium;
   std::vector<int>     *el_mediumIso;
   std::vector<int>     *el_tight;
   std::vector<int>     *el_tightIso;
   std::vector<float>   *el_Ethad;
   std::vector<float>   *el_Ethad1;
   std::vector<float>   *el_f1;
   std::vector<float>   *el_f1core;
   std::vector<float>   *el_Emins1;
   std::vector<float>   *el_fside;
   std::vector<float>   *el_Emax2;
   std::vector<float>   *el_ws3;
   std::vector<float>   *el_wstot;
   std::vector<float>   *el_emaxs1;
   std::vector<float>   *el_deltaEs;
   std::vector<float>   *el_E233;
   std::vector<float>   *el_E237;
   std::vector<float>   *el_E277;
   std::vector<float>   *el_weta2;
   std::vector<float>   *el_f3;
   std::vector<float>   *el_f3core;
   std::vector<float>   *el_rphiallcalo;
   std::vector<float>   *el_Etcone45;
   std::vector<float>   *el_Etcone20;
   std::vector<float>   *el_Etcone30;
   std::vector<float>   *el_Etcone40;
   std::vector<float>   *el_ptcone30;
   std::vector<float>   *el_convanglematch;
   std::vector<float>   *el_convtrackmatch;
   std::vector<int>     *el_hasconv;
   std::vector<float>   *el_convvtxx;
   std::vector<float>   *el_convvtxy;
   std::vector<float>   *el_convvtxz;
   std::vector<float>   *el_Rconv;
   std::vector<float>   *el_zconv;
   std::vector<float>   *el_convvtxchi2;
   std::vector<float>   *el_pt1conv;
   std::vector<int>     *el_convtrk1nBLHits;
   std::vector<int>     *el_convtrk1nPixHits;
   std::vector<int>     *el_convtrk1nSCTHits;
   std::vector<int>     *el_convtrk1nTRTHits;
   std::vector<float>   *el_pt2conv;
   std::vector<int>     *el_convtrk2nBLHits;
   std::vector<int>     *el_convtrk2nPixHits;
   std::vector<int>     *el_convtrk2nSCTHits;
   std::vector<int>     *el_convtrk2nTRTHits;
   std::vector<float>   *el_ptconv;
   std::vector<float>   *el_pzconv;
   std::vector<float>   *el_pos7;
   std::vector<float>   *el_etacorrmag;
   std::vector<float>   *el_deltaeta1;
   std::vector<float>   *el_deltaeta2;
   std::vector<float>   *el_deltaphi2;
   std::vector<float>   *el_deltaphiRescaled;
   std::vector<float>   *el_expectHitInBLayer;
   std::vector<float>   *el_reta;
   std::vector<float>   *el_rphi;
   std::vector<float>   *el_EtringnoisedR03sig2;
   std::vector<float>   *el_EtringnoisedR03sig3;
   std::vector<float>   *el_EtringnoisedR03sig4;
   std::vector<double>  *el_isolationlikelihoodjets;
   std::vector<double>  *el_isolationlikelihoodhqelectrons;
   std::vector<double>  *el_electronweight;
   std::vector<double>  *el_electronbgweight;
   std::vector<double>  *el_softeweight;
   std::vector<double>  *el_softebgweight;
   std::vector<double>  *el_neuralnet;
   std::vector<double>  *el_Hmatrix;
   std::vector<double>  *el_Hmatrix5;
   std::vector<double>  *el_adaboost;
   std::vector<double>  *el_softeneuralnet;
   std::vector<float>   *el_zvertex;
   std::vector<float>   *el_errz;
   std::vector<float>   *el_etap;
   std::vector<float>   *el_depth;
   std::vector<float>   *el_breminvpt;
   std::vector<float>   *el_bremradius;
   std::vector<float>   *el_bremx;
   std::vector<float>   *el_bremclusterradius;
   std::vector<float>   *el_breminvpterr;
   std::vector<float>   *el_bremtrackauthor;
   std::vector<float>   *el_hasbrem;
   std::vector<float>   *el_bremdeltaqoverp;
   std::vector<float>   *el_bremmaterialtraversed;
   std::vector<float>   *el_refittedtrackqoverp;
   std::vector<float>   *el_refittedtrackd0;
   std::vector<float>   *el_refittedtrackz0;
   std::vector<float>   *el_refittedtracktheta;
   std::vector<float>   *el_refittedtrackphi;
   std::vector<float>   *el_Es0;
   std::vector<float>   *el_etas0;
   std::vector<float>   *el_phis0;
   std::vector<float>   *el_Es1;
   std::vector<float>   *el_etas1;
   std::vector<float>   *el_phis1;
   std::vector<float>   *el_Es2;
   std::vector<float>   *el_etas2;
   std::vector<float>   *el_phis2;
   std::vector<float>   *el_Es3;
   std::vector<float>   *el_etas3;
   std::vector<float>   *el_phis3;
   std::vector<float>   *el_cl_E;
   std::vector<float>   *el_cl_pt;
   std::vector<float>   *el_cl_eta;
   std::vector<float>   *el_cl_phi;
   std::vector<float>   *el_firstEdens;
   std::vector<float>   *el_cellmaxfrac;
   std::vector<float>   *el_longitudinal;
   std::vector<float>   *el_secondlambda;
   std::vector<float>   *el_lateral;
   std::vector<float>   *el_secondR;
   std::vector<float>   *el_centerlambda;
   std::vector<float>   *el_rawcl_Es0;
   std::vector<float>   *el_rawcl_etas0;
   std::vector<float>   *el_rawcl_phis0;
   std::vector<float>   *el_rawcl_Es1;
   std::vector<float>   *el_rawcl_etas1;
   std::vector<float>   *el_rawcl_phis1;
   std::vector<float>   *el_rawcl_Es2;
   std::vector<float>   *el_rawcl_etas2;
   std::vector<float>   *el_rawcl_phis2;
   std::vector<float>   *el_rawcl_Es3;
   std::vector<float>   *el_rawcl_etas3;
   std::vector<float>   *el_rawcl_phis3;
   std::vector<float>   *el_rawcl_E;
   std::vector<float>   *el_rawcl_pt;
   std::vector<float>   *el_rawcl_eta;
   std::vector<float>   *el_rawcl_phi;
   std::vector<float>   *el_refittedtrackcovd0;
   std::vector<float>   *el_refittedtrackcovz0;
   std::vector<float>   *el_refittedtrackcovphi;
   std::vector<float>   *el_refittedtrackcovtheta;
   std::vector<float>   *el_refittedtrackcovqoverp;
   std::vector<float>   *el_refittedtrackcovd0z0;
   std::vector<float>   *el_refittedtrackcovz0phi;
   std::vector<float>   *el_refittedtrackcovz0theta;
   std::vector<float>   *el_refittedtrackcovz0qoverp;
   std::vector<float>   *el_refittedtrackcovd0phi;
   std::vector<float>   *el_refittedtrackcovd0theta;
   std::vector<float>   *el_refittedtrackcovd0qoverp;
   std::vector<float>   *el_refittedtrackcovphitheta;
   std::vector<float>   *el_refittedtrackcovphiqoverp;
   std::vector<float>   *el_refittedtrackcovthetaqoverp;
   std::vector<float>   *el_trackd0;
   std::vector<float>   *el_trackz0;
   std::vector<float>   *el_trackphi;
   std::vector<float>   *el_tracktheta;
   std::vector<float>   *el_trackqoverp;
   std::vector<float>   *el_trackpt;
   std::vector<float>   *el_tracketa;
   std::vector<float>   *el_trackcov_d0;
   std::vector<float>   *el_trackcov_z0;
   std::vector<float>   *el_trackcov_phi;
   std::vector<float>   *el_trackcov_theta;
   std::vector<float>   *el_trackcov_qoverp;
   std::vector<float>   *el_trackcov_d0_z0;
   std::vector<float>   *el_trackcov_d0_phi;
   std::vector<float>   *el_trackcov_d0_theta;
   std::vector<float>   *el_trackcov_d0_qoverp;
   std::vector<float>   *el_trackcov_z0_phi;
   std::vector<float>   *el_trackcov_z0_theta;
   std::vector<float>   *el_trackcov_z0_qoverp;
   std::vector<float>   *el_trackcov_phi_theta;
   std::vector<float>   *el_trackcov_phi_qoverp;
   std::vector<float>   *el_trackcov_theta_qoverp;
   std::vector<float>   *el_trackfitchi2;
   std::vector<int>     *el_trackfitndof;
   std::vector<int>     *el_nBLHits;
   std::vector<int>     *el_nPixHits;
   std::vector<int>     *el_nSCTHits;
   std::vector<int>     *el_nTRTHits;
   std::vector<int>     *el_nPixHoles;
   std::vector<int>     *el_nSCTHoles;
   std::vector<int>     *el_nBLSharedHits;
   std::vector<int>     *el_nPixSharedHits;
   std::vector<int>     *el_nSCTSharedHits;
   std::vector<int>     *el_nTRTHighTHits;
   std::vector<int>     *el_nTRTOutliers;
   std::vector<int>     *el_nTRTHighTOutliers;
   std::vector<int>     *el_nSiHits;
   std::vector<float>   *el_TRTHighTHitsRatio;
   std::vector<float>   *el_pixeldEdx;
   std::vector<float>   *el_vertx;
   std::vector<float>   *el_verty;
   std::vector<float>   *el_vertz;
   std::vector<int>     *el_hastrack;
   std::vector<float>   *el_deltaEmax2;
   std::vector<float>   *el_trackd0beam;
   std::vector<unsigned int> *el_isIso;
   std::vector<float>   *el_jet_dr;
   std::vector<float>   *el_jet_E;
   std::vector<float>   *el_jet_pt;
   std::vector<float>   *el_jet_m;
   std::vector<float>   *el_jet_eta;
   std::vector<float>   *el_jet_phi;
   std::vector<float>   *el_jet_truth_dr;
   std::vector<float>   *el_jet_truth_E;
   std::vector<float>   *el_jet_truth_pt;
   std::vector<float>   *el_jet_truth_m;
   std::vector<float>   *el_jet_truth_eta;
   std::vector<float>   *el_jet_truth_phi;
   std::vector<int>     *el_jet_truth_matched;
   std::vector<int>     *el_jet_matched;
   std::vector<float>   *el_EF_dr;
   std::vector<int>     *el_EF_index;
   std::vector<float>   *el_L2_dr;
   std::vector<int>     *el_L2_index;
   std::vector<float>   *el_L1_dr;
   std::vector<int>     *el_L1_index;
   std::vector<float>   *el_EF_E;
   std::vector<float>   *el_EF_Et;
   std::vector<float>   *el_EF_pt;
   std::vector<float>   *el_EF_eta;
   std::vector<float>   *el_EF_phi;
   std::vector<float>   *el_EF_charge;
   std::vector<float>   *el_EF_Ethad;
   std::vector<float>   *el_EF_Ethad1;
   std::vector<float>   *el_EF_f1;
   std::vector<float>   *el_EF_Emins1;
   std::vector<float>   *el_EF_fside;
   std::vector<float>   *el_EF_Emax2;
   std::vector<float>   *el_EF_ws3;
   std::vector<float>   *el_EF_wstot;
   std::vector<float>   *el_EF_E233;
   std::vector<float>   *el_EF_E237;
   std::vector<float>   *el_EF_E277;
   std::vector<float>   *el_EF_weta2;
   std::vector<float>   *el_EF_rphiallcalo;
   std::vector<float>   *el_EF_deltaeta1;
   std::vector<float>   *el_EF_deltaeta2;
   std::vector<float>   *el_EF_deltaphi2;
   std::vector<float>   *el_EF_expectHitInBLayer;
   std::vector<float>   *el_EF_Etcone45;
   std::vector<float>   *el_EF_Etcone20;
   std::vector<float>   *el_EF_Etcone30;
   std::vector<float>   *el_EF_Etcone40;
   std::vector<float>   *el_EF_etacorrmag;
   std::vector<float>   *el_EF_zvertex;
   std::vector<float>   *el_EF_errz;
   std::vector<float>   *el_EF_trackd0;
   std::vector<float>   *el_EF_trackz0;
   std::vector<float>   *el_EF_trackphi;
   std::vector<float>   *el_EF_tracktheta;
   std::vector<float>   *el_EF_trackqoverp;
   std::vector<float>   *el_EF_trackpt;
   std::vector<float>   *el_EF_tracketa;
   std::vector<int>     *el_EF_nBLHits;
   std::vector<int>     *el_EF_nPixHits;
   std::vector<int>     *el_EF_nSCTHits;
   std::vector<int>     *el_EF_nTRTHits;
   std::vector<int>     *el_EF_nTRTHighTHits;
   std::vector<int>     *el_EF_nSiHits;
   std::vector<float>   *el_EF_TRTHighTHitsRatio;
   std::vector<float>   *el_EF_pixeldEdx;
   std::vector<int>     *el_EF_hastrack;
   std::vector<int>     *el_EF_matched;
   std::vector<float>   *el_L2_E;
   std::vector<float>   *el_L2_Et;
   std::vector<float>   *el_L2_pt;
   std::vector<float>   *el_L2_eta;
   std::vector<float>   *el_L2_phi;
   std::vector<float>   *el_L2_charge;
   std::vector<float>   *el_L2_Ethad1;
   std::vector<float>   *el_L2_reta;
   std::vector<float>   *el_L2_Eratio;
   std::vector<float>   *el_L2_trackclusterdeta;
   std::vector<float>   *el_L2_trackclusterdphi;
   std::vector<float>   *el_L2_Etoverpt;
   std::vector<float>   *el_L2_trackpt;
   std::vector<int>     *el_L2_trackalgo;
   std::vector<float>   *el_L2_tracketa;
   std::vector<float>   *el_L2_trackd0;
   std::vector<float>   *el_L2_trackz0;
   std::vector<float>   *el_L2_tracktheta;
   std::vector<float>   *el_L2_trackphi;
   std::vector<float>   *el_L2_tracketaatcalo;
   std::vector<float>   *el_L2_trackphiatcalo;
   std::vector<float>   *el_L2_errpt;
   std::vector<float>   *el_L2_erreta;
   std::vector<float>   *el_L2_errphi;
   std::vector<float>   *el_L2_E237;
   std::vector<float>   *el_L2_E277;
   std::vector<float>   *el_L2_fside;
   std::vector<float>   *el_L2_weta2;
   std::vector<float>   *el_L2_Emaxs1;
   std::vector<float>   *el_L2_Emax2;
   std::vector<int>     *el_L2_matched;
   std::vector<float>   *el_L1_eta;
   std::vector<float>   *el_L1_phi;
   std::vector<float>   *el_L1_EMisol;
   std::vector<float>   *el_L1_hadisol;
   std::vector<float>   *el_L1_hadcore;
   std::vector<float>   *el_L1_pt;
   std::vector<float>   *el_L1_Et;
   std::vector<int>     *el_L1_matched;
   Bool_t          EF_2e3_NoCut;
   Bool_t          EF_2e3_loose;
   Bool_t          EF_2e3_medium;
   Bool_t          EF_2e5_loose;
   Bool_t          EF_2e5_medium;
   Bool_t          EF_2e6_nocut;
   Bool_t          EF_2g10_nocut;
   Bool_t          EF_2g3_loose;
   Bool_t          EF_2g3_nocut;
   Bool_t          EF_2g5_loose;
   Bool_t          EF_2mu10;
   Bool_t          EF_2mu4;
   Bool_t          EF_2mu4_DiMu;
   Bool_t          EF_2mu4_DiMu_SiTrk;
   Bool_t          EF_2mu4_DiMu_noOS;
   Bool_t          EF_2mu4_DiMu_noOS_passL2;
   Bool_t          EF_2mu4_DiMu_noVtx_noOS;
   Bool_t          EF_2mu4_DiMu_noVtx_noOS_passL2;
   Bool_t          EF_2mu4_MSonly;
   Bool_t          EF_2mu6;
   Bool_t          EF_2mu6_DiMu;
   Bool_t          EF_2mu6_DiMu_noVtx_noOS;
   Bool_t          EF_e10_loose;
   Bool_t          EF_e10_loose_IdScan;
   Bool_t          EF_e10_loose_TRT;
   Bool_t          EF_e10_loose_passEF;
   Bool_t          EF_e10_loose_passL2;
   Bool_t          EF_e10_medium;
   Bool_t          EF_e10_medium_IdScan;
   Bool_t          EF_e10_medium_TRT;
   Bool_t          EF_e10i_loose;
   Bool_t          EF_e15_loose;
   Bool_t          EF_e15i_loose;
   Bool_t          EF_e20_loose_NoIDTrkCut;
   Bool_t          EF_e3_NoCut;
   Bool_t          EF_e3_NoCut_IdScan;
   Bool_t          EF_e3_NoCut_TRT;
   Bool_t          EF_e3_loose;
   Bool_t          EF_e3_loose_IdScan;
   Bool_t          EF_e3_loose_TRT;
   Bool_t          EF_e3_medium;
   Bool_t          EF_e3_medium_IdScan;
   Bool_t          EF_e3_medium_TRT;
   Bool_t          EF_e5_EFfullcalo;
   Bool_t          EF_e5_NoCut;
   Bool_t          EF_e5_NoCut_FwdBackTrk;
   Bool_t          EF_e5_NoCut_IdScan;
   Bool_t          EF_e5_NoCut_L2SW;
   Bool_t          EF_e5_NoCut_Ringer;
   Bool_t          EF_e5_NoCut_SiTrk;
   Bool_t          EF_e5_NoCut_TRT;
   Bool_t          EF_e5_NoCut_cells;
   Bool_t          EF_e5_NoCut_cosmic;
   Bool_t          EF_e5_NoCut_firstempty;
   Bool_t          EF_e5_NoCut_unpaired;
   Bool_t          EF_e5_loose;
   Bool_t          EF_e5_loose_IdScan;
   Bool_t          EF_e5_loose_NoIDTrkCut;
   Bool_t          EF_e5_loose_TRT;
   Bool_t          EF_e5_medium;
   Bool_t          EF_e5_medium_IdScan;
   Bool_t          EF_e5_medium_MV;
   Bool_t          EF_e5_medium_TRT;
   Bool_t          EF_e6_loose;
   Bool_t          EF_g10_loose;
   Bool_t          EF_g10_loose_larcalib;
   Bool_t          EF_g10i_loose;
   Bool_t          EF_g15_loose;
   Bool_t          EF_g15i_loose;
   Bool_t          EF_g3_loose;
   Bool_t          EF_g3_nocut;
   Bool_t          EF_g3_nocut_larcalib;
   Bool_t          EF_g3_nocut_unpaired;
   Bool_t          EF_g5_EFfullcalo;
   Bool_t          EF_g5_loose;
   Bool_t          EF_g5_loose_larcalib;
   Bool_t          EF_g5_nocut;
   Bool_t          EF_g5_nocut_cosmic;
   Bool_t          EF_g5_nocut_firstempty;
   Bool_t          EF_g5_nocut_unpaired;
   Bool_t          EF_g6_loose;
   Bool_t          EF_mu0_missingRoi;
   Bool_t          EF_mu0_rpcOnly;
   Bool_t          EF_mu10;
   Bool_t          EF_mu10_L2MSonly_EFFS_passL2;
   Bool_t          EF_mu10_MG;
   Bool_t          EF_mu10_MSonly;
   Bool_t          EF_mu10_MSonly_EFFS_passL2;
   Bool_t          EF_mu10_MSonly_cosmic;
   Bool_t          EF_mu10_NoIDTrkCut;
   Bool_t          EF_mu10_SiTrk;
   Bool_t          EF_mu10_cosmic;
   Bool_t          EF_mu10_muCombTag;
   Bool_t          EF_mu10_muCombTag_SiTrk;
   Bool_t          EF_mu10_passHLT;
   Bool_t          EF_mu10i_loose;
   Bool_t          EF_mu13;
   Bool_t          EF_mu15;
   Bool_t          EF_mu20;
   Bool_t          EF_mu20_MSonly;
   Bool_t          EF_mu20_passHLT;
   Bool_t          EF_mu4;
   Bool_t          EF_mu4_DiMu;
   Bool_t          EF_mu4_DiMu_FS;
   Bool_t          EF_mu4_DiMu_FS_noOS;
   Bool_t          EF_mu4_Jpsie5e3;
   Bool_t          EF_mu4_Jpsie5e3_FS;
   Bool_t          EF_mu4_Jpsie5e3_SiTrk;
   Bool_t          EF_mu4_Jpsie5e3_SiTrk_FS;
   Bool_t          EF_mu4_L2MSonly_EFFS_passL2;
   Bool_t          EF_mu4_MG;
   Bool_t          EF_mu4_MSonly;
   Bool_t          EF_mu4_MSonly_EFFS_passL2;
   Bool_t          EF_mu4_MSonly_MB2_noL2_EFFS;
   Bool_t          EF_mu4_MSonly_Trk_Jpsi_loose;
   Bool_t          EF_mu4_MSonly_Trk_Upsi_loose_FS;
   Bool_t          EF_mu4_MSonly_cosmic;
   Bool_t          EF_mu4_MSonly_firstempty;
   Bool_t          EF_mu4_MV;
   Bool_t          EF_mu4_NoIDTrkCut;
   Bool_t          EF_mu4_SiTrk;
   Bool_t          EF_mu4_Trk_Jpsi_loose;
   Bool_t          EF_mu4_Trk_Upsi_loose_FS;
   Bool_t          EF_mu4_comm;
   Bool_t          EF_mu4_comm_MSonly;
   Bool_t          EF_mu4_comm_MSonly_cosmic;
   Bool_t          EF_mu4_comm_MSonly_firstempty;
   Bool_t          EF_mu4_comm_cosmic;
   Bool_t          EF_mu4_comm_firstempty;
   Bool_t          EF_mu4_cosmic;
   Bool_t          EF_mu4_firstempty;
   Bool_t          EF_mu4_j10_matched;
   Bool_t          EF_mu4_j20_matched;
   Bool_t          EF_mu4_j5_matched;
   Bool_t          EF_mu4_mu6;
   Bool_t          EF_mu4_muCombTag;
   Bool_t          EF_mu4_muCombTag_SiTrk;
   Bool_t          EF_mu4_passHLT;
   Bool_t          EF_mu4_tile;
   Bool_t          EF_mu4_tile_SiTrk;
   Bool_t          EF_mu4_trod;
   Bool_t          EF_mu4_trod_SiTrk;
   Bool_t          EF_mu4_unpaired;
   Bool_t          EF_mu4mu6_DiMu;
   Bool_t          EF_mu6;
   Bool_t          EF_mu6_DiMu;
   Bool_t          EF_mu6_DiMu_FS;
   Bool_t          EF_mu6_DiMu_FS_noOS;
   Bool_t          EF_mu6_DsPhiPi;
   Bool_t          EF_mu6_Jpsie5e3;
   Bool_t          EF_mu6_Jpsie5e3_FS;
   Bool_t          EF_mu6_Jpsie5e3_SiTrk;
   Bool_t          EF_mu6_Jpsie5e3_SiTrk_FS;
   Bool_t          EF_mu6_L2MSonly_EFFS_passL2;
   Bool_t          EF_mu6_MG;
   Bool_t          EF_mu6_MSonly;
   Bool_t          EF_mu6_MSonly_EFFS_passL2;
   Bool_t          EF_mu6_MSonly_cosmic;
   Bool_t          EF_mu6_MSonly_firstempty;
   Bool_t          EF_mu6_MSonly_unpaired;
   Bool_t          EF_mu6_SiTrk;
   Bool_t          EF_mu6_Trk_Jpsi_loose;
   Bool_t          EF_mu6_Trk_Jpsi_loose_FS;
   Bool_t          EF_mu6_Trk_Upsi_loose_FS;
   Bool_t          EF_mu6_cosmic;
   Bool_t          EF_mu6_firstempty;
   Bool_t          EF_mu6_muCombTag;
   Bool_t          EF_mu6_muCombTag_SiTrk;
   Bool_t          EF_mu6_passHLT;
   Bool_t          EF_mu6_unpaired;
   Bool_t          EF_tau12_loose_xe20;
   Bool_t          EF_tau16_loose_xe20;
   Bool_t          EF_tauNoCut_hasTrk_xe20;
   Bool_t          EF_xe20;
   Bool_t          EF_xe20_FEB;
   Bool_t          EF_xe20_allCells;
   Bool_t          EF_xe20_noMu;
   Bool_t          EF_xe25_noMu;
   Bool_t          EF_xe30_allL1;
   Bool_t          EF_xe30_allL1_FEB;
   Bool_t          EF_xe30_allL1_allCells;
   Bool_t          EF_xe30_noMu;
   Bool_t          EF_xe30_unbiased;
   Bool_t          EF_xe30_unbiased_FEB;
   Bool_t          EF_xe30_unbiased_allCells;
   Bool_t          EF_xe35_noMu;
   Bool_t          EF_xe40_noMu;
   Bool_t          EF_xe45_noMu;
   Bool_t          EF_xe60_medium_noMu;
   Bool_t          EF_xe70_medium_noMu;
   Bool_t          EF_xe80_medium_noMu;
   Bool_t          L1_2MU0;
   Bool_t          L1_2MU0_MU6;
   Bool_t          L1_2MU10;
   Bool_t          L1_2MU6;
   Bool_t          L1_EM10;
   Bool_t          L1_EM10I;
   Bool_t          L1_EM14;
   Bool_t          L1_EM2;
   Bool_t          L1_EM2_UNPAIRED;
   Bool_t          L1_EM3;
   Bool_t          L1_EM3_EMPTY;
   Bool_t          L1_EM3_FIRSTEMPTY;
   Bool_t          L1_EM3_MV;
   Bool_t          L1_EM3_UNPAIRED;
   Bool_t          L1_EM4;
   Bool_t          L1_EM5;
   Bool_t          L1_EM5I;
   Bool_t          L1_MU0;
   Bool_t          L1_MU0_COMM;
   Bool_t          L1_MU0_COMM_EMPTY;
   Bool_t          L1_MU0_COMM_FIRSTEMPTY;
   Bool_t          L1_MU0_EM3;
   Bool_t          L1_MU0_EMPTY;
   Bool_t          L1_MU0_FIRSTEMPTY;
   Bool_t          L1_MU0_J10;
   Bool_t          L1_MU0_J15;
   Bool_t          L1_MU0_J5;
   Bool_t          L1_MU0_MV;
   Bool_t          L1_MU0_UNPAIRED;
   Bool_t          L1_MU10;
   Bool_t          L1_MU10_EMPTY;
   Bool_t          L1_MU15;
   Bool_t          L1_MU20;
   Bool_t          L1_MU6;
   Bool_t          L1_MU6_EM3;
   Bool_t          L1_MU6_EMPTY;
   Bool_t          L1_MU6_FIRSTEMPTY;
   Bool_t          L1_MU6_J5;
   Bool_t          L1_MU6_UNPAIRED;
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
   Bool_t          L2_2e3_NoCut;
   Bool_t          L2_2e3_loose;
   Bool_t          L2_2e3_medium;
   Bool_t          L2_2e5_loose;
   Bool_t          L2_2e5_medium;
   Bool_t          L2_2e6_nocut;
   Bool_t          L2_2g10_nocut;
   Bool_t          L2_2g3_loose;
   Bool_t          L2_2g3_nocut;
   Bool_t          L2_2g5_loose;
   Bool_t          L2_2mu10;
   Bool_t          L2_2mu4;
   Bool_t          L2_2mu4_DiMu;
   Bool_t          L2_2mu4_DiMu_SiTrk;
   Bool_t          L2_2mu4_DiMu_noOS;
   Bool_t          L2_2mu4_DiMu_noOS_passL2;
   Bool_t          L2_2mu4_DiMu_noVtx_noOS;
   Bool_t          L2_2mu4_DiMu_noVtx_noOS_passL2;
   Bool_t          L2_2mu4_MSonly;
   Bool_t          L2_2mu6;
   Bool_t          L2_2mu6_DiMu;
   Bool_t          L2_2mu6_DiMu_noVtx_noOS;
   Bool_t          L2_e10_loose;
   Bool_t          L2_e10_loose_IdScan;
   Bool_t          L2_e10_loose_TRT;
   Bool_t          L2_e10_loose_passEF;
   Bool_t          L2_e10_loose_passL2;
   Bool_t          L2_e10_medium;
   Bool_t          L2_e10_medium_IdScan;
   Bool_t          L2_e10_medium_TRT;
   Bool_t          L2_e10i_loose;
   Bool_t          L2_e15_loose;
   Bool_t          L2_e15i_loose;
   Bool_t          L2_e20_loose_NoIDTrkCut;
   Bool_t          L2_e3_NoCut;
   Bool_t          L2_e3_NoCut_IdScan;
   Bool_t          L2_e3_NoCut_TRT;
   Bool_t          L2_e3_loose;
   Bool_t          L2_e3_loose_IdScan;
   Bool_t          L2_e3_loose_TRT;
   Bool_t          L2_e3_medium;
   Bool_t          L2_e3_medium_IdScan;
   Bool_t          L2_e3_medium_TRT;
   Bool_t          L2_e5_EFfullcalo;
   Bool_t          L2_e5_NoCut;
   Bool_t          L2_e5_NoCut_FwdBackTrk;
   Bool_t          L2_e5_NoCut_IdScan;
   Bool_t          L2_e5_NoCut_L2SW;
   Bool_t          L2_e5_NoCut_Ringer;
   Bool_t          L2_e5_NoCut_SiTrk;
   Bool_t          L2_e5_NoCut_TRT;
   Bool_t          L2_e5_NoCut_cells;
   Bool_t          L2_e5_NoCut_cosmic;
   Bool_t          L2_e5_NoCut_firstempty;
   Bool_t          L2_e5_NoCut_unpaired;
   Bool_t          L2_e5_loose;
   Bool_t          L2_e5_loose_IdScan;
   Bool_t          L2_e5_loose_NoIDTrkCut;
   Bool_t          L2_e5_loose_TRT;
   Bool_t          L2_e5_medium;
   Bool_t          L2_e5_medium_IdScan;
   Bool_t          L2_e5_medium_MV;
   Bool_t          L2_e5_medium_TRT;
   Bool_t          L2_e6_loose;
   Bool_t          L2_g10_loose;
   Bool_t          L2_g10i_loose;
   Bool_t          L2_g15_loose;
   Bool_t          L2_g15i_loose;
   Bool_t          L2_g3_loose;
   Bool_t          L2_g3_nocut;
   Bool_t          L2_g3_nocut_unpaired;
   Bool_t          L2_g5_EFfullcalo;
   Bool_t          L2_g5_loose;
   Bool_t          L2_g5_nocut;
   Bool_t          L2_g5_nocut_cosmic;
   Bool_t          L2_g5_nocut_firstempty;
   Bool_t          L2_g5_nocut_unpaired;
   Bool_t          L2_g6_loose;
   Bool_t          L2_mu0_missingRoi;
   Bool_t          L2_mu0_rpcOnly;
   Bool_t          L2_mu10;
   Bool_t          L2_mu10_L2MSonly_EFFS_passL2;
   Bool_t          L2_mu10_MG;
   Bool_t          L2_mu10_MSonly;
   Bool_t          L2_mu10_MSonly_EFFS_passL2;
   Bool_t          L2_mu10_MSonly_cosmic;
   Bool_t          L2_mu10_NoIDTrkCut;
   Bool_t          L2_mu10_SiTrk;
   Bool_t          L2_mu10_cosmic;
   Bool_t          L2_mu10_muCombTag;
   Bool_t          L2_mu10_muCombTag_SiTrk;
   Bool_t          L2_mu10_passHLT;
   Bool_t          L2_mu10i_loose;
   Bool_t          L2_mu13;
   Bool_t          L2_mu15;
   Bool_t          L2_mu20;
   Bool_t          L2_mu20_MSonly;
   Bool_t          L2_mu20_passHLT;
   Bool_t          L2_mu4;
   Bool_t          L2_mu4_DiMu;
   Bool_t          L2_mu4_DiMu_FS;
   Bool_t          L2_mu4_DiMu_FS_noOS;
   Bool_t          L2_mu4_Jpsie5e3;
   Bool_t          L2_mu4_Jpsie5e3_FS;
   Bool_t          L2_mu4_Jpsie5e3_SiTrk;
   Bool_t          L2_mu4_Jpsie5e3_SiTrk_FS;
   Bool_t          L2_mu4_L2MSonly_EFFS_passL2;
   Bool_t          L2_mu4_MG;
   Bool_t          L2_mu4_MSonly;
   Bool_t          L2_mu4_MSonly_EFFS_passL2;
   Bool_t          L2_mu4_MSonly_MB2_noL2_EFFS;
   Bool_t          L2_mu4_MSonly_Trk_Jpsi_loose;
   Bool_t          L2_mu4_MSonly_Trk_Upsi_loose_FS;
   Bool_t          L2_mu4_MSonly_cosmic;
   Bool_t          L2_mu4_MSonly_firstempty;
   Bool_t          L2_mu4_MV;
   Bool_t          L2_mu4_NoIDTrkCut;
   Bool_t          L2_mu4_SiTrk;
   Bool_t          L2_mu4_Trk_Jpsi_loose;
   Bool_t          L2_mu4_Trk_Upsi_loose_FS;
   Bool_t          L2_mu4_comm;
   Bool_t          L2_mu4_comm_MSonly;
   Bool_t          L2_mu4_comm_MSonly_cosmic;
   Bool_t          L2_mu4_comm_MSonly_firstempty;
   Bool_t          L2_mu4_comm_cosmic;
   Bool_t          L2_mu4_comm_firstempty;
   Bool_t          L2_mu4_cosmic;
   Bool_t          L2_mu4_firstempty;
   Bool_t          L2_mu4_j10_matched;
   Bool_t          L2_mu4_j20_matched;
   Bool_t          L2_mu4_j5_matched;
   Bool_t          L2_mu4_mu6;
   Bool_t          L2_mu4_muCombTag;
   Bool_t          L2_mu4_muCombTag_SiTrk;
   Bool_t          L2_mu4_passHLT;
   Bool_t          L2_mu4_tile;
   Bool_t          L2_mu4_tile_SiTrk;
   Bool_t          L2_mu4_trod;
   Bool_t          L2_mu4_trod_SiTrk;
   Bool_t          L2_mu4_unpaired;
   Bool_t          L2_mu4mu6_DiMu;
   Bool_t          L2_mu6;
   Bool_t          L2_mu6_DiMu;
   Bool_t          L2_mu6_DiMu_FS;
   Bool_t          L2_mu6_DiMu_FS_noOS;
   Bool_t          L2_mu6_DsPhiPi;
   Bool_t          L2_mu6_Jpsie5e3;
   Bool_t          L2_mu6_Jpsie5e3_FS;
   Bool_t          L2_mu6_Jpsie5e3_SiTrk;
   Bool_t          L2_mu6_Jpsie5e3_SiTrk_FS;
   Bool_t          L2_mu6_L2MSonly_EFFS_passL2;
   Bool_t          L2_mu6_MG;
   Bool_t          L2_mu6_MSonly;
   Bool_t          L2_mu6_MSonly_EFFS_passL2;
   Bool_t          L2_mu6_MSonly_cosmic;
   Bool_t          L2_mu6_MSonly_firstempty;
   Bool_t          L2_mu6_MSonly_unpaired;
   Bool_t          L2_mu6_SiTrk;
   Bool_t          L2_mu6_Trk_Jpsi_loose;
   Bool_t          L2_mu6_Trk_Jpsi_loose_FS;
   Bool_t          L2_mu6_Trk_Upsi_loose_FS;
   Bool_t          L2_mu6_cosmic;
   Bool_t          L2_mu6_firstempty;
   Bool_t          L2_mu6_muCombTag;
   Bool_t          L2_mu6_muCombTag_SiTrk;
   Bool_t          L2_mu6_passHLT;
   Bool_t          L2_mu6_unpaired;
   Bool_t          L2_tau12_loose_xe20;
   Bool_t          L2_tau16_loose_xe20;
   Bool_t          L2_tauNoCut_hasTrk_xe20;
   Bool_t          L2_xe12;
   Bool_t          L2_xe12_FEB;
   Bool_t          L2_xe12_allCells;
   Bool_t          L2_xe12_noMu;
   Bool_t          L2_xe15_noMu;
   Bool_t          L2_xe20_noMu;
   Bool_t          L2_xe25_noMu;
   Bool_t          L2_xe30_allL1;
   Bool_t          L2_xe30_allL1_FEB;
   Bool_t          L2_xe30_allL1_allCells;
   Bool_t          L2_xe30_noMu;
   Bool_t          L2_xe30_unbiased;
   Bool_t          L2_xe30_unbiased_FEB;
   Bool_t          L2_xe30_unbiased_allCells;
   Bool_t          L2_xe35_noMu;
   Bool_t          L2_xe40_medium_noMu;
   Bool_t          L2_xe50_medium_noMu;
   Bool_t          L2_xe60_medium_noMu;
   Int_t           ph_n;
   std::vector<float>   *ph_E;
   std::vector<float>   *ph_Et;
   std::vector<float>   *ph_pt;
   std::vector<float>   *ph_m;
   std::vector<float>   *ph_eta;
   std::vector<float>   *ph_phi;
   std::vector<float>   *ph_px;
   std::vector<float>   *ph_py;
   std::vector<float>   *ph_pz;
   std::vector<int>     *ph_author;
   std::vector<int>     *ph_isRecovered;
   std::vector<unsigned int> *ph_isEM;
   std::vector<int>     *ph_convFlag;
   std::vector<int>     *ph_isConv;
   std::vector<int>     *ph_nConv;
   std::vector<int>     *ph_nSingleTrackConv;
   std::vector<int>     *ph_nDoubleTrackConv;
   std::vector<int>     *ph_type;
   std::vector<int>     *ph_origin;
   std::vector<float>   *ph_truth_deltaRRecPhoton;
   std::vector<float>   *ph_truth_E;
   std::vector<float>   *ph_truth_pt;
   std::vector<float>   *ph_truth_eta;
   std::vector<float>   *ph_truth_phi;
   std::vector<int>     *ph_truth_type;
   std::vector<int>     *ph_truth_status;
   std::vector<int>     *ph_truth_barcode;
   std::vector<int>     *ph_truth_mothertype;
   std::vector<int>     *ph_truth_motherbarcode;
   std::vector<int>     *ph_truth_matched;
   std::vector<int>     *ph_loose;
   std::vector<int>     *ph_tight;
   std::vector<int>     *ph_tightIso;
   std::vector<float>   *ph_Ethad;
   std::vector<float>   *ph_Ethad1;
   std::vector<float>   *ph_E033;
   std::vector<float>   *ph_f1;
   std::vector<float>   *ph_f1core;
   std::vector<float>   *ph_Emins1;
   std::vector<float>   *ph_fside;
   std::vector<float>   *ph_Emax2;
   std::vector<float>   *ph_ws3;
   std::vector<float>   *ph_wstot;
   std::vector<float>   *ph_E132;
   std::vector<float>   *ph_E1152;
   std::vector<float>   *ph_emaxs1;
   std::vector<float>   *ph_deltaEs;
   std::vector<float>   *ph_E233;
   std::vector<float>   *ph_E237;
   std::vector<float>   *ph_E277;
   std::vector<float>   *ph_weta2;
   std::vector<float>   *ph_f3;
   std::vector<float>   *ph_f3core;
   std::vector<float>   *ph_rphiallcalo;
   std::vector<float>   *ph_Etcone45;
   std::vector<float>   *ph_Etcone20;
   std::vector<float>   *ph_Etcone30;
   std::vector<float>   *ph_Etcone40;
   std::vector<float>   *ph_ptcone30;
   std::vector<float>   *ph_convanglematch;
   std::vector<float>   *ph_convtrackmatch;
   std::vector<int>     *ph_hasconv;
   std::vector<float>   *ph_convvtxx;
   std::vector<float>   *ph_convvtxy;
   std::vector<float>   *ph_convvtxz;
   std::vector<float>   *ph_Rconv;
   std::vector<float>   *ph_zconv;
   std::vector<float>   *ph_convvtxchi2;
   std::vector<float>   *ph_pt1conv;
   std::vector<int>     *ph_convtrk1nBLHits;
   std::vector<int>     *ph_convtrk1nPixHits;
   std::vector<int>     *ph_convtrk1nSCTHits;
   std::vector<int>     *ph_convtrk1nTRTHits;
   std::vector<float>   *ph_pt2conv;
   std::vector<int>     *ph_convtrk2nBLHits;
   std::vector<int>     *ph_convtrk2nPixHits;
   std::vector<int>     *ph_convtrk2nSCTHits;
   std::vector<int>     *ph_convtrk2nTRTHits;
   std::vector<float>   *ph_ptconv;
   std::vector<float>   *ph_pzconv;
   std::vector<float>   *ph_reta;
   std::vector<float>   *ph_rphi;
   std::vector<float>   *ph_EtringnoisedR03sig2;
   std::vector<float>   *ph_EtringnoisedR03sig3;
   std::vector<float>   *ph_EtringnoisedR03sig4;
   std::vector<double>  *ph_isolationlikelihoodjets;
   std::vector<double>  *ph_isolationlikelihoodhqelectrons;
   std::vector<double>  *ph_loglikelihood;
   std::vector<double>  *ph_photonweight;
   std::vector<double>  *ph_photonbgweight;
   std::vector<double>  *ph_neuralnet;
   std::vector<double>  *ph_Hmatrix;
   std::vector<double>  *ph_Hmatrix5;
   std::vector<double>  *ph_adaboost;
   std::vector<float>   *ph_zvertex;
   std::vector<float>   *ph_errz;
   std::vector<float>   *ph_etap;
   std::vector<float>   *ph_depth;
   std::vector<float>   *ph_cl_E;
   std::vector<float>   *ph_cl_pt;
   std::vector<float>   *ph_cl_eta;
   std::vector<float>   *ph_cl_phi;
   std::vector<float>   *ph_Es0;
   std::vector<float>   *ph_etas0;
   std::vector<float>   *ph_phis0;
   std::vector<float>   *ph_Es1;
   std::vector<float>   *ph_etas1;
   std::vector<float>   *ph_phis1;
   std::vector<float>   *ph_Es2;
   std::vector<float>   *ph_etas2;
   std::vector<float>   *ph_phis2;
   std::vector<float>   *ph_Es3;
   std::vector<float>   *ph_etas3;
   std::vector<float>   *ph_phis3;
   std::vector<float>   *ph_rawcl_Es0;
   std::vector<float>   *ph_rawcl_etas0;
   std::vector<float>   *ph_rawcl_phis0;
   std::vector<float>   *ph_rawcl_Es1;
   std::vector<float>   *ph_rawcl_etas1;
   std::vector<float>   *ph_rawcl_phis1;
   std::vector<float>   *ph_rawcl_Es2;
   std::vector<float>   *ph_rawcl_etas2;
   std::vector<float>   *ph_rawcl_phis2;
   std::vector<float>   *ph_rawcl_Es3;
   std::vector<float>   *ph_rawcl_etas3;
   std::vector<float>   *ph_rawcl_phis3;
   std::vector<float>   *ph_rawcl_E;
   std::vector<float>   *ph_rawcl_pt;
   std::vector<float>   *ph_rawcl_eta;
   std::vector<float>   *ph_rawcl_phi;
   std::vector<int>     *ph_truth_isConv;
   std::vector<int>     *ph_truth_isBrem;
   std::vector<int>     *ph_truth_isFromHardProc;
   std::vector<int>     *ph_truth_isPhotonFromHardProc;
   std::vector<float>   *ph_truth_Rconv;
   std::vector<float>   *ph_truth_zconv;
   std::vector<float>   *ph_deltaEmax2;
   std::vector<unsigned int> *ph_isIso;
   std::vector<float>   *ph_topoEtcone20;
   std::vector<float>   *ph_topoEtcone40;
   std::vector<float>   *ph_topoEtcone60;
   std::vector<float>   *ph_jet_dr;
   std::vector<float>   *ph_jet_E;
   std::vector<float>   *ph_jet_pt;
   std::vector<float>   *ph_jet_m;
   std::vector<float>   *ph_jet_eta;
   std::vector<float>   *ph_jet_phi;
   std::vector<float>   *ph_jet_truth_dr;
   std::vector<float>   *ph_jet_truth_E;
   std::vector<float>   *ph_jet_truth_pt;
   std::vector<float>   *ph_jet_truth_m;
   std::vector<float>   *ph_jet_truth_eta;
   std::vector<float>   *ph_jet_truth_phi;
   std::vector<int>     *ph_jet_truth_matched;
   std::vector<int>     *ph_jet_matched;
   std::vector<float>   *ph_trackIsol;
   std::vector<float>   *ph_convIP;
   std::vector<float>   *ph_convIPRev;
   std::vector<float>   *ph_HPV_zvertex;
   std::vector<float>   *ph_HPV_sigma_zvertex;
   std::vector<float>   *ph_HPV_zvertex_charged;
   std::vector<float>   *ph_HPV_sigma_zvertex_charged;
   std::vector<float>   *ph_HPV_zvertex_neutral;
   std::vector<float>   *ph_HPV_sigma_zvertex_neutral;
   std::vector<float>   *ph_HPV_eta;
   std::vector<float>   *ph_HPV_sigma_eta;
   std::vector<float>   *ph_HPV_convTrk1_zvertex;
   std::vector<float>   *ph_HPV_convTrk1_sigma_zvertex;
   std::vector<float>   *ph_HPV_convTrk2_zvertex;
   std::vector<float>   *ph_HPV_convTrk2_sigma_zvertex;
   std::vector<float>   *ph_ptIsolationCone;
   std::vector<float>   *ph_ptIsolationConePhAngle;
   std::vector<float>   *ph_topodr;
   std::vector<float>   *ph_topopt;
   std::vector<float>   *ph_topoeta;
   std::vector<float>   *ph_topophi;
   std::vector<int>     *ph_topomatched;
   std::vector<float>   *ph_topoEMdr;
   std::vector<float>   *ph_topoEMpt;
   std::vector<float>   *ph_topoEMeta;
   std::vector<float>   *ph_topoEMphi;
   std::vector<int>     *ph_topoEMmatched;
   std::vector<float>   *ph_EF_dr;
   std::vector<float>   *ph_EF_E;
   std::vector<float>   *ph_EF_Et;
   std::vector<float>   *ph_EF_pt;
   std::vector<float>   *ph_EF_eta;
   std::vector<float>   *ph_EF_phi;
   std::vector<float>   *ph_EF_Ethad;
   std::vector<float>   *ph_EF_Ethad1;
   std::vector<float>   *ph_EF_f1;
   std::vector<float>   *ph_EF_Emins1;
   std::vector<float>   *ph_EF_fside;
   std::vector<float>   *ph_EF_Emax2;
   std::vector<float>   *ph_EF_ws3;
   std::vector<float>   *ph_EF_wstot;
   std::vector<float>   *ph_EF_E233;
   std::vector<float>   *ph_EF_E237;
   std::vector<float>   *ph_EF_E277;
   std::vector<float>   *ph_EF_weta2;
   std::vector<float>   *ph_EF_rphiallcalo;
   std::vector<float>   *ph_EF_Etcone45;
   std::vector<float>   *ph_EF_Etcone20;
   std::vector<float>   *ph_EF_Etcone30;
   std::vector<float>   *ph_EF_Etcone40;
   std::vector<float>   *ph_EF_zvertex;
   std::vector<float>   *ph_EF_errz;
   std::vector<int>     *ph_EF_matched;
   std::vector<float>   *ph_L2_dr;
   std::vector<float>   *ph_L2_E;
   std::vector<float>   *ph_L2_Et;
   std::vector<float>   *ph_L2_pt;
   std::vector<float>   *ph_L2_eta;
   std::vector<float>   *ph_L2_phi;
   std::vector<float>   *ph_L2_Ethad1;
   std::vector<float>   *ph_L2_reta;
   std::vector<float>   *ph_L2_Eratio;
   std::vector<float>   *ph_L2_E237;
   std::vector<float>   *ph_L2_E277;
   std::vector<float>   *ph_L2_fside;
   std::vector<float>   *ph_L2_weta2;
   std::vector<float>   *ph_L2_Emaxs1;
   std::vector<float>   *ph_L2_Emax2;
   std::vector<int>     *ph_L2_matched;
   std::vector<float>   *ph_L1_dr;
   std::vector<float>   *ph_L1_eta;
   std::vector<float>   *ph_L1_phi;
   std::vector<float>   *ph_L1_EMisol;
   std::vector<float>   *ph_L1_hadisol;
   std::vector<float>   *ph_L1_hadcore;
   std::vector<float>   *ph_L1_pt;
   std::vector<float>   *ph_L1_Et;
   std::vector<int>     *ph_L1_matched;
   Int_t           mu_n;
   std::vector<float>   *mu_E;
   std::vector<float>   *mu_pt;
   std::vector<float>   *mu_m;
   std::vector<float>   *mu_eta;
   std::vector<float>   *mu_phi;
   std::vector<float>   *mu_px;
   std::vector<float>   *mu_py;
   std::vector<float>   *mu_pz;
   std::vector<float>   *mu_charge;
   std::vector<unsigned short> *mu_allauthor;
   std::vector<int>     *mu_author;
   std::vector<float>   *mu_matchchi2;
   std::vector<int>     *mu_matchndof;
   std::vector<float>   *mu_etcone20;
   std::vector<float>   *mu_etcone30;
   std::vector<float>   *mu_etcone40;
   std::vector<float>   *mu_nucone20;
   std::vector<float>   *mu_nucone30;
   std::vector<float>   *mu_nucone40;
   std::vector<float>   *mu_ptcone20;
   std::vector<float>   *mu_ptcone30;
   std::vector<float>   *mu_ptcone40;
   std::vector<float>   *mu_energyLossPar;
   std::vector<float>   *mu_energyLossErr;
   std::vector<int>     *mu_bestMatch;
   std::vector<int>     *mu_isStandAloneMuon;
   std::vector<int>     *mu_isCombinedMuon;
   std::vector<int>     *mu_isLowPtReconstructedMuon;
   std::vector<float>   *mu_d0_exPV;
   std::vector<float>   *mu_z0_exPV;
   std::vector<float>   *mu_phi_exPV;
   std::vector<float>   *mu_theta_exPV;
   std::vector<float>   *mu_qoverp_exPV;
   std::vector<float>   *mu_ms_d0;
   std::vector<float>   *mu_ms_z0;
   std::vector<float>   *mu_ms_phi;
   std::vector<float>   *mu_ms_theta;
   std::vector<float>   *mu_ms_qoverp;
   std::vector<float>   *mu_id_d0;
   std::vector<float>   *mu_id_z0;
   std::vector<float>   *mu_id_phi;
   std::vector<float>   *mu_id_theta;
   std::vector<float>   *mu_id_qoverp;
   std::vector<float>   *mu_msextrap_d0;
   std::vector<float>   *mu_msextrap_z0;
   std::vector<float>   *mu_msextrap_phi;
   std::vector<float>   *mu_msextrap_theta;
   std::vector<float>   *mu_msextrap_qoverp;
   std::vector<float>   *mu_idextrap_d0;
   std::vector<float>   *mu_idextrap_z0;
   std::vector<float>   *mu_idextrap_phi;
   std::vector<float>   *mu_idextrap_theta;
   std::vector<float>   *mu_idextrap_qoverp;
   std::vector<int>     *mu_nBLHits;
   std::vector<int>     *mu_nPixHits;
   std::vector<int>     *mu_nSCTHits;
   std::vector<int>     *mu_nTRTHits;
   std::vector<int>     *mu_nTRTHighTHits;
   std::vector<int>     *mu_nBLSharedHits;
   std::vector<int>     *mu_nPixSharedHits;
   std::vector<int>     *mu_nPixHoles;
   std::vector<int>     *mu_nSCTSharedHits;
   std::vector<int>     *mu_nSCTHoles;
   std::vector<int>     *mu_nTRTOutliers;
   std::vector<int>     *mu_nTRTHighTOutliers;
   std::vector<int>     *mu_nMDTHits;
   std::vector<int>     *mu_nMDTHoles;
   std::vector<int>     *mu_nCSCEtaHits;
   std::vector<int>     *mu_nCSCEtaHoles;
   std::vector<int>     *mu_nCSCPhiHits;
   std::vector<int>     *mu_nCSCPhiHoles;
   std::vector<int>     *mu_nRPCEtaHits;
   std::vector<int>     *mu_nRPCEtaHoles;
   std::vector<int>     *mu_nRPCPhiHits;
   std::vector<int>     *mu_nRPCPhiHoles;
   std::vector<int>     *mu_nTGCEtaHits;
   std::vector<int>     *mu_nTGCEtaHoles;
   std::vector<int>     *mu_nTGCPhiHits;
   std::vector<int>     *mu_nTGCPhiHoles;
   std::vector<int>     *mu_nGangedPixels;
   std::vector<int>     *mu_nOutliersOnTrack;
   std::vector<int>     *mu_nMDTBIHits;
   std::vector<int>     *mu_nMDTBMHits;
   std::vector<int>     *mu_nMDTBOHits;
   std::vector<int>     *mu_nMDTBEEHits;
   std::vector<int>     *mu_nMDTBIS78Hits;
   std::vector<int>     *mu_nMDTEIHits;
   std::vector<int>     *mu_nMDTEMHits;
   std::vector<int>     *mu_nMDTEOHits;
   std::vector<int>     *mu_nMDTEEHits;
   std::vector<int>     *mu_nRPCLayer1EtaHits;
   std::vector<int>     *mu_nRPCLayer2EtaHits;
   std::vector<int>     *mu_nRPCLayer3EtaHits;
   std::vector<int>     *mu_nRPCLayer1PhiHits;
   std::vector<int>     *mu_nRPCLayer2PhiHits;
   std::vector<int>     *mu_nRPCLayer3PhiHits;
   std::vector<int>     *mu_nTGCLayer1EtaHits;
   std::vector<int>     *mu_nTGCLayer2EtaHits;
   std::vector<int>     *mu_nTGCLayer3EtaHits;
   std::vector<int>     *mu_nTGCLayer4EtaHits;
   std::vector<int>     *mu_nTGCLayer1PhiHits;
   std::vector<int>     *mu_nTGCLayer2PhiHits;
   std::vector<int>     *mu_nTGCLayer3PhiHits;
   std::vector<int>     *mu_nTGCLayer4PhiHits;
   std::vector<int>     *mu_barrelSectors;
   std::vector<int>     *mu_endcapSectors;
   std::vector<float>   *mu_trackd0;
   std::vector<float>   *mu_trackz0;
   std::vector<float>   *mu_trackphi;
   std::vector<float>   *mu_tracktheta;
   std::vector<float>   *mu_trackqoverp;
   std::vector<float>   *mu_trackcov_d0;
   std::vector<float>   *mu_trackcov_z0;
   std::vector<float>   *mu_trackcov_phi;
   std::vector<float>   *mu_trackcov_theta;
   std::vector<float>   *mu_trackcov_qoverp;
   std::vector<float>   *mu_trackcov_d0_z0;
   std::vector<float>   *mu_trackcov_d0_phi;
   std::vector<float>   *mu_trackcov_d0_theta;
   std::vector<float>   *mu_trackcov_d0_qoverp;
   std::vector<float>   *mu_trackcov_z0_phi;
   std::vector<float>   *mu_trackcov_z0_theta;
   std::vector<float>   *mu_trackcov_z0_qoverp;
   std::vector<float>   *mu_trackcov_phi_theta;
   std::vector<float>   *mu_trackcov_phi_qoverp;
   std::vector<float>   *mu_trackcov_theta_qoverp;
   std::vector<float>   *mu_trackfitchi2;
   std::vector<int>     *mu_trackfitndof;
   std::vector<int>     *mu_hastrack;
   std::vector<float>   *mu_truth_dr;
   std::vector<float>   *mu_truth_E;
   std::vector<float>   *mu_truth_pt;
   std::vector<float>   *mu_truth_eta;
   std::vector<float>   *mu_truth_phi;
   std::vector<int>     *mu_truth_type;
   std::vector<int>     *mu_truth_status;
   std::vector<int>     *mu_truth_barcode;
   std::vector<int>     *mu_truth_mothertype;
   std::vector<int>     *mu_truth_motherbarcode;
   std::vector<int>     *mu_truth_matched;
   std::vector<float>   *mu_EF_dr;
   std::vector<float>   *mu_EF_cb_eta;
   std::vector<float>   *mu_EF_cb_phi;
   std::vector<float>   *mu_EF_cb_pt;
   std::vector<float>   *mu_EF_ms_eta;
   std::vector<float>   *mu_EF_ms_phi;
   std::vector<float>   *mu_EF_ms_pt;
   std::vector<float>   *mu_EF_me_eta;
   std::vector<float>   *mu_EF_me_phi;
   std::vector<float>   *mu_EF_me_pt;
   std::vector<int>     *mu_EF_matched;
   std::vector<float>   *mu_L2CB_dr;
   std::vector<float>   *mu_L2CB_pt;
   std::vector<float>   *mu_L2CB_eta;
   std::vector<float>   *mu_L2CB_phi;
   std::vector<float>   *mu_L2CB_id_pt;
   std::vector<float>   *mu_L2CB_ms_pt;
   std::vector<int>     *mu_L2CB_nPixHits;
   std::vector<int>     *mu_L2CB_nSCTHits;
   std::vector<int>     *mu_L2CB_nTRTHits;
   std::vector<int>     *mu_L2CB_nTRTHighTHits;
   std::vector<int>     *mu_L2CB_matched;
   std::vector<float>   *mu_L1_dr;
   std::vector<float>   *mu_L1_pt;
   std::vector<float>   *mu_L1_eta;
   std::vector<float>   *mu_L1_phi;
   std::vector<short>   *mu_L1_thrNumber;
   std::vector<short>   *mu_L1_RoINumber;
   std::vector<short>   *mu_L1_sectorAddress;
   std::vector<int>     *mu_L1_firstCandidate;
   std::vector<int>     *mu_L1_moreCandInRoI;
   std::vector<int>     *mu_L1_moreCandInSector;
   std::vector<short>   *mu_L1_source;
   std::vector<short>   *mu_L1_hemisphere;
   std::vector<int>     *mu_L1_matched;
   Int_t           mu_seg_n;
   std::vector<float>   *mu_seg_x;
   std::vector<float>   *mu_seg_y;
   std::vector<float>   *mu_seg_z;
   std::vector<float>   *mu_seg_phi;
   std::vector<float>   *mu_seg_theta;
   std::vector<float>   *mu_seg_locX;
   std::vector<float>   *mu_seg_locY;
   std::vector<float>   *mu_seg_locAngleXZ;
   std::vector<float>   *mu_seg_locAngleYZ;
   std::vector<int>     *mu_seg_sector;
   std::vector<int>     *mu_seg_stationEta;
   std::vector<int>     *mu_seg_isEndcap;
   std::vector<int>     *mu_seg_stationName;
   std::vector<int>     *mu_seg_author;
   std::vector<float>   *mu_seg_chi2;
   std::vector<int>     *mu_seg_ndof;
   Int_t           jet_n;
   std::vector<float>   *jet_E;
   std::vector<float>   *jet_pt;
   std::vector<float>   *jet_m;
   std::vector<float>   *jet_eta;
   std::vector<float>   *jet_phi;
   std::vector<float>   *jet_WIDTH;
   std::vector<float>   *jet_n90;
   std::vector<float>   *jet_n90constituents;
   std::vector<float>   *jet_fracSamplingMax;
   std::vector<float>   *jet_SamplingMax;
   std::vector<float>   *jet_BCH_CORR_CELL;
   std::vector<float>   *jet_BCH_CORR_JET;
   std::vector<float>   *jet_BCH_CORR_JET_FORCELL;
   std::vector<float>   *jet_ENG_BAD_CELLS;
   std::vector<float>   *jet_N_BAD_CELLS;
   std::vector<float>   *jet_N_BAD_CELLS_CORR;
   std::vector<float>   *jet_BAD_CELLS_CORR_E;
   std::vector<double>  *jet_timing;
   std::vector<double>  *jet_quality;
   std::vector<float>   *jet_emfrac;
   std::vector<float>   *jet_GCWJES;
   std::vector<float>   *jet_EMJES;
   std::vector<float>   *jet_emscale_E;
   std::vector<float>   *jet_emscale_pt;
   std::vector<float>   *jet_emscale_m;
   std::vector<float>   *jet_emscale_eta;
   std::vector<float>   *jet_emscale_phi;
   std::vector<float>   *jet_jvtxf;
   std::vector<float>   *jet_jvtx_x;
   std::vector<float>   *jet_jvtx_y;
   std::vector<float>   *jet_jvtx_z;
   std::vector<double>  *jet_flavor_weight;
   std::vector<float>   *jet_el_dr;
   std::vector<int>     *jet_el_matched;
   std::vector<float>   *jet_mu_dr;
   std::vector<int>     *jet_mu_matched;
   Float_t         MET_RefFinal_etx;
   Float_t         MET_RefFinal_ety;
   Float_t         MET_RefFinal_sumet;
   Float_t         MET_RefFinal_et;
   Float_t         MET_RefFinal_phi;
   Float_t         MET_MuonBoy_etx;
   Float_t         MET_MuonBoy_ety;
   Float_t         MET_MuonBoy_sumet;
   Float_t         MET_MuonBoy_et;
   Float_t         MET_MuonBoy_phi;
   Float_t         MET_RefEle_etx;
   Float_t         MET_RefEle_ety;
   Float_t         MET_RefEle_sumet;
   Float_t         MET_RefEle_et;
   Float_t         MET_RefEle_phi;
   Float_t         MET_RefJet_etx;
   Float_t         MET_RefJet_ety;
   Float_t         MET_RefJet_sumet;
   Float_t         MET_RefJet_et;
   Float_t         MET_RefJet_phi;
   Float_t         MET_RefGamma_etx;
   Float_t         MET_RefGamma_ety;
   Float_t         MET_RefGamma_sumet;
   Float_t         MET_RefGamma_et;
   Float_t         MET_RefGamma_phi;
   Float_t         MET_RefTau_etx;
   Float_t         MET_RefTau_ety;
   Float_t         MET_RefTau_sumet;
   Float_t         MET_RefTau_et;
   Float_t         MET_RefTau_phi;
   Float_t         MET_Cryo_etx;
   Float_t         MET_Cryo_ety;
   Float_t         MET_Cryo_sumet;
   Float_t         MET_Cryo_et;
   Float_t         MET_Cryo_phi;
   Float_t         MET_CellOut_etx;
   Float_t         MET_CellOut_ety;
   Float_t         MET_CellOut_sumet;
   Float_t         MET_CellOut_et;
   Float_t         MET_CellOut_phi;
   Float_t         MET_MuonBoy_Spectro_etx;
   Float_t         MET_MuonBoy_Spectro_ety;
   Float_t         MET_MuonBoy_Spectro_sumet;
   Float_t         MET_MuonBoy_Spectro_et;
   Float_t         MET_MuonBoy_Spectro_phi;
   Float_t         MET_MuonBoy_Track_etx;
   Float_t         MET_MuonBoy_Track_ety;
   Float_t         MET_MuonBoy_Track_sumet;
   Float_t         MET_MuonBoy_Track_et;
   Float_t         MET_MuonBoy_Track_phi;
   Float_t         MET_CryoCone_etx;
   Float_t         MET_CryoCone_ety;
   Float_t         MET_CryoCone_sumet;
   Float_t         MET_CryoCone_et;
   Float_t         MET_CryoCone_phi;
   Float_t         MET_Muon_etx;
   Float_t         MET_Muon_ety;
   Float_t         MET_Muon_sumet;
   Float_t         MET_Muon_et;
   Float_t         MET_Muon_phi;
   Float_t         MET_Final_etx;
   Float_t         MET_Final_ety;
   Float_t         MET_Final_sumet;
   Float_t         MET_Final_et;
   Float_t         MET_Final_phi;
   Float_t         MET_TopoObj_etx;
   Float_t         MET_TopoObj_ety;
   Float_t         MET_TopoObj_sumet;
   Float_t         MET_TopoObj_et;
   Float_t         MET_TopoObj_phi;
   Float_t         MET_LocHadTopoObj_etx;
   Float_t         MET_LocHadTopoObj_ety;
   Float_t         MET_LocHadTopoObj_sumet;
   Float_t         MET_LocHadTopoObj_et;
   Float_t         MET_LocHadTopoObj_phi;
   Float_t         MET_Base_etx;
   Float_t         MET_Base_ety;
   Float_t         MET_Base_sumet;
   Float_t         MET_Base_et;
   Float_t         MET_Base_phi;
   Float_t         MET_Base0_etx;
   Float_t         MET_Base0_ety;
   Float_t         MET_Base0_sumet;
   Float_t         MET_Base0_et;
   Float_t         MET_Base0_phi;
   Float_t         MET_Calib_etx;
   Float_t         MET_Calib_ety;
   Float_t         MET_Calib_sumet;
   Float_t         MET_Calib_et;
   Float_t         MET_Calib_phi;
   Float_t         MET_Topo_etx;
   Float_t         MET_Topo_ety;
   Float_t         MET_Topo_sumet;
   Float_t         MET_Topo_et;
   Float_t         MET_Topo_phi;
   Float_t         MET_CorrTopo_etx;
   Float_t         MET_CorrTopo_ety;
   Float_t         MET_CorrTopo_sumet;
   Float_t         MET_CorrTopo_et;
   Float_t         MET_CorrTopo_phi;
   Float_t         MET_LocHadTopo_etx;
   Float_t         MET_LocHadTopo_ety;
   Float_t         MET_LocHadTopo_sumet;
   Float_t         MET_LocHadTopo_et;
   Float_t         MET_LocHadTopo_phi;
   Int_t           cl_n;
   std::vector<float>   *cl_E;
   std::vector<float>   *cl_pt;
   std::vector<float>   *cl_m;
   std::vector<float>   *cl_eta;
   std::vector<float>   *cl_phi;
   std::vector<float>   *cl_E_em;
   std::vector<float>   *cl_E_had;
   std::vector<float>   *cl_firstEdens;
   std::vector<float>   *cl_cellmaxfrac;
   std::vector<float>   *cl_longitudinal;
   std::vector<float>   *cl_secondlambda;
   std::vector<float>   *cl_lateral;
   std::vector<float>   *cl_secondR;
   std::vector<float>   *cl_centerlambda;
   std::vector<float>   *cl_deltaTheta;
   std::vector<float>   *cl_deltaPhi;
   std::vector<float>   *cl_time;
   Int_t           emcl_n;
   std::vector<float>   *emcl_E;
   std::vector<float>   *emcl_pt;
   std::vector<float>   *emcl_m;
   std::vector<float>   *emcl_eta;
   std::vector<float>   *emcl_phi;
   std::vector<float>   *emcl_E_em;
   std::vector<float>   *emcl_E_had;
   std::vector<float>   *emcl_firstEdens;
   std::vector<float>   *emcl_cellmaxfrac;
   std::vector<float>   *emcl_longitudinal;
   std::vector<float>   *emcl_secondlambda;
   std::vector<float>   *emcl_lateral;
   std::vector<float>   *emcl_secondR;
   std::vector<float>   *emcl_centerlambda;
   std::vector<float>   *emcl_deltaTheta;
   std::vector<float>   *emcl_deltaPhi;
   std::vector<float>   *emcl_time;
   std::vector<float>   *emcl_E_PreSamplerB;
   std::vector<float>   *emcl_E_EMB1;
   std::vector<float>   *emcl_E_EMB2;
   std::vector<float>   *emcl_E_EMB3;
   std::vector<float>   *emcl_E_PreSamplerE;
   std::vector<float>   *emcl_E_EME1;
   std::vector<float>   *emcl_E_EME2;
   std::vector<float>   *emcl_E_EME3;
   std::vector<float>   *emcl_E_HEC0;
   std::vector<float>   *emcl_E_HEC1;
   std::vector<float>   *emcl_E_HEC2;
   std::vector<float>   *emcl_E_HEC3;
   std::vector<float>   *emcl_E_TileBar0;
   std::vector<float>   *emcl_E_TileBar1;
   std::vector<float>   *emcl_E_TileBar2;
   std::vector<float>   *emcl_E_TileGap1;
   std::vector<float>   *emcl_E_TileGap2;
   std::vector<float>   *emcl_E_TileGap3;
   std::vector<float>   *emcl_E_TileExt0;
   std::vector<float>   *emcl_E_TileExt1;
   std::vector<float>   *emcl_E_TileExt2;
   std::vector<float>   *emcl_E_FCAL0;
   std::vector<float>   *emcl_E_FCAL1;
   std::vector<float>   *emcl_E_FCAL2;
   Int_t           trk_n;
   std::vector<float>   *trk_d0;
   std::vector<float>   *trk_z0;
   std::vector<float>   *trk_phi;
   std::vector<float>   *trk_theta;
   std::vector<float>   *trk_qoverp;
   std::vector<float>   *trk_pt;
   std::vector<float>   *trk_eta;
   std::vector<float>   *trk_err_d0;
   std::vector<float>   *trk_err_z0;
   std::vector<float>   *trk_err_phi;
   std::vector<float>   *trk_err_theta;
   std::vector<float>   *trk_err_qoverp;
   std::vector<float>   *trk_cov_d0;
   std::vector<float>   *trk_cov_z0;
   std::vector<float>   *trk_cov_phi;
   std::vector<float>   *trk_cov_theta;
   std::vector<float>   *trk_cov_qoverp;
   std::vector<float>   *trk_cov_d0_z0;
   std::vector<float>   *trk_cov_d0_phi;
   std::vector<float>   *trk_cov_d0_theta;
   std::vector<float>   *trk_cov_d0_qoverp;
   std::vector<float>   *trk_cov_z0_phi;
   std::vector<float>   *trk_cov_z0_theta;
   std::vector<float>   *trk_cov_z0_qoverp;
   std::vector<float>   *trk_cov_phi_theta;
   std::vector<float>   *trk_cov_phi_qoverp;
   std::vector<float>   *trk_cov_theta_qoverp;
   std::vector<int>     *trk_blayerPrediction_expectHit;
   std::vector<std::vector<float> > *trk_blayerPrediction_x;
   std::vector<std::vector<float> > *trk_blayerPrediction_y;
   std::vector<std::vector<float> > *trk_blayerPrediction_z;
   std::vector<std::vector<float> > *trk_blayerPrediction_locX;
   std::vector<std::vector<float> > *trk_blayerPrediction_locY;
   std::vector<std::vector<float> > *trk_blayerPrediction_err_locX;
   std::vector<std::vector<float> > *trk_blayerPrediction_err_locY;
   std::vector<std::vector<float> > *trk_blayerPrediction_etaDistToEdge;
   std::vector<std::vector<float> > *trk_blayerPrediction_phiDistToEdge;
   std::vector<std::vector<unsigned int> > *trk_blayerPrediction_detElementId;
   std::vector<std::vector<int> > *trk_blayerPrediction_row;
   std::vector<std::vector<int> > *trk_blayerPrediction_col;
   std::vector<std::vector<int> > *trk_blayerPrediction_type;
   std::vector<float>   *trk_d0_wrtPV;
   std::vector<float>   *trk_z0_wrtPV;
   std::vector<float>   *trk_phi_wrtPV;
   std::vector<float>   *trk_theta_wrtPV;
   std::vector<float>   *trk_qoverp_wrtPV;
   std::vector<float>   *trk_chi2;
   std::vector<int>     *trk_ndof;
   std::vector<int>     *trk_nBLHits;
   std::vector<int>     *trk_nPixHits;
   std::vector<int>     *trk_nSCTHits;
   std::vector<int>     *trk_nTRTHits;
   std::vector<int>     *trk_nPixHoles;
   std::vector<int>     *trk_nSCTHoles;
   std::vector<int>     *trk_nMDTHits;
   std::vector<int>     *trk_nCSCEtaHits;
   std::vector<int>     *trk_nCSCPhiHits;
   std::vector<int>     *trk_nRPCEtaHits;
   std::vector<int>     *trk_nRPCPhiHits;
   std::vector<int>     *trk_nTGCEtaHits;
   std::vector<int>     *trk_nTGCPhiHits;
   std::vector<int>     *trk_nHits;
   std::vector<int>     *trk_nHoles;
   std::vector<int>     *trk_hitPattern;
   std::vector<float>   *trk_TRTHighTHitsRatio;
   std::vector<float>   *trk_pixeldEdx;
   std::vector<int>     *trk_fitter;
   std::vector<int>     *trk_patternReco1;
   std::vector<int>     *trk_patternReco2;
   std::vector<int>     *trk_seedFinder;
   Int_t           tau_n;
   std::vector<float>   *tau_Et;
   std::vector<float>   *tau_pt;
   std::vector<float>   *tau_m;
   std::vector<float>   *tau_eta;
   std::vector<float>   *tau_phi;
   std::vector<float>   *tau_charge;
   std::vector<float>   *tau_BDTEleScore;
   std::vector<float>   *tau_BDTJetScore;
   std::vector<float>   *tau_discCut;
   std::vector<float>   *tau_discCutTMVA;
   std::vector<float>   *tau_discLL;
   std::vector<float>   *tau_discNN;
   std::vector<float>   *tau_efficLL;
   std::vector<float>   *tau_efficNN;
   std::vector<float>   *tau_likelihood;
   std::vector<float>   *tau_tauJetNeuralNetwork;
   std::vector<float>   *tau_tauENeuralNetwork;
   std::vector<float>   *tau_tauElTauLikelihood;
   std::vector<int>     *tau_electronVetoLoose;
   std::vector<int>     *tau_electronVetoMedium;
   std::vector<int>     *tau_electronVetoTight;
   std::vector<int>     *tau_muonVeto;
   std::vector<int>     *tau_tauCutLoose;
   std::vector<int>     *tau_tauCutMedium;
   std::vector<int>     *tau_tauCutTight;
   std::vector<int>     *tau_tauCutSafeLoose;
   std::vector<int>     *tau_tauCutSafeMedium;
   std::vector<int>     *tau_tauCutSafeTight;
   std::vector<int>     *tau_tauCutSafeCaloLoose;
   std::vector<int>     *tau_tauCutSafeCaloMedium;
   std::vector<int>     *tau_tauCutSafeCaloTight;
   std::vector<int>     *tau_tauLlhLoose;
   std::vector<int>     *tau_tauLlhMedium;
   std::vector<int>     *tau_tauLlhTight;
   std::vector<int>     *tau_author;
   std::vector<int>     *tau_ROIword;
   std::vector<int>     *tau_nProng;
   std::vector<int>     *tau_numTrack;
   std::vector<float>   *tau_etOverPtLeadTrk;
   std::vector<float>   *tau_ipZ0SinThetaSigLeadTrk;
   std::vector<float>   *tau_leadTrkPt;
   std::vector<int>     *tau_nLooseTrk;
   std::vector<int>     *tau_nLooseConvTrk;
   std::vector<int>     *tau_nProngLoose;
   std::vector<float>   *tau_ipSigLeadTrk;
   std::vector<float>   *tau_ipSigLeadLooseTrk;
   std::vector<float>   *tau_etOverPtLeadLooseTrk;
   std::vector<float>   *tau_leadLooseTrkPt;
   std::vector<float>   *tau_chrgLooseTrk;
   std::vector<float>   *tau_massTrkSys;
   std::vector<float>   *tau_trkWidth2;
   std::vector<float>   *tau_trFlightPathSig;
   std::vector<float>   *tau_etEflow;
   std::vector<float>   *tau_mEflow;
   std::vector<int>     *tau_nPi0;
   std::vector<float>   *tau_ele_E237E277;
   std::vector<float>   *tau_ele_PresamplerFraction;
   std::vector<float>   *tau_ele_ECALFirstFraction;
   std::vector<float>   *tau_seedCalo_EMRadius;
   std::vector<float>   *tau_seedCalo_hadRadius;
   std::vector<float>   *tau_seedCalo_etEMAtEMScale;
   std::vector<float>   *tau_seedCalo_etHadAtEMScale;
   std::vector<float>   *tau_seedCalo_isolFrac;
   std::vector<float>   *tau_seedCalo_centFrac;
   std::vector<float>   *tau_seedCalo_stripWidth2;
   std::vector<int>     *tau_seedCalo_nStrip;
   std::vector<float>   *tau_seedCalo_etEMCalib;
   std::vector<float>   *tau_seedCalo_etHadCalib;
   std::vector<float>   *tau_seedCalo_eta;
   std::vector<float>   *tau_seedCalo_phi;
   std::vector<float>   *tau_seedCalo_nIsolLooseTrk;
   std::vector<float>   *tau_seedCalo_trkAvgDist;
   std::vector<float>   *tau_seedCalo_trkRmsDist;
   std::vector<float>   *tau_seedTrk_EMRadius;
   std::vector<float>   *tau_seedTrk_isolFrac;
   std::vector<float>   *tau_seedTrk_etChrgHadOverSumTrkPt;
   std::vector<float>   *tau_seedTrk_isolFracWide;
   std::vector<float>   *tau_seedTrk_etHadAtEMScale;
   std::vector<float>   *tau_seedTrk_etEMAtEMScale;
   std::vector<float>   *tau_seedTrk_etEMCL;
   std::vector<float>   *tau_seedTrk_etChrgEM;
   std::vector<float>   *tau_seedTrk_etNeuEM;
   std::vector<float>   *tau_seedTrk_etResNeuEM;
   std::vector<float>   *tau_seedTrk_hadLeakEt;
   std::vector<float>   *tau_seedTrk_sumEMCellEtOverLeadTrkPt;
   std::vector<float>   *tau_seedTrk_secMaxStripEt;
   std::vector<float>   *tau_seedTrk_stripWidth2;
   std::vector<int>     *tau_seedTrk_nStrip;
   std::vector<float>   *tau_seedTrk_etChrgHad;
   std::vector<int>     *tau_seedTrk_nOtherCoreTrk;
   std::vector<int>     *tau_seedTrk_nIsolTrk;
   std::vector<float>   *tau_seedTrk_etIsolEM;
   std::vector<float>   *tau_seedTrk_etIsolHad;
   std::vector<float>   *tau_calcVars_sumTrkPt;
   std::vector<float>   *tau_calcVars_numTopoClusters;
   std::vector<float>   *tau_calcVars_numEffTopoClusters;
   std::vector<float>   *tau_calcVars_topoInvMass;
   std::vector<float>   *tau_calcVars_effTopoInvMass;
   std::vector<float>   *tau_calcVars_topoMeanDeltaR;
   std::vector<float>   *tau_calcVars_effTopoMeanDeltaR;
   std::vector<float>   *tau_calcVars_etHadSumPtTracks;
   std::vector<float>   *tau_calcVars_etEMSumPtTracks;
   std::vector<int>     *tau_track_n;
   std::vector<float>   *tau_EF_dr;
   std::vector<int>     *tau_EF_matched;
   std::vector<float>   *tau_L2_dr;
   std::vector<float>   *tau_L2_E;
   std::vector<float>   *tau_L2_Et;
   std::vector<float>   *tau_L2_pt;
   std::vector<float>   *tau_L2_eta;
   std::vector<float>   *tau_L2_phi;
   std::vector<int>     *tau_L2_matched;
   std::vector<float>   *tau_L1_dr;
   std::vector<float>   *tau_L1_Et;
   std::vector<float>   *tau_L1_pt;
   std::vector<float>   *tau_L1_eta;
   std::vector<float>   *tau_L1_phi;
   std::vector<int>     *tau_L1_matched;
   Int_t           mb_n;
   std::vector<float>   *mb_E;
   std::vector<float>   *mb_eta;
   std::vector<float>   *mb_phi;
   std::vector<float>   *mb_time;
   std::vector<int>     *mb_quality;
   std::vector<int>     *mb_type;
   std::vector<int>     *mb_module;
   std::vector<int>     *mb_channel;
   Float_t         mbtime_timeDiff;
   Float_t         mbtime_timeA;
   Float_t         mbtime_timeC;
   Int_t           mbtime_countA;
   Int_t           mbtime_countC;
   Bool_t          L1_MBTS_1;
   Bool_t          L1_MBTS_1_1;
   Bool_t          L1_MBTS_1_1_Col;
   Bool_t          L1_MBTS_1_1_EMPTY;
   Bool_t          L1_MBTS_1_1_FIRSTEMPTY;
   Bool_t          L1_MBTS_1_1_UNPAIRED;
   Bool_t          L1_MBTS_1_1_UNPAIRED1;
   Bool_t          L1_MBTS_1_1_UNPAIRED2;
   Bool_t          L1_MBTS_1_Col;
   Bool_t          L1_MBTS_1_EMPTY;
   Bool_t          L1_MBTS_1_FIRSTEMPTY;
   Bool_t          L1_MBTS_1_UNPAIRED;
   Bool_t          L1_MBTS_1_UNPAIRED1;
   Bool_t          L1_MBTS_1_UNPAIRED2;
   Bool_t          L1_MBTS_2;
   Bool_t          L1_MBTS_2_Col;
   Bool_t          L1_MBTS_2_EMPTY;
   Bool_t          L1_MBTS_2_FIRSTEMPTY;
   Bool_t          L1_MBTS_2_UNPAIRED;
   Bool_t          L1_MBTS_2_UNPAIRED1;
   Bool_t          L1_MBTS_2_UNPAIRED2;
   Bool_t          L1_MBTS_4_4;
   Bool_t          L1_MBTS_4_4_FIRSTEMPTY;
   Bool_t          L1_MBTS_4_4_UNPAIRED;
   Bool_t          L1_MBTS_4_4_UNPAIRED1;
   Bool_t          L1_MBTS_4_4_UNPAIRED2;
   Bool_t          L1_MBTS_A;
   Bool_t          L1_MBTS_C;
   Bool_t          collcand_passMBTSTime;
   Bool_t          collcand_passCaloTime;
   Bool_t          collcand_passTrigger;
   Bool_t          collcand_pass;
   Int_t           vxp_n;
   std::vector<float>   *vxp_x;
   std::vector<float>   *vxp_y;
   std::vector<float>   *vxp_z;
   std::vector<float>   *vxp_chi2;
   std::vector<int>     *vxp_ndof;
   std::vector<float>   *vxp_err_x;
   std::vector<float>   *vxp_err_y;
   std::vector<float>   *vxp_err_z;
   std::vector<float>   *vxp_px;
   std::vector<float>   *vxp_py;
   std::vector<float>   *vxp_pz;
   std::vector<float>   *vxp_E;
   std::vector<float>   *vxp_m;
   std::vector<int>     *vxp_nTracks;
   std::vector<float>   *vxp_sumPt;
   std::vector<int>     *vxp_type;
   std::vector<int>     *vxp_trk_n;
   std::vector<std::vector<float> > *vxp_trk_chi2;
   std::vector<std::vector<float> > *vxp_trk_d0;
   std::vector<std::vector<float> > *vxp_trk_z0;
   std::vector<std::vector<float> > *vxp_trk_unbiased_d0;
   std::vector<std::vector<float> > *vxp_trk_unbiased_z0;
   std::vector<std::vector<float> > *vxp_trk_err_unbiased_d0;
   std::vector<std::vector<float> > *vxp_trk_err_unbiased_z0;
   std::vector<std::vector<float> > *vxp_trk_phi;
   std::vector<std::vector<float> > *vxp_trk_theta;
   std::vector<std::vector<float> > *vxp_trk_weight;
   std::vector<std::vector<int> > *vxp_trk_index;
   Int_t           muonTruth_n;
   std::vector<float>   *muonTruth_pt;
   std::vector<float>   *muonTruth_m;
   std::vector<float>   *muonTruth_eta;
   std::vector<float>   *muonTruth_phi;
   std::vector<float>   *muonTruth_charge;
   std::vector<int>     *muonTruth_PDGID;
   std::vector<int>     *muonTruth_barcode;
   std::vector<int>     *muonTruth_type;
   std::vector<int>     *muonTruth_origin;
   Int_t           mcevt_n;
   std::vector<int>     *mcevt_signal_process_id;
   std::vector<int>     *mcevt_event_number;
   std::vector<double>  *mcevt_event_scale;
   std::vector<double>  *mcevt_alphaQCD;
   std::vector<double>  *mcevt_alphaQED;
   std::vector<int>     *mcevt_pdf_id1;
   std::vector<int>     *mcevt_pdf_id2;
   std::vector<double>  *mcevt_pdf_x1;
   std::vector<double>  *mcevt_pdf_x2;
   std::vector<double>  *mcevt_pdf_scale;
   std::vector<double>  *mcevt_pdf1;
   std::vector<double>  *mcevt_pdf2;
   std::vector<double>  *mcevt_weight;
   Int_t           mc_n;
   std::vector<float>   *mc_pt;
   std::vector<float>   *mc_m;
   std::vector<float>   *mc_eta;
   std::vector<float>   *mc_phi;
   std::vector<int>     *mc_status;
   std::vector<int>     *mc_barcode;
   std::vector<std::vector<int> > *mc_parents;
   std::vector<std::vector<int> > *mc_children;
   std::vector<int>     *mc_pdgId;
   std::vector<float>   *mc_charge;
   std::vector<float>   *mc_vx_x;
   std::vector<float>   *mc_vx_y;
   std::vector<float>   *mc_vx_z;
   std::vector<std::vector<int> > *mc_child_index;
   std::vector<std::vector<int> > *mc_parent_index;
   Int_t           trig_L1_emtau_n;
   std::vector<float>   *trig_L1_emtau_eta;
   std::vector<float>   *trig_L1_emtau_phi;
   std::vector<std::vector<std::string> > *trig_L1_emtau_thrNames;
   std::vector<std::vector<float> > *trig_L1_emtau_thrValues;
   std::vector<float>   *trig_L1_emtau_core;
   std::vector<float>   *trig_L1_emtau_EMClus;
   std::vector<float>   *trig_L1_emtau_tauClus;
   std::vector<float>   *trig_L1_emtau_EMIsol;
   std::vector<float>   *trig_L1_emtau_hadIsol;
   std::vector<float>   *trig_L1_emtau_hadCore;
   std::vector<unsigned int> *trig_L1_emtau_thrPattern;
   std::vector<int>     *trig_L1_emtau_L1_EM10;
   std::vector<int>     *trig_L1_emtau_L1_EM10I;
   std::vector<int>     *trig_L1_emtau_L1_EM14;
   std::vector<int>     *trig_L1_emtau_L1_EM2;
   std::vector<int>     *trig_L1_emtau_L1_EM2_UNPAIRED;
   std::vector<int>     *trig_L1_emtau_L1_EM3;
   std::vector<int>     *trig_L1_emtau_L1_EM3_EMPTY;
   std::vector<int>     *trig_L1_emtau_L1_EM3_FIRSTEMPTY;
   std::vector<int>     *trig_L1_emtau_L1_EM3_MV;
   std::vector<int>     *trig_L1_emtau_L1_EM3_UNPAIRED;
   std::vector<int>     *trig_L1_emtau_L1_EM4;
   std::vector<int>     *trig_L1_emtau_L1_EM5;
   std::vector<int>     *trig_L1_emtau_L1_EM5I;
   Int_t           trig_L2_emcl_n;
   std::vector<unsigned int> *trig_L2_emcl_quality;
   std::vector<float>   *trig_L2_emcl_E;
   std::vector<float>   *trig_L2_emcl_Et;
   std::vector<float>   *trig_L2_emcl_eta;
   std::vector<float>   *trig_L2_emcl_phi;
   std::vector<float>   *trig_L2_emcl_E237;
   std::vector<float>   *trig_L2_emcl_E277;
   std::vector<float>   *trig_L2_emcl_fracs1;
   std::vector<float>   *trig_L2_emcl_weta2;
   std::vector<float>   *trig_L2_emcl_Ehad1;
   std::vector<float>   *trig_L2_emcl_eta1;
   std::vector<float>   *trig_L2_emcl_emaxs1;
   std::vector<float>   *trig_L2_emcl_e2tsts1;
   Int_t           trig_L2_trk_idscan_eGamma_n;
   std::vector<int>     *trig_L2_trk_idscan_eGamma_algorithmId;
   std::vector<short>   *trig_L2_trk_idscan_eGamma_trackStatus;
   std::vector<float>   *trig_L2_trk_idscan_eGamma_chi2Ndof;
   std::vector<int>     *trig_L2_trk_idscan_eGamma_nStrawHits;
   std::vector<int>     *trig_L2_trk_idscan_eGamma_nHighThrHits;
   std::vector<int>     *trig_L2_trk_idscan_eGamma_nPixelSpacePoints;
   std::vector<int>     *trig_L2_trk_idscan_eGamma_nSCT_SpacePoints;
   std::vector<float>   *trig_L2_trk_idscan_eGamma_a0;
   std::vector<float>   *trig_L2_trk_idscan_eGamma_z0;
   std::vector<float>   *trig_L2_trk_idscan_eGamma_phi0;
   std::vector<float>   *trig_L2_trk_idscan_eGamma_eta;
   std::vector<float>   *trig_L2_trk_idscan_eGamma_pt;
   Int_t           trig_L2_trk_sitrack_eGamma_n;
   std::vector<int>     *trig_L2_trk_sitrack_eGamma_algorithmId;
   std::vector<short>   *trig_L2_trk_sitrack_eGamma_trackStatus;
   std::vector<float>   *trig_L2_trk_sitrack_eGamma_chi2Ndof;
   std::vector<int>     *trig_L2_trk_sitrack_eGamma_nStrawHits;
   std::vector<int>     *trig_L2_trk_sitrack_eGamma_nHighThrHits;
   std::vector<int>     *trig_L2_trk_sitrack_eGamma_nPixelSpacePoints;
   std::vector<int>     *trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints;
   std::vector<float>   *trig_L2_trk_sitrack_eGamma_a0;
   std::vector<float>   *trig_L2_trk_sitrack_eGamma_z0;
   std::vector<float>   *trig_L2_trk_sitrack_eGamma_phi0;
   std::vector<float>   *trig_L2_trk_sitrack_eGamma_eta;
   std::vector<float>   *trig_L2_trk_sitrack_eGamma_pt;
   Int_t           trig_L2_el_n;
   std::vector<float>   *trig_L2_el_E;
   std::vector<float>   *trig_L2_el_Et;
   std::vector<float>   *trig_L2_el_pt;
   std::vector<float>   *trig_L2_el_eta;
   std::vector<float>   *trig_L2_el_phi;
   std::vector<int>     *trig_L2_el_RoIWord;
   std::vector<float>   *trig_L2_el_zvertex;
   std::vector<int>     *trig_L2_el_charge;
   std::vector<int>     *trig_L2_el_trackAlgo;
   std::vector<float>   *trig_L2_el_TRTHighTHitsRatio;
   std::vector<float>   *trig_L2_el_deltaeta1;
   std::vector<float>   *trig_L2_el_deltaphi2;
   std::vector<float>   *trig_L2_el_EtOverPt;
   std::vector<float>   *trig_L2_el_reta;
   std::vector<float>   *trig_L2_el_Eratio;
   std::vector<float>   *trig_L2_el_Ethad1;
   std::vector<int>     *trig_L2_el_L2_2e3_NoCut;
   std::vector<int>     *trig_L2_el_L2_2e3_loose;
   std::vector<int>     *trig_L2_el_L2_2e3_medium;
   std::vector<int>     *trig_L2_el_L2_2e5_loose;
   std::vector<int>     *trig_L2_el_L2_2e5_medium;
   std::vector<int>     *trig_L2_el_L2_2e6_nocut;
   std::vector<int>     *trig_L2_el_L2_e10_loose;
   std::vector<int>     *trig_L2_el_L2_e10_loose_IdScan;
   std::vector<int>     *trig_L2_el_L2_e10_loose_TRT;
   std::vector<int>     *trig_L2_el_L2_e10_loose_passEF;
   std::vector<int>     *trig_L2_el_L2_e10_loose_passL2;
   std::vector<int>     *trig_L2_el_L2_e10_medium;
   std::vector<int>     *trig_L2_el_L2_e10_medium_IdScan;
   std::vector<int>     *trig_L2_el_L2_e10_medium_TRT;
   std::vector<int>     *trig_L2_el_L2_e10i_loose;
   std::vector<int>     *trig_L2_el_L2_e15_loose;
   std::vector<int>     *trig_L2_el_L2_e15i_loose;
   std::vector<int>     *trig_L2_el_L2_e20_loose_NoIDTrkCut;
   std::vector<int>     *trig_L2_el_L2_e3_NoCut;
   std::vector<int>     *trig_L2_el_L2_e3_NoCut_IdScan;
   std::vector<int>     *trig_L2_el_L2_e3_NoCut_TRT;
   std::vector<int>     *trig_L2_el_L2_e3_loose;
   std::vector<int>     *trig_L2_el_L2_e3_loose_IdScan;
   std::vector<int>     *trig_L2_el_L2_e3_loose_TRT;
   std::vector<int>     *trig_L2_el_L2_e3_medium;
   std::vector<int>     *trig_L2_el_L2_e3_medium_IdScan;
   std::vector<int>     *trig_L2_el_L2_e3_medium_TRT;
   std::vector<int>     *trig_L2_el_L2_e5_EFfullcalo;
   std::vector<int>     *trig_L2_el_L2_e5_NoCut;
   std::vector<int>     *trig_L2_el_L2_e5_NoCut_FwdBackTrk;
   std::vector<int>     *trig_L2_el_L2_e5_NoCut_IdScan;
   std::vector<int>     *trig_L2_el_L2_e5_NoCut_L2SW;
   std::vector<int>     *trig_L2_el_L2_e5_NoCut_Ringer;
   std::vector<int>     *trig_L2_el_L2_e5_NoCut_SiTrk;
   std::vector<int>     *trig_L2_el_L2_e5_NoCut_TRT;
   std::vector<int>     *trig_L2_el_L2_e5_NoCut_cells;
   std::vector<int>     *trig_L2_el_L2_e5_NoCut_cosmic;
   std::vector<int>     *trig_L2_el_L2_e5_NoCut_firstempty;
   std::vector<int>     *trig_L2_el_L2_e5_NoCut_unpaired;
   std::vector<int>     *trig_L2_el_L2_e5_loose;
   std::vector<int>     *trig_L2_el_L2_e5_loose_IdScan;
   std::vector<int>     *trig_L2_el_L2_e5_loose_NoIDTrkCut;
   std::vector<int>     *trig_L2_el_L2_e5_loose_TRT;
   std::vector<int>     *trig_L2_el_L2_e5_medium;
   std::vector<int>     *trig_L2_el_L2_e5_medium_IdScan;
   std::vector<int>     *trig_L2_el_L2_e5_medium_MV;
   std::vector<int>     *trig_L2_el_L2_e5_medium_TRT;
   std::vector<int>     *trig_L2_el_L2_e6_loose;
   Int_t           trig_L2_ph_n;
   std::vector<float>   *trig_L2_ph_E;
   std::vector<float>   *trig_L2_ph_Et;
   std::vector<float>   *trig_L2_ph_pt;
   std::vector<float>   *trig_L2_ph_eta;
   std::vector<float>   *trig_L2_ph_phi;
   std::vector<int>     *trig_L2_ph_RoIWord;
   std::vector<float>   *trig_L2_ph_HadEt1;
   std::vector<float>   *trig_L2_ph_Eratio;
   std::vector<float>   *trig_L2_ph_Reta;
   std::vector<float>   *trig_L2_ph_dPhi;
   std::vector<float>   *trig_L2_ph_dEta;
   std::vector<float>   *trig_L2_ph_F1;
   std::vector<int>     *trig_L2_ph_L2_2g10_nocut;
   std::vector<int>     *trig_L2_ph_L2_2g3_loose;
   std::vector<int>     *trig_L2_ph_L2_2g3_nocut;
   std::vector<int>     *trig_L2_ph_L2_2g5_loose;
   std::vector<int>     *trig_L2_ph_L2_g10_loose;
   std::vector<int>     *trig_L2_ph_L2_g10i_loose;
   std::vector<int>     *trig_L2_ph_L2_g15_loose;
   std::vector<int>     *trig_L2_ph_L2_g15i_loose;
   std::vector<int>     *trig_L2_ph_L2_g3_loose;
   std::vector<int>     *trig_L2_ph_L2_g3_nocut;
   std::vector<int>     *trig_L2_ph_L2_g3_nocut_unpaired;
   std::vector<int>     *trig_L2_ph_L2_g5_EFfullcalo;
   std::vector<int>     *trig_L2_ph_L2_g5_loose;
   std::vector<int>     *trig_L2_ph_L2_g5_nocut;
   std::vector<int>     *trig_L2_ph_L2_g5_nocut_cosmic;
   std::vector<int>     *trig_L2_ph_L2_g5_nocut_firstempty;
   std::vector<int>     *trig_L2_ph_L2_g5_nocut_unpaired;
   std::vector<int>     *trig_L2_ph_L2_g6_loose;
   Int_t           trig_EF_emcl_n;
   std::vector<float>   *trig_EF_emcl_E;
   std::vector<float>   *trig_EF_emcl_pt;
   std::vector<float>   *trig_EF_emcl_m;
   std::vector<float>   *trig_EF_emcl_eta;
   std::vector<float>   *trig_EF_emcl_phi;
   std::vector<float>   *trig_EF_emcl_E_em;
   std::vector<float>   *trig_EF_emcl_E_had;
   std::vector<float>   *trig_EF_emcl_firstEdens;
   std::vector<float>   *trig_EF_emcl_cellmaxfrac;
   std::vector<float>   *trig_EF_emcl_longitudinal;
   std::vector<float>   *trig_EF_emcl_secondlambda;
   std::vector<float>   *trig_EF_emcl_lateral;
   std::vector<float>   *trig_EF_emcl_secondR;
   std::vector<float>   *trig_EF_emcl_centerlambda;
   std::vector<float>   *trig_EF_emcl_deltaTheta;
   std::vector<float>   *trig_EF_emcl_deltaPhi;
   std::vector<float>   *trig_EF_emcl_time;
   Int_t           trig_EF_emcl_slw_n;
   std::vector<float>   *trig_EF_emcl_slw_E;
   std::vector<float>   *trig_EF_emcl_slw_pt;
   std::vector<float>   *trig_EF_emcl_slw_m;
   std::vector<float>   *trig_EF_emcl_slw_eta;
   std::vector<float>   *trig_EF_emcl_slw_phi;
   std::vector<float>   *trig_EF_emcl_slw_E_em;
   std::vector<float>   *trig_EF_emcl_slw_E_had;
   std::vector<float>   *trig_EF_emcl_slw_firstEdens;
   std::vector<float>   *trig_EF_emcl_slw_cellmaxfrac;
   std::vector<float>   *trig_EF_emcl_slw_longitudinal;
   std::vector<float>   *trig_EF_emcl_slw_secondlambda;
   std::vector<float>   *trig_EF_emcl_slw_lateral;
   std::vector<float>   *trig_EF_emcl_slw_secondR;
   std::vector<float>   *trig_EF_emcl_slw_centerlambda;
   std::vector<float>   *trig_EF_emcl_slw_deltaTheta;
   std::vector<float>   *trig_EF_emcl_slw_deltaPhi;
   std::vector<float>   *trig_EF_emcl_slw_time;
   Int_t           trig_EF_el_n;
   std::vector<float>   *trig_EF_el_E;
   std::vector<float>   *trig_EF_el_Et;
   std::vector<float>   *trig_EF_el_pt;
   std::vector<float>   *trig_EF_el_m;
   std::vector<float>   *trig_EF_el_eta;
   std::vector<float>   *trig_EF_el_phi;
   std::vector<float>   *trig_EF_el_px;
   std::vector<float>   *trig_EF_el_py;
   std::vector<float>   *trig_EF_el_pz;
   std::vector<float>   *trig_EF_el_charge;
   std::vector<int>     *trig_EF_el_author;
   std::vector<unsigned int> *trig_EF_el_isEM;
   std::vector<int>     *trig_EF_el_loose;
   std::vector<int>     *trig_EF_el_medium;
   std::vector<int>     *trig_EF_el_mediumIso;
   std::vector<int>     *trig_EF_el_tight;
   std::vector<int>     *trig_EF_el_tightIso;
   std::vector<float>   *trig_EF_el_Ethad;
   std::vector<float>   *trig_EF_el_Ethad1;
   std::vector<float>   *trig_EF_el_f1;
   std::vector<float>   *trig_EF_el_f1core;
   std::vector<float>   *trig_EF_el_Emins1;
   std::vector<float>   *trig_EF_el_fside;
   std::vector<float>   *trig_EF_el_Emax2;
   std::vector<float>   *trig_EF_el_ws3;
   std::vector<float>   *trig_EF_el_wstot;
   std::vector<float>   *trig_EF_el_emaxs1;
   std::vector<float>   *trig_EF_el_deltaEs;
   std::vector<float>   *trig_EF_el_E233;
   std::vector<float>   *trig_EF_el_E237;
   std::vector<float>   *trig_EF_el_E277;
   std::vector<float>   *trig_EF_el_weta2;
   std::vector<float>   *trig_EF_el_f3;
   std::vector<float>   *trig_EF_el_f3core;
   std::vector<float>   *trig_EF_el_rphiallcalo;
   std::vector<float>   *trig_EF_el_Etcone45;
   std::vector<float>   *trig_EF_el_Etcone20;
   std::vector<float>   *trig_EF_el_Etcone30;
   std::vector<float>   *trig_EF_el_Etcone40;
   std::vector<float>   *trig_EF_el_pos7;
   std::vector<float>   *trig_EF_el_etacorrmag;
   std::vector<float>   *trig_EF_el_deltaeta1;
   std::vector<float>   *trig_EF_el_deltaeta2;
   std::vector<float>   *trig_EF_el_deltaphi2;
   std::vector<float>   *trig_EF_el_reta;
   std::vector<float>   *trig_EF_el_rphi;
   std::vector<int>     *trig_EF_el_EF_2e3_NoCut;
   std::vector<int>     *trig_EF_el_EF_2e3_loose;
   std::vector<int>     *trig_EF_el_EF_2e3_medium;
   std::vector<int>     *trig_EF_el_EF_2e5_loose;
   std::vector<int>     *trig_EF_el_EF_2e5_medium;
   std::vector<int>     *trig_EF_el_EF_2e6_nocut;
   std::vector<int>     *trig_EF_el_EF_e10_loose;
   std::vector<int>     *trig_EF_el_EF_e10_loose_IdScan;
   std::vector<int>     *trig_EF_el_EF_e10_loose_TRT;
   std::vector<int>     *trig_EF_el_EF_e10_loose_passEF;
   std::vector<int>     *trig_EF_el_EF_e10_loose_passL2;
   std::vector<int>     *trig_EF_el_EF_e10_medium;
   std::vector<int>     *trig_EF_el_EF_e10_medium_IdScan;
   std::vector<int>     *trig_EF_el_EF_e10_medium_TRT;
   std::vector<int>     *trig_EF_el_EF_e10i_loose;
   std::vector<int>     *trig_EF_el_EF_e15_loose;
   std::vector<int>     *trig_EF_el_EF_e15i_loose;
   std::vector<int>     *trig_EF_el_EF_e20_loose_NoIDTrkCut;
   std::vector<int>     *trig_EF_el_EF_e3_NoCut;
   std::vector<int>     *trig_EF_el_EF_e3_NoCut_IdScan;
   std::vector<int>     *trig_EF_el_EF_e3_NoCut_TRT;
   std::vector<int>     *trig_EF_el_EF_e3_loose;
   std::vector<int>     *trig_EF_el_EF_e3_loose_IdScan;
   std::vector<int>     *trig_EF_el_EF_e3_loose_TRT;
   std::vector<int>     *trig_EF_el_EF_e3_medium;
   std::vector<int>     *trig_EF_el_EF_e3_medium_IdScan;
   std::vector<int>     *trig_EF_el_EF_e3_medium_TRT;
   std::vector<int>     *trig_EF_el_EF_e5_EFfullcalo;
   std::vector<int>     *trig_EF_el_EF_e5_NoCut;
   std::vector<int>     *trig_EF_el_EF_e5_NoCut_FwdBackTrk;
   std::vector<int>     *trig_EF_el_EF_e5_NoCut_IdScan;
   std::vector<int>     *trig_EF_el_EF_e5_NoCut_L2SW;
   std::vector<int>     *trig_EF_el_EF_e5_NoCut_Ringer;
   std::vector<int>     *trig_EF_el_EF_e5_NoCut_SiTrk;
   std::vector<int>     *trig_EF_el_EF_e5_NoCut_TRT;
   std::vector<int>     *trig_EF_el_EF_e5_NoCut_cells;
   std::vector<int>     *trig_EF_el_EF_e5_NoCut_cosmic;
   std::vector<int>     *trig_EF_el_EF_e5_NoCut_firstempty;
   std::vector<int>     *trig_EF_el_EF_e5_NoCut_unpaired;
   std::vector<int>     *trig_EF_el_EF_e5_loose;
   std::vector<int>     *trig_EF_el_EF_e5_loose_IdScan;
   std::vector<int>     *trig_EF_el_EF_e5_loose_NoIDTrkCut;
   std::vector<int>     *trig_EF_el_EF_e5_loose_TRT;
   std::vector<int>     *trig_EF_el_EF_e5_medium;
   std::vector<int>     *trig_EF_el_EF_e5_medium_IdScan;
   std::vector<int>     *trig_EF_el_EF_e5_medium_MV;
   std::vector<int>     *trig_EF_el_EF_e5_medium_TRT;
   std::vector<int>     *trig_EF_el_EF_e6_loose;
   Int_t           trig_EF_ph_n;
   std::vector<float>   *trig_EF_ph_E;
   std::vector<float>   *trig_EF_ph_Et;
   std::vector<float>   *trig_EF_ph_pt;
   std::vector<float>   *trig_EF_ph_m;
   std::vector<float>   *trig_EF_ph_eta;
   std::vector<float>   *trig_EF_ph_phi;
   std::vector<float>   *trig_EF_ph_px;
   std::vector<float>   *trig_EF_ph_py;
   std::vector<float>   *trig_EF_ph_pz;
   std::vector<int>     *trig_EF_ph_EF_2g10_nocut;
   std::vector<int>     *trig_EF_ph_EF_2g3_loose;
   std::vector<int>     *trig_EF_ph_EF_2g3_nocut;
   std::vector<int>     *trig_EF_ph_EF_2g5_loose;
   std::vector<int>     *trig_EF_ph_EF_g10_loose;
   std::vector<int>     *trig_EF_ph_EF_g10_loose_larcalib;
   std::vector<int>     *trig_EF_ph_EF_g10i_loose;
   std::vector<int>     *trig_EF_ph_EF_g15_loose;
   std::vector<int>     *trig_EF_ph_EF_g15i_loose;
   std::vector<int>     *trig_EF_ph_EF_g3_loose;
   std::vector<int>     *trig_EF_ph_EF_g3_nocut;
   std::vector<int>     *trig_EF_ph_EF_g3_nocut_larcalib;
   std::vector<int>     *trig_EF_ph_EF_g3_nocut_unpaired;
   std::vector<int>     *trig_EF_ph_EF_g5_EFfullcalo;
   std::vector<int>     *trig_EF_ph_EF_g5_loose;
   std::vector<int>     *trig_EF_ph_EF_g5_loose_larcalib;
   std::vector<int>     *trig_EF_ph_EF_g5_nocut;
   std::vector<int>     *trig_EF_ph_EF_g5_nocut_cosmic;
   std::vector<int>     *trig_EF_ph_EF_g5_nocut_firstempty;
   std::vector<int>     *trig_EF_ph_EF_g5_nocut_unpaired;
   std::vector<int>     *trig_EF_ph_EF_g6_loose;
   Int_t           trig_Nav_n;
   std::vector<short>   *trig_Nav_chain_ChainId;
   std::vector<std::vector<int> > *trig_Nav_chain_RoIType;
   std::vector<std::vector<int> > *trig_Nav_chain_RoIIndex;
   Int_t           trig_RoI_L2_e_n;
   std::vector<short>   *trig_RoI_L2_e_type;
   std::vector<short>   *trig_RoI_L2_e_lastStep;
   std::vector<int>     *trig_RoI_L2_e_TrigEMCluster;
   std::vector<int>     *trig_RoI_L2_e_TrigEMClusterStatus;
   std::vector<int>     *trig_RoI_L2_e_EmTau_ROI;
   std::vector<int>     *trig_RoI_L2_e_EmTau_ROIStatus;
   std::vector<std::vector<int> > *trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGamma;
   std::vector<std::vector<int> > *trig_RoI_L2_e_TrigInDetTrackCollection_TrigSiTrack_eGammaStatus;
   std::vector<std::vector<int> > *trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGamma;
   std::vector<std::vector<int> > *trig_RoI_L2_e_TrigInDetTrackCollection_TrigIDSCAN_eGammaStatus;
   std::vector<std::vector<int> > *trig_RoI_L2_e_TrigElectronContainer;
   std::vector<std::vector<int> > *trig_RoI_L2_e_TrigElectronContainerStatus;
   Int_t           trig_RoI_EF_e_n;
   std::vector<short>   *trig_RoI_EF_e_type;
   std::vector<short>   *trig_RoI_EF_e_lastStep;
   std::vector<int>     *trig_RoI_EF_e_EmTau_ROI;
   std::vector<int>     *trig_RoI_EF_e_EmTau_ROIStatus;
   Int_t           trig_L1_mu_n;
   std::vector<float>   *trig_L1_mu_pt;
   std::vector<float>   *trig_L1_mu_eta;
   std::vector<float>   *trig_L1_mu_phi;
   std::vector<std::string>  *trig_L1_mu_thrName;
   std::vector<short>   *trig_L1_mu_thrNumber;
   std::vector<short>   *trig_L1_mu_RoINumber;
   std::vector<short>   *trig_L1_mu_sectorAddress;
   std::vector<int>     *trig_L1_mu_firstCandidate;
   std::vector<int>     *trig_L1_mu_moreCandInRoI;
   std::vector<int>     *trig_L1_mu_moreCandInSector;
   std::vector<short>   *trig_L1_mu_source;
   std::vector<short>   *trig_L1_mu_hemisphere;
   std::vector<unsigned int> *trig_L1_mu_RoIWord;
   Int_t           trig_l2muonfeature_n;
   std::vector<float>   *trig_l2muonfeature_pt;
   std::vector<float>   *trig_l2muonfeature_eta;
   std::vector<float>   *trig_l2muonfeature_phi;
   std::vector<float>   *trig_l2muonfeature_dir_phi;
   std::vector<float>   *trig_l2muonfeature_zeta;
   std::vector<float>   *trig_l2muonfeature_dir_zeta;
   std::vector<int>     *trig_l2muonfeature_roiId;
   std::vector<int>     *trig_l2muonfeature_saddress;
   std::vector<float>   *trig_l2muonfeature_radius;
   std::vector<float>   *trig_l2muonfeature_beta;
   std::vector<float>   *trig_l2muonfeature_sp1_r;
   std::vector<float>   *trig_l2muonfeature_sp1_z;
   std::vector<float>   *trig_l2muonfeature_sp1_slope;
   std::vector<float>   *trig_l2muonfeature_sp2_r;
   std::vector<float>   *trig_l2muonfeature_sp2_z;
   std::vector<float>   *trig_l2muonfeature_sp2_slope;
   std::vector<float>   *trig_l2muonfeature_sp3_r;
   std::vector<float>   *trig_l2muonfeature_sp3_z;
   std::vector<float>   *trig_l2muonfeature_sp3_slope;
   std::vector<float>   *trig_l2muonfeature_br_radius;
   std::vector<float>   *trig_l2muonfeature_br_sagitta;
   std::vector<float>   *trig_l2muonfeature_ec_alpha;
   std::vector<float>   *trig_l2muonfeature_ec_beta;
   std::vector<double>  *trig_l2muonfeature_dq_var1;
   std::vector<double>  *trig_l2muonfeature_dq_var2;
   Int_t           trig_l2combmuonfeature_n;
   std::vector<float>   *trig_l2combmuonfeature_sigma_pt;
   std::vector<float>   *trig_l2combmuonfeature_pt;
   std::vector<float>   *trig_l2combmuonfeature_eta;
   std::vector<float>   *trig_l2combmuonfeature_phi;
   std::vector<float>   *trig_l2combmuonfeature_mf_pt;
   std::vector<float>   *trig_l2combmuonfeature_mf_eta;
   std::vector<float>   *trig_l2combmuonfeature_mf_phi;
   std::vector<float>   *trig_l2combmuonfeature_mf_dir_phi;
   std::vector<float>   *trig_l2combmuonfeature_mf_zeta;
   std::vector<float>   *trig_l2combmuonfeature_mf_dir_zeta;
   std::vector<int>     *trig_l2combmuonfeature_mf_hasmf;
   Int_t           trig_l2isomuonfeature_n;
   std::vector<float>   *trig_l2isomuonfeature_EtInnerConeEC ;
   std::vector<float>   *trig_l2isomuonfeature_EtOuterConeEC;
   std::vector<float>   *trig_l2isomuonfeature_EtInnerConeHC;
   std::vector<float>   *trig_l2isomuonfeature_EtOuterConeHC;
   std::vector<int>     *trig_l2isomuonfeature_NTracksCone;
   std::vector<float>   *trig_l2isomuonfeature_SumPtTracksCone;
   std::vector<float>   *trig_l2isomuonfeature_PtMuTracksCone;
   std::vector<int>     *trig_l2isomuonfeature_iso_strategy;
   std::vector<int>     *trig_l2isomuonfeature_RoiIdMu ;
   std::vector<float>   *trig_l2isomuonfeature_PtMu;
   std::vector<float>   *trig_l2isomuonfeature_EtaMu;
   std::vector<float>   *trig_l2isomuonfeature_PhiMu;
   std::vector<float>   *trig_l2isomuonfeature_LArWeight;
   std::vector<float>   *trig_l2isomuonfeature_TileWeight;
   std::vector<float>   *trig_l2isomuonfeature_pt;
   std::vector<float>   *trig_l2isomuonfeature_eta;
   std::vector<float>   *trig_l2isomuonfeature_phi;
   Int_t           trig_l2tilemufeature_n;
   std::vector<float>   *trig_l2tilemufeature_eta;
   std::vector<float>   *trig_l2tilemufeature_phi;
   std::vector<std::vector<float> > *trig_l2tilemufeature_enedep;
   std::vector<float>   *trig_l2tilemufeature_quality;
   Int_t           trig_l2tiletrackmufeature_n;
   std::vector<float>   *trig_l2tiletrackmufeature_PtTR_Trk;
   std::vector<float>   *trig_l2tiletrackmufeature_EtaTR_Trk;
   std::vector<float>   *trig_l2tiletrackmufeature_PhiTR_Trk;
   std::vector<int>     *trig_l2tiletrackmufeature_Typ_IDTrk;
   std::vector<float>   *trig_l2tiletrackmufeature_pt;
   std::vector<float>   *trig_l2tiletrackmufeature_eta;
   std::vector<float>   *trig_l2tiletrackmufeature_phi;
   std::vector<float>   *trig_l2tiletrackmufeature_tilemu_eta;
   std::vector<float>   *trig_l2tiletrackmufeature_tilemu_phi;
   std::vector<std::vector<float> > *trig_l2tiletrackmufeature_tilemu_enedep;
   std::vector<float>   *trig_l2tiletrackmufeature_tilemu_quality;
   std::vector<int>     *trig_l2tiletrackmufeature_tilemu_hastilemu;
   Int_t           trig_EF_tau_n;
   std::vector<float>   *trig_EF_tau_Et;
   std::vector<float>   *trig_EF_tau_pt;
   std::vector<float>   *trig_EF_tau_m;
   std::vector<float>   *trig_EF_tau_eta;
   std::vector<float>   *trig_EF_tau_phi;
   std::vector<float>   *trig_EF_tau_px;
   std::vector<float>   *trig_EF_tau_py;
   std::vector<float>   *trig_EF_tau_pz;
   std::vector<unsigned int> *trig_EF_tau_RoIword;
   std::vector<unsigned int> *trig_EF_tau_nLooseTrk;
   std::vector<double>  *trig_EF_tau_leadLooseTrkPt;
   std::vector<float>   *trig_EF_tau_leadLooseTrkEta;
   std::vector<float>   *trig_EF_tau_leadLooseTrkPhi;
   std::vector<double>  *trig_EF_tau_seedCalo_EMRadius;
   std::vector<double>  *trig_EF_tau_seedCalo_hadRadius;
   std::vector<double>  *trig_EF_tau_seedCalo_etEMAtEMScale;
   std::vector<double>  *trig_EF_tau_seedCalo_etHadAtEMScale;
   std::vector<double>  *trig_EF_tau_seedCalo_isolFrac;
   std::vector<double>  *trig_EF_tau_seedCalo_centFrac;
   std::vector<double>  *trig_EF_tau_seedCalo_etEMCalib;
   std::vector<double>  *trig_EF_tau_seedCalo_etHadCalib;
   std::vector<double>  *trig_EF_tau_seedCalo_eta;
   std::vector<double>  *trig_EF_tau_seedCalo_phi;
   std::vector<float>   *trig_EF_tau_invMassOfTrks;
   std::vector<float>   *trig_EF_tau_looseTrkWidth;
   std::vector<float>   *trig_EF_tau_looseTrkRadius;
   std::vector<int>     *trig_EF_tau_EF_tau12_loose;
   std::vector<int>     *trig_EF_tau_EF_tau12_loose_PT;
   std::vector<int>     *trig_EF_tau_EF_tau12_loose_e10_loose;
   std::vector<int>     *trig_EF_tau_EF_tau12_loose_mu10;
   std::vector<int>     *trig_EF_tau_EF_tau12_loose_xe20;
   std::vector<int>     *trig_EF_tau_EF_tau16_loose;
   std::vector<int>     *trig_EF_tau_EF_tau16_loose_3j40;
   std::vector<int>     *trig_EF_tau_EF_tau16_loose_TAU8;
   std::vector<int>     *trig_EF_tau_EF_tau16_loose_xe20;
   std::vector<int>     *trig_EF_tau_EF_tau16i_NoIDTrkCut;
   std::vector<int>     *trig_EF_tau_EF_tau16i_loose;
   std::vector<int>     *trig_EF_tau_EF_tau20_loose;
   std::vector<int>     *trig_EF_tau_EF_tau20_medium;
   std::vector<int>     *trig_EF_tau_EF_tau29_loose;
   std::vector<int>     *trig_EF_tau_EF_tau29_medium;
   std::vector<int>     *trig_EF_tau_EF_tau29i_loose;
   std::vector<int>     *trig_EF_tau_EF_tau50_loose;
   std::vector<int>     *trig_EF_tau_EF_tau50_loose_TAU30;
   std::vector<int>     *trig_EF_tau_EF_tauNoCut;
   std::vector<int>     *trig_EF_tau_EF_tauNoCut_NoIDTrkCut;
   std::vector<int>     *trig_EF_tau_EF_tauNoCut_SiTrk;
   std::vector<int>     *trig_EF_tau_EF_tauNoCut_cells;
   std::vector<int>     *trig_EF_tau_EF_tauNoCut_cosmic;
   std::vector<int>     *trig_EF_tau_EF_tauNoCut_firstempty;
   std::vector<int>     *trig_EF_tau_EF_tauNoCut_hasTrk;
   std::vector<int>     *trig_EF_tau_EF_tauNoCut_hasTrk_MV;
   std::vector<int>     *trig_EF_tau_EF_tauNoCut_hasTrk_SiTrk;
   std::vector<int>     *trig_EF_tau_EF_tauNoCut_hasTrk_xe20;
   std::vector<int>     *trig_EF_tau_EF_tauNoCut_unpaired;
   Int_t           trig_L2_tau_n;
   std::vector<float>   *trig_L2_tau_pt;
   std::vector<float>   *trig_L2_tau_eta;
   std::vector<float>   *trig_L2_tau_phi;
   std::vector<float>   *trig_L2_tau_px;
   std::vector<float>   *trig_L2_tau_py;
   std::vector<float>   *trig_L2_tau_pz;
   std::vector<unsigned int> *trig_L2_tau_RoIword;
   std::vector<float>   *trig_L2_tau_simpleEtFlow;
   std::vector<int>     *trig_L2_tau_nMatchedTracks;
   std::vector<int>     *trig_L2_tau_L2_tau12_loose;
   std::vector<int>     *trig_L2_tau_L2_tau12_loose_PT;
   std::vector<int>     *trig_L2_tau_L2_tau12_loose_e10_loose;
   std::vector<int>     *trig_L2_tau_L2_tau12_loose_mu10;
   std::vector<int>     *trig_L2_tau_L2_tau12_loose_xe20;
   std::vector<int>     *trig_L2_tau_L2_tau16_loose;
   std::vector<int>     *trig_L2_tau_L2_tau16_loose_3j30;
   std::vector<int>     *trig_L2_tau_L2_tau16_loose_TAU8;
   std::vector<int>     *trig_L2_tau_L2_tau16_loose_xe20;
   std::vector<int>     *trig_L2_tau_L2_tau16i_NoIDTrkCut;
   std::vector<int>     *trig_L2_tau_L2_tau16i_loose;
   std::vector<int>     *trig_L2_tau_L2_tau20_loose;
   std::vector<int>     *trig_L2_tau_L2_tau20_medium;
   std::vector<int>     *trig_L2_tau_L2_tau29_loose;
   std::vector<int>     *trig_L2_tau_L2_tau29_medium;
   std::vector<int>     *trig_L2_tau_L2_tau29i_loose;
   std::vector<int>     *trig_L2_tau_L2_tau50_loose;
   std::vector<int>     *trig_L2_tau_L2_tau50_loose_TAU30;
   std::vector<int>     *trig_L2_tau_L2_tauNoCut;
   std::vector<int>     *trig_L2_tau_L2_tauNoCut_NoIDTrkCut;
   std::vector<int>     *trig_L2_tau_L2_tauNoCut_SiTrk;
   std::vector<int>     *trig_L2_tau_L2_tauNoCut_cells;
   std::vector<int>     *trig_L2_tau_L2_tauNoCut_cosmic;
   std::vector<int>     *trig_L2_tau_L2_tauNoCut_firstempty;
   std::vector<int>     *trig_L2_tau_L2_tauNoCut_hasTrk;
   std::vector<int>     *trig_L2_tau_L2_tauNoCut_hasTrk_MV;
   std::vector<int>     *trig_L2_tau_L2_tauNoCut_hasTrk_SiTrk;
   std::vector<int>     *trig_L2_tau_L2_tauNoCut_hasTrk_xe20;
   std::vector<int>     *trig_L2_tau_L2_tauNoCut_unpaired;
   std::vector<int>     *trig_L2_tau_cluster_quality;
   std::vector<float>   *trig_L2_tau_cluster_EMenergy;
   std::vector<float>   *trig_L2_tau_cluster_HADenergy;
   std::vector<float>   *trig_L2_tau_cluster_eta;
   std::vector<float>   *trig_L2_tau_cluster_phi;
   std::vector<float>   *trig_L2_tau_cluster_EMRadius2;
   std::vector<float>   *trig_L2_tau_cluster_CaloRadius;
   std::vector<float>   *trig_L2_tau_cluster_IsoFrac;
   std::vector<int>     *trig_L2_tau_cluster_numTotCells;
   std::vector<float>   *trig_L2_tau_cluster_stripWidth;
   std::vector<float>   *trig_L2_tau_cluster_stripWidthOffline;
   std::vector<std::vector<float> > *trig_L2_tau_cluster_EMenergyNor;
   std::vector<std::vector<float> > *trig_L2_tau_cluster_EMenergyNar;
   std::vector<std::vector<float> > *trig_L2_tau_cluster_HADenergyNor;
   std::vector<std::vector<float> > *trig_L2_tau_cluster_HADenergyNar;
   std::vector<float>   *trig_L2_tau_cluster_etNar;
   std::vector<int>     *trig_L2_tau_tracksinfo_nCoreTracks;
   std::vector<int>     *trig_L2_tau_tracksinfo_nSlowTracks;
   std::vector<int>     *trig_L2_tau_tracksinfo_nIsoTracks;
   std::vector<float>   *trig_L2_tau_tracksinfo_charge;
   std::vector<float>   *trig_L2_tau_tracksinfo_leadingTrackPt;
   std::vector<float>   *trig_L2_tau_tracksinfo_scalarPtSumCore;
   std::vector<float>   *trig_L2_tau_tracksinfo_scalarPtSumIso;
   std::vector<double>  *trig_L2_tau_tracksinfo_3fastest_pt;
   std::vector<double>  *trig_L2_tau_tracksinfo_3fastest_eta;
   std::vector<double>  *trig_L2_tau_tracksinfo_3fastest_phi;
   std::vector<double>  *trig_L2_tau_tracksinfo_3fastest_m;
   Int_t           trig_RoI_L2_tau_n;
   std::vector<short>   *trig_RoI_L2_tau_type;
   std::vector<short>   *trig_RoI_L2_tau_lastStep;
   std::vector<int>     *trig_RoI_L2_tau_TrigRoiDescriptor;
   std::vector<int>     *trig_RoI_L2_tau_TrigRoiDescriptorStatus;
   std::vector<int>     *trig_RoI_L2_tau_TrigTauCluster;
   std::vector<int>     *trig_RoI_L2_tau_TrigTauClusterStatus;
   std::vector<int>     *trig_RoI_L2_tau_TrigTauClusterDetails;
   std::vector<int>     *trig_RoI_L2_tau_TrigTauClusterDetailsStatus;
   std::vector<int>     *trig_RoI_L2_tau_TrigTauTracksInfo;
   std::vector<int>     *trig_RoI_L2_tau_TrigTauTracksInfoStatus;
   std::vector<int>     *trig_RoI_L2_tau_TrigTau;
   std::vector<int>     *trig_RoI_L2_tau_TrigTauStatus;
   std::vector<int>     *trig_RoI_L2_tau_EmTau_ROI;
   std::vector<int>     *trig_RoI_L2_tau_EmTau_ROIStatus;
   std::vector<std::vector<int> > *trig_RoI_L2_tau_TrigInDetTrackCollection;
   std::vector<std::vector<int> > *trig_RoI_L2_tau_TrigInDetTrackCollectionStatus;
   Int_t           trig_RoI_EF_tau_n;
   std::vector<short>   *trig_RoI_EF_tau_type;
   std::vector<short>   *trig_RoI_EF_tau_lastStep;
   std::vector<int>     *trig_RoI_EF_tau_EmTau_ROI;
   std::vector<int>     *trig_RoI_EF_tau_EmTau_ROIStatus;
//    std::vector<std::vector<int> > *trig_RoI_EF_tau_Analysis::TauJetContainer;
//    std::vector<std::vector<int> > *trig_RoI_EF_tau_Analysis::TauJetContainerStatus;
   std::vector<unsigned int> *trig_L1_TAV;
   std::vector<short>   *trig_L2_passedPhysics;
   std::vector<short>   *trig_EF_passedPhysics;
   std::vector<unsigned int> *trig_L1_TBP;
   std::vector<unsigned int> *trig_L1_TAP;
   std::vector<short>   *trig_L2_passedRaw;
   std::vector<short>   *trig_EF_passedRaw;
   std::vector<short>   *trig_L2_resurrected;
   std::vector<short>   *trig_EF_resurrected;
   std::vector<short>   *trig_L2_prescaled;
   std::vector<short>   *trig_EF_prescaled;

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
   TBranch        *b_el_breminvpt;   //!
   TBranch        *b_el_bremradius;   //!
   TBranch        *b_el_bremx;   //!
   TBranch        *b_el_bremclusterradius;   //!
   TBranch        *b_el_breminvpterr;   //!
   TBranch        *b_el_bremtrackauthor;   //!
   TBranch        *b_el_hasbrem;   //!
   TBranch        *b_el_bremdeltaqoverp;   //!
   TBranch        *b_el_bremmaterialtraversed;   //!
   TBranch        *b_el_refittedtrackqoverp;   //!
   TBranch        *b_el_refittedtrackd0;   //!
   TBranch        *b_el_refittedtrackz0;   //!
   TBranch        *b_el_refittedtracktheta;   //!
   TBranch        *b_el_refittedtrackphi;   //!
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
   TBranch        *b_el_vertx;   //!
   TBranch        *b_el_verty;   //!
   TBranch        *b_el_vertz;   //!
   TBranch        *b_el_hastrack;   //!
   TBranch        *b_el_deltaEmax2;   //!
   TBranch        *b_el_trackd0beam;   //!
   TBranch        *b_el_isIso;   //!
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
   TBranch        *b_el_EF_dr;   //!
   TBranch        *b_el_EF_index;   //!
   TBranch        *b_el_L2_dr;   //!
   TBranch        *b_el_L2_index;   //!
   TBranch        *b_el_L1_dr;   //!
   TBranch        *b_el_L1_index;   //!
   TBranch        *b_el_EF_E;   //!
   TBranch        *b_el_EF_Et;   //!
   TBranch        *b_el_EF_pt;   //!
   TBranch        *b_el_EF_eta;   //!
   TBranch        *b_el_EF_phi;   //!
   TBranch        *b_el_EF_charge;   //!
   TBranch        *b_el_EF_Ethad;   //!
   TBranch        *b_el_EF_Ethad1;   //!
   TBranch        *b_el_EF_f1;   //!
   TBranch        *b_el_EF_Emins1;   //!
   TBranch        *b_el_EF_fside;   //!
   TBranch        *b_el_EF_Emax2;   //!
   TBranch        *b_el_EF_ws3;   //!
   TBranch        *b_el_EF_wstot;   //!
   TBranch        *b_el_EF_E233;   //!
   TBranch        *b_el_EF_E237;   //!
   TBranch        *b_el_EF_E277;   //!
   TBranch        *b_el_EF_weta2;   //!
   TBranch        *b_el_EF_rphiallcalo;   //!
   TBranch        *b_el_EF_deltaeta1;   //!
   TBranch        *b_el_EF_deltaeta2;   //!
   TBranch        *b_el_EF_deltaphi2;   //!
   TBranch        *b_el_EF_expectHitInBLayer;   //!
   TBranch        *b_el_EF_Etcone45;   //!
   TBranch        *b_el_EF_Etcone20;   //!
   TBranch        *b_el_EF_Etcone30;   //!
   TBranch        *b_el_EF_Etcone40;   //!
   TBranch        *b_el_EF_etacorrmag;   //!
   TBranch        *b_el_EF_zvertex;   //!
   TBranch        *b_el_EF_errz;   //!
   TBranch        *b_el_EF_trackd0;   //!
   TBranch        *b_el_EF_trackz0;   //!
   TBranch        *b_el_EF_trackphi;   //!
   TBranch        *b_el_EF_tracktheta;   //!
   TBranch        *b_el_EF_trackqoverp;   //!
   TBranch        *b_el_EF_trackpt;   //!
   TBranch        *b_el_EF_tracketa;   //!
   TBranch        *b_el_EF_nBLHits;   //!
   TBranch        *b_el_EF_nPixHits;   //!
   TBranch        *b_el_EF_nSCTHits;   //!
   TBranch        *b_el_EF_nTRTHits;   //!
   TBranch        *b_el_EF_nTRTHighTHits;   //!
   TBranch        *b_el_EF_nSiHits;   //!
   TBranch        *b_el_EF_TRTHighTHitsRatio;   //!
   TBranch        *b_el_EF_pixeldEdx;   //!
   TBranch        *b_el_EF_hastrack;   //!
   TBranch        *b_el_EF_matched;   //!
   TBranch        *b_el_L2_E;   //!
   TBranch        *b_el_L2_Et;   //!
   TBranch        *b_el_L2_pt;   //!
   TBranch        *b_el_L2_eta;   //!
   TBranch        *b_el_L2_phi;   //!
   TBranch        *b_el_L2_charge;   //!
   TBranch        *b_el_L2_Ethad1;   //!
   TBranch        *b_el_L2_reta;   //!
   TBranch        *b_el_L2_Eratio;   //!
   TBranch        *b_el_L2_trackclusterdeta;   //!
   TBranch        *b_el_L2_trackclusterdphi;   //!
   TBranch        *b_el_L2_Etoverpt;   //!
   TBranch        *b_el_L2_trackpt;   //!
   TBranch        *b_el_L2_trackalgo;   //!
   TBranch        *b_el_L2_tracketa;   //!
   TBranch        *b_el_L2_trackd0;   //!
   TBranch        *b_el_L2_trackz0;   //!
   TBranch        *b_el_L2_tracktheta;   //!
   TBranch        *b_el_L2_trackphi;   //!
   TBranch        *b_el_L2_tracketaatcalo;   //!
   TBranch        *b_el_L2_trackphiatcalo;   //!
   TBranch        *b_el_L2_errpt;   //!
   TBranch        *b_el_L2_erreta;   //!
   TBranch        *b_el_L2_errphi;   //!
   TBranch        *b_el_L2_E237;   //!
   TBranch        *b_el_L2_E277;   //!
   TBranch        *b_el_L2_fside;   //!
   TBranch        *b_el_L2_weta2;   //!
   TBranch        *b_el_L2_Emaxs1;   //!
   TBranch        *b_el_L2_Emax2;   //!
   TBranch        *b_el_L2_matched;   //!
   TBranch        *b_el_L1_eta;   //!
   TBranch        *b_el_L1_phi;   //!
   TBranch        *b_el_L1_EMisol;   //!
   TBranch        *b_el_L1_hadisol;   //!
   TBranch        *b_el_L1_hadcore;   //!
   TBranch        *b_el_L1_pt;   //!
   TBranch        *b_el_L1_Et;   //!
   TBranch        *b_el_L1_matched;   //!
   TBranch        *b_EF_2e3_NoCut;   //!
   TBranch        *b_EF_2e3_loose;   //!
   TBranch        *b_EF_2e3_medium;   //!
   TBranch        *b_EF_2e5_loose;   //!
   TBranch        *b_EF_2e5_medium;   //!
   TBranch        *b_EF_2e6_nocut;   //!
   TBranch        *b_EF_2g10_nocut;   //!
   TBranch        *b_EF_2g3_loose;   //!
   TBranch        *b_EF_2g3_nocut;   //!
   TBranch        *b_EF_2g5_loose;   //!
   TBranch        *b_EF_2mu10;   //!
   TBranch        *b_EF_2mu4;   //!
   TBranch        *b_EF_2mu4_DiMu;   //!
   TBranch        *b_EF_2mu4_DiMu_SiTrk;   //!
   TBranch        *b_EF_2mu4_DiMu_noOS;   //!
   TBranch        *b_EF_2mu4_DiMu_noOS_passL2;   //!
   TBranch        *b_EF_2mu4_DiMu_noVtx_noOS;   //!
   TBranch        *b_EF_2mu4_DiMu_noVtx_noOS_passL2;   //!
   TBranch        *b_EF_2mu4_MSonly;   //!
   TBranch        *b_EF_2mu6;   //!
   TBranch        *b_EF_2mu6_DiMu;   //!
   TBranch        *b_EF_2mu6_DiMu_noVtx_noOS;   //!
   TBranch        *b_EF_e10_loose;   //!
   TBranch        *b_EF_e10_loose_IdScan;   //!
   TBranch        *b_EF_e10_loose_TRT;   //!
   TBranch        *b_EF_e10_loose_passEF;   //!
   TBranch        *b_EF_e10_loose_passL2;   //!
   TBranch        *b_EF_e10_medium;   //!
   TBranch        *b_EF_e10_medium_IdScan;   //!
   TBranch        *b_EF_e10_medium_TRT;   //!
   TBranch        *b_EF_e10i_loose;   //!
   TBranch        *b_EF_e15_loose;   //!
   TBranch        *b_EF_e15i_loose;   //!
   TBranch        *b_EF_e20_loose_NoIDTrkCut;   //!
   TBranch        *b_EF_e3_NoCut;   //!
   TBranch        *b_EF_e3_NoCut_IdScan;   //!
   TBranch        *b_EF_e3_NoCut_TRT;   //!
   TBranch        *b_EF_e3_loose;   //!
   TBranch        *b_EF_e3_loose_IdScan;   //!
   TBranch        *b_EF_e3_loose_TRT;   //!
   TBranch        *b_EF_e3_medium;   //!
   TBranch        *b_EF_e3_medium_IdScan;   //!
   TBranch        *b_EF_e3_medium_TRT;   //!
   TBranch        *b_EF_e5_EFfullcalo;   //!
   TBranch        *b_EF_e5_NoCut;   //!
   TBranch        *b_EF_e5_NoCut_FwdBackTrk;   //!
   TBranch        *b_EF_e5_NoCut_IdScan;   //!
   TBranch        *b_EF_e5_NoCut_L2SW;   //!
   TBranch        *b_EF_e5_NoCut_Ringer;   //!
   TBranch        *b_EF_e5_NoCut_SiTrk;   //!
   TBranch        *b_EF_e5_NoCut_TRT;   //!
   TBranch        *b_EF_e5_NoCut_cells;   //!
   TBranch        *b_EF_e5_NoCut_cosmic;   //!
   TBranch        *b_EF_e5_NoCut_firstempty;   //!
   TBranch        *b_EF_e5_NoCut_unpaired;   //!
   TBranch        *b_EF_e5_loose;   //!
   TBranch        *b_EF_e5_loose_IdScan;   //!
   TBranch        *b_EF_e5_loose_NoIDTrkCut;   //!
   TBranch        *b_EF_e5_loose_TRT;   //!
   TBranch        *b_EF_e5_medium;   //!
   TBranch        *b_EF_e5_medium_IdScan;   //!
   TBranch        *b_EF_e5_medium_MV;   //!
   TBranch        *b_EF_e5_medium_TRT;   //!
   TBranch        *b_EF_e6_loose;   //!
   TBranch        *b_EF_g10_loose;   //!
   TBranch        *b_EF_g10_loose_larcalib;   //!
   TBranch        *b_EF_g10i_loose;   //!
   TBranch        *b_EF_g15_loose;   //!
   TBranch        *b_EF_g15i_loose;   //!
   TBranch        *b_EF_g3_loose;   //!
   TBranch        *b_EF_g3_nocut;   //!
   TBranch        *b_EF_g3_nocut_larcalib;   //!
   TBranch        *b_EF_g3_nocut_unpaired;   //!
   TBranch        *b_EF_g5_EFfullcalo;   //!
   TBranch        *b_EF_g5_loose;   //!
   TBranch        *b_EF_g5_loose_larcalib;   //!
   TBranch        *b_EF_g5_nocut;   //!
   TBranch        *b_EF_g5_nocut_cosmic;   //!
   TBranch        *b_EF_g5_nocut_firstempty;   //!
   TBranch        *b_EF_g5_nocut_unpaired;   //!
   TBranch        *b_EF_g6_loose;   //!
   TBranch        *b_EF_mu0_missingRoi;   //!
   TBranch        *b_EF_mu0_rpcOnly;   //!
   TBranch        *b_EF_mu10;   //!
   TBranch        *b_EF_mu10_L2MSonly_EFFS_passL2;   //!
   TBranch        *b_EF_mu10_MG;   //!
   TBranch        *b_EF_mu10_MSonly;   //!
   TBranch        *b_EF_mu10_MSonly_EFFS_passL2;   //!
   TBranch        *b_EF_mu10_MSonly_cosmic;   //!
   TBranch        *b_EF_mu10_NoIDTrkCut;   //!
   TBranch        *b_EF_mu10_SiTrk;   //!
   TBranch        *b_EF_mu10_cosmic;   //!
   TBranch        *b_EF_mu10_muCombTag;   //!
   TBranch        *b_EF_mu10_muCombTag_SiTrk;   //!
   TBranch        *b_EF_mu10_passHLT;   //!
   TBranch        *b_EF_mu10i_loose;   //!
   TBranch        *b_EF_mu13;   //!
   TBranch        *b_EF_mu15;   //!
   TBranch        *b_EF_mu20;   //!
   TBranch        *b_EF_mu20_MSonly;   //!
   TBranch        *b_EF_mu20_passHLT;   //!
   TBranch        *b_EF_mu4;   //!
   TBranch        *b_EF_mu4_DiMu;   //!
   TBranch        *b_EF_mu4_DiMu_FS;   //!
   TBranch        *b_EF_mu4_DiMu_FS_noOS;   //!
   TBranch        *b_EF_mu4_Jpsie5e3;   //!
   TBranch        *b_EF_mu4_Jpsie5e3_FS;   //!
   TBranch        *b_EF_mu4_Jpsie5e3_SiTrk;   //!
   TBranch        *b_EF_mu4_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_EF_mu4_L2MSonly_EFFS_passL2;   //!
   TBranch        *b_EF_mu4_MG;   //!
   TBranch        *b_EF_mu4_MSonly;   //!
   TBranch        *b_EF_mu4_MSonly_EFFS_passL2;   //!
   TBranch        *b_EF_mu4_MSonly_MB2_noL2_EFFS;   //!
   TBranch        *b_EF_mu4_MSonly_Trk_Jpsi_loose;   //!
   TBranch        *b_EF_mu4_MSonly_Trk_Upsi_loose_FS;   //!
   TBranch        *b_EF_mu4_MSonly_cosmic;   //!
   TBranch        *b_EF_mu4_MSonly_firstempty;   //!
   TBranch        *b_EF_mu4_MV;   //!
   TBranch        *b_EF_mu4_NoIDTrkCut;   //!
   TBranch        *b_EF_mu4_SiTrk;   //!
   TBranch        *b_EF_mu4_Trk_Jpsi_loose;   //!
   TBranch        *b_EF_mu4_Trk_Upsi_loose_FS;   //!
   TBranch        *b_EF_mu4_comm;   //!
   TBranch        *b_EF_mu4_comm_MSonly;   //!
   TBranch        *b_EF_mu4_comm_MSonly_cosmic;   //!
   TBranch        *b_EF_mu4_comm_MSonly_firstempty;   //!
   TBranch        *b_EF_mu4_comm_cosmic;   //!
   TBranch        *b_EF_mu4_comm_firstempty;   //!
   TBranch        *b_EF_mu4_cosmic;   //!
   TBranch        *b_EF_mu4_firstempty;   //!
   TBranch        *b_EF_mu4_j10_matched;   //!
   TBranch        *b_EF_mu4_j20_matched;   //!
   TBranch        *b_EF_mu4_j5_matched;   //!
   TBranch        *b_EF_mu4_mu6;   //!
   TBranch        *b_EF_mu4_muCombTag;   //!
   TBranch        *b_EF_mu4_muCombTag_SiTrk;   //!
   TBranch        *b_EF_mu4_passHLT;   //!
   TBranch        *b_EF_mu4_tile;   //!
   TBranch        *b_EF_mu4_tile_SiTrk;   //!
   TBranch        *b_EF_mu4_trod;   //!
   TBranch        *b_EF_mu4_trod_SiTrk;   //!
   TBranch        *b_EF_mu4_unpaired;   //!
   TBranch        *b_EF_mu4mu6_DiMu;   //!
   TBranch        *b_EF_mu6;   //!
   TBranch        *b_EF_mu6_DiMu;   //!
   TBranch        *b_EF_mu6_DiMu_FS;   //!
   TBranch        *b_EF_mu6_DiMu_FS_noOS;   //!
   TBranch        *b_EF_mu6_DsPhiPi;   //!
   TBranch        *b_EF_mu6_Jpsie5e3;   //!
   TBranch        *b_EF_mu6_Jpsie5e3_FS;   //!
   TBranch        *b_EF_mu6_Jpsie5e3_SiTrk;   //!
   TBranch        *b_EF_mu6_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_EF_mu6_L2MSonly_EFFS_passL2;   //!
   TBranch        *b_EF_mu6_MG;   //!
   TBranch        *b_EF_mu6_MSonly;   //!
   TBranch        *b_EF_mu6_MSonly_EFFS_passL2;   //!
   TBranch        *b_EF_mu6_MSonly_cosmic;   //!
   TBranch        *b_EF_mu6_MSonly_firstempty;   //!
   TBranch        *b_EF_mu6_MSonly_unpaired;   //!
   TBranch        *b_EF_mu6_SiTrk;   //!
   TBranch        *b_EF_mu6_Trk_Jpsi_loose;   //!
   TBranch        *b_EF_mu6_Trk_Jpsi_loose_FS;   //!
   TBranch        *b_EF_mu6_Trk_Upsi_loose_FS;   //!
   TBranch        *b_EF_mu6_cosmic;   //!
   TBranch        *b_EF_mu6_firstempty;   //!
   TBranch        *b_EF_mu6_muCombTag;   //!
   TBranch        *b_EF_mu6_muCombTag_SiTrk;   //!
   TBranch        *b_EF_mu6_passHLT;   //!
   TBranch        *b_EF_mu6_unpaired;   //!
   TBranch        *b_EF_tau12_loose_xe20;   //!
   TBranch        *b_EF_tau16_loose_xe20;   //!
   TBranch        *b_EF_tauNoCut_hasTrk_xe20;   //!
   TBranch        *b_EF_xe20;   //!
   TBranch        *b_EF_xe20_FEB;   //!
   TBranch        *b_EF_xe20_allCells;   //!
   TBranch        *b_EF_xe20_noMu;   //!
   TBranch        *b_EF_xe25_noMu;   //!
   TBranch        *b_EF_xe30_allL1;   //!
   TBranch        *b_EF_xe30_allL1_FEB;   //!
   TBranch        *b_EF_xe30_allL1_allCells;   //!
   TBranch        *b_EF_xe30_noMu;   //!
   TBranch        *b_EF_xe30_unbiased;   //!
   TBranch        *b_EF_xe30_unbiased_FEB;   //!
   TBranch        *b_EF_xe30_unbiased_allCells;   //!
   TBranch        *b_EF_xe35_noMu;   //!
   TBranch        *b_EF_xe40_noMu;   //!
   TBranch        *b_EF_xe45_noMu;   //!
   TBranch        *b_EF_xe60_medium_noMu;   //!
   TBranch        *b_EF_xe70_medium_noMu;   //!
   TBranch        *b_EF_xe80_medium_noMu;   //!
   TBranch        *b_L1_2MU0;   //!
   TBranch        *b_L1_2MU0_MU6;   //!
   TBranch        *b_L1_2MU10;   //!
   TBranch        *b_L1_2MU6;   //!
   TBranch        *b_L1_EM10;   //!
   TBranch        *b_L1_EM10I;   //!
   TBranch        *b_L1_EM14;   //!
   TBranch        *b_L1_EM2;   //!
   TBranch        *b_L1_EM2_UNPAIRED;   //!
   TBranch        *b_L1_EM3;   //!
   TBranch        *b_L1_EM3_EMPTY;   //!
   TBranch        *b_L1_EM3_FIRSTEMPTY;   //!
   TBranch        *b_L1_EM3_MV;   //!
   TBranch        *b_L1_EM3_UNPAIRED;   //!
   TBranch        *b_L1_EM4;   //!
   TBranch        *b_L1_EM5;   //!
   TBranch        *b_L1_EM5I;   //!
   TBranch        *b_L1_MU0;   //!
   TBranch        *b_L1_MU0_COMM;   //!
   TBranch        *b_L1_MU0_COMM_EMPTY;   //!
   TBranch        *b_L1_MU0_COMM_FIRSTEMPTY;   //!
   TBranch        *b_L1_MU0_EM3;   //!
   TBranch        *b_L1_MU0_EMPTY;   //!
   TBranch        *b_L1_MU0_FIRSTEMPTY;   //!
   TBranch        *b_L1_MU0_J10;   //!
   TBranch        *b_L1_MU0_J15;   //!
   TBranch        *b_L1_MU0_J5;   //!
   TBranch        *b_L1_MU0_MV;   //!
   TBranch        *b_L1_MU0_UNPAIRED;   //!
   TBranch        *b_L1_MU10;   //!
   TBranch        *b_L1_MU10_EMPTY;   //!
   TBranch        *b_L1_MU15;   //!
   TBranch        *b_L1_MU20;   //!
   TBranch        *b_L1_MU6;   //!
   TBranch        *b_L1_MU6_EM3;   //!
   TBranch        *b_L1_MU6_EMPTY;   //!
   TBranch        *b_L1_MU6_FIRSTEMPTY;   //!
   TBranch        *b_L1_MU6_J5;   //!
   TBranch        *b_L1_MU6_UNPAIRED;   //!
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
   TBranch        *b_L2_2e3_NoCut;   //!
   TBranch        *b_L2_2e3_loose;   //!
   TBranch        *b_L2_2e3_medium;   //!
   TBranch        *b_L2_2e5_loose;   //!
   TBranch        *b_L2_2e5_medium;   //!
   TBranch        *b_L2_2e6_nocut;   //!
   TBranch        *b_L2_2g10_nocut;   //!
   TBranch        *b_L2_2g3_loose;   //!
   TBranch        *b_L2_2g3_nocut;   //!
   TBranch        *b_L2_2g5_loose;   //!
   TBranch        *b_L2_2mu10;   //!
   TBranch        *b_L2_2mu4;   //!
   TBranch        *b_L2_2mu4_DiMu;   //!
   TBranch        *b_L2_2mu4_DiMu_SiTrk;   //!
   TBranch        *b_L2_2mu4_DiMu_noOS;   //!
   TBranch        *b_L2_2mu4_DiMu_noOS_passL2;   //!
   TBranch        *b_L2_2mu4_DiMu_noVtx_noOS;   //!
   TBranch        *b_L2_2mu4_DiMu_noVtx_noOS_passL2;   //!
   TBranch        *b_L2_2mu4_MSonly;   //!
   TBranch        *b_L2_2mu6;   //!
   TBranch        *b_L2_2mu6_DiMu;   //!
   TBranch        *b_L2_2mu6_DiMu_noVtx_noOS;   //!
   TBranch        *b_L2_e10_loose;   //!
   TBranch        *b_L2_e10_loose_IdScan;   //!
   TBranch        *b_L2_e10_loose_TRT;   //!
   TBranch        *b_L2_e10_loose_passEF;   //!
   TBranch        *b_L2_e10_loose_passL2;   //!
   TBranch        *b_L2_e10_medium;   //!
   TBranch        *b_L2_e10_medium_IdScan;   //!
   TBranch        *b_L2_e10_medium_TRT;   //!
   TBranch        *b_L2_e10i_loose;   //!
   TBranch        *b_L2_e15_loose;   //!
   TBranch        *b_L2_e15i_loose;   //!
   TBranch        *b_L2_e20_loose_NoIDTrkCut;   //!
   TBranch        *b_L2_e3_NoCut;   //!
   TBranch        *b_L2_e3_NoCut_IdScan;   //!
   TBranch        *b_L2_e3_NoCut_TRT;   //!
   TBranch        *b_L2_e3_loose;   //!
   TBranch        *b_L2_e3_loose_IdScan;   //!
   TBranch        *b_L2_e3_loose_TRT;   //!
   TBranch        *b_L2_e3_medium;   //!
   TBranch        *b_L2_e3_medium_IdScan;   //!
   TBranch        *b_L2_e3_medium_TRT;   //!
   TBranch        *b_L2_e5_EFfullcalo;   //!
   TBranch        *b_L2_e5_NoCut;   //!
   TBranch        *b_L2_e5_NoCut_FwdBackTrk;   //!
   TBranch        *b_L2_e5_NoCut_IdScan;   //!
   TBranch        *b_L2_e5_NoCut_L2SW;   //!
   TBranch        *b_L2_e5_NoCut_Ringer;   //!
   TBranch        *b_L2_e5_NoCut_SiTrk;   //!
   TBranch        *b_L2_e5_NoCut_TRT;   //!
   TBranch        *b_L2_e5_NoCut_cells;   //!
   TBranch        *b_L2_e5_NoCut_cosmic;   //!
   TBranch        *b_L2_e5_NoCut_firstempty;   //!
   TBranch        *b_L2_e5_NoCut_unpaired;   //!
   TBranch        *b_L2_e5_loose;   //!
   TBranch        *b_L2_e5_loose_IdScan;   //!
   TBranch        *b_L2_e5_loose_NoIDTrkCut;   //!
   TBranch        *b_L2_e5_loose_TRT;   //!
   TBranch        *b_L2_e5_medium;   //!
   TBranch        *b_L2_e5_medium_IdScan;   //!
   TBranch        *b_L2_e5_medium_MV;   //!
   TBranch        *b_L2_e5_medium_TRT;   //!
   TBranch        *b_L2_e6_loose;   //!
   TBranch        *b_L2_g10_loose;   //!
   TBranch        *b_L2_g10i_loose;   //!
   TBranch        *b_L2_g15_loose;   //!
   TBranch        *b_L2_g15i_loose;   //!
   TBranch        *b_L2_g3_loose;   //!
   TBranch        *b_L2_g3_nocut;   //!
   TBranch        *b_L2_g3_nocut_unpaired;   //!
   TBranch        *b_L2_g5_EFfullcalo;   //!
   TBranch        *b_L2_g5_loose;   //!
   TBranch        *b_L2_g5_nocut;   //!
   TBranch        *b_L2_g5_nocut_cosmic;   //!
   TBranch        *b_L2_g5_nocut_firstempty;   //!
   TBranch        *b_L2_g5_nocut_unpaired;   //!
   TBranch        *b_L2_g6_loose;   //!
   TBranch        *b_L2_mu0_missingRoi;   //!
   TBranch        *b_L2_mu0_rpcOnly;   //!
   TBranch        *b_L2_mu10;   //!
   TBranch        *b_L2_mu10_L2MSonly_EFFS_passL2;   //!
   TBranch        *b_L2_mu10_MG;   //!
   TBranch        *b_L2_mu10_MSonly;   //!
   TBranch        *b_L2_mu10_MSonly_EFFS_passL2;   //!
   TBranch        *b_L2_mu10_MSonly_cosmic;   //!
   TBranch        *b_L2_mu10_NoIDTrkCut;   //!
   TBranch        *b_L2_mu10_SiTrk;   //!
   TBranch        *b_L2_mu10_cosmic;   //!
   TBranch        *b_L2_mu10_muCombTag;   //!
   TBranch        *b_L2_mu10_muCombTag_SiTrk;   //!
   TBranch        *b_L2_mu10_passHLT;   //!
   TBranch        *b_L2_mu10i_loose;   //!
   TBranch        *b_L2_mu13;   //!
   TBranch        *b_L2_mu15;   //!
   TBranch        *b_L2_mu20;   //!
   TBranch        *b_L2_mu20_MSonly;   //!
   TBranch        *b_L2_mu20_passHLT;   //!
   TBranch        *b_L2_mu4;   //!
   TBranch        *b_L2_mu4_DiMu;   //!
   TBranch        *b_L2_mu4_DiMu_FS;   //!
   TBranch        *b_L2_mu4_DiMu_FS_noOS;   //!
   TBranch        *b_L2_mu4_Jpsie5e3;   //!
   TBranch        *b_L2_mu4_Jpsie5e3_FS;   //!
   TBranch        *b_L2_mu4_Jpsie5e3_SiTrk;   //!
   TBranch        *b_L2_mu4_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_L2_mu4_L2MSonly_EFFS_passL2;   //!
   TBranch        *b_L2_mu4_MG;   //!
   TBranch        *b_L2_mu4_MSonly;   //!
   TBranch        *b_L2_mu4_MSonly_EFFS_passL2;   //!
   TBranch        *b_L2_mu4_MSonly_MB2_noL2_EFFS;   //!
   TBranch        *b_L2_mu4_MSonly_Trk_Jpsi_loose;   //!
   TBranch        *b_L2_mu4_MSonly_Trk_Upsi_loose_FS;   //!
   TBranch        *b_L2_mu4_MSonly_cosmic;   //!
   TBranch        *b_L2_mu4_MSonly_firstempty;   //!
   TBranch        *b_L2_mu4_MV;   //!
   TBranch        *b_L2_mu4_NoIDTrkCut;   //!
   TBranch        *b_L2_mu4_SiTrk;   //!
   TBranch        *b_L2_mu4_Trk_Jpsi_loose;   //!
   TBranch        *b_L2_mu4_Trk_Upsi_loose_FS;   //!
   TBranch        *b_L2_mu4_comm;   //!
   TBranch        *b_L2_mu4_comm_MSonly;   //!
   TBranch        *b_L2_mu4_comm_MSonly_cosmic;   //!
   TBranch        *b_L2_mu4_comm_MSonly_firstempty;   //!
   TBranch        *b_L2_mu4_comm_cosmic;   //!
   TBranch        *b_L2_mu4_comm_firstempty;   //!
   TBranch        *b_L2_mu4_cosmic;   //!
   TBranch        *b_L2_mu4_firstempty;   //!
   TBranch        *b_L2_mu4_j10_matched;   //!
   TBranch        *b_L2_mu4_j20_matched;   //!
   TBranch        *b_L2_mu4_j5_matched;   //!
   TBranch        *b_L2_mu4_mu6;   //!
   TBranch        *b_L2_mu4_muCombTag;   //!
   TBranch        *b_L2_mu4_muCombTag_SiTrk;   //!
   TBranch        *b_L2_mu4_passHLT;   //!
   TBranch        *b_L2_mu4_tile;   //!
   TBranch        *b_L2_mu4_tile_SiTrk;   //!
   TBranch        *b_L2_mu4_trod;   //!
   TBranch        *b_L2_mu4_trod_SiTrk;   //!
   TBranch        *b_L2_mu4_unpaired;   //!
   TBranch        *b_L2_mu4mu6_DiMu;   //!
   TBranch        *b_L2_mu6;   //!
   TBranch        *b_L2_mu6_DiMu;   //!
   TBranch        *b_L2_mu6_DiMu_FS;   //!
   TBranch        *b_L2_mu6_DiMu_FS_noOS;   //!
   TBranch        *b_L2_mu6_DsPhiPi;   //!
   TBranch        *b_L2_mu6_Jpsie5e3;   //!
   TBranch        *b_L2_mu6_Jpsie5e3_FS;   //!
   TBranch        *b_L2_mu6_Jpsie5e3_SiTrk;   //!
   TBranch        *b_L2_mu6_Jpsie5e3_SiTrk_FS;   //!
   TBranch        *b_L2_mu6_L2MSonly_EFFS_passL2;   //!
   TBranch        *b_L2_mu6_MG;   //!
   TBranch        *b_L2_mu6_MSonly;   //!
   TBranch        *b_L2_mu6_MSonly_EFFS_passL2;   //!
   TBranch        *b_L2_mu6_MSonly_cosmic;   //!
   TBranch        *b_L2_mu6_MSonly_firstempty;   //!
   TBranch        *b_L2_mu6_MSonly_unpaired;   //!
   TBranch        *b_L2_mu6_SiTrk;   //!
   TBranch        *b_L2_mu6_Trk_Jpsi_loose;   //!
   TBranch        *b_L2_mu6_Trk_Jpsi_loose_FS;   //!
   TBranch        *b_L2_mu6_Trk_Upsi_loose_FS;   //!
   TBranch        *b_L2_mu6_cosmic;   //!
   TBranch        *b_L2_mu6_firstempty;   //!
   TBranch        *b_L2_mu6_muCombTag;   //!
   TBranch        *b_L2_mu6_muCombTag_SiTrk;   //!
   TBranch        *b_L2_mu6_passHLT;   //!
   TBranch        *b_L2_mu6_unpaired;   //!
   TBranch        *b_L2_tau12_loose_xe20;   //!
   TBranch        *b_L2_tau16_loose_xe20;   //!
   TBranch        *b_L2_tauNoCut_hasTrk_xe20;   //!
   TBranch        *b_L2_xe12;   //!
   TBranch        *b_L2_xe12_FEB;   //!
   TBranch        *b_L2_xe12_allCells;   //!
   TBranch        *b_L2_xe12_noMu;   //!
   TBranch        *b_L2_xe15_noMu;   //!
   TBranch        *b_L2_xe20_noMu;   //!
   TBranch        *b_L2_xe25_noMu;   //!
   TBranch        *b_L2_xe30_allL1;   //!
   TBranch        *b_L2_xe30_allL1_FEB;   //!
   TBranch        *b_L2_xe30_allL1_allCells;   //!
   TBranch        *b_L2_xe30_noMu;   //!
   TBranch        *b_L2_xe30_unbiased;   //!
   TBranch        *b_L2_xe30_unbiased_FEB;   //!
   TBranch        *b_L2_xe30_unbiased_allCells;   //!
   TBranch        *b_L2_xe35_noMu;   //!
   TBranch        *b_L2_xe40_medium_noMu;   //!
   TBranch        *b_L2_xe50_medium_noMu;   //!
   TBranch        *b_L2_xe60_medium_noMu;   //!
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
   TBranch        *b_ph_isIso;   //!
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
   TBranch        *b_ph_trackIsol;   //!
   TBranch        *b_ph_convIP;   //!
   TBranch        *b_ph_convIPRev;   //!
   TBranch        *b_ph_HPV_zvertex;   //!
   TBranch        *b_ph_HPV_sigma_zvertex;   //!
   TBranch        *b_ph_HPV_zvertex_charged;   //!
   TBranch        *b_ph_HPV_sigma_zvertex_charged;   //!
   TBranch        *b_ph_HPV_zvertex_neutral;   //!
   TBranch        *b_ph_HPV_sigma_zvertex_neutral;   //!
   TBranch        *b_ph_HPV_eta;   //!
   TBranch        *b_ph_HPV_sigma_eta;   //!
   TBranch        *b_ph_HPV_convTrk1_zvertex;   //!
   TBranch        *b_ph_HPV_convTrk1_sigma_zvertex;   //!
   TBranch        *b_ph_HPV_convTrk2_zvertex;   //!
   TBranch        *b_ph_HPV_convTrk2_sigma_zvertex;   //!
   TBranch        *b_ph_ptIsolationCone;   //!
   TBranch        *b_ph_ptIsolationConePhAngle;   //!
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
   TBranch        *b_ph_EF_E;   //!
   TBranch        *b_ph_EF_Et;   //!
   TBranch        *b_ph_EF_pt;   //!
   TBranch        *b_ph_EF_eta;   //!
   TBranch        *b_ph_EF_phi;   //!
   TBranch        *b_ph_EF_Ethad;   //!
   TBranch        *b_ph_EF_Ethad1;   //!
   TBranch        *b_ph_EF_f1;   //!
   TBranch        *b_ph_EF_Emins1;   //!
   TBranch        *b_ph_EF_fside;   //!
   TBranch        *b_ph_EF_Emax2;   //!
   TBranch        *b_ph_EF_ws3;   //!
   TBranch        *b_ph_EF_wstot;   //!
   TBranch        *b_ph_EF_E233;   //!
   TBranch        *b_ph_EF_E237;   //!
   TBranch        *b_ph_EF_E277;   //!
   TBranch        *b_ph_EF_weta2;   //!
   TBranch        *b_ph_EF_rphiallcalo;   //!
   TBranch        *b_ph_EF_Etcone45;   //!
   TBranch        *b_ph_EF_Etcone20;   //!
   TBranch        *b_ph_EF_Etcone30;   //!
   TBranch        *b_ph_EF_Etcone40;   //!
   TBranch        *b_ph_EF_zvertex;   //!
   TBranch        *b_ph_EF_errz;   //!
   TBranch        *b_ph_EF_matched;   //!
   TBranch        *b_ph_L2_dr;   //!
   TBranch        *b_ph_L2_E;   //!
   TBranch        *b_ph_L2_Et;   //!
   TBranch        *b_ph_L2_pt;   //!
   TBranch        *b_ph_L2_eta;   //!
   TBranch        *b_ph_L2_phi;   //!
   TBranch        *b_ph_L2_Ethad1;   //!
   TBranch        *b_ph_L2_reta;   //!
   TBranch        *b_ph_L2_Eratio;   //!
   TBranch        *b_ph_L2_E237;   //!
   TBranch        *b_ph_L2_E277;   //!
   TBranch        *b_ph_L2_fside;   //!
   TBranch        *b_ph_L2_weta2;   //!
   TBranch        *b_ph_L2_Emaxs1;   //!
   TBranch        *b_ph_L2_Emax2;   //!
   TBranch        *b_ph_L2_matched;   //!
   TBranch        *b_ph_L1_dr;   //!
   TBranch        *b_ph_L1_eta;   //!
   TBranch        *b_ph_L1_phi;   //!
   TBranch        *b_ph_L1_EMisol;   //!
   TBranch        *b_ph_L1_hadisol;   //!
   TBranch        *b_ph_L1_hadcore;   //!
   TBranch        *b_ph_L1_pt;   //!
   TBranch        *b_ph_L1_Et;   //!
   TBranch        *b_ph_L1_matched;   //!
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
   TBranch        *b_mu_bestMatch;   //!
   TBranch        *b_mu_isStandAloneMuon;   //!
   TBranch        *b_mu_isCombinedMuon;   //!
   TBranch        *b_mu_isLowPtReconstructedMuon;   //!
   TBranch        *b_mu_d0_exPV;   //!
   TBranch        *b_mu_z0_exPV;   //!
   TBranch        *b_mu_phi_exPV;   //!
   TBranch        *b_mu_theta_exPV;   //!
   TBranch        *b_mu_qoverp_exPV;   //!
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
   TBranch        *b_mu_msextrap_d0;   //!
   TBranch        *b_mu_msextrap_z0;   //!
   TBranch        *b_mu_msextrap_phi;   //!
   TBranch        *b_mu_msextrap_theta;   //!
   TBranch        *b_mu_msextrap_qoverp;   //!
   TBranch        *b_mu_idextrap_d0;   //!
   TBranch        *b_mu_idextrap_z0;   //!
   TBranch        *b_mu_idextrap_phi;   //!
   TBranch        *b_mu_idextrap_theta;   //!
   TBranch        *b_mu_idextrap_qoverp;   //!
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
   TBranch        *b_mu_seg_n;   //!
   TBranch        *b_mu_seg_x;   //!
   TBranch        *b_mu_seg_y;   //!
   TBranch        *b_mu_seg_z;   //!
   TBranch        *b_mu_seg_phi;   //!
   TBranch        *b_mu_seg_theta;   //!
   TBranch        *b_mu_seg_locX;   //!
   TBranch        *b_mu_seg_locY;   //!
   TBranch        *b_mu_seg_locAngleXZ;   //!
   TBranch        *b_mu_seg_locAngleYZ;   //!
   TBranch        *b_mu_seg_sector;   //!
   TBranch        *b_mu_seg_stationEta;   //!
   TBranch        *b_mu_seg_isEndcap;   //!
   TBranch        *b_mu_seg_stationName;   //!
   TBranch        *b_mu_seg_author;   //!
   TBranch        *b_mu_seg_chi2;   //!
   TBranch        *b_mu_seg_ndof;   //!
   TBranch        *b_jet_n;   //!
   TBranch        *b_jet_E;   //!
   TBranch        *b_jet_pt;   //!
   TBranch        *b_jet_m;   //!
   TBranch        *b_jet_eta;   //!
   TBranch        *b_jet_phi;   //!
   TBranch        *b_jet_WIDTH;   //!
   TBranch        *b_jet_n90;   //!
   TBranch        *b_jet_n90constituents;   //!
   TBranch        *b_jet_fracSamplingMax;   //!
   TBranch        *b_jet_SamplingMax;   //!
   TBranch        *b_jet_BCH_CORR_CELL;   //!
   TBranch        *b_jet_BCH_CORR_JET;   //!
   TBranch        *b_jet_BCH_CORR_JET_FORCELL;   //!
   TBranch        *b_jet_ENG_BAD_CELLS;   //!
   TBranch        *b_jet_N_BAD_CELLS;   //!
   TBranch        *b_jet_N_BAD_CELLS_CORR;   //!
   TBranch        *b_jet_BAD_CELLS_CORR_E;   //!
   TBranch        *b_jet_timing;   //!
   TBranch        *b_jet_quality;   //!
   TBranch        *b_jet_emfrac;   //!
   TBranch        *b_jet_GCWJES;   //!
   TBranch        *b_jet_EMJES;   //!
   TBranch        *b_jet_emscale_E;   //!
   TBranch        *b_jet_emscale_pt;   //!
   TBranch        *b_jet_emscale_m;   //!
   TBranch        *b_jet_emscale_eta;   //!
   TBranch        *b_jet_emscale_phi;   //!
   TBranch        *b_jet_jvtxf;   //!
   TBranch        *b_jet_jvtx_x;   //!
   TBranch        *b_jet_jvtx_y;   //!
   TBranch        *b_jet_jvtx_z;   //!
   TBranch        *b_jet_flavor_weight;   //!
   TBranch        *b_jet_el_dr;   //!
   TBranch        *b_jet_el_matched;   //!
   TBranch        *b_jet_mu_dr;   //!
   TBranch        *b_jet_mu_matched;   //!
   TBranch        *b_MET_RefFinal_etx;   //!
   TBranch        *b_MET_RefFinal_ety;   //!
   TBranch        *b_MET_RefFinal_sumet;   //!
   TBranch        *b_MET_RefFinal_et;   //!
   TBranch        *b_MET_RefFinal_phi;   //!
   TBranch        *b_MET_MuonBoy_etx;   //!
   TBranch        *b_MET_MuonBoy_ety;   //!
   TBranch        *b_MET_MuonBoy_sumet;   //!
   TBranch        *b_MET_MuonBoy_et;   //!
   TBranch        *b_MET_MuonBoy_phi;   //!
   TBranch        *b_MET_RefEle_etx;   //!
   TBranch        *b_MET_RefEle_ety;   //!
   TBranch        *b_MET_RefEle_sumet;   //!
   TBranch        *b_MET_RefEle_et;   //!
   TBranch        *b_MET_RefEle_phi;   //!
   TBranch        *b_MET_RefJet_etx;   //!
   TBranch        *b_MET_RefJet_ety;   //!
   TBranch        *b_MET_RefJet_sumet;   //!
   TBranch        *b_MET_RefJet_et;   //!
   TBranch        *b_MET_RefJet_phi;   //!
   TBranch        *b_MET_RefGamma_etx;   //!
   TBranch        *b_MET_RefGamma_ety;   //!
   TBranch        *b_MET_RefGamma_sumet;   //!
   TBranch        *b_MET_RefGamma_et;   //!
   TBranch        *b_MET_RefGamma_phi;   //!
   TBranch        *b_MET_RefTau_etx;   //!
   TBranch        *b_MET_RefTau_ety;   //!
   TBranch        *b_MET_RefTau_sumet;   //!
   TBranch        *b_MET_RefTau_et;   //!
   TBranch        *b_MET_RefTau_phi;   //!
   TBranch        *b_MET_Cryo_etx;   //!
   TBranch        *b_MET_Cryo_ety;   //!
   TBranch        *b_MET_Cryo_sumet;   //!
   TBranch        *b_MET_Cryo_et;   //!
   TBranch        *b_MET_Cryo_phi;   //!
   TBranch        *b_MET_CellOut_etx;   //!
   TBranch        *b_MET_CellOut_ety;   //!
   TBranch        *b_MET_CellOut_sumet;   //!
   TBranch        *b_MET_CellOut_et;   //!
   TBranch        *b_MET_CellOut_phi;   //!
   TBranch        *b_MET_MuonBoy_Spectro_etx;   //!
   TBranch        *b_MET_MuonBoy_Spectro_ety;   //!
   TBranch        *b_MET_MuonBoy_Spectro_sumet;   //!
   TBranch        *b_MET_MuonBoy_Spectro_et;   //!
   TBranch        *b_MET_MuonBoy_Spectro_phi;   //!
   TBranch        *b_MET_MuonBoy_Track_etx;   //!
   TBranch        *b_MET_MuonBoy_Track_ety;   //!
   TBranch        *b_MET_MuonBoy_Track_sumet;   //!
   TBranch        *b_MET_MuonBoy_Track_et;   //!
   TBranch        *b_MET_MuonBoy_Track_phi;   //!
   TBranch        *b_MET_CryoCone_etx;   //!
   TBranch        *b_MET_CryoCone_ety;   //!
   TBranch        *b_MET_CryoCone_sumet;   //!
   TBranch        *b_MET_CryoCone_et;   //!
   TBranch        *b_MET_CryoCone_phi;   //!
   TBranch        *b_MET_Muon_etx;   //!
   TBranch        *b_MET_Muon_ety;   //!
   TBranch        *b_MET_Muon_sumet;   //!
   TBranch        *b_MET_Muon_et;   //!
   TBranch        *b_MET_Muon_phi;   //!
   TBranch        *b_MET_Final_etx;   //!
   TBranch        *b_MET_Final_ety;   //!
   TBranch        *b_MET_Final_sumet;   //!
   TBranch        *b_MET_Final_et;   //!
   TBranch        *b_MET_Final_phi;   //!
   TBranch        *b_MET_TopoObj_etx;   //!
   TBranch        *b_MET_TopoObj_ety;   //!
   TBranch        *b_MET_TopoObj_sumet;   //!
   TBranch        *b_MET_TopoObj_et;   //!
   TBranch        *b_MET_TopoObj_phi;   //!
   TBranch        *b_MET_LocHadTopoObj_etx;   //!
   TBranch        *b_MET_LocHadTopoObj_ety;   //!
   TBranch        *b_MET_LocHadTopoObj_sumet;   //!
   TBranch        *b_MET_LocHadTopoObj_et;   //!
   TBranch        *b_MET_LocHadTopoObj_phi;   //!
   TBranch        *b_MET_Base_etx;   //!
   TBranch        *b_MET_Base_ety;   //!
   TBranch        *b_MET_Base_sumet;   //!
   TBranch        *b_MET_Base_et;   //!
   TBranch        *b_MET_Base_phi;   //!
   TBranch        *b_MET_Base0_etx;   //!
   TBranch        *b_MET_Base0_ety;   //!
   TBranch        *b_MET_Base0_sumet;   //!
   TBranch        *b_MET_Base0_et;   //!
   TBranch        *b_MET_Base0_phi;   //!
   TBranch        *b_MET_Calib_etx;   //!
   TBranch        *b_MET_Calib_ety;   //!
   TBranch        *b_MET_Calib_sumet;   //!
   TBranch        *b_MET_Calib_et;   //!
   TBranch        *b_MET_Calib_phi;   //!
   TBranch        *b_MET_Topo_etx;   //!
   TBranch        *b_MET_Topo_ety;   //!
   TBranch        *b_MET_Topo_sumet;   //!
   TBranch        *b_MET_Topo_et;   //!
   TBranch        *b_MET_Topo_phi;   //!
   TBranch        *b_MET_CorrTopo_etx;   //!
   TBranch        *b_MET_CorrTopo_ety;   //!
   TBranch        *b_MET_CorrTopo_sumet;   //!
   TBranch        *b_MET_CorrTopo_et;   //!
   TBranch        *b_MET_CorrTopo_phi;   //!
   TBranch        *b_MET_LocHadTopo_etx;   //!
   TBranch        *b_MET_LocHadTopo_ety;   //!
   TBranch        *b_MET_LocHadTopo_sumet;   //!
   TBranch        *b_MET_LocHadTopo_et;   //!
   TBranch        *b_MET_LocHadTopo_phi;   //!
   TBranch        *b_cl_n;   //!
   TBranch        *b_cl_E;   //!
   TBranch        *b_cl_pt;   //!
   TBranch        *b_cl_m;   //!
   TBranch        *b_cl_eta;   //!
   TBranch        *b_cl_phi;   //!
   TBranch        *b_cl_E_em;   //!
   TBranch        *b_cl_E_had;   //!
   TBranch        *b_cl_firstEdens;   //!
   TBranch        *b_cl_cellmaxfrac;   //!
   TBranch        *b_cl_longitudinal;   //!
   TBranch        *b_cl_secondlambda;   //!
   TBranch        *b_cl_lateral;   //!
   TBranch        *b_cl_secondR;   //!
   TBranch        *b_cl_centerlambda;   //!
   TBranch        *b_cl_deltaTheta;   //!
   TBranch        *b_cl_deltaPhi;   //!
   TBranch        *b_cl_time;   //!
   TBranch        *b_emcl_n;   //!
   TBranch        *b_emcl_E;   //!
   TBranch        *b_emcl_pt;   //!
   TBranch        *b_emcl_m;   //!
   TBranch        *b_emcl_eta;   //!
   TBranch        *b_emcl_phi;   //!
   TBranch        *b_emcl_E_em;   //!
   TBranch        *b_emcl_E_had;   //!
   TBranch        *b_emcl_firstEdens;   //!
   TBranch        *b_emcl_cellmaxfrac;   //!
   TBranch        *b_emcl_longitudinal;   //!
   TBranch        *b_emcl_secondlambda;   //!
   TBranch        *b_emcl_lateral;   //!
   TBranch        *b_emcl_secondR;   //!
   TBranch        *b_emcl_centerlambda;   //!
   TBranch        *b_emcl_deltaTheta;   //!
   TBranch        *b_emcl_deltaPhi;   //!
   TBranch        *b_emcl_time;   //!
   TBranch        *b_emcl_E_PreSamplerB;   //!
   TBranch        *b_emcl_E_EMB1;   //!
   TBranch        *b_emcl_E_EMB2;   //!
   TBranch        *b_emcl_E_EMB3;   //!
   TBranch        *b_emcl_E_PreSamplerE;   //!
   TBranch        *b_emcl_E_EME1;   //!
   TBranch        *b_emcl_E_EME2;   //!
   TBranch        *b_emcl_E_EME3;   //!
   TBranch        *b_emcl_E_HEC0;   //!
   TBranch        *b_emcl_E_HEC1;   //!
   TBranch        *b_emcl_E_HEC2;   //!
   TBranch        *b_emcl_E_HEC3;   //!
   TBranch        *b_emcl_E_TileBar0;   //!
   TBranch        *b_emcl_E_TileBar1;   //!
   TBranch        *b_emcl_E_TileBar2;   //!
   TBranch        *b_emcl_E_TileGap1;   //!
   TBranch        *b_emcl_E_TileGap2;   //!
   TBranch        *b_emcl_E_TileGap3;   //!
   TBranch        *b_emcl_E_TileExt0;   //!
   TBranch        *b_emcl_E_TileExt1;   //!
   TBranch        *b_emcl_E_TileExt2;   //!
   TBranch        *b_emcl_E_FCAL0;   //!
   TBranch        *b_emcl_E_FCAL1;   //!
   TBranch        *b_emcl_E_FCAL2;   //!
   TBranch        *b_trk_n;   //!
   TBranch        *b_trk_d0;   //!
   TBranch        *b_trk_z0;   //!
   TBranch        *b_trk_phi;   //!
   TBranch        *b_trk_theta;   //!
   TBranch        *b_trk_qoverp;   //!
   TBranch        *b_trk_pt;   //!
   TBranch        *b_trk_eta;   //!
   TBranch        *b_trk_err_d0;   //!
   TBranch        *b_trk_err_z0;   //!
   TBranch        *b_trk_err_phi;   //!
   TBranch        *b_trk_err_theta;   //!
   TBranch        *b_trk_err_qoverp;   //!
   TBranch        *b_trk_cov_d0;   //!
   TBranch        *b_trk_cov_z0;   //!
   TBranch        *b_trk_cov_phi;   //!
   TBranch        *b_trk_cov_theta;   //!
   TBranch        *b_trk_cov_qoverp;   //!
   TBranch        *b_trk_cov_d0_z0;   //!
   TBranch        *b_trk_cov_d0_phi;   //!
   TBranch        *b_trk_cov_d0_theta;   //!
   TBranch        *b_trk_cov_d0_qoverp;   //!
   TBranch        *b_trk_cov_z0_phi;   //!
   TBranch        *b_trk_cov_z0_theta;   //!
   TBranch        *b_trk_cov_z0_qoverp;   //!
   TBranch        *b_trk_cov_phi_theta;   //!
   TBranch        *b_trk_cov_phi_qoverp;   //!
   TBranch        *b_trk_cov_theta_qoverp;   //!
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
   TBranch        *b_trk_d0_wrtPV;   //!
   TBranch        *b_trk_z0_wrtPV;   //!
   TBranch        *b_trk_phi_wrtPV;   //!
   TBranch        *b_trk_theta_wrtPV;   //!
   TBranch        *b_trk_qoverp_wrtPV;   //!
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
   TBranch        *b_tau_n;   //!
   TBranch        *b_tau_Et;   //!
   TBranch        *b_tau_pt;   //!
   TBranch        *b_tau_m;   //!
   TBranch        *b_tau_eta;   //!
   TBranch        *b_tau_phi;   //!
   TBranch        *b_tau_charge;   //!
   TBranch        *b_tau_BDTEleScore;   //!
   TBranch        *b_tau_BDTJetScore;   //!
   TBranch        *b_tau_discCut;   //!
   TBranch        *b_tau_discCutTMVA;   //!
   TBranch        *b_tau_discLL;   //!
   TBranch        *b_tau_discNN;   //!
   TBranch        *b_tau_efficLL;   //!
   TBranch        *b_tau_efficNN;   //!
   TBranch        *b_tau_likelihood;   //!
   TBranch        *b_tau_tauJetNeuralNetwork;   //!
   TBranch        *b_tau_tauENeuralNetwork;   //!
   TBranch        *b_tau_tauElTauLikelihood;   //!
   TBranch        *b_tau_electronVetoLoose;   //!
   TBranch        *b_tau_electronVetoMedium;   //!
   TBranch        *b_tau_electronVetoTight;   //!
   TBranch        *b_tau_muonVeto;   //!
   TBranch        *b_tau_tauCutLoose;   //!
   TBranch        *b_tau_tauCutMedium;   //!
   TBranch        *b_tau_tauCutTight;   //!
   TBranch        *b_tau_tauCutSafeLoose;   //!
   TBranch        *b_tau_tauCutSafeMedium;   //!
   TBranch        *b_tau_tauCutSafeTight;   //!
   TBranch        *b_tau_tauCutSafeCaloLoose;   //!
   TBranch        *b_tau_tauCutSafeCaloMedium;   //!
   TBranch        *b_tau_tauCutSafeCaloTight;   //!
   TBranch        *b_tau_tauLlhLoose;   //!
   TBranch        *b_tau_tauLlhMedium;   //!
   TBranch        *b_tau_tauLlhTight;   //!
   TBranch        *b_tau_author;   //!
   TBranch        *b_tau_ROIword;   //!
   TBranch        *b_tau_nProng;   //!
   TBranch        *b_tau_numTrack;   //!
   TBranch        *b_tau_etOverPtLeadTrk;   //!
   TBranch        *b_tau_ipZ0SinThetaSigLeadTrk;   //!
   TBranch        *b_tau_leadTrkPt;   //!
   TBranch        *b_tau_nLooseTrk;   //!
   TBranch        *b_tau_nLooseConvTrk;   //!
   TBranch        *b_tau_nProngLoose;   //!
   TBranch        *b_tau_ipSigLeadTrk;   //!
   TBranch        *b_tau_ipSigLeadLooseTrk;   //!
   TBranch        *b_tau_etOverPtLeadLooseTrk;   //!
   TBranch        *b_tau_leadLooseTrkPt;   //!
   TBranch        *b_tau_chrgLooseTrk;   //!
   TBranch        *b_tau_massTrkSys;   //!
   TBranch        *b_tau_trkWidth2;   //!
   TBranch        *b_tau_trFlightPathSig;   //!
   TBranch        *b_tau_etEflow;   //!
   TBranch        *b_tau_mEflow;   //!
   TBranch        *b_tau_nPi0;   //!
   TBranch        *b_tau_ele_E237E277;   //!
   TBranch        *b_tau_ele_PresamplerFraction;   //!
   TBranch        *b_tau_ele_ECALFirstFraction;   //!
   TBranch        *b_tau_seedCalo_EMRadius;   //!
   TBranch        *b_tau_seedCalo_hadRadius;   //!
   TBranch        *b_tau_seedCalo_etEMAtEMScale;   //!
   TBranch        *b_tau_seedCalo_etHadAtEMScale;   //!
   TBranch        *b_tau_seedCalo_isolFrac;   //!
   TBranch        *b_tau_seedCalo_centFrac;   //!
   TBranch        *b_tau_seedCalo_stripWidth2;   //!
   TBranch        *b_tau_seedCalo_nStrip;   //!
   TBranch        *b_tau_seedCalo_etEMCalib;   //!
   TBranch        *b_tau_seedCalo_etHadCalib;   //!
   TBranch        *b_tau_seedCalo_eta;   //!
   TBranch        *b_tau_seedCalo_phi;   //!
   TBranch        *b_tau_seedCalo_nIsolLooseTrk;   //!
   TBranch        *b_tau_seedCalo_trkAvgDist;   //!
   TBranch        *b_tau_seedCalo_trkRmsDist;   //!
   TBranch        *b_tau_seedTrk_EMRadius;   //!
   TBranch        *b_tau_seedTrk_isolFrac;   //!
   TBranch        *b_tau_seedTrk_etChrgHadOverSumTrkPt;   //!
   TBranch        *b_tau_seedTrk_isolFracWide;   //!
   TBranch        *b_tau_seedTrk_etHadAtEMScale;   //!
   TBranch        *b_tau_seedTrk_etEMAtEMScale;   //!
   TBranch        *b_tau_seedTrk_etEMCL;   //!
   TBranch        *b_tau_seedTrk_etChrgEM;   //!
   TBranch        *b_tau_seedTrk_etNeuEM;   //!
   TBranch        *b_tau_seedTrk_etResNeuEM;   //!
   TBranch        *b_tau_seedTrk_hadLeakEt;   //!
   TBranch        *b_tau_seedTrk_sumEMCellEtOverLeadTrkPt;   //!
   TBranch        *b_tau_seedTrk_secMaxStripEt;   //!
   TBranch        *b_tau_seedTrk_stripWidth2;   //!
   TBranch        *b_tau_seedTrk_nStrip;   //!
   TBranch        *b_tau_seedTrk_etChrgHad;   //!
   TBranch        *b_tau_seedTrk_nOtherCoreTrk;   //!
   TBranch        *b_tau_seedTrk_nIsolTrk;   //!
   TBranch        *b_tau_seedTrk_etIsolEM;   //!
   TBranch        *b_tau_seedTrk_etIsolHad;   //!
   TBranch        *b_tau_calcVars_sumTrkPt;   //!
   TBranch        *b_tau_calcVars_numTopoClusters;   //!
   TBranch        *b_tau_calcVars_numEffTopoClusters;   //!
   TBranch        *b_tau_calcVars_topoInvMass;   //!
   TBranch        *b_tau_calcVars_effTopoInvMass;   //!
   TBranch        *b_tau_calcVars_topoMeanDeltaR;   //!
   TBranch        *b_tau_calcVars_effTopoMeanDeltaR;   //!
   TBranch        *b_tau_calcVars_etHadSumPtTracks;   //!
   TBranch        *b_tau_calcVars_etEMSumPtTracks;   //!
   TBranch        *b_tau_track_n;   //!
   TBranch        *b_tau_EF_dr;   //!
   TBranch        *b_tau_EF_matched;   //!
   TBranch        *b_tau_L2_dr;   //!
   TBranch        *b_tau_L2_E;   //!
   TBranch        *b_tau_L2_Et;   //!
   TBranch        *b_tau_L2_pt;   //!
   TBranch        *b_tau_L2_eta;   //!
   TBranch        *b_tau_L2_phi;   //!
   TBranch        *b_tau_L2_matched;   //!
   TBranch        *b_tau_L1_dr;   //!
   TBranch        *b_tau_L1_Et;   //!
   TBranch        *b_tau_L1_pt;   //!
   TBranch        *b_tau_L1_eta;   //!
   TBranch        *b_tau_L1_phi;   //!
   TBranch        *b_tau_L1_matched;   //!
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
   TBranch        *b_L1_MBTS_1_1_Col;   //!
   TBranch        *b_L1_MBTS_1_1_EMPTY;   //!
   TBranch        *b_L1_MBTS_1_1_FIRSTEMPTY;   //!
   TBranch        *b_L1_MBTS_1_1_UNPAIRED;   //!
   TBranch        *b_L1_MBTS_1_1_UNPAIRED1;   //!
   TBranch        *b_L1_MBTS_1_1_UNPAIRED2;   //!
   TBranch        *b_L1_MBTS_1_Col;   //!
   TBranch        *b_L1_MBTS_1_EMPTY;   //!
   TBranch        *b_L1_MBTS_1_FIRSTEMPTY;   //!
   TBranch        *b_L1_MBTS_1_UNPAIRED;   //!
   TBranch        *b_L1_MBTS_1_UNPAIRED1;   //!
   TBranch        *b_L1_MBTS_1_UNPAIRED2;   //!
   TBranch        *b_L1_MBTS_2;   //!
   TBranch        *b_L1_MBTS_2_Col;   //!
   TBranch        *b_L1_MBTS_2_EMPTY;   //!
   TBranch        *b_L1_MBTS_2_FIRSTEMPTY;   //!
   TBranch        *b_L1_MBTS_2_UNPAIRED;   //!
   TBranch        *b_L1_MBTS_2_UNPAIRED1;   //!
   TBranch        *b_L1_MBTS_2_UNPAIRED2;   //!
   TBranch        *b_L1_MBTS_4_4;   //!
   TBranch        *b_L1_MBTS_4_4_FIRSTEMPTY;   //!
   TBranch        *b_L1_MBTS_4_4_UNPAIRED;   //!
   TBranch        *b_L1_MBTS_4_4_UNPAIRED1;   //!
   TBranch        *b_L1_MBTS_4_4_UNPAIRED2;   //!
   TBranch        *b_L1_MBTS_A;   //!
   TBranch        *b_L1_MBTS_C;   //!
   TBranch        *b_collcand_passMBTSTime;   //!
   TBranch        *b_collcand_passCaloTime;   //!
   TBranch        *b_collcand_passTrigger;   //!
   TBranch        *b_collcand_pass;   //!
   TBranch        *b_vxp_n;   //!
   TBranch        *b_vxp_x;   //!
   TBranch        *b_vxp_y;   //!
   TBranch        *b_vxp_z;   //!
   TBranch        *b_vxp_chi2;   //!
   TBranch        *b_vxp_ndof;   //!
   TBranch        *b_vxp_err_x;   //!
   TBranch        *b_vxp_err_y;   //!
   TBranch        *b_vxp_err_z;   //!
   TBranch        *b_vxp_px;   //!
   TBranch        *b_vxp_py;   //!
   TBranch        *b_vxp_pz;   //!
   TBranch        *b_vxp_E;   //!
   TBranch        *b_vxp_m;   //!
   TBranch        *b_vxp_nTracks;   //!
   TBranch        *b_vxp_sumPt;   //!
   TBranch        *b_vxp_type;   //!
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
   TBranch        *b_trig_L1_emtau_L1_EM10;   //!
   TBranch        *b_trig_L1_emtau_L1_EM10I;   //!
   TBranch        *b_trig_L1_emtau_L1_EM14;   //!
   TBranch        *b_trig_L1_emtau_L1_EM2;   //!
   TBranch        *b_trig_L1_emtau_L1_EM2_UNPAIRED;   //!
   TBranch        *b_trig_L1_emtau_L1_EM3;   //!
   TBranch        *b_trig_L1_emtau_L1_EM3_EMPTY;   //!
   TBranch        *b_trig_L1_emtau_L1_EM3_FIRSTEMPTY;   //!
   TBranch        *b_trig_L1_emtau_L1_EM3_MV;   //!
   TBranch        *b_trig_L1_emtau_L1_EM3_UNPAIRED;   //!
   TBranch        *b_trig_L1_emtau_L1_EM4;   //!
   TBranch        *b_trig_L1_emtau_L1_EM5;   //!
   TBranch        *b_trig_L1_emtau_L1_EM5I;   //!
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
   TBranch        *b_trig_L2_trk_idscan_eGamma_n;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_algorithmId;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_trackStatus;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_chi2Ndof;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_nStrawHits;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_nHighThrHits;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_nPixelSpacePoints;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_nSCT_SpacePoints;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_a0;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_z0;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_phi0;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_eta;   //!
   TBranch        *b_trig_L2_trk_idscan_eGamma_pt;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_n;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_algorithmId;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_trackStatus;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_chi2Ndof;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_nStrawHits;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_nHighThrHits;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_nPixelSpacePoints;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_a0;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_z0;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_phi0;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_eta;   //!
   TBranch        *b_trig_L2_trk_sitrack_eGamma_pt;   //!
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
   TBranch        *b_trig_L2_el_L2_2e3_NoCut;   //!
   TBranch        *b_trig_L2_el_L2_2e3_loose;   //!
   TBranch        *b_trig_L2_el_L2_2e3_medium;   //!
   TBranch        *b_trig_L2_el_L2_2e5_loose;   //!
   TBranch        *b_trig_L2_el_L2_2e5_medium;   //!
   TBranch        *b_trig_L2_el_L2_2e6_nocut;   //!
   TBranch        *b_trig_L2_el_L2_e10_loose;   //!
   TBranch        *b_trig_L2_el_L2_e10_loose_IdScan;   //!
   TBranch        *b_trig_L2_el_L2_e10_loose_TRT;   //!
   TBranch        *b_trig_L2_el_L2_e10_loose_passEF;   //!
   TBranch        *b_trig_L2_el_L2_e10_loose_passL2;   //!
   TBranch        *b_trig_L2_el_L2_e10_medium;   //!
   TBranch        *b_trig_L2_el_L2_e10_medium_IdScan;   //!
   TBranch        *b_trig_L2_el_L2_e10_medium_TRT;   //!
   TBranch        *b_trig_L2_el_L2_e10i_loose;   //!
   TBranch        *b_trig_L2_el_L2_e15_loose;   //!
   TBranch        *b_trig_L2_el_L2_e15i_loose;   //!
   TBranch        *b_trig_L2_el_L2_e20_loose_NoIDTrkCut;   //!
   TBranch        *b_trig_L2_el_L2_e3_NoCut;   //!
   TBranch        *b_trig_L2_el_L2_e3_NoCut_IdScan;   //!
   TBranch        *b_trig_L2_el_L2_e3_NoCut_TRT;   //!
   TBranch        *b_trig_L2_el_L2_e3_loose;   //!
   TBranch        *b_trig_L2_el_L2_e3_loose_IdScan;   //!
   TBranch        *b_trig_L2_el_L2_e3_loose_TRT;   //!
   TBranch        *b_trig_L2_el_L2_e3_medium;   //!
   TBranch        *b_trig_L2_el_L2_e3_medium_IdScan;   //!
   TBranch        *b_trig_L2_el_L2_e3_medium_TRT;   //!
   TBranch        *b_trig_L2_el_L2_e5_EFfullcalo;   //!
   TBranch        *b_trig_L2_el_L2_e5_NoCut;   //!
   TBranch        *b_trig_L2_el_L2_e5_NoCut_FwdBackTrk;   //!
   TBranch        *b_trig_L2_el_L2_e5_NoCut_IdScan;   //!
   TBranch        *b_trig_L2_el_L2_e5_NoCut_L2SW;   //!
   TBranch        *b_trig_L2_el_L2_e5_NoCut_Ringer;   //!
   TBranch        *b_trig_L2_el_L2_e5_NoCut_SiTrk;   //!
   TBranch        *b_trig_L2_el_L2_e5_NoCut_TRT;   //!
   TBranch        *b_trig_L2_el_L2_e5_NoCut_cells;   //!
   TBranch        *b_trig_L2_el_L2_e5_NoCut_cosmic;   //!
   TBranch        *b_trig_L2_el_L2_e5_NoCut_firstempty;   //!
   TBranch        *b_trig_L2_el_L2_e5_NoCut_unpaired;   //!
   TBranch        *b_trig_L2_el_L2_e5_loose;   //!
   TBranch        *b_trig_L2_el_L2_e5_loose_IdScan;   //!
   TBranch        *b_trig_L2_el_L2_e5_loose_NoIDTrkCut;   //!
   TBranch        *b_trig_L2_el_L2_e5_loose_TRT;   //!
   TBranch        *b_trig_L2_el_L2_e5_medium;   //!
   TBranch        *b_trig_L2_el_L2_e5_medium_IdScan;   //!
   TBranch        *b_trig_L2_el_L2_e5_medium_MV;   //!
   TBranch        *b_trig_L2_el_L2_e5_medium_TRT;   //!
   TBranch        *b_trig_L2_el_L2_e6_loose;   //!
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
   TBranch        *b_trig_L2_ph_L2_2g10_nocut;   //!
   TBranch        *b_trig_L2_ph_L2_2g3_loose;   //!
   TBranch        *b_trig_L2_ph_L2_2g3_nocut;   //!
   TBranch        *b_trig_L2_ph_L2_2g5_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g10_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g10i_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g15_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g15i_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g3_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g3_nocut;   //!
   TBranch        *b_trig_L2_ph_L2_g3_nocut_unpaired;   //!
   TBranch        *b_trig_L2_ph_L2_g5_EFfullcalo;   //!
   TBranch        *b_trig_L2_ph_L2_g5_loose;   //!
   TBranch        *b_trig_L2_ph_L2_g5_nocut;   //!
   TBranch        *b_trig_L2_ph_L2_g5_nocut_cosmic;   //!
   TBranch        *b_trig_L2_ph_L2_g5_nocut_firstempty;   //!
   TBranch        *b_trig_L2_ph_L2_g5_nocut_unpaired;   //!
   TBranch        *b_trig_L2_ph_L2_g6_loose;   //!
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
   TBranch        *b_trig_EF_el_EF_2e3_NoCut;   //!
   TBranch        *b_trig_EF_el_EF_2e3_loose;   //!
   TBranch        *b_trig_EF_el_EF_2e3_medium;   //!
   TBranch        *b_trig_EF_el_EF_2e5_loose;   //!
   TBranch        *b_trig_EF_el_EF_2e5_medium;   //!
   TBranch        *b_trig_EF_el_EF_2e6_nocut;   //!
   TBranch        *b_trig_EF_el_EF_e10_loose;   //!
   TBranch        *b_trig_EF_el_EF_e10_loose_IdScan;   //!
   TBranch        *b_trig_EF_el_EF_e10_loose_TRT;   //!
   TBranch        *b_trig_EF_el_EF_e10_loose_passEF;   //!
   TBranch        *b_trig_EF_el_EF_e10_loose_passL2;   //!
   TBranch        *b_trig_EF_el_EF_e10_medium;   //!
   TBranch        *b_trig_EF_el_EF_e10_medium_IdScan;   //!
   TBranch        *b_trig_EF_el_EF_e10_medium_TRT;   //!
   TBranch        *b_trig_EF_el_EF_e10i_loose;   //!
   TBranch        *b_trig_EF_el_EF_e15_loose;   //!
   TBranch        *b_trig_EF_el_EF_e15i_loose;   //!
   TBranch        *b_trig_EF_el_EF_e20_loose_NoIDTrkCut;   //!
   TBranch        *b_trig_EF_el_EF_e3_NoCut;   //!
   TBranch        *b_trig_EF_el_EF_e3_NoCut_IdScan;   //!
   TBranch        *b_trig_EF_el_EF_e3_NoCut_TRT;   //!
   TBranch        *b_trig_EF_el_EF_e3_loose;   //!
   TBranch        *b_trig_EF_el_EF_e3_loose_IdScan;   //!
   TBranch        *b_trig_EF_el_EF_e3_loose_TRT;   //!
   TBranch        *b_trig_EF_el_EF_e3_medium;   //!
   TBranch        *b_trig_EF_el_EF_e3_medium_IdScan;   //!
   TBranch        *b_trig_EF_el_EF_e3_medium_TRT;   //!
   TBranch        *b_trig_EF_el_EF_e5_EFfullcalo;   //!
   TBranch        *b_trig_EF_el_EF_e5_NoCut;   //!
   TBranch        *b_trig_EF_el_EF_e5_NoCut_FwdBackTrk;   //!
   TBranch        *b_trig_EF_el_EF_e5_NoCut_IdScan;   //!
   TBranch        *b_trig_EF_el_EF_e5_NoCut_L2SW;   //!
   TBranch        *b_trig_EF_el_EF_e5_NoCut_Ringer;   //!
   TBranch        *b_trig_EF_el_EF_e5_NoCut_SiTrk;   //!
   TBranch        *b_trig_EF_el_EF_e5_NoCut_TRT;   //!
   TBranch        *b_trig_EF_el_EF_e5_NoCut_cells;   //!
   TBranch        *b_trig_EF_el_EF_e5_NoCut_cosmic;   //!
   TBranch        *b_trig_EF_el_EF_e5_NoCut_firstempty;   //!
   TBranch        *b_trig_EF_el_EF_e5_NoCut_unpaired;   //!
   TBranch        *b_trig_EF_el_EF_e5_loose;   //!
   TBranch        *b_trig_EF_el_EF_e5_loose_IdScan;   //!
   TBranch        *b_trig_EF_el_EF_e5_loose_NoIDTrkCut;   //!
   TBranch        *b_trig_EF_el_EF_e5_loose_TRT;   //!
   TBranch        *b_trig_EF_el_EF_e5_medium;   //!
   TBranch        *b_trig_EF_el_EF_e5_medium_IdScan;   //!
   TBranch        *b_trig_EF_el_EF_e5_medium_MV;   //!
   TBranch        *b_trig_EF_el_EF_e5_medium_TRT;   //!
   TBranch        *b_trig_EF_el_EF_e6_loose;   //!
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
   TBranch        *b_trig_EF_ph_EF_2g10_nocut;   //!
   TBranch        *b_trig_EF_ph_EF_2g3_loose;   //!
   TBranch        *b_trig_EF_ph_EF_2g3_nocut;   //!
   TBranch        *b_trig_EF_ph_EF_2g5_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g10_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g10_loose_larcalib;   //!
   TBranch        *b_trig_EF_ph_EF_g10i_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g15_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g15i_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g3_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g3_nocut;   //!
   TBranch        *b_trig_EF_ph_EF_g3_nocut_larcalib;   //!
   TBranch        *b_trig_EF_ph_EF_g3_nocut_unpaired;   //!
   TBranch        *b_trig_EF_ph_EF_g5_EFfullcalo;   //!
   TBranch        *b_trig_EF_ph_EF_g5_loose;   //!
   TBranch        *b_trig_EF_ph_EF_g5_loose_larcalib;   //!
   TBranch        *b_trig_EF_ph_EF_g5_nocut;   //!
   TBranch        *b_trig_EF_ph_EF_g5_nocut_cosmic;   //!
   TBranch        *b_trig_EF_ph_EF_g5_nocut_firstempty;   //!
   TBranch        *b_trig_EF_ph_EF_g5_nocut_unpaired;   //!
   TBranch        *b_trig_EF_ph_EF_g6_loose;   //!
   TBranch        *b_trig_Nav_n;   //!
   TBranch        *b_trig_Nav_chain_ChainId;   //!
   TBranch        *b_trig_Nav_chain_RoIType;   //!
   TBranch        *b_trig_Nav_chain_RoIIndex;   //!
   TBranch        *b_trig_RoI_L2_e_n;   //!
   TBranch        *b_trig_RoI_L2_e_type;   //!
   TBranch        *b_trig_RoI_L2_e_lastStep;   //!
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
   TBranch        *b_trig_RoI_EF_e_n;   //!
   TBranch        *b_trig_RoI_EF_e_type;   //!
   TBranch        *b_trig_RoI_EF_e_lastStep;   //!
   TBranch        *b_trig_RoI_EF_e_EmTau_ROI;   //!
   TBranch        *b_trig_RoI_EF_e_EmTau_ROIStatus;   //!
   TBranch        *b_trig_L1_mu_n;   //!
   TBranch        *b_trig_L1_mu_pt;   //!
   TBranch        *b_trig_L1_mu_eta;   //!
   TBranch        *b_trig_L1_mu_phi;   //!
   TBranch        *b_trig_L1_mu_thrName;   //!
   TBranch        *b_trig_L1_mu_thrNumber;   //!
   TBranch        *b_trig_L1_mu_RoINumber;   //!
   TBranch        *b_trig_L1_mu_sectorAddress;   //!
   TBranch        *b_trig_L1_mu_firstCandidate;   //!
   TBranch        *b_trig_L1_mu_moreCandInRoI;   //!
   TBranch        *b_trig_L1_mu_moreCandInSector;   //!
   TBranch        *b_trig_L1_mu_source;   //!
   TBranch        *b_trig_L1_mu_hemisphere;   //!
   TBranch        *b_trig_L1_mu_RoIWord;   //!
   TBranch        *b_trig_l2muonfeature_n;   //!
   TBranch        *b_trig_l2muonfeature_pt;   //!
   TBranch        *b_trig_l2muonfeature_eta;   //!
   TBranch        *b_trig_l2muonfeature_phi;   //!
   TBranch        *b_trig_l2muonfeature_dir_phi;   //!
   TBranch        *b_trig_l2muonfeature_zeta;   //!
   TBranch        *b_trig_l2muonfeature_dir_zeta;   //!
   TBranch        *b_trig_l2muonfeature_roiId;   //!
   TBranch        *b_trig_l2muonfeature_saddress;   //!
   TBranch        *b_trig_l2muonfeature_radius;   //!
   TBranch        *b_trig_l2muonfeature_beta;   //!
   TBranch        *b_trig_l2muonfeature_sp1_r;   //!
   TBranch        *b_trig_l2muonfeature_sp1_z;   //!
   TBranch        *b_trig_l2muonfeature_sp1_slope;   //!
   TBranch        *b_trig_l2muonfeature_sp2_r;   //!
   TBranch        *b_trig_l2muonfeature_sp2_z;   //!
   TBranch        *b_trig_l2muonfeature_sp2_slope;   //!
   TBranch        *b_trig_l2muonfeature_sp3_r;   //!
   TBranch        *b_trig_l2muonfeature_sp3_z;   //!
   TBranch        *b_trig_l2muonfeature_sp3_slope;   //!
   TBranch        *b_trig_l2muonfeature_br_radius;   //!
   TBranch        *b_trig_l2muonfeature_br_sagitta;   //!
   TBranch        *b_trig_l2muonfeature_ec_alpha;   //!
   TBranch        *b_trig_l2muonfeature_ec_beta;   //!
   TBranch        *b_trig_l2muonfeature_dq_var1;   //!
   TBranch        *b_trig_l2muonfeature_dq_var2;   //!
   TBranch        *b_trig_l2combmuonfeature_n;   //!
   TBranch        *b_trig_l2combmuonfeature_sigma_pt;   //!
   TBranch        *b_trig_l2combmuonfeature_pt;   //!
   TBranch        *b_trig_l2combmuonfeature_eta;   //!
   TBranch        *b_trig_l2combmuonfeature_phi;   //!
   TBranch        *b_trig_l2combmuonfeature_mf_pt;   //!
   TBranch        *b_trig_l2combmuonfeature_mf_eta;   //!
   TBranch        *b_trig_l2combmuonfeature_mf_phi;   //!
   TBranch        *b_trig_l2combmuonfeature_mf_dir_phi;   //!
   TBranch        *b_trig_l2combmuonfeature_mf_zeta;   //!
   TBranch        *b_trig_l2combmuonfeature_mf_dir_zeta;   //!
   TBranch        *b_trig_l2combmuonfeature_mf_hasmf;   //!
   TBranch        *b_trig_l2isomuonfeature_n;   //!
   TBranch        *b_trig_l2isomuonfeature_EtInnerConeEC ;   //!
   TBranch        *b_trig_l2isomuonfeature_EtOuterConeEC;   //!
   TBranch        *b_trig_l2isomuonfeature_EtInnerConeHC;   //!
   TBranch        *b_trig_l2isomuonfeature_EtOuterConeHC;   //!
   TBranch        *b_trig_l2isomuonfeature_NTracksCone;   //!
   TBranch        *b_trig_l2isomuonfeature_SumPtTracksCone;   //!
   TBranch        *b_trig_l2isomuonfeature_PtMuTracksCone;   //!
   TBranch        *b_trig_l2isomuonfeature_iso_strategy;   //!
   TBranch        *b_trig_l2isomuonfeature_RoiIdMu ;   //!
   TBranch        *b_trig_l2isomuonfeature_PtMu;   //!
   TBranch        *b_trig_l2isomuonfeature_EtaMu;   //!
   TBranch        *b_trig_l2isomuonfeature_PhiMu;   //!
   TBranch        *b_trig_l2isomuonfeature_LArWeight;   //!
   TBranch        *b_trig_l2isomuonfeature_TileWeight;   //!
   TBranch        *b_trig_l2isomuonfeature_pt;   //!
   TBranch        *b_trig_l2isomuonfeature_eta;   //!
   TBranch        *b_trig_l2isomuonfeature_phi;   //!
   TBranch        *b_trig_l2tilemufeature_n;   //!
   TBranch        *b_trig_l2tilemufeature_eta;   //!
   TBranch        *b_trig_l2tilemufeature_phi;   //!
   TBranch        *b_trig_l2tilemufeature_enedep;   //!
   TBranch        *b_trig_l2tilemufeature_quality;   //!
   TBranch        *b_trig_l2tiletrackmufeature_n;   //!
   TBranch        *b_trig_l2tiletrackmufeature_PtTR_Trk;   //!
   TBranch        *b_trig_l2tiletrackmufeature_EtaTR_Trk;   //!
   TBranch        *b_trig_l2tiletrackmufeature_PhiTR_Trk;   //!
   TBranch        *b_trig_l2tiletrackmufeature_Typ_IDTrk;   //!
   TBranch        *b_trig_l2tiletrackmufeature_pt;   //!
   TBranch        *b_trig_l2tiletrackmufeature_eta;   //!
   TBranch        *b_trig_l2tiletrackmufeature_phi;   //!
   TBranch        *b_trig_l2tiletrackmufeature_tilemu_eta;   //!
   TBranch        *b_trig_l2tiletrackmufeature_tilemu_phi;   //!
   TBranch        *b_trig_l2tiletrackmufeature_tilemu_enedep;   //!
   TBranch        *b_trig_l2tiletrackmufeature_tilemu_quality;   //!
   TBranch        *b_trig_l2tiletrackmufeature_tilemu_hastilemu;   //!
   TBranch        *b_trig_EF_tau_n;   //!
   TBranch        *b_trig_EF_tau_Et;   //!
   TBranch        *b_trig_EF_tau_pt;   //!
   TBranch        *b_trig_EF_tau_m;   //!
   TBranch        *b_trig_EF_tau_eta;   //!
   TBranch        *b_trig_EF_tau_phi;   //!
   TBranch        *b_trig_EF_tau_px;   //!
   TBranch        *b_trig_EF_tau_py;   //!
   TBranch        *b_trig_EF_tau_pz;   //!
   TBranch        *b_trig_EF_tau_RoIword;   //!
   TBranch        *b_trig_EF_tau_nLooseTrk;   //!
   TBranch        *b_trig_EF_tau_leadLooseTrkPt;   //!
   TBranch        *b_trig_EF_tau_leadLooseTrkEta;   //!
   TBranch        *b_trig_EF_tau_leadLooseTrkPhi;   //!
   TBranch        *b_trig_EF_tau_seedCalo_EMRadius;   //!
   TBranch        *b_trig_EF_tau_seedCalo_hadRadius;   //!
   TBranch        *b_trig_EF_tau_seedCalo_etEMAtEMScale;   //!
   TBranch        *b_trig_EF_tau_seedCalo_etHadAtEMScale;   //!
   TBranch        *b_trig_EF_tau_seedCalo_isolFrac;   //!
   TBranch        *b_trig_EF_tau_seedCalo_centFrac;   //!
   TBranch        *b_trig_EF_tau_seedCalo_etEMCalib;   //!
   TBranch        *b_trig_EF_tau_seedCalo_etHadCalib;   //!
   TBranch        *b_trig_EF_tau_seedCalo_eta;   //!
   TBranch        *b_trig_EF_tau_seedCalo_phi;   //!
   TBranch        *b_trig_EF_tau_invMassOfTrks;   //!
   TBranch        *b_trig_EF_tau_looseTrkWidth;   //!
   TBranch        *b_trig_EF_tau_looseTrkRadius;   //!
   TBranch        *b_trig_EF_tau_EF_tau12_loose;   //!
   TBranch        *b_trig_EF_tau_EF_tau12_loose_PT;   //!
   TBranch        *b_trig_EF_tau_EF_tau12_loose_e10_loose;   //!
   TBranch        *b_trig_EF_tau_EF_tau12_loose_mu10;   //!
   TBranch        *b_trig_EF_tau_EF_tau12_loose_xe20;   //!
   TBranch        *b_trig_EF_tau_EF_tau16_loose;   //!
   TBranch        *b_trig_EF_tau_EF_tau16_loose_3j40;   //!
   TBranch        *b_trig_EF_tau_EF_tau16_loose_TAU8;   //!
   TBranch        *b_trig_EF_tau_EF_tau16_loose_xe20;   //!
   TBranch        *b_trig_EF_tau_EF_tau16i_NoIDTrkCut;   //!
   TBranch        *b_trig_EF_tau_EF_tau16i_loose;   //!
   TBranch        *b_trig_EF_tau_EF_tau20_loose;   //!
   TBranch        *b_trig_EF_tau_EF_tau20_medium;   //!
   TBranch        *b_trig_EF_tau_EF_tau29_loose;   //!
   TBranch        *b_trig_EF_tau_EF_tau29_medium;   //!
   TBranch        *b_trig_EF_tau_EF_tau29i_loose;   //!
   TBranch        *b_trig_EF_tau_EF_tau50_loose;   //!
   TBranch        *b_trig_EF_tau_EF_tau50_loose_TAU30;   //!
   TBranch        *b_trig_EF_tau_EF_tauNoCut;   //!
   TBranch        *b_trig_EF_tau_EF_tauNoCut_NoIDTrkCut;   //!
   TBranch        *b_trig_EF_tau_EF_tauNoCut_SiTrk;   //!
   TBranch        *b_trig_EF_tau_EF_tauNoCut_cells;   //!
   TBranch        *b_trig_EF_tau_EF_tauNoCut_cosmic;   //!
   TBranch        *b_trig_EF_tau_EF_tauNoCut_firstempty;   //!
   TBranch        *b_trig_EF_tau_EF_tauNoCut_hasTrk;   //!
   TBranch        *b_trig_EF_tau_EF_tauNoCut_hasTrk_MV;   //!
   TBranch        *b_trig_EF_tau_EF_tauNoCut_hasTrk_SiTrk;   //!
   TBranch        *b_trig_EF_tau_EF_tauNoCut_hasTrk_xe20;   //!
   TBranch        *b_trig_EF_tau_EF_tauNoCut_unpaired;   //!
   TBranch        *b_trig_L2_tau_n;   //!
   TBranch        *b_trig_L2_tau_pt;   //!
   TBranch        *b_trig_L2_tau_eta;   //!
   TBranch        *b_trig_L2_tau_phi;   //!
   TBranch        *b_trig_L2_tau_px;   //!
   TBranch        *b_trig_L2_tau_py;   //!
   TBranch        *b_trig_L2_tau_pz;   //!
   TBranch        *b_trig_L2_tau_RoIword;   //!
   TBranch        *b_trig_L2_tau_simpleEtFlow;   //!
   TBranch        *b_trig_L2_tau_nMatchedTracks;   //!
   TBranch        *b_trig_L2_tau_L2_tau12_loose;   //!
   TBranch        *b_trig_L2_tau_L2_tau12_loose_PT;   //!
   TBranch        *b_trig_L2_tau_L2_tau12_loose_e10_loose;   //!
   TBranch        *b_trig_L2_tau_L2_tau12_loose_mu10;   //!
   TBranch        *b_trig_L2_tau_L2_tau12_loose_xe20;   //!
   TBranch        *b_trig_L2_tau_L2_tau16_loose;   //!
   TBranch        *b_trig_L2_tau_L2_tau16_loose_3j30;   //!
   TBranch        *b_trig_L2_tau_L2_tau16_loose_TAU8;   //!
   TBranch        *b_trig_L2_tau_L2_tau16_loose_xe20;   //!
   TBranch        *b_trig_L2_tau_L2_tau16i_NoIDTrkCut;   //!
   TBranch        *b_trig_L2_tau_L2_tau16i_loose;   //!
   TBranch        *b_trig_L2_tau_L2_tau20_loose;   //!
   TBranch        *b_trig_L2_tau_L2_tau20_medium;   //!
   TBranch        *b_trig_L2_tau_L2_tau29_loose;   //!
   TBranch        *b_trig_L2_tau_L2_tau29_medium;   //!
   TBranch        *b_trig_L2_tau_L2_tau29i_loose;   //!
   TBranch        *b_trig_L2_tau_L2_tau50_loose;   //!
   TBranch        *b_trig_L2_tau_L2_tau50_loose_TAU30;   //!
   TBranch        *b_trig_L2_tau_L2_tauNoCut;   //!
   TBranch        *b_trig_L2_tau_L2_tauNoCut_NoIDTrkCut;   //!
   TBranch        *b_trig_L2_tau_L2_tauNoCut_SiTrk;   //!
   TBranch        *b_trig_L2_tau_L2_tauNoCut_cells;   //!
   TBranch        *b_trig_L2_tau_L2_tauNoCut_cosmic;   //!
   TBranch        *b_trig_L2_tau_L2_tauNoCut_firstempty;   //!
   TBranch        *b_trig_L2_tau_L2_tauNoCut_hasTrk;   //!
   TBranch        *b_trig_L2_tau_L2_tauNoCut_hasTrk_MV;   //!
   TBranch        *b_trig_L2_tau_L2_tauNoCut_hasTrk_SiTrk;   //!
   TBranch        *b_trig_L2_tau_L2_tauNoCut_hasTrk_xe20;   //!
   TBranch        *b_trig_L2_tau_L2_tauNoCut_unpaired;   //!
   TBranch        *b_trig_L2_tau_cluster_quality;   //!
   TBranch        *b_trig_L2_tau_cluster_EMenergy;   //!
   TBranch        *b_trig_L2_tau_cluster_HADenergy;   //!
   TBranch        *b_trig_L2_tau_cluster_eta;   //!
   TBranch        *b_trig_L2_tau_cluster_phi;   //!
   TBranch        *b_trig_L2_tau_cluster_EMRadius2;   //!
   TBranch        *b_trig_L2_tau_cluster_CaloRadius;   //!
   TBranch        *b_trig_L2_tau_cluster_IsoFrac;   //!
   TBranch        *b_trig_L2_tau_cluster_numTotCells;   //!
   TBranch        *b_trig_L2_tau_cluster_stripWidth;   //!
   TBranch        *b_trig_L2_tau_cluster_stripWidthOffline;   //!
   TBranch        *b_trig_L2_tau_cluster_EMenergyNor;   //!
   TBranch        *b_trig_L2_tau_cluster_EMenergyNar;   //!
   TBranch        *b_trig_L2_tau_cluster_HADenergyNor;   //!
   TBranch        *b_trig_L2_tau_cluster_HADenergyNar;   //!
   TBranch        *b_trig_L2_tau_cluster_etNar;   //!
   TBranch        *b_trig_L2_tau_tracksinfo_nCoreTracks;   //!
   TBranch        *b_trig_L2_tau_tracksinfo_nSlowTracks;   //!
   TBranch        *b_trig_L2_tau_tracksinfo_nIsoTracks;   //!
   TBranch        *b_trig_L2_tau_tracksinfo_charge;   //!
   TBranch        *b_trig_L2_tau_tracksinfo_leadingTrackPt;   //!
   TBranch        *b_trig_L2_tau_tracksinfo_scalarPtSumCore;   //!
   TBranch        *b_trig_L2_tau_tracksinfo_scalarPtSumIso;   //!
   TBranch        *b_trig_L2_tau_tracksinfo_3fastest_pt;   //!
   TBranch        *b_trig_L2_tau_tracksinfo_3fastest_eta;   //!
   TBranch        *b_trig_L2_tau_tracksinfo_3fastest_phi;   //!
   TBranch        *b_trig_L2_tau_tracksinfo_3fastest_m;   //!
   TBranch        *b_trig_RoI_L2_tau_n;   //!
   TBranch        *b_trig_RoI_L2_tau_type;   //!
   TBranch        *b_trig_RoI_L2_tau_lastStep;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigRoiDescriptor;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigRoiDescriptorStatus;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigTauCluster;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigTauClusterStatus;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigTauClusterDetails;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigTauClusterDetailsStatus;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigTauTracksInfo;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigTauTracksInfoStatus;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigTau;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigTauStatus;   //!
   TBranch        *b_trig_RoI_L2_tau_EmTau_ROI;   //!
   TBranch        *b_trig_RoI_L2_tau_EmTau_ROIStatus;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigInDetTrackCollection;   //!
   TBranch        *b_trig_RoI_L2_tau_TrigInDetTrackCollectionStatus;   //!
   TBranch        *b_trig_RoI_EF_tau_n;   //!
   TBranch        *b_trig_RoI_EF_tau_type;   //!
   TBranch        *b_trig_RoI_EF_tau_lastStep;   //!
   TBranch        *b_trig_RoI_EF_tau_EmTau_ROI;   //!
   TBranch        *b_trig_RoI_EF_tau_EmTau_ROIStatus;   //!
//    TBranch        *b_trig_RoI_EF_tau_Analysis__TauJetContainer;   //!
//    TBranch        *b_trig_RoI_EF_tau_Analysis__TauJetContainerStatus;   //!
   TBranch        *b_trig_L1_TAV;   //!
   TBranch        *b_trig_L2_passedPhysics;   //!
   TBranch        *b_trig_EF_passedPhysics;   //!
   TBranch        *b_trig_L1_TBP;   //!
   TBranch        *b_trig_L1_TAP;   //!
   TBranch        *b_trig_L2_passedRaw;   //!
   TBranch        *b_trig_EF_passedRaw;   //!
   TBranch        *b_trig_L2_resurrected;   //!
   TBranch        *b_trig_EF_resurrected;   //!
   TBranch        *b_trig_L2_prescaled;   //!
   TBranch        *b_trig_EF_prescaled;   //!

   physics(TTree *tree=0);
   virtual ~physics();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
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

Int_t physics::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t physics::LoadTree(Long64_t entry)
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

void physics::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
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
   el_breminvpt = 0;
   el_bremradius = 0;
   el_bremx = 0;
   el_bremclusterradius = 0;
   el_breminvpterr = 0;
   el_bremtrackauthor = 0;
   el_hasbrem = 0;
   el_bremdeltaqoverp = 0;
   el_bremmaterialtraversed = 0;
   el_refittedtrackqoverp = 0;
   el_refittedtrackd0 = 0;
   el_refittedtrackz0 = 0;
   el_refittedtracktheta = 0;
   el_refittedtrackphi = 0;
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
   el_vertx = 0;
   el_verty = 0;
   el_vertz = 0;
   el_hastrack = 0;
   el_deltaEmax2 = 0;
   el_trackd0beam = 0;
   el_isIso = 0;
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
   el_EF_dr = 0;
   el_EF_index = 0;
   el_L2_dr = 0;
   el_L2_index = 0;
   el_L1_dr = 0;
   el_L1_index = 0;
   el_EF_E = 0;
   el_EF_Et = 0;
   el_EF_pt = 0;
   el_EF_eta = 0;
   el_EF_phi = 0;
   el_EF_charge = 0;
   el_EF_Ethad = 0;
   el_EF_Ethad1 = 0;
   el_EF_f1 = 0;
   el_EF_Emins1 = 0;
   el_EF_fside = 0;
   el_EF_Emax2 = 0;
   el_EF_ws3 = 0;
   el_EF_wstot = 0;
   el_EF_E233 = 0;
   el_EF_E237 = 0;
   el_EF_E277 = 0;
   el_EF_weta2 = 0;
   el_EF_rphiallcalo = 0;
   el_EF_deltaeta1 = 0;
   el_EF_deltaeta2 = 0;
   el_EF_deltaphi2 = 0;
   el_EF_expectHitInBLayer = 0;
   el_EF_Etcone45 = 0;
   el_EF_Etcone20 = 0;
   el_EF_Etcone30 = 0;
   el_EF_Etcone40 = 0;
   el_EF_etacorrmag = 0;
   el_EF_zvertex = 0;
   el_EF_errz = 0;
   el_EF_trackd0 = 0;
   el_EF_trackz0 = 0;
   el_EF_trackphi = 0;
   el_EF_tracktheta = 0;
   el_EF_trackqoverp = 0;
   el_EF_trackpt = 0;
   el_EF_tracketa = 0;
   el_EF_nBLHits = 0;
   el_EF_nPixHits = 0;
   el_EF_nSCTHits = 0;
   el_EF_nTRTHits = 0;
   el_EF_nTRTHighTHits = 0;
   el_EF_nSiHits = 0;
   el_EF_TRTHighTHitsRatio = 0;
   el_EF_pixeldEdx = 0;
   el_EF_hastrack = 0;
   el_EF_matched = 0;
   el_L2_E = 0;
   el_L2_Et = 0;
   el_L2_pt = 0;
   el_L2_eta = 0;
   el_L2_phi = 0;
   el_L2_charge = 0;
   el_L2_Ethad1 = 0;
   el_L2_reta = 0;
   el_L2_Eratio = 0;
   el_L2_trackclusterdeta = 0;
   el_L2_trackclusterdphi = 0;
   el_L2_Etoverpt = 0;
   el_L2_trackpt = 0;
   el_L2_trackalgo = 0;
   el_L2_tracketa = 0;
   el_L2_trackd0 = 0;
   el_L2_trackz0 = 0;
   el_L2_tracktheta = 0;
   el_L2_trackphi = 0;
   el_L2_tracketaatcalo = 0;
   el_L2_trackphiatcalo = 0;
   el_L2_errpt = 0;
   el_L2_erreta = 0;
   el_L2_errphi = 0;
   el_L2_E237 = 0;
   el_L2_E277 = 0;
   el_L2_fside = 0;
   el_L2_weta2 = 0;
   el_L2_Emaxs1 = 0;
   el_L2_Emax2 = 0;
   el_L2_matched = 0;
   el_L1_eta = 0;
   el_L1_phi = 0;
   el_L1_EMisol = 0;
   el_L1_hadisol = 0;
   el_L1_hadcore = 0;
   el_L1_pt = 0;
   el_L1_Et = 0;
   el_L1_matched = 0;
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
   ph_isIso = 0;
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
   ph_trackIsol = 0;
   ph_convIP = 0;
   ph_convIPRev = 0;
   ph_HPV_zvertex = 0;
   ph_HPV_sigma_zvertex = 0;
   ph_HPV_zvertex_charged = 0;
   ph_HPV_sigma_zvertex_charged = 0;
   ph_HPV_zvertex_neutral = 0;
   ph_HPV_sigma_zvertex_neutral = 0;
   ph_HPV_eta = 0;
   ph_HPV_sigma_eta = 0;
   ph_HPV_convTrk1_zvertex = 0;
   ph_HPV_convTrk1_sigma_zvertex = 0;
   ph_HPV_convTrk2_zvertex = 0;
   ph_HPV_convTrk2_sigma_zvertex = 0;
   ph_ptIsolationCone = 0;
   ph_ptIsolationConePhAngle = 0;
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
   ph_EF_E = 0;
   ph_EF_Et = 0;
   ph_EF_pt = 0;
   ph_EF_eta = 0;
   ph_EF_phi = 0;
   ph_EF_Ethad = 0;
   ph_EF_Ethad1 = 0;
   ph_EF_f1 = 0;
   ph_EF_Emins1 = 0;
   ph_EF_fside = 0;
   ph_EF_Emax2 = 0;
   ph_EF_ws3 = 0;
   ph_EF_wstot = 0;
   ph_EF_E233 = 0;
   ph_EF_E237 = 0;
   ph_EF_E277 = 0;
   ph_EF_weta2 = 0;
   ph_EF_rphiallcalo = 0;
   ph_EF_Etcone45 = 0;
   ph_EF_Etcone20 = 0;
   ph_EF_Etcone30 = 0;
   ph_EF_Etcone40 = 0;
   ph_EF_zvertex = 0;
   ph_EF_errz = 0;
   ph_EF_matched = 0;
   ph_L2_dr = 0;
   ph_L2_E = 0;
   ph_L2_Et = 0;
   ph_L2_pt = 0;
   ph_L2_eta = 0;
   ph_L2_phi = 0;
   ph_L2_Ethad1 = 0;
   ph_L2_reta = 0;
   ph_L2_Eratio = 0;
   ph_L2_E237 = 0;
   ph_L2_E277 = 0;
   ph_L2_fside = 0;
   ph_L2_weta2 = 0;
   ph_L2_Emaxs1 = 0;
   ph_L2_Emax2 = 0;
   ph_L2_matched = 0;
   ph_L1_dr = 0;
   ph_L1_eta = 0;
   ph_L1_phi = 0;
   ph_L1_EMisol = 0;
   ph_L1_hadisol = 0;
   ph_L1_hadcore = 0;
   ph_L1_pt = 0;
   ph_L1_Et = 0;
   ph_L1_matched = 0;
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
   mu_bestMatch = 0;
   mu_isStandAloneMuon = 0;
   mu_isCombinedMuon = 0;
   mu_isLowPtReconstructedMuon = 0;
   mu_d0_exPV = 0;
   mu_z0_exPV = 0;
   mu_phi_exPV = 0;
   mu_theta_exPV = 0;
   mu_qoverp_exPV = 0;
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
   mu_msextrap_d0 = 0;
   mu_msextrap_z0 = 0;
   mu_msextrap_phi = 0;
   mu_msextrap_theta = 0;
   mu_msextrap_qoverp = 0;
   mu_idextrap_d0 = 0;
   mu_idextrap_z0 = 0;
   mu_idextrap_phi = 0;
   mu_idextrap_theta = 0;
   mu_idextrap_qoverp = 0;
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
   mu_seg_x = 0;
   mu_seg_y = 0;
   mu_seg_z = 0;
   mu_seg_phi = 0;
   mu_seg_theta = 0;
   mu_seg_locX = 0;
   mu_seg_locY = 0;
   mu_seg_locAngleXZ = 0;
   mu_seg_locAngleYZ = 0;
   mu_seg_sector = 0;
   mu_seg_stationEta = 0;
   mu_seg_isEndcap = 0;
   mu_seg_stationName = 0;
   mu_seg_author = 0;
   mu_seg_chi2 = 0;
   mu_seg_ndof = 0;
   jet_E = 0;
   jet_pt = 0;
   jet_m = 0;
   jet_eta = 0;
   jet_phi = 0;
   jet_WIDTH = 0;
   jet_n90 = 0;
   jet_n90constituents = 0;
   jet_fracSamplingMax = 0;
   jet_SamplingMax = 0;
   jet_BCH_CORR_CELL = 0;
   jet_BCH_CORR_JET = 0;
   jet_BCH_CORR_JET_FORCELL = 0;
   jet_ENG_BAD_CELLS = 0;
   jet_N_BAD_CELLS = 0;
   jet_N_BAD_CELLS_CORR = 0;
   jet_BAD_CELLS_CORR_E = 0;
   jet_timing = 0;
   jet_quality = 0;
   jet_emfrac = 0;
   jet_GCWJES = 0;
   jet_EMJES = 0;
   jet_emscale_E = 0;
   jet_emscale_pt = 0;
   jet_emscale_m = 0;
   jet_emscale_eta = 0;
   jet_emscale_phi = 0;
   jet_jvtxf = 0;
   jet_jvtx_x = 0;
   jet_jvtx_y = 0;
   jet_jvtx_z = 0;
   jet_flavor_weight = 0;
   jet_el_dr = 0;
   jet_el_matched = 0;
   jet_mu_dr = 0;
   jet_mu_matched = 0;
   cl_E = 0;
   cl_pt = 0;
   cl_m = 0;
   cl_eta = 0;
   cl_phi = 0;
   cl_E_em = 0;
   cl_E_had = 0;
   cl_firstEdens = 0;
   cl_cellmaxfrac = 0;
   cl_longitudinal = 0;
   cl_secondlambda = 0;
   cl_lateral = 0;
   cl_secondR = 0;
   cl_centerlambda = 0;
   cl_deltaTheta = 0;
   cl_deltaPhi = 0;
   cl_time = 0;
   emcl_E = 0;
   emcl_pt = 0;
   emcl_m = 0;
   emcl_eta = 0;
   emcl_phi = 0;
   emcl_E_em = 0;
   emcl_E_had = 0;
   emcl_firstEdens = 0;
   emcl_cellmaxfrac = 0;
   emcl_longitudinal = 0;
   emcl_secondlambda = 0;
   emcl_lateral = 0;
   emcl_secondR = 0;
   emcl_centerlambda = 0;
   emcl_deltaTheta = 0;
   emcl_deltaPhi = 0;
   emcl_time = 0;
   emcl_E_PreSamplerB = 0;
   emcl_E_EMB1 = 0;
   emcl_E_EMB2 = 0;
   emcl_E_EMB3 = 0;
   emcl_E_PreSamplerE = 0;
   emcl_E_EME1 = 0;
   emcl_E_EME2 = 0;
   emcl_E_EME3 = 0;
   emcl_E_HEC0 = 0;
   emcl_E_HEC1 = 0;
   emcl_E_HEC2 = 0;
   emcl_E_HEC3 = 0;
   emcl_E_TileBar0 = 0;
   emcl_E_TileBar1 = 0;
   emcl_E_TileBar2 = 0;
   emcl_E_TileGap1 = 0;
   emcl_E_TileGap2 = 0;
   emcl_E_TileGap3 = 0;
   emcl_E_TileExt0 = 0;
   emcl_E_TileExt1 = 0;
   emcl_E_TileExt2 = 0;
   emcl_E_FCAL0 = 0;
   emcl_E_FCAL1 = 0;
   emcl_E_FCAL2 = 0;
   trk_d0 = 0;
   trk_z0 = 0;
   trk_phi = 0;
   trk_theta = 0;
   trk_qoverp = 0;
   trk_pt = 0;
   trk_eta = 0;
   trk_err_d0 = 0;
   trk_err_z0 = 0;
   trk_err_phi = 0;
   trk_err_theta = 0;
   trk_err_qoverp = 0;
   trk_cov_d0 = 0;
   trk_cov_z0 = 0;
   trk_cov_phi = 0;
   trk_cov_theta = 0;
   trk_cov_qoverp = 0;
   trk_cov_d0_z0 = 0;
   trk_cov_d0_phi = 0;
   trk_cov_d0_theta = 0;
   trk_cov_d0_qoverp = 0;
   trk_cov_z0_phi = 0;
   trk_cov_z0_theta = 0;
   trk_cov_z0_qoverp = 0;
   trk_cov_phi_theta = 0;
   trk_cov_phi_qoverp = 0;
   trk_cov_theta_qoverp = 0;
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
   trk_d0_wrtPV = 0;
   trk_z0_wrtPV = 0;
   trk_phi_wrtPV = 0;
   trk_theta_wrtPV = 0;
   trk_qoverp_wrtPV = 0;
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
   tau_Et = 0;
   tau_pt = 0;
   tau_m = 0;
   tau_eta = 0;
   tau_phi = 0;
   tau_charge = 0;
   tau_BDTEleScore = 0;
   tau_BDTJetScore = 0;
   tau_discCut = 0;
   tau_discCutTMVA = 0;
   tau_discLL = 0;
   tau_discNN = 0;
   tau_efficLL = 0;
   tau_efficNN = 0;
   tau_likelihood = 0;
   tau_tauJetNeuralNetwork = 0;
   tau_tauENeuralNetwork = 0;
   tau_tauElTauLikelihood = 0;
   tau_electronVetoLoose = 0;
   tau_electronVetoMedium = 0;
   tau_electronVetoTight = 0;
   tau_muonVeto = 0;
   tau_tauCutLoose = 0;
   tau_tauCutMedium = 0;
   tau_tauCutTight = 0;
   tau_tauCutSafeLoose = 0;
   tau_tauCutSafeMedium = 0;
   tau_tauCutSafeTight = 0;
   tau_tauCutSafeCaloLoose = 0;
   tau_tauCutSafeCaloMedium = 0;
   tau_tauCutSafeCaloTight = 0;
   tau_tauLlhLoose = 0;
   tau_tauLlhMedium = 0;
   tau_tauLlhTight = 0;
   tau_author = 0;
   tau_ROIword = 0;
   tau_nProng = 0;
   tau_numTrack = 0;
   tau_etOverPtLeadTrk = 0;
   tau_ipZ0SinThetaSigLeadTrk = 0;
   tau_leadTrkPt = 0;
   tau_nLooseTrk = 0;
   tau_nLooseConvTrk = 0;
   tau_nProngLoose = 0;
   tau_ipSigLeadTrk = 0;
   tau_ipSigLeadLooseTrk = 0;
   tau_etOverPtLeadLooseTrk = 0;
   tau_leadLooseTrkPt = 0;
   tau_chrgLooseTrk = 0;
   tau_massTrkSys = 0;
   tau_trkWidth2 = 0;
   tau_trFlightPathSig = 0;
   tau_etEflow = 0;
   tau_mEflow = 0;
   tau_nPi0 = 0;
   tau_ele_E237E277 = 0;
   tau_ele_PresamplerFraction = 0;
   tau_ele_ECALFirstFraction = 0;
   tau_seedCalo_EMRadius = 0;
   tau_seedCalo_hadRadius = 0;
   tau_seedCalo_etEMAtEMScale = 0;
   tau_seedCalo_etHadAtEMScale = 0;
   tau_seedCalo_isolFrac = 0;
   tau_seedCalo_centFrac = 0;
   tau_seedCalo_stripWidth2 = 0;
   tau_seedCalo_nStrip = 0;
   tau_seedCalo_etEMCalib = 0;
   tau_seedCalo_etHadCalib = 0;
   tau_seedCalo_eta = 0;
   tau_seedCalo_phi = 0;
   tau_seedCalo_nIsolLooseTrk = 0;
   tau_seedCalo_trkAvgDist = 0;
   tau_seedCalo_trkRmsDist = 0;
   tau_seedTrk_EMRadius = 0;
   tau_seedTrk_isolFrac = 0;
   tau_seedTrk_etChrgHadOverSumTrkPt = 0;
   tau_seedTrk_isolFracWide = 0;
   tau_seedTrk_etHadAtEMScale = 0;
   tau_seedTrk_etEMAtEMScale = 0;
   tau_seedTrk_etEMCL = 0;
   tau_seedTrk_etChrgEM = 0;
   tau_seedTrk_etNeuEM = 0;
   tau_seedTrk_etResNeuEM = 0;
   tau_seedTrk_hadLeakEt = 0;
   tau_seedTrk_sumEMCellEtOverLeadTrkPt = 0;
   tau_seedTrk_secMaxStripEt = 0;
   tau_seedTrk_stripWidth2 = 0;
   tau_seedTrk_nStrip = 0;
   tau_seedTrk_etChrgHad = 0;
   tau_seedTrk_nOtherCoreTrk = 0;
   tau_seedTrk_nIsolTrk = 0;
   tau_seedTrk_etIsolEM = 0;
   tau_seedTrk_etIsolHad = 0;
   tau_calcVars_sumTrkPt = 0;
   tau_calcVars_numTopoClusters = 0;
   tau_calcVars_numEffTopoClusters = 0;
   tau_calcVars_topoInvMass = 0;
   tau_calcVars_effTopoInvMass = 0;
   tau_calcVars_topoMeanDeltaR = 0;
   tau_calcVars_effTopoMeanDeltaR = 0;
   tau_calcVars_etHadSumPtTracks = 0;
   tau_calcVars_etEMSumPtTracks = 0;
   tau_track_n = 0;
   tau_EF_dr = 0;
   tau_EF_matched = 0;
   tau_L2_dr = 0;
   tau_L2_E = 0;
   tau_L2_Et = 0;
   tau_L2_pt = 0;
   tau_L2_eta = 0;
   tau_L2_phi = 0;
   tau_L2_matched = 0;
   tau_L1_dr = 0;
   tau_L1_Et = 0;
   tau_L1_pt = 0;
   tau_L1_eta = 0;
   tau_L1_phi = 0;
   tau_L1_matched = 0;
   mb_E = 0;
   mb_eta = 0;
   mb_phi = 0;
   mb_time = 0;
   mb_quality = 0;
   mb_type = 0;
   mb_module = 0;
   mb_channel = 0;
   vxp_x = 0;
   vxp_y = 0;
   vxp_z = 0;
   vxp_chi2 = 0;
   vxp_ndof = 0;
   vxp_err_x = 0;
   vxp_err_y = 0;
   vxp_err_z = 0;
   vxp_px = 0;
   vxp_py = 0;
   vxp_pz = 0;
   vxp_E = 0;
   vxp_m = 0;
   vxp_nTracks = 0;
   vxp_sumPt = 0;
   vxp_type = 0;
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
   muonTruth_pt = 0;
   muonTruth_m = 0;
   muonTruth_eta = 0;
   muonTruth_phi = 0;
   muonTruth_charge = 0;
   muonTruth_PDGID = 0;
   muonTruth_barcode = 0;
   muonTruth_type = 0;
   muonTruth_origin = 0;
   mcevt_signal_process_id = 0;
   mcevt_event_number = 0;
   mcevt_event_scale = 0;
   mcevt_alphaQCD = 0;
   mcevt_alphaQED = 0;
   mcevt_pdf_id1 = 0;
   mcevt_pdf_id2 = 0;
   mcevt_pdf_x1 = 0;
   mcevt_pdf_x2 = 0;
   mcevt_pdf_scale = 0;
   mcevt_pdf1 = 0;
   mcevt_pdf2 = 0;
   mcevt_weight = 0;
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
   trig_L1_emtau_L1_EM10 = 0;
   trig_L1_emtau_L1_EM10I = 0;
   trig_L1_emtau_L1_EM14 = 0;
   trig_L1_emtau_L1_EM2 = 0;
   trig_L1_emtau_L1_EM2_UNPAIRED = 0;
   trig_L1_emtau_L1_EM3 = 0;
   trig_L1_emtau_L1_EM3_EMPTY = 0;
   trig_L1_emtau_L1_EM3_FIRSTEMPTY = 0;
   trig_L1_emtau_L1_EM3_MV = 0;
   trig_L1_emtau_L1_EM3_UNPAIRED = 0;
   trig_L1_emtau_L1_EM4 = 0;
   trig_L1_emtau_L1_EM5 = 0;
   trig_L1_emtau_L1_EM5I = 0;
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
   trig_L2_trk_idscan_eGamma_algorithmId = 0;
   trig_L2_trk_idscan_eGamma_trackStatus = 0;
   trig_L2_trk_idscan_eGamma_chi2Ndof = 0;
   trig_L2_trk_idscan_eGamma_nStrawHits = 0;
   trig_L2_trk_idscan_eGamma_nHighThrHits = 0;
   trig_L2_trk_idscan_eGamma_nPixelSpacePoints = 0;
   trig_L2_trk_idscan_eGamma_nSCT_SpacePoints = 0;
   trig_L2_trk_idscan_eGamma_a0 = 0;
   trig_L2_trk_idscan_eGamma_z0 = 0;
   trig_L2_trk_idscan_eGamma_phi0 = 0;
   trig_L2_trk_idscan_eGamma_eta = 0;
   trig_L2_trk_idscan_eGamma_pt = 0;
   trig_L2_trk_sitrack_eGamma_algorithmId = 0;
   trig_L2_trk_sitrack_eGamma_trackStatus = 0;
   trig_L2_trk_sitrack_eGamma_chi2Ndof = 0;
   trig_L2_trk_sitrack_eGamma_nStrawHits = 0;
   trig_L2_trk_sitrack_eGamma_nHighThrHits = 0;
   trig_L2_trk_sitrack_eGamma_nPixelSpacePoints = 0;
   trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints = 0;
   trig_L2_trk_sitrack_eGamma_a0 = 0;
   trig_L2_trk_sitrack_eGamma_z0 = 0;
   trig_L2_trk_sitrack_eGamma_phi0 = 0;
   trig_L2_trk_sitrack_eGamma_eta = 0;
   trig_L2_trk_sitrack_eGamma_pt = 0;
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
   trig_L2_el_L2_2e3_NoCut = 0;
   trig_L2_el_L2_2e3_loose = 0;
   trig_L2_el_L2_2e3_medium = 0;
   trig_L2_el_L2_2e5_loose = 0;
   trig_L2_el_L2_2e5_medium = 0;
   trig_L2_el_L2_2e6_nocut = 0;
   trig_L2_el_L2_e10_loose = 0;
   trig_L2_el_L2_e10_loose_IdScan = 0;
   trig_L2_el_L2_e10_loose_TRT = 0;
   trig_L2_el_L2_e10_loose_passEF = 0;
   trig_L2_el_L2_e10_loose_passL2 = 0;
   trig_L2_el_L2_e10_medium = 0;
   trig_L2_el_L2_e10_medium_IdScan = 0;
   trig_L2_el_L2_e10_medium_TRT = 0;
   trig_L2_el_L2_e10i_loose = 0;
   trig_L2_el_L2_e15_loose = 0;
   trig_L2_el_L2_e15i_loose = 0;
   trig_L2_el_L2_e20_loose_NoIDTrkCut = 0;
   trig_L2_el_L2_e3_NoCut = 0;
   trig_L2_el_L2_e3_NoCut_IdScan = 0;
   trig_L2_el_L2_e3_NoCut_TRT = 0;
   trig_L2_el_L2_e3_loose = 0;
   trig_L2_el_L2_e3_loose_IdScan = 0;
   trig_L2_el_L2_e3_loose_TRT = 0;
   trig_L2_el_L2_e3_medium = 0;
   trig_L2_el_L2_e3_medium_IdScan = 0;
   trig_L2_el_L2_e3_medium_TRT = 0;
   trig_L2_el_L2_e5_EFfullcalo = 0;
   trig_L2_el_L2_e5_NoCut = 0;
   trig_L2_el_L2_e5_NoCut_FwdBackTrk = 0;
   trig_L2_el_L2_e5_NoCut_IdScan = 0;
   trig_L2_el_L2_e5_NoCut_L2SW = 0;
   trig_L2_el_L2_e5_NoCut_Ringer = 0;
   trig_L2_el_L2_e5_NoCut_SiTrk = 0;
   trig_L2_el_L2_e5_NoCut_TRT = 0;
   trig_L2_el_L2_e5_NoCut_cells = 0;
   trig_L2_el_L2_e5_NoCut_cosmic = 0;
   trig_L2_el_L2_e5_NoCut_firstempty = 0;
   trig_L2_el_L2_e5_NoCut_unpaired = 0;
   trig_L2_el_L2_e5_loose = 0;
   trig_L2_el_L2_e5_loose_IdScan = 0;
   trig_L2_el_L2_e5_loose_NoIDTrkCut = 0;
   trig_L2_el_L2_e5_loose_TRT = 0;
   trig_L2_el_L2_e5_medium = 0;
   trig_L2_el_L2_e5_medium_IdScan = 0;
   trig_L2_el_L2_e5_medium_MV = 0;
   trig_L2_el_L2_e5_medium_TRT = 0;
   trig_L2_el_L2_e6_loose = 0;
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
   trig_L2_ph_L2_2g10_nocut = 0;
   trig_L2_ph_L2_2g3_loose = 0;
   trig_L2_ph_L2_2g3_nocut = 0;
   trig_L2_ph_L2_2g5_loose = 0;
   trig_L2_ph_L2_g10_loose = 0;
   trig_L2_ph_L2_g10i_loose = 0;
   trig_L2_ph_L2_g15_loose = 0;
   trig_L2_ph_L2_g15i_loose = 0;
   trig_L2_ph_L2_g3_loose = 0;
   trig_L2_ph_L2_g3_nocut = 0;
   trig_L2_ph_L2_g3_nocut_unpaired = 0;
   trig_L2_ph_L2_g5_EFfullcalo = 0;
   trig_L2_ph_L2_g5_loose = 0;
   trig_L2_ph_L2_g5_nocut = 0;
   trig_L2_ph_L2_g5_nocut_cosmic = 0;
   trig_L2_ph_L2_g5_nocut_firstempty = 0;
   trig_L2_ph_L2_g5_nocut_unpaired = 0;
   trig_L2_ph_L2_g6_loose = 0;
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
   trig_EF_el_EF_2e3_NoCut = 0;
   trig_EF_el_EF_2e3_loose = 0;
   trig_EF_el_EF_2e3_medium = 0;
   trig_EF_el_EF_2e5_loose = 0;
   trig_EF_el_EF_2e5_medium = 0;
   trig_EF_el_EF_2e6_nocut = 0;
   trig_EF_el_EF_e10_loose = 0;
   trig_EF_el_EF_e10_loose_IdScan = 0;
   trig_EF_el_EF_e10_loose_TRT = 0;
   trig_EF_el_EF_e10_loose_passEF = 0;
   trig_EF_el_EF_e10_loose_passL2 = 0;
   trig_EF_el_EF_e10_medium = 0;
   trig_EF_el_EF_e10_medium_IdScan = 0;
   trig_EF_el_EF_e10_medium_TRT = 0;
   trig_EF_el_EF_e10i_loose = 0;
   trig_EF_el_EF_e15_loose = 0;
   trig_EF_el_EF_e15i_loose = 0;
   trig_EF_el_EF_e20_loose_NoIDTrkCut = 0;
   trig_EF_el_EF_e3_NoCut = 0;
   trig_EF_el_EF_e3_NoCut_IdScan = 0;
   trig_EF_el_EF_e3_NoCut_TRT = 0;
   trig_EF_el_EF_e3_loose = 0;
   trig_EF_el_EF_e3_loose_IdScan = 0;
   trig_EF_el_EF_e3_loose_TRT = 0;
   trig_EF_el_EF_e3_medium = 0;
   trig_EF_el_EF_e3_medium_IdScan = 0;
   trig_EF_el_EF_e3_medium_TRT = 0;
   trig_EF_el_EF_e5_EFfullcalo = 0;
   trig_EF_el_EF_e5_NoCut = 0;
   trig_EF_el_EF_e5_NoCut_FwdBackTrk = 0;
   trig_EF_el_EF_e5_NoCut_IdScan = 0;
   trig_EF_el_EF_e5_NoCut_L2SW = 0;
   trig_EF_el_EF_e5_NoCut_Ringer = 0;
   trig_EF_el_EF_e5_NoCut_SiTrk = 0;
   trig_EF_el_EF_e5_NoCut_TRT = 0;
   trig_EF_el_EF_e5_NoCut_cells = 0;
   trig_EF_el_EF_e5_NoCut_cosmic = 0;
   trig_EF_el_EF_e5_NoCut_firstempty = 0;
   trig_EF_el_EF_e5_NoCut_unpaired = 0;
   trig_EF_el_EF_e5_loose = 0;
   trig_EF_el_EF_e5_loose_IdScan = 0;
   trig_EF_el_EF_e5_loose_NoIDTrkCut = 0;
   trig_EF_el_EF_e5_loose_TRT = 0;
   trig_EF_el_EF_e5_medium = 0;
   trig_EF_el_EF_e5_medium_IdScan = 0;
   trig_EF_el_EF_e5_medium_MV = 0;
   trig_EF_el_EF_e5_medium_TRT = 0;
   trig_EF_el_EF_e6_loose = 0;
   trig_EF_ph_E = 0;
   trig_EF_ph_Et = 0;
   trig_EF_ph_pt = 0;
   trig_EF_ph_m = 0;
   trig_EF_ph_eta = 0;
   trig_EF_ph_phi = 0;
   trig_EF_ph_px = 0;
   trig_EF_ph_py = 0;
   trig_EF_ph_pz = 0;
   trig_EF_ph_EF_2g10_nocut = 0;
   trig_EF_ph_EF_2g3_loose = 0;
   trig_EF_ph_EF_2g3_nocut = 0;
   trig_EF_ph_EF_2g5_loose = 0;
   trig_EF_ph_EF_g10_loose = 0;
   trig_EF_ph_EF_g10_loose_larcalib = 0;
   trig_EF_ph_EF_g10i_loose = 0;
   trig_EF_ph_EF_g15_loose = 0;
   trig_EF_ph_EF_g15i_loose = 0;
   trig_EF_ph_EF_g3_loose = 0;
   trig_EF_ph_EF_g3_nocut = 0;
   trig_EF_ph_EF_g3_nocut_larcalib = 0;
   trig_EF_ph_EF_g3_nocut_unpaired = 0;
   trig_EF_ph_EF_g5_EFfullcalo = 0;
   trig_EF_ph_EF_g5_loose = 0;
   trig_EF_ph_EF_g5_loose_larcalib = 0;
   trig_EF_ph_EF_g5_nocut = 0;
   trig_EF_ph_EF_g5_nocut_cosmic = 0;
   trig_EF_ph_EF_g5_nocut_firstempty = 0;
   trig_EF_ph_EF_g5_nocut_unpaired = 0;
   trig_EF_ph_EF_g6_loose = 0;
   trig_Nav_chain_ChainId = 0;
   trig_Nav_chain_RoIType = 0;
   trig_Nav_chain_RoIIndex = 0;
   trig_RoI_L2_e_type = 0;
   trig_RoI_L2_e_lastStep = 0;
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
   trig_RoI_EF_e_type = 0;
   trig_RoI_EF_e_lastStep = 0;
   trig_RoI_EF_e_EmTau_ROI = 0;
   trig_RoI_EF_e_EmTau_ROIStatus = 0;
   trig_L1_mu_pt = 0;
   trig_L1_mu_eta = 0;
   trig_L1_mu_phi = 0;
   trig_L1_mu_thrName = 0;
   trig_L1_mu_thrNumber = 0;
   trig_L1_mu_RoINumber = 0;
   trig_L1_mu_sectorAddress = 0;
   trig_L1_mu_firstCandidate = 0;
   trig_L1_mu_moreCandInRoI = 0;
   trig_L1_mu_moreCandInSector = 0;
   trig_L1_mu_source = 0;
   trig_L1_mu_hemisphere = 0;
   trig_L1_mu_RoIWord = 0;
   trig_l2muonfeature_pt = 0;
   trig_l2muonfeature_eta = 0;
   trig_l2muonfeature_phi = 0;
   trig_l2muonfeature_dir_phi = 0;
   trig_l2muonfeature_zeta = 0;
   trig_l2muonfeature_dir_zeta = 0;
   trig_l2muonfeature_roiId = 0;
   trig_l2muonfeature_saddress = 0;
   trig_l2muonfeature_radius = 0;
   trig_l2muonfeature_beta = 0;
   trig_l2muonfeature_sp1_r = 0;
   trig_l2muonfeature_sp1_z = 0;
   trig_l2muonfeature_sp1_slope = 0;
   trig_l2muonfeature_sp2_r = 0;
   trig_l2muonfeature_sp2_z = 0;
   trig_l2muonfeature_sp2_slope = 0;
   trig_l2muonfeature_sp3_r = 0;
   trig_l2muonfeature_sp3_z = 0;
   trig_l2muonfeature_sp3_slope = 0;
   trig_l2muonfeature_br_radius = 0;
   trig_l2muonfeature_br_sagitta = 0;
   trig_l2muonfeature_ec_alpha = 0;
   trig_l2muonfeature_ec_beta = 0;
   trig_l2muonfeature_dq_var1 = 0;
   trig_l2muonfeature_dq_var2 = 0;
   trig_l2combmuonfeature_sigma_pt = 0;
   trig_l2combmuonfeature_pt = 0;
   trig_l2combmuonfeature_eta = 0;
   trig_l2combmuonfeature_phi = 0;
   trig_l2combmuonfeature_mf_pt = 0;
   trig_l2combmuonfeature_mf_eta = 0;
   trig_l2combmuonfeature_mf_phi = 0;
   trig_l2combmuonfeature_mf_dir_phi = 0;
   trig_l2combmuonfeature_mf_zeta = 0;
   trig_l2combmuonfeature_mf_dir_zeta = 0;
   trig_l2combmuonfeature_mf_hasmf = 0;
   trig_l2isomuonfeature_EtInnerConeEC  = 0;
   trig_l2isomuonfeature_EtOuterConeEC = 0;
   trig_l2isomuonfeature_EtInnerConeHC = 0;
   trig_l2isomuonfeature_EtOuterConeHC = 0;
   trig_l2isomuonfeature_NTracksCone = 0;
   trig_l2isomuonfeature_SumPtTracksCone = 0;
   trig_l2isomuonfeature_PtMuTracksCone = 0;
   trig_l2isomuonfeature_iso_strategy = 0;
   trig_l2isomuonfeature_RoiIdMu  = 0;
   trig_l2isomuonfeature_PtMu = 0;
   trig_l2isomuonfeature_EtaMu = 0;
   trig_l2isomuonfeature_PhiMu = 0;
   trig_l2isomuonfeature_LArWeight = 0;
   trig_l2isomuonfeature_TileWeight = 0;
   trig_l2isomuonfeature_pt = 0;
   trig_l2isomuonfeature_eta = 0;
   trig_l2isomuonfeature_phi = 0;
   trig_l2tilemufeature_eta = 0;
   trig_l2tilemufeature_phi = 0;
   trig_l2tilemufeature_enedep = 0;
   trig_l2tilemufeature_quality = 0;
   trig_l2tiletrackmufeature_PtTR_Trk = 0;
   trig_l2tiletrackmufeature_EtaTR_Trk = 0;
   trig_l2tiletrackmufeature_PhiTR_Trk = 0;
   trig_l2tiletrackmufeature_Typ_IDTrk = 0;
   trig_l2tiletrackmufeature_pt = 0;
   trig_l2tiletrackmufeature_eta = 0;
   trig_l2tiletrackmufeature_phi = 0;
   trig_l2tiletrackmufeature_tilemu_eta = 0;
   trig_l2tiletrackmufeature_tilemu_phi = 0;
   trig_l2tiletrackmufeature_tilemu_enedep = 0;
   trig_l2tiletrackmufeature_tilemu_quality = 0;
   trig_l2tiletrackmufeature_tilemu_hastilemu = 0;
   trig_EF_tau_Et = 0;
   trig_EF_tau_pt = 0;
   trig_EF_tau_m = 0;
   trig_EF_tau_eta = 0;
   trig_EF_tau_phi = 0;
   trig_EF_tau_px = 0;
   trig_EF_tau_py = 0;
   trig_EF_tau_pz = 0;
   trig_EF_tau_RoIword = 0;
   trig_EF_tau_nLooseTrk = 0;
   trig_EF_tau_leadLooseTrkPt = 0;
   trig_EF_tau_leadLooseTrkEta = 0;
   trig_EF_tau_leadLooseTrkPhi = 0;
   trig_EF_tau_seedCalo_EMRadius = 0;
   trig_EF_tau_seedCalo_hadRadius = 0;
   trig_EF_tau_seedCalo_etEMAtEMScale = 0;
   trig_EF_tau_seedCalo_etHadAtEMScale = 0;
   trig_EF_tau_seedCalo_isolFrac = 0;
   trig_EF_tau_seedCalo_centFrac = 0;
   trig_EF_tau_seedCalo_etEMCalib = 0;
   trig_EF_tau_seedCalo_etHadCalib = 0;
   trig_EF_tau_seedCalo_eta = 0;
   trig_EF_tau_seedCalo_phi = 0;
   trig_EF_tau_invMassOfTrks = 0;
   trig_EF_tau_looseTrkWidth = 0;
   trig_EF_tau_looseTrkRadius = 0;
   trig_EF_tau_EF_tau12_loose = 0;
   trig_EF_tau_EF_tau12_loose_PT = 0;
   trig_EF_tau_EF_tau12_loose_e10_loose = 0;
   trig_EF_tau_EF_tau12_loose_mu10 = 0;
   trig_EF_tau_EF_tau12_loose_xe20 = 0;
   trig_EF_tau_EF_tau16_loose = 0;
   trig_EF_tau_EF_tau16_loose_3j40 = 0;
   trig_EF_tau_EF_tau16_loose_TAU8 = 0;
   trig_EF_tau_EF_tau16_loose_xe20 = 0;
   trig_EF_tau_EF_tau16i_NoIDTrkCut = 0;
   trig_EF_tau_EF_tau16i_loose = 0;
   trig_EF_tau_EF_tau20_loose = 0;
   trig_EF_tau_EF_tau20_medium = 0;
   trig_EF_tau_EF_tau29_loose = 0;
   trig_EF_tau_EF_tau29_medium = 0;
   trig_EF_tau_EF_tau29i_loose = 0;
   trig_EF_tau_EF_tau50_loose = 0;
   trig_EF_tau_EF_tau50_loose_TAU30 = 0;
   trig_EF_tau_EF_tauNoCut = 0;
   trig_EF_tau_EF_tauNoCut_NoIDTrkCut = 0;
   trig_EF_tau_EF_tauNoCut_SiTrk = 0;
   trig_EF_tau_EF_tauNoCut_cells = 0;
   trig_EF_tau_EF_tauNoCut_cosmic = 0;
   trig_EF_tau_EF_tauNoCut_firstempty = 0;
   trig_EF_tau_EF_tauNoCut_hasTrk = 0;
   trig_EF_tau_EF_tauNoCut_hasTrk_MV = 0;
   trig_EF_tau_EF_tauNoCut_hasTrk_SiTrk = 0;
   trig_EF_tau_EF_tauNoCut_hasTrk_xe20 = 0;
   trig_EF_tau_EF_tauNoCut_unpaired = 0;
   trig_L2_tau_pt = 0;
   trig_L2_tau_eta = 0;
   trig_L2_tau_phi = 0;
   trig_L2_tau_px = 0;
   trig_L2_tau_py = 0;
   trig_L2_tau_pz = 0;
   trig_L2_tau_RoIword = 0;
   trig_L2_tau_simpleEtFlow = 0;
   trig_L2_tau_nMatchedTracks = 0;
   trig_L2_tau_L2_tau12_loose = 0;
   trig_L2_tau_L2_tau12_loose_PT = 0;
   trig_L2_tau_L2_tau12_loose_e10_loose = 0;
   trig_L2_tau_L2_tau12_loose_mu10 = 0;
   trig_L2_tau_L2_tau12_loose_xe20 = 0;
   trig_L2_tau_L2_tau16_loose = 0;
   trig_L2_tau_L2_tau16_loose_3j30 = 0;
   trig_L2_tau_L2_tau16_loose_TAU8 = 0;
   trig_L2_tau_L2_tau16_loose_xe20 = 0;
   trig_L2_tau_L2_tau16i_NoIDTrkCut = 0;
   trig_L2_tau_L2_tau16i_loose = 0;
   trig_L2_tau_L2_tau20_loose = 0;
   trig_L2_tau_L2_tau20_medium = 0;
   trig_L2_tau_L2_tau29_loose = 0;
   trig_L2_tau_L2_tau29_medium = 0;
   trig_L2_tau_L2_tau29i_loose = 0;
   trig_L2_tau_L2_tau50_loose = 0;
   trig_L2_tau_L2_tau50_loose_TAU30 = 0;
   trig_L2_tau_L2_tauNoCut = 0;
   trig_L2_tau_L2_tauNoCut_NoIDTrkCut = 0;
   trig_L2_tau_L2_tauNoCut_SiTrk = 0;
   trig_L2_tau_L2_tauNoCut_cells = 0;
   trig_L2_tau_L2_tauNoCut_cosmic = 0;
   trig_L2_tau_L2_tauNoCut_firstempty = 0;
   trig_L2_tau_L2_tauNoCut_hasTrk = 0;
   trig_L2_tau_L2_tauNoCut_hasTrk_MV = 0;
   trig_L2_tau_L2_tauNoCut_hasTrk_SiTrk = 0;
   trig_L2_tau_L2_tauNoCut_hasTrk_xe20 = 0;
   trig_L2_tau_L2_tauNoCut_unpaired = 0;
   trig_L2_tau_cluster_quality = 0;
   trig_L2_tau_cluster_EMenergy = 0;
   trig_L2_tau_cluster_HADenergy = 0;
   trig_L2_tau_cluster_eta = 0;
   trig_L2_tau_cluster_phi = 0;
   trig_L2_tau_cluster_EMRadius2 = 0;
   trig_L2_tau_cluster_CaloRadius = 0;
   trig_L2_tau_cluster_IsoFrac = 0;
   trig_L2_tau_cluster_numTotCells = 0;
   trig_L2_tau_cluster_stripWidth = 0;
   trig_L2_tau_cluster_stripWidthOffline = 0;
   trig_L2_tau_cluster_EMenergyNor = 0;
   trig_L2_tau_cluster_EMenergyNar = 0;
   trig_L2_tau_cluster_HADenergyNor = 0;
   trig_L2_tau_cluster_HADenergyNar = 0;
   trig_L2_tau_cluster_etNar = 0;
   trig_L2_tau_tracksinfo_nCoreTracks = 0;
   trig_L2_tau_tracksinfo_nSlowTracks = 0;
   trig_L2_tau_tracksinfo_nIsoTracks = 0;
   trig_L2_tau_tracksinfo_charge = 0;
   trig_L2_tau_tracksinfo_leadingTrackPt = 0;
   trig_L2_tau_tracksinfo_scalarPtSumCore = 0;
   trig_L2_tau_tracksinfo_scalarPtSumIso = 0;
   trig_L2_tau_tracksinfo_3fastest_pt = 0;
   trig_L2_tau_tracksinfo_3fastest_eta = 0;
   trig_L2_tau_tracksinfo_3fastest_phi = 0;
   trig_L2_tau_tracksinfo_3fastest_m = 0;
   trig_RoI_L2_tau_type = 0;
   trig_RoI_L2_tau_lastStep = 0;
   trig_RoI_L2_tau_TrigRoiDescriptor = 0;
   trig_RoI_L2_tau_TrigRoiDescriptorStatus = 0;
   trig_RoI_L2_tau_TrigTauCluster = 0;
   trig_RoI_L2_tau_TrigTauClusterStatus = 0;
   trig_RoI_L2_tau_TrigTauClusterDetails = 0;
   trig_RoI_L2_tau_TrigTauClusterDetailsStatus = 0;
   trig_RoI_L2_tau_TrigTauTracksInfo = 0;
   trig_RoI_L2_tau_TrigTauTracksInfoStatus = 0;
   trig_RoI_L2_tau_TrigTau = 0;
   trig_RoI_L2_tau_TrigTauStatus = 0;
   trig_RoI_L2_tau_EmTau_ROI = 0;
   trig_RoI_L2_tau_EmTau_ROIStatus = 0;
   trig_RoI_L2_tau_TrigInDetTrackCollection = 0;
   trig_RoI_L2_tau_TrigInDetTrackCollectionStatus = 0;
   trig_RoI_EF_tau_type = 0;
   trig_RoI_EF_tau_lastStep = 0;
   trig_RoI_EF_tau_EmTau_ROI = 0;
   trig_RoI_EF_tau_EmTau_ROIStatus = 0;
//    trig_RoI_EF_tau_Analysis__TauJetContainer = 0;
//    trig_RoI_EF_tau_Analysis__TauJetContainerStatus = 0;
   trig_L1_TAV = 0;
   trig_L2_passedPhysics = 0;
   trig_EF_passedPhysics = 0;
   trig_L1_TBP = 0;
   trig_L1_TAP = 0;
   trig_L2_passedRaw = 0;
   trig_EF_passedRaw = 0;
   trig_L2_resurrected = 0;
   trig_EF_resurrected = 0;
   trig_L2_prescaled = 0;
   trig_EF_prescaled = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

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
   fChain->SetBranchAddress("el_breminvpt", &el_breminvpt, &b_el_breminvpt);
   fChain->SetBranchAddress("el_bremradius", &el_bremradius, &b_el_bremradius);
   fChain->SetBranchAddress("el_bremx", &el_bremx, &b_el_bremx);
   fChain->SetBranchAddress("el_bremclusterradius", &el_bremclusterradius, &b_el_bremclusterradius);
   fChain->SetBranchAddress("el_breminvpterr", &el_breminvpterr, &b_el_breminvpterr);
   fChain->SetBranchAddress("el_bremtrackauthor", &el_bremtrackauthor, &b_el_bremtrackauthor);
   fChain->SetBranchAddress("el_hasbrem", &el_hasbrem, &b_el_hasbrem);
   fChain->SetBranchAddress("el_bremdeltaqoverp", &el_bremdeltaqoverp, &b_el_bremdeltaqoverp);
   fChain->SetBranchAddress("el_bremmaterialtraversed", &el_bremmaterialtraversed, &b_el_bremmaterialtraversed);
   fChain->SetBranchAddress("el_refittedtrackqoverp", &el_refittedtrackqoverp, &b_el_refittedtrackqoverp);
   fChain->SetBranchAddress("el_refittedtrackd0", &el_refittedtrackd0, &b_el_refittedtrackd0);
   fChain->SetBranchAddress("el_refittedtrackz0", &el_refittedtrackz0, &b_el_refittedtrackz0);
   fChain->SetBranchAddress("el_refittedtracktheta", &el_refittedtracktheta, &b_el_refittedtracktheta);
   fChain->SetBranchAddress("el_refittedtrackphi", &el_refittedtrackphi, &b_el_refittedtrackphi);
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
   fChain->SetBranchAddress("el_vertx", &el_vertx, &b_el_vertx);
   fChain->SetBranchAddress("el_verty", &el_verty, &b_el_verty);
   fChain->SetBranchAddress("el_vertz", &el_vertz, &b_el_vertz);
   fChain->SetBranchAddress("el_hastrack", &el_hastrack, &b_el_hastrack);
   fChain->SetBranchAddress("el_deltaEmax2", &el_deltaEmax2, &b_el_deltaEmax2);
   fChain->SetBranchAddress("el_trackd0beam", &el_trackd0beam, &b_el_trackd0beam);
   fChain->SetBranchAddress("el_isIso", &el_isIso, &b_el_isIso);
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
   fChain->SetBranchAddress("el_EF_dr", &el_EF_dr, &b_el_EF_dr);
   fChain->SetBranchAddress("el_EF_index", &el_EF_index, &b_el_EF_index);
   fChain->SetBranchAddress("el_L2_dr", &el_L2_dr, &b_el_L2_dr);
   fChain->SetBranchAddress("el_L2_index", &el_L2_index, &b_el_L2_index);
   fChain->SetBranchAddress("el_L1_dr", &el_L1_dr, &b_el_L1_dr);
   fChain->SetBranchAddress("el_L1_index", &el_L1_index, &b_el_L1_index);
   fChain->SetBranchAddress("el_EF_E", &el_EF_E, &b_el_EF_E);
   fChain->SetBranchAddress("el_EF_Et", &el_EF_Et, &b_el_EF_Et);
   fChain->SetBranchAddress("el_EF_pt", &el_EF_pt, &b_el_EF_pt);
   fChain->SetBranchAddress("el_EF_eta", &el_EF_eta, &b_el_EF_eta);
   fChain->SetBranchAddress("el_EF_phi", &el_EF_phi, &b_el_EF_phi);
   fChain->SetBranchAddress("el_EF_charge", &el_EF_charge, &b_el_EF_charge);
   fChain->SetBranchAddress("el_EF_Ethad", &el_EF_Ethad, &b_el_EF_Ethad);
   fChain->SetBranchAddress("el_EF_Ethad1", &el_EF_Ethad1, &b_el_EF_Ethad1);
   fChain->SetBranchAddress("el_EF_f1", &el_EF_f1, &b_el_EF_f1);
   fChain->SetBranchAddress("el_EF_Emins1", &el_EF_Emins1, &b_el_EF_Emins1);
   fChain->SetBranchAddress("el_EF_fside", &el_EF_fside, &b_el_EF_fside);
   fChain->SetBranchAddress("el_EF_Emax2", &el_EF_Emax2, &b_el_EF_Emax2);
   fChain->SetBranchAddress("el_EF_ws3", &el_EF_ws3, &b_el_EF_ws3);
   fChain->SetBranchAddress("el_EF_wstot", &el_EF_wstot, &b_el_EF_wstot);
   fChain->SetBranchAddress("el_EF_E233", &el_EF_E233, &b_el_EF_E233);
   fChain->SetBranchAddress("el_EF_E237", &el_EF_E237, &b_el_EF_E237);
   fChain->SetBranchAddress("el_EF_E277", &el_EF_E277, &b_el_EF_E277);
   fChain->SetBranchAddress("el_EF_weta2", &el_EF_weta2, &b_el_EF_weta2);
   fChain->SetBranchAddress("el_EF_rphiallcalo", &el_EF_rphiallcalo, &b_el_EF_rphiallcalo);
   fChain->SetBranchAddress("el_EF_deltaeta1", &el_EF_deltaeta1, &b_el_EF_deltaeta1);
   fChain->SetBranchAddress("el_EF_deltaeta2", &el_EF_deltaeta2, &b_el_EF_deltaeta2);
   fChain->SetBranchAddress("el_EF_deltaphi2", &el_EF_deltaphi2, &b_el_EF_deltaphi2);
   fChain->SetBranchAddress("el_EF_expectHitInBLayer", &el_EF_expectHitInBLayer, &b_el_EF_expectHitInBLayer);
   fChain->SetBranchAddress("el_EF_Etcone45", &el_EF_Etcone45, &b_el_EF_Etcone45);
   fChain->SetBranchAddress("el_EF_Etcone20", &el_EF_Etcone20, &b_el_EF_Etcone20);
   fChain->SetBranchAddress("el_EF_Etcone30", &el_EF_Etcone30, &b_el_EF_Etcone30);
   fChain->SetBranchAddress("el_EF_Etcone40", &el_EF_Etcone40, &b_el_EF_Etcone40);
   fChain->SetBranchAddress("el_EF_etacorrmag", &el_EF_etacorrmag, &b_el_EF_etacorrmag);
   fChain->SetBranchAddress("el_EF_zvertex", &el_EF_zvertex, &b_el_EF_zvertex);
   fChain->SetBranchAddress("el_EF_errz", &el_EF_errz, &b_el_EF_errz);
   fChain->SetBranchAddress("el_EF_trackd0", &el_EF_trackd0, &b_el_EF_trackd0);
   fChain->SetBranchAddress("el_EF_trackz0", &el_EF_trackz0, &b_el_EF_trackz0);
   fChain->SetBranchAddress("el_EF_trackphi", &el_EF_trackphi, &b_el_EF_trackphi);
   fChain->SetBranchAddress("el_EF_tracktheta", &el_EF_tracktheta, &b_el_EF_tracktheta);
   fChain->SetBranchAddress("el_EF_trackqoverp", &el_EF_trackqoverp, &b_el_EF_trackqoverp);
   fChain->SetBranchAddress("el_EF_trackpt", &el_EF_trackpt, &b_el_EF_trackpt);
   fChain->SetBranchAddress("el_EF_tracketa", &el_EF_tracketa, &b_el_EF_tracketa);
   fChain->SetBranchAddress("el_EF_nBLHits", &el_EF_nBLHits, &b_el_EF_nBLHits);
   fChain->SetBranchAddress("el_EF_nPixHits", &el_EF_nPixHits, &b_el_EF_nPixHits);
   fChain->SetBranchAddress("el_EF_nSCTHits", &el_EF_nSCTHits, &b_el_EF_nSCTHits);
   fChain->SetBranchAddress("el_EF_nTRTHits", &el_EF_nTRTHits, &b_el_EF_nTRTHits);
   fChain->SetBranchAddress("el_EF_nTRTHighTHits", &el_EF_nTRTHighTHits, &b_el_EF_nTRTHighTHits);
   fChain->SetBranchAddress("el_EF_nSiHits", &el_EF_nSiHits, &b_el_EF_nSiHits);
   fChain->SetBranchAddress("el_EF_TRTHighTHitsRatio", &el_EF_TRTHighTHitsRatio, &b_el_EF_TRTHighTHitsRatio);
   fChain->SetBranchAddress("el_EF_pixeldEdx", &el_EF_pixeldEdx, &b_el_EF_pixeldEdx);
   fChain->SetBranchAddress("el_EF_hastrack", &el_EF_hastrack, &b_el_EF_hastrack);
   fChain->SetBranchAddress("el_EF_matched", &el_EF_matched, &b_el_EF_matched);
   fChain->SetBranchAddress("el_L2_E", &el_L2_E, &b_el_L2_E);
   fChain->SetBranchAddress("el_L2_Et", &el_L2_Et, &b_el_L2_Et);
   fChain->SetBranchAddress("el_L2_pt", &el_L2_pt, &b_el_L2_pt);
   fChain->SetBranchAddress("el_L2_eta", &el_L2_eta, &b_el_L2_eta);
   fChain->SetBranchAddress("el_L2_phi", &el_L2_phi, &b_el_L2_phi);
   fChain->SetBranchAddress("el_L2_charge", &el_L2_charge, &b_el_L2_charge);
   fChain->SetBranchAddress("el_L2_Ethad1", &el_L2_Ethad1, &b_el_L2_Ethad1);
   fChain->SetBranchAddress("el_L2_reta", &el_L2_reta, &b_el_L2_reta);
   fChain->SetBranchAddress("el_L2_Eratio", &el_L2_Eratio, &b_el_L2_Eratio);
   fChain->SetBranchAddress("el_L2_trackclusterdeta", &el_L2_trackclusterdeta, &b_el_L2_trackclusterdeta);
   fChain->SetBranchAddress("el_L2_trackclusterdphi", &el_L2_trackclusterdphi, &b_el_L2_trackclusterdphi);
   fChain->SetBranchAddress("el_L2_Etoverpt", &el_L2_Etoverpt, &b_el_L2_Etoverpt);
   fChain->SetBranchAddress("el_L2_trackpt", &el_L2_trackpt, &b_el_L2_trackpt);
   fChain->SetBranchAddress("el_L2_trackalgo", &el_L2_trackalgo, &b_el_L2_trackalgo);
   fChain->SetBranchAddress("el_L2_tracketa", &el_L2_tracketa, &b_el_L2_tracketa);
   fChain->SetBranchAddress("el_L2_trackd0", &el_L2_trackd0, &b_el_L2_trackd0);
   fChain->SetBranchAddress("el_L2_trackz0", &el_L2_trackz0, &b_el_L2_trackz0);
   fChain->SetBranchAddress("el_L2_tracktheta", &el_L2_tracktheta, &b_el_L2_tracktheta);
   fChain->SetBranchAddress("el_L2_trackphi", &el_L2_trackphi, &b_el_L2_trackphi);
   fChain->SetBranchAddress("el_L2_tracketaatcalo", &el_L2_tracketaatcalo, &b_el_L2_tracketaatcalo);
   fChain->SetBranchAddress("el_L2_trackphiatcalo", &el_L2_trackphiatcalo, &b_el_L2_trackphiatcalo);
   fChain->SetBranchAddress("el_L2_errpt", &el_L2_errpt, &b_el_L2_errpt);
   fChain->SetBranchAddress("el_L2_erreta", &el_L2_erreta, &b_el_L2_erreta);
   fChain->SetBranchAddress("el_L2_errphi", &el_L2_errphi, &b_el_L2_errphi);
   fChain->SetBranchAddress("el_L2_E237", &el_L2_E237, &b_el_L2_E237);
   fChain->SetBranchAddress("el_L2_E277", &el_L2_E277, &b_el_L2_E277);
   fChain->SetBranchAddress("el_L2_fside", &el_L2_fside, &b_el_L2_fside);
   fChain->SetBranchAddress("el_L2_weta2", &el_L2_weta2, &b_el_L2_weta2);
   fChain->SetBranchAddress("el_L2_Emaxs1", &el_L2_Emaxs1, &b_el_L2_Emaxs1);
   fChain->SetBranchAddress("el_L2_Emax2", &el_L2_Emax2, &b_el_L2_Emax2);
   fChain->SetBranchAddress("el_L2_matched", &el_L2_matched, &b_el_L2_matched);
   fChain->SetBranchAddress("el_L1_eta", &el_L1_eta, &b_el_L1_eta);
   fChain->SetBranchAddress("el_L1_phi", &el_L1_phi, &b_el_L1_phi);
   fChain->SetBranchAddress("el_L1_EMisol", &el_L1_EMisol, &b_el_L1_EMisol);
   fChain->SetBranchAddress("el_L1_hadisol", &el_L1_hadisol, &b_el_L1_hadisol);
   fChain->SetBranchAddress("el_L1_hadcore", &el_L1_hadcore, &b_el_L1_hadcore);
   fChain->SetBranchAddress("el_L1_pt", &el_L1_pt, &b_el_L1_pt);
   fChain->SetBranchAddress("el_L1_Et", &el_L1_Et, &b_el_L1_Et);
   fChain->SetBranchAddress("el_L1_matched", &el_L1_matched, &b_el_L1_matched);
   fChain->SetBranchAddress("EF_2e3_NoCut", &EF_2e3_NoCut, &b_EF_2e3_NoCut);
   fChain->SetBranchAddress("EF_2e3_loose", &EF_2e3_loose, &b_EF_2e3_loose);
   fChain->SetBranchAddress("EF_2e3_medium", &EF_2e3_medium, &b_EF_2e3_medium);
   fChain->SetBranchAddress("EF_2e5_loose", &EF_2e5_loose, &b_EF_2e5_loose);
   fChain->SetBranchAddress("EF_2e5_medium", &EF_2e5_medium, &b_EF_2e5_medium);
   fChain->SetBranchAddress("EF_2e6_nocut", &EF_2e6_nocut, &b_EF_2e6_nocut);
   fChain->SetBranchAddress("EF_2g10_nocut", &EF_2g10_nocut, &b_EF_2g10_nocut);
   fChain->SetBranchAddress("EF_2g3_loose", &EF_2g3_loose, &b_EF_2g3_loose);
   fChain->SetBranchAddress("EF_2g3_nocut", &EF_2g3_nocut, &b_EF_2g3_nocut);
   fChain->SetBranchAddress("EF_2g5_loose", &EF_2g5_loose, &b_EF_2g5_loose);
   fChain->SetBranchAddress("EF_2mu10", &EF_2mu10, &b_EF_2mu10);
   fChain->SetBranchAddress("EF_2mu4", &EF_2mu4, &b_EF_2mu4);
   fChain->SetBranchAddress("EF_2mu4_DiMu", &EF_2mu4_DiMu, &b_EF_2mu4_DiMu);
   fChain->SetBranchAddress("EF_2mu4_DiMu_SiTrk", &EF_2mu4_DiMu_SiTrk, &b_EF_2mu4_DiMu_SiTrk);
   fChain->SetBranchAddress("EF_2mu4_DiMu_noOS", &EF_2mu4_DiMu_noOS, &b_EF_2mu4_DiMu_noOS);
   fChain->SetBranchAddress("EF_2mu4_DiMu_noOS_passL2", &EF_2mu4_DiMu_noOS_passL2, &b_EF_2mu4_DiMu_noOS_passL2);
   fChain->SetBranchAddress("EF_2mu4_DiMu_noVtx_noOS", &EF_2mu4_DiMu_noVtx_noOS, &b_EF_2mu4_DiMu_noVtx_noOS);
   fChain->SetBranchAddress("EF_2mu4_DiMu_noVtx_noOS_passL2", &EF_2mu4_DiMu_noVtx_noOS_passL2, &b_EF_2mu4_DiMu_noVtx_noOS_passL2);
   fChain->SetBranchAddress("EF_2mu4_MSonly", &EF_2mu4_MSonly, &b_EF_2mu4_MSonly);
   fChain->SetBranchAddress("EF_2mu6", &EF_2mu6, &b_EF_2mu6);
   fChain->SetBranchAddress("EF_2mu6_DiMu", &EF_2mu6_DiMu, &b_EF_2mu6_DiMu);
   fChain->SetBranchAddress("EF_2mu6_DiMu_noVtx_noOS", &EF_2mu6_DiMu_noVtx_noOS, &b_EF_2mu6_DiMu_noVtx_noOS);
   fChain->SetBranchAddress("EF_e10_loose", &EF_e10_loose, &b_EF_e10_loose);
   fChain->SetBranchAddress("EF_e10_loose_IdScan", &EF_e10_loose_IdScan, &b_EF_e10_loose_IdScan);
   fChain->SetBranchAddress("EF_e10_loose_TRT", &EF_e10_loose_TRT, &b_EF_e10_loose_TRT);
   fChain->SetBranchAddress("EF_e10_loose_passEF", &EF_e10_loose_passEF, &b_EF_e10_loose_passEF);
   fChain->SetBranchAddress("EF_e10_loose_passL2", &EF_e10_loose_passL2, &b_EF_e10_loose_passL2);
   fChain->SetBranchAddress("EF_e10_medium", &EF_e10_medium, &b_EF_e10_medium);
   fChain->SetBranchAddress("EF_e10_medium_IdScan", &EF_e10_medium_IdScan, &b_EF_e10_medium_IdScan);
   fChain->SetBranchAddress("EF_e10_medium_TRT", &EF_e10_medium_TRT, &b_EF_e10_medium_TRT);
   fChain->SetBranchAddress("EF_e10i_loose", &EF_e10i_loose, &b_EF_e10i_loose);
   fChain->SetBranchAddress("EF_e15_loose", &EF_e15_loose, &b_EF_e15_loose);
   fChain->SetBranchAddress("EF_e15i_loose", &EF_e15i_loose, &b_EF_e15i_loose);
   fChain->SetBranchAddress("EF_e20_loose_NoIDTrkCut", &EF_e20_loose_NoIDTrkCut, &b_EF_e20_loose_NoIDTrkCut);
   fChain->SetBranchAddress("EF_e3_NoCut", &EF_e3_NoCut, &b_EF_e3_NoCut);
   fChain->SetBranchAddress("EF_e3_NoCut_IdScan", &EF_e3_NoCut_IdScan, &b_EF_e3_NoCut_IdScan);
   fChain->SetBranchAddress("EF_e3_NoCut_TRT", &EF_e3_NoCut_TRT, &b_EF_e3_NoCut_TRT);
   fChain->SetBranchAddress("EF_e3_loose", &EF_e3_loose, &b_EF_e3_loose);
   fChain->SetBranchAddress("EF_e3_loose_IdScan", &EF_e3_loose_IdScan, &b_EF_e3_loose_IdScan);
   fChain->SetBranchAddress("EF_e3_loose_TRT", &EF_e3_loose_TRT, &b_EF_e3_loose_TRT);
   fChain->SetBranchAddress("EF_e3_medium", &EF_e3_medium, &b_EF_e3_medium);
   fChain->SetBranchAddress("EF_e3_medium_IdScan", &EF_e3_medium_IdScan, &b_EF_e3_medium_IdScan);
   fChain->SetBranchAddress("EF_e3_medium_TRT", &EF_e3_medium_TRT, &b_EF_e3_medium_TRT);
   fChain->SetBranchAddress("EF_e5_EFfullcalo", &EF_e5_EFfullcalo, &b_EF_e5_EFfullcalo);
   fChain->SetBranchAddress("EF_e5_NoCut", &EF_e5_NoCut, &b_EF_e5_NoCut);
   fChain->SetBranchAddress("EF_e5_NoCut_FwdBackTrk", &EF_e5_NoCut_FwdBackTrk, &b_EF_e5_NoCut_FwdBackTrk);
   fChain->SetBranchAddress("EF_e5_NoCut_IdScan", &EF_e5_NoCut_IdScan, &b_EF_e5_NoCut_IdScan);
   fChain->SetBranchAddress("EF_e5_NoCut_L2SW", &EF_e5_NoCut_L2SW, &b_EF_e5_NoCut_L2SW);
   fChain->SetBranchAddress("EF_e5_NoCut_Ringer", &EF_e5_NoCut_Ringer, &b_EF_e5_NoCut_Ringer);
   fChain->SetBranchAddress("EF_e5_NoCut_SiTrk", &EF_e5_NoCut_SiTrk, &b_EF_e5_NoCut_SiTrk);
   fChain->SetBranchAddress("EF_e5_NoCut_TRT", &EF_e5_NoCut_TRT, &b_EF_e5_NoCut_TRT);
   fChain->SetBranchAddress("EF_e5_NoCut_cells", &EF_e5_NoCut_cells, &b_EF_e5_NoCut_cells);
   fChain->SetBranchAddress("EF_e5_NoCut_cosmic", &EF_e5_NoCut_cosmic, &b_EF_e5_NoCut_cosmic);
   fChain->SetBranchAddress("EF_e5_NoCut_firstempty", &EF_e5_NoCut_firstempty, &b_EF_e5_NoCut_firstempty);
   fChain->SetBranchAddress("EF_e5_NoCut_unpaired", &EF_e5_NoCut_unpaired, &b_EF_e5_NoCut_unpaired);
   fChain->SetBranchAddress("EF_e5_loose", &EF_e5_loose, &b_EF_e5_loose);
   fChain->SetBranchAddress("EF_e5_loose_IdScan", &EF_e5_loose_IdScan, &b_EF_e5_loose_IdScan);
   fChain->SetBranchAddress("EF_e5_loose_NoIDTrkCut", &EF_e5_loose_NoIDTrkCut, &b_EF_e5_loose_NoIDTrkCut);
   fChain->SetBranchAddress("EF_e5_loose_TRT", &EF_e5_loose_TRT, &b_EF_e5_loose_TRT);
   fChain->SetBranchAddress("EF_e5_medium", &EF_e5_medium, &b_EF_e5_medium);
   fChain->SetBranchAddress("EF_e5_medium_IdScan", &EF_e5_medium_IdScan, &b_EF_e5_medium_IdScan);
   fChain->SetBranchAddress("EF_e5_medium_MV", &EF_e5_medium_MV, &b_EF_e5_medium_MV);
   fChain->SetBranchAddress("EF_e5_medium_TRT", &EF_e5_medium_TRT, &b_EF_e5_medium_TRT);
   fChain->SetBranchAddress("EF_e6_loose", &EF_e6_loose, &b_EF_e6_loose);
   fChain->SetBranchAddress("EF_g10_loose", &EF_g10_loose, &b_EF_g10_loose);
   fChain->SetBranchAddress("EF_g10_loose_larcalib", &EF_g10_loose_larcalib, &b_EF_g10_loose_larcalib);
   fChain->SetBranchAddress("EF_g10i_loose", &EF_g10i_loose, &b_EF_g10i_loose);
   fChain->SetBranchAddress("EF_g15_loose", &EF_g15_loose, &b_EF_g15_loose);
   fChain->SetBranchAddress("EF_g15i_loose", &EF_g15i_loose, &b_EF_g15i_loose);
   fChain->SetBranchAddress("EF_g3_loose", &EF_g3_loose, &b_EF_g3_loose);
   fChain->SetBranchAddress("EF_g3_nocut", &EF_g3_nocut, &b_EF_g3_nocut);
   fChain->SetBranchAddress("EF_g3_nocut_larcalib", &EF_g3_nocut_larcalib, &b_EF_g3_nocut_larcalib);
   fChain->SetBranchAddress("EF_g3_nocut_unpaired", &EF_g3_nocut_unpaired, &b_EF_g3_nocut_unpaired);
   fChain->SetBranchAddress("EF_g5_EFfullcalo", &EF_g5_EFfullcalo, &b_EF_g5_EFfullcalo);
   fChain->SetBranchAddress("EF_g5_loose", &EF_g5_loose, &b_EF_g5_loose);
   fChain->SetBranchAddress("EF_g5_loose_larcalib", &EF_g5_loose_larcalib, &b_EF_g5_loose_larcalib);
   fChain->SetBranchAddress("EF_g5_nocut", &EF_g5_nocut, &b_EF_g5_nocut);
   fChain->SetBranchAddress("EF_g5_nocut_cosmic", &EF_g5_nocut_cosmic, &b_EF_g5_nocut_cosmic);
   fChain->SetBranchAddress("EF_g5_nocut_firstempty", &EF_g5_nocut_firstempty, &b_EF_g5_nocut_firstempty);
   fChain->SetBranchAddress("EF_g5_nocut_unpaired", &EF_g5_nocut_unpaired, &b_EF_g5_nocut_unpaired);
   fChain->SetBranchAddress("EF_g6_loose", &EF_g6_loose, &b_EF_g6_loose);
   fChain->SetBranchAddress("EF_mu0_missingRoi", &EF_mu0_missingRoi, &b_EF_mu0_missingRoi);
   fChain->SetBranchAddress("EF_mu0_rpcOnly", &EF_mu0_rpcOnly, &b_EF_mu0_rpcOnly);
   fChain->SetBranchAddress("EF_mu10", &EF_mu10, &b_EF_mu10);
   fChain->SetBranchAddress("EF_mu10_L2MSonly_EFFS_passL2", &EF_mu10_L2MSonly_EFFS_passL2, &b_EF_mu10_L2MSonly_EFFS_passL2);
   fChain->SetBranchAddress("EF_mu10_MG", &EF_mu10_MG, &b_EF_mu10_MG);
   fChain->SetBranchAddress("EF_mu10_MSonly", &EF_mu10_MSonly, &b_EF_mu10_MSonly);
   fChain->SetBranchAddress("EF_mu10_MSonly_EFFS_passL2", &EF_mu10_MSonly_EFFS_passL2, &b_EF_mu10_MSonly_EFFS_passL2);
   fChain->SetBranchAddress("EF_mu10_MSonly_cosmic", &EF_mu10_MSonly_cosmic, &b_EF_mu10_MSonly_cosmic);
   fChain->SetBranchAddress("EF_mu10_NoIDTrkCut", &EF_mu10_NoIDTrkCut, &b_EF_mu10_NoIDTrkCut);
   fChain->SetBranchAddress("EF_mu10_SiTrk", &EF_mu10_SiTrk, &b_EF_mu10_SiTrk);
   fChain->SetBranchAddress("EF_mu10_cosmic", &EF_mu10_cosmic, &b_EF_mu10_cosmic);
   fChain->SetBranchAddress("EF_mu10_muCombTag", &EF_mu10_muCombTag, &b_EF_mu10_muCombTag);
   fChain->SetBranchAddress("EF_mu10_muCombTag_SiTrk", &EF_mu10_muCombTag_SiTrk, &b_EF_mu10_muCombTag_SiTrk);
   fChain->SetBranchAddress("EF_mu10_passHLT", &EF_mu10_passHLT, &b_EF_mu10_passHLT);
   fChain->SetBranchAddress("EF_mu10i_loose", &EF_mu10i_loose, &b_EF_mu10i_loose);
   fChain->SetBranchAddress("EF_mu13", &EF_mu13, &b_EF_mu13);
   fChain->SetBranchAddress("EF_mu15", &EF_mu15, &b_EF_mu15);
   fChain->SetBranchAddress("EF_mu20", &EF_mu20, &b_EF_mu20);
   fChain->SetBranchAddress("EF_mu20_MSonly", &EF_mu20_MSonly, &b_EF_mu20_MSonly);
   fChain->SetBranchAddress("EF_mu20_passHLT", &EF_mu20_passHLT, &b_EF_mu20_passHLT);
   fChain->SetBranchAddress("EF_mu4", &EF_mu4, &b_EF_mu4);
   fChain->SetBranchAddress("EF_mu4_DiMu", &EF_mu4_DiMu, &b_EF_mu4_DiMu);
   fChain->SetBranchAddress("EF_mu4_DiMu_FS", &EF_mu4_DiMu_FS, &b_EF_mu4_DiMu_FS);
   fChain->SetBranchAddress("EF_mu4_DiMu_FS_noOS", &EF_mu4_DiMu_FS_noOS, &b_EF_mu4_DiMu_FS_noOS);
   fChain->SetBranchAddress("EF_mu4_Jpsie5e3", &EF_mu4_Jpsie5e3, &b_EF_mu4_Jpsie5e3);
   fChain->SetBranchAddress("EF_mu4_Jpsie5e3_FS", &EF_mu4_Jpsie5e3_FS, &b_EF_mu4_Jpsie5e3_FS);
   fChain->SetBranchAddress("EF_mu4_Jpsie5e3_SiTrk", &EF_mu4_Jpsie5e3_SiTrk, &b_EF_mu4_Jpsie5e3_SiTrk);
   fChain->SetBranchAddress("EF_mu4_Jpsie5e3_SiTrk_FS", &EF_mu4_Jpsie5e3_SiTrk_FS, &b_EF_mu4_Jpsie5e3_SiTrk_FS);
   fChain->SetBranchAddress("EF_mu4_L2MSonly_EFFS_passL2", &EF_mu4_L2MSonly_EFFS_passL2, &b_EF_mu4_L2MSonly_EFFS_passL2);
   fChain->SetBranchAddress("EF_mu4_MG", &EF_mu4_MG, &b_EF_mu4_MG);
   fChain->SetBranchAddress("EF_mu4_MSonly", &EF_mu4_MSonly, &b_EF_mu4_MSonly);
   fChain->SetBranchAddress("EF_mu4_MSonly_EFFS_passL2", &EF_mu4_MSonly_EFFS_passL2, &b_EF_mu4_MSonly_EFFS_passL2);
   fChain->SetBranchAddress("EF_mu4_MSonly_MB2_noL2_EFFS", &EF_mu4_MSonly_MB2_noL2_EFFS, &b_EF_mu4_MSonly_MB2_noL2_EFFS);
   fChain->SetBranchAddress("EF_mu4_MSonly_Trk_Jpsi_loose", &EF_mu4_MSonly_Trk_Jpsi_loose, &b_EF_mu4_MSonly_Trk_Jpsi_loose);
   fChain->SetBranchAddress("EF_mu4_MSonly_Trk_Upsi_loose_FS", &EF_mu4_MSonly_Trk_Upsi_loose_FS, &b_EF_mu4_MSonly_Trk_Upsi_loose_FS);
   fChain->SetBranchAddress("EF_mu4_MSonly_cosmic", &EF_mu4_MSonly_cosmic, &b_EF_mu4_MSonly_cosmic);
   fChain->SetBranchAddress("EF_mu4_MSonly_firstempty", &EF_mu4_MSonly_firstempty, &b_EF_mu4_MSonly_firstempty);
   fChain->SetBranchAddress("EF_mu4_MV", &EF_mu4_MV, &b_EF_mu4_MV);
   fChain->SetBranchAddress("EF_mu4_NoIDTrkCut", &EF_mu4_NoIDTrkCut, &b_EF_mu4_NoIDTrkCut);
   fChain->SetBranchAddress("EF_mu4_SiTrk", &EF_mu4_SiTrk, &b_EF_mu4_SiTrk);
   fChain->SetBranchAddress("EF_mu4_Trk_Jpsi_loose", &EF_mu4_Trk_Jpsi_loose, &b_EF_mu4_Trk_Jpsi_loose);
   fChain->SetBranchAddress("EF_mu4_Trk_Upsi_loose_FS", &EF_mu4_Trk_Upsi_loose_FS, &b_EF_mu4_Trk_Upsi_loose_FS);
   fChain->SetBranchAddress("EF_mu4_comm", &EF_mu4_comm, &b_EF_mu4_comm);
   fChain->SetBranchAddress("EF_mu4_comm_MSonly", &EF_mu4_comm_MSonly, &b_EF_mu4_comm_MSonly);
   fChain->SetBranchAddress("EF_mu4_comm_MSonly_cosmic", &EF_mu4_comm_MSonly_cosmic, &b_EF_mu4_comm_MSonly_cosmic);
   fChain->SetBranchAddress("EF_mu4_comm_MSonly_firstempty", &EF_mu4_comm_MSonly_firstempty, &b_EF_mu4_comm_MSonly_firstempty);
   fChain->SetBranchAddress("EF_mu4_comm_cosmic", &EF_mu4_comm_cosmic, &b_EF_mu4_comm_cosmic);
   fChain->SetBranchAddress("EF_mu4_comm_firstempty", &EF_mu4_comm_firstempty, &b_EF_mu4_comm_firstempty);
   fChain->SetBranchAddress("EF_mu4_cosmic", &EF_mu4_cosmic, &b_EF_mu4_cosmic);
   fChain->SetBranchAddress("EF_mu4_firstempty", &EF_mu4_firstempty, &b_EF_mu4_firstempty);
   fChain->SetBranchAddress("EF_mu4_j10_matched", &EF_mu4_j10_matched, &b_EF_mu4_j10_matched);
   fChain->SetBranchAddress("EF_mu4_j20_matched", &EF_mu4_j20_matched, &b_EF_mu4_j20_matched);
   fChain->SetBranchAddress("EF_mu4_j5_matched", &EF_mu4_j5_matched, &b_EF_mu4_j5_matched);
   fChain->SetBranchAddress("EF_mu4_mu6", &EF_mu4_mu6, &b_EF_mu4_mu6);
   fChain->SetBranchAddress("EF_mu4_muCombTag", &EF_mu4_muCombTag, &b_EF_mu4_muCombTag);
   fChain->SetBranchAddress("EF_mu4_muCombTag_SiTrk", &EF_mu4_muCombTag_SiTrk, &b_EF_mu4_muCombTag_SiTrk);
   fChain->SetBranchAddress("EF_mu4_passHLT", &EF_mu4_passHLT, &b_EF_mu4_passHLT);
   fChain->SetBranchAddress("EF_mu4_tile", &EF_mu4_tile, &b_EF_mu4_tile);
   fChain->SetBranchAddress("EF_mu4_tile_SiTrk", &EF_mu4_tile_SiTrk, &b_EF_mu4_tile_SiTrk);
   fChain->SetBranchAddress("EF_mu4_trod", &EF_mu4_trod, &b_EF_mu4_trod);
   fChain->SetBranchAddress("EF_mu4_trod_SiTrk", &EF_mu4_trod_SiTrk, &b_EF_mu4_trod_SiTrk);
   fChain->SetBranchAddress("EF_mu4_unpaired", &EF_mu4_unpaired, &b_EF_mu4_unpaired);
   fChain->SetBranchAddress("EF_mu4mu6_DiMu", &EF_mu4mu6_DiMu, &b_EF_mu4mu6_DiMu);
   fChain->SetBranchAddress("EF_mu6", &EF_mu6, &b_EF_mu6);
   fChain->SetBranchAddress("EF_mu6_DiMu", &EF_mu6_DiMu, &b_EF_mu6_DiMu);
   fChain->SetBranchAddress("EF_mu6_DiMu_FS", &EF_mu6_DiMu_FS, &b_EF_mu6_DiMu_FS);
   fChain->SetBranchAddress("EF_mu6_DiMu_FS_noOS", &EF_mu6_DiMu_FS_noOS, &b_EF_mu6_DiMu_FS_noOS);
   fChain->SetBranchAddress("EF_mu6_DsPhiPi", &EF_mu6_DsPhiPi, &b_EF_mu6_DsPhiPi);
   fChain->SetBranchAddress("EF_mu6_Jpsie5e3", &EF_mu6_Jpsie5e3, &b_EF_mu6_Jpsie5e3);
   fChain->SetBranchAddress("EF_mu6_Jpsie5e3_FS", &EF_mu6_Jpsie5e3_FS, &b_EF_mu6_Jpsie5e3_FS);
   fChain->SetBranchAddress("EF_mu6_Jpsie5e3_SiTrk", &EF_mu6_Jpsie5e3_SiTrk, &b_EF_mu6_Jpsie5e3_SiTrk);
   fChain->SetBranchAddress("EF_mu6_Jpsie5e3_SiTrk_FS", &EF_mu6_Jpsie5e3_SiTrk_FS, &b_EF_mu6_Jpsie5e3_SiTrk_FS);
   fChain->SetBranchAddress("EF_mu6_L2MSonly_EFFS_passL2", &EF_mu6_L2MSonly_EFFS_passL2, &b_EF_mu6_L2MSonly_EFFS_passL2);
   fChain->SetBranchAddress("EF_mu6_MG", &EF_mu6_MG, &b_EF_mu6_MG);
   fChain->SetBranchAddress("EF_mu6_MSonly", &EF_mu6_MSonly, &b_EF_mu6_MSonly);
   fChain->SetBranchAddress("EF_mu6_MSonly_EFFS_passL2", &EF_mu6_MSonly_EFFS_passL2, &b_EF_mu6_MSonly_EFFS_passL2);
   fChain->SetBranchAddress("EF_mu6_MSonly_cosmic", &EF_mu6_MSonly_cosmic, &b_EF_mu6_MSonly_cosmic);
   fChain->SetBranchAddress("EF_mu6_MSonly_firstempty", &EF_mu6_MSonly_firstempty, &b_EF_mu6_MSonly_firstempty);
   fChain->SetBranchAddress("EF_mu6_MSonly_unpaired", &EF_mu6_MSonly_unpaired, &b_EF_mu6_MSonly_unpaired);
   fChain->SetBranchAddress("EF_mu6_SiTrk", &EF_mu6_SiTrk, &b_EF_mu6_SiTrk);
   fChain->SetBranchAddress("EF_mu6_Trk_Jpsi_loose", &EF_mu6_Trk_Jpsi_loose, &b_EF_mu6_Trk_Jpsi_loose);
   fChain->SetBranchAddress("EF_mu6_Trk_Jpsi_loose_FS", &EF_mu6_Trk_Jpsi_loose_FS, &b_EF_mu6_Trk_Jpsi_loose_FS);
   fChain->SetBranchAddress("EF_mu6_Trk_Upsi_loose_FS", &EF_mu6_Trk_Upsi_loose_FS, &b_EF_mu6_Trk_Upsi_loose_FS);
   fChain->SetBranchAddress("EF_mu6_cosmic", &EF_mu6_cosmic, &b_EF_mu6_cosmic);
   fChain->SetBranchAddress("EF_mu6_firstempty", &EF_mu6_firstempty, &b_EF_mu6_firstempty);
   fChain->SetBranchAddress("EF_mu6_muCombTag", &EF_mu6_muCombTag, &b_EF_mu6_muCombTag);
   fChain->SetBranchAddress("EF_mu6_muCombTag_SiTrk", &EF_mu6_muCombTag_SiTrk, &b_EF_mu6_muCombTag_SiTrk);
   fChain->SetBranchAddress("EF_mu6_passHLT", &EF_mu6_passHLT, &b_EF_mu6_passHLT);
   fChain->SetBranchAddress("EF_mu6_unpaired", &EF_mu6_unpaired, &b_EF_mu6_unpaired);
   fChain->SetBranchAddress("EF_tau12_loose_xe20", &EF_tau12_loose_xe20, &b_EF_tau12_loose_xe20);
   fChain->SetBranchAddress("EF_tau16_loose_xe20", &EF_tau16_loose_xe20, &b_EF_tau16_loose_xe20);
   fChain->SetBranchAddress("EF_tauNoCut_hasTrk_xe20", &EF_tauNoCut_hasTrk_xe20, &b_EF_tauNoCut_hasTrk_xe20);
   fChain->SetBranchAddress("EF_xe20", &EF_xe20, &b_EF_xe20);
   fChain->SetBranchAddress("EF_xe20_FEB", &EF_xe20_FEB, &b_EF_xe20_FEB);
   fChain->SetBranchAddress("EF_xe20_allCells", &EF_xe20_allCells, &b_EF_xe20_allCells);
   fChain->SetBranchAddress("EF_xe20_noMu", &EF_xe20_noMu, &b_EF_xe20_noMu);
   fChain->SetBranchAddress("EF_xe25_noMu", &EF_xe25_noMu, &b_EF_xe25_noMu);
   fChain->SetBranchAddress("EF_xe30_allL1", &EF_xe30_allL1, &b_EF_xe30_allL1);
   fChain->SetBranchAddress("EF_xe30_allL1_FEB", &EF_xe30_allL1_FEB, &b_EF_xe30_allL1_FEB);
   fChain->SetBranchAddress("EF_xe30_allL1_allCells", &EF_xe30_allL1_allCells, &b_EF_xe30_allL1_allCells);
   fChain->SetBranchAddress("EF_xe30_noMu", &EF_xe30_noMu, &b_EF_xe30_noMu);
   fChain->SetBranchAddress("EF_xe30_unbiased", &EF_xe30_unbiased, &b_EF_xe30_unbiased);
   fChain->SetBranchAddress("EF_xe30_unbiased_FEB", &EF_xe30_unbiased_FEB, &b_EF_xe30_unbiased_FEB);
   fChain->SetBranchAddress("EF_xe30_unbiased_allCells", &EF_xe30_unbiased_allCells, &b_EF_xe30_unbiased_allCells);
   fChain->SetBranchAddress("EF_xe35_noMu", &EF_xe35_noMu, &b_EF_xe35_noMu);
   fChain->SetBranchAddress("EF_xe40_noMu", &EF_xe40_noMu, &b_EF_xe40_noMu);
   fChain->SetBranchAddress("EF_xe45_noMu", &EF_xe45_noMu, &b_EF_xe45_noMu);
   fChain->SetBranchAddress("EF_xe60_medium_noMu", &EF_xe60_medium_noMu, &b_EF_xe60_medium_noMu);
   fChain->SetBranchAddress("EF_xe70_medium_noMu", &EF_xe70_medium_noMu, &b_EF_xe70_medium_noMu);
   fChain->SetBranchAddress("EF_xe80_medium_noMu", &EF_xe80_medium_noMu, &b_EF_xe80_medium_noMu);
   fChain->SetBranchAddress("L1_2MU0", &L1_2MU0, &b_L1_2MU0);
   fChain->SetBranchAddress("L1_2MU0_MU6", &L1_2MU0_MU6, &b_L1_2MU0_MU6);
   fChain->SetBranchAddress("L1_2MU10", &L1_2MU10, &b_L1_2MU10);
   fChain->SetBranchAddress("L1_2MU6", &L1_2MU6, &b_L1_2MU6);
   fChain->SetBranchAddress("L1_EM10", &L1_EM10, &b_L1_EM10);
   fChain->SetBranchAddress("L1_EM10I", &L1_EM10I, &b_L1_EM10I);
   fChain->SetBranchAddress("L1_EM14", &L1_EM14, &b_L1_EM14);
   fChain->SetBranchAddress("L1_EM2", &L1_EM2, &b_L1_EM2);
   fChain->SetBranchAddress("L1_EM2_UNPAIRED", &L1_EM2_UNPAIRED, &b_L1_EM2_UNPAIRED);
   fChain->SetBranchAddress("L1_EM3", &L1_EM3, &b_L1_EM3);
   fChain->SetBranchAddress("L1_EM3_EMPTY", &L1_EM3_EMPTY, &b_L1_EM3_EMPTY);
   fChain->SetBranchAddress("L1_EM3_FIRSTEMPTY", &L1_EM3_FIRSTEMPTY, &b_L1_EM3_FIRSTEMPTY);
   fChain->SetBranchAddress("L1_EM3_MV", &L1_EM3_MV, &b_L1_EM3_MV);
   fChain->SetBranchAddress("L1_EM3_UNPAIRED", &L1_EM3_UNPAIRED, &b_L1_EM3_UNPAIRED);
   fChain->SetBranchAddress("L1_EM4", &L1_EM4, &b_L1_EM4);
   fChain->SetBranchAddress("L1_EM5", &L1_EM5, &b_L1_EM5);
   fChain->SetBranchAddress("L1_EM5I", &L1_EM5I, &b_L1_EM5I);
   fChain->SetBranchAddress("L1_MU0", &L1_MU0, &b_L1_MU0);
   fChain->SetBranchAddress("L1_MU0_COMM", &L1_MU0_COMM, &b_L1_MU0_COMM);
   fChain->SetBranchAddress("L1_MU0_COMM_EMPTY", &L1_MU0_COMM_EMPTY, &b_L1_MU0_COMM_EMPTY);
   fChain->SetBranchAddress("L1_MU0_COMM_FIRSTEMPTY", &L1_MU0_COMM_FIRSTEMPTY, &b_L1_MU0_COMM_FIRSTEMPTY);
   fChain->SetBranchAddress("L1_MU0_EM3", &L1_MU0_EM3, &b_L1_MU0_EM3);
   fChain->SetBranchAddress("L1_MU0_EMPTY", &L1_MU0_EMPTY, &b_L1_MU0_EMPTY);
   fChain->SetBranchAddress("L1_MU0_FIRSTEMPTY", &L1_MU0_FIRSTEMPTY, &b_L1_MU0_FIRSTEMPTY);
   fChain->SetBranchAddress("L1_MU0_J10", &L1_MU0_J10, &b_L1_MU0_J10);
   fChain->SetBranchAddress("L1_MU0_J15", &L1_MU0_J15, &b_L1_MU0_J15);
   fChain->SetBranchAddress("L1_MU0_J5", &L1_MU0_J5, &b_L1_MU0_J5);
   fChain->SetBranchAddress("L1_MU0_MV", &L1_MU0_MV, &b_L1_MU0_MV);
   fChain->SetBranchAddress("L1_MU0_UNPAIRED", &L1_MU0_UNPAIRED, &b_L1_MU0_UNPAIRED);
   fChain->SetBranchAddress("L1_MU10", &L1_MU10, &b_L1_MU10);
   fChain->SetBranchAddress("L1_MU10_EMPTY", &L1_MU10_EMPTY, &b_L1_MU10_EMPTY);
   fChain->SetBranchAddress("L1_MU15", &L1_MU15, &b_L1_MU15);
   fChain->SetBranchAddress("L1_MU20", &L1_MU20, &b_L1_MU20);
   fChain->SetBranchAddress("L1_MU6", &L1_MU6, &b_L1_MU6);
   fChain->SetBranchAddress("L1_MU6_EM3", &L1_MU6_EM3, &b_L1_MU6_EM3);
   fChain->SetBranchAddress("L1_MU6_EMPTY", &L1_MU6_EMPTY, &b_L1_MU6_EMPTY);
   fChain->SetBranchAddress("L1_MU6_FIRSTEMPTY", &L1_MU6_FIRSTEMPTY, &b_L1_MU6_FIRSTEMPTY);
   fChain->SetBranchAddress("L1_MU6_J5", &L1_MU6_J5, &b_L1_MU6_J5);
   fChain->SetBranchAddress("L1_MU6_UNPAIRED", &L1_MU6_UNPAIRED, &b_L1_MU6_UNPAIRED);
   fChain->SetBranchAddress("L1_TAU5_XE10", &L1_TAU5_XE10, &b_L1_TAU5_XE10);
   fChain->SetBranchAddress("L1_TAU6_XE10", &L1_TAU6_XE10, &b_L1_TAU6_XE10);
   fChain->SetBranchAddress("L1_XE10", &L1_XE10, &b_L1_XE10);
   fChain->SetBranchAddress("L1_XE15", &L1_XE15, &b_L1_XE15);
   fChain->SetBranchAddress("L1_XE20", &L1_XE20, &b_L1_XE20);
   fChain->SetBranchAddress("L1_XE25", &L1_XE25, &b_L1_XE25);
   fChain->SetBranchAddress("L1_XE30", &L1_XE30, &b_L1_XE30);
   fChain->SetBranchAddress("L1_XE35", &L1_XE35, &b_L1_XE35);
   fChain->SetBranchAddress("L1_XE40", &L1_XE40, &b_L1_XE40);
   fChain->SetBranchAddress("L1_XE50", &L1_XE50, &b_L1_XE50);
   fChain->SetBranchAddress("L2_2e3_NoCut", &L2_2e3_NoCut, &b_L2_2e3_NoCut);
   fChain->SetBranchAddress("L2_2e3_loose", &L2_2e3_loose, &b_L2_2e3_loose);
   fChain->SetBranchAddress("L2_2e3_medium", &L2_2e3_medium, &b_L2_2e3_medium);
   fChain->SetBranchAddress("L2_2e5_loose", &L2_2e5_loose, &b_L2_2e5_loose);
   fChain->SetBranchAddress("L2_2e5_medium", &L2_2e5_medium, &b_L2_2e5_medium);
   fChain->SetBranchAddress("L2_2e6_nocut", &L2_2e6_nocut, &b_L2_2e6_nocut);
   fChain->SetBranchAddress("L2_2g10_nocut", &L2_2g10_nocut, &b_L2_2g10_nocut);
   fChain->SetBranchAddress("L2_2g3_loose", &L2_2g3_loose, &b_L2_2g3_loose);
   fChain->SetBranchAddress("L2_2g3_nocut", &L2_2g3_nocut, &b_L2_2g3_nocut);
   fChain->SetBranchAddress("L2_2g5_loose", &L2_2g5_loose, &b_L2_2g5_loose);
   fChain->SetBranchAddress("L2_2mu10", &L2_2mu10, &b_L2_2mu10);
   fChain->SetBranchAddress("L2_2mu4", &L2_2mu4, &b_L2_2mu4);
   fChain->SetBranchAddress("L2_2mu4_DiMu", &L2_2mu4_DiMu, &b_L2_2mu4_DiMu);
   fChain->SetBranchAddress("L2_2mu4_DiMu_SiTrk", &L2_2mu4_DiMu_SiTrk, &b_L2_2mu4_DiMu_SiTrk);
   fChain->SetBranchAddress("L2_2mu4_DiMu_noOS", &L2_2mu4_DiMu_noOS, &b_L2_2mu4_DiMu_noOS);
   fChain->SetBranchAddress("L2_2mu4_DiMu_noOS_passL2", &L2_2mu4_DiMu_noOS_passL2, &b_L2_2mu4_DiMu_noOS_passL2);
   fChain->SetBranchAddress("L2_2mu4_DiMu_noVtx_noOS", &L2_2mu4_DiMu_noVtx_noOS, &b_L2_2mu4_DiMu_noVtx_noOS);
   fChain->SetBranchAddress("L2_2mu4_DiMu_noVtx_noOS_passL2", &L2_2mu4_DiMu_noVtx_noOS_passL2, &b_L2_2mu4_DiMu_noVtx_noOS_passL2);
   fChain->SetBranchAddress("L2_2mu4_MSonly", &L2_2mu4_MSonly, &b_L2_2mu4_MSonly);
   fChain->SetBranchAddress("L2_2mu6", &L2_2mu6, &b_L2_2mu6);
   fChain->SetBranchAddress("L2_2mu6_DiMu", &L2_2mu6_DiMu, &b_L2_2mu6_DiMu);
   fChain->SetBranchAddress("L2_2mu6_DiMu_noVtx_noOS", &L2_2mu6_DiMu_noVtx_noOS, &b_L2_2mu6_DiMu_noVtx_noOS);
   fChain->SetBranchAddress("L2_e10_loose", &L2_e10_loose, &b_L2_e10_loose);
   fChain->SetBranchAddress("L2_e10_loose_IdScan", &L2_e10_loose_IdScan, &b_L2_e10_loose_IdScan);
   fChain->SetBranchAddress("L2_e10_loose_TRT", &L2_e10_loose_TRT, &b_L2_e10_loose_TRT);
   fChain->SetBranchAddress("L2_e10_loose_passEF", &L2_e10_loose_passEF, &b_L2_e10_loose_passEF);
   fChain->SetBranchAddress("L2_e10_loose_passL2", &L2_e10_loose_passL2, &b_L2_e10_loose_passL2);
   fChain->SetBranchAddress("L2_e10_medium", &L2_e10_medium, &b_L2_e10_medium);
   fChain->SetBranchAddress("L2_e10_medium_IdScan", &L2_e10_medium_IdScan, &b_L2_e10_medium_IdScan);
   fChain->SetBranchAddress("L2_e10_medium_TRT", &L2_e10_medium_TRT, &b_L2_e10_medium_TRT);
   fChain->SetBranchAddress("L2_e10i_loose", &L2_e10i_loose, &b_L2_e10i_loose);
   fChain->SetBranchAddress("L2_e15_loose", &L2_e15_loose, &b_L2_e15_loose);
   fChain->SetBranchAddress("L2_e15i_loose", &L2_e15i_loose, &b_L2_e15i_loose);
   fChain->SetBranchAddress("L2_e20_loose_NoIDTrkCut", &L2_e20_loose_NoIDTrkCut, &b_L2_e20_loose_NoIDTrkCut);
   fChain->SetBranchAddress("L2_e3_NoCut", &L2_e3_NoCut, &b_L2_e3_NoCut);
   fChain->SetBranchAddress("L2_e3_NoCut_IdScan", &L2_e3_NoCut_IdScan, &b_L2_e3_NoCut_IdScan);
   fChain->SetBranchAddress("L2_e3_NoCut_TRT", &L2_e3_NoCut_TRT, &b_L2_e3_NoCut_TRT);
   fChain->SetBranchAddress("L2_e3_loose", &L2_e3_loose, &b_L2_e3_loose);
   fChain->SetBranchAddress("L2_e3_loose_IdScan", &L2_e3_loose_IdScan, &b_L2_e3_loose_IdScan);
   fChain->SetBranchAddress("L2_e3_loose_TRT", &L2_e3_loose_TRT, &b_L2_e3_loose_TRT);
   fChain->SetBranchAddress("L2_e3_medium", &L2_e3_medium, &b_L2_e3_medium);
   fChain->SetBranchAddress("L2_e3_medium_IdScan", &L2_e3_medium_IdScan, &b_L2_e3_medium_IdScan);
   fChain->SetBranchAddress("L2_e3_medium_TRT", &L2_e3_medium_TRT, &b_L2_e3_medium_TRT);
   fChain->SetBranchAddress("L2_e5_EFfullcalo", &L2_e5_EFfullcalo, &b_L2_e5_EFfullcalo);
   fChain->SetBranchAddress("L2_e5_NoCut", &L2_e5_NoCut, &b_L2_e5_NoCut);
   fChain->SetBranchAddress("L2_e5_NoCut_FwdBackTrk", &L2_e5_NoCut_FwdBackTrk, &b_L2_e5_NoCut_FwdBackTrk);
   fChain->SetBranchAddress("L2_e5_NoCut_IdScan", &L2_e5_NoCut_IdScan, &b_L2_e5_NoCut_IdScan);
   fChain->SetBranchAddress("L2_e5_NoCut_L2SW", &L2_e5_NoCut_L2SW, &b_L2_e5_NoCut_L2SW);
   fChain->SetBranchAddress("L2_e5_NoCut_Ringer", &L2_e5_NoCut_Ringer, &b_L2_e5_NoCut_Ringer);
   fChain->SetBranchAddress("L2_e5_NoCut_SiTrk", &L2_e5_NoCut_SiTrk, &b_L2_e5_NoCut_SiTrk);
   fChain->SetBranchAddress("L2_e5_NoCut_TRT", &L2_e5_NoCut_TRT, &b_L2_e5_NoCut_TRT);
   fChain->SetBranchAddress("L2_e5_NoCut_cells", &L2_e5_NoCut_cells, &b_L2_e5_NoCut_cells);
   fChain->SetBranchAddress("L2_e5_NoCut_cosmic", &L2_e5_NoCut_cosmic, &b_L2_e5_NoCut_cosmic);
   fChain->SetBranchAddress("L2_e5_NoCut_firstempty", &L2_e5_NoCut_firstempty, &b_L2_e5_NoCut_firstempty);
   fChain->SetBranchAddress("L2_e5_NoCut_unpaired", &L2_e5_NoCut_unpaired, &b_L2_e5_NoCut_unpaired);
   fChain->SetBranchAddress("L2_e5_loose", &L2_e5_loose, &b_L2_e5_loose);
   fChain->SetBranchAddress("L2_e5_loose_IdScan", &L2_e5_loose_IdScan, &b_L2_e5_loose_IdScan);
   fChain->SetBranchAddress("L2_e5_loose_NoIDTrkCut", &L2_e5_loose_NoIDTrkCut, &b_L2_e5_loose_NoIDTrkCut);
   fChain->SetBranchAddress("L2_e5_loose_TRT", &L2_e5_loose_TRT, &b_L2_e5_loose_TRT);
   fChain->SetBranchAddress("L2_e5_medium", &L2_e5_medium, &b_L2_e5_medium);
   fChain->SetBranchAddress("L2_e5_medium_IdScan", &L2_e5_medium_IdScan, &b_L2_e5_medium_IdScan);
   fChain->SetBranchAddress("L2_e5_medium_MV", &L2_e5_medium_MV, &b_L2_e5_medium_MV);
   fChain->SetBranchAddress("L2_e5_medium_TRT", &L2_e5_medium_TRT, &b_L2_e5_medium_TRT);
   fChain->SetBranchAddress("L2_e6_loose", &L2_e6_loose, &b_L2_e6_loose);
   fChain->SetBranchAddress("L2_g10_loose", &L2_g10_loose, &b_L2_g10_loose);
   fChain->SetBranchAddress("L2_g10i_loose", &L2_g10i_loose, &b_L2_g10i_loose);
   fChain->SetBranchAddress("L2_g15_loose", &L2_g15_loose, &b_L2_g15_loose);
   fChain->SetBranchAddress("L2_g15i_loose", &L2_g15i_loose, &b_L2_g15i_loose);
   fChain->SetBranchAddress("L2_g3_loose", &L2_g3_loose, &b_L2_g3_loose);
   fChain->SetBranchAddress("L2_g3_nocut", &L2_g3_nocut, &b_L2_g3_nocut);
   fChain->SetBranchAddress("L2_g3_nocut_unpaired", &L2_g3_nocut_unpaired, &b_L2_g3_nocut_unpaired);
   fChain->SetBranchAddress("L2_g5_EFfullcalo", &L2_g5_EFfullcalo, &b_L2_g5_EFfullcalo);
   fChain->SetBranchAddress("L2_g5_loose", &L2_g5_loose, &b_L2_g5_loose);
   fChain->SetBranchAddress("L2_g5_nocut", &L2_g5_nocut, &b_L2_g5_nocut);
   fChain->SetBranchAddress("L2_g5_nocut_cosmic", &L2_g5_nocut_cosmic, &b_L2_g5_nocut_cosmic);
   fChain->SetBranchAddress("L2_g5_nocut_firstempty", &L2_g5_nocut_firstempty, &b_L2_g5_nocut_firstempty);
   fChain->SetBranchAddress("L2_g5_nocut_unpaired", &L2_g5_nocut_unpaired, &b_L2_g5_nocut_unpaired);
   fChain->SetBranchAddress("L2_g6_loose", &L2_g6_loose, &b_L2_g6_loose);
   fChain->SetBranchAddress("L2_mu0_missingRoi", &L2_mu0_missingRoi, &b_L2_mu0_missingRoi);
   fChain->SetBranchAddress("L2_mu0_rpcOnly", &L2_mu0_rpcOnly, &b_L2_mu0_rpcOnly);
   fChain->SetBranchAddress("L2_mu10", &L2_mu10, &b_L2_mu10);
   fChain->SetBranchAddress("L2_mu10_L2MSonly_EFFS_passL2", &L2_mu10_L2MSonly_EFFS_passL2, &b_L2_mu10_L2MSonly_EFFS_passL2);
   fChain->SetBranchAddress("L2_mu10_MG", &L2_mu10_MG, &b_L2_mu10_MG);
   fChain->SetBranchAddress("L2_mu10_MSonly", &L2_mu10_MSonly, &b_L2_mu10_MSonly);
   fChain->SetBranchAddress("L2_mu10_MSonly_EFFS_passL2", &L2_mu10_MSonly_EFFS_passL2, &b_L2_mu10_MSonly_EFFS_passL2);
   fChain->SetBranchAddress("L2_mu10_MSonly_cosmic", &L2_mu10_MSonly_cosmic, &b_L2_mu10_MSonly_cosmic);
   fChain->SetBranchAddress("L2_mu10_NoIDTrkCut", &L2_mu10_NoIDTrkCut, &b_L2_mu10_NoIDTrkCut);
   fChain->SetBranchAddress("L2_mu10_SiTrk", &L2_mu10_SiTrk, &b_L2_mu10_SiTrk);
   fChain->SetBranchAddress("L2_mu10_cosmic", &L2_mu10_cosmic, &b_L2_mu10_cosmic);
   fChain->SetBranchAddress("L2_mu10_muCombTag", &L2_mu10_muCombTag, &b_L2_mu10_muCombTag);
   fChain->SetBranchAddress("L2_mu10_muCombTag_SiTrk", &L2_mu10_muCombTag_SiTrk, &b_L2_mu10_muCombTag_SiTrk);
   fChain->SetBranchAddress("L2_mu10_passHLT", &L2_mu10_passHLT, &b_L2_mu10_passHLT);
   fChain->SetBranchAddress("L2_mu10i_loose", &L2_mu10i_loose, &b_L2_mu10i_loose);
   fChain->SetBranchAddress("L2_mu13", &L2_mu13, &b_L2_mu13);
   fChain->SetBranchAddress("L2_mu15", &L2_mu15, &b_L2_mu15);
   fChain->SetBranchAddress("L2_mu20", &L2_mu20, &b_L2_mu20);
   fChain->SetBranchAddress("L2_mu20_MSonly", &L2_mu20_MSonly, &b_L2_mu20_MSonly);
   fChain->SetBranchAddress("L2_mu20_passHLT", &L2_mu20_passHLT, &b_L2_mu20_passHLT);
   fChain->SetBranchAddress("L2_mu4", &L2_mu4, &b_L2_mu4);
   fChain->SetBranchAddress("L2_mu4_DiMu", &L2_mu4_DiMu, &b_L2_mu4_DiMu);
   fChain->SetBranchAddress("L2_mu4_DiMu_FS", &L2_mu4_DiMu_FS, &b_L2_mu4_DiMu_FS);
   fChain->SetBranchAddress("L2_mu4_DiMu_FS_noOS", &L2_mu4_DiMu_FS_noOS, &b_L2_mu4_DiMu_FS_noOS);
   fChain->SetBranchAddress("L2_mu4_Jpsie5e3", &L2_mu4_Jpsie5e3, &b_L2_mu4_Jpsie5e3);
   fChain->SetBranchAddress("L2_mu4_Jpsie5e3_FS", &L2_mu4_Jpsie5e3_FS, &b_L2_mu4_Jpsie5e3_FS);
   fChain->SetBranchAddress("L2_mu4_Jpsie5e3_SiTrk", &L2_mu4_Jpsie5e3_SiTrk, &b_L2_mu4_Jpsie5e3_SiTrk);
   fChain->SetBranchAddress("L2_mu4_Jpsie5e3_SiTrk_FS", &L2_mu4_Jpsie5e3_SiTrk_FS, &b_L2_mu4_Jpsie5e3_SiTrk_FS);
   fChain->SetBranchAddress("L2_mu4_L2MSonly_EFFS_passL2", &L2_mu4_L2MSonly_EFFS_passL2, &b_L2_mu4_L2MSonly_EFFS_passL2);
   fChain->SetBranchAddress("L2_mu4_MG", &L2_mu4_MG, &b_L2_mu4_MG);
   fChain->SetBranchAddress("L2_mu4_MSonly", &L2_mu4_MSonly, &b_L2_mu4_MSonly);
   fChain->SetBranchAddress("L2_mu4_MSonly_EFFS_passL2", &L2_mu4_MSonly_EFFS_passL2, &b_L2_mu4_MSonly_EFFS_passL2);
   fChain->SetBranchAddress("L2_mu4_MSonly_MB2_noL2_EFFS", &L2_mu4_MSonly_MB2_noL2_EFFS, &b_L2_mu4_MSonly_MB2_noL2_EFFS);
   fChain->SetBranchAddress("L2_mu4_MSonly_Trk_Jpsi_loose", &L2_mu4_MSonly_Trk_Jpsi_loose, &b_L2_mu4_MSonly_Trk_Jpsi_loose);
   fChain->SetBranchAddress("L2_mu4_MSonly_Trk_Upsi_loose_FS", &L2_mu4_MSonly_Trk_Upsi_loose_FS, &b_L2_mu4_MSonly_Trk_Upsi_loose_FS);
   fChain->SetBranchAddress("L2_mu4_MSonly_cosmic", &L2_mu4_MSonly_cosmic, &b_L2_mu4_MSonly_cosmic);
   fChain->SetBranchAddress("L2_mu4_MSonly_firstempty", &L2_mu4_MSonly_firstempty, &b_L2_mu4_MSonly_firstempty);
   fChain->SetBranchAddress("L2_mu4_MV", &L2_mu4_MV, &b_L2_mu4_MV);
   fChain->SetBranchAddress("L2_mu4_NoIDTrkCut", &L2_mu4_NoIDTrkCut, &b_L2_mu4_NoIDTrkCut);
   fChain->SetBranchAddress("L2_mu4_SiTrk", &L2_mu4_SiTrk, &b_L2_mu4_SiTrk);
   fChain->SetBranchAddress("L2_mu4_Trk_Jpsi_loose", &L2_mu4_Trk_Jpsi_loose, &b_L2_mu4_Trk_Jpsi_loose);
   fChain->SetBranchAddress("L2_mu4_Trk_Upsi_loose_FS", &L2_mu4_Trk_Upsi_loose_FS, &b_L2_mu4_Trk_Upsi_loose_FS);
   fChain->SetBranchAddress("L2_mu4_comm", &L2_mu4_comm, &b_L2_mu4_comm);
   fChain->SetBranchAddress("L2_mu4_comm_MSonly", &L2_mu4_comm_MSonly, &b_L2_mu4_comm_MSonly);
   fChain->SetBranchAddress("L2_mu4_comm_MSonly_cosmic", &L2_mu4_comm_MSonly_cosmic, &b_L2_mu4_comm_MSonly_cosmic);
   fChain->SetBranchAddress("L2_mu4_comm_MSonly_firstempty", &L2_mu4_comm_MSonly_firstempty, &b_L2_mu4_comm_MSonly_firstempty);
   fChain->SetBranchAddress("L2_mu4_comm_cosmic", &L2_mu4_comm_cosmic, &b_L2_mu4_comm_cosmic);
   fChain->SetBranchAddress("L2_mu4_comm_firstempty", &L2_mu4_comm_firstempty, &b_L2_mu4_comm_firstempty);
   fChain->SetBranchAddress("L2_mu4_cosmic", &L2_mu4_cosmic, &b_L2_mu4_cosmic);
   fChain->SetBranchAddress("L2_mu4_firstempty", &L2_mu4_firstempty, &b_L2_mu4_firstempty);
   fChain->SetBranchAddress("L2_mu4_j10_matched", &L2_mu4_j10_matched, &b_L2_mu4_j10_matched);
   fChain->SetBranchAddress("L2_mu4_j20_matched", &L2_mu4_j20_matched, &b_L2_mu4_j20_matched);
   fChain->SetBranchAddress("L2_mu4_j5_matched", &L2_mu4_j5_matched, &b_L2_mu4_j5_matched);
   fChain->SetBranchAddress("L2_mu4_mu6", &L2_mu4_mu6, &b_L2_mu4_mu6);
   fChain->SetBranchAddress("L2_mu4_muCombTag", &L2_mu4_muCombTag, &b_L2_mu4_muCombTag);
   fChain->SetBranchAddress("L2_mu4_muCombTag_SiTrk", &L2_mu4_muCombTag_SiTrk, &b_L2_mu4_muCombTag_SiTrk);
   fChain->SetBranchAddress("L2_mu4_passHLT", &L2_mu4_passHLT, &b_L2_mu4_passHLT);
   fChain->SetBranchAddress("L2_mu4_tile", &L2_mu4_tile, &b_L2_mu4_tile);
   fChain->SetBranchAddress("L2_mu4_tile_SiTrk", &L2_mu4_tile_SiTrk, &b_L2_mu4_tile_SiTrk);
   fChain->SetBranchAddress("L2_mu4_trod", &L2_mu4_trod, &b_L2_mu4_trod);
   fChain->SetBranchAddress("L2_mu4_trod_SiTrk", &L2_mu4_trod_SiTrk, &b_L2_mu4_trod_SiTrk);
   fChain->SetBranchAddress("L2_mu4_unpaired", &L2_mu4_unpaired, &b_L2_mu4_unpaired);
   fChain->SetBranchAddress("L2_mu4mu6_DiMu", &L2_mu4mu6_DiMu, &b_L2_mu4mu6_DiMu);
   fChain->SetBranchAddress("L2_mu6", &L2_mu6, &b_L2_mu6);
   fChain->SetBranchAddress("L2_mu6_DiMu", &L2_mu6_DiMu, &b_L2_mu6_DiMu);
   fChain->SetBranchAddress("L2_mu6_DiMu_FS", &L2_mu6_DiMu_FS, &b_L2_mu6_DiMu_FS);
   fChain->SetBranchAddress("L2_mu6_DiMu_FS_noOS", &L2_mu6_DiMu_FS_noOS, &b_L2_mu6_DiMu_FS_noOS);
   fChain->SetBranchAddress("L2_mu6_DsPhiPi", &L2_mu6_DsPhiPi, &b_L2_mu6_DsPhiPi);
   fChain->SetBranchAddress("L2_mu6_Jpsie5e3", &L2_mu6_Jpsie5e3, &b_L2_mu6_Jpsie5e3);
   fChain->SetBranchAddress("L2_mu6_Jpsie5e3_FS", &L2_mu6_Jpsie5e3_FS, &b_L2_mu6_Jpsie5e3_FS);
   fChain->SetBranchAddress("L2_mu6_Jpsie5e3_SiTrk", &L2_mu6_Jpsie5e3_SiTrk, &b_L2_mu6_Jpsie5e3_SiTrk);
   fChain->SetBranchAddress("L2_mu6_Jpsie5e3_SiTrk_FS", &L2_mu6_Jpsie5e3_SiTrk_FS, &b_L2_mu6_Jpsie5e3_SiTrk_FS);
   fChain->SetBranchAddress("L2_mu6_L2MSonly_EFFS_passL2", &L2_mu6_L2MSonly_EFFS_passL2, &b_L2_mu6_L2MSonly_EFFS_passL2);
   fChain->SetBranchAddress("L2_mu6_MG", &L2_mu6_MG, &b_L2_mu6_MG);
   fChain->SetBranchAddress("L2_mu6_MSonly", &L2_mu6_MSonly, &b_L2_mu6_MSonly);
   fChain->SetBranchAddress("L2_mu6_MSonly_EFFS_passL2", &L2_mu6_MSonly_EFFS_passL2, &b_L2_mu6_MSonly_EFFS_passL2);
   fChain->SetBranchAddress("L2_mu6_MSonly_cosmic", &L2_mu6_MSonly_cosmic, &b_L2_mu6_MSonly_cosmic);
   fChain->SetBranchAddress("L2_mu6_MSonly_firstempty", &L2_mu6_MSonly_firstempty, &b_L2_mu6_MSonly_firstempty);
   fChain->SetBranchAddress("L2_mu6_MSonly_unpaired", &L2_mu6_MSonly_unpaired, &b_L2_mu6_MSonly_unpaired);
   fChain->SetBranchAddress("L2_mu6_SiTrk", &L2_mu6_SiTrk, &b_L2_mu6_SiTrk);
   fChain->SetBranchAddress("L2_mu6_Trk_Jpsi_loose", &L2_mu6_Trk_Jpsi_loose, &b_L2_mu6_Trk_Jpsi_loose);
   fChain->SetBranchAddress("L2_mu6_Trk_Jpsi_loose_FS", &L2_mu6_Trk_Jpsi_loose_FS, &b_L2_mu6_Trk_Jpsi_loose_FS);
   fChain->SetBranchAddress("L2_mu6_Trk_Upsi_loose_FS", &L2_mu6_Trk_Upsi_loose_FS, &b_L2_mu6_Trk_Upsi_loose_FS);
   fChain->SetBranchAddress("L2_mu6_cosmic", &L2_mu6_cosmic, &b_L2_mu6_cosmic);
   fChain->SetBranchAddress("L2_mu6_firstempty", &L2_mu6_firstempty, &b_L2_mu6_firstempty);
   fChain->SetBranchAddress("L2_mu6_muCombTag", &L2_mu6_muCombTag, &b_L2_mu6_muCombTag);
   fChain->SetBranchAddress("L2_mu6_muCombTag_SiTrk", &L2_mu6_muCombTag_SiTrk, &b_L2_mu6_muCombTag_SiTrk);
   fChain->SetBranchAddress("L2_mu6_passHLT", &L2_mu6_passHLT, &b_L2_mu6_passHLT);
   fChain->SetBranchAddress("L2_mu6_unpaired", &L2_mu6_unpaired, &b_L2_mu6_unpaired);
   fChain->SetBranchAddress("L2_tau12_loose_xe20", &L2_tau12_loose_xe20, &b_L2_tau12_loose_xe20);
   fChain->SetBranchAddress("L2_tau16_loose_xe20", &L2_tau16_loose_xe20, &b_L2_tau16_loose_xe20);
   fChain->SetBranchAddress("L2_tauNoCut_hasTrk_xe20", &L2_tauNoCut_hasTrk_xe20, &b_L2_tauNoCut_hasTrk_xe20);
   fChain->SetBranchAddress("L2_xe12", &L2_xe12, &b_L2_xe12);
   fChain->SetBranchAddress("L2_xe12_FEB", &L2_xe12_FEB, &b_L2_xe12_FEB);
   fChain->SetBranchAddress("L2_xe12_allCells", &L2_xe12_allCells, &b_L2_xe12_allCells);
   fChain->SetBranchAddress("L2_xe12_noMu", &L2_xe12_noMu, &b_L2_xe12_noMu);
   fChain->SetBranchAddress("L2_xe15_noMu", &L2_xe15_noMu, &b_L2_xe15_noMu);
   fChain->SetBranchAddress("L2_xe20_noMu", &L2_xe20_noMu, &b_L2_xe20_noMu);
   fChain->SetBranchAddress("L2_xe25_noMu", &L2_xe25_noMu, &b_L2_xe25_noMu);
   fChain->SetBranchAddress("L2_xe30_allL1", &L2_xe30_allL1, &b_L2_xe30_allL1);
   fChain->SetBranchAddress("L2_xe30_allL1_FEB", &L2_xe30_allL1_FEB, &b_L2_xe30_allL1_FEB);
   fChain->SetBranchAddress("L2_xe30_allL1_allCells", &L2_xe30_allL1_allCells, &b_L2_xe30_allL1_allCells);
   fChain->SetBranchAddress("L2_xe30_noMu", &L2_xe30_noMu, &b_L2_xe30_noMu);
   fChain->SetBranchAddress("L2_xe30_unbiased", &L2_xe30_unbiased, &b_L2_xe30_unbiased);
   fChain->SetBranchAddress("L2_xe30_unbiased_FEB", &L2_xe30_unbiased_FEB, &b_L2_xe30_unbiased_FEB);
   fChain->SetBranchAddress("L2_xe30_unbiased_allCells", &L2_xe30_unbiased_allCells, &b_L2_xe30_unbiased_allCells);
   fChain->SetBranchAddress("L2_xe35_noMu", &L2_xe35_noMu, &b_L2_xe35_noMu);
   fChain->SetBranchAddress("L2_xe40_medium_noMu", &L2_xe40_medium_noMu, &b_L2_xe40_medium_noMu);
   fChain->SetBranchAddress("L2_xe50_medium_noMu", &L2_xe50_medium_noMu, &b_L2_xe50_medium_noMu);
   fChain->SetBranchAddress("L2_xe60_medium_noMu", &L2_xe60_medium_noMu, &b_L2_xe60_medium_noMu);
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
   fChain->SetBranchAddress("ph_isIso", &ph_isIso, &b_ph_isIso);
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
   fChain->SetBranchAddress("ph_trackIsol", &ph_trackIsol, &b_ph_trackIsol);
   fChain->SetBranchAddress("ph_convIP", &ph_convIP, &b_ph_convIP);
   fChain->SetBranchAddress("ph_convIPRev", &ph_convIPRev, &b_ph_convIPRev);
   fChain->SetBranchAddress("ph_HPV_zvertex", &ph_HPV_zvertex, &b_ph_HPV_zvertex);
   fChain->SetBranchAddress("ph_HPV_sigma_zvertex", &ph_HPV_sigma_zvertex, &b_ph_HPV_sigma_zvertex);
   fChain->SetBranchAddress("ph_HPV_zvertex_charged", &ph_HPV_zvertex_charged, &b_ph_HPV_zvertex_charged);
   fChain->SetBranchAddress("ph_HPV_sigma_zvertex_charged", &ph_HPV_sigma_zvertex_charged, &b_ph_HPV_sigma_zvertex_charged);
   fChain->SetBranchAddress("ph_HPV_zvertex_neutral", &ph_HPV_zvertex_neutral, &b_ph_HPV_zvertex_neutral);
   fChain->SetBranchAddress("ph_HPV_sigma_zvertex_neutral", &ph_HPV_sigma_zvertex_neutral, &b_ph_HPV_sigma_zvertex_neutral);
   fChain->SetBranchAddress("ph_HPV_eta", &ph_HPV_eta, &b_ph_HPV_eta);
   fChain->SetBranchAddress("ph_HPV_sigma_eta", &ph_HPV_sigma_eta, &b_ph_HPV_sigma_eta);
   fChain->SetBranchAddress("ph_HPV_convTrk1_zvertex", &ph_HPV_convTrk1_zvertex, &b_ph_HPV_convTrk1_zvertex);
   fChain->SetBranchAddress("ph_HPV_convTrk1_sigma_zvertex", &ph_HPV_convTrk1_sigma_zvertex, &b_ph_HPV_convTrk1_sigma_zvertex);
   fChain->SetBranchAddress("ph_HPV_convTrk2_zvertex", &ph_HPV_convTrk2_zvertex, &b_ph_HPV_convTrk2_zvertex);
   fChain->SetBranchAddress("ph_HPV_convTrk2_sigma_zvertex", &ph_HPV_convTrk2_sigma_zvertex, &b_ph_HPV_convTrk2_sigma_zvertex);
   fChain->SetBranchAddress("ph_ptIsolationCone", &ph_ptIsolationCone, &b_ph_ptIsolationCone);
   fChain->SetBranchAddress("ph_ptIsolationConePhAngle", &ph_ptIsolationConePhAngle, &b_ph_ptIsolationConePhAngle);
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
   fChain->SetBranchAddress("ph_EF_E", &ph_EF_E, &b_ph_EF_E);
   fChain->SetBranchAddress("ph_EF_Et", &ph_EF_Et, &b_ph_EF_Et);
   fChain->SetBranchAddress("ph_EF_pt", &ph_EF_pt, &b_ph_EF_pt);
   fChain->SetBranchAddress("ph_EF_eta", &ph_EF_eta, &b_ph_EF_eta);
   fChain->SetBranchAddress("ph_EF_phi", &ph_EF_phi, &b_ph_EF_phi);
   fChain->SetBranchAddress("ph_EF_Ethad", &ph_EF_Ethad, &b_ph_EF_Ethad);
   fChain->SetBranchAddress("ph_EF_Ethad1", &ph_EF_Ethad1, &b_ph_EF_Ethad1);
   fChain->SetBranchAddress("ph_EF_f1", &ph_EF_f1, &b_ph_EF_f1);
   fChain->SetBranchAddress("ph_EF_Emins1", &ph_EF_Emins1, &b_ph_EF_Emins1);
   fChain->SetBranchAddress("ph_EF_fside", &ph_EF_fside, &b_ph_EF_fside);
   fChain->SetBranchAddress("ph_EF_Emax2", &ph_EF_Emax2, &b_ph_EF_Emax2);
   fChain->SetBranchAddress("ph_EF_ws3", &ph_EF_ws3, &b_ph_EF_ws3);
   fChain->SetBranchAddress("ph_EF_wstot", &ph_EF_wstot, &b_ph_EF_wstot);
   fChain->SetBranchAddress("ph_EF_E233", &ph_EF_E233, &b_ph_EF_E233);
   fChain->SetBranchAddress("ph_EF_E237", &ph_EF_E237, &b_ph_EF_E237);
   fChain->SetBranchAddress("ph_EF_E277", &ph_EF_E277, &b_ph_EF_E277);
   fChain->SetBranchAddress("ph_EF_weta2", &ph_EF_weta2, &b_ph_EF_weta2);
   fChain->SetBranchAddress("ph_EF_rphiallcalo", &ph_EF_rphiallcalo, &b_ph_EF_rphiallcalo);
   fChain->SetBranchAddress("ph_EF_Etcone45", &ph_EF_Etcone45, &b_ph_EF_Etcone45);
   fChain->SetBranchAddress("ph_EF_Etcone20", &ph_EF_Etcone20, &b_ph_EF_Etcone20);
   fChain->SetBranchAddress("ph_EF_Etcone30", &ph_EF_Etcone30, &b_ph_EF_Etcone30);
   fChain->SetBranchAddress("ph_EF_Etcone40", &ph_EF_Etcone40, &b_ph_EF_Etcone40);
   fChain->SetBranchAddress("ph_EF_zvertex", &ph_EF_zvertex, &b_ph_EF_zvertex);
   fChain->SetBranchAddress("ph_EF_errz", &ph_EF_errz, &b_ph_EF_errz);
   fChain->SetBranchAddress("ph_EF_matched", &ph_EF_matched, &b_ph_EF_matched);
   fChain->SetBranchAddress("ph_L2_dr", &ph_L2_dr, &b_ph_L2_dr);
   fChain->SetBranchAddress("ph_L2_E", &ph_L2_E, &b_ph_L2_E);
   fChain->SetBranchAddress("ph_L2_Et", &ph_L2_Et, &b_ph_L2_Et);
   fChain->SetBranchAddress("ph_L2_pt", &ph_L2_pt, &b_ph_L2_pt);
   fChain->SetBranchAddress("ph_L2_eta", &ph_L2_eta, &b_ph_L2_eta);
   fChain->SetBranchAddress("ph_L2_phi", &ph_L2_phi, &b_ph_L2_phi);
   fChain->SetBranchAddress("ph_L2_Ethad1", &ph_L2_Ethad1, &b_ph_L2_Ethad1);
   fChain->SetBranchAddress("ph_L2_reta", &ph_L2_reta, &b_ph_L2_reta);
   fChain->SetBranchAddress("ph_L2_Eratio", &ph_L2_Eratio, &b_ph_L2_Eratio);
   fChain->SetBranchAddress("ph_L2_E237", &ph_L2_E237, &b_ph_L2_E237);
   fChain->SetBranchAddress("ph_L2_E277", &ph_L2_E277, &b_ph_L2_E277);
   fChain->SetBranchAddress("ph_L2_fside", &ph_L2_fside, &b_ph_L2_fside);
   fChain->SetBranchAddress("ph_L2_weta2", &ph_L2_weta2, &b_ph_L2_weta2);
   fChain->SetBranchAddress("ph_L2_Emaxs1", &ph_L2_Emaxs1, &b_ph_L2_Emaxs1);
   fChain->SetBranchAddress("ph_L2_Emax2", &ph_L2_Emax2, &b_ph_L2_Emax2);
   fChain->SetBranchAddress("ph_L2_matched", &ph_L2_matched, &b_ph_L2_matched);
   fChain->SetBranchAddress("ph_L1_dr", &ph_L1_dr, &b_ph_L1_dr);
   fChain->SetBranchAddress("ph_L1_eta", &ph_L1_eta, &b_ph_L1_eta);
   fChain->SetBranchAddress("ph_L1_phi", &ph_L1_phi, &b_ph_L1_phi);
   fChain->SetBranchAddress("ph_L1_EMisol", &ph_L1_EMisol, &b_ph_L1_EMisol);
   fChain->SetBranchAddress("ph_L1_hadisol", &ph_L1_hadisol, &b_ph_L1_hadisol);
   fChain->SetBranchAddress("ph_L1_hadcore", &ph_L1_hadcore, &b_ph_L1_hadcore);
   fChain->SetBranchAddress("ph_L1_pt", &ph_L1_pt, &b_ph_L1_pt);
   fChain->SetBranchAddress("ph_L1_Et", &ph_L1_Et, &b_ph_L1_Et);
   fChain->SetBranchAddress("ph_L1_matched", &ph_L1_matched, &b_ph_L1_matched);
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
   fChain->SetBranchAddress("mu_bestMatch", &mu_bestMatch, &b_mu_bestMatch);
   fChain->SetBranchAddress("mu_isStandAloneMuon", &mu_isStandAloneMuon, &b_mu_isStandAloneMuon);
   fChain->SetBranchAddress("mu_isCombinedMuon", &mu_isCombinedMuon, &b_mu_isCombinedMuon);
   fChain->SetBranchAddress("mu_isLowPtReconstructedMuon", &mu_isLowPtReconstructedMuon, &b_mu_isLowPtReconstructedMuon);
   fChain->SetBranchAddress("mu_d0_exPV", &mu_d0_exPV, &b_mu_d0_exPV);
   fChain->SetBranchAddress("mu_z0_exPV", &mu_z0_exPV, &b_mu_z0_exPV);
   fChain->SetBranchAddress("mu_phi_exPV", &mu_phi_exPV, &b_mu_phi_exPV);
   fChain->SetBranchAddress("mu_theta_exPV", &mu_theta_exPV, &b_mu_theta_exPV);
   fChain->SetBranchAddress("mu_qoverp_exPV", &mu_qoverp_exPV, &b_mu_qoverp_exPV);
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
   fChain->SetBranchAddress("mu_msextrap_d0", &mu_msextrap_d0, &b_mu_msextrap_d0);
   fChain->SetBranchAddress("mu_msextrap_z0", &mu_msextrap_z0, &b_mu_msextrap_z0);
   fChain->SetBranchAddress("mu_msextrap_phi", &mu_msextrap_phi, &b_mu_msextrap_phi);
   fChain->SetBranchAddress("mu_msextrap_theta", &mu_msextrap_theta, &b_mu_msextrap_theta);
   fChain->SetBranchAddress("mu_msextrap_qoverp", &mu_msextrap_qoverp, &b_mu_msextrap_qoverp);
   fChain->SetBranchAddress("mu_idextrap_d0", &mu_idextrap_d0, &b_mu_idextrap_d0);
   fChain->SetBranchAddress("mu_idextrap_z0", &mu_idextrap_z0, &b_mu_idextrap_z0);
   fChain->SetBranchAddress("mu_idextrap_phi", &mu_idextrap_phi, &b_mu_idextrap_phi);
   fChain->SetBranchAddress("mu_idextrap_theta", &mu_idextrap_theta, &b_mu_idextrap_theta);
   fChain->SetBranchAddress("mu_idextrap_qoverp", &mu_idextrap_qoverp, &b_mu_idextrap_qoverp);
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
   fChain->SetBranchAddress("mu_seg_n", &mu_seg_n, &b_mu_seg_n);
   fChain->SetBranchAddress("mu_seg_x", &mu_seg_x, &b_mu_seg_x);
   fChain->SetBranchAddress("mu_seg_y", &mu_seg_y, &b_mu_seg_y);
   fChain->SetBranchAddress("mu_seg_z", &mu_seg_z, &b_mu_seg_z);
   fChain->SetBranchAddress("mu_seg_phi", &mu_seg_phi, &b_mu_seg_phi);
   fChain->SetBranchAddress("mu_seg_theta", &mu_seg_theta, &b_mu_seg_theta);
   fChain->SetBranchAddress("mu_seg_locX", &mu_seg_locX, &b_mu_seg_locX);
   fChain->SetBranchAddress("mu_seg_locY", &mu_seg_locY, &b_mu_seg_locY);
   fChain->SetBranchAddress("mu_seg_locAngleXZ", &mu_seg_locAngleXZ, &b_mu_seg_locAngleXZ);
   fChain->SetBranchAddress("mu_seg_locAngleYZ", &mu_seg_locAngleYZ, &b_mu_seg_locAngleYZ);
   fChain->SetBranchAddress("mu_seg_sector", &mu_seg_sector, &b_mu_seg_sector);
   fChain->SetBranchAddress("mu_seg_stationEta", &mu_seg_stationEta, &b_mu_seg_stationEta);
   fChain->SetBranchAddress("mu_seg_isEndcap", &mu_seg_isEndcap, &b_mu_seg_isEndcap);
   fChain->SetBranchAddress("mu_seg_stationName", &mu_seg_stationName, &b_mu_seg_stationName);
   fChain->SetBranchAddress("mu_seg_author", &mu_seg_author, &b_mu_seg_author);
   fChain->SetBranchAddress("mu_seg_chi2", &mu_seg_chi2, &b_mu_seg_chi2);
   fChain->SetBranchAddress("mu_seg_ndof", &mu_seg_ndof, &b_mu_seg_ndof);
   fChain->SetBranchAddress("jet_n", &jet_n, &b_jet_n);
   fChain->SetBranchAddress("jet_E", &jet_E, &b_jet_E);
   fChain->SetBranchAddress("jet_pt", &jet_pt, &b_jet_pt);
   fChain->SetBranchAddress("jet_m", &jet_m, &b_jet_m);
   fChain->SetBranchAddress("jet_eta", &jet_eta, &b_jet_eta);
   fChain->SetBranchAddress("jet_phi", &jet_phi, &b_jet_phi);
   fChain->SetBranchAddress("jet_WIDTH", &jet_WIDTH, &b_jet_WIDTH);
   fChain->SetBranchAddress("jet_n90", &jet_n90, &b_jet_n90);
   fChain->SetBranchAddress("jet_n90constituents", &jet_n90constituents, &b_jet_n90constituents);
   fChain->SetBranchAddress("jet_fracSamplingMax", &jet_fracSamplingMax, &b_jet_fracSamplingMax);
   fChain->SetBranchAddress("jet_SamplingMax", &jet_SamplingMax, &b_jet_SamplingMax);
   fChain->SetBranchAddress("jet_BCH_CORR_CELL", &jet_BCH_CORR_CELL, &b_jet_BCH_CORR_CELL);
   fChain->SetBranchAddress("jet_BCH_CORR_JET", &jet_BCH_CORR_JET, &b_jet_BCH_CORR_JET);
   fChain->SetBranchAddress("jet_BCH_CORR_JET_FORCELL", &jet_BCH_CORR_JET_FORCELL, &b_jet_BCH_CORR_JET_FORCELL);
   fChain->SetBranchAddress("jet_ENG_BAD_CELLS", &jet_ENG_BAD_CELLS, &b_jet_ENG_BAD_CELLS);
   fChain->SetBranchAddress("jet_N_BAD_CELLS", &jet_N_BAD_CELLS, &b_jet_N_BAD_CELLS);
   fChain->SetBranchAddress("jet_N_BAD_CELLS_CORR", &jet_N_BAD_CELLS_CORR, &b_jet_N_BAD_CELLS_CORR);
   fChain->SetBranchAddress("jet_BAD_CELLS_CORR_E", &jet_BAD_CELLS_CORR_E, &b_jet_BAD_CELLS_CORR_E);
   fChain->SetBranchAddress("jet_timing", &jet_timing, &b_jet_timing);
   fChain->SetBranchAddress("jet_quality", &jet_quality, &b_jet_quality);
   fChain->SetBranchAddress("jet_emfrac", &jet_emfrac, &b_jet_emfrac);
   fChain->SetBranchAddress("jet_GCWJES", &jet_GCWJES, &b_jet_GCWJES);
   fChain->SetBranchAddress("jet_EMJES", &jet_EMJES, &b_jet_EMJES);
   fChain->SetBranchAddress("jet_emscale_E", &jet_emscale_E, &b_jet_emscale_E);
   fChain->SetBranchAddress("jet_emscale_pt", &jet_emscale_pt, &b_jet_emscale_pt);
   fChain->SetBranchAddress("jet_emscale_m", &jet_emscale_m, &b_jet_emscale_m);
   fChain->SetBranchAddress("jet_emscale_eta", &jet_emscale_eta, &b_jet_emscale_eta);
   fChain->SetBranchAddress("jet_emscale_phi", &jet_emscale_phi, &b_jet_emscale_phi);
   fChain->SetBranchAddress("jet_jvtxf", &jet_jvtxf, &b_jet_jvtxf);
   fChain->SetBranchAddress("jet_jvtx_x", &jet_jvtx_x, &b_jet_jvtx_x);
   fChain->SetBranchAddress("jet_jvtx_y", &jet_jvtx_y, &b_jet_jvtx_y);
   fChain->SetBranchAddress("jet_jvtx_z", &jet_jvtx_z, &b_jet_jvtx_z);
   fChain->SetBranchAddress("jet_flavor_weight", &jet_flavor_weight, &b_jet_flavor_weight);
   fChain->SetBranchAddress("jet_el_dr", &jet_el_dr, &b_jet_el_dr);
   fChain->SetBranchAddress("jet_el_matched", &jet_el_matched, &b_jet_el_matched);
   fChain->SetBranchAddress("jet_mu_dr", &jet_mu_dr, &b_jet_mu_dr);
   fChain->SetBranchAddress("jet_mu_matched", &jet_mu_matched, &b_jet_mu_matched);
   fChain->SetBranchAddress("MET_RefFinal_etx", &MET_RefFinal_etx, &b_MET_RefFinal_etx);
   fChain->SetBranchAddress("MET_RefFinal_ety", &MET_RefFinal_ety, &b_MET_RefFinal_ety);
   fChain->SetBranchAddress("MET_RefFinal_sumet", &MET_RefFinal_sumet, &b_MET_RefFinal_sumet);
   fChain->SetBranchAddress("MET_RefFinal_et", &MET_RefFinal_et, &b_MET_RefFinal_et);
   fChain->SetBranchAddress("MET_RefFinal_phi", &MET_RefFinal_phi, &b_MET_RefFinal_phi);
   fChain->SetBranchAddress("MET_MuonBoy_etx", &MET_MuonBoy_etx, &b_MET_MuonBoy_etx);
   fChain->SetBranchAddress("MET_MuonBoy_ety", &MET_MuonBoy_ety, &b_MET_MuonBoy_ety);
   fChain->SetBranchAddress("MET_MuonBoy_sumet", &MET_MuonBoy_sumet, &b_MET_MuonBoy_sumet);
   fChain->SetBranchAddress("MET_MuonBoy_et", &MET_MuonBoy_et, &b_MET_MuonBoy_et);
   fChain->SetBranchAddress("MET_MuonBoy_phi", &MET_MuonBoy_phi, &b_MET_MuonBoy_phi);
   fChain->SetBranchAddress("MET_RefEle_etx", &MET_RefEle_etx, &b_MET_RefEle_etx);
   fChain->SetBranchAddress("MET_RefEle_ety", &MET_RefEle_ety, &b_MET_RefEle_ety);
   fChain->SetBranchAddress("MET_RefEle_sumet", &MET_RefEle_sumet, &b_MET_RefEle_sumet);
   fChain->SetBranchAddress("MET_RefEle_et", &MET_RefEle_et, &b_MET_RefEle_et);
   fChain->SetBranchAddress("MET_RefEle_phi", &MET_RefEle_phi, &b_MET_RefEle_phi);
   fChain->SetBranchAddress("MET_RefJet_etx", &MET_RefJet_etx, &b_MET_RefJet_etx);
   fChain->SetBranchAddress("MET_RefJet_ety", &MET_RefJet_ety, &b_MET_RefJet_ety);
   fChain->SetBranchAddress("MET_RefJet_sumet", &MET_RefJet_sumet, &b_MET_RefJet_sumet);
   fChain->SetBranchAddress("MET_RefJet_et", &MET_RefJet_et, &b_MET_RefJet_et);
   fChain->SetBranchAddress("MET_RefJet_phi", &MET_RefJet_phi, &b_MET_RefJet_phi);
   fChain->SetBranchAddress("MET_RefGamma_etx", &MET_RefGamma_etx, &b_MET_RefGamma_etx);
   fChain->SetBranchAddress("MET_RefGamma_ety", &MET_RefGamma_ety, &b_MET_RefGamma_ety);
   fChain->SetBranchAddress("MET_RefGamma_sumet", &MET_RefGamma_sumet, &b_MET_RefGamma_sumet);
   fChain->SetBranchAddress("MET_RefGamma_et", &MET_RefGamma_et, &b_MET_RefGamma_et);
   fChain->SetBranchAddress("MET_RefGamma_phi", &MET_RefGamma_phi, &b_MET_RefGamma_phi);
   fChain->SetBranchAddress("MET_RefTau_etx", &MET_RefTau_etx, &b_MET_RefTau_etx);
   fChain->SetBranchAddress("MET_RefTau_ety", &MET_RefTau_ety, &b_MET_RefTau_ety);
   fChain->SetBranchAddress("MET_RefTau_sumet", &MET_RefTau_sumet, &b_MET_RefTau_sumet);
   fChain->SetBranchAddress("MET_RefTau_et", &MET_RefTau_et, &b_MET_RefTau_et);
   fChain->SetBranchAddress("MET_RefTau_phi", &MET_RefTau_phi, &b_MET_RefTau_phi);
   fChain->SetBranchAddress("MET_Cryo_etx", &MET_Cryo_etx, &b_MET_Cryo_etx);
   fChain->SetBranchAddress("MET_Cryo_ety", &MET_Cryo_ety, &b_MET_Cryo_ety);
   fChain->SetBranchAddress("MET_Cryo_sumet", &MET_Cryo_sumet, &b_MET_Cryo_sumet);
   fChain->SetBranchAddress("MET_Cryo_et", &MET_Cryo_et, &b_MET_Cryo_et);
   fChain->SetBranchAddress("MET_Cryo_phi", &MET_Cryo_phi, &b_MET_Cryo_phi);
   fChain->SetBranchAddress("MET_CellOut_etx", &MET_CellOut_etx, &b_MET_CellOut_etx);
   fChain->SetBranchAddress("MET_CellOut_ety", &MET_CellOut_ety, &b_MET_CellOut_ety);
   fChain->SetBranchAddress("MET_CellOut_sumet", &MET_CellOut_sumet, &b_MET_CellOut_sumet);
   fChain->SetBranchAddress("MET_CellOut_et", &MET_CellOut_et, &b_MET_CellOut_et);
   fChain->SetBranchAddress("MET_CellOut_phi", &MET_CellOut_phi, &b_MET_CellOut_phi);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_etx", &MET_MuonBoy_Spectro_etx, &b_MET_MuonBoy_Spectro_etx);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_ety", &MET_MuonBoy_Spectro_ety, &b_MET_MuonBoy_Spectro_ety);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_sumet", &MET_MuonBoy_Spectro_sumet, &b_MET_MuonBoy_Spectro_sumet);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_et", &MET_MuonBoy_Spectro_et, &b_MET_MuonBoy_Spectro_et);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_phi", &MET_MuonBoy_Spectro_phi, &b_MET_MuonBoy_Spectro_phi);
   fChain->SetBranchAddress("MET_MuonBoy_Track_etx", &MET_MuonBoy_Track_etx, &b_MET_MuonBoy_Track_etx);
   fChain->SetBranchAddress("MET_MuonBoy_Track_ety", &MET_MuonBoy_Track_ety, &b_MET_MuonBoy_Track_ety);
   fChain->SetBranchAddress("MET_MuonBoy_Track_sumet", &MET_MuonBoy_Track_sumet, &b_MET_MuonBoy_Track_sumet);
   fChain->SetBranchAddress("MET_MuonBoy_Track_et", &MET_MuonBoy_Track_et, &b_MET_MuonBoy_Track_et);
   fChain->SetBranchAddress("MET_MuonBoy_Track_phi", &MET_MuonBoy_Track_phi, &b_MET_MuonBoy_Track_phi);
   fChain->SetBranchAddress("MET_CryoCone_etx", &MET_CryoCone_etx, &b_MET_CryoCone_etx);
   fChain->SetBranchAddress("MET_CryoCone_ety", &MET_CryoCone_ety, &b_MET_CryoCone_ety);
   fChain->SetBranchAddress("MET_CryoCone_sumet", &MET_CryoCone_sumet, &b_MET_CryoCone_sumet);
   fChain->SetBranchAddress("MET_CryoCone_et", &MET_CryoCone_et, &b_MET_CryoCone_et);
   fChain->SetBranchAddress("MET_CryoCone_phi", &MET_CryoCone_phi, &b_MET_CryoCone_phi);
   fChain->SetBranchAddress("MET_Muon_etx", &MET_Muon_etx, &b_MET_Muon_etx);
   fChain->SetBranchAddress("MET_Muon_ety", &MET_Muon_ety, &b_MET_Muon_ety);
   fChain->SetBranchAddress("MET_Muon_sumet", &MET_Muon_sumet, &b_MET_Muon_sumet);
   fChain->SetBranchAddress("MET_Muon_et", &MET_Muon_et, &b_MET_Muon_et);
   fChain->SetBranchAddress("MET_Muon_phi", &MET_Muon_phi, &b_MET_Muon_phi);
   fChain->SetBranchAddress("MET_Final_etx", &MET_Final_etx, &b_MET_Final_etx);
   fChain->SetBranchAddress("MET_Final_ety", &MET_Final_ety, &b_MET_Final_ety);
   fChain->SetBranchAddress("MET_Final_sumet", &MET_Final_sumet, &b_MET_Final_sumet);
   fChain->SetBranchAddress("MET_Final_et", &MET_Final_et, &b_MET_Final_et);
   fChain->SetBranchAddress("MET_Final_phi", &MET_Final_phi, &b_MET_Final_phi);
   fChain->SetBranchAddress("MET_TopoObj_etx", &MET_TopoObj_etx, &b_MET_TopoObj_etx);
   fChain->SetBranchAddress("MET_TopoObj_ety", &MET_TopoObj_ety, &b_MET_TopoObj_ety);
   fChain->SetBranchAddress("MET_TopoObj_sumet", &MET_TopoObj_sumet, &b_MET_TopoObj_sumet);
   fChain->SetBranchAddress("MET_TopoObj_et", &MET_TopoObj_et, &b_MET_TopoObj_et);
   fChain->SetBranchAddress("MET_TopoObj_phi", &MET_TopoObj_phi, &b_MET_TopoObj_phi);
   fChain->SetBranchAddress("MET_LocHadTopoObj_etx", &MET_LocHadTopoObj_etx, &b_MET_LocHadTopoObj_etx);
   fChain->SetBranchAddress("MET_LocHadTopoObj_ety", &MET_LocHadTopoObj_ety, &b_MET_LocHadTopoObj_ety);
   fChain->SetBranchAddress("MET_LocHadTopoObj_sumet", &MET_LocHadTopoObj_sumet, &b_MET_LocHadTopoObj_sumet);
   fChain->SetBranchAddress("MET_LocHadTopoObj_et", &MET_LocHadTopoObj_et, &b_MET_LocHadTopoObj_et);
   fChain->SetBranchAddress("MET_LocHadTopoObj_phi", &MET_LocHadTopoObj_phi, &b_MET_LocHadTopoObj_phi);
   fChain->SetBranchAddress("MET_Base_etx", &MET_Base_etx, &b_MET_Base_etx);
   fChain->SetBranchAddress("MET_Base_ety", &MET_Base_ety, &b_MET_Base_ety);
   fChain->SetBranchAddress("MET_Base_sumet", &MET_Base_sumet, &b_MET_Base_sumet);
   fChain->SetBranchAddress("MET_Base_et", &MET_Base_et, &b_MET_Base_et);
   fChain->SetBranchAddress("MET_Base_phi", &MET_Base_phi, &b_MET_Base_phi);
   fChain->SetBranchAddress("MET_Base0_etx", &MET_Base0_etx, &b_MET_Base0_etx);
   fChain->SetBranchAddress("MET_Base0_ety", &MET_Base0_ety, &b_MET_Base0_ety);
   fChain->SetBranchAddress("MET_Base0_sumet", &MET_Base0_sumet, &b_MET_Base0_sumet);
   fChain->SetBranchAddress("MET_Base0_et", &MET_Base0_et, &b_MET_Base0_et);
   fChain->SetBranchAddress("MET_Base0_phi", &MET_Base0_phi, &b_MET_Base0_phi);
   fChain->SetBranchAddress("MET_Calib_etx", &MET_Calib_etx, &b_MET_Calib_etx);
   fChain->SetBranchAddress("MET_Calib_ety", &MET_Calib_ety, &b_MET_Calib_ety);
   fChain->SetBranchAddress("MET_Calib_sumet", &MET_Calib_sumet, &b_MET_Calib_sumet);
   fChain->SetBranchAddress("MET_Calib_et", &MET_Calib_et, &b_MET_Calib_et);
   fChain->SetBranchAddress("MET_Calib_phi", &MET_Calib_phi, &b_MET_Calib_phi);
   fChain->SetBranchAddress("MET_Topo_etx", &MET_Topo_etx, &b_MET_Topo_etx);
   fChain->SetBranchAddress("MET_Topo_ety", &MET_Topo_ety, &b_MET_Topo_ety);
   fChain->SetBranchAddress("MET_Topo_sumet", &MET_Topo_sumet, &b_MET_Topo_sumet);
   fChain->SetBranchAddress("MET_Topo_et", &MET_Topo_et, &b_MET_Topo_et);
   fChain->SetBranchAddress("MET_Topo_phi", &MET_Topo_phi, &b_MET_Topo_phi);
   fChain->SetBranchAddress("MET_CorrTopo_etx", &MET_CorrTopo_etx, &b_MET_CorrTopo_etx);
   fChain->SetBranchAddress("MET_CorrTopo_ety", &MET_CorrTopo_ety, &b_MET_CorrTopo_ety);
   fChain->SetBranchAddress("MET_CorrTopo_sumet", &MET_CorrTopo_sumet, &b_MET_CorrTopo_sumet);
   fChain->SetBranchAddress("MET_CorrTopo_et", &MET_CorrTopo_et, &b_MET_CorrTopo_et);
   fChain->SetBranchAddress("MET_CorrTopo_phi", &MET_CorrTopo_phi, &b_MET_CorrTopo_phi);
   fChain->SetBranchAddress("MET_LocHadTopo_etx", &MET_LocHadTopo_etx, &b_MET_LocHadTopo_etx);
   fChain->SetBranchAddress("MET_LocHadTopo_ety", &MET_LocHadTopo_ety, &b_MET_LocHadTopo_ety);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet", &MET_LocHadTopo_sumet, &b_MET_LocHadTopo_sumet);
   fChain->SetBranchAddress("MET_LocHadTopo_et", &MET_LocHadTopo_et, &b_MET_LocHadTopo_et);
   fChain->SetBranchAddress("MET_LocHadTopo_phi", &MET_LocHadTopo_phi, &b_MET_LocHadTopo_phi);
   fChain->SetBranchAddress("cl_n", &cl_n, &b_cl_n);
   fChain->SetBranchAddress("cl_E", &cl_E, &b_cl_E);
   fChain->SetBranchAddress("cl_pt", &cl_pt, &b_cl_pt);
   fChain->SetBranchAddress("cl_m", &cl_m, &b_cl_m);
   fChain->SetBranchAddress("cl_eta", &cl_eta, &b_cl_eta);
   fChain->SetBranchAddress("cl_phi", &cl_phi, &b_cl_phi);
   fChain->SetBranchAddress("cl_E_em", &cl_E_em, &b_cl_E_em);
   fChain->SetBranchAddress("cl_E_had", &cl_E_had, &b_cl_E_had);
   fChain->SetBranchAddress("cl_firstEdens", &cl_firstEdens, &b_cl_firstEdens);
   fChain->SetBranchAddress("cl_cellmaxfrac", &cl_cellmaxfrac, &b_cl_cellmaxfrac);
   fChain->SetBranchAddress("cl_longitudinal", &cl_longitudinal, &b_cl_longitudinal);
   fChain->SetBranchAddress("cl_secondlambda", &cl_secondlambda, &b_cl_secondlambda);
   fChain->SetBranchAddress("cl_lateral", &cl_lateral, &b_cl_lateral);
   fChain->SetBranchAddress("cl_secondR", &cl_secondR, &b_cl_secondR);
   fChain->SetBranchAddress("cl_centerlambda", &cl_centerlambda, &b_cl_centerlambda);
   fChain->SetBranchAddress("cl_deltaTheta", &cl_deltaTheta, &b_cl_deltaTheta);
   fChain->SetBranchAddress("cl_deltaPhi", &cl_deltaPhi, &b_cl_deltaPhi);
   fChain->SetBranchAddress("cl_time", &cl_time, &b_cl_time);
   fChain->SetBranchAddress("emcl_n", &emcl_n, &b_emcl_n);
   fChain->SetBranchAddress("emcl_E", &emcl_E, &b_emcl_E);
   fChain->SetBranchAddress("emcl_pt", &emcl_pt, &b_emcl_pt);
   fChain->SetBranchAddress("emcl_m", &emcl_m, &b_emcl_m);
   fChain->SetBranchAddress("emcl_eta", &emcl_eta, &b_emcl_eta);
   fChain->SetBranchAddress("emcl_phi", &emcl_phi, &b_emcl_phi);
   fChain->SetBranchAddress("emcl_E_em", &emcl_E_em, &b_emcl_E_em);
   fChain->SetBranchAddress("emcl_E_had", &emcl_E_had, &b_emcl_E_had);
   fChain->SetBranchAddress("emcl_firstEdens", &emcl_firstEdens, &b_emcl_firstEdens);
   fChain->SetBranchAddress("emcl_cellmaxfrac", &emcl_cellmaxfrac, &b_emcl_cellmaxfrac);
   fChain->SetBranchAddress("emcl_longitudinal", &emcl_longitudinal, &b_emcl_longitudinal);
   fChain->SetBranchAddress("emcl_secondlambda", &emcl_secondlambda, &b_emcl_secondlambda);
   fChain->SetBranchAddress("emcl_lateral", &emcl_lateral, &b_emcl_lateral);
   fChain->SetBranchAddress("emcl_secondR", &emcl_secondR, &b_emcl_secondR);
   fChain->SetBranchAddress("emcl_centerlambda", &emcl_centerlambda, &b_emcl_centerlambda);
   fChain->SetBranchAddress("emcl_deltaTheta", &emcl_deltaTheta, &b_emcl_deltaTheta);
   fChain->SetBranchAddress("emcl_deltaPhi", &emcl_deltaPhi, &b_emcl_deltaPhi);
   fChain->SetBranchAddress("emcl_time", &emcl_time, &b_emcl_time);
   fChain->SetBranchAddress("emcl_E_PreSamplerB", &emcl_E_PreSamplerB, &b_emcl_E_PreSamplerB);
   fChain->SetBranchAddress("emcl_E_EMB1", &emcl_E_EMB1, &b_emcl_E_EMB1);
   fChain->SetBranchAddress("emcl_E_EMB2", &emcl_E_EMB2, &b_emcl_E_EMB2);
   fChain->SetBranchAddress("emcl_E_EMB3", &emcl_E_EMB3, &b_emcl_E_EMB3);
   fChain->SetBranchAddress("emcl_E_PreSamplerE", &emcl_E_PreSamplerE, &b_emcl_E_PreSamplerE);
   fChain->SetBranchAddress("emcl_E_EME1", &emcl_E_EME1, &b_emcl_E_EME1);
   fChain->SetBranchAddress("emcl_E_EME2", &emcl_E_EME2, &b_emcl_E_EME2);
   fChain->SetBranchAddress("emcl_E_EME3", &emcl_E_EME3, &b_emcl_E_EME3);
   fChain->SetBranchAddress("emcl_E_HEC0", &emcl_E_HEC0, &b_emcl_E_HEC0);
   fChain->SetBranchAddress("emcl_E_HEC1", &emcl_E_HEC1, &b_emcl_E_HEC1);
   fChain->SetBranchAddress("emcl_E_HEC2", &emcl_E_HEC2, &b_emcl_E_HEC2);
   fChain->SetBranchAddress("emcl_E_HEC3", &emcl_E_HEC3, &b_emcl_E_HEC3);
   fChain->SetBranchAddress("emcl_E_TileBar0", &emcl_E_TileBar0, &b_emcl_E_TileBar0);
   fChain->SetBranchAddress("emcl_E_TileBar1", &emcl_E_TileBar1, &b_emcl_E_TileBar1);
   fChain->SetBranchAddress("emcl_E_TileBar2", &emcl_E_TileBar2, &b_emcl_E_TileBar2);
   fChain->SetBranchAddress("emcl_E_TileGap1", &emcl_E_TileGap1, &b_emcl_E_TileGap1);
   fChain->SetBranchAddress("emcl_E_TileGap2", &emcl_E_TileGap2, &b_emcl_E_TileGap2);
   fChain->SetBranchAddress("emcl_E_TileGap3", &emcl_E_TileGap3, &b_emcl_E_TileGap3);
   fChain->SetBranchAddress("emcl_E_TileExt0", &emcl_E_TileExt0, &b_emcl_E_TileExt0);
   fChain->SetBranchAddress("emcl_E_TileExt1", &emcl_E_TileExt1, &b_emcl_E_TileExt1);
   fChain->SetBranchAddress("emcl_E_TileExt2", &emcl_E_TileExt2, &b_emcl_E_TileExt2);
   fChain->SetBranchAddress("emcl_E_FCAL0", &emcl_E_FCAL0, &b_emcl_E_FCAL0);
   fChain->SetBranchAddress("emcl_E_FCAL1", &emcl_E_FCAL1, &b_emcl_E_FCAL1);
   fChain->SetBranchAddress("emcl_E_FCAL2", &emcl_E_FCAL2, &b_emcl_E_FCAL2);
   fChain->SetBranchAddress("trk_n", &trk_n, &b_trk_n);
   fChain->SetBranchAddress("trk_d0", &trk_d0, &b_trk_d0);
   fChain->SetBranchAddress("trk_z0", &trk_z0, &b_trk_z0);
   fChain->SetBranchAddress("trk_phi", &trk_phi, &b_trk_phi);
   fChain->SetBranchAddress("trk_theta", &trk_theta, &b_trk_theta);
   fChain->SetBranchAddress("trk_qoverp", &trk_qoverp, &b_trk_qoverp);
   fChain->SetBranchAddress("trk_pt", &trk_pt, &b_trk_pt);
   fChain->SetBranchAddress("trk_eta", &trk_eta, &b_trk_eta);
   fChain->SetBranchAddress("trk_err_d0", &trk_err_d0, &b_trk_err_d0);
   fChain->SetBranchAddress("trk_err_z0", &trk_err_z0, &b_trk_err_z0);
   fChain->SetBranchAddress("trk_err_phi", &trk_err_phi, &b_trk_err_phi);
   fChain->SetBranchAddress("trk_err_theta", &trk_err_theta, &b_trk_err_theta);
   fChain->SetBranchAddress("trk_err_qoverp", &trk_err_qoverp, &b_trk_err_qoverp);
   fChain->SetBranchAddress("trk_cov_d0", &trk_cov_d0, &b_trk_cov_d0);
   fChain->SetBranchAddress("trk_cov_z0", &trk_cov_z0, &b_trk_cov_z0);
   fChain->SetBranchAddress("trk_cov_phi", &trk_cov_phi, &b_trk_cov_phi);
   fChain->SetBranchAddress("trk_cov_theta", &trk_cov_theta, &b_trk_cov_theta);
   fChain->SetBranchAddress("trk_cov_qoverp", &trk_cov_qoverp, &b_trk_cov_qoverp);
   fChain->SetBranchAddress("trk_cov_d0_z0", &trk_cov_d0_z0, &b_trk_cov_d0_z0);
   fChain->SetBranchAddress("trk_cov_d0_phi", &trk_cov_d0_phi, &b_trk_cov_d0_phi);
   fChain->SetBranchAddress("trk_cov_d0_theta", &trk_cov_d0_theta, &b_trk_cov_d0_theta);
   fChain->SetBranchAddress("trk_cov_d0_qoverp", &trk_cov_d0_qoverp, &b_trk_cov_d0_qoverp);
   fChain->SetBranchAddress("trk_cov_z0_phi", &trk_cov_z0_phi, &b_trk_cov_z0_phi);
   fChain->SetBranchAddress("trk_cov_z0_theta", &trk_cov_z0_theta, &b_trk_cov_z0_theta);
   fChain->SetBranchAddress("trk_cov_z0_qoverp", &trk_cov_z0_qoverp, &b_trk_cov_z0_qoverp);
   fChain->SetBranchAddress("trk_cov_phi_theta", &trk_cov_phi_theta, &b_trk_cov_phi_theta);
   fChain->SetBranchAddress("trk_cov_phi_qoverp", &trk_cov_phi_qoverp, &b_trk_cov_phi_qoverp);
   fChain->SetBranchAddress("trk_cov_theta_qoverp", &trk_cov_theta_qoverp, &b_trk_cov_theta_qoverp);
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
   fChain->SetBranchAddress("trk_d0_wrtPV", &trk_d0_wrtPV, &b_trk_d0_wrtPV);
   fChain->SetBranchAddress("trk_z0_wrtPV", &trk_z0_wrtPV, &b_trk_z0_wrtPV);
   fChain->SetBranchAddress("trk_phi_wrtPV", &trk_phi_wrtPV, &b_trk_phi_wrtPV);
   fChain->SetBranchAddress("trk_theta_wrtPV", &trk_theta_wrtPV, &b_trk_theta_wrtPV);
   fChain->SetBranchAddress("trk_qoverp_wrtPV", &trk_qoverp_wrtPV, &b_trk_qoverp_wrtPV);
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
   fChain->SetBranchAddress("tau_n", &tau_n, &b_tau_n);
   fChain->SetBranchAddress("tau_Et", &tau_Et, &b_tau_Et);
   fChain->SetBranchAddress("tau_pt", &tau_pt, &b_tau_pt);
   fChain->SetBranchAddress("tau_m", &tau_m, &b_tau_m);
   fChain->SetBranchAddress("tau_eta", &tau_eta, &b_tau_eta);
   fChain->SetBranchAddress("tau_phi", &tau_phi, &b_tau_phi);
   fChain->SetBranchAddress("tau_charge", &tau_charge, &b_tau_charge);
   fChain->SetBranchAddress("tau_BDTEleScore", &tau_BDTEleScore, &b_tau_BDTEleScore);
   fChain->SetBranchAddress("tau_BDTJetScore", &tau_BDTJetScore, &b_tau_BDTJetScore);
   fChain->SetBranchAddress("tau_discCut", &tau_discCut, &b_tau_discCut);
   fChain->SetBranchAddress("tau_discCutTMVA", &tau_discCutTMVA, &b_tau_discCutTMVA);
   fChain->SetBranchAddress("tau_discLL", &tau_discLL, &b_tau_discLL);
   fChain->SetBranchAddress("tau_discNN", &tau_discNN, &b_tau_discNN);
   fChain->SetBranchAddress("tau_efficLL", &tau_efficLL, &b_tau_efficLL);
   fChain->SetBranchAddress("tau_efficNN", &tau_efficNN, &b_tau_efficNN);
   fChain->SetBranchAddress("tau_likelihood", &tau_likelihood, &b_tau_likelihood);
   fChain->SetBranchAddress("tau_tauJetNeuralNetwork", &tau_tauJetNeuralNetwork, &b_tau_tauJetNeuralNetwork);
   fChain->SetBranchAddress("tau_tauENeuralNetwork", &tau_tauENeuralNetwork, &b_tau_tauENeuralNetwork);
   fChain->SetBranchAddress("tau_tauElTauLikelihood", &tau_tauElTauLikelihood, &b_tau_tauElTauLikelihood);
   fChain->SetBranchAddress("tau_electronVetoLoose", &tau_electronVetoLoose, &b_tau_electronVetoLoose);
   fChain->SetBranchAddress("tau_electronVetoMedium", &tau_electronVetoMedium, &b_tau_electronVetoMedium);
   fChain->SetBranchAddress("tau_electronVetoTight", &tau_electronVetoTight, &b_tau_electronVetoTight);
   fChain->SetBranchAddress("tau_muonVeto", &tau_muonVeto, &b_tau_muonVeto);
   fChain->SetBranchAddress("tau_tauCutLoose", &tau_tauCutLoose, &b_tau_tauCutLoose);
   fChain->SetBranchAddress("tau_tauCutMedium", &tau_tauCutMedium, &b_tau_tauCutMedium);
   fChain->SetBranchAddress("tau_tauCutTight", &tau_tauCutTight, &b_tau_tauCutTight);
   fChain->SetBranchAddress("tau_tauCutSafeLoose", &tau_tauCutSafeLoose, &b_tau_tauCutSafeLoose);
   fChain->SetBranchAddress("tau_tauCutSafeMedium", &tau_tauCutSafeMedium, &b_tau_tauCutSafeMedium);
   fChain->SetBranchAddress("tau_tauCutSafeTight", &tau_tauCutSafeTight, &b_tau_tauCutSafeTight);
   fChain->SetBranchAddress("tau_tauCutSafeCaloLoose", &tau_tauCutSafeCaloLoose, &b_tau_tauCutSafeCaloLoose);
   fChain->SetBranchAddress("tau_tauCutSafeCaloMedium", &tau_tauCutSafeCaloMedium, &b_tau_tauCutSafeCaloMedium);
   fChain->SetBranchAddress("tau_tauCutSafeCaloTight", &tau_tauCutSafeCaloTight, &b_tau_tauCutSafeCaloTight);
   fChain->SetBranchAddress("tau_tauLlhLoose", &tau_tauLlhLoose, &b_tau_tauLlhLoose);
   fChain->SetBranchAddress("tau_tauLlhMedium", &tau_tauLlhMedium, &b_tau_tauLlhMedium);
   fChain->SetBranchAddress("tau_tauLlhTight", &tau_tauLlhTight, &b_tau_tauLlhTight);
   fChain->SetBranchAddress("tau_author", &tau_author, &b_tau_author);
   fChain->SetBranchAddress("tau_ROIword", &tau_ROIword, &b_tau_ROIword);
   fChain->SetBranchAddress("tau_nProng", &tau_nProng, &b_tau_nProng);
   fChain->SetBranchAddress("tau_numTrack", &tau_numTrack, &b_tau_numTrack);
   fChain->SetBranchAddress("tau_etOverPtLeadTrk", &tau_etOverPtLeadTrk, &b_tau_etOverPtLeadTrk);
   fChain->SetBranchAddress("tau_ipZ0SinThetaSigLeadTrk", &tau_ipZ0SinThetaSigLeadTrk, &b_tau_ipZ0SinThetaSigLeadTrk);
   fChain->SetBranchAddress("tau_leadTrkPt", &tau_leadTrkPt, &b_tau_leadTrkPt);
   fChain->SetBranchAddress("tau_nLooseTrk", &tau_nLooseTrk, &b_tau_nLooseTrk);
   fChain->SetBranchAddress("tau_nLooseConvTrk", &tau_nLooseConvTrk, &b_tau_nLooseConvTrk);
   fChain->SetBranchAddress("tau_nProngLoose", &tau_nProngLoose, &b_tau_nProngLoose);
   fChain->SetBranchAddress("tau_ipSigLeadTrk", &tau_ipSigLeadTrk, &b_tau_ipSigLeadTrk);
   fChain->SetBranchAddress("tau_ipSigLeadLooseTrk", &tau_ipSigLeadLooseTrk, &b_tau_ipSigLeadLooseTrk);
   fChain->SetBranchAddress("tau_etOverPtLeadLooseTrk", &tau_etOverPtLeadLooseTrk, &b_tau_etOverPtLeadLooseTrk);
   fChain->SetBranchAddress("tau_leadLooseTrkPt", &tau_leadLooseTrkPt, &b_tau_leadLooseTrkPt);
   fChain->SetBranchAddress("tau_chrgLooseTrk", &tau_chrgLooseTrk, &b_tau_chrgLooseTrk);
   fChain->SetBranchAddress("tau_massTrkSys", &tau_massTrkSys, &b_tau_massTrkSys);
   fChain->SetBranchAddress("tau_trkWidth2", &tau_trkWidth2, &b_tau_trkWidth2);
   fChain->SetBranchAddress("tau_trFlightPathSig", &tau_trFlightPathSig, &b_tau_trFlightPathSig);
   fChain->SetBranchAddress("tau_etEflow", &tau_etEflow, &b_tau_etEflow);
   fChain->SetBranchAddress("tau_mEflow", &tau_mEflow, &b_tau_mEflow);
   fChain->SetBranchAddress("tau_nPi0", &tau_nPi0, &b_tau_nPi0);
   fChain->SetBranchAddress("tau_ele_E237E277", &tau_ele_E237E277, &b_tau_ele_E237E277);
   fChain->SetBranchAddress("tau_ele_PresamplerFraction", &tau_ele_PresamplerFraction, &b_tau_ele_PresamplerFraction);
   fChain->SetBranchAddress("tau_ele_ECALFirstFraction", &tau_ele_ECALFirstFraction, &b_tau_ele_ECALFirstFraction);
   fChain->SetBranchAddress("tau_seedCalo_EMRadius", &tau_seedCalo_EMRadius, &b_tau_seedCalo_EMRadius);
   fChain->SetBranchAddress("tau_seedCalo_hadRadius", &tau_seedCalo_hadRadius, &b_tau_seedCalo_hadRadius);
   fChain->SetBranchAddress("tau_seedCalo_etEMAtEMScale", &tau_seedCalo_etEMAtEMScale, &b_tau_seedCalo_etEMAtEMScale);
   fChain->SetBranchAddress("tau_seedCalo_etHadAtEMScale", &tau_seedCalo_etHadAtEMScale, &b_tau_seedCalo_etHadAtEMScale);
   fChain->SetBranchAddress("tau_seedCalo_isolFrac", &tau_seedCalo_isolFrac, &b_tau_seedCalo_isolFrac);
   fChain->SetBranchAddress("tau_seedCalo_centFrac", &tau_seedCalo_centFrac, &b_tau_seedCalo_centFrac);
   fChain->SetBranchAddress("tau_seedCalo_stripWidth2", &tau_seedCalo_stripWidth2, &b_tau_seedCalo_stripWidth2);
   fChain->SetBranchAddress("tau_seedCalo_nStrip", &tau_seedCalo_nStrip, &b_tau_seedCalo_nStrip);
   fChain->SetBranchAddress("tau_seedCalo_etEMCalib", &tau_seedCalo_etEMCalib, &b_tau_seedCalo_etEMCalib);
   fChain->SetBranchAddress("tau_seedCalo_etHadCalib", &tau_seedCalo_etHadCalib, &b_tau_seedCalo_etHadCalib);
   fChain->SetBranchAddress("tau_seedCalo_eta", &tau_seedCalo_eta, &b_tau_seedCalo_eta);
   fChain->SetBranchAddress("tau_seedCalo_phi", &tau_seedCalo_phi, &b_tau_seedCalo_phi);
   fChain->SetBranchAddress("tau_seedCalo_nIsolLooseTrk", &tau_seedCalo_nIsolLooseTrk, &b_tau_seedCalo_nIsolLooseTrk);
   fChain->SetBranchAddress("tau_seedCalo_trkAvgDist", &tau_seedCalo_trkAvgDist, &b_tau_seedCalo_trkAvgDist);
   fChain->SetBranchAddress("tau_seedCalo_trkRmsDist", &tau_seedCalo_trkRmsDist, &b_tau_seedCalo_trkRmsDist);
   fChain->SetBranchAddress("tau_seedTrk_EMRadius", &tau_seedTrk_EMRadius, &b_tau_seedTrk_EMRadius);
   fChain->SetBranchAddress("tau_seedTrk_isolFrac", &tau_seedTrk_isolFrac, &b_tau_seedTrk_isolFrac);
   fChain->SetBranchAddress("tau_seedTrk_etChrgHadOverSumTrkPt", &tau_seedTrk_etChrgHadOverSumTrkPt, &b_tau_seedTrk_etChrgHadOverSumTrkPt);
   fChain->SetBranchAddress("tau_seedTrk_isolFracWide", &tau_seedTrk_isolFracWide, &b_tau_seedTrk_isolFracWide);
   fChain->SetBranchAddress("tau_seedTrk_etHadAtEMScale", &tau_seedTrk_etHadAtEMScale, &b_tau_seedTrk_etHadAtEMScale);
   fChain->SetBranchAddress("tau_seedTrk_etEMAtEMScale", &tau_seedTrk_etEMAtEMScale, &b_tau_seedTrk_etEMAtEMScale);
   fChain->SetBranchAddress("tau_seedTrk_etEMCL", &tau_seedTrk_etEMCL, &b_tau_seedTrk_etEMCL);
   fChain->SetBranchAddress("tau_seedTrk_etChrgEM", &tau_seedTrk_etChrgEM, &b_tau_seedTrk_etChrgEM);
   fChain->SetBranchAddress("tau_seedTrk_etNeuEM", &tau_seedTrk_etNeuEM, &b_tau_seedTrk_etNeuEM);
   fChain->SetBranchAddress("tau_seedTrk_etResNeuEM", &tau_seedTrk_etResNeuEM, &b_tau_seedTrk_etResNeuEM);
   fChain->SetBranchAddress("tau_seedTrk_hadLeakEt", &tau_seedTrk_hadLeakEt, &b_tau_seedTrk_hadLeakEt);
   fChain->SetBranchAddress("tau_seedTrk_sumEMCellEtOverLeadTrkPt", &tau_seedTrk_sumEMCellEtOverLeadTrkPt, &b_tau_seedTrk_sumEMCellEtOverLeadTrkPt);
   fChain->SetBranchAddress("tau_seedTrk_secMaxStripEt", &tau_seedTrk_secMaxStripEt, &b_tau_seedTrk_secMaxStripEt);
   fChain->SetBranchAddress("tau_seedTrk_stripWidth2", &tau_seedTrk_stripWidth2, &b_tau_seedTrk_stripWidth2);
   fChain->SetBranchAddress("tau_seedTrk_nStrip", &tau_seedTrk_nStrip, &b_tau_seedTrk_nStrip);
   fChain->SetBranchAddress("tau_seedTrk_etChrgHad", &tau_seedTrk_etChrgHad, &b_tau_seedTrk_etChrgHad);
   fChain->SetBranchAddress("tau_seedTrk_nOtherCoreTrk", &tau_seedTrk_nOtherCoreTrk, &b_tau_seedTrk_nOtherCoreTrk);
   fChain->SetBranchAddress("tau_seedTrk_nIsolTrk", &tau_seedTrk_nIsolTrk, &b_tau_seedTrk_nIsolTrk);
   fChain->SetBranchAddress("tau_seedTrk_etIsolEM", &tau_seedTrk_etIsolEM, &b_tau_seedTrk_etIsolEM);
   fChain->SetBranchAddress("tau_seedTrk_etIsolHad", &tau_seedTrk_etIsolHad, &b_tau_seedTrk_etIsolHad);
   fChain->SetBranchAddress("tau_calcVars_sumTrkPt", &tau_calcVars_sumTrkPt, &b_tau_calcVars_sumTrkPt);
   fChain->SetBranchAddress("tau_calcVars_numTopoClusters", &tau_calcVars_numTopoClusters, &b_tau_calcVars_numTopoClusters);
   fChain->SetBranchAddress("tau_calcVars_numEffTopoClusters", &tau_calcVars_numEffTopoClusters, &b_tau_calcVars_numEffTopoClusters);
   fChain->SetBranchAddress("tau_calcVars_topoInvMass", &tau_calcVars_topoInvMass, &b_tau_calcVars_topoInvMass);
   fChain->SetBranchAddress("tau_calcVars_effTopoInvMass", &tau_calcVars_effTopoInvMass, &b_tau_calcVars_effTopoInvMass);
   fChain->SetBranchAddress("tau_calcVars_topoMeanDeltaR", &tau_calcVars_topoMeanDeltaR, &b_tau_calcVars_topoMeanDeltaR);
   fChain->SetBranchAddress("tau_calcVars_effTopoMeanDeltaR", &tau_calcVars_effTopoMeanDeltaR, &b_tau_calcVars_effTopoMeanDeltaR);
   fChain->SetBranchAddress("tau_calcVars_etHadSumPtTracks", &tau_calcVars_etHadSumPtTracks, &b_tau_calcVars_etHadSumPtTracks);
   fChain->SetBranchAddress("tau_calcVars_etEMSumPtTracks", &tau_calcVars_etEMSumPtTracks, &b_tau_calcVars_etEMSumPtTracks);
   fChain->SetBranchAddress("tau_track_n", &tau_track_n, &b_tau_track_n);
   fChain->SetBranchAddress("tau_EF_dr", &tau_EF_dr, &b_tau_EF_dr);
   fChain->SetBranchAddress("tau_EF_matched", &tau_EF_matched, &b_tau_EF_matched);
   fChain->SetBranchAddress("tau_L2_dr", &tau_L2_dr, &b_tau_L2_dr);
   fChain->SetBranchAddress("tau_L2_E", &tau_L2_E, &b_tau_L2_E);
   fChain->SetBranchAddress("tau_L2_Et", &tau_L2_Et, &b_tau_L2_Et);
   fChain->SetBranchAddress("tau_L2_pt", &tau_L2_pt, &b_tau_L2_pt);
   fChain->SetBranchAddress("tau_L2_eta", &tau_L2_eta, &b_tau_L2_eta);
   fChain->SetBranchAddress("tau_L2_phi", &tau_L2_phi, &b_tau_L2_phi);
   fChain->SetBranchAddress("tau_L2_matched", &tau_L2_matched, &b_tau_L2_matched);
   fChain->SetBranchAddress("tau_L1_dr", &tau_L1_dr, &b_tau_L1_dr);
   fChain->SetBranchAddress("tau_L1_Et", &tau_L1_Et, &b_tau_L1_Et);
   fChain->SetBranchAddress("tau_L1_pt", &tau_L1_pt, &b_tau_L1_pt);
   fChain->SetBranchAddress("tau_L1_eta", &tau_L1_eta, &b_tau_L1_eta);
   fChain->SetBranchAddress("tau_L1_phi", &tau_L1_phi, &b_tau_L1_phi);
   fChain->SetBranchAddress("tau_L1_matched", &tau_L1_matched, &b_tau_L1_matched);
   fChain->SetBranchAddress("mb_n", &mb_n, &b_mb_n);
   fChain->SetBranchAddress("mb_E", &mb_E, &b_mb_E);
   fChain->SetBranchAddress("mb_eta", &mb_eta, &b_mb_eta);
   fChain->SetBranchAddress("mb_phi", &mb_phi, &b_mb_phi);
   fChain->SetBranchAddress("mb_time", &mb_time, &b_mb_time);
   fChain->SetBranchAddress("mb_quality", &mb_quality, &b_mb_quality);
   fChain->SetBranchAddress("mb_type", &mb_type, &b_mb_type);
   fChain->SetBranchAddress("mb_module", &mb_module, &b_mb_module);
   fChain->SetBranchAddress("mb_channel", &mb_channel, &b_mb_channel);
   fChain->SetBranchAddress("mbtime_timeDiff", &mbtime_timeDiff, &b_mbtime_timeDiff);
   fChain->SetBranchAddress("mbtime_timeA", &mbtime_timeA, &b_mbtime_timeA);
   fChain->SetBranchAddress("mbtime_timeC", &mbtime_timeC, &b_mbtime_timeC);
   fChain->SetBranchAddress("mbtime_countA", &mbtime_countA, &b_mbtime_countA);
   fChain->SetBranchAddress("mbtime_countC", &mbtime_countC, &b_mbtime_countC);
   fChain->SetBranchAddress("L1_MBTS_1", &L1_MBTS_1, &b_L1_MBTS_1);
   fChain->SetBranchAddress("L1_MBTS_1_1", &L1_MBTS_1_1, &b_L1_MBTS_1_1);
   fChain->SetBranchAddress("L1_MBTS_1_1_Col", &L1_MBTS_1_1_Col, &b_L1_MBTS_1_1_Col);
   fChain->SetBranchAddress("L1_MBTS_1_1_EMPTY", &L1_MBTS_1_1_EMPTY, &b_L1_MBTS_1_1_EMPTY);
   fChain->SetBranchAddress("L1_MBTS_1_1_FIRSTEMPTY", &L1_MBTS_1_1_FIRSTEMPTY, &b_L1_MBTS_1_1_FIRSTEMPTY);
   fChain->SetBranchAddress("L1_MBTS_1_1_UNPAIRED", &L1_MBTS_1_1_UNPAIRED, &b_L1_MBTS_1_1_UNPAIRED);
   fChain->SetBranchAddress("L1_MBTS_1_1_UNPAIRED1", &L1_MBTS_1_1_UNPAIRED1, &b_L1_MBTS_1_1_UNPAIRED1);
   fChain->SetBranchAddress("L1_MBTS_1_1_UNPAIRED2", &L1_MBTS_1_1_UNPAIRED2, &b_L1_MBTS_1_1_UNPAIRED2);
   fChain->SetBranchAddress("L1_MBTS_1_Col", &L1_MBTS_1_Col, &b_L1_MBTS_1_Col);
   fChain->SetBranchAddress("L1_MBTS_1_EMPTY", &L1_MBTS_1_EMPTY, &b_L1_MBTS_1_EMPTY);
   fChain->SetBranchAddress("L1_MBTS_1_FIRSTEMPTY", &L1_MBTS_1_FIRSTEMPTY, &b_L1_MBTS_1_FIRSTEMPTY);
   fChain->SetBranchAddress("L1_MBTS_1_UNPAIRED", &L1_MBTS_1_UNPAIRED, &b_L1_MBTS_1_UNPAIRED);
   fChain->SetBranchAddress("L1_MBTS_1_UNPAIRED1", &L1_MBTS_1_UNPAIRED1, &b_L1_MBTS_1_UNPAIRED1);
   fChain->SetBranchAddress("L1_MBTS_1_UNPAIRED2", &L1_MBTS_1_UNPAIRED2, &b_L1_MBTS_1_UNPAIRED2);
   fChain->SetBranchAddress("L1_MBTS_2", &L1_MBTS_2, &b_L1_MBTS_2);
   fChain->SetBranchAddress("L1_MBTS_2_Col", &L1_MBTS_2_Col, &b_L1_MBTS_2_Col);
   fChain->SetBranchAddress("L1_MBTS_2_EMPTY", &L1_MBTS_2_EMPTY, &b_L1_MBTS_2_EMPTY);
   fChain->SetBranchAddress("L1_MBTS_2_FIRSTEMPTY", &L1_MBTS_2_FIRSTEMPTY, &b_L1_MBTS_2_FIRSTEMPTY);
   fChain->SetBranchAddress("L1_MBTS_2_UNPAIRED", &L1_MBTS_2_UNPAIRED, &b_L1_MBTS_2_UNPAIRED);
   fChain->SetBranchAddress("L1_MBTS_2_UNPAIRED1", &L1_MBTS_2_UNPAIRED1, &b_L1_MBTS_2_UNPAIRED1);
   fChain->SetBranchAddress("L1_MBTS_2_UNPAIRED2", &L1_MBTS_2_UNPAIRED2, &b_L1_MBTS_2_UNPAIRED2);
   fChain->SetBranchAddress("L1_MBTS_4_4", &L1_MBTS_4_4, &b_L1_MBTS_4_4);
   fChain->SetBranchAddress("L1_MBTS_4_4_FIRSTEMPTY", &L1_MBTS_4_4_FIRSTEMPTY, &b_L1_MBTS_4_4_FIRSTEMPTY);
   fChain->SetBranchAddress("L1_MBTS_4_4_UNPAIRED", &L1_MBTS_4_4_UNPAIRED, &b_L1_MBTS_4_4_UNPAIRED);
   fChain->SetBranchAddress("L1_MBTS_4_4_UNPAIRED1", &L1_MBTS_4_4_UNPAIRED1, &b_L1_MBTS_4_4_UNPAIRED1);
   fChain->SetBranchAddress("L1_MBTS_4_4_UNPAIRED2", &L1_MBTS_4_4_UNPAIRED2, &b_L1_MBTS_4_4_UNPAIRED2);
   fChain->SetBranchAddress("L1_MBTS_A", &L1_MBTS_A, &b_L1_MBTS_A);
   fChain->SetBranchAddress("L1_MBTS_C", &L1_MBTS_C, &b_L1_MBTS_C);
   fChain->SetBranchAddress("collcand_passMBTSTime", &collcand_passMBTSTime, &b_collcand_passMBTSTime);
   fChain->SetBranchAddress("collcand_passCaloTime", &collcand_passCaloTime, &b_collcand_passCaloTime);
   fChain->SetBranchAddress("collcand_passTrigger", &collcand_passTrigger, &b_collcand_passTrigger);
   fChain->SetBranchAddress("collcand_pass", &collcand_pass, &b_collcand_pass);
   fChain->SetBranchAddress("vxp_n", &vxp_n, &b_vxp_n);
   fChain->SetBranchAddress("vxp_x", &vxp_x, &b_vxp_x);
   fChain->SetBranchAddress("vxp_y", &vxp_y, &b_vxp_y);
   fChain->SetBranchAddress("vxp_z", &vxp_z, &b_vxp_z);
   fChain->SetBranchAddress("vxp_chi2", &vxp_chi2, &b_vxp_chi2);
   fChain->SetBranchAddress("vxp_ndof", &vxp_ndof, &b_vxp_ndof);
   fChain->SetBranchAddress("vxp_err_x", &vxp_err_x, &b_vxp_err_x);
   fChain->SetBranchAddress("vxp_err_y", &vxp_err_y, &b_vxp_err_y);
   fChain->SetBranchAddress("vxp_err_z", &vxp_err_z, &b_vxp_err_z);
   fChain->SetBranchAddress("vxp_px", &vxp_px, &b_vxp_px);
   fChain->SetBranchAddress("vxp_py", &vxp_py, &b_vxp_py);
   fChain->SetBranchAddress("vxp_pz", &vxp_pz, &b_vxp_pz);
   fChain->SetBranchAddress("vxp_E", &vxp_E, &b_vxp_E);
   fChain->SetBranchAddress("vxp_m", &vxp_m, &b_vxp_m);
   fChain->SetBranchAddress("vxp_nTracks", &vxp_nTracks, &b_vxp_nTracks);
   fChain->SetBranchAddress("vxp_sumPt", &vxp_sumPt, &b_vxp_sumPt);
   fChain->SetBranchAddress("vxp_type", &vxp_type, &b_vxp_type);
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
   fChain->SetBranchAddress("muonTruth_n", &muonTruth_n, &b_muonTruth_n);
   fChain->SetBranchAddress("muonTruth_pt", &muonTruth_pt, &b_muonTruth_pt);
   fChain->SetBranchAddress("muonTruth_m", &muonTruth_m, &b_muonTruth_m);
   fChain->SetBranchAddress("muonTruth_eta", &muonTruth_eta, &b_muonTruth_eta);
   fChain->SetBranchAddress("muonTruth_phi", &muonTruth_phi, &b_muonTruth_phi);
   fChain->SetBranchAddress("muonTruth_charge", &muonTruth_charge, &b_muonTruth_charge);
   fChain->SetBranchAddress("muonTruth_PDGID", &muonTruth_PDGID, &b_muonTruth_PDGID);
   fChain->SetBranchAddress("muonTruth_barcode", &muonTruth_barcode, &b_muonTruth_barcode);
   fChain->SetBranchAddress("muonTruth_type", &muonTruth_type, &b_muonTruth_type);
   fChain->SetBranchAddress("muonTruth_origin", &muonTruth_origin, &b_muonTruth_origin);
   fChain->SetBranchAddress("mcevt_n", &mcevt_n, &b_mcevt_n);
   fChain->SetBranchAddress("mcevt_signal_process_id", &mcevt_signal_process_id, &b_mcevt_signal_process_id);
   fChain->SetBranchAddress("mcevt_event_number", &mcevt_event_number, &b_mcevt_event_number);
   fChain->SetBranchAddress("mcevt_event_scale", &mcevt_event_scale, &b_mcevt_event_scale);
   fChain->SetBranchAddress("mcevt_alphaQCD", &mcevt_alphaQCD, &b_mcevt_alphaQCD);
   fChain->SetBranchAddress("mcevt_alphaQED", &mcevt_alphaQED, &b_mcevt_alphaQED);
   fChain->SetBranchAddress("mcevt_pdf_id1", &mcevt_pdf_id1, &b_mcevt_pdf_id1);
   fChain->SetBranchAddress("mcevt_pdf_id2", &mcevt_pdf_id2, &b_mcevt_pdf_id2);
   fChain->SetBranchAddress("mcevt_pdf_x1", &mcevt_pdf_x1, &b_mcevt_pdf_x1);
   fChain->SetBranchAddress("mcevt_pdf_x2", &mcevt_pdf_x2, &b_mcevt_pdf_x2);
   fChain->SetBranchAddress("mcevt_pdf_scale", &mcevt_pdf_scale, &b_mcevt_pdf_scale);
   fChain->SetBranchAddress("mcevt_pdf1", &mcevt_pdf1, &b_mcevt_pdf1);
   fChain->SetBranchAddress("mcevt_pdf2", &mcevt_pdf2, &b_mcevt_pdf2);
   fChain->SetBranchAddress("mcevt_weight", &mcevt_weight, &b_mcevt_weight);
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
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM10", &trig_L1_emtau_L1_EM10, &b_trig_L1_emtau_L1_EM10);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM10I", &trig_L1_emtau_L1_EM10I, &b_trig_L1_emtau_L1_EM10I);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM14", &trig_L1_emtau_L1_EM14, &b_trig_L1_emtau_L1_EM14);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM2", &trig_L1_emtau_L1_EM2, &b_trig_L1_emtau_L1_EM2);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM2_UNPAIRED", &trig_L1_emtau_L1_EM2_UNPAIRED, &b_trig_L1_emtau_L1_EM2_UNPAIRED);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM3", &trig_L1_emtau_L1_EM3, &b_trig_L1_emtau_L1_EM3);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM3_EMPTY", &trig_L1_emtau_L1_EM3_EMPTY, &b_trig_L1_emtau_L1_EM3_EMPTY);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM3_FIRSTEMPTY", &trig_L1_emtau_L1_EM3_FIRSTEMPTY, &b_trig_L1_emtau_L1_EM3_FIRSTEMPTY);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM3_MV", &trig_L1_emtau_L1_EM3_MV, &b_trig_L1_emtau_L1_EM3_MV);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM3_UNPAIRED", &trig_L1_emtau_L1_EM3_UNPAIRED, &b_trig_L1_emtau_L1_EM3_UNPAIRED);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM4", &trig_L1_emtau_L1_EM4, &b_trig_L1_emtau_L1_EM4);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM5", &trig_L1_emtau_L1_EM5, &b_trig_L1_emtau_L1_EM5);
   fChain->SetBranchAddress("trig_L1_emtau_L1_EM5I", &trig_L1_emtau_L1_EM5I, &b_trig_L1_emtau_L1_EM5I);
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
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_n", &trig_L2_trk_idscan_eGamma_n, &b_trig_L2_trk_idscan_eGamma_n);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_algorithmId", &trig_L2_trk_idscan_eGamma_algorithmId, &b_trig_L2_trk_idscan_eGamma_algorithmId);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_trackStatus", &trig_L2_trk_idscan_eGamma_trackStatus, &b_trig_L2_trk_idscan_eGamma_trackStatus);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_chi2Ndof", &trig_L2_trk_idscan_eGamma_chi2Ndof, &b_trig_L2_trk_idscan_eGamma_chi2Ndof);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_nStrawHits", &trig_L2_trk_idscan_eGamma_nStrawHits, &b_trig_L2_trk_idscan_eGamma_nStrawHits);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_nHighThrHits", &trig_L2_trk_idscan_eGamma_nHighThrHits, &b_trig_L2_trk_idscan_eGamma_nHighThrHits);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_nPixelSpacePoints", &trig_L2_trk_idscan_eGamma_nPixelSpacePoints, &b_trig_L2_trk_idscan_eGamma_nPixelSpacePoints);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_nSCT_SpacePoints", &trig_L2_trk_idscan_eGamma_nSCT_SpacePoints, &b_trig_L2_trk_idscan_eGamma_nSCT_SpacePoints);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_a0", &trig_L2_trk_idscan_eGamma_a0, &b_trig_L2_trk_idscan_eGamma_a0);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_z0", &trig_L2_trk_idscan_eGamma_z0, &b_trig_L2_trk_idscan_eGamma_z0);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_phi0", &trig_L2_trk_idscan_eGamma_phi0, &b_trig_L2_trk_idscan_eGamma_phi0);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_eta", &trig_L2_trk_idscan_eGamma_eta, &b_trig_L2_trk_idscan_eGamma_eta);
   fChain->SetBranchAddress("trig_L2_trk_idscan_eGamma_pt", &trig_L2_trk_idscan_eGamma_pt, &b_trig_L2_trk_idscan_eGamma_pt);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_n", &trig_L2_trk_sitrack_eGamma_n, &b_trig_L2_trk_sitrack_eGamma_n);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_algorithmId", &trig_L2_trk_sitrack_eGamma_algorithmId, &b_trig_L2_trk_sitrack_eGamma_algorithmId);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_trackStatus", &trig_L2_trk_sitrack_eGamma_trackStatus, &b_trig_L2_trk_sitrack_eGamma_trackStatus);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_chi2Ndof", &trig_L2_trk_sitrack_eGamma_chi2Ndof, &b_trig_L2_trk_sitrack_eGamma_chi2Ndof);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_nStrawHits", &trig_L2_trk_sitrack_eGamma_nStrawHits, &b_trig_L2_trk_sitrack_eGamma_nStrawHits);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_nHighThrHits", &trig_L2_trk_sitrack_eGamma_nHighThrHits, &b_trig_L2_trk_sitrack_eGamma_nHighThrHits);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_nPixelSpacePoints", &trig_L2_trk_sitrack_eGamma_nPixelSpacePoints, &b_trig_L2_trk_sitrack_eGamma_nPixelSpacePoints);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints", &trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints, &b_trig_L2_trk_sitrack_eGamma_nSCT_SpacePoints);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_a0", &trig_L2_trk_sitrack_eGamma_a0, &b_trig_L2_trk_sitrack_eGamma_a0);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_z0", &trig_L2_trk_sitrack_eGamma_z0, &b_trig_L2_trk_sitrack_eGamma_z0);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_phi0", &trig_L2_trk_sitrack_eGamma_phi0, &b_trig_L2_trk_sitrack_eGamma_phi0);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_eta", &trig_L2_trk_sitrack_eGamma_eta, &b_trig_L2_trk_sitrack_eGamma_eta);
   fChain->SetBranchAddress("trig_L2_trk_sitrack_eGamma_pt", &trig_L2_trk_sitrack_eGamma_pt, &b_trig_L2_trk_sitrack_eGamma_pt);
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
   fChain->SetBranchAddress("trig_L2_el_L2_2e3_NoCut", &trig_L2_el_L2_2e3_NoCut, &b_trig_L2_el_L2_2e3_NoCut);
   fChain->SetBranchAddress("trig_L2_el_L2_2e3_loose", &trig_L2_el_L2_2e3_loose, &b_trig_L2_el_L2_2e3_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_2e3_medium", &trig_L2_el_L2_2e3_medium, &b_trig_L2_el_L2_2e3_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_2e5_loose", &trig_L2_el_L2_2e5_loose, &b_trig_L2_el_L2_2e5_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_2e5_medium", &trig_L2_el_L2_2e5_medium, &b_trig_L2_el_L2_2e5_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_2e6_nocut", &trig_L2_el_L2_2e6_nocut, &b_trig_L2_el_L2_2e6_nocut);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_loose", &trig_L2_el_L2_e10_loose, &b_trig_L2_el_L2_e10_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_loose_IdScan", &trig_L2_el_L2_e10_loose_IdScan, &b_trig_L2_el_L2_e10_loose_IdScan);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_loose_TRT", &trig_L2_el_L2_e10_loose_TRT, &b_trig_L2_el_L2_e10_loose_TRT);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_loose_passEF", &trig_L2_el_L2_e10_loose_passEF, &b_trig_L2_el_L2_e10_loose_passEF);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_loose_passL2", &trig_L2_el_L2_e10_loose_passL2, &b_trig_L2_el_L2_e10_loose_passL2);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_medium", &trig_L2_el_L2_e10_medium, &b_trig_L2_el_L2_e10_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_medium_IdScan", &trig_L2_el_L2_e10_medium_IdScan, &b_trig_L2_el_L2_e10_medium_IdScan);
   fChain->SetBranchAddress("trig_L2_el_L2_e10_medium_TRT", &trig_L2_el_L2_e10_medium_TRT, &b_trig_L2_el_L2_e10_medium_TRT);
   fChain->SetBranchAddress("trig_L2_el_L2_e10i_loose", &trig_L2_el_L2_e10i_loose, &b_trig_L2_el_L2_e10i_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_e15_loose", &trig_L2_el_L2_e15_loose, &b_trig_L2_el_L2_e15_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_e15i_loose", &trig_L2_el_L2_e15i_loose, &b_trig_L2_el_L2_e15i_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_e20_loose_NoIDTrkCut", &trig_L2_el_L2_e20_loose_NoIDTrkCut, &b_trig_L2_el_L2_e20_loose_NoIDTrkCut);
   fChain->SetBranchAddress("trig_L2_el_L2_e3_NoCut", &trig_L2_el_L2_e3_NoCut, &b_trig_L2_el_L2_e3_NoCut);
   fChain->SetBranchAddress("trig_L2_el_L2_e3_NoCut_IdScan", &trig_L2_el_L2_e3_NoCut_IdScan, &b_trig_L2_el_L2_e3_NoCut_IdScan);
   fChain->SetBranchAddress("trig_L2_el_L2_e3_NoCut_TRT", &trig_L2_el_L2_e3_NoCut_TRT, &b_trig_L2_el_L2_e3_NoCut_TRT);
   fChain->SetBranchAddress("trig_L2_el_L2_e3_loose", &trig_L2_el_L2_e3_loose, &b_trig_L2_el_L2_e3_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_e3_loose_IdScan", &trig_L2_el_L2_e3_loose_IdScan, &b_trig_L2_el_L2_e3_loose_IdScan);
   fChain->SetBranchAddress("trig_L2_el_L2_e3_loose_TRT", &trig_L2_el_L2_e3_loose_TRT, &b_trig_L2_el_L2_e3_loose_TRT);
   fChain->SetBranchAddress("trig_L2_el_L2_e3_medium", &trig_L2_el_L2_e3_medium, &b_trig_L2_el_L2_e3_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_e3_medium_IdScan", &trig_L2_el_L2_e3_medium_IdScan, &b_trig_L2_el_L2_e3_medium_IdScan);
   fChain->SetBranchAddress("trig_L2_el_L2_e3_medium_TRT", &trig_L2_el_L2_e3_medium_TRT, &b_trig_L2_el_L2_e3_medium_TRT);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_EFfullcalo", &trig_L2_el_L2_e5_EFfullcalo, &b_trig_L2_el_L2_e5_EFfullcalo);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_NoCut", &trig_L2_el_L2_e5_NoCut, &b_trig_L2_el_L2_e5_NoCut);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_NoCut_FwdBackTrk", &trig_L2_el_L2_e5_NoCut_FwdBackTrk, &b_trig_L2_el_L2_e5_NoCut_FwdBackTrk);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_NoCut_IdScan", &trig_L2_el_L2_e5_NoCut_IdScan, &b_trig_L2_el_L2_e5_NoCut_IdScan);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_NoCut_L2SW", &trig_L2_el_L2_e5_NoCut_L2SW, &b_trig_L2_el_L2_e5_NoCut_L2SW);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_NoCut_Ringer", &trig_L2_el_L2_e5_NoCut_Ringer, &b_trig_L2_el_L2_e5_NoCut_Ringer);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_NoCut_SiTrk", &trig_L2_el_L2_e5_NoCut_SiTrk, &b_trig_L2_el_L2_e5_NoCut_SiTrk);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_NoCut_TRT", &trig_L2_el_L2_e5_NoCut_TRT, &b_trig_L2_el_L2_e5_NoCut_TRT);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_NoCut_cells", &trig_L2_el_L2_e5_NoCut_cells, &b_trig_L2_el_L2_e5_NoCut_cells);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_NoCut_cosmic", &trig_L2_el_L2_e5_NoCut_cosmic, &b_trig_L2_el_L2_e5_NoCut_cosmic);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_NoCut_firstempty", &trig_L2_el_L2_e5_NoCut_firstempty, &b_trig_L2_el_L2_e5_NoCut_firstempty);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_NoCut_unpaired", &trig_L2_el_L2_e5_NoCut_unpaired, &b_trig_L2_el_L2_e5_NoCut_unpaired);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_loose", &trig_L2_el_L2_e5_loose, &b_trig_L2_el_L2_e5_loose);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_loose_IdScan", &trig_L2_el_L2_e5_loose_IdScan, &b_trig_L2_el_L2_e5_loose_IdScan);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_loose_NoIDTrkCut", &trig_L2_el_L2_e5_loose_NoIDTrkCut, &b_trig_L2_el_L2_e5_loose_NoIDTrkCut);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_loose_TRT", &trig_L2_el_L2_e5_loose_TRT, &b_trig_L2_el_L2_e5_loose_TRT);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_medium", &trig_L2_el_L2_e5_medium, &b_trig_L2_el_L2_e5_medium);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_medium_IdScan", &trig_L2_el_L2_e5_medium_IdScan, &b_trig_L2_el_L2_e5_medium_IdScan);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_medium_MV", &trig_L2_el_L2_e5_medium_MV, &b_trig_L2_el_L2_e5_medium_MV);
   fChain->SetBranchAddress("trig_L2_el_L2_e5_medium_TRT", &trig_L2_el_L2_e5_medium_TRT, &b_trig_L2_el_L2_e5_medium_TRT);
   fChain->SetBranchAddress("trig_L2_el_L2_e6_loose", &trig_L2_el_L2_e6_loose, &b_trig_L2_el_L2_e6_loose);
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
   fChain->SetBranchAddress("trig_L2_ph_L2_2g10_nocut", &trig_L2_ph_L2_2g10_nocut, &b_trig_L2_ph_L2_2g10_nocut);
   fChain->SetBranchAddress("trig_L2_ph_L2_2g3_loose", &trig_L2_ph_L2_2g3_loose, &b_trig_L2_ph_L2_2g3_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_2g3_nocut", &trig_L2_ph_L2_2g3_nocut, &b_trig_L2_ph_L2_2g3_nocut);
   fChain->SetBranchAddress("trig_L2_ph_L2_2g5_loose", &trig_L2_ph_L2_2g5_loose, &b_trig_L2_ph_L2_2g5_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g10_loose", &trig_L2_ph_L2_g10_loose, &b_trig_L2_ph_L2_g10_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g10i_loose", &trig_L2_ph_L2_g10i_loose, &b_trig_L2_ph_L2_g10i_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g15_loose", &trig_L2_ph_L2_g15_loose, &b_trig_L2_ph_L2_g15_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g15i_loose", &trig_L2_ph_L2_g15i_loose, &b_trig_L2_ph_L2_g15i_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g3_loose", &trig_L2_ph_L2_g3_loose, &b_trig_L2_ph_L2_g3_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g3_nocut", &trig_L2_ph_L2_g3_nocut, &b_trig_L2_ph_L2_g3_nocut);
   fChain->SetBranchAddress("trig_L2_ph_L2_g3_nocut_unpaired", &trig_L2_ph_L2_g3_nocut_unpaired, &b_trig_L2_ph_L2_g3_nocut_unpaired);
   fChain->SetBranchAddress("trig_L2_ph_L2_g5_EFfullcalo", &trig_L2_ph_L2_g5_EFfullcalo, &b_trig_L2_ph_L2_g5_EFfullcalo);
   fChain->SetBranchAddress("trig_L2_ph_L2_g5_loose", &trig_L2_ph_L2_g5_loose, &b_trig_L2_ph_L2_g5_loose);
   fChain->SetBranchAddress("trig_L2_ph_L2_g5_nocut", &trig_L2_ph_L2_g5_nocut, &b_trig_L2_ph_L2_g5_nocut);
   fChain->SetBranchAddress("trig_L2_ph_L2_g5_nocut_cosmic", &trig_L2_ph_L2_g5_nocut_cosmic, &b_trig_L2_ph_L2_g5_nocut_cosmic);
   fChain->SetBranchAddress("trig_L2_ph_L2_g5_nocut_firstempty", &trig_L2_ph_L2_g5_nocut_firstempty, &b_trig_L2_ph_L2_g5_nocut_firstempty);
   fChain->SetBranchAddress("trig_L2_ph_L2_g5_nocut_unpaired", &trig_L2_ph_L2_g5_nocut_unpaired, &b_trig_L2_ph_L2_g5_nocut_unpaired);
   fChain->SetBranchAddress("trig_L2_ph_L2_g6_loose", &trig_L2_ph_L2_g6_loose, &b_trig_L2_ph_L2_g6_loose);
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
   fChain->SetBranchAddress("trig_EF_el_EF_2e3_NoCut", &trig_EF_el_EF_2e3_NoCut, &b_trig_EF_el_EF_2e3_NoCut);
   fChain->SetBranchAddress("trig_EF_el_EF_2e3_loose", &trig_EF_el_EF_2e3_loose, &b_trig_EF_el_EF_2e3_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_2e3_medium", &trig_EF_el_EF_2e3_medium, &b_trig_EF_el_EF_2e3_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_2e5_loose", &trig_EF_el_EF_2e5_loose, &b_trig_EF_el_EF_2e5_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_2e5_medium", &trig_EF_el_EF_2e5_medium, &b_trig_EF_el_EF_2e5_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_2e6_nocut", &trig_EF_el_EF_2e6_nocut, &b_trig_EF_el_EF_2e6_nocut);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_loose", &trig_EF_el_EF_e10_loose, &b_trig_EF_el_EF_e10_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_loose_IdScan", &trig_EF_el_EF_e10_loose_IdScan, &b_trig_EF_el_EF_e10_loose_IdScan);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_loose_TRT", &trig_EF_el_EF_e10_loose_TRT, &b_trig_EF_el_EF_e10_loose_TRT);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_loose_passEF", &trig_EF_el_EF_e10_loose_passEF, &b_trig_EF_el_EF_e10_loose_passEF);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_loose_passL2", &trig_EF_el_EF_e10_loose_passL2, &b_trig_EF_el_EF_e10_loose_passL2);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_medium", &trig_EF_el_EF_e10_medium, &b_trig_EF_el_EF_e10_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_medium_IdScan", &trig_EF_el_EF_e10_medium_IdScan, &b_trig_EF_el_EF_e10_medium_IdScan);
   fChain->SetBranchAddress("trig_EF_el_EF_e10_medium_TRT", &trig_EF_el_EF_e10_medium_TRT, &b_trig_EF_el_EF_e10_medium_TRT);
   fChain->SetBranchAddress("trig_EF_el_EF_e10i_loose", &trig_EF_el_EF_e10i_loose, &b_trig_EF_el_EF_e10i_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_e15_loose", &trig_EF_el_EF_e15_loose, &b_trig_EF_el_EF_e15_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_e15i_loose", &trig_EF_el_EF_e15i_loose, &b_trig_EF_el_EF_e15i_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_e20_loose_NoIDTrkCut", &trig_EF_el_EF_e20_loose_NoIDTrkCut, &b_trig_EF_el_EF_e20_loose_NoIDTrkCut);
   fChain->SetBranchAddress("trig_EF_el_EF_e3_NoCut", &trig_EF_el_EF_e3_NoCut, &b_trig_EF_el_EF_e3_NoCut);
   fChain->SetBranchAddress("trig_EF_el_EF_e3_NoCut_IdScan", &trig_EF_el_EF_e3_NoCut_IdScan, &b_trig_EF_el_EF_e3_NoCut_IdScan);
   fChain->SetBranchAddress("trig_EF_el_EF_e3_NoCut_TRT", &trig_EF_el_EF_e3_NoCut_TRT, &b_trig_EF_el_EF_e3_NoCut_TRT);
   fChain->SetBranchAddress("trig_EF_el_EF_e3_loose", &trig_EF_el_EF_e3_loose, &b_trig_EF_el_EF_e3_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_e3_loose_IdScan", &trig_EF_el_EF_e3_loose_IdScan, &b_trig_EF_el_EF_e3_loose_IdScan);
   fChain->SetBranchAddress("trig_EF_el_EF_e3_loose_TRT", &trig_EF_el_EF_e3_loose_TRT, &b_trig_EF_el_EF_e3_loose_TRT);
   fChain->SetBranchAddress("trig_EF_el_EF_e3_medium", &trig_EF_el_EF_e3_medium, &b_trig_EF_el_EF_e3_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_e3_medium_IdScan", &trig_EF_el_EF_e3_medium_IdScan, &b_trig_EF_el_EF_e3_medium_IdScan);
   fChain->SetBranchAddress("trig_EF_el_EF_e3_medium_TRT", &trig_EF_el_EF_e3_medium_TRT, &b_trig_EF_el_EF_e3_medium_TRT);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_EFfullcalo", &trig_EF_el_EF_e5_EFfullcalo, &b_trig_EF_el_EF_e5_EFfullcalo);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_NoCut", &trig_EF_el_EF_e5_NoCut, &b_trig_EF_el_EF_e5_NoCut);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_NoCut_FwdBackTrk", &trig_EF_el_EF_e5_NoCut_FwdBackTrk, &b_trig_EF_el_EF_e5_NoCut_FwdBackTrk);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_NoCut_IdScan", &trig_EF_el_EF_e5_NoCut_IdScan, &b_trig_EF_el_EF_e5_NoCut_IdScan);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_NoCut_L2SW", &trig_EF_el_EF_e5_NoCut_L2SW, &b_trig_EF_el_EF_e5_NoCut_L2SW);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_NoCut_Ringer", &trig_EF_el_EF_e5_NoCut_Ringer, &b_trig_EF_el_EF_e5_NoCut_Ringer);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_NoCut_SiTrk", &trig_EF_el_EF_e5_NoCut_SiTrk, &b_trig_EF_el_EF_e5_NoCut_SiTrk);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_NoCut_TRT", &trig_EF_el_EF_e5_NoCut_TRT, &b_trig_EF_el_EF_e5_NoCut_TRT);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_NoCut_cells", &trig_EF_el_EF_e5_NoCut_cells, &b_trig_EF_el_EF_e5_NoCut_cells);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_NoCut_cosmic", &trig_EF_el_EF_e5_NoCut_cosmic, &b_trig_EF_el_EF_e5_NoCut_cosmic);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_NoCut_firstempty", &trig_EF_el_EF_e5_NoCut_firstempty, &b_trig_EF_el_EF_e5_NoCut_firstempty);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_NoCut_unpaired", &trig_EF_el_EF_e5_NoCut_unpaired, &b_trig_EF_el_EF_e5_NoCut_unpaired);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_loose", &trig_EF_el_EF_e5_loose, &b_trig_EF_el_EF_e5_loose);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_loose_IdScan", &trig_EF_el_EF_e5_loose_IdScan, &b_trig_EF_el_EF_e5_loose_IdScan);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_loose_NoIDTrkCut", &trig_EF_el_EF_e5_loose_NoIDTrkCut, &b_trig_EF_el_EF_e5_loose_NoIDTrkCut);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_loose_TRT", &trig_EF_el_EF_e5_loose_TRT, &b_trig_EF_el_EF_e5_loose_TRT);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_medium", &trig_EF_el_EF_e5_medium, &b_trig_EF_el_EF_e5_medium);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_medium_IdScan", &trig_EF_el_EF_e5_medium_IdScan, &b_trig_EF_el_EF_e5_medium_IdScan);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_medium_MV", &trig_EF_el_EF_e5_medium_MV, &b_trig_EF_el_EF_e5_medium_MV);
   fChain->SetBranchAddress("trig_EF_el_EF_e5_medium_TRT", &trig_EF_el_EF_e5_medium_TRT, &b_trig_EF_el_EF_e5_medium_TRT);
   fChain->SetBranchAddress("trig_EF_el_EF_e6_loose", &trig_EF_el_EF_e6_loose, &b_trig_EF_el_EF_e6_loose);
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
   fChain->SetBranchAddress("trig_EF_ph_EF_2g10_nocut", &trig_EF_ph_EF_2g10_nocut, &b_trig_EF_ph_EF_2g10_nocut);
   fChain->SetBranchAddress("trig_EF_ph_EF_2g3_loose", &trig_EF_ph_EF_2g3_loose, &b_trig_EF_ph_EF_2g3_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_2g3_nocut", &trig_EF_ph_EF_2g3_nocut, &b_trig_EF_ph_EF_2g3_nocut);
   fChain->SetBranchAddress("trig_EF_ph_EF_2g5_loose", &trig_EF_ph_EF_2g5_loose, &b_trig_EF_ph_EF_2g5_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g10_loose", &trig_EF_ph_EF_g10_loose, &b_trig_EF_ph_EF_g10_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g10_loose_larcalib", &trig_EF_ph_EF_g10_loose_larcalib, &b_trig_EF_ph_EF_g10_loose_larcalib);
   fChain->SetBranchAddress("trig_EF_ph_EF_g10i_loose", &trig_EF_ph_EF_g10i_loose, &b_trig_EF_ph_EF_g10i_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g15_loose", &trig_EF_ph_EF_g15_loose, &b_trig_EF_ph_EF_g15_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g15i_loose", &trig_EF_ph_EF_g15i_loose, &b_trig_EF_ph_EF_g15i_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g3_loose", &trig_EF_ph_EF_g3_loose, &b_trig_EF_ph_EF_g3_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g3_nocut", &trig_EF_ph_EF_g3_nocut, &b_trig_EF_ph_EF_g3_nocut);
   fChain->SetBranchAddress("trig_EF_ph_EF_g3_nocut_larcalib", &trig_EF_ph_EF_g3_nocut_larcalib, &b_trig_EF_ph_EF_g3_nocut_larcalib);
   fChain->SetBranchAddress("trig_EF_ph_EF_g3_nocut_unpaired", &trig_EF_ph_EF_g3_nocut_unpaired, &b_trig_EF_ph_EF_g3_nocut_unpaired);
   fChain->SetBranchAddress("trig_EF_ph_EF_g5_EFfullcalo", &trig_EF_ph_EF_g5_EFfullcalo, &b_trig_EF_ph_EF_g5_EFfullcalo);
   fChain->SetBranchAddress("trig_EF_ph_EF_g5_loose", &trig_EF_ph_EF_g5_loose, &b_trig_EF_ph_EF_g5_loose);
   fChain->SetBranchAddress("trig_EF_ph_EF_g5_loose_larcalib", &trig_EF_ph_EF_g5_loose_larcalib, &b_trig_EF_ph_EF_g5_loose_larcalib);
   fChain->SetBranchAddress("trig_EF_ph_EF_g5_nocut", &trig_EF_ph_EF_g5_nocut, &b_trig_EF_ph_EF_g5_nocut);
   fChain->SetBranchAddress("trig_EF_ph_EF_g5_nocut_cosmic", &trig_EF_ph_EF_g5_nocut_cosmic, &b_trig_EF_ph_EF_g5_nocut_cosmic);
   fChain->SetBranchAddress("trig_EF_ph_EF_g5_nocut_firstempty", &trig_EF_ph_EF_g5_nocut_firstempty, &b_trig_EF_ph_EF_g5_nocut_firstempty);
   fChain->SetBranchAddress("trig_EF_ph_EF_g5_nocut_unpaired", &trig_EF_ph_EF_g5_nocut_unpaired, &b_trig_EF_ph_EF_g5_nocut_unpaired);
   fChain->SetBranchAddress("trig_EF_ph_EF_g6_loose", &trig_EF_ph_EF_g6_loose, &b_trig_EF_ph_EF_g6_loose);
   fChain->SetBranchAddress("trig_Nav_n", &trig_Nav_n, &b_trig_Nav_n);
   fChain->SetBranchAddress("trig_Nav_chain_ChainId", &trig_Nav_chain_ChainId, &b_trig_Nav_chain_ChainId);
   fChain->SetBranchAddress("trig_Nav_chain_RoIType", &trig_Nav_chain_RoIType, &b_trig_Nav_chain_RoIType);
   fChain->SetBranchAddress("trig_Nav_chain_RoIIndex", &trig_Nav_chain_RoIIndex, &b_trig_Nav_chain_RoIIndex);
   fChain->SetBranchAddress("trig_RoI_L2_e_n", &trig_RoI_L2_e_n, &b_trig_RoI_L2_e_n);
   fChain->SetBranchAddress("trig_RoI_L2_e_type", &trig_RoI_L2_e_type, &b_trig_RoI_L2_e_type);
   fChain->SetBranchAddress("trig_RoI_L2_e_lastStep", &trig_RoI_L2_e_lastStep, &b_trig_RoI_L2_e_lastStep);
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
   fChain->SetBranchAddress("trig_RoI_EF_e_n", &trig_RoI_EF_e_n, &b_trig_RoI_EF_e_n);
   fChain->SetBranchAddress("trig_RoI_EF_e_type", &trig_RoI_EF_e_type, &b_trig_RoI_EF_e_type);
   fChain->SetBranchAddress("trig_RoI_EF_e_lastStep", &trig_RoI_EF_e_lastStep, &b_trig_RoI_EF_e_lastStep);
   fChain->SetBranchAddress("trig_RoI_EF_e_EmTau_ROI", &trig_RoI_EF_e_EmTau_ROI, &b_trig_RoI_EF_e_EmTau_ROI);
   fChain->SetBranchAddress("trig_RoI_EF_e_EmTau_ROIStatus", &trig_RoI_EF_e_EmTau_ROIStatus, &b_trig_RoI_EF_e_EmTau_ROIStatus);
   fChain->SetBranchAddress("trig_L1_mu_n", &trig_L1_mu_n, &b_trig_L1_mu_n);
   fChain->SetBranchAddress("trig_L1_mu_pt", &trig_L1_mu_pt, &b_trig_L1_mu_pt);
   fChain->SetBranchAddress("trig_L1_mu_eta", &trig_L1_mu_eta, &b_trig_L1_mu_eta);
   fChain->SetBranchAddress("trig_L1_mu_phi", &trig_L1_mu_phi, &b_trig_L1_mu_phi);
   fChain->SetBranchAddress("trig_L1_mu_thrName", &trig_L1_mu_thrName, &b_trig_L1_mu_thrName);
   fChain->SetBranchAddress("trig_L1_mu_thrNumber", &trig_L1_mu_thrNumber, &b_trig_L1_mu_thrNumber);
   fChain->SetBranchAddress("trig_L1_mu_RoINumber", &trig_L1_mu_RoINumber, &b_trig_L1_mu_RoINumber);
   fChain->SetBranchAddress("trig_L1_mu_sectorAddress", &trig_L1_mu_sectorAddress, &b_trig_L1_mu_sectorAddress);
   fChain->SetBranchAddress("trig_L1_mu_firstCandidate", &trig_L1_mu_firstCandidate, &b_trig_L1_mu_firstCandidate);
   fChain->SetBranchAddress("trig_L1_mu_moreCandInRoI", &trig_L1_mu_moreCandInRoI, &b_trig_L1_mu_moreCandInRoI);
   fChain->SetBranchAddress("trig_L1_mu_moreCandInSector", &trig_L1_mu_moreCandInSector, &b_trig_L1_mu_moreCandInSector);
   fChain->SetBranchAddress("trig_L1_mu_source", &trig_L1_mu_source, &b_trig_L1_mu_source);
   fChain->SetBranchAddress("trig_L1_mu_hemisphere", &trig_L1_mu_hemisphere, &b_trig_L1_mu_hemisphere);
   fChain->SetBranchAddress("trig_L1_mu_RoIWord", &trig_L1_mu_RoIWord, &b_trig_L1_mu_RoIWord);
   fChain->SetBranchAddress("trig_l2muonfeature_n", &trig_l2muonfeature_n, &b_trig_l2muonfeature_n);
   fChain->SetBranchAddress("trig_l2muonfeature_pt", &trig_l2muonfeature_pt, &b_trig_l2muonfeature_pt);
   fChain->SetBranchAddress("trig_l2muonfeature_eta", &trig_l2muonfeature_eta, &b_trig_l2muonfeature_eta);
   fChain->SetBranchAddress("trig_l2muonfeature_phi", &trig_l2muonfeature_phi, &b_trig_l2muonfeature_phi);
   fChain->SetBranchAddress("trig_l2muonfeature_dir_phi", &trig_l2muonfeature_dir_phi, &b_trig_l2muonfeature_dir_phi);
   fChain->SetBranchAddress("trig_l2muonfeature_zeta", &trig_l2muonfeature_zeta, &b_trig_l2muonfeature_zeta);
   fChain->SetBranchAddress("trig_l2muonfeature_dir_zeta", &trig_l2muonfeature_dir_zeta, &b_trig_l2muonfeature_dir_zeta);
   fChain->SetBranchAddress("trig_l2muonfeature_roiId", &trig_l2muonfeature_roiId, &b_trig_l2muonfeature_roiId);
   fChain->SetBranchAddress("trig_l2muonfeature_saddress", &trig_l2muonfeature_saddress, &b_trig_l2muonfeature_saddress);
   fChain->SetBranchAddress("trig_l2muonfeature_radius", &trig_l2muonfeature_radius, &b_trig_l2muonfeature_radius);
   fChain->SetBranchAddress("trig_l2muonfeature_beta", &trig_l2muonfeature_beta, &b_trig_l2muonfeature_beta);
   fChain->SetBranchAddress("trig_l2muonfeature_sp1_r", &trig_l2muonfeature_sp1_r, &b_trig_l2muonfeature_sp1_r);
   fChain->SetBranchAddress("trig_l2muonfeature_sp1_z", &trig_l2muonfeature_sp1_z, &b_trig_l2muonfeature_sp1_z);
   fChain->SetBranchAddress("trig_l2muonfeature_sp1_slope", &trig_l2muonfeature_sp1_slope, &b_trig_l2muonfeature_sp1_slope);
   fChain->SetBranchAddress("trig_l2muonfeature_sp2_r", &trig_l2muonfeature_sp2_r, &b_trig_l2muonfeature_sp2_r);
   fChain->SetBranchAddress("trig_l2muonfeature_sp2_z", &trig_l2muonfeature_sp2_z, &b_trig_l2muonfeature_sp2_z);
   fChain->SetBranchAddress("trig_l2muonfeature_sp2_slope", &trig_l2muonfeature_sp2_slope, &b_trig_l2muonfeature_sp2_slope);
   fChain->SetBranchAddress("trig_l2muonfeature_sp3_r", &trig_l2muonfeature_sp3_r, &b_trig_l2muonfeature_sp3_r);
   fChain->SetBranchAddress("trig_l2muonfeature_sp3_z", &trig_l2muonfeature_sp3_z, &b_trig_l2muonfeature_sp3_z);
   fChain->SetBranchAddress("trig_l2muonfeature_sp3_slope", &trig_l2muonfeature_sp3_slope, &b_trig_l2muonfeature_sp3_slope);
   fChain->SetBranchAddress("trig_l2muonfeature_br_radius", &trig_l2muonfeature_br_radius, &b_trig_l2muonfeature_br_radius);
   fChain->SetBranchAddress("trig_l2muonfeature_br_sagitta", &trig_l2muonfeature_br_sagitta, &b_trig_l2muonfeature_br_sagitta);
   fChain->SetBranchAddress("trig_l2muonfeature_ec_alpha", &trig_l2muonfeature_ec_alpha, &b_trig_l2muonfeature_ec_alpha);
   fChain->SetBranchAddress("trig_l2muonfeature_ec_beta", &trig_l2muonfeature_ec_beta, &b_trig_l2muonfeature_ec_beta);
   fChain->SetBranchAddress("trig_l2muonfeature_dq_var1", &trig_l2muonfeature_dq_var1, &b_trig_l2muonfeature_dq_var1);
   fChain->SetBranchAddress("trig_l2muonfeature_dq_var2", &trig_l2muonfeature_dq_var2, &b_trig_l2muonfeature_dq_var2);
   fChain->SetBranchAddress("trig_l2combmuonfeature_n", &trig_l2combmuonfeature_n, &b_trig_l2combmuonfeature_n);
   fChain->SetBranchAddress("trig_l2combmuonfeature_sigma_pt", &trig_l2combmuonfeature_sigma_pt, &b_trig_l2combmuonfeature_sigma_pt);
   fChain->SetBranchAddress("trig_l2combmuonfeature_pt", &trig_l2combmuonfeature_pt, &b_trig_l2combmuonfeature_pt);
   fChain->SetBranchAddress("trig_l2combmuonfeature_eta", &trig_l2combmuonfeature_eta, &b_trig_l2combmuonfeature_eta);
   fChain->SetBranchAddress("trig_l2combmuonfeature_phi", &trig_l2combmuonfeature_phi, &b_trig_l2combmuonfeature_phi);
   fChain->SetBranchAddress("trig_l2combmuonfeature_mf_pt", &trig_l2combmuonfeature_mf_pt, &b_trig_l2combmuonfeature_mf_pt);
   fChain->SetBranchAddress("trig_l2combmuonfeature_mf_eta", &trig_l2combmuonfeature_mf_eta, &b_trig_l2combmuonfeature_mf_eta);
   fChain->SetBranchAddress("trig_l2combmuonfeature_mf_phi", &trig_l2combmuonfeature_mf_phi, &b_trig_l2combmuonfeature_mf_phi);
   fChain->SetBranchAddress("trig_l2combmuonfeature_mf_dir_phi", &trig_l2combmuonfeature_mf_dir_phi, &b_trig_l2combmuonfeature_mf_dir_phi);
   fChain->SetBranchAddress("trig_l2combmuonfeature_mf_zeta", &trig_l2combmuonfeature_mf_zeta, &b_trig_l2combmuonfeature_mf_zeta);
   fChain->SetBranchAddress("trig_l2combmuonfeature_mf_dir_zeta", &trig_l2combmuonfeature_mf_dir_zeta, &b_trig_l2combmuonfeature_mf_dir_zeta);
   fChain->SetBranchAddress("trig_l2combmuonfeature_mf_hasmf", &trig_l2combmuonfeature_mf_hasmf, &b_trig_l2combmuonfeature_mf_hasmf);
   fChain->SetBranchAddress("trig_l2isomuonfeature_n", &trig_l2isomuonfeature_n, &b_trig_l2isomuonfeature_n);
   fChain->SetBranchAddress("trig_l2isomuonfeature_EtInnerConeEC ", &trig_l2isomuonfeature_EtInnerConeEC , &b_trig_l2isomuonfeature_EtInnerConeEC );
   fChain->SetBranchAddress("trig_l2isomuonfeature_EtOuterConeEC", &trig_l2isomuonfeature_EtOuterConeEC, &b_trig_l2isomuonfeature_EtOuterConeEC);
   fChain->SetBranchAddress("trig_l2isomuonfeature_EtInnerConeHC", &trig_l2isomuonfeature_EtInnerConeHC, &b_trig_l2isomuonfeature_EtInnerConeHC);
   fChain->SetBranchAddress("trig_l2isomuonfeature_EtOuterConeHC", &trig_l2isomuonfeature_EtOuterConeHC, &b_trig_l2isomuonfeature_EtOuterConeHC);
   fChain->SetBranchAddress("trig_l2isomuonfeature_NTracksCone", &trig_l2isomuonfeature_NTracksCone, &b_trig_l2isomuonfeature_NTracksCone);
   fChain->SetBranchAddress("trig_l2isomuonfeature_SumPtTracksCone", &trig_l2isomuonfeature_SumPtTracksCone, &b_trig_l2isomuonfeature_SumPtTracksCone);
   fChain->SetBranchAddress("trig_l2isomuonfeature_PtMuTracksCone", &trig_l2isomuonfeature_PtMuTracksCone, &b_trig_l2isomuonfeature_PtMuTracksCone);
   fChain->SetBranchAddress("trig_l2isomuonfeature_iso_strategy", &trig_l2isomuonfeature_iso_strategy, &b_trig_l2isomuonfeature_iso_strategy);
   fChain->SetBranchAddress("trig_l2isomuonfeature_RoiIdMu ", &trig_l2isomuonfeature_RoiIdMu , &b_trig_l2isomuonfeature_RoiIdMu );
   fChain->SetBranchAddress("trig_l2isomuonfeature_PtMu", &trig_l2isomuonfeature_PtMu, &b_trig_l2isomuonfeature_PtMu);
   fChain->SetBranchAddress("trig_l2isomuonfeature_EtaMu", &trig_l2isomuonfeature_EtaMu, &b_trig_l2isomuonfeature_EtaMu);
   fChain->SetBranchAddress("trig_l2isomuonfeature_PhiMu", &trig_l2isomuonfeature_PhiMu, &b_trig_l2isomuonfeature_PhiMu);
   fChain->SetBranchAddress("trig_l2isomuonfeature_LArWeight", &trig_l2isomuonfeature_LArWeight, &b_trig_l2isomuonfeature_LArWeight);
   fChain->SetBranchAddress("trig_l2isomuonfeature_TileWeight", &trig_l2isomuonfeature_TileWeight, &b_trig_l2isomuonfeature_TileWeight);
   fChain->SetBranchAddress("trig_l2isomuonfeature_pt", &trig_l2isomuonfeature_pt, &b_trig_l2isomuonfeature_pt);
   fChain->SetBranchAddress("trig_l2isomuonfeature_eta", &trig_l2isomuonfeature_eta, &b_trig_l2isomuonfeature_eta);
   fChain->SetBranchAddress("trig_l2isomuonfeature_phi", &trig_l2isomuonfeature_phi, &b_trig_l2isomuonfeature_phi);
   fChain->SetBranchAddress("trig_l2tilemufeature_n", &trig_l2tilemufeature_n, &b_trig_l2tilemufeature_n);
   fChain->SetBranchAddress("trig_l2tilemufeature_eta", &trig_l2tilemufeature_eta, &b_trig_l2tilemufeature_eta);
   fChain->SetBranchAddress("trig_l2tilemufeature_phi", &trig_l2tilemufeature_phi, &b_trig_l2tilemufeature_phi);
   fChain->SetBranchAddress("trig_l2tilemufeature_enedep", &trig_l2tilemufeature_enedep, &b_trig_l2tilemufeature_enedep);
   fChain->SetBranchAddress("trig_l2tilemufeature_quality", &trig_l2tilemufeature_quality, &b_trig_l2tilemufeature_quality);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_n", &trig_l2tiletrackmufeature_n, &b_trig_l2tiletrackmufeature_n);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_PtTR_Trk", &trig_l2tiletrackmufeature_PtTR_Trk, &b_trig_l2tiletrackmufeature_PtTR_Trk);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_EtaTR_Trk", &trig_l2tiletrackmufeature_EtaTR_Trk, &b_trig_l2tiletrackmufeature_EtaTR_Trk);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_PhiTR_Trk", &trig_l2tiletrackmufeature_PhiTR_Trk, &b_trig_l2tiletrackmufeature_PhiTR_Trk);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_Typ_IDTrk", &trig_l2tiletrackmufeature_Typ_IDTrk, &b_trig_l2tiletrackmufeature_Typ_IDTrk);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_pt", &trig_l2tiletrackmufeature_pt, &b_trig_l2tiletrackmufeature_pt);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_eta", &trig_l2tiletrackmufeature_eta, &b_trig_l2tiletrackmufeature_eta);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_phi", &trig_l2tiletrackmufeature_phi, &b_trig_l2tiletrackmufeature_phi);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_tilemu_eta", &trig_l2tiletrackmufeature_tilemu_eta, &b_trig_l2tiletrackmufeature_tilemu_eta);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_tilemu_phi", &trig_l2tiletrackmufeature_tilemu_phi, &b_trig_l2tiletrackmufeature_tilemu_phi);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_tilemu_enedep", &trig_l2tiletrackmufeature_tilemu_enedep, &b_trig_l2tiletrackmufeature_tilemu_enedep);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_tilemu_quality", &trig_l2tiletrackmufeature_tilemu_quality, &b_trig_l2tiletrackmufeature_tilemu_quality);
   fChain->SetBranchAddress("trig_l2tiletrackmufeature_tilemu_hastilemu", &trig_l2tiletrackmufeature_tilemu_hastilemu, &b_trig_l2tiletrackmufeature_tilemu_hastilemu);
   fChain->SetBranchAddress("trig_EF_tau_n", &trig_EF_tau_n, &b_trig_EF_tau_n);
   fChain->SetBranchAddress("trig_EF_tau_Et", &trig_EF_tau_Et, &b_trig_EF_tau_Et);
   fChain->SetBranchAddress("trig_EF_tau_pt", &trig_EF_tau_pt, &b_trig_EF_tau_pt);
   fChain->SetBranchAddress("trig_EF_tau_m", &trig_EF_tau_m, &b_trig_EF_tau_m);
   fChain->SetBranchAddress("trig_EF_tau_eta", &trig_EF_tau_eta, &b_trig_EF_tau_eta);
   fChain->SetBranchAddress("trig_EF_tau_phi", &trig_EF_tau_phi, &b_trig_EF_tau_phi);
   fChain->SetBranchAddress("trig_EF_tau_px", &trig_EF_tau_px, &b_trig_EF_tau_px);
   fChain->SetBranchAddress("trig_EF_tau_py", &trig_EF_tau_py, &b_trig_EF_tau_py);
   fChain->SetBranchAddress("trig_EF_tau_pz", &trig_EF_tau_pz, &b_trig_EF_tau_pz);
   fChain->SetBranchAddress("trig_EF_tau_RoIword", &trig_EF_tau_RoIword, &b_trig_EF_tau_RoIword);
   fChain->SetBranchAddress("trig_EF_tau_nLooseTrk", &trig_EF_tau_nLooseTrk, &b_trig_EF_tau_nLooseTrk);
   fChain->SetBranchAddress("trig_EF_tau_leadLooseTrkPt", &trig_EF_tau_leadLooseTrkPt, &b_trig_EF_tau_leadLooseTrkPt);
   fChain->SetBranchAddress("trig_EF_tau_leadLooseTrkEta", &trig_EF_tau_leadLooseTrkEta, &b_trig_EF_tau_leadLooseTrkEta);
   fChain->SetBranchAddress("trig_EF_tau_leadLooseTrkPhi", &trig_EF_tau_leadLooseTrkPhi, &b_trig_EF_tau_leadLooseTrkPhi);
   fChain->SetBranchAddress("trig_EF_tau_seedCalo_EMRadius", &trig_EF_tau_seedCalo_EMRadius, &b_trig_EF_tau_seedCalo_EMRadius);
   fChain->SetBranchAddress("trig_EF_tau_seedCalo_hadRadius", &trig_EF_tau_seedCalo_hadRadius, &b_trig_EF_tau_seedCalo_hadRadius);
   fChain->SetBranchAddress("trig_EF_tau_seedCalo_etEMAtEMScale", &trig_EF_tau_seedCalo_etEMAtEMScale, &b_trig_EF_tau_seedCalo_etEMAtEMScale);
   fChain->SetBranchAddress("trig_EF_tau_seedCalo_etHadAtEMScale", &trig_EF_tau_seedCalo_etHadAtEMScale, &b_trig_EF_tau_seedCalo_etHadAtEMScale);
   fChain->SetBranchAddress("trig_EF_tau_seedCalo_isolFrac", &trig_EF_tau_seedCalo_isolFrac, &b_trig_EF_tau_seedCalo_isolFrac);
   fChain->SetBranchAddress("trig_EF_tau_seedCalo_centFrac", &trig_EF_tau_seedCalo_centFrac, &b_trig_EF_tau_seedCalo_centFrac);
   fChain->SetBranchAddress("trig_EF_tau_seedCalo_etEMCalib", &trig_EF_tau_seedCalo_etEMCalib, &b_trig_EF_tau_seedCalo_etEMCalib);
   fChain->SetBranchAddress("trig_EF_tau_seedCalo_etHadCalib", &trig_EF_tau_seedCalo_etHadCalib, &b_trig_EF_tau_seedCalo_etHadCalib);
   fChain->SetBranchAddress("trig_EF_tau_seedCalo_eta", &trig_EF_tau_seedCalo_eta, &b_trig_EF_tau_seedCalo_eta);
   fChain->SetBranchAddress("trig_EF_tau_seedCalo_phi", &trig_EF_tau_seedCalo_phi, &b_trig_EF_tau_seedCalo_phi);
   fChain->SetBranchAddress("trig_EF_tau_invMassOfTrks", &trig_EF_tau_invMassOfTrks, &b_trig_EF_tau_invMassOfTrks);
   fChain->SetBranchAddress("trig_EF_tau_looseTrkWidth", &trig_EF_tau_looseTrkWidth, &b_trig_EF_tau_looseTrkWidth);
   fChain->SetBranchAddress("trig_EF_tau_looseTrkRadius", &trig_EF_tau_looseTrkRadius, &b_trig_EF_tau_looseTrkRadius);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau12_loose", &trig_EF_tau_EF_tau12_loose, &b_trig_EF_tau_EF_tau12_loose);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau12_loose_PT", &trig_EF_tau_EF_tau12_loose_PT, &b_trig_EF_tau_EF_tau12_loose_PT);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau12_loose_e10_loose", &trig_EF_tau_EF_tau12_loose_e10_loose, &b_trig_EF_tau_EF_tau12_loose_e10_loose);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau12_loose_mu10", &trig_EF_tau_EF_tau12_loose_mu10, &b_trig_EF_tau_EF_tau12_loose_mu10);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau12_loose_xe20", &trig_EF_tau_EF_tau12_loose_xe20, &b_trig_EF_tau_EF_tau12_loose_xe20);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau16_loose", &trig_EF_tau_EF_tau16_loose, &b_trig_EF_tau_EF_tau16_loose);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau16_loose_3j40", &trig_EF_tau_EF_tau16_loose_3j40, &b_trig_EF_tau_EF_tau16_loose_3j40);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau16_loose_TAU8", &trig_EF_tau_EF_tau16_loose_TAU8, &b_trig_EF_tau_EF_tau16_loose_TAU8);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau16_loose_xe20", &trig_EF_tau_EF_tau16_loose_xe20, &b_trig_EF_tau_EF_tau16_loose_xe20);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau16i_NoIDTrkCut", &trig_EF_tau_EF_tau16i_NoIDTrkCut, &b_trig_EF_tau_EF_tau16i_NoIDTrkCut);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau16i_loose", &trig_EF_tau_EF_tau16i_loose, &b_trig_EF_tau_EF_tau16i_loose);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau20_loose", &trig_EF_tau_EF_tau20_loose, &b_trig_EF_tau_EF_tau20_loose);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau20_medium", &trig_EF_tau_EF_tau20_medium, &b_trig_EF_tau_EF_tau20_medium);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau29_loose", &trig_EF_tau_EF_tau29_loose, &b_trig_EF_tau_EF_tau29_loose);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau29_medium", &trig_EF_tau_EF_tau29_medium, &b_trig_EF_tau_EF_tau29_medium);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau29i_loose", &trig_EF_tau_EF_tau29i_loose, &b_trig_EF_tau_EF_tau29i_loose);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau50_loose", &trig_EF_tau_EF_tau50_loose, &b_trig_EF_tau_EF_tau50_loose);
   fChain->SetBranchAddress("trig_EF_tau_EF_tau50_loose_TAU30", &trig_EF_tau_EF_tau50_loose_TAU30, &b_trig_EF_tau_EF_tau50_loose_TAU30);
   fChain->SetBranchAddress("trig_EF_tau_EF_tauNoCut", &trig_EF_tau_EF_tauNoCut, &b_trig_EF_tau_EF_tauNoCut);
   fChain->SetBranchAddress("trig_EF_tau_EF_tauNoCut_NoIDTrkCut", &trig_EF_tau_EF_tauNoCut_NoIDTrkCut, &b_trig_EF_tau_EF_tauNoCut_NoIDTrkCut);
   fChain->SetBranchAddress("trig_EF_tau_EF_tauNoCut_SiTrk", &trig_EF_tau_EF_tauNoCut_SiTrk, &b_trig_EF_tau_EF_tauNoCut_SiTrk);
   fChain->SetBranchAddress("trig_EF_tau_EF_tauNoCut_cells", &trig_EF_tau_EF_tauNoCut_cells, &b_trig_EF_tau_EF_tauNoCut_cells);
   fChain->SetBranchAddress("trig_EF_tau_EF_tauNoCut_cosmic", &trig_EF_tau_EF_tauNoCut_cosmic, &b_trig_EF_tau_EF_tauNoCut_cosmic);
   fChain->SetBranchAddress("trig_EF_tau_EF_tauNoCut_firstempty", &trig_EF_tau_EF_tauNoCut_firstempty, &b_trig_EF_tau_EF_tauNoCut_firstempty);
   fChain->SetBranchAddress("trig_EF_tau_EF_tauNoCut_hasTrk", &trig_EF_tau_EF_tauNoCut_hasTrk, &b_trig_EF_tau_EF_tauNoCut_hasTrk);
   fChain->SetBranchAddress("trig_EF_tau_EF_tauNoCut_hasTrk_MV", &trig_EF_tau_EF_tauNoCut_hasTrk_MV, &b_trig_EF_tau_EF_tauNoCut_hasTrk_MV);
   fChain->SetBranchAddress("trig_EF_tau_EF_tauNoCut_hasTrk_SiTrk", &trig_EF_tau_EF_tauNoCut_hasTrk_SiTrk, &b_trig_EF_tau_EF_tauNoCut_hasTrk_SiTrk);
   fChain->SetBranchAddress("trig_EF_tau_EF_tauNoCut_hasTrk_xe20", &trig_EF_tau_EF_tauNoCut_hasTrk_xe20, &b_trig_EF_tau_EF_tauNoCut_hasTrk_xe20);
   fChain->SetBranchAddress("trig_EF_tau_EF_tauNoCut_unpaired", &trig_EF_tau_EF_tauNoCut_unpaired, &b_trig_EF_tau_EF_tauNoCut_unpaired);
   fChain->SetBranchAddress("trig_L2_tau_n", &trig_L2_tau_n, &b_trig_L2_tau_n);
   fChain->SetBranchAddress("trig_L2_tau_pt", &trig_L2_tau_pt, &b_trig_L2_tau_pt);
   fChain->SetBranchAddress("trig_L2_tau_eta", &trig_L2_tau_eta, &b_trig_L2_tau_eta);
   fChain->SetBranchAddress("trig_L2_tau_phi", &trig_L2_tau_phi, &b_trig_L2_tau_phi);
   fChain->SetBranchAddress("trig_L2_tau_px", &trig_L2_tau_px, &b_trig_L2_tau_px);
   fChain->SetBranchAddress("trig_L2_tau_py", &trig_L2_tau_py, &b_trig_L2_tau_py);
   fChain->SetBranchAddress("trig_L2_tau_pz", &trig_L2_tau_pz, &b_trig_L2_tau_pz);
   fChain->SetBranchAddress("trig_L2_tau_RoIword", &trig_L2_tau_RoIword, &b_trig_L2_tau_RoIword);
   fChain->SetBranchAddress("trig_L2_tau_simpleEtFlow", &trig_L2_tau_simpleEtFlow, &b_trig_L2_tau_simpleEtFlow);
   fChain->SetBranchAddress("trig_L2_tau_nMatchedTracks", &trig_L2_tau_nMatchedTracks, &b_trig_L2_tau_nMatchedTracks);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau12_loose", &trig_L2_tau_L2_tau12_loose, &b_trig_L2_tau_L2_tau12_loose);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau12_loose_PT", &trig_L2_tau_L2_tau12_loose_PT, &b_trig_L2_tau_L2_tau12_loose_PT);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau12_loose_e10_loose", &trig_L2_tau_L2_tau12_loose_e10_loose, &b_trig_L2_tau_L2_tau12_loose_e10_loose);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau12_loose_mu10", &trig_L2_tau_L2_tau12_loose_mu10, &b_trig_L2_tau_L2_tau12_loose_mu10);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau12_loose_xe20", &trig_L2_tau_L2_tau12_loose_xe20, &b_trig_L2_tau_L2_tau12_loose_xe20);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau16_loose", &trig_L2_tau_L2_tau16_loose, &b_trig_L2_tau_L2_tau16_loose);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau16_loose_3j30", &trig_L2_tau_L2_tau16_loose_3j30, &b_trig_L2_tau_L2_tau16_loose_3j30);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau16_loose_TAU8", &trig_L2_tau_L2_tau16_loose_TAU8, &b_trig_L2_tau_L2_tau16_loose_TAU8);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau16_loose_xe20", &trig_L2_tau_L2_tau16_loose_xe20, &b_trig_L2_tau_L2_tau16_loose_xe20);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau16i_NoIDTrkCut", &trig_L2_tau_L2_tau16i_NoIDTrkCut, &b_trig_L2_tau_L2_tau16i_NoIDTrkCut);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau16i_loose", &trig_L2_tau_L2_tau16i_loose, &b_trig_L2_tau_L2_tau16i_loose);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau20_loose", &trig_L2_tau_L2_tau20_loose, &b_trig_L2_tau_L2_tau20_loose);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau20_medium", &trig_L2_tau_L2_tau20_medium, &b_trig_L2_tau_L2_tau20_medium);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau29_loose", &trig_L2_tau_L2_tau29_loose, &b_trig_L2_tau_L2_tau29_loose);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau29_medium", &trig_L2_tau_L2_tau29_medium, &b_trig_L2_tau_L2_tau29_medium);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau29i_loose", &trig_L2_tau_L2_tau29i_loose, &b_trig_L2_tau_L2_tau29i_loose);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau50_loose", &trig_L2_tau_L2_tau50_loose, &b_trig_L2_tau_L2_tau50_loose);
   fChain->SetBranchAddress("trig_L2_tau_L2_tau50_loose_TAU30", &trig_L2_tau_L2_tau50_loose_TAU30, &b_trig_L2_tau_L2_tau50_loose_TAU30);
   fChain->SetBranchAddress("trig_L2_tau_L2_tauNoCut", &trig_L2_tau_L2_tauNoCut, &b_trig_L2_tau_L2_tauNoCut);
   fChain->SetBranchAddress("trig_L2_tau_L2_tauNoCut_NoIDTrkCut", &trig_L2_tau_L2_tauNoCut_NoIDTrkCut, &b_trig_L2_tau_L2_tauNoCut_NoIDTrkCut);
   fChain->SetBranchAddress("trig_L2_tau_L2_tauNoCut_SiTrk", &trig_L2_tau_L2_tauNoCut_SiTrk, &b_trig_L2_tau_L2_tauNoCut_SiTrk);
   fChain->SetBranchAddress("trig_L2_tau_L2_tauNoCut_cells", &trig_L2_tau_L2_tauNoCut_cells, &b_trig_L2_tau_L2_tauNoCut_cells);
   fChain->SetBranchAddress("trig_L2_tau_L2_tauNoCut_cosmic", &trig_L2_tau_L2_tauNoCut_cosmic, &b_trig_L2_tau_L2_tauNoCut_cosmic);
   fChain->SetBranchAddress("trig_L2_tau_L2_tauNoCut_firstempty", &trig_L2_tau_L2_tauNoCut_firstempty, &b_trig_L2_tau_L2_tauNoCut_firstempty);
   fChain->SetBranchAddress("trig_L2_tau_L2_tauNoCut_hasTrk", &trig_L2_tau_L2_tauNoCut_hasTrk, &b_trig_L2_tau_L2_tauNoCut_hasTrk);
   fChain->SetBranchAddress("trig_L2_tau_L2_tauNoCut_hasTrk_MV", &trig_L2_tau_L2_tauNoCut_hasTrk_MV, &b_trig_L2_tau_L2_tauNoCut_hasTrk_MV);
   fChain->SetBranchAddress("trig_L2_tau_L2_tauNoCut_hasTrk_SiTrk", &trig_L2_tau_L2_tauNoCut_hasTrk_SiTrk, &b_trig_L2_tau_L2_tauNoCut_hasTrk_SiTrk);
   fChain->SetBranchAddress("trig_L2_tau_L2_tauNoCut_hasTrk_xe20", &trig_L2_tau_L2_tauNoCut_hasTrk_xe20, &b_trig_L2_tau_L2_tauNoCut_hasTrk_xe20);
   fChain->SetBranchAddress("trig_L2_tau_L2_tauNoCut_unpaired", &trig_L2_tau_L2_tauNoCut_unpaired, &b_trig_L2_tau_L2_tauNoCut_unpaired);
   fChain->SetBranchAddress("trig_L2_tau_cluster_quality", &trig_L2_tau_cluster_quality, &b_trig_L2_tau_cluster_quality);
   fChain->SetBranchAddress("trig_L2_tau_cluster_EMenergy", &trig_L2_tau_cluster_EMenergy, &b_trig_L2_tau_cluster_EMenergy);
   fChain->SetBranchAddress("trig_L2_tau_cluster_HADenergy", &trig_L2_tau_cluster_HADenergy, &b_trig_L2_tau_cluster_HADenergy);
   fChain->SetBranchAddress("trig_L2_tau_cluster_eta", &trig_L2_tau_cluster_eta, &b_trig_L2_tau_cluster_eta);
   fChain->SetBranchAddress("trig_L2_tau_cluster_phi", &trig_L2_tau_cluster_phi, &b_trig_L2_tau_cluster_phi);
   fChain->SetBranchAddress("trig_L2_tau_cluster_EMRadius2", &trig_L2_tau_cluster_EMRadius2, &b_trig_L2_tau_cluster_EMRadius2);
   fChain->SetBranchAddress("trig_L2_tau_cluster_CaloRadius", &trig_L2_tau_cluster_CaloRadius, &b_trig_L2_tau_cluster_CaloRadius);
   fChain->SetBranchAddress("trig_L2_tau_cluster_IsoFrac", &trig_L2_tau_cluster_IsoFrac, &b_trig_L2_tau_cluster_IsoFrac);
   fChain->SetBranchAddress("trig_L2_tau_cluster_numTotCells", &trig_L2_tau_cluster_numTotCells, &b_trig_L2_tau_cluster_numTotCells);
   fChain->SetBranchAddress("trig_L2_tau_cluster_stripWidth", &trig_L2_tau_cluster_stripWidth, &b_trig_L2_tau_cluster_stripWidth);
   fChain->SetBranchAddress("trig_L2_tau_cluster_stripWidthOffline", &trig_L2_tau_cluster_stripWidthOffline, &b_trig_L2_tau_cluster_stripWidthOffline);
   fChain->SetBranchAddress("trig_L2_tau_cluster_EMenergyNor", &trig_L2_tau_cluster_EMenergyNor, &b_trig_L2_tau_cluster_EMenergyNor);
   fChain->SetBranchAddress("trig_L2_tau_cluster_EMenergyNar", &trig_L2_tau_cluster_EMenergyNar, &b_trig_L2_tau_cluster_EMenergyNar);
   fChain->SetBranchAddress("trig_L2_tau_cluster_HADenergyNor", &trig_L2_tau_cluster_HADenergyNor, &b_trig_L2_tau_cluster_HADenergyNor);
   fChain->SetBranchAddress("trig_L2_tau_cluster_HADenergyNar", &trig_L2_tau_cluster_HADenergyNar, &b_trig_L2_tau_cluster_HADenergyNar);
   fChain->SetBranchAddress("trig_L2_tau_cluster_etNar", &trig_L2_tau_cluster_etNar, &b_trig_L2_tau_cluster_etNar);
   fChain->SetBranchAddress("trig_L2_tau_tracksinfo_nCoreTracks", &trig_L2_tau_tracksinfo_nCoreTracks, &b_trig_L2_tau_tracksinfo_nCoreTracks);
   fChain->SetBranchAddress("trig_L2_tau_tracksinfo_nSlowTracks", &trig_L2_tau_tracksinfo_nSlowTracks, &b_trig_L2_tau_tracksinfo_nSlowTracks);
   fChain->SetBranchAddress("trig_L2_tau_tracksinfo_nIsoTracks", &trig_L2_tau_tracksinfo_nIsoTracks, &b_trig_L2_tau_tracksinfo_nIsoTracks);
   fChain->SetBranchAddress("trig_L2_tau_tracksinfo_charge", &trig_L2_tau_tracksinfo_charge, &b_trig_L2_tau_tracksinfo_charge);
   fChain->SetBranchAddress("trig_L2_tau_tracksinfo_leadingTrackPt", &trig_L2_tau_tracksinfo_leadingTrackPt, &b_trig_L2_tau_tracksinfo_leadingTrackPt);
   fChain->SetBranchAddress("trig_L2_tau_tracksinfo_scalarPtSumCore", &trig_L2_tau_tracksinfo_scalarPtSumCore, &b_trig_L2_tau_tracksinfo_scalarPtSumCore);
   fChain->SetBranchAddress("trig_L2_tau_tracksinfo_scalarPtSumIso", &trig_L2_tau_tracksinfo_scalarPtSumIso, &b_trig_L2_tau_tracksinfo_scalarPtSumIso);
   fChain->SetBranchAddress("trig_L2_tau_tracksinfo_3fastest_pt", &trig_L2_tau_tracksinfo_3fastest_pt, &b_trig_L2_tau_tracksinfo_3fastest_pt);
   fChain->SetBranchAddress("trig_L2_tau_tracksinfo_3fastest_eta", &trig_L2_tau_tracksinfo_3fastest_eta, &b_trig_L2_tau_tracksinfo_3fastest_eta);
   fChain->SetBranchAddress("trig_L2_tau_tracksinfo_3fastest_phi", &trig_L2_tau_tracksinfo_3fastest_phi, &b_trig_L2_tau_tracksinfo_3fastest_phi);
   fChain->SetBranchAddress("trig_L2_tau_tracksinfo_3fastest_m", &trig_L2_tau_tracksinfo_3fastest_m, &b_trig_L2_tau_tracksinfo_3fastest_m);
   fChain->SetBranchAddress("trig_RoI_L2_tau_n", &trig_RoI_L2_tau_n, &b_trig_RoI_L2_tau_n);
   fChain->SetBranchAddress("trig_RoI_L2_tau_type", &trig_RoI_L2_tau_type, &b_trig_RoI_L2_tau_type);
   fChain->SetBranchAddress("trig_RoI_L2_tau_lastStep", &trig_RoI_L2_tau_lastStep, &b_trig_RoI_L2_tau_lastStep);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigRoiDescriptor", &trig_RoI_L2_tau_TrigRoiDescriptor, &b_trig_RoI_L2_tau_TrigRoiDescriptor);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigRoiDescriptorStatus", &trig_RoI_L2_tau_TrigRoiDescriptorStatus, &b_trig_RoI_L2_tau_TrigRoiDescriptorStatus);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigTauCluster", &trig_RoI_L2_tau_TrigTauCluster, &b_trig_RoI_L2_tau_TrigTauCluster);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigTauClusterStatus", &trig_RoI_L2_tau_TrigTauClusterStatus, &b_trig_RoI_L2_tau_TrigTauClusterStatus);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigTauClusterDetails", &trig_RoI_L2_tau_TrigTauClusterDetails, &b_trig_RoI_L2_tau_TrigTauClusterDetails);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigTauClusterDetailsStatus", &trig_RoI_L2_tau_TrigTauClusterDetailsStatus, &b_trig_RoI_L2_tau_TrigTauClusterDetailsStatus);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigTauTracksInfo", &trig_RoI_L2_tau_TrigTauTracksInfo, &b_trig_RoI_L2_tau_TrigTauTracksInfo);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigTauTracksInfoStatus", &trig_RoI_L2_tau_TrigTauTracksInfoStatus, &b_trig_RoI_L2_tau_TrigTauTracksInfoStatus);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigTau", &trig_RoI_L2_tau_TrigTau, &b_trig_RoI_L2_tau_TrigTau);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigTauStatus", &trig_RoI_L2_tau_TrigTauStatus, &b_trig_RoI_L2_tau_TrigTauStatus);
   fChain->SetBranchAddress("trig_RoI_L2_tau_EmTau_ROI", &trig_RoI_L2_tau_EmTau_ROI, &b_trig_RoI_L2_tau_EmTau_ROI);
   fChain->SetBranchAddress("trig_RoI_L2_tau_EmTau_ROIStatus", &trig_RoI_L2_tau_EmTau_ROIStatus, &b_trig_RoI_L2_tau_EmTau_ROIStatus);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigInDetTrackCollection", &trig_RoI_L2_tau_TrigInDetTrackCollection, &b_trig_RoI_L2_tau_TrigInDetTrackCollection);
   fChain->SetBranchAddress("trig_RoI_L2_tau_TrigInDetTrackCollectionStatus", &trig_RoI_L2_tau_TrigInDetTrackCollectionStatus, &b_trig_RoI_L2_tau_TrigInDetTrackCollectionStatus);
   fChain->SetBranchAddress("trig_RoI_EF_tau_n", &trig_RoI_EF_tau_n, &b_trig_RoI_EF_tau_n);
   fChain->SetBranchAddress("trig_RoI_EF_tau_type", &trig_RoI_EF_tau_type, &b_trig_RoI_EF_tau_type);
   fChain->SetBranchAddress("trig_RoI_EF_tau_lastStep", &trig_RoI_EF_tau_lastStep, &b_trig_RoI_EF_tau_lastStep);
   fChain->SetBranchAddress("trig_RoI_EF_tau_EmTau_ROI", &trig_RoI_EF_tau_EmTau_ROI, &b_trig_RoI_EF_tau_EmTau_ROI);
   fChain->SetBranchAddress("trig_RoI_EF_tau_EmTau_ROIStatus", &trig_RoI_EF_tau_EmTau_ROIStatus, &b_trig_RoI_EF_tau_EmTau_ROIStatus);
//    fChain->SetBranchAddress("trig_RoI_EF_tau_Analysis::TauJetContainer", &trig_RoI_EF_tau_Analysis__TauJetContainer, &b_trig_RoI_EF_tau_Analysis__TauJetContainer);
//    fChain->SetBranchAddress("trig_RoI_EF_tau_Analysis::TauJetContainerStatus", &trig_RoI_EF_tau_Analysis__TauJetContainerStatus, &b_trig_RoI_EF_tau_Analysis__TauJetContainerStatus);
   fChain->SetBranchAddress("trig_L1_TAV", &trig_L1_TAV, &b_trig_L1_TAV);
   fChain->SetBranchAddress("trig_L2_passedPhysics", &trig_L2_passedPhysics, &b_trig_L2_passedPhysics);
   fChain->SetBranchAddress("trig_EF_passedPhysics", &trig_EF_passedPhysics, &b_trig_EF_passedPhysics);
   fChain->SetBranchAddress("trig_L1_TBP", &trig_L1_TBP, &b_trig_L1_TBP);
   fChain->SetBranchAddress("trig_L1_TAP", &trig_L1_TAP, &b_trig_L1_TAP);
   fChain->SetBranchAddress("trig_L2_passedRaw", &trig_L2_passedRaw, &b_trig_L2_passedRaw);
   fChain->SetBranchAddress("trig_EF_passedRaw", &trig_EF_passedRaw, &b_trig_EF_passedRaw);
   fChain->SetBranchAddress("trig_L2_resurrected", &trig_L2_resurrected, &b_trig_L2_resurrected);
   fChain->SetBranchAddress("trig_EF_resurrected", &trig_EF_resurrected, &b_trig_EF_resurrected);
   fChain->SetBranchAddress("trig_L2_prescaled", &trig_L2_prescaled, &b_trig_L2_prescaled);
   fChain->SetBranchAddress("trig_EF_prescaled", &trig_EF_prescaled, &b_trig_EF_prescaled);
   Notify();
}

Bool_t physics::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void physics::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t physics::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef physics_cxx
