/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Feb  3 16:02:09 2010 by ROOT version 5.25/04
// from TTree tree/Combined Ntuple
// found on file: cbnt_RecExTB_Combined_1207_2102095.0.root
//////////////////////////////////////////////////////////

#ifndef TB_tree_h
#define TB_tree_h

#include "CLIDSvc/CLASS_DEF.h"

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TString.h"

class TB_tree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Run;
   Int_t           Event;
   Int_t           Time;
   Double_t        Weight;
   Int_t           IEvent;
   Double_t        RunEta;
   Int_t           RunPartType;
   Int_t           RunEnergy;
   Float_t         Energy;
   Float_t         ErrAbsEnergy;
   Float_t         ErrCollimators;
   Float_t         ErrCurrents;
   Float_t         SyncLoss;
   Float_t         ErrSyncLoss;
   Int_t           Quad_nc;
   Float_t         Quad_file[22];   //[Quad_nc]
   Float_t         Quad_equip[22];   //[Quad_nc]
   Int_t           Bend_nc;
   Float_t         Bend_file[9];   //[Bend_nc]
   Float_t         Bend_equip[9];   //[Bend_nc]
   Int_t           Trim_nc;
   Float_t         Trim_file[10];   //[Trim_nc]
   Float_t         Trim_equip[10];   //[Trim_nc]
   Int_t           Coll_nc;
   Float_t         Coll_file[24];   //[Coll_nc]
   Float_t         Coll_equip[24];   //[Coll_nc]
   Float_t         sADC_S0;
   Float_t         sTDC_S0;
   Float_t         sADC_S1;
   Float_t         sTDC_S1;
   Float_t         sADC_S2mUp;
   Float_t         sTDC_S2mUp;
   Float_t         sADC_S2mDown;
   Float_t         sTDC_S2mDown;
   Float_t         sADC_S3mLeft;
   Float_t         sTDC_S3mLeft;
   Float_t         sADC_S3mRight;
   Float_t         sTDC_S3mRight;
   Float_t         sADC_C1;
   Float_t         sTDC_C1;
   Float_t         sADC_C2;
   Float_t         sTDC_C2;
   Float_t         sADC_muTag;
   Float_t         sTDC_muTag;
   Float_t         sADC_muHalo;
   Float_t         sTDC_muHalo;
   Float_t         sADC_muVeto;
   Float_t         sTDC_muVeto;
   Float_t         sADC_SC1;
   Float_t         sTDC_SC1;
   Float_t         sADC_SC2;
   Float_t         sTDC_SC2;
   Float_t         sADC_TRTSci;
   Float_t         sTDC_TRTSci;
   Float_t         bpcX_BPCm2;
   Float_t         bpcY_BPCm2;
   Float_t         bpcX_BPCm1;
   Float_t         bpcY_BPCm1;
   Float_t         bpcX_BPC0;
   Float_t         bpcY_BPC0;
   Float_t         bpcX_BPC1;
   Float_t         bpcY_BPC1;
   Float_t         bpcX_BPC2;
   Float_t         bpcY_BPC2;
   Int_t           Trigger;
   Float_t         Clock;
   Int_t           Nh_Calo;
   Float_t         Eh_Calo;
   Int_t           Nh_EM;
   Float_t         Eh_EM;
   Int_t           Nh_HAD;
   Float_t         Eh_HAD;
   Int_t           Nh_PresB;
   Float_t         Eh_PresB;
   Int_t           Nh_EMB;
   Float_t         Eh_EMB[3];
   Int_t           Nh_EMEC;
   Float_t         Eh_EMEC[3];
   Int_t           Nh_Tile;
   Float_t         Eh_Tile[4];
   Int_t           Nh_TileGap;
   Float_t         Eh_TileGap;
   Int_t           Nh_HEC;
   Float_t         Eh_HEC[4];
   Int_t           Nh_FCal;
   Float_t         Eh_FCal[3];
   Int_t           Nh_PresE;
   Float_t         Eh_PresE;
   Int_t           Nh_Scint;
   Float_t         Eh_Scint;
   Int_t           nhit_lar;
   Float_t         ecell_lar;
   Int_t           NCells_lar;
   Float_t         ECells_lar[40000];   //[NCells_lar]
   Float_t         EtaCells_lar[40000];   //[NCells_lar]
   Float_t         PhiCells_lar[40000];   //[NCells_lar]
   Float_t         QCells_lar[40000];   //[NCells_lar]
   Int_t           GainCells_lar[40000];   //[NCells_lar]
   Int_t           DetCells_lar[40000];   //[NCells_lar]
   Float_t         TimeCells_lar[40000];   //[NCells_lar]
   Int_t           BadCells_lar[40000];   //[NCells_lar]
   Int_t           nhit_tile;
   Float_t         ecell_tile;
   Int_t           NCells_tile;
   Float_t         ECells_tile[40000];   //[NCells_tile]
   Float_t         EtaCells_tile[40000];   //[NCells_tile]
   Float_t         PhiCells_tile[40000];   //[NCells_tile]
   Float_t         QCells_tile[40000];   //[NCells_tile]
   Int_t           GainCells_tile[40000];   //[NCells_tile]
   Int_t           DetCells_tile[40000];   //[NCells_tile]
   Float_t         TimeCells_tile[40000];   //[NCells_tile]
   Float_t         cl_ecluster_em;
   Int_t           cl_nctotal_em;
   Int_t           cl_nc_em;
   Float_t         cl_et_em[30];   //[cl_nc_em]
   Float_t         cl_e_em[30];   //[cl_nc_em]
   Float_t         cl_eemb0_em[30];   //[cl_nc_em]
   Float_t         cl_eemb1_em[30];   //[cl_nc_em]
   Float_t         cl_eemb2_em[30];   //[cl_nc_em]
   Float_t         cl_eemb3_em[30];   //[cl_nc_em]
   Float_t         cl_phi2_em[30];   //[cl_nc_em]
   Float_t         cl_eta0_em[30];   //[cl_nc_em]
   Float_t         cl_eta1_em[30];   //[cl_nc_em]
   Float_t         cl_eta2_em[30];   //[cl_nc_em]
   Float_t         cl_eta3_em[30];   //[cl_nc_em]
   Float_t         cl_etileg1_em[30];   //[cl_nc_em]
   Float_t         cl_etileg2_em[30];   //[cl_nc_em]
   Float_t         cl_etileg3_em[30];   //[cl_nc_em]
   Float_t         cl_eta_em[30];   //[cl_nc_em]
   Float_t         cl_phi_em[30];   //[cl_nc_em]
   Int_t           cl_reco_stat_em[30];   //[cl_nc_em]
   Float_t         cl_m1_eta_em[30];   //[cl_nc_em]
   Float_t         cl_m1_phi_em[30];   //[cl_nc_em]
   Float_t         cl_m2_r_em[30];   //[cl_nc_em]
   Float_t         cl_m2_lambda_em[30];   //[cl_nc_em]
   Float_t         cl_delta_phi_em[30];   //[cl_nc_em]
   Float_t         cl_delta_theta_em[30];   //[cl_nc_em]
   Float_t         cl_delta_alpha_em[30];   //[cl_nc_em]
   Float_t         cl_center_x_em[30];   //[cl_nc_em]
   Float_t         cl_center_y_em[30];   //[cl_nc_em]
   Float_t         cl_center_z_em[30];   //[cl_nc_em]
   Float_t         cl_center_lambda_em[30];   //[cl_nc_em]
   Float_t         cl_lateral_em[30];   //[cl_nc_em]
   Float_t         cl_longitudinal_em[30];   //[cl_nc_em]
   Float_t         cl_eng_frac_em_em[30];   //[cl_nc_em]
   Float_t         cl_eng_frac_max_em[30];   //[cl_nc_em]
   Float_t         cl_eng_frac_core_em[30];   //[cl_nc_em]
   Float_t         cl_m1_dens_em[30];   //[cl_nc_em]
   Float_t         cl_m2_dens_em[30];   //[cl_nc_em]
   Int_t           cl_ntotcells_em[30];   //[cl_nc_em]
   Int_t           cl_cells_em[30][200];   //[cl_nc_em]
   Float_t         cl_ecluster_tb_em;
   Int_t           cl_nctotal_tb_em;
   Int_t           cl_nc_tb_em;
   Float_t         cl_et_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_e_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eemb0_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eemb1_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eemb2_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eemb3_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_phi2_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eta0_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eta1_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eta2_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eta3_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_etileg1_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_etileg2_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_etileg3_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eta_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_phi_tb_em[30];   //[cl_nc_tb_em]
   Int_t           cl_reco_stat_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_m1_eta_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_m1_phi_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_m2_r_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_m2_lambda_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_delta_phi_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_delta_theta_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_delta_alpha_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_center_x_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_center_y_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_center_z_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_center_lambda_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_lateral_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_longitudinal_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eng_frac_em_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eng_frac_max_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_eng_frac_core_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_m1_dens_tb_em[30];   //[cl_nc_tb_em]
   Float_t         cl_m2_dens_tb_em[30];   //[cl_nc_tb_em]
   Int_t           cl_ntotcells_tb_em[30];   //[cl_nc_tb_em]
   Int_t           cl_cells_tb_em[30][200];   //[cl_nc_tb_em]
   Float_t         cl_ecluster_topo;
   Int_t           cl_nctotal_topo;
   Int_t           cl_nc_topo;
   Float_t         cl_et_topo[30];   //[cl_nc_topo]
   Float_t         cl_e_topo[30];   //[cl_nc_topo]
   Float_t         cl_eemb0_topo[30];   //[cl_nc_topo]
   Float_t         cl_eemb1_topo[30];   //[cl_nc_topo]
   Float_t         cl_eemb2_topo[30];   //[cl_nc_topo]
   Float_t         cl_eemb3_topo[30];   //[cl_nc_topo]
   Int_t           cl_nemb0_topo[30];   //[cl_nc_topo]
   Int_t           cl_nemb1_topo[30];   //[cl_nc_topo]
   Int_t           cl_nemb2_topo[30];   //[cl_nc_topo]
   Int_t           cl_nemb3_topo[30];   //[cl_nc_topo]
   Float_t         cl_phi2_topo[30];   //[cl_nc_topo]
   Float_t         cl_eta0_topo[30];   //[cl_nc_topo]
   Float_t         cl_eta1_topo[30];   //[cl_nc_topo]
   Float_t         cl_eta2_topo[30];   //[cl_nc_topo]
   Float_t         cl_eta3_topo[30];   //[cl_nc_topo]
   Float_t         cl_etileg1_topo[30];   //[cl_nc_topo]
   Float_t         cl_etileg2_topo[30];   //[cl_nc_topo]
   Float_t         cl_etileg3_topo[30];   //[cl_nc_topo]
   Int_t           cl_ntileg1_topo[30];   //[cl_nc_topo]
   Int_t           cl_ntileg2_topo[30];   //[cl_nc_topo]
   Int_t           cl_ntileg3_topo[30];   //[cl_nc_topo]
   Float_t         cl_eta_topo[30];   //[cl_nc_topo]
   Float_t         cl_phi_topo[30];   //[cl_nc_topo]
   Int_t           cl_reco_stat_topo[30];   //[cl_nc_topo]
   Float_t         cl_m1_eta_topo[30];   //[cl_nc_topo]
   Float_t         cl_m1_phi_topo[30];   //[cl_nc_topo]
   Float_t         cl_m2_r_topo[30];   //[cl_nc_topo]
   Float_t         cl_m2_lambda_topo[30];   //[cl_nc_topo]
   Float_t         cl_delta_phi_topo[30];   //[cl_nc_topo]
   Float_t         cl_delta_theta_topo[30];   //[cl_nc_topo]
   Float_t         cl_delta_alpha_topo[30];   //[cl_nc_topo]
   Float_t         cl_center_x_topo[30];   //[cl_nc_topo]
   Float_t         cl_center_y_topo[30];   //[cl_nc_topo]
   Float_t         cl_center_z_topo[30];   //[cl_nc_topo]
   Float_t         cl_center_lambda_topo[30];   //[cl_nc_topo]
   Float_t         cl_lateral_topo[30];   //[cl_nc_topo]
   Float_t         cl_longitudinal_topo[30];   //[cl_nc_topo]
   Float_t         cl_eng_frac_em_topo[30];   //[cl_nc_topo]
   Float_t         cl_eng_frac_max_topo[30];   //[cl_nc_topo]
   Float_t         cl_eng_frac_core_topo[30];   //[cl_nc_topo]
   Float_t         cl_m1_dens_topo[30];   //[cl_nc_topo]
   Float_t         cl_m2_dens_topo[30];   //[cl_nc_topo]
   Float_t         cl_etileb0_topo[30];   //[cl_nc_topo]
   Float_t         cl_etileb1_topo[30];   //[cl_nc_topo]
   Float_t         cl_etileb2_topo[30];   //[cl_nc_topo]
   Int_t           cl_ntileb0_topo[30];   //[cl_nc_topo]
   Int_t           cl_ntileb1_topo[30];   //[cl_nc_topo]
   Int_t           cl_ntileb2_topo[30];   //[cl_nc_topo]
   Int_t           cl_ntotcells_topo[30];   //[cl_nc_topo]
   Int_t           cl_cells_topo[30][200];   //[cl_nc_topo]
   Int_t           ncellstotal_topo;
   Int_t           ncells_topo;
   Float_t         celleng_topo[9000];   //[ncells_topo]
   Float_t         cellet_topo[9000];   //[ncells_topo]
   Float_t         celleta_topo[9000];   //[ncells_topo]
   Float_t         cellphi_topo[9000];   //[ncells_topo]
   Float_t         cellr_topo[9000];   //[ncells_topo]
   Float_t         cellz_topo[9000];   //[ncells_topo]
   Float_t         celldeta_topo[9000];   //[ncells_topo]
   Float_t         celldphi_topo[9000];   //[ncells_topo]
   Float_t         celldr_topo[9000];   //[ncells_topo]
   Float_t         celldz_topo[9000];   //[ncells_topo]
   Float_t         cellvol_topo[9000];   //[ncells_topo]
   Float_t         celltime_topo[9000];   //[ncells_topo]
   Float_t         cellquality_topo[9000];   //[ncells_topo]
   Float_t         cellweight_topo[9000];   //[ncells_topo]
   Int_t           cellgain_topo[9000];   //[ncells_topo]
   Float_t         cellnoise_topo[9000];   //[ncells_topo]
   Float_t         celltotnoise_topo[9000];   //[ncells_topo]
   Int_t           cellcluster_topo[9000];   //[ncells_topo]
   Int_t           cell_calo_topo[9000];   //[ncells_topo]
   Int_t           cell_side_topo[9000];   //[ncells_topo]
   Int_t           cell_samp_topo[9000];   //[ncells_topo]
   Int_t           cell_ireg_topo[9000];   //[ncells_topo]
   Int_t           cell_ieta_topo[9000];   //[ncells_topo]
   Int_t           cell_iphi_topo[9000];   //[ncells_topo]
   Float_t         cl_ecluster_topoEM;
   Int_t           cl_nctotal_topoEM;
   Int_t           cl_nc_topoEM;
   Float_t         cl_et_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_e_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eemb0_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eemb1_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eemb2_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eemb3_topoEM[30];   //[cl_nc_topoEM]
   Int_t           cl_nemb0_topoEM[30];   //[cl_nc_topoEM]
   Int_t           cl_nemb1_topoEM[30];   //[cl_nc_topoEM]
   Int_t           cl_nemb2_topoEM[30];   //[cl_nc_topoEM]
   Int_t           cl_nemb3_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_phi2_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eta0_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eta1_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eta2_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eta3_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_etileg1_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_etileg2_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_etileg3_topoEM[30];   //[cl_nc_topoEM]
   Int_t           cl_ntileg1_topoEM[30];   //[cl_nc_topoEM]
   Int_t           cl_ntileg2_topoEM[30];   //[cl_nc_topoEM]
   Int_t           cl_ntileg3_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eta_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_phi_topoEM[30];   //[cl_nc_topoEM]
   Int_t           cl_reco_stat_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_m1_eta_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_m1_phi_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_m2_r_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_m2_lambda_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_delta_phi_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_delta_theta_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_delta_alpha_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_center_x_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_center_y_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_center_z_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_center_lambda_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_lateral_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_longitudinal_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eng_frac_em_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eng_frac_max_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_eng_frac_core_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_m1_dens_topoEM[30];   //[cl_nc_topoEM]
   Float_t         cl_m2_dens_topoEM[30];   //[cl_nc_topoEM]
   Int_t           cl_ntotcells_topoEM[30];   //[cl_nc_topoEM]
   Int_t           cl_cells_topoEM[30][200];   //[cl_nc_topoEM]
   Int_t           NCells;
   Int_t           NFebs;
   Int_t           NLayers;
   Float_t         TimeCell[5000];   //[NCells]
   Float_t         EnergyCell[5000];   //[NCells]
   Int_t           LayerCell[5000];   //[NCells]
   Int_t           EtaCell[5000];   //[NCells]
   Int_t           PhiCell[5000];   //[NCells]
   Int_t           FebIDCell[5000];   //[NCells]
   Float_t         TimeLayer[10];   //[NLayers]
   Int_t           LayerIDLayer[10];   //[NLayers]
   Float_t         TimeFeb[500];   //[NFebs]
   Int_t           FebIdFeb[500];   //[NFebs]
   Int_t           SlotFeb[500];   //[NFebs]
   Int_t           FeedthroughFeb[500];   //[NFebs]
   Float_t         Phase;
   Float_t         TimeTotal;
   Float_t         EnergyTotal;
   Int_t           phasetime;
   Int_t           pixel_BCID2;
   Int_t           sct_BCID;
   Int_t           trt_BCID;
   Int_t           pixel_nhits;
   Float_t         pixel_layer[24000];   //[pixel_nhits]
   Float_t         pixel_phi[24000];   //[pixel_nhits]
   Float_t         pixel_row[24000];   //[pixel_nhits]
   Float_t         pixel_col[24000];   //[pixel_nhits]
   Float_t         pixel_tot[24000];   //[pixel_nhits]
   Int_t           pixel_BCID[24000];   //[pixel_nhits]
   Int_t           sct_rdo_nhits;
   Float_t         sct_rdo_strips[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_tbin[24000];   //[sct_rdo_nhits]
   Int_t           sct_rdo_rdoId[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_layer[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_eta[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_phi[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_side[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_errorhit[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_SyncronizationError[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_PreambleError[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_LVL1Error[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_BCIDError[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_FormatterError[24000];   //[sct_rdo_nhits]
   Float_t         sct_rdo_FirstSecondHitError[24000];   //[sct_rdo_nhits]
   Int_t           trt_rdo_nrdos;
   Float_t         trt_rdo_drifttime[10000];   //[trt_rdo_nrdos]
   Float_t         trt_rdo_timeoverthr[10000];   //[trt_rdo_nrdos]
   Float_t         trt_rdo_highlevel[10000];   //[trt_rdo_nrdos]
   Int_t           trt_rdo_timepll[10000];   //[trt_rdo_nrdos]
   Int_t           trt_rdo_straw[10000];   //[trt_rdo_nrdos]
   Int_t           trt_rdo_strawlayer[10000];   //[trt_rdo_nrdos]
   Int_t           trt_rdo_layer[10000];   //[trt_rdo_nrdos]
   Int_t           trt_rdo_phisector[10000];   //[trt_rdo_nrdos]
   Int_t           sct_clus_nclusters;
   Float_t         sct_clus_x[24000];   //[sct_clus_nclusters]
   Float_t         sct_clus_y[24000];   //[sct_clus_nclusters]
   Float_t         sct_clus_z[24000];   //[sct_clus_nclusters]
   Int_t           sct_clus_groupsize[24000];   //[sct_clus_nclusters]
   Int_t           sct_clus_layer[24000];   //[sct_clus_nclusters]
   Int_t           sct_clus_eta[24000];   //[sct_clus_nclusters]
   Int_t           sct_clus_phi[24000];   //[sct_clus_nclusters]
   Int_t           sct_clus_side[24000];   //[sct_clus_nclusters]
   Int_t           pixel_clus_nclusters;
   Float_t         pixel_clus_LocX[24000];   //[pixel_clus_nclusters]
   Float_t         pixel_clus_LocY[24000];   //[pixel_clus_nclusters]
   Int_t           pixel_clus_groupsize[24000];   //[pixel_clus_nclusters]
   Int_t           pixel_clus_layer[24000];   //[pixel_clus_nclusters]
   Int_t           pixel_clus_phi[24000];   //[pixel_clus_nclusters]
   Int_t           trt_nDCs;
   Float_t         trt_DC_timeoverthr[10000];   //[trt_nDCs]
   Float_t         trt_DC_highlevel[10000];   //[trt_nDCs]
   Float_t         trt_DC_radius[10000];   //[trt_nDCs]
   Int_t           trt_DC_straw[10000];   //[trt_nDCs]
   Int_t           trt_DC_strawlayer[10000];   //[trt_nDCs]
   Int_t           trt_DC_layer[10000];   //[trt_nDCs]
   Int_t           trt_DC_phisector[10000];   //[trt_nDCs]
   Float_t         trt_DC_straw_y[10000];   //[trt_nDCs]
   Float_t         trt_DC_straw_x[10000];   //[trt_nDCs]
   Int_t           sct_sp_nspacepoints;
   Float_t         sct_sp_x[12000];   //[sct_sp_nspacepoints]
   Float_t         sct_sp_y[12000];   //[sct_sp_nspacepoints]
   Float_t         sct_sp_z[12000];   //[sct_sp_nspacepoints]
   Int_t           sct_sp_layer[12000];   //[sct_sp_nspacepoints]
   Int_t           sct_sp_phi[12000];   //[sct_sp_nspacepoints]
   Int_t           pixel_sp_nspacepoints;
   Float_t         pixel_sp_x[12000];   //[pixel_sp_nspacepoints]
   Float_t         pixel_sp_y[12000];   //[pixel_sp_nspacepoints]
   Float_t         pixel_sp_z[12000];   //[pixel_sp_nspacepoints]
   Int_t           pixel_sp_layer[12000];   //[pixel_sp_nspacepoints]
   Int_t           pixel_sp_phi[12000];   //[pixel_sp_nspacepoints]
   Int_t           trk_nTracks;
   Int_t           trk_nPixelHits[60];   //[trk_nTracks]
   Float_t         trk_PixResPhi[60][8];   //[trk_nTracks]
   Float_t         trk_PixResEta[60][8];   //[trk_nTracks]
   Float_t         trk_PixPullPhi[60][8];   //[trk_nTracks]
   Float_t         trk_PixPullEta[60][8];   //[trk_nTracks]
   Float_t         trk_PixErrTrkPhi[60][8];   //[trk_nTracks]
   Float_t         trk_PixErrTrkEta[60][8];   //[trk_nTracks]
   Float_t         trk_PixErrHitPhi[60][8];   //[trk_nTracks]
   Float_t         trk_PixErrHitEta[60][8];   //[trk_nTracks]
   Int_t           trk_Pix_layer[60][8];   //[trk_nTracks]
   Int_t           trk_Sct_eta[60][32];   //[trk_nTracks]
   Int_t           trk_Pix_phi[60][8];   //[trk_nTracks]
   Float_t         trk_Pix_clusWidthRow[60][8];   //[trk_nTracks]
   Float_t         trk_Pix_clusWidthCol[60][8];   //[trk_nTracks]
   Int_t           trk_nSctHits[60];   //[trk_nTracks]
   Float_t         trk_SctRes[60][32];   //[trk_nTracks]
   Float_t         trk_SctPull[60][32];   //[trk_nTracks]
   Float_t         trk_SctErrTrk[60][32];   //[trk_nTracks]
   Float_t         trk_SctErrHit[60][32];   //[trk_nTracks]
   Int_t           trk_Sct_layer[60][32];   //[trk_nTracks]
   Int_t           trk_Sct_phi[60][32];   //[trk_nTracks]
   Int_t           trk_Sct_side[60][32];   //[trk_nTracks]
   Int_t           trk_Sct_rdoId[60][64];   //[trk_nTracks]
   Int_t           trk_Sct_clusGroupsize[60][32];   //[trk_nTracks]
   Float_t         trk_Sct_clusx[60][32];   //[trk_nTracks]
   Float_t         trk_Sct_clusy[60][32];   //[trk_nTracks]
   Float_t         trk_Sct_clusz[60][32];   //[trk_nTracks]
   Int_t           trk_nTrtHits[60];   //[trk_nTracks]
   Float_t         trk_TrtRes[60][100];   //[trk_nTracks]
   Float_t         trk_TrtPull[60][100];   //[trk_nTracks]
   Float_t         trk_TrtErrTrk[60][100];   //[trk_nTracks]
   Float_t         trk_TrtErrHit[60][100];   //[trk_nTracks]
   Float_t         trk_Trt_R[60][100];   //[trk_nTracks]
   Float_t         trk_Trt_Rtrack[60][100];   //[trk_nTracks]
   Float_t         trk_Trt_t[60][100];   //[trk_nTracks]
   Float_t         trk_Trt_x[60][100];   //[trk_nTracks]
   Float_t         trk_Trt_y[60][100];   //[trk_nTracks]
   Int_t           trk_Trt_HL[60][100];   //[trk_nTracks]
   Int_t           trk_Trt_strawlayer[60][100];   //[trk_nTracks]
   Int_t           trk_Trt_layer[60][100];   //[trk_nTracks]
   Int_t           trk_Trt_straw[60][100];   //[trk_nTracks]
   Int_t           trk_Trt_phisector[60][100];   //[trk_nTracks]
   Float_t         trk_Rec_d0[60];   //[trk_nTracks]
   Float_t         trk_Rec_z0[60];   //[trk_nTracks]
   Float_t         trk_Rec_phi0[60];   //[trk_nTracks]
   Float_t         trk_Rec_theta0[60];   //[trk_nTracks]
   Float_t         trk_Rec_qoverp[60];   //[trk_nTracks]
   Float_t         trk_Rec_chi2[60];   //[trk_nTracks]
   Int_t           trk_Rec_ndof[60];   //[trk_nTracks]
   Float_t         trk_Rec_eta[60];   //[trk_nTracks]
   Float_t         trk_Rec_error_d0[60];   //[trk_nTracks]
   Float_t         trk_Rec_error_z0[60];   //[trk_nTracks]
   Float_t         trk_Rec_error_phi0[60];   //[trk_nTracks]
   Float_t         trk_Rec_error_theta0[60];   //[trk_nTracks]
   Float_t         trk_Rec_error_qoverp[60];   //[trk_nTracks]
   Float_t         trk_Rec_chi2prob[60];   //[trk_nTracks]
   Int_t           trk_nTracksSi;
   Int_t           trk_nPixelHitsSi[60];   //[trk_nTracksSi]
   Float_t         trk_PixResPhiSi[60][8];   //[trk_nTracksSi]
   Float_t         trk_PixResEtaSi[60][8];   //[trk_nTracksSi]
   Float_t         trk_PixPullPhiSi[60][8];   //[trk_nTracksSi]
   Float_t         trk_PixPullEtaSi[60][8];   //[trk_nTracksSi]
   Float_t         trk_PixErrTrkPhiSi[60][8];   //[trk_nTracksSi]
   Float_t         trk_PixErrTrkEtaSi[60][8];   //[trk_nTracksSi]
   Float_t         trk_PixErrHitPhiSi[60][8];   //[trk_nTracksSi]
   Float_t         trk_PixErrHitEtaSi[60][8];   //[trk_nTracksSi]
   Int_t           trk_Pix_layerSi[60][8];   //[trk_nTracksSi]
   Int_t           trk_Sct_etaSi[60][32];   //[trk_nTracksSi]
   Int_t           trk_Pix_phiSi[60][8];   //[trk_nTracksSi]
   Float_t         trk_Pix_clusWidthRowSi[60][8];   //[trk_nTracksSi]
   Float_t         trk_Pix_clusWidthColSi[60][8];   //[trk_nTracksSi]
   Int_t           trk_nSctHitsSi[60];   //[trk_nTracksSi]
   Float_t         trk_SctResSi[60][32];   //[trk_nTracksSi]
   Float_t         trk_SctPullSi[60][32];   //[trk_nTracksSi]
   Float_t         trk_SctErrTrkSi[60][32];   //[trk_nTracksSi]
   Float_t         trk_SctErrHitSi[60][32];   //[trk_nTracksSi]
   Int_t           trk_Sct_layerSi[60][32];   //[trk_nTracksSi]
   Int_t           trk_Sct_phiSi[60][32];   //[trk_nTracksSi]
   Int_t           trk_Sct_sideSi[60][32];   //[trk_nTracksSi]
   Int_t           trk_Sct_rdoIdSi[60][64];   //[trk_nTracksSi]
   Int_t           trk_Sct_clusGroupsizeSi[60][32];   //[trk_nTracksSi]
   Float_t         trk_Sct_clusxSi[60][32];   //[trk_nTracksSi]
   Float_t         trk_Sct_clusySi[60][32];   //[trk_nTracksSi]
   Float_t         trk_Sct_cluszSi[60][32];   //[trk_nTracksSi]
   Int_t           trk_nTrtHitsSi[60];   //[trk_nTracksSi]
   Float_t         trk_TrtResSi[60][100];   //[trk_nTracksSi]
   Float_t         trk_TrtPullSi[60][100];   //[trk_nTracksSi]
   Float_t         trk_TrtErrTrkSi[60][100];   //[trk_nTracksSi]
   Float_t         trk_TrtErrHitSi[60][100];   //[trk_nTracksSi]
   Float_t         trk_Trt_RSi[60][100];   //[trk_nTracksSi]
   Float_t         trk_Trt_RtrackSi[60][100];   //[trk_nTracksSi]
   Float_t         trk_Trt_tSi[60][100];   //[trk_nTracksSi]
   Float_t         trk_Trt_xSi[60][100];   //[trk_nTracksSi]
   Float_t         trk_Trt_ySi[60][100];   //[trk_nTracksSi]
   Int_t           trk_Trt_HLSi[60][100];   //[trk_nTracksSi]
   Int_t           trk_Trt_strawlayerSi[60][100];   //[trk_nTracksSi]
   Int_t           trk_Trt_layerSi[60][100];   //[trk_nTracksSi]
   Int_t           trk_Trt_strawSi[60][100];   //[trk_nTracksSi]
   Int_t           trk_Trt_phisectorSi[60][100];   //[trk_nTracksSi]
   Float_t         trk_Rec_d0Si[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_z0Si[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_phi0Si[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_theta0Si[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_qoverpSi[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_chi2Si[60];   //[trk_nTracksSi]
   Int_t           trk_Rec_ndofSi[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_etaSi[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_error_d0Si[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_error_z0Si[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_error_phi0Si[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_error_theta0Si[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_error_qoverpSi[60];   //[trk_nTracksSi]
   Float_t         trk_Rec_chi2probSi[60];   //[trk_nTracksSi]
   Int_t           trk_nTracksPixel;
   Int_t           trk_nPixelHitsPixel[60];   //[trk_nTracksPixel]
   Float_t         trk_PixResPhiPixel[60][8];   //[trk_nTracksPixel]
   Float_t         trk_PixResEtaPixel[60][8];   //[trk_nTracksPixel]
   Float_t         trk_PixPullPhiPixel[60][8];   //[trk_nTracksPixel]
   Float_t         trk_PixPullEtaPixel[60][8];   //[trk_nTracksPixel]
   Float_t         trk_PixErrTrkPhiPixel[60][8];   //[trk_nTracksPixel]
   Float_t         trk_PixErrTrkEtaPixel[60][8];   //[trk_nTracksPixel]
   Float_t         trk_PixErrHitPhiPixel[60][8];   //[trk_nTracksPixel]
   Float_t         trk_PixErrHitEtaPixel[60][8];   //[trk_nTracksPixel]
   Int_t           trk_Pix_layerPixel[60][8];   //[trk_nTracksPixel]
   Int_t           trk_Sct_etaPixel[60][32];   //[trk_nTracksPixel]
   Int_t           trk_Pix_phiPixel[60][8];   //[trk_nTracksPixel]
   Float_t         trk_Pix_clusWidthRowPixel[60][8];   //[trk_nTracksPixel]
   Float_t         trk_Pix_clusWidthColPixel[60][8];   //[trk_nTracksPixel]
   Int_t           trk_nSctHitsPixel[60];   //[trk_nTracksPixel]
   Float_t         trk_SctResPixel[60][32];   //[trk_nTracksPixel]
   Float_t         trk_SctPullPixel[60][32];   //[trk_nTracksPixel]
   Float_t         trk_SctErrTrkPixel[60][32];   //[trk_nTracksPixel]
   Float_t         trk_SctErrHitPixel[60][32];   //[trk_nTracksPixel]
   Int_t           trk_Sct_layerPixel[60][32];   //[trk_nTracksPixel]
   Int_t           trk_Sct_phiPixel[60][32];   //[trk_nTracksPixel]
   Int_t           trk_Sct_sidePixel[60][32];   //[trk_nTracksPixel]
   Int_t           trk_Sct_rdoIdPixel[60][64];   //[trk_nTracksPixel]
   Int_t           trk_Sct_clusGroupsizePixel[60][32];   //[trk_nTracksPixel]
   Float_t         trk_Sct_clusxPixel[60][32];   //[trk_nTracksPixel]
   Float_t         trk_Sct_clusyPixel[60][32];   //[trk_nTracksPixel]
   Float_t         trk_Sct_cluszPixel[60][32];   //[trk_nTracksPixel]
   Int_t           trk_nTrtHitsPixel[60];   //[trk_nTracksPixel]
   Float_t         trk_TrtResPixel[60][100];   //[trk_nTracksPixel]
   Float_t         trk_TrtPullPixel[60][100];   //[trk_nTracksPixel]
   Float_t         trk_TrtErrTrkPixel[60][100];   //[trk_nTracksPixel]
   Float_t         trk_TrtErrHitPixel[60][100];   //[trk_nTracksPixel]
   Float_t         trk_Trt_RPixel[60][100];   //[trk_nTracksPixel]
   Float_t         trk_Trt_RtrackPixel[60][100];   //[trk_nTracksPixel]
   Float_t         trk_Trt_tPixel[60][100];   //[trk_nTracksPixel]
   Float_t         trk_Trt_xPixel[60][100];   //[trk_nTracksPixel]
   Float_t         trk_Trt_yPixel[60][100];   //[trk_nTracksPixel]
   Int_t           trk_Trt_HLPixel[60][100];   //[trk_nTracksPixel]
   Int_t           trk_Trt_strawlayerPixel[60][100];   //[trk_nTracksPixel]
   Int_t           trk_Trt_layerPixel[60][100];   //[trk_nTracksPixel]
   Int_t           trk_Trt_strawPixel[60][100];   //[trk_nTracksPixel]
   Int_t           trk_Trt_phisectorPixel[60][100];   //[trk_nTracksPixel]
   Float_t         trk_Rec_d0Pixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_z0Pixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_phi0Pixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_theta0Pixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_qoverpPixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_chi2Pixel[60];   //[trk_nTracksPixel]
   Int_t           trk_Rec_ndofPixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_etaPixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_error_d0Pixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_error_z0Pixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_error_phi0Pixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_error_theta0Pixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_error_qoverpPixel[60];   //[trk_nTracksPixel]
   Float_t         trk_Rec_chi2probPixel[60];   //[trk_nTracksPixel]
   Int_t           trk_nTracksSCT;
   Int_t           trk_nPixelHitsSCT[60];   //[trk_nTracksSCT]
   Float_t         trk_PixResPhiSCT[60][8];   //[trk_nTracksSCT]
   Float_t         trk_PixResEtaSCT[60][8];   //[trk_nTracksSCT]
   Float_t         trk_PixPullPhiSCT[60][8];   //[trk_nTracksSCT]
   Float_t         trk_PixPullEtaSCT[60][8];   //[trk_nTracksSCT]
   Float_t         trk_PixErrTrkPhiSCT[60][8];   //[trk_nTracksSCT]
   Float_t         trk_PixErrTrkEtaSCT[60][8];   //[trk_nTracksSCT]
   Float_t         trk_PixErrHitPhiSCT[60][8];   //[trk_nTracksSCT]
   Float_t         trk_PixErrHitEtaSCT[60][8];   //[trk_nTracksSCT]
   Int_t           trk_Pix_layerSCT[60][8];   //[trk_nTracksSCT]
   Int_t           trk_Sct_etaSCT[60][32];   //[trk_nTracksSCT]
   Int_t           trk_Pix_phiSCT[60][8];   //[trk_nTracksSCT]
   Float_t         trk_Pix_clusWidthRowSCT[60][8];   //[trk_nTracksSCT]
   Float_t         trk_Pix_clusWidthColSCT[60][8];   //[trk_nTracksSCT]
   Int_t           trk_nSctHitsSCT[60];   //[trk_nTracksSCT]
   Float_t         trk_SctResSCT[60][32];   //[trk_nTracksSCT]
   Float_t         trk_SctPullSCT[60][32];   //[trk_nTracksSCT]
   Float_t         trk_SctErrTrkSCT[60][32];   //[trk_nTracksSCT]
   Float_t         trk_SctErrHitSCT[60][32];   //[trk_nTracksSCT]
   Int_t           trk_Sct_layerSCT[60][32];   //[trk_nTracksSCT]
   Int_t           trk_Sct_phiSCT[60][32];   //[trk_nTracksSCT]
   Int_t           trk_Sct_sideSCT[60][32];   //[trk_nTracksSCT]
   Int_t           trk_Sct_rdoIdSCT[60][64];   //[trk_nTracksSCT]
   Int_t           trk_Sct_clusGroupsizeSCT[60][32];   //[trk_nTracksSCT]
   Float_t         trk_Sct_clusxSCT[60][32];   //[trk_nTracksSCT]
   Float_t         trk_Sct_clusySCT[60][32];   //[trk_nTracksSCT]
   Float_t         trk_Sct_cluszSCT[60][32];   //[trk_nTracksSCT]
   Int_t           trk_nTrtHitsSCT[60];   //[trk_nTracksSCT]
   Float_t         trk_TrtResSCT[60][100];   //[trk_nTracksSCT]
   Float_t         trk_TrtPullSCT[60][100];   //[trk_nTracksSCT]
   Float_t         trk_TrtErrTrkSCT[60][100];   //[trk_nTracksSCT]
   Float_t         trk_TrtErrHitSCT[60][100];   //[trk_nTracksSCT]
   Float_t         trk_Trt_RSCT[60][100];   //[trk_nTracksSCT]
   Float_t         trk_Trt_RtrackSCT[60][100];   //[trk_nTracksSCT]
   Float_t         trk_Trt_tSCT[60][100];   //[trk_nTracksSCT]
   Float_t         trk_Trt_xSCT[60][100];   //[trk_nTracksSCT]
   Float_t         trk_Trt_ySCT[60][100];   //[trk_nTracksSCT]
   Int_t           trk_Trt_HLSCT[60][100];   //[trk_nTracksSCT]
   Int_t           trk_Trt_strawlayerSCT[60][100];   //[trk_nTracksSCT]
   Int_t           trk_Trt_layerSCT[60][100];   //[trk_nTracksSCT]
   Int_t           trk_Trt_strawSCT[60][100];   //[trk_nTracksSCT]
   Int_t           trk_Trt_phisectorSCT[60][100];   //[trk_nTracksSCT]
   Float_t         trk_Rec_d0SCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_z0SCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_phi0SCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_theta0SCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_qoverpSCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_chi2SCT[60];   //[trk_nTracksSCT]
   Int_t           trk_Rec_ndofSCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_etaSCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_error_d0SCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_error_z0SCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_error_phi0SCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_error_theta0SCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_error_qoverpSCT[60];   //[trk_nTracksSCT]
   Float_t         trk_Rec_chi2probSCT[60];   //[trk_nTracksSCT]
   Int_t           trk_nTracksTRT;
   Int_t           trk_nPixelHitsTRT[60];   //[trk_nTracksTRT]
   Float_t         trk_PixResPhiTRT[60][8];   //[trk_nTracksTRT]
   Float_t         trk_PixResEtaTRT[60][8];   //[trk_nTracksTRT]
   Float_t         trk_PixPullPhiTRT[60][8];   //[trk_nTracksTRT]
   Float_t         trk_PixPullEtaTRT[60][8];   //[trk_nTracksTRT]
   Float_t         trk_PixErrTrkPhiTRT[60][8];   //[trk_nTracksTRT]
   Float_t         trk_PixErrTrkEtaTRT[60][8];   //[trk_nTracksTRT]
   Float_t         trk_PixErrHitPhiTRT[60][8];   //[trk_nTracksTRT]
   Float_t         trk_PixErrHitEtaTRT[60][8];   //[trk_nTracksTRT]
   Int_t           trk_Pix_layerTRT[60][8];   //[trk_nTracksTRT]
   Int_t           trk_Sct_etaTRT[60][32];   //[trk_nTracksTRT]
   Int_t           trk_Pix_phiTRT[60][8];   //[trk_nTracksTRT]
   Float_t         trk_Pix_clusWidthRowTRT[60][8];   //[trk_nTracksTRT]
   Float_t         trk_Pix_clusWidthColTRT[60][8];   //[trk_nTracksTRT]
   Int_t           trk_nSctHitsTRT[60];   //[trk_nTracksTRT]
   Float_t         trk_SctResTRT[60][32];   //[trk_nTracksTRT]
   Float_t         trk_SctPullTRT[60][32];   //[trk_nTracksTRT]
   Float_t         trk_SctErrTrkTRT[60][32];   //[trk_nTracksTRT]
   Float_t         trk_SctErrHitTRT[60][32];   //[trk_nTracksTRT]
   Int_t           trk_Sct_layerTRT[60][32];   //[trk_nTracksTRT]
   Int_t           trk_Sct_phiTRT[60][32];   //[trk_nTracksTRT]
   Int_t           trk_Sct_sideTRT[60][32];   //[trk_nTracksTRT]
   Int_t           trk_Sct_rdoIdTRT[60][64];   //[trk_nTracksTRT]
   Int_t           trk_Sct_clusGroupsizeTRT[60][32];   //[trk_nTracksTRT]
   Float_t         trk_Sct_clusxTRT[60][32];   //[trk_nTracksTRT]
   Float_t         trk_Sct_clusyTRT[60][32];   //[trk_nTracksTRT]
   Float_t         trk_Sct_cluszTRT[60][32];   //[trk_nTracksTRT]
   Int_t           trk_nTrtHitsTRT[60];   //[trk_nTracksTRT]
   Float_t         trk_TrtResTRT[60][100];   //[trk_nTracksTRT]
   Float_t         trk_TrtPullTRT[60][100];   //[trk_nTracksTRT]
   Float_t         trk_TrtErrTrkTRT[60][100];   //[trk_nTracksTRT]
   Float_t         trk_TrtErrHitTRT[60][100];   //[trk_nTracksTRT]
   Float_t         trk_Trt_RTRT[60][100];   //[trk_nTracksTRT]
   Float_t         trk_Trt_RtrackTRT[60][100];   //[trk_nTracksTRT]
   Float_t         trk_Trt_tTRT[60][100];   //[trk_nTracksTRT]
   Float_t         trk_Trt_xTRT[60][100];   //[trk_nTracksTRT]
   Float_t         trk_Trt_yTRT[60][100];   //[trk_nTracksTRT]
   Int_t           trk_Trt_HLTRT[60][100];   //[trk_nTracksTRT]
   Int_t           trk_Trt_strawlayerTRT[60][100];   //[trk_nTracksTRT]
   Int_t           trk_Trt_layerTRT[60][100];   //[trk_nTracksTRT]
   Int_t           trk_Trt_strawTRT[60][100];   //[trk_nTracksTRT]
   Int_t           trk_Trt_phisectorTRT[60][100];   //[trk_nTracksTRT]
   Float_t         trk_Rec_d0TRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_z0TRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_phi0TRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_theta0TRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_qoverpTRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_chi2TRT[60];   //[trk_nTracksTRT]
   Int_t           trk_Rec_ndofTRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_etaTRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_error_d0TRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_error_z0TRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_error_phi0TRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_error_theta0TRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_error_qoverpTRT[60];   //[trk_nTracksTRT]
   Float_t         trk_Rec_chi2probTRT[60];   //[trk_nTracksTRT]
   Int_t           Trk_totalNumTracks;
   Float_t         Trk_d0[500];   //[Trk_totalNumTracks]
   Float_t         Trk_z0[500];   //[Trk_totalNumTracks]
   Float_t         Trk_phi[500];   //[Trk_totalNumTracks]
   Float_t         Trk_theta[500];   //[Trk_totalNumTracks]
   Float_t         Trk_qOverP[500];   //[Trk_totalNumTracks]
   Float_t         Trk_Chi2[500];   //[Trk_totalNumTracks]
   Int_t           Trk_Ndf[500];   //[Trk_totalNumTracks]
   Float_t         Trk_sigd0[500];   //[Trk_totalNumTracks]
   Float_t         Trk_sigz0[500];   //[Trk_totalNumTracks]
   Float_t         Trk_sigphi[500];   //[Trk_totalNumTracks]
   Float_t         Trk_sigtheta[500];   //[Trk_totalNumTracks]
   Float_t         Trk_sigqOverP[500];   //[Trk_totalNumTracks]
   Float_t         Trk_covVert21[500];   //[Trk_totalNumTracks]
   Float_t         Trk_covVert31[500];   //[Trk_totalNumTracks]
   Float_t         Trk_covVert32[500];   //[Trk_totalNumTracks]
   Float_t         Trk_covVert41[500];   //[Trk_totalNumTracks]
   Float_t         Trk_covVert42[500];   //[Trk_totalNumTracks]
   Float_t         Trk_covVert43[500];   //[Trk_totalNumTracks]
   Float_t         Trk_covVert51[500];   //[Trk_totalNumTracks]
   Float_t         Trk_covVert52[500];   //[Trk_totalNumTracks]
   Float_t         Trk_covVert53[500];   //[Trk_totalNumTracks]
   Float_t         Trk_covVert54[500];   //[Trk_totalNumTracks]
   Float_t         Trk_px[500];   //[Trk_totalNumTracks]
   Float_t         Trk_py[500];   //[Trk_totalNumTracks]
   Float_t         Trk_pz[500];   //[Trk_totalNumTracks]
   Float_t         Trk_pt[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfBLayerHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfPixelHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfSCTHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfTRTHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfTRTHighThresholdHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfBLayerSharedHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfPixelSharedHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfPixelHoles[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfSCTSharedHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfSCTHoles[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfTRTOutliers[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfTRTHighThresholdOutliers[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfMdtHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfTgcPhiHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfRpcPhiHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfCscPhiHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfTgcEtaHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfRpcEtaHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfCscEtaHits[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfGangedPixels[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfOutliersOnTrack[500];   //[Trk_totalNumTracks]
   Int_t           Trk_numberOfTrackSummaryTypes[500];   //[Trk_totalNumTracks]
   Int_t           Trk_truthBarcode[500];   //[Trk_totalNumTracks]
   Int_t           Trk_truthNt[500];   //[Trk_totalNumTracks]
   Int_t           Trk_ParticlePdg[500];   //[Trk_totalNumTracks]
   Int_t           Trk_ParentPdg[500];   //[Trk_totalNumTracks]
   Int_t           IIC_InDet_NImpact;
   Float_t         IIC_InDet_x0[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_y0[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_z0[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_eta0_local[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_phi0_local[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_trketa_at0[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_trkphi_at0[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_x1[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_y1[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_z1[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_eta1_local[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_phi1_local[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_trketa_at1[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_trkphi_at1[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_x2[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_y2[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_z2[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_eta2_local[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_phi2_local[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_trketa_at2[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_trkphi_at2[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_x3[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_y3[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_z3[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_eta3_local[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_phi3_local[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_trketa_at3[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_trkphi_at3[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_x_tile[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_y_tile[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_z_tile[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_eta_tile_local[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_phi_tile_local[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_trketa_attile[500];   //[IIC_InDet_NImpact]
   Float_t         IIC_InDet_trkphi_attile[500];   //[IIC_InDet_NImpact]

   // List of branches
   TBranch        *b_Run;   //!
   TBranch        *b_Event;   //!
   TBranch        *b_Time;   //!
   TBranch        *b_Weight;   //!
   TBranch        *b_IEvent;   //!
   TBranch        *b_RunEta;   //!
   TBranch        *b_RunPartType;   //!
   TBranch        *b_RunEnergy;   //!
   TBranch        *b_Energy;   //!
   TBranch        *b_ErrAbsEnergy;   //!
   TBranch        *b_ErrCollimators;   //!
   TBranch        *b_ErrCurrents;   //!
   TBranch        *b_SyncLoss;   //!
   TBranch        *b_ErrSyncLoss;   //!
   TBranch        *b_Quad_nc;   //!
   TBranch        *b_Quad_file;   //!
   TBranch        *b_Quad_equip;   //!
   TBranch        *b_Bend_nc;   //!
   TBranch        *b_Bend_file;   //!
   TBranch        *b_Bend_equip;   //!
   TBranch        *b_Trim_nc;   //!
   TBranch        *b_Trim_file;   //!
   TBranch        *b_Trim_equip;   //!
   TBranch        *b_Coll_nc;   //!
   TBranch        *b_Coll_file;   //!
   TBranch        *b_Coll_equip;   //!
   TBranch        *b_sADC_S0;   //!
   TBranch        *b_sTDC_S0;   //!
   TBranch        *b_sADC_S1;   //!
   TBranch        *b_sTDC_S1;   //!
   TBranch        *b_sADC_S2mUp;   //!
   TBranch        *b_sTDC_S2mUp;   //!
   TBranch        *b_sADC_S2mDown;   //!
   TBranch        *b_sTDC_S2mDown;   //!
   TBranch        *b_sADC_S3mLeft;   //!
   TBranch        *b_sTDC_S3mLeft;   //!
   TBranch        *b_sADC_S3mRight;   //!
   TBranch        *b_sTDC_S3mRight;   //!
   TBranch        *b_sADC_C1;   //!
   TBranch        *b_sTDC_C1;   //!
   TBranch        *b_sADC_C2;   //!
   TBranch        *b_sTDC_C2;   //!
   TBranch        *b_sADC_muTag;   //!
   TBranch        *b_sTDC_muTag;   //!
   TBranch        *b_sADC_muHalo;   //!
   TBranch        *b_sTDC_muHalo;   //!
   TBranch        *b_sADC_muVeto;   //!
   TBranch        *b_sTDC_muVeto;   //!
   TBranch        *b_sADC_SC1;   //!
   TBranch        *b_sTDC_SC1;   //!
   TBranch        *b_sADC_SC2;   //!
   TBranch        *b_sTDC_SC2;   //!
   TBranch        *b_sADC_TRTSci;   //!
   TBranch        *b_sTDC_TRTSci;   //!
   TBranch        *b_bpcX_BPCm2;   //!
   TBranch        *b_bpcY_BPCm2;   //!
   TBranch        *b_bpcX_BPCm1;   //!
   TBranch        *b_bpcY_BPCm1;   //!
   TBranch        *b_bpcX_BPC0;   //!
   TBranch        *b_bpcY_BPC0;   //!
   TBranch        *b_bpcX_BPC1;   //!
   TBranch        *b_bpcY_BPC1;   //!
   TBranch        *b_bpcX_BPC2;   //!
   TBranch        *b_bpcY_BPC2;   //!
   TBranch        *b_Trigger;   //!
   TBranch        *b_Clock;   //!
   TBranch        *b_Nh_Calo;   //!
   TBranch        *b_Eh_Calo;   //!
   TBranch        *b_Nh_EM;   //!
   TBranch        *b_Eh_EM;   //!
   TBranch        *b_Nh_HAD;   //!
   TBranch        *b_Eh_HAD;   //!
   TBranch        *b_Nh_PresB;   //!
   TBranch        *b_Eh_PresB;   //!
   TBranch        *b_Nh_EMB;   //!
   TBranch        *b_Eh_EMB;   //!
   TBranch        *b_Nh_EMEC;   //!
   TBranch        *b_Eh_EMEC;   //!
   TBranch        *b_Nh_Tile;   //!
   TBranch        *b_Eh_Tile;   //!
   TBranch        *b_Nh_TileGap;   //!
   TBranch        *b_Eh_TileGap;   //!
   TBranch        *b_Nh_HEC;   //!
   TBranch        *b_Eh_HEC;   //!
   TBranch        *b_Nh_FCal;   //!
   TBranch        *b_Eh_FCal;   //!
   TBranch        *b_Nh_PresE;   //!
   TBranch        *b_Eh_PresE;   //!
   TBranch        *b_Nh_Scint;   //!
   TBranch        *b_Eh_Scint;   //!
   TBranch        *b_nhit_lar;   //!
   TBranch        *b_ecell_lar;   //!
   TBranch        *b_NCells_lar;   //!
   TBranch        *b_ECells_lar;   //!
   TBranch        *b_EtaCells_lar;   //!
   TBranch        *b_PhiCells_lar;   //!
   TBranch        *b_QCells_lar;   //!
   TBranch        *b_GainCells_lar;   //!
   TBranch        *b_DetCells_lar;   //!
   TBranch        *b_TimeCells_lar;   //!
   TBranch        *b_BadCells_lar;   //!
   TBranch        *b_nhit_tile;   //!
   TBranch        *b_ecell_tile;   //!
   TBranch        *b_NCells_tile;   //!
   TBranch        *b_ECells_tile;   //!
   TBranch        *b_EtaCells_tile;   //!
   TBranch        *b_PhiCells_tile;   //!
   TBranch        *b_QCells_tile;   //!
   TBranch        *b_GainCells_tile;   //!
   TBranch        *b_DetCells_tile;   //!
   TBranch        *b_TimeCells_tile;   //!
   TBranch        *b_cl_ecluster_em;   //!
   TBranch        *b_cl_nctotal_em;   //!
   TBranch        *b_cl_nc_em;   //!
   TBranch        *b_cl_et_em;   //!
   TBranch        *b_cl_e_em;   //!
   TBranch        *b_cl_eemb0_em;   //!
   TBranch        *b_cl_eemb1_em;   //!
   TBranch        *b_cl_eemb2_em;   //!
   TBranch        *b_cl_eemb3_em;   //!
   TBranch        *b_cl_phi2_em;   //!
   TBranch        *b_cl_eta0_em;   //!
   TBranch        *b_cl_eta1_em;   //!
   TBranch        *b_cl_eta2_em;   //!
   TBranch        *b_cl_eta3_em;   //!
   TBranch        *b_cl_etileg1_em;   //!
   TBranch        *b_cl_etileg2_em;   //!
   TBranch        *b_cl_etileg3_em;   //!
   TBranch        *b_cl_eta_em;   //!
   TBranch        *b_cl_phi_em;   //!
   TBranch        *b_cl_reco_stat_em;   //!
   TBranch        *b_cl_m1_eta_em;   //!
   TBranch        *b_cl_m1_phi_em;   //!
   TBranch        *b_cl_m2_r_em;   //!
   TBranch        *b_cl_m2_lambda_em;   //!
   TBranch        *b_cl_delta_phi_em;   //!
   TBranch        *b_cl_delta_theta_em;   //!
   TBranch        *b_cl_delta_alpha_em;   //!
   TBranch        *b_cl_center_x_em;   //!
   TBranch        *b_cl_center_y_em;   //!
   TBranch        *b_cl_center_z_em;   //!
   TBranch        *b_cl_center_lambda_em;   //!
   TBranch        *b_cl_lateral_em;   //!
   TBranch        *b_cl_longitudinal_em;   //!
   TBranch        *b_cl_eng_frac_em_em;   //!
   TBranch        *b_cl_eng_frac_max_em;   //!
   TBranch        *b_cl_eng_frac_core_em;   //!
   TBranch        *b_cl_m1_dens_em;   //!
   TBranch        *b_cl_m2_dens_em;   //!
   TBranch        *b_cl_ntotcells_em;   //!
   TBranch        *b_cl_cells_em;   //!
   TBranch        *b_cl_ecluster_tb_em;   //!
   TBranch        *b_cl_nctotal_tb_em;   //!
   TBranch        *b_cl_nc_tb_em;   //!
   TBranch        *b_cl_et_tb_em;   //!
   TBranch        *b_cl_e_tb_em;   //!
   TBranch        *b_cl_eemb0_tb_em;   //!
   TBranch        *b_cl_eemb1_tb_em;   //!
   TBranch        *b_cl_eemb2_tb_em;   //!
   TBranch        *b_cl_eemb3_tb_em;   //!
   TBranch        *b_cl_phi2_tb_em;   //!
   TBranch        *b_cl_eta0_tb_em;   //!
   TBranch        *b_cl_eta1_tb_em;   //!
   TBranch        *b_cl_eta2_tb_em;   //!
   TBranch        *b_cl_eta3_tb_em;   //!
   TBranch        *b_cl_etileg1_tb_em;   //!
   TBranch        *b_cl_etileg2_tb_em;   //!
   TBranch        *b_cl_etileg3_tb_em;   //!
   TBranch        *b_cl_eta_tb_em;   //!
   TBranch        *b_cl_phi_tb_em;   //!
   TBranch        *b_cl_reco_stat_tb_em;   //!
   TBranch        *b_cl_m1_eta_tb_em;   //!
   TBranch        *b_cl_m1_phi_tb_em;   //!
   TBranch        *b_cl_m2_r_tb_em;   //!
   TBranch        *b_cl_m2_lambda_tb_em;   //!
   TBranch        *b_cl_delta_phi_tb_em;   //!
   TBranch        *b_cl_delta_theta_tb_em;   //!
   TBranch        *b_cl_delta_alpha_tb_em;   //!
   TBranch        *b_cl_center_x_tb_em;   //!
   TBranch        *b_cl_center_y_tb_em;   //!
   TBranch        *b_cl_center_z_tb_em;   //!
   TBranch        *b_cl_center_lambda_tb_em;   //!
   TBranch        *b_cl_lateral_tb_em;   //!
   TBranch        *b_cl_longitudinal_tb_em;   //!
   TBranch        *b_cl_eng_frac_em_tb_em;   //!
   TBranch        *b_cl_eng_frac_max_tb_em;   //!
   TBranch        *b_cl_eng_frac_core_tb_em;   //!
   TBranch        *b_cl_m1_dens_tb_em;   //!
   TBranch        *b_cl_m2_dens_tb_em;   //!
   TBranch        *b_cl_ntotcells_tb_em;   //!
   TBranch        *b_cl_cells_tb_em;   //!
   TBranch        *b_cl_ecluster_topo;   //!
   TBranch        *b_cl_nctotal_topo;   //!
   TBranch        *b_cl_nc_topo;   //!
   TBranch        *b_cl_et_topo;   //!
   TBranch        *b_cl_e_topo;   //!
   TBranch        *b_cl_eemb0_topo;   //!
   TBranch        *b_cl_eemb1_topo;   //!
   TBranch        *b_cl_eemb2_topo;   //!
   TBranch        *b_cl_eemb3_topo;   //!
   TBranch        *b_cl_nemb0_topo;   //!
   TBranch        *b_cl_nemb1_topo;   //!
   TBranch        *b_cl_nemb2_topo;   //!
   TBranch        *b_cl_nemb3_topo;   //!
   TBranch        *b_cl_phi2_topo;   //!
   TBranch        *b_cl_eta0_topo;   //!
   TBranch        *b_cl_eta1_topo;   //!
   TBranch        *b_cl_eta2_topo;   //!
   TBranch        *b_cl_eta3_topo;   //!
   TBranch        *b_cl_etileg1_topo;   //!
   TBranch        *b_cl_etileg2_topo;   //!
   TBranch        *b_cl_etileg3_topo;   //!
   TBranch        *b_cl_ntileg1_topo;   //!
   TBranch        *b_cl_ntileg2_topo;   //!
   TBranch        *b_cl_ntileg3_topo;   //!
   TBranch        *b_cl_eta_topo;   //!
   TBranch        *b_cl_phi_topo;   //!
   TBranch        *b_cl_reco_stat_topo;   //!
   TBranch        *b_cl_m1_eta_topo;   //!
   TBranch        *b_cl_m1_phi_topo;   //!
   TBranch        *b_cl_m2_r_topo;   //!
   TBranch        *b_cl_m2_lambda_topo;   //!
   TBranch        *b_cl_delta_phi_topo;   //!
   TBranch        *b_cl_delta_theta_topo;   //!
   TBranch        *b_cl_delta_alpha_topo;   //!
   TBranch        *b_cl_center_x_topo;   //!
   TBranch        *b_cl_center_y_topo;   //!
   TBranch        *b_cl_center_z_topo;   //!
   TBranch        *b_cl_center_lambda_topo;   //!
   TBranch        *b_cl_lateral_topo;   //!
   TBranch        *b_cl_longitudinal_topo;   //!
   TBranch        *b_cl_eng_frac_em_topo;   //!
   TBranch        *b_cl_eng_frac_max_topo;   //!
   TBranch        *b_cl_eng_frac_core_topo;   //!
   TBranch        *b_cl_m1_dens_topo;   //!
   TBranch        *b_cl_m2_dens_topo;   //!
   TBranch        *b_cl_etileb0_topo;   //!
   TBranch        *b_cl_etileb1_topo;   //!
   TBranch        *b_cl_etileb2_topo;   //!
   TBranch        *b_cl_ntileb0_topo;   //!
   TBranch        *b_cl_ntileb1_topo;   //!
   TBranch        *b_cl_ntileb2_topo;   //!
   TBranch        *b_cl_ntotcells_topo;   //!
   TBranch        *b_cl_cells_topo;   //!
   TBranch        *b_ncellstotal_topo;   //!
   TBranch        *b_ncells_topo;   //!
   TBranch        *b_celleng_topo;   //!
   TBranch        *b_cellet_topo;   //!
   TBranch        *b_celleta_topo;   //!
   TBranch        *b_cellphi_topo;   //!
   TBranch        *b_cellr_topo;   //!
   TBranch        *b_cellz_topo;   //!
   TBranch        *b_celldeta_topo;   //!
   TBranch        *b_celldphi_topo;   //!
   TBranch        *b_celldr_topo;   //!
   TBranch        *b_celldz_topo;   //!
   TBranch        *b_cellvol_topo;   //!
   TBranch        *b_celltime_topo;   //!
   TBranch        *b_cellquality_topo;   //!
   TBranch        *b_cellweight_topo;   //!
   TBranch        *b_cellgain_topo;   //!
   TBranch        *b_cellnoise_topo;   //!
   TBranch        *b_celltotnoise_topo;   //!
   TBranch        *b_cellcluster_topo;   //!
   TBranch        *b_cell_calo_topo;   //!
   TBranch        *b_cell_side_topo;   //!
   TBranch        *b_cell_samp_topo;   //!
   TBranch        *b_cell_ireg_topo;   //!
   TBranch        *b_cell_ieta_topo;   //!
   TBranch        *b_cell_iphi_topo;   //!
   TBranch        *b_cl_ecluster_topoEM;   //!
   TBranch        *b_cl_nctotal_topoEM;   //!
   TBranch        *b_cl_nc_topoEM;   //!
   TBranch        *b_cl_et_topoEM;   //!
   TBranch        *b_cl_e_topoEM;   //!
   TBranch        *b_cl_eemb0_topoEM;   //!
   TBranch        *b_cl_eemb1_topoEM;   //!
   TBranch        *b_cl_eemb2_topoEM;   //!
   TBranch        *b_cl_eemb3_topoEM;   //!
   TBranch        *b_cl_nemb0_topoEM;   //!
   TBranch        *b_cl_nemb1_topoEM;   //!
   TBranch        *b_cl_nemb2_topoEM;   //!
   TBranch        *b_cl_nemb3_topoEM;   //!
   TBranch        *b_cl_phi2_topoEM;   //!
   TBranch        *b_cl_eta0_topoEM;   //!
   TBranch        *b_cl_eta1_topoEM;   //!
   TBranch        *b_cl_eta2_topoEM;   //!
   TBranch        *b_cl_eta3_topoEM;   //!
   TBranch        *b_cl_etileg1_topoEM;   //!
   TBranch        *b_cl_etileg2_topoEM;   //!
   TBranch        *b_cl_etileg3_topoEM;   //!
   TBranch        *b_cl_ntileg1_topoEM;   //!
   TBranch        *b_cl_ntileg2_topoEM;   //!
   TBranch        *b_cl_ntileg3_topoEM;   //!
   TBranch        *b_cl_eta_topoEM;   //!
   TBranch        *b_cl_phi_topoEM;   //!
   TBranch        *b_cl_reco_stat_topoEM;   //!
   TBranch        *b_cl_m1_eta_topoEM;   //!
   TBranch        *b_cl_m1_phi_topoEM;   //!
   TBranch        *b_cl_m2_r_topoEM;   //!
   TBranch        *b_cl_m2_lambda_topoEM;   //!
   TBranch        *b_cl_delta_phi_topoEM;   //!
   TBranch        *b_cl_delta_theta_topoEM;   //!
   TBranch        *b_cl_delta_alpha_topoEM;   //!
   TBranch        *b_cl_center_x_topoEM;   //!
   TBranch        *b_cl_center_y_topoEM;   //!
   TBranch        *b_cl_center_z_topoEM;   //!
   TBranch        *b_cl_center_lambda_topoEM;   //!
   TBranch        *b_cl_lateral_topoEM;   //!
   TBranch        *b_cl_longitudinal_topoEM;   //!
   TBranch        *b_cl_eng_frac_em_topoEM;   //!
   TBranch        *b_cl_eng_frac_max_topoEM;   //!
   TBranch        *b_cl_eng_frac_core_topoEM;   //!
   TBranch        *b_cl_m1_dens_topoEM;   //!
   TBranch        *b_cl_m2_dens_topoEM;   //!
   TBranch        *b_cl_ntotcells_topoEM;   //!
   TBranch        *b_cl_cells_topoEM;   //!
   TBranch        *b_NCells;   //!
   TBranch        *b_NFebs;   //!
   TBranch        *b_NLayers;   //!
   TBranch        *b_TimeCell;   //!
   TBranch        *b_EnergyCell;   //!
   TBranch        *b_LayerCell;   //!
   TBranch        *b_EtaCell;   //!
   TBranch        *b_PhiCell;   //!
   TBranch        *b_FebIDCell;   //!
   TBranch        *b_TimeLayer;   //!
   TBranch        *b_LayerIDLayer;   //!
   TBranch        *b_TimeFeb;   //!
   TBranch        *b_FebIdFeb;   //!
   TBranch        *b_SlotFeb;   //!
   TBranch        *b_FeedthroughFeb;   //!
   TBranch        *b_Phase;   //!
   TBranch        *b_TimeTotal;   //!
   TBranch        *b_EnergyTotal;   //!
   TBranch        *b_phasetime;   //!
   TBranch        *b_pixel_BCID2;   //!
   TBranch        *b_sct_BCID;   //!
   TBranch        *b_trt_BCID;   //!
   TBranch        *b_pixel_nhits;   //!
   TBranch        *b_pixel_layer;   //!
   TBranch        *b_pixel_phi;   //!
   TBranch        *b_pixel_row;   //!
   TBranch        *b_pixel_col;   //!
   TBranch        *b_pixel_tot;   //!
   TBranch        *b_pixel_BCID;   //!
   TBranch        *b_sct_rdo_nhits;   //!
   TBranch        *b_sct_rdo_strips;   //!
   TBranch        *b_sct_rdo_tbin;   //!
   TBranch        *b_sct_rdo_rdoId;   //!
   TBranch        *b_sct_rdo_layer;   //!
   TBranch        *b_sct_rdo_eta;   //!
   TBranch        *b_sct_rdo_phi;   //!
   TBranch        *b_sct_rdo_side;   //!
   TBranch        *b_sct_rdo_errorhit;   //!
   TBranch        *b_sct_rdo_SyncronizationError;   //!
   TBranch        *b_sct_rdo_PreambleError;   //!
   TBranch        *b_sct_rdo_LVL1Error;   //!
   TBranch        *b_sct_rdo_BCIDError;   //!
   TBranch        *b_sct_rdo_FormatterError;   //!
   TBranch        *b_sct_rdo_FirstSecondHitError;   //!
   TBranch        *b_trt_rdo_nrdos;   //!
   TBranch        *b_trt_rdo_drifttime;   //!
   TBranch        *b_trt_rdo_timeoverthr;   //!
   TBranch        *b_trt_rdo_highlevel;   //!
   TBranch        *b_trt_rdo_timepll;   //!
   TBranch        *b_trt_rdo_straw;   //!
   TBranch        *b_trt_rdo_strawlayer;   //!
   TBranch        *b_trt_rdo_layer;   //!
   TBranch        *b_trt_rdo_phisector;   //!
   TBranch        *b_sct_clus_nclusters;   //!
   TBranch        *b_sct_clus_x;   //!
   TBranch        *b_sct_clus_y;   //!
   TBranch        *b_sct_clus_z;   //!
   TBranch        *b_sct_clus_groupsize;   //!
   TBranch        *b_sct_clus_layer;   //!
   TBranch        *b_sct_clus_eta;   //!
   TBranch        *b_sct_clus_phi;   //!
   TBranch        *b_sct_clus_side;   //!
   TBranch        *b_pixel_clus_nclusters;   //!
   TBranch        *b_pixel_clus_LocX;   //!
   TBranch        *b_pixel_clus_LocY;   //!
   TBranch        *b_pixel_clus_groupsize;   //!
   TBranch        *b_pixel_clus_layer;   //!
   TBranch        *b_pixel_clus_phi;   //!
   TBranch        *b_trt_nDCs;   //!
   TBranch        *b_trt_DC_timeoverthr;   //!
   TBranch        *b_trt_DC_highlevel;   //!
   TBranch        *b_trt_DC_radius;   //!
   TBranch        *b_trt_DC_straw;   //!
   TBranch        *b_trt_DC_strawlayer;   //!
   TBranch        *b_trt_DC_layer;   //!
   TBranch        *b_trt_DC_phisector;   //!
   TBranch        *b_trt_DC_straw_y;   //!
   TBranch        *b_trt_DC_straw_x;   //!
   TBranch        *b_sct_sp_nspacepoints;   //!
   TBranch        *b_sct_sp_x;   //!
   TBranch        *b_sct_sp_y;   //!
   TBranch        *b_sct_sp_z;   //!
   TBranch        *b_sct_sp_layer;   //!
   TBranch        *b_sct_sp_phi;   //!
   TBranch        *b_pixel_sp_nspacepoints;   //!
   TBranch        *b_pixel_sp_x;   //!
   TBranch        *b_pixel_sp_y;   //!
   TBranch        *b_pixel_sp_z;   //!
   TBranch        *b_pixel_sp_layer;   //!
   TBranch        *b_pixel_sp_phi;   //!
   TBranch        *b_trk_nTracks;   //!
   TBranch        *b_trk_nPixelHits;   //!
   TBranch        *b_trk_PixResPhi;   //!
   TBranch        *b_trk_PixResEta;   //!
   TBranch        *b_trk_PixPullPhi;   //!
   TBranch        *b_trk_PixPullEta;   //!
   TBranch        *b_trk_PixErrTrkPhi;   //!
   TBranch        *b_trk_PixErrTrkEta;   //!
   TBranch        *b_trk_PixErrHitPhi;   //!
   TBranch        *b_trk_PixErrHitEta;   //!
   TBranch        *b_trk_Pix_layer;   //!
   TBranch        *b_trk_Sct_eta;   //!
   TBranch        *b_trk_Pix_phi;   //!
   TBranch        *b_trk_Pix_clusWidthRow;   //!
   TBranch        *b_trk_Pix_clusWidthCol;   //!
   TBranch        *b_trk_nSctHits;   //!
   TBranch        *b_trk_SctRes;   //!
   TBranch        *b_trk_SctPull;   //!
   TBranch        *b_trk_SctErrTrk;   //!
   TBranch        *b_trk_SctErrHit;   //!
   TBranch        *b_trk_Sct_layer;   //!
   TBranch        *b_trk_Sct_phi;   //!
   TBranch        *b_trk_Sct_side;   //!
   TBranch        *b_trk_Sct_rdoId;   //!
   TBranch        *b_trk_Sct_clusGroupsize;   //!
   TBranch        *b_trk_Sct_clusx;   //!
   TBranch        *b_trk_Sct_clusy;   //!
   TBranch        *b_trk_Sct_clusz;   //!
   TBranch        *b_trk_nTrtHits;   //!
   TBranch        *b_trk_TrtRes;   //!
   TBranch        *b_trk_TrtPull;   //!
   TBranch        *b_trk_TrtErrTrk;   //!
   TBranch        *b_trk_TrtErrHit;   //!
   TBranch        *b_trk_Trt_R;   //!
   TBranch        *b_trk_Trt_Rtrack;   //!
   TBranch        *b_trk_Trt_t;   //!
   TBranch        *b_trk_Trt_x;   //!
   TBranch        *b_trk_Trt_y;   //!
   TBranch        *b_trk_Trt_HL;   //!
   TBranch        *b_trk_Trt_strawlayer;   //!
   TBranch        *b_trk_Trt_layer;   //!
   TBranch        *b_trk_Trt_straw;   //!
   TBranch        *b_trk_Trt_phisector;   //!
   TBranch        *b_trk_Rec_d0;   //!
   TBranch        *b_trk_Rec_z0;   //!
   TBranch        *b_trk_Rec_phi0;   //!
   TBranch        *b_trk_Rec_theta0;   //!
   TBranch        *b_trk_Rec_qoverp;   //!
   TBranch        *b_trk_Rec_chi2;   //!
   TBranch        *b_trk_Rec_ndof;   //!
   TBranch        *b_trk_Rec_eta;   //!
   TBranch        *b_trk_Rec_error_d0;   //!
   TBranch        *b_trk_Rec_error_z0;   //!
   TBranch        *b_trk_Rec_error_phi0;   //!
   TBranch        *b_trk_Rec_error_theta0;   //!
   TBranch        *b_trk_Rec_error_qoverp;   //!
   TBranch        *b_trk_Rec_chi2prob;   //!
   TBranch        *b_trk_nTracksSi;   //!
   TBranch        *b_trk_nPixelHitsSi;   //!
   TBranch        *b_trk_PixResPhiSi;   //!
   TBranch        *b_trk_PixResEtaSi;   //!
   TBranch        *b_trk_PixPullPhiSi;   //!
   TBranch        *b_trk_PixPullEtaSi;   //!
   TBranch        *b_trk_PixErrTrkPhiSi;   //!
   TBranch        *b_trk_PixErrTrkEtaSi;   //!
   TBranch        *b_trk_PixErrHitPhiSi;   //!
   TBranch        *b_trk_PixErrHitEtaSi;   //!
   TBranch        *b_trk_Pix_layerSi;   //!
   TBranch        *b_trk_Sct_etaSi;   //!
   TBranch        *b_trk_Pix_phiSi;   //!
   TBranch        *b_trk_Pix_clusWidthRowSi;   //!
   TBranch        *b_trk_Pix_clusWidthColSi;   //!
   TBranch        *b_trk_nSctHitsSi;   //!
   TBranch        *b_trk_SctResSi;   //!
   TBranch        *b_trk_SctPullSi;   //!
   TBranch        *b_trk_SctErrTrkSi;   //!
   TBranch        *b_trk_SctErrHitSi;   //!
   TBranch        *b_trk_Sct_layerSi;   //!
   TBranch        *b_trk_Sct_phiSi;   //!
   TBranch        *b_trk_Sct_sideSi;   //!
   TBranch        *b_trk_Sct_rdoIdSi;   //!
   TBranch        *b_trk_Sct_clusGroupsizeSi;   //!
   TBranch        *b_trk_Sct_clusxSi;   //!
   TBranch        *b_trk_Sct_clusySi;   //!
   TBranch        *b_trk_Sct_cluszSi;   //!
   TBranch        *b_trk_nTrtHitsSi;   //!
   TBranch        *b_trk_TrtResSi;   //!
   TBranch        *b_trk_TrtPullSi;   //!
   TBranch        *b_trk_TrtErrTrkSi;   //!
   TBranch        *b_trk_TrtErrHitSi;   //!
   TBranch        *b_trk_Trt_RSi;   //!
   TBranch        *b_trk_Trt_RtrackSi;   //!
   TBranch        *b_trk_Trt_tSi;   //!
   TBranch        *b_trk_Trt_xSi;   //!
   TBranch        *b_trk_Trt_ySi;   //!
   TBranch        *b_trk_Trt_HLSi;   //!
   TBranch        *b_trk_Trt_strawlayerSi;   //!
   TBranch        *b_trk_Trt_layerSi;   //!
   TBranch        *b_trk_Trt_strawSi;   //!
   TBranch        *b_trk_Trt_phisectorSi;   //!
   TBranch        *b_trk_Rec_d0Si;   //!
   TBranch        *b_trk_Rec_z0Si;   //!
   TBranch        *b_trk_Rec_phi0Si;   //!
   TBranch        *b_trk_Rec_theta0Si;   //!
   TBranch        *b_trk_Rec_qoverpSi;   //!
   TBranch        *b_trk_Rec_chi2Si;   //!
   TBranch        *b_trk_Rec_ndofSi;   //!
   TBranch        *b_trk_Rec_etaSi;   //!
   TBranch        *b_trk_Rec_error_d0Si;   //!
   TBranch        *b_trk_Rec_error_z0Si;   //!
   TBranch        *b_trk_Rec_error_phi0Si;   //!
   TBranch        *b_trk_Rec_error_theta0Si;   //!
   TBranch        *b_trk_Rec_error_qoverpSi;   //!
   TBranch        *b_trk_Rec_chi2probSi;   //!
   TBranch        *b_trk_nTracksPixel;   //!
   TBranch        *b_trk_nPixelHitsPixel;   //!
   TBranch        *b_trk_PixResPhiPixel;   //!
   TBranch        *b_trk_PixResEtaPixel;   //!
   TBranch        *b_trk_PixPullPhiPixel;   //!
   TBranch        *b_trk_PixPullEtaPixel;   //!
   TBranch        *b_trk_PixErrTrkPhiPixel;   //!
   TBranch        *b_trk_PixErrTrkEtaPixel;   //!
   TBranch        *b_trk_PixErrHitPhiPixel;   //!
   TBranch        *b_trk_PixErrHitEtaPixel;   //!
   TBranch        *b_trk_Pix_layerPixel;   //!
   TBranch        *b_trk_Sct_etaPixel;   //!
   TBranch        *b_trk_Pix_phiPixel;   //!
   TBranch        *b_trk_Pix_clusWidthRowPixel;   //!
   TBranch        *b_trk_Pix_clusWidthColPixel;   //!
   TBranch        *b_trk_nSctHitsPixel;   //!
   TBranch        *b_trk_SctResPixel;   //!
   TBranch        *b_trk_SctPullPixel;   //!
   TBranch        *b_trk_SctErrTrkPixel;   //!
   TBranch        *b_trk_SctErrHitPixel;   //!
   TBranch        *b_trk_Sct_layerPixel;   //!
   TBranch        *b_trk_Sct_phiPixel;   //!
   TBranch        *b_trk_Sct_sidePixel;   //!
   TBranch        *b_trk_Sct_rdoIdPixel;   //!
   TBranch        *b_trk_Sct_clusGroupsizePixel;   //!
   TBranch        *b_trk_Sct_clusxPixel;   //!
   TBranch        *b_trk_Sct_clusyPixel;   //!
   TBranch        *b_trk_Sct_cluszPixel;   //!
   TBranch        *b_trk_nTrtHitsPixel;   //!
   TBranch        *b_trk_TrtResPixel;   //!
   TBranch        *b_trk_TrtPullPixel;   //!
   TBranch        *b_trk_TrtErrTrkPixel;   //!
   TBranch        *b_trk_TrtErrHitPixel;   //!
   TBranch        *b_trk_Trt_RPixel;   //!
   TBranch        *b_trk_Trt_RtrackPixel;   //!
   TBranch        *b_trk_Trt_tPixel;   //!
   TBranch        *b_trk_Trt_xPixel;   //!
   TBranch        *b_trk_Trt_yPixel;   //!
   TBranch        *b_trk_Trt_HLPixel;   //!
   TBranch        *b_trk_Trt_strawlayerPixel;   //!
   TBranch        *b_trk_Trt_layerPixel;   //!
   TBranch        *b_trk_Trt_strawPixel;   //!
   TBranch        *b_trk_Trt_phisectorPixel;   //!
   TBranch        *b_trk_Rec_d0Pixel;   //!
   TBranch        *b_trk_Rec_z0Pixel;   //!
   TBranch        *b_trk_Rec_phi0Pixel;   //!
   TBranch        *b_trk_Rec_theta0Pixel;   //!
   TBranch        *b_trk_Rec_qoverpPixel;   //!
   TBranch        *b_trk_Rec_chi2Pixel;   //!
   TBranch        *b_trk_Rec_ndofPixel;   //!
   TBranch        *b_trk_Rec_etaPixel;   //!
   TBranch        *b_trk_Rec_error_d0Pixel;   //!
   TBranch        *b_trk_Rec_error_z0Pixel;   //!
   TBranch        *b_trk_Rec_error_phi0Pixel;   //!
   TBranch        *b_trk_Rec_error_theta0Pixel;   //!
   TBranch        *b_trk_Rec_error_qoverpPixel;   //!
   TBranch        *b_trk_Rec_chi2probPixel;   //!
   TBranch        *b_trk_nTracksSCT;   //!
   TBranch        *b_trk_nPixelHitsSCT;   //!
   TBranch        *b_trk_PixResPhiSCT;   //!
   TBranch        *b_trk_PixResEtaSCT;   //!
   TBranch        *b_trk_PixPullPhiSCT;   //!
   TBranch        *b_trk_PixPullEtaSCT;   //!
   TBranch        *b_trk_PixErrTrkPhiSCT;   //!
   TBranch        *b_trk_PixErrTrkEtaSCT;   //!
   TBranch        *b_trk_PixErrHitPhiSCT;   //!
   TBranch        *b_trk_PixErrHitEtaSCT;   //!
   TBranch        *b_trk_Pix_layerSCT;   //!
   TBranch        *b_trk_Sct_etaSCT;   //!
   TBranch        *b_trk_Pix_phiSCT;   //!
   TBranch        *b_trk_Pix_clusWidthRowSCT;   //!
   TBranch        *b_trk_Pix_clusWidthColSCT;   //!
   TBranch        *b_trk_nSctHitsSCT;   //!
   TBranch        *b_trk_SctResSCT;   //!
   TBranch        *b_trk_SctPullSCT;   //!
   TBranch        *b_trk_SctErrTrkSCT;   //!
   TBranch        *b_trk_SctErrHitSCT;   //!
   TBranch        *b_trk_Sct_layerSCT;   //!
   TBranch        *b_trk_Sct_phiSCT;   //!
   TBranch        *b_trk_Sct_sideSCT;   //!
   TBranch        *b_trk_Sct_rdoIdSCT;   //!
   TBranch        *b_trk_Sct_clusGroupsizeSCT;   //!
   TBranch        *b_trk_Sct_clusxSCT;   //!
   TBranch        *b_trk_Sct_clusySCT;   //!
   TBranch        *b_trk_Sct_cluszSCT;   //!
   TBranch        *b_trk_nTrtHitsSCT;   //!
   TBranch        *b_trk_TrtResSCT;   //!
   TBranch        *b_trk_TrtPullSCT;   //!
   TBranch        *b_trk_TrtErrTrkSCT;   //!
   TBranch        *b_trk_TrtErrHitSCT;   //!
   TBranch        *b_trk_Trt_RSCT;   //!
   TBranch        *b_trk_Trt_RtrackSCT;   //!
   TBranch        *b_trk_Trt_tSCT;   //!
   TBranch        *b_trk_Trt_xSCT;   //!
   TBranch        *b_trk_Trt_ySCT;   //!
   TBranch        *b_trk_Trt_HLSCT;   //!
   TBranch        *b_trk_Trt_strawlayerSCT;   //!
   TBranch        *b_trk_Trt_layerSCT;   //!
   TBranch        *b_trk_Trt_strawSCT;   //!
   TBranch        *b_trk_Trt_phisectorSCT;   //!
   TBranch        *b_trk_Rec_d0SCT;   //!
   TBranch        *b_trk_Rec_z0SCT;   //!
   TBranch        *b_trk_Rec_phi0SCT;   //!
   TBranch        *b_trk_Rec_theta0SCT;   //!
   TBranch        *b_trk_Rec_qoverpSCT;   //!
   TBranch        *b_trk_Rec_chi2SCT;   //!
   TBranch        *b_trk_Rec_ndofSCT;   //!
   TBranch        *b_trk_Rec_etaSCT;   //!
   TBranch        *b_trk_Rec_error_d0SCT;   //!
   TBranch        *b_trk_Rec_error_z0SCT;   //!
   TBranch        *b_trk_Rec_error_phi0SCT;   //!
   TBranch        *b_trk_Rec_error_theta0SCT;   //!
   TBranch        *b_trk_Rec_error_qoverpSCT;   //!
   TBranch        *b_trk_Rec_chi2probSCT;   //!
   TBranch        *b_trk_nTracksTRT;   //!
   TBranch        *b_trk_nPixelHitsTRT;   //!
   TBranch        *b_trk_PixResPhiTRT;   //!
   TBranch        *b_trk_PixResEtaTRT;   //!
   TBranch        *b_trk_PixPullPhiTRT;   //!
   TBranch        *b_trk_PixPullEtaTRT;   //!
   TBranch        *b_trk_PixErrTrkPhiTRT;   //!
   TBranch        *b_trk_PixErrTrkEtaTRT;   //!
   TBranch        *b_trk_PixErrHitPhiTRT;   //!
   TBranch        *b_trk_PixErrHitEtaTRT;   //!
   TBranch        *b_trk_Pix_layerTRT;   //!
   TBranch        *b_trk_Sct_etaTRT;   //!
   TBranch        *b_trk_Pix_phiTRT;   //!
   TBranch        *b_trk_Pix_clusWidthRowTRT;   //!
   TBranch        *b_trk_Pix_clusWidthColTRT;   //!
   TBranch        *b_trk_nSctHitsTRT;   //!
   TBranch        *b_trk_SctResTRT;   //!
   TBranch        *b_trk_SctPullTRT;   //!
   TBranch        *b_trk_SctErrTrkTRT;   //!
   TBranch        *b_trk_SctErrHitTRT;   //!
   TBranch        *b_trk_Sct_layerTRT;   //!
   TBranch        *b_trk_Sct_phiTRT;   //!
   TBranch        *b_trk_Sct_sideTRT;   //!
   TBranch        *b_trk_Sct_rdoIdTRT;   //!
   TBranch        *b_trk_Sct_clusGroupsizeTRT;   //!
   TBranch        *b_trk_Sct_clusxTRT;   //!
   TBranch        *b_trk_Sct_clusyTRT;   //!
   TBranch        *b_trk_Sct_cluszTRT;   //!
   TBranch        *b_trk_nTrtHitsTRT;   //!
   TBranch        *b_trk_TrtResTRT;   //!
   TBranch        *b_trk_TrtPullTRT;   //!
   TBranch        *b_trk_TrtErrTrkTRT;   //!
   TBranch        *b_trk_TrtErrHitTRT;   //!
   TBranch        *b_trk_Trt_RTRT;   //!
   TBranch        *b_trk_Trt_RtrackTRT;   //!
   TBranch        *b_trk_Trt_tTRT;   //!
   TBranch        *b_trk_Trt_xTRT;   //!
   TBranch        *b_trk_Trt_yTRT;   //!
   TBranch        *b_trk_Trt_HLTRT;   //!
   TBranch        *b_trk_Trt_strawlayerTRT;   //!
   TBranch        *b_trk_Trt_layerTRT;   //!
   TBranch        *b_trk_Trt_strawTRT;   //!
   TBranch        *b_trk_Trt_phisectorTRT;   //!
   TBranch        *b_trk_Rec_d0TRT;   //!
   TBranch        *b_trk_Rec_z0TRT;   //!
   TBranch        *b_trk_Rec_phi0TRT;   //!
   TBranch        *b_trk_Rec_theta0TRT;   //!
   TBranch        *b_trk_Rec_qoverpTRT;   //!
   TBranch        *b_trk_Rec_chi2TRT;   //!
   TBranch        *b_trk_Rec_ndofTRT;   //!
   TBranch        *b_trk_Rec_etaTRT;   //!
   TBranch        *b_trk_Rec_error_d0TRT;   //!
   TBranch        *b_trk_Rec_error_z0TRT;   //!
   TBranch        *b_trk_Rec_error_phi0TRT;   //!
   TBranch        *b_trk_Rec_error_theta0TRT;   //!
   TBranch        *b_trk_Rec_error_qoverpTRT;   //!
   TBranch        *b_trk_Rec_chi2probTRT;   //!
   TBranch        *b_Trk_totalNumTracks;   //!
   TBranch        *b_Trk_d0;   //!
   TBranch        *b_Trk_z0;   //!
   TBranch        *b_Trk_phi;   //!
   TBranch        *b_Trk_theta;   //!
   TBranch        *b_Trk_qOverP;   //!
   TBranch        *b_Trk_Chi2;   //!
   TBranch        *b_Trk_Ndf;   //!
   TBranch        *b_Trk_sigd0;   //!
   TBranch        *b_Trk_sigz0;   //!
   TBranch        *b_Trk_sigphi;   //!
   TBranch        *b_Trk_sigtheta;   //!
   TBranch        *b_Trk_sigqOverP;   //!
   TBranch        *b_Trk_covVert21;   //!
   TBranch        *b_Trk_covVert31;   //!
   TBranch        *b_Trk_covVert32;   //!
   TBranch        *b_Trk_covVert41;   //!
   TBranch        *b_Trk_covVert42;   //!
   TBranch        *b_Trk_covVert43;   //!
   TBranch        *b_Trk_covVert51;   //!
   TBranch        *b_Trk_covVert52;   //!
   TBranch        *b_Trk_covVert53;   //!
   TBranch        *b_Trk_covVert54;   //!
   TBranch        *b_Trk_px;   //!
   TBranch        *b_Trk_py;   //!
   TBranch        *b_Trk_pz;   //!
   TBranch        *b_Trk_pt;   //!
   TBranch        *b_Trk_numberOfBLayerHits;   //!
   TBranch        *b_Trk_numberOfPixelHits;   //!
   TBranch        *b_Trk_numberOfSCTHits;   //!
   TBranch        *b_Trk_numberOfTRTHits;   //!
   TBranch        *b_Trk_numberOfTRTHighThresholdHits;   //!
   TBranch        *b_Trk_numberOfBLayerSharedHits;   //!
   TBranch        *b_Trk_numberOfPixelSharedHits;   //!
   TBranch        *b_Trk_numberOfPixelHoles;   //!
   TBranch        *b_Trk_numberOfSCTSharedHits;   //!
   TBranch        *b_Trk_numberOfSCTHoles;   //!
   TBranch        *b_Trk_numberOfTRTOutliers;   //!
   TBranch        *b_Trk_numberOfTRTHighThresholdOutliers;   //!
   TBranch        *b_Trk_numberOfMdtHits;   //!
   TBranch        *b_Trk_numberOfTgcPhiHits;   //!
   TBranch        *b_Trk_numberOfRpcPhiHits;   //!
   TBranch        *b_Trk_numberOfCscPhiHits;   //!
   TBranch        *b_Trk_numberOfTgcEtaHits;   //!
   TBranch        *b_Trk_numberOfRpcEtaHits;   //!
   TBranch        *b_Trk_numberOfCscEtaHits;   //!
   TBranch        *b_Trk_numberOfGangedPixels;   //!
   TBranch        *b_Trk_numberOfOutliersOnTrack;   //!
   TBranch        *b_Trk_numberOfTrackSummaryTypes;   //!
   TBranch        *b_Trk_truthBarcode;   //!
   TBranch        *b_Trk_truthNt;   //!
   TBranch        *b_Trk_ParticlePdg;   //!
   TBranch        *b_Trk_ParentPdg;   //!
   TBranch        *b_IIC_InDet_NImpact;   //!
   TBranch        *b_IIC_InDet_x0;   //!
   TBranch        *b_IIC_InDet_y0;   //!
   TBranch        *b_IIC_InDet_z0;   //!
   TBranch        *b_IIC_InDet_eta0_local;   //!
   TBranch        *b_IIC_InDet_phi0_local;   //!
   TBranch        *b_IIC_InDet_trketa_at0;   //!
   TBranch        *b_IIC_InDet_trkphi_at0;   //!
   TBranch        *b_IIC_InDet_x1;   //!
   TBranch        *b_IIC_InDet_y1;   //!
   TBranch        *b_IIC_InDet_z1;   //!
   TBranch        *b_IIC_InDet_eta1_local;   //!
   TBranch        *b_IIC_InDet_phi1_local;   //!
   TBranch        *b_IIC_InDet_trketa_at1;   //!
   TBranch        *b_IIC_InDet_trkphi_at1;   //!
   TBranch        *b_IIC_InDet_x2;   //!
   TBranch        *b_IIC_InDet_y2;   //!
   TBranch        *b_IIC_InDet_z2;   //!
   TBranch        *b_IIC_InDet_eta2_local;   //!
   TBranch        *b_IIC_InDet_phi2_local;   //!
   TBranch        *b_IIC_InDet_trketa_at2;   //!
   TBranch        *b_IIC_InDet_trkphi_at2;   //!
   TBranch        *b_IIC_InDet_x3;   //!
   TBranch        *b_IIC_InDet_y3;   //!
   TBranch        *b_IIC_InDet_z3;   //!
   TBranch        *b_IIC_InDet_eta3_local;   //!
   TBranch        *b_IIC_InDet_phi3_local;   //!
   TBranch        *b_IIC_InDet_trketa_at3;   //!
   TBranch        *b_IIC_InDet_trkphi_at3;   //!
   TBranch        *b_IIC_InDet_x_tile;   //!
   TBranch        *b_IIC_InDet_y_tile;   //!
   TBranch        *b_IIC_InDet_z_tile;   //!
   TBranch        *b_IIC_InDet_eta_tile_local;   //!
   TBranch        *b_IIC_InDet_phi_tile_local;   //!
   TBranch        *b_IIC_InDet_trketa_attile;   //!
   TBranch        *b_IIC_InDet_trkphi_attile;   //!

   TB_tree(TTree *tree=0);
   virtual ~TB_tree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual int      Loop(int evtToFind);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

// generated by:
// $ clid -m "TB_tree"
CLASS_DEF( TB_tree , 105243974 , 1 )

#endif
