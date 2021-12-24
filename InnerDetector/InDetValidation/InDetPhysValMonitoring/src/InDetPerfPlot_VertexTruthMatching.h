/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_VERTEXTRUTHMATCHING
#define INDETPHYSVALMONITORING_INDETPERFPLOT_VERTEXTRUTHMATCHING
/**
 * @file InDetPerfPlot_VertexTruthMatching.h
 * @author Valentina Cairo, mbasso
 **/

// local includes
#include "InDetPlotBase.h"
#include "InDetPhysValMonitoringUtilities.h"

// Tracking includes:
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexFwd.h"

// xAOD truth object includes:
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthVertex.h"

// Vertex validation:
#include "InDetTruthVertexValidation/InDetVertexTruthMatchTool.h"
#include "InDetTruthVertexValidation/InDetVertexTruthMatchUtils.h"

// std includes
#include <string>

// root includes
class TH1;
class TH2;
class TProfile;
class TEfficiency;


///class holding plots for truth matched vertices
class InDetPerfPlot_VertexTruthMatching: public InDetPlotBase {
public:
    InDetPerfPlot_VertexTruthMatching(InDetPlotBase* pParent, const std::string& dirName, const int iDetailLevel = 10);
    void fill(const xAOD::Vertex& vertex, const xAOD::TruthVertex * tvrt =0, float weight=1.0);
    void fill(const xAOD::Vertex* recoHardScatter, const xAOD::VertexContainer& vertexContainer, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices, float weight=1.0);
private:
    int m_iDetailLevel;
    float m_cutMinTruthRecoRadialDiff = 0.1;
    ///truth type
    TH1* m_vx_type_truth{};
    TH1* m_vx_z_diff{};
    TH1* m_vx_z_diff_pull{};
    ///hardscatter classification
    TH1* m_vx_hs_classification{};
    ///vertex reco efficiency
    TProfile* m_vx_nReco_vs_nTruth_inclusive;
    TProfile* m_vx_nReco_vs_nTruth_matched;
    TProfile* m_vx_nReco_vs_nTruth_merged;
    TProfile* m_vx_nReco_vs_nTruth_split;
    TProfile* m_vx_nReco_vs_nTruth_fake;
    TProfile* m_vx_nReco_vs_nTruth_dummy;
    TProfile* m_vx_nReco_vs_nTruth_clean;
    TProfile* m_vx_nReco_vs_nTruth_lowpu;
    TProfile* m_vx_nReco_vs_nTruth_highpu;
    TProfile* m_vx_nReco_vs_nTruth_hssplit;
    TProfile* m_vx_nReco_vs_nTruth_none;
    // HS vertex reconstruction efficiency vs PU
    TEfficiency* m_vx_hs_reco_eff;
    // HS vertex selection efficiency vs PU
    TEfficiency* m_vx_hs_sel_eff;
    // For reco (covariance) resolutions:
    TProfile* m_vx_hs_reco_long_reso;
    TProfile* m_vx_hs_reco_trans_reso;
    // For reco-truth resolutions:
    TH2* m_vx_hs_truth_long_reso_vs_PU;
    TH2* m_vx_hs_truth_trans_reso_vs_PU;

    TH1* m_vx_hs_truth_long_reso;
    TH1* m_vx_hs_truth_trans_reso;
     
    TH1* m_vx_hs_z_pull;
    TH1* m_vx_hs_y_pull;
    TH1* m_vx_hs_x_pull;
    TH1* m_vx_all_z_pull;
    TH1* m_vx_all_y_pull;
    TH1* m_vx_all_x_pull;
    
    TH1* m_vx_hs_z_res;
    TH1* m_vx_hs_y_res;
    TH1* m_vx_hs_x_res;
    TH1* m_vx_all_z_res;
    TH1* m_vx_all_y_res;
    TH1* m_vx_all_x_res;

