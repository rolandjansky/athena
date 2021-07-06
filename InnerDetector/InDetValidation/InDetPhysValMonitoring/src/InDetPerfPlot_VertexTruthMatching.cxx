/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_VertexTruthMatching.cxx
 * @author Valentina Cairo, mbasso
 **/

#include "InDetPerfPlot_VertexTruthMatching.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
// 3 new includes HRM
#include "InDetPerfPlot_Vertex.h"
#include "InDetPerfPlot_nTracks.h"
//#include "VxVertex/VxTrackAtVertex.h"

using namespace IDPVM;

InDetPerfPlot_VertexTruthMatching::InDetPerfPlot_VertexTruthMatching(InDetPlotBase* pParent, const std::string& sDir, const int iDetailLevel) :
    InDetPlotBase(pParent, sDir),
    m_iDetailLevel(iDetailLevel),
    m_vx_type_truth(nullptr),
    m_vx_hs_classification(nullptr),
    m_vx_nReco_vs_nTruth_inclusive(nullptr),
    m_vx_nReco_vs_nTruth_matched(nullptr),
    m_vx_nReco_vs_nTruth_merged(nullptr),
    m_vx_nReco_vs_nTruth_split(nullptr),
    m_vx_nReco_vs_nTruth_fake(nullptr),
    m_vx_nReco_vs_nTruth_dummy(nullptr),
    m_vx_nReco_vs_nTruth_clean(nullptr),
    m_vx_nReco_vs_nTruth_lowpu(nullptr),
    m_vx_nReco_vs_nTruth_highpu(nullptr),
    m_vx_nReco_vs_nTruth_hssplit(nullptr),
    m_vx_nReco_vs_nTruth_none(nullptr),
    m_vx_hs_reco_eff(nullptr),
    m_vx_hs_sel_eff(nullptr),
    m_vx_hs_reco_long_reso(nullptr),
    m_vx_hs_reco_trans_reso(nullptr),
    m_vx_hs_truth_long_reso_vs_PU(nullptr),
    m_vx_hs_truth_trans_reso_vs_PU(nullptr),
    m_vx_hs_truth_long_reso(nullptr),
    m_vx_hs_truth_trans_reso(nullptr),
    m_vx_hs_z_pull(nullptr),
    m_vx_hs_y_pull(nullptr),
    m_vx_hs_x_pull(nullptr),
    m_vx_all_z_pull(nullptr),
    m_vx_all_y_pull(nullptr),
    m_vx_all_x_pull(nullptr),
    m_vx_hs_z_res(nullptr),
    m_vx_hs_y_res(nullptr),
    m_vx_hs_x_res(nullptr),
    m_vx_all_z_res(nullptr),
    m_vx_all_y_res(nullptr),
    m_vx_all_x_res(nullptr),
    m_vx_all_truth_z_res_vs_PU(nullptr),
    m_vx_all_truth_x_res_vs_PU(nullptr),
    m_vx_all_truth_y_res_vs_PU(nullptr),
    m_vx_all_truth_z_pull_vs_PU(nullptr),
    m_vx_all_truth_x_pull_vs_PU(nullptr),
    m_vx_all_truth_y_pull_vs_PU(nullptr),
    m_vx_all_truth_z_res_vs_nTrk(nullptr),
    m_vx_all_truth_x_res_vs_nTrk(nullptr),
    m_vx_all_truth_y_res_vs_nTrk(nullptr),
    m_vx_all_truth_z_pull_vs_nTrk(nullptr),
    m_vx_all_truth_x_pull_vs_nTrk(nullptr),
    m_vx_all_truth_y_pull_vs_nTrk(nullptr),
    m_vx_hs_truth_z_res_vs_PU(nullptr),
    m_vx_hs_truth_x_res_vs_PU(nullptr),
    m_vx_hs_truth_y_res_vs_PU(nullptr),
    m_vx_hs_truth_z_pull_vs_PU(nullptr),
    m_vx_hs_truth_x_pull_vs_PU(nullptr),
    m_vx_hs_truth_y_pull_vs_PU(nullptr),
    m_vx_hs_truth_z_res_vs_nTrk(nullptr),
    m_vx_hs_truth_x_res_vs_nTrk(nullptr),
    m_vx_hs_truth_y_res_vs_nTrk(nullptr),
    m_vx_hs_truth_z_pull_vs_nTrk(nullptr),
    m_vx_hs_truth_x_pull_vs_nTrk(nullptr),
    m_vx_hs_truth_y_pull_vs_nTrk(nullptr),
   // New histos HRM Helen OTP
    m_vx_ntracks_vs_nmatchedvertices(nullptr),
    m_vx_ntracks_vs_nmergedvertices(nullptr),
    m_vx_ntracks_vs_nsplitvertices(nullptr),
    m_vx_ntracks_vs_nfakevertices(nullptr),
    m_vx_ntracks_vs_HS_nmatchedvertices(nullptr),
    m_vx_ntracks_vs_HS_nmergedvertices(nullptr),
    m_vx_ntracks_vs_HS_nsplitvertices(nullptr),
    m_vx_ntracks_vs_HS_nfakevertices(nullptr),
    m_vx_sumpT_matched(nullptr),
    m_vx_sumpT_merged(nullptr),
    m_vx_sumpT_split(nullptr),
    m_vx_sumpT_fake(nullptr),
    m_vx_sumpT_HS_matched(nullptr),
    m_vx_sumpT_HS_merged(nullptr),
    m_vx_sumpT_HS_split(nullptr),
    m_vx_sumpT_HS_fake(nullptr),
    //Helen OTP sum of charges
    m_vx_sumCharge_matched(nullptr),
    m_vx_sumCharge_merged(nullptr),
    m_vx_sumCharge_split(nullptr),
    m_vx_sumCharge_fake(nullptr),
    m_vx_sumCharge_HS_matched(nullptr),
    m_vx_sumCharge_HS_merged(nullptr),
    m_vx_sumCharge_HS_split(nullptr),
    m_vx_sumCharge_HS_fake(nullptr),
    m_vx_sumCharge_N_trk_HS_matched(nullptr),
    m_vx_sumCharge_N_trk_HS_merged(nullptr),
    m_vx_sumCharge_N_trk_HS_split(nullptr),
    m_vx_sumCharge_N_trk_HS_fake(nullptr),
    m_vx_sumCharge_N_trk_matched(nullptr),
    m_vx_sumCharge_N_trk_merged(nullptr),
    m_vx_sumCharge_N_trk_split(nullptr),
    m_vx_sumCharge_N_trk_fake(nullptr),
    m_vx_z_asym_matched(nullptr),
    m_vx_z_asym_merged(nullptr),
    m_vx_z_asym_split(nullptr),
    m_vx_z_asym_fake(nullptr),
    m_vx_z_asym_HS_matched(nullptr),
    m_vx_z_asym_HS_merged(nullptr),
    m_vx_z_asym_HS_split(nullptr),
    m_vx_z_asym_HS_fake(nullptr),

    m_vx_track_weight_matched(nullptr),
    m_vx_track_weight_merged(nullptr),
    m_vx_track_weight_split(nullptr),
    m_vx_track_weight_fake(nullptr),
    m_vx_track_weight_HS_matched(nullptr),
    m_vx_track_weight_HS_merged(nullptr),
    m_vx_track_weight_HS_split(nullptr),
    m_vx_track_weight_HS_fake(nullptr),
    
    m_vx_normalised_track_weight_matched(nullptr),
    m_vx_normalised_track_weight_merged(nullptr),
    m_vx_normalised_track_weight_split(nullptr),
    m_vx_normalised_track_weight_fake(nullptr),
    m_vx_normalised_track_weight_HS_matched(nullptr),
    m_vx_normalised_track_weight_HS_merged(nullptr),
    m_vx_normalised_track_weight_HS_split(nullptr),
    m_vx_normalised_track_weight_HS_fake(nullptr),

    m_vx_d_asym_matched(nullptr),
    m_vx_d_asym_merged(nullptr),
    m_vx_d_asym_split(nullptr),
    m_vx_d_asym_fake(nullptr),
    m_vx_d_asym_HS_matched(nullptr),
    m_vx_d_asym_HS_merged(nullptr),
    m_vx_d_asym_HS_split(nullptr),
    m_vx_d_asym_HS_fake(nullptr),
    
    m_vx_chi2Over_ndf_matched(nullptr),
    m_vx_chi2Over_ndf_merged(nullptr),
    m_vx_chi2Over_ndf_split(nullptr),
    m_vx_chi2Over_ndf_fake(nullptr),
    m_vx_chi2Over_ndf_HS_matched(nullptr),
    m_vx_chi2Over_ndf_HS_merged(nullptr),
    m_vx_chi2Over_ndf_HS_split(nullptr),
    m_vx_chi2Over_ndf_HS_fake(nullptr),

    m_vx_z0_skewness_matched(nullptr),
    m_vx_z0_skewness_merged(nullptr),
    m_vx_z0_skewness_split(nullptr),
    m_vx_z0_skewness_fake(nullptr),
    m_vx_z0_skewness_HS_matched(nullptr),
    m_vx_z0_skewness_HS_merged(nullptr),
    m_vx_z0_skewness_HS_split(nullptr),
    m_vx_z0_skewness_HS_fake(nullptr),

    m_vx_z0_kurtosis_matched(nullptr),
    m_vx_z0_kurtosis_merged(nullptr),
    m_vx_z0_kurtosis_split(nullptr),
    m_vx_z0_kurtosis_fake(nullptr),
    m_vx_z0_kurtosis_HS_matched(nullptr),
    m_vx_z0_kurtosis_HS_merged(nullptr),
    m_vx_z0_kurtosis_HS_split(nullptr),
    m_vx_z0_kurtosis_HS_fake(nullptr),

    m_vx_d0_skewness_matched(nullptr),
    m_vx_d0_skewness_merged(nullptr),
    m_vx_d0_skewness_split(nullptr),
    m_vx_d0_skewness_fake(nullptr),
    m_vx_d0_skewness_HS_matched(nullptr),
    m_vx_d0_skewness_HS_merged(nullptr),
    m_vx_d0_skewness_HS_split(nullptr),
    m_vx_d0_skewness_HS_fake(nullptr),
    m_vx_d0_kurtosis_matched(nullptr),
    m_vx_d0_kurtosis_merged(nullptr),
    m_vx_d0_kurtosis_split(nullptr),
    m_vx_d0_kurtosis_fake(nullptr),
    m_vx_d0_kurtosis_HS_matched(nullptr),
    m_vx_d0_kurtosis_HS_merged(nullptr),
    m_vx_d0_kurtosis_HS_split(nullptr),
    m_vx_d0_kurtosis_HS_fake(nullptr),
    m_vx_pt_skewness_matched(nullptr),
    m_vx_pt_skewness_merged(nullptr),
    m_vx_pt_skewness_split(nullptr),
    m_vx_pt_skewness_fake(nullptr),
    m_vx_pt_skewness_HS_matched(nullptr),
    m_vx_pt_skewness_HS_merged(nullptr),
    m_vx_pt_skewness_HS_split(nullptr),
    m_vx_pt_skewness_HS_fake(nullptr),
    m_vx_pt_kurtosis_matched(nullptr),
    m_vx_pt_kurtosis_merged(nullptr),
    m_vx_pt_kurtosis_split(nullptr),
    m_vx_pt_kurtosis_fake(nullptr),
    m_vx_pt_kurtosis_HS_matched(nullptr),
    m_vx_pt_kurtosis_HS_merged(nullptr),
    m_vx_pt_kurtosis_HS_split(nullptr),
    m_vx_pt_kurtosis_HS_fake(nullptr),

    m_vx_sumpT_ALL_matched(nullptr),
    m_vx_sumpT_ALL_merged(nullptr),
    m_vx_sumpT_ALL_split(nullptr),
    m_vx_sumpT_ALL_fake(nullptr),
    m_vx_sumCharge_ALL_matched(nullptr),
    m_vx_sumCharge_ALL_merged(nullptr),
    m_vx_sumCharge_ALL_split(nullptr),
    m_vx_sumCharge_ALL_fake(nullptr),
    m_vx_sumCharge_N_trk_ALL_matched(nullptr),
    m_vx_sumCharge_N_trk_ALL_merged(nullptr),
    m_vx_sumCharge_N_trk_ALL_split(nullptr),
    m_vx_sumCharge_N_trk_ALL_fake(nullptr),
    m_vx_z_asym_ALL_matched(nullptr),
    m_vx_z_asym_ALL_merged(nullptr),
    m_vx_z_asym_ALL_split(nullptr),
    m_vx_z_asym_ALL_fake(nullptr),
    m_vx_d_asym_ALL_matched(nullptr),
    m_vx_d_asym_ALL_merged(nullptr),
    m_vx_d_asym_ALL_split(nullptr),
    m_vx_d_asym_ALL_fake(nullptr),
    
    m_vx_track_weight_ALL_matched(nullptr),
    m_vx_track_weight_ALL_merged(nullptr),
    m_vx_track_weight_ALL_split(nullptr),
    m_vx_track_weight_ALL_fake(nullptr),

    m_vx_normalised_track_weight_ALL_matched(nullptr),
    m_vx_normalised_track_weight_ALL_merged(nullptr),
    m_vx_normalised_track_weight_ALL_split(nullptr),
    m_vx_normalised_track_weight_ALL_fake(nullptr),

    m_vx_chi2Over_ndf_ALL_matched(nullptr),
    m_vx_chi2Over_ndf_ALL_merged(nullptr),
    m_vx_chi2Over_ndf_ALL_split(nullptr),
    m_vx_chi2Over_ndf_ALL_fake(nullptr),

    m_vx_z0_skewness_ALL_matched(nullptr),
    m_vx_z0_skewness_ALL_merged(nullptr),
    m_vx_z0_skewness_ALL_split(nullptr),
    m_vx_z0_skewness_ALL_fake(nullptr),

    m_vx_z0_kurtosis_ALL_matched(nullptr),
    m_vx_z0_kurtosis_ALL_merged(nullptr),
    m_vx_z0_kurtosis_ALL_split(nullptr),
    m_vx_z0_kurtosis_ALL_fake(nullptr),

    m_vx_d0_skewness_ALL_matched(nullptr),
    m_vx_d0_skewness_ALL_merged(nullptr),
    m_vx_d0_skewness_ALL_split(nullptr),
    m_vx_d0_skewness_ALL_fake(nullptr),
    m_vx_d0_kurtosis_ALL_matched(nullptr),
    m_vx_d0_kurtosis_ALL_merged(nullptr),
    m_vx_d0_kurtosis_ALL_split(nullptr),
    m_vx_d0_kurtosis_ALL_fake(nullptr),
    m_vx_pt_skewness_ALL_matched(nullptr),
    m_vx_pt_skewness_ALL_merged(nullptr),
    m_vx_pt_skewness_ALL_split(nullptr),
    m_vx_pt_skewness_ALL_fake(nullptr),
    m_vx_pt_kurtosis_ALL_matched(nullptr),
    m_vx_pt_kurtosis_ALL_merged(nullptr),
    m_vx_pt_kurtosis_ALL_split(nullptr),
    m_vx_pt_kurtosis_ALL_fake(nullptr),
    m_vx_nVertices_ALL_matched(nullptr),
    m_vx_nVertices_ALL_merged(nullptr),
    m_vx_nVertices_ALL_split(nullptr),
    m_vx_nVertices_HS_matched(nullptr),
    m_vx_nVertices_HS_merged(nullptr),
    m_vx_nVertices_HS_split(nullptr),
    m_vx_nVertices_matched(nullptr),
    m_vx_nVertices_merged(nullptr),
    m_vx_nVertices_split(nullptr),

    m_vx_track_z0_ALL_matched(nullptr),
    m_vx_track_z0_ALL_merged(nullptr),
    m_vx_track_z0_ALL_split(nullptr),
    m_vx_track_z0_HS_matched(nullptr),
    m_vx_track_z0_HS_merged(nullptr),
    m_vx_track_z0_HS_split(nullptr),
    m_vx_track_z0_matched(nullptr),
    m_vx_track_z0_merged(nullptr),
    m_vx_track_z0_split(nullptr),

    m_vx_track_d0_ALL_matched(nullptr),
    m_vx_track_d0_ALL_merged(nullptr),
    m_vx_track_d0_ALL_split(nullptr),
    m_vx_track_d0_HS_matched(nullptr),
    m_vx_track_d0_HS_merged(nullptr),
    m_vx_track_d0_HS_split(nullptr),
    m_vx_track_d0_matched(nullptr),
    m_vx_track_d0_merged(nullptr),
    m_vx_track_d0_split(nullptr),

    m_vx_track_z0_err_ALL_matched(nullptr),
    m_vx_track_z0_err_ALL_merged(nullptr),
    m_vx_track_z0_err_ALL_split(nullptr),
    m_vx_track_z0_err_HS_matched(nullptr),
    m_vx_track_z0_err_HS_merged(nullptr),
    m_vx_track_z0_err_HS_split(nullptr),
    m_vx_track_z0_err_matched(nullptr),
    m_vx_track_z0_err_merged(nullptr),
    m_vx_track_z0_err_split(nullptr),

    m_vx_track_d0_err_ALL_matched(nullptr),
    m_vx_track_d0_err_ALL_merged(nullptr),
    m_vx_track_d0_err_ALL_split(nullptr),
    m_vx_track_d0_err_HS_matched(nullptr),
    m_vx_track_d0_err_HS_merged(nullptr),
    m_vx_track_d0_err_HS_split(nullptr),
    m_vx_track_d0_err_matched(nullptr),
    m_vx_track_d0_err_merged(nullptr),
    m_vx_track_d0_err_split(nullptr),

    m_vx_track_z0_ALL_matched_vs_track_pt(nullptr),
    m_vx_track_z0_ALL_merged_vs_track_pt(nullptr),
    m_vx_track_z0_ALL_split_vs_track_pt(nullptr),
    m_vx_track_z0_HS_matched_vs_track_pt(nullptr),
    m_vx_track_z0_HS_merged_vs_track_pt(nullptr),
    m_vx_track_z0_HS_split_vs_track_pt(nullptr),
    m_vx_track_z0_matched_vs_track_pt(nullptr),
    m_vx_track_z0_merged_vs_track_pt(nullptr),
    m_vx_track_z0_split_vs_track_pt(nullptr),

    m_vx_track_d0_ALL_matched_vs_track_pt(nullptr),
    m_vx_track_d0_ALL_merged_vs_track_pt(nullptr),
    m_vx_track_d0_ALL_split_vs_track_pt(nullptr),
    m_vx_track_d0_HS_matched_vs_track_pt(nullptr),
    m_vx_track_d0_HS_merged_vs_track_pt(nullptr),
    m_vx_track_d0_HS_split_vs_track_pt(nullptr),
    m_vx_track_d0_matched_vs_track_pt(nullptr),
    m_vx_track_d0_merged_vs_track_pt(nullptr),
    m_vx_track_d0_split_vs_track_pt(nullptr),

    m_vx_track_z0_err_ALL_matched_vs_track_pt(nullptr),
    m_vx_track_z0_err_ALL_merged_vs_track_pt(nullptr),
    m_vx_track_z0_err_ALL_split_vs_track_pt(nullptr),
    m_vx_track_z0_err_HS_matched_vs_track_pt(nullptr),
    m_vx_track_z0_err_HS_merged_vs_track_pt(nullptr),
    m_vx_track_z0_err_HS_split_vs_track_pt(nullptr),
    m_vx_track_z0_err_matched_vs_track_pt(nullptr),
    m_vx_track_z0_err_merged_vs_track_pt(nullptr),
    m_vx_track_z0_err_split_vs_track_pt(nullptr),

    m_vx_track_d0_err_ALL_matched_vs_track_pt(nullptr),
    m_vx_track_d0_err_ALL_merged_vs_track_pt(nullptr),
    m_vx_track_d0_err_ALL_split_vs_track_pt(nullptr),
    m_vx_track_d0_err_HS_matched_vs_track_pt(nullptr),
    m_vx_track_d0_err_HS_merged_vs_track_pt(nullptr),
    m_vx_track_d0_err_HS_split_vs_track_pt(nullptr),
    m_vx_track_d0_err_matched_vs_track_pt(nullptr),
    m_vx_track_d0_err_merged_vs_track_pt(nullptr),
    m_vx_track_d0_err_split_vs_track_pt(nullptr)






