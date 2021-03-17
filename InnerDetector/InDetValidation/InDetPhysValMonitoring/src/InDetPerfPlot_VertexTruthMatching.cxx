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
    m_vx_hs_truth_y_pull_vs_nTrk(nullptr)

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

void InDetPerfPlot_VertexTruthMatching::fill(const xAOD::Vertex& vertex, const xAOD::TruthVertex * tvrt) {

    if(tvrt){
      float diff_z=vertex.z()-tvrt->z();
      const AmgSymMatrix(3)& covariance = vertex.covariancePosition();
      float err_z = fabs(Amg::error(covariance, 2)) > 1e-7 ? Amg::error(covariance, 2) : 1000.;
      fillHisto(m_vx_z_diff,diff_z);
      fillHisto(m_vx_z_diff_pull,diff_z/err_z);
    }

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


        } // end loop over vertices

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
        fillResoHist(m_vx_hs_truth_long_reso, m_vx_hs_truth_long_reso_vs_PU);
        fillResoHist(m_vx_hs_truth_trans_reso, m_vx_hs_truth_trans_reso_vs_PU);
    }

} // end InDetPerfPlot_VertexTruthMatching::finalizePlots()
