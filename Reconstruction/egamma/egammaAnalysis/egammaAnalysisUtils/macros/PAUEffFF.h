/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb 11 03:48:31 2011 by ROOT version 5.26/00e
// from TChain PAUReco/
//////////////////////////////////////////////////////////

#ifndef PAUEffFF_h
#define PAUEffFF_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class PAUEffFF {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Run;
   Int_t           Event;
   Int_t           LumiBlock;
   Int_t           BCID;
   Float_t         DeltaT_Calo;
   Float_t         DeltaT_MBTS;
   Bool_t          isInBunchTrain;
   Bool_t          isEmptyBunch;
   Int_t           nsFromTrainFront;
   Int_t           bunchesFromTrainFront;
   vector<bool>    *prevBunchStates;
   vector<float>   *energyDensityMedian;
   vector<float>   *energyDensitySigma;
   vector<int>     *energyDensityNjets;
   vector<float>   *energyDensityEtaMin;
   vector<unsigned int> *ph_L1_matchPass;
   vector<unsigned int> *ph_L2_matchPass;
   vector<unsigned int> *ph_EF_matchPass;
   vector<float>   *ph_L1_bestMatchDeltaR;
   vector<float>   *ph_L2_bestMatchDeltaR;
   vector<float>   *ph_EF_bestMatchDeltaR;
   vector<string>  *TriggersRunL1Seed_ph;
   vector<string>  *TriggersRunL1Seed_el;
   vector<string>  *TriggersRun_ph;
   vector<string>  *TriggersRun_el;
   vector<int>     *PassL1_ph;
   vector<float>   *PrescaleL1_ph;
   vector<int>     *PassL2_ph;
   vector<float>   *PrescaleL2_ph;
   vector<int>     *PassEF_ph;
   vector<float>   *PrescaleEF_ph;
   vector<int>     *PassL1_el;
   vector<float>   *PrescaleL1_el;
   vector<int>     *PassL2_el;
   vector<float>   *PrescaleL2_el;
   vector<int>     *PassEF_el;
   vector<float>   *PrescaleEF_el;
   vector<int>     *PassEF_el_passedthrough;
   Int_t           Accept_MBTSTime;
   Int_t           Accept_CaloTime;
   Int_t           Accept_Trigger;
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
   vector<float>   *PV_ID_xvertex;
   vector<float>   *PV_ID_yvertex;
   vector<float>   *PV_ID_zvertex;
   vector<int>     *PV_ntracks;
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
   vector<float>   *truth_ph_partonIsolation15_UE;
   vector<float>   *truth_ph_partonIsolation20_UE;
   vector<float>   *truth_ph_partonIsolation25_UE;
   vector<float>   *truth_ph_partonIsolation30_UE;
   vector<float>   *truth_ph_partonIsolation35_UE;
   vector<float>   *truth_ph_partonIsolation40_UE;
   vector<float>   *truth_ph_partonIsolation45_UE;
   vector<float>   *truth_ph_partonIsolation60_UE;
   vector<float>   *truth_ph_partonIsolation15;
   vector<float>   *truth_ph_partonIsolation20;
   vector<float>   *truth_ph_partonIsolation25;
   vector<float>   *truth_ph_partonIsolation30;
   vector<float>   *truth_ph_partonIsolation35;
   vector<float>   *truth_ph_partonIsolation40;
   vector<float>   *truth_ph_partonIsolation45;
   vector<float>   *truth_ph_partonIsolation60;
   vector<float>   *truth_ph_particleIsolation40;
   vector<float>   *truth_ph_particleIsolation40_ED_corrected;
   vector<float>   *truth_ph_particleIsolation40_ED_corrected_reco;
   vector<float>   *truth_jet_eta;
   vector<float>   *truth_jet_phi;
   vector<float>   *truth_jet_pt;
   vector<float>   *truth_jet_E;
   vector<int>     *truth_jet_parton;
   vector<int>     *truth_jet_iRecPhoton;
   vector<float>   *truth_jet_deltaR_RecPhoton;
   vector<int>     *ph_indexElectronSameAthenaBarcode;
   vector<int>     *ph_author;
   vector<unsigned int> *ph_isGoodOQbits;
   vector<int>     *ph_isGoodOQphoton;
   vector<int>     *ph_isGoodOQelectron;
   vector<unsigned int> *test_ph_isGoodOQbits_beforeTool;
   vector<int>     *test_ph_isGoodOQphoton_beforeTool;
   vector<int>     *test_ph_isGoodOQelectron_beforeTool;
   vector<float>   *ph_eta;
   vector<float>   *ph_etaS0;
   vector<float>   *ph_etaS1;
   vector<float>   *ph_etaS2;
   vector<float>   *ph_phi;
   vector<float>   *ph_pt;
   vector<float>   *ph_Tclus;
   vector<float>   *ph_E;
   vector<float>   *ph_ES0_real;
   vector<float>   *ph_ES1_real;
   vector<float>   *ph_ES2_real;
   vector<float>   *ph_ES3_real;
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
   vector<float>   *ph_convTrk1_chi2;
   vector<float>   *ph_convTrk2_chi2;
   vector<float>   *ph_conv_chi2;
   vector<float>   *ph_conv_dcottheta;
   vector<float>   *ph_conv_dphi;
   vector<float>   *ph_conv_dist;
   vector<float>   *ph_conv_DR1R2;
   vector<int>     *ph_convTrk1_expectHitInBLayer;
   vector<int>     *ph_convTrk1_numBLHits;
   vector<int>     *ph_convTrk1_numPixelHits;
   vector<int>     *ph_convTrk1_numSCTHits;
   vector<int>     *ph_convTrk1_numTRTHits;
   vector<int>     *ph_convTrk1_numTRTOutliers;
   vector<int>     *ph_convTrk1_numTRTHTHits;
   vector<int>     *ph_convTrk1_numTRTHTOutliers;
   vector<int>     *ph_convTrk2_expectHitInBLayer;
   vector<int>     *ph_convTrk2_numBLHits;
   vector<int>     *ph_convTrk2_numPixelHits;
   vector<int>     *ph_convTrk2_numSCTHits;
   vector<int>     *ph_convTrk2_numTRTHits;
   vector<int>     *ph_convTrk2_numTRTOutliers;
   vector<int>     *ph_convTrk2_numTRTHTHits;
   vector<int>     *ph_convTrk2_numTRTHTOutliers;
   vector<float>   *ph_convTrk1_trtPID;
   vector<float>   *ph_convTrk2_trtPID;
   vector<float>   *ph_convTrk1_trtPIDHT;
   vector<float>   *ph_convTrk2_trtPIDHT;
   vector<float>   *ph_convTrk1_trtPIDToT;
   vector<float>   *ph_convTrk2_trtPIDToT;
   vector<float>   *ph_convTrk1_ToTvar;
   vector<float>   *ph_convTrk2_ToTvar;
   vector<float>   *ph_convTrk1_trtRatioHT;
   vector<float>   *ph_convTrk2_trtRatioHT;
   vector<float>   *ph_convTrk1_charge;
   vector<float>   *ph_convTrk2_charge;
   vector<float>   *ph_convTrk1_DeltaEta_track_calo;
   vector<float>   *ph_convTrk2_DeltaEta_track_calo;
   vector<float>   *ph_convTrk1_DeltaPhi_track_calo;
   vector<float>   *ph_convTrk2_DeltaPhi_track_calo;
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
   vector<float>   *ph_parton_pt_max_MC;
   vector<float>   *ph_parton_eta_MC;
   vector<float>   *ph_parton_phi_MC;
   vector<int>     *ph_parton_barcode_MC;
   vector<int>     *ph_parton_pdg_MC;
   vector<unsigned int> *ph_isEM;
   vector<int>     *ph_isPhotonLoose;
   vector<int>     *ph_isPhotonTight;
   vector<int>     *ph_ambiguityResult;
   vector<int>     *ph_itune1;
   vector<int>     *ph_itune2;
   vector<int>     *ph_itune3;
   vector<float>   *ph_logLikelihood;
   vector<float>   *ph_PtIsolationCone;
   vector<float>   *ph_PtIsolationConePhAngle;
   vector<float>   *ph_trackIsol;
   vector<float>   *ph_PtIsolationCone_woIC;
   vector<float>   *ph_PtIsolationConePhAngle_woIC;
   vector<float>   *ph_trackIsol_woIC;
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
   vector<float>   *ph_etconoisedR04Sig2;
   vector<float>   *ph_etconoisedR04Sig3;
   vector<float>   *ph_shwr_EtCone20;
   vector<float>   *ph_shwr_EtCone30;
   vector<float>   *ph_shwr_EtCone40;
   vector<float>   *ph_shwr_EtCone45;
   vector<float>   *ph_shwr_EtCone20_corrected;
   vector<float>   *ph_shwr_EtCone30_corrected;
   vector<float>   *ph_shwr_EtCone40_corrected;
   vector<float>   *ph_shwr_EtCone20_pt_corrected;
   vector<float>   *ph_shwr_EtCone30_pt_corrected;
   vector<float>   *ph_shwr_EtCone40_pt_corrected;
   vector<float>   *ph_shwr_EtCone20_ED_corrected;
   vector<float>   *ph_shwr_EtCone30_ED_corrected;
   vector<float>   *ph_shwr_EtCone40_ED_corrected;
   vector<float>   *ph_shwr_EtCone40_ED0_corrected;
   vector<float>   *ph_shwr_EtCone40_ED1_corrected;
   vector<float>   *ph_shwr_EtCone40_ED2_corrected;
   vector<float>   *ph_shwr_EtCone40_ED3_corrected;
   vector<float>   *ph_shwr_EtCone40_ED4_corrected;
   vector<float>   *ph_shwr_EtCone40_ED5_corrected;
   vector<float>   *ph_ED_median;
   vector<float>   *ph_ED_sigma;
   vector<int>     *ph_ED_Njets;
   vector<float>   *ph_transmin_etcone40;
   vector<float>   *ph_transmax_etcone40;
   vector<float>   *ph_transmin_etcone100;
   vector<float>   *ph_transmax_etcone100;
   vector<float>   *ph_transmin;
   vector<float>   *ph_transmax;
   vector<float>   *ph_transmin_loweta;
   vector<float>   *ph_transmax_loweta;
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
   vector<int>     *ph_isG4part;
   vector<int>     *ph_motherPDG;
   vector<int>     *ph_isBrem_MC;
   vector<int>     *ph_isFromHardProc_MC;
   vector<int>     *ph_isPhotonFromHardProc_MC;
   vector<int>     *ph_imatchParton_MC;
   vector<float>   *ph_partonIsolation15_UE_MC;
   vector<float>   *ph_partonIsolation20_UE_MC;
   vector<float>   *ph_partonIsolation25_UE_MC;
   vector<float>   *ph_partonIsolation30_UE_MC;
   vector<float>   *ph_partonIsolation35_UE_MC;
   vector<float>   *ph_partonIsolation40_UE_MC;
   vector<float>   *ph_partonIsolation45_UE_MC;
   vector<float>   *ph_partonIsolation60_UE_MC;
   vector<float>   *ph_partonIsolation15_MC;
   vector<float>   *ph_partonIsolation20_MC;
   vector<float>   *ph_partonIsolation25_MC;
   vector<float>   *ph_partonIsolation30_MC;
   vector<float>   *ph_partonIsolation35_MC;
   vector<float>   *ph_partonIsolation40_MC;
   vector<float>   *ph_partonIsolation45_MC;
   vector<float>   *ph_partonIsolation60_MC;
   vector<float>   *ph_particleIsolation40_MC;
   vector<float>   *ph_particleIsolation40_ED_corrected_MC;
   vector<float>   *ph_particleIsolation40_ED_corrected_reco_MC;
   vector<float>   *ph_shwr_EtCone10;
   vector<float>   *ph_shwr_EtCone15;
   vector<float>   *ph_shwr_EtCone25;
   vector<float>   *ph_shwr_EtCone35;
   vector<float>   *ph_shwr_EtCone10_corrected;
   vector<float>   *ph_shwr_EtCone15_corrected;
   vector<float>   *ph_shwr_EtCone25_corrected;
   vector<float>   *ph_shwr_EtCone35_corrected;
   vector<float>   *ph_shwr_EtCone10_pt_corrected;
   vector<float>   *ph_shwr_EtCone15_pt_corrected;
   vector<float>   *ph_shwr_EtCone25_pt_corrected;
   vector<float>   *ph_shwr_EtCone35_pt_corrected;
   vector<float>   *ph_shwr_EtCone10_ED_corrected;
   vector<float>   *ph_shwr_EtCone15_ED_corrected;
   vector<float>   *ph_shwr_EtCone25_ED_corrected;
   vector<float>   *ph_shwr_EtCone35_ED_corrected;
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
   vector<float>   *trk_charge;
   vector<float>   *trk_d0;
   vector<float>   *trk_x0;
   vector<float>   *trk_y0;
   vector<float>   *trk_z0;
   vector<int>     *trk_nhits_b;
   vector<int>     *trk_nhits_p;
   vector<int>     *trk_ndead_p;
   vector<int>     *trk_nhits_s;
   vector<int>     *trk_ndead_s;
   vector<int>     *trk_nhits_trt;
   vector<int>     *trk_nhits_trth;
   vector<float>   *trk_chiSquared;
   vector<float>   *trk_numberDoF;
   vector<int>     *trk_gConv;
   vector<int>     *trk_conv;
   vector<vector<float> > *trk_calo_eta;
   vector<vector<float> > *trk_calo_phi;
   vector<vector<float> > *trk_calo_r;
   vector<float>   *jet_eta;
   vector<float>   *jet_phi;
   vector<float>   *jet_pt;
   vector<float>   *jet_E;
   vector<int>     *jet_selected_1j;
   vector<int>     *jet_selected_2j;
   vector<int>     *jet_n90cells;
   vector<int>     *jet_n90constituents;
   vector<float>   *jet_hecf;
   vector<float>   *jet_tileGap3F;
   vector<float>   *jet_fcor;
   vector<float>   *jet_corr_cell;
   vector<float>   *jet_corr_dotx;
   vector<float>   *jet_samplingMax;
   vector<int>     *jet_samplingMaxId;
   vector<float>   *jet_emFraction;
   vector<float>   *jet_pt_emScale;
   vector<float>   *jet_eta_emScale;
   vector<float>   *jet_phi_emScale;
   vector<float>   *jet_e_emScale;
   vector<float>   *jet_time;
   vector<float>   *jet_quality;
   vector<bool>    *jet_isGood;
   vector<bool>    *jet_isBad;
   vector<bool>    *jet_isUgly;
   vector<float>   *jet_EMJES;
   vector<int>     *jet_imatchTrueJet_MC;
   vector<float>   *jet_eta_MC;
   vector<float>   *jet_phi_MC;
   vector<float>   *jet_pt_MC;
   vector<float>   *jet_E_MC;
   vector<int>     *el_author;
   vector<unsigned int> *el_isGoodOQbits;
   vector<int>     *el_isGoodOQphoton;
   vector<int>     *el_isGoodOQelectron;
   vector<unsigned int> *test_el_isGoodOQbits_beforeTool;
   vector<int>     *test_el_isGoodOQphoton_beforeTool;
   vector<int>     *test_el_isGoodOQelectron_beforeTool;
   vector<float>   *el_eta;
   vector<float>   *el_phi;
   vector<float>   *el_pt;
   vector<float>   *el_E;
   vector<float>   *el_etaS0;
   vector<float>   *el_etaS1;
   vector<float>   *el_etaS2;
   vector<float>   *el_ES0_real;
   vector<float>   *el_ES1_real;
   vector<float>   *el_ES2_real;
   vector<float>   *el_ES3_real;
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
   vector<float>   *el_pt_trk;
   vector<float>   *el_eta_trk;
   vector<float>   *el_d0;
   vector<float>   *el_z0;
   vector<float>   *el_theta;
   vector<float>   *el_phi_trk;
   vector<float>   *el_qOverP;
   vector<int>     *el_nBHits;
   vector<int>     *el_nPixelHits;
   vector<int>     *el_nSCTHits;
   vector<int>     *el_nTRTHits;
   vector<int>     *el_iPV;
   vector<unsigned int> *el_isEM;
   vector<int>     *el_isElectronLoose;
   vector<int>     *el_isElectronMedium;
   vector<int>     *el_isElectronTight;
   vector<int>     *el_isRobustLoose;
   vector<int>     *el_isRobustMedium;
   vector<int>     *el_isRobustTight;
   vector<int>     *el_isRobusterTight;
   vector<int>     *el_expectHitInBLayer;
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
   vector<int>     *el_ambiguityResult;
   vector<int>     *el_itune1;
   vector<int>     *el_itune2;
   vector<int>     *el_itune3;
   vector<float>   *el_logLikelihood;
   vector<float>   *el_PtIsolationCone;
   vector<float>   *el_trackIsol;
   vector<float>   *el_PtIsolationCone_woIC;
   vector<float>   *el_trackIsol_woIC;
   vector<float>   *el_dR_isol;
   vector<int>     *el_selected_etmiss;
   vector<int>     *el_selected_etlep;
   vector<float>   *el_shwr_EtCone20;
   vector<float>   *el_shwr_EtCone30;
   vector<float>   *el_shwr_EtCone40;
   vector<float>   *el_shwr_EtCone45;
   vector<float>   *el_shwr_EtCone20_corrected;
   vector<float>   *el_shwr_EtCone30_corrected;
   vector<float>   *el_shwr_EtCone40_corrected;
   vector<float>   *el_shwr_EtCone20_pt_corrected;
   vector<float>   *el_shwr_EtCone30_pt_corrected;
   vector<float>   *el_shwr_EtCone40_pt_corrected;
   vector<float>   *el_shwr_EtCone20_ED_corrected;
   vector<float>   *el_shwr_EtCone30_ED_corrected;
   vector<float>   *el_shwr_EtCone40_ED_corrected;
   vector<float>   *el_ED_median;
   vector<float>   *el_ED_sigma;
   vector<int>     *el_ED_Njets;
   vector<float>   *el_shwr_EtRing1030sig2;
   vector<float>   *el_shwr_EtRing1030sig3;
   vector<float>   *el_shwr_EtRing1030sig4;
   vector<int>     *el_matchMC;
   vector<float>   *el_eta_MC;
   vector<float>   *el_phi_MC;
   vector<float>   *el_pt_MC;
   vector<float>   *el_E_MC;
   vector<int>     *el_PDG;
   vector<int>     *el_motherPDG;
   vector<unsigned int> *el_as_unconv_ph_isEM;
   vector<int>     *el_as_unconv_ph_isPhotonLoose;
   vector<int>     *el_as_unconv_ph_isPhotonTight;
   vector<float>   *el_as_unconv_ph_E;
   vector<float>   *el_as_unconv_ph_eta_clus;
   vector<float>   *el_as_unconv_ph_phi_clus;
   vector<float>   *el_as_unconv_ph_pt_clus;
   vector<float>   *el_as_unconv_ph_etaS1;
   vector<float>   *el_as_unconv_ph_etaS2;
   vector<unsigned int> *el_as_conv_ph_isEM;
   vector<int>     *el_as_conv_ph_isPhotonLoose;
   vector<int>     *el_as_conv_ph_isPhotonTight;
   vector<float>   *el_as_conv_ph_E;
   vector<float>   *el_as_conv_ph_eta_clus;
   vector<float>   *el_as_conv_ph_phi_clus;
   vector<float>   *el_as_conv_ph_pt_clus;
   vector<float>   *el_as_conv_ph_etaS1;
   vector<float>   *el_as_conv_ph_etaS2;
   vector<float>   *el_as_conv_ph_shwr_Ethad1;
   vector<float>   *el_as_conv_ph_shwr_Ethad;
   vector<float>   *el_as_conv_ph_shwr_E277;
   vector<float>   *el_as_conv_ph_shwr_E237;
   vector<float>   *el_as_conv_ph_shwr_E233;
   vector<float>   *el_as_conv_ph_shwr_weta2;
   vector<float>   *el_as_conv_ph_shwr_f1;
   vector<float>   *el_as_conv_ph_shwr_Emax2;
   vector<float>   *el_as_conv_ph_shwr_Emax1;
   vector<float>   *el_as_conv_ph_shwr_Emin;
   vector<float>   *el_as_conv_ph_shwr_fracm;
   vector<float>   *el_as_conv_ph_shwr_wtot;
   vector<float>   *el_as_conv_ph_shwr_w1;
   vector<float>   *el_as_unconv_ph_shwr_Ethad1;
   vector<float>   *el_as_unconv_ph_shwr_Ethad;
   vector<float>   *el_as_unconv_ph_shwr_E277;
   vector<float>   *el_as_unconv_ph_shwr_E237;
   vector<float>   *el_as_unconv_ph_shwr_E233;
   vector<float>   *el_as_unconv_ph_shwr_weta2;
   vector<float>   *el_as_unconv_ph_shwr_f1;
   vector<float>   *el_as_unconv_ph_shwr_Emax2;
   vector<float>   *el_as_unconv_ph_shwr_Emax1;
   vector<float>   *el_as_unconv_ph_shwr_Emin;
   vector<float>   *el_as_unconv_ph_shwr_fracm;
   vector<float>   *el_as_unconv_ph_shwr_wtot;
   vector<float>   *el_as_unconv_ph_shwr_w1;
   vector<unsigned int> *el_L1_matchPass;
   vector<unsigned int> *el_L2_matchPass;
   vector<unsigned int> *el_EF_matchPass;
   vector<float>   *el_L1_bestMatchDeltaR;
   vector<float>   *el_L2_bestMatchDeltaR;
   vector<float>   *el_EF_bestMatchDeltaR;
   vector<unsigned int> *el_L1_matchPass_ph;
   vector<unsigned int> *el_L2_matchPass_ph;
   vector<unsigned int> *el_EF_matchPass_ph;
   vector<float>   *el_L1_bestMatchDeltaR_ph;
   vector<float>   *el_L2_bestMatchDeltaR_ph;
   vector<float>   *el_EF_bestMatchDeltaR_ph;
   vector<float>   *el_L1_et;
   vector<float>   *el_L1_eta;
   vector<float>   *el_L1_phi;
   vector<float>   *el_L1_EMIsol;
   vector<float>   *el_L1_HadIsol;
   vector<float>   *el_L1_HadCore;
   vector<float>   *el_L2_e;
   vector<float>   *el_L2_pt;
   vector<float>   *el_L2_eta;
   vector<float>   *el_L2_phi;
   vector<float>   *el_L2_cl_e;
   vector<float>   *el_L2_cl_et;
   vector<float>   *el_L2_cl_eta;
   vector<float>   *el_L2_cl_phi;
   vector<float>   *el_L2_cl_e237;
   vector<float>   *el_L2_cl_e277;
   vector<float>   *el_L2_cl_fracs1;
   vector<float>   *el_L2_cl_weta2;
   vector<float>   *el_L2_cl_ehad1;
   vector<float>   *el_L2_cl_emaxs1;
   vector<float>   *el_L2_cl_e2tsts1;
   vector<float>   *el_L2_trkCl_etOverPt;
   vector<float>   *el_L2_trkCl_dEta;
   vector<float>   *el_L2_trkCl_dPhi;
   vector<float>   *el_L2_trk_pt;
   vector<float>   *el_L2_trk_errPt;
   vector<float>   *el_L2_trk_etaAtCalo;
   vector<float>   *el_L2_trk_errEta;
   vector<float>   *el_L2_trk_phiAtCalo;
   vector<float>   *el_L2_trk_errPhi;
   vector<int>     *el_L2_trk_charge;
   vector<int>     *el_L2_trk_algo;
   vector<float>   *el_L2_trk_zvtx;
   vector<float>   *el_L2_trk_errZvtx;
   vector<float>   *el_EF_e;
   vector<float>   *el_EF_et;
   vector<float>   *el_EF_eta;
   vector<float>   *el_EF_phi;
   vector<int>     *el_EF_author;
   vector<int>     *el_EF_IsEM;
   vector<float>   *el_EF_ethad;
   vector<float>   *el_EF_ethad1;
   vector<float>   *el_EF_e233;
   vector<float>   *el_EF_e237;
   vector<float>   *el_EF_e277;
   vector<float>   *el_EF_emaxs1;
   vector<float>   *el_EF_e2tsts1;
   vector<float>   *el_EF_fracs1;
   vector<float>   *el_EF_weta1;
   vector<float>   *el_EF_weta2;
   vector<float>   *el_EF_wtots1;
   vector<float>   *el_EF_f1;
   vector<float>   *el_EF_emins1;
   vector<float>   *el_EF_etcone20;
   vector<float>   *el_EF_dEta;
   vector<float>   *el_EF_dPhi;
   vector<float>   *el_EF_etOverPt;
   vector<int>     *el_EF_nBL;
   vector<int>     *el_EF_nPix;
   vector<int>     *el_EF_nSCT;
   vector<int>     *el_EF_nTRThigh;
   vector<int>     *el_EF_nTRThighOut;
   vector<int>     *el_EF_nTRT;
   vector<int>     *el_EF_nTRTOut;
   vector<float>   *el_EF_trkPt;
   vector<float>   *el_EF_trkD0;
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
   Float_t         mEt_topo_ex;
   Float_t         mEt_topo_ey;
   Float_t         mEt_topo_et;
   Float_t         mEt_topo_sumet;
   Float_t         mEt_Topo_central_ex;
   Float_t         mEt_Topo_central_ey;
   Float_t         mEt_Topo_central_sumet;
   Float_t         mEt_Topo_endcap_ex;
   Float_t         mEt_Topo_endcap_ey;
   Float_t         mEt_Topo_endcap_sumet;
   Float_t         mEt_Topo_forward_ex;
   Float_t         mEt_Topo_forward_ey;
   Float_t         mEt_Topo_forward_sumet;
   Float_t         mEt_Topo_PEMB_ex;
   Float_t         mEt_Topo_PEMB_ey;
   Float_t         mEt_Topo_PEMB_ncell;
   Float_t         mEt_Topo_PEMB_sumet;
   Float_t         mEt_Topo_EMB_ex;
   Float_t         mEt_Topo_EMB_ey;
   Float_t         mEt_Topo_EMB_ncell;
   Float_t         mEt_Topo_EMB_sumet;
   Float_t         mEt_Topo_PEMEC_ex;
   Float_t         mEt_Topo_PEMEC_ey;
   Float_t         mEt_Topo_PEMEC_ncell;
   Float_t         mEt_Topo_PEMEC_sumet;
   Float_t         mEt_Topo_EME_ex;
   Float_t         mEt_Topo_EME_ey;
   Float_t         mEt_Topo_EME_ncell;
   Float_t         mEt_Topo_EME_sumet;
   Float_t         mEt_Topo_TILE_ex;
   Float_t         mEt_Topo_TILE_ey;
   Float_t         mEt_Topo_TILE_ncell;
   Float_t         mEt_Topo_TILE_sumet;
   Float_t         mEt_Topo_HEC_ex;
   Float_t         mEt_Topo_HEC_ey;
   Float_t         mEt_Topo_HEC_ncell;
   Float_t         mEt_Topo_HEC_sumet;
   Float_t         mEt_Topo_FCAL_ex;
   Float_t         mEt_Topo_FCAL_ey;
   Float_t         mEt_Topo_FCAL_ncell;
   Float_t         mEt_Topo_FCAL_sumet;
   Float_t         mEt_LocHadtopo_ex;
   Float_t         mEt_LocHadtopo_ey;
   Float_t         mEt_LocHadtopo_sumet;
   Float_t         mEt_LocHadtopo_et;
   Float_t         mEt_LocHadtopo_central_ex;
   Float_t         mEt_LocHadtopo_central_ey;
   Float_t         mEt_LocHadtopo_central_sumet;
   Float_t         mEt_LocHadtopo_endcap_ex;
   Float_t         mEt_LocHadtopo_endcap_ey;
   Float_t         mEt_LocHadtopo_endcap_sumet;
   Float_t         mEt_LocHadtopo_forward_ex;
   Float_t         mEt_LocHadtopo_forward_ey;
   Float_t         mEt_LocHadtopo_forward_sumet;
   Float_t         mEt_LocHadtopo_PEMB_ex;
   Float_t         mEt_LocHadtopo_PEMB_ey;
   Float_t         mEt_LocHadtopo_PEMB_ncell;
   Float_t         mEt_LocHadtopo_PEMB_sumet;
   Float_t         mEt_LocHadtopo_EMB_ex;
   Float_t         mEt_LocHadtopo_EMB_ey;
   Float_t         mEt_LocHadtopo_EMB_ncell;
   Float_t         mEt_LocHadtopo_EMB_sumet;
   Float_t         mEt_LocHadtopo_PEMEC_ex;
   Float_t         mEt_LocHadtopo_PEMEC_ey;
   Float_t         mEt_LocHadtopo_PEMEC_ncell;
   Float_t         mEt_LocHadtopo_PEMEC_sumet;
   Float_t         mEt_LocHadtopo_EME_ex;
   Float_t         mEt_LocHadtopo_EME_ey;
   Float_t         mEt_LocHadtopo_EME_ncell;
   Float_t         mEt_LocHadtopo_EME_sumet;
   Float_t         mEt_LocHadtopo_TILE_ex;
   Float_t         mEt_LocHadtopo_TILE_ey;
   Float_t         mEt_LocHadtopo_TILE_ncell;
   Float_t         mEt_LocHadtopo_TILE_sumet;
   Float_t         mEt_LocHadtopo_HEC_ex;
   Float_t         mEt_LocHadtopo_HEC_ey;
   Float_t         mEt_LocHadtopo_HEC_ncell;
   Float_t         mEt_LocHadtopo_HEC_sumet;
   Float_t         mEt_LocHadtopo_FCAL_ex;
   Float_t         mEt_LocHadtopo_FCAL_ey;
   Float_t         mEt_LocHadtopo_FCAL_ncell;
   Float_t         mEt_LocHadtopo_FCAL_sumet;
   Float_t         mEt_MuonBoy_Track_ex;
   Float_t         mEt_MuonBoy_Track_ey;
   Float_t         mEt_MuonBoy_Track_sumet;
   Float_t         mEt_MuonBoy_ex;
   Float_t         mEt_MuonBoy_ey;
   Float_t         mEt_MuonBoy_sumet;
   Float_t         mEt_MuID_ex;
   Float_t         mEt_MuID_ey;
   Float_t         mEt_MuID_sumet;
   Float_t         mEt_Cryo_ex;
   Float_t         mEt_Cryo_ey;
   Float_t         mEt_Cryo_sumet;
   Float_t         mEt_RefEle_ex;
   Float_t         mEt_RefEle_ey;
   Float_t         mEt_RefEle_sumet;
   Float_t         mEt_RefGamma_ex;
   Float_t         mEt_RefGamma_ey;
   Float_t         mEt_RefGamma_sumet;
   Float_t         mEt_RefTau_ex;
   Float_t         mEt_RefTau_ey;
   Float_t         mEt_RefTau_sumet;
   Float_t         mEt_RefJet_ex;
   Float_t         mEt_RefJet_ey;
   Float_t         mEt_RefJet_sumet;
   Float_t         mEt_RefMuon_ex;
   Float_t         mEt_RefMuon_ey;
   Float_t         mEt_RefMuon_sumet;
   Float_t         mEt_RefMuon_Track_ex;
   Float_t         mEt_RefMuon_Track_ey;
   Float_t         mEt_RefMuon_Track_sumet;
   Float_t         mEt_CellOut_ex;
   Float_t         mEt_CellOut_ey;
   Float_t         mEt_CellOut_sumet;
   Float_t         mEt_RefFinal_ex;
   Float_t         mEt_RefFinal_ey;
   Float_t         mEt_RefFinal_sumet;
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
   Float_t         hgg_HPV_zcommon;
   Float_t         hgg_HPV_zcommon_primVtxLH;
   Float_t         hgg_HPV_mgg;
   Float_t         hgg_HPV_mgg_primVtxLH;
   Float_t         hgg_HPV_ptgg;
   Float_t         hgg_HPV_detagg;
   Float_t         hgg_HPV_dphigg;
   Float_t         hgg_HPV_cosThetaStar;
   Float_t         hgg_HPV_cosThetaStarCS;
   vector<float>   *hgg_HPV_eta_zcommon;
   vector<float>   *hgg_HPV_eta_zcommon_primVtxLH;
   Float_t         hgg_HPV_vtxLikelihood;
   Float_t         hgg_HPV_mgg_Ztruth;
   vector<float>   *ph_HPV_convTrk1_zvertex;
   vector<float>   *ph_HPV_convTrk2_zvertex;
   vector<float>   *ph_HPV_convTrk1_sigma_zvertex;
   vector<float>   *ph_HPV_convTrk2_sigma_zvertex;
   vector<float>   *el_CaloPointing_zvertex;
   vector<float>   *el_CaloPointing_sigma_zvertex;
   vector<float>   *el_CaloPointing_eta;
   vector<float>   *el_CaloPointing_sigma_eta;
   vector<float>   *ph_CaloPointing_zvertex;
   vector<float>   *ph_CaloPointing_sigma_zvertex;
   vector<float>   *ph_CaloPointing_eta;
   vector<float>   *ph_CaloPointing_sigma_eta;
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
   vector<float>   *test_ph_E_calopoint;
   vector<float>   *test_ph_eta_calopoint;
   vector<float>   *test_ph_phi_calopoint;
   vector<float>   *test_ph_pt_calopoint;
   vector<int>     *ph_particleType;
   vector<int>     *ph_particleOrig;
   vector<float>   *clus_SW_eta;
   vector<float>   *clus_SW_phi;
   vector<float>   *clus_SW_pt;
   vector<float>   *clus_SW_E;
   vector<int>     *clus_SW_size;
   vector<int>     *clus_SW_etasize;
   vector<int>     *clus_SW_phisize;
   vector<float>   *clus_SW_time;
   vector<float>   *clus_SW_eta_MC;
   vector<float>   *clus_SW_phi_MC;
   vector<float>   *clus_SW_pt_MC;
   vector<float>   *clus_SW_E_MC;
   vector<float>   *clus_SW_PDG;
   vector<float>   *clus_SW_motherPDG;
   vector<float>   *clus_EMtopo_eta;
   vector<float>   *clus_EMtopo_phi;
   vector<float>   *clus_EMtopo_pt;
   vector<float>   *clus_EMtopo_E;
   vector<float>   *clus_EMtopo_time;
   vector<float>   *clus_EMtopo_eta_MC;
   vector<float>   *clus_EMtopo_phi_MC;
   vector<float>   *clus_EMtopo_pt_MC;
   vector<float>   *clus_EMtopo_E_MC;
   vector<float>   *clus_EMtopo_PDG;
   vector<float>   *clus_EMtopo_motherPDG;
   vector<int>     *clus_EMtopo_iRecPhoton;
   vector<float>   *clus_EMtopo_deltaR_RecPhoton;
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
   vector<float>   *clus_topo420_E;
   vector<float>   *clus_lhtopo420_E;
   vector<float>   *clus_lhtopo420_phi;
   vector<float>   *clus_lhtopo420_eta;
   Float_t         UED_diphomass;
   Float_t         UED_diphoang;

   // List of branches
   TBranch        *b_Run;   //!
   TBranch        *b_Event;   //!
   TBranch        *b_LumiBlock;   //!
   TBranch        *b_BCID;   //!
   TBranch        *b_DeltaT_Calo;   //!
   TBranch        *b_DeltaT_MBTS;   //!
   TBranch        *b_isInBunchTrain;   //!
   TBranch        *b_isEmptyBunch;   //!
   TBranch        *b_nsFromTrainFront;   //!
   TBranch        *b_bunchesFromTrainFront;   //!
   TBranch        *b_prevBunchStates;   //!
   TBranch        *b_energyDensityMedian;   //!
   TBranch        *b_energyDensitySigma;   //!
   TBranch        *b_energyDensityNjets;   //!
   TBranch        *b_energyDensityEtaMin;   //!
   TBranch        *b_ph_L1_matchPass;   //!
   TBranch        *b_ph_L2_matchPass;   //!
   TBranch        *b_ph_EF_matchPass;   //!
   TBranch        *b_ph_L1_bestMatchDeltaR;   //!
   TBranch        *b_ph_L2_bestMatchDeltaR;   //!
   TBranch        *b_ph_EF_bestMatchDeltaR;   //!
   TBranch        *b_TriggersRunL1Seed_ph;   //!
   TBranch        *b_TriggersRunL1Seed_el;   //!
   TBranch        *b_TriggersRun_ph;   //!
   TBranch        *b_TriggersRun_el;   //!
   TBranch        *b_PassL1_ph;   //!
   TBranch        *b_PrescaleL1_ph;   //!
   TBranch        *b_PassL2_ph;   //!
   TBranch        *b_PrescaleL2_ph;   //!
   TBranch        *b_PassEF_ph;   //!
   TBranch        *b_PrescaleEF_ph;   //!
   TBranch        *b_PassL1_el;   //!
   TBranch        *b_PrescaleL1_el;   //!
   TBranch        *b_PassL2_el;   //!
   TBranch        *b_PrescaleL2_el;   //!
   TBranch        *b_PassEF_el;   //!
   TBranch        *b_PrescaleEF_el;   //!
   TBranch        *b_PassEF_el_passedthrough;   //!
   TBranch        *b_Accept_MBTSTime;   //!
   TBranch        *b_Accept_CaloTime;   //!
   TBranch        *b_Accept_Trigger;   //!
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
   TBranch        *b_PV_ID_xvertex;   //!
   TBranch        *b_PV_ID_yvertex;   //!
   TBranch        *b_PV_ID_zvertex;   //!
   TBranch        *b_PV_ntracks;   //!
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
   TBranch        *b_truth_ph_partonIsolation15_UE;   //!
   TBranch        *b_truth_ph_partonIsolation20_UE;   //!
   TBranch        *b_truth_ph_partonIsolation25_UE;   //!
   TBranch        *b_truth_ph_partonIsolation30_UE;   //!
   TBranch        *b_truth_ph_partonIsolation35_UE;   //!
   TBranch        *b_truth_ph_partonIsolation40_UE;   //!
   TBranch        *b_truth_ph_partonIsolation45_UE;   //!
   TBranch        *b_truth_ph_partonIsolation60_UE;   //!
   TBranch        *b_truth_ph_partonIsolation15;   //!
   TBranch        *b_truth_ph_partonIsolation20;   //!
   TBranch        *b_truth_ph_partonIsolation25;   //!
   TBranch        *b_truth_ph_partonIsolation30;   //!
   TBranch        *b_truth_ph_partonIsolation35;   //!
   TBranch        *b_truth_ph_partonIsolation40;   //!
   TBranch        *b_truth_ph_partonIsolation45;   //!
   TBranch        *b_truth_ph_partonIsolation60;   //!
   TBranch        *b_truth_ph_particleIsolation40;   //!
   TBranch        *b_truth_ph_particleIsolation40_ED_corrected;   //!
   TBranch        *b_truth_ph_particleIsolation40_ED_corrected_reco;   //!
   TBranch        *b_truth_jet_eta;   //!
   TBranch        *b_truth_jet_phi;   //!
   TBranch        *b_truth_jet_pt;   //!
   TBranch        *b_truth_jet_E;   //!
   TBranch        *b_truth_jet_parton;   //!
   TBranch        *b_truth_jet_iRecPhoton;   //!
   TBranch        *b_truth_jet_deltaR_RecPhoton;   //!
   TBranch        *b_ph_indexElectronSameAthenaBarcode;   //!
   TBranch        *b_ph_author;   //!
   TBranch        *b_ph_isGoodOQbits;   //!
   TBranch        *b_ph_isGoodOQphoton;   //!
   TBranch        *b_ph_isGoodOQelectron;   //!
   TBranch        *b_test_ph_isGoodOQbits_beforeTool;   //!
   TBranch        *b_test_ph_isGoodOQphoton_beforeTool;   //!
   TBranch        *b_test_ph_isGoodOQelectron_beforeTool;   //!
   TBranch        *b_ph_eta;   //!
   TBranch        *b_ph_etaS0;   //!
   TBranch        *b_ph_etaS1;   //!
   TBranch        *b_ph_etaS2;   //!
   TBranch        *b_ph_phi;   //!
   TBranch        *b_ph_pt;   //!
   TBranch        *b_ph_Tclus;   //!
   TBranch        *b_ph_E;   //!
   TBranch        *b_ph_ES0_real;   //!
   TBranch        *b_ph_ES1_real;   //!
   TBranch        *b_ph_ES2_real;   //!
   TBranch        *b_ph_ES3_real;   //!
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
   TBranch        *b_ph_convTrk1_chi2;   //!
   TBranch        *b_ph_convTrk2_chi2;   //!
   TBranch        *b_ph_conv_chi2;   //!
   TBranch        *b_ph_conv_dcottheta;   //!
   TBranch        *b_ph_conv_dphi;   //!
   TBranch        *b_ph_conv_dist;   //!
   TBranch        *b_ph_conv_DR1R2;   //!
   TBranch        *b_ph_convTrk1_expectHitInBLayer;   //!
   TBranch        *b_ph_convTrk1_numBLHits;   //!
   TBranch        *b_ph_convTrk1_numPixelHits;   //!
   TBranch        *b_ph_convTrk1_numSCTHits;   //!
   TBranch        *b_ph_convTrk1_numTRTHits;   //!
   TBranch        *b_ph_convTrk1_numTRTOutliers;   //!
   TBranch        *b_ph_convTrk1_numTRTHTHits;   //!
   TBranch        *b_ph_convTrk1_numTRTHTOutliers;   //!
   TBranch        *b_ph_convTrk2_expectHitInBLayer;   //!
   TBranch        *b_ph_convTrk2_numBLHits;   //!
   TBranch        *b_ph_convTrk2_numPixelHits;   //!
   TBranch        *b_ph_convTrk2_numSCTHits;   //!
   TBranch        *b_ph_convTrk2_numTRTHits;   //!
   TBranch        *b_ph_convTrk2_numTRTOutliers;   //!
   TBranch        *b_ph_convTrk2_numTRTHTHits;   //!
   TBranch        *b_ph_convTrk2_numTRTHTOutliers;   //!
   TBranch        *b_ph_convTrk1_trtPID;   //!
   TBranch        *b_ph_convTrk2_trtPID;   //!
   TBranch        *b_ph_convTrk1_trtPIDHT;   //!
   TBranch        *b_ph_convTrk2_trtPIDHT;   //!
   TBranch        *b_ph_convTrk1_trtPIDToT;   //!
   TBranch        *b_ph_convTrk2_trtPIDToT;   //!
   TBranch        *b_ph_convTrk1_ToTvar;   //!
   TBranch        *b_ph_convTrk2_ToTvar;   //!
   TBranch        *b_ph_convTrk1_trtRatioHT;   //!
   TBranch        *b_ph_convTrk2_trtRatioHT;   //!
   TBranch        *b_ph_convTrk1_charge;   //!
   TBranch        *b_ph_convTrk2_charge;   //!
   TBranch        *b_ph_convTrk1_DeltaEta_track_calo;   //!
   TBranch        *b_ph_convTrk2_DeltaEta_track_calo;   //!
   TBranch        *b_ph_convTrk1_DeltaPhi_track_calo;   //!
   TBranch        *b_ph_convTrk2_DeltaPhi_track_calo;   //!
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
   TBranch        *b_ph_parton_pt_max_MC;   //!
   TBranch        *b_ph_parton_eta_MC;   //!
   TBranch        *b_ph_parton_phi_MC;   //!
   TBranch        *b_ph_parton_barcode_MC;   //!
   TBranch        *b_ph_parton_pdg_MC;   //!
   TBranch        *b_ph_isEM;   //!
   TBranch        *b_ph_isPhotonLoose;   //!
   TBranch        *b_ph_isPhotonTight;   //!
   TBranch        *b_ph_ambiguityResult;   //!
   TBranch        *b_ph_itune1;   //!
   TBranch        *b_ph_itune2;   //!
   TBranch        *b_ph_itune3;   //!
   TBranch        *b_ph_logLikelihood;   //!
   TBranch        *b_ph_PtIsolationCone;   //!
   TBranch        *b_ph_PtIsolationConePhAngle;   //!
   TBranch        *b_ph_trackIsol;   //!
   TBranch        *b_ph_PtIsolationCone_woIC;   //!
   TBranch        *b_ph_PtIsolationConePhAngle_woIC;   //!
   TBranch        *b_ph_trackIsol_woIC;   //!
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
   TBranch        *b_ph_etconoisedR04Sig2;   //!
   TBranch        *b_ph_etconoisedR04Sig3;   //!
   TBranch        *b_ph_shwr_EtCone20;   //!
   TBranch        *b_ph_shwr_EtCone30;   //!
   TBranch        *b_ph_shwr_EtCone40;   //!
   TBranch        *b_ph_shwr_EtCone45;   //!
   TBranch        *b_ph_shwr_EtCone20_corrected;   //!
   TBranch        *b_ph_shwr_EtCone30_corrected;   //!
   TBranch        *b_ph_shwr_EtCone40_corrected;   //!
   TBranch        *b_ph_shwr_EtCone20_pt_corrected;   //!
   TBranch        *b_ph_shwr_EtCone30_pt_corrected;   //!
   TBranch        *b_ph_shwr_EtCone40_pt_corrected;   //!
   TBranch        *b_ph_shwr_EtCone20_ED_corrected;   //!
   TBranch        *b_ph_shwr_EtCone30_ED_corrected;   //!
   TBranch        *b_ph_shwr_EtCone40_ED_corrected;   //!
   TBranch        *b_ph_shwr_EtCone40_ED0_corrected;   //!
   TBranch        *b_ph_shwr_EtCone40_ED1_corrected;   //!
   TBranch        *b_ph_shwr_EtCone40_ED2_corrected;   //!
   TBranch        *b_ph_shwr_EtCone40_ED3_corrected;   //!
   TBranch        *b_ph_shwr_EtCone40_ED4_corrected;   //!
   TBranch        *b_ph_shwr_EtCone40_ED5_corrected;   //!
   TBranch        *b_ph_ED_median;   //!
   TBranch        *b_ph_ED_sigma;   //!
   TBranch        *b_ph_ED_Njets;   //!
   TBranch        *b_ph_transmin_etcone40;   //!
   TBranch        *b_ph_transmax_etcone40;   //!
   TBranch        *b_ph_transmin_etcone100;   //!
   TBranch        *b_ph_transmax_etcone100;   //!
   TBranch        *b_ph_transmin;   //!
   TBranch        *b_ph_transmax;   //!
   TBranch        *b_ph_transmin_loweta;   //!
   TBranch        *b_ph_transmax_loweta;   //!
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
   TBranch        *b_ph_isG4part;   //!
   TBranch        *b_ph_motherPDG;   //!
   TBranch        *b_ph_isBrem_MC;   //!
   TBranch        *b_ph_isFromHardProc_MC;   //!
   TBranch        *b_ph_isPhotonFromHardProc_MC;   //!
   TBranch        *b_ph_imatchParton_MC;   //!
   TBranch        *b_ph_partonIsolation15_UE_MC;   //!
   TBranch        *b_ph_partonIsolation20_UE_MC;   //!
   TBranch        *b_ph_partonIsolation25_UE_MC;   //!
   TBranch        *b_ph_partonIsolation30_UE_MC;   //!
   TBranch        *b_ph_partonIsolation35_UE_MC;   //!
   TBranch        *b_ph_partonIsolation40_UE_MC;   //!
   TBranch        *b_ph_partonIsolation45_UE_MC;   //!
   TBranch        *b_ph_partonIsolation60_UE_MC;   //!
   TBranch        *b_ph_partonIsolation15_MC;   //!
   TBranch        *b_ph_partonIsolation20_MC;   //!
   TBranch        *b_ph_partonIsolation25_MC;   //!
   TBranch        *b_ph_partonIsolation30_MC;   //!
   TBranch        *b_ph_partonIsolation35_MC;   //!
   TBranch        *b_ph_partonIsolation40_MC;   //!
   TBranch        *b_ph_partonIsolation45_MC;   //!
   TBranch        *b_ph_partonIsolation60_MC;   //!
   TBranch        *b_ph_particleIsolation40_MC;   //!
   TBranch        *b_ph_particleIsolation40_ED_corrected_MC;   //!
   TBranch        *b_ph_particleIsolation40_ED_corrected_reco_MC;   //!
   TBranch        *b_ph_shwr_EtCone10;   //!
   TBranch        *b_ph_shwr_EtCone15;   //!
   TBranch        *b_ph_shwr_EtCone25;   //!
   TBranch        *b_ph_shwr_EtCone35;   //!
   TBranch        *b_ph_shwr_EtCone10_corrected;   //!
   TBranch        *b_ph_shwr_EtCone15_corrected;   //!
   TBranch        *b_ph_shwr_EtCone25_corrected;   //!
   TBranch        *b_ph_shwr_EtCone35_corrected;   //!
   TBranch        *b_ph_shwr_EtCone10_pt_corrected;   //!
   TBranch        *b_ph_shwr_EtCone15_pt_corrected;   //!
   TBranch        *b_ph_shwr_EtCone25_pt_corrected;   //!
   TBranch        *b_ph_shwr_EtCone35_pt_corrected;   //!
   TBranch        *b_ph_shwr_EtCone10_ED_corrected;   //!
   TBranch        *b_ph_shwr_EtCone15_ED_corrected;   //!
   TBranch        *b_ph_shwr_EtCone25_ED_corrected;   //!
   TBranch        *b_ph_shwr_EtCone35_ED_corrected;   //!
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
   TBranch        *b_trk_charge;   //!
   TBranch        *b_trk_d0;   //!
   TBranch        *b_trk_x0;   //!
   TBranch        *b_trk_y0;   //!
   TBranch        *b_trk_z0;   //!
   TBranch        *b_trk_nhits_b;   //!
   TBranch        *b_trk_nhits_p;   //!
   TBranch        *b_trk_ndead_p;   //!
   TBranch        *b_trk_nhits_s;   //!
   TBranch        *b_trk_ndead_s;   //!
   TBranch        *b_trk_nhits_trt;   //!
   TBranch        *b_trk_nhits_trth;   //!
   TBranch        *b_trk_chiSquared;   //!
   TBranch        *b_trk_numberDoF;   //!
   TBranch        *b_trk_gConv;   //!
   TBranch        *b_trk_conv;   //!
   TBranch        *b_trk_calo_eta;   //!
   TBranch        *b_trk_calo_phi;   //!
   TBranch        *b_trk_calo_r;   //!
   TBranch        *b_jet_eta;   //!
   TBranch        *b_jet_phi;   //!
   TBranch        *b_jet_pt;   //!
   TBranch        *b_jet_E;   //!
   TBranch        *b_jet_selected_1j;   //!
   TBranch        *b_jet_selected_2j;   //!
   TBranch        *b_jet_n90cells;   //!
   TBranch        *b_jet_n90constituents;   //!
   TBranch        *b_jet_hecf;   //!
   TBranch        *b_jet_tileGap3F;   //!
   TBranch        *b_jet_fcor;   //!
   TBranch        *b_jet_corr_cell;   //!
   TBranch        *b_jet_corr_dotx;   //!
   TBranch        *b_jet_samplingMax;   //!
   TBranch        *b_jet_samplingMaxId;   //!
   TBranch        *b_jet_emFraction;   //!
   TBranch        *b_jet_pt_emScale;   //!
   TBranch        *b_jet_eta_emScale;   //!
   TBranch        *b_jet_phi_emScale;   //!
   TBranch        *b_jet_e_emScale;   //!
   TBranch        *b_jet_time;   //!
   TBranch        *b_jet_quality;   //!
   TBranch        *b_jet_isGood;   //!
   TBranch        *b_jet_isBad;   //!
   TBranch        *b_jet_isUgly;   //!
   TBranch        *b_jet_EMJES;   //!
   TBranch        *b_jet_imatchTrueJet_MC;   //!
   TBranch        *b_jet_eta_MC;   //!
   TBranch        *b_jet_phi_MC;   //!
   TBranch        *b_jet_pt_MC;   //!
   TBranch        *b_jet_E_MC;   //!
   TBranch        *b_el_author;   //!
   TBranch        *b_el_isGoodOQbits;   //!
   TBranch        *b_el_isGoodOQphoton;   //!
   TBranch        *b_el_isGoodOQelectron;   //!
   TBranch        *b_test_el_isGoodOQbits_beforeTool;   //!
   TBranch        *b_test_el_isGoodOQphoton_beforeTool;   //!
   TBranch        *b_test_el_isGoodOQelectron_beforeTool;   //!
   TBranch        *b_el_eta;   //!
   TBranch        *b_el_phi;   //!
   TBranch        *b_el_pt;   //!
   TBranch        *b_el_E;   //!
   TBranch        *b_el_etaS0;   //!
   TBranch        *b_el_etaS1;   //!
   TBranch        *b_el_etaS2;   //!
   TBranch        *b_el_ES0_real;   //!
   TBranch        *b_el_ES1_real;   //!
   TBranch        *b_el_ES2_real;   //!
   TBranch        *b_el_ES3_real;   //!
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
   TBranch        *b_el_pt_trk;   //!
   TBranch        *b_el_eta_trk;   //!
   TBranch        *b_el_d0;   //!
   TBranch        *b_el_z0;   //!
   TBranch        *b_el_theta;   //!
   TBranch        *b_el_phi_trk;   //!
   TBranch        *b_el_qOverP;   //!
   TBranch        *b_el_nBHits;   //!
   TBranch        *b_el_nPixelHits;   //!
   TBranch        *b_el_nSCTHits;   //!
   TBranch        *b_el_nTRTHits;   //!
   TBranch        *b_el_iPV;   //!
   TBranch        *b_el_isEM;   //!
   TBranch        *b_el_isElectronLoose;   //!
   TBranch        *b_el_isElectronMedium;   //!
   TBranch        *b_el_isElectronTight;   //!
   TBranch        *b_el_isRobustLoose;   //!
   TBranch        *b_el_isRobustMedium;   //!
   TBranch        *b_el_isRobustTight;   //!
   TBranch        *b_el_isRobusterTight;   //!
   TBranch        *b_el_expectHitInBLayer;   //!
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
   TBranch        *b_el_ambiguityResult;   //!
   TBranch        *b_el_itune1;   //!
   TBranch        *b_el_itune2;   //!
   TBranch        *b_el_itune3;   //!
   TBranch        *b_el_logLikelihood;   //!
   TBranch        *b_el_PtIsolationCone;   //!
   TBranch        *b_el_trackIsol;   //!
   TBranch        *b_el_PtIsolationCone_woIC;   //!
   TBranch        *b_el_trackIsol_woIC;   //!
   TBranch        *b_el_dR_isol;   //!
   TBranch        *b_el_selected_etmiss;   //!
   TBranch        *b_el_selected_etlep;   //!
   TBranch        *b_el_shwr_EtCone20;   //!
   TBranch        *b_el_shwr_EtCone30;   //!
   TBranch        *b_el_shwr_EtCone40;   //!
   TBranch        *b_el_shwr_EtCone45;   //!
   TBranch        *b_el_shwr_EtCone20_corrected;   //!
   TBranch        *b_el_shwr_EtCone30_corrected;   //!
   TBranch        *b_el_shwr_EtCone40_corrected;   //!
   TBranch        *b_el_shwr_EtCone20_pt_corrected;   //!
   TBranch        *b_el_shwr_EtCone30_pt_corrected;   //!
   TBranch        *b_el_shwr_EtCone40_pt_corrected;   //!
   TBranch        *b_el_shwr_EtCone20_ED_corrected;   //!
   TBranch        *b_el_shwr_EtCone30_ED_corrected;   //!
   TBranch        *b_el_shwr_EtCone40_ED_corrected;   //!
   TBranch        *b_el_ED_median;   //!
   TBranch        *b_el_ED_sigma;   //!
   TBranch        *b_el_ED_Njets;   //!
   TBranch        *b_el_shwr_EtRing1030sig2;   //!
   TBranch        *b_el_shwr_EtRing1030sig3;   //!
   TBranch        *b_el_shwr_EtRing1030sig4;   //!
   TBranch        *b_el_matchMC;   //!
   TBranch        *b_el_eta_MC;   //!
   TBranch        *b_el_phi_MC;   //!
   TBranch        *b_el_pt_MC;   //!
   TBranch        *b_el_E_MC;   //!
   TBranch        *b_el_PDG;   //!
   TBranch        *b_el_motherPDG;   //!
   TBranch        *b_el_as_unconv_ph_isEM;   //!
   TBranch        *b_el_as_unconv_ph_isPhotonLoose;   //!
   TBranch        *b_el_as_unconv_ph_isPhotonTight;   //!
   TBranch        *b_el_as_unconv_ph_E;   //!
   TBranch        *b_el_as_unconv_ph_eta_clus;   //!
   TBranch        *b_el_as_unconv_ph_phi_clus;   //!
   TBranch        *b_el_as_unconv_ph_pt_clus;   //!
   TBranch        *b_el_as_unconv_ph_etaS1;   //!
   TBranch        *b_el_as_unconv_ph_etaS2;   //!
   TBranch        *b_el_as_conv_ph_isEM;   //!
   TBranch        *b_el_as_conv_ph_isPhotonLoose;   //!
   TBranch        *b_el_as_conv_ph_isPhotonTight;   //!
   TBranch        *b_el_as_conv_ph_E;   //!
   TBranch        *b_el_as_conv_ph_eta_clus;   //!
   TBranch        *b_el_as_conv_ph_phi_clus;   //!
   TBranch        *b_el_as_conv_ph_pt_clus;   //!
   TBranch        *b_el_as_conv_ph_etaS1;   //!
   TBranch        *b_el_as_conv_ph_etaS2;   //!
   TBranch        *b_el_as_conv_ph_shwr_Ethad1;   //!
   TBranch        *b_el_as_conv_ph_shwr_Ethad;   //!
   TBranch        *b_el_as_conv_ph_shwr_E277;   //!
   TBranch        *b_el_as_conv_ph_shwr_E237;   //!
   TBranch        *b_el_as_conv_ph_shwr_E233;   //!
   TBranch        *b_el_as_conv_ph_shwr_weta2;   //!
   TBranch        *b_el_as_conv_ph_shwr_f1;   //!
   TBranch        *b_el_as_conv_ph_shwr_Emax2;   //!
   TBranch        *b_el_as_conv_ph_shwr_Emax1;   //!
   TBranch        *b_el_as_conv_ph_shwr_Emin;   //!
   TBranch        *b_el_as_conv_ph_shwr_fracm;   //!
   TBranch        *b_el_as_conv_ph_shwr_wtot;   //!
   TBranch        *b_el_as_conv_ph_shwr_w1;   //!
   TBranch        *b_el_as_unconv_ph_shwr_Ethad1;   //!
   TBranch        *b_el_as_unconv_ph_shwr_Ethad;   //!
   TBranch        *b_el_as_unconv_ph_shwr_E277;   //!
   TBranch        *b_el_as_unconv_ph_shwr_E237;   //!
   TBranch        *b_el_as_unconv_ph_shwr_E233;   //!
   TBranch        *b_el_as_unconv_ph_shwr_weta2;   //!
   TBranch        *b_el_as_unconv_ph_shwr_f1;   //!
   TBranch        *b_el_as_unconv_ph_shwr_Emax2;   //!
   TBranch        *b_el_as_unconv_ph_shwr_Emax1;   //!
   TBranch        *b_el_as_unconv_ph_shwr_Emin;   //!
   TBranch        *b_el_as_unconv_ph_shwr_fracm;   //!
   TBranch        *b_el_as_unconv_ph_shwr_wtot;   //!
   TBranch        *b_el_as_unconv_ph_shwr_w1;   //!
   TBranch        *b_el_L1_matchPass;   //!
   TBranch        *b_el_L2_matchPass;   //!
   TBranch        *b_el_EF_matchPass;   //!
   TBranch        *b_el_L1_bestMatchDeltaR;   //!
   TBranch        *b_el_L2_bestMatchDeltaR;   //!
   TBranch        *b_el_EF_bestMatchDeltaR;   //!
   TBranch        *b_el_L1_matchPass_ph;   //!
   TBranch        *b_el_L2_matchPass_ph;   //!
   TBranch        *b_el_EF_matchPass_ph;   //!
   TBranch        *b_el_L1_bestMatchDeltaR_ph;   //!
   TBranch        *b_el_L2_bestMatchDeltaR_ph;   //!
   TBranch        *b_el_EF_bestMatchDeltaR_ph;   //!
   TBranch        *b_el_L1_et;   //!
   TBranch        *b_el_L1_eta;   //!
   TBranch        *b_el_L1_phi;   //!
   TBranch        *b_el_L1_EMIsol;   //!
   TBranch        *b_el_L1_HadIsol;   //!
   TBranch        *b_el_L1_HadCore;   //!
   TBranch        *b_el_L2_e;   //!
   TBranch        *b_el_L2_pt;   //!
   TBranch        *b_el_L2_eta;   //!
   TBranch        *b_el_L2_phi;   //!
   TBranch        *b_el_L2_cl_e;   //!
   TBranch        *b_el_L2_cl_et;   //!
   TBranch        *b_el_L2_cl_eta;   //!
   TBranch        *b_el_L2_cl_phi;   //!
   TBranch        *b_el_L2_cl_e237;   //!
   TBranch        *b_el_L2_cl_e277;   //!
   TBranch        *b_el_L2_cl_fracs1;   //!
   TBranch        *b_el_L2_cl_weta2;   //!
   TBranch        *b_el_L2_cl_ehad1;   //!
   TBranch        *b_el_L2_cl_emaxs1;   //!
   TBranch        *b_el_L2_cl_e2tsts1;   //!
   TBranch        *b_el_L2_trkCl_etOverPt;   //!
   TBranch        *b_el_L2_trkCl_dEta;   //!
   TBranch        *b_el_L2_trkCl_dPhi;   //!
   TBranch        *b_el_L2_trk_pt;   //!
   TBranch        *b_el_L2_trk_errPt;   //!
   TBranch        *b_el_L2_trk_etaAtCalo;   //!
   TBranch        *b_el_L2_trk_errEta;   //!
   TBranch        *b_el_L2_trk_phiAtCalo;   //!
   TBranch        *b_el_L2_trk_errPhi;   //!
   TBranch        *b_el_L2_trk_charge;   //!
   TBranch        *b_el_L2_trk_algo;   //!
   TBranch        *b_el_L2_trk_zvtx;   //!
   TBranch        *b_el_L2_trk_errZvtx;   //!
   TBranch        *b_el_EF_e;   //!
   TBranch        *b_el_EF_et;   //!
   TBranch        *b_el_EF_eta;   //!
   TBranch        *b_el_EF_phi;   //!
   TBranch        *b_el_EF_author;   //!
   TBranch        *b_el_EF_IsEM;   //!
   TBranch        *b_el_EF_ethad;   //!
   TBranch        *b_el_EF_ethad1;   //!
   TBranch        *b_el_EF_e233;   //!
   TBranch        *b_el_EF_e237;   //!
   TBranch        *b_el_EF_e277;   //!
   TBranch        *b_el_EF_emaxs1;   //!
   TBranch        *b_el_EF_e2tsts1;   //!
   TBranch        *b_el_EF_fracs1;   //!
   TBranch        *b_el_EF_weta1;   //!
   TBranch        *b_el_EF_weta2;   //!
   TBranch        *b_el_EF_wtots1;   //!
   TBranch        *b_el_EF_f1;   //!
   TBranch        *b_el_EF_emins1;   //!
   TBranch        *b_el_EF_etcone20;   //!
   TBranch        *b_el_EF_dEta;   //!
   TBranch        *b_el_EF_dPhi;   //!
   TBranch        *b_el_EF_etOverPt;   //!
   TBranch        *b_el_EF_nBL;   //!
   TBranch        *b_el_EF_nPix;   //!
   TBranch        *b_el_EF_nSCT;   //!
   TBranch        *b_el_EF_nTRThigh;   //!
   TBranch        *b_el_EF_nTRThighOut;   //!
   TBranch        *b_el_EF_nTRT;   //!
   TBranch        *b_el_EF_nTRTOut;   //!
   TBranch        *b_el_EF_trkPt;   //!
   TBranch        *b_el_EF_trkD0;   //!
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
   TBranch        *b_mEt_topo_ex;   //!
   TBranch        *b_mEt_topo_ey;   //!
   TBranch        *b_mEt_topo_et;   //!
   TBranch        *b_mEt_topo_sumet;   //!
   TBranch        *b_mEt_Topo_central_ex;   //!
   TBranch        *b_mEt_Topo_central_ey;   //!
   TBranch        *b_mEt_Topo_central_sumet;   //!
   TBranch        *b_mEt_Topo_endcap_ex;   //!
   TBranch        *b_mEt_Topo_endcap_ey;   //!
   TBranch        *b_mEt_Topo_endcap_sumet;   //!
   TBranch        *b_mEt_Topo_forward_ex;   //!
   TBranch        *b_mEt_Topo_forward_ey;   //!
   TBranch        *b_mEt_Topo_forward_sumet;   //!
   TBranch        *b_mEt_Topo_PEMB_ex;   //!
   TBranch        *b_mEt_Topo_PEMB_ey;   //!
   TBranch        *b_mEt_Topo_PEMB_ncell;   //!
   TBranch        *b_mEt_Topo_PEMB_sumet;   //!
   TBranch        *b_mEt_Topo_EMB_ex;   //!
   TBranch        *b_mEt_Topo_EMB_ey;   //!
   TBranch        *b_mEt_Topo_EMB_ncell;   //!
   TBranch        *b_mEt_Topo_EMB_sumet;   //!
   TBranch        *b_mEt_Topo_PEMEC_ex;   //!
   TBranch        *b_mEt_Topo_PEMEC_ey;   //!
   TBranch        *b_mEt_Topo_PEMEC_ncell;   //!
   TBranch        *b_mEt_Topo_PEMEC_sumet;   //!
   TBranch        *b_mEt_Topo_EME_ex;   //!
   TBranch        *b_mEt_Topo_EME_ey;   //!
   TBranch        *b_mEt_Topo_EME_ncell;   //!
   TBranch        *b_mEt_Topo_EME_sumet;   //!
   TBranch        *b_mEt_Topo_TILE_ex;   //!
   TBranch        *b_mEt_Topo_TILE_ey;   //!
   TBranch        *b_mEt_Topo_TILE_ncell;   //!
   TBranch        *b_mEt_Topo_TILE_sumet;   //!
   TBranch        *b_mEt_Topo_HEC_ex;   //!
   TBranch        *b_mEt_Topo_HEC_ey;   //!
   TBranch        *b_mEt_Topo_HEC_ncell;   //!
   TBranch        *b_mEt_Topo_HEC_sumet;   //!
   TBranch        *b_mEt_Topo_FCAL_ex;   //!
   TBranch        *b_mEt_Topo_FCAL_ey;   //!
   TBranch        *b_mEt_Topo_FCAL_ncell;   //!
   TBranch        *b_mEt_Topo_FCAL_sumet;   //!
   TBranch        *b_mEt_LocHadtopo_ex;   //!
   TBranch        *b_mEt_LocHadtopo_ey;   //!
   TBranch        *b_mEt_LocHadtopo_sumet;   //!
   TBranch        *b_mEt_LocHadtopo_et;   //!
   TBranch        *b_mEt_LocHadtopo_central_ex;   //!
   TBranch        *b_mEt_LocHadtopo_central_ey;   //!
   TBranch        *b_mEt_LocHadtopo_central_sumet;   //!
   TBranch        *b_mEt_LocHadtopo_endcap_ex;   //!
   TBranch        *b_mEt_LocHadtopo_endcap_ey;   //!
   TBranch        *b_mEt_LocHadtopo_endcap_sumet;   //!
   TBranch        *b_mEt_LocHadtopo_forward_ex;   //!
   TBranch        *b_mEt_LocHadtopo_forward_ey;   //!
   TBranch        *b_mEt_LocHadtopo_forward_sumet;   //!
   TBranch        *b_mEt_LocHadtopo_PEMB_ex;   //!
   TBranch        *b_mEt_LocHadtopo_PEMB_ey;   //!
   TBranch        *b_mEt_LocHadtopo_PEMB_ncell;   //!
   TBranch        *b_mEt_LocHadtopo_PEMB_sumet;   //!
   TBranch        *b_mEt_LocHadtopo_EMB_ex;   //!
   TBranch        *b_mEt_LocHadtopo_EMB_ey;   //!
   TBranch        *b_mEt_LocHadtopo_EMB_ncell;   //!
   TBranch        *b_mEt_LocHadtopo_EMB_sumet;   //!
   TBranch        *b_mEt_LocHadtopo_PEMEC_ex;   //!
   TBranch        *b_mEt_LocHadtopo_PEMEC_ey;   //!
   TBranch        *b_mEt_LocHadtopo_PEMEC_ncell;   //!
   TBranch        *b_mEt_LocHadtopo_PEMEC_sumet;   //!
   TBranch        *b_mEt_LocHadtopo_EME_ex;   //!
   TBranch        *b_mEt_LocHadtopo_EME_ey;   //!
   TBranch        *b_mEt_LocHadtopo_EME_ncell;   //!
   TBranch        *b_mEt_LocHadtopo_EME_sumet;   //!
   TBranch        *b_mEt_LocHadtopo_TILE_ex;   //!
   TBranch        *b_mEt_LocHadtopo_TILE_ey;   //!
   TBranch        *b_mEt_LocHadtopo_TILE_ncell;   //!
   TBranch        *b_mEt_LocHadtopo_TILE_sumet;   //!
   TBranch        *b_mEt_LocHadtopo_HEC_ex;   //!
   TBranch        *b_mEt_LocHadtopo_HEC_ey;   //!
   TBranch        *b_mEt_LocHadtopo_HEC_ncell;   //!
   TBranch        *b_mEt_LocHadtopo_HEC_sumet;   //!
   TBranch        *b_mEt_LocHadtopo_FCAL_ex;   //!
   TBranch        *b_mEt_LocHadtopo_FCAL_ey;   //!
   TBranch        *b_mEt_LocHadtopo_FCAL_ncell;   //!
   TBranch        *b_mEt_LocHadtopo_FCAL_sumet;   //!
   TBranch        *b_mEt_MuonBoy_Track_ex;   //!
   TBranch        *b_mEt_MuonBoy_Track_ey;   //!
   TBranch        *b_mEt_MuonBoy_Track_sumet;   //!
   TBranch        *b_mEt_MuonBoy_ex;   //!
   TBranch        *b_mEt_MuonBoy_ey;   //!
   TBranch        *b_mEt_MuonBoy_sumet;   //!
   TBranch        *b_mEt_MuID_ex;   //!
   TBranch        *b_mEt_MuID_ey;   //!
   TBranch        *b_mEt_MuID_sumet;   //!
   TBranch        *b_mEt_Cryo_ex;   //!
   TBranch        *b_mEt_Cryo_ey;   //!
   TBranch        *b_mEt_Cryo_sumet;   //!
   TBranch        *b_mEt_RefEle_ex;   //!
   TBranch        *b_mEt_RefEle_ey;   //!
   TBranch        *b_mEt_RefEle_sumet;   //!
   TBranch        *b_mEt_RefGamma_ex;   //!
   TBranch        *b_mEt_RefGamma_ey;   //!
   TBranch        *b_mEt_RefGamma_sumet;   //!
   TBranch        *b_mEt_RefTau_ex;   //!
   TBranch        *b_mEt_RefTau_ey;   //!
   TBranch        *b_mEt_RefTau_sumet;   //!
   TBranch        *b_mEt_RefJet_ex;   //!
   TBranch        *b_mEt_RefJet_ey;   //!
   TBranch        *b_mEt_RefJet_sumet;   //!
   TBranch        *b_mEt_RefMuon_ex;   //!
   TBranch        *b_mEt_RefMuon_ey;   //!
   TBranch        *b_mEt_RefMuon_sumet;   //!
   TBranch        *b_mEt_RefMuon_Track_ex;   //!
   TBranch        *b_mEt_RefMuon_Track_ey;   //!
   TBranch        *b_mEt_RefMuon_Track_sumet;   //!
   TBranch        *b_mEt_CellOut_ex;   //!
   TBranch        *b_mEt_CellOut_ey;   //!
   TBranch        *b_mEt_CellOut_sumet;   //!
   TBranch        *b_mEt_RefFinal_ex;   //!
   TBranch        *b_mEt_RefFinal_ey;   //!
   TBranch        *b_mEt_RefFinal_sumet;   //!
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
   TBranch        *b_el_CaloPointing_zvertex;   //!
   TBranch        *b_el_CaloPointing_sigma_zvertex;   //!
   TBranch        *b_el_CaloPointing_eta;   //!
   TBranch        *b_el_CaloPointing_sigma_eta;   //!
   TBranch        *b_ph_CaloPointing_zvertex;   //!
   TBranch        *b_ph_CaloPointing_sigma_zvertex;   //!
   TBranch        *b_ph_CaloPointing_eta;   //!
   TBranch        *b_ph_CaloPointing_sigma_eta;   //!
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
   TBranch        *b_test_ph_E_calopoint;   //!
   TBranch        *b_test_ph_eta_calopoint;   //!
   TBranch        *b_test_ph_phi_calopoint;   //!
   TBranch        *b_test_ph_pt_calopoint;   //!
   TBranch        *b_ph_particleType;   //!
   TBranch        *b_ph_particleOrig;   //!
   TBranch        *b_clus_SW_eta;   //!
   TBranch        *b_clus_SW_phi;   //!
   TBranch        *b_clus_SW_pt;   //!
   TBranch        *b_clus_SW_E;   //!
   TBranch        *b_clus_SW_size;   //!
   TBranch        *b_clus_SW_etasize;   //!
   TBranch        *b_clus_SW_phisize;   //!
   TBranch        *b_clus_SW_time;   //!
   TBranch        *b_clus_SW_eta_MC;   //!
   TBranch        *b_clus_SW_phi_MC;   //!
   TBranch        *b_clus_SW_pt_MC;   //!
   TBranch        *b_clus_SW_E_MC;   //!
   TBranch        *b_clus_SW_PDG;   //!
   TBranch        *b_clus_SW_motherPDG;   //!
   TBranch        *b_clus_EMtopo_eta;   //!
   TBranch        *b_clus_EMtopo_phi;   //!
   TBranch        *b_clus_EMtopo_pt;   //!
   TBranch        *b_clus_EMtopo_E;   //!
   TBranch        *b_clus_EMtopo_time;   //!
   TBranch        *b_clus_EMtopo_eta_MC;   //!
   TBranch        *b_clus_EMtopo_phi_MC;   //!
   TBranch        *b_clus_EMtopo_pt_MC;   //!
   TBranch        *b_clus_EMtopo_E_MC;   //!
   TBranch        *b_clus_EMtopo_PDG;   //!
   TBranch        *b_clus_EMtopo_motherPDG;   //!
   TBranch        *b_clus_EMtopo_iRecPhoton;   //!
   TBranch        *b_clus_EMtopo_deltaR_RecPhoton;   //!
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
   TBranch        *b_clus_topo420_E;   //!
   TBranch        *b_clus_lhtopo420_E;   //!
   TBranch        *b_clus_lhtopo420_phi;   //!
   TBranch        *b_clus_lhtopo420_eta;   //!
   TBranch        *b_UED_diphomass;   //!
   TBranch        *b_UED_diphoang;   //!

   PAUEffFF(TTree *tree=0);
   virtual ~PAUEffFF();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef PAUEffFF_cxx