{
  // nop
}

void InDetPerfPlot_VertexTruthMatching::initializePlots() {

    book(m_vx_type_truth,"vx_type_truth");
    if (m_iDetailLevel >= 200) {
        book(m_vx_hs_classification,"vx_hs_classification");
        book(m_vx_nReco_vs_nTruth_inclusive,"vx_nReco_vs_nTruth_inclusive");
        book(m_vx_nReco_vs_nTruth_matched,"vx_nReco_vs_nTruth_matched");
        book(m_vx_nReco_vs_nTruth_merged,"vx_nReco_vs_nTruth_merged");
        book(m_vx_nReco_vs_nTruth_split,"vx_nReco_vs_nTruth_split");
        book(m_vx_nReco_vs_nTruth_fake,"vx_nReco_vs_nTruth_fake");
        book(m_vx_nReco_vs_nTruth_dummy,"vx_nReco_vs_nTruth_dummy");
        book(m_vx_nReco_vs_nTruth_clean,"vx_nReco_vs_nTruth_clean");
        book(m_vx_nReco_vs_nTruth_lowpu,"vx_nReco_vs_nTruth_lowpu");
        book(m_vx_nReco_vs_nTruth_highpu,"vx_nReco_vs_nTruth_highpu");
        book(m_vx_nReco_vs_nTruth_hssplit,"vx_nReco_vs_nTruth_hssplit");
        book(m_vx_nReco_vs_nTruth_none,"vx_nReco_vs_nTruth_none");
        book(m_vx_hs_reco_eff,"vx_hs_reco_eff");
        book(m_vx_hs_sel_eff,"vx_hs_sel_eff");
        book(m_vx_hs_reco_long_reso,"vx_hs_reco_long_reso");
        book(m_vx_hs_reco_trans_reso,"vx_hs_reco_trans_reso");
        book(m_vx_hs_truth_long_reso,"vx_hs_truth_long_reso");
        book(m_vx_hs_truth_trans_reso,"vx_hs_truth_trans_reso");
        book(m_vx_hs_truth_long_reso_vs_PU,"vx_hs_truth_long_reso_vs_PU");
        book(m_vx_hs_truth_trans_reso_vs_PU,"vx_hs_truth_trans_reso_vs_PU");
 
        book(m_vx_hs_z_pull,"vx_TYPE_z_pull","vx_hs_z_pull");
        book(m_vx_hs_y_pull,"vx_TYPE_y_pull","vx_hs_y_pull");
        book(m_vx_hs_x_pull,"vx_TYPE_x_pull","vx_hs_x_pull");
   
        book(m_vx_all_z_pull,"vx_TYPE_z_pull","vx_all_z_pull");
        book(m_vx_all_y_pull,"vx_TYPE_y_pull","vx_all_y_pull");
        book(m_vx_all_x_pull,"vx_TYPE_x_pull","vx_all_x_pull");

        book(m_vx_hs_z_res,"vx_TYPE_z_reso","vx_hs_z_res");
        book(m_vx_hs_y_res,"vx_TYPE_y_reso","vx_hs_y_res");
        book(m_vx_hs_x_res,"vx_TYPE_x_reso","vx_hs_x_res");
        book(m_vx_all_z_res,"vx_TYPE_z_reso","vx_all_z_res");
        book(m_vx_all_y_res,"vx_TYPE_y_reso","vx_all_y_res");
        book(m_vx_all_x_res,"vx_TYPE_x_reso","vx_all_x_res");

        book(m_vx_all_truth_z_res_vs_PU, "vx_TYPE_truth_reso_z_vs_PU", "vx_all_truth_reso_z_vs_PU");
        book(m_vx_all_truth_x_res_vs_PU, "vx_TYPE_truth_reso_x_vs_PU", "vx_all_truth_reso_x_vs_PU");
        book(m_vx_all_truth_y_res_vs_PU, "vx_TYPE_truth_reso_y_vs_PU", "vx_all_truth_reso_y_vs_PU");
        book(m_vx_all_truth_z_res_vs_nTrk, "vx_TYPE_truth_reso_z_vs_nTrk", "vx_all_truth_reso_z_vs_nTrk");
        book(m_vx_all_truth_x_res_vs_nTrk, "vx_TYPE_truth_reso_x_vs_nTrk", "vx_all_truth_reso_x_vs_nTrk");
        book(m_vx_all_truth_y_res_vs_nTrk, "vx_TYPE_truth_reso_y_vs_nTrk", "vx_all_truth_reso_y_vs_nTrk");

        book(m_vx_all_truth_z_pull_vs_PU, "vx_TYPE_truth_pull_z_vs_PU", "vx_all_truth_pull_z_vs_PU");
        book(m_vx_all_truth_x_pull_vs_PU, "vx_TYPE_truth_pull_x_vs_PU", "vx_all_truth_pull_x_vs_PU");
        book(m_vx_all_truth_y_pull_vs_PU, "vx_TYPE_truth_pull_y_vs_PU", "vx_all_truth_pull_y_vs_PU");
        book(m_vx_all_truth_z_pull_vs_nTrk, "vx_TYPE_truth_pull_z_vs_nTrk", "vx_all_truth_pull_z_vs_nTrk");
        book(m_vx_all_truth_x_pull_vs_nTrk, "vx_TYPE_truth_pull_x_vs_nTrk", "vx_all_truth_pull_x_vs_nTrk");
        book(m_vx_all_truth_y_pull_vs_nTrk, "vx_TYPE_truth_pull_y_vs_nTrk", "vx_all_truth_pull_y_vs_nTrk");

        book(m_vx_hs_truth_z_res_vs_PU, "vx_TYPE_truth_reso_z_vs_PU", "vx_hs_truth_reso_z_vs_PU");
        book(m_vx_hs_truth_x_res_vs_PU, "vx_TYPE_truth_reso_x_vs_PU", "vx_hs_truth_reso_x_vs_PU");
        book(m_vx_hs_truth_y_res_vs_PU, "vx_TYPE_truth_reso_y_vs_PU", "vx_hs_truth_reso_y_vs_PU");
        book(m_vx_hs_truth_z_res_vs_nTrk, "vx_TYPE_truth_reso_z_vs_nTrk", "vx_hs_truth_reso_z_vs_nTrk");
        book(m_vx_hs_truth_x_res_vs_nTrk, "vx_TYPE_truth_reso_x_vs_nTrk", "vx_hs_truth_reso_x_vs_nTrk");
        book(m_vx_hs_truth_y_res_vs_nTrk, "vx_TYPE_truth_reso_y_vs_nTrk", "vx_hs_truth_reso_y_vs_nTrk");

        book(m_vx_hs_truth_z_pull_vs_PU, "vx_TYPE_truth_pull_z_vs_PU", "vx_hs_truth_pull_z_vs_PU");
        book(m_vx_hs_truth_x_pull_vs_PU, "vx_TYPE_truth_pull_x_vs_PU", "vx_hs_truth_pull_x_vs_PU");
        book(m_vx_hs_truth_y_pull_vs_PU, "vx_TYPE_truth_pull_y_vs_PU", "vx_hs_truth_pull_y_vs_PU");
        book(m_vx_hs_truth_z_pull_vs_nTrk, "vx_TYPE_truth_pull_z_vs_nTrk", "vx_hs_truth_pull_z_vs_nTrk");
        book(m_vx_hs_truth_x_pull_vs_nTrk, "vx_TYPE_truth_pull_x_vs_nTrk", "vx_hs_truth_pull_x_vs_nTrk");
        book(m_vx_hs_truth_y_pull_vs_nTrk, "vx_TYPE_truth_pull_y_vs_nTrk", "vx_hs_truth_pull_y_vs_nTrk");

	// book the new histos  Helen OTP
	book(m_vx_ntracks_vs_nmatchedvertices,"vx_ntracks_vs_nmatchedvertices");
    	book(m_vx_ntracks_vs_nmergedvertices,"vx_ntracks_vs_nmergedvertices");
    	book(m_vx_ntracks_vs_nsplitvertices,"vx_ntracks_vs_nsplitvertices");
    	book(m_vx_ntracks_vs_nfakevertices,"vx_ntracks_vs_nfakevertices");
	book(m_vx_ntracks_vs_HS_nmatchedvertices,"vx_ntracks_vs_HS_nmatchedvertices");
    	book(m_vx_ntracks_vs_HS_nmergedvertices,"vx_ntracks_vs_HS_nmergedvertices");
    	book(m_vx_ntracks_vs_HS_nsplitvertices,"vx_ntracks_vs_HS_nsplitvertices");
    	book(m_vx_ntracks_vs_HS_nfakevertices,"vx_ntracks_vs_HS_nfakevertices");
        book(m_vx_sumpT_matched,"vx_sumpT_matched");
        book(m_vx_sumpT_merged,"vx_sumpT_merged");
        book(m_vx_sumpT_split,"vx_sumpT_split");
        book( m_vx_sumpT_fake,"vx_sumpT_fake");
        book(m_vx_sumpT_HS_matched,"vx_sumpT_HS_matched");
        book(m_vx_sumpT_HS_merged,"vx_sumpT_HS_merged");
        book(m_vx_sumpT_HS_split,"vx_sumpT_HS_split");
        book( m_vx_sumpT_HS_fake,"vx_sumpT_HS_fake");
        // Helen OTP sum of charges
        book(m_vx_sumCharge_matched,"vx_sumCharge_matched");
        book(m_vx_sumCharge_merged,"vx_sumCharge_merged");
        book(m_vx_sumCharge_split,"vx_sumCharge_split");
        book( m_vx_sumCharge_fake,"vx_sumCharge_fake");
        book(m_vx_sumCharge_HS_matched,"vx_sumCharge_HS_matched");
        book(m_vx_sumCharge_HS_merged,"vx_sumCharge_HS_merged");
        book(m_vx_sumCharge_HS_split,"vx_sumCharge_HS_split");
        book(m_vx_sumCharge_HS_fake,"vx_sumCharge_HS_fake");
        book(m_vx_sumCharge_N_trk_HS_matched, "vx_sumCharge_N_trk_HS_matched");
        book(m_vx_sumCharge_N_trk_HS_merged, "vx_sumCharge_N_trk_HS_merged");
        book(m_vx_sumCharge_N_trk_HS_split, "vx_sumCharge_N_trk_HS_split");
        book(m_vx_sumCharge_N_trk_HS_fake, "vx_sumCharge_N_trk_HS_fake");
        book(m_vx_sumCharge_N_trk_matched, "vx_sumCharge_N_trk_matched");
        book(m_vx_sumCharge_N_trk_merged, "vx_sumCharge_N_trk_merged");
        book(m_vx_sumCharge_N_trk_split, "vx_sumCharge_N_trk_split");
        book(m_vx_sumCharge_N_trk_fake, "vx_sumCharge_N_trk_fake");
        book(m_vx_z_asym_matched,"vx_z_asym_matched");
        book(m_vx_z_asym_merged,"vx_z_asym_merged");
        book(m_vx_z_asym_split,"vx_z_asym_split");
        book(m_vx_z_asym_fake,"vx_z_asym_fake");
        book(m_vx_z_asym_HS_matched,"vx_z_asym_HS_matched");
        book(m_vx_z_asym_HS_merged,"vx_z_asym_HS_merged");
        book(m_vx_z_asym_HS_split,"vx_z_asym_HS_split");
        book(m_vx_z_asym_HS_fake,"vx_z_asym_HS_fake");
        book(m_vx_track_weight_matched, "vx_track_weight_matched");
        book(m_vx_track_weight_merged, "vx_track_weight_merged");
        book(m_vx_track_weight_split, "vx_track_weight_split");
        book(m_vx_track_weight_fake, "vx_track_weight_fake");
        book(m_vx_track_weight_HS_matched, "vx_track_weight_HS_matched");
        book(m_vx_track_weight_HS_merged, "vx_track_weight_HS_merged");
        book(m_vx_track_weight_HS_split, "vx_track_weight_HS_split");
        book(m_vx_track_weight_HS_fake, "vx_track_weight_HS_fake");


        book(m_vx_normalised_track_weight_matched, "vx_normalised_track_weight_matched");
        book(m_vx_normalised_track_weight_merged, "vx_normalised_track_weight_merged");
        book(m_vx_normalised_track_weight_split, "vx_normalised_track_weight_split");
        book(m_vx_normalised_track_weight_fake, "vx_normalised_track_weight_fake");
        book(m_vx_normalised_track_weight_HS_matched, "vx_normalised_track_weight_HS_matched");
        book(m_vx_normalised_track_weight_HS_merged, "vx_normalised_track_weight_HS_merged");
        book(m_vx_normalised_track_weight_HS_split, "vx_normalised_track_weight_HS_split");
        book(m_vx_normalised_track_weight_HS_fake, "vx_normalised_track_weight_HS_fake");

        book(m_vx_d_asym_matched,"vx_d_asym_matched");
        book(m_vx_d_asym_merged,"vx_d_asym_merged");
        book(m_vx_d_asym_split,"vx_d_asym_split");
        book(m_vx_d_asym_fake,"vx_d_asym_fake");
        book(m_vx_d_asym_HS_matched,"vx_d_asym_HS_matched");
        book(m_vx_d_asym_HS_merged,"vx_d_asym_HS_merged");
        book(m_vx_d_asym_HS_split,"vx_d_asym_HS_split");
        book(m_vx_d_asym_HS_fake,"vx_d_asym_HS_fake");
 
        book(m_vx_chi2Over_ndf_matched,"vx_chi2Over_ndf_matched");
        book(m_vx_chi2Over_ndf_merged,"vx_chi2Over_ndf_merged");
        book(m_vx_chi2Over_ndf_split,"vx_chi2Over_ndf_split");
        book(m_vx_chi2Over_ndf_fake,"vx_chi2Over_ndf_fake");
        book(m_vx_chi2Over_ndf_HS_matched,"vx_chi2Over_ndf_HS_matched");
        book(m_vx_chi2Over_ndf_HS_merged,"vx_chi2Over_ndf_HS_merged");
        book(m_vx_chi2Over_ndf_HS_split,"vx_chi2Over_ndf_HS_split");
        book(m_vx_chi2Over_ndf_HS_fake,"vx_chi2Over_ndf_HS_fake");
        
        book(m_vx_z0_skewness_matched, "vx_z0_skewness_matched");
        book(m_vx_z0_skewness_merged, "vx_z0_skewness_merged");
        book(m_vx_z0_skewness_split, "vx_z0_skewness_split");
        book(m_vx_z0_skewness_fake, "vx_z0_skewness_fake");
        book(m_vx_z0_skewness_HS_matched, "vx_z0_skewness_HS_matched");
        book(m_vx_z0_skewness_HS_merged, "vx_z0_skewness_HS_merged");
        book(m_vx_z0_skewness_HS_split, "vx_z0_skewness_HS_split");
        book(m_vx_z0_skewness_HS_fake, "vx_z0_skewness_HS_fake");
        book(m_vx_z0_kurtosis_matched,"vx_z0_kurtosis_matched");
        book(m_vx_z0_kurtosis_merged,"vx_z0_kurtosis_merged");
        book(m_vx_z0_kurtosis_split,"vx_z0_kurtosis_split");
        book(m_vx_z0_kurtosis_fake,"vx_z0_kurtosis_fake");
        book(m_vx_z0_kurtosis_HS_matched,"vx_z0_kurtosis_HS_matched");
        book(m_vx_z0_kurtosis_HS_merged,"vx_z0_kurtosis_HS_merged");
        book(m_vx_z0_kurtosis_HS_split,"vx_z0_kurtosis_HS_split");
        book(m_vx_z0_kurtosis_HS_fake,"vx_z0_kurtosis_HS_fake");

        book(m_vx_d0_skewness_matched,"vx_d0_skewness_matched");
        book(m_vx_d0_skewness_merged,"vx_d0_skewness_merged");
        book(m_vx_d0_skewness_split,"vx_d0_skewness_split");
        book(m_vx_d0_skewness_fake,"vx_d0_skewness_fake");
        book(m_vx_d0_skewness_HS_matched,"vx_d0_skewness_HS_matched");
        book(m_vx_d0_skewness_HS_merged,"vx_d0_skewness_HS_merged");
        book(m_vx_d0_skewness_HS_split,"vx_d0_skewness_HS_split");
        book(m_vx_d0_skewness_HS_fake,"vx_d0_skewness_HS_fake");
        book(m_vx_d0_kurtosis_matched,"vx_d0_kurtosis_matched");
        book(m_vx_d0_kurtosis_merged,"vx_d0_kurtosis_merged");
        book(m_vx_d0_kurtosis_split,"vx_d0_kurtosis_split");
        book(m_vx_d0_kurtosis_fake,"vx_d0_kurtosis_fake");
        book(m_vx_d0_kurtosis_HS_matched,"vx_d0_kurtosis_HS_matched");
        book(m_vx_d0_kurtosis_HS_merged,"vx_d0_kurtosis_HS_merged");
        book(m_vx_d0_kurtosis_HS_split,"vx_d0_kurtosis_HS_split");
        book(m_vx_d0_kurtosis_HS_fake,"vx_d0_kurtosis_HS_fake");

        
        book(m_vx_pt_skewness_matched,"vx_pt_skewness_matched");
        book(m_vx_pt_skewness_merged,"vx_pt_skewness_merged");
        book(m_vx_pt_skewness_split,"vx_pt_skewness_split");
        book(m_vx_pt_skewness_fake,"vx_pt_skewness_fake");
        book(m_vx_pt_skewness_HS_matched,"vx_pt_skewness_HS_matched");
        book(m_vx_pt_skewness_HS_merged,"vx_pt_skewness_HS_merged");
        book(m_vx_pt_skewness_HS_split,"vx_pt_skewness_HS_split");
        book(m_vx_pt_skewness_HS_fake,"vx_pt_skewness_HS_fake");
        book(m_vx_pt_kurtosis_matched,"vx_pt_kurtosis_matched");
        book(m_vx_pt_kurtosis_merged,"vx_pt_kurtosis_merged");
        book(m_vx_pt_kurtosis_split,"vx_pt_kurtosis_split");
        book(m_vx_pt_kurtosis_fake,"vx_pt_kurtosis_fake");
        book(m_vx_pt_kurtosis_HS_matched,"vx_pt_kurtosis_HS_matched");
        book(m_vx_pt_kurtosis_HS_merged,"vx_pt_kurtosis_HS_merged");
        book(m_vx_pt_kurtosis_HS_split,"vx_pt_kurtosis_HS_split");
        book(m_vx_pt_kurtosis_HS_fake,"vx_pt_kurtosis_HS_fake");
        // Histograms with all vertices
        book(m_vx_sumpT_ALL_matched,"vx_sumpT_ALL_matched");
        book(m_vx_sumpT_ALL_merged,"vx_sumpT_ALL_merged");
        book(m_vx_sumpT_ALL_split,"vx_sumpT_ALL_split");
        book( m_vx_sumpT_ALL_fake,"vx_sumpT_ALL_fake");
        
        book(m_vx_sumCharge_ALL_matched,"vx_sumCharge_ALL_matched");
        book(m_vx_sumCharge_ALL_merged,"vx_sumCharge_ALL_merged");
        book(m_vx_sumCharge_ALL_split,"vx_sumCharge_ALL_split");
        book(m_vx_sumCharge_ALL_fake,"vx_sumCharge_ALL_fake");
        book(m_vx_sumCharge_N_trk_ALL_matched, "vx_sumCharge_N_trk_ALL_matched");
        book(m_vx_sumCharge_N_trk_ALL_merged, "vx_sumCharge_N_trk_ALL_merged");
        book(m_vx_sumCharge_N_trk_ALL_split, "vx_sumCharge_N_trk_ALL_split");
        book(m_vx_sumCharge_N_trk_ALL_fake, "vx_sumCharge_N_trk_ALL_fake");
 
        book(m_vx_z_asym_ALL_matched,"vx_z_asym_ALL_matched");
        book(m_vx_z_asym_ALL_merged,"vx_z_asym_ALL_merged");
        book(m_vx_z_asym_ALL_split,"vx_z_asym_ALL_split");
        book(m_vx_z_asym_ALL_fake,"vx_z_asym_ALL_fake");
        
        book(m_vx_track_weight_ALL_matched, "vx_track_weight_ALL_matched");
        book(m_vx_track_weight_ALL_merged, "vx_track_weight_ALL_merged");
        book(m_vx_track_weight_ALL_split, "vx_track_weight_ALL_split");
        book(m_vx_track_weight_ALL_fake, "vx_track_weight_ALL_fake");
        
        book(m_vx_normalised_track_weight_ALL_matched, "vx_normalised_track_weight_ALL_matched");
        book(m_vx_normalised_track_weight_ALL_merged, "vx_normalised_track_weight_ALL_merged");
        book(m_vx_normalised_track_weight_ALL_split, "vx_normalised_track_weight_ALL_split");
        book(m_vx_normalised_track_weight_ALL_fake, "vx_normalised_track_weight_ALL_fake");
        
        book(m_vx_d_asym_ALL_matched,"vx_d_asym_ALL_matched");
        book(m_vx_d_asym_ALL_merged,"vx_d_asym_ALL_merged");
        book(m_vx_d_asym_ALL_split,"vx_d_asym_ALL_split");
        book(m_vx_d_asym_ALL_fake,"vx_d_asym_ALL_fake");
        book(m_vx_chi2Over_ndf_ALL_matched,"vx_chi2Over_ndf_ALL_matched");
        book(m_vx_chi2Over_ndf_ALL_merged,"vx_chi2Over_ndf_ALL_merged");
        book(m_vx_chi2Over_ndf_ALL_split,"vx_chi2Over_ndf_ALL_split");
        book(m_vx_chi2Over_ndf_ALL_fake,"vx_chi2Over_ndf_ALL_fake");
        
        book(m_vx_z0_skewness_ALL_matched, "vx_z0_skewness_ALL_matched");
        book(m_vx_z0_skewness_ALL_merged, "vx_z0_skewness_ALL_merged");
        book(m_vx_z0_skewness_ALL_split, "vx_z0_skewness_ALL_split");
        book(m_vx_z0_skewness_ALL_fake, "vx_z0_skewness_ALL_fake");
        book(m_vx_z0_kurtosis_ALL_matched,"vx_z0_kurtosis_ALL_matched");
        book(m_vx_z0_kurtosis_ALL_merged,"vx_z0_kurtosis_ALL_merged");
        book(m_vx_z0_kurtosis_ALL_split,"vx_z0_kurtosis_ALL_split");
        book(m_vx_z0_kurtosis_ALL_fake,"vx_z0_kurtosis_ALL_fake");

        book(m_vx_d0_skewness_ALL_matched,"vx_d0_skewness_ALL_matched");
        book(m_vx_d0_skewness_ALL_merged,"vx_d0_skewness_ALL_merged");
        book(m_vx_d0_skewness_ALL_split,"vx_d0_skewness_ALL_split");
        book(m_vx_d0_skewness_ALL_fake,"vx_d0_skewness_ALL_fake");
        book(m_vx_d0_kurtosis_ALL_matched,"vx_d0_kurtosis_ALL_matched");
        book(m_vx_d0_kurtosis_ALL_merged,"vx_d0_kurtosis_ALL_merged");
        book(m_vx_d0_kurtosis_ALL_split,"vx_d0_kurtosis_ALL_split");
        book(m_vx_d0_kurtosis_ALL_fake,"vx_d0_kurtosis_ALL_fake");

        book(m_vx_pt_skewness_ALL_matched,"vx_pt_skewness_ALL_matched");
        book(m_vx_pt_skewness_ALL_merged,"vx_pt_skewness_ALL_merged");
        book(m_vx_pt_skewness_ALL_split,"vx_pt_skewness_ALL_split");
        book(m_vx_pt_skewness_ALL_fake,"vx_pt_skewness_ALL_fake");
        book(m_vx_pt_kurtosis_ALL_matched,"vx_pt_kurtosis_ALL_matched");
        book(m_vx_pt_kurtosis_ALL_merged,"vx_pt_kurtosis_ALL_merged");
        book(m_vx_pt_kurtosis_ALL_split,"vx_pt_kurtosis_ALL_split");
        book(m_vx_pt_kurtosis_ALL_fake,"vx_pt_kurtosis_ALL_fake");
        
        book(m_vx_nVertices_ALL_matched,"vx_nVertices_ALL_matched");
        book(m_vx_nVertices_ALL_merged,"vx_nVertices_ALL_merged");
        book(m_vx_nVertices_ALL_split,"vx_nVertices_ALL_split");
        book(m_vx_nVertices_HS_matched,"vx_nVertices_HS_matched");
        book(m_vx_nVertices_HS_merged,"vx_nVertices_HS_merged");
        book(m_vx_nVertices_HS_split,"vx_nVertices_HS_split");
        book(m_vx_nVertices_matched,"vx_nVertices_matched");
        book(m_vx_nVertices_merged,"vx_nVertices_merged");
        book(m_vx_nVertices_split, "vx_nVertices_split");

        book(m_vx_track_z0_ALL_matched,"vx_track_z0_ALL_matched");
        book(m_vx_track_z0_ALL_merged,"vx_track_z0_ALL_merged");
        book(m_vx_track_z0_ALL_split,"vx_track_z0_ALL_split");
        book(m_vx_track_z0_HS_matched,"vx_track_z0_HS_matched");
        book(m_vx_track_z0_HS_merged,"vx_track_z0_HS_merged");
        book(m_vx_track_z0_HS_split,"vx_track_z0_HS_split");
        book(m_vx_track_z0_matched,"vx_track_z0_matched");
        book(m_vx_track_z0_merged,"vx_track_z0_merged");
        book(m_vx_track_z0_split, "vx_track_z0_split");
        
        book(m_vx_track_d0_ALL_matched,"vx_track_d0_ALL_matched");
        book(m_vx_track_d0_ALL_merged,"vx_track_d0_ALL_merged");
        book(m_vx_track_d0_ALL_split,"vx_track_d0_ALL_split");
        book(m_vx_track_d0_HS_matched,"vx_track_d0_HS_matched");
        book(m_vx_track_d0_HS_merged,"vx_track_d0_HS_merged");
        book(m_vx_track_d0_HS_split,"vx_track_d0_HS_split");
        book(m_vx_track_d0_matched,"vx_track_d0_matched");
        book(m_vx_track_d0_merged,"vx_track_d0_merged");
        book(m_vx_track_d0_split, "vx_track_d0_split");
    
        book(m_vx_track_z0_err_ALL_matched,"vx_track_z0_err_ALL_matched");
        book(m_vx_track_z0_err_ALL_merged,"vx_track_z0_err_ALL_merged");
        book(m_vx_track_z0_err_ALL_split,"vx_track_z0_err_ALL_split");
        book(m_vx_track_z0_err_HS_matched,"vx_track_z0_err_HS_matched");
        book(m_vx_track_z0_err_HS_merged,"vx_track_z0_err_HS_merged");
        book(m_vx_track_z0_err_HS_split,"vx_track_z0_err_HS_split");
        book(m_vx_track_z0_err_matched,"vx_track_z0_err_matched");
        book(m_vx_track_z0_err_merged,"vx_track_z0_err_merged");
        book(m_vx_track_z0_err_split, "vx_track_z0_err_split");
        
        book(m_vx_track_d0_err_ALL_matched,"vx_track_d0_err_ALL_matched");
        book(m_vx_track_d0_err_ALL_merged,"vx_track_d0_err_ALL_merged");
        book(m_vx_track_d0_err_ALL_split,"vx_track_d0_err_ALL_split");
        book(m_vx_track_d0_err_HS_matched,"vx_track_d0_err_HS_matched");
        book(m_vx_track_d0_err_HS_merged,"vx_track_d0_err_HS_merged");
        book(m_vx_track_d0_err_HS_split,"vx_track_d0_err_HS_split");
        book(m_vx_track_d0_err_matched,"vx_track_d0_err_matched");
        book(m_vx_track_d0_err_merged,"vx_track_d0_err_merged");
        book(m_vx_track_d0_err_split, "vx_track_d0_err_split");

        book(m_vx_track_z0_ALL_matched_vs_track_pt,"vx_track_z0_ALL_matched_vs_track_pt");
        book(m_vx_track_z0_ALL_merged_vs_track_pt,"vx_track_z0_ALL_merged_vs_track_pt");
        book(m_vx_track_z0_ALL_split_vs_track_pt,"vx_track_z0_ALL_split_vs_track_pt");
        book(m_vx_track_z0_HS_matched_vs_track_pt,"vx_track_z0_HS_matched_vs_track_pt");
        book(m_vx_track_z0_HS_merged_vs_track_pt,"vx_track_z0_HS_merged_vs_track_pt");
        book(m_vx_track_z0_HS_split_vs_track_pt,"vx_track_z0_HS_split_vs_track_pt");
        book(m_vx_track_z0_matched_vs_track_pt,"vx_track_z0_matched_vs_track_pt");
        book(m_vx_track_z0_merged_vs_track_pt,"vx_track_z0_merged_vs_track_pt");
        book(m_vx_track_z0_split_vs_track_pt, "vx_track_z0_split_vs_track_pt");
        
        book(m_vx_track_d0_ALL_matched_vs_track_pt,"vx_track_d0_ALL_matched_vs_track_pt");
        book(m_vx_track_d0_ALL_merged_vs_track_pt,"vx_track_d0_ALL_merged_vs_track_pt");
        book(m_vx_track_d0_ALL_split_vs_track_pt,"vx_track_d0_ALL_split_vs_track_pt");
        book(m_vx_track_d0_HS_matched_vs_track_pt,"vx_track_d0_HS_matched_vs_track_pt");
        book(m_vx_track_d0_HS_merged_vs_track_pt,"vx_track_d0_HS_merged_vs_track_pt");
        book(m_vx_track_d0_HS_split_vs_track_pt,"vx_track_d0_HS_split_vs_track_pt");
        book(m_vx_track_d0_matched_vs_track_pt,"vx_track_d0_matched_vs_track_pt");
        book(m_vx_track_d0_merged_vs_track_pt,"vx_track_d0_merged_vs_track_pt");
        book(m_vx_track_d0_split_vs_track_pt, "vx_track_d0_split_vs_track_pt");
    
        book(m_vx_track_z0_err_ALL_matched_vs_track_pt,"vx_track_z0_err_ALL_matched_vs_track_pt");
        book(m_vx_track_z0_err_ALL_merged_vs_track_pt,"vx_track_z0_err_ALL_merged_vs_track_pt");
        book(m_vx_track_z0_err_ALL_split_vs_track_pt,"vx_track_z0_err_ALL_split_vs_track_pt");
        book(m_vx_track_z0_err_HS_matched_vs_track_pt,"vx_track_z0_err_HS_matched_vs_track_pt");
        book(m_vx_track_z0_err_HS_merged_vs_track_pt,"vx_track_z0_err_HS_merged_vs_track_pt");
        book(m_vx_track_z0_err_HS_split_vs_track_pt,"vx_track_z0_err_HS_split_vs_track_pt");
        book(m_vx_track_z0_err_matched_vs_track_pt,"vx_track_z0_err_matched_vs_track_pt");
        book(m_vx_track_z0_err_merged_vs_track_pt,"vx_track_z0_err_merged_vs_track_pt");
        book(m_vx_track_z0_err_split_vs_track_pt, "vx_track_z0_err_split_vs_track_pt");
        
        book(m_vx_track_d0_err_ALL_matched_vs_track_pt,"vx_track_d0_err_ALL_matched_vs_track_pt");
        book(m_vx_track_d0_err_ALL_merged_vs_track_pt,"vx_track_d0_err_ALL_merged_vs_track_pt");
        book(m_vx_track_d0_err_ALL_split_vs_track_pt,"vx_track_d0_err_ALL_split_vs_track_pt");
        book(m_vx_track_d0_err_HS_matched_vs_track_pt,"vx_track_d0_err_HS_matched_vs_track_pt");
        book(m_vx_track_d0_err_HS_merged_vs_track_pt,"vx_track_d0_err_HS_merged_vs_track_pt");
        book(m_vx_track_d0_err_HS_split_vs_track_pt,"vx_track_d0_err_HS_split_vs_track_pt");
        book(m_vx_track_d0_err_matched_vs_track_pt,"vx_track_d0_err_matched_vs_track_pt");
        book(m_vx_track_d0_err_merged_vs_track_pt,"vx_track_d0_err_merged_vs_track_pt");
        book(m_vx_track_d0_err_split_vs_track_pt, "vx_track_d0_err_split_vs_track_pt");

    }

}
const xAOD::Vertex* InDetPerfPlot_VertexTruthMatching::getHSRecoVertexSumPt2(const xAOD::VertexContainer& recoVertices) const {
    const xAOD::Vertex* recoHSVertex = nullptr;
    float sumPtMax = -1.;
    const xAOD::TrackParticle* trackTmp = nullptr;
    float sumPtTmp;
    for (const auto& vtx : recoVertices.stdcont()) {
        if (vtx) {
            sumPtTmp = 0.;
            for (size_t i = 0; i < vtx->nTrackParticles(); i++) {
                trackTmp = vtx->trackParticle(i);
                if (trackTmp) {
                    sumPtTmp += std::pow(trackTmp->pt(), 2);
                }
            }
            if (sumPtTmp > sumPtMax) {
                sumPtMax = sumPtTmp;
                recoHSVertex = vtx;
            }
        }
    }
    return recoHSVertex;
}





