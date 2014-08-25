/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Jan  9 00:12:15 2011 by ROOT version 5.26/00e
// from TTree qcd/qcd
// found on file: /atlaslocalgroupdisk/user/sresconi/data10_7TeV/user.sresconi.data10_7TeV.00152166.physics_L1Calo.merge.NTUP_JETMET.r1647_p306_p350.101222151102/user.sresconi.000342._00001.qcd.root
//////////////////////////////////////////////////////////

#ifndef D3PD_h
#define D3PD_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class D3PD {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   UInt_t          RunNumber;
   UInt_t          EventNumber;
   UInt_t          lbn;
   UInt_t          bcid;
   Bool_t          collcand_passMBTSTime;
   Bool_t          collcand_passCaloTime;
   Bool_t          collcand_passTrigger;
   Bool_t          collcand_pass;
   vector<float>   *beamSpot_x;
   vector<float>   *beamSpot_y;
   vector<float>   *beamSpot_z;
   Int_t           vxp_n;
   vector<float>   *vxp_x;
   vector<float>   *vxp_y;
   vector<float>   *vxp_z;
   vector<float>   *vxp_cov_x;
   vector<float>   *vxp_cov_y;
   vector<float>   *vxp_cov_z;
   vector<float>   *vxp_cov_xy;
   vector<float>   *vxp_cov_xz;
   vector<float>   *vxp_cov_yz;
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
   vector<vector<int> > *vxp_trk_index;
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
   vector<int>     *el_loose;
   vector<int>     *el_medium;
   vector<int>     *el_mediumIso;
   vector<int>     *el_tight;
   vector<int>     *el_tightIso;
   vector<float>   *el_Etcone45;
   vector<float>   *el_Etcone20;
   vector<float>   *el_Etcone30;
   vector<float>   *el_Etcone40;
   vector<float>   *el_Es2;
   vector<float>   *el_etas2;
   vector<float>   *el_phis2;
   vector<float>   *el_cl_E;
   vector<float>   *el_cl_pt;
   vector<float>   *el_cl_eta;
   vector<float>   *el_cl_phi;
   vector<float>   *el_trackd0;
   vector<float>   *el_trackz0;
   vector<float>   *el_trackphi;
   vector<float>   *el_tracktheta;
   vector<float>   *el_trackqoverp;
   vector<float>   *el_trackpt;
   vector<float>   *el_tracketa;
   vector<float>   *el_vertx;
   vector<float>   *el_verty;
   vector<float>   *el_vertz;
   Bool_t          EF_e10_loose;
   Bool_t          EF_e10_loose_passEF;
   Bool_t          EF_e10_loose_passL2;
   Bool_t          EF_e10_medium;
   Bool_t          EF_e20_loose_NoIDTrkCut;
   Bool_t          EF_mu10;
   Bool_t          EF_mu15;
   Bool_t          L1_EM10;
   Bool_t          L1_EM10I;
   Bool_t          L1_EM14;
   Bool_t          L1_MU10;
   Bool_t          L1_MU15;
   Bool_t          L1_MU20;
   Bool_t          L1_MU6;
   Bool_t          L1_XE10;
   Bool_t          L1_XE15;
   Bool_t          L1_XE20;
   Bool_t          L1_XE30;
   Int_t           mu_staco_n;
   vector<float>   *mu_staco_E;
   vector<float>   *mu_staco_px;
   vector<float>   *mu_staco_py;
   vector<float>   *mu_staco_pz;
   vector<float>   *mu_staco_charge;
   vector<int>     *mu_staco_author;
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
   vector<int>     *mu_staco_bestMatch;
   vector<int>     *mu_staco_isStandAloneMuon;
   vector<int>     *mu_staco_isCombinedMuon;
   vector<int>     *mu_staco_isLowPtReconstructedMuon;
   vector<float>   *mu_staco_d0_exPV;
   vector<float>   *mu_staco_z0_exPV;
   vector<float>   *mu_staco_ms_phi;
   vector<float>   *mu_staco_ms_theta;
   vector<float>   *mu_staco_ms_qoverp;
   vector<float>   *mu_staco_id_phi;
   vector<float>   *mu_staco_id_theta;
   vector<float>   *mu_staco_id_qoverp;
   Int_t           jet_AntiKt4LCTopo_n;
   vector<float>   *jet_AntiKt4LCTopo_E;
   vector<float>   *jet_AntiKt4LCTopo_pt;
   vector<float>   *jet_AntiKt4LCTopo_m;
   vector<float>   *jet_AntiKt4LCTopo_eta;
   vector<float>   *jet_AntiKt4LCTopo_phi;
   vector<float>   *jet_AntiKt4LCTopo_n90;
   vector<float>   *jet_AntiKt4LCTopo_Timing;
   vector<float>   *jet_AntiKt4LCTopo_LArQuality;
   vector<float>   *jet_AntiKt4LCTopo_HECQuality;
   vector<float>   *jet_AntiKt4LCTopo_BCH_CORR_CELL;
   vector<float>   *jet_AntiKt4LCTopo_BCH_CORR_DOTX;
   vector<int>     *jet_AntiKt4LCTopo_SamplingMax;
   vector<double>  *jet_AntiKt4LCTopo_fracSamplingMax;
   vector<float>   *jet_AntiKt4LCTopo_hecf;
   vector<float>   *jet_AntiKt4LCTopo_tgap3f;
   vector<float>   *jet_AntiKt4LCTopo_emfrac;
   vector<float>   *jet_AntiKt4LCTopo_EMJES;
   vector<float>   *jet_AntiKt4LCTopo_GCWJES;
   vector<float>   *jet_AntiKt4LCTopo_emscale_E;
   vector<float>   *jet_AntiKt4LCTopo_emscale_pt;
   vector<float>   *jet_AntiKt4LCTopo_emscale_m;
   vector<float>   *jet_AntiKt4LCTopo_emscale_eta;
   vector<float>   *jet_AntiKt4LCTopo_emscale_phi;
   Int_t           jet_AntiKt6LCTopo_n;
   vector<float>   *jet_AntiKt6LCTopo_E;
   vector<float>   *jet_AntiKt6LCTopo_pt;
   vector<float>   *jet_AntiKt6LCTopo_m;
   vector<float>   *jet_AntiKt6LCTopo_eta;
   vector<float>   *jet_AntiKt6LCTopo_phi;
   vector<float>   *jet_AntiKt6LCTopo_n90;
   vector<float>   *jet_AntiKt6LCTopo_Timing;
   vector<float>   *jet_AntiKt6LCTopo_LArQuality;
   vector<float>   *jet_AntiKt6LCTopo_HECQuality;
   vector<float>   *jet_AntiKt6LCTopo_BCH_CORR_CELL;
   vector<float>   *jet_AntiKt6LCTopo_BCH_CORR_DOTX;
   vector<int>     *jet_AntiKt6LCTopo_SamplingMax;
   vector<double>  *jet_AntiKt6LCTopo_fracSamplingMax;
   vector<float>   *jet_AntiKt6LCTopo_hecf;
   vector<float>   *jet_AntiKt6LCTopo_tgap3f;
   vector<float>   *jet_AntiKt6LCTopo_emfrac;
   vector<float>   *jet_AntiKt6LCTopo_EMJES;
   vector<float>   *jet_AntiKt6LCTopo_GCWJES;
   vector<float>   *jet_AntiKt6LCTopo_emscale_E;
   vector<float>   *jet_AntiKt6LCTopo_emscale_pt;
   vector<float>   *jet_AntiKt6LCTopo_emscale_m;
   vector<float>   *jet_AntiKt6LCTopo_emscale_eta;
   vector<float>   *jet_AntiKt6LCTopo_emscale_phi;
   Float_t         MET_RefFinal_etx;
   Float_t         MET_RefFinal_ety;
   Float_t         MET_RefFinal_phi;
   Float_t         MET_RefFinal_et;
   Float_t         MET_RefFinal_sumet;
   Float_t         MET_RefFinal_etx_CentralReg;
   Float_t         MET_RefFinal_ety_CentralReg;
   Float_t         MET_RefFinal_sumet_CentralReg;
   Float_t         MET_RefFinal_phi_CentralReg;
   Float_t         MET_RefFinal_etx_EndcapRegion;
   Float_t         MET_RefFinal_ety_EndcapRegion;
   Float_t         MET_RefFinal_sumet_EndcapRegion;
   Float_t         MET_RefFinal_phi_EndcapRegion;
   Float_t         MET_RefFinal_etx_ForwardReg;
   Float_t         MET_RefFinal_ety_ForwardReg;
   Float_t         MET_RefFinal_sumet_ForwardReg;
   Float_t         MET_RefFinal_phi_ForwardReg;
   Float_t         MET_Cryo_etx;
   Float_t         MET_Cryo_ety;
   Float_t         MET_Cryo_phi;
   Float_t         MET_Cryo_et;
   Float_t         MET_Cryo_sumet;
   Float_t         MET_Cryo_etx_CentralReg;
   Float_t         MET_Cryo_ety_CentralReg;
   Float_t         MET_Cryo_sumet_CentralReg;
   Float_t         MET_Cryo_phi_CentralReg;
   Float_t         MET_Cryo_etx_EndcapRegion;
   Float_t         MET_Cryo_ety_EndcapRegion;
   Float_t         MET_Cryo_sumet_EndcapRegion;
   Float_t         MET_Cryo_phi_EndcapRegion;
   Float_t         MET_Cryo_etx_ForwardReg;
   Float_t         MET_Cryo_ety_ForwardReg;
   Float_t         MET_Cryo_sumet_ForwardReg;
   Float_t         MET_Cryo_phi_ForwardReg;
   Float_t         MET_RefEle_etx;
   Float_t         MET_RefEle_ety;
   Float_t         MET_RefEle_phi;
   Float_t         MET_RefEle_et;
   Float_t         MET_RefEle_sumet;
   Float_t         MET_RefEle_etx_CentralReg;
   Float_t         MET_RefEle_ety_CentralReg;
   Float_t         MET_RefEle_sumet_CentralReg;
   Float_t         MET_RefEle_phi_CentralReg;
   Float_t         MET_RefEle_etx_EndcapRegion;
   Float_t         MET_RefEle_ety_EndcapRegion;
   Float_t         MET_RefEle_sumet_EndcapRegion;
   Float_t         MET_RefEle_phi_EndcapRegion;
   Float_t         MET_RefEle_etx_ForwardReg;
   Float_t         MET_RefEle_ety_ForwardReg;
   Float_t         MET_RefEle_sumet_ForwardReg;
   Float_t         MET_RefEle_phi_ForwardReg;
   Float_t         MET_RefJet_etx;
   Float_t         MET_RefJet_ety;
   Float_t         MET_RefJet_phi;
   Float_t         MET_RefJet_et;
   Float_t         MET_RefJet_sumet;
   Float_t         MET_RefJet_etx_CentralReg;
   Float_t         MET_RefJet_ety_CentralReg;
   Float_t         MET_RefJet_sumet_CentralReg;
   Float_t         MET_RefJet_phi_CentralReg;
   Float_t         MET_RefJet_etx_EndcapRegion;
   Float_t         MET_RefJet_ety_EndcapRegion;
   Float_t         MET_RefJet_sumet_EndcapRegion;
   Float_t         MET_RefJet_phi_EndcapRegion;
   Float_t         MET_RefJet_etx_ForwardReg;
   Float_t         MET_RefJet_ety_ForwardReg;
   Float_t         MET_RefJet_sumet_ForwardReg;
   Float_t         MET_RefJet_phi_ForwardReg;
   Float_t         MET_SoftJets_etx;
   Float_t         MET_SoftJets_ety;
   Float_t         MET_SoftJets_phi;
   Float_t         MET_SoftJets_et;
   Float_t         MET_SoftJets_sumet;
   Float_t         MET_SoftJets_etx_CentralReg;
   Float_t         MET_SoftJets_ety_CentralReg;
   Float_t         MET_SoftJets_sumet_CentralReg;
   Float_t         MET_SoftJets_phi_CentralReg;
   Float_t         MET_SoftJets_etx_EndcapRegion;
   Float_t         MET_SoftJets_ety_EndcapRegion;
   Float_t         MET_SoftJets_sumet_EndcapRegion;
   Float_t         MET_SoftJets_phi_EndcapRegion;
   Float_t         MET_SoftJets_etx_ForwardReg;
   Float_t         MET_SoftJets_ety_ForwardReg;
   Float_t         MET_SoftJets_sumet_ForwardReg;
   Float_t         MET_SoftJets_phi_ForwardReg;
   Float_t         MET_RefMuon_etx;
   Float_t         MET_RefMuon_ety;
   Float_t         MET_RefMuon_phi;
   Float_t         MET_RefMuon_et;
   Float_t         MET_RefMuon_sumet;
   Float_t         MET_RefMuon_etx_CentralReg;
   Float_t         MET_RefMuon_ety_CentralReg;
   Float_t         MET_RefMuon_sumet_CentralReg;
   Float_t         MET_RefMuon_phi_CentralReg;
   Float_t         MET_RefMuon_etx_EndcapRegion;
   Float_t         MET_RefMuon_ety_EndcapRegion;
   Float_t         MET_RefMuon_sumet_EndcapRegion;
   Float_t         MET_RefMuon_phi_EndcapRegion;
   Float_t         MET_RefMuon_etx_ForwardReg;
   Float_t         MET_RefMuon_ety_ForwardReg;
   Float_t         MET_RefMuon_sumet_ForwardReg;
   Float_t         MET_RefMuon_phi_ForwardReg;
   Float_t         MET_RefMuon_Staco_etx;
   Float_t         MET_RefMuon_Staco_ety;
   Float_t         MET_RefMuon_Staco_phi;
   Float_t         MET_RefMuon_Staco_et;
   Float_t         MET_RefMuon_Staco_sumet;
   Float_t         MET_RefMuon_Staco_etx_CentralReg;
   Float_t         MET_RefMuon_Staco_ety_CentralReg;
   Float_t         MET_RefMuon_Staco_sumet_CentralReg;
   Float_t         MET_RefMuon_Staco_phi_CentralReg;
   Float_t         MET_RefMuon_Staco_etx_EndcapRegion;
   Float_t         MET_RefMuon_Staco_ety_EndcapRegion;
   Float_t         MET_RefMuon_Staco_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Staco_phi_EndcapRegion;
   Float_t         MET_RefMuon_Staco_etx_ForwardReg;
   Float_t         MET_RefMuon_Staco_ety_ForwardReg;
   Float_t         MET_RefMuon_Staco_sumet_ForwardReg;
   Float_t         MET_RefMuon_Staco_phi_ForwardReg;
   Float_t         MET_RefMuon_Muid_etx;
   Float_t         MET_RefMuon_Muid_ety;
   Float_t         MET_RefMuon_Muid_phi;
   Float_t         MET_RefMuon_Muid_et;
   Float_t         MET_RefMuon_Muid_sumet;
   Float_t         MET_RefMuon_Muid_etx_CentralReg;
   Float_t         MET_RefMuon_Muid_ety_CentralReg;
   Float_t         MET_RefMuon_Muid_sumet_CentralReg;
   Float_t         MET_RefMuon_Muid_phi_CentralReg;
   Float_t         MET_RefMuon_Muid_etx_EndcapRegion;
   Float_t         MET_RefMuon_Muid_ety_EndcapRegion;
   Float_t         MET_RefMuon_Muid_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Muid_phi_EndcapRegion;
   Float_t         MET_RefMuon_Muid_etx_ForwardReg;
   Float_t         MET_RefMuon_Muid_ety_ForwardReg;
   Float_t         MET_RefMuon_Muid_sumet_ForwardReg;
   Float_t         MET_RefMuon_Muid_phi_ForwardReg;
   Float_t         MET_RefGamma_etx;
   Float_t         MET_RefGamma_ety;
   Float_t         MET_RefGamma_phi;
   Float_t         MET_RefGamma_et;
   Float_t         MET_RefGamma_sumet;
   Float_t         MET_RefGamma_etx_CentralReg;
   Float_t         MET_RefGamma_ety_CentralReg;
   Float_t         MET_RefGamma_sumet_CentralReg;
   Float_t         MET_RefGamma_phi_CentralReg;
   Float_t         MET_RefGamma_etx_EndcapRegion;
   Float_t         MET_RefGamma_ety_EndcapRegion;
   Float_t         MET_RefGamma_sumet_EndcapRegion;
   Float_t         MET_RefGamma_phi_EndcapRegion;
   Float_t         MET_RefGamma_etx_ForwardReg;
   Float_t         MET_RefGamma_ety_ForwardReg;
   Float_t         MET_RefGamma_sumet_ForwardReg;
   Float_t         MET_RefGamma_phi_ForwardReg;
   Float_t         MET_RefTau_etx;
   Float_t         MET_RefTau_ety;
   Float_t         MET_RefTau_phi;
   Float_t         MET_RefTau_et;
   Float_t         MET_RefTau_sumet;
   Float_t         MET_RefTau_etx_CentralReg;
   Float_t         MET_RefTau_ety_CentralReg;
   Float_t         MET_RefTau_sumet_CentralReg;
   Float_t         MET_RefTau_phi_CentralReg;
   Float_t         MET_RefTau_etx_EndcapRegion;
   Float_t         MET_RefTau_ety_EndcapRegion;
   Float_t         MET_RefTau_sumet_EndcapRegion;
   Float_t         MET_RefTau_phi_EndcapRegion;
   Float_t         MET_RefTau_etx_ForwardReg;
   Float_t         MET_RefTau_ety_ForwardReg;
   Float_t         MET_RefTau_sumet_ForwardReg;
   Float_t         MET_RefTau_phi_ForwardReg;
   Float_t         MET_CellOut_etx;
   Float_t         MET_CellOut_ety;
   Float_t         MET_CellOut_phi;
   Float_t         MET_CellOut_et;
   Float_t         MET_CellOut_sumet;
   Float_t         MET_CellOut_etx_CentralReg;
   Float_t         MET_CellOut_ety_CentralReg;
   Float_t         MET_CellOut_sumet_CentralReg;
   Float_t         MET_CellOut_phi_CentralReg;
   Float_t         MET_CellOut_etx_EndcapRegion;
   Float_t         MET_CellOut_ety_EndcapRegion;
   Float_t         MET_CellOut_sumet_EndcapRegion;
   Float_t         MET_CellOut_phi_EndcapRegion;
   Float_t         MET_CellOut_etx_ForwardReg;
   Float_t         MET_CellOut_ety_ForwardReg;
   Float_t         MET_CellOut_sumet_ForwardReg;
   Float_t         MET_CellOut_phi_ForwardReg;
   Float_t         MET_Track_etx;
   Float_t         MET_Track_ety;
   Float_t         MET_Track_phi;
   Float_t         MET_Track_et;
   Float_t         MET_Track_sumet;
   Float_t         MET_Track_etx_CentralReg;
   Float_t         MET_Track_ety_CentralReg;
   Float_t         MET_Track_sumet_CentralReg;
   Float_t         MET_Track_phi_CentralReg;
   Float_t         MET_Track_etx_EndcapRegion;
   Float_t         MET_Track_ety_EndcapRegion;
   Float_t         MET_Track_sumet_EndcapRegion;
   Float_t         MET_Track_phi_EndcapRegion;
   Float_t         MET_Track_etx_ForwardReg;
   Float_t         MET_Track_ety_ForwardReg;
   Float_t         MET_Track_sumet_ForwardReg;
   Float_t         MET_Track_phi_ForwardReg;
   Float_t         MET_RefFinal_em_etx;
   Float_t         MET_RefFinal_em_ety;
   Float_t         MET_RefFinal_em_phi;
   Float_t         MET_RefFinal_em_et;
   Float_t         MET_RefFinal_em_sumet;
   Float_t         MET_RefFinal_em_etx_CentralReg;
   Float_t         MET_RefFinal_em_ety_CentralReg;
   Float_t         MET_RefFinal_em_sumet_CentralReg;
   Float_t         MET_RefFinal_em_phi_CentralReg;
   Float_t         MET_RefFinal_em_etx_EndcapRegion;
   Float_t         MET_RefFinal_em_ety_EndcapRegion;
   Float_t         MET_RefFinal_em_sumet_EndcapRegion;
   Float_t         MET_RefFinal_em_phi_EndcapRegion;
   Float_t         MET_RefFinal_em_etx_ForwardReg;
   Float_t         MET_RefFinal_em_ety_ForwardReg;
   Float_t         MET_RefFinal_em_sumet_ForwardReg;
   Float_t         MET_RefFinal_em_phi_ForwardReg;
   Float_t         MET_RefEle_em_etx;
   Float_t         MET_RefEle_em_ety;
   Float_t         MET_RefEle_em_phi;
   Float_t         MET_RefEle_em_et;
   Float_t         MET_RefEle_em_sumet;
   Float_t         MET_RefEle_em_etx_CentralReg;
   Float_t         MET_RefEle_em_ety_CentralReg;
   Float_t         MET_RefEle_em_sumet_CentralReg;
   Float_t         MET_RefEle_em_phi_CentralReg;
   Float_t         MET_RefEle_em_etx_EndcapRegion;
   Float_t         MET_RefEle_em_ety_EndcapRegion;
   Float_t         MET_RefEle_em_sumet_EndcapRegion;
   Float_t         MET_RefEle_em_phi_EndcapRegion;
   Float_t         MET_RefEle_em_etx_ForwardReg;
   Float_t         MET_RefEle_em_ety_ForwardReg;
   Float_t         MET_RefEle_em_sumet_ForwardReg;
   Float_t         MET_RefEle_em_phi_ForwardReg;
   Float_t         MET_RefJet_em_etx;
   Float_t         MET_RefJet_em_ety;
   Float_t         MET_RefJet_em_phi;
   Float_t         MET_RefJet_em_et;
   Float_t         MET_RefJet_em_sumet;
   Float_t         MET_RefJet_em_etx_CentralReg;
   Float_t         MET_RefJet_em_ety_CentralReg;
   Float_t         MET_RefJet_em_sumet_CentralReg;
   Float_t         MET_RefJet_em_phi_CentralReg;
   Float_t         MET_RefJet_em_etx_EndcapRegion;
   Float_t         MET_RefJet_em_ety_EndcapRegion;
   Float_t         MET_RefJet_em_sumet_EndcapRegion;
   Float_t         MET_RefJet_em_phi_EndcapRegion;
   Float_t         MET_RefJet_em_etx_ForwardReg;
   Float_t         MET_RefJet_em_ety_ForwardReg;
   Float_t         MET_RefJet_em_sumet_ForwardReg;
   Float_t         MET_RefJet_em_phi_ForwardReg;
   Float_t         MET_SoftJets_em_etx;
   Float_t         MET_SoftJets_em_ety;
   Float_t         MET_SoftJets_em_phi;
   Float_t         MET_SoftJets_em_et;
   Float_t         MET_SoftJets_em_sumet;
   Float_t         MET_SoftJets_em_etx_CentralReg;
   Float_t         MET_SoftJets_em_ety_CentralReg;
   Float_t         MET_SoftJets_em_sumet_CentralReg;
   Float_t         MET_SoftJets_em_phi_CentralReg;
   Float_t         MET_SoftJets_em_etx_EndcapRegion;
   Float_t         MET_SoftJets_em_ety_EndcapRegion;
   Float_t         MET_SoftJets_em_sumet_EndcapRegion;
   Float_t         MET_SoftJets_em_phi_EndcapRegion;
   Float_t         MET_SoftJets_em_etx_ForwardReg;
   Float_t         MET_SoftJets_em_ety_ForwardReg;
   Float_t         MET_SoftJets_em_sumet_ForwardReg;
   Float_t         MET_SoftJets_em_phi_ForwardReg;
   Float_t         MET_RefMuon_em_etx;
   Float_t         MET_RefMuon_em_ety;
   Float_t         MET_RefMuon_em_phi;
   Float_t         MET_RefMuon_em_et;
   Float_t         MET_RefMuon_em_sumet;
   Float_t         MET_RefMuon_em_etx_CentralReg;
   Float_t         MET_RefMuon_em_ety_CentralReg;
   Float_t         MET_RefMuon_em_sumet_CentralReg;
   Float_t         MET_RefMuon_em_phi_CentralReg;
   Float_t         MET_RefMuon_em_etx_EndcapRegion;
   Float_t         MET_RefMuon_em_ety_EndcapRegion;
   Float_t         MET_RefMuon_em_sumet_EndcapRegion;
   Float_t         MET_RefMuon_em_phi_EndcapRegion;
   Float_t         MET_RefMuon_em_etx_ForwardReg;
   Float_t         MET_RefMuon_em_ety_ForwardReg;
   Float_t         MET_RefMuon_em_sumet_ForwardReg;
   Float_t         MET_RefMuon_em_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_em_etx;
   Float_t         MET_RefMuon_Track_em_ety;
   Float_t         MET_RefMuon_Track_em_phi;
   Float_t         MET_RefMuon_Track_em_et;
   Float_t         MET_RefMuon_Track_em_sumet;
   Float_t         MET_RefMuon_Track_em_etx_CentralReg;
   Float_t         MET_RefMuon_Track_em_ety_CentralReg;
   Float_t         MET_RefMuon_Track_em_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_em_phi_CentralReg;
   Float_t         MET_RefMuon_Track_em_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_em_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_em_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_em_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_em_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_em_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_em_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_em_phi_ForwardReg;
   Float_t         MET_RefGamma_em_etx;
   Float_t         MET_RefGamma_em_ety;
   Float_t         MET_RefGamma_em_phi;
   Float_t         MET_RefGamma_em_et;
   Float_t         MET_RefGamma_em_sumet;
   Float_t         MET_RefGamma_em_etx_CentralReg;
   Float_t         MET_RefGamma_em_ety_CentralReg;
   Float_t         MET_RefGamma_em_sumet_CentralReg;
   Float_t         MET_RefGamma_em_phi_CentralReg;
   Float_t         MET_RefGamma_em_etx_EndcapRegion;
   Float_t         MET_RefGamma_em_ety_EndcapRegion;
   Float_t         MET_RefGamma_em_sumet_EndcapRegion;
   Float_t         MET_RefGamma_em_phi_EndcapRegion;
   Float_t         MET_RefGamma_em_etx_ForwardReg;
   Float_t         MET_RefGamma_em_ety_ForwardReg;
   Float_t         MET_RefGamma_em_sumet_ForwardReg;
   Float_t         MET_RefGamma_em_phi_ForwardReg;
   Float_t         MET_RefTau_em_etx;
   Float_t         MET_RefTau_em_ety;
   Float_t         MET_RefTau_em_phi;
   Float_t         MET_RefTau_em_et;
   Float_t         MET_RefTau_em_sumet;
   Float_t         MET_RefTau_em_etx_CentralReg;
   Float_t         MET_RefTau_em_ety_CentralReg;
   Float_t         MET_RefTau_em_sumet_CentralReg;
   Float_t         MET_RefTau_em_phi_CentralReg;
   Float_t         MET_RefTau_em_etx_EndcapRegion;
   Float_t         MET_RefTau_em_ety_EndcapRegion;
   Float_t         MET_RefTau_em_sumet_EndcapRegion;
   Float_t         MET_RefTau_em_phi_EndcapRegion;
   Float_t         MET_RefTau_em_etx_ForwardReg;
   Float_t         MET_RefTau_em_ety_ForwardReg;
   Float_t         MET_RefTau_em_sumet_ForwardReg;
   Float_t         MET_RefTau_em_phi_ForwardReg;
   Float_t         MET_CellOut_em_etx;
   Float_t         MET_CellOut_em_ety;
   Float_t         MET_CellOut_em_phi;
   Float_t         MET_CellOut_em_et;
   Float_t         MET_CellOut_em_sumet;
   Float_t         MET_CellOut_em_etx_CentralReg;
   Float_t         MET_CellOut_em_ety_CentralReg;
   Float_t         MET_CellOut_em_sumet_CentralReg;
   Float_t         MET_CellOut_em_phi_CentralReg;
   Float_t         MET_CellOut_em_etx_EndcapRegion;
   Float_t         MET_CellOut_em_ety_EndcapRegion;
   Float_t         MET_CellOut_em_sumet_EndcapRegion;
   Float_t         MET_CellOut_em_phi_EndcapRegion;
   Float_t         MET_CellOut_em_etx_ForwardReg;
   Float_t         MET_CellOut_em_ety_ForwardReg;
   Float_t         MET_CellOut_em_sumet_ForwardReg;
   Float_t         MET_CellOut_em_phi_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_etx;
   Float_t         MET_Muon_Isol_Staco_ety;
   Float_t         MET_Muon_Isol_Staco_phi;
   Float_t         MET_Muon_Isol_Staco_et;
   Float_t         MET_Muon_Isol_Staco_sumet;
   Float_t         MET_Muon_Isol_Staco_etx_CentralReg;
   Float_t         MET_Muon_Isol_Staco_ety_CentralReg;
   Float_t         MET_Muon_Isol_Staco_sumet_CentralReg;
   Float_t         MET_Muon_Isol_Staco_phi_CentralReg;
   Float_t         MET_Muon_Isol_Staco_etx_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_ety_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_sumet_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_phi_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_etx_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_ety_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_sumet_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_phi_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_etx;
   Float_t         MET_Muon_NonIsol_Staco_ety;
   Float_t         MET_Muon_NonIsol_Staco_phi;
   Float_t         MET_Muon_NonIsol_Staco_et;
   Float_t         MET_Muon_NonIsol_Staco_sumet;
   Float_t         MET_Muon_NonIsol_Staco_etx_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_ety_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_sumet_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_phi_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_etx_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_ety_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_sumet_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_phi_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_etx_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_ety_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_sumet_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_phi_ForwardReg;
   Float_t         MET_Muon_Total_Staco_etx;
   Float_t         MET_Muon_Total_Staco_ety;
   Float_t         MET_Muon_Total_Staco_phi;
   Float_t         MET_Muon_Total_Staco_et;
   Float_t         MET_Muon_Total_Staco_sumet;
   Float_t         MET_Muon_Total_Staco_etx_CentralReg;
   Float_t         MET_Muon_Total_Staco_ety_CentralReg;
   Float_t         MET_Muon_Total_Staco_sumet_CentralReg;
   Float_t         MET_Muon_Total_Staco_phi_CentralReg;
   Float_t         MET_Muon_Total_Staco_etx_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_ety_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_sumet_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_phi_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_etx_ForwardReg;
   Float_t         MET_Muon_Total_Staco_ety_ForwardReg;
   Float_t         MET_Muon_Total_Staco_sumet_ForwardReg;
   Float_t         MET_Muon_Total_Staco_phi_ForwardReg;
   Float_t         MET_Muon_Isol_Muid_etx;
   Float_t         MET_Muon_Isol_Muid_ety;
   Float_t         MET_Muon_Isol_Muid_phi;
   Float_t         MET_Muon_Isol_Muid_et;
   Float_t         MET_Muon_Isol_Muid_sumet;
   Float_t         MET_Muon_Isol_Muid_etx_CentralReg;
   Float_t         MET_Muon_Isol_Muid_ety_CentralReg;
   Float_t         MET_Muon_Isol_Muid_sumet_CentralReg;
   Float_t         MET_Muon_Isol_Muid_phi_CentralReg;
   Float_t         MET_Muon_Isol_Muid_etx_EndcapRegion;
   Float_t         MET_Muon_Isol_Muid_ety_EndcapRegion;
   Float_t         MET_Muon_Isol_Muid_sumet_EndcapRegion;
   Float_t         MET_Muon_Isol_Muid_phi_EndcapRegion;
   Float_t         MET_Muon_Isol_Muid_etx_ForwardReg;
   Float_t         MET_Muon_Isol_Muid_ety_ForwardReg;
   Float_t         MET_Muon_Isol_Muid_sumet_ForwardReg;
   Float_t         MET_Muon_Isol_Muid_phi_ForwardReg;
   Float_t         MET_Muon_NonIsol_Muid_etx;
   Float_t         MET_Muon_NonIsol_Muid_ety;
   Float_t         MET_Muon_NonIsol_Muid_phi;
   Float_t         MET_Muon_NonIsol_Muid_et;
   Float_t         MET_Muon_NonIsol_Muid_sumet;
   Float_t         MET_Muon_NonIsol_Muid_etx_CentralReg;
   Float_t         MET_Muon_NonIsol_Muid_ety_CentralReg;
   Float_t         MET_Muon_NonIsol_Muid_sumet_CentralReg;
   Float_t         MET_Muon_NonIsol_Muid_phi_CentralReg;
   Float_t         MET_Muon_NonIsol_Muid_etx_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Muid_ety_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Muid_sumet_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Muid_phi_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Muid_etx_ForwardReg;
   Float_t         MET_Muon_NonIsol_Muid_ety_ForwardReg;
   Float_t         MET_Muon_NonIsol_Muid_sumet_ForwardReg;
   Float_t         MET_Muon_NonIsol_Muid_phi_ForwardReg;
   Float_t         MET_Muon_Total_Muid_etx;
   Float_t         MET_Muon_Total_Muid_ety;
   Float_t         MET_Muon_Total_Muid_phi;
   Float_t         MET_Muon_Total_Muid_et;
   Float_t         MET_Muon_Total_Muid_sumet;
   Float_t         MET_Muon_Total_Muid_etx_CentralReg;
   Float_t         MET_Muon_Total_Muid_ety_CentralReg;
   Float_t         MET_Muon_Total_Muid_sumet_CentralReg;
   Float_t         MET_Muon_Total_Muid_phi_CentralReg;
   Float_t         MET_Muon_Total_Muid_etx_EndcapRegion;
   Float_t         MET_Muon_Total_Muid_ety_EndcapRegion;
   Float_t         MET_Muon_Total_Muid_sumet_EndcapRegion;
   Float_t         MET_Muon_Total_Muid_phi_EndcapRegion;
   Float_t         MET_Muon_Total_Muid_etx_ForwardReg;
   Float_t         MET_Muon_Total_Muid_ety_ForwardReg;
   Float_t         MET_Muon_Total_Muid_sumet_ForwardReg;
   Float_t         MET_Muon_Total_Muid_phi_ForwardReg;
   Float_t         MET_MuonBoy_etx;
   Float_t         MET_MuonBoy_ety;
   Float_t         MET_MuonBoy_phi;
   Float_t         MET_MuonBoy_et;
   Float_t         MET_MuonBoy_sumet;
   Float_t         MET_MuonBoy_etx_CentralReg;
   Float_t         MET_MuonBoy_ety_CentralReg;
   Float_t         MET_MuonBoy_sumet_CentralReg;
   Float_t         MET_MuonBoy_phi_CentralReg;
   Float_t         MET_MuonBoy_etx_EndcapRegion;
   Float_t         MET_MuonBoy_ety_EndcapRegion;
   Float_t         MET_MuonBoy_sumet_EndcapRegion;
   Float_t         MET_MuonBoy_phi_EndcapRegion;
   Float_t         MET_MuonBoy_etx_ForwardReg;
   Float_t         MET_MuonBoy_ety_ForwardReg;
   Float_t         MET_MuonBoy_sumet_ForwardReg;
   Float_t         MET_MuonBoy_phi_ForwardReg;
   Float_t         MET_CellOut_Eflow_etx;
   Float_t         MET_CellOut_Eflow_ety;
   Float_t         MET_CellOut_Eflow_phi;
   Float_t         MET_CellOut_Eflow_et;
   Float_t         MET_CellOut_Eflow_sumet;
   Float_t         MET_CellOut_Eflow_etx_CentralReg;
   Float_t         MET_CellOut_Eflow_ety_CentralReg;
   Float_t         MET_CellOut_Eflow_sumet_CentralReg;
   Float_t         MET_CellOut_Eflow_phi_CentralReg;
   Float_t         MET_CellOut_Eflow_etx_EndcapRegion;
   Float_t         MET_CellOut_Eflow_ety_EndcapRegion;
   Float_t         MET_CellOut_Eflow_sumet_EndcapRegion;
   Float_t         MET_CellOut_Eflow_phi_EndcapRegion;
   Float_t         MET_CellOut_Eflow_etx_ForwardReg;
   Float_t         MET_CellOut_Eflow_ety_ForwardReg;
   Float_t         MET_CellOut_Eflow_sumet_ForwardReg;
   Float_t         MET_CellOut_Eflow_phi_ForwardReg;
   Float_t         MET_CellOut_MiniJet_etx;
   Float_t         MET_CellOut_MiniJet_ety;
   Float_t         MET_CellOut_MiniJet_phi;
   Float_t         MET_CellOut_MiniJet_et;
   Float_t         MET_CellOut_MiniJet_sumet;
   Float_t         MET_CellOut_MiniJet_etx_CentralReg;
   Float_t         MET_CellOut_MiniJet_ety_CentralReg;
   Float_t         MET_CellOut_MiniJet_sumet_CentralReg;
   Float_t         MET_CellOut_MiniJet_phi_CentralReg;
   Float_t         MET_CellOut_MiniJet_etx_EndcapRegion;
   Float_t         MET_CellOut_MiniJet_ety_EndcapRegion;
   Float_t         MET_CellOut_MiniJet_sumet_EndcapRegion;
   Float_t         MET_CellOut_MiniJet_phi_EndcapRegion;
   Float_t         MET_CellOut_MiniJet_etx_ForwardReg;
   Float_t         MET_CellOut_MiniJet_ety_ForwardReg;
   Float_t         MET_CellOut_MiniJet_sumet_ForwardReg;
   Float_t         MET_CellOut_MiniJet_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_etx;
   Float_t         MET_RefMuon_Track_ety;
   Float_t         MET_RefMuon_Track_phi;
   Float_t         MET_RefMuon_Track_et;
   Float_t         MET_RefMuon_Track_sumet;
   Float_t         MET_RefMuon_Track_etx_CentralReg;
   Float_t         MET_RefMuon_Track_ety_CentralReg;
   Float_t         MET_RefMuon_Track_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_phi_CentralReg;
   Float_t         MET_RefMuon_Track_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_etx;
   Float_t         MET_RefMuon_Track_Staco_ety;
   Float_t         MET_RefMuon_Track_Staco_phi;
   Float_t         MET_RefMuon_Track_Staco_et;
   Float_t         MET_RefMuon_Track_Staco_sumet;
   Float_t         MET_RefMuon_Track_Staco_etx_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_ety_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_phi_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_Muid_etx;
   Float_t         MET_RefMuon_Track_Muid_ety;
   Float_t         MET_RefMuon_Track_Muid_phi;
   Float_t         MET_RefMuon_Track_Muid_et;
   Float_t         MET_RefMuon_Track_Muid_sumet;
   Float_t         MET_RefMuon_Track_Muid_etx_CentralReg;
   Float_t         MET_RefMuon_Track_Muid_ety_CentralReg;
   Float_t         MET_RefMuon_Track_Muid_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_Muid_phi_CentralReg;
   Float_t         MET_RefMuon_Track_Muid_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_Muid_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_Muid_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_Muid_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_Muid_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_Muid_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_Muid_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_Muid_phi_ForwardReg;
   Float_t         MET_CryoCone_etx;
   Float_t         MET_CryoCone_ety;
   Float_t         MET_CryoCone_phi;
   Float_t         MET_CryoCone_et;
   Float_t         MET_CryoCone_sumet;
   Float_t         MET_CryoCone_etx_CentralReg;
   Float_t         MET_CryoCone_ety_CentralReg;
   Float_t         MET_CryoCone_sumet_CentralReg;
   Float_t         MET_CryoCone_phi_CentralReg;
   Float_t         MET_CryoCone_etx_EndcapRegion;
   Float_t         MET_CryoCone_ety_EndcapRegion;
   Float_t         MET_CryoCone_sumet_EndcapRegion;
   Float_t         MET_CryoCone_phi_EndcapRegion;
   Float_t         MET_CryoCone_etx_ForwardReg;
   Float_t         MET_CryoCone_ety_ForwardReg;
   Float_t         MET_CryoCone_sumet_ForwardReg;
   Float_t         MET_CryoCone_phi_ForwardReg;
   Float_t         MET_Final_etx;
   Float_t         MET_Final_ety;
   Float_t         MET_Final_phi;
   Float_t         MET_Final_et;
   Float_t         MET_Final_sumet;
   Float_t         MET_Final_etx_CentralReg;
   Float_t         MET_Final_ety_CentralReg;
   Float_t         MET_Final_sumet_CentralReg;
   Float_t         MET_Final_phi_CentralReg;
   Float_t         MET_Final_etx_EndcapRegion;
   Float_t         MET_Final_ety_EndcapRegion;
   Float_t         MET_Final_sumet_EndcapRegion;
   Float_t         MET_Final_phi_EndcapRegion;
   Float_t         MET_Final_etx_ForwardReg;
   Float_t         MET_Final_ety_ForwardReg;
   Float_t         MET_Final_sumet_ForwardReg;
   Float_t         MET_Final_phi_ForwardReg;
   Float_t         MET_MuonBoy_Spectro_etx;
   Float_t         MET_MuonBoy_Spectro_ety;
   Float_t         MET_MuonBoy_Spectro_phi;
   Float_t         MET_MuonBoy_Spectro_et;
   Float_t         MET_MuonBoy_Spectro_sumet;
   Float_t         MET_MuonBoy_Spectro_etx_CentralReg;
   Float_t         MET_MuonBoy_Spectro_ety_CentralReg;
   Float_t         MET_MuonBoy_Spectro_sumet_CentralReg;
   Float_t         MET_MuonBoy_Spectro_phi_CentralReg;
   Float_t         MET_MuonBoy_Spectro_etx_EndcapRegion;
   Float_t         MET_MuonBoy_Spectro_ety_EndcapRegion;
   Float_t         MET_MuonBoy_Spectro_sumet_EndcapRegion;
   Float_t         MET_MuonBoy_Spectro_phi_EndcapRegion;
   Float_t         MET_MuonBoy_Spectro_etx_ForwardReg;
   Float_t         MET_MuonBoy_Spectro_ety_ForwardReg;
   Float_t         MET_MuonBoy_Spectro_sumet_ForwardReg;
   Float_t         MET_MuonBoy_Spectro_phi_ForwardReg;
   Float_t         MET_MuonBoy_Track_etx;
   Float_t         MET_MuonBoy_Track_ety;
   Float_t         MET_MuonBoy_Track_phi;
   Float_t         MET_MuonBoy_Track_et;
   Float_t         MET_MuonBoy_Track_sumet;
   Float_t         MET_MuonBoy_Track_etx_CentralReg;
   Float_t         MET_MuonBoy_Track_ety_CentralReg;
   Float_t         MET_MuonBoy_Track_sumet_CentralReg;
   Float_t         MET_MuonBoy_Track_phi_CentralReg;
   Float_t         MET_MuonBoy_Track_etx_EndcapRegion;
   Float_t         MET_MuonBoy_Track_ety_EndcapRegion;
   Float_t         MET_MuonBoy_Track_sumet_EndcapRegion;
   Float_t         MET_MuonBoy_Track_phi_EndcapRegion;
   Float_t         MET_MuonBoy_Track_etx_ForwardReg;
   Float_t         MET_MuonBoy_Track_ety_ForwardReg;
   Float_t         MET_MuonBoy_Track_sumet_ForwardReg;
   Float_t         MET_MuonBoy_Track_phi_ForwardReg;
   Float_t         MET_MuonMuid_etx;
   Float_t         MET_MuonMuid_ety;
   Float_t         MET_MuonMuid_phi;
   Float_t         MET_MuonMuid_et;
   Float_t         MET_MuonMuid_sumet;
   Float_t         MET_MuonMuid_etx_CentralReg;
   Float_t         MET_MuonMuid_ety_CentralReg;
   Float_t         MET_MuonMuid_sumet_CentralReg;
   Float_t         MET_MuonMuid_phi_CentralReg;
   Float_t         MET_MuonMuid_etx_EndcapRegion;
   Float_t         MET_MuonMuid_ety_EndcapRegion;
   Float_t         MET_MuonMuid_sumet_EndcapRegion;
   Float_t         MET_MuonMuid_phi_EndcapRegion;
   Float_t         MET_MuonMuid_etx_ForwardReg;
   Float_t         MET_MuonMuid_ety_ForwardReg;
   Float_t         MET_MuonMuid_sumet_ForwardReg;
   Float_t         MET_MuonMuid_phi_ForwardReg;
   Float_t         MET_Muid_etx;
   Float_t         MET_Muid_ety;
   Float_t         MET_Muid_phi;
   Float_t         MET_Muid_et;
   Float_t         MET_Muid_sumet;
   Float_t         MET_Muid_etx_CentralReg;
   Float_t         MET_Muid_ety_CentralReg;
   Float_t         MET_Muid_sumet_CentralReg;
   Float_t         MET_Muid_phi_CentralReg;
   Float_t         MET_Muid_etx_EndcapRegion;
   Float_t         MET_Muid_ety_EndcapRegion;
   Float_t         MET_Muid_sumet_EndcapRegion;
   Float_t         MET_Muid_phi_EndcapRegion;
   Float_t         MET_Muid_etx_ForwardReg;
   Float_t         MET_Muid_ety_ForwardReg;
   Float_t         MET_Muid_sumet_ForwardReg;
   Float_t         MET_Muid_phi_ForwardReg;
   Float_t         MET_Muid_Spectro_etx;
   Float_t         MET_Muid_Spectro_ety;
   Float_t         MET_Muid_Spectro_phi;
   Float_t         MET_Muid_Spectro_et;
   Float_t         MET_Muid_Spectro_sumet;
   Float_t         MET_Muid_Spectro_etx_CentralReg;
   Float_t         MET_Muid_Spectro_ety_CentralReg;
   Float_t         MET_Muid_Spectro_sumet_CentralReg;
   Float_t         MET_Muid_Spectro_phi_CentralReg;
   Float_t         MET_Muid_Spectro_etx_EndcapRegion;
   Float_t         MET_Muid_Spectro_ety_EndcapRegion;
   Float_t         MET_Muid_Spectro_sumet_EndcapRegion;
   Float_t         MET_Muid_Spectro_phi_EndcapRegion;
   Float_t         MET_Muid_Spectro_etx_ForwardReg;
   Float_t         MET_Muid_Spectro_ety_ForwardReg;
   Float_t         MET_Muid_Spectro_sumet_ForwardReg;
   Float_t         MET_Muid_Spectro_phi_ForwardReg;
   Float_t         MET_Muid_Track_etx;
   Float_t         MET_Muid_Track_ety;
   Float_t         MET_Muid_Track_phi;
   Float_t         MET_Muid_Track_et;
   Float_t         MET_Muid_Track_sumet;
   Float_t         MET_Muid_Track_etx_CentralReg;
   Float_t         MET_Muid_Track_ety_CentralReg;
   Float_t         MET_Muid_Track_sumet_CentralReg;
   Float_t         MET_Muid_Track_phi_CentralReg;
   Float_t         MET_Muid_Track_etx_EndcapRegion;
   Float_t         MET_Muid_Track_ety_EndcapRegion;
   Float_t         MET_Muid_Track_sumet_EndcapRegion;
   Float_t         MET_Muid_Track_phi_EndcapRegion;
   Float_t         MET_Muid_Track_etx_ForwardReg;
   Float_t         MET_Muid_Track_ety_ForwardReg;
   Float_t         MET_Muid_Track_sumet_ForwardReg;
   Float_t         MET_Muid_Track_phi_ForwardReg;
   Float_t         MET_Muon_etx;
   Float_t         MET_Muon_ety;
   Float_t         MET_Muon_phi;
   Float_t         MET_Muon_et;
   Float_t         MET_Muon_sumet;
   Float_t         MET_Muon_etx_CentralReg;
   Float_t         MET_Muon_ety_CentralReg;
   Float_t         MET_Muon_sumet_CentralReg;
   Float_t         MET_Muon_phi_CentralReg;
   Float_t         MET_Muon_etx_EndcapRegion;
   Float_t         MET_Muon_ety_EndcapRegion;
   Float_t         MET_Muon_sumet_EndcapRegion;
   Float_t         MET_Muon_phi_EndcapRegion;
   Float_t         MET_Muon_etx_ForwardReg;
   Float_t         MET_Muon_ety_ForwardReg;
   Float_t         MET_Muon_sumet_ForwardReg;
   Float_t         MET_Muon_phi_ForwardReg;
   Float_t         MET_TopoObj_etx;
   Float_t         MET_TopoObj_ety;
   Float_t         MET_TopoObj_phi;
   Float_t         MET_TopoObj_et;
   Float_t         MET_TopoObj_sumet;
   Float_t         MET_TopoObj_etx_CentralReg;
   Float_t         MET_TopoObj_ety_CentralReg;
   Float_t         MET_TopoObj_sumet_CentralReg;
   Float_t         MET_TopoObj_phi_CentralReg;
   Float_t         MET_TopoObj_etx_EndcapRegion;
   Float_t         MET_TopoObj_ety_EndcapRegion;
   Float_t         MET_TopoObj_sumet_EndcapRegion;
   Float_t         MET_TopoObj_phi_EndcapRegion;
   Float_t         MET_TopoObj_etx_ForwardReg;
   Float_t         MET_TopoObj_ety_ForwardReg;
   Float_t         MET_TopoObj_sumet_ForwardReg;
   Float_t         MET_TopoObj_phi_ForwardReg;
   Float_t         MET_LocHadTopoObj_etx;
   Float_t         MET_LocHadTopoObj_ety;
   Float_t         MET_LocHadTopoObj_phi;
   Float_t         MET_LocHadTopoObj_et;
   Float_t         MET_LocHadTopoObj_sumet;
   Float_t         MET_LocHadTopoObj_etx_CentralReg;
   Float_t         MET_LocHadTopoObj_ety_CentralReg;
   Float_t         MET_LocHadTopoObj_sumet_CentralReg;
   Float_t         MET_LocHadTopoObj_phi_CentralReg;
   Float_t         MET_LocHadTopoObj_etx_EndcapRegion;
   Float_t         MET_LocHadTopoObj_ety_EndcapRegion;
   Float_t         MET_LocHadTopoObj_sumet_EndcapRegion;
   Float_t         MET_LocHadTopoObj_phi_EndcapRegion;
   Float_t         MET_LocHadTopoObj_etx_ForwardReg;
   Float_t         MET_LocHadTopoObj_ety_ForwardReg;
   Float_t         MET_LocHadTopoObj_sumet_ForwardReg;
   Float_t         MET_LocHadTopoObj_phi_ForwardReg;
   Float_t         MET_Topo_etx;
   Float_t         MET_Topo_ety;
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
   Float_t         MET_CorrTopo_etx;
   Float_t         MET_CorrTopo_ety;
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
   Float_t         MET_LocHadTopo_etx;
   Float_t         MET_LocHadTopo_ety;
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
   Float_t         MET_RefFinal_GCW_pt20_etx;
   Float_t         MET_RefFinal_GCW_pt20_ety;
   Float_t         MET_RefFinal_GCW_pt20_phi;
   Float_t         MET_RefFinal_GCW_pt20_et;
   Float_t         MET_RefFinal_GCW_pt20_sumet;
   Float_t         MET_RefFinal_GCW_pt20_etx_CentralReg;
   Float_t         MET_RefFinal_GCW_pt20_ety_CentralReg;
   Float_t         MET_RefFinal_GCW_pt20_sumet_CentralReg;
   Float_t         MET_RefFinal_GCW_pt20_phi_CentralReg;
   Float_t         MET_RefFinal_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefFinal_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefFinal_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefFinal_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefFinal_GCW_pt20_etx_ForwardReg;
   Float_t         MET_RefFinal_GCW_pt20_ety_ForwardReg;
   Float_t         MET_RefFinal_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefFinal_GCW_pt20_phi_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow4_etx;
   Float_t         MET_RefFinal_LCW_NI_eflow4_ety;
   Float_t         MET_RefFinal_LCW_NI_eflow4_phi;
   Float_t         MET_RefFinal_LCW_NI_eflow4_et;
   Float_t         MET_RefFinal_LCW_NI_eflow4_sumet;
   Float_t         MET_RefFinal_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_etx;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_ety;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_phi;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_et;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_sumet;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_etx;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_ety;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_phi;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_et;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_sumet;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_RefFinal_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_RefGamma_GCW_pt20_etx;
   Float_t         MET_RefGamma_GCW_pt20_ety;
   Float_t         MET_RefGamma_GCW_pt20_phi;
   Float_t         MET_RefGamma_GCW_pt20_et;
   Float_t         MET_RefGamma_GCW_pt20_sumet;
   Float_t         MET_RefGamma_GCW_pt20_etx_CentralReg;
   Float_t         MET_RefGamma_GCW_pt20_ety_CentralReg;
   Float_t         MET_RefGamma_GCW_pt20_sumet_CentralReg;
   Float_t         MET_RefGamma_GCW_pt20_phi_CentralReg;
   Float_t         MET_RefGamma_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefGamma_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefGamma_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefGamma_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefGamma_GCW_pt20_etx_ForwardReg;
   Float_t         MET_RefGamma_GCW_pt20_ety_ForwardReg;
   Float_t         MET_RefGamma_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefGamma_GCW_pt20_phi_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow4_etx;
   Float_t         MET_RefGamma_LCW_NI_eflow4_ety;
   Float_t         MET_RefGamma_LCW_NI_eflow4_phi;
   Float_t         MET_RefGamma_LCW_NI_eflow4_et;
   Float_t         MET_RefGamma_LCW_NI_eflow4_sumet;
   Float_t         MET_RefGamma_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_etx;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_ety;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_phi;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_et;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_sumet;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_etx;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_ety;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_phi;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_et;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_sumet;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_RefGamma_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_RefEle_GCW_pt20_etx;
   Float_t         MET_RefEle_GCW_pt20_ety;
   Float_t         MET_RefEle_GCW_pt20_phi;
   Float_t         MET_RefEle_GCW_pt20_et;
   Float_t         MET_RefEle_GCW_pt20_sumet;
   Float_t         MET_RefEle_GCW_pt20_etx_CentralReg;
   Float_t         MET_RefEle_GCW_pt20_ety_CentralReg;
   Float_t         MET_RefEle_GCW_pt20_sumet_CentralReg;
   Float_t         MET_RefEle_GCW_pt20_phi_CentralReg;
   Float_t         MET_RefEle_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefEle_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefEle_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefEle_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefEle_GCW_pt20_etx_ForwardReg;
   Float_t         MET_RefEle_GCW_pt20_ety_ForwardReg;
   Float_t         MET_RefEle_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefEle_GCW_pt20_phi_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow4_etx;
   Float_t         MET_RefEle_LCW_NI_eflow4_ety;
   Float_t         MET_RefEle_LCW_NI_eflow4_phi;
   Float_t         MET_RefEle_LCW_NI_eflow4_et;
   Float_t         MET_RefEle_LCW_NI_eflow4_sumet;
   Float_t         MET_RefEle_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_etx;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_ety;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_phi;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_et;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_sumet;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow_400_etx;
   Float_t         MET_RefEle_LCW_NI_eflow_400_ety;
   Float_t         MET_RefEle_LCW_NI_eflow_400_phi;
   Float_t         MET_RefEle_LCW_NI_eflow_400_et;
   Float_t         MET_RefEle_LCW_NI_eflow_400_sumet;
   Float_t         MET_RefEle_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_RefEle_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_RefEle_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_RefEle_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_RefTau_GCW_pt20_etx;
   Float_t         MET_RefTau_GCW_pt20_ety;
   Float_t         MET_RefTau_GCW_pt20_phi;
   Float_t         MET_RefTau_GCW_pt20_et;
   Float_t         MET_RefTau_GCW_pt20_sumet;
   Float_t         MET_RefTau_GCW_pt20_etx_CentralReg;
   Float_t         MET_RefTau_GCW_pt20_ety_CentralReg;
   Float_t         MET_RefTau_GCW_pt20_sumet_CentralReg;
   Float_t         MET_RefTau_GCW_pt20_phi_CentralReg;
   Float_t         MET_RefTau_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefTau_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefTau_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefTau_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefTau_GCW_pt20_etx_ForwardReg;
   Float_t         MET_RefTau_GCW_pt20_ety_ForwardReg;
   Float_t         MET_RefTau_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefTau_GCW_pt20_phi_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow4_etx;
   Float_t         MET_RefTau_LCW_NI_eflow4_ety;
   Float_t         MET_RefTau_LCW_NI_eflow4_phi;
   Float_t         MET_RefTau_LCW_NI_eflow4_et;
   Float_t         MET_RefTau_LCW_NI_eflow4_sumet;
   Float_t         MET_RefTau_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_etx;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_ety;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_phi;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_et;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_sumet;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow_400_etx;
   Float_t         MET_RefTau_LCW_NI_eflow_400_ety;
   Float_t         MET_RefTau_LCW_NI_eflow_400_phi;
   Float_t         MET_RefTau_LCW_NI_eflow_400_et;
   Float_t         MET_RefTau_LCW_NI_eflow_400_sumet;
   Float_t         MET_RefTau_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_RefTau_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_RefTau_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_RefTau_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_RefJet_GCW_pt20_etx;
   Float_t         MET_RefJet_GCW_pt20_ety;
   Float_t         MET_RefJet_GCW_pt20_phi;
   Float_t         MET_RefJet_GCW_pt20_et;
   Float_t         MET_RefJet_GCW_pt20_sumet;
   Float_t         MET_RefJet_GCW_pt20_etx_CentralReg;
   Float_t         MET_RefJet_GCW_pt20_ety_CentralReg;
   Float_t         MET_RefJet_GCW_pt20_sumet_CentralReg;
   Float_t         MET_RefJet_GCW_pt20_phi_CentralReg;
   Float_t         MET_RefJet_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefJet_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefJet_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefJet_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefJet_GCW_pt20_etx_ForwardReg;
   Float_t         MET_RefJet_GCW_pt20_ety_ForwardReg;
   Float_t         MET_RefJet_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefJet_GCW_pt20_phi_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow4_etx;
   Float_t         MET_RefJet_LCW_NI_eflow4_ety;
   Float_t         MET_RefJet_LCW_NI_eflow4_phi;
   Float_t         MET_RefJet_LCW_NI_eflow4_et;
   Float_t         MET_RefJet_LCW_NI_eflow4_sumet;
   Float_t         MET_RefJet_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_etx;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_ety;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_phi;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_et;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_sumet;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow_400_etx;
   Float_t         MET_RefJet_LCW_NI_eflow_400_ety;
   Float_t         MET_RefJet_LCW_NI_eflow_400_phi;
   Float_t         MET_RefJet_LCW_NI_eflow_400_et;
   Float_t         MET_RefJet_LCW_NI_eflow_400_sumet;
   Float_t         MET_RefJet_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_RefJet_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_RefJet_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_RefJet_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_RefMuon_GCW_pt20_etx;
   Float_t         MET_RefMuon_GCW_pt20_ety;
   Float_t         MET_RefMuon_GCW_pt20_phi;
   Float_t         MET_RefMuon_GCW_pt20_et;
   Float_t         MET_RefMuon_GCW_pt20_sumet;
   Float_t         MET_RefMuon_GCW_pt20_etx_CentralReg;
   Float_t         MET_RefMuon_GCW_pt20_ety_CentralReg;
   Float_t         MET_RefMuon_GCW_pt20_sumet_CentralReg;
   Float_t         MET_RefMuon_GCW_pt20_phi_CentralReg;
   Float_t         MET_RefMuon_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefMuon_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefMuon_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefMuon_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefMuon_GCW_pt20_etx_ForwardReg;
   Float_t         MET_RefMuon_GCW_pt20_ety_ForwardReg;
   Float_t         MET_RefMuon_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefMuon_GCW_pt20_phi_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow4_etx;
   Float_t         MET_RefMuon_LCW_NI_eflow4_ety;
   Float_t         MET_RefMuon_LCW_NI_eflow4_phi;
   Float_t         MET_RefMuon_LCW_NI_eflow4_et;
   Float_t         MET_RefMuon_LCW_NI_eflow4_sumet;
   Float_t         MET_RefMuon_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_etx;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_ety;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_phi;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_et;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_sumet;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_etx;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_ety;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_phi;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_et;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_sumet;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_RefMuon_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_RefMuon_Staco_GCW_pt20_etx;
   Float_t         MET_RefMuon_Staco_GCW_pt20_ety;
   Float_t         MET_RefMuon_Staco_GCW_pt20_phi;
   Float_t         MET_RefMuon_Staco_GCW_pt20_et;
   Float_t         MET_RefMuon_Staco_GCW_pt20_sumet;
   Float_t         MET_RefMuon_Staco_GCW_pt20_etx_CentralReg;
   Float_t         MET_RefMuon_Staco_GCW_pt20_ety_CentralReg;
   Float_t         MET_RefMuon_Staco_GCW_pt20_sumet_CentralReg;
   Float_t         MET_RefMuon_Staco_GCW_pt20_phi_CentralReg;
   Float_t         MET_RefMuon_Staco_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefMuon_Staco_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefMuon_Staco_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Staco_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefMuon_Staco_GCW_pt20_etx_ForwardReg;
   Float_t         MET_RefMuon_Staco_GCW_pt20_ety_ForwardReg;
   Float_t         MET_RefMuon_Staco_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefMuon_Staco_GCW_pt20_phi_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_etx;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_ety;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_phi;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_et;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_sumet;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_etx;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_ety;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_phi;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_et;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_etx;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_ety;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_phi;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_et;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_sumet;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_RefMuon_Staco_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_CellOut_GCW_pt20_etx;
   Float_t         MET_CellOut_GCW_pt20_ety;
   Float_t         MET_CellOut_GCW_pt20_phi;
   Float_t         MET_CellOut_GCW_pt20_et;
   Float_t         MET_CellOut_GCW_pt20_sumet;
   Float_t         MET_CellOut_GCW_pt20_etx_CentralReg;
   Float_t         MET_CellOut_GCW_pt20_ety_CentralReg;
   Float_t         MET_CellOut_GCW_pt20_sumet_CentralReg;
   Float_t         MET_CellOut_GCW_pt20_phi_CentralReg;
   Float_t         MET_CellOut_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_CellOut_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_CellOut_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_CellOut_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_CellOut_GCW_pt20_etx_ForwardReg;
   Float_t         MET_CellOut_GCW_pt20_ety_ForwardReg;
   Float_t         MET_CellOut_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_CellOut_GCW_pt20_phi_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow4_etx;
   Float_t         MET_CellOut_LCW_NI_eflow4_ety;
   Float_t         MET_CellOut_LCW_NI_eflow4_phi;
   Float_t         MET_CellOut_LCW_NI_eflow4_et;
   Float_t         MET_CellOut_LCW_NI_eflow4_sumet;
   Float_t         MET_CellOut_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_etx;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_ety;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_phi;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_et;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_sumet;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow_400_etx;
   Float_t         MET_CellOut_LCW_NI_eflow_400_ety;
   Float_t         MET_CellOut_LCW_NI_eflow_400_phi;
   Float_t         MET_CellOut_LCW_NI_eflow_400_et;
   Float_t         MET_CellOut_LCW_NI_eflow_400_sumet;
   Float_t         MET_CellOut_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_CellOut_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_CellOut_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_CellOut_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_Cryo_GCW_pt20_etx;
   Float_t         MET_Cryo_GCW_pt20_ety;
   Float_t         MET_Cryo_GCW_pt20_phi;
   Float_t         MET_Cryo_GCW_pt20_et;
   Float_t         MET_Cryo_GCW_pt20_sumet;
   Float_t         MET_Cryo_GCW_pt20_etx_CentralReg;
   Float_t         MET_Cryo_GCW_pt20_ety_CentralReg;
   Float_t         MET_Cryo_GCW_pt20_sumet_CentralReg;
   Float_t         MET_Cryo_GCW_pt20_phi_CentralReg;
   Float_t         MET_Cryo_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_Cryo_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_Cryo_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_Cryo_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_Cryo_GCW_pt20_etx_ForwardReg;
   Float_t         MET_Cryo_GCW_pt20_ety_ForwardReg;
   Float_t         MET_Cryo_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_Cryo_GCW_pt20_phi_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow4_etx;
   Float_t         MET_Cryo_LCW_NI_eflow4_ety;
   Float_t         MET_Cryo_LCW_NI_eflow4_phi;
   Float_t         MET_Cryo_LCW_NI_eflow4_et;
   Float_t         MET_Cryo_LCW_NI_eflow4_sumet;
   Float_t         MET_Cryo_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_etx;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_ety;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_phi;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_et;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_sumet;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow_400_etx;
   Float_t         MET_Cryo_LCW_NI_eflow_400_ety;
   Float_t         MET_Cryo_LCW_NI_eflow_400_phi;
   Float_t         MET_Cryo_LCW_NI_eflow_400_et;
   Float_t         MET_Cryo_LCW_NI_eflow_400_sumet;
   Float_t         MET_Cryo_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_Cryo_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_Cryo_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_Cryo_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_etx;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_ety;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_phi;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_et;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_sumet;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_etx_CentralReg;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_ety_CentralReg;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_sumet_CentralReg;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_phi_CentralReg;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_etx_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_ety_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_GCW_pt20_phi_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_etx;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_ety;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_phi;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_et;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_et;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_et;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_etx;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_ety;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_phi;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_et;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_sumet;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_etx_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_ety_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_sumet_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_phi_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_etx_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_ety_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_GCW_pt20_phi_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_et;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_et;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_et;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_etx;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_ety;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_phi;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_et;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_sumet;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_etx_CentralReg;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_ety_CentralReg;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_sumet_CentralReg;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_phi_CentralReg;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_etx_ForwardReg;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_ety_ForwardReg;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_Muon_Total_Staco_GCW_pt20_phi_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_etx;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_ety;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_phi;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_et;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_sumet;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_et;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_etx;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_ety;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_phi;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_et;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_SoftJets_GCW_pt20_etx;
   Float_t         MET_SoftJets_GCW_pt20_ety;
   Float_t         MET_SoftJets_GCW_pt20_phi;
   Float_t         MET_SoftJets_GCW_pt20_et;
   Float_t         MET_SoftJets_GCW_pt20_sumet;
   Float_t         MET_SoftJets_GCW_pt20_etx_CentralReg;
   Float_t         MET_SoftJets_GCW_pt20_ety_CentralReg;
   Float_t         MET_SoftJets_GCW_pt20_sumet_CentralReg;
   Float_t         MET_SoftJets_GCW_pt20_phi_CentralReg;
   Float_t         MET_SoftJets_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_SoftJets_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_SoftJets_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_SoftJets_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_SoftJets_GCW_pt20_etx_ForwardReg;
   Float_t         MET_SoftJets_GCW_pt20_ety_ForwardReg;
   Float_t         MET_SoftJets_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_SoftJets_GCW_pt20_phi_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow4_etx;
   Float_t         MET_SoftJets_LCW_NI_eflow4_ety;
   Float_t         MET_SoftJets_LCW_NI_eflow4_phi;
   Float_t         MET_SoftJets_LCW_NI_eflow4_et;
   Float_t         MET_SoftJets_LCW_NI_eflow4_sumet;
   Float_t         MET_SoftJets_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_etx;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_ety;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_phi;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_et;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_sumet;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_etx;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_ety;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_phi;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_et;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_sumet;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_SoftJets_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_GCW_pt20_etx;
   Float_t         MET_RefMuon_Track_GCW_pt20_ety;
   Float_t         MET_RefMuon_Track_GCW_pt20_phi;
   Float_t         MET_RefMuon_Track_GCW_pt20_et;
   Float_t         MET_RefMuon_Track_GCW_pt20_sumet;
   Float_t         MET_RefMuon_Track_GCW_pt20_etx_CentralReg;
   Float_t         MET_RefMuon_Track_GCW_pt20_ety_CentralReg;
   Float_t         MET_RefMuon_Track_GCW_pt20_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_GCW_pt20_phi_CentralReg;
   Float_t         MET_RefMuon_Track_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_GCW_pt20_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_GCW_pt20_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_GCW_pt20_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_etx;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_ety;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_phi;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_et;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_sumet;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_etx;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_ety;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_phi;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_et;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_sumet;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_etx;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_ety;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_phi;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_et;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_sumet;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_etx;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_ety;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_phi;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_et;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_sumet;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_etx_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_ety_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_phi_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_GCW_pt20_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_et;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_et;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_et;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_CentralReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_ForwardReg;
   Float_t         MET_RefFinal_LCW_pt20_etx;
   Float_t         MET_RefFinal_LCW_pt20_ety;
   Float_t         MET_RefFinal_LCW_pt20_phi;
   Float_t         MET_RefFinal_LCW_pt20_et;
   Float_t         MET_RefFinal_LCW_pt20_sumet;
   Float_t         MET_RefFinal_LCW_pt20_etx_CentralReg;
   Float_t         MET_RefFinal_LCW_pt20_ety_CentralReg;
   Float_t         MET_RefFinal_LCW_pt20_sumet_CentralReg;
   Float_t         MET_RefFinal_LCW_pt20_phi_CentralReg;
   Float_t         MET_RefFinal_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefFinal_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefFinal_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefFinal_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefFinal_LCW_pt20_etx_ForwardReg;
   Float_t         MET_RefFinal_LCW_pt20_ety_ForwardReg;
   Float_t         MET_RefFinal_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefFinal_LCW_pt20_phi_ForwardReg;
   Float_t         MET_RefGamma_LCW_pt20_etx;
   Float_t         MET_RefGamma_LCW_pt20_ety;
   Float_t         MET_RefGamma_LCW_pt20_phi;
   Float_t         MET_RefGamma_LCW_pt20_et;
   Float_t         MET_RefGamma_LCW_pt20_sumet;
   Float_t         MET_RefGamma_LCW_pt20_etx_CentralReg;
   Float_t         MET_RefGamma_LCW_pt20_ety_CentralReg;
   Float_t         MET_RefGamma_LCW_pt20_sumet_CentralReg;
   Float_t         MET_RefGamma_LCW_pt20_phi_CentralReg;
   Float_t         MET_RefGamma_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefGamma_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefGamma_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefGamma_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefGamma_LCW_pt20_etx_ForwardReg;
   Float_t         MET_RefGamma_LCW_pt20_ety_ForwardReg;
   Float_t         MET_RefGamma_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefGamma_LCW_pt20_phi_ForwardReg;
   Float_t         MET_RefEle_LCW_pt20_etx;
   Float_t         MET_RefEle_LCW_pt20_ety;
   Float_t         MET_RefEle_LCW_pt20_phi;
   Float_t         MET_RefEle_LCW_pt20_et;
   Float_t         MET_RefEle_LCW_pt20_sumet;
   Float_t         MET_RefEle_LCW_pt20_etx_CentralReg;
   Float_t         MET_RefEle_LCW_pt20_ety_CentralReg;
   Float_t         MET_RefEle_LCW_pt20_sumet_CentralReg;
   Float_t         MET_RefEle_LCW_pt20_phi_CentralReg;
   Float_t         MET_RefEle_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefEle_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefEle_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefEle_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefEle_LCW_pt20_etx_ForwardReg;
   Float_t         MET_RefEle_LCW_pt20_ety_ForwardReg;
   Float_t         MET_RefEle_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefEle_LCW_pt20_phi_ForwardReg;
   Float_t         MET_RefTau_LCW_pt20_etx;
   Float_t         MET_RefTau_LCW_pt20_ety;
   Float_t         MET_RefTau_LCW_pt20_phi;
   Float_t         MET_RefTau_LCW_pt20_et;
   Float_t         MET_RefTau_LCW_pt20_sumet;
   Float_t         MET_RefTau_LCW_pt20_etx_CentralReg;
   Float_t         MET_RefTau_LCW_pt20_ety_CentralReg;
   Float_t         MET_RefTau_LCW_pt20_sumet_CentralReg;
   Float_t         MET_RefTau_LCW_pt20_phi_CentralReg;
   Float_t         MET_RefTau_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefTau_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefTau_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefTau_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefTau_LCW_pt20_etx_ForwardReg;
   Float_t         MET_RefTau_LCW_pt20_ety_ForwardReg;
   Float_t         MET_RefTau_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefTau_LCW_pt20_phi_ForwardReg;
   Float_t         MET_RefJet_LCW_pt20_etx;
   Float_t         MET_RefJet_LCW_pt20_ety;
   Float_t         MET_RefJet_LCW_pt20_phi;
   Float_t         MET_RefJet_LCW_pt20_et;
   Float_t         MET_RefJet_LCW_pt20_sumet;
   Float_t         MET_RefJet_LCW_pt20_etx_CentralReg;
   Float_t         MET_RefJet_LCW_pt20_ety_CentralReg;
   Float_t         MET_RefJet_LCW_pt20_sumet_CentralReg;
   Float_t         MET_RefJet_LCW_pt20_phi_CentralReg;
   Float_t         MET_RefJet_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefJet_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefJet_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefJet_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefJet_LCW_pt20_etx_ForwardReg;
   Float_t         MET_RefJet_LCW_pt20_ety_ForwardReg;
   Float_t         MET_RefJet_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefJet_LCW_pt20_phi_ForwardReg;
   Float_t         MET_RefMuon_LCW_pt20_etx;
   Float_t         MET_RefMuon_LCW_pt20_ety;
   Float_t         MET_RefMuon_LCW_pt20_phi;
   Float_t         MET_RefMuon_LCW_pt20_et;
   Float_t         MET_RefMuon_LCW_pt20_sumet;
   Float_t         MET_RefMuon_LCW_pt20_etx_CentralReg;
   Float_t         MET_RefMuon_LCW_pt20_ety_CentralReg;
   Float_t         MET_RefMuon_LCW_pt20_sumet_CentralReg;
   Float_t         MET_RefMuon_LCW_pt20_phi_CentralReg;
   Float_t         MET_RefMuon_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefMuon_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefMuon_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefMuon_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefMuon_LCW_pt20_etx_ForwardReg;
   Float_t         MET_RefMuon_LCW_pt20_ety_ForwardReg;
   Float_t         MET_RefMuon_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefMuon_LCW_pt20_phi_ForwardReg;
   Float_t         MET_RefMuon_Muid_LCW_pt20_etx;
   Float_t         MET_RefMuon_Muid_LCW_pt20_ety;
   Float_t         MET_RefMuon_Muid_LCW_pt20_phi;
   Float_t         MET_RefMuon_Muid_LCW_pt20_et;
   Float_t         MET_RefMuon_Muid_LCW_pt20_sumet;
   Float_t         MET_RefMuon_Muid_LCW_pt20_etx_CentralReg;
   Float_t         MET_RefMuon_Muid_LCW_pt20_ety_CentralReg;
   Float_t         MET_RefMuon_Muid_LCW_pt20_sumet_CentralReg;
   Float_t         MET_RefMuon_Muid_LCW_pt20_phi_CentralReg;
   Float_t         MET_RefMuon_Muid_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefMuon_Muid_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefMuon_Muid_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Muid_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefMuon_Muid_LCW_pt20_etx_ForwardReg;
   Float_t         MET_RefMuon_Muid_LCW_pt20_ety_ForwardReg;
   Float_t         MET_RefMuon_Muid_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefMuon_Muid_LCW_pt20_phi_ForwardReg;
   Float_t         MET_CellOut_LCW_pt20_etx;
   Float_t         MET_CellOut_LCW_pt20_ety;
   Float_t         MET_CellOut_LCW_pt20_phi;
   Float_t         MET_CellOut_LCW_pt20_et;
   Float_t         MET_CellOut_LCW_pt20_sumet;
   Float_t         MET_CellOut_LCW_pt20_etx_CentralReg;
   Float_t         MET_CellOut_LCW_pt20_ety_CentralReg;
   Float_t         MET_CellOut_LCW_pt20_sumet_CentralReg;
   Float_t         MET_CellOut_LCW_pt20_phi_CentralReg;
   Float_t         MET_CellOut_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_CellOut_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_CellOut_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_CellOut_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_CellOut_LCW_pt20_etx_ForwardReg;
   Float_t         MET_CellOut_LCW_pt20_ety_ForwardReg;
   Float_t         MET_CellOut_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_CellOut_LCW_pt20_phi_ForwardReg;
   Float_t         MET_Cryo_LCW_pt20_etx;
   Float_t         MET_Cryo_LCW_pt20_ety;
   Float_t         MET_Cryo_LCW_pt20_phi;
   Float_t         MET_Cryo_LCW_pt20_et;
   Float_t         MET_Cryo_LCW_pt20_sumet;
   Float_t         MET_Cryo_LCW_pt20_etx_CentralReg;
   Float_t         MET_Cryo_LCW_pt20_ety_CentralReg;
   Float_t         MET_Cryo_LCW_pt20_sumet_CentralReg;
   Float_t         MET_Cryo_LCW_pt20_phi_CentralReg;
   Float_t         MET_Cryo_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_Cryo_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_Cryo_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_Cryo_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_Cryo_LCW_pt20_etx_ForwardReg;
   Float_t         MET_Cryo_LCW_pt20_ety_ForwardReg;
   Float_t         MET_Cryo_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_Cryo_LCW_pt20_phi_ForwardReg;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_etx;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_ety;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_phi;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_et;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_sumet;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_etx_CentralReg;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_ety_CentralReg;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_sumet_CentralReg;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_phi_CentralReg;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_etx_ForwardReg;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_ety_ForwardReg;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_Muon_Isol_Muid_LCW_pt20_phi_ForwardReg;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_etx;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_ety;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_phi;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_et;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_sumet;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_etx_CentralReg;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_ety_CentralReg;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_sumet_CentralReg;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_phi_CentralReg;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_etx_ForwardReg;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_ety_ForwardReg;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_Muon_NonIsol_Muid_LCW_pt20_phi_ForwardReg;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_etx;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_ety;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_phi;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_et;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_sumet;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_etx_CentralReg;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_ety_CentralReg;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_sumet_CentralReg;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_phi_CentralReg;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_etx_ForwardReg;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_ety_ForwardReg;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_Muon_Total_Muid_LCW_pt20_phi_ForwardReg;
   Float_t         MET_SoftJets_LCW_pt20_etx;
   Float_t         MET_SoftJets_LCW_pt20_ety;
   Float_t         MET_SoftJets_LCW_pt20_phi;
   Float_t         MET_SoftJets_LCW_pt20_et;
   Float_t         MET_SoftJets_LCW_pt20_sumet;
   Float_t         MET_SoftJets_LCW_pt20_etx_CentralReg;
   Float_t         MET_SoftJets_LCW_pt20_ety_CentralReg;
   Float_t         MET_SoftJets_LCW_pt20_sumet_CentralReg;
   Float_t         MET_SoftJets_LCW_pt20_phi_CentralReg;
   Float_t         MET_SoftJets_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_SoftJets_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_SoftJets_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_SoftJets_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_SoftJets_LCW_pt20_etx_ForwardReg;
   Float_t         MET_SoftJets_LCW_pt20_ety_ForwardReg;
   Float_t         MET_SoftJets_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_SoftJets_LCW_pt20_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_pt20_etx;
   Float_t         MET_RefMuon_Track_LCW_pt20_ety;
   Float_t         MET_RefMuon_Track_LCW_pt20_phi;
   Float_t         MET_RefMuon_Track_LCW_pt20_et;
   Float_t         MET_RefMuon_Track_LCW_pt20_sumet;
   Float_t         MET_RefMuon_Track_LCW_pt20_etx_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_pt20_ety_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_pt20_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_pt20_phi_CentralReg;
   Float_t         MET_RefMuon_Track_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_LCW_pt20_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_pt20_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_LCW_pt20_phi_ForwardReg;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_etx;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_ety;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_phi;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_et;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_sumet;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_etx_CentralReg;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_ety_CentralReg;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_sumet_CentralReg;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_phi_CentralReg;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_etx_EndcapRegion;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_ety_EndcapRegion;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_sumet_EndcapRegion;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_phi_EndcapRegion;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_etx_ForwardReg;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_ety_ForwardReg;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_sumet_ForwardReg;
   Float_t         MET_RefMuon_Track_Muid_LCW_pt20_phi_ForwardReg;
   Bool_t          L1_2J10;
   Bool_t          L1_2J15;
   Bool_t          L1_2J5;
   Bool_t          L1_J10;
   Bool_t          L1_J15;
   Bool_t          L1_J30;
   Bool_t          L1_J5;
   Bool_t          L1_J55;
   Bool_t          L1_J75;
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
   UInt_t          trig_DB_SMK;
   UInt_t          trig_DB_L1PSK;
   UInt_t          trig_DB_HLTPSK;

   // List of branches
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_lbn;   //!
   TBranch        *b_bcid;   //!
   TBranch        *b_collcand_passMBTSTime;   //!
   TBranch        *b_collcand_passCaloTime;   //!
   TBranch        *b_collcand_passTrigger;   //!
   TBranch        *b_collcand_pass;   //!
   TBranch        *b_beamSpot_x;   //!
   TBranch        *b_beamSpot_y;   //!
   TBranch        *b_beamSpot_z;   //!
   TBranch        *b_vxp_n;   //!
   TBranch        *b_vxp_x;   //!
   TBranch        *b_vxp_y;   //!
   TBranch        *b_vxp_z;   //!
   TBranch        *b_vxp_cov_x;   //!
   TBranch        *b_vxp_cov_y;   //!
   TBranch        *b_vxp_cov_z;   //!
   TBranch        *b_vxp_cov_xy;   //!
   TBranch        *b_vxp_cov_xz;   //!
   TBranch        *b_vxp_cov_yz;   //!
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
   TBranch        *b_vxp_trk_index;   //!
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
   TBranch        *b_el_loose;   //!
   TBranch        *b_el_medium;   //!
   TBranch        *b_el_mediumIso;   //!
   TBranch        *b_el_tight;   //!
   TBranch        *b_el_tightIso;   //!
   TBranch        *b_el_Etcone45;   //!
   TBranch        *b_el_Etcone20;   //!
   TBranch        *b_el_Etcone30;   //!
   TBranch        *b_el_Etcone40;   //!
   TBranch        *b_el_Es2;   //!
   TBranch        *b_el_etas2;   //!
   TBranch        *b_el_phis2;   //!
   TBranch        *b_el_cl_E;   //!
   TBranch        *b_el_cl_pt;   //!
   TBranch        *b_el_cl_eta;   //!
   TBranch        *b_el_cl_phi;   //!
   TBranch        *b_el_trackd0;   //!
   TBranch        *b_el_trackz0;   //!
   TBranch        *b_el_trackphi;   //!
   TBranch        *b_el_tracktheta;   //!
   TBranch        *b_el_trackqoverp;   //!
   TBranch        *b_el_trackpt;   //!
   TBranch        *b_el_tracketa;   //!
   TBranch        *b_el_vertx;   //!
   TBranch        *b_el_verty;   //!
   TBranch        *b_el_vertz;   //!
   TBranch        *b_EF_e10_loose;   //!
   TBranch        *b_EF_e10_loose_passEF;   //!
   TBranch        *b_EF_e10_loose_passL2;   //!
   TBranch        *b_EF_e10_medium;   //!
   TBranch        *b_EF_e20_loose_NoIDTrkCut;   //!
   TBranch        *b_EF_mu10;   //!
   TBranch        *b_EF_mu15;   //!
   TBranch        *b_L1_EM10;   //!
   TBranch        *b_L1_EM10I;   //!
   TBranch        *b_L1_EM14;   //!
   TBranch        *b_L1_MU10;   //!
   TBranch        *b_L1_MU15;   //!
   TBranch        *b_L1_MU20;   //!
   TBranch        *b_L1_MU6;   //!
   TBranch        *b_L1_XE10;   //!
   TBranch        *b_L1_XE15;   //!
   TBranch        *b_L1_XE20;   //!
   TBranch        *b_L1_XE30;   //!
   TBranch        *b_mu_staco_n;   //!
   TBranch        *b_mu_staco_E;   //!
   TBranch        *b_mu_staco_px;   //!
   TBranch        *b_mu_staco_py;   //!
   TBranch        *b_mu_staco_pz;   //!
   TBranch        *b_mu_staco_charge;   //!
   TBranch        *b_mu_staco_author;   //!
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
   TBranch        *b_mu_staco_bestMatch;   //!
   TBranch        *b_mu_staco_isStandAloneMuon;   //!
   TBranch        *b_mu_staco_isCombinedMuon;   //!
   TBranch        *b_mu_staco_isLowPtReconstructedMuon;   //!
   TBranch        *b_mu_staco_d0_exPV;   //!
   TBranch        *b_mu_staco_z0_exPV;   //!
   TBranch        *b_mu_staco_ms_phi;   //!
   TBranch        *b_mu_staco_ms_theta;   //!
   TBranch        *b_mu_staco_ms_qoverp;   //!
   TBranch        *b_mu_staco_id_phi;   //!
   TBranch        *b_mu_staco_id_theta;   //!
   TBranch        *b_mu_staco_id_qoverp;   //!
   TBranch        *b_jet_AntiKt4LCTopo_n;   //!
   TBranch        *b_jet_AntiKt4LCTopo_E;   //!
   TBranch        *b_jet_AntiKt4LCTopo_pt;   //!
   TBranch        *b_jet_AntiKt4LCTopo_m;   //!
   TBranch        *b_jet_AntiKt4LCTopo_eta;   //!
   TBranch        *b_jet_AntiKt4LCTopo_phi;   //!
   TBranch        *b_jet_AntiKt4LCTopo_n90;   //!
   TBranch        *b_jet_AntiKt4LCTopo_Timing;   //!
   TBranch        *b_jet_AntiKt4LCTopo_LArQuality;   //!
   TBranch        *b_jet_AntiKt4LCTopo_HECQuality;   //!
   TBranch        *b_jet_AntiKt4LCTopo_BCH_CORR_CELL;   //!
   TBranch        *b_jet_AntiKt4LCTopo_BCH_CORR_DOTX;   //!
   TBranch        *b_jet_AntiKt4LCTopo_SamplingMax;   //!
   TBranch        *b_jet_AntiKt4LCTopo_fracSamplingMax;   //!
   TBranch        *b_jet_AntiKt4LCTopo_hecf;   //!
   TBranch        *b_jet_AntiKt4LCTopo_tgap3f;   //!
   TBranch        *b_jet_AntiKt4LCTopo_emfrac;   //!
   TBranch        *b_jet_AntiKt4LCTopo_EMJES;   //!
   TBranch        *b_jet_AntiKt4LCTopo_GCWJES;   //!
   TBranch        *b_jet_AntiKt4LCTopo_emscale_E;   //!
   TBranch        *b_jet_AntiKt4LCTopo_emscale_pt;   //!
   TBranch        *b_jet_AntiKt4LCTopo_emscale_m;   //!
   TBranch        *b_jet_AntiKt4LCTopo_emscale_eta;   //!
   TBranch        *b_jet_AntiKt4LCTopo_emscale_phi;   //!
   TBranch        *b_jet_AntiKt6LCTopo_n;   //!
   TBranch        *b_jet_AntiKt6LCTopo_E;   //!
   TBranch        *b_jet_AntiKt6LCTopo_pt;   //!
   TBranch        *b_jet_AntiKt6LCTopo_m;   //!
   TBranch        *b_jet_AntiKt6LCTopo_eta;   //!
   TBranch        *b_jet_AntiKt6LCTopo_phi;   //!
   TBranch        *b_jet_AntiKt6LCTopo_n90;   //!
   TBranch        *b_jet_AntiKt6LCTopo_Timing;   //!
   TBranch        *b_jet_AntiKt6LCTopo_LArQuality;   //!
   TBranch        *b_jet_AntiKt6LCTopo_HECQuality;   //!
   TBranch        *b_jet_AntiKt6LCTopo_BCH_CORR_CELL;   //!
   TBranch        *b_jet_AntiKt6LCTopo_BCH_CORR_DOTX;   //!
   TBranch        *b_jet_AntiKt6LCTopo_SamplingMax;   //!
   TBranch        *b_jet_AntiKt6LCTopo_fracSamplingMax;   //!
   TBranch        *b_jet_AntiKt6LCTopo_hecf;   //!
   TBranch        *b_jet_AntiKt6LCTopo_tgap3f;   //!
   TBranch        *b_jet_AntiKt6LCTopo_emfrac;   //!
   TBranch        *b_jet_AntiKt6LCTopo_EMJES;   //!
   TBranch        *b_jet_AntiKt6LCTopo_GCWJES;   //!
   TBranch        *b_jet_AntiKt6LCTopo_emscale_E;   //!
   TBranch        *b_jet_AntiKt6LCTopo_emscale_pt;   //!
   TBranch        *b_jet_AntiKt6LCTopo_emscale_m;   //!
   TBranch        *b_jet_AntiKt6LCTopo_emscale_eta;   //!
   TBranch        *b_jet_AntiKt6LCTopo_emscale_phi;   //!
   TBranch        *b_MET_RefFinal_etx;   //!
   TBranch        *b_MET_RefFinal_ety;   //!
   TBranch        *b_MET_RefFinal_phi;   //!
   TBranch        *b_MET_RefFinal_et;   //!
   TBranch        *b_MET_RefFinal_sumet;   //!
   TBranch        *b_MET_RefFinal_etx_CentralReg;   //!
   TBranch        *b_MET_RefFinal_ety_CentralReg;   //!
   TBranch        *b_MET_RefFinal_sumet_CentralReg;   //!
   TBranch        *b_MET_RefFinal_phi_CentralReg;   //!
   TBranch        *b_MET_RefFinal_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_etx_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_ety_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_phi_ForwardReg;   //!
   TBranch        *b_MET_Cryo_etx;   //!
   TBranch        *b_MET_Cryo_ety;   //!
   TBranch        *b_MET_Cryo_phi;   //!
   TBranch        *b_MET_Cryo_et;   //!
   TBranch        *b_MET_Cryo_sumet;   //!
   TBranch        *b_MET_Cryo_etx_CentralReg;   //!
   TBranch        *b_MET_Cryo_ety_CentralReg;   //!
   TBranch        *b_MET_Cryo_sumet_CentralReg;   //!
   TBranch        *b_MET_Cryo_phi_CentralReg;   //!
   TBranch        *b_MET_Cryo_etx_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_ety_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_phi_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_etx_ForwardReg;   //!
   TBranch        *b_MET_Cryo_ety_ForwardReg;   //!
   TBranch        *b_MET_Cryo_sumet_ForwardReg;   //!
   TBranch        *b_MET_Cryo_phi_ForwardReg;   //!
   TBranch        *b_MET_RefEle_etx;   //!
   TBranch        *b_MET_RefEle_ety;   //!
   TBranch        *b_MET_RefEle_phi;   //!
   TBranch        *b_MET_RefEle_et;   //!
   TBranch        *b_MET_RefEle_sumet;   //!
   TBranch        *b_MET_RefEle_etx_CentralReg;   //!
   TBranch        *b_MET_RefEle_ety_CentralReg;   //!
   TBranch        *b_MET_RefEle_sumet_CentralReg;   //!
   TBranch        *b_MET_RefEle_phi_CentralReg;   //!
   TBranch        *b_MET_RefEle_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_etx_ForwardReg;   //!
   TBranch        *b_MET_RefEle_ety_ForwardReg;   //!
   TBranch        *b_MET_RefEle_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefEle_phi_ForwardReg;   //!
   TBranch        *b_MET_RefJet_etx;   //!
   TBranch        *b_MET_RefJet_ety;   //!
   TBranch        *b_MET_RefJet_phi;   //!
   TBranch        *b_MET_RefJet_et;   //!
   TBranch        *b_MET_RefJet_sumet;   //!
   TBranch        *b_MET_RefJet_etx_CentralReg;   //!
   TBranch        *b_MET_RefJet_ety_CentralReg;   //!
   TBranch        *b_MET_RefJet_sumet_CentralReg;   //!
   TBranch        *b_MET_RefJet_phi_CentralReg;   //!
   TBranch        *b_MET_RefJet_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_etx_ForwardReg;   //!
   TBranch        *b_MET_RefJet_ety_ForwardReg;   //!
   TBranch        *b_MET_RefJet_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefJet_phi_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_etx;   //!
   TBranch        *b_MET_SoftJets_ety;   //!
   TBranch        *b_MET_SoftJets_phi;   //!
   TBranch        *b_MET_SoftJets_et;   //!
   TBranch        *b_MET_SoftJets_sumet;   //!
   TBranch        *b_MET_SoftJets_etx_CentralReg;   //!
   TBranch        *b_MET_SoftJets_ety_CentralReg;   //!
   TBranch        *b_MET_SoftJets_sumet_CentralReg;   //!
   TBranch        *b_MET_SoftJets_phi_CentralReg;   //!
   TBranch        *b_MET_SoftJets_etx_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_ety_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_sumet_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_phi_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_etx_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_ety_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_sumet_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_etx;   //!
   TBranch        *b_MET_RefMuon_ety;   //!
   TBranch        *b_MET_RefMuon_phi;   //!
   TBranch        *b_MET_RefMuon_et;   //!
   TBranch        *b_MET_RefMuon_sumet;   //!
   TBranch        *b_MET_RefMuon_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_etx;   //!
   TBranch        *b_MET_RefMuon_Staco_ety;   //!
   TBranch        *b_MET_RefMuon_Staco_phi;   //!
   TBranch        *b_MET_RefMuon_Staco_et;   //!
   TBranch        *b_MET_RefMuon_Staco_sumet;   //!
   TBranch        *b_MET_RefMuon_Staco_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Muid_etx;   //!
   TBranch        *b_MET_RefMuon_Muid_ety;   //!
   TBranch        *b_MET_RefMuon_Muid_phi;   //!
   TBranch        *b_MET_RefMuon_Muid_et;   //!
   TBranch        *b_MET_RefMuon_Muid_sumet;   //!
   TBranch        *b_MET_RefMuon_Muid_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Muid_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Muid_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Muid_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Muid_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Muid_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Muid_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Muid_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Muid_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Muid_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Muid_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Muid_phi_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_etx;   //!
   TBranch        *b_MET_RefGamma_ety;   //!
   TBranch        *b_MET_RefGamma_phi;   //!
   TBranch        *b_MET_RefGamma_et;   //!
   TBranch        *b_MET_RefGamma_sumet;   //!
   TBranch        *b_MET_RefGamma_etx_CentralReg;   //!
   TBranch        *b_MET_RefGamma_ety_CentralReg;   //!
   TBranch        *b_MET_RefGamma_sumet_CentralReg;   //!
   TBranch        *b_MET_RefGamma_phi_CentralReg;   //!
   TBranch        *b_MET_RefGamma_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_etx_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_ety_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_phi_ForwardReg;   //!
   TBranch        *b_MET_RefTau_etx;   //!
   TBranch        *b_MET_RefTau_ety;   //!
   TBranch        *b_MET_RefTau_phi;   //!
   TBranch        *b_MET_RefTau_et;   //!
   TBranch        *b_MET_RefTau_sumet;   //!
   TBranch        *b_MET_RefTau_etx_CentralReg;   //!
   TBranch        *b_MET_RefTau_ety_CentralReg;   //!
   TBranch        *b_MET_RefTau_sumet_CentralReg;   //!
   TBranch        *b_MET_RefTau_phi_CentralReg;   //!
   TBranch        *b_MET_RefTau_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_etx_ForwardReg;   //!
   TBranch        *b_MET_RefTau_ety_ForwardReg;   //!
   TBranch        *b_MET_RefTau_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefTau_phi_ForwardReg;   //!
   TBranch        *b_MET_CellOut_etx;   //!
   TBranch        *b_MET_CellOut_ety;   //!
   TBranch        *b_MET_CellOut_phi;   //!
   TBranch        *b_MET_CellOut_et;   //!
   TBranch        *b_MET_CellOut_sumet;   //!
   TBranch        *b_MET_CellOut_etx_CentralReg;   //!
   TBranch        *b_MET_CellOut_ety_CentralReg;   //!
   TBranch        *b_MET_CellOut_sumet_CentralReg;   //!
   TBranch        *b_MET_CellOut_phi_CentralReg;   //!
   TBranch        *b_MET_CellOut_etx_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_ety_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_sumet_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_phi_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_etx_ForwardReg;   //!
   TBranch        *b_MET_CellOut_ety_ForwardReg;   //!
   TBranch        *b_MET_CellOut_sumet_ForwardReg;   //!
   TBranch        *b_MET_CellOut_phi_ForwardReg;   //!
   TBranch        *b_MET_Track_etx;   //!
   TBranch        *b_MET_Track_ety;   //!
   TBranch        *b_MET_Track_phi;   //!
   TBranch        *b_MET_Track_et;   //!
   TBranch        *b_MET_Track_sumet;   //!
   TBranch        *b_MET_Track_etx_CentralReg;   //!
   TBranch        *b_MET_Track_ety_CentralReg;   //!
   TBranch        *b_MET_Track_sumet_CentralReg;   //!
   TBranch        *b_MET_Track_phi_CentralReg;   //!
   TBranch        *b_MET_Track_etx_EndcapRegion;   //!
   TBranch        *b_MET_Track_ety_EndcapRegion;   //!
   TBranch        *b_MET_Track_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Track_phi_EndcapRegion;   //!
   TBranch        *b_MET_Track_etx_ForwardReg;   //!
   TBranch        *b_MET_Track_ety_ForwardReg;   //!
   TBranch        *b_MET_Track_sumet_ForwardReg;   //!
   TBranch        *b_MET_Track_phi_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_em_etx;   //!
   TBranch        *b_MET_RefFinal_em_ety;   //!
   TBranch        *b_MET_RefFinal_em_phi;   //!
   TBranch        *b_MET_RefFinal_em_et;   //!
   TBranch        *b_MET_RefFinal_em_sumet;   //!
   TBranch        *b_MET_RefFinal_em_etx_CentralReg;   //!
   TBranch        *b_MET_RefFinal_em_ety_CentralReg;   //!
   TBranch        *b_MET_RefFinal_em_sumet_CentralReg;   //!
   TBranch        *b_MET_RefFinal_em_phi_CentralReg;   //!
   TBranch        *b_MET_RefFinal_em_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_em_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_em_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_em_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_em_etx_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_em_ety_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_em_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_em_phi_ForwardReg;   //!
   TBranch        *b_MET_RefEle_em_etx;   //!
   TBranch        *b_MET_RefEle_em_ety;   //!
   TBranch        *b_MET_RefEle_em_phi;   //!
   TBranch        *b_MET_RefEle_em_et;   //!
   TBranch        *b_MET_RefEle_em_sumet;   //!
   TBranch        *b_MET_RefEle_em_etx_CentralReg;   //!
   TBranch        *b_MET_RefEle_em_ety_CentralReg;   //!
   TBranch        *b_MET_RefEle_em_sumet_CentralReg;   //!
   TBranch        *b_MET_RefEle_em_phi_CentralReg;   //!
   TBranch        *b_MET_RefEle_em_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_em_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_em_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_em_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_em_etx_ForwardReg;   //!
   TBranch        *b_MET_RefEle_em_ety_ForwardReg;   //!
   TBranch        *b_MET_RefEle_em_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefEle_em_phi_ForwardReg;   //!
   TBranch        *b_MET_RefJet_em_etx;   //!
   TBranch        *b_MET_RefJet_em_ety;   //!
   TBranch        *b_MET_RefJet_em_phi;   //!
   TBranch        *b_MET_RefJet_em_et;   //!
   TBranch        *b_MET_RefJet_em_sumet;   //!
   TBranch        *b_MET_RefJet_em_etx_CentralReg;   //!
   TBranch        *b_MET_RefJet_em_ety_CentralReg;   //!
   TBranch        *b_MET_RefJet_em_sumet_CentralReg;   //!
   TBranch        *b_MET_RefJet_em_phi_CentralReg;   //!
   TBranch        *b_MET_RefJet_em_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_em_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_em_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_em_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_em_etx_ForwardReg;   //!
   TBranch        *b_MET_RefJet_em_ety_ForwardReg;   //!
   TBranch        *b_MET_RefJet_em_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefJet_em_phi_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_em_etx;   //!
   TBranch        *b_MET_SoftJets_em_ety;   //!
   TBranch        *b_MET_SoftJets_em_phi;   //!
   TBranch        *b_MET_SoftJets_em_et;   //!
   TBranch        *b_MET_SoftJets_em_sumet;   //!
   TBranch        *b_MET_SoftJets_em_etx_CentralReg;   //!
   TBranch        *b_MET_SoftJets_em_ety_CentralReg;   //!
   TBranch        *b_MET_SoftJets_em_sumet_CentralReg;   //!
   TBranch        *b_MET_SoftJets_em_phi_CentralReg;   //!
   TBranch        *b_MET_SoftJets_em_etx_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_em_ety_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_em_sumet_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_em_phi_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_em_etx_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_em_ety_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_em_sumet_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_em_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_em_etx;   //!
   TBranch        *b_MET_RefMuon_em_ety;   //!
   TBranch        *b_MET_RefMuon_em_phi;   //!
   TBranch        *b_MET_RefMuon_em_et;   //!
   TBranch        *b_MET_RefMuon_em_sumet;   //!
   TBranch        *b_MET_RefMuon_em_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_em_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_em_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_em_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_em_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_em_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_em_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_em_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_em_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_em_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_em_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_em_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_em_etx;   //!
   TBranch        *b_MET_RefMuon_Track_em_ety;   //!
   TBranch        *b_MET_RefMuon_Track_em_phi;   //!
   TBranch        *b_MET_RefMuon_Track_em_et;   //!
   TBranch        *b_MET_RefMuon_Track_em_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_em_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_em_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_em_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_em_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_em_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_em_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_em_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_em_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_em_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_em_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_em_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_em_phi_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_em_etx;   //!
   TBranch        *b_MET_RefGamma_em_ety;   //!
   TBranch        *b_MET_RefGamma_em_phi;   //!
   TBranch        *b_MET_RefGamma_em_et;   //!
   TBranch        *b_MET_RefGamma_em_sumet;   //!
   TBranch        *b_MET_RefGamma_em_etx_CentralReg;   //!
   TBranch        *b_MET_RefGamma_em_ety_CentralReg;   //!
   TBranch        *b_MET_RefGamma_em_sumet_CentralReg;   //!
   TBranch        *b_MET_RefGamma_em_phi_CentralReg;   //!
   TBranch        *b_MET_RefGamma_em_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_em_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_em_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_em_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_em_etx_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_em_ety_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_em_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_em_phi_ForwardReg;   //!
   TBranch        *b_MET_RefTau_em_etx;   //!
   TBranch        *b_MET_RefTau_em_ety;   //!
   TBranch        *b_MET_RefTau_em_phi;   //!
   TBranch        *b_MET_RefTau_em_et;   //!
   TBranch        *b_MET_RefTau_em_sumet;   //!
   TBranch        *b_MET_RefTau_em_etx_CentralReg;   //!
   TBranch        *b_MET_RefTau_em_ety_CentralReg;   //!
   TBranch        *b_MET_RefTau_em_sumet_CentralReg;   //!
   TBranch        *b_MET_RefTau_em_phi_CentralReg;   //!
   TBranch        *b_MET_RefTau_em_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_em_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_em_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_em_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_em_etx_ForwardReg;   //!
   TBranch        *b_MET_RefTau_em_ety_ForwardReg;   //!
   TBranch        *b_MET_RefTau_em_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefTau_em_phi_ForwardReg;   //!
   TBranch        *b_MET_CellOut_em_etx;   //!
   TBranch        *b_MET_CellOut_em_ety;   //!
   TBranch        *b_MET_CellOut_em_phi;   //!
   TBranch        *b_MET_CellOut_em_et;   //!
   TBranch        *b_MET_CellOut_em_sumet;   //!
   TBranch        *b_MET_CellOut_em_etx_CentralReg;   //!
   TBranch        *b_MET_CellOut_em_ety_CentralReg;   //!
   TBranch        *b_MET_CellOut_em_sumet_CentralReg;   //!
   TBranch        *b_MET_CellOut_em_phi_CentralReg;   //!
   TBranch        *b_MET_CellOut_em_etx_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_em_ety_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_em_sumet_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_em_phi_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_em_etx_ForwardReg;   //!
   TBranch        *b_MET_CellOut_em_ety_ForwardReg;   //!
   TBranch        *b_MET_CellOut_em_sumet_ForwardReg;   //!
   TBranch        *b_MET_CellOut_em_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_etx;   //!
   TBranch        *b_MET_Muon_Isol_Staco_ety;   //!
   TBranch        *b_MET_Muon_Isol_Staco_phi;   //!
   TBranch        *b_MET_Muon_Isol_Staco_et;   //!
   TBranch        *b_MET_Muon_Isol_Staco_sumet;   //!
   TBranch        *b_MET_Muon_Isol_Staco_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_etx;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_ety;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_phi;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_et;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_sumet;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_etx;   //!
   TBranch        *b_MET_Muon_Total_Staco_ety;   //!
   TBranch        *b_MET_Muon_Total_Staco_phi;   //!
   TBranch        *b_MET_Muon_Total_Staco_et;   //!
   TBranch        *b_MET_Muon_Total_Staco_sumet;   //!
   TBranch        *b_MET_Muon_Total_Staco_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_etx;   //!
   TBranch        *b_MET_Muon_Isol_Muid_ety;   //!
   TBranch        *b_MET_Muon_Isol_Muid_phi;   //!
   TBranch        *b_MET_Muon_Isol_Muid_et;   //!
   TBranch        *b_MET_Muon_Isol_Muid_sumet;   //!
   TBranch        *b_MET_Muon_Isol_Muid_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Muid_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Muid_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Muid_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Muid_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_etx;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_ety;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_phi;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_et;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_sumet;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_etx;   //!
   TBranch        *b_MET_Muon_Total_Muid_ety;   //!
   TBranch        *b_MET_Muon_Total_Muid_phi;   //!
   TBranch        *b_MET_Muon_Total_Muid_et;   //!
   TBranch        *b_MET_Muon_Total_Muid_sumet;   //!
   TBranch        *b_MET_Muon_Total_Muid_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Muid_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Muid_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Muid_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Muid_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_phi_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_etx;   //!
   TBranch        *b_MET_MuonBoy_ety;   //!
   TBranch        *b_MET_MuonBoy_phi;   //!
   TBranch        *b_MET_MuonBoy_et;   //!
   TBranch        *b_MET_MuonBoy_sumet;   //!
   TBranch        *b_MET_MuonBoy_etx_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_ety_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_sumet_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_phi_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_etx_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_ety_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_sumet_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_phi_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_etx_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_ety_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_sumet_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_phi_ForwardReg;   //!
   TBranch        *b_MET_CellOut_Eflow_etx;   //!
   TBranch        *b_MET_CellOut_Eflow_ety;   //!
   TBranch        *b_MET_CellOut_Eflow_phi;   //!
   TBranch        *b_MET_CellOut_Eflow_et;   //!
   TBranch        *b_MET_CellOut_Eflow_sumet;   //!
   TBranch        *b_MET_CellOut_Eflow_etx_CentralReg;   //!
   TBranch        *b_MET_CellOut_Eflow_ety_CentralReg;   //!
   TBranch        *b_MET_CellOut_Eflow_sumet_CentralReg;   //!
   TBranch        *b_MET_CellOut_Eflow_phi_CentralReg;   //!
   TBranch        *b_MET_CellOut_Eflow_etx_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_Eflow_ety_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_Eflow_sumet_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_Eflow_phi_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_Eflow_etx_ForwardReg;   //!
   TBranch        *b_MET_CellOut_Eflow_ety_ForwardReg;   //!
   TBranch        *b_MET_CellOut_Eflow_sumet_ForwardReg;   //!
   TBranch        *b_MET_CellOut_Eflow_phi_ForwardReg;   //!
   TBranch        *b_MET_CellOut_MiniJet_etx;   //!
   TBranch        *b_MET_CellOut_MiniJet_ety;   //!
   TBranch        *b_MET_CellOut_MiniJet_phi;   //!
   TBranch        *b_MET_CellOut_MiniJet_et;   //!
   TBranch        *b_MET_CellOut_MiniJet_sumet;   //!
   TBranch        *b_MET_CellOut_MiniJet_etx_CentralReg;   //!
   TBranch        *b_MET_CellOut_MiniJet_ety_CentralReg;   //!
   TBranch        *b_MET_CellOut_MiniJet_sumet_CentralReg;   //!
   TBranch        *b_MET_CellOut_MiniJet_phi_CentralReg;   //!
   TBranch        *b_MET_CellOut_MiniJet_etx_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_MiniJet_ety_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_MiniJet_sumet_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_MiniJet_phi_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_MiniJet_etx_ForwardReg;   //!
   TBranch        *b_MET_CellOut_MiniJet_ety_ForwardReg;   //!
   TBranch        *b_MET_CellOut_MiniJet_sumet_ForwardReg;   //!
   TBranch        *b_MET_CellOut_MiniJet_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_etx;   //!
   TBranch        *b_MET_RefMuon_Track_ety;   //!
   TBranch        *b_MET_RefMuon_Track_phi;   //!
   TBranch        *b_MET_RefMuon_Track_et;   //!
   TBranch        *b_MET_RefMuon_Track_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_etx;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_ety;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_phi;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_et;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_etx;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_ety;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_phi;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_et;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_phi_ForwardReg;   //!
   TBranch        *b_MET_CryoCone_etx;   //!
   TBranch        *b_MET_CryoCone_ety;   //!
   TBranch        *b_MET_CryoCone_phi;   //!
   TBranch        *b_MET_CryoCone_et;   //!
   TBranch        *b_MET_CryoCone_sumet;   //!
   TBranch        *b_MET_CryoCone_etx_CentralReg;   //!
   TBranch        *b_MET_CryoCone_ety_CentralReg;   //!
   TBranch        *b_MET_CryoCone_sumet_CentralReg;   //!
   TBranch        *b_MET_CryoCone_phi_CentralReg;   //!
   TBranch        *b_MET_CryoCone_etx_EndcapRegion;   //!
   TBranch        *b_MET_CryoCone_ety_EndcapRegion;   //!
   TBranch        *b_MET_CryoCone_sumet_EndcapRegion;   //!
   TBranch        *b_MET_CryoCone_phi_EndcapRegion;   //!
   TBranch        *b_MET_CryoCone_etx_ForwardReg;   //!
   TBranch        *b_MET_CryoCone_ety_ForwardReg;   //!
   TBranch        *b_MET_CryoCone_sumet_ForwardReg;   //!
   TBranch        *b_MET_CryoCone_phi_ForwardReg;   //!
   TBranch        *b_MET_Final_etx;   //!
   TBranch        *b_MET_Final_ety;   //!
   TBranch        *b_MET_Final_phi;   //!
   TBranch        *b_MET_Final_et;   //!
   TBranch        *b_MET_Final_sumet;   //!
   TBranch        *b_MET_Final_etx_CentralReg;   //!
   TBranch        *b_MET_Final_ety_CentralReg;   //!
   TBranch        *b_MET_Final_sumet_CentralReg;   //!
   TBranch        *b_MET_Final_phi_CentralReg;   //!
   TBranch        *b_MET_Final_etx_EndcapRegion;   //!
   TBranch        *b_MET_Final_ety_EndcapRegion;   //!
   TBranch        *b_MET_Final_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Final_phi_EndcapRegion;   //!
   TBranch        *b_MET_Final_etx_ForwardReg;   //!
   TBranch        *b_MET_Final_ety_ForwardReg;   //!
   TBranch        *b_MET_Final_sumet_ForwardReg;   //!
   TBranch        *b_MET_Final_phi_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_Spectro_etx;   //!
   TBranch        *b_MET_MuonBoy_Spectro_ety;   //!
   TBranch        *b_MET_MuonBoy_Spectro_phi;   //!
   TBranch        *b_MET_MuonBoy_Spectro_et;   //!
   TBranch        *b_MET_MuonBoy_Spectro_sumet;   //!
   TBranch        *b_MET_MuonBoy_Spectro_etx_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_Spectro_ety_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_Spectro_sumet_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_Spectro_phi_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_Spectro_etx_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_Spectro_ety_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_Spectro_sumet_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_Spectro_phi_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_Spectro_etx_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_Spectro_ety_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_Spectro_sumet_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_Spectro_phi_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_Track_etx;   //!
   TBranch        *b_MET_MuonBoy_Track_ety;   //!
   TBranch        *b_MET_MuonBoy_Track_phi;   //!
   TBranch        *b_MET_MuonBoy_Track_et;   //!
   TBranch        *b_MET_MuonBoy_Track_sumet;   //!
   TBranch        *b_MET_MuonBoy_Track_etx_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_Track_ety_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_Track_sumet_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_Track_phi_CentralReg;   //!
   TBranch        *b_MET_MuonBoy_Track_etx_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_Track_ety_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_Track_sumet_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_Track_phi_EndcapRegion;   //!
   TBranch        *b_MET_MuonBoy_Track_etx_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_Track_ety_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_Track_sumet_ForwardReg;   //!
   TBranch        *b_MET_MuonBoy_Track_phi_ForwardReg;   //!
   TBranch        *b_MET_MuonMuid_etx;   //!
   TBranch        *b_MET_MuonMuid_ety;   //!
   TBranch        *b_MET_MuonMuid_phi;   //!
   TBranch        *b_MET_MuonMuid_et;   //!
   TBranch        *b_MET_MuonMuid_sumet;   //!
   TBranch        *b_MET_MuonMuid_etx_CentralReg;   //!
   TBranch        *b_MET_MuonMuid_ety_CentralReg;   //!
   TBranch        *b_MET_MuonMuid_sumet_CentralReg;   //!
   TBranch        *b_MET_MuonMuid_phi_CentralReg;   //!
   TBranch        *b_MET_MuonMuid_etx_EndcapRegion;   //!
   TBranch        *b_MET_MuonMuid_ety_EndcapRegion;   //!
   TBranch        *b_MET_MuonMuid_sumet_EndcapRegion;   //!
   TBranch        *b_MET_MuonMuid_phi_EndcapRegion;   //!
   TBranch        *b_MET_MuonMuid_etx_ForwardReg;   //!
   TBranch        *b_MET_MuonMuid_ety_ForwardReg;   //!
   TBranch        *b_MET_MuonMuid_sumet_ForwardReg;   //!
   TBranch        *b_MET_MuonMuid_phi_ForwardReg;   //!
   TBranch        *b_MET_Muid_etx;   //!
   TBranch        *b_MET_Muid_ety;   //!
   TBranch        *b_MET_Muid_phi;   //!
   TBranch        *b_MET_Muid_et;   //!
   TBranch        *b_MET_Muid_sumet;   //!
   TBranch        *b_MET_Muid_etx_CentralReg;   //!
   TBranch        *b_MET_Muid_ety_CentralReg;   //!
   TBranch        *b_MET_Muid_sumet_CentralReg;   //!
   TBranch        *b_MET_Muid_phi_CentralReg;   //!
   TBranch        *b_MET_Muid_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muid_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muid_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muid_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muid_etx_ForwardReg;   //!
   TBranch        *b_MET_Muid_ety_ForwardReg;   //!
   TBranch        *b_MET_Muid_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muid_phi_ForwardReg;   //!
   TBranch        *b_MET_Muid_Spectro_etx;   //!
   TBranch        *b_MET_Muid_Spectro_ety;   //!
   TBranch        *b_MET_Muid_Spectro_phi;   //!
   TBranch        *b_MET_Muid_Spectro_et;   //!
   TBranch        *b_MET_Muid_Spectro_sumet;   //!
   TBranch        *b_MET_Muid_Spectro_etx_CentralReg;   //!
   TBranch        *b_MET_Muid_Spectro_ety_CentralReg;   //!
   TBranch        *b_MET_Muid_Spectro_sumet_CentralReg;   //!
   TBranch        *b_MET_Muid_Spectro_phi_CentralReg;   //!
   TBranch        *b_MET_Muid_Spectro_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muid_Spectro_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muid_Spectro_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muid_Spectro_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muid_Spectro_etx_ForwardReg;   //!
   TBranch        *b_MET_Muid_Spectro_ety_ForwardReg;   //!
   TBranch        *b_MET_Muid_Spectro_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muid_Spectro_phi_ForwardReg;   //!
   TBranch        *b_MET_Muid_Track_etx;   //!
   TBranch        *b_MET_Muid_Track_ety;   //!
   TBranch        *b_MET_Muid_Track_phi;   //!
   TBranch        *b_MET_Muid_Track_et;   //!
   TBranch        *b_MET_Muid_Track_sumet;   //!
   TBranch        *b_MET_Muid_Track_etx_CentralReg;   //!
   TBranch        *b_MET_Muid_Track_ety_CentralReg;   //!
   TBranch        *b_MET_Muid_Track_sumet_CentralReg;   //!
   TBranch        *b_MET_Muid_Track_phi_CentralReg;   //!
   TBranch        *b_MET_Muid_Track_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muid_Track_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muid_Track_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muid_Track_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muid_Track_etx_ForwardReg;   //!
   TBranch        *b_MET_Muid_Track_ety_ForwardReg;   //!
   TBranch        *b_MET_Muid_Track_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muid_Track_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_etx;   //!
   TBranch        *b_MET_Muon_ety;   //!
   TBranch        *b_MET_Muon_phi;   //!
   TBranch        *b_MET_Muon_et;   //!
   TBranch        *b_MET_Muon_sumet;   //!
   TBranch        *b_MET_Muon_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_phi_ForwardReg;   //!
   TBranch        *b_MET_TopoObj_etx;   //!
   TBranch        *b_MET_TopoObj_ety;   //!
   TBranch        *b_MET_TopoObj_phi;   //!
   TBranch        *b_MET_TopoObj_et;   //!
   TBranch        *b_MET_TopoObj_sumet;   //!
   TBranch        *b_MET_TopoObj_etx_CentralReg;   //!
   TBranch        *b_MET_TopoObj_ety_CentralReg;   //!
   TBranch        *b_MET_TopoObj_sumet_CentralReg;   //!
   TBranch        *b_MET_TopoObj_phi_CentralReg;   //!
   TBranch        *b_MET_TopoObj_etx_EndcapRegion;   //!
   TBranch        *b_MET_TopoObj_ety_EndcapRegion;   //!
   TBranch        *b_MET_TopoObj_sumet_EndcapRegion;   //!
   TBranch        *b_MET_TopoObj_phi_EndcapRegion;   //!
   TBranch        *b_MET_TopoObj_etx_ForwardReg;   //!
   TBranch        *b_MET_TopoObj_ety_ForwardReg;   //!
   TBranch        *b_MET_TopoObj_sumet_ForwardReg;   //!
   TBranch        *b_MET_TopoObj_phi_ForwardReg;   //!
   TBranch        *b_MET_LocHadTopoObj_etx;   //!
   TBranch        *b_MET_LocHadTopoObj_ety;   //!
   TBranch        *b_MET_LocHadTopoObj_phi;   //!
   TBranch        *b_MET_LocHadTopoObj_et;   //!
   TBranch        *b_MET_LocHadTopoObj_sumet;   //!
   TBranch        *b_MET_LocHadTopoObj_etx_CentralReg;   //!
   TBranch        *b_MET_LocHadTopoObj_ety_CentralReg;   //!
   TBranch        *b_MET_LocHadTopoObj_sumet_CentralReg;   //!
   TBranch        *b_MET_LocHadTopoObj_phi_CentralReg;   //!
   TBranch        *b_MET_LocHadTopoObj_etx_EndcapRegion;   //!
   TBranch        *b_MET_LocHadTopoObj_ety_EndcapRegion;   //!
   TBranch        *b_MET_LocHadTopoObj_sumet_EndcapRegion;   //!
   TBranch        *b_MET_LocHadTopoObj_phi_EndcapRegion;   //!
   TBranch        *b_MET_LocHadTopoObj_etx_ForwardReg;   //!
   TBranch        *b_MET_LocHadTopoObj_ety_ForwardReg;   //!
   TBranch        *b_MET_LocHadTopoObj_sumet_ForwardReg;   //!
   TBranch        *b_MET_LocHadTopoObj_phi_ForwardReg;   //!
   TBranch        *b_MET_Topo_etx;   //!
   TBranch        *b_MET_Topo_ety;   //!
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
   TBranch        *b_MET_CorrTopo_etx;   //!
   TBranch        *b_MET_CorrTopo_ety;   //!
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
   TBranch        *b_MET_LocHadTopo_etx;   //!
   TBranch        *b_MET_LocHadTopo_ety;   //!
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
   TBranch        *b_MET_RefFinal_GCW_pt20_etx;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_ety;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_phi;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_et;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_sumet;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_etx;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_ety;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_phi;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_et;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_sumet;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_etx;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_ety;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_phi;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_et;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_sumet;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefEle_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_etx;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_ety;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_phi;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_et;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_sumet;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefTau_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_etx;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_ety;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_phi;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_et;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_sumet;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefJet_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_etx;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_ety;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_phi;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_et;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_sumet;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_etx;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_ety;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_phi;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_et;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_sumet;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Staco_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_etx;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_ety;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_phi;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_et;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_sumet;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_CellOut_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_etx;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_ety;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_phi;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_et;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_sumet;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_Cryo_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_etx;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_ety;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_phi;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_et;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_sumet;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_etx;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_ety;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_phi;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_et;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_sumet;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_etx;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_ety;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_phi;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_et;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_sumet;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_etx;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_ety;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_phi;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_et;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_sumet;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_etx;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_ety;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_phi;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_et;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_etx;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_ety;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_phi;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_et;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_GCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_et;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_et;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_et;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_etx;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_ety;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_phi;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_et;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_sumet;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefFinal_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_etx;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_ety;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_phi;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_et;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_sumet;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefGamma_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_etx;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_ety;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_phi;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_et;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_sumet;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefEle_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_etx;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_ety;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_phi;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_et;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_sumet;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefTau_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_etx;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_ety;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_phi;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_et;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_sumet;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefJet_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_etx;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_ety;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_phi;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_et;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_sumet;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_etx;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_ety;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_phi;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_et;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_sumet;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Muid_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_etx;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_ety;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_phi;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_et;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_sumet;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_CellOut_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_etx;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_ety;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_phi;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_et;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_sumet;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_Cryo_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_etx;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_ety;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_phi;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_et;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_sumet;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Isol_Muid_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_etx;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_ety;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_phi;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_et;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_sumet;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_NonIsol_Muid_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_etx;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_ety;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_phi;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_et;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_sumet;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_Muon_Total_Muid_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_etx;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_ety;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_phi;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_et;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_sumet;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_SoftJets_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_etx;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_ety;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_phi;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_et;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_etx;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_ety;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_phi;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_et;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_sumet;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_etx_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_ety_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_sumet_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_phi_CentralReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_etx_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_ety_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_sumet_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_phi_EndcapRegion;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_etx_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_ety_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_sumet_ForwardReg;   //!
   TBranch        *b_MET_RefMuon_Track_Muid_LCW_pt20_phi_ForwardReg;   //!
   TBranch        *b_L1_2J10;   //!
   TBranch        *b_L1_2J15;   //!
   TBranch        *b_L1_2J5;   //!
   TBranch        *b_L1_J10;   //!
   TBranch        *b_L1_J15;   //!
   TBranch        *b_L1_J30;   //!
   TBranch        *b_L1_J5;   //!
   TBranch        *b_L1_J55;   //!
   TBranch        *b_L1_J75;   //!
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
   TBranch        *b_trig_DB_SMK;   //!
   TBranch        *b_trig_DB_L1PSK;   //!
   TBranch        *b_trig_DB_HLTPSK;   //!

   D3PD(TTree *tree=0);
   virtual ~D3PD();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

D3PD::D3PD(TTree *tree)
{

   Init(tree);
}

D3PD::~D3PD()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t D3PD::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t D3PD::LoadTree(Long64_t entry)
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

void D3PD::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   beamSpot_x = 0;
   beamSpot_y = 0;
   beamSpot_z = 0;
   vxp_x = 0;
   vxp_y = 0;
   vxp_z = 0;
   vxp_cov_x = 0;
   vxp_cov_y = 0;
   vxp_cov_z = 0;
   vxp_cov_xy = 0;
   vxp_cov_xz = 0;
   vxp_cov_yz = 0;
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
   vxp_trk_n = 0;
   vxp_trk_index = 0;
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
   el_loose = 0;
   el_medium = 0;
   el_mediumIso = 0;
   el_tight = 0;
   el_tightIso = 0;
   el_Etcone45 = 0;
   el_Etcone20 = 0;
   el_Etcone30 = 0;
   el_Etcone40 = 0;
   el_Es2 = 0;
   el_etas2 = 0;
   el_phis2 = 0;
   el_cl_E = 0;
   el_cl_pt = 0;
   el_cl_eta = 0;
   el_cl_phi = 0;
   el_trackd0 = 0;
   el_trackz0 = 0;
   el_trackphi = 0;
   el_tracktheta = 0;
   el_trackqoverp = 0;
   el_trackpt = 0;
   el_tracketa = 0;
   el_vertx = 0;
   el_verty = 0;
   el_vertz = 0;
   mu_staco_E = 0;
   mu_staco_px = 0;
   mu_staco_py = 0;
   mu_staco_pz = 0;
   mu_staco_charge = 0;
   mu_staco_author = 0;
   mu_staco_matchchi2 = 0;
   mu_staco_matchndof = 0;
   mu_staco_etcone20 = 0;
   mu_staco_etcone30 = 0;
   mu_staco_etcone40 = 0;
   mu_staco_nucone20 = 0;
   mu_staco_nucone30 = 0;
   mu_staco_nucone40 = 0;
   mu_staco_ptcone20 = 0;
   mu_staco_ptcone30 = 0;
   mu_staco_ptcone40 = 0;
   mu_staco_bestMatch = 0;
   mu_staco_isStandAloneMuon = 0;
   mu_staco_isCombinedMuon = 0;
   mu_staco_isLowPtReconstructedMuon = 0;
   mu_staco_d0_exPV = 0;
   mu_staco_z0_exPV = 0;
   mu_staco_ms_phi = 0;
   mu_staco_ms_theta = 0;
   mu_staco_ms_qoverp = 0;
   mu_staco_id_phi = 0;
   mu_staco_id_theta = 0;
   mu_staco_id_qoverp = 0;
   jet_AntiKt4LCTopo_E = 0;
   jet_AntiKt4LCTopo_pt = 0;
   jet_AntiKt4LCTopo_m = 0;
   jet_AntiKt4LCTopo_eta = 0;
   jet_AntiKt4LCTopo_phi = 0;
   jet_AntiKt4LCTopo_n90 = 0;
   jet_AntiKt4LCTopo_Timing = 0;
   jet_AntiKt4LCTopo_LArQuality = 0;
   jet_AntiKt4LCTopo_HECQuality = 0;
   jet_AntiKt4LCTopo_BCH_CORR_CELL = 0;
   jet_AntiKt4LCTopo_BCH_CORR_DOTX = 0;
   jet_AntiKt4LCTopo_SamplingMax = 0;
   jet_AntiKt4LCTopo_fracSamplingMax = 0;
   jet_AntiKt4LCTopo_hecf = 0;
   jet_AntiKt4LCTopo_tgap3f = 0;
   jet_AntiKt4LCTopo_emfrac = 0;
   jet_AntiKt4LCTopo_EMJES = 0;
   jet_AntiKt4LCTopo_GCWJES = 0;
   jet_AntiKt4LCTopo_emscale_E = 0;
   jet_AntiKt4LCTopo_emscale_pt = 0;
   jet_AntiKt4LCTopo_emscale_m = 0;
   jet_AntiKt4LCTopo_emscale_eta = 0;
   jet_AntiKt4LCTopo_emscale_phi = 0;
   jet_AntiKt6LCTopo_E = 0;
   jet_AntiKt6LCTopo_pt = 0;
   jet_AntiKt6LCTopo_m = 0;
   jet_AntiKt6LCTopo_eta = 0;
   jet_AntiKt6LCTopo_phi = 0;
   jet_AntiKt6LCTopo_n90 = 0;
   jet_AntiKt6LCTopo_Timing = 0;
   jet_AntiKt6LCTopo_LArQuality = 0;
   jet_AntiKt6LCTopo_HECQuality = 0;
   jet_AntiKt6LCTopo_BCH_CORR_CELL = 0;
   jet_AntiKt6LCTopo_BCH_CORR_DOTX = 0;
   jet_AntiKt6LCTopo_SamplingMax = 0;
   jet_AntiKt6LCTopo_fracSamplingMax = 0;
   jet_AntiKt6LCTopo_hecf = 0;
   jet_AntiKt6LCTopo_tgap3f = 0;
   jet_AntiKt6LCTopo_emfrac = 0;
   jet_AntiKt6LCTopo_EMJES = 0;
   jet_AntiKt6LCTopo_GCWJES = 0;
   jet_AntiKt6LCTopo_emscale_E = 0;
   jet_AntiKt6LCTopo_emscale_pt = 0;
   jet_AntiKt6LCTopo_emscale_m = 0;
   jet_AntiKt6LCTopo_emscale_eta = 0;
   jet_AntiKt6LCTopo_emscale_phi = 0;
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
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
   fChain->SetBranchAddress("lbn", &lbn, &b_lbn);
   fChain->SetBranchAddress("bcid", &bcid, &b_bcid);
   fChain->SetBranchAddress("collcand_passMBTSTime", &collcand_passMBTSTime, &b_collcand_passMBTSTime);
   fChain->SetBranchAddress("collcand_passCaloTime", &collcand_passCaloTime, &b_collcand_passCaloTime);
   fChain->SetBranchAddress("collcand_passTrigger", &collcand_passTrigger, &b_collcand_passTrigger);
   fChain->SetBranchAddress("collcand_pass", &collcand_pass, &b_collcand_pass);
   fChain->SetBranchAddress("beamSpot_x", &beamSpot_x, &b_beamSpot_x);
   fChain->SetBranchAddress("beamSpot_y", &beamSpot_y, &b_beamSpot_y);
   fChain->SetBranchAddress("beamSpot_z", &beamSpot_z, &b_beamSpot_z);
   fChain->SetBranchAddress("vxp_n", &vxp_n, &b_vxp_n);
   fChain->SetBranchAddress("vxp_x", &vxp_x, &b_vxp_x);
   fChain->SetBranchAddress("vxp_y", &vxp_y, &b_vxp_y);
   fChain->SetBranchAddress("vxp_z", &vxp_z, &b_vxp_z);
   fChain->SetBranchAddress("vxp_cov_x", &vxp_cov_x, &b_vxp_cov_x);
   fChain->SetBranchAddress("vxp_cov_y", &vxp_cov_y, &b_vxp_cov_y);
   fChain->SetBranchAddress("vxp_cov_z", &vxp_cov_z, &b_vxp_cov_z);
   fChain->SetBranchAddress("vxp_cov_xy", &vxp_cov_xy, &b_vxp_cov_xy);
   fChain->SetBranchAddress("vxp_cov_xz", &vxp_cov_xz, &b_vxp_cov_xz);
   fChain->SetBranchAddress("vxp_cov_yz", &vxp_cov_yz, &b_vxp_cov_yz);
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
   fChain->SetBranchAddress("vxp_trk_n", &vxp_trk_n, &b_vxp_trk_n);
   fChain->SetBranchAddress("vxp_trk_index", &vxp_trk_index, &b_vxp_trk_index);
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
   fChain->SetBranchAddress("el_loose", &el_loose, &b_el_loose);
   fChain->SetBranchAddress("el_medium", &el_medium, &b_el_medium);
   fChain->SetBranchAddress("el_mediumIso", &el_mediumIso, &b_el_mediumIso);
   fChain->SetBranchAddress("el_tight", &el_tight, &b_el_tight);
   fChain->SetBranchAddress("el_tightIso", &el_tightIso, &b_el_tightIso);
   fChain->SetBranchAddress("el_Etcone45", &el_Etcone45, &b_el_Etcone45);
   fChain->SetBranchAddress("el_Etcone20", &el_Etcone20, &b_el_Etcone20);
   fChain->SetBranchAddress("el_Etcone30", &el_Etcone30, &b_el_Etcone30);
   fChain->SetBranchAddress("el_Etcone40", &el_Etcone40, &b_el_Etcone40);
   fChain->SetBranchAddress("el_Es2", &el_Es2, &b_el_Es2);
   fChain->SetBranchAddress("el_etas2", &el_etas2, &b_el_etas2);
   fChain->SetBranchAddress("el_phis2", &el_phis2, &b_el_phis2);
   fChain->SetBranchAddress("el_cl_E", &el_cl_E, &b_el_cl_E);
   fChain->SetBranchAddress("el_cl_pt", &el_cl_pt, &b_el_cl_pt);
   fChain->SetBranchAddress("el_cl_eta", &el_cl_eta, &b_el_cl_eta);
   fChain->SetBranchAddress("el_cl_phi", &el_cl_phi, &b_el_cl_phi);
   fChain->SetBranchAddress("el_trackd0", &el_trackd0, &b_el_trackd0);
   fChain->SetBranchAddress("el_trackz0", &el_trackz0, &b_el_trackz0);
   fChain->SetBranchAddress("el_trackphi", &el_trackphi, &b_el_trackphi);
   fChain->SetBranchAddress("el_tracktheta", &el_tracktheta, &b_el_tracktheta);
   fChain->SetBranchAddress("el_trackqoverp", &el_trackqoverp, &b_el_trackqoverp);
   fChain->SetBranchAddress("el_trackpt", &el_trackpt, &b_el_trackpt);
   fChain->SetBranchAddress("el_tracketa", &el_tracketa, &b_el_tracketa);
   fChain->SetBranchAddress("el_vertx", &el_vertx, &b_el_vertx);
   fChain->SetBranchAddress("el_verty", &el_verty, &b_el_verty);
   fChain->SetBranchAddress("el_vertz", &el_vertz, &b_el_vertz);
   fChain->SetBranchAddress("EF_e10_loose", &EF_e10_loose, &b_EF_e10_loose);
   fChain->SetBranchAddress("EF_e10_loose_passEF", &EF_e10_loose_passEF, &b_EF_e10_loose_passEF);
   fChain->SetBranchAddress("EF_e10_loose_passL2", &EF_e10_loose_passL2, &b_EF_e10_loose_passL2);
   fChain->SetBranchAddress("EF_e10_medium", &EF_e10_medium, &b_EF_e10_medium);
   fChain->SetBranchAddress("EF_e20_loose_NoIDTrkCut", &EF_e20_loose_NoIDTrkCut, &b_EF_e20_loose_NoIDTrkCut);
   fChain->SetBranchAddress("EF_mu10", &EF_mu10, &b_EF_mu10);
   fChain->SetBranchAddress("EF_mu15", &EF_mu15, &b_EF_mu15);
   fChain->SetBranchAddress("L1_EM10", &L1_EM10, &b_L1_EM10);
   fChain->SetBranchAddress("L1_EM10I", &L1_EM10I, &b_L1_EM10I);
   fChain->SetBranchAddress("L1_EM14", &L1_EM14, &b_L1_EM14);
   fChain->SetBranchAddress("L1_MU10", &L1_MU10, &b_L1_MU10);
   fChain->SetBranchAddress("L1_MU15", &L1_MU15, &b_L1_MU15);
   fChain->SetBranchAddress("L1_MU20", &L1_MU20, &b_L1_MU20);
   fChain->SetBranchAddress("L1_MU6", &L1_MU6, &b_L1_MU6);
   fChain->SetBranchAddress("L1_XE10", &L1_XE10, &b_L1_XE10);
   fChain->SetBranchAddress("L1_XE15", &L1_XE15, &b_L1_XE15);
   fChain->SetBranchAddress("L1_XE20", &L1_XE20, &b_L1_XE20);
   fChain->SetBranchAddress("L1_XE30", &L1_XE30, &b_L1_XE30);
   fChain->SetBranchAddress("mu_staco_n", &mu_staco_n, &b_mu_staco_n);
   fChain->SetBranchAddress("mu_staco_E", &mu_staco_E, &b_mu_staco_E);
   fChain->SetBranchAddress("mu_staco_px", &mu_staco_px, &b_mu_staco_px);
   fChain->SetBranchAddress("mu_staco_py", &mu_staco_py, &b_mu_staco_py);
   fChain->SetBranchAddress("mu_staco_pz", &mu_staco_pz, &b_mu_staco_pz);
   fChain->SetBranchAddress("mu_staco_charge", &mu_staco_charge, &b_mu_staco_charge);
   fChain->SetBranchAddress("mu_staco_author", &mu_staco_author, &b_mu_staco_author);
   fChain->SetBranchAddress("mu_staco_matchchi2", &mu_staco_matchchi2, &b_mu_staco_matchchi2);
   fChain->SetBranchAddress("mu_staco_matchndof", &mu_staco_matchndof, &b_mu_staco_matchndof);
   fChain->SetBranchAddress("mu_staco_etcone20", &mu_staco_etcone20, &b_mu_staco_etcone20);
   fChain->SetBranchAddress("mu_staco_etcone30", &mu_staco_etcone30, &b_mu_staco_etcone30);
   fChain->SetBranchAddress("mu_staco_etcone40", &mu_staco_etcone40, &b_mu_staco_etcone40);
   fChain->SetBranchAddress("mu_staco_nucone20", &mu_staco_nucone20, &b_mu_staco_nucone20);
   fChain->SetBranchAddress("mu_staco_nucone30", &mu_staco_nucone30, &b_mu_staco_nucone30);
   fChain->SetBranchAddress("mu_staco_nucone40", &mu_staco_nucone40, &b_mu_staco_nucone40);
   fChain->SetBranchAddress("mu_staco_ptcone20", &mu_staco_ptcone20, &b_mu_staco_ptcone20);
   fChain->SetBranchAddress("mu_staco_ptcone30", &mu_staco_ptcone30, &b_mu_staco_ptcone30);
   fChain->SetBranchAddress("mu_staco_ptcone40", &mu_staco_ptcone40, &b_mu_staco_ptcone40);
   fChain->SetBranchAddress("mu_staco_bestMatch", &mu_staco_bestMatch, &b_mu_staco_bestMatch);
   fChain->SetBranchAddress("mu_staco_isStandAloneMuon", &mu_staco_isStandAloneMuon, &b_mu_staco_isStandAloneMuon);
   fChain->SetBranchAddress("mu_staco_isCombinedMuon", &mu_staco_isCombinedMuon, &b_mu_staco_isCombinedMuon);
   fChain->SetBranchAddress("mu_staco_isLowPtReconstructedMuon", &mu_staco_isLowPtReconstructedMuon, &b_mu_staco_isLowPtReconstructedMuon);
   fChain->SetBranchAddress("mu_staco_d0_exPV", &mu_staco_d0_exPV, &b_mu_staco_d0_exPV);
   fChain->SetBranchAddress("mu_staco_z0_exPV", &mu_staco_z0_exPV, &b_mu_staco_z0_exPV);
   fChain->SetBranchAddress("mu_staco_ms_phi", &mu_staco_ms_phi, &b_mu_staco_ms_phi);
   fChain->SetBranchAddress("mu_staco_ms_theta", &mu_staco_ms_theta, &b_mu_staco_ms_theta);
   fChain->SetBranchAddress("mu_staco_ms_qoverp", &mu_staco_ms_qoverp, &b_mu_staco_ms_qoverp);
   fChain->SetBranchAddress("mu_staco_id_phi", &mu_staco_id_phi, &b_mu_staco_id_phi);
   fChain->SetBranchAddress("mu_staco_id_theta", &mu_staco_id_theta, &b_mu_staco_id_theta);
   fChain->SetBranchAddress("mu_staco_id_qoverp", &mu_staco_id_qoverp, &b_mu_staco_id_qoverp);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_n", &jet_AntiKt4LCTopo_n, &b_jet_AntiKt4LCTopo_n);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_E", &jet_AntiKt4LCTopo_E, &b_jet_AntiKt4LCTopo_E);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_pt", &jet_AntiKt4LCTopo_pt, &b_jet_AntiKt4LCTopo_pt);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_m", &jet_AntiKt4LCTopo_m, &b_jet_AntiKt4LCTopo_m);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_eta", &jet_AntiKt4LCTopo_eta, &b_jet_AntiKt4LCTopo_eta);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_phi", &jet_AntiKt4LCTopo_phi, &b_jet_AntiKt4LCTopo_phi);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_n90", &jet_AntiKt4LCTopo_n90, &b_jet_AntiKt4LCTopo_n90);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_Timing", &jet_AntiKt4LCTopo_Timing, &b_jet_AntiKt4LCTopo_Timing);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_LArQuality", &jet_AntiKt4LCTopo_LArQuality, &b_jet_AntiKt4LCTopo_LArQuality);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_HECQuality", &jet_AntiKt4LCTopo_HECQuality, &b_jet_AntiKt4LCTopo_HECQuality);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_BCH_CORR_CELL", &jet_AntiKt4LCTopo_BCH_CORR_CELL, &b_jet_AntiKt4LCTopo_BCH_CORR_CELL);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_BCH_CORR_DOTX", &jet_AntiKt4LCTopo_BCH_CORR_DOTX, &b_jet_AntiKt4LCTopo_BCH_CORR_DOTX);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_SamplingMax", &jet_AntiKt4LCTopo_SamplingMax, &b_jet_AntiKt4LCTopo_SamplingMax);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_fracSamplingMax", &jet_AntiKt4LCTopo_fracSamplingMax, &b_jet_AntiKt4LCTopo_fracSamplingMax);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_hecf", &jet_AntiKt4LCTopo_hecf, &b_jet_AntiKt4LCTopo_hecf);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_tgap3f", &jet_AntiKt4LCTopo_tgap3f, &b_jet_AntiKt4LCTopo_tgap3f);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_emfrac", &jet_AntiKt4LCTopo_emfrac, &b_jet_AntiKt4LCTopo_emfrac);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_EMJES", &jet_AntiKt4LCTopo_EMJES, &b_jet_AntiKt4LCTopo_EMJES);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_GCWJES", &jet_AntiKt4LCTopo_GCWJES, &b_jet_AntiKt4LCTopo_GCWJES);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_emscale_E", &jet_AntiKt4LCTopo_emscale_E, &b_jet_AntiKt4LCTopo_emscale_E);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_emscale_pt", &jet_AntiKt4LCTopo_emscale_pt, &b_jet_AntiKt4LCTopo_emscale_pt);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_emscale_m", &jet_AntiKt4LCTopo_emscale_m, &b_jet_AntiKt4LCTopo_emscale_m);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_emscale_eta", &jet_AntiKt4LCTopo_emscale_eta, &b_jet_AntiKt4LCTopo_emscale_eta);
   fChain->SetBranchAddress("jet_AntiKt4LCTopo_emscale_phi", &jet_AntiKt4LCTopo_emscale_phi, &b_jet_AntiKt4LCTopo_emscale_phi);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_n", &jet_AntiKt6LCTopo_n, &b_jet_AntiKt6LCTopo_n);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_E", &jet_AntiKt6LCTopo_E, &b_jet_AntiKt6LCTopo_E);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_pt", &jet_AntiKt6LCTopo_pt, &b_jet_AntiKt6LCTopo_pt);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_m", &jet_AntiKt6LCTopo_m, &b_jet_AntiKt6LCTopo_m);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_eta", &jet_AntiKt6LCTopo_eta, &b_jet_AntiKt6LCTopo_eta);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_phi", &jet_AntiKt6LCTopo_phi, &b_jet_AntiKt6LCTopo_phi);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_n90", &jet_AntiKt6LCTopo_n90, &b_jet_AntiKt6LCTopo_n90);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_Timing", &jet_AntiKt6LCTopo_Timing, &b_jet_AntiKt6LCTopo_Timing);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_LArQuality", &jet_AntiKt6LCTopo_LArQuality, &b_jet_AntiKt6LCTopo_LArQuality);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_HECQuality", &jet_AntiKt6LCTopo_HECQuality, &b_jet_AntiKt6LCTopo_HECQuality);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_BCH_CORR_CELL", &jet_AntiKt6LCTopo_BCH_CORR_CELL, &b_jet_AntiKt6LCTopo_BCH_CORR_CELL);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_BCH_CORR_DOTX", &jet_AntiKt6LCTopo_BCH_CORR_DOTX, &b_jet_AntiKt6LCTopo_BCH_CORR_DOTX);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_SamplingMax", &jet_AntiKt6LCTopo_SamplingMax, &b_jet_AntiKt6LCTopo_SamplingMax);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_fracSamplingMax", &jet_AntiKt6LCTopo_fracSamplingMax, &b_jet_AntiKt6LCTopo_fracSamplingMax);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_hecf", &jet_AntiKt6LCTopo_hecf, &b_jet_AntiKt6LCTopo_hecf);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_tgap3f", &jet_AntiKt6LCTopo_tgap3f, &b_jet_AntiKt6LCTopo_tgap3f);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_emfrac", &jet_AntiKt6LCTopo_emfrac, &b_jet_AntiKt6LCTopo_emfrac);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_EMJES", &jet_AntiKt6LCTopo_EMJES, &b_jet_AntiKt6LCTopo_EMJES);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_GCWJES", &jet_AntiKt6LCTopo_GCWJES, &b_jet_AntiKt6LCTopo_GCWJES);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_emscale_E", &jet_AntiKt6LCTopo_emscale_E, &b_jet_AntiKt6LCTopo_emscale_E);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_emscale_pt", &jet_AntiKt6LCTopo_emscale_pt, &b_jet_AntiKt6LCTopo_emscale_pt);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_emscale_m", &jet_AntiKt6LCTopo_emscale_m, &b_jet_AntiKt6LCTopo_emscale_m);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_emscale_eta", &jet_AntiKt6LCTopo_emscale_eta, &b_jet_AntiKt6LCTopo_emscale_eta);
   fChain->SetBranchAddress("jet_AntiKt6LCTopo_emscale_phi", &jet_AntiKt6LCTopo_emscale_phi, &b_jet_AntiKt6LCTopo_emscale_phi);
   fChain->SetBranchAddress("MET_RefFinal_etx", &MET_RefFinal_etx, &b_MET_RefFinal_etx);
   fChain->SetBranchAddress("MET_RefFinal_ety", &MET_RefFinal_ety, &b_MET_RefFinal_ety);
   fChain->SetBranchAddress("MET_RefFinal_phi", &MET_RefFinal_phi, &b_MET_RefFinal_phi);
   fChain->SetBranchAddress("MET_RefFinal_et", &MET_RefFinal_et, &b_MET_RefFinal_et);
   fChain->SetBranchAddress("MET_RefFinal_sumet", &MET_RefFinal_sumet, &b_MET_RefFinal_sumet);
   fChain->SetBranchAddress("MET_RefFinal_etx_CentralReg", &MET_RefFinal_etx_CentralReg, &b_MET_RefFinal_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_ety_CentralReg", &MET_RefFinal_ety_CentralReg, &b_MET_RefFinal_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_sumet_CentralReg", &MET_RefFinal_sumet_CentralReg, &b_MET_RefFinal_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_phi_CentralReg", &MET_RefFinal_phi_CentralReg, &b_MET_RefFinal_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_etx_EndcapRegion", &MET_RefFinal_etx_EndcapRegion, &b_MET_RefFinal_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_ety_EndcapRegion", &MET_RefFinal_ety_EndcapRegion, &b_MET_RefFinal_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_sumet_EndcapRegion", &MET_RefFinal_sumet_EndcapRegion, &b_MET_RefFinal_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_phi_EndcapRegion", &MET_RefFinal_phi_EndcapRegion, &b_MET_RefFinal_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_etx_ForwardReg", &MET_RefFinal_etx_ForwardReg, &b_MET_RefFinal_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_ety_ForwardReg", &MET_RefFinal_ety_ForwardReg, &b_MET_RefFinal_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_sumet_ForwardReg", &MET_RefFinal_sumet_ForwardReg, &b_MET_RefFinal_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_phi_ForwardReg", &MET_RefFinal_phi_ForwardReg, &b_MET_RefFinal_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_etx", &MET_Cryo_etx, &b_MET_Cryo_etx);
   fChain->SetBranchAddress("MET_Cryo_ety", &MET_Cryo_ety, &b_MET_Cryo_ety);
   fChain->SetBranchAddress("MET_Cryo_phi", &MET_Cryo_phi, &b_MET_Cryo_phi);
   fChain->SetBranchAddress("MET_Cryo_et", &MET_Cryo_et, &b_MET_Cryo_et);
   fChain->SetBranchAddress("MET_Cryo_sumet", &MET_Cryo_sumet, &b_MET_Cryo_sumet);
   fChain->SetBranchAddress("MET_Cryo_etx_CentralReg", &MET_Cryo_etx_CentralReg, &b_MET_Cryo_etx_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_ety_CentralReg", &MET_Cryo_ety_CentralReg, &b_MET_Cryo_ety_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_sumet_CentralReg", &MET_Cryo_sumet_CentralReg, &b_MET_Cryo_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_phi_CentralReg", &MET_Cryo_phi_CentralReg, &b_MET_Cryo_phi_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_etx_EndcapRegion", &MET_Cryo_etx_EndcapRegion, &b_MET_Cryo_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_ety_EndcapRegion", &MET_Cryo_ety_EndcapRegion, &b_MET_Cryo_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_sumet_EndcapRegion", &MET_Cryo_sumet_EndcapRegion, &b_MET_Cryo_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_phi_EndcapRegion", &MET_Cryo_phi_EndcapRegion, &b_MET_Cryo_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_etx_ForwardReg", &MET_Cryo_etx_ForwardReg, &b_MET_Cryo_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_ety_ForwardReg", &MET_Cryo_ety_ForwardReg, &b_MET_Cryo_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_sumet_ForwardReg", &MET_Cryo_sumet_ForwardReg, &b_MET_Cryo_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_phi_ForwardReg", &MET_Cryo_phi_ForwardReg, &b_MET_Cryo_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_etx", &MET_RefEle_etx, &b_MET_RefEle_etx);
   fChain->SetBranchAddress("MET_RefEle_ety", &MET_RefEle_ety, &b_MET_RefEle_ety);
   fChain->SetBranchAddress("MET_RefEle_phi", &MET_RefEle_phi, &b_MET_RefEle_phi);
   fChain->SetBranchAddress("MET_RefEle_et", &MET_RefEle_et, &b_MET_RefEle_et);
   fChain->SetBranchAddress("MET_RefEle_sumet", &MET_RefEle_sumet, &b_MET_RefEle_sumet);
   fChain->SetBranchAddress("MET_RefEle_etx_CentralReg", &MET_RefEle_etx_CentralReg, &b_MET_RefEle_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_ety_CentralReg", &MET_RefEle_ety_CentralReg, &b_MET_RefEle_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_sumet_CentralReg", &MET_RefEle_sumet_CentralReg, &b_MET_RefEle_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_phi_CentralReg", &MET_RefEle_phi_CentralReg, &b_MET_RefEle_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_etx_EndcapRegion", &MET_RefEle_etx_EndcapRegion, &b_MET_RefEle_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_ety_EndcapRegion", &MET_RefEle_ety_EndcapRegion, &b_MET_RefEle_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_sumet_EndcapRegion", &MET_RefEle_sumet_EndcapRegion, &b_MET_RefEle_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_phi_EndcapRegion", &MET_RefEle_phi_EndcapRegion, &b_MET_RefEle_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_etx_ForwardReg", &MET_RefEle_etx_ForwardReg, &b_MET_RefEle_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_ety_ForwardReg", &MET_RefEle_ety_ForwardReg, &b_MET_RefEle_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_sumet_ForwardReg", &MET_RefEle_sumet_ForwardReg, &b_MET_RefEle_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_phi_ForwardReg", &MET_RefEle_phi_ForwardReg, &b_MET_RefEle_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_etx", &MET_RefJet_etx, &b_MET_RefJet_etx);
   fChain->SetBranchAddress("MET_RefJet_ety", &MET_RefJet_ety, &b_MET_RefJet_ety);
   fChain->SetBranchAddress("MET_RefJet_phi", &MET_RefJet_phi, &b_MET_RefJet_phi);
   fChain->SetBranchAddress("MET_RefJet_et", &MET_RefJet_et, &b_MET_RefJet_et);
   fChain->SetBranchAddress("MET_RefJet_sumet", &MET_RefJet_sumet, &b_MET_RefJet_sumet);
   fChain->SetBranchAddress("MET_RefJet_etx_CentralReg", &MET_RefJet_etx_CentralReg, &b_MET_RefJet_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_ety_CentralReg", &MET_RefJet_ety_CentralReg, &b_MET_RefJet_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_sumet_CentralReg", &MET_RefJet_sumet_CentralReg, &b_MET_RefJet_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_phi_CentralReg", &MET_RefJet_phi_CentralReg, &b_MET_RefJet_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_etx_EndcapRegion", &MET_RefJet_etx_EndcapRegion, &b_MET_RefJet_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_ety_EndcapRegion", &MET_RefJet_ety_EndcapRegion, &b_MET_RefJet_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_sumet_EndcapRegion", &MET_RefJet_sumet_EndcapRegion, &b_MET_RefJet_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_phi_EndcapRegion", &MET_RefJet_phi_EndcapRegion, &b_MET_RefJet_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_etx_ForwardReg", &MET_RefJet_etx_ForwardReg, &b_MET_RefJet_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_ety_ForwardReg", &MET_RefJet_ety_ForwardReg, &b_MET_RefJet_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_sumet_ForwardReg", &MET_RefJet_sumet_ForwardReg, &b_MET_RefJet_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_phi_ForwardReg", &MET_RefJet_phi_ForwardReg, &b_MET_RefJet_phi_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_etx", &MET_SoftJets_etx, &b_MET_SoftJets_etx);
   fChain->SetBranchAddress("MET_SoftJets_ety", &MET_SoftJets_ety, &b_MET_SoftJets_ety);
   fChain->SetBranchAddress("MET_SoftJets_phi", &MET_SoftJets_phi, &b_MET_SoftJets_phi);
   fChain->SetBranchAddress("MET_SoftJets_et", &MET_SoftJets_et, &b_MET_SoftJets_et);
   fChain->SetBranchAddress("MET_SoftJets_sumet", &MET_SoftJets_sumet, &b_MET_SoftJets_sumet);
   fChain->SetBranchAddress("MET_SoftJets_etx_CentralReg", &MET_SoftJets_etx_CentralReg, &b_MET_SoftJets_etx_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_ety_CentralReg", &MET_SoftJets_ety_CentralReg, &b_MET_SoftJets_ety_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_sumet_CentralReg", &MET_SoftJets_sumet_CentralReg, &b_MET_SoftJets_sumet_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_phi_CentralReg", &MET_SoftJets_phi_CentralReg, &b_MET_SoftJets_phi_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_etx_EndcapRegion", &MET_SoftJets_etx_EndcapRegion, &b_MET_SoftJets_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_ety_EndcapRegion", &MET_SoftJets_ety_EndcapRegion, &b_MET_SoftJets_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_sumet_EndcapRegion", &MET_SoftJets_sumet_EndcapRegion, &b_MET_SoftJets_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_phi_EndcapRegion", &MET_SoftJets_phi_EndcapRegion, &b_MET_SoftJets_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_etx_ForwardReg", &MET_SoftJets_etx_ForwardReg, &b_MET_SoftJets_etx_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_ety_ForwardReg", &MET_SoftJets_ety_ForwardReg, &b_MET_SoftJets_ety_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_sumet_ForwardReg", &MET_SoftJets_sumet_ForwardReg, &b_MET_SoftJets_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_phi_ForwardReg", &MET_SoftJets_phi_ForwardReg, &b_MET_SoftJets_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_etx", &MET_RefMuon_etx, &b_MET_RefMuon_etx);
   fChain->SetBranchAddress("MET_RefMuon_ety", &MET_RefMuon_ety, &b_MET_RefMuon_ety);
   fChain->SetBranchAddress("MET_RefMuon_phi", &MET_RefMuon_phi, &b_MET_RefMuon_phi);
   fChain->SetBranchAddress("MET_RefMuon_et", &MET_RefMuon_et, &b_MET_RefMuon_et);
   fChain->SetBranchAddress("MET_RefMuon_sumet", &MET_RefMuon_sumet, &b_MET_RefMuon_sumet);
   fChain->SetBranchAddress("MET_RefMuon_etx_CentralReg", &MET_RefMuon_etx_CentralReg, &b_MET_RefMuon_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_ety_CentralReg", &MET_RefMuon_ety_CentralReg, &b_MET_RefMuon_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_sumet_CentralReg", &MET_RefMuon_sumet_CentralReg, &b_MET_RefMuon_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_phi_CentralReg", &MET_RefMuon_phi_CentralReg, &b_MET_RefMuon_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_etx_EndcapRegion", &MET_RefMuon_etx_EndcapRegion, &b_MET_RefMuon_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_ety_EndcapRegion", &MET_RefMuon_ety_EndcapRegion, &b_MET_RefMuon_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_sumet_EndcapRegion", &MET_RefMuon_sumet_EndcapRegion, &b_MET_RefMuon_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_phi_EndcapRegion", &MET_RefMuon_phi_EndcapRegion, &b_MET_RefMuon_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_etx_ForwardReg", &MET_RefMuon_etx_ForwardReg, &b_MET_RefMuon_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_ety_ForwardReg", &MET_RefMuon_ety_ForwardReg, &b_MET_RefMuon_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_sumet_ForwardReg", &MET_RefMuon_sumet_ForwardReg, &b_MET_RefMuon_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_phi_ForwardReg", &MET_RefMuon_phi_ForwardReg, &b_MET_RefMuon_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_etx", &MET_RefMuon_Staco_etx, &b_MET_RefMuon_Staco_etx);
   fChain->SetBranchAddress("MET_RefMuon_Staco_ety", &MET_RefMuon_Staco_ety, &b_MET_RefMuon_Staco_ety);
   fChain->SetBranchAddress("MET_RefMuon_Staco_phi", &MET_RefMuon_Staco_phi, &b_MET_RefMuon_Staco_phi);
   fChain->SetBranchAddress("MET_RefMuon_Staco_et", &MET_RefMuon_Staco_et, &b_MET_RefMuon_Staco_et);
   fChain->SetBranchAddress("MET_RefMuon_Staco_sumet", &MET_RefMuon_Staco_sumet, &b_MET_RefMuon_Staco_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Staco_etx_CentralReg", &MET_RefMuon_Staco_etx_CentralReg, &b_MET_RefMuon_Staco_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_ety_CentralReg", &MET_RefMuon_Staco_ety_CentralReg, &b_MET_RefMuon_Staco_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_sumet_CentralReg", &MET_RefMuon_Staco_sumet_CentralReg, &b_MET_RefMuon_Staco_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_phi_CentralReg", &MET_RefMuon_Staco_phi_CentralReg, &b_MET_RefMuon_Staco_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_etx_EndcapRegion", &MET_RefMuon_Staco_etx_EndcapRegion, &b_MET_RefMuon_Staco_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_ety_EndcapRegion", &MET_RefMuon_Staco_ety_EndcapRegion, &b_MET_RefMuon_Staco_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_sumet_EndcapRegion", &MET_RefMuon_Staco_sumet_EndcapRegion, &b_MET_RefMuon_Staco_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_phi_EndcapRegion", &MET_RefMuon_Staco_phi_EndcapRegion, &b_MET_RefMuon_Staco_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_etx_ForwardReg", &MET_RefMuon_Staco_etx_ForwardReg, &b_MET_RefMuon_Staco_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_ety_ForwardReg", &MET_RefMuon_Staco_ety_ForwardReg, &b_MET_RefMuon_Staco_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_sumet_ForwardReg", &MET_RefMuon_Staco_sumet_ForwardReg, &b_MET_RefMuon_Staco_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_phi_ForwardReg", &MET_RefMuon_Staco_phi_ForwardReg, &b_MET_RefMuon_Staco_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_etx", &MET_RefMuon_Muid_etx, &b_MET_RefMuon_Muid_etx);
   fChain->SetBranchAddress("MET_RefMuon_Muid_ety", &MET_RefMuon_Muid_ety, &b_MET_RefMuon_Muid_ety);
   fChain->SetBranchAddress("MET_RefMuon_Muid_phi", &MET_RefMuon_Muid_phi, &b_MET_RefMuon_Muid_phi);
   fChain->SetBranchAddress("MET_RefMuon_Muid_et", &MET_RefMuon_Muid_et, &b_MET_RefMuon_Muid_et);
   fChain->SetBranchAddress("MET_RefMuon_Muid_sumet", &MET_RefMuon_Muid_sumet, &b_MET_RefMuon_Muid_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Muid_etx_CentralReg", &MET_RefMuon_Muid_etx_CentralReg, &b_MET_RefMuon_Muid_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_ety_CentralReg", &MET_RefMuon_Muid_ety_CentralReg, &b_MET_RefMuon_Muid_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_sumet_CentralReg", &MET_RefMuon_Muid_sumet_CentralReg, &b_MET_RefMuon_Muid_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_phi_CentralReg", &MET_RefMuon_Muid_phi_CentralReg, &b_MET_RefMuon_Muid_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_etx_EndcapRegion", &MET_RefMuon_Muid_etx_EndcapRegion, &b_MET_RefMuon_Muid_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Muid_ety_EndcapRegion", &MET_RefMuon_Muid_ety_EndcapRegion, &b_MET_RefMuon_Muid_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Muid_sumet_EndcapRegion", &MET_RefMuon_Muid_sumet_EndcapRegion, &b_MET_RefMuon_Muid_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Muid_phi_EndcapRegion", &MET_RefMuon_Muid_phi_EndcapRegion, &b_MET_RefMuon_Muid_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Muid_etx_ForwardReg", &MET_RefMuon_Muid_etx_ForwardReg, &b_MET_RefMuon_Muid_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_ety_ForwardReg", &MET_RefMuon_Muid_ety_ForwardReg, &b_MET_RefMuon_Muid_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_sumet_ForwardReg", &MET_RefMuon_Muid_sumet_ForwardReg, &b_MET_RefMuon_Muid_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_phi_ForwardReg", &MET_RefMuon_Muid_phi_ForwardReg, &b_MET_RefMuon_Muid_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_etx", &MET_RefGamma_etx, &b_MET_RefGamma_etx);
   fChain->SetBranchAddress("MET_RefGamma_ety", &MET_RefGamma_ety, &b_MET_RefGamma_ety);
   fChain->SetBranchAddress("MET_RefGamma_phi", &MET_RefGamma_phi, &b_MET_RefGamma_phi);
   fChain->SetBranchAddress("MET_RefGamma_et", &MET_RefGamma_et, &b_MET_RefGamma_et);
   fChain->SetBranchAddress("MET_RefGamma_sumet", &MET_RefGamma_sumet, &b_MET_RefGamma_sumet);
   fChain->SetBranchAddress("MET_RefGamma_etx_CentralReg", &MET_RefGamma_etx_CentralReg, &b_MET_RefGamma_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_ety_CentralReg", &MET_RefGamma_ety_CentralReg, &b_MET_RefGamma_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_sumet_CentralReg", &MET_RefGamma_sumet_CentralReg, &b_MET_RefGamma_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_phi_CentralReg", &MET_RefGamma_phi_CentralReg, &b_MET_RefGamma_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_etx_EndcapRegion", &MET_RefGamma_etx_EndcapRegion, &b_MET_RefGamma_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_ety_EndcapRegion", &MET_RefGamma_ety_EndcapRegion, &b_MET_RefGamma_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_sumet_EndcapRegion", &MET_RefGamma_sumet_EndcapRegion, &b_MET_RefGamma_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_phi_EndcapRegion", &MET_RefGamma_phi_EndcapRegion, &b_MET_RefGamma_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_etx_ForwardReg", &MET_RefGamma_etx_ForwardReg, &b_MET_RefGamma_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_ety_ForwardReg", &MET_RefGamma_ety_ForwardReg, &b_MET_RefGamma_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_sumet_ForwardReg", &MET_RefGamma_sumet_ForwardReg, &b_MET_RefGamma_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_phi_ForwardReg", &MET_RefGamma_phi_ForwardReg, &b_MET_RefGamma_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_etx", &MET_RefTau_etx, &b_MET_RefTau_etx);
   fChain->SetBranchAddress("MET_RefTau_ety", &MET_RefTau_ety, &b_MET_RefTau_ety);
   fChain->SetBranchAddress("MET_RefTau_phi", &MET_RefTau_phi, &b_MET_RefTau_phi);
   fChain->SetBranchAddress("MET_RefTau_et", &MET_RefTau_et, &b_MET_RefTau_et);
   fChain->SetBranchAddress("MET_RefTau_sumet", &MET_RefTau_sumet, &b_MET_RefTau_sumet);
   fChain->SetBranchAddress("MET_RefTau_etx_CentralReg", &MET_RefTau_etx_CentralReg, &b_MET_RefTau_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_ety_CentralReg", &MET_RefTau_ety_CentralReg, &b_MET_RefTau_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_sumet_CentralReg", &MET_RefTau_sumet_CentralReg, &b_MET_RefTau_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_phi_CentralReg", &MET_RefTau_phi_CentralReg, &b_MET_RefTau_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_etx_EndcapRegion", &MET_RefTau_etx_EndcapRegion, &b_MET_RefTau_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_ety_EndcapRegion", &MET_RefTau_ety_EndcapRegion, &b_MET_RefTau_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_sumet_EndcapRegion", &MET_RefTau_sumet_EndcapRegion, &b_MET_RefTau_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_phi_EndcapRegion", &MET_RefTau_phi_EndcapRegion, &b_MET_RefTau_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_etx_ForwardReg", &MET_RefTau_etx_ForwardReg, &b_MET_RefTau_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_ety_ForwardReg", &MET_RefTau_ety_ForwardReg, &b_MET_RefTau_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_sumet_ForwardReg", &MET_RefTau_sumet_ForwardReg, &b_MET_RefTau_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_phi_ForwardReg", &MET_RefTau_phi_ForwardReg, &b_MET_RefTau_phi_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_etx", &MET_CellOut_etx, &b_MET_CellOut_etx);
   fChain->SetBranchAddress("MET_CellOut_ety", &MET_CellOut_ety, &b_MET_CellOut_ety);
   fChain->SetBranchAddress("MET_CellOut_phi", &MET_CellOut_phi, &b_MET_CellOut_phi);
   fChain->SetBranchAddress("MET_CellOut_et", &MET_CellOut_et, &b_MET_CellOut_et);
   fChain->SetBranchAddress("MET_CellOut_sumet", &MET_CellOut_sumet, &b_MET_CellOut_sumet);
   fChain->SetBranchAddress("MET_CellOut_etx_CentralReg", &MET_CellOut_etx_CentralReg, &b_MET_CellOut_etx_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_ety_CentralReg", &MET_CellOut_ety_CentralReg, &b_MET_CellOut_ety_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_sumet_CentralReg", &MET_CellOut_sumet_CentralReg, &b_MET_CellOut_sumet_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_phi_CentralReg", &MET_CellOut_phi_CentralReg, &b_MET_CellOut_phi_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_etx_EndcapRegion", &MET_CellOut_etx_EndcapRegion, &b_MET_CellOut_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_ety_EndcapRegion", &MET_CellOut_ety_EndcapRegion, &b_MET_CellOut_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_sumet_EndcapRegion", &MET_CellOut_sumet_EndcapRegion, &b_MET_CellOut_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_phi_EndcapRegion", &MET_CellOut_phi_EndcapRegion, &b_MET_CellOut_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_etx_ForwardReg", &MET_CellOut_etx_ForwardReg, &b_MET_CellOut_etx_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_ety_ForwardReg", &MET_CellOut_ety_ForwardReg, &b_MET_CellOut_ety_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_sumet_ForwardReg", &MET_CellOut_sumet_ForwardReg, &b_MET_CellOut_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_phi_ForwardReg", &MET_CellOut_phi_ForwardReg, &b_MET_CellOut_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Track_etx", &MET_Track_etx, &b_MET_Track_etx);
   fChain->SetBranchAddress("MET_Track_ety", &MET_Track_ety, &b_MET_Track_ety);
   fChain->SetBranchAddress("MET_Track_phi", &MET_Track_phi, &b_MET_Track_phi);
   fChain->SetBranchAddress("MET_Track_et", &MET_Track_et, &b_MET_Track_et);
   fChain->SetBranchAddress("MET_Track_sumet", &MET_Track_sumet, &b_MET_Track_sumet);
   fChain->SetBranchAddress("MET_Track_etx_CentralReg", &MET_Track_etx_CentralReg, &b_MET_Track_etx_CentralReg);
   fChain->SetBranchAddress("MET_Track_ety_CentralReg", &MET_Track_ety_CentralReg, &b_MET_Track_ety_CentralReg);
   fChain->SetBranchAddress("MET_Track_sumet_CentralReg", &MET_Track_sumet_CentralReg, &b_MET_Track_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Track_phi_CentralReg", &MET_Track_phi_CentralReg, &b_MET_Track_phi_CentralReg);
   fChain->SetBranchAddress("MET_Track_etx_EndcapRegion", &MET_Track_etx_EndcapRegion, &b_MET_Track_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Track_ety_EndcapRegion", &MET_Track_ety_EndcapRegion, &b_MET_Track_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Track_sumet_EndcapRegion", &MET_Track_sumet_EndcapRegion, &b_MET_Track_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Track_phi_EndcapRegion", &MET_Track_phi_EndcapRegion, &b_MET_Track_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Track_etx_ForwardReg", &MET_Track_etx_ForwardReg, &b_MET_Track_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Track_ety_ForwardReg", &MET_Track_ety_ForwardReg, &b_MET_Track_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Track_sumet_ForwardReg", &MET_Track_sumet_ForwardReg, &b_MET_Track_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Track_phi_ForwardReg", &MET_Track_phi_ForwardReg, &b_MET_Track_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_em_etx", &MET_RefFinal_em_etx, &b_MET_RefFinal_em_etx);
   fChain->SetBranchAddress("MET_RefFinal_em_ety", &MET_RefFinal_em_ety, &b_MET_RefFinal_em_ety);
   fChain->SetBranchAddress("MET_RefFinal_em_phi", &MET_RefFinal_em_phi, &b_MET_RefFinal_em_phi);
   fChain->SetBranchAddress("MET_RefFinal_em_et", &MET_RefFinal_em_et, &b_MET_RefFinal_em_et);
   fChain->SetBranchAddress("MET_RefFinal_em_sumet", &MET_RefFinal_em_sumet, &b_MET_RefFinal_em_sumet);
   fChain->SetBranchAddress("MET_RefFinal_em_etx_CentralReg", &MET_RefFinal_em_etx_CentralReg, &b_MET_RefFinal_em_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_em_ety_CentralReg", &MET_RefFinal_em_ety_CentralReg, &b_MET_RefFinal_em_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_em_sumet_CentralReg", &MET_RefFinal_em_sumet_CentralReg, &b_MET_RefFinal_em_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_em_phi_CentralReg", &MET_RefFinal_em_phi_CentralReg, &b_MET_RefFinal_em_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_em_etx_EndcapRegion", &MET_RefFinal_em_etx_EndcapRegion, &b_MET_RefFinal_em_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_em_ety_EndcapRegion", &MET_RefFinal_em_ety_EndcapRegion, &b_MET_RefFinal_em_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_em_sumet_EndcapRegion", &MET_RefFinal_em_sumet_EndcapRegion, &b_MET_RefFinal_em_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_em_phi_EndcapRegion", &MET_RefFinal_em_phi_EndcapRegion, &b_MET_RefFinal_em_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_em_etx_ForwardReg", &MET_RefFinal_em_etx_ForwardReg, &b_MET_RefFinal_em_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_em_ety_ForwardReg", &MET_RefFinal_em_ety_ForwardReg, &b_MET_RefFinal_em_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_em_sumet_ForwardReg", &MET_RefFinal_em_sumet_ForwardReg, &b_MET_RefFinal_em_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_em_phi_ForwardReg", &MET_RefFinal_em_phi_ForwardReg, &b_MET_RefFinal_em_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_em_etx", &MET_RefEle_em_etx, &b_MET_RefEle_em_etx);
   fChain->SetBranchAddress("MET_RefEle_em_ety", &MET_RefEle_em_ety, &b_MET_RefEle_em_ety);
   fChain->SetBranchAddress("MET_RefEle_em_phi", &MET_RefEle_em_phi, &b_MET_RefEle_em_phi);
   fChain->SetBranchAddress("MET_RefEle_em_et", &MET_RefEle_em_et, &b_MET_RefEle_em_et);
   fChain->SetBranchAddress("MET_RefEle_em_sumet", &MET_RefEle_em_sumet, &b_MET_RefEle_em_sumet);
   fChain->SetBranchAddress("MET_RefEle_em_etx_CentralReg", &MET_RefEle_em_etx_CentralReg, &b_MET_RefEle_em_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_em_ety_CentralReg", &MET_RefEle_em_ety_CentralReg, &b_MET_RefEle_em_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_em_sumet_CentralReg", &MET_RefEle_em_sumet_CentralReg, &b_MET_RefEle_em_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_em_phi_CentralReg", &MET_RefEle_em_phi_CentralReg, &b_MET_RefEle_em_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_em_etx_EndcapRegion", &MET_RefEle_em_etx_EndcapRegion, &b_MET_RefEle_em_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_em_ety_EndcapRegion", &MET_RefEle_em_ety_EndcapRegion, &b_MET_RefEle_em_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_em_sumet_EndcapRegion", &MET_RefEle_em_sumet_EndcapRegion, &b_MET_RefEle_em_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_em_phi_EndcapRegion", &MET_RefEle_em_phi_EndcapRegion, &b_MET_RefEle_em_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_em_etx_ForwardReg", &MET_RefEle_em_etx_ForwardReg, &b_MET_RefEle_em_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_em_ety_ForwardReg", &MET_RefEle_em_ety_ForwardReg, &b_MET_RefEle_em_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_em_sumet_ForwardReg", &MET_RefEle_em_sumet_ForwardReg, &b_MET_RefEle_em_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_em_phi_ForwardReg", &MET_RefEle_em_phi_ForwardReg, &b_MET_RefEle_em_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_em_etx", &MET_RefJet_em_etx, &b_MET_RefJet_em_etx);
   fChain->SetBranchAddress("MET_RefJet_em_ety", &MET_RefJet_em_ety, &b_MET_RefJet_em_ety);
   fChain->SetBranchAddress("MET_RefJet_em_phi", &MET_RefJet_em_phi, &b_MET_RefJet_em_phi);
   fChain->SetBranchAddress("MET_RefJet_em_et", &MET_RefJet_em_et, &b_MET_RefJet_em_et);
   fChain->SetBranchAddress("MET_RefJet_em_sumet", &MET_RefJet_em_sumet, &b_MET_RefJet_em_sumet);
   fChain->SetBranchAddress("MET_RefJet_em_etx_CentralReg", &MET_RefJet_em_etx_CentralReg, &b_MET_RefJet_em_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_em_ety_CentralReg", &MET_RefJet_em_ety_CentralReg, &b_MET_RefJet_em_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_em_sumet_CentralReg", &MET_RefJet_em_sumet_CentralReg, &b_MET_RefJet_em_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_em_phi_CentralReg", &MET_RefJet_em_phi_CentralReg, &b_MET_RefJet_em_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_em_etx_EndcapRegion", &MET_RefJet_em_etx_EndcapRegion, &b_MET_RefJet_em_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_em_ety_EndcapRegion", &MET_RefJet_em_ety_EndcapRegion, &b_MET_RefJet_em_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_em_sumet_EndcapRegion", &MET_RefJet_em_sumet_EndcapRegion, &b_MET_RefJet_em_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_em_phi_EndcapRegion", &MET_RefJet_em_phi_EndcapRegion, &b_MET_RefJet_em_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_em_etx_ForwardReg", &MET_RefJet_em_etx_ForwardReg, &b_MET_RefJet_em_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_em_ety_ForwardReg", &MET_RefJet_em_ety_ForwardReg, &b_MET_RefJet_em_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_em_sumet_ForwardReg", &MET_RefJet_em_sumet_ForwardReg, &b_MET_RefJet_em_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_em_phi_ForwardReg", &MET_RefJet_em_phi_ForwardReg, &b_MET_RefJet_em_phi_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_em_etx", &MET_SoftJets_em_etx, &b_MET_SoftJets_em_etx);
   fChain->SetBranchAddress("MET_SoftJets_em_ety", &MET_SoftJets_em_ety, &b_MET_SoftJets_em_ety);
   fChain->SetBranchAddress("MET_SoftJets_em_phi", &MET_SoftJets_em_phi, &b_MET_SoftJets_em_phi);
   fChain->SetBranchAddress("MET_SoftJets_em_et", &MET_SoftJets_em_et, &b_MET_SoftJets_em_et);
   fChain->SetBranchAddress("MET_SoftJets_em_sumet", &MET_SoftJets_em_sumet, &b_MET_SoftJets_em_sumet);
   fChain->SetBranchAddress("MET_SoftJets_em_etx_CentralReg", &MET_SoftJets_em_etx_CentralReg, &b_MET_SoftJets_em_etx_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_em_ety_CentralReg", &MET_SoftJets_em_ety_CentralReg, &b_MET_SoftJets_em_ety_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_em_sumet_CentralReg", &MET_SoftJets_em_sumet_CentralReg, &b_MET_SoftJets_em_sumet_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_em_phi_CentralReg", &MET_SoftJets_em_phi_CentralReg, &b_MET_SoftJets_em_phi_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_em_etx_EndcapRegion", &MET_SoftJets_em_etx_EndcapRegion, &b_MET_SoftJets_em_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_em_ety_EndcapRegion", &MET_SoftJets_em_ety_EndcapRegion, &b_MET_SoftJets_em_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_em_sumet_EndcapRegion", &MET_SoftJets_em_sumet_EndcapRegion, &b_MET_SoftJets_em_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_em_phi_EndcapRegion", &MET_SoftJets_em_phi_EndcapRegion, &b_MET_SoftJets_em_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_em_etx_ForwardReg", &MET_SoftJets_em_etx_ForwardReg, &b_MET_SoftJets_em_etx_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_em_ety_ForwardReg", &MET_SoftJets_em_ety_ForwardReg, &b_MET_SoftJets_em_ety_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_em_sumet_ForwardReg", &MET_SoftJets_em_sumet_ForwardReg, &b_MET_SoftJets_em_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_em_phi_ForwardReg", &MET_SoftJets_em_phi_ForwardReg, &b_MET_SoftJets_em_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_em_etx", &MET_RefMuon_em_etx, &b_MET_RefMuon_em_etx);
   fChain->SetBranchAddress("MET_RefMuon_em_ety", &MET_RefMuon_em_ety, &b_MET_RefMuon_em_ety);
   fChain->SetBranchAddress("MET_RefMuon_em_phi", &MET_RefMuon_em_phi, &b_MET_RefMuon_em_phi);
   fChain->SetBranchAddress("MET_RefMuon_em_et", &MET_RefMuon_em_et, &b_MET_RefMuon_em_et);
   fChain->SetBranchAddress("MET_RefMuon_em_sumet", &MET_RefMuon_em_sumet, &b_MET_RefMuon_em_sumet);
   fChain->SetBranchAddress("MET_RefMuon_em_etx_CentralReg", &MET_RefMuon_em_etx_CentralReg, &b_MET_RefMuon_em_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_em_ety_CentralReg", &MET_RefMuon_em_ety_CentralReg, &b_MET_RefMuon_em_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_em_sumet_CentralReg", &MET_RefMuon_em_sumet_CentralReg, &b_MET_RefMuon_em_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_em_phi_CentralReg", &MET_RefMuon_em_phi_CentralReg, &b_MET_RefMuon_em_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_em_etx_EndcapRegion", &MET_RefMuon_em_etx_EndcapRegion, &b_MET_RefMuon_em_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_em_ety_EndcapRegion", &MET_RefMuon_em_ety_EndcapRegion, &b_MET_RefMuon_em_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_em_sumet_EndcapRegion", &MET_RefMuon_em_sumet_EndcapRegion, &b_MET_RefMuon_em_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_em_phi_EndcapRegion", &MET_RefMuon_em_phi_EndcapRegion, &b_MET_RefMuon_em_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_em_etx_ForwardReg", &MET_RefMuon_em_etx_ForwardReg, &b_MET_RefMuon_em_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_em_ety_ForwardReg", &MET_RefMuon_em_ety_ForwardReg, &b_MET_RefMuon_em_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_em_sumet_ForwardReg", &MET_RefMuon_em_sumet_ForwardReg, &b_MET_RefMuon_em_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_em_phi_ForwardReg", &MET_RefMuon_em_phi_ForwardReg, &b_MET_RefMuon_em_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_etx", &MET_RefMuon_Track_em_etx, &b_MET_RefMuon_Track_em_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_ety", &MET_RefMuon_Track_em_ety, &b_MET_RefMuon_Track_em_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_phi", &MET_RefMuon_Track_em_phi, &b_MET_RefMuon_Track_em_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_et", &MET_RefMuon_Track_em_et, &b_MET_RefMuon_Track_em_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_sumet", &MET_RefMuon_Track_em_sumet, &b_MET_RefMuon_Track_em_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_etx_CentralReg", &MET_RefMuon_Track_em_etx_CentralReg, &b_MET_RefMuon_Track_em_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_ety_CentralReg", &MET_RefMuon_Track_em_ety_CentralReg, &b_MET_RefMuon_Track_em_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_sumet_CentralReg", &MET_RefMuon_Track_em_sumet_CentralReg, &b_MET_RefMuon_Track_em_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_phi_CentralReg", &MET_RefMuon_Track_em_phi_CentralReg, &b_MET_RefMuon_Track_em_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_etx_EndcapRegion", &MET_RefMuon_Track_em_etx_EndcapRegion, &b_MET_RefMuon_Track_em_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_ety_EndcapRegion", &MET_RefMuon_Track_em_ety_EndcapRegion, &b_MET_RefMuon_Track_em_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_sumet_EndcapRegion", &MET_RefMuon_Track_em_sumet_EndcapRegion, &b_MET_RefMuon_Track_em_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_phi_EndcapRegion", &MET_RefMuon_Track_em_phi_EndcapRegion, &b_MET_RefMuon_Track_em_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_etx_ForwardReg", &MET_RefMuon_Track_em_etx_ForwardReg, &b_MET_RefMuon_Track_em_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_ety_ForwardReg", &MET_RefMuon_Track_em_ety_ForwardReg, &b_MET_RefMuon_Track_em_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_sumet_ForwardReg", &MET_RefMuon_Track_em_sumet_ForwardReg, &b_MET_RefMuon_Track_em_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_em_phi_ForwardReg", &MET_RefMuon_Track_em_phi_ForwardReg, &b_MET_RefMuon_Track_em_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_em_etx", &MET_RefGamma_em_etx, &b_MET_RefGamma_em_etx);
   fChain->SetBranchAddress("MET_RefGamma_em_ety", &MET_RefGamma_em_ety, &b_MET_RefGamma_em_ety);
   fChain->SetBranchAddress("MET_RefGamma_em_phi", &MET_RefGamma_em_phi, &b_MET_RefGamma_em_phi);
   fChain->SetBranchAddress("MET_RefGamma_em_et", &MET_RefGamma_em_et, &b_MET_RefGamma_em_et);
   fChain->SetBranchAddress("MET_RefGamma_em_sumet", &MET_RefGamma_em_sumet, &b_MET_RefGamma_em_sumet);
   fChain->SetBranchAddress("MET_RefGamma_em_etx_CentralReg", &MET_RefGamma_em_etx_CentralReg, &b_MET_RefGamma_em_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_em_ety_CentralReg", &MET_RefGamma_em_ety_CentralReg, &b_MET_RefGamma_em_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_em_sumet_CentralReg", &MET_RefGamma_em_sumet_CentralReg, &b_MET_RefGamma_em_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_em_phi_CentralReg", &MET_RefGamma_em_phi_CentralReg, &b_MET_RefGamma_em_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_em_etx_EndcapRegion", &MET_RefGamma_em_etx_EndcapRegion, &b_MET_RefGamma_em_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_em_ety_EndcapRegion", &MET_RefGamma_em_ety_EndcapRegion, &b_MET_RefGamma_em_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_em_sumet_EndcapRegion", &MET_RefGamma_em_sumet_EndcapRegion, &b_MET_RefGamma_em_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_em_phi_EndcapRegion", &MET_RefGamma_em_phi_EndcapRegion, &b_MET_RefGamma_em_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_em_etx_ForwardReg", &MET_RefGamma_em_etx_ForwardReg, &b_MET_RefGamma_em_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_em_ety_ForwardReg", &MET_RefGamma_em_ety_ForwardReg, &b_MET_RefGamma_em_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_em_sumet_ForwardReg", &MET_RefGamma_em_sumet_ForwardReg, &b_MET_RefGamma_em_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_em_phi_ForwardReg", &MET_RefGamma_em_phi_ForwardReg, &b_MET_RefGamma_em_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_em_etx", &MET_RefTau_em_etx, &b_MET_RefTau_em_etx);
   fChain->SetBranchAddress("MET_RefTau_em_ety", &MET_RefTau_em_ety, &b_MET_RefTau_em_ety);
   fChain->SetBranchAddress("MET_RefTau_em_phi", &MET_RefTau_em_phi, &b_MET_RefTau_em_phi);
   fChain->SetBranchAddress("MET_RefTau_em_et", &MET_RefTau_em_et, &b_MET_RefTau_em_et);
   fChain->SetBranchAddress("MET_RefTau_em_sumet", &MET_RefTau_em_sumet, &b_MET_RefTau_em_sumet);
   fChain->SetBranchAddress("MET_RefTau_em_etx_CentralReg", &MET_RefTau_em_etx_CentralReg, &b_MET_RefTau_em_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_em_ety_CentralReg", &MET_RefTau_em_ety_CentralReg, &b_MET_RefTau_em_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_em_sumet_CentralReg", &MET_RefTau_em_sumet_CentralReg, &b_MET_RefTau_em_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_em_phi_CentralReg", &MET_RefTau_em_phi_CentralReg, &b_MET_RefTau_em_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_em_etx_EndcapRegion", &MET_RefTau_em_etx_EndcapRegion, &b_MET_RefTau_em_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_em_ety_EndcapRegion", &MET_RefTau_em_ety_EndcapRegion, &b_MET_RefTau_em_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_em_sumet_EndcapRegion", &MET_RefTau_em_sumet_EndcapRegion, &b_MET_RefTau_em_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_em_phi_EndcapRegion", &MET_RefTau_em_phi_EndcapRegion, &b_MET_RefTau_em_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_em_etx_ForwardReg", &MET_RefTau_em_etx_ForwardReg, &b_MET_RefTau_em_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_em_ety_ForwardReg", &MET_RefTau_em_ety_ForwardReg, &b_MET_RefTau_em_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_em_sumet_ForwardReg", &MET_RefTau_em_sumet_ForwardReg, &b_MET_RefTau_em_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_em_phi_ForwardReg", &MET_RefTau_em_phi_ForwardReg, &b_MET_RefTau_em_phi_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_em_etx", &MET_CellOut_em_etx, &b_MET_CellOut_em_etx);
   fChain->SetBranchAddress("MET_CellOut_em_ety", &MET_CellOut_em_ety, &b_MET_CellOut_em_ety);
   fChain->SetBranchAddress("MET_CellOut_em_phi", &MET_CellOut_em_phi, &b_MET_CellOut_em_phi);
   fChain->SetBranchAddress("MET_CellOut_em_et", &MET_CellOut_em_et, &b_MET_CellOut_em_et);
   fChain->SetBranchAddress("MET_CellOut_em_sumet", &MET_CellOut_em_sumet, &b_MET_CellOut_em_sumet);
   fChain->SetBranchAddress("MET_CellOut_em_etx_CentralReg", &MET_CellOut_em_etx_CentralReg, &b_MET_CellOut_em_etx_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_em_ety_CentralReg", &MET_CellOut_em_ety_CentralReg, &b_MET_CellOut_em_ety_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_em_sumet_CentralReg", &MET_CellOut_em_sumet_CentralReg, &b_MET_CellOut_em_sumet_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_em_phi_CentralReg", &MET_CellOut_em_phi_CentralReg, &b_MET_CellOut_em_phi_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_em_etx_EndcapRegion", &MET_CellOut_em_etx_EndcapRegion, &b_MET_CellOut_em_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_em_ety_EndcapRegion", &MET_CellOut_em_ety_EndcapRegion, &b_MET_CellOut_em_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_em_sumet_EndcapRegion", &MET_CellOut_em_sumet_EndcapRegion, &b_MET_CellOut_em_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_em_phi_EndcapRegion", &MET_CellOut_em_phi_EndcapRegion, &b_MET_CellOut_em_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_em_etx_ForwardReg", &MET_CellOut_em_etx_ForwardReg, &b_MET_CellOut_em_etx_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_em_ety_ForwardReg", &MET_CellOut_em_ety_ForwardReg, &b_MET_CellOut_em_ety_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_em_sumet_ForwardReg", &MET_CellOut_em_sumet_ForwardReg, &b_MET_CellOut_em_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_em_phi_ForwardReg", &MET_CellOut_em_phi_ForwardReg, &b_MET_CellOut_em_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_etx", &MET_Muon_Isol_Staco_etx, &b_MET_Muon_Isol_Staco_etx);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_ety", &MET_Muon_Isol_Staco_ety, &b_MET_Muon_Isol_Staco_ety);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_phi", &MET_Muon_Isol_Staco_phi, &b_MET_Muon_Isol_Staco_phi);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_et", &MET_Muon_Isol_Staco_et, &b_MET_Muon_Isol_Staco_et);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_sumet", &MET_Muon_Isol_Staco_sumet, &b_MET_Muon_Isol_Staco_sumet);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_etx_CentralReg", &MET_Muon_Isol_Staco_etx_CentralReg, &b_MET_Muon_Isol_Staco_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_ety_CentralReg", &MET_Muon_Isol_Staco_ety_CentralReg, &b_MET_Muon_Isol_Staco_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_sumet_CentralReg", &MET_Muon_Isol_Staco_sumet_CentralReg, &b_MET_Muon_Isol_Staco_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_phi_CentralReg", &MET_Muon_Isol_Staco_phi_CentralReg, &b_MET_Muon_Isol_Staco_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_etx_EndcapRegion", &MET_Muon_Isol_Staco_etx_EndcapRegion, &b_MET_Muon_Isol_Staco_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_ety_EndcapRegion", &MET_Muon_Isol_Staco_ety_EndcapRegion, &b_MET_Muon_Isol_Staco_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_sumet_EndcapRegion", &MET_Muon_Isol_Staco_sumet_EndcapRegion, &b_MET_Muon_Isol_Staco_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_phi_EndcapRegion", &MET_Muon_Isol_Staco_phi_EndcapRegion, &b_MET_Muon_Isol_Staco_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_etx_ForwardReg", &MET_Muon_Isol_Staco_etx_ForwardReg, &b_MET_Muon_Isol_Staco_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_ety_ForwardReg", &MET_Muon_Isol_Staco_ety_ForwardReg, &b_MET_Muon_Isol_Staco_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_sumet_ForwardReg", &MET_Muon_Isol_Staco_sumet_ForwardReg, &b_MET_Muon_Isol_Staco_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_phi_ForwardReg", &MET_Muon_Isol_Staco_phi_ForwardReg, &b_MET_Muon_Isol_Staco_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_etx", &MET_Muon_NonIsol_Staco_etx, &b_MET_Muon_NonIsol_Staco_etx);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_ety", &MET_Muon_NonIsol_Staco_ety, &b_MET_Muon_NonIsol_Staco_ety);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_phi", &MET_Muon_NonIsol_Staco_phi, &b_MET_Muon_NonIsol_Staco_phi);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_et", &MET_Muon_NonIsol_Staco_et, &b_MET_Muon_NonIsol_Staco_et);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_sumet", &MET_Muon_NonIsol_Staco_sumet, &b_MET_Muon_NonIsol_Staco_sumet);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_etx_CentralReg", &MET_Muon_NonIsol_Staco_etx_CentralReg, &b_MET_Muon_NonIsol_Staco_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_ety_CentralReg", &MET_Muon_NonIsol_Staco_ety_CentralReg, &b_MET_Muon_NonIsol_Staco_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_sumet_CentralReg", &MET_Muon_NonIsol_Staco_sumet_CentralReg, &b_MET_Muon_NonIsol_Staco_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_phi_CentralReg", &MET_Muon_NonIsol_Staco_phi_CentralReg, &b_MET_Muon_NonIsol_Staco_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_etx_EndcapRegion", &MET_Muon_NonIsol_Staco_etx_EndcapRegion, &b_MET_Muon_NonIsol_Staco_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_ety_EndcapRegion", &MET_Muon_NonIsol_Staco_ety_EndcapRegion, &b_MET_Muon_NonIsol_Staco_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_sumet_EndcapRegion", &MET_Muon_NonIsol_Staco_sumet_EndcapRegion, &b_MET_Muon_NonIsol_Staco_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_phi_EndcapRegion", &MET_Muon_NonIsol_Staco_phi_EndcapRegion, &b_MET_Muon_NonIsol_Staco_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_etx_ForwardReg", &MET_Muon_NonIsol_Staco_etx_ForwardReg, &b_MET_Muon_NonIsol_Staco_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_ety_ForwardReg", &MET_Muon_NonIsol_Staco_ety_ForwardReg, &b_MET_Muon_NonIsol_Staco_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_sumet_ForwardReg", &MET_Muon_NonIsol_Staco_sumet_ForwardReg, &b_MET_Muon_NonIsol_Staco_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_phi_ForwardReg", &MET_Muon_NonIsol_Staco_phi_ForwardReg, &b_MET_Muon_NonIsol_Staco_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_etx", &MET_Muon_Total_Staco_etx, &b_MET_Muon_Total_Staco_etx);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_ety", &MET_Muon_Total_Staco_ety, &b_MET_Muon_Total_Staco_ety);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_phi", &MET_Muon_Total_Staco_phi, &b_MET_Muon_Total_Staco_phi);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_et", &MET_Muon_Total_Staco_et, &b_MET_Muon_Total_Staco_et);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_sumet", &MET_Muon_Total_Staco_sumet, &b_MET_Muon_Total_Staco_sumet);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_etx_CentralReg", &MET_Muon_Total_Staco_etx_CentralReg, &b_MET_Muon_Total_Staco_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_ety_CentralReg", &MET_Muon_Total_Staco_ety_CentralReg, &b_MET_Muon_Total_Staco_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_sumet_CentralReg", &MET_Muon_Total_Staco_sumet_CentralReg, &b_MET_Muon_Total_Staco_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_phi_CentralReg", &MET_Muon_Total_Staco_phi_CentralReg, &b_MET_Muon_Total_Staco_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_etx_EndcapRegion", &MET_Muon_Total_Staco_etx_EndcapRegion, &b_MET_Muon_Total_Staco_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_ety_EndcapRegion", &MET_Muon_Total_Staco_ety_EndcapRegion, &b_MET_Muon_Total_Staco_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_sumet_EndcapRegion", &MET_Muon_Total_Staco_sumet_EndcapRegion, &b_MET_Muon_Total_Staco_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_phi_EndcapRegion", &MET_Muon_Total_Staco_phi_EndcapRegion, &b_MET_Muon_Total_Staco_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_etx_ForwardReg", &MET_Muon_Total_Staco_etx_ForwardReg, &b_MET_Muon_Total_Staco_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_ety_ForwardReg", &MET_Muon_Total_Staco_ety_ForwardReg, &b_MET_Muon_Total_Staco_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_sumet_ForwardReg", &MET_Muon_Total_Staco_sumet_ForwardReg, &b_MET_Muon_Total_Staco_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_phi_ForwardReg", &MET_Muon_Total_Staco_phi_ForwardReg, &b_MET_Muon_Total_Staco_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_etx", &MET_Muon_Isol_Muid_etx, &b_MET_Muon_Isol_Muid_etx);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_ety", &MET_Muon_Isol_Muid_ety, &b_MET_Muon_Isol_Muid_ety);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_phi", &MET_Muon_Isol_Muid_phi, &b_MET_Muon_Isol_Muid_phi);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_et", &MET_Muon_Isol_Muid_et, &b_MET_Muon_Isol_Muid_et);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_sumet", &MET_Muon_Isol_Muid_sumet, &b_MET_Muon_Isol_Muid_sumet);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_etx_CentralReg", &MET_Muon_Isol_Muid_etx_CentralReg, &b_MET_Muon_Isol_Muid_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_ety_CentralReg", &MET_Muon_Isol_Muid_ety_CentralReg, &b_MET_Muon_Isol_Muid_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_sumet_CentralReg", &MET_Muon_Isol_Muid_sumet_CentralReg, &b_MET_Muon_Isol_Muid_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_phi_CentralReg", &MET_Muon_Isol_Muid_phi_CentralReg, &b_MET_Muon_Isol_Muid_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_etx_EndcapRegion", &MET_Muon_Isol_Muid_etx_EndcapRegion, &b_MET_Muon_Isol_Muid_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_ety_EndcapRegion", &MET_Muon_Isol_Muid_ety_EndcapRegion, &b_MET_Muon_Isol_Muid_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_sumet_EndcapRegion", &MET_Muon_Isol_Muid_sumet_EndcapRegion, &b_MET_Muon_Isol_Muid_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_phi_EndcapRegion", &MET_Muon_Isol_Muid_phi_EndcapRegion, &b_MET_Muon_Isol_Muid_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_etx_ForwardReg", &MET_Muon_Isol_Muid_etx_ForwardReg, &b_MET_Muon_Isol_Muid_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_ety_ForwardReg", &MET_Muon_Isol_Muid_ety_ForwardReg, &b_MET_Muon_Isol_Muid_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_sumet_ForwardReg", &MET_Muon_Isol_Muid_sumet_ForwardReg, &b_MET_Muon_Isol_Muid_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_phi_ForwardReg", &MET_Muon_Isol_Muid_phi_ForwardReg, &b_MET_Muon_Isol_Muid_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_etx", &MET_Muon_NonIsol_Muid_etx, &b_MET_Muon_NonIsol_Muid_etx);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_ety", &MET_Muon_NonIsol_Muid_ety, &b_MET_Muon_NonIsol_Muid_ety);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_phi", &MET_Muon_NonIsol_Muid_phi, &b_MET_Muon_NonIsol_Muid_phi);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_et", &MET_Muon_NonIsol_Muid_et, &b_MET_Muon_NonIsol_Muid_et);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_sumet", &MET_Muon_NonIsol_Muid_sumet, &b_MET_Muon_NonIsol_Muid_sumet);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_etx_CentralReg", &MET_Muon_NonIsol_Muid_etx_CentralReg, &b_MET_Muon_NonIsol_Muid_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_ety_CentralReg", &MET_Muon_NonIsol_Muid_ety_CentralReg, &b_MET_Muon_NonIsol_Muid_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_sumet_CentralReg", &MET_Muon_NonIsol_Muid_sumet_CentralReg, &b_MET_Muon_NonIsol_Muid_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_phi_CentralReg", &MET_Muon_NonIsol_Muid_phi_CentralReg, &b_MET_Muon_NonIsol_Muid_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_etx_EndcapRegion", &MET_Muon_NonIsol_Muid_etx_EndcapRegion, &b_MET_Muon_NonIsol_Muid_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_ety_EndcapRegion", &MET_Muon_NonIsol_Muid_ety_EndcapRegion, &b_MET_Muon_NonIsol_Muid_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_sumet_EndcapRegion", &MET_Muon_NonIsol_Muid_sumet_EndcapRegion, &b_MET_Muon_NonIsol_Muid_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_phi_EndcapRegion", &MET_Muon_NonIsol_Muid_phi_EndcapRegion, &b_MET_Muon_NonIsol_Muid_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_etx_ForwardReg", &MET_Muon_NonIsol_Muid_etx_ForwardReg, &b_MET_Muon_NonIsol_Muid_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_ety_ForwardReg", &MET_Muon_NonIsol_Muid_ety_ForwardReg, &b_MET_Muon_NonIsol_Muid_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_sumet_ForwardReg", &MET_Muon_NonIsol_Muid_sumet_ForwardReg, &b_MET_Muon_NonIsol_Muid_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_phi_ForwardReg", &MET_Muon_NonIsol_Muid_phi_ForwardReg, &b_MET_Muon_NonIsol_Muid_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_etx", &MET_Muon_Total_Muid_etx, &b_MET_Muon_Total_Muid_etx);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_ety", &MET_Muon_Total_Muid_ety, &b_MET_Muon_Total_Muid_ety);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_phi", &MET_Muon_Total_Muid_phi, &b_MET_Muon_Total_Muid_phi);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_et", &MET_Muon_Total_Muid_et, &b_MET_Muon_Total_Muid_et);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_sumet", &MET_Muon_Total_Muid_sumet, &b_MET_Muon_Total_Muid_sumet);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_etx_CentralReg", &MET_Muon_Total_Muid_etx_CentralReg, &b_MET_Muon_Total_Muid_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_ety_CentralReg", &MET_Muon_Total_Muid_ety_CentralReg, &b_MET_Muon_Total_Muid_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_sumet_CentralReg", &MET_Muon_Total_Muid_sumet_CentralReg, &b_MET_Muon_Total_Muid_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_phi_CentralReg", &MET_Muon_Total_Muid_phi_CentralReg, &b_MET_Muon_Total_Muid_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_etx_EndcapRegion", &MET_Muon_Total_Muid_etx_EndcapRegion, &b_MET_Muon_Total_Muid_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_ety_EndcapRegion", &MET_Muon_Total_Muid_ety_EndcapRegion, &b_MET_Muon_Total_Muid_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_sumet_EndcapRegion", &MET_Muon_Total_Muid_sumet_EndcapRegion, &b_MET_Muon_Total_Muid_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_phi_EndcapRegion", &MET_Muon_Total_Muid_phi_EndcapRegion, &b_MET_Muon_Total_Muid_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_etx_ForwardReg", &MET_Muon_Total_Muid_etx_ForwardReg, &b_MET_Muon_Total_Muid_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_ety_ForwardReg", &MET_Muon_Total_Muid_ety_ForwardReg, &b_MET_Muon_Total_Muid_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_sumet_ForwardReg", &MET_Muon_Total_Muid_sumet_ForwardReg, &b_MET_Muon_Total_Muid_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_phi_ForwardReg", &MET_Muon_Total_Muid_phi_ForwardReg, &b_MET_Muon_Total_Muid_phi_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_etx", &MET_MuonBoy_etx, &b_MET_MuonBoy_etx);
   fChain->SetBranchAddress("MET_MuonBoy_ety", &MET_MuonBoy_ety, &b_MET_MuonBoy_ety);
   fChain->SetBranchAddress("MET_MuonBoy_phi", &MET_MuonBoy_phi, &b_MET_MuonBoy_phi);
   fChain->SetBranchAddress("MET_MuonBoy_et", &MET_MuonBoy_et, &b_MET_MuonBoy_et);
   fChain->SetBranchAddress("MET_MuonBoy_sumet", &MET_MuonBoy_sumet, &b_MET_MuonBoy_sumet);
   fChain->SetBranchAddress("MET_MuonBoy_etx_CentralReg", &MET_MuonBoy_etx_CentralReg, &b_MET_MuonBoy_etx_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_ety_CentralReg", &MET_MuonBoy_ety_CentralReg, &b_MET_MuonBoy_ety_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_sumet_CentralReg", &MET_MuonBoy_sumet_CentralReg, &b_MET_MuonBoy_sumet_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_phi_CentralReg", &MET_MuonBoy_phi_CentralReg, &b_MET_MuonBoy_phi_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_etx_EndcapRegion", &MET_MuonBoy_etx_EndcapRegion, &b_MET_MuonBoy_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_ety_EndcapRegion", &MET_MuonBoy_ety_EndcapRegion, &b_MET_MuonBoy_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_sumet_EndcapRegion", &MET_MuonBoy_sumet_EndcapRegion, &b_MET_MuonBoy_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_phi_EndcapRegion", &MET_MuonBoy_phi_EndcapRegion, &b_MET_MuonBoy_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_etx_ForwardReg", &MET_MuonBoy_etx_ForwardReg, &b_MET_MuonBoy_etx_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_ety_ForwardReg", &MET_MuonBoy_ety_ForwardReg, &b_MET_MuonBoy_ety_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_sumet_ForwardReg", &MET_MuonBoy_sumet_ForwardReg, &b_MET_MuonBoy_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_phi_ForwardReg", &MET_MuonBoy_phi_ForwardReg, &b_MET_MuonBoy_phi_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_Eflow_etx", &MET_CellOut_Eflow_etx, &b_MET_CellOut_Eflow_etx);
   fChain->SetBranchAddress("MET_CellOut_Eflow_ety", &MET_CellOut_Eflow_ety, &b_MET_CellOut_Eflow_ety);
   fChain->SetBranchAddress("MET_CellOut_Eflow_phi", &MET_CellOut_Eflow_phi, &b_MET_CellOut_Eflow_phi);
   fChain->SetBranchAddress("MET_CellOut_Eflow_et", &MET_CellOut_Eflow_et, &b_MET_CellOut_Eflow_et);
   fChain->SetBranchAddress("MET_CellOut_Eflow_sumet", &MET_CellOut_Eflow_sumet, &b_MET_CellOut_Eflow_sumet);
   fChain->SetBranchAddress("MET_CellOut_Eflow_etx_CentralReg", &MET_CellOut_Eflow_etx_CentralReg, &b_MET_CellOut_Eflow_etx_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_Eflow_ety_CentralReg", &MET_CellOut_Eflow_ety_CentralReg, &b_MET_CellOut_Eflow_ety_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_Eflow_sumet_CentralReg", &MET_CellOut_Eflow_sumet_CentralReg, &b_MET_CellOut_Eflow_sumet_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_Eflow_phi_CentralReg", &MET_CellOut_Eflow_phi_CentralReg, &b_MET_CellOut_Eflow_phi_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_Eflow_etx_EndcapRegion", &MET_CellOut_Eflow_etx_EndcapRegion, &b_MET_CellOut_Eflow_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_Eflow_ety_EndcapRegion", &MET_CellOut_Eflow_ety_EndcapRegion, &b_MET_CellOut_Eflow_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_Eflow_sumet_EndcapRegion", &MET_CellOut_Eflow_sumet_EndcapRegion, &b_MET_CellOut_Eflow_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_Eflow_phi_EndcapRegion", &MET_CellOut_Eflow_phi_EndcapRegion, &b_MET_CellOut_Eflow_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_Eflow_etx_ForwardReg", &MET_CellOut_Eflow_etx_ForwardReg, &b_MET_CellOut_Eflow_etx_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_Eflow_ety_ForwardReg", &MET_CellOut_Eflow_ety_ForwardReg, &b_MET_CellOut_Eflow_ety_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_Eflow_sumet_ForwardReg", &MET_CellOut_Eflow_sumet_ForwardReg, &b_MET_CellOut_Eflow_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_Eflow_phi_ForwardReg", &MET_CellOut_Eflow_phi_ForwardReg, &b_MET_CellOut_Eflow_phi_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_etx", &MET_CellOut_MiniJet_etx, &b_MET_CellOut_MiniJet_etx);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_ety", &MET_CellOut_MiniJet_ety, &b_MET_CellOut_MiniJet_ety);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_phi", &MET_CellOut_MiniJet_phi, &b_MET_CellOut_MiniJet_phi);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_et", &MET_CellOut_MiniJet_et, &b_MET_CellOut_MiniJet_et);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_sumet", &MET_CellOut_MiniJet_sumet, &b_MET_CellOut_MiniJet_sumet);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_etx_CentralReg", &MET_CellOut_MiniJet_etx_CentralReg, &b_MET_CellOut_MiniJet_etx_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_ety_CentralReg", &MET_CellOut_MiniJet_ety_CentralReg, &b_MET_CellOut_MiniJet_ety_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_sumet_CentralReg", &MET_CellOut_MiniJet_sumet_CentralReg, &b_MET_CellOut_MiniJet_sumet_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_phi_CentralReg", &MET_CellOut_MiniJet_phi_CentralReg, &b_MET_CellOut_MiniJet_phi_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_etx_EndcapRegion", &MET_CellOut_MiniJet_etx_EndcapRegion, &b_MET_CellOut_MiniJet_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_ety_EndcapRegion", &MET_CellOut_MiniJet_ety_EndcapRegion, &b_MET_CellOut_MiniJet_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_sumet_EndcapRegion", &MET_CellOut_MiniJet_sumet_EndcapRegion, &b_MET_CellOut_MiniJet_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_phi_EndcapRegion", &MET_CellOut_MiniJet_phi_EndcapRegion, &b_MET_CellOut_MiniJet_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_etx_ForwardReg", &MET_CellOut_MiniJet_etx_ForwardReg, &b_MET_CellOut_MiniJet_etx_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_ety_ForwardReg", &MET_CellOut_MiniJet_ety_ForwardReg, &b_MET_CellOut_MiniJet_ety_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_sumet_ForwardReg", &MET_CellOut_MiniJet_sumet_ForwardReg, &b_MET_CellOut_MiniJet_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_MiniJet_phi_ForwardReg", &MET_CellOut_MiniJet_phi_ForwardReg, &b_MET_CellOut_MiniJet_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_etx", &MET_RefMuon_Track_etx, &b_MET_RefMuon_Track_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_ety", &MET_RefMuon_Track_ety, &b_MET_RefMuon_Track_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_phi", &MET_RefMuon_Track_phi, &b_MET_RefMuon_Track_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_et", &MET_RefMuon_Track_et, &b_MET_RefMuon_Track_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_sumet", &MET_RefMuon_Track_sumet, &b_MET_RefMuon_Track_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_etx_CentralReg", &MET_RefMuon_Track_etx_CentralReg, &b_MET_RefMuon_Track_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_ety_CentralReg", &MET_RefMuon_Track_ety_CentralReg, &b_MET_RefMuon_Track_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_sumet_CentralReg", &MET_RefMuon_Track_sumet_CentralReg, &b_MET_RefMuon_Track_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_phi_CentralReg", &MET_RefMuon_Track_phi_CentralReg, &b_MET_RefMuon_Track_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_etx_EndcapRegion", &MET_RefMuon_Track_etx_EndcapRegion, &b_MET_RefMuon_Track_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_ety_EndcapRegion", &MET_RefMuon_Track_ety_EndcapRegion, &b_MET_RefMuon_Track_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_sumet_EndcapRegion", &MET_RefMuon_Track_sumet_EndcapRegion, &b_MET_RefMuon_Track_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_phi_EndcapRegion", &MET_RefMuon_Track_phi_EndcapRegion, &b_MET_RefMuon_Track_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_etx_ForwardReg", &MET_RefMuon_Track_etx_ForwardReg, &b_MET_RefMuon_Track_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_ety_ForwardReg", &MET_RefMuon_Track_ety_ForwardReg, &b_MET_RefMuon_Track_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_sumet_ForwardReg", &MET_RefMuon_Track_sumet_ForwardReg, &b_MET_RefMuon_Track_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_phi_ForwardReg", &MET_RefMuon_Track_phi_ForwardReg, &b_MET_RefMuon_Track_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_etx", &MET_RefMuon_Track_Staco_etx, &b_MET_RefMuon_Track_Staco_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_ety", &MET_RefMuon_Track_Staco_ety, &b_MET_RefMuon_Track_Staco_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_phi", &MET_RefMuon_Track_Staco_phi, &b_MET_RefMuon_Track_Staco_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_et", &MET_RefMuon_Track_Staco_et, &b_MET_RefMuon_Track_Staco_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_sumet", &MET_RefMuon_Track_Staco_sumet, &b_MET_RefMuon_Track_Staco_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_etx_CentralReg", &MET_RefMuon_Track_Staco_etx_CentralReg, &b_MET_RefMuon_Track_Staco_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_ety_CentralReg", &MET_RefMuon_Track_Staco_ety_CentralReg, &b_MET_RefMuon_Track_Staco_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_sumet_CentralReg", &MET_RefMuon_Track_Staco_sumet_CentralReg, &b_MET_RefMuon_Track_Staco_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_phi_CentralReg", &MET_RefMuon_Track_Staco_phi_CentralReg, &b_MET_RefMuon_Track_Staco_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_etx_EndcapRegion", &MET_RefMuon_Track_Staco_etx_EndcapRegion, &b_MET_RefMuon_Track_Staco_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_ety_EndcapRegion", &MET_RefMuon_Track_Staco_ety_EndcapRegion, &b_MET_RefMuon_Track_Staco_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_sumet_EndcapRegion", &MET_RefMuon_Track_Staco_sumet_EndcapRegion, &b_MET_RefMuon_Track_Staco_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_phi_EndcapRegion", &MET_RefMuon_Track_Staco_phi_EndcapRegion, &b_MET_RefMuon_Track_Staco_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_etx_ForwardReg", &MET_RefMuon_Track_Staco_etx_ForwardReg, &b_MET_RefMuon_Track_Staco_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_ety_ForwardReg", &MET_RefMuon_Track_Staco_ety_ForwardReg, &b_MET_RefMuon_Track_Staco_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_sumet_ForwardReg", &MET_RefMuon_Track_Staco_sumet_ForwardReg, &b_MET_RefMuon_Track_Staco_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_phi_ForwardReg", &MET_RefMuon_Track_Staco_phi_ForwardReg, &b_MET_RefMuon_Track_Staco_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_etx", &MET_RefMuon_Track_Muid_etx, &b_MET_RefMuon_Track_Muid_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_ety", &MET_RefMuon_Track_Muid_ety, &b_MET_RefMuon_Track_Muid_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_phi", &MET_RefMuon_Track_Muid_phi, &b_MET_RefMuon_Track_Muid_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_et", &MET_RefMuon_Track_Muid_et, &b_MET_RefMuon_Track_Muid_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_sumet", &MET_RefMuon_Track_Muid_sumet, &b_MET_RefMuon_Track_Muid_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_etx_CentralReg", &MET_RefMuon_Track_Muid_etx_CentralReg, &b_MET_RefMuon_Track_Muid_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_ety_CentralReg", &MET_RefMuon_Track_Muid_ety_CentralReg, &b_MET_RefMuon_Track_Muid_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_sumet_CentralReg", &MET_RefMuon_Track_Muid_sumet_CentralReg, &b_MET_RefMuon_Track_Muid_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_phi_CentralReg", &MET_RefMuon_Track_Muid_phi_CentralReg, &b_MET_RefMuon_Track_Muid_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_etx_EndcapRegion", &MET_RefMuon_Track_Muid_etx_EndcapRegion, &b_MET_RefMuon_Track_Muid_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_ety_EndcapRegion", &MET_RefMuon_Track_Muid_ety_EndcapRegion, &b_MET_RefMuon_Track_Muid_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_sumet_EndcapRegion", &MET_RefMuon_Track_Muid_sumet_EndcapRegion, &b_MET_RefMuon_Track_Muid_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_phi_EndcapRegion", &MET_RefMuon_Track_Muid_phi_EndcapRegion, &b_MET_RefMuon_Track_Muid_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_etx_ForwardReg", &MET_RefMuon_Track_Muid_etx_ForwardReg, &b_MET_RefMuon_Track_Muid_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_ety_ForwardReg", &MET_RefMuon_Track_Muid_ety_ForwardReg, &b_MET_RefMuon_Track_Muid_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_sumet_ForwardReg", &MET_RefMuon_Track_Muid_sumet_ForwardReg, &b_MET_RefMuon_Track_Muid_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_phi_ForwardReg", &MET_RefMuon_Track_Muid_phi_ForwardReg, &b_MET_RefMuon_Track_Muid_phi_ForwardReg);
   fChain->SetBranchAddress("MET_CryoCone_etx", &MET_CryoCone_etx, &b_MET_CryoCone_etx);
   fChain->SetBranchAddress("MET_CryoCone_ety", &MET_CryoCone_ety, &b_MET_CryoCone_ety);
   fChain->SetBranchAddress("MET_CryoCone_phi", &MET_CryoCone_phi, &b_MET_CryoCone_phi);
   fChain->SetBranchAddress("MET_CryoCone_et", &MET_CryoCone_et, &b_MET_CryoCone_et);
   fChain->SetBranchAddress("MET_CryoCone_sumet", &MET_CryoCone_sumet, &b_MET_CryoCone_sumet);
   fChain->SetBranchAddress("MET_CryoCone_etx_CentralReg", &MET_CryoCone_etx_CentralReg, &b_MET_CryoCone_etx_CentralReg);
   fChain->SetBranchAddress("MET_CryoCone_ety_CentralReg", &MET_CryoCone_ety_CentralReg, &b_MET_CryoCone_ety_CentralReg);
   fChain->SetBranchAddress("MET_CryoCone_sumet_CentralReg", &MET_CryoCone_sumet_CentralReg, &b_MET_CryoCone_sumet_CentralReg);
   fChain->SetBranchAddress("MET_CryoCone_phi_CentralReg", &MET_CryoCone_phi_CentralReg, &b_MET_CryoCone_phi_CentralReg);
   fChain->SetBranchAddress("MET_CryoCone_etx_EndcapRegion", &MET_CryoCone_etx_EndcapRegion, &b_MET_CryoCone_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_CryoCone_ety_EndcapRegion", &MET_CryoCone_ety_EndcapRegion, &b_MET_CryoCone_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_CryoCone_sumet_EndcapRegion", &MET_CryoCone_sumet_EndcapRegion, &b_MET_CryoCone_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_CryoCone_phi_EndcapRegion", &MET_CryoCone_phi_EndcapRegion, &b_MET_CryoCone_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_CryoCone_etx_ForwardReg", &MET_CryoCone_etx_ForwardReg, &b_MET_CryoCone_etx_ForwardReg);
   fChain->SetBranchAddress("MET_CryoCone_ety_ForwardReg", &MET_CryoCone_ety_ForwardReg, &b_MET_CryoCone_ety_ForwardReg);
   fChain->SetBranchAddress("MET_CryoCone_sumet_ForwardReg", &MET_CryoCone_sumet_ForwardReg, &b_MET_CryoCone_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_CryoCone_phi_ForwardReg", &MET_CryoCone_phi_ForwardReg, &b_MET_CryoCone_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Final_etx", &MET_Final_etx, &b_MET_Final_etx);
   fChain->SetBranchAddress("MET_Final_ety", &MET_Final_ety, &b_MET_Final_ety);
   fChain->SetBranchAddress("MET_Final_phi", &MET_Final_phi, &b_MET_Final_phi);
   fChain->SetBranchAddress("MET_Final_et", &MET_Final_et, &b_MET_Final_et);
   fChain->SetBranchAddress("MET_Final_sumet", &MET_Final_sumet, &b_MET_Final_sumet);
   fChain->SetBranchAddress("MET_Final_etx_CentralReg", &MET_Final_etx_CentralReg, &b_MET_Final_etx_CentralReg);
   fChain->SetBranchAddress("MET_Final_ety_CentralReg", &MET_Final_ety_CentralReg, &b_MET_Final_ety_CentralReg);
   fChain->SetBranchAddress("MET_Final_sumet_CentralReg", &MET_Final_sumet_CentralReg, &b_MET_Final_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Final_phi_CentralReg", &MET_Final_phi_CentralReg, &b_MET_Final_phi_CentralReg);
   fChain->SetBranchAddress("MET_Final_etx_EndcapRegion", &MET_Final_etx_EndcapRegion, &b_MET_Final_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Final_ety_EndcapRegion", &MET_Final_ety_EndcapRegion, &b_MET_Final_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Final_sumet_EndcapRegion", &MET_Final_sumet_EndcapRegion, &b_MET_Final_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Final_phi_EndcapRegion", &MET_Final_phi_EndcapRegion, &b_MET_Final_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Final_etx_ForwardReg", &MET_Final_etx_ForwardReg, &b_MET_Final_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Final_ety_ForwardReg", &MET_Final_ety_ForwardReg, &b_MET_Final_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Final_sumet_ForwardReg", &MET_Final_sumet_ForwardReg, &b_MET_Final_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Final_phi_ForwardReg", &MET_Final_phi_ForwardReg, &b_MET_Final_phi_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_etx", &MET_MuonBoy_Spectro_etx, &b_MET_MuonBoy_Spectro_etx);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_ety", &MET_MuonBoy_Spectro_ety, &b_MET_MuonBoy_Spectro_ety);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_phi", &MET_MuonBoy_Spectro_phi, &b_MET_MuonBoy_Spectro_phi);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_et", &MET_MuonBoy_Spectro_et, &b_MET_MuonBoy_Spectro_et);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_sumet", &MET_MuonBoy_Spectro_sumet, &b_MET_MuonBoy_Spectro_sumet);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_etx_CentralReg", &MET_MuonBoy_Spectro_etx_CentralReg, &b_MET_MuonBoy_Spectro_etx_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_ety_CentralReg", &MET_MuonBoy_Spectro_ety_CentralReg, &b_MET_MuonBoy_Spectro_ety_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_sumet_CentralReg", &MET_MuonBoy_Spectro_sumet_CentralReg, &b_MET_MuonBoy_Spectro_sumet_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_phi_CentralReg", &MET_MuonBoy_Spectro_phi_CentralReg, &b_MET_MuonBoy_Spectro_phi_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_etx_EndcapRegion", &MET_MuonBoy_Spectro_etx_EndcapRegion, &b_MET_MuonBoy_Spectro_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_ety_EndcapRegion", &MET_MuonBoy_Spectro_ety_EndcapRegion, &b_MET_MuonBoy_Spectro_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_sumet_EndcapRegion", &MET_MuonBoy_Spectro_sumet_EndcapRegion, &b_MET_MuonBoy_Spectro_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_phi_EndcapRegion", &MET_MuonBoy_Spectro_phi_EndcapRegion, &b_MET_MuonBoy_Spectro_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_etx_ForwardReg", &MET_MuonBoy_Spectro_etx_ForwardReg, &b_MET_MuonBoy_Spectro_etx_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_ety_ForwardReg", &MET_MuonBoy_Spectro_ety_ForwardReg, &b_MET_MuonBoy_Spectro_ety_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_sumet_ForwardReg", &MET_MuonBoy_Spectro_sumet_ForwardReg, &b_MET_MuonBoy_Spectro_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_Spectro_phi_ForwardReg", &MET_MuonBoy_Spectro_phi_ForwardReg, &b_MET_MuonBoy_Spectro_phi_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_Track_etx", &MET_MuonBoy_Track_etx, &b_MET_MuonBoy_Track_etx);
   fChain->SetBranchAddress("MET_MuonBoy_Track_ety", &MET_MuonBoy_Track_ety, &b_MET_MuonBoy_Track_ety);
   fChain->SetBranchAddress("MET_MuonBoy_Track_phi", &MET_MuonBoy_Track_phi, &b_MET_MuonBoy_Track_phi);
   fChain->SetBranchAddress("MET_MuonBoy_Track_et", &MET_MuonBoy_Track_et, &b_MET_MuonBoy_Track_et);
   fChain->SetBranchAddress("MET_MuonBoy_Track_sumet", &MET_MuonBoy_Track_sumet, &b_MET_MuonBoy_Track_sumet);
   fChain->SetBranchAddress("MET_MuonBoy_Track_etx_CentralReg", &MET_MuonBoy_Track_etx_CentralReg, &b_MET_MuonBoy_Track_etx_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_Track_ety_CentralReg", &MET_MuonBoy_Track_ety_CentralReg, &b_MET_MuonBoy_Track_ety_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_Track_sumet_CentralReg", &MET_MuonBoy_Track_sumet_CentralReg, &b_MET_MuonBoy_Track_sumet_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_Track_phi_CentralReg", &MET_MuonBoy_Track_phi_CentralReg, &b_MET_MuonBoy_Track_phi_CentralReg);
   fChain->SetBranchAddress("MET_MuonBoy_Track_etx_EndcapRegion", &MET_MuonBoy_Track_etx_EndcapRegion, &b_MET_MuonBoy_Track_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_Track_ety_EndcapRegion", &MET_MuonBoy_Track_ety_EndcapRegion, &b_MET_MuonBoy_Track_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_Track_sumet_EndcapRegion", &MET_MuonBoy_Track_sumet_EndcapRegion, &b_MET_MuonBoy_Track_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_Track_phi_EndcapRegion", &MET_MuonBoy_Track_phi_EndcapRegion, &b_MET_MuonBoy_Track_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonBoy_Track_etx_ForwardReg", &MET_MuonBoy_Track_etx_ForwardReg, &b_MET_MuonBoy_Track_etx_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_Track_ety_ForwardReg", &MET_MuonBoy_Track_ety_ForwardReg, &b_MET_MuonBoy_Track_ety_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_Track_sumet_ForwardReg", &MET_MuonBoy_Track_sumet_ForwardReg, &b_MET_MuonBoy_Track_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_MuonBoy_Track_phi_ForwardReg", &MET_MuonBoy_Track_phi_ForwardReg, &b_MET_MuonBoy_Track_phi_ForwardReg);
   fChain->SetBranchAddress("MET_MuonMuid_etx", &MET_MuonMuid_etx, &b_MET_MuonMuid_etx);
   fChain->SetBranchAddress("MET_MuonMuid_ety", &MET_MuonMuid_ety, &b_MET_MuonMuid_ety);
   fChain->SetBranchAddress("MET_MuonMuid_phi", &MET_MuonMuid_phi, &b_MET_MuonMuid_phi);
   fChain->SetBranchAddress("MET_MuonMuid_et", &MET_MuonMuid_et, &b_MET_MuonMuid_et);
   fChain->SetBranchAddress("MET_MuonMuid_sumet", &MET_MuonMuid_sumet, &b_MET_MuonMuid_sumet);
   fChain->SetBranchAddress("MET_MuonMuid_etx_CentralReg", &MET_MuonMuid_etx_CentralReg, &b_MET_MuonMuid_etx_CentralReg);
   fChain->SetBranchAddress("MET_MuonMuid_ety_CentralReg", &MET_MuonMuid_ety_CentralReg, &b_MET_MuonMuid_ety_CentralReg);
   fChain->SetBranchAddress("MET_MuonMuid_sumet_CentralReg", &MET_MuonMuid_sumet_CentralReg, &b_MET_MuonMuid_sumet_CentralReg);
   fChain->SetBranchAddress("MET_MuonMuid_phi_CentralReg", &MET_MuonMuid_phi_CentralReg, &b_MET_MuonMuid_phi_CentralReg);
   fChain->SetBranchAddress("MET_MuonMuid_etx_EndcapRegion", &MET_MuonMuid_etx_EndcapRegion, &b_MET_MuonMuid_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonMuid_ety_EndcapRegion", &MET_MuonMuid_ety_EndcapRegion, &b_MET_MuonMuid_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonMuid_sumet_EndcapRegion", &MET_MuonMuid_sumet_EndcapRegion, &b_MET_MuonMuid_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonMuid_phi_EndcapRegion", &MET_MuonMuid_phi_EndcapRegion, &b_MET_MuonMuid_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_MuonMuid_etx_ForwardReg", &MET_MuonMuid_etx_ForwardReg, &b_MET_MuonMuid_etx_ForwardReg);
   fChain->SetBranchAddress("MET_MuonMuid_ety_ForwardReg", &MET_MuonMuid_ety_ForwardReg, &b_MET_MuonMuid_ety_ForwardReg);
   fChain->SetBranchAddress("MET_MuonMuid_sumet_ForwardReg", &MET_MuonMuid_sumet_ForwardReg, &b_MET_MuonMuid_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_MuonMuid_phi_ForwardReg", &MET_MuonMuid_phi_ForwardReg, &b_MET_MuonMuid_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_etx", &MET_Muid_etx, &b_MET_Muid_etx);
   fChain->SetBranchAddress("MET_Muid_ety", &MET_Muid_ety, &b_MET_Muid_ety);
   fChain->SetBranchAddress("MET_Muid_phi", &MET_Muid_phi, &b_MET_Muid_phi);
   fChain->SetBranchAddress("MET_Muid_et", &MET_Muid_et, &b_MET_Muid_et);
   fChain->SetBranchAddress("MET_Muid_sumet", &MET_Muid_sumet, &b_MET_Muid_sumet);
   fChain->SetBranchAddress("MET_Muid_etx_CentralReg", &MET_Muid_etx_CentralReg, &b_MET_Muid_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muid_ety_CentralReg", &MET_Muid_ety_CentralReg, &b_MET_Muid_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muid_sumet_CentralReg", &MET_Muid_sumet_CentralReg, &b_MET_Muid_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muid_phi_CentralReg", &MET_Muid_phi_CentralReg, &b_MET_Muid_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muid_etx_EndcapRegion", &MET_Muid_etx_EndcapRegion, &b_MET_Muid_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_ety_EndcapRegion", &MET_Muid_ety_EndcapRegion, &b_MET_Muid_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_sumet_EndcapRegion", &MET_Muid_sumet_EndcapRegion, &b_MET_Muid_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_phi_EndcapRegion", &MET_Muid_phi_EndcapRegion, &b_MET_Muid_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_etx_ForwardReg", &MET_Muid_etx_ForwardReg, &b_MET_Muid_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_ety_ForwardReg", &MET_Muid_ety_ForwardReg, &b_MET_Muid_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_sumet_ForwardReg", &MET_Muid_sumet_ForwardReg, &b_MET_Muid_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_phi_ForwardReg", &MET_Muid_phi_ForwardReg, &b_MET_Muid_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_Spectro_etx", &MET_Muid_Spectro_etx, &b_MET_Muid_Spectro_etx);
   fChain->SetBranchAddress("MET_Muid_Spectro_ety", &MET_Muid_Spectro_ety, &b_MET_Muid_Spectro_ety);
   fChain->SetBranchAddress("MET_Muid_Spectro_phi", &MET_Muid_Spectro_phi, &b_MET_Muid_Spectro_phi);
   fChain->SetBranchAddress("MET_Muid_Spectro_et", &MET_Muid_Spectro_et, &b_MET_Muid_Spectro_et);
   fChain->SetBranchAddress("MET_Muid_Spectro_sumet", &MET_Muid_Spectro_sumet, &b_MET_Muid_Spectro_sumet);
   fChain->SetBranchAddress("MET_Muid_Spectro_etx_CentralReg", &MET_Muid_Spectro_etx_CentralReg, &b_MET_Muid_Spectro_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muid_Spectro_ety_CentralReg", &MET_Muid_Spectro_ety_CentralReg, &b_MET_Muid_Spectro_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muid_Spectro_sumet_CentralReg", &MET_Muid_Spectro_sumet_CentralReg, &b_MET_Muid_Spectro_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muid_Spectro_phi_CentralReg", &MET_Muid_Spectro_phi_CentralReg, &b_MET_Muid_Spectro_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muid_Spectro_etx_EndcapRegion", &MET_Muid_Spectro_etx_EndcapRegion, &b_MET_Muid_Spectro_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_Spectro_ety_EndcapRegion", &MET_Muid_Spectro_ety_EndcapRegion, &b_MET_Muid_Spectro_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_Spectro_sumet_EndcapRegion", &MET_Muid_Spectro_sumet_EndcapRegion, &b_MET_Muid_Spectro_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_Spectro_phi_EndcapRegion", &MET_Muid_Spectro_phi_EndcapRegion, &b_MET_Muid_Spectro_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_Spectro_etx_ForwardReg", &MET_Muid_Spectro_etx_ForwardReg, &b_MET_Muid_Spectro_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_Spectro_ety_ForwardReg", &MET_Muid_Spectro_ety_ForwardReg, &b_MET_Muid_Spectro_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_Spectro_sumet_ForwardReg", &MET_Muid_Spectro_sumet_ForwardReg, &b_MET_Muid_Spectro_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_Spectro_phi_ForwardReg", &MET_Muid_Spectro_phi_ForwardReg, &b_MET_Muid_Spectro_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_Track_etx", &MET_Muid_Track_etx, &b_MET_Muid_Track_etx);
   fChain->SetBranchAddress("MET_Muid_Track_ety", &MET_Muid_Track_ety, &b_MET_Muid_Track_ety);
   fChain->SetBranchAddress("MET_Muid_Track_phi", &MET_Muid_Track_phi, &b_MET_Muid_Track_phi);
   fChain->SetBranchAddress("MET_Muid_Track_et", &MET_Muid_Track_et, &b_MET_Muid_Track_et);
   fChain->SetBranchAddress("MET_Muid_Track_sumet", &MET_Muid_Track_sumet, &b_MET_Muid_Track_sumet);
   fChain->SetBranchAddress("MET_Muid_Track_etx_CentralReg", &MET_Muid_Track_etx_CentralReg, &b_MET_Muid_Track_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muid_Track_ety_CentralReg", &MET_Muid_Track_ety_CentralReg, &b_MET_Muid_Track_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muid_Track_sumet_CentralReg", &MET_Muid_Track_sumet_CentralReg, &b_MET_Muid_Track_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muid_Track_phi_CentralReg", &MET_Muid_Track_phi_CentralReg, &b_MET_Muid_Track_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muid_Track_etx_EndcapRegion", &MET_Muid_Track_etx_EndcapRegion, &b_MET_Muid_Track_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_Track_ety_EndcapRegion", &MET_Muid_Track_ety_EndcapRegion, &b_MET_Muid_Track_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_Track_sumet_EndcapRegion", &MET_Muid_Track_sumet_EndcapRegion, &b_MET_Muid_Track_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_Track_phi_EndcapRegion", &MET_Muid_Track_phi_EndcapRegion, &b_MET_Muid_Track_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muid_Track_etx_ForwardReg", &MET_Muid_Track_etx_ForwardReg, &b_MET_Muid_Track_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_Track_ety_ForwardReg", &MET_Muid_Track_ety_ForwardReg, &b_MET_Muid_Track_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_Track_sumet_ForwardReg", &MET_Muid_Track_sumet_ForwardReg, &b_MET_Muid_Track_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muid_Track_phi_ForwardReg", &MET_Muid_Track_phi_ForwardReg, &b_MET_Muid_Track_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_etx", &MET_Muon_etx, &b_MET_Muon_etx);
   fChain->SetBranchAddress("MET_Muon_ety", &MET_Muon_ety, &b_MET_Muon_ety);
   fChain->SetBranchAddress("MET_Muon_phi", &MET_Muon_phi, &b_MET_Muon_phi);
   fChain->SetBranchAddress("MET_Muon_et", &MET_Muon_et, &b_MET_Muon_et);
   fChain->SetBranchAddress("MET_Muon_sumet", &MET_Muon_sumet, &b_MET_Muon_sumet);
   fChain->SetBranchAddress("MET_Muon_etx_CentralReg", &MET_Muon_etx_CentralReg, &b_MET_Muon_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_ety_CentralReg", &MET_Muon_ety_CentralReg, &b_MET_Muon_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_sumet_CentralReg", &MET_Muon_sumet_CentralReg, &b_MET_Muon_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_phi_CentralReg", &MET_Muon_phi_CentralReg, &b_MET_Muon_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_etx_EndcapRegion", &MET_Muon_etx_EndcapRegion, &b_MET_Muon_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_ety_EndcapRegion", &MET_Muon_ety_EndcapRegion, &b_MET_Muon_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_sumet_EndcapRegion", &MET_Muon_sumet_EndcapRegion, &b_MET_Muon_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_phi_EndcapRegion", &MET_Muon_phi_EndcapRegion, &b_MET_Muon_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_etx_ForwardReg", &MET_Muon_etx_ForwardReg, &b_MET_Muon_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_ety_ForwardReg", &MET_Muon_ety_ForwardReg, &b_MET_Muon_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_sumet_ForwardReg", &MET_Muon_sumet_ForwardReg, &b_MET_Muon_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_phi_ForwardReg", &MET_Muon_phi_ForwardReg, &b_MET_Muon_phi_ForwardReg);
   fChain->SetBranchAddress("MET_TopoObj_etx", &MET_TopoObj_etx, &b_MET_TopoObj_etx);
   fChain->SetBranchAddress("MET_TopoObj_ety", &MET_TopoObj_ety, &b_MET_TopoObj_ety);
   fChain->SetBranchAddress("MET_TopoObj_phi", &MET_TopoObj_phi, &b_MET_TopoObj_phi);
   fChain->SetBranchAddress("MET_TopoObj_et", &MET_TopoObj_et, &b_MET_TopoObj_et);
   fChain->SetBranchAddress("MET_TopoObj_sumet", &MET_TopoObj_sumet, &b_MET_TopoObj_sumet);
   fChain->SetBranchAddress("MET_TopoObj_etx_CentralReg", &MET_TopoObj_etx_CentralReg, &b_MET_TopoObj_etx_CentralReg);
   fChain->SetBranchAddress("MET_TopoObj_ety_CentralReg", &MET_TopoObj_ety_CentralReg, &b_MET_TopoObj_ety_CentralReg);
   fChain->SetBranchAddress("MET_TopoObj_sumet_CentralReg", &MET_TopoObj_sumet_CentralReg, &b_MET_TopoObj_sumet_CentralReg);
   fChain->SetBranchAddress("MET_TopoObj_phi_CentralReg", &MET_TopoObj_phi_CentralReg, &b_MET_TopoObj_phi_CentralReg);
   fChain->SetBranchAddress("MET_TopoObj_etx_EndcapRegion", &MET_TopoObj_etx_EndcapRegion, &b_MET_TopoObj_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_TopoObj_ety_EndcapRegion", &MET_TopoObj_ety_EndcapRegion, &b_MET_TopoObj_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_TopoObj_sumet_EndcapRegion", &MET_TopoObj_sumet_EndcapRegion, &b_MET_TopoObj_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_TopoObj_phi_EndcapRegion", &MET_TopoObj_phi_EndcapRegion, &b_MET_TopoObj_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_TopoObj_etx_ForwardReg", &MET_TopoObj_etx_ForwardReg, &b_MET_TopoObj_etx_ForwardReg);
   fChain->SetBranchAddress("MET_TopoObj_ety_ForwardReg", &MET_TopoObj_ety_ForwardReg, &b_MET_TopoObj_ety_ForwardReg);
   fChain->SetBranchAddress("MET_TopoObj_sumet_ForwardReg", &MET_TopoObj_sumet_ForwardReg, &b_MET_TopoObj_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_TopoObj_phi_ForwardReg", &MET_TopoObj_phi_ForwardReg, &b_MET_TopoObj_phi_ForwardReg);
   fChain->SetBranchAddress("MET_LocHadTopoObj_etx", &MET_LocHadTopoObj_etx, &b_MET_LocHadTopoObj_etx);
   fChain->SetBranchAddress("MET_LocHadTopoObj_ety", &MET_LocHadTopoObj_ety, &b_MET_LocHadTopoObj_ety);
   fChain->SetBranchAddress("MET_LocHadTopoObj_phi", &MET_LocHadTopoObj_phi, &b_MET_LocHadTopoObj_phi);
   fChain->SetBranchAddress("MET_LocHadTopoObj_et", &MET_LocHadTopoObj_et, &b_MET_LocHadTopoObj_et);
   fChain->SetBranchAddress("MET_LocHadTopoObj_sumet", &MET_LocHadTopoObj_sumet, &b_MET_LocHadTopoObj_sumet);
   fChain->SetBranchAddress("MET_LocHadTopoObj_etx_CentralReg", &MET_LocHadTopoObj_etx_CentralReg, &b_MET_LocHadTopoObj_etx_CentralReg);
   fChain->SetBranchAddress("MET_LocHadTopoObj_ety_CentralReg", &MET_LocHadTopoObj_ety_CentralReg, &b_MET_LocHadTopoObj_ety_CentralReg);
   fChain->SetBranchAddress("MET_LocHadTopoObj_sumet_CentralReg", &MET_LocHadTopoObj_sumet_CentralReg, &b_MET_LocHadTopoObj_sumet_CentralReg);
   fChain->SetBranchAddress("MET_LocHadTopoObj_phi_CentralReg", &MET_LocHadTopoObj_phi_CentralReg, &b_MET_LocHadTopoObj_phi_CentralReg);
   fChain->SetBranchAddress("MET_LocHadTopoObj_etx_EndcapRegion", &MET_LocHadTopoObj_etx_EndcapRegion, &b_MET_LocHadTopoObj_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_LocHadTopoObj_ety_EndcapRegion", &MET_LocHadTopoObj_ety_EndcapRegion, &b_MET_LocHadTopoObj_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_LocHadTopoObj_sumet_EndcapRegion", &MET_LocHadTopoObj_sumet_EndcapRegion, &b_MET_LocHadTopoObj_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_LocHadTopoObj_phi_EndcapRegion", &MET_LocHadTopoObj_phi_EndcapRegion, &b_MET_LocHadTopoObj_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_LocHadTopoObj_etx_ForwardReg", &MET_LocHadTopoObj_etx_ForwardReg, &b_MET_LocHadTopoObj_etx_ForwardReg);
   fChain->SetBranchAddress("MET_LocHadTopoObj_ety_ForwardReg", &MET_LocHadTopoObj_ety_ForwardReg, &b_MET_LocHadTopoObj_ety_ForwardReg);
   fChain->SetBranchAddress("MET_LocHadTopoObj_sumet_ForwardReg", &MET_LocHadTopoObj_sumet_ForwardReg, &b_MET_LocHadTopoObj_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_LocHadTopoObj_phi_ForwardReg", &MET_LocHadTopoObj_phi_ForwardReg, &b_MET_LocHadTopoObj_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Topo_etx", &MET_Topo_etx, &b_MET_Topo_etx);
   fChain->SetBranchAddress("MET_Topo_ety", &MET_Topo_ety, &b_MET_Topo_ety);
   fChain->SetBranchAddress("MET_Topo_phi", &MET_Topo_phi, &b_MET_Topo_phi);
   fChain->SetBranchAddress("MET_Topo_et", &MET_Topo_et, &b_MET_Topo_et);
   fChain->SetBranchAddress("MET_Topo_sumet", &MET_Topo_sumet, &b_MET_Topo_sumet);
   fChain->SetBranchAddress("MET_Topo_SUMET_EMFrac", &MET_Topo_SUMET_EMFrac, &b_MET_Topo_SUMET_EMFrac);
   fChain->SetBranchAddress("MET_Topo_etx_PEMB", &MET_Topo_etx_PEMB, &b_MET_Topo_etx_PEMB);
   fChain->SetBranchAddress("MET_Topo_ety_PEMB", &MET_Topo_ety_PEMB, &b_MET_Topo_ety_PEMB);
   fChain->SetBranchAddress("MET_Topo_sumet_PEMB", &MET_Topo_sumet_PEMB, &b_MET_Topo_sumet_PEMB);
   fChain->SetBranchAddress("MET_Topo_phi_PEMB", &MET_Topo_phi_PEMB, &b_MET_Topo_phi_PEMB);
   fChain->SetBranchAddress("MET_Topo_etx_EMB", &MET_Topo_etx_EMB, &b_MET_Topo_etx_EMB);
   fChain->SetBranchAddress("MET_Topo_ety_EMB", &MET_Topo_ety_EMB, &b_MET_Topo_ety_EMB);
   fChain->SetBranchAddress("MET_Topo_sumet_EMB", &MET_Topo_sumet_EMB, &b_MET_Topo_sumet_EMB);
   fChain->SetBranchAddress("MET_Topo_phi_EMB", &MET_Topo_phi_EMB, &b_MET_Topo_phi_EMB);
   fChain->SetBranchAddress("MET_Topo_etx_PEMEC", &MET_Topo_etx_PEMEC, &b_MET_Topo_etx_PEMEC);
   fChain->SetBranchAddress("MET_Topo_ety_PEMEC", &MET_Topo_ety_PEMEC, &b_MET_Topo_ety_PEMEC);
   fChain->SetBranchAddress("MET_Topo_sumet_PEMEC", &MET_Topo_sumet_PEMEC, &b_MET_Topo_sumet_PEMEC);
   fChain->SetBranchAddress("MET_Topo_phi_PEMEC", &MET_Topo_phi_PEMEC, &b_MET_Topo_phi_PEMEC);
   fChain->SetBranchAddress("MET_Topo_etx_EME", &MET_Topo_etx_EME, &b_MET_Topo_etx_EME);
   fChain->SetBranchAddress("MET_Topo_ety_EME", &MET_Topo_ety_EME, &b_MET_Topo_ety_EME);
   fChain->SetBranchAddress("MET_Topo_sumet_EME", &MET_Topo_sumet_EME, &b_MET_Topo_sumet_EME);
   fChain->SetBranchAddress("MET_Topo_phi_EME", &MET_Topo_phi_EME, &b_MET_Topo_phi_EME);
   fChain->SetBranchAddress("MET_Topo_etx_TILE", &MET_Topo_etx_TILE, &b_MET_Topo_etx_TILE);
   fChain->SetBranchAddress("MET_Topo_ety_TILE", &MET_Topo_ety_TILE, &b_MET_Topo_ety_TILE);
   fChain->SetBranchAddress("MET_Topo_sumet_TILE", &MET_Topo_sumet_TILE, &b_MET_Topo_sumet_TILE);
   fChain->SetBranchAddress("MET_Topo_phi_TILE", &MET_Topo_phi_TILE, &b_MET_Topo_phi_TILE);
   fChain->SetBranchAddress("MET_Topo_etx_HEC", &MET_Topo_etx_HEC, &b_MET_Topo_etx_HEC);
   fChain->SetBranchAddress("MET_Topo_ety_HEC", &MET_Topo_ety_HEC, &b_MET_Topo_ety_HEC);
   fChain->SetBranchAddress("MET_Topo_sumet_HEC", &MET_Topo_sumet_HEC, &b_MET_Topo_sumet_HEC);
   fChain->SetBranchAddress("MET_Topo_phi_HEC", &MET_Topo_phi_HEC, &b_MET_Topo_phi_HEC);
   fChain->SetBranchAddress("MET_Topo_etx_FCAL", &MET_Topo_etx_FCAL, &b_MET_Topo_etx_FCAL);
   fChain->SetBranchAddress("MET_Topo_ety_FCAL", &MET_Topo_ety_FCAL, &b_MET_Topo_ety_FCAL);
   fChain->SetBranchAddress("MET_Topo_sumet_FCAL", &MET_Topo_sumet_FCAL, &b_MET_Topo_sumet_FCAL);
   fChain->SetBranchAddress("MET_Topo_phi_FCAL", &MET_Topo_phi_FCAL, &b_MET_Topo_phi_FCAL);
   fChain->SetBranchAddress("MET_Topo_nCell_PEMB", &MET_Topo_nCell_PEMB, &b_MET_Topo_nCell_PEMB);
   fChain->SetBranchAddress("MET_Topo_nCell_EMB", &MET_Topo_nCell_EMB, &b_MET_Topo_nCell_EMB);
   fChain->SetBranchAddress("MET_Topo_nCell_PEMEC", &MET_Topo_nCell_PEMEC, &b_MET_Topo_nCell_PEMEC);
   fChain->SetBranchAddress("MET_Topo_nCell_EME", &MET_Topo_nCell_EME, &b_MET_Topo_nCell_EME);
   fChain->SetBranchAddress("MET_Topo_nCell_TILE", &MET_Topo_nCell_TILE, &b_MET_Topo_nCell_TILE);
   fChain->SetBranchAddress("MET_Topo_nCell_HEC", &MET_Topo_nCell_HEC, &b_MET_Topo_nCell_HEC);
   fChain->SetBranchAddress("MET_Topo_nCell_FCAL", &MET_Topo_nCell_FCAL, &b_MET_Topo_nCell_FCAL);
   fChain->SetBranchAddress("MET_Topo_etx_CentralReg", &MET_Topo_etx_CentralReg, &b_MET_Topo_etx_CentralReg);
   fChain->SetBranchAddress("MET_Topo_ety_CentralReg", &MET_Topo_ety_CentralReg, &b_MET_Topo_ety_CentralReg);
   fChain->SetBranchAddress("MET_Topo_sumet_CentralReg", &MET_Topo_sumet_CentralReg, &b_MET_Topo_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Topo_phi_CentralReg", &MET_Topo_phi_CentralReg, &b_MET_Topo_phi_CentralReg);
   fChain->SetBranchAddress("MET_Topo_etx_EndcapRegion", &MET_Topo_etx_EndcapRegion, &b_MET_Topo_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Topo_ety_EndcapRegion", &MET_Topo_ety_EndcapRegion, &b_MET_Topo_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Topo_sumet_EndcapRegion", &MET_Topo_sumet_EndcapRegion, &b_MET_Topo_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Topo_phi_EndcapRegion", &MET_Topo_phi_EndcapRegion, &b_MET_Topo_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Topo_etx_ForwardReg", &MET_Topo_etx_ForwardReg, &b_MET_Topo_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Topo_ety_ForwardReg", &MET_Topo_ety_ForwardReg, &b_MET_Topo_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Topo_sumet_ForwardReg", &MET_Topo_sumet_ForwardReg, &b_MET_Topo_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Topo_phi_ForwardReg", &MET_Topo_phi_ForwardReg, &b_MET_Topo_phi_ForwardReg);
   fChain->SetBranchAddress("MET_CorrTopo_etx", &MET_CorrTopo_etx, &b_MET_CorrTopo_etx);
   fChain->SetBranchAddress("MET_CorrTopo_ety", &MET_CorrTopo_ety, &b_MET_CorrTopo_ety);
   fChain->SetBranchAddress("MET_CorrTopo_phi", &MET_CorrTopo_phi, &b_MET_CorrTopo_phi);
   fChain->SetBranchAddress("MET_CorrTopo_et", &MET_CorrTopo_et, &b_MET_CorrTopo_et);
   fChain->SetBranchAddress("MET_CorrTopo_sumet", &MET_CorrTopo_sumet, &b_MET_CorrTopo_sumet);
   fChain->SetBranchAddress("MET_CorrTopo_SUMET_EMFrac", &MET_CorrTopo_SUMET_EMFrac, &b_MET_CorrTopo_SUMET_EMFrac);
   fChain->SetBranchAddress("MET_CorrTopo_etx_PEMB", &MET_CorrTopo_etx_PEMB, &b_MET_CorrTopo_etx_PEMB);
   fChain->SetBranchAddress("MET_CorrTopo_ety_PEMB", &MET_CorrTopo_ety_PEMB, &b_MET_CorrTopo_ety_PEMB);
   fChain->SetBranchAddress("MET_CorrTopo_sumet_PEMB", &MET_CorrTopo_sumet_PEMB, &b_MET_CorrTopo_sumet_PEMB);
   fChain->SetBranchAddress("MET_CorrTopo_phi_PEMB", &MET_CorrTopo_phi_PEMB, &b_MET_CorrTopo_phi_PEMB);
   fChain->SetBranchAddress("MET_CorrTopo_etx_EMB", &MET_CorrTopo_etx_EMB, &b_MET_CorrTopo_etx_EMB);
   fChain->SetBranchAddress("MET_CorrTopo_ety_EMB", &MET_CorrTopo_ety_EMB, &b_MET_CorrTopo_ety_EMB);
   fChain->SetBranchAddress("MET_CorrTopo_sumet_EMB", &MET_CorrTopo_sumet_EMB, &b_MET_CorrTopo_sumet_EMB);
   fChain->SetBranchAddress("MET_CorrTopo_phi_EMB", &MET_CorrTopo_phi_EMB, &b_MET_CorrTopo_phi_EMB);
   fChain->SetBranchAddress("MET_CorrTopo_etx_PEMEC", &MET_CorrTopo_etx_PEMEC, &b_MET_CorrTopo_etx_PEMEC);
   fChain->SetBranchAddress("MET_CorrTopo_ety_PEMEC", &MET_CorrTopo_ety_PEMEC, &b_MET_CorrTopo_ety_PEMEC);
   fChain->SetBranchAddress("MET_CorrTopo_sumet_PEMEC", &MET_CorrTopo_sumet_PEMEC, &b_MET_CorrTopo_sumet_PEMEC);
   fChain->SetBranchAddress("MET_CorrTopo_phi_PEMEC", &MET_CorrTopo_phi_PEMEC, &b_MET_CorrTopo_phi_PEMEC);
   fChain->SetBranchAddress("MET_CorrTopo_etx_EME", &MET_CorrTopo_etx_EME, &b_MET_CorrTopo_etx_EME);
   fChain->SetBranchAddress("MET_CorrTopo_ety_EME", &MET_CorrTopo_ety_EME, &b_MET_CorrTopo_ety_EME);
   fChain->SetBranchAddress("MET_CorrTopo_sumet_EME", &MET_CorrTopo_sumet_EME, &b_MET_CorrTopo_sumet_EME);
   fChain->SetBranchAddress("MET_CorrTopo_phi_EME", &MET_CorrTopo_phi_EME, &b_MET_CorrTopo_phi_EME);
   fChain->SetBranchAddress("MET_CorrTopo_etx_TILE", &MET_CorrTopo_etx_TILE, &b_MET_CorrTopo_etx_TILE);
   fChain->SetBranchAddress("MET_CorrTopo_ety_TILE", &MET_CorrTopo_ety_TILE, &b_MET_CorrTopo_ety_TILE);
   fChain->SetBranchAddress("MET_CorrTopo_sumet_TILE", &MET_CorrTopo_sumet_TILE, &b_MET_CorrTopo_sumet_TILE);
   fChain->SetBranchAddress("MET_CorrTopo_phi_TILE", &MET_CorrTopo_phi_TILE, &b_MET_CorrTopo_phi_TILE);
   fChain->SetBranchAddress("MET_CorrTopo_etx_HEC", &MET_CorrTopo_etx_HEC, &b_MET_CorrTopo_etx_HEC);
   fChain->SetBranchAddress("MET_CorrTopo_ety_HEC", &MET_CorrTopo_ety_HEC, &b_MET_CorrTopo_ety_HEC);
   fChain->SetBranchAddress("MET_CorrTopo_sumet_HEC", &MET_CorrTopo_sumet_HEC, &b_MET_CorrTopo_sumet_HEC);
   fChain->SetBranchAddress("MET_CorrTopo_phi_HEC", &MET_CorrTopo_phi_HEC, &b_MET_CorrTopo_phi_HEC);
   fChain->SetBranchAddress("MET_CorrTopo_etx_FCAL", &MET_CorrTopo_etx_FCAL, &b_MET_CorrTopo_etx_FCAL);
   fChain->SetBranchAddress("MET_CorrTopo_ety_FCAL", &MET_CorrTopo_ety_FCAL, &b_MET_CorrTopo_ety_FCAL);
   fChain->SetBranchAddress("MET_CorrTopo_sumet_FCAL", &MET_CorrTopo_sumet_FCAL, &b_MET_CorrTopo_sumet_FCAL);
   fChain->SetBranchAddress("MET_CorrTopo_phi_FCAL", &MET_CorrTopo_phi_FCAL, &b_MET_CorrTopo_phi_FCAL);
   fChain->SetBranchAddress("MET_CorrTopo_nCell_PEMB", &MET_CorrTopo_nCell_PEMB, &b_MET_CorrTopo_nCell_PEMB);
   fChain->SetBranchAddress("MET_CorrTopo_nCell_EMB", &MET_CorrTopo_nCell_EMB, &b_MET_CorrTopo_nCell_EMB);
   fChain->SetBranchAddress("MET_CorrTopo_nCell_PEMEC", &MET_CorrTopo_nCell_PEMEC, &b_MET_CorrTopo_nCell_PEMEC);
   fChain->SetBranchAddress("MET_CorrTopo_nCell_EME", &MET_CorrTopo_nCell_EME, &b_MET_CorrTopo_nCell_EME);
   fChain->SetBranchAddress("MET_CorrTopo_nCell_TILE", &MET_CorrTopo_nCell_TILE, &b_MET_CorrTopo_nCell_TILE);
   fChain->SetBranchAddress("MET_CorrTopo_nCell_HEC", &MET_CorrTopo_nCell_HEC, &b_MET_CorrTopo_nCell_HEC);
   fChain->SetBranchAddress("MET_CorrTopo_nCell_FCAL", &MET_CorrTopo_nCell_FCAL, &b_MET_CorrTopo_nCell_FCAL);
   fChain->SetBranchAddress("MET_CorrTopo_etx_CentralReg", &MET_CorrTopo_etx_CentralReg, &b_MET_CorrTopo_etx_CentralReg);
   fChain->SetBranchAddress("MET_CorrTopo_ety_CentralReg", &MET_CorrTopo_ety_CentralReg, &b_MET_CorrTopo_ety_CentralReg);
   fChain->SetBranchAddress("MET_CorrTopo_sumet_CentralReg", &MET_CorrTopo_sumet_CentralReg, &b_MET_CorrTopo_sumet_CentralReg);
   fChain->SetBranchAddress("MET_CorrTopo_phi_CentralReg", &MET_CorrTopo_phi_CentralReg, &b_MET_CorrTopo_phi_CentralReg);
   fChain->SetBranchAddress("MET_CorrTopo_etx_EndcapRegion", &MET_CorrTopo_etx_EndcapRegion, &b_MET_CorrTopo_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_CorrTopo_ety_EndcapRegion", &MET_CorrTopo_ety_EndcapRegion, &b_MET_CorrTopo_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_CorrTopo_sumet_EndcapRegion", &MET_CorrTopo_sumet_EndcapRegion, &b_MET_CorrTopo_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_CorrTopo_phi_EndcapRegion", &MET_CorrTopo_phi_EndcapRegion, &b_MET_CorrTopo_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_CorrTopo_etx_ForwardReg", &MET_CorrTopo_etx_ForwardReg, &b_MET_CorrTopo_etx_ForwardReg);
   fChain->SetBranchAddress("MET_CorrTopo_ety_ForwardReg", &MET_CorrTopo_ety_ForwardReg, &b_MET_CorrTopo_ety_ForwardReg);
   fChain->SetBranchAddress("MET_CorrTopo_sumet_ForwardReg", &MET_CorrTopo_sumet_ForwardReg, &b_MET_CorrTopo_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_CorrTopo_phi_ForwardReg", &MET_CorrTopo_phi_ForwardReg, &b_MET_CorrTopo_phi_ForwardReg);
   fChain->SetBranchAddress("MET_LocHadTopo_etx", &MET_LocHadTopo_etx, &b_MET_LocHadTopo_etx);
   fChain->SetBranchAddress("MET_LocHadTopo_ety", &MET_LocHadTopo_ety, &b_MET_LocHadTopo_ety);
   fChain->SetBranchAddress("MET_LocHadTopo_phi", &MET_LocHadTopo_phi, &b_MET_LocHadTopo_phi);
   fChain->SetBranchAddress("MET_LocHadTopo_et", &MET_LocHadTopo_et, &b_MET_LocHadTopo_et);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet", &MET_LocHadTopo_sumet, &b_MET_LocHadTopo_sumet);
   fChain->SetBranchAddress("MET_LocHadTopo_SUMET_EMFrac", &MET_LocHadTopo_SUMET_EMFrac, &b_MET_LocHadTopo_SUMET_EMFrac);
   fChain->SetBranchAddress("MET_LocHadTopo_etx_PEMB", &MET_LocHadTopo_etx_PEMB, &b_MET_LocHadTopo_etx_PEMB);
   fChain->SetBranchAddress("MET_LocHadTopo_ety_PEMB", &MET_LocHadTopo_ety_PEMB, &b_MET_LocHadTopo_ety_PEMB);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet_PEMB", &MET_LocHadTopo_sumet_PEMB, &b_MET_LocHadTopo_sumet_PEMB);
   fChain->SetBranchAddress("MET_LocHadTopo_phi_PEMB", &MET_LocHadTopo_phi_PEMB, &b_MET_LocHadTopo_phi_PEMB);
   fChain->SetBranchAddress("MET_LocHadTopo_etx_EMB", &MET_LocHadTopo_etx_EMB, &b_MET_LocHadTopo_etx_EMB);
   fChain->SetBranchAddress("MET_LocHadTopo_ety_EMB", &MET_LocHadTopo_ety_EMB, &b_MET_LocHadTopo_ety_EMB);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet_EMB", &MET_LocHadTopo_sumet_EMB, &b_MET_LocHadTopo_sumet_EMB);
   fChain->SetBranchAddress("MET_LocHadTopo_phi_EMB", &MET_LocHadTopo_phi_EMB, &b_MET_LocHadTopo_phi_EMB);
   fChain->SetBranchAddress("MET_LocHadTopo_etx_PEMEC", &MET_LocHadTopo_etx_PEMEC, &b_MET_LocHadTopo_etx_PEMEC);
   fChain->SetBranchAddress("MET_LocHadTopo_ety_PEMEC", &MET_LocHadTopo_ety_PEMEC, &b_MET_LocHadTopo_ety_PEMEC);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet_PEMEC", &MET_LocHadTopo_sumet_PEMEC, &b_MET_LocHadTopo_sumet_PEMEC);
   fChain->SetBranchAddress("MET_LocHadTopo_phi_PEMEC", &MET_LocHadTopo_phi_PEMEC, &b_MET_LocHadTopo_phi_PEMEC);
   fChain->SetBranchAddress("MET_LocHadTopo_etx_EME", &MET_LocHadTopo_etx_EME, &b_MET_LocHadTopo_etx_EME);
   fChain->SetBranchAddress("MET_LocHadTopo_ety_EME", &MET_LocHadTopo_ety_EME, &b_MET_LocHadTopo_ety_EME);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet_EME", &MET_LocHadTopo_sumet_EME, &b_MET_LocHadTopo_sumet_EME);
   fChain->SetBranchAddress("MET_LocHadTopo_phi_EME", &MET_LocHadTopo_phi_EME, &b_MET_LocHadTopo_phi_EME);
   fChain->SetBranchAddress("MET_LocHadTopo_etx_TILE", &MET_LocHadTopo_etx_TILE, &b_MET_LocHadTopo_etx_TILE);
   fChain->SetBranchAddress("MET_LocHadTopo_ety_TILE", &MET_LocHadTopo_ety_TILE, &b_MET_LocHadTopo_ety_TILE);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet_TILE", &MET_LocHadTopo_sumet_TILE, &b_MET_LocHadTopo_sumet_TILE);
   fChain->SetBranchAddress("MET_LocHadTopo_phi_TILE", &MET_LocHadTopo_phi_TILE, &b_MET_LocHadTopo_phi_TILE);
   fChain->SetBranchAddress("MET_LocHadTopo_etx_HEC", &MET_LocHadTopo_etx_HEC, &b_MET_LocHadTopo_etx_HEC);
   fChain->SetBranchAddress("MET_LocHadTopo_ety_HEC", &MET_LocHadTopo_ety_HEC, &b_MET_LocHadTopo_ety_HEC);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet_HEC", &MET_LocHadTopo_sumet_HEC, &b_MET_LocHadTopo_sumet_HEC);
   fChain->SetBranchAddress("MET_LocHadTopo_phi_HEC", &MET_LocHadTopo_phi_HEC, &b_MET_LocHadTopo_phi_HEC);
   fChain->SetBranchAddress("MET_LocHadTopo_etx_FCAL", &MET_LocHadTopo_etx_FCAL, &b_MET_LocHadTopo_etx_FCAL);
   fChain->SetBranchAddress("MET_LocHadTopo_ety_FCAL", &MET_LocHadTopo_ety_FCAL, &b_MET_LocHadTopo_ety_FCAL);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet_FCAL", &MET_LocHadTopo_sumet_FCAL, &b_MET_LocHadTopo_sumet_FCAL);
   fChain->SetBranchAddress("MET_LocHadTopo_phi_FCAL", &MET_LocHadTopo_phi_FCAL, &b_MET_LocHadTopo_phi_FCAL);
   fChain->SetBranchAddress("MET_LocHadTopo_nCell_PEMB", &MET_LocHadTopo_nCell_PEMB, &b_MET_LocHadTopo_nCell_PEMB);
   fChain->SetBranchAddress("MET_LocHadTopo_nCell_EMB", &MET_LocHadTopo_nCell_EMB, &b_MET_LocHadTopo_nCell_EMB);
   fChain->SetBranchAddress("MET_LocHadTopo_nCell_PEMEC", &MET_LocHadTopo_nCell_PEMEC, &b_MET_LocHadTopo_nCell_PEMEC);
   fChain->SetBranchAddress("MET_LocHadTopo_nCell_EME", &MET_LocHadTopo_nCell_EME, &b_MET_LocHadTopo_nCell_EME);
   fChain->SetBranchAddress("MET_LocHadTopo_nCell_TILE", &MET_LocHadTopo_nCell_TILE, &b_MET_LocHadTopo_nCell_TILE);
   fChain->SetBranchAddress("MET_LocHadTopo_nCell_HEC", &MET_LocHadTopo_nCell_HEC, &b_MET_LocHadTopo_nCell_HEC);
   fChain->SetBranchAddress("MET_LocHadTopo_nCell_FCAL", &MET_LocHadTopo_nCell_FCAL, &b_MET_LocHadTopo_nCell_FCAL);
   fChain->SetBranchAddress("MET_LocHadTopo_etx_CentralReg", &MET_LocHadTopo_etx_CentralReg, &b_MET_LocHadTopo_etx_CentralReg);
   fChain->SetBranchAddress("MET_LocHadTopo_ety_CentralReg", &MET_LocHadTopo_ety_CentralReg, &b_MET_LocHadTopo_ety_CentralReg);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet_CentralReg", &MET_LocHadTopo_sumet_CentralReg, &b_MET_LocHadTopo_sumet_CentralReg);
   fChain->SetBranchAddress("MET_LocHadTopo_phi_CentralReg", &MET_LocHadTopo_phi_CentralReg, &b_MET_LocHadTopo_phi_CentralReg);
   fChain->SetBranchAddress("MET_LocHadTopo_etx_EndcapRegion", &MET_LocHadTopo_etx_EndcapRegion, &b_MET_LocHadTopo_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_LocHadTopo_ety_EndcapRegion", &MET_LocHadTopo_ety_EndcapRegion, &b_MET_LocHadTopo_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet_EndcapRegion", &MET_LocHadTopo_sumet_EndcapRegion, &b_MET_LocHadTopo_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_LocHadTopo_phi_EndcapRegion", &MET_LocHadTopo_phi_EndcapRegion, &b_MET_LocHadTopo_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_LocHadTopo_etx_ForwardReg", &MET_LocHadTopo_etx_ForwardReg, &b_MET_LocHadTopo_etx_ForwardReg);
   fChain->SetBranchAddress("MET_LocHadTopo_ety_ForwardReg", &MET_LocHadTopo_ety_ForwardReg, &b_MET_LocHadTopo_ety_ForwardReg);
   fChain->SetBranchAddress("MET_LocHadTopo_sumet_ForwardReg", &MET_LocHadTopo_sumet_ForwardReg, &b_MET_LocHadTopo_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_LocHadTopo_phi_ForwardReg", &MET_LocHadTopo_phi_ForwardReg, &b_MET_LocHadTopo_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_etx", &MET_RefFinal_GCW_pt20_etx, &b_MET_RefFinal_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_ety", &MET_RefFinal_GCW_pt20_ety, &b_MET_RefFinal_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_phi", &MET_RefFinal_GCW_pt20_phi, &b_MET_RefFinal_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_et", &MET_RefFinal_GCW_pt20_et, &b_MET_RefFinal_GCW_pt20_et);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_sumet", &MET_RefFinal_GCW_pt20_sumet, &b_MET_RefFinal_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_etx_CentralReg", &MET_RefFinal_GCW_pt20_etx_CentralReg, &b_MET_RefFinal_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_ety_CentralReg", &MET_RefFinal_GCW_pt20_ety_CentralReg, &b_MET_RefFinal_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_sumet_CentralReg", &MET_RefFinal_GCW_pt20_sumet_CentralReg, &b_MET_RefFinal_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_phi_CentralReg", &MET_RefFinal_GCW_pt20_phi_CentralReg, &b_MET_RefFinal_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_etx_EndcapRegion", &MET_RefFinal_GCW_pt20_etx_EndcapRegion, &b_MET_RefFinal_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_ety_EndcapRegion", &MET_RefFinal_GCW_pt20_ety_EndcapRegion, &b_MET_RefFinal_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_sumet_EndcapRegion", &MET_RefFinal_GCW_pt20_sumet_EndcapRegion, &b_MET_RefFinal_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_phi_EndcapRegion", &MET_RefFinal_GCW_pt20_phi_EndcapRegion, &b_MET_RefFinal_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_etx_ForwardReg", &MET_RefFinal_GCW_pt20_etx_ForwardReg, &b_MET_RefFinal_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_ety_ForwardReg", &MET_RefFinal_GCW_pt20_ety_ForwardReg, &b_MET_RefFinal_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_sumet_ForwardReg", &MET_RefFinal_GCW_pt20_sumet_ForwardReg, &b_MET_RefFinal_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_GCW_pt20_phi_ForwardReg", &MET_RefFinal_GCW_pt20_phi_ForwardReg, &b_MET_RefFinal_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_etx", &MET_RefFinal_LCW_NI_eflow4_etx, &b_MET_RefFinal_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_ety", &MET_RefFinal_LCW_NI_eflow4_ety, &b_MET_RefFinal_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_phi", &MET_RefFinal_LCW_NI_eflow4_phi, &b_MET_RefFinal_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_et", &MET_RefFinal_LCW_NI_eflow4_et, &b_MET_RefFinal_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_sumet", &MET_RefFinal_LCW_NI_eflow4_sumet, &b_MET_RefFinal_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_etx_CentralReg", &MET_RefFinal_LCW_NI_eflow4_etx_CentralReg, &b_MET_RefFinal_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_ety_CentralReg", &MET_RefFinal_LCW_NI_eflow4_ety_CentralReg, &b_MET_RefFinal_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_sumet_CentralReg", &MET_RefFinal_LCW_NI_eflow4_sumet_CentralReg, &b_MET_RefFinal_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_phi_CentralReg", &MET_RefFinal_LCW_NI_eflow4_phi_CentralReg, &b_MET_RefFinal_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_etx_EndcapRegion", &MET_RefFinal_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_ety_EndcapRegion", &MET_RefFinal_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_sumet_EndcapRegion", &MET_RefFinal_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_phi_EndcapRegion", &MET_RefFinal_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_etx_ForwardReg", &MET_RefFinal_LCW_NI_eflow4_etx_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_ety_ForwardReg", &MET_RefFinal_LCW_NI_eflow4_ety_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_sumet_ForwardReg", &MET_RefFinal_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow4_phi_ForwardReg", &MET_RefFinal_LCW_NI_eflow4_phi_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_etx", &MET_RefFinal_LCW_NI_eflow_CB_etx, &b_MET_RefFinal_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_ety", &MET_RefFinal_LCW_NI_eflow_CB_ety, &b_MET_RefFinal_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_phi", &MET_RefFinal_LCW_NI_eflow_CB_phi, &b_MET_RefFinal_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_et", &MET_RefFinal_LCW_NI_eflow_CB_et, &b_MET_RefFinal_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_sumet", &MET_RefFinal_LCW_NI_eflow_CB_sumet, &b_MET_RefFinal_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_etx_CentralReg", &MET_RefFinal_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_RefFinal_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_ety_CentralReg", &MET_RefFinal_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_RefFinal_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_sumet_CentralReg", &MET_RefFinal_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_RefFinal_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_phi_CentralReg", &MET_RefFinal_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_RefFinal_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_RefFinal_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_RefFinal_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_RefFinal_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_RefFinal_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_etx_ForwardReg", &MET_RefFinal_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_ety_ForwardReg", &MET_RefFinal_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_RefFinal_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_CB_phi_ForwardReg", &MET_RefFinal_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_etx", &MET_RefFinal_LCW_NI_eflow_400_etx, &b_MET_RefFinal_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_ety", &MET_RefFinal_LCW_NI_eflow_400_ety, &b_MET_RefFinal_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_phi", &MET_RefFinal_LCW_NI_eflow_400_phi, &b_MET_RefFinal_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_et", &MET_RefFinal_LCW_NI_eflow_400_et, &b_MET_RefFinal_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_sumet", &MET_RefFinal_LCW_NI_eflow_400_sumet, &b_MET_RefFinal_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_etx_CentralReg", &MET_RefFinal_LCW_NI_eflow_400_etx_CentralReg, &b_MET_RefFinal_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_ety_CentralReg", &MET_RefFinal_LCW_NI_eflow_400_ety_CentralReg, &b_MET_RefFinal_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_sumet_CentralReg", &MET_RefFinal_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_RefFinal_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_phi_CentralReg", &MET_RefFinal_LCW_NI_eflow_400_phi_CentralReg, &b_MET_RefFinal_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_etx_EndcapRegion", &MET_RefFinal_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_ety_EndcapRegion", &MET_RefFinal_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_RefFinal_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_phi_EndcapRegion", &MET_RefFinal_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_RefFinal_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_etx_ForwardReg", &MET_RefFinal_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_ety_ForwardReg", &MET_RefFinal_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_sumet_ForwardReg", &MET_RefFinal_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_NI_eflow_400_phi_ForwardReg", &MET_RefFinal_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_RefFinal_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_etx", &MET_RefGamma_GCW_pt20_etx, &b_MET_RefGamma_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_ety", &MET_RefGamma_GCW_pt20_ety, &b_MET_RefGamma_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_phi", &MET_RefGamma_GCW_pt20_phi, &b_MET_RefGamma_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_et", &MET_RefGamma_GCW_pt20_et, &b_MET_RefGamma_GCW_pt20_et);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_sumet", &MET_RefGamma_GCW_pt20_sumet, &b_MET_RefGamma_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_etx_CentralReg", &MET_RefGamma_GCW_pt20_etx_CentralReg, &b_MET_RefGamma_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_ety_CentralReg", &MET_RefGamma_GCW_pt20_ety_CentralReg, &b_MET_RefGamma_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_sumet_CentralReg", &MET_RefGamma_GCW_pt20_sumet_CentralReg, &b_MET_RefGamma_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_phi_CentralReg", &MET_RefGamma_GCW_pt20_phi_CentralReg, &b_MET_RefGamma_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_etx_EndcapRegion", &MET_RefGamma_GCW_pt20_etx_EndcapRegion, &b_MET_RefGamma_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_ety_EndcapRegion", &MET_RefGamma_GCW_pt20_ety_EndcapRegion, &b_MET_RefGamma_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_sumet_EndcapRegion", &MET_RefGamma_GCW_pt20_sumet_EndcapRegion, &b_MET_RefGamma_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_phi_EndcapRegion", &MET_RefGamma_GCW_pt20_phi_EndcapRegion, &b_MET_RefGamma_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_etx_ForwardReg", &MET_RefGamma_GCW_pt20_etx_ForwardReg, &b_MET_RefGamma_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_ety_ForwardReg", &MET_RefGamma_GCW_pt20_ety_ForwardReg, &b_MET_RefGamma_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_sumet_ForwardReg", &MET_RefGamma_GCW_pt20_sumet_ForwardReg, &b_MET_RefGamma_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_GCW_pt20_phi_ForwardReg", &MET_RefGamma_GCW_pt20_phi_ForwardReg, &b_MET_RefGamma_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_etx", &MET_RefGamma_LCW_NI_eflow4_etx, &b_MET_RefGamma_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_ety", &MET_RefGamma_LCW_NI_eflow4_ety, &b_MET_RefGamma_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_phi", &MET_RefGamma_LCW_NI_eflow4_phi, &b_MET_RefGamma_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_et", &MET_RefGamma_LCW_NI_eflow4_et, &b_MET_RefGamma_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_sumet", &MET_RefGamma_LCW_NI_eflow4_sumet, &b_MET_RefGamma_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_etx_CentralReg", &MET_RefGamma_LCW_NI_eflow4_etx_CentralReg, &b_MET_RefGamma_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_ety_CentralReg", &MET_RefGamma_LCW_NI_eflow4_ety_CentralReg, &b_MET_RefGamma_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_sumet_CentralReg", &MET_RefGamma_LCW_NI_eflow4_sumet_CentralReg, &b_MET_RefGamma_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_phi_CentralReg", &MET_RefGamma_LCW_NI_eflow4_phi_CentralReg, &b_MET_RefGamma_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_etx_EndcapRegion", &MET_RefGamma_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_ety_EndcapRegion", &MET_RefGamma_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_sumet_EndcapRegion", &MET_RefGamma_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_phi_EndcapRegion", &MET_RefGamma_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_etx_ForwardReg", &MET_RefGamma_LCW_NI_eflow4_etx_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_ety_ForwardReg", &MET_RefGamma_LCW_NI_eflow4_ety_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_sumet_ForwardReg", &MET_RefGamma_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow4_phi_ForwardReg", &MET_RefGamma_LCW_NI_eflow4_phi_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_etx", &MET_RefGamma_LCW_NI_eflow_CB_etx, &b_MET_RefGamma_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_ety", &MET_RefGamma_LCW_NI_eflow_CB_ety, &b_MET_RefGamma_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_phi", &MET_RefGamma_LCW_NI_eflow_CB_phi, &b_MET_RefGamma_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_et", &MET_RefGamma_LCW_NI_eflow_CB_et, &b_MET_RefGamma_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_sumet", &MET_RefGamma_LCW_NI_eflow_CB_sumet, &b_MET_RefGamma_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_etx_CentralReg", &MET_RefGamma_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_RefGamma_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_ety_CentralReg", &MET_RefGamma_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_RefGamma_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_sumet_CentralReg", &MET_RefGamma_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_RefGamma_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_phi_CentralReg", &MET_RefGamma_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_RefGamma_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_RefGamma_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_RefGamma_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_RefGamma_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_RefGamma_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_etx_ForwardReg", &MET_RefGamma_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_ety_ForwardReg", &MET_RefGamma_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_RefGamma_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_CB_phi_ForwardReg", &MET_RefGamma_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_etx", &MET_RefGamma_LCW_NI_eflow_400_etx, &b_MET_RefGamma_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_ety", &MET_RefGamma_LCW_NI_eflow_400_ety, &b_MET_RefGamma_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_phi", &MET_RefGamma_LCW_NI_eflow_400_phi, &b_MET_RefGamma_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_et", &MET_RefGamma_LCW_NI_eflow_400_et, &b_MET_RefGamma_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_sumet", &MET_RefGamma_LCW_NI_eflow_400_sumet, &b_MET_RefGamma_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_etx_CentralReg", &MET_RefGamma_LCW_NI_eflow_400_etx_CentralReg, &b_MET_RefGamma_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_ety_CentralReg", &MET_RefGamma_LCW_NI_eflow_400_ety_CentralReg, &b_MET_RefGamma_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_sumet_CentralReg", &MET_RefGamma_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_RefGamma_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_phi_CentralReg", &MET_RefGamma_LCW_NI_eflow_400_phi_CentralReg, &b_MET_RefGamma_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_etx_EndcapRegion", &MET_RefGamma_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_ety_EndcapRegion", &MET_RefGamma_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_RefGamma_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_phi_EndcapRegion", &MET_RefGamma_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_RefGamma_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_etx_ForwardReg", &MET_RefGamma_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_ety_ForwardReg", &MET_RefGamma_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_sumet_ForwardReg", &MET_RefGamma_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_NI_eflow_400_phi_ForwardReg", &MET_RefGamma_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_RefGamma_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_etx", &MET_RefEle_GCW_pt20_etx, &b_MET_RefEle_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_ety", &MET_RefEle_GCW_pt20_ety, &b_MET_RefEle_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_phi", &MET_RefEle_GCW_pt20_phi, &b_MET_RefEle_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_et", &MET_RefEle_GCW_pt20_et, &b_MET_RefEle_GCW_pt20_et);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_sumet", &MET_RefEle_GCW_pt20_sumet, &b_MET_RefEle_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_etx_CentralReg", &MET_RefEle_GCW_pt20_etx_CentralReg, &b_MET_RefEle_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_ety_CentralReg", &MET_RefEle_GCW_pt20_ety_CentralReg, &b_MET_RefEle_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_sumet_CentralReg", &MET_RefEle_GCW_pt20_sumet_CentralReg, &b_MET_RefEle_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_phi_CentralReg", &MET_RefEle_GCW_pt20_phi_CentralReg, &b_MET_RefEle_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_etx_EndcapRegion", &MET_RefEle_GCW_pt20_etx_EndcapRegion, &b_MET_RefEle_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_ety_EndcapRegion", &MET_RefEle_GCW_pt20_ety_EndcapRegion, &b_MET_RefEle_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_sumet_EndcapRegion", &MET_RefEle_GCW_pt20_sumet_EndcapRegion, &b_MET_RefEle_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_phi_EndcapRegion", &MET_RefEle_GCW_pt20_phi_EndcapRegion, &b_MET_RefEle_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_etx_ForwardReg", &MET_RefEle_GCW_pt20_etx_ForwardReg, &b_MET_RefEle_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_ety_ForwardReg", &MET_RefEle_GCW_pt20_ety_ForwardReg, &b_MET_RefEle_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_sumet_ForwardReg", &MET_RefEle_GCW_pt20_sumet_ForwardReg, &b_MET_RefEle_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_GCW_pt20_phi_ForwardReg", &MET_RefEle_GCW_pt20_phi_ForwardReg, &b_MET_RefEle_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_etx", &MET_RefEle_LCW_NI_eflow4_etx, &b_MET_RefEle_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_ety", &MET_RefEle_LCW_NI_eflow4_ety, &b_MET_RefEle_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_phi", &MET_RefEle_LCW_NI_eflow4_phi, &b_MET_RefEle_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_et", &MET_RefEle_LCW_NI_eflow4_et, &b_MET_RefEle_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_sumet", &MET_RefEle_LCW_NI_eflow4_sumet, &b_MET_RefEle_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_etx_CentralReg", &MET_RefEle_LCW_NI_eflow4_etx_CentralReg, &b_MET_RefEle_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_ety_CentralReg", &MET_RefEle_LCW_NI_eflow4_ety_CentralReg, &b_MET_RefEle_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_sumet_CentralReg", &MET_RefEle_LCW_NI_eflow4_sumet_CentralReg, &b_MET_RefEle_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_phi_CentralReg", &MET_RefEle_LCW_NI_eflow4_phi_CentralReg, &b_MET_RefEle_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_etx_EndcapRegion", &MET_RefEle_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_ety_EndcapRegion", &MET_RefEle_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_sumet_EndcapRegion", &MET_RefEle_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_phi_EndcapRegion", &MET_RefEle_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_etx_ForwardReg", &MET_RefEle_LCW_NI_eflow4_etx_ForwardReg, &b_MET_RefEle_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_ety_ForwardReg", &MET_RefEle_LCW_NI_eflow4_ety_ForwardReg, &b_MET_RefEle_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_sumet_ForwardReg", &MET_RefEle_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_RefEle_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow4_phi_ForwardReg", &MET_RefEle_LCW_NI_eflow4_phi_ForwardReg, &b_MET_RefEle_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_etx", &MET_RefEle_LCW_NI_eflow_CB_etx, &b_MET_RefEle_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_ety", &MET_RefEle_LCW_NI_eflow_CB_ety, &b_MET_RefEle_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_phi", &MET_RefEle_LCW_NI_eflow_CB_phi, &b_MET_RefEle_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_et", &MET_RefEle_LCW_NI_eflow_CB_et, &b_MET_RefEle_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_sumet", &MET_RefEle_LCW_NI_eflow_CB_sumet, &b_MET_RefEle_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_etx_CentralReg", &MET_RefEle_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_RefEle_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_ety_CentralReg", &MET_RefEle_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_RefEle_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_sumet_CentralReg", &MET_RefEle_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_RefEle_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_phi_CentralReg", &MET_RefEle_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_RefEle_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_RefEle_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_RefEle_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_RefEle_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_RefEle_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_etx_ForwardReg", &MET_RefEle_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_RefEle_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_ety_ForwardReg", &MET_RefEle_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_RefEle_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_RefEle_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_RefEle_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_CB_phi_ForwardReg", &MET_RefEle_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_RefEle_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_etx", &MET_RefEle_LCW_NI_eflow_400_etx, &b_MET_RefEle_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_ety", &MET_RefEle_LCW_NI_eflow_400_ety, &b_MET_RefEle_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_phi", &MET_RefEle_LCW_NI_eflow_400_phi, &b_MET_RefEle_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_et", &MET_RefEle_LCW_NI_eflow_400_et, &b_MET_RefEle_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_sumet", &MET_RefEle_LCW_NI_eflow_400_sumet, &b_MET_RefEle_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_etx_CentralReg", &MET_RefEle_LCW_NI_eflow_400_etx_CentralReg, &b_MET_RefEle_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_ety_CentralReg", &MET_RefEle_LCW_NI_eflow_400_ety_CentralReg, &b_MET_RefEle_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_sumet_CentralReg", &MET_RefEle_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_RefEle_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_phi_CentralReg", &MET_RefEle_LCW_NI_eflow_400_phi_CentralReg, &b_MET_RefEle_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_etx_EndcapRegion", &MET_RefEle_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_ety_EndcapRegion", &MET_RefEle_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_RefEle_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_phi_EndcapRegion", &MET_RefEle_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_RefEle_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_etx_ForwardReg", &MET_RefEle_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_RefEle_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_ety_ForwardReg", &MET_RefEle_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_RefEle_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_sumet_ForwardReg", &MET_RefEle_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_RefEle_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_NI_eflow_400_phi_ForwardReg", &MET_RefEle_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_RefEle_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_etx", &MET_RefTau_GCW_pt20_etx, &b_MET_RefTau_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_ety", &MET_RefTau_GCW_pt20_ety, &b_MET_RefTau_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_phi", &MET_RefTau_GCW_pt20_phi, &b_MET_RefTau_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_et", &MET_RefTau_GCW_pt20_et, &b_MET_RefTau_GCW_pt20_et);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_sumet", &MET_RefTau_GCW_pt20_sumet, &b_MET_RefTau_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_etx_CentralReg", &MET_RefTau_GCW_pt20_etx_CentralReg, &b_MET_RefTau_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_ety_CentralReg", &MET_RefTau_GCW_pt20_ety_CentralReg, &b_MET_RefTau_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_sumet_CentralReg", &MET_RefTau_GCW_pt20_sumet_CentralReg, &b_MET_RefTau_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_phi_CentralReg", &MET_RefTau_GCW_pt20_phi_CentralReg, &b_MET_RefTau_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_etx_EndcapRegion", &MET_RefTau_GCW_pt20_etx_EndcapRegion, &b_MET_RefTau_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_ety_EndcapRegion", &MET_RefTau_GCW_pt20_ety_EndcapRegion, &b_MET_RefTau_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_sumet_EndcapRegion", &MET_RefTau_GCW_pt20_sumet_EndcapRegion, &b_MET_RefTau_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_phi_EndcapRegion", &MET_RefTau_GCW_pt20_phi_EndcapRegion, &b_MET_RefTau_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_etx_ForwardReg", &MET_RefTau_GCW_pt20_etx_ForwardReg, &b_MET_RefTau_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_ety_ForwardReg", &MET_RefTau_GCW_pt20_ety_ForwardReg, &b_MET_RefTau_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_sumet_ForwardReg", &MET_RefTau_GCW_pt20_sumet_ForwardReg, &b_MET_RefTau_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_GCW_pt20_phi_ForwardReg", &MET_RefTau_GCW_pt20_phi_ForwardReg, &b_MET_RefTau_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_etx", &MET_RefTau_LCW_NI_eflow4_etx, &b_MET_RefTau_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_ety", &MET_RefTau_LCW_NI_eflow4_ety, &b_MET_RefTau_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_phi", &MET_RefTau_LCW_NI_eflow4_phi, &b_MET_RefTau_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_et", &MET_RefTau_LCW_NI_eflow4_et, &b_MET_RefTau_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_sumet", &MET_RefTau_LCW_NI_eflow4_sumet, &b_MET_RefTau_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_etx_CentralReg", &MET_RefTau_LCW_NI_eflow4_etx_CentralReg, &b_MET_RefTau_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_ety_CentralReg", &MET_RefTau_LCW_NI_eflow4_ety_CentralReg, &b_MET_RefTau_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_sumet_CentralReg", &MET_RefTau_LCW_NI_eflow4_sumet_CentralReg, &b_MET_RefTau_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_phi_CentralReg", &MET_RefTau_LCW_NI_eflow4_phi_CentralReg, &b_MET_RefTau_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_etx_EndcapRegion", &MET_RefTau_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_ety_EndcapRegion", &MET_RefTau_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_sumet_EndcapRegion", &MET_RefTau_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_phi_EndcapRegion", &MET_RefTau_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_etx_ForwardReg", &MET_RefTau_LCW_NI_eflow4_etx_ForwardReg, &b_MET_RefTau_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_ety_ForwardReg", &MET_RefTau_LCW_NI_eflow4_ety_ForwardReg, &b_MET_RefTau_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_sumet_ForwardReg", &MET_RefTau_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_RefTau_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow4_phi_ForwardReg", &MET_RefTau_LCW_NI_eflow4_phi_ForwardReg, &b_MET_RefTau_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_etx", &MET_RefTau_LCW_NI_eflow_CB_etx, &b_MET_RefTau_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_ety", &MET_RefTau_LCW_NI_eflow_CB_ety, &b_MET_RefTau_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_phi", &MET_RefTau_LCW_NI_eflow_CB_phi, &b_MET_RefTau_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_et", &MET_RefTau_LCW_NI_eflow_CB_et, &b_MET_RefTau_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_sumet", &MET_RefTau_LCW_NI_eflow_CB_sumet, &b_MET_RefTau_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_etx_CentralReg", &MET_RefTau_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_RefTau_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_ety_CentralReg", &MET_RefTau_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_RefTau_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_sumet_CentralReg", &MET_RefTau_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_RefTau_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_phi_CentralReg", &MET_RefTau_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_RefTau_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_RefTau_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_RefTau_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_RefTau_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_RefTau_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_etx_ForwardReg", &MET_RefTau_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_RefTau_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_ety_ForwardReg", &MET_RefTau_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_RefTau_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_RefTau_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_RefTau_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_CB_phi_ForwardReg", &MET_RefTau_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_RefTau_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_etx", &MET_RefTau_LCW_NI_eflow_400_etx, &b_MET_RefTau_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_ety", &MET_RefTau_LCW_NI_eflow_400_ety, &b_MET_RefTau_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_phi", &MET_RefTau_LCW_NI_eflow_400_phi, &b_MET_RefTau_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_et", &MET_RefTau_LCW_NI_eflow_400_et, &b_MET_RefTau_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_sumet", &MET_RefTau_LCW_NI_eflow_400_sumet, &b_MET_RefTau_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_etx_CentralReg", &MET_RefTau_LCW_NI_eflow_400_etx_CentralReg, &b_MET_RefTau_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_ety_CentralReg", &MET_RefTau_LCW_NI_eflow_400_ety_CentralReg, &b_MET_RefTau_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_sumet_CentralReg", &MET_RefTau_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_RefTau_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_phi_CentralReg", &MET_RefTau_LCW_NI_eflow_400_phi_CentralReg, &b_MET_RefTau_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_etx_EndcapRegion", &MET_RefTau_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_ety_EndcapRegion", &MET_RefTau_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_RefTau_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_phi_EndcapRegion", &MET_RefTau_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_RefTau_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_etx_ForwardReg", &MET_RefTau_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_RefTau_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_ety_ForwardReg", &MET_RefTau_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_RefTau_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_sumet_ForwardReg", &MET_RefTau_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_RefTau_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_NI_eflow_400_phi_ForwardReg", &MET_RefTau_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_RefTau_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_etx", &MET_RefJet_GCW_pt20_etx, &b_MET_RefJet_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_ety", &MET_RefJet_GCW_pt20_ety, &b_MET_RefJet_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_phi", &MET_RefJet_GCW_pt20_phi, &b_MET_RefJet_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_et", &MET_RefJet_GCW_pt20_et, &b_MET_RefJet_GCW_pt20_et);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_sumet", &MET_RefJet_GCW_pt20_sumet, &b_MET_RefJet_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_etx_CentralReg", &MET_RefJet_GCW_pt20_etx_CentralReg, &b_MET_RefJet_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_ety_CentralReg", &MET_RefJet_GCW_pt20_ety_CentralReg, &b_MET_RefJet_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_sumet_CentralReg", &MET_RefJet_GCW_pt20_sumet_CentralReg, &b_MET_RefJet_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_phi_CentralReg", &MET_RefJet_GCW_pt20_phi_CentralReg, &b_MET_RefJet_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_etx_EndcapRegion", &MET_RefJet_GCW_pt20_etx_EndcapRegion, &b_MET_RefJet_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_ety_EndcapRegion", &MET_RefJet_GCW_pt20_ety_EndcapRegion, &b_MET_RefJet_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_sumet_EndcapRegion", &MET_RefJet_GCW_pt20_sumet_EndcapRegion, &b_MET_RefJet_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_phi_EndcapRegion", &MET_RefJet_GCW_pt20_phi_EndcapRegion, &b_MET_RefJet_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_etx_ForwardReg", &MET_RefJet_GCW_pt20_etx_ForwardReg, &b_MET_RefJet_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_ety_ForwardReg", &MET_RefJet_GCW_pt20_ety_ForwardReg, &b_MET_RefJet_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_sumet_ForwardReg", &MET_RefJet_GCW_pt20_sumet_ForwardReg, &b_MET_RefJet_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_GCW_pt20_phi_ForwardReg", &MET_RefJet_GCW_pt20_phi_ForwardReg, &b_MET_RefJet_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_etx", &MET_RefJet_LCW_NI_eflow4_etx, &b_MET_RefJet_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_ety", &MET_RefJet_LCW_NI_eflow4_ety, &b_MET_RefJet_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_phi", &MET_RefJet_LCW_NI_eflow4_phi, &b_MET_RefJet_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_et", &MET_RefJet_LCW_NI_eflow4_et, &b_MET_RefJet_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_sumet", &MET_RefJet_LCW_NI_eflow4_sumet, &b_MET_RefJet_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_etx_CentralReg", &MET_RefJet_LCW_NI_eflow4_etx_CentralReg, &b_MET_RefJet_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_ety_CentralReg", &MET_RefJet_LCW_NI_eflow4_ety_CentralReg, &b_MET_RefJet_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_sumet_CentralReg", &MET_RefJet_LCW_NI_eflow4_sumet_CentralReg, &b_MET_RefJet_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_phi_CentralReg", &MET_RefJet_LCW_NI_eflow4_phi_CentralReg, &b_MET_RefJet_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_etx_EndcapRegion", &MET_RefJet_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_ety_EndcapRegion", &MET_RefJet_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_sumet_EndcapRegion", &MET_RefJet_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_phi_EndcapRegion", &MET_RefJet_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_etx_ForwardReg", &MET_RefJet_LCW_NI_eflow4_etx_ForwardReg, &b_MET_RefJet_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_ety_ForwardReg", &MET_RefJet_LCW_NI_eflow4_ety_ForwardReg, &b_MET_RefJet_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_sumet_ForwardReg", &MET_RefJet_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_RefJet_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow4_phi_ForwardReg", &MET_RefJet_LCW_NI_eflow4_phi_ForwardReg, &b_MET_RefJet_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_etx", &MET_RefJet_LCW_NI_eflow_CB_etx, &b_MET_RefJet_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_ety", &MET_RefJet_LCW_NI_eflow_CB_ety, &b_MET_RefJet_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_phi", &MET_RefJet_LCW_NI_eflow_CB_phi, &b_MET_RefJet_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_et", &MET_RefJet_LCW_NI_eflow_CB_et, &b_MET_RefJet_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_sumet", &MET_RefJet_LCW_NI_eflow_CB_sumet, &b_MET_RefJet_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_etx_CentralReg", &MET_RefJet_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_RefJet_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_ety_CentralReg", &MET_RefJet_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_RefJet_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_sumet_CentralReg", &MET_RefJet_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_RefJet_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_phi_CentralReg", &MET_RefJet_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_RefJet_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_RefJet_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_RefJet_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_RefJet_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_RefJet_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_etx_ForwardReg", &MET_RefJet_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_RefJet_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_ety_ForwardReg", &MET_RefJet_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_RefJet_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_RefJet_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_RefJet_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_CB_phi_ForwardReg", &MET_RefJet_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_RefJet_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_etx", &MET_RefJet_LCW_NI_eflow_400_etx, &b_MET_RefJet_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_ety", &MET_RefJet_LCW_NI_eflow_400_ety, &b_MET_RefJet_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_phi", &MET_RefJet_LCW_NI_eflow_400_phi, &b_MET_RefJet_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_et", &MET_RefJet_LCW_NI_eflow_400_et, &b_MET_RefJet_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_sumet", &MET_RefJet_LCW_NI_eflow_400_sumet, &b_MET_RefJet_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_etx_CentralReg", &MET_RefJet_LCW_NI_eflow_400_etx_CentralReg, &b_MET_RefJet_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_ety_CentralReg", &MET_RefJet_LCW_NI_eflow_400_ety_CentralReg, &b_MET_RefJet_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_sumet_CentralReg", &MET_RefJet_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_RefJet_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_phi_CentralReg", &MET_RefJet_LCW_NI_eflow_400_phi_CentralReg, &b_MET_RefJet_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_etx_EndcapRegion", &MET_RefJet_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_ety_EndcapRegion", &MET_RefJet_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_RefJet_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_phi_EndcapRegion", &MET_RefJet_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_RefJet_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_etx_ForwardReg", &MET_RefJet_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_RefJet_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_ety_ForwardReg", &MET_RefJet_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_RefJet_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_sumet_ForwardReg", &MET_RefJet_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_RefJet_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_NI_eflow_400_phi_ForwardReg", &MET_RefJet_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_RefJet_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_etx", &MET_RefMuon_GCW_pt20_etx, &b_MET_RefMuon_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_ety", &MET_RefMuon_GCW_pt20_ety, &b_MET_RefMuon_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_phi", &MET_RefMuon_GCW_pt20_phi, &b_MET_RefMuon_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_et", &MET_RefMuon_GCW_pt20_et, &b_MET_RefMuon_GCW_pt20_et);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_sumet", &MET_RefMuon_GCW_pt20_sumet, &b_MET_RefMuon_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_etx_CentralReg", &MET_RefMuon_GCW_pt20_etx_CentralReg, &b_MET_RefMuon_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_ety_CentralReg", &MET_RefMuon_GCW_pt20_ety_CentralReg, &b_MET_RefMuon_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_sumet_CentralReg", &MET_RefMuon_GCW_pt20_sumet_CentralReg, &b_MET_RefMuon_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_phi_CentralReg", &MET_RefMuon_GCW_pt20_phi_CentralReg, &b_MET_RefMuon_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_etx_EndcapRegion", &MET_RefMuon_GCW_pt20_etx_EndcapRegion, &b_MET_RefMuon_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_ety_EndcapRegion", &MET_RefMuon_GCW_pt20_ety_EndcapRegion, &b_MET_RefMuon_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_sumet_EndcapRegion", &MET_RefMuon_GCW_pt20_sumet_EndcapRegion, &b_MET_RefMuon_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_phi_EndcapRegion", &MET_RefMuon_GCW_pt20_phi_EndcapRegion, &b_MET_RefMuon_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_etx_ForwardReg", &MET_RefMuon_GCW_pt20_etx_ForwardReg, &b_MET_RefMuon_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_ety_ForwardReg", &MET_RefMuon_GCW_pt20_ety_ForwardReg, &b_MET_RefMuon_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_sumet_ForwardReg", &MET_RefMuon_GCW_pt20_sumet_ForwardReg, &b_MET_RefMuon_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_GCW_pt20_phi_ForwardReg", &MET_RefMuon_GCW_pt20_phi_ForwardReg, &b_MET_RefMuon_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_etx", &MET_RefMuon_LCW_NI_eflow4_etx, &b_MET_RefMuon_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_ety", &MET_RefMuon_LCW_NI_eflow4_ety, &b_MET_RefMuon_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_phi", &MET_RefMuon_LCW_NI_eflow4_phi, &b_MET_RefMuon_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_et", &MET_RefMuon_LCW_NI_eflow4_et, &b_MET_RefMuon_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_sumet", &MET_RefMuon_LCW_NI_eflow4_sumet, &b_MET_RefMuon_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_etx_CentralReg", &MET_RefMuon_LCW_NI_eflow4_etx_CentralReg, &b_MET_RefMuon_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_ety_CentralReg", &MET_RefMuon_LCW_NI_eflow4_ety_CentralReg, &b_MET_RefMuon_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_sumet_CentralReg", &MET_RefMuon_LCW_NI_eflow4_sumet_CentralReg, &b_MET_RefMuon_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_phi_CentralReg", &MET_RefMuon_LCW_NI_eflow4_phi_CentralReg, &b_MET_RefMuon_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_etx_EndcapRegion", &MET_RefMuon_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_ety_EndcapRegion", &MET_RefMuon_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_sumet_EndcapRegion", &MET_RefMuon_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_phi_EndcapRegion", &MET_RefMuon_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_etx_ForwardReg", &MET_RefMuon_LCW_NI_eflow4_etx_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_ety_ForwardReg", &MET_RefMuon_LCW_NI_eflow4_ety_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_sumet_ForwardReg", &MET_RefMuon_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow4_phi_ForwardReg", &MET_RefMuon_LCW_NI_eflow4_phi_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_etx", &MET_RefMuon_LCW_NI_eflow_CB_etx, &b_MET_RefMuon_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_ety", &MET_RefMuon_LCW_NI_eflow_CB_ety, &b_MET_RefMuon_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_phi", &MET_RefMuon_LCW_NI_eflow_CB_phi, &b_MET_RefMuon_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_et", &MET_RefMuon_LCW_NI_eflow_CB_et, &b_MET_RefMuon_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_sumet", &MET_RefMuon_LCW_NI_eflow_CB_sumet, &b_MET_RefMuon_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_etx_CentralReg", &MET_RefMuon_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_RefMuon_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_ety_CentralReg", &MET_RefMuon_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_RefMuon_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_sumet_CentralReg", &MET_RefMuon_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_RefMuon_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_phi_CentralReg", &MET_RefMuon_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_RefMuon_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_RefMuon_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_RefMuon_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_RefMuon_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_RefMuon_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_etx_ForwardReg", &MET_RefMuon_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_ety_ForwardReg", &MET_RefMuon_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_RefMuon_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_CB_phi_ForwardReg", &MET_RefMuon_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_etx", &MET_RefMuon_LCW_NI_eflow_400_etx, &b_MET_RefMuon_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_ety", &MET_RefMuon_LCW_NI_eflow_400_ety, &b_MET_RefMuon_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_phi", &MET_RefMuon_LCW_NI_eflow_400_phi, &b_MET_RefMuon_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_et", &MET_RefMuon_LCW_NI_eflow_400_et, &b_MET_RefMuon_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_sumet", &MET_RefMuon_LCW_NI_eflow_400_sumet, &b_MET_RefMuon_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_etx_CentralReg", &MET_RefMuon_LCW_NI_eflow_400_etx_CentralReg, &b_MET_RefMuon_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_ety_CentralReg", &MET_RefMuon_LCW_NI_eflow_400_ety_CentralReg, &b_MET_RefMuon_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_sumet_CentralReg", &MET_RefMuon_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_RefMuon_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_phi_CentralReg", &MET_RefMuon_LCW_NI_eflow_400_phi_CentralReg, &b_MET_RefMuon_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_etx_EndcapRegion", &MET_RefMuon_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_ety_EndcapRegion", &MET_RefMuon_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_RefMuon_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_phi_EndcapRegion", &MET_RefMuon_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_RefMuon_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_etx_ForwardReg", &MET_RefMuon_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_ety_ForwardReg", &MET_RefMuon_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_sumet_ForwardReg", &MET_RefMuon_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_NI_eflow_400_phi_ForwardReg", &MET_RefMuon_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_RefMuon_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_etx", &MET_RefMuon_Staco_GCW_pt20_etx, &b_MET_RefMuon_Staco_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_ety", &MET_RefMuon_Staco_GCW_pt20_ety, &b_MET_RefMuon_Staco_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_phi", &MET_RefMuon_Staco_GCW_pt20_phi, &b_MET_RefMuon_Staco_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_et", &MET_RefMuon_Staco_GCW_pt20_et, &b_MET_RefMuon_Staco_GCW_pt20_et);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_sumet", &MET_RefMuon_Staco_GCW_pt20_sumet, &b_MET_RefMuon_Staco_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_etx_CentralReg", &MET_RefMuon_Staco_GCW_pt20_etx_CentralReg, &b_MET_RefMuon_Staco_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_ety_CentralReg", &MET_RefMuon_Staco_GCW_pt20_ety_CentralReg, &b_MET_RefMuon_Staco_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_sumet_CentralReg", &MET_RefMuon_Staco_GCW_pt20_sumet_CentralReg, &b_MET_RefMuon_Staco_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_phi_CentralReg", &MET_RefMuon_Staco_GCW_pt20_phi_CentralReg, &b_MET_RefMuon_Staco_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_etx_EndcapRegion", &MET_RefMuon_Staco_GCW_pt20_etx_EndcapRegion, &b_MET_RefMuon_Staco_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_ety_EndcapRegion", &MET_RefMuon_Staco_GCW_pt20_ety_EndcapRegion, &b_MET_RefMuon_Staco_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_sumet_EndcapRegion", &MET_RefMuon_Staco_GCW_pt20_sumet_EndcapRegion, &b_MET_RefMuon_Staco_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_phi_EndcapRegion", &MET_RefMuon_Staco_GCW_pt20_phi_EndcapRegion, &b_MET_RefMuon_Staco_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_etx_ForwardReg", &MET_RefMuon_Staco_GCW_pt20_etx_ForwardReg, &b_MET_RefMuon_Staco_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_ety_ForwardReg", &MET_RefMuon_Staco_GCW_pt20_ety_ForwardReg, &b_MET_RefMuon_Staco_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_sumet_ForwardReg", &MET_RefMuon_Staco_GCW_pt20_sumet_ForwardReg, &b_MET_RefMuon_Staco_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_GCW_pt20_phi_ForwardReg", &MET_RefMuon_Staco_GCW_pt20_phi_ForwardReg, &b_MET_RefMuon_Staco_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_etx", &MET_RefMuon_Staco_LCW_NI_eflow4_etx, &b_MET_RefMuon_Staco_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_ety", &MET_RefMuon_Staco_LCW_NI_eflow4_ety, &b_MET_RefMuon_Staco_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_phi", &MET_RefMuon_Staco_LCW_NI_eflow4_phi, &b_MET_RefMuon_Staco_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_et", &MET_RefMuon_Staco_LCW_NI_eflow4_et, &b_MET_RefMuon_Staco_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_sumet", &MET_RefMuon_Staco_LCW_NI_eflow4_sumet, &b_MET_RefMuon_Staco_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_etx_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow4_etx_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_ety_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow4_ety_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_sumet_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow4_sumet_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_phi_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow4_phi_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_etx_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_ety_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_sumet_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_phi_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_etx_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow4_etx_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_ety_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow4_ety_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_sumet_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow4_phi_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow4_phi_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_etx", &MET_RefMuon_Staco_LCW_NI_eflow_CB_etx, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_ety", &MET_RefMuon_Staco_LCW_NI_eflow_CB_ety, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_phi", &MET_RefMuon_Staco_LCW_NI_eflow_CB_phi, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_et", &MET_RefMuon_Staco_LCW_NI_eflow_CB_et, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet", &MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_etx", &MET_RefMuon_Staco_LCW_NI_eflow_400_etx, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_ety", &MET_RefMuon_Staco_LCW_NI_eflow_400_ety, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_phi", &MET_RefMuon_Staco_LCW_NI_eflow_400_phi, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_et", &MET_RefMuon_Staco_LCW_NI_eflow_400_et, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_sumet", &MET_RefMuon_Staco_LCW_NI_eflow_400_sumet, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_etx_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow_400_etx_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_ety_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow_400_ety_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_phi_CentralReg", &MET_RefMuon_Staco_LCW_NI_eflow_400_phi_CentralReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_etx_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_ety_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_phi_EndcapRegion", &MET_RefMuon_Staco_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_etx_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_ety_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Staco_LCW_NI_eflow_400_phi_ForwardReg", &MET_RefMuon_Staco_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_RefMuon_Staco_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_etx", &MET_CellOut_GCW_pt20_etx, &b_MET_CellOut_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_ety", &MET_CellOut_GCW_pt20_ety, &b_MET_CellOut_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_phi", &MET_CellOut_GCW_pt20_phi, &b_MET_CellOut_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_et", &MET_CellOut_GCW_pt20_et, &b_MET_CellOut_GCW_pt20_et);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_sumet", &MET_CellOut_GCW_pt20_sumet, &b_MET_CellOut_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_etx_CentralReg", &MET_CellOut_GCW_pt20_etx_CentralReg, &b_MET_CellOut_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_ety_CentralReg", &MET_CellOut_GCW_pt20_ety_CentralReg, &b_MET_CellOut_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_sumet_CentralReg", &MET_CellOut_GCW_pt20_sumet_CentralReg, &b_MET_CellOut_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_phi_CentralReg", &MET_CellOut_GCW_pt20_phi_CentralReg, &b_MET_CellOut_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_etx_EndcapRegion", &MET_CellOut_GCW_pt20_etx_EndcapRegion, &b_MET_CellOut_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_ety_EndcapRegion", &MET_CellOut_GCW_pt20_ety_EndcapRegion, &b_MET_CellOut_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_sumet_EndcapRegion", &MET_CellOut_GCW_pt20_sumet_EndcapRegion, &b_MET_CellOut_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_phi_EndcapRegion", &MET_CellOut_GCW_pt20_phi_EndcapRegion, &b_MET_CellOut_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_etx_ForwardReg", &MET_CellOut_GCW_pt20_etx_ForwardReg, &b_MET_CellOut_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_ety_ForwardReg", &MET_CellOut_GCW_pt20_ety_ForwardReg, &b_MET_CellOut_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_sumet_ForwardReg", &MET_CellOut_GCW_pt20_sumet_ForwardReg, &b_MET_CellOut_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_GCW_pt20_phi_ForwardReg", &MET_CellOut_GCW_pt20_phi_ForwardReg, &b_MET_CellOut_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_etx", &MET_CellOut_LCW_NI_eflow4_etx, &b_MET_CellOut_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_ety", &MET_CellOut_LCW_NI_eflow4_ety, &b_MET_CellOut_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_phi", &MET_CellOut_LCW_NI_eflow4_phi, &b_MET_CellOut_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_et", &MET_CellOut_LCW_NI_eflow4_et, &b_MET_CellOut_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_sumet", &MET_CellOut_LCW_NI_eflow4_sumet, &b_MET_CellOut_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_etx_CentralReg", &MET_CellOut_LCW_NI_eflow4_etx_CentralReg, &b_MET_CellOut_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_ety_CentralReg", &MET_CellOut_LCW_NI_eflow4_ety_CentralReg, &b_MET_CellOut_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_sumet_CentralReg", &MET_CellOut_LCW_NI_eflow4_sumet_CentralReg, &b_MET_CellOut_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_phi_CentralReg", &MET_CellOut_LCW_NI_eflow4_phi_CentralReg, &b_MET_CellOut_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_etx_EndcapRegion", &MET_CellOut_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_ety_EndcapRegion", &MET_CellOut_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_sumet_EndcapRegion", &MET_CellOut_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_phi_EndcapRegion", &MET_CellOut_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_etx_ForwardReg", &MET_CellOut_LCW_NI_eflow4_etx_ForwardReg, &b_MET_CellOut_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_ety_ForwardReg", &MET_CellOut_LCW_NI_eflow4_ety_ForwardReg, &b_MET_CellOut_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_sumet_ForwardReg", &MET_CellOut_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_CellOut_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow4_phi_ForwardReg", &MET_CellOut_LCW_NI_eflow4_phi_ForwardReg, &b_MET_CellOut_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_etx", &MET_CellOut_LCW_NI_eflow_CB_etx, &b_MET_CellOut_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_ety", &MET_CellOut_LCW_NI_eflow_CB_ety, &b_MET_CellOut_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_phi", &MET_CellOut_LCW_NI_eflow_CB_phi, &b_MET_CellOut_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_et", &MET_CellOut_LCW_NI_eflow_CB_et, &b_MET_CellOut_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_sumet", &MET_CellOut_LCW_NI_eflow_CB_sumet, &b_MET_CellOut_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_etx_CentralReg", &MET_CellOut_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_CellOut_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_ety_CentralReg", &MET_CellOut_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_CellOut_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_sumet_CentralReg", &MET_CellOut_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_CellOut_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_phi_CentralReg", &MET_CellOut_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_CellOut_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_CellOut_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_CellOut_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_CellOut_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_CellOut_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_etx_ForwardReg", &MET_CellOut_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_CellOut_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_ety_ForwardReg", &MET_CellOut_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_CellOut_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_CellOut_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_CellOut_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_CB_phi_ForwardReg", &MET_CellOut_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_CellOut_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_etx", &MET_CellOut_LCW_NI_eflow_400_etx, &b_MET_CellOut_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_ety", &MET_CellOut_LCW_NI_eflow_400_ety, &b_MET_CellOut_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_phi", &MET_CellOut_LCW_NI_eflow_400_phi, &b_MET_CellOut_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_et", &MET_CellOut_LCW_NI_eflow_400_et, &b_MET_CellOut_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_sumet", &MET_CellOut_LCW_NI_eflow_400_sumet, &b_MET_CellOut_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_etx_CentralReg", &MET_CellOut_LCW_NI_eflow_400_etx_CentralReg, &b_MET_CellOut_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_ety_CentralReg", &MET_CellOut_LCW_NI_eflow_400_ety_CentralReg, &b_MET_CellOut_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_sumet_CentralReg", &MET_CellOut_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_CellOut_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_phi_CentralReg", &MET_CellOut_LCW_NI_eflow_400_phi_CentralReg, &b_MET_CellOut_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_etx_EndcapRegion", &MET_CellOut_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_ety_EndcapRegion", &MET_CellOut_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_CellOut_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_phi_EndcapRegion", &MET_CellOut_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_CellOut_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_etx_ForwardReg", &MET_CellOut_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_CellOut_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_ety_ForwardReg", &MET_CellOut_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_CellOut_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_sumet_ForwardReg", &MET_CellOut_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_CellOut_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_NI_eflow_400_phi_ForwardReg", &MET_CellOut_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_CellOut_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_etx", &MET_Cryo_GCW_pt20_etx, &b_MET_Cryo_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_ety", &MET_Cryo_GCW_pt20_ety, &b_MET_Cryo_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_phi", &MET_Cryo_GCW_pt20_phi, &b_MET_Cryo_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_et", &MET_Cryo_GCW_pt20_et, &b_MET_Cryo_GCW_pt20_et);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_sumet", &MET_Cryo_GCW_pt20_sumet, &b_MET_Cryo_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_etx_CentralReg", &MET_Cryo_GCW_pt20_etx_CentralReg, &b_MET_Cryo_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_ety_CentralReg", &MET_Cryo_GCW_pt20_ety_CentralReg, &b_MET_Cryo_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_sumet_CentralReg", &MET_Cryo_GCW_pt20_sumet_CentralReg, &b_MET_Cryo_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_phi_CentralReg", &MET_Cryo_GCW_pt20_phi_CentralReg, &b_MET_Cryo_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_etx_EndcapRegion", &MET_Cryo_GCW_pt20_etx_EndcapRegion, &b_MET_Cryo_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_ety_EndcapRegion", &MET_Cryo_GCW_pt20_ety_EndcapRegion, &b_MET_Cryo_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_sumet_EndcapRegion", &MET_Cryo_GCW_pt20_sumet_EndcapRegion, &b_MET_Cryo_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_phi_EndcapRegion", &MET_Cryo_GCW_pt20_phi_EndcapRegion, &b_MET_Cryo_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_etx_ForwardReg", &MET_Cryo_GCW_pt20_etx_ForwardReg, &b_MET_Cryo_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_ety_ForwardReg", &MET_Cryo_GCW_pt20_ety_ForwardReg, &b_MET_Cryo_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_sumet_ForwardReg", &MET_Cryo_GCW_pt20_sumet_ForwardReg, &b_MET_Cryo_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_GCW_pt20_phi_ForwardReg", &MET_Cryo_GCW_pt20_phi_ForwardReg, &b_MET_Cryo_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_etx", &MET_Cryo_LCW_NI_eflow4_etx, &b_MET_Cryo_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_ety", &MET_Cryo_LCW_NI_eflow4_ety, &b_MET_Cryo_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_phi", &MET_Cryo_LCW_NI_eflow4_phi, &b_MET_Cryo_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_et", &MET_Cryo_LCW_NI_eflow4_et, &b_MET_Cryo_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_sumet", &MET_Cryo_LCW_NI_eflow4_sumet, &b_MET_Cryo_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_etx_CentralReg", &MET_Cryo_LCW_NI_eflow4_etx_CentralReg, &b_MET_Cryo_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_ety_CentralReg", &MET_Cryo_LCW_NI_eflow4_ety_CentralReg, &b_MET_Cryo_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_sumet_CentralReg", &MET_Cryo_LCW_NI_eflow4_sumet_CentralReg, &b_MET_Cryo_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_phi_CentralReg", &MET_Cryo_LCW_NI_eflow4_phi_CentralReg, &b_MET_Cryo_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_etx_EndcapRegion", &MET_Cryo_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_ety_EndcapRegion", &MET_Cryo_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_sumet_EndcapRegion", &MET_Cryo_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_phi_EndcapRegion", &MET_Cryo_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_etx_ForwardReg", &MET_Cryo_LCW_NI_eflow4_etx_ForwardReg, &b_MET_Cryo_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_ety_ForwardReg", &MET_Cryo_LCW_NI_eflow4_ety_ForwardReg, &b_MET_Cryo_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_sumet_ForwardReg", &MET_Cryo_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_Cryo_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow4_phi_ForwardReg", &MET_Cryo_LCW_NI_eflow4_phi_ForwardReg, &b_MET_Cryo_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_etx", &MET_Cryo_LCW_NI_eflow_CB_etx, &b_MET_Cryo_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_ety", &MET_Cryo_LCW_NI_eflow_CB_ety, &b_MET_Cryo_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_phi", &MET_Cryo_LCW_NI_eflow_CB_phi, &b_MET_Cryo_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_et", &MET_Cryo_LCW_NI_eflow_CB_et, &b_MET_Cryo_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_sumet", &MET_Cryo_LCW_NI_eflow_CB_sumet, &b_MET_Cryo_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_etx_CentralReg", &MET_Cryo_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_Cryo_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_ety_CentralReg", &MET_Cryo_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_Cryo_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_sumet_CentralReg", &MET_Cryo_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_Cryo_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_phi_CentralReg", &MET_Cryo_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_Cryo_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_Cryo_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_Cryo_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_Cryo_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_Cryo_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_etx_ForwardReg", &MET_Cryo_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_Cryo_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_ety_ForwardReg", &MET_Cryo_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_Cryo_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_Cryo_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_Cryo_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_CB_phi_ForwardReg", &MET_Cryo_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_Cryo_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_etx", &MET_Cryo_LCW_NI_eflow_400_etx, &b_MET_Cryo_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_ety", &MET_Cryo_LCW_NI_eflow_400_ety, &b_MET_Cryo_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_phi", &MET_Cryo_LCW_NI_eflow_400_phi, &b_MET_Cryo_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_et", &MET_Cryo_LCW_NI_eflow_400_et, &b_MET_Cryo_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_sumet", &MET_Cryo_LCW_NI_eflow_400_sumet, &b_MET_Cryo_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_etx_CentralReg", &MET_Cryo_LCW_NI_eflow_400_etx_CentralReg, &b_MET_Cryo_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_ety_CentralReg", &MET_Cryo_LCW_NI_eflow_400_ety_CentralReg, &b_MET_Cryo_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_sumet_CentralReg", &MET_Cryo_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_Cryo_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_phi_CentralReg", &MET_Cryo_LCW_NI_eflow_400_phi_CentralReg, &b_MET_Cryo_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_etx_EndcapRegion", &MET_Cryo_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_ety_EndcapRegion", &MET_Cryo_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_Cryo_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_phi_EndcapRegion", &MET_Cryo_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_Cryo_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_etx_ForwardReg", &MET_Cryo_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_Cryo_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_ety_ForwardReg", &MET_Cryo_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_Cryo_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_sumet_ForwardReg", &MET_Cryo_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_Cryo_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_NI_eflow_400_phi_ForwardReg", &MET_Cryo_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_Cryo_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_etx", &MET_Muon_Isol_Staco_GCW_pt20_etx, &b_MET_Muon_Isol_Staco_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_ety", &MET_Muon_Isol_Staco_GCW_pt20_ety, &b_MET_Muon_Isol_Staco_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_phi", &MET_Muon_Isol_Staco_GCW_pt20_phi, &b_MET_Muon_Isol_Staco_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_et", &MET_Muon_Isol_Staco_GCW_pt20_et, &b_MET_Muon_Isol_Staco_GCW_pt20_et);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_sumet", &MET_Muon_Isol_Staco_GCW_pt20_sumet, &b_MET_Muon_Isol_Staco_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_etx_CentralReg", &MET_Muon_Isol_Staco_GCW_pt20_etx_CentralReg, &b_MET_Muon_Isol_Staco_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_ety_CentralReg", &MET_Muon_Isol_Staco_GCW_pt20_ety_CentralReg, &b_MET_Muon_Isol_Staco_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_sumet_CentralReg", &MET_Muon_Isol_Staco_GCW_pt20_sumet_CentralReg, &b_MET_Muon_Isol_Staco_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_phi_CentralReg", &MET_Muon_Isol_Staco_GCW_pt20_phi_CentralReg, &b_MET_Muon_Isol_Staco_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_etx_EndcapRegion", &MET_Muon_Isol_Staco_GCW_pt20_etx_EndcapRegion, &b_MET_Muon_Isol_Staco_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_ety_EndcapRegion", &MET_Muon_Isol_Staco_GCW_pt20_ety_EndcapRegion, &b_MET_Muon_Isol_Staco_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_sumet_EndcapRegion", &MET_Muon_Isol_Staco_GCW_pt20_sumet_EndcapRegion, &b_MET_Muon_Isol_Staco_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_phi_EndcapRegion", &MET_Muon_Isol_Staco_GCW_pt20_phi_EndcapRegion, &b_MET_Muon_Isol_Staco_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_etx_ForwardReg", &MET_Muon_Isol_Staco_GCW_pt20_etx_ForwardReg, &b_MET_Muon_Isol_Staco_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_ety_ForwardReg", &MET_Muon_Isol_Staco_GCW_pt20_ety_ForwardReg, &b_MET_Muon_Isol_Staco_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_sumet_ForwardReg", &MET_Muon_Isol_Staco_GCW_pt20_sumet_ForwardReg, &b_MET_Muon_Isol_Staco_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_GCW_pt20_phi_ForwardReg", &MET_Muon_Isol_Staco_GCW_pt20_phi_ForwardReg, &b_MET_Muon_Isol_Staco_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_etx", &MET_Muon_Isol_Staco_LCW_NI_eflow4_etx, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_ety", &MET_Muon_Isol_Staco_LCW_NI_eflow4_ety, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_phi", &MET_Muon_Isol_Staco_LCW_NI_eflow4_phi, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_et", &MET_Muon_Isol_Staco_LCW_NI_eflow4_et, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet", &MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_et", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_et, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_et", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_et, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_CentralReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_CentralReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_EndcapRegion", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_ForwardReg", &MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_Muon_Isol_Staco_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_etx", &MET_Muon_NonIsol_Staco_GCW_pt20_etx, &b_MET_Muon_NonIsol_Staco_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_ety", &MET_Muon_NonIsol_Staco_GCW_pt20_ety, &b_MET_Muon_NonIsol_Staco_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_phi", &MET_Muon_NonIsol_Staco_GCW_pt20_phi, &b_MET_Muon_NonIsol_Staco_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_et", &MET_Muon_NonIsol_Staco_GCW_pt20_et, &b_MET_Muon_NonIsol_Staco_GCW_pt20_et);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_sumet", &MET_Muon_NonIsol_Staco_GCW_pt20_sumet, &b_MET_Muon_NonIsol_Staco_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_etx_CentralReg", &MET_Muon_NonIsol_Staco_GCW_pt20_etx_CentralReg, &b_MET_Muon_NonIsol_Staco_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_ety_CentralReg", &MET_Muon_NonIsol_Staco_GCW_pt20_ety_CentralReg, &b_MET_Muon_NonIsol_Staco_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_sumet_CentralReg", &MET_Muon_NonIsol_Staco_GCW_pt20_sumet_CentralReg, &b_MET_Muon_NonIsol_Staco_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_phi_CentralReg", &MET_Muon_NonIsol_Staco_GCW_pt20_phi_CentralReg, &b_MET_Muon_NonIsol_Staco_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_etx_EndcapRegion", &MET_Muon_NonIsol_Staco_GCW_pt20_etx_EndcapRegion, &b_MET_Muon_NonIsol_Staco_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_ety_EndcapRegion", &MET_Muon_NonIsol_Staco_GCW_pt20_ety_EndcapRegion, &b_MET_Muon_NonIsol_Staco_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_sumet_EndcapRegion", &MET_Muon_NonIsol_Staco_GCW_pt20_sumet_EndcapRegion, &b_MET_Muon_NonIsol_Staco_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_phi_EndcapRegion", &MET_Muon_NonIsol_Staco_GCW_pt20_phi_EndcapRegion, &b_MET_Muon_NonIsol_Staco_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_etx_ForwardReg", &MET_Muon_NonIsol_Staco_GCW_pt20_etx_ForwardReg, &b_MET_Muon_NonIsol_Staco_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_ety_ForwardReg", &MET_Muon_NonIsol_Staco_GCW_pt20_ety_ForwardReg, &b_MET_Muon_NonIsol_Staco_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_sumet_ForwardReg", &MET_Muon_NonIsol_Staco_GCW_pt20_sumet_ForwardReg, &b_MET_Muon_NonIsol_Staco_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_GCW_pt20_phi_ForwardReg", &MET_Muon_NonIsol_Staco_GCW_pt20_phi_ForwardReg, &b_MET_Muon_NonIsol_Staco_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_et", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_et, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_et", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_et, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_et", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_et, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_CentralReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_CentralReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_EndcapRegion", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_ForwardReg", &MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_Muon_NonIsol_Staco_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_etx", &MET_Muon_Total_Staco_GCW_pt20_etx, &b_MET_Muon_Total_Staco_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_ety", &MET_Muon_Total_Staco_GCW_pt20_ety, &b_MET_Muon_Total_Staco_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_phi", &MET_Muon_Total_Staco_GCW_pt20_phi, &b_MET_Muon_Total_Staco_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_et", &MET_Muon_Total_Staco_GCW_pt20_et, &b_MET_Muon_Total_Staco_GCW_pt20_et);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_sumet", &MET_Muon_Total_Staco_GCW_pt20_sumet, &b_MET_Muon_Total_Staco_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_etx_CentralReg", &MET_Muon_Total_Staco_GCW_pt20_etx_CentralReg, &b_MET_Muon_Total_Staco_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_ety_CentralReg", &MET_Muon_Total_Staco_GCW_pt20_ety_CentralReg, &b_MET_Muon_Total_Staco_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_sumet_CentralReg", &MET_Muon_Total_Staco_GCW_pt20_sumet_CentralReg, &b_MET_Muon_Total_Staco_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_phi_CentralReg", &MET_Muon_Total_Staco_GCW_pt20_phi_CentralReg, &b_MET_Muon_Total_Staco_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_etx_EndcapRegion", &MET_Muon_Total_Staco_GCW_pt20_etx_EndcapRegion, &b_MET_Muon_Total_Staco_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_ety_EndcapRegion", &MET_Muon_Total_Staco_GCW_pt20_ety_EndcapRegion, &b_MET_Muon_Total_Staco_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_sumet_EndcapRegion", &MET_Muon_Total_Staco_GCW_pt20_sumet_EndcapRegion, &b_MET_Muon_Total_Staco_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_phi_EndcapRegion", &MET_Muon_Total_Staco_GCW_pt20_phi_EndcapRegion, &b_MET_Muon_Total_Staco_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_etx_ForwardReg", &MET_Muon_Total_Staco_GCW_pt20_etx_ForwardReg, &b_MET_Muon_Total_Staco_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_ety_ForwardReg", &MET_Muon_Total_Staco_GCW_pt20_ety_ForwardReg, &b_MET_Muon_Total_Staco_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_sumet_ForwardReg", &MET_Muon_Total_Staco_GCW_pt20_sumet_ForwardReg, &b_MET_Muon_Total_Staco_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_GCW_pt20_phi_ForwardReg", &MET_Muon_Total_Staco_GCW_pt20_phi_ForwardReg, &b_MET_Muon_Total_Staco_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_etx", &MET_Muon_Total_Staco_LCW_NI_eflow4_etx, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_ety", &MET_Muon_Total_Staco_LCW_NI_eflow4_ety, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_phi", &MET_Muon_Total_Staco_LCW_NI_eflow4_phi, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_et", &MET_Muon_Total_Staco_LCW_NI_eflow4_et, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_sumet", &MET_Muon_Total_Staco_LCW_NI_eflow4_sumet, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_etx_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow4_etx_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_ety_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow4_ety_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_phi_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow4_phi_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_etx_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_ety_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_phi_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_etx_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow4_etx_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_ety_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow4_ety_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow4_phi_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow4_phi_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_et", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_et, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_etx", &MET_Muon_Total_Staco_LCW_NI_eflow_400_etx, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_ety", &MET_Muon_Total_Staco_LCW_NI_eflow_400_ety, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_phi", &MET_Muon_Total_Staco_LCW_NI_eflow_400_phi, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_et", &MET_Muon_Total_Staco_LCW_NI_eflow_400_et, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet", &MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_CentralReg", &MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_CentralReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_EndcapRegion", &MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_ForwardReg", &MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_Muon_Total_Staco_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_etx", &MET_SoftJets_GCW_pt20_etx, &b_MET_SoftJets_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_ety", &MET_SoftJets_GCW_pt20_ety, &b_MET_SoftJets_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_phi", &MET_SoftJets_GCW_pt20_phi, &b_MET_SoftJets_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_et", &MET_SoftJets_GCW_pt20_et, &b_MET_SoftJets_GCW_pt20_et);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_sumet", &MET_SoftJets_GCW_pt20_sumet, &b_MET_SoftJets_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_etx_CentralReg", &MET_SoftJets_GCW_pt20_etx_CentralReg, &b_MET_SoftJets_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_ety_CentralReg", &MET_SoftJets_GCW_pt20_ety_CentralReg, &b_MET_SoftJets_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_sumet_CentralReg", &MET_SoftJets_GCW_pt20_sumet_CentralReg, &b_MET_SoftJets_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_phi_CentralReg", &MET_SoftJets_GCW_pt20_phi_CentralReg, &b_MET_SoftJets_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_etx_EndcapRegion", &MET_SoftJets_GCW_pt20_etx_EndcapRegion, &b_MET_SoftJets_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_ety_EndcapRegion", &MET_SoftJets_GCW_pt20_ety_EndcapRegion, &b_MET_SoftJets_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_sumet_EndcapRegion", &MET_SoftJets_GCW_pt20_sumet_EndcapRegion, &b_MET_SoftJets_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_phi_EndcapRegion", &MET_SoftJets_GCW_pt20_phi_EndcapRegion, &b_MET_SoftJets_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_etx_ForwardReg", &MET_SoftJets_GCW_pt20_etx_ForwardReg, &b_MET_SoftJets_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_ety_ForwardReg", &MET_SoftJets_GCW_pt20_ety_ForwardReg, &b_MET_SoftJets_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_sumet_ForwardReg", &MET_SoftJets_GCW_pt20_sumet_ForwardReg, &b_MET_SoftJets_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_GCW_pt20_phi_ForwardReg", &MET_SoftJets_GCW_pt20_phi_ForwardReg, &b_MET_SoftJets_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_etx", &MET_SoftJets_LCW_NI_eflow4_etx, &b_MET_SoftJets_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_ety", &MET_SoftJets_LCW_NI_eflow4_ety, &b_MET_SoftJets_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_phi", &MET_SoftJets_LCW_NI_eflow4_phi, &b_MET_SoftJets_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_et", &MET_SoftJets_LCW_NI_eflow4_et, &b_MET_SoftJets_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_sumet", &MET_SoftJets_LCW_NI_eflow4_sumet, &b_MET_SoftJets_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_etx_CentralReg", &MET_SoftJets_LCW_NI_eflow4_etx_CentralReg, &b_MET_SoftJets_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_ety_CentralReg", &MET_SoftJets_LCW_NI_eflow4_ety_CentralReg, &b_MET_SoftJets_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_sumet_CentralReg", &MET_SoftJets_LCW_NI_eflow4_sumet_CentralReg, &b_MET_SoftJets_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_phi_CentralReg", &MET_SoftJets_LCW_NI_eflow4_phi_CentralReg, &b_MET_SoftJets_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_etx_EndcapRegion", &MET_SoftJets_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_ety_EndcapRegion", &MET_SoftJets_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_sumet_EndcapRegion", &MET_SoftJets_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_phi_EndcapRegion", &MET_SoftJets_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_etx_ForwardReg", &MET_SoftJets_LCW_NI_eflow4_etx_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_ety_ForwardReg", &MET_SoftJets_LCW_NI_eflow4_ety_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_sumet_ForwardReg", &MET_SoftJets_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow4_phi_ForwardReg", &MET_SoftJets_LCW_NI_eflow4_phi_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_etx", &MET_SoftJets_LCW_NI_eflow_CB_etx, &b_MET_SoftJets_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_ety", &MET_SoftJets_LCW_NI_eflow_CB_ety, &b_MET_SoftJets_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_phi", &MET_SoftJets_LCW_NI_eflow_CB_phi, &b_MET_SoftJets_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_et", &MET_SoftJets_LCW_NI_eflow_CB_et, &b_MET_SoftJets_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_sumet", &MET_SoftJets_LCW_NI_eflow_CB_sumet, &b_MET_SoftJets_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_etx_CentralReg", &MET_SoftJets_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_SoftJets_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_ety_CentralReg", &MET_SoftJets_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_SoftJets_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_sumet_CentralReg", &MET_SoftJets_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_SoftJets_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_phi_CentralReg", &MET_SoftJets_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_SoftJets_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_SoftJets_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_SoftJets_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_SoftJets_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_SoftJets_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_etx_ForwardReg", &MET_SoftJets_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_ety_ForwardReg", &MET_SoftJets_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_SoftJets_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_CB_phi_ForwardReg", &MET_SoftJets_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_etx", &MET_SoftJets_LCW_NI_eflow_400_etx, &b_MET_SoftJets_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_ety", &MET_SoftJets_LCW_NI_eflow_400_ety, &b_MET_SoftJets_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_phi", &MET_SoftJets_LCW_NI_eflow_400_phi, &b_MET_SoftJets_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_et", &MET_SoftJets_LCW_NI_eflow_400_et, &b_MET_SoftJets_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_sumet", &MET_SoftJets_LCW_NI_eflow_400_sumet, &b_MET_SoftJets_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_etx_CentralReg", &MET_SoftJets_LCW_NI_eflow_400_etx_CentralReg, &b_MET_SoftJets_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_ety_CentralReg", &MET_SoftJets_LCW_NI_eflow_400_ety_CentralReg, &b_MET_SoftJets_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_sumet_CentralReg", &MET_SoftJets_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_SoftJets_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_phi_CentralReg", &MET_SoftJets_LCW_NI_eflow_400_phi_CentralReg, &b_MET_SoftJets_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_etx_EndcapRegion", &MET_SoftJets_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_ety_EndcapRegion", &MET_SoftJets_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_SoftJets_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_phi_EndcapRegion", &MET_SoftJets_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_SoftJets_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_etx_ForwardReg", &MET_SoftJets_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_ety_ForwardReg", &MET_SoftJets_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_sumet_ForwardReg", &MET_SoftJets_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_NI_eflow_400_phi_ForwardReg", &MET_SoftJets_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_SoftJets_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_etx", &MET_RefMuon_Track_GCW_pt20_etx, &b_MET_RefMuon_Track_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_ety", &MET_RefMuon_Track_GCW_pt20_ety, &b_MET_RefMuon_Track_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_phi", &MET_RefMuon_Track_GCW_pt20_phi, &b_MET_RefMuon_Track_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_et", &MET_RefMuon_Track_GCW_pt20_et, &b_MET_RefMuon_Track_GCW_pt20_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_sumet", &MET_RefMuon_Track_GCW_pt20_sumet, &b_MET_RefMuon_Track_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_etx_CentralReg", &MET_RefMuon_Track_GCW_pt20_etx_CentralReg, &b_MET_RefMuon_Track_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_ety_CentralReg", &MET_RefMuon_Track_GCW_pt20_ety_CentralReg, &b_MET_RefMuon_Track_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_sumet_CentralReg", &MET_RefMuon_Track_GCW_pt20_sumet_CentralReg, &b_MET_RefMuon_Track_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_phi_CentralReg", &MET_RefMuon_Track_GCW_pt20_phi_CentralReg, &b_MET_RefMuon_Track_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_etx_EndcapRegion", &MET_RefMuon_Track_GCW_pt20_etx_EndcapRegion, &b_MET_RefMuon_Track_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_ety_EndcapRegion", &MET_RefMuon_Track_GCW_pt20_ety_EndcapRegion, &b_MET_RefMuon_Track_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_sumet_EndcapRegion", &MET_RefMuon_Track_GCW_pt20_sumet_EndcapRegion, &b_MET_RefMuon_Track_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_phi_EndcapRegion", &MET_RefMuon_Track_GCW_pt20_phi_EndcapRegion, &b_MET_RefMuon_Track_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_etx_ForwardReg", &MET_RefMuon_Track_GCW_pt20_etx_ForwardReg, &b_MET_RefMuon_Track_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_ety_ForwardReg", &MET_RefMuon_Track_GCW_pt20_ety_ForwardReg, &b_MET_RefMuon_Track_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_sumet_ForwardReg", &MET_RefMuon_Track_GCW_pt20_sumet_ForwardReg, &b_MET_RefMuon_Track_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_GCW_pt20_phi_ForwardReg", &MET_RefMuon_Track_GCW_pt20_phi_ForwardReg, &b_MET_RefMuon_Track_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_etx", &MET_RefMuon_Track_LCW_NI_eflow4_etx, &b_MET_RefMuon_Track_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_ety", &MET_RefMuon_Track_LCW_NI_eflow4_ety, &b_MET_RefMuon_Track_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_phi", &MET_RefMuon_Track_LCW_NI_eflow4_phi, &b_MET_RefMuon_Track_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_et", &MET_RefMuon_Track_LCW_NI_eflow4_et, &b_MET_RefMuon_Track_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_sumet", &MET_RefMuon_Track_LCW_NI_eflow4_sumet, &b_MET_RefMuon_Track_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_etx_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow4_etx_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_ety_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow4_ety_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_sumet_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow4_sumet_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_phi_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow4_phi_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_etx_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_ety_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_sumet_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_phi_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_etx_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow4_etx_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_ety_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow4_ety_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_sumet_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow4_phi_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow4_phi_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_etx", &MET_RefMuon_Track_LCW_NI_eflow_CB_etx, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_ety", &MET_RefMuon_Track_LCW_NI_eflow_CB_ety, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_phi", &MET_RefMuon_Track_LCW_NI_eflow_CB_phi, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_et", &MET_RefMuon_Track_LCW_NI_eflow_CB_et, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_sumet", &MET_RefMuon_Track_LCW_NI_eflow_CB_sumet, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_etx_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_ety_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_phi_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_etx_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_ety_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_CB_phi_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_etx", &MET_RefMuon_Track_LCW_NI_eflow_400_etx, &b_MET_RefMuon_Track_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_ety", &MET_RefMuon_Track_LCW_NI_eflow_400_ety, &b_MET_RefMuon_Track_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_phi", &MET_RefMuon_Track_LCW_NI_eflow_400_phi, &b_MET_RefMuon_Track_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_et", &MET_RefMuon_Track_LCW_NI_eflow_400_et, &b_MET_RefMuon_Track_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_sumet", &MET_RefMuon_Track_LCW_NI_eflow_400_sumet, &b_MET_RefMuon_Track_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_etx_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow_400_etx_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_ety_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow_400_ety_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_sumet_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_phi_CentralReg", &MET_RefMuon_Track_LCW_NI_eflow_400_phi_CentralReg, &b_MET_RefMuon_Track_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_etx_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_ety_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_phi_EndcapRegion", &MET_RefMuon_Track_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_RefMuon_Track_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_etx_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_ety_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_sumet_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_NI_eflow_400_phi_ForwardReg", &MET_RefMuon_Track_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_RefMuon_Track_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_etx", &MET_RefMuon_Track_Staco_GCW_pt20_etx, &b_MET_RefMuon_Track_Staco_GCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_ety", &MET_RefMuon_Track_Staco_GCW_pt20_ety, &b_MET_RefMuon_Track_Staco_GCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_phi", &MET_RefMuon_Track_Staco_GCW_pt20_phi, &b_MET_RefMuon_Track_Staco_GCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_et", &MET_RefMuon_Track_Staco_GCW_pt20_et, &b_MET_RefMuon_Track_Staco_GCW_pt20_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_sumet", &MET_RefMuon_Track_Staco_GCW_pt20_sumet, &b_MET_RefMuon_Track_Staco_GCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_etx_CentralReg", &MET_RefMuon_Track_Staco_GCW_pt20_etx_CentralReg, &b_MET_RefMuon_Track_Staco_GCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_ety_CentralReg", &MET_RefMuon_Track_Staco_GCW_pt20_ety_CentralReg, &b_MET_RefMuon_Track_Staco_GCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_sumet_CentralReg", &MET_RefMuon_Track_Staco_GCW_pt20_sumet_CentralReg, &b_MET_RefMuon_Track_Staco_GCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_phi_CentralReg", &MET_RefMuon_Track_Staco_GCW_pt20_phi_CentralReg, &b_MET_RefMuon_Track_Staco_GCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_etx_EndcapRegion", &MET_RefMuon_Track_Staco_GCW_pt20_etx_EndcapRegion, &b_MET_RefMuon_Track_Staco_GCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_ety_EndcapRegion", &MET_RefMuon_Track_Staco_GCW_pt20_ety_EndcapRegion, &b_MET_RefMuon_Track_Staco_GCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_sumet_EndcapRegion", &MET_RefMuon_Track_Staco_GCW_pt20_sumet_EndcapRegion, &b_MET_RefMuon_Track_Staco_GCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_phi_EndcapRegion", &MET_RefMuon_Track_Staco_GCW_pt20_phi_EndcapRegion, &b_MET_RefMuon_Track_Staco_GCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_etx_ForwardReg", &MET_RefMuon_Track_Staco_GCW_pt20_etx_ForwardReg, &b_MET_RefMuon_Track_Staco_GCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_ety_ForwardReg", &MET_RefMuon_Track_Staco_GCW_pt20_ety_ForwardReg, &b_MET_RefMuon_Track_Staco_GCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_sumet_ForwardReg", &MET_RefMuon_Track_Staco_GCW_pt20_sumet_ForwardReg, &b_MET_RefMuon_Track_Staco_GCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_GCW_pt20_phi_ForwardReg", &MET_RefMuon_Track_Staco_GCW_pt20_phi_ForwardReg, &b_MET_RefMuon_Track_Staco_GCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_et", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_et, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow4_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_et", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_et, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_CB_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_et", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_et, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_CentralReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_CentralReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_EndcapRegion", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_EndcapRegion, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_ForwardReg", &MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_ForwardReg, &b_MET_RefMuon_Track_Staco_LCW_NI_eflow_400_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_etx", &MET_RefFinal_LCW_pt20_etx, &b_MET_RefFinal_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_ety", &MET_RefFinal_LCW_pt20_ety, &b_MET_RefFinal_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_phi", &MET_RefFinal_LCW_pt20_phi, &b_MET_RefFinal_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_et", &MET_RefFinal_LCW_pt20_et, &b_MET_RefFinal_LCW_pt20_et);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_sumet", &MET_RefFinal_LCW_pt20_sumet, &b_MET_RefFinal_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_etx_CentralReg", &MET_RefFinal_LCW_pt20_etx_CentralReg, &b_MET_RefFinal_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_ety_CentralReg", &MET_RefFinal_LCW_pt20_ety_CentralReg, &b_MET_RefFinal_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_sumet_CentralReg", &MET_RefFinal_LCW_pt20_sumet_CentralReg, &b_MET_RefFinal_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_phi_CentralReg", &MET_RefFinal_LCW_pt20_phi_CentralReg, &b_MET_RefFinal_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_etx_EndcapRegion", &MET_RefFinal_LCW_pt20_etx_EndcapRegion, &b_MET_RefFinal_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_ety_EndcapRegion", &MET_RefFinal_LCW_pt20_ety_EndcapRegion, &b_MET_RefFinal_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_sumet_EndcapRegion", &MET_RefFinal_LCW_pt20_sumet_EndcapRegion, &b_MET_RefFinal_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_phi_EndcapRegion", &MET_RefFinal_LCW_pt20_phi_EndcapRegion, &b_MET_RefFinal_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_etx_ForwardReg", &MET_RefFinal_LCW_pt20_etx_ForwardReg, &b_MET_RefFinal_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_ety_ForwardReg", &MET_RefFinal_LCW_pt20_ety_ForwardReg, &b_MET_RefFinal_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_sumet_ForwardReg", &MET_RefFinal_LCW_pt20_sumet_ForwardReg, &b_MET_RefFinal_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefFinal_LCW_pt20_phi_ForwardReg", &MET_RefFinal_LCW_pt20_phi_ForwardReg, &b_MET_RefFinal_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_etx", &MET_RefGamma_LCW_pt20_etx, &b_MET_RefGamma_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_ety", &MET_RefGamma_LCW_pt20_ety, &b_MET_RefGamma_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_phi", &MET_RefGamma_LCW_pt20_phi, &b_MET_RefGamma_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_et", &MET_RefGamma_LCW_pt20_et, &b_MET_RefGamma_LCW_pt20_et);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_sumet", &MET_RefGamma_LCW_pt20_sumet, &b_MET_RefGamma_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_etx_CentralReg", &MET_RefGamma_LCW_pt20_etx_CentralReg, &b_MET_RefGamma_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_ety_CentralReg", &MET_RefGamma_LCW_pt20_ety_CentralReg, &b_MET_RefGamma_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_sumet_CentralReg", &MET_RefGamma_LCW_pt20_sumet_CentralReg, &b_MET_RefGamma_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_phi_CentralReg", &MET_RefGamma_LCW_pt20_phi_CentralReg, &b_MET_RefGamma_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_etx_EndcapRegion", &MET_RefGamma_LCW_pt20_etx_EndcapRegion, &b_MET_RefGamma_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_ety_EndcapRegion", &MET_RefGamma_LCW_pt20_ety_EndcapRegion, &b_MET_RefGamma_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_sumet_EndcapRegion", &MET_RefGamma_LCW_pt20_sumet_EndcapRegion, &b_MET_RefGamma_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_phi_EndcapRegion", &MET_RefGamma_LCW_pt20_phi_EndcapRegion, &b_MET_RefGamma_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_etx_ForwardReg", &MET_RefGamma_LCW_pt20_etx_ForwardReg, &b_MET_RefGamma_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_ety_ForwardReg", &MET_RefGamma_LCW_pt20_ety_ForwardReg, &b_MET_RefGamma_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_sumet_ForwardReg", &MET_RefGamma_LCW_pt20_sumet_ForwardReg, &b_MET_RefGamma_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefGamma_LCW_pt20_phi_ForwardReg", &MET_RefGamma_LCW_pt20_phi_ForwardReg, &b_MET_RefGamma_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_etx", &MET_RefEle_LCW_pt20_etx, &b_MET_RefEle_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_ety", &MET_RefEle_LCW_pt20_ety, &b_MET_RefEle_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_phi", &MET_RefEle_LCW_pt20_phi, &b_MET_RefEle_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_et", &MET_RefEle_LCW_pt20_et, &b_MET_RefEle_LCW_pt20_et);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_sumet", &MET_RefEle_LCW_pt20_sumet, &b_MET_RefEle_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_etx_CentralReg", &MET_RefEle_LCW_pt20_etx_CentralReg, &b_MET_RefEle_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_ety_CentralReg", &MET_RefEle_LCW_pt20_ety_CentralReg, &b_MET_RefEle_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_sumet_CentralReg", &MET_RefEle_LCW_pt20_sumet_CentralReg, &b_MET_RefEle_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_phi_CentralReg", &MET_RefEle_LCW_pt20_phi_CentralReg, &b_MET_RefEle_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_etx_EndcapRegion", &MET_RefEle_LCW_pt20_etx_EndcapRegion, &b_MET_RefEle_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_ety_EndcapRegion", &MET_RefEle_LCW_pt20_ety_EndcapRegion, &b_MET_RefEle_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_sumet_EndcapRegion", &MET_RefEle_LCW_pt20_sumet_EndcapRegion, &b_MET_RefEle_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_phi_EndcapRegion", &MET_RefEle_LCW_pt20_phi_EndcapRegion, &b_MET_RefEle_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_etx_ForwardReg", &MET_RefEle_LCW_pt20_etx_ForwardReg, &b_MET_RefEle_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_ety_ForwardReg", &MET_RefEle_LCW_pt20_ety_ForwardReg, &b_MET_RefEle_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_sumet_ForwardReg", &MET_RefEle_LCW_pt20_sumet_ForwardReg, &b_MET_RefEle_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefEle_LCW_pt20_phi_ForwardReg", &MET_RefEle_LCW_pt20_phi_ForwardReg, &b_MET_RefEle_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_etx", &MET_RefTau_LCW_pt20_etx, &b_MET_RefTau_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_ety", &MET_RefTau_LCW_pt20_ety, &b_MET_RefTau_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_phi", &MET_RefTau_LCW_pt20_phi, &b_MET_RefTau_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_et", &MET_RefTau_LCW_pt20_et, &b_MET_RefTau_LCW_pt20_et);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_sumet", &MET_RefTau_LCW_pt20_sumet, &b_MET_RefTau_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_etx_CentralReg", &MET_RefTau_LCW_pt20_etx_CentralReg, &b_MET_RefTau_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_ety_CentralReg", &MET_RefTau_LCW_pt20_ety_CentralReg, &b_MET_RefTau_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_sumet_CentralReg", &MET_RefTau_LCW_pt20_sumet_CentralReg, &b_MET_RefTau_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_phi_CentralReg", &MET_RefTau_LCW_pt20_phi_CentralReg, &b_MET_RefTau_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_etx_EndcapRegion", &MET_RefTau_LCW_pt20_etx_EndcapRegion, &b_MET_RefTau_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_ety_EndcapRegion", &MET_RefTau_LCW_pt20_ety_EndcapRegion, &b_MET_RefTau_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_sumet_EndcapRegion", &MET_RefTau_LCW_pt20_sumet_EndcapRegion, &b_MET_RefTau_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_phi_EndcapRegion", &MET_RefTau_LCW_pt20_phi_EndcapRegion, &b_MET_RefTau_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_etx_ForwardReg", &MET_RefTau_LCW_pt20_etx_ForwardReg, &b_MET_RefTau_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_ety_ForwardReg", &MET_RefTau_LCW_pt20_ety_ForwardReg, &b_MET_RefTau_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_sumet_ForwardReg", &MET_RefTau_LCW_pt20_sumet_ForwardReg, &b_MET_RefTau_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefTau_LCW_pt20_phi_ForwardReg", &MET_RefTau_LCW_pt20_phi_ForwardReg, &b_MET_RefTau_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_etx", &MET_RefJet_LCW_pt20_etx, &b_MET_RefJet_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_ety", &MET_RefJet_LCW_pt20_ety, &b_MET_RefJet_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_phi", &MET_RefJet_LCW_pt20_phi, &b_MET_RefJet_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_et", &MET_RefJet_LCW_pt20_et, &b_MET_RefJet_LCW_pt20_et);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_sumet", &MET_RefJet_LCW_pt20_sumet, &b_MET_RefJet_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_etx_CentralReg", &MET_RefJet_LCW_pt20_etx_CentralReg, &b_MET_RefJet_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_ety_CentralReg", &MET_RefJet_LCW_pt20_ety_CentralReg, &b_MET_RefJet_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_sumet_CentralReg", &MET_RefJet_LCW_pt20_sumet_CentralReg, &b_MET_RefJet_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_phi_CentralReg", &MET_RefJet_LCW_pt20_phi_CentralReg, &b_MET_RefJet_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_etx_EndcapRegion", &MET_RefJet_LCW_pt20_etx_EndcapRegion, &b_MET_RefJet_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_ety_EndcapRegion", &MET_RefJet_LCW_pt20_ety_EndcapRegion, &b_MET_RefJet_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_sumet_EndcapRegion", &MET_RefJet_LCW_pt20_sumet_EndcapRegion, &b_MET_RefJet_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_phi_EndcapRegion", &MET_RefJet_LCW_pt20_phi_EndcapRegion, &b_MET_RefJet_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_etx_ForwardReg", &MET_RefJet_LCW_pt20_etx_ForwardReg, &b_MET_RefJet_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_ety_ForwardReg", &MET_RefJet_LCW_pt20_ety_ForwardReg, &b_MET_RefJet_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_sumet_ForwardReg", &MET_RefJet_LCW_pt20_sumet_ForwardReg, &b_MET_RefJet_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefJet_LCW_pt20_phi_ForwardReg", &MET_RefJet_LCW_pt20_phi_ForwardReg, &b_MET_RefJet_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_etx", &MET_RefMuon_LCW_pt20_etx, &b_MET_RefMuon_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_ety", &MET_RefMuon_LCW_pt20_ety, &b_MET_RefMuon_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_phi", &MET_RefMuon_LCW_pt20_phi, &b_MET_RefMuon_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_et", &MET_RefMuon_LCW_pt20_et, &b_MET_RefMuon_LCW_pt20_et);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_sumet", &MET_RefMuon_LCW_pt20_sumet, &b_MET_RefMuon_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_etx_CentralReg", &MET_RefMuon_LCW_pt20_etx_CentralReg, &b_MET_RefMuon_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_ety_CentralReg", &MET_RefMuon_LCW_pt20_ety_CentralReg, &b_MET_RefMuon_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_sumet_CentralReg", &MET_RefMuon_LCW_pt20_sumet_CentralReg, &b_MET_RefMuon_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_phi_CentralReg", &MET_RefMuon_LCW_pt20_phi_CentralReg, &b_MET_RefMuon_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_etx_EndcapRegion", &MET_RefMuon_LCW_pt20_etx_EndcapRegion, &b_MET_RefMuon_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_ety_EndcapRegion", &MET_RefMuon_LCW_pt20_ety_EndcapRegion, &b_MET_RefMuon_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_sumet_EndcapRegion", &MET_RefMuon_LCW_pt20_sumet_EndcapRegion, &b_MET_RefMuon_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_phi_EndcapRegion", &MET_RefMuon_LCW_pt20_phi_EndcapRegion, &b_MET_RefMuon_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_etx_ForwardReg", &MET_RefMuon_LCW_pt20_etx_ForwardReg, &b_MET_RefMuon_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_ety_ForwardReg", &MET_RefMuon_LCW_pt20_ety_ForwardReg, &b_MET_RefMuon_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_sumet_ForwardReg", &MET_RefMuon_LCW_pt20_sumet_ForwardReg, &b_MET_RefMuon_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_LCW_pt20_phi_ForwardReg", &MET_RefMuon_LCW_pt20_phi_ForwardReg, &b_MET_RefMuon_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_etx", &MET_RefMuon_Muid_LCW_pt20_etx, &b_MET_RefMuon_Muid_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_ety", &MET_RefMuon_Muid_LCW_pt20_ety, &b_MET_RefMuon_Muid_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_phi", &MET_RefMuon_Muid_LCW_pt20_phi, &b_MET_RefMuon_Muid_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_et", &MET_RefMuon_Muid_LCW_pt20_et, &b_MET_RefMuon_Muid_LCW_pt20_et);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_sumet", &MET_RefMuon_Muid_LCW_pt20_sumet, &b_MET_RefMuon_Muid_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_etx_CentralReg", &MET_RefMuon_Muid_LCW_pt20_etx_CentralReg, &b_MET_RefMuon_Muid_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_ety_CentralReg", &MET_RefMuon_Muid_LCW_pt20_ety_CentralReg, &b_MET_RefMuon_Muid_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_sumet_CentralReg", &MET_RefMuon_Muid_LCW_pt20_sumet_CentralReg, &b_MET_RefMuon_Muid_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_phi_CentralReg", &MET_RefMuon_Muid_LCW_pt20_phi_CentralReg, &b_MET_RefMuon_Muid_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_etx_EndcapRegion", &MET_RefMuon_Muid_LCW_pt20_etx_EndcapRegion, &b_MET_RefMuon_Muid_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_ety_EndcapRegion", &MET_RefMuon_Muid_LCW_pt20_ety_EndcapRegion, &b_MET_RefMuon_Muid_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_sumet_EndcapRegion", &MET_RefMuon_Muid_LCW_pt20_sumet_EndcapRegion, &b_MET_RefMuon_Muid_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_phi_EndcapRegion", &MET_RefMuon_Muid_LCW_pt20_phi_EndcapRegion, &b_MET_RefMuon_Muid_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_etx_ForwardReg", &MET_RefMuon_Muid_LCW_pt20_etx_ForwardReg, &b_MET_RefMuon_Muid_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_ety_ForwardReg", &MET_RefMuon_Muid_LCW_pt20_ety_ForwardReg, &b_MET_RefMuon_Muid_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_sumet_ForwardReg", &MET_RefMuon_Muid_LCW_pt20_sumet_ForwardReg, &b_MET_RefMuon_Muid_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Muid_LCW_pt20_phi_ForwardReg", &MET_RefMuon_Muid_LCW_pt20_phi_ForwardReg, &b_MET_RefMuon_Muid_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_etx", &MET_CellOut_LCW_pt20_etx, &b_MET_CellOut_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_ety", &MET_CellOut_LCW_pt20_ety, &b_MET_CellOut_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_phi", &MET_CellOut_LCW_pt20_phi, &b_MET_CellOut_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_et", &MET_CellOut_LCW_pt20_et, &b_MET_CellOut_LCW_pt20_et);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_sumet", &MET_CellOut_LCW_pt20_sumet, &b_MET_CellOut_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_etx_CentralReg", &MET_CellOut_LCW_pt20_etx_CentralReg, &b_MET_CellOut_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_ety_CentralReg", &MET_CellOut_LCW_pt20_ety_CentralReg, &b_MET_CellOut_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_sumet_CentralReg", &MET_CellOut_LCW_pt20_sumet_CentralReg, &b_MET_CellOut_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_phi_CentralReg", &MET_CellOut_LCW_pt20_phi_CentralReg, &b_MET_CellOut_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_etx_EndcapRegion", &MET_CellOut_LCW_pt20_etx_EndcapRegion, &b_MET_CellOut_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_ety_EndcapRegion", &MET_CellOut_LCW_pt20_ety_EndcapRegion, &b_MET_CellOut_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_sumet_EndcapRegion", &MET_CellOut_LCW_pt20_sumet_EndcapRegion, &b_MET_CellOut_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_phi_EndcapRegion", &MET_CellOut_LCW_pt20_phi_EndcapRegion, &b_MET_CellOut_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_etx_ForwardReg", &MET_CellOut_LCW_pt20_etx_ForwardReg, &b_MET_CellOut_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_ety_ForwardReg", &MET_CellOut_LCW_pt20_ety_ForwardReg, &b_MET_CellOut_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_sumet_ForwardReg", &MET_CellOut_LCW_pt20_sumet_ForwardReg, &b_MET_CellOut_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_CellOut_LCW_pt20_phi_ForwardReg", &MET_CellOut_LCW_pt20_phi_ForwardReg, &b_MET_CellOut_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_etx", &MET_Cryo_LCW_pt20_etx, &b_MET_Cryo_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_ety", &MET_Cryo_LCW_pt20_ety, &b_MET_Cryo_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_phi", &MET_Cryo_LCW_pt20_phi, &b_MET_Cryo_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_et", &MET_Cryo_LCW_pt20_et, &b_MET_Cryo_LCW_pt20_et);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_sumet", &MET_Cryo_LCW_pt20_sumet, &b_MET_Cryo_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_etx_CentralReg", &MET_Cryo_LCW_pt20_etx_CentralReg, &b_MET_Cryo_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_ety_CentralReg", &MET_Cryo_LCW_pt20_ety_CentralReg, &b_MET_Cryo_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_sumet_CentralReg", &MET_Cryo_LCW_pt20_sumet_CentralReg, &b_MET_Cryo_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_phi_CentralReg", &MET_Cryo_LCW_pt20_phi_CentralReg, &b_MET_Cryo_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_etx_EndcapRegion", &MET_Cryo_LCW_pt20_etx_EndcapRegion, &b_MET_Cryo_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_ety_EndcapRegion", &MET_Cryo_LCW_pt20_ety_EndcapRegion, &b_MET_Cryo_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_sumet_EndcapRegion", &MET_Cryo_LCW_pt20_sumet_EndcapRegion, &b_MET_Cryo_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_phi_EndcapRegion", &MET_Cryo_LCW_pt20_phi_EndcapRegion, &b_MET_Cryo_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_etx_ForwardReg", &MET_Cryo_LCW_pt20_etx_ForwardReg, &b_MET_Cryo_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_ety_ForwardReg", &MET_Cryo_LCW_pt20_ety_ForwardReg, &b_MET_Cryo_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_sumet_ForwardReg", &MET_Cryo_LCW_pt20_sumet_ForwardReg, &b_MET_Cryo_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Cryo_LCW_pt20_phi_ForwardReg", &MET_Cryo_LCW_pt20_phi_ForwardReg, &b_MET_Cryo_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_etx", &MET_Muon_Isol_Muid_LCW_pt20_etx, &b_MET_Muon_Isol_Muid_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_ety", &MET_Muon_Isol_Muid_LCW_pt20_ety, &b_MET_Muon_Isol_Muid_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_phi", &MET_Muon_Isol_Muid_LCW_pt20_phi, &b_MET_Muon_Isol_Muid_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_et", &MET_Muon_Isol_Muid_LCW_pt20_et, &b_MET_Muon_Isol_Muid_LCW_pt20_et);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_sumet", &MET_Muon_Isol_Muid_LCW_pt20_sumet, &b_MET_Muon_Isol_Muid_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_etx_CentralReg", &MET_Muon_Isol_Muid_LCW_pt20_etx_CentralReg, &b_MET_Muon_Isol_Muid_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_ety_CentralReg", &MET_Muon_Isol_Muid_LCW_pt20_ety_CentralReg, &b_MET_Muon_Isol_Muid_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_sumet_CentralReg", &MET_Muon_Isol_Muid_LCW_pt20_sumet_CentralReg, &b_MET_Muon_Isol_Muid_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_phi_CentralReg", &MET_Muon_Isol_Muid_LCW_pt20_phi_CentralReg, &b_MET_Muon_Isol_Muid_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_etx_EndcapRegion", &MET_Muon_Isol_Muid_LCW_pt20_etx_EndcapRegion, &b_MET_Muon_Isol_Muid_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_ety_EndcapRegion", &MET_Muon_Isol_Muid_LCW_pt20_ety_EndcapRegion, &b_MET_Muon_Isol_Muid_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_sumet_EndcapRegion", &MET_Muon_Isol_Muid_LCW_pt20_sumet_EndcapRegion, &b_MET_Muon_Isol_Muid_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_phi_EndcapRegion", &MET_Muon_Isol_Muid_LCW_pt20_phi_EndcapRegion, &b_MET_Muon_Isol_Muid_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_etx_ForwardReg", &MET_Muon_Isol_Muid_LCW_pt20_etx_ForwardReg, &b_MET_Muon_Isol_Muid_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_ety_ForwardReg", &MET_Muon_Isol_Muid_LCW_pt20_ety_ForwardReg, &b_MET_Muon_Isol_Muid_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_sumet_ForwardReg", &MET_Muon_Isol_Muid_LCW_pt20_sumet_ForwardReg, &b_MET_Muon_Isol_Muid_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Isol_Muid_LCW_pt20_phi_ForwardReg", &MET_Muon_Isol_Muid_LCW_pt20_phi_ForwardReg, &b_MET_Muon_Isol_Muid_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_etx", &MET_Muon_NonIsol_Muid_LCW_pt20_etx, &b_MET_Muon_NonIsol_Muid_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_ety", &MET_Muon_NonIsol_Muid_LCW_pt20_ety, &b_MET_Muon_NonIsol_Muid_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_phi", &MET_Muon_NonIsol_Muid_LCW_pt20_phi, &b_MET_Muon_NonIsol_Muid_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_et", &MET_Muon_NonIsol_Muid_LCW_pt20_et, &b_MET_Muon_NonIsol_Muid_LCW_pt20_et);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_sumet", &MET_Muon_NonIsol_Muid_LCW_pt20_sumet, &b_MET_Muon_NonIsol_Muid_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_etx_CentralReg", &MET_Muon_NonIsol_Muid_LCW_pt20_etx_CentralReg, &b_MET_Muon_NonIsol_Muid_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_ety_CentralReg", &MET_Muon_NonIsol_Muid_LCW_pt20_ety_CentralReg, &b_MET_Muon_NonIsol_Muid_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_sumet_CentralReg", &MET_Muon_NonIsol_Muid_LCW_pt20_sumet_CentralReg, &b_MET_Muon_NonIsol_Muid_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_phi_CentralReg", &MET_Muon_NonIsol_Muid_LCW_pt20_phi_CentralReg, &b_MET_Muon_NonIsol_Muid_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_etx_EndcapRegion", &MET_Muon_NonIsol_Muid_LCW_pt20_etx_EndcapRegion, &b_MET_Muon_NonIsol_Muid_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_ety_EndcapRegion", &MET_Muon_NonIsol_Muid_LCW_pt20_ety_EndcapRegion, &b_MET_Muon_NonIsol_Muid_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_sumet_EndcapRegion", &MET_Muon_NonIsol_Muid_LCW_pt20_sumet_EndcapRegion, &b_MET_Muon_NonIsol_Muid_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_phi_EndcapRegion", &MET_Muon_NonIsol_Muid_LCW_pt20_phi_EndcapRegion, &b_MET_Muon_NonIsol_Muid_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_etx_ForwardReg", &MET_Muon_NonIsol_Muid_LCW_pt20_etx_ForwardReg, &b_MET_Muon_NonIsol_Muid_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_ety_ForwardReg", &MET_Muon_NonIsol_Muid_LCW_pt20_ety_ForwardReg, &b_MET_Muon_NonIsol_Muid_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_sumet_ForwardReg", &MET_Muon_NonIsol_Muid_LCW_pt20_sumet_ForwardReg, &b_MET_Muon_NonIsol_Muid_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_NonIsol_Muid_LCW_pt20_phi_ForwardReg", &MET_Muon_NonIsol_Muid_LCW_pt20_phi_ForwardReg, &b_MET_Muon_NonIsol_Muid_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_etx", &MET_Muon_Total_Muid_LCW_pt20_etx, &b_MET_Muon_Total_Muid_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_ety", &MET_Muon_Total_Muid_LCW_pt20_ety, &b_MET_Muon_Total_Muid_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_phi", &MET_Muon_Total_Muid_LCW_pt20_phi, &b_MET_Muon_Total_Muid_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_et", &MET_Muon_Total_Muid_LCW_pt20_et, &b_MET_Muon_Total_Muid_LCW_pt20_et);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_sumet", &MET_Muon_Total_Muid_LCW_pt20_sumet, &b_MET_Muon_Total_Muid_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_etx_CentralReg", &MET_Muon_Total_Muid_LCW_pt20_etx_CentralReg, &b_MET_Muon_Total_Muid_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_ety_CentralReg", &MET_Muon_Total_Muid_LCW_pt20_ety_CentralReg, &b_MET_Muon_Total_Muid_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_sumet_CentralReg", &MET_Muon_Total_Muid_LCW_pt20_sumet_CentralReg, &b_MET_Muon_Total_Muid_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_phi_CentralReg", &MET_Muon_Total_Muid_LCW_pt20_phi_CentralReg, &b_MET_Muon_Total_Muid_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_etx_EndcapRegion", &MET_Muon_Total_Muid_LCW_pt20_etx_EndcapRegion, &b_MET_Muon_Total_Muid_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_ety_EndcapRegion", &MET_Muon_Total_Muid_LCW_pt20_ety_EndcapRegion, &b_MET_Muon_Total_Muid_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_sumet_EndcapRegion", &MET_Muon_Total_Muid_LCW_pt20_sumet_EndcapRegion, &b_MET_Muon_Total_Muid_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_phi_EndcapRegion", &MET_Muon_Total_Muid_LCW_pt20_phi_EndcapRegion, &b_MET_Muon_Total_Muid_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_etx_ForwardReg", &MET_Muon_Total_Muid_LCW_pt20_etx_ForwardReg, &b_MET_Muon_Total_Muid_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_ety_ForwardReg", &MET_Muon_Total_Muid_LCW_pt20_ety_ForwardReg, &b_MET_Muon_Total_Muid_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_sumet_ForwardReg", &MET_Muon_Total_Muid_LCW_pt20_sumet_ForwardReg, &b_MET_Muon_Total_Muid_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_Muon_Total_Muid_LCW_pt20_phi_ForwardReg", &MET_Muon_Total_Muid_LCW_pt20_phi_ForwardReg, &b_MET_Muon_Total_Muid_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_etx", &MET_SoftJets_LCW_pt20_etx, &b_MET_SoftJets_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_ety", &MET_SoftJets_LCW_pt20_ety, &b_MET_SoftJets_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_phi", &MET_SoftJets_LCW_pt20_phi, &b_MET_SoftJets_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_et", &MET_SoftJets_LCW_pt20_et, &b_MET_SoftJets_LCW_pt20_et);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_sumet", &MET_SoftJets_LCW_pt20_sumet, &b_MET_SoftJets_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_etx_CentralReg", &MET_SoftJets_LCW_pt20_etx_CentralReg, &b_MET_SoftJets_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_ety_CentralReg", &MET_SoftJets_LCW_pt20_ety_CentralReg, &b_MET_SoftJets_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_sumet_CentralReg", &MET_SoftJets_LCW_pt20_sumet_CentralReg, &b_MET_SoftJets_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_phi_CentralReg", &MET_SoftJets_LCW_pt20_phi_CentralReg, &b_MET_SoftJets_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_etx_EndcapRegion", &MET_SoftJets_LCW_pt20_etx_EndcapRegion, &b_MET_SoftJets_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_ety_EndcapRegion", &MET_SoftJets_LCW_pt20_ety_EndcapRegion, &b_MET_SoftJets_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_sumet_EndcapRegion", &MET_SoftJets_LCW_pt20_sumet_EndcapRegion, &b_MET_SoftJets_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_phi_EndcapRegion", &MET_SoftJets_LCW_pt20_phi_EndcapRegion, &b_MET_SoftJets_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_etx_ForwardReg", &MET_SoftJets_LCW_pt20_etx_ForwardReg, &b_MET_SoftJets_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_ety_ForwardReg", &MET_SoftJets_LCW_pt20_ety_ForwardReg, &b_MET_SoftJets_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_sumet_ForwardReg", &MET_SoftJets_LCW_pt20_sumet_ForwardReg, &b_MET_SoftJets_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_SoftJets_LCW_pt20_phi_ForwardReg", &MET_SoftJets_LCW_pt20_phi_ForwardReg, &b_MET_SoftJets_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_etx", &MET_RefMuon_Track_LCW_pt20_etx, &b_MET_RefMuon_Track_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_ety", &MET_RefMuon_Track_LCW_pt20_ety, &b_MET_RefMuon_Track_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_phi", &MET_RefMuon_Track_LCW_pt20_phi, &b_MET_RefMuon_Track_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_et", &MET_RefMuon_Track_LCW_pt20_et, &b_MET_RefMuon_Track_LCW_pt20_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_sumet", &MET_RefMuon_Track_LCW_pt20_sumet, &b_MET_RefMuon_Track_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_etx_CentralReg", &MET_RefMuon_Track_LCW_pt20_etx_CentralReg, &b_MET_RefMuon_Track_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_ety_CentralReg", &MET_RefMuon_Track_LCW_pt20_ety_CentralReg, &b_MET_RefMuon_Track_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_sumet_CentralReg", &MET_RefMuon_Track_LCW_pt20_sumet_CentralReg, &b_MET_RefMuon_Track_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_phi_CentralReg", &MET_RefMuon_Track_LCW_pt20_phi_CentralReg, &b_MET_RefMuon_Track_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_etx_EndcapRegion", &MET_RefMuon_Track_LCW_pt20_etx_EndcapRegion, &b_MET_RefMuon_Track_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_ety_EndcapRegion", &MET_RefMuon_Track_LCW_pt20_ety_EndcapRegion, &b_MET_RefMuon_Track_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_sumet_EndcapRegion", &MET_RefMuon_Track_LCW_pt20_sumet_EndcapRegion, &b_MET_RefMuon_Track_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_phi_EndcapRegion", &MET_RefMuon_Track_LCW_pt20_phi_EndcapRegion, &b_MET_RefMuon_Track_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_etx_ForwardReg", &MET_RefMuon_Track_LCW_pt20_etx_ForwardReg, &b_MET_RefMuon_Track_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_ety_ForwardReg", &MET_RefMuon_Track_LCW_pt20_ety_ForwardReg, &b_MET_RefMuon_Track_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_sumet_ForwardReg", &MET_RefMuon_Track_LCW_pt20_sumet_ForwardReg, &b_MET_RefMuon_Track_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_LCW_pt20_phi_ForwardReg", &MET_RefMuon_Track_LCW_pt20_phi_ForwardReg, &b_MET_RefMuon_Track_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_etx", &MET_RefMuon_Track_Muid_LCW_pt20_etx, &b_MET_RefMuon_Track_Muid_LCW_pt20_etx);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_ety", &MET_RefMuon_Track_Muid_LCW_pt20_ety, &b_MET_RefMuon_Track_Muid_LCW_pt20_ety);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_phi", &MET_RefMuon_Track_Muid_LCW_pt20_phi, &b_MET_RefMuon_Track_Muid_LCW_pt20_phi);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_et", &MET_RefMuon_Track_Muid_LCW_pt20_et, &b_MET_RefMuon_Track_Muid_LCW_pt20_et);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_sumet", &MET_RefMuon_Track_Muid_LCW_pt20_sumet, &b_MET_RefMuon_Track_Muid_LCW_pt20_sumet);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_etx_CentralReg", &MET_RefMuon_Track_Muid_LCW_pt20_etx_CentralReg, &b_MET_RefMuon_Track_Muid_LCW_pt20_etx_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_ety_CentralReg", &MET_RefMuon_Track_Muid_LCW_pt20_ety_CentralReg, &b_MET_RefMuon_Track_Muid_LCW_pt20_ety_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_sumet_CentralReg", &MET_RefMuon_Track_Muid_LCW_pt20_sumet_CentralReg, &b_MET_RefMuon_Track_Muid_LCW_pt20_sumet_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_phi_CentralReg", &MET_RefMuon_Track_Muid_LCW_pt20_phi_CentralReg, &b_MET_RefMuon_Track_Muid_LCW_pt20_phi_CentralReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_etx_EndcapRegion", &MET_RefMuon_Track_Muid_LCW_pt20_etx_EndcapRegion, &b_MET_RefMuon_Track_Muid_LCW_pt20_etx_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_ety_EndcapRegion", &MET_RefMuon_Track_Muid_LCW_pt20_ety_EndcapRegion, &b_MET_RefMuon_Track_Muid_LCW_pt20_ety_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_sumet_EndcapRegion", &MET_RefMuon_Track_Muid_LCW_pt20_sumet_EndcapRegion, &b_MET_RefMuon_Track_Muid_LCW_pt20_sumet_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_phi_EndcapRegion", &MET_RefMuon_Track_Muid_LCW_pt20_phi_EndcapRegion, &b_MET_RefMuon_Track_Muid_LCW_pt20_phi_EndcapRegion);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_etx_ForwardReg", &MET_RefMuon_Track_Muid_LCW_pt20_etx_ForwardReg, &b_MET_RefMuon_Track_Muid_LCW_pt20_etx_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_ety_ForwardReg", &MET_RefMuon_Track_Muid_LCW_pt20_ety_ForwardReg, &b_MET_RefMuon_Track_Muid_LCW_pt20_ety_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_sumet_ForwardReg", &MET_RefMuon_Track_Muid_LCW_pt20_sumet_ForwardReg, &b_MET_RefMuon_Track_Muid_LCW_pt20_sumet_ForwardReg);
   fChain->SetBranchAddress("MET_RefMuon_Track_Muid_LCW_pt20_phi_ForwardReg", &MET_RefMuon_Track_Muid_LCW_pt20_phi_ForwardReg, &b_MET_RefMuon_Track_Muid_LCW_pt20_phi_ForwardReg);
   fChain->SetBranchAddress("L1_2J10", &L1_2J10, &b_L1_2J10);
   fChain->SetBranchAddress("L1_2J15", &L1_2J15, &b_L1_2J15);
   fChain->SetBranchAddress("L1_2J5", &L1_2J5, &b_L1_2J5);
   fChain->SetBranchAddress("L1_J10", &L1_J10, &b_L1_J10);
   fChain->SetBranchAddress("L1_J15", &L1_J15, &b_L1_J15);
   fChain->SetBranchAddress("L1_J30", &L1_J30, &b_L1_J30);
   fChain->SetBranchAddress("L1_J5", &L1_J5, &b_L1_J5);
   fChain->SetBranchAddress("L1_J55", &L1_J55, &b_L1_J55);
   fChain->SetBranchAddress("L1_J75", &L1_J75, &b_L1_J75);
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
   fChain->SetBranchAddress("trig_DB_SMK", &trig_DB_SMK, &b_trig_DB_SMK);
   fChain->SetBranchAddress("trig_DB_L1PSK", &trig_DB_L1PSK, &b_trig_DB_L1PSK);
   fChain->SetBranchAddress("trig_DB_HLTPSK", &trig_DB_HLTPSK, &b_trig_DB_HLTPSK);
   Notify();
}

Bool_t D3PD::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void D3PD::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t D3PD::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   if (entry) return 1;   
   return 1;
}
#endif // #ifdef D3PD_cxx
