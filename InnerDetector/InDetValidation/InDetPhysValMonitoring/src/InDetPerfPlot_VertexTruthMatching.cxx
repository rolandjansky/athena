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
#include "InDetPerfPlot_Vertex.h"
#include "InDetPerfPlot_nTracks.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

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
   // New Expert Histograms for vertex classifiations
    m_vx_ntracks_matched(nullptr),
    m_vx_ntracks_merged(nullptr),
    m_vx_ntracks_split(nullptr),
    m_vx_ntracks_HS_matched(nullptr),
    m_vx_ntracks_HS_merged(nullptr),
    m_vx_ntracks_HS_split(nullptr),
    m_vx_ntracks_ALL_matched(nullptr),
    m_vx_ntracks_ALL_merged(nullptr),
    m_vx_ntracks_ALL_split(nullptr),
    m_vx_sumpT_matched(nullptr),
    m_vx_sumpT_merged(nullptr),
    m_vx_sumpT_split(nullptr),
    m_vx_sumpT_HS_matched(nullptr),
    m_vx_sumpT_HS_merged(nullptr),
    m_vx_sumpT_HS_split(nullptr),
    
    m_vx_z_asym_matched(nullptr),
    m_vx_z_asym_merged(nullptr),
    m_vx_z_asym_split(nullptr),
    m_vx_z_asym_HS_matched(nullptr),
    m_vx_z_asym_HS_merged(nullptr),
    m_vx_z_asym_HS_split(nullptr),
    m_vx_z_asym_weighted_matched(nullptr),
    m_vx_z_asym_weighted_merged(nullptr),
    m_vx_z_asym_weighted_split(nullptr),
    m_vx_z_asym_weighted_HS_matched(nullptr),
    m_vx_z_asym_weighted_HS_merged(nullptr),
    m_vx_z_asym_weighted_HS_split(nullptr),

    m_vx_track_weight_matched(nullptr),
    m_vx_track_weight_merged(nullptr),
    m_vx_track_weight_split(nullptr),
    m_vx_track_weight_HS_matched(nullptr),
    m_vx_track_weight_HS_merged(nullptr),
    m_vx_track_weight_HS_split(nullptr),
    
    m_vx_normalised_track_weight_matched(nullptr),
    m_vx_normalised_track_weight_merged(nullptr),
    m_vx_normalised_track_weight_split(nullptr),
    m_vx_normalised_track_weight_HS_matched(nullptr),
    m_vx_normalised_track_weight_HS_merged(nullptr),
    m_vx_normalised_track_weight_HS_split(nullptr),

    m_vx_chi2Over_ndf_matched(nullptr),
    m_vx_chi2Over_ndf_merged(nullptr),
    m_vx_chi2Over_ndf_split(nullptr),
    m_vx_chi2Over_ndf_HS_matched(nullptr),
    m_vx_chi2Over_ndf_HS_merged(nullptr),
    m_vx_chi2Over_ndf_HS_split(nullptr),

    m_vx_z0_skewness_matched(nullptr),
    m_vx_z0_skewness_merged(nullptr),
    m_vx_z0_skewness_split(nullptr),
    m_vx_z0_skewness_HS_matched(nullptr),
    m_vx_z0_skewness_HS_merged(nullptr),
    m_vx_z0_skewness_HS_split(nullptr),

    m_vx_z0_kurtosis_matched(nullptr),
    m_vx_z0_kurtosis_merged(nullptr),
    m_vx_z0_kurtosis_split(nullptr),
    m_vx_z0_kurtosis_HS_matched(nullptr),
    m_vx_z0_kurtosis_HS_merged(nullptr),
    m_vx_z0_kurtosis_HS_split(nullptr),

    m_vx_sumpT_ALL_matched(nullptr),
    m_vx_sumpT_ALL_merged(nullptr),
    m_vx_sumpT_ALL_split(nullptr),
    m_vx_z_asym_ALL_matched(nullptr),
    m_vx_z_asym_ALL_merged(nullptr),
    m_vx_z_asym_ALL_split(nullptr),
    m_vx_z_asym_weighted_ALL_matched(nullptr),
    m_vx_z_asym_weighted_ALL_merged(nullptr),
    m_vx_z_asym_weighted_ALL_split(nullptr),
    
    m_vx_track_weight_ALL_matched(nullptr),
    m_vx_track_weight_ALL_merged(nullptr),
    m_vx_track_weight_ALL_split(nullptr),

    m_vx_normalised_track_weight_ALL_matched(nullptr),
    m_vx_normalised_track_weight_ALL_merged(nullptr),
    m_vx_normalised_track_weight_ALL_split(nullptr),

    m_vx_chi2Over_ndf_ALL_matched(nullptr),
    m_vx_chi2Over_ndf_ALL_merged(nullptr),
    m_vx_chi2Over_ndf_ALL_split(nullptr),

    m_vx_z0_skewness_ALL_matched(nullptr),
    m_vx_z0_skewness_ALL_merged(nullptr),
    m_vx_z0_skewness_ALL_split(nullptr),

    m_vx_z0_kurtosis_ALL_matched(nullptr),
    m_vx_z0_kurtosis_ALL_merged(nullptr),
    m_vx_z0_kurtosis_ALL_split(nullptr),

    m_vx_nVertices_ALL_matched(nullptr),
    m_vx_nVertices_ALL_merged(nullptr),
    m_vx_nVertices_ALL_split(nullptr),
    m_vx_nVertices_HS_matched(nullptr),
    m_vx_nVertices_HS_merged(nullptr),
    m_vx_nVertices_HS_split(nullptr),
    m_vx_nVertices_matched(nullptr),
    m_vx_nVertices_merged(nullptr),
    m_vx_nVertices_split(nullptr)