template<typename U, typename V>
float InDetPerfPlot_VertexTruthMatching::getRadialDiff2(const U* vtx1, const V* vtx2) const {
    return (std::pow((vtx1->x() - vtx2->x()), 2) + std::pow((vtx1->y() - vtx2->y()), 2) + std::pow((vtx1->z() - vtx2->z()), 2));
}

float InDetPerfPlot_VertexTruthMatching::getLocalPUDensity(const xAOD::TruthVertex* vtxOfInterest, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices, const float radialWindow) const {
    float radialWindow2 = std::pow(radialWindow, 2);
    int nTracksInWindow = 0;
    float localPUDensity;
    float radialDiff2;
    for (const auto& vtx : truthHSVertices) {
        if (vtx != vtxOfInterest) {
            radialDiff2 = getRadialDiff2(vtxOfInterest, vtx);
            if (radialDiff2 < radialWindow2) {
                nTracksInWindow += 1;
            }
        }
    }
    for (const auto& vtx : truthPUVertices) {
        if (vtx != vtxOfInterest) {
            radialDiff2 = getRadialDiff2(vtxOfInterest, vtx);
            if (radialDiff2 < radialWindow2) {
                nTracksInWindow += 1;
            }
        }
    }
    localPUDensity = (float)(nTracksInWindow) / (2 * radialWindow);
    return localPUDensity;
}

