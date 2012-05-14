/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Apr 29 10:24:17 2011 by ROOT version 5.26/00
// from TChain susy/
//////////////////////////////////////////////////////////

#ifndef Reco_dpd2_h
#define Reco_dpd2_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include<iostream>
#include<vector>
#include "SUSYTools/egammaPIDdefs.h"
#include "egammaAnalysisUtils/checkOQ.h"
//#include "EnergyRescaler.h"
//#include "egammaSFclass.h"
#include "SUSYTools/SUSYObjDef.h"
//#include "StacoCBScaleEffFactors.h"
#include "SUSYTools/APWeightEntry.h"  //General weight representation (mandatory)
#include "SUSYTools/APReweightND.h"   //3D Reweight class (only for ND parametrization of the weights)
#include "SUSYTools/APEvtWeight.h"   //Event Weight calculation class (if single event weights need to be known, e.g. for propagation or usage outside a simple counting/summing)
#include "THnSparse.h"
#include "SUSYTools/FakeMetEstimator.h" 


using namespace std;
//using namespace SUSY;

bool isBadJet(int criteria, double emf,	double hecf, double larq, double hecq, double time, double sumpttrk, double eta, double pt, double fmax,double negE);

class Reco_dpd2 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          RunNumber;
   UInt_t          EventNumber;
   UInt_t          lbn;
   UInt_t          bcid;
   Int_t           el_n;
   vector<float>   *el_E;
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
   vector<unsigned int> *el_OQ;
   vector<int>     *el_medium;
   vector<float>   *el_weta2;
   vector<float>   *el_Etcone20;
   vector<float>   *el_expectHitInBLayer;
   vector<float>   *el_reta;
   vector<float>   *el_etas2;
   vector<float>   *el_cl_E;
   vector<float>   *el_cl_pt;
   vector<float>   *el_cl_eta;
   vector<float>   *el_cl_phi;
   vector<float>   *el_trackphi;
   vector<float>   *el_tracketa;
   vector<int>     *el_nPixHits;
   vector<int>     *el_nSCTHits;
   vector<float>   *el_ptcone20;
   Bool_t          EF_e10_medium;
   Bool_t          EF_e20_medium;
   Bool_t          EF_mu10;
   Bool_t          EF_mu13;
   Bool_t          EF_mu18;
   Bool_t          EF_mu20;
   Bool_t          L1_EM10;
   Bool_t          L1_J10;
   Bool_t          L1_J15;
   Bool_t          L1_J30;
   Bool_t          L1_J75;
   Bool_t          L1_MU0;
   Bool_t          L1_MU10;
   Bool_t          L1_MU6;
   Bool_t          L2_e10_medium;
   Bool_t          L2_mu10;
   Int_t           mu_staco_n;
   Int_t				 larError;
   vector<float>   *mu_staco_E;
   vector<float>   *mu_staco_pt;
   vector<float>   *mu_staco_m;
   vector<float>   *mu_staco_eta;
   vector<float>   *mu_staco_phi;
   vector<float>   *mu_staco_px;
   vector<float>   *mu_staco_py;
   vector<float>   *mu_staco_pz;
   vector<float>   *mu_staco_charge;
   vector<float>   *mu_staco_matchchi2;
   vector<float>   *mu_staco_etcone20;
   vector<float>   *mu_staco_ptcone20;
   vector<int>     *mu_staco_bestMatch;
   vector<int>     *mu_staco_isCombinedMuon;
   vector<int>     *mu_staco_isLowPtReconstructedMuon;
   vector<int>     *mu_staco_tight;
   vector<int>     *mu_staco_loose;
   vector<float>   *mu_staco_d0_exPV;
   vector<float>   *mu_staco_z0_exPV;
   vector<float>   *mu_staco_qoverp_exPV;
   vector<float>   *mu_staco_cov_qoverp_exPV;
   vector<float>   *mu_staco_id_theta_exPV;
   vector<float>   *mu_staco_id_qoverp_exPV;
   vector<float>   *mu_staco_me_theta_exPV;
   vector<float>   *mu_staco_me_qoverp_exPV;
   vector<float>   *mu_staco_id_theta;
   vector<int>     *mu_staco_nBLHits;
   vector<int>     *mu_staco_nPixHits;
   vector<int>     *mu_staco_nPixHoles;
   vector<int>     *mu_staco_nSCTHits;
   vector<int>     *mu_staco_nTRTHits;
   vector<int>     *mu_staco_nSCTHoles;
   vector<int>     *mu_staco_nTRTOutliers;
   vector<int>     *mu_staco_nPixelDeadSensors;
   vector<int>     *mu_staco_nSCTDeadSensors;
   vector<int>     *mu_staco_expectBLayerHit;
   Float_t         MET_RefFinal_etx;
   Float_t         MET_RefFinal_ety;
   Float_t         MET_RefFinal_phi;
   Float_t         MET_RefFinal_et;
   Float_t         MET_RefEle_etx;
   Float_t         MET_RefEle_ety;
   Float_t         MET_RefJet_etx;
   Float_t         MET_RefJet_ety;
   Float_t         MET_SoftJets_etx;
   Float_t         MET_SoftJets_ety;
   Float_t         MET_RefMuon_etx;
   Float_t         MET_RefMuon_ety;
   Float_t         MET_RefMuon_Staco_etx;
   Float_t         MET_RefMuon_Staco_ety;
   Float_t         MET_RefMuon_Muid_etx;
   Float_t         MET_RefMuon_Muid_ety;
   Float_t         MET_RefTau_etx;
   Float_t         MET_RefTau_ety;
   Float_t         MET_CellOut_etx;
   Float_t         MET_CellOut_ety;
   Float_t         MET_Track_etx;
   Float_t         MET_Track_ety;
   Float_t         MET_RefFinal_em_etx;
   Float_t         MET_RefFinal_em_ety;
   Float_t         MET_RefEle_em_etx;
   Float_t         MET_RefEle_em_ety;
   Float_t         MET_RefJet_em_etx;
   Float_t         MET_RefJet_em_ety;
   Float_t         MET_SoftJets_em_etx;
   Float_t         MET_SoftJets_em_ety;
   Float_t         MET_RefMuon_em_etx;
   Float_t         MET_RefMuon_em_ety;
   Float_t         MET_RefGamma_em_etx;
   Float_t         MET_RefGamma_em_ety;
   Float_t         MET_RefTau_em_etx;
   Float_t         MET_RefTau_em_ety;
   Float_t         MET_Muon_Isol_Staco_etx;
   Float_t         MET_Muon_Isol_Staco_ety;
   Float_t         MET_Muon_NonIsol_Staco_etx;
   Float_t         MET_Muon_NonIsol_Staco_ety;
   Float_t         MET_Muon_Total_Staco_etx;
   Float_t         MET_Muon_Total_Staco_ety;
   Float_t         MET_Muon_Isol_Muid_etx;
   Float_t         MET_Muon_Isol_Muid_ety;
   Float_t         MET_Muon_NonIsol_Muid_etx;
   Float_t         MET_Muon_NonIsol_Muid_ety;
   Float_t         MET_Muon_Total_Muid_etx;
   Float_t         MET_Muon_Total_Muid_ety;
   Float_t         MET_MuonBoy_etx;
   Float_t         MET_MuonBoy_ety;
   Float_t         MET_RefMuon_Track_etx;
   Float_t         MET_RefMuon_Track_ety;
   Float_t         MET_RefMuon_Track_Staco_etx;
   Float_t         MET_RefMuon_Track_Staco_ety;
   Float_t         MET_RefMuon_Track_Muid_etx;
   Float_t         MET_RefMuon_Track_Muid_ety;
   Float_t         MET_Final_etx;
   Float_t         MET_Final_ety;
   Float_t         MET_TopoObj_etx;
   Float_t         MET_TopoObj_ety;
   Float_t         MET_LocHadTopo_etx;
   Float_t         MET_LocHadTopo_ety;
   Float_t         MET_Simplified20_RefGamma_etx;
   Float_t         MET_Simplified20_RefGamma_ety;
   Float_t         MET_Simplified20withTightPhotons_RefGamma_etx;
   Float_t         MET_Simplified20withTightPhotons_RefGamma_ety;
   Float_t         MET_Simplified20_RefFinal_etx;
   Float_t         MET_Simplified20_RefFinal_ety;
   Float_t         MET_Simplified20_RefFinal_etx_CentralReg;
   Float_t	   MET_Simplified20_RefFinal_ety_CentralReg;
   Float_t	   MET_Simplified20_RefFinal_etx_EndcapRegion;
   Float_t	   MET_Simplified20_RefFinal_ety_EndcapRegion;
   Float_t	   MET_Simplified20_RefFinal_etx_ForwardReg;
   Float_t	   MET_Simplified20_RefFinal_ety_ForwardReg;
   Float_t         MET_Simplified20withTightPhotons_RefFinal_etx;
   Float_t         MET_Simplified20withTightPhotons_RefFinal_ety;
   Float_t         MET_Simplified20_CellOut_etx;
   Float_t         MET_Simplified20_CellOut_ety;
   Float_t         MET_Simplified20withTightPhotons_CellOut_etx;
   Float_t         MET_Simplified20withTightPhotons_CellOut_ety;
   Float_t         MET_Simplified20_RefJet_etx;
   Float_t         MET_Simplified20_RefJet_ety;
   Float_t	   MET_Simplified20_RefJet_etx_CentralReg;
   Float_t	   MET_Simplified20_RefJet_ety_CentralReg;
   Float_t	   MET_Simplified20_RefJet_etx_EndcapRegion;
   Float_t	   MET_Simplified20_RefJet_ety_EndcapRegion;
   Float_t	   MET_Simplified20_RefJet_etx_ForwardReg;
   Float_t	   MET_Simplified20_RefJet_ety_ForwardReg;
   Float_t         MET_Simplified20withTightPhotons_RefJet_etx;
   Float_t         MET_Simplified20withTightPhotons_RefJet_ety;
   Float_t         MET_Simplified20_RefEle_etx;
   Float_t         MET_Simplified20_RefEle_ety;
   Float_t         MET_Simplified20withTightPhotons_RefEle_etx;
   Float_t         MET_Simplified20withTightPhotons_RefEle_ety;
   Float_t         MET_Simplified20_Muon_Total_Staco_etx;
   Float_t         MET_Simplified20_Muon_Total_Staco_ety;
   Float_t         MET_Simplified20withTightPhotons_Muon_Total_Staco_etx;
   Float_t         MET_Simplified20withTightPhotons_Muon_Total_Staco_ety;
   Float_t         MET_Simplified20_Muon_Isol_Staco_etx;
   Float_t         MET_Simplified20_Muon_Isol_Staco_ety;
   Float_t         MET_Simplified20withTightPhotons_Muon_Isol_Staco_etx;
   Float_t         MET_Simplified20withTightPhotons_Muon_Isol_Staco_ety;
   Float_t         MET_Simplified20_Muon_NonIsol_Staco_etx;
   Float_t         MET_Simplified20_Muon_NonIsol_Staco_ety;
   Float_t         MET_Simplified20withTightPhotons_Muon_NonIsol_Staco_etx;
   Float_t         MET_Simplified20withTightPhotons_Muon_NonIsol_Staco_ety;
   Float_t         MET_Simplified20_Muon_Isol_Muid_etx;
   Float_t         MET_Simplified20_Muon_Isol_Muid_ety;
   Float_t         MET_Simplified20withTightPhotons_Muon_Isol_Muid_etx;
   Float_t         MET_Simplified20withTightPhotons_Muon_Isol_Muid_ety;
   Float_t         MET_Simplified20_Muon_NonIsol_Muid_etx;
   Float_t         MET_Simplified20_Muon_NonIsol_Muid_ety;
   Float_t         MET_Simplified20withTightPhotons_Muon_NonIsol_Muid_etx;
   Float_t         MET_Simplified20withTightPhotons_Muon_NonIsol_Muid_ety;
   Float_t         MET_Simplified20_Muon_Total_Muid_etx;
   Float_t         MET_Simplified20_Muon_Total_Muid_ety;
   Float_t         MET_Simplified20withTightPhotons_Muon_Total_Muid_etx;
   Float_t         MET_Simplified20withTightPhotons_Muon_Total_Muid_ety;
   Float_t         MET_Simplified20_Systematics_RefJet_smearedJER_etx;
   Float_t         MET_Simplified20_Systematics_RefJet_smearedJER_ety;
   Float_t         MET_Simplified20withTightPhotons_Systematics_RefJet_smearedJER_etx;
   Float_t         MET_Simplified20withTightPhotons_Systematics_RefJet_smearedJER_ety;
   Float_t         MET_Simplified20_Systematics_RefJet_plusSigmaJES_etx;
   Float_t         MET_Simplified20_Systematics_RefJet_plusSigmaJES_ety;
   Float_t         MET_Simplified20withTightPhotons_Systematics_RefJet_plusSigmaJES_etx;
   Float_t         MET_Simplified20withTightPhotons_Systematics_RefJet_plusSigmaJES_ety;
   Float_t         MET_Simplified20_Systematics_RefJet_minusSigmaJES_etx;
   Float_t         MET_Simplified20_Systematics_RefJet_minusSigmaJES_ety;
   Float_t         MET_Simplified20withTightPhotons_Systematics_RefJet_minusSigmaJES_etx;
   Float_t         MET_Simplified20withTightPhotons_Systematics_RefJet_minusSigmaJES_ety;
   Float_t         MET_Simplified20_Systematics_CellOut_plusSigma_etx;
   Float_t         MET_Simplified20_Systematics_CellOut_plusSigma_ety;
   Float_t         MET_Simplified20withTightPhotons_Systematics_CellOut_plusSigma_etx;
   Float_t         MET_Simplified20withTightPhotons_Systematics_CellOut_plusSigma_ety;
   Float_t         MET_Simplified20_Systematics_CellOut_minusSigma_etx;
   Float_t         MET_Simplified20_Systematics_CellOut_minusSigma_ety;
   Float_t         MET_Simplified20withTightPhotons_Systematics_CellOut_minusSigma_etx;
   Float_t         MET_Simplified20withTightPhotons_Systematics_CellOut_minusSigma_ety;
   Float_t         MET_Simplified20_Systematics_CellOut_smeared_etx;
   Float_t         MET_Simplified20_Systematics_CellOut_smeared_ety;
   Float_t         MET_Simplified20withTightPhotons_Systematics_CellOut_smeared_etx;
   Float_t         MET_Simplified20withTightPhotons_Systematics_CellOut_smeared_ety;
   Int_t           jet_AntiKt4TopoNewEM_n;
   vector<float>   *jet_AntiKt4TopoNewEM_E;
   vector<float>   *jet_AntiKt4TopoNewEM_pt;
   vector<float>   *jet_AntiKt4TopoNewEM_m;
   vector<float>   *jet_AntiKt4TopoNewEM_eta;
   vector<float>   *jet_AntiKt4TopoNewEM_emscale_eta;
   vector<float>   *jet_AntiKt4TopoNewEM_phi;
   vector<float>   *jet_AntiKt4TopoNewEM_jvtxf;
   vector<float>   *jet_AntiKt4TopoNewEM_n90;
   vector<float>   *jet_AntiKt4TopoNewEM_Timing;
   vector<float>   *jet_AntiKt4TopoNewEM_LArQuality;
   vector<float>   *jet_AntiKt4TopoNewEM_sumPtTrk;
   vector<float>   *jet_AntiKt4TopoNewEM_HECQuality;
   vector<float>   *jet_AntiKt4TopoNewEM_NegativeE;
   vector<float>   *jet_AntiKt4TopoNewEM_BCH_CORR_CELL;
   vector<float>   *jet_AntiKt4TopoNewEM_BCH_CORR_JET;
   vector<float>   *jet_AntiKt4TopoNewEM_BCH_CORR_DOTX;
   vector<float>   *jet_AntiKt4TopoNewEM_fracSamplingMax;
   vector<float>   *jet_AntiKt4TopoNewEM_hecf;
   vector<float>   *jet_AntiKt4TopoNewEM_emfrac;
   vector<float>   *jet_AntiKt4TopoNewEM_e_HEC0;
   vector<float>   *jet_AntiKt4TopoNewEM_e_HEC1;
   vector<float>   *jet_AntiKt4TopoNewEM_e_HEC2;
   vector<float>   *jet_AntiKt4TopoNewEM_e_HEC3;
   vector<float>   *jet_AntiKt4TopoNewEM_e_TileGap3;
   vector<double>  *jet_AntiKt4TopoNewEM_flavor_weight_SV0;
   Int_t           vx_n;
   vector<int>     *vx_nTracks;
   vector<vector<double> >  *mcevt_weight;
   vector<vector<float> > *trig_EF_trigmuonef_track_CB_eta;
   vector<vector<float> > *trig_EF_trigmuonef_track_CB_phi;
   vector<vector<int> > *trig_EF_trigmuonef_track_CB_hasCB;
   vector<vector<float> > *trig_EF_trigmuonef_track_CB_pt;
   vector<int>     *trig_EF_trigmuonef_track_n;
   Int_t           trig_EF_trigmuonef_n;

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_lbn;   //!
   TBranch        *b_bcid;   //!
   TBranch        *b_el_n;   //!
   TBranch        *b_el_E;   //!
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
   TBranch        *b_el_OQ;   //!
   TBranch        *b_el_medium;   //!
   TBranch        *b_el_weta2;   //!
   TBranch        *b_el_Etcone20;   //!
   TBranch        *b_el_expectHitInBLayer;   //!
   TBranch        *b_el_reta;   //!
   TBranch        *b_el_etas2;   //!
   TBranch        *b_el_cl_E;   //!
   TBranch        *b_el_cl_pt;   //!
   TBranch        *b_el_cl_eta;   //!
   TBranch        *b_el_cl_phi;   //!
   TBranch        *b_el_trackphi;   //!
   TBranch        *b_el_tracketa;   //!
   TBranch        *b_el_nPixHits;   //!
   TBranch        *b_el_nSCTHits;   //!
   TBranch        *b_el_ptcone20;   //!
   TBranch        *b_EF_e10_medium;   //!
   TBranch        *b_EF_e20_medium;   //!
   TBranch        *b_EF_mu10;   //!
   TBranch        *b_EF_mu13;   //!
   TBranch        *b_EF_mu18;   //!
   TBranch        *b_EF_mu20;   //!
   TBranch        *b_L1_EM10;   //!
   TBranch        *b_L1_J10;   //!
   TBranch        *b_L1_J15;   //!
   TBranch        *b_L1_J30;   //!
   TBranch        *b_L1_J75;   //!
   TBranch        *b_L1_MU0;   //!
   TBranch        *b_L1_MU10;   //!
   TBranch        *b_L1_MU6;   //!
   TBranch        *b_L2_e10_medium;   //!
   TBranch        *b_L2_mu10;   //!
	TBranch			*b_larError;	//!
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
   TBranch        *b_mu_staco_matchchi2;   //!
   TBranch        *b_mu_staco_etcone20;   //!
   TBranch        *b_mu_staco_ptcone20;   //!
   TBranch        *b_mu_staco_bestMatch;   //!
   TBranch        *b_mu_staco_isCombinedMuon;   //!
   TBranch        *b_mu_staco_isLowPtReconstructedMuon;   //!
   TBranch        *b_mu_staco_tight;   //!
   TBranch        *b_mu_staco_loose;   //!
   TBranch        *b_mu_staco_d0_exPV;   //!
   TBranch        *b_mu_staco_z0_exPV;   //!
   TBranch        *b_mu_staco_id_theta_exPV;   //!
   TBranch        *b_mu_staco_id_qoverp_exPV;   //!
   TBranch        *b_mu_staco_me_theta_exPV;   //!
   TBranch        *b_mu_staco_me_qoverp_exPV;   //!
   TBranch        *b_mu_staco_qoverp_exPV;   //!
   TBranch        *b_mu_staco_cov_qoverp_exPV;   //!
   TBranch        *b_mu_staco_id_theta;   //!
   TBranch        *b_mu_staco_nBLHits;   //!
   TBranch        *b_mu_staco_nPixHits;   //!
   TBranch        *b_mu_staco_nPixHoles;   //!
   TBranch        *b_mu_staco_nSCTHits;   //!
   TBranch        *b_mu_staco_nTRTHits;   //!
   TBranch        *b_mu_staco_nSCTHoles;   //!
   TBranch        *b_mu_staco_nTRTOutliers;   //!
   TBranch        *b_mu_staco_nPixelDeadSensors;   //!
   TBranch        *b_mu_staco_nSCTDeadSensors;   //!
   TBranch        *b_mu_staco_expectBLayerHit;   //!
   TBranch        *b_MET_RefFinal_etx;   //!
   TBranch        *b_MET_RefFinal_ety;   //!
   TBranch	  *b_MET_Simplified20_RefFinal_etx_CentralReg;   //!
   TBranch	  *b_MET_Simplified20_RefFinal_ety_CentralReg;   //!
   TBranch	  *b_MET_Simplified20_RefFinal_etx_EndcapRegion;   //!
   TBranch	  *b_MET_Simplified20_RefFinal_ety_EndcapRegion;   //!
   TBranch	  *b_MET_Simplified20_RefFinal_etx_ForwardReg;   //!
   TBranch	  *b_MET_Simplified20_RefFinal_ety_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_phi;   //!
   TBranch        *b_MET_RefFinal_et;   //!
   TBranch        *b_MET_RefEle_etx;   //!
   TBranch        *b_MET_RefEle_ety;   //!
   TBranch        *b_MET_RefJet_etx;   //!
   TBranch        *b_MET_RefJet_ety;   //!
   TBranch        *b_MET_Simplified20_RefJet_etx_CentralReg;   //!
   TBranch	  *b_MET_Simplified20_RefJet_ety_CentralReg;   //!
   TBranch	  *b_MET_Simplified20_RefJet_etx_EndcapRegion;   //!
   TBranch	  *b_MET_Simplified20_RefJet_ety_EndcapRegion;   //!
   TBranch	  *b_MET_Simplified20_RefJet_etx_ForwardReg;   //!
   TBranch	  *b_MET_Simplified20_RefJet_ety_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_etx;   //!
   TBranch        *b_MET_SoftJets_ety;   //!
   TBranch        *b_MET_RefMuon_etx;   //!
   TBranch        *b_MET_RefMuon_ety;   //!
   TBranch        *b_MET_RefMuon_Staco_etx;   //!
   TBranch        *b_MET_RefMuon_Staco_ety;   //!
   TBranch        *b_MET_RefMuon_Muid_etx;   //!
   TBranch        *b_MET_RefMuon_Muid_ety;   //!
   TBranch        *b_MET_RefTau_etx;   //!
   TBranch        *b_MET_RefTau_ety;   //!
   TBranch        *b_MET_CellOut_etx;   //!
   TBranch        *b_MET_CellOut_ety;   //!
   TBranch        *b_MET_Track_etx;   //!
   TBranch        *b_MET_Track_ety;   //!
   TBranch        *b_MET_RefFinal_em_etx;   //!
   TBranch        *b_MET_RefFinal_em_ety;   //!
   TBranch        *b_MET_RefEle_em_etx;   //!
   TBranch        *b_MET_RefEle_em_ety;   //!
   TBranch        *b_MET_RefJet_em_etx;   //!
   TBranch        *b_MET_RefJet_em_ety;   //!
   TBranch        *b_MET_SoftJets_em_etx;   //!
   TBranch        *b_MET_SoftJets_em_ety;   //!
   TBranch        *b_MET_RefMuon_em_etx;   //!
   TBranch        *b_MET_RefMuon_em_ety;   //!
   TBranch        *b_MET_RefGamma_em_etx;   //!
   TBranch        *b_MET_RefGamma_em_ety;   //!
   TBranch        *b_MET_RefTau_em_etx;   //!
   TBranch        *b_MET_RefTau_em_ety;   //!
   TBranch        *b_MET_Muon_Isol_Staco_etx;   //!
   TBranch        *b_MET_Muon_Isol_Staco_ety;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_etx;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_ety;   //!
   TBranch        *b_MET_Muon_Total_Staco_etx;   //!
   TBranch        *b_MET_Muon_Total_Staco_ety;   //!
   TBranch        *b_MET_Muon_Isol_Muid_etx;   //!
   TBranch        *b_MET_Muon_Isol_Muid_ety;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_etx;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_ety;   //!
   TBranch        *b_MET_Muon_Total_Muid_etx;   //!
   TBranch        *b_MET_Muon_Total_Muid_ety;   //!
   TBranch        *b_MET_MuonBoy_etx;   //!
   TBranch        *b_MET_MuonBoy_ety;   //!
   TBranch        *b_MET_RefMuon_Track_etx;   //!
   TBranch        *b_MET_RefMuon_Track_ety;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_etx;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_ety;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_etx;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_ety;   //!
   TBranch        *b_MET_Final_etx;   //!
   TBranch        *b_MET_Final_ety;   //!
   TBranch        *b_MET_TopoObj_etx;   //!
   TBranch        *b_MET_TopoObj_ety;   //!
   TBranch        *b_MET_LocHadTopo_etx;   //!
   TBranch        *b_MET_LocHadTopo_ety;   //!
   TBranch        *b_MET_Simplified20_RefGamma_etx;   //!
   TBranch        *b_MET_Simplified20_RefGamma_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_RefGamma_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_RefGamma_ety;   //!
   TBranch        *b_MET_Simplified20_RefFinal_etx;   //!
   TBranch        *b_MET_Simplified20_RefFinal_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_RefFinal_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_RefFinal_ety;   //!
   TBranch        *b_MET_Simplified20_CellOut_etx;   //!
   TBranch        *b_MET_Simplified20_CellOut_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_CellOut_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_CellOut_ety;   //!
   TBranch        *b_MET_Simplified20_RefJet_etx;   //!
   TBranch        *b_MET_Simplified20_RefJet_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_RefJet_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_RefJet_ety;   //!
   TBranch        *b_MET_Simplified20_RefEle_etx;   //!
   TBranch        *b_MET_Simplified20_RefEle_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_RefEle_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_RefEle_ety;   //!
   TBranch        *b_MET_Simplified20_Muon_Total_Staco_etx;   //!
   TBranch        *b_MET_Simplified20_Muon_Total_Staco_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_Total_Staco_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_Total_Staco_ety;   //!
   TBranch        *b_MET_Simplified20_Muon_Isol_Staco_etx;   //!
   TBranch        *b_MET_Simplified20_Muon_Isol_Staco_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_Isol_Staco_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_Isol_Staco_ety;   //!
   TBranch        *b_MET_Simplified20_Muon_NonIsol_Staco_etx;   //!
   TBranch        *b_MET_Simplified20_Muon_NonIsol_Staco_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_NonIsol_Staco_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_NonIsol_Staco_ety;   //!
   TBranch        *b_MET_Simplified20_Muon_Isol_Muid_etx;   //!
   TBranch        *b_MET_Simplified20_Muon_Isol_Muid_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_Isol_Muid_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_Isol_Muid_ety;   //!
   TBranch        *b_MET_Simplified20_Muon_NonIsol_Muid_etx;   //!
   TBranch        *b_MET_Simplified20_Muon_NonIsol_Muid_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_NonIsol_Muid_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_NonIsol_Muid_ety;   //!
   TBranch        *b_MET_Simplified20_Muon_Total_Muid_etx;   //!
   TBranch        *b_MET_Simplified20_Muon_Total_Muid_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_Total_Muid_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Muon_Total_Muid_ety;   //!
   TBranch        *b_MET_Simplified20_Systematics_RefJet_smearedJER_etx;   //!
   TBranch        *b_MET_Simplified20_Systematics_RefJet_smearedJER_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_RefJet_smearedJER_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_RefJet_smearedJER_ety;   //!
   TBranch        *b_MET_Simplified20_Systematics_RefJet_plusSigmaJES_etx;   //!
   TBranch        *b_MET_Simplified20_Systematics_RefJet_plusSigmaJES_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_RefJet_plusSigmaJES_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_RefJet_plusSigmaJES_ety;   //!
   TBranch        *b_MET_Simplified20_Systematics_RefJet_minusSigmaJES_etx;   //!
   TBranch        *b_MET_Simplified20_Systematics_RefJet_minusSigmaJES_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_RefJet_minusSigmaJES_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_RefJet_minusSigmaJES_ety;   //!
   TBranch        *b_MET_Simplified20_Systematics_CellOut_plusSigma_etx;   //!
   TBranch        *b_MET_Simplified20_Systematics_CellOut_plusSigma_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_CellOut_plusSigma_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_CellOut_plusSigma_ety;   //!
   TBranch        *b_MET_Simplified20_Systematics_CellOut_minusSigma_etx;   //!
   TBranch        *b_MET_Simplified20_Systematics_CellOut_minusSigma_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_CellOut_minusSigma_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_CellOut_minusSigma_ety;   //!
   TBranch        *b_MET_Simplified20_Systematics_CellOut_smeared_etx;   //!
   TBranch        *b_MET_Simplified20_Systematics_CellOut_smeared_ety;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_CellOut_smeared_etx;   //!
   TBranch        *b_MET_Simplified20withTightPhotons_Systematics_CellOut_smeared_ety;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_n;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_E;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_pt;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_m;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_eta;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_emscale_eta;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_phi;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_jvtxf;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_n90;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_Timing;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_LArQuality;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_sumPtTrk;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_HECQuality;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_NegativeE;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_BCH_CORR_CELL;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_BCH_CORR_JET;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_BCH_CORR_DOTX;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_fracSamplingMax;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_hecf;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_emfrac;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_e_HEC0;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_e_HEC1;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_e_HEC2;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_e_HEC3;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_e_TileGap3;   //!
   TBranch        *b_jet_AntiKt4TopoNewEM_flavor_weight_SV0;   //!
   TBranch        *b_vx_n;   //!
   TBranch        *b_vx_nTracks;   //!
   TBranch        *b_mcevt_weight;   //!
   TBranch        *b_trig_EF_trigmuonef_track_n;   //!
   TBranch        *b_trig_EF_trigmuonef_track_CB_eta;   //!
   TBranch        *b_trig_EF_trigmuonef_track_CB_phi;   //!
   TBranch        *b_trig_EF_trigmuonef_track_CB_hasCB;   //!
   TBranch        *b_trig_EF_trigmuonef_track_CB_pt;   //!
   TBranch        *b_trig_EF_trigmuonef_n;   //!


   Reco_dpd2(TTree *tree=0);
   virtual ~Reco_dpd2();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   //virtual void     Loop();
   virtual void     Loop(const std::string &argStr);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   vector<int> get_good_electron_index(int myRunNumber, int kIsData);
   vector<int> get_overlap_electron_index(vector<int> elec_index_met);
   vector<int> get_iso_electron_index(int n_jet,vector<int> jet_index, vector<int> elec_index_noiso /*, int kIsData*/);
   vector<int> get_signal_electron_index(vector<int> elec_index);
   vector<int> get_good_muon_index(int n_jet,vector<int> jet_index, vector<int> muon_index2 /*, int kIsData*/);
   vector<int> get_good_muon_index2(int kIsData);
   vector<int> get_good_muon_index_iso(vector<int> muon_index);
   vector<int> get_muon_cosmics(vector<int> muon_index);
   vector<int> get_good_jet_index(int n_elec,vector<int> elec_index, int KIsData);
   vector<int> get_signal_jet_index(vector<Container> jet_index);
   int get_jet(int ijet, int n_elec, vector<int> elec_index, int KIsData);
   int Mu_JetinDR04(int imuon,int n_jet,vector<int> jet_index);
   double calculate_meff(vector<int> jet, int n_elec, int n_muon, vector<int> elec_index, vector<int> muon_index, double met);
   double calculate_mt_1lepton(int n_elec, int n_muon, vector<int> elec_index, vector<int> muon_index, double metx, double mety, double met);
   int El_JetinDR04(int ielec,int n_jet,vector<int> jet_index);

 private:
   SUSYObjDef susyobj;
   APReweightND *TrigWeighter;    //Declaration of weighting instance 
   FakeMetEstimator fakeMetEst;
};