    TH2* m_vx_all_truth_z_res_vs_PU;
    TH2* m_vx_all_truth_x_res_vs_PU;
    TH2* m_vx_all_truth_y_res_vs_PU;
    TH2* m_vx_all_truth_z_pull_vs_PU;
    TH2* m_vx_all_truth_x_pull_vs_PU;
    TH2* m_vx_all_truth_y_pull_vs_PU;

    TH2* m_vx_all_truth_z_res_vs_nTrk;
    TH2* m_vx_all_truth_x_res_vs_nTrk;
    TH2* m_vx_all_truth_y_res_vs_nTrk;
    TH2* m_vx_all_truth_z_pull_vs_nTrk;
    TH2* m_vx_all_truth_x_pull_vs_nTrk;
    TH2* m_vx_all_truth_y_pull_vs_nTrk;

    TH2* m_vx_hs_truth_z_res_vs_PU;
    TH2* m_vx_hs_truth_x_res_vs_PU;
    TH2* m_vx_hs_truth_y_res_vs_PU;
    TH2* m_vx_hs_truth_z_pull_vs_PU;
    TH2* m_vx_hs_truth_x_pull_vs_PU;
    TH2* m_vx_hs_truth_y_pull_vs_PU;

    TH2* m_vx_hs_truth_z_res_vs_nTrk;
    TH2* m_vx_hs_truth_x_res_vs_nTrk;
    TH2* m_vx_hs_truth_y_res_vs_nTrk;
    TH2* m_vx_hs_truth_z_pull_vs_nTrk;
    TH2* m_vx_hs_truth_x_pull_vs_nTrk;
    TH2* m_vx_hs_truth_y_pull_vs_nTrk;
    // New Histograms for Expert Plots for observables for different vertex classifications
    TH1* m_vx_ntracks_matched;
    TH1* m_vx_ntracks_merged;
    TH1* m_vx_ntracks_split;
    TH1* m_vx_ntracks_HS_matched;
    TH1* m_vx_ntracks_HS_merged;
    TH1* m_vx_ntracks_HS_split;
    TH1* m_vx_ntracks_ALL_matched;
    TH1* m_vx_ntracks_ALL_merged;
    TH1* m_vx_ntracks_ALL_split;
    TH1* m_vx_sumpT_matched;
    TH1* m_vx_sumpT_merged;
    TH1* m_vx_sumpT_split;
    TH1* m_vx_sumpT_HS_matched;
    TH1* m_vx_sumpT_HS_merged;
    TH1* m_vx_sumpT_HS_split;
    
    TH1* m_vx_z_asym_matched;
    TH1* m_vx_z_asym_merged;
    TH1* m_vx_z_asym_split;
    TH1* m_vx_z_asym_HS_matched;
    TH1* m_vx_z_asym_HS_merged;
    TH1* m_vx_z_asym_HS_split;
    TH1* m_vx_z_asym_weighted_matched;
    TH1* m_vx_z_asym_weighted_merged;
    TH1* m_vx_z_asym_weighted_split;
    TH1* m_vx_z_asym_weighted_HS_matched;
    TH1* m_vx_z_asym_weighted_HS_merged;
    TH1* m_vx_z_asym_weighted_HS_split;
    
    TH1* m_vx_track_weight_matched;
    TH1* m_vx_track_weight_merged;
    TH1* m_vx_track_weight_split;
    TH1* m_vx_track_weight_HS_matched;
    TH1* m_vx_track_weight_HS_merged;
    TH1* m_vx_track_weight_HS_split;

    TH1* m_vx_normalised_track_weight_matched;
    TH1* m_vx_normalised_track_weight_merged;
    TH1* m_vx_normalised_track_weight_split;
    TH1* m_vx_normalised_track_weight_HS_matched;
    TH1* m_vx_normalised_track_weight_HS_merged;
    TH1* m_vx_normalised_track_weight_HS_split;