float InDetPerfPlot_VertexTruthMatching::getRecoLongitudinalReso(const xAOD::Vertex* recoVtx) const {
    return std::sqrt(recoVtx->covariancePosition()(2, 2));
}

float InDetPerfPlot_VertexTruthMatching::getRecoTransverseReso(const xAOD::Vertex* recoVtx) const {
    float x = recoVtx->x();
    float y = recoVtx->y();
    float xErr2 = recoVtx->covariancePosition()(0, 0);
    float yErr2 = recoVtx->covariancePosition()(1, 1);
    float xyCov = recoVtx->covariancePosition()(0, 1);
    float r2 = std::pow(x, 2) + std::pow(y, 2);
    return std::sqrt(std::pow(x, 2) / r2 * xErr2 + std::pow(y, 2) / r2 * yErr2 + 2 * x * y / r2 * xyCov);
}

// Copied from Graham:
void InDetPerfPlot_VertexTruthMatching::fillResoHist(TH1* resoHist, const TH2* resoHist2D) {

    TH1* projHist = nullptr;
    int safety_counter;
    TFitResultPtr fitResult;
    double mean;
    double rms;
    double itr_rms = -1.;
    double itr_rms_err;

    for (int i = 1; i < resoHist2D->GetNbinsX() + 1; i++) {

        projHist = resoHist2D->ProjectionY("projectionY", i, i);

        if (projHist->GetEntries() == 0.) {
            resoHist->SetBinContent(i, 0.);
            resoHist->SetBinError(i, 0.);
            continue;
        }

        safety_counter = 0;

        fitResult = projHist->Fit("gaus", "QS0");
        if (!fitResult.Get()) {
            // Is it necessary to also require fitResult->Status() % 1000 == 0 for a successful fit?
            // --> fitStatus = migradResult + 10 * minosResult + 100 * hesseResult + 1000 * improveResult
            resoHist->SetBinContent(i, 0.);
            resoHist->SetBinError(i, 0.);
            continue;
        }
        mean = fitResult->Parameter(1);
        rms  = fitResult->Parameter(2);

        while (true) {

            projHist->SetAxisRange(mean - 3 * rms, mean + 3 * rms, "X");

            fitResult = projHist->Fit("gaus", "QS0");
            if (!fitResult.Get()) {
                itr_rms     = 0.;
                itr_rms_err = 0.;
                break;
            }
            itr_rms     = fitResult->Parameter(2);
            itr_rms_err = fitResult->ParError(2);

            if ((fabs(itr_rms - rms) < 0.0001) || (safety_counter == 5)) {
                break;
            }

            safety_counter++;
            mean = fitResult->Parameter(1);
            rms  = itr_rms;
            continue;

        }

        resoHist->SetBinContent(i, itr_rms);
        resoHist->SetBinError(i, itr_rms_err);

    }
}

