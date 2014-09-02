/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jun 29 19:02:44 2010 by ROOT version 5.27/02
// from TTree MinBiasTree/MinBiasTree
// found on file: VtxNtuple.root
//////////////////////////////////////////////////////////

#ifndef VtxNtuple_h
#define VtxNtuple_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>

class VtxNtuple {
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
   Int_t           mc_n;
   std::vector<float>   *mc_gen_pt;
   std::vector<float>   *mc_gen_eta;
   std::vector<float>   *mc_gen_phi;
   std::vector<float>   *mc_gen_energy;
   std::vector<int>     *mc_perigee_ok;
   std::vector<float>   *mc_perigee_d0;
   std::vector<float>   *mc_perigee_z0;
   std::vector<float>   *mc_perigee_phi;
   std::vector<float>   *mc_perigee_theta;
   std::vector<float>   *mc_perigee_qoverp;
   std::vector<int>     *mc_pdg;
   std::vector<float>   *mc_charge;
   std::vector<int>     *mc_barcode;
   std::vector<int>     *mc_status;
   std::vector<int>     *mc_mother_pdg;
   std::vector<int>     *mc_mother_barcode;
   std::vector<int>     *mc_child_n;
   std::vector<std::vector<int> > *mc_child_barcode;
   std::vector<float>   *mc_begVtx_x;
   std::vector<float>   *mc_begVtx_y;
   std::vector<float>   *mc_begVtx_z;
   std::vector<float>   *mc_begVtx_barcode;
   std::vector<float>   *mc_endVtx_x;
   std::vector<float>   *mc_endVtx_y;
   std::vector<float>   *mc_endVtx_z;
   std::vector<float>   *mc_endVtx_barcode;
   Int_t           mcVx_n;
   std::vector<float>   *mcVx_x;
   std::vector<float>   *mcVx_y;
   std::vector<float>   *mcVx_z;
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
   std::vector<float>   *trk_err_d0_wrtPV;
   std::vector<float>   *trk_err_z0_wrtPV;
   std::vector<float>   *trk_err_phi_wrtPV;
   std::vector<float>   *trk_err_theta_wrtPV;
   std::vector<float>   *trk_err_qoverp_wrtPV;
   std::vector<float>   *trk_cov_d0_wrtPV;
   std::vector<float>   *trk_cov_z0_wrtPV;
   std::vector<float>   *trk_cov_phi_wrtPV;
   std::vector<float>   *trk_cov_theta_wrtPV;
   std::vector<float>   *trk_cov_qoverp_wrtPV;
   std::vector<float>   *trk_cov_d0_z0_wrtPV;
   std::vector<float>   *trk_cov_d0_phi_wrtPV;
   std::vector<float>   *trk_cov_d0_theta_wrtPV;
   std::vector<float>   *trk_cov_d0_qoverp_wrtPV;
   std::vector<float>   *trk_cov_z0_phi_wrtPV;
   std::vector<float>   *trk_cov_z0_theta_wrtPV;
   std::vector<float>   *trk_cov_z0_qoverp_wrtPV;
   std::vector<float>   *trk_cov_phi_theta_wrtPV;
   std::vector<float>   *trk_cov_phi_qoverp_wrtPV;
   std::vector<float>   *trk_cov_theta_qoverp_wrtPV;
   std::vector<float>   *trk_d0_wrtBS;
   std::vector<float>   *trk_z0_wrtBS;
   std::vector<float>   *trk_phi_wrtBS;
   std::vector<float>   *trk_theta_wrtBS;
   std::vector<float>   *trk_qoverp_wrtBS;
   std::vector<float>   *trk_err_d0_wrtBS;
   std::vector<float>   *trk_err_z0_wrtBS;
   std::vector<float>   *trk_err_phi_wrtBS;
   std::vector<float>   *trk_err_theta_wrtBS;
   std::vector<float>   *trk_err_qoverp_wrtBS;
   std::vector<float>   *trk_cov_d0_wrtBS;
   std::vector<float>   *trk_cov_z0_wrtBS;
   std::vector<float>   *trk_cov_phi_wrtBS;
   std::vector<float>   *trk_cov_theta_wrtBS;
   std::vector<float>   *trk_cov_qoverp_wrtBS;
   std::vector<float>   *trk_cov_d0_z0_wrtBS;
   std::vector<float>   *trk_cov_d0_phi_wrtBS;
   std::vector<float>   *trk_cov_d0_theta_wrtBS;
   std::vector<float>   *trk_cov_d0_qoverp_wrtBS;
   std::vector<float>   *trk_cov_z0_phi_wrtBS;
   std::vector<float>   *trk_cov_z0_theta_wrtBS;
   std::vector<float>   *trk_cov_z0_qoverp_wrtBS;
   std::vector<float>   *trk_cov_phi_theta_wrtBS;
   std::vector<float>   *trk_cov_phi_qoverp_wrtBS;
   std::vector<float>   *trk_cov_theta_qoverp_wrtBS;
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
   std::vector<int>     *trk_hit_n;
   std::vector<std::vector<int> > *trk_hit_type;
   std::vector<std::vector<unsigned int> > *trk_hit_id;
   std::vector<std::vector<unsigned int> > *trk_hit_detElementId;
   std::vector<std::vector<int> > *trk_hit_detType;
   std::vector<std::vector<int> > *trk_hit_bec;
   std::vector<std::vector<int> > *trk_hit_layer;
   std::vector<std::vector<float> > *trk_hit_charge;
   std::vector<std::vector<int> > *trk_hit_sizePhi;
   std::vector<std::vector<int> > *trk_hit_sizeZ;
   std::vector<std::vector<int> > *trk_hit_size;
   std::vector<std::vector<int> > *trk_hit_isCompetingRIO;
   std::vector<std::vector<float> > *trk_hit_locX;
   std::vector<std::vector<float> > *trk_hit_locY;
   std::vector<std::vector<float> > *trk_hit_locPhi;
   std::vector<std::vector<float> > *trk_hit_locTheta;
   std::vector<std::vector<float> > *trk_hit_err_locX;
   std::vector<std::vector<float> > *trk_hit_err_locY;
   std::vector<std::vector<float> > *trk_hit_cov_locXY;
   std::vector<std::vector<float> > *trk_hit_x;
   std::vector<std::vector<float> > *trk_hit_y;
   std::vector<std::vector<float> > *trk_hit_z;
   std::vector<std::vector<float> > *trk_hit_trkLocX;
   std::vector<std::vector<float> > *trk_hit_trkLocY;
   std::vector<std::vector<float> > *trk_hit_err_trkLocX;
   std::vector<std::vector<float> > *trk_hit_err_trkLocY;
   std::vector<std::vector<float> > *trk_hit_cov_trkLocXY;
   std::vector<std::vector<float> > *trk_hit_chi2;
   std::vector<std::vector<int> > *trk_hit_ndof;
   std::vector<int>     *trk_outlier_n;
   std::vector<std::vector<int> > *trk_outlier_type;
   std::vector<std::vector<unsigned int> > *trk_outlier_id;
   std::vector<std::vector<unsigned int> > *trk_outlier_detElementId;
   std::vector<std::vector<int> > *trk_outlier_detType;
   std::vector<std::vector<int> > *trk_outlier_bec;
   std::vector<std::vector<int> > *trk_outlier_layer;
   std::vector<std::vector<float> > *trk_outlier_charge;
   std::vector<std::vector<int> > *trk_outlier_sizePhi;
   std::vector<std::vector<int> > *trk_outlier_sizeZ;
   std::vector<std::vector<int> > *trk_outlier_size;
   std::vector<std::vector<int> > *trk_outlier_isCompetingRIO;
   std::vector<std::vector<float> > *trk_outlier_locX;
   std::vector<std::vector<float> > *trk_outlier_locY;
   std::vector<std::vector<float> > *trk_outlier_locPhi;
   std::vector<std::vector<float> > *trk_outlier_locTheta;
   std::vector<std::vector<float> > *trk_outlier_err_locX;
   std::vector<std::vector<float> > *trk_outlier_err_locY;
   std::vector<std::vector<float> > *trk_outlier_cov_locXY;
   std::vector<std::vector<float> > *trk_outlier_x;
   std::vector<std::vector<float> > *trk_outlier_y;
   std::vector<std::vector<float> > *trk_outlier_z;
   std::vector<std::vector<float> > *trk_outlier_trkLocX;
   std::vector<std::vector<float> > *trk_outlier_trkLocY;
   std::vector<std::vector<float> > *trk_outlier_err_trkLocX;
   std::vector<std::vector<float> > *trk_outlier_err_trkLocY;
   std::vector<std::vector<float> > *trk_outlier_cov_trkLocXY;
   std::vector<std::vector<float> > *trk_outlier_chi2;
   std::vector<std::vector<int> > *trk_outlier_ndof;
   std::vector<int>     *trk_hole_n;
   std::vector<std::vector<int> > *trk_hole_type;
   std::vector<std::vector<unsigned int> > *trk_hole_id;
   std::vector<std::vector<unsigned int> > *trk_hole_detElementId;
   std::vector<std::vector<int> > *trk_hole_detType;
   std::vector<std::vector<int> > *trk_hole_bec;
   std::vector<std::vector<int> > *trk_hole_layer;
   std::vector<std::vector<float> > *trk_hole_charge;
   std::vector<std::vector<int> > *trk_hole_sizePhi;
   std::vector<std::vector<int> > *trk_hole_sizeZ;
   std::vector<std::vector<int> > *trk_hole_size;
   std::vector<std::vector<int> > *trk_hole_isCompetingRIO;
   std::vector<std::vector<float> > *trk_hole_locX;
   std::vector<std::vector<float> > *trk_hole_locY;
   std::vector<std::vector<float> > *trk_hole_locPhi;
   std::vector<std::vector<float> > *trk_hole_locTheta;
   std::vector<std::vector<float> > *trk_hole_err_locX;
   std::vector<std::vector<float> > *trk_hole_err_locY;
   std::vector<std::vector<float> > *trk_hole_cov_locXY;
   std::vector<std::vector<float> > *trk_hole_x;
   std::vector<std::vector<float> > *trk_hole_y;
   std::vector<std::vector<float> > *trk_hole_z;
   std::vector<std::vector<float> > *trk_hole_trkLocX;
   std::vector<std::vector<float> > *trk_hole_trkLocY;
   std::vector<std::vector<float> > *trk_hole_err_trkLocX;
   std::vector<std::vector<float> > *trk_hole_err_trkLocY;
   std::vector<std::vector<float> > *trk_hole_cov_trkLocXY;
   std::vector<std::vector<float> > *trk_hole_chi2;
   std::vector<std::vector<int> > *trk_hole_ndof;
   Int_t           vx_n;
   std::vector<float>   *vx_x;
   std::vector<float>   *vx_y;
   std::vector<float>   *vx_z;
   std::vector<float>   *vx_chi2;
   std::vector<int>     *vx_ndof;
   std::vector<float>   *vx_err_x;
   std::vector<float>   *vx_err_y;
   std::vector<float>   *vx_err_z;
   std::vector<float>   *vx_cov_x;
   std::vector<float>   *vx_cov_y;
   std::vector<float>   *vx_cov_z;
   std::vector<float>   *vx_cov_xy;
   std::vector<float>   *vx_cov_xz;
   std::vector<float>   *vx_cov_yz;
   std::vector<float>   *vx_px;
   std::vector<float>   *vx_py;
   std::vector<float>   *vx_pz;
   std::vector<float>   *vx_E;
   std::vector<float>   *vx_m;
   std::vector<int>     *vx_nTracks;
   std::vector<float>   *vx_sumPt;
   std::vector<int>     *vx_type;
   std::vector<int>     *vx_trk_n;
   std::vector<std::vector<float> > *vx_trk_chi2;
   std::vector<std::vector<float> > *vx_trk_d0;
   std::vector<std::vector<float> > *vx_trk_z0;
   std::vector<std::vector<float> > *vx_trk_unbiased_d0;
   std::vector<std::vector<float> > *vx_trk_unbiased_z0;
   std::vector<std::vector<float> > *vx_trk_err_unbiased_d0;
   std::vector<std::vector<float> > *vx_trk_err_unbiased_z0;
   std::vector<std::vector<float> > *vx_trk_phi;
   std::vector<std::vector<float> > *vx_trk_theta;
   std::vector<std::vector<float> > *vx_trk_weight;
   std::vector<std::vector<int> > *vx_trk_index;
   Int_t           vxPix_n;
   std::vector<float>   *vxPix_x;
   std::vector<float>   *vxPix_y;
   std::vector<float>   *vxPix_z;
   std::vector<float>   *vxPix_chi2;
   std::vector<int>     *vxPix_ndof;
   std::vector<float>   *vxPix_err_x;
   std::vector<float>   *vxPix_err_y;
   std::vector<float>   *vxPix_err_z;
   std::vector<float>   *vxPix_cov_x;
   std::vector<float>   *vxPix_cov_y;
   std::vector<float>   *vxPix_cov_z;
   std::vector<float>   *vxPix_cov_xy;
   std::vector<float>   *vxPix_cov_xz;
   std::vector<float>   *vxPix_cov_yz;
   std::vector<float>   *vxPix_px;
   std::vector<float>   *vxPix_py;
   std::vector<float>   *vxPix_pz;
   std::vector<float>   *vxPix_E;
   std::vector<float>   *vxPix_m;
   std::vector<int>     *vxPix_nTracks;
   std::vector<float>   *vxPix_sumPt;
   std::vector<int>     *vxPix_type;
   std::vector<int>     *vxPix_trkPix_n;
   std::vector<std::vector<float> > *vxPix_trkPix_chi2;
   std::vector<std::vector<float> > *vxPix_trkPix_d0;
   std::vector<std::vector<float> > *vxPix_trkPix_z0;
   std::vector<std::vector<float> > *vxPix_trkPix_unbiased_d0;
   std::vector<std::vector<float> > *vxPix_trkPix_unbiased_z0;
   std::vector<std::vector<float> > *vxPix_trkPix_err_unbiased_d0;
   std::vector<std::vector<float> > *vxPix_trkPix_err_unbiased_z0;
   std::vector<std::vector<float> > *vxPix_trkPix_phi;
   std::vector<std::vector<float> > *vxPix_trkPix_theta;
   std::vector<std::vector<float> > *vxPix_trkPix_weight;
   Int_t           vxSCT_n;
   std::vector<float>   *vxSCT_x;
   std::vector<float>   *vxSCT_y;
   std::vector<float>   *vxSCT_z;
   std::vector<float>   *vxSCT_chi2;
   std::vector<int>     *vxSCT_ndof;
   std::vector<float>   *vxSCT_err_x;
   std::vector<float>   *vxSCT_err_y;
   std::vector<float>   *vxSCT_err_z;
   std::vector<float>   *vxSCT_cov_x;
   std::vector<float>   *vxSCT_cov_y;
   std::vector<float>   *vxSCT_cov_z;
   std::vector<float>   *vxSCT_cov_xy;
   std::vector<float>   *vxSCT_cov_xz;
   std::vector<float>   *vxSCT_cov_yz;
   std::vector<float>   *vxSCT_px;
   std::vector<float>   *vxSCT_py;
   std::vector<float>   *vxSCT_pz;
   std::vector<float>   *vxSCT_E;
   std::vector<float>   *vxSCT_m;
   std::vector<int>     *vxSCT_nTracks;
   std::vector<float>   *vxSCT_sumPt;
   std::vector<int>     *vxSCT_type;
   std::vector<int>     *vxSCT_trkSCT_n;
   std::vector<std::vector<float> > *vxSCT_trkSCT_chi2;
   std::vector<std::vector<float> > *vxSCT_trkSCT_d0;
   std::vector<std::vector<float> > *vxSCT_trkSCT_z0;
   std::vector<std::vector<float> > *vxSCT_trkSCT_unbiased_d0;
   std::vector<std::vector<float> > *vxSCT_trkSCT_unbiased_z0;
   std::vector<std::vector<float> > *vxSCT_trkSCT_err_unbiased_d0;
   std::vector<std::vector<float> > *vxSCT_trkSCT_err_unbiased_z0;
   std::vector<std::vector<float> > *vxSCT_trkSCT_phi;
   std::vector<std::vector<float> > *vxSCT_trkSCT_theta;
   std::vector<std::vector<float> > *vxSCT_trkSCT_weight;
   Int_t           vxSi_n;
   std::vector<float>   *vxSi_x;
   std::vector<float>   *vxSi_y;
   std::vector<float>   *vxSi_z;
   std::vector<float>   *vxSi_chi2;
   std::vector<int>     *vxSi_ndof;
   std::vector<float>   *vxSi_err_x;
   std::vector<float>   *vxSi_err_y;
   std::vector<float>   *vxSi_err_z;
   std::vector<float>   *vxSi_cov_x;
   std::vector<float>   *vxSi_cov_y;
   std::vector<float>   *vxSi_cov_z;
   std::vector<float>   *vxSi_cov_xy;
   std::vector<float>   *vxSi_cov_xz;
   std::vector<float>   *vxSi_cov_yz;
   std::vector<float>   *vxSi_px;
   std::vector<float>   *vxSi_py;
   std::vector<float>   *vxSi_pz;
   std::vector<float>   *vxSi_E;
   std::vector<float>   *vxSi_m;
   std::vector<int>     *vxSi_nTracks;
   std::vector<float>   *vxSi_sumPt;
   std::vector<int>     *vxSi_type;
   std::vector<int>     *vxSi_trkResolvedTrack_n;
   std::vector<std::vector<float> > *vxSi_trkResolvedTrack_chi2;
   std::vector<std::vector<float> > *vxSi_trkResolvedTrack_d0;
   std::vector<std::vector<float> > *vxSi_trkResolvedTrack_z0;
   std::vector<std::vector<float> > *vxSi_trkResolvedTrack_unbiased_d0;
   std::vector<std::vector<float> > *vxSi_trkResolvedTrack_unbiased_z0;
   std::vector<std::vector<float> > *vxSi_trkResolvedTrack_err_unbiased_d0;
   std::vector<std::vector<float> > *vxSi_trkResolvedTrack_err_unbiased_z0;
   std::vector<std::vector<float> > *vxSi_trkResolvedTrack_phi;
   std::vector<std::vector<float> > *vxSi_trkResolvedTrack_theta;
   std::vector<std::vector<float> > *vxSi_trkResolvedTrack_weight;
   Int_t           vxFull_n;
   std::vector<float>   *vxFull_x;
   std::vector<float>   *vxFull_y;
   std::vector<float>   *vxFull_z;
   std::vector<float>   *vxFull_chi2;
   std::vector<int>     *vxFull_ndof;
   std::vector<float>   *vxFull_err_x;
   std::vector<float>   *vxFull_err_y;
   std::vector<float>   *vxFull_err_z;
   std::vector<float>   *vxFull_cov_x;
   std::vector<float>   *vxFull_cov_y;
   std::vector<float>   *vxFull_cov_z;
   std::vector<float>   *vxFull_cov_xy;
   std::vector<float>   *vxFull_cov_xz;
   std::vector<float>   *vxFull_cov_yz;
   std::vector<float>   *vxFull_px;
   std::vector<float>   *vxFull_py;
   std::vector<float>   *vxFull_pz;
   std::vector<float>   *vxFull_E;
   std::vector<float>   *vxFull_m;
   std::vector<int>     *vxFull_nTracks;
   std::vector<float>   *vxFull_sumPt;
   std::vector<int>     *vxFull_type;
   std::vector<int>     *vxFull_trk_n;
   std::vector<std::vector<float> > *vxFull_trk_chi2;
   std::vector<std::vector<float> > *vxFull_trk_d0;
   std::vector<std::vector<float> > *vxFull_trk_z0;
   std::vector<std::vector<float> > *vxFull_trk_unbiased_d0;
   std::vector<std::vector<float> > *vxFull_trk_unbiased_z0;
   std::vector<std::vector<float> > *vxFull_trk_err_unbiased_d0;
   std::vector<std::vector<float> > *vxFull_trk_err_unbiased_z0;
   std::vector<std::vector<float> > *vxFull_trk_phi;
   std::vector<std::vector<float> > *vxFull_trk_theta;
   std::vector<std::vector<float> > *vxFull_trk_weight;
   std::vector<std::vector<int> > *vxFull_trk_index;
   Int_t           vxFast_n;
   std::vector<float>   *vxFast_x;
   std::vector<float>   *vxFast_y;
   std::vector<float>   *vxFast_z;
   std::vector<float>   *vxFast_chi2;
   std::vector<int>     *vxFast_ndof;
   std::vector<float>   *vxFast_err_x;
   std::vector<float>   *vxFast_err_y;
   std::vector<float>   *vxFast_err_z;
   std::vector<float>   *vxFast_cov_x;
   std::vector<float>   *vxFast_cov_y;
   std::vector<float>   *vxFast_cov_z;
   std::vector<float>   *vxFast_cov_xy;
   std::vector<float>   *vxFast_cov_xz;
   std::vector<float>   *vxFast_cov_yz;
   std::vector<float>   *vxFast_px;
   std::vector<float>   *vxFast_py;
   std::vector<float>   *vxFast_pz;
   std::vector<float>   *vxFast_E;
   std::vector<float>   *vxFast_m;
   std::vector<int>     *vxFast_nTracks;
   std::vector<float>   *vxFast_sumPt;
   std::vector<int>     *vxFast_type;
   std::vector<int>     *vxFast_trk_n;
   std::vector<std::vector<float> > *vxFast_trk_chi2;
   std::vector<std::vector<float> > *vxFast_trk_d0;
   std::vector<std::vector<float> > *vxFast_trk_z0;
   std::vector<std::vector<float> > *vxFast_trk_unbiased_d0;
   std::vector<std::vector<float> > *vxFast_trk_unbiased_z0;
   std::vector<std::vector<float> > *vxFast_trk_err_unbiased_d0;
   std::vector<std::vector<float> > *vxFast_trk_err_unbiased_z0;
   std::vector<std::vector<float> > *vxFast_trk_phi;
   std::vector<std::vector<float> > *vxFast_trk_theta;
   std::vector<std::vector<float> > *vxFast_trk_weight;
   std::vector<std::vector<int> > *vxFast_trk_index;
   Int_t           vxFullPix_n;
   std::vector<float>   *vxFullPix_x;
   std::vector<float>   *vxFullPix_y;
   std::vector<float>   *vxFullPix_z;
   std::vector<float>   *vxFullPix_chi2;
   std::vector<int>     *vxFullPix_ndof;
   std::vector<float>   *vxFullPix_err_x;
   std::vector<float>   *vxFullPix_err_y;
   std::vector<float>   *vxFullPix_err_z;
   std::vector<float>   *vxFullPix_cov_x;
   std::vector<float>   *vxFullPix_cov_y;
   std::vector<float>   *vxFullPix_cov_z;
   std::vector<float>   *vxFullPix_cov_xy;
   std::vector<float>   *vxFullPix_cov_xz;
   std::vector<float>   *vxFullPix_cov_yz;
   std::vector<float>   *vxFullPix_px;
   std::vector<float>   *vxFullPix_py;
   std::vector<float>   *vxFullPix_pz;
   std::vector<float>   *vxFullPix_E;
   std::vector<float>   *vxFullPix_m;
   std::vector<int>     *vxFullPix_nTracks;
   std::vector<float>   *vxFullPix_sumPt;
   std::vector<int>     *vxFullPix_type;
   std::vector<int>     *vxFullPix_trkPix_n;
   std::vector<std::vector<float> > *vxFullPix_trkPix_chi2;
   std::vector<std::vector<float> > *vxFullPix_trkPix_d0;
   std::vector<std::vector<float> > *vxFullPix_trkPix_z0;
   std::vector<std::vector<float> > *vxFullPix_trkPix_unbiased_d0;
   std::vector<std::vector<float> > *vxFullPix_trkPix_unbiased_z0;
   std::vector<std::vector<float> > *vxFullPix_trkPix_err_unbiased_d0;
   std::vector<std::vector<float> > *vxFullPix_trkPix_err_unbiased_z0;
   std::vector<std::vector<float> > *vxFullPix_trkPix_phi;
   std::vector<std::vector<float> > *vxFullPix_trkPix_theta;
   std::vector<std::vector<float> > *vxFullPix_trkPix_weight;
   Int_t           vxFastPix_n;
   std::vector<float>   *vxFastPix_x;
   std::vector<float>   *vxFastPix_y;
   std::vector<float>   *vxFastPix_z;
   std::vector<float>   *vxFastPix_chi2;
   std::vector<int>     *vxFastPix_ndof;
   std::vector<float>   *vxFastPix_err_x;
   std::vector<float>   *vxFastPix_err_y;
   std::vector<float>   *vxFastPix_err_z;
   std::vector<float>   *vxFastPix_cov_x;
   std::vector<float>   *vxFastPix_cov_y;
   std::vector<float>   *vxFastPix_cov_z;
   std::vector<float>   *vxFastPix_cov_xy;
   std::vector<float>   *vxFastPix_cov_xz;
   std::vector<float>   *vxFastPix_cov_yz;
   std::vector<float>   *vxFastPix_px;
   std::vector<float>   *vxFastPix_py;
   std::vector<float>   *vxFastPix_pz;
   std::vector<float>   *vxFastPix_E;
   std::vector<float>   *vxFastPix_m;
   std::vector<int>     *vxFastPix_nTracks;
   std::vector<float>   *vxFastPix_sumPt;
   std::vector<int>     *vxFastPix_type;
   std::vector<int>     *vxFastPix_trkPix_n;
   std::vector<std::vector<float> > *vxFastPix_trkPix_chi2;
   std::vector<std::vector<float> > *vxFastPix_trkPix_d0;
   std::vector<std::vector<float> > *vxFastPix_trkPix_z0;
   std::vector<std::vector<float> > *vxFastPix_trkPix_unbiased_d0;
   std::vector<std::vector<float> > *vxFastPix_trkPix_unbiased_z0;
   std::vector<std::vector<float> > *vxFastPix_trkPix_err_unbiased_d0;
   std::vector<std::vector<float> > *vxFastPix_trkPix_err_unbiased_z0;
   std::vector<std::vector<float> > *vxFastPix_trkPix_phi;
   std::vector<std::vector<float> > *vxFastPix_trkPix_theta;
   std::vector<std::vector<float> > *vxFastPix_trkPix_weight;
   Int_t           vxFullSCT_n;
   std::vector<float>   *vxFullSCT_x;
   std::vector<float>   *vxFullSCT_y;
   std::vector<float>   *vxFullSCT_z;
   std::vector<float>   *vxFullSCT_chi2;
   std::vector<int>     *vxFullSCT_ndof;
   std::vector<float>   *vxFullSCT_err_x;
   std::vector<float>   *vxFullSCT_err_y;
   std::vector<float>   *vxFullSCT_err_z;
   std::vector<float>   *vxFullSCT_cov_x;
   std::vector<float>   *vxFullSCT_cov_y;
   std::vector<float>   *vxFullSCT_cov_z;
   std::vector<float>   *vxFullSCT_cov_xy;
   std::vector<float>   *vxFullSCT_cov_xz;
   std::vector<float>   *vxFullSCT_cov_yz;
   std::vector<float>   *vxFullSCT_px;
   std::vector<float>   *vxFullSCT_py;
   std::vector<float>   *vxFullSCT_pz;
   std::vector<float>   *vxFullSCT_E;
   std::vector<float>   *vxFullSCT_m;
   std::vector<int>     *vxFullSCT_nTracks;
   std::vector<float>   *vxFullSCT_sumPt;
   std::vector<int>     *vxFullSCT_type;
   std::vector<int>     *vxFullSCT_trkSCT_n;
   std::vector<std::vector<float> > *vxFullSCT_trkSCT_chi2;
   std::vector<std::vector<float> > *vxFullSCT_trkSCT_d0;
   std::vector<std::vector<float> > *vxFullSCT_trkSCT_z0;
   std::vector<std::vector<float> > *vxFullSCT_trkSCT_unbiased_d0;
   std::vector<std::vector<float> > *vxFullSCT_trkSCT_unbiased_z0;
   std::vector<std::vector<float> > *vxFullSCT_trkSCT_err_unbiased_d0;
   std::vector<std::vector<float> > *vxFullSCT_trkSCT_err_unbiased_z0;
   std::vector<std::vector<float> > *vxFullSCT_trkSCT_phi;
   std::vector<std::vector<float> > *vxFullSCT_trkSCT_theta;
   std::vector<std::vector<float> > *vxFullSCT_trkSCT_weight;
   Int_t           vxFastSCT_n;
   std::vector<float>   *vxFastSCT_x;
   std::vector<float>   *vxFastSCT_y;
   std::vector<float>   *vxFastSCT_z;
   std::vector<float>   *vxFastSCT_chi2;
   std::vector<int>     *vxFastSCT_ndof;
   std::vector<float>   *vxFastSCT_err_x;
   std::vector<float>   *vxFastSCT_err_y;
   std::vector<float>   *vxFastSCT_err_z;
   std::vector<float>   *vxFastSCT_cov_x;
   std::vector<float>   *vxFastSCT_cov_y;
   std::vector<float>   *vxFastSCT_cov_z;
   std::vector<float>   *vxFastSCT_cov_xy;
   std::vector<float>   *vxFastSCT_cov_xz;
   std::vector<float>   *vxFastSCT_cov_yz;
   std::vector<float>   *vxFastSCT_px;
   std::vector<float>   *vxFastSCT_py;
   std::vector<float>   *vxFastSCT_pz;
   std::vector<float>   *vxFastSCT_E;
   std::vector<float>   *vxFastSCT_m;
   std::vector<int>     *vxFastSCT_nTracks;
   std::vector<float>   *vxFastSCT_sumPt;
   std::vector<int>     *vxFastSCT_type;
   std::vector<int>     *vxFastSCT_trkSCT_n;
   std::vector<std::vector<float> > *vxFastSCT_trkSCT_chi2;
   std::vector<std::vector<float> > *vxFastSCT_trkSCT_d0;
   std::vector<std::vector<float> > *vxFastSCT_trkSCT_z0;
   std::vector<std::vector<float> > *vxFastSCT_trkSCT_unbiased_d0;
   std::vector<std::vector<float> > *vxFastSCT_trkSCT_unbiased_z0;
   std::vector<std::vector<float> > *vxFastSCT_trkSCT_err_unbiased_d0;
   std::vector<std::vector<float> > *vxFastSCT_trkSCT_err_unbiased_z0;
   std::vector<std::vector<float> > *vxFastSCT_trkSCT_phi;
   std::vector<std::vector<float> > *vxFastSCT_trkSCT_theta;
   std::vector<std::vector<float> > *vxFastSCT_trkSCT_weight;
   Int_t           vxFullSi_n;
   std::vector<float>   *vxFullSi_x;
   std::vector<float>   *vxFullSi_y;
   std::vector<float>   *vxFullSi_z;
   std::vector<float>   *vxFullSi_chi2;
   std::vector<int>     *vxFullSi_ndof;
   std::vector<float>   *vxFullSi_err_x;
   std::vector<float>   *vxFullSi_err_y;
   std::vector<float>   *vxFullSi_err_z;
   std::vector<float>   *vxFullSi_cov_x;
   std::vector<float>   *vxFullSi_cov_y;
   std::vector<float>   *vxFullSi_cov_z;
   std::vector<float>   *vxFullSi_cov_xy;
   std::vector<float>   *vxFullSi_cov_xz;
   std::vector<float>   *vxFullSi_cov_yz;
   std::vector<float>   *vxFullSi_px;
   std::vector<float>   *vxFullSi_py;
   std::vector<float>   *vxFullSi_pz;
   std::vector<float>   *vxFullSi_E;
   std::vector<float>   *vxFullSi_m;
   std::vector<int>     *vxFullSi_nTracks;
   std::vector<float>   *vxFullSi_sumPt;
   std::vector<int>     *vxFullSi_type;
   std::vector<int>     *vxFullSi_trkResolvedTrack_n;
   std::vector<std::vector<float> > *vxFullSi_trkResolvedTrack_chi2;
   std::vector<std::vector<float> > *vxFullSi_trkResolvedTrack_d0;
   std::vector<std::vector<float> > *vxFullSi_trkResolvedTrack_z0;
   std::vector<std::vector<float> > *vxFullSi_trkResolvedTrack_unbiased_d0;
   std::vector<std::vector<float> > *vxFullSi_trkResolvedTrack_unbiased_z0;
   std::vector<std::vector<float> > *vxFullSi_trkResolvedTrack_err_unbiased_d0;
   std::vector<std::vector<float> > *vxFullSi_trkResolvedTrack_err_unbiased_z0;
   std::vector<std::vector<float> > *vxFullSi_trkResolvedTrack_phi;
   std::vector<std::vector<float> > *vxFullSi_trkResolvedTrack_theta;
   std::vector<std::vector<float> > *vxFullSi_trkResolvedTrack_weight;
   Int_t           vxFastSi_n;
   std::vector<float>   *vxFastSi_x;
   std::vector<float>   *vxFastSi_y;
   std::vector<float>   *vxFastSi_z;
   std::vector<float>   *vxFastSi_chi2;
   std::vector<int>     *vxFastSi_ndof;
   std::vector<float>   *vxFastSi_err_x;
   std::vector<float>   *vxFastSi_err_y;
   std::vector<float>   *vxFastSi_err_z;
   std::vector<float>   *vxFastSi_cov_x;
   std::vector<float>   *vxFastSi_cov_y;
   std::vector<float>   *vxFastSi_cov_z;
   std::vector<float>   *vxFastSi_cov_xy;
   std::vector<float>   *vxFastSi_cov_xz;
   std::vector<float>   *vxFastSi_cov_yz;
   std::vector<float>   *vxFastSi_px;
   std::vector<float>   *vxFastSi_py;
   std::vector<float>   *vxFastSi_pz;
   std::vector<float>   *vxFastSi_E;
   std::vector<float>   *vxFastSi_m;
   std::vector<int>     *vxFastSi_nTracks;
   std::vector<float>   *vxFastSi_sumPt;
   std::vector<int>     *vxFastSi_type;
   std::vector<int>     *vxFastSi_trkResolvedTrack_n;
   std::vector<std::vector<float> > *vxFastSi_trkResolvedTrack_chi2;
   std::vector<std::vector<float> > *vxFastSi_trkResolvedTrack_d0;
   std::vector<std::vector<float> > *vxFastSi_trkResolvedTrack_z0;
   std::vector<std::vector<float> > *vxFastSi_trkResolvedTrack_unbiased_d0;
   std::vector<std::vector<float> > *vxFastSi_trkResolvedTrack_unbiased_z0;
   std::vector<std::vector<float> > *vxFastSi_trkResolvedTrack_err_unbiased_d0;
   std::vector<std::vector<float> > *vxFastSi_trkResolvedTrack_err_unbiased_z0;
   std::vector<std::vector<float> > *vxFastSi_trkResolvedTrack_phi;
   std::vector<std::vector<float> > *vxFastSi_trkResolvedTrack_theta;
   std::vector<std::vector<float> > *vxFastSi_trkResolvedTrack_weight;