{
  // nop
}

void InDetPerfPlot_VertexTruthMatching::initializePlots() {

    book(m_vx_type_truth,"vx_type_truth");
    book(m_vx_z_diff,"vx_z_diff");
    book(m_vx_z_diff_pull,"vx_z_diff_pull");
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
     
	// book the new expert histos for vertex classifications
	book(m_vx_ntracks_matched,"vx_ntracks_matched");
    	book(m_vx_ntracks_merged,"vx_ntracks_merged");
    	book(m_vx_ntracks_split,"vx_ntracks_split");
	book(m_vx_ntracks_HS_matched,"vx_ntracks_HS_matched");
    	book(m_vx_ntracks_HS_merged,"vx_ntracks_HS_merged");
    	book(m_vx_ntracks_HS_split,"vx_ntracks_HS_split");
	book(m_vx_ntracks_ALL_matched,"vx_ntracks_ALL_matched");
    	book(m_vx_ntracks_ALL_merged,"vx_ntracks_ALL_merged");
    	book(m_vx_ntracks_ALL_split,"vx_ntracks_ALL_split");
        book(m_vx_sumpT_matched,"vx_sumpT_matched");
        book(m_vx_sumpT_merged,"vx_sumpT_merged");
        book(m_vx_sumpT_split,"vx_sumpT_split");
        book(m_vx_sumpT_HS_matched,"vx_sumpT_HS_matched");
        book(m_vx_sumpT_HS_merged,"vx_sumpT_HS_merged");
        book(m_vx_sumpT_HS_split,"vx_sumpT_HS_split");
        book(m_vx_sumpT_ALL_matched,"vx_sumpT_ALL_matched");
        book(m_vx_sumpT_ALL_merged,"vx_sumpT_ALL_merged");
        book(m_vx_sumpT_ALL_split,"vx_sumpT_ALL_split");
        
        book(m_vx_z_asym_matched,"vx_z_asym_matched");
        book(m_vx_z_asym_merged,"vx_z_asym_merged");
        book(m_vx_z_asym_split,"vx_z_asym_split");
        book(m_vx_z_asym_HS_matched,"vx_z_asym_HS_matched");
        book(m_vx_z_asym_HS_merged,"vx_z_asym_HS_merged");
        book(m_vx_z_asym_HS_split,"vx_z_asym_HS_split");
        book(m_vx_z_asym_ALL_matched,"vx_z_asym_ALL_matched");
        book(m_vx_z_asym_ALL_merged,"vx_z_asym_ALL_merged");
        book(m_vx_z_asym_ALL_split,"vx_z_asym_ALL_split");
        book(m_vx_z_asym_weighted_matched,"vx_z_asym_weighted_matched");
        book(m_vx_z_asym_weighted_merged,"vx_z_asym_weighted_merged");
        book(m_vx_z_asym_weighted_split,"vx_z_asym_weighted_split");
        book(m_vx_z_asym_weighted_HS_matched,"vx_z_asym_weighted_HS_matched");
        book(m_vx_z_asym_weighted_HS_merged,"vx_z_asym_weighted_HS_merged");
        book(m_vx_z_asym_weighted_HS_split,"vx_z_asym_weighted_HS_split");
        book(m_vx_z_asym_weighted_ALL_matched,"vx_z_asym_weighted_ALL_matched");
        book(m_vx_z_asym_weighted_ALL_merged,"vx_z_asym_weighted_ALL_merged");
        book(m_vx_z_asym_weighted_ALL_split,"vx_z_asym_weighted_ALL_split");
        
        book(m_vx_track_weight_matched, "vx_track_weight_matched");
        book(m_vx_track_weight_merged, "vx_track_weight_merged");
        book(m_vx_track_weight_split, "vx_track_weight_split");
        book(m_vx_track_weight_HS_matched, "vx_track_weight_HS_matched");
        book(m_vx_track_weight_HS_merged, "vx_track_weight_HS_merged");
        book(m_vx_track_weight_HS_split, "vx_track_weight_HS_split");
        book(m_vx_track_weight_ALL_matched, "vx_track_weight_ALL_matched");
        book(m_vx_track_weight_ALL_merged, "vx_track_weight_ALL_merged");
        book(m_vx_track_weight_ALL_split, "vx_track_weight_ALL_split");

        book(m_vx_normalised_track_weight_matched, "vx_normalised_track_weight_matched");
        book(m_vx_normalised_track_weight_merged, "vx_normalised_track_weight_merged");
        book(m_vx_normalised_track_weight_split, "vx_normalised_track_weight_split");
        book(m_vx_normalised_track_weight_HS_matched, "vx_normalised_track_weight_HS_matched");
        book(m_vx_normalised_track_weight_HS_merged, "vx_normalised_track_weight_HS_merged");
        book(m_vx_normalised_track_weight_HS_split, "vx_normalised_track_weight_HS_split");
        book(m_vx_normalised_track_weight_ALL_matched, "vx_normalised_track_weight_ALL_matched");
        book(m_vx_normalised_track_weight_ALL_merged, "vx_normalised_track_weight_ALL_merged");
        book(m_vx_normalised_track_weight_ALL_split, "vx_normalised_track_weight_ALL_split");

        book(m_vx_chi2Over_ndf_matched,"vx_chi2Over_ndf_matched");
        book(m_vx_chi2Over_ndf_merged,"vx_chi2Over_ndf_merged");
        book(m_vx_chi2Over_ndf_split,"vx_chi2Over_ndf_split");
        book(m_vx_chi2Over_ndf_HS_matched,"vx_chi2Over_ndf_HS_matched");
        book(m_vx_chi2Over_ndf_HS_merged,"vx_chi2Over_ndf_HS_merged");
        book(m_vx_chi2Over_ndf_HS_split,"vx_chi2Over_ndf_HS_split");
        book(m_vx_chi2Over_ndf_ALL_matched,"vx_chi2Over_ndf_ALL_matched");
        book(m_vx_chi2Over_ndf_ALL_merged,"vx_chi2Over_ndf_ALL_merged");
        book(m_vx_chi2Over_ndf_ALL_split,"vx_chi2Over_ndf_ALL_split");
        
        book(m_vx_z0_skewness_matched, "vx_z0_skewness_matched");
        book(m_vx_z0_skewness_merged, "vx_z0_skewness_merged");
        book(m_vx_z0_skewness_split, "vx_z0_skewness_split");
        book(m_vx_z0_skewness_HS_matched, "vx_z0_skewness_HS_matched");
        book(m_vx_z0_skewness_HS_merged, "vx_z0_skewness_HS_merged");
        book(m_vx_z0_skewness_HS_split, "vx_z0_skewness_HS_split");
        book(m_vx_z0_skewness_ALL_matched, "vx_z0_skewness_ALL_matched");
        book(m_vx_z0_skewness_ALL_merged, "vx_z0_skewness_ALL_merged");
        book(m_vx_z0_skewness_ALL_split, "vx_z0_skewness_ALL_split");
        book(m_vx_z0_kurtosis_matched,"vx_z0_kurtosis_matched");
        book(m_vx_z0_kurtosis_merged,"vx_z0_kurtosis_merged");
        book(m_vx_z0_kurtosis_split,"vx_z0_kurtosis_split");
        book(m_vx_z0_kurtosis_HS_matched,"vx_z0_kurtosis_HS_matched");
        book(m_vx_z0_kurtosis_HS_merged,"vx_z0_kurtosis_HS_merged");
        book(m_vx_z0_kurtosis_HS_split,"vx_z0_kurtosis_HS_split");
        book(m_vx_z0_kurtosis_ALL_matched,"vx_z0_kurtosis_ALL_matched");
        book(m_vx_z0_kurtosis_ALL_merged,"vx_z0_kurtosis_ALL_merged");
        book(m_vx_z0_kurtosis_ALL_split,"vx_z0_kurtosis_ALL_split");


        book(m_vx_nVertices_matched,"vx_nVertices_matched");
        book(m_vx_nVertices_merged,"vx_nVertices_merged");
        book(m_vx_nVertices_split, "vx_nVertices_split");
        book(m_vx_nVertices_HS_matched,"vx_nVertices_HS_matched");
        book(m_vx_nVertices_HS_merged,"vx_nVertices_HS_merged");
        book(m_vx_nVertices_HS_split,"vx_nVertices_HS_split");
        book(m_vx_nVertices_ALL_matched,"vx_nVertices_ALL_matched");
        book(m_vx_nVertices_ALL_merged,"vx_nVertices_ALL_merged");
        book(m_vx_nVertices_ALL_split,"vx_nVertices_ALL_split");

    }

}
const xAOD::Vertex* InDetPerfPlot_VertexTruthMatching::getHSRecoVertexSumPt2(const xAOD::VertexContainer& recoVertices) {
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

float InDetPerfPlot_VertexTruthMatching::getRecoLongitudinalReso(const xAOD::Vertex* recoVtx) {
    return std::sqrt(recoVtx->covariancePosition()(2, 2));
}

float InDetPerfPlot_VertexTruthMatching::getRecoTransverseReso(const xAOD::Vertex* recoVtx) {
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

        }

        resoHist->SetBinContent(i, itr_rms);
        resoHist->SetBinError(i, itr_rms_err);

    }
}