const xAOD::TruthVertex* InDetPerfPlot_VertexTruthMatching::getTruthVertex(const xAOD::Vertex* recoVtx) const {
    const xAOD::TruthVertex* truthVtx = nullptr;
    if (recoVtx) {
        const static xAOD::Vertex::Decorator<std::vector<InDetVertexTruthMatchUtils::VertexTruthMatchInfo>> truthMatchingInfos("TruthEventMatchingInfos");
        const std::vector<InDetVertexTruthMatchUtils::VertexTruthMatchInfo>& truthInfos = truthMatchingInfos(*recoVtx);
        if (!truthInfos.empty()) {
            const InDetVertexTruthMatchUtils::VertexTruthMatchInfo& truthInfo = truthInfos.at(0);
            const ElementLink<xAOD::TruthEventBaseContainer> truthEventLink = std::get<0>(truthInfo);
            const xAOD::TruthEvent* truthEvent = nullptr;
            if (truthEventLink.isValid()) {
                truthEvent = static_cast<const xAOD::TruthEvent*>(*truthEventLink);
                if (truthEvent) {
                    truthVtx = truthEvent->truthVertex(0);
                }
            }
        }
        else {
            ATH_MSG_WARNING("TruthEventMatchingInfos DECORATOR yields empty vector -- returning nullptr!");
        }
    }
    return truthVtx;
}

void InDetPerfPlot_VertexTruthMatching::fill(const xAOD::Vertex& vertex) {

    // Get the match type info for each vertex:
    const static xAOD::Vertex::Decorator<InDetVertexTruthMatchUtils::VertexMatchType> recoVtxMatchTypeInfo("VertexMatchType");
    InDetVertexTruthMatchUtils::VertexMatchType matchType;
    if (recoVtxMatchTypeInfo.isAvailable(vertex)) {
        try {
            matchType = recoVtxMatchTypeInfo(vertex);
            ATH_MSG_DEBUG("VERTEX DECORATOR ======= " << matchType << ", with nTRACKS === " << vertex.nTrackParticles() << ", vertex index = " << vertex.index() << " AT (x, y, z) = (" << vertex.x() << ", " << vertex.y() << ", " << vertex.z() << ")");
            fillHisto(m_vx_type_truth, matchType);




        }
        catch (SG::ExcBadAuxVar &) {
            ATH_MSG_WARNING("VertexMatchType DECORATOR seems to be available, but may be broken  ===========");
        } 
    }
    else {
        ATH_MSG_WARNING("VertexMatchType DECORATOR is NOT available  ===========");
    }

} // void InDetPerfPlot_VertexTruthMatching::fill(const xAOD::Vertex& vertex) {