   // List of branches
   TBranch        *b_ei_RunNumber;   //!
   TBranch        *b_ei_EventNumber;   //!
   TBranch        *b_ei_timestamp;   //!
   TBranch        *b_ei_timestamp_ns;   //!
   TBranch        *b_ei_lbn;   //!
   TBranch        *b_ei_bcid;   //!
   TBranch        *b_ei_detmask0;   //!
   TBranch        *b_ei_detmask1;   //!
   TBranch        *b_mc_n;   //!
   TBranch        *b_mc_gen_pt;   //!
   TBranch        *b_mc_gen_eta;   //!
   TBranch        *b_mc_gen_phi;   //!
   TBranch        *b_mc_gen_energy;   //!
   TBranch        *b_mc_perigee_ok;   //!
   TBranch        *b_mc_perigee_d0;   //!
   TBranch        *b_mc_perigee_z0;   //!
   TBranch        *b_mc_perigee_phi;   //!
   TBranch        *b_mc_perigee_theta;   //!
   TBranch        *b_mc_perigee_qoverp;   //!
   TBranch        *b_mc_pdg;   //!
   TBranch        *b_mc_charge;   //!
   TBranch        *b_mc_barcode;   //!
   TBranch        *b_mc_status;   //!
   TBranch        *b_mc_mother_pdg;   //!
   TBranch        *b_mc_mother_barcode;   //!
   TBranch        *b_mc_child_n;   //!
   TBranch        *b_mc_child_barcode;   //!
   TBranch        *b_mc_begVtx_x;   //!
   TBranch        *b_mc_begVtx_y;   //!
   TBranch        *b_mc_begVtx_z;   //!
   TBranch        *b_mc_begVtx_barcode;   //!
   TBranch        *b_mc_endVtx_x;   //!
   TBranch        *b_mc_endVtx_y;   //!
   TBranch        *b_mc_endVtx_z;   //!
   TBranch        *b_mc_endVtx_barcode;   //!
   TBranch        *b_mcVx_n;   //!
   TBranch        *b_mcVx_x;   //!
   TBranch        *b_mcVx_y;   //!
   TBranch        *b_mcVx_z;   //!
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
   TBranch        *b_trk_err_d0_wrtPV;   //!
   TBranch        *b_trk_err_z0_wrtPV;   //!
   TBranch        *b_trk_err_phi_wrtPV;   //!
   TBranch        *b_trk_err_theta_wrtPV;   //!
   TBranch        *b_trk_err_qoverp_wrtPV;   //!
   TBranch        *b_trk_cov_d0_wrtPV;   //!
   TBranch        *b_trk_cov_z0_wrtPV;   //!
   TBranch        *b_trk_cov_phi_wrtPV;   //!
   TBranch        *b_trk_cov_theta_wrtPV;   //!
   TBranch        *b_trk_cov_qoverp_wrtPV;   //!
   TBranch        *b_trk_cov_d0_z0_wrtPV;   //!
   TBranch        *b_trk_cov_d0_phi_wrtPV;   //!
   TBranch        *b_trk_cov_d0_theta_wrtPV;   //!
   TBranch        *b_trk_cov_d0_qoverp_wrtPV;   //!
   TBranch        *b_trk_cov_z0_phi_wrtPV;   //!
   TBranch        *b_trk_cov_z0_theta_wrtPV;   //!
   TBranch        *b_trk_cov_z0_qoverp_wrtPV;   //!
   TBranch        *b_trk_cov_phi_theta_wrtPV;   //!
   TBranch        *b_trk_cov_phi_qoverp_wrtPV;   //!
   TBranch        *b_trk_cov_theta_qoverp_wrtPV;   //!
   TBranch        *b_trk_d0_wrtBS;   //!
   TBranch        *b_trk_z0_wrtBS;   //!
   TBranch        *b_trk_phi_wrtBS;   //!
   TBranch        *b_trk_theta_wrtBS;   //!
   TBranch        *b_trk_qoverp_wrtBS;   //!
   TBranch        *b_trk_err_d0_wrtBS;   //!
   TBranch        *b_trk_err_z0_wrtBS;   //!
   TBranch        *b_trk_err_phi_wrtBS;   //!
   TBranch        *b_trk_err_theta_wrtBS;   //!
   TBranch        *b_trk_err_qoverp_wrtBS;   //!
   TBranch        *b_trk_cov_d0_wrtBS;   //!
   TBranch        *b_trk_cov_z0_wrtBS;   //!
   TBranch        *b_trk_cov_phi_wrtBS;   //!
   TBranch        *b_trk_cov_theta_wrtBS;   //!
   TBranch        *b_trk_cov_qoverp_wrtBS;   //!
   TBranch        *b_trk_cov_d0_z0_wrtBS;   //!
   TBranch        *b_trk_cov_d0_phi_wrtBS;   //!
   TBranch        *b_trk_cov_d0_theta_wrtBS;   //!
   TBranch        *b_trk_cov_d0_qoverp_wrtBS;   //!
   TBranch        *b_trk_cov_z0_phi_wrtBS;   //!
   TBranch        *b_trk_cov_z0_theta_wrtBS;   //!
   TBranch        *b_trk_cov_z0_qoverp_wrtBS;   //!
   TBranch        *b_trk_cov_phi_theta_wrtBS;   //!
   TBranch        *b_trk_cov_phi_qoverp_wrtBS;   //!
   TBranch        *b_trk_cov_theta_qoverp_wrtBS;   //!
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
   TBranch        *b_trk_hit_n;   //!
   TBranch        *b_trk_hit_type;   //!
   TBranch        *b_trk_hit_id;   //!
   TBranch        *b_trk_hit_detElementId;   //!
   TBranch        *b_trk_hit_detType;   //!
   TBranch        *b_trk_hit_bec;   //!
   TBranch        *b_trk_hit_layer;   //!
   TBranch        *b_trk_hit_charge;   //!
   TBranch        *b_trk_hit_sizePhi;   //!
   TBranch        *b_trk_hit_sizeZ;   //!
   TBranch        *b_trk_hit_size;   //!
   TBranch        *b_trk_hit_isCompetingRIO;   //!
   TBranch        *b_trk_hit_locX;   //!
   TBranch        *b_trk_hit_locY;   //!
   TBranch        *b_trk_hit_locPhi;   //!
   TBranch        *b_trk_hit_locTheta;   //!
   TBranch        *b_trk_hit_err_locX;   //!
   TBranch        *b_trk_hit_err_locY;   //!
   TBranch        *b_trk_hit_cov_locXY;   //!
   TBranch        *b_trk_hit_x;   //!
   TBranch        *b_trk_hit_y;   //!
   TBranch        *b_trk_hit_z;   //!
   TBranch        *b_trk_hit_trkLocX;   //!
   TBranch        *b_trk_hit_trkLocY;   //!
   TBranch        *b_trk_hit_err_trkLocX;   //!
   TBranch        *b_trk_hit_err_trkLocY;   //!
   TBranch        *b_trk_hit_cov_trkLocXY;   //!
   TBranch        *b_trk_hit_chi2;   //!
   TBranch        *b_trk_hit_ndof;   //!
   TBranch        *b_trk_outlier_n;   //!
   TBranch        *b_trk_outlier_type;   //!
   TBranch        *b_trk_outlier_id;   //!
   TBranch        *b_trk_outlier_detElementId;   //!
   TBranch        *b_trk_outlier_detType;   //!
   TBranch        *b_trk_outlier_bec;   //!
   TBranch        *b_trk_outlier_layer;   //!
   TBranch        *b_trk_outlier_charge;   //!
   TBranch        *b_trk_outlier_sizePhi;   //!
   TBranch        *b_trk_outlier_sizeZ;   //!
   TBranch        *b_trk_outlier_size;   //!
   TBranch        *b_trk_outlier_isCompetingRIO;   //!
   TBranch        *b_trk_outlier_locX;   //!
   TBranch        *b_trk_outlier_locY;   //!
   TBranch        *b_trk_outlier_locPhi;   //!
   TBranch        *b_trk_outlier_locTheta;   //!
   TBranch        *b_trk_outlier_err_locX;   //!
   TBranch        *b_trk_outlier_err_locY;   //!
   TBranch        *b_trk_outlier_cov_locXY;   //!
   TBranch        *b_trk_outlier_x;   //!
   TBranch        *b_trk_outlier_y;   //!
   TBranch        *b_trk_outlier_z;   //!
   TBranch        *b_trk_outlier_trkLocX;   //!
   TBranch        *b_trk_outlier_trkLocY;   //!
   TBranch        *b_trk_outlier_err_trkLocX;   //!
   TBranch        *b_trk_outlier_err_trkLocY;   //!
   TBranch        *b_trk_outlier_cov_trkLocXY;   //!
   TBranch        *b_trk_outlier_chi2;   //!
   TBranch        *b_trk_outlier_ndof;   //!
   TBranch        *b_trk_hole_n;   //!
   TBranch        *b_trk_hole_type;   //!
   TBranch        *b_trk_hole_id;   //!
   TBranch        *b_trk_hole_detElementId;   //!
   TBranch        *b_trk_hole_detType;   //!
   TBranch        *b_trk_hole_bec;   //!
   TBranch        *b_trk_hole_layer;   //!
   TBranch        *b_trk_hole_charge;   //!
   TBranch        *b_trk_hole_sizePhi;   //!
   TBranch        *b_trk_hole_sizeZ;   //!
   TBranch        *b_trk_hole_size;   //!
   TBranch        *b_trk_hole_isCompetingRIO;   //!
   TBranch        *b_trk_hole_locX;   //!
   TBranch        *b_trk_hole_locY;   //!
   TBranch        *b_trk_hole_locPhi;   //!
   TBranch        *b_trk_hole_locTheta;   //!
   TBranch        *b_trk_hole_err_locX;   //!
   TBranch        *b_trk_hole_err_locY;   //!
   TBranch        *b_trk_hole_cov_locXY;   //!
   TBranch        *b_trk_hole_x;   //!
   TBranch        *b_trk_hole_y;   //!
   TBranch        *b_trk_hole_z;   //!
   TBranch        *b_trk_hole_trkLocX;   //!
   TBranch        *b_trk_hole_trkLocY;   //!
   TBranch        *b_trk_hole_err_trkLocX;   //!
   TBranch        *b_trk_hole_err_trkLocY;   //!
   TBranch        *b_trk_hole_cov_trkLocXY;   //!
   TBranch        *b_trk_hole_chi2;   //!
   TBranch        *b_trk_hole_ndof;   //!
   TBranch        *b_vx_n;   //!
   TBranch        *b_vx_x;   //!
   TBranch        *b_vx_y;   //!
   TBranch        *b_vx_z;   //!
   TBranch        *b_vx_chi2;   //!
   TBranch        *b_vx_ndof;   //!
   TBranch        *b_vx_err_x;   //!
   TBranch        *b_vx_err_y;   //!
   TBranch        *b_vx_err_z;   //!
   TBranch        *b_vx_cov_x;   //!
   TBranch        *b_vx_cov_y;   //!
   TBranch        *b_vx_cov_z;   //!
   TBranch        *b_vx_cov_xy;   //!
   TBranch        *b_vx_cov_xz;   //!
   TBranch        *b_vx_cov_yz;   //!
   TBranch        *b_vx_px;   //!
   TBranch        *b_vx_py;   //!
   TBranch        *b_vx_pz;   //!
   TBranch        *b_vx_E;   //!
   TBranch        *b_vx_m;   //!
   TBranch        *b_vx_nTracks;   //!
   TBranch        *b_vx_sumPt;   //!
   TBranch        *b_vx_type;   //!
   TBranch        *b_vx_trk_n;   //!
   TBranch        *b_vx_trk_chi2;   //!
   TBranch        *b_vx_trk_d0;   //!
   TBranch        *b_vx_trk_z0;   //!
   TBranch        *b_vx_trk_unbiased_d0;   //!
   TBranch        *b_vx_trk_unbiased_z0;   //!
   TBranch        *b_vx_trk_err_unbiased_d0;   //!
   TBranch        *b_vx_trk_err_unbiased_z0;   //!
   TBranch        *b_vx_trk_phi;   //!
   TBranch        *b_vx_trk_theta;   //!
   TBranch        *b_vx_trk_weight;   //!
   TBranch        *b_vx_trk_index;   //!
   TBranch        *b_vxPix_n;   //!
   TBranch        *b_vxPix_x;   //!
   TBranch        *b_vxPix_y;   //!
   TBranch        *b_vxPix_z;   //!
   TBranch        *b_vxPix_chi2;   //!
   TBranch        *b_vxPix_ndof;   //!
   TBranch        *b_vxPix_err_x;   //!
   TBranch        *b_vxPix_err_y;   //!
   TBranch        *b_vxPix_err_z;   //!
   TBranch        *b_vxPix_cov_x;   //!
   TBranch        *b_vxPix_cov_y;   //!
   TBranch        *b_vxPix_cov_z;   //!
   TBranch        *b_vxPix_cov_xy;   //!
   TBranch        *b_vxPix_cov_xz;   //!
   TBranch        *b_vxPix_cov_yz;   //!
   TBranch        *b_vxPix_px;   //!
   TBranch        *b_vxPix_py;   //!
   TBranch        *b_vxPix_pz;   //!
   TBranch        *b_vxPix_E;   //!
   TBranch        *b_vxPix_m;   //!
   TBranch        *b_vxPix_nTracks;   //!
   TBranch        *b_vxPix_sumPt;   //!
   TBranch        *b_vxPix_type;   //!
   TBranch        *b_vxPix_trkPix_n;   //!
   TBranch        *b_vxPix_trkPix_chi2;   //!
   TBranch        *b_vxPix_trkPix_d0;   //!
   TBranch        *b_vxPix_trkPix_z0;   //!
   TBranch        *b_vxPix_trkPix_unbiased_d0;   //!
   TBranch        *b_vxPix_trkPix_unbiased_z0;   //!
   TBranch        *b_vxPix_trkPix_err_unbiased_d0;   //!
   TBranch        *b_vxPix_trkPix_err_unbiased_z0;   //!
   TBranch        *b_vxPix_trkPix_phi;   //!
   TBranch        *b_vxPix_trkPix_theta;   //!
   TBranch        *b_vxPix_trkPix_weight;   //!
   TBranch        *b_vxSCT_n;   //!
   TBranch        *b_vxSCT_x;   //!
   TBranch        *b_vxSCT_y;   //!
   TBranch        *b_vxSCT_z;   //!
   TBranch        *b_vxSCT_chi2;   //!
   TBranch        *b_vxSCT_ndof;   //!
   TBranch        *b_vxSCT_err_x;   //!
   TBranch        *b_vxSCT_err_y;   //!
   TBranch        *b_vxSCT_err_z;   //!
   TBranch        *b_vxSCT_cov_x;   //!
   TBranch        *b_vxSCT_cov_y;   //!
   TBranch        *b_vxSCT_cov_z;   //!
   TBranch        *b_vxSCT_cov_xy;   //!
   TBranch        *b_vxSCT_cov_xz;   //!
   TBranch        *b_vxSCT_cov_yz;   //!
   TBranch        *b_vxSCT_px;   //!
   TBranch        *b_vxSCT_py;   //!
   TBranch        *b_vxSCT_pz;   //!
   TBranch        *b_vxSCT_E;   //!
   TBranch        *b_vxSCT_m;   //!
   TBranch        *b_vxSCT_nTracks;   //!
   TBranch        *b_vxSCT_sumPt;   //!
   TBranch        *b_vxSCT_type;   //!
   TBranch        *b_vxSCT_trkSCT_n;   //!
   TBranch        *b_vxSCT_trkSCT_chi2;   //!
   TBranch        *b_vxSCT_trkSCT_d0;   //!
   TBranch        *b_vxSCT_trkSCT_z0;   //!
   TBranch        *b_vxSCT_trkSCT_unbiased_d0;   //!
   TBranch        *b_vxSCT_trkSCT_unbiased_z0;   //!
   TBranch        *b_vxSCT_trkSCT_err_unbiased_d0;   //!
   TBranch        *b_vxSCT_trkSCT_err_unbiased_z0;   //!
   TBranch        *b_vxSCT_trkSCT_phi;   //!
   TBranch        *b_vxSCT_trkSCT_theta;   //!
   TBranch        *b_vxSCT_trkSCT_weight;   //!
   TBranch        *b_vxSi_n;   //!
   TBranch        *b_vxSi_x;   //!
   TBranch        *b_vxSi_y;   //!
   TBranch        *b_vxSi_z;   //!
   TBranch        *b_vxSi_chi2;   //!
   TBranch        *b_vxSi_ndof;   //!
   TBranch        *b_vxSi_err_x;   //!
   TBranch        *b_vxSi_err_y;   //!
   TBranch        *b_vxSi_err_z;   //!
   TBranch        *b_vxSi_cov_x;   //!
   TBranch        *b_vxSi_cov_y;   //!
   TBranch        *b_vxSi_cov_z;   //!
   TBranch        *b_vxSi_cov_xy;   //!
   TBranch        *b_vxSi_cov_xz;   //!
   TBranch        *b_vxSi_cov_yz;   //!
   TBranch        *b_vxSi_px;   //!
   TBranch        *b_vxSi_py;   //!
   TBranch        *b_vxSi_pz;   //!
   TBranch        *b_vxSi_E;   //!
   TBranch        *b_vxSi_m;   //!
   TBranch        *b_vxSi_nTracks;   //!
   TBranch        *b_vxSi_sumPt;   //!
   TBranch        *b_vxSi_type;   //!
   TBranch        *b_vxSi_trkResolvedTrack_n;   //!
   TBranch        *b_vxSi_trkResolvedTrack_chi2;   //!
   TBranch        *b_vxSi_trkResolvedTrack_d0;   //!
   TBranch        *b_vxSi_trkResolvedTrack_z0;   //!
   TBranch        *b_vxSi_trkResolvedTrack_unbiased_d0;   //!
   TBranch        *b_vxSi_trkResolvedTrack_unbiased_z0;   //!
   TBranch        *b_vxSi_trkResolvedTrack_err_unbiased_d0;   //!
   TBranch        *b_vxSi_trkResolvedTrack_err_unbiased_z0;   //!
   TBranch        *b_vxSi_trkResolvedTrack_phi;   //!
   TBranch        *b_vxSi_trkResolvedTrack_theta;   //!
   TBranch        *b_vxSi_trkResolvedTrack_weight;   //!
   TBranch        *b_vxFull_n;   //!
   TBranch        *b_vxFull_x;   //!
   TBranch        *b_vxFull_y;   //!
   TBranch        *b_vxFull_z;   //!
   TBranch        *b_vxFull_chi2;   //!
   TBranch        *b_vxFull_ndof;   //!
   TBranch        *b_vxFull_err_x;   //!
   TBranch        *b_vxFull_err_y;   //!
   TBranch        *b_vxFull_err_z;   //!
   TBranch        *b_vxFull_cov_x;   //!
   TBranch        *b_vxFull_cov_y;   //!
   TBranch        *b_vxFull_cov_z;   //!
   TBranch        *b_vxFull_cov_xy;   //!
   TBranch        *b_vxFull_cov_xz;   //!
   TBranch        *b_vxFull_cov_yz;   //!
   TBranch        *b_vxFull_px;   //!
   TBranch        *b_vxFull_py;   //!
   TBranch        *b_vxFull_pz;   //!
   TBranch        *b_vxFull_E;   //!
   TBranch        *b_vxFull_m;   //!
   TBranch        *b_vxFull_nTracks;   //!
   TBranch        *b_vxFull_sumPt;   //!
   TBranch        *b_vxFull_type;   //!
   TBranch        *b_vxFull_trk_n;   //!
   TBranch        *b_vxFull_trk_chi2;   //!
   TBranch        *b_vxFull_trk_d0;   //!
   TBranch        *b_vxFull_trk_z0;   //!
   TBranch        *b_vxFull_trk_unbiased_d0;   //!
   TBranch        *b_vxFull_trk_unbiased_z0;   //!
   TBranch        *b_vxFull_trk_err_unbiased_d0;   //!
   TBranch        *b_vxFull_trk_err_unbiased_z0;   //!
   TBranch        *b_vxFull_trk_phi;   //!
   TBranch        *b_vxFull_trk_theta;   //!
   TBranch        *b_vxFull_trk_weight;   //!
   TBranch        *b_vxFull_trk_index;   //!
   TBranch        *b_vxFast_n;   //!
   TBranch        *b_vxFast_x;   //!
   TBranch        *b_vxFast_y;   //!
   TBranch        *b_vxFast_z;   //!
   TBranch        *b_vxFast_chi2;   //!
   TBranch        *b_vxFast_ndof;   //!
   TBranch        *b_vxFast_err_x;   //!
   TBranch        *b_vxFast_err_y;   //!
   TBranch        *b_vxFast_err_z;   //!
   TBranch        *b_vxFast_cov_x;   //!
   TBranch        *b_vxFast_cov_y;   //!
   TBranch        *b_vxFast_cov_z;   //!
   TBranch        *b_vxFast_cov_xy;   //!
   TBranch        *b_vxFast_cov_xz;   //!
   TBranch        *b_vxFast_cov_yz;   //!
   TBranch        *b_vxFast_px;   //!
   TBranch        *b_vxFast_py;   //!
   TBranch        *b_vxFast_pz;   //!
   TBranch        *b_vxFast_E;   //!
   TBranch        *b_vxFast_m;   //!
   TBranch        *b_vxFast_nTracks;   //!
   TBranch        *b_vxFast_sumPt;   //!
   TBranch        *b_vxFast_type;   //!
   TBranch        *b_vxFast_trk_n;   //!
   TBranch        *b_vxFast_trk_chi2;   //!
   TBranch        *b_vxFast_trk_d0;   //!
   TBranch        *b_vxFast_trk_z0;   //!
   TBranch        *b_vxFast_trk_unbiased_d0;   //!
   TBranch        *b_vxFast_trk_unbiased_z0;   //!
   TBranch        *b_vxFast_trk_err_unbiased_d0;   //!
   TBranch        *b_vxFast_trk_err_unbiased_z0;   //!
   TBranch        *b_vxFast_trk_phi;   //!
   TBranch        *b_vxFast_trk_theta;   //!
   TBranch        *b_vxFast_trk_weight;   //!
   TBranch        *b_vxFast_trk_index;   //!
   TBranch        *b_vxFullPix_n;   //!
   TBranch        *b_vxFullPix_x;   //!
   TBranch        *b_vxFullPix_y;   //!
   TBranch        *b_vxFullPix_z;   //!
   TBranch        *b_vxFullPix_chi2;   //!
   TBranch        *b_vxFullPix_ndof;   //!
   TBranch        *b_vxFullPix_err_x;   //!
   TBranch        *b_vxFullPix_err_y;   //!
   TBranch        *b_vxFullPix_err_z;   //!
   TBranch        *b_vxFullPix_cov_x;   //!
   TBranch        *b_vxFullPix_cov_y;   //!
   TBranch        *b_vxFullPix_cov_z;   //!
   TBranch        *b_vxFullPix_cov_xy;   //!
   TBranch        *b_vxFullPix_cov_xz;   //!
   TBranch        *b_vxFullPix_cov_yz;   //!
   TBranch        *b_vxFullPix_px;   //!
   TBranch        *b_vxFullPix_py;   //!
   TBranch        *b_vxFullPix_pz;   //!
   TBranch        *b_vxFullPix_E;   //!
   TBranch        *b_vxFullPix_m;   //!
   TBranch        *b_vxFullPix_nTracks;   //!
   TBranch        *b_vxFullPix_sumPt;   //!
   TBranch        *b_vxFullPix_type;   //!
   TBranch        *b_vxFullPix_trkPix_n;   //!
   TBranch        *b_vxFullPix_trkPix_chi2;   //!
   TBranch        *b_vxFullPix_trkPix_d0;   //!
   TBranch        *b_vxFullPix_trkPix_z0;   //!
   TBranch        *b_vxFullPix_trkPix_unbiased_d0;   //!
   TBranch        *b_vxFullPix_trkPix_unbiased_z0;   //!
   TBranch        *b_vxFullPix_trkPix_err_unbiased_d0;   //!
   TBranch        *b_vxFullPix_trkPix_err_unbiased_z0;   //!
   TBranch        *b_vxFullPix_trkPix_phi;   //!
   TBranch        *b_vxFullPix_trkPix_theta;   //!
   TBranch        *b_vxFullPix_trkPix_weight;   //!
   TBranch        *b_vxFastPix_n;   //!
   TBranch        *b_vxFastPix_x;   //!
   TBranch        *b_vxFastPix_y;   //!
   TBranch        *b_vxFastPix_z;   //!
   TBranch        *b_vxFastPix_chi2;   //!
   TBranch        *b_vxFastPix_ndof;   //!
   TBranch        *b_vxFastPix_err_x;   //!
   TBranch        *b_vxFastPix_err_y;   //!
   TBranch        *b_vxFastPix_err_z;   //!
   TBranch        *b_vxFastPix_cov_x;   //!
   TBranch        *b_vxFastPix_cov_y;   //!
   TBranch        *b_vxFastPix_cov_z;   //!
   TBranch        *b_vxFastPix_cov_xy;   //!
   TBranch        *b_vxFastPix_cov_xz;   //!
   TBranch        *b_vxFastPix_cov_yz;   //!
   TBranch        *b_vxFastPix_px;   //!
   TBranch        *b_vxFastPix_py;   //!
   TBranch        *b_vxFastPix_pz;   //!
   TBranch        *b_vxFastPix_E;   //!
   TBranch        *b_vxFastPix_m;   //!
   TBranch        *b_vxFastPix_nTracks;   //!
   TBranch        *b_vxFastPix_sumPt;   //!
   TBranch        *b_vxFastPix_type;   //!
   TBranch        *b_vxFastPix_trkPix_n;   //!
   TBranch        *b_vxFastPix_trkPix_chi2;   //!
   TBranch        *b_vxFastPix_trkPix_d0;   //!
   TBranch        *b_vxFastPix_trkPix_z0;   //!
   TBranch        *b_vxFastPix_trkPix_unbiased_d0;   //!
   TBranch        *b_vxFastPix_trkPix_unbiased_z0;   //!
   TBranch        *b_vxFastPix_trkPix_err_unbiased_d0;   //!
   TBranch        *b_vxFastPix_trkPix_err_unbiased_z0;   //!
   TBranch        *b_vxFastPix_trkPix_phi;   //!
   TBranch        *b_vxFastPix_trkPix_theta;   //!
   TBranch        *b_vxFastPix_trkPix_weight;   //!
   TBranch        *b_vxFullSCT_n;   //!
   TBranch        *b_vxFullSCT_x;   //!
   TBranch        *b_vxFullSCT_y;   //!
   TBranch        *b_vxFullSCT_z;   //!
   TBranch        *b_vxFullSCT_chi2;   //!
   TBranch        *b_vxFullSCT_ndof;   //!
   TBranch        *b_vxFullSCT_err_x;   //!
   TBranch        *b_vxFullSCT_err_y;   //!
   TBranch        *b_vxFullSCT_err_z;   //!
   TBranch        *b_vxFullSCT_cov_x;   //!
   TBranch        *b_vxFullSCT_cov_y;   //!
   TBranch        *b_vxFullSCT_cov_z;   //!
   TBranch        *b_vxFullSCT_cov_xy;   //!
   TBranch        *b_vxFullSCT_cov_xz;   //!
   TBranch        *b_vxFullSCT_cov_yz;   //!
   TBranch        *b_vxFullSCT_px;   //!
   TBranch        *b_vxFullSCT_py;   //!
   TBranch        *b_vxFullSCT_pz;   //!
   TBranch        *b_vxFullSCT_E;   //!
   TBranch        *b_vxFullSCT_m;   //!
   TBranch        *b_vxFullSCT_nTracks;   //!
   TBranch        *b_vxFullSCT_sumPt;   //!
   TBranch        *b_vxFullSCT_type;   //!
   TBranch        *b_vxFullSCT_trkSCT_n;   //!
   TBranch        *b_vxFullSCT_trkSCT_chi2;   //!
   TBranch        *b_vxFullSCT_trkSCT_d0;   //!
   TBranch        *b_vxFullSCT_trkSCT_z0;   //!
   TBranch        *b_vxFullSCT_trkSCT_unbiased_d0;   //!
   TBranch        *b_vxFullSCT_trkSCT_unbiased_z0;   //!
   TBranch        *b_vxFullSCT_trkSCT_err_unbiased_d0;   //!
   TBranch        *b_vxFullSCT_trkSCT_err_unbiased_z0;   //!
   TBranch        *b_vxFullSCT_trkSCT_phi;   //!
   TBranch        *b_vxFullSCT_trkSCT_theta;   //!
   TBranch        *b_vxFullSCT_trkSCT_weight;   //!
   TBranch        *b_vxFastSCT_n;   //!
   TBranch        *b_vxFastSCT_x;   //!
   TBranch        *b_vxFastSCT_y;   //!
   TBranch        *b_vxFastSCT_z;   //!
   TBranch        *b_vxFastSCT_chi2;   //!
   TBranch        *b_vxFastSCT_ndof;   //!
   TBranch        *b_vxFastSCT_err_x;   //!
   TBranch        *b_vxFastSCT_err_y;   //!
   TBranch        *b_vxFastSCT_err_z;   //!
   TBranch        *b_vxFastSCT_cov_x;   //!
   TBranch        *b_vxFastSCT_cov_y;   //!
   TBranch        *b_vxFastSCT_cov_z;   //!
   TBranch        *b_vxFastSCT_cov_xy;   //!
   TBranch        *b_vxFastSCT_cov_xz;   //!
   TBranch        *b_vxFastSCT_cov_yz;   //!
   TBranch        *b_vxFastSCT_px;   //!
   TBranch        *b_vxFastSCT_py;   //!
   TBranch        *b_vxFastSCT_pz;   //!
   TBranch        *b_vxFastSCT_E;   //!
   TBranch        *b_vxFastSCT_m;   //!
   TBranch        *b_vxFastSCT_nTracks;   //!
   TBranch        *b_vxFastSCT_sumPt;   //!
   TBranch        *b_vxFastSCT_type;   //!
   TBranch        *b_vxFastSCT_trkSCT_n;   //!
   TBranch        *b_vxFastSCT_trkSCT_chi2;   //!
   TBranch        *b_vxFastSCT_trkSCT_d0;   //!
   TBranch        *b_vxFastSCT_trkSCT_z0;   //!
   TBranch        *b_vxFastSCT_trkSCT_unbiased_d0;   //!
   TBranch        *b_vxFastSCT_trkSCT_unbiased_z0;   //!
   TBranch        *b_vxFastSCT_trkSCT_err_unbiased_d0;   //!
   TBranch        *b_vxFastSCT_trkSCT_err_unbiased_z0;   //!
   TBranch        *b_vxFastSCT_trkSCT_phi;   //!
   TBranch        *b_vxFastSCT_trkSCT_theta;   //!
   TBranch        *b_vxFastSCT_trkSCT_weight;   //!
   TBranch        *b_vxFullSi_n;   //!
   TBranch        *b_vxFullSi_x;   //!
   TBranch        *b_vxFullSi_y;   //!
   TBranch        *b_vxFullSi_z;   //!
   TBranch        *b_vxFullSi_chi2;   //!
   TBranch        *b_vxFullSi_ndof;   //!
   TBranch        *b_vxFullSi_err_x;   //!
   TBranch        *b_vxFullSi_err_y;   //!
   TBranch        *b_vxFullSi_err_z;   //!
   TBranch        *b_vxFullSi_cov_x;   //!
   TBranch        *b_vxFullSi_cov_y;   //!
   TBranch        *b_vxFullSi_cov_z;   //!
   TBranch        *b_vxFullSi_cov_xy;   //!
   TBranch        *b_vxFullSi_cov_xz;   //!
   TBranch        *b_vxFullSi_cov_yz;   //!
   TBranch        *b_vxFullSi_px;   //!
   TBranch        *b_vxFullSi_py;   //!
   TBranch        *b_vxFullSi_pz;   //!
   TBranch        *b_vxFullSi_E;   //!
   TBranch        *b_vxFullSi_m;   //!
   TBranch        *b_vxFullSi_nTracks;   //!
   TBranch        *b_vxFullSi_sumPt;   //!
   TBranch        *b_vxFullSi_type;   //!
   TBranch        *b_vxFullSi_trkResolvedTrack_n;   //!
   TBranch        *b_vxFullSi_trkResolvedTrack_chi2;   //!
   TBranch        *b_vxFullSi_trkResolvedTrack_d0;   //!
   TBranch        *b_vxFullSi_trkResolvedTrack_z0;   //!
   TBranch        *b_vxFullSi_trkResolvedTrack_unbiased_d0;   //!
   TBranch        *b_vxFullSi_trkResolvedTrack_unbiased_z0;   //!
   TBranch        *b_vxFullSi_trkResolvedTrack_err_unbiased_d0;   //!
   TBranch        *b_vxFullSi_trkResolvedTrack_err_unbiased_z0;   //!
   TBranch        *b_vxFullSi_trkResolvedTrack_phi;   //!
   TBranch        *b_vxFullSi_trkResolvedTrack_theta;   //!
   TBranch        *b_vxFullSi_trkResolvedTrack_weight;   //!
   TBranch        *b_vxFastSi_n;   //!
   TBranch        *b_vxFastSi_x;   //!
   TBranch        *b_vxFastSi_y;   //!
   TBranch        *b_vxFastSi_z;   //!
   TBranch        *b_vxFastSi_chi2;   //!
   TBranch        *b_vxFastSi_ndof;   //!
   TBranch        *b_vxFastSi_err_x;   //!
   TBranch        *b_vxFastSi_err_y;   //!
   TBranch        *b_vxFastSi_err_z;   //!
   TBranch        *b_vxFastSi_cov_x;   //!
   TBranch        *b_vxFastSi_cov_y;   //!
   TBranch        *b_vxFastSi_cov_z;   //!
   TBranch        *b_vxFastSi_cov_xy;   //!
   TBranch        *b_vxFastSi_cov_xz;   //!
   TBranch        *b_vxFastSi_cov_yz;   //!
   TBranch        *b_vxFastSi_px;   //!
   TBranch        *b_vxFastSi_py;   //!
   TBranch        *b_vxFastSi_pz;   //!
   TBranch        *b_vxFastSi_E;   //!
   TBranch        *b_vxFastSi_m;   //!
   TBranch        *b_vxFastSi_nTracks;   //!
   TBranch        *b_vxFastSi_sumPt;   //!
   TBranch        *b_vxFastSi_type;   //!
   TBranch        *b_vxFastSi_trkResolvedTrack_n;   //!
   TBranch        *b_vxFastSi_trkResolvedTrack_chi2;   //!
   TBranch        *b_vxFastSi_trkResolvedTrack_d0;   //!
   TBranch        *b_vxFastSi_trkResolvedTrack_z0;   //!
   TBranch        *b_vxFastSi_trkResolvedTrack_unbiased_d0;   //!
   TBranch        *b_vxFastSi_trkResolvedTrack_unbiased_z0;   //!
   TBranch        *b_vxFastSi_trkResolvedTrack_err_unbiased_d0;   //!
   TBranch        *b_vxFastSi_trkResolvedTrack_err_unbiased_z0;   //!
   TBranch        *b_vxFastSi_trkResolvedTrack_phi;   //!
   TBranch        *b_vxFastSi_trkResolvedTrack_theta;   //!
   TBranch        *b_vxFastSi_trkResolvedTrack_weight;   //!

