/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "PreselCaloExtensionBuilderAlg.h"

#include <memory>

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

namespace Trk {
    PreselCaloExtensionBuilderAlg::PreselCaloExtensionBuilderAlg(const std::string& name, ISvcLocator* pSvcLocator) :
        AthReentrantAlgorithm(name, pSvcLocator) {}

    PreselCaloExtensionBuilderAlg::~PreselCaloExtensionBuilderAlg() {}

    StatusCode PreselCaloExtensionBuilderAlg::initialize() {
        ATH_CHECK(m_particleCaloExtensionTool.retrieve());
        ATH_CHECK(m_inputTracksKey.initialize());
        ATH_CHECK(m_outputCacheKey.initialize());
        return StatusCode::SUCCESS;
    }

    StatusCode PreselCaloExtensionBuilderAlg::execute(const EventContext& ctx) const {
        auto tracks = SG::makeHandle(m_inputTracksKey, ctx);
        if (!tracks.isValid()) {
            ATH_MSG_ERROR("Failed to retrieve " << m_inputTracksKey);
            return StatusCode::FAILURE;
        }
        auto cache = std::make_unique<CaloExtensionCollection>();
        if (!tracks->empty())
            // Use the index of the last track as a good guess of the largest index
            // present in the container. This will be the right value if the view
            // has not been sorted differently from the original container
            cache->resize(tracks->back()->index() + 1);
        for (const xAOD::TrackParticle* itrk : *tracks) {
            if (itrk->index() >= cache->size())
                // Handle the case where the above assumption does not hold
                cache->resize(itrk->index() + 1);
            cache->at(itrk->index()) = m_particleCaloExtensionTool->caloExtension(ctx, *itrk);
        }
        auto outputHandle = SG::makeHandle(m_outputCacheKey, ctx);
        ATH_CHECK(outputHandle.record(std::move(cache)));
        return StatusCode::SUCCESS;
    }
}  // namespace Trk
