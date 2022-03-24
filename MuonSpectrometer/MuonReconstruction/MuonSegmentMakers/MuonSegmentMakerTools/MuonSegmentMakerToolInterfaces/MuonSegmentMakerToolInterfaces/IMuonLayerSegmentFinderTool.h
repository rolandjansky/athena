/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONLAYERSEGMENTFINDERTOOL_H
#define MUON_IMUONLAYERSEGMENTFINDERTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "MuonLayerEvent/MuonLayerPrepRawData.h"
#include "MuonLayerEvent/MuonSystemExtension.h"

namespace Muon {

    class MuonSegment;

    /** Interface for tools calculating hit count summaries for track */
    class IMuonLayerSegmentFinderTool : virtual public IAlgTool {
    public:
        /** IAlgTool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonLayerSegmentFinderTool("Muon::IMuonLayerSegmentFinderTool", 1, 0);
            return IID_IMuonLayerSegmentFinderTool;
        }

        /** @brief Find segments for a given MuonSystemExtension::Intersection */
        virtual void find(const EventContext& ctx,
                  const MuonSystemExtension::Intersection& intersection, 
                  const  MuonLayerPrepRawData& layerPrepRawData,
                  std::vector<std::shared_ptr<const Muon::MuonSegment> >& segments) const = 0;

        virtual ~IMuonLayerSegmentFinderTool() = default;
    };

}  // namespace Muon

#endif  // IMuonLayerSegmentFinderTool_H