void InDetPerfPlot_VertexTruthMatching::fill(const xAOD::VertexContainer& vertexContainer, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices) {

    if (m_iDetailLevel >= 200) {

        // Fill our histograms
        // Inclusive:
        int nTruthVertices = (int)(truthHSVertices.size() + truthPUVertices.size());
        int nRecoVertices = (int)vertexContainer.size();
        fillHisto(m_vx_nReco_vs_nTruth_inclusive, nTruthVertices, nRecoVertices);
        
        // Let's also plot the vertices by vertex match type:
        const static xAOD::Vertex::Decorator<InDetVertexTruthMatchUtils::VertexMatchType> recoVtxMatchTypeInfo("VertexMatchType");
        std::map<InDetVertexTruthMatchUtils::VertexMatchType, int> breakdown = {};
        breakdown[InDetVertexTruthMatchUtils::VertexMatchType::MATCHED] = 0;
        breakdown[InDetVertexTruthMatchUtils::VertexMatchType::MERGED]  = 0;
        breakdown[InDetVertexTruthMatchUtils::VertexMatchType::SPLIT]   = 0;
        breakdown[InDetVertexTruthMatchUtils::VertexMatchType::FAKE]    = 0;
        breakdown[InDetVertexTruthMatchUtils::VertexMatchType::DUMMY]   = 0;

        const xAOD::TruthVertex* truthVtx = nullptr;
        float localPUDensity;

        // Best reco HS vertex identified via sumpt2
        const xAOD::Vertex* bestRecoHSVtx_sumpt2 = getHSRecoVertexSumPt2(vertexContainer); // Could potentially use the first vertex in the container if sumpt2-ordered
        // Best reco HS vertex identified via truth HS weights
        const xAOD::Vertex* bestRecoHSVtx_truth = InDetVertexTruthMatchUtils::bestHardScatterMatch(vertexContainer);

       // add code to add protection against non-identified reconstructed HS vertex to avoid crashes in samples with low track multiplicity HS vertex
       if (!bestRecoHSVtx_truth){
            ATH_MSG_INFO("No bestRecoHS vertex - not filling vertex truth matching.");
            return;
        }



        // Did we correctly select the best reco HS vertex using sumpt2?
        truthVtx = getTruthVertex(bestRecoHSVtx_sumpt2);
        if (!truthVtx){
            ATH_MSG_INFO("No truth HS - not filling vertex truth matching."); 
            return;
        }
        localPUDensity = getLocalPUDensity(truthVtx, truthHSVertices, truthPUVertices);
        fillHisto(m_vx_hs_sel_eff, localPUDensity, (bestRecoHSVtx_sumpt2 == bestRecoHSVtx_truth));

        // Did we successfully reconstruct our truth HS vertex?
        bool truthHSVtxRecoed = false;
        float minTruthRecoRadialDiff2 = std::pow(m_cutMinTruthRecoRadialDiff, 2);
        float truthRecoRadialDiff2 = -1.;
        const xAOD::TruthVertex* truthHSVtx = nullptr;
        // Check that we have *exactly* 1 truth HS vertex
        if (truthHSVertices.size() != 0) {
            if (truthHSVertices.size() != 1) {
                ATH_MSG_WARNING("Size of truth HS vertex vector is >1 -- only using the first one in the vector.");
            }
            truthHSVtx = truthHSVertices.at(0);
            // If the radial difference between the truth-pkg-selected best reco HS vertex and the truth HS vertex is
            // less than some cut (e.g., 0.1 mm), then we say the truth HS vertex is reconstructed
            truthRecoRadialDiff2 = getRadialDiff2(bestRecoHSVtx_truth, truthHSVtx);
            if (truthRecoRadialDiff2 < minTruthRecoRadialDiff2) {
                truthHSVtxRecoed = true;
                minTruthRecoRadialDiff2 = truthRecoRadialDiff2;
            }
        }
        else {
            ATH_MSG_WARNING("Size of truth HS vertex vector is 0 -- assuming truth HS vertex to NOT be reconstructed.");
        }
         

        // add variables here so that they are in correct scope (outside loop over vertices)
        float number_matched = 0;
        float number_merged = 0;
        float number_split = 0;
        float number_matched_HS = 0;
        float number_merged_HS = 0;
        float number_split_HS = 0;
        float number_matched_PU = 0;
        float number_merged_PU = 0;
        float number_split_PU = 0;
      
   //   std::cout << "declaring vertex number variables helenmaguire";

 
        // Iterate over vertices:
        InDetVertexTruthMatchUtils::VertexMatchType matchType;
        for (const auto& vertex : vertexContainer.stdcont()) {

            // Skip dummy vertex (last one in the container)
            if (vertex->vertexType() == xAOD::VxType::NoVtx) {
                continue;
            }

            fill(*vertex);

            matchType = recoVtxMatchTypeInfo(*vertex);
            breakdown[matchType] += 1;

            // If we have reconstructed the truth HS vertex but we have a different reco vertex closer to the truth HS vertex
            // than the best one identified by the truth pkg, we say we have NOT successfully reconstructed the truth HS vertex
            if (truthHSVtxRecoed && (vertex != bestRecoHSVtx_truth)) {
                truthRecoRadialDiff2 = getRadialDiff2(vertex, truthHSVtx);
                if (truthRecoRadialDiff2 < minTruthRecoRadialDiff2) {
                    truthHSVtxRecoed = false;
                    minTruthRecoRadialDiff2 = truthRecoRadialDiff2;
                }
            }
          
            const xAOD::TruthVertex *matchVertex = getTruthVertex(vertex);
            if(!matchVertex) continue;
            float residual_z = matchVertex->z() - vertex->z();
            float residual_x = matchVertex->x() - vertex->x();
            float residual_y = matchVertex->y() - vertex->y();
            const AmgSymMatrix(3)& covariance = vertex->covariancePosition();
            float vtxerr_x = fabs(Amg::error(covariance, 0)) > 1e-7 ? Amg::error(covariance, 0) : 1000.;
            float vtxerr_y = fabs(Amg::error(covariance, 1)) > 1e-7 ? Amg::error(covariance, 1) : 1000.;
            float vtxerr_z = fabs(Amg::error(covariance, 2)) > 1e-7 ? Amg::error(covariance, 2) : 1000.;
            localPUDensity = getLocalPUDensity(matchVertex, truthHSVertices, truthPUVertices);
    
            fillHisto(m_vx_all_z_pull, residual_z/vtxerr_z);
            fillHisto(m_vx_all_y_pull, residual_y/vtxerr_y);
            fillHisto(m_vx_all_x_pull, residual_x/vtxerr_x);

            fillHisto(m_vx_all_truth_z_res_vs_PU, localPUDensity, residual_z);
            fillHisto(m_vx_all_truth_x_res_vs_PU, localPUDensity, residual_x);
            fillHisto(m_vx_all_truth_y_res_vs_PU, localPUDensity, residual_y);

            fillHisto(m_vx_all_z_res, residual_z);
            fillHisto(m_vx_all_y_res, residual_y);
            fillHisto(m_vx_all_x_res, residual_x);
            
            fillHisto(m_vx_all_truth_z_pull_vs_PU, localPUDensity, residual_z/vtxerr_z);
            fillHisto(m_vx_all_truth_x_pull_vs_PU, localPUDensity, residual_x/vtxerr_y);
            fillHisto(m_vx_all_truth_y_pull_vs_PU, localPUDensity, residual_y/vtxerr_x);

            fillHisto(m_vx_all_truth_z_res_vs_nTrk, vertex->nTrackParticles(), residual_z);
            fillHisto(m_vx_all_truth_x_res_vs_nTrk, vertex->nTrackParticles(), residual_x);
            fillHisto(m_vx_all_truth_y_res_vs_nTrk, vertex->nTrackParticles(), residual_y);
            
            fillHisto(m_vx_all_truth_z_pull_vs_nTrk, vertex->nTrackParticles(), residual_z/vtxerr_z);
            fillHisto(m_vx_all_truth_x_pull_vs_nTrk, vertex->nTrackParticles(), residual_x/vtxerr_y);
            fillHisto(m_vx_all_truth_y_pull_vs_nTrk, vertex->nTrackParticles(), residual_y/vtxerr_x);




// Helen OTP - get sumpt for each vertex (not just for HS vertex)
// For each vertex, loop over all tracks and get sumpt and sum of charges
// also use this to get the z asymmetry around the vertex.
   
        
         const xAOD::TrackParticle* trackTmp = nullptr;
         float sumPt =0;
         float trackPt = 0;
         float sum_dD =0;

         // get the charge associated with each track
         double sumCharge = 0;
         float z_asym = 0;
         float d_asym = 0;
         float sumDZ = 0;
         float deltaZ =0;
         float deltaD =0;
         float modsumDZ =0;
         float modsumDD =0;

         // make vector
         std::vector<float> track_deltaZ;
         track_deltaZ.clear();
         std::vector<float> track_deltaD;
         track_deltaD.clear();
         std::vector<float> track_deltaPt;
         track_deltaPt.clear();

            // loop over tracks 
            //  std::cout << "loop over tracks helenmaguire";
            for (size_t i = 0; i < vertex->nTrackParticles(); i++) {
                trackTmp = vertex->trackParticle(i);
                trackPt = trackTmp->pt() / Gaudi::Units::GeV; 
                deltaZ = trackTmp->z0() - vertex->z();
                deltaD = trackTmp->d0();
                const xAOD::ParametersCovMatrix_t covTrk = trackTmp->definingParametersCovMatrix();
        
 
           if (vertex == bestRecoHSVtx_truth) {
 
           switch (matchType) {
               case InDetVertexTruthMatchUtils::VertexMatchType::MATCHED: {
                   fillHisto(m_vx_track_z0_HS_matched, deltaZ);
                   fillHisto(m_vx_track_d0_HS_matched, deltaD);
                   fillHisto(m_vx_track_z0_err_HS_matched,Amg::error(covTrk, 1));
                   fillHisto(m_vx_track_d0_err_HS_matched, Amg::error(covTrk, 0));
                   fillHisto(m_vx_track_z0_HS_matched_vs_track_pt, deltaZ, trackPt);
                   fillHisto(m_vx_track_d0_HS_matched_vs_track_pt, deltaD, trackPt);
                   fillHisto(m_vx_track_z0_err_HS_matched_vs_track_pt,Amg::error(covTrk, 1), trackPt);
                   fillHisto(m_vx_track_d0_err_HS_matched_vs_track_pt, Amg::error(covTrk, 0), trackPt);
                   break;
               }
               case InDetVertexTruthMatchUtils::VertexMatchType::MERGED: {
                   fillHisto(m_vx_track_z0_HS_merged, deltaZ);
                   fillHisto(m_vx_track_d0_HS_merged, deltaD);
                   fillHisto(m_vx_track_z0_err_HS_merged,Amg::error(covTrk, 1));
                   fillHisto(m_vx_track_d0_err_HS_merged, Amg::error(covTrk, 0));
                   fillHisto(m_vx_track_z0_HS_merged_vs_track_pt, deltaZ, trackPt);
                   fillHisto(m_vx_track_d0_HS_merged_vs_track_pt, deltaD, trackPt);
                   fillHisto(m_vx_track_z0_err_HS_merged_vs_track_pt,Amg::error(covTrk, 1), trackPt);
                   fillHisto(m_vx_track_d0_err_HS_merged_vs_track_pt, Amg::error(covTrk, 0), trackPt);
                   break;
               }
               case InDetVertexTruthMatchUtils::VertexMatchType::SPLIT: {
                   fillHisto(m_vx_track_z0_HS_split, deltaZ);
                   fillHisto(m_vx_track_d0_HS_split, deltaD);
                   fillHisto(m_vx_track_z0_err_HS_split,Amg::error(covTrk, 1));
                   fillHisto(m_vx_track_d0_err_HS_split, Amg::error(covTrk, 0));
                   fillHisto(m_vx_track_z0_HS_split_vs_track_pt, deltaZ, trackPt);
                   fillHisto(m_vx_track_d0_HS_split_vs_track_pt, deltaD, trackPt);
                   fillHisto(m_vx_track_z0_err_HS_split_vs_track_pt,Amg::error(covTrk, 1), trackPt);
                   fillHisto(m_vx_track_d0_err_HS_split_vs_track_pt, Amg::error(covTrk, 0), trackPt);
                   break;
              }
              default: {
                   break;
               }
           
            }   // end switch
            }  // end if HS
            else {
             switch (matchType) {
               case InDetVertexTruthMatchUtils::VertexMatchType::MATCHED: {
                   fillHisto(m_vx_track_z0_matched, deltaZ);
                   fillHisto(m_vx_track_d0_matched, deltaD);
                   fillHisto(m_vx_track_z0_err_matched,Amg::error(covTrk, 1));
                   fillHisto(m_vx_track_d0_err_matched, Amg::error(covTrk, 0));
                   fillHisto(m_vx_track_z0_matched_vs_track_pt, deltaZ, trackPt);
                   fillHisto(m_vx_track_d0_matched_vs_track_pt, deltaD, trackPt);
                   fillHisto(m_vx_track_z0_err_matched_vs_track_pt,Amg::error(covTrk, 1), trackPt);
                   fillHisto(m_vx_track_d0_err_matched_vs_track_pt, Amg::error(covTrk, 0), trackPt);
                   break;
               }
               case InDetVertexTruthMatchUtils::VertexMatchType::MERGED: {
                   fillHisto(m_vx_track_z0_merged, deltaZ);
                   fillHisto(m_vx_track_d0_merged, deltaD);
                   fillHisto(m_vx_track_z0_err_merged,Amg::error(covTrk, 1));
                   fillHisto(m_vx_track_d0_err_merged, Amg::error(covTrk, 0));
                   fillHisto(m_vx_track_z0_merged_vs_track_pt, deltaZ, trackPt);
                   fillHisto(m_vx_track_d0_merged_vs_track_pt, deltaD, trackPt);
                   fillHisto(m_vx_track_z0_err_merged_vs_track_pt,Amg::error(covTrk, 1), trackPt);
                   fillHisto(m_vx_track_d0_err_merged_vs_track_pt, Amg::error(covTrk, 0), trackPt);
                   break;
               }
               case InDetVertexTruthMatchUtils::VertexMatchType::SPLIT: {
                   fillHisto(m_vx_track_z0_split, deltaZ);
                   fillHisto(m_vx_track_d0_split, deltaD);
                   fillHisto(m_vx_track_z0_err_split,Amg::error(covTrk, 1));
                   fillHisto(m_vx_track_d0_err_split, Amg::error(covTrk, 0));
                   fillHisto(m_vx_track_z0_split_vs_track_pt, deltaZ, trackPt);
                   fillHisto(m_vx_track_d0_split_vs_track_pt, deltaD, trackPt);
                   fillHisto(m_vx_track_z0_err_split_vs_track_pt,Amg::error(covTrk, 1), trackPt);
                   fillHisto(m_vx_track_d0_err_split_vs_track_pt, Amg::error(covTrk, 0), trackPt);
                   break;
               }
              default: {
                   break;
               }
            } // end switch
           }  // end else


             double minpt = 20 ; // GeV
             if (sumPt/Gaudi::Units::GeV > minpt) {

             switch (matchType) {
               case InDetVertexTruthMatchUtils::VertexMatchType::MATCHED: {
                   fillHisto(m_vx_track_z0_ALL_matched, deltaZ);
                   fillHisto(m_vx_track_d0_ALL_matched, deltaD);
                   fillHisto(m_vx_track_z0_err_ALL_matched,Amg::error(covTrk, 1));
                   fillHisto(m_vx_track_d0_err_ALL_matched, Amg::error(covTrk, 0));
                   fillHisto(m_vx_track_z0_ALL_matched_vs_track_pt, deltaZ, trackPt);
                   fillHisto(m_vx_track_d0_ALL_matched_vs_track_pt, deltaD, trackPt);
                   fillHisto(m_vx_track_z0_err_ALL_matched_vs_track_pt,Amg::error(covTrk, 1), trackPt);
                   fillHisto(m_vx_track_d0_err_ALL_matched_vs_track_pt, Amg::error(covTrk, 0), trackPt);
                   break;
               }
               case InDetVertexTruthMatchUtils::VertexMatchType::MERGED: {
                   fillHisto(m_vx_track_z0_ALL_merged, deltaZ);
                   fillHisto(m_vx_track_d0_ALL_merged, deltaD);
                   fillHisto(m_vx_track_z0_err_ALL_merged,Amg::error(covTrk, 1));
                   fillHisto(m_vx_track_d0_err_ALL_merged, Amg::error(covTrk, 0));
                   fillHisto(m_vx_track_z0_ALL_merged_vs_track_pt, deltaZ, trackPt);
                   fillHisto(m_vx_track_d0_ALL_merged_vs_track_pt, deltaD, trackPt);
                   fillHisto(m_vx_track_z0_err_ALL_merged_vs_track_pt,Amg::error(covTrk, 1), trackPt);
                   fillHisto(m_vx_track_d0_err_ALL_merged_vs_track_pt, Amg::error(covTrk, 0), trackPt);
                   break;
               }
               case InDetVertexTruthMatchUtils::VertexMatchType::SPLIT: {
                   fillHisto(m_vx_track_z0_ALL_split, deltaZ);
                   fillHisto(m_vx_track_d0_ALL_split, deltaD);
                   fillHisto(m_vx_track_z0_err_ALL_split,Amg::error(covTrk, 1));
                   fillHisto(m_vx_track_d0_err_ALL_split, Amg::error(covTrk, 0));
                   fillHisto(m_vx_track_z0_ALL_split_vs_track_pt, deltaZ, trackPt);
                   fillHisto(m_vx_track_d0_ALL_split_vs_track_pt, deltaD, trackPt);
                   fillHisto(m_vx_track_z0_err_ALL_split_vs_track_pt,Amg::error(covTrk, 1), trackPt);
                   fillHisto(m_vx_track_d0_err_ALL_split_vs_track_pt, Amg::error(covTrk, 0), trackPt);
                   break;
               }
              default: {
                   break;
               }

             } // end switch
           } // end if sumPt


          
               
                if (trackTmp) {
                    sumPt = sumPt + trackTmp->pt();
                    sumCharge = sumCharge + trackTmp->charge();
                    track_deltaZ.push_back(deltaZ);
                    track_deltaD.push_back(deltaD);
                    track_deltaPt.push_back(trackTmp->pt());
                    // sum of delta z
                    sumDZ = sumDZ + deltaZ; 
                    modsumDZ = modsumDZ + sqrt(deltaZ * deltaZ);
                    sum_dD = sum_dD + trackTmp->d0();
                    modsumDD = modsumDD + sqrt(deltaD * deltaD);
               }

            } // end loop over tracks
            if  (modsumDZ >0) {
                z_asym = sumDZ/modsumDZ;
            }
            if  (modsumDD >0) {
                d_asym = sum_dD/modsumDD;
            }


          
            double av_sumPt =0;
            double mean_Dz =0;
            mean_Dz=sumDZ/track_deltaZ.size(); //calculate average
            sum_dD=sum_dD/track_deltaD.size(); //calculate average
            av_sumPt=sumPt/track_deltaPt.size(); //calculate average
            double n=0;
            n = track_deltaZ.size(); // get number of tracks 

            double z_sd = 0; //  standard deviation
            double z_skew = 0;
            double z_kurt = 0;
            double z_var=0;
            double d_sd = 0;
            double d_var = 0;
            double d_kurt =0;
            double d_skew =0;
            double pt_sd = 0;
            double pt_var = 0;
            double pt_kurt =0;
            double pt_skew =0;

            for ( size_t i = 0; i < track_deltaZ.size(); i++) {

               z_var =(z_var + (track_deltaZ.at(i) - mean_Dz )*(track_deltaZ.at(i) - mean_Dz ));
               z_skew =(z_skew + (track_deltaZ.at(i) - mean_Dz )*(track_deltaZ.at(i) - mean_Dz )*(track_deltaZ.at(i) - mean_Dz ));
               z_kurt =(z_kurt + (track_deltaZ.at(i) - mean_Dz )*(track_deltaZ.at(i) - mean_Dz )*(track_deltaZ.at(i) - mean_Dz )*(track_deltaZ.at(i) - mean_Dz ));

           } 
               z_var = z_var/(n-1);
               z_sd = sqrt(z_var);
               z_skew = z_skew/((n-1)*z_sd*z_sd*z_sd);
               z_kurt = z_kurt/((n-1)*z_sd*z_sd*z_sd*z_sd);

            for ( size_t i = 0; i < track_deltaD.size(); i++) {

               d_var =(d_var + (track_deltaD.at(i) - sum_dD )*(track_deltaD.at(i) - sum_dD ));
               d_skew =(d_skew + (track_deltaD.at(i) - sum_dD )*(track_deltaD.at(i) - sum_dD )*(track_deltaD.at(i) - sum_dD ));
               d_kurt =(d_kurt + (track_deltaD.at(i) - sum_dD )*(track_deltaD.at(i) - sum_dD )*(track_deltaD.at(i) - sum_dD )*(track_deltaD.at(i) - sum_dD ));

           } 
               d_var = d_var/(n-1);
               d_sd = sqrt(d_var);
               d_skew = d_skew/((n-1)*d_sd*d_sd*d_sd);
               d_kurt = d_kurt/((n-1)*d_sd*d_sd*d_sd*d_sd);


           for ( size_t i = 0; i < track_deltaPt.size(); i++) {

               pt_var =(pt_var + (track_deltaPt.at(i) - av_sumPt )*(track_deltaPt.at(i) - av_sumPt ));
               pt_skew =(pt_skew + (track_deltaPt.at(i) - av_sumPt )*(track_deltaPt.at(i) - av_sumPt )*(track_deltaPt.at(i) - av_sumPt ));
               pt_kurt =(pt_kurt + (track_deltaPt.at(i) - av_sumPt )*(track_deltaPt.at(i) - av_sumPt )*(track_deltaPt.at(i) - av_sumPt )*(track_deltaPt.at(i) - av_sumPt ));

          }
               pt_var = pt_var/(n-1);
               pt_sd = sqrt(pt_var);
               pt_skew = pt_skew/((n-1)*pt_sd*pt_sd*pt_sd);
               pt_kurt = pt_kurt/((n-1)*pt_sd*pt_sd*pt_sd*pt_sd);

          float ndf = vertex->numberDoF();
          if (ndf != 0) {



           if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::MATCHED) {
              if (vertex == bestRecoHSVtx_truth) {
	
		 fillHisto(m_vx_sumpT_HS_matched,sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_HS_matched,sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_HS_matched, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_HS_matched, z_asym);
                 fillHisto(m_vx_d_asym_HS_matched, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_HS_matched, vertex->chiSquared()/ndf);

                 fillHisto(m_vx_z0_skewness_HS_matched, z_skew);
                 fillHisto(m_vx_z0_kurtosis_HS_matched, z_kurt);
                 fillHisto(m_vx_d0_skewness_HS_matched, d_skew);
                 fillHisto(m_vx_d0_kurtosis_HS_matched, d_kurt);
                 fillHisto(m_vx_pt_skewness_HS_matched, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_HS_matched, pt_kurt);
       

                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_HS_matched, weight);
                     fillHisto(m_vx_normalised_track_weight_HS_matched, weight/n);
                 }

              }	
	      else { 
		
                 fillHisto(m_vx_sumpT_matched,sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_matched,sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_matched, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_matched, z_asym);
                 fillHisto(m_vx_d_asym_matched, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_matched, vertex->chiSquared()/ndf);
              
                 fillHisto(m_vx_z0_skewness_matched, z_skew);
                 fillHisto(m_vx_z0_kurtosis_matched, z_kurt);
                 fillHisto(m_vx_d0_skewness_matched, d_skew);
                 fillHisto(m_vx_d0_kurtosis_matched, d_kurt);
                 fillHisto(m_vx_pt_skewness_matched, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_matched, pt_kurt);



                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_matched, weight);
                     fillHisto(m_vx_normalised_track_weight_matched, weight/n);
                 }
             }
