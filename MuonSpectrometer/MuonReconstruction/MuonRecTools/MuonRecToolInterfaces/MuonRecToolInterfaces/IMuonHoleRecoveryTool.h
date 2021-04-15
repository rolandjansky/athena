/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONHOLERECOVERYTOOL_H
#define MUON_IMUONHOLERECOVERYTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"

namespace Trk {
    class Track;
}

namespace Muon {

    /** @brief The IMuonHoleRecoveryTool is a pure virtual interface for tools that recover hole on a track */
    class IMuonHoleRecoveryTool : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonHoleRecoveryTool("Muon::IMuonHoleRecoveryTool", 1, 0);
            return IID_IMuonHoleRecoveryTool;
        }

        /** @brief recover missing hits (holes) on tracks
            @return a pointer to a track, the ownership of the output track is passed to the client calling the tool.
        */
        virtual Trk::Track* recover(const Trk::Track& track, const EventContext& ctx) const = 0;
        /// For backward compability
        virtual Trk::Track* recover(const Trk::Track& track) const = 0;

        virtual void createHoleTSOSsForClusterChamber(const Identifier& detElId, const EventContext& ctx, const Trk::TrackParameters& pars,
                                                      std::set<Identifier>& layIds,
                                                      std::vector<std::pair<bool, const Trk::TrackStateOnSurface*> >& states) const = 0;

        /** virtual destructor **/
        virtual ~IMuonHoleRecoveryTool() = default;
    };

}  // namespace Muon

#endif