    TH1* m_vx_chi2Over_ndf_matched;
    TH1* m_vx_chi2Over_ndf_merged;
    TH1* m_vx_chi2Over_ndf_split;
    TH1* m_vx_chi2Over_ndf_HS_matched;
    TH1* m_vx_chi2Over_ndf_HS_merged;
    TH1* m_vx_chi2Over_ndf_HS_split;

    TH1* m_vx_z0_skewness_matched;
    TH1* m_vx_z0_skewness_merged;
    TH1* m_vx_z0_skewness_split;
    TH1* m_vx_z0_skewness_HS_matched;
    TH1* m_vx_z0_skewness_HS_merged;
    TH1* m_vx_z0_skewness_HS_split;
    TH1* m_vx_z0_kurtosis_matched;
    TH1* m_vx_z0_kurtosis_merged;
    TH1* m_vx_z0_kurtosis_split;
    TH1* m_vx_z0_kurtosis_HS_matched;
    TH1* m_vx_z0_kurtosis_HS_merged;
    TH1* m_vx_z0_kurtosis_HS_split;

    // ALL Vertices
    TH1* m_vx_sumpT_ALL_matched;
    TH1* m_vx_sumpT_ALL_merged;
    TH1* m_vx_sumpT_ALL_split;
    
    TH1* m_vx_z_asym_ALL_matched;
    TH1* m_vx_z_asym_ALL_merged;
    TH1* m_vx_z_asym_ALL_split;
    TH1* m_vx_z_asym_weighted_ALL_matched;
    TH1* m_vx_z_asym_weighted_ALL_merged;
    TH1* m_vx_z_asym_weighted_ALL_split;
    TH1* m_vx_track_weight_ALL_matched;
    TH1* m_vx_track_weight_ALL_merged;
    TH1* m_vx_track_weight_ALL_split;
    TH1* m_vx_normalised_track_weight_ALL_matched;
    TH1* m_vx_normalised_track_weight_ALL_merged;
    TH1* m_vx_normalised_track_weight_ALL_split;

    TH1* m_vx_chi2Over_ndf_ALL_matched;
    TH1* m_vx_chi2Over_ndf_ALL_merged;
    TH1* m_vx_chi2Over_ndf_ALL_split;

    TH1* m_vx_z0_skewness_ALL_matched;
    TH1* m_vx_z0_skewness_ALL_merged;
    TH1* m_vx_z0_skewness_ALL_split;
    TH1* m_vx_z0_kurtosis_ALL_matched;
    TH1* m_vx_z0_kurtosis_ALL_merged;
    TH1* m_vx_z0_kurtosis_ALL_split;

    TH1* m_vx_nVertices_ALL_matched;
    TH1* m_vx_nVertices_ALL_merged;
    TH1* m_vx_nVertices_ALL_split;
    TH1* m_vx_nVertices_HS_matched;
    TH1* m_vx_nVertices_HS_merged;
    TH1* m_vx_nVertices_HS_split;
    TH1* m_vx_nVertices_matched;
    TH1* m_vx_nVertices_merged;
    TH1* m_vx_nVertices_split;


    ///@}
private:
    // plot base has no default implementation of this; we use it to book the histos
    void initializePlots();
    static const xAOD::Vertex* getHSRecoVertexSumPt2(const xAOD::VertexContainer& recoVertices) ;
    template<typename U, typename V>
    float getRadialDiff2(const U* vtx1, const V* vtx2) const;
    float getLocalPUDensity(const xAOD::TruthVertex* vtxOfInterest, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices, const float radialWindow = 2.0) const;
    static float getRecoLongitudinalReso(const xAOD::Vertex* recoVtx) ;
    static float getRecoTransverseReso(const xAOD::Vertex* recoVtx) ;
    const xAOD::TruthVertex* getTruthVertex(const xAOD::Vertex* recoVtx) const;
    static void fillResoHist(TH1* resoHist, const TH2* resoHist2D);
    void finalizePlots();
};

#endif
