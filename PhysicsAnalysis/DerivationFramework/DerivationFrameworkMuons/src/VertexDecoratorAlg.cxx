/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMuons/VertexDecoratorAlg.h"
#include <InDetTrackSelectionTool/IInDetTrackSelectionTool.h>
#include <xAODTracking/VertexContainer.h>
#include <xAODTracking/TrackParticle.h>

VertexDecoratorAlg::VertexDecoratorAlg(const std::string& name, ISvcLocator* pSvcLocator) :
            AthAlgorithm(name, pSvcLocator),
            m_VertexContainer("PrimaryVertices"),
            m_decorationPrefix(""),
            m_trkselTool(""),
            m_filterTracks(true),
            m_dec_sum_pt(nullptr),
            m_dec_n_trks(nullptr) {
    declareProperty("VertxContainer", m_VertexContainer);
    declareProperty("Prefix" , m_decorationPrefix);
    declareProperty("TrackSelectionTool", m_trkselTool );
}

StatusCode VertexDecoratorAlg::initialize() {
    m_filterTracks = m_trkselTool.isSet();
    if (m_filterTracks) ATH_CHECK(m_trkselTool.retrieve());
    ATH_MSG_DEBUG("in initialize - retrieving tools");
    m_dec_sum_pt = std::make_unique< SG::AuxElement::Decorator<float>> (std::string("Trk_SumPt") + std::string(m_decorationPrefix.empty()? "" : "_") + m_decorationPrefix);
    m_dec_n_trks = std::make_unique< SG::AuxElement::Decorator<unsigned int>> (std::string("nTracks") + std::string(m_decorationPrefix.empty()? "" : "_") + m_decorationPrefix);
    return StatusCode::SUCCESS;
}


StatusCode VertexDecoratorAlg::execute() {

    ATH_MSG_DEBUG("in execute");
    const xAOD::VertexContainer* Vertices = nullptr;
    ATH_CHECK(evtStore()->retrieve(Vertices, m_VertexContainer));
    for (const auto& vertex  : *Vertices ){
        unsigned int n_Trks = 0;
        float sum_pt = 0;
        for (unsigned int t = 0 ; t < vertex->nTrackParticles(); ++t){
             const xAOD::TrackParticle* track = vertex->trackParticle(t);
             if (!track){
                ATH_MSG_DEBUG("Track "<<t<<" is a nullptr");
                continue;
            }
            if (!m_filterTracks ||  m_trkselTool->accept(*track, vertex)){
                ++n_Trks;
                sum_pt += track->pt();
            }
        }
        m_dec_sum_pt->operator()(*vertex) = sum_pt;
        m_dec_n_trks->operator()(*vertex) = n_Trks;
    }
    return StatusCode::SUCCESS;
}


