/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSYSTEMEXTENSIONTOOL_H
#define MUON_IMUONSYSTEMEXTENSIONTOOL_H

/**
    @class IMuonSystemExtensionTool

*/

#include <map>
#include <optional>
#include <set>

#include "GaudiKernel/IAlgTool.h"
#include "MuonStationIndex/MuonStationIndex.h"
#include "xAODTracking/TrackParticle.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"
#include "MuonLayerEvent/MuonSystemExtension.h"

namespace MuonCombined {
    class InDetCandidate;
}
namespace Muon {
   
    class IMuonSystemExtensionTool : virtual public IAlgTool {
    public:
        /**Virtual destructor*/
        virtual ~IMuonSystemExtensionTool() = default;

        /** AlgTool and IAlgTool interface methods */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonSystemExtensionTool("Muon::IMuonSystemExtensionTool", 1, 0);
            return IID_IMuonSystemExtensionTool;
        }
        /// Helper struct to pipe all data needed by the tool 
        /// to equip the Id track with a MuonSystemExtension
        struct SystemExtensionCache {
            
            /// Inner detector candidate
            std::unique_ptr<MuonCombined::InDetCandidate> candidate;
            /// Cache the sectors which have a recorded hit. Divided into
            ///     Barrel / EndcapA  / EndcapC
            const std::map<MuonStationIndex::DetectorRegionIndex, std::set<int>>* sectorsWithHits{nullptr};
            /// Cache of the CaloExtensions
            const CaloExtensionCollection* extensionContainer{nullptr};
            /// Switch to restrict the intersection search only to
            /// the sectors with hits
            bool useHitSectors{false};            
            /// Try to create the muon system extension
            bool createSystemExtension{true};
            /// Require that the muon system extension was successful
            bool requireSystemExtension{false};
        };

        /** get muon system extension */
        virtual bool muonSystemExtension(const EventContext& ctx, SystemExtensionCache& cache) const = 0;

        /** Constructs the next intersection of the track parameters with the MS **/
        virtual MuonSystemExtension::Intersection getInterSection(const EventContext& ctx, const Trk::TrackParameters& muon_pars) const = 0;
    };

}  // namespace Muon

#endif
