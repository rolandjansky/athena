/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRACKTOCALO_PRESELCALOEXTENSIONBUILDERALG_H
#define TRACKTOCALO_PRESELCALOEXTENSIONBUILDERALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace Trk {
    /**
     * @brief Build the calo extensions for a preselected collection of tracks
     * @author Jon Burr
     *
     * This algorithm is designed to work correctly with a view container. The
     * indices used in the CaloExtensionCollection use the track's index function
     * to key the correct extension (this is consistent with its use in the tool)
     * so the produced extension collection can also safely be used with the
     * original track collection from which the view was created.
     *
     * Note that this is not guaranteed to work if a view container is provided
     * that was produced from more than one container, as the indices of the
     * tracks are not guaranteed to be unique.
     */
    class PreselCaloExtensionBuilderAlg : public AthReentrantAlgorithm {
    public:
        PreselCaloExtensionBuilderAlg(const std::string& name, ISvcLocator* pSvcLocator);
        ~PreselCaloExtensionBuilderAlg();

        virtual StatusCode initialize() override;
        virtual StatusCode execute(const EventContext& ctx) const override;

    private:
        ToolHandle<Trk::IParticleCaloExtensionTool> m_particleCaloExtensionTool{this, "ParticleCaloExtensionTool",
                                                                                "ParticleCaloExtensionTool"};
        SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inputTracksKey{this, "InputTracks", "", "Input track container"};
        SG::WriteHandleKey<CaloExtensionCollection> m_outputCacheKey{this, "OutputCache", "", "Output extension cache"};
    };  //> end class PreselCaloExtensionBuilderAlg
}  // namespace Trk

#endif  //> !TRACKTOCALO_PRESELCALOEXTENSIONBUILDERALG_H
