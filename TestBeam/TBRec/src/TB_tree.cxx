/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TB_tree.h"

TB_tree::TB_tree(TTree *tree)
  : fChain(0),
    b_Run(0),
    b_Event(0),
    b_Time(0),
    b_Weight(0),
    b_IEvent(0),
    b_RunEta(0),
    b_RunPartType(0),
    b_RunEnergy(0),
    b_Energy(0),
    b_ErrAbsEnergy(0),
    b_ErrCollimators(0),
    b_ErrCurrents(0),
    b_SyncLoss(0),
    b_ErrSyncLoss(0),
    b_Quad_nc(0),
    b_Quad_file(0),
    b_Quad_equip(0),
    b_Bend_nc(0),
    b_Bend_file(0),
    b_Bend_equip(0),
    b_Trim_nc(0),
    b_Trim_file(0),
    b_Trim_equip(0),
    b_Coll_nc(0),
    b_Coll_file(0),
    b_Coll_equip(0),
    b_sADC_S0(0),
    b_sTDC_S0(0),
    b_sADC_S1(0),
    b_sTDC_S1(0),
    b_sADC_S2mUp(0),
    b_sTDC_S2mUp(0),
    b_sADC_S2mDown(0),
    b_sTDC_S2mDown(0),
    b_sADC_S3mLeft(0),
    b_sTDC_S3mLeft(0),
    b_sADC_S3mRight(0),
    b_sTDC_S3mRight(0),
    b_sADC_C1(0),
    b_sTDC_C1(0),
    b_sADC_C2(0),
    b_sTDC_C2(0),
    b_sADC_muTag(0),
    b_sTDC_muTag(0),
    b_sADC_muHalo(0),
    b_sTDC_muHalo(0),
    b_sADC_muVeto(0),
    b_sTDC_muVeto(0),
    b_sADC_SC1(0),
    b_sTDC_SC1(0),
    b_sADC_SC2(0),
    b_sTDC_SC2(0),
    b_sADC_TRTSci(0),
    b_sTDC_TRTSci(0),
    b_bpcX_BPCm2(0),
    b_bpcY_BPCm2(0),
    b_bpcX_BPCm1(0),
    b_bpcY_BPCm1(0),
    b_bpcX_BPC0(0),
    b_bpcY_BPC0(0),
    b_bpcX_BPC1(0),
    b_bpcY_BPC1(0),
    b_bpcX_BPC2(0),
    b_bpcY_BPC2(0),
    b_Trigger(0),
    b_Clock(0),
    b_Nh_Calo(0),
    b_Eh_Calo(0),
    b_Nh_EM(0),
    b_Eh_EM(0),
    b_Nh_HAD(0),
    b_Eh_HAD(0),
    b_Nh_PresB(0),
    b_Eh_PresB(0),
    b_Nh_EMB(0),
    b_Eh_EMB(0),
    b_Nh_EMEC(0),
    b_Eh_EMEC(0),
    b_Nh_Tile(0),
    b_Eh_Tile(0),
    b_Nh_TileGap(0),
    b_Eh_TileGap(0),
    b_Nh_HEC(0),
    b_Eh_HEC(0),
    b_Nh_FCal(0),
    b_Eh_FCal(0),
    b_Nh_PresE(0),
    b_Eh_PresE(0),
    b_Nh_Scint(0),
    b_Eh_Scint(0),
    b_nhit_lar(0),
    b_ecell_lar(0),
    b_NCells_lar(0),
    b_ECells_lar(0),
    b_EtaCells_lar(0),
    b_PhiCells_lar(0),
    b_QCells_lar(0),
    b_GainCells_lar(0),
    b_DetCells_lar(0),
    b_TimeCells_lar(0),
    b_BadCells_lar(0),
    b_nhit_tile(0),
    b_ecell_tile(0),
    b_NCells_tile(0),
    b_ECells_tile(0),
    b_EtaCells_tile(0),
    b_PhiCells_tile(0),
    b_QCells_tile(0),
    b_GainCells_tile(0),
    b_DetCells_tile(0),
    b_TimeCells_tile(0),
    b_cl_ecluster_em(0),
    b_cl_nctotal_em(0),
    b_cl_nc_em(0),
    b_cl_et_em(0),
    b_cl_e_em(0),
    b_cl_eemb0_em(0),
    b_cl_eemb1_em(0),
    b_cl_eemb2_em(0),
    b_cl_eemb3_em(0),
    b_cl_phi2_em(0),
    b_cl_eta0_em(0),
    b_cl_eta1_em(0),
    b_cl_eta2_em(0),
    b_cl_eta3_em(0),
    b_cl_etileg1_em(0),
    b_cl_etileg2_em(0),
    b_cl_etileg3_em(0),
    b_cl_eta_em(0),
    b_cl_phi_em(0),
    b_cl_reco_stat_em(0),
    b_cl_m1_eta_em(0),
    b_cl_m1_phi_em(0),
    b_cl_m2_r_em(0),
    b_cl_m2_lambda_em(0),
    b_cl_delta_phi_em(0),
    b_cl_delta_theta_em(0),
    b_cl_delta_alpha_em(0),
    b_cl_center_x_em(0),
    b_cl_center_y_em(0),
    b_cl_center_z_em(0),
    b_cl_center_lambda_em(0),
    b_cl_lateral_em(0),
    b_cl_longitudinal_em(0),
    b_cl_eng_frac_em_em(0),
    b_cl_eng_frac_max_em(0),
    b_cl_eng_frac_core_em(0),
    b_cl_m1_dens_em(0),
    b_cl_m2_dens_em(0),
    b_cl_ntotcells_em(0),
    b_cl_cells_em(0),
    b_cl_ecluster_tb_em(0),
    b_cl_nctotal_tb_em(0),
    b_cl_nc_tb_em(0),
    b_cl_et_tb_em(0),
    b_cl_e_tb_em(0),
    b_cl_eemb0_tb_em(0),
    b_cl_eemb1_tb_em(0),
    b_cl_eemb2_tb_em(0),
    b_cl_eemb3_tb_em(0),
    b_cl_phi2_tb_em(0),
    b_cl_eta0_tb_em(0),
    b_cl_eta1_tb_em(0),
    b_cl_eta2_tb_em(0),
    b_cl_eta3_tb_em(0),
    b_cl_etileg1_tb_em(0),
    b_cl_etileg2_tb_em(0),
    b_cl_etileg3_tb_em(0),
    b_cl_eta_tb_em(0),
    b_cl_phi_tb_em(0),
    b_cl_reco_stat_tb_em(0),
    b_cl_m1_eta_tb_em(0),
    b_cl_m1_phi_tb_em(0),
    b_cl_m2_r_tb_em(0),
    b_cl_m2_lambda_tb_em(0),
    b_cl_delta_phi_tb_em(0),
    b_cl_delta_theta_tb_em(0),
    b_cl_delta_alpha_tb_em(0),
    b_cl_center_x_tb_em(0),
    b_cl_center_y_tb_em(0),
    b_cl_center_z_tb_em(0),
    b_cl_center_lambda_tb_em(0),
    b_cl_lateral_tb_em(0),
    b_cl_longitudinal_tb_em(0),
    b_cl_eng_frac_em_tb_em(0),
    b_cl_eng_frac_max_tb_em(0),
    b_cl_eng_frac_core_tb_em(0),
    b_cl_m1_dens_tb_em(0),
    b_cl_m2_dens_tb_em(0),
    b_cl_ntotcells_tb_em(0),
    b_cl_cells_tb_em(0),
    b_cl_ecluster_topo(0),
    b_cl_nctotal_topo(0),
    b_cl_nc_topo(0),
    b_cl_et_topo(0),
    b_cl_e_topo(0),
    b_cl_eemb0_topo(0),
    b_cl_eemb1_topo(0),
    b_cl_eemb2_topo(0),
    b_cl_eemb3_topo(0),
    b_cl_nemb0_topo(0),
    b_cl_nemb1_topo(0),
    b_cl_nemb2_topo(0),
    b_cl_nemb3_topo(0),
    b_cl_phi2_topo(0),
    b_cl_eta0_topo(0),
    b_cl_eta1_topo(0),
    b_cl_eta2_topo(0),
    b_cl_eta3_topo(0),
    b_cl_etileg1_topo(0),
    b_cl_etileg2_topo(0),
    b_cl_etileg3_topo(0),
    b_cl_ntileg1_topo(0),
    b_cl_ntileg2_topo(0),
    b_cl_ntileg3_topo(0),
    b_cl_eta_topo(0),
    b_cl_phi_topo(0),
    b_cl_reco_stat_topo(0),
    b_cl_m1_eta_topo(0),
    b_cl_m1_phi_topo(0),
    b_cl_m2_r_topo(0),
    b_cl_m2_lambda_topo(0),
    b_cl_delta_phi_topo(0),
    b_cl_delta_theta_topo(0),
    b_cl_delta_alpha_topo(0),
    b_cl_center_x_topo(0),
    b_cl_center_y_topo(0),
    b_cl_center_z_topo(0),
    b_cl_center_lambda_topo(0),
    b_cl_lateral_topo(0),
    b_cl_longitudinal_topo(0),
    b_cl_eng_frac_em_topo(0),
    b_cl_eng_frac_max_topo(0),
    b_cl_eng_frac_core_topo(0),
    b_cl_m1_dens_topo(0),
    b_cl_m2_dens_topo(0),
    b_cl_etileb0_topo(0),
    b_cl_etileb1_topo(0),
    b_cl_etileb2_topo(0),
    b_cl_ntileb0_topo(0),
    b_cl_ntileb1_topo(0),
    b_cl_ntileb2_topo(0),
    b_cl_ntotcells_topo(0),
    b_cl_cells_topo(0),
    b_ncellstotal_topo(0),
    b_ncells_topo(0),
    b_celleng_topo(0),
    b_cellet_topo(0),
    b_celleta_topo(0),
    b_cellphi_topo(0),
    b_cellr_topo(0),
    b_cellz_topo(0),
    b_celldeta_topo(0),
    b_celldphi_topo(0),
    b_celldr_topo(0),
    b_celldz_topo(0),
    b_cellvol_topo(0),
    b_celltime_topo(0),
    b_cellquality_topo(0),
    b_cellweight_topo(0),
    b_cellgain_topo(0),
    b_cellnoise_topo(0),
    b_celltotnoise_topo(0),
    b_cellcluster_topo(0),
    b_cell_calo_topo(0),
    b_cell_side_topo(0),
    b_cell_samp_topo(0),
    b_cell_ireg_topo(0),
    b_cell_ieta_topo(0),
    b_cell_iphi_topo(0),
    b_cl_ecluster_topoEM(0),
    b_cl_nctotal_topoEM(0),
    b_cl_nc_topoEM(0),
    b_cl_et_topoEM(0),
    b_cl_e_topoEM(0),
    b_cl_eemb0_topoEM(0),
    b_cl_eemb1_topoEM(0),
    b_cl_eemb2_topoEM(0),
    b_cl_eemb3_topoEM(0),
    b_cl_nemb0_topoEM(0),
    b_cl_nemb1_topoEM(0),
    b_cl_nemb2_topoEM(0),
    b_cl_nemb3_topoEM(0),
    b_cl_phi2_topoEM(0),
    b_cl_eta0_topoEM(0),
    b_cl_eta1_topoEM(0),
    b_cl_eta2_topoEM(0),
    b_cl_eta3_topoEM(0),
    b_cl_etileg1_topoEM(0),
    b_cl_etileg2_topoEM(0),
    b_cl_etileg3_topoEM(0),
    b_cl_ntileg1_topoEM(0),
    b_cl_ntileg2_topoEM(0),
    b_cl_ntileg3_topoEM(0),
    b_cl_eta_topoEM(0),
    b_cl_phi_topoEM(0),
    b_cl_reco_stat_topoEM(0),
    b_cl_m1_eta_topoEM(0),
    b_cl_m1_phi_topoEM(0),
    b_cl_m2_r_topoEM(0),
    b_cl_m2_lambda_topoEM(0),
    b_cl_delta_phi_topoEM(0),
    b_cl_delta_theta_topoEM(0),
    b_cl_delta_alpha_topoEM(0),
    b_cl_center_x_topoEM(0),
    b_cl_center_y_topoEM(0),
    b_cl_center_z_topoEM(0),
    b_cl_center_lambda_topoEM(0),
    b_cl_lateral_topoEM(0),
    b_cl_longitudinal_topoEM(0),
    b_cl_eng_frac_em_topoEM(0),
    b_cl_eng_frac_max_topoEM(0),
    b_cl_eng_frac_core_topoEM(0),
    b_cl_m1_dens_topoEM(0),
    b_cl_m2_dens_topoEM(0),
    b_cl_ntotcells_topoEM(0),
    b_cl_cells_topoEM(0),
    b_NCells(0),
    b_NFebs(0),
    b_NLayers(0),
    b_TimeCell(0),
    b_EnergyCell(0),
    b_LayerCell(0),
    b_EtaCell(0),
    b_PhiCell(0),
    b_FebIDCell(0),
    b_TimeLayer(0),
    b_LayerIDLayer(0),
    b_TimeFeb(0),
    b_FebIdFeb(0),
    b_SlotFeb(0),
    b_FeedthroughFeb(0),
    b_Phase(0),
    b_TimeTotal(0),
    b_EnergyTotal(0),
    b_phasetime(0),
    b_pixel_BCID2(0),
    b_sct_BCID(0),
    b_trt_BCID(0),
    b_pixel_nhits(0),
    b_pixel_layer(0),
    b_pixel_phi(0),
    b_pixel_row(0),
    b_pixel_col(0),
    b_pixel_tot(0),
    b_pixel_BCID(0),
    b_sct_rdo_nhits(0),
    b_sct_rdo_strips(0),
    b_sct_rdo_tbin(0),
    b_sct_rdo_rdoId(0),
    b_sct_rdo_layer(0),
    b_sct_rdo_eta(0),
    b_sct_rdo_phi(0),
    b_sct_rdo_side(0),
    b_sct_rdo_errorhit(0),
    b_sct_rdo_SyncronizationError(0),
    b_sct_rdo_PreambleError(0),
    b_sct_rdo_LVL1Error(0),
    b_sct_rdo_BCIDError(0),
    b_sct_rdo_FormatterError(0),
    b_sct_rdo_FirstSecondHitError(0),
    b_trt_rdo_nrdos(0),
    b_trt_rdo_drifttime(0),
    b_trt_rdo_timeoverthr(0),
    b_trt_rdo_highlevel(0),
    b_trt_rdo_timepll(0),
    b_trt_rdo_straw(0),
    b_trt_rdo_strawlayer(0),
    b_trt_rdo_layer(0),
    b_trt_rdo_phisector(0),
    b_sct_clus_nclusters(0),
    b_sct_clus_x(0),
    b_sct_clus_y(0),
    b_sct_clus_z(0),
    b_sct_clus_groupsize(0),
    b_sct_clus_layer(0),
    b_sct_clus_eta(0),
    b_sct_clus_phi(0),
    b_sct_clus_side(0),
    b_pixel_clus_nclusters(0),
    b_pixel_clus_LocX(0),
    b_pixel_clus_LocY(0),
    b_pixel_clus_groupsize(0),
    b_pixel_clus_layer(0),
    b_pixel_clus_phi(0),
    b_trt_nDCs(0),
    b_trt_DC_timeoverthr(0),
    b_trt_DC_highlevel(0),
    b_trt_DC_radius(0),
    b_trt_DC_straw(0),
    b_trt_DC_strawlayer(0),
    b_trt_DC_layer(0),
    b_trt_DC_phisector(0),
    b_trt_DC_straw_y(0),
    b_trt_DC_straw_x(0),
    b_sct_sp_nspacepoints(0),
    b_sct_sp_x(0),
    b_sct_sp_y(0),
    b_sct_sp_z(0),
    b_sct_sp_layer(0),
    b_sct_sp_phi(0),
    b_pixel_sp_nspacepoints(0),
    b_pixel_sp_x(0),
    b_pixel_sp_y(0),
    b_pixel_sp_z(0),
    b_pixel_sp_layer(0),
    b_pixel_sp_phi(0),
    b_trk_nTracks(0),
    b_trk_nPixelHits(0),
    b_trk_PixResPhi(0),
    b_trk_PixResEta(0),
    b_trk_PixPullPhi(0),
    b_trk_PixPullEta(0),
    b_trk_PixErrTrkPhi(0),
    b_trk_PixErrTrkEta(0),
    b_trk_PixErrHitPhi(0),
    b_trk_PixErrHitEta(0),
    b_trk_Pix_layer(0),
    b_trk_Sct_eta(0),
    b_trk_Pix_phi(0),
    b_trk_Pix_clusWidthRow(0),
    b_trk_Pix_clusWidthCol(0),
    b_trk_nSctHits(0),
    b_trk_SctRes(0),
    b_trk_SctPull(0),
    b_trk_SctErrTrk(0),
    b_trk_SctErrHit(0),
    b_trk_Sct_layer(0),
    b_trk_Sct_phi(0),
    b_trk_Sct_side(0),
    b_trk_Sct_rdoId(0),
    b_trk_Sct_clusGroupsize(0),
    b_trk_Sct_clusx(0),
    b_trk_Sct_clusy(0),
    b_trk_Sct_clusz(0),
    b_trk_nTrtHits(0),
    b_trk_TrtRes(0),
    b_trk_TrtPull(0),
    b_trk_TrtErrTrk(0),
    b_trk_TrtErrHit(0),
    b_trk_Trt_R(0),
    b_trk_Trt_Rtrack(0),
    b_trk_Trt_t(0),
    b_trk_Trt_x(0),
    b_trk_Trt_y(0),
    b_trk_Trt_HL(0),
    b_trk_Trt_strawlayer(0),
    b_trk_Trt_layer(0),
    b_trk_Trt_straw(0),
    b_trk_Trt_phisector(0),
    b_trk_Rec_d0(0),
    b_trk_Rec_z0(0),
    b_trk_Rec_phi0(0),
    b_trk_Rec_theta0(0),
    b_trk_Rec_qoverp(0),
    b_trk_Rec_chi2(0),
    b_trk_Rec_ndof(0),
    b_trk_Rec_eta(0),
    b_trk_Rec_error_d0(0),
    b_trk_Rec_error_z0(0),
    b_trk_Rec_error_phi0(0),
    b_trk_Rec_error_theta0(0),
    b_trk_Rec_error_qoverp(0),
    b_trk_Rec_chi2prob(0),
    b_trk_nTracksSi(0),
    b_trk_nPixelHitsSi(0),
    b_trk_PixResPhiSi(0),
    b_trk_PixResEtaSi(0),
    b_trk_PixPullPhiSi(0),
    b_trk_PixPullEtaSi(0),
    b_trk_PixErrTrkPhiSi(0),
    b_trk_PixErrTrkEtaSi(0),
    b_trk_PixErrHitPhiSi(0),
    b_trk_PixErrHitEtaSi(0),
    b_trk_Pix_layerSi(0),
    b_trk_Sct_etaSi(0),
    b_trk_Pix_phiSi(0),
    b_trk_Pix_clusWidthRowSi(0),
    b_trk_Pix_clusWidthColSi(0),
    b_trk_nSctHitsSi(0),
    b_trk_SctResSi(0),
    b_trk_SctPullSi(0),
    b_trk_SctErrTrkSi(0),
    b_trk_SctErrHitSi(0),
    b_trk_Sct_layerSi(0),
    b_trk_Sct_phiSi(0),
    b_trk_Sct_sideSi(0),
    b_trk_Sct_rdoIdSi(0),
    b_trk_Sct_clusGroupsizeSi(0),
    b_trk_Sct_clusxSi(0),
    b_trk_Sct_clusySi(0),
    b_trk_Sct_cluszSi(0),
    b_trk_nTrtHitsSi(0),
    b_trk_TrtResSi(0),
    b_trk_TrtPullSi(0),
    b_trk_TrtErrTrkSi(0),
    b_trk_TrtErrHitSi(0),
    b_trk_Trt_RSi(0),
    b_trk_Trt_RtrackSi(0),
    b_trk_Trt_tSi(0),
    b_trk_Trt_xSi(0),
    b_trk_Trt_ySi(0),
    b_trk_Trt_HLSi(0),
    b_trk_Trt_strawlayerSi(0),
    b_trk_Trt_layerSi(0),
    b_trk_Trt_strawSi(0),
    b_trk_Trt_phisectorSi(0),
    b_trk_Rec_d0Si(0),
    b_trk_Rec_z0Si(0),
    b_trk_Rec_phi0Si(0),
    b_trk_Rec_theta0Si(0),
    b_trk_Rec_qoverpSi(0),
    b_trk_Rec_chi2Si(0),
    b_trk_Rec_ndofSi(0),
    b_trk_Rec_etaSi(0),
    b_trk_Rec_error_d0Si(0),
    b_trk_Rec_error_z0Si(0),
    b_trk_Rec_error_phi0Si(0),
    b_trk_Rec_error_theta0Si(0),
    b_trk_Rec_error_qoverpSi(0),
    b_trk_Rec_chi2probSi(0),
    b_trk_nTracksPixel(0),
    b_trk_nPixelHitsPixel(0),
    b_trk_PixResPhiPixel(0),
    b_trk_PixResEtaPixel(0),
    b_trk_PixPullPhiPixel(0),
    b_trk_PixPullEtaPixel(0),
    b_trk_PixErrTrkPhiPixel(0),
    b_trk_PixErrTrkEtaPixel(0),
    b_trk_PixErrHitPhiPixel(0),
    b_trk_PixErrHitEtaPixel(0),
    b_trk_Pix_layerPixel(0),
    b_trk_Sct_etaPixel(0),
    b_trk_Pix_phiPixel(0),
    b_trk_Pix_clusWidthRowPixel(0),
    b_trk_Pix_clusWidthColPixel(0),
    b_trk_nSctHitsPixel(0),
    b_trk_SctResPixel(0),
    b_trk_SctPullPixel(0),
    b_trk_SctErrTrkPixel(0),
    b_trk_SctErrHitPixel(0),
    b_trk_Sct_layerPixel(0),
    b_trk_Sct_phiPixel(0),
    b_trk_Sct_sidePixel(0),
    b_trk_Sct_rdoIdPixel(0),
    b_trk_Sct_clusGroupsizePixel(0),
    b_trk_Sct_clusxPixel(0),
    b_trk_Sct_clusyPixel(0),
    b_trk_Sct_cluszPixel(0),
    b_trk_nTrtHitsPixel(0),
    b_trk_TrtResPixel(0),
    b_trk_TrtPullPixel(0),
    b_trk_TrtErrTrkPixel(0),
    b_trk_TrtErrHitPixel(0),
    b_trk_Trt_RPixel(0),
    b_trk_Trt_RtrackPixel(0),
    b_trk_Trt_tPixel(0),
    b_trk_Trt_xPixel(0),
    b_trk_Trt_yPixel(0),
    b_trk_Trt_HLPixel(0),
    b_trk_Trt_strawlayerPixel(0),
    b_trk_Trt_layerPixel(0),
    b_trk_Trt_strawPixel(0),
    b_trk_Trt_phisectorPixel(0),
    b_trk_Rec_d0Pixel(0),
    b_trk_Rec_z0Pixel(0),
    b_trk_Rec_phi0Pixel(0),
    b_trk_Rec_theta0Pixel(0),
    b_trk_Rec_qoverpPixel(0),
    b_trk_Rec_chi2Pixel(0),
    b_trk_Rec_ndofPixel(0),
    b_trk_Rec_etaPixel(0),
    b_trk_Rec_error_d0Pixel(0),
    b_trk_Rec_error_z0Pixel(0),
    b_trk_Rec_error_phi0Pixel(0),
    b_trk_Rec_error_theta0Pixel(0),
    b_trk_Rec_error_qoverpPixel(0),
    b_trk_Rec_chi2probPixel(0),
    b_trk_nTracksSCT(0),
    b_trk_nPixelHitsSCT(0),
    b_trk_PixResPhiSCT(0),
    b_trk_PixResEtaSCT(0),
    b_trk_PixPullPhiSCT(0),
    b_trk_PixPullEtaSCT(0),
    b_trk_PixErrTrkPhiSCT(0),
    b_trk_PixErrTrkEtaSCT(0),
    b_trk_PixErrHitPhiSCT(0),
    b_trk_PixErrHitEtaSCT(0),
    b_trk_Pix_layerSCT(0),
    b_trk_Sct_etaSCT(0),
    b_trk_Pix_phiSCT(0),
    b_trk_Pix_clusWidthRowSCT(0),
    b_trk_Pix_clusWidthColSCT(0),
    b_trk_nSctHitsSCT(0),
    b_trk_SctResSCT(0),
    b_trk_SctPullSCT(0),
    b_trk_SctErrTrkSCT(0),
    b_trk_SctErrHitSCT(0),
    b_trk_Sct_layerSCT(0),
    b_trk_Sct_phiSCT(0),
    b_trk_Sct_sideSCT(0),
    b_trk_Sct_rdoIdSCT(0),
    b_trk_Sct_clusGroupsizeSCT(0),
    b_trk_Sct_clusxSCT(0),
    b_trk_Sct_clusySCT(0),
    b_trk_Sct_cluszSCT(0),
    b_trk_nTrtHitsSCT(0),
    b_trk_TrtResSCT(0),
    b_trk_TrtPullSCT(0),
    b_trk_TrtErrTrkSCT(0),
    b_trk_TrtErrHitSCT(0),
    b_trk_Trt_RSCT(0),
    b_trk_Trt_RtrackSCT(0),
    b_trk_Trt_tSCT(0),
    b_trk_Trt_xSCT(0),
    b_trk_Trt_ySCT(0),
    b_trk_Trt_HLSCT(0),
    b_trk_Trt_strawlayerSCT(0),
    b_trk_Trt_layerSCT(0),
    b_trk_Trt_strawSCT(0),
    b_trk_Trt_phisectorSCT(0),
    b_trk_Rec_d0SCT(0),
    b_trk_Rec_z0SCT(0),
    b_trk_Rec_phi0SCT(0),
    b_trk_Rec_theta0SCT(0),
    b_trk_Rec_qoverpSCT(0),
    b_trk_Rec_chi2SCT(0),
    b_trk_Rec_ndofSCT(0),
    b_trk_Rec_etaSCT(0),
    b_trk_Rec_error_d0SCT(0),
    b_trk_Rec_error_z0SCT(0),
    b_trk_Rec_error_phi0SCT(0),
    b_trk_Rec_error_theta0SCT(0),
    b_trk_Rec_error_qoverpSCT(0),
    b_trk_Rec_chi2probSCT(0),
    b_trk_nTracksTRT(0),
    b_trk_nPixelHitsTRT(0),
    b_trk_PixResPhiTRT(0),
    b_trk_PixResEtaTRT(0),
    b_trk_PixPullPhiTRT(0),
    b_trk_PixPullEtaTRT(0),
    b_trk_PixErrTrkPhiTRT(0),
    b_trk_PixErrTrkEtaTRT(0),
    b_trk_PixErrHitPhiTRT(0),
    b_trk_PixErrHitEtaTRT(0),
    b_trk_Pix_layerTRT(0),
    b_trk_Sct_etaTRT(0),
    b_trk_Pix_phiTRT(0),
    b_trk_Pix_clusWidthRowTRT(0),
    b_trk_Pix_clusWidthColTRT(0),
    b_trk_nSctHitsTRT(0),
    b_trk_SctResTRT(0),
    b_trk_SctPullTRT(0),
    b_trk_SctErrTrkTRT(0),
    b_trk_SctErrHitTRT(0),
    b_trk_Sct_layerTRT(0),
    b_trk_Sct_phiTRT(0),
    b_trk_Sct_sideTRT(0),
    b_trk_Sct_rdoIdTRT(0),
    b_trk_Sct_clusGroupsizeTRT(0),
    b_trk_Sct_clusxTRT(0),
    b_trk_Sct_clusyTRT(0),
    b_trk_Sct_cluszTRT(0),
    b_trk_nTrtHitsTRT(0),
    b_trk_TrtResTRT(0),
    b_trk_TrtPullTRT(0),
    b_trk_TrtErrTrkTRT(0),
    b_trk_TrtErrHitTRT(0),
    b_trk_Trt_RTRT(0),
    b_trk_Trt_RtrackTRT(0),
    b_trk_Trt_tTRT(0),
    b_trk_Trt_xTRT(0),
    b_trk_Trt_yTRT(0),
    b_trk_Trt_HLTRT(0),
    b_trk_Trt_strawlayerTRT(0),
    b_trk_Trt_layerTRT(0),
    b_trk_Trt_strawTRT(0),
    b_trk_Trt_phisectorTRT(0),
    b_trk_Rec_d0TRT(0),
    b_trk_Rec_z0TRT(0),
    b_trk_Rec_phi0TRT(0),
    b_trk_Rec_theta0TRT(0),
    b_trk_Rec_qoverpTRT(0),
    b_trk_Rec_chi2TRT(0),
    b_trk_Rec_ndofTRT(0),
    b_trk_Rec_etaTRT(0),
    b_trk_Rec_error_d0TRT(0),
    b_trk_Rec_error_z0TRT(0),
    b_trk_Rec_error_phi0TRT(0),
    b_trk_Rec_error_theta0TRT(0),
    b_trk_Rec_error_qoverpTRT(0),
    b_trk_Rec_chi2probTRT(0),
    b_Trk_totalNumTracks(0),
    b_Trk_d0(0),
    b_Trk_z0(0),
    b_Trk_phi(0),
    b_Trk_theta(0),
    b_Trk_qOverP(0),
    b_Trk_Chi2(0),
    b_Trk_Ndf(0),
    b_Trk_sigd0(0),
    b_Trk_sigz0(0),
    b_Trk_sigphi(0),
    b_Trk_sigtheta(0),
    b_Trk_sigqOverP(0),
    b_Trk_covVert21(0),
    b_Trk_covVert31(0),
    b_Trk_covVert32(0),
    b_Trk_covVert41(0),
    b_Trk_covVert42(0),
    b_Trk_covVert43(0),
    b_Trk_covVert51(0),
    b_Trk_covVert52(0),
    b_Trk_covVert53(0),
    b_Trk_covVert54(0),
    b_Trk_px(0),
    b_Trk_py(0),
    b_Trk_pz(0),
    b_Trk_pt(0),
    b_Trk_numberOfBLayerHits(0),
    b_Trk_numberOfPixelHits(0),
    b_Trk_numberOfSCTHits(0),
    b_Trk_numberOfTRTHits(0),
    b_Trk_numberOfTRTHighThresholdHits(0),
    b_Trk_numberOfBLayerSharedHits(0),
    b_Trk_numberOfPixelSharedHits(0),
    b_Trk_numberOfPixelHoles(0),
    b_Trk_numberOfSCTSharedHits(0),
    b_Trk_numberOfSCTHoles(0),
    b_Trk_numberOfTRTOutliers(0),
    b_Trk_numberOfTRTHighThresholdOutliers(0),
    b_Trk_numberOfMdtHits(0),
    b_Trk_numberOfTgcPhiHits(0),
    b_Trk_numberOfRpcPhiHits(0),
    b_Trk_numberOfCscPhiHits(0),
    b_Trk_numberOfTgcEtaHits(0),
    b_Trk_numberOfRpcEtaHits(0),
    b_Trk_numberOfCscEtaHits(0),
    b_Trk_numberOfGangedPixels(0),
    b_Trk_numberOfOutliersOnTrack(0),
    b_Trk_numberOfTrackSummaryTypes(0),
    b_Trk_truthBarcode(0),
    b_Trk_truthNt(0),
    b_Trk_ParticlePdg(0),
    b_Trk_ParentPdg(0),
    b_IIC_InDet_NImpact(0),
    b_IIC_InDet_x0(0),
    b_IIC_InDet_y0(0),
    b_IIC_InDet_z0(0),
    b_IIC_InDet_eta0_local(0),
    b_IIC_InDet_phi0_local(0),
    b_IIC_InDet_trketa_at0(0),
    b_IIC_InDet_trkphi_at0(0),
    b_IIC_InDet_x1(0),
    b_IIC_InDet_y1(0),
    b_IIC_InDet_z1(0),
    b_IIC_InDet_eta1_local(0),
    b_IIC_InDet_phi1_local(0),
    b_IIC_InDet_trketa_at1(0),
    b_IIC_InDet_trkphi_at1(0),
    b_IIC_InDet_x2(0),
    b_IIC_InDet_y2(0),
    b_IIC_InDet_z2(0),
    b_IIC_InDet_eta2_local(0),
    b_IIC_InDet_phi2_local(0),
    b_IIC_InDet_trketa_at2(0),
    b_IIC_InDet_trkphi_at2(0),
    b_IIC_InDet_x3(0),
    b_IIC_InDet_y3(0),
    b_IIC_InDet_z3(0),
    b_IIC_InDet_eta3_local(0),
    b_IIC_InDet_phi3_local(0),
    b_IIC_InDet_trketa_at3(0),
    b_IIC_InDet_trkphi_at3(0),
    b_IIC_InDet_x_tile(0),
    b_IIC_InDet_y_tile(0),
    b_IIC_InDet_z_tile(0),
    b_IIC_InDet_eta_tile_local(0),
    b_IIC_InDet_phi_tile_local(0),
    b_IIC_InDet_trketa_attile(0),
    b_IIC_InDet_trkphi_attile(0)
    {
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("cbnt_RecExTB_Combined_1207_2102095.0.root");
      if (!f) {
         f = new TFile("cbnt_RecExTB_Combined_1207_2102095.0.root");
         f->cd("cbnt_RecExTB_Combined_1207_2102095.0.root:/TB");
      }
      tree = (TTree*)gDirectory->Get("tree");

   }
   Init(tree);
}