const xAOD::TruthVertex* InDetPerfPlot_VertexTruthMatching::getTruthVertex(const xAOD::Vertex* recoVtx) const {
    const xAOD::TruthVertex* truthVtx = nullptr;
    if (recoVtx) {
        const static xAOD::Vertex::Decorator<std::vector<InDetVertexTruthMatchUtils::VertexTruthMatchInfo>> truthMatchingInfos("TruthEventMatchingInfos");
        try{
            if (!truthMatchingInfos.isAvailable(*recoVtx)){
                ATH_MSG_WARNING("TruthEventMatchingInfos DECORATOR not available -- returning nullptr!");
                return truthVtx;
            }
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
        catch (SG::ExcBadAuxVar &){
            ATH_MSG_WARNING("TruthEventMatchingInfos DECORATOR yields empty vector -- returning nullptr!");
        }
    }
    return truthVtx;
}

void InDetPerfPlot_VertexTruthMatching::fill(const xAOD::Vertex& vertex, const xAOD::TruthVertex * tvrt, float weight) {
  // not sure how to deal with this type of histogram
    if(tvrt){
      float diff_z=vertex.z()-tvrt->z();
      const AmgSymMatrix(3)& covariance = vertex.covariancePosition();
      float err_z = fabs(Amg::error(covariance, 2)) > 1e-7 ? Amg::error(covariance, 2) : 1000.;
      fillHisto(m_vx_z_diff,diff_z, weight);
      fillHisto(m_vx_z_diff_pull,diff_z/err_z, weight);
    }

    // Get the match type info for each vertex:
    const static xAOD::Vertex::Decorator<InDetVertexTruthMatchUtils::VertexMatchType> recoVtxMatchTypeInfo("VertexMatchType");
    InDetVertexTruthMatchUtils::VertexMatchType matchType;
    if (recoVtxMatchTypeInfo.isAvailable(vertex)) {
        try {
            matchType = recoVtxMatchTypeInfo(vertex);
            ATH_MSG_DEBUG("VERTEX DECORATOR ======= " << matchType << ", with nTRACKS === " << vertex.nTrackParticles() << ", vertex index = " << vertex.index() << " AT (x, y, z) = (" << vertex.x() << ", " << vertex.y() << ", " << vertex.z() << ")");
            fillHisto(m_vx_type_truth, matchType, weight);
        }
        catch (SG::ExcBadAuxVar &) {
            ATH_MSG_WARNING("VertexMatchType DECORATOR seems to be available, but may be broken  ===========");
        } 
    }
    else {
        ATH_MSG_WARNING("VertexMatchType DECORATOR is NOT available  ===========");
    }

} // void InDetPerfPlot_VertexTruthMatching::fill(const xAOD::Vertex& vertex) {

void InDetPerfPlot_VertexTruthMatching::fill(const xAOD::Vertex* recoHardScatter,const xAOD::VertexContainer& vertexContainer, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices, float weight) {

    if (m_iDetailLevel >= 200) {
        // Fill our histograms
        // Inclusive:
        int nTruthVertices = (int)(truthHSVertices.size() + truthPUVertices.size());
        int nRecoVertices = (int)vertexContainer.size();
        fillHisto(m_vx_nReco_vs_nTruth_inclusive, nTruthVertices, nRecoVertices, weight);
        
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

        // Best reco HS vertex identified via truth HS weights
        const xAOD::Vertex* bestRecoHSVtx_truth = InDetVertexTruthMatchUtils::bestHardScatterMatch(vertexContainer);
        if (!bestRecoHSVtx_truth){
            ATH_MSG_INFO("No bestRecoHS vertex - not filling vertex truth matching.");
            return;
        }

       // add code to add protection against non-identified reconstructed HS vertex to avoid crashes in samples with low track multiplicity HS vertex
       if (!bestRecoHSVtx_truth){
            ATH_MSG_INFO("No bestRecoHS vertex - not filling vertex truth matching.");
            return;
        }



        // Did we correctly select the best reco HS vertex using sumpt2?
        truthVtx = getTruthVertex(recoHardScatter);
        if (!truthVtx){
            ATH_MSG_INFO("No truth HS - not filling vertex truth matching."); 
            return;
        }
        localPUDensity = getLocalPUDensity(truthVtx, truthHSVertices, truthPUVertices);
        fillHisto(m_vx_hs_sel_eff, localPUDensity, (recoHardScatter == bestRecoHSVtx_truth), weight);

        // Did we successfully reconstruct our truth HS vertex?
        bool truthHSVtxRecoed = false;
        float minTruthRecoRadialDiff2 = std::pow(m_cutMinTruthRecoRadialDiff, 2);
        float truthRecoRadialDiff2 = -1.;
        const xAOD::TruthVertex* truthHSVtx = nullptr;
        // Check that we have *exactly* 1 truth HS vertex
        if (!truthHSVertices.empty()) {
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
    
            fillHisto(m_vx_all_z_pull, residual_z/vtxerr_z, weight);
            fillHisto(m_vx_all_y_pull, residual_y/vtxerr_y, weight);
            fillHisto(m_vx_all_x_pull, residual_x/vtxerr_x, weight);

            fillHisto(m_vx_all_truth_z_res_vs_PU, localPUDensity, residual_z, weight);
            fillHisto(m_vx_all_truth_x_res_vs_PU, localPUDensity, residual_x, weight);
            fillHisto(m_vx_all_truth_y_res_vs_PU, localPUDensity, residual_y, weight);

            fillHisto(m_vx_all_z_res, residual_z, weight);
            fillHisto(m_vx_all_y_res, residual_y, weight);
            fillHisto(m_vx_all_x_res, residual_x, weight);
            
            fillHisto(m_vx_all_truth_z_pull_vs_PU, localPUDensity, residual_z/vtxerr_z, weight);
            fillHisto(m_vx_all_truth_x_pull_vs_PU, localPUDensity, residual_x/vtxerr_x, weight);
            fillHisto(m_vx_all_truth_y_pull_vs_PU, localPUDensity, residual_y/vtxerr_y, weight);

            fillHisto(m_vx_all_truth_z_res_vs_nTrk, vertex->nTrackParticles(), residual_z, weight);
            fillHisto(m_vx_all_truth_x_res_vs_nTrk, vertex->nTrackParticles(), residual_x, weight);
            fillHisto(m_vx_all_truth_y_res_vs_nTrk, vertex->nTrackParticles(), residual_y, weight);
            
            fillHisto(m_vx_all_truth_z_pull_vs_nTrk, vertex->nTrackParticles(), residual_z/vtxerr_z, weight);
            fillHisto(m_vx_all_truth_x_pull_vs_nTrk, vertex->nTrackParticles(), residual_x/vtxerr_x, weight);
            fillHisto(m_vx_all_truth_y_pull_vs_nTrk, vertex->nTrackParticles(), residual_y/vtxerr_y, weight);




       // New Expert histograms for observables for vertex classifications for HS and PU 
       // For each vertex, loop over all tracks and get sumpt and sum of charges
       // also use this to get the z asymmetry around the vertex.
   
         // Declaring variables for the observables        
         const xAOD::TrackParticle* trackTmp = nullptr;
         float sumPt =0;
         float minpt = 20000 ; // minimum sum pt required for the 'All' vertices plots - 20 GeV
         float trackPt = 0;

         // variables for calculation of delta Z asymmetry and delta d asymmetry
         float z_asym = 0;
         float sumDZ = 0;
         float deltaZ =0;
         float modsumDZ =0;
         float weighted_sumDZ = 0;
         float weighted_deltaZ = 0;
         float weighted_modsumDZ = 0;
         float weighted_z_asym =0;

         // make vector
         std::vector<float> track_deltaZ;
         std::vector<float> track_deltaPt;
         std::vector<float> track_deltaZ_weighted;
         // loop over tracks 
         for (size_t i = 0; i < vertex->nTrackParticles(); i++) {
            trackTmp = vertex->trackParticle(i);
            trackPt = trackTmp->pt();  //  MeV 
            sumPt = sumPt + trackPt;  // in MeV
            deltaZ = trackTmp->z0() - vertex->z();
        
            if (trackTmp) {
                 track_deltaZ.push_back(deltaZ);
                 // get the track weight for each track to get the deltaZ/trk_weight
                 float trk_weight = vertex->trackWeight(i);
                 weighted_deltaZ = deltaZ*trk_weight;
                 // sum of delta z
                 sumDZ = sumDZ + deltaZ; 
                 modsumDZ = modsumDZ + std::abs(deltaZ);
                 weighted_sumDZ =  weighted_sumDZ + weighted_deltaZ;
                 weighted_modsumDZ = weighted_modsumDZ + std::abs(weighted_deltaZ);
               
            }
           } // end loop over tracks
            if  (modsumDZ >0) {
                z_asym = sumDZ/modsumDZ;
            }
            if  (weighted_modsumDZ >0) {
                weighted_z_asym = weighted_sumDZ/weighted_modsumDZ;
            }


          
            double mean_Dz =0;
            mean_Dz=sumDZ/track_deltaZ.size(); //calculate average
            double number_tracks =0;
            number_tracks = track_deltaZ.size(); // get number of tracks 

            double z_sd = 0; //  standard deviation
            double z_skew = 0;  // skewness of DeltaZ asymmetry
            double z_kurt = 0;  // Kurtosis of DeltaZ asymmetry 
            double z_var=0;     // variance of DeltaZ
            double z_zbar=0;    // for use in calculation below

            for ( auto i : track_deltaZ) {
      
               z_zbar = (i - mean_Dz);
               z_var =(z_var + z_zbar*z_zbar);
               z_skew =(z_skew + z_zbar*z_zbar*z_zbar);
               z_kurt =(z_kurt + z_zbar*z_zbar*z_zbar*z_zbar);

            } 
               z_var = z_var/(number_tracks -1);
               z_sd = std::sqrt(z_var);
               z_skew = z_skew/((number_tracks -1)*z_sd*z_sd*z_sd);
               z_kurt = z_kurt/((number_tracks -1)*z_sd*z_sd*z_sd*z_sd);

            float ndf = vertex->numberDoF();
            if (ndf != 0) {

              if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::MATCHED) {
               if (vertex == bestRecoHSVtx_truth) {
	
		 fillHisto(m_vx_sumpT_HS_matched,sumPt ,weight);
                 fillHisto(m_vx_z_asym_HS_matched, z_asym,weight);
                 fillHisto(m_vx_z_asym_weighted_HS_matched, weighted_z_asym,weight);
                 fillHisto(m_vx_chi2Over_ndf_HS_matched, vertex->chiSquared()/ndf,weight);

                 fillHisto(m_vx_z0_skewness_HS_matched, z_skew,weight);
                 fillHisto(m_vx_z0_kurtosis_HS_matched, z_kurt,weight);
       

                 for (const float& trkWeight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_HS_matched, trkWeight,weight);
                     fillHisto(m_vx_normalised_track_weight_HS_matched, trkWeight/number_tracks,weight);
                 }

               }	
	       else { 
		
                 fillHisto(m_vx_sumpT_matched,sumPt ,weight);
                 fillHisto(m_vx_z_asym_matched, z_asym,weight);
                 fillHisto(m_vx_z_asym_weighted_matched, weighted_z_asym,weight);
                 fillHisto(m_vx_chi2Over_ndf_matched, vertex->chiSquared()/ndf,weight);
              
                 fillHisto(m_vx_z0_skewness_matched, z_skew,weight);
                 fillHisto(m_vx_z0_kurtosis_matched, z_kurt,weight);


                 for (const float& trkWeight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_matched, trkWeight,weight);
                     fillHisto(m_vx_normalised_track_weight_matched, trkWeight/number_tracks,weight);
                 }
               }
// fill some histograms that contain both HS and PU above a min pt - say 20GeV
              if (sumPt > minpt) {
                 fillHisto(m_vx_sumpT_ALL_matched,sumPt ,weight);
                 fillHisto(m_vx_z_asym_ALL_matched, z_asym,weight);
                 fillHisto(m_vx_z_asym_weighted_ALL_matched, weighted_z_asym,weight);
                 fillHisto(m_vx_chi2Over_ndf_ALL_matched, vertex->chiSquared()/ndf,weight);

                 fillHisto(m_vx_z0_skewness_ALL_matched, z_skew,weight);
                 fillHisto(m_vx_z0_kurtosis_ALL_matched, z_kurt,weight);
                 for (const float& trkWeight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_ALL_matched, trkWeight,weight);
                     fillHisto(m_vx_normalised_track_weight_ALL_matched, trkWeight/number_tracks,weight);
                 }

              }
             }  // end of if matched vertices

             else if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::MERGED) {
              if (vertex == bestRecoHSVtx_truth) {
              
                 fillHisto(m_vx_sumpT_HS_merged, sumPt ,weight);
                 fillHisto(m_vx_z_asym_HS_merged, z_asym,weight);
                 fillHisto(m_vx_z_asym_weighted_HS_merged, weighted_z_asym,weight);
                 fillHisto(m_vx_chi2Over_ndf_HS_merged, vertex->chiSquared()/ndf,weight);

                 fillHisto(m_vx_z0_skewness_HS_merged, z_skew,weight);
                 fillHisto(m_vx_z0_kurtosis_HS_merged, z_kurt,weight);
                 for (const float& trkWeight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_HS_merged, trkWeight,weight);
                     fillHisto(m_vx_normalised_track_weight_HS_merged, trkWeight/number_tracks,weight);
                 }



              }
              else {
                 fillHisto(m_vx_sumpT_merged, sumPt ,weight);
                 fillHisto(m_vx_z_asym_merged, z_asym,weight);
                 fillHisto(m_vx_z_asym_weighted_merged, weighted_z_asym,weight);
                 fillHisto(m_vx_chi2Over_ndf_merged, vertex->chiSquared()/ndf,weight);
             
                 fillHisto(m_vx_z0_skewness_merged, z_skew,weight);
                 fillHisto(m_vx_z0_kurtosis_merged, z_kurt,weight);
                 for (const float& trkWeight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_merged, trkWeight,weight);
                     fillHisto(m_vx_normalised_track_weight_merged, trkWeight/number_tracks,weight);
                 }

              }
              if (sumPt > minpt) {
                 fillHisto(m_vx_sumpT_ALL_merged,sumPt ,weight);
                 fillHisto(m_vx_z_asym_ALL_merged, z_asym,weight);
                 fillHisto(m_vx_z_asym_weighted_ALL_merged, weighted_z_asym,weight);
                 fillHisto(m_vx_chi2Over_ndf_ALL_merged, vertex->chiSquared()/ndf,weight);

                 fillHisto(m_vx_z0_skewness_ALL_merged, z_skew,weight);
                 fillHisto(m_vx_z0_kurtosis_ALL_merged, z_kurt,weight);
                 for (const float& trkWeight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_ALL_merged, trkWeight,weight);
                     fillHisto(m_vx_normalised_track_weight_ALL_merged, trkWeight/number_tracks,weight);
                 }

              }
             }   //end of if merged vertices

             else if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::SPLIT) {
              if (vertex == bestRecoHSVtx_truth) {
                 fillHisto(m_vx_sumpT_HS_split, sumPt ,weight);
                 fillHisto(m_vx_z_asym_HS_split, z_asym,weight);
                 fillHisto(m_vx_z_asym_weighted_HS_split, weighted_z_asym,weight);
                 fillHisto(m_vx_chi2Over_ndf_HS_split, vertex->chiSquared()/ndf,weight);

                 fillHisto(m_vx_z0_skewness_HS_split, z_skew,weight);
                 fillHisto(m_vx_z0_kurtosis_HS_split, z_kurt,weight);
                 for (const float& trkWeight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_HS_split, trkWeight,weight);
                     fillHisto(m_vx_normalised_track_weight_HS_split, trkWeight/number_tracks,weight);
                 }


              }
              else {
                 fillHisto(m_vx_sumpT_split, sumPt ,weight);
                 fillHisto(m_vx_z_asym_split, z_asym,weight);
                 fillHisto(m_vx_z_asym_weighted_split, weighted_z_asym,weight);
                 fillHisto(m_vx_chi2Over_ndf_split, vertex->chiSquared()/ndf,weight);

                 fillHisto(m_vx_z0_skewness_split, z_skew,weight);
                 fillHisto(m_vx_z0_kurtosis_split, z_kurt,weight);
                 for (const float& trkWeight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_split, trkWeight,weight);
                     fillHisto(m_vx_normalised_track_weight_split, trkWeight/number_tracks,weight);
                 }


              }

              if (sumPt > minpt) {
                 fillHisto(m_vx_sumpT_ALL_split,sumPt ,weight);
                 fillHisto(m_vx_z_asym_ALL_split, z_asym,weight);
                 fillHisto(m_vx_z_asym_weighted_ALL_split, weighted_z_asym,weight);
                 fillHisto(m_vx_chi2Over_ndf_ALL_split, vertex->chiSquared()/ndf,weight);

                 fillHisto(m_vx_z0_skewness_ALL_split, z_skew,weight);
                 fillHisto(m_vx_z0_kurtosis_ALL_split, z_kurt,weight);
                 for (const float& trkWeight : vertex->trackWeights()) {
                     fillHisto(m_vx_track_weight_ALL_split, trkWeight,weight);
                     fillHisto(m_vx_normalised_track_weight_ALL_split, trkWeight/number_tracks,weight);
                 }
              }

	     }   // end of if split vertices

