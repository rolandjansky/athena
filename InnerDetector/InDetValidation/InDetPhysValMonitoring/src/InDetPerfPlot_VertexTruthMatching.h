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
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

///class holding plots for truth matched vertices
class InDetPerfPlot_VertexTruthMatching: public InDetPlotBase {
public:
    InDetPerfPlot_VertexTruthMatching(InDetPlotBase* pParent, const std::string& dirName, const int iDetailLevel = 10);
    void fill(const xAOD::Vertex& vertex);
    void fill(const xAOD::VertexContainer& vertexContainer, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices);
private:
    int m_iDetailLevel;
    float m_cutMinTruthRecoRadialDiff = 0.1;
    ///truth type
    TH1* m_vx_type_truth;
    ///hardscatter classification
    TH1* m_vx_hs_classification;
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
    ///@}
private:
    // plot base has no default implementation of this; we use it to book the histos
    void initializePlots();
    const xAOD::Vertex* getHSRecoVertexSumPt2(const xAOD::VertexContainer& recoVertices) const;
    template<typename U, typename V>
    float getRadialDiff2(const U* vtx1, const V* vtx2) const;
    float getLocalPUDensity(const xAOD::TruthVertex* vtxOfInterest, const std::vector<const xAOD::TruthVertex*>& truthHSVertices, const std::vector<const xAOD::TruthVertex*>& truthPUVertices, const float radialWindow = 2.0) const;
    float getRecoLongitudinalReso(const xAOD::Vertex* recoVtx) const;
    float getRecoTransverseReso(const xAOD::Vertex* recoVtx) const;
    const xAOD::TruthVertex* getTruthVertex(const xAOD::Vertex* recoVtx) const;
    void fillResoHist(TH1* resoHist, const TH2* resoHist2D);
    void finalizePlots();
};

#endif