// fill some histograms that contain both HS and PU above a min pt - say 20GeV
              double minpt = 20 ; // GeV
              if (sumPt/Gaudi::Units::GeV > minpt) {
                 fillHisto(m_vx_sumpT_ALL_matched,sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_ALL_matched,sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_ALL_matched, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_ALL_matched, z_asym);
                 fillHisto(m_vx_d_asym_ALL_matched, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_ALL_matched, vertex->chiSquared()/ndf);

                 fillHisto(m_vx_z0_skewness_ALL_matched, z_skew);
                 fillHisto(m_vx_z0_kurtosis_ALL_matched, z_kurt);
                 fillHisto(m_vx_d0_skewness_ALL_matched, d_skew);
                 fillHisto(m_vx_d0_kurtosis_ALL_matched, d_kurt);
                 fillHisto(m_vx_pt_skewness_ALL_matched, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_ALL_matched, pt_kurt);
                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_ALL_matched, weight);
                     fillHisto(m_vx_normalised_track_weight_ALL_matched, weight/n);
                 }

            }
           }  // end of if matched vertices

           if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::MERGED) {
              if (vertex == bestRecoHSVtx_truth) {
              
                 fillHisto(m_vx_sumpT_HS_merged, sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_HS_merged, sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_HS_merged, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_HS_merged, z_asym);
                 fillHisto(m_vx_d_asym_HS_merged, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_HS_merged, vertex->chiSquared()/ndf);

                 fillHisto(m_vx_z0_skewness_HS_merged, z_skew);
                 fillHisto(m_vx_z0_kurtosis_HS_merged, z_kurt);
                 fillHisto(m_vx_d0_skewness_HS_merged, d_skew);
                 fillHisto(m_vx_d0_kurtosis_HS_merged, d_kurt);
                 fillHisto(m_vx_pt_skewness_HS_merged, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_HS_merged, pt_kurt);
                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_HS_merged, weight);
                     fillHisto(m_vx_normalised_track_weight_HS_merged, weight/n);
                 }



              }
              else {
                 fillHisto(m_vx_sumpT_merged, sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_merged, sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_merged, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_merged, z_asym);
                 fillHisto(m_vx_d_asym_merged, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_merged, vertex->chiSquared()/ndf);
             
                 fillHisto(m_vx_z0_skewness_merged, z_skew);
                 fillHisto(m_vx_z0_kurtosis_merged, z_kurt);
                 fillHisto(m_vx_d0_skewness_merged, d_skew);
                 fillHisto(m_vx_d0_kurtosis_merged, d_kurt);
                 fillHisto(m_vx_pt_skewness_merged, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_merged, pt_kurt);
                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_merged, weight);
                     fillHisto(m_vx_normalised_track_weight_merged, weight/n);
                 }

              }
              double minpt = 20 ; // GeV
              if (sumPt/Gaudi::Units::GeV > minpt) {
                 fillHisto(m_vx_sumpT_ALL_merged,sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_ALL_merged,sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_ALL_merged, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_ALL_merged, z_asym);
                 fillHisto(m_vx_d_asym_ALL_merged, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_ALL_merged, vertex->chiSquared()/ndf);

                 fillHisto(m_vx_z0_skewness_ALL_merged, z_skew);
                 fillHisto(m_vx_z0_kurtosis_ALL_merged, z_kurt);
                 fillHisto(m_vx_d0_skewness_ALL_merged, d_skew);
                 fillHisto(m_vx_d0_kurtosis_ALL_merged, d_kurt);
                 fillHisto(m_vx_pt_skewness_ALL_merged, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_ALL_merged, pt_kurt);
                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_ALL_merged, weight);
                     fillHisto(m_vx_normalised_track_weight_ALL_merged, weight/n);
                 }

             }
           }   //end of if merged vertices

           if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::SPLIT) {
              if (vertex == bestRecoHSVtx_truth) {
                 fillHisto(m_vx_sumpT_HS_split, sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_HS_split, sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_HS_split, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_HS_split, z_asym);
                 fillHisto(m_vx_d_asym_HS_split, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_HS_split, vertex->chiSquared()/ndf);

                 fillHisto(m_vx_z0_skewness_HS_split, z_skew);
                 fillHisto(m_vx_z0_kurtosis_HS_split, z_kurt);
                 fillHisto(m_vx_d0_skewness_HS_split, d_skew);
                 fillHisto(m_vx_d0_kurtosis_HS_split, d_kurt);
                 fillHisto(m_vx_pt_skewness_HS_split, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_HS_split, pt_kurt);
                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_HS_split, weight);
                     fillHisto(m_vx_normalised_track_weight_HS_split, weight/n);
                 }


              }
              else {
                 fillHisto(m_vx_sumpT_split, sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_split, sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_split, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_split, z_asym);
                 fillHisto(m_vx_d_asym_split, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_split, vertex->chiSquared()/ndf);

                 fillHisto(m_vx_z0_skewness_split, z_skew);
                 fillHisto(m_vx_z0_kurtosis_split, z_kurt);
                 fillHisto(m_vx_d0_skewness_split, d_skew);
                 fillHisto(m_vx_d0_kurtosis_split, d_kurt);
                 fillHisto(m_vx_pt_skewness_split, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_split, pt_kurt);
                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_split, weight);
                     fillHisto(m_vx_normalised_track_weight_split, weight/n);
                 }


              }