PAUEffFF::PAUEffFF(TTree *tree)
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
      chain->Add("/tmp/tripiana/mc10_7TeV.108087.PythiaPhotonJet_Unbinned17.merge.NTUP_PROMPTPHOT.e574_s933_s946_r1834_r1700_p390_tid250933_00/NTUP_PROMPTPHOT.250933._000250.root.1/PAUReco");
      chain->Add("/tmp/tripiana/mc10_7TeV.108087.PythiaPhotonJet_Unbinned17.merge.NTUP_PROMPTPHOT.e574_s933_s946_r1834_r1700_p390_tid250933_00/NTUP_PROMPTPHOT.250933._000134.root.1/PAUReco");
      chain->Add("/tmp/tripiana/mc10_7TeV.108087.PythiaPhotonJet_Unbinned17.merge.NTUP_PROMPTPHOT.e574_s933_s946_r1834_r1700_p390_tid250933_00/NTUP_PROMPTPHOT.250933._000154.root.1/PAUReco");
      chain->Add("/tmp/tripiana/mc10_7TeV.108087.PythiaPhotonJet_Unbinned17.merge.NTUP_PROMPTPHOT.e574_s933_s946_r1834_r1700_p390_tid250933_00/NTUP_PROMPTPHOT.250933._000250.root.1/PAUReco");
      chain->Add("/tmp/tripiana/mc10_7TeV.108087.PythiaPhotonJet_Unbinned17.merge.NTUP_PROMPTPHOT.e574_s933_s946_r1834_r1700_p390_tid250933_00/NTUP_PROMPTPHOT.250933._000380.root.1/PAUReco");
      chain->Add("/tmp/tripiana/mc10_7TeV.108087.PythiaPhotonJet_Unbinned17.merge.NTUP_PROMPTPHOT.e574_s933_s946_r1834_r1700_p390_tid250933_00/NTUP_PROMPTPHOT.250933._000458.root.1/PAUReco");
      chain->Add("/tmp/tripiana/mc10_7TeV.108087.PythiaPhotonJet_Unbinned17.merge.NTUP_PROMPTPHOT.e574_s933_s946_r1834_r1700_p390_tid250933_00/NTUP_PROMPTPHOT.250933._000466.root.1/PAUReco");
      //      chain->Add("root://castoratlas//castor/cern.ch/user/k/koffas/mc10_7TeV/NewBasicSkimPAU/DP17_16033_PAU-00-02-78.root/PAUReco");
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