// Count the number of vertices for each type per event
         

         if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::MATCHED) {
              if (vertex == bestRecoHSVtx_truth) {
                 number_matched_HS++;
              }
              else {
                 number_matched_PU++;         
              }
              if (sumPt > minpt) {
                  number_matched++;
             }
         }

         else if (matchType  == InDetVertexTruthMatchUtils::VertexMatchType::MERGED) {
              if (vertex == bestRecoHSVtx_truth) {
                 number_merged_HS++;
              }
              else {
                 number_merged_PU++;         
              }
              if (sumPt > minpt) {
                  number_merged++;
             }
         }

         else if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::SPLIT) {
              if (vertex == bestRecoHSVtx_truth) {
                 number_split_HS++;
              }
              else {
                 number_split_PU++;         
              }
              if (sumPt > minpt) {
                  number_split++;    
             }
         }

        } // end of if (ndf != 0)
 


// New histos to check for number of tracks for each vertex type   
        for (const auto& vertex : vertexContainer.stdcont()) {
           if (vertex == bestRecoHSVtx_truth) {

         
             if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::MATCHED) {

                fillHisto(m_vx_ntracks_HS_matched, vertex->nTrackParticles(),weight);
             } 

             else if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::MERGED) {
                 fillHisto(m_vx_ntracks_HS_merged,  vertex->nTrackParticles(),weight);

             }

             else if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::SPLIT) {
                 fillHisto(m_vx_ntracks_HS_split,   vertex->nTrackParticles(),weight);
             }
           }
           else {


             if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::MATCHED) {
              
                fillHisto(m_vx_ntracks_matched, vertex->nTrackParticles(),weight);
             }

             else if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::MERGED) {
                fillHisto(m_vx_ntracks_merged,  vertex->nTrackParticles(),weight);

             }

             else if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::SPLIT) {
                 fillHisto(m_vx_ntracks_split,   vertex->nTrackParticles(),weight);
             }  

           }
           if (sumPt > minpt) {
             if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::MATCHED) {
              
                fillHisto(m_vx_ntracks_ALL_matched, vertex->nTrackParticles(),weight);
             }

             else if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::MERGED) {
                fillHisto(m_vx_ntracks_ALL_merged,  vertex->nTrackParticles(),weight);

             }

             else if (matchType == InDetVertexTruthMatchUtils::VertexMatchType::SPLIT) {
                 fillHisto(m_vx_ntracks_ALL_split,   vertex->nTrackParticles(),weight);
             }  

           }
   
        } 
       } // end loop over vertices