   VtxNtuple(TTree *tree=0);
   virtual ~VtxNtuple();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef VtxNtuple_cxx
VtxNtuple::VtxNtuple(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("VtxNtuple.root");
      if (!f) {
         f = new TFile("VtxNtuple.root");
      }
      tree = (TTree*)gDirectory->Get("MinBiasTree");

   }
   Init(tree);
}

VtxNtuple::~VtxNtuple()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t VtxNtuple::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t VtxNtuple::LoadTree(Long64_t entry)
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

void VtxNtuple::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   mc_gen_pt = 0;
   mc_gen_eta = 0;
   mc_gen_phi = 0;
   mc_gen_energy = 0;
   mc_perigee_ok = 0;
   mc_perigee_d0 = 0;
   mc_perigee_z0 = 0;
   mc_perigee_phi = 0;
   mc_perigee_theta = 0;
   mc_perigee_qoverp = 0;
   mc_pdg = 0;
   mc_charge = 0;
   mc_barcode = 0;
   mc_status = 0;
   mc_mother_pdg = 0;
   mc_mother_barcode = 0;
   mc_child_n = 0;
   mc_child_barcode = 0;
   mc_begVtx_x = 0;
   mc_begVtx_y = 0;
   mc_begVtx_z = 0;
   mc_begVtx_barcode = 0;
   mc_endVtx_x = 0;
   mc_endVtx_y = 0;
   mc_endVtx_z = 0;
   mc_endVtx_barcode = 0;
   mcVx_x = 0;
   mcVx_y = 0;
   mcVx_z = 0;
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
   trk_err_d0_wrtPV = 0;
   trk_err_z0_wrtPV = 0;
   trk_err_phi_wrtPV = 0;
   trk_err_theta_wrtPV = 0;
   trk_err_qoverp_wrtPV = 0;
   trk_cov_d0_wrtPV = 0;
   trk_cov_z0_wrtPV = 0;
   trk_cov_phi_wrtPV = 0;
   trk_cov_theta_wrtPV = 0;
   trk_cov_qoverp_wrtPV = 0;
   trk_cov_d0_z0_wrtPV = 0;
   trk_cov_d0_phi_wrtPV = 0;
   trk_cov_d0_theta_wrtPV = 0;
   trk_cov_d0_qoverp_wrtPV = 0;
   trk_cov_z0_phi_wrtPV = 0;
   trk_cov_z0_theta_wrtPV = 0;
   trk_cov_z0_qoverp_wrtPV = 0;
   trk_cov_phi_theta_wrtPV = 0;
   trk_cov_phi_qoverp_wrtPV = 0;
   trk_cov_theta_qoverp_wrtPV = 0;
   trk_d0_wrtBS = 0;
   trk_z0_wrtBS = 0;
   trk_phi_wrtBS = 0;
   trk_theta_wrtBS = 0;
   trk_qoverp_wrtBS = 0;
   trk_err_d0_wrtBS = 0;
   trk_err_z0_wrtBS = 0;
   trk_err_phi_wrtBS = 0;
   trk_err_theta_wrtBS = 0;
   trk_err_qoverp_wrtBS = 0;
   trk_cov_d0_wrtBS = 0;
   trk_cov_z0_wrtBS = 0;
   trk_cov_phi_wrtBS = 0;
   trk_cov_theta_wrtBS = 0;
   trk_cov_qoverp_wrtBS = 0;
   trk_cov_d0_z0_wrtBS = 0;
   trk_cov_d0_phi_wrtBS = 0;
   trk_cov_d0_theta_wrtBS = 0;
   trk_cov_d0_qoverp_wrtBS = 0;
   trk_cov_z0_phi_wrtBS = 0;
   trk_cov_z0_theta_wrtBS = 0;
   trk_cov_z0_qoverp_wrtBS = 0;
   trk_cov_phi_theta_wrtBS = 0;
   trk_cov_phi_qoverp_wrtBS = 0;
   trk_cov_theta_qoverp_wrtBS = 0;
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
   trk_hit_n = 0;
   trk_hit_type = 0;
   trk_hit_id = 0;
   trk_hit_detElementId = 0;
   trk_hit_detType = 0;
   trk_hit_bec = 0;
   trk_hit_layer = 0;
   trk_hit_charge = 0;
   trk_hit_sizePhi = 0;
   trk_hit_sizeZ = 0;
   trk_hit_size = 0;
   trk_hit_isCompetingRIO = 0;
   trk_hit_locX = 0;
   trk_hit_locY = 0;
   trk_hit_locPhi = 0;
   trk_hit_locTheta = 0;
   trk_hit_err_locX = 0;
   trk_hit_err_locY = 0;
   trk_hit_cov_locXY = 0;
   trk_hit_x = 0;
   trk_hit_y = 0;
   trk_hit_z = 0;
   trk_hit_trkLocX = 0;
   trk_hit_trkLocY = 0;
   trk_hit_err_trkLocX = 0;
   trk_hit_err_trkLocY = 0;
   trk_hit_cov_trkLocXY = 0;
   trk_hit_chi2 = 0;
   trk_hit_ndof = 0;
   trk_outlier_n = 0;
   trk_outlier_type = 0;
   trk_outlier_id = 0;
   trk_outlier_detElementId = 0;
   trk_outlier_detType = 0;
   trk_outlier_bec = 0;
   trk_outlier_layer = 0;
   trk_outlier_charge = 0;
   trk_outlier_sizePhi = 0;
   trk_outlier_sizeZ = 0;
   trk_outlier_size = 0;
   trk_outlier_isCompetingRIO = 0;
   trk_outlier_locX = 0;
   trk_outlier_locY = 0;
   trk_outlier_locPhi = 0;
   trk_outlier_locTheta = 0;
   trk_outlier_err_locX = 0;
   trk_outlier_err_locY = 0;
   trk_outlier_cov_locXY = 0;
   trk_outlier_x = 0;
   trk_outlier_y = 0;
   trk_outlier_z = 0;
   trk_outlier_trkLocX = 0;
   trk_outlier_trkLocY = 0;
   trk_outlier_err_trkLocX = 0;
   trk_outlier_err_trkLocY = 0;
   trk_outlier_cov_trkLocXY = 0;
   trk_outlier_chi2 = 0;
   trk_outlier_ndof = 0;
   trk_hole_n = 0;
   trk_hole_type = 0;
   trk_hole_id = 0;
   trk_hole_detElementId = 0;
   trk_hole_detType = 0;
   trk_hole_bec = 0;
   trk_hole_layer = 0;
   trk_hole_charge = 0;
   trk_hole_sizePhi = 0;
   trk_hole_sizeZ = 0;
   trk_hole_size = 0;
   trk_hole_isCompetingRIO = 0;
   trk_hole_locX = 0;
   trk_hole_locY = 0;
   trk_hole_locPhi = 0;
   trk_hole_locTheta = 0;
   trk_hole_err_locX = 0;
   trk_hole_err_locY = 0;
   trk_hole_cov_locXY = 0;
   trk_hole_x = 0;
   trk_hole_y = 0;
   trk_hole_z = 0;
   trk_hole_trkLocX = 0;
   trk_hole_trkLocY = 0;
   trk_hole_err_trkLocX = 0;
   trk_hole_err_trkLocY = 0;
   trk_hole_cov_trkLocXY = 0;
   trk_hole_chi2 = 0;
   trk_hole_ndof = 0;
   vx_x = 0;
   vx_y = 0;
   vx_z = 0;
   vx_chi2 = 0;
   vx_ndof = 0;
   vx_err_x = 0;
   vx_err_y = 0;
   vx_err_z = 0;
   vx_cov_x = 0;
   vx_cov_y = 0;
   vx_cov_z = 0;
   vx_cov_xy = 0;
   vx_cov_xz = 0;
   vx_cov_yz = 0;
   vx_px = 0;
   vx_py = 0;
   vx_pz = 0;
   vx_E = 0;
   vx_m = 0;
   vx_nTracks = 0;
   vx_sumPt = 0;
   vx_type = 0;
   vx_trk_n = 0;
   vx_trk_chi2 = 0;
   vx_trk_d0 = 0;
   vx_trk_z0 = 0;
   vx_trk_unbiased_d0 = 0;
   vx_trk_unbiased_z0 = 0;
   vx_trk_err_unbiased_d0 = 0;
   vx_trk_err_unbiased_z0 = 0;
   vx_trk_phi = 0;
   vx_trk_theta = 0;
   vx_trk_weight = 0;
   vx_trk_index = 0;
   vxPix_x = 0;
   vxPix_y = 0;
   vxPix_z = 0;
   vxPix_chi2 = 0;
   vxPix_ndof = 0;
   vxPix_err_x = 0;
   vxPix_err_y = 0;
   vxPix_err_z = 0;
   vxPix_cov_x = 0;
   vxPix_cov_y = 0;
   vxPix_cov_z = 0;
   vxPix_cov_xy = 0;
   vxPix_cov_xz = 0;
   vxPix_cov_yz = 0;
   vxPix_px = 0;
   vxPix_py = 0;
   vxPix_pz = 0;
   vxPix_E = 0;
   vxPix_m = 0;
   vxPix_nTracks = 0;
   vxPix_sumPt = 0;
   vxPix_type = 0;
   vxPix_trkPix_n = 0;
   vxPix_trkPix_chi2 = 0;
   vxPix_trkPix_d0 = 0;
   vxPix_trkPix_z0 = 0;
   vxPix_trkPix_unbiased_d0 = 0;
   vxPix_trkPix_unbiased_z0 = 0;
   vxPix_trkPix_err_unbiased_d0 = 0;
   vxPix_trkPix_err_unbiased_z0 = 0;
   vxPix_trkPix_phi = 0;
   vxPix_trkPix_theta = 0;
   vxPix_trkPix_weight = 0;
   vxSCT_x = 0;
   vxSCT_y = 0;
   vxSCT_z = 0;
   vxSCT_chi2 = 0;
   vxSCT_ndof = 0;
   vxSCT_err_x = 0;
   vxSCT_err_y = 0;
   vxSCT_err_z = 0;
   vxSCT_cov_x = 0;
   vxSCT_cov_y = 0;
   vxSCT_cov_z = 0;
   vxSCT_cov_xy = 0;
   vxSCT_cov_xz = 0;
   vxSCT_cov_yz = 0;
   vxSCT_px = 0;
   vxSCT_py = 0;
   vxSCT_pz = 0;
   vxSCT_E = 0;
   vxSCT_m = 0;
   vxSCT_nTracks = 0;
   vxSCT_sumPt = 0;
   vxSCT_type = 0;
   vxSCT_trkSCT_n = 0;
   vxSCT_trkSCT_chi2 = 0;
   vxSCT_trkSCT_d0 = 0;
   vxSCT_trkSCT_z0 = 0;
   vxSCT_trkSCT_unbiased_d0 = 0;
   vxSCT_trkSCT_unbiased_z0 = 0;
   vxSCT_trkSCT_err_unbiased_d0 = 0;
   vxSCT_trkSCT_err_unbiased_z0 = 0;
   vxSCT_trkSCT_phi = 0;
   vxSCT_trkSCT_theta = 0;
   vxSCT_trkSCT_weight = 0;
   vxSi_x = 0;
   vxSi_y = 0;
   vxSi_z = 0;
   vxSi_chi2 = 0;
   vxSi_ndof = 0;
   vxSi_err_x = 0;
   vxSi_err_y = 0;
   vxSi_err_z = 0;
   vxSi_cov_x = 0;
   vxSi_cov_y = 0;
   vxSi_cov_z = 0;
   vxSi_cov_xy = 0;
   vxSi_cov_xz = 0;
   vxSi_cov_yz = 0;
   vxSi_px = 0;
   vxSi_py = 0;
   vxSi_pz = 0;
   vxSi_E = 0;
   vxSi_m = 0;
   vxSi_nTracks = 0;
   vxSi_sumPt = 0;
   vxSi_type = 0;
   vxSi_trkResolvedTrack_n = 0;
   vxSi_trkResolvedTrack_chi2 = 0;
   vxSi_trkResolvedTrack_d0 = 0;
   vxSi_trkResolvedTrack_z0 = 0;
   vxSi_trkResolvedTrack_unbiased_d0 = 0;
   vxSi_trkResolvedTrack_unbiased_z0 = 0;
   vxSi_trkResolvedTrack_err_unbiased_d0 = 0;
   vxSi_trkResolvedTrack_err_unbiased_z0 = 0;
   vxSi_trkResolvedTrack_phi = 0;
   vxSi_trkResolvedTrack_theta = 0;
   vxSi_trkResolvedTrack_weight = 0;
   vxFull_x = 0;
   vxFull_y = 0;
   vxFull_z = 0;
   vxFull_chi2 = 0;
   vxFull_ndof = 0;
   vxFull_err_x = 0;
   vxFull_err_y = 0;
   vxFull_err_z = 0;
   vxFull_cov_x = 0;
   vxFull_cov_y = 0;
   vxFull_cov_z = 0;
   vxFull_cov_xy = 0;
   vxFull_cov_xz = 0;
   vxFull_cov_yz = 0;
   vxFull_px = 0;
   vxFull_py = 0;
   vxFull_pz = 0;
   vxFull_E = 0;
   vxFull_m = 0;
   vxFull_nTracks = 0;
   vxFull_sumPt = 0;
   vxFull_type = 0;
   vxFull_trk_n = 0;
   vxFull_trk_chi2 = 0;
   vxFull_trk_d0 = 0;
   vxFull_trk_z0 = 0;
   vxFull_trk_unbiased_d0 = 0;
   vxFull_trk_unbiased_z0 = 0;
   vxFull_trk_err_unbiased_d0 = 0;
   vxFull_trk_err_unbiased_z0 = 0;
   vxFull_trk_phi = 0;
   vxFull_trk_theta = 0;
   vxFull_trk_weight = 0;
   vxFull_trk_index = 0;
   vxFast_x = 0;
   vxFast_y = 0;
   vxFast_z = 0;
   vxFast_chi2 = 0;
   vxFast_ndof = 0;
   vxFast_err_x = 0;
   vxFast_err_y = 0;
   vxFast_err_z = 0;
   vxFast_cov_x = 0;
   vxFast_cov_y = 0;
   vxFast_cov_z = 0;
   vxFast_cov_xy = 0;
   vxFast_cov_xz = 0;
   vxFast_cov_yz = 0;
   vxFast_px = 0;
   vxFast_py = 0;
   vxFast_pz = 0;
   vxFast_E = 0;
   vxFast_m = 0;
   vxFast_nTracks = 0;
   vxFast_sumPt = 0;
   vxFast_type = 0;
   vxFast_trk_n = 0;
   vxFast_trk_chi2 = 0;
   vxFast_trk_d0 = 0;
   vxFast_trk_z0 = 0;
   vxFast_trk_unbiased_d0 = 0;
   vxFast_trk_unbiased_z0 = 0;
   vxFast_trk_err_unbiased_d0 = 0;
   vxFast_trk_err_unbiased_z0 = 0;
   vxFast_trk_phi = 0;
   vxFast_trk_theta = 0;
   vxFast_trk_weight = 0;
   vxFast_trk_index = 0;
   vxFullPix_x = 0;
   vxFullPix_y = 0;
   vxFullPix_z = 0;
   vxFullPix_chi2 = 0;
   vxFullPix_ndof = 0;
   vxFullPix_err_x = 0;
   vxFullPix_err_y = 0;
   vxFullPix_err_z = 0;
   vxFullPix_cov_x = 0;
   vxFullPix_cov_y = 0;
   vxFullPix_cov_z = 0;
   vxFullPix_cov_xy = 0;
   vxFullPix_cov_xz = 0;
   vxFullPix_cov_yz = 0;
   vxFullPix_px = 0;
   vxFullPix_py = 0;
   vxFullPix_pz = 0;
   vxFullPix_E = 0;
   vxFullPix_m = 0;
   vxFullPix_nTracks = 0;
   vxFullPix_sumPt = 0;
   vxFullPix_type = 0;
   vxFullPix_trkPix_n = 0;
   vxFullPix_trkPix_chi2 = 0;
   vxFullPix_trkPix_d0 = 0;
   vxFullPix_trkPix_z0 = 0;
   vxFullPix_trkPix_unbiased_d0 = 0;
   vxFullPix_trkPix_unbiased_z0 = 0;
   vxFullPix_trkPix_err_unbiased_d0 = 0;
   vxFullPix_trkPix_err_unbiased_z0 = 0;
   vxFullPix_trkPix_phi = 0;
   vxFullPix_trkPix_theta = 0;
   vxFullPix_trkPix_weight = 0;
   vxFastPix_x = 0;
   vxFastPix_y = 0;
   vxFastPix_z = 0;
   vxFastPix_chi2 = 0;
   vxFastPix_ndof = 0;
   vxFastPix_err_x = 0;
   vxFastPix_err_y = 0;
   vxFastPix_err_z = 0;
   vxFastPix_cov_x = 0;
   vxFastPix_cov_y = 0;
   vxFastPix_cov_z = 0;
   vxFastPix_cov_xy = 0;
   vxFastPix_cov_xz = 0;
   vxFastPix_cov_yz = 0;
   vxFastPix_px = 0;
   vxFastPix_py = 0;
   vxFastPix_pz = 0;
   vxFastPix_E = 0;
   vxFastPix_m = 0;
   vxFastPix_nTracks = 0;
   vxFastPix_sumPt = 0;
   vxFastPix_type = 0;
   vxFastPix_trkPix_n = 0;
   vxFastPix_trkPix_chi2 = 0;
   vxFastPix_trkPix_d0 = 0;
   vxFastPix_trkPix_z0 = 0;
   vxFastPix_trkPix_unbiased_d0 = 0;
   vxFastPix_trkPix_unbiased_z0 = 0;
   vxFastPix_trkPix_err_unbiased_d0 = 0;
   vxFastPix_trkPix_err_unbiased_z0 = 0;
   vxFastPix_trkPix_phi = 0;
   vxFastPix_trkPix_theta = 0;
   vxFastPix_trkPix_weight = 0;
   vxFullSCT_x = 0;
   vxFullSCT_y = 0;
   vxFullSCT_z = 0;
   vxFullSCT_chi2 = 0;
   vxFullSCT_ndof = 0;
   vxFullSCT_err_x = 0;
   vxFullSCT_err_y = 0;
   vxFullSCT_err_z = 0;
   vxFullSCT_cov_x = 0;
   vxFullSCT_cov_y = 0;
   vxFullSCT_cov_z = 0;
   vxFullSCT_cov_xy = 0;
   vxFullSCT_cov_xz = 0;
   vxFullSCT_cov_yz = 0;
   vxFullSCT_px = 0;
   vxFullSCT_py = 0;
   vxFullSCT_pz = 0;
   vxFullSCT_E = 0;
   vxFullSCT_m = 0;
   vxFullSCT_nTracks = 0;
   vxFullSCT_sumPt = 0;
   vxFullSCT_type = 0;
   vxFullSCT_trkSCT_n = 0;
   vxFullSCT_trkSCT_chi2 = 0;
   vxFullSCT_trkSCT_d0 = 0;
   vxFullSCT_trkSCT_z0 = 0;
   vxFullSCT_trkSCT_unbiased_d0 = 0;
   vxFullSCT_trkSCT_unbiased_z0 = 0;
   vxFullSCT_trkSCT_err_unbiased_d0 = 0;
   vxFullSCT_trkSCT_err_unbiased_z0 = 0;
   vxFullSCT_trkSCT_phi = 0;
   vxFullSCT_trkSCT_theta = 0;
   vxFullSCT_trkSCT_weight = 0;
   vxFastSCT_x = 0;
   vxFastSCT_y = 0;
   vxFastSCT_z = 0;
   vxFastSCT_chi2 = 0;
   vxFastSCT_ndof = 0;
   vxFastSCT_err_x = 0;
   vxFastSCT_err_y = 0;
   vxFastSCT_err_z = 0;
   vxFastSCT_cov_x = 0;
   vxFastSCT_cov_y = 0;
   vxFastSCT_cov_z = 0;
   vxFastSCT_cov_xy = 0;
   vxFastSCT_cov_xz = 0;
   vxFastSCT_cov_yz = 0;
   vxFastSCT_px = 0;
   vxFastSCT_py = 0;
   vxFastSCT_pz = 0;
   vxFastSCT_E = 0;
   vxFastSCT_m = 0;
   vxFastSCT_nTracks = 0;
   vxFastSCT_sumPt = 0;
   vxFastSCT_type = 0;
   vxFastSCT_trkSCT_n = 0;
   vxFastSCT_trkSCT_chi2 = 0;
   vxFastSCT_trkSCT_d0 = 0;
   vxFastSCT_trkSCT_z0 = 0;
   vxFastSCT_trkSCT_unbiased_d0 = 0;
   vxFastSCT_trkSCT_unbiased_z0 = 0;
   vxFastSCT_trkSCT_err_unbiased_d0 = 0;
   vxFastSCT_trkSCT_err_unbiased_z0 = 0;
   vxFastSCT_trkSCT_phi = 0;
   vxFastSCT_trkSCT_theta = 0;
   vxFastSCT_trkSCT_weight = 0;
   vxFullSi_x = 0;
   vxFullSi_y = 0;
   vxFullSi_z = 0;
   vxFullSi_chi2 = 0;
   vxFullSi_ndof = 0;
   vxFullSi_err_x = 0;
   vxFullSi_err_y = 0;
   vxFullSi_err_z = 0;
   vxFullSi_cov_x = 0;
   vxFullSi_cov_y = 0;
   vxFullSi_cov_z = 0;
   vxFullSi_cov_xy = 0;
   vxFullSi_cov_xz = 0;
   vxFullSi_cov_yz = 0;
   vxFullSi_px = 0;
   vxFullSi_py = 0;
   vxFullSi_pz = 0;
   vxFullSi_E = 0;
   vxFullSi_m = 0;
   vxFullSi_nTracks = 0;
   vxFullSi_sumPt = 0;
   vxFullSi_type = 0;
   vxFullSi_trkResolvedTrack_n = 0;
   vxFullSi_trkResolvedTrack_chi2 = 0;
   vxFullSi_trkResolvedTrack_d0 = 0;
   vxFullSi_trkResolvedTrack_z0 = 0;
   vxFullSi_trkResolvedTrack_unbiased_d0 = 0;
   vxFullSi_trkResolvedTrack_unbiased_z0 = 0;
   vxFullSi_trkResolvedTrack_err_unbiased_d0 = 0;
   vxFullSi_trkResolvedTrack_err_unbiased_z0 = 0;
   vxFullSi_trkResolvedTrack_phi = 0;
   vxFullSi_trkResolvedTrack_theta = 0;
   vxFullSi_trkResolvedTrack_weight = 0;
   vxFastSi_x = 0;
   vxFastSi_y = 0;
   vxFastSi_z = 0;
   vxFastSi_chi2 = 0;
   vxFastSi_ndof = 0;
   vxFastSi_err_x = 0;
   vxFastSi_err_y = 0;
   vxFastSi_err_z = 0;
   vxFastSi_cov_x = 0;
   vxFastSi_cov_y = 0;
   vxFastSi_cov_z = 0;
   vxFastSi_cov_xy = 0;
   vxFastSi_cov_xz = 0;
   vxFastSi_cov_yz = 0;
   vxFastSi_px = 0;
   vxFastSi_py = 0;
   vxFastSi_pz = 0;
   vxFastSi_E = 0;
   vxFastSi_m = 0;
   vxFastSi_nTracks = 0;
   vxFastSi_sumPt = 0;
   vxFastSi_type = 0;
   vxFastSi_trkResolvedTrack_n = 0;
   vxFastSi_trkResolvedTrack_chi2 = 0;
   vxFastSi_trkResolvedTrack_d0 = 0;
   vxFastSi_trkResolvedTrack_z0 = 0;
   vxFastSi_trkResolvedTrack_unbiased_d0 = 0;
   vxFastSi_trkResolvedTrack_unbiased_z0 = 0;
   vxFastSi_trkResolvedTrack_err_unbiased_d0 = 0;
   vxFastSi_trkResolvedTrack_err_unbiased_z0 = 0;
   vxFastSi_trkResolvedTrack_phi = 0;
   vxFastSi_trkResolvedTrack_theta = 0;
   vxFastSi_trkResolvedTrack_weight = 0;
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
   fChain->SetBranchAddress("mc_n", &mc_n, &b_mc_n);
   fChain->SetBranchAddress("mc_gen_pt", &mc_gen_pt, &b_mc_gen_pt);
   fChain->SetBranchAddress("mc_gen_eta", &mc_gen_eta, &b_mc_gen_eta);
   fChain->SetBranchAddress("mc_gen_phi", &mc_gen_phi, &b_mc_gen_phi);
   fChain->SetBranchAddress("mc_gen_energy", &mc_gen_energy, &b_mc_gen_energy);
   fChain->SetBranchAddress("mc_perigee_ok", &mc_perigee_ok, &b_mc_perigee_ok);
   fChain->SetBranchAddress("mc_perigee_d0", &mc_perigee_d0, &b_mc_perigee_d0);
   fChain->SetBranchAddress("mc_perigee_z0", &mc_perigee_z0, &b_mc_perigee_z0);
   fChain->SetBranchAddress("mc_perigee_phi", &mc_perigee_phi, &b_mc_perigee_phi);
   fChain->SetBranchAddress("mc_perigee_theta", &mc_perigee_theta, &b_mc_perigee_theta);
   fChain->SetBranchAddress("mc_perigee_qoverp", &mc_perigee_qoverp, &b_mc_perigee_qoverp);
   fChain->SetBranchAddress("mc_pdg", &mc_pdg, &b_mc_pdg);
   fChain->SetBranchAddress("mc_charge", &mc_charge, &b_mc_charge);
   fChain->SetBranchAddress("mc_barcode", &mc_barcode, &b_mc_barcode);
   fChain->SetBranchAddress("mc_status", &mc_status, &b_mc_status);
   fChain->SetBranchAddress("mc_mother_pdg", &mc_mother_pdg, &b_mc_mother_pdg);
   fChain->SetBranchAddress("mc_mother_barcode", &mc_mother_barcode, &b_mc_mother_barcode);
   fChain->SetBranchAddress("mc_child_n", &mc_child_n, &b_mc_child_n);
   fChain->SetBranchAddress("mc_child_barcode", &mc_child_barcode, &b_mc_child_barcode);
   fChain->SetBranchAddress("mc_begVtx_x", &mc_begVtx_x, &b_mc_begVtx_x);
   fChain->SetBranchAddress("mc_begVtx_y", &mc_begVtx_y, &b_mc_begVtx_y);
   fChain->SetBranchAddress("mc_begVtx_z", &mc_begVtx_z, &b_mc_begVtx_z);
   fChain->SetBranchAddress("mc_begVtx_barcode", &mc_begVtx_barcode, &b_mc_begVtx_barcode);
   fChain->SetBranchAddress("mc_endVtx_x", &mc_endVtx_x, &b_mc_endVtx_x);
   fChain->SetBranchAddress("mc_endVtx_y", &mc_endVtx_y, &b_mc_endVtx_y);
   fChain->SetBranchAddress("mc_endVtx_z", &mc_endVtx_z, &b_mc_endVtx_z);
   fChain->SetBranchAddress("mc_endVtx_barcode", &mc_endVtx_barcode, &b_mc_endVtx_barcode);
   fChain->SetBranchAddress("mcVx_n", &mcVx_n, &b_mcVx_n);
   fChain->SetBranchAddress("mcVx_x", &mcVx_x, &b_mcVx_x);
   fChain->SetBranchAddress("mcVx_y", &mcVx_y, &b_mcVx_y);
   fChain->SetBranchAddress("mcVx_z", &mcVx_z, &b_mcVx_z);
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
   fChain->SetBranchAddress("trk_err_d0_wrtPV", &trk_err_d0_wrtPV, &b_trk_err_d0_wrtPV);
   fChain->SetBranchAddress("trk_err_z0_wrtPV", &trk_err_z0_wrtPV, &b_trk_err_z0_wrtPV);
   fChain->SetBranchAddress("trk_err_phi_wrtPV", &trk_err_phi_wrtPV, &b_trk_err_phi_wrtPV);
   fChain->SetBranchAddress("trk_err_theta_wrtPV", &trk_err_theta_wrtPV, &b_trk_err_theta_wrtPV);
   fChain->SetBranchAddress("trk_err_qoverp_wrtPV", &trk_err_qoverp_wrtPV, &b_trk_err_qoverp_wrtPV);
   fChain->SetBranchAddress("trk_cov_d0_wrtPV", &trk_cov_d0_wrtPV, &b_trk_cov_d0_wrtPV);
   fChain->SetBranchAddress("trk_cov_z0_wrtPV", &trk_cov_z0_wrtPV, &b_trk_cov_z0_wrtPV);
   fChain->SetBranchAddress("trk_cov_phi_wrtPV", &trk_cov_phi_wrtPV, &b_trk_cov_phi_wrtPV);
   fChain->SetBranchAddress("trk_cov_theta_wrtPV", &trk_cov_theta_wrtPV, &b_trk_cov_theta_wrtPV);
   fChain->SetBranchAddress("trk_cov_qoverp_wrtPV", &trk_cov_qoverp_wrtPV, &b_trk_cov_qoverp_wrtPV);
   fChain->SetBranchAddress("trk_cov_d0_z0_wrtPV", &trk_cov_d0_z0_wrtPV, &b_trk_cov_d0_z0_wrtPV);
   fChain->SetBranchAddress("trk_cov_d0_phi_wrtPV", &trk_cov_d0_phi_wrtPV, &b_trk_cov_d0_phi_wrtPV);
   fChain->SetBranchAddress("trk_cov_d0_theta_wrtPV", &trk_cov_d0_theta_wrtPV, &b_trk_cov_d0_theta_wrtPV);
   fChain->SetBranchAddress("trk_cov_d0_qoverp_wrtPV", &trk_cov_d0_qoverp_wrtPV, &b_trk_cov_d0_qoverp_wrtPV);
   fChain->SetBranchAddress("trk_cov_z0_phi_wrtPV", &trk_cov_z0_phi_wrtPV, &b_trk_cov_z0_phi_wrtPV);
   fChain->SetBranchAddress("trk_cov_z0_theta_wrtPV", &trk_cov_z0_theta_wrtPV, &b_trk_cov_z0_theta_wrtPV);
   fChain->SetBranchAddress("trk_cov_z0_qoverp_wrtPV", &trk_cov_z0_qoverp_wrtPV, &b_trk_cov_z0_qoverp_wrtPV);
   fChain->SetBranchAddress("trk_cov_phi_theta_wrtPV", &trk_cov_phi_theta_wrtPV, &b_trk_cov_phi_theta_wrtPV);
   fChain->SetBranchAddress("trk_cov_phi_qoverp_wrtPV", &trk_cov_phi_qoverp_wrtPV, &b_trk_cov_phi_qoverp_wrtPV);
   fChain->SetBranchAddress("trk_cov_theta_qoverp_wrtPV", &trk_cov_theta_qoverp_wrtPV, &b_trk_cov_theta_qoverp_wrtPV);
   fChain->SetBranchAddress("trk_d0_wrtBS", &trk_d0_wrtBS, &b_trk_d0_wrtBS);
   fChain->SetBranchAddress("trk_z0_wrtBS", &trk_z0_wrtBS, &b_trk_z0_wrtBS);
   fChain->SetBranchAddress("trk_phi_wrtBS", &trk_phi_wrtBS, &b_trk_phi_wrtBS);
   fChain->SetBranchAddress("trk_theta_wrtBS", &trk_theta_wrtBS, &b_trk_theta_wrtBS);
   fChain->SetBranchAddress("trk_qoverp_wrtBS", &trk_qoverp_wrtBS, &b_trk_qoverp_wrtBS);
   fChain->SetBranchAddress("trk_err_d0_wrtBS", &trk_err_d0_wrtBS, &b_trk_err_d0_wrtBS);
   fChain->SetBranchAddress("trk_err_z0_wrtBS", &trk_err_z0_wrtBS, &b_trk_err_z0_wrtBS);
   fChain->SetBranchAddress("trk_err_phi_wrtBS", &trk_err_phi_wrtBS, &b_trk_err_phi_wrtBS);
   fChain->SetBranchAddress("trk_err_theta_wrtBS", &trk_err_theta_wrtBS, &b_trk_err_theta_wrtBS);
   fChain->SetBranchAddress("trk_err_qoverp_wrtBS", &trk_err_qoverp_wrtBS, &b_trk_err_qoverp_wrtBS);
   fChain->SetBranchAddress("trk_cov_d0_wrtBS", &trk_cov_d0_wrtBS, &b_trk_cov_d0_wrtBS);
   fChain->SetBranchAddress("trk_cov_z0_wrtBS", &trk_cov_z0_wrtBS, &b_trk_cov_z0_wrtBS);
   fChain->SetBranchAddress("trk_cov_phi_wrtBS", &trk_cov_phi_wrtBS, &b_trk_cov_phi_wrtBS);
   fChain->SetBranchAddress("trk_cov_theta_wrtBS", &trk_cov_theta_wrtBS, &b_trk_cov_theta_wrtBS);
   fChain->SetBranchAddress("trk_cov_qoverp_wrtBS", &trk_cov_qoverp_wrtBS, &b_trk_cov_qoverp_wrtBS);
   fChain->SetBranchAddress("trk_cov_d0_z0_wrtBS", &trk_cov_d0_z0_wrtBS, &b_trk_cov_d0_z0_wrtBS);
   fChain->SetBranchAddress("trk_cov_d0_phi_wrtBS", &trk_cov_d0_phi_wrtBS, &b_trk_cov_d0_phi_wrtBS);
   fChain->SetBranchAddress("trk_cov_d0_theta_wrtBS", &trk_cov_d0_theta_wrtBS, &b_trk_cov_d0_theta_wrtBS);
   fChain->SetBranchAddress("trk_cov_d0_qoverp_wrtBS", &trk_cov_d0_qoverp_wrtBS, &b_trk_cov_d0_qoverp_wrtBS);
   fChain->SetBranchAddress("trk_cov_z0_phi_wrtBS", &trk_cov_z0_phi_wrtBS, &b_trk_cov_z0_phi_wrtBS);
   fChain->SetBranchAddress("trk_cov_z0_theta_wrtBS", &trk_cov_z0_theta_wrtBS, &b_trk_cov_z0_theta_wrtBS);
   fChain->SetBranchAddress("trk_cov_z0_qoverp_wrtBS", &trk_cov_z0_qoverp_wrtBS, &b_trk_cov_z0_qoverp_wrtBS);
   fChain->SetBranchAddress("trk_cov_phi_theta_wrtBS", &trk_cov_phi_theta_wrtBS, &b_trk_cov_phi_theta_wrtBS);
   fChain->SetBranchAddress("trk_cov_phi_qoverp_wrtBS", &trk_cov_phi_qoverp_wrtBS, &b_trk_cov_phi_qoverp_wrtBS);
   fChain->SetBranchAddress("trk_cov_theta_qoverp_wrtBS", &trk_cov_theta_qoverp_wrtBS, &b_trk_cov_theta_qoverp_wrtBS);
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
   fChain->SetBranchAddress("trk_hit_n", &trk_hit_n, &b_trk_hit_n);
   fChain->SetBranchAddress("trk_hit_type", &trk_hit_type, &b_trk_hit_type);
   fChain->SetBranchAddress("trk_hit_id", &trk_hit_id, &b_trk_hit_id);
   fChain->SetBranchAddress("trk_hit_detElementId", &trk_hit_detElementId, &b_trk_hit_detElementId);
   fChain->SetBranchAddress("trk_hit_detType", &trk_hit_detType, &b_trk_hit_detType);
   fChain->SetBranchAddress("trk_hit_bec", &trk_hit_bec, &b_trk_hit_bec);
   fChain->SetBranchAddress("trk_hit_layer", &trk_hit_layer, &b_trk_hit_layer);
   fChain->SetBranchAddress("trk_hit_charge", &trk_hit_charge, &b_trk_hit_charge);
   fChain->SetBranchAddress("trk_hit_sizePhi", &trk_hit_sizePhi, &b_trk_hit_sizePhi);
   fChain->SetBranchAddress("trk_hit_sizeZ", &trk_hit_sizeZ, &b_trk_hit_sizeZ);
   fChain->SetBranchAddress("trk_hit_size", &trk_hit_size, &b_trk_hit_size);
   fChain->SetBranchAddress("trk_hit_isCompetingRIO", &trk_hit_isCompetingRIO, &b_trk_hit_isCompetingRIO);
   fChain->SetBranchAddress("trk_hit_locX", &trk_hit_locX, &b_trk_hit_locX);
   fChain->SetBranchAddress("trk_hit_locY", &trk_hit_locY, &b_trk_hit_locY);
   fChain->SetBranchAddress("trk_hit_locPhi", &trk_hit_locPhi, &b_trk_hit_locPhi);
   fChain->SetBranchAddress("trk_hit_locTheta", &trk_hit_locTheta, &b_trk_hit_locTheta);
   fChain->SetBranchAddress("trk_hit_err_locX", &trk_hit_err_locX, &b_trk_hit_err_locX);
   fChain->SetBranchAddress("trk_hit_err_locY", &trk_hit_err_locY, &b_trk_hit_err_locY);
   fChain->SetBranchAddress("trk_hit_cov_locXY", &trk_hit_cov_locXY, &b_trk_hit_cov_locXY);
   fChain->SetBranchAddress("trk_hit_x", &trk_hit_x, &b_trk_hit_x);
   fChain->SetBranchAddress("trk_hit_y", &trk_hit_y, &b_trk_hit_y);
   fChain->SetBranchAddress("trk_hit_z", &trk_hit_z, &b_trk_hit_z);
   fChain->SetBranchAddress("trk_hit_trkLocX", &trk_hit_trkLocX, &b_trk_hit_trkLocX);
   fChain->SetBranchAddress("trk_hit_trkLocY", &trk_hit_trkLocY, &b_trk_hit_trkLocY);
   fChain->SetBranchAddress("trk_hit_err_trkLocX", &trk_hit_err_trkLocX, &b_trk_hit_err_trkLocX);
   fChain->SetBranchAddress("trk_hit_err_trkLocY", &trk_hit_err_trkLocY, &b_trk_hit_err_trkLocY);
   fChain->SetBranchAddress("trk_hit_cov_trkLocXY", &trk_hit_cov_trkLocXY, &b_trk_hit_cov_trkLocXY);
   fChain->SetBranchAddress("trk_hit_chi2", &trk_hit_chi2, &b_trk_hit_chi2);
   fChain->SetBranchAddress("trk_hit_ndof", &trk_hit_ndof, &b_trk_hit_ndof);
   fChain->SetBranchAddress("trk_outlier_n", &trk_outlier_n, &b_trk_outlier_n);
   fChain->SetBranchAddress("trk_outlier_type", &trk_outlier_type, &b_trk_outlier_type);
   fChain->SetBranchAddress("trk_outlier_id", &trk_outlier_id, &b_trk_outlier_id);
   fChain->SetBranchAddress("trk_outlier_detElementId", &trk_outlier_detElementId, &b_trk_outlier_detElementId);
   fChain->SetBranchAddress("trk_outlier_detType", &trk_outlier_detType, &b_trk_outlier_detType);
   fChain->SetBranchAddress("trk_outlier_bec", &trk_outlier_bec, &b_trk_outlier_bec);
   fChain->SetBranchAddress("trk_outlier_layer", &trk_outlier_layer, &b_trk_outlier_layer);
   fChain->SetBranchAddress("trk_outlier_charge", &trk_outlier_charge, &b_trk_outlier_charge);
   fChain->SetBranchAddress("trk_outlier_sizePhi", &trk_outlier_sizePhi, &b_trk_outlier_sizePhi);
   fChain->SetBranchAddress("trk_outlier_sizeZ", &trk_outlier_sizeZ, &b_trk_outlier_sizeZ);
   fChain->SetBranchAddress("trk_outlier_size", &trk_outlier_size, &b_trk_outlier_size);
   fChain->SetBranchAddress("trk_outlier_isCompetingRIO", &trk_outlier_isCompetingRIO, &b_trk_outlier_isCompetingRIO);
   fChain->SetBranchAddress("trk_outlier_locX", &trk_outlier_locX, &b_trk_outlier_locX);
   fChain->SetBranchAddress("trk_outlier_locY", &trk_outlier_locY, &b_trk_outlier_locY);
   fChain->SetBranchAddress("trk_outlier_locPhi", &trk_outlier_locPhi, &b_trk_outlier_locPhi);
   fChain->SetBranchAddress("trk_outlier_locTheta", &trk_outlier_locTheta, &b_trk_outlier_locTheta);
   fChain->SetBranchAddress("trk_outlier_err_locX", &trk_outlier_err_locX, &b_trk_outlier_err_locX);
   fChain->SetBranchAddress("trk_outlier_err_locY", &trk_outlier_err_locY, &b_trk_outlier_err_locY);
   fChain->SetBranchAddress("trk_outlier_cov_locXY", &trk_outlier_cov_locXY, &b_trk_outlier_cov_locXY);
   fChain->SetBranchAddress("trk_outlier_x", &trk_outlier_x, &b_trk_outlier_x);
   fChain->SetBranchAddress("trk_outlier_y", &trk_outlier_y, &b_trk_outlier_y);
   fChain->SetBranchAddress("trk_outlier_z", &trk_outlier_z, &b_trk_outlier_z);
   fChain->SetBranchAddress("trk_outlier_trkLocX", &trk_outlier_trkLocX, &b_trk_outlier_trkLocX);
   fChain->SetBranchAddress("trk_outlier_trkLocY", &trk_outlier_trkLocY, &b_trk_outlier_trkLocY);
   fChain->SetBranchAddress("trk_outlier_err_trkLocX", &trk_outlier_err_trkLocX, &b_trk_outlier_err_trkLocX);
   fChain->SetBranchAddress("trk_outlier_err_trkLocY", &trk_outlier_err_trkLocY, &b_trk_outlier_err_trkLocY);
   fChain->SetBranchAddress("trk_outlier_cov_trkLocXY", &trk_outlier_cov_trkLocXY, &b_trk_outlier_cov_trkLocXY);
   fChain->SetBranchAddress("trk_outlier_chi2", &trk_outlier_chi2, &b_trk_outlier_chi2);
   fChain->SetBranchAddress("trk_outlier_ndof", &trk_outlier_ndof, &b_trk_outlier_ndof);
   fChain->SetBranchAddress("trk_hole_n", &trk_hole_n, &b_trk_hole_n);
   fChain->SetBranchAddress("trk_hole_type", &trk_hole_type, &b_trk_hole_type);
   fChain->SetBranchAddress("trk_hole_id", &trk_hole_id, &b_trk_hole_id);
   fChain->SetBranchAddress("trk_hole_detElementId", &trk_hole_detElementId, &b_trk_hole_detElementId);
   fChain->SetBranchAddress("trk_hole_detType", &trk_hole_detType, &b_trk_hole_detType);
   fChain->SetBranchAddress("trk_hole_bec", &trk_hole_bec, &b_trk_hole_bec);
   fChain->SetBranchAddress("trk_hole_layer", &trk_hole_layer, &b_trk_hole_layer);
   fChain->SetBranchAddress("trk_hole_charge", &trk_hole_charge, &b_trk_hole_charge);
   fChain->SetBranchAddress("trk_hole_sizePhi", &trk_hole_sizePhi, &b_trk_hole_sizePhi);
   fChain->SetBranchAddress("trk_hole_sizeZ", &trk_hole_sizeZ, &b_trk_hole_sizeZ);
   fChain->SetBranchAddress("trk_hole_size", &trk_hole_size, &b_trk_hole_size);
   fChain->SetBranchAddress("trk_hole_isCompetingRIO", &trk_hole_isCompetingRIO, &b_trk_hole_isCompetingRIO);
   fChain->SetBranchAddress("trk_hole_locX", &trk_hole_locX, &b_trk_hole_locX);
   fChain->SetBranchAddress("trk_hole_locY", &trk_hole_locY, &b_trk_hole_locY);
   fChain->SetBranchAddress("trk_hole_locPhi", &trk_hole_locPhi, &b_trk_hole_locPhi);
   fChain->SetBranchAddress("trk_hole_locTheta", &trk_hole_locTheta, &b_trk_hole_locTheta);
   fChain->SetBranchAddress("trk_hole_err_locX", &trk_hole_err_locX, &b_trk_hole_err_locX);
   fChain->SetBranchAddress("trk_hole_err_locY", &trk_hole_err_locY, &b_trk_hole_err_locY);
   fChain->SetBranchAddress("trk_hole_cov_locXY", &trk_hole_cov_locXY, &b_trk_hole_cov_locXY);
   fChain->SetBranchAddress("trk_hole_x", &trk_hole_x, &b_trk_hole_x);
   fChain->SetBranchAddress("trk_hole_y", &trk_hole_y, &b_trk_hole_y);
   fChain->SetBranchAddress("trk_hole_z", &trk_hole_z, &b_trk_hole_z);
   fChain->SetBranchAddress("trk_hole_trkLocX", &trk_hole_trkLocX, &b_trk_hole_trkLocX);
   fChain->SetBranchAddress("trk_hole_trkLocY", &trk_hole_trkLocY, &b_trk_hole_trkLocY);
   fChain->SetBranchAddress("trk_hole_err_trkLocX", &trk_hole_err_trkLocX, &b_trk_hole_err_trkLocX);
   fChain->SetBranchAddress("trk_hole_err_trkLocY", &trk_hole_err_trkLocY, &b_trk_hole_err_trkLocY);
   fChain->SetBranchAddress("trk_hole_cov_trkLocXY", &trk_hole_cov_trkLocXY, &b_trk_hole_cov_trkLocXY);
   fChain->SetBranchAddress("trk_hole_chi2", &trk_hole_chi2, &b_trk_hole_chi2);
   fChain->SetBranchAddress("trk_hole_ndof", &trk_hole_ndof, &b_trk_hole_ndof);
   
   
   fChain->SetBranchAddress("vx_n", &vx_n, &b_vx_n);
   fChain->SetBranchAddress("vx_x", &vx_x, &b_vx_x);
   fChain->SetBranchAddress("vx_y", &vx_y, &b_vx_y);
   fChain->SetBranchAddress("vx_z", &vx_z, &b_vx_z);
   fChain->SetBranchAddress("vx_chi2", &vx_chi2, &b_vx_chi2);
   fChain->SetBranchAddress("vx_ndof", &vx_ndof, &b_vx_ndof);
   fChain->SetBranchAddress("vx_err_x", &vx_err_x, &b_vx_err_x);
   fChain->SetBranchAddress("vx_err_y", &vx_err_y, &b_vx_err_y);
   fChain->SetBranchAddress("vx_err_z", &vx_err_z, &b_vx_err_z);
   fChain->SetBranchAddress("vx_cov_x", &vx_cov_x, &b_vx_cov_x);
   fChain->SetBranchAddress("vx_cov_y", &vx_cov_y, &b_vx_cov_y);
   fChain->SetBranchAddress("vx_cov_z", &vx_cov_z, &b_vx_cov_z);
   fChain->SetBranchAddress("vx_cov_xy", &vx_cov_xy, &b_vx_cov_xy);
   fChain->SetBranchAddress("vx_cov_xz", &vx_cov_xz, &b_vx_cov_xz);
   fChain->SetBranchAddress("vx_cov_yz", &vx_cov_yz, &b_vx_cov_yz);
   fChain->SetBranchAddress("vx_px", &vx_px, &b_vx_px);
   fChain->SetBranchAddress("vx_py", &vx_py, &b_vx_py);
   fChain->SetBranchAddress("vx_pz", &vx_pz, &b_vx_pz);
   fChain->SetBranchAddress("vx_E", &vx_E, &b_vx_E);
   fChain->SetBranchAddress("vx_m", &vx_m, &b_vx_m);
   fChain->SetBranchAddress("vx_nTracks", &vx_nTracks, &b_vx_nTracks);
   fChain->SetBranchAddress("vx_sumPt", &vx_sumPt, &b_vx_sumPt);
   fChain->SetBranchAddress("vx_type", &vx_type, &b_vx_type);
   fChain->SetBranchAddress("vx_trk_n", &vx_trk_n, &b_vx_trk_n);
   fChain->SetBranchAddress("vx_trk_chi2", &vx_trk_chi2, &b_vx_trk_chi2);
   fChain->SetBranchAddress("vx_trk_d0", &vx_trk_d0, &b_vx_trk_d0);
   fChain->SetBranchAddress("vx_trk_z0", &vx_trk_z0, &b_vx_trk_z0);
   fChain->SetBranchAddress("vx_trk_unbiased_d0", &vx_trk_unbiased_d0, &b_vx_trk_unbiased_d0);
   fChain->SetBranchAddress("vx_trk_unbiased_z0", &vx_trk_unbiased_z0, &b_vx_trk_unbiased_z0);
   fChain->SetBranchAddress("vx_trk_err_unbiased_d0", &vx_trk_err_unbiased_d0, &b_vx_trk_err_unbiased_d0);
   fChain->SetBranchAddress("vx_trk_err_unbiased_z0", &vx_trk_err_unbiased_z0, &b_vx_trk_err_unbiased_z0);
   fChain->SetBranchAddress("vx_trk_phi", &vx_trk_phi, &b_vx_trk_phi);
   fChain->SetBranchAddress("vx_trk_theta", &vx_trk_theta, &b_vx_trk_theta);
   fChain->SetBranchAddress("vx_trk_weight", &vx_trk_weight, &b_vx_trk_weight);
   fChain->SetBranchAddress("vx_trk_index", &vx_trk_index, &b_vx_trk_index);
   fChain->SetBranchAddress("vxPix_n", &vxPix_n, &b_vxPix_n);
   fChain->SetBranchAddress("vxPix_x", &vxPix_x, &b_vxPix_x);
   fChain->SetBranchAddress("vxPix_y", &vxPix_y, &b_vxPix_y);
   fChain->SetBranchAddress("vxPix_z", &vxPix_z, &b_vxPix_z);
   fChain->SetBranchAddress("vxPix_chi2", &vxPix_chi2, &b_vxPix_chi2);
   fChain->SetBranchAddress("vxPix_ndof", &vxPix_ndof, &b_vxPix_ndof);
   fChain->SetBranchAddress("vxPix_err_x", &vxPix_err_x, &b_vxPix_err_x);
   fChain->SetBranchAddress("vxPix_err_y", &vxPix_err_y, &b_vxPix_err_y);
   fChain->SetBranchAddress("vxPix_err_z", &vxPix_err_z, &b_vxPix_err_z);
   fChain->SetBranchAddress("vxPix_cov_x", &vxPix_cov_x, &b_vxPix_cov_x);
   fChain->SetBranchAddress("vxPix_cov_y", &vxPix_cov_y, &b_vxPix_cov_y);
   fChain->SetBranchAddress("vxPix_cov_z", &vxPix_cov_z, &b_vxPix_cov_z);
   fChain->SetBranchAddress("vxPix_cov_xy", &vxPix_cov_xy, &b_vxPix_cov_xy);
   fChain->SetBranchAddress("vxPix_cov_xz", &vxPix_cov_xz, &b_vxPix_cov_xz);
   fChain->SetBranchAddress("vxPix_cov_yz", &vxPix_cov_yz, &b_vxPix_cov_yz);
   fChain->SetBranchAddress("vxPix_px", &vxPix_px, &b_vxPix_px);
   fChain->SetBranchAddress("vxPix_py", &vxPix_py, &b_vxPix_py);
   fChain->SetBranchAddress("vxPix_pz", &vxPix_pz, &b_vxPix_pz);
   fChain->SetBranchAddress("vxPix_E", &vxPix_E, &b_vxPix_E);
   fChain->SetBranchAddress("vxPix_m", &vxPix_m, &b_vxPix_m);
   fChain->SetBranchAddress("vxPix_nTracks", &vxPix_nTracks, &b_vxPix_nTracks);
   fChain->SetBranchAddress("vxPix_sumPt", &vxPix_sumPt, &b_vxPix_sumPt);
   fChain->SetBranchAddress("vxPix_type", &vxPix_type, &b_vxPix_type);
   fChain->SetBranchAddress("vxPix_trkPix_n", &vxPix_trkPix_n, &b_vxPix_trkPix_n);
   fChain->SetBranchAddress("vxPix_trkPix_chi2", &vxPix_trkPix_chi2, &b_vxPix_trkPix_chi2);
   fChain->SetBranchAddress("vxPix_trkPix_d0", &vxPix_trkPix_d0, &b_vxPix_trkPix_d0);
   fChain->SetBranchAddress("vxPix_trkPix_z0", &vxPix_trkPix_z0, &b_vxPix_trkPix_z0);
   fChain->SetBranchAddress("vxPix_trkPix_unbiased_d0", &vxPix_trkPix_unbiased_d0, &b_vxPix_trkPix_unbiased_d0);
   fChain->SetBranchAddress("vxPix_trkPix_unbiased_z0", &vxPix_trkPix_unbiased_z0, &b_vxPix_trkPix_unbiased_z0);
   fChain->SetBranchAddress("vxPix_trkPix_err_unbiased_d0", &vxPix_trkPix_err_unbiased_d0, &b_vxPix_trkPix_err_unbiased_d0);
   fChain->SetBranchAddress("vxPix_trkPix_err_unbiased_z0", &vxPix_trkPix_err_unbiased_z0, &b_vxPix_trkPix_err_unbiased_z0);
   fChain->SetBranchAddress("vxPix_trkPix_phi", &vxPix_trkPix_phi, &b_vxPix_trkPix_phi);
   fChain->SetBranchAddress("vxPix_trkPix_theta", &vxPix_trkPix_theta, &b_vxPix_trkPix_theta);
   fChain->SetBranchAddress("vxPix_trkPix_weight", &vxPix_trkPix_weight, &b_vxPix_trkPix_weight);
   fChain->SetBranchAddress("vxSCT_n", &vxSCT_n, &b_vxSCT_n);
   fChain->SetBranchAddress("vxSCT_x", &vxSCT_x, &b_vxSCT_x);
   fChain->SetBranchAddress("vxSCT_y", &vxSCT_y, &b_vxSCT_y);
   fChain->SetBranchAddress("vxSCT_z", &vxSCT_z, &b_vxSCT_z);
   fChain->SetBranchAddress("vxSCT_chi2", &vxSCT_chi2, &b_vxSCT_chi2);
   fChain->SetBranchAddress("vxSCT_ndof", &vxSCT_ndof, &b_vxSCT_ndof);
   fChain->SetBranchAddress("vxSCT_err_x", &vxSCT_err_x, &b_vxSCT_err_x);
   fChain->SetBranchAddress("vxSCT_err_y", &vxSCT_err_y, &b_vxSCT_err_y);
   fChain->SetBranchAddress("vxSCT_err_z", &vxSCT_err_z, &b_vxSCT_err_z);
   fChain->SetBranchAddress("vxSCT_cov_x", &vxSCT_cov_x, &b_vxSCT_cov_x);
   fChain->SetBranchAddress("vxSCT_cov_y", &vxSCT_cov_y, &b_vxSCT_cov_y);
   fChain->SetBranchAddress("vxSCT_cov_z", &vxSCT_cov_z, &b_vxSCT_cov_z);
   fChain->SetBranchAddress("vxSCT_cov_xy", &vxSCT_cov_xy, &b_vxSCT_cov_xy);
   fChain->SetBranchAddress("vxSCT_cov_xz", &vxSCT_cov_xz, &b_vxSCT_cov_xz);
   fChain->SetBranchAddress("vxSCT_cov_yz", &vxSCT_cov_yz, &b_vxSCT_cov_yz);
   fChain->SetBranchAddress("vxSCT_px", &vxSCT_px, &b_vxSCT_px);
   fChain->SetBranchAddress("vxSCT_py", &vxSCT_py, &b_vxSCT_py);
   fChain->SetBranchAddress("vxSCT_pz", &vxSCT_pz, &b_vxSCT_pz);
   fChain->SetBranchAddress("vxSCT_E", &vxSCT_E, &b_vxSCT_E);
   fChain->SetBranchAddress("vxSCT_m", &vxSCT_m, &b_vxSCT_m);
   fChain->SetBranchAddress("vxSCT_nTracks", &vxSCT_nTracks, &b_vxSCT_nTracks);
   fChain->SetBranchAddress("vxSCT_sumPt", &vxSCT_sumPt, &b_vxSCT_sumPt);
   fChain->SetBranchAddress("vxSCT_type", &vxSCT_type, &b_vxSCT_type);
   fChain->SetBranchAddress("vxSCT_trkSCT_n", &vxSCT_trkSCT_n, &b_vxSCT_trkSCT_n);
   fChain->SetBranchAddress("vxSCT_trkSCT_chi2", &vxSCT_trkSCT_chi2, &b_vxSCT_trkSCT_chi2);
   fChain->SetBranchAddress("vxSCT_trkSCT_d0", &vxSCT_trkSCT_d0, &b_vxSCT_trkSCT_d0);
   fChain->SetBranchAddress("vxSCT_trkSCT_z0", &vxSCT_trkSCT_z0, &b_vxSCT_trkSCT_z0);
   fChain->SetBranchAddress("vxSCT_trkSCT_unbiased_d0", &vxSCT_trkSCT_unbiased_d0, &b_vxSCT_trkSCT_unbiased_d0);
   fChain->SetBranchAddress("vxSCT_trkSCT_unbiased_z0", &vxSCT_trkSCT_unbiased_z0, &b_vxSCT_trkSCT_unbiased_z0);
   fChain->SetBranchAddress("vxSCT_trkSCT_err_unbiased_d0", &vxSCT_trkSCT_err_unbiased_d0, &b_vxSCT_trkSCT_err_unbiased_d0);
   fChain->SetBranchAddress("vxSCT_trkSCT_err_unbiased_z0", &vxSCT_trkSCT_err_unbiased_z0, &b_vxSCT_trkSCT_err_unbiased_z0);
   fChain->SetBranchAddress("vxSCT_trkSCT_phi", &vxSCT_trkSCT_phi, &b_vxSCT_trkSCT_phi);
   fChain->SetBranchAddress("vxSCT_trkSCT_theta", &vxSCT_trkSCT_theta, &b_vxSCT_trkSCT_theta);
   fChain->SetBranchAddress("vxSCT_trkSCT_weight", &vxSCT_trkSCT_weight, &b_vxSCT_trkSCT_weight);
   fChain->SetBranchAddress("vxSi_n", &vxSi_n, &b_vxSi_n);
   fChain->SetBranchAddress("vxSi_x", &vxSi_x, &b_vxSi_x);
   fChain->SetBranchAddress("vxSi_y", &vxSi_y, &b_vxSi_y);
   fChain->SetBranchAddress("vxSi_z", &vxSi_z, &b_vxSi_z);
   fChain->SetBranchAddress("vxSi_chi2", &vxSi_chi2, &b_vxSi_chi2);
   fChain->SetBranchAddress("vxSi_ndof", &vxSi_ndof, &b_vxSi_ndof);
   fChain->SetBranchAddress("vxSi_err_x", &vxSi_err_x, &b_vxSi_err_x);
   fChain->SetBranchAddress("vxSi_err_y", &vxSi_err_y, &b_vxSi_err_y);
   fChain->SetBranchAddress("vxSi_err_z", &vxSi_err_z, &b_vxSi_err_z);
   fChain->SetBranchAddress("vxSi_cov_x", &vxSi_cov_x, &b_vxSi_cov_x);
   fChain->SetBranchAddress("vxSi_cov_y", &vxSi_cov_y, &b_vxSi_cov_y);
   fChain->SetBranchAddress("vxSi_cov_z", &vxSi_cov_z, &b_vxSi_cov_z);
   fChain->SetBranchAddress("vxSi_cov_xy", &vxSi_cov_xy, &b_vxSi_cov_xy);
   fChain->SetBranchAddress("vxSi_cov_xz", &vxSi_cov_xz, &b_vxSi_cov_xz);
   fChain->SetBranchAddress("vxSi_cov_yz", &vxSi_cov_yz, &b_vxSi_cov_yz);
   fChain->SetBranchAddress("vxSi_px", &vxSi_px, &b_vxSi_px);
   fChain->SetBranchAddress("vxSi_py", &vxSi_py, &b_vxSi_py);
   fChain->SetBranchAddress("vxSi_pz", &vxSi_pz, &b_vxSi_pz);
   fChain->SetBranchAddress("vxSi_E", &vxSi_E, &b_vxSi_E);
   fChain->SetBranchAddress("vxSi_m", &vxSi_m, &b_vxSi_m);
   fChain->SetBranchAddress("vxSi_nTracks", &vxSi_nTracks, &b_vxSi_nTracks);
   fChain->SetBranchAddress("vxSi_sumPt", &vxSi_sumPt, &b_vxSi_sumPt);
   fChain->SetBranchAddress("vxSi_type", &vxSi_type, &b_vxSi_type);
   fChain->SetBranchAddress("vxSi_trkResolvedTrack_n", &vxSi_trkResolvedTrack_n, &b_vxSi_trkResolvedTrack_n);
   fChain->SetBranchAddress("vxSi_trkResolvedTrack_chi2", &vxSi_trkResolvedTrack_chi2, &b_vxSi_trkResolvedTrack_chi2);
   fChain->SetBranchAddress("vxSi_trkResolvedTrack_d0", &vxSi_trkResolvedTrack_d0, &b_vxSi_trkResolvedTrack_d0);
   fChain->SetBranchAddress("vxSi_trkResolvedTrack_z0", &vxSi_trkResolvedTrack_z0, &b_vxSi_trkResolvedTrack_z0);
   fChain->SetBranchAddress("vxSi_trkResolvedTrack_unbiased_d0", &vxSi_trkResolvedTrack_unbiased_d0, &b_vxSi_trkResolvedTrack_unbiased_d0);
   fChain->SetBranchAddress("vxSi_trkResolvedTrack_unbiased_z0", &vxSi_trkResolvedTrack_unbiased_z0, &b_vxSi_trkResolvedTrack_unbiased_z0);
   fChain->SetBranchAddress("vxSi_trkResolvedTrack_err_unbiased_d0", &vxSi_trkResolvedTrack_err_unbiased_d0, &b_vxSi_trkResolvedTrack_err_unbiased_d0);
   fChain->SetBranchAddress("vxSi_trkResolvedTrack_err_unbiased_z0", &vxSi_trkResolvedTrack_err_unbiased_z0, &b_vxSi_trkResolvedTrack_err_unbiased_z0);
   fChain->SetBranchAddress("vxSi_trkResolvedTrack_phi", &vxSi_trkResolvedTrack_phi, &b_vxSi_trkResolvedTrack_phi);
   fChain->SetBranchAddress("vxSi_trkResolvedTrack_theta", &vxSi_trkResolvedTrack_theta, &b_vxSi_trkResolvedTrack_theta);
   fChain->SetBranchAddress("vxSi_trkResolvedTrack_weight", &vxSi_trkResolvedTrack_weight, &b_vxSi_trkResolvedTrack_weight);
   fChain->SetBranchAddress("vxFull_n", &vxFull_n, &b_vxFull_n);
   fChain->SetBranchAddress("vxFull_x", &vxFull_x, &b_vxFull_x);
   fChain->SetBranchAddress("vxFull_y", &vxFull_y, &b_vxFull_y);
   fChain->SetBranchAddress("vxFull_z", &vxFull_z, &b_vxFull_z);
   fChain->SetBranchAddress("vxFull_chi2", &vxFull_chi2, &b_vxFull_chi2);
   fChain->SetBranchAddress("vxFull_ndof", &vxFull_ndof, &b_vxFull_ndof);
   fChain->SetBranchAddress("vxFull_err_x", &vxFull_err_x, &b_vxFull_err_x);
   fChain->SetBranchAddress("vxFull_err_y", &vxFull_err_y, &b_vxFull_err_y);
   fChain->SetBranchAddress("vxFull_err_z", &vxFull_err_z, &b_vxFull_err_z);
   fChain->SetBranchAddress("vxFull_cov_x", &vxFull_cov_x, &b_vxFull_cov_x);
   fChain->SetBranchAddress("vxFull_cov_y", &vxFull_cov_y, &b_vxFull_cov_y);
   fChain->SetBranchAddress("vxFull_cov_z", &vxFull_cov_z, &b_vxFull_cov_z);
   fChain->SetBranchAddress("vxFull_cov_xy", &vxFull_cov_xy, &b_vxFull_cov_xy);
   fChain->SetBranchAddress("vxFull_cov_xz", &vxFull_cov_xz, &b_vxFull_cov_xz);
   fChain->SetBranchAddress("vxFull_cov_yz", &vxFull_cov_yz, &b_vxFull_cov_yz);
   fChain->SetBranchAddress("vxFull_px", &vxFull_px, &b_vxFull_px);
   fChain->SetBranchAddress("vxFull_py", &vxFull_py, &b_vxFull_py);
   fChain->SetBranchAddress("vxFull_pz", &vxFull_pz, &b_vxFull_pz);
   fChain->SetBranchAddress("vxFull_E", &vxFull_E, &b_vxFull_E);
   fChain->SetBranchAddress("vxFull_m", &vxFull_m, &b_vxFull_m);
   fChain->SetBranchAddress("vxFull_nTracks", &vxFull_nTracks, &b_vxFull_nTracks);
   fChain->SetBranchAddress("vxFull_sumPt", &vxFull_sumPt, &b_vxFull_sumPt);
   fChain->SetBranchAddress("vxFull_type", &vxFull_type, &b_vxFull_type);
   fChain->SetBranchAddress("vxFull_trk_n", &vxFull_trk_n, &b_vxFull_trk_n);
   fChain->SetBranchAddress("vxFull_trk_chi2", &vxFull_trk_chi2, &b_vxFull_trk_chi2);
   fChain->SetBranchAddress("vxFull_trk_d0", &vxFull_trk_d0, &b_vxFull_trk_d0);
   fChain->SetBranchAddress("vxFull_trk_z0", &vxFull_trk_z0, &b_vxFull_trk_z0);
   fChain->SetBranchAddress("vxFull_trk_unbiased_d0", &vxFull_trk_unbiased_d0, &b_vxFull_trk_unbiased_d0);
   fChain->SetBranchAddress("vxFull_trk_unbiased_z0", &vxFull_trk_unbiased_z0, &b_vxFull_trk_unbiased_z0);
   fChain->SetBranchAddress("vxFull_trk_err_unbiased_d0", &vxFull_trk_err_unbiased_d0, &b_vxFull_trk_err_unbiased_d0);
   fChain->SetBranchAddress("vxFull_trk_err_unbiased_z0", &vxFull_trk_err_unbiased_z0, &b_vxFull_trk_err_unbiased_z0);
   fChain->SetBranchAddress("vxFull_trk_phi", &vxFull_trk_phi, &b_vxFull_trk_phi);
   fChain->SetBranchAddress("vxFull_trk_theta", &vxFull_trk_theta, &b_vxFull_trk_theta);
   fChain->SetBranchAddress("vxFull_trk_weight", &vxFull_trk_weight, &b_vxFull_trk_weight);
   fChain->SetBranchAddress("vxFull_trk_index", &vxFull_trk_index, &b_vxFull_trk_index);
   fChain->SetBranchAddress("vxFast_n", &vxFast_n, &b_vxFast_n);
   fChain->SetBranchAddress("vxFast_x", &vxFast_x, &b_vxFast_x);
   fChain->SetBranchAddress("vxFast_y", &vxFast_y, &b_vxFast_y);
   fChain->SetBranchAddress("vxFast_z", &vxFast_z, &b_vxFast_z);
   fChain->SetBranchAddress("vxFast_chi2", &vxFast_chi2, &b_vxFast_chi2);
   fChain->SetBranchAddress("vxFast_ndof", &vxFast_ndof, &b_vxFast_ndof);
   fChain->SetBranchAddress("vxFast_err_x", &vxFast_err_x, &b_vxFast_err_x);
   fChain->SetBranchAddress("vxFast_err_y", &vxFast_err_y, &b_vxFast_err_y);
   fChain->SetBranchAddress("vxFast_err_z", &vxFast_err_z, &b_vxFast_err_z);
   fChain->SetBranchAddress("vxFast_cov_x", &vxFast_cov_x, &b_vxFast_cov_x);
   fChain->SetBranchAddress("vxFast_cov_y", &vxFast_cov_y, &b_vxFast_cov_y);
   fChain->SetBranchAddress("vxFast_cov_z", &vxFast_cov_z, &b_vxFast_cov_z);
   fChain->SetBranchAddress("vxFast_cov_xy", &vxFast_cov_xy, &b_vxFast_cov_xy);
   fChain->SetBranchAddress("vxFast_cov_xz", &vxFast_cov_xz, &b_vxFast_cov_xz);
   fChain->SetBranchAddress("vxFast_cov_yz", &vxFast_cov_yz, &b_vxFast_cov_yz);
   fChain->SetBranchAddress("vxFast_px", &vxFast_px, &b_vxFast_px);
   fChain->SetBranchAddress("vxFast_py", &vxFast_py, &b_vxFast_py);
   fChain->SetBranchAddress("vxFast_pz", &vxFast_pz, &b_vxFast_pz);
   fChain->SetBranchAddress("vxFast_E", &vxFast_E, &b_vxFast_E);
   fChain->SetBranchAddress("vxFast_m", &vxFast_m, &b_vxFast_m);
   fChain->SetBranchAddress("vxFast_nTracks", &vxFast_nTracks, &b_vxFast_nTracks);
   fChain->SetBranchAddress("vxFast_sumPt", &vxFast_sumPt, &b_vxFast_sumPt);
   fChain->SetBranchAddress("vxFast_type", &vxFast_type, &b_vxFast_type);
   fChain->SetBranchAddress("vxFast_trk_n", &vxFast_trk_n, &b_vxFast_trk_n);
   fChain->SetBranchAddress("vxFast_trk_chi2", &vxFast_trk_chi2, &b_vxFast_trk_chi2);
   fChain->SetBranchAddress("vxFast_trk_d0", &vxFast_trk_d0, &b_vxFast_trk_d0);
   fChain->SetBranchAddress("vxFast_trk_z0", &vxFast_trk_z0, &b_vxFast_trk_z0);
   fChain->SetBranchAddress("vxFast_trk_unbiased_d0", &vxFast_trk_unbiased_d0, &b_vxFast_trk_unbiased_d0);
   fChain->SetBranchAddress("vxFast_trk_unbiased_z0", &vxFast_trk_unbiased_z0, &b_vxFast_trk_unbiased_z0);
   fChain->SetBranchAddress("vxFast_trk_err_unbiased_d0", &vxFast_trk_err_unbiased_d0, &b_vxFast_trk_err_unbiased_d0);
   fChain->SetBranchAddress("vxFast_trk_err_unbiased_z0", &vxFast_trk_err_unbiased_z0, &b_vxFast_trk_err_unbiased_z0);
   fChain->SetBranchAddress("vxFast_trk_phi", &vxFast_trk_phi, &b_vxFast_trk_phi);
   fChain->SetBranchAddress("vxFast_trk_theta", &vxFast_trk_theta, &b_vxFast_trk_theta);
   fChain->SetBranchAddress("vxFast_trk_weight", &vxFast_trk_weight, &b_vxFast_trk_weight);
   fChain->SetBranchAddress("vxFast_trk_index", &vxFast_trk_index, &b_vxFast_trk_index);
   fChain->SetBranchAddress("vxFullPix_n", &vxFullPix_n, &b_vxFullPix_n);
   fChain->SetBranchAddress("vxFullPix_x", &vxFullPix_x, &b_vxFullPix_x);
   fChain->SetBranchAddress("vxFullPix_y", &vxFullPix_y, &b_vxFullPix_y);
   fChain->SetBranchAddress("vxFullPix_z", &vxFullPix_z, &b_vxFullPix_z);
   fChain->SetBranchAddress("vxFullPix_chi2", &vxFullPix_chi2, &b_vxFullPix_chi2);
   fChain->SetBranchAddress("vxFullPix_ndof", &vxFullPix_ndof, &b_vxFullPix_ndof);
   fChain->SetBranchAddress("vxFullPix_err_x", &vxFullPix_err_x, &b_vxFullPix_err_x);
   fChain->SetBranchAddress("vxFullPix_err_y", &vxFullPix_err_y, &b_vxFullPix_err_y);
   fChain->SetBranchAddress("vxFullPix_err_z", &vxFullPix_err_z, &b_vxFullPix_err_z);
   fChain->SetBranchAddress("vxFullPix_cov_x", &vxFullPix_cov_x, &b_vxFullPix_cov_x);
   fChain->SetBranchAddress("vxFullPix_cov_y", &vxFullPix_cov_y, &b_vxFullPix_cov_y);
   fChain->SetBranchAddress("vxFullPix_cov_z", &vxFullPix_cov_z, &b_vxFullPix_cov_z);
   fChain->SetBranchAddress("vxFullPix_cov_xy", &vxFullPix_cov_xy, &b_vxFullPix_cov_xy);
   fChain->SetBranchAddress("vxFullPix_cov_xz", &vxFullPix_cov_xz, &b_vxFullPix_cov_xz);
   fChain->SetBranchAddress("vxFullPix_cov_yz", &vxFullPix_cov_yz, &b_vxFullPix_cov_yz);
   fChain->SetBranchAddress("vxFullPix_px", &vxFullPix_px, &b_vxFullPix_px);
   fChain->SetBranchAddress("vxFullPix_py", &vxFullPix_py, &b_vxFullPix_py);
   fChain->SetBranchAddress("vxFullPix_pz", &vxFullPix_pz, &b_vxFullPix_pz);
   fChain->SetBranchAddress("vxFullPix_E", &vxFullPix_E, &b_vxFullPix_E);
   fChain->SetBranchAddress("vxFullPix_m", &vxFullPix_m, &b_vxFullPix_m);
   fChain->SetBranchAddress("vxFullPix_nTracks", &vxFullPix_nTracks, &b_vxFullPix_nTracks);
   fChain->SetBranchAddress("vxFullPix_sumPt", &vxFullPix_sumPt, &b_vxFullPix_sumPt);
   fChain->SetBranchAddress("vxFullPix_type", &vxFullPix_type, &b_vxFullPix_type);
   fChain->SetBranchAddress("vxFullPix_trkPix_n", &vxFullPix_trkPix_n, &b_vxFullPix_trkPix_n);
   fChain->SetBranchAddress("vxFullPix_trkPix_chi2", &vxFullPix_trkPix_chi2, &b_vxFullPix_trkPix_chi2);
   fChain->SetBranchAddress("vxFullPix_trkPix_d0", &vxFullPix_trkPix_d0, &b_vxFullPix_trkPix_d0);
   fChain->SetBranchAddress("vxFullPix_trkPix_z0", &vxFullPix_trkPix_z0, &b_vxFullPix_trkPix_z0);
   fChain->SetBranchAddress("vxFullPix_trkPix_unbiased_d0", &vxFullPix_trkPix_unbiased_d0, &b_vxFullPix_trkPix_unbiased_d0);
   fChain->SetBranchAddress("vxFullPix_trkPix_unbiased_z0", &vxFullPix_trkPix_unbiased_z0, &b_vxFullPix_trkPix_unbiased_z0);
   fChain->SetBranchAddress("vxFullPix_trkPix_err_unbiased_d0", &vxFullPix_trkPix_err_unbiased_d0, &b_vxFullPix_trkPix_err_unbiased_d0);
   fChain->SetBranchAddress("vxFullPix_trkPix_err_unbiased_z0", &vxFullPix_trkPix_err_unbiased_z0, &b_vxFullPix_trkPix_err_unbiased_z0);
   fChain->SetBranchAddress("vxFullPix_trkPix_phi", &vxFullPix_trkPix_phi, &b_vxFullPix_trkPix_phi);
   fChain->SetBranchAddress("vxFullPix_trkPix_theta", &vxFullPix_trkPix_theta, &b_vxFullPix_trkPix_theta);
   fChain->SetBranchAddress("vxFullPix_trkPix_weight", &vxFullPix_trkPix_weight, &b_vxFullPix_trkPix_weight);
   fChain->SetBranchAddress("vxFastPix_n", &vxFastPix_n, &b_vxFastPix_n);
   fChain->SetBranchAddress("vxFastPix_x", &vxFastPix_x, &b_vxFastPix_x);
   fChain->SetBranchAddress("vxFastPix_y", &vxFastPix_y, &b_vxFastPix_y);
   fChain->SetBranchAddress("vxFastPix_z", &vxFastPix_z, &b_vxFastPix_z);
   fChain->SetBranchAddress("vxFastPix_chi2", &vxFastPix_chi2, &b_vxFastPix_chi2);
   fChain->SetBranchAddress("vxFastPix_ndof", &vxFastPix_ndof, &b_vxFastPix_ndof);
   fChain->SetBranchAddress("vxFastPix_err_x", &vxFastPix_err_x, &b_vxFastPix_err_x);
   fChain->SetBranchAddress("vxFastPix_err_y", &vxFastPix_err_y, &b_vxFastPix_err_y);
   fChain->SetBranchAddress("vxFastPix_err_z", &vxFastPix_err_z, &b_vxFastPix_err_z);
   fChain->SetBranchAddress("vxFastPix_cov_x", &vxFastPix_cov_x, &b_vxFastPix_cov_x);
   fChain->SetBranchAddress("vxFastPix_cov_y", &vxFastPix_cov_y, &b_vxFastPix_cov_y);
   fChain->SetBranchAddress("vxFastPix_cov_z", &vxFastPix_cov_z, &b_vxFastPix_cov_z);
   fChain->SetBranchAddress("vxFastPix_cov_xy", &vxFastPix_cov_xy, &b_vxFastPix_cov_xy);
   fChain->SetBranchAddress("vxFastPix_cov_xz", &vxFastPix_cov_xz, &b_vxFastPix_cov_xz);
   fChain->SetBranchAddress("vxFastPix_cov_yz", &vxFastPix_cov_yz, &b_vxFastPix_cov_yz);
   fChain->SetBranchAddress("vxFastPix_px", &vxFastPix_px, &b_vxFastPix_px);
   fChain->SetBranchAddress("vxFastPix_py", &vxFastPix_py, &b_vxFastPix_py);
   fChain->SetBranchAddress("vxFastPix_pz", &vxFastPix_pz, &b_vxFastPix_pz);
   fChain->SetBranchAddress("vxFastPix_E", &vxFastPix_E, &b_vxFastPix_E);
   fChain->SetBranchAddress("vxFastPix_m", &vxFastPix_m, &b_vxFastPix_m);
   fChain->SetBranchAddress("vxFastPix_nTracks", &vxFastPix_nTracks, &b_vxFastPix_nTracks);
   fChain->SetBranchAddress("vxFastPix_sumPt", &vxFastPix_sumPt, &b_vxFastPix_sumPt);
   fChain->SetBranchAddress("vxFastPix_type", &vxFastPix_type, &b_vxFastPix_type);
   fChain->SetBranchAddress("vxFastPix_trkPix_n", &vxFastPix_trkPix_n, &b_vxFastPix_trkPix_n);
   fChain->SetBranchAddress("vxFastPix_trkPix_chi2", &vxFastPix_trkPix_chi2, &b_vxFastPix_trkPix_chi2);
   fChain->SetBranchAddress("vxFastPix_trkPix_d0", &vxFastPix_trkPix_d0, &b_vxFastPix_trkPix_d0);
   fChain->SetBranchAddress("vxFastPix_trkPix_z0", &vxFastPix_trkPix_z0, &b_vxFastPix_trkPix_z0);
   fChain->SetBranchAddress("vxFastPix_trkPix_unbiased_d0", &vxFastPix_trkPix_unbiased_d0, &b_vxFastPix_trkPix_unbiased_d0);
   fChain->SetBranchAddress("vxFastPix_trkPix_unbiased_z0", &vxFastPix_trkPix_unbiased_z0, &b_vxFastPix_trkPix_unbiased_z0);
   fChain->SetBranchAddress("vxFastPix_trkPix_err_unbiased_d0", &vxFastPix_trkPix_err_unbiased_d0, &b_vxFastPix_trkPix_err_unbiased_d0);
   fChain->SetBranchAddress("vxFastPix_trkPix_err_unbiased_z0", &vxFastPix_trkPix_err_unbiased_z0, &b_vxFastPix_trkPix_err_unbiased_z0);
   fChain->SetBranchAddress("vxFastPix_trkPix_phi", &vxFastPix_trkPix_phi, &b_vxFastPix_trkPix_phi);
   fChain->SetBranchAddress("vxFastPix_trkPix_theta", &vxFastPix_trkPix_theta, &b_vxFastPix_trkPix_theta);
   fChain->SetBranchAddress("vxFastPix_trkPix_weight", &vxFastPix_trkPix_weight, &b_vxFastPix_trkPix_weight);
   fChain->SetBranchAddress("vxFullSCT_n", &vxFullSCT_n, &b_vxFullSCT_n);
   fChain->SetBranchAddress("vxFullSCT_x", &vxFullSCT_x, &b_vxFullSCT_x);
   fChain->SetBranchAddress("vxFullSCT_y", &vxFullSCT_y, &b_vxFullSCT_y);
   fChain->SetBranchAddress("vxFullSCT_z", &vxFullSCT_z, &b_vxFullSCT_z);
   fChain->SetBranchAddress("vxFullSCT_chi2", &vxFullSCT_chi2, &b_vxFullSCT_chi2);
   fChain->SetBranchAddress("vxFullSCT_ndof", &vxFullSCT_ndof, &b_vxFullSCT_ndof);
   fChain->SetBranchAddress("vxFullSCT_err_x", &vxFullSCT_err_x, &b_vxFullSCT_err_x);
   fChain->SetBranchAddress("vxFullSCT_err_y", &vxFullSCT_err_y, &b_vxFullSCT_err_y);
   fChain->SetBranchAddress("vxFullSCT_err_z", &vxFullSCT_err_z, &b_vxFullSCT_err_z);
   fChain->SetBranchAddress("vxFullSCT_cov_x", &vxFullSCT_cov_x, &b_vxFullSCT_cov_x);
   fChain->SetBranchAddress("vxFullSCT_cov_y", &vxFullSCT_cov_y, &b_vxFullSCT_cov_y);
   fChain->SetBranchAddress("vxFullSCT_cov_z", &vxFullSCT_cov_z, &b_vxFullSCT_cov_z);
   fChain->SetBranchAddress("vxFullSCT_cov_xy", &vxFullSCT_cov_xy, &b_vxFullSCT_cov_xy);
   fChain->SetBranchAddress("vxFullSCT_cov_xz", &vxFullSCT_cov_xz, &b_vxFullSCT_cov_xz);
   fChain->SetBranchAddress("vxFullSCT_cov_yz", &vxFullSCT_cov_yz, &b_vxFullSCT_cov_yz);
   fChain->SetBranchAddress("vxFullSCT_px", &vxFullSCT_px, &b_vxFullSCT_px);
   fChain->SetBranchAddress("vxFullSCT_py", &vxFullSCT_py, &b_vxFullSCT_py);
   fChain->SetBranchAddress("vxFullSCT_pz", &vxFullSCT_pz, &b_vxFullSCT_pz);
   fChain->SetBranchAddress("vxFullSCT_E", &vxFullSCT_E, &b_vxFullSCT_E);
   fChain->SetBranchAddress("vxFullSCT_m", &vxFullSCT_m, &b_vxFullSCT_m);
   fChain->SetBranchAddress("vxFullSCT_nTracks", &vxFullSCT_nTracks, &b_vxFullSCT_nTracks);
   fChain->SetBranchAddress("vxFullSCT_sumPt", &vxFullSCT_sumPt, &b_vxFullSCT_sumPt);
   fChain->SetBranchAddress("vxFullSCT_type", &vxFullSCT_type, &b_vxFullSCT_type);
   fChain->SetBranchAddress("vxFullSCT_trkSCT_n", &vxFullSCT_trkSCT_n, &b_vxFullSCT_trkSCT_n);
   fChain->SetBranchAddress("vxFullSCT_trkSCT_chi2", &vxFullSCT_trkSCT_chi2, &b_vxFullSCT_trkSCT_chi2);
   fChain->SetBranchAddress("vxFullSCT_trkSCT_d0", &vxFullSCT_trkSCT_d0, &b_vxFullSCT_trkSCT_d0);
   fChain->SetBranchAddress("vxFullSCT_trkSCT_z0", &vxFullSCT_trkSCT_z0, &b_vxFullSCT_trkSCT_z0);
   fChain->SetBranchAddress("vxFullSCT_trkSCT_unbiased_d0", &vxFullSCT_trkSCT_unbiased_d0, &b_vxFullSCT_trkSCT_unbiased_d0);
   fChain->SetBranchAddress("vxFullSCT_trkSCT_unbiased_z0", &vxFullSCT_trkSCT_unbiased_z0, &b_vxFullSCT_trkSCT_unbiased_z0);
   fChain->SetBranchAddress("vxFullSCT_trkSCT_err_unbiased_d0", &vxFullSCT_trkSCT_err_unbiased_d0, &b_vxFullSCT_trkSCT_err_unbiased_d0);
   fChain->SetBranchAddress("vxFullSCT_trkSCT_err_unbiased_z0", &vxFullSCT_trkSCT_err_unbiased_z0, &b_vxFullSCT_trkSCT_err_unbiased_z0);
   fChain->SetBranchAddress("vxFullSCT_trkSCT_phi", &vxFullSCT_trkSCT_phi, &b_vxFullSCT_trkSCT_phi);
   fChain->SetBranchAddress("vxFullSCT_trkSCT_theta", &vxFullSCT_trkSCT_theta, &b_vxFullSCT_trkSCT_theta);
   fChain->SetBranchAddress("vxFullSCT_trkSCT_weight", &vxFullSCT_trkSCT_weight, &b_vxFullSCT_trkSCT_weight);
   fChain->SetBranchAddress("vxFastSCT_n", &vxFastSCT_n, &b_vxFastSCT_n);
   fChain->SetBranchAddress("vxFastSCT_x", &vxFastSCT_x, &b_vxFastSCT_x);
   fChain->SetBranchAddress("vxFastSCT_y", &vxFastSCT_y, &b_vxFastSCT_y);
   fChain->SetBranchAddress("vxFastSCT_z", &vxFastSCT_z, &b_vxFastSCT_z);
   fChain->SetBranchAddress("vxFastSCT_chi2", &vxFastSCT_chi2, &b_vxFastSCT_chi2);
   fChain->SetBranchAddress("vxFastSCT_ndof", &vxFastSCT_ndof, &b_vxFastSCT_ndof);
   fChain->SetBranchAddress("vxFastSCT_err_x", &vxFastSCT_err_x, &b_vxFastSCT_err_x);
   fChain->SetBranchAddress("vxFastSCT_err_y", &vxFastSCT_err_y, &b_vxFastSCT_err_y);
   fChain->SetBranchAddress("vxFastSCT_err_z", &vxFastSCT_err_z, &b_vxFastSCT_err_z);
   fChain->SetBranchAddress("vxFastSCT_cov_x", &vxFastSCT_cov_x, &b_vxFastSCT_cov_x);
   fChain->SetBranchAddress("vxFastSCT_cov_y", &vxFastSCT_cov_y, &b_vxFastSCT_cov_y);
   fChain->SetBranchAddress("vxFastSCT_cov_z", &vxFastSCT_cov_z, &b_vxFastSCT_cov_z);
   fChain->SetBranchAddress("vxFastSCT_cov_xy", &vxFastSCT_cov_xy, &b_vxFastSCT_cov_xy);
   fChain->SetBranchAddress("vxFastSCT_cov_xz", &vxFastSCT_cov_xz, &b_vxFastSCT_cov_xz);
   fChain->SetBranchAddress("vxFastSCT_cov_yz", &vxFastSCT_cov_yz, &b_vxFastSCT_cov_yz);
   fChain->SetBranchAddress("vxFastSCT_px", &vxFastSCT_px, &b_vxFastSCT_px);
   fChain->SetBranchAddress("vxFastSCT_py", &vxFastSCT_py, &b_vxFastSCT_py);
   fChain->SetBranchAddress("vxFastSCT_pz", &vxFastSCT_pz, &b_vxFastSCT_pz);
   fChain->SetBranchAddress("vxFastSCT_E", &vxFastSCT_E, &b_vxFastSCT_E);
   fChain->SetBranchAddress("vxFastSCT_m", &vxFastSCT_m, &b_vxFastSCT_m);
   fChain->SetBranchAddress("vxFastSCT_nTracks", &vxFastSCT_nTracks, &b_vxFastSCT_nTracks);
   fChain->SetBranchAddress("vxFastSCT_sumPt", &vxFastSCT_sumPt, &b_vxFastSCT_sumPt);
   fChain->SetBranchAddress("vxFastSCT_type", &vxFastSCT_type, &b_vxFastSCT_type);
   fChain->SetBranchAddress("vxFastSCT_trkSCT_n", &vxFastSCT_trkSCT_n, &b_vxFastSCT_trkSCT_n);
   fChain->SetBranchAddress("vxFastSCT_trkSCT_chi2", &vxFastSCT_trkSCT_chi2, &b_vxFastSCT_trkSCT_chi2);
   fChain->SetBranchAddress("vxFastSCT_trkSCT_d0", &vxFastSCT_trkSCT_d0, &b_vxFastSCT_trkSCT_d0);
   fChain->SetBranchAddress("vxFastSCT_trkSCT_z0", &vxFastSCT_trkSCT_z0, &b_vxFastSCT_trkSCT_z0);
   fChain->SetBranchAddress("vxFastSCT_trkSCT_unbiased_d0", &vxFastSCT_trkSCT_unbiased_d0, &b_vxFastSCT_trkSCT_unbiased_d0);
   fChain->SetBranchAddress("vxFastSCT_trkSCT_unbiased_z0", &vxFastSCT_trkSCT_unbiased_z0, &b_vxFastSCT_trkSCT_unbiased_z0);
   fChain->SetBranchAddress("vxFastSCT_trkSCT_err_unbiased_d0", &vxFastSCT_trkSCT_err_unbiased_d0, &b_vxFastSCT_trkSCT_err_unbiased_d0);
   fChain->SetBranchAddress("vxFastSCT_trkSCT_err_unbiased_z0", &vxFastSCT_trkSCT_err_unbiased_z0, &b_vxFastSCT_trkSCT_err_unbiased_z0);
   fChain->SetBranchAddress("vxFastSCT_trkSCT_phi", &vxFastSCT_trkSCT_phi, &b_vxFastSCT_trkSCT_phi);
   fChain->SetBranchAddress("vxFastSCT_trkSCT_theta", &vxFastSCT_trkSCT_theta, &b_vxFastSCT_trkSCT_theta);
   fChain->SetBranchAddress("vxFastSCT_trkSCT_weight", &vxFastSCT_trkSCT_weight, &b_vxFastSCT_trkSCT_weight);
   fChain->SetBranchAddress("vxFullSi_n", &vxFullSi_n, &b_vxFullSi_n);
   fChain->SetBranchAddress("vxFullSi_x", &vxFullSi_x, &b_vxFullSi_x);
   fChain->SetBranchAddress("vxFullSi_y", &vxFullSi_y, &b_vxFullSi_y);
   fChain->SetBranchAddress("vxFullSi_z", &vxFullSi_z, &b_vxFullSi_z);
   fChain->SetBranchAddress("vxFullSi_chi2", &vxFullSi_chi2, &b_vxFullSi_chi2);
   fChain->SetBranchAddress("vxFullSi_ndof", &vxFullSi_ndof, &b_vxFullSi_ndof);
   fChain->SetBranchAddress("vxFullSi_err_x", &vxFullSi_err_x, &b_vxFullSi_err_x);
   fChain->SetBranchAddress("vxFullSi_err_y", &vxFullSi_err_y, &b_vxFullSi_err_y);
   fChain->SetBranchAddress("vxFullSi_err_z", &vxFullSi_err_z, &b_vxFullSi_err_z);
   fChain->SetBranchAddress("vxFullSi_cov_x", &vxFullSi_cov_x, &b_vxFullSi_cov_x);
   fChain->SetBranchAddress("vxFullSi_cov_y", &vxFullSi_cov_y, &b_vxFullSi_cov_y);
   fChain->SetBranchAddress("vxFullSi_cov_z", &vxFullSi_cov_z, &b_vxFullSi_cov_z);
   fChain->SetBranchAddress("vxFullSi_cov_xy", &vxFullSi_cov_xy, &b_vxFullSi_cov_xy);
   fChain->SetBranchAddress("vxFullSi_cov_xz", &vxFullSi_cov_xz, &b_vxFullSi_cov_xz);
   fChain->SetBranchAddress("vxFullSi_cov_yz", &vxFullSi_cov_yz, &b_vxFullSi_cov_yz);
   fChain->SetBranchAddress("vxFullSi_px", &vxFullSi_px, &b_vxFullSi_px);
   fChain->SetBranchAddress("vxFullSi_py", &vxFullSi_py, &b_vxFullSi_py);
   fChain->SetBranchAddress("vxFullSi_pz", &vxFullSi_pz, &b_vxFullSi_pz);
   fChain->SetBranchAddress("vxFullSi_E", &vxFullSi_E, &b_vxFullSi_E);
   fChain->SetBranchAddress("vxFullSi_m", &vxFullSi_m, &b_vxFullSi_m);
   fChain->SetBranchAddress("vxFullSi_nTracks", &vxFullSi_nTracks, &b_vxFullSi_nTracks);
   fChain->SetBranchAddress("vxFullSi_sumPt", &vxFullSi_sumPt, &b_vxFullSi_sumPt);
   fChain->SetBranchAddress("vxFullSi_type", &vxFullSi_type, &b_vxFullSi_type);
   fChain->SetBranchAddress("vxFullSi_trkResolvedTrack_n", &vxFullSi_trkResolvedTrack_n, &b_vxFullSi_trkResolvedTrack_n);
   fChain->SetBranchAddress("vxFullSi_trkResolvedTrack_chi2", &vxFullSi_trkResolvedTrack_chi2, &b_vxFullSi_trkResolvedTrack_chi2);
   fChain->SetBranchAddress("vxFullSi_trkResolvedTrack_d0", &vxFullSi_trkResolvedTrack_d0, &b_vxFullSi_trkResolvedTrack_d0);
   fChain->SetBranchAddress("vxFullSi_trkResolvedTrack_z0", &vxFullSi_trkResolvedTrack_z0, &b_vxFullSi_trkResolvedTrack_z0);
   fChain->SetBranchAddress("vxFullSi_trkResolvedTrack_unbiased_d0", &vxFullSi_trkResolvedTrack_unbiased_d0, &b_vxFullSi_trkResolvedTrack_unbiased_d0);
   fChain->SetBranchAddress("vxFullSi_trkResolvedTrack_unbiased_z0", &vxFullSi_trkResolvedTrack_unbiased_z0, &b_vxFullSi_trkResolvedTrack_unbiased_z0);
   fChain->SetBranchAddress("vxFullSi_trkResolvedTrack_err_unbiased_d0", &vxFullSi_trkResolvedTrack_err_unbiased_d0, &b_vxFullSi_trkResolvedTrack_err_unbiased_d0);
   fChain->SetBranchAddress("vxFullSi_trkResolvedTrack_err_unbiased_z0", &vxFullSi_trkResolvedTrack_err_unbiased_z0, &b_vxFullSi_trkResolvedTrack_err_unbiased_z0);
   fChain->SetBranchAddress("vxFullSi_trkResolvedTrack_phi", &vxFullSi_trkResolvedTrack_phi, &b_vxFullSi_trkResolvedTrack_phi);
   fChain->SetBranchAddress("vxFullSi_trkResolvedTrack_theta", &vxFullSi_trkResolvedTrack_theta, &b_vxFullSi_trkResolvedTrack_theta);
   fChain->SetBranchAddress("vxFullSi_trkResolvedTrack_weight", &vxFullSi_trkResolvedTrack_weight, &b_vxFullSi_trkResolvedTrack_weight);
   fChain->SetBranchAddress("vxFastSi_n", &vxFastSi_n, &b_vxFastSi_n);
   fChain->SetBranchAddress("vxFastSi_x", &vxFastSi_x, &b_vxFastSi_x);
   fChain->SetBranchAddress("vxFastSi_y", &vxFastSi_y, &b_vxFastSi_y);
   fChain->SetBranchAddress("vxFastSi_z", &vxFastSi_z, &b_vxFastSi_z);
   fChain->SetBranchAddress("vxFastSi_chi2", &vxFastSi_chi2, &b_vxFastSi_chi2);
   fChain->SetBranchAddress("vxFastSi_ndof", &vxFastSi_ndof, &b_vxFastSi_ndof);
   fChain->SetBranchAddress("vxFastSi_err_x", &vxFastSi_err_x, &b_vxFastSi_err_x);
   fChain->SetBranchAddress("vxFastSi_err_y", &vxFastSi_err_y, &b_vxFastSi_err_y);
   fChain->SetBranchAddress("vxFastSi_err_z", &vxFastSi_err_z, &b_vxFastSi_err_z);
   fChain->SetBranchAddress("vxFastSi_cov_x", &vxFastSi_cov_x, &b_vxFastSi_cov_x);
   fChain->SetBranchAddress("vxFastSi_cov_y", &vxFastSi_cov_y, &b_vxFastSi_cov_y);
   fChain->SetBranchAddress("vxFastSi_cov_z", &vxFastSi_cov_z, &b_vxFastSi_cov_z);
   fChain->SetBranchAddress("vxFastSi_cov_xy", &vxFastSi_cov_xy, &b_vxFastSi_cov_xy);
   fChain->SetBranchAddress("vxFastSi_cov_xz", &vxFastSi_cov_xz, &b_vxFastSi_cov_xz);
   fChain->SetBranchAddress("vxFastSi_cov_yz", &vxFastSi_cov_yz, &b_vxFastSi_cov_yz);
   fChain->SetBranchAddress("vxFastSi_px", &vxFastSi_px, &b_vxFastSi_px);
   fChain->SetBranchAddress("vxFastSi_py", &vxFastSi_py, &b_vxFastSi_py);
   fChain->SetBranchAddress("vxFastSi_pz", &vxFastSi_pz, &b_vxFastSi_pz);
   fChain->SetBranchAddress("vxFastSi_E", &vxFastSi_E, &b_vxFastSi_E);
   fChain->SetBranchAddress("vxFastSi_m", &vxFastSi_m, &b_vxFastSi_m);
   fChain->SetBranchAddress("vxFastSi_nTracks", &vxFastSi_nTracks, &b_vxFastSi_nTracks);
   fChain->SetBranchAddress("vxFastSi_sumPt", &vxFastSi_sumPt, &b_vxFastSi_sumPt);
   fChain->SetBranchAddress("vxFastSi_type", &vxFastSi_type, &b_vxFastSi_type);
   fChain->SetBranchAddress("vxFastSi_trkResolvedTrack_n", &vxFastSi_trkResolvedTrack_n, &b_vxFastSi_trkResolvedTrack_n);
   fChain->SetBranchAddress("vxFastSi_trkResolvedTrack_chi2", &vxFastSi_trkResolvedTrack_chi2, &b_vxFastSi_trkResolvedTrack_chi2);
   fChain->SetBranchAddress("vxFastSi_trkResolvedTrack_d0", &vxFastSi_trkResolvedTrack_d0, &b_vxFastSi_trkResolvedTrack_d0);
   fChain->SetBranchAddress("vxFastSi_trkResolvedTrack_z0", &vxFastSi_trkResolvedTrack_z0, &b_vxFastSi_trkResolvedTrack_z0);
   fChain->SetBranchAddress("vxFastSi_trkResolvedTrack_unbiased_d0", &vxFastSi_trkResolvedTrack_unbiased_d0, &b_vxFastSi_trkResolvedTrack_unbiased_d0);
   fChain->SetBranchAddress("vxFastSi_trkResolvedTrack_unbiased_z0", &vxFastSi_trkResolvedTrack_unbiased_z0, &b_vxFastSi_trkResolvedTrack_unbiased_z0);
   fChain->SetBranchAddress("vxFastSi_trkResolvedTrack_err_unbiased_d0", &vxFastSi_trkResolvedTrack_err_unbiased_d0, &b_vxFastSi_trkResolvedTrack_err_unbiased_d0);
   fChain->SetBranchAddress("vxFastSi_trkResolvedTrack_err_unbiased_z0", &vxFastSi_trkResolvedTrack_err_unbiased_z0, &b_vxFastSi_trkResolvedTrack_err_unbiased_z0);
   fChain->SetBranchAddress("vxFastSi_trkResolvedTrack_phi", &vxFastSi_trkResolvedTrack_phi, &b_vxFastSi_trkResolvedTrack_phi);
   fChain->SetBranchAddress("vxFastSi_trkResolvedTrack_theta", &vxFastSi_trkResolvedTrack_theta, &b_vxFastSi_trkResolvedTrack_theta);
   fChain->SetBranchAddress("vxFastSi_trkResolvedTrack_weight", &vxFastSi_trkResolvedTrack_weight, &b_vxFastSi_trkResolvedTrack_weight);
   Notify();
}

Bool_t VtxNtuple::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void VtxNtuple::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t VtxNtuple::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef VtxNtuple_cxx