PAUEffFF::~PAUEffFF()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t PAUEffFF::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t PAUEffFF::LoadTree(Long64_t entry)
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

void PAUEffFF::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   prevBunchStates = 0;
   energyDensityMedian = 0;
   energyDensitySigma = 0;
   energyDensityNjets = 0;
   energyDensityEtaMin = 0;
   ph_L1_matchPass = 0;
   ph_L2_matchPass = 0;
   ph_EF_matchPass = 0;
   ph_L1_bestMatchDeltaR = 0;
   ph_L2_bestMatchDeltaR = 0;
   ph_EF_bestMatchDeltaR = 0;
   TriggersRunL1Seed_ph = 0;
   TriggersRunL1Seed_el = 0;
   TriggersRun_ph = 0;
   TriggersRun_el = 0;
   PassL1_ph = 0;
   PrescaleL1_ph = 0;
   PassL2_ph = 0;
   PrescaleL2_ph = 0;
   PassEF_ph = 0;
   PrescaleEF_ph = 0;
   PassL1_el = 0;
   PrescaleL1_el = 0;
   PassL2_el = 0;
   PrescaleL2_el = 0;
   PassEF_el = 0;
   PrescaleEF_el = 0;
   PassEF_el_passedthrough = 0;
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
   PV_ID_xvertex = 0;
   PV_ID_yvertex = 0;
   PV_ID_zvertex = 0;
   PV_ntracks = 0;
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
   truth_ph_partonIsolation15_UE = 0;
   truth_ph_partonIsolation20_UE = 0;
   truth_ph_partonIsolation25_UE = 0;
   truth_ph_partonIsolation30_UE = 0;
   truth_ph_partonIsolation35_UE = 0;
   truth_ph_partonIsolation40_UE = 0;
   truth_ph_partonIsolation45_UE = 0;
   truth_ph_partonIsolation60_UE = 0;
   truth_ph_partonIsolation15 = 0;
   truth_ph_partonIsolation20 = 0;
   truth_ph_partonIsolation25 = 0;
   truth_ph_partonIsolation30 = 0;
   truth_ph_partonIsolation35 = 0;
   truth_ph_partonIsolation40 = 0;
   truth_ph_partonIsolation45 = 0;
   truth_ph_partonIsolation60 = 0;
   truth_ph_particleIsolation40 = 0;
   truth_ph_particleIsolation40_ED_corrected = 0;
   truth_ph_particleIsolation40_ED_corrected_reco = 0;
   truth_jet_eta = 0;
   truth_jet_phi = 0;
   truth_jet_pt = 0;
   truth_jet_E = 0;
   truth_jet_parton = 0;
   truth_jet_iRecPhoton = 0;
   truth_jet_deltaR_RecPhoton = 0;
   ph_indexElectronSameAthenaBarcode = 0;
   ph_author = 0;
   ph_isGoodOQbits = 0;
   ph_isGoodOQphoton = 0;
   ph_isGoodOQelectron = 0;
   test_ph_isGoodOQbits_beforeTool = 0;
   test_ph_isGoodOQphoton_beforeTool = 0;
   test_ph_isGoodOQelectron_beforeTool = 0;
   ph_eta = 0;
   ph_etaS0 = 0;
   ph_etaS1 = 0;
   ph_etaS2 = 0;
   ph_phi = 0;
   ph_pt = 0;
   ph_Tclus = 0;
   ph_E = 0;
   ph_ES0_real = 0;
   ph_ES1_real = 0;
   ph_ES2_real = 0;
   ph_ES3_real = 0;
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
   ph_convTrk1_chi2 = 0;
   ph_convTrk2_chi2 = 0;
   ph_conv_chi2 = 0;
   ph_conv_dcottheta = 0;
   ph_conv_dphi = 0;
   ph_conv_dist = 0;
   ph_conv_DR1R2 = 0;
   ph_convTrk1_expectHitInBLayer = 0;
   ph_convTrk1_numBLHits = 0;
   ph_convTrk1_numPixelHits = 0;
   ph_convTrk1_numSCTHits = 0;
   ph_convTrk1_numTRTHits = 0;
   ph_convTrk1_numTRTOutliers = 0;
   ph_convTrk1_numTRTHTHits = 0;
   ph_convTrk1_numTRTHTOutliers = 0;
   ph_convTrk2_expectHitInBLayer = 0;
   ph_convTrk2_numBLHits = 0;
   ph_convTrk2_numPixelHits = 0;
   ph_convTrk2_numSCTHits = 0;
   ph_convTrk2_numTRTHits = 0;
   ph_convTrk2_numTRTOutliers = 0;
   ph_convTrk2_numTRTHTHits = 0;
   ph_convTrk2_numTRTHTOutliers = 0;
   ph_convTrk1_trtPID = 0;
   ph_convTrk2_trtPID = 0;
   ph_convTrk1_trtPIDHT = 0;
   ph_convTrk2_trtPIDHT = 0;
   ph_convTrk1_trtPIDToT = 0;
   ph_convTrk2_trtPIDToT = 0;
   ph_convTrk1_ToTvar = 0;
   ph_convTrk2_ToTvar = 0;
   ph_convTrk1_trtRatioHT = 0;
   ph_convTrk2_trtRatioHT = 0;
   ph_convTrk1_charge = 0;
   ph_convTrk2_charge = 0;
   ph_convTrk1_DeltaEta_track_calo = 0;
   ph_convTrk2_DeltaEta_track_calo = 0;
   ph_convTrk1_DeltaPhi_track_calo = 0;
   ph_convTrk2_DeltaPhi_track_calo = 0;
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
   ph_parton_pt_max_MC = 0;
   ph_parton_eta_MC = 0;
   ph_parton_phi_MC = 0;
   ph_parton_barcode_MC = 0;
   ph_parton_pdg_MC = 0;
   ph_isEM = 0;
   ph_isPhotonLoose = 0;
   ph_isPhotonTight = 0;
   ph_ambiguityResult = 0;
   ph_itune1 = 0;
   ph_itune2 = 0;
   ph_itune3 = 0;
   ph_logLikelihood = 0;
   ph_PtIsolationCone = 0;
   ph_PtIsolationConePhAngle = 0;
   ph_trackIsol = 0;
   ph_PtIsolationCone_woIC = 0;
   ph_PtIsolationConePhAngle_woIC = 0;
   ph_trackIsol_woIC = 0;
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
   ph_etconoisedR04Sig2 = 0;
   ph_etconoisedR04Sig3 = 0;
   ph_shwr_EtCone20 = 0;
   ph_shwr_EtCone30 = 0;
   ph_shwr_EtCone40 = 0;
   ph_shwr_EtCone45 = 0;
   ph_shwr_EtCone20_corrected = 0;
   ph_shwr_EtCone30_corrected = 0;
   ph_shwr_EtCone40_corrected = 0;
   ph_shwr_EtCone20_pt_corrected = 0;
   ph_shwr_EtCone30_pt_corrected = 0;
   ph_shwr_EtCone40_pt_corrected = 0;
   ph_shwr_EtCone20_ED_corrected = 0;
   ph_shwr_EtCone30_ED_corrected = 0;
   ph_shwr_EtCone40_ED_corrected = 0;
   ph_shwr_EtCone40_ED0_corrected = 0;
   ph_shwr_EtCone40_ED1_corrected = 0;
   ph_shwr_EtCone40_ED2_corrected = 0;
   ph_shwr_EtCone40_ED3_corrected = 0;
   ph_shwr_EtCone40_ED4_corrected = 0;
   ph_shwr_EtCone40_ED5_corrected = 0;
   ph_ED_median = 0;
   ph_ED_sigma = 0;
   ph_ED_Njets = 0;
   ph_transmin_etcone40 = 0;
   ph_transmax_etcone40 = 0;
   ph_transmin_etcone100 = 0;
   ph_transmax_etcone100 = 0;
   ph_transmin = 0;
   ph_transmax = 0;
   ph_transmin_loweta = 0;
   ph_transmax_loweta = 0;
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
   ph_isG4part = 0;
   ph_motherPDG = 0;
   ph_isBrem_MC = 0;
   ph_isFromHardProc_MC = 0;
   ph_isPhotonFromHardProc_MC = 0;
   ph_imatchParton_MC = 0;
   ph_partonIsolation15_UE_MC = 0;
   ph_partonIsolation20_UE_MC = 0;
   ph_partonIsolation25_UE_MC = 0;
   ph_partonIsolation30_UE_MC = 0;
   ph_partonIsolation35_UE_MC = 0;
   ph_partonIsolation40_UE_MC = 0;
   ph_partonIsolation45_UE_MC = 0;
   ph_partonIsolation60_UE_MC = 0;
   ph_partonIsolation15_MC = 0;
   ph_partonIsolation20_MC = 0;
   ph_partonIsolation25_MC = 0;
   ph_partonIsolation30_MC = 0;
   ph_partonIsolation35_MC = 0;
   ph_partonIsolation40_MC = 0;
   ph_partonIsolation45_MC = 0;
   ph_partonIsolation60_MC = 0;
   ph_particleIsolation40_MC = 0;
   ph_particleIsolation40_ED_corrected_MC = 0;
   ph_particleIsolation40_ED_corrected_reco_MC = 0;
   ph_shwr_EtCone10 = 0;
   ph_shwr_EtCone15 = 0;
   ph_shwr_EtCone25 = 0;
   ph_shwr_EtCone35 = 0;
   ph_shwr_EtCone10_corrected = 0;
   ph_shwr_EtCone15_corrected = 0;
   ph_shwr_EtCone25_corrected = 0;
   ph_shwr_EtCone35_corrected = 0;
   ph_shwr_EtCone10_pt_corrected = 0;
   ph_shwr_EtCone15_pt_corrected = 0;
   ph_shwr_EtCone25_pt_corrected = 0;
   ph_shwr_EtCone35_pt_corrected = 0;
   ph_shwr_EtCone10_ED_corrected = 0;
   ph_shwr_EtCone15_ED_corrected = 0;
   ph_shwr_EtCone25_ED_corrected = 0;
   ph_shwr_EtCone35_ED_corrected = 0;
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
   trk_charge = 0;
   trk_d0 = 0;
   trk_x0 = 0;
   trk_y0 = 0;
   trk_z0 = 0;
   trk_nhits_b = 0;
   trk_nhits_p = 0;
   trk_ndead_p = 0;
   trk_nhits_s = 0;
   trk_ndead_s = 0;
   trk_nhits_trt = 0;
   trk_nhits_trth = 0;
   trk_chiSquared = 0;
   trk_numberDoF = 0;
   trk_gConv = 0;
   trk_conv = 0;
   trk_calo_eta = 0;
   trk_calo_phi = 0;
   trk_calo_r = 0;
   jet_eta = 0;
   jet_phi = 0;
   jet_pt = 0;
   jet_E = 0;
   jet_selected_1j = 0;
   jet_selected_2j = 0;
   jet_n90cells = 0;
   jet_n90constituents = 0;
   jet_hecf = 0;
   jet_tileGap3F = 0;
   jet_fcor = 0;
   jet_corr_cell = 0;
   jet_corr_dotx = 0;
   jet_samplingMax = 0;
   jet_samplingMaxId = 0;
   jet_emFraction = 0;
   jet_pt_emScale = 0;
   jet_eta_emScale = 0;
   jet_phi_emScale = 0;
   jet_e_emScale = 0;
   jet_time = 0;
   jet_quality = 0;
   jet_isGood = 0;
   jet_isBad = 0;
   jet_isUgly = 0;
   jet_EMJES = 0;
   jet_imatchTrueJet_MC = 0;
   jet_eta_MC = 0;
   jet_phi_MC = 0;
   jet_pt_MC = 0;
   jet_E_MC = 0;
   el_author = 0;
   el_isGoodOQbits = 0;
   el_isGoodOQphoton = 0;
   el_isGoodOQelectron = 0;
   test_el_isGoodOQbits_beforeTool = 0;
   test_el_isGoodOQphoton_beforeTool = 0;
   test_el_isGoodOQelectron_beforeTool = 0;
   el_eta = 0;
   el_phi = 0;
   el_pt = 0;
   el_E = 0;
   el_etaS0 = 0;
   el_etaS1 = 0;
   el_etaS2 = 0;
   el_ES0_real = 0;
   el_ES1_real = 0;
   el_ES2_real = 0;
   el_ES3_real = 0;
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
   el_pt_trk = 0;
   el_eta_trk = 0;
   el_d0 = 0;
   el_z0 = 0;
   el_theta = 0;
   el_phi_trk = 0;
   el_qOverP = 0;
   el_nBHits = 0;
   el_nPixelHits = 0;
   el_nSCTHits = 0;
   el_nTRTHits = 0;
   el_iPV = 0;
   el_isEM = 0;
   el_isElectronLoose = 0;
   el_isElectronMedium = 0;
   el_isElectronTight = 0;
   el_isRobustLoose = 0;
   el_isRobustMedium = 0;
   el_isRobustTight = 0;
   el_isRobusterTight = 0;
   el_expectHitInBLayer = 0;
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
   el_ambiguityResult = 0;
   el_itune1 = 0;
   el_itune2 = 0;
   el_itune3 = 0;
   el_logLikelihood = 0;
   el_PtIsolationCone = 0;
   el_trackIsol = 0;
   el_PtIsolationCone_woIC = 0;
   el_trackIsol_woIC = 0;
   el_dR_isol = 0;
   el_selected_etmiss = 0;
   el_selected_etlep = 0;
   el_shwr_EtCone20 = 0;
   el_shwr_EtCone30 = 0;
   el_shwr_EtCone40 = 0;
   el_shwr_EtCone45 = 0;
   el_shwr_EtCone20_corrected = 0;
   el_shwr_EtCone30_corrected = 0;
   el_shwr_EtCone40_corrected = 0;
   el_shwr_EtCone20_pt_corrected = 0;
   el_shwr_EtCone30_pt_corrected = 0;
   el_shwr_EtCone40_pt_corrected = 0;
   el_shwr_EtCone20_ED_corrected = 0;
   el_shwr_EtCone30_ED_corrected = 0;
   el_shwr_EtCone40_ED_corrected = 0;
   el_ED_median = 0;
   el_ED_sigma = 0;
   el_ED_Njets = 0;
   el_shwr_EtRing1030sig2 = 0;
   el_shwr_EtRing1030sig3 = 0;
   el_shwr_EtRing1030sig4 = 0;
   el_matchMC = 0;
   el_eta_MC = 0;
   el_phi_MC = 0;
   el_pt_MC = 0;
   el_E_MC = 0;
   el_PDG = 0;
   el_motherPDG = 0;
   el_as_unconv_ph_isEM = 0;
   el_as_unconv_ph_isPhotonLoose = 0;
   el_as_unconv_ph_isPhotonTight = 0;
   el_as_unconv_ph_E = 0;
   el_as_unconv_ph_eta_clus = 0;
   el_as_unconv_ph_phi_clus = 0;
   el_as_unconv_ph_pt_clus = 0;
   el_as_unconv_ph_etaS1 = 0;
   el_as_unconv_ph_etaS2 = 0;
   el_as_conv_ph_isEM = 0;
   el_as_conv_ph_isPhotonLoose = 0;
   el_as_conv_ph_isPhotonTight = 0;
   el_as_conv_ph_E = 0;
   el_as_conv_ph_eta_clus = 0;
   el_as_conv_ph_phi_clus = 0;
   el_as_conv_ph_pt_clus = 0;
   el_as_conv_ph_etaS1 = 0;
   el_as_conv_ph_etaS2 = 0;
   el_as_conv_ph_shwr_Ethad1 = 0;
   el_as_conv_ph_shwr_Ethad = 0;
   el_as_conv_ph_shwr_E277 = 0;
   el_as_conv_ph_shwr_E237 = 0;
   el_as_conv_ph_shwr_E233 = 0;
   el_as_conv_ph_shwr_weta2 = 0;
   el_as_conv_ph_shwr_f1 = 0;
   el_as_conv_ph_shwr_Emax2 = 0;
   el_as_conv_ph_shwr_Emax1 = 0;
   el_as_conv_ph_shwr_Emin = 0;
   el_as_conv_ph_shwr_fracm = 0;
   el_as_conv_ph_shwr_wtot = 0;
   el_as_conv_ph_shwr_w1 = 0;
   el_as_unconv_ph_shwr_Ethad1 = 0;
   el_as_unconv_ph_shwr_Ethad = 0;
   el_as_unconv_ph_shwr_E277 = 0;
   el_as_unconv_ph_shwr_E237 = 0;
   el_as_unconv_ph_shwr_E233 = 0;
   el_as_unconv_ph_shwr_weta2 = 0;
   el_as_unconv_ph_shwr_f1 = 0;
   el_as_unconv_ph_shwr_Emax2 = 0;
   el_as_unconv_ph_shwr_Emax1 = 0;
   el_as_unconv_ph_shwr_Emin = 0;
   el_as_unconv_ph_shwr_fracm = 0;
   el_as_unconv_ph_shwr_wtot = 0;
   el_as_unconv_ph_shwr_w1 = 0;
   el_L1_matchPass = 0;
   el_L2_matchPass = 0;
   el_EF_matchPass = 0;
   el_L1_bestMatchDeltaR = 0;
   el_L2_bestMatchDeltaR = 0;
   el_EF_bestMatchDeltaR = 0;
   el_L1_matchPass_ph = 0;
   el_L2_matchPass_ph = 0;
   el_EF_matchPass_ph = 0;
   el_L1_bestMatchDeltaR_ph = 0;
   el_L2_bestMatchDeltaR_ph = 0;
   el_EF_bestMatchDeltaR_ph = 0;
   el_L1_et = 0;
   el_L1_eta = 0;
   el_L1_phi = 0;
   el_L1_EMIsol = 0;
   el_L1_HadIsol = 0;
   el_L1_HadCore = 0;
   el_L2_e = 0;
   el_L2_pt = 0;
   el_L2_eta = 0;
   el_L2_phi = 0;
   el_L2_cl_e = 0;
   el_L2_cl_et = 0;
   el_L2_cl_eta = 0;
   el_L2_cl_phi = 0;
   el_L2_cl_e237 = 0;
   el_L2_cl_e277 = 0;
   el_L2_cl_fracs1 = 0;
   el_L2_cl_weta2 = 0;
   el_L2_cl_ehad1 = 0;
   el_L2_cl_emaxs1 = 0;
   el_L2_cl_e2tsts1 = 0;
   el_L2_trkCl_etOverPt = 0;
   el_L2_trkCl_dEta = 0;
   el_L2_trkCl_dPhi = 0;
   el_L2_trk_pt = 0;
   el_L2_trk_errPt = 0;
   el_L2_trk_etaAtCalo = 0;
   el_L2_trk_errEta = 0;
   el_L2_trk_phiAtCalo = 0;
   el_L2_trk_errPhi = 0;
   el_L2_trk_charge = 0;
   el_L2_trk_algo = 0;
   el_L2_trk_zvtx = 0;
   el_L2_trk_errZvtx = 0;
   el_EF_e = 0;
   el_EF_et = 0;
   el_EF_eta = 0;
   el_EF_phi = 0;
   el_EF_author = 0;
   el_EF_IsEM = 0;
   el_EF_ethad = 0;
   el_EF_ethad1 = 0;
   el_EF_e233 = 0;
   el_EF_e237 = 0;
   el_EF_e277 = 0;
   el_EF_emaxs1 = 0;
   el_EF_e2tsts1 = 0;
   el_EF_fracs1 = 0;
   el_EF_weta1 = 0;
   el_EF_weta2 = 0;
   el_EF_wtots1 = 0;
   el_EF_f1 = 0;
   el_EF_emins1 = 0;
   el_EF_etcone20 = 0;
   el_EF_dEta = 0;
   el_EF_dPhi = 0;
   el_EF_etOverPt = 0;
   el_EF_nBL = 0;
   el_EF_nPix = 0;
   el_EF_nSCT = 0;
   el_EF_nTRThigh = 0;
   el_EF_nTRThighOut = 0;
   el_EF_nTRT = 0;
   el_EF_nTRTOut = 0;
   el_EF_trkPt = 0;
   el_EF_trkD0 = 0;
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
   hgg_HPV_eta_zcommon = 0;
   hgg_HPV_eta_zcommon_primVtxLH = 0;
   ph_HPV_convTrk1_zvertex = 0;
   ph_HPV_convTrk2_zvertex = 0;
   ph_HPV_convTrk1_sigma_zvertex = 0;
   ph_HPV_convTrk2_sigma_zvertex = 0;
   el_CaloPointing_zvertex = 0;
   el_CaloPointing_sigma_zvertex = 0;
   el_CaloPointing_eta = 0;
   el_CaloPointing_sigma_eta = 0;
   ph_CaloPointing_zvertex = 0;
   ph_CaloPointing_sigma_zvertex = 0;
   ph_CaloPointing_eta = 0;
   ph_CaloPointing_sigma_eta = 0;
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
   test_ph_E_calopoint = 0;
   test_ph_eta_calopoint = 0;
   test_ph_phi_calopoint = 0;
   test_ph_pt_calopoint = 0;
   ph_particleType = 0;
   ph_particleOrig = 0;
   clus_SW_eta = 0;
   clus_SW_phi = 0;
   clus_SW_pt = 0;
   clus_SW_E = 0;
   clus_SW_size = 0;
   clus_SW_etasize = 0;
   clus_SW_phisize = 0;
   clus_SW_time = 0;
   clus_SW_eta_MC = 0;
   clus_SW_phi_MC = 0;
   clus_SW_pt_MC = 0;
   clus_SW_E_MC = 0;
   clus_SW_PDG = 0;
   clus_SW_motherPDG = 0;
   clus_EMtopo_eta = 0;
   clus_EMtopo_phi = 0;
   clus_EMtopo_pt = 0;
   clus_EMtopo_E = 0;
   clus_EMtopo_time = 0;
   clus_EMtopo_eta_MC = 0;
   clus_EMtopo_phi_MC = 0;
   clus_EMtopo_pt_MC = 0;
   clus_EMtopo_E_MC = 0;
   clus_EMtopo_PDG = 0;
   clus_EMtopo_motherPDG = 0;
   clus_EMtopo_iRecPhoton = 0;
   clus_EMtopo_deltaR_RecPhoton = 0;
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
   clus_topo420_E = 0;
   clus_lhtopo420_E = 0;
   clus_lhtopo420_phi = 0;
   clus_lhtopo420_eta = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Run", &Run, &b_Run);
   fChain->SetBranchAddress("Event", &Event, &b_Event);
   fChain->SetBranchAddress("LumiBlock", &LumiBlock, &b_LumiBlock);
   fChain->SetBranchAddress("BCID", &BCID, &b_BCID);
   fChain->SetBranchAddress("DeltaT_Calo", &DeltaT_Calo, &b_DeltaT_Calo);
   fChain->SetBranchAddress("DeltaT_MBTS", &DeltaT_MBTS, &b_DeltaT_MBTS);
   fChain->SetBranchAddress("isInBunchTrain", &isInBunchTrain, &b_isInBunchTrain);
   fChain->SetBranchAddress("isEmptyBunch", &isEmptyBunch, &b_isEmptyBunch);
   fChain->SetBranchAddress("nsFromTrainFront", &nsFromTrainFront, &b_nsFromTrainFront);
   fChain->SetBranchAddress("bunchesFromTrainFront", &bunchesFromTrainFront, &b_bunchesFromTrainFront);
   fChain->SetBranchAddress("prevBunchStates", &prevBunchStates, &b_prevBunchStates);
   fChain->SetBranchAddress("energyDensityMedian", &energyDensityMedian, &b_energyDensityMedian);
   fChain->SetBranchAddress("energyDensitySigma", &energyDensitySigma, &b_energyDensitySigma);
   fChain->SetBranchAddress("energyDensityNjets", &energyDensityNjets, &b_energyDensityNjets);
   fChain->SetBranchAddress("energyDensityEtaMin", &energyDensityEtaMin, &b_energyDensityEtaMin);
   fChain->SetBranchAddress("ph_L1_matchPass", &ph_L1_matchPass, &b_ph_L1_matchPass);
   fChain->SetBranchAddress("ph_L2_matchPass", &ph_L2_matchPass, &b_ph_L2_matchPass);
   fChain->SetBranchAddress("ph_EF_matchPass", &ph_EF_matchPass, &b_ph_EF_matchPass);
   fChain->SetBranchAddress("ph_L1_bestMatchDeltaR", &ph_L1_bestMatchDeltaR, &b_ph_L1_bestMatchDeltaR);
   fChain->SetBranchAddress("ph_L2_bestMatchDeltaR", &ph_L2_bestMatchDeltaR, &b_ph_L2_bestMatchDeltaR);
   fChain->SetBranchAddress("ph_EF_bestMatchDeltaR", &ph_EF_bestMatchDeltaR, &b_ph_EF_bestMatchDeltaR);
   fChain->SetBranchAddress("TriggersRunL1Seed_ph", &TriggersRunL1Seed_ph, &b_TriggersRunL1Seed_ph);
   fChain->SetBranchAddress("TriggersRunL1Seed_el", &TriggersRunL1Seed_el, &b_TriggersRunL1Seed_el);
   fChain->SetBranchAddress("TriggersRun_ph", &TriggersRun_ph, &b_TriggersRun_ph);
   fChain->SetBranchAddress("TriggersRun_el", &TriggersRun_el, &b_TriggersRun_el);
   fChain->SetBranchAddress("PassL1_ph", &PassL1_ph, &b_PassL1_ph);
   fChain->SetBranchAddress("PrescaleL1_ph", &PrescaleL1_ph, &b_PrescaleL1_ph);
   fChain->SetBranchAddress("PassL2_ph", &PassL2_ph, &b_PassL2_ph);
   fChain->SetBranchAddress("PrescaleL2_ph", &PrescaleL2_ph, &b_PrescaleL2_ph);
   fChain->SetBranchAddress("PassEF_ph", &PassEF_ph, &b_PassEF_ph);
   fChain->SetBranchAddress("PrescaleEF_ph", &PrescaleEF_ph, &b_PrescaleEF_ph);
   fChain->SetBranchAddress("PassL1_el", &PassL1_el, &b_PassL1_el);
   fChain->SetBranchAddress("PrescaleL1_el", &PrescaleL1_el, &b_PrescaleL1_el);
   fChain->SetBranchAddress("PassL2_el", &PassL2_el, &b_PassL2_el);
   fChain->SetBranchAddress("PrescaleL2_el", &PrescaleL2_el, &b_PrescaleL2_el);
   fChain->SetBranchAddress("PassEF_el", &PassEF_el, &b_PassEF_el);
   fChain->SetBranchAddress("PrescaleEF_el", &PrescaleEF_el, &b_PrescaleEF_el);
   fChain->SetBranchAddress("PassEF_el_passedthrough", &PassEF_el_passedthrough, &b_PassEF_el_passedthrough);
   fChain->SetBranchAddress("Accept_MBTSTime", &Accept_MBTSTime, &b_Accept_MBTSTime);
   fChain->SetBranchAddress("Accept_CaloTime", &Accept_CaloTime, &b_Accept_CaloTime);
   fChain->SetBranchAddress("Accept_Trigger", &Accept_Trigger, &b_Accept_Trigger);
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
   fChain->SetBranchAddress("PV_ID_xvertex", &PV_ID_xvertex, &b_PV_ID_xvertex);
   fChain->SetBranchAddress("PV_ID_yvertex", &PV_ID_yvertex, &b_PV_ID_yvertex);
   fChain->SetBranchAddress("PV_ID_zvertex", &PV_ID_zvertex, &b_PV_ID_zvertex);
   fChain->SetBranchAddress("PV_ntracks", &PV_ntracks, &b_PV_ntracks);
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
   fChain->SetBranchAddress("truth_ph_partonIsolation15_UE", &truth_ph_partonIsolation15_UE, &b_truth_ph_partonIsolation15_UE);
   fChain->SetBranchAddress("truth_ph_partonIsolation20_UE", &truth_ph_partonIsolation20_UE, &b_truth_ph_partonIsolation20_UE);
   fChain->SetBranchAddress("truth_ph_partonIsolation25_UE", &truth_ph_partonIsolation25_UE, &b_truth_ph_partonIsolation25_UE);
   fChain->SetBranchAddress("truth_ph_partonIsolation30_UE", &truth_ph_partonIsolation30_UE, &b_truth_ph_partonIsolation30_UE);
   fChain->SetBranchAddress("truth_ph_partonIsolation35_UE", &truth_ph_partonIsolation35_UE, &b_truth_ph_partonIsolation35_UE);
   fChain->SetBranchAddress("truth_ph_partonIsolation40_UE", &truth_ph_partonIsolation40_UE, &b_truth_ph_partonIsolation40_UE);
   fChain->SetBranchAddress("truth_ph_partonIsolation45_UE", &truth_ph_partonIsolation45_UE, &b_truth_ph_partonIsolation45_UE);
   fChain->SetBranchAddress("truth_ph_partonIsolation60_UE", &truth_ph_partonIsolation60_UE, &b_truth_ph_partonIsolation60_UE);
   fChain->SetBranchAddress("truth_ph_partonIsolation15", &truth_ph_partonIsolation15, &b_truth_ph_partonIsolation15);
   fChain->SetBranchAddress("truth_ph_partonIsolation20", &truth_ph_partonIsolation20, &b_truth_ph_partonIsolation20);
   fChain->SetBranchAddress("truth_ph_partonIsolation25", &truth_ph_partonIsolation25, &b_truth_ph_partonIsolation25);
   fChain->SetBranchAddress("truth_ph_partonIsolation30", &truth_ph_partonIsolation30, &b_truth_ph_partonIsolation30);
   fChain->SetBranchAddress("truth_ph_partonIsolation35", &truth_ph_partonIsolation35, &b_truth_ph_partonIsolation35);
   fChain->SetBranchAddress("truth_ph_partonIsolation40", &truth_ph_partonIsolation40, &b_truth_ph_partonIsolation40);
   fChain->SetBranchAddress("truth_ph_partonIsolation45", &truth_ph_partonIsolation45, &b_truth_ph_partonIsolation45);
   fChain->SetBranchAddress("truth_ph_partonIsolation60", &truth_ph_partonIsolation60, &b_truth_ph_partonIsolation60);
   fChain->SetBranchAddress("truth_ph_particleIsolation40", &truth_ph_particleIsolation40, &b_truth_ph_particleIsolation40);
   fChain->SetBranchAddress("truth_ph_particleIsolation40_ED_corrected", &truth_ph_particleIsolation40_ED_corrected, &b_truth_ph_particleIsolation40_ED_corrected);
   fChain->SetBranchAddress("truth_ph_particleIsolation40_ED_corrected_reco", &truth_ph_particleIsolation40_ED_corrected_reco, &b_truth_ph_particleIsolation40_ED_corrected_reco);
   fChain->SetBranchAddress("truth_jet_eta", &truth_jet_eta, &b_truth_jet_eta);
   fChain->SetBranchAddress("truth_jet_phi", &truth_jet_phi, &b_truth_jet_phi);
   fChain->SetBranchAddress("truth_jet_pt", &truth_jet_pt, &b_truth_jet_pt);
   fChain->SetBranchAddress("truth_jet_E", &truth_jet_E, &b_truth_jet_E);
   fChain->SetBranchAddress("truth_jet_parton", &truth_jet_parton, &b_truth_jet_parton);
   fChain->SetBranchAddress("truth_jet_iRecPhoton", &truth_jet_iRecPhoton, &b_truth_jet_iRecPhoton);
   fChain->SetBranchAddress("truth_jet_deltaR_RecPhoton", &truth_jet_deltaR_RecPhoton, &b_truth_jet_deltaR_RecPhoton);
   fChain->SetBranchAddress("ph_indexElectronSameAthenaBarcode", &ph_indexElectronSameAthenaBarcode, &b_ph_indexElectronSameAthenaBarcode);
   fChain->SetBranchAddress("ph_author", &ph_author, &b_ph_author);
   fChain->SetBranchAddress("ph_isGoodOQbits", &ph_isGoodOQbits, &b_ph_isGoodOQbits);
   fChain->SetBranchAddress("ph_isGoodOQphoton", &ph_isGoodOQphoton, &b_ph_isGoodOQphoton);
   fChain->SetBranchAddress("ph_isGoodOQelectron", &ph_isGoodOQelectron, &b_ph_isGoodOQelectron);
   fChain->SetBranchAddress("test_ph_isGoodOQbits_beforeTool", &test_ph_isGoodOQbits_beforeTool, &b_test_ph_isGoodOQbits_beforeTool);
   fChain->SetBranchAddress("test_ph_isGoodOQphoton_beforeTool", &test_ph_isGoodOQphoton_beforeTool, &b_test_ph_isGoodOQphoton_beforeTool);
   fChain->SetBranchAddress("test_ph_isGoodOQelectron_beforeTool", &test_ph_isGoodOQelectron_beforeTool, &b_test_ph_isGoodOQelectron_beforeTool);
   fChain->SetBranchAddress("ph_eta", &ph_eta, &b_ph_eta);
   fChain->SetBranchAddress("ph_etaS0", &ph_etaS0, &b_ph_etaS0);
   fChain->SetBranchAddress("ph_etaS1", &ph_etaS1, &b_ph_etaS1);
   fChain->SetBranchAddress("ph_etaS2", &ph_etaS2, &b_ph_etaS2);
   fChain->SetBranchAddress("ph_phi", &ph_phi, &b_ph_phi);
   fChain->SetBranchAddress("ph_pt", &ph_pt, &b_ph_pt);
   fChain->SetBranchAddress("ph_Tclus", &ph_Tclus, &b_ph_Tclus);
   fChain->SetBranchAddress("ph_E", &ph_E, &b_ph_E);
   fChain->SetBranchAddress("ph_ES0_real", &ph_ES0_real, &b_ph_ES0_real);
   fChain->SetBranchAddress("ph_ES1_real", &ph_ES1_real, &b_ph_ES1_real);
   fChain->SetBranchAddress("ph_ES2_real", &ph_ES2_real, &b_ph_ES2_real);
   fChain->SetBranchAddress("ph_ES3_real", &ph_ES3_real, &b_ph_ES3_real);
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
   fChain->SetBranchAddress("ph_convTrk1_chi2", &ph_convTrk1_chi2, &b_ph_convTrk1_chi2);
   fChain->SetBranchAddress("ph_convTrk2_chi2", &ph_convTrk2_chi2, &b_ph_convTrk2_chi2);
   fChain->SetBranchAddress("ph_conv_chi2", &ph_conv_chi2, &b_ph_conv_chi2);
   fChain->SetBranchAddress("ph_conv_dcottheta", &ph_conv_dcottheta, &b_ph_conv_dcottheta);
   fChain->SetBranchAddress("ph_conv_dphi", &ph_conv_dphi, &b_ph_conv_dphi);
   fChain->SetBranchAddress("ph_conv_dist", &ph_conv_dist, &b_ph_conv_dist);
   fChain->SetBranchAddress("ph_conv_DR1R2", &ph_conv_DR1R2, &b_ph_conv_DR1R2);
   fChain->SetBranchAddress("ph_convTrk1_expectHitInBLayer", &ph_convTrk1_expectHitInBLayer, &b_ph_convTrk1_expectHitInBLayer);
   fChain->SetBranchAddress("ph_convTrk1_numBLHits", &ph_convTrk1_numBLHits, &b_ph_convTrk1_numBLHits);
   fChain->SetBranchAddress("ph_convTrk1_numPixelHits", &ph_convTrk1_numPixelHits, &b_ph_convTrk1_numPixelHits);
   fChain->SetBranchAddress("ph_convTrk1_numSCTHits", &ph_convTrk1_numSCTHits, &b_ph_convTrk1_numSCTHits);
   fChain->SetBranchAddress("ph_convTrk1_numTRTHits", &ph_convTrk1_numTRTHits, &b_ph_convTrk1_numTRTHits);
   fChain->SetBranchAddress("ph_convTrk1_numTRTOutliers", &ph_convTrk1_numTRTOutliers, &b_ph_convTrk1_numTRTOutliers);
   fChain->SetBranchAddress("ph_convTrk1_numTRTHTHits", &ph_convTrk1_numTRTHTHits, &b_ph_convTrk1_numTRTHTHits);
   fChain->SetBranchAddress("ph_convTrk1_numTRTHTOutliers", &ph_convTrk1_numTRTHTOutliers, &b_ph_convTrk1_numTRTHTOutliers);
   fChain->SetBranchAddress("ph_convTrk2_expectHitInBLayer", &ph_convTrk2_expectHitInBLayer, &b_ph_convTrk2_expectHitInBLayer);
   fChain->SetBranchAddress("ph_convTrk2_numBLHits", &ph_convTrk2_numBLHits, &b_ph_convTrk2_numBLHits);
   fChain->SetBranchAddress("ph_convTrk2_numPixelHits", &ph_convTrk2_numPixelHits, &b_ph_convTrk2_numPixelHits);
   fChain->SetBranchAddress("ph_convTrk2_numSCTHits", &ph_convTrk2_numSCTHits, &b_ph_convTrk2_numSCTHits);
   fChain->SetBranchAddress("ph_convTrk2_numTRTHits", &ph_convTrk2_numTRTHits, &b_ph_convTrk2_numTRTHits);
   fChain->SetBranchAddress("ph_convTrk2_numTRTOutliers", &ph_convTrk2_numTRTOutliers, &b_ph_convTrk2_numTRTOutliers);
   fChain->SetBranchAddress("ph_convTrk2_numTRTHTHits", &ph_convTrk2_numTRTHTHits, &b_ph_convTrk2_numTRTHTHits);
   fChain->SetBranchAddress("ph_convTrk2_numTRTHTOutliers", &ph_convTrk2_numTRTHTOutliers, &b_ph_convTrk2_numTRTHTOutliers);
   fChain->SetBranchAddress("ph_convTrk1_trtPID", &ph_convTrk1_trtPID, &b_ph_convTrk1_trtPID);
   fChain->SetBranchAddress("ph_convTrk2_trtPID", &ph_convTrk2_trtPID, &b_ph_convTrk2_trtPID);
   fChain->SetBranchAddress("ph_convTrk1_trtPIDHT", &ph_convTrk1_trtPIDHT, &b_ph_convTrk1_trtPIDHT);
   fChain->SetBranchAddress("ph_convTrk2_trtPIDHT", &ph_convTrk2_trtPIDHT, &b_ph_convTrk2_trtPIDHT);
   fChain->SetBranchAddress("ph_convTrk1_trtPIDToT", &ph_convTrk1_trtPIDToT, &b_ph_convTrk1_trtPIDToT);
   fChain->SetBranchAddress("ph_convTrk2_trtPIDToT", &ph_convTrk2_trtPIDToT, &b_ph_convTrk2_trtPIDToT);
   fChain->SetBranchAddress("ph_convTrk1_ToTvar", &ph_convTrk1_ToTvar, &b_ph_convTrk1_ToTvar);
   fChain->SetBranchAddress("ph_convTrk2_ToTvar", &ph_convTrk2_ToTvar, &b_ph_convTrk2_ToTvar);
   fChain->SetBranchAddress("ph_convTrk1_trtRatioHT", &ph_convTrk1_trtRatioHT, &b_ph_convTrk1_trtRatioHT);
   fChain->SetBranchAddress("ph_convTrk2_trtRatioHT", &ph_convTrk2_trtRatioHT, &b_ph_convTrk2_trtRatioHT);
   fChain->SetBranchAddress("ph_convTrk1_charge", &ph_convTrk1_charge, &b_ph_convTrk1_charge);
   fChain->SetBranchAddress("ph_convTrk2_charge", &ph_convTrk2_charge, &b_ph_convTrk2_charge);
   fChain->SetBranchAddress("ph_convTrk1_DeltaEta_track_calo", &ph_convTrk1_DeltaEta_track_calo, &b_ph_convTrk1_DeltaEta_track_calo);
   fChain->SetBranchAddress("ph_convTrk2_DeltaEta_track_calo", &ph_convTrk2_DeltaEta_track_calo, &b_ph_convTrk2_DeltaEta_track_calo);
   fChain->SetBranchAddress("ph_convTrk1_DeltaPhi_track_calo", &ph_convTrk1_DeltaPhi_track_calo, &b_ph_convTrk1_DeltaPhi_track_calo);
   fChain->SetBranchAddress("ph_convTrk2_DeltaPhi_track_calo", &ph_convTrk2_DeltaPhi_track_calo, &b_ph_convTrk2_DeltaPhi_track_calo);
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
   fChain->SetBranchAddress("ph_parton_pt_max_MC", &ph_parton_pt_max_MC, &b_ph_parton_pt_max_MC);
   fChain->SetBranchAddress("ph_parton_eta_MC", &ph_parton_eta_MC, &b_ph_parton_eta_MC);
   fChain->SetBranchAddress("ph_parton_phi_MC", &ph_parton_phi_MC, &b_ph_parton_phi_MC);
   fChain->SetBranchAddress("ph_parton_barcode_MC", &ph_parton_barcode_MC, &b_ph_parton_barcode_MC);
   fChain->SetBranchAddress("ph_parton_pdg_MC", &ph_parton_pdg_MC, &b_ph_parton_pdg_MC);
   fChain->SetBranchAddress("ph_isEM", &ph_isEM, &b_ph_isEM);
   fChain->SetBranchAddress("ph_isPhotonLoose", &ph_isPhotonLoose, &b_ph_isPhotonLoose);
   fChain->SetBranchAddress("ph_isPhotonTight", &ph_isPhotonTight, &b_ph_isPhotonTight);
   fChain->SetBranchAddress("ph_ambiguityResult", &ph_ambiguityResult, &b_ph_ambiguityResult);
   fChain->SetBranchAddress("ph_itune1", &ph_itune1, &b_ph_itune1);
   fChain->SetBranchAddress("ph_itune2", &ph_itune2, &b_ph_itune2);
   fChain->SetBranchAddress("ph_itune3", &ph_itune3, &b_ph_itune3);
   fChain->SetBranchAddress("ph_logLikelihood", &ph_logLikelihood, &b_ph_logLikelihood);
   fChain->SetBranchAddress("ph_PtIsolationCone", &ph_PtIsolationCone, &b_ph_PtIsolationCone);
   fChain->SetBranchAddress("ph_PtIsolationConePhAngle", &ph_PtIsolationConePhAngle, &b_ph_PtIsolationConePhAngle);
   fChain->SetBranchAddress("ph_trackIsol", &ph_trackIsol, &b_ph_trackIsol);
   fChain->SetBranchAddress("ph_PtIsolationCone_woIC", &ph_PtIsolationCone_woIC, &b_ph_PtIsolationCone_woIC);
   fChain->SetBranchAddress("ph_PtIsolationConePhAngle_woIC", &ph_PtIsolationConePhAngle_woIC, &b_ph_PtIsolationConePhAngle_woIC);
   fChain->SetBranchAddress("ph_trackIsol_woIC", &ph_trackIsol_woIC, &b_ph_trackIsol_woIC);
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
   fChain->SetBranchAddress("ph_etconoisedR04Sig2", &ph_etconoisedR04Sig2, &b_ph_etconoisedR04Sig2);
   fChain->SetBranchAddress("ph_etconoisedR04Sig3", &ph_etconoisedR04Sig3, &b_ph_etconoisedR04Sig3);
   fChain->SetBranchAddress("ph_shwr_EtCone20", &ph_shwr_EtCone20, &b_ph_shwr_EtCone20);
   fChain->SetBranchAddress("ph_shwr_EtCone30", &ph_shwr_EtCone30, &b_ph_shwr_EtCone30);
   fChain->SetBranchAddress("ph_shwr_EtCone40", &ph_shwr_EtCone40, &b_ph_shwr_EtCone40);
   fChain->SetBranchAddress("ph_shwr_EtCone45", &ph_shwr_EtCone45, &b_ph_shwr_EtCone45);
   fChain->SetBranchAddress("ph_shwr_EtCone20_corrected", &ph_shwr_EtCone20_corrected, &b_ph_shwr_EtCone20_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone30_corrected", &ph_shwr_EtCone30_corrected, &b_ph_shwr_EtCone30_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone40_corrected", &ph_shwr_EtCone40_corrected, &b_ph_shwr_EtCone40_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone20_pt_corrected", &ph_shwr_EtCone20_pt_corrected, &b_ph_shwr_EtCone20_pt_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone30_pt_corrected", &ph_shwr_EtCone30_pt_corrected, &b_ph_shwr_EtCone30_pt_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone40_pt_corrected", &ph_shwr_EtCone40_pt_corrected, &b_ph_shwr_EtCone40_pt_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone20_ED_corrected", &ph_shwr_EtCone20_ED_corrected, &b_ph_shwr_EtCone20_ED_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone30_ED_corrected", &ph_shwr_EtCone30_ED_corrected, &b_ph_shwr_EtCone30_ED_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone40_ED_corrected", &ph_shwr_EtCone40_ED_corrected, &b_ph_shwr_EtCone40_ED_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone40_ED0_corrected", &ph_shwr_EtCone40_ED0_corrected, &b_ph_shwr_EtCone40_ED0_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone40_ED1_corrected", &ph_shwr_EtCone40_ED1_corrected, &b_ph_shwr_EtCone40_ED1_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone40_ED2_corrected", &ph_shwr_EtCone40_ED2_corrected, &b_ph_shwr_EtCone40_ED2_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone40_ED3_corrected", &ph_shwr_EtCone40_ED3_corrected, &b_ph_shwr_EtCone40_ED3_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone40_ED4_corrected", &ph_shwr_EtCone40_ED4_corrected, &b_ph_shwr_EtCone40_ED4_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone40_ED5_corrected", &ph_shwr_EtCone40_ED5_corrected, &b_ph_shwr_EtCone40_ED5_corrected);
   fChain->SetBranchAddress("ph_ED_median", &ph_ED_median, &b_ph_ED_median);
   fChain->SetBranchAddress("ph_ED_sigma", &ph_ED_sigma, &b_ph_ED_sigma);
   fChain->SetBranchAddress("ph_ED_Njets", &ph_ED_Njets, &b_ph_ED_Njets);
   fChain->SetBranchAddress("ph_transmin_etcone40", &ph_transmin_etcone40, &b_ph_transmin_etcone40);
   fChain->SetBranchAddress("ph_transmax_etcone40", &ph_transmax_etcone40, &b_ph_transmax_etcone40);
   fChain->SetBranchAddress("ph_transmin_etcone100", &ph_transmin_etcone100, &b_ph_transmin_etcone100);
   fChain->SetBranchAddress("ph_transmax_etcone100", &ph_transmax_etcone100, &b_ph_transmax_etcone100);
   fChain->SetBranchAddress("ph_transmin", &ph_transmin, &b_ph_transmin);
   fChain->SetBranchAddress("ph_transmax", &ph_transmax, &b_ph_transmax);
   fChain->SetBranchAddress("ph_transmin_loweta", &ph_transmin_loweta, &b_ph_transmin_loweta);
   fChain->SetBranchAddress("ph_transmax_loweta", &ph_transmax_loweta, &b_ph_transmax_loweta);
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
   fChain->SetBranchAddress("ph_isG4part", &ph_isG4part, &b_ph_isG4part);
   fChain->SetBranchAddress("ph_motherPDG", &ph_motherPDG, &b_ph_motherPDG);
   fChain->SetBranchAddress("ph_isBrem_MC", &ph_isBrem_MC, &b_ph_isBrem_MC);
   fChain->SetBranchAddress("ph_isFromHardProc_MC", &ph_isFromHardProc_MC, &b_ph_isFromHardProc_MC);
   fChain->SetBranchAddress("ph_isPhotonFromHardProc_MC", &ph_isPhotonFromHardProc_MC, &b_ph_isPhotonFromHardProc_MC);
   fChain->SetBranchAddress("ph_imatchParton_MC", &ph_imatchParton_MC, &b_ph_imatchParton_MC);
   fChain->SetBranchAddress("ph_partonIsolation15_UE_MC", &ph_partonIsolation15_UE_MC, &b_ph_partonIsolation15_UE_MC);
   fChain->SetBranchAddress("ph_partonIsolation20_UE_MC", &ph_partonIsolation20_UE_MC, &b_ph_partonIsolation20_UE_MC);
   fChain->SetBranchAddress("ph_partonIsolation25_UE_MC", &ph_partonIsolation25_UE_MC, &b_ph_partonIsolation25_UE_MC);
   fChain->SetBranchAddress("ph_partonIsolation30_UE_MC", &ph_partonIsolation30_UE_MC, &b_ph_partonIsolation30_UE_MC);
   fChain->SetBranchAddress("ph_partonIsolation35_UE_MC", &ph_partonIsolation35_UE_MC, &b_ph_partonIsolation35_UE_MC);
   fChain->SetBranchAddress("ph_partonIsolation40_UE_MC", &ph_partonIsolation40_UE_MC, &b_ph_partonIsolation40_UE_MC);
   fChain->SetBranchAddress("ph_partonIsolation45_UE_MC", &ph_partonIsolation45_UE_MC, &b_ph_partonIsolation45_UE_MC);
   fChain->SetBranchAddress("ph_partonIsolation60_UE_MC", &ph_partonIsolation60_UE_MC, &b_ph_partonIsolation60_UE_MC);
   fChain->SetBranchAddress("ph_partonIsolation15_MC", &ph_partonIsolation15_MC, &b_ph_partonIsolation15_MC);
   fChain->SetBranchAddress("ph_partonIsolation20_MC", &ph_partonIsolation20_MC, &b_ph_partonIsolation20_MC);
   fChain->SetBranchAddress("ph_partonIsolation25_MC", &ph_partonIsolation25_MC, &b_ph_partonIsolation25_MC);
   fChain->SetBranchAddress("ph_partonIsolation30_MC", &ph_partonIsolation30_MC, &b_ph_partonIsolation30_MC);
   fChain->SetBranchAddress("ph_partonIsolation35_MC", &ph_partonIsolation35_MC, &b_ph_partonIsolation35_MC);
   fChain->SetBranchAddress("ph_partonIsolation40_MC", &ph_partonIsolation40_MC, &b_ph_partonIsolation40_MC);
   fChain->SetBranchAddress("ph_partonIsolation45_MC", &ph_partonIsolation45_MC, &b_ph_partonIsolation45_MC);
   fChain->SetBranchAddress("ph_partonIsolation60_MC", &ph_partonIsolation60_MC, &b_ph_partonIsolation60_MC);
   fChain->SetBranchAddress("ph_particleIsolation40_MC", &ph_particleIsolation40_MC, &b_ph_particleIsolation40_MC);
   fChain->SetBranchAddress("ph_particleIsolation40_ED_corrected_MC", &ph_particleIsolation40_ED_corrected_MC, &b_ph_particleIsolation40_ED_corrected_MC);
   fChain->SetBranchAddress("ph_particleIsolation40_ED_corrected_reco_MC", &ph_particleIsolation40_ED_corrected_reco_MC, &b_ph_particleIsolation40_ED_corrected_reco_MC);
   fChain->SetBranchAddress("ph_shwr_EtCone10", &ph_shwr_EtCone10, &b_ph_shwr_EtCone10);
   fChain->SetBranchAddress("ph_shwr_EtCone15", &ph_shwr_EtCone15, &b_ph_shwr_EtCone15);
   fChain->SetBranchAddress("ph_shwr_EtCone25", &ph_shwr_EtCone25, &b_ph_shwr_EtCone25);
   fChain->SetBranchAddress("ph_shwr_EtCone35", &ph_shwr_EtCone35, &b_ph_shwr_EtCone35);
   fChain->SetBranchAddress("ph_shwr_EtCone10_corrected", &ph_shwr_EtCone10_corrected, &b_ph_shwr_EtCone10_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone15_corrected", &ph_shwr_EtCone15_corrected, &b_ph_shwr_EtCone15_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone25_corrected", &ph_shwr_EtCone25_corrected, &b_ph_shwr_EtCone25_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone35_corrected", &ph_shwr_EtCone35_corrected, &b_ph_shwr_EtCone35_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone10_pt_corrected", &ph_shwr_EtCone10_pt_corrected, &b_ph_shwr_EtCone10_pt_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone15_pt_corrected", &ph_shwr_EtCone15_pt_corrected, &b_ph_shwr_EtCone15_pt_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone25_pt_corrected", &ph_shwr_EtCone25_pt_corrected, &b_ph_shwr_EtCone25_pt_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone35_pt_corrected", &ph_shwr_EtCone35_pt_corrected, &b_ph_shwr_EtCone35_pt_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone10_ED_corrected", &ph_shwr_EtCone10_ED_corrected, &b_ph_shwr_EtCone10_ED_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone15_ED_corrected", &ph_shwr_EtCone15_ED_corrected, &b_ph_shwr_EtCone15_ED_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone25_ED_corrected", &ph_shwr_EtCone25_ED_corrected, &b_ph_shwr_EtCone25_ED_corrected);
   fChain->SetBranchAddress("ph_shwr_EtCone35_ED_corrected", &ph_shwr_EtCone35_ED_corrected, &b_ph_shwr_EtCone35_ED_corrected);
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
   fChain->SetBranchAddress("trk_charge", &trk_charge, &b_trk_charge);
   fChain->SetBranchAddress("trk_d0", &trk_d0, &b_trk_d0);
   fChain->SetBranchAddress("trk_x0", &trk_x0, &b_trk_x0);
   fChain->SetBranchAddress("trk_y0", &trk_y0, &b_trk_y0);
   fChain->SetBranchAddress("trk_z0", &trk_z0, &b_trk_z0);
   fChain->SetBranchAddress("trk_nhits_b", &trk_nhits_b, &b_trk_nhits_b);
   fChain->SetBranchAddress("trk_nhits_p", &trk_nhits_p, &b_trk_nhits_p);
   fChain->SetBranchAddress("trk_ndead_p", &trk_ndead_p, &b_trk_ndead_p);
   fChain->SetBranchAddress("trk_nhits_s", &trk_nhits_s, &b_trk_nhits_s);
   fChain->SetBranchAddress("trk_ndead_s", &trk_ndead_s, &b_trk_ndead_s);
   fChain->SetBranchAddress("trk_nhits_trt", &trk_nhits_trt, &b_trk_nhits_trt);
   fChain->SetBranchAddress("trk_nhits_trth", &trk_nhits_trth, &b_trk_nhits_trth);
   fChain->SetBranchAddress("trk_chiSquared", &trk_chiSquared, &b_trk_chiSquared);
   fChain->SetBranchAddress("trk_numberDoF", &trk_numberDoF, &b_trk_numberDoF);
   fChain->SetBranchAddress("trk_gConv", &trk_gConv, &b_trk_gConv);
   fChain->SetBranchAddress("trk_conv", &trk_conv, &b_trk_conv);
   fChain->SetBranchAddress("trk_calo_eta", &trk_calo_eta, &b_trk_calo_eta);
   fChain->SetBranchAddress("trk_calo_phi", &trk_calo_phi, &b_trk_calo_phi);
   fChain->SetBranchAddress("trk_calo_r", &trk_calo_r, &b_trk_calo_r);
   fChain->SetBranchAddress("jet_eta", &jet_eta, &b_jet_eta);
   fChain->SetBranchAddress("jet_phi", &jet_phi, &b_jet_phi);
   fChain->SetBranchAddress("jet_pt", &jet_pt, &b_jet_pt);
   fChain->SetBranchAddress("jet_E", &jet_E, &b_jet_E);
   fChain->SetBranchAddress("jet_selected_1j", &jet_selected_1j, &b_jet_selected_1j);
   fChain->SetBranchAddress("jet_selected_2j", &jet_selected_2j, &b_jet_selected_2j);
   fChain->SetBranchAddress("jet_n90cells", &jet_n90cells, &b_jet_n90cells);
   fChain->SetBranchAddress("jet_n90constituents", &jet_n90constituents, &b_jet_n90constituents);
   fChain->SetBranchAddress("jet_hecf", &jet_hecf, &b_jet_hecf);
   fChain->SetBranchAddress("jet_tileGap3F", &jet_tileGap3F, &b_jet_tileGap3F);
   fChain->SetBranchAddress("jet_fcor", &jet_fcor, &b_jet_fcor);
   fChain->SetBranchAddress("jet_corr_cell", &jet_corr_cell, &b_jet_corr_cell);
   fChain->SetBranchAddress("jet_corr_dotx", &jet_corr_dotx, &b_jet_corr_dotx);
   fChain->SetBranchAddress("jet_samplingMax", &jet_samplingMax, &b_jet_samplingMax);
   fChain->SetBranchAddress("jet_samplingMaxId", &jet_samplingMaxId, &b_jet_samplingMaxId);
   fChain->SetBranchAddress("jet_emFraction", &jet_emFraction, &b_jet_emFraction);
   fChain->SetBranchAddress("jet_pt_emScale", &jet_pt_emScale, &b_jet_pt_emScale);
   fChain->SetBranchAddress("jet_eta_emScale", &jet_eta_emScale, &b_jet_eta_emScale);
   fChain->SetBranchAddress("jet_phi_emScale", &jet_phi_emScale, &b_jet_phi_emScale);
   fChain->SetBranchAddress("jet_e_emScale", &jet_e_emScale, &b_jet_e_emScale);
   fChain->SetBranchAddress("jet_time", &jet_time, &b_jet_time);
   fChain->SetBranchAddress("jet_quality", &jet_quality, &b_jet_quality);
   fChain->SetBranchAddress("jet_isGood", &jet_isGood, &b_jet_isGood);
   fChain->SetBranchAddress("jet_isBad", &jet_isBad, &b_jet_isBad);
   fChain->SetBranchAddress("jet_isUgly", &jet_isUgly, &b_jet_isUgly);
   fChain->SetBranchAddress("jet_EMJES", &jet_EMJES, &b_jet_EMJES);
   fChain->SetBranchAddress("jet_imatchTrueJet_MC", &jet_imatchTrueJet_MC, &b_jet_imatchTrueJet_MC);
   fChain->SetBranchAddress("jet_eta_MC", &jet_eta_MC, &b_jet_eta_MC);
   fChain->SetBranchAddress("jet_phi_MC", &jet_phi_MC, &b_jet_phi_MC);
   fChain->SetBranchAddress("jet_pt_MC", &jet_pt_MC, &b_jet_pt_MC);
   fChain->SetBranchAddress("jet_E_MC", &jet_E_MC, &b_jet_E_MC);
   fChain->SetBranchAddress("el_author", &el_author, &b_el_author);
   fChain->SetBranchAddress("el_isGoodOQbits", &el_isGoodOQbits, &b_el_isGoodOQbits);
   fChain->SetBranchAddress("el_isGoodOQphoton", &el_isGoodOQphoton, &b_el_isGoodOQphoton);
   fChain->SetBranchAddress("el_isGoodOQelectron", &el_isGoodOQelectron, &b_el_isGoodOQelectron);
   fChain->SetBranchAddress("test_el_isGoodOQbits_beforeTool", &test_el_isGoodOQbits_beforeTool, &b_test_el_isGoodOQbits_beforeTool);
   fChain->SetBranchAddress("test_el_isGoodOQphoton_beforeTool", &test_el_isGoodOQphoton_beforeTool, &b_test_el_isGoodOQphoton_beforeTool);
   fChain->SetBranchAddress("test_el_isGoodOQelectron_beforeTool", &test_el_isGoodOQelectron_beforeTool, &b_test_el_isGoodOQelectron_beforeTool);
   fChain->SetBranchAddress("el_eta", &el_eta, &b_el_eta);
   fChain->SetBranchAddress("el_phi", &el_phi, &b_el_phi);
   fChain->SetBranchAddress("el_pt", &el_pt, &b_el_pt);
   fChain->SetBranchAddress("el_E", &el_E, &b_el_E);
   fChain->SetBranchAddress("el_etaS0", &el_etaS0, &b_el_etaS0);
   fChain->SetBranchAddress("el_etaS1", &el_etaS1, &b_el_etaS1);
   fChain->SetBranchAddress("el_etaS2", &el_etaS2, &b_el_etaS2);
   fChain->SetBranchAddress("el_ES0_real", &el_ES0_real, &b_el_ES0_real);
   fChain->SetBranchAddress("el_ES1_real", &el_ES1_real, &b_el_ES1_real);
   fChain->SetBranchAddress("el_ES2_real", &el_ES2_real, &b_el_ES2_real);
   fChain->SetBranchAddress("el_ES3_real", &el_ES3_real, &b_el_ES3_real);
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
   fChain->SetBranchAddress("el_pt_trk", &el_pt_trk, &b_el_pt_trk);
   fChain->SetBranchAddress("el_eta_trk", &el_eta_trk, &b_el_eta_trk);
   fChain->SetBranchAddress("el_d0", &el_d0, &b_el_d0);
   fChain->SetBranchAddress("el_z0", &el_z0, &b_el_z0);
   fChain->SetBranchAddress("el_theta", &el_theta, &b_el_theta);
   fChain->SetBranchAddress("el_phi_trk", &el_phi_trk, &b_el_phi_trk);
   fChain->SetBranchAddress("el_qOverP", &el_qOverP, &b_el_qOverP);
   fChain->SetBranchAddress("el_nBHits", &el_nBHits, &b_el_nBHits);
   fChain->SetBranchAddress("el_nPixelHits", &el_nPixelHits, &b_el_nPixelHits);
   fChain->SetBranchAddress("el_nSCTHits", &el_nSCTHits, &b_el_nSCTHits);
   fChain->SetBranchAddress("el_nTRTHits", &el_nTRTHits, &b_el_nTRTHits);
   fChain->SetBranchAddress("el_iPV", &el_iPV, &b_el_iPV);
   fChain->SetBranchAddress("el_isEM", &el_isEM, &b_el_isEM);
   fChain->SetBranchAddress("el_isElectronLoose", &el_isElectronLoose, &b_el_isElectronLoose);
   fChain->SetBranchAddress("el_isElectronMedium", &el_isElectronMedium, &b_el_isElectronMedium);
   fChain->SetBranchAddress("el_isElectronTight", &el_isElectronTight, &b_el_isElectronTight);
   fChain->SetBranchAddress("el_isRobustLoose", &el_isRobustLoose, &b_el_isRobustLoose);
   fChain->SetBranchAddress("el_isRobustMedium", &el_isRobustMedium, &b_el_isRobustMedium);
   fChain->SetBranchAddress("el_isRobustTight", &el_isRobustTight, &b_el_isRobustTight);
   fChain->SetBranchAddress("el_isRobusterTight", &el_isRobusterTight, &b_el_isRobusterTight);
   fChain->SetBranchAddress("el_expectHitInBLayer", &el_expectHitInBLayer, &b_el_expectHitInBLayer);
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
   fChain->SetBranchAddress("el_ambiguityResult", &el_ambiguityResult, &b_el_ambiguityResult);
   fChain->SetBranchAddress("el_itune1", &el_itune1, &b_el_itune1);
   fChain->SetBranchAddress("el_itune2", &el_itune2, &b_el_itune2);
   fChain->SetBranchAddress("el_itune3", &el_itune3, &b_el_itune3);
   fChain->SetBranchAddress("el_logLikelihood", &el_logLikelihood, &b_el_logLikelihood);
   fChain->SetBranchAddress("el_PtIsolationCone", &el_PtIsolationCone, &b_el_PtIsolationCone);
   fChain->SetBranchAddress("el_trackIsol", &el_trackIsol, &b_el_trackIsol);
   fChain->SetBranchAddress("el_PtIsolationCone_woIC", &el_PtIsolationCone_woIC, &b_el_PtIsolationCone_woIC);
   fChain->SetBranchAddress("el_trackIsol_woIC", &el_trackIsol_woIC, &b_el_trackIsol_woIC);
   fChain->SetBranchAddress("el_dR_isol", &el_dR_isol, &b_el_dR_isol);
   fChain->SetBranchAddress("el_selected_etmiss", &el_selected_etmiss, &b_el_selected_etmiss);
   fChain->SetBranchAddress("el_selected_etlep", &el_selected_etlep, &b_el_selected_etlep);
   fChain->SetBranchAddress("el_shwr_EtCone20", &el_shwr_EtCone20, &b_el_shwr_EtCone20);
   fChain->SetBranchAddress("el_shwr_EtCone30", &el_shwr_EtCone30, &b_el_shwr_EtCone30);
   fChain->SetBranchAddress("el_shwr_EtCone40", &el_shwr_EtCone40, &b_el_shwr_EtCone40);
   fChain->SetBranchAddress("el_shwr_EtCone45", &el_shwr_EtCone45, &b_el_shwr_EtCone45);
   fChain->SetBranchAddress("el_shwr_EtCone20_corrected", &el_shwr_EtCone20_corrected, &b_el_shwr_EtCone20_corrected);
   fChain->SetBranchAddress("el_shwr_EtCone30_corrected", &el_shwr_EtCone30_corrected, &b_el_shwr_EtCone30_corrected);
   fChain->SetBranchAddress("el_shwr_EtCone40_corrected", &el_shwr_EtCone40_corrected, &b_el_shwr_EtCone40_corrected);
   fChain->SetBranchAddress("el_shwr_EtCone20_pt_corrected", &el_shwr_EtCone20_pt_corrected, &b_el_shwr_EtCone20_pt_corrected);
   fChain->SetBranchAddress("el_shwr_EtCone30_pt_corrected", &el_shwr_EtCone30_pt_corrected, &b_el_shwr_EtCone30_pt_corrected);
   fChain->SetBranchAddress("el_shwr_EtCone40_pt_corrected", &el_shwr_EtCone40_pt_corrected, &b_el_shwr_EtCone40_pt_corrected);
   fChain->SetBranchAddress("el_shwr_EtCone20_ED_corrected", &el_shwr_EtCone20_ED_corrected, &b_el_shwr_EtCone20_ED_corrected);
   fChain->SetBranchAddress("el_shwr_EtCone30_ED_corrected", &el_shwr_EtCone30_ED_corrected, &b_el_shwr_EtCone30_ED_corrected);
   fChain->SetBranchAddress("el_shwr_EtCone40_ED_corrected", &el_shwr_EtCone40_ED_corrected, &b_el_shwr_EtCone40_ED_corrected);
   fChain->SetBranchAddress("el_ED_median", &el_ED_median, &b_el_ED_median);
   fChain->SetBranchAddress("el_ED_sigma", &el_ED_sigma, &b_el_ED_sigma);
   fChain->SetBranchAddress("el_ED_Njets", &el_ED_Njets, &b_el_ED_Njets);
   fChain->SetBranchAddress("el_shwr_EtRing1030sig2", &el_shwr_EtRing1030sig2, &b_el_shwr_EtRing1030sig2);
   fChain->SetBranchAddress("el_shwr_EtRing1030sig3", &el_shwr_EtRing1030sig3, &b_el_shwr_EtRing1030sig3);
   fChain->SetBranchAddress("el_shwr_EtRing1030sig4", &el_shwr_EtRing1030sig4, &b_el_shwr_EtRing1030sig4);
   fChain->SetBranchAddress("el_matchMC", &el_matchMC, &b_el_matchMC);
   fChain->SetBranchAddress("el_eta_MC", &el_eta_MC, &b_el_eta_MC);
   fChain->SetBranchAddress("el_phi_MC", &el_phi_MC, &b_el_phi_MC);
   fChain->SetBranchAddress("el_pt_MC", &el_pt_MC, &b_el_pt_MC);
   fChain->SetBranchAddress("el_E_MC", &el_E_MC, &b_el_E_MC);
   fChain->SetBranchAddress("el_PDG", &el_PDG, &b_el_PDG);
   fChain->SetBranchAddress("el_motherPDG", &el_motherPDG, &b_el_motherPDG);
   fChain->SetBranchAddress("el_as_unconv_ph_isEM", &el_as_unconv_ph_isEM, &b_el_as_unconv_ph_isEM);
   fChain->SetBranchAddress("el_as_unconv_ph_isPhotonLoose", &el_as_unconv_ph_isPhotonLoose, &b_el_as_unconv_ph_isPhotonLoose);
   fChain->SetBranchAddress("el_as_unconv_ph_isPhotonTight", &el_as_unconv_ph_isPhotonTight, &b_el_as_unconv_ph_isPhotonTight);
   fChain->SetBranchAddress("el_as_unconv_ph_E", &el_as_unconv_ph_E, &b_el_as_unconv_ph_E);
   fChain->SetBranchAddress("el_as_unconv_ph_eta_clus", &el_as_unconv_ph_eta_clus, &b_el_as_unconv_ph_eta_clus);
   fChain->SetBranchAddress("el_as_unconv_ph_phi_clus", &el_as_unconv_ph_phi_clus, &b_el_as_unconv_ph_phi_clus);
   fChain->SetBranchAddress("el_as_unconv_ph_pt_clus", &el_as_unconv_ph_pt_clus, &b_el_as_unconv_ph_pt_clus);
   fChain->SetBranchAddress("el_as_unconv_ph_etaS1", &el_as_unconv_ph_etaS1, &b_el_as_unconv_ph_etaS1);
   fChain->SetBranchAddress("el_as_unconv_ph_etaS2", &el_as_unconv_ph_etaS2, &b_el_as_unconv_ph_etaS2);
   fChain->SetBranchAddress("el_as_conv_ph_isEM", &el_as_conv_ph_isEM, &b_el_as_conv_ph_isEM);
   fChain->SetBranchAddress("el_as_conv_ph_isPhotonLoose", &el_as_conv_ph_isPhotonLoose, &b_el_as_conv_ph_isPhotonLoose);
   fChain->SetBranchAddress("el_as_conv_ph_isPhotonTight", &el_as_conv_ph_isPhotonTight, &b_el_as_conv_ph_isPhotonTight);
   fChain->SetBranchAddress("el_as_conv_ph_E", &el_as_conv_ph_E, &b_el_as_conv_ph_E);
   fChain->SetBranchAddress("el_as_conv_ph_eta_clus", &el_as_conv_ph_eta_clus, &b_el_as_conv_ph_eta_clus);
   fChain->SetBranchAddress("el_as_conv_ph_phi_clus", &el_as_conv_ph_phi_clus, &b_el_as_conv_ph_phi_clus);
   fChain->SetBranchAddress("el_as_conv_ph_pt_clus", &el_as_conv_ph_pt_clus, &b_el_as_conv_ph_pt_clus);
   fChain->SetBranchAddress("el_as_conv_ph_etaS1", &el_as_conv_ph_etaS1, &b_el_as_conv_ph_etaS1);
   fChain->SetBranchAddress("el_as_conv_ph_etaS2", &el_as_conv_ph_etaS2, &b_el_as_conv_ph_etaS2);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_Ethad1", &el_as_conv_ph_shwr_Ethad1, &b_el_as_conv_ph_shwr_Ethad1);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_Ethad", &el_as_conv_ph_shwr_Ethad, &b_el_as_conv_ph_shwr_Ethad);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_E277", &el_as_conv_ph_shwr_E277, &b_el_as_conv_ph_shwr_E277);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_E237", &el_as_conv_ph_shwr_E237, &b_el_as_conv_ph_shwr_E237);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_E233", &el_as_conv_ph_shwr_E233, &b_el_as_conv_ph_shwr_E233);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_weta2", &el_as_conv_ph_shwr_weta2, &b_el_as_conv_ph_shwr_weta2);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_f1", &el_as_conv_ph_shwr_f1, &b_el_as_conv_ph_shwr_f1);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_Emax2", &el_as_conv_ph_shwr_Emax2, &b_el_as_conv_ph_shwr_Emax2);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_Emax1", &el_as_conv_ph_shwr_Emax1, &b_el_as_conv_ph_shwr_Emax1);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_Emin", &el_as_conv_ph_shwr_Emin, &b_el_as_conv_ph_shwr_Emin);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_fracm", &el_as_conv_ph_shwr_fracm, &b_el_as_conv_ph_shwr_fracm);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_wtot", &el_as_conv_ph_shwr_wtot, &b_el_as_conv_ph_shwr_wtot);
   fChain->SetBranchAddress("el_as_conv_ph_shwr_w1", &el_as_conv_ph_shwr_w1, &b_el_as_conv_ph_shwr_w1);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_Ethad1", &el_as_unconv_ph_shwr_Ethad1, &b_el_as_unconv_ph_shwr_Ethad1);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_Ethad", &el_as_unconv_ph_shwr_Ethad, &b_el_as_unconv_ph_shwr_Ethad);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_E277", &el_as_unconv_ph_shwr_E277, &b_el_as_unconv_ph_shwr_E277);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_E237", &el_as_unconv_ph_shwr_E237, &b_el_as_unconv_ph_shwr_E237);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_E233", &el_as_unconv_ph_shwr_E233, &b_el_as_unconv_ph_shwr_E233);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_weta2", &el_as_unconv_ph_shwr_weta2, &b_el_as_unconv_ph_shwr_weta2);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_f1", &el_as_unconv_ph_shwr_f1, &b_el_as_unconv_ph_shwr_f1);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_Emax2", &el_as_unconv_ph_shwr_Emax2, &b_el_as_unconv_ph_shwr_Emax2);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_Emax1", &el_as_unconv_ph_shwr_Emax1, &b_el_as_unconv_ph_shwr_Emax1);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_Emin", &el_as_unconv_ph_shwr_Emin, &b_el_as_unconv_ph_shwr_Emin);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_fracm", &el_as_unconv_ph_shwr_fracm, &b_el_as_unconv_ph_shwr_fracm);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_wtot", &el_as_unconv_ph_shwr_wtot, &b_el_as_unconv_ph_shwr_wtot);
   fChain->SetBranchAddress("el_as_unconv_ph_shwr_w1", &el_as_unconv_ph_shwr_w1, &b_el_as_unconv_ph_shwr_w1);
   fChain->SetBranchAddress("el_L1_matchPass", &el_L1_matchPass, &b_el_L1_matchPass);
   fChain->SetBranchAddress("el_L2_matchPass", &el_L2_matchPass, &b_el_L2_matchPass);
   fChain->SetBranchAddress("el_EF_matchPass", &el_EF_matchPass, &b_el_EF_matchPass);
   fChain->SetBranchAddress("el_L1_bestMatchDeltaR", &el_L1_bestMatchDeltaR, &b_el_L1_bestMatchDeltaR);
   fChain->SetBranchAddress("el_L2_bestMatchDeltaR", &el_L2_bestMatchDeltaR, &b_el_L2_bestMatchDeltaR);
   fChain->SetBranchAddress("el_EF_bestMatchDeltaR", &el_EF_bestMatchDeltaR, &b_el_EF_bestMatchDeltaR);
   fChain->SetBranchAddress("el_L1_matchPass_ph", &el_L1_matchPass_ph, &b_el_L1_matchPass_ph);
   fChain->SetBranchAddress("el_L2_matchPass_ph", &el_L2_matchPass_ph, &b_el_L2_matchPass_ph);
   fChain->SetBranchAddress("el_EF_matchPass_ph", &el_EF_matchPass_ph, &b_el_EF_matchPass_ph);
   fChain->SetBranchAddress("el_L1_bestMatchDeltaR_ph", &el_L1_bestMatchDeltaR_ph, &b_el_L1_bestMatchDeltaR_ph);
   fChain->SetBranchAddress("el_L2_bestMatchDeltaR_ph", &el_L2_bestMatchDeltaR_ph, &b_el_L2_bestMatchDeltaR_ph);
   fChain->SetBranchAddress("el_EF_bestMatchDeltaR_ph", &el_EF_bestMatchDeltaR_ph, &b_el_EF_bestMatchDeltaR_ph);
   fChain->SetBranchAddress("el_L1_et", &el_L1_et, &b_el_L1_et);
   fChain->SetBranchAddress("el_L1_eta", &el_L1_eta, &b_el_L1_eta);
   fChain->SetBranchAddress("el_L1_phi", &el_L1_phi, &b_el_L1_phi);
   fChain->SetBranchAddress("el_L1_EMIsol", &el_L1_EMIsol, &b_el_L1_EMIsol);
   fChain->SetBranchAddress("el_L1_HadIsol", &el_L1_HadIsol, &b_el_L1_HadIsol);
   fChain->SetBranchAddress("el_L1_HadCore", &el_L1_HadCore, &b_el_L1_HadCore);
   fChain->SetBranchAddress("el_L2_e", &el_L2_e, &b_el_L2_e);
   fChain->SetBranchAddress("el_L2_pt", &el_L2_pt, &b_el_L2_pt);
   fChain->SetBranchAddress("el_L2_eta", &el_L2_eta, &b_el_L2_eta);
   fChain->SetBranchAddress("el_L2_phi", &el_L2_phi, &b_el_L2_phi);
   fChain->SetBranchAddress("el_L2_cl_e", &el_L2_cl_e, &b_el_L2_cl_e);
   fChain->SetBranchAddress("el_L2_cl_et", &el_L2_cl_et, &b_el_L2_cl_et);
   fChain->SetBranchAddress("el_L2_cl_eta", &el_L2_cl_eta, &b_el_L2_cl_eta);
   fChain->SetBranchAddress("el_L2_cl_phi", &el_L2_cl_phi, &b_el_L2_cl_phi);
   fChain->SetBranchAddress("el_L2_cl_e237", &el_L2_cl_e237, &b_el_L2_cl_e237);
   fChain->SetBranchAddress("el_L2_cl_e277", &el_L2_cl_e277, &b_el_L2_cl_e277);
   fChain->SetBranchAddress("el_L2_cl_fracs1", &el_L2_cl_fracs1, &b_el_L2_cl_fracs1);
   fChain->SetBranchAddress("el_L2_cl_weta2", &el_L2_cl_weta2, &b_el_L2_cl_weta2);
   fChain->SetBranchAddress("el_L2_cl_ehad1", &el_L2_cl_ehad1, &b_el_L2_cl_ehad1);
   fChain->SetBranchAddress("el_L2_cl_emaxs1", &el_L2_cl_emaxs1, &b_el_L2_cl_emaxs1);
   fChain->SetBranchAddress("el_L2_cl_e2tsts1", &el_L2_cl_e2tsts1, &b_el_L2_cl_e2tsts1);
   fChain->SetBranchAddress("el_L2_trkCl_etOverPt", &el_L2_trkCl_etOverPt, &b_el_L2_trkCl_etOverPt);
   fChain->SetBranchAddress("el_L2_trkCl_dEta", &el_L2_trkCl_dEta, &b_el_L2_trkCl_dEta);
   fChain->SetBranchAddress("el_L2_trkCl_dPhi", &el_L2_trkCl_dPhi, &b_el_L2_trkCl_dPhi);
   fChain->SetBranchAddress("el_L2_trk_pt", &el_L2_trk_pt, &b_el_L2_trk_pt);
   fChain->SetBranchAddress("el_L2_trk_errPt", &el_L2_trk_errPt, &b_el_L2_trk_errPt);
   fChain->SetBranchAddress("el_L2_trk_etaAtCalo", &el_L2_trk_etaAtCalo, &b_el_L2_trk_etaAtCalo);
   fChain->SetBranchAddress("el_L2_trk_errEta", &el_L2_trk_errEta, &b_el_L2_trk_errEta);
   fChain->SetBranchAddress("el_L2_trk_phiAtCalo", &el_L2_trk_phiAtCalo, &b_el_L2_trk_phiAtCalo);
   fChain->SetBranchAddress("el_L2_trk_errPhi", &el_L2_trk_errPhi, &b_el_L2_trk_errPhi);
   fChain->SetBranchAddress("el_L2_trk_charge", &el_L2_trk_charge, &b_el_L2_trk_charge);
   fChain->SetBranchAddress("el_L2_trk_algo", &el_L2_trk_algo, &b_el_L2_trk_algo);
   fChain->SetBranchAddress("el_L2_trk_zvtx", &el_L2_trk_zvtx, &b_el_L2_trk_zvtx);
   fChain->SetBranchAddress("el_L2_trk_errZvtx", &el_L2_trk_errZvtx, &b_el_L2_trk_errZvtx);
   fChain->SetBranchAddress("el_EF_e", &el_EF_e, &b_el_EF_e);
   fChain->SetBranchAddress("el_EF_et", &el_EF_et, &b_el_EF_et);
   fChain->SetBranchAddress("el_EF_eta", &el_EF_eta, &b_el_EF_eta);
   fChain->SetBranchAddress("el_EF_phi", &el_EF_phi, &b_el_EF_phi);
   fChain->SetBranchAddress("el_EF_author", &el_EF_author, &b_el_EF_author);
   fChain->SetBranchAddress("el_EF_IsEM", &el_EF_IsEM, &b_el_EF_IsEM);
   fChain->SetBranchAddress("el_EF_ethad", &el_EF_ethad, &b_el_EF_ethad);
   fChain->SetBranchAddress("el_EF_ethad1", &el_EF_ethad1, &b_el_EF_ethad1);
   fChain->SetBranchAddress("el_EF_e233", &el_EF_e233, &b_el_EF_e233);
   fChain->SetBranchAddress("el_EF_e237", &el_EF_e237, &b_el_EF_e237);
   fChain->SetBranchAddress("el_EF_e277", &el_EF_e277, &b_el_EF_e277);
   fChain->SetBranchAddress("el_EF_emaxs1", &el_EF_emaxs1, &b_el_EF_emaxs1);
   fChain->SetBranchAddress("el_EF_e2tsts1", &el_EF_e2tsts1, &b_el_EF_e2tsts1);
   fChain->SetBranchAddress("el_EF_fracs1", &el_EF_fracs1, &b_el_EF_fracs1);
   fChain->SetBranchAddress("el_EF_weta1", &el_EF_weta1, &b_el_EF_weta1);
   fChain->SetBranchAddress("el_EF_weta2", &el_EF_weta2, &b_el_EF_weta2);
   fChain->SetBranchAddress("el_EF_wtots1", &el_EF_wtots1, &b_el_EF_wtots1);
   fChain->SetBranchAddress("el_EF_f1", &el_EF_f1, &b_el_EF_f1);
   fChain->SetBranchAddress("el_EF_emins1", &el_EF_emins1, &b_el_EF_emins1);
   fChain->SetBranchAddress("el_EF_etcone20", &el_EF_etcone20, &b_el_EF_etcone20);
   fChain->SetBranchAddress("el_EF_dEta", &el_EF_dEta, &b_el_EF_dEta);
   fChain->SetBranchAddress("el_EF_dPhi", &el_EF_dPhi, &b_el_EF_dPhi);
   fChain->SetBranchAddress("el_EF_etOverPt", &el_EF_etOverPt, &b_el_EF_etOverPt);
   fChain->SetBranchAddress("el_EF_nBL", &el_EF_nBL, &b_el_EF_nBL);
   fChain->SetBranchAddress("el_EF_nPix", &el_EF_nPix, &b_el_EF_nPix);
   fChain->SetBranchAddress("el_EF_nSCT", &el_EF_nSCT, &b_el_EF_nSCT);
   fChain->SetBranchAddress("el_EF_nTRThigh", &el_EF_nTRThigh, &b_el_EF_nTRThigh);
   fChain->SetBranchAddress("el_EF_nTRThighOut", &el_EF_nTRThighOut, &b_el_EF_nTRThighOut);
   fChain->SetBranchAddress("el_EF_nTRT", &el_EF_nTRT, &b_el_EF_nTRT);
   fChain->SetBranchAddress("el_EF_nTRTOut", &el_EF_nTRTOut, &b_el_EF_nTRTOut);
   fChain->SetBranchAddress("el_EF_trkPt", &el_EF_trkPt, &b_el_EF_trkPt);
   fChain->SetBranchAddress("el_EF_trkD0", &el_EF_trkD0, &b_el_EF_trkD0);
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
   fChain->SetBranchAddress("mEt_topo_ex", &mEt_topo_ex, &b_mEt_topo_ex);
   fChain->SetBranchAddress("mEt_topo_ey", &mEt_topo_ey, &b_mEt_topo_ey);
   fChain->SetBranchAddress("mEt_topo_et", &mEt_topo_et, &b_mEt_topo_et);
   fChain->SetBranchAddress("mEt_topo_sumet", &mEt_topo_sumet, &b_mEt_topo_sumet);
   fChain->SetBranchAddress("mEt_Topo_central_ex", &mEt_Topo_central_ex, &b_mEt_Topo_central_ex);
   fChain->SetBranchAddress("mEt_Topo_central_ey", &mEt_Topo_central_ey, &b_mEt_Topo_central_ey);
   fChain->SetBranchAddress("mEt_Topo_central_sumet", &mEt_Topo_central_sumet, &b_mEt_Topo_central_sumet);
   fChain->SetBranchAddress("mEt_Topo_endcap_ex", &mEt_Topo_endcap_ex, &b_mEt_Topo_endcap_ex);
   fChain->SetBranchAddress("mEt_Topo_endcap_ey", &mEt_Topo_endcap_ey, &b_mEt_Topo_endcap_ey);
   fChain->SetBranchAddress("mEt_Topo_endcap_sumet", &mEt_Topo_endcap_sumet, &b_mEt_Topo_endcap_sumet);
   fChain->SetBranchAddress("mEt_Topo_forward_ex", &mEt_Topo_forward_ex, &b_mEt_Topo_forward_ex);
   fChain->SetBranchAddress("mEt_Topo_forward_ey", &mEt_Topo_forward_ey, &b_mEt_Topo_forward_ey);
   fChain->SetBranchAddress("mEt_Topo_forward_sumet", &mEt_Topo_forward_sumet, &b_mEt_Topo_forward_sumet);
   fChain->SetBranchAddress("mEt_Topo_PEMB_ex", &mEt_Topo_PEMB_ex, &b_mEt_Topo_PEMB_ex);
   fChain->SetBranchAddress("mEt_Topo_PEMB_ey", &mEt_Topo_PEMB_ey, &b_mEt_Topo_PEMB_ey);
   fChain->SetBranchAddress("mEt_Topo_PEMB_ncell", &mEt_Topo_PEMB_ncell, &b_mEt_Topo_PEMB_ncell);
   fChain->SetBranchAddress("mEt_Topo_PEMB_sumet", &mEt_Topo_PEMB_sumet, &b_mEt_Topo_PEMB_sumet);
   fChain->SetBranchAddress("mEt_Topo_EMB_ex", &mEt_Topo_EMB_ex, &b_mEt_Topo_EMB_ex);
   fChain->SetBranchAddress("mEt_Topo_EMB_ey", &mEt_Topo_EMB_ey, &b_mEt_Topo_EMB_ey);
   fChain->SetBranchAddress("mEt_Topo_EMB_ncell", &mEt_Topo_EMB_ncell, &b_mEt_Topo_EMB_ncell);
   fChain->SetBranchAddress("mEt_Topo_EMB_sumet", &mEt_Topo_EMB_sumet, &b_mEt_Topo_EMB_sumet);
   fChain->SetBranchAddress("mEt_Topo_PEMEC_ex", &mEt_Topo_PEMEC_ex, &b_mEt_Topo_PEMEC_ex);
   fChain->SetBranchAddress("mEt_Topo_PEMEC_ey", &mEt_Topo_PEMEC_ey, &b_mEt_Topo_PEMEC_ey);
   fChain->SetBranchAddress("mEt_Topo_PEMEC_ncell", &mEt_Topo_PEMEC_ncell, &b_mEt_Topo_PEMEC_ncell);
   fChain->SetBranchAddress("mEt_Topo_PEMEC_sumet", &mEt_Topo_PEMEC_sumet, &b_mEt_Topo_PEMEC_sumet);
   fChain->SetBranchAddress("mEt_Topo_EME_ex", &mEt_Topo_EME_ex, &b_mEt_Topo_EME_ex);
   fChain->SetBranchAddress("mEt_Topo_EME_ey", &mEt_Topo_EME_ey, &b_mEt_Topo_EME_ey);
   fChain->SetBranchAddress("mEt_Topo_EME_ncell", &mEt_Topo_EME_ncell, &b_mEt_Topo_EME_ncell);
   fChain->SetBranchAddress("mEt_Topo_EME_sumet", &mEt_Topo_EME_sumet, &b_mEt_Topo_EME_sumet);
   fChain->SetBranchAddress("mEt_Topo_TILE_ex", &mEt_Topo_TILE_ex, &b_mEt_Topo_TILE_ex);
   fChain->SetBranchAddress("mEt_Topo_TILE_ey", &mEt_Topo_TILE_ey, &b_mEt_Topo_TILE_ey);
   fChain->SetBranchAddress("mEt_Topo_TILE_ncell", &mEt_Topo_TILE_ncell, &b_mEt_Topo_TILE_ncell);
   fChain->SetBranchAddress("mEt_Topo_TILE_sumet", &mEt_Topo_TILE_sumet, &b_mEt_Topo_TILE_sumet);
   fChain->SetBranchAddress("mEt_Topo_HEC_ex", &mEt_Topo_HEC_ex, &b_mEt_Topo_HEC_ex);
   fChain->SetBranchAddress("mEt_Topo_HEC_ey", &mEt_Topo_HEC_ey, &b_mEt_Topo_HEC_ey);
   fChain->SetBranchAddress("mEt_Topo_HEC_ncell", &mEt_Topo_HEC_ncell, &b_mEt_Topo_HEC_ncell);
   fChain->SetBranchAddress("mEt_Topo_HEC_sumet", &mEt_Topo_HEC_sumet, &b_mEt_Topo_HEC_sumet);
   fChain->SetBranchAddress("mEt_Topo_FCAL_ex", &mEt_Topo_FCAL_ex, &b_mEt_Topo_FCAL_ex);
   fChain->SetBranchAddress("mEt_Topo_FCAL_ey", &mEt_Topo_FCAL_ey, &b_mEt_Topo_FCAL_ey);
   fChain->SetBranchAddress("mEt_Topo_FCAL_ncell", &mEt_Topo_FCAL_ncell, &b_mEt_Topo_FCAL_ncell);
   fChain->SetBranchAddress("mEt_Topo_FCAL_sumet", &mEt_Topo_FCAL_sumet, &b_mEt_Topo_FCAL_sumet);
   fChain->SetBranchAddress("mEt_LocHadtopo_ex", &mEt_LocHadtopo_ex, &b_mEt_LocHadtopo_ex);
   fChain->SetBranchAddress("mEt_LocHadtopo_ey", &mEt_LocHadtopo_ey, &b_mEt_LocHadtopo_ey);
   fChain->SetBranchAddress("mEt_LocHadtopo_sumet", &mEt_LocHadtopo_sumet, &b_mEt_LocHadtopo_sumet);
   fChain->SetBranchAddress("mEt_LocHadtopo_et", &mEt_LocHadtopo_et, &b_mEt_LocHadtopo_et);
   fChain->SetBranchAddress("mEt_LocHadtopo_central_ex", &mEt_LocHadtopo_central_ex, &b_mEt_LocHadtopo_central_ex);
   fChain->SetBranchAddress("mEt_LocHadtopo_central_ey", &mEt_LocHadtopo_central_ey, &b_mEt_LocHadtopo_central_ey);
   fChain->SetBranchAddress("mEt_LocHadtopo_central_sumet", &mEt_LocHadtopo_central_sumet, &b_mEt_LocHadtopo_central_sumet);
   fChain->SetBranchAddress("mEt_LocHadtopo_endcap_ex", &mEt_LocHadtopo_endcap_ex, &b_mEt_LocHadtopo_endcap_ex);
   fChain->SetBranchAddress("mEt_LocHadtopo_endcap_ey", &mEt_LocHadtopo_endcap_ey, &b_mEt_LocHadtopo_endcap_ey);
   fChain->SetBranchAddress("mEt_LocHadtopo_endcap_sumet", &mEt_LocHadtopo_endcap_sumet, &b_mEt_LocHadtopo_endcap_sumet);
   fChain->SetBranchAddress("mEt_LocHadtopo_forward_ex", &mEt_LocHadtopo_forward_ex, &b_mEt_LocHadtopo_forward_ex);
   fChain->SetBranchAddress("mEt_LocHadtopo_forward_ey", &mEt_LocHadtopo_forward_ey, &b_mEt_LocHadtopo_forward_ey);
   fChain->SetBranchAddress("mEt_LocHadtopo_forward_sumet", &mEt_LocHadtopo_forward_sumet, &b_mEt_LocHadtopo_forward_sumet);
   fChain->SetBranchAddress("mEt_LocHadtopo_PEMB_ex", &mEt_LocHadtopo_PEMB_ex, &b_mEt_LocHadtopo_PEMB_ex);
   fChain->SetBranchAddress("mEt_LocHadtopo_PEMB_ey", &mEt_LocHadtopo_PEMB_ey, &b_mEt_LocHadtopo_PEMB_ey);
   fChain->SetBranchAddress("mEt_LocHadtopo_PEMB_ncell", &mEt_LocHadtopo_PEMB_ncell, &b_mEt_LocHadtopo_PEMB_ncell);
   fChain->SetBranchAddress("mEt_LocHadtopo_PEMB_sumet", &mEt_LocHadtopo_PEMB_sumet, &b_mEt_LocHadtopo_PEMB_sumet);
   fChain->SetBranchAddress("mEt_LocHadtopo_EMB_ex", &mEt_LocHadtopo_EMB_ex, &b_mEt_LocHadtopo_EMB_ex);
   fChain->SetBranchAddress("mEt_LocHadtopo_EMB_ey", &mEt_LocHadtopo_EMB_ey, &b_mEt_LocHadtopo_EMB_ey);
   fChain->SetBranchAddress("mEt_LocHadtopo_EMB_ncell", &mEt_LocHadtopo_EMB_ncell, &b_mEt_LocHadtopo_EMB_ncell);
   fChain->SetBranchAddress("mEt_LocHadtopo_EMB_sumet", &mEt_LocHadtopo_EMB_sumet, &b_mEt_LocHadtopo_EMB_sumet);
   fChain->SetBranchAddress("mEt_LocHadtopo_PEMEC_ex", &mEt_LocHadtopo_PEMEC_ex, &b_mEt_LocHadtopo_PEMEC_ex);
   fChain->SetBranchAddress("mEt_LocHadtopo_PEMEC_ey", &mEt_LocHadtopo_PEMEC_ey, &b_mEt_LocHadtopo_PEMEC_ey);
   fChain->SetBranchAddress("mEt_LocHadtopo_PEMEC_ncell", &mEt_LocHadtopo_PEMEC_ncell, &b_mEt_LocHadtopo_PEMEC_ncell);
   fChain->SetBranchAddress("mEt_LocHadtopo_PEMEC_sumet", &mEt_LocHadtopo_PEMEC_sumet, &b_mEt_LocHadtopo_PEMEC_sumet);
   fChain->SetBranchAddress("mEt_LocHadtopo_EME_ex", &mEt_LocHadtopo_EME_ex, &b_mEt_LocHadtopo_EME_ex);
   fChain->SetBranchAddress("mEt_LocHadtopo_EME_ey", &mEt_LocHadtopo_EME_ey, &b_mEt_LocHadtopo_EME_ey);
   fChain->SetBranchAddress("mEt_LocHadtopo_EME_ncell", &mEt_LocHadtopo_EME_ncell, &b_mEt_LocHadtopo_EME_ncell);
   fChain->SetBranchAddress("mEt_LocHadtopo_EME_sumet", &mEt_LocHadtopo_EME_sumet, &b_mEt_LocHadtopo_EME_sumet);
   fChain->SetBranchAddress("mEt_LocHadtopo_TILE_ex", &mEt_LocHadtopo_TILE_ex, &b_mEt_LocHadtopo_TILE_ex);
   fChain->SetBranchAddress("mEt_LocHadtopo_TILE_ey", &mEt_LocHadtopo_TILE_ey, &b_mEt_LocHadtopo_TILE_ey);
   fChain->SetBranchAddress("mEt_LocHadtopo_TILE_ncell", &mEt_LocHadtopo_TILE_ncell, &b_mEt_LocHadtopo_TILE_ncell);
   fChain->SetBranchAddress("mEt_LocHadtopo_TILE_sumet", &mEt_LocHadtopo_TILE_sumet, &b_mEt_LocHadtopo_TILE_sumet);
   fChain->SetBranchAddress("mEt_LocHadtopo_HEC_ex", &mEt_LocHadtopo_HEC_ex, &b_mEt_LocHadtopo_HEC_ex);
   fChain->SetBranchAddress("mEt_LocHadtopo_HEC_ey", &mEt_LocHadtopo_HEC_ey, &b_mEt_LocHadtopo_HEC_ey);
   fChain->SetBranchAddress("mEt_LocHadtopo_HEC_ncell", &mEt_LocHadtopo_HEC_ncell, &b_mEt_LocHadtopo_HEC_ncell);
   fChain->SetBranchAddress("mEt_LocHadtopo_HEC_sumet", &mEt_LocHadtopo_HEC_sumet, &b_mEt_LocHadtopo_HEC_sumet);
   fChain->SetBranchAddress("mEt_LocHadtopo_FCAL_ex", &mEt_LocHadtopo_FCAL_ex, &b_mEt_LocHadtopo_FCAL_ex);
   fChain->SetBranchAddress("mEt_LocHadtopo_FCAL_ey", &mEt_LocHadtopo_FCAL_ey, &b_mEt_LocHadtopo_FCAL_ey);
   fChain->SetBranchAddress("mEt_LocHadtopo_FCAL_ncell", &mEt_LocHadtopo_FCAL_ncell, &b_mEt_LocHadtopo_FCAL_ncell);
   fChain->SetBranchAddress("mEt_LocHadtopo_FCAL_sumet", &mEt_LocHadtopo_FCAL_sumet, &b_mEt_LocHadtopo_FCAL_sumet);
   fChain->SetBranchAddress("mEt_MuonBoy_Track_ex", &mEt_MuonBoy_Track_ex, &b_mEt_MuonBoy_Track_ex);
   fChain->SetBranchAddress("mEt_MuonBoy_Track_ey", &mEt_MuonBoy_Track_ey, &b_mEt_MuonBoy_Track_ey);
   fChain->SetBranchAddress("mEt_MuonBoy_Track_sumet", &mEt_MuonBoy_Track_sumet, &b_mEt_MuonBoy_Track_sumet);
   fChain->SetBranchAddress("mEt_MuonBoy_ex", &mEt_MuonBoy_ex, &b_mEt_MuonBoy_ex);
   fChain->SetBranchAddress("mEt_MuonBoy_ey", &mEt_MuonBoy_ey, &b_mEt_MuonBoy_ey);
   fChain->SetBranchAddress("mEt_MuonBoy_sumet", &mEt_MuonBoy_sumet, &b_mEt_MuonBoy_sumet);
   fChain->SetBranchAddress("mEt_MuID_ex", &mEt_MuID_ex, &b_mEt_MuID_ex);
   fChain->SetBranchAddress("mEt_MuID_ey", &mEt_MuID_ey, &b_mEt_MuID_ey);
   fChain->SetBranchAddress("mEt_MuID_sumet", &mEt_MuID_sumet, &b_mEt_MuID_sumet);
   fChain->SetBranchAddress("mEt_Cryo_ex", &mEt_Cryo_ex, &b_mEt_Cryo_ex);
   fChain->SetBranchAddress("mEt_Cryo_ey", &mEt_Cryo_ey, &b_mEt_Cryo_ey);
   fChain->SetBranchAddress("mEt_Cryo_sumet", &mEt_Cryo_sumet, &b_mEt_Cryo_sumet);
   fChain->SetBranchAddress("mEt_RefEle_ex", &mEt_RefEle_ex, &b_mEt_RefEle_ex);
   fChain->SetBranchAddress("mEt_RefEle_ey", &mEt_RefEle_ey, &b_mEt_RefEle_ey);
   fChain->SetBranchAddress("mEt_RefEle_sumet", &mEt_RefEle_sumet, &b_mEt_RefEle_sumet);
   fChain->SetBranchAddress("mEt_RefGamma_ex", &mEt_RefGamma_ex, &b_mEt_RefGamma_ex);
   fChain->SetBranchAddress("mEt_RefGamma_ey", &mEt_RefGamma_ey, &b_mEt_RefGamma_ey);
   fChain->SetBranchAddress("mEt_RefGamma_sumet", &mEt_RefGamma_sumet, &b_mEt_RefGamma_sumet);
   fChain->SetBranchAddress("mEt_RefTau_ex", &mEt_RefTau_ex, &b_mEt_RefTau_ex);
   fChain->SetBranchAddress("mEt_RefTau_ey", &mEt_RefTau_ey, &b_mEt_RefTau_ey);
   fChain->SetBranchAddress("mEt_RefTau_sumet", &mEt_RefTau_sumet, &b_mEt_RefTau_sumet);
   fChain->SetBranchAddress("mEt_RefJet_ex", &mEt_RefJet_ex, &b_mEt_RefJet_ex);
   fChain->SetBranchAddress("mEt_RefJet_ey", &mEt_RefJet_ey, &b_mEt_RefJet_ey);
   fChain->SetBranchAddress("mEt_RefJet_sumet", &mEt_RefJet_sumet, &b_mEt_RefJet_sumet);
   fChain->SetBranchAddress("mEt_RefMuon_ex", &mEt_RefMuon_ex, &b_mEt_RefMuon_ex);
   fChain->SetBranchAddress("mEt_RefMuon_ey", &mEt_RefMuon_ey, &b_mEt_RefMuon_ey);
   fChain->SetBranchAddress("mEt_RefMuon_sumet", &mEt_RefMuon_sumet, &b_mEt_RefMuon_sumet);
   fChain->SetBranchAddress("mEt_RefMuon_Track_ex", &mEt_RefMuon_Track_ex, &b_mEt_RefMuon_Track_ex);
   fChain->SetBranchAddress("mEt_RefMuon_Track_ey", &mEt_RefMuon_Track_ey, &b_mEt_RefMuon_Track_ey);
   fChain->SetBranchAddress("mEt_RefMuon_Track_sumet", &mEt_RefMuon_Track_sumet, &b_mEt_RefMuon_Track_sumet);
   fChain->SetBranchAddress("mEt_CellOut_ex", &mEt_CellOut_ex, &b_mEt_CellOut_ex);
   fChain->SetBranchAddress("mEt_CellOut_ey", &mEt_CellOut_ey, &b_mEt_CellOut_ey);
   fChain->SetBranchAddress("mEt_CellOut_sumet", &mEt_CellOut_sumet, &b_mEt_CellOut_sumet);
   fChain->SetBranchAddress("mEt_RefFinal_ex", &mEt_RefFinal_ex, &b_mEt_RefFinal_ex);
   fChain->SetBranchAddress("mEt_RefFinal_ey", &mEt_RefFinal_ey, &b_mEt_RefFinal_ey);
   fChain->SetBranchAddress("mEt_RefFinal_sumet", &mEt_RefFinal_sumet, &b_mEt_RefFinal_sumet);
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
   fChain->SetBranchAddress("el_CaloPointing_zvertex", &el_CaloPointing_zvertex, &b_el_CaloPointing_zvertex);
   fChain->SetBranchAddress("el_CaloPointing_sigma_zvertex", &el_CaloPointing_sigma_zvertex, &b_el_CaloPointing_sigma_zvertex);
   fChain->SetBranchAddress("el_CaloPointing_eta", &el_CaloPointing_eta, &b_el_CaloPointing_eta);
   fChain->SetBranchAddress("el_CaloPointing_sigma_eta", &el_CaloPointing_sigma_eta, &b_el_CaloPointing_sigma_eta);
   fChain->SetBranchAddress("ph_CaloPointing_zvertex", &ph_CaloPointing_zvertex, &b_ph_CaloPointing_zvertex);
   fChain->SetBranchAddress("ph_CaloPointing_sigma_zvertex", &ph_CaloPointing_sigma_zvertex, &b_ph_CaloPointing_sigma_zvertex);
   fChain->SetBranchAddress("ph_CaloPointing_eta", &ph_CaloPointing_eta, &b_ph_CaloPointing_eta);
   fChain->SetBranchAddress("ph_CaloPointing_sigma_eta", &ph_CaloPointing_sigma_eta, &b_ph_CaloPointing_sigma_eta);
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
   fChain->SetBranchAddress("test_ph_E_calopoint", &test_ph_E_calopoint, &b_test_ph_E_calopoint);
   fChain->SetBranchAddress("test_ph_eta_calopoint", &test_ph_eta_calopoint, &b_test_ph_eta_calopoint);
   fChain->SetBranchAddress("test_ph_phi_calopoint", &test_ph_phi_calopoint, &b_test_ph_phi_calopoint);
   fChain->SetBranchAddress("test_ph_pt_calopoint", &test_ph_pt_calopoint, &b_test_ph_pt_calopoint);
   fChain->SetBranchAddress("ph_particleType", &ph_particleType, &b_ph_particleType);
   fChain->SetBranchAddress("ph_particleOrig", &ph_particleOrig, &b_ph_particleOrig);
   fChain->SetBranchAddress("clus_SW_eta", &clus_SW_eta, &b_clus_SW_eta);
   fChain->SetBranchAddress("clus_SW_phi", &clus_SW_phi, &b_clus_SW_phi);
   fChain->SetBranchAddress("clus_SW_pt", &clus_SW_pt, &b_clus_SW_pt);
   fChain->SetBranchAddress("clus_SW_E", &clus_SW_E, &b_clus_SW_E);
   fChain->SetBranchAddress("clus_SW_size", &clus_SW_size, &b_clus_SW_size);
   fChain->SetBranchAddress("clus_SW_etasize", &clus_SW_etasize, &b_clus_SW_etasize);
   fChain->SetBranchAddress("clus_SW_phisize", &clus_SW_phisize, &b_clus_SW_phisize);
   fChain->SetBranchAddress("clus_SW_time", &clus_SW_time, &b_clus_SW_time);
   fChain->SetBranchAddress("clus_SW_eta_MC", &clus_SW_eta_MC, &b_clus_SW_eta_MC);
   fChain->SetBranchAddress("clus_SW_phi_MC", &clus_SW_phi_MC, &b_clus_SW_phi_MC);
   fChain->SetBranchAddress("clus_SW_pt_MC", &clus_SW_pt_MC, &b_clus_SW_pt_MC);
   fChain->SetBranchAddress("clus_SW_E_MC", &clus_SW_E_MC, &b_clus_SW_E_MC);
   fChain->SetBranchAddress("clus_SW_PDG", &clus_SW_PDG, &b_clus_SW_PDG);
   fChain->SetBranchAddress("clus_SW_motherPDG", &clus_SW_motherPDG, &b_clus_SW_motherPDG);
   fChain->SetBranchAddress("clus_EMtopo_eta", &clus_EMtopo_eta, &b_clus_EMtopo_eta);
   fChain->SetBranchAddress("clus_EMtopo_phi", &clus_EMtopo_phi, &b_clus_EMtopo_phi);
   fChain->SetBranchAddress("clus_EMtopo_pt", &clus_EMtopo_pt, &b_clus_EMtopo_pt);
   fChain->SetBranchAddress("clus_EMtopo_E", &clus_EMtopo_E, &b_clus_EMtopo_E);
   fChain->SetBranchAddress("clus_EMtopo_time", &clus_EMtopo_time, &b_clus_EMtopo_time);
   fChain->SetBranchAddress("clus_EMtopo_eta_MC", &clus_EMtopo_eta_MC, &b_clus_EMtopo_eta_MC);
   fChain->SetBranchAddress("clus_EMtopo_phi_MC", &clus_EMtopo_phi_MC, &b_clus_EMtopo_phi_MC);
   fChain->SetBranchAddress("clus_EMtopo_pt_MC", &clus_EMtopo_pt_MC, &b_clus_EMtopo_pt_MC);
   fChain->SetBranchAddress("clus_EMtopo_E_MC", &clus_EMtopo_E_MC, &b_clus_EMtopo_E_MC);
   fChain->SetBranchAddress("clus_EMtopo_PDG", &clus_EMtopo_PDG, &b_clus_EMtopo_PDG);
   fChain->SetBranchAddress("clus_EMtopo_motherPDG", &clus_EMtopo_motherPDG, &b_clus_EMtopo_motherPDG);
   fChain->SetBranchAddress("clus_EMtopo_iRecPhoton", &clus_EMtopo_iRecPhoton, &b_clus_EMtopo_iRecPhoton);
   fChain->SetBranchAddress("clus_EMtopo_deltaR_RecPhoton", &clus_EMtopo_deltaR_RecPhoton, &b_clus_EMtopo_deltaR_RecPhoton);
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
   fChain->SetBranchAddress("clus_topo420_E", &clus_topo420_E, &b_clus_topo420_E);
   fChain->SetBranchAddress("clus_lhtopo420_E", &clus_lhtopo420_E, &b_clus_lhtopo420_E);
   fChain->SetBranchAddress("clus_lhtopo420_phi", &clus_lhtopo420_phi, &b_clus_lhtopo420_phi);
   fChain->SetBranchAddress("clus_lhtopo420_eta", &clus_lhtopo420_eta, &b_clus_lhtopo420_eta);
   fChain->SetBranchAddress("UED_diphomass", &UED_diphomass, &b_UED_diphomass);
   fChain->SetBranchAddress("UED_diphoang", &UED_diphoang, &b_UED_diphoang);
   Notify();
}

Bool_t PAUEffFF::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void PAUEffFF::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t PAUEffFF::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef PAUEffFF_cxx
