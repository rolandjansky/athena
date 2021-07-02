/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTOVERLAPREMOVALTOOL_H
#define MUON_IMUONSEGMENTOVERLAPREMOVALTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "TrkSegment/SegmentCollection.h"

namespace Muon {

    class MuonSegment;
    class MuonSegmentCombination;

    /**
       @brief abstract interface for tools removing duplicates from segment collections and segment combinations

    */
    class IMuonSegmentOverlapRemovalTool : virtual public IAlgTool {
    public:
        /** @brief access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonSegmentOverlapRemovalTool("Muon::IMuonSegmentOverlapRemovalTool", 1, 0);
            return IID_IMuonSegmentOverlapRemovalTool;
        }

        /** @brief remove duplicates from a Trk::SegmentCollection*/
        virtual void removeDuplicates(Trk::SegmentCollection& segments) const = 0;

        /** @brief remove duplicates from a MuonSegmentCombination.
            The callers should take ownership of the new SegmentCombination. If the combination is
            unchanged the routine will return a nullptr */
        virtual std::unique_ptr<MuonSegmentCombination> removeDuplicates(const MuonSegmentCombination& combi) const = 0;
    };

}  // namespace Muon

#endif
