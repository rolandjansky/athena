/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONLAYERSEGMENTMATCHINGTOOL_H
#define MUON_IMUONLAYERSEGMENTMATCHINGTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "MuonLayerEvent/MuonSystemExtension.h"


namespace Muon {

    class MuonSegment;

    /** Interface for tools calculating hit count summaries for track */
    class IMuonLayerSegmentMatchingTool : virtual public IAlgTool {
    public:
       virtual ~IMuonLayerSegmentMatchingTool() = default;
        /** IAlgTool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonLayerSegmentMatchingTool("Muon::IMuonLayerSegmentMatchingTool", 1, 0);
            return IID_IMuonLayerSegmentMatchingTool; 
        }

        /** @brief Match segments for to a given MuonSystemExtension::Intersection, return selected segments */
        virtual void select(const EventContext& ctx,
                            const MuonSystemExtension::Intersection& intersection,
                            const std::vector<std::shared_ptr<const Muon::MuonSegment> >& segments,
                            std::vector<std::shared_ptr<const Muon::MuonSegment> >& selectedSegments) const = 0;
    };
}  // namespace Muon

#endif  // IMuonLayerSegmentMatchingTool_H