// new histos to count number of vertices per event
        fillHisto(m_vx_nVertices_ALL_matched, number_matched,weight);
        fillHisto(m_vx_nVertices_ALL_merged, number_merged,weight);
        fillHisto(m_vx_nVertices_ALL_split, number_split,weight);
        fillHisto(m_vx_nVertices_HS_matched, number_matched_HS,weight);
        fillHisto(m_vx_nVertices_HS_merged, number_merged_HS,weight);
        fillHisto(m_vx_nVertices_HS_split, number_split_HS,weight);
        fillHisto(m_vx_nVertices_matched, number_matched_PU,weight);
        fillHisto(m_vx_nVertices_merged, number_merged_PU,weight);
        fillHisto(m_vx_nVertices_split, number_split_PU,weight);

        // Now fill plots relating to the reconstruction of our truth HS vertex (efficiency and resolutions)
        if (!truthHSVertices.empty()) {
            localPUDensity = getLocalPUDensity(truthHSVtx, truthHSVertices, truthPUVertices);
            if (truthHSVtxRecoed) {
                float residual_z = truthHSVtx->z() - bestRecoHSVtx_truth->z();
                float residual_r = std::sqrt(std::pow(truthHSVtx->x() - bestRecoHSVtx_truth->x(), 2) + std::pow(truthHSVtx->y() - bestRecoHSVtx_truth->y(), 2));
                float residual_x = truthHSVtx->x() - bestRecoHSVtx_truth->x();
                float residual_y = truthHSVtx->y() - bestRecoHSVtx_truth->y();
                fillHisto(m_vx_hs_reco_eff, localPUDensity, 1, weight);
                fillHisto(m_vx_hs_reco_long_reso, localPUDensity, getRecoLongitudinalReso(bestRecoHSVtx_truth), weight);
                fillHisto(m_vx_hs_reco_trans_reso, localPUDensity, getRecoTransverseReso(bestRecoHSVtx_truth), weight);
                fillHisto(m_vx_hs_truth_long_reso_vs_PU, localPUDensity, residual_z, weight);
                fillHisto(m_vx_hs_truth_trans_reso_vs_PU, localPUDensity, residual_r, weight);

                const AmgSymMatrix(3)& covariance = bestRecoHSVtx_truth->covariancePosition();
                float vtxerr_x = Amg::error(covariance, 0);
                float vtxerr_y = Amg::error(covariance, 1);
                float vtxerr_z = Amg::error(covariance, 2);

                if(fabs(vtxerr_z) > 1e-7) fillHisto(m_vx_hs_z_pull, residual_z/vtxerr_z, weight);
                if(fabs(vtxerr_y) > 1e-7) fillHisto(m_vx_hs_y_pull, residual_y/vtxerr_y, weight);
                if(fabs(vtxerr_x) > 1e-7) fillHisto(m_vx_hs_x_pull, residual_x/vtxerr_x, weight);

                fillHisto(m_vx_hs_truth_z_res_vs_PU, localPUDensity, residual_z, weight);
                fillHisto(m_vx_hs_truth_x_res_vs_PU, localPUDensity, residual_x, weight);
                fillHisto(m_vx_hs_truth_y_res_vs_PU, localPUDensity, residual_y, weight);

                fillHisto(m_vx_hs_z_res, residual_z, weight);
                fillHisto(m_vx_hs_y_res, residual_y, weight);
                fillHisto(m_vx_hs_x_res, residual_x, weight);

                fillHisto(m_vx_hs_truth_z_pull_vs_PU, localPUDensity, residual_z/vtxerr_z, weight);
                fillHisto(m_vx_hs_truth_x_pull_vs_PU, localPUDensity, residual_x/vtxerr_x, weight);
                fillHisto(m_vx_hs_truth_y_pull_vs_PU, localPUDensity, residual_y/vtxerr_y, weight);

                fillHisto(m_vx_hs_truth_z_res_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_z, weight);
                fillHisto(m_vx_hs_truth_x_res_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_x, weight);
                fillHisto(m_vx_hs_truth_y_res_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_y, weight);
            
                fillHisto(m_vx_hs_truth_z_pull_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_z/vtxerr_z, weight);
                fillHisto(m_vx_hs_truth_x_pull_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_x/vtxerr_x, weight);
                fillHisto(m_vx_hs_truth_y_pull_vs_nTrk, bestRecoHSVtx_truth->nTrackParticles(), residual_y/vtxerr_y, weight);

            }
            else {
                fillHisto(m_vx_hs_reco_eff, localPUDensity, 0, weight);
            }
        }

        fillHisto(m_vx_nReco_vs_nTruth_matched, nTruthVertices, breakdown[InDetVertexTruthMatchUtils::VertexMatchType::MATCHED], weight);
        fillHisto(m_vx_nReco_vs_nTruth_merged,  nTruthVertices, breakdown[InDetVertexTruthMatchUtils::VertexMatchType::MERGED], weight);
        fillHisto(m_vx_nReco_vs_nTruth_split,   nTruthVertices, breakdown[InDetVertexTruthMatchUtils::VertexMatchType::SPLIT], weight);
        fillHisto(m_vx_nReco_vs_nTruth_fake,    nTruthVertices, breakdown[InDetVertexTruthMatchUtils::VertexMatchType::FAKE], weight);
        fillHisto(m_vx_nReco_vs_nTruth_dummy,   nTruthVertices, breakdown[InDetVertexTruthMatchUtils::VertexMatchType::DUMMY], weight);
        
        // And by hardscatter type:
        InDetVertexTruthMatchUtils::HardScatterType hsType = InDetVertexTruthMatchUtils::classifyHardScatter(vertexContainer);
        fillHisto(m_vx_hs_classification, hsType, weight);
        switch (hsType) {
            case InDetVertexTruthMatchUtils::HardScatterType::CLEAN: {
                fillHisto(m_vx_nReco_vs_nTruth_clean, nTruthVertices, nRecoVertices, weight);
                break;
            }
            case InDetVertexTruthMatchUtils::HardScatterType::LOWPU: {
                fillHisto(m_vx_nReco_vs_nTruth_lowpu, nTruthVertices, nRecoVertices, weight);
                break;
            }
            case InDetVertexTruthMatchUtils::HardScatterType::HIGHPU: {
                fillHisto(m_vx_nReco_vs_nTruth_highpu, nTruthVertices, nRecoVertices, weight);
                break;
            }
            case InDetVertexTruthMatchUtils::HardScatterType::HSSPLIT: {
                fillHisto(m_vx_nReco_vs_nTruth_hssplit, nTruthVertices, nRecoVertices, weight);
                break;
            }
            case InDetVertexTruthMatchUtils::HardScatterType::NONE: {
                fillHisto(m_vx_nReco_vs_nTruth_none, nTruthVertices, nRecoVertices, weight);
                break;
            }
            default: {
                break;
            }
        }  // End of switch
    }   // end of EXpert plots - (if (m_iDetailLevel >= 200))

} // end InDetPerfPlot_VertexTruthMatching::fill(const xAOD::VertexContainer& vertexContainer, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices)

void InDetPerfPlot_VertexTruthMatching::finalizePlots() {

    if (m_iDetailLevel >= 200) {
        
        fillResoHist(m_vx_hs_truth_long_reso, m_vx_hs_truth_long_reso_vs_PU);
        fillResoHist(m_vx_hs_truth_trans_reso, m_vx_hs_truth_trans_reso_vs_PU);
    }

} // end InDetPerfPlot_VertexTruthMatching::finalizePlots()