TB_tree::~TB_tree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TB_tree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TB_tree::LoadTree(Long64_t entry)
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

void TB_tree::Init(TTree *tree)
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

   fChain->SetBranchAddress("Run", &Run, &b_Run);
   fChain->SetBranchAddress("Event", &Event, &b_Event);
   fChain->SetBranchAddress("Time", &Time, &b_Time);
   fChain->SetBranchAddress("Weight", &Weight, &b_Weight);
   fChain->SetBranchAddress("IEvent", &IEvent, &b_IEvent);
   fChain->SetBranchAddress("RunEta", &RunEta, &b_RunEta);
   fChain->SetBranchAddress("RunPartType", &RunPartType, &b_RunPartType);
   fChain->SetBranchAddress("RunEnergy", &RunEnergy, &b_RunEnergy);
   fChain->SetBranchAddress("Energy", &Energy, &b_Energy);
   fChain->SetBranchAddress("ErrAbsEnergy", &ErrAbsEnergy, &b_ErrAbsEnergy);
   fChain->SetBranchAddress("ErrCollimators", &ErrCollimators, &b_ErrCollimators);
   fChain->SetBranchAddress("ErrCurrents", &ErrCurrents, &b_ErrCurrents);
   fChain->SetBranchAddress("SyncLoss", &SyncLoss, &b_SyncLoss);
   fChain->SetBranchAddress("ErrSyncLoss", &ErrSyncLoss, &b_ErrSyncLoss);
   fChain->SetBranchAddress("Quad_nc", &Quad_nc, &b_Quad_nc);
   fChain->SetBranchAddress("Quad_file", Quad_file, &b_Quad_file);
   fChain->SetBranchAddress("Quad_equip", Quad_equip, &b_Quad_equip);
   fChain->SetBranchAddress("Bend_nc", &Bend_nc, &b_Bend_nc);
   fChain->SetBranchAddress("Bend_file", Bend_file, &b_Bend_file);
   fChain->SetBranchAddress("Bend_equip", Bend_equip, &b_Bend_equip);
   fChain->SetBranchAddress("Trim_nc", &Trim_nc, &b_Trim_nc);
   fChain->SetBranchAddress("Trim_file", Trim_file, &b_Trim_file);
   fChain->SetBranchAddress("Trim_equip", Trim_equip, &b_Trim_equip);
   fChain->SetBranchAddress("Coll_nc", &Coll_nc, &b_Coll_nc);
   fChain->SetBranchAddress("Coll_file", Coll_file, &b_Coll_file);
   fChain->SetBranchAddress("Coll_equip", Coll_equip, &b_Coll_equip);
   fChain->SetBranchAddress("sADC_S0", &sADC_S0, &b_sADC_S0);
   fChain->SetBranchAddress("sTDC_S0", &sTDC_S0, &b_sTDC_S0);
   fChain->SetBranchAddress("sADC_S1", &sADC_S1, &b_sADC_S1);
   fChain->SetBranchAddress("sTDC_S1", &sTDC_S1, &b_sTDC_S1);
   fChain->SetBranchAddress("sADC_S2mUp", &sADC_S2mUp, &b_sADC_S2mUp);
   fChain->SetBranchAddress("sTDC_S2mUp", &sTDC_S2mUp, &b_sTDC_S2mUp);
   fChain->SetBranchAddress("sADC_S2mDown", &sADC_S2mDown, &b_sADC_S2mDown);
   fChain->SetBranchAddress("sTDC_S2mDown", &sTDC_S2mDown, &b_sTDC_S2mDown);
   fChain->SetBranchAddress("sADC_S3mLeft", &sADC_S3mLeft, &b_sADC_S3mLeft);
   fChain->SetBranchAddress("sTDC_S3mLeft", &sTDC_S3mLeft, &b_sTDC_S3mLeft);
   fChain->SetBranchAddress("sADC_S3mRight", &sADC_S3mRight, &b_sADC_S3mRight);
   fChain->SetBranchAddress("sTDC_S3mRight", &sTDC_S3mRight, &b_sTDC_S3mRight);
   fChain->SetBranchAddress("sADC_C1", &sADC_C1, &b_sADC_C1);
   fChain->SetBranchAddress("sTDC_C1", &sTDC_C1, &b_sTDC_C1);
   fChain->SetBranchAddress("sADC_C2", &sADC_C2, &b_sADC_C2);
   fChain->SetBranchAddress("sTDC_C2", &sTDC_C2, &b_sTDC_C2);
   fChain->SetBranchAddress("sADC_muTag", &sADC_muTag, &b_sADC_muTag);
   fChain->SetBranchAddress("sTDC_muTag", &sTDC_muTag, &b_sTDC_muTag);
   fChain->SetBranchAddress("sADC_muHalo", &sADC_muHalo, &b_sADC_muHalo);
   fChain->SetBranchAddress("sTDC_muHalo", &sTDC_muHalo, &b_sTDC_muHalo);
   fChain->SetBranchAddress("sADC_muVeto", &sADC_muVeto, &b_sADC_muVeto);
   fChain->SetBranchAddress("sTDC_muVeto", &sTDC_muVeto, &b_sTDC_muVeto);
   fChain->SetBranchAddress("sADC_SC1", &sADC_SC1, &b_sADC_SC1);
   fChain->SetBranchAddress("sTDC_SC1", &sTDC_SC1, &b_sTDC_SC1);
   fChain->SetBranchAddress("sADC_SC2", &sADC_SC2, &b_sADC_SC2);
   fChain->SetBranchAddress("sTDC_SC2", &sTDC_SC2, &b_sTDC_SC2);
   fChain->SetBranchAddress("sADC_TRTSci", &sADC_TRTSci, &b_sADC_TRTSci);
   fChain->SetBranchAddress("sTDC_TRTSci", &sTDC_TRTSci, &b_sTDC_TRTSci);
   fChain->SetBranchAddress("bpcX_BPCm2", &bpcX_BPCm2, &b_bpcX_BPCm2);
   fChain->SetBranchAddress("bpcY_BPCm2", &bpcY_BPCm2, &b_bpcY_BPCm2);
   fChain->SetBranchAddress("bpcX_BPCm1", &bpcX_BPCm1, &b_bpcX_BPCm1);
   fChain->SetBranchAddress("bpcY_BPCm1", &bpcY_BPCm1, &b_bpcY_BPCm1);
   fChain->SetBranchAddress("bpcX_BPC0", &bpcX_BPC0, &b_bpcX_BPC0);
   fChain->SetBranchAddress("bpcY_BPC0", &bpcY_BPC0, &b_bpcY_BPC0);
   fChain->SetBranchAddress("bpcX_BPC1", &bpcX_BPC1, &b_bpcX_BPC1);
   fChain->SetBranchAddress("bpcY_BPC1", &bpcY_BPC1, &b_bpcY_BPC1);
   fChain->SetBranchAddress("bpcX_BPC2", &bpcX_BPC2, &b_bpcX_BPC2);
   fChain->SetBranchAddress("bpcY_BPC2", &bpcY_BPC2, &b_bpcY_BPC2);
   fChain->SetBranchAddress("Trigger", &Trigger, &b_Trigger);
   fChain->SetBranchAddress("Clock", &Clock, &b_Clock);
   fChain->SetBranchAddress("Nh_Calo", &Nh_Calo, &b_Nh_Calo);
   fChain->SetBranchAddress("Eh_Calo", &Eh_Calo, &b_Eh_Calo);
   fChain->SetBranchAddress("Nh_EM", &Nh_EM, &b_Nh_EM);
   fChain->SetBranchAddress("Eh_EM", &Eh_EM, &b_Eh_EM);
   fChain->SetBranchAddress("Nh_HAD", &Nh_HAD, &b_Nh_HAD);
   fChain->SetBranchAddress("Eh_HAD", &Eh_HAD, &b_Eh_HAD);
   fChain->SetBranchAddress("Nh_PresB", &Nh_PresB, &b_Nh_PresB);
   fChain->SetBranchAddress("Eh_PresB", &Eh_PresB, &b_Eh_PresB);
   fChain->SetBranchAddress("Nh_EMB", &Nh_EMB, &b_Nh_EMB);
   fChain->SetBranchAddress("Eh_EMB", Eh_EMB, &b_Eh_EMB);
   fChain->SetBranchAddress("Nh_EMEC", &Nh_EMEC, &b_Nh_EMEC);
   fChain->SetBranchAddress("Eh_EMEC", Eh_EMEC, &b_Eh_EMEC);
   fChain->SetBranchAddress("Nh_Tile", &Nh_Tile, &b_Nh_Tile);
   fChain->SetBranchAddress("Eh_Tile", Eh_Tile, &b_Eh_Tile);
   fChain->SetBranchAddress("Nh_TileGap", &Nh_TileGap, &b_Nh_TileGap);
   fChain->SetBranchAddress("Eh_TileGap", &Eh_TileGap, &b_Eh_TileGap);
   fChain->SetBranchAddress("Nh_HEC", &Nh_HEC, &b_Nh_HEC);
   fChain->SetBranchAddress("Eh_HEC", Eh_HEC, &b_Eh_HEC);
   fChain->SetBranchAddress("Nh_FCal", &Nh_FCal, &b_Nh_FCal);
   fChain->SetBranchAddress("Eh_FCal", Eh_FCal, &b_Eh_FCal);
   fChain->SetBranchAddress("Nh_PresE", &Nh_PresE, &b_Nh_PresE);
   fChain->SetBranchAddress("Eh_PresE", &Eh_PresE, &b_Eh_PresE);
   fChain->SetBranchAddress("Nh_Scint", &Nh_Scint, &b_Nh_Scint);
   fChain->SetBranchAddress("Eh_Scint", &Eh_Scint, &b_Eh_Scint);
   fChain->SetBranchAddress("nhit_lar", &nhit_lar, &b_nhit_lar);
   fChain->SetBranchAddress("ecell_lar", &ecell_lar, &b_ecell_lar);
   fChain->SetBranchAddress("NCells_lar", &NCells_lar, &b_NCells_lar);
   fChain->SetBranchAddress("ECells_lar", ECells_lar, &b_ECells_lar);
   fChain->SetBranchAddress("EtaCells_lar", EtaCells_lar, &b_EtaCells_lar);
   fChain->SetBranchAddress("PhiCells_lar", PhiCells_lar, &b_PhiCells_lar);
   fChain->SetBranchAddress("QCells_lar", QCells_lar, &b_QCells_lar);
   fChain->SetBranchAddress("GainCells_lar", GainCells_lar, &b_GainCells_lar);
   fChain->SetBranchAddress("DetCells_lar", DetCells_lar, &b_DetCells_lar);
   fChain->SetBranchAddress("TimeCells_lar", TimeCells_lar, &b_TimeCells_lar);
   fChain->SetBranchAddress("BadCells_lar", BadCells_lar, &b_BadCells_lar);
   fChain->SetBranchAddress("nhit_tile", &nhit_tile, &b_nhit_tile);
   fChain->SetBranchAddress("ecell_tile", &ecell_tile, &b_ecell_tile);
   fChain->SetBranchAddress("NCells_tile", &NCells_tile, &b_NCells_tile);
   fChain->SetBranchAddress("ECells_tile", ECells_tile, &b_ECells_tile);
   fChain->SetBranchAddress("EtaCells_tile", EtaCells_tile, &b_EtaCells_tile);
   fChain->SetBranchAddress("PhiCells_tile", PhiCells_tile, &b_PhiCells_tile);
   fChain->SetBranchAddress("QCells_tile", QCells_tile, &b_QCells_tile);
   fChain->SetBranchAddress("GainCells_tile", GainCells_tile, &b_GainCells_tile);
   fChain->SetBranchAddress("DetCells_tile", DetCells_tile, &b_DetCells_tile);
   fChain->SetBranchAddress("TimeCells_tile", TimeCells_tile, &b_TimeCells_tile);
   fChain->SetBranchAddress("cl_ecluster_em", &cl_ecluster_em, &b_cl_ecluster_em);
   fChain->SetBranchAddress("cl_nctotal_em", &cl_nctotal_em, &b_cl_nctotal_em);
   fChain->SetBranchAddress("cl_nc_em", &cl_nc_em, &b_cl_nc_em);
   fChain->SetBranchAddress("cl_et_em", cl_et_em, &b_cl_et_em);
   fChain->SetBranchAddress("cl_e_em", cl_e_em, &b_cl_e_em);
   fChain->SetBranchAddress("cl_eemb0_em", cl_eemb0_em, &b_cl_eemb0_em);
   fChain->SetBranchAddress("cl_eemb1_em", cl_eemb1_em, &b_cl_eemb1_em);
   fChain->SetBranchAddress("cl_eemb2_em", cl_eemb2_em, &b_cl_eemb2_em);
   fChain->SetBranchAddress("cl_eemb3_em", cl_eemb3_em, &b_cl_eemb3_em);
   fChain->SetBranchAddress("cl_phi2_em", cl_phi2_em, &b_cl_phi2_em);
   fChain->SetBranchAddress("cl_eta0_em", cl_eta0_em, &b_cl_eta0_em);
   fChain->SetBranchAddress("cl_eta1_em", cl_eta1_em, &b_cl_eta1_em);
   fChain->SetBranchAddress("cl_eta2_em", cl_eta2_em, &b_cl_eta2_em);
   fChain->SetBranchAddress("cl_eta3_em", cl_eta3_em, &b_cl_eta3_em);
   fChain->SetBranchAddress("cl_etileg1_em", cl_etileg1_em, &b_cl_etileg1_em);
   fChain->SetBranchAddress("cl_etileg2_em", cl_etileg2_em, &b_cl_etileg2_em);
   fChain->SetBranchAddress("cl_etileg3_em", cl_etileg3_em, &b_cl_etileg3_em);
   fChain->SetBranchAddress("cl_eta_em", cl_eta_em, &b_cl_eta_em);
   fChain->SetBranchAddress("cl_phi_em", cl_phi_em, &b_cl_phi_em);
   fChain->SetBranchAddress("cl_reco_stat_em", cl_reco_stat_em, &b_cl_reco_stat_em);
   fChain->SetBranchAddress("cl_m1_eta_em", cl_m1_eta_em, &b_cl_m1_eta_em);
   fChain->SetBranchAddress("cl_m1_phi_em", cl_m1_phi_em, &b_cl_m1_phi_em);
   fChain->SetBranchAddress("cl_m2_r_em", cl_m2_r_em, &b_cl_m2_r_em);
   fChain->SetBranchAddress("cl_m2_lambda_em", cl_m2_lambda_em, &b_cl_m2_lambda_em);
   fChain->SetBranchAddress("cl_delta_phi_em", cl_delta_phi_em, &b_cl_delta_phi_em);
   fChain->SetBranchAddress("cl_delta_theta_em", cl_delta_theta_em, &b_cl_delta_theta_em);
   fChain->SetBranchAddress("cl_delta_alpha_em", cl_delta_alpha_em, &b_cl_delta_alpha_em);
   fChain->SetBranchAddress("cl_center_x_em", cl_center_x_em, &b_cl_center_x_em);
   fChain->SetBranchAddress("cl_center_y_em", cl_center_y_em, &b_cl_center_y_em);
   fChain->SetBranchAddress("cl_center_z_em", cl_center_z_em, &b_cl_center_z_em);
   fChain->SetBranchAddress("cl_center_lambda_em", cl_center_lambda_em, &b_cl_center_lambda_em);
   fChain->SetBranchAddress("cl_lateral_em", cl_lateral_em, &b_cl_lateral_em);
   fChain->SetBranchAddress("cl_longitudinal_em", cl_longitudinal_em, &b_cl_longitudinal_em);
   fChain->SetBranchAddress("cl_eng_frac_em_em", cl_eng_frac_em_em, &b_cl_eng_frac_em_em);
   fChain->SetBranchAddress("cl_eng_frac_max_em", cl_eng_frac_max_em, &b_cl_eng_frac_max_em);
   fChain->SetBranchAddress("cl_eng_frac_core_em", cl_eng_frac_core_em, &b_cl_eng_frac_core_em);
   fChain->SetBranchAddress("cl_m1_dens_em", cl_m1_dens_em, &b_cl_m1_dens_em);
   fChain->SetBranchAddress("cl_m2_dens_em", cl_m2_dens_em, &b_cl_m2_dens_em);
   fChain->SetBranchAddress("cl_ntotcells_em", cl_ntotcells_em, &b_cl_ntotcells_em);
   fChain->SetBranchAddress("cl_cells_em", cl_cells_em, &b_cl_cells_em);
   fChain->SetBranchAddress("cl_ecluster_tb_em", &cl_ecluster_tb_em, &b_cl_ecluster_tb_em);
   fChain->SetBranchAddress("cl_nctotal_tb_em", &cl_nctotal_tb_em, &b_cl_nctotal_tb_em);
   fChain->SetBranchAddress("cl_nc_tb_em", &cl_nc_tb_em, &b_cl_nc_tb_em);
   fChain->SetBranchAddress("cl_et_tb_em", cl_et_tb_em, &b_cl_et_tb_em);
   fChain->SetBranchAddress("cl_e_tb_em", cl_e_tb_em, &b_cl_e_tb_em);
   fChain->SetBranchAddress("cl_eemb0_tb_em", cl_eemb0_tb_em, &b_cl_eemb0_tb_em);
   fChain->SetBranchAddress("cl_eemb1_tb_em", cl_eemb1_tb_em, &b_cl_eemb1_tb_em);
   fChain->SetBranchAddress("cl_eemb2_tb_em", cl_eemb2_tb_em, &b_cl_eemb2_tb_em);
   fChain->SetBranchAddress("cl_eemb3_tb_em", cl_eemb3_tb_em, &b_cl_eemb3_tb_em);
   fChain->SetBranchAddress("cl_phi2_tb_em", cl_phi2_tb_em, &b_cl_phi2_tb_em);
   fChain->SetBranchAddress("cl_eta0_tb_em", cl_eta0_tb_em, &b_cl_eta0_tb_em);
   fChain->SetBranchAddress("cl_eta1_tb_em", cl_eta1_tb_em, &b_cl_eta1_tb_em);
   fChain->SetBranchAddress("cl_eta2_tb_em", cl_eta2_tb_em, &b_cl_eta2_tb_em);
   fChain->SetBranchAddress("cl_eta3_tb_em", cl_eta3_tb_em, &b_cl_eta3_tb_em);
   fChain->SetBranchAddress("cl_etileg1_tb_em", cl_etileg1_tb_em, &b_cl_etileg1_tb_em);
   fChain->SetBranchAddress("cl_etileg2_tb_em", cl_etileg2_tb_em, &b_cl_etileg2_tb_em);
   fChain->SetBranchAddress("cl_etileg3_tb_em", cl_etileg3_tb_em, &b_cl_etileg3_tb_em);
   fChain->SetBranchAddress("cl_eta_tb_em", cl_eta_tb_em, &b_cl_eta_tb_em);
   fChain->SetBranchAddress("cl_phi_tb_em", cl_phi_tb_em, &b_cl_phi_tb_em);
   fChain->SetBranchAddress("cl_reco_stat_tb_em", cl_reco_stat_tb_em, &b_cl_reco_stat_tb_em);
   fChain->SetBranchAddress("cl_m1_eta_tb_em", cl_m1_eta_tb_em, &b_cl_m1_eta_tb_em);
   fChain->SetBranchAddress("cl_m1_phi_tb_em", cl_m1_phi_tb_em, &b_cl_m1_phi_tb_em);
   fChain->SetBranchAddress("cl_m2_r_tb_em", cl_m2_r_tb_em, &b_cl_m2_r_tb_em);
   fChain->SetBranchAddress("cl_m2_lambda_tb_em", cl_m2_lambda_tb_em, &b_cl_m2_lambda_tb_em);
   fChain->SetBranchAddress("cl_delta_phi_tb_em", cl_delta_phi_tb_em, &b_cl_delta_phi_tb_em);
   fChain->SetBranchAddress("cl_delta_theta_tb_em", cl_delta_theta_tb_em, &b_cl_delta_theta_tb_em);
   fChain->SetBranchAddress("cl_delta_alpha_tb_em", cl_delta_alpha_tb_em, &b_cl_delta_alpha_tb_em);
   fChain->SetBranchAddress("cl_center_x_tb_em", cl_center_x_tb_em, &b_cl_center_x_tb_em);
   fChain->SetBranchAddress("cl_center_y_tb_em", cl_center_y_tb_em, &b_cl_center_y_tb_em);
   fChain->SetBranchAddress("cl_center_z_tb_em", cl_center_z_tb_em, &b_cl_center_z_tb_em);
   fChain->SetBranchAddress("cl_center_lambda_tb_em", cl_center_lambda_tb_em, &b_cl_center_lambda_tb_em);
   fChain->SetBranchAddress("cl_lateral_tb_em", cl_lateral_tb_em, &b_cl_lateral_tb_em);
   fChain->SetBranchAddress("cl_longitudinal_tb_em", cl_longitudinal_tb_em, &b_cl_longitudinal_tb_em);
   fChain->SetBranchAddress("cl_eng_frac_em_tb_em", cl_eng_frac_em_tb_em, &b_cl_eng_frac_em_tb_em);
   fChain->SetBranchAddress("cl_eng_frac_max_tb_em", cl_eng_frac_max_tb_em, &b_cl_eng_frac_max_tb_em);
   fChain->SetBranchAddress("cl_eng_frac_core_tb_em", cl_eng_frac_core_tb_em, &b_cl_eng_frac_core_tb_em);
   fChain->SetBranchAddress("cl_m1_dens_tb_em", cl_m1_dens_tb_em, &b_cl_m1_dens_tb_em);
   fChain->SetBranchAddress("cl_m2_dens_tb_em", cl_m2_dens_tb_em, &b_cl_m2_dens_tb_em);
   fChain->SetBranchAddress("cl_ntotcells_tb_em", cl_ntotcells_tb_em, &b_cl_ntotcells_tb_em);
   fChain->SetBranchAddress("cl_cells_tb_em", cl_cells_tb_em, &b_cl_cells_tb_em);
   fChain->SetBranchAddress("cl_ecluster_topo", &cl_ecluster_topo, &b_cl_ecluster_topo);
   fChain->SetBranchAddress("cl_nctotal_topo", &cl_nctotal_topo, &b_cl_nctotal_topo);
   fChain->SetBranchAddress("cl_nc_topo", &cl_nc_topo, &b_cl_nc_topo);
   fChain->SetBranchAddress("cl_et_topo", cl_et_topo, &b_cl_et_topo);
   fChain->SetBranchAddress("cl_e_topo", cl_e_topo, &b_cl_e_topo);
   fChain->SetBranchAddress("cl_eemb0_topo", cl_eemb0_topo, &b_cl_eemb0_topo);
   fChain->SetBranchAddress("cl_eemb1_topo", cl_eemb1_topo, &b_cl_eemb1_topo);
   fChain->SetBranchAddress("cl_eemb2_topo", cl_eemb2_topo, &b_cl_eemb2_topo);
   fChain->SetBranchAddress("cl_eemb3_topo", cl_eemb3_topo, &b_cl_eemb3_topo);
   fChain->SetBranchAddress("cl_nemb0_topo", cl_nemb0_topo, &b_cl_nemb0_topo);
   fChain->SetBranchAddress("cl_nemb1_topo", cl_nemb1_topo, &b_cl_nemb1_topo);
   fChain->SetBranchAddress("cl_nemb2_topo", cl_nemb2_topo, &b_cl_nemb2_topo);
   fChain->SetBranchAddress("cl_nemb3_topo", cl_nemb3_topo, &b_cl_nemb3_topo);
   fChain->SetBranchAddress("cl_phi2_topo", cl_phi2_topo, &b_cl_phi2_topo);
   fChain->SetBranchAddress("cl_eta0_topo", cl_eta0_topo, &b_cl_eta0_topo);
   fChain->SetBranchAddress("cl_eta1_topo", cl_eta1_topo, &b_cl_eta1_topo);
   fChain->SetBranchAddress("cl_eta2_topo", cl_eta2_topo, &b_cl_eta2_topo);
   fChain->SetBranchAddress("cl_eta3_topo", cl_eta3_topo, &b_cl_eta3_topo);
   fChain->SetBranchAddress("cl_etileg1_topo", cl_etileg1_topo, &b_cl_etileg1_topo);
   fChain->SetBranchAddress("cl_etileg2_topo", cl_etileg2_topo, &b_cl_etileg2_topo);
   fChain->SetBranchAddress("cl_etileg3_topo", cl_etileg3_topo, &b_cl_etileg3_topo);
   fChain->SetBranchAddress("cl_ntileg1_topo", cl_ntileg1_topo, &b_cl_ntileg1_topo);
   fChain->SetBranchAddress("cl_ntileg2_topo", cl_ntileg2_topo, &b_cl_ntileg2_topo);
   fChain->SetBranchAddress("cl_ntileg3_topo", cl_ntileg3_topo, &b_cl_ntileg3_topo);
   fChain->SetBranchAddress("cl_eta_topo", cl_eta_topo, &b_cl_eta_topo);
   fChain->SetBranchAddress("cl_phi_topo", cl_phi_topo, &b_cl_phi_topo);
   fChain->SetBranchAddress("cl_reco_stat_topo", cl_reco_stat_topo, &b_cl_reco_stat_topo);
   fChain->SetBranchAddress("cl_m1_eta_topo", cl_m1_eta_topo, &b_cl_m1_eta_topo);
   fChain->SetBranchAddress("cl_m1_phi_topo", cl_m1_phi_topo, &b_cl_m1_phi_topo);
   fChain->SetBranchAddress("cl_m2_r_topo", cl_m2_r_topo, &b_cl_m2_r_topo);
   fChain->SetBranchAddress("cl_m2_lambda_topo", cl_m2_lambda_topo, &b_cl_m2_lambda_topo);
   fChain->SetBranchAddress("cl_delta_phi_topo", cl_delta_phi_topo, &b_cl_delta_phi_topo);
   fChain->SetBranchAddress("cl_delta_theta_topo", cl_delta_theta_topo, &b_cl_delta_theta_topo);
   fChain->SetBranchAddress("cl_delta_alpha_topo", cl_delta_alpha_topo, &b_cl_delta_alpha_topo);
   fChain->SetBranchAddress("cl_center_x_topo", cl_center_x_topo, &b_cl_center_x_topo);
   fChain->SetBranchAddress("cl_center_y_topo", cl_center_y_topo, &b_cl_center_y_topo);
   fChain->SetBranchAddress("cl_center_z_topo", cl_center_z_topo, &b_cl_center_z_topo);
   fChain->SetBranchAddress("cl_center_lambda_topo", cl_center_lambda_topo, &b_cl_center_lambda_topo);
   fChain->SetBranchAddress("cl_lateral_topo", cl_lateral_topo, &b_cl_lateral_topo);
   fChain->SetBranchAddress("cl_longitudinal_topo", cl_longitudinal_topo, &b_cl_longitudinal_topo);
   fChain->SetBranchAddress("cl_eng_frac_em_topo", cl_eng_frac_em_topo, &b_cl_eng_frac_em_topo);
   fChain->SetBranchAddress("cl_eng_frac_max_topo", cl_eng_frac_max_topo, &b_cl_eng_frac_max_topo);
   fChain->SetBranchAddress("cl_eng_frac_core_topo", cl_eng_frac_core_topo, &b_cl_eng_frac_core_topo);
   fChain->SetBranchAddress("cl_m1_dens_topo", cl_m1_dens_topo, &b_cl_m1_dens_topo);
   fChain->SetBranchAddress("cl_m2_dens_topo", cl_m2_dens_topo, &b_cl_m2_dens_topo);
   fChain->SetBranchAddress("cl_etileb0_topo", cl_etileb0_topo, &b_cl_etileb0_topo);
   fChain->SetBranchAddress("cl_etileb1_topo", cl_etileb1_topo, &b_cl_etileb1_topo);
   fChain->SetBranchAddress("cl_etileb2_topo", cl_etileb2_topo, &b_cl_etileb2_topo);
   fChain->SetBranchAddress("cl_ntileb0_topo", cl_ntileb0_topo, &b_cl_ntileb0_topo);
   fChain->SetBranchAddress("cl_ntileb1_topo", cl_ntileb1_topo, &b_cl_ntileb1_topo);
   fChain->SetBranchAddress("cl_ntileb2_topo", cl_ntileb2_topo, &b_cl_ntileb2_topo);
   fChain->SetBranchAddress("cl_ntotcells_topo", cl_ntotcells_topo, &b_cl_ntotcells_topo);
   fChain->SetBranchAddress("cl_cells_topo", cl_cells_topo, &b_cl_cells_topo);
   fChain->SetBranchAddress("ncellstotal_topo", &ncellstotal_topo, &b_ncellstotal_topo);
   fChain->SetBranchAddress("ncells_topo", &ncells_topo, &b_ncells_topo);
   fChain->SetBranchAddress("celleng_topo", celleng_topo, &b_celleng_topo);
   fChain->SetBranchAddress("cellet_topo", cellet_topo, &b_cellet_topo);
   fChain->SetBranchAddress("celleta_topo", celleta_topo, &b_celleta_topo);
   fChain->SetBranchAddress("cellphi_topo", cellphi_topo, &b_cellphi_topo);
   fChain->SetBranchAddress("cellr_topo", cellr_topo, &b_cellr_topo);
   fChain->SetBranchAddress("cellz_topo", cellz_topo, &b_cellz_topo);
   fChain->SetBranchAddress("celldeta_topo", celldeta_topo, &b_celldeta_topo);
   fChain->SetBranchAddress("celldphi_topo", celldphi_topo, &b_celldphi_topo);
   fChain->SetBranchAddress("celldr_topo", celldr_topo, &b_celldr_topo);
   fChain->SetBranchAddress("celldz_topo", celldz_topo, &b_celldz_topo);
   fChain->SetBranchAddress("cellvol_topo", cellvol_topo, &b_cellvol_topo);
   fChain->SetBranchAddress("celltime_topo", celltime_topo, &b_celltime_topo);
   fChain->SetBranchAddress("cellquality_topo", cellquality_topo, &b_cellquality_topo);
   fChain->SetBranchAddress("cellweight_topo", cellweight_topo, &b_cellweight_topo);
   fChain->SetBranchAddress("cellgain_topo", cellgain_topo, &b_cellgain_topo);
   fChain->SetBranchAddress("cellnoise_topo", cellnoise_topo, &b_cellnoise_topo);
   fChain->SetBranchAddress("celltotnoise_topo", celltotnoise_topo, &b_celltotnoise_topo);
   fChain->SetBranchAddress("cellcluster_topo", cellcluster_topo, &b_cellcluster_topo);
   fChain->SetBranchAddress("cell_calo_topo", cell_calo_topo, &b_cell_calo_topo);
   fChain->SetBranchAddress("cell_side_topo", cell_side_topo, &b_cell_side_topo);
   fChain->SetBranchAddress("cell_samp_topo", cell_samp_topo, &b_cell_samp_topo);
   fChain->SetBranchAddress("cell_ireg_topo", cell_ireg_topo, &b_cell_ireg_topo);
   fChain->SetBranchAddress("cell_ieta_topo", cell_ieta_topo, &b_cell_ieta_topo);
   fChain->SetBranchAddress("cell_iphi_topo", cell_iphi_topo, &b_cell_iphi_topo);
   fChain->SetBranchAddress("cl_ecluster_topoEM", &cl_ecluster_topoEM, &b_cl_ecluster_topoEM);
   fChain->SetBranchAddress("cl_nctotal_topoEM", &cl_nctotal_topoEM, &b_cl_nctotal_topoEM);
   fChain->SetBranchAddress("cl_nc_topoEM", &cl_nc_topoEM, &b_cl_nc_topoEM);
   fChain->SetBranchAddress("cl_et_topoEM", cl_et_topoEM, &b_cl_et_topoEM);
   fChain->SetBranchAddress("cl_e_topoEM", cl_e_topoEM, &b_cl_e_topoEM);
   fChain->SetBranchAddress("cl_eemb0_topoEM", cl_eemb0_topoEM, &b_cl_eemb0_topoEM);
   fChain->SetBranchAddress("cl_eemb1_topoEM", cl_eemb1_topoEM, &b_cl_eemb1_topoEM);
   fChain->SetBranchAddress("cl_eemb2_topoEM", cl_eemb2_topoEM, &b_cl_eemb2_topoEM);
   fChain->SetBranchAddress("cl_eemb3_topoEM", cl_eemb3_topoEM, &b_cl_eemb3_topoEM);
   fChain->SetBranchAddress("cl_nemb0_topoEM", cl_nemb0_topoEM, &b_cl_nemb0_topoEM);
   fChain->SetBranchAddress("cl_nemb1_topoEM", cl_nemb1_topoEM, &b_cl_nemb1_topoEM);
   fChain->SetBranchAddress("cl_nemb2_topoEM", cl_nemb2_topoEM, &b_cl_nemb2_topoEM);
   fChain->SetBranchAddress("cl_nemb3_topoEM", cl_nemb3_topoEM, &b_cl_nemb3_topoEM);
   fChain->SetBranchAddress("cl_phi2_topoEM", cl_phi2_topoEM, &b_cl_phi2_topoEM);
   fChain->SetBranchAddress("cl_eta0_topoEM", cl_eta0_topoEM, &b_cl_eta0_topoEM);
   fChain->SetBranchAddress("cl_eta1_topoEM", cl_eta1_topoEM, &b_cl_eta1_topoEM);
   fChain->SetBranchAddress("cl_eta2_topoEM", cl_eta2_topoEM, &b_cl_eta2_topoEM);
   fChain->SetBranchAddress("cl_eta3_topoEM", cl_eta3_topoEM, &b_cl_eta3_topoEM);
   fChain->SetBranchAddress("cl_etileg1_topoEM", cl_etileg1_topoEM, &b_cl_etileg1_topoEM);
   fChain->SetBranchAddress("cl_etileg2_topoEM", cl_etileg2_topoEM, &b_cl_etileg2_topoEM);
   fChain->SetBranchAddress("cl_etileg3_topoEM", cl_etileg3_topoEM, &b_cl_etileg3_topoEM);
   fChain->SetBranchAddress("cl_ntileg1_topoEM", cl_ntileg1_topoEM, &b_cl_ntileg1_topoEM);
   fChain->SetBranchAddress("cl_ntileg2_topoEM", cl_ntileg2_topoEM, &b_cl_ntileg2_topoEM);
   fChain->SetBranchAddress("cl_ntileg3_topoEM", cl_ntileg3_topoEM, &b_cl_ntileg3_topoEM);
   fChain->SetBranchAddress("cl_eta_topoEM", cl_eta_topoEM, &b_cl_eta_topoEM);
   fChain->SetBranchAddress("cl_phi_topoEM", cl_phi_topoEM, &b_cl_phi_topoEM);
   fChain->SetBranchAddress("cl_reco_stat_topoEM", cl_reco_stat_topoEM, &b_cl_reco_stat_topoEM);
   fChain->SetBranchAddress("cl_m1_eta_topoEM", cl_m1_eta_topoEM, &b_cl_m1_eta_topoEM);
   fChain->SetBranchAddress("cl_m1_phi_topoEM", cl_m1_phi_topoEM, &b_cl_m1_phi_topoEM);
   fChain->SetBranchAddress("cl_m2_r_topoEM", cl_m2_r_topoEM, &b_cl_m2_r_topoEM);
   fChain->SetBranchAddress("cl_m2_lambda_topoEM", cl_m2_lambda_topoEM, &b_cl_m2_lambda_topoEM);
   fChain->SetBranchAddress("cl_delta_phi_topoEM", cl_delta_phi_topoEM, &b_cl_delta_phi_topoEM);
   fChain->SetBranchAddress("cl_delta_theta_topoEM", cl_delta_theta_topoEM, &b_cl_delta_theta_topoEM);
   fChain->SetBranchAddress("cl_delta_alpha_topoEM", cl_delta_alpha_topoEM, &b_cl_delta_alpha_topoEM);
   fChain->SetBranchAddress("cl_center_x_topoEM", cl_center_x_topoEM, &b_cl_center_x_topoEM);
   fChain->SetBranchAddress("cl_center_y_topoEM", cl_center_y_topoEM, &b_cl_center_y_topoEM);
   fChain->SetBranchAddress("cl_center_z_topoEM", cl_center_z_topoEM, &b_cl_center_z_topoEM);
   fChain->SetBranchAddress("cl_center_lambda_topoEM", cl_center_lambda_topoEM, &b_cl_center_lambda_topoEM);
   fChain->SetBranchAddress("cl_lateral_topoEM", cl_lateral_topoEM, &b_cl_lateral_topoEM);
   fChain->SetBranchAddress("cl_longitudinal_topoEM", cl_longitudinal_topoEM, &b_cl_longitudinal_topoEM);
   fChain->SetBranchAddress("cl_eng_frac_em_topoEM", cl_eng_frac_em_topoEM, &b_cl_eng_frac_em_topoEM);
   fChain->SetBranchAddress("cl_eng_frac_max_topoEM", cl_eng_frac_max_topoEM, &b_cl_eng_frac_max_topoEM);
   fChain->SetBranchAddress("cl_eng_frac_core_topoEM", cl_eng_frac_core_topoEM, &b_cl_eng_frac_core_topoEM);
   fChain->SetBranchAddress("cl_m1_dens_topoEM", cl_m1_dens_topoEM, &b_cl_m1_dens_topoEM);
   fChain->SetBranchAddress("cl_m2_dens_topoEM", cl_m2_dens_topoEM, &b_cl_m2_dens_topoEM);
   fChain->SetBranchAddress("cl_ntotcells_topoEM", cl_ntotcells_topoEM, &b_cl_ntotcells_topoEM);
   fChain->SetBranchAddress("cl_cells_topoEM", cl_cells_topoEM, &b_cl_cells_topoEM);
   fChain->SetBranchAddress("NCells", &NCells, &b_NCells);
   fChain->SetBranchAddress("NFebs", &NFebs, &b_NFebs);
   fChain->SetBranchAddress("NLayers", &NLayers, &b_NLayers);
   fChain->SetBranchAddress("TimeCell", TimeCell, &b_TimeCell);
   fChain->SetBranchAddress("EnergyCell", EnergyCell, &b_EnergyCell);
   fChain->SetBranchAddress("LayerCell", LayerCell, &b_LayerCell);
   fChain->SetBranchAddress("EtaCell", EtaCell, &b_EtaCell);
   fChain->SetBranchAddress("PhiCell", PhiCell, &b_PhiCell);
   fChain->SetBranchAddress("FebIDCell", FebIDCell, &b_FebIDCell);
   fChain->SetBranchAddress("TimeLayer", TimeLayer, &b_TimeLayer);
   fChain->SetBranchAddress("LayerIDLayer", LayerIDLayer, &b_LayerIDLayer);
   fChain->SetBranchAddress("TimeFeb", TimeFeb, &b_TimeFeb);
   fChain->SetBranchAddress("FebIdFeb", FebIdFeb, &b_FebIdFeb);
   fChain->SetBranchAddress("SlotFeb", SlotFeb, &b_SlotFeb);
   fChain->SetBranchAddress("FeedthroughFeb", FeedthroughFeb, &b_FeedthroughFeb);
   fChain->SetBranchAddress("Phase", &Phase, &b_Phase);
   fChain->SetBranchAddress("TimeTotal", &TimeTotal, &b_TimeTotal);
   fChain->SetBranchAddress("EnergyTotal", &EnergyTotal, &b_EnergyTotal);
   fChain->SetBranchAddress("phasetime", &phasetime, &b_phasetime);
   fChain->SetBranchAddress("pixel_BCID2", &pixel_BCID2, &b_pixel_BCID2);
   fChain->SetBranchAddress("sct_BCID", &sct_BCID, &b_sct_BCID);
   fChain->SetBranchAddress("trt_BCID", &trt_BCID, &b_trt_BCID);
   fChain->SetBranchAddress("pixel_nhits", &pixel_nhits, &b_pixel_nhits);
   fChain->SetBranchAddress("pixel_layer", pixel_layer, &b_pixel_layer);
   fChain->SetBranchAddress("pixel_phi", pixel_phi, &b_pixel_phi);
   fChain->SetBranchAddress("pixel_row", pixel_row, &b_pixel_row);
   fChain->SetBranchAddress("pixel_col", pixel_col, &b_pixel_col);
   fChain->SetBranchAddress("pixel_tot", pixel_tot, &b_pixel_tot);
   fChain->SetBranchAddress("pixel_BCID", pixel_BCID, &b_pixel_BCID);
   fChain->SetBranchAddress("sct_rdo_nhits", &sct_rdo_nhits, &b_sct_rdo_nhits);
   fChain->SetBranchAddress("sct_rdo_strips", sct_rdo_strips, &b_sct_rdo_strips);
   fChain->SetBranchAddress("sct_rdo_tbin", sct_rdo_tbin, &b_sct_rdo_tbin);
   fChain->SetBranchAddress("sct_rdo_rdoId", sct_rdo_rdoId, &b_sct_rdo_rdoId);
   fChain->SetBranchAddress("sct_rdo_layer", sct_rdo_layer, &b_sct_rdo_layer);
   fChain->SetBranchAddress("sct_rdo_eta", sct_rdo_eta, &b_sct_rdo_eta);
   fChain->SetBranchAddress("sct_rdo_phi", sct_rdo_phi, &b_sct_rdo_phi);
   fChain->SetBranchAddress("sct_rdo_side", sct_rdo_side, &b_sct_rdo_side);
   fChain->SetBranchAddress("sct_rdo_errorhit", sct_rdo_errorhit, &b_sct_rdo_errorhit);
   fChain->SetBranchAddress("sct_rdo_SyncronizationError", sct_rdo_SyncronizationError, &b_sct_rdo_SyncronizationError);
   fChain->SetBranchAddress("sct_rdo_PreambleError", sct_rdo_PreambleError, &b_sct_rdo_PreambleError);
   fChain->SetBranchAddress("sct_rdo_LVL1Error", sct_rdo_LVL1Error, &b_sct_rdo_LVL1Error);
   fChain->SetBranchAddress("sct_rdo_BCIDError", sct_rdo_BCIDError, &b_sct_rdo_BCIDError);
   fChain->SetBranchAddress("sct_rdo_FormatterError", sct_rdo_FormatterError, &b_sct_rdo_FormatterError);
   fChain->SetBranchAddress("sct_rdo_FirstSecondHitError", sct_rdo_FirstSecondHitError, &b_sct_rdo_FirstSecondHitError);
   fChain->SetBranchAddress("trt_rdo_nrdos", &trt_rdo_nrdos, &b_trt_rdo_nrdos);
   fChain->SetBranchAddress("trt_rdo_drifttime", trt_rdo_drifttime, &b_trt_rdo_drifttime);
   fChain->SetBranchAddress("trt_rdo_timeoverthr", trt_rdo_timeoverthr, &b_trt_rdo_timeoverthr);
   fChain->SetBranchAddress("trt_rdo_highlevel", trt_rdo_highlevel, &b_trt_rdo_highlevel);
   fChain->SetBranchAddress("trt_rdo_timepll", trt_rdo_timepll, &b_trt_rdo_timepll);
   fChain->SetBranchAddress("trt_rdo_straw", trt_rdo_straw, &b_trt_rdo_straw);
   fChain->SetBranchAddress("trt_rdo_strawlayer", trt_rdo_strawlayer, &b_trt_rdo_strawlayer);
   fChain->SetBranchAddress("trt_rdo_layer", trt_rdo_layer, &b_trt_rdo_layer);
   fChain->SetBranchAddress("trt_rdo_phisector", trt_rdo_phisector, &b_trt_rdo_phisector);
   fChain->SetBranchAddress("sct_clus_nclusters", &sct_clus_nclusters, &b_sct_clus_nclusters);
   fChain->SetBranchAddress("sct_clus_x", sct_clus_x, &b_sct_clus_x);
   fChain->SetBranchAddress("sct_clus_y", sct_clus_y, &b_sct_clus_y);
   fChain->SetBranchAddress("sct_clus_z", sct_clus_z, &b_sct_clus_z);
   fChain->SetBranchAddress("sct_clus_groupsize", sct_clus_groupsize, &b_sct_clus_groupsize);
   fChain->SetBranchAddress("sct_clus_layer", sct_clus_layer, &b_sct_clus_layer);
   fChain->SetBranchAddress("sct_clus_eta", sct_clus_eta, &b_sct_clus_eta);
   fChain->SetBranchAddress("sct_clus_phi", sct_clus_phi, &b_sct_clus_phi);
   fChain->SetBranchAddress("sct_clus_side", sct_clus_side, &b_sct_clus_side);
   fChain->SetBranchAddress("pixel_clus_nclusters", &pixel_clus_nclusters, &b_pixel_clus_nclusters);
   fChain->SetBranchAddress("pixel_clus_LocX", pixel_clus_LocX, &b_pixel_clus_LocX);
   fChain->SetBranchAddress("pixel_clus_LocY", pixel_clus_LocY, &b_pixel_clus_LocY);
   fChain->SetBranchAddress("pixel_clus_groupsize", pixel_clus_groupsize, &b_pixel_clus_groupsize);
   fChain->SetBranchAddress("pixel_clus_layer", pixel_clus_layer, &b_pixel_clus_layer);
   fChain->SetBranchAddress("pixel_clus_phi", pixel_clus_phi, &b_pixel_clus_phi);
   fChain->SetBranchAddress("trt_nDCs", &trt_nDCs, &b_trt_nDCs);
   fChain->SetBranchAddress("trt_DC_timeoverthr", trt_DC_timeoverthr, &b_trt_DC_timeoverthr);
   fChain->SetBranchAddress("trt_DC_highlevel", trt_DC_highlevel, &b_trt_DC_highlevel);
   fChain->SetBranchAddress("trt_DC_radius", trt_DC_radius, &b_trt_DC_radius);
   fChain->SetBranchAddress("trt_DC_straw", trt_DC_straw, &b_trt_DC_straw);
   fChain->SetBranchAddress("trt_DC_strawlayer", trt_DC_strawlayer, &b_trt_DC_strawlayer);
   fChain->SetBranchAddress("trt_DC_layer", trt_DC_layer, &b_trt_DC_layer);
   fChain->SetBranchAddress("trt_DC_phisector", trt_DC_phisector, &b_trt_DC_phisector);
   fChain->SetBranchAddress("trt_DC_straw_y", trt_DC_straw_y, &b_trt_DC_straw_y);
   fChain->SetBranchAddress("trt_DC_straw_x", trt_DC_straw_x, &b_trt_DC_straw_x);
   fChain->SetBranchAddress("sct_sp_nspacepoints", &sct_sp_nspacepoints, &b_sct_sp_nspacepoints);
   fChain->SetBranchAddress("sct_sp_x", sct_sp_x, &b_sct_sp_x);
   fChain->SetBranchAddress("sct_sp_y", sct_sp_y, &b_sct_sp_y);
   fChain->SetBranchAddress("sct_sp_z", sct_sp_z, &b_sct_sp_z);
   fChain->SetBranchAddress("sct_sp_layer", sct_sp_layer, &b_sct_sp_layer);
   fChain->SetBranchAddress("sct_sp_phi", sct_sp_phi, &b_sct_sp_phi);
   fChain->SetBranchAddress("pixel_sp_nspacepoints", &pixel_sp_nspacepoints, &b_pixel_sp_nspacepoints);
   fChain->SetBranchAddress("pixel_sp_x", pixel_sp_x, &b_pixel_sp_x);
   fChain->SetBranchAddress("pixel_sp_y", pixel_sp_y, &b_pixel_sp_y);
   fChain->SetBranchAddress("pixel_sp_z", pixel_sp_z, &b_pixel_sp_z);
   fChain->SetBranchAddress("pixel_sp_layer", pixel_sp_layer, &b_pixel_sp_layer);
   fChain->SetBranchAddress("pixel_sp_phi", pixel_sp_phi, &b_pixel_sp_phi);
   fChain->SetBranchAddress("trk_nTracks", &trk_nTracks, &b_trk_nTracks);
   fChain->SetBranchAddress("trk_nPixelHits", trk_nPixelHits, &b_trk_nPixelHits);
   fChain->SetBranchAddress("trk_PixResPhi", trk_PixResPhi, &b_trk_PixResPhi);
   fChain->SetBranchAddress("trk_PixResEta", trk_PixResEta, &b_trk_PixResEta);
   fChain->SetBranchAddress("trk_PixPullPhi", trk_PixPullPhi, &b_trk_PixPullPhi);
   fChain->SetBranchAddress("trk_PixPullEta", trk_PixPullEta, &b_trk_PixPullEta);
   fChain->SetBranchAddress("trk_PixErrTrkPhi", trk_PixErrTrkPhi, &b_trk_PixErrTrkPhi);
   fChain->SetBranchAddress("trk_PixErrTrkEta", trk_PixErrTrkEta, &b_trk_PixErrTrkEta);
   fChain->SetBranchAddress("trk_PixErrHitPhi", trk_PixErrHitPhi, &b_trk_PixErrHitPhi);
   fChain->SetBranchAddress("trk_PixErrHitEta", trk_PixErrHitEta, &b_trk_PixErrHitEta);
   fChain->SetBranchAddress("trk_Pix_layer", trk_Pix_layer, &b_trk_Pix_layer);
   fChain->SetBranchAddress("trk_Sct_eta", trk_Sct_eta, &b_trk_Sct_eta);
   fChain->SetBranchAddress("trk_Pix_phi", trk_Pix_phi, &b_trk_Pix_phi);
   fChain->SetBranchAddress("trk_Pix_clusWidthRow", trk_Pix_clusWidthRow, &b_trk_Pix_clusWidthRow);
   fChain->SetBranchAddress("trk_Pix_clusWidthCol", trk_Pix_clusWidthCol, &b_trk_Pix_clusWidthCol);
   fChain->SetBranchAddress("trk_nSctHits", trk_nSctHits, &b_trk_nSctHits);
   fChain->SetBranchAddress("trk_SctRes", trk_SctRes, &b_trk_SctRes);
   fChain->SetBranchAddress("trk_SctPull", trk_SctPull, &b_trk_SctPull);
   fChain->SetBranchAddress("trk_SctErrTrk", trk_SctErrTrk, &b_trk_SctErrTrk);
   fChain->SetBranchAddress("trk_SctErrHit", trk_SctErrHit, &b_trk_SctErrHit);
   fChain->SetBranchAddress("trk_Sct_layer", trk_Sct_layer, &b_trk_Sct_layer);
   fChain->SetBranchAddress("trk_Sct_phi", trk_Sct_phi, &b_trk_Sct_phi);
   fChain->SetBranchAddress("trk_Sct_side", trk_Sct_side, &b_trk_Sct_side);
   fChain->SetBranchAddress("trk_Sct_rdoId", trk_Sct_rdoId, &b_trk_Sct_rdoId);
   fChain->SetBranchAddress("trk_Sct_clusGroupsize", trk_Sct_clusGroupsize, &b_trk_Sct_clusGroupsize);
   fChain->SetBranchAddress("trk_Sct_clusx", trk_Sct_clusx, &b_trk_Sct_clusx);
   fChain->SetBranchAddress("trk_Sct_clusy", trk_Sct_clusy, &b_trk_Sct_clusy);
   fChain->SetBranchAddress("trk_Sct_clusz", trk_Sct_clusz, &b_trk_Sct_clusz);
   fChain->SetBranchAddress("trk_nTrtHits", trk_nTrtHits, &b_trk_nTrtHits);
   fChain->SetBranchAddress("trk_TrtRes", trk_TrtRes, &b_trk_TrtRes);
   fChain->SetBranchAddress("trk_TrtPull", trk_TrtPull, &b_trk_TrtPull);
   fChain->SetBranchAddress("trk_TrtErrTrk", trk_TrtErrTrk, &b_trk_TrtErrTrk);
   fChain->SetBranchAddress("trk_TrtErrHit", trk_TrtErrHit, &b_trk_TrtErrHit);
   fChain->SetBranchAddress("trk_Trt_R", trk_Trt_R, &b_trk_Trt_R);
   fChain->SetBranchAddress("trk_Trt_Rtrack", trk_Trt_Rtrack, &b_trk_Trt_Rtrack);
   fChain->SetBranchAddress("trk_Trt_t", trk_Trt_t, &b_trk_Trt_t);
   fChain->SetBranchAddress("trk_Trt_x", trk_Trt_x, &b_trk_Trt_x);
   fChain->SetBranchAddress("trk_Trt_y", trk_Trt_y, &b_trk_Trt_y);
   fChain->SetBranchAddress("trk_Trt_HL", trk_Trt_HL, &b_trk_Trt_HL);
   fChain->SetBranchAddress("trk_Trt_strawlayer", trk_Trt_strawlayer, &b_trk_Trt_strawlayer);
   fChain->SetBranchAddress("trk_Trt_layer", trk_Trt_layer, &b_trk_Trt_layer);
   fChain->SetBranchAddress("trk_Trt_straw", trk_Trt_straw, &b_trk_Trt_straw);
   fChain->SetBranchAddress("trk_Trt_phisector", trk_Trt_phisector, &b_trk_Trt_phisector);
   fChain->SetBranchAddress("trk_Rec_d0", trk_Rec_d0, &b_trk_Rec_d0);
   fChain->SetBranchAddress("trk_Rec_z0", trk_Rec_z0, &b_trk_Rec_z0);
   fChain->SetBranchAddress("trk_Rec_phi0", trk_Rec_phi0, &b_trk_Rec_phi0);
   fChain->SetBranchAddress("trk_Rec_theta0", trk_Rec_theta0, &b_trk_Rec_theta0);
   fChain->SetBranchAddress("trk_Rec_qoverp", trk_Rec_qoverp, &b_trk_Rec_qoverp);
   fChain->SetBranchAddress("trk_Rec_chi2", trk_Rec_chi2, &b_trk_Rec_chi2);
   fChain->SetBranchAddress("trk_Rec_ndof", trk_Rec_ndof, &b_trk_Rec_ndof);
   fChain->SetBranchAddress("trk_Rec_eta", trk_Rec_eta, &b_trk_Rec_eta);
   fChain->SetBranchAddress("trk_Rec_error_d0", trk_Rec_error_d0, &b_trk_Rec_error_d0);
   fChain->SetBranchAddress("trk_Rec_error_z0", trk_Rec_error_z0, &b_trk_Rec_error_z0);
   fChain->SetBranchAddress("trk_Rec_error_phi0", trk_Rec_error_phi0, &b_trk_Rec_error_phi0);
   fChain->SetBranchAddress("trk_Rec_error_theta0", trk_Rec_error_theta0, &b_trk_Rec_error_theta0);
   fChain->SetBranchAddress("trk_Rec_error_qoverp", trk_Rec_error_qoverp, &b_trk_Rec_error_qoverp);
   fChain->SetBranchAddress("trk_Rec_chi2prob", trk_Rec_chi2prob, &b_trk_Rec_chi2prob);
   fChain->SetBranchAddress("trk_nTracksSi", &trk_nTracksSi, &b_trk_nTracksSi);
   fChain->SetBranchAddress("trk_nPixelHitsSi", trk_nPixelHitsSi, &b_trk_nPixelHitsSi);
   fChain->SetBranchAddress("trk_PixResPhiSi", trk_PixResPhiSi, &b_trk_PixResPhiSi);
   fChain->SetBranchAddress("trk_PixResEtaSi", trk_PixResEtaSi, &b_trk_PixResEtaSi);
   fChain->SetBranchAddress("trk_PixPullPhiSi", trk_PixPullPhiSi, &b_trk_PixPullPhiSi);
   fChain->SetBranchAddress("trk_PixPullEtaSi", trk_PixPullEtaSi, &b_trk_PixPullEtaSi);
   fChain->SetBranchAddress("trk_PixErrTrkPhiSi", trk_PixErrTrkPhiSi, &b_trk_PixErrTrkPhiSi);
   fChain->SetBranchAddress("trk_PixErrTrkEtaSi", trk_PixErrTrkEtaSi, &b_trk_PixErrTrkEtaSi);
   fChain->SetBranchAddress("trk_PixErrHitPhiSi", trk_PixErrHitPhiSi, &b_trk_PixErrHitPhiSi);
   fChain->SetBranchAddress("trk_PixErrHitEtaSi", trk_PixErrHitEtaSi, &b_trk_PixErrHitEtaSi);
   fChain->SetBranchAddress("trk_Pix_layerSi", trk_Pix_layerSi, &b_trk_Pix_layerSi);
   fChain->SetBranchAddress("trk_Sct_etaSi", trk_Sct_etaSi, &b_trk_Sct_etaSi);
   fChain->SetBranchAddress("trk_Pix_phiSi", trk_Pix_phiSi, &b_trk_Pix_phiSi);
   fChain->SetBranchAddress("trk_Pix_clusWidthRowSi", trk_Pix_clusWidthRowSi, &b_trk_Pix_clusWidthRowSi);
   fChain->SetBranchAddress("trk_Pix_clusWidthColSi", trk_Pix_clusWidthColSi, &b_trk_Pix_clusWidthColSi);
   fChain->SetBranchAddress("trk_nSctHitsSi", trk_nSctHitsSi, &b_trk_nSctHitsSi);
   fChain->SetBranchAddress("trk_SctResSi", trk_SctResSi, &b_trk_SctResSi);
   fChain->SetBranchAddress("trk_SctPullSi", trk_SctPullSi, &b_trk_SctPullSi);
   fChain->SetBranchAddress("trk_SctErrTrkSi", trk_SctErrTrkSi, &b_trk_SctErrTrkSi);
   fChain->SetBranchAddress("trk_SctErrHitSi", trk_SctErrHitSi, &b_trk_SctErrHitSi);
   fChain->SetBranchAddress("trk_Sct_layerSi", trk_Sct_layerSi, &b_trk_Sct_layerSi);
   fChain->SetBranchAddress("trk_Sct_phiSi", trk_Sct_phiSi, &b_trk_Sct_phiSi);
   fChain->SetBranchAddress("trk_Sct_sideSi", trk_Sct_sideSi, &b_trk_Sct_sideSi);
   fChain->SetBranchAddress("trk_Sct_rdoIdSi", trk_Sct_rdoIdSi, &b_trk_Sct_rdoIdSi);
   fChain->SetBranchAddress("trk_Sct_clusGroupsizeSi", trk_Sct_clusGroupsizeSi, &b_trk_Sct_clusGroupsizeSi);
   fChain->SetBranchAddress("trk_Sct_clusxSi", trk_Sct_clusxSi, &b_trk_Sct_clusxSi);
   fChain->SetBranchAddress("trk_Sct_clusySi", trk_Sct_clusySi, &b_trk_Sct_clusySi);
   fChain->SetBranchAddress("trk_Sct_cluszSi", trk_Sct_cluszSi, &b_trk_Sct_cluszSi);
   fChain->SetBranchAddress("trk_nTrtHitsSi", trk_nTrtHitsSi, &b_trk_nTrtHitsSi);
   fChain->SetBranchAddress("trk_TrtResSi", trk_TrtResSi, &b_trk_TrtResSi);
   fChain->SetBranchAddress("trk_TrtPullSi", trk_TrtPullSi, &b_trk_TrtPullSi);
   fChain->SetBranchAddress("trk_TrtErrTrkSi", trk_TrtErrTrkSi, &b_trk_TrtErrTrkSi);
   fChain->SetBranchAddress("trk_TrtErrHitSi", trk_TrtErrHitSi, &b_trk_TrtErrHitSi);
   fChain->SetBranchAddress("trk_Trt_RSi", trk_Trt_RSi, &b_trk_Trt_RSi);
   fChain->SetBranchAddress("trk_Trt_RtrackSi", trk_Trt_RtrackSi, &b_trk_Trt_RtrackSi);
   fChain->SetBranchAddress("trk_Trt_tSi", trk_Trt_tSi, &b_trk_Trt_tSi);
   fChain->SetBranchAddress("trk_Trt_xSi", trk_Trt_xSi, &b_trk_Trt_xSi);
   fChain->SetBranchAddress("trk_Trt_ySi", trk_Trt_ySi, &b_trk_Trt_ySi);
   fChain->SetBranchAddress("trk_Trt_HLSi", trk_Trt_HLSi, &b_trk_Trt_HLSi);
   fChain->SetBranchAddress("trk_Trt_strawlayerSi", trk_Trt_strawlayerSi, &b_trk_Trt_strawlayerSi);
   fChain->SetBranchAddress("trk_Trt_layerSi", trk_Trt_layerSi, &b_trk_Trt_layerSi);
   fChain->SetBranchAddress("trk_Trt_strawSi", trk_Trt_strawSi, &b_trk_Trt_strawSi);
   fChain->SetBranchAddress("trk_Trt_phisectorSi", trk_Trt_phisectorSi, &b_trk_Trt_phisectorSi);
   fChain->SetBranchAddress("trk_Rec_d0Si", trk_Rec_d0Si, &b_trk_Rec_d0Si);
   fChain->SetBranchAddress("trk_Rec_z0Si", trk_Rec_z0Si, &b_trk_Rec_z0Si);
   fChain->SetBranchAddress("trk_Rec_phi0Si", trk_Rec_phi0Si, &b_trk_Rec_phi0Si);
   fChain->SetBranchAddress("trk_Rec_theta0Si", trk_Rec_theta0Si, &b_trk_Rec_theta0Si);
   fChain->SetBranchAddress("trk_Rec_qoverpSi", trk_Rec_qoverpSi, &b_trk_Rec_qoverpSi);
   fChain->SetBranchAddress("trk_Rec_chi2Si", trk_Rec_chi2Si, &b_trk_Rec_chi2Si);
   fChain->SetBranchAddress("trk_Rec_ndofSi", trk_Rec_ndofSi, &b_trk_Rec_ndofSi);
   fChain->SetBranchAddress("trk_Rec_etaSi", trk_Rec_etaSi, &b_trk_Rec_etaSi);
   fChain->SetBranchAddress("trk_Rec_error_d0Si", trk_Rec_error_d0Si, &b_trk_Rec_error_d0Si);
   fChain->SetBranchAddress("trk_Rec_error_z0Si", trk_Rec_error_z0Si, &b_trk_Rec_error_z0Si);
   fChain->SetBranchAddress("trk_Rec_error_phi0Si", trk_Rec_error_phi0Si, &b_trk_Rec_error_phi0Si);
   fChain->SetBranchAddress("trk_Rec_error_theta0Si", trk_Rec_error_theta0Si, &b_trk_Rec_error_theta0Si);
   fChain->SetBranchAddress("trk_Rec_error_qoverpSi", trk_Rec_error_qoverpSi, &b_trk_Rec_error_qoverpSi);
   fChain->SetBranchAddress("trk_Rec_chi2probSi", trk_Rec_chi2probSi, &b_trk_Rec_chi2probSi);
   fChain->SetBranchAddress("trk_nTracksPixel", &trk_nTracksPixel, &b_trk_nTracksPixel);
   fChain->SetBranchAddress("trk_nPixelHitsPixel", trk_nPixelHitsPixel, &b_trk_nPixelHitsPixel);
   fChain->SetBranchAddress("trk_PixResPhiPixel", trk_PixResPhiPixel, &b_trk_PixResPhiPixel);
   fChain->SetBranchAddress("trk_PixResEtaPixel", trk_PixResEtaPixel, &b_trk_PixResEtaPixel);
   fChain->SetBranchAddress("trk_PixPullPhiPixel", trk_PixPullPhiPixel, &b_trk_PixPullPhiPixel);
   fChain->SetBranchAddress("trk_PixPullEtaPixel", trk_PixPullEtaPixel, &b_trk_PixPullEtaPixel);
   fChain->SetBranchAddress("trk_PixErrTrkPhiPixel", trk_PixErrTrkPhiPixel, &b_trk_PixErrTrkPhiPixel);
   fChain->SetBranchAddress("trk_PixErrTrkEtaPixel", trk_PixErrTrkEtaPixel, &b_trk_PixErrTrkEtaPixel);
   fChain->SetBranchAddress("trk_PixErrHitPhiPixel", trk_PixErrHitPhiPixel, &b_trk_PixErrHitPhiPixel);
   fChain->SetBranchAddress("trk_PixErrHitEtaPixel", trk_PixErrHitEtaPixel, &b_trk_PixErrHitEtaPixel);
   fChain->SetBranchAddress("trk_Pix_layerPixel", trk_Pix_layerPixel, &b_trk_Pix_layerPixel);
   fChain->SetBranchAddress("trk_Sct_etaPixel", trk_Sct_etaPixel, &b_trk_Sct_etaPixel);
   fChain->SetBranchAddress("trk_Pix_phiPixel", trk_Pix_phiPixel, &b_trk_Pix_phiPixel);
   fChain->SetBranchAddress("trk_Pix_clusWidthRowPixel", trk_Pix_clusWidthRowPixel, &b_trk_Pix_clusWidthRowPixel);
   fChain->SetBranchAddress("trk_Pix_clusWidthColPixel", trk_Pix_clusWidthColPixel, &b_trk_Pix_clusWidthColPixel);
   fChain->SetBranchAddress("trk_nSctHitsPixel", trk_nSctHitsPixel, &b_trk_nSctHitsPixel);
   fChain->SetBranchAddress("trk_SctResPixel", trk_SctResPixel, &b_trk_SctResPixel);
   fChain->SetBranchAddress("trk_SctPullPixel", trk_SctPullPixel, &b_trk_SctPullPixel);
   fChain->SetBranchAddress("trk_SctErrTrkPixel", trk_SctErrTrkPixel, &b_trk_SctErrTrkPixel);
   fChain->SetBranchAddress("trk_SctErrHitPixel", trk_SctErrHitPixel, &b_trk_SctErrHitPixel);
   fChain->SetBranchAddress("trk_Sct_layerPixel", trk_Sct_layerPixel, &b_trk_Sct_layerPixel);
   fChain->SetBranchAddress("trk_Sct_phiPixel", trk_Sct_phiPixel, &b_trk_Sct_phiPixel);
   fChain->SetBranchAddress("trk_Sct_sidePixel", trk_Sct_sidePixel, &b_trk_Sct_sidePixel);
   fChain->SetBranchAddress("trk_Sct_rdoIdPixel", trk_Sct_rdoIdPixel, &b_trk_Sct_rdoIdPixel);
   fChain->SetBranchAddress("trk_Sct_clusGroupsizePixel", trk_Sct_clusGroupsizePixel, &b_trk_Sct_clusGroupsizePixel);
   fChain->SetBranchAddress("trk_Sct_clusxPixel", trk_Sct_clusxPixel, &b_trk_Sct_clusxPixel);
   fChain->SetBranchAddress("trk_Sct_clusyPixel", trk_Sct_clusyPixel, &b_trk_Sct_clusyPixel);
   fChain->SetBranchAddress("trk_Sct_cluszPixel", trk_Sct_cluszPixel, &b_trk_Sct_cluszPixel);
   fChain->SetBranchAddress("trk_nTrtHitsPixel", trk_nTrtHitsPixel, &b_trk_nTrtHitsPixel);
   fChain->SetBranchAddress("trk_TrtResPixel", trk_TrtResPixel, &b_trk_TrtResPixel);
   fChain->SetBranchAddress("trk_TrtPullPixel", trk_TrtPullPixel, &b_trk_TrtPullPixel);
   fChain->SetBranchAddress("trk_TrtErrTrkPixel", trk_TrtErrTrkPixel, &b_trk_TrtErrTrkPixel);
   fChain->SetBranchAddress("trk_TrtErrHitPixel", trk_TrtErrHitPixel, &b_trk_TrtErrHitPixel);
   fChain->SetBranchAddress("trk_Trt_RPixel", trk_Trt_RPixel, &b_trk_Trt_RPixel);
   fChain->SetBranchAddress("trk_Trt_RtrackPixel", trk_Trt_RtrackPixel, &b_trk_Trt_RtrackPixel);
   fChain->SetBranchAddress("trk_Trt_tPixel", trk_Trt_tPixel, &b_trk_Trt_tPixel);
   fChain->SetBranchAddress("trk_Trt_xPixel", trk_Trt_xPixel, &b_trk_Trt_xPixel);
   fChain->SetBranchAddress("trk_Trt_yPixel", trk_Trt_yPixel, &b_trk_Trt_yPixel);
   fChain->SetBranchAddress("trk_Trt_HLPixel", trk_Trt_HLPixel, &b_trk_Trt_HLPixel);
   fChain->SetBranchAddress("trk_Trt_strawlayerPixel", trk_Trt_strawlayerPixel, &b_trk_Trt_strawlayerPixel);
   fChain->SetBranchAddress("trk_Trt_layerPixel", trk_Trt_layerPixel, &b_trk_Trt_layerPixel);
   fChain->SetBranchAddress("trk_Trt_strawPixel", trk_Trt_strawPixel, &b_trk_Trt_strawPixel);
   fChain->SetBranchAddress("trk_Trt_phisectorPixel", trk_Trt_phisectorPixel, &b_trk_Trt_phisectorPixel);
   fChain->SetBranchAddress("trk_Rec_d0Pixel", trk_Rec_d0Pixel, &b_trk_Rec_d0Pixel);
   fChain->SetBranchAddress("trk_Rec_z0Pixel", trk_Rec_z0Pixel, &b_trk_Rec_z0Pixel);
   fChain->SetBranchAddress("trk_Rec_phi0Pixel", trk_Rec_phi0Pixel, &b_trk_Rec_phi0Pixel);
   fChain->SetBranchAddress("trk_Rec_theta0Pixel", trk_Rec_theta0Pixel, &b_trk_Rec_theta0Pixel);
   fChain->SetBranchAddress("trk_Rec_qoverpPixel", trk_Rec_qoverpPixel, &b_trk_Rec_qoverpPixel);
   fChain->SetBranchAddress("trk_Rec_chi2Pixel", trk_Rec_chi2Pixel, &b_trk_Rec_chi2Pixel);
   fChain->SetBranchAddress("trk_Rec_ndofPixel", trk_Rec_ndofPixel, &b_trk_Rec_ndofPixel);
   fChain->SetBranchAddress("trk_Rec_etaPixel", trk_Rec_etaPixel, &b_trk_Rec_etaPixel);
   fChain->SetBranchAddress("trk_Rec_error_d0Pixel", trk_Rec_error_d0Pixel, &b_trk_Rec_error_d0Pixel);
   fChain->SetBranchAddress("trk_Rec_error_z0Pixel", trk_Rec_error_z0Pixel, &b_trk_Rec_error_z0Pixel);
   fChain->SetBranchAddress("trk_Rec_error_phi0Pixel", trk_Rec_error_phi0Pixel, &b_trk_Rec_error_phi0Pixel);
   fChain->SetBranchAddress("trk_Rec_error_theta0Pixel", trk_Rec_error_theta0Pixel, &b_trk_Rec_error_theta0Pixel);
   fChain->SetBranchAddress("trk_Rec_error_qoverpPixel", trk_Rec_error_qoverpPixel, &b_trk_Rec_error_qoverpPixel);
   fChain->SetBranchAddress("trk_Rec_chi2probPixel", trk_Rec_chi2probPixel, &b_trk_Rec_chi2probPixel);
   fChain->SetBranchAddress("trk_nTracksSCT", &trk_nTracksSCT, &b_trk_nTracksSCT);
   fChain->SetBranchAddress("trk_nPixelHitsSCT", trk_nPixelHitsSCT, &b_trk_nPixelHitsSCT);
   fChain->SetBranchAddress("trk_PixResPhiSCT", trk_PixResPhiSCT, &b_trk_PixResPhiSCT);
   fChain->SetBranchAddress("trk_PixResEtaSCT", trk_PixResEtaSCT, &b_trk_PixResEtaSCT);
   fChain->SetBranchAddress("trk_PixPullPhiSCT", trk_PixPullPhiSCT, &b_trk_PixPullPhiSCT);
   fChain->SetBranchAddress("trk_PixPullEtaSCT", trk_PixPullEtaSCT, &b_trk_PixPullEtaSCT);
   fChain->SetBranchAddress("trk_PixErrTrkPhiSCT", trk_PixErrTrkPhiSCT, &b_trk_PixErrTrkPhiSCT);
   fChain->SetBranchAddress("trk_PixErrTrkEtaSCT", trk_PixErrTrkEtaSCT, &b_trk_PixErrTrkEtaSCT);
   fChain->SetBranchAddress("trk_PixErrHitPhiSCT", trk_PixErrHitPhiSCT, &b_trk_PixErrHitPhiSCT);
   fChain->SetBranchAddress("trk_PixErrHitEtaSCT", trk_PixErrHitEtaSCT, &b_trk_PixErrHitEtaSCT);
   fChain->SetBranchAddress("trk_Pix_layerSCT", trk_Pix_layerSCT, &b_trk_Pix_layerSCT);
   fChain->SetBranchAddress("trk_Sct_etaSCT", trk_Sct_etaSCT, &b_trk_Sct_etaSCT);
   fChain->SetBranchAddress("trk_Pix_phiSCT", trk_Pix_phiSCT, &b_trk_Pix_phiSCT);
   fChain->SetBranchAddress("trk_Pix_clusWidthRowSCT", trk_Pix_clusWidthRowSCT, &b_trk_Pix_clusWidthRowSCT);
   fChain->SetBranchAddress("trk_Pix_clusWidthColSCT", trk_Pix_clusWidthColSCT, &b_trk_Pix_clusWidthColSCT);
   fChain->SetBranchAddress("trk_nSctHitsSCT", trk_nSctHitsSCT, &b_trk_nSctHitsSCT);
   fChain->SetBranchAddress("trk_SctResSCT", trk_SctResSCT, &b_trk_SctResSCT);
   fChain->SetBranchAddress("trk_SctPullSCT", trk_SctPullSCT, &b_trk_SctPullSCT);
   fChain->SetBranchAddress("trk_SctErrTrkSCT", trk_SctErrTrkSCT, &b_trk_SctErrTrkSCT);
   fChain->SetBranchAddress("trk_SctErrHitSCT", trk_SctErrHitSCT, &b_trk_SctErrHitSCT);
   fChain->SetBranchAddress("trk_Sct_layerSCT", trk_Sct_layerSCT, &b_trk_Sct_layerSCT);
   fChain->SetBranchAddress("trk_Sct_phiSCT", trk_Sct_phiSCT, &b_trk_Sct_phiSCT);
   fChain->SetBranchAddress("trk_Sct_sideSCT", trk_Sct_sideSCT, &b_trk_Sct_sideSCT);
   fChain->SetBranchAddress("trk_Sct_rdoIdSCT", trk_Sct_rdoIdSCT, &b_trk_Sct_rdoIdSCT);
   fChain->SetBranchAddress("trk_Sct_clusGroupsizeSCT", trk_Sct_clusGroupsizeSCT, &b_trk_Sct_clusGroupsizeSCT);
   fChain->SetBranchAddress("trk_Sct_clusxSCT", trk_Sct_clusxSCT, &b_trk_Sct_clusxSCT);
   fChain->SetBranchAddress("trk_Sct_clusySCT", trk_Sct_clusySCT, &b_trk_Sct_clusySCT);
   fChain->SetBranchAddress("trk_Sct_cluszSCT", trk_Sct_cluszSCT, &b_trk_Sct_cluszSCT);
   fChain->SetBranchAddress("trk_nTrtHitsSCT", trk_nTrtHitsSCT, &b_trk_nTrtHitsSCT);
   fChain->SetBranchAddress("trk_TrtResSCT", trk_TrtResSCT, &b_trk_TrtResSCT);
   fChain->SetBranchAddress("trk_TrtPullSCT", trk_TrtPullSCT, &b_trk_TrtPullSCT);
   fChain->SetBranchAddress("trk_TrtErrTrkSCT", trk_TrtErrTrkSCT, &b_trk_TrtErrTrkSCT);
   fChain->SetBranchAddress("trk_TrtErrHitSCT", trk_TrtErrHitSCT, &b_trk_TrtErrHitSCT);
   fChain->SetBranchAddress("trk_Trt_RSCT", trk_Trt_RSCT, &b_trk_Trt_RSCT);
   fChain->SetBranchAddress("trk_Trt_RtrackSCT", trk_Trt_RtrackSCT, &b_trk_Trt_RtrackSCT);
   fChain->SetBranchAddress("trk_Trt_tSCT", trk_Trt_tSCT, &b_trk_Trt_tSCT);
   fChain->SetBranchAddress("trk_Trt_xSCT", trk_Trt_xSCT, &b_trk_Trt_xSCT);
   fChain->SetBranchAddress("trk_Trt_ySCT", trk_Trt_ySCT, &b_trk_Trt_ySCT);
   fChain->SetBranchAddress("trk_Trt_HLSCT", trk_Trt_HLSCT, &b_trk_Trt_HLSCT);
   fChain->SetBranchAddress("trk_Trt_strawlayerSCT", trk_Trt_strawlayerSCT, &b_trk_Trt_strawlayerSCT);
   fChain->SetBranchAddress("trk_Trt_layerSCT", trk_Trt_layerSCT, &b_trk_Trt_layerSCT);
   fChain->SetBranchAddress("trk_Trt_strawSCT", trk_Trt_strawSCT, &b_trk_Trt_strawSCT);
   fChain->SetBranchAddress("trk_Trt_phisectorSCT", trk_Trt_phisectorSCT, &b_trk_Trt_phisectorSCT);
   fChain->SetBranchAddress("trk_Rec_d0SCT", trk_Rec_d0SCT, &b_trk_Rec_d0SCT);
   fChain->SetBranchAddress("trk_Rec_z0SCT", trk_Rec_z0SCT, &b_trk_Rec_z0SCT);
   fChain->SetBranchAddress("trk_Rec_phi0SCT", trk_Rec_phi0SCT, &b_trk_Rec_phi0SCT);
   fChain->SetBranchAddress("trk_Rec_theta0SCT", trk_Rec_theta0SCT, &b_trk_Rec_theta0SCT);
   fChain->SetBranchAddress("trk_Rec_qoverpSCT", trk_Rec_qoverpSCT, &b_trk_Rec_qoverpSCT);
   fChain->SetBranchAddress("trk_Rec_chi2SCT", trk_Rec_chi2SCT, &b_trk_Rec_chi2SCT);
   fChain->SetBranchAddress("trk_Rec_ndofSCT", trk_Rec_ndofSCT, &b_trk_Rec_ndofSCT);
   fChain->SetBranchAddress("trk_Rec_etaSCT", trk_Rec_etaSCT, &b_trk_Rec_etaSCT);
   fChain->SetBranchAddress("trk_Rec_error_d0SCT", trk_Rec_error_d0SCT, &b_trk_Rec_error_d0SCT);
   fChain->SetBranchAddress("trk_Rec_error_z0SCT", trk_Rec_error_z0SCT, &b_trk_Rec_error_z0SCT);
   fChain->SetBranchAddress("trk_Rec_error_phi0SCT", trk_Rec_error_phi0SCT, &b_trk_Rec_error_phi0SCT);
   fChain->SetBranchAddress("trk_Rec_error_theta0SCT", trk_Rec_error_theta0SCT, &b_trk_Rec_error_theta0SCT);
   fChain->SetBranchAddress("trk_Rec_error_qoverpSCT", trk_Rec_error_qoverpSCT, &b_trk_Rec_error_qoverpSCT);
   fChain->SetBranchAddress("trk_Rec_chi2probSCT", trk_Rec_chi2probSCT, &b_trk_Rec_chi2probSCT);
   fChain->SetBranchAddress("trk_nTracksTRT", &trk_nTracksTRT, &b_trk_nTracksTRT);
   fChain->SetBranchAddress("trk_nPixelHitsTRT", trk_nPixelHitsTRT, &b_trk_nPixelHitsTRT);
   fChain->SetBranchAddress("trk_PixResPhiTRT", trk_PixResPhiTRT, &b_trk_PixResPhiTRT);
   fChain->SetBranchAddress("trk_PixResEtaTRT", trk_PixResEtaTRT, &b_trk_PixResEtaTRT);
   fChain->SetBranchAddress("trk_PixPullPhiTRT", trk_PixPullPhiTRT, &b_trk_PixPullPhiTRT);
   fChain->SetBranchAddress("trk_PixPullEtaTRT", trk_PixPullEtaTRT, &b_trk_PixPullEtaTRT);
   fChain->SetBranchAddress("trk_PixErrTrkPhiTRT", trk_PixErrTrkPhiTRT, &b_trk_PixErrTrkPhiTRT);
   fChain->SetBranchAddress("trk_PixErrTrkEtaTRT", trk_PixErrTrkEtaTRT, &b_trk_PixErrTrkEtaTRT);
   fChain->SetBranchAddress("trk_PixErrHitPhiTRT", trk_PixErrHitPhiTRT, &b_trk_PixErrHitPhiTRT);
   fChain->SetBranchAddress("trk_PixErrHitEtaTRT", trk_PixErrHitEtaTRT, &b_trk_PixErrHitEtaTRT);
   fChain->SetBranchAddress("trk_Pix_layerTRT", trk_Pix_layerTRT, &b_trk_Pix_layerTRT);
   fChain->SetBranchAddress("trk_Sct_etaTRT", trk_Sct_etaTRT, &b_trk_Sct_etaTRT);
   fChain->SetBranchAddress("trk_Pix_phiTRT", trk_Pix_phiTRT, &b_trk_Pix_phiTRT);
   fChain->SetBranchAddress("trk_Pix_clusWidthRowTRT", trk_Pix_clusWidthRowTRT, &b_trk_Pix_clusWidthRowTRT);
   fChain->SetBranchAddress("trk_Pix_clusWidthColTRT", trk_Pix_clusWidthColTRT, &b_trk_Pix_clusWidthColTRT);
   fChain->SetBranchAddress("trk_nSctHitsTRT", trk_nSctHitsTRT, &b_trk_nSctHitsTRT);
   fChain->SetBranchAddress("trk_SctResTRT", trk_SctResTRT, &b_trk_SctResTRT);
   fChain->SetBranchAddress("trk_SctPullTRT", trk_SctPullTRT, &b_trk_SctPullTRT);
   fChain->SetBranchAddress("trk_SctErrTrkTRT", trk_SctErrTrkTRT, &b_trk_SctErrTrkTRT);
   fChain->SetBranchAddress("trk_SctErrHitTRT", trk_SctErrHitTRT, &b_trk_SctErrHitTRT);
   fChain->SetBranchAddress("trk_Sct_layerTRT", trk_Sct_layerTRT, &b_trk_Sct_layerTRT);
   fChain->SetBranchAddress("trk_Sct_phiTRT", trk_Sct_phiTRT, &b_trk_Sct_phiTRT);
   fChain->SetBranchAddress("trk_Sct_sideTRT", trk_Sct_sideTRT, &b_trk_Sct_sideTRT);
   fChain->SetBranchAddress("trk_Sct_rdoIdTRT", trk_Sct_rdoIdTRT, &b_trk_Sct_rdoIdTRT);
   fChain->SetBranchAddress("trk_Sct_clusGroupsizeTRT", trk_Sct_clusGroupsizeTRT, &b_trk_Sct_clusGroupsizeTRT);
   fChain->SetBranchAddress("trk_Sct_clusxTRT", trk_Sct_clusxTRT, &b_trk_Sct_clusxTRT);
   fChain->SetBranchAddress("trk_Sct_clusyTRT", trk_Sct_clusyTRT, &b_trk_Sct_clusyTRT);
   fChain->SetBranchAddress("trk_Sct_cluszTRT", trk_Sct_cluszTRT, &b_trk_Sct_cluszTRT);
   fChain->SetBranchAddress("trk_nTrtHitsTRT", trk_nTrtHitsTRT, &b_trk_nTrtHitsTRT);
   fChain->SetBranchAddress("trk_TrtResTRT", trk_TrtResTRT, &b_trk_TrtResTRT);
   fChain->SetBranchAddress("trk_TrtPullTRT", trk_TrtPullTRT, &b_trk_TrtPullTRT);
   fChain->SetBranchAddress("trk_TrtErrTrkTRT", trk_TrtErrTrkTRT, &b_trk_TrtErrTrkTRT);
   fChain->SetBranchAddress("trk_TrtErrHitTRT", trk_TrtErrHitTRT, &b_trk_TrtErrHitTRT);
   fChain->SetBranchAddress("trk_Trt_RTRT", trk_Trt_RTRT, &b_trk_Trt_RTRT);
   fChain->SetBranchAddress("trk_Trt_RtrackTRT", trk_Trt_RtrackTRT, &b_trk_Trt_RtrackTRT);
   fChain->SetBranchAddress("trk_Trt_tTRT", trk_Trt_tTRT, &b_trk_Trt_tTRT);
   fChain->SetBranchAddress("trk_Trt_xTRT", trk_Trt_xTRT, &b_trk_Trt_xTRT);
   fChain->SetBranchAddress("trk_Trt_yTRT", trk_Trt_yTRT, &b_trk_Trt_yTRT);
   fChain->SetBranchAddress("trk_Trt_HLTRT", trk_Trt_HLTRT, &b_trk_Trt_HLTRT);
   fChain->SetBranchAddress("trk_Trt_strawlayerTRT", trk_Trt_strawlayerTRT, &b_trk_Trt_strawlayerTRT);
   fChain->SetBranchAddress("trk_Trt_layerTRT", trk_Trt_layerTRT, &b_trk_Trt_layerTRT);
   fChain->SetBranchAddress("trk_Trt_strawTRT", trk_Trt_strawTRT, &b_trk_Trt_strawTRT);
   fChain->SetBranchAddress("trk_Trt_phisectorTRT", trk_Trt_phisectorTRT, &b_trk_Trt_phisectorTRT);
   fChain->SetBranchAddress("trk_Rec_d0TRT", trk_Rec_d0TRT, &b_trk_Rec_d0TRT);
   fChain->SetBranchAddress("trk_Rec_z0TRT", trk_Rec_z0TRT, &b_trk_Rec_z0TRT);
   fChain->SetBranchAddress("trk_Rec_phi0TRT", trk_Rec_phi0TRT, &b_trk_Rec_phi0TRT);
   fChain->SetBranchAddress("trk_Rec_theta0TRT", trk_Rec_theta0TRT, &b_trk_Rec_theta0TRT);
   fChain->SetBranchAddress("trk_Rec_qoverpTRT", trk_Rec_qoverpTRT, &b_trk_Rec_qoverpTRT);
   fChain->SetBranchAddress("trk_Rec_chi2TRT", trk_Rec_chi2TRT, &b_trk_Rec_chi2TRT);
   fChain->SetBranchAddress("trk_Rec_ndofTRT", trk_Rec_ndofTRT, &b_trk_Rec_ndofTRT);
   fChain->SetBranchAddress("trk_Rec_etaTRT", trk_Rec_etaTRT, &b_trk_Rec_etaTRT);
   fChain->SetBranchAddress("trk_Rec_error_d0TRT", trk_Rec_error_d0TRT, &b_trk_Rec_error_d0TRT);
   fChain->SetBranchAddress("trk_Rec_error_z0TRT", trk_Rec_error_z0TRT, &b_trk_Rec_error_z0TRT);
   fChain->SetBranchAddress("trk_Rec_error_phi0TRT", trk_Rec_error_phi0TRT, &b_trk_Rec_error_phi0TRT);
   fChain->SetBranchAddress("trk_Rec_error_theta0TRT", trk_Rec_error_theta0TRT, &b_trk_Rec_error_theta0TRT);
   fChain->SetBranchAddress("trk_Rec_error_qoverpTRT", trk_Rec_error_qoverpTRT, &b_trk_Rec_error_qoverpTRT);
   fChain->SetBranchAddress("trk_Rec_chi2probTRT", trk_Rec_chi2probTRT, &b_trk_Rec_chi2probTRT);
   fChain->SetBranchAddress("Trk_totalNumTracks", &Trk_totalNumTracks, &b_Trk_totalNumTracks);
   fChain->SetBranchAddress("Trk_d0", Trk_d0, &b_Trk_d0);
   fChain->SetBranchAddress("Trk_z0", Trk_z0, &b_Trk_z0);
   fChain->SetBranchAddress("Trk_phi", Trk_phi, &b_Trk_phi);
   fChain->SetBranchAddress("Trk_theta", Trk_theta, &b_Trk_theta);
   fChain->SetBranchAddress("Trk_qOverP", Trk_qOverP, &b_Trk_qOverP);
   fChain->SetBranchAddress("Trk_Chi2", Trk_Chi2, &b_Trk_Chi2);
   fChain->SetBranchAddress("Trk_Ndf", Trk_Ndf, &b_Trk_Ndf);
   fChain->SetBranchAddress("Trk_sigd0", Trk_sigd0, &b_Trk_sigd0);
   fChain->SetBranchAddress("Trk_sigz0", Trk_sigz0, &b_Trk_sigz0);
   fChain->SetBranchAddress("Trk_sigphi", Trk_sigphi, &b_Trk_sigphi);
   fChain->SetBranchAddress("Trk_sigtheta", Trk_sigtheta, &b_Trk_sigtheta);
   fChain->SetBranchAddress("Trk_sigqOverP", Trk_sigqOverP, &b_Trk_sigqOverP);
   fChain->SetBranchAddress("Trk_covVert21", Trk_covVert21, &b_Trk_covVert21);
   fChain->SetBranchAddress("Trk_covVert31", Trk_covVert31, &b_Trk_covVert31);
   fChain->SetBranchAddress("Trk_covVert32", Trk_covVert32, &b_Trk_covVert32);
   fChain->SetBranchAddress("Trk_covVert41", Trk_covVert41, &b_Trk_covVert41);
   fChain->SetBranchAddress("Trk_covVert42", Trk_covVert42, &b_Trk_covVert42);
   fChain->SetBranchAddress("Trk_covVert43", Trk_covVert43, &b_Trk_covVert43);
   fChain->SetBranchAddress("Trk_covVert51", Trk_covVert51, &b_Trk_covVert51);
   fChain->SetBranchAddress("Trk_covVert52", Trk_covVert52, &b_Trk_covVert52);
   fChain->SetBranchAddress("Trk_covVert53", Trk_covVert53, &b_Trk_covVert53);
   fChain->SetBranchAddress("Trk_covVert54", Trk_covVert54, &b_Trk_covVert54);
   fChain->SetBranchAddress("Trk_px", Trk_px, &b_Trk_px);
   fChain->SetBranchAddress("Trk_py", Trk_py, &b_Trk_py);
   fChain->SetBranchAddress("Trk_pz", Trk_pz, &b_Trk_pz);
   fChain->SetBranchAddress("Trk_pt", Trk_pt, &b_Trk_pt);
   fChain->SetBranchAddress("Trk_numberOfBLayerHits", Trk_numberOfBLayerHits, &b_Trk_numberOfBLayerHits);
   fChain->SetBranchAddress("Trk_numberOfPixelHits", Trk_numberOfPixelHits, &b_Trk_numberOfPixelHits);
   fChain->SetBranchAddress("Trk_numberOfSCTHits", Trk_numberOfSCTHits, &b_Trk_numberOfSCTHits);
   fChain->SetBranchAddress("Trk_numberOfTRTHits", Trk_numberOfTRTHits, &b_Trk_numberOfTRTHits);
   fChain->SetBranchAddress("Trk_numberOfTRTHighThresholdHits", Trk_numberOfTRTHighThresholdHits, &b_Trk_numberOfTRTHighThresholdHits);
   fChain->SetBranchAddress("Trk_numberOfBLayerSharedHits", Trk_numberOfBLayerSharedHits, &b_Trk_numberOfBLayerSharedHits);
   fChain->SetBranchAddress("Trk_numberOfPixelSharedHits", Trk_numberOfPixelSharedHits, &b_Trk_numberOfPixelSharedHits);
   fChain->SetBranchAddress("Trk_numberOfPixelHoles", Trk_numberOfPixelHoles, &b_Trk_numberOfPixelHoles);
   fChain->SetBranchAddress("Trk_numberOfSCTSharedHits", Trk_numberOfSCTSharedHits, &b_Trk_numberOfSCTSharedHits);
   fChain->SetBranchAddress("Trk_numberOfSCTHoles", Trk_numberOfSCTHoles, &b_Trk_numberOfSCTHoles);
   fChain->SetBranchAddress("Trk_numberOfTRTOutliers", Trk_numberOfTRTOutliers, &b_Trk_numberOfTRTOutliers);
   fChain->SetBranchAddress("Trk_numberOfTRTHighThresholdOutliers", Trk_numberOfTRTHighThresholdOutliers, &b_Trk_numberOfTRTHighThresholdOutliers);
   fChain->SetBranchAddress("Trk_numberOfMdtHits", Trk_numberOfMdtHits, &b_Trk_numberOfMdtHits);
   fChain->SetBranchAddress("Trk_numberOfTgcPhiHits", Trk_numberOfTgcPhiHits, &b_Trk_numberOfTgcPhiHits);
   fChain->SetBranchAddress("Trk_numberOfRpcPhiHits", Trk_numberOfRpcPhiHits, &b_Trk_numberOfRpcPhiHits);
   fChain->SetBranchAddress("Trk_numberOfCscPhiHits", Trk_numberOfCscPhiHits, &b_Trk_numberOfCscPhiHits);
   fChain->SetBranchAddress("Trk_numberOfTgcEtaHits", Trk_numberOfTgcEtaHits, &b_Trk_numberOfTgcEtaHits);
   fChain->SetBranchAddress("Trk_numberOfRpcEtaHits", Trk_numberOfRpcEtaHits, &b_Trk_numberOfRpcEtaHits);
   fChain->SetBranchAddress("Trk_numberOfCscEtaHits", Trk_numberOfCscEtaHits, &b_Trk_numberOfCscEtaHits);
   fChain->SetBranchAddress("Trk_numberOfGangedPixels", Trk_numberOfGangedPixels, &b_Trk_numberOfGangedPixels);
   fChain->SetBranchAddress("Trk_numberOfOutliersOnTrack", Trk_numberOfOutliersOnTrack, &b_Trk_numberOfOutliersOnTrack);
   fChain->SetBranchAddress("Trk_numberOfTrackSummaryTypes", Trk_numberOfTrackSummaryTypes, &b_Trk_numberOfTrackSummaryTypes);
   fChain->SetBranchAddress("Trk_truthBarcode", Trk_truthBarcode, &b_Trk_truthBarcode);
   fChain->SetBranchAddress("Trk_truthNt", Trk_truthNt, &b_Trk_truthNt);
   fChain->SetBranchAddress("Trk_ParticlePdg", Trk_ParticlePdg, &b_Trk_ParticlePdg);
   fChain->SetBranchAddress("Trk_ParentPdg", Trk_ParentPdg, &b_Trk_ParentPdg);
   fChain->SetBranchAddress("IIC_InDet_NImpact", &IIC_InDet_NImpact, &b_IIC_InDet_NImpact);
   fChain->SetBranchAddress("IIC_InDet_x0", IIC_InDet_x0, &b_IIC_InDet_x0);
   fChain->SetBranchAddress("IIC_InDet_y0", IIC_InDet_y0, &b_IIC_InDet_y0);
   fChain->SetBranchAddress("IIC_InDet_z0", IIC_InDet_z0, &b_IIC_InDet_z0);
   fChain->SetBranchAddress("IIC_InDet_eta0_local", IIC_InDet_eta0_local, &b_IIC_InDet_eta0_local);
   fChain->SetBranchAddress("IIC_InDet_phi0_local", IIC_InDet_phi0_local, &b_IIC_InDet_phi0_local);
   fChain->SetBranchAddress("IIC_InDet_trketa_at0", IIC_InDet_trketa_at0, &b_IIC_InDet_trketa_at0);
   fChain->SetBranchAddress("IIC_InDet_trkphi_at0", IIC_InDet_trkphi_at0, &b_IIC_InDet_trkphi_at0);
   fChain->SetBranchAddress("IIC_InDet_x1", IIC_InDet_x1, &b_IIC_InDet_x1);
   fChain->SetBranchAddress("IIC_InDet_y1", IIC_InDet_y1, &b_IIC_InDet_y1);
   fChain->SetBranchAddress("IIC_InDet_z1", IIC_InDet_z1, &b_IIC_InDet_z1);
   fChain->SetBranchAddress("IIC_InDet_eta1_local", IIC_InDet_eta1_local, &b_IIC_InDet_eta1_local);
   fChain->SetBranchAddress("IIC_InDet_phi1_local", IIC_InDet_phi1_local, &b_IIC_InDet_phi1_local);
   fChain->SetBranchAddress("IIC_InDet_trketa_at1", IIC_InDet_trketa_at1, &b_IIC_InDet_trketa_at1);
   fChain->SetBranchAddress("IIC_InDet_trkphi_at1", IIC_InDet_trkphi_at1, &b_IIC_InDet_trkphi_at1);
   fChain->SetBranchAddress("IIC_InDet_x2", IIC_InDet_x2, &b_IIC_InDet_x2);
   fChain->SetBranchAddress("IIC_InDet_y2", IIC_InDet_y2, &b_IIC_InDet_y2);
   fChain->SetBranchAddress("IIC_InDet_z2", IIC_InDet_z2, &b_IIC_InDet_z2);
   fChain->SetBranchAddress("IIC_InDet_eta2_local", IIC_InDet_eta2_local, &b_IIC_InDet_eta2_local);
   fChain->SetBranchAddress("IIC_InDet_phi2_local", IIC_InDet_phi2_local, &b_IIC_InDet_phi2_local);
   fChain->SetBranchAddress("IIC_InDet_trketa_at2", IIC_InDet_trketa_at2, &b_IIC_InDet_trketa_at2);
   fChain->SetBranchAddress("IIC_InDet_trkphi_at2", IIC_InDet_trkphi_at2, &b_IIC_InDet_trkphi_at2);
   fChain->SetBranchAddress("IIC_InDet_x3", IIC_InDet_x3, &b_IIC_InDet_x3);
   fChain->SetBranchAddress("IIC_InDet_y3", IIC_InDet_y3, &b_IIC_InDet_y3);
   fChain->SetBranchAddress("IIC_InDet_z3", IIC_InDet_z3, &b_IIC_InDet_z3);
   fChain->SetBranchAddress("IIC_InDet_eta3_local", IIC_InDet_eta3_local, &b_IIC_InDet_eta3_local);
   fChain->SetBranchAddress("IIC_InDet_phi3_local", IIC_InDet_phi3_local, &b_IIC_InDet_phi3_local);
   fChain->SetBranchAddress("IIC_InDet_trketa_at3", IIC_InDet_trketa_at3, &b_IIC_InDet_trketa_at3);
   fChain->SetBranchAddress("IIC_InDet_trkphi_at3", IIC_InDet_trkphi_at3, &b_IIC_InDet_trkphi_at3);
   fChain->SetBranchAddress("IIC_InDet_x_tile", IIC_InDet_x_tile, &b_IIC_InDet_x_tile);
   fChain->SetBranchAddress("IIC_InDet_y_tile", IIC_InDet_y_tile, &b_IIC_InDet_y_tile);
   fChain->SetBranchAddress("IIC_InDet_z_tile", IIC_InDet_z_tile, &b_IIC_InDet_z_tile);
   fChain->SetBranchAddress("IIC_InDet_eta_tile_local", IIC_InDet_eta_tile_local, &b_IIC_InDet_eta_tile_local);
   fChain->SetBranchAddress("IIC_InDet_phi_tile_local", IIC_InDet_phi_tile_local, &b_IIC_InDet_phi_tile_local);
   fChain->SetBranchAddress("IIC_InDet_trketa_attile", IIC_InDet_trketa_attile, &b_IIC_InDet_trketa_attile);
   fChain->SetBranchAddress("IIC_InDet_trkphi_attile", IIC_InDet_trkphi_attile, &b_IIC_InDet_trkphi_attile);
   Notify();
}

Bool_t TB_tree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TB_tree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TB_tree::Cut(Long64_t /*entry*/)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

void TB_tree::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L TB_tree.C
//      Root > TB_tree t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      fChain->GetEntry(jentry);
      // if (Cut(ientry) < 0) continue;
   }
}


int TB_tree::Loop(int evtToFind)
{
   if (fChain == 0) return -1;

   Long64_t nentries = fChain->GetEntriesFast();

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      fChain->GetEntry(jentry);

      if(Event == evtToFind)
        return int(jentry);
   }
   return -1;
}
