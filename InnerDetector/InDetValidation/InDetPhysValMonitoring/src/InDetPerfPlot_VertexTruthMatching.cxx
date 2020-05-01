/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_VertexTruthMatching.cxx
 * @author Valentina Cairo, mbasso
 **/

#include "InDetPerfPlot_VertexTruthMatching.h"

using namespace IDPVM;

InDetPerfPlot_VertexTruthMatching::InDetPerfPlot_VertexTruthMatching(InDetPlotBase* pParent, const std::string& sDir) :
    InDetPlotBase(pParent, sDir),
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
    m_vx_nReco_vs_nTruth_none(nullptr)
{
  // nop
}

void
InDetPerfPlot_VertexTruthMatching::initializePlots() {

    IDPVM_BOOK(m_vx_type_truth);
    IDPVM_BOOK(m_vx_hs_classification);
    IDPVM_BOOK(m_vx_nReco_vs_nTruth_inclusive);
    IDPVM_BOOK(m_vx_nReco_vs_nTruth_matched);
    IDPVM_BOOK(m_vx_nReco_vs_nTruth_merged);
    IDPVM_BOOK(m_vx_nReco_vs_nTruth_split);
    IDPVM_BOOK(m_vx_nReco_vs_nTruth_fake);
    IDPVM_BOOK(m_vx_nReco_vs_nTruth_dummy);
    IDPVM_BOOK(m_vx_nReco_vs_nTruth_clean);
    IDPVM_BOOK(m_vx_nReco_vs_nTruth_lowpu);
    IDPVM_BOOK(m_vx_nReco_vs_nTruth_highpu);
    IDPVM_BOOK(m_vx_nReco_vs_nTruth_hssplit);
    IDPVM_BOOK(m_vx_nReco_vs_nTruth_none);

}

void 
InDetPerfPlot_VertexTruthMatching::fill(const xAOD::Vertex& vertex) {

    // fill vertex truth match type
    const static xAOD::Vertex::Decorator<InDetVertexTruthMatchUtils::VertexMatchType> recoVtxMatchTypeInfo("VertexMatchType");
    if (recoVtxMatchTypeInfo.isAvailable(vertex)) {
        try {
            ATH_MSG_DEBUG("VERTEX DECORATOR ======= "<< recoVtxMatchTypeInfo(vertex)<< ", with nTRACKS === " << vertex.nTrackParticles() << ", vertex index = " << vertex.index()<< " AT (" << vertex.x() << ", " << vertex.y() << ", " << vertex.z() << ")");
            fillHisto(m_vx_type_truth, recoVtxMatchTypeInfo(vertex));
        }
        catch (SG::ExcBadAuxVar &) {
            ATH_MSG_WARNING("VertexMatchType DECORATOR seems to be available, but may be broken  ===========");
        } 
    }
    else {
        ATH_MSG_WARNING("VertexMatchType DECORATOR is NOT available  ===========");
    } 

} //end InDetPerfPlot_VertexTruthMatching::fill(const xAOD::Vertex& vertex)

void 
InDetPerfPlot_VertexTruthMatching::fill(const xAOD::VertexContainer& vertexContainer, const std::vector<const xAOD::TruthVertex*>& truthVertices) {

    // Fill our histogram
    // We just need to add the size of the vertex container as an argument, are there other things we might need to fill in this kind of function?
    // Inclusive:
    fillHisto(m_vx_nReco_vs_nTruth_inclusive, (float)truthVertices.size(), (float)vertexContainer.size());
    
    // Let's also plot the vertices by vertex match type:
    const static xAOD::Vertex::Decorator<InDetVertexTruthMatchUtils::VertexMatchType> recoVtxMatchTypeInfo("VertexMatchType");
    std::map<InDetVertexTruthMatchUtils::VertexMatchType, int> breakdown = {};
    breakdown[InDetVertexTruthMatchUtils::VertexMatchType::MATCHED] = 0;
    breakdown[InDetVertexTruthMatchUtils::VertexMatchType::MERGED]  = 0;
    breakdown[InDetVertexTruthMatchUtils::VertexMatchType::SPLIT]   = 0;
    breakdown[InDetVertexTruthMatchUtils::VertexMatchType::FAKE]    = 0;
    breakdown[InDetVertexTruthMatchUtils::VertexMatchType::DUMMY]   = 0;
    for (const auto& vertex : vertexContainer.stdcont()) {
        if (recoVtxMatchTypeInfo.isAvailable(*vertex)) {
            breakdown[recoVtxMatchTypeInfo(*vertex)] += 1;
        }
    }
    fillHisto(m_vx_nReco_vs_nTruth_matched, (float)truthVertices.size(), (float)breakdown[InDetVertexTruthMatchUtils::VertexMatchType::MATCHED]);
    fillHisto(m_vx_nReco_vs_nTruth_merged,  (float)truthVertices.size(), (float)breakdown[InDetVertexTruthMatchUtils::VertexMatchType::MERGED]);
    fillHisto(m_vx_nReco_vs_nTruth_split,   (float)truthVertices.size(), (float)breakdown[InDetVertexTruthMatchUtils::VertexMatchType::SPLIT]);
    fillHisto(m_vx_nReco_vs_nTruth_fake,    (float)truthVertices.size(), (float)breakdown[InDetVertexTruthMatchUtils::VertexMatchType::FAKE]);
    fillHisto(m_vx_nReco_vs_nTruth_dummy,   (float)truthVertices.size(), (float)breakdown[InDetVertexTruthMatchUtils::VertexMatchType::DUMMY]);
    
    // And by hardscatter type:
    InDetVertexTruthMatchUtils::HardScatterType hsType = InDetVertexTruthMatchUtils::classifyHardScatter(vertexContainer);
    fillHisto(m_vx_hs_classification, hsType);
    switch(hsType) {
        case InDetVertexTruthMatchUtils::HardScatterType::CLEAN: {
            fillHisto(m_vx_nReco_vs_nTruth_clean, (float)truthVertices.size(), (float)vertexContainer.size());
            break;
        }
        case InDetVertexTruthMatchUtils::HardScatterType::LOWPU: {
            fillHisto(m_vx_nReco_vs_nTruth_lowpu, (float)truthVertices.size(), (float)vertexContainer.size());
            break;
        }
        case InDetVertexTruthMatchUtils::HardScatterType::HIGHPU: {
            fillHisto(m_vx_nReco_vs_nTruth_highpu, (float)truthVertices.size(), (float)vertexContainer.size());
            break;
        }
        case InDetVertexTruthMatchUtils::HardScatterType::HSSPLIT: {
            fillHisto(m_vx_nReco_vs_nTruth_hssplit, (float)truthVertices.size(), (float)vertexContainer.size());
            break;
        }
        case InDetVertexTruthMatchUtils::HardScatterType::NONE: {
            fillHisto(m_vx_nReco_vs_nTruth_none, (float)truthVertices.size(), (float)vertexContainer.size());
            break;
        }
        default: {
            break;
        }
    }

}//end InDetPerfPlot_VertexTruthMatching::fill(const xAOD::VertexContainer& vertexContainer, const xAOD::TruthEventContainer& truthHSEvents)