// fill some histograms that contain both HS and PU above a min pt - say 20GeV
              double minpt = 20 ; // GeV
              if (sumPt/Gaudi::Units::GeV > minpt) {
                 fillHisto(m_vx_sumpT_ALL_split,sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_ALL_split,sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_ALL_split, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_ALL_split, z_asym);
                 fillHisto(m_vx_d_asym_ALL_split, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_ALL_split, vertex->chiSquared()/ndf);

                 fillHisto(m_vx_z0_skewness_ALL_split, z_skew);
                 fillHisto(m_vx_z0_kurtosis_ALL_split, z_kurt);
                 fillHisto(m_vx_d0_skewness_ALL_split, d_skew);
                 fillHisto(m_vx_d0_kurtosis_ALL_split, d_kurt);
                 fillHisto(m_vx_pt_skewness_ALL_split, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_ALL_split, pt_kurt);
                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_ALL_split, weight);
                     fillHisto(m_vx_normalised_track_weight_ALL_split, weight/n);
                 }
              }

	   }   // end of if split vertices


           if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::FAKE) {
              if (vertex == bestRecoHSVtx_truth) {
                 fillHisto(m_vx_sumpT_HS_fake, sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_HS_fake, sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_HS_fake, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_HS_fake, z_asym);
                 fillHisto(m_vx_d_asym_HS_fake, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_HS_fake, vertex->chiSquared()/ndf);

                 fillHisto(m_vx_z0_skewness_HS_fake, z_skew);
                 fillHisto(m_vx_z0_kurtosis_HS_fake, z_kurt);
                 fillHisto(m_vx_d0_skewness_HS_fake, d_skew);
                 fillHisto(m_vx_d0_kurtosis_HS_fake, d_kurt);
                 fillHisto(m_vx_pt_skewness_HS_fake, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_HS_fake, pt_kurt);
                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_HS_fake, weight);
                 }
              }

              else {

                 fillHisto(m_vx_sumpT_fake, sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_fake, sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_fake, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_fake, z_asym);
                 fillHisto(m_vx_d_asym_fake, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_fake, vertex->chiSquared()/ndf);

                 fillHisto(m_vx_z0_skewness_fake, z_skew);
                 fillHisto(m_vx_z0_kurtosis_fake, z_kurt);
                 fillHisto(m_vx_d0_skewness_fake, d_skew);
                 fillHisto(m_vx_d0_kurtosis_fake, d_kurt);
                 fillHisto(m_vx_pt_skewness_fake, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_fake, pt_kurt);
                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_fake, weight);
                 }
             }
// fill some histograms that contain both HS and PU above a min pt - say 20GeV
              double minpt = 20 ; // GeV
              if (sumPt/Gaudi::Units::GeV > minpt) {
                 fillHisto(m_vx_sumpT_ALL_fake,sumPt / Gaudi::Units::GeV);
                 fillHisto(m_vx_sumCharge_ALL_fake,sumCharge);
                 fillHisto(m_vx_sumCharge_N_trk_ALL_fake, sumCharge/vertex->nTrackParticles());
                 fillHisto(m_vx_z_asym_ALL_fake, z_asym);
                 fillHisto(m_vx_d_asym_ALL_fake, d_asym);
                 fillHisto(m_vx_chi2Over_ndf_ALL_fake, vertex->chiSquared()/ndf);

                 fillHisto(m_vx_z0_skewness_ALL_fake, z_skew);
                 fillHisto(m_vx_z0_kurtosis_ALL_fake, z_kurt);
                 fillHisto(m_vx_d0_skewness_ALL_fake, d_skew);
                 fillHisto(m_vx_d0_kurtosis_ALL_fake, d_kurt);
                 fillHisto(m_vx_pt_skewness_ALL_fake, pt_skew);
                 fillHisto(m_vx_pt_kurtosis_ALL_fake, pt_kurt);
                 for (const float& weight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_ALL_fake, weight);
                 }

             }
           }   // end of fake vertices
// Helen OTP count the number of vertices for each type per event
         

         if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::MATCHED) {
              if (vertex == bestRecoHSVtx_truth) {
                 number_matched_HS++;
              }
              else {
                 number_matched_PU++;         
              }
              double minpt = 20 ; // GeV
              if (sumPt/Gaudi::Units::GeV > minpt) {
                  number_matched++;
             }
        }

         if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::MERGED) {
              if (vertex == bestRecoHSVtx_truth) {
                 number_merged_HS++;
              }
              else {
                 number_merged_PU++;         
              }
              double minpt = 20 ; // GeV
              if (sumPt/Gaudi::Units::GeV > minpt) {
                  number_merged++;
             }
        }

         if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::SPLIT) {
              if (vertex == bestRecoHSVtx_truth) {
                 number_split_HS++;
              }
              else {
                 number_split_PU++;         
              }
              double minpt = 20 ; // GeV
              if (sumPt/Gaudi::Units::GeV > minpt) {
                  number_split++;    
             }
         }

        } // end of if (ndf != 0)
 


// Add new histos to check for number of tracks for each vertex type Helen OTP  
        for (const auto& vertex : vertexContainer.stdcont()) {
           if (vertex == bestRecoHSVtx_truth) {

         
             if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::MATCHED) {

                fillHisto(m_vx_ntracks_vs_HS_nmatchedvertices, vertex->nTrackParticles());
             } 

             if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::MERGED) {
                 fillHisto(m_vx_ntracks_vs_HS_nmergedvertices,  vertex->nTrackParticles());

             }

             if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::SPLIT) {
                 fillHisto(m_vx_ntracks_vs_HS_nsplitvertices,   vertex->nTrackParticles());
             }
             if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::FAKE) {
                 fillHisto(m_vx_ntracks_vs_HS_nfakevertices,    vertex->nTrackParticles());
             }
           }
           else {

         

             if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::MATCHED) {
              
                fillHisto(m_vx_ntracks_vs_nmatchedvertices, vertex->nTrackParticles());
             }

             if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::MERGED) {
                fillHisto(m_vx_ntracks_vs_nmergedvertices,  vertex->nTrackParticles());

             }

             if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::SPLIT) {
                 fillHisto(m_vx_ntracks_vs_nsplitvertices,   vertex->nTrackParticles());
             }  
             if (recoVtxMatchTypeInfo(*vertex) == InDetVertexTruthMatchUtils::VertexMatchType::FAKE) {
                 fillHisto(m_vx_ntracks_vs_nfakevertices,    vertex->nTrackParticles());
             }

           }
       
   
       } 
     //     std::cout << " End loop over vertices  helenmaguire";
        } // end loop over vertices

// Helen OTP - add new histos to count number of vertices per event
        fillHisto(m_vx_nVertices_ALL_matched, number_matched);
        fillHisto(m_vx_nVertices_ALL_merged, number_merged);
        fillHisto(m_vx_nVertices_ALL_split, number_split);
        fillHisto(m_vx_nVertices_HS_matched, number_matched_HS);
        fillHisto(m_vx_nVertices_HS_merged, number_merged_HS);
        fillHisto(m_vx_nVertices_HS_split, number_split_HS);
        fillHisto(m_vx_nVertices_matched, number_matched_PU);
        fillHisto(m_vx_nVertices_merged, number_merged_PU);
        fillHisto(m_vx_nVertices_split, number_split_PU);


        // Now fill plots relating to the reconstruction of our truth HS vertex (efficiency and resolutions)
        if (truthHSVertices.size() != 0) {
            localPUDensity = getLocalPUDensity(truthHSVtx, truthHSVertices, truthPUVertices);
            if (truthHSVtxRecoed) {
                float residual_z = truthHSVtx->z() - bestRecoHSVtx_truth->z();
                float residual_r = std::sqrt(std::pow(truthHSVtx->x() - bestRecoHSVtx_truth->x(), 2) + std::pow(truthHSVtx->y() - bestRecoHSVtx_truth->y(), 2));
                float residual_x = truthHSVtx->x() - bestRecoHSVtx_truth->x();
                float residual_y = truthHSVtx->y() - bestRecoHSVtx_truth->y();
                fillHisto(m_vx_hs_reco_eff, localPUDensity, 1);
                fillHisto(m_vx_hs_reco_long_reso, localPUDensity, getRecoLongitudinalReso(bestRecoHSVtx_truth));
                fillHisto(m_vx_hs_reco_trans_reso, localPUDensity, getRecoTransverseReso(bestRecoHSVtx_truth));
                fillHisto(m_vx_hs_truth_long_reso_vs_PU, localPUDensity, residual_z);
                fillHisto(m_vx_hs_truth_trans_reso_vs_PU, localPUDensity, residual_r);

                const AmgSymMatrix(3)& covariance = bestRecoHSVtx_truth->covariancePosition();
                float vtxerr_x = Amg::error(covariance, 0);
                float vtxerr_y = Amg::error(covariance, 1);
                float vtxerr_z = Amg::error(covariance, 2);

                if(fabs(vtxerr_z) > 1e-7) fillHisto(m_vx_hs_z_pull, residual_z/vtxerr_z);
                if(fabs(vtxerr_y) > 1e-7) fillHisto(m_vx_hs_y_pull, residual_y/vtxerr_y);
                if(fabs(vtxerr_x) > 1e-7) fillHisto(m_vx_hs_x_pull, residual_x/vtxerr_x);

                fillHisto(m_vx_hs_truth_z_res_vs_PU, localPUDensity, residual_z);
                fillHisto(m_vx_hs_truth_x_res_vs_PU, localPUDensity, residual_x);
                fillHisto(m_vx_hs_truth_y_res_vs_PU, localPUDensity, residual_y);

                fillHisto(m_vx_hs_z_res, residual_z);
                fillHisto(m_vx_hs_y_res, residual_y);
                fillHisto(m_vx_hs_x_res, residual_x);            

                fillHisto(m_vx_hs_truth_z_pull_vs_PU, localPUDensity, residual_z/vtxerr_z);
                fillHisto(m_vx_hs_truth_x_pull_vs_PU, localPUDensity, residual_x/vtxerr_y);
                fillHisto(m_vx_hs_truth_y_pull_vs_PU, localPUDensity, residual_y/vtxerr_x);

                fillHisto(m_vx_hs_truth_z_res_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_z);
                fillHisto(m_vx_hs_truth_x_res_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_x);
                fillHisto(m_vx_hs_truth_y_res_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_y);
            
                fillHisto(m_vx_hs_truth_z_pull_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_z/vtxerr_z);
                fillHisto(m_vx_hs_truth_x_pull_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_x/vtxerr_y);
                fillHisto(m_vx_hs_truth_y_pull_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_y/vtxerr_x);

            }
            else {
                fillHisto(m_vx_hs_reco_eff, localPUDensity, 0);
            }
        }

        fillHisto(m_vx_nReco_vs_nTruth_matched, nTruthVertices, breakdown[InDetVertexTruthMatchUtils::VertexMatchType::MATCHED]);
        fillHisto(m_vx_nReco_vs_nTruth_merged,  nTruthVertices, breakdown[InDetVertexTruthMatchUtils::VertexMatchType::MERGED]);
        fillHisto(m_vx_nReco_vs_nTruth_split,   nTruthVertices, breakdown[InDetVertexTruthMatchUtils::VertexMatchType::SPLIT]);
        fillHisto(m_vx_nReco_vs_nTruth_fake,    nTruthVertices, breakdown[InDetVertexTruthMatchUtils::VertexMatchType::FAKE]);
        fillHisto(m_vx_nReco_vs_nTruth_dummy,   nTruthVertices, breakdown[InDetVertexTruthMatchUtils::VertexMatchType::DUMMY]);


        
        // And by hardscatter type:
        InDetVertexTruthMatchUtils::HardScatterType hsType = InDetVertexTruthMatchUtils::classifyHardScatter(vertexContainer);
        fillHisto(m_vx_hs_classification, hsType);
        switch (hsType) {
            case InDetVertexTruthMatchUtils::HardScatterType::CLEAN: {
                fillHisto(m_vx_nReco_vs_nTruth_clean, nTruthVertices, nRecoVertices);
                break;
            }
            case InDetVertexTruthMatchUtils::HardScatterType::LOWPU: {
                fillHisto(m_vx_nReco_vs_nTruth_lowpu, nTruthVertices, nRecoVertices);
                break;
            }
            case InDetVertexTruthMatchUtils::HardScatterType::HIGHPU: {
                fillHisto(m_vx_nReco_vs_nTruth_highpu, nTruthVertices, nRecoVertices);
                break;
            }
            case InDetVertexTruthMatchUtils::HardScatterType::HSSPLIT: {
                fillHisto(m_vx_nReco_vs_nTruth_hssplit, nTruthVertices, nRecoVertices);
                break;
            }
            case InDetVertexTruthMatchUtils::HardScatterType::NONE: {
                fillHisto(m_vx_nReco_vs_nTruth_none, nTruthVertices, nRecoVertices);
                break;
            }
            default: {
                break;
            }
        }
    }

} // end InDetPerfPlot_VertexTruthMatching::fill(const xAOD::VertexContainer& vertexContainer, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices)

void InDetPerfPlot_VertexTruthMatching::finalizePlots() {

    if (m_iDetailLevel >= 200) {
        
//        std::cout << " finalizePlots helenmaguire    !!!*";

        fillResoHist(m_vx_hs_truth_long_reso, m_vx_hs_truth_long_reso_vs_PU);
        fillResoHist(m_vx_hs_truth_trans_reso, m_vx_hs_truth_trans_reso_vs_PU);
    }

} // end InDetPerfPlot_VertexTruthMatching::finalizePlots()