#endif

#ifdef Reco_dpd2_cxx
Reco_dpd2::Reco_dpd2(TTree *tree)
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
      tree = (TTree*)gDirectory->Get("susy");

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("susy","");
      chain->Add("slimmed_d3pd.root/susy");
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

Reco_dpd2::~Reco_dpd2()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Reco_dpd2::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Reco_dpd2::LoadTree(Long64_t entry)
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

void Reco_dpd2::Init(TTree *tree)
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
   el_OQ = 0;
   el_medium = 0;
   el_weta2 = 0;
   el_Etcone20 = 0;
   el_expectHitInBLayer = 0;
   el_reta = 0;
   el_etas2 = 0;
   el_cl_E = 0;
   el_cl_pt = 0;
   el_cl_eta = 0;
   el_cl_phi = 0;
   el_trackphi = 0;
   el_tracketa = 0;
   el_nPixHits = 0;
   el_nSCTHits = 0;
   el_ptcone20 = 0;
   mu_staco_E = 0;
   mu_staco_pt = 0;
   mu_staco_m = 0;
   mu_staco_eta = 0;
   mu_staco_phi = 0;
   mu_staco_px = 0;
   mu_staco_py = 0;
   mu_staco_pz = 0;
   mu_staco_charge = 0;
   mu_staco_matchchi2 = 0;
   mu_staco_etcone20 = 0;
   mu_staco_ptcone20 = 0;
   mu_staco_bestMatch = 0;
   mu_staco_isCombinedMuon = 0;
   mu_staco_isLowPtReconstructedMuon = 0;
   mu_staco_tight = 0;
   mu_staco_loose = 0;
   mu_staco_d0_exPV = 0;
   mu_staco_z0_exPV = 0;
   mu_staco_id_theta_exPV = 0;
   mu_staco_id_qoverp_exPV = 0;
   mu_staco_me_theta_exPV = 0;
   mu_staco_me_qoverp_exPV = 0;
   mu_staco_qoverp_exPV = 0;
   mu_staco_cov_qoverp_exPV = 0;
   mu_staco_id_theta = 0;
   mu_staco_nBLHits = 0;
   mu_staco_nPixHits = 0;
   mu_staco_nPixHoles = 0;
   mu_staco_nSCTHits = 0;
   mu_staco_nTRTHits = 0;
   mu_staco_nSCTHoles = 0;
   mu_staco_nTRTOutliers = 0;
   mu_staco_nPixelDeadSensors = 0;
   mu_staco_nSCTDeadSensors = 0;
   mu_staco_expectBLayerHit = 0;
   jet_AntiKt4TopoNewEM_E = 0;
   jet_AntiKt4TopoNewEM_pt = 0;
   jet_AntiKt4TopoNewEM_m = 0;
   jet_AntiKt4TopoNewEM_eta = 0;
   jet_AntiKt4TopoNewEM_emscale_eta = 0;
   jet_AntiKt4TopoNewEM_phi = 0;
   jet_AntiKt4TopoNewEM_jvtxf = 0;
   jet_AntiKt4TopoNewEM_n90 = 0;
   jet_AntiKt4TopoNewEM_Timing = 0;
   jet_AntiKt4TopoNewEM_LArQuality = 0;
   jet_AntiKt4TopoNewEM_sumPtTrk = 0;
   jet_AntiKt4TopoNewEM_HECQuality = 0;
   jet_AntiKt4TopoNewEM_NegativeE = 0;
   jet_AntiKt4TopoNewEM_BCH_CORR_CELL = 0;
   jet_AntiKt4TopoNewEM_BCH_CORR_JET = 0;
   jet_AntiKt4TopoNewEM_BCH_CORR_DOTX = 0;
   jet_AntiKt4TopoNewEM_fracSamplingMax = 0;
   jet_AntiKt4TopoNewEM_hecf = 0;
   jet_AntiKt4TopoNewEM_emfrac = 0;
   jet_AntiKt4TopoNewEM_e_HEC0 = 0;
   jet_AntiKt4TopoNewEM_e_HEC1 = 0;
   jet_AntiKt4TopoNewEM_e_HEC2 = 0;
   jet_AntiKt4TopoNewEM_e_HEC3 = 0;
   jet_AntiKt4TopoNewEM_e_TileGap3 = 0;
   jet_AntiKt4TopoNewEM_flavor_weight_SV0 = 0;
   vx_nTracks = 0;
   mcevt_weight = 0;
   trig_EF_trigmuonef_track_CB_eta = 0;
   trig_EF_trigmuonef_track_CB_phi = 0;
   trig_EF_trigmuonef_track_CB_pt = 0;
   trig_EF_trigmuonef_track_CB_hasCB = 0;
   trig_EF_trigmuonef_track_n = 0;

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
   fChain->SetBranchAddress("lbn", &lbn, &b_lbn);
   fChain->SetBranchAddress("bcid", &bcid, &b_bcid);
   fChain->SetBranchAddress("el_n", &el_n, &b_el_n);
   fChain->SetBranchAddress("el_E", &el_E, &b_el_E);
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
   fChain->SetBranchAddress("el_OQ", &el_OQ, &b_el_OQ);
   fChain->SetBranchAddress("el_medium", &el_medium, &b_el_medium);
   fChain->SetBranchAddress("el_weta2", &el_weta2, &b_el_weta2);
   fChain->SetBranchAddress("el_Etcone20", &el_Etcone20, &b_el_Etcone20);
   fChain->SetBranchAddress("el_expectHitInBLayer", &el_expectHitInBLayer, &b_el_expectHitInBLayer);
   fChain->SetBranchAddress("el_reta", &el_reta, &b_el_reta);
   fChain->SetBranchAddress("el_etas2", &el_etas2, &b_el_etas2);
   fChain->SetBranchAddress("el_cl_E", &el_cl_E, &b_el_cl_E);
   fChain->SetBranchAddress("el_cl_pt", &el_cl_pt, &b_el_cl_pt);
   fChain->SetBranchAddress("el_cl_eta", &el_cl_eta, &b_el_cl_eta);
   fChain->SetBranchAddress("el_cl_phi", &el_cl_phi, &b_el_cl_phi);
   fChain->SetBranchAddress("el_trackphi", &el_trackphi, &b_el_trackphi);
   fChain->SetBranchAddress("el_tracketa", &el_tracketa, &b_el_tracketa);
   fChain->SetBranchAddress("el_nPixHits", &el_nPixHits, &b_el_nPixHits);
   fChain->SetBranchAddress("el_nSCTHits", &el_nSCTHits, &b_el_nSCTHits);
   fChain->SetBranchAddress("el_ptcone20", &el_ptcone20, &b_el_ptcone20);
   fChain->SetBranchAddress("EF_e10_medium", &EF_e10_medium, &b_EF_e10_medium);
   fChain->SetBranchAddress("EF_e20_medium", &EF_e20_medium, &b_EF_e20_medium);
   fChain->SetBranchAddress("EF_mu10", &EF_mu10, &b_EF_mu10);
   fChain->SetBranchAddress("EF_mu13", &EF_mu13, &b_EF_mu13);
   fChain->SetBranchAddress("EF_mu18", &EF_mu18, &b_EF_mu18);
   fChain->SetBranchAddress("EF_mu20", &EF_mu20, &b_EF_mu20);
   fChain->SetBranchAddress("L1_EM10", &L1_EM10, &b_L1_EM10);
   fChain->SetBranchAddress("L1_J10", &L1_J10, &b_L1_J10);
   fChain->SetBranchAddress("L1_J15", &L1_J15, &b_L1_J15);
   fChain->SetBranchAddress("L1_J30", &L1_J30, &b_L1_J30);
   fChain->SetBranchAddress("L1_J75", &L1_J75, &b_L1_J75);
   fChain->SetBranchAddress("L1_MU0", &L1_MU0, &b_L1_MU0);
   fChain->SetBranchAddress("L1_MU10", &L1_MU10, &b_L1_MU10);
   fChain->SetBranchAddress("L1_MU6", &L1_MU6, &b_L1_MU6);
   fChain->SetBranchAddress("L2_e10_medium", &L2_e10_medium, &b_L2_e10_medium);
   fChain->SetBranchAddress("L2_mu10", &L2_mu10, &b_L2_mu10);
	fChain->SetBranchAddress("larError", &larError, &b_larError);
   fChain->SetBranchAddress("mu_staco_n", &mu_staco_n, &b_mu_staco_n);
   fChain->SetBranchAddress("mu_staco_E", &mu_staco_E, &b_mu_staco_E);
   fChain->SetBranchAddress("mu_staco_pt", &mu_staco_pt, &b_mu_staco_pt);
   fChain->SetBranchAddress("mu_staco_m", &mu_staco_m, &b_mu_staco_m);
   fChain->SetBranchAddress("mu_staco_eta", &mu_staco_eta, &b_mu_staco_eta);
   fChain->SetBranchAddress("mu_staco_phi", &mu_staco_phi, &b_mu_staco_phi);
   fChain->SetBranchAddress("mu_staco_px", &mu_staco_px, &b_mu_staco_px);
   fChain->SetBranchAddress("mu_staco_py", &mu_staco_py, &b_mu_staco_py);
   fChain->SetBranchAddress("mu_staco_pz", &mu_staco_pz, &b_mu_staco_pz);
   fChain->SetBranchAddress("mu_staco_charge", &mu_staco_charge, &b_mu_staco_charge);
   fChain->SetBranchAddress("mu_staco_matchchi2", &mu_staco_matchchi2, &b_mu_staco_matchchi2);
   fChain->SetBranchAddress("mu_staco_etcone20", &mu_staco_etcone20, &b_mu_staco_etcone20);
   fChain->SetBranchAddress("mu_staco_ptcone20", &mu_staco_ptcone20, &b_mu_staco_ptcone20);
   fChain->SetBranchAddress("mu_staco_bestMatch", &mu_staco_bestMatch, &b_mu_staco_bestMatch);
   fChain->SetBranchAddress("mu_staco_isCombinedMuon", &mu_staco_isCombinedMuon, &b_mu_staco_isCombinedMuon);
   fChain->SetBranchAddress("mu_staco_isLowPtReconstructedMuon", &mu_staco_isLowPtReconstructedMuon, &b_mu_staco_isLowPtReconstructedMuon);
   fChain->SetBranchAddress("mu_staco_tight", &mu_staco_tight, &b_mu_staco_tight);
   fChain->SetBranchAddress("mu_staco_loose", &mu_staco_loose, &b_mu_staco_loose);
   fChain->SetBranchAddress("mu_staco_d0_exPV", &mu_staco_d0_exPV, &b_mu_staco_d0_exPV);
   fChain->SetBranchAddress("mu_staco_z0_exPV", &mu_staco_z0_exPV, &b_mu_staco_z0_exPV);
   fChain->SetBranchAddress("mu_staco_id_theta_exPV", &mu_staco_id_theta_exPV, &b_mu_staco_id_theta_exPV);
   fChain->SetBranchAddress("mu_staco_id_qoverp_exPV", &mu_staco_id_qoverp_exPV, &b_mu_staco_id_qoverp_exPV);
   fChain->SetBranchAddress("mu_staco_me_theta_exPV", &mu_staco_me_theta_exPV, &b_mu_staco_me_theta_exPV);
   fChain->SetBranchAddress("mu_staco_me_qoverp_exPV", &mu_staco_me_qoverp_exPV, &b_mu_staco_me_qoverp_exPV);
   fChain->SetBranchAddress("mu_staco_qoverp_exPV", &mu_staco_qoverp_exPV, &b_mu_staco_qoverp_exPV);
   fChain->SetBranchAddress("mu_staco_cov_qoverp_exPV", &mu_staco_cov_qoverp_exPV, &b_mu_staco_cov_qoverp_exPV);
   fChain->SetBranchAddress("mu_staco_id_theta", &mu_staco_id_theta, &b_mu_staco_id_theta);
   fChain->SetBranchAddress("mu_staco_nBLHits", &mu_staco_nBLHits, &b_mu_staco_nBLHits);
   fChain->SetBranchAddress("mu_staco_nPixHits", &mu_staco_nPixHits, &b_mu_staco_nPixHits);
   fChain->SetBranchAddress("mu_staco_nPixHoles", &mu_staco_nPixHoles, &b_mu_staco_nPixHoles);
   fChain->SetBranchAddress("mu_staco_nSCTHits", &mu_staco_nSCTHits, &b_mu_staco_nSCTHits);
   fChain->SetBranchAddress("mu_staco_nTRTHits", &mu_staco_nTRTHits, &b_mu_staco_nTRTHits);
   fChain->SetBranchAddress("mu_staco_nSCTHoles", &mu_staco_nSCTHoles, &b_mu_staco_nSCTHoles);
   fChain->SetBranchAddress("mu_staco_nTRTOutliers", &mu_staco_nTRTOutliers, &b_mu_staco_nTRTOutliers);
   fChain->SetBranchAddress("mu_staco_nPixelDeadSensors", &mu_staco_nPixelDeadSensors, &b_mu_staco_nPixelDeadSensors);
   fChain->SetBranchAddress("mu_staco_nSCTDeadSensors", &mu_staco_nSCTDeadSensors, &b_mu_staco_nSCTDeadSensors);
   fChain->SetBranchAddress("mu_staco_expectBLayerHit", &mu_staco_expectBLayerHit, &b_mu_staco_expectBLayerHit);
   fChain->SetBranchAddress("MET_RefFinal_etx", &MET_RefFinal_etx, &b_MET_RefFinal_etx);
   fChain->SetBranchAddress("MET_RefFinal_ety", &MET_RefFinal_ety, &b_MET_RefFinal_ety);
   fChain->SetBranchAddress("MET_Simplified20_RefFinal_etx_CentralReg", &MET_Simplified20_RefFinal_etx_CentralReg, &b_MET_Simplified20_RefFinal_etx_CentralReg);
   fChain->SetBranchAddress("MET_Simplified20_RefFinal_ety_CentralReg", &MET_Simplified20_RefFinal_ety_CentralReg, &b_MET_Simplified20_RefFinal_ety_CentralReg);
   fChain->SetBranchAddress("MET_Simplified20_RefFinal_etx_EndcapRegion", &MET_Simplified20_RefFinal_etx_EndcapRegion, &b_MET_Simplified20_RefFinal_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Simplified20_RefFinal_ety_EndcapRegion", &MET_Simplified20_RefFinal_ety_EndcapRegion, &b_MET_Simplified20_RefFinal_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Simplified20_RefFinal_etx_ForwardReg", &MET_Simplified20_RefFinal_etx_ForwardReg, &b_MET_Simplified20_RefFinal_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Simplified20_RefFinal_ety_ForwardReg", &MET_Simplified20_RefFinal_ety_ForwardReg, &b_MET_Simplified20_RefFinal_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_phi", &MET_RefFinal_phi, &b_MET_RefFinal_phi);
   fChain->SetBranchAddress("MET_RefFinal_et", &MET_RefFinal_et, &b_MET_RefFinal_et);
   fChain->SetBranchAddress("MET_RefEle_etx", &MET_RefEle_etx, &b_MET_RefEle_etx);
   fChain->SetBranchAddress("MET_RefEle_ety", &MET_RefEle_ety, &b_MET_RefEle_ety);
   fChain->SetBranchAddress("MET_RefJet_etx", &MET_RefJet_etx, &b_MET_RefJet_etx);
   fChain->SetBranchAddress("MET_RefJet_ety", &MET_RefJet_ety, &b_MET_RefJet_ety);
   fChain->SetBranchAddress("MET_Simplified20_RefJet_etx_CentralReg", &MET_Simplified20_RefJet_etx_CentralReg, &b_MET_Simplified20_RefJet_etx_CentralReg);
   fChain->SetBranchAddress("MET_Simplified20_RefJet_ety_CentralReg", &MET_Simplified20_RefJet_ety_CentralReg, &b_MET_Simplified20_RefJet_ety_CentralReg);
   fChain->SetBranchAddress("MET_Simplified20_RefJet_etx_EndcapRegion", &MET_Simplified20_RefJet_etx_EndcapRegion, &b_MET_Simplified20_RefJet_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Simplified20_RefJet_ety_EndcapRegion", &MET_Simplified20_RefJet_ety_EndcapRegion, &b_MET_Simplified20_RefJet_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Simplified20_RefJet_etx_ForwardReg", &MET_Simplified20_RefJet_etx_ForwardReg, &b_MET_Simplified20_RefJet_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Simplified20_RefJet_ety_ForwardReg", &MET_Simplified20_RefJet_ety_ForwardReg, &b_MET_Simplified20_RefJet_ety_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_etx", &MET_SoftJets_etx, &b_MET_SoftJets_etx);
   fChain->SetBranchAddress("MET_SoftJets_ety", &MET_SoftJets_ety, &b_MET_SoftJets_ety);
   fChain->SetBranchAddress("MET_RefMuon_etx", &MET_RefMuon_etx, &b_MET_RefMuon_etx);
   fChain->SetBranchAddress("MET_RefMuon_ety", &MET_RefMuon_ety, &b_MET_RefMuon_ety);
   fChain->SetBranchAddress("MET_RefMuon_Staco_etx", &MET_RefMuon_Staco_etx, &b_MET_RefMuon_Staco_etx);
   fChain->SetBranchAddress("MET_RefMuon_Staco_ety", &MET_RefMuon_Staco_ety, &b_MET_RefMuon_Staco_ety);
   fChain->SetBranchAddress("MET_RefMuon_Muid_etx", &MET_RefMuon_Muid_etx, &b_MET_RefMuon_Muid_etx);
   fChain->SetBranchAddress("MET_RefMuon_Muid_ety", &MET_RefMuon_Muid_ety, &b_MET_RefMuon_Muid_ety);
   fChain->SetBranchAddress("MET_RefTau_etx", &MET_RefTau_etx, &b_MET_RefTau_etx);
   fChain->SetBranchAddress("MET_RefTau_ety", &MET_RefTau_ety, &b_MET_RefTau_ety);
   fChain->SetBranchAddress("MET_CellOut_etx", &MET_CellOut_etx, &b_MET_CellOut_etx);
   fChain->SetBranchAddress("MET_CellOut_ety", &MET_CellOut_ety, &b_MET_CellOut_ety);
   fChain->SetBranchAddress("MET_Track_etx", &MET_Track_etx, &b_MET_Track_etx);
   fChain->SetBranchAddress("MET_Track_ety", &MET_Track_ety, &b_MET_Track_ety);
   fChain->SetBranchAddress("MET_RefFinal_em_etx", &MET_RefFinal_em_etx, &b_MET_RefFinal_em_etx);
   fChain->SetBranchAddress("MET_RefFinal_em_ety", &MET_RefFinal_em_ety, &b_MET_RefFinal_em_ety);
   fChain->SetBranchAddress("MET_RefEle_em_etx", &MET_RefEle_em_etx, &b_MET_RefEle_em_etx);
   fChain->SetBranchAddress("MET_RefEle_em_ety", &MET_RefEle_em_ety, &b_MET_RefEle_em_ety);
   fChain->SetBranchAddress("MET_RefJet_em_etx", &MET_RefJet_em_etx, &b_MET_RefJet_em_etx);
   fChain->SetBranchAddress("MET_RefJet_em_ety", &MET_RefJet_em_ety, &b_MET_RefJet_em_ety);
   fChain->SetBranchAddress("MET_SoftJets_em_etx", &MET_SoftJets_em_etx, &b_MET_SoftJets_em_etx);
   fChain->SetBranchAddress("MET_SoftJets_em_ety", &MET_SoftJets_em_ety, &b_MET_SoftJets_em_ety);
   fChain->SetBranchAddress("MET_RefMuon_em_etx", &MET_RefMuon_em_etx, &b_MET_RefMuon_em_etx);
   fChain->SetBranchAddress("MET_RefMuon_em_ety", &MET_RefMuon_em_ety, &b_MET_RefMuon_em_ety);
   fChain->SetBranchAddress("MET_RefGamma_em_etx", &MET_RefGamma_em_etx, &b_MET_RefGamma_em_etx);
   fChain->SetBranchAddress("MET_RefGamma_em_ety", &MET_RefGamma_em_ety, &b_MET_RefGamma_em_ety);
   fChain->SetBranchAddress("MET_RefTau_em_etx", &MET_RefTau_em_etx, &b_MET_RefTau_em_etx);
   fChain->SetBranchAddress("MET_RefTau_em_ety", &MET_RefTau_em_ety, &b_MET_RefTau_em_ety);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_etx", &MET_Muon_Isol_Staco_etx, &b_MET_Muon_Isol_Staco_etx);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_ety", &MET_Muon_Isol_Staco_ety, &b_MET_Muon_Isol_Staco_ety);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_etx", &MET_Muon_NonIsol_Staco_etx, &b_MET_Muon_NonIsol_Staco_etx);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_ety", &MET_Muon_NonIsol_Staco_ety, &b_MET_Muon_NonIsol_Staco_ety);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_etx", &MET_Muon_Total_Staco_etx, &b_MET_Muon_Total_Staco_etx);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_ety", &MET_Muon_Total_Staco_ety, &b_MET_Muon_Total_Staco_ety);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_etx", &MET_Muon_Isol_Muid_etx, &b_MET_Muon_Isol_Muid_etx);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_ety", &MET_Muon_Isol_Muid_ety, &b_MET_Muon_Isol_Muid_ety);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_etx", &MET_Muon_NonIsol_Muid_etx, &b_MET_Muon_NonIsol_Muid_etx);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_ety", &MET_Muon_NonIsol_Muid_ety, &b_MET_Muon_NonIsol_Muid_ety);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_etx", &MET_Muon_Total_Muid_etx, &b_MET_Muon_Total_Muid_etx);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_ety", &MET_Muon_Total_Muid_ety, &b_MET_Muon_Total_Muid_ety);
   fChain->SetBranchAddress("MET_MuonBoy_etx", &MET_MuonBoy_etx, &b_MET_MuonBoy_etx);
   fChain->SetBranchAddress("MET_MuonBoy_ety", &MET_MuonBoy_ety, &b_MET_MuonBoy_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_etx", &MET_RefMuon_Track_etx, &b_MET_RefMuon_Track_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_ety", &MET_RefMuon_Track_ety, &b_MET_RefMuon_Track_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_etx", &MET_RefMuon_Track_Staco_etx, &b_MET_RefMuon_Track_Staco_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_ety", &MET_RefMuon_Track_Staco_ety, &b_MET_RefMuon_Track_Staco_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_etx", &MET_RefMuon_Track_Muid_etx, &b_MET_RefMuon_Track_Muid_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_ety", &MET_RefMuon_Track_Muid_ety, &b_MET_RefMuon_Track_Muid_ety);
   fChain->SetBranchAddress("MET_Final_etx", &MET_Final_etx, &b_MET_Final_etx);
   fChain->SetBranchAddress("MET_Final_ety", &MET_Final_ety, &b_MET_Final_ety);
   fChain->SetBranchAddress("MET_TopoObj_etx", &MET_TopoObj_etx, &b_MET_TopoObj_etx);
   fChain->SetBranchAddress("MET_TopoObj_ety", &MET_TopoObj_ety, &b_MET_TopoObj_ety);
   fChain->SetBranchAddress("MET_LocHadTopo_etx", &MET_LocHadTopo_etx, &b_MET_LocHadTopo_etx);
   fChain->SetBranchAddress("MET_LocHadTopo_ety", &MET_LocHadTopo_ety, &b_MET_LocHadTopo_ety);
   fChain->SetBranchAddress("MET_Simplified20_RefGamma_etx", &MET_Simplified20_RefGamma_etx, &b_MET_Simplified20_RefGamma_etx);
   fChain->SetBranchAddress("MET_Simplified20_RefGamma_ety", &MET_Simplified20_RefGamma_ety, &b_MET_Simplified20_RefGamma_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_RefGamma_etx", &MET_Simplified20withTightPhotons_RefGamma_etx, &b_MET_Simplified20withTightPhotons_RefGamma_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_RefGamma_ety", &MET_Simplified20withTightPhotons_RefGamma_ety, &b_MET_Simplified20withTightPhotons_RefGamma_ety);
   fChain->SetBranchAddress("MET_Simplified20_RefFinal_etx", &MET_Simplified20_RefFinal_etx, &b_MET_Simplified20_RefFinal_etx);
   fChain->SetBranchAddress("MET_Simplified20_RefFinal_ety", &MET_Simplified20_RefFinal_ety, &b_MET_Simplified20_RefFinal_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_RefFinal_etx", &MET_Simplified20withTightPhotons_RefFinal_etx, &b_MET_Simplified20withTightPhotons_RefFinal_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_RefFinal_ety", &MET_Simplified20withTightPhotons_RefFinal_ety, &b_MET_Simplified20withTightPhotons_RefFinal_ety);
   fChain->SetBranchAddress("MET_Simplified20_CellOut_etx", &MET_Simplified20_CellOut_etx, &b_MET_Simplified20_CellOut_etx);
   fChain->SetBranchAddress("MET_Simplified20_CellOut_ety", &MET_Simplified20_CellOut_ety, &b_MET_Simplified20_CellOut_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_CellOut_etx", &MET_Simplified20withTightPhotons_CellOut_etx, &b_MET_Simplified20withTightPhotons_CellOut_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_CellOut_ety", &MET_Simplified20withTightPhotons_CellOut_ety, &b_MET_Simplified20withTightPhotons_CellOut_ety);
   fChain->SetBranchAddress("MET_Simplified20_RefJet_etx", &MET_Simplified20_RefJet_etx, &b_MET_Simplified20_RefJet_etx);
   fChain->SetBranchAddress("MET_Simplified20_RefJet_ety", &MET_Simplified20_RefJet_ety, &b_MET_Simplified20_RefJet_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_RefJet_etx", &MET_Simplified20withTightPhotons_RefJet_etx, &b_MET_Simplified20withTightPhotons_RefJet_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_RefJet_ety", &MET_Simplified20withTightPhotons_RefJet_ety, &b_MET_Simplified20withTightPhotons_RefJet_ety);
   fChain->SetBranchAddress("MET_Simplified20_RefEle_etx", &MET_Simplified20_RefEle_etx, &b_MET_Simplified20_RefEle_etx);
   fChain->SetBranchAddress("MET_Simplified20_RefEle_ety", &MET_Simplified20_RefEle_ety, &b_MET_Simplified20_RefEle_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_RefEle_etx", &MET_Simplified20withTightPhotons_RefEle_etx, &b_MET_Simplified20withTightPhotons_RefEle_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_RefEle_ety", &MET_Simplified20withTightPhotons_RefEle_ety, &b_MET_Simplified20withTightPhotons_RefEle_ety);
   fChain->SetBranchAddress("MET_Simplified20_Muon_Total_Staco_etx", &MET_Simplified20_Muon_Total_Staco_etx, &b_MET_Simplified20_Muon_Total_Staco_etx);
   fChain->SetBranchAddress("MET_Simplified20_Muon_Total_Staco_ety", &MET_Simplified20_Muon_Total_Staco_ety, &b_MET_Simplified20_Muon_Total_Staco_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_Total_Staco_etx", &MET_Simplified20withTightPhotons_Muon_Total_Staco_etx, &b_MET_Simplified20withTightPhotons_Muon_Total_Staco_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_Total_Staco_ety", &MET_Simplified20withTightPhotons_Muon_Total_Staco_ety, &b_MET_Simplified20withTightPhotons_Muon_Total_Staco_ety);
   fChain->SetBranchAddress("MET_Simplified20_Muon_Isol_Staco_etx", &MET_Simplified20_Muon_Isol_Staco_etx, &b_MET_Simplified20_Muon_Isol_Staco_etx);
   fChain->SetBranchAddress("MET_Simplified20_Muon_Isol_Staco_ety", &MET_Simplified20_Muon_Isol_Staco_ety, &b_MET_Simplified20_Muon_Isol_Staco_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_Isol_Staco_etx", &MET_Simplified20withTightPhotons_Muon_Isol_Staco_etx, &b_MET_Simplified20withTightPhotons_Muon_Isol_Staco_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_Isol_Staco_ety", &MET_Simplified20withTightPhotons_Muon_Isol_Staco_ety, &b_MET_Simplified20withTightPhotons_Muon_Isol_Staco_ety);
   fChain->SetBranchAddress("MET_Simplified20_Muon_NonIsol_Staco_etx", &MET_Simplified20_Muon_NonIsol_Staco_etx, &b_MET_Simplified20_Muon_NonIsol_Staco_etx);
   fChain->SetBranchAddress("MET_Simplified20_Muon_NonIsol_Staco_ety", &MET_Simplified20_Muon_NonIsol_Staco_ety, &b_MET_Simplified20_Muon_NonIsol_Staco_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_NonIsol_Staco_etx", &MET_Simplified20withTightPhotons_Muon_NonIsol_Staco_etx, &b_MET_Simplified20withTightPhotons_Muon_NonIsol_Staco_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_NonIsol_Staco_ety", &MET_Simplified20withTightPhotons_Muon_NonIsol_Staco_ety, &b_MET_Simplified20withTightPhotons_Muon_NonIsol_Staco_ety);
   fChain->SetBranchAddress("MET_Simplified20_Muon_Isol_Muid_etx", &MET_Simplified20_Muon_Isol_Muid_etx, &b_MET_Simplified20_Muon_Isol_Muid_etx);
   fChain->SetBranchAddress("MET_Simplified20_Muon_Isol_Muid_ety", &MET_Simplified20_Muon_Isol_Muid_ety, &b_MET_Simplified20_Muon_Isol_Muid_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_Isol_Muid_etx", &MET_Simplified20withTightPhotons_Muon_Isol_Muid_etx, &b_MET_Simplified20withTightPhotons_Muon_Isol_Muid_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_Isol_Muid_ety", &MET_Simplified20withTightPhotons_Muon_Isol_Muid_ety, &b_MET_Simplified20withTightPhotons_Muon_Isol_Muid_ety);
   fChain->SetBranchAddress("MET_Simplified20_Muon_NonIsol_Muid_etx", &MET_Simplified20_Muon_NonIsol_Muid_etx, &b_MET_Simplified20_Muon_NonIsol_Muid_etx);
   fChain->SetBranchAddress("MET_Simplified20_Muon_NonIsol_Muid_ety", &MET_Simplified20_Muon_NonIsol_Muid_ety, &b_MET_Simplified20_Muon_NonIsol_Muid_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_NonIsol_Muid_etx", &MET_Simplified20withTightPhotons_Muon_NonIsol_Muid_etx, &b_MET_Simplified20withTightPhotons_Muon_NonIsol_Muid_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_NonIsol_Muid_ety", &MET_Simplified20withTightPhotons_Muon_NonIsol_Muid_ety, &b_MET_Simplified20withTightPhotons_Muon_NonIsol_Muid_ety);
   fChain->SetBranchAddress("MET_Simplified20_Muon_Total_Muid_etx", &MET_Simplified20_Muon_Total_Muid_etx, &b_MET_Simplified20_Muon_Total_Muid_etx);
   fChain->SetBranchAddress("MET_Simplified20_Muon_Total_Muid_ety", &MET_Simplified20_Muon_Total_Muid_ety, &b_MET_Simplified20_Muon_Total_Muid_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_Total_Muid_etx", &MET_Simplified20withTightPhotons_Muon_Total_Muid_etx, &b_MET_Simplified20withTightPhotons_Muon_Total_Muid_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Muon_Total_Muid_ety", &MET_Simplified20withTightPhotons_Muon_Total_Muid_ety, &b_MET_Simplified20withTightPhotons_Muon_Total_Muid_ety);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_RefJet_smearedJER_etx", &MET_Simplified20_Systematics_RefJet_smearedJER_etx, &b_MET_Simplified20_Systematics_RefJet_smearedJER_etx);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_RefJet_smearedJER_ety", &MET_Simplified20_Systematics_RefJet_smearedJER_ety, &b_MET_Simplified20_Systematics_RefJet_smearedJER_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_RefJet_smearedJER_etx", &MET_Simplified20withTightPhotons_Systematics_RefJet_smearedJER_etx, &b_MET_Simplified20withTightPhotons_Systematics_RefJet_smearedJER_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_RefJet_smearedJER_ety", &MET_Simplified20withTightPhotons_Systematics_RefJet_smearedJER_ety, &b_MET_Simplified20withTightPhotons_Systematics_RefJet_smearedJER_ety);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_RefJet_plusSigmaJES_etx", &MET_Simplified20_Systematics_RefJet_plusSigmaJES_etx, &b_MET_Simplified20_Systematics_RefJet_plusSigmaJES_etx);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_RefJet_plusSigmaJES_ety", &MET_Simplified20_Systematics_RefJet_plusSigmaJES_ety, &b_MET_Simplified20_Systematics_RefJet_plusSigmaJES_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_RefJet_plusSigmaJES_etx", &MET_Simplified20withTightPhotons_Systematics_RefJet_plusSigmaJES_etx, &b_MET_Simplified20withTightPhotons_Systematics_RefJet_plusSigmaJES_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_RefJet_plusSigmaJES_ety", &MET_Simplified20withTightPhotons_Systematics_RefJet_plusSigmaJES_ety, &b_MET_Simplified20withTightPhotons_Systematics_RefJet_plusSigmaJES_ety);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_RefJet_minusSigmaJES_etx", &MET_Simplified20_Systematics_RefJet_minusSigmaJES_etx, &b_MET_Simplified20_Systematics_RefJet_minusSigmaJES_etx);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_RefJet_minusSigmaJES_ety", &MET_Simplified20_Systematics_RefJet_minusSigmaJES_ety, &b_MET_Simplified20_Systematics_RefJet_minusSigmaJES_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_RefJet_minusSigmaJES_etx", &MET_Simplified20withTightPhotons_Systematics_RefJet_minusSigmaJES_etx, &b_MET_Simplified20withTightPhotons_Systematics_RefJet_minusSigmaJES_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_RefJet_minusSigmaJES_ety", &MET_Simplified20withTightPhotons_Systematics_RefJet_minusSigmaJES_ety, &b_MET_Simplified20withTightPhotons_Systematics_RefJet_minusSigmaJES_ety);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_CellOut_plusSigma_etx", &MET_Simplified20_Systematics_CellOut_plusSigma_etx, &b_MET_Simplified20_Systematics_CellOut_plusSigma_etx);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_CellOut_plusSigma_ety", &MET_Simplified20_Systematics_CellOut_plusSigma_ety, &b_MET_Simplified20_Systematics_CellOut_plusSigma_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_CellOut_plusSigma_etx", &MET_Simplified20withTightPhotons_Systematics_CellOut_plusSigma_etx, &b_MET_Simplified20withTightPhotons_Systematics_CellOut_plusSigma_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_CellOut_plusSigma_ety", &MET_Simplified20withTightPhotons_Systematics_CellOut_plusSigma_ety, &b_MET_Simplified20withTightPhotons_Systematics_CellOut_plusSigma_ety);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_CellOut_minusSigma_etx", &MET_Simplified20_Systematics_CellOut_minusSigma_etx, &b_MET_Simplified20_Systematics_CellOut_minusSigma_etx);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_CellOut_minusSigma_ety", &MET_Simplified20_Systematics_CellOut_minusSigma_ety, &b_MET_Simplified20_Systematics_CellOut_minusSigma_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_CellOut_minusSigma_etx", &MET_Simplified20withTightPhotons_Systematics_CellOut_minusSigma_etx, &b_MET_Simplified20withTightPhotons_Systematics_CellOut_minusSigma_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_CellOut_minusSigma_ety", &MET_Simplified20withTightPhotons_Systematics_CellOut_minusSigma_ety, &b_MET_Simplified20withTightPhotons_Systematics_CellOut_minusSigma_ety);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_CellOut_smeared_etx", &MET_Simplified20_Systematics_CellOut_smeared_etx, &b_MET_Simplified20_Systematics_CellOut_smeared_etx);
   fChain->SetBranchAddress("MET_Simplified20_Systematics_CellOut_smeared_ety", &MET_Simplified20_Systematics_CellOut_smeared_ety, &b_MET_Simplified20_Systematics_CellOut_smeared_ety);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_CellOut_smeared_etx", &MET_Simplified20withTightPhotons_Systematics_CellOut_smeared_etx, &b_MET_Simplified20withTightPhotons_Systematics_CellOut_smeared_etx);
   fChain->SetBranchAddress("MET_Simplified20withTightPhotons_Systematics_CellOut_smeared_ety", &MET_Simplified20withTightPhotons_Systematics_CellOut_smeared_ety, &b_MET_Simplified20withTightPhotons_Systematics_CellOut_smeared_ety);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_n", &jet_AntiKt4TopoNewEM_n, &b_jet_AntiKt4TopoNewEM_n);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_E", &jet_AntiKt4TopoNewEM_E, &b_jet_AntiKt4TopoNewEM_E);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_pt", &jet_AntiKt4TopoNewEM_pt, &b_jet_AntiKt4TopoNewEM_pt);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_m", &jet_AntiKt4TopoNewEM_m, &b_jet_AntiKt4TopoNewEM_m);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_eta", &jet_AntiKt4TopoNewEM_eta, &b_jet_AntiKt4TopoNewEM_eta);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_emscale_eta", &jet_AntiKt4TopoNewEM_emscale_eta, &b_jet_AntiKt4TopoNewEM_emscale_eta);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_phi", &jet_AntiKt4TopoNewEM_phi, &b_jet_AntiKt4TopoNewEM_phi);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_jvtxf", &jet_AntiKt4TopoNewEM_jvtxf, &b_jet_AntiKt4TopoNewEM_jvtxf);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_n90", &jet_AntiKt4TopoNewEM_n90, &b_jet_AntiKt4TopoNewEM_n90);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_Timing", &jet_AntiKt4TopoNewEM_Timing, &b_jet_AntiKt4TopoNewEM_Timing);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_LArQuality", &jet_AntiKt4TopoNewEM_LArQuality, &b_jet_AntiKt4TopoNewEM_LArQuality);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_sumPtTrk", &jet_AntiKt4TopoNewEM_sumPtTrk, &b_jet_AntiKt4TopoNewEM_sumPtTrk);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_HECQuality", &jet_AntiKt4TopoNewEM_HECQuality, &b_jet_AntiKt4TopoNewEM_HECQuality);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_NegativeE", &jet_AntiKt4TopoNewEM_NegativeE, &b_jet_AntiKt4TopoNewEM_NegativeE);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_BCH_CORR_CELL", &jet_AntiKt4TopoNewEM_BCH_CORR_CELL, &b_jet_AntiKt4TopoNewEM_BCH_CORR_CELL);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_BCH_CORR_JET", &jet_AntiKt4TopoNewEM_BCH_CORR_JET, &b_jet_AntiKt4TopoNewEM_BCH_CORR_JET);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_BCH_CORR_DOTX", &jet_AntiKt4TopoNewEM_BCH_CORR_DOTX, &b_jet_AntiKt4TopoNewEM_BCH_CORR_DOTX);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_fracSamplingMax", &jet_AntiKt4TopoNewEM_fracSamplingMax, &b_jet_AntiKt4TopoNewEM_fracSamplingMax);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_hecf", &jet_AntiKt4TopoNewEM_hecf, &b_jet_AntiKt4TopoNewEM_hecf);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_emfrac", &jet_AntiKt4TopoNewEM_emfrac, &b_jet_AntiKt4TopoNewEM_emfrac);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_e_HEC0", &jet_AntiKt4TopoNewEM_e_HEC0, &b_jet_AntiKt4TopoNewEM_e_HEC0);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_e_HEC1", &jet_AntiKt4TopoNewEM_e_HEC1, &b_jet_AntiKt4TopoNewEM_e_HEC1);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_e_HEC2", &jet_AntiKt4TopoNewEM_e_HEC2, &b_jet_AntiKt4TopoNewEM_e_HEC2);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_e_HEC3", &jet_AntiKt4TopoNewEM_e_HEC3, &b_jet_AntiKt4TopoNewEM_e_HEC3);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_e_TileGap3", &jet_AntiKt4TopoNewEM_e_TileGap3, &b_jet_AntiKt4TopoNewEM_e_TileGap3);
   fChain->SetBranchAddress("jet_AntiKt4TopoNewEM_flavor_weight_SV0", &jet_AntiKt4TopoNewEM_flavor_weight_SV0, &b_jet_AntiKt4TopoNewEM_flavor_weight_SV0);
   fChain->SetBranchAddress("vx_n", &vx_n, &b_vx_n);
   fChain->SetBranchAddress("vx_nTracks", &vx_nTracks, &b_vx_nTracks);
  fChain->SetBranchAddress("mcevt_weight", &mcevt_weight, &b_mcevt_weight);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_CB_eta", &trig_EF_trigmuonef_track_CB_eta, &b_trig_EF_trigmuonef_track_CB_eta);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_CB_phi", &trig_EF_trigmuonef_track_CB_phi, &b_trig_EF_trigmuonef_track_CB_phi);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_CB_hasCB", &trig_EF_trigmuonef_track_CB_hasCB, &b_trig_EF_trigmuonef_track_CB_hasCB);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_CB_pt", &trig_EF_trigmuonef_track_CB_pt, &b_trig_EF_trigmuonef_track_CB_pt);
   fChain->SetBranchAddress("trig_EF_trigmuonef_track_n", &trig_EF_trigmuonef_track_n, &b_trig_EF_trigmuonef_track_n);
   fChain->SetBranchAddress("trig_EF_trigmuonef_n", &trig_EF_trigmuonef_n, &b_trig_EF_trigmuonef_n);

   Notify();
}

Bool_t Reco_dpd2::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Reco_dpd2::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Reco_dpd2::Cut(Long64_t /*entry*/)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Reco_dpd2_cxx
